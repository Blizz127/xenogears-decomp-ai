/* Retail run 1 of misc9.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in misc9.c. */
#define FIELD_TU_SUB 1
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc9_q1", func_800AA9DC);
INCLUDE_ASM("asm/field/nonmatchings/main/misc9_q1", func_800AAA74);
#endif
#include "misc9.c"
