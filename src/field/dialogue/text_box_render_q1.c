/* Retail run 1 of text_box_render.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in text_box_render.c. */
#define FIELD_TU_SUB 1
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/dialogue/text_box_render_q1", func_8007DCF8);
INCLUDE_ASM("asm/field/nonmatchings/dialogue/text_box_render_q1", FieldTextBoxInitialize);
#endif
#include "text_box_render.c"
