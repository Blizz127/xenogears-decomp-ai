/* Retail run 29 of misc.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in misc.c. */
#define FIELD_TU_SUB 29
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc_q29", FieldScriptWriteActorFlags1);
INCLUDE_ASM("asm/field/nonmatchings/main/misc_q29", FieldScriptWriteActorFlags2);
INCLUDE_ASM("asm/field/nonmatchings/main/misc_q29", FieldScriptWriteActorFlags3);
INCLUDE_ASM("asm/field/nonmatchings/main/misc_q29", FieldScriptWriteActorFlags4);
#endif
#include "misc.c"
