/*
 * World-map scheduler callback 0x800907F4 (slot-8 cb1).
 *
 * Register-faithful transcription of retail world_map.bin
 * [0x800907F4, 0x80090A18).  See world_map_callback_907f4.h.
 *
 * Only JAL is PsyQ RotMatrixZYX (0x8004ABBC), twice, into context
 * records 2 (+0xA8) and 3 (+0xFC) at +0x20.
 */
#include <stdint.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_callback_907f4.h"

typedef struct {
    s16 vx;
    s16 vy;
    s16 vz;
    s16 pad;
} WM_SVECTOR;

typedef struct {
    s16 m[3][3];
    s32 t[3];
} WM_MATRIX;

extern WM_MATRIX* RotMatrixZYX(WM_SVECTOR* r, WM_MATRIX* m);

#define F4_POOL_PTR     0x8009BE24u
#define F4_CONTEXT_PTR  0x8009C620u
#define F4_SCRATCH      0x1F800000u
#define F4_REC2         0xA8u
#define F4_REC3         0xFCu

#if defined(WM_907F4_TEST_TRACE)
extern void wm_907f4_test_store(u32 address, u32 width, u32 value);
extern void wm_907f4_test_rotmatrixzyx(u32 vec, u32 mat);
#define F4_TRACE_STORE(a, w, v) wm_907f4_test_store((a), (w), (v))
#define F4_CALL_ROT(v, m)       wm_907f4_test_rotmatrixzyx((v), (m))
#else
#define F4_TRACE_STORE(a, w, v) ((void)0)
#define F4_CALL_ROT(v, m) \
    ((void)RotMatrixZYX((WM_SVECTOR*)PSX_ADDR(v), (WM_MATRIX*)PSX_ADDR(m)))
#endif

static u32 f4_lw(u32 a)
{
    u32 v;

    memcpy(&v, PSX_ADDR(a), 4);
    return v;
}

static void f4_sw(u32 a, u32 v)
{
    memcpy(PSX_ADDR(a), &v, 4);
    F4_TRACE_STORE(a, 4u, v);
}

static s16 f4_lh(u32 a)
{
    s16 v;

    memcpy(&v, PSX_ADDR(a), 2);
    return v;
}

static u16 f4_lhu(u32 a)
{
    u16 v;

    memcpy(&v, PSX_ADDR(a), 2);
    return v;
}

static void f4_sh(u32 a, u16 v)
{
    memcpy(PSX_ADDR(a), &v, 2);
    F4_TRACE_STORE(a, 2u, v);
}

static s32 f4_s32(u32 v)
{
    s32 r;

    memcpy(&r, &v, 4);
    return r;
}

