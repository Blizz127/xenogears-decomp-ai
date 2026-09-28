/* Retail run 2 of misc.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in misc.c. */
#define FIELD_TU_SUB 2
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc_q2", func_80087148);
INCLUDE_ASM("asm/field/nonmatchings/main/misc_q2", func_800871B0);
#endif
#include "misc.c"
