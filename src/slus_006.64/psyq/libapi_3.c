#ifdef XENO_PC_PORT
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

#else /* retail matching build: last byte-exact version (dbb08e3b) */
#include "common.h"

// Belongs to libapi?

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libapi_3", ChangeClearRCnt);
#endif
