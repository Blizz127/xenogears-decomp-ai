/* Retail run 7 of misc2.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in misc2.c. */
#define FIELD_TU_SUB 7
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc2_q7", func_80075484);
INCLUDE_ASM("asm/field/nonmatchings/main/misc2_q7", func_8007554C);
INCLUDE_ASM("asm/field/nonmatchings/main/misc2_q7", func_80075910);
#endif
#include "misc2.c"
