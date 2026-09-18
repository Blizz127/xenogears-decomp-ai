#include "common.h"
#include "psyq/interrupts.h"

// Adapted from https://github.com/sozud/psy-q-decomp

void ChangeClearRCnt(int, int);

int Vsync(int mode) {
    int nGpuStatusReg;
    int nResult;
    int nMax;
    int nWaitCount;

    nGpuStatusReg = *g_pGPUSTAT;
    nResult = (*g_pTMR_HRETRACE_VAL - g_HsyncInterruptCount) & 0xFFFF;
    
    if (mode < 0)
        return g_VsyncInterruptCount;
    
    if (mode == 1)
        return nResult;
    
    if (mode > 0) {
        nWaitCount = 1;
        nWaitCount = (g_VsyncPrevInterruptCount - nWaitCount) + mode;
    } else {
        nWaitCount = g_VsyncPrevInterruptCount;
    }
    
    nMax = 0;
    if (mode > 0)
        nMax = mode - 1;
    
    v_wait(nWaitCount, nMax);
    nGpuStatusReg = *g_pGPUSTAT;
    v_wait(g_VsyncInterruptCount + 1, 1);
    if ((nGpuStatusReg & 0x400000) && ((nGpuStatusReg ^ *g_pGPUSTAT) >= 0)) {
        do {} while (!((nGpuStatusReg ^ *g_pGPUSTAT) & 0x80000000));
    }
    g_VsyncPrevInterruptCount = g_VsyncInterruptCount;
    g_HsyncInterruptCount = *g_pTMR_HRETRACE_VAL;
    return nResult;
}

extern char D_800193C4; // "VSync: timeout\n"
void v_wait(int count, int max) {
    volatile int timeout[2];

    timeout[0] = max << 0xF;
    while (g_VsyncInterruptCount < count) {
        if (!timeout[0]--) {
            puts(&D_800193C4);
            ChangeClearPAD(0);
            ChangeClearRCnt(3, 0);
            return;
        }
    }
}