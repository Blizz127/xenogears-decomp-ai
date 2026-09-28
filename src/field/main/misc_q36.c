/* Retail run 36 of misc.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in misc.c. */
#define FIELD_TU_SUB 36
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc_q36", func_8008F7B8);
INCLUDE_ASM("asm/field/nonmatchings/main/misc_q36", func_8008F90C);
#endif
#include "misc.c"
