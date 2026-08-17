/*
 * World-map scheduler callback 0x8008B644 (slot-2/3 companion handler).
 *
 * Register-faithful transcription of retail world_map.bin
 * [0x8008B644, 0x8008BB40).  See world_map_callback_8b644.h and
 * docs/evidence/w34b24-pre4-8b644/.
 *
 * JT1 @0x80070670, 65 entries (index = lh slot[+0x20], sltiu 0x41,
 * out-of-range -> common tail).  Retail slot->target map:
 *   0x00,0x01 -> 0x8008B718  follow ring or copy pool+0x180+(slot<<7)
 *   0x02      -> 0x8008B880  follow slot-7 record copy
 *   0x08      -> 0x8008B8BC  approach setup vs neighbor, falls into 0x09
 *   0x09      -> 0x8008B904  approach step + 8C1DC
 *   0x0A      -> 0x8008B93C  handoff 97770(lh+6,4)
 *   0x28      -> 0x8008B96C  warp-in; joins 0x30 anim/8BEC8 tail
 *   0x29,0x31 -> 0x8008BABC  approach step
 *   0x2A      -> 0x8008BA48  clear follow flag, state=0x40
 *   0x30      -> 0x8008BA60  approach vs pool+0xA8
 *   0x32      -> 0x8008BAE4  97770(1,6) -> state 0
 *   parked    -> 0x8008BAFC (common tail)
 */
#include <stdint.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_callback_8b644.h"
#include "world_map_common_tail.h"
#include "world_map_helper_74794.h"
#include "world_map_helper_8bec8.h"
#include "world_map_helper_8c1dc.h"
#include "world_map_helper_941c4.h"
#include "world_map_helper_97770.h"

extern void func_800245D8(void* object, s16 animation);
extern void wm_800894C8(u32 record_index);
extern s32 wm_80093978(s32 x, s32 z);
extern long rcos(long a);
extern long rsin(long a);

#define B644_POOL_PTR     0x8009BE24u
#define B644_RING_INDEX   0x8009D154u
#define B644_RING_BASE    0x8009CEC4u
#define B644_FOLLOW_BASE  0x8006F8E4u
#define B644_WARP_XZ      0x8006EF8Au
#define B644_WARP_HEAD    0x8006EE58u
#define B644_SCRATCH      0x1F800000u
#define B644_SLOT7_X      0x3A8u
#define B644_PLAYER_X     0xA8u

#if defined(WM_8B644_TEST_TRACE)
extern void wm_8b644_test_store(u32 address, u32 width, u32 value);
extern void wm_8b644_test_245d8(u32 object_bits, s16 animation);
extern void wm_8b644_test_894c8(u32 record_index);
extern void wm_8b644_test_8c1dc(u32 ctx, u32 obj, u32 out);
extern s32 wm_8b644_test_941c4(u32 a, u32 b, u32 out, u32 angle);
extern s32 wm_8b644_test_8bec8(u32 obj);
extern s32 wm_8b644_test_97770(u32 slot, s32 value);
extern s32 wm_8b644_test_93978(s32 x, s32 z);
extern long wm_8b644_test_rcos(long a);
extern long wm_8b644_test_rsin(long a);
extern void wm_8b644_test_74794(s32 tag, u32 pose);
#define B644_TRACE_STORE(a, w, v) wm_8b644_test_store((a), (w), (v))
#define B644_CALL_245D8(o, n)     wm_8b644_test_245d8((o), (n))
#define B644_CALL_894C8(i)        wm_8b644_test_894c8(i)
#define B644_CALL_8C1DC(c, o, u)  wm_8b644_test_8c1dc((c), (o), (u))
#define B644_CALL_941C4(a, b, o, g) wm_8b644_test_941c4((a), (b), (o), (g))
#define B644_CALL_8BEC8(o)        wm_8b644_test_8bec8(o)
#define B644_CALL_97770(s, v)     wm_8b644_test_97770((s), (v))
#define B644_CALL_93978(x, z)     wm_8b644_test_93978((x), (z))
#define B644_CALL_RCOS(a)         wm_8b644_test_rcos(a)
#define B644_CALL_RSIN(a)         wm_8b644_test_rsin(a)
#define B644_CALL_74794(t, p)     wm_8b644_test_74794((t), (p))
#else
static void wm_8b644_native_245d8(u32 object_bits, s16 animation)
{
    func_800245D8((void*)(uintptr_t)object_bits, animation);
}
#define B644_TRACE_STORE(a, w, v) ((void)0)
#define B644_CALL_245D8(o, n)     wm_8b644_native_245d8((o), (n))
#define B644_CALL_894C8(i)        wm_800894C8(i)
#define B644_CALL_8C1DC(c, o, u)  wm_8008C1DC((c), (o), (u))
#define B644_CALL_941C4(a, b, o, g) wm_800941C4((a), (b), (o), (g))
#define B644_CALL_8BEC8(o)        wm_8008BEC8(o)
#define B644_CALL_97770(s, v)     wm_80097770((s), (v))
#define B644_CALL_93978(x, z)     wm_80093978((x), (z))
#define B644_CALL_RCOS(a)         rcos(a)
#define B644_CALL_RSIN(a)         rsin(a)
#define B644_CALL_74794(t, p)     wm_80074794((t), (p))
#endif

