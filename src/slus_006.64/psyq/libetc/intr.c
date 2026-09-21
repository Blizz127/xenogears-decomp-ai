#include "common.h"
#include "psyq/interrupts.h"

// Adapted from https://github.com/sozud/psy-q-decomp


extern u_short D_800578A6;
extern u_short D_800578D4;
extern u_short D_800578D6;
extern int D_800578D8;
extern VoidCallback_t D_800578A8[];
extern int D_800578E0;

int setjmp(int*);
SetVsyncIntrCallback_t startIntrVSync();
VoidCallback_t setIntrDMA(int index, VoidCallback_t callback);
void trapIntrDMA(void);

void ChangeClearRCnt(int, int);

long long startIntrDMA();

/*
static InterruptEnvironment_t g_InterruptEnvironment = {
    0,
    0,
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
    0,
    0,
    0,
};

static InterruptControl_t g_InterruptControl = {
    "$$Id: intr.c,v 1.76 1997/02/12 12:45:05 makoto Exp $",
    0,
    setIntr,
    startIntr,
    stopIntr,
    0,
    restartIntr,
    &g_InterruptEnvironment
};
*/



void* ResetCallback(void) { 
    return g_pInterruptControl->intrResetHandler();
}

void InterruptCallback(unsigned int irq, VoidCallback_t fn) {
    g_pInterruptControl->setIntrHandler(irq, fn);
}

VoidCallback_t DMACallback(int dma, VoidCallback_t fn) {
    return g_pInterruptControl->setDMAIntrCallback(dma, fn);
}

void func_8004B7D0(VoidCallback_t fn) {
    g_pInterruptControl->setVsyncIntrCallback(4, fn);
}

int func_8004B804(int ch, VoidCallback_t fn) {
    return g_pInterruptControl->setVsyncIntrCallback(ch, fn);
}

void* StopCallback(void) {
    return g_pInterruptControl->intrStopHandler(); 
}

void* RestartCallback(void) {
    return g_pInterruptControl->intrRestartHandler(); 
}

int CheckCallback(void) {
    return D_800578A6;
}

u_short GetIntrMask(void) { 
    return *g_pI_MASK; 
}

u_short SetIntrMask(u_short newMask) {
    u_short mask;

    mask = *g_pI_MASK;
    *g_pI_MASK = newMask;
    return mask;
}

#if defined(SKIP_ASM) || defined(XENO_PC_PORT)
void* startIntr() {
    if (g_InterruptEnvironment.interruptsInitialized)
        return NULL;

    *g_pI_STAT = *g_pI_MASK = 0;
    *g_pDMA_DPCR = 0x33333333;
    memclrIntr(&g_InterruptEnvironment, sizeof(g_InterruptEnvironment) / sizeof(int));
    if (setjmp(g_InterruptEnvironment.buf))
        trapIntr();

    g_InterruptEnvironment.buf[JB_SP] = (int)&g_InterruptEnvironment.stack[1004];
    HookEntryInt((u_short*)g_InterruptEnvironment.buf);
    g_InterruptEnvironment.interruptsInitialized = 1;
    g_pInterruptControl->setVsyncIntrCallback = startIntrVSync();
    g_pInterruptControl->setDMAIntrCallback = startIntrDMA();
    func_8004BED8();
    ExitCriticalSection();
    return &g_InterruptEnvironment;
}
#else
__asm__(
        ".globl startIntr\n\t"
        ".ent\tstartIntr\n\t"
        "startIntr:\n\t"
        ".word 0x27bdffe8\n\t"
        ".word 0xafb00010\n\t"
        "lui $s0, %hi(g_InterruptEnvironment)\n\t"
        "addiu $s0, $s0, %lo(g_InterruptEnvironment)\n\t"
        ".word 0xafbf0014\n\t"
        ".word 0x96020000\n\t"
        ".word 0x00000000\n\t"
        ".word 0x1440002a\n\t"
        ".word 0x00001021\n\t"
        "lui $v1, %hi(g_pI_STAT)\n\t"
        "lw $v1, %lo(g_pI_STAT)($v1)\n\t"
        "lui $v0, %hi(g_pI_MASK)\n\t"
        "lw $v0, %lo(g_pI_MASK)($v0)\n\t"
        ".word 0x3c053333\n\t"
        ".word 0xa4400000\n\t"
        ".word 0x94420000\n\t"
        ".word 0x34a53333\n\t"
        ".word 0xa4620000\n\t"
        "lui $v0, %hi(g_pDMA_DPCR)\n\t"
        "lw $v0, %lo(g_pDMA_DPCR)($v0)\n\t"
        ".word 0x02002021\n\t"
        ".word 0xac450000\n\t"
        ".set\tnoreorder\n\t"
        "jal memclrIntr\n\t"
        "addiu $a1, $zero, 0x41A\n\t"
        ".set\treorder\n\t"
        ".set\tnoreorder\n\t"
        "jal setjmp\n\t"
        "addiu $a0, $s0, 0x38\n\t"
        ".set\treorder\n\t"
        ".word 0x10400003\n\t"
        ".word 0x00000000\n\t"
        ".set\tnoreorder\n\t"
        "jal trapIntr\n\t"
        ".word 0x00000000\n\t"
        ".set\treorder\n\t"
        "lui $s0, %hi(D_800578E0)\n\t"
        "addiu $s0, $s0, %lo(D_800578E0)\n\t"
        ".word 0x2604fffc\n\t"
        ".word 0x26020fdc\n\t"
        ".set\tnoreorder\n\t"
        "jal HookEntryInt\n\t"
        "sw $v0, 0($s0)\n\t"
        ".set\treorder\n\t"
        ".word 0x24020001\n\t"
        ".set\tnoreorder\n\t"
        "jal startIntrVSync\n\t"
        ".word 0xa602ffc4\n\t"
        ".set\treorder\n\t"
        "lui $v1, %hi(g_pInterruptControl)\n\t"
        "lw $v1, %lo(g_pInterruptControl)($v1)\n\t"
        ".set\tnoreorder\n\t"
        "jal startIntrDMA\n\t"
        "sw $v0, 20($v1)\n\t"
        ".set\treorder\n\t"
        "lui $a0, %hi(g_pInterruptControl)\n\t"
        "lw $a0, %lo(g_pInterruptControl)($a0)\n\t"
        ".set\tnoreorder\n\t"
        "jal func_8004BED8\n\t"
        "sw $v0, 4($a0)\n\t"
        ".set\treorder\n\t"
        ".set\tnoreorder\n\t"
        "jal ExitCriticalSection\n\t"
        "addiu $s0, $s0, -60\n\t"
        ".set\treorder\n\t"
        ".word 0x02001021\n\t"
        ".word 0x8fbf0014\n\t"
        ".word 0x8fb00010\n\t"
        ".word 0x27bd0018\n\t"
        ".word 0x03e00008\n\t"
        ".word 0x00000000\n\t"
        ".end\tstartIntr");
