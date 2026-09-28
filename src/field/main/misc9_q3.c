/* Retail run 3 of misc9.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in misc9.c. */
#define FIELD_TU_SUB 3
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc9_q3", func_800AB748);
INCLUDE_ASM("asm/field/nonmatchings/main/misc9_q3", func_800AB808);
INCLUDE_ASM("asm/field/nonmatchings/main/misc9_q3", func_800ABA98);
#endif
#include "misc9.c"
