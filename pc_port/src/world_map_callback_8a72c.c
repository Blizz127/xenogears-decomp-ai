/*
 * World-map scheduler callback 0x8008A72C.
 *
 * Register-faithful transcription of retail world_map.bin
 * [0x8008A72C, 0x8008B2BC).  Slot-1 cb1: entry +4 machine, 65-way
 * control dispatch, live movement arm through both wm_80095414 sites,
 * remaining table arms, shared epilogue.  Always returns 1.
 */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_callback_8a72c.h"
#include "world_map_common_tail.h"
#include "world_map_func_95414.h"
#include "world_map_helper_74794.h"
#include "world_map_helper_7528c.h"
#include "world_map_helper_8bec8.h"
#include "world_map_helper_8c040.h"
#include "world_map_helper_8c1dc.h"
#include "world_map_helper_90a84.h"
#include "world_map_helper_941c4.h"
#include "world_map_helper_94238.h"
#include "world_map_helper_97770.h"
#include "world_map_terrain_sampler.h"

#define WM_U8(a)  (*(u8*)PSX_ADDR(a))
#define WM_U16(a) (*(u16*)PSX_ADDR(a))
#define WM_U32(a) (*(u32*)PSX_ADDR(a))

#define WM_POOL_PTR              0x8009BE24u
#define WM_MODE                  0x8009BE10u
#define WM_BD04                  0x8009BD04u
#define WM_BD60                  0x8009BD60u
#define WM_C170                  0x8009C170u
#define WM_D554                  0x8009D554u
#define WM_D7CC                  0x8009D7CCu
#define WM_D738                  0x8009D738u
#define WM_B180                  0x8009B180u
#define WM_D154                  0x8009D154u
#define WM_POSE_TABLE            0x8009CEC4u
#define WM_D55C                  0x8009D55Cu
#define WM_D52C                  0x8009D52Cu
#define WM_F8E5                  0x8006F8E5u
#define WM_F8E6                  0x8006F8E6u
#define WM_F8E7                  0x8006F8E7u
#define WM_F368                  0x8006F368u
#define WM_F369                  0x8006F369u
#define WM_F36A                  0x8006F36Au
#define WM_EE54                  0x8006EE54u
#define WM_EE56                  0x8006EE56u
#define WM_EE58                  0x8006EE58u
#define WM_EE5A                  0x8006EE5Au
#define WM_EF90                  0x8006EF90u
#define WM_EF92                  0x8006EF92u
#define WM_SCRATCH               0x1F800000u

#define WM_SLOT_STRIDE           0x80u
#define WM_SLOT_OFF_CLAIM        0x04u
#define WM_SLOT_OFF_CONTROL      0x20u
#define WM_SLOT_OFF_FLAG         0x24u
#define WM_SLOT_OFF_X            0x28u
#define WM_SLOT_OFF_Y            0x2Cu
#define WM_SLOT_OFF_Z            0x30u
#define WM_SLOT_OFF_AUX          0x34u
#define WM_SLOT_OFF_VX           0x38u
#define WM_SLOT_OFF_VY           0x3Cu
#define WM_SLOT_OFF_VZ           0x40u
#define WM_SLOT_OFF_VW           0x44u
#define WM_SLOT_OFF_STATE        0x48u
#define WM_SLOT_OFF_CONST        0x4Au
#define WM_SLOT_OFF_OBJECT       0x4Cu
#define WM_SLOT_OFF_SX           0x50u
#define WM_SLOT_OFF_SZ           0x54u
#define WM_SLOT_OFF_COUNTER      0x58u
#define WM_SLOT_OFF_SIGNED       0x5Cu
#define WM_OBJECT_BYTE_AF        0xAFu
#define WM_POSE_STRIDE           0x14u

extern void func_800245D8(void* object, s16 animation);
extern long rcos(long a);
extern long rsin(long a);

