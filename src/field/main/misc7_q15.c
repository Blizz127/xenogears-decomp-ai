/* Retail run 15 of misc7.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in misc7.c. */
#define FIELD_TU_SUB 15
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc7_q15", func_8009B8E4);
INCLUDE_ASM("asm/field/nonmatchings/main/misc7_q15", func_8009B9A0);
#endif
#include "misc7.c"
