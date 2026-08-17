/*
 * World-map scheduler callback 0x800914D0 (slot-9 cb1).
 *
 * Register-faithful transcription of retail world_map.bin
 * [0x800914D0, 0x80091B54).  See world_map_callback_914d0.h.
 *
 * JALs: wm_80097770 (once, state-2 arrival), wm_80093484 (approach or
 * snap delta VECTOR), wm_80093354 (common tail, slot+0x28).
 * No GTE / GPU / OT / scratchpad COP2.  Stack delta VECTOR is hosted
 * at 0x1F800010 so the wrap leaf can address guest memory.
 */
#include <stdint.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_callback_914d0.h"
#include "world_map_helper_93354.h"
#include "world_map_helper_93484.h"
#include "world_map_helper_97770.h"

#define D4_POOL_PTR  0x8009BE24u
#define D4_CD4C      0x8009CD4Cu
#define D4_BD3A      0x8009BD3Au
#define D4_D52C      0x8009D52Cu
#define D4_POSE      0x8009D55Cu
#define D4_BBB4      0x8009BBB4u
#define D4_BBBC      0x8009BBBCu
#define D4_BE28      0x8009BE28u
#define D4_DELTA     0x1F800010u

#if defined(WM_914D0_TEST_TRACE)
extern void wm_914d0_test_store(u32 address, u32 width, u32 value);
extern void wm_914d0_test_93354(u32 vec);
extern void wm_914d0_test_93484(u32 vec);
extern s32 wm_914d0_test_97770(u32 slot, s32 value);
#define D4_TRACE_STORE(a, w, v) wm_914d0_test_store((a), (w), (v))
#define D4_CALL_93354(a)        wm_914d0_test_93354(a)
#define D4_CALL_93484(a)        wm_914d0_test_93484(a)
#define D4_CALL_97770(s, v)     wm_914d0_test_97770((s), (v))
#else
#define D4_TRACE_STORE(a, w, v) ((void)0)
#define D4_CALL_93354(a)        wm_80093354(a)
#define D4_CALL_93484(a)        wm_80093484(a)
#define D4_CALL_97770(s, v)     wm_80097770((s), (v))
#endif

static u32 d4_lw(u32 a)
{
    u32 v;

    memcpy(&v, PSX_ADDR(a), 4);
    return v;
}

static void d4_sw(u32 a, u32 v)
{
    memcpy(PSX_ADDR(a), &v, 4);
    D4_TRACE_STORE(a, 4u, v);
}

static u16 d4_lhu(u32 a)
{
    u16 v;

    memcpy(&v, PSX_ADDR(a), 2);
    return v;
}

static s16 d4_lh(u32 a)
{
    s16 v;

    memcpy(&v, PSX_ADDR(a), 2);
    return v;
}

static void d4_sh(u32 a, u16 v)
{
    memcpy(PSX_ADDR(a), &v, 2);
    D4_TRACE_STORE(a, 2u, v);
}

static s32 d4_s32(u32 v)
{
    s32 r;

    memcpy(&r, &v, 4);
    return r;
}

static u32 d4_u32(s32 v)
{
    u32 r;

    memcpy(&r, &v, 4);
    return r;
}

static u32 d4_sra(u32 b, u32 amt)
{
    u32 v = b >> amt;

    if ((b & 0x80000000u) != 0u)
        v |= UINT32_MAX << (32u - amt);
    return v;
}

static s32 d4_abs_s32(s32 v)
{
    return (v < 0) ? -v : v;
}

static void d4_accum_heading(u32 slot, s32 delta, u32 sra_amt)
{
    u32 acc = d4_lw(slot + 0x58u);
    u32 inc = d4_sra(d4_u32(delta) << 12, sra_amt);
    u32 bits = (acc + inc) & 0x00FFFFFFu;

    d4_sw(slot + 0x58u, bits);
    d4_sh(D4_BD3A, (u16)d4_sra(bits, 12u));
}

