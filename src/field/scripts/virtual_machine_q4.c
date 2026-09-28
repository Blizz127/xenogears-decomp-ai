/* Retail run 4 of virtual_machine.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in virtual_machine.c. */
#define FIELD_TU_SUB 4
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/scripts/virtual_machine_q4", func_800A28D4);
#endif
#include "virtual_machine.c"
