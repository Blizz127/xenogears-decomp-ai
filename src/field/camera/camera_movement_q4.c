/* Retail run 4 of camera_movement.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in camera_movement.c. */
#define FIELD_TU_SUB 4
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/camera/camera_movement_q4", FieldScriptStartCameraMovement);
#endif
#include "camera_movement.c"
