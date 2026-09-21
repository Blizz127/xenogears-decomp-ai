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

void InitCARD(long val) {
    extern void func_8004E8D8(void);
    extern void func_8004E990(void);

    ChangeClearPAD(0);
    EnterCriticalSection();
    InitCARD2(val);
    func_8004E8D8();
    func_8004E990();
    ExitCriticalSection();
}

void StartCARD(void) {
    EnterCriticalSection();
    StartCARD2();
    ChangeClearPAD(0);
    ExitCriticalSection();
}

long StopCARD(void) {
    extern void func_8004E990(void);
    extern long func_8004EA20(void);

    StopCARD2();
    func_8004E990();
    return func_8004EA20();
}

__asm__(
        ".globl InitCARD2\n\t"
        ".ent InitCARD2\n\t"
        "InitCARD2:\n\t"
        ".word 0x240a00b0\n\t"
        ".word 0x01400008\n\t"
        ".word 0x2409004a\n\t"
        ".end InitCARD2");

__asm__(
        ".globl StartCARD2\n\t"
        ".ent StartCARD2\n\t"
        "StartCARD2:\n\t"
        ".word 0x240a00b0\n\t"
        ".word 0x01400008\n\t"
        ".word 0x2409004b\n\t"
        ".end StartCARD2");

__asm__(
        ".globl StopCARD2\n\t"
        ".ent StopCARD2\n\t"
        "StopCARD2:\n\t"
        ".word 0x240a00b0\n\t"
        ".word 0x01400008\n\t"
        ".word 0x2409004c\n\t"
        ".end StopCARD2");
