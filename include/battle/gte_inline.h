/* Retail battle code inlines the Psy-Q GTE macros (inline_c.h style: two
 * hazard nops before each GTE command).  psyq/inline_c.h spells the commands
 * as DMPSX placeholders, so the matching build writes the real cop2 words
 * here instead; they are self-contained (no macro.inc), which keeps the
 * SKIP_ASM report build assembling too.  Retail-only: the port runs these
 * paths through PsyCross. */
#ifndef BATTLE_GTE_INLINE_H
#define BATTLE_GTE_INLINE_H

/* V0 = *(SVECTOR*)p */
#define BATTLE_GTE_LDV0(p) __asm__ volatile("lwc2 $0, 0(%0)\n\tlwc2 $1, 4(%0)" : : "r"(p))
/* MVMVA sf=1 RT*V0 + TR (gte_rtv0tr) */
#define BATTLE_GTE_RTV0TR() __asm__ volatile("nop\n\tnop\n\t.word 0x4A480012")
/* *(VECTOR*)p = MAC1..MAC3 (gte_stlvnl) */
#define BATTLE_GTE_STLVNL(p) \
    __asm__ volatile("swc2 $25, 0(%0)\n\tswc2 $26, 4(%0)\n\tswc2 $27, 8(%0)" : : "r"(p) : "memory")

/* IR0 = v (gte_lddp) */
#define BATTLE_GTE_LDDP(v) __asm__ volatile("mtc2 %0, $8" : : "r"(v))
/* IR1..IR3 = *(VECTOR*)p (gte_ldlvl) */
#define BATTLE_GTE_LDLVL(p) __asm__ volatile("lwc2 $9, 0(%0)\n\tlwc2 $10, 4(%0)\n\tlwc2 $11, 8(%0)" : : "r"(p))
/* GPF sf=0: IR = IR * IR0 (gte_gpf0) */
#define BATTLE_GTE_GPF0() __asm__ volatile("nop\n\tnop\n\t.word 0x4B90003D")
/* GPF sf=1: IR = IR * IR0 >> 12 (gte_gpf12) */
#define BATTLE_GTE_GPF12() __asm__ volatile("nop\n\tnop\n\t.word 0x4B98003D")
/* *(VECTOR*)p = IR1..IR3 (gte_stlvl) */
#define BATTLE_GTE_STLVL(p) \
    __asm__ volatile("swc2 $9, 0(%0)\n\tswc2 $10, 4(%0)\n\tswc2 $11, 8(%0)" : : "r"(p) : "memory")

/* V0, V1, V2 = *(SVECTOR*)a, b, c (gte_ldv3) */
#define BATTLE_GTE_LDV3(a, b, c)                                                                    \
    __asm__ volatile("lwc2 $0, 0(%0)\n\tlwc2 $1, 4(%0)\n\tlwc2 $2, 0(%1)\n\tlwc2 $3, 4(%1)\n\t" \
                     "lwc2 $4, 0(%2)\n\tlwc2 $5, 4(%2)"                                              \
                     :                                                                              \
                     : "r"(a), "r"(b), "r"(c))
/* RTPS: perspective-transform V0 (gte_rtps) */
#define BATTLE_GTE_RTPS() __asm__ volatile("nop\n\tnop\n\t.word 0x4A180001")
/* RTPT: perspective-transform V0..V2 (gte_rtpt) */
#define BATTLE_GTE_RTPT() __asm__ volatile("nop\n\tnop\n\t.word 0x4A280030")
/* *a, *b, *c = SXY0, SXY1, SXY2 (gte_stsxy3) */
#define BATTLE_GTE_STSXY3(a, b, c) \
    __asm__ volatile("swc2 $12, 0(%0)\n\tswc2 $13, 0(%1)\n\tswc2 $14, 0(%2)" : : "r"(a), "r"(b), "r"(c) : "memory")
/* *p = SXY2 (gte_stsxy) */
#define BATTLE_GTE_STSXY(p) __asm__ volatile("swc2 $14, 0(%0)" : : "r"(p) : "memory")
/* *(s32*)p = SZ3 >> 2 (gte_stszotz; clobbers $12) */
#define BATTLE_GTE_STSZOTZ(p) \
    __asm__ volatile("mfc2 $12, $19\n\tnop\n\tsra $12, $12, 2\n\tsw $12, 0(%0)" : : "r"(p) : "$12", "memory")

#endif
