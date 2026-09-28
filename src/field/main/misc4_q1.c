/* Retail run 1 of misc4.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in misc4.c. */
#define FIELD_TU_SUB 1
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc4_q1", func_80078B5C);
INCLUDE_ASM("asm/field/nonmatchings/main/misc4_q1", func_80078BC8);
#endif
#include "misc4.c"
