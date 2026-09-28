/* Retail run 2 of misc5.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in misc5.c. */
#define FIELD_TU_SUB 2
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc5_q2", func_800A5924);
INCLUDE_ASM("asm/field/nonmatchings/main/misc5_q2", func_800A5C40);
INCLUDE_ASM("asm/field/nonmatchings/main/misc5_q2", FieldZoomFadeEffectUpdate);
INCLUDE_ASM("asm/field/nonmatchings/main/misc5_q2", FieldZoomFadeEffectInitialize);
#endif
#include "misc5.c"
