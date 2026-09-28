/* Retail run 1 of misc3.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in misc3.c. */
#define FIELD_TU_SUB 1
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc3_q1", func_8006FDEC);
#endif
#include "misc3.c"
