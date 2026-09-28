/* Retail run 4 of misc9.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in misc9.c. */
#define FIELD_TU_SUB 4
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc9_q4", func_800ABFDC);
INCLUDE_ASM("asm/field/nonmatchings/main/misc9_q4", func_800AC03C);
INCLUDE_ASM("asm/field/nonmatchings/main/misc9_q4", func_800AC0F0);
#endif
#include "misc9.c"
