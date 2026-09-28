/* Retail run 2 of distortion.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in distortion.c. */
#define FIELD_TU_SUB 2
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/effects/distortion_q2", FieldDistortionInitialize);
#endif
#include "distortion.c"
