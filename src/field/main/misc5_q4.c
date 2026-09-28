/* Retail run 4 of misc5.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in misc5.c. */
#define FIELD_TU_SUB 4
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc5_q4", func_800A708C);
INCLUDE_ASM("asm/field/nonmatchings/main/misc5_q4", func_800A7120);
INCLUDE_ASM("asm/field/nonmatchings/main/misc5_q4", func_800A7218);
INCLUDE_ASM("asm/field/nonmatchings/main/misc5_q4", func_800A732C);
#endif
#include "misc5.c"
