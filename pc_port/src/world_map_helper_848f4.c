/*
 * World-map region model submitter 0x800848F4.
 *
 * Register-faithful transcription of retail world_map.bin
 * [0x800848F4, 0x80084D00).  See world_map_helper_848f4.h.
 */
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_848f4.h"
#include "world_map_helper_93534.h"

#define W8F4_POS_X        0x8009BE28u
#define W8F4_POS_Z        0x8009BE30u
#define W8F4_COUNT        0x8009D7E0u
#define W8F4_ROOT         0x8009C620u
#define W8F4_INDEX        0x8009D7F0u
#define W8F4_DB           0x8009BE3Cu
#define W8F4_CAMERA       0x8009C808u
#define W8F4_TABLE        0x8009AD2Cu
#define W8F4_D104         0x80050104u
#define W8F4_C0           0x800595C0u
#define W8F4_C78          0x80059578u
#define W8F4_SC           0x1F800000u
#define W8F4_SC_SCALE     0x1F800010u
#define W8F4_SC_FLAG      0x1F800020u
#define W8F4_SC_SZ        0x1F800028u
#define W8F4_SC_SVEC      0x1F8000A0u
#define W8F4_SC_MTX       0x1F8000F0u
#define W8F4_SC_COMP      0x1F800110u
#define W8F4_STRIDE       0x54u
#define W8F4_SCALE        0x800u
#define W8F4_SZ_LIMIT     0xD80

typedef struct {
    s16 vx;
    s16 vy;
    s16 vz;
    s16 pad;
} Wm8f4Svector;

typedef struct {
    s32 vx;
    s32 vy;
    s32 vz;
    s32 pad;
} Wm8f4Vector;

typedef struct {
    s16 m[3][3];
    s32 t[3];
} Wm8f4Matrix;

extern Wm8f4Matrix *ScaleMatrix(Wm8f4Matrix *m, Wm8f4Vector *v);
extern Wm8f4Matrix *CompMatrix(Wm8f4Matrix *m0, Wm8f4Matrix *m1,
                               Wm8f4Matrix *m2);
extern void SetRotMatrix(Wm8f4Matrix *m);
extern void SetTransMatrix(Wm8f4Matrix *m);
extern Wm8f4Svector *ApplyMatrixSV(Wm8f4Matrix *m, Wm8f4Svector *v0,
                                   Wm8f4Svector *v1);
extern void RotTrans(Wm8f4Svector *v0, Wm8f4Vector *v1, long *flag);
extern s32 func_8002C700(void *a0, void *a1, void *a2, s32 a3);

#if defined(WM_848F4_TEST_TRACE)
extern u32 wm_848f4_test_rtps(u32 svec_addr, u32 *flag_out);
extern void wm_848f4_test_store(u32 address, u32 value);
#define W8F4_TRACE_STORE(a, v) wm_848f4_test_store((a), (v))
#else
#define W8F4_TRACE_STORE(a, v) ((void)0)
#endif

static u32 w8f4_lw(u32 a)
{
    u32 v;

    memcpy(&v, PSX_ADDR(a), 4);
    return v;
}

static s32 w8f4_lh(u32 a)
{
    s16 h;

    memcpy(&h, PSX_ADDR(a), 2);
    return (s32)h;
}

static void w8f4_sh(u32 a, u32 v)
{
    u16 h = (u16)(v & 0xFFFFu);

    memcpy(PSX_ADDR(a), &h, 2);
}

static void w8f4_sw(u32 a, u32 v)
{
    memcpy(PSX_ADDR(a), &v, 4);
    W8F4_TRACE_STORE(a, v);
}

static s32 w8f4_bits_as_s32(u32 value)
{
    s32 result;

    memcpy(&result, &value, sizeof(result));
    return result;
}

static u32 w8f4_s32_as_bits(s32 value)
{
    u32 result;

    memcpy(&result, &value, sizeof(result));
    return result;
}

static s32 w8f4_sra(s32 value, u32 shift)
{
    u32 amount = shift & 31u;
    u32 bits = w8f4_s32_as_bits(value);
    u32 result;

    if (amount == 0u)
        return value;
    result = bits >> amount;
    if (value < 0)
        result |= UINT32_MAX << (32u - amount);
    return w8f4_bits_as_s32(result);
}

#if !defined(WM_848F4_TEST_TRACE)
#include <psx/inline_c.h>

