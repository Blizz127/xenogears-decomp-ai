/* Retail run 11 of misc7.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in misc7.c. */
#define FIELD_TU_SUB 11
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc7_q11", func_8009AE3C);
INCLUDE_ASM("asm/field/nonmatchings/main/misc7_q11", func_8009AEE0);
INCLUDE_ASM("asm/field/nonmatchings/main/misc7_q11", func_8009B15C);
#endif
#include "misc7.c"
