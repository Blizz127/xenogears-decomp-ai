/* Retail run 8 of misc.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in misc.c. */
#define FIELD_TU_SUB 8
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc_q8", func_8008800C);
INCLUDE_ASM("asm/field/nonmatchings/main/misc_q8", func_80088198);
INCLUDE_ASM("asm/field/nonmatchings/main/misc_q8", func_800881E8);
#endif
#include "misc.c"