static void d4_sync_heading_to_bd3a(u32 slot)
{
    s32 bd = (s32)d4_lh(D4_BD3A);
    s32 cur = d4_s32(d4_lw(slot + 0x50u));
    s32 delta;

    if (bd == cur) {
        d4_sw(slot + 0x58u, d4_u32(bd) << 12);
        return;
    }
    delta = cur - bd;
#if defined(WM_914D0_MUTANT_SYNC_THRESHOLD)
    if (d4_abs_s32(delta) >= 0x801)
#else
    if (d4_abs_s32(delta) >= 0xC01)
#endif
        delta += (delta < 0) ? 0x1000 : -0x1000;
    d4_accum_heading(slot, delta, 3u);
}

static s32 d4_chase_delta(u32 slot)
{
    s32 bd = (s32)d4_lh(D4_BD3A);
    s32 cur = d4_s32(d4_lw(slot + 0x50u));
    s32 delta = cur - bd;

    if (d4_abs_s32(delta) >= 0x801)
        delta += (delta < 0) ? 0x1000 : -0x1000;
    if (d4_abs_s32(delta) >= 0x181)
        delta = (delta < 0) ? -0x180 : 0x180;
    return delta;
}

static int d4_heading_arrived(u32 slot, u32 acc_bits)
{
    u32 pub = d4_sra(acc_bits, 12u);
    u32 eq_h = ((pub ^ d4_lw(slot + 0x50u)) < 1u);
    u32 eq_60 = (d4_lw(slot + 0x60u) < 1u);

    return (eq_h & eq_60) != 0u;
}

static void d4_fill_delta(u32 slot)
{
    u32 tx = d4_lw(D4_POSE);
    u32 ty = d4_lw(D4_POSE + 4u);
    u32 tz = d4_lw(D4_POSE + 8u);

    d4_sw(D4_DELTA, tx - d4_lw(slot + 0x28u));
    d4_sw(D4_DELTA + 4u, ty - d4_lw(slot + 0x2Cu));
    d4_sw(D4_DELTA + 8u, tz - d4_lw(slot + 0x30u));
    D4_CALL_93484(D4_DELTA);
}

static int d4_pose_equal(u32 slot)
{
    u32 ne = 0u;

    ne |= (d4_lw(slot + 0x28u) ^ d4_lw(D4_POSE)) != 0u;
    ne |= (d4_lw(slot + 0x2Cu) ^ d4_lw(D4_POSE + 4u)) != 0u;
    ne |= (d4_lw(slot + 0x30u) ^ d4_lw(D4_POSE + 8u)) != 0u;
    return ne == 0u;
}

static void d4_approach_path(u32 slot)
{
    u32 dx;
    u32 dy;
    u32 dz;
    u32 step_x;
    u32 step_z;
    s32 ax;
    s32 az;
    int close;

    if (d4_pose_equal(slot))
        return;

    d4_fill_delta(slot);
    dx = d4_lw(D4_DELTA);
    dy = d4_lw(D4_DELTA + 4u);
    dz = d4_lw(D4_DELTA + 8u);
    step_x = d4_sra(dx, 3u);
    step_z = d4_sra(dz, 3u);
    ax = d4_s32(step_x);
    az = d4_s32(step_z);
    if (ax < 0)
        ax = -ax;
    if (az < 0)
        az = -az;
#if defined(WM_914D0_MUTANT_CLOSE_THRESHOLD)
    close = (ax < 0x20) && (az < 0x20);
#else
    close = (ax < 0x40) && (az < 0x40);
#endif
    if (close) {
#if !defined(WM_914D0_MUTANT_SKIP_60)
        d4_sw(slot + 0x60u, 0u);
#endif
        d4_sw(D4_BBB4, d4_lw(D4_BBB4) + dx);
        d4_sw(D4_BBBC, d4_lw(D4_BBBC) + dz);
        d4_sw(slot + 0x28u, d4_lw(D4_POSE));
        d4_sw(slot + 0x30u, d4_lw(D4_POSE + 8u));
    } else {
#if !defined(WM_914D0_MUTANT_SKIP_60)
        d4_sw(slot + 0x60u, 1u);
#endif
        d4_sw(slot + 0x28u, d4_lw(slot + 0x28u) + step_x);
        d4_sw(slot + 0x30u, d4_lw(slot + 0x30u) + step_z);
        d4_sw(D4_BBB4, d4_lw(D4_BBB4) + step_x);
        d4_sw(D4_BBBC, d4_lw(D4_BBBC) + step_z);
    }
#if !defined(WM_914D0_MUTANT_APPROACH_SKIP_Y)
    d4_sw(slot + 0x2Cu, d4_lw(slot + 0x2Cu) + d4_sra(dy, 3u));
#else
    (void)dy;
#endif
}

