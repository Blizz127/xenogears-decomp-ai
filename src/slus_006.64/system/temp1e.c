#include "common.h"
#include "psyq/libgpu.h"
#include "psyq/libgte.h"
#include "system/memory.h"

/* Retail TU 0x800250E0..0x80025C04 (sprite/prim work-buffer allocation), split out of system/temp1.c: retail temp1
 * was several TUs, which is visible in how each group reaches the shared Gfx
 * globals (gp-relative for its own .sbss, absolute for other TUs'). The port
 * still builds these functions from temp1.c, so this TU is empty there. */
#ifndef XENO_PC_PORT

// This function is in another TU than func_80024F64 due to GP Rel variables
typedef struct LinkedListEntry {
    void* pData;
    struct LinkedListEntry* pNext;
} LinkedListEntry;

typedef struct ImageEntry {
    RECT rect;
    u_long* addr;
    struct ImageEntry* pNext;
} ImageEntry;

/* This TU's own .sbss (reached %gp_rel); g_GfxWorkBufferSize is shared with
 * temp1b as a common. */
s32 g_GfxCurContext;
int g_GfxWorkBufferSize;
s32 D_80059524;
void* g_GfxCurWorkBufferEnd;
void* g_GfxCurWorkBuffer;

extern LinkedListEntry* D_80059300[2];
extern void* g_GfxWorkBuffers[];
extern ImageEntry* g_GfxImageList[];

/* Starts a frame on work buffer `context`: resets the bump pointer and frees
 * the heap blocks queued on that context's list. */
void func_800250E0(int context) {
    void* pPrimBuffer;
    LinkedListEntry* pCurEntry;

    pPrimBuffer = g_GfxWorkBuffers[context];
    pCurEntry = D_80059300[context];
    g_GfxCurContext = context;
    g_GfxCurWorkBuffer = pPrimBuffer;
    D_80059524 = (s32)pPrimBuffer;
    g_GfxCurWorkBufferEnd = (u8*)pPrimBuffer + g_GfxWorkBufferSize;
    while (pCurEntry != NULL) {
        HeapFree(pCurEntry->pData);
        pCurEntry = pCurEntry->pNext;
    }

    D_80059300[context] = NULL;
}

/* Queues pData to be freed when this context's buffer is next reused: bumps
 * an 8-byte node off the work buffer (retail advances even when it is NULL). */
void func_80025180(void* pData) {
    LinkedListEntry* pEntry = (LinkedListEntry*)g_GfxCurWorkBuffer;

    g_GfxCurWorkBuffer = pEntry + 1;
    if (pEntry != NULL) {
        pEntry->pData = pData;
        pEntry->pNext = D_80059300[g_GfxCurContext];
        D_80059300[g_GfxCurContext] = pEntry;
    }
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1e", func_800251C8);
/* Candidate C: same instructions except that retail loads `height` from
 * the stack before the bounds check (scheduling only).
 Queues a LoadImage of addr into (x, y, width, height) for this frame. 
void func_800251C8(u_long* addr, int x, int y, int width, int height) {
    ImageEntry* pCurrent;
    ImageEntry* pNext;

    pCurrent = (ImageEntry*)g_GfxCurWorkBuffer;
    pNext = pCurrent + 1;
    if ((void*)pNext < g_GfxCurWorkBufferEnd) {
        pCurrent->rect.h = height;
        pCurrent->rect.x = x;
        pCurrent->rect.y = y;
        pCurrent->rect.w = width;
        pCurrent->addr = addr;
        g_GfxCurWorkBuffer = (void*)pNext;

        pCurrent->pNext = g_GfxImageList[g_GfxCurContext];
        g_GfxImageList[g_GfxCurContext] = pCurrent;
    }
}
*/

/* Sets a sprite task's trigger callback from the D_8004FD40 handler table. */
extern void (*D_8004FD40[])(void*);
extern void WorkListSetTaskCallback(void* pTask, void (*pCallback)(void*));

void func_80025224(void* pTask, int handlerIndex) {
    WorkListSetTaskCallback(pTask, D_8004FD40[handlerIndex]);
}

extern s32 D_80050100;
extern u8 D_800C3664;
extern void func_8001E3D8(void* pSpriteData, void* ot);
extern void func_8001E298(void* pSpriteData, void* ot);


INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1e", func_80025258);

extern s32 D_80050100;

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1e", func_8002541C);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1e", func_80025544);

void func_80025710(void) {}

extern MATRIX D_8004FBB8;
extern void func_8002C700(void* a, void* b, u_long* ot, s32 flags);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1e", func_80025718);

/* Transcribed from asm/slus_006.64/nonmatchings/system/temp1/func_800257F0.s
 * (0x800257F0-0x800258A0 ... 0x80025A6C). Sprite transform/render setup:
 * func_80022038 on the sprite, an optional colour/light matrix block built from
 * the +0x20 data (RotMatrix + two MulMatrix0 with D_8004FDA0, SetBackColor
 * 0x202020, SetColorMatrix/SetLightMatrix then PopMatrix), the +2/+6/+0xA
 * translation via TransMatrix, either CompMatrix(D_8004FBB8) or the raw matrix
 * for SetRotMatrix, SetTransMatrix, an optional geom-offset override
 * (ReadGeomOffset/SetGeomOffset 0xA0,0x70) and finally func_800B1F6C with either
 * the +0x30 half-word and D_80050100 unchanged, or D_80050100 = 0x10 with 0xFEC
 * (restored afterwards); the geom offset is restored when the flag word was
 * negative. */
extern void func_80022038(void* p);
extern u16 D_8004FD80[];
extern void func_800B1F6C();
extern MATRIX D_8004FDA0;

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1e", func_800257F0);

extern s32 D_80050100;
extern void func_800B1F6C(void* a, void* b, u_long* ot, s32 c, s32 d, s32 e, s32 f);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1e", func_80025A88);

#endif /* !XENO_PC_PORT */
