/* Retail run 5 of misc8.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in misc8.c. */
#define FIELD_TU_SUB 5
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc8_q5", func_800825AC);
INCLUDE_ASM("asm/field/nonmatchings/main/misc8_q5", func_80082620);
INCLUDE_ASM("asm/field/nonmatchings/main/misc8_q5", func_80082BB8);
#endif
#include "misc8.c"
