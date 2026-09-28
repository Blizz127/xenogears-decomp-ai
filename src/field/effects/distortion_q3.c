/* Retail run 3 of distortion.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in distortion.c. */
#define FIELD_TU_SUB 3
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/effects/distortion_q3", FieldDistortionDraw);
#endif
#include "distortion.c"
