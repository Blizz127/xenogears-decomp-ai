/*
 * World-map scheduler callback 0x8008C844 (slot-4 world handler).
 *
 * Register-faithful transcription of retail world_map.bin
 * [0x8008C844, 0x8008D3F0).  See world_map_callback_8c844.h.
 *
 * JT1 @0x800707CC, 65 entries (index = lh slot[+0x20], sltiu 0x41,
 * out-of-range -> common tail).  Retail slot->target map:
 *   0x00,0x01 -> 0x8008C95C  resync gate + 90C68 class tree / movement
 *   0x02      -> 0x8008CCD4  follow slot-7 X/Z/heading (Y skipped)
 *   0x08      -> 0x8008CD04  slot-2 handshake
 *   0x09      -> 0x8008CD8C  slot-3 handshake
 *   0x0A      -> 0x8008CE14  approach setup; falls into 0x0B
 *   0x0B      -> 0x8008CE8C  approach step + 8C1DC(0x2C)
 *   0x0C      -> 0x8008CF00  handoff 97770(area,4) -> state 2
 *   0x10      -> 0x8008CF34  mode wait
 *   0x11      -> 0x8008CFC0  slot-2 release 97770(5,5)
 *   0x12      -> 0x8008CFEC  slot-3 release -> state 0x40
 *   0x18      -> 0x8008D01C  89160 / timer-8, or state 0x1A if byte==7
 *   0x19      -> 0x8008D08C  timer dec
 *   0x1A      -> 0x8008D0BC  timer dec -> anim 3, state 2
 *   0x20      -> 0x8008D0F8  release pair; falls into 0x21
 *   0x21      -> 0x8008D154  97770(1,2) + publish slot idx to pool+0x86
 *   0x22      -> 0x8008D18C  state = 0
 *   0x30      -> 0x8008D194  8DFF4 + facing *96; falls into 0x31
 *   0x31      -> 0x8008D24C  approach step
 *   0x32      -> 0x8008D2C4  ring refill, state 1, MODE_FLAG=2
 *   parked    -> 0x8008D34C (common tail): 45 of 65 slots
 */
#include <stdint.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_callback_8c844.h"
#include "world_map_common_tail.h"
#include "world_map_func_95414.h"
#include "world_map_helper_74794.h"
#include "world_map_helper_7528c.h"
#include "world_map_helper_8bec8.h"
#include "world_map_helper_8c040.h"
#include "world_map_helper_8c1dc.h"
#include "world_map_helper_8dff4.h"
#include "world_map_helper_90c68.h"
#include "world_map_helper_93f18.h"
#include "world_map_helper_94060.h"
#include "world_map_helper_941c4.h"
#include "world_map_helper_94238.h"
#include "world_map_helper_97770.h"

extern void func_800245D8(void* object, s16 animation);
extern void wm_800894C8(u32 record_index);
extern long rcos(long a);
extern long rsin(long a);

#define C844_POOL_PTR    0x8009BE24u
#define C844_MODE_WORD   0x8009C170u
#define C844_MODE_FLAG   0x8009BE10u
#define C844_BD04        0x8009BD04u
#define C844_AREA_BYTE   0x8009BD60u
#define C844_BOUND_BYTE  0x8009D738u
#define C844_CRUMB_TAB   0x8009B180u
#define C844_RING_INDEX  0x8009D154u
#define C844_RING_BASE   0x8009CEC4u
#define C844_POSE_BLOCK  0x8009D55Cu
#define C844_HEAD_MIRROR 0x8009D52Cu
#define C844_D554        0x8009D554u
#define C844_D7CC        0x8009D7CCu
#define C844_RESYNC_BYTE 0x8006F8E5u
#define C844_BUSY2_BYTE  0x8006F8E6u
#define C844_BUSY3_BYTE  0x8006F8E7u
#define C844_SLOT4_PRES  0x8006F368u
#define C844_SLOT2_PRES  0x8006F369u
#define C844_SLOT3_PRES  0x8006F36Au
#define C844_SLOT_BYTE   0x8006F364u
#define C844_PUB_X       0x8006EF90u
#define C844_PUB_Z       0x8006EF92u
#define C844_PUB_HEAD    0x8006EE5Au
#define C844_SCRATCH     0x1F800000u

