/* Retail run 7 of misc8.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in misc8.c. */
#define FIELD_TU_SUB 7
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc8_q7", func_8008399C);
INCLUDE_ASM("asm/field/nonmatchings/main/misc8_q7", func_80084158);
INCLUDE_ASM("asm/field/nonmatchings/main/misc8_q7", func_8008492C);
INCLUDE_ASM("asm/field/nonmatchings/main/misc8_q7", func_80084A40);
#endif
#include "misc8.c"
