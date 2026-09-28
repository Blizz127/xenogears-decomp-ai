/* Retail run 21 of misc.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in misc.c. */
#define FIELD_TU_SUB 21
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc_q21", func_8008B978);
INCLUDE_ASM("asm/field/nonmatchings/main/misc_q21", func_8008BC80);
INCLUDE_ASM("asm/field/nonmatchings/main/misc_q21", func_8008BDD8);
INCLUDE_ASM("asm/field/nonmatchings/main/misc_q21", func_8008BF38);
INCLUDE_ASM("asm/field/nonmatchings/main/misc_q21", func_8008C180);
INCLUDE_ASM("asm/field/nonmatchings/main/misc_q21", func_8008C334);
#endif
#include "misc.c"
