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

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libapi", func_8004078C);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libapi", InitPAD);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libapi", StartPAD);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libapi", func_800408F4);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libapi", func_8004092C);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libapi", func_800409AC);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libapi", func_800409E4);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libapi", func_80040A4C);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libapi", InitPAD2);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libapi", StartPAD2);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libapi", StopPAD2);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libapi", func_80040ABC);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libapi", SysEnqIntRP);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libapi", SysDeqIntRP);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libapi", EnablePAD);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libapi", func_80040B00);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libapi", _patch_pad);
