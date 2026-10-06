#include "common.h"
/* Retail TU split: gcc 2.7.2 cc1 emits every file-scope INCLUDE_ASM block
 * before the first compiled body, so a mixed TU whose retail layout has a C
 * function ahead of an assembly function cannot be one object.  Each
 * contiguous [asm run][C run] of the retail layout is therefore its own
 * matching TU: src/battle/main72_p<N>.c defines BATTLE_TU_PART=<N> and
 * includes this file.  Host/port builds compile this file whole (part 0). */
#ifndef BATTLE_TU_PART
#define BATTLE_TU_PART 0
#endif
#define BATTLE_PART(n) (BATTLE_TU_PART == 0 || BATTLE_TU_PART == (n))


extern void func_800A3490(void);
extern void func_800A3514(void);
extern void func_800A3578(void);
extern void func_800A35C8(void);

#if BATTLE_PART(1)
/* func_800AA820.s: dispatch arg0 to a func_800A34xx/35xx entry point:
 * 2 -> func_800A3578, 1 -> func_800A3514, 3 -> func_800A35C8,
 * anything else -> func_800A3490 (the < 3 test is signed).  Retail
 * materialises each address in its own arm and leaves through one exit. */
void* func_800AA820(s32 arg0) {
    /* Case order matters: cc1 emits the arm blocks in source order and retail
     * lays out case 1 before case 2. */
    switch (arg0) {
    case 1:
        return func_800A3514;
    case 2:
        return func_800A3578;
    case 3:
        return func_800A35C8;
    }
    return func_800A3490;
}

/* func_800AA898.s: init the struct at p (a1 unused): words from a2/a3,
 * marker bytes 0xFF/0x6B, zeroed tables, -1 slots, and 1 at +0x8E.
 * Retail v0 is 1 on return. */
/* Not yet byte-matching: the matching build assembles retail bytes for
 * this function from its own asm segment (see config/battle.yaml).  The
 * body below is kept for the port and the differential harnesses. */
#ifdef XENO_PC_PORT
u32 func_800AA898(u8* p, u32 a1, u32 a2, u32 a3) {
    (void)a1;
    *(u16*)(p + 0x3C) = 0xFFFF;
    *(p + 0x5C) = 0xFF;
    *(p + 0x39) = 0x6B;
    *(u32*)(p + 0x8) = a2;
    *(u32*)(p + 0xC) = 0;
    *(u32*)(p + 0x10) = 0;
    *(u32*)(p + 0x14) = a3;
    *(u32*)(p + 0x18) = 0;
    *(p + 0x2B) = 0;
    *(u16*)(p + 0x98) = 0xFFFF;
    *(u16*)(p + 0x58) = 0;
    *(p + 0x35) = 0;
    *(p + 0x37) = 0;
    *(p + 0x38) = 0;
    *(u16*)(p + 0x3A) = 0xFFFF;
    *(u16*)(p + 0x70) = 0;
    *(u16*)(p + 0x72) = 0;
    *(u16*)(p + 0x74) = 0;
    *(u16*)(p + 0x76) = 0;
    *(u16*)(p + 0x78) = 0;
    *(u16*)(p + 0x7A) = 0;
    *(u16*)(p + 0x7C) = 0;
    *(u16*)(p + 0x7E) = 0;
    *(u16*)(p + 0x80) = 0;
    *(u16*)(p + 0x82) = 0;
    *(u16*)(p + 0x84) = 0;
    *(u16*)(p + 0x86) = 0;
    *(u16*)(p + 0x88) = 0;
    *(u16*)(p + 0x8A) = 0;
    *(u16*)(p + 0x8C) = 0;
    *(u16*)(p + 0x8E) = 1;
    *(p + 0x36) = 0;
    *(u16*)(p + 0x1E) = 0xFFFF;
    return 1;
}
#endif /* XENO_PC_PORT */
#endif /* BATTLE_PART(1) */
#if BATTLE_PART(2)
#endif /* BATTLE_PART(2) */
#if BATTLE_PART(2)
/* func_800AE1BC.s: if *(u16*)(a1 + 0x12) == 0, mark +0x98 with -1 and
 * return -1; else clear +0x98 (delay slot), select +0x9A from a2,
 * mirror the +0x12 halfword to +0x9E, and store a1 + *(u32*)(a1 + 0x14)
 * to +0xA0/+0xA4, returning it. */
/* Not yet byte-matching: the matching build assembles retail bytes for
 * this function from its own asm segment (see config/battle.yaml).  The
 * body below is kept for the port and the differential harnesses. */
#ifdef XENO_PC_PORT
u32 func_800AE1BC(u8* a0, u8* a1, u32 a2) {
    u32 ret;
    if (*(u16*)(a1 + 0x12) == 0) {
        ret = 0xFFFFFFFFu;
        *(u16*)(a0 + 0x98) = 0xFFFF;
    } else {
        u16 w;
        *(u16*)(a0 + 0x98) = 0;
        if (a2 == 0) {
            w = 0xFFFF;
        } else {
            w = *(u16*)(a1 + 0x2);
        }
        *(u16*)(a0 + 0x9A) = w;
        *(u16*)(a0 + 0x9C) = 0;
        *(u16*)(a0 + 0x9E) = *(u16*)(a1 + 0x12);
        ret = (u32)a1 + *(u32*)(a1 + 0x14);
        *(u32*)(a0 + 0xA0) = ret;
        *(u32*)(a0 + 0xA4) = ret;
    }
    return ret;
}
#endif /* XENO_PC_PORT */
#endif /* BATTLE_PART(2) */
extern u8* D_8005919C;
#ifndef XENO_PC_PORT
extern u8* D_800C4924;
#endif

