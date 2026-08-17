/*
 * World-map scheduler callback 0x8008D678 (slot-5/6 handler).
 *
 * Register-faithful transcription of retail world_map.bin
 * [0x8008D678, 0x8008DD6C).  See world_map_callback_8d678.h.
 *
 * Sub JT @0x800708D4, index = (s16)(lhu +4 - 1), sltiu 8:
 *   1 -> state 8 + neighbor pool+(lh+6)<<7
 *   2 -> 0x20; 3 -> 0x30; 4 -> 0x40 + follow=1
 *   5 -> 0x10; 8 -> 0x18; 6/7/OOR -> dispatch
 * JT1 @0x800708F4, 65 entries, sltiu 0x41:
 *   0x00,0x01 ring-follow or dormant anim-3
 *   0x02 slot-7 X/Z/heading (Y skipped)
 *   0x08 approach vs neighbor; falls into 0x09
 *   0x09 8BEC8 + 8C1DC(slot+0x28)
 *   0x0A handoff 97770(lh+6,4) -> state 2
 *   0x10 ratan2 toward slot-4; falls into 0x11
 *   0x11 8BEC8 advance
 *   0x12 97770(4,7) -> state 1
 *   0x18 89160 / timer-8, or 0x1A if byte==7
 *   0x19 timer dec; 0x1A timer dec -> anim 3, state 2
 *   0x20 idle anim 0; 0x21 97770(slot-3,2)
 *   0x22 state 0
 *   0x30 8DFF4 + facing *96; 0x31 8BEC8; 0x32 state 1
 *   parked -> common tail (47 of 65)
 */
#include <stdint.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_callback_8d678.h"
#include "world_map_common_tail.h"
#include "world_map_helper_74794.h"
#include "world_map_helper_8bec8.h"
#include "world_map_helper_8c1dc.h"
#include "world_map_helper_8dff4.h"
#include "world_map_helper_941c4.h"
#include "world_map_helper_97770.h"

extern void func_800245D8(void* object, s16 animation);
extern void wm_800894C8(u32 record_index);
extern long rcos(long a);
extern long rsin(long a);
extern long ratan2(long y, long x);

#define D678_POOL_PTR    0x8009BE24u
#define D678_MODE_FLAG   0x8009BE10u
#define D678_RING_INDEX  0x8009D154u
#define D678_RING_BASE   0x8009CEC4u
#define D678_FOLLOW_BASE 0x8006F8E1u
#define D678_SLOT_BYTE   0x8006F364u
#define D678_PUB_XZ      0x8006EF90u
#define D678_PUB_HEAD    0x8006EE52u
#define D678_SCRATCH     0x1F800000u

#if defined(WM_8D678_TEST_TRACE)
extern void wm_8d678_test_store(u32 address, u32 width, u32 value);
extern void wm_8d678_test_245d8(u32 object_bits, s16 animation);
extern void wm_8d678_test_894c8(u32 record_index);
extern void wm_8d678_test_8c1dc(u32 ctx, u32 obj, u32 out);
extern s32 wm_8d678_test_941c4(u32 a, u32 b, u32 out, u32 angle);
extern s32 wm_8d678_test_8bec8(u32 obj);
extern s32 wm_8d678_test_97770(u32 slot, s32 value);
extern void wm_8d678_test_8dff4(u32 out);
extern void wm_8d678_test_89160(u32 a0, u32 a1, u32 a2);
extern long wm_8d678_test_rcos(long a);
extern long wm_8d678_test_rsin(long a);
extern long wm_8d678_test_ratan2(long y, long x);
extern void wm_8d678_test_74794(s32 tag, u32 pose);
#define D678_TRACE_STORE(a, w, v) wm_8d678_test_store((a), (w), (v))
#define D678_CALL_245D8(o, n)     wm_8d678_test_245d8((o), (n))
#define D678_CALL_894C8(i)        wm_8d678_test_894c8(i)
#define D678_CALL_8C1DC(c, o, u)  wm_8d678_test_8c1dc((c), (o), (u))
#define D678_CALL_941C4(a, b, o, g) wm_8d678_test_941c4((a), (b), (o), (g))
#define D678_CALL_8BEC8(o)        wm_8d678_test_8bec8(o)
#define D678_CALL_97770(s, v)     wm_8d678_test_97770((s), (v))
#define D678_CALL_8DFF4(o)        wm_8d678_test_8dff4(o)
#define D678_CALL_89160(a, b, c)  wm_8d678_test_89160((a), (b), (c))
#define D678_CALL_RCOS(a)         wm_8d678_test_rcos(a)
#define D678_CALL_RSIN(a)         wm_8d678_test_rsin(a)
#define D678_CALL_RATAN2(y, x)    wm_8d678_test_ratan2((y), (x))
#define D678_CALL_74794(t, p)     wm_8d678_test_74794((t), (p))
#else
static void wm_8d678_native_245d8(u32 object_bits, s16 animation)
{
    func_800245D8((void*)(uintptr_t)object_bits, animation);
}
#define D678_TRACE_STORE(a, w, v) ((void)0)
#define D678_CALL_245D8(o, n)     wm_8d678_native_245d8((o), (n))
#define D678_CALL_894C8(i)        wm_800894C8(i)
#define D678_CALL_8C1DC(c, o, u)  wm_8008C1DC((c), (o), (u))
#define D678_CALL_941C4(a, b, o, g) wm_800941C4((a), (b), (o), (g))
#define D678_CALL_8BEC8(o)        wm_8008BEC8(o)
#define D678_CALL_97770(s, v)     wm_80097770((s), (v))
#define D678_CALL_8DFF4(o)        wm_8008DFF4(o)
#define D678_CALL_89160(a, b, c)  wm_80089160((a), (b), (c))
#define D678_CALL_RCOS(a)         rcos(a)
#define D678_CALL_RSIN(a)         rsin(a)
#define D678_CALL_RATAN2(y, x)    ratan2((y), (x))
#define D678_CALL_74794(t, p)     wm_80074794((t), (p))
#endif

