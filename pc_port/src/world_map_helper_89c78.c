/*
 * World-map POLY_FT4 billboard submitter 0x80089C78.
 *
 * Register-faithful transcription of retail world_map.bin
 * [0x80089C78, 0x8008A2C8).  See world_map_helper_89c78.h.
 *
 * 256 slots, cursor *0x8009BDF4+6, stride 0x4C. Enable is lh(cursor).
 * Packet pool * (0x8009BE1C + index*4). Template MATRIX 0x8009A180.
 */
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_89c78.h"
#include "world_map_helper_93534.h"

#define W9C_CAMERA        0x8009C808u
#define W9C_TEMPLATE      0x8009A180u
#define W9C_VERTS         0x8009B040u
#define W9C_UV            0x8009AFF0u
#define W9C_POS_X         0x8009BE28u
#define W9C_POS_Z         0x8009BE30u
#define W9C_INDEX         0x8009D7F0u
#define W9C_PKT_TABLE     0x8009BE1Cu
#define W9C_REC_BASE      0x8009BDF4u
#define W9C_DB            0x8009BE3Cu
#define W9C_SC            0x1F800000u
#define W9C_SC_SVEC       0x1F800020u
#define W9C_SC_CAM        0x1F800028u
#define W9C_SC_MTX        0x1F800048u
#define W9C_SC_TMPL       0x1F800068u
#define W9C_SC_WRAP       0x1F800088u
#define W9C_SC_SCALE      0x1F800098u
#define W9C_SC_FLAG       0x1F8000ACu
#define W9C_SC_SZ         0x1F8000B0u
#define W9C_COUNT         0x100u
#define W9C_STRIDE        0x4Cu
#define W9C_PKT_STRIDE    0x28u
#define W9C_X_LIMIT       0x140
#define W9C_Y_LIMIT       0xD8
#define W9C_SZ_LIMIT      0xC00
#define W9C_OT_HI         0xFF000000u
#define W9C_OT_LO         0x00FFFFFFu

typedef struct {
    s16 vx;
    s16 vy;
    s16 vz;
    s16 pad;
} Wm9cSvector;

typedef struct {
    s32 vx;
    s32 vy;
    s32 vz;
    s32 pad;
} Wm9cVector;

typedef struct {
    s16 m[3][3];
    s32 t[3];
} Wm9cMatrix;

extern Wm9cMatrix *RotMatrixZ(s32 r, Wm9cMatrix *m);
extern Wm9cMatrix *ScaleMatrix(Wm9cMatrix *m, Wm9cVector *v);
extern Wm9cVector *ApplyMatrix(Wm9cMatrix *m, Wm9cSvector *v0, Wm9cVector *v1);
extern void SetRotMatrix(Wm9cMatrix *m);
extern void SetTransMatrix(Wm9cMatrix *m);

#if defined(WM_89C78_TEST_TRACE)
extern u32 wm_89c78_test_rtpt(u32 v0, u32 v1, u32 v2);
extern u32 wm_89c78_test_rtps(u32 v3, u32 *sz_out);
extern void wm_89c78_test_write_sxy3(u32 sxy0, u32 sxy1, u32 sxy2);
extern void wm_89c78_test_write_sxy(u32 sxy3);
extern void wm_89c78_test_store(u32 address, u32 value);
#define W9C_TRACE_STORE(a, v) wm_89c78_test_store((a), (v))
#else
#define W9C_TRACE_STORE(a, v) ((void)0)
#endif

static u32 w9c_lw(u32 a)
{
    u32 v;

    memcpy(&v, PSX_ADDR(a), 4);
    return v;
}

static s32 w9c_lh(u32 a)
{
    s16 h;

    memcpy(&h, PSX_ADDR(a), 2);
    return (s32)h;
}

static u32 w9c_lhu(u32 a)
{
    u16 h;

    memcpy(&h, PSX_ADDR(a), 2);
    return (u32)h;
}

static u32 w9c_lbu(u32 a)
{
    u8 b;

    memcpy(&b, PSX_ADDR(a), 1);
    return (u32)b;
}

