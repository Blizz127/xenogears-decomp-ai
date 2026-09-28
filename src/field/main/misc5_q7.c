/* Retail run 7 of misc5.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in misc5.c. */
#define FIELD_TU_SUB 7
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc5_q7", func_800A8408);
INCLUDE_ASM("asm/field/nonmatchings/main/misc5_q7", func_800A84C0);
INCLUDE_ASM("asm/field/nonmatchings/main/misc5_q7", func_800A8BA4);
INCLUDE_ASM("asm/field/nonmatchings/main/misc5_q7", FieldInitializeParticlePrimitive);
INCLUDE_ASM("asm/field/nonmatchings/main/misc5_q7", func_800A90B4);
#endif
#include "misc5.c"
