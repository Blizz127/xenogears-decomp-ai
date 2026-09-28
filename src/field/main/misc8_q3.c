/* Retail run 3 of misc8.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in misc8.c. */
#define FIELD_TU_SUB 3
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc8_q3", func_80080A74);
INCLUDE_ASM("asm/field/nonmatchings/main/misc8_q3", func_80080F44);
INCLUDE_ASM("asm/field/nonmatchings/main/misc8_q3", func_8008110C);
INCLUDE_ASM("asm/field/nonmatchings/main/misc8_q3", func_800815F0);
INCLUDE_ASM("asm/field/nonmatchings/main/misc8_q3", func_80081C54);
#endif
#include "misc8.c"