static void w9c_sh(u32 a, u32 v)
{
    u16 h = (u16)(v & 0xFFFFu);

    memcpy(PSX_ADDR(a), &h, 2);
}

static void __attribute__((unused)) w9c_sb(u32 a, u32 v)
{
    u8 b = (u8)(v & 0xFFu);

    memcpy(PSX_ADDR(a), &b, 1);
}

static void w9c_sw(u32 a, u32 v)
{
    memcpy(PSX_ADDR(a), &v, 4);
    W9C_TRACE_STORE(a, v);
}

static s32 w9c_bits_as_s32(u32 value)
{
    s32 result;

    memcpy(&result, &value, sizeof(result));
    return result;
}

static u32 w9c_s32_as_bits(s32 value)
{
    u32 result;

    memcpy(&result, &value, sizeof(result));
    return result;
}

static s32 w9c_sra(s32 value, u32 shift)
{
    u32 amount = shift & 31u;
    u32 bits = w9c_s32_as_bits(value);
    u32 result;

    if (amount == 0u)
        return value;
    result = bits >> amount;
    if (value < 0)
        result |= UINT32_MAX << (32u - amount);
    return w9c_bits_as_s32(result);
}

static void w9c_copy32(u32 dst, u32 src)
{
    u32 i;

    for (i = 0u; i < 8u; i++)
        w9c_sw(dst + i * 4u, w9c_lw(src + i * 4u));
}

#if !defined(WM_89C78_TEST_TRACE)
#include <psx/inline_c.h>

static u32 w9c_rtpt_flag(u32 v0, u32 v1, u32 v2)
{
    u32 flag = 0u;

    gte_ldv0(PSX_ADDR(v0));
    gte_ldv1(PSX_ADDR(v1));
    gte_ldv2(PSX_ADDR(v2));
    gte_rtpt();
    gte_stflg(&flag);
    return flag;
}

static void w9c_rtpt_sxy(u32 sxy0, u32 sxy1, u32 sxy2)
{
    gte_stsxy3(PSX_ADDR(sxy0), PSX_ADDR(sxy1), PSX_ADDR(sxy2));
}

static u32 w9c_rtps_flag(u32 v3, u32 *sz_out)
{
    u32 flag = 0u;
    u32 sz = 0u;

    gte_ldv0(PSX_ADDR(v3));
    gte_rtps();
    gte_stflg(&flag);
    gte_stsz(&sz);
    *sz_out = sz;
    return flag;
}

static void w9c_rtps_sxy(u32 sxy3)
{
    gte_stsxy(PSX_ADDR(sxy3));
}
#endif

