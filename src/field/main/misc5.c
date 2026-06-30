#include "common.h"
#include "field/main.h"
#include "field/actor.h"
#include "system/memory.h"
#include "psyq/libgpu.h"
#include "field/effects.h"
#include "field/graphics.h"

//
void func_800A55B8(void* arg0, s32 arg1, s32 arg2, s32 arg3) {
    *(s32*)((u8*)arg0 + 0x14) = arg1;
    *(s32*)((u8*)arg0 + 0x18) = arg2;
    *(s32*)((u8*)arg0 + 0x1C) = arg3;
}

extern void* D_800AFE80;
extern void* D_800B069C;

void func_800A55C8(void) {
    HeapFree(D_800AFE80);
    HeapFree(D_800B069C);
}

// Set PolyFT4 color for PolyFT4s of next swap chain
void func_800A5600(u_char color) {
    int i;
    POLY_FT4* pPoly;
    int nOffset;
    
    for (i = 0; i < 5; i++) {
        nOffset = i * 2;
        (g_FieldZoomFadeEffect.polygons + nOffset + ((g_FieldCurRenderContextIndex + 1) & 1))->r0 = color;
        (g_FieldZoomFadeEffect.polygons + nOffset + ((g_FieldCurRenderContextIndex + 1) & 1))->g0 = color;
        (g_FieldZoomFadeEffect.polygons + nOffset + ((g_FieldCurRenderContextIndex + 1) & 1))->b0 = color;
    }
}

// Field Fade Effect
// ---------------------------------------------------
void func_800A56A8(int duration) {
    g_FieldEffects.fades[0].b0 = 0xFF00;
    g_FieldEffects.fades[0].g0 = 0xFF00;
    g_FieldEffects.fades[0].r0 = 0xFF00;
    g_FieldEffects.fades[0].duration = duration + 1;
    g_FieldEffects.fades[0].isVisible = 1;
    g_FieldEffects.fades[0].semitransparency = 1;
    g_FieldEffects.fades[0].blueDelta = -0x10000 / duration;
    g_FieldEffects.fades[0].greenDelta = -0x10000 / duration;
    g_FieldEffects.fades[0].redDelta = -0x10000 / duration;
}

void func_800A5710(int duration) {
    g_FieldEffects.fades[0].b0 = 0x0;
    g_FieldEffects.fades[0].g0 = 0x0;
    g_FieldEffects.fades[0].r0 = 0x0;
    g_FieldEffects.fades[0].duration = duration + 1;
    g_FieldEffects.fades[0].isVisible = 1;
    g_FieldEffects.fades[0].semitransparency = 1;
    g_FieldEffects.fades[0].blueDelta = 0x10000 / duration;
    g_FieldEffects.fades[0].greenDelta = 0x10000 / duration;
    g_FieldEffects.fades[0].redDelta = 0x10000 / duration;
}
// ---------------------------------------------------

void func_800A5774(int x, int y, int h) {
    RECT rect;
    int nSize;
    int i;
    /* 15-bit VRAM words: u32 (not u_long) so each |= 0x80008000 covers exactly two
     * pixels and the stride stays 4 bytes on the 64-bit port. u32 == u_long on PSX,
     * and nSize keys off sizeof, so this is byte-identical for matching. */
    u32* pImageBuffer;
    u32* pWorkBuffer;

    rect.x = x;
    rect.y = y;
    rect.w = 0x40;
    rect.h = h;
    pImageBuffer = HeapAlloc(h * 0x80, 0x1);
    StoreImage( &rect, pImageBuffer);
    DrawSync(0);

    pWorkBuffer = pImageBuffer;
    nSize = h * (0x80 / sizeof(u32));
    for (i = 0; i < nSize; i += 8) {
        pWorkBuffer[0] |= 0x80008000;
        pWorkBuffer[1] |= 0x80008000;
        pWorkBuffer[2] |= 0x80008000;
        pWorkBuffer[3] |= 0x80008000;
        pWorkBuffer[4] |= 0x80008000;
        pWorkBuffer[5] |= 0x80008000;
        pWorkBuffer[6] |= 0x80008000;
        pWorkBuffer[7] |= 0x80008000; 
        pWorkBuffer += 8;
    }
    
    LoadImage(&rect, pImageBuffer);
    DrawSync(0);
    HeapFree(pImageBuffer);
}

// Zoom fade effect stuff
void func_800A5884(void) {
    int i;
    int nCurX;

    FieldZoomFadeEffectInitialize();
    for (i = 0; i < 2; i++) {
        FieldClearAndSwapOTag();
        FieldZoomFadeEffectUpdate();
        FieldDisplay();
    }

    nCurX = 0x2C0;
    for (i = 0; i < 5; i++) {
        func_800A5774(nCurX, 0x100, 0xE0);
        nCurX += 0x40;
    }

    for (i = 0; i < 2; i++) {
        FieldClearAndSwapOTag();
        FieldZoomFadeEffectUpdate();
        FieldDisplay();
    }
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800A5924);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800A5C40);

// Or Draw?
INCLUDE_ASM("asm/field/nonmatchings/main/misc5", FieldZoomFadeEffectUpdate);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", FieldZoomFadeEffectInitialize);

