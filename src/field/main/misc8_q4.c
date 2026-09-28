/* Retail run 4 of misc8.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in misc8.c. */
#define FIELD_TU_SUB 4
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc8_q4", func_80081F80);
INCLUDE_ASM("asm/field/nonmatchings/main/misc8_q4", func_800821F4);
INCLUDE_ASM("asm/field/nonmatchings/main/misc8_q4", func_8008237C);
#endif
#include "misc8.c"
