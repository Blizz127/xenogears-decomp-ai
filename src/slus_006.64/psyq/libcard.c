#include "common.h"

#include "psyq/libapi.h"

__asm__(
        ".globl _card_info\n\t"
        ".ent _card_info\n\t"
        "_card_info:\n\t"
        ".word 0x240a00a0\n\t"
        ".word 0x01400008\n\t"
        ".word 0x240900ab\n\t"
        ".end _card_info");

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libcard", InitCARD);

void StartCARD(void) {
    EnterCriticalSection();
    StartCARD2();
    ChangeClearPAD(0);
    ExitCriticalSection();
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libcard", StopCARD);

__asm__(
        ".globl InitCARD2\n\t"
        ".ent InitCARD2\n\t"
        "InitCARD2:\n\t"
        ".word 0x240a00b0\n\t"
        ".word 0x01400008\n\t"
        ".word 0x2409004a\n\t"
        ".end InitCARD2");

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libcard", StartCARD2);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libcard", StopCARD2);
