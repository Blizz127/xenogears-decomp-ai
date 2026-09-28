/* Retail run 16 of misc7.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in misc7.c. */
#define FIELD_TU_SUB 16
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc7_q16", func_8009BA7C);
#endif
#include "misc7.c"
