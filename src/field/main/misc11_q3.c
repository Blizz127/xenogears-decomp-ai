/* Retail run 3 of misc11.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in misc11.c. */
#define FIELD_TU_SUB 3
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc11_q3", func_80091ADC);
INCLUDE_ASM("asm/field/nonmatchings/main/misc11_q3", func_80091BBC);
INCLUDE_ASM("asm/field/nonmatchings/main/misc11_q3", func_80091E00);
INCLUDE_ASM("asm/field/nonmatchings/main/misc11_q3", func_80091E98);
#endif
#include "misc11.c"
