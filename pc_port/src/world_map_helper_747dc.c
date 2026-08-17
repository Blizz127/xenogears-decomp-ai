/*
 * World-map keep-list GTE submitter 0x800747DC.
 *
 * Register-faithful transcription of retail world_map.bin
 * [0x800747DC, 0x80074E58).  See world_map_helper_747dc.h.
 */
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_747dc.h"

extern s32 wm_80093978(s32 x, s32 z);
extern s32 wm_80093740(u32 out_normal_addr, s32 x, s32 z);

#define W7DC_COUNT     0x8009BE38u
#define W7DC_CAM       0x8009C808u
#define W7DC_RECS      0x8009D30Cu
#define W7DC_INDEX     0x8009D7F0u
#define W7DC_POS_X     0x8009BE28u
#define W7DC_POS_Z     0x8009BE30u
#define W7DC_DESTS     0x8009BE14u
#define W7DC_MODE2     0x8009A180u
#define W7DC_ANGLE     0x8006EE66u
#define W7DC_DB        0x8009BE3Cu
#define W7DC_SC        0x1F800000u
#define W7DC_OT_HI     0xFF000000u
#define W7DC_OT_LO     0x00FFFFFFu
#define W7DC_PKT       0x28u
#define W7DC_REC       8u

typedef struct {
    s16 vx;
    s16 vy;
    s16 vz;
    s16 pad;
} Wm7dcSvector;

typedef struct {
    s32 vx;
    s32 vy;
    s32 vz;
    s32 pad;
} Wm7dcVector;

typedef struct {
    s16 m[3][3];
    s32 t[3];
} Wm7dcMatrix;

extern void OuterProduct12(Wm7dcVector *v0, Wm7dcVector *v1, Wm7dcVector *v2);
extern long VectorNormal(Wm7dcVector *v0, Wm7dcVector *v1);
extern Wm7dcMatrix *RotMatrixY(int r, Wm7dcMatrix *m);
extern Wm7dcMatrix *MulMatrix(Wm7dcMatrix *m0, Wm7dcMatrix *m1);
extern Wm7dcMatrix *ScaleMatrix(Wm7dcMatrix *m, Wm7dcVector *v);
extern void SetRotMatrix(Wm7dcMatrix *m);
extern void SetTransMatrix(Wm7dcMatrix *m);

#if defined(WM_747DC_TEST_TRACE)
extern void wm_747dc_test_store(u32 address, u32 value);
extern s32 wm_747dc_test_flag(void);
extern void wm_747dc_test_rtir(u32 src, u32 dst);
extern void wm_747dc_test_rt(u32 vec, u32 dst);
extern void wm_747dc_test_rtpt(u32 v0, u32 v1, u32 v2);
extern void wm_747dc_test_rtpt_store(u32 pkt);
extern void wm_747dc_test_rtps(u32 vec, u32 pkt);
#define W7DC_TRACE_STORE(a, v) wm_747dc_test_store((a), (v))
#else
#define W7DC_TRACE_STORE(a, v) ((void)0)
#endif

static u32 w7dc_lw(u32 a)
{
    u32 v;

    memcpy(&v, PSX_ADDR(a), 4);
    return v;
}

static s32 w7dc_lh(u32 a)
{
    s16 h;

    memcpy(&h, PSX_ADDR(a), 2);
    return (s32)h;
}

static u32 w7dc_lhu(u32 a)
{
    u16 h;

    memcpy(&h, PSX_ADDR(a), 2);
    return (u32)h;
}

static void w7dc_sw(u32 a, u32 v)
{
    memcpy(PSX_ADDR(a), &v, 4);
    W7DC_TRACE_STORE(a, v);
}

static void w7dc_sh(u32 a, u32 v)
{
    u16 h = (u16)v;

    memcpy(PSX_ADDR(a), &h, 2);
    W7DC_TRACE_STORE(a, (u32)h);
}

static void w7dc_copy32(u32 dst, u32 src, u32 words)
{
    u32 i;

    for (i = 0u; i < words; i++)
        w7dc_sw(dst + i * 4u, w7dc_lw(src + i * 4u));
}

#if !defined(WM_747DC_TEST_TRACE)
#include <psx/inline_c.h>

static void w7dc_rtir_col(u32 src, u32 dst)
{
    gte_ldclmv(PSX_ADDR(src));
    gte_rtir();
    gte_stclmv(PSX_ADDR(dst));
}

static s32 w7dc_rtpt(u32 v0, u32 v1, u32 v2)
{
    s32 flag = 0;

    gte_ldv0(PSX_ADDR(v0));
    gte_ldv1(PSX_ADDR(v1));
    gte_ldv2(PSX_ADDR(v2));
    gte_rtpt();
    gte_stflg(&flag);
    return flag;
}