static void d4_snap_path(u32 slot)
{
    u32 dy;

    if (d4_pose_equal(slot))
        return;

    d4_fill_delta(slot);
    dy = d4_lw(D4_DELTA + 4u);
#if defined(WM_914D0_MUTANT_SNAP_Y_SHIFT)
    d4_sw(slot + 0x2Cu, d4_lw(slot + 0x2Cu) + d4_sra(dy, 3u));
#else
    d4_sw(slot + 0x2Cu, d4_lw(slot + 0x2Cu) + d4_sra(dy, 4u));
#endif
    d4_sw(D4_BBB4, d4_lw(D4_BBB4) + d4_lw(D4_DELTA));
    d4_sw(D4_BBBC, d4_lw(D4_BBBC) + d4_lw(D4_DELTA + 8u));
    d4_sw(slot + 0x28u, d4_lw(D4_POSE));
    d4_sw(slot + 0x30u, d4_lw(D4_POSE + 8u));
}

static void d4_tail(u32 slot)
{
#if !defined(WM_914D0_MUTANT_SKIP_93354)
    D4_CALL_93354(slot + 0x28u);
#endif
#if !defined(WM_914D0_MUTANT_SKIP_PUBLISH)
    d4_sw(D4_BE28, d4_lw(slot + 0x28u));
    d4_sw(D4_BE28 + 4u, d4_lw(slot + 0x2Cu));
    d4_sw(D4_BE28 + 8u, d4_lw(slot + 0x30u));
    d4_sw(D4_BE28 + 12u, d4_lw(slot + 0x34u));
#endif
}

