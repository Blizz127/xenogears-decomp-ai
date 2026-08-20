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

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libapi_2", func_8004BED8);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libapi_2", ReturnFromException);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libapi_2", ResetEntryInt);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libapi_2", HookEntryInt);