#if defined(WM_8A72C_TEST_TRACE)
s32 wm_8a72c_test_90a84(u32 slot_addr);
s32 wm_8a72c_test_95414(u32 pos, u32 dir, u32 out, s32 scale, s32 mode);
void wm_8a72c_test_894c8(u32 record_index);
void wm_8a72c_test_8c1dc(u32 ctx, u32 obj, u32 out);
void wm_8a72c_test_8c040(u32 vec, s32 a1, s32 a2, u32 out1, u32 out2);
s32 wm_8a72c_test_94238(u32 pos, u32 list_index);
void wm_8a72c_test_7528c(void);
void wm_8a72c_test_74794(s32 tag, u32 pose_addr);
s32 wm_8a72c_test_97770(u32 slot_idx, s32 value);
s32 wm_8a72c_test_941c4(u32 a, u32 b, u32 out, u32 angle);
s32 wm_8a72c_test_8bec8(u32 obj);
s32 wm_8a72c_test_93978(s32 x, s32 z);
void wm_8a72c_test_245d8(void* object, s16 animation);
long wm_8a72c_test_rcos(long a);
long wm_8a72c_test_rsin(long a);
#define CALL_90A84(s)           wm_8a72c_test_90a84(s)
#define CALL_95414(p, d, o, sc, m) wm_8a72c_test_95414((p), (d), (o), (sc), (m))
#define CALL_894C8(i)           wm_8a72c_test_894c8(i)
#define CALL_8C1DC(c, o, u)     wm_8a72c_test_8c1dc((c), (o), (u))
#define CALL_8C040(v, a, b, x, y) wm_8a72c_test_8c040((v), (a), (b), (x), (y))
#define CALL_94238(p, i)        wm_8a72c_test_94238((p), (i))
#define CALL_7528C()            wm_8a72c_test_7528c()
#define CALL_74794(t, p)        wm_8a72c_test_74794((t), (p))
#define CALL_97770(i, v)        wm_8a72c_test_97770((i), (v))
#define CALL_941C4(a, b, o, n)  wm_8a72c_test_941c4((a), (b), (o), (n))
#define CALL_8BEC8(o)           wm_8a72c_test_8bec8(o)
#define CALL_93978(x, z)        wm_8a72c_test_93978((x), (z))
#define CALL_245D8(o, a)        wm_8a72c_test_245d8((o), (a))
#define CALL_RCOS(a)            wm_8a72c_test_rcos(a)
#define CALL_RSIN(a)            wm_8a72c_test_rsin(a)
#else
#define CALL_90A84(s)           wm_80090A84(s)
#define CALL_95414(p, d, o, sc, m) wm_80095414((p), (d), (o), (sc), (m))
#define CALL_894C8(i)           wm_800894C8(i)
#define CALL_8C1DC(c, o, u)     wm_8008C1DC((c), (o), (u))
#define CALL_8C040(v, a, b, x, y) wm_8008C040((v), (a), (b), (x), (y))
#define CALL_94238(p, i)        wm_80094238((p), (i))
#define CALL_7528C()            wm_8007528C()
#define CALL_74794(t, p)        wm_80074794((t), (p))
#define CALL_97770(i, v)        wm_80097770((i), (v))
#define CALL_941C4(a, b, o, n)  wm_800941C4((a), (b), (o), (n))
#define CALL_8BEC8(o)           wm_8008BEC8(o)
#define CALL_93978(x, z)        wm_80093978((x), (z))
#define CALL_245D8(o, a)        func_800245D8((o), (a))
#define CALL_RCOS(a)            rcos(a)
#define CALL_RSIN(a)            rsin(a)
#endif

static s32 wm_sign16(u32 bits)
{
    return (s32)(s16)(u16)bits;
}

static s32 wm_sra12_u32(u32 bits)
{
    return (s32)bits >> 12;
}

static void* wm_native_from_bits(u32 bits)
{
    return (void*)(uintptr_t)bits;
}

static s8 wm_object_byte_af(u32 slot_addr)
{
    void* object = wm_native_from_bits(WM_U32(slot_addr + WM_SLOT_OFF_OBJECT));
    return *((s8*)((u8*)object + WM_OBJECT_BYTE_AF));
}