static u32 d678_lw(u32 a)
{
    u32 v;

    memcpy(&v, PSX_ADDR(a), 4);
    return v;
}

static void d678_sw(u32 a, u32 v)
{
    memcpy(PSX_ADDR(a), &v, 4);
    D678_TRACE_STORE(a, 4u, v);
}

static u16 d678_lhu(u32 a)
{
    u16 v;

    memcpy(&v, PSX_ADDR(a), 2);
    return v;
}

static s16 d678_lh(u32 a)
{
    s16 v;

    memcpy(&v, PSX_ADDR(a), 2);
    return v;
}

static void d678_sh(u32 a, u16 v)
{
    memcpy(PSX_ADDR(a), &v, 2);
    D678_TRACE_STORE(a, 2u, v);
}

static u8 d678_lbu(u32 a)
{
    u8 v;

    memcpy(&v, PSX_ADDR(a), 1);
    return v;
}

static void d678_sb(u32 a, u8 v) __attribute__((unused));
static void d678_sb(u32 a, u8 v)
{
    memcpy(PSX_ADDR(a), &v, 1);
    D678_TRACE_STORE(a, 1u, v);
}

static u32 d678_sra(u32 b, u32 amt)
{
    u32 v = b >> amt;

    if (b & 0x80000000u)
        v |= UINT32_MAX << (32u - amt);
    return v;
}

static s32 d678_sign16(u32 v)
{
    return (s32)(s16)(u16)(v & 0xFFFFu);
}

static s8 d678_af(u32 obj)
{
    return *((s8*)PSX_ADDR(obj) + 0xAF);
}