void FieldDisplay(void) {
    DrawSync(0);
    Vsync(2);
    ClearImage(&g_FieldCurRenderContext->drawEnvs[0].clip, 0x0, 0x0, 0x0);
    PutDrawEnv(&g_FieldCurRenderContext->drawEnvs[0]);
    PutDispEnv(&g_FieldCurRenderContext->dispEnv);
    DrawOTag(g_FieldCurRenderContext->ot3 + 7);
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800A6998);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800A6C40);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800A6E70);

extern void* D_800B00C4;

void func_800A7064(void) {
    HeapFree(D_800B00C4);
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800A708C);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800A7120);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800A7218);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800A732C);

void func_800A7394(void) {
    do {
        func_80077DAC();
        func_8007554C();
    } while (ArchiveDataSync() != 0 || g_FieldCurRenderContextIndex != 0);
    CdDataSync(0);
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800A73E8);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800A74F8);

extern int g_FieldPixelIndex;
/* These image-convert buffers are 32-bit words on PSX (u_long == u32 == 4 bytes
 * there). The 64-bit port widens u_long to 8 bytes, which would double the deref
 * width and pointer stride below -- overflowing pImage15Bit (0x7000 bytes, written
 * 0x1C00 times) into adjacent heap blocks. Use u32 so the stride stays 4 bytes;
 * byte-identical to u_long on the MIPS target, so matching is preserved. */
extern u32 g_FieldCurPixel;
extern u32* g_Field24BitImageData;
extern u32* g_Field15BitImageData;

u_int FieldImageConvert24BitColorTo15Bit(void) {
    u32 nPixel;
    u_int nLSB;

    // Are we done reading RGB channels?
    if (!(g_FieldPixelIndex & 3)) {
        nPixel = *g_Field24BitImageData;
        g_Field24BitImageData += 1;
        g_FieldCurPixel = nPixel;
    }
    
    g_FieldPixelIndex++;
    
    nLSB = g_FieldCurPixel & 0xFF;
    g_FieldCurPixel >>= 8;
    
    if (nLSB) {
        nLSB = nLSB >> 3;
        if (nLSB == 0)
            nLSB = 1;
    }
    
    return nLSB;
}

void FieldImageConvert24BitTo15Bit(void) {
    RECT rect;
    int i;
    int j;
    u32* pImage24Bit;
    u32* pImage15Bit;
    u32 n15BitPixels;

    pImage24Bit = HeapAlloc(0xA800, 0x0);
    pImage15Bit = HeapAlloc(0x7000, 0x0);
    
    for (i = 0; i < 5; i++) {
        rect.x = i * 0x60;
        rect.y = 0;
        rect.w = 0x60;
        rect.h = 0xE0;
        StoreImage(&rect, pImage24Bit);
        DrawSync(0);
        g_Field24BitImageData = pImage24Bit;
        g_Field15BitImageData = pImage15Bit;
        g_FieldPixelIndex = 0;
        
        for (j = 0; j < 0x1C00; j++) {
            // 15 Bit: RGBRGB
            n15BitPixels = FieldImageConvert24BitColorTo15Bit();
            n15BitPixels |= FieldImageConvert24BitColorTo15Bit() << 0x5;
            n15BitPixels |= FieldImageConvert24BitColorTo15Bit() << 0xA;
            n15BitPixels |= FieldImageConvert24BitColorTo15Bit() << 0x10;
            n15BitPixels |= FieldImageConvert24BitColorTo15Bit() << 0x15;
            n15BitPixels |= FieldImageConvert24BitColorTo15Bit() << 0x1A;
            *g_Field15BitImageData = n15BitPixels;
            g_Field15BitImageData += 1;
        }
        
        rect.x = i << 6;
        rect.y = 0x100;
        rect.w = 0x40;
        rect.h = 0xE0;
        LoadImage(&rect, pImage15Bit);
        DrawSync(0);
    }
    HeapFree(pImage24Bit);
    HeapFree(pImage15Bit);
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800A7948);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800A7C58);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800A8314);

extern s32 D_800AF278;
extern void* D_800AFC60;
extern void* D_800AFC64;

void func_800A83B4(void) {
    if (D_800AF278 != 0) {
        D_800AF278 = 0;
        DrawSync(0);
        HeapFree(D_800AFC60);
        HeapFree(D_800AFC64);
    }
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800A8408);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800A84C0);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800A8BA4);

// https://decomp.me/scratch/3xYjM
INCLUDE_ASM("asm/field/nonmatchings/main/misc5", FieldInitializeParticlePrimitive);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800A90B4);

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800A915C);

/*
Matches, but g_FieldStoredImageDest seems to be part of some struct which needs recovery first.

extern int D_800ADB34;
extern RECT g_FieldStoredImageDest[];
extern u_long* D_800AFC70;

void func_800A915C(void) {
    if (D_800ADB34 != 1) {
        D_800ADB34 = 1;
        HeapChangeCurrentUser(HEAP_USER_YOSI, NULL);
        D_800AFC70 = HeapAlloc(0x8000, 0x1);
        setRECT(&g_FieldStoredImageDest[0], 0x3C0, 0x100, 0x40, 0x100);
        StoreImage(&g_FieldStoredImageDest[0], D_800AFC70);
        DrawSync(0);
    }
}
*/


INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800A91F0);