static void wm_copy4(u32 dst, u32 src)
{
    WM_U32(dst + 0u) = WM_U32(src + 0u);
    WM_U32(dst + 4u) = WM_U32(src + 4u);
    WM_U32(dst + 8u) = WM_U32(src + 8u);
    WM_U32(dst + 12u) = WM_U32(src + 12u);
}

static void wm_publish_d55c(u32 slot_addr)
{
    wm_copy4(WM_D55C, slot_addr + WM_SLOT_OFF_X);
    WM_U16(WM_D52C) = WM_U16(slot_addr + WM_SLOT_OFF_STATE);
}

static void wm_fill_pose_ring(u32 src_xyz, u16 state_bits)
{
    u32 dst = WM_POSE_TABLE;
    s32 remain = 0x1F;
    s32 i;

    for (i = 0; i < 32; i++) {
        wm_copy4(dst, src_xyz);
        WM_U16(dst + 0x10u) = state_bits;
        dst += WM_POSE_STRIDE;
        remain -= 1;
        (void)remain;
    }
}

static s32 wm_8a72c_epilogue(u32 slot_addr)
{
    WM_U16(WM_EE54) =
        (u16)wm_sra12_u32(WM_U32(slot_addr + WM_SLOT_OFF_X));
    WM_U16(WM_EE56) =
        (u16)wm_sra12_u32(WM_U32(slot_addr + WM_SLOT_OFF_Z));
    WM_U16(WM_EE58) = WM_U16(slot_addr + WM_SLOT_OFF_STATE);
    if (wm_sign16(WM_U16(slot_addr + WM_SLOT_OFF_FLAG)) == 0) {
#if defined(WM_8A72C_MUTANT_SKIP_74794)
        /* mutant: skip pose publish */
#else
        CALL_74794(0, slot_addr + WM_SLOT_OFF_X);
#endif
    }
    return 1;
}

static s32 wm_8a72c_set_control(u32 slot_addr, u16 control)
{
    WM_U16(slot_addr + WM_SLOT_OFF_CONTROL) = control;
    return wm_8a72c_epilogue(slot_addr);
}

static s32 wm_8a72c_c170_gate(u32 slot_addr)
{
    s32 c170 = (s32)WM_U32(WM_C170);

    if (c170 == 2) {
        if (WM_U8(WM_F8E6) == 0u)
            return wm_8a72c_set_control(
                slot_addr,
                (u16)(WM_U16(slot_addr + WM_SLOT_OFF_CONTROL) + 1u));
        return wm_8a72c_epilogue(slot_addr);
    }
    if (c170 < 3) {
        if (c170 == 1)
            return wm_8a72c_set_control(slot_addr, 1u);
        return wm_8a72c_epilogue(slot_addr);
    }
    if (c170 == 3) {
        if ((WM_U8(WM_F8E6) | WM_U8(WM_F8E7)) == 0u)
            return wm_8a72c_set_control(
                slot_addr,
                (u16)(WM_U16(slot_addr + WM_SLOT_OFF_CONTROL) + 1u));
        return wm_8a72c_epilogue(slot_addr);
    }
    return wm_8a72c_epilogue(slot_addr);
}

