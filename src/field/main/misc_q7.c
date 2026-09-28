/* Retail run 7 of misc.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in misc.c. */
#define FIELD_TU_SUB 7
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc_q7", func_80087E5C);
INCLUDE_ASM("asm/field/nonmatchings/main/misc_q7", func_80087E98);
#endif
#include "misc.c"
