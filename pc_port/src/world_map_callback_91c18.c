/*
 * World-map scheduler callback 0x80091C18 (slot-10 cb1).
 *
 * Register-faithful transcription of retail world_map.bin
 * [0x80091C18, 0x80091FF8).  See world_map_callback_91c18.h.
 */
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_callback_91c18.h"
#include "world_map_helper_91ff8.h"
#include "world_map_helper_96f18.h"

#define C18_POOL    0x8009BE24u
#define C18_D144    0x8009D144u
#define C18_D3F0    0x8009D3F0u
#define C18_B214    0x8009B214u
#define C18_B224    0x8009B224u
#define C18_B22C    0x8009B22Cu
#define C18_B234    0x8009B234u
#define C18_B23C    0x8009B23Cu
#define C18_BD38    0x8009BD38u
#define C18_BD40    0x8009BD40u
#define C18_BD48    0x8009BD48u
#define C18_BD4A    0x8009BD4Au
#define C18_BD4C    0x8009BD4Cu
#define C18_BE28    0x8009BE28u
#define C18_BE2C    0x8009BE2Cu

#if defined(WM_91C18_TEST_TRACE)
extern void wm_91c18_test_store(u32 address, u32 width, u32 value);
extern s32 wm_91c18_test_91ff8(s32 threshold, u32 headings, u32 heights);
extern void wm_91c18_test_96f18(u32 dest, u32 pose, s32 scale, u32 angles);
#define C18_TRACE_STORE(a, w, v) wm_91c18_test_store((a), (w), (v))
#define C18_CALL_91FF8(t, h, y)  wm_91c18_test_91ff8((t), (h), (y))
#define C18_CALL_96F18(d, p, s, a) wm_91c18_test_96f18((d), (p), (s), (a))
#else
#define C18_TRACE_STORE(a, w, v) ((void)0)
#define C18_CALL_91FF8(t, h, y)  wm_80091FF8((t), (h), (y))
#define C18_CALL_96F18(d, p, s, a) wm_80096F18((d), (p), (s), (a))
#endif

static u32 c18_lw(u32 a)
{
    u32 v;

    memcpy(&v, PSX_ADDR(a), 4);
    return v;
}

static void c18_sw(u32 a, u32 v)
{
    memcpy(PSX_ADDR(a), &v, 4);
    C18_TRACE_STORE(a, 4u, v);
}

static s16 c18_lh(u32 a)
{
    s16 v;

    memcpy(&v, PSX_ADDR(a), 2);
    return v;
}

static u16 c18_lhu(u32 a)
{
    u16 v;

    memcpy(&v, PSX_ADDR(a), 2);
    return v;
}

static void c18_sh(u32 a, u16 v)
{
    memcpy(PSX_ADDR(a), &v, 2);
    C18_TRACE_STORE(a, 2u, v);
}

static s32 c18_s32(u32 v)
{
    s32 r;

    memcpy(&r, &v, 4);
    return r;
}

static u32 c18_bits(s32 v)
{
    u32 r;

    memcpy(&r, &v, 4);
    return r;
}

static s32 c18_sll(s32 v, u32 sh)
{
    return c18_s32(c18_bits(v) << sh);
}

static void c18_install_tables(u32 slot, u32 t64, u32 t68)
{
    u32 cur = c18_lw(slot + 0x50u);

    c18_sw(slot + 0x64u, t64);
    c18_sh(slot + 4u, 0);
    c18_sw(slot + 0x68u, t68);
    if (cur == 0u)
        c18_sw(slot + 0x50u, 1u);
}

static void c18_apply_probe_result(u32 slot, u32 headings, s32 result)
{
    s32 idx = result;

    c18_sw(slot + 0x50u, (u32)idx);
    c18_sw(slot + 0x54u, c18_lw(C18_B214 + (u32)idx * 4u));
    c18_sw(slot + 0x58u, c18_bits(c18_lh(headings + (u32)idx * 2u)));
}

static void c18_call_96f18(void)
{
#if !defined(WM_91C18_MUTANT_SKIP_96F18)
    C18_CALL_96F18(C18_BD40, C18_BE28, c18_s32(c18_lw(C18_D3F0)), C18_BD38);
#endif
}

