/* Retail run 4 of misc4.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in misc4.c. */
#define FIELD_TU_SUB 4
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc4_q4", func_8007A5C4);
#endif
#include "misc4.c"
