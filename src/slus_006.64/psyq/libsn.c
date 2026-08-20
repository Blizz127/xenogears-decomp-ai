#include "common.h"

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libsn", PCopen);

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

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libsn", PCcreate);

__asm__(
        ".globl PCinit\n\t"
        ".ent PCinit\n\t"
        "PCinit:\n\t"
        ".word 0x0000404d\n\t"
        ".word 0x03e00008\n\t"
        ".word 0x00000000\n\t"
        ".end PCinit");

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libsn", PCread);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libsn", func_8004C458);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libsn", PCwrite);
