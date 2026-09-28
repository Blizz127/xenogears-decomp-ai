/* Retail run 25 of misc.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in misc.c. */
#define FIELD_TU_SUB 25
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc_q25", func_8008D0F4);
INCLUDE_ASM("asm/field/nonmatchings/main/misc_q25", func_8008D180);
INCLUDE_ASM("asm/field/nonmatchings/main/misc_q25", func_8008D230);
INCLUDE_ASM("asm/field/nonmatchings/main/misc_q25", func_8008D26C);
#endif
#include "misc.c"
