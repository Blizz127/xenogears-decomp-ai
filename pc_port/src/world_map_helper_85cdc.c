/*
 * World-map actor billboard submitter 0x80085CDC.
 *
 * Register-faithful transcription of retail world_map.bin
 * [0x80085CDC, 0x80085F58).  See world_map_helper_85cdc.h.
 *
 * Three 64-slot walks over *0x8009BE24, stride 0x80:
 *   1. If lh(+0x24)==0 and lw(+0x4C)!=0: wrap X/Z vs 0x8009BE28
 *      through wm_80093484, write (X<<4, Y<<4, -Z<<4) into the sprite
 *      VECTOR at +0x4C.
 *   2. SetRot/SetTrans(0x8009C808). Same gate: load sprite +2/+6/+0xA
 *      as SVECTOR, RTPS, swc2 SZ3 to scratch 0x18+i*4.
 *   3. func_80024FF4(0x8009C808). Same gate and SZ3 < 0xB00:
 *      func_8001E298(sprite, OT+(SZ3>>4)*4), 12-bit angle chase
 *      stored at +0x5C, func_800223B0(sprite, (ang-camY-0x400)&0xFFF),
 *      AnimScriptTick(sprite).
 */
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_85cdc.h"
#include "world_map_helper_93484.h"

#define WCDC_POOL         0x8009BE24u
#define WCDC_POS          0x8009BE28u
#define WCDC_CAMERA       0x8009C808u
#define WCDC_DB           0x8009BE3Cu
#define WCDC_ANGLE        0x8009BD3Au
#define WCDC_SCRATCH      0x1F800000u
#define WCDC_SLOTS        0x40u
#define WCDC_STRIDE       0x80u
#define WCDC_SZ_LIMIT     0xB00
#define WCDC_HALF         0x801
#define WCDC_SNAP         0x100
#define WCDC_WRAP         0x1000
#define WCDC_CAM_BIAS     0x400
#define WCDC_ANGLE_MASK   0xFFFu

typedef struct {
    s16 m[3][3];
    s32 t[3];
} Wm85cdcMatrix;

typedef struct {
    s16 vx;
    s16 vy;
    s16 vz;
    s16 pad;
} Wm85cdcSvector;

extern void SetRotMatrix(Wm85cdcMatrix *m);
extern void SetTransMatrix(Wm85cdcMatrix *m);
extern void func_80024FF4(Wm85cdcMatrix *matrix);
extern void func_8001E298(void *sprite, void *ot);
extern void func_800223B0(void *sprite, s16 angle);
extern void AnimScriptTick(void *sprite);

#if defined(WM_85CDC_TEST_TRACE)
extern u32 wm_85cdc_test_rtps_sz3(u32 svec_addr);
extern void wm_85cdc_test_store(u32 address, u32 value);
#define WCDC_RTPS_SZ3(sv) wm_85cdc_test_rtps_sz3(sv)
#define WCDC_TRACE_STORE(a, v) wm_85cdc_test_store((a), (v))
#else
#define WCDC_TRACE_STORE(a, v) ((void)0)
#endif

static u32 wcdc_lw(u32 a)
{
    u32 v;

    memcpy(&v, PSX_ADDR(a), 4);
    return v;
}

static s32 wcdc_lh(u32 a)
{
    s16 h;

    memcpy(&h, PSX_ADDR(a), 2);
    return (s32)h;
}

static u32 wcdc_lhu(u32 a)
{
    u16 h;

    memcpy(&h, PSX_ADDR(a), 2);
    return (u32)h;
}

static void wcdc_sh(u32 a, u32 v)
{
    u16 h = (u16)(v & 0xFFFFu);

    memcpy(PSX_ADDR(a), &h, 2);
}

static void wcdc_sw(u32 a, u32 v)
{
    memcpy(PSX_ADDR(a), &v, 4);
    WCDC_TRACE_STORE(a, v);
}

static s32 wcdc_bits_as_s32(u32 value)
{
    s32 result;

    memcpy(&result, &value, sizeof(result));
    return result;
}

