/* Retail run 4 of misc2.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in misc2.c. */
#define FIELD_TU_SUB 4
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc2_q4", func_80072A38);
INCLUDE_ASM("asm/field/nonmatchings/main/misc2_q4", func_80072D74);
INCLUDE_ASM("asm/field/nonmatchings/main/misc2_q4", func_80073230);
#endif
#include "misc2.c"
