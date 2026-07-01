#include "common.h"
#include "field/actor.h"
#include "field/main.h"
#include "field/text_box.h"
#include "field/effects.h"
#include "system/memory.h"
#include "system/archive.h"
#include "system/sound.h"
#include "psyq/libetc.h"
#include "psyq/libcd.h"

/* Per-frame helper: rand seed, map-load check, angle-step timer. */
extern s32 D_8004F308, D_8004F324, D_800ADC18;
extern int func_80085C90(int);
void func_80078B5C(void) {
    rand();
    if (D_8004F308 == -1) {
        D_8004F308 = func_80085C90(D_8004F324);
    }
    if (D_800ADC18 != 0) {
        D_800ADC18--;
    }
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc4", func_80078BC8);

extern s16 D_800B2344;
void func_80078C5C(void) {
    RECT rect;
    int i;
    u_int* pImage;

    if (D_800B2344 != 0) {
        g_FieldRenderContexts[0].drawEnvs[0].dtd = 0;
        g_FieldRenderContexts[1].drawEnvs[0].dtd = 0;
        pImage = HeapAlloc(0x4000, 0x0);
        rect.y = 0x1E0;
        rect.w = 0x100;
        rect.x = 0;
        rect.h = 0x20;
        StoreImage(&rect, pImage);
        DrawSync(0);

        // Process image before storing it back in VRAM
        for (i = 0; i < 0x1000; i++) {
            if (pImage[i] & 0xFFFF) {
                pImage[i] |= 0xC63;
            }
            if (pImage[i] & 0xFFFF0000) {
                pImage[i] |= 0x0C630000;
            }
        }
        
        LoadImage(&rect, pImage);
        DrawSync(0);
        HeapFree(pImage);
    }
}

/* ---- func_80078D44: field-init / initial map-load orchestrator --------------
 * Functional decompile (port-first; control flow mirrors the asm). FieldMain's
 * setup calls this: it loads the map file (func_800777DC -> func_8001B484), brings
 * up the render contexts + VRAM, sets g_FieldCurRenderContextIndex=1, then parses
 * the map via FieldLoad (which allocates g_FieldActors). The fade/zoom loops play a
 * transition; in the port their effect calls stub out and ArchiveDataSync returns
 * 0, so the loops just iterate their fixed counts. */
extern s32 D_8004F2F8, D_8004F304, D_8004F308, D_8004F310, D_8004F324, D_8004F338;
extern s32 D_8004F2FC, D_8004F348, D_800ADB60, D_800C2684, D_800B2264, D_800AFD04;
extern s32 g_GamePartySkinsInitialized;
extern u8 D_800594D0, D_8005942C;
extern void *D_800ADC14, *D_80062528, *g_GameCurLoadedWDS;
extern void func_80077544(), func_800A915C(), func_800777DC(), func_800A4748();
extern void func_800A476C(), func_800A5884(), func_80070488(), func_801E7378();
extern void func_800A5600(), func_80039C4C(), func_800399D4(), func_800A24C4();
extern void func_8001B66C(), func_80085B20(), func_80085EEC(), func_800A31E8();
extern void func_800A91F0(), func_80077DAC(), func_80078B5C(), func_8007554C();

void func_80078D44(void) {
    RECT rect;
    s32 s0, s1, s2;

    func_80077544();
    func_800A915C();
    ArchiveSetIndex(4, 0);
    func_800777DC();              /* load the map file into D_8005A4E0 */
    FieldRenderSync();
    if (D_8004F2F8 == 0) {
        FieldImageConvert24BitTo15Bit(0);
    }
    FieldInitializeRenderContexts();
    s0 = 1;
    if (D_8004F2F8 == 0) {
        rect.x = 0; rect.y = 0x100; rect.w = 0x140; rect.h = 0xE0;
        MoveImage(&rect, 0, 0);
        s0 = 1;
    }
    g_FieldCurRenderContextIndex = s0;
    func_800A4748();
    func_800A476C(0, 0x100);
    DrawSync(0);
    FieldClearAndSwapOTag();
    FieldRenderSync();
    if (D_800594D0 == 1) {
        func_800A5884(0, 0);
    } else if (D_8005942C != 1) {
        func_800A5884(1, 1);
    } else {
        func_800A5884(0, 0);
    }
    D_8004F2F8 = 1;
    ArchiveCdDataSync(0);
    ArchiveSetIndex(4, 0);
    FieldLoad();                  /* parse map -> allocates g_FieldActors */
    func_80070488();
    D_800AFD04 = 1;
    if (D_800B2264 != 0) {
#ifndef XENO_PC_PORT
        func_801E7378(1);
#endif
    }
    if (D_800594D0 == 1) {
        s2 = 0;
    } else if (D_8005942C != 1) {
        s2 = 0x20;
    } else {
        s2 = 0;
    }

    s0 = 0x800000;
    if (D_800ADB60 == 1) {
        do {
            FieldClearAndSwapOTag();
            FieldZoomFadeEffectUpdate();
            FieldDisplay();
            if (D_8005942C == 1) {
                func_800A5600(s0 >> 16);
                s0 -= 0x40000;
                if (s0 < 0) s0 = 0;
            } else if (D_800C2684 < 0x22C0) {
                D_800C2684 += s2;
            }
        } while (ArchiveDataSync() != 0);
        DrawSync(0);
        HeapFree(D_800ADC14);
        D_800ADB60 = 0;
        func_80078C5C();
    }

    if (D_8005942C == 1) {
        do {
            FieldClearAndSwapOTag();
            FieldZoomFadeEffectUpdate();
            FieldDisplay();
            func_800A5600(s0 >> 16);
            s0 -= 0x40000;
        } while (s0 >= 0);
    }

    if (D_800594D0 == 1) {
        rect.x = 0; rect.y = 0; rect.w = 0x140; rect.h = 0xE0;
        MoveImage(&rect, 0x200, 0);
    }
    if (D_8004F304 != 0) {
        func_80039C4C(D_80062528);
        func_800399D4(D_80062528);
        SoundFreeWdsEntry(g_GameCurLoadedWDS);
        D_8004F304 = 0;
    }
    func_800A24C4();
    D_8004F310 = 0;
    g_GamePartySkinsInitialized = 0;
    FieldRenderSync();
    ControllerResetState();
    D_8004F308 = 0;
    if (D_800594D0 == 1) {
        D_8004F324 = 0xE;
        func_80085EEC();
    }
    if (D_8004F338 != D_8004F324) {
        func_8001B66C();
        D_8004F308 = -1;
        if (D_8004F2FC != 0) {
            D_8004F348 = 1;
        }
        func_80085B20(D_8004F324, 1);
    } else {
        func_80085EEC();
    }
    func_800A31E8();

    if (D_800594D0 == 1) {
        s1 = 0;
        do {
            func_80077DAC();
            s1++;
            FieldZoomFadeEffectUpdate();
            func_8007554C();
            func_80078B5C();
        } while (s1 < 8);
    } else if (D_8005942C == 1) {
        FieldFadeToBlack(0x20);
    } else {
        FieldFadeToBlack(0x20);
        s0 = 0x800000;
        s1 = 0;
        do {
            func_80077DAC();
            FieldZoomFadeEffectUpdate();
            func_8007554C();
            func_80078B5C();
            if (D_800594D0 != 1) {
                func_800A5600(s0 >> 16);
                s0 -= 0x40000;
                if (s0 < 0) s0 = 0;
                if (D_800C2684 < 0x22C0) {
                    D_800C2684 += s2;
                }
            }
            s1++;
        } while (s1 < 0x20);
    }

    if (D_800B2264 != 0) {
#ifndef XENO_PC_PORT
        func_801E7378(0);
#endif
    }
    func_800A91F0();
    HeapConsolidate();
    FontFree();
    func_80077544();
    D_800AFD04 = 0;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc4", func_80079288);

INCLUDE_ASM("asm/field/nonmatchings/main/misc4", func_8007954C);

void func_800796F4(void) {}

void FieldSwapRenderContext(void) {
    g_FieldCurRenderContextIndex = (g_FieldCurRenderContextIndex + 1) % 2;
    g_FieldCurRenderContext = &g_FieldRenderContexts[g_FieldCurRenderContextIndex];
    PutDispEnv(&g_FieldCurRenderContext->dispEnv);
    PutDrawEnv(&g_FieldCurRenderContext->drawEnvs[0]);
}

extern RECT D_800AFE4C;
extern TILE D_800AFE54[];
extern DR_MODE D_800AFE24[];
	
void func_80079784(int color) {
    FieldClearAndSwapOTag();
    setRGB(D_800AFE54[g_FieldCurRenderContextIndex], color * 4);
    addPrim(g_FieldCurRenderContext->ot1, &D_800AFE54[g_FieldCurRenderContextIndex]);
    addPrim(g_FieldCurRenderContext->ot1, &D_800AFE24[g_FieldCurRenderContextIndex]);
    FieldRenderSync();
    MoveImage(&D_800AFE4C, 0, g_FieldCurRenderContextIndex * 0x100);
    PutDispEnv(&g_FieldCurRenderContext->dispEnv);
    PutDrawEnv(&g_FieldCurRenderContext->drawEnvs[0]);
    DrawOTag(g_FieldCurRenderContext->ot1 + 1);
}


INCLUDE_ASM("asm/field/nonmatchings/main/misc4", func_800798BC);

void func_8007995C(short w, short h, short x, short y, int destX, int destY) {
    RECT rect;

    rect.w = w;
    rect.h = h;
    rect.x = x;
    rect.y = y;
    MoveImage(&rect, destX, destY);
    DrawSync(0);
}

void FieldRenderSyncAndFlush(void) {
    FieldRenderSync();
    EnterCriticalSection();
    FlushCache();
    ExitCriticalSection();
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc4", func_800799D4);


// Quad rendering
// --------------------
void FieldClampPolyFT4UVs(POLY_FT4* poly, short u0, short v0, short u1, short v1, short u2, short v2, short u3, short v3) {
    if (u0 < 0) u0 = 0;
    if (u1 < 0) u1 = 0;
    if (u2 < 0) u2 = 0;
    if (u3 < 0) u3 = 0;
    if (v0 < 0) v0 = 0;
    if (v1 < 0) v1 = 0;
    if (v2 < 0) v2 = 0;
    if (v3 < 0) v3 = 0;
    
    if (u0 >= 0x100) u0 = 0xFF;
    if (u1 >= 0x100) u1 = 0xFF;
    if (u2 >= 0x100) u2 = 0xFF;
    if (u3 >= 0x100) u3 = 0xFF;
    if (v0 >= 0x100) v0 = 0xFF;
    if (v1 >= 0x100) v1 = 0xFF;
    if (v2 >= 0x100) v2 = 0xFF;
    if (v3 >= 0x100) v3 = 0xFF;

    poly->u0 = u0;
    poly->v0 = v0;
    poly->u1 = u1;
    poly->v1 = v1;
    poly->u2 = u2;
    poly->v2 = v2;
    poly->u3 = u3;
    poly->v3 = v3;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc4", func_8007A5C4);

extern SVec4 D_800ADCE0[]; // X0 -> X3 positions
extern SVec4 D_800ADD28[]; // Y0 -> Y3 positions
extern SVec4 D_800ADD70[]; // U0 -> U3s
extern SVec4 D_800ADDB8[]; // V0 -> V3s

// Texture / Clut stuff
extern short D_800ADE00[];
extern short D_800ADE02[];
extern short D_800ADE04[];
extern short D_800ADE06[];
extern short D_800ADE08[];
extern short D_800ADE0A[];

// Compass primitive initialization?
void func_8007A7F4(Quad* pPart, int x, int y, int tex) {
    int nIndexTex;
    POLY_FT4* pNextPoly;
    char _pad[0x88];

    pNextPoly = &pPart->polys[1]; 
    SetPolyFT4(&pPart->polys[0]);
    
    nIndexTex = tex * 0x6,

    // Set Position 0, 1, 2, and 3
    pPart->vertices[0].vx = D_800ADCE0[x].x;
    pPart->vertices[0].vy = 0x0;
    pPart->vertices[0].vz = D_800ADD28[y].x;

    pPart->vertices[1].vx = D_800ADCE0[x].y;
    pPart->vertices[1].vy = 0;
    pPart->vertices[1].vz = D_800ADD28[y].y;

    pPart->vertices[2].vx = D_800ADCE0[x].z;
    pPart->vertices[2].vy = 0;
    pPart->vertices[2].vz = D_800ADD28[y].z;

    pPart->vertices[3].vx = D_800ADCE0[x].w;
    pPart->vertices[3].vy = 0;
    pPart->vertices[3].vz = D_800ADD28[y].w;
    
    setRGB0(&pPart->polys[0], 0x80, 0x80, 0x80);
    
    pPart->polys[0].tpage = GetTPage(
        D_800ADE00[nIndexTex], 
        D_800ADE02[nIndexTex], 
        D_800ADE04[nIndexTex], 
        D_800ADE06[nIndexTex]
    );
    
    pPart->polys[0].clut = GetClut(D_800ADE08[nIndexTex], D_800ADE0A[nIndexTex]);
    
    FieldClampPolyFT4UVs(&pPart->polys[0], 
        D_800ADD70[x].x, D_800ADDB8[y].x, 
        D_800ADD70[x].y, D_800ADDB8[y].y, 
        D_800ADD70[x].z, D_800ADDB8[y].z, 
        D_800ADD70[x].w, D_800ADDB8[y].w
    );

    *pNextPoly = pPart->polys[0];
}

// Actor primitive initialization
void func_8007AA44(Quad* pQuad) {
    POLY_FT4* pPoly;
    POLY_FT4* pPoly2;

    pPoly = &pQuad->polys[0];
    pPoly2 = &pQuad->polys[1];
    
    SetPolyFT4(pPoly);
    pQuad->vertices[3].vx = -0x18;
    pQuad->vertices[3].vz = -0x18;
    pQuad->vertices[3].vy = 0;

    pQuad->vertices[2].vx = 0x18;
    pQuad->vertices[2].vy = 0;
    pQuad->vertices[2].vz = -0x18;

    pQuad->vertices[1].vx = -0x18;
    pQuad->vertices[1].vy = 0;
    pQuad->vertices[1].vz = 0x18;

    pQuad->vertices[0].vx = 0x18;
    pQuad->vertices[0].vy = 0;
    pQuad->vertices[0].vz = 0x18;

    setRGB0(pPoly, 0x80, 0x80, 0x80);
    pPoly->tpage = GetTPage(0, 2, 0x280, 0x1E0);
    pPoly->clut = GetClut(0x100, 0xF3);
    SetSemiTrans(pPoly, 1);

    pPoly->v0 = 0xE0;
    pPoly->v1 = 0xE0;
    pPoly->u0 = 0;
    pPoly->u1 = 0xF;
    pPoly->u2 = 0;
    pPoly->v2 = 0xEF;
    pPoly->u3 = 0xF;
    pPoly->v3 = 0xEF;
    
    *pPoly2 = *pPoly;
}

void FieldRenderQuad(u_long* ot, Quad* pQuad, MATRIX* pMatTransform, int renderContext) {
    long nInterpolation;
    long nFlag;
    POLY_FT4* pPoly;

    pPoly = &pQuad->polys[renderContext];
    PushMatrix();

    // Set our GTE transform matrix (RTM and TRV)
    SetRotMatrix(pMatTransform);
    SetTransMatrix(pMatTransform);

    // Transform our vertices to screen coordinates
    RotAverage4(
        &pQuad->vertices[0], &pQuad->vertices[1], &pQuad->vertices[2], &pQuad->vertices[3], 
        (long*)&pPoly->x0, (long*)&pPoly->x1, (long*)&pPoly->x2, (long*)&pPoly->x3, 
        &nInterpolation, &nFlag
    );

    // Queue our primitive for rendering
    addPrim(ot + 1, pPoly);

    PopMatrix();
}

void func_8007AC58(u_long* ot, Quad* pQuad, MATRIX* pMatTransform, int renderContext) {
    int nInterpolation;
    int nFlag;
    int nPosY;
    int nPosX1;
    POLY_FT4* pPoly;
    int nPosX2;

    pPoly = &pQuad->polys[renderContext];
    PushMatrix();

    // Set our GTE transform matrix (RTM and TRV)
    SetRotMatrix(pMatTransform);
    SetTransMatrix(pMatTransform);

    // Transform our vertices to screen coordinates
    RotAverage4(
        &pQuad->vertices[0], &pQuad->vertices[1], &pQuad->vertices[2], &pQuad->vertices[3], 
        (long*)&pPoly->x0, (long*)&pPoly->x1, (long*)&pPoly->x2, (long*)&pPoly->x3, 
        &nInterpolation, &nFlag
    );
    
    nPosX1 = (pPoly->x3 + pPoly->x2) / 2;
    nPosY = pPoly->y3;

    nPosX2 = nPosX1 + 8;
    nPosX1 = nPosX1 - 8;
    setXY4(pPoly,
        nPosX1, nPosY - 10,
        nPosX2, nPosY - 10,
        nPosX1, nPosY,
        nPosX2, nPosY
    );
    
    // Queue our primitive for rendering
    addPrim(ot + 1, pPoly);

    PopMatrix();
}


// Controller stuff
// --------------------
extern s32 g_pFieldControllerBuffer1;
extern s32 g_pFieldControllerBuffer2;

extern u_short g_FieldMouseSpeedX;
extern u_short g_FieldMouseSpeedY;
extern s32 D_800C3A44;
extern s32 D_800C3A4C;
extern s32 D_800C3A50;
extern s32 D_800C3A54;
extern int g_FieldMousePositionsX[2];
extern int g_FieldMousePositionsY[2];

void FieldSetControllerBuffers(void* controllerBuffer1, void* controllerBuffer2) {
    g_pFieldControllerBuffer1 = controllerBuffer1;
    g_pFieldControllerBuffer2 = controllerBuffer2;
}

void func_8007ADA4(int arg0, int arg1, int arg2, int arg3) {
    D_800C3A44 = arg0 * g_FieldMouseSpeedX;
    D_800C3A50 = arg1 * g_FieldMouseSpeedX;
    D_800C3A4C = arg2 * g_FieldMouseSpeedY;
    D_800C3A54 = arg3 * g_FieldMouseSpeedY;
}

void FieldSetMouseSpeed(u_short xSpeed, u_short ySpeed) {
    g_FieldMouseSpeedX = xSpeed;
    g_FieldMouseSpeedY = ySpeed;
}

void FieldSetMousePosition(int mouseIndex, int xMovement, int yMovement) {
    g_FieldMousePositionsX[mouseIndex] = xMovement * g_FieldMouseSpeedX;
    g_FieldMousePositionsY[mouseIndex] = yMovement * g_FieldMouseSpeedY;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc4", func_8007AE78);

INCLUDE_ASM("asm/field/nonmatchings/main/misc4", func_8007AF74);

INCLUDE_ASM("asm/field/nonmatchings/main/misc4", func_8007B07C);

INCLUDE_ASM("asm/field/nonmatchings/main/misc4", func_8007B1C4);

INCLUDE_ASM("asm/field/nonmatchings/main/misc4", func_8007B478);

INCLUDE_ASM("asm/field/nonmatchings/main/misc4", func_8007B614);

s32 func_8007B694(s32* arg0) {
    return -ratan2(arg0[2], arg0[0]) & 0xFFF;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc4", func_8007B6C4);

INCLUDE_ASM("asm/field/nonmatchings/main/misc4", func_8007B814);

INCLUDE_ASM("asm/field/nonmatchings/main/misc4", func_8007BAC0);

INCLUDE_ASM("asm/field/nonmatchings/main/misc4", func_8007BEF4);

INCLUDE_ASM("asm/field/nonmatchings/main/misc4", func_8007C670);

INCLUDE_ASM("asm/field/nonmatchings/main/misc4", func_8007C694);

extern s32 D_800ADC10;

void* func_8007CD3C(s32 a0) {
    s32 old = D_800ADC10;
    D_800ADC10 = old + a0;
    return (void*)((old << 2) + 0x1F800000);
}

void func_8007CD60(s32 a0) {
    D_800ADC10 -= a0;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc4", func_8007CD80);

INCLUDE_ASM("asm/field/nonmatchings/main/misc4", func_8007D3D4);

INCLUDE_ASM("asm/field/nonmatchings/main/misc4", func_8007D818);

INCLUDE_ASM("asm/field/nonmatchings/main/misc4", func_8007D8B4);




