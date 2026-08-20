#include "common.h"

__asm__(
        ".globl WaitEvent\n\t"
        ".ent WaitEvent\n\t"
        "WaitEvent:\n\t"
        ".word 0x240a00b0\n\t"
        ".word 0x01400008\n\t"
        ".word 0x2409000a\n\t"
        ".end WaitEvent");
