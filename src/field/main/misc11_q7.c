/* Retail run 7 of misc11.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in misc11.c. */
#define FIELD_TU_SUB 7
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc11_q7", func_80093014);
#endif
#include "misc11.c"
