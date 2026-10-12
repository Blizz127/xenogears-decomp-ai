#include "common.h"


#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/main65", func_8009F5B8);
typedef struct {
    u8 pad0[0x5C];
    void* buf; /* +0x5C */
    s32 size;  /* +0x60 */
    u8 pad64[0x18];
} Entry; /* 0x7C */

typedef struct {
    u8 pad0[0xA];
    u16 count;        /* +0xA */
    Entry entries[1]; /* +0xC */
} Set;


/* free every entry buffer of the set (clearing buf/size), then the set. */
void func_8009F708(Set* set) {
    s32 i;
    Entry* e;

    if (set != NULL) {
        i = 0;
        if (i < set->count) {
            e = set->entries;
            do {
                void* buf = e->buf;

                i++;
                if (buf != NULL) {
                    HeapFree(buf);
                    e->buf = NULL;
                    e->size = 0;
                }
                e++;
            } while (i < set->count);
        }
        set->count = 0;
        HeapFree(set);
    }
}

typedef struct {
    u8** items;
    u32 count;
} ItemList;

extern void func_8002CBBC(u8* item);

/* release an ItemList: optionally func_8002CBBC every item, then free the
 * pointer array. */
void func_8009F794(ItemList* list, s32 release) {
    u32 i;

    if (list != NULL) {
        for (i = 0; i < list->count; i++) {
            if (list->items != NULL && list->items[i] != NULL && release != 0) {
                func_8002CBBC(list->items[i]);
            }
        }
        if (list->items != NULL) {
            HeapFree(list->items);
            list->items = NULL;
        }
    }
}

INCLUDE_ASM("asm/battle/nonmatchings/main65", func_8009F844);
INCLUDE_ASM("asm/battle/nonmatchings/main65", func_800A0838);
INCLUDE_ASM("asm/battle/nonmatchings/main65", func_800A1B50);
typedef struct {
    u8 unk0[0x54];
    s16 rot[3];
    u16 unk5A;
    s32 pos[3];
    u8 unk68[8];
    u8* h70;
    u8* h74;
    u8* h78;
} M65Node; /* 0x7C, the node layout of func_8009EC4C (main64) */
extern u8* func_800A2330(u8* ctx);
extern void func_800A23E8(u8* a0, u32 p);

/* func_800A1CF4.s: one pass over the nodes after node 0 of a func_8009EC4C
 * set, driven by the halfword record `anim` (+4 flags, +6, +0xC and +0xE
 * counts, triples from +0x18, skipping (+0xC count + 1) triples when +6 is
 * 0).  While triples remain, one that differs from the node's +0x54 halfword
 * triple (stored wrapped to +-0x800 after & 0xFFF, so `rot`) or its +0x5C
 * word triple (`pos`) gets a handle from func_800A2330 in +0x70 / +0x74;
 * otherwise a handle whose byte 3 is not 0xFF is released through
 * func_800A23E8.  The handle takes a3 (0 means 1) at +0x12, a5 & 1 and a6
 * as bytes 1 and 3, and a4 & 1 picks its +0xA..+0xE form.  Returns the node
 * count minus one. */
u16 func_800A1CF4(u8* ctx, M65Node* nodes, s16* anim, s32 a3, s32 a4, s32 a5, s32 a6) {
    M65Node* node;
    u16 nrot;
    u16 crot;
    u16 npos;
    u16 cpos;
    u16 count;
    u16 flags;
    s32 i;
    s32 x;
    s32 y;
    s32 z;
    u8* h;

    if (a3 == 0) {
        a3 = 1;
    }
    crot = 0;
    cpos = 0;
    a5 &= 1;
    nrot = anim[6];
    i = anim[3];
    npos = anim[7];
    a4 &= 1;
    flags = anim[2];
    anim += 0xC;
    if (i == 0) {
        anim += (nrot + 1) * 3;
    }
    count = *(u16*)((u8*)nodes + 0xA) - 1;
    node = nodes;
    for (i = 0; i < count; i++) {
        node++;
        if (!(flags & 1) && crot < nrot) {
            x = *anim++;
            y = *anim++;
            z = *anim++;
            crot++;
            if (node->rot[0] != x || node->rot[1] != y || node->rot[2] != z) {
                if (node->h70 != NULL) {
                    h = node->h70;
                    if (h[3] == 0xFF) {
                        goto pos;
                    }
                } else {
                    h = func_800A2330(ctx);
                }
                if (h != NULL) {
                    h[0] = 1;
                    h[1] = a5;
                    h[2] = a4 + 3;
                    h[3] = a6;
                    *(u16*)(h + 4) = node->rot[0];
                    *(u16*)(h + 6) = node->rot[1];
                    *(u16*)(h + 8) = node->rot[2];
                    x = (x - node->rot[0]) & 0xFFF;
                    if (x >= 0x800) {
                        x -= 0x1000;
                    }
                    *(s16*)(h + 0xA) = x;
                    y = (y - node->rot[1]) & 0xFFF;
                    if (y >= 0x800) {
                        y -= 0x1000;
                    }
                    *(s16*)(h + 0xC) = y;
                    z = (z - node->rot[2]) & 0xFFF;
                    if (z >= 0x800) {
                        z -= 0x1000;
                    }
                    *(s16*)(h + 0xE) = z;
                    if (a4 != 0) {
                        *(u16*)(h + 0xA) += node->rot[0];
                        *(u16*)(h + 0xC) += node->rot[1];
                        *(u16*)(h + 0xE) += node->rot[2];
                    }
                    *(u16*)(h + 0x10) = 0;
                    *(u16*)(h + 0x12) = a3;
                    node->h70 = h;
                    goto pos;
                }
            }
        }
        if (node->h70 != NULL && node->h70[3] != 0xFF) {
            func_800A23E8(ctx, (u32)node->h70);
            node->h70 = NULL;
        }
    pos:
        if (!(flags & 2) && cpos < npos) {
            x = *anim++;
            y = *anim++;
            z = *anim++;
            cpos++;
            if (node->pos[0] != x || node->pos[1] != y || node->pos[2] != z) {
                if (node->h74 != NULL) {
                    h = node->h74;
                    if (h[3] == 0xFF) {
                        continue;
                    }
                } else {
                    h = func_800A2330(ctx);
                }
                if (h != NULL) {
                    h[0] = 1;
                    h[1] = a5;
                    h[2] = a4 + 3;
                    h[3] = a6;
                    *(u16*)(h + 4) = node->pos[0];
                    *(u16*)(h + 6) = node->pos[1];
                    *(u16*)(h + 8) = node->pos[2];
                    if (a4 != 0) {
                        *(s16*)(h + 0xA) = x;
                        *(s16*)(h + 0xC) = y;
                        *(s16*)(h + 0xE) = z;
                    } else {
                        *(s16*)(h + 0xA) = x - (u16)node->pos[0];
                        *(s16*)(h + 0xC) = y - (u16)node->pos[1];
                        *(s16*)(h + 0xE) = z - (u16)node->pos[2];
                    }
                    *(u16*)(h + 0x10) = 0;
                    *(u16*)(h + 0x12) = a3;
                    node->h74 = h;
                    continue;
                }
            }
        }
        if (node->h74 != NULL && node->h74[3] != 0xFF) {
            func_800A23E8(ctx, (u32)node->h74);
            node->h74 = NULL;
        }
    }
    return count;
}
#endif
#ifdef XENO_PC_PORT
/* Instruction-faithful port bodies of unmatched main65 functions (M-a).
 * Generated from the retail listings by tools/scripts/battle_lift_asm.py
 * (one statement per MIPS instruction; include/battle_lift.h has the
 * register, memory, COP2, frame and call contract) and committed as
 * reviewed source.  Regenerate with
 *   tools/scripts/battle_lift_asm.py asm/battle/nonmatchings/main65/<fn>.s --args N
 */
#include "battle_lift.h"

/* func_800A0838.s (1222 instructions): one step of the up-to-three track
 * records (+0x6C/+0x70/+0x74) of each of model a1's +0xA 0x7C-byte entries,
 * the first two dispatched on the track kind through jtbl_80070514 /
 * jtbl_8007053C; returns the OR of the per-track flags.  Args as
 * 800AAA20 passes them: the track pool, model, a2 (u16), frame (s16).  Overlay callee still
 * interpreted through bl_call_guest: func_800A23E8.  Main-exe callees
 * (native): ApplyMatrix, SquareRoot0, ratan2. */
