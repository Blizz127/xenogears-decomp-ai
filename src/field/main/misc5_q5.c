/* Retail run 5 of misc5.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in misc5.c. */
#define FIELD_TU_SUB 5
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc5_q5", func_800A73E8);
INCLUDE_ASM("asm/field/nonmatchings/main/misc5_q5", func_800A74F8);
#endif
#include "misc5.c"
