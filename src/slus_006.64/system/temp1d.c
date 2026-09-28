#include "common.h"
#include "psyq/libgpu.h"
#include "psyq/libgte.h"
#include "system/memory.h"

/* Retail TU 0x80025044..0x800250E0 (image-list upload), split out of system/temp1.c: retail temp1
 * was several TUs, which is visible in how each group reaches the shared Gfx
 * globals (gp-relative for its own .sbss, absolute for other TUs'). The port
 * still builds these functions from temp1.c, so this TU is empty there. */
#ifndef XENO_PC_PORT

typedef struct Image {
    RECT rect;
    u_long* addr;
    struct Image* pNext;
} Image;

int g_GfxCurContext; /* this TU's own .sbss */
extern Image* g_GfxImageList[2];

/* Walks g_GfxImageList[g_GfxCurContext]: LoadImage entries with pixel data,
 * ClearImage the rest, then empties the list. */
void func_80025044(void) {
    Image* pListHead;
    Image* pImage;

    pListHead = g_GfxImageList[g_GfxCurContext];
    for (pImage = pListHead; pImage != NULL; pImage = pImage->pNext) {
        if (pImage->addr) {
            LoadImage(&pImage->rect, pImage->addr);
        } else {
            ClearImage(&pImage->rect, 0x0, 0x0, 0x0);
        }
    }

    g_GfxImageList[g_GfxCurContext] = NULL;
}

#endif /* !XENO_PC_PORT */
