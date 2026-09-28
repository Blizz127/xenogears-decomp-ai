/* Retail run 5 of virtual_machine.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in virtual_machine.c. */
#define FIELD_TU_SUB 5
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/scripts/virtual_machine_q5", FieldScriptMemoryWriteU16);
#endif
#include "virtual_machine.c"