#if defined(WM_8C844_TEST_TRACE)
extern void wm_8c844_test_store(u32 address, u32 width, u32 value);
extern s32 wm_8c844_test_90c68(u32 slot_addr);
extern void wm_8c844_test_245d8(u32 object_bits, s16 animation);
extern void wm_8c844_test_894c8(u32 record_index);
extern void wm_8c844_test_8c1dc(u32 ctx, u32 obj, u32 out);
extern s32 wm_8c844_test_95414(u32 pos, u32 dir, u32 out, s32 scale, s32 mode);
extern void wm_8c844_test_8c040(u32 vec, s32 a1, s32 a2, u32 o1, u32 o2);
extern void wm_8c844_test_7528c(void);
extern s32 wm_8c844_test_94238(u32 pos, u32 idx);
extern s32 wm_8c844_test_97770(u32 slot, s32 value);
extern s32 wm_8c844_test_941c4(u32 a, u32 b, u32 out, u32 angle);
extern s32 wm_8c844_test_8bec8(u32 obj);
extern s32 wm_8c844_test_93f18(u32 vec);
extern s32 wm_8c844_test_94060(s32 a0, s32 a1);
extern void wm_8c844_test_8dff4(u32 out);
extern void wm_8c844_test_89160(u32 a0, u32 a1, u32 a2);
extern long wm_8c844_test_rcos(long a);
extern long wm_8c844_test_rsin(long a);
extern void wm_8c844_test_74794(s32 tag, u32 pose);
#define C844_TRACE_STORE(a, w, v) wm_8c844_test_store((a), (w), (v))
#define C844_CALL_90C68(a)        wm_8c844_test_90c68(a)
#define C844_CALL_245D8(o, n)     wm_8c844_test_245d8((o), (n))
#define C844_CALL_894C8(i)        wm_8c844_test_894c8(i)
#define C844_CALL_8C1DC(c, o, u)  wm_8c844_test_8c1dc((c), (o), (u))
#define C844_CALL_95414(p, d, o, s, m) wm_8c844_test_95414((p), (d), (o), (s), (m))
#define C844_CALL_8C040(v, x, y, o1, o2) wm_8c844_test_8c040((v), (x), (y), (o1), (o2))
#define C844_CALL_7528C()         wm_8c844_test_7528c()
#define C844_CALL_94238(p, i)     wm_8c844_test_94238((p), (i))
#define C844_CALL_97770(s, v)     wm_8c844_test_97770((s), (v))
#define C844_CALL_941C4(a, b, o, g) wm_8c844_test_941c4((a), (b), (o), (g))
#define C844_CALL_8BEC8(o)        wm_8c844_test_8bec8(o)
#define C844_CALL_93F18(v)        wm_8c844_test_93f18(v)
#define C844_CALL_94060(a, b)     wm_8c844_test_94060((a), (b))
#define C844_CALL_8DFF4(o)        wm_8c844_test_8dff4(o)
#define C844_CALL_89160(a, b, c)  wm_8c844_test_89160((a), (b), (c))
#define C844_CALL_RCOS(a)         wm_8c844_test_rcos(a)
#define C844_CALL_RSIN(a)         wm_8c844_test_rsin(a)
#define C844_CALL_74794(t, p)     wm_8c844_test_74794((t), (p))
#else
static void wm_8c844_native_245d8(u32 object_bits, s16 animation)
{
    func_800245D8((void*)(uintptr_t)object_bits, animation);
}
#define C844_TRACE_STORE(a, w, v) ((void)0)
#define C844_CALL_90C68(a)        wm_80090C68(a)
#define C844_CALL_245D8(o, n)     wm_8c844_native_245d8((o), (n))
#define C844_CALL_894C8(i)        wm_800894C8(i)
#define C844_CALL_8C1DC(c, o, u)  wm_8008C1DC((c), (o), (u))
#define C844_CALL_95414(p, d, o, s, m) wm_80095414((p), (d), (o), (s), (m))
#define C844_CALL_8C040(v, x, y, o1, o2) wm_8008C040((v), (x), (y), (o1), (o2))
#define C844_CALL_7528C()         wm_8007528C()
#define C844_CALL_94238(p, i)     wm_80094238((p), (i))
#define C844_CALL_97770(s, v)     wm_80097770((s), (v))
#define C844_CALL_941C4(a, b, o, g) wm_800941C4((a), (b), (o), (g))
#define C844_CALL_8BEC8(o)        wm_8008BEC8(o)
#define C844_CALL_93F18(v)        wm_80093F18(v)
#define C844_CALL_94060(a, b)     wm_80094060((a), (b))
#define C844_CALL_8DFF4(o)        wm_8008DFF4(o)
#define C844_CALL_89160(a, b, c)  wm_80089160((a), (b), (c))
#define C844_CALL_RCOS(a)         rcos(a)
#define C844_CALL_RSIN(a)         rsin(a)
#define C844_CALL_74794(t, p)     wm_80074794((t), (p))
#endif