static u32 w8f4_rtps(u32 svec_addr, u32 *flag_out)
{
    u32 sz = 0u;
    u32 flag = 0u;

    gte_ldv0(PSX_ADDR(svec_addr));
    gte_rtps();
    gte_stflg(&flag);
    gte_stsz(&sz);
    *flag_out = flag;
    return sz;
}
#endif

static void w8f4_copy_matrix(u32 dst, u32 src)
{
    u32 i;

    for (i = 0u; i < 8u; i++)
        w8f4_sw(dst + i * 4u, w8f4_lw(src + i * 4u));
}

static void w8f4_prepare_link_t(u32 obj)
{
    w8f4_sw(obj + 0x34u, w8f4_lw(obj + 8u));
    w8f4_sw(obj + 0x38u, w8f4_lw(obj + 0xCu));
    w8f4_sw(obj + 0x3Cu, 0u - w8f4_lw(obj + 0x10u));
}

static void w8f4_apply_link(u32 link, u32 mtx)
{
    Wm8f4Svector col;
    Wm8f4Svector out;
    Wm8f4Svector tin;
    long dead_flag;
    u32 c;

#if !defined(WM_848F4_MUTANT_SKIP_LINK)
    w8f4_prepare_link_t(link);
    SetRotMatrix((Wm8f4Matrix *)PSX_ADDR(link + 0x20u));
    for (c = 0u; c < 3u; c++) {
        col.vx = (s16)w8f4_lh(mtx + c * 2u);
        col.vy = (s16)w8f4_lh(mtx + 6u + c * 2u);
        col.vz = (s16)w8f4_lh(mtx + 12u + c * 2u);
        (void)ApplyMatrixSV((Wm8f4Matrix *)PSX_ADDR(link + 0x20u), &col, &out);
        w8f4_sh(mtx + c * 2u, (u32)(u16)out.vx);
        w8f4_sh(mtx + 6u + c * 2u, (u32)(u16)out.vy);
        w8f4_sh(mtx + 12u + c * 2u, (u32)(u16)out.vz);
    }
    SetTransMatrix((Wm8f4Matrix *)PSX_ADDR(link + 0x20u));
    tin.vx = (s16)(w8f4_lw(mtx + 0x14u) & 0xFFFFu);
    tin.vy = (s16)(w8f4_lw(mtx + 0x18u) & 0xFFFFu);
    tin.vz = (s16)(w8f4_lw(mtx + 0x1Cu) & 0xFFFFu);
    RotTrans(&tin, (Wm8f4Vector *)PSX_ADDR(mtx + 0x14u), &dead_flag);
#else
    (void)link;
    (void)mtx;
    (void)col;
    (void)out;
    (void)tin;
    (void)dead_flag;
    (void)c;
#endif
}

