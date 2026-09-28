/* Retail run 8 of misc4.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in misc4.c. */
#define FIELD_TU_SUB 8
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc4_q8", func_8007CD80);
INCLUDE_ASM("asm/field/nonmatchings/main/misc4_q8", func_8007D3D4);
#endif
#include "misc4.c"
