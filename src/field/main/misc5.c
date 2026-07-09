#include "common.h"
#include "field/main.h"
#include "field/actor.h"
#include "system/memory.h"
#include "psyq/libgpu.h"
#include "psyq/libgte.h"
#include "field/effects.h"
#include "field/graphics.h"
#ifdef XENO_PC_PORT
#include <assert.h>
#else
/* <assert.h> is unavailable under the matching build's -nostdinc MIPS
 * preprocessor. The assert(0) below marks an unimplemented path in a function
 * not yet byte-matched, so a no-op assert compiles safely there. */
#define assert(x) ((void)0)
#endif

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
void func_800A5884(int semiTrans, int abr) {
    int i;
    int nCurX;

    FieldZoomFadeEffectInitialize(semiTrans, abr);
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

extern s32 D_800ADB38;
extern s32 D_800ADB3C;
extern s16 D_800ADC08;
extern s32 D_800C3A40;
extern void FontFree(void);
extern void func_80070C84(void);
extern void func_800A915C(void);
extern void func_800A4748(void);
extern void FieldClearAndSwapOTag(void);
extern void FieldRenderSync(void);
extern void func_800A6C40(void);
extern void func_800A6E70(void);
extern void func_80077DAC(void);
extern void func_8007554C(void);
extern void func_80078B5C(void);
extern void FieldFadeToBlack(s32 duration);
extern void FieldFadeToWhite(s32 duration);
extern void func_800A91F0(void);
extern void func_80077544(void);

void func_800A5924(void) {
    s32 i;
    s32 color;

    if (D_800ADB38 == 0) {
        return;
    }

    FontFree();
    func_80070C84();
    func_800A915C();

    if (D_800ADB38 == 1 || D_800ADB38 == 4) {
        func_800A4748();
        DrawSync(0);
        FieldClearAndSwapOTag();
        FieldRenderSync();
    }

    FieldRenderSync();

    for (;;) {
        if (D_800ADB38 == 3) {
            FieldZoomFadeEffectInitialize(1, 1);
            for (i = 0; i < 5; i++) {
                func_800A5774(0x2C0 + i * 0x40, 0x100, 0xE0);
            }
            FieldFadeToWhite(D_800ADB3C);
            func_800A5600(0);

            color = 0;
            for (i = 0; i < D_800ADB3C; i++) {
                func_80077DAC();
                FieldZoomFadeEffectUpdate();
                func_8007554C();
                func_80078B5C();
                func_800A5600(color >> 16);
                color += 0x800000 / D_800ADB3C;
            }

            while (D_800ADB38 == 3) {
                func_80077DAC();
                FieldZoomFadeEffectUpdate();
                func_8007554C();
                func_80078B5C();
            }
            continue;
        }

        if (D_800ADB38 == 4) {
            func_800A5884(1, 1);
            func_800A6E70();
            D_800ADC08 = 1;
            FieldFadeToBlack(D_800ADB3C);
            D_800C3A40 = 0;

            for (i = 0; i < D_800ADB3C; i++) {
                func_80077DAC();
                func_800A6C40();
                func_8007554C();
                func_80078B5C();
                D_800C3A40 += 6;
            }

            DrawSync(0);
            func_800A7064();
        } else if (D_800ADB38 > 0 && D_800ADB38 < 4) {
            func_800A5884(1, 1);
            D_800ADC08 = 1;
            FieldFadeToBlack(D_800ADB3C);

            color = 0x800000;
            for (i = 0; i < D_800ADB3C; i++) {
                func_80077DAC();
                FieldZoomFadeEffectUpdate();
                func_8007554C();
                func_80078B5C();
                func_800A5600(color >> 16);
                color -= 0x800000 / D_800ADB3C;
                if (color < 0) {
                    color = 0;
                }
            }
        }

        D_800ADB38 = 0;
        func_800A91F0();
        func_80077544();
        return;
    }
}

extern s32 D_800B0048;
extern s32 D_800AFD14;
extern s32 D_800C2684;
extern s32 D_800ADB60;
extern void* D_800ADC14;
extern s32 D_800AFD04;
extern s32 D_8004F308;
extern s32 D_8004F324;
extern int D_8005A4C0;
extern void* D_8005A4E0;
extern void func_800864F0(void);
extern void func_800A915C(void);
extern void func_800A4748(void);
extern void func_800A90B4(s32);
extern void func_800A5710(int);
extern void func_800A56A8(int);
extern void func_80078C5C(void);
extern void FieldLoad(void);
extern void func_8007FFE8(void);
extern void FieldParticlesFreeAll(void);
extern int ArchiveDataSync(void);
extern void func_80070488(void);
extern void func_80070508(void);
extern void GamePartySyncSkinData(void);
extern void GamePartySyncStreamedData(void);
extern void FieldFadeUpdateAndDraw(void* ot, int renderContextIndex);

// FieldMain's seamless map-transition orchestrator (called once from
// main.c's after_ctx block once func_800932D0/CHANGE_FIELD has cleared
// D_800ADBEC and the stream loader is idle). Tears down the outgoing field,
// preserves the one persistent 0x8005A4C0-sized heap block across that
// teardown, then dispatches on D_800B0048 (the transition-effect selector,
// jtbl_8006FDAC) to one of 7 fade/zoom variants around the actual
// FieldLoad() call, and finally normalizes state for the next transition
// (D_800B0048=2, D_800AFD14=0x20, D_800AFD04=0).
void func_800A5C40(void) {
    void* pSaved;
    s32 savedMapMode;
    s32 savedFadeDur;
    s32 transitionMode;
    s32 runSharedFadeToBlack;
    s32 color;
    s32 i;
    RECT rect;

    FontFree();
    FieldParticlesFreeAll();
    func_800864F0();
    func_8007FFE8();

    if (D_800B0048 != 6) {
        func_800A915C();
        if (D_800B0048 != 4) {
            func_800A4748();
        }
    }

    DrawSync(0);
    FieldClearAndSwapOTag();
    FieldRenderSync();
    FieldFree();

    pSaved = HeapAlloc(D_8005A4C0, 0);
    memcpy(pSaved, D_8005A4E0, D_8005A4C0);
    HeapUnpinBlock(D_8005A4E0);
    HeapFree(D_8005A4E0);

    if (D_800B0048 != 6) {
        func_800A90B4(1);
    }

    D_8005A4E0 = HeapAlloc(D_8005A4C0, 1);
    memcpy(D_8005A4E0, pSaved, D_8005A4C0);
    HeapPinBlock(D_8005A4E0);
    HeapFree(pSaved);

    transitionMode = D_800B0048;
    runSharedFadeToBlack = 0;

    if ((u32)transitionMode < 7) {
        switch (transitionMode) {
        case 6:
            FieldFadeToWhite(D_800AFD14);
            for (i = 0; i < D_800AFD14; i++) {
                FieldClearAndSwapOTag();
                FieldFadeUpdateAndDraw((u8*)g_FieldCurRenderContext + 0x80D4, g_FieldCurRenderContextIndex);
                FieldDisplay();
            }
            runSharedFadeToBlack = 1;
            break;

        case 0:
            FieldZoomFadeEffectInitialize(0, 0);
            FieldFadeToWhite(D_800AFD14);
            for (i = 0; i < D_800AFD14; i++) {
                FieldClearAndSwapOTag();
                FieldFadeUpdateAndDraw((u8*)g_FieldCurRenderContext + 0x80D4, g_FieldCurRenderContextIndex);
                FieldZoomFadeEffectUpdate();
                FieldDisplay();
            }
            runSharedFadeToBlack = 1;
            break;

        case 1:
            FieldZoomFadeEffectInitialize(0, 0);
            func_800A5710(D_800AFD14);
            for (i = 0; i < D_800AFD14; i++) {
                FieldClearAndSwapOTag();
                FieldFadeUpdateAndDraw((u8*)g_FieldCurRenderContext + 0x80D4, g_FieldCurRenderContextIndex);
                FieldZoomFadeEffectUpdate();
                FieldDisplay();
            }
            FieldRenderSync();
            GamePartySyncSkinData();
            GamePartySyncStreamedData();
            savedMapMode = D_800B0048;
            savedFadeDur = D_800AFD14;
            FieldLoad();
            func_80070488();
            func_80070508();
            D_800B0048 = savedMapMode;
            D_800AFD14 = savedFadeDur;
            if (D_8004F308 == -1) {
                func_80085B20(D_8004F324, 0);
            }
            func_800A56A8(D_800AFD14);
            break;

        case 2:
        case 4:
            func_800A5884(1, 1);
            GamePartySyncSkinData();
            GamePartySyncStreamedData();
            savedMapMode = D_800B0048;
            savedFadeDur = D_800AFD14;
            FieldLoad();
            func_80070488();
            if (D_800ADB60 == 1) {
                while (ArchiveDataSync() != 0) {
                    FieldClearAndSwapOTag();
                    FieldZoomFadeEffectUpdate();
                    FieldDisplay();
                    if (D_800C2684 < 0x22C0) {
                        D_800C2684 += 0x20;
                    }
                }
                HeapFree(D_800ADC14);
                D_800ADB60 = 0;
                func_80078C5C();
            }
            D_800AFD04 = 1;
            D_800B0048 = savedMapMode;
            D_800AFD14 = savedFadeDur;
            if (D_8004F308 == -1) {
                func_80085B20(D_8004F324, 0);
            }
            FieldFadeToBlack(D_800AFD14);
            if (D_800AFD14 > 0) {
                color = 0x800000;
                for (i = 0; i < D_800AFD14; i++) {
                    func_80077DAC();
                    FieldZoomFadeEffectUpdate();
                    func_8007554C();
                    func_80078B5C();
                    func_800A5600(color >> 16);
                    color -= 0x800000 / D_800AFD14;
                    if (color < 0) {
                        color = 0;
                    }
                    if (D_800C2684 < 0x22C0) {
                        D_800C2684 += 0x20;
                    }
                }
            }
            break;

        case 3:
            FieldZoomFadeEffectInitialize(0, 0);
            func_80070488();
            FieldClearAndSwapOTag();
            FieldZoomFadeEffectUpdate();
            FieldDisplay();
            GamePartySyncSkinData();
            GamePartySyncStreamedData();
            savedMapMode = D_800B0048;
            savedFadeDur = D_800AFD14;
            D_800AFD04 = 1;
            FieldLoad();
            func_80070508();
            D_800B0048 = savedMapMode;
            D_800AFD14 = savedFadeDur;
            if (D_8004F308 == -1) {
                func_80085B20(D_8004F324, 0);
            }
            for (i = 0; i < 4; i++) {
                func_80077DAC();
                FieldZoomFadeEffectUpdate();
                func_8007554C();
                func_80078B5C();
            }
            break;

        case 5:
            FieldZoomFadeEffectInitialize(0, 0);
            func_80070488();
            FieldClearAndSwapOTag();
            FieldZoomFadeEffectUpdate();
            FieldDisplay();
            GamePartySyncSkinData();
            GamePartySyncStreamedData();
            savedMapMode = D_800B0048;
            savedFadeDur = D_800AFD14;
            D_800AFD04 = 1;
            FieldLoad();
            func_80070508();
            rect.x = 0x2C0;
            rect.y = 0x100;
            rect.w = 0x140;
            rect.h = 0xFF;
            D_800B0048 = savedMapMode;
            D_800AFD14 = savedFadeDur;
            MoveImage(&rect, 0x140, 0xFF);
            if (D_8004F308 == -1) {
                func_80085B20(D_8004F324, 0);
            }
            for (i = 0; i < 4; i++) {
                func_80077DAC();
                FieldZoomFadeEffectUpdate();
                func_8007554C();
                func_80078B5C();
            }
            break;
        }

        if (runSharedFadeToBlack) {
            FieldClearAndSwapOTag();
            FieldDisplay();
            GamePartySyncSkinData();
            GamePartySyncStreamedData();
            savedMapMode = D_800B0048;
            savedFadeDur = D_800AFD14;
            FieldLoad();
            func_80070488();
            func_80070508();
            D_800B0048 = savedMapMode;
            D_800AFD14 = savedFadeDur;
            if (D_8004F308 == -1) {
                func_80085B20(D_8004F324, 0);
            }
            FieldFadeToBlack(D_800AFD14);
        }
    }

    if (D_800B0048 != 6) {
        func_800A91F0();
    }
    D_800B0048 = 2;
    D_800AFD14 = 0x20;
    D_800AFD04 = 0;
    func_80077544();
    HeapConsolidate();
}

extern s32 D_800C2684;
extern SVECTOR D_800B00B8;

// Or Draw?
void FieldZoomFadeEffectUpdate(void) {
    MATRIX matrix;
    VECTOR scale;
    int interpolation;
    int flag;
    int i;

    RotMatrix(&D_800B00B8, &matrix);
    scale.vx = D_800C2684;
    scale.vy = D_800C2684;
    scale.vz = D_800C2684;
    ScaleMatrix(&matrix, &scale);
    SetRotMatrix(&matrix);
    SetTransMatrix(&matrix);

    for (i = 0; i < 5; i++) {
        POLY_FT4* poly = &g_FieldZoomFadeEffect.polygons[(i * 2) + g_FieldCurRenderContextIndex];
        DR_MODE* drMode = &g_FieldZoomFadeEffect.drawModes[(i * 2) + g_FieldCurRenderContextIndex];

        if (D_800C2684 != 0x1000) {
            SVECTOR* vertices = &g_FieldZoomFadeEffect.vectors[i * 4];
            RotAverage4(
                &vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (long*)&poly->x0, (long*)&poly->x1, (long*)&poly->x2, (long*)&poly->x3,
                (long*)&interpolation, (long*)&flag
            );
        }

        addPrim(g_FieldCurRenderContext->ot3, poly);
        addPrim(g_FieldCurRenderContext->ot3, drMode);
    }
}

void FieldZoomFadeEffectInitialize(int semiTrans, int abr) {
    int i;
    int tpageX;
    int x0;
    int x1;
    int vx0;
    int vx1;

    D_800C2684 = 0x1000;
    D_800B00B8.vx = 0;
    D_800B00B8.vy = 0;
    D_800B00B8.vz = 0;

    tpageX = 0x2C0;
    x0 = 0;
    x1 = 0x40;
    vx0 = -0x50;
    vx1 = -0x30;

    for (i = 0; i < 5; i++) {
        POLY_FT4* poly0 = &g_FieldZoomFadeEffect.polygons[i * 2];
        POLY_FT4* poly1 = &g_FieldZoomFadeEffect.polygons[(i * 2) + 1];
        RECT* rect0 = &g_FieldZoomFadeEffect.rects[i * 2];
        RECT* rect1 = &g_FieldZoomFadeEffect.rects[(i * 2) + 1];
        u_short tpage = GetTPage(2, abr, tpageX, 0x100);
        SVECTOR* vertices = &g_FieldZoomFadeEffect.vectors[i * 4];

        SetPolyFT4(poly0);
        setRGB0(poly0, 0x80, 0x80, 0x80);
        setXY4(poly0, x0, 0, x1, 0, x0, 0xDF, x1, 0xDF);
        setUV4(poly0, 0, 0, 0x40, 0, 0, 0xDF, 0x40, 0xDF);
        poly0->tpage = tpage;
        SetSemiTrans(poly0, semiTrans);

        vertices[0].vx = vx0;
        vertices[0].vy = -0x38;
        vertices[0].vz = 0;
        vertices[1].vx = vx1;
        vertices[1].vy = -0x38;
        vertices[1].vz = 0;
        vertices[2].vx = vx0;
        vertices[2].vy = 0x38;
        vertices[2].vz = 0;
        vertices[3].vx = vx1;
        vertices[3].vy = 0x38;
        vertices[3].vz = 0;

        rect0->x = 0;
        rect0->y = 0;
        rect0->w = 0xFF;
        rect0->h = 0xFF;
        rect1->x = 0;
        rect1->y = 0;
        rect1->w = 0xFF;
        rect1->h = 0xFF;

        SetDrawMode(&g_FieldZoomFadeEffect.drawModes[i * 2], 0, 0, tpage, rect0);
        SetDrawMode(&g_FieldZoomFadeEffect.drawModes[(i * 2) + 1], 0, 0, tpage, rect1);

        memcpy(poly1, poly0, sizeof(POLY_FT4));

        tpageX += 0x40;
        x0 += 0x40;
        x1 += 0x40;
        vx0 += 0x20;
        vx1 += 0x20;
    }
}

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

void func_800A84C0(void) {
    if (D_800AF278 == 0) {
        return;
    }

    assert(0 && "func_800A84C0 active field overlay path is not migrated");
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc5", func_800A8BA4);

// https://decomp.me/scratch/3xYjM
INCLUDE_ASM("asm/field/nonmatchings/main/misc5", FieldInitializeParticlePrimitive);

extern s32 D_800ADB34;
extern u32 D_800AFC70;

// Re-pin the 0x8000-byte persistent buffer under a new heap user (only while
// D_800ADB34 marks it active, e.g. a pause-menu background snapshot): alloc
// a fresh block under HEAP_USER_YOSI, copy the old contents over, free the old.
void func_800A90B4(s32 pinFlag) {
    void* pNew;

    if (D_800ADB34 == 1) {
        HeapChangeCurrentUser(HEAP_USER_YOSI, NULL);
        pNew = HeapAlloc(0x8000, pinFlag);
        memcpy(pNew, (void*)(uintptr_t)D_800AFC70, 0x8000);
        HeapFree((void*)(uintptr_t)D_800AFC70);
        D_800AFC70 = (u32)(uintptr_t)pNew;
    }
}

/* Matches, but g_FieldStoredImageDest seems to be part of some struct which
 * needs recovery first. */

extern int D_800ADB34;
extern RECT g_FieldStoredImageDest[];
extern u32 D_800AFC70;

void func_800A915C(void) {
    if (D_800ADB34 != 1) {
        D_800ADB34 = 1;
        HeapChangeCurrentUser(HEAP_USER_YOSI, NULL);
        D_800AFC70 = (u32)(uintptr_t)HeapAlloc(0x8000, 0x1);
        setRECT(&g_FieldStoredImageDest[0], 0x3C0, 0x100, 0x40, 0x100);
        StoreImage(&g_FieldStoredImageDest[0], (u_long*)(uintptr_t)D_800AFC70);
        DrawSync(0);
    }
}


void func_800A91F0(void) {
    if (D_800ADB34) {
        setRECT(&g_FieldStoredImageDest[0], 0x3C0, 0x100, 0x40, 0x100);
        D_800ADB34 = 0;
        LoadImage(&g_FieldStoredImageDest[0], (u_long*)(uintptr_t)D_800AFC70);
        DrawSync(0);
        HeapFree((void*)(uintptr_t)D_800AFC70);
    }
}