void wm_800848F4(void)
{
    s32 count;
    s32 idx;
    u32 rec;
    u32 link;

    w8f4_sw(W8F4_SC_SCALE + 8u, W8F4_SCALE);
    w8f4_sw(W8F4_SC_SCALE + 4u, W8F4_SCALE);
    w8f4_sw(W8F4_SC_SCALE, W8F4_SCALE);
    w8f4_sw(W8F4_D104, 3u);
    w8f4_sw(W8F4_C0, 0u);
    w8f4_sw(W8F4_C78, 0u);
    w8f4_sh(W8F4_SC_SVEC + 4u, 0u);
    w8f4_sh(W8F4_SC_SVEC + 2u, 0u);
    w8f4_sh(W8F4_SC_SVEC, 0u);

    count = w8f4_lh(W8F4_COUNT);
    if (count <= 0)
        return;

    for (idx = 0; idx < w8f4_lh(W8F4_COUNT); idx++) {
        s32 pos_x;
        s32 pos_z;
        u32 flag;
        u32 sz;

        rec = w8f4_lw(W8F4_ROOT) + (u32)idx * W8F4_STRIDE;
#if defined(WM_848F4_MUTANT_SKIP_ENABLE)
        (void)0;
#else
        if (w8f4_lh(rec) != 0)
            continue;
#endif
        w8f4_copy_matrix(W8F4_SC_MTX, rec + 0x20u);
        w8f4_sw(W8F4_SC_MTX + 0x14u, w8f4_lw(rec + 8u));
        w8f4_sw(W8F4_SC_MTX + 0x18u, w8f4_lw(rec + 0xCu));
        w8f4_sw(W8F4_SC_MTX + 0x1Cu, 0u - w8f4_lw(rec + 0x10u));

        link = w8f4_lw(rec + 0x50u);
        while (link != 0u) {
            w8f4_apply_link(link, W8F4_SC_MTX);
            link = w8f4_lw(link + 0x50u);
        }

#if defined(WM_848F4_MUTANT_WRONG_SRA)
        pos_x = w8f4_sra(w8f4_bits_as_s32(w8f4_lw(W8F4_POS_X)), 8u);
        pos_z = w8f4_sra(w8f4_bits_as_s32(w8f4_lw(W8F4_POS_Z)), 8u);
#else
        pos_x = w8f4_sra(w8f4_bits_as_s32(w8f4_lw(W8F4_POS_X)), 12u);
        pos_z = w8f4_sra(w8f4_bits_as_s32(w8f4_lw(W8F4_POS_Z)), 12u);
#endif
        w8f4_sw(W8F4_SC, w8f4_lw(W8F4_SC_MTX + 0x14u) - w8f4_s32_as_bits(pos_x));
        w8f4_sw(W8F4_SC + 8u,
                (0u - w8f4_lw(W8F4_SC_MTX + 0x1Cu)) - w8f4_s32_as_bits(pos_z));
#if !defined(WM_848F4_MUTANT_SKIP_WRAP)
        wm_80093534(W8F4_SC);
#endif
#if !defined(WM_848F4_MUTANT_SKIP_T_PUBLISH)
        w8f4_sw(W8F4_SC_MTX + 0x14u, w8f4_lw(W8F4_SC));
        w8f4_sw(W8F4_SC_MTX + 0x1Cu, 0u - w8f4_lw(W8F4_SC + 8u));
#endif

#if !defined(WM_848F4_MUTANT_SKIP_SCALE)
        (void)ScaleMatrix((Wm8f4Matrix *)PSX_ADDR(W8F4_SC_MTX),
                          (Wm8f4Vector *)PSX_ADDR(W8F4_SC_SCALE));
#endif
#if !defined(WM_848F4_MUTANT_SKIP_COMP)
        (void)CompMatrix((Wm8f4Matrix *)PSX_ADDR(W8F4_CAMERA),
                         (Wm8f4Matrix *)PSX_ADDR(W8F4_SC_MTX),
                         (Wm8f4Matrix *)PSX_ADDR(W8F4_SC_COMP));
#endif
#if !defined(WM_848F4_MUTANT_SKIP_SETROT)
        SetRotMatrix((Wm8f4Matrix *)PSX_ADDR(W8F4_SC_COMP));
#endif
#if !defined(WM_848F4_MUTANT_SKIP_SETTRANS)
        SetTransMatrix((Wm8f4Matrix *)PSX_ADDR(W8F4_SC_COMP));
#endif

#if defined(WM_848F4_TEST_TRACE)
        sz = wm_848f4_test_rtps(W8F4_SC_SVEC, &flag);
#else
        sz = w8f4_rtps(W8F4_SC_SVEC, &flag);
#endif
        w8f4_sw(W8F4_SC_FLAG, flag);
#if defined(WM_848F4_MUTANT_SKIP_FLAG)
        flag = 0u;
#endif
        if (w8f4_bits_as_s32(flag) < 0)
            continue;
        w8f4_sw(W8F4_SC_SZ, sz);
#if defined(WM_848F4_MUTANT_WRONG_LIMIT)
        if (w8f4_bits_as_s32(sz) >= 0x1000)
#else
        if (w8f4_bits_as_s32(sz) >= W8F4_SZ_LIMIT)
#endif
            continue;

        rec = w8f4_lw(W8F4_ROOT) + (u32)idx * W8F4_STRIDE;
#if !defined(WM_848F4_MUTANT_SKIP_SUBMIT)
        {
            u32 packet = w8f4_lw(rec + 0x40u);
            u32 work = w8f4_lw(rec + 0x48u + (w8f4_lw(W8F4_INDEX) << 2));
            u32 ot = w8f4_lw(w8f4_lw(W8F4_DB) + 0x70u);
            s32 variant = w8f4_lh(W8F4_TABLE + (u32)(w8f4_lh(rec + 4u) << 1));

            (void)func_8002C700(PSX_ADDR(packet), PSX_ADDR(work), PSX_ADDR(ot),
                                variant);
        }
#endif
    }
}