s32 wm_800914D0(s32 slot_idx)
{
    u32 pool = d4_lw(D4_POOL_PTR);
    u32 slot = pool + ((u32)slot_idx << 7);
    s32 sub;
    s32 state;
    u32 pad;

    sub = (s32)(s16)(d4_lhu(slot + 4u) - 9);
    if ((u32)sub < 9u) {
        switch (sub) {
        case 0: /* raw +4 == 9 */
#if defined(WM_914D0_MUTANT_SUBSTATE_POLARITY)
            d4_sh(slot + 4u, 0u);
            d4_sh(slot + 0x20u, 0u);
            d4_sw(slot + 0x50u, d4_u32((s32)d4_lh(D4_BD3A)));
#else
            d4_sh(slot + 4u, 0u);
            d4_sh(slot + 0x20u, 2u);
            d4_sw(slot + 0x50u, d4_u32((s32)d4_lh(D4_D52C)));
            d4_sw(slot + 0x58u, d4_u32((s32)d4_lh(D4_BD3A)) << 12);
#endif
            break;
        case 1: /* raw +4 == 0xA */
#if defined(WM_914D0_MUTANT_SUBSTATE_POLARITY)
            d4_sh(slot + 4u, 0u);
            d4_sh(slot + 0x20u, 2u);
            d4_sw(slot + 0x50u, d4_u32((s32)d4_lh(D4_D52C)));
            d4_sw(slot + 0x58u, d4_u32((s32)d4_lh(D4_BD3A)) << 12);
#else
            d4_sh(slot + 4u, 0u);
            d4_sh(slot + 0x20u, 0u);
            d4_sw(slot + 0x50u, d4_u32((s32)d4_lh(D4_BD3A)));
#endif
            break;
        case 6: /* raw +4 == 0xF */
            d4_sh(slot + 0x20u, 0x10u);
            d4_sh(slot + 4u, 0u);
#if defined(WM_914D0_MUTANT_SUB_HEADING)
            d4_sw(slot + 0x50u, 0x400u);
#else
            d4_sw(slot + 0x50u, 0x800u);
#endif
            break;
        case 7: /* raw +4 == 0x10 */
            d4_sh(slot + 0x20u, 0x10u);
            d4_sh(slot + 4u, 0u);
            d4_sw(slot + 0x50u, 0xA00u);
            break;
        case 8: /* raw +4 == 0x11 */
            d4_sh(slot + 0x20u, 0x10u);
            d4_sh(slot + 4u, 0u);
            d4_sw(slot + 0x50u, 0xC00u);
            break;
        default:
            break;
        }
    }

    state = (s32)d4_lh(slot + 0x20u);
#if defined(WM_914D0_MUTANT_JT1_GUARD)
    if ((u32)state < 0x10u)
#else
    if ((u32)state < 0x11u)
#endif
    {
        switch (state) {
        case 0:
#if defined(WM_914D0_MUTANT_PAD_SHIFT)
            pad = ((u32)d4_lhu(D4_CD4C) >> 4) & 3u;
#else
            pad = ((u32)d4_lhu(D4_CD4C) >> 2) & 3u;
#endif
            if (pad == 2u) {
                d4_sh(slot + 0x20u, 1u);
#if defined(WM_914D0_MUTANT_PAD_POLARITY)
                d4_sw(slot + 0x54u, d4_u32(-0x40));
                d4_sw(slot + 0x5Cu,
                      (d4_lw(slot + 0x50u) - 0x200u) & 0xFFFu);
#else
                d4_sw(slot + 0x54u, 0x40u);
                d4_sw(slot + 0x5Cu,
                      (d4_lw(slot + 0x50u) + 0x200u) & 0xFFFu);
#endif
            } else if (pad == 1u || pad == 3u) {
                d4_sh(slot + 0x20u, 1u);
#if defined(WM_914D0_MUTANT_PAD_POLARITY)
                d4_sw(slot + 0x54u, 0x40u);
                d4_sw(slot + 0x5Cu,
                      (d4_lw(slot + 0x50u) + 0x200u) & 0xFFFu);
#else
                d4_sw(slot + 0x54u, d4_u32(-0x40));
                d4_sw(slot + 0x5Cu,
                      (d4_lw(slot + 0x50u) - 0x200u) & 0xFFFu);
#endif
            }
            d4_sync_heading_to_bd3a(slot);
            break;
        case 1: {
            u32 h = (d4_lw(slot + 0x50u) + d4_lw(slot + 0x54u)) & 0xFFFu;

            d4_sw(slot + 0x50u, h);
            if (h == d4_lw(slot + 0x5Cu)) {
#if !defined(WM_914D0_MUTANT_STATE1_SKIP_CLEAR)
                d4_sh(slot + 0x20u, 0u);
#endif
            }
            d4_sync_heading_to_bd3a(slot);
            break;
        }
        case 2: {
            s32 delta = d4_chase_delta(slot);
            u32 sra_amt;
            u32 acc;

#if defined(WM_914D0_MUTANT_CHASE_SRA)
            sra_amt = 5u;
#else
            sra_amt = 3u;
#endif
            d4_accum_heading(slot, delta, sra_amt);
            acc = d4_lw(slot + 0x58u);
            if (d4_heading_arrived(slot, acc)) {
                d4_sh(slot + 0x20u, 3u);
#if defined(WM_914D0_MUTANT_WRONG_97770_ARGS)
                (void)D4_CALL_97770(8u, 11);
#elif !defined(WM_914D0_MUTANT_SKIP_97770)
                (void)D4_CALL_97770(7u, 11);
#endif
            }
            break;
        }
        case 0x10: {
            s32 delta = d4_chase_delta(slot);
            u32 sra_amt;
            u32 acc;

#if defined(WM_914D0_MUTANT_SLOW_SRA)
            sra_amt = 3u;
#else
            sra_amt = 5u;
#endif
            d4_accum_heading(slot, delta, sra_amt);
            acc = d4_lw(slot + 0x58u);
            if (d4_heading_arrived(slot, acc))
                d4_sh(slot + 0x20u, 3u);
            break;
        }
        default:
            break;
        }
    }

    state = (s32)d4_lh(slot + 0x20u);
    if (state == 3)
        d4_snap_path(slot);
    else if (state < 4) {
        if (state >= 0)
            d4_approach_path(slot);
    } else if (state == 0x10) {
        d4_approach_path(slot);
    }

    d4_tail(slot);
#if defined(WM_914D0_MUTANT_WRONG_RETURN)
    return state;
#else
    return 1;
#endif
}