u32 func_800A0838(u32 arg0, u32 arg1, u32 arg2, u32 arg3) {
    static const char FN[] = "func_800A0838";
    u32 r_at = 0, r_v0 = 0, r_v1 = 0, r_a0 = 0, r_a1 = 0, r_a2 = 0, r_a3 = 0, r_t0 = 0, r_t1 = 0, r_t2 = 0, r_t3 = 0, r_t4 = 0, r_t5 = 0, r_t6 = 0, r_t7 = 0, r_s0 = 0, r_s1 = 0, r_s2 = 0, r_s3 = 0, r_s4 = 0, r_s5 = 0, r_s6 = 0, r_s7 = 0, r_t8 = 0, r_t9 = 0, r_k0 = 0, r_k1 = 0, r_gp = 0, r_sp = 0, r_fp = 0, r_ra = 0;
    u32 hi = 0, lo = 0, bl_saved;
    r_sp = bl_enter(FN, 0xB8u, &bl_saved);
    r_a0 = arg0;
    r_a1 = arg1;
    r_a2 = arg2;
    r_a3 = arg3;
    r_sp = r_sp + 0xFFFFFF48u; /* 800A0838 addiu $sp, $sp, -0xB8 */
    bl_sw(r_sp + 0xB4u, r_ra); /* 800A083C sw $ra, 0xB4($sp) */
    bl_sw(r_sp + 0xB0u, r_fp); /* 800A0840 sw $fp, 0xB0($sp) */
    bl_sw(r_sp + 0xACu, r_s7); /* 800A0844 sw $s7, 0xAC($sp) */
    bl_sw(r_sp + 0xA8u, r_s6); /* 800A0848 sw $s6, 0xA8($sp) */
    bl_sw(r_sp + 0xA4u, r_s5); /* 800A084C sw $s5, 0xA4($sp) */
    bl_sw(r_sp + 0xA0u, r_s4); /* 800A0850 sw $s4, 0xA0($sp) */
    bl_sw(r_sp + 0x9Cu, r_s3); /* 800A0854 sw $s3, 0x9C($sp) */
    bl_sw(r_sp + 0x98u, r_s2); /* 800A0858 sw $s2, 0x98($sp) */
    bl_sw(r_sp + 0x94u, r_s1); /* 800A085C sw $s1, 0x94($sp) */
    bl_sw(r_sp + 0x90u, r_s0); /* 800A0860 sw $s0, 0x90($sp) */
    bl_sw(r_sp + 0x28u, r_a0); /* 800A0864 sw $a0, 0x28($sp) */
    bl_sw(r_sp + 0x30u, r_a1); /* 800A0868 sw $a1, 0x30($sp) */
    bl_sw(r_sp + 0x38u, r_a2); /* 800A086C sw $a2, 0x38($sp) */
    bl_sw(r_sp + 0x40u, r_a3); /* 800A0870 sw $a3, 0x40($sp) */
    bl_sw(r_sp + 0x48u, 0u); /* 800A0874 sw $zero, 0x48($sp) */
    r_t4 = bl_lhu(r_a1 + 0xAu); /* 800A0878 lhu $t4, 0xA($a1) */
    r_s5 = 0u + 0u; /* 800A087C addu $s5, $zero, $zero */
    /* 800A0880 beqz $t4, .L800A1B18 */
    { int c_ = (r_t4 == 0); bl_sw(r_sp + 0x50u, r_t4); if (c_) goto L_800A1B18; }
    r_s1 = r_a1 + 0x4u; /* 800A0888 addiu $s1, $a1, 0x4 */
L_800A088C:
    r_a1 = bl_lw(r_s1 + 0x6Cu); /* 800A088C lw $a1, 0x6C($s1) */
    ; /* 800A0890 nop  */
    /* 800A0894 beqz $a1, .L800A102C */
    { int c_ = (r_a1 == 0); r_s0 = r_a1 + 0u; if (c_) goto L_800A102C; }
    r_fp = bl_lbu(r_s0 + 0x2u); /* 800A089C lbu $fp, 0x2($s0) */
    ; /* 800A08A0 nop  */
    r_v1 = r_fp & 0xFu; /* 800A08A4 andi $v1, $fp, 0xF */
    r_v0 = (u32)(r_v1 < 0x9u); /* 800A08A8 sltiu $v0, $v1, 0x9 */
    /* 800A08AC beqz $v0, .L800A0F48 */
    { int c_ = (r_v0 == 0); r_v0 = r_v1 << 2; if (c_) goto L_800A0F48; }
    r_at = 0x80070000u; /* 800A08B4 lui $at, %hi(jtbl_80070514) */
    r_at = r_at + r_v0; /* 800A08B8 addu $at, $at, $v0 */
    r_v0 = bl_lw(r_at + 0x514u); /* 800A08BC lw $v0, %lo(jtbl_80070514)($at) */
    ; /* 800A08C0 nop  */
    /* 800A08C4 jr $v0 */
    { u32 t_ = r_v0; ; switch (t_) { case 0x800A08CCu: goto L_800A08CC; case 0x800A0938u: goto L_800A0938; case 0x800A0A60u: goto L_800A0A60; case 0x800A0AF8u: goto L_800A0AF8; case 0x800A0BFCu: goto L_800A0BFC; case 0x800A0D2Cu: goto L_800A0D2C; case 0x800A0D9Cu: goto L_800A0D9C; case 0x800A0F48u: goto L_800A0F48; case 0x800A106Cu: goto L_800A106C; case 0x800A10ECu: goto L_800A10EC; case 0x800A120Cu: goto L_800A120C; case 0x800A1368u: goto L_800A1368; case 0x800A146Cu: goto L_800A146C; case 0x800A15B8u: goto L_800A15B8; default: bl_fail(FN, "jump-table target", t_); } }
L_800A08CC:
    r_a0 = bl_lw(r_s0 + 0x8u); /* 800A08CC lw $a0, 0x8($s0) */
    r_v0 = r_fp & 0x10u; /* 800A08D0 andi $v0, $fp, 0x10 */
    /* 800A08D4 bnez $v0, .L800A08FC */
    { int c_ = (r_v0 != 0); r_v0 = r_fp & 0x20u; if (c_) goto L_800A08FC; }
    r_v0 = bl_lhu(r_a0); /* 800A08DC lhu $v0, 0x0($a0) */
    ; /* 800A08E0 nop  */
    bl_sh(r_s1 + 0x50u, r_v0); /* 800A08E4 sh $v0, 0x50($s1) */
    r_v0 = bl_lw(r_s0 + 0x8u); /* 800A08E8 lw $v0, 0x8($s0) */
    r_a0 = r_a0 + 0x2u; /* 800A08EC addiu $a0, $a0, 0x2 */
    r_v0 = r_v0 + 0x2u; /* 800A08F0 addiu $v0, $v0, 0x2 */
    bl_sw(r_s0 + 0x8u, r_v0); /* 800A08F4 sw $v0, 0x8($s0) */
    r_v0 = r_fp & 0x20u; /* 800A08F8 andi $v0, $fp, 0x20 */
L_800A08FC:
    /* 800A08FC bnez $v0, .L800A0924 */
    { int c_ = (r_v0 != 0); r_v0 = r_fp & 0x40u; if (c_) goto L_800A0924; }
    r_v0 = bl_lhu(r_a0); /* 800A0904 lhu $v0, 0x0($a0) */
    ; /* 800A0908 nop  */
    bl_sh(r_s1 + 0x52u, r_v0); /* 800A090C sh $v0, 0x52($s1) */
    r_v0 = bl_lw(r_s0 + 0x8u); /* 800A0910 lw $v0, 0x8($s0) */
    r_a0 = r_a0 + 0x2u; /* 800A0914 addiu $a0, $a0, 0x2 */
    r_v0 = r_v0 + 0x2u; /* 800A0918 addiu $v0, $v0, 0x2 */
    bl_sw(r_s0 + 0x8u, r_v0); /* 800A091C sw $v0, 0x8($s0) */
    r_v0 = r_fp & 0x40u; /* 800A0920 andi $v0, $fp, 0x40 */
L_800A0924:
    /* 800A0924 bnez $v0, .L800A0F48 */
    { int c_ = (r_v0 != 0); ; if (c_) goto L_800A0F48; }
    r_v0 = bl_lhu(r_a0); /* 800A092C lhu $v0, 0x0($a0) */
    /* 800A0930 j .L800A0AE4 */
    bl_sh(r_s1 + 0x54u, r_v0); goto L_800A0AE4;
L_800A0938:
    r_v0 = r_fp & 0x10u; /* 800A0938 andi $v0, $fp, 0x10 */
    /* 800A093C bnez $v0, .L800A099C */
    { int c_ = (r_v0 != 0); r_v0 = r_fp & 0x20u; if (c_) goto L_800A099C; }
    r_v1 = bl_lw(r_s0 + 0x8u); /* 800A0944 lw $v1, 0x8($s0) */
    ; /* 800A0948 nop  */
    r_v0 = r_v1 + 0x1u; /* 800A094C addiu $v0, $v1, 0x1 */
    bl_sw(r_s0 + 0x8u, r_v0); /* 800A0950 sw $v0, 0x8($s0) */
    r_a0 = bl_lb(r_v1); /* 800A0954 lb $a0, 0x0($v1) */
    r_v0 = 0u + 0xFFFFFF80u; /* 800A0958 addiu $v0, $zero, -0x80 */
    /* 800A095C beq $a0, $v0, .L800A0970 */
    { int c_ = (r_a0 == r_v0); ; if (c_) goto L_800A0970; }
    r_v0 = bl_lhu(r_s1 + 0x50u); /* 800A0964 lhu $v0, 0x50($s1) */
    /* 800A0968 j .L800A0994 */
    r_v0 = r_v0 + r_a0; goto L_800A0994;
L_800A0970:
    r_v0 = r_v1 + 0x2u; /* 800A0970 addiu $v0, $v1, 0x2 */
    bl_sw(r_s0 + 0x8u, r_v0); /* 800A0974 sw $v0, 0x8($s0) */
    r_a0 = bl_lbu(r_v1 + 0x1u); /* 800A0978 lbu $a0, 0x1($v1) */
    r_v0 = r_v1 + 0x3u; /* 800A097C addiu $v0, $v1, 0x3 */
    bl_sw(r_s0 + 0x8u, r_v0); /* 800A0980 sw $v0, 0x8($s0) */
    r_v0 = bl_lb(r_v1 + 0x2u); /* 800A0984 lb $v0, 0x2($v1) */
    ; /* 800A0988 nop  */
    r_v0 = r_v0 << 8; /* 800A098C sll $v0, $v0, 8 */
    r_v0 = r_a0 | r_v0; /* 800A0990 or $v0, $a0, $v0 */
L_800A0994:
    bl_sh(r_s1 + 0x50u, r_v0); /* 800A0994 sh $v0, 0x50($s1) */
    r_v0 = r_fp & 0x20u; /* 800A0998 andi $v0, $fp, 0x20 */
L_800A099C:
    /* 800A099C bnez $v0, .L800A09FC */
    { int c_ = (r_v0 != 0); r_v0 = r_fp & 0x40u; if (c_) goto L_800A09FC; }
    r_v1 = bl_lw(r_s0 + 0x8u); /* 800A09A4 lw $v1, 0x8($s0) */
    ; /* 800A09A8 nop  */
    r_v0 = r_v1 + 0x1u; /* 800A09AC addiu $v0, $v1, 0x1 */
    bl_sw(r_s0 + 0x8u, r_v0); /* 800A09B0 sw $v0, 0x8($s0) */
    r_a0 = bl_lb(r_v1); /* 800A09B4 lb $a0, 0x0($v1) */
    r_v0 = 0u + 0xFFFFFF80u; /* 800A09B8 addiu $v0, $zero, -0x80 */
    /* 800A09BC beq $a0, $v0, .L800A09D0 */
    { int c_ = (r_a0 == r_v0); ; if (c_) goto L_800A09D0; }
    r_v0 = bl_lhu(r_s1 + 0x52u); /* 800A09C4 lhu $v0, 0x52($s1) */
    /* 800A09C8 j .L800A09F4 */
    r_v0 = r_v0 + r_a0; goto L_800A09F4;
L_800A09D0:
    r_v0 = r_v1 + 0x2u; /* 800A09D0 addiu $v0, $v1, 0x2 */
    bl_sw(r_s0 + 0x8u, r_v0); /* 800A09D4 sw $v0, 0x8($s0) */
    r_a0 = bl_lbu(r_v1 + 0x1u); /* 800A09D8 lbu $a0, 0x1($v1) */
    r_v0 = r_v1 + 0x3u; /* 800A09DC addiu $v0, $v1, 0x3 */
    bl_sw(r_s0 + 0x8u, r_v0); /* 800A09E0 sw $v0, 0x8($s0) */
    r_v0 = bl_lb(r_v1 + 0x2u); /* 800A09E4 lb $v0, 0x2($v1) */
    ; /* 800A09E8 nop  */
    r_v0 = r_v0 << 8; /* 800A09EC sll $v0, $v0, 8 */
    r_v0 = r_a0 | r_v0; /* 800A09F0 or $v0, $a0, $v0 */
L_800A09F4:
    bl_sh(r_s1 + 0x52u, r_v0); /* 800A09F4 sh $v0, 0x52($s1) */
    r_v0 = r_fp & 0x40u; /* 800A09F8 andi $v0, $fp, 0x40 */
L_800A09FC:
    /* 800A09FC bnez $v0, .L800A0F48 */
    { int c_ = (r_v0 != 0); ; if (c_) goto L_800A0F48; }
    r_v1 = bl_lw(r_s0 + 0x8u); /* 800A0A04 lw $v1, 0x8($s0) */
    ; /* 800A0A08 nop  */
    r_v0 = r_v1 + 0x1u; /* 800A0A0C addiu $v0, $v1, 0x1 */
    bl_sw(r_s0 + 0x8u, r_v0); /* 800A0A10 sw $v0, 0x8($s0) */
    r_a0 = bl_lb(r_v1); /* 800A0A14 lb $a0, 0x0($v1) */
    r_v0 = 0u + 0xFFFFFF80u; /* 800A0A18 addiu $v0, $zero, -0x80 */
    /* 800A0A1C beq $a0, $v0, .L800A0A38 */
    { int c_ = (r_a0 == r_v0); r_v0 = r_v1 + 0x2u; if (c_) goto L_800A0A38; }
    r_v0 = bl_lhu(r_s1 + 0x54u); /* 800A0A24 lhu $v0, 0x54($s1) */
    ; /* 800A0A28 nop  */
    r_v0 = r_v0 + r_a0; /* 800A0A2C addu $v0, $v0, $a0 */
    /* 800A0A30 j .L800A0F48 */
    bl_sh(r_s1 + 0x54u, r_v0); goto L_800A0F48;
L_800A0A38:
    bl_sw(r_s0 + 0x8u, r_v0); /* 800A0A38 sw $v0, 0x8($s0) */
    r_a0 = bl_lbu(r_v1 + 0x1u); /* 800A0A3C lbu $a0, 0x1($v1) */
    r_v0 = r_v1 + 0x3u; /* 800A0A40 addiu $v0, $v1, 0x3 */
    bl_sw(r_s0 + 0x8u, r_v0); /* 800A0A44 sw $v0, 0x8($s0) */
    r_v0 = bl_lb(r_v1 + 0x2u); /* 800A0A48 lb $v0, 0x2($v1) */
    ; /* 800A0A4C nop  */
    r_v0 = r_v0 << 8; /* 800A0A50 sll $v0, $v0, 8 */
    r_v0 = r_a0 | r_v0; /* 800A0A54 or $v0, $a0, $v0 */
    /* 800A0A58 j .L800A0F48 */
    bl_sh(r_s1 + 0x54u, r_v0); goto L_800A0F48;
L_800A0A60:
    r_a0 = bl_lw(r_s0 + 0x8u); /* 800A0A60 lw $a0, 0x8($s0) */
    r_v0 = r_fp & 0x10u; /* 800A0A64 andi $v0, $fp, 0x10 */
    /* 800A0A68 bnez $v0, .L800A0A98 */
    { int c_ = (r_v0 != 0); r_v0 = r_fp & 0x20u; if (c_) goto L_800A0A98; }
    r_v0 = bl_lhu(r_s1 + 0x50u); /* 800A0A70 lhu $v0, 0x50($s1) */
    r_v1 = bl_lhu(r_a0); /* 800A0A74 lhu $v1, 0x0($a0) */
    ; /* 800A0A78 nop  */
    r_v0 = r_v0 + r_v1; /* 800A0A7C addu $v0, $v0, $v1 */
    bl_sh(r_s1 + 0x50u, r_v0); /* 800A0A80 sh $v0, 0x50($s1) */
    r_v0 = bl_lw(r_s0 + 0x8u); /* 800A0A84 lw $v0, 0x8($s0) */
    r_a0 = r_a0 + 0x2u; /* 800A0A88 addiu $a0, $a0, 0x2 */
    r_v0 = r_v0 + 0x2u; /* 800A0A8C addiu $v0, $v0, 0x2 */
    bl_sw(r_s0 + 0x8u, r_v0); /* 800A0A90 sw $v0, 0x8($s0) */
    r_v0 = r_fp & 0x20u; /* 800A0A94 andi $v0, $fp, 0x20 */
L_800A0A98:
    /* 800A0A98 bnez $v0, .L800A0AC8 */
    { int c_ = (r_v0 != 0); r_v0 = r_fp & 0x40u; if (c_) goto L_800A0AC8; }
    r_v0 = bl_lhu(r_s1 + 0x52u); /* 800A0AA0 lhu $v0, 0x52($s1) */
    r_v1 = bl_lhu(r_a0); /* 800A0AA4 lhu $v1, 0x0($a0) */
    ; /* 800A0AA8 nop  */
    r_v0 = r_v0 + r_v1; /* 800A0AAC addu $v0, $v0, $v1 */
    bl_sh(r_s1 + 0x52u, r_v0); /* 800A0AB0 sh $v0, 0x52($s1) */
    r_v0 = bl_lw(r_s0 + 0x8u); /* 800A0AB4 lw $v0, 0x8($s0) */
    r_a0 = r_a0 + 0x2u; /* 800A0AB8 addiu $a0, $a0, 0x2 */
    r_v0 = r_v0 + 0x2u; /* 800A0ABC addiu $v0, $v0, 0x2 */
    bl_sw(r_s0 + 0x8u, r_v0); /* 800A0AC0 sw $v0, 0x8($s0) */
    r_v0 = r_fp & 0x40u; /* 800A0AC4 andi $v0, $fp, 0x40 */
L_800A0AC8:
    /* 800A0AC8 bnez $v0, .L800A0F48 */
    { int c_ = (r_v0 != 0); ; if (c_) goto L_800A0F48; }
    r_v0 = bl_lhu(r_s1 + 0x54u); /* 800A0AD0 lhu $v0, 0x54($s1) */
    r_v1 = bl_lhu(r_a0); /* 800A0AD4 lhu $v1, 0x0($a0) */
    ; /* 800A0AD8 nop  */
    r_v0 = r_v0 + r_v1; /* 800A0ADC addu $v0, $v0, $v1 */
    bl_sh(r_s1 + 0x54u, r_v0); /* 800A0AE0 sh $v0, 0x54($s1) */
L_800A0AE4:
    r_v0 = bl_lw(r_s0 + 0x8u); /* 800A0AE4 lw $v0, 0x8($s0) */
    ; /* 800A0AE8 nop  */
    r_v0 = r_v0 + 0x2u; /* 800A0AEC addiu $v0, $v0, 0x2 */
    /* 800A0AF0 j .L800A0F48 */
    bl_sw(r_s0 + 0x8u, r_v0); goto L_800A0F48;
L_800A0AF8:
    r_a0 = bl_lhu(r_s0 + 0x10u); /* 800A0AF8 lhu $a0, 0x10($s0) */
    r_v0 = bl_lh(r_s0 + 0xAu); /* 800A0AFC lh $v0, 0xA($s0) */
    r_a0 = r_a0 + 0x1u; /* 800A0B00 addiu $a0, $a0, 0x1 */
    r_a0 = r_a0 << 16; /* 800A0B04 sll $a0, $a0, 16 */
    r_a0 = BL_SRA(r_a0, 16); /* 800A0B08 sra $a0, $a0, 16 */
    BL_MULT(r_v0, r_a0); /* 800A0B0C mult $v0, $a0 */
    r_v1 = lo; /* 800A0B10 mflo $v1 */
    r_v0 = bl_lh(r_s0 + 0x12u); /* 800A0B14 lh $v0, 0x12($s0) */
    ; /* 800A0B18 nop  */
    BL_DIV(r_v1, r_v0); /* 800A0B1C div $zero, $v1, $v0 */
    /* 800A0B20 bnez $v0, .L800A0B2C */
    { int c_ = (r_v0 != 0); ; if (c_) goto L_800A0B2C; }
    bl_fail(FN, "BREAK", 7u); /* 800A0B28 break 7 */
L_800A0B2C:
    r_at = 0u + 0xFFFFFFFFu; /* 800A0B2C addiu $at, $zero, -0x1 */
    /* 800A0B30 bne $v0, $at, .L800A0B44 */
    { int c_ = (r_v0 != r_at); r_at = 0x80000000u; if (c_) goto L_800A0B44; }
    /* 800A0B38 bne $v1, $at, .L800A0B44 */
    { int c_ = (r_v1 != r_at); ; if (c_) goto L_800A0B44; }
    bl_fail(FN, "BREAK", 6u); /* 800A0B40 break 6 */
L_800A0B44:
    r_v1 = lo; /* 800A0B44 mflo $v1 */
    r_v0 = bl_lhu(r_s0 + 0x4u); /* 800A0B48 lhu $v0, 0x4($s0) */
    ; /* 800A0B4C nop  */
    r_v0 = r_v0 + r_v1; /* 800A0B50 addu $v0, $v0, $v1 */
    bl_sh(r_s1 + 0x50u, r_v0); /* 800A0B54 sh $v0, 0x50($s1) */
    r_v0 = bl_lh(r_s0 + 0xCu); /* 800A0B58 lh $v0, 0xC($s0) */
    ; /* 800A0B5C nop  */
    BL_MULT(r_v0, r_a0); /* 800A0B60 mult $v0, $a0 */
    r_v1 = lo; /* 800A0B64 mflo $v1 */
    r_v0 = bl_lh(r_s0 + 0x12u); /* 800A0B68 lh $v0, 0x12($s0) */
    ; /* 800A0B6C nop  */
    BL_DIV(r_v1, r_v0); /* 800A0B70 div $zero, $v1, $v0 */
    /* 800A0B74 bnez $v0, .L800A0B80 */
    { int c_ = (r_v0 != 0); ; if (c_) goto L_800A0B80; }
    bl_fail(FN, "BREAK", 7u); /* 800A0B7C break 7 */
L_800A0B80:
    r_at = 0u + 0xFFFFFFFFu; /* 800A0B80 addiu $at, $zero, -0x1 */
    /* 800A0B84 bne $v0, $at, .L800A0B98 */
    { int c_ = (r_v0 != r_at); r_at = 0x80000000u; if (c_) goto L_800A0B98; }
    /* 800A0B8C bne $v1, $at, .L800A0B98 */
    { int c_ = (r_v1 != r_at); ; if (c_) goto L_800A0B98; }
    bl_fail(FN, "BREAK", 6u); /* 800A0B94 break 6 */
L_800A0B98:
    r_v1 = lo; /* 800A0B98 mflo $v1 */
    r_v0 = bl_lhu(r_s0 + 0x6u); /* 800A0B9C lhu $v0, 0x6($s0) */
    ; /* 800A0BA0 nop  */
    r_v0 = r_v0 + r_v1; /* 800A0BA4 addu $v0, $v0, $v1 */
    bl_sh(r_s1 + 0x52u, r_v0); /* 800A0BA8 sh $v0, 0x52($s1) */
    r_v0 = bl_lh(r_s0 + 0xEu); /* 800A0BAC lh $v0, 0xE($s0) */
    ; /* 800A0BB0 nop  */
    BL_MULT(r_v0, r_a0); /* 800A0BB4 mult $v0, $a0 */
    r_v1 = lo; /* 800A0BB8 mflo $v1 */
    r_v0 = bl_lh(r_s0 + 0x12u); /* 800A0BBC lh $v0, 0x12($s0) */
    ; /* 800A0BC0 nop  */
    BL_DIV(r_v1, r_v0); /* 800A0BC4 div $zero, $v1, $v0 */
    /* 800A0BC8 bnez $v0, .L800A0BD4 */
    { int c_ = (r_v0 != 0); ; if (c_) goto L_800A0BD4; }
    bl_fail(FN, "BREAK", 7u); /* 800A0BD0 break 7 */
L_800A0BD4:
    r_at = 0u + 0xFFFFFFFFu; /* 800A0BD4 addiu $at, $zero, -0x1 */
    /* 800A0BD8 bne $v0, $at, .L800A0BEC */
    { int c_ = (r_v0 != r_at); r_at = 0x80000000u; if (c_) goto L_800A0BEC; }
    /* 800A0BE0 bne $v1, $at, .L800A0BEC */
    { int c_ = (r_v1 != r_at); ; if (c_) goto L_800A0BEC; }
    bl_fail(FN, "BREAK", 6u); /* 800A0BE8 break 6 */
L_800A0BEC:
    r_v1 = lo; /* 800A0BEC mflo $v1 */
    r_v0 = bl_lhu(r_s0 + 0x8u); /* 800A0BF0 lhu $v0, 0x8($s0) */
    /* 800A0BF4 j .L800A0D90 */
    r_s6 = r_s0 + 0u; goto L_800A0D90;
L_800A0BFC:
    r_a0 = bl_lh(r_s0 + 0xAu); /* 800A0BFC lh $a0, 0xA($s0) */
    r_v0 = bl_lh(r_s1 + 0x50u); /* 800A0C00 lh $v0, 0x50($s1) */
    r_a1 = bl_lh(r_s0 + 0x12u); /* 800A0C04 lh $a1, 0x12($s0) */
    r_a0 = r_a0 - r_v0; /* 800A0C08 subu $a0, $a0, $v0 */
    BL_DIV(r_a0, r_a1); /* 800A0C0C div $zero, $a0, $a1 */
    /* 800A0C10 bnez $a1, .L800A0C1C */
    { int c_ = (r_a1 != 0); ; if (c_) goto L_800A0C1C; }
    bl_fail(FN, "BREAK", 7u); /* 800A0C18 break 7 */
L_800A0C1C:
    r_at = 0u + 0xFFFFFFFFu; /* 800A0C1C addiu $at, $zero, -0x1 */
    /* 800A0C20 bne $a1, $at, .L800A0C34 */
    { int c_ = (r_a1 != r_at); r_at = 0x80000000u; if (c_) goto L_800A0C34; }
    /* 800A0C28 bne $a0, $at, .L800A0C34 */
    { int c_ = (r_a0 != r_at); ; if (c_) goto L_800A0C34; }
    bl_fail(FN, "BREAK", 6u); /* 800A0C30 break 6 */
L_800A0C34:
    r_a0 = lo; /* 800A0C34 mflo $a0 */
    r_v1 = bl_lh(r_s1 + 0x52u); /* 800A0C38 lh $v1, 0x52($s1) */
    r_v0 = bl_lh(r_s0 + 0xCu); /* 800A0C3C lh $v0, 0xC($s0) */
    ; /* 800A0C40 nop  */
    r_v0 = r_v0 - r_v1; /* 800A0C44 subu $v0, $v0, $v1 */
    BL_DIV(r_v0, r_a1); /* 800A0C48 div $zero, $v0, $a1 */
    /* 800A0C4C bnez $a1, .L800A0C58 */
    { int c_ = (r_a1 != 0); ; if (c_) goto L_800A0C58; }
    bl_fail(FN, "BREAK", 7u); /* 800A0C54 break 7 */
L_800A0C58:
    r_at = 0u + 0xFFFFFFFFu; /* 800A0C58 addiu $at, $zero, -0x1 */
    /* 800A0C5C bne $a1, $at, .L800A0C70 */
    { int c_ = (r_a1 != r_at); r_at = 0x80000000u; if (c_) goto L_800A0C70; }
    /* 800A0C64 bne $v0, $at, .L800A0C70 */
    { int c_ = (r_v0 != r_at); ; if (c_) goto L_800A0C70; }
    bl_fail(FN, "BREAK", 6u); /* 800A0C6C break 6 */
L_800A0C70:
    r_a2 = lo; /* 800A0C70 mflo $a2 */
    r_v0 = bl_lh(r_s0 + 0xEu); /* 800A0C74 lh $v0, 0xE($s0) */
    r_v1 = bl_lh(r_s1 + 0x54u); /* 800A0C78 lh $v1, 0x54($s1) */
    ; /* 800A0C7C nop  */
    r_v0 = r_v0 - r_v1; /* 800A0C80 subu $v0, $v0, $v1 */
    BL_DIV(r_v0, r_a1); /* 800A0C84 div $zero, $v0, $a1 */
    /* 800A0C88 bnez $a1, .L800A0C94 */
    { int c_ = (r_a1 != 0); ; if (c_) goto L_800A0C94; }
    bl_fail(FN, "BREAK", 7u); /* 800A0C90 break 7 */
L_800A0C94:
    r_at = 0u + 0xFFFFFFFFu; /* 800A0C94 addiu $at, $zero, -0x1 */
    /* 800A0C98 bne $a1, $at, .L800A0CAC */
    { int c_ = (r_a1 != r_at); r_at = 0x80000000u; if (c_) goto L_800A0CAC; }
    /* 800A0CA0 bne $v0, $at, .L800A0CAC */
    { int c_ = (r_v0 != r_at); ; if (c_) goto L_800A0CAC; }
    bl_fail(FN, "BREAK", 6u); /* 800A0CA8 break 6 */
L_800A0CAC:
    r_v1 = lo; /* 800A0CAC mflo $v1 */
    r_s6 = r_s0 + 0u; /* 800A0CB0 addu $s6, $s0, $zero */
    r_a3 = r_a0 + 0u; /* 800A0CB4 addu $a3, $a0, $zero */
    r_a0 = r_a0 << 16; /* 800A0CB8 sll $a0, $a0, 16 */
    r_t0 = r_a2 + 0u; /* 800A0CBC addu $t0, $a2, $zero */
    /* 800A0CC0 bnez $a0, .L800A0D00 */
    { int c_ = (r_a0 != 0); r_t1 = r_v1 + 0u; if (c_) goto L_800A0D00; }
    r_v0 = r_a2 << 16; /* 800A0CC8 sll $v0, $a2, 16 */
    /* 800A0CCC bnez $v0, .L800A0D00 */
    { int c_ = (r_v0 != 0); r_v0 = r_v1 << 16; if (c_) goto L_800A0D00; }
    /* 800A0CD4 bnez $v0, .L800A0D00 */
    { int c_ = (r_v0 != 0); ; if (c_) goto L_800A0D00; }
    r_v0 = bl_lhu(r_s0 + 0xAu); /* 800A0CDC lhu $v0, 0xA($s0) */
    bl_sh(r_s0 + 0x10u, r_a1); /* 800A0CE0 sh $a1, 0x10($s0) */
    bl_sh(r_s1 + 0x50u, r_v0); /* 800A0CE4 sh $v0, 0x50($s1) */
    r_v0 = bl_lhu(r_s0 + 0xCu); /* 800A0CE8 lhu $v0, 0xC($s0) */
    ; /* 800A0CEC nop  */
    bl_sh(r_s1 + 0x52u, r_v0); /* 800A0CF0 sh $v0, 0x52($s1) */
    r_v0 = bl_lhu(r_s0 + 0xEu); /* 800A0CF4 lhu $v0, 0xE($s0) */
    /* 800A0CF8 j .L800A0F48 */
    bl_sh(r_s1 + 0x54u, r_v0); goto L_800A0F48;
L_800A0D00:
    r_v0 = bl_lhu(r_s1 + 0x50u); /* 800A0D00 lhu $v0, 0x50($s1) */
    r_v1 = bl_lhu(r_s1 + 0x54u); /* 800A0D04 lhu $v1, 0x54($s1) */
    r_v0 = r_a3 + r_v0; /* 800A0D08 addu $v0, $a3, $v0 */
    bl_sh(r_s1 + 0x50u, r_v0); /* 800A0D0C sh $v0, 0x50($s1) */
    r_v0 = bl_lhu(r_s1 + 0x52u); /* 800A0D10 lhu $v0, 0x52($s1) */
    r_v1 = r_t1 + r_v1; /* 800A0D14 addu $v1, $t1, $v1 */
    bl_sh(r_s1 + 0x54u, r_v1); /* 800A0D18 sh $v1, 0x54($s1) */
    r_v0 = r_t0 + r_v0; /* 800A0D1C addu $v0, $t0, $v0 */
    bl_sh(r_s1 + 0x52u, r_v0); /* 800A0D20 sh $v0, 0x52($s1) */
    /* 800A0D24 j .L800A0F48 */
    bl_sh(r_s0 + 0x10u, 0u); goto L_800A0F48;
L_800A0D2C:
    r_v1 = bl_lhu(r_s0 + 0x4u); /* 800A0D2C lhu $v1, 0x4($s0) */
    r_v0 = bl_lhu(r_s0 + 0xAu); /* 800A0D30 lhu $v0, 0xA($s0) */
    ; /* 800A0D34 nop  */
    r_v1 = r_v1 + r_v0; /* 800A0D38 addu $v1, $v1, $v0 */
    bl_sh(r_s0 + 0x4u, r_v1); /* 800A0D3C sh $v1, 0x4($s0) */
    r_v0 = bl_lhu(r_s1 + 0x50u); /* 800A0D40 lhu $v0, 0x50($s1) */
    ; /* 800A0D44 nop  */
    r_v0 = r_v0 + r_v1; /* 800A0D48 addu $v0, $v0, $v1 */
    bl_sh(r_s1 + 0x50u, r_v0); /* 800A0D4C sh $v0, 0x50($s1) */
    r_v1 = bl_lhu(r_s0 + 0x6u); /* 800A0D50 lhu $v1, 0x6($s0) */
    r_v0 = bl_lhu(r_s0 + 0xCu); /* 800A0D54 lhu $v0, 0xC($s0) */
    ; /* 800A0D58 nop  */
    r_v1 = r_v1 + r_v0; /* 800A0D5C addu $v1, $v1, $v0 */
    bl_sh(r_s0 + 0x6u, r_v1); /* 800A0D60 sh $v1, 0x6($s0) */
    r_v0 = bl_lhu(r_s1 + 0x52u); /* 800A0D64 lhu $v0, 0x52($s1) */
    ; /* 800A0D68 nop  */
    r_v0 = r_v0 + r_v1; /* 800A0D6C addu $v0, $v0, $v1 */
    bl_sh(r_s1 + 0x52u, r_v0); /* 800A0D70 sh $v0, 0x52($s1) */
    r_v1 = bl_lhu(r_s0 + 0x8u); /* 800A0D74 lhu $v1, 0x8($s0) */
    r_v0 = bl_lhu(r_s0 + 0xEu); /* 800A0D78 lhu $v0, 0xE($s0) */
    ; /* 800A0D7C nop  */
    r_v1 = r_v1 + r_v0; /* 800A0D80 addu $v1, $v1, $v0 */
    bl_sh(r_s0 + 0x8u, r_v1); /* 800A0D84 sh $v1, 0x8($s0) */
    r_v0 = bl_lhu(r_s1 + 0x54u); /* 800A0D88 lhu $v0, 0x54($s1) */
    r_s6 = r_s0 + 0u; /* 800A0D8C addu $s6, $s0, $zero */
L_800A0D90:
    r_v0 = r_v0 + r_v1; /* 800A0D90 addu $v0, $v0, $v1 */
    /* 800A0D94 j .L800A0F48 */
    bl_sh(r_s1 + 0x54u, r_v0); goto L_800A0F48;
L_800A0D9C:
    r_v1 = bl_lh(r_s0 + 0xAu); /* 800A0D9C lh $v1, 0xA($s0) */
    r_v0 = bl_lw(r_s1 + 0x58u); /* 800A0DA0 lw $v0, 0x58($s1) */
    ; /* 800A0DA4 nop  */
    r_s3 = r_v1 - r_v0; /* 800A0DA8 subu $s3, $v1, $v0 */
    BL_MULT(r_s3, r_s3); /* 800A0DAC mult $s3, $s3 */
    r_v1 = bl_lh(r_s0 + 0xCu); /* 800A0DB0 lh $v1, 0xC($s0) */
    r_v0 = bl_lw(r_s1 + 0x5Cu); /* 800A0DB4 lw $v0, 0x5C($s1) */
    r_a1 = lo; /* 800A0DB8 mflo $a1 */
    r_s7 = r_v1 - r_v0; /* 800A0DBC subu $s7, $v1, $v0 */
    ; /* 800A0DC0 nop  */
    BL_MULT(r_s7, r_s7); /* 800A0DC4 mult $s7, $s7 */
    r_v1 = bl_lh(r_s0 + 0xEu); /* 800A0DC8 lh $v1, 0xE($s0) */
    r_v0 = bl_lw(r_s1 + 0x60u); /* 800A0DCC lw $v0, 0x60($s1) */
    r_a0 = lo; /* 800A0DD0 mflo $a0 */
    r_s2 = r_v1 - r_v0; /* 800A0DD4 subu $s2, $v1, $v0 */
    ; /* 800A0DD8 nop  */
    BL_MULT(r_s2, r_s2); /* 800A0DDC mult $s2, $s2 */
    r_a1 = r_a1 + r_a0; /* 800A0DE0 addu $a1, $a1, $a0 */
    r_a0 = lo; /* 800A0DE4 mflo $a0 */
    /* 800A0DE8 jal SquareRoot0 */
    r_a0 = r_a1 + r_a0;
    r_v0 = (u32)SquareRoot0((int)(uintptr_t)bg_arg(r_a0)); /* SquareRoot0 */
    r_a0 = 0u - r_s3; /* 800A0DF0 negu $a0, $s3 */
    r_a1 = 0u - r_s2; /* 800A0DF4 negu $a1, $s2 */
    /* 800A0DF8 jal ratan2 */
    r_s4 = r_v0 + 0x1u;
    r_v0 = (u32)ratan2((int)(uintptr_t)bg_arg(r_a0), (int)(uintptr_t)bg_arg(r_a1)); /* ratan2 */
    r_v1 = bl_lh(r_s1 + 0x52u); /* 800A0E00 lh $v1, 0x52($s1) */
    r_s6 = r_s0 + 0u; /* 800A0E04 addu $s6, $s0, $zero */
    r_v0 = r_v0 - r_v1; /* 800A0E08 subu $v0, $v0, $v1 */
    r_a0 = r_v0 & 0xFFFu; /* 800A0E0C andi $a0, $v0, 0xFFF */
    r_v0 = (u32)((s32)r_a0 < 2048); /* 800A0E10 slti $v0, $a0, 0x800 */
    /* 800A0E14 bnez $v0, .L800A0E20 */
    { int c_ = (r_v0 != 0); r_a1 = r_v1 + 0u; if (c_) goto L_800A0E20; }
    r_a0 = r_a0 + 0xFFFFF000u; /* 800A0E1C addiu $a0, $a0, -0x1000 */
L_800A0E20:
    r_v0 = bl_lh(r_s0 + 0x10u); /* 800A0E20 lh $v0, 0x10($s0) */
    r_v1 = bl_lh(r_s0 + 0x8u); /* 800A0E24 lh $v1, 0x8($s0) */
    r_v0 = r_s4 + r_v0; /* 800A0E28 addu $v0, $s4, $v0 */
    BL_MULT(r_v0, r_v1); /* 800A0E2C mult $v0, $v1 */
    r_v1 = lo; /* 800A0E30 mflo $v1 */
    r_v0 = bl_lh(r_s0 + 0x4u); /* 800A0E34 lh $v0, 0x4($s0) */
    ; /* 800A0E38 nop  */
    BL_DIV(r_v1, r_v0); /* 800A0E3C div $zero, $v1, $v0 */
    /* 800A0E40 bnez $v0, .L800A0E4C */
    { int c_ = (r_v0 != 0); ; if (c_) goto L_800A0E4C; }
    bl_fail(FN, "BREAK", 7u); /* 800A0E48 break 7 */
L_800A0E4C:
    r_at = 0u + 0xFFFFFFFFu; /* 800A0E4C addiu $at, $zero, -0x1 */
    /* 800A0E50 bne $v0, $at, .L800A0E64 */
    { int c_ = (r_v0 != r_at); r_at = 0x80000000u; if (c_) goto L_800A0E64; }
    /* 800A0E58 bne $v1, $at, .L800A0E64 */
    { int c_ = (r_v1 != r_at); ; if (c_) goto L_800A0E64; }
    bl_fail(FN, "BREAK", 6u); /* 800A0E60 break 6 */
L_800A0E64:
    r_v1 = lo; /* 800A0E64 mflo $v1 */
    r_v0 = bl_lh(r_s0 + 0x6u); /* 800A0E68 lh $v0, 0x6($s0) */
    ; /* 800A0E6C nop  */
    r_s0 = r_v0 + r_v1; /* 800A0E70 addu $s0, $v0, $v1 */
    /* 800A0E74 bgez $a0, .L800A0E80 */
    { int c_ = ((s32)r_a0 >= 0); r_v0 = r_a0 + 0u; if (c_) goto L_800A0E80; }
    r_v0 = 0u - r_v0; /* 800A0E7C negu $v0, $v0 */
L_800A0E80:
    r_v0 = (u32)((s32)r_v0 < (s32)r_s0); /* 800A0E80 slt $v0, $v0, $s0 */
    /* 800A0E84 bnez $v0, .L800A0E98 */
    { int c_ = (r_v0 != 0); r_v0 = r_a1 + r_a0; if (c_) goto L_800A0E98; }
    /* 800A0E8C bgez $a0, .L800A0E98 */
    { int c_ = ((s32)r_a0 >= 0); r_v0 = r_a1 + r_s0; if (c_) goto L_800A0E98; }
    r_v0 = r_a1 - r_s0; /* 800A0E94 subu $v0, $a1, $s0 */
L_800A0E98:
    bl_sh(r_s1 + 0x52u, r_v0); /* 800A0E98 sh $v0, 0x52($s1) */
    r_v1 = r_fp & 0xFu; /* 800A0E9C andi $v1, $fp, 0xF */
    r_v0 = 0u | 0x7u; /* 800A0EA0 ori $v0, $zero, 0x7 */
    /* 800A0EA4 bne $v1, $v0, .L800A0F1C */
    { int c_ = (r_v1 != r_v0); BL_MULT(r_s3, r_s3); if (c_) goto L_800A0F1C; }
    r_v0 = lo; /* 800A0EAC mflo $v0 */
    ; /* 800A0EB0 nop  */
    ; /* 800A0EB4 nop  */
    BL_MULT(r_s2, r_s2); /* 800A0EB8 mult $s2, $s2 */
    r_a0 = lo; /* 800A0EBC mflo $a0 */
    /* 800A0EC0 jal SquareRoot0 */
    r_a0 = r_v0 + r_a0;
    r_v0 = (u32)SquareRoot0((int)(uintptr_t)bg_arg(r_a0)); /* SquareRoot0 */
    r_a0 = r_s7 + 0u; /* 800A0EC8 addu $a0, $s7, $zero */
    /* 800A0ECC jal ratan2 */
    r_a1 = r_v0 + 0u;
    r_v0 = (u32)ratan2((int)(uintptr_t)bg_arg(r_a0), (int)(uintptr_t)bg_arg(r_a1)); /* ratan2 */
    r_v1 = bl_lh(r_s1 + 0x50u); /* 800A0ED4 lh $v1, 0x50($s1) */
    ; /* 800A0ED8 nop  */
    r_v0 = r_v0 - r_v1; /* 800A0EDC subu $v0, $v0, $v1 */
    r_a0 = r_v0 & 0xFFFu; /* 800A0EE0 andi $a0, $v0, 0xFFF */
    r_v0 = (u32)((s32)r_a0 < 2048); /* 800A0EE4 slti $v0, $a0, 0x800 */
    /* 800A0EE8 bnez $v0, .L800A0EF4 */
    { int c_ = (r_v0 != 0); ; if (c_) goto L_800A0EF4; }
    r_a0 = r_a0 + 0xFFFFF000u; /* 800A0EF0 addiu $a0, $a0, -0x1000 */
L_800A0EF4:
    /* 800A0EF4 bgez $a0, .L800A0F00 */
    { int c_ = ((s32)r_a0 >= 0); r_v0 = r_a0 + 0u; if (c_) goto L_800A0F00; }
    r_v0 = 0u - r_v0; /* 800A0EFC negu $v0, $v0 */
L_800A0F00:
    r_v0 = (u32)((s32)r_v0 < (s32)r_s0); /* 800A0F00 slt $v0, $v0, $s0 */
    /* 800A0F04 bnez $v0, .L800A0F18 */
    { int c_ = (r_v0 != 0); r_v0 = r_v1 + r_a0; if (c_) goto L_800A0F18; }
    /* 800A0F0C bgez $a0, .L800A0F18 */
    { int c_ = ((s32)r_a0 >= 0); r_v0 = r_v1 + r_s0; if (c_) goto L_800A0F18; }
    r_v0 = r_v1 - r_s0; /* 800A0F14 subu $v0, $v1, $s0 */
L_800A0F18:
    bl_sh(r_s1 + 0x50u, r_v0); /* 800A0F18 sh $v0, 0x50($s1) */
L_800A0F1C:
    r_v0 = bl_lh(r_s6 + 0x10u); /* 800A0F1C lh $v0, 0x10($s6) */
    ; /* 800A0F20 nop  */
    r_v1 = r_v0 + 0u; /* 800A0F24 addu $v1, $v0, $zero */
    r_v0 = (u32)((s32)r_v0 < 32000); /* 800A0F28 slti $v0, $v0, 0x7D00 */
    /* 800A0F2C beqz $v0, .L800A1024 */
    { int c_ = (r_v0 == 0); r_v0 = 0u | 0x1u; if (c_) goto L_800A1024; }
    r_v0 = bl_lhu(r_s6 + 0x12u); /* 800A0F34 lhu $v0, 0x12($s6) */
    ; /* 800A0F38 nop  */
    r_v0 = r_v1 + r_v0; /* 800A0F3C addu $v0, $v1, $v0 */
    /* 800A0F40 j .L800A1020 */
    bl_sh(r_s6 + 0x10u, r_v0); goto L_800A1020;
L_800A0F48:
    r_v0 = bl_lhu(r_s0 + 0x10u); /* 800A0F48 lhu $v0, 0x10($s0) */
    r_v1 = bl_lh(r_s0 + 0x12u); /* 800A0F4C lh $v1, 0x12($s0) */
    r_v0 = r_v0 + 0x1u; /* 800A0F50 addiu $v0, $v0, 0x1 */
    bl_sh(r_s0 + 0x10u, r_v0); /* 800A0F54 sh $v0, 0x10($s0) */
    r_v0 = r_v0 << 16; /* 800A0F58 sll $v0, $v0, 16 */
    r_v0 = BL_SRA(r_v0, 16); /* 800A0F5C sra $v0, $v0, 16 */
    r_v0 = (u32)((s32)r_v0 < (s32)r_v1); /* 800A0F60 slt $v0, $v0, $v1 */
    /* 800A0F64 bnez $v0, .L800A1004 */
    { int c_ = (r_v0 != 0); ; if (c_) goto L_800A1004; }
    r_v0 = bl_lbu(r_s0 + 0x1u); /* 800A0F6C lbu $v0, 0x1($s0) */
    ; /* 800A0F70 nop  */
    /* 800A0F74 bnez $v0, .L800A0FAC */
    { int c_ = (r_v0 != 0); ; if (c_) goto L_800A0FAC; }
    r_v0 = bl_lbu(r_s0 + 0x3u); /* 800A0F7C lbu $v0, 0x3($s0) */
    r_t3 = bl_lw(r_sp + 0x38u); /* 800A0F80 lw $t3, 0x38($sp) */
    ; /* 800A0F84 nop  */
    /* 800A0F88 bne $v0, $t3, .L800A0F94 */
    { int c_ = (r_v0 != r_t3); ; if (c_) goto L_800A0F94; }
    r_s5 = r_s5 | 0x2u; /* 800A0F90 ori $s5, $s5, 0x2 */
L_800A0F94:
    r_s5 = r_s5 | 0x200u; /* 800A0F94 ori $s5, $s5, 0x200 */
    r_a0 = bl_lw(r_sp + 0x28u); /* 800A0F98 lw $a0, 0x28($sp) */
    /* 800A0F9C jal func_800A23E8 */
    r_a1 = r_s0 + 0u;
    r_v0 = bl_call_guest(FN, 0x800A23E8u, r_a0, r_a1, r_a2, r_a3, r_sp); /* func_800A23E8 */
    /* 800A0FA4 j .L800A1020 */
    bl_sw(r_s1 + 0x6Cu, 0u); goto L_800A1020;
L_800A0FAC:
    r_v0 = bl_lbu(r_s0 + 0x3u); /* 800A0FAC lbu $v0, 0x3($s0) */
    r_t4 = bl_lw(r_sp + 0x38u); /* 800A0FB0 lw $t4, 0x38($sp) */
    ; /* 800A0FB4 nop  */
    /* 800A0FB8 bne $v0, $t4, .L800A0FC4 */
    { int c_ = (r_v0 != r_t4); r_v1 = r_fp & 0xFu; if (c_) goto L_800A0FC4; }
    r_s5 = r_s5 | 0x4u; /* 800A0FC0 ori $s5, $s5, 0x4 */
L_800A0FC4:
    r_v0 = (u32)(r_v1 < 0x3u); /* 800A0FC4 sltiu $v0, $v1, 0x3 */
    /* 800A0FC8 beqz $v0, .L800A0FE0 */
    { int c_ = (r_v0 == 0); r_s5 = r_s5 | 0x400u; if (c_) goto L_800A0FE0; }
    r_v0 = bl_lw(r_s0 + 0x4u); /* 800A0FD0 lw $v0, 0x4($s0) */
    bl_sh(r_s0 + 0x10u, 0u); /* 800A0FD4 sh $zero, 0x10($s0) */
    /* 800A0FD8 j .L800A1020 */
    bl_sw(r_s0 + 0x8u, r_v0); goto L_800A1020;
L_800A0FE0:
    r_v0 = 0u + 0xFFFFFFFFu; /* 800A0FE0 addiu $v0, $zero, -0x1 */
    bl_sh(r_s0 + 0x10u, r_v0); /* 800A0FE4 sh $v0, 0x10($s0) */
    r_v0 = 0u | 0x5u; /* 800A0FE8 ori $v0, $zero, 0x5 */
    /* 800A0FEC bne $v1, $v0, .L800A1024 */
    { int c_ = (r_v1 != r_v0); r_v0 = 0u | 0x1u; if (c_) goto L_800A1024; }
    bl_sh(r_s6 + 0xAu, 0u); /* 800A0FF4 sh $zero, 0xA($s6) */
    bl_sh(r_s6 + 0xCu, 0u); /* 800A0FF8 sh $zero, 0xC($s6) */
    /* 800A0FFC j .L800A1024 */
    bl_sh(r_s6 + 0xEu, 0u); goto L_800A1024;
L_800A1004:
    r_v0 = bl_lbu(r_s0 + 0x3u); /* 800A1004 lbu $v0, 0x3($s0) */
    r_t3 = bl_lw(r_sp + 0x38u); /* 800A1008 lw $t3, 0x38($sp) */
    ; /* 800A100C nop  */
    /* 800A1010 bne $v0, $t3, .L800A101C */
    { int c_ = (r_v0 != r_t3); ; if (c_) goto L_800A101C; }
    r_s5 = r_s5 | 0x1u; /* 800A1018 ori $s5, $s5, 0x1 */
L_800A101C:
    r_s5 = r_s5 | 0x100u; /* 800A101C ori $s5, $s5, 0x100 */
L_800A1020:
    r_v0 = 0u | 0x1u; /* 800A1020 ori $v0, $zero, 0x1 */
L_800A1024:
    bl_sb(r_s1 + 0x1u, r_v0); /* 800A1024 sb $v0, 0x1($s1) */
    bl_sb(r_s1, r_v0); /* 800A1028 sb $v0, 0x0($s1) */
L_800A102C:
    r_a1 = bl_lw(r_s1 + 0x70u); /* 800A102C lw $a1, 0x70($s1) */
    ; /* 800A1030 nop  */
    /* 800A1034 beqz $a1, .L800A1710 */
    { int c_ = (r_a1 == 0); r_s0 = r_a1 + 0u; if (c_) goto L_800A1710; }
    r_s2 = bl_lbu(r_s0 + 0x2u); /* 800A103C lbu $s2, 0x2($s0) */
    ; /* 800A1040 nop  */
    r_v1 = r_s2 & 0xFu; /* 800A1044 andi $v1, $s2, 0xF */
    r_v0 = (u32)(r_v1 < 0x6u); /* 800A1048 sltiu $v0, $v1, 0x6 */
    /* 800A104C beqz $v0, .L800A1630 */
    { int c_ = (r_v0 == 0); r_v0 = r_v1 << 2; if (c_) goto L_800A1630; }
    r_at = 0x80070000u; /* 800A1054 lui $at, %hi(jtbl_8007053C) */
    r_at = r_at + r_v0; /* 800A1058 addu $at, $at, $v0 */
    r_v0 = bl_lw(r_at + 0x53Cu); /* 800A105C lw $v0, %lo(jtbl_8007053C)($at) */
    ; /* 800A1060 nop  */
    /* 800A1064 jr $v0 */
    { u32 t_ = r_v0; ; switch (t_) { case 0x800A08CCu: goto L_800A08CC; case 0x800A0938u: goto L_800A0938; case 0x800A0A60u: goto L_800A0A60; case 0x800A0AF8u: goto L_800A0AF8; case 0x800A0BFCu: goto L_800A0BFC; case 0x800A0D2Cu: goto L_800A0D2C; case 0x800A0D9Cu: goto L_800A0D9C; case 0x800A0F48u: goto L_800A0F48; case 0x800A106Cu: goto L_800A106C; case 0x800A10ECu: goto L_800A10EC; case 0x800A120Cu: goto L_800A120C; case 0x800A1368u: goto L_800A1368; case 0x800A146Cu: goto L_800A146C; case 0x800A15B8u: goto L_800A15B8; default: bl_fail(FN, "jump-table target", t_); } }
L_800A106C:
    r_a0 = bl_lw(r_s0 + 0x8u); /* 800A106C lw $a0, 0x8($s0) */
    r_v0 = r_s2 & 0x10u; /* 800A1070 andi $v0, $s2, 0x10 */
    /* 800A1074 bnez $v0, .L800A109C */
    { int c_ = (r_v0 != 0); r_v0 = r_s2 & 0x20u; if (c_) goto L_800A109C; }
    r_v0 = bl_lh(r_a0); /* 800A107C lh $v0, 0x0($a0) */
    ; /* 800A1080 nop  */
    bl_sw(r_s1 + 0x58u, r_v0); /* 800A1084 sw $v0, 0x58($s1) */
    r_v0 = bl_lw(r_s0 + 0x8u); /* 800A1088 lw $v0, 0x8($s0) */
    r_a0 = r_a0 + 0x2u; /* 800A108C addiu $a0, $a0, 0x2 */
    r_v0 = r_v0 + 0x2u; /* 800A1090 addiu $v0, $v0, 0x2 */
    bl_sw(r_s0 + 0x8u, r_v0); /* 800A1094 sw $v0, 0x8($s0) */
    r_v0 = r_s2 & 0x20u; /* 800A1098 andi $v0, $s2, 0x20 */
L_800A109C:
    /* 800A109C bnez $v0, .L800A10C4 */
    { int c_ = (r_v0 != 0); r_v0 = r_s2 & 0x40u; if (c_) goto L_800A10C4; }
    r_v0 = bl_lh(r_a0); /* 800A10A4 lh $v0, 0x0($a0) */
    ; /* 800A10A8 nop  */
    bl_sw(r_s1 + 0x5Cu, r_v0); /* 800A10AC sw $v0, 0x5C($s1) */
    r_v0 = bl_lw(r_s0 + 0x8u); /* 800A10B0 lw $v0, 0x8($s0) */
    r_a0 = r_a0 + 0x2u; /* 800A10B4 addiu $a0, $a0, 0x2 */
    r_v0 = r_v0 + 0x2u; /* 800A10B8 addiu $v0, $v0, 0x2 */
    bl_sw(r_s0 + 0x8u, r_v0); /* 800A10BC sw $v0, 0x8($s0) */
    r_v0 = r_s2 & 0x40u; /* 800A10C0 andi $v0, $s2, 0x40 */
L_800A10C4:
    /* 800A10C4 bnez $v0, .L800A1630 */
    { int c_ = (r_v0 != 0); ; if (c_) goto L_800A1630; }
    r_v0 = bl_lh(r_a0); /* 800A10CC lh $v0, 0x0($a0) */
    ; /* 800A10D0 nop  */
    bl_sw(r_s1 + 0x60u, r_v0); /* 800A10D4 sw $v0, 0x60($s1) */
    r_v0 = bl_lw(r_s0 + 0x8u); /* 800A10D8 lw $v0, 0x8($s0) */
    ; /* 800A10DC nop  */
    r_v0 = r_v0 + 0x2u; /* 800A10E0 addiu $v0, $v0, 0x2 */
    /* 800A10E4 j .L800A1630 */
    bl_sw(r_s0 + 0x8u, r_v0); goto L_800A1630;
L_800A10EC:
    r_v0 = r_s2 & 0x10u; /* 800A10EC andi $v0, $s2, 0x10 */
    /* 800A10F0 bnez $v0, .L800A1150 */
    { int c_ = (r_v0 != 0); r_v0 = r_s2 & 0x20u; if (c_) goto L_800A1150; }
    r_v1 = bl_lw(r_s0 + 0x8u); /* 800A10F8 lw $v1, 0x8($s0) */
    ; /* 800A10FC nop  */
    r_v0 = r_v1 + 0x1u; /* 800A1100 addiu $v0, $v1, 0x1 */
    bl_sw(r_s0 + 0x8u, r_v0); /* 800A1104 sw $v0, 0x8($s0) */
    r_a0 = bl_lb(r_v1); /* 800A1108 lb $a0, 0x0($v1) */
    r_v0 = 0u + 0xFFFFFF80u; /* 800A110C addiu $v0, $zero, -0x80 */
    /* 800A1110 beq $a0, $v0, .L800A1124 */
    { int c_ = (r_a0 == r_v0); ; if (c_) goto L_800A1124; }
    r_v0 = bl_lw(r_s1 + 0x58u); /* 800A1118 lw $v0, 0x58($s1) */
    /* 800A111C j .L800A1148 */
    r_v0 = r_a0 + r_v0; goto L_800A1148;
L_800A1124:
    r_v0 = r_v1 + 0x2u; /* 800A1124 addiu $v0, $v1, 0x2 */
    bl_sw(r_s0 + 0x8u, r_v0); /* 800A1128 sw $v0, 0x8($s0) */
    r_a0 = bl_lbu(r_v1 + 0x1u); /* 800A112C lbu $a0, 0x1($v1) */
    r_v0 = r_v1 + 0x3u; /* 800A1130 addiu $v0, $v1, 0x3 */
    bl_sw(r_s0 + 0x8u, r_v0); /* 800A1134 sw $v0, 0x8($s0) */
    r_v0 = bl_lb(r_v1 + 0x2u); /* 800A1138 lb $v0, 0x2($v1) */
    ; /* 800A113C nop  */
    r_v0 = r_v0 << 8; /* 800A1140 sll $v0, $v0, 8 */
    r_v0 = r_a0 | r_v0; /* 800A1144 or $v0, $a0, $v0 */
L_800A1148:
    bl_sw(r_s1 + 0x58u, r_v0); /* 800A1148 sw $v0, 0x58($s1) */
    r_v0 = r_s2 & 0x20u; /* 800A114C andi $v0, $s2, 0x20 */
L_800A1150:
    /* 800A1150 bnez $v0, .L800A11B0 */
    { int c_ = (r_v0 != 0); r_v0 = r_s2 & 0x40u; if (c_) goto L_800A11B0; }
    r_v1 = bl_lw(r_s0 + 0x8u); /* 800A1158 lw $v1, 0x8($s0) */
    ; /* 800A115C nop  */
    r_v0 = r_v1 + 0x1u; /* 800A1160 addiu $v0, $v1, 0x1 */
    bl_sw(r_s0 + 0x8u, r_v0); /* 800A1164 sw $v0, 0x8($s0) */
    r_a0 = bl_lb(r_v1); /* 800A1168 lb $a0, 0x0($v1) */
    r_v0 = 0u + 0xFFFFFF80u; /* 800A116C addiu $v0, $zero, -0x80 */
    /* 800A1170 beq $a0, $v0, .L800A1184 */
    { int c_ = (r_a0 == r_v0); ; if (c_) goto L_800A1184; }
    r_v0 = bl_lw(r_s1 + 0x5Cu); /* 800A1178 lw $v0, 0x5C($s1) */
    /* 800A117C j .L800A11A8 */
    r_v0 = r_a0 + r_v0; goto L_800A11A8;
L_800A1184:
    r_v0 = r_v1 + 0x2u; /* 800A1184 addiu $v0, $v1, 0x2 */
    bl_sw(r_s0 + 0x8u, r_v0); /* 800A1188 sw $v0, 0x8($s0) */
    r_a0 = bl_lbu(r_v1 + 0x1u); /* 800A118C lbu $a0, 0x1($v1) */
    r_v0 = r_v1 + 0x3u; /* 800A1190 addiu $v0, $v1, 0x3 */
    bl_sw(r_s0 + 0x8u, r_v0); /* 800A1194 sw $v0, 0x8($s0) */
    r_v0 = bl_lb(r_v1 + 0x2u); /* 800A1198 lb $v0, 0x2($v1) */
    ; /* 800A119C nop  */
    r_v0 = r_v0 << 8; /* 800A11A0 sll $v0, $v0, 8 */
    r_v0 = r_a0 | r_v0; /* 800A11A4 or $v0, $a0, $v0 */
L_800A11A8:
    bl_sw(r_s1 + 0x5Cu, r_v0); /* 800A11A8 sw $v0, 0x5C($s1) */
    r_v0 = r_s2 & 0x40u; /* 800A11AC andi $v0, $s2, 0x40 */
L_800A11B0:
    /* 800A11B0 bnez $v0, .L800A1630 */
    { int c_ = (r_v0 != 0); ; if (c_) goto L_800A1630; }
    r_v1 = bl_lw(r_s0 + 0x8u); /* 800A11B8 lw $v1, 0x8($s0) */
    ; /* 800A11BC nop  */
    r_v0 = r_v1 + 0x1u; /* 800A11C0 addiu $v0, $v1, 0x1 */
    bl_sw(r_s0 + 0x8u, r_v0); /* 800A11C4 sw $v0, 0x8($s0) */
    r_a0 = bl_lb(r_v1); /* 800A11C8 lb $a0, 0x0($v1) */
    r_v0 = 0u + 0xFFFFFF80u; /* 800A11CC addiu $v0, $zero, -0x80 */
    /* 800A11D0 beq $a0, $v0, .L800A11E4 */
    { int c_ = (r_a0 == r_v0); ; if (c_) goto L_800A11E4; }
    r_v0 = bl_lw(r_s1 + 0x60u); /* 800A11D8 lw $v0, 0x60($s1) */
    /* 800A11DC j .L800A162C */
    r_v0 = r_a0 + r_v0; goto L_800A162C;
L_800A11E4:
    r_v0 = r_v1 + 0x2u; /* 800A11E4 addiu $v0, $v1, 0x2 */
    bl_sw(r_s0 + 0x8u, r_v0); /* 800A11E8 sw $v0, 0x8($s0) */
    r_a0 = bl_lbu(r_v1 + 0x1u); /* 800A11EC lbu $a0, 0x1($v1) */
    r_v0 = r_v1 + 0x3u; /* 800A11F0 addiu $v0, $v1, 0x3 */
    bl_sw(r_s0 + 0x8u, r_v0); /* 800A11F4 sw $v0, 0x8($s0) */
    r_v0 = bl_lb(r_v1 + 0x2u); /* 800A11F8 lb $v0, 0x2($v1) */
    ; /* 800A11FC nop  */
    r_v0 = r_v0 << 8; /* 800A1200 sll $v0, $v0, 8 */
    /* 800A1204 j .L800A162C */
    r_v0 = r_a0 | r_v0; goto L_800A162C;
L_800A120C:
    r_a0 = bl_lw(r_s0 + 0x8u); /* 800A120C lw $a0, 0x8($s0) */
    r_v0 = r_s2 & 0x10u; /* 800A1210 andi $v0, $s2, 0x10 */
    /* 800A1214 bnez $v0, .L800A123C */
    { int c_ = (r_v0 != 0); ; if (c_) goto L_800A123C; }
    r_v0 = bl_lhu(r_a0); /* 800A121C lhu $v0, 0x0($a0) */
    ; /* 800A1220 nop  */
    bl_sh(r_sp + 0x10u, r_v0); /* 800A1224 sh $v0, 0x10($sp) */
    r_v0 = bl_lw(r_s0 + 0x8u); /* 800A1228 lw $v0, 0x8($s0) */
    r_a0 = r_a0 + 0x2u; /* 800A122C addiu $a0, $a0, 0x2 */
    r_v0 = r_v0 + 0x2u; /* 800A1230 addiu $v0, $v0, 0x2 */
    /* 800A1234 j .L800A1240 */
    bl_sw(r_s0 + 0x8u, r_v0); goto L_800A1240;
L_800A123C:
    bl_sh(r_sp + 0x10u, 0u); /* 800A123C sh $zero, 0x10($sp) */
L_800A1240:
    r_v0 = r_s2 & 0x20u; /* 800A1240 andi $v0, $s2, 0x20 */
    /* 800A1244 bnez $v0, .L800A126C */
    { int c_ = (r_v0 != 0); ; if (c_) goto L_800A126C; }
    r_v0 = bl_lhu(r_a0); /* 800A124C lhu $v0, 0x0($a0) */
    ; /* 800A1250 nop  */
    bl_sh(r_sp + 0x12u, r_v0); /* 800A1254 sh $v0, 0x12($sp) */
    r_v0 = bl_lw(r_s0 + 0x8u); /* 800A1258 lw $v0, 0x8($s0) */
    r_a0 = r_a0 + 0x2u; /* 800A125C addiu $a0, $a0, 0x2 */
    r_v0 = r_v0 + 0x2u; /* 800A1260 addiu $v0, $v0, 0x2 */
    /* 800A1264 j .L800A1270 */
    bl_sw(r_s0 + 0x8u, r_v0); goto L_800A1270;
L_800A126C:
    bl_sh(r_sp + 0x12u, 0u); /* 800A126C sh $zero, 0x12($sp) */
L_800A1270:
    r_v0 = r_s2 & 0x40u; /* 800A1270 andi $v0, $s2, 0x40 */
    /* 800A1274 bnez $v0, .L800A129C */
    { int c_ = (r_v0 != 0); ; if (c_) goto L_800A129C; }
    r_v0 = bl_lhu(r_a0); /* 800A127C lhu $v0, 0x0($a0) */
    ; /* 800A1280 nop  */
    bl_sh(r_sp + 0x14u, r_v0); /* 800A1284 sh $v0, 0x14($sp) */
    r_v0 = bl_lw(r_s0 + 0x8u); /* 800A1288 lw $v0, 0x8($s0) */
    ; /* 800A128C nop  */
    r_v0 = r_v0 + 0x2u; /* 800A1290 addiu $v0, $v0, 0x2 */
    /* 800A1294 j .L800A12A0 */
    bl_sw(r_s0 + 0x8u, r_v0); goto L_800A12A0;
L_800A129C:
    bl_sh(r_sp + 0x14u, 0u); /* 800A129C sh $zero, 0x14($sp) */
L_800A12A0:
    r_v1 = bl_lh(r_sp + 0x10u); /* 800A12A0 lh $v1, 0x10($sp) */
    r_v0 = bl_lh(r_s1 + 0x48u); /* 800A12A4 lh $v0, 0x48($s1) */
    ; /* 800A12A8 nop  */
    BL_MULT(r_v1, r_v0); /* 800A12AC mult $v1, $v0 */
    r_v1 = bl_lh(r_sp + 0x12u); /* 800A12B0 lh $v1, 0x12($sp) */
    r_v0 = lo; /* 800A12B4 mflo $v0 */
    r_v0 = BL_SRA(r_v0, 12); /* 800A12B8 sra $v0, $v0, 12 */
    bl_sh(r_sp + 0x10u, r_v0); /* 800A12BC sh $v0, 0x10($sp) */
    r_v0 = bl_lh(r_s1 + 0x4Au); /* 800A12C0 lh $v0, 0x4A($s1) */
    ; /* 800A12C4 nop  */
    BL_MULT(r_v1, r_v0); /* 800A12C8 mult $v1, $v0 */
    r_v1 = bl_lh(r_sp + 0x14u); /* 800A12CC lh $v1, 0x14($sp) */
    r_v0 = lo; /* 800A12D0 mflo $v0 */
    r_v0 = BL_SRA(r_v0, 12); /* 800A12D4 sra $v0, $v0, 12 */
    bl_sh(r_sp + 0x12u, r_v0); /* 800A12D8 sh $v0, 0x12($sp) */
    r_v0 = bl_lh(r_s1 + 0x4Cu); /* 800A12DC lh $v0, 0x4C($s1) */
    ; /* 800A12E0 nop  */
    BL_MULT(r_v1, r_v0); /* 800A12E4 mult $v1, $v0 */
    r_a1 = r_sp + 0x10u; /* 800A12E8 addiu $a1, $sp, 0x10 */
    r_t4 = bl_lw(r_sp + 0x30u); /* 800A12EC lw $t4, 0x30($sp) */
    r_a2 = r_sp + 0x18u; /* 800A12F0 addiu $a2, $sp, 0x18 */
    r_a0 = r_t4 + 0x2Cu; /* 800A12F4 addiu $a0, $t4, 0x2C */
    r_v0 = lo; /* 800A12F8 mflo $v0 */
    r_v0 = BL_SRA(r_v0, 12); /* 800A12FC sra $v0, $v0, 12 */
    /* 800A1300 jal ApplyMatrix */
    bl_sh(r_sp + 0x14u, r_v0);
    { uint64_t r_ = (uint64_t)(uintptr_t)ApplyMatrix((MATRIX*)(uintptr_t)bg_arg(r_a0), (SVECTOR*)(uintptr_t)bg_arg(r_a1), (VECTOR*)(uintptr_t)bg_arg(r_a2)); r_v0 = (u32)r_; r_v1 = (u32)(r_ >> 32); } /* ApplyMatrix */
    r_v0 = bl_lw(r_sp + 0x18u); /* 800A1308 lw $v0, 0x18($sp) */
    r_t3 = bl_lw(r_sp + 0x40u); /* 800A130C lw $t3, 0x40($sp) */
    ; /* 800A1310 nop  */
    BL_MULT(r_t3, r_v0); /* 800A1314 mult $t3, $v0 */
    r_v1 = bl_lw(r_s1 + 0x58u); /* 800A1318 lw $v1, 0x58($s1) */
    r_v0 = lo; /* 800A131C mflo $v0 */
    r_v0 = BL_SRA(r_v0, 12); /* 800A1320 sra $v0, $v0, 12 */
    r_v0 = r_v0 + r_v1; /* 800A1324 addu $v0, $v0, $v1 */
    bl_sw(r_s1 + 0x58u, r_v0); /* 800A1328 sw $v0, 0x58($s1) */
    r_v0 = bl_lw(r_sp + 0x1Cu); /* 800A132C lw $v0, 0x1C($sp) */
    ; /* 800A1330 nop  */
    BL_MULT(r_t3, r_v0); /* 800A1334 mult $t3, $v0 */
    r_v1 = bl_lw(r_s1 + 0x5Cu); /* 800A1338 lw $v1, 0x5C($s1) */
    r_v0 = lo; /* 800A133C mflo $v0 */
    r_v0 = BL_SRA(r_v0, 12); /* 800A1340 sra $v0, $v0, 12 */
    r_v0 = r_v0 + r_v1; /* 800A1344 addu $v0, $v0, $v1 */
    bl_sw(r_s1 + 0x5Cu, r_v0); /* 800A1348 sw $v0, 0x5C($s1) */
    r_v0 = bl_lw(r_sp + 0x20u); /* 800A134C lw $v0, 0x20($sp) */
    ; /* 800A1350 nop  */
    BL_MULT(r_t3, r_v0); /* 800A1354 mult $t3, $v0 */
    r_v1 = bl_lw(r_s1 + 0x60u); /* 800A1358 lw $v1, 0x60($s1) */
    r_v0 = lo; /* 800A135C mflo $v0 */
    /* 800A1360 j .L800A1628 */
    r_v0 = BL_SRA(r_v0, 12); goto L_800A1628;
L_800A1368:
    r_a0 = bl_lhu(r_s0 + 0x10u); /* 800A1368 lhu $a0, 0x10($s0) */
    r_v0 = bl_lh(r_s0 + 0xAu); /* 800A136C lh $v0, 0xA($s0) */
    r_a0 = r_a0 + 0x1u; /* 800A1370 addiu $a0, $a0, 0x1 */
    r_a0 = r_a0 << 16; /* 800A1374 sll $a0, $a0, 16 */
    r_a0 = BL_SRA(r_a0, 16); /* 800A1378 sra $a0, $a0, 16 */
    BL_MULT(r_v0, r_a0); /* 800A137C mult $v0, $a0 */
    r_v1 = lo; /* 800A1380 mflo $v1 */
    r_v0 = bl_lh(r_s0 + 0x12u); /* 800A1384 lh $v0, 0x12($s0) */
    ; /* 800A1388 nop  */
    BL_DIV(r_v1, r_v0); /* 800A138C div $zero, $v1, $v0 */
    /* 800A1390 bnez $v0, .L800A139C */
    { int c_ = (r_v0 != 0); ; if (c_) goto L_800A139C; }
    bl_fail(FN, "BREAK", 7u); /* 800A1398 break 7 */
L_800A139C:
    r_at = 0u + 0xFFFFFFFFu; /* 800A139C addiu $at, $zero, -0x1 */
    /* 800A13A0 bne $v0, $at, .L800A13B4 */
    { int c_ = (r_v0 != r_at); r_at = 0x80000000u; if (c_) goto L_800A13B4; }
    /* 800A13A8 bne $v1, $at, .L800A13B4 */
    { int c_ = (r_v1 != r_at); ; if (c_) goto L_800A13B4; }
    bl_fail(FN, "BREAK", 6u); /* 800A13B0 break 6 */
L_800A13B4:
    r_v1 = lo; /* 800A13B4 mflo $v1 */
    r_v0 = bl_lh(r_s0 + 0x4u); /* 800A13B8 lh $v0, 0x4($s0) */
    ; /* 800A13BC nop  */
    r_v0 = r_v0 + r_v1; /* 800A13C0 addu $v0, $v0, $v1 */
    bl_sw(r_s1 + 0x58u, r_v0); /* 800A13C4 sw $v0, 0x58($s1) */
    r_v0 = bl_lh(r_s0 + 0xCu); /* 800A13C8 lh $v0, 0xC($s0) */
    ; /* 800A13CC nop  */
    BL_MULT(r_v0, r_a0); /* 800A13D0 mult $v0, $a0 */
    r_v1 = lo; /* 800A13D4 mflo $v1 */
    r_v0 = bl_lh(r_s0 + 0x12u); /* 800A13D8 lh $v0, 0x12($s0) */
    ; /* 800A13DC nop  */
    BL_DIV(r_v1, r_v0); /* 800A13E0 div $zero, $v1, $v0 */
    /* 800A13E4 bnez $v0, .L800A13F0 */
    { int c_ = (r_v0 != 0); ; if (c_) goto L_800A13F0; }
    bl_fail(FN, "BREAK", 7u); /* 800A13EC break 7 */
L_800A13F0:
    r_at = 0u + 0xFFFFFFFFu; /* 800A13F0 addiu $at, $zero, -0x1 */
    /* 800A13F4 bne $v0, $at, .L800A1408 */
    { int c_ = (r_v0 != r_at); r_at = 0x80000000u; if (c_) goto L_800A1408; }
    /* 800A13FC bne $v1, $at, .L800A1408 */
    { int c_ = (r_v1 != r_at); ; if (c_) goto L_800A1408; }
    bl_fail(FN, "BREAK", 6u); /* 800A1404 break 6 */
L_800A1408:
    r_v1 = lo; /* 800A1408 mflo $v1 */
    r_v0 = bl_lh(r_s0 + 0x6u); /* 800A140C lh $v0, 0x6($s0) */
    ; /* 800A1410 nop  */
    r_v0 = r_v0 + r_v1; /* 800A1414 addu $v0, $v0, $v1 */
    bl_sw(r_s1 + 0x5Cu, r_v0); /* 800A1418 sw $v0, 0x5C($s1) */
    r_v0 = bl_lh(r_s0 + 0xEu); /* 800A141C lh $v0, 0xE($s0) */
    ; /* 800A1420 nop  */
    BL_MULT(r_v0, r_a0); /* 800A1424 mult $v0, $a0 */
    r_v1 = lo; /* 800A1428 mflo $v1 */
    r_v0 = bl_lh(r_s0 + 0x12u); /* 800A142C lh $v0, 0x12($s0) */
    ; /* 800A1430 nop  */
    BL_DIV(r_v1, r_v0); /* 800A1434 div $zero, $v1, $v0 */
    /* 800A1438 bnez $v0, .L800A1444 */
    { int c_ = (r_v0 != 0); ; if (c_) goto L_800A1444; }
    bl_fail(FN, "BREAK", 7u); /* 800A1440 break 7 */
L_800A1444:
    r_at = 0u + 0xFFFFFFFFu; /* 800A1444 addiu $at, $zero, -0x1 */
    /* 800A1448 bne $v0, $at, .L800A145C */
    { int c_ = (r_v0 != r_at); r_at = 0x80000000u; if (c_) goto L_800A145C; }
    /* 800A1450 bne $v1, $at, .L800A145C */
    { int c_ = (r_v1 != r_at); ; if (c_) goto L_800A145C; }
    bl_fail(FN, "BREAK", 6u); /* 800A1458 break 6 */
L_800A145C:
    r_v1 = lo; /* 800A145C mflo $v1 */
    r_v0 = bl_lh(r_s0 + 0x8u); /* 800A1460 lh $v0, 0x8($s0) */
    /* 800A1464 j .L800A1628 */
    r_s6 = r_s0 + 0u; goto L_800A1628;
L_800A146C:
    r_a0 = bl_lh(r_s0 + 0xAu); /* 800A146C lh $a0, 0xA($s0) */
    r_v0 = bl_lw(r_s1 + 0x58u); /* 800A1470 lw $v0, 0x58($s1) */
    r_a1 = bl_lh(r_s0 + 0x12u); /* 800A1474 lh $a1, 0x12($s0) */
    r_a0 = r_a0 - r_v0; /* 800A1478 subu $a0, $a0, $v0 */
    BL_DIV(r_a0, r_a1); /* 800A147C div $zero, $a0, $a1 */
    /* 800A1480 bnez $a1, .L800A148C */
    { int c_ = (r_a1 != 0); ; if (c_) goto L_800A148C; }
    bl_fail(FN, "BREAK", 7u); /* 800A1488 break 7 */
L_800A148C:
    r_at = 0u + 0xFFFFFFFFu; /* 800A148C addiu $at, $zero, -0x1 */
    /* 800A1490 bne $a1, $at, .L800A14A4 */
    { int c_ = (r_a1 != r_at); r_at = 0x80000000u; if (c_) goto L_800A14A4; }
    /* 800A1498 bne $a0, $at, .L800A14A4 */
    { int c_ = (r_a0 != r_at); ; if (c_) goto L_800A14A4; }
    bl_fail(FN, "BREAK", 6u); /* 800A14A0 break 6 */
L_800A14A4:
    r_a0 = lo; /* 800A14A4 mflo $a0 */
    r_v1 = bl_lw(r_s1 + 0x5Cu); /* 800A14A8 lw $v1, 0x5C($s1) */
    r_v0 = bl_lh(r_s0 + 0xCu); /* 800A14AC lh $v0, 0xC($s0) */
    ; /* 800A14B0 nop  */
    r_v0 = r_v0 - r_v1; /* 800A14B4 subu $v0, $v0, $v1 */
    BL_DIV(r_v0, r_a1); /* 800A14B8 div $zero, $v0, $a1 */
    /* 800A14BC bnez $a1, .L800A14C8 */
    { int c_ = (r_a1 != 0); ; if (c_) goto L_800A14C8; }
    bl_fail(FN, "BREAK", 7u); /* 800A14C4 break 7 */
L_800A14C8:
    r_at = 0u + 0xFFFFFFFFu; /* 800A14C8 addiu $at, $zero, -0x1 */
    /* 800A14CC bne $a1, $at, .L800A14E0 */
    { int c_ = (r_a1 != r_at); r_at = 0x80000000u; if (c_) goto L_800A14E0; }
    /* 800A14D4 bne $v0, $at, .L800A14E0 */
    { int c_ = (r_v0 != r_at); ; if (c_) goto L_800A14E0; }
    bl_fail(FN, "BREAK", 6u); /* 800A14DC break 6 */
L_800A14E0:
    r_a2 = lo; /* 800A14E0 mflo $a2 */
    r_v0 = bl_lh(r_s0 + 0xEu); /* 800A14E4 lh $v0, 0xE($s0) */
    r_v1 = bl_lw(r_s1 + 0x60u); /* 800A14E8 lw $v1, 0x60($s1) */
    ; /* 800A14EC nop  */
    r_v0 = r_v0 - r_v1; /* 800A14F0 subu $v0, $v0, $v1 */
    BL_DIV(r_v0, r_a1); /* 800A14F4 div $zero, $v0, $a1 */
    /* 800A14F8 bnez $a1, .L800A1504 */
    { int c_ = (r_a1 != 0); ; if (c_) goto L_800A1504; }
    bl_fail(FN, "BREAK", 7u); /* 800A1500 break 7 */
L_800A1504:
    r_at = 0u + 0xFFFFFFFFu; /* 800A1504 addiu $at, $zero, -0x1 */
    /* 800A1508 bne $a1, $at, .L800A151C */
    { int c_ = (r_a1 != r_at); r_at = 0x80000000u; if (c_) goto L_800A151C; }
    /* 800A1510 bne $v0, $at, .L800A151C */
    { int c_ = (r_v0 != r_at); ; if (c_) goto L_800A151C; }
    bl_fail(FN, "BREAK", 6u); /* 800A1518 break 6 */
L_800A151C:
    r_v1 = lo; /* 800A151C mflo $v1 */
    r_s6 = r_s0 + 0u; /* 800A1520 addu $s6, $s0, $zero */
    r_a3 = r_a0 + 0u; /* 800A1524 addu $a3, $a0, $zero */
    r_a0 = r_a0 << 16; /* 800A1528 sll $a0, $a0, 16 */
    r_t0 = r_a2 + 0u; /* 800A152C addu $t0, $a2, $zero */
    /* 800A1530 bnez $a0, .L800A1574 */
    { int c_ = (r_a0 != 0); r_t1 = r_v1 + 0u; if (c_) goto L_800A1574; }
    r_v0 = r_a2 << 16; /* 800A1538 sll $v0, $a2, 16 */
    /* 800A153C bnez $v0, .L800A1578 */
    { int c_ = (r_v0 != 0); r_v0 = r_a3 << 16; if (c_) goto L_800A1578; }
    r_v0 = r_v1 << 16; /* 800A1544 sll $v0, $v1, 16 */
    /* 800A1548 bnez $v0, .L800A1578 */
    { int c_ = (r_v0 != 0); r_v0 = r_a3 << 16; if (c_) goto L_800A1578; }
    r_v0 = bl_lh(r_s0 + 0xAu); /* 800A1550 lh $v0, 0xA($s0) */
    bl_sh(r_s0 + 0x10u, r_a1); /* 800A1554 sh $a1, 0x10($s0) */
    bl_sw(r_s1 + 0x58u, r_v0); /* 800A1558 sw $v0, 0x58($s1) */
    r_v0 = bl_lh(r_s0 + 0xCu); /* 800A155C lh $v0, 0xC($s0) */
    ; /* 800A1560 nop  */
    bl_sw(r_s1 + 0x5Cu, r_v0); /* 800A1564 sw $v0, 0x5C($s1) */
    r_v0 = bl_lh(r_s0 + 0xEu); /* 800A1568 lh $v0, 0xE($s0) */
    /* 800A156C j .L800A1630 */
    bl_sw(r_s1 + 0x60u, r_v0); goto L_800A1630;
L_800A1574:
    r_v0 = r_a3 << 16; /* 800A1574 sll $v0, $a3, 16 */
L_800A1578:
    r_v1 = bl_lw(r_s1 + 0x58u); /* 800A1578 lw $v1, 0x58($s1) */
    r_v0 = BL_SRA(r_v0, 16); /* 800A157C sra $v0, $v0, 16 */
    r_v0 = r_v0 + r_v1; /* 800A1580 addu $v0, $v0, $v1 */
    bl_sw(r_s1 + 0x58u, r_v0); /* 800A1584 sw $v0, 0x58($s1) */
    r_v0 = r_t0 << 16; /* 800A1588 sll $v0, $t0, 16 */
    r_v1 = bl_lw(r_s1 + 0x5Cu); /* 800A158C lw $v1, 0x5C($s1) */
    r_v0 = BL_SRA(r_v0, 16); /* 800A1590 sra $v0, $v0, 16 */
    r_v0 = r_v0 + r_v1; /* 800A1594 addu $v0, $v0, $v1 */
    bl_sw(r_s1 + 0x5Cu, r_v0); /* 800A1598 sw $v0, 0x5C($s1) */
    r_v0 = r_t1 << 16; /* 800A159C sll $v0, $t1, 16 */
    r_v1 = bl_lw(r_s1 + 0x60u); /* 800A15A0 lw $v1, 0x60($s1) */
    r_v0 = BL_SRA(r_v0, 16); /* 800A15A4 sra $v0, $v0, 16 */
    r_v0 = r_v0 + r_v1; /* 800A15A8 addu $v0, $v0, $v1 */
    bl_sw(r_s1 + 0x60u, r_v0); /* 800A15AC sw $v0, 0x60($s1) */
    /* 800A15B0 j .L800A1630 */
    bl_sh(r_s0 + 0x10u, 0u); goto L_800A1630;
L_800A15B8:
    r_v0 = bl_lhu(r_s0 + 0x4u); /* 800A15B8 lhu $v0, 0x4($s0) */
    r_v1 = bl_lhu(r_s0 + 0xAu); /* 800A15BC lhu $v1, 0xA($s0) */
    ; /* 800A15C0 nop  */
    r_v0 = r_v0 + r_v1; /* 800A15C4 addu $v0, $v0, $v1 */
    bl_sh(r_s0 + 0x4u, r_v0); /* 800A15C8 sh $v0, 0x4($s0) */
    r_v0 = r_v0 << 16; /* 800A15CC sll $v0, $v0, 16 */
    r_v1 = bl_lw(r_s1 + 0x58u); /* 800A15D0 lw $v1, 0x58($s1) */
    r_v0 = BL_SRA(r_v0, 16); /* 800A15D4 sra $v0, $v0, 16 */
    r_v0 = r_v0 + r_v1; /* 800A15D8 addu $v0, $v0, $v1 */
    bl_sw(r_s1 + 0x58u, r_v0); /* 800A15DC sw $v0, 0x58($s1) */
    r_v0 = bl_lhu(r_s0 + 0x6u); /* 800A15E0 lhu $v0, 0x6($s0) */
    r_v1 = bl_lhu(r_s0 + 0xCu); /* 800A15E4 lhu $v1, 0xC($s0) */
    ; /* 800A15E8 nop  */
    r_v0 = r_v0 + r_v1; /* 800A15EC addu $v0, $v0, $v1 */
    bl_sh(r_s0 + 0x6u, r_v0); /* 800A15F0 sh $v0, 0x6($s0) */
    r_v0 = r_v0 << 16; /* 800A15F4 sll $v0, $v0, 16 */
    r_v1 = bl_lw(r_s1 + 0x5Cu); /* 800A15F8 lw $v1, 0x5C($s1) */
    r_v0 = BL_SRA(r_v0, 16); /* 800A15FC sra $v0, $v0, 16 */
    r_v0 = r_v0 + r_v1; /* 800A1600 addu $v0, $v0, $v1 */
    bl_sw(r_s1 + 0x5Cu, r_v0); /* 800A1604 sw $v0, 0x5C($s1) */
    r_v0 = bl_lhu(r_s0 + 0x8u); /* 800A1608 lhu $v0, 0x8($s0) */
    r_v1 = bl_lhu(r_s0 + 0xEu); /* 800A160C lhu $v1, 0xE($s0) */
    r_s6 = r_s0 + 0u; /* 800A1610 addu $s6, $s0, $zero */
    r_v0 = r_v0 + r_v1; /* 800A1614 addu $v0, $v0, $v1 */
    bl_sh(r_s0 + 0x8u, r_v0); /* 800A1618 sh $v0, 0x8($s0) */
    r_v0 = r_v0 << 16; /* 800A161C sll $v0, $v0, 16 */
    r_v1 = bl_lw(r_s1 + 0x60u); /* 800A1620 lw $v1, 0x60($s1) */
    r_v0 = BL_SRA(r_v0, 16); /* 800A1624 sra $v0, $v0, 16 */
L_800A1628:
    r_v0 = r_v0 + r_v1; /* 800A1628 addu $v0, $v0, $v1 */
L_800A162C:
    bl_sw(r_s1 + 0x60u, r_v0); /* 800A162C sw $v0, 0x60($s1) */
L_800A1630:
    r_v0 = bl_lhu(r_s0 + 0x10u); /* 800A1630 lhu $v0, 0x10($s0) */
    r_v1 = bl_lh(r_s0 + 0x12u); /* 800A1634 lh $v1, 0x12($s0) */
    r_v0 = r_v0 + 0x1u; /* 800A1638 addiu $v0, $v0, 0x1 */
    bl_sh(r_s0 + 0x10u, r_v0); /* 800A163C sh $v0, 0x10($s0) */
    r_v0 = r_v0 << 16; /* 800A1640 sll $v0, $v0, 16 */
    r_v0 = BL_SRA(r_v0, 16); /* 800A1644 sra $v0, $v0, 16 */
    r_v0 = (u32)((s32)r_v0 < (s32)r_v1); /* 800A1648 slt $v0, $v0, $v1 */
    /* 800A164C bnez $v0, .L800A16EC */
    { int c_ = (r_v0 != 0); ; if (c_) goto L_800A16EC; }
    r_v0 = bl_lbu(r_s0 + 0x1u); /* 800A1654 lbu $v0, 0x1($s0) */
    ; /* 800A1658 nop  */
    /* 800A165C bnez $v0, .L800A1694 */
    { int c_ = (r_v0 != 0); ; if (c_) goto L_800A1694; }
    r_v0 = bl_lbu(r_s0 + 0x3u); /* 800A1664 lbu $v0, 0x3($s0) */
    r_t4 = bl_lw(r_sp + 0x38u); /* 800A1668 lw $t4, 0x38($sp) */
    ; /* 800A166C nop  */
    /* 800A1670 bne $v0, $t4, .L800A167C */
    { int c_ = (r_v0 != r_t4); ; if (c_) goto L_800A167C; }
    r_s5 = r_s5 | 0x2u; /* 800A1678 ori $s5, $s5, 0x2 */
L_800A167C:
    r_s5 = r_s5 | 0x200u; /* 800A167C ori $s5, $s5, 0x200 */
    r_a0 = bl_lw(r_sp + 0x28u); /* 800A1680 lw $a0, 0x28($sp) */
    /* 800A1684 jal func_800A23E8 */
    r_a1 = r_s0 + 0u;
    r_v0 = bl_call_guest(FN, 0x800A23E8u, r_a0, r_a1, r_a2, r_a3, r_sp); /* func_800A23E8 */
    /* 800A168C j .L800A1708 */
    bl_sw(r_s1 + 0x70u, 0u); goto L_800A1708;
L_800A1694:
    r_v0 = bl_lbu(r_s0 + 0x3u); /* 800A1694 lbu $v0, 0x3($s0) */
    r_t3 = bl_lw(r_sp + 0x38u); /* 800A1698 lw $t3, 0x38($sp) */
    ; /* 800A169C nop  */
    /* 800A16A0 bne $v0, $t3, .L800A16AC */
    { int c_ = (r_v0 != r_t3); r_v1 = r_s2 & 0xFu; if (c_) goto L_800A16AC; }
    r_s5 = r_s5 | 0x4u; /* 800A16A8 ori $s5, $s5, 0x4 */
L_800A16AC:
    r_v0 = (u32)(r_v1 < 0x3u); /* 800A16AC sltiu $v0, $v1, 0x3 */
    /* 800A16B0 beqz $v0, .L800A16C8 */
    { int c_ = (r_v0 == 0); r_s5 = r_s5 | 0x400u; if (c_) goto L_800A16C8; }
    r_v0 = bl_lw(r_s0 + 0x4u); /* 800A16B8 lw $v0, 0x4($s0) */
    bl_sh(r_s0 + 0x10u, 0u); /* 800A16BC sh $zero, 0x10($s0) */
    /* 800A16C0 j .L800A1708 */
    bl_sw(r_s0 + 0x8u, r_v0); goto L_800A1708;
L_800A16C8:
    r_v0 = 0u + 0xFFFFFFFFu; /* 800A16C8 addiu $v0, $zero, -0x1 */
    bl_sh(r_s0 + 0x10u, r_v0); /* 800A16CC sh $v0, 0x10($s0) */
    r_v0 = 0u | 0x5u; /* 800A16D0 ori $v0, $zero, 0x5 */
    /* 800A16D4 bne $v1, $v0, .L800A170C */
    { int c_ = (r_v1 != r_v0); r_v0 = 0u | 0x1u; if (c_) goto L_800A170C; }
    bl_sh(r_s6 + 0xAu, 0u); /* 800A16DC sh $zero, 0xA($s6) */
    bl_sh(r_s6 + 0xCu, 0u); /* 800A16E0 sh $zero, 0xC($s6) */
    /* 800A16E4 j .L800A170C */
    bl_sh(r_s6 + 0xEu, 0u); goto L_800A170C;
L_800A16EC:
    r_v0 = bl_lbu(r_s0 + 0x3u); /* 800A16EC lbu $v0, 0x3($s0) */
    r_t4 = bl_lw(r_sp + 0x38u); /* 800A16F0 lw $t4, 0x38($sp) */
    ; /* 800A16F4 nop  */
    /* 800A16F8 bne $v0, $t4, .L800A1704 */
    { int c_ = (r_v0 != r_t4); ; if (c_) goto L_800A1704; }
    r_s5 = r_s5 | 0x1u; /* 800A1700 ori $s5, $s5, 0x1 */
L_800A1704:
    r_s5 = r_s5 | 0x100u; /* 800A1704 ori $s5, $s5, 0x100 */
L_800A1708:
    r_v0 = 0u | 0x1u; /* 800A1708 ori $v0, $zero, 0x1 */
L_800A170C:
    bl_sb(r_s1, r_v0); /* 800A170C sb $v0, 0x0($s1) */
L_800A1710:
    r_a1 = bl_lw(r_s1 + 0x74u); /* 800A1710 lw $a1, 0x74($s1) */
    ; /* 800A1714 nop  */
    /* 800A1718 beqz $a1, .L800A1AF0 */
    { int c_ = (r_a1 == 0); r_s0 = r_a1 + 0u; if (c_) goto L_800A1AF0; }
    r_v0 = bl_lbu(r_s0 + 0x2u); /* 800A1720 lbu $v0, 0x2($s0) */
    ; /* 800A1724 nop  */
    r_t2 = r_v0 & 0xFu; /* 800A1728 andi $t2, $v0, 0xF */
    r_v1 = r_t2 & 0xFFu; /* 800A172C andi $v1, $t2, 0xFF */
    r_v0 = 0u | 0x4u; /* 800A1730 ori $v0, $zero, 0x4 */
    /* 800A1734 beq $v1, $v0, .L800A186C */
    { int c_ = (r_v1 == r_v0); r_v0 = (u32)((s32)r_v1 < 5); if (c_) goto L_800A186C; }
    /* 800A173C beqz $v0, .L800A1754 */
    { int c_ = (r_v0 == 0); r_v0 = 0u | 0x3u; if (c_) goto L_800A1754; }
    /* 800A1744 beq $v1, $v0, .L800A1768 */
    { int c_ = (r_v1 == r_v0); ; if (c_) goto L_800A1768; }
    /* 800A174C j .L800A1A28 */
    ; goto L_800A1A28;
L_800A1754:
    r_v0 = 0u | 0x5u; /* 800A1754 ori $v0, $zero, 0x5 */
    /* 800A1758 beq $v1, $v0, .L800A19BC */
    { int c_ = (r_v1 == r_v0); ; if (c_) goto L_800A19BC; }
    /* 800A1760 j .L800A1A28 */
    ; goto L_800A1A28;
L_800A1768:
    r_a0 = bl_lhu(r_s0 + 0x10u); /* 800A1768 lhu $a0, 0x10($s0) */
    r_v0 = bl_lh(r_s0 + 0xAu); /* 800A176C lh $v0, 0xA($s0) */
    r_a0 = r_a0 + 0x1u; /* 800A1770 addiu $a0, $a0, 0x1 */
    r_a0 = r_a0 << 16; /* 800A1774 sll $a0, $a0, 16 */
    r_a0 = BL_SRA(r_a0, 16); /* 800A1778 sra $a0, $a0, 16 */
    BL_MULT(r_v0, r_a0); /* 800A177C mult $v0, $a0 */
    r_v1 = lo; /* 800A1780 mflo $v1 */
    r_v0 = bl_lh(r_s0 + 0x12u); /* 800A1784 lh $v0, 0x12($s0) */
    ; /* 800A1788 nop  */
    BL_DIV(r_v1, r_v0); /* 800A178C div $zero, $v1, $v0 */
    /* 800A1790 bnez $v0, .L800A179C */
    { int c_ = (r_v0 != 0); ; if (c_) goto L_800A179C; }
    bl_fail(FN, "BREAK", 7u); /* 800A1798 break 7 */
L_800A179C:
    r_at = 0u + 0xFFFFFFFFu; /* 800A179C addiu $at, $zero, -0x1 */
    /* 800A17A0 bne $v0, $at, .L800A17B4 */
    { int c_ = (r_v0 != r_at); r_at = 0x80000000u; if (c_) goto L_800A17B4; }
    /* 800A17A8 bne $v1, $at, .L800A17B4 */
    { int c_ = (r_v1 != r_at); ; if (c_) goto L_800A17B4; }
    bl_fail(FN, "BREAK", 6u); /* 800A17B0 break 6 */
L_800A17B4:
    r_v1 = lo; /* 800A17B4 mflo $v1 */
    r_v0 = bl_lhu(r_s0 + 0x4u); /* 800A17B8 lhu $v0, 0x4($s0) */
    ; /* 800A17BC nop  */
    r_v0 = r_v0 + r_v1; /* 800A17C0 addu $v0, $v0, $v1 */
    bl_sh(r_s1 + 0x48u, r_v0); /* 800A17C4 sh $v0, 0x48($s1) */
    r_v0 = bl_lh(r_s0 + 0xCu); /* 800A17C8 lh $v0, 0xC($s0) */
    ; /* 800A17CC nop  */
    BL_MULT(r_v0, r_a0); /* 800A17D0 mult $v0, $a0 */
    r_v1 = lo; /* 800A17D4 mflo $v1 */
    r_v0 = bl_lh(r_s0 + 0x12u); /* 800A17D8 lh $v0, 0x12($s0) */
    ; /* 800A17DC nop  */
    BL_DIV(r_v1, r_v0); /* 800A17E0 div $zero, $v1, $v0 */
    /* 800A17E4 bnez $v0, .L800A17F0 */
    { int c_ = (r_v0 != 0); ; if (c_) goto L_800A17F0; }
    bl_fail(FN, "BREAK", 7u); /* 800A17EC break 7 */
L_800A17F0:
    r_at = 0u + 0xFFFFFFFFu; /* 800A17F0 addiu $at, $zero, -0x1 */
    /* 800A17F4 bne $v0, $at, .L800A1808 */
    { int c_ = (r_v0 != r_at); r_at = 0x80000000u; if (c_) goto L_800A1808; }
    /* 800A17FC bne $v1, $at, .L800A1808 */
    { int c_ = (r_v1 != r_at); ; if (c_) goto L_800A1808; }
    bl_fail(FN, "BREAK", 6u); /* 800A1804 break 6 */
L_800A1808:
    r_v1 = lo; /* 800A1808 mflo $v1 */
    r_v0 = bl_lhu(r_s0 + 0x6u); /* 800A180C lhu $v0, 0x6($s0) */
    ; /* 800A1810 nop  */
    r_v0 = r_v0 + r_v1; /* 800A1814 addu $v0, $v0, $v1 */
    bl_sh(r_s1 + 0x4Au, r_v0); /* 800A1818 sh $v0, 0x4A($s1) */
    r_v0 = bl_lh(r_s0 + 0xEu); /* 800A181C lh $v0, 0xE($s0) */
    ; /* 800A1820 nop  */
    BL_MULT(r_v0, r_a0); /* 800A1824 mult $v0, $a0 */
    r_v1 = lo; /* 800A1828 mflo $v1 */
    r_v0 = bl_lh(r_s0 + 0x12u); /* 800A182C lh $v0, 0x12($s0) */
    ; /* 800A1830 nop  */
    BL_DIV(r_v1, r_v0); /* 800A1834 div $zero, $v1, $v0 */
    /* 800A1838 bnez $v0, .L800A1844 */
    { int c_ = (r_v0 != 0); ; if (c_) goto L_800A1844; }
    bl_fail(FN, "BREAK", 7u); /* 800A1840 break 7 */
L_800A1844:
    r_at = 0u + 0xFFFFFFFFu; /* 800A1844 addiu $at, $zero, -0x1 */
    /* 800A1848 bne $v0, $at, .L800A185C */
    { int c_ = (r_v0 != r_at); r_at = 0x80000000u; if (c_) goto L_800A185C; }
    /* 800A1850 bne $v1, $at, .L800A185C */
    { int c_ = (r_v1 != r_at); ; if (c_) goto L_800A185C; }
    bl_fail(FN, "BREAK", 6u); /* 800A1858 break 6 */
L_800A185C:
    r_v1 = lo; /* 800A185C mflo $v1 */
    r_v0 = bl_lhu(r_s0 + 0x8u); /* 800A1860 lhu $v0, 0x8($s0) */
    /* 800A1864 j .L800A1A20 */
    r_s6 = r_s0 + 0u; goto L_800A1A20;
L_800A186C:
    r_a0 = bl_lh(r_s0 + 0xAu); /* 800A186C lh $a0, 0xA($s0) */
    r_v0 = bl_lh(r_s0 + 0x4u); /* 800A1870 lh $v0, 0x4($s0) */
    r_a1 = bl_lh(r_s0 + 0x12u); /* 800A1874 lh $a1, 0x12($s0) */
    r_a0 = r_a0 - r_v0; /* 800A1878 subu $a0, $a0, $v0 */
    BL_DIV(r_a0, r_a1); /* 800A187C div $zero, $a0, $a1 */
    /* 800A1880 bnez $a1, .L800A188C */
    { int c_ = (r_a1 != 0); ; if (c_) goto L_800A188C; }
    bl_fail(FN, "BREAK", 7u); /* 800A1888 break 7 */
L_800A188C:
    r_at = 0u + 0xFFFFFFFFu; /* 800A188C addiu $at, $zero, -0x1 */
    /* 800A1890 bne $a1, $at, .L800A18A4 */
    { int c_ = (r_a1 != r_at); r_at = 0x80000000u; if (c_) goto L_800A18A4; }
    /* 800A1898 bne $a0, $at, .L800A18A4 */
    { int c_ = (r_a0 != r_at); ; if (c_) goto L_800A18A4; }
    bl_fail(FN, "BREAK", 6u); /* 800A18A0 break 6 */
L_800A18A4:
    r_a0 = lo; /* 800A18A4 mflo $a0 */
    r_v1 = bl_lh(r_s0 + 0x6u); /* 800A18A8 lh $v1, 0x6($s0) */
    r_v0 = bl_lh(r_s0 + 0xCu); /* 800A18AC lh $v0, 0xC($s0) */
    ; /* 800A18B0 nop  */
    r_v0 = r_v0 - r_v1; /* 800A18B4 subu $v0, $v0, $v1 */
    BL_DIV(r_v0, r_a1); /* 800A18B8 div $zero, $v0, $a1 */
    /* 800A18BC bnez $a1, .L800A18C8 */
    { int c_ = (r_a1 != 0); ; if (c_) goto L_800A18C8; }
    bl_fail(FN, "BREAK", 7u); /* 800A18C4 break 7 */
L_800A18C8:
    r_at = 0u + 0xFFFFFFFFu; /* 800A18C8 addiu $at, $zero, -0x1 */
    /* 800A18CC bne $a1, $at, .L800A18E0 */
    { int c_ = (r_a1 != r_at); r_at = 0x80000000u; if (c_) goto L_800A18E0; }
    /* 800A18D4 bne $v0, $at, .L800A18E0 */
    { int c_ = (r_v0 != r_at); ; if (c_) goto L_800A18E0; }
    bl_fail(FN, "BREAK", 6u); /* 800A18DC break 6 */
L_800A18E0:
    r_a2 = lo; /* 800A18E0 mflo $a2 */
    r_v0 = bl_lh(r_s0 + 0xEu); /* 800A18E4 lh $v0, 0xE($s0) */
    r_v1 = bl_lh(r_s0 + 0x8u); /* 800A18E8 lh $v1, 0x8($s0) */
    ; /* 800A18EC nop  */
    r_v0 = r_v0 - r_v1; /* 800A18F0 subu $v0, $v0, $v1 */
    BL_DIV(r_v0, r_a1); /* 800A18F4 div $zero, $v0, $a1 */
    /* 800A18F8 bnez $a1, .L800A1904 */
    { int c_ = (r_a1 != 0); ; if (c_) goto L_800A1904; }
    bl_fail(FN, "BREAK", 7u); /* 800A1900 break 7 */
L_800A1904:
    r_at = 0u + 0xFFFFFFFFu; /* 800A1904 addiu $at, $zero, -0x1 */
    /* 800A1908 bne $a1, $at, .L800A191C */
    { int c_ = (r_a1 != r_at); r_at = 0x80000000u; if (c_) goto L_800A191C; }
    /* 800A1910 bne $v0, $at, .L800A191C */
    { int c_ = (r_v0 != r_at); ; if (c_) goto L_800A191C; }
    bl_fail(FN, "BREAK", 6u); /* 800A1918 break 6 */
L_800A191C:
    r_v1 = lo; /* 800A191C mflo $v1 */
    r_s6 = r_s0 + 0u; /* 800A1920 addu $s6, $s0, $zero */
    r_a3 = r_a0 + 0u; /* 800A1924 addu $a3, $a0, $zero */
    r_a0 = r_a0 << 16; /* 800A1928 sll $a0, $a0, 16 */
    r_t0 = r_a2 + 0u; /* 800A192C addu $t0, $a2, $zero */
    /* 800A1930 bnez $a0, .L800A1970 */
    { int c_ = (r_a0 != 0); r_t1 = r_v1 + 0u; if (c_) goto L_800A1970; }
    r_v0 = r_a2 << 16; /* 800A1938 sll $v0, $a2, 16 */
    /* 800A193C bnez $v0, .L800A1970 */
    { int c_ = (r_v0 != 0); r_v0 = r_v1 << 16; if (c_) goto L_800A1970; }
    /* 800A1944 bnez $v0, .L800A1970 */
    { int c_ = (r_v0 != 0); ; if (c_) goto L_800A1970; }
    r_v0 = bl_lhu(r_s0 + 0xAu); /* 800A194C lhu $v0, 0xA($s0) */
    bl_sh(r_s0 + 0x10u, r_a1); /* 800A1950 sh $a1, 0x10($s0) */
    bl_sh(r_s1 + 0x48u, r_v0); /* 800A1954 sh $v0, 0x48($s1) */
    r_v0 = bl_lhu(r_s0 + 0xCu); /* 800A1958 lhu $v0, 0xC($s0) */
    ; /* 800A195C nop  */
    bl_sh(r_s1 + 0x4Au, r_v0); /* 800A1960 sh $v0, 0x4A($s1) */
    r_v0 = bl_lhu(r_s0 + 0xEu); /* 800A1964 lhu $v0, 0xE($s0) */
    /* 800A1968 j .L800A1A28 */
    bl_sh(r_s1 + 0x4Cu, r_v0); goto L_800A1A28;
L_800A1970:
    r_v0 = bl_lhu(r_s6 + 0x4u); /* 800A1970 lhu $v0, 0x4($s6) */
    r_v1 = bl_lhu(r_s6 + 0x6u); /* 800A1974 lhu $v1, 0x6($s6) */
    r_v0 = r_a3 + r_v0; /* 800A1978 addu $v0, $a3, $v0 */
    bl_sh(r_s6 + 0x4u, r_v0); /* 800A197C sh $v0, 0x4($s6) */
    r_v0 = bl_lhu(r_s6 + 0x8u); /* 800A1980 lhu $v0, 0x8($s6) */
    r_v1 = r_t0 + r_v1; /* 800A1984 addu $v1, $t0, $v1 */
    bl_sh(r_s6 + 0x6u, r_v1); /* 800A1988 sh $v1, 0x6($s6) */
    r_v1 = bl_lhu(r_s6 + 0x4u); /* 800A198C lhu $v1, 0x4($s6) */
    r_v0 = r_t1 + r_v0; /* 800A1990 addu $v0, $t1, $v0 */
    bl_sh(r_s6 + 0x8u, r_v0); /* 800A1994 sh $v0, 0x8($s6) */
    bl_sh(r_s1 + 0x48u, r_v1); /* 800A1998 sh $v1, 0x48($s1) */
    r_v0 = bl_lhu(r_s6 + 0x6u); /* 800A199C lhu $v0, 0x6($s6) */
    ; /* 800A19A0 nop  */
    bl_sh(r_s1 + 0x4Au, r_v0); /* 800A19A4 sh $v0, 0x4A($s1) */
    r_v0 = bl_lhu(r_s6 + 0x8u); /* 800A19A8 lhu $v0, 0x8($s6) */
    ; /* 800A19AC nop  */
    bl_sh(r_s1 + 0x4Cu, r_v0); /* 800A19B0 sh $v0, 0x4C($s1) */
    /* 800A19B4 j .L800A1A28 */
    bl_sh(r_s0 + 0x10u, 0u); goto L_800A1A28;
L_800A19BC:
    r_v1 = bl_lhu(r_s0 + 0x4u); /* 800A19BC lhu $v1, 0x4($s0) */
    r_v0 = bl_lhu(r_s0 + 0xAu); /* 800A19C0 lhu $v0, 0xA($s0) */
    ; /* 800A19C4 nop  */
    r_v1 = r_v1 + r_v0; /* 800A19C8 addu $v1, $v1, $v0 */
    bl_sh(r_s0 + 0x4u, r_v1); /* 800A19CC sh $v1, 0x4($s0) */
    r_v0 = bl_lhu(r_s1 + 0x48u); /* 800A19D0 lhu $v0, 0x48($s1) */
    ; /* 800A19D4 nop  */
    r_v0 = r_v0 + r_v1; /* 800A19D8 addu $v0, $v0, $v1 */
    bl_sh(r_s1 + 0x48u, r_v0); /* 800A19DC sh $v0, 0x48($s1) */
    r_v1 = bl_lhu(r_s0 + 0x6u); /* 800A19E0 lhu $v1, 0x6($s0) */
    r_v0 = bl_lhu(r_s0 + 0xCu); /* 800A19E4 lhu $v0, 0xC($s0) */
    ; /* 800A19E8 nop  */
    r_v1 = r_v1 + r_v0; /* 800A19EC addu $v1, $v1, $v0 */
    bl_sh(r_s0 + 0x6u, r_v1); /* 800A19F0 sh $v1, 0x6($s0) */
    r_v0 = bl_lhu(r_s1 + 0x4Au); /* 800A19F4 lhu $v0, 0x4A($s1) */
    ; /* 800A19F8 nop  */
    r_v0 = r_v0 + r_v1; /* 800A19FC addu $v0, $v0, $v1 */
    bl_sh(r_s1 + 0x4Au, r_v0); /* 800A1A00 sh $v0, 0x4A($s1) */
    r_v1 = bl_lhu(r_s0 + 0x8u); /* 800A1A04 lhu $v1, 0x8($s0) */
    r_v0 = bl_lhu(r_s0 + 0xEu); /* 800A1A08 lhu $v0, 0xE($s0) */
    ; /* 800A1A0C nop  */
    r_v1 = r_v1 + r_v0; /* 800A1A10 addu $v1, $v1, $v0 */
    bl_sh(r_s0 + 0x8u, r_v1); /* 800A1A14 sh $v1, 0x8($s0) */
    r_v0 = bl_lhu(r_s1 + 0x4Cu); /* 800A1A18 lhu $v0, 0x4C($s1) */
    r_s6 = r_s0 + 0u; /* 800A1A1C addu $s6, $s0, $zero */
L_800A1A20:
    r_v0 = r_v0 + r_v1; /* 800A1A20 addu $v0, $v0, $v1 */
    bl_sh(r_s1 + 0x4Cu, r_v0); /* 800A1A24 sh $v0, 0x4C($s1) */
L_800A1A28:
    r_v0 = bl_lhu(r_s0 + 0x10u); /* 800A1A28 lhu $v0, 0x10($s0) */
    r_v1 = bl_lh(r_s0 + 0x12u); /* 800A1A2C lh $v1, 0x12($s0) */
    r_v0 = r_v0 + 0x1u; /* 800A1A30 addiu $v0, $v0, 0x1 */
    bl_sh(r_s0 + 0x10u, r_v0); /* 800A1A34 sh $v0, 0x10($s0) */
    r_v0 = r_v0 << 16; /* 800A1A38 sll $v0, $v0, 16 */
    r_v0 = BL_SRA(r_v0, 16); /* 800A1A3C sra $v0, $v0, 16 */
    r_v0 = (u32)((s32)r_v0 < (s32)r_v1); /* 800A1A40 slt $v0, $v0, $v1 */
    /* 800A1A44 bnez $v0, .L800A1AC8 */
    { int c_ = (r_v0 != 0); ; if (c_) goto L_800A1AC8; }
    r_v0 = bl_lbu(r_s0 + 0x1u); /* 800A1A4C lbu $v0, 0x1($s0) */
    ; /* 800A1A50 nop  */
    /* 800A1A54 bnez $v0, .L800A1A8C */
    { int c_ = (r_v0 != 0); ; if (c_) goto L_800A1A8C; }
    r_v0 = bl_lbu(r_s0 + 0x3u); /* 800A1A5C lbu $v0, 0x3($s0) */
    r_t3 = bl_lw(r_sp + 0x38u); /* 800A1A60 lw $t3, 0x38($sp) */
    ; /* 800A1A64 nop  */
    /* 800A1A68 bne $v0, $t3, .L800A1A74 */
    { int c_ = (r_v0 != r_t3); ; if (c_) goto L_800A1A74; }
    r_s5 = r_s5 | 0x2u; /* 800A1A70 ori $s5, $s5, 0x2 */
L_800A1A74:
    r_s5 = r_s5 | 0x200u; /* 800A1A74 ori $s5, $s5, 0x200 */
    r_a0 = bl_lw(r_sp + 0x28u); /* 800A1A78 lw $a0, 0x28($sp) */
    /* 800A1A7C jal func_800A23E8 */
    r_a1 = r_s0 + 0u;
    r_v0 = bl_call_guest(FN, 0x800A23E8u, r_a0, r_a1, r_a2, r_a3, r_sp); /* func_800A23E8 */
    /* 800A1A84 j .L800A1AE4 */
    bl_sw(r_s1 + 0x74u, 0u); goto L_800A1AE4;
L_800A1A8C:
    r_v0 = bl_lbu(r_s0 + 0x3u); /* 800A1A8C lbu $v0, 0x3($s0) */
    r_t4 = bl_lw(r_sp + 0x38u); /* 800A1A90 lw $t4, 0x38($sp) */
    ; /* 800A1A94 nop  */
    /* 800A1A98 bne $v0, $t4, .L800A1AA4 */
    { int c_ = (r_v0 != r_t4); r_v1 = r_t2 & 0xFFu; if (c_) goto L_800A1AA4; }
    r_s5 = r_s5 | 0x4u; /* 800A1AA0 ori $s5, $s5, 0x4 */
L_800A1AA4:
    r_v0 = 0u + 0xFFFFFFFFu; /* 800A1AA4 addiu $v0, $zero, -0x1 */
    bl_sh(r_s0 + 0x10u, r_v0); /* 800A1AA8 sh $v0, 0x10($s0) */
    r_v0 = 0u | 0x5u; /* 800A1AAC ori $v0, $zero, 0x5 */
    /* 800A1AB0 bne $v1, $v0, .L800A1AE4 */
    { int c_ = (r_v1 != r_v0); r_s5 = r_s5 | 0x400u; if (c_) goto L_800A1AE4; }
    bl_sh(r_s6 + 0xAu, 0u); /* 800A1AB8 sh $zero, 0xA($s6) */
    bl_sh(r_s6 + 0xCu, 0u); /* 800A1ABC sh $zero, 0xC($s6) */
    /* 800A1AC0 j .L800A1AE4 */
    bl_sh(r_s6 + 0xEu, 0u); goto L_800A1AE4;
L_800A1AC8:
    r_v0 = bl_lbu(r_s0 + 0x3u); /* 800A1AC8 lbu $v0, 0x3($s0) */
    r_t3 = bl_lw(r_sp + 0x38u); /* 800A1ACC lw $t3, 0x38($sp) */
    ; /* 800A1AD0 nop  */
    /* 800A1AD4 bne $v0, $t3, .L800A1AE0 */
    { int c_ = (r_v0 != r_t3); ; if (c_) goto L_800A1AE0; }
    r_s5 = r_s5 | 0x1u; /* 800A1ADC ori $s5, $s5, 0x1 */
L_800A1AE0:
    r_s5 = r_s5 | 0x100u; /* 800A1AE0 ori $s5, $s5, 0x100 */
L_800A1AE4:
    r_v0 = 0u | 0x1u; /* 800A1AE4 ori $v0, $zero, 0x1 */
    bl_sb(r_s1 + 0x1u, r_v0); /* 800A1AE8 sb $v0, 0x1($s1) */
    bl_sb(r_s1, r_v0); /* 800A1AEC sb $v0, 0x0($s1) */
L_800A1AF0:
    r_s1 = r_s1 + 0x7Cu; /* 800A1AF0 addiu $s1, $s1, 0x7C */
    r_t3 = bl_lw(r_sp + 0x30u); /* 800A1AF4 lw $t3, 0x30($sp) */
    r_t4 = bl_lw(r_sp + 0x48u); /* 800A1AF8 lw $t4, 0x48($sp) */
    r_t3 = r_t3 + 0x7Cu; /* 800A1AFC addiu $t3, $t3, 0x7C */
    bl_sw(r_sp + 0x30u, r_t3); /* 800A1B00 sw $t3, 0x30($sp) */
    r_t3 = bl_lw(r_sp + 0x50u); /* 800A1B04 lw $t3, 0x50($sp) */
    r_t4 = r_t4 + 0x1u; /* 800A1B08 addiu $t4, $t4, 0x1 */
    r_v0 = (u32)(r_t4 < r_t3); /* 800A1B0C sltu $v0, $t4, $t3 */
    /* 800A1B10 bnez $v0, .L800A088C */
    { int c_ = (r_v0 != 0); bl_sw(r_sp + 0x48u, r_t4); if (c_) goto L_800A088C; }
L_800A1B18:
    r_v0 = r_s5 + 0u; /* 800A1B18 addu $v0, $s5, $zero */
    r_ra = bl_lw(r_sp + 0xB4u); /* 800A1B1C lw $ra, 0xB4($sp) */
    r_fp = bl_lw(r_sp + 0xB0u); /* 800A1B20 lw $fp, 0xB0($sp) */
    r_s7 = bl_lw(r_sp + 0xACu); /* 800A1B24 lw $s7, 0xAC($sp) */
    r_s6 = bl_lw(r_sp + 0xA8u); /* 800A1B28 lw $s6, 0xA8($sp) */
    r_s5 = bl_lw(r_sp + 0xA4u); /* 800A1B2C lw $s5, 0xA4($sp) */
    r_s4 = bl_lw(r_sp + 0xA0u); /* 800A1B30 lw $s4, 0xA0($sp) */
    r_s3 = bl_lw(r_sp + 0x9Cu); /* 800A1B34 lw $s3, 0x9C($sp) */
    r_s2 = bl_lw(r_sp + 0x98u); /* 800A1B38 lw $s2, 0x98($sp) */
    r_s1 = bl_lw(r_sp + 0x94u); /* 800A1B3C lw $s1, 0x94($sp) */
    r_s0 = bl_lw(r_sp + 0x90u); /* 800A1B40 lw $s0, 0x90($sp) */
    r_sp = r_sp + 0xB8u; /* 800A1B44 addiu $sp, $sp, 0xB8 */
    /* 800A1B48 jr $ra */
    ; goto bl_exit;
bl_exit:
    bl_leave(bl_saved);
    (void)hi; (void)lo; (void)r_at; (void)r_v1; (void)r_k0; (void)r_k1; (void)r_gp; (void)r_ra;
    return r_v0;
}
#endif /* XENO_PC_PORT */
extern void func_800A23E8(u8* a0, u32 p);
/* func_800A216C.s: release up to three slot pointers (+0x70/+0x74/+0x78) of
 * entry idx in the 124-byte array at a1, each gated by its own mask bit, when
 * idx is below the +0xA count. */