static s32 wm_8a72c_state_01(u32 slot_addr)
{
    s32 classified;
    u32 idx;
    s32 ret;
    u32 pos;
    u32 dir;
    u32 out;
    s32 scale;
    s32 mode;
    u32 object_bits;
    void* object;

    if (WM_U8(WM_F8E5) != 0u) {
        u32 pool = WM_U32(WM_POOL_PTR);
        WM_U16(slot_addr + WM_SLOT_OFF_FLAG) = 1u;
        WM_U32(slot_addr + WM_SLOT_OFF_X) = WM_U32(pool + 0x228u);
        WM_U32(slot_addr + WM_SLOT_OFF_Y) = WM_U32(pool + 0x22Cu);
        WM_U32(slot_addr + WM_SLOT_OFF_Z) = WM_U32(pool + 0x230u);
        WM_U16(slot_addr + WM_SLOT_OFF_STATE) = WM_U16(pool + 0x248u);
        CALL_894C8(0x2Fu);
        return wm_8a72c_epilogue(slot_addr);
    }

    classified = CALL_90A84(slot_addr);
    idx = (u32)(classified - 1);
    if (idx < 5u) {
        if (idx == 0u) {
#if defined(WM_8A72C_MUTANT_90A84_RET1_CONTROL)
            WM_U16(slot_addr + WM_SLOT_OFF_CONTROL) = 0x3Fu;
#else
            WM_U16(slot_addr + WM_SLOT_OFF_CONTROL) = 0x40u;
#endif
            WM_U32(WM_D554) = 0u;
            WM_U32(WM_D7CC) = 0u;
        } else if (idx == 2u) {
            if (WM_U8(WM_BD60) == 7u) {
                WM_U16(slot_addr + WM_SLOT_OFF_CONTROL) = 8u;
            } else {
                WM_U8(WM_BD60) = 4u;
                WM_U16(slot_addr + WM_SLOT_OFF_CONTROL) = 0x10u;
            }
        }
        WM_U16(WM_BD04) = 0u;
        WM_U16(slot_addr + WM_SLOT_OFF_FLAG) = 0u;
        return wm_8a72c_epilogue(slot_addr);
    }

    if ((WM_U32(slot_addr + WM_SLOT_OFF_VX) == 0u) &&
        (WM_U32(slot_addr + WM_SLOT_OFF_VY) == 0u) &&
        (WM_U32(slot_addr + WM_SLOT_OFF_VZ) == 0u)) {
        object_bits = WM_U32(slot_addr + WM_SLOT_OFF_OBJECT);
        object = wm_native_from_bits(object_bits);
        if (wm_object_byte_af(slot_addr) != 0) {
            CALL_245D8(object, (s16)0);
            CALL_894C8(0x2Fu);
        }
    } else {
        object_bits = WM_U32(slot_addr + WM_SLOT_OFF_OBJECT);
        object = wm_native_from_bits(object_bits);
        if (wm_object_byte_af(slot_addr) != 1) {
            CALL_245D8(object, (s16)1);
        }
        CALL_8C1DC(0x2Fu, slot_addr, WM_SCRATCH);
    }

    pos = slot_addr + WM_SLOT_OFF_X;
    dir = slot_addr + WM_SLOT_OFF_VX;
    out = WM_SCRATCH + 0x90u;
    scale = (s32)((u32)wm_sign16(WM_U16(slot_addr + WM_SLOT_OFF_CONST)) << 12);
    mode = (s32)WM_U32(WM_MODE);
    ret = CALL_95414(pos, dir, out, scale, mode);
    if (((u32)ret << 16) == 0u) {
        wm_copy4(slot_addr + WM_SLOT_OFF_VX, WM_SCRATCH + 0x90u);
#if defined(WM_8A72C_MUTANT_NO_SECOND_95414)
        ret = 0;
#else
        ret = CALL_95414(pos, dir, out, scale, mode);
#endif
        if (((u32)ret << 16) == 0u) {
            WM_U32(slot_addr + WM_SLOT_OFF_VZ) = 0u;
            WM_U32(slot_addr + WM_SLOT_OFF_VX) = 0u;
        }
    }

    if (wm_sign16((u32)ret) == 1) {
        u32 lookup_idx;
        u16 hit;

        CALL_8C040(WM_SCRATCH + 0x90u, 16, 32, WM_D738, WM_BD60);
        if (WM_U8(WM_BD60) == 7u)
            lookup_idx = (u32)WM_U8(WM_D738) + 3u;
        else
            lookup_idx = (u32)WM_U8(WM_D738);
        hit = WM_U16(WM_B180 + ((u32)wm_sign16(lookup_idx) << 1));
        if (hit != 0u) {
            wm_copy4(slot_addr + WM_SLOT_OFF_X, WM_SCRATCH + 0x90u);
            if ((WM_U32(slot_addr + WM_SLOT_OFF_VX) |
                 WM_U32(slot_addr + WM_SLOT_OFF_VZ)) != 0u) {
                u16 ring = (u16)((WM_U16(WM_D154) + 1u) & 0x1Fu);
                u32 entry = WM_POSE_TABLE + (u32)ring * WM_POSE_STRIDE;
                WM_U16(WM_D154) = ring;
                wm_copy4(entry, slot_addr + WM_SLOT_OFF_X);
                WM_U16(entry + 0x10u) =
                    WM_U16(slot_addr + WM_SLOT_OFF_STATE);
                CALL_7528C();
            }
        }
    } else {
        CALL_8C040(slot_addr + WM_SLOT_OFF_X, 16, 32, WM_D738, WM_BD60);
    }

    CALL_94238(slot_addr + WM_SLOT_OFF_X, 0u);
    WM_U32(slot_addr + WM_SLOT_OFF_VZ) = 0u;
    WM_U32(slot_addr + WM_SLOT_OFF_VY) = 0u;
    WM_U32(slot_addr + WM_SLOT_OFF_VX) = 0u;
    wm_publish_d55c(slot_addr);
    WM_U16(WM_BD04) = 0u;
    WM_U16(slot_addr + WM_SLOT_OFF_FLAG) = 0u;
    return wm_8a72c_epilogue(slot_addr);
}