static u32 b644_lw(u32 a)
{
    u32 v;

    memcpy(&v, PSX_ADDR(a), 4);
    return v;
}

static void b644_sw(u32 a, u32 v)
{
    memcpy(PSX_ADDR(a), &v, 4);
    B644_TRACE_STORE(a, 4u, v);
}

static u16 b644_lhu(u32 a)
{
    u16 v;

    memcpy(&v, PSX_ADDR(a), 2);
    return v;
}

static s16 b644_lh(u32 a)
{
    s16 v;

    memcpy(&v, PSX_ADDR(a), 2);
    return v;
}

static void b644_sh(u32 a, u16 v)
{
    memcpy(PSX_ADDR(a), &v, 2);
    B644_TRACE_STORE(a, 2u, v);
}

static u8 b644_lbu(u32 a)
{
    u8 v;

    memcpy(&v, PSX_ADDR(a), 1);
    return v;
}

static void b644_sb(u32 a, u8 v)
{
    memcpy(PSX_ADDR(a), &v, 1);
    B644_TRACE_STORE(a, 1u, v);
}

static u32 b644_sra(u32 b, u32 amt)
{
    u32 v = b >> amt;

    if (b & 0x80000000u)
        v |= UINT32_MAX << (32u - amt);
    return v;
}

static s32 b644_sign16(u32 v)
{
    return (s32)(s16)(u16)(v & 0xFFFFu);
}

static s32 b644_s32_from_u32(u32 b)
{
    s32 v;

    memcpy(&v, &b, 4);
    return v;
}

static s8 b644_actor_af(u32 obj)
{
    return *((s8*)PSX_ADDR(obj) + 0xAF);
}

static void b644_copy_pos3(u32 dst, u32 src)
{
    b644_sw(dst + 0x28u, b644_lw(src + 0u));
    b644_sw(dst + 0x2Cu, b644_lw(src + 4u));
    b644_sw(dst + 0x30u, b644_lw(src + 8u));
}

static void b644_approach_step(u32 slot)
{
    s32 close = B644_CALL_8BEC8(slot);

#if defined(WM_8B644_MUTANT_SKIP_8BEC8_ADVANCE)
    (void)close;
#else
    if (close == 3) {
        u16 st = b644_lhu(slot + 0x20u);

        b644_sh(slot + 0x20u, (u16)(st + 1u));
    }
#endif
}

