/* Retail run 5 of misc9.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in misc9.c. */
#define FIELD_TU_SUB 5
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc9_q5", func_800AC3AC);
INCLUDE_ASM("asm/field/nonmatchings/main/misc9_q5", func_800AC99C);
INCLUDE_ASM("asm/field/nonmatchings/main/misc9_q5", func_800ACB90);
#endif
#include "misc9.c"