s32 wm_800907F4(s32 slot_idx)
{
    u32 pool = f4_lw(F4_POOL_PTR);
    u32 ctx = f4_lw(F4_CONTEXT_PTR);
    u32 slot = pool + ((u32)slot_idx << 7);
    u32 rec2 = ctx + F4_REC2;
    u32 rec3 = ctx + F4_REC3;
    u32 sc = F4_SCRATCH;
    s32 sub = (s32)f4_lh(slot + 4u);
    s32 state;
    u32 a58;
    u32 a5c;

    if (sub == 9) {
#if defined(WM_907F4_MUTANT_SUBSTATE_POLARITY)
        f4_sh(slot + 4u, 0u);
        f4_sh(slot + 0x20u, 2u);
#else
        f4_sh(slot + 4u, 0u);
        f4_sh(slot + 0x20u, 1u);
#endif
    } else if (sub == 0xA) {
#if defined(WM_907F4_MUTANT_SUBSTATE_POLARITY)
        f4_sh(slot + 4u, 0u);
        f4_sh(slot + 0x20u, 1u);
#else
        f4_sh(slot + 4u, 0u);
        f4_sh(slot + 0x20u, 2u);
#endif
    }

    state = (s32)f4_lh(slot + 0x20u);
    if (state == 1) {
#if defined(WM_907F4_MUTANT_STATE1_STEP)
        a58 = f4_lw(slot + 0x58u) + 8u;
#else
        a58 = f4_lw(slot + 0x58u) + 4u;
#endif
        f4_sw(slot + 0x58u, a58);
#if defined(WM_907F4_MUTANT_STATE1_GATE)
        if (f4_s32(a58) >= 0x10)
#else
        if (f4_s32(a58) >= 0x11)
#endif
            f4_sw(slot + 0x5Cu, f4_lw(slot + 0x5Cu) + 4u);
        a58 = f4_lw(slot + 0x58u);
#if defined(WM_907F4_MUTANT_STATE1_CLAMP)
        if (f4_s32(a58) >= 0x7F)
            f4_sw(slot + 0x58u, 0x7Fu);
#else
        if (f4_s32(a58) >= 0x80)
            f4_sw(slot + 0x58u, 0x80u);
#endif
        a5c = f4_lw(slot + 0x5Cu);
        if (f4_s32(a5c) >= 0x80)
            f4_sw(slot + 0x5Cu, 0x80u);
        a58 = f4_lw(slot + 0x58u);
        a5c = f4_lw(slot + 0x5Cu);
#if !defined(WM_907F4_MUTANT_STATE1_ADVANCE)
        if (f4_s32(a58) >= 0x80 && f4_s32(a5c) >= 0x80)
            f4_sh(slot + 0x20u, 3u);
#endif
    } else if (state < 2) {
        if (state == 0) {
#if !defined(WM_907F4_MUTANT_STATE0_SKIP_CLEAR)
            f4_sw(slot + 0x58u, 0u);
            f4_sw(slot + 0x5Cu, 0u);
#endif
        }
    } else if (state == 2) {
#if defined(WM_907F4_MUTANT_STATE2_STEP)
        a58 = f4_lw(slot + 0x58u) - 8u;
#else
        a58 = f4_lw(slot + 0x58u) - 4u;
#endif
        f4_sw(slot + 0x58u, a58);
#if defined(WM_907F4_MUTANT_STATE2_GATE)
        if (f4_s32(a58) < 0x6F)
#else
        if (f4_s32(a58) < 0x70)
#endif
            f4_sw(slot + 0x5Cu, f4_lw(slot + 0x5Cu) - 4u);
        a58 = f4_lw(slot + 0x58u);
#if defined(WM_907F4_MUTANT_STATE2_CLAMP)
        (void)a58;
#else
        if (f4_s32(a58) < 0)
            f4_sw(slot + 0x58u, 0u);
#endif
        a5c = f4_lw(slot + 0x5Cu);
        if (f4_s32(a5c) < 0)
            f4_sw(slot + 0x5Cu, 0u);
        a58 = f4_lw(slot + 0x58u);
        a5c = f4_lw(slot + 0x5Cu);
#if !defined(WM_907F4_MUTANT_STATE2_ADVANCE)
        if (a58 == 0u && a5c == 0u)
            f4_sh(slot + 0x20u, 0u);
#endif
    }

#if !defined(WM_907F4_MUTANT_SKIP_50_UPDATE)
    f4_sw(slot + 0x50u, f4_lw(slot + 0x50u) + f4_lw(slot + 0x58u));
#endif
#if !defined(WM_907F4_MUTANT_SKIP_54_SUB)
    f4_sw(slot + 0x54u, f4_lw(slot + 0x54u) - f4_lw(slot + 0x5Cu));
#endif

    f4_sh(sc + 0xA8u, 0u);
    f4_sh(sc + 0xA0u, 0u);
#if defined(WM_907F4_MUTANT_SKIP_VECTOR_Y)
    f4_sh(sc + 0xA2u, 0u);
#else
    f4_sh(sc + 0xA2u, (u16)f4_lw(slot + 0x50u));
#endif
    f4_sh(sc + 0xAAu, (u16)f4_lw(slot + 0x54u));
    f4_sh(sc + 0xA4u, f4_lhu(rec2 + 0x1Cu));
    f4_sh(sc + 0xACu, f4_lhu(rec3 + 0x1Cu));
#if defined(WM_907F4_MUTANT_ROTMATRIX_SWAP)
    F4_CALL_ROT(sc + 0xA0u, rec3 + 0x20u);
    F4_CALL_ROT(sc + 0xA8u, rec2 + 0x20u);
#else
    F4_CALL_ROT(sc + 0xA0u, rec2 + 0x20u);
    F4_CALL_ROT(sc + 0xA8u, rec3 + 0x20u);
#endif

#if defined(WM_907F4_MUTANT_WRONG_RETURN)
    return (s32)f4_lh(slot + 0x20u);
#else
    return 1;
#endif
}
