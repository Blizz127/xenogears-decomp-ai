#include "common.h"

__asm__(
        ".globl PCopen\n\t"
        ".ent PCopen\n\t"
        "PCopen:\n\t"
        ".word 0x00a03021\n\t"
        ".word 0x00802821\n\t"
        ".word 0x000040cd\n\t"
        ".word 0x10400002\n\t"
        ".word 0x00601021\n\t"
        ".word 0x2402ffff\n\t"
        ".word 0x03e00008\n\t"
        ".word 0x00000000\n\t"
        ".end PCopen");

__asm__(
        ".globl PCclose\n\t"
        ".ent PCclose\n\t"
        "PCclose:\n\t"
        ".word 0x00802821\n\t"
        ".word 0x0000410d\n\t"
        ".word 0x03e00008\n\t"
        ".word 0x00000000\n\t"
        ".end PCclose");

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libsn", PClseek);

__asm__(
        ".globl PCcreate\n\t"
        ".ent PCcreate\n\t"
        "PCcreate:\n\t"
        ".word 0x00802821\n\t"
        ".word 0x00003021\n\t"
        ".word 0x0000408d\n\t"
        ".word 0x10400002\n\t"
        ".word 0x00601021\n\t"
        ".word 0x2402ffff\n\t"
        ".word 0x03e00008\n\t"
        ".word 0x00000000\n\t"
        ".end PCcreate");

__asm__(
        ".globl PCinit\n\t"
        ".ent PCinit\n\t"
        "PCinit:\n\t"
        ".word 0x0000404d\n\t"
        ".word 0x03e00008\n\t"
        ".word 0x00000000\n\t"
        ".end PCinit");

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libsn", PCread);

__asm__(
        ".globl func_8004C458\n\t"
        ".ent func_8004C458\n\t"
        "func_8004C458:\n\t"
        ".word 0x0000414d\n\t"
        ".word 0x10400002\n\t"
        ".word 0x00601021\n\t"
        ".word 0x2402ffff\n\t"
        ".word 0x03e00008\n\t"
        ".word 0x00000000\n\t"
        ".end func_8004C458");

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libsn", PCwrite);