static s32 wm_8a72c_state_23(u32 slot_addr)
{
    u32 pool = WM_U32(WM_POOL_PTR);
    WM_U32(slot_addr + WM_SLOT_OFF_X) = WM_U32(pool + 0x3A8u);
    WM_U32(slot_addr + WM_SLOT_OFF_Y) = WM_U32(pool + 0x3ACu);
    WM_U32(slot_addr + WM_SLOT_OFF_Z) = WM_U32(pool + 0x3B0u);
    WM_U16(slot_addr + WM_SLOT_OFF_STATE) = WM_U16(pool + 0x3C8u);
    return wm_8a72c_epilogue(slot_addr);
}

static s32 wm_8a72c_state_8(u32 slot_addr)
{
    if (WM_U8(WM_F369) != 0xFFu) {
        if (WM_U8(WM_F8E6) == 0u) {
            CALL_97770(2u, 1);
            WM_U16(slot_addr + 0x86u) = (u16)WM_U8(WM_BD60);
            CALL_97770(5u, 8);
        } else {
            CALL_97770(5u, 1);
            WM_U16(WM_U32(WM_POOL_PTR) + 0x286u) = (u16)WM_U8(WM_BD60);
        }
    }
    return wm_8a72c_set_control(
        slot_addr, (u16)(WM_U16(slot_addr + WM_SLOT_OFF_CONTROL) + 1u));
}

static s32 wm_8a72c_state_9(u32 slot_addr)
{
    if (WM_U8(WM_F36A) != 0xFFu) {
        if (WM_U8(WM_F8E7) == 0u) {
            CALL_97770(3u, 1);
            WM_U16(slot_addr + 0x106u) = (u16)WM_U8(WM_BD60);
            CALL_97770(6u, 8);
        } else {
            CALL_97770(6u, 1);
            WM_U16(WM_U32(WM_POOL_PTR) + 0x306u) = (u16)WM_U8(WM_BD60);
        }
    }
    return wm_8a72c_set_control(
        slot_addr, (u16)(WM_U16(slot_addr + WM_SLOT_OFF_CONTROL) + 1u));
}

static s32 wm_8a72c_state_10(u32 slot_addr)
{
    if (WM_U8(WM_F368) != 0xFFu) {
        if (CALL_97770(4u, 8) == 0)
            return wm_8a72c_epilogue(slot_addr);
        return wm_8a72c_set_control(slot_addr, 0xDu);
    }
    return wm_8a72c_set_control(slot_addr, 0xDu);
}

