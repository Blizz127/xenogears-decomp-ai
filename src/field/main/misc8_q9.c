/* Retail run 9 of misc8.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in misc8.c. */
#define FIELD_TU_SUB 9
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc8_q9", func_80085788);
#endif
#include "misc8.c"
