/* Retail run 6 of misc4.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in misc4.c. */
#define FIELD_TU_SUB 6
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc4_q6", func_8007B6C4);
INCLUDE_ASM("asm/field/nonmatchings/main/misc4_q6", func_8007B814);
INCLUDE_ASM("asm/field/nonmatchings/main/misc4_q6", func_8007BAC0);
INCLUDE_ASM("asm/field/nonmatchings/main/misc4_q6", func_8007BEF4);
#endif
#include "misc4.c"