void func_800A216C(u8* a0, u8* a1, s32 idx, u32 mask) {
    u8* e = a1;

    if (idx < *(u16*)(e + 0xA)) {
        e += idx * 124;

        if (*(u32*)(e + 0x70) != 0 && (mask & 1)) {
            func_800A23E8(a0, *(u32*)(e + 0x70));
            *(u32*)(e + 0x70) = 0;
        }
        if (*(u32*)(e + 0x74) != 0 && (mask & 2)) {
            func_800A23E8(a0, *(u32*)(e + 0x74));
            *(u32*)(e + 0x74) = 0;
        }
        if (*(u32*)(e + 0x78) != 0 && (mask & 4)) {
            func_800A23E8(a0, *(u32*)(e + 0x78));
            *(u32*)(e + 0x78) = 0;
        }
    }
}
/* func_800A2234.s: the halfword store precedes the HeapChangeCurrentUser call
 * in C; sched sinks it below the arg setups into the jal delay slot. */
extern void HeapChangeCurrentUser(u32 a0, u32 a1);
extern void* HeapAlloc(u32 size, u32 flag);
extern void func_800A22E8(u8* p);
u8* func_800A2234(u8* a0, int a1) {
    void* p;
    int size;
    if (a1 <= 0)
        return 0;
    *(u16*)(a0 + 6) = a1;
    HeapChangeCurrentUser(4, 0);
    size = a1 * 20;
    p = HeapAlloc(size, 0);
    *(void**)a0 = p;
    if (p != 0) {
        func_800A22E8(a0);
        return a0;
    }
    return 0;
}


extern void HeapFree(u32 p);


/* func_800A22A8.s */
void func_800A22A8(u8* p) {
    u32 q = *(u32*)p;

    *(u16*)(p + 4) = 0;
    if (q != 0) {
        HeapFree(q);
    }
    *(u32*)p = 0;
}
