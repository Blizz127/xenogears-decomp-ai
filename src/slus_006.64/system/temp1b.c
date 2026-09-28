#include "common.h"
#include "psyq/libgpu.h"
#include "psyq/libgte.h"
#include "system/memory.h"

/* Retail TU 0x80024F64..0x80024FF4 (work buffers / current OT), split out of system/temp1.c: retail temp1
 * was several TUs, which is visible in how each group reaches the shared Gfx
 * globals (gp-relative for its own .sbss, absolute for other TUs'). The port
 * still builds these functions from temp1.c, so this TU is empty there. */
#ifndef XENO_PC_PORT

extern void* g_GfxWorkBuffer2;
extern void func_8001D298(void);
extern void func_8001D2A4(void);

/* This TU's own .sbss (reached %gp_rel). */
int g_GfxWorkBufferSize;
s32 D_80059300; /* linked lists of SpriteTileData pointers */
s32 D_80059304;
void* g_GfxWorkBuffers;
s32 g_GfxImageList;

#define NUM_RENDER_CONTEXTS 2

void GfxAllocateWorkBuffers(int workBufferSize, unsigned int allocFlag) {
    void* pWorkBuffers;

    g_GfxWorkBufferSize = workBufferSize;
    pWorkBuffers = HeapAlloc(workBufferSize * NUM_RENDER_CONTEXTS, allocFlag);
    g_GfxWorkBuffers = pWorkBuffers;
    g_GfxWorkBuffer2 = (u8*)pWorkBuffers + workBufferSize;
    D_80059304 = 0;
    D_80059300 = 0;
    g_GfxImageList = 0;
    func_8001D298();
}

void GfxFreeWorkBuffers(void) {
    HeapFree(g_GfxWorkBuffers);
    func_8001D2A4();
}

extern u_long* g_GfxCurOT;

void GfxSetCurrentOT(u_long* ot) {
    g_GfxCurOT = ot;
}

#endif /* !XENO_PC_PORT */