s32 wm_8008D678(s32 slot_idx)
{
    u32 pool = d678_lw(D678_POOL_PTR);
    u32 slot = pool + (u32)slot_idx * 128u;
    u32 sc = D678_SCRATCH;
    u32 neighbor = 0u;
    u32 rec = (u32)slot_idx + 0x28u;
    s32 state;
    s32 sub;

    sub = d678_sign16((u32)d678_lhu(slot + 4u) - 1u);
    if ((u32)sub < 8u) {
        switch (sub) {
        case 0: /* raw +4 == 1 */
            d678_sh(slot + 0x20u, 8u);
            neighbor = pool + ((u32)d678_lh(slot + 6u) << 7);
            d678_sh(slot + 4u, 0u);
            break;
        case 1:
            d678_sh(slot + 4u, 0u);
            d678_sh(slot + 0x20u, 0x20u);
            break;
        case 2:
            d678_sh(slot + 4u, 0u);
            d678_sh(slot + 0x20u, 0x30u);
            break;
        case 3:
#if defined(WM_8D678_MUTANT_SUBSTATE_POLARITY)
            d678_sh(slot + 4u, 0u);
            d678_sh(slot + 0x20u, 8u);
#else
            d678_sh(slot + 0x20u, 0x40u);
            d678_sh(slot + 4u, 0u);
            d678_sb(D678_FOLLOW_BASE + (u32)slot_idx, 1u);
#endif
            break;
        case 4:
            d678_sh(slot + 4u, 0u);
            d678_sh(slot + 0x20u, 0x10u);
            break;
        case 7:
            d678_sh(slot + 4u, 0u);
            d678_sh(slot + 0x20u, 0x18u);
            break;
        default:
            break;
        }
    }

    state = (s32)d678_lh(slot + 0x20u);
#if defined(WM_8D678_MUTANT_JT1_GUARD)
    if ((u32)state >= 0x2Du)
#else
    if ((u32)state >= 0x41u)
#endif
        goto common_tail;

    switch (state) {
    case 0x00:
    case 0x01: {
        u8 flag;

#if defined(WM_8D678_MUTANT_WRONG_FOLLOW_FLAG)
        flag = d678_lbu(0x8006F8E4u);
#else
        flag = d678_lbu(D678_FOLLOW_BASE + (u32)slot_idx);
#endif
        if (flag == 1u) {
            u32 ring_i = (u32)d678_lh(D678_RING_INDEX);
            u32 lag = d678_lw(slot + 0x58u);
#if defined(WM_8D678_MUTANT_WRONG_RING_MASK)
            u32 idx = (ring_i - lag) & 0x3Fu;
#else
            u32 idx = (ring_i - lag) & 0x1Fu;
#endif
            u32 entry = D678_RING_BASE + idx * 0x14u;
            u32 eq_x = (d678_lw(slot + 0x28u) ^ d678_lw(entry)) < 1u;
            u32 eq_y = (d678_lw(slot + 0x2Cu) ^ d678_lw(entry + 4u)) < 1u;
            u32 eq_z = (d678_lw(slot + 0x30u) ^ d678_lw(entry + 8u)) < 1u;
            u32 obj = d678_lw(slot + 0x4Cu);

            if ((eq_x & eq_y & eq_z) != 0u) {
                if (d678_af(obj) != 0) {
                    D678_CALL_245D8(obj, (s16)0);
#if defined(WM_8D678_MUTANT_WRONG_RECORD)
                    D678_CALL_894C8(0x2Cu);
#else
                    D678_CALL_894C8(rec);
#endif
                }
            } else {
                if (d678_af(obj) != 1)
                    D678_CALL_245D8(obj, (s16)1);
#if defined(WM_8D678_MUTANT_WRONG_RECORD)
                D678_CALL_8C1DC(0x2Cu, slot, sc);
#else
                D678_CALL_8C1DC(rec, slot, sc);
#endif
            }
#if !defined(WM_8D678_MUTANT_FOLLOW_SKIP_COPY)
            d678_sw(slot + 0x28u, d678_lw(entry));
            d678_sw(slot + 0x2Cu, d678_lw(entry + 4u));
            d678_sw(slot + 0x30u, d678_lw(entry + 8u));
            d678_sh(slot + 0x48u, d678_lhu(entry + 0x10u));
#endif
            goto common_tail;
        }
        {
            u32 obj = d678_lw(slot + 0x4Cu);

            if (d678_af(obj) != 3) {
                D678_CALL_245D8(obj, (s16)3);
#if defined(WM_8D678_MUTANT_WRONG_RECORD)
                D678_CALL_894C8(0x2Cu);
#else
                D678_CALL_894C8(rec);
#endif
            }
            goto common_tail;
        }
    }

    case 0x02:
        d678_sw(slot + 0x28u, d678_lw(pool + 0x3A8u));
#if defined(WM_8D678_MUTANT_STATE2_COPY_Y)
        d678_sw(slot + 0x2Cu, d678_lw(pool + 0x3ACu));
#endif
        d678_sw(slot + 0x30u, d678_lw(pool + 0x3B0u));
        d678_sh(slot + 0x48u, d678_lhu(pool + 0x3C8u));
        goto common_tail;

    case 0x08:
        (void)D678_CALL_941C4(slot + 0x28u, neighbor + 0x28u,
                              slot + 0x38u, slot + 0x48u);
        d678_sw(slot + 0x50u, d678_sra(d678_lw(neighbor + 0x28u), 12u));
        d678_sw(slot + 0x54u, d678_sra(d678_lw(neighbor + 0x30u), 12u));
        D678_CALL_245D8(d678_lw(slot + 0x4Cu), (s16)1);
        state = (s32)d678_lhu(slot + 0x20u) + 1;
        d678_sh(slot + 0x20u, (u16)state);
        /* fall into 0x09 */
        /* fallthrough */
    case 0x09:
        if (D678_CALL_8BEC8(slot) == 3) {
#if !defined(WM_8D678_MUTANT_SKIP_8BEC8_ADVANCE)
            state = (s32)d678_lhu(slot + 0x20u) + 1;
            d678_sh(slot + 0x20u, (u16)state);
#endif
        }
#if defined(WM_8D678_MUTANT_WRONG_RECORD)
        D678_CALL_8C1DC(0x2Cu, slot, sc);
#else
        D678_CALL_8C1DC(rec, slot, sc);
#endif
        goto common_tail;

    case 0x0A:
#if defined(WM_8D678_MUTANT_WRONG_97770_ARGS)
        if (D678_CALL_97770((u32)d678_lh(slot + 6u), 1) == 0)
#else
        if (D678_CALL_97770((u32)d678_lh(slot + 6u), 4) == 0)
#endif
            goto common_tail;
        d678_sh(slot + 0x24u, 1u);
#if defined(WM_8D678_MUTANT_WRONG_HANDOFF_STATE)
        d678_sh(slot + 0x20u, 1u);
#else
        d678_sh(slot + 0x20u, 2u);
#endif
        D678_CALL_894C8(rec);
        goto common_tail;

    case 0x10: {
        u32 tx = d678_lw(pool + 0x228u);
        u32 tz = d678_lw(pool + 0x230u);
        s32 dx = (s32)tx - (s32)d678_lw(slot + 0x28u);
        s32 dz = (s32)tz - (s32)d678_lw(slot + 0x30u);
        long ang;
        long c;
        long s;

        d678_sw(slot + 0x50u, tx);
        d678_sw(slot + 0x54u, tz);
        d678_sw(sc + 0u, (u32)dx);
        d678_sw(sc + 8u, (u32)dz);
#if defined(WM_8D678_MUTANT_RATAN2_SWAP)
        ang = D678_CALL_RATAN2((long)dx, (long)dz);
#else
        ang = D678_CALL_RATAN2((long)dz, (long)dx);
#endif
        ang = (long)((u32)(ang + 0x400) & 0xFFFu);
        d678_sh(slot + 0x48u, (u16)ang);
        c = D678_CALL_RCOS(ang);
        s = D678_CALL_RSIN((long)d678_lh(slot + 0x48u));
        d678_sw(slot + 0x38u, (u32)c);
#if defined(WM_8D678_MUTANT_SKIP_RSIN_NEG)
        d678_sw(slot + 0x40u, (u32)s);
#else
        d678_sw(slot + 0x40u, 0u - (u32)s);
#endif
        d678_sw(slot + 0x50u, d678_sra(d678_lw(slot + 0x50u), 12u));
        d678_sw(slot + 0x54u, d678_sra(d678_lw(slot + 0x54u), 12u));
        D678_CALL_245D8(d678_lw(slot + 0x4Cu), (s16)1);
        state = (s32)d678_lhu(slot + 0x20u) + 1;
        d678_sh(slot + 0x20u, (u16)state);
        /* fall into 0x11 */
    }
        /* fallthrough */
    case 0x11:
        if (D678_CALL_8BEC8(slot) != 3)
            goto common_tail;
        state = (s32)d678_lhu(slot + 0x20u) + 1;
        goto set_state;

    case 0x12:
        if (D678_CALL_97770(4u, 7) == 0)
            goto common_tail;
        state = 1;
        goto set_state;

    case 0x18:
        if (d678_lbu(D678_SLOT_BYTE + (u32)slot_idx) == 7u) {
            d678_sh(slot + 0x22u, 1u);
            state = 0x1A;
            goto set_state;
        }
        d678_sh(sc + 0xA0u, (u16)d678_sra(d678_lw(slot + 0x28u), 12u));
        d678_sh(sc + 0xA2u, (u16)d678_sra(d678_lw(slot + 0x2Cu), 12u));
        d678_sh(sc + 0xA4u, (u16)d678_sra(d678_lw(slot + 0x30u), 12u));
        D678_CALL_89160((u32)slot_idx + 1u, sc + 0xA0u, 0u);
        d678_sh(slot + 0x22u, 8u);
        state = (s32)d678_lhu(slot + 0x20u) + 1;
        goto set_state;

    case 0x19:
        state = (s32)d678_lhu(slot + 0x22u) - 1;
        d678_sh(slot + 0x22u, (u16)state);
        if (d678_sign16((u32)state) > 0)
            goto common_tail;
        d678_sh(slot + 0x24u, 1u);
        d678_sh(slot + 0x22u, 0x10u);
        state = (s32)d678_lhu(slot + 0x20u) + 1;
        goto set_state;

    case 0x1A:
        state = (s32)d678_lhu(slot + 0x22u) - 1;
        d678_sh(slot + 0x22u, (u16)state);
        if (d678_sign16((u32)state) > 0)
            goto common_tail;
        D678_CALL_245D8(d678_lw(slot + 0x4Cu), (s16)3);
        d678_sh(slot + 0x24u, 1u);
        state = 2;
        goto set_state;

    case 0x20:
        D678_CALL_245D8(d678_lw(slot + 0x4Cu), (s16)0);
        d678_sw(slot + 0x40u, 0u);
        d678_sw(slot + 0x3Cu, 0u);
        d678_sw(slot + 0x38u, 0u);
        state = (s32)d678_lhu(slot + 0x20u) + 1;
        goto set_state;

    case 0x21:
        if (D678_CALL_97770((u32)slot_idx - 3u, 2) == 0)
            goto common_tail;
        d678_sh(pool + ((u32)slot_idx << 7) - 0x17Au, (u16)slot_idx);
        state = (s32)d678_lhu(slot + 0x20u) + 1;
        goto set_state;

    case 0x22:
        d678_sh(slot + 0x20u, 0u);
        goto common_tail;

    case 0x30: {
        long c;
        long s;
        u32 head_raw;

        D678_CALL_8DFF4(slot + 0x28u);
        d678_sh(slot + 0x24u, 0u);
        head_raw = d678_lw(d678_lw(D678_POOL_PTR) + 0x3F8u);
        d678_sh(slot + 0x48u, (u16)d678_sign16(head_raw));
        c = D678_CALL_RCOS((long)d678_sign16(head_raw));
        d678_sw(sc + 0u, d678_lw(slot + 0x28u) + (u32)c * 96u);
        s = D678_CALL_RSIN((long)d678_lh(slot + 0x48u));
        d678_sw(sc + 8u, d678_lw(slot + 0x30u) + (0u - (u32)s) * 96u);
        (void)D678_CALL_941C4(slot + 0x28u, sc, slot + 0x38u, slot + 0x48u);
        d678_sw(slot + 0x50u, d678_sra(d678_lw(sc + 0u), 12u));
        d678_sw(slot + 0x54u, d678_sra(d678_lw(sc + 8u), 12u));
        D678_CALL_245D8(d678_lw(slot + 0x4Cu), (s16)1);
        state = (s32)d678_lhu(slot + 0x20u) + 1;
        goto set_state;
    }

    case 0x31:
        if (D678_CALL_8BEC8(slot) != 3)
            goto common_tail;
        D678_CALL_245D8(d678_lw(slot + 0x4Cu), (s16)0);
        d678_sw(slot + 0x40u, 0u);
        d678_sw(slot + 0x3Cu, 0u);
        d678_sw(slot + 0x38u, 0u);
        state = (s32)d678_lhu(slot + 0x20u) + 1;
        goto set_state;

    case 0x32:
#if defined(WM_8D678_MUTANT_STATE32_TARGET)
        state = 0;
#else
        state = 1;
#endif
        goto set_state;

    default:
        goto common_tail;
    }

set_state:
    d678_sh(slot + 0x20u, (u16)state);
    /* fallthrough */
common_tail:
    {
        s32 st = (s32)d678_lh(slot + 0x20u);
        u32 mode = d678_lw(D678_MODE_FLAG);
        int call_74794;

#if defined(WM_8D678_MUTANT_74794_GATE)
        (void)mode;
        call_74794 = (st == 2);
#else
        if (st == 2)
            call_74794 = 0;
        else if (mode == 2u)
            call_74794 = 1;
        else if (d678_lbu(D678_SLOT_BYTE + (u32)slot_idx) == 7u)
            call_74794 = 0;
        else
            call_74794 = 1;
#endif
        if (call_74794)
            D678_CALL_74794(1, slot + 0x28u);
    }
#if !defined(WM_8D678_MUTANT_SKIP_PUBLISH)
    {
#if defined(WM_8D678_MUTANT_PUBLISH_SLOT0)
        u32 xz_off = 0u;
        u32 h_off = 0u;
#else
        u32 xz_off = (u32)(slot_idx - 4) * 6u;
        u32 h_off = (u32)slot_idx * 2u;
#endif
        d678_sh(D678_PUB_XZ + xz_off,
                (u16)d678_sra(d678_lw(slot + 0x28u), 12u));
        d678_sh(D678_PUB_XZ + 2u + xz_off,
                (u16)d678_sra(d678_lw(slot + 0x30u), 12u));
        d678_sh(D678_PUB_HEAD + h_off, d678_lhu(slot + 0x48u));
    }
#endif
#if defined(WM_8D678_MUTANT_WRONG_RETURN)
    return d678_lh(slot + 0x20u);
#else
    return 1;
#endif
}