static s32 wm_8a72c_state_13(u32 slot_addr)
{
    u8 sel = WM_U8(WM_BD60);
    u32 other = WM_U32(WM_POOL_PTR) + ((u32)sel << 7);
    void* object;

    CALL_941C4(slot_addr + WM_SLOT_OFF_X, other + WM_SLOT_OFF_X,
               slot_addr + WM_SLOT_OFF_VX, slot_addr + WM_SLOT_OFF_STATE);
    WM_U32(slot_addr + WM_SLOT_OFF_SX) =
        (u32)wm_sra12_u32(WM_U32(other + WM_SLOT_OFF_X));
    WM_U32(slot_addr + WM_SLOT_OFF_SZ) =
        (u32)wm_sra12_u32(WM_U32(other + WM_SLOT_OFF_Z));
    object = wm_native_from_bits(WM_U32(slot_addr + WM_SLOT_OFF_OBJECT));
    CALL_245D8(object, (s16)1);
    return wm_8a72c_set_control(
        slot_addr, (u16)(WM_U16(slot_addr + WM_SLOT_OFF_CONTROL) + 1u));
}

static s32 wm_8a72c_state_14(u32 slot_addr, s32 slot_index)
{
    s32 approached = CALL_8BEC8(slot_addr);
    if (approached == 3)
        WM_U16(slot_addr + WM_SLOT_OFF_CONTROL) =
            (u16)(WM_U16(slot_addr + WM_SLOT_OFF_CONTROL) + 1u);
    wm_publish_d55c(slot_addr);
    CALL_8C1DC((u32)slot_index + 0x2Eu, slot_addr, WM_SCRATCH);
    return wm_8a72c_epilogue(slot_addr);
}

static s32 wm_8a72c_state_15(u32 slot_addr)
{
    if (CALL_97770((u32)WM_U8(WM_BD60), 4) == 0)
        return wm_8a72c_epilogue(slot_addr);
    WM_U16(slot_addr + WM_SLOT_OFF_FLAG) = 1u;
    WM_U16(slot_addr + WM_SLOT_OFF_CONTROL) = 2u;
    CALL_894C8(0x2Fu);
    return wm_8a72c_epilogue(slot_addr);
}

static s32 wm_8a72c_state_16(u32 slot_addr)
{
    if (WM_U8(WM_F369) != 0xFFu) {
        if (CALL_97770(2u, 1) == 0)
            return wm_8a72c_epilogue(slot_addr);
        WM_U16(slot_addr + 0x86u) = 5u;
    }
    return wm_8a72c_set_control(
        slot_addr, (u16)(WM_U16(slot_addr + WM_SLOT_OFF_CONTROL) + 1u));
}

static s32 wm_8a72c_state_17(u32 slot_addr)
{
    if (WM_U8(WM_F36A) != 0xFFu) {
        if (CALL_97770(3u, 1) == 0)
            return wm_8a72c_epilogue(slot_addr);
        WM_U16(slot_addr + 0x106u) = 6u;
    }
    return wm_8a72c_set_control(
        slot_addr, (u16)(WM_U16(slot_addr + WM_SLOT_OFF_CONTROL) + 1u));
}

static s32 wm_8a72c_state_18(u32 slot_addr)
{
    u32 x = (u32)WM_U16(WM_EF90) << 12;
    u32 z = (u32)WM_U16(WM_EF92) << 12;
    s32 y;
    u16 heading;

    WM_U16(WM_D154) = 0u;
    WM_U32(WM_SCRATCH + 0x30u) = x;
    WM_U32(WM_SCRATCH + 0x38u) = z;
    y = CALL_93978((s32)x, (s32)z);
    heading = WM_U16(WM_EE5A);
    WM_U32(WM_SCRATCH + 0x34u) = (u32)y;
    WM_U16(WM_SCRATCH + 0xA0u) = heading;
    wm_fill_pose_ring(WM_SCRATCH + 0x30u, heading);
    return wm_8a72c_set_control(slot_addr, 0xDu);
}