s32 wm_80091C18(s32 slot_idx)
{
#if defined(WM_91C18_MUTANT_SKIP_COUNTER)
    if (0)
        (void)c18_lhu(0);
#endif
#if defined(WM_91C18_MUTANT_SKIP_WORD_TABLE)
    if (0)
        c18_apply_probe_result(0, 0, 0);
#endif
    u32 pool = c18_lw(C18_POOL);
    u32 slot = pool + ((u32)slot_idx << 7);
    s16 sub = c18_lh(slot + 4u);
    s16 state;
    u32 headings;
    s32 do_96 = 1;

    if (sub == 9) {
#if defined(WM_91C18_MUTANT_SUB9_TABLE)
        c18_install_tables(slot, C18_B224, C18_B234);
#else
        c18_install_tables(slot, C18_B22C, C18_B23C);
#endif
    } else if (sub == 0xA) {
        c18_install_tables(slot, C18_B224, C18_B234);
    } else if (sub == 0xE) {
        c18_sh(slot + 4u, 0);
        c18_sh(slot + 0x20u, 0);
        c18_sw(C18_D144, 0u);
    } else if (sub == 0x11) {
#if defined(WM_91C18_MUTANT_SUB11_D3F0)
        c18_sw(C18_D3F0, 0x40u);
#else
        c18_sw(C18_D3F0, 0x00400000u);
#endif
        c18_sh(slot + 0x20u, 3);
        c18_sh(slot + 4u, 0);
        c18_sw(slot + 0x58u, (u32)(s32)-0x100);
    }

    state = c18_lh(slot + 0x20u);
    headings = c18_lw(slot + 0x64u);

    if (state == 0) {
        s32 got;

#if !defined(WM_91C18_MUTANT_SKIP_91FF8)
        got = C18_CALL_91FF8(c18_s32(c18_lw(slot + 0x50u)), headings,
                             c18_lw(slot + 0x68u));
#else
        got = c18_s32(c18_lw(slot + 0x50u));
#endif
        if (c18_s32(c18_lw(slot + 0x50u)) != got) {
            c18_sh(slot + 0x20u, 1);
            c18_sh(slot + 0x22u, 0);
#if !defined(WM_91C18_MUTANT_SKIP_WORD_TABLE)
            c18_apply_probe_result(slot, headings, got);
#else
            c18_sw(slot + 0x50u, (u32)got);
#endif
            c18_sw(slot + 0x60u, c18_bits(c18_sll((s32)c18_lh(C18_BD38), 12u)));
        }
        c18_call_96f18();
    } else if (state == 1) {
        s32 ctr = (s32)c18_lh(slot + 0x22u);

#if defined(WM_91C18_MUTANT_COUNTER_GE4)
        if (ctr >= 4)
#else
        if (ctr >= 5)
#endif
        {
            s32 got;

#if !defined(WM_91C18_MUTANT_SKIP_91FF8)
            got = C18_CALL_91FF8(c18_s32(c18_lw(slot + 0x50u)), headings,
                                 c18_lw(slot + 0x68u));
#else
            got = c18_s32(c18_lw(slot + 0x50u));
#endif
            if (c18_s32(c18_lw(slot + 0x50u)) != got) {
#if !defined(WM_91C18_MUTANT_SKIP_WORD_TABLE)
                c18_apply_probe_result(slot, headings, got);
#else
                c18_sw(slot + 0x50u, (u32)got);
#endif
            }
            c18_sh(slot + 0x22u, 0);
        }

        {
            s32 target = c18_s32(c18_lw(slot + 0x54u));
            s32 cur = c18_s32(c18_lw(C18_D3F0));

            if (target != cur) {
                s32 d = target - cur;

                if (d <= 0) {
#if defined(WM_91C18_MUTANT_STATE1_NEG_STEP)
                    c18_sw(C18_D3F0, c18_bits(cur - 0x1000));
#else
                    c18_sw(C18_D3F0, c18_bits(cur - 0x2000));
#endif
                } else {
                    s32 step = d;

                    if (0x40000 < step)
                        step = 0x40000;
#if defined(WM_91C18_MUTANT_STATE1_SRA)
                    step = c18_s32(c18_bits(step) >> 2);
#else
                    step = c18_s32(c18_bits(step) >> 3);
#endif
                    if (step < 0x40)
                        c18_sw(C18_D3F0, (u32)target);
                    else
                        c18_sw(C18_D3F0, c18_bits(cur + step));
                }
            }
        }

        {
            s32 hd = (s32)c18_lh(C18_BD38);
            s32 want = c18_s32(c18_lw(slot + 0x58u));

            if (want != hd) {
                s32 delta = want - hd;
                s32 shifted = c18_sll(delta, 12u);
                s32 acc = c18_s32(c18_lw(slot + 0x60u));

                if (shifted < 0)
                    acc += c18_s32(c18_bits(shifted) >> 5);
                else if (c18_lw(slot + 0x50u) == 0u) {
#if defined(WM_91C18_MUTANT_HEADING_POS_CONST)
                    acc += 0x799;
#else
                    acc += 0xF32;
#endif
                } else {
#if defined(WM_91C18_MUTANT_HEADING_POS_CONST)
                    acc += 0xF32;
#else
                    acc += 0x799;
#endif
                }
                c18_sw(slot + 0x60u, c18_bits(acc));
                c18_sh(C18_BD38, (u16)(c18_s32(c18_lw(slot + 0x60u)) >> 12));
            }
        }

        if (c18_lw(slot + 0x54u) == c18_lw(C18_D3F0) &&
            c18_s32(c18_lw(slot + 0x58u)) == (s32)c18_lh(C18_BD38)) {
#if !defined(WM_91C18_MUTANT_ARRIVE_SKIP_CLEAR)
            c18_sh(slot + 0x20u, 0);
#endif
        }
        c18_call_96f18();
    } else if (state == 2) {
#if !defined(WM_91C18_MUTANT_STATE2_SKIP)
        c18_sh(C18_BD48, 0);
        c18_sh(C18_BD4C, 0);
        c18_sh(C18_BD4A, (u16)(c18_s32(c18_lw(C18_BE2C)) >> 12));
#endif
        do_96 = 0;
    } else if (state == 3) {
        s32 hd = (s32)c18_lh(C18_BD38);
        s32 want = c18_s32(c18_lw(slot + 0x58u));

        if (want != hd) {
#if defined(WM_91C18_MUTANT_STATE3_STEP)
            c18_sh(C18_BD38, (u16)(hd + 1));
#else
            c18_sh(C18_BD38, (u16)(hd + 2));
#endif
        }
        c18_call_96f18();
    } else {
        do_96 = 0;
    }

    (void)do_96;
#if !defined(WM_91C18_MUTANT_SKIP_COUNTER)
    c18_sh(slot + 0x22u, (u16)(c18_lhu(slot + 0x22u) + 1u));
#endif
#if defined(WM_91C18_MUTANT_WRONG_RETURN)
    return 0;
#else
    return 1;
#endif
}
