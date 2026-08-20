#include "common.h"
#include "psyq/kernel.h"

typedef struct {
    u16 rootCounter;
    s16 unk2;
    s16 mode;
    s16 : 16;
    s16 target;
    s32 : 32;
} Counter;

extern volatile long* g_pInterruptStatusRegister;
extern volatile Counter* g_pRCounters;
extern volatile long g_InterruptStatusMasks[4];

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libapi", ChangeClearPAD);

long SetRCnt(long spec, short target, long mode) {
    int i = spec & 0xFFFF;
    int final_mode = 0x48;

    if (i >= 3)
        return 0;

    g_pRCounters[i].mode = 0;
    g_pRCounters[i].target = target;

    if (i < 2U) {
        if (mode & RCntMdGATE) final_mode = 0x49;
        if (!(mode & RCntMdSC)) final_mode |= 0x100;
    } else if (i == 2U) {
        if (!(mode & RCntMdSC)) final_mode = 0x248;
    }

    if ((mode & RCntMdINTR) != 0)
        final_mode |= 0x10;

    g_pRCounters[i].mode = final_mode;
    return 1;
}

long GetRCnt(long spec) {
    int i = spec & 0xFFFF;
    if (i >= 3)
        return 0;
    return g_pRCounters[i].rootCounter;
}

long StartRCnt(long spec) {
    int i = spec & 0xFFFF;
    long* mask = g_InterruptStatusMasks;

    g_pInterruptStatusRegister[1] |= mask[i];
    return i < 3;
}

long StopRCnt(long spec) {
    int i = spec & 0xFFFF;
    long* mask = g_InterruptStatusMasks;
    g_pInterruptStatusRegister[1] &= ~mask[i];
    return 1;
}

long ResetRCnt(long spec) {
    int i = spec & 0xFFFF;
    if (i >= 3)
        return 0;
    g_pRCounters[i].rootCounter = 0;
    return 1;
}

extern s32 D_80056414;

void func_8004076C(s32 a0) {
    D_80056414 = a0;
}

s32 func_8004077C(void) {
    return D_80056414;
}

extern void func_80040C5C(void);
extern void _patch_pad(void);
extern void ChangeClearPAD(s32);
extern void func_80040ABC(s32, s32, s32, s32);
extern void func_80040BA4(void);

s32 func_8004078C(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    func_80040C5C();
    EnterCriticalSection();
    _patch_pad();
    ExitCriticalSection();
    ChangeClearPAD(0);
    func_8004092C();
    func_80040ABC(arg0, arg1, arg2, arg3);
    func_80040BA4();
    D_80056414 = 1;
    return 1;
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libapi", InitPAD);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libapi", StartPAD);

extern void func_80040B00(void);
extern void StopPAD2(void);
extern s32 D_80056414;

void func_800408F4(void) {
    func_80040B00();
    StopPAD2();
    func_800409AC();
    D_80056414 = 0;
}

extern void* D_8005A204;
extern void* D_8005A208;
extern void* D_8005A200;
extern void* D_8005A20C;
extern void func_800409E4(void);
extern s32 func_80040A4C(void);
extern void SysEnqIntRP(s32, void*);

s32 func_8004092C(void) {
    void* pHandler = (u8*)&D_8005A204 - 4;
    EnterCriticalSection();
    D_8005A204 = func_800409E4;
    D_8005A208 = func_80040A4C;
    D_8005A200 = NULL;
    D_8005A20C = NULL;
    SysDeqIntRP(1, pHandler);
    SysEnqIntRP(1, pHandler);
    ExitCriticalSection();
    return 1;
}

extern void* D_8005A200;
extern void SysDeqIntRP(s32, void*);

s32 func_800409AC(void) {
    EnterCriticalSection();
    SysDeqIntRP(1, D_8005A200);
    ExitCriticalSection();
    return 1;
}

extern void* D_80056418;

void func_800409E4(void) {
    s32 i;
    void* pPad = D_80056418;
    *(u16*)((u8*)pPad + 0xA) = 0;
    for (i = 9; i != -1; i--) {}
}

extern void* D_8005641C;

s32 func_80040A4C(void) {
    void* pPad = D_8005641C;
    if (!(*(u32*)((u8*)pPad + 4) & 1)) return 0;
    if (*(u32*)pPad & 1) return 0;
    return 1;
}

__asm__(
        ".globl InitPAD2\n\t"
        ".ent InitPAD2\n\t"
        "InitPAD2:\n\t"
        ".word 0x240a00b0\n\t"
        ".word 0x01400008\n\t"
        ".word 0x24090012\n\t"
        ".end InitPAD2");

__asm__(
        ".globl StartPAD2\n\t"
        ".ent StartPAD2\n\t"
        "StartPAD2:\n\t"
        ".word 0x240a00b0\n\t"
        ".word 0x01400008\n\t"
        ".word 0x24090013\n\t"
        ".end StartPAD2");

__asm__(
        ".globl StopPAD2\n\t"
        ".ent StopPAD2\n\t"
        "StopPAD2:\n\t"
        ".word 0x240a00b0\n\t"
        ".word 0x01400008\n\t"
        ".word 0x24090014\n\t"
        ".end StopPAD2");

__asm__(
        ".globl func_80040ABC\n\t"
        ".ent func_80040ABC\n\t"
        "func_80040ABC:\n\t"
        ".word 0x240a00b0\n\t"
        ".word 0x01400008\n\t"
        ".word 0x24090015\n\t"
        ".end func_80040ABC");

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libapi", SysEnqIntRP);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libapi", SysDeqIntRP);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libapi", EnablePAD);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libapi", func_80040B00);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libapi", _patch_pad);