static void w7dc_rtpt_store(u32 pkt)
{
    gte_stsxy3(PSX_ADDR(pkt + 8u), PSX_ADDR(pkt + 0x10u),
               PSX_ADDR(pkt + 0x18u));
    gte_stsz3(PSX_ADDR(W7DC_SC + 0x80u), PSX_ADDR(W7DC_SC + 0x84u),
              PSX_ADDR(W7DC_SC + 0x88u));
}

static void w7dc_rt(u32 vec, u32 dst)
{
    gte_ldlv0(PSX_ADDR(vec));
    gte_rt();
    gte_stlvnl(PSX_ADDR(dst));
}

static void w7dc_rtps(u32 vec, u32 pkt)
{
    gte_ldv0(PSX_ADDR(vec));
    gte_rtps();
    gte_stsxy2(PSX_ADDR(pkt + 0x20u));
    gte_stsz(PSX_ADDR(W7DC_SC + 0x80u));
}
#endif

static s32 w7dc_min3(s32 a, s32 b, s32 c)
{
    if (b < a)
        a = b;
    if (c < a)
        a = c;
    return a;
}

static void w7dc_ot_insert(u32 pkt, s32 depth)
{
    u32 db;
    u32 ot;
    u32 slot;
    u32 head;
    u32 word;

#if defined(WM_747DC_MUTANT_SKIP_OT)
    (void)pkt;
    (void)depth;
    return;
#endif
    db = w7dc_lw(W7DC_DB);
    ot = w7dc_lw(db + 0x70u);
#if defined(WM_747DC_MUTANT_WRONG_ZSHIFT)
    slot = (u32)depth * 4u;
#else
    slot = (u32)(depth >> 4) * 4u;
#endif
    head = w7dc_lw(ot + slot);
    word = w7dc_lw(pkt);
    w7dc_sw(pkt, (word & W7DC_OT_HI) | (head & W7DC_OT_LO));
    w7dc_sw(ot + slot, (head & W7DC_OT_HI) | (pkt & W7DC_OT_LO));
}

static void w7dc_init_scratch(void)
{
    w7dc_sh(W7DC_SC + 0xA0u, (u32)(u16)(s16)-0x10);
    w7dc_sh(W7DC_SC + 0xA4u, 0x10u);
    w7dc_sh(W7DC_SC + 0xA8u, 0x10u);
    w7dc_sh(W7DC_SC + 0xACu, 0x10u);
    w7dc_sh(W7DC_SC + 0xB0u, (u32)(u16)(s16)-0x10);
    w7dc_sh(W7DC_SC + 0xB4u, (u32)(u16)(s16)-0x10);
    w7dc_sh(W7DC_SC + 0xD8u, 0x10u);
    w7dc_sh(W7DC_SC + 0xDCu, (u32)(u16)(s16)-0x10);
    w7dc_sh(W7DC_SC + 0xDAu, 0u);
    w7dc_sh(W7DC_SC + 0xB2u, 0u);
    w7dc_sh(W7DC_SC + 0xAAu, 0u);
    w7dc_sh(W7DC_SC + 0xA2u, 0u);
    w7dc_copy32(W7DC_SC + 0x130u, W7DC_CAM, 8u);
    w7dc_sw(W7DC_SC + 0x48u, 0x1000u);
    w7dc_sw(W7DC_SC + 0x40u, 0u);
    w7dc_sw(W7DC_SC + 0x44u, 0u);
}

static void w7dc_pack_matrix(void)
{
    w7dc_sh(W7DC_SC + 0xF0u, (u32)(u16)w7dc_lw(W7DC_SC + 0x60u));
    w7dc_sh(W7DC_SC + 0xF2u, (u32)(u16)w7dc_lw(W7DC_SC + 0x64u));
    w7dc_sh(W7DC_SC + 0xF4u, (u32)(u16)w7dc_lw(W7DC_SC + 0x68u));
    w7dc_sh(W7DC_SC + 0xF6u, (u32)(u16)w7dc_lw(W7DC_SC + 0x30u));
    w7dc_sh(W7DC_SC + 0xF8u, (u32)(u16)w7dc_lw(W7DC_SC + 0x34u));
    w7dc_sh(W7DC_SC + 0xFAu, (u32)(u16)w7dc_lw(W7DC_SC + 0x38u));
    w7dc_sh(W7DC_SC + 0xFCu, (u32)(u16)w7dc_lw(W7DC_SC + 0x50u));
    w7dc_sh(W7DC_SC + 0xFEu, (u32)(u16)w7dc_lw(W7DC_SC + 0x54u));
    w7dc_sh(W7DC_SC + 0x100u, (u32)(u16)w7dc_lw(W7DC_SC + 0x58u));
}