static u32 wcdc_s32_as_bits(s32 value)
{
    u32 result;

    memcpy(&result, &value, sizeof(result));
    return result;
}

static s32 wcdc_sra(s32 value, u32 shift)
{
    u32 amount = shift & 31u;
    u32 bits = wcdc_s32_as_bits(value);
    u32 result;

    if (amount == 0u)
        return value;
    result = bits >> amount;
    if (value < 0)
        result |= UINT32_MAX << (32u - amount);
    return wcdc_bits_as_s32(result);
}

#if !defined(WM_85CDC_TEST_TRACE)
#include <psx/inline_c.h>

static u32 wcdc_rtps_sz3(u32 svec_addr)
{
    u32 sz = 0u;

    gte_ldv0(PSX_ADDR(svec_addr));
    gte_rtps();
    gte_stsz(&sz);
    return sz;
}

#define WCDC_RTPS_SZ3(sv) wcdc_rtps_sz3(sv)
#endif

static u32 wcdc_chase_angle(s32 target, u32 current_bits)
{
    u32 v1_bits = wcdc_s32_as_bits(target) - current_bits;
    s32 v1 = wcdc_bits_as_s32(v1_bits);
    s32 current = wcdc_bits_as_s32(current_bits);
    s32 ok_half;

    ok_half = (v1 < WCDC_HALF) ? 1 : 0;
    if (v1 < 0) {
        v1_bits = v1_bits + (u32)WCDC_WRAP;
        v1 = wcdc_bits_as_s32(v1_bits);
        ok_half = (v1 < WCDC_HALF) ? 1 : 0;
    }
    if (ok_half != 0) {
        if (v1 < WCDC_SNAP)
            return wcdc_s32_as_bits(target);
        return wcdc_s32_as_bits(current + WCDC_SNAP);
    }
    v1_bits = v1_bits - (u32)WCDC_WRAP;
    v1 = wcdc_bits_as_s32(v1_bits);
    if (v1 < -0xFF)
        return wcdc_s32_as_bits(current - WCDC_SNAP);
    return wcdc_s32_as_bits(target);
}

