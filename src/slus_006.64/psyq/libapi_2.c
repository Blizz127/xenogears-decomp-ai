#include "common.h"

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libapi_2", setjmp);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libapi_2", longjmp);

void func_8004BEC0(void) {}

__asm__(
        ".globl func_8004BED0\n\t"
        ".ent func_8004BED0\n\t"
        "func_8004BED0:\n\t"
        ".word 0x15007350\n\t"
        ".word 0x0040809c\n\t"
        ".end func_8004BED0");

__asm__(
        ".globl func_8004BED8\n\t"
        ".ent func_8004BED8\n\t"
        "func_8004BED8:\n\t"
        ".word 0x240a00a0\n\t"
        ".word 0x01400008\n\t"
        ".word 0x24090072\n\t"
        ".end func_8004BED8");

__asm__(
        ".globl ReturnFromException\n\t"
        ".ent ReturnFromException\n\t"
        "ReturnFromException:\n\t"
        ".word 0x240a00b0\n\t"
        ".word 0x01400008\n\t"
        ".word 0x24090017\n\t"
        ".end ReturnFromException");

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libapi_2", ResetEntryInt);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libapi_2", HookEntryInt);
