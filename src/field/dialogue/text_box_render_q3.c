/* Retail run 3 of text_box_render.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in text_box_render.c. */
#define FIELD_TU_SUB 3
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/dialogue/text_box_render_q3", func_8008004C);
#endif
#include "text_box_render.c"
