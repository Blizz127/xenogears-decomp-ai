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
#ifdef XENO_PC_PORT
/* func_800AAD54.s (3186 instructions; INCLUDE_ASM in main72_p2.c): the
 * motion-state update 800AAA20 ends with (obj, id, step flags, steps, a4 in
 * the guest stack slot), generated from the retail listing by
 *   tools/scripts/battle_lift_asm.py asm/battle/nonmatchings/main72_p2/func_800AAD54.s --args 5 --jal-service
 * (one statement per MIPS instruction; include/battle_lift.h).  It reaches
 * archive, heap, sound, sprite and image services whose interpreter-bridge
 * binding carries runtime hooks (the file-1 identity and RESULTS archive
 * gates, HeapAlloc's guest-domain return), so EVERY call goes through
 * PcPort_BattleMipsCallJal: the interpreter's own jal path, native wherever
 * the bridge binds the target, interpreted otherwise.  Overlay callees
 * without a native binding therefore still run in the interpreter (see the
 * commit for the list). */
#include "battle_lift.h"
void func_800AAD54(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4) {
    static const char FN[] = "func_800AAD54";
    u32 r_at = 0, r_v0 = 0, r_v1 = 0, r_a0 = 0, r_a1 = 0, r_a2 = 0, r_a3 = 0, r_t0 = 0, r_t1 = 0, r_t2 = 0, r_t3 = 0, r_t4 = 0, r_t5 = 0, r_t6 = 0, r_t7 = 0, r_s0 = 0, r_s1 = 0, r_s2 = 0, r_s3 = 0, r_s4 = 0, r_s5 = 0, r_s6 = 0, r_s7 = 0, r_t8 = 0, r_t9 = 0, r_k0 = 0, r_k1 = 0, r_gp = 0, r_sp = 0, r_fp = 0, r_ra = 0;
    u32 hi = 0, lo = 0, bl_saved;
    r_sp = bl_enter(FN, 0x1F8u, &bl_saved);
    r_a0 = arg0;
    r_a1 = arg1;
    r_a2 = arg2;
    r_a3 = arg3;
    bl_sw(r_sp + 0x10u, arg4);
    r_sp = r_sp + 0xFFFFFE08u; /* 800AAD54 addiu $sp, $sp, -0x1F8 */
    bl_sw(r_sp + 0x1D0u, r_s0); /* 800AAD58 sw $s0, 0x1D0($sp) */
    r_s0 = bl_lw(r_sp + 0x208u); /* 800AAD5C lw $s0, 0x208($sp) */
    bl_sw(r_sp + 0x1E0u, r_s4); /* 800AAD60 sw $s4, 0x1E0($sp) */
    r_s4 = r_a0 + 0u; /* 800AAD64 addu $s4, $a0, $zero */
    bl_sw(r_sp + 0x1F4u, r_ra); /* 800AAD68 sw $ra, 0x1F4($sp) */
    bl_sw(r_sp + 0x1F0u, r_fp); /* 800AAD6C sw $fp, 0x1F0($sp) */
    bl_sw(r_sp + 0x1ECu, r_s7); /* 800AAD70 sw $s7, 0x1EC($sp) */
    bl_sw(r_sp + 0x1E8u, r_s6); /* 800AAD74 sw $s6, 0x1E8($sp) */
    bl_sw(r_sp + 0x1E4u, r_s5); /* 800AAD78 sw $s5, 0x1E4($sp) */
    bl_sw(r_sp + 0x1DCu, r_s3); /* 800AAD7C sw $s3, 0x1DC($sp) */
    bl_sw(r_sp + 0x1D8u, r_s2); /* 800AAD80 sw $s2, 0x1D8($sp) */
    bl_sw(r_sp + 0x1D4u, r_s1); /* 800AAD84 sw $s1, 0x1D4($sp) */
    bl_sw(r_sp + 0x130u, r_a1); /* 800AAD88 sw $a1, 0x130($sp) */
    bl_sw(r_sp + 0x138u, r_a2); /* 800AAD8C sw $a2, 0x138($sp) */
    /* 800AAD90 beqz $a3, .L800ADEE8 */
    { int c_ = (r_a3 == 0); bl_sw(r_sp + 0x140u, r_a3); if (c_) goto L_800ADEE8; }
    r_v0 = bl_lw(r_s4 + 0x10u); /* 800AAD98 lw $v0, 0x10($s4) */
    ; /* 800AAD9C nop  */
    /* 800AADA0 beqz $v0, .L800ADEE8 */
    { int c_ = (r_v0 == 0); r_a0 = 0u | 0x4u; if (c_) goto L_800ADEE8; }
    /* 800AADA8 jal HeapChangeCurrentUser */
    r_a1 = 0u + 0u;
    bl_jal(FN, 0x80032498u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* HeapChangeCurrentUser */
    r_v0 = r_s0 + 0x1u; /* 800AADB0 addiu $v0, $s0, 0x1 */
    bl_sw(r_sp + 0x98u, 0u); /* 800AADB4 sw $zero, 0x98($sp) */
    bl_sw(r_sp + 0x158u, 0u); /* 800AADB8 sw $zero, 0x158($sp) */
    /* 800AADBC blez $v0, .L800AAF90 */
    { int c_ = ((s32)r_v0 <= 0); bl_sw(r_sp + 0x148u, r_s4); if (c_) goto L_800AAF90; }
L_800AADC4:
    r_v0 = bl_lhu(r_s4 + 0x70u); /* 800AADC4 lhu $v0, 0x70($s4) */
    r_a0 = bl_lhu(r_s4 + 0x76u); /* 800AADC8 lhu $a0, 0x76($s4) */
    r_v1 = bl_lhu(r_s4 + 0x72u); /* 800AADCC lhu $v1, 0x72($s4) */
    r_a1 = bl_lhu(r_s4 + 0x78u); /* 800AADD0 lhu $a1, 0x78($s4) */
    r_v0 = r_v0 + r_a0; /* 800AADD4 addu $v0, $v0, $a0 */
    bl_sh(r_s4 + 0x70u, r_v0); /* 800AADD8 sh $v0, 0x70($s4) */
    r_v0 = bl_lhu(r_s4 + 0x74u); /* 800AADDC lhu $v0, 0x74($s4) */
    r_a0 = bl_lhu(r_s4 + 0x7Au); /* 800AADE0 lhu $a0, 0x7A($s4) */
    r_v1 = r_v1 + r_a1; /* 800AADE4 addu $v1, $v1, $a1 */
    bl_sh(r_s4 + 0x72u, r_v1); /* 800AADE8 sh $v1, 0x72($s4) */
    r_v1 = bl_lhu(r_s4 + 0x7Cu); /* 800AADEC lhu $v1, 0x7C($s4) */
    r_a1 = bl_lhu(r_s4 + 0x82u); /* 800AADF0 lhu $a1, 0x82($s4) */
    r_v0 = r_v0 + r_a0; /* 800AADF4 addu $v0, $v0, $a0 */
    bl_sh(r_s4 + 0x74u, r_v0); /* 800AADF8 sh $v0, 0x74($s4) */
    r_v0 = bl_lhu(r_s4 + 0x7Eu); /* 800AADFC lhu $v0, 0x7E($s4) */
    r_a0 = bl_lhu(r_s4 + 0x84u); /* 800AAE00 lhu $a0, 0x84($s4) */
    r_v1 = r_v1 + r_a1; /* 800AAE04 addu $v1, $v1, $a1 */
    bl_sh(r_s4 + 0x7Cu, r_v1); /* 800AAE08 sh $v1, 0x7C($s4) */
    r_v1 = bl_lhu(r_s4 + 0x80u); /* 800AAE0C lhu $v1, 0x80($s4) */
    r_a1 = bl_lhu(r_s4 + 0x86u); /* 800AAE10 lhu $a1, 0x86($s4) */
    r_v0 = r_v0 + r_a0; /* 800AAE14 addu $v0, $v0, $a0 */
    r_v1 = r_v1 + r_a1; /* 800AAE18 addu $v1, $v1, $a1 */
    bl_sh(r_s4 + 0x80u, r_v1); /* 800AAE1C sh $v1, 0x80($s4) */
    r_v1 = bl_lhu(r_s4 + 0x70u); /* 800AAE20 lhu $v1, 0x70($s4) */
    r_a0 = bl_lw(r_s4 + 0x4u); /* 800AAE24 lw $a0, 0x4($s4) */
    bl_sh(r_s4 + 0x7Eu, r_v0); /* 800AAE28 sh $v0, 0x7E($s4) */
    r_v1 = r_v1 << 16; /* 800AAE2C sll $v1, $v1, 16 */
    r_v0 = bl_lhu(r_a0 + 0x54u); /* 800AAE30 lhu $v0, 0x54($a0) */
    r_v1 = BL_SRA(r_v1, 19); /* 800AAE34 sra $v1, $v1, 19 */
    r_v0 = r_v0 + r_v1; /* 800AAE38 addu $v0, $v0, $v1 */
    bl_sh(r_a0 + 0x54u, r_v0); /* 800AAE3C sh $v0, 0x54($a0) */
    r_v1 = bl_lhu(r_s4 + 0x72u); /* 800AAE40 lhu $v1, 0x72($s4) */
    r_a0 = bl_lw(r_s4 + 0x4u); /* 800AAE44 lw $a0, 0x4($s4) */
    r_v1 = r_v1 << 16; /* 800AAE48 sll $v1, $v1, 16 */
    r_v0 = bl_lhu(r_a0 + 0x56u); /* 800AAE4C lhu $v0, 0x56($a0) */
    r_v1 = BL_SRA(r_v1, 19); /* 800AAE50 sra $v1, $v1, 19 */
    r_v0 = r_v0 + r_v1; /* 800AAE54 addu $v0, $v0, $v1 */
    bl_sh(r_a0 + 0x56u, r_v0); /* 800AAE58 sh $v0, 0x56($a0) */
    r_v1 = bl_lhu(r_s4 + 0x74u); /* 800AAE5C lhu $v1, 0x74($s4) */
    r_a0 = bl_lw(r_s4 + 0x4u); /* 800AAE60 lw $a0, 0x4($s4) */
    r_v1 = r_v1 << 16; /* 800AAE64 sll $v1, $v1, 16 */
    r_v0 = bl_lhu(r_a0 + 0x58u); /* 800AAE68 lhu $v0, 0x58($a0) */
    r_v1 = BL_SRA(r_v1, 19); /* 800AAE6C sra $v1, $v1, 19 */
    r_v0 = r_v0 + r_v1; /* 800AAE70 addu $v0, $v0, $v1 */
    bl_sh(r_a0 + 0x58u, r_v0); /* 800AAE74 sh $v0, 0x58($a0) */
    r_v0 = bl_lw(r_s4 + 0x4u); /* 800AAE78 lw $v0, 0x4($s4) */
    r_v1 = bl_lh(r_s4 + 0x7Cu); /* 800AAE7C lh $v1, 0x7C($s4) */
    r_v0 = bl_lh(r_v0 + 0x4Cu); /* 800AAE80 lh $v0, 0x4C($v0) */
    ; /* 800AAE84 nop  */
    BL_MULT(r_v1, r_v0); /* 800AAE88 mult $v1, $v0 */
    r_v0 = lo; /* 800AAE8C mflo $v0 */
    r_v0 = BL_SRA(r_v0, 12); /* 800AAE90 sra $v0, $v0, 12 */
    bl_sh(r_sp + 0x60u, r_v0); /* 800AAE94 sh $v0, 0x60($sp) */
    r_v0 = bl_lw(r_s4 + 0x4u); /* 800AAE98 lw $v0, 0x4($s4) */
    r_v1 = bl_lh(r_s4 + 0x7Eu); /* 800AAE9C lh $v1, 0x7E($s4) */
    r_v0 = bl_lh(r_v0 + 0x4Eu); /* 800AAEA0 lh $v0, 0x4E($v0) */
    ; /* 800AAEA4 nop  */
    BL_MULT(r_v1, r_v0); /* 800AAEA8 mult $v1, $v0 */
    r_v0 = lo; /* 800AAEAC mflo $v0 */
    r_v0 = BL_SRA(r_v0, 12); /* 800AAEB0 sra $v0, $v0, 12 */
    bl_sh(r_sp + 0x62u, r_v0); /* 800AAEB4 sh $v0, 0x62($sp) */
    r_v0 = bl_lw(r_s4 + 0x4u); /* 800AAEB8 lw $v0, 0x4($s4) */
    r_v1 = bl_lh(r_s4 + 0x80u); /* 800AAEBC lh $v1, 0x80($s4) */
    r_v0 = bl_lh(r_v0 + 0x50u); /* 800AAEC0 lh $v0, 0x50($v0) */
    ; /* 800AAEC4 nop  */
    BL_MULT(r_v1, r_v0); /* 800AAEC8 mult $v1, $v0 */
    r_a2 = r_sp + 0x50u; /* 800AAECC addiu $a2, $sp, 0x50 */
    r_v0 = lo; /* 800AAED0 mflo $v0 */
    r_v0 = BL_SRA(r_v0, 12); /* 800AAED4 sra $v0, $v0, 12 */
    bl_sh(r_sp + 0x64u, r_v0); /* 800AAED8 sh $v0, 0x64($sp) */
    r_a0 = bl_lw(r_s4 + 0x4u); /* 800AAEDC lw $a0, 0x4($s4) */
    r_a1 = r_sp + 0x60u; /* 800AAEE0 addiu $a1, $sp, 0x60 */
    /* 800AAEE4 jal ApplyMatrix */
    r_a0 = r_a0 + 0x2Cu;
    bl_jal(FN, 0x80049CECu, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* ApplyMatrix */
    r_v1 = bl_lh(r_s4 + 0x1Cu); /* 800AAEEC lh $v1, 0x1C($s4) */
    r_v0 = bl_lw(r_sp + 0x50u); /* 800AAEF0 lw $v0, 0x50($sp) */
    ; /* 800AAEF4 nop  */
    BL_MULT(r_v1, r_v0); /* 800AAEF8 mult $v1, $v0 */
    r_a0 = bl_lw(r_s4 + 0x4u); /* 800AAEFC lw $a0, 0x4($s4) */
    ; /* 800AAF00 nop  */
    r_v1 = bl_lw(r_a0 + 0x5Cu); /* 800AAF04 lw $v1, 0x5C($a0) */
    r_v0 = lo; /* 800AAF08 mflo $v0 */
    r_v0 = BL_SRA(r_v0, 12); /* 800AAF0C sra $v0, $v0, 12 */
    r_v0 = r_v0 + r_v1; /* 800AAF10 addu $v0, $v0, $v1 */
    bl_sw(r_a0 + 0x5Cu, r_v0); /* 800AAF14 sw $v0, 0x5C($a0) */
    r_v1 = bl_lh(r_s4 + 0x1Cu); /* 800AAF18 lh $v1, 0x1C($s4) */
    r_v0 = bl_lw(r_sp + 0x54u); /* 800AAF1C lw $v0, 0x54($sp) */
    ; /* 800AAF20 nop  */
    BL_MULT(r_v1, r_v0); /* 800AAF24 mult $v1, $v0 */
    r_a0 = bl_lw(r_s4 + 0x4u); /* 800AAF28 lw $a0, 0x4($s4) */
    ; /* 800AAF2C nop  */
    r_v1 = bl_lw(r_a0 + 0x60u); /* 800AAF30 lw $v1, 0x60($a0) */
    r_v0 = lo; /* 800AAF34 mflo $v0 */
    r_v0 = BL_SRA(r_v0, 12); /* 800AAF38 sra $v0, $v0, 12 */
    r_v0 = r_v0 + r_v1; /* 800AAF3C addu $v0, $v0, $v1 */
    bl_sw(r_a0 + 0x60u, r_v0); /* 800AAF40 sw $v0, 0x60($a0) */
    r_v1 = bl_lh(r_s4 + 0x1Cu); /* 800AAF44 lh $v1, 0x1C($s4) */
    r_v0 = bl_lw(r_sp + 0x58u); /* 800AAF48 lw $v0, 0x58($sp) */
    ; /* 800AAF4C nop  */
    BL_MULT(r_v1, r_v0); /* 800AAF50 mult $v1, $v0 */
    r_a0 = bl_lw(r_sp + 0x98u); /* 800AAF54 lw $a0, 0x98($sp) */
    ; /* 800AAF58 nop  */
    r_a0 = r_a0 + 0x1u; /* 800AAF5C addiu $a0, $a0, 0x1 */
    bl_sw(r_sp + 0x98u, r_a0); /* 800AAF60 sw $a0, 0x98($sp) */
    r_a1 = bl_lw(r_s4 + 0x4u); /* 800AAF64 lw $a1, 0x4($s4) */
    ; /* 800AAF68 nop  */
    r_v1 = bl_lw(r_a1 + 0x64u); /* 800AAF6C lw $v1, 0x64($a1) */
    r_v0 = lo; /* 800AAF70 mflo $v0 */
    r_v0 = BL_SRA(r_v0, 12); /* 800AAF74 sra $v0, $v0, 12 */
    r_v0 = r_v0 + r_v1; /* 800AAF78 addu $v0, $v0, $v1 */
    bl_sw(r_a1 + 0x64u, r_v0); /* 800AAF7C sw $v0, 0x64($a1) */
    r_v0 = r_s0 + 0x1u; /* 800AAF80 addiu $v0, $s0, 0x1 */
    r_a0 = (u32)((s32)r_a0 < (s32)r_v0); /* 800AAF84 slt $a0, $a0, $v0 */
    /* 800AAF88 bnez $a0, .L800AADC4 */
    { int c_ = (r_a0 != 0); ; if (c_) goto L_800AADC4; }
L_800AAF90:
    r_t8 = 0u | 0x1u; /* 800AAF90 ori $t8, $zero, 0x1 */
    bl_sw(r_sp + 0x150u, r_t8); /* 800AAF94 sw $t8, 0x150($sp) */
    r_v0 = bl_lw(r_s4 + 0x4Cu); /* 800AAF98 lw $v0, 0x4C($s4) */
    r_s3 = bl_lw(r_s4 + 0x10u); /* 800AAF9C lw $s3, 0x10($s4) */
    /* 800AAFA0 beqz $v0, .L800AAFD0 */
    { int c_ = (r_v0 == 0); ; if (c_) goto L_800AAFD0; }
    /* 800AAFA8 jal func_800AEEF8 */
    r_a0 = r_s4 + 0u;
    bl_jal(FN, 0x800AEEF8u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800AEEF8 */
    r_v1 = bl_lh(r_s4 + 0x48u); /* 800AAFB0 lh $v1, 0x48($s4) */
    ; /* 800AAFB4 nop  */
    r_v1 = (u32)((s32)r_v1 < (s32)r_v0); /* 800AAFB8 slt $v1, $v1, $v0 */
    /* 800AAFBC bnez $v1, .L800AAFD0 */
    { int c_ = (r_v1 != 0); ; if (c_) goto L_800AAFD0; }
    r_s3 = bl_lw(r_s4 + 0x4Cu); /* 800AAFC4 lw $s3, 0x4C($s4) */
    /* 800AAFC8 j .L800AB0AC */
    bl_sw(r_s4 + 0x4Cu, 0u); goto L_800AB0AC;
L_800AAFD0:
    r_v0 = bl_lw(r_s4 + 0x54u); /* 800AAFD0 lw $v0, 0x54($s4) */
    ; /* 800AAFD4 nop  */
    /* 800AAFD8 beqz $v0, .L800AB06C */
    { int c_ = (r_v0 == 0); r_s0 = r_sp + 0x78u; if (c_) goto L_800AB06C; }
    r_v0 = bl_lw(r_s4 + 0x4u); /* 800AAFE0 lw $v0, 0x4($s4) */
    ; /* 800AAFE4 nop  */
    r_v0 = bl_lhu(r_v0 + 0x5Cu); /* 800AAFE8 lhu $v0, 0x5C($v0) */
    bl_sh(r_sp + 0x7Au, 0u); /* 800AAFEC sh $zero, 0x7A($sp) */
    bl_sh(r_sp + 0x78u, r_v0); /* 800AAFF0 sh $v0, 0x78($sp) */
    r_v0 = bl_lw(r_s4 + 0x4u); /* 800AAFF4 lw $v0, 0x4($s4) */
    ; /* 800AAFF8 nop  */
    r_v0 = bl_lhu(r_v0 + 0x64u); /* 800AAFFC lhu $v0, 0x64($v0) */
    r_a0 = r_s0 + 0u; /* 800AB000 addu $a0, $s0, $zero */
    bl_sh(r_sp + 0x7Cu, r_v0); /* 800AB004 sh $v0, 0x7C($sp) */
    r_a1 = bl_lh(r_s4 + 0x1Eu); /* 800AB008 lh $a1, 0x1E($s4) */
    /* 800AB00C jal func_800A5914 */
    r_a2 = 0u | 0x4u;
    bl_jal(FN, 0x800A5914u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800A5914 */
    bl_sh(r_s4 + 0x1Eu, r_v0); /* 800AB014 sh $v0, 0x1E($s4) */
    r_v0 = r_v0 << 16; /* 800AB018 sll $v0, $v0, 16 */
    /* 800AB01C bgez $v0, .L800AB034 */
    { int c_ = ((s32)r_v0 >= 0); r_a0 = r_s0 + 0u; if (c_) goto L_800AB034; }
    /* 800AB024 jal func_800A579C */
    r_a0 = r_s0 + 0u;
    bl_jal(FN, 0x800A579Cu, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800A579C */
    bl_sh(r_s4 + 0x1Eu, r_v0); /* 800AB02C sh $v0, 0x1E($s4) */
    r_a0 = r_s0 + 0u; /* 800AB030 addu $a0, $s0, $zero */
L_800AB034:
    r_a1 = bl_lh(r_s4 + 0x1Eu); /* 800AB034 lh $a1, 0x1E($s4) */
    /* 800AB038 jal func_800A5870 */
    r_a2 = r_sp + 0x68u;
    bl_jal(FN, 0x800A5870u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800A5870 */
    r_a0 = bl_lw(r_s4 + 0x4u); /* 800AB040 lw $a0, 0x4($s4) */
    r_v1 = bl_lh(r_sp + 0x7Au); /* 800AB044 lh $v1, 0x7A($sp) */
    r_v0 = bl_lw(r_a0 + 0x60u); /* 800AB048 lw $v0, 0x60($a0) */
    ; /* 800AB04C nop  */
    r_v0 = (u32)((s32)r_v1 < (s32)r_v0); /* 800AB050 slt $v0, $v1, $v0 */
    /* 800AB054 beqz $v0, .L800AB06C */
    { int c_ = (r_v0 == 0); ; if (c_) goto L_800AB06C; }
    bl_sw(r_a0 + 0x60u, r_v1); /* 800AB05C sw $v1, 0x60($a0) */
    r_s3 = bl_lw(r_s4 + 0x54u); /* 800AB060 lw $s3, 0x54($s4) */
    /* 800AB064 j .L800AB0AC */
    bl_sw(r_s4 + 0x54u, 0u); goto L_800AB0AC;
L_800AB06C:
    r_v0 = bl_lw(r_s4 + 0x50u); /* 800AB06C lw $v0, 0x50($s4) */
    ; /* 800AB070 nop  */
    /* 800AB074 beqz $v0, .L800AB0AC */
    { int c_ = (r_v0 == 0); ; if (c_) goto L_800AB0AC; }
    r_v0 = bl_lhu(r_s4 + 0x44u); /* 800AB07C lhu $v0, 0x44($s4) */
    r_t7 = bl_lw(r_sp + 0x140u); /* 800AB080 lw $t7, 0x140($sp) */
    r_v1 = bl_lhu(r_s4 + 0x46u); /* 800AB084 lhu $v1, 0x46($s4) */
    r_v0 = r_v0 + r_t7; /* 800AB088 addu $v0, $v0, $t7 */
    bl_sh(r_s4 + 0x44u, r_v0); /* 800AB08C sh $v0, 0x44($s4) */
    r_v0 = bl_lhu(r_s4 + 0x44u); /* 800AB090 lhu $v0, 0x44($s4) */
    ; /* 800AB094 nop  */
    r_v0 = (u32)(r_v0 < r_v1); /* 800AB098 sltu $v0, $v0, $v1 */
    /* 800AB09C bnez $v0, .L800AB0AC */
    { int c_ = (r_v0 != 0); ; if (c_) goto L_800AB0AC; }
    r_s3 = bl_lw(r_s4 + 0x50u); /* 800AB0A4 lw $s3, 0x50($s4) */
    bl_sw(r_s4 + 0x50u, 0u); /* 800AB0A8 sw $zero, 0x50($s4) */
L_800AB0AC:
    r_v0 = bl_lh(r_s4 + 0x58u); /* 800AB0AC lh $v0, 0x58($s4) */
    ; /* 800AB0B0 nop  */
    /* 800AB0B4 beqz $v0, .L800AB0C4 */
    { int c_ = (r_v0 == 0); ; if (c_) goto L_800AB0C4; }
    /* 800AB0BC jal func_800AEF68 */
    r_a0 = r_s4 + 0u;
    bl_jal(FN, 0x800AEF68u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800AEF68 */
L_800AB0C4:
    r_t8 = bl_lw(r_sp + 0x150u); /* 800AB0C4 lw $t8, 0x150($sp) */
    ; /* 800AB0C8 nop  */
    /* 800AB0CC beqz $t8, .L800ADECC */
    { int c_ = (r_t8 == 0); r_s5 = r_s3 + 0u; if (c_) goto L_800ADECC; }
L_800AB0D4:
    r_v0 = bl_lhu(r_s3); /* 800AB0D4 lhu $v0, 0x0($s3) */
    ; /* 800AB0D8 nop  */
    bl_sh(r_sp + 0x9Cu, r_v0); /* 800AB0DC sh $v0, 0x9C($sp) */
    r_fp = bl_lbu(r_sp + 0x9Cu); /* 800AB0E0 lbu $fp, 0x9C($sp) */
    r_s6 = r_v0 >> 8; /* 800AB0E4 srl $s6, $v0, 8 */
    r_v1 = r_fp & 0xFFu; /* 800AB0E8 andi $v1, $fp, 0xFF */
    r_v0 = (u32)(r_v1 < 0x76u); /* 800AB0EC sltiu $v0, $v1, 0x76 */
    /* 800AB0F0 beqz $v0, .L800ADEB4 */
    { int c_ = (r_v0 == 0); r_s3 = r_s3 + 0x2u; if (c_) goto L_800ADEB4; }
    r_v0 = r_v1 << 2; /* 800AB0F8 sll $v0, $v1, 2 */
    r_at = 0x80070000u; /* 800AB0FC lui $at, %hi(jtbl_8007056C) */
    r_at = r_at + r_v0; /* 800AB100 addu $at, $at, $v0 */
    r_v0 = bl_lw(r_at + 0x56Cu); /* 800AB104 lw $v0, %lo(jtbl_8007056C)($at) */
    ; /* 800AB108 nop  */
    /* 800AB10C jr $v0 */
    { u32 t_ = r_v0; ; switch (t_) { case 0x800AB114u: goto L_800AB114; case 0x800AB11Cu: goto L_800AB11C; case 0x800AB180u: goto L_800AB180; case 0x800AB1B0u: goto L_800AB1B0; case 0x800AB1FCu: goto L_800AB1FC; case 0x800AB284u: goto L_800AB284; case 0x800AB44Cu: goto L_800AB44C; case 0x800AB45Cu: goto L_800AB45C; case 0x800AB46Cu: goto L_800AB46C; case 0x800AB484u: goto L_800AB484; case 0x800AB49Cu: goto L_800AB49C; case 0x800AB4B8u: goto L_800AB4B8; case 0x800AB520u: goto L_800AB520; case 0x800AB554u: goto L_800AB554; case 0x800AB570u: goto L_800AB570; case 0x800AB58Cu: goto L_800AB58C; case 0x800AB5B0u: goto L_800AB5B0; case 0x800AB5D4u: goto L_800AB5D4; case 0x800AB678u: goto L_800AB678; case 0x800AB704u: goto L_800AB704; case 0x800AB778u: goto L_800AB778; case 0x800AB898u: goto L_800AB898; case 0x800ABB48u: goto L_800ABB48; case 0x800ABBB8u: goto L_800ABBB8; case 0x800ABBCCu: goto L_800ABBCC; case 0x800ABBF8u: goto L_800ABBF8; case 0x800ABC08u: goto L_800ABC08; case 0x800ABCCCu: goto L_800ABCCC; case 0x800ABEFCu: goto L_800ABEFC; case 0x800ABF2Cu: goto L_800ABF2C; case 0x800ABFECu: goto L_800ABFEC; case 0x800ABFF4u: goto L_800ABFF4; case 0x800AC030u: goto L_800AC030; case 0x800AC058u: goto L_800AC058; case 0x800AC088u: goto L_800AC088; case 0x800AC154u: goto L_800AC154; case 0x800AC184u: goto L_800AC184; case 0x800AC194u: goto L_800AC194; case 0x800AC4A8u: goto L_800AC4A8; case 0x800AC520u: goto L_800AC520; case 0x800AC560u: goto L_800AC560; case 0x800AC660u: goto L_800AC660; case 0x800AC684u: goto L_800AC684; case 0x800AC6A8u: goto L_800AC6A8; case 0x800AC758u: goto L_800AC758; case 0x800AC794u: goto L_800AC794; case 0x800AC7ACu: goto L_800AC7AC; case 0x800AC7B8u: goto L_800AC7B8; case 0x800AC80Cu: goto L_800AC80C; case 0x800AC818u: goto L_800AC818; case 0x800AC838u: goto L_800AC838; case 0x800AC85Cu: goto L_800AC85C; case 0x800AC880u: goto L_800AC880; case 0x800AC8C0u: goto L_800AC8C0; case 0x800AC8F0u: goto L_800AC8F0; case 0x800AC930u: goto L_800AC930; case 0x800AC984u: goto L_800AC984; case 0x800AC9D4u: goto L_800AC9D4; case 0x800ACA04u: goto L_800ACA04; case 0x800ACA3Cu: goto L_800ACA3C; case 0x800ACAA0u: goto L_800ACAA0; case 0x800ACB24u: goto L_800ACB24; case 0x800ACB2Cu: goto L_800ACB2C; case 0x800ACB34u: goto L_800ACB34; case 0x800ACBCCu: goto L_800ACBCC; case 0x800ACBDCu: goto L_800ACBDC; case 0x800ACC24u: goto L_800ACC24; case 0x800ACC98u: goto L_800ACC98; case 0x800ACD64u: goto L_800ACD64; case 0x800ACD8Cu: goto L_800ACD8C; case 0x800ACDCCu: goto L_800ACDCC; case 0x800ACDF4u: goto L_800ACDF4; case 0x800ACE34u: goto L_800ACE34; case 0x800ACE3Cu: goto L_800ACE3C; case 0x800ACE70u: goto L_800ACE70; case 0x800AD020u: goto L_800AD020; case 0x800AD048u: goto L_800AD048; case 0x800AD088u: goto L_800AD088; case 0x800AD0B0u: goto L_800AD0B0; case 0x800AD0F0u: goto L_800AD0F0; case 0x800AD230u: goto L_800AD230; case 0x800AD25Cu: goto L_800AD25C; case 0x800AD2BCu: goto L_800AD2BC; case 0x800AD2FCu: goto L_800AD2FC; case 0x800AD338u: goto L_800AD338; case 0x800AD370u: goto L_800AD370; case 0x800AD3A8u: goto L_800AD3A8; case 0x800AD3C0u: goto L_800AD3C0; case 0x800AD3F0u: goto L_800AD3F0; case 0x800AD424u: goto L_800AD424; case 0x800AD468u: goto L_800AD468; case 0x800AD4ACu: goto L_800AD4AC; case 0x800AD550u: goto L_800AD550; case 0x800AD5A0u: goto L_800AD5A0; case 0x800AD5D4u: goto L_800AD5D4; case 0x800AD5E8u: goto L_800AD5E8; case 0x800AD5FCu: goto L_800AD5FC; case 0x800AD660u: goto L_800AD660; case 0x800AD68Cu: goto L_800AD68C; case 0x800AD6DCu: goto L_800AD6DC; case 0x800AD714u: goto L_800AD714; case 0x800AD724u: goto L_800AD724; case 0x800AD990u: goto L_800AD990; case 0x800ADBF4u: goto L_800ADBF4; case 0x800ADC9Cu: goto L_800ADC9C; case 0x800ADCF0u: goto L_800ADCF0; case 0x800ADD04u: goto L_800ADD04; case 0x800ADD34u: goto L_800ADD34; case 0x800ADD4Cu: goto L_800ADD4C; case 0x800ADD58u: goto L_800ADD58; case 0x800ADDA8u: goto L_800ADDA8; case 0x800ADDCCu: goto L_800ADDCC; case 0x800ADDF8u: goto L_800ADDF8; case 0x800ADE1Cu: goto L_800ADE1C; case 0x800ADE38u: goto L_800ADE38; case 0x800ADE50u: goto L_800ADE50; case 0x800ADE60u: goto L_800ADE60; default: bl_fail(FN, "jump-table target", t_); } }
L_800AB114:
    /* 800AB114 j .L800ADEB8 */
    r_s3 = r_s5 + 0u; goto L_800ADEB8;
L_800AB11C:
    r_t7 = bl_lw(r_sp + 0x138u); /* 800AB11C lw $t7, 0x138($sp) */
    r_v0 = 0u + 0xFFFFFFFFu; /* 800AB120 addiu $v0, $zero, -0x1 */
    /* 800AB124 beq $t7, $v0, .L800AB178 */
    { int c_ = (r_t7 == r_v0); ; if (c_) goto L_800AB178; }
    r_v1 = bl_lhu(r_s3); /* 800AB12C lhu $v1, 0x0($s3) */
    r_v0 = bl_lhu(r_s4 + 0x40u); /* 800AB130 lhu $v0, 0x40($s4) */
    r_t8 = bl_lw(r_sp + 0x140u); /* 800AB134 lw $t8, 0x140($sp) */
    ; /* 800AB138 nop  */
    r_v0 = r_v0 + r_t8; /* 800AB13C addu $v0, $v0, $t8 */
    bl_sh(r_sp + 0x9Cu, r_v1); /* 800AB140 sh $v1, 0x9C($sp) */
    bl_sh(r_s4 + 0x40u, r_v0); /* 800AB144 sh $v0, 0x40($s4) */
    r_v0 = r_v0 << 16; /* 800AB148 sll $v0, $v0, 16 */
    r_v0 = BL_SRA(r_v0, 16); /* 800AB14C sra $v0, $v0, 16 */
    r_v1 = r_v1 << 16; /* 800AB150 sll $v1, $v1, 16 */
    r_v1 = BL_SRA(r_v1, 16); /* 800AB154 sra $v1, $v1, 16 */
    r_v0 = (u32)((s32)r_v0 < (s32)r_v1); /* 800AB158 slt $v0, $v0, $v1 */
    /* 800AB15C beqz $v0, .L800AB16C */
    { int c_ = (r_v0 == 0); r_s3 = r_s3 + 0x2u; if (c_) goto L_800AB16C; }
    /* 800AB164 j .L800ADEB8 */
    r_s3 = r_s5 + 0u; goto L_800ADEB8;
L_800AB16C:
    bl_sh(r_s4 + 0x40u, 0u); /* 800AB16C sh $zero, 0x40($s4) */
    /* 800AB170 j .L800ADEBC */
    bl_sw(r_sp + 0x140u, 0u); goto L_800ADEBC;
L_800AB178:
    /* 800AB178 j .L800ADEB8 */
    r_s3 = r_s5 + 0u; goto L_800ADEB8;
L_800AB180:
    r_v0 = bl_lbu(r_s4 + 0x35u); /* 800AB180 lbu $v0, 0x35($s4) */
    ; /* 800AB184 nop  */
    /* 800AB188 beqz $v0, .L800AB1A8 */
    { int c_ = (r_v0 == 0); r_t7 = 0u | 0x1u; if (c_) goto L_800AB1A8; }
    /* 800AB190 jal func_800BF6F8 */
    ;
    bl_jal(FN, 0x800BF6F8u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800BF6F8 */
    /* 800AB198 beqz $v0, .L800AB1D8 */
    { int c_ = (r_v0 == 0); ; if (c_) goto L_800AB1D8; }
    /* 800AB1A0 j .L800ADEB8 */
    r_s3 = r_s5 + 0u; goto L_800ADEB8;
L_800AB1A8:
    /* 800AB1A8 j .L800ADEBC */
    bl_sw(r_sp + 0x158u, r_t7); goto L_800ADEBC;
L_800AB1B0:
    r_v0 = bl_lbu(r_s4 + 0x35u); /* 800AB1B0 lbu $v0, 0x35($s4) */
    ; /* 800AB1B4 nop  */
    /* 800AB1B8 beqz $v0, .L800ADEBC */
    { int c_ = (r_v0 == 0); ; if (c_) goto L_800ADEBC; }
    /* 800AB1C0 jal func_800BF6F8 */
    ;
    bl_jal(FN, 0x800BF6F8u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800BF6F8 */
    /* 800AB1C8 beqz $v0, .L800AB1D8 */
    { int c_ = (r_v0 == 0); ; if (c_) goto L_800AB1D8; }
    /* 800AB1D0 j .L800ADEB8 */
    r_s3 = r_s5 + 0u; goto L_800ADEB8;
L_800AB1D8:
    r_at = 0x80060000u; /* 800AB1D8 lui $at, %hi(D_80059464) */
    bl_sw(r_at + 0xFFFF9464u, 0u); /* 800AB1DC sw $zero, %lo(D_80059464)($at) */
    r_at = 0x80060000u; /* 800AB1E0 lui $at, %hi(D_800591AC) */
    bl_sb(r_at + 0xFFFF91ACu, 0u); /* 800AB1E4 sb $zero, %lo(D_800591AC)($at) */
    r_a0 = bl_lbu(r_s4 + 0x20u); /* 800AB1E8 lbu $a0, 0x20($s4) */
    /* 800AB1EC jal func_80080C6C */
    bl_sb(r_s4 + 0x35u, 0u);
    bl_jal(FN, 0x80080C6Cu, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_80080C6C */
    /* 800AB1F4 j .L800ADEBC */
    ; goto L_800ADEBC;
L_800AB1FC:
    r_v0 = bl_lhu(r_s3); /* 800AB1FC lhu $v0, 0x0($s3) */
    r_v1 = bl_lw(r_s4 + 0xCu); /* 800AB200 lw $v1, 0xC($s4) */
    r_s3 = r_s3 + 0x2u; /* 800AB204 addiu $s3, $s3, 0x2 */
    /* 800AB208 bnez $v1, .L800ADEBC */
    { int c_ = (r_v1 != 0); bl_sh(r_sp + 0x9Cu, r_v0); if (c_) goto L_800ADEBC; }
    r_a0 = r_sp + 0x80u; /* 800AB210 addiu $a0, $sp, 0x80 */
    /* 800AB214 jal ArchiveGetArchiveOffsetIndices */
    r_a1 = r_sp + 0x84u;
    bl_jal(FN, 0x800284B4u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* ArchiveGetArchiveOffsetIndices */
    r_a0 = 0u | 0x28u; /* 800AB21C ori $a0, $zero, 0x28 */
    /* 800AB220 jal ArchiveSetIndex */
    r_a1 = 0u | 0x2u;
    bl_jal(FN, 0x80028470u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* ArchiveSetIndex */
    r_v0 = bl_lh(r_sp + 0x9Cu); /* 800AB228 lh $v0, 0x9C($sp) */
    r_at = 0x800C0000u; /* 800AB22C lui $at, %hi(D_800C3530) */
    r_at = r_at + r_v0; /* 800AB230 addu $at, $at, $v0 */
    r_v0 = bl_lbu(r_at + 0x3530u); /* 800AB234 lbu $v0, %lo(D_800C3530)($at) */
    r_s0 = r_s6 & 0xFFu; /* 800AB238 andi $s0, $s6, 0xFF */
    r_s0 = r_s0 + r_v0; /* 800AB23C addu $s0, $s0, $v0 */
    /* 800AB240 jal ArchiveDecodeAlignedSize */
    r_a0 = r_s0 + 0u;
    bl_jal(FN, 0x800288ECu, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* ArchiveDecodeAlignedSize */
    r_a0 = r_v0 + 0u; /* 800AB248 addu $a0, $v0, $zero */
    /* 800AB24C jal HeapAlloc */
    r_a1 = 0u + 0u;
    bl_jal(FN, 0x80031BDCu, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* HeapAlloc */
    r_a0 = r_s0 + 0u; /* 800AB254 addu $a0, $s0, $zero */
    r_a1 = r_v0 + 0u; /* 800AB258 addu $a1, $v0, $zero */
    r_a2 = 0u + 0u; /* 800AB25C addu $a2, $zero, $zero */
    r_a3 = 0u + 0u; /* 800AB260 addu $a3, $zero, $zero */
    /* 800AB264 jal ArchiveReadFileToBuffer */
    bl_sw(r_s4 + 0xCu, r_a1);
    bl_jal(FN, 0x800295D8u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* ArchiveReadFileToBuffer */
    r_a0 = bl_lw(r_sp + 0x80u); /* 800AB26C lw $a0, 0x80($sp) */
    r_a1 = bl_lw(r_sp + 0x84u); /* 800AB270 lw $a1, 0x84($sp) */
    /* 800AB274 jal ArchiveSetIndex */
    ;
    bl_jal(FN, 0x80028470u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* ArchiveSetIndex */
    /* 800AB27C j .L800ADEBC */
    ; goto L_800ADEBC;
L_800AB284:
    r_v0 = bl_lbu(r_s4 + 0x23u); /* 800AB284 lbu $v0, 0x23($s4) */
    ; /* 800AB288 nop  */
    /* 800AB28C bnez $v0, .L800AB42C */
    { int c_ = (r_v0 != 0); ; if (c_) goto L_800AB42C; }
    r_v0 = bl_lw(r_s4 + 0xCu); /* 800AB294 lw $v0, 0xC($s4) */
    ; /* 800AB298 nop  */
    /* 800AB29C beqz $v0, .L800ADEBC */
    { int c_ = (r_v0 == 0); ; if (c_) goto L_800ADEBC; }
    r_v0 = bl_lw(r_s4 + 0x18u); /* 800AB2A4 lw $v0, 0x18($s4) */
    ; /* 800AB2A8 nop  */
    /* 800AB2AC bnez $v0, .L800ADEBC */
    { int c_ = (r_v0 != 0); ; if (c_) goto L_800ADEBC; }
    /* 800AB2B4 jal ArchiveDataSync */
    ;
    bl_jal(FN, 0x800286CCu, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* ArchiveDataSync */
    /* 800AB2BC bnez $v0, .L800AB424 */
    { int c_ = (r_v0 != 0); ; if (c_) goto L_800AB424; }
    r_a0 = bl_lw(r_s4 + 0xCu); /* 800AB2C4 lw $a0, 0xC($s4) */
    /* 800AB2C8 jal ResolveArchiveEntryPointers */
    ;
    bl_jal(FN, 0x8003342Cu, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* ResolveArchiveEntryPointers */
    r_v0 = bl_lw(r_s4 + 0xCu); /* 800AB2D0 lw $v0, 0xC($s4) */
    ; /* 800AB2D4 nop  */
    r_s0 = bl_lw(r_v0 + 0x8u); /* 800AB2D8 lw $s0, 0x8($v0) */
    /* 800AB2DC jal ResolveArchiveEntryPointers */
    r_a0 = r_s0 + 0u;
    bl_jal(FN, 0x8003342Cu, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* ResolveArchiveEntryPointers */
    bl_sw(r_s4 + 0xB4u, r_s0); /* 800AB2E4 sw $s0, 0xB4($s4) */
    r_v1 = bl_lw(r_s0 + 0x18u); /* 800AB2E8 lw $v1, 0x18($s0) */
    r_v0 = bl_lw(r_s0 + 0x14u); /* 800AB2EC lw $v0, 0x14($s0) */
    ; /* 800AB2F0 nop  */
    /* 800AB2F4 beq $v1, $v0, .L800AB320 */
    { int c_ = (r_v1 == r_v0); r_a1 = 0u | 0x1u; if (c_) goto L_800AB320; }
    r_a2 = 0u | 0x380u; /* 800AB2FC ori $a2, $zero, 0x380 */
    r_t8 = 0u | 0x1u; /* 800AB300 ori $t8, $zero, 0x1 */
    r_v0 = 0u | 0x1D0u; /* 800AB304 ori $v0, $zero, 0x1D0 */
    bl_sw(r_sp + 0x10u, r_t8); /* 800AB308 sw $t8, 0x10($sp) */
    bl_sw(r_sp + 0x14u, 0u); /* 800AB30C sw $zero, 0x14($sp) */
    bl_sw(r_sp + 0x18u, r_v0); /* 800AB310 sw $v0, 0x18($sp) */
    r_a0 = bl_lw(r_s0 + 0x14u); /* 800AB314 lw $a0, 0x14($s0) */
    /* 800AB318 jal func_8002DDE4 */
    r_a3 = 0u | 0x100u;
    bl_jal(FN, 0x8002DDE4u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_8002DDE4 */
L_800AB320:
    r_v0 = bl_lw(r_s4 + 0xB4u); /* 800AB320 lw $v0, 0xB4($s4) */
    ; /* 800AB324 nop  */
    r_v1 = bl_lw(r_v0 + 0xCu); /* 800AB328 lw $v1, 0xC($v0) */
    r_a0 = bl_lw(r_v0 + 0x8u); /* 800AB32C lw $a0, 0x8($v0) */
    ; /* 800AB330 nop  */
    /* 800AB334 beq $v1, $a0, .L800AB36C */
    { int c_ = (r_v1 == r_a0); ; if (c_) goto L_800AB36C; }
    /* 800AB33C jal func_8003864C */
    r_a1 = 0u + 0u;
    bl_jal(FN, 0x8003864Cu, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_8003864C */
    /* 800AB344 bnez $v0, .L800AB36C */
    { int c_ = (r_v0 != 0); ; if (c_) goto L_800AB36C; }
    r_v0 = bl_lw(r_s4 + 0xB4u); /* 800AB34C lw $v0, 0xB4($s4) */
    ; /* 800AB350 nop  */
    r_a0 = bl_lw(r_v0 + 0x8u); /* 800AB354 lw $a0, 0x8($v0) */
    /* 800AB358 jal SoundAddSedsEntry */
    ;
    bl_jal(FN, 0x80038428u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* SoundAddSedsEntry */
    r_t7 = 0u | 0x1u; /* 800AB360 ori $t7, $zero, 0x1 */
    /* 800AB364 j .L800AB370 */
    bl_sb(r_s4 + 0x63u, r_t7); goto L_800AB370;
L_800AB36C:
    bl_sb(r_s4 + 0x63u, 0u); /* 800AB36C sb $zero, 0x63($s4) */
L_800AB370:
    r_v0 = bl_lw(r_s4 + 0xCu); /* 800AB370 lw $v0, 0xC($s4) */
    ; /* 800AB374 nop  */
    r_s0 = bl_lw(r_v0 + 0x4u); /* 800AB378 lw $s0, 0x4($v0) */
    /* 800AB37C jal ResolveArchiveEntryPointers */
    r_a0 = r_s0 + 0u;
    bl_jal(FN, 0x8003342Cu, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* ResolveArchiveEntryPointers */
    r_s0 = bl_lw(r_s0 + 0x4u); /* 800AB384 lw $s0, 0x4($s0) */
    /* 800AB388 jal ResolveArchiveEntryPointers */
    r_a0 = r_s0 + 0u;
    bl_jal(FN, 0x8003342Cu, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* ResolveArchiveEntryPointers */
    r_a0 = 0u + 0u; /* 800AB390 addu $a0, $zero, $zero */
    /* 800AB394 jal DrawSync */
    bl_sw(r_s4 + 0x18u, r_s0);
    bl_jal(FN, 0x800445D0u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* DrawSync */
    r_v0 = bl_lw(r_s4 + 0xB4u); /* 800AB39C lw $v0, 0xB4($s4) */
    r_a0 = bl_lw(r_s4 + 0xCu); /* 800AB3A0 lw $a0, 0xC($s4) */
    r_a1 = bl_lw(r_v0 + 0xCu); /* 800AB3A4 lw $a1, 0xC($v0) */
    /* 800AB3A8 jal HeapInsertAlloc */
    r_a1 = r_a1 - r_a0;
    bl_jal(FN, 0x80031F70u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* HeapInsertAlloc */
    r_v0 = bl_lw(r_s4 + 0xB4u); /* 800AB3B0 lw $v0, 0xB4($s4) */
    ; /* 800AB3B4 nop  */
    r_v1 = bl_lw(r_v0 + 0x8u); /* 800AB3B8 lw $v1, 0x8($v0) */
    r_v0 = bl_lw(r_v0 + 0x4u); /* 800AB3BC lw $v0, 0x4($v0) */
    ; /* 800AB3C0 nop  */
    /* 800AB3C4 beq $v1, $v0, .L800AB41C */
    { int c_ = (r_v1 == r_v0); r_v0 = 0u | 0x380u; if (c_) goto L_800AB41C; }
    bl_sh(r_sp + 0x88u, r_v0); /* 800AB3CC sh $v0, 0x88($sp) */
    r_v0 = 0u | 0x100u; /* 800AB3D0 ori $v0, $zero, 0x100 */
    bl_sh(r_sp + 0x8Au, r_v0); /* 800AB3D4 sh $v0, 0x8A($sp) */
    r_v0 = 0u | 0x1D0u; /* 800AB3D8 ori $v0, $zero, 0x1D0 */
    bl_sh(r_sp + 0x90u, 0u); /* 800AB3DC sh $zero, 0x90($sp) */
    bl_sh(r_sp + 0x92u, r_v0); /* 800AB3E0 sh $v0, 0x92($sp) */
    r_v0 = bl_lw(r_s4 + 0xB4u); /* 800AB3E4 lw $v0, 0xB4($s4) */
    r_a0 = 0x80060000u; /* 800AB3E8 lui $a0, %hi(D_8005A474) */
    r_a0 = r_a0 + 0xFFFFA474u; /* 800AB3EC addiu $a0, $a0, %lo(D_8005A474) */
    bl_sw(r_sp + 0x10u, 0u); /* 800AB3F0 sw $zero, 0x10($sp) */
    r_v1 = bl_lhu(r_sp + 0x88u); /* 800AB3F4 lhu $v1, 0x88($sp) */
    r_a2 = bl_lhu(r_sp + 0x8Au); /* 800AB3F8 lhu $a2, 0x8A($sp) */
    r_t0 = bl_lhu(r_sp + 0x90u); /* 800AB3FC lhu $t0, 0x90($sp) */
    r_a3 = bl_lhu(r_sp + 0x92u); /* 800AB400 lhu $a3, 0x92($sp) */
    r_a2 = r_a2 << 16; /* 800AB404 sll $a2, $a2, 16 */
    r_a2 = r_v1 | r_a2; /* 800AB408 or $a2, $v1, $a2 */
    r_a3 = r_a3 << 16; /* 800AB40C sll $a3, $a3, 16 */
    r_a1 = bl_lw(r_v0 + 0x4u); /* 800AB410 lw $a1, 0x4($v0) */
    /* 800AB414 jal func_80022224 */
    r_a3 = r_t0 | r_a3;
    bl_jal(FN, 0x80022224u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_80022224 */
L_800AB41C:
    r_t8 = 0u | 0x1u; /* 800AB41C ori $t8, $zero, 0x1 */
    bl_sb(r_s4 + 0x23u, r_t8); /* 800AB420 sb $t8, 0x23($s4) */
L_800AB424:
    /* 800AB424 j .L800ADEB8 */
    r_s3 = r_s5 + 0u; goto L_800ADEB8;
L_800AB42C:
    /* 800AB42C jal ArchiveDataSync */
    ;
    bl_jal(FN, 0x800286CCu, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* ArchiveDataSync */
    /* 800AB434 beqz $v0, .L800AB444 */
    { int c_ = (r_v0 == 0); ; if (c_) goto L_800AB444; }
    /* 800AB43C j .L800ADEB8 */
    r_s3 = r_s5 + 0u; goto L_800ADEB8;
L_800AB444:
    /* 800AB444 j .L800ADEBC */
    bl_sb(r_s4 + 0x23u, 0u); goto L_800ADEBC;
L_800AB44C:
    /* 800AB44C jal func_800B0060 */
    r_a0 = r_s4 + 0u;
    bl_jal(FN, 0x800B0060u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800B0060 */
    /* 800AB454 j .L800ADEBC */
    ; goto L_800ADEBC;
L_800AB45C:
    /* 800AB45C jal func_8002A498 */
    r_a0 = 0u + 0u;
    bl_jal(FN, 0x8002A498u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_8002A498 */
    /* 800AB464 j .L800ADEBC */
    ; goto L_800ADEBC;
L_800AB46C:
    r_a1 = bl_lw(r_s4 + 0x4u); /* 800AB46C lw $a1, 0x4($s4) */
    r_a0 = bl_lw(r_sp + 0x130u); /* 800AB470 lw $a0, 0x130($sp) */
    /* 800AB474 jal func_800A2ACC */
    ;
    bl_jal(FN, 0x800A2ACCu, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800A2ACC */
    /* 800AB47C j .L800ABBF8 */
    ; goto L_800ABBF8;
L_800AB484:
    r_a0 = bl_lw(r_sp + 0x130u); /* 800AB484 lw $a0, 0x130($sp) */
    r_a1 = bl_lw(r_s4 + 0x4u); /* 800AB488 lw $a1, 0x4($s4) */
    /* 800AB48C jal func_800A2BB8 */
    r_a2 = r_s6 & 0xFFu;
    bl_jal(FN, 0x800A2BB8u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800A2BB8 */
    /* 800AB494 j .L800ADEBC */
    ; goto L_800ADEBC;
L_800AB49C:
    r_a2 = r_s6 & 0xFFu; /* 800AB49C andi $a2, $s6, 0xFF */
    r_a0 = bl_lw(r_sp + 0x130u); /* 800AB4A0 lw $a0, 0x130($sp) */
    r_a1 = bl_lw(r_s4 + 0x4u); /* 800AB4A4 lw $a1, 0x4($s4) */
    /* 800AB4A8 jal func_800A216C */
    r_a3 = 0u | 0x7u;
    bl_jal(FN, 0x800A216Cu, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800A216C */
    /* 800AB4B0 j .L800ADEBC */
    ; goto L_800ADEBC;
L_800AB4B8:
    r_s0 = bl_lw(r_s4 + 0x4u); /* 800AB4B8 lw $s0, 0x4($s4) */
    r_a0 = bl_lw(r_sp + 0x130u); /* 800AB4BC lw $a0, 0x130($sp) */
    /* 800AB4C0 jal func_800A2ACC */
    r_a1 = r_s0 + 0u;
    bl_jal(FN, 0x800A2ACCu, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800A2ACC */
    r_v0 = bl_lhu(r_s0 + 0xAu); /* 800AB4C8 lhu $v0, 0xA($s0) */
    ; /* 800AB4CC nop  */
    r_a0 = r_v0 + 0xFFFFFFFFu; /* 800AB4D0 addiu $a0, $v0, -0x1 */
    /* 800AB4D4 blez $a0, .L800ADEBC */
    { int c_ = ((s32)r_a0 <= 0); r_v1 = 0u + 0u; if (c_) goto L_800ADEBC; }
    r_a1 = r_s0 + 0x5u; /* 800AB4DC addiu $a1, $s0, 0x5 */
L_800AB4E0:
    r_a1 = r_a1 + 0x7Cu; /* 800AB4E0 addiu $a1, $a1, 0x7C */
    r_v1 = r_v1 + 0x1u; /* 800AB4E4 addiu $v1, $v1, 0x1 */
    r_t7 = 0u | 0x1u; /* 800AB4E8 ori $t7, $zero, 0x1 */
    r_t8 = 0u | 0x1u; /* 800AB4EC ori $t8, $zero, 0x1 */
    r_v0 = (u32)((s32)r_v1 < (s32)r_a0); /* 800AB4F0 slt $v0, $v1, $a0 */
    bl_sh(r_a1 + 0x4Fu, 0u); /* 800AB4F4 sh $zero, 0x4F($a1) */
    bl_sh(r_a1 + 0x51u, 0u); /* 800AB4F8 sh $zero, 0x51($a1) */
    bl_sh(r_a1 + 0x53u, 0u); /* 800AB4FC sh $zero, 0x53($a1) */
    bl_sw(r_a1 + 0x57u, 0u); /* 800AB500 sw $zero, 0x57($a1) */
    bl_sw(r_a1 + 0x5Bu, 0u); /* 800AB504 sw $zero, 0x5B($a1) */
    bl_sw(r_a1 + 0x5Fu, 0u); /* 800AB508 sw $zero, 0x5F($a1) */
    bl_sb(r_a1 + 0xFFFFFFFFu, r_t7); /* 800AB50C sb $t7, -0x1($a1) */
    /* 800AB510 bnez $v0, .L800AB4E0 */
    { int c_ = (r_v0 != 0); bl_sb(r_a1, r_t8); if (c_) goto L_800AB4E0; }
    /* 800AB518 j .L800ADEBC */
    ; goto L_800ADEBC;
L_800AB520:
    bl_sh(r_s4 + 0x70u, 0u); /* 800AB520 sh $zero, 0x70($s4) */
    bl_sh(r_s4 + 0x72u, 0u); /* 800AB524 sh $zero, 0x72($s4) */
    bl_sh(r_s4 + 0x74u, 0u); /* 800AB528 sh $zero, 0x74($s4) */
    bl_sh(r_s4 + 0x76u, 0u); /* 800AB52C sh $zero, 0x76($s4) */
    bl_sh(r_s4 + 0x78u, 0u); /* 800AB530 sh $zero, 0x78($s4) */
    bl_sh(r_s4 + 0x7Au, 0u); /* 800AB534 sh $zero, 0x7A($s4) */
    bl_sh(r_s4 + 0x7Cu, 0u); /* 800AB538 sh $zero, 0x7C($s4) */
    bl_sh(r_s4 + 0x7Eu, 0u); /* 800AB53C sh $zero, 0x7E($s4) */
    bl_sh(r_s4 + 0x80u, 0u); /* 800AB540 sh $zero, 0x80($s4) */
    bl_sh(r_s4 + 0x82u, 0u); /* 800AB544 sh $zero, 0x82($s4) */
    bl_sh(r_s4 + 0x84u, 0u); /* 800AB548 sh $zero, 0x84($s4) */
    /* 800AB54C j .L800ADEBC */
    bl_sh(r_s4 + 0x86u, 0u); goto L_800ADEBC;
L_800AB554:
    r_a2 = r_s6 & 0xFFu; /* 800AB554 andi $a2, $s6, 0xFF */
    r_a0 = bl_lw(r_sp + 0x130u); /* 800AB558 lw $a0, 0x130($sp) */
    r_a1 = bl_lw(r_s4 + 0x4u); /* 800AB55C lw $a1, 0x4($s4) */
    /* 800AB560 jal func_800A216C */
    r_a3 = 0u | 0x1u;
    bl_jal(FN, 0x800A216Cu, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800A216C */
    /* 800AB568 j .L800ADEBC */
    ; goto L_800ADEBC;
L_800AB570:
    r_a2 = r_s6 & 0xFFu; /* 800AB570 andi $a2, $s6, 0xFF */
    r_a0 = bl_lw(r_sp + 0x130u); /* 800AB574 lw $a0, 0x130($sp) */
    r_a1 = bl_lw(r_s4 + 0x4u); /* 800AB578 lw $a1, 0x4($s4) */
    /* 800AB57C jal func_800A216C */
    r_a3 = 0u | 0x2u;
    bl_jal(FN, 0x800A216Cu, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800A216C */
    /* 800AB584 j .L800ADEBC */
    ; goto L_800ADEBC;
L_800AB58C:
    /* 800AB58C jal func_800BCAD0 */
    ;
    bl_jal(FN, 0x800BCAD0u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800BCAD0 */
    r_a0 = bl_lw(r_sp + 0x130u); /* 800AB594 lw $a0, 0x130($sp) */
    /* 800AB598 jal func_800B00F4 */
    ;
    bl_jal(FN, 0x800B00F4u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800B00F4 */
    r_at = 0x800C0000u; /* 800AB5A0 lui $at, %hi(D_800C3DF8) */
    bl_sb(r_at + 0x3DF8u, 0u); /* 800AB5A4 sb $zero, %lo(D_800C3DF8)($at) */
    /* 800AB5A8 j .L800ADEBC */
    ; goto L_800ADEBC;
L_800AB5B0:
    r_a0 = r_s4 + 0u; /* 800AB5B0 addu $a0, $s4, $zero */
    r_a1 = r_s6 & 0xFFu; /* 800AB5B4 andi $a1, $s6, 0xFF */
    /* 800AB5B8 jal func_800AF518 */
    r_a2 = r_sp + 0x98u;
    bl_jal(FN, 0x800AF518u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800AF518 */
    r_a0 = bl_lw(r_s4 + 0x4u); /* 800AB5C0 lw $a0, 0x4($s4) */
    /* 800AB5C4 jal func_800A1B50 */
    r_a1 = r_v0 + 0u;
    bl_jal(FN, 0x800A1B50u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800A1B50 */
    /* 800AB5CC j .L800ADEBC */
    ; goto L_800ADEBC;
L_800AB5D4:
    r_a0 = r_s4 + 0u; /* 800AB5D4 addu $a0, $s4, $zero */
    r_a1 = r_s6 & 0xFFu; /* 800AB5D8 andi $a1, $s6, 0xFF */
    /* 800AB5DC jal func_800AF518 */
    r_a2 = r_sp + 0x98u;
    bl_jal(FN, 0x800AF518u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800AF518 */
    r_s1 = r_v0 + 0u; /* 800AB5E4 addu $s1, $v0, $zero */
    r_s0 = bl_lhu(r_s3); /* 800AB5E8 lhu $s0, 0x0($s3) */
    r_v0 = bl_lw(r_sp + 0x98u); /* 800AB5EC lw $v0, 0x98($sp) */
    r_s3 = r_s3 + 0x2u; /* 800AB5F0 addiu $s3, $s3, 0x2 */
    /* 800AB5F4 bnez $v0, .L800ADEBC */
    { int c_ = (r_v0 != 0); bl_sh(r_sp + 0x9Cu, r_s0); if (c_) goto L_800ADEBC; }
    r_a2 = r_s1 + 0u; /* 800AB5FC addu $a2, $s1, $zero */
    r_a0 = bl_lw(r_sp + 0x130u); /* 800AB600 lw $a0, 0x130($sp) */
    r_v0 = bl_lbu(r_sp + 0x9Cu); /* 800AB604 lbu $v0, 0x9C($sp) */
    r_s0 = r_s0 >> 8; /* 800AB608 srl $s0, $s0, 8 */
    bl_sw(r_sp + 0x10u, r_v0); /* 800AB60C sw $v0, 0x10($sp) */
    r_a1 = bl_lw(r_s4 + 0x4u); /* 800AB610 lw $a1, 0x4($s4) */
    /* 800AB614 jal func_800A2434 */
    r_a3 = r_s0 + 0u;
    bl_jal(FN, 0x800A2434u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800A2434 */
    r_v0 = bl_lw(r_s4 + 0x4u); /* 800AB61C lw $v0, 0x4($s4) */
    r_v1 = bl_lh(r_s4 + 0x1Cu); /* 800AB620 lh $v1, 0x1C($s4) */
    r_v0 = bl_lh(r_v0 + 0x50u); /* 800AB624 lh $v0, 0x50($v0) */
    ; /* 800AB628 nop  */
    BL_MULT(r_v1, r_v0); /* 800AB62C mult $v1, $v0 */
    r_v0 = lo; /* 800AB630 mflo $v0 */
    r_v1 = bl_lh(r_s1 + 0x10u); /* 800AB634 lh $v1, 0x10($s1) */
    r_v0 = BL_SRA(r_v0, 12); /* 800AB638 sra $v0, $v0, 12 */
    BL_MULT(r_v1, r_v0); /* 800AB63C mult $v1, $v0 */
    r_t7 = 0u + 0xFFFFFFFFu; /* 800AB640 addiu $t7, $zero, -0x1 */
    r_a0 = r_s4 + 0u; /* 800AB644 addu $a0, $s4, $zero */
    r_a1 = r_s1 + 0u; /* 800AB648 addu $a1, $s1, $zero */
    r_a2 = r_s0 + 0u; /* 800AB64C addu $a2, $s0, $zero */
    bl_sw(r_sp + 0x138u, r_t7); /* 800AB650 sw $t7, 0x138($sp) */
    r_v0 = lo; /* 800AB654 mflo $v0 */
    r_v0 = BL_SRA(r_v0, 12); /* 800AB658 sra $v0, $v0, 12 */
    /* 800AB65C bgez $v0, .L800AB668 */
    { int c_ = ((s32)r_v0 >= 0); ; if (c_) goto L_800AB668; }
    r_v0 = 0u - r_v0; /* 800AB664 negu $v0, $v0 */
L_800AB668:
    /* 800AB668 jal func_800AE1BC */
    bl_sh(r_s4 + 0x8Eu, r_v0);
    bl_jal(FN, 0x800AE1BCu, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800AE1BC */
    /* 800AB670 j .L800ADEBC */
    ; goto L_800ADEBC;
L_800AB678:
    r_a0 = r_s4 + 0u; /* 800AB678 addu $a0, $s4, $zero */
    r_a1 = r_s6 & 0xFFu; /* 800AB67C andi $a1, $s6, 0xFF */
    /* 800AB680 jal func_800AF518 */
    r_a2 = r_sp + 0x98u;
    bl_jal(FN, 0x800AF518u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800AF518 */
    r_s1 = r_v0 + 0u; /* 800AB688 addu $s1, $v0, $zero */
    r_a3 = bl_lhu(r_s3); /* 800AB68C lhu $a3, 0x0($s3) */
    r_v0 = bl_lw(r_sp + 0x98u); /* 800AB690 lw $v0, 0x98($sp) */
    r_s3 = r_s3 + 0x2u; /* 800AB694 addiu $s3, $s3, 0x2 */
    /* 800AB698 bnez $v0, .L800ADEBC */
    { int c_ = (r_v0 != 0); bl_sh(r_sp + 0x9Cu, r_a3); if (c_) goto L_800ADEBC; }
    r_a0 = bl_lw(r_sp + 0x130u); /* 800AB6A0 lw $a0, 0x130($sp) */
    r_v0 = bl_lbu(r_sp + 0x9Cu); /* 800AB6A4 lbu $v0, 0x9C($sp) */
    r_a2 = r_s1 + 0u; /* 800AB6A8 addu $a2, $s1, $zero */
    bl_sw(r_sp + 0x10u, r_v0); /* 800AB6AC sw $v0, 0x10($sp) */
    r_a1 = bl_lw(r_s4 + 0x4u); /* 800AB6B0 lw $a1, 0x4($s4) */
    /* 800AB6B4 jal func_800A2704 */
    r_a3 = r_a3 >> 8;
    bl_jal(FN, 0x800A2704u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800A2704 */
    r_v0 = bl_lw(r_s4 + 0x4u); /* 800AB6BC lw $v0, 0x4($s4) */
    r_v1 = bl_lh(r_s4 + 0x1Cu); /* 800AB6C0 lh $v1, 0x1C($s4) */
    r_v0 = bl_lh(r_v0 + 0x50u); /* 800AB6C4 lh $v0, 0x50($v0) */
    ; /* 800AB6C8 nop  */
    BL_MULT(r_v1, r_v0); /* 800AB6CC mult $v1, $v0 */
    r_v0 = lo; /* 800AB6D0 mflo $v0 */
    r_v1 = bl_lh(r_s1 + 0x10u); /* 800AB6D4 lh $v1, 0x10($s1) */
    r_v0 = BL_SRA(r_v0, 12); /* 800AB6D8 sra $v0, $v0, 12 */
    BL_MULT(r_v1, r_v0); /* 800AB6DC mult $v1, $v0 */
    r_t8 = 0u + 0xFFFFFFFFu; /* 800AB6E0 addiu $t8, $zero, -0x1 */
    bl_sw(r_sp + 0x138u, r_t8); /* 800AB6E4 sw $t8, 0x138($sp) */
    r_v0 = lo; /* 800AB6E8 mflo $v0 */
    r_v0 = BL_SRA(r_v0, 12); /* 800AB6EC sra $v0, $v0, 12 */
    /* 800AB6F0 bgez $v0, .L800AB6FC */
    { int c_ = ((s32)r_v0 >= 0); ; if (c_) goto L_800AB6FC; }
    r_v0 = 0u - r_v0; /* 800AB6F8 negu $v0, $v0 */
L_800AB6FC:
    /* 800AB6FC j .L800ADEBC */
    bl_sh(r_s4 + 0x8Eu, r_v0); goto L_800ADEBC;
L_800AB704:
    r_v0 = bl_lhu(r_s3); /* 800AB704 lhu $v0, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800AB708 addiu $s3, $s3, 0x2 */
    r_a0 = r_s4 + 0u; /* 800AB70C addu $a0, $s4, $zero */
    r_a2 = r_sp + 0x98u; /* 800AB710 addiu $a2, $sp, 0x98 */
    r_t7 = 0u + 0xFFFFFFFFu; /* 800AB714 addiu $t7, $zero, -0x1 */
    bl_sw(r_sp + 0x138u, r_t7); /* 800AB718 sw $t7, 0x138($sp) */
    r_s2 = r_v0 >> 8; /* 800AB71C srl $s2, $v0, 8 */
    bl_sh(r_sp + 0x9Cu, r_v0); /* 800AB720 sh $v0, 0x9C($sp) */
    r_v0 = bl_lhu(r_s3); /* 800AB724 lhu $v0, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800AB728 addiu $s3, $s3, 0x2 */
    r_s1 = bl_lbu(r_sp + 0x9Cu); /* 800AB72C lbu $s1, 0x9C($sp) */
    r_a3 = r_v0 >> 8; /* 800AB730 srl $a3, $v0, 8 */
    bl_sh(r_sp + 0x9Cu, r_v0); /* 800AB734 sh $v0, 0x9C($sp) */
    r_s7 = bl_lbu(r_sp + 0x9Cu); /* 800AB738 lbu $s7, 0x9C($sp) */
    r_a1 = r_s1 + 0u; /* 800AB73C addu $a1, $s1, $zero */
    /* 800AB740 jal func_800AF518 */
    bl_sw(r_sp + 0x1C8u, r_a3);
    bl_jal(FN, 0x800AF518u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800AF518 */
    r_a2 = r_v0 + 0u; /* 800AB748 addu $a2, $v0, $zero */
    r_a0 = bl_lw(r_sp + 0x130u); /* 800AB74C lw $a0, 0x130($sp) */
    r_a3 = bl_lw(r_sp + 0x1C8u); /* 800AB750 lw $a3, 0x1C8($sp) */
    r_v0 = r_s6 & 0xFFu; /* 800AB754 andi $v0, $s6, 0xFF */
    bl_sw(r_sp + 0x10u, r_v0); /* 800AB758 sw $v0, 0x10($sp) */
    bl_sw(r_sp + 0x18u, r_s2); /* 800AB75C sw $s2, 0x18($sp) */
    bl_sw(r_sp + 0x14u, r_s7); /* 800AB760 sw $s7, 0x14($sp) */
    r_a1 = bl_lw(r_s4 + 0x4u); /* 800AB764 lw $a1, 0x4($s4) */
    /* 800AB768 jal func_800A1CF4 */
    ;
    bl_jal(FN, 0x800A1CF4u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800A1CF4 */
    /* 800AB770 j .L800ABBF8 */
    ; goto L_800ABBF8;
L_800AB778:
    r_v0 = bl_lhu(r_s3); /* 800AB778 lhu $v0, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800AB77C addiu $s3, $s3, 0x2 */
    r_a0 = r_s4 + 0u; /* 800AB780 addu $a0, $s4, $zero */
    r_s1 = r_sp + 0x9Cu; /* 800AB784 addiu $s1, $sp, 0x9C */
    r_a2 = r_s1 + 0u; /* 800AB788 addu $a2, $s1, $zero */
    bl_sh(r_sp + 0x9Cu, r_v0); /* 800AB78C sh $v0, 0x9C($sp) */
    r_s5 = bl_lbu(r_sp + 0x9Cu); /* 800AB790 lbu $s5, 0x9C($sp) */
    r_s2 = r_v0 >> 8; /* 800AB794 srl $s2, $v0, 8 */
    /* 800AB798 jal func_800AF438 */
    r_a1 = r_s5 + 0u;
    bl_jal(FN, 0x800AF438u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800AF438 */
    r_a3 = r_v0 + 0u; /* 800AB7A0 addu $a3, $v0, $zero */
    r_a0 = r_s4 + 0u; /* 800AB7A4 addu $a0, $s4, $zero */
    r_s0 = r_s6 & 0xFFu; /* 800AB7A8 andi $s0, $s6, 0xFF */
    r_a1 = r_s0 + 0u; /* 800AB7AC addu $a1, $s0, $zero */
    r_a2 = r_s1 + 0u; /* 800AB7B0 addu $a2, $s1, $zero */
    /* 800AB7B4 jal func_800AF438 */
    bl_sw(r_sp + 0x1C8u, r_a3);
    bl_jal(FN, 0x800AF438u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800AF438 */
    r_v0 = 0u | 0xFDu; /* 800AB7BC ori $v0, $zero, 0xFD */
    r_s7 = bl_lbu(r_s4 + 0x2Bu); /* 800AB7C0 lbu $s7, 0x2B($s4) */
    r_a3 = bl_lw(r_sp + 0x1C8u); /* 800AB7C4 lw $a3, 0x1C8($sp) */
    /* 800AB7C8 bne $s0, $v0, .L800AB7D4 */
    { int c_ = (r_s0 != r_v0); r_s1 = r_s5 + 0u; if (c_) goto L_800AB7D4; }
    bl_sb(r_s4 + 0x2Bu, 0u); /* 800AB7D0 sb $zero, 0x2B($s4) */
L_800AB7D4:
    bl_sw(r_sp + 0x98u, 0u); /* 800AB7D4 sw $zero, 0x98($sp) */
    r_v0 = r_a3 & 0xFFu; /* 800AB7D8 andi $v0, $a3, 0xFF */
    r_v0 = r_v0 << 2; /* 800AB7DC sll $v0, $v0, 2 */
    r_t8 = 0x800D0000u; /* 800AB7E0 lui $t8, %hi(D_800D3368) */
    r_t8 = r_t8 + 0x3368u; /* 800AB7E4 addiu $t8, $t8, %lo(D_800D3368) */
    r_s0 = r_v0 + r_t8; /* 800AB7E8 addu $s0, $v0, $t8 */
L_800AB7EC:
    r_v0 = bl_lh(r_sp + 0x9Cu); /* 800AB7EC lh $v0, 0x9C($sp) */
    r_v1 = bl_lw(r_sp + 0x98u); /* 800AB7F0 lw $v1, 0x98($sp) */
    ; /* 800AB7F4 nop  */
    r_v0 = BL_SRA(r_v0, r_v1); /* 800AB7F8 srav $v0, $v0, $v1 */
    r_v0 = r_v0 & 0x1u; /* 800AB7FC andi $v0, $v0, 0x1 */
    /* 800AB800 beqz $v0, .L800AB854 */
    { int c_ = (r_v0 == 0); r_t7 = 0u | 0xFFu; if (c_) goto L_800AB854; }
    /* 800AB808 bne $s1, $t7, .L800AB830 */
    { int c_ = (r_s1 != r_t7); r_v0 = r_v1 << 2; if (c_) goto L_800AB830; }
    r_t8 = 0x800D0000u; /* 800AB810 lui $t8, %hi(D_800D3368) */
    r_t8 = r_t8 + 0x3368u; /* 800AB814 addiu $t8, $t8, %lo(D_800D3368) */
    r_v0 = r_v0 + r_t8; /* 800AB818 addu $v0, $v0, $t8 */
    r_a3 = r_s2 & 0xFFu; /* 800AB81C andi $a3, $s2, 0xFF */
    r_a0 = bl_lw(r_v0); /* 800AB820 lw $a0, 0x0($v0) */
    r_a2 = bl_lw(r_sp + 0x130u); /* 800AB824 lw $a2, 0x130($sp) */
    /* 800AB828 j .L800AB84C */
    r_a1 = r_a0 + 0u; goto L_800AB84C;
L_800AB830:
    r_t7 = 0x800D0000u; /* 800AB830 lui $t7, %hi(D_800D3368) */
    r_t7 = r_t7 + 0x3368u; /* 800AB834 addiu $t7, $t7, %lo(D_800D3368) */
    r_v0 = r_v0 + r_t7; /* 800AB838 addu $v0, $v0, $t7 */
    r_a2 = bl_lw(r_sp + 0x130u); /* 800AB83C lw $a2, 0x130($sp) */
    r_a0 = bl_lw(r_v0); /* 800AB840 lw $a0, 0x0($v0) */
    r_a1 = bl_lw(r_s0); /* 800AB844 lw $a1, 0x0($s0) */
    r_a3 = r_s2 & 0xFFu; /* 800AB848 andi $a3, $s2, 0xFF */
L_800AB84C:
    /* 800AB84C jal func_800AA934 */
    ;
    bl_jal(FN, 0x800AA934u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800AA934 */
L_800AB854:
    r_v0 = bl_lw(r_sp + 0x98u); /* 800AB854 lw $v0, 0x98($sp) */
    ; /* 800AB858 nop  */
    r_v0 = r_v0 + 0x1u; /* 800AB85C addiu $v0, $v0, 0x1 */
    bl_sw(r_sp + 0x98u, r_v0); /* 800AB860 sw $v0, 0x98($sp) */
    r_v0 = (u32)((s32)r_v0 < 13); /* 800AB864 slti $v0, $v0, 0xD */
    /* 800AB868 bnez $v0, .L800AB7EC */
    { int c_ = (r_v0 != 0); r_v1 = r_s6 & 0xFFu; if (c_) goto L_800AB7EC; }
    r_v0 = 0u | 0xFDu; /* 800AB870 ori $v0, $zero, 0xFD */
    /* 800AB874 beq $v1, $v0, .L800ADEE8 */
    { int c_ = (r_v1 == r_v0); bl_sb(r_s4 + 0x2Bu, r_s7); if (c_) goto L_800ADEE8; }
    /* 800AB87C j .L800ADEBC */
    ; goto L_800ADEBC;
L_800AB884:
    r_a0 = 0u | 0x11Cu; /* 800AB884 ori $a0, $zero, 0x11C */
    /* 800AB888 jal HeapAlloc */
    r_a1 = 0u | 0x1u;
    bl_jal(FN, 0x80031BDCu, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* HeapAlloc */
    /* 800AB890 j .L800AB8E0 */
    bl_sw(r_sp + 0x168u, r_v0); goto L_800AB8E0;
L_800AB898:
    r_v1 = bl_lhu(r_s3); /* 800AB898 lhu $v1, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800AB89C addiu $s3, $s3, 0x2 */
    r_v0 = 0u | 0x13u; /* 800AB8A0 ori $v0, $zero, 0x13 */
    bl_sw(r_sp + 0x98u, r_v0); /* 800AB8A4 sw $v0, 0x98($sp) */
    bl_sh(r_sp + 0x9Cu, r_v1); /* 800AB8A8 sh $v1, 0x9C($sp) */
L_800AB8AC:
    r_v1 = bl_lw(r_sp + 0x98u); /* 800AB8AC lw $v1, 0x98($sp) */
    r_t8 = 0x800D0000u; /* 800AB8B0 lui $t8, %hi(D_800D3368) */
    r_t8 = r_t8 + 0x3368u; /* 800AB8B4 addiu $t8, $t8, %lo(D_800D3368) */
    r_v0 = r_v1 << 2; /* 800AB8B8 sll $v0, $v1, 2 */
    r_v0 = r_v0 + r_t8; /* 800AB8BC addu $v0, $v0, $t8 */
    r_v0 = bl_lw(r_v0); /* 800AB8C0 lw $v0, 0x0($v0) */
    ; /* 800AB8C4 nop  */
    /* 800AB8C8 beqz $v0, .L800AB884 */
    { int c_ = (r_v0 == 0); r_v0 = r_v1 + 0x1u; if (c_) goto L_800AB884; }
    bl_sw(r_sp + 0x98u, r_v0); /* 800AB8D0 sw $v0, 0x98($sp) */
    r_v0 = (u32)((s32)r_v0 < 31); /* 800AB8D4 slti $v0, $v0, 0x1F */
    /* 800AB8D8 bnez $v0, .L800AB8AC */
    { int c_ = (r_v0 != 0); ; if (c_) goto L_800AB8AC; }
L_800AB8E0:
    r_a3 = bl_lw(r_sp + 0x168u); /* 800AB8E0 lw $a3, 0x168($sp) */
    r_a2 = r_s4 + 0u; /* 800AB8E4 addu $a2, $s4, $zero */
    r_t0 = r_s4 + 0x110u; /* 800AB8E8 addiu $t0, $s4, 0x110 */
L_800AB8EC:
    r_v0 = bl_lw(r_a2); /* 800AB8EC lw $v0, 0x0($a2) */
    r_v1 = bl_lw(r_a2 + 0x4u); /* 800AB8F0 lw $v1, 0x4($a2) */
    r_a0 = bl_lw(r_a2 + 0x8u); /* 800AB8F4 lw $a0, 0x8($a2) */
    r_a1 = bl_lw(r_a2 + 0xCu); /* 800AB8F8 lw $a1, 0xC($a2) */
    bl_sw(r_a3, r_v0); /* 800AB8FC sw $v0, 0x0($a3) */
    bl_sw(r_a3 + 0x4u, r_v1); /* 800AB900 sw $v1, 0x4($a3) */
    bl_sw(r_a3 + 0x8u, r_a0); /* 800AB904 sw $a0, 0x8($a3) */
    bl_sw(r_a3 + 0xCu, r_a1); /* 800AB908 sw $a1, 0xC($a3) */
    r_a2 = r_a2 + 0x10u; /* 800AB90C addiu $a2, $a2, 0x10 */
    /* 800AB910 bne $a2, $t0, .L800AB8EC */
    { int c_ = (r_a2 != r_t0); r_a3 = r_a3 + 0x10u; if (c_) goto L_800AB8EC; }
    r_v0 = bl_lw(r_a2); /* 800AB918 lw $v0, 0x0($a2) */
    r_v1 = bl_lw(r_a2 + 0x4u); /* 800AB91C lw $v1, 0x4($a2) */
    r_a0 = bl_lw(r_a2 + 0x8u); /* 800AB920 lw $a0, 0x8($a2) */
    bl_sw(r_a3, r_v0); /* 800AB924 sw $v0, 0x0($a3) */
    bl_sw(r_a3 + 0x4u, r_v1); /* 800AB928 sw $v1, 0x4($a3) */
    bl_sw(r_a3 + 0x8u, r_a0); /* 800AB92C sw $a0, 0x8($a3) */
    r_t7 = 0x800D0000u; /* 800AB930 lui $t7, %hi(D_800D3368) */
    r_t7 = r_t7 + 0x3368u; /* 800AB934 addiu $t7, $t7, %lo(D_800D3368) */
    r_v1 = 0u + 0xFFFFFFFFu; /* 800AB938 addiu $v1, $zero, -0x1 */
    r_v0 = bl_lw(r_sp + 0x98u); /* 800AB93C lw $v0, 0x98($sp) */
    r_t8 = bl_lw(r_sp + 0x168u); /* 800AB940 lw $t8, 0x168($sp) */
    r_v0 = r_v0 << 2; /* 800AB944 sll $v0, $v0, 2 */
    r_v0 = r_v0 + r_t7; /* 800AB948 addu $v0, $v0, $t7 */
    bl_sw(r_v0, r_t8); /* 800AB94C sw $t8, 0x0($v0) */
    r_v0 = 0u | 0xFFFFu; /* 800AB950 ori $v0, $zero, 0xFFFF */
    r_t7 = 0u | 0xFFu; /* 800AB954 ori $t7, $zero, 0xFF */
    bl_sh(r_t8 + 0x3Cu, r_v0); /* 800AB958 sh $v0, 0x3C($t8) */
    r_v0 = 0u | 0x6Bu; /* 800AB95C ori $v0, $zero, 0x6B */
    bl_sh(r_t8 + 0x98u, r_v1); /* 800AB960 sh $v1, 0x98($t8) */
    bl_sh(r_t8 + 0x40u, 0u); /* 800AB964 sh $zero, 0x40($t8) */
    bl_sh(r_t8 + 0x58u, 0u); /* 800AB968 sh $zero, 0x58($t8) */
    bl_sb(r_t8 + 0x5Cu, r_t7); /* 800AB96C sb $t7, 0x5C($t8) */
    bl_sh(r_t8 + 0x90u, r_v1); /* 800AB970 sh $v1, 0x90($t8) */
    bl_sb(r_t8 + 0x62u, 0u); /* 800AB974 sb $zero, 0x62($t8) */
    bl_sw(r_t8 + 0xA8u, 0u); /* 800AB978 sw $zero, 0xA8($t8) */
    bl_sw(r_t8 + 0xACu, 0u); /* 800AB97C sw $zero, 0xAC($t8) */
    bl_sb(r_t8 + 0x39u, r_v0); /* 800AB980 sb $v0, 0x39($t8) */
    bl_sw(r_t8 + 0x10u, 0u); /* 800AB984 sw $zero, 0x10($t8) */
    r_v0 = bl_lbu(r_s4 + 0x20u); /* 800AB988 lbu $v0, 0x20($s4) */
    ; /* 800AB98C nop  */
    bl_sb(r_t8 + 0x21u, r_v0); /* 800AB990 sb $v0, 0x21($t8) */
    r_v0 = bl_lbu(r_sp + 0x98u); /* 800AB994 lbu $v0, 0x98($sp) */
    r_a0 = bl_lw(r_sp + 0x168u); /* 800AB998 lw $a0, 0x168($sp) */
    bl_sb(r_t8 + 0x10Du, 0u); /* 800AB99C sb $zero, 0x10D($t8) */
    bl_sb(r_t8 + 0x10Eu, 0u); /* 800AB9A0 sb $zero, 0x10E($t8) */
    /* 800AB9A4 jal func_800AA6E0 */
    bl_sb(r_t8 + 0x20u, r_v0);
    bl_jal(FN, 0x800AA6E0u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800AA6E0 */
    r_v0 = bl_lw(r_s4 + 0x4u); /* 800AB9AC lw $v0, 0x4($s4) */
    ; /* 800AB9B0 nop  */
    r_v0 = bl_lhu(r_v0 + 0xAu); /* 800AB9B4 lhu $v0, 0xA($v0) */
    r_a1 = 0u | 0x1u; /* 800AB9B8 ori $a1, $zero, 0x1 */
    r_a0 = r_v0 << 5; /* 800AB9BC sll $a0, $v0, 5 */
    r_a0 = r_a0 - r_v0; /* 800AB9C0 subu $a0, $a0, $v0 */
    /* 800AB9C4 jal HeapAlloc */
    r_a0 = r_a0 << 2;
    bl_jal(FN, 0x80031BDCu, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* HeapAlloc */
    r_t8 = bl_lw(r_sp + 0x168u); /* 800AB9CC lw $t8, 0x168($sp) */
    r_a3 = r_v0 + 0u; /* 800AB9D0 addu $a3, $v0, $zero */
    bl_sw(r_t8 + 0x4u, r_a3); /* 800AB9D4 sw $a3, 0x4($t8) */
    r_a0 = bl_lw(r_s4 + 0x4u); /* 800AB9D8 lw $a0, 0x4($s4) */
    ; /* 800AB9DC nop  */
    r_v0 = bl_lhu(r_a0 + 0xAu); /* 800AB9E0 lhu $v0, 0xA($a0) */
    ; /* 800AB9E4 nop  */
    /* 800AB9E8 beqz $v0, .L800ABB08 */
    { int c_ = (r_v0 == 0); bl_sw(r_sp + 0x98u, 0u); if (c_) goto L_800ABB08; }
L_800AB9F0:
    r_v1 = bl_lw(r_sp + 0x98u); /* 800AB9F0 lw $v1, 0x98($sp) */
    ; /* 800AB9F4 nop  */
    r_v0 = r_v1 << 5; /* 800AB9F8 sll $v0, $v1, 5 */
    r_v0 = r_v0 - r_v1; /* 800AB9FC subu $v0, $v0, $v1 */
    r_v0 = r_v0 << 2; /* 800ABA00 sll $v0, $v0, 2 */
    r_t0 = r_v0 + r_a3; /* 800ABA04 addu $t0, $v0, $a3 */
    r_a2 = r_v0 + r_a0; /* 800ABA08 addu $a2, $v0, $a0 */
    r_t1 = r_a2 + 0x70u; /* 800ABA0C addiu $t1, $a2, 0x70 */
L_800ABA10:
    r_v0 = bl_lw(r_a2); /* 800ABA10 lw $v0, 0x0($a2) */
    r_v1 = bl_lw(r_a2 + 0x4u); /* 800ABA14 lw $v1, 0x4($a2) */
    r_a0 = bl_lw(r_a2 + 0x8u); /* 800ABA18 lw $a0, 0x8($a2) */
    r_a1 = bl_lw(r_a2 + 0xCu); /* 800ABA1C lw $a1, 0xC($a2) */
    bl_sw(r_t0, r_v0); /* 800ABA20 sw $v0, 0x0($t0) */
    bl_sw(r_t0 + 0x4u, r_v1); /* 800ABA24 sw $v1, 0x4($t0) */
    bl_sw(r_t0 + 0x8u, r_a0); /* 800ABA28 sw $a0, 0x8($t0) */
    bl_sw(r_t0 + 0xCu, r_a1); /* 800ABA2C sw $a1, 0xC($t0) */
    r_a2 = r_a2 + 0x10u; /* 800ABA30 addiu $a2, $a2, 0x10 */
    /* 800ABA34 bne $a2, $t1, .L800ABA10 */
    { int c_ = (r_a2 != r_t1); r_t0 = r_t0 + 0x10u; if (c_) goto L_800ABA10; }
    r_v0 = bl_lw(r_a2); /* 800ABA3C lw $v0, 0x0($a2) */
    r_v1 = bl_lw(r_a2 + 0x4u); /* 800ABA40 lw $v1, 0x4($a2) */
    r_a0 = bl_lw(r_a2 + 0x8u); /* 800ABA44 lw $a0, 0x8($a2) */
    bl_sw(r_t0, r_v0); /* 800ABA48 sw $v0, 0x0($t0) */
    bl_sw(r_t0 + 0x4u, r_v1); /* 800ABA4C sw $v1, 0x4($t0) */
    bl_sw(r_t0 + 0x8u, r_a0); /* 800ABA50 sw $a0, 0x8($t0) */
    r_v0 = bl_lw(r_sp + 0x98u); /* 800ABA54 lw $v0, 0x98($sp) */
    r_a1 = bl_lw(r_s4 + 0x4u); /* 800ABA58 lw $a1, 0x4($s4) */
    r_v1 = r_v0 << 5; /* 800ABA5C sll $v1, $v0, 5 */
    r_v1 = r_v1 - r_v0; /* 800ABA60 subu $v1, $v1, $v0 */
    r_a0 = r_v1 << 2; /* 800ABA64 sll $a0, $v1, 2 */
    r_v0 = r_a0 + r_a1; /* 800ABA68 addu $v0, $a0, $a1 */
    r_v0 = bl_lw(r_v0); /* 800ABA6C lw $v0, 0x0($v0) */
    ; /* 800ABA70 nop  */
    /* 800ABA74 beqz $v0, .L800ABAAC */
    { int c_ = (r_v0 == 0); r_v1 = 0x21080000u; if (c_) goto L_800ABAAC; }
    r_v1 = r_v1 | 0x4211u; /* 800ABA7C ori $v1, $v1, (0x21084211 & 0xFFFF) */
    r_v0 = r_v0 - r_a1; /* 800ABA80 subu $v0, $v0, $a1 */
    r_v0 = r_v0 >> 2; /* 800ABA84 srl $v0, $v0, 2 */
    BL_MULTU(r_v0, r_v1); /* 800ABA88 multu $v0, $v1 */
    r_a0 = r_a0 + r_a3; /* 800ABA8C addu $a0, $a0, $a3 */
    r_v1 = hi; /* 800ABA90 mfhi $v1 */
    r_v1 = r_v1 >> 2; /* 800ABA94 srl $v1, $v1, 2 */
    r_v0 = r_v1 << 5; /* 800ABA98 sll $v0, $v1, 5 */
    r_v0 = r_v0 - r_v1; /* 800ABA9C subu $v0, $v0, $v1 */
    r_v0 = r_v0 << 2; /* 800ABAA0 sll $v0, $v0, 2 */
    r_v0 = r_a3 + r_v0; /* 800ABAA4 addu $v0, $a3, $v0 */
    bl_sw(r_a0, r_v0); /* 800ABAA8 sw $v0, 0x0($a0) */
L_800ABAAC:
    r_v1 = bl_lw(r_sp + 0x98u); /* 800ABAAC lw $v1, 0x98($sp) */
    ; /* 800ABAB0 nop  */
    r_v0 = r_v1 << 5; /* 800ABAB4 sll $v0, $v1, 5 */
    r_v0 = r_v0 - r_v1; /* 800ABAB8 subu $v0, $v0, $v1 */
    r_v0 = r_v0 << 2; /* 800ABABC sll $v0, $v0, 2 */
    r_v0 = r_v0 + r_a3; /* 800ABAC0 addu $v0, $v0, $a3 */
    bl_sb(r_v0 + 0x7u, 0u); /* 800ABAC4 sb $zero, 0x7($v0) */
    r_v1 = bl_lw(r_sp + 0x98u); /* 800ABAC8 lw $v1, 0x98($sp) */
    ; /* 800ABACC nop  */
    r_v0 = r_v1 << 5; /* 800ABAD0 sll $v0, $v1, 5 */
    r_v0 = r_v0 - r_v1; /* 800ABAD4 subu $v0, $v0, $v1 */
    r_v0 = r_v0 << 2; /* 800ABAD8 sll $v0, $v0, 2 */
    r_v0 = r_v0 + r_a3; /* 800ABADC addu $v0, $v0, $a3 */
    bl_sw(r_v0 + 0x70u, 0u); /* 800ABAE0 sw $zero, 0x70($v0) */
    bl_sw(r_v0 + 0x74u, 0u); /* 800ABAE4 sw $zero, 0x74($v0) */
    r_a0 = bl_lw(r_s4 + 0x4u); /* 800ABAE8 lw $a0, 0x4($s4) */
    ; /* 800ABAEC nop  */
    r_v0 = bl_lhu(r_a0 + 0xAu); /* 800ABAF0 lhu $v0, 0xA($a0) */
    r_v1 = r_v1 + 0x1u; /* 800ABAF4 addiu $v1, $v1, 0x1 */
    bl_sw(r_sp + 0x98u, r_v1); /* 800ABAF8 sw $v1, 0x98($sp) */
    r_v1 = (u32)((s32)r_v1 < (s32)r_v0); /* 800ABAFC slt $v1, $v1, $v0 */
    /* 800ABB00 bnez $v1, .L800AB9F0 */
    { int c_ = (r_v1 != 0); ; if (c_) goto L_800AB9F0; }
L_800ABB08:
    r_a0 = bl_lw(r_sp + 0x130u); /* 800ABB08 lw $a0, 0x130($sp) */
    r_a1 = bl_lh(r_sp + 0x9Cu); /* 800ABB0C lh $a1, 0x9C($sp) */
    r_a2 = bl_lw(r_s4 + 0x4u); /* 800ABB10 lw $a2, 0x4($s4) */
    /* 800ABB14 jal func_800AF180 */
    ;
    bl_jal(FN, 0x800AF180u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800AF180 */
    r_a3 = r_s6 & 0xFFu; /* 800ABB1C andi $a3, $s6, 0xFF */
    r_t7 = 0u | 0xFFu; /* 800ABB20 ori $t7, $zero, 0xFF */
    /* 800ABB24 beq $a3, $t7, .L800ADEBC */
    { int c_ = (r_a3 == r_t7); ; if (c_) goto L_800ADEBC; }
    r_a0 = bl_lw(r_sp + 0x168u); /* 800ABB2C lw $a0, 0x168($sp) */
    r_a1 = bl_lw(r_sp + 0x168u); /* 800ABB30 lw $a1, 0x168($sp) */
    r_a2 = bl_lw(r_sp + 0x130u); /* 800ABB34 lw $a2, 0x130($sp) */
    /* 800ABB38 jal func_800AA934 */
    ;
    bl_jal(FN, 0x800AA934u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800AA934 */
    /* 800ABB40 j .L800ADEBC */
    ; goto L_800ADEBC;
L_800ABB48:
    r_v0 = bl_lbu(r_s4 + 0x21u); /* 800ABB48 lbu $v0, 0x21($s4) */
    r_t8 = 0x800D0000u; /* 800ABB4C lui $t8, %hi(D_800D3368) */
    r_t8 = r_t8 + 0x3368u; /* 800ABB50 addiu $t8, $t8, %lo(D_800D3368) */
    r_v0 = r_v0 << 2; /* 800ABB54 sll $v0, $v0, 2 */
    r_v0 = r_v0 + r_t8; /* 800ABB58 addu $v0, $v0, $t8 */
    r_v0 = bl_lw(r_v0); /* 800ABB5C lw $v0, 0x0($v0) */
    r_a0 = bl_lw(r_s4 + 0x4u); /* 800ABB60 lw $a0, 0x4($s4) */
    r_a1 = bl_lw(r_v0 + 0x4u); /* 800ABB64 lw $a1, 0x4($v0) */
    /* 800ABB68 jal func_800AF270 */
    ;
    bl_jal(FN, 0x800AF270u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800AF270 */
    r_a0 = bl_lbu(r_s4 + 0x20u); /* 800ABB70 lbu $a0, 0x20($s4) */
    /* 800ABB74 jal func_800A9FF0 */
    ;
    bl_jal(FN, 0x800A9FF0u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800A9FF0 */
    r_a3 = r_s6 & 0xFFu; /* 800ABB7C andi $a3, $s6, 0xFF */
    r_t7 = 0u | 0xFFu; /* 800ABB80 ori $t7, $zero, 0xFF */
    /* 800ABB84 beq $a3, $t7, .L800ADEE8 */
    { int c_ = (r_a3 == r_t7); ; if (c_) goto L_800ADEE8; }
    r_v0 = bl_lbu(r_s4 + 0x21u); /* 800ABB8C lbu $v0, 0x21($s4) */
    r_t8 = 0x800D0000u; /* 800ABB90 lui $t8, %hi(D_800D3368) */
    r_t8 = r_t8 + 0x3368u; /* 800ABB94 addiu $t8, $t8, %lo(D_800D3368) */
    r_v0 = r_v0 << 2; /* 800ABB98 sll $v0, $v0, 2 */
    r_v0 = r_v0 + r_t8; /* 800ABB9C addu $v0, $v0, $t8 */
    r_a0 = bl_lw(r_v0); /* 800ABBA0 lw $a0, 0x0($v0) */
    r_a2 = bl_lw(r_sp + 0x130u); /* 800ABBA4 lw $a2, 0x130($sp) */
    /* 800ABBA8 jal func_800AA934 */
    r_a1 = r_a0 + 0u;
    bl_jal(FN, 0x800AA934u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800AA934 */
    /* 800ABBB0 j .L800ADEE8 */
    ; goto L_800ADEE8;
L_800ABBB8:
    r_a0 = bl_lbu(r_s4 + 0x20u); /* 800ABBB8 lbu $a0, 0x20($s4) */
    /* 800ABBBC jal func_800A9FF0 */
    ;
    bl_jal(FN, 0x800A9FF0u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800A9FF0 */
    /* 800ABBC4 j .L800ADEE8 */
    ; goto L_800ADEE8;
L_800ABBCC:
    r_a0 = r_s4 + 0u; /* 800ABBCC addu $a0, $s4, $zero */
    r_a1 = r_s6 & 0xFFu; /* 800ABBD0 andi $a1, $s6, 0xFF */
    /* 800ABBD4 jal func_800AF518 */
    r_a2 = r_sp + 0x98u;
    bl_jal(FN, 0x800AF518u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800AF518 */
    r_a2 = bl_lh(r_s3); /* 800ABBDC lh $a2, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800ABBE0 addiu $s3, $s3, 0x2 */
    r_a0 = r_s4 + 0u; /* 800ABBE4 addu $a0, $s4, $zero */
    /* 800ABBE8 jal func_800AE1BC */
    r_a1 = r_v0 + 0u;
    bl_jal(FN, 0x800AE1BCu, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800AE1BC */
    /* 800ABBF0 j .L800ADEBC */
    ; goto L_800ADEBC;
L_800ABBF8:
    /* 800ABBF8 jal func_800AEEEC */
    r_a0 = r_s4 + 0u;
    bl_jal(FN, 0x800AEEECu, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800AEEEC */
    /* 800ABC00 j .L800ADEBC */
    ; goto L_800ADEBC;
L_800ABC08:
    r_a0 = bl_lhu(r_s3); /* 800ABC08 lhu $a0, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800ABC0C addiu $s3, $s3, 0x2 */
    bl_sh(r_sp + 0xA0u, r_a0); /* 800ABC10 sh $a0, 0xA0($sp) */
    r_a3 = bl_lhu(r_s3); /* 800ABC14 lhu $a3, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800ABC18 addiu $s3, $s3, 0x2 */
    bl_sh(r_sp + 0xA2u, r_a3); /* 800ABC1C sh $a3, 0xA2($sp) */
    r_a1 = bl_lhu(r_s3); /* 800ABC20 lhu $a1, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800ABC24 addiu $s3, $s3, 0x2 */
    r_a2 = bl_lhu(r_s3); /* 800ABC28 lhu $a2, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800ABC2C addiu $s3, $s3, 0x2 */
    r_v0 = bl_lhu(r_s3); /* 800ABC30 lhu $v0, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800ABC34 addiu $s3, $s3, 0x2 */
    r_v0 = r_v0 << 16; /* 800ABC38 sll $v0, $v0, 16 */
    r_v0 = BL_SRA(r_v0, 16); /* 800ABC3C sra $v0, $v0, 16 */
    r_v0 = r_v0 + 0x1u; /* 800ABC40 addiu $v0, $v0, 0x1 */
    r_v1 = r_v0 >> 31; /* 800ABC44 srl $v1, $v0, 31 */
    r_v0 = r_v0 + r_v1; /* 800ABC48 addu $v0, $v0, $v1 */
    r_v0 = r_v0 & 0xFFFEu; /* 800ABC4C andi $v0, $v0, 0xFFFE */
    bl_sh(r_sp + 0xA4u, r_v0); /* 800ABC50 sh $v0, 0xA4($sp) */
    r_v0 = bl_lhu(r_s3); /* 800ABC54 lhu $v0, 0x0($s3) */
    ; /* 800ABC58 nop  */
    bl_sh(r_sp + 0xA6u, r_v0); /* 800ABC5C sh $v0, 0xA6($sp) */
    r_v0 = r_s6 & 0x1u; /* 800ABC60 andi $v0, $s6, 0x1 */
    /* 800ABC64 beqz $v0, .L800ABCAC */
    { int c_ = (r_v0 == 0); r_s3 = r_s3 + 0x2u; if (c_) goto L_800ABCAC; }
    r_v0 = bl_lh(r_s4 + 0x90u); /* 800ABC6C lh $v0, 0x90($s4) */
    ; /* 800ABC70 nop  */
    /* 800ABC74 bltz $v0, .L800ADEBC */
    { int c_ = ((s32)r_v0 < 0); r_v1 = r_v0 + 0u; if (c_) goto L_800ADEBC; }
    r_v0 = r_a0 + r_v1; /* 800ABC7C addu $v0, $a0, $v1 */
    bl_sh(r_sp + 0xA0u, r_v0); /* 800ABC80 sh $v0, 0xA0($sp) */
    r_v0 = bl_lhu(r_s4 + 0x92u); /* 800ABC84 lhu $v0, 0x92($s4) */
    ; /* 800ABC88 nop  */
    r_v0 = r_a3 + r_v0; /* 800ABC8C addu $v0, $a3, $v0 */
    bl_sh(r_sp + 0xA2u, r_v0); /* 800ABC90 sh $v0, 0xA2($sp) */
    r_v0 = bl_lhu(r_s4 + 0x90u); /* 800ABC94 lhu $v0, 0x90($s4) */
    r_v1 = bl_lhu(r_s4 + 0x92u); /* 800ABC98 lhu $v1, 0x92($s4) */
    r_v0 = r_a1 + r_v0; /* 800ABC9C addu $v0, $a1, $v0 */
    r_a1 = r_v0 + 0u; /* 800ABCA0 addu $a1, $v0, $zero */
    r_v1 = r_a2 + r_v1; /* 800ABCA4 addu $v1, $a2, $v1 */
    r_a2 = r_v1 + 0u; /* 800ABCA8 addu $a2, $v1, $zero */
L_800ABCAC:
    r_a0 = r_sp + 0xA0u; /* 800ABCAC addiu $a0, $sp, 0xA0 */
    r_a1 = r_a1 << 16; /* 800ABCB0 sll $a1, $a1, 16 */
    r_a1 = BL_SRA(r_a1, 16); /* 800ABCB4 sra $a1, $a1, 16 */
    r_a2 = r_a2 << 16; /* 800ABCB8 sll $a2, $a2, 16 */
    /* 800ABCBC jal MoveImage */
    r_a2 = BL_SRA(r_a2, 16);
    bl_jal(FN, 0x8004495Cu, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* MoveImage */
    /* 800ABCC4 j .L800ADEBC */
    ; goto L_800ADEBC;
L_800ABCCC:
    r_v1 = bl_lbu(r_s4 + 0x10Eu); /* 800ABCCC lbu $v1, 0x10E($s4) */
    r_v0 = r_s6 & 0xFFu; /* 800ABCD0 andi $v0, $s6, 0xFF */
    r_v0 = (u32)(r_v0 < r_v1); /* 800ABCD4 sltu $v0, $v0, $v1 */
    /* 800ABCD8 beqz $v0, .L800ABEF4 */
    { int c_ = (r_v0 == 0); r_t7 = 0u | 0xFFu; if (c_) goto L_800ABEF4; }
    r_v0 = bl_lhu(r_s3); /* 800ABCE0 lhu $v0, 0x0($s3) */
    ; /* 800ABCE4 nop  */
    bl_sh(r_sp + 0x9Cu, r_v0); /* 800ABCE8 sh $v0, 0x9C($sp) */
    r_v1 = bl_lbu(r_sp + 0x9Cu); /* 800ABCEC lbu $v1, 0x9C($sp) */
    ; /* 800ABCF0 nop  */
    /* 800ABCF4 beq $v1, $t7, .L800ABD24 */
    { int c_ = (r_v1 == r_t7); r_s3 = r_s3 + 0x2u; if (c_) goto L_800ABD24; }
    r_v0 = bl_lbu(r_s4 + 0x10Eu); /* 800ABCFC lbu $v0, 0x10E($s4) */
    ; /* 800ABD00 nop  */
    r_v0 = (u32)(r_v1 < r_v0); /* 800ABD04 sltu $v0, $v1, $v0 */
    /* 800ABD08 beqz $v0, .L800ABD24 */
    { int c_ = (r_v0 == 0); r_v0 = r_v1 << 1; if (c_) goto L_800ABD24; }
    r_v0 = r_v0 + r_v1; /* 800ABD10 addu $v0, $v0, $v1 */
    r_v1 = bl_lw(r_s4 + 0x118u); /* 800ABD14 lw $v1, 0x118($s4) */
    r_v0 = r_v0 << 4; /* 800ABD18 sll $v0, $v0, 4 */
    /* 800ABD1C j .L800ABD28 */
    r_s1 = r_v0 + r_v1; goto L_800ABD28;
L_800ABD24:
    r_s1 = 0u + 0u; /* 800ABD24 addu $s1, $zero, $zero */
L_800ABD28:
    r_v0 = bl_lhu(r_sp + 0x9Cu); /* 800ABD28 lhu $v0, 0x9C($sp) */
    ; /* 800ABD2C nop  */
    r_v0 = r_v0 << 16; /* 800ABD30 sll $v0, $v0, 16 */
    r_v0 = BL_SRA(r_v0, 24); /* 800ABD34 sra $v0, $v0, 24 */
    r_s2 = r_v0 + 0u; /* 800ABD38 addu $s2, $v0, $zero */
    r_v0 = r_v0 & 0x7Fu; /* 800ABD3C andi $v0, $v0, 0x7F */
    r_v0 = (u32)(r_v0 < 0x4u); /* 800ABD40 sltiu $v0, $v0, 0x4 */
    /* 800ABD44 bnez $v0, .L800ABD54 */
    { int c_ = (r_v0 != 0); r_s0 = 0u + 0u; if (c_) goto L_800ABD54; }
    r_s0 = 0x800D0000u; /* 800ABD4C lui $s0, %hi(D_800D2FC0) */
    r_s0 = bl_lw(r_s0 + 0x2FC0u); /* 800ABD50 lw $s0, %lo(D_800D2FC0)($s0) */
L_800ABD54:
    r_v0 = bl_lhu(r_s3); /* 800ABD54 lhu $v0, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800ABD58 addiu $s3, $s3, 0x2 */
    bl_sh(r_sp + 0x9Cu, r_v0); /* 800ABD5C sh $v0, 0x9C($sp) */
    r_s7 = bl_lbu(r_sp + 0x9Cu); /* 800ABD60 lbu $s7, 0x9C($sp) */
    /* 800ABD64 jal func_800AA820 */
    r_a0 = r_v0 >> 8;
    bl_jal(FN, 0x800AA820u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800AA820 */
    r_a1 = bl_lhu(r_s3); /* 800ABD6C lhu $a1, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800ABD70 addiu $s3, $s3, 0x2 */
    r_a2 = bl_lhu(r_s3); /* 800ABD74 lhu $a2, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800ABD78 addiu $s3, $s3, 0x2 */
    r_t4 = bl_lhu(r_s3); /* 800ABD7C lhu $t4, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800ABD80 addiu $s3, $s3, 0x2 */
    r_a3 = bl_lhu(r_s3); /* 800ABD84 lhu $a3, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800ABD88 addiu $s3, $s3, 0x2 */
    r_t1 = bl_lhu(r_s3); /* 800ABD8C lhu $t1, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800ABD90 addiu $s3, $s3, 0x2 */
    r_t5 = bl_lhu(r_s3); /* 800ABD94 lhu $t5, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800ABD98 addiu $s3, $s3, 0x2 */
    r_t2 = bl_lhu(r_s3); /* 800ABD9C lhu $t2, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800ABDA0 addiu $s3, $s3, 0x2 */
    r_t3 = bl_lhu(r_s3); /* 800ABDA4 lhu $t3, 0x0($s3) */
    r_t6 = r_v0 + 0u; /* 800ABDA8 addu $t6, $v0, $zero */
    r_v0 = r_s2 & 0x80u; /* 800ABDAC andi $v0, $s2, 0x80 */
    /* 800ABDB0 beqz $v0, .L800ABE20 */
    { int c_ = (r_v0 == 0); r_s3 = r_s3 + 0x2u; if (c_) goto L_800ABE20; }
    r_v0 = bl_lh(r_s4 + 0x90u); /* 800ABDB8 lh $v0, 0x90($s4) */
    ; /* 800ABDBC nop  */
    /* 800ABDC0 bgez $v0, .L800ABDD0 */
    { int c_ = ((s32)r_v0 >= 0); r_t8 = 0u | 0x1u; if (c_) goto L_800ABDD0; }
    /* 800ABDC8 j .L800ADEBC */
    r_s3 = r_s3 + 0xAu; goto L_800ADEBC;
L_800ABDD0:
    r_v1 = bl_lhu(r_s4 + 0x94u); /* 800ABDD0 lhu $v1, 0x94($s4) */
    r_a0 = bl_lhu(r_s4 + 0x96u); /* 800ABDD4 lhu $a0, 0x96($s4) */
    r_v0 = r_t2 + r_v1; /* 800ABDD8 addu $v0, $t2, $v1 */
    r_t2 = r_v0 + 0u; /* 800ABDDC addu $t2, $v0, $zero */
    r_v0 = r_t3 + r_a0; /* 800ABDE0 addu $v0, $t3, $a0 */
    r_t3 = r_v0 + 0u; /* 800ABDE4 addu $t3, $v0, $zero */
    r_v0 = r_s7 & 0xFu; /* 800ABDE8 andi $v0, $s7, 0xF */
    /* 800ABDEC bne $v0, $t8, .L800ABE04 */
    { int c_ = (r_v0 != r_t8); r_t7 = 0u | 0x1u; if (c_) goto L_800ABE04; }
    r_v0 = r_a1 + r_v1; /* 800ABDF4 addu $v0, $a1, $v1 */
    r_a1 = r_v0 + 0u; /* 800ABDF8 addu $a1, $v0, $zero */
    r_v0 = r_a2 + r_a0; /* 800ABDFC addu $v0, $a2, $a0 */
    r_a2 = r_v0 + 0u; /* 800ABE00 addu $a2, $v0, $zero */
L_800ABE04:
    r_v0 = r_s7 >> 4; /* 800ABE04 srl $v0, $s7, 4 */
    /* 800ABE08 bne $v0, $t7, .L800ABE24 */
    { int c_ = (r_v0 != r_t7); r_v0 = r_a1 << 16; if (c_) goto L_800ABE24; }
    r_v0 = r_a3 + r_v1; /* 800ABE10 addu $v0, $a3, $v1 */
    r_a3 = r_v0 + 0u; /* 800ABE14 addu $a3, $v0, $zero */
    r_v0 = r_t1 + r_a0; /* 800ABE18 addu $v0, $t1, $a0 */
    r_t1 = r_v0 + 0u; /* 800ABE1C addu $t1, $v0, $zero */
L_800ABE20:
    r_v0 = r_a1 << 16; /* 800ABE20 sll $v0, $a1, 16 */
L_800ABE24:
    r_t0 = bl_lw(r_s4 + 0x118u); /* 800ABE24 lw $t0, 0x118($s4) */
    r_v0 = BL_SRA(r_v0, 16); /* 800ABE28 sra $v0, $v0, 16 */
    bl_sw(r_sp + 0x14u, r_v0); /* 800ABE2C sw $v0, 0x14($sp) */
    r_v0 = r_a2 << 16; /* 800ABE30 sll $v0, $a2, 16 */
    r_v0 = BL_SRA(r_v0, 16); /* 800ABE34 sra $v0, $v0, 16 */
    bl_sw(r_sp + 0x18u, r_v0); /* 800ABE38 sw $v0, 0x18($sp) */
    r_v0 = r_t4 << 16; /* 800ABE3C sll $v0, $t4, 16 */
    r_v0 = BL_SRA(r_v0, 16); /* 800ABE40 sra $v0, $v0, 16 */
    bl_sw(r_sp + 0x1Cu, r_v0); /* 800ABE44 sw $v0, 0x1C($sp) */
    r_v0 = r_a3 << 16; /* 800ABE48 sll $v0, $a3, 16 */
    r_v0 = BL_SRA(r_v0, 16); /* 800ABE4C sra $v0, $v0, 16 */
    bl_sw(r_sp + 0x20u, r_v0); /* 800ABE50 sw $v0, 0x20($sp) */
    r_v0 = r_t1 << 16; /* 800ABE54 sll $v0, $t1, 16 */
    r_v0 = BL_SRA(r_v0, 16); /* 800ABE58 sra $v0, $v0, 16 */
    bl_sw(r_sp + 0x24u, r_v0); /* 800ABE5C sw $v0, 0x24($sp) */
    r_v0 = r_t5 << 16; /* 800ABE60 sll $v0, $t5, 16 */
    r_v0 = BL_SRA(r_v0, 16); /* 800ABE64 sra $v0, $v0, 16 */
    bl_sw(r_sp + 0x28u, r_v0); /* 800ABE68 sw $v0, 0x28($sp) */
    r_v0 = r_t2 << 16; /* 800ABE6C sll $v0, $t2, 16 */
    r_v0 = BL_SRA(r_v0, 16); /* 800ABE70 sra $v0, $v0, 16 */
    bl_sw(r_sp + 0x2Cu, r_v0); /* 800ABE74 sw $v0, 0x2C($sp) */
    r_v0 = r_t3 << 16; /* 800ABE78 sll $v0, $t3, 16 */
    r_v0 = BL_SRA(r_v0, 16); /* 800ABE7C sra $v0, $v0, 16 */
    bl_sw(r_sp + 0x10u, r_s0); /* 800ABE80 sw $s0, 0x10($sp) */
    bl_sw(r_sp + 0x30u, r_v0); /* 800ABE84 sw $v0, 0x30($sp) */
    r_v0 = bl_lh(r_s3); /* 800ABE88 lh $v0, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800ABE8C addiu $s3, $s3, 0x2 */
    r_a1 = r_s1 + 0u; /* 800ABE90 addu $a1, $s1, $zero */
    r_a2 = r_s2 & 0x7Fu; /* 800ABE94 andi $a2, $s2, 0x7F */
    r_a3 = r_s7 | 0x700u; /* 800ABE98 ori $a3, $s7, 0x700 */
    bl_sw(r_sp + 0x34u, r_v0); /* 800ABE9C sw $v0, 0x34($sp) */
    r_v1 = bl_lh(r_s3); /* 800ABEA0 lh $v1, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800ABEA4 addiu $s3, $s3, 0x2 */
    r_v0 = r_s6 & 0xFFu; /* 800ABEA8 andi $v0, $s6, 0xFF */
    r_a0 = r_v0 << 1; /* 800ABEAC sll $a0, $v0, 1 */
    r_a0 = r_a0 + r_v0; /* 800ABEB0 addu $a0, $a0, $v0 */
    r_a0 = r_a0 << 4; /* 800ABEB4 sll $a0, $a0, 4 */
    r_a0 = r_a0 + r_t0; /* 800ABEB8 addu $a0, $a0, $t0 */
    bl_sw(r_sp + 0x38u, r_v1); /* 800ABEBC sw $v1, 0x38($sp) */
    r_v0 = bl_lh(r_s3); /* 800ABEC0 lh $v0, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800ABEC4 addiu $s3, $s3, 0x2 */
    bl_sw(r_sp + 0x3Cu, r_v0); /* 800ABEC8 sw $v0, 0x3C($sp) */
    r_v0 = bl_lh(r_s3); /* 800ABECC lh $v0, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800ABED0 addiu $s3, $s3, 0x2 */
    bl_sw(r_sp + 0x40u, r_v0); /* 800ABED4 sw $v0, 0x40($sp) */
    r_v0 = bl_lh(r_s3); /* 800ABED8 lh $v0, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800ABEDC addiu $s3, $s3, 0x2 */
    bl_sw(r_sp + 0x48u, r_t6); /* 800ABEE0 sw $t6, 0x48($sp) */
    /* 800ABEE4 jal func_800A3640 */
    bl_sw(r_sp + 0x44u, r_v0);
    bl_jal(FN, 0x800A3640u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800A3640 */
    /* 800ABEEC j .L800ADEBC */
    ; goto L_800ADEBC;
L_800ABEF4:
    /* 800ABEF4 j .L800ADEBC */
    r_s3 = r_s3 + 0x1Eu; goto L_800ADEBC;
L_800ABEFC:
    r_v0 = bl_lbu(r_s4 + 0x10Eu); /* 800ABEFC lbu $v0, 0x10E($s4) */
    r_v1 = r_s6 & 0xFFu; /* 800ABF00 andi $v1, $s6, 0xFF */
    r_v0 = (u32)(r_v1 < r_v0); /* 800ABF04 sltu $v0, $v1, $v0 */
    /* 800ABF08 beqz $v0, .L800ADEBC */
    { int c_ = (r_v0 == 0); r_a0 = r_v1 << 1; if (c_) goto L_800ADEBC; }
    r_a0 = r_a0 + r_v1; /* 800ABF10 addu $a0, $a0, $v1 */
    r_v0 = bl_lw(r_s4 + 0x118u); /* 800ABF14 lw $v0, 0x118($s4) */
    r_a0 = r_a0 << 4; /* 800ABF18 sll $a0, $a0, 4 */
    /* 800ABF1C jal func_800A429C */
    r_a0 = r_a0 + r_v0;
    bl_jal(FN, 0x800A429Cu, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800A429C */
    /* 800ABF24 j .L800ADEBC */
    ; goto L_800ADEBC;
L_800ABF2C:
    r_t0 = bl_lhu(r_s3); /* 800ABF2C lhu $t0, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800ABF30 addiu $s3, $s3, 0x2 */
    r_a0 = r_s4 + 0u; /* 800ABF34 addu $a0, $s4, $zero */
    r_t8 = 0u + 0xFFFFFFFFu; /* 800ABF38 addiu $t8, $zero, -0x1 */
    r_v0 = r_s6 & 0xFFu; /* 800ABF3C andi $v0, $s6, 0xFF */
    r_a2 = r_v0 << 5; /* 800ABF40 sll $a2, $v0, 5 */
    r_a2 = r_a2 - r_v0; /* 800ABF44 subu $a2, $a2, $v0 */
    r_a1 = bl_lw(r_sp + 0x130u); /* 800ABF48 lw $a1, 0x130($sp) */
    r_a2 = r_a2 << 2; /* 800ABF4C sll $a2, $a2, 2 */
    bl_sw(r_sp + 0x138u, r_t8); /* 800ABF50 sw $t8, 0x138($sp) */
    bl_sh(r_sp + 0x9Cu, r_t0); /* 800ABF54 sh $t0, 0x9C($sp) */
    r_v1 = bl_lhu(r_s3); /* 800ABF58 lhu $v1, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800ABF5C addiu $s3, $s3, 0x2 */
    r_t0 = r_t0 >> 8; /* 800ABF60 srl $t0, $t0, 8 */
    r_v0 = bl_lw(r_s4 + 0x4u); /* 800ABF64 lw $v0, 0x4($s4) */
    r_s1 = bl_lbu(r_sp + 0x9Cu); /* 800ABF68 lbu $s1, 0x9C($sp) */
    r_a2 = r_a2 + r_v0; /* 800ABF6C addu $a2, $a2, $v0 */
    r_a3 = r_s1 + 0u; /* 800ABF70 addu $a3, $s1, $zero */
    bl_sh(r_sp + 0x9Cu, r_v1); /* 800ABF74 sh $v1, 0x9C($sp) */
    bl_sw(r_sp + 0x10u, r_t0); /* 800ABF78 sw $t0, 0x10($sp) */
    r_v0 = bl_lbu(r_sp + 0x9Cu); /* 800ABF7C lbu $v0, 0x9C($sp) */
    r_v1 = r_v1 >> 8; /* 800ABF80 srl $v1, $v1, 8 */
    bl_sw(r_sp + 0x18u, r_v1); /* 800ABF84 sw $v1, 0x18($sp) */
    bl_sw(r_sp + 0x14u, r_v0); /* 800ABF88 sw $v0, 0x14($sp) */
    r_v0 = bl_lh(r_s3); /* 800ABF8C lh $v0, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800ABF90 addiu $s3, $s3, 0x2 */
    bl_sw(r_sp + 0x1Cu, r_v0); /* 800ABF94 sw $v0, 0x1C($sp) */
    r_v0 = bl_lh(r_s3); /* 800ABF98 lh $v0, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800ABF9C addiu $s3, $s3, 0x2 */
    bl_sw(r_sp + 0x20u, r_v0); /* 800ABFA0 sw $v0, 0x20($sp) */
    r_v0 = bl_lh(r_s3); /* 800ABFA4 lh $v0, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800ABFA8 addiu $s3, $s3, 0x2 */
    bl_sw(r_sp + 0x24u, r_v0); /* 800ABFAC sw $v0, 0x24($sp) */
    r_v0 = bl_lh(r_s3); /* 800ABFB0 lh $v0, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800ABFB4 addiu $s3, $s3, 0x2 */
    bl_sw(r_sp + 0x28u, r_v0); /* 800ABFB8 sw $v0, 0x28($sp) */
    r_v0 = bl_lh(r_s3); /* 800ABFBC lh $v0, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800ABFC0 addiu $s3, $s3, 0x2 */
    bl_sw(r_sp + 0x2Cu, r_v0); /* 800ABFC4 sw $v0, 0x2C($sp) */
    r_v0 = bl_lh(r_s3); /* 800ABFC8 lh $v0, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800ABFCC addiu $s3, $s3, 0x2 */
    bl_sw(r_sp + 0x30u, r_v0); /* 800ABFD0 sw $v0, 0x30($sp) */
    r_v0 = bl_lh(r_s3); /* 800ABFD4 lh $v0, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800ABFD8 addiu $s3, $s3, 0x2 */
    /* 800ABFDC jal func_800AF678 */
    bl_sw(r_sp + 0x34u, r_v0);
    bl_jal(FN, 0x800AF678u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800AF678 */
    /* 800ABFE4 j .L800ADEBC */
    ; goto L_800ADEBC;
L_800ABFEC:
    /* 800ABFEC j .L800ADEBC */
    bl_sb(r_s4 + 0x37u, r_s6); goto L_800ADEBC;
L_800ABFF4:
    r_a1 = r_s6 & 0xFFu; /* 800ABFF4 andi $a1, $s6, 0xFF */
    r_a0 = bl_lw(r_sp + 0x148u); /* 800ABFF8 lw $a0, 0x148($sp) */
    /* 800ABFFC jal func_800AF438 */
    r_a2 = r_sp + 0x9Cu;
    bl_jal(FN, 0x800AF438u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800AF438 */
    r_v0 = r_v0 & 0xFFu; /* 800AC004 andi $v0, $v0, 0xFF */
    r_v0 = r_v0 << 2; /* 800AC008 sll $v0, $v0, 2 */
    r_t7 = 0x800D0000u; /* 800AC00C lui $t7, %hi(D_800D3368) */
    r_t7 = r_t7 + 0x3368u; /* 800AC010 addiu $t7, $t7, %lo(D_800D3368) */
    r_v0 = r_v0 + r_t7; /* 800AC014 addu $v0, $v0, $t7 */
    r_v0 = bl_lw(r_v0); /* 800AC018 lw $v0, 0x0($v0) */
    ; /* 800AC01C nop  */
    /* 800AC020 beqz $v0, .L800ADEBC */
    { int c_ = (r_v0 == 0); ; if (c_) goto L_800ADEBC; }
    /* 800AC028 j .L800ADEBC */
    r_s4 = r_v0 + 0u; goto L_800ADEBC;
L_800AC030:
    r_t8 = bl_lw(r_sp + 0x138u); /* 800AC030 lw $t8, 0x138($sp) */
    r_v0 = 0u + 0xFFFFFFFFu; /* 800AC034 addiu $v0, $zero, -0x1 */
    /* 800AC038 beq $t8, $v0, .L800AC050 */
    { int c_ = (r_t8 == r_v0); r_v0 = r_t8 & 0x100u; if (c_) goto L_800AC050; }
    /* 800AC040 beqz $v0, .L800ADEBC */
    { int c_ = (r_v0 == 0); ; if (c_) goto L_800ADEBC; }
    /* 800AC048 j .L800ADEB8 */
    r_s3 = r_s5 + 0u; goto L_800ADEB8;
L_800AC050:
    /* 800AC050 j .L800ADEB8 */
    r_s3 = r_s5 + 0u; goto L_800ADEB8;
L_800AC058:
    r_v0 = r_s6 & 0xFFu; /* 800AC058 andi $v0, $s6, 0xFF */
    bl_sh(r_s4 + 0x3Cu, r_v0); /* 800AC05C sh $v0, 0x3C($s4) */
    r_t7 = bl_lw(r_sp + 0x138u); /* 800AC060 lw $t7, 0x138($sp) */
    r_v0 = 0u + 0xFFFFFFFFu; /* 800AC064 addiu $v0, $zero, -0x1 */
    /* 800AC068 beq $t7, $v0, .L800AC080 */
    { int c_ = (r_t7 == r_v0); r_v0 = r_t7 & 0x1u; if (c_) goto L_800AC080; }
    /* 800AC070 beqz $v0, .L800ADEBC */
    { int c_ = (r_v0 == 0); ; if (c_) goto L_800ADEBC; }
    /* 800AC078 j .L800ADEB8 */
    r_s3 = r_s5 + 0u; goto L_800ADEB8;
L_800AC080:
    /* 800AC080 j .L800ADEB8 */
    r_s3 = r_s5 + 0u; goto L_800ADEB8;
L_800AC088:
    r_t8 = bl_lw(r_sp + 0x138u); /* 800AC088 lw $t8, 0x138($sp) */
    r_v0 = 0u + 0xFFFFFFFFu; /* 800AC08C addiu $v0, $zero, -0x1 */
    /* 800AC090 beq $t8, $v0, .L800AC14C */
    { int c_ = (r_t8 == r_v0); r_v0 = r_s6 & 0xFFu; if (c_) goto L_800AC14C; }
    r_v1 = bl_lhu(r_s3); /* 800AC098 lhu $v1, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800AC09C addiu $s3, $s3, 0x2 */
    r_t7 = 0u | 0xFFu; /* 800AC0A0 ori $t7, $zero, 0xFF */
    /* 800AC0A4 bne $v0, $t7, .L800AC0F0 */
    { int c_ = (r_v0 != r_t7); bl_sh(r_sp + 0x9Cu, r_v1); if (c_) goto L_800AC0F0; }
    r_v0 = r_t8 & 0x400u; /* 800AC0AC andi $v0, $t8, 0x400 */
    /* 800AC0B0 bnez $v0, .L800AC0C0 */
    { int c_ = (r_v0 != 0); ; if (c_) goto L_800AC0C0; }
    /* 800AC0B8 j .L800ADEB8 */
    r_s3 = r_s5 + 0u; goto L_800ADEB8;
L_800AC0C0:
    r_v0 = bl_lhu(r_s4 + 0x42u); /* 800AC0C0 lhu $v0, 0x42($s4) */
    ; /* 800AC0C4 nop  */
    r_v0 = r_v0 + 0x1u; /* 800AC0C8 addiu $v0, $v0, 0x1 */
    bl_sh(r_s4 + 0x42u, r_v0); /* 800AC0CC sh $v0, 0x42($s4) */
    r_v0 = r_v1 << 16; /* 800AC0D0 sll $v0, $v1, 16 */
    r_v1 = bl_lhu(r_s4 + 0x42u); /* 800AC0D4 lhu $v1, 0x42($s4) */
    r_v0 = BL_SRA(r_v0, 16); /* 800AC0D8 sra $v0, $v0, 16 */
    r_v1 = (u32)((s32)r_v1 < (s32)r_v0); /* 800AC0DC slt $v1, $v1, $v0 */
    /* 800AC0E0 beqz $v1, .L800AC144 */
    { int c_ = (r_v1 == 0); ; if (c_) goto L_800AC144; }
    /* 800AC0E8 j .L800ADEB8 */
    r_s3 = r_s5 + 0u; goto L_800ADEB8;
L_800AC0F0:
    r_v0 = r_s6 & 0xFFu; /* 800AC0F0 andi $v0, $s6, 0xFF */
    bl_sh(r_s4 + 0x3Cu, r_v0); /* 800AC0F4 sh $v0, 0x3C($s4) */
    r_t8 = bl_lw(r_sp + 0x138u); /* 800AC0F8 lw $t8, 0x138($sp) */
    ; /* 800AC0FC nop  */
    r_v0 = r_t8 & 0x4u; /* 800AC100 andi $v0, $t8, 0x4 */
    /* 800AC104 bnez $v0, .L800AC114 */
    { int c_ = (r_v0 != 0); ; if (c_) goto L_800AC114; }
    /* 800AC10C j .L800ADEB8 */
    r_s3 = r_s5 + 0u; goto L_800ADEB8;
L_800AC114:
    r_v0 = bl_lhu(r_s4 + 0x42u); /* 800AC114 lhu $v0, 0x42($s4) */
    ; /* 800AC118 nop  */
    r_v0 = r_v0 + 0x1u; /* 800AC11C addiu $v0, $v0, 0x1 */
    bl_sh(r_s4 + 0x42u, r_v0); /* 800AC120 sh $v0, 0x42($s4) */
    r_v0 = r_v1 << 16; /* 800AC124 sll $v0, $v1, 16 */
    r_v1 = bl_lhu(r_s4 + 0x42u); /* 800AC128 lhu $v1, 0x42($s4) */
    r_v0 = BL_SRA(r_v0, 16); /* 800AC12C sra $v0, $v0, 16 */
    r_v1 = (u32)((s32)r_v1 < (s32)r_v0); /* 800AC130 slt $v1, $v1, $v0 */
    /* 800AC134 beqz $v1, .L800AC144 */
    { int c_ = (r_v1 == 0); ; if (c_) goto L_800AC144; }
    /* 800AC13C j .L800ADEB8 */
    r_s3 = r_s5 + 0u; goto L_800ADEB8;
L_800AC144:
    /* 800AC144 j .L800ADEBC */
    bl_sh(r_s4 + 0x42u, 0u); goto L_800ADEBC;
L_800AC14C:
    /* 800AC14C j .L800ADEB8 */
    r_s3 = r_s5 + 0u; goto L_800ADEB8;
L_800AC154:
    r_v0 = bl_lh(r_s3); /* 800AC154 lh $v0, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800AC158 addiu $s3, $s3, 0x2 */
    r_a0 = r_s4 + 0u; /* 800AC15C addu $a0, $s4, $zero */
    r_a2 = r_s6 & 0xFFu; /* 800AC160 andi $a2, $s6, 0xFF */
    r_a1 = r_v0 << 5; /* 800AC164 sll $a1, $v0, 5 */
    r_a1 = r_a1 - r_v0; /* 800AC168 subu $a1, $a1, $v0 */
    r_v0 = bl_lw(r_s4 + 0x4u); /* 800AC16C lw $v0, 0x4($s4) */
    r_a1 = r_a1 << 2; /* 800AC170 sll $a1, $a1, 2 */
    /* 800AC174 jal func_800AFA98 */
    r_a1 = r_a1 + r_v0;
    bl_jal(FN, 0x800AFA98u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800AFA98 */
    /* 800AC17C j .L800ADEBC */
    ; goto L_800ADEBC;
L_800AC184:
    /* 800AC184 beqz $s4, .L800ADEBC */
    { int c_ = (r_s4 == 0); r_v0 = r_s6 & 0x1u; if (c_) goto L_800ADEBC; }
    /* 800AC18C j .L800ADEBC */
    bl_sb(r_s4 + 0x34u, r_v0); goto L_800ADEBC;
L_800AC194:
    r_v0 = bl_lhu(r_s3); /* 800AC194 lhu $v0, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800AC198 addiu $s3, $s3, 0x2 */
    r_a0 = r_s4 + 0u; /* 800AC19C addu $a0, $s4, $zero */
    r_a2 = r_sp + 0x9Cu; /* 800AC1A0 addiu $a2, $sp, 0x9C */
    r_t7 = r_sp + 0x108u; /* 800AC1A4 addiu $t7, $sp, 0x108 */
    r_fp = r_sp + 0xD8u; /* 800AC1A8 addiu $fp, $sp, 0xD8 */
    r_s7 = r_sp + 0xE8u; /* 800AC1AC addiu $s7, $sp, 0xE8 */
    r_s5 = r_sp + 0xF8u; /* 800AC1B0 addiu $s5, $sp, 0xF8 */
    r_s1 = r_sp + 0xC8u; /* 800AC1B4 addiu $s1, $sp, 0xC8 */
    bl_sw(r_sp + 0x190u, r_t7); /* 800AC1B8 sw $t7, 0x190($sp) */
    bl_sh(r_sp + 0x9Cu, r_v0); /* 800AC1BC sh $v0, 0x9C($sp) */
    r_a1 = bl_lbu(r_sp + 0x9Cu); /* 800AC1C0 lbu $a1, 0x9C($sp) */
    /* 800AC1C4 jal func_800AF438 */
    r_s2 = r_v0 >> 8;
    bl_jal(FN, 0x800AF438u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800AF438 */
    r_t8 = bl_lhu(r_s3); /* 800AC1CC lhu $t8, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800AC1D0 addiu $s3, $s3, 0x2 */
    bl_sh(r_sp + 0x178u, r_t8); /* 800AC1D4 sh $t8, 0x178($sp) */
    r_t7 = bl_lhu(r_s3); /* 800AC1D8 lhu $t7, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800AC1DC addiu $s3, $s3, 0x2 */
    bl_sh(r_sp + 0x180u, r_t7); /* 800AC1E0 sh $t7, 0x180($sp) */
    r_t8 = bl_lhu(r_s3); /* 800AC1E4 lhu $t8, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800AC1E8 addiu $s3, $s3, 0x2 */
    bl_sw(r_sp + 0x98u, 0u); /* 800AC1EC sw $zero, 0x98($sp) */
    bl_sh(r_sp + 0x188u, r_t8); /* 800AC1F0 sh $t8, 0x188($sp) */
L_800AC1F4:
    r_v0 = bl_lh(r_sp + 0x9Cu); /* 800AC1F4 lh $v0, 0x9C($sp) */
    r_v1 = bl_lw(r_sp + 0x98u); /* 800AC1F8 lw $v1, 0x98($sp) */
    ; /* 800AC1FC nop  */
    r_v0 = BL_SRA(r_v0, r_v1); /* 800AC200 srav $v0, $v0, $v1 */
    r_v0 = r_v0 & 0x1u; /* 800AC204 andi $v0, $v0, 0x1 */
    /* 800AC208 beqz $v0, .L800AC484 */
    { int c_ = (r_v0 == 0); r_v0 = r_v1 << 2; if (c_) goto L_800AC484; }
    r_t7 = 0x800D0000u; /* 800AC210 lui $t7, %hi(D_800D3368) */
    r_t7 = r_t7 + 0x3368u; /* 800AC214 addiu $t7, $t7, %lo(D_800D3368) */
    r_v1 = r_v0 + r_t7; /* 800AC218 addu $v1, $v0, $t7 */
    r_a0 = bl_lw(r_v1); /* 800AC21C lw $a0, 0x0($v1) */
    ; /* 800AC220 nop  */
    /* 800AC224 beqz $a0, .L800AC484 */
    { int c_ = (r_a0 == 0); r_v0 = r_s2 & 0xFFu; if (c_) goto L_800AC484; }
    bl_sh(r_a0 + 0x5Eu, r_v0); /* 800AC22C sh $v0, 0x5E($a0) */
    r_v1 = bl_lw(r_v1); /* 800AC230 lw $v1, 0x0($v1) */
    r_v0 = bl_lbu(r_s4 + 0x20u); /* 800AC234 lbu $v0, 0x20($s4) */
    ; /* 800AC238 nop  */
    bl_sb(r_v1 + 0x5Cu, r_v0); /* 800AC23C sb $v0, 0x5C($v1) */
    r_v0 = bl_lw(r_sp + 0x98u); /* 800AC240 lw $v0, 0x98($sp) */
    ; /* 800AC244 nop  */
    r_v0 = r_v0 << 2; /* 800AC248 sll $v0, $v0, 2 */
    r_v0 = r_v0 + r_t7; /* 800AC24C addu $v0, $v0, $t7 */
    r_v1 = bl_lw(r_v0); /* 800AC250 lw $v1, 0x0($v0) */
    r_v0 = r_s6 & 0x2u; /* 800AC254 andi $v0, $s6, 0x2 */
    bl_sb(r_v1 + 0x5Du, r_v0); /* 800AC258 sb $v0, 0x5D($v1) */
    r_v0 = bl_lw(r_sp + 0x98u); /* 800AC25C lw $v0, 0x98($sp) */
    ; /* 800AC260 nop  */
    r_v0 = r_v0 << 2; /* 800AC264 sll $v0, $v0, 2 */
    r_v0 = r_v0 + r_t7; /* 800AC268 addu $v0, $v0, $t7 */
    r_v0 = bl_lw(r_v0); /* 800AC26C lw $v0, 0x0($v0) */
    r_t8 = 0u | 0x1u; /* 800AC270 ori $t8, $zero, 0x1 */
    bl_sb(r_v0 + 0x36u, r_t8); /* 800AC274 sb $t8, 0x36($v0) */
    r_v0 = r_s6 & 0x1u; /* 800AC278 andi $v0, $s6, 0x1 */
    /* 800AC27C beqz $v0, .L800AC440 */
    { int c_ = (r_v0 == 0); r_v0 = r_s2 & 0xFFu; if (c_) goto L_800AC440; }
    r_s0 = r_v0 << 5; /* 800AC284 sll $s0, $v0, 5 */
    r_s0 = r_s0 - r_v0; /* 800AC288 subu $s0, $s0, $v0 */
    r_v0 = bl_lw(r_s4 + 0x4u); /* 800AC28C lw $v0, 0x4($s4) */
    r_s0 = r_s0 << 2; /* 800AC290 sll $s0, $s0, 2 */
    r_s0 = r_s0 + r_v0; /* 800AC294 addu $s0, $s0, $v0 */
    /* 800AC298 jal SetRotMatrix */
    r_a0 = r_s0 + 0x2Cu;
    bl_jal(FN, 0x80049EFCu, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* SetRotMatrix */
    r_a0 = r_sp + 0xA8u; /* 800AC2A0 addiu $a0, $sp, 0xA8 */
    bl_sw(r_sp + 0xBCu, 0u); /* 800AC2A4 sw $zero, 0xBC($sp) */
    bl_sw(r_sp + 0xC0u, 0u); /* 800AC2A8 sw $zero, 0xC0($sp) */
    /* 800AC2AC jal SetTransMatrix */
    bl_sw(r_sp + 0xC4u, 0u);
    bl_jal(FN, 0x80049F8Cu, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* SetTransMatrix */
    r_t8 = bl_lw(r_sp + 0x190u); /* 800AC2B4 lw $t8, 0x190($sp) */
    r_t7 = 0u | 0x1000u; /* 800AC2B8 ori $t7, $zero, 0x1000 */
    bl_sh(r_sp + 0x108u, r_t7); /* 800AC2BC sh $t7, 0x108($sp) */
    bl_sh(r_sp + 0x10Au, 0u); /* 800AC2C0 sh $zero, 0x10A($sp) */
    bl_sh(r_sp + 0x10Cu, 0u); /* 800AC2C4 sh $zero, 0x10C($sp) */
    MTC2(bl_lw(r_t8), 0); /* 800AC2C8 lwc2 $0, 0x0($t8) */
    MTC2(bl_lw(r_t8 + 0x4u), 1); /* 800AC2CC lwc2 $1, 0x4($t8) */
    ; /* 800AC2D0 nop  */
    ; /* 800AC2D4 nop  */
    doCOP2((int)0x4A480012u); /* MVMVA 1, 0, 0, 0, 0 */ /* 800AC2D8 MVMVA 1, 0, 0, 0, 0 */
    bl_sw(r_fp, (u32)MFC2(25)); /* 800AC2DC swc2 $25, 0x0($fp) */
    bl_sw(r_fp + 0x4u, (u32)MFC2(26)); /* 800AC2E0 swc2 $26, 0x4($fp) */
    bl_sw(r_fp + 0x8u, (u32)MFC2(27)); /* 800AC2E4 swc2 $27, 0x8($fp) */
    bl_sh(r_sp + 0x108u, 0u); /* 800AC2E8 sh $zero, 0x108($sp) */
    bl_sh(r_sp + 0x10Au, r_t7); /* 800AC2EC sh $t7, 0x10A($sp) */
    MTC2(bl_lw(r_t8), 0); /* 800AC2F0 lwc2 $0, 0x0($t8) */
    MTC2(bl_lw(r_t8 + 0x4u), 1); /* 800AC2F4 lwc2 $1, 0x4($t8) */
    ; /* 800AC2F8 nop  */
    ; /* 800AC2FC nop  */
    doCOP2((int)0x4A480012u); /* MVMVA 1, 0, 0, 0, 0 */ /* 800AC300 MVMVA 1, 0, 0, 0, 0 */
    bl_sw(r_s7, (u32)MFC2(25)); /* 800AC304 swc2 $25, 0x0($s7) */
    bl_sw(r_s7 + 0x4u, (u32)MFC2(26)); /* 800AC308 swc2 $26, 0x4($s7) */
    bl_sw(r_s7 + 0x8u, (u32)MFC2(27)); /* 800AC30C swc2 $27, 0x8($s7) */
    r_t7 = bl_lw(r_sp + 0x190u); /* 800AC310 lw $t7, 0x190($sp) */
    r_t8 = 0u | 0x1000u; /* 800AC314 ori $t8, $zero, 0x1000 */
    bl_sh(r_sp + 0x10Au, 0u); /* 800AC318 sh $zero, 0x10A($sp) */
    bl_sh(r_sp + 0x10Cu, r_t8); /* 800AC31C sh $t8, 0x10C($sp) */
    MTC2(bl_lw(r_t7), 0); /* 800AC320 lwc2 $0, 0x0($t7) */
    MTC2(bl_lw(r_t7 + 0x4u), 1); /* 800AC324 lwc2 $1, 0x4($t7) */
    ; /* 800AC328 nop  */
    ; /* 800AC32C nop  */
    doCOP2((int)0x4A480012u); /* MVMVA 1, 0, 0, 0, 0 */ /* 800AC330 MVMVA 1, 0, 0, 0, 0 */
    bl_sw(r_s5, (u32)MFC2(25)); /* 800AC334 swc2 $25, 0x0($s5) */
    bl_sw(r_s5 + 0x4u, (u32)MFC2(26)); /* 800AC338 swc2 $26, 0x4($s5) */
    bl_sw(r_s5 + 0x8u, (u32)MFC2(27)); /* 800AC33C swc2 $27, 0x8($s5) */
    r_a0 = bl_lw(r_sp + 0x98u); /* 800AC340 lw $a0, 0x98($sp) */
    r_t8 = 0x800D0000u; /* 800AC344 lui $t8, %hi(D_800D3368) */
    r_t8 = r_t8 + 0x3368u; /* 800AC348 addiu $t8, $t8, %lo(D_800D3368) */
    r_a0 = r_a0 << 2; /* 800AC34C sll $a0, $a0, 2 */
    r_a0 = r_a0 + r_t8; /* 800AC350 addu $a0, $a0, $t8 */
    r_v0 = bl_lw(r_a0); /* 800AC354 lw $v0, 0x0($a0) */
    ; /* 800AC358 nop  */
    r_v0 = bl_lw(r_v0 + 0x4u); /* 800AC35C lw $v0, 0x4($v0) */
    r_v1 = bl_lw(r_s0 + 0x40u); /* 800AC360 lw $v1, 0x40($s0) */
    r_v0 = bl_lw(r_v0 + 0x5Cu); /* 800AC364 lw $v0, 0x5C($v0) */
    ; /* 800AC368 nop  */
    r_v0 = r_v0 - r_v1; /* 800AC36C subu $v0, $v0, $v1 */
    bl_sw(r_sp + 0xC8u, r_v0); /* 800AC370 sw $v0, 0xC8($sp) */
    r_v0 = bl_lw(r_a0); /* 800AC374 lw $v0, 0x0($a0) */
    ; /* 800AC378 nop  */
    r_v0 = bl_lw(r_v0 + 0x4u); /* 800AC37C lw $v0, 0x4($v0) */
    r_v1 = bl_lw(r_s0 + 0x44u); /* 800AC380 lw $v1, 0x44($s0) */
    r_v0 = bl_lw(r_v0 + 0x60u); /* 800AC384 lw $v0, 0x60($v0) */
    ; /* 800AC388 nop  */
    r_v0 = r_v0 - r_v1; /* 800AC38C subu $v0, $v0, $v1 */
    bl_sw(r_sp + 0xCCu, r_v0); /* 800AC390 sw $v0, 0xCC($sp) */
    r_v0 = bl_lw(r_a0); /* 800AC394 lw $v0, 0x0($a0) */
    r_a1 = r_s7 + 0u; /* 800AC398 addu $a1, $s7, $zero */
    r_v0 = bl_lw(r_v0 + 0x4u); /* 800AC39C lw $v0, 0x4($v0) */
    r_v1 = bl_lw(r_s0 + 0x48u); /* 800AC3A0 lw $v1, 0x48($s0) */
    r_v0 = bl_lw(r_v0 + 0x64u); /* 800AC3A4 lw $v0, 0x64($v0) */
    r_a2 = r_s5 + 0u; /* 800AC3A8 addu $a2, $s5, $zero */
    r_v0 = r_v0 - r_v1; /* 800AC3AC subu $v0, $v0, $v1 */
    bl_sw(r_sp + 0xD0u, r_v0); /* 800AC3B0 sw $v0, 0xD0($sp) */
    r_a3 = bl_lh(r_s4 + 0x1Cu); /* 800AC3B4 lh $a3, 0x1C($s4) */
    /* 800AC3B8 jal func_800AF2C4 */
    r_a0 = r_s1 + 0u;
    bl_jal(FN, 0x800AF2C4u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800AF2C4 */
    r_a0 = r_s1 + 0u; /* 800AC3C0 addu $a0, $s1, $zero */
    r_v1 = bl_lw(r_sp + 0x98u); /* 800AC3C4 lw $v1, 0x98($sp) */
    r_t7 = 0x800D0000u; /* 800AC3C8 lui $t7, %hi(D_800D3368) */
    r_t7 = r_t7 + 0x3368u; /* 800AC3CC addiu $t7, $t7, %lo(D_800D3368) */
    r_v1 = r_v1 << 2; /* 800AC3D0 sll $v1, $v1, 2 */
    r_v1 = r_v1 + r_t7; /* 800AC3D4 addu $v1, $v1, $t7 */
    r_v1 = bl_lw(r_v1); /* 800AC3D8 lw $v1, 0x0($v1) */
    r_a1 = r_s5 + 0u; /* 800AC3DC addu $a1, $s5, $zero */
    bl_sh(r_v1 + 0x6Au, r_v0); /* 800AC3E0 sh $v0, 0x6A($v1) */
    r_a3 = bl_lh(r_s4 + 0x1Cu); /* 800AC3E4 lh $a3, 0x1C($s4) */
    /* 800AC3E8 jal func_800AF2C4 */
    r_a2 = r_fp + 0u;
    bl_jal(FN, 0x800AF2C4u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800AF2C4 */
    r_a0 = r_s1 + 0u; /* 800AC3F0 addu $a0, $s1, $zero */
    r_v1 = bl_lw(r_sp + 0x98u); /* 800AC3F4 lw $v1, 0x98($sp) */
    r_t8 = 0x800D0000u; /* 800AC3F8 lui $t8, %hi(D_800D3368) */
    r_t8 = r_t8 + 0x3368u; /* 800AC3FC addiu $t8, $t8, %lo(D_800D3368) */
    r_v1 = r_v1 << 2; /* 800AC400 sll $v1, $v1, 2 */
    r_v1 = r_v1 + r_t8; /* 800AC404 addu $v1, $v1, $t8 */
    r_v1 = bl_lw(r_v1); /* 800AC408 lw $v1, 0x0($v1) */
    r_a1 = r_fp + 0u; /* 800AC40C addu $a1, $fp, $zero */
    bl_sh(r_v1 + 0x6Cu, r_v0); /* 800AC410 sh $v0, 0x6C($v1) */
    r_a3 = bl_lh(r_s4 + 0x1Cu); /* 800AC414 lh $a3, 0x1C($s4) */
    /* 800AC418 jal func_800AF2C4 */
    r_a2 = r_s7 + 0u;
    bl_jal(FN, 0x800AF2C4u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800AF2C4 */
    r_v1 = bl_lw(r_sp + 0x98u); /* 800AC420 lw $v1, 0x98($sp) */
    r_t7 = 0x800D0000u; /* 800AC424 lui $t7, %hi(D_800D3368) */
    r_t7 = r_t7 + 0x3368u; /* 800AC428 addiu $t7, $t7, %lo(D_800D3368) */
    r_v1 = r_v1 << 2; /* 800AC42C sll $v1, $v1, 2 */
    r_v1 = r_v1 + r_t7; /* 800AC430 addu $v1, $v1, $t7 */
    r_v1 = bl_lw(r_v1); /* 800AC434 lw $v1, 0x0($v1) */
    /* 800AC438 j .L800AC484 */
    bl_sh(r_v1 + 0x6Eu, r_v0); goto L_800AC484;
L_800AC440:
    r_t8 = 0x800D0000u; /* 800AC440 lui $t8, %hi(D_800D3368) */
    r_t8 = r_t8 + 0x3368u; /* 800AC444 addiu $t8, $t8, %lo(D_800D3368) */
    r_v0 = bl_lw(r_sp + 0x98u); /* 800AC448 lw $v0, 0x98($sp) */
    r_t7 = bl_lhu(r_sp + 0x178u); /* 800AC44C lhu $t7, 0x178($sp) */
    r_v0 = r_v0 << 2; /* 800AC450 sll $v0, $v0, 2 */
    r_v0 = r_v0 + r_t8; /* 800AC454 addu $v0, $v0, $t8 */
    r_v1 = bl_lw(r_v0); /* 800AC458 lw $v1, 0x0($v0) */
    ; /* 800AC45C nop  */
    bl_sh(r_v1 + 0x6Au, r_t7); /* 800AC460 sh $t7, 0x6A($v1) */
    r_v1 = bl_lw(r_v0); /* 800AC464 lw $v1, 0x0($v0) */
    r_t8 = bl_lhu(r_sp + 0x180u); /* 800AC468 lhu $t8, 0x180($sp) */
    ; /* 800AC46C nop  */
    bl_sh(r_v1 + 0x6Cu, r_t8); /* 800AC470 sh $t8, 0x6C($v1) */
    r_v0 = bl_lw(r_v0); /* 800AC474 lw $v0, 0x0($v0) */
    r_t7 = bl_lhu(r_sp + 0x188u); /* 800AC478 lhu $t7, 0x188($sp) */
    ; /* 800AC47C nop  */
    bl_sh(r_v0 + 0x6Eu, r_t7); /* 800AC480 sh $t7, 0x6E($v0) */
L_800AC484:
    r_v0 = bl_lw(r_sp + 0x98u); /* 800AC484 lw $v0, 0x98($sp) */
    ; /* 800AC488 nop  */
    r_v0 = r_v0 + 0x1u; /* 800AC48C addiu $v0, $v0, 0x1 */
    bl_sw(r_sp + 0x98u, r_v0); /* 800AC490 sw $v0, 0x98($sp) */
    r_v0 = (u32)((s32)r_v0 < 13); /* 800AC494 slti $v0, $v0, 0xD */
    /* 800AC498 bnez $v0, .L800AC1F4 */
    { int c_ = (r_v0 != 0); ; if (c_) goto L_800AC1F4; }
    /* 800AC4A0 j .L800ADEBC */
    ; goto L_800ADEBC;
L_800AC4A8:
    r_a0 = r_s4 + 0u; /* 800AC4A8 addu $a0, $s4, $zero */
    r_a1 = r_s6 & 0xFFu; /* 800AC4AC andi $a1, $s6, 0xFF */
    /* 800AC4B0 jal func_800AF438 */
    r_a2 = r_sp + 0x9Cu;
    bl_jal(FN, 0x800AF438u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800AF438 */
    bl_sw(r_sp + 0x98u, 0u); /* 800AC4B8 sw $zero, 0x98($sp) */
L_800AC4BC:
    r_v0 = bl_lh(r_sp + 0x9Cu); /* 800AC4BC lh $v0, 0x9C($sp) */
    r_v1 = bl_lw(r_sp + 0x98u); /* 800AC4C0 lw $v1, 0x98($sp) */
    ; /* 800AC4C4 nop  */
    r_v0 = BL_SRA(r_v0, r_v1); /* 800AC4C8 srav $v0, $v0, $v1 */
    r_v0 = r_v0 & 0x1u; /* 800AC4CC andi $v0, $v0, 0x1 */
    /* 800AC4D0 beqz $v0, .L800AC4FC */
    { int c_ = (r_v0 == 0); r_v0 = r_v1 << 2; if (c_) goto L_800AC4FC; }
    r_t8 = 0x800D0000u; /* 800AC4D8 lui $t8, %hi(D_800D3368) */
    r_t8 = r_t8 + 0x3368u; /* 800AC4DC addiu $t8, $t8, %lo(D_800D3368) */
    r_v0 = r_v0 + r_t8; /* 800AC4E0 addu $v0, $v0, $t8 */
    r_v0 = bl_lw(r_v0); /* 800AC4E4 lw $v0, 0x0($v0) */
    ; /* 800AC4E8 nop  */
    /* 800AC4EC beqz $v0, .L800AC4FC */
    { int c_ = (r_v0 == 0); ; if (c_) goto L_800AC4FC; }
    r_t7 = 0u | 0xFFu; /* 800AC4F4 ori $t7, $zero, 0xFF */
    bl_sb(r_v0 + 0x5Cu, r_t7); /* 800AC4F8 sb $t7, 0x5C($v0) */
L_800AC4FC:
    r_v0 = bl_lw(r_sp + 0x98u); /* 800AC4FC lw $v0, 0x98($sp) */
    ; /* 800AC500 nop  */
    r_v0 = r_v0 + 0x1u; /* 800AC504 addiu $v0, $v0, 0x1 */
    bl_sw(r_sp + 0x98u, r_v0); /* 800AC508 sw $v0, 0x98($sp) */
    r_v0 = (u32)((s32)r_v0 < 13); /* 800AC50C slti $v0, $v0, 0xD */
    /* 800AC510 bnez $v0, .L800AC4BC */
    { int c_ = (r_v0 != 0); ; if (c_) goto L_800AC4BC; }
    /* 800AC518 j .L800ADEBC */
    ; goto L_800ADEBC;
L_800AC520:
    r_v0 = bl_lbu(r_s4 + 0x37u); /* 800AC520 lbu $v0, 0x37($s4) */
    ; /* 800AC524 nop  */
    /* 800AC528 beqz $v0, .L800AC548 */
    { int c_ = (r_v0 == 0); ; if (c_) goto L_800AC548; }
    r_a0 = bl_lw(r_s4 + 0x4u); /* 800AC530 lw $a0, 0x4($s4) */
    r_a1 = bl_lh(r_s4 + 0x1Cu); /* 800AC534 lh $a1, 0x1C($s4) */
    /* 800AC538 jal func_8009F1C4 */
    ;
    bl_jal(FN, 0x8009F1C4u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_8009F1C4 */
    /* 800AC540 j .L800ADEBC */
    ; goto L_800ADEBC;
L_800AC548:
    r_a0 = bl_lw(r_s4 + 0x4u); /* 800AC548 lw $a0, 0x4($s4) */
    r_a1 = bl_lh(r_s4 + 0x1Cu); /* 800AC54C lh $a1, 0x1C($s4) */
    /* 800AC550 jal func_8009EF3C */
    ;
    bl_jal(FN, 0x8009EF3Cu, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_8009EF3C */
    /* 800AC558 j .L800ADEBC */
    ; goto L_800ADEBC;
L_800AC560:
    /* 800AC560 jal func_800AEEF8 */
    r_a0 = r_s4 + 0u;
    bl_jal(FN, 0x800AEEF8u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800AEEF8 */
    r_v1 = bl_lh(r_s4 + 0x8Eu); /* 800AC568 lh $v1, 0x8E($s4) */
    ; /* 800AC56C nop  */
    /* 800AC570 bnez $v1, .L800AC580 */
    { int c_ = (r_v1 != 0); r_a2 = r_v0 + 0u; if (c_) goto L_800AC580; }
    r_t8 = 0u | 0x1u; /* 800AC578 ori $t8, $zero, 0x1 */
    bl_sh(r_s4 + 0x8Eu, r_t8); /* 800AC57C sh $t8, 0x8E($s4) */
L_800AC580:
    r_v0 = bl_lhu(r_s3); /* 800AC580 lhu $v0, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800AC584 addiu $s3, $s3, 0x2 */
    bl_sh(r_sp + 0x9Cu, r_v0); /* 800AC588 sh $v0, 0x9C($sp) */
    r_v1 = bl_lbu(r_sp + 0x9Cu); /* 800AC58C lbu $v1, 0x9C($sp) */
    r_s2 = r_v0 >> 8; /* 800AC590 srl $s2, $v0, 8 */
    r_v0 = r_v1 << 5; /* 800AC594 sll $v0, $v1, 5 */
    r_v0 = r_v0 - r_v1; /* 800AC598 subu $v0, $v0, $v1 */
    r_v1 = bl_lw(r_s4 + 0x4u); /* 800AC59C lw $v1, 0x4($s4) */
    r_v0 = r_v0 << 2; /* 800AC5A0 sll $v0, $v0, 2 */
    r_a0 = r_v0 + r_v1; /* 800AC5A4 addu $a0, $v0, $v1 */
    r_v0 = bl_lh(r_s4 + 0x8Eu); /* 800AC5A8 lh $v0, 0x8E($s4) */
    r_v1 = bl_lw(r_a0 + 0x70u); /* 800AC5AC lw $v1, 0x70($a0) */
    BL_DIV(r_a2, r_v0); /* 800AC5B0 div $zero, $a2, $v0 */
    /* 800AC5B4 bnez $v0, .L800AC5C0 */
    { int c_ = (r_v0 != 0); ; if (c_) goto L_800AC5C0; }
    bl_fail(FN, "BREAK", 7u); /* 800AC5BC break 7 */
L_800AC5C0:
    r_at = 0u + 0xFFFFFFFFu; /* 800AC5C0 addiu $at, $zero, -0x1 */
    /* 800AC5C4 bne $v0, $at, .L800AC5D8 */
    { int c_ = (r_v0 != r_at); r_at = 0x80000000u; if (c_) goto L_800AC5D8; }
    /* 800AC5CC bne $a2, $at, .L800AC5D8 */
    { int c_ = (r_a2 != r_at); ; if (c_) goto L_800AC5D8; }
    bl_fail(FN, "BREAK", 6u); /* 800AC5D4 break 6 */
L_800AC5D8:
    r_a2 = lo; /* 800AC5D8 mflo $a2 */
    /* 800AC5DC beqz $v1, .L800AC5F8 */
    { int c_ = (r_v1 == 0); r_a1 = 0u + 0u; if (c_) goto L_800AC5F8; }
    r_v0 = bl_lh(r_v1 + 0x10u); /* 800AC5E4 lh $v0, 0x10($v1) */
    ; /* 800AC5E8 nop  */
    r_v0 = r_s2 ^ r_v0; /* 800AC5EC xor $v0, $s2, $v0 */
    /* 800AC5F0 j .L800AC61C */
    r_a1 = (u32)(r_v0 < 0x1u); goto L_800AC61C;
L_800AC5F8:
    r_v0 = bl_lw(r_a0 + 0x74u); /* 800AC5F8 lw $v0, 0x74($a0) */
    ; /* 800AC5FC nop  */
    /* 800AC600 beqz $v0, .L800AC61C */
    { int c_ = (r_v0 == 0); ; if (c_) goto L_800AC61C; }
    r_v1 = bl_lh(r_v0 + 0x10u); /* 800AC608 lh $v1, 0x10($v0) */
    r_v0 = r_s2 & 0xFFu; /* 800AC60C andi $v0, $s2, 0xFF */
    /* 800AC610 bne $v1, $v0, .L800AC61C */
    { int c_ = (r_v1 != r_v0); ; if (c_) goto L_800AC61C; }
    r_a1 = 0u | 0x1u; /* 800AC618 ori $a1, $zero, 0x1 */
L_800AC61C:
    /* 800AC61C beqz $a1, .L800AC658 */
    { int c_ = (r_a1 == 0); r_v1 = r_fp & 0xFFu; if (c_) goto L_800AC658; }
    r_v0 = 0u | 0x28u; /* 800AC624 ori $v0, $zero, 0x28 */
    /* 800AC628 bne $v1, $v0, .L800AC644 */
    { int c_ = (r_v1 != r_v0); r_v0 = r_s6 & 0xFFu; if (c_) goto L_800AC644; }
    r_v0 = (u32)((s32)r_a2 < (s32)r_v0); /* 800AC630 slt $v0, $a2, $v0 */
    /* 800AC634 bnez $v0, .L800ADEBC */
    { int c_ = (r_v0 != 0); ; if (c_) goto L_800ADEBC; }
    /* 800AC63C j .L800ADEB8 */
    r_s3 = r_s5 + 0u; goto L_800ADEB8;
L_800AC644:
    r_v0 = (u32)((s32)r_a2 < (s32)r_v0); /* 800AC644 slt $v0, $a2, $v0 */
    /* 800AC648 beqz $v0, .L800ADEBC */
    { int c_ = (r_v0 == 0); ; if (c_) goto L_800ADEBC; }
    /* 800AC650 j .L800ADEB8 */
    r_s3 = r_s5 + 0u; goto L_800ADEB8;
L_800AC658:
    /* 800AC658 j .L800ADEB8 */
    r_s3 = r_s5 + 0u; goto L_800ADEB8;
L_800AC660:
    /* 800AC660 jal func_800AEEF8 */
    r_a0 = r_s4 + 0u;
    bl_jal(FN, 0x800AEEF8u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800AEEF8 */
    r_v1 = bl_lh(r_s4 + 0x8Eu); /* 800AC668 lh $v1, 0x8E($s4) */
    ; /* 800AC66C nop  */
    r_v0 = (u32)((s32)r_v0 < (s32)r_v1); /* 800AC670 slt $v0, $v0, $v1 */
    /* 800AC674 bnez $v0, .L800ADEBC */
    { int c_ = (r_v0 != 0); ; if (c_) goto L_800ADEBC; }
    /* 800AC67C j .L800ADEB8 */
    r_s3 = r_s5 + 0u; goto L_800ADEB8;
L_800AC684:
    /* 800AC684 jal func_800AEEF8 */
    r_a0 = r_s4 + 0u;
    bl_jal(FN, 0x800AEEF8u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800AEEF8 */
    r_v1 = bl_lh(r_s4 + 0x8Eu); /* 800AC68C lh $v1, 0x8E($s4) */
    ; /* 800AC690 nop  */
    r_v1 = (u32)((s32)r_v1 < (s32)r_v0); /* 800AC694 slt $v1, $v1, $v0 */
    /* 800AC698 bnez $v1, .L800ADEBC */
    { int c_ = (r_v1 != 0); ; if (c_) goto L_800ADEBC; }
    /* 800AC6A0 j .L800ADEB8 */
    r_s3 = r_s5 + 0u; goto L_800ADEB8;
L_800AC6A8:
    r_a0 = bl_lw(r_s4 + 0x4u); /* 800AC6A8 lw $a0, 0x4($s4) */
    r_v0 = bl_lh(r_s3); /* 800AC6AC lh $v0, 0x0($s3) */
    r_v1 = bl_lw(r_a0 + 0x5Cu); /* 800AC6B0 lw $v1, 0x5C($a0) */
    ; /* 800AC6B4 nop  */
    r_v0 = r_v0 - r_v1; /* 800AC6B8 subu $v0, $v0, $v1 */
    BL_MULT(r_v0, r_v0); /* 800AC6BC mult $v0, $v0 */
    r_s3 = r_s3 + 0x2u; /* 800AC6C0 addiu $s3, $s3, 0x2 */
    r_v0 = bl_lh(r_s3); /* 800AC6C4 lh $v0, 0x0($s3) */
    r_v1 = bl_lw(r_a0 + 0x60u); /* 800AC6C8 lw $v1, 0x60($a0) */
    r_a1 = lo; /* 800AC6CC mflo $a1 */
    r_v0 = r_v0 - r_v1; /* 800AC6D0 subu $v0, $v0, $v1 */
    ; /* 800AC6D4 nop  */
    BL_MULT(r_v0, r_v0); /* 800AC6D8 mult $v0, $v0 */
    r_s3 = r_s3 + 0x2u; /* 800AC6DC addiu $s3, $s3, 0x2 */
    r_v1 = bl_lw(r_a0 + 0x64u); /* 800AC6E0 lw $v1, 0x64($a0) */
    r_v0 = bl_lh(r_s3); /* 800AC6E4 lh $v0, 0x0($s3) */
    r_a0 = lo; /* 800AC6E8 mflo $a0 */
    r_v0 = r_v0 - r_v1; /* 800AC6EC subu $v0, $v0, $v1 */
    ; /* 800AC6F0 nop  */
    BL_MULT(r_v0, r_v0); /* 800AC6F4 mult $v0, $v0 */
    r_s3 = r_s3 + 0x2u; /* 800AC6F8 addiu $s3, $s3, 0x2 */
    r_a1 = r_a1 + r_a0; /* 800AC6FC addu $a1, $a1, $a0 */
    r_a0 = lo; /* 800AC700 mflo $a0 */
    /* 800AC704 jal SquareRoot0 */
    r_a0 = r_a1 + r_a0;
    bl_jal(FN, 0x80048C4Cu, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* SquareRoot0 */
    r_a0 = r_v0 + 0u; /* 800AC70C addu $a0, $v0, $zero */
    r_v1 = r_fp & 0xFFu; /* 800AC710 andi $v1, $fp, 0xFF */
    r_v0 = 0u | 0x2Cu; /* 800AC714 ori $v0, $zero, 0x2C */
    /* 800AC718 bne $v1, $v0, .L800AC73C */
    { int c_ = (r_v1 != r_v0); ; if (c_) goto L_800AC73C; }
    r_v0 = bl_lh(r_s4 + 0x8Eu); /* 800AC720 lh $v0, 0x8E($s4) */
    ; /* 800AC724 nop  */
    r_v0 = (u32)((s32)r_a0 < (s32)r_v0); /* 800AC728 slt $v0, $a0, $v0 */
    /* 800AC72C bnez $v0, .L800ADEBC */
    { int c_ = (r_v0 != 0); ; if (c_) goto L_800ADEBC; }
    /* 800AC734 j .L800ADEB8 */
    r_s3 = r_s5 + 0u; goto L_800ADEB8;
L_800AC73C:
    r_v0 = bl_lh(r_s4 + 0x8Eu); /* 800AC73C lh $v0, 0x8E($s4) */
    ; /* 800AC740 nop  */
    r_v0 = (u32)((s32)r_v0 < (s32)r_a0); /* 800AC744 slt $v0, $v0, $a0 */
    /* 800AC748 bnez $v0, .L800ADEBC */
    { int c_ = (r_v0 != 0); ; if (c_) goto L_800ADEBC; }
    /* 800AC750 j .L800ADEB8 */
    r_s3 = r_s5 + 0u; goto L_800ADEB8;
L_800AC758:
    r_v0 = bl_lhu(r_s4 + 0x8Eu); /* 800AC758 lhu $v0, 0x8E($s4) */
    ; /* 800AC75C nop  */
    bl_sh(r_s4 + 0x48u, r_v0); /* 800AC760 sh $v0, 0x48($s4) */
    r_v1 = bl_lhu(r_s3); /* 800AC764 lhu $v1, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800AC768 addiu $s3, $s3, 0x2 */
    r_v0 = r_s6 & 0xFFu; /* 800AC76C andi $v0, $s6, 0xFF */
    /* 800AC770 beqz $v0, .L800AC78C */
    { int c_ = (r_v0 == 0); bl_sh(r_sp + 0x9Cu, r_v1); if (c_) goto L_800AC78C; }
    r_v0 = r_v1 << 16; /* 800AC778 sll $v0, $v1, 16 */
    r_v0 = BL_SRA(r_v0, 16); /* 800AC77C sra $v0, $v0, 16 */
    r_v0 = r_s5 + r_v0; /* 800AC780 addu $v0, $s5, $v0 */
    /* 800AC784 j .L800ADEBC */
    bl_sw(r_s4 + 0x4Cu, r_v0); goto L_800ADEBC;
L_800AC78C:
    /* 800AC78C j .L800ADEBC */
    bl_sw(r_s4 + 0x4Cu, 0u); goto L_800ADEBC;
L_800AC794:
    /* 800AC794 jal func_800BF6F8 */
    ;
    bl_jal(FN, 0x800BF6F8u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800BF6F8 */
    /* 800AC79C beqz $v0, .L800ADEBC */
    { int c_ = (r_v0 == 0); ; if (c_) goto L_800ADEBC; }
    /* 800AC7A4 j .L800ADEB8 */
    r_s3 = r_s5 + 0u; goto L_800ADEB8;
L_800AC7AC:
    bl_sh(r_s3, 0u); /* 800AC7AC sh $zero, 0x0($s3) */
    /* 800AC7B0 j .L800ADEBC */
    r_s3 = r_s3 + 0x2u; goto L_800ADEBC;
L_800AC7B8:
    r_v1 = bl_lhu(r_s3); /* 800AC7B8 lhu $v1, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800AC7BC addiu $s3, $s3, 0x2 */
    r_v0 = r_v1 << 16; /* 800AC7C0 sll $v0, $v1, 16 */
    r_v0 = BL_SRA(r_v0, 16); /* 800AC7C4 sra $v0, $v0, 16 */
    r_a0 = r_s5 + r_v0; /* 800AC7C8 addu $a0, $s5, $v0 */
    bl_sh(r_sp + 0x9Cu, r_v1); /* 800AC7CC sh $v1, 0x9C($sp) */
    r_v1 = bl_lhu(r_a0); /* 800AC7D0 lhu $v1, 0x0($a0) */
    r_a0 = r_a0 + 0x2u; /* 800AC7D4 addiu $a0, $a0, 0x2 */
    bl_sh(r_sp + 0x9Cu, r_v1); /* 800AC7D8 sh $v1, 0x9C($sp) */
    r_v0 = bl_lhu(r_a0); /* 800AC7DC lhu $v0, 0x0($a0) */
    r_v1 = r_v1 >> 8; /* 800AC7E0 srl $v1, $v1, 8 */
    r_v0 = r_v0 + 0x1u; /* 800AC7E4 addiu $v0, $v0, 0x1 */
    bl_sh(r_sp + 0x9Cu, r_v0); /* 800AC7E8 sh $v0, 0x9C($sp) */
    bl_sh(r_a0, r_v0); /* 800AC7EC sh $v0, 0x0($a0) */
    r_v0 = bl_lh(r_sp + 0x9Cu); /* 800AC7F0 lh $v0, 0x9C($sp) */
    ; /* 800AC7F4 nop  */
    r_v0 = (u32)((s32)r_v0 < (s32)r_v1); /* 800AC7F8 slt $v0, $v0, $v1 */
    /* 800AC7FC beqz $v0, .L800ADEBC */
    { int c_ = (r_v0 == 0); r_a0 = r_a0 + 0x2u; if (c_) goto L_800ADEBC; }
    /* 800AC804 j .L800ADEBC */
    r_s3 = r_a0 + 0u; goto L_800ADEBC;
L_800AC80C:
    r_v1 = bl_lhu(r_s3); /* 800AC80C lhu $v1, 0x0($s3) */
    /* 800AC810 j .L800AD704 */
    r_v0 = r_v1 << 16; goto L_800AD704;
L_800AC818:
    r_v1 = bl_lhu(r_s3); /* 800AC818 lhu $v1, 0x0($s3) */
    r_v0 = 0x800D0000u; /* 800AC81C lui $v0, %hi(D_800D36B8) */
    r_v0 = bl_lbu(r_v0 + 0x36B8u); /* 800AC820 lbu $v0, %lo(D_800D36B8)($v0) */
    r_s3 = r_s3 + 0x2u; /* 800AC824 addiu $s3, $s3, 0x2 */
    /* 800AC828 beqz $v0, .L800ADEBC */
    { int c_ = (r_v0 == 0); bl_sh(r_sp + 0x9Cu, r_v1); if (c_) goto L_800ADEBC; }
    /* 800AC830 j .L800AD594 */
    r_v0 = r_v1 << 16; goto L_800AD594;
L_800AC838:
    r_v1 = bl_lhu(r_s3); /* 800AC838 lhu $v1, 0x0($s3) */
    ; /* 800AC83C nop  */
    bl_sh(r_sp + 0x9Cu, r_v1); /* 800AC840 sh $v1, 0x9C($sp) */
    r_v0 = bl_lbu(r_s4 + 0x22u); /* 800AC844 lbu $v0, 0x22($s4) */
    ; /* 800AC848 nop  */
    /* 800AC84C bnez $v0, .L800ADEBC */
    { int c_ = (r_v0 != 0); r_s3 = r_s3 + 0x2u; if (c_) goto L_800ADEBC; }
    /* 800AC854 j .L800AD594 */
    r_v0 = r_v1 << 16; goto L_800AD594;
L_800AC85C:
    r_v0 = bl_lhu(r_s3); /* 800AC85C lhu $v0, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800AC860 addiu $s3, $s3, 0x2 */
    /* 800AC864 jal rand */
    bl_sh(r_sp + 0x9Cu, r_v0);
    bl_jal(FN, 0x8003FA38u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* rand */
    r_v0 = (u32)((s32)r_v0 < 16384); /* 800AC86C slti $v0, $v0, 0x4000 */
    /* 800AC870 beqz $v0, .L800ADEA8 */
    { int c_ = (r_v0 == 0); ; if (c_) goto L_800ADEA8; }
    /* 800AC878 j .L800ADEBC */
    ; goto L_800ADEBC;
L_800AC880:
    bl_sh(r_s4 + 0x44u, 0u); /* 800AC880 sh $zero, 0x44($s4) */
    r_v0 = bl_lhu(r_s3); /* 800AC884 lhu $v0, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800AC888 addiu $s3, $s3, 0x2 */
    bl_sh(r_s4 + 0x46u, r_v0); /* 800AC88C sh $v0, 0x46($s4) */
    r_v1 = bl_lhu(r_s3); /* 800AC890 lhu $v1, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800AC894 addiu $s3, $s3, 0x2 */
    r_v0 = r_s6 & 0xFFu; /* 800AC898 andi $v0, $s6, 0xFF */
    /* 800AC89C beqz $v0, .L800AC8B8 */
    { int c_ = (r_v0 == 0); bl_sh(r_sp + 0x9Cu, r_v1); if (c_) goto L_800AC8B8; }
    r_v0 = r_v1 << 16; /* 800AC8A4 sll $v0, $v1, 16 */
    r_v0 = BL_SRA(r_v0, 16); /* 800AC8A8 sra $v0, $v0, 16 */
    r_v0 = r_s5 + r_v0; /* 800AC8AC addu $v0, $s5, $v0 */
    /* 800AC8B0 j .L800ADEBC */
    bl_sw(r_s4 + 0x50u, r_v0); goto L_800ADEBC;
L_800AC8B8:
    /* 800AC8B8 j .L800ADEBC */
    bl_sw(r_s4 + 0x50u, 0u); goto L_800ADEBC;
L_800AC8C0:
    r_v1 = bl_lhu(r_s3); /* 800AC8C0 lhu $v1, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800AC8C4 addiu $s3, $s3, 0x2 */
    r_v0 = r_s6 & 0xFFu; /* 800AC8C8 andi $v0, $s6, 0xFF */
    /* 800AC8CC beqz $v0, .L800AC8E8 */
    { int c_ = (r_v0 == 0); bl_sh(r_sp + 0x9Cu, r_v1); if (c_) goto L_800AC8E8; }
    r_v0 = r_v1 << 16; /* 800AC8D4 sll $v0, $v1, 16 */
    r_v0 = BL_SRA(r_v0, 16); /* 800AC8D8 sra $v0, $v0, 16 */
    r_v0 = r_s5 + r_v0; /* 800AC8DC addu $v0, $s5, $v0 */
    /* 800AC8E0 j .L800ADEBC */
    bl_sw(r_s4 + 0x54u, r_v0); goto L_800ADEBC;
L_800AC8E8:
    /* 800AC8E8 j .L800ADEBC */
    bl_sw(r_s4 + 0x54u, 0u); goto L_800ADEBC;
L_800AC8F0:
    r_v0 = bl_lhu(r_s3); /* 800AC8F0 lhu $v0, 0x0($s3) */
    r_a0 = bl_lw(r_sp + 0x130u); /* 800AC8F4 lw $a0, 0x130($sp) */
    bl_sh(r_sp + 0x9Cu, r_v0); /* 800AC8F8 sh $v0, 0x9C($sp) */
    r_v1 = bl_lbu(r_sp + 0x9Cu); /* 800AC8FC lbu $v1, 0x9C($sp) */
    r_v0 = r_v0 >> 8; /* 800AC900 srl $v0, $v0, 8 */
    bl_sw(r_sp + 0x14u, r_v0); /* 800AC904 sw $v0, 0x14($sp) */
    bl_sw(r_sp + 0x10u, r_v1); /* 800AC908 sw $v1, 0x10($sp) */
    r_v0 = bl_lh(r_s4 + 0x88u); /* 800AC90C lh $v0, 0x88($s4) */
    ; /* 800AC910 nop  */
    bl_sw(r_sp + 0x18u, r_v0); /* 800AC914 sw $v0, 0x18($sp) */
    r_v0 = bl_lh(r_s4 + 0x8Au); /* 800AC918 lh $v0, 0x8A($s4) */
    r_s3 = r_s3 + 0x2u; /* 800AC91C addiu $s3, $s3, 0x2 */
    bl_sw(r_sp + 0x1Cu, r_v0); /* 800AC920 sw $v0, 0x1C($sp) */
    r_v0 = bl_lh(r_s4 + 0x8Cu); /* 800AC924 lh $v0, 0x8C($s4) */
    /* 800AC928 j .L800AC96C */
    r_a2 = 0u + 0u; goto L_800AC96C;
L_800AC930:
    r_v0 = bl_lhu(r_s3); /* 800AC930 lhu $v0, 0x0($s3) */
    r_a0 = bl_lw(r_sp + 0x130u); /* 800AC934 lw $a0, 0x130($sp) */
    bl_sh(r_sp + 0x9Cu, r_v0); /* 800AC938 sh $v0, 0x9C($sp) */
    r_v1 = bl_lbu(r_sp + 0x9Cu); /* 800AC93C lbu $v1, 0x9C($sp) */
    r_v0 = r_v0 >> 8; /* 800AC940 srl $v0, $v0, 8 */
    bl_sw(r_sp + 0x14u, r_v0); /* 800AC944 sw $v0, 0x14($sp) */
    bl_sw(r_sp + 0x10u, r_v1); /* 800AC948 sw $v1, 0x10($sp) */
    r_v0 = bl_lh(r_s4 + 0x88u); /* 800AC94C lh $v0, 0x88($s4) */
    ; /* 800AC950 nop  */
    bl_sw(r_sp + 0x18u, r_v0); /* 800AC954 sw $v0, 0x18($sp) */
    r_v0 = bl_lh(r_s4 + 0x8Au); /* 800AC958 lh $v0, 0x8A($s4) */
    r_s3 = r_s3 + 0x2u; /* 800AC95C addiu $s3, $s3, 0x2 */
    bl_sw(r_sp + 0x1Cu, r_v0); /* 800AC960 sw $v0, 0x1C($sp) */
    r_v0 = bl_lh(r_s4 + 0x8Cu); /* 800AC964 lh $v0, 0x8C($s4) */
    r_a2 = 0u | 0x1u; /* 800AC968 ori $a2, $zero, 0x1 */
L_800AC96C:
    bl_sw(r_sp + 0x20u, r_v0); /* 800AC96C sw $v0, 0x20($sp) */
    r_a1 = bl_lw(r_s4 + 0x4u); /* 800AC970 lw $a1, 0x4($s4) */
    /* 800AC974 jal func_800AE098 */
    r_a3 = r_s6 & 0xFFu;
    bl_jal(FN, 0x800AE098u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800AE098 */
    /* 800AC97C j .L800ADEBC */
    ; goto L_800ADEBC;
L_800AC984:
    /* 800AC984 jal func_800AA7DC */
    r_a0 = 0u | 0x2u;
    bl_jal(FN, 0x800AA7DCu, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800AA7DC */
    r_v0 = r_v0 & 0xFFu; /* 800AC98C andi $v0, $v0, 0xFF */
    r_v1 = 0u | 0xFEu; /* 800AC990 ori $v1, $zero, 0xFE */
    /* 800AC994 beq $v0, $v1, .L800AC9BC */
    { int c_ = (r_v0 == r_v1); ; if (c_) goto L_800AC9BC; }
    r_v0 = bl_lbu(r_s4 + 0x22u); /* 800AC99C lbu $v0, 0x22($s4) */
    ; /* 800AC9A0 nop  */
    /* 800AC9A4 beqz $v0, .L800AC9BC */
    { int c_ = (r_v0 == 0); ; if (c_) goto L_800AC9BC; }
    r_v0 = bl_lbu(r_s4 + 0x35u); /* 800AC9AC lbu $v0, 0x35($s4) */
    ; /* 800AC9B0 nop  */
    /* 800AC9B4 beqz $v0, .L800ADEBC */
    { int c_ = (r_v0 == 0); ; if (c_) goto L_800ADEBC; }
L_800AC9BC:
    /* 800AC9BC jal func_800BF6F8 */
    ;
    bl_jal(FN, 0x800BF6F8u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800BF6F8 */
    /* 800AC9C4 beqz $v0, .L800ADEBC */
    { int c_ = (r_v0 == 0); ; if (c_) goto L_800ADEBC; }
    /* 800AC9CC j .L800ADEB8 */
    r_s3 = r_s5 + 0u; goto L_800ADEB8;
L_800AC9D4:
    r_a0 = bl_lhu(r_s3); /* 800AC9D4 lhu $a0, 0x0($s3) */
    r_v0 = 0x800C0000u; /* 800AC9D8 lui $v0, %hi(D_800C48E8) */
    r_v0 = bl_lhu(r_v0 + 0x48E8u); /* 800AC9DC lhu $v0, %lo(D_800C48E8)($v0) */
    bl_sh(r_sp + 0x9Cu, r_a0); /* 800AC9E0 sh $a0, 0x9C($sp) */
    r_v1 = bl_lbu(r_s4 + 0x20u); /* 800AC9E4 lbu $v1, 0x20($s4) */
    ; /* 800AC9E8 nop  */
    r_v0 = BL_SRA(r_v0, r_v1); /* 800AC9EC srav $v0, $v0, $v1 */
    r_v0 = r_v0 & 0x1u; /* 800AC9F0 andi $v0, $v0, 0x1 */
    /* 800AC9F4 beqz $v0, .L800ADEBC */
    { int c_ = (r_v0 == 0); r_s3 = r_s3 + 0x2u; if (c_) goto L_800ADEBC; }
    /* 800AC9FC j .L800AD594 */
    r_v0 = r_a0 << 16; goto L_800AD594;
L_800ACA04:
    r_v0 = bl_lhu(r_s3); /* 800ACA04 lhu $v0, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800ACA08 addiu $s3, $s3, 0x2 */
    r_a0 = r_s4 + 0u; /* 800ACA0C addu $a0, $s4, $zero */
    r_a1 = r_s6 & 0xFFu; /* 800ACA10 andi $a1, $s6, 0xFF */
    bl_sh(r_sp + 0x9Cu, r_v0); /* 800ACA14 sh $v0, 0x9C($sp) */
    r_s1 = bl_lbu(r_sp + 0x9Cu); /* 800ACA18 lbu $s1, 0x9C($sp) */
    /* 800ACA1C jal func_800AE220 */
    r_s2 = r_v0 >> 8;
    bl_jal(FN, 0x800AE220u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800AE220 */
    r_a1 = 0u + 0u; /* 800ACA24 addu $a1, $zero, $zero */
    r_a2 = r_s2 + 0u; /* 800ACA28 addu $a2, $s2, $zero */
    /* 800ACA2C jal func_8003A3B8 */
    r_a0 = r_s1 + r_v0;
    bl_jal(FN, 0x8003A3B8u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_8003A3B8 */
    /* 800ACA34 j .L800ADEBC */
    ; goto L_800ADEBC;
L_800ACA3C:
    r_v0 = bl_lbu(r_s4 + 0x2Bu); /* 800ACA3C lbu $v0, 0x2B($s4) */
    ; /* 800ACA40 nop  */
    r_v1 = r_v0 + 0u; /* 800ACA44 addu $v1, $v0, $zero */
    bl_sh(r_sp + 0x9Cu, r_v0); /* 800ACA48 sh $v0, 0x9C($sp) */
    r_v0 = (u32)((s32)r_v1 < 2); /* 800ACA4C slti $v0, $v1, 0x2 */
    /* 800ACA50 bnez $v0, .L800ADEBC */
    { int c_ = (r_v0 != 0); r_t7 = 0u | 0x1u; if (c_) goto L_800ADEBC; }
    r_v0 = (u32)((s32)r_t7 < (s32)r_v1); /* 800ACA58 slt $v0, $t7, $v1 */
    /* 800ACA5C beqz $v0, .L800ADEBC */
    { int c_ = (r_v0 == 0); bl_sw(r_sp + 0x98u, r_t7); if (c_) goto L_800ADEBC; }
    r_a0 = r_s6 & 0xFFu; /* 800ACA64 andi $a0, $s6, 0xFF */
    r_t1 = r_v1 + 0u; /* 800ACA68 addu $t1, $v1, $zero */
L_800ACA6C:
    r_t0 = bl_lw(r_sp + 0x98u); /* 800ACA6C lw $t0, 0x98($sp) */
    ; /* 800ACA70 nop  */
    r_v0 = r_s4 + r_t0; /* 800ACA74 addu $v0, $s4, $t0 */
    r_v0 = bl_lbu(r_v0 + 0x2Fu); /* 800ACA78 lbu $v0, 0x2F($v0) */
    ; /* 800ACA7C nop  */
    /* 800ACA80 beq $v0, $a0, .L800AD4D8 */
    { int c_ = (r_v0 == r_a0); r_v0 = r_t0 + 0x1u; if (c_) goto L_800AD4D8; }
    bl_sw(r_sp + 0x98u, r_v0); /* 800ACA88 sw $v0, 0x98($sp) */
    r_v0 = (u32)((s32)r_v0 < (s32)r_t1); /* 800ACA8C slt $v0, $v0, $t1 */
    /* 800ACA90 bnez $v0, .L800ACA6C */
    { int c_ = (r_v0 != 0); ; if (c_) goto L_800ACA6C; }
    /* 800ACA98 j .L800ADEBC */
    ; goto L_800ADEBC;
L_800ACAA0:
    r_s1 = 0u | 0x1u; /* 800ACAA0 ori $s1, $zero, 0x1 */
    r_a3 = 0x80070000u; /* 800ACAA4 lui $a3, %hi(jtbl_80070744) */
    r_a3 = r_a3 + 0x744u; /* 800ACAA8 addiu $a3, $a3, %lo(jtbl_80070744) */
    r_a0 = r_s6 & 0xFFu; /* 800ACAAC andi $a0, $s6, 0xFF */
    r_a2 = 0x800C0000u; /* 800ACAB0 lui $a2, %hi(D_800C3E30) */
    r_a2 = bl_lhu(r_a2 + 0x3E30u); /* 800ACAB4 lhu $a2, %lo(D_800C3E30)($a2) */
    r_v0 = 0x800C0000u; /* 800ACAB8 lui $v0, %hi(D_800C360C) */
    r_v0 = bl_lw(r_v0 + 0x360Cu); /* 800ACABC lw $v0, %lo(D_800C360C)($v0) */
    r_a1 = (u32)(r_a0 < 0x8u); /* 800ACAC0 sltiu $a1, $a0, 0x8 */
    bl_sw(r_sp + 0x98u, 0u); /* 800ACAC4 sw $zero, 0x98($sp) */
    r_v0 = r_v0 + 0xFFFFFFFFu; /* 800ACAC8 addiu $v0, $v0, -0x1 */
    r_v1 = r_v0 << 3; /* 800ACACC sll $v1, $v0, 3 */
    r_v1 = r_v1 + r_v0; /* 800ACAD0 addu $v1, $v1, $v0 */
    r_v1 = r_v1 << 3; /* 800ACAD4 sll $v1, $v1, 3 */
    r_v0 = 0x800C0000u; /* 800ACAD8 lui $v0, %hi(D_800C4000) */
    r_v0 = r_v0 + 0x4000u; /* 800ACADC addiu $v0, $v0, %lo(D_800C4000) */
    r_v1 = r_v1 + r_v0; /* 800ACAE0 addu $v1, $v1, $v0 */
L_800ACAE4:
    r_t0 = bl_lw(r_sp + 0x98u); /* 800ACAE4 lw $t0, 0x98($sp) */
    ; /* 800ACAE8 nop  */
    r_v0 = BL_SRA(r_a2, r_t0); /* 800ACAEC srav $v0, $a2, $t0 */
    r_v0 = r_v0 & 0x1u; /* 800ACAF0 andi $v0, $v0, 0x1 */
    /* 800ACAF4 beqz $v0, .L800ACB98 */
    { int c_ = (r_v0 == 0); r_v0 = r_v1 + r_t0; if (c_) goto L_800ACB98; }
    r_t0 = bl_lbu(r_v0); /* 800ACAFC lbu $t0, 0x0($v0) */
    ; /* 800ACB00 nop  */
    r_v0 = (u32)(r_t0 < 0x6u); /* 800ACB04 sltiu $v0, $t0, 0x6 */
    /* 800ACB08 beqz $v0, .L800ACB34 */
    { int c_ = (r_v0 == 0); r_v0 = r_t0 << 2; if (c_) goto L_800ACB34; }
    r_v0 = r_v0 + r_a3; /* 800ACB10 addu $v0, $v0, $a3 */
    r_v0 = bl_lw(r_v0); /* 800ACB14 lw $v0, 0x0($v0) */
    ; /* 800ACB18 nop  */
    /* 800ACB1C jr $v0 */
    { u32 t_ = r_v0; ; switch (t_) { case 0x800AB114u: goto L_800AB114; case 0x800AB11Cu: goto L_800AB11C; case 0x800AB180u: goto L_800AB180; case 0x800AB1B0u: goto L_800AB1B0; case 0x800AB1FCu: goto L_800AB1FC; case 0x800AB284u: goto L_800AB284; case 0x800AB44Cu: goto L_800AB44C; case 0x800AB45Cu: goto L_800AB45C; case 0x800AB46Cu: goto L_800AB46C; case 0x800AB484u: goto L_800AB484; case 0x800AB49Cu: goto L_800AB49C; case 0x800AB4B8u: goto L_800AB4B8; case 0x800AB520u: goto L_800AB520; case 0x800AB554u: goto L_800AB554; case 0x800AB570u: goto L_800AB570; case 0x800AB58Cu: goto L_800AB58C; case 0x800AB5B0u: goto L_800AB5B0; case 0x800AB5D4u: goto L_800AB5D4; case 0x800AB678u: goto L_800AB678; case 0x800AB704u: goto L_800AB704; case 0x800AB778u: goto L_800AB778; case 0x800AB898u: goto L_800AB898; case 0x800ABB48u: goto L_800ABB48; case 0x800ABBB8u: goto L_800ABBB8; case 0x800ABBCCu: goto L_800ABBCC; case 0x800ABBF8u: goto L_800ABBF8; case 0x800ABC08u: goto L_800ABC08; case 0x800ABCCCu: goto L_800ABCCC; case 0x800ABEFCu: goto L_800ABEFC; case 0x800ABF2Cu: goto L_800ABF2C; case 0x800ABFECu: goto L_800ABFEC; case 0x800ABFF4u: goto L_800ABFF4; case 0x800AC030u: goto L_800AC030; case 0x800AC058u: goto L_800AC058; case 0x800AC088u: goto L_800AC088; case 0x800AC154u: goto L_800AC154; case 0x800AC184u: goto L_800AC184; case 0x800AC194u: goto L_800AC194; case 0x800AC4A8u: goto L_800AC4A8; case 0x800AC520u: goto L_800AC520; case 0x800AC560u: goto L_800AC560; case 0x800AC660u: goto L_800AC660; case 0x800AC684u: goto L_800AC684; case 0x800AC6A8u: goto L_800AC6A8; case 0x800AC758u: goto L_800AC758; case 0x800AC794u: goto L_800AC794; case 0x800AC7ACu: goto L_800AC7AC; case 0x800AC7B8u: goto L_800AC7B8; case 0x800AC80Cu: goto L_800AC80C; case 0x800AC818u: goto L_800AC818; case 0x800AC838u: goto L_800AC838; case 0x800AC85Cu: goto L_800AC85C; case 0x800AC880u: goto L_800AC880; case 0x800AC8C0u: goto L_800AC8C0; case 0x800AC8F0u: goto L_800AC8F0; case 0x800AC930u: goto L_800AC930; case 0x800AC984u: goto L_800AC984; case 0x800AC9D4u: goto L_800AC9D4; case 0x800ACA04u: goto L_800ACA04; case 0x800ACA3Cu: goto L_800ACA3C; case 0x800ACAA0u: goto L_800ACAA0; case 0x800ACB24u: goto L_800ACB24; case 0x800ACB2Cu: goto L_800ACB2C; case 0x800ACB34u: goto L_800ACB34; case 0x800ACBCCu: goto L_800ACBCC; case 0x800ACBDCu: goto L_800ACBDC; case 0x800ACC24u: goto L_800ACC24; case 0x800ACC98u: goto L_800ACC98; case 0x800ACD64u: goto L_800ACD64; case 0x800ACD8Cu: goto L_800ACD8C; case 0x800ACDCCu: goto L_800ACDCC; case 0x800ACDF4u: goto L_800ACDF4; case 0x800ACE34u: goto L_800ACE34; case 0x800ACE3Cu: goto L_800ACE3C; case 0x800ACE70u: goto L_800ACE70; case 0x800AD020u: goto L_800AD020; case 0x800AD048u: goto L_800AD048; case 0x800AD088u: goto L_800AD088; case 0x800AD0B0u: goto L_800AD0B0; case 0x800AD0F0u: goto L_800AD0F0; case 0x800AD230u: goto L_800AD230; case 0x800AD25Cu: goto L_800AD25C; case 0x800AD2BCu: goto L_800AD2BC; case 0x800AD2FCu: goto L_800AD2FC; case 0x800AD338u: goto L_800AD338; case 0x800AD370u: goto L_800AD370; case 0x800AD3A8u: goto L_800AD3A8; case 0x800AD3C0u: goto L_800AD3C0; case 0x800AD3F0u: goto L_800AD3F0; case 0x800AD424u: goto L_800AD424; case 0x800AD468u: goto L_800AD468; case 0x800AD4ACu: goto L_800AD4AC; case 0x800AD550u: goto L_800AD550; case 0x800AD5A0u: goto L_800AD5A0; case 0x800AD5D4u: goto L_800AD5D4; case 0x800AD5E8u: goto L_800AD5E8; case 0x800AD5FCu: goto L_800AD5FC; case 0x800AD660u: goto L_800AD660; case 0x800AD68Cu: goto L_800AD68C; case 0x800AD6DCu: goto L_800AD6DC; case 0x800AD714u: goto L_800AD714; case 0x800AD724u: goto L_800AD724; case 0x800AD990u: goto L_800AD990; case 0x800ADBF4u: goto L_800ADBF4; case 0x800ADC9Cu: goto L_800ADC9C; case 0x800ADCF0u: goto L_800ADCF0; case 0x800ADD04u: goto L_800ADD04; case 0x800ADD34u: goto L_800ADD34; case 0x800ADD4Cu: goto L_800ADD4C; case 0x800ADD58u: goto L_800ADD58; case 0x800ADDA8u: goto L_800ADDA8; case 0x800ADDCCu: goto L_800ADDCC; case 0x800ADDF8u: goto L_800ADDF8; case 0x800ADE1Cu: goto L_800ADE1C; case 0x800ADE38u: goto L_800ADE38; case 0x800ADE50u: goto L_800ADE50; case 0x800ADE60u: goto L_800ADE60; default: bl_fail(FN, "jump-table target", t_); } }
L_800ACB24:
    /* 800ACB24 j .L800ACB38 */
    r_s2 = 0u + 0u; goto L_800ACB38;
L_800ACB2C:
    /* 800ACB2C j .L800ACB38 */
    r_s2 = 0u | 0x5u; goto L_800ACB38;
L_800ACB34:
    r_s2 = 0u | 0x4u; /* 800ACB34 ori $s2, $zero, 0x4 */
L_800ACB38:
    r_v0 = r_s2 & 0xFFu; /* 800ACB38 andi $v0, $s2, 0xFF */
    /* 800ACB3C beq $v0, $a0, .L800ACB50 */
    { int c_ = (r_v0 == r_a0); ; if (c_) goto L_800ACB50; }
    /* 800ACB44 beqz $a1, .L800ACB54 */
    { int c_ = (r_a1 == 0); r_v0 = 0u | 0x8u; if (c_) goto L_800ACB54; }
    r_s1 = 0u + 0u; /* 800ACB4C addu $s1, $zero, $zero */
L_800ACB50:
    r_v0 = 0u | 0x8u; /* 800ACB50 ori $v0, $zero, 0x8 */
L_800ACB54:
    /* 800ACB54 bne $a0, $v0, .L800ACB98 */
    { int c_ = (r_a0 != r_v0); ; if (c_) goto L_800ACB98; }
    r_v0 = bl_lw(r_sp + 0x98u); /* 800ACB5C lw $v0, 0x98($sp) */
    r_t8 = 0x800D0000u; /* 800ACB60 lui $t8, %hi(D_800D3368) */
    r_t8 = r_t8 + 0x3368u; /* 800ACB64 addiu $t8, $t8, %lo(D_800D3368) */
    r_v0 = r_v0 << 2; /* 800ACB68 sll $v0, $v0, 2 */
    r_v0 = r_v0 + r_t8; /* 800ACB6C addu $v0, $v0, $t8 */
    r_v0 = bl_lw(r_v0); /* 800ACB70 lw $v0, 0x0($v0) */
    ; /* 800ACB74 nop  */
    /* 800ACB78 beqz $v0, .L800ACB94 */
    { int c_ = (r_v0 == 0); ; if (c_) goto L_800ACB94; }
    r_v0 = bl_lhu(r_v0 + 0x4Au); /* 800ACB80 lhu $v0, 0x4A($v0) */
    ; /* 800ACB84 nop  */
    r_v0 = r_v0 & 0x2u; /* 800ACB88 andi $v0, $v0, 0x2 */
    /* 800ACB8C bnez $v0, .L800ACB98 */
    { int c_ = (r_v0 != 0); ; if (c_) goto L_800ACB98; }
L_800ACB94:
    r_s1 = 0u + 0u; /* 800ACB94 addu $s1, $zero, $zero */
L_800ACB98:
    r_v0 = bl_lw(r_sp + 0x98u); /* 800ACB98 lw $v0, 0x98($sp) */
    ; /* 800ACB9C nop  */
    r_v0 = r_v0 + 0x1u; /* 800ACBA0 addiu $v0, $v0, 0x1 */
    bl_sw(r_sp + 0x98u, r_v0); /* 800ACBA4 sw $v0, 0x98($sp) */
    r_v0 = (u32)((s32)r_v0 < 13); /* 800ACBA8 slti $v0, $v0, 0xD */
    /* 800ACBAC bnez $v0, .L800ACAE4 */
    { int c_ = (r_v0 != 0); ; if (c_) goto L_800ACAE4; }
    r_v0 = bl_lhu(r_s3); /* 800ACBB4 lhu $v0, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800ACBB8 addiu $s3, $s3, 0x2 */
    /* 800ACBBC beqz $s1, .L800ADEBC */
    { int c_ = (r_s1 == 0); bl_sh(r_sp + 0x9Cu, r_v0); if (c_) goto L_800ADEBC; }
    /* 800ACBC4 j .L800AD594 */
    r_v0 = r_v0 << 16; goto L_800AD594;
L_800ACBCC:
    /* 800ACBCC jal func_800BF6CC */
    ;
    bl_jal(FN, 0x800BF6CCu, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800BF6CC */
    /* 800ACBD4 j .L800ADEBC */
    ; goto L_800ADEBC;
L_800ACBDC:
    r_a3 = bl_lhu(r_s3); /* 800ACBDC lhu $a3, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800ACBE0 addiu $s3, $s3, 0x2 */
    r_v0 = bl_lhu(r_s3); /* 800ACBE4 lhu $v0, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800ACBE8 addiu $s3, $s3, 0x2 */
    r_v1 = bl_lhu(r_s3); /* 800ACBEC lhu $v1, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800ACBF0 addiu $s3, $s3, 0x2 */
    r_a2 = r_s6 & 0xFFu; /* 800ACBF4 andi $a2, $s6, 0xFF */
    r_a0 = bl_lw(r_sp + 0x130u); /* 800ACBF8 lw $a0, 0x130($sp) */
    r_t7 = 0u + 0xFFFFFFFFu; /* 800ACBFC addiu $t7, $zero, -0x1 */
    bl_sw(r_sp + 0x138u, r_t7); /* 800ACC00 sw $t7, 0x138($sp) */
    r_v0 = r_v0 << 16; /* 800ACC04 sll $v0, $v0, 16 */
    r_v0 = BL_SRA(r_v0, 16); /* 800ACC08 sra $v0, $v0, 16 */
    r_v1 = r_v1 << 16; /* 800ACC0C sll $v1, $v1, 16 */
    r_v1 = BL_SRA(r_v1, 16); /* 800ACC10 sra $v1, $v1, 16 */
    r_a3 = r_a3 << 16; /* 800ACC14 sll $a3, $a3, 16 */
    bl_sw(r_sp + 0x10u, r_v0); /* 800ACC18 sw $v0, 0x10($sp) */
    /* 800ACC1C j .L800ACC84 */
    bl_sw(r_sp + 0x14u, r_v1); goto L_800ACC84;
L_800ACC24:
    r_t1 = bl_lhu(r_s3); /* 800ACC24 lhu $t1, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800ACC28 addiu $s3, $s3, 0x2 */
    r_a1 = bl_lhu(r_s3); /* 800ACC2C lhu $a1, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800ACC30 addiu $s3, $s3, 0x2 */
    r_t0 = bl_lhu(r_s3); /* 800ACC34 lhu $t0, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800ACC38 addiu $s3, $s3, 0x2 */
    r_a2 = r_s6 & 0xFFu; /* 800ACC3C andi $a2, $s6, 0xFF */
    r_v0 = bl_lw(r_s4 + 0x4u); /* 800ACC40 lw $v0, 0x4($s4) */
    r_a0 = bl_lw(r_sp + 0x130u); /* 800ACC44 lw $a0, 0x130($sp) */
    r_v1 = bl_lhu(r_v0 + 0x56u); /* 800ACC48 lhu $v1, 0x56($v0) */
    r_t8 = 0u + 0xFFFFFFFFu; /* 800ACC4C addiu $t8, $zero, -0x1 */
    bl_sw(r_sp + 0x138u, r_t8); /* 800ACC50 sw $t8, 0x138($sp) */
    r_a3 = bl_lhu(r_v0 + 0x54u); /* 800ACC54 lhu $a3, 0x54($v0) */
    r_v0 = bl_lhu(r_v0 + 0x58u); /* 800ACC58 lhu $v0, 0x58($v0) */
    r_v1 = r_v1 + r_a1; /* 800ACC5C addu $v1, $v1, $a1 */
    r_v1 = r_v1 << 16; /* 800ACC60 sll $v1, $v1, 16 */
    r_v1 = BL_SRA(r_v1, 16); /* 800ACC64 sra $v1, $v1, 16 */
    r_v0 = r_v0 + r_t0; /* 800ACC68 addu $v0, $v0, $t0 */
    r_v0 = r_v0 << 16; /* 800ACC6C sll $v0, $v0, 16 */
    r_v0 = BL_SRA(r_v0, 16); /* 800ACC70 sra $v0, $v0, 16 */
    r_a3 = r_a3 + r_t1; /* 800ACC74 addu $a3, $a3, $t1 */
    r_a3 = r_a3 << 16; /* 800ACC78 sll $a3, $a3, 16 */
    bl_sw(r_sp + 0x10u, r_v1); /* 800ACC7C sw $v1, 0x10($sp) */
    bl_sw(r_sp + 0x14u, r_v0); /* 800ACC80 sw $v0, 0x14($sp) */
L_800ACC84:
    r_a1 = bl_lw(r_s4 + 0x4u); /* 800ACC84 lw $a1, 0x4($s4) */
    /* 800ACC88 jal func_800ADF1C */
    r_a3 = BL_SRA(r_a3, 16);
    bl_jal(FN, 0x800ADF1Cu, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800ADF1C */
    /* 800ACC90 j .L800ADEBC */
    ; goto L_800ADEBC;
L_800ACC98:
    r_a0 = bl_lw(r_s4 + 0x4u); /* 800ACC98 lw $a0, 0x4($s4) */
    r_v1 = bl_lh(r_s4 + 0x8Au); /* 800ACC9C lh $v1, 0x8A($s4) */
    r_a2 = bl_lh(r_s4 + 0x88u); /* 800ACCA0 lh $a2, 0x88($s4) */
    r_v0 = bl_lw(r_a0 + 0x60u); /* 800ACCA4 lw $v0, 0x60($a0) */
    r_a1 = bl_lh(r_s4 + 0x8Cu); /* 800ACCA8 lh $a1, 0x8C($s4) */
    r_s2 = r_v1 - r_v0; /* 800ACCAC subu $s2, $v1, $v0 */
    r_v1 = bl_lw(r_a0 + 0x5Cu); /* 800ACCB0 lw $v1, 0x5C($a0) */
    r_v0 = bl_lw(r_a0 + 0x64u); /* 800ACCB4 lw $v0, 0x64($a0) */
    r_s0 = r_a2 - r_v1; /* 800ACCB8 subu $s0, $a2, $v1 */
    r_s1 = r_a1 - r_v0; /* 800ACCBC subu $s1, $a1, $v0 */
    r_v1 = r_fp & 0xFFu; /* 800ACCC0 andi $v1, $fp, 0xFF */
    r_v0 = 0u | 0x43u; /* 800ACCC4 ori $v0, $zero, 0x43 */
    /* 800ACCC8 bne $v1, $v0, .L800ACCDC */
    { int c_ = (r_v1 != r_v0); BL_MULT(r_s0, r_s0); if (c_) goto L_800ACCDC; }
    r_s5 = 0u + 0u; /* 800ACCD0 addu $s5, $zero, $zero */
    /* 800ACCD4 j .L800ACD08 */
    r_s2 = 0u + 0u; goto L_800ACD08;
L_800ACCDC:
    r_v0 = lo; /* 800ACCDC mflo $v0 */
    ; /* 800ACCE0 nop  */
    ; /* 800ACCE4 nop  */
    BL_MULT(r_s1, r_s1); /* 800ACCE8 mult $s1, $s1 */
    r_a0 = lo; /* 800ACCEC mflo $a0 */
    /* 800ACCF0 jal SquareRoot0 */
    r_a0 = r_v0 + r_a0;
    bl_jal(FN, 0x80048C4Cu, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* SquareRoot0 */
    r_a0 = r_s2 + 0u; /* 800ACCF8 addu $a0, $s2, $zero */
    /* 800ACCFC jal ratan2 */
    r_a1 = r_v0 + 0u;
    bl_jal(FN, 0x8004B32Cu, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* ratan2 */
    r_s5 = r_v0 + 0u; /* 800ACD04 addu $s5, $v0, $zero */
L_800ACD08:
    r_a0 = 0u - r_s0; /* 800ACD08 negu $a0, $s0 */
    /* 800ACD0C jal ratan2 */
    r_a1 = 0u - r_s1;
    bl_jal(FN, 0x8004B32Cu, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* ratan2 */
    /* 800ACD14 bnez $s0, .L800ACD2C */
    { int c_ = (r_s0 != 0); r_s7 = 0u + 0u; if (c_) goto L_800ACD2C; }
    /* 800ACD1C bnez $s2, .L800ACD30 */
    { int c_ = (r_s2 != 0); r_a2 = r_s6 & 0xFFu; if (c_) goto L_800ACD30; }
    /* 800ACD24 beqz $s1, .L800ADEBC */
    { int c_ = (r_s1 == 0); ; if (c_) goto L_800ADEBC; }
L_800ACD2C:
    r_a2 = r_s6 & 0xFFu; /* 800ACD2C andi $a2, $s6, 0xFF */
L_800ACD30:
    r_a3 = r_s5 << 16; /* 800ACD30 sll $a3, $s5, 16 */
    r_a3 = BL_SRA(r_a3, 16); /* 800ACD34 sra $a3, $a3, 16 */
    r_v0 = r_v0 << 16; /* 800ACD38 sll $v0, $v0, 16 */
    r_a0 = bl_lw(r_sp + 0x130u); /* 800ACD3C lw $a0, 0x130($sp) */
    r_v0 = BL_SRA(r_v0, 16); /* 800ACD40 sra $v0, $v0, 16 */
    bl_sw(r_sp + 0x10u, r_v0); /* 800ACD44 sw $v0, 0x10($sp) */
    bl_sw(r_sp + 0x14u, 0u); /* 800ACD48 sw $zero, 0x14($sp) */
    r_a1 = bl_lw(r_s4 + 0x4u); /* 800ACD4C lw $a1, 0x4($s4) */
    r_t7 = 0u + 0xFFFFFFFFu; /* 800ACD50 addiu $t7, $zero, -0x1 */
    /* 800ACD54 jal func_800ADF1C */
    bl_sw(r_sp + 0x138u, r_t7);
    bl_jal(FN, 0x800ADF1Cu, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800ADF1C */
    /* 800ACD5C j .L800ADEBC */
    ; goto L_800ADEBC;
L_800ACD64:
    r_v0 = bl_lhu(r_s3); /* 800ACD64 lhu $v0, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800ACD68 addiu $s3, $s3, 0x2 */
    bl_sh(r_s4 + 0x70u, r_v0); /* 800ACD6C sh $v0, 0x70($s4) */
    r_v0 = bl_lhu(r_s3); /* 800ACD70 lhu $v0, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800ACD74 addiu $s3, $s3, 0x2 */
    bl_sh(r_s4 + 0x72u, r_v0); /* 800ACD78 sh $v0, 0x72($s4) */
    r_v0 = bl_lhu(r_s3); /* 800ACD7C lhu $v0, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800ACD80 addiu $s3, $s3, 0x2 */
    /* 800ACD84 j .L800ADEBC */
    bl_sh(r_s4 + 0x74u, r_v0); goto L_800ADEBC;
L_800ACD8C:
    r_v0 = bl_lhu(r_s4 + 0x70u); /* 800ACD8C lhu $v0, 0x70($s4) */
    r_v1 = bl_lhu(r_s3); /* 800ACD90 lhu $v1, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800ACD94 addiu $s3, $s3, 0x2 */
    r_v0 = r_v0 + r_v1; /* 800ACD98 addu $v0, $v0, $v1 */
    bl_sh(r_s4 + 0x70u, r_v0); /* 800ACD9C sh $v0, 0x70($s4) */
    r_v0 = bl_lhu(r_s4 + 0x72u); /* 800ACDA0 lhu $v0, 0x72($s4) */
    r_v1 = bl_lhu(r_s3); /* 800ACDA4 lhu $v1, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800ACDA8 addiu $s3, $s3, 0x2 */
    r_v0 = r_v0 + r_v1; /* 800ACDAC addu $v0, $v0, $v1 */
    bl_sh(r_s4 + 0x72u, r_v0); /* 800ACDB0 sh $v0, 0x72($s4) */
    r_v0 = bl_lhu(r_s4 + 0x74u); /* 800ACDB4 lhu $v0, 0x74($s4) */
    r_v1 = bl_lhu(r_s3); /* 800ACDB8 lhu $v1, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800ACDBC addiu $s3, $s3, 0x2 */
    r_v0 = r_v0 + r_v1; /* 800ACDC0 addu $v0, $v0, $v1 */
    /* 800ACDC4 j .L800ADEBC */
    bl_sh(r_s4 + 0x74u, r_v0); goto L_800ADEBC;
L_800ACDCC:
    r_v0 = bl_lhu(r_s3); /* 800ACDCC lhu $v0, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800ACDD0 addiu $s3, $s3, 0x2 */
    bl_sh(r_s4 + 0x76u, r_v0); /* 800ACDD4 sh $v0, 0x76($s4) */
    r_v0 = bl_lhu(r_s3); /* 800ACDD8 lhu $v0, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800ACDDC addiu $s3, $s3, 0x2 */
    bl_sh(r_s4 + 0x78u, r_v0); /* 800ACDE0 sh $v0, 0x78($s4) */
    r_v0 = bl_lhu(r_s3); /* 800ACDE4 lhu $v0, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800ACDE8 addiu $s3, $s3, 0x2 */
    /* 800ACDEC j .L800ADEBC */
    bl_sh(r_s4 + 0x7Au, r_v0); goto L_800ADEBC;
L_800ACDF4:
    r_v0 = bl_lhu(r_s4 + 0x76u); /* 800ACDF4 lhu $v0, 0x76($s4) */
    r_v1 = bl_lhu(r_s3); /* 800ACDF8 lhu $v1, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800ACDFC addiu $s3, $s3, 0x2 */
    r_v0 = r_v0 + r_v1; /* 800ACE00 addu $v0, $v0, $v1 */
    bl_sh(r_s4 + 0x76u, r_v0); /* 800ACE04 sh $v0, 0x76($s4) */
    r_v0 = bl_lhu(r_s4 + 0x78u); /* 800ACE08 lhu $v0, 0x78($s4) */
    r_v1 = bl_lhu(r_s3); /* 800ACE0C lhu $v1, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800ACE10 addiu $s3, $s3, 0x2 */
    r_v0 = r_v0 + r_v1; /* 800ACE14 addu $v0, $v0, $v1 */
    bl_sh(r_s4 + 0x78u, r_v0); /* 800ACE18 sh $v0, 0x78($s4) */
    r_v0 = bl_lhu(r_s4 + 0x7Au); /* 800ACE1C lhu $v0, 0x7A($s4) */
    r_v1 = bl_lhu(r_s3); /* 800ACE20 lhu $v1, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800ACE24 addiu $s3, $s3, 0x2 */
    r_v0 = r_v0 + r_v1; /* 800ACE28 addu $v0, $v0, $v1 */
    /* 800ACE2C j .L800ADEBC */
    bl_sh(r_s4 + 0x7Au, r_v0); goto L_800ADEBC;
L_800ACE34:
    /* 800ACE34 j .L800ADEBC */
    bl_sb(r_s4 + 0x36u, r_s6); goto L_800ADEBC;
L_800ACE3C:
    r_v1 = bl_lw(r_s4 + 0x4u); /* 800ACE3C lw $v1, 0x4($s4) */
    r_v0 = bl_lh(r_s3); /* 800ACE40 lh $v0, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800ACE44 addiu $s3, $s3, 0x2 */
    bl_sw(r_v1 + 0x5Cu, r_v0); /* 800ACE48 sw $v0, 0x5C($v1) */
    r_v1 = bl_lw(r_s4 + 0x4u); /* 800ACE4C lw $v1, 0x4($s4) */
    r_v0 = bl_lh(r_s3); /* 800ACE50 lh $v0, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800ACE54 addiu $s3, $s3, 0x2 */
    bl_sw(r_v1 + 0x60u, r_v0); /* 800ACE58 sw $v0, 0x60($v1) */
    r_v1 = bl_lw(r_s4 + 0x4u); /* 800ACE5C lw $v1, 0x4($s4) */
    r_v0 = bl_lh(r_s3); /* 800ACE60 lh $v0, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800ACE64 addiu $s3, $s3, 0x2 */
    /* 800ACE68 j .L800ADEBC */
    bl_sw(r_v1 + 0x64u, r_v0); goto L_800ADEBC;
L_800ACE70:
    r_a1 = r_s6 & 0xFFu; /* 800ACE70 andi $a1, $s6, 0xFF */
    r_v0 = 0u | 0xFBu; /* 800ACE74 ori $v0, $zero, 0xFB */
    /* 800ACE78 bne $a1, $v0, .L800ACFD8 */
    { int c_ = (r_a1 != r_v0); r_a0 = r_s4 + 0u; if (c_) goto L_800ACFD8; }
    r_v1 = bl_lw(r_s4 + 0x4u); /* 800ACE80 lw $v1, 0x4($s4) */
    r_v0 = bl_lh(r_s4 + 0x88u); /* 800ACE84 lh $v0, 0x88($s4) */
    r_s0 = bl_lw(r_v1 + 0x5Cu); /* 800ACE88 lw $s0, 0x5C($v1) */
    ; /* 800ACE8C nop  */
    r_s0 = r_s0 - r_v0; /* 800ACE90 subu $s0, $s0, $v0 */
    BL_MULT(r_s0, r_s0); /* 800ACE94 mult $s0, $s0 */
    r_s1 = bl_lw(r_v1 + 0x60u); /* 800ACE98 lw $s1, 0x60($v1) */
    r_v0 = bl_lh(r_s4 + 0x8Au); /* 800ACE9C lh $v0, 0x8A($s4) */
    r_a1 = lo; /* 800ACEA0 mflo $a1 */
    r_s1 = r_s1 - r_v0; /* 800ACEA4 subu $s1, $s1, $v0 */
    ; /* 800ACEA8 nop  */
    BL_MULT(r_s1, r_s1); /* 800ACEAC mult $s1, $s1 */
    r_s2 = bl_lw(r_v1 + 0x64u); /* 800ACEB0 lw $s2, 0x64($v1) */
    r_v0 = bl_lh(r_s4 + 0x8Cu); /* 800ACEB4 lh $v0, 0x8C($s4) */
    r_v1 = lo; /* 800ACEB8 mflo $v1 */
    r_s2 = r_s2 - r_v0; /* 800ACEBC subu $s2, $s2, $v0 */
    ; /* 800ACEC0 nop  */
    BL_MULT(r_s2, r_s2); /* 800ACEC4 mult $s2, $s2 */
    r_a1 = r_a1 + r_v1; /* 800ACEC8 addu $a1, $a1, $v1 */
    r_a0 = lo; /* 800ACECC mflo $a0 */
    /* 800ACED0 jal SquareRoot0 */
    r_a0 = r_a1 + r_a0;
    bl_jal(FN, 0x80048C4Cu, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* SquareRoot0 */
    r_v1 = bl_lh(r_s4 + 0x8Eu); /* 800ACED8 lh $v1, 0x8E($s4) */
    ; /* 800ACEDC nop  */
    BL_MULT(r_s0, r_v1); /* 800ACEE0 mult $s0, $v1 */
    r_a0 = lo; /* 800ACEE4 mflo $a0 */
    r_v0 = r_v0 + 0x1u; /* 800ACEE8 addiu $v0, $v0, 0x1 */
    ; /* 800ACEEC nop  */
    BL_DIV(r_a0, r_v0); /* 800ACEF0 div $zero, $a0, $v0 */
    /* 800ACEF4 bnez $v0, .L800ACF00 */
    { int c_ = (r_v0 != 0); ; if (c_) goto L_800ACF00; }
    bl_fail(FN, "BREAK", 7u); /* 800ACEFC break 7 */
L_800ACF00:
    r_at = 0u + 0xFFFFFFFFu; /* 800ACF00 addiu $at, $zero, -0x1 */
    /* 800ACF04 bne $v0, $at, .L800ACF18 */
    { int c_ = (r_v0 != r_at); r_at = 0x80000000u; if (c_) goto L_800ACF18; }
    /* 800ACF0C bne $a0, $at, .L800ACF18 */
    { int c_ = (r_a0 != r_at); ; if (c_) goto L_800ACF18; }
    bl_fail(FN, "BREAK", 6u); /* 800ACF14 break 6 */
L_800ACF18:
    r_a0 = lo; /* 800ACF18 mflo $a0 */
    r_v1 = bl_lh(r_s4 + 0x88u); /* 800ACF1C lh $v1, 0x88($s4) */
    r_a1 = bl_lw(r_s4 + 0x4u); /* 800ACF20 lw $a1, 0x4($s4) */
    r_v1 = r_v1 + r_a0; /* 800ACF24 addu $v1, $v1, $a0 */
    bl_sw(r_a1 + 0x5Cu, r_v1); /* 800ACF28 sw $v1, 0x5C($a1) */
    r_v1 = bl_lh(r_s4 + 0x8Eu); /* 800ACF2C lh $v1, 0x8E($s4) */
    ; /* 800ACF30 nop  */
    BL_MULT(r_s1, r_v1); /* 800ACF34 mult $s1, $v1 */
    r_a0 = lo; /* 800ACF38 mflo $a0 */
    ; /* 800ACF3C nop  */
    ; /* 800ACF40 nop  */
    BL_DIV(r_a0, r_v0); /* 800ACF44 div $zero, $a0, $v0 */
    /* 800ACF48 bnez $v0, .L800ACF54 */
    { int c_ = (r_v0 != 0); ; if (c_) goto L_800ACF54; }
    bl_fail(FN, "BREAK", 7u); /* 800ACF50 break 7 */
L_800ACF54:
    r_at = 0u + 0xFFFFFFFFu; /* 800ACF54 addiu $at, $zero, -0x1 */
    /* 800ACF58 bne $v0, $at, .L800ACF6C */
    { int c_ = (r_v0 != r_at); r_at = 0x80000000u; if (c_) goto L_800ACF6C; }
    /* 800ACF60 bne $a0, $at, .L800ACF6C */
    { int c_ = (r_a0 != r_at); ; if (c_) goto L_800ACF6C; }
    bl_fail(FN, "BREAK", 6u); /* 800ACF68 break 6 */
L_800ACF6C:
    r_a0 = lo; /* 800ACF6C mflo $a0 */
    r_v1 = bl_lh(r_s4 + 0x8Au); /* 800ACF70 lh $v1, 0x8A($s4) */
    r_a1 = bl_lw(r_s4 + 0x4u); /* 800ACF74 lw $a1, 0x4($s4) */
    r_v1 = r_v1 + r_a0; /* 800ACF78 addu $v1, $v1, $a0 */
    bl_sw(r_a1 + 0x60u, r_v1); /* 800ACF7C sw $v1, 0x60($a1) */
    r_v1 = bl_lh(r_s4 + 0x8Eu); /* 800ACF80 lh $v1, 0x8E($s4) */
    ; /* 800ACF84 nop  */
    BL_MULT(r_s2, r_v1); /* 800ACF88 mult $s2, $v1 */
    r_v1 = lo; /* 800ACF8C mflo $v1 */
    ; /* 800ACF90 nop  */
    ; /* 800ACF94 nop  */
    BL_DIV(r_v1, r_v0); /* 800ACF98 div $zero, $v1, $v0 */
    /* 800ACF9C bnez $v0, .L800ACFA8 */
    { int c_ = (r_v0 != 0); ; if (c_) goto L_800ACFA8; }
    bl_fail(FN, "BREAK", 7u); /* 800ACFA4 break 7 */
L_800ACFA8:
    r_at = 0u + 0xFFFFFFFFu; /* 800ACFA8 addiu $at, $zero, -0x1 */
    /* 800ACFAC bne $v0, $at, .L800ACFC0 */
    { int c_ = (r_v0 != r_at); r_at = 0x80000000u; if (c_) goto L_800ACFC0; }
    /* 800ACFB4 bne $v1, $at, .L800ACFC0 */
    { int c_ = (r_v1 != r_at); ; if (c_) goto L_800ACFC0; }
    bl_fail(FN, "BREAK", 6u); /* 800ACFBC break 6 */
L_800ACFC0:
    r_v1 = lo; /* 800ACFC0 mflo $v1 */
    r_v0 = bl_lh(r_s4 + 0x8Cu); /* 800ACFC4 lh $v0, 0x8C($s4) */
    r_a0 = bl_lw(r_s4 + 0x4u); /* 800ACFC8 lw $a0, 0x4($s4) */
    r_v0 = r_v0 + r_v1; /* 800ACFCC addu $v0, $v0, $v1 */
    /* 800ACFD0 j .L800ADEBC */
    bl_sw(r_a0 + 0x64u, r_v0); goto L_800ADEBC;
L_800ACFD8:
    /* 800ACFD8 jal func_800AF438 */
    r_a2 = r_sp + 0x9Cu;
    bl_jal(FN, 0x800AF438u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800AF438 */
    r_v0 = r_v0 & 0xFFu; /* 800ACFE0 andi $v0, $v0, 0xFF */
    r_v1 = r_v0 << 3; /* 800ACFE4 sll $v1, $v0, 3 */
    r_v1 = r_v1 - r_v0; /* 800ACFE8 subu $v1, $v1, $v0 */
    r_v1 = r_v1 << 2; /* 800ACFEC sll $v1, $v1, 2 */
    r_a0 = bl_lw(r_s4 + 0x4u); /* 800ACFF0 lw $a0, 0x4($s4) */
    r_at = 0x800C0000u; /* 800ACFF4 lui $at, %hi(D_800C3EBE) */
    r_at = r_at + r_v1; /* 800ACFF8 addu $at, $at, $v1 */
    r_v0 = bl_lhu(r_at + 0x3EBEu); /* 800ACFFC lhu $v0, %lo(D_800C3EBE)($at) */
    ; /* 800AD000 nop  */
    bl_sw(r_a0 + 0x5Cu, r_v0); /* 800AD004 sw $v0, 0x5C($a0) */
    r_a0 = bl_lw(r_s4 + 0x4u); /* 800AD008 lw $a0, 0x4($s4) */
    r_at = 0x800C0000u; /* 800AD00C lui $at, %hi(D_800C3EC0) */
    r_at = r_at + r_v1; /* 800AD010 addu $at, $at, $v1 */
    r_v0 = bl_lhu(r_at + 0x3EC0u); /* 800AD014 lhu $v0, %lo(D_800C3EC0)($at) */
    /* 800AD018 j .L800ADEBC */
    bl_sw(r_a0 + 0x64u, r_v0); goto L_800ADEBC;
L_800AD020:
    r_v0 = bl_lhu(r_s3); /* 800AD020 lhu $v0, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800AD024 addiu $s3, $s3, 0x2 */
    bl_sh(r_s4 + 0x7Cu, r_v0); /* 800AD028 sh $v0, 0x7C($s4) */
    r_v0 = bl_lhu(r_s3); /* 800AD02C lhu $v0, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800AD030 addiu $s3, $s3, 0x2 */
    bl_sh(r_s4 + 0x7Eu, r_v0); /* 800AD034 sh $v0, 0x7E($s4) */
    r_v0 = bl_lhu(r_s3); /* 800AD038 lhu $v0, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800AD03C addiu $s3, $s3, 0x2 */
    /* 800AD040 j .L800ADEBC */
    bl_sh(r_s4 + 0x80u, r_v0); goto L_800ADEBC;
L_800AD048:
    r_v0 = bl_lhu(r_s4 + 0x7Cu); /* 800AD048 lhu $v0, 0x7C($s4) */
    r_v1 = bl_lhu(r_s3); /* 800AD04C lhu $v1, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800AD050 addiu $s3, $s3, 0x2 */
    r_v0 = r_v0 + r_v1; /* 800AD054 addu $v0, $v0, $v1 */
    bl_sh(r_s4 + 0x7Cu, r_v0); /* 800AD058 sh $v0, 0x7C($s4) */
    r_v0 = bl_lhu(r_s4 + 0x7Eu); /* 800AD05C lhu $v0, 0x7E($s4) */
    r_v1 = bl_lhu(r_s3); /* 800AD060 lhu $v1, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800AD064 addiu $s3, $s3, 0x2 */
    r_v0 = r_v0 + r_v1; /* 800AD068 addu $v0, $v0, $v1 */
    bl_sh(r_s4 + 0x7Eu, r_v0); /* 800AD06C sh $v0, 0x7E($s4) */
    r_v0 = bl_lhu(r_s4 + 0x80u); /* 800AD070 lhu $v0, 0x80($s4) */
    r_v1 = bl_lhu(r_s3); /* 800AD074 lhu $v1, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800AD078 addiu $s3, $s3, 0x2 */
    r_v0 = r_v0 + r_v1; /* 800AD07C addu $v0, $v0, $v1 */
    /* 800AD080 j .L800ADEBC */
    bl_sh(r_s4 + 0x80u, r_v0); goto L_800ADEBC;
L_800AD088:
    r_v0 = bl_lhu(r_s3); /* 800AD088 lhu $v0, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800AD08C addiu $s3, $s3, 0x2 */
    bl_sh(r_s4 + 0x82u, r_v0); /* 800AD090 sh $v0, 0x82($s4) */
    r_v0 = bl_lhu(r_s3); /* 800AD094 lhu $v0, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800AD098 addiu $s3, $s3, 0x2 */
    bl_sh(r_s4 + 0x84u, r_v0); /* 800AD09C sh $v0, 0x84($s4) */
    r_v0 = bl_lhu(r_s3); /* 800AD0A0 lhu $v0, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800AD0A4 addiu $s3, $s3, 0x2 */
    /* 800AD0A8 j .L800ADEBC */
    bl_sh(r_s4 + 0x86u, r_v0); goto L_800ADEBC;
L_800AD0B0:
    r_v0 = bl_lhu(r_s4 + 0x82u); /* 800AD0B0 lhu $v0, 0x82($s4) */
    r_v1 = bl_lhu(r_s3); /* 800AD0B4 lhu $v1, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800AD0B8 addiu $s3, $s3, 0x2 */
    r_v0 = r_v0 + r_v1; /* 800AD0BC addu $v0, $v0, $v1 */
    bl_sh(r_s4 + 0x82u, r_v0); /* 800AD0C0 sh $v0, 0x82($s4) */
    r_v0 = bl_lhu(r_s4 + 0x84u); /* 800AD0C4 lhu $v0, 0x84($s4) */
    r_v1 = bl_lhu(r_s3); /* 800AD0C8 lhu $v1, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800AD0CC addiu $s3, $s3, 0x2 */
    r_v0 = r_v0 + r_v1; /* 800AD0D0 addu $v0, $v0, $v1 */
    bl_sh(r_s4 + 0x84u, r_v0); /* 800AD0D4 sh $v0, 0x84($s4) */
    r_v0 = bl_lhu(r_s4 + 0x86u); /* 800AD0D8 lhu $v0, 0x86($s4) */
    r_v1 = bl_lhu(r_s3); /* 800AD0DC lhu $v1, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800AD0E0 addiu $s3, $s3, 0x2 */
    r_v0 = r_v0 + r_v1; /* 800AD0E4 addu $v0, $v0, $v1 */
    /* 800AD0E8 j .L800ADEBC */
    bl_sh(r_s4 + 0x86u, r_v0); goto L_800ADEBC;
L_800AD0F0:
    r_a0 = bl_lw(r_s4 + 0x4u); /* 800AD0F0 lw $a0, 0x4($s4) */
    r_v1 = bl_lh(r_s4 + 0x88u); /* 800AD0F4 lh $v1, 0x88($s4) */
    r_a2 = bl_lh(r_s4 + 0x8Au); /* 800AD0F8 lh $a2, 0x8A($s4) */
    r_v0 = bl_lw(r_a0 + 0x5Cu); /* 800AD0FC lw $v0, 0x5C($a0) */
    r_a1 = bl_lh(r_s4 + 0x8Cu); /* 800AD100 lh $a1, 0x8C($s4) */
    r_s1 = r_v1 - r_v0; /* 800AD104 subu $s1, $v1, $v0 */
    r_v1 = bl_lw(r_a0 + 0x60u); /* 800AD108 lw $v1, 0x60($a0) */
    r_v0 = bl_lw(r_a0 + 0x64u); /* 800AD10C lw $v0, 0x64($a0) */
    ; /* 800AD110 nop  */
    r_s0 = r_a1 - r_v0; /* 800AD114 subu $s0, $a1, $v0 */
    r_v0 = r_s6 & 0xFFu; /* 800AD118 andi $v0, $s6, 0xFF */
    /* 800AD11C bnez $v0, .L800AD128 */
    { int c_ = (r_v0 != 0); r_a2 = r_a2 - r_v1; if (c_) goto L_800AD128; }
    r_s6 = 0u | 0x1u; /* 800AD124 ori $s6, $zero, 0x1 */
L_800AD128:
    BL_MULT(r_s1, r_s1); /* 800AD128 mult $s1, $s1 */
    r_v0 = lo; /* 800AD12C mflo $v0 */
    ; /* 800AD130 nop  */
    ; /* 800AD134 nop  */
    BL_MULT(r_a2, r_a2); /* 800AD138 mult $a2, $a2 */
    r_v1 = lo; /* 800AD13C mflo $v1 */
    ; /* 800AD140 nop  */
    ; /* 800AD144 nop  */
    BL_MULT(r_s0, r_s0); /* 800AD148 mult $s0, $s0 */
    r_v0 = r_v0 + r_v1; /* 800AD14C addu $v0, $v0, $v1 */
    r_a0 = lo; /* 800AD150 mflo $a0 */
    /* 800AD154 jal SquareRoot0 */
    r_a0 = r_v0 + r_a0;
    bl_jal(FN, 0x80048C4Cu, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* SquareRoot0 */
    r_v1 = r_s6 & 0xFFu; /* 800AD15C andi $v1, $s6, 0xFF */
    BL_DIV(r_v0, r_v1); /* 800AD160 div $zero, $v0, $v1 */
    /* 800AD164 bnez $v1, .L800AD170 */
    { int c_ = (r_v1 != 0); ; if (c_) goto L_800AD170; }
    bl_fail(FN, "BREAK", 7u); /* 800AD16C break 7 */
L_800AD170:
    r_at = 0u + 0xFFFFFFFFu; /* 800AD170 addiu $at, $zero, -0x1 */
    /* 800AD174 bne $v1, $at, .L800AD188 */
    { int c_ = (r_v1 != r_at); r_at = 0x80000000u; if (c_) goto L_800AD188; }
    /* 800AD17C bne $v0, $at, .L800AD188 */
    { int c_ = (r_v0 != r_at); ; if (c_) goto L_800AD188; }
    bl_fail(FN, "BREAK", 6u); /* 800AD184 break 6 */
L_800AD188:
    r_v0 = lo; /* 800AD188 mflo $v0 */
    r_v1 = bl_lw(r_s4 + 0x4u); /* 800AD18C lw $v1, 0x4($s4) */
    r_a0 = bl_lh(r_s4 + 0x1Cu); /* 800AD190 lh $a0, 0x1C($s4) */
    r_v1 = bl_lh(r_v1 + 0x50u); /* 800AD194 lh $v1, 0x50($v1) */
    ; /* 800AD198 nop  */
    BL_MULT(r_a0, r_v1); /* 800AD19C mult $a0, $v1 */
    r_v1 = lo; /* 800AD1A0 mflo $v1 */
    r_v1 = BL_SRA(r_v1, 12); /* 800AD1A4 sra $v1, $v1, 12 */
    r_v0 = r_v0 << 12; /* 800AD1A8 sll $v0, $v0, 12 */
    BL_DIV(r_v0, r_v1); /* 800AD1AC div $zero, $v0, $v1 */
    /* 800AD1B0 bnez $v1, .L800AD1BC */
    { int c_ = (r_v1 != 0); ; if (c_) goto L_800AD1BC; }
    bl_fail(FN, "BREAK", 7u); /* 800AD1B8 break 7 */
L_800AD1BC:
    r_at = 0u + 0xFFFFFFFFu; /* 800AD1BC addiu $at, $zero, -0x1 */
    /* 800AD1C0 bne $v1, $at, .L800AD1D4 */
    { int c_ = (r_v1 != r_at); r_at = 0x80000000u; if (c_) goto L_800AD1D4; }
    /* 800AD1C8 bne $v0, $at, .L800AD1D4 */
    { int c_ = (r_v0 != r_at); ; if (c_) goto L_800AD1D4; }
    bl_fail(FN, "BREAK", 6u); /* 800AD1D0 break 6 */
L_800AD1D4:
    r_v0 = lo; /* 800AD1D4 mflo $v0 */
    r_a1 = 0u - r_s0; /* 800AD1D8 negu $a1, $s0 */
    r_a0 = 0u - r_s1; /* 800AD1DC negu $a0, $s1 */
    r_v1 = r_v0 >> 31; /* 800AD1E0 srl $v1, $v0, 31 */
    r_v0 = r_v0 + r_v1; /* 800AD1E4 addu $v0, $v0, $v1 */
    r_v0 = BL_SRA(r_v0, 1); /* 800AD1E8 sra $v0, $v0, 1 */
    /* 800AD1EC jal ratan2 */
    bl_sh(r_s4 + 0x80u, r_v0);
    bl_jal(FN, 0x8004B32Cu, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* ratan2 */
    r_v1 = bl_lw(r_s4 + 0x4u); /* 800AD1F4 lw $v1, 0x4($s4) */
    ; /* 800AD1F8 nop  */
    r_v1 = bl_lhu(r_v1 + 0x56u); /* 800AD1FC lhu $v1, 0x56($v1) */
    ; /* 800AD200 nop  */
    r_v0 = r_v0 - r_v1; /* 800AD204 subu $v0, $v0, $v1 */
    r_v0 = r_v0 + 0x400u; /* 800AD208 addiu $v0, $v0, 0x400 */
    r_v0 = r_v0 & 0xFFFu; /* 800AD20C andi $v0, $v0, 0xFFF */
    r_v0 = (u32)((s32)r_v0 < 2048); /* 800AD210 slti $v0, $v0, 0x800 */
    /* 800AD214 beqz $v0, .L800ADEBC */
    { int c_ = (r_v0 == 0); ; if (c_) goto L_800ADEBC; }
    r_v0 = bl_lhu(r_s4 + 0x80u); /* 800AD21C lhu $v0, 0x80($s4) */
    ; /* 800AD220 nop  */
    r_v0 = 0u - r_v0; /* 800AD224 negu $v0, $v0 */
    /* 800AD228 j .L800ADEBC */
    bl_sh(r_s4 + 0x80u, r_v0); goto L_800ADEBC;
L_800AD230:
    bl_sh(r_s4 + 0x58u, 0u); /* 800AD230 sh $zero, 0x58($s4) */
    r_v0 = bl_lhu(r_s3); /* 800AD234 lhu $v0, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800AD238 addiu $s3, $s3, 0x2 */
    bl_sh(r_s4 + 0x88u, r_v0); /* 800AD23C sh $v0, 0x88($s4) */
    r_v0 = bl_lhu(r_s3); /* 800AD240 lhu $v0, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800AD244 addiu $s3, $s3, 0x2 */
    bl_sh(r_s4 + 0x8Au, r_v0); /* 800AD248 sh $v0, 0x8A($s4) */
    r_v0 = bl_lhu(r_s3); /* 800AD24C lhu $v0, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800AD250 addiu $s3, $s3, 0x2 */
    /* 800AD254 j .L800ADEBC */
    bl_sh(r_s4 + 0x8Cu, r_v0); goto L_800ADEBC;
L_800AD25C:
    r_a0 = r_s4 + 0u; /* 800AD25C addu $a0, $s4, $zero */
    r_a1 = r_s6 & 0xFFu; /* 800AD260 andi $a1, $s6, 0xFF */
    r_a2 = r_sp + 0x9Cu; /* 800AD264 addiu $a2, $sp, 0x9C */
    /* 800AD268 jal func_800AF438 */
    bl_sh(r_s4 + 0x58u, 0u);
    bl_jal(FN, 0x800AF438u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800AF438 */
    r_v0 = r_v0 & 0xFFu; /* 800AD270 andi $v0, $v0, 0xFF */
    r_v1 = r_v0 << 3; /* 800AD274 sll $v1, $v0, 3 */
    r_v1 = r_v1 - r_v0; /* 800AD278 subu $v1, $v1, $v0 */
    r_v1 = r_v1 << 2; /* 800AD27C sll $v1, $v1, 2 */
    r_at = 0x800C0000u; /* 800AD280 lui $at, %hi(D_800C3EBE) */
    r_at = r_at + r_v1; /* 800AD284 addu $at, $at, $v1 */
    r_v0 = bl_lhu(r_at + 0x3EBEu); /* 800AD288 lhu $v0, %lo(D_800C3EBE)($at) */
    ; /* 800AD28C nop  */
    bl_sh(r_s4 + 0x88u, r_v0); /* 800AD290 sh $v0, 0x88($s4) */
    r_at = 0x800C0000u; /* 800AD294 lui $at, %hi(D_800C3EC2) */
    r_at = r_at + r_v1; /* 800AD298 addu $at, $at, $v1 */
    r_v0 = bl_lhu(r_at + 0x3EC2u); /* 800AD29C lhu $v0, %lo(D_800C3EC2)($at) */
    ; /* 800AD2A0 nop  */
    bl_sh(r_s4 + 0x8Au, r_v0); /* 800AD2A4 sh $v0, 0x8A($s4) */
    r_at = 0x800C0000u; /* 800AD2A8 lui $at, %hi(D_800C3EC0) */
    r_at = r_at + r_v1; /* 800AD2AC addu $at, $at, $v1 */
    r_v0 = bl_lhu(r_at + 0x3EC0u); /* 800AD2B0 lhu $v0, %lo(D_800C3EC0)($at) */
    /* 800AD2B4 j .L800ADEBC */
    bl_sh(r_s4 + 0x8Cu, r_v0); goto L_800ADEBC;
L_800AD2BC:
    r_v0 = r_s6 & 0xFFu; /* 800AD2BC andi $v0, $s6, 0xFF */
    bl_sh(r_s4 + 0x58u, r_v0); /* 800AD2C0 sh $v0, 0x58($s4) */
    r_v0 = bl_lhu(r_s3); /* 800AD2C4 lhu $v0, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800AD2C8 addiu $s3, $s3, 0x2 */
    bl_sh(r_s4 + 0x5Au, r_v0); /* 800AD2CC sh $v0, 0x5A($s4) */
    r_v0 = bl_lhu(r_s3); /* 800AD2D0 lhu $v0, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800AD2D4 addiu $s3, $s3, 0x2 */
    bl_sh(r_s4 + 0x64u, r_v0); /* 800AD2D8 sh $v0, 0x64($s4) */
    r_v0 = bl_lhu(r_s3); /* 800AD2DC lhu $v0, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800AD2E0 addiu $s3, $s3, 0x2 */
    bl_sh(r_s4 + 0x66u, r_v0); /* 800AD2E4 sh $v0, 0x66($s4) */
    r_v0 = bl_lhu(r_s3); /* 800AD2E8 lhu $v0, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800AD2EC addiu $s3, $s3, 0x2 */
    bl_sh(r_s4 + 0x68u, r_v0); /* 800AD2F0 sh $v0, 0x68($s4) */
    /* 800AD2F4 j .L800AD40C */
    r_v0 = r_s6 & 0xFFu; goto L_800AD40C;
L_800AD2FC:
    r_v0 = bl_lhu(r_s4 + 0x88u); /* 800AD2FC lhu $v0, 0x88($s4) */
    r_s0 = r_sp + 0x120u; /* 800AD300 addiu $s0, $sp, 0x120 */
    bl_sh(r_sp + 0x122u, 0u); /* 800AD304 sh $zero, 0x122($sp) */
    bl_sh(r_sp + 0x120u, r_v0); /* 800AD308 sh $v0, 0x120($sp) */
    r_v0 = bl_lhu(r_s4 + 0x8Cu); /* 800AD30C lhu $v0, 0x8C($s4) */
    r_a0 = r_s0 + 0u; /* 800AD310 addu $a0, $s0, $zero */
    /* 800AD314 jal func_800A579C */
    bl_sh(r_sp + 0x124u, r_v0);
    bl_jal(FN, 0x800A579Cu, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800A579C */
    r_a0 = r_s0 + 0u; /* 800AD31C addu $a0, $s0, $zero */
    r_a1 = r_v0 + 0u; /* 800AD320 addu $a1, $v0, $zero */
    /* 800AD324 jal func_800A5870 */
    r_a2 = r_sp + 0x110u;
    bl_jal(FN, 0x800A5870u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800A5870 */
    r_v0 = bl_lhu(r_sp + 0x122u); /* 800AD32C lhu $v0, 0x122($sp) */
    /* 800AD330 j .L800ADEBC */
    bl_sh(r_s4 + 0x8Au, r_v0); goto L_800ADEBC;
L_800AD338:
    r_v0 = bl_lw(r_s4 + 0x4u); /* 800AD338 lw $v0, 0x4($s4) */
    r_v1 = bl_lh(r_s4 + 0x1Cu); /* 800AD33C lh $v1, 0x1C($s4) */
    r_v0 = bl_lh(r_v0 + 0x50u); /* 800AD340 lh $v0, 0x50($v0) */
    ; /* 800AD344 nop  */
    BL_MULT(r_v1, r_v0); /* 800AD348 mult $v1, $v0 */
    r_v0 = lo; /* 800AD34C mflo $v0 */
    r_v1 = bl_lh(r_s3); /* 800AD350 lh $v1, 0x0($s3) */
    r_v0 = BL_SRA(r_v0, 12); /* 800AD354 sra $v0, $v0, 12 */
    BL_MULT(r_v1, r_v0); /* 800AD358 mult $v1, $v0 */
    r_s3 = r_s3 + 0x2u; /* 800AD35C addiu $s3, $s3, 0x2 */
    r_v0 = lo; /* 800AD360 mflo $v0 */
    r_v0 = BL_SRA(r_v0, 12); /* 800AD364 sra $v0, $v0, 12 */
    /* 800AD368 j .L800ADEBC */
    bl_sh(r_s4 + 0x8Eu, r_v0); goto L_800ADEBC;
L_800AD370:
    r_v0 = bl_lw(r_s4 + 0x4u); /* 800AD370 lw $v0, 0x4($s4) */
    r_v1 = bl_lh(r_s4 + 0x1Cu); /* 800AD374 lh $v1, 0x1C($s4) */
    r_v0 = bl_lh(r_v0 + 0x50u); /* 800AD378 lh $v0, 0x50($v0) */
    ; /* 800AD37C nop  */
    BL_MULT(r_v1, r_v0); /* 800AD380 mult $v1, $v0 */
    r_v0 = lo; /* 800AD384 mflo $v0 */
    r_v1 = bl_lh(r_s3); /* 800AD388 lh $v1, 0x0($s3) */
    r_v0 = BL_SRA(r_v0, 12); /* 800AD38C sra $v0, $v0, 12 */
    BL_MULT(r_v1, r_v0); /* 800AD390 mult $v1, $v0 */
    r_s3 = r_s3 + 0x2u; /* 800AD394 addiu $s3, $s3, 0x2 */
    r_v1 = bl_lhu(r_s4 + 0x8Eu); /* 800AD398 lhu $v1, 0x8E($s4) */
    r_v0 = lo; /* 800AD39C mflo $v0 */
    /* 800AD3A0 j .L800AD3E0 */
    r_v0 = BL_SRA(r_v0, 12); goto L_800AD3E0;
L_800AD3A8:
    r_v0 = bl_lhu(r_s4 + 0x8Eu); /* 800AD3A8 lhu $v0, 0x8E($s4) */
    r_v1 = bl_lhu(r_s3); /* 800AD3AC lhu $v1, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800AD3B0 addiu $s3, $s3, 0x2 */
    r_v0 = r_v0 + r_v1; /* 800AD3B4 addu $v0, $v0, $v1 */
    /* 800AD3B8 j .L800ADEBC */
    bl_sh(r_s4 + 0x8Eu, r_v0); goto L_800ADEBC;
L_800AD3C0:
    r_a0 = r_s4 + 0u; /* 800AD3C0 addu $a0, $s4, $zero */
    r_a1 = r_s6 & 0xFFu; /* 800AD3C4 andi $a1, $s6, 0xFF */
    /* 800AD3C8 jal func_800AF438 */
    r_a2 = r_sp + 0x9Cu;
    bl_jal(FN, 0x800AF438u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800AF438 */
    r_a0 = r_v0 & 0xFFu; /* 800AD3D0 andi $a0, $v0, 0xFF */
    /* 800AD3D4 jal func_800AA650 */
    bl_sw(r_sp + 0x98u, r_a0);
    bl_jal(FN, 0x800AA650u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800AA650 */
    r_v1 = bl_lhu(r_s4 + 0x8Eu); /* 800AD3DC lhu $v1, 0x8E($s4) */
L_800AD3E0:
    ; /* 800AD3E0 nop  */
    r_v1 = r_v1 + r_v0; /* 800AD3E4 addu $v1, $v1, $v0 */
    /* 800AD3E8 j .L800ADEBC */
    bl_sh(r_s4 + 0x8Eu, r_v1); goto L_800ADEBC;
L_800AD3F0:
    r_v0 = r_s6 & 0xFFu; /* 800AD3F0 andi $v0, $s6, 0xFF */
    bl_sh(r_s4 + 0x58u, r_v0); /* 800AD3F4 sh $v0, 0x58($s4) */
    r_v0 = r_s6 & 0xFFu; /* 800AD3F8 andi $v0, $s6, 0xFF */
    bl_sh(r_s4 + 0x5Au, 0u); /* 800AD3FC sh $zero, 0x5A($s4) */
    bl_sh(r_s4 + 0x64u, 0u); /* 800AD400 sh $zero, 0x64($s4) */
    bl_sh(r_s4 + 0x66u, 0u); /* 800AD404 sh $zero, 0x66($s4) */
    bl_sh(r_s4 + 0x68u, 0u); /* 800AD408 sh $zero, 0x68($s4) */
L_800AD40C:
    /* 800AD40C beqz $v0, .L800ADEBC */
    { int c_ = (r_v0 == 0); ; if (c_) goto L_800ADEBC; }
    /* 800AD414 jal func_800AEF68 */
    r_a0 = r_s4 + 0u;
    bl_jal(FN, 0x800AEF68u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800AEF68 */
    /* 800AD41C j .L800ADEBC */
    ; goto L_800ADEBC;
L_800AD424:
    r_v1 = r_s6 & 0xFFu; /* 800AD424 andi $v1, $s6, 0xFF */
    r_v0 = r_v1 << 1; /* 800AD428 sll $v0, $v1, 1 */
    r_v0 = r_v0 + r_v1; /* 800AD42C addu $v0, $v0, $v1 */
    r_v1 = 0x80060000u; /* 800AD430 lui $v1, %hi(D_800658C8) */
    r_v1 = bl_lw(r_v1 + 0x58C8u); /* 800AD434 lw $v1, %lo(D_800658C8)($v1) */
    r_v0 = r_v0 << 2; /* 800AD438 sll $v0, $v0, 2 */
    bl_sh(r_s4 + 0x58u, 0u); /* 800AD43C sh $zero, 0x58($s4) */
    r_v1 = r_v1 + r_v0; /* 800AD440 addu $v1, $v1, $v0 */
    r_v0 = bl_lhu(r_v1 + 0x482u); /* 800AD444 lhu $v0, 0x482($v1) */
    ; /* 800AD448 nop  */
    bl_sh(r_s4 + 0x88u, r_v0); /* 800AD44C sh $v0, 0x88($s4) */
    r_v0 = bl_lhu(r_v1 + 0x484u); /* 800AD450 lhu $v0, 0x484($v1) */
    ; /* 800AD454 nop  */
    bl_sh(r_s4 + 0x8Au, r_v0); /* 800AD458 sh $v0, 0x8A($s4) */
    r_v0 = bl_lhu(r_v1 + 0x486u); /* 800AD45C lhu $v0, 0x486($v1) */
    /* 800AD460 j .L800ADEBC */
    bl_sh(r_s4 + 0x8Cu, r_v0); goto L_800ADEBC;
L_800AD468:
    r_v1 = r_s6 & 0xFFu; /* 800AD468 andi $v1, $s6, 0xFF */
    r_v0 = r_v1 << 1; /* 800AD46C sll $v0, $v1, 1 */
    r_v0 = r_v0 + r_v1; /* 800AD470 addu $v0, $v0, $v1 */
    r_v1 = 0x80060000u; /* 800AD474 lui $v1, %hi(D_800658C8) */
    r_v1 = bl_lw(r_v1 + 0x58C8u); /* 800AD478 lw $v1, %lo(D_800658C8)($v1) */
    r_v0 = r_v0 << 2; /* 800AD47C sll $v0, $v0, 2 */
    bl_sh(r_s4 + 0x58u, 0u); /* 800AD480 sh $zero, 0x58($s4) */
    r_v1 = r_v1 + r_v0; /* 800AD484 addu $v1, $v1, $v0 */
    r_v0 = bl_lhu(r_v1 + 0x47Cu); /* 800AD488 lhu $v0, 0x47C($v1) */
    ; /* 800AD48C nop  */
    bl_sh(r_s4 + 0x88u, r_v0); /* 800AD490 sh $v0, 0x88($s4) */
    r_v0 = bl_lhu(r_v1 + 0x47Eu); /* 800AD494 lhu $v0, 0x47E($v1) */
    ; /* 800AD498 nop  */
    bl_sh(r_s4 + 0x8Au, r_v0); /* 800AD49C sh $v0, 0x8A($s4) */
    r_v0 = bl_lhu(r_v1 + 0x480u); /* 800AD4A0 lhu $v0, 0x480($v1) */
    /* 800AD4A4 j .L800ADEBC */
    bl_sh(r_s4 + 0x8Cu, r_v0); goto L_800ADEBC;
L_800AD4AC:
    r_v1 = r_s6 & 0xFFu; /* 800AD4AC andi $v1, $s6, 0xFF */
    r_v0 = bl_lbu(r_s4 + 0x2Bu); /* 800AD4B0 lbu $v0, 0x2B($s4) */
    r_t8 = 0u | 0x1u; /* 800AD4B4 ori $t8, $zero, 0x1 */
    /* 800AD4B8 bne $v1, $t8, .L800AD4CC */
    { int c_ = (r_v1 != r_t8); bl_sh(r_sp + 0x9Cu, r_v0); if (c_) goto L_800AD4CC; }
    r_v0 = (u32)((s32)r_v0 < 2); /* 800AD4C0 slti $v0, $v0, 0x2 */
    /* 800AD4C4 beqz $v0, .L800ADEBC */
    { int c_ = (r_v0 == 0); ; if (c_) goto L_800ADEBC; }
L_800AD4CC:
    r_v0 = 0u | 0x2u; /* 800AD4CC ori $v0, $zero, 0x2 */
    /* 800AD4D0 bne $v1, $v0, .L800ADEBC */
    { int c_ = (r_v1 != r_v0); bl_sb(r_s4 + 0x2Bu, r_s6); if (c_) goto L_800ADEBC; }
L_800AD4D8:
    bl_sb(r_s4 + 0x2Bu, 0u); /* 800AD4D8 sb $zero, 0x2B($s4) */
    r_v1 = bl_lh(r_sp + 0x9Cu); /* 800AD4DC lh $v1, 0x9C($sp) */
    ; /* 800AD4E0 nop  */
    r_v0 = (u32)((s32)r_v1 < 2); /* 800AD4E4 slti $v0, $v1, 0x2 */
    /* 800AD4E8 bnez $v0, .L800ADEBC */
    { int c_ = (r_v0 != 0); r_t7 = 0u | 0x1u; if (c_) goto L_800ADEBC; }
    r_v0 = (u32)((s32)r_t7 < (s32)r_v1); /* 800AD4F0 slt $v0, $t7, $v1 */
    /* 800AD4F4 beqz $v0, .L800ADEE8 */
    { int c_ = (r_v0 == 0); bl_sw(r_sp + 0x98u, r_t7); if (c_) goto L_800ADEE8; }
L_800AD4FC:
    r_t8 = 0x800D0000u; /* 800AD4FC lui $t8, %hi(D_800D3368) */
    r_t8 = r_t8 + 0x3368u; /* 800AD500 addiu $t8, $t8, %lo(D_800D3368) */
    r_v1 = bl_lw(r_sp + 0x98u); /* 800AD504 lw $v1, 0x98($sp) */
    r_a2 = bl_lw(r_sp + 0x130u); /* 800AD508 lw $a2, 0x130($sp) */
    r_v1 = r_s4 + r_v1; /* 800AD50C addu $v1, $s4, $v1 */
    r_v0 = bl_lbu(r_v1 + 0x2Bu); /* 800AD510 lbu $v0, 0x2B($v1) */
    r_a3 = bl_lbu(r_v1 + 0x2Fu); /* 800AD514 lbu $a3, 0x2F($v1) */
    r_v0 = r_v0 << 2; /* 800AD518 sll $v0, $v0, 2 */
    r_v0 = r_v0 + r_t8; /* 800AD51C addu $v0, $v0, $t8 */
    r_a1 = bl_lw(r_v0); /* 800AD520 lw $a1, 0x0($v0) */
    /* 800AD524 jal func_800AA934 */
    r_a0 = r_s4 + 0u;
    bl_jal(FN, 0x800AA934u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800AA934 */
    r_v0 = bl_lw(r_sp + 0x98u); /* 800AD52C lw $v0, 0x98($sp) */
    r_v1 = bl_lh(r_sp + 0x9Cu); /* 800AD530 lh $v1, 0x9C($sp) */
    r_v0 = r_v0 + 0x1u; /* 800AD534 addiu $v0, $v0, 0x1 */
    bl_sw(r_sp + 0x98u, r_v0); /* 800AD538 sw $v0, 0x98($sp) */
    r_v0 = (u32)((s32)r_v0 < (s32)r_v1); /* 800AD53C slt $v0, $v0, $v1 */
    /* 800AD540 beqz $v0, .L800ADEE8 */
    { int c_ = (r_v0 == 0); ; if (c_) goto L_800ADEE8; }
    /* 800AD548 j .L800AD4FC */
    ; goto L_800AD4FC;
L_800AD550:
    r_a1 = bl_lhu(r_s3); /* 800AD550 lhu $a1, 0x0($s3) */
    r_a0 = bl_lw(r_s4 + 0x4u); /* 800AD554 lw $a0, 0x4($s4) */
    r_v1 = bl_lh(r_s4 + 0x88u); /* 800AD558 lh $v1, 0x88($s4) */
    r_v0 = bl_lw(r_a0 + 0x5Cu); /* 800AD55C lw $v0, 0x5C($a0) */
    r_s3 = r_s3 + 0x2u; /* 800AD560 addiu $s3, $s3, 0x2 */
    /* 800AD564 bne $v1, $v0, .L800ADEBC */
    { int c_ = (r_v1 != r_v0); bl_sh(r_sp + 0x9Cu, r_a1); if (c_) goto L_800ADEBC; }
    r_v1 = bl_lh(r_s4 + 0x8Au); /* 800AD56C lh $v1, 0x8A($s4) */
    r_v0 = bl_lw(r_a0 + 0x60u); /* 800AD570 lw $v0, 0x60($a0) */
    ; /* 800AD574 nop  */
    /* 800AD578 bne $v1, $v0, .L800ADEBC */
    { int c_ = (r_v1 != r_v0); ; if (c_) goto L_800ADEBC; }
    r_v1 = bl_lh(r_s4 + 0x8Cu); /* 800AD580 lh $v1, 0x8C($s4) */
    r_v0 = bl_lw(r_a0 + 0x64u); /* 800AD584 lw $v0, 0x64($a0) */
    ; /* 800AD588 nop  */
    /* 800AD58C bne $v1, $v0, .L800ADEBC */
    { int c_ = (r_v1 != r_v0); r_v0 = r_a1 << 16; if (c_) goto L_800ADEBC; }
L_800AD594:
    r_v0 = BL_SRA(r_v0, 16); /* 800AD594 sra $v0, $v0, 16 */
    /* 800AD598 j .L800ADEBC */
    r_s3 = r_s5 + r_v0; goto L_800ADEBC;
L_800AD5A0:
    r_v0 = bl_lhu(r_s3); /* 800AD5A0 lhu $v0, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800AD5A4 addiu $s3, $s3, 0x2 */
    bl_sh(r_sp + 0x9Cu, r_v0); /* 800AD5A8 sh $v0, 0x9C($sp) */
    r_v0 = r_v0 << 16; /* 800AD5AC sll $v0, $v0, 16 */
    r_v0 = BL_SRA(r_v0, 16); /* 800AD5B0 sra $v0, $v0, 16 */
    r_v1 = r_v0 << 5; /* 800AD5B4 sll $v1, $v0, 5 */
    r_v1 = r_v1 - r_v0; /* 800AD5B8 subu $v1, $v1, $v0 */
    r_v0 = bl_lw(r_s4 + 0x4u); /* 800AD5BC lw $v0, 0x4($s4) */
    r_v1 = r_v1 << 2; /* 800AD5C0 sll $v1, $v1, 2 */
    r_v1 = r_v1 + r_v0; /* 800AD5C4 addu $v1, $v1, $v0 */
    r_v0 = r_s6 & 0xFFu; /* 800AD5C8 andi $v0, $s6, 0xFF */
    /* 800AD5CC j .L800ADEBC */
    bl_sh(r_v1 + 0x52u, r_v0); goto L_800ADEBC;
L_800AD5D4:
    r_v0 = bl_lhu(r_s3); /* 800AD5D4 lhu $v0, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800AD5D8 addiu $s3, $s3, 0x2 */
    bl_sh(r_sp + 0x9Cu, r_v0); /* 800AD5DC sh $v0, 0x9C($sp) */
    /* 800AD5E0 j .L800ADEBC */
    bl_sh(r_s4 + 0x1Cu, r_v0); goto L_800ADEBC;
L_800AD5E8:
    r_v0 = bl_lhu(r_s3); /* 800AD5E8 lhu $v0, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800AD5EC addiu $s3, $s3, 0x2 */
    bl_sh(r_sp + 0x9Cu, r_v0); /* 800AD5F0 sh $v0, 0x9C($sp) */
    /* 800AD5F4 j .L800ADEBC */
    bl_sh(r_s4 + 0x4Au, r_v0); goto L_800ADEBC;
L_800AD5FC:
    r_v1 = bl_lbu(r_s4 + 0x20u); /* 800AD5FC lbu $v1, 0x20($s4) */
    ; /* 800AD600 nop  */
    r_v0 = r_v1 << 3; /* 800AD604 sll $v0, $v1, 3 */
    r_v0 = r_v0 - r_v1; /* 800AD608 subu $v0, $v0, $v1 */
    r_v0 = r_v0 << 2; /* 800AD60C sll $v0, $v0, 2 */
    r_at = 0x800C0000u; /* 800AD610 lui $at, %hi(D_800C3EB4) */
    r_at = r_at + r_v0; /* 800AD614 addu $at, $at, $v0 */
    r_v0 = bl_lbu(r_at + 0x3EB4u); /* 800AD618 lbu $v0, %lo(D_800C3EB4)($at) */
    r_v1 = 0x80060000u; /* 800AD61C lui $v1, %hi(D_800658C8) */
    r_v1 = bl_lw(r_v1 + 0x58C8u); /* 800AD620 lw $v1, %lo(D_800658C8)($v1) */
    r_v0 = r_v0 << 3; /* 800AD624 sll $v0, $v0, 3 */
    r_v1 = r_v1 + r_v0; /* 800AD628 addu $v1, $v1, $v0 */
    r_v0 = bl_lhu(r_v1 + 0x100u); /* 800AD62C lhu $v0, 0x100($v1) */
    r_a0 = bl_lhu(r_v1 + 0x104u); /* 800AD630 lhu $a0, 0x104($v1) */
    bl_sh(r_s4 + 0x8Au, 0u); /* 800AD634 sh $zero, 0x8A($s4) */
    r_v0 = r_v0 + r_a0; /* 800AD638 addu $v0, $v0, $a0 */
    r_v0 = BL_SRA(r_v0, 1); /* 800AD63C sra $v0, $v0, 1 */
    bl_sh(r_s4 + 0x88u, r_v0); /* 800AD640 sh $v0, 0x88($s4) */
    r_v0 = bl_lhu(r_v1 + 0x102u); /* 800AD644 lhu $v0, 0x102($v1) */
    r_v1 = bl_lhu(r_v1 + 0x106u); /* 800AD648 lhu $v1, 0x106($v1) */
    ; /* 800AD64C nop  */
    r_v0 = r_v0 + r_v1; /* 800AD650 addu $v0, $v0, $v1 */
    r_v0 = BL_SRA(r_v0, 1); /* 800AD654 sra $v0, $v0, 1 */
    /* 800AD658 j .L800ADEBC */
    bl_sh(r_s4 + 0x8Cu, r_v0); goto L_800ADEBC;
L_800AD660:
    r_v0 = bl_lhu(r_s3); /* 800AD660 lhu $v0, 0x0($s3) */
    ; /* 800AD664 nop  */
    bl_sh(r_sp + 0x9Cu, r_v0); /* 800AD668 sh $v0, 0x9C($sp) */
    r_a0 = bl_lbu(r_s4 + 0x20u); /* 800AD66C lbu $a0, 0x20($s4) */
    /* 800AD670 jal func_800885D0 */
    r_s3 = r_s3 + 0x2u;
    bl_jal(FN, 0x800885D0u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800885D0 */
    r_v0 = r_v0 & 0xFFu; /* 800AD678 andi $v0, $v0, 0xFF */
    /* 800AD67C beqz $v0, .L800ADEBC */
    { int c_ = (r_v0 == 0); ; if (c_) goto L_800ADEBC; }
    /* 800AD684 j .L800ADEA8 */
    ; goto L_800ADEA8;
L_800AD68C:
    r_t0 = bl_lh(r_s3); /* 800AD68C lh $t0, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800AD690 addiu $s3, $s3, 0x2 */
    r_a3 = bl_lh(r_s3); /* 800AD694 lh $a3, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800AD698 addiu $s3, $s3, 0x2 */
    r_v0 = bl_lh(r_s3); /* 800AD69C lh $v0, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800AD6A0 addiu $s3, $s3, 0x2 */
    r_a0 = r_s4 + 0u; /* 800AD6A4 addu $a0, $s4, $zero */
    r_v1 = bl_lw(r_s4 + 0x4u); /* 800AD6A8 lw $v1, 0x4($s4) */
    r_a2 = r_s6 & 0xFFu; /* 800AD6AC andi $a2, $s6, 0xFF */
    bl_sw(r_sp + 0x10u, r_v0); /* 800AD6B0 sw $v0, 0x10($sp) */
    r_v0 = bl_lh(r_s3); /* 800AD6B4 lh $v0, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800AD6B8 addiu $s3, $s3, 0x2 */
    r_a1 = r_t0 << 5; /* 800AD6BC sll $a1, $t0, 5 */
    r_a1 = r_a1 - r_t0; /* 800AD6C0 subu $a1, $a1, $t0 */
    r_a1 = r_a1 << 2; /* 800AD6C4 sll $a1, $a1, 2 */
    r_a1 = r_a1 + r_v1; /* 800AD6C8 addu $a1, $a1, $v1 */
    /* 800AD6CC jal func_800AFD98 */
    bl_sw(r_sp + 0x14u, r_v0);
    bl_jal(FN, 0x800AFD98u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800AFD98 */
    /* 800AD6D4 j .L800ADEBC */
    ; goto L_800ADEBC;
L_800AD6DC:
    r_v1 = bl_lhu(r_s3); /* 800AD6DC lhu $v1, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800AD6E0 addiu $s3, $s3, 0x2 */
    r_v0 = 0u + 0xFFFFFFFFu; /* 800AD6E4 addiu $v0, $zero, -0x1 */
    bl_sh(r_s4 + 0x9Au, r_v0); /* 800AD6E8 sh $v0, 0x9A($s4) */
    r_v0 = r_s6 & 0xFFu; /* 800AD6EC andi $v0, $s6, 0xFF */
    bl_sh(r_s4 + 0x98u, 0u); /* 800AD6F0 sh $zero, 0x98($s4) */
    bl_sh(r_s4 + 0x9Cu, 0u); /* 800AD6F4 sh $zero, 0x9C($s4) */
    bl_sh(r_s4 + 0x9Eu, r_v0); /* 800AD6F8 sh $v0, 0x9E($s4) */
    bl_sw(r_s4 + 0xA0u, r_s3); /* 800AD6FC sw $s3, 0xA0($s4) */
    r_v0 = r_v1 << 16; /* 800AD700 sll $v0, $v1, 16 */
L_800AD704:
    r_v0 = BL_SRA(r_v0, 16); /* 800AD704 sra $v0, $v0, 16 */
    r_s3 = r_s5 + r_v0; /* 800AD708 addu $s3, $s5, $v0 */
    /* 800AD70C j .L800ADEBC */
    bl_sh(r_sp + 0x9Cu, r_v1); goto L_800ADEBC;
L_800AD714:
    r_v0 = bl_lhu(r_s3); /* 800AD714 lhu $v0, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800AD718 addiu $s3, $s3, 0x2 */
    /* 800AD71C j .L800ADEBC */
    bl_sh(r_s4 + 0x3Eu, r_v0); goto L_800ADEBC;
L_800AD724:
    r_v0 = bl_lhu(r_s3); /* 800AD724 lhu $v0, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800AD728 addiu $s3, $s3, 0x2 */
    r_v1 = r_fp & 0xFFu; /* 800AD72C andi $v1, $fp, 0xFF */
    r_s2 = r_v0 >> 8; /* 800AD730 srl $s2, $v0, 8 */
    bl_sh(r_sp + 0x9Cu, r_v0); /* 800AD734 sh $v0, 0x9C($sp) */
    r_v0 = bl_lhu(r_s3); /* 800AD738 lhu $v0, 0x0($s3) */
    r_s1 = bl_lbu(r_sp + 0x9Cu); /* 800AD73C lbu $s1, 0x9C($sp) */
    r_a3 = r_v0 >> 8; /* 800AD740 srl $a3, $v0, 8 */
    bl_sh(r_sp + 0x9Cu, r_v0); /* 800AD744 sh $v0, 0x9C($sp) */
    r_v0 = 0u | 0x65u; /* 800AD748 ori $v0, $zero, 0x65 */
    r_s7 = bl_lbu(r_sp + 0x9Cu); /* 800AD74C lbu $s7, 0x9C($sp) */
    /* 800AD750 bne $v1, $v0, .L800AD778 */
    { int c_ = (r_v1 != r_v0); r_s3 = r_s3 + 0x2u; if (c_) goto L_800AD778; }
    r_t7 = 0x800D0000u; /* 800AD758 lui $t7, %hi(D_800D335E) */
    r_t7 = bl_lhu(r_t7 + 0x335Eu); /* 800AD75C lhu $t7, %lo(D_800D335E)($t7) */
    r_s5 = 0x800D0000u; /* 800AD760 lui $s5, %hi(D_800D335C) */
    r_s5 = bl_lhu(r_s5 + 0x335Cu); /* 800AD764 lhu $s5, %lo(D_800D335C)($s5) */
    r_t8 = 0x800D0000u; /* 800AD768 lui $t8, %hi(D_800D3360) */
    r_t8 = bl_lhu(r_t8 + 0x3360u); /* 800AD76C lhu $t8, %lo(D_800D3360)($t8) */
    /* 800AD770 j .L800AD794 */
    bl_sh(r_sp + 0x1A0u, r_t7); goto L_800AD794;
L_800AD778:
    r_t7 = 0x800D0000u; /* 800AD778 lui $t7, %hi(D_800D3356) */
    r_t7 = bl_lhu(r_t7 + 0x3356u); /* 800AD77C lhu $t7, %lo(D_800D3356)($t7) */
    r_s5 = 0x800D0000u; /* 800AD780 lui $s5, %hi(D_800D3354) */
    r_s5 = bl_lhu(r_s5 + 0x3354u); /* 800AD784 lhu $s5, %lo(D_800D3354)($s5) */
    r_t8 = 0x800D0000u; /* 800AD788 lui $t8, %hi(D_800D3358) */
    r_t8 = bl_lhu(r_t8 + 0x3358u); /* 800AD78C lhu $t8, %lo(D_800D3358)($t8) */
    bl_sh(r_sp + 0x1A0u, r_t7); /* 800AD790 sh $t7, 0x1A0($sp) */
L_800AD794:
    bl_sh(r_sp + 0x1A8u, r_t8); /* 800AD794 sh $t8, 0x1A8($sp) */
    r_a0 = r_s4 + 0u; /* 800AD798 addu $a0, $s4, $zero */
    r_s0 = r_s2 & 0xFFu; /* 800AD79C andi $s0, $s2, 0xFF */
    r_a1 = r_s0 + 0u; /* 800AD7A0 addu $a1, $s0, $zero */
    r_a2 = r_sp + 0x128u; /* 800AD7A4 addiu $a2, $sp, 0x128 */
    /* 800AD7A8 jal func_800AF438 */
    bl_sw(r_sp + 0x1C8u, r_a3);
    bl_jal(FN, 0x800AF438u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800AF438 */
    r_s2 = r_v0 & 0xFFu; /* 800AD7B0 andi $s2, $v0, 0xFF */
    r_a1 = bl_lhu(r_s3); /* 800AD7B4 lhu $a1, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800AD7B8 addiu $s3, $s3, 0x2 */
    r_a2 = 0u | 0x2u; /* 800AD7BC ori $a2, $zero, 0x2 */
    r_a3 = bl_lw(r_sp + 0x1C8u); /* 800AD7C0 lw $a3, 0x1C8($sp) */
    r_v0 = 0u | 0xF6u; /* 800AD7C4 ori $v0, $zero, 0xF6 */
    /* 800AD7C8 bne $s0, $v0, .L800AD7E4 */
    { int c_ = (r_s0 != r_v0); bl_sh(r_sp + 0x128u, r_a1); if (c_) goto L_800AD7E4; }
    r_s2 = bl_lhu(r_s4 + 0x88u); /* 800AD7D0 lhu $s2, 0x88($s4) */
    r_v0 = bl_lhu(r_s4 + 0x8Au); /* 800AD7D4 lhu $v0, 0x8A($s4) */
    r_v1 = bl_lhu(r_s4 + 0x8Cu); /* 800AD7D8 lhu $v1, 0x8C($s4) */
    /* 800AD7DC j .L800AD84C */
    r_v0 = r_a1 + r_v0; goto L_800AD84C;
L_800AD7E4:
    r_v0 = 0u | 0xF5u; /* 800AD7E4 ori $v0, $zero, 0xF5 */
    /* 800AD7E8 bne $s0, $v0, .L800AD81C */
    { int c_ = (r_s0 != r_v0); r_v0 = 0u | 0xF4u; if (c_) goto L_800AD81C; }
    r_v0 = r_s7 << 1; /* 800AD7F0 sll $v0, $s7, 1 */
    r_v0 = r_v0 + r_s7; /* 800AD7F4 addu $v0, $v0, $s7 */
    r_v1 = 0x80060000u; /* 800AD7F8 lui $v1, %hi(D_800658C8) */
    r_v1 = bl_lw(r_v1 + 0x58C8u); /* 800AD7FC lw $v1, %lo(D_800658C8)($v1) */
    r_v0 = r_v0 << 2; /* 800AD800 sll $v0, $v0, 2 */
    r_v1 = r_v1 + r_v0; /* 800AD804 addu $v1, $v1, $v0 */
    r_s2 = bl_lhu(r_v1 + 0x47Cu); /* 800AD808 lhu $s2, 0x47C($v1) */
    r_v0 = bl_lhu(r_v1 + 0x47Eu); /* 800AD80C lhu $v0, 0x47E($v1) */
    r_v1 = bl_lhu(r_v1 + 0x480u); /* 800AD810 lhu $v1, 0x480($v1) */
    /* 800AD814 j .L800AD84C */
    r_v0 = r_a1 + r_v0; goto L_800AD84C;
L_800AD81C:
    /* 800AD81C bne $s0, $v0, .L800AD854 */
    { int c_ = (r_s0 != r_v0); r_v0 = r_s2 << 16; if (c_) goto L_800AD854; }
    r_v0 = r_s7 << 1; /* 800AD824 sll $v0, $s7, 1 */
    r_v0 = r_v0 + r_s7; /* 800AD828 addu $v0, $v0, $s7 */
    r_v1 = 0x80060000u; /* 800AD82C lui $v1, %hi(D_800658C8) */
    r_v1 = bl_lw(r_v1 + 0x58C8u); /* 800AD830 lw $v1, %lo(D_800658C8)($v1) */
    r_v0 = r_v0 << 2; /* 800AD834 sll $v0, $v0, 2 */
    r_v1 = r_v1 + r_v0; /* 800AD838 addu $v1, $v1, $v0 */
    r_s2 = bl_lhu(r_v1 + 0x482u); /* 800AD83C lhu $s2, 0x482($v1) */
    r_v0 = bl_lhu(r_v1 + 0x484u); /* 800AD840 lhu $v0, 0x484($v1) */
    r_v1 = bl_lhu(r_v1 + 0x486u); /* 800AD844 lhu $v1, 0x486($v1) */
    r_v0 = r_a1 + r_v0; /* 800AD848 addu $v0, $a1, $v0 */
L_800AD84C:
    /* 800AD84C j .L800AD910 */
    bl_sh(r_sp + 0x128u, r_v0); goto L_800AD910;
L_800AD854:
    r_a0 = BL_SRA(r_v0, 16); /* 800AD854 sra $a0, $v0, 16 */
    r_v0 = r_a0 << 2; /* 800AD858 sll $v0, $a0, 2 */
    r_t7 = 0x800D0000u; /* 800AD85C lui $t7, %hi(D_800D3368) */
    r_t7 = r_t7 + 0x3368u; /* 800AD860 addiu $t7, $t7, %lo(D_800D3368) */
    r_v0 = r_v0 + r_t7; /* 800AD864 addu $v0, $v0, $t7 */
    r_v0 = bl_lw(r_v0); /* 800AD868 lw $v0, 0x0($v0) */
    ; /* 800AD86C nop  */
    /* 800AD870 beqz $v0, .L800AD8F4 */
    { int c_ = (r_v0 == 0); r_v0 = r_a1 << 16; if (c_) goto L_800AD8F4; }
    /* 800AD878 jal func_800AA600 */
    bl_sw(r_sp + 0x1C8u, r_a3);
    bl_jal(FN, 0x800AA600u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800AA600 */
    r_v1 = bl_lh(r_sp + 0x128u); /* 800AD880 lh $v1, 0x128($sp) */
    ; /* 800AD884 nop  */
    BL_MULT(r_v0, r_v1); /* 800AD888 mult $v0, $v1 */
    r_a3 = bl_lw(r_sp + 0x1C8u); /* 800AD88C lw $a3, 0x1C8($sp) */
    r_v0 = lo; /* 800AD890 mflo $v0 */
    /* 800AD894 bgez $v0, .L800AD8A0 */
    { int c_ = ((s32)r_v0 >= 0); r_a2 = 0u + 0u; if (c_) goto L_800AD8A0; }
    r_v0 = r_v0 + 0xFFFu; /* 800AD89C addiu $v0, $v0, 0xFFF */
L_800AD8A0:
    r_v0 = BL_SRA(r_v0, 12); /* 800AD8A0 sra $v0, $v0, 12 */
    bl_sh(r_sp + 0x128u, r_v0); /* 800AD8A4 sh $v0, 0x128($sp) */
    r_v0 = 0u | 0xF9u; /* 800AD8A8 ori $v0, $zero, 0xF9 */
    /* 800AD8AC bne $s0, $v0, .L800AD910 */
    { int c_ = (r_s0 != r_v0); r_v1 = 0u + 0xFFFFFFFFu; if (c_) goto L_800AD910; }
    r_v1 = 0u + 0u; /* 800AD8B4 addu $v1, $zero, $zero */
    r_a0 = 0x800C0000u; /* 800AD8B8 lui $a0, %hi(D_800C3E30) */
    r_a0 = bl_lhu(r_a0 + 0x3E30u); /* 800AD8BC lhu $a0, %lo(D_800C3E30)($a0) */
    ; /* 800AD8C0 nop  */
    r_v0 = BL_SRA(r_a0, r_v1); /* 800AD8C4 srav $v0, $a0, $v1 */
L_800AD8C8:
    r_v0 = r_v0 & 0x1u; /* 800AD8C8 andi $v0, $v0, 0x1 */
    /* 800AD8CC bnez $v0, .L800AD910 */
    { int c_ = (r_v0 != 0); r_v0 = r_v1 + 0x1u; if (c_) goto L_800AD910; }
    r_v1 = r_v0 + 0u; /* 800AD8D4 addu $v1, $v0, $zero */
    r_v0 = r_v0 << 16; /* 800AD8D8 sll $v0, $v0, 16 */
    r_v0 = BL_SRA(r_v0, 16); /* 800AD8DC sra $v0, $v0, 16 */
    r_v0 = (u32)((s32)r_v0 < 13); /* 800AD8E0 slti $v0, $v0, 0xD */
    /* 800AD8E4 bnez $v0, .L800AD8C8 */
    { int c_ = (r_v0 != 0); r_v0 = BL_SRA(r_a0, r_v1); if (c_) goto L_800AD8C8; }
    /* 800AD8EC j .L800AD914 */
    r_t0 = r_a3 & 0xFFu; goto L_800AD914;
L_800AD8F4:
    r_v0 = BL_SRA(r_v0, 16); /* 800AD8F4 sra $v0, $v0, 16 */
    /* 800AD8F8 bgez $v0, .L800AD904 */
    { int c_ = ((s32)r_v0 >= 0); r_a2 = 0u + 0u; if (c_) goto L_800AD904; }
    r_v0 = r_v0 + 0x3Fu; /* 800AD900 addiu $v0, $v0, 0x3F */
L_800AD904:
    r_v0 = BL_SRA(r_v0, 6); /* 800AD904 sra $v0, $v0, 6 */
    bl_sh(r_sp + 0x128u, r_v0); /* 800AD908 sh $v0, 0x128($sp) */
    r_v1 = 0u + 0xFFFFFFFFu; /* 800AD90C addiu $v1, $zero, -0x1 */
L_800AD910:
    r_t0 = r_a3 & 0xFFu; /* 800AD910 andi $t0, $a3, 0xFF */
L_800AD914:
    r_a1 = r_fp & 0xFFu; /* 800AD914 andi $a1, $fp, 0xFF */
    r_a1 = r_a1 + 0xFFFFFFA2u; /* 800AD918 addiu $a1, $a1, -0x5E */
    r_v0 = r_s6 & 0xFFu; /* 800AD91C andi $v0, $s6, 0xFF */
    r_a2 = r_a2 << 16; /* 800AD920 sll $a2, $a2, 16 */
    r_a2 = BL_SRA(r_a2, 16); /* 800AD924 sra $a2, $a2, 16 */
    r_a2 = r_v0 + r_a2; /* 800AD928 addu $a2, $v0, $a2 */
    r_a3 = r_s1 + 0u; /* 800AD92C addu $a3, $s1, $zero */
    r_v0 = r_s5 << 16; /* 800AD930 sll $v0, $s5, 16 */
    r_a0 = bl_lw(r_sp + 0x130u); /* 800AD934 lw $a0, 0x130($sp) */
    r_t8 = bl_lhu(r_sp + 0x1A0u); /* 800AD938 lhu $t8, 0x1A0($sp) */
    r_t7 = bl_lhu(r_sp + 0x1A8u); /* 800AD93C lhu $t7, 0x1A8($sp) */
    r_v0 = BL_SRA(r_v0, 16); /* 800AD940 sra $v0, $v0, 16 */
    bl_sw(r_sp + 0x10u, r_v0); /* 800AD944 sw $v0, 0x10($sp) */
    r_v0 = r_t8 << 16; /* 800AD948 sll $v0, $t8, 16 */
    r_v0 = BL_SRA(r_v0, 16); /* 800AD94C sra $v0, $v0, 16 */
    bl_sw(r_sp + 0x14u, r_v0); /* 800AD950 sw $v0, 0x14($sp) */
    r_v0 = r_t7 << 16; /* 800AD954 sll $v0, $t7, 16 */
    r_v0 = BL_SRA(r_v0, 16); /* 800AD958 sra $v0, $v0, 16 */
    bl_sw(r_sp + 0x18u, r_v0); /* 800AD95C sw $v0, 0x18($sp) */
    r_v0 = r_s2 << 16; /* 800AD960 sll $v0, $s2, 16 */
    r_v0 = BL_SRA(r_v0, 16); /* 800AD964 sra $v0, $v0, 16 */
    bl_sw(r_sp + 0x1Cu, r_v0); /* 800AD968 sw $v0, 0x1C($sp) */
    r_v0 = r_v1 << 16; /* 800AD96C sll $v0, $v1, 16 */
    r_v1 = bl_lh(r_sp + 0x128u); /* 800AD970 lh $v1, 0x128($sp) */
    r_v0 = BL_SRA(r_v0, 16); /* 800AD974 sra $v0, $v0, 16 */
    bl_sw(r_sp + 0x24u, r_v0); /* 800AD978 sw $v0, 0x24($sp) */
    bl_sw(r_sp + 0x28u, r_t0); /* 800AD97C sw $t0, 0x28($sp) */
    /* 800AD980 jal func_800B0164 */
    bl_sw(r_sp + 0x20u, r_v1);
    bl_jal(FN, 0x800B0164u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800B0164 */
    /* 800AD988 j .L800ADEBC */
    ; goto L_800ADEBC;
L_800AD990:
    r_v0 = bl_lhu(r_s3); /* 800AD990 lhu $v0, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800AD994 addiu $s3, $s3, 0x2 */
    r_s2 = r_v0 >> 8; /* 800AD998 srl $s2, $v0, 8 */
    bl_sh(r_sp + 0x9Cu, r_v0); /* 800AD99C sh $v0, 0x9C($sp) */
    r_v0 = bl_lhu(r_s3); /* 800AD9A0 lhu $v0, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800AD9A4 addiu $s3, $s3, 0x2 */
    r_s1 = bl_lbu(r_sp + 0x9Cu); /* 800AD9A8 lbu $s1, 0x9C($sp) */
    r_a3 = r_v0 >> 8; /* 800AD9AC srl $a3, $v0, 8 */
    bl_sh(r_sp + 0x9Cu, r_v0); /* 800AD9B0 sh $v0, 0x9C($sp) */
    r_a1 = bl_lhu(r_s3); /* 800AD9B4 lhu $a1, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800AD9B8 addiu $s3, $s3, 0x2 */
    r_s7 = bl_lbu(r_sp + 0x9Cu); /* 800AD9BC lbu $s7, 0x9C($sp) */
    r_v0 = bl_lhu(r_s3); /* 800AD9C0 lhu $v0, 0x0($s3) */
    ; /* 800AD9C4 nop  */
    bl_sh(r_sp + 0x9Cu, r_v0); /* 800AD9C8 sh $v0, 0x9C($sp) */
    r_v0 = r_s6 & 0x20u; /* 800AD9CC andi $v0, $s6, 0x20 */
    /* 800AD9D0 beqz $v0, .L800AD9E0 */
    { int c_ = (r_v0 == 0); r_s3 = r_s3 + 0x2u; if (c_) goto L_800AD9E0; }
    /* 800AD9D8 j .L800AD9EC */
    r_a2 = 0u + 0u; goto L_800AD9EC;
L_800AD9E0:
    r_v0 = bl_lw(r_s4 + 0x4u); /* 800AD9E0 lw $v0, 0x4($s4) */
    ; /* 800AD9E4 nop  */
    r_a2 = bl_lhu(r_v0 + 0x56u); /* 800AD9E8 lhu $a2, 0x56($v0) */
L_800AD9EC:
    r_v1 = r_s2 & 0xFFu; /* 800AD9EC andi $v1, $s2, 0xFF */
    /* 800AD9F0 bnez $v1, .L800ADA08 */
    { int c_ = (r_v1 != 0); r_s6 = r_s6 & 0x1Fu; if (c_) goto L_800ADA08; }
    r_v0 = 0x800C0000u; /* 800AD9F8 lui $v0, %hi(D_800C3B90) */
    r_v0 = bl_lhu(r_v0 + 0x3B90u); /* 800AD9FC lhu $v0, %lo(D_800C3B90)($v0) */
    /* 800ADA00 j .L800ADA3C */
    r_v0 = r_v0 & 0xFFFu; goto L_800ADA3C;
L_800ADA08:
    r_t8 = 0u | 0x1u; /* 800ADA08 ori $t8, $zero, 0x1 */
    /* 800ADA0C bne $v1, $t8, .L800ADA24 */
    { int c_ = (r_v1 != r_t8); r_v0 = 0u | 0x2u; if (c_) goto L_800ADA24; }
    r_v0 = 0x800C0000u; /* 800ADA14 lui $v0, %hi(D_800C3BA0) */
    r_v0 = bl_lhu(r_v0 + 0x3BA0u); /* 800ADA18 lhu $v0, %lo(D_800C3BA0)($v0) */
    /* 800ADA1C j .L800ADA3C */
    r_v0 = r_v0 & 0xFFFu; goto L_800ADA3C;
L_800ADA24:
    /* 800ADA24 bne $v1, $v0, .L800ADA44 */
    { int c_ = (r_v1 != r_v0); r_v0 = 0u | 0x3u; if (c_) goto L_800ADA44; }
    r_v0 = 0x800C0000u; /* 800ADA2C lui $v0, %hi(D_800C3B94) */
    r_v0 = bl_lhu(r_v0 + 0x3B94u); /* 800ADA30 lhu $v0, %lo(D_800C3B94)($v0) */
    ; /* 800ADA34 nop  */
    r_v0 = r_v0 & 0xFFFu; /* 800ADA38 andi $v0, $v0, 0xFFF */
L_800ADA3C:
    /* 800ADA3C j .L800ADADC */
    bl_sh(r_sp + 0x1B0u, r_v0); goto L_800ADADC;
L_800ADA44:
    /* 800ADA44 bne $v1, $v0, .L800ADA5C */
    { int c_ = (r_v1 != r_v0); r_v0 = 0u | 0x4u; if (c_) goto L_800ADA5C; }
    r_t7 = 0x800C0000u; /* 800ADA4C lui $t7, %hi(D_800C3B98) */
    r_t7 = bl_lhu(r_t7 + 0x3B98u); /* 800ADA50 lhu $t7, %lo(D_800C3B98)($t7) */
    /* 800ADA54 j .L800ADAA4 */
    bl_sh(r_sp + 0x1B0u, r_t7); goto L_800ADAA4;
L_800ADA5C:
    /* 800ADA5C bne $v1, $v0, .L800ADA74 */
    { int c_ = (r_v1 != r_v0); r_v0 = 0u | 0x5u; if (c_) goto L_800ADA74; }
    r_t8 = 0x800C0000u; /* 800ADA64 lui $t8, %hi(D_800C3BA4) */
    r_t8 = bl_lhu(r_t8 + 0x3BA4u); /* 800ADA68 lhu $t8, %lo(D_800C3BA4)($t8) */
    /* 800ADA6C j .L800ADAA4 */
    bl_sh(r_sp + 0x1B0u, r_t8); goto L_800ADAA4;
L_800ADA74:
    /* 800ADA74 bne $v1, $v0, .L800ADA8C */
    { int c_ = (r_v1 != r_v0); r_v0 = 0u | 0x6u; if (c_) goto L_800ADA8C; }
    r_t7 = 0x800C0000u; /* 800ADA7C lui $t7, %hi(D_800C3BA8) */
    r_t7 = bl_lhu(r_t7 + 0x3BA8u); /* 800ADA80 lhu $t7, %lo(D_800C3BA8)($t7) */
    /* 800ADA84 j .L800ADAA4 */
    bl_sh(r_sp + 0x1B0u, r_t7); goto L_800ADAA4;
L_800ADA8C:
    /* 800ADA8C bne $v1, $v0, .L800ADAA4 */
    { int c_ = (r_v1 != r_v0); ; if (c_) goto L_800ADAA4; }
    r_t8 = 0x800C0000u; /* 800ADA94 lui $t8, %hi(D_800C3B9C) */
    r_t8 = bl_lhu(r_t8 + 0x3B9Cu); /* 800ADA98 lhu $t8, %lo(D_800C3B9C)($t8) */
    ; /* 800ADA9C nop  */
    bl_sh(r_sp + 0x1B0u, r_t8); /* 800ADAA0 sh $t8, 0x1B0($sp) */
L_800ADAA4:
    r_v1 = 0x80060000u; /* 800ADAA4 lui $v1, %hi(D_800658C8) */
    r_v1 = bl_lw(r_v1 + 0x58C8u); /* 800ADAA8 lw $v1, %lo(D_800658C8)($v1) */
    r_v0 = r_a1 << 16; /* 800ADAAC sll $v0, $a1, 16 */
    r_a0 = bl_lh(r_v1 + 0x344u); /* 800ADAB0 lh $a0, 0x344($v1) */
    r_v0 = BL_SRA(r_v0, 16); /* 800ADAB4 sra $v0, $v0, 16 */
    BL_MULT(r_v0, r_a0); /* 800ADAB8 mult $v0, $a0 */
    r_v1 = lo; /* 800ADABC mflo $v1 */
    r_v0 = bl_lh(r_sp + 0x9Cu); /* 800ADAC0 lh $v0, 0x9C($sp) */
    ; /* 800ADAC4 nop  */
    BL_MULT(r_v0, r_a0); /* 800ADAC8 mult $v0, $a0 */
    r_a1 = r_v1 >> 12; /* 800ADACC srl $a1, $v1, 12 */
    r_v0 = lo; /* 800ADAD0 mflo $v0 */
    r_v0 = BL_SRA(r_v0, 12); /* 800ADAD4 sra $v0, $v0, 12 */
    bl_sh(r_sp + 0x9Cu, r_v0); /* 800ADAD8 sh $v0, 0x9C($sp) */
L_800ADADC:
    r_v0 = r_s7 & 0x20u; /* 800ADADC andi $v0, $s7, 0x20 */
    /* 800ADAE0 beqz $v0, .L800ADAFC */
    { int c_ = (r_v0 == 0); r_v1 = r_s2 & 0xFFu; if (c_) goto L_800ADAFC; }
    r_t0 = bl_lhu(r_sp + 0x1B0u); /* 800ADAE8 lhu $t0, 0x1B0($sp) */
    ; /* 800ADAEC nop  */
    r_v0 = r_t0 + r_a1; /* 800ADAF0 addu $v0, $t0, $a1 */
    /* 800ADAF4 j .L800ADB30 */
    bl_sh(r_sp + 0x1B0u, r_v0); goto L_800ADB30;
L_800ADAFC:
    r_v0 = (u32)(r_v1 < 0x2u); /* 800ADAFC sltiu $v0, $v1, 0x2 */
    /* 800ADB00 beqz $v0, .L800ADB14 */
    { int c_ = (r_v0 == 0); r_v0 = r_a1 + r_a2; if (c_) goto L_800ADB14; }
    r_v0 = r_v0 & 0xFFFu; /* 800ADB08 andi $v0, $v0, 0xFFF */
    /* 800ADB0C j .L800ADB2C */
    bl_sh(r_sp + 0x1B0u, r_v0); goto L_800ADB2C;
L_800ADB14:
    r_v0 = (u32)(r_v1 < 0x3u); /* 800ADB14 sltiu $v0, $v1, 0x3 */
    /* 800ADB18 beqz $v0, .L800ADB28 */
    { int c_ = (r_v0 == 0); r_t7 = r_a1 & 0xFFFu; if (c_) goto L_800ADB28; }
    /* 800ADB20 j .L800ADB2C */
    bl_sh(r_sp + 0x1B0u, r_t7); goto L_800ADB2C;
L_800ADB28:
    bl_sh(r_sp + 0x1B0u, r_a1); /* 800ADB28 sh $a1, 0x1B0($sp) */
L_800ADB2C:
    r_t0 = bl_lhu(r_sp + 0x1B0u); /* 800ADB2C lhu $t0, 0x1B0($sp) */
L_800ADB30:
    r_v0 = r_s7 & 0x40u; /* 800ADB30 andi $v0, $s7, 0x40 */
    /* 800ADB34 beqz $v0, .L800ADB50 */
    { int c_ = (r_v0 == 0); r_v1 = r_s2 & 0xFFu; if (c_) goto L_800ADB50; }
    r_v0 = bl_lhu(r_sp + 0x9Cu); /* 800ADB3C lhu $v0, 0x9C($sp) */
    ; /* 800ADB40 nop  */
    r_v0 = r_t0 + r_v0; /* 800ADB44 addu $v0, $t0, $v0 */
    /* 800ADB48 j .L800ADBA0 */
    r_t0 = r_v0 + 0u; goto L_800ADBA0;
L_800ADB50:
    r_v0 = (u32)(r_v1 < 0x3u); /* 800ADB50 sltiu $v0, $v1, 0x3 */
    /* 800ADB54 beqz $v0, .L800ADB9C */
    { int c_ = (r_v0 == 0); r_v0 = (u32)(r_v1 < 0x2u); if (c_) goto L_800ADB9C; }
    /* 800ADB5C beqz $v0, .L800ADB6C */
    { int c_ = (r_v0 == 0); r_v0 = r_a1 + r_a2; if (c_) goto L_800ADB6C; }
    /* 800ADB64 j .L800ADB70 */
    r_a0 = r_v0 & 0xFFFu; goto L_800ADB70;
L_800ADB6C:
    r_a0 = r_a1 & 0xFFFu; /* 800ADB6C andi $a0, $a1, 0xFFF */
L_800ADB70:
    r_v0 = bl_lhu(r_sp + 0x9Cu); /* 800ADB70 lhu $v0, 0x9C($sp) */
    ; /* 800ADB74 nop  */
    r_v0 = r_v0 - r_a1; /* 800ADB78 subu $v0, $v0, $a1 */
    r_v1 = r_v0 & 0xFFFu; /* 800ADB7C andi $v1, $v0, 0xFFF */
    r_v0 = (u32)((s32)r_v1 < 2048); /* 800ADB80 slti $v0, $v1, 0x800 */
    /* 800ADB84 bnez $v0, .L800ADB90 */
    { int c_ = (r_v0 != 0); r_t0 = r_v1 + 0u; if (c_) goto L_800ADB90; }
    r_t0 = r_v1 | 0xF000u; /* 800ADB8C ori $t0, $v1, 0xF000 */
L_800ADB90:
    r_v0 = r_t0 + r_a0; /* 800ADB90 addu $v0, $t0, $a0 */
    /* 800ADB94 j .L800ADBA0 */
    r_t0 = r_v0 + 0u; goto L_800ADBA0;
L_800ADB9C:
    r_t0 = bl_lhu(r_sp + 0x9Cu); /* 800ADB9C lhu $t0, 0x9C($sp) */
L_800ADBA0:
    r_v1 = r_a3 & 0xFFu; /* 800ADBA0 andi $v1, $a3, 0xFF */
    r_a1 = r_s2 & 0xFFu; /* 800ADBA4 andi $a1, $s2, 0xFF */
    r_a2 = r_s6 & 0xFFu; /* 800ADBA8 andi $a2, $s6, 0xFF */
    r_a2 = r_a2 + 0x2u; /* 800ADBAC addiu $a2, $a2, 0x2 */
    r_a0 = bl_lw(r_sp + 0x130u); /* 800ADBB0 lw $a0, 0x130($sp) */
    r_t8 = bl_lhu(r_sp + 0x1B0u); /* 800ADBB4 lhu $t8, 0x1B0($sp) */
    r_a3 = r_s1 + 0u; /* 800ADBB8 addu $a3, $s1, $zero */
    bl_sw(r_sp + 0x14u, 0u); /* 800ADBBC sw $zero, 0x14($sp) */
    bl_sw(r_sp + 0x18u, 0u); /* 800ADBC0 sw $zero, 0x18($sp) */
    bl_sw(r_sp + 0x20u, 0u); /* 800ADBC4 sw $zero, 0x20($sp) */
    bl_sw(r_sp + 0x24u, 0u); /* 800ADBC8 sw $zero, 0x24($sp) */
    bl_sw(r_sp + 0x28u, r_v1); /* 800ADBCC sw $v1, 0x28($sp) */
    r_v0 = r_t8 << 16; /* 800ADBD0 sll $v0, $t8, 16 */
    r_v0 = BL_SRA(r_v0, 16); /* 800ADBD4 sra $v0, $v0, 16 */
    bl_sw(r_sp + 0x10u, r_v0); /* 800ADBD8 sw $v0, 0x10($sp) */
    r_v0 = r_t0 << 16; /* 800ADBDC sll $v0, $t0, 16 */
    r_v0 = BL_SRA(r_v0, 16); /* 800ADBE0 sra $v0, $v0, 16 */
    /* 800ADBE4 jal func_800B0164 */
    bl_sw(r_sp + 0x1Cu, r_v0);
    bl_jal(FN, 0x800B0164u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800B0164 */
    /* 800ADBEC j .L800ADEBC */
    ; goto L_800ADEBC;
L_800ADBF4:
    r_a0 = bl_lw(r_sp + 0x130u); /* 800ADBF4 lw $a0, 0x130($sp) */
    r_s1 = 0x800D0000u; /* 800ADBF8 lui $s1, %hi(D_800D335C) */
    r_s1 = bl_lh(r_s1 + 0x335Cu); /* 800ADBFC lh $s1, %lo(D_800D335C)($s1) */
    r_v1 = 0x800D0000u; /* 800ADC00 lui $v1, %hi(D_800D3354) */
    r_v1 = bl_lh(r_v1 + 0x3354u); /* 800ADC04 lh $v1, %lo(D_800D3354)($v1) */
    r_s0 = 0x800D0000u; /* 800ADC08 lui $s0, %hi(D_800D3360) */
    r_s0 = bl_lh(r_s0 + 0x3360u); /* 800ADC0C lh $s0, %lo(D_800D3360)($s0) */
    r_v0 = 0x800D0000u; /* 800ADC10 lui $v0, %hi(D_800D3358) */
    r_v0 = bl_lh(r_v0 + 0x3358u); /* 800ADC14 lh $v0, %lo(D_800D3358)($v0) */
    r_s1 = r_s1 - r_v1; /* 800ADC18 subu $s1, $s1, $v1 */
    /* 800ADC1C jal func_800B00F4 */
    r_s0 = r_s0 - r_v0;
    bl_jal(FN, 0x800B00F4u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800B00F4 */
    /* 800ADC24 jal func_800BCAA4 */
    ;
    bl_jal(FN, 0x800BCAA4u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800BCAA4 */
    r_a0 = r_s1 + 0u; /* 800ADC2C addu $a0, $s1, $zero */
    r_t7 = 0u | 0x1u; /* 800ADC30 ori $t7, $zero, 0x1 */
    r_t8 = 0u | 0xFFu; /* 800ADC34 ori $t8, $zero, 0xFF */
    r_at = 0x800C0000u; /* 800ADC38 lui $at, %hi(D_800C3DF8) */
    bl_sb(r_at + 0x3DF8u, r_t7); /* 800ADC3C sb $t7, %lo(D_800C3DF8)($at) */
    r_at = 0x800C0000u; /* 800ADC40 lui $at, %hi(D_800C3B84) */
    bl_sb(r_at + 0x3B84u, r_t8); /* 800ADC44 sb $t8, %lo(D_800C3B84)($at) */
    /* 800ADC48 jal ratan2 */
    r_a1 = r_s0 + 0u;
    bl_jal(FN, 0x8004B32Cu, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* ratan2 */
    r_v0 = r_v0 & 0xFFFu; /* 800ADC50 andi $v0, $v0, 0xFFF */
    r_at = 0x800C0000u; /* 800ADC54 lui $at, %hi(D_800C3B90) */
    bl_sh(r_at + 0x3B90u, r_v0); /* 800ADC58 sh $v0, %lo(D_800C3B90)($at) */
    r_at = 0x800C0000u; /* 800ADC5C lui $at, %hi(D_800C3BA0) */
    bl_sh(r_at + 0x3BA0u, r_v0); /* 800ADC60 sh $v0, %lo(D_800C3BA0)($at) */
    r_at = 0x800C0000u; /* 800ADC64 lui $at, %hi(D_800C3B94) */
    bl_sh(r_at + 0x3B94u, 0u); /* 800ADC68 sh $zero, %lo(D_800C3B94)($at) */
    r_at = 0x800C0000u; /* 800ADC6C lui $at, %hi(D_800C3B98) */
    bl_sh(r_at + 0x3B98u, 0u); /* 800ADC70 sh $zero, %lo(D_800C3B98)($at) */
    r_at = 0x800C0000u; /* 800ADC74 lui $at, %hi(D_800C3B9C) */
    bl_sh(r_at + 0x3B9Cu, 0u); /* 800ADC78 sh $zero, %lo(D_800C3B9C)($at) */
    r_at = 0x800C0000u; /* 800ADC7C lui $at, %hi(D_800C3B8C) */
    bl_sb(r_at + 0x3B8Cu, 0u); /* 800ADC80 sb $zero, %lo(D_800C3B8C)($at) */
    r_at = 0x800C0000u; /* 800ADC84 lui $at, %hi(D_800C3BA8) */
    bl_sh(r_at + 0x3BA8u, 0u); /* 800ADC88 sh $zero, %lo(D_800C3BA8)($at) */
    r_at = 0x800C0000u; /* 800ADC8C lui $at, %hi(D_800C3BA4) */
    bl_sh(r_at + 0x3BA4u, 0u); /* 800ADC90 sh $zero, %lo(D_800C3BA4)($at) */
    /* 800ADC94 j .L800ADEBC */
    ; goto L_800ADEBC;
L_800ADC9C:
    r_v0 = 0x800C0000u; /* 800ADC9C lui $v0, %hi(D_800C3B84) */
    r_v0 = bl_lbu(r_v0 + 0x3B84u); /* 800ADCA0 lbu $v0, %lo(D_800C3B84)($v0) */
    r_t7 = 0u | 0xFFu; /* 800ADCA4 ori $t7, $zero, 0xFF */
    /* 800ADCA8 beq $v0, $t7, .L800ADCE0 */
    { int c_ = (r_v0 == r_t7); ; if (c_) goto L_800ADCE0; }
    r_v0 = 0x800C0000u; /* 800ADCB0 lui $v0, %hi(D_800C3B88) */
    r_v0 = bl_lbu(r_v0 + 0x3B88u); /* 800ADCB4 lbu $v0, %lo(D_800C3B88)($v0) */
    ; /* 800ADCB8 nop  */
    r_v0 = r_v0 & 0x1u; /* 800ADCBC andi $v0, $v0, 0x1 */
    /* 800ADCC0 beqz $v0, .L800ADCD0 */
    { int c_ = (r_v0 == 0); r_t8 = 0u | 0xFFu; if (c_) goto L_800ADCD0; }
    /* 800ADCC8 j .L800ADEB8 */
    r_s3 = r_s5 + 0u; goto L_800ADEB8;
L_800ADCD0:
    r_at = 0x800C0000u; /* 800ADCD0 lui $at, %hi(D_800C3B84) */
    bl_sb(r_at + 0x3B84u, r_t8); /* 800ADCD4 sb $t8, %lo(D_800C3B84)($at) */
    /* 800ADCD8 j .L800ADEBC */
    ; goto L_800ADEBC;
L_800ADCE0:
    r_at = 0x800C0000u; /* 800ADCE0 lui $at, %hi(D_800C3B84) */
    bl_sb(r_at + 0x3B84u, r_s6); /* 800ADCE4 sb $s6, %lo(D_800C3B84)($at) */
    /* 800ADCE8 j .L800ADEB8 */
    r_s3 = r_s5 + 0u; goto L_800ADEB8;
L_800ADCF0:
    r_t7 = 0u | 0x1u; /* 800ADCF0 ori $t7, $zero, 0x1 */
    r_at = 0x800C0000u; /* 800ADCF4 lui $at, %hi(D_800C3B8C) */
    bl_sb(r_at + 0x3B8Cu, r_t7); /* 800ADCF8 sb $t7, %lo(D_800C3B8C)($at) */
    /* 800ADCFC j .L800ADEBC */
    ; goto L_800ADEBC;
L_800ADD04:
    r_v0 = bl_lhu(r_s3); /* 800ADD04 lhu $v0, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800ADD08 addiu $s3, $s3, 0x2 */
    bl_sh(r_sp + 0x9Cu, r_v0); /* 800ADD0C sh $v0, 0x9C($sp) */
    r_v0 = r_v0 << 16; /* 800ADD10 sll $v0, $v0, 16 */
    r_v0 = BL_SRA(r_v0, 16); /* 800ADD14 sra $v0, $v0, 16 */
    r_v1 = r_v0 << 5; /* 800ADD18 sll $v1, $v0, 5 */
    r_v1 = r_v1 - r_v0; /* 800ADD1C subu $v1, $v1, $v0 */
    r_v0 = bl_lw(r_s4 + 0x4u); /* 800ADD20 lw $v0, 0x4($s4) */
    r_v1 = r_v1 << 2; /* 800ADD24 sll $v1, $v1, 2 */
    r_v1 = r_v1 + r_v0; /* 800ADD28 addu $v1, $v1, $v0 */
    /* 800ADD2C j .L800ADEBC */
    bl_sb(r_v1 + 0x6u, r_s6); goto L_800ADEBC;
L_800ADD34:
    /* 800ADD34 jal ArchiveDataSync */
    ;
    bl_jal(FN, 0x800286CCu, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* ArchiveDataSync */
    /* 800ADD3C beqz $v0, .L800ADEBC */
    { int c_ = (r_v0 == 0); ; if (c_) goto L_800ADEBC; }
    /* 800ADD44 j .L800ADEB8 */
    r_s3 = r_s5 + 0u; goto L_800ADEB8;
L_800ADD4C:
    r_v0 = r_s6 & 0x1u; /* 800ADD4C andi $v0, $s6, 0x1 */
    /* 800ADD50 j .L800ADEBC */
    bl_sb(r_s4 + 0x38u, r_v0); goto L_800ADEBC;
L_800ADD58:
    r_a0 = r_s4 + 0u; /* 800ADD58 addu $a0, $s4, $zero */
    r_a1 = r_s6 & 0xFFu; /* 800ADD5C andi $a1, $s6, 0xFF */
    /* 800ADD60 jal func_800AF438 */
    r_a2 = r_sp + 0x9Cu;
    bl_jal(FN, 0x800AF438u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800AF438 */
    r_v0 = r_v0 & 0xFFu; /* 800ADD68 andi $v0, $v0, 0xFF */
    r_v0 = r_v0 << 2; /* 800ADD6C sll $v0, $v0, 2 */
    r_t8 = 0x800D0000u; /* 800ADD70 lui $t8, %hi(D_800D3368) */
    r_t8 = r_t8 + 0x3368u; /* 800ADD74 addiu $t8, $t8, %lo(D_800D3368) */
    r_v0 = r_v0 + r_t8; /* 800ADD78 addu $v0, $v0, $t8 */
    r_a0 = bl_lhu(r_s3); /* 800ADD7C lhu $a0, 0x0($s3) */
    r_v0 = bl_lw(r_v0); /* 800ADD80 lw $v0, 0x0($v0) */
    r_s3 = r_s3 + 0x2u; /* 800ADD84 addiu $s3, $s3, 0x2 */
    /* 800ADD88 beqz $v0, .L800ADEBC */
    { int c_ = (r_v0 == 0); bl_sh(r_sp + 0x9Cu, r_a0); if (c_) goto L_800ADEBC; }
    r_v1 = bl_lbu(r_v0 + 0x38u); /* 800ADD90 lbu $v1, 0x38($v0) */
    r_v0 = r_a0 & 0x1u; /* 800ADD94 andi $v0, $a0, 0x1 */
    /* 800ADD98 bne $v1, $v0, .L800ADEBC */
    { int c_ = (r_v1 != r_v0); ; if (c_) goto L_800ADEBC; }
    /* 800ADDA0 j .L800ADEB8 */
    r_s3 = r_s5 + 0u; goto L_800ADEB8;
L_800ADDA8:
    r_v0 = r_s6 & 0xFFu; /* 800ADDA8 andi $v0, $s6, 0xFF */
    /* 800ADDAC beqz $v0, .L800ADDC4 */
    { int c_ = (r_v0 == 0); r_v0 = 0u + 0xFFFFFFFFu; if (c_) goto L_800ADDC4; }
    r_v0 = 0x800C0000u; /* 800ADDB4 lui $v0, %hi(D_800C3E30) */
    r_v0 = bl_lhu(r_v0 + 0x3E30u); /* 800ADDB8 lhu $v0, %lo(D_800C3E30)($v0) */
    /* 800ADDBC j .L800ADEBC */
    bl_sh(r_s4 + 0x3Au, r_v0); goto L_800ADEBC;
L_800ADDC4:
    /* 800ADDC4 j .L800ADEBC */
    bl_sh(r_s4 + 0x3Au, r_v0); goto L_800ADEBC;
L_800ADDCC:
    r_a0 = bl_lhu(r_s3); /* 800ADDCC lhu $a0, 0x0($s3) */
    r_v1 = bl_lh(r_s4 + 0x3Au); /* 800ADDD0 lh $v1, 0x3A($s4) */
    r_v0 = 0x800C0000u; /* 800ADDD4 lui $v0, %hi(D_800C3E30) */
    r_v0 = bl_lhu(r_v0 + 0x3E30u); /* 800ADDD8 lhu $v0, %lo(D_800C3E30)($v0) */
    r_s3 = r_s3 + 0x2u; /* 800ADDDC addiu $s3, $s3, 0x2 */
    /* 800ADDE0 bne $v1, $v0, .L800ADEBC */
    { int c_ = (r_v1 != r_v0); bl_sh(r_sp + 0x9Cu, r_a0); if (c_) goto L_800ADEBC; }
    r_v0 = r_a0 << 16; /* 800ADDE8 sll $v0, $a0, 16 */
    r_v0 = BL_SRA(r_v0, 16); /* 800ADDEC sra $v0, $v0, 16 */
    /* 800ADDF0 j .L800ADEB8 */
    r_s3 = r_s5 + r_v0; goto L_800ADEB8;
L_800ADDF8:
    r_v1 = bl_lhu(r_s3); /* 800ADDF8 lhu $v1, 0x0($s3) */
    r_s3 = r_s3 + 0x2u; /* 800ADDFC addiu $s3, $s3, 0x2 */
    r_v0 = r_s6 & 0xFFu; /* 800ADE00 andi $v0, $s6, 0xFF */
    /* 800ADE04 bnez $v0, .L800ADEBC */
    { int c_ = (r_v0 != 0); bl_sh(r_sp + 0x9Cu, r_v1); if (c_) goto L_800ADEBC; }
    r_at = 0x800D0000u; /* 800ADE0C lui $at, %hi(D_800D39E4) */
    bl_sh(r_at + 0x39E4u, r_v1); /* 800ADE10 sh $v1, %lo(D_800D39E4)($at) */
    /* 800ADE14 j .L800ADEBC */
    ; goto L_800ADEBC;
L_800ADE1C:
    r_v0 = 0x80060000u; /* 800ADE1C lui $v0, %hi(D_800591B1) */
    r_v0 = bl_lbu(r_v0 + 0xFFFF91B1u); /* 800ADE20 lbu $v0, %lo(D_800591B1)($v0) */
    ; /* 800ADE24 nop  */
    /* 800ADE28 bnez $v0, .L800ADEB8 */
    { int c_ = (r_v0 != 0); ; if (c_) goto L_800ADEB8; }
    /* 800ADE30 j .L800ADEB8 */
    r_s3 = r_s5 + 0u; goto L_800ADEB8;
L_800ADE38:
    r_a0 = 0x800D0000u; /* 800ADE38 lui $a0, %hi(D_800D39E4) */
    r_a0 = bl_lh(r_a0 + 0x39E4u); /* 800ADE3C lh $a0, %lo(D_800D39E4)($a0) */
    /* 800ADE40 jal func_800B8054 */
    bl_sw(r_sp + 0x150u, 0u);
    bl_jal(FN, 0x800B8054u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800B8054 */
    /* 800ADE48 j .L800ADEBC */
    ; goto L_800ADEBC;
L_800ADE50:
    /* 800ADE50 jal func_800BF998 */
    ;
    bl_jal(FN, 0x800BF998u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800BF998 */
    /* 800ADE58 j .L800ADEE8 */
    ; goto L_800ADEE8;
L_800ADE60:
    r_v0 = bl_lhu(r_s3); /* 800ADE60 lhu $v0, 0x0($s3) */
    ; /* 800ADE64 nop  */
    bl_sh(r_sp + 0x9Cu, r_v0); /* 800ADE68 sh $v0, 0x9C($sp) */
    r_v0 = bl_lw(r_s4 + 0x4u); /* 800ADE6C lw $v0, 0x4($s4) */
    r_a2 = bl_lh(r_s4 + 0x88u); /* 800ADE70 lh $a2, 0x88($s4) */
    r_v1 = bl_lh(r_s4 + 0x8Cu); /* 800ADE74 lh $v1, 0x8C($s4) */
    r_a0 = bl_lw(r_v0 + 0x5Cu); /* 800ADE78 lw $a0, 0x5C($v0) */
    r_a1 = bl_lw(r_v0 + 0x64u); /* 800ADE7C lw $a1, 0x64($v0) */
    r_a0 = r_a0 - r_a2; /* 800ADE80 subu $a0, $a0, $a2 */
    /* 800ADE84 jal ratan2 */
    r_a1 = r_a1 - r_v1;
    bl_jal(FN, 0x8004B32Cu, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* ratan2 */
    r_v1 = bl_lw(r_s4 + 0x4u); /* 800ADE8C lw $v1, 0x4($s4) */
    ; /* 800ADE90 nop  */
    r_v1 = bl_lhu(r_v1 + 0x56u); /* 800ADE94 lhu $v1, 0x56($v1) */
    r_v0 = r_v0 & 0xFFFu; /* 800ADE98 andi $v0, $v0, 0xFFF */
    r_v1 = r_v1 & 0xFFFu; /* 800ADE9C andi $v1, $v1, 0xFFF */
    /* 800ADEA0 bne $v0, $v1, .L800ADEBC */
    { int c_ = (r_v0 != r_v1); r_s3 = r_s3 + 0x2u; if (c_) goto L_800ADEBC; }
L_800ADEA8:
    r_v0 = bl_lh(r_sp + 0x9Cu); /* 800ADEA8 lh $v0, 0x9C($sp) */
    /* 800ADEAC j .L800ADEBC */
    r_s3 = r_s5 + r_v0; goto L_800ADEBC;
L_800ADEB4:
    r_s3 = r_s3 + 0xFFFFFFFEu; /* 800ADEB4 addiu $s3, $s3, -0x2 */
L_800ADEB8:
    bl_sw(r_sp + 0x150u, 0u); /* 800ADEB8 sw $zero, 0x150($sp) */
L_800ADEBC:
    r_t7 = bl_lw(r_sp + 0x150u); /* 800ADEBC lw $t7, 0x150($sp) */
    ; /* 800ADEC0 nop  */
    /* 800ADEC4 bnez $t7, .L800AB0D4 */
    { int c_ = (r_t7 != 0); r_s5 = r_s3 + 0u; if (c_) goto L_800AB0D4; }
L_800ADECC:
    bl_sw(r_s4 + 0x10u, r_s3); /* 800ADECC sw $s3, 0x10($s4) */
    r_t8 = bl_lw(r_sp + 0x158u); /* 800ADED0 lw $t8, 0x158($sp) */
    ; /* 800ADED4 nop  */
    /* 800ADED8 beqz $t8, .L800ADEE8 */
    { int c_ = (r_t8 == 0); ; if (c_) goto L_800ADEE8; }
    /* 800ADEE0 jal func_800B9258 */
    ;
    bl_jal(FN, 0x800B9258u, r_a0, r_a1, r_a2, r_a3, r_sp, &r_v0, &r_v1); /* func_800B9258 */
L_800ADEE8:
    r_ra = bl_lw(r_sp + 0x1F4u); /* 800ADEE8 lw $ra, 0x1F4($sp) */
    r_fp = bl_lw(r_sp + 0x1F0u); /* 800ADEEC lw $fp, 0x1F0($sp) */
    r_s7 = bl_lw(r_sp + 0x1ECu); /* 800ADEF0 lw $s7, 0x1EC($sp) */
    r_s6 = bl_lw(r_sp + 0x1E8u); /* 800ADEF4 lw $s6, 0x1E8($sp) */
    r_s5 = bl_lw(r_sp + 0x1E4u); /* 800ADEF8 lw $s5, 0x1E4($sp) */
    r_s4 = bl_lw(r_sp + 0x1E0u); /* 800ADEFC lw $s4, 0x1E0($sp) */
    r_s3 = bl_lw(r_sp + 0x1DCu); /* 800ADF00 lw $s3, 0x1DC($sp) */
    r_s2 = bl_lw(r_sp + 0x1D8u); /* 800ADF04 lw $s2, 0x1D8($sp) */
    r_s1 = bl_lw(r_sp + 0x1D4u); /* 800ADF08 lw $s1, 0x1D4($sp) */
    r_s0 = bl_lw(r_sp + 0x1D0u); /* 800ADF0C lw $s0, 0x1D0($sp) */
    r_sp = r_sp + 0x1F8u; /* 800ADF10 addiu $sp, $sp, 0x1F8 */
    /* 800ADF14 jr $ra */
    ; goto bl_exit;
bl_exit:
    bl_leave(bl_saved);
    (void)hi; (void)lo; (void)r_at; (void)r_v1; (void)r_k0; (void)r_k1; (void)r_gp; (void)r_ra;
    (void)r_v0;
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
