/* Retail run 2 of misc4.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in misc4.c. */
#define FIELD_TU_SUB 2
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc4_q2", func_80078D44);
INCLUDE_ASM("asm/field/nonmatchings/main/misc4_q2", func_80079288);
#endif
#include "misc4.c"