static u32 c844_lw(u32 a)
{
    u32 v;

    memcpy(&v, PSX_ADDR(a), 4);
    return v;
}

static void c844_sw(u32 a, u32 v)
{
    memcpy(PSX_ADDR(a), &v, 4);
    C844_TRACE_STORE(a, 4u, v);
}

static u16 c844_lhu(u32 a)
{
    u16 v;

    memcpy(&v, PSX_ADDR(a), 2);
    return v;
}

static s16 c844_lh(u32 a)
{
    s16 v;

    memcpy(&v, PSX_ADDR(a), 2);
    return v;
}

static void c844_sh(u32 a, u16 v)
{
    memcpy(PSX_ADDR(a), &v, 2);
    C844_TRACE_STORE(a, 2u, v);
}

static u8 c844_lbu(u32 a)
{
    u8 v;

    memcpy(&v, PSX_ADDR(a), 1);
    return v;
}

static void __attribute__((unused)) c844_sb(u32 a, u8 v)
{
    memcpy(PSX_ADDR(a), &v, 1);
    C844_TRACE_STORE(a, 1u, v);
}

static s32 c844_s32(u32 b)
{
    s32 v;

    memcpy(&v, &b, 4);
    return v;
}

static u32 c844_sra(u32 b, u32 amt)
{
    u32 v = b >> amt;

    if (b & 0x80000000u)
        v |= UINT32_MAX << (32u - amt);
    return v;
}

static s32 c844_sign16(u32 v)
{
    return (s32)(s16)(u16)(v & 0xFFFFu);
}

static void c844_copy4(u32 dst, u32 src)
{
    c844_sw(dst + 0u, c844_lw(src + 0u));
    c844_sw(dst + 4u, c844_lw(src + 4u));
    c844_sw(dst + 8u, c844_lw(src + 8u));
    c844_sw(dst + 12u, c844_lw(src + 12u));
}

static u32 c844_ring_entry(u32 i)
{
    return C844_RING_BASE + i * 0x14u;
}

static void c844_ring_fill(u32 src_vec, u32 src_head)
{
    u32 entry = C844_RING_BASE;
    s32 counter = 0x1F;

    for (;;) {
        c844_copy4(entry, src_vec);
        counter -= 1;
        c844_sh(entry + 0x10u, c844_lhu(src_head));
        if (c844_sign16((u32)counter) == -1)
            break;
        entry += 0x14u;
    }
}

static void c844_copy_pose(u32 slot)
{
    c844_copy4(C844_POSE_BLOCK, slot + 0x28u);
    c844_sh(C844_HEAD_MIRROR, c844_lhu(slot + 0x48u));
}

