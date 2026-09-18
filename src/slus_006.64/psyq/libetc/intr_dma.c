#include "common.h"
#include "psyq/interrupts.h"

// Adapted from https://github.com/sozud/psy-q-decomp

void trapIntrDMA(void);
VoidCallback_t setIntrDMA(unsigned int index, VoidCallback_t callback);
void memclrIntrDMA(void(**callbacks)(void), unsigned int numCallbacks);

void* startIntrDMA(void) {
    memclrIntrDMA(&g_DmaInterruptCallbacks, MAX_INTERRUPT_CALLBACKS);
    *g_pDMA_DICR = 0;
    InterruptCallback(3, &trapIntrDMA);
    return &setIntrDMA;
}

#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libetc/intr_dma", trapIntrDMA);
#else
void trapIntrDMA(void) {
}
#endif

#if defined(SKIP_ASM) || defined(XENO_PC_PORT)
VoidCallback_t setIntrDMA(unsigned int index, VoidCallback_t callback) {
    VoidCallback_t old = g_DmaInterruptCallbacks[index];
    if (callback != old) {
        if (callback != NULL) {
            g_DmaInterruptCallbacks[index] = callback;
            *g_pDMA_DICR = (*g_pDMA_DICR & 0xFFFFFF) | (0x800000 | (1 << (index + 0x10)));
        } else {
            int reg = *g_pDMA_DICR;
            g_DmaInterruptCallbacks[index] = NULL;
            reg = (reg & 0xFFFFFF) | 0x800000;
            *g_pDMA_DICR = reg & ~(1 << (index + 0x10));
        }
    }
    return old;
}
#else
__asm__(
        ".globl setIntrDMA\n\t"
        ".ent\tsetIntrDMA\n\t"
        "setIntrDMA:\n\t"
        ".word 0x00803021\n\t"
        "lui $v1, %hi(g_DmaInterruptCallbacks)\n\t"
        "addiu $v1, $v1, %lo(g_DmaInterruptCallbacks)\n\t"
        ".word 0x00061080\n\t"
        ".word 0x00431821\n\t"
        ".word 0x8c670000\n\t"
        ".word 0x00a02021\n\t"
        ".word 0x10870020\n\t"
        ".word 0x00000000\n\t"
        ".word 0x10800010\n\t"
        ".word 0x3c0200ff\n\t"
        "lui $a1, %hi(g_pDMA_DICR)\n\t"
        "lw $a1, %lo(g_pDMA_DICR)($a1)\n\t"
        ".word 0x3442ffff\n\t"
        ".word 0xac640000\n\t"
        ".word 0x8ca40000\n\t"
        ".word 0x24c30010\n\t"
        ".word 0x00822024\n\t"
        ".word 0x24020001\n\t"
        ".word 0x00621004\n\t"
        ".word 0x3c030080\n\t"
        ".word 0x00431025\n\t"
        ".word 0x00822025\n\t"
        ".word 0xaca40000\n\t"
        ".reloc ., R_MIPS_26, .LsetIntrDMAEnd\n\t"
        ".word 0x08000000\n\t"
        ".word 0x00000000\n\t"
        "lui $a1, %hi(g_pDMA_DICR)\n\t"
        "lw $a1, %lo(g_pDMA_DICR)($a1)\n\t"
        ".word 0x3442ffff\n\t"
        ".word 0xac600000\n\t"
        ".word 0x8ca30000\n\t"
        ".word 0x24c40010\n\t"
        ".word 0x00621824\n\t"
        ".word 0x3c020080\n\t"
        ".word 0x00621825\n\t"
        ".word 0x24020001\n\t"
        ".word 0x00821004\n\t"
        ".word 0x00021027\n\t"
        ".word 0x00621824\n\t"
        ".word 0xaca30000\n\t"
        ".LsetIntrDMAEnd:\n\t"
        ".word 0x03e00008\n\t"
        ".word 0x00e01021\n\t"
        ".end\tsetIntrDMA");
#endif

void memclrIntrDMA(void(**callbacks)(void), unsigned int numCallbacks) {
    while (numCallbacks--) {
        *callbacks++ = NULL;
    }
}