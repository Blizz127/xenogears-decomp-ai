/* Retail run 3 of misc2.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in misc2.c. */
#define FIELD_TU_SUB 3
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc2_q3", func_800723E4);
INCLUDE_ASM("asm/field/nonmatchings/main/misc2_q3", func_8007254C);
#endif
#include "misc2.c"
