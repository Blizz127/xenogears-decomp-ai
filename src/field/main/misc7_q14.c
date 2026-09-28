/* Retail run 14 of misc7.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in misc7.c. */
#define FIELD_TU_SUB 14
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc7_q14", func_8009B708);
INCLUDE_ASM("asm/field/nonmatchings/main/misc7_q14", func_8009B7A8);
#endif
#include "misc7.c"
