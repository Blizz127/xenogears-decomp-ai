/*
 * World-map scheduler callback 0x800922AC (slot-11 cb1).
 *
 * Register-faithful transcription of retail world_map.bin
 * [0x800922AC, 0x800923A8).  See world_map_callback_922ac.h.
 */
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_callback_922ac.h"

#define AC_POOL  0x8009BE24u
#define AC_BE0C  0x8009BE0Cu

#if defined(WM_922AC_TEST_TRACE)
extern void wm_922ac_test_store(u32 address, u32 value);
#define AC_TRACE_STORE(a, v) wm_922ac_test_store((a), (v))
#else
#define AC_TRACE_STORE(a, v) ((void)0)
#endif

static u32 ac_lw(u32 a)
{
    u32 v;

    memcpy(&v, PSX_ADDR(a), 4);
    return v;
}

static void ac_sw(u32 a, u32 v)
{
    memcpy(PSX_ADDR(a), &v, 4);
    AC_TRACE_STORE(a, v);
}

static s16 ac_lh(u32 a)
{
    s16 v;

    memcpy(&v, PSX_ADDR(a), 2);
    return v;
}

static void ac_sh(u32 a, u16 v)
{
    memcpy(PSX_ADDR(a), &v, 2);
    AC_TRACE_STORE(a, (u32)v);
}

static s32 ac_s32(u32 v)
{
    s32 r;

    memcpy(&r, &v, 4);
    return r;
}

s32 wm_800922AC(s32 slot_idx)
{
    u32 slot = ac_lw(AC_POOL) + ((u32)slot_idx << 7);
    s16 sub = ac_lh(slot + 4u);
    s16 state;
    s32 ret = 1;
    s32 scaled;

    if (sub == 9) {
        ac_sh(slot + 4u, 0);
#if defined(WM_922AC_MUTANT_SUB9_STATE)
        ac_sh(slot + 0x20u, 2);
#else
        ac_sh(slot + 0x20u, 1);
#endif
    } else if (sub == 0xA) {
        ac_sh(slot + 4u, 0);
        ac_sh(slot + 0x20u, 2);
    }

    state = ac_lh(slot + 0x20u);
    if (state == 0) {
#if defined(WM_922AC_MUTANT_STATE0_RETURN)
        return 1;
#else
        return 3;
#endif
    }
    if (state == 1) {
#if defined(WM_922AC_MUTANT_STATE1_STEP)
        ac_sw(slot + 0x50u, ac_lw(slot + 0x50u) - 0x800u);
#else
        ac_sw(slot + 0x50u, ac_lw(slot + 0x50u) - 0x1000u);
#endif
        scaled = ac_s32(ac_lw(slot + 0x50u)) >> 12;
#if !defined(WM_922AC_MUTANT_SKIP_PUBLISH)
        ac_sw(AC_BE0C, (u32)scaled);
#endif
#if defined(WM_922AC_MUTANT_STATE1_THRESH)
        if (scaled < 0x70)
#else
        if (scaled < 0x78)
#endif
        {
#if defined(WM_922AC_MUTANT_WRONG_CLAMP_LO)
            ac_sw(AC_BE0C, 0x70u);
            ac_sw(slot + 0x50u, 0x00070000u);
#else
            ac_sw(AC_BE0C, 0x78u);
            ac_sw(slot + 0x50u, 0x00078000u);
#endif
#if !defined(WM_922AC_MUTANT_SKIP_CLEAR_STATE)
            ac_sh(slot + 0x20u, 0);
#endif
        }
        return ret;
    }
    if (state == 2) {
        ac_sw(slot + 0x50u, ac_lw(slot + 0x50u) + 0x1000u);
        scaled = ac_s32(ac_lw(slot + 0x50u)) >> 12;
#if !defined(WM_922AC_MUTANT_SKIP_PUBLISH)
#if defined(WM_922AC_MUTANT_WRONG_GLOBAL)
        ac_sw(0x8009BE10u, (u32)scaled);
#else
        ac_sw(AC_BE0C, (u32)scaled);
#endif
#endif
#if defined(WM_922AC_MUTANT_STATE2_THRESH)
        if (scaled < 0x80)
#else
        if (scaled < 0x8C)
#endif
            return ret;
        ac_sw(AC_BE0C, 0x8Cu);
        ac_sw(slot + 0x50u, 0x0008C000u);
#if !defined(WM_922AC_MUTANT_SKIP_CLEAR_STATE)
        ac_sh(slot + 0x20u, 0);
#endif
        return ret;
    }
    return ret;
}
