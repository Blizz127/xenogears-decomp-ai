/* Retail run 11 of misc8.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in misc8.c. */
#define FIELD_TU_SUB 11
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc8_q11", func_80086078);
INCLUDE_ASM("asm/field/nonmatchings/main/misc8_q11", func_800860F0);
INCLUDE_ASM("asm/field/nonmatchings/main/misc8_q11", FieldActorWorldToScreenPosition);
INCLUDE_ASM("asm/field/nonmatchings/main/misc8_q11", func_800862CC);
INCLUDE_ASM("asm/field/nonmatchings/main/misc8_q11", func_800863E8);
INCLUDE_ASM("asm/field/nonmatchings/main/misc8_q11", func_80086470);
INCLUDE_ASM("asm/field/nonmatchings/main/misc8_q11", func_800864B4);
INCLUDE_ASM("asm/field/nonmatchings/main/misc8_q11", func_800864F0);
INCLUDE_ASM("asm/field/nonmatchings/main/misc8_q11", func_80086590);
#endif
#include "misc8.c"
