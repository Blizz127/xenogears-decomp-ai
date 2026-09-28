/* Retail run 3 of camera_movement.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in camera_movement.c. */
#define FIELD_TU_SUB 3
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/camera/camera_movement_q3", func_8008FC4C);
#endif
#include "camera_movement.c"
