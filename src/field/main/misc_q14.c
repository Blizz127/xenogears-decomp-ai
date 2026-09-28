/* Retail run 14 of misc.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in misc.c. */
#define FIELD_TU_SUB 14
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc_q14", func_8008A2E8);
#endif
#include "misc.c"