s32 wm_8008C844(s32 slot_idx)
{
    u32 pool = c844_lw(C844_POOL_PTR);
    u32 slot = pool + (u32)slot_idx * 128u;
    u32 sc = C844_SCRATCH;
    s32 state;
    s32 sub;

    sub = (s32)c844_lh(slot + 4u);
    if (sub == 4) {
#if defined(WM_8C844_MUTANT_SUBSTATE_POLARITY)
        c844_sh(slot + 4u, 0u);
        c844_sh(slot + 0x20u, 0x30u);
#else
        c844_sh(slot + 0x20u, 0x10u);
        c844_sh(slot + 4u, 0u);
        c844_sw(slot + 0x58u, 1u);
        c844_sb(C844_RESYNC_BYTE, 1u);
#endif
    } else if (sub < 5) {
        if (sub == 3) {
#if defined(WM_8C844_MUTANT_SUBSTATE_POLARITY)
            c844_sh(slot + 4u, 0u);
            c844_sh(slot + 0x20u, 0x10u);
#else
            c844_sh(slot + 4u, 0u);
            c844_sh(slot + 0x20u, 0x30u);
#endif
        }
    } else if (sub == 7) {
        u32 lap = c844_lw(slot + 0x58u) + 1u;
        u32 mode = c844_lw(C844_MODE_WORD);

        c844_sh(slot + 4u, 0u);
        c844_sw(slot + 0x58u, lap);
#if defined(WM_8C844_MUTANT_LAP_GATE)
        if (mode != lap) {
#else
        if (mode == lap) {
#endif
            c844_sh(slot + 0x20u, 1u);
            c844_sw(C844_MODE_FLAG, 2u);
            c844_sh(C844_BD04, 0u);
        }
    } else if (sub == 8) {
        c844_sh(slot + 4u, 0u);
        c844_sh(slot + 0x20u, 0x18u);
    }

    state = (s32)c844_lh(slot + 0x20u);

#if defined(WM_8C844_MUTANT_JT1_GUARD)
    if ((u32)state >= 0x2Du)
#else
    if ((u32)state >= 0x41u)
#endif
        goto common_tail;

    switch (state) {
    case 0x00:
    case 0x01:
#if defined(WM_8C844_MUTANT_RESYNC_POLARITY)
        if (c844_lbu(C844_RESYNC_BYTE) == 1u)
#else
        if (c844_lbu(C844_RESYNC_BYTE) != 1u)
#endif
            goto resync_ne1;
        {
            s32 cls;

#if defined(WM_8C844_MUTANT_SKIP_90C68)
            cls = 0;
#else
            cls = C844_CALL_90C68(slot);
#endif
            if (cls == 3) {
                c844_sh(slot + 0x20u, 8u);
                goto pre_tail;
            }
            if (cls < 4) {
#if defined(WM_8C844_MUTANT_CLASS1_AS_MOVE)
                if (cls == 1)
                    goto movement_block;
#endif
                if (cls == 1) {
                    c844_sw(C844_D554, 0u);
                    c844_sw(C844_D7CC, 0u);
                    goto pre_tail;
                }
                goto movement_block;
            }
            if (cls == 4) {
                s32 h = C844_CALL_93F18(slot + 0x28u);
                s32 r = C844_CALL_94060(1, c844_sign16((u32)h));

                if (((u32)r << 16) == 0u)
                    goto pre_tail;
                c844_sh(slot + 0x20u, 0x20u);
                goto pre_tail;
            }
            goto movement_block;
        }

    case 0x02:
        c844_sw(slot + 0x28u, c844_lw(pool + 0x3A8u));
#if defined(WM_8C844_MUTANT_STATE2_COPY_Y)
        c844_sw(slot + 0x2Cu, c844_lw(pool + 0x3ACu));
#endif
        c844_sw(slot + 0x30u, c844_lw(pool + 0x3B0u));
        c844_sh(slot + 0x48u, c844_lhu(pool + 0x3C8u));
        goto common_tail;

    case 0x08:
        if (c844_lbu(C844_SLOT2_PRES) == 0xFFu)
            goto inc_state;
        if (c844_lbu(C844_BUSY2_BYTE) == 1u) {
#if defined(WM_8C844_MUTANT_WRONG_97770_ARGS)
            if (C844_CALL_97770(2u, 1) == 0)
#else
            if (C844_CALL_97770(5u, 1) == 0)
#endif
                goto common_tail;
            c844_sh(slot + 0x86u, c844_lbu(C844_AREA_BYTE));
            state = (s32)c844_lhu(slot + 0x20u) + 1;
            goto set_state;
        }
        (void)C844_CALL_97770(2u, 1);
        c844_sh(pool + 0x106u, c844_lbu(C844_AREA_BYTE));
        (void)C844_CALL_97770(5u, 8);
        goto inc_state;

    case 0x09:
        if (c844_lbu(C844_SLOT3_PRES) == 0xFFu)
            goto inc_state;
        if (c844_lbu(C844_BUSY3_BYTE) == 1u) {
            if (C844_CALL_97770(6u, 1) == 0)
                goto common_tail;
            c844_sh(slot + 0x106u, c844_lbu(C844_AREA_BYTE));
            state = (s32)c844_lhu(slot + 0x20u) + 1;
            goto set_state;
        }
        (void)C844_CALL_97770(3u, 1);
        c844_sh(pool + 0x186u, c844_lbu(C844_AREA_BYTE));
        (void)C844_CALL_97770(6u, 8);
        goto inc_state;

    case 0x0A: {
        u32 area = c844_lbu(C844_AREA_BYTE);
        u32 target = c844_lw(C844_POOL_PTR) + area * 128u;

        (void)C844_CALL_941C4(slot + 0x28u, target + 0x28u,
                              slot + 0x38u, slot + 0x48u);
        area = c844_lbu(C844_AREA_BYTE);
        target = c844_lw(C844_POOL_PTR) + area * 128u;
        c844_sw(slot + 0x50u, c844_sra(c844_lw(target + 0x28u), 12u));
        c844_sw(slot + 0x54u, c844_sra(c844_lw(target + 0x30u), 12u));
        C844_CALL_245D8(c844_lw(slot + 0x4Cu), (s16)1);
        state = (s32)c844_lhu(slot + 0x20u) + 1;
        c844_sh(slot + 0x20u, (u16)state);
        /* fall into 0x0B */
    }
        /* fallthrough */
    case 0x0B:
        if (C844_CALL_8BEC8(slot) == 3) {
#if !defined(WM_8C844_MUTANT_SKIP_8BEC8_ADVANCE)
            state = (s32)c844_lhu(slot + 0x20u) + 1;
            c844_sh(slot + 0x20u, (u16)state);
#endif
        }
        c844_copy_pose(slot);
#if defined(WM_8C844_MUTANT_WRONG_RECORD)
        C844_CALL_8C1DC(0x2Fu, slot, sc);
#else
        C844_CALL_8C1DC(0x2Cu, slot, sc);
#endif
        goto common_tail;

    case 0x0C:
        if (C844_CALL_97770(c844_lbu(C844_AREA_BYTE), 4) == 0)
            goto common_tail;
        c844_sh(slot + 0x24u, 1u);
#if defined(WM_8C844_MUTANT_WRONG_HANDOFF_STATE)
        c844_sh(slot + 0x20u, 1u);
#else
        c844_sh(slot + 0x20u, 2u);
#endif
#if defined(WM_8C844_MUTANT_WRONG_RECORD)
        C844_CALL_894C8(0x2Fu);
#else
        C844_CALL_894C8(0x2Cu);
#endif
        goto common_tail;

    case 0x10: {
        s32 mode = c844_s32(c844_lw(C844_MODE_WORD));

        if (mode == 2) {
            if (c844_lbu(C844_BUSY2_BYTE) == 0u)
                goto common_tail;
            goto inc_state;
        }
        if (mode < 3) {
            if (mode == 1) {
                state = 1;
                goto set_state;
            }
            goto common_tail;
        }
        if (mode == 3) {
#if defined(WM_8C844_MUTANT_MODE3_BUSY_OR)
            if ((c844_lbu(C844_BUSY2_BYTE) | c844_lbu(C844_BUSY3_BYTE)) == 0u)
#else
            if ((c844_lbu(C844_BUSY2_BYTE) & c844_lbu(C844_BUSY3_BYTE)) == 0u)
#endif
                goto common_tail;
            goto inc_state;
        }
        goto common_tail;
    }

    case 0x11:
        if (c844_lbu(C844_SLOT2_PRES) == 0xFFu)
            goto inc_state;
        if (C844_CALL_97770(5u, 5) == 0)
            goto common_tail;
        goto inc_state;

    case 0x12:
        if (c844_lbu(C844_SLOT3_PRES) != 0xFFu) {
            if (C844_CALL_97770(6u, 5) == 0)
                goto common_tail;
        }
        state = 0x40;
        goto set_state;

    case 0x18:
        if (c844_lbu(C844_SLOT_BYTE + (u32)slot_idx) == 7u) {
            c844_sh(slot + 0x22u, 1u);
            c844_sh(slot + 0x20u, 0x1Au);
            goto common_tail;
        }
        c844_sh(sc + 0xA0u, (u16)c844_sra(c844_lw(slot + 0x28u), 12u));
        c844_sh(sc + 0xA2u, (u16)c844_sra(c844_lw(slot + 0x2Cu), 12u));
        c844_sh(sc + 0xA4u, (u16)c844_sra(c844_lw(slot + 0x30u), 12u));
        C844_CALL_89160(5u, sc + 0xA0u, 0u);
        c844_sh(slot + 0x22u, 8u);
        goto inc_state;

    case 0x19:
        state = (s32)c844_lhu(slot + 0x22u) - 1;
        c844_sh(slot + 0x22u, (u16)state);
        if (c844_sign16((u32)state) > 0)
            goto common_tail;
        c844_sh(slot + 0x24u, 1u);
        c844_sh(slot + 0x22u, 0x10u);
        goto inc_state;

    case 0x1A:
        state = (s32)c844_lhu(slot + 0x22u) - 1;
        c844_sh(slot + 0x22u, (u16)state);
        if (c844_sign16((u32)state) > 0)
            goto common_tail;
        C844_CALL_245D8(c844_lw(slot + 0x4Cu), (s16)3);
        c844_sh(slot + 0x24u, 1u);
        c844_sh(slot + 0x20u, 2u);
        goto common_tail;

    case 0x20:
        C844_CALL_245D8(c844_lw(slot + 0x4Cu), (s16)0);
        if (c844_lbu(C844_SLOT2_PRES) != 0xFFu)
            (void)C844_CALL_97770(5u, 2);
        if (c844_lbu(C844_SLOT3_PRES) != 0xFFu)
            (void)C844_CALL_97770(6u, 2);
        c844_sw(slot + 0x40u, 0u);
        c844_sw(slot + 0x3Cu, 0u);
        c844_sw(slot + 0x38u, 0u);
        state = (s32)c844_lhu(slot + 0x20u) + 1;
        c844_sh(slot + 0x20u, (u16)state);
#if defined(WM_8C844_MUTANT_STATE20_NO_FALLTHROUGH)
        goto common_tail;
#endif
        /* fall into 0x21 */
        /* fallthrough */
    case 0x21:
        if (C844_CALL_97770(1u, 2) == 0)
            goto common_tail;
        c844_sh(c844_lw(C844_POOL_PTR) + 0x86u, (u16)slot_idx);
        goto inc_state;

    case 0x22:
        c844_sh(slot + 0x20u, 0u);
        goto common_tail;

    case 0x30: {
        long c;
        long s;
        u32 head_raw;

        C844_CALL_8DFF4(slot + 0x28u);
        c844_sh(slot + 0x24u, 0u);
        head_raw = c844_lw(c844_lw(C844_POOL_PTR) + 0x3F8u);
        c844_sh(slot + 0x48u, (u16)c844_sign16(head_raw));
        c = C844_CALL_RCOS((long)c844_sign16(head_raw));
        c844_sw(sc + 0u, c844_lw(slot + 0x28u) + (u32)c * 96u);
        s = C844_CALL_RSIN((long)c844_lh(slot + 0x48u));
        c844_sw(sc + 8u, c844_lw(slot + 0x30u) + (0u - (u32)s) * 96u);
        (void)C844_CALL_941C4(slot + 0x28u, sc, slot + 0x38u, slot + 0x48u);
        c844_sw(slot + 0x50u, c844_sra(c844_lw(sc + 0u), 12u));
        c844_sw(slot + 0x54u, c844_sra(c844_lw(sc + 8u), 12u));
        C844_CALL_245D8(c844_lw(slot + 0x4Cu), (s16)1);
        state = (s32)c844_lhu(slot + 0x20u) + 1;
        c844_sh(slot + 0x20u, (u16)state);
        /* fall into 0x31 */
    }
        /* fallthrough */
    case 0x31:
        if (C844_CALL_8BEC8(slot) == 3) {
            C844_CALL_245D8(c844_lw(slot + 0x4Cu), (s16)0);
            c844_sw(slot + 0x40u, 0u);
            c844_sw(slot + 0x3Cu, 0u);
            c844_sw(slot + 0x38u, 0u);
            state = (s32)c844_lhu(slot + 0x20u) + 1;
            c844_sh(slot + 0x20u, (u16)state);
        }
        c844_copy_pose(slot);
        goto common_tail;

    case 0x32:
        c844_sh(C844_RING_INDEX, 0u);
        c844_copy4(sc + 0x30u, slot + 0x28u);
        c844_sh(sc + 0xA0u, c844_lhu(slot + 0x48u));
        c844_ring_fill(sc + 0x30u, sc + 0xA0u);
        c844_sh(slot + 0x20u, 1u);
        c844_sw(C844_MODE_FLAG, 2u);
        goto common_tail;

    default:
        goto common_tail;
    }

movement_block:
    {
        u32 pos = slot + 0x28u;
        u32 vel = slot + 0x38u;
        u32 outv = sc + 0x90u;
        s32 r;

        if ((c844_lw(slot + 0x38u) | c844_lw(slot + 0x3Cu) |
             c844_lw(slot + 0x40u)) == 0u) {
            u32 obj = c844_lw(slot + 0x4Cu);

            if (*((s8*)PSX_ADDR(obj) + 0xAF) != 0) {
                C844_CALL_245D8(obj, (s16)0);
#if defined(WM_8C844_MUTANT_WRONG_RECORD)
                C844_CALL_894C8(0x2Fu);
#else
                C844_CALL_894C8(0x2Cu);
#endif
            }
        } else {
            u32 obj = c844_lw(slot + 0x4Cu);

            if (*((s8*)PSX_ADDR(obj) + 0xAF) != 1)
                C844_CALL_245D8(obj, (s16)1);
#if defined(WM_8C844_MUTANT_WRONG_RECORD)
            C844_CALL_8C1DC(0x2Fu, slot, sc);
#else
            C844_CALL_8C1DC(0x2Cu, slot, sc);
#endif
        }

        r = C844_CALL_95414(pos, vel, outv,
                            c844_s32((u32)c844_lh(slot + 0x4Au) << 12),
                            c844_s32(c844_lw(C844_MODE_FLAG)));
#if defined(WM_8C844_MUTANT_R_TEST_WIDTH)
        if ((u16)(u32)r == 0u) {
#else
        if (r == 0) {
#endif
            c844_sw(slot + 0x38u, c844_lw(sc + 0x90u));
            c844_sw(slot + 0x3Cu, c844_lw(sc + 0x94u));
            c844_sw(slot + 0x40u, c844_lw(sc + 0x98u));
            c844_sw(slot + 0x44u, c844_lw(sc + 0x9Cu));
            r = C844_CALL_95414(pos, vel, outv,
                                c844_s32((u32)c844_lh(slot + 0x4Au) << 12),
                                c844_s32(c844_lw(C844_MODE_FLAG)));
            if (r == 0) {
                c844_sw(slot + 0x40u, 0u);
                c844_sw(slot + 0x38u, 0u);
            }
        }

        if (r == 1) {
            u32 area;

#if defined(WM_8C844_MUTANT_8C040_SCALE)
            C844_CALL_8C040(outv, 0x10, 0x20, C844_BOUND_BYTE,
                            C844_AREA_BYTE);
#else
            C844_CALL_8C040(outv, 0x18, 0x30, C844_BOUND_BYTE,
                            C844_AREA_BYTE);
#endif
            if (c844_lbu(C844_AREA_BYTE) == 7u)
                area = (u32)c844_lbu(C844_BOUND_BYTE) + 3u;
            else
                area = (u32)c844_lbu(C844_BOUND_BYTE);
            {
                u32 gate = c844_lhu(C844_CRUMB_TAB + area * 2u);

                if (gate != 0u) {
                    c844_copy4(slot + 0x28u, sc + 0x90u);
                    if ((c844_lw(slot + 0x38u) | c844_lw(slot + 0x40u)) != 0u) {
#if defined(WM_8C844_MUTANT_RING_MASK)
                        u32 i = ((u32)c844_lhu(C844_RING_INDEX) + 1u) & 0x3Fu;
#else
                        u32 i = ((u32)c844_lhu(C844_RING_INDEX) + 1u) & 0x1Fu;
#endif
                        u32 entry = c844_ring_entry(i);

                        c844_sh(C844_RING_INDEX, (u16)i);
                        c844_copy4(entry, slot + 0x28u);
                        c844_sh(entry + 0x10u, c844_lhu(slot + 0x48u));
                        C844_CALL_7528C();
                    }
                }
            }
        } else {
#if defined(WM_8C844_MUTANT_8C040_SCALE)
            C844_CALL_8C040(slot + 0x28u, 0x10, 0x20, C844_BOUND_BYTE,
                            C844_AREA_BYTE);
#else
            C844_CALL_8C040(slot + 0x28u, 0x18, 0x30, C844_BOUND_BYTE,
                            C844_AREA_BYTE);
#endif
        }

#if defined(WM_8C844_MUTANT_94238_IDX)
        (void)C844_CALL_94238(slot + 0x28u, 0u);
#else
        (void)C844_CALL_94238(slot + 0x28u, 1u);
#endif
        c844_sw(slot + 0x40u, 0u);
        c844_sw(slot + 0x3Cu, 0u);
        c844_sw(slot + 0x38u, 0u);
        c844_copy_pose(slot);
        goto pre_tail;
    }

resync_ne1:
    {
        u32 obj = c844_lw(slot + 0x4Cu);

        if (*((s8*)PSX_ADDR(obj) + 0xAF) != 3) {
            C844_CALL_245D8(obj, (s16)3);
#if defined(WM_8C844_MUTANT_WRONG_RECORD)
            C844_CALL_894C8(0x2Fu);
#else
            C844_CALL_894C8(0x2Cu);
#endif
        }
        goto common_tail;
    }

inc_state:
    state = (s32)c844_lhu(slot + 0x20u) + 1;
    /* fallthrough */
set_state:
    c844_sh(slot + 0x20u, (u16)state);
    goto common_tail;

pre_tail:
    c844_sh(C844_BD04, 0u);
    /* fallthrough */
common_tail:
    {
        s32 st = (s32)c844_lh(slot + 0x20u);
        u32 mode = c844_lw(C844_MODE_FLAG);
        int call_74794;

#if defined(WM_8C844_MUTANT_74794_GATE)
        (void)mode;
        call_74794 = (st == 2);
#else
        if (st == 2)
            call_74794 = 0;
        else if (mode == 2u)
            call_74794 = 1;
        else if (c844_lbu(C844_SLOT4_PRES) == 7u)
            call_74794 = 0;
        else
            call_74794 = 1;
#endif
        if (call_74794) {
#if defined(WM_8C844_MUTANT_74794_TAG)
            C844_CALL_74794(0, slot + 0x28u);
#else
            C844_CALL_74794(1, slot + 0x28u);
#endif
        }
    }
#if !defined(WM_8C844_MUTANT_SKIP_SRA12_PUBLISH)
#if defined(WM_8C844_MUTANT_PUBLISH_MIRROR)
    c844_sh(0x8006EE54u, (u16)c844_sra(c844_lw(slot + 0x28u), 12u));
    c844_sh(0x8006EE56u, (u16)c844_sra(c844_lw(slot + 0x30u), 12u));
    c844_sh(0x8006EE58u, c844_lhu(slot + 0x48u));
#else
    c844_sh(C844_PUB_X, (u16)c844_sra(c844_lw(slot + 0x28u), 12u));
    c844_sh(C844_PUB_Z, (u16)c844_sra(c844_lw(slot + 0x30u), 12u));
    c844_sh(C844_PUB_HEAD, c844_lhu(slot + 0x48u));
#endif
#endif
#if defined(WM_8C844_MUTANT_WRONG_RETURN)
    return c844_lh(slot + 0x20u);
#else
    return 1;
#endif
}
