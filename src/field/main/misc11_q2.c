/* Retail run 2 of misc11.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in misc11.c. */
#define FIELD_TU_SUB 2
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc11_q2", func_80091A08);
INCLUDE_ASM("asm/field/nonmatchings/main/misc11_q2", func_80091A78);
#endif
#include "misc11.c"
