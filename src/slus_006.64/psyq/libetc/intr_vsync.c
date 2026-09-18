#include "common.h"
#include "psyq/interrupts.h"

// Adapted from https://github.com/sozud/psy-q-decomp

SetVsyncIntrCallback_t startIntrVSync();
void trapIntrVSync(void);
void setIntrVSync(unsigned int index, VoidCallback_t callback);
void memclrIntrVSync(void(**callbacks)(void), unsigned int numCallbacks);

SetVsyncIntrCallback_t startIntrVSync() {
    *g_pTMR_HRETRACE_MODE = 0x107;
    g_VsyncInterruptCount = 0;
    memclrIntrVSync(g_VsyncInterruptCallbacks, 8);
    InterruptCallback(0, trapIntrVSync);
    return &setIntrVSync;
}

void trapIntrVSync(void) {
    int i;

    g_VsyncInterruptCount++;
    for (i = 0; i < MAX_INTERRUPT_CALLBACKS; i++) {
        if (g_VsyncInterruptCallbacks[i]) {
            g_VsyncInterruptCallbacks[i]();
        }
    }
}

void setIntrVSync(unsigned int index, VoidCallback_t callback) {
    if (callback != g_VsyncInterruptCallbacks[index]) {
        g_VsyncInterruptCallbacks[index] = callback;
    }
}

void memclrIntrVSync(void(**callbacks)(void), unsigned int numCallbacks) {
    while (numCallbacks--) {
        *callbacks++ = NULL;
    }
}