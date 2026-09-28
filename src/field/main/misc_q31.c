/* Retail run 31 of misc.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in misc.c. */
#define FIELD_TU_SUB 31
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc_q31", func_8008EA58);
INCLUDE_ASM("asm/field/nonmatchings/main/misc_q31", func_8008EC30);
INCLUDE_ASM("asm/field/nonmatchings/main/misc_q31", func_8008EE14);
#endif
#include "misc.c"
