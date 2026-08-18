#include "common.h"
#include "psyq/interrupts.h"

// Adapted from https://github.com/sozud/psy-q-decomp

void trapIntrDMA(void);
VoidCallback_t setIntrDMA(unsigned int index, VoidCallback_t callback);
static void memclrIntrDMA(void(**callbacks)(void), unsigned int numCallbacks);

void* startIntrDMA(void) {
    memclrIntrDMA(&g_DmaInterruptCallbacks, MAX_INTERRUPT_CALLBACKS);
    *g_pDMA_DICR = 0;
    InterruptCallback(3, &trapIntrDMA);
    return &setIntrDMA;
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libetc/intr_dma", trapIntrDMA);

VoidCallback_t setIntrDMA(unsigned int index, VoidCallback_t callback) {
    VoidCallback_t old = g_DmaInterruptCallbacks[index];
    if (callback != old) {
        if (callback != NULL) {
            g_DmaInterruptCallbacks[index] = callback;
            *g_pDMA_DICR = (*g_pDMA_DICR & 0xFFFFFF) | (1 << (index + 0x10)) | 0x800000;
        } else {
            g_DmaInterruptCallbacks[index] = NULL;
            *g_pDMA_DICR = (*g_pDMA_DICR & 0xFFFFFF) | 0x800000;
            *g_pDMA_DICR &= ~(1 << (index + 0x10));
        }
    }
    return old;
}

static void memclrIntrDMA(void(**callbacks)(void), unsigned int numCallbacks) {
    while (numCallbacks--) {
        *callbacks++ = NULL;
    }
}