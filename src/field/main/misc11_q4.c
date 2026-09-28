/* Retail run 4 of misc11.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in misc11.c. */
#define FIELD_TU_SUB 4
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc11_q4", func_80092044);
INCLUDE_ASM("asm/field/nonmatchings/main/misc11_q4", func_800920D8);
INCLUDE_ASM("asm/field/nonmatchings/main/misc11_q4", func_80092148);
INCLUDE_ASM("asm/field/nonmatchings/main/misc11_q4", func_800921E8);
#endif
#include "misc11.c"
