/* Retail run 8 of misc6.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in misc6.c. */
#define FIELD_TU_SUB 8
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc6_q8", func_800A0EE8);
INCLUDE_ASM("asm/field/nonmatchings/main/misc6_q8", func_800A0FD8);
#endif
#include "misc6.c"