s32 wm_8008B644(s32 slot_idx)
{
    u32 pool = b644_lw(B644_POOL_PTR);
#if defined(WM_8B644_MUTANT_WRONG_SLOT_OFFSET)
    u32 slot = pool + (u32)slot_idx * 64u;
#else
    u32 slot = pool + (u32)slot_idx * 128u;
#endif
    u32 sc = B644_SCRATCH;
    u32 neighbor = 0u;
    u32 obj_id = (u32)slot_idx + 0x2Eu;
    s32 state;
    s32 sub = (s32)b644_lh(slot + 4u);

    if (sub == 2) {
#if defined(WM_8B644_MUTANT_SUBSTATE_POLARITY)
        b644_sh(slot + 4u, 0u);
        b644_sh(slot + 0x20u, 8u);
#else
        b644_sh(slot + 4u, 0u);
        b644_sh(slot + 0x20u, 0x28u);
#endif
    } else if (sub < 3) {
        if (sub == 1) {
#if defined(WM_8B644_MUTANT_SUBSTATE_POLARITY)
            b644_sh(slot + 4u, 0u);
            b644_sh(slot + 0x20u, 0x28u);
#else
            s32 nb = (s32)b644_lh(slot + 6u);

            b644_sh(slot + 4u, 0u);
            b644_sh(slot + 0x20u, 8u);
            neighbor = pool + ((u32)nb << 7);
#endif
        }
    } else if (sub == 3) {
        b644_sh(slot + 4u, 0u);
        b644_sh(slot + 0x20u, 1u);
    } else if (sub == 5) {
        b644_sh(slot + 4u, 0u);
        b644_sh(slot + 0x20u, 0x30u);
    }

    state = (s32)b644_lh(slot + 0x20u);

#if defined(WM_8B644_MUTANT_JT1_GUARD)
    if ((u32)state >= 0x2Du)
#else
    if ((u32)state >= 0x41u)
#endif
        goto common_tail;

    switch (state) {
    case 0x00:
    case 0x01: {
        u8 flag;
        u32 obj = b644_lw(slot + 0x4Cu);

#if defined(WM_8B644_MUTANT_WRONG_FOLLOW_FLAG)
        flag = b644_lbu(0x8006F8E5u);
#else
        flag = b644_lbu(B644_FOLLOW_BASE + (u32)slot_idx);
#endif
        if (flag != 0u) {
            u32 src = pool + 0x180u + ((u32)slot_idx << 7);

            b644_sw(slot + 0x28u, b644_lw(src + 0x28u));
            b644_sw(slot + 0x2Cu, b644_lw(src + 0x2Cu));
            b644_sw(slot + 0x30u, b644_lw(src + 0x30u));
            b644_sh(slot + 0x24u, 1u);
            b644_sh(slot + 0x48u, b644_lhu(src + 0x48u));
            B644_CALL_894C8(obj_id);
            goto common_tail;
        }
        {
            u32 ring_i = (u32)b644_lh(B644_RING_INDEX);
            u32 lag = b644_lw(slot + 0x58u);
#if defined(WM_8B644_MUTANT_WRONG_RING_MASK)
            u32 idx = (ring_i - lag) & 0x3Fu;
#else
            u32 idx = (ring_i - lag) & 0x1Fu;
#endif
            u32 entry = B644_RING_BASE + idx * 0x14u;
            u32 eq_x = (b644_lw(slot + 0x28u) ^ b644_lw(entry + 0u)) < 1u;
            u32 eq_y = (b644_lw(slot + 0x2Cu) ^ b644_lw(entry + 4u)) < 1u;
            u32 eq_z = (b644_lw(slot + 0x30u) ^ b644_lw(entry + 8u)) < 1u;

            if ((eq_x & eq_y & eq_z) != 0u) {
                if (b644_actor_af(obj) != 0) {
                    B644_CALL_245D8(obj, (s16)0);
                    B644_CALL_894C8(obj_id);
                }
            } else {
                if (b644_actor_af(obj) != 1)
                    B644_CALL_245D8(obj, (s16)1);
                B644_CALL_8C1DC(obj_id, slot, sc);
            }
            b644_copy_pos3(slot, entry);
            b644_sh(slot + 0x24u, 0u);
            b644_sh(slot + 0x48u, b644_lhu(entry + 0x10u));
            goto common_tail;
        }
    }

    case 0x02:
        b644_sw(slot + 0x28u, b644_lw(pool + B644_SLOT7_X));
        b644_sw(slot + 0x2Cu, b644_lw(pool + B644_SLOT7_X + 4u));
        b644_sw(slot + 0x30u, b644_lw(pool + B644_SLOT7_X + 8u));
        b644_sh(slot + 0x48u, b644_lhu(pool + 0x3C8u));
        goto common_tail;

    case 0x08: {
        u32 obj = b644_lw(slot + 0x4Cu);

#if !defined(WM_8B644_MUTANT_SKIP_941C4)
        (void)B644_CALL_941C4(slot + 0x28u, neighbor + 0x28u, slot + 0x38u,
                              slot + 0x48u);
#endif
        b644_sw(slot + 0x50u, b644_sra(b644_lw(neighbor + 0x28u), 12u));
        b644_sw(slot + 0x54u, b644_sra(b644_lw(neighbor + 0x30u), 12u));
        B644_CALL_245D8(obj, (s16)1);
        b644_sh(slot + 0x20u, (u16)(b644_lhu(slot + 0x20u) + 1u));
        /* Fall into state 9: 8BEC8 then 8C1DC. */
        b644_approach_step(slot);
        B644_CALL_8C1DC(obj_id, slot, sc);
        goto common_tail;
    }

    case 0x09:
        b644_approach_step(slot);
        B644_CALL_8C1DC(obj_id, slot, sc);
        goto common_tail;

    case 0x0A:
        if (B644_CALL_97770((u32)(s32)b644_lh(slot + 6u), 4) != 0) {
            b644_sh(slot + 0x24u, 1u);
#if defined(WM_8B644_MUTANT_WRONG_HANDOFF_STATE)
            b644_sh(slot + 0x20u, 1u);
#else
            b644_sh(slot + 0x20u, 2u);
#endif
            B644_CALL_894C8(obj_id);
        }
        goto common_tail;

    case 0x28: {
        u32 xz_off;
        u32 head;
        u32 obj = b644_lw(slot + 0x4Cu);
        long c;
        long s;
        u32 fx;
        u32 fz;

#if defined(WM_8B644_MUTANT_WRONG_WARP_STRIDE)
        xz_off = (u32)slot_idx * 4u;
#else
        xz_off = (u32)slot_idx * 6u;
#endif
        {
            u32 wx = (u32)b644_lhu(B644_WARP_XZ + xz_off);
            u32 wz = (u32)b644_lhu(B644_WARP_XZ + 2u + xz_off);

#if defined(WM_8B644_MUTANT_WRONG_WARP_SHIFT)
            b644_sw(slot + 0x28u, wx << 8);
            b644_sw(slot + 0x30u, wz << 8);
#else
            b644_sw(slot + 0x28u, wx << 12);
            b644_sw(slot + 0x30u, wz << 12);
#endif
            b644_sw(slot + 0x2Cu,
                    (u32)B644_CALL_93978(
                        b644_s32_from_u32(b644_lw(slot + 0x28u)),
                        b644_s32_from_u32(b644_lw(slot + 0x30u))));
        }
        head = (u32)b644_lhu(B644_WARP_HEAD + (u32)slot_idx * 2u);
        b644_sw(slot + 0x5Cu, head);
        b644_sh(slot + 0x48u, (u16)head);
        c = B644_CALL_RCOS((long)b644_sign16(head));
        fx = b644_lw(slot + 0x28u) + (u32)c * 48u;
        b644_sw(sc + 0u, fx);
        s = B644_CALL_RSIN((long)b644_sign16((u32)b644_lh(slot + 0x48u)));
#if defined(WM_8B644_MUTANT_SKIP_RSIN_NEG)
        fz = b644_lw(slot + 0x30u) + (u32)s * 48u;
#else
        fz = b644_lw(slot + 0x30u) + (0u - (u32)s) * 48u;
#endif
        b644_sw(sc + 8u, fz);
        (void)B644_CALL_941C4(slot + 0x28u, sc, slot + 0x38u, slot + 0x48u);
        b644_sw(slot + 0x50u, b644_sra(b644_lw(sc + 0u), 12u));
        b644_sh(slot + 0x24u, 0u);
        b644_sw(slot + 0x54u, b644_sra(b644_lw(sc + 8u), 12u));
        /* Join state 0x30's 245D8 / state++ / 8BEC8 tail (not its 941C4). */
        B644_CALL_245D8(obj, (s16)1);
        b644_sh(slot + 0x20u, (u16)(b644_lhu(slot + 0x20u) + 1u));
        b644_approach_step(slot);
        goto common_tail;
    }

    case 0x2A:
#if defined(WM_8B644_MUTANT_STATE2A_TARGET)
        b644_sb(B644_FOLLOW_BASE + (u32)slot_idx, 0u);
        b644_sh(slot + 0x20u, 0x41u);
#else
        b644_sb(B644_FOLLOW_BASE + (u32)slot_idx, 0u);
        b644_sh(slot + 0x20u, 0x40u);
#endif
        goto common_tail;

    case 0x30: {
        u32 obj = b644_lw(slot + 0x4Cu);

        (void)B644_CALL_941C4(slot + 0x28u, pool + B644_PLAYER_X, slot + 0x38u,
                              slot + 0x48u);
        b644_sw(slot + 0x50u, b644_sra(b644_lw(pool + B644_PLAYER_X), 12u));
        b644_sw(slot + 0x54u, b644_sra(b644_lw(pool + 0xB0u), 12u));
        B644_CALL_245D8(obj, (s16)1);
        b644_sh(slot + 0x20u, (u16)(b644_lhu(slot + 0x20u) + 1u));
        b644_approach_step(slot);
        goto common_tail;
    }

    case 0x29:
    case 0x31:
        b644_approach_step(slot);
        goto common_tail;

    case 0x32:
#if defined(WM_8B644_MUTANT_WRONG_97770_ARGS)
        if (B644_CALL_97770(6u, 1) != 0)
#else
        if (B644_CALL_97770(1u, 6) != 0)
#endif
            b644_sh(slot + 0x20u, 0u);
        goto common_tail;

#if defined(WM_8B644_MUTANT_DEFAULT_AS_STATE01)
    default:
        /* treat parked/OOR as the live follow arm */
        {
            u32 obj = b644_lw(slot + 0x4Cu);
            u32 entry = B644_RING_BASE;

            if (b644_actor_af(obj) != 1)
                B644_CALL_245D8(obj, (s16)1);
            B644_CALL_8C1DC(obj_id, slot, sc);
            b644_copy_pos3(slot, entry);
            b644_sh(slot + 0x24u, 0u);
            b644_sh(slot + 0x48u, b644_lhu(entry + 0x10u));
        }
        goto common_tail;
#else
    default:
        goto common_tail;
#endif
    }

common_tail:
#if defined(WM_8B644_MUTANT_SRA12_PUBLISH)
    b644_sh(0x8006EE54u, (u16)b644_sra(b644_lw(slot + 0x28u), 12u));
    b644_sh(0x8006EE56u, (u16)b644_sra(b644_lw(slot + 0x30u), 12u));
    b644_sh(0x8006EE58u, b644_lhu(slot + 0x48u));
#endif
#if defined(WM_8B644_MUTANT_74794_GATE)
    if (b644_lh(slot + 0x24u) != 0)
#else
    if (b644_lh(slot + 0x24u) == 0)
#endif
        B644_CALL_74794(0, slot + 0x28u);
#if defined(WM_8B644_MUTANT_WRONG_RETURN)
    return b644_lh(slot + 0x20u);
#else
    return 1;
#endif
}
