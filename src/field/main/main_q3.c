/* Retail run 3 of main.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in main.c. */
#define FIELD_TU_SUB 3
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/main_q3", func_80077884);
INCLUDE_ASM("asm/field/nonmatchings/main/main_q3", func_80077AB4);
#endif
#include "main.c"
