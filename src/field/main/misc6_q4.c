/* Retail run 4 of misc6.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in misc6.c. */
#define FIELD_TU_SUB 4
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc6_q4", func_8009FA54);
INCLUDE_ASM("asm/field/nonmatchings/main/misc6_q4", func_8009FB98);
#endif
#include "misc6.c"
