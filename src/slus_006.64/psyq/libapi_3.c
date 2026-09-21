#include "common.h"

// Belongs to libapi?

__asm__(
        ".globl ChangeClearRCnt\n\t"
        ".ent ChangeClearRCnt\n\t"
        "ChangeClearRCnt:\n\t"
        ".word 0x240a00c0\n\t"
        ".word 0x01400008\n\t"
        ".word 0x2409000a\n\t"
        ".end ChangeClearRCnt");
