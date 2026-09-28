/* Retail run 35 of misc.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in misc.c. */
#define FIELD_TU_SUB 35
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc_q35", func_8008F5E4);
#endif
#include "misc.c"
