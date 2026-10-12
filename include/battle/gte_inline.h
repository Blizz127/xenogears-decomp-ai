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

#endif
