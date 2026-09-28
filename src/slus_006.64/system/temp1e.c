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
extern void* g_GfxCurWorkBuffer;
extern void* g_GfxCurWorkBufferEnd;
extern uintptr_t D_80059524;

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1e", func_800250E0);
/*
Matches on GCC 2.7.2-970404, ASPSX 2.67

typedef struct {
    void* pData;
    struct LinkedListEntry* pNext;
} LinkedListEntry;

s32 g_GfxCurContext;
int g_GfxWorkBufferSize;
extern LinkedListEntry* D_80059300[2];
extern int g_GfxWorkBuffers[];
s32 D_80059524;
void* g_GfxCurWorkBufferEnd;
void* g_GfxCurWorkBuffer;

void func_800250E0(int context) {
    void* pPrimBuffer;
    LinkedListEntry* pCurEntry;

    pPrimBuffer = g_GfxWorkBuffers[context];
    pCurEntry = D_80059300[context];
    g_GfxCurContext = context;
    g_GfxCurWorkBuffer = pPrimBuffer;
    D_80059524 = pPrimBuffer;
    g_GfxCurWorkBufferEnd = pPrimBuffer + g_GfxWorkBufferSize;
    while (pCurEntry != NULL) {
        HeapFree(pCurEntry->pData);
        pCurEntry = pCurEntry->pNext;
    }
    
    D_80059300[context] = NULL;
}
*/


/* func_80025180: GfxQueueWorkEntry -- bump an 8-byte node off
 * g_GfxCurWorkBuffer (retail advances the head by 8 even when it is NULL) and
 * push it onto the D_80059300[g_GfxCurContext] list, pData at +0 and the
 * previous head at +4.  Not decompiled here on purpose: retail reaches
 * g_GfxCurWorkBuffer and g_GfxCurContext through $gp in one instruction each
 * because they are DEFINED small objects in its TU, while they are `extern`
 * here, so a C body costs three extra instructions (84 vs 72 bytes).  The PORT
 * already owns this symbol as a host-safe override in
 * pc_port/src/game_overrides.c (g_GfxCurWorkBuffer is a u32 there, not a
 * pointer), so adding a body here would be a duplicate definition. */
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1e", func_80025180);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1e", func_800251C8);
/*
Matches, but uses variable in COMMON so can't compile in yet.

struct ImageEntry {
    RECT rect;
    u_long* addr;
    struct ImageEntry* pNext;
};

typedef struct ImageEntry ImageEntry;

extern ImageEntry* g_GfxImageList[];
int g_GfxCurContext; // Cur index?
void* g_GfxCurWorkBufferEnd;
ImageEntry* g_GfxCurWorkBuffer;

// GfxQueueShapeTransfer
void func_800251C8(u_long* addr, int x, int y, int width, int height) {
    ImageEntry* pCurrent;
    ImageEntry* pNext;

    pCurrent = (ImageEntry*) g_GfxCurWorkBuffer;
    pNext = pCurrent + 1;
    if (pNext < g_GfxCurWorkBufferEnd) {
        pCurrent->rect.h = height;
        pCurrent->rect.x = x;
        pCurrent->rect.y = y;
        pCurrent->rect.w = width;
        pCurrent->addr = addr;
        g_GfxCurWorkBuffer = (void*) pNext;

        pCurrent->pNext = g_GfxImageList[g_GfxCurContext];
        g_GfxImageList[g_GfxCurContext] = pCurrent;
    }
}
*/

// Sets pStruct->unk8 callback
/* Transcribed from asm/slus_006.64/nonmatchings/system/temp1/func_80025224.s
 * (0x80025224-0x80025258). Loads the handler pointer from the retail callback
 * table at D_8004FD40 (16 entries; see the table listing below) and hands it to
 * WorkListSetTaskCallback. NOTE: in the current build that table symbol is
 * parked at 0x8004FDBC instead of 0x8004FD40 (data-symbol placement, see
 * ACTIVE_HANDOFF), so the emitted immediate is not yet retail-identical. */
extern void (*D_8004FD40[])(void*);
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1e", func_80025224);
/*
CALLBACK_TABLE
8004fd40 func_80025258
8004fd44 func_80025710 => Dummy function
8004fd48 func_80025718
8004fd4c NULL
8004fd50 NULL
8004fd54 func_80025258
8004fd58 func_80025258
8004fd5c func_80025718
8004fd60 func_8002541c
8004fd64 func_80025544
8004fd68 NULL
8004fd6c NULL
8004fd70 NULL
8004fd74 NULL
8004fd78 func_80025258
8004fd7c func_800257f0

void func_80025224(WorkListEntry* pTask, int handlerIndex) {
    WorkListSetTaskCallback(pTask, &CALLBACK_TABLE[handlerIndex]);
}
*/

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