#if BATTLE_PART(2)
/* func_800AE220.s: select a node by a1 (0 -> D_8005919C, 1 -> +0xB0 chain,
 * 2 -> +0xB4 chain, 3 -> D_800C4924) and return *(u16*)(node + 0x14) << 16;
 * any other a1 returns 3 (falls into jr with the leftover v0 unshifted).
 * Node slots are 32-bit retail pointers, loaded as u32 so the host build
 * performs the same 4-byte load (a native 8-byte load at +0xB4 would be
 * misaligned on a 64-bit host).  The a1 == 3 arm returns on its own, so the
 * halfword load and shift appear twice, as they do in retail. */
/* Not yet byte-matching: the matching build assembles retail bytes for
 * this function from its own asm segment (see config/battle.yaml).  The
 * body below is kept for the port and the differential harnesses. */
#ifdef XENO_PC_PORT
u32 func_800AE220(u8* a0, u32 a1) {
    u8* p;

    if (a1 == 0) {
        p = D_8005919C;
    } else if (a1 == 1) {
        p = (u8*)(uintptr_t)(*(u32*)(a0 + 0xB0));
        p = (u8*)(uintptr_t)(*(u32*)(p + 8));
    } else if (a1 == 2) {
        p = (u8*)(uintptr_t)(*(u32*)(a0 + 0xB4));
        p = (u8*)(uintptr_t)(*(u32*)(p + 8));
    } else if (a1 == 3) {
        return (u32)(*(u16*)(D_800C4924 + 0x14)) << 16;
    } else {
        return 3;
    }
    return (u32)(*(u16*)(p + 0x14)) << 16;
}
#endif /* XENO_PC_PORT */

#endif /* BATTLE_PART(2) */
#if BATTLE_PART(2)
/* func_800AAA20.s: step obj's motion `steps` times (when +0x34 is set),
 * OR-ing the per-step flags of func_800A0838, then hand them to
 * func_800AAD54.  The matched C is main72_p2.c (retail-only there); this is
 * its port body, statement for statement, with each callee that is still
 * interpreted (func_800AFF9C, func_8009F1C4, func_8009EF3C, func_800A0838,
 * func_800AE2A4, func_800AAD54) reached through PcPort_BattleMipsCallGuest
 * with the retail argument words, the fifth func_800AAD54 word in the guest
 * stack slot as retail passes it.  obj is the bridge's host pointer into
 * guest RAM, so it goes back out as its guest address.
 *
 * Return: when *obj == 0 retail returns the caller's s2 unchanged (v0 = s2,
 * never assigned on that path).  The one retail caller, func_800A9A50 at
 * 800A9DB4, does not read v0, so the port body returns a defined 0 there. */
#ifdef XENO_PC_PORT
#include "battle_host_guest.h"
#include <stdio.h>
#include <stdlib.h>
static u32 m72_guest_call(u32 target, const u32* args, unsigned argc) {
    u32 v0 = 0;

    if (PcPort_BattleMipsCallGuest(target, args, argc, &v0) != 0) {
        fprintf(stderr, "[xeno-port][battle] func_800AAA20: guest call 0x%08x failed\n",
                target);
        abort();
    }
    return v0;
}

s32 func_800AAA20(u8* obj, s32 id, s32 steps, s32 a3, s32 a4) {
    u32 flags;
    u32 args[5];
    s32 i;

    if (*(u32*)obj == 0) {
        return 0;
    }
    flags = 0;
    if (obj[0x34] != 0) {
        /* The matched C declares func_800AFF9C(void), but retail reaches
         * it with a0 still holding obj, and func_800AFF9C reads it. */
        args[0] = BG_ADDR(obj);
        m72_guest_call(0x800AFF9Cu, args, 1); /* func_800AFF9C(obj) */
        args[0] = *(u32*)(obj + 4);
        args[1] = (u32)(s32)*(s16*)(obj + 0x1C);
        if (obj[0x37] != 0) {
            m72_guest_call(0x8009F1C4u, args, 2); /* func_8009F1C4 */
        } else {
            m72_guest_call(0x8009EF3Cu, args, 2); /* func_8009EF3C */
        }
        for (i = 0; i < steps; i++) {
            args[0] = (u32)id;
            args[1] = *(u32*)(obj + 4);
            args[2] = *(u16*)(obj + 0x3C);
            args[3] = (u32)(s32)*(s16*)(obj + 0x1C);
            flags |= m72_guest_call(0x800A0838u, args, 4); /* func_800A0838 */
            args[0] = BG_ADDR(obj);
            args[1] = (u32)id;
            args[2] = (u32)a3;
            m72_guest_call(0x800AE2A4u, args, 3); /* func_800AE2A4 */
        }
    }
    args[0] = BG_ADDR(obj);
    args[1] = (u32)id;
    args[2] = flags;
    args[3] = (u32)steps;
    args[4] = (u32)a4;
    m72_guest_call(0x800AAD54u, args, 5); /* func_800AAD54 */
    return (s32)flags;
}
#endif /* XENO_PC_PORT */
#endif /* BATTLE_PART(2) */
#if BATTLE_PART(3)
#endif /* BATTLE_PART(3) */


#if BATTLE_PART(3)
/* func_800AEEEC.s */
void func_800AEEEC(u8* p) {
    *(s16*)(p + 0x98) = -1;
}
#endif /* BATTLE_PART(3) */
