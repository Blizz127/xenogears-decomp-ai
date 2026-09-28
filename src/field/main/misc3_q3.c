/* Retail run 3 of misc3.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in misc3.c. */
#define FIELD_TU_SUB 3
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc3_q3", func_800705DC);
INCLUDE_ASM("asm/field/nonmatchings/main/misc3_q3", func_80070C84);
INCLUDE_ASM("asm/field/nonmatchings/main/misc3_q3", FieldLoad);
#endif
#include "misc3.c"