void wm_80089C78(void)
{
    s32 pos_x;
    s32 pos_z;
    u32 packet;
    u32 slot;
    u32 i;

    w9c_copy32(W9C_SC_CAM, W9C_CAMERA);
    w9c_copy32(W9C_SC_TMPL, W9C_TEMPLATE);

#if defined(WM_89C78_MUTANT_WRONG_SRA)
    pos_x = w9c_sra(w9c_bits_as_s32(w9c_lw(W9C_POS_X)), 8u);
    pos_z = w9c_sra(w9c_bits_as_s32(w9c_lw(W9C_POS_Z)), 8u);
#else
    pos_x = w9c_sra(w9c_bits_as_s32(w9c_lw(W9C_POS_X)), 12u);
    pos_z = w9c_sra(w9c_bits_as_s32(w9c_lw(W9C_POS_Z)), 12u);
#endif
    packet = w9c_lw(W9C_PKT_TABLE + (w9c_lw(W9C_INDEX) << 2));
    slot = w9c_lw(W9C_REC_BASE) + 6u;

#if defined(WM_89C78_MUTANT_WRONG_COUNT)
    for (i = 0u; i < 0x80u; i++, slot += W9C_STRIDE)
#else
    for (i = 0u; i < W9C_COUNT; i++, slot += W9C_STRIDE)
#endif
    {
        s32 idx;
        u32 flag;
        u32 sz;

#if defined(WM_89C78_MUTANT_SKIP_ENABLE)
        (void)0;
#else
        if (w9c_lh(slot) == 0)
            continue;
#endif
        w9c_sw(W9C_SC_SCALE, w9c_lhu(slot + 0x32u));
        w9c_sw(W9C_SC_SCALE + 8u, 0x1000u);
        w9c_sw(W9C_SC_SCALE + 4u, w9c_lhu(slot + 0x34u));
        w9c_copy32(W9C_SC_MTX, W9C_SC_TMPL);
#if defined(WM_89C78_MUTANT_FORCE_ROTZ)
        (void)RotMatrixZ(w9c_lh(slot - 4u), (Wm9cMatrix *)PSX_ADDR(W9C_SC_MTX));
#elif !defined(WM_89C78_MUTANT_SKIP_ROTZ)
        if ((w9c_lbu(slot + 0x41u) & 1u) != 0u)
            (void)RotMatrixZ(w9c_lh(slot - 4u),
                             (Wm9cMatrix *)PSX_ADDR(W9C_SC_MTX));
#endif
#if !defined(WM_89C78_MUTANT_SKIP_SCALE)
        (void)ScaleMatrix((Wm9cMatrix *)PSX_ADDR(W9C_SC_MTX),
                          (Wm9cVector *)PSX_ADDR(W9C_SC_SCALE));
#endif

        idx = w9c_lh(slot);
        w9c_copy32(W9C_SC, W9C_VERTS + ((u32)idx << 5));

        w9c_sw(W9C_SC_WRAP,
               w9c_s32_as_bits(w9c_sra(w9c_bits_as_s32(w9c_lw(slot + 2u)), 12u) -
                               pos_x));
        w9c_sw(W9C_SC_WRAP + 8u,
               w9c_s32_as_bits(w9c_sra(w9c_bits_as_s32(w9c_lw(slot + 0xAu)), 12u) -
                               pos_z));
#if !defined(WM_89C78_MUTANT_SKIP_WRAP)
        wm_80093534(W9C_SC_WRAP);
#endif
        w9c_sh(W9C_SC_SVEC, w9c_lw(W9C_SC_WRAP));
        w9c_sh(W9C_SC_SVEC + 4u, 0u - w9c_lw(W9C_SC_WRAP + 8u));
        w9c_sh(W9C_SC_SVEC + 2u,
               (u32)w9c_s32_as_bits(
                   w9c_sra(w9c_bits_as_s32(w9c_lw(slot + 6u)), 12u)));

#if !defined(WM_89C78_MUTANT_SKIP_APPLY)
        SetRotMatrix((Wm9cMatrix *)PSX_ADDR(W9C_SC_CAM));
        (void)ApplyMatrix((Wm9cMatrix *)PSX_ADDR(W9C_SC_CAM),
                          (Wm9cSvector *)PSX_ADDR(W9C_SC_SVEC),
                          (Wm9cVector *)PSX_ADDR(W9C_SC_WRAP));
        w9c_sw(W9C_SC_MTX + 0x14u,
               w9c_lw(W9C_SC_WRAP) + w9c_lw(W9C_SC_CAM + 0x14u));
        w9c_sw(W9C_SC_MTX + 0x18u,
               w9c_lw(W9C_SC_WRAP + 4u) + w9c_lw(W9C_SC_CAM + 0x18u));
        w9c_sw(W9C_SC_MTX + 0x1Cu,
               w9c_lw(W9C_SC_WRAP + 8u) + w9c_lw(W9C_SC_CAM + 0x1Cu));
#endif

        SetRotMatrix((Wm9cMatrix *)PSX_ADDR(W9C_SC_MTX));
        SetTransMatrix((Wm9cMatrix *)PSX_ADDR(W9C_SC_MTX));

#if defined(WM_89C78_TEST_TRACE)
        flag = wm_89c78_test_rtpt(W9C_SC, W9C_SC + 8u, W9C_SC + 0x10u);
#else
        flag = w9c_rtpt_flag(W9C_SC, W9C_SC + 8u, W9C_SC + 0x10u);
#endif
        w9c_sw(W9C_SC_FLAG, flag);
#if defined(WM_89C78_MUTANT_SKIP_FLAG)
        flag = 0u;
#endif
        if (w9c_bits_as_s32(flag) < 0)
            continue;
#if defined(WM_89C78_TEST_TRACE)
        wm_89c78_test_write_sxy3(packet + 8u, packet + 0x10u, packet + 0x18u);
#else
        w9c_rtpt_sxy(packet + 8u, packet + 0x10u, packet + 0x18u);
#endif

#if defined(WM_89C78_TEST_TRACE)
        flag = wm_89c78_test_rtps(W9C_SC + 0x18u, &sz);
#else
        flag = w9c_rtps_flag(W9C_SC + 0x18u, &sz);
#endif
        w9c_sw(W9C_SC_FLAG, flag);
#if defined(WM_89C78_MUTANT_SKIP_FLAG)
        flag = 0u;
#endif
        if ((flag & 0x80000000u) != 0u)
            continue;
#if defined(WM_89C78_TEST_TRACE)
        wm_89c78_test_write_sxy(packet + 0x20u);
#else
        w9c_rtps_sxy(packet + 0x20u);
#endif

        w9c_sw(W9C_SC_SZ, sz);
#if defined(WM_89C78_MUTANT_WRONG_XCLIP)
        if (w9c_lh(packet + 8u) >= 0x100 && w9c_lh(packet + 0x10u) >= 0x100 &&
            w9c_lh(packet + 0x18u) >= 0x100 && w9c_lh(packet + 0x20u) >= 0x100)
#else
        if (w9c_lh(packet + 8u) >= W9C_X_LIMIT &&
            w9c_lh(packet + 0x10u) >= W9C_X_LIMIT &&
            w9c_lh(packet + 0x18u) >= W9C_X_LIMIT &&
            w9c_lh(packet + 0x20u) >= W9C_X_LIMIT)
#endif
            continue;
#if defined(WM_89C78_MUTANT_WRONG_YCLIP)
        if (w9c_lh(packet + 0xAu) >= 0x100 && w9c_lh(packet + 0x12u) >= 0x100 &&
            w9c_lh(packet + 0x1Au) >= 0x100 && w9c_lh(packet + 0x22u) >= 0x100)
#else
        if (w9c_lh(packet + 0xAu) >= W9C_Y_LIMIT &&
            w9c_lh(packet + 0x12u) >= W9C_Y_LIMIT &&
            w9c_lh(packet + 0x1Au) >= W9C_Y_LIMIT &&
            w9c_lh(packet + 0x22u) >= W9C_Y_LIMIT)
#endif
            continue;
#if defined(WM_89C78_MUTANT_WRONG_SZ)
        if (w9c_bits_as_s32(sz) >= 0x1000)
#else
        if (w9c_bits_as_s32(sz) >= W9C_SZ_LIMIT)
#endif
            continue;

#if !defined(WM_89C78_MUTANT_SKIP_RGB)
        w9c_sb(packet + 4u, w9c_lbu(slot + 0x3Au));
        w9c_sb(packet + 5u, w9c_lbu(slot + 0x3Bu));
        w9c_sb(packet + 6u, w9c_lbu(slot + 0x3Cu));
        w9c_sh(packet + 0x16u, w9c_lhu(slot + 0x42u));
#endif
#if !defined(WM_89C78_MUTANT_SKIP_UV)
        {
            u32 uv = W9C_UV + ((u32)idx << 3);

            w9c_sh(packet + 0xCu, w9c_lhu(uv));
            w9c_sh(packet + 0x14u, w9c_lhu(uv + 2u));
            w9c_sh(packet + 0x1Cu, w9c_lhu(uv + 4u));
            w9c_sh(packet + 0x24u, w9c_lhu(uv + 6u));
        }
#endif
#if !defined(WM_89C78_MUTANT_SKIP_OT)
        {
            u32 ot = w9c_lw(w9c_lw(W9C_DB) + 0x70u) + ((sz >> 4) << 2);
            u32 head = w9c_lw(ot);
            u32 tag = w9c_lw(packet);

            w9c_sw(packet, (tag & W9C_OT_HI) | (head & W9C_OT_LO));
            w9c_sw(ot, (head & W9C_OT_HI) | (packet & W9C_OT_LO));
            packet += W9C_PKT_STRIDE;
        }
#endif
    }
}
