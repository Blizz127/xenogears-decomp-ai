/* Retail run 13 of misc.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in misc.c. */
#define FIELD_TU_SUB 13
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc_q13", func_80089FD0);
INCLUDE_ASM("asm/field/nonmatchings/main/misc_q13", func_8008A08C);
INCLUDE_ASM("asm/field/nonmatchings/main/misc_q13", func_8008A148);
#endif
#include "misc.c"
