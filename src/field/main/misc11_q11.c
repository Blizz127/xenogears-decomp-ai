/* Retail run 11 of misc11.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in misc11.c. */
#define FIELD_TU_SUB 11
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc11_q11", func_80095284);
#endif
#include "misc11.c"
