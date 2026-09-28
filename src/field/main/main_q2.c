/* Retail run 2 of main.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in main.c. */
#define FIELD_TU_SUB 2
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/main_q2", FieldLoadUITextures);
#endif
#include "main.c"