#endif

extern char D_80019408; // "unexpected interrupt(%04x)\n"
extern char D_80019424; // "intr timeout(%04x:%04x)\n"
extern u_short D_800578A6;
extern u_short D_800578D4;

void trapIntr() {
    int i;
    u_short mask;

    if (g_InterruptEnvironment.interruptsInitialized == 0) {
        printf(&D_80019408, *g_pI_STAT);
        ReturnFromException();
    }

    D_800578A6 = 1;
    while (mask = (D_800578D4 & *g_pI_STAT) & *g_pI_MASK) {
        for (i = 0; mask && i < 11; ++i, mask >>= 1) {
            if (mask & 1) {
                *g_pI_STAT = ~(1 << i);
                if (g_InterruptEnvironment.handlers[i]) {
                    g_InterruptEnvironment.handlers[i]();
                }
            }
        }
    }

    if (*g_pI_STAT & *g_pI_MASK) {
        if (D_8005893C++ > 0x800) {
            printf(&D_80019424, *g_pI_STAT, *g_pI_MASK);
            D_8005893C = 0;
            *g_pI_STAT = 0;
        }
    } else {
        D_8005893C = 0;
    }

    D_800578A6 = 0;
    ReturnFromException();
}

#if defined(SKIP_ASM) || defined(XENO_PC_PORT)
VoidCallback_t setIntr(int index, VoidCallback_t fn) {
    VoidCallback_t pHandlerFn;
    u_short nMask;
    int nNewMask;
    VoidCallback_t* pHandlers = D_800578A8;

    pHandlerFn = pHandlers[index];
    if ((fn != pHandlerFn) && *(u_short*)((u8*)pHandlers - 4)) {
        nMask = *g_pI_MASK;
        *g_pI_MASK = 0;
        nNewMask = nMask & 0xFFFF;
        if (fn != NULL) {
            pHandlers[index] = fn;
            nNewMask = nNewMask | (1 << index);
            *(u_short*)((u8*)pHandlers + 0x2C) |= (1 << index);
        } else {
            pHandlers[index] = 0;
            nNewMask = nNewMask & ~(1 << index);
            D_800578D4 &= ~(1 << index);
        }
        if (index == 0) {
            ChangeClearPAD(fn == NULL);
            ChangeClearRCnt(3, fn == NULL);
        }
        if (index == 4) {
            ChangeClearRCnt(0, fn == NULL);
        }
        if (index == 5) {
            ChangeClearRCnt(1, fn == NULL);
        }
        if (index == 6) {
            ChangeClearRCnt(2, fn == NULL);
        }
        *g_pI_MASK = nNewMask;
    }
    return pHandlerFn;
}
#else
__asm__(
        ".globl setIntr\n\t"
        ".ent\tsetIntr\n\t"
        "setIntr:\n\t"
        ".word 0x27bdffd8\n\t"
        ".word 0xafb10014\n\t"
        ".word 0x00808821\n\t"
        ".word 0xafb20018\n\t"
        ".word 0x00a09021\n\t"
        "lui $a1, %hi(D_800578A8)\n\t"
        "addiu $a1, $a1, %lo(D_800578A8)\n\t"
        ".word 0x00111080\n\t"
        ".word 0x00452021\n\t"
        ".word 0xafbf0024\n\t"
        ".word 0xafb40020\n\t"
        ".word 0xafb3001c\n\t"
        ".word 0xafb00010\n\t"
        ".word 0x8c940000\n\t"
        ".word 0x00000000\n\t"
        ".word 0x1254003c\n\t"
        ".word 0x02801021\n\t"
        ".word 0x94a2fffc\n\t"
        ".word 0x00000000\n\t"
        ".word 0x10400038\n\t"
        ".word 0x02801021\n\t"
        "lui $v0, %hi(g_pI_MASK)\n\t"
        "lw $v0, %lo(g_pI_MASK)($v0)\n\t"
        ".word 0x00000000\n\t"
        ".word 0x94430000\n\t"
        ".word 0xa4400000\n\t"
        ".word 0x12400009\n\t"
        ".word 0x3073ffff\n\t"
        ".word 0x24030001\n\t"
        ".word 0x02231804\n\t"
        ".word 0xac920000\n\t"
        ".word 0x94a2002c\n\t"
        ".word 0x02639825\n\t"
        ".word 0x00431025\n\t"
        ".reloc ., R_MIPS_26, .LsetIntr54\n\t"
        ".word 0x08000000\n\t"
        ".word 0xa4a2002c\n\t"
        ".word 0x24020001\n\t"
        ".word 0x02221004\n\t"
        ".word 0x00021027\n\t"
        ".word 0xac800000\n\t"
        "lui $v1, %hi(D_800578D4)\n\t"
        "lhu $v1, %lo(D_800578D4)($v1)\n\t"
        ".word 0x02629824\n\t"
        ".word 0x00621824\n\t"
        "lui $at, %hi(D_800578D4)\n\t"
        "sh $v1, %lo(D_800578D4)($at)\n\t"
        ".LsetIntr54:\n\t"
        ".word 0x16200008\n\t"
        ".word 0x24020004\n\t"
        ".word 0x2e500001\n\t"
        ".set\tnoreorder\n\t"
        "jal ChangeClearPAD\n\t"
        "addu $a0, $s0, $zero\n\t"
        ".set\treorder\n\t"
        ".word 0x24040003\n\t"
        ".set\tnoreorder\n\t"
        "jal ChangeClearRCnt\n\t"
        "addu $a1, $s0, $zero\n\t"
        ".set\treorder\n\t"
        ".word 0x24020004\n\t"
        ".word 0x16220005\n\t"
        ".word 0x24020005\n\t"
        ".word 0x00002021\n\t"
        ".set\tnoreorder\n\t"
        "jal ChangeClearRCnt\n\t"
        "sltiu $a1, $s2, 0x1\n\t"
        ".set\treorder\n\t"
        ".word 0x24020005\n\t"
        ".word 0x16220005\n\t"
        ".word 0x24020006\n\t"
        ".word 0x24040001\n\t"
        ".set\tnoreorder\n\t"
        "jal ChangeClearRCnt\n\t"
        "sltiu $a1, $s2, 0x1\n\t"
        ".set\treorder\n\t"
        ".word 0x24020006\n\t"
        ".word 0x16220003\n\t"
        ".word 0x24040002\n\t"
        ".set\tnoreorder\n\t"
        "jal ChangeClearRCnt\n\t"
        "sltiu $a1, $s2, 0x1\n\t"
        ".set\treorder\n\t"
        "lui $v0, %hi(g_pI_MASK)\n\t"
        "lw $v0, %lo(g_pI_MASK)($v0)\n\t"
        ".word 0x00000000\n\t"
        ".word 0xa4530000\n\t"
        ".word 0x02801021\n\t"
        ".word 0x8fbf0024\n\t"
        ".word 0x8fb40020\n\t"
        ".word 0x8fb3001c\n\t"
        ".word 0x8fb20018\n\t"
        ".word 0x8fb10014\n\t"
        ".word 0x8fb00010\n\t"
        ".word 0x27bd0028\n\t"
        ".word 0x03e00008\n\t"
        ".word 0x00000000\n\t"
        ".end\tsetIntr");
#endif

void* stopIntr() {
    if (!g_InterruptEnvironment.interruptsInitialized)
        return NULL;

    EnterCriticalSection();
    D_800578D6 = *g_pI_MASK;
    D_800578D8 = *g_pDMA_DPCR;
    *g_pI_STAT = *g_pI_MASK = 0;
    *g_pDMA_DPCR &= 0x77777777;
    ResetEntryInt();
    g_InterruptEnvironment.interruptsInitialized = 0;
    return &g_InterruptEnvironment;
}

void* restartIntr() {
    if (g_InterruptEnvironment.interruptsInitialized)
        return 0;

    HookEntryInt((u_short*)g_InterruptEnvironment.buf);
    g_InterruptEnvironment.interruptsInitialized = 1;
    *g_pI_MASK = D_800578D6;
    *g_pDMA_DPCR = D_800578D8;
    ExitCriticalSection();
    return &g_InterruptEnvironment;
}

void memclrIntr(void(**callbacks)(void), unsigned int numCallbacks) {
    while (numCallbacks--) {
        *callbacks++ = NULL;
    }
}
