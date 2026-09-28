/* Retail run 6 of misc11.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in misc11.c. */
#define FIELD_TU_SUB 6
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc11_q6", func_800926C8);
INCLUDE_ASM("asm/field/nonmatchings/main/misc11_q6", func_80092768);
INCLUDE_ASM("asm/field/nonmatchings/main/misc11_q6", func_80092808);
INCLUDE_ASM("asm/field/nonmatchings/main/misc11_q6", func_80092894);
INCLUDE_ASM("asm/field/nonmatchings/main/misc11_q6", func_80092C20);
#endif
#include "misc11.c"
