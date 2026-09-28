/* Retail run 6 of misc2.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in misc2.c. */
#define FIELD_TU_SUB 6
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc2_q6", func_80074108);
INCLUDE_ASM("asm/field/nonmatchings/main/misc2_q6", func_8007469C);
INCLUDE_ASM("asm/field/nonmatchings/main/misc2_q6", FieldPollControllers);
INCLUDE_ASM("asm/field/nonmatchings/main/misc2_q6", func_800748E8);
INCLUDE_ASM("asm/field/nonmatchings/main/misc2_q6", func_8007520C);
INCLUDE_ASM("asm/field/nonmatchings/main/misc2_q6", func_800752C8);
#endif
#include "misc2.c"
