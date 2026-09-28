/* Retail run 1 of misc10.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in misc10.c. */
#define FIELD_TU_SUB 1
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc10_q1", func_80096214);
#endif
#include "misc10.c"