void wm_80085CDC(void)
{
    u32 pool = wcdc_lw(WCDC_POOL);
    u32 rec;
    u32 i;
    u32 scratch = WCDC_SCRATCH;

#if defined(WM_85CDC_MUTANT_WRONG_COUNT)
    const u32 nslots = 2u;
#else
    const u32 nslots = WCDC_SLOTS;
#endif

    rec = pool + 0x2Cu;
    for (i = 0u; i < nslots; i++) {
        s32 flag = wcdc_lh(rec - 8u);
        u32 sprite = wcdc_lw(rec + 0x20u);

        if (flag == 0 && sprite != 0u) {
#if defined(WM_85CDC_MUTANT_SKIP_POS_SUB)
            u32 dx = wcdc_lw(rec - 4u);
            u32 dz = wcdc_lw(rec + 4u);
#else
            u32 dx = wcdc_lw(rec - 4u) - wcdc_lw(WCDC_POS);
            u32 dz = wcdc_lw(rec + 4u) - wcdc_lw(WCDC_POS + 8u);
#endif

            wcdc_sw(scratch + 8u, dx);
            wcdc_sw(scratch + 0x10u, dz);
#if !defined(WM_85CDC_MUTANT_SKIP_WRAP_CALL)
            wm_80093484(scratch + 8u);
#endif
#if defined(WM_85CDC_MUTANT_SKIP_X)
            (void)0;
#else
            wcdc_sw(sprite, wcdc_lw(scratch + 8u) << 4);
#endif
#if defined(WM_85CDC_MUTANT_SKIP_NEG_Z)
            wcdc_sw(sprite + 8u, wcdc_lw(scratch + 0x10u) << 4);
#else
            wcdc_sw(sprite + 8u, (0u - wcdc_lw(scratch + 0x10u)) << 4);
#endif
#if !defined(WM_85CDC_MUTANT_SKIP_Y)
            wcdc_sw(sprite + 4u, wcdc_lw(rec) << 4);
#endif
        }
        rec += WCDC_STRIDE;
    }

#if !defined(WM_85CDC_MUTANT_SKIP_SETROT)
    SetRotMatrix((Wm85cdcMatrix *)PSX_ADDR(WCDC_CAMERA));
#endif
#if !defined(WM_85CDC_MUTANT_SKIP_SETTRANS)
    SetTransMatrix((Wm85cdcMatrix *)PSX_ADDR(WCDC_CAMERA));
#endif

    rec = pool + 0x4Cu;
    for (i = 0u; i < nslots; i++) {
        s32 flag = wcdc_lh(rec - 0x28u);
        u32 sprite = wcdc_lw(rec);

        if (flag == 0 && sprite != 0u) {
            wcdc_sh(scratch, (u32)wcdc_lh(sprite + 2u));
            wcdc_sh(scratch + 2u, (u32)wcdc_lh(sprite + 6u));
            wcdc_sh(scratch + 4u, (u32)wcdc_lh(sprite + 0xAu));
#if !defined(WM_85CDC_MUTANT_SKIP_RTPS)
            wcdc_sw(scratch + 0x18u + i * 4u, WCDC_RTPS_SZ3(scratch));
#endif
        }
        rec += WCDC_STRIDE;
    }

#if !defined(WM_85CDC_MUTANT_SKIP_24FF4)
    func_80024FF4((Wm85cdcMatrix *)PSX_ADDR(WCDC_CAMERA));
#endif

    rec = pool + 0x4Cu;
    for (i = 0u; i < nslots; i++) {
        s32 flag = wcdc_lh(rec - 0x28u);
        u32 sprite = wcdc_lw(rec);
        s32 sz = wcdc_bits_as_s32(wcdc_lw(scratch + 0x18u));
#if defined(WM_85CDC_MUTANT_WRONG_LIMIT)
        s32 limit = 0x800;
#else
        s32 limit = WCDC_SZ_LIMIT;
#endif

        if (flag == 0 && sprite != 0u && sz < limit) {
            s32 shifted;
            u32 ot;
            u32 db;
            u32 chased;
            u32 view;

#if defined(WM_85CDC_MUTANT_WRONG_SHIFT)
            shifted = wcdc_sra(sz, 2u);
#else
            shifted = wcdc_sra(sz, 4u);
#endif
            db = wcdc_lw(WCDC_DB);
            ot = wcdc_lw(db + 0x70u) + ((u32)shifted << 2);
#if !defined(WM_85CDC_MUTANT_SKIP_SUBMIT)
            func_8001E298(PSX_ADDR(sprite), PSX_ADDR(ot));
#else
            (void)ot;
#endif
            chased = wcdc_chase_angle(wcdc_lh(rec - 4u), wcdc_lw(rec + 0x10u));
#if !defined(WM_85CDC_MUTANT_SKIP_CHASE)
            wcdc_sw(rec + 0x10u, chased);
#endif
#if defined(WM_85CDC_MUTANT_WRONG_CAM)
            view = wcdc_s32_as_bits(wcdc_bits_as_s32(chased) -
                                    (s32)wcdc_lhu(WCDC_ANGLE));
#else
            view = wcdc_s32_as_bits(
                wcdc_bits_as_s32(chased) - (s32)wcdc_lhu(WCDC_ANGLE) -
                WCDC_CAM_BIAS);
#endif
            view &= WCDC_ANGLE_MASK;
#if !defined(WM_85CDC_MUTANT_SKIP_223B0)
            func_800223B0(PSX_ADDR(sprite), (s16)view);
#else
            (void)view;
#endif
#if !defined(WM_85CDC_MUTANT_SKIP_TICK)
            AnimScriptTick(PSX_ADDR(sprite));
#endif
        }
        scratch += 4u;
        rec += WCDC_STRIDE;
    }
}
