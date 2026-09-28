/* Retail run 4 of misc.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in misc.c. */
#define FIELD_TU_SUB 4
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc_q4", func_80087A40);
INCLUDE_ASM("asm/field/nonmatchings/main/misc_q4", func_80087A7C);
INCLUDE_ASM("asm/field/nonmatchings/main/misc_q4", func_80087AB8);
INCLUDE_ASM("asm/field/nonmatchings/main/misc_q4", func_80087B5C);
#endif
#include "misc.c"
