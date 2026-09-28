/* Retail run 5 of misc4.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in misc4.c. */
#define FIELD_TU_SUB 5
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc4_q5", func_8007AE78);
INCLUDE_ASM("asm/field/nonmatchings/main/misc4_q5", func_8007AF74);
INCLUDE_ASM("asm/field/nonmatchings/main/misc4_q5", func_8007B07C);
INCLUDE_ASM("asm/field/nonmatchings/main/misc4_q5", func_8007B1C4);
INCLUDE_ASM("asm/field/nonmatchings/main/misc4_q5", func_8007B478);
INCLUDE_ASM("asm/field/nonmatchings/main/misc4_q5", func_8007B614);
#endif
#include "misc4.c"
