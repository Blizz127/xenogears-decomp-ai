/* Retail run 12 of misc.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in misc.c. */
#define FIELD_TU_SUB 12
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc_q12", func_80089DCC);
INCLUDE_ASM("asm/field/nonmatchings/main/misc_q12", func_80089F18);
INCLUDE_ASM("asm/field/nonmatchings/main/misc_q12", func_80089F54);
#endif
#include "misc.c"
