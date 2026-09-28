/* Retail run 10 of misc8.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in misc8.c. */
#define FIELD_TU_SUB 10
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc8_q10", func_800859DC);
INCLUDE_ASM("asm/field/nonmatchings/main/misc8_q10", func_80085B20);
#endif
#include "misc8.c"