static s32 wm_8a72c_state_40(u32 slot_addr)
{
    u32 x = (u32)WM_U16(WM_EF90) << 12;
    u32 z = (u32)WM_U16(WM_EF92) << 12;
    u16 heading = WM_U16(WM_EE5A);
    s32 cosv;
    s32 sinv;
    void* object;

    WM_U32(slot_addr + WM_SLOT_OFF_X) = x;
    WM_U32(slot_addr + WM_SLOT_OFF_Z) = z;
    WM_U32(slot_addr + WM_SLOT_OFF_Y) = (u32)CALL_93978((s32)x, (s32)z);
    WM_U32(slot_addr + WM_SLOT_OFF_SIGNED) = (u32)heading;
    WM_U16(slot_addr + WM_SLOT_OFF_STATE) = heading;
    cosv = (s32)CALL_RCOS((long)wm_sign16(heading));
    WM_U32(WM_SCRATCH + 0u) =
        WM_U32(slot_addr + WM_SLOT_OFF_X) + (((u32)cosv * 3u) << 4);
    sinv = (s32)CALL_RSIN((long)wm_sign16(WM_U16(slot_addr + WM_SLOT_OFF_STATE)));
    sinv = (s32)(0u - (u32)sinv);
    WM_U32(WM_SCRATCH + 8u) =
        WM_U32(slot_addr + WM_SLOT_OFF_Z) + (((u32)sinv * 3u) << 4);
    CALL_941C4(slot_addr + WM_SLOT_OFF_X, WM_SCRATCH,
               slot_addr + WM_SLOT_OFF_VX, slot_addr + WM_SLOT_OFF_STATE);
    WM_U32(slot_addr + WM_SLOT_OFF_SX) =
        (u32)wm_sra12_u32(WM_U32(WM_SCRATCH + 0u));
    WM_U16(slot_addr + WM_SLOT_OFF_FLAG) = 0u;
    WM_U32(slot_addr + WM_SLOT_OFF_SZ) =
        (u32)wm_sra12_u32(WM_U32(WM_SCRATCH + 8u));
    object = wm_native_from_bits(WM_U32(slot_addr + WM_SLOT_OFF_OBJECT));
    CALL_245D8(object, (s16)1);
    return wm_8a72c_set_control(
        slot_addr, (u16)(WM_U16(slot_addr + WM_SLOT_OFF_CONTROL) + 1u));
}

static s32 wm_8a72c_state_41(u32 slot_addr)
{
    if (CALL_8BEC8(slot_addr) == 3) {
        void* object =
            wm_native_from_bits(WM_U32(slot_addr + WM_SLOT_OFF_OBJECT));
        CALL_245D8(object, (s16)0);
        WM_U32(slot_addr + WM_SLOT_OFF_VZ) = 0u;
        WM_U32(slot_addr + WM_SLOT_OFF_VY) = 0u;
        WM_U32(slot_addr + WM_SLOT_OFF_VX) = 0u;
        WM_U16(slot_addr + WM_SLOT_OFF_CONTROL) =
            (u16)(WM_U16(slot_addr + WM_SLOT_OFF_CONTROL) + 1u);
    }
    wm_publish_d55c(slot_addr);
    return wm_8a72c_epilogue(slot_addr);
}

static s32 wm_8a72c_state_42(u32 slot_addr)
{
    WM_U8(WM_F8E5) = 0u;
    WM_U32(slot_addr + WM_SLOT_OFF_COUNTER) = 1u;
    WM_U16(WM_D154) = 0u;
    wm_copy4(WM_SCRATCH, slot_addr + WM_SLOT_OFF_X);
    WM_U16(WM_SCRATCH + 0xA0u) = WM_U16(slot_addr + WM_SLOT_OFF_STATE);
    wm_fill_pose_ring(WM_SCRATCH, WM_U16(WM_SCRATCH + 0xA0u));
    WM_U16(slot_addr + WM_SLOT_OFF_CONTROL) =
        (u16)(WM_U16(slot_addr + WM_SLOT_OFF_CONTROL) + 1u);
    return wm_8a72c_c170_gate(slot_addr);
}