static void w7dc_apply_cam(void)
{
    SetRotMatrix((Wm7dcMatrix *)PSX_ADDR(W7DC_SC + 0x130u));
#if defined(WM_747DC_TEST_TRACE)
    wm_747dc_test_rtir(W7DC_SC + 0xF0u, W7DC_SC + 0x110u);
    wm_747dc_test_rtir(W7DC_SC + 0xF2u, W7DC_SC + 0x112u);
    wm_747dc_test_rtir(W7DC_SC + 0xF4u, W7DC_SC + 0x114u);
#else
    w7dc_rtir_col(W7DC_SC + 0xF0u, W7DC_SC + 0x110u);
    w7dc_rtir_col(W7DC_SC + 0xF2u, W7DC_SC + 0x112u);
    w7dc_rtir_col(W7DC_SC + 0xF4u, W7DC_SC + 0x114u);
#endif
    SetTransMatrix((Wm7dcMatrix *)PSX_ADDR(W7DC_SC + 0x130u));
#if defined(WM_747DC_TEST_TRACE)
    wm_747dc_test_rt(W7DC_SC + 0x104u, W7DC_SC + 0x124u);
#else
    w7dc_rt(W7DC_SC + 0x104u, W7DC_SC + 0x124u);
#endif
    SetRotMatrix((Wm7dcMatrix *)PSX_ADDR(W7DC_SC + 0x110u));
    SetTransMatrix((Wm7dcMatrix *)PSX_ADDR(W7DC_SC + 0x110u));
}

