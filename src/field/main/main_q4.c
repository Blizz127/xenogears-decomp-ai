/* Retail run 4 of main.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in main.c. */
#define FIELD_TU_SUB 4
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/main_q4", func_80077E10);
INCLUDE_ASM("asm/field/nonmatchings/main/main_q4", FieldMain);
#endif
#include "main.c"