static s32 wm_8a72c_state_44(u32 slot_addr)
{
    if (WM_U8(WM_F369) != 0xFFu) {
        if (CALL_97770(2u, 5) == 0)
            return wm_8a72c_epilogue(slot_addr);
    }
    return wm_8a72c_set_control(
        slot_addr, (u16)(WM_U16(slot_addr + WM_SLOT_OFF_CONTROL) + 1u));
}

static s32 wm_8a72c_state_45(u32 slot_addr)
{
    if (WM_U8(WM_F36A) != 0xFFu) {
        if (CALL_97770(3u, 5) == 0)
            return wm_8a72c_epilogue(slot_addr);
    }
    return wm_8a72c_set_control(slot_addr, 0x40u);
}

s32 wm_8008A72C(s32 slot_index)
{
    u32 slot_addr = WM_U32(WM_POOL_PTR) + ((u32)slot_index << 7);
    s32 claim = wm_sign16(WM_U16(slot_addr + WM_SLOT_OFF_CLAIM));
    s32 control;

    if (claim == 3) {
        WM_U16(slot_addr + WM_SLOT_OFF_CLAIM) = 0u;
        WM_U16(slot_addr + WM_SLOT_OFF_CONTROL) = 1u;
    } else if (claim == 2) {
        WM_U16(slot_addr + WM_SLOT_OFF_CLAIM) = 0u;
#if defined(WM_8A72C_MUTANT_PLUS4_CASE2)
        WM_U16(slot_addr + WM_SLOT_OFF_CONTROL) = 1u;
#else
        WM_U16(slot_addr + WM_SLOT_OFF_CONTROL) = 0x28u;
#endif
    } else if (claim == 6) {
        u32 counter = WM_U32(slot_addr + WM_SLOT_OFF_COUNTER) + 1u;
        WM_U16(slot_addr + WM_SLOT_OFF_CLAIM) = 0u;
        WM_U32(slot_addr + WM_SLOT_OFF_COUNTER) = counter;
        if (WM_U32(WM_C170) == counter) {
            WM_U16(slot_addr + WM_SLOT_OFF_CONTROL) = 0u;
            WM_U32(WM_MODE) = 1u;
            WM_U16(WM_BD04) = 0u;
        }
    }

    control = wm_sign16(WM_U16(slot_addr + WM_SLOT_OFF_CONTROL));
#if defined(WM_8A72C_MUTANT_BOUND_OFF_BY_ONE)
    if ((u32)control >= 64u)
        return wm_8a72c_epilogue(slot_addr);
#else
    if ((u32)control >= 65u)
        return wm_8a72c_epilogue(slot_addr);
#endif

    switch (control) {
    case 0:
    case 1:
        return wm_8a72c_state_01(slot_addr);
    case 2:
    case 3:
        return wm_8a72c_state_23(slot_addr);
    case 8:
        return wm_8a72c_state_8(slot_addr);
    case 9:
        return wm_8a72c_state_9(slot_addr);
    case 10:
        return wm_8a72c_state_10(slot_addr);
    case 13:
        return wm_8a72c_state_13(slot_addr);
    case 14:
        return wm_8a72c_state_14(slot_addr, slot_index);
    case 15:
        return wm_8a72c_state_15(slot_addr);
    case 16:
        return wm_8a72c_state_16(slot_addr);
    case 17:
        return wm_8a72c_state_17(slot_addr);
    case 18:
        return wm_8a72c_state_18(slot_addr);
    case 40:
        return wm_8a72c_state_40(slot_addr);
    case 41:
        return wm_8a72c_state_41(slot_addr);
    case 42:
        return wm_8a72c_state_42(slot_addr);
    case 43:
        return wm_8a72c_c170_gate(slot_addr);
    case 44:
        return wm_8a72c_state_44(slot_addr);
    case 45:
        return wm_8a72c_state_45(slot_addr);
    default:
        return wm_8a72c_epilogue(slot_addr);
    }
}
