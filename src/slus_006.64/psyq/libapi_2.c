#include "common.h"

__asm__(
        ".globl setjmp\n\t"
        ".ent setjmp\n\t"
        "setjmp:\n\t"
        ".word 0xac9f0000, 0xac9c002c, 0xac9d0004, 0xac9e0008\n\t"
        ".word 0xac90000c, 0xac910010, 0xac920014, 0xac930018\n\t"
        ".word 0xac94001c, 0xac950020, 0xac960024, 0xac970028\n\t"
        ".word 0x00001021, 0x03e00008, 0x00000000\n\t"
        ".end setjmp");

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

__asm__(
        ".globl ResetEntryInt\n\t"
        ".ent ResetEntryInt\n\t"
        "ResetEntryInt:\n\t"
        ".word 0x240a00b0\n\t"
        ".word 0x01400008\n\t"
        ".word 0x24090018\n\t"
        ".end ResetEntryInt");

__asm__(
        ".globl HookEntryInt\n\t"
        ".ent HookEntryInt\n\t"
        "HookEntryInt:\n\t"
        ".word 0x240a00b0\n\t"
        ".word 0x01400008\n\t"
        ".word 0x24090019\n\t"
        ".end HookEntryInt");