void wm_800747DC(void)
{
    s32 count;
    s32 left;
    s32 mode;
    s32 x;
    s32 z;
    s32 y;
    s32 depth;
    s32 flag;
    u32 recs;
    u32 pkt;
    u32 idx;

    (void)w7dc_init_scratch;
    (void)w7dc_pack_matrix;
    (void)w7dc_lhu;
    (void)w7dc_min3;
    count = (s32)w7dc_lw(W7DC_COUNT);
#if !defined(WM_747DC_MUTANT_SKIP_EARLY)
    if (count == 0)
        return;
#endif

#if !defined(WM_747DC_MUTANT_SKIP_SCRATCH)
    w7dc_init_scratch();
#endif
    recs = w7dc_lw(W7DC_RECS);
    idx = w7dc_lw(W7DC_INDEX);
    pkt = w7dc_lw(W7DC_DESTS + idx * 4u);
#if defined(WM_747DC_MUTANT_WRONG_COUNT)
    left = count - 2;
#else
    left = count - 1;
#endif

    while (left != -1) {
#if defined(WM_747DC_MUTANT_WRONG_SHIFT)
        x = w7dc_lh(recs) << 8;
        z = w7dc_lh(recs + 4u) << 8;
#else
        x = w7dc_lh(recs) << 12;
        z = w7dc_lh(recs + 4u) << 12;
#endif
        w7dc_sw(W7DC_SC + 0x70u, (u32)x);
        w7dc_sw(W7DC_SC + 0x78u, (u32)z);
#if !defined(WM_747DC_MUTANT_SKIP_93978)
        y = wm_80093978(x, z);
#else
        y = 0;
#endif
        w7dc_sw(W7DC_SC + 0x74u, (u32)y);
#if !defined(WM_747DC_MUTANT_SKIP_93740)
        (void)wm_80093740(W7DC_SC + 0x30u, x, z);
#endif
        OuterProduct12((Wm7dcVector *)PSX_ADDR(W7DC_SC + 0x40u),
                       (Wm7dcVector *)PSX_ADDR(W7DC_SC + 0x30u),
                       (Wm7dcVector *)PSX_ADDR(W7DC_SC + 0x50u));
        (void)VectorNormal((Wm7dcVector *)PSX_ADDR(W7DC_SC + 0x50u),
                           (Wm7dcVector *)PSX_ADDR(W7DC_SC + 0x60u));
        OuterProduct12((Wm7dcVector *)PSX_ADDR(W7DC_SC + 0x30u),
                       (Wm7dcVector *)PSX_ADDR(W7DC_SC + 0x60u),
                       (Wm7dcVector *)PSX_ADDR(W7DC_SC + 0x80u));
        (void)VectorNormal((Wm7dcVector *)PSX_ADDR(W7DC_SC + 0x80u),
                           (Wm7dcVector *)PSX_ADDR(W7DC_SC + 0x50u));
#if !defined(WM_747DC_MUTANT_SKIP_PACK)
        w7dc_pack_matrix();
#endif
        mode = w7dc_lh(recs + 2u);
        if (mode == 1) {
#if !defined(WM_747DC_MUTANT_SKIP_MODE1)
            w7dc_sw(W7DC_SC + 0x88u, 0x1800u);
            w7dc_sw(W7DC_SC + 0x84u, 0x1800u);
            w7dc_sw(W7DC_SC + 0x80u, 0x1800u);
            (void)ScaleMatrix((Wm7dcMatrix *)PSX_ADDR(W7DC_SC + 0xF0u),
                              (Wm7dcVector *)PSX_ADDR(W7DC_SC + 0x80u));
#endif
        } else if (mode == 2) {
#if !defined(WM_747DC_MUTANT_SKIP_MODE2)
            w7dc_copy32(W7DC_SC + 0x150u, W7DC_MODE2, 8u);
            (void)RotMatrixY((int)w7dc_lhu(W7DC_ANGLE),
                             (Wm7dcMatrix *)PSX_ADDR(W7DC_SC + 0x150u));
            (void)MulMatrix((Wm7dcMatrix *)PSX_ADDR(W7DC_SC + 0xF0u),
                            (Wm7dcMatrix *)PSX_ADDR(W7DC_SC + 0x150u));
            w7dc_sw(W7DC_SC + 0x80u, 0x1800u);
            w7dc_sw(W7DC_SC + 0x84u, 0x1000u);
            w7dc_sw(W7DC_SC + 0x88u, 0x4800u);
            (void)ScaleMatrix((Wm7dcMatrix *)PSX_ADDR(W7DC_SC + 0xF0u),
                              (Wm7dcVector *)PSX_ADDR(W7DC_SC + 0x80u));
#endif
        }

#if defined(WM_747DC_MUTANT_WRONG_POS)
        w7dc_sw(W7DC_SC + 0x104u, (u32)((z - (s32)w7dc_lw(W7DC_POS_Z)) >> 12));
        w7dc_sw(W7DC_SC + 0x10Cu, (u32)(((s32)w7dc_lw(W7DC_POS_X) - x) >> 12));
#else
        w7dc_sw(W7DC_SC + 0x104u, (u32)((x - (s32)w7dc_lw(W7DC_POS_X)) >> 12));
        w7dc_sw(W7DC_SC + 0x10Cu, (u32)(((s32)w7dc_lw(W7DC_POS_Z) - z) >> 12));
#endif
        w7dc_sw(W7DC_SC + 0x108u, (u32)(y >> 12));
        w7dc_apply_cam();
#if defined(WM_747DC_TEST_TRACE)
        flag = wm_747dc_test_flag();
        wm_747dc_test_rtpt(W7DC_SC + 0xA0u, W7DC_SC + 0xA8u, W7DC_SC + 0xB0u);
#else
        flag = w7dc_rtpt(W7DC_SC + 0xA0u, W7DC_SC + 0xA8u, W7DC_SC + 0xB0u);
#endif
#if defined(WM_747DC_MUTANT_FORCE_CLIP)
        flag = -1;
#endif
        if (flag >= 0) {
#if defined(WM_747DC_TEST_TRACE)
            wm_747dc_test_rtpt_store(pkt);
#else
            w7dc_rtpt_store(pkt);
#endif
            depth = w7dc_min3((s32)w7dc_lw(W7DC_SC + 0x80u),
                              (s32)w7dc_lw(W7DC_SC + 0x84u),
                              (s32)w7dc_lw(W7DC_SC + 0x88u));
#if defined(WM_747DC_TEST_TRACE)
            wm_747dc_test_rtps(W7DC_SC + 0xD8u, pkt);
#else
            w7dc_rtps(W7DC_SC + 0xD8u, pkt);
#endif
#if !defined(WM_747DC_MUTANT_SKIP_MIN_RTPS)
            if ((s32)w7dc_lw(W7DC_SC + 0x80u) < depth)
                depth = (s32)w7dc_lw(W7DC_SC + 0x80u);
#endif
#if defined(WM_747DC_MUTANT_WRONG_THRESH)
            if (depth < 0x800) {
#else
            if (depth < 0x1000) {
#endif
                w7dc_ot_insert(pkt, depth);
#if defined(WM_747DC_MUTANT_WRONG_PKT)
                pkt += 0x20u;
#else
                pkt += W7DC_PKT;
#endif
            }
        }
        left -= 1;
#if defined(WM_747DC_MUTANT_WRONG_REC)
        recs += 4u;
#else
        recs += W7DC_REC;
#endif
    }

#if !defined(WM_747DC_MUTANT_SKIP_CLEAR)
    w7dc_sw(W7DC_COUNT, 0u);
#endif
}
