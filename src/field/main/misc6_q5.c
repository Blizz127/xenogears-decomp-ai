/* Retail run 5 of misc6.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in misc6.c. */
#define FIELD_TU_SUB 5
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc6_q5", func_800A0228);
INCLUDE_ASM("asm/field/nonmatchings/main/misc6_q5", func_800A0524);
INCLUDE_ASM("asm/field/nonmatchings/main/misc6_q5", func_800A06E8);
INCLUDE_ASM("asm/field/nonmatchings/main/misc6_q5", func_800A08B8);
#endif
#include "misc6.c"
