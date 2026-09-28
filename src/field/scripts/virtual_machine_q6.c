/* Retail run 6 of virtual_machine.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in virtual_machine.c. */
#define FIELD_TU_SUB 6
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/scripts/virtual_machine_q6", FieldScriptWritePartyMemberIDs);
INCLUDE_ASM("asm/field/nonmatchings/scripts/virtual_machine_q6", func_800A30FC);
INCLUDE_ASM("asm/field/nonmatchings/scripts/virtual_machine_q6", func_800A31E8);
INCLUDE_ASM("asm/field/nonmatchings/scripts/virtual_machine_q6", func_800A3474);
INCLUDE_ASM("asm/field/nonmatchings/scripts/virtual_machine_q6", func_800A3C8C);
INCLUDE_ASM("asm/field/nonmatchings/scripts/virtual_machine_q6", func_800A3F4C);
#endif
#include "virtual_machine.c"
