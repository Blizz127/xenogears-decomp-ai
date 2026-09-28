/* Retail run 2 of text_box_render.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in text_box_render.c. */
#define FIELD_TU_SUB 2
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/dialogue/text_box_render_q2", func_8007E16C);
INCLUDE_ASM("asm/field/nonmatchings/dialogue/text_box_render_q2", func_8007E1C0);
INCLUDE_ASM("asm/field/nonmatchings/dialogue/text_box_render_q2", FieldTextBoxInitializePrimitives);
INCLUDE_ASM("asm/field/nonmatchings/dialogue/text_box_render_q2", func_8007F5AC);
INCLUDE_ASM("asm/field/nonmatchings/dialogue/text_box_render_q2", func_8007F6F8);
INCLUDE_ASM("asm/field/nonmatchings/dialogue/text_box_render_q2", func_8007F814);
INCLUDE_ASM("asm/field/nonmatchings/dialogue/text_box_render_q2", func_8007F8DC);
#endif
#include "text_box_render.c"
