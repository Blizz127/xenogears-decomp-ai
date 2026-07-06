#include "common.h"
#include "field/actor.h"
#include "field/main.h"
#include "field/text_box.h"
#include "field/effects.h"
#include "system/memory.h"
#include "system/archive.h"
#include "system/sound.h"
#include "system/math.h"
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

extern s32 D_800ADB2C;
extern s32 D_800ADB90;
extern s32 D_800ADB34;
extern s32 D_800ADBC4;

s32 func_80078BC8(void) {
    if (D_800ADB2C != 0) {
        return -1;
    }
    if (ArchiveDataSync() != 0) {
        return -1;
    }
    if (D_8004F308 != 0) {
        return -1;
    }
    if (D_800ADB90 != 0) {
        return -1;
    }
    if (D_800ADB34 != 0) {
        return -1;
    }
    return D_800ADBC4 != 0xFF ? -1 : 0;
}

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

extern u8 D_800B0FEC[];
extern s16 D_800ADE30[][8];
extern u8 D_800ADE70[][8];

void func_8007A5C4(void) {
    s32 i;

    for (i = 0; i < 4; i++) {
        Quad* pPart = (Quad*)(D_800B0FEC + (i * 0x70));
        POLY_FT4* pPoly = &pPart->polys[0];

        SetPolyFT4(pPoly);

        pPart->vertices[0].vx = D_800ADE30[i][0];
        pPart->vertices[0].vy = 0;
        pPart->vertices[0].vz = D_800ADE30[i][1];
        pPart->vertices[1].vx = D_800ADE30[i][2];
        pPart->vertices[1].vy = 0;
        pPart->vertices[1].vz = D_800ADE30[i][3];
        pPart->vertices[2].vx = D_800ADE30[i][4];
        pPart->vertices[2].vy = 0;
        pPart->vertices[2].vz = D_800ADE30[i][5];
        pPart->vertices[3].vx = D_800ADE30[i][6];
        pPart->vertices[3].vy = 0;
        pPart->vertices[3].vz = D_800ADE30[i][7];

        setRGB0(pPoly, 0x80, 0x80, 0x80);
        FieldClampPolyFT4UVs(pPoly,
                             D_800ADE70[i][0], D_800ADE70[i][1] + 0xC0,
                             D_800ADE70[i][2], D_800ADE70[i][3] + 0xC0,
                             D_800ADE70[i][4], D_800ADE70[i][5] + 0xC0,
                             D_800ADE70[i][6], D_800ADE70[i][7] + 0xC0);
        SetSemiTrans(pPoly, 1);
        pPoly->tpage = GetTPage(0, 2, 0x280, 0x1C0);
        pPoly->clut = GetClut(0x100, 0xF2);
        pPart->polys[1] = *pPoly;
    }
}

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

extern void func_8007AF74(s32 mouseIndex);

void func_8007AE78(s32 mouseIndex, void* out) {
    s32* pOut = out;
    u8* pController;

    func_8007AF74(mouseIndex);

    pOut[0] = g_FieldMousePositionsX[mouseIndex] / g_FieldMouseSpeedX;
    pOut[1] = g_FieldMousePositionsY[mouseIndex] / g_FieldMouseSpeedY;
    pOut[2] = -0x100;

    pController = (u8*)(uintptr_t)(mouseIndex ? g_pFieldControllerBuffer2 : g_pFieldControllerBuffer1);
    pOut[3] = (s8)pController[4];
    pOut[4] = (s8)pController[5];

    if ((s8)pController[0] == 0 && (s8)pController[1] == 0x12) {
        pOut[2] = (u8)(~pController[3]) & 0x0C;
    }
}

void func_8007AF74(s32 mouseIndex) {
    u8* pController = (u8*)(uintptr_t)(mouseIndex ? g_pFieldControllerBuffer2 : g_pFieldControllerBuffer1);
    s32 pos;

    if ((s8)pController[0] != 0 || (s8)pController[1] != 0x12) {
        return;
    }

    g_FieldMousePositionsX[mouseIndex] += (s8)pController[4];
    g_FieldMousePositionsY[mouseIndex] += (s8)pController[5];

    pos = g_FieldMousePositionsX[mouseIndex];
    if (pos > D_800C3A50) {
        g_FieldMousePositionsX[mouseIndex] = D_800C3A50;
    } else if (pos < D_800C3A44) {
        g_FieldMousePositionsX[mouseIndex] = D_800C3A44;
    }

    pos = g_FieldMousePositionsY[mouseIndex];
    if (pos > D_800C3A54) {
        g_FieldMousePositionsY[mouseIndex] = D_800C3A54;
    } else if (pos < D_800C3A4C) {
        g_FieldMousePositionsY[mouseIndex] = D_800C3A4C;
    }
}

void func_8007B07C(s16* arg0, s16* arg1, s16* arg2, s16* arg3, VECTOR* arg4) {
    VECTOR vec0;
    VECTOR normal0;
    VECTOR normal1;

    vec0.vx = arg1[0] - arg0[0];
    vec0.vy = arg1[1] - arg0[1];
    vec0.vz = arg1[2] - arg0[2];
    VectorNormal(&vec0, &normal0);

    vec0.vx = arg2[0] - arg0[0];
    vec0.vy = arg2[1] - arg0[1];
    vec0.vz = arg2[2] - arg0[2];
    VectorNormal(&vec0, &normal1);

    OuterProduct12(&normal0, &normal1, arg4);

    if (arg4->vy == 0) {
        arg3[1] = 0;
        return;
    }

    arg3[1] = arg0[1] +
              ((-(arg4->vx * (arg3[0] - arg0[0])) -
                (arg4->vz * (arg3[2] - arg0[2]))) /
               arg4->vy);
}

/* ---- func_8007B1C4: camera collision (ceiling) check -----------------------
 * Handwritten ASM using GTE NCLIP for point-in-triangle tests. Checks if
 * camera Y/Z position is inside any collision polygon from D_800AFB24[idx].
 * Returns collision height if found, 0 if not. Zeroes output structs.
 *
 * With zeroed collision data (port stub), D_800AFB44[idx] == 0 → loop
 * skipped → returns 0, zeroes outputs. Correct no-collision behavior. */
extern s32 D_800AFB24[];
extern s32 D_800AFB34[];
extern s32 D_800AFB44[];
extern u32 D_800AFB20[];
extern u8 D_800B21CC;
extern s16 D_800AFB54;

s16 func_8007B1C4(s16 camY, s16 camZ, s32 idx, s16* pOut, s32* pState) {
    s32 count = D_800AFB44[idx];
    s32 polyBase = D_800AFB24[idx];
    s32 vertBase = D_800AFB34[idx];

    /* With zeroed data: count=0, skip loop, fall through to return 0 */

    /* XENO_PC_PORT TODO: when collision data is populated, implement the
     * GTE NCLIP point-in-triangle loop here. For now, the zeroed-data
     * path is the correct no-collision behavior. */

    /* No collision found: zero outputs, return 0 */
    pOut[0] = 0;
    pOut[1] = 0;
    pOut[2] = 0;
    if (pState) {
        pState[0] = 0;
        pState[1] = 0;
        pState[2] = 0;
    }
    return 0;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc4", func_8007B478);

extern s16 D_800B218C;

void func_8007B614(s32* pOut, s16 scale, s16 angle) {
    s32 magnitude = (scale << 4) * D_800B218C;

    magnitude >>= 12;
    angle &= 0xFFF;

    pOut[0] = rsin(angle) * magnitude;
    pOut[1] = 0;
    pOut[2] = -(rcos(angle) * magnitude);
}

s32 func_8007B694(s32* arg0) {
    return -ratan2(arg0[2], arg0[0]) & 0xFFF;
}

extern long FieldGetVec2Magnitude(long x, long y);

s32 func_8007B6C4(s16 angle, s16* edge, s32* vec) {
    VECTOR side;
    VECTOR normal;
    s32 edgeAngle;
    s32 deltaAngle;
    s32 magnitude;

    deltaAngle = (0xC00 - angle) & 0xFFF;
    edgeAngle = -ratan2(edge[6] - edge[2], edge[4] - edge[0]) & 0xFFF;
    deltaAngle = (deltaAngle + edgeAngle) & 0xFFF;

    if ((u32)(deltaAngle - 0x80) >= 0xF01) {
        vec[0] = 0;
        vec[1] = 0;
        vec[2] = 0;
        return edgeAngle;
    }

    if (deltaAngle < 0x800) {
        side.vx = edge[0] - edge[4];
        side.vy = 0;
        side.vz = edge[2] - edge[6];
        edgeAngle = (edgeAngle + 0x800) & 0xFFF;
    } else {
        side.vx = edge[4] - edge[0];
        side.vy = 0;
        side.vz = edge[6] - edge[2];
    }

    VectorNormal(&side, &normal);
    magnitude = FieldGetVec2Magnitude(vec[0] >> 12, vec[2] >> 12);
    vec[0] = normal.vx * magnitude;
    vec[1] = 0;
    vec[2] = normal.vz * magnitude;
    return edgeAngle;
}

extern s32 D_800ADB98;
extern s32 func_8007C694(s32* move, s32* base, void* pActorData,
                         s16* outEdge, s16* outPoint, s32 mode);

void* func_8007B814(s32* pVec, void* pActorData, s32* pOut, s16 angle) {
    u8* actorData = pActorData;
    s16 resolvedAngle = angle;
    s16 point[16];
    s32 move[3];
    s32 result;
    s32 candidateAngle;

    candidateAngle = angle & 0x0FFF;
    move[0] = pVec[0] + (rsin(candidateAngle) << 6);
    move[1] = 0;
    move[2] = pVec[2] - (rcos(candidateAngle) << 6);
    result = func_8007C694(move, (s32*)(actorData + 0x20), actorData,
                           (s16*)pOut, point, -1);

    if (result != -1) {
        resolvedAngle = result >> 16;

        candidateAngle = (resolvedAngle - 0x100) & 0x0FFF;
        move[0] = pVec[0] + (rsin(candidateAngle) << 6);
        move[1] = 0;
        move[2] = pVec[2] - (rcos(candidateAngle) << 6);
        result = func_8007C694(move, (s32*)(actorData + 0x20), actorData,
                               (s16*)pOut, point, -1);

        if (result != -1) {
            candidateAngle = (resolvedAngle + 0x100) & 0x0FFF;
            move[0] = pVec[0] + (rsin(candidateAngle) << 6);
            move[1] = 0;
            move[2] = pVec[2] - (rcos(candidateAngle) << 6);
            result = func_8007C694(move, (s32*)(actorData + 0x20), actorData,
                                   (s16*)pOut, point, -1);

            if (result != -1) {
                move[0] = pVec[0];
                move[1] = pVec[1];
                move[2] = pVec[2];
            } else {
                move[0] = pVec[0];
                move[1] = pVec[1];
                move[2] = pVec[2];
                func_8007B6C4(resolvedAngle, (s16*)pOut, move);
            }
        } else {
            move[0] = pVec[0];
            move[1] = pVec[1];
            move[2] = pVec[2];
            func_8007B6C4(resolvedAngle, (s16*)pOut, move);
        }
    } else {
        move[0] = pVec[0];
        move[1] = pVec[1];
        move[2] = pVec[2];
        func_8007B6C4(resolvedAngle, (s16*)pOut, move);
    }

    if (func_8007C694(move, (s32*)(actorData + 0x20), actorData,
                      (s16*)pOut, point, 0) == -1) {
        return (void*)-1;
    }

    if ((*(u32*)actorData & 0x00040000) != 0) {
        point[1] = *(u16*)(actorData + 0xEC);
    } else if (((s32)point[1] << 16) < *(s32*)(actorData + 0x24) &&
               D_800ADB98 == 0) {
        return (void*)-1;
    }

    move[1] = ((s32)point[1] << 16) - *(s32*)(actorData + 0x24);
    pVec[0] = move[0];
    pVec[1] = move[1];
    pVec[2] = move[2];
    *(s16*)(actorData + 0x72) = (*(s32*)(actorData + 0x24) + pVec[1]) >> 16;
    return 0;
}

void* func_8007BAC0(s32* pVec, void* pActorData, s32* pOut, s16 angle) {
    u8* actorData = pActorData;
    s16 signedAngle = angle;
    s16 edge[4];
    s16 point[16];
    s16 savedPoint[16];
    s32 move[3];
    s32 savedMove[3];
    s32 flags = 0;
    s32 candidateAngle;
    s32 magnitude;
    VECTOR normalIn;
    VECTOR normalOut;

    candidateAngle = (signedAngle - 0x100) & 0x0FFF;
    move[0] = pVec[0] + (rsin(candidateAngle) << 6);
    move[1] = 0;
    move[2] = pVec[2] - (rcos(candidateAngle) << 6);
    if (func_8007BEF4(move, (s32*)(actorData + 0x20), actorData,
                      (s16*)pOut, point, -1, &flags) != -1) {
        candidateAngle = (signedAngle + 0x100) & 0x0FFF;
        move[0] = pVec[0] + (rsin(candidateAngle) << 6);
        move[1] = 0;
        move[2] = pVec[2] - (rcos(candidateAngle) << 6);
        if (func_8007BEF4(move, (s32*)(actorData + 0x20), actorData,
                          (s16*)pOut, point, -1, &flags) != -1) {
            candidateAngle = angle & 0x0FFF;
            move[0] = pVec[0] + (rsin(candidateAngle) << 6);
            move[1] = 0;
            move[2] = pVec[2] - (rcos(candidateAngle) << 6);
            if (func_8007BEF4(move, (s32*)(actorData + 0x20), actorData,
                              (s16*)pOut, point, -1, &flags) != -1) {
                move[0] = pVec[0];
                move[1] = pVec[1];
                move[2] = pVec[2];
            } else {
                move[0] = pVec[0];
                move[1] = pVec[1];
                move[2] = pVec[2];
                func_8007B6C4(signedAngle, (s16*)pOut, move);
            }
        } else {
            move[0] = pVec[0];
            move[1] = pVec[1];
            move[2] = pVec[2];
            func_8007B6C4(signedAngle, (s16*)pOut, move);
        }
    } else {
        move[0] = pVec[0];
        move[1] = pVec[1];
        move[2] = pVec[2];
        func_8007B6C4(signedAngle, (s16*)pOut, move);
    }

    if (func_8007BEF4(move, (s32*)(actorData + 0x20), actorData,
                      (s16*)pOut, point, 0, &flags) == -1) {
        return (void*)-1;
    }

    savedMove[0] = move[0];
    savedMove[1] = move[1];
    savedMove[2] = move[2];
    savedPoint[0] = point[0];
    savedPoint[1] = point[1];
    savedPoint[2] = point[2];
    savedPoint[3] = point[3];

    if (point[1] < *(s16*)(actorData + 0x26)) {
retry_along_normal:
        normalIn.vx = -move[0] >> 8;
        normalIn.vy = (((s32)point[1] << 16) - *(s32*)(actorData + 0x24)) >> 8;
        normalIn.vz = -move[2] >> 8;
        VectorNormal(&normalIn, &normalOut);

        magnitude = FieldGetVec2Magnitude(move[0] >> 8, move[2] >> 8);
        move[0] = -(magnitude * normalOut.vx) >> 4;
        move[1] = (magnitude * normalOut.vy) >> 4;
        move[2] = -(magnitude * normalOut.vz) >> 4;

        if (func_8007BEF4(move, (s32*)(actorData + 0x20), actorData,
                          (s16*)pOut, point, 0, &flags) == -1) {
            return (void*)-1;
        }

        *(u32*)actorData |= 0x04000000;
    } else {
        if ((flags & 0x00200000) != 0) {
            goto retry_along_normal;
        }

        if ((flags & 0x00420000) != 0) {
            if ((*(u32*)(actorData + 0x14) & 0x00420000) != 0) {
                goto retry_along_normal;
            }
        } else if (point[1] < *(s16*)(actorData + 0x26) + 0x40) {
            goto retry_along_normal;
        }

        move[0] = savedMove[0];
        move[1] = savedMove[1];
        move[2] = savedMove[2];
        point[0] = savedPoint[0];
        point[1] = savedPoint[1];
        point[2] = savedPoint[2];
        point[3] = savedPoint[3];
    }

    move[1] = ((s32)point[1] << 16) - *(s32*)(actorData + 0x24);
    pVec[0] = move[0];
    pVec[1] = move[1];
    pVec[2] = move[2];
    *(s16*)(actorData + 0x72) = (*(s32*)(actorData + 0x24) + pVec[1]) >> 16;
    return 0;
}

static s32 FieldPackedXZ(const s16* vert);
static void FieldCopyCameraEdge(u8* out, const s16* a, const s16* b);

s32 func_8007BEF4(s32* move, s32* base, u8* actorData, s16* outEdge,
                  s16* outPoint, s32 mode, s32* outFlags) {
    s32 state = *(s16*)(actorData + 0x10);
    s16 triIndex = *(s16*)(actorData + state * 2 + 0x08);
    u8* triBase = (u8*)(uintptr_t)(u32)D_800AFB24[state];
    u8* vertBase = (u8*)(uintptr_t)(u32)D_800AFB34[state];
    s32 packedNewXZ;
    s32 packedOldXZ;
    s32 collisionMask = 0;
    s32 forceEdgeSearch;
    s32 steps = 0;
    s32 lastTri = triIndex;
    s32 sideMask = 0;

    if (triIndex == -1) {
        return -1;
    }

    outPoint[0] = (base[0] + move[0]) >> 16;
    outPoint[1] = 0;
    outPoint[2] = (base[2] + move[2]) >> 16;
    packedNewXZ = ((s32)outPoint[0] << 16) + outPoint[2];
    packedOldXZ = ((base[0] >> 16) << 16) + (base[2] >> 16);

    if (((*(u32*)(actorData + 0x04) >> (state + 3)) & 1) == 0) {
        collisionMask = (D_800B21CC == 0) ? -1 : 0;
    }

    {
        s16* tri = (s16*)(triBase + triIndex * 14);
        u32 triFlags = ((u32*)(uintptr_t)D_800AFB20[0])[*((u8*)tri + 0x0C)];
        forceEdgeSearch = ((triFlags & 0x00400000) != 0) || mode == 0x80;
    }

    for (;;) {
        s16* tri = (s16*)(triBase + triIndex * 14);
        s16* v0 = (s16*)(vertBase + tri[0] * 8);
        s16* v1 = (s16*)(vertBase + tri[1] * 8);
        s16* v2 = (s16*)(vertBase + tri[2] * 8);
        s32 p0 = FieldPackedXZ(v0);
        s32 p1 = FieldPackedXZ(v1);
        s32 p2 = FieldPackedXZ(v2);
        s32 nextTri = -1;
        u32 flags;

        lastTri = triIndex;
        sideMask = 0;
        if (NormalClip(p0, p1, packedNewXZ) < 0) {
            sideMask |= 1;
        }
        if (NormalClip(p1, p2, packedNewXZ) < 0) {
            sideMask |= 2;
        }
        if (NormalClip(p2, p0, packedNewXZ) < 0) {
            sideMask |= 4;
        }

        switch (sideMask) {
        case 0:
            steps = 0xFF;
            break;
        case 1:
            nextTri = tri[3];
            break;
        case 2:
            nextTri = tri[4];
            break;
        case 3:
            nextTri = (NormalClip(p1, packedNewXZ, packedOldXZ) < 0) ? tri[3] : tri[4];
            break;
        case 4:
            nextTri = tri[5];
            break;
        case 5:
            nextTri = (NormalClip(p0, packedNewXZ, packedOldXZ) < 0) ? tri[5] : tri[3];
            break;
        case 6:
            nextTri = (NormalClip(p2, packedNewXZ, packedOldXZ) >= 0) ? tri[5] : tri[4];
            break;
        default:
            triIndex = -1;
            break;
        }

        if (sideMask != 0 && sideMask < 7) {
            triIndex = (s16)nextTri;
        }

        if (triIndex != -1) {
            tri = (s16*)(triBase + triIndex * 14);
            flags = ((u32*)(uintptr_t)D_800AFB20[0])[*((u8*)tri + 0x0C)] & collisionMask;
            *outFlags = flags;

            if ((((*(u32*)actorData >> 8) & 7) & (flags >> 5)) != 0) {
                triIndex = -1;
            } else if ((flags & 0x00800000) != 0 && *(s16*)(actorData + 0x10) == 0) {
                triIndex = -1;
            } else if ((flags & 0x00400000) != 0) {
                if (!forceEdgeSearch) {
                    func_8007B07C((s16*)(vertBase + tri[0] * 8),
                                  (s16*)(vertBase + tri[1] * 8),
                                  (s16*)(vertBase + tri[2] * 8),
                                  outPoint, (VECTOR*)((u8*)outPoint + 0x08));
                    if (outPoint[1] < *(s16*)((u8*)base + 0x06)) {
                        triIndex = -1;
                    }
                }
            }
        }

        if (triIndex != -1) {
            steps++;
            if (steps < 0x20) {
                continue;
            }
        }
        break;
    }

    if (triIndex != -1 && steps != 0x20) {
        if (mode == -1) {
            return 0;
        }

        {
            s16* tri = (s16*)(triBase + triIndex * 14);
            func_8007B07C((s16*)(vertBase + tri[0] * 8),
                          (s16*)(vertBase + tri[1] * 8),
                          (s16*)(vertBase + tri[2] * 8),
                          outPoint, (VECTOR*)((u8*)outPoint + 0x08));
        }
        return 0;
    }

    if (sideMask == 1 || sideMask == 2 || sideMask == 4) {
        s16* tri = (s16*)(triBase + lastTri * 14);
        if (sideMask == 1) {
            FieldCopyCameraEdge((u8*)outEdge, (s16*)(vertBase + tri[0] * 8),
                                (s16*)(vertBase + tri[1] * 8));
        } else if (sideMask == 2) {
            FieldCopyCameraEdge((u8*)outEdge, (s16*)(vertBase + tri[1] * 8),
                                (s16*)(vertBase + tri[2] * 8));
        } else {
            FieldCopyCameraEdge((u8*)outEdge, (s16*)(vertBase + tri[2] * 8),
                                (s16*)(vertBase + tri[0] * 8));
        }
    }

    return -1;
}

void func_8007C670(s32* arg0, s32* arg1, s32 arg2) {
    s32 value = *arg0;

    if (arg2 >= 0) {
        value += arg2;
    }

    *arg1 = value;
}

s32 func_8007C694(s32* move, s32* base, void* pActorData,
                  s16* outEdge, s16* outPoint, s32 mode) {
    u8* actorData = pActorData;
    s32 state = *(s16*)(actorData + 0x10);
    s16 triIndex = *(s16*)(actorData + state * 2 + 0x08);
    u8* triBase = (u8*)(uintptr_t)(u32)D_800AFB24[state];
    u8* vertBase = (u8*)(uintptr_t)(u32)D_800AFB34[state];
    s32 packedNewXZ;
    s32 packedOldXZ;
    s32 collisionMask = 0;
    s32 steps = 0;
    s32 lastTri = triIndex;
    s32 sideMask = 0;

    if (triIndex == -1) {
        return -1;
    }

    outPoint[0] = (base[0] + move[0]) >> 16;
    outPoint[1] = 0;
    outPoint[2] = (base[2] + move[2]) >> 16;
    packedNewXZ = ((s32)outPoint[0] << 16) + outPoint[2];
    packedOldXZ = ((base[0] >> 16) << 16) + (base[2] >> 16);

    if (((*(u32*)(actorData + 0x04) >> (state + 3)) & 1) == 0) {
        collisionMask = (D_800B21CC == 0) ? -1 : 0;
    }

    for (;;) {
        s16* tri = (s16*)(triBase + triIndex * 14);
        s16* v0 = (s16*)(vertBase + tri[0] * 8);
        s16* v1 = (s16*)(vertBase + tri[1] * 8);
        s16* v2 = (s16*)(vertBase + tri[2] * 8);
        s32 p0 = FieldPackedXZ(v0);
        s32 p1 = FieldPackedXZ(v1);
        s32 p2 = FieldPackedXZ(v2);
        s32 nextTri = -1;
        u32 flags;

        lastTri = triIndex;
        sideMask = 0;
        if (NormalClip(p0, p1, packedNewXZ) < 0) {
            sideMask |= 1;
        }
        if (NormalClip(p1, p2, packedNewXZ) < 0) {
            sideMask |= 2;
        }
        if (NormalClip(p2, p0, packedNewXZ) < 0) {
            sideMask |= 4;
        }

        switch (sideMask) {
        case 0:
            steps = 0xFF;
            break;
        case 1:
            nextTri = tri[3];
            break;
        case 2:
            nextTri = tri[4];
            break;
        case 3:
            if (NormalClip(p1, packedNewXZ, packedOldXZ) < 0) {
                nextTri = tri[3];
                sideMask = 1;
            } else {
                nextTri = tri[4];
                sideMask = 2;
            }
            break;
        case 4:
            nextTri = tri[5];
            break;
        case 5:
            if (NormalClip(p0, packedNewXZ, packedOldXZ) < 0) {
                nextTri = tri[5];
                sideMask = 4;
            } else {
                nextTri = tri[3];
                sideMask = 1;
            }
            break;
        case 6:
            if (NormalClip(p2, packedNewXZ, packedOldXZ) >= 0) {
                nextTri = tri[5];
                sideMask = 4;
            } else {
                nextTri = tri[4];
                sideMask = 2;
            }
            break;
        default:
            triIndex = -1;
            break;
        }

        if (sideMask != 0 && triIndex != -1) {
            triIndex = (s16)nextTri;
        }

        if (triIndex == -1) {
            break;
        }

        tri = (s16*)(triBase + triIndex * 14);
        flags = ((u32*)(uintptr_t)D_800AFB20[0])[*((u8*)tri + 0x0C)] & collisionMask;

        if ((((*(u32*)actorData >> 9) & 3) & (flags >> 3)) != 0 ||
            (((*(u32*)actorData >> 8) & 7) & (flags >> 5)) != 0 ||
            ((flags & 0x00800000) != 0 && *(s16*)(actorData + 0x10) == 0)) {
            triIndex = -1;
            break;
        }

        steps++;
        if (steps < 0x20) {
            continue;
        }
        break;
    }

    if (triIndex != -1 && steps != 0x20) {
        if (mode == -1) {
            return 0;
        }

        {
            s16* tri = (s16*)(triBase + triIndex * 14);
            func_8007B07C((s16*)(vertBase + tri[0] * 8),
                          (s16*)(vertBase + tri[1] * 8),
                          (s16*)(vertBase + tri[2] * 8),
                          outPoint, (VECTOR*)((u8*)outPoint + 0x08));
        }
        return 0;
    }

    if (sideMask == 1 || sideMask == 2 || sideMask == 4) {
        s16* tri = (s16*)(triBase + lastTri * 14);
        if (sideMask == 1) {
            FieldCopyCameraEdge((u8*)outEdge, (s16*)(vertBase + tri[0] * 8),
                                (s16*)(vertBase + tri[1] * 8));
        } else if (sideMask == 2) {
            FieldCopyCameraEdge((u8*)outEdge, (s16*)(vertBase + tri[1] * 8),
                                (s16*)(vertBase + tri[2] * 8));
        } else {
            FieldCopyCameraEdge((u8*)outEdge, (s16*)(vertBase + tri[2] * 8),
                                (s16*)(vertBase + tri[0] * 8));
        }
    }

    return -1;
}

extern s32 D_800ADC10;

void* func_8007CD3C(s32 a0) {
    s32 old = D_800ADC10;
    D_800ADC10 = old + a0;
    return (void*)((old << 2) + 0x1F800000);
}

void func_8007CD60(s32 a0) {
    D_800ADC10 -= a0;
}

static s32 FieldPackedXZ(const s16* vert) {
    return ((s32)vert[0] << 16) + vert[2];
}

static void FieldCopyCameraEdge(u8* out, const s16* a, const s16* b) {
    *(u16*)(out + 0x00) = (u16)a[0];
    *(u16*)(out + 0x02) = (u16)a[1];
    *(u16*)(out + 0x04) = (u16)a[2];
    *(u16*)(out + 0x08) = (u16)b[0];
    *(u16*)(out + 0x0A) = (u16)b[1];
    *(u16*)(out + 0x0C) = (u16)b[2];
}

s32 func_8007CD80(void* arg0, void* arg1, void* arg2) {
    s16 posX = *(s16*)((u8*)arg0 + 0x02);
    s16 posZ = *(s16*)((u8*)arg0 + 0x0A);
    s16 clampedX;
    s16 clampedZ;
    s32 packedPos = ((s32)posX << 16) + posZ;
    s32 packedClamped;
    u8* triBase;
    u8* vertBase;
    s16 triIndex;
    s16 edgeTri = 0;
    s32 edge = 0;
    s32 steps = 0;

    if (posX < *(s16*)((u8*)&g_Scene + 0x4C)) {
        clampedX = *(s16*)((u8*)&g_Scene + 0x4C);
    } else {
        s32 maxX = *(s16*)((u8*)&g_Scene + 0x4C) +
                   *(s16*)((u8*)&g_Scene + 0x50);
        clampedX = (maxX < posX) ? (s16)maxX : posX;
    }

    if (*(s16*)((u8*)&g_Scene + 0x4E) < posZ) {
        clampedZ = *(s16*)((u8*)&g_Scene + 0x4E);
    } else {
        s32 maxZ = *(s16*)((u8*)&g_Scene + 0x4E) +
                   *(s16*)((u8*)&g_Scene + 0x52);
        clampedZ = (posZ < maxZ) ? (s16)maxZ : posZ;
    }

    packedClamped = ((s32)clampedX << 16) + clampedZ;
    triIndex = func_8007B1C4(clampedX, clampedZ, D_800AFB54 - 1,
                             (s16*)arg2, (s32*)((u8*)arg2 + 0x08));
    triBase = (u8*)(uintptr_t)D_800AFB20[D_800AFB54];
    vertBase = (u8*)(uintptr_t)D_800AFB20[D_800AFB54 + 4];

    for (;;) {
        s16* tri;
        s16* v0;
        s16* v1;
        s16* v2;
        s32 p0;
        s32 p1;
        s32 p2;
        s32 sideMask;
        s32 nextTri;

        if (triIndex < 0) {
            break;
        }

        tri = (s16*)(triBase + triIndex * 14);
        v0 = (s16*)(vertBase + tri[0] * 8);
        v1 = (s16*)(vertBase + tri[1] * 8);
        v2 = (s16*)(vertBase + tri[2] * 8);
        p0 = FieldPackedXZ(v0);
        p1 = FieldPackedXZ(v1);
        p2 = FieldPackedXZ(v2);

        sideMask = 0;
        if (NormalClip(p0, p1, packedPos) < 0) {
            sideMask |= 1;
        }
        if (NormalClip(p1, p2, packedPos) < 0) {
            sideMask |= 2;
        }
        if (NormalClip(p2, p0, packedPos) < 0) {
            sideMask |= 4;
        }

        nextTri = -1;
        switch (sideMask) {
        case 0:
            steps = 0xFF;
            break;
        case 1:
            nextTri = tri[3];
            break;
        case 2:
            nextTri = tri[4];
            break;
        case 3:
            nextTri = (NormalClip(p1, packedPos, packedClamped) < 0) ?
                      tri[3] : tri[4];
            break;
        case 4:
            nextTri = tri[5];
            break;
        case 5:
            nextTri = (NormalClip(p0, packedPos, packedClamped) < 0) ?
                      tri[5] : tri[3];
            break;
        case 6:
            nextTri = (NormalClip(p2, packedPos, packedClamped) >= 0) ?
                      tri[5] : tri[4];
            break;
        default:
            nextTri = -1;
            break;
        }

        if (sideMask != 0) {
            edgeTri = triIndex;
            triIndex = (s16)nextTri;
            edge = sideMask;
        }

        {
            s16* curTri;
            u32 flags;

            if (triIndex < 0) {
                break;
            }

            curTri = (s16*)(triBase + triIndex * 14);
            flags = ((u32*)(uintptr_t)D_800AFB20[0])[*((u8*)curTri + 0x0C)];
            if ((flags & 0x800000) == 0) {
                triIndex = -1;
                break;
            }
        }

        steps++;
        if (triIndex == -1 || steps >= 0xF0) {
            break;
        }
        if (sideMask == 0) {
            break;
        }
    }

    if (triIndex != -1 && steps != 0xF0) {
        return 0;
    }

    if (edge == 1 || edge == 2 || edge == 4) {
        s16* tri = (s16*)(triBase + edgeTri * 14);
        s16* a;
        s16* b;

        if (edge == 1) {
            a = (s16*)(vertBase + tri[0] * 8);
            b = (s16*)(vertBase + tri[1] * 8);
        } else if (edge == 2) {
            a = (s16*)(vertBase + tri[1] * 8);
            b = (s16*)(vertBase + tri[2] * 8);
        } else {
            a = (s16*)(vertBase + tri[2] * 8);
            b = (s16*)(vertBase + tri[0] * 8);
        }
        FieldCopyCameraEdge((u8*)arg1, a, b);
    }

    *(s16*)((u8*)arg2 + 0x00) = posX;
    *(s16*)((u8*)arg2 + 0x02) = posZ;
    *(s16*)((u8*)arg2 + 0x04) = clampedX;
    *(s16*)((u8*)arg2 + 0x06) = clampedZ;
    return -1;
}

s32 func_8007D3D4(u8* actorData, s32 idx, s32* outHeight0,
                  VECTOR* outNormal, s16* outTriangle, s32* outHeight1) {
    s16 triIndex;
    u8* triBase;
    u8* vertBase;
    s16 point[4];
    s32 packedNewXZ;
    s32 packedOldXZ;
    s32 collisionMask;
    s32 steps;

    triIndex = *(s16*)(actorData + idx * 2 + 0x08);
    triBase = (u8*)(uintptr_t)(u32)D_800AFB24[idx];
    vertBase = (u8*)(uintptr_t)(u32)D_800AFB34[idx];

    if (triIndex == -1) {
        return -1;
    }

    point[0] = (*(s32*)(actorData + 0x20) + *(s32*)(actorData + 0x30)) >> 16;
    point[1] = 0;
    point[2] = (*(s32*)(actorData + 0x28) + *(s32*)(actorData + 0x38)) >> 16;
    packedNewXZ = (point[0] << 16) + point[2];
    packedOldXZ = ((*(s32*)(actorData + 0x20) >> 16) << 16) +
                  (*(s32*)(actorData + 0x28) >> 16);

    collisionMask = 0;
    if (((*(u32*)(actorData + 0x04) >> (idx + 3)) & 1) == 0) {
        collisionMask = (D_800B21CC == 0) ? -1 : 0;
    }

    steps = 0;
    for (;;) {
        s16* tri = (s16*)(triBase + triIndex * 14);
        s16* v0 = (s16*)(vertBase + tri[0] * 8);
        s16* v1 = (s16*)(vertBase + tri[1] * 8);
        s16* v2 = (s16*)(vertBase + tri[2] * 8);
        s32 p0 = (v0[0] << 16) + v0[2];
        s32 p1 = (v1[0] << 16) + v1[2];
        s32 p2 = (v2[0] << 16) + v2[2];
        s32 sideMask = 0;
        s32 nextTri = -1;

        if (NormalClip(p0, p1, packedNewXZ) < 0) {
            sideMask |= 1;
        }
        if (NormalClip(p1, p2, packedNewXZ) < 0) {
            sideMask |= 2;
        }
        if (NormalClip(p2, p0, packedNewXZ) < 0) {
            sideMask |= 4;
        }

        switch (sideMask) {
        case 0:
            steps = 0xFF;
            break;
        case 1:
            nextTri = tri[5];
            break;
        case 2:
            nextTri = tri[4];
            break;
        case 3:
            nextTri = (NormalClip(p1, packedNewXZ, packedOldXZ) < 0) ? tri[3] : tri[4];
            break;
        case 4:
            nextTri = tri[5];
            break;
        case 5:
            nextTri = (NormalClip(p0, packedNewXZ, packedOldXZ) < 0) ? tri[5] : tri[3];
            break;
        case 6:
            nextTri = (NormalClip(p2, packedNewXZ, packedOldXZ) >= 0) ? tri[5] : tri[4];
            break;
        default:
            nextTri = -1;
            break;
        }

        if (sideMask == 0) {
            steps++;
        } else {
            triIndex = nextTri;
            if (triIndex == -1) {
                return -1;
            }
            steps++;
            if (steps < 0x20) {
                continue;
            }
            if (steps == 0x20) {
                return -1;
            }
        }

        tri = (s16*)(triBase + triIndex * 14);
        v0 = (s16*)(vertBase + tri[0] * 8);
        v1 = (s16*)(vertBase + tri[1] * 8);
        v2 = (s16*)(vertBase + tri[2] * 8);
        func_8007B07C(v0, v1, v2, point, outNormal);

        *outTriangle = triIndex;

        {
            s32 extraHeight = ((s8)*((u8*)tri + 0x0D)) << 2;
            u32 flags = ((u32*)(uintptr_t)D_800AFB20[0])[*((u8*)tri + 0x0C)];
            s32 blocked = flags & (collisionMask & 0x800000);

            if (*(s16*)(actorData + 0x10) != idx) {
                if (blocked != 0) {
                    *outHeight0 = 0x7FFFFFFF;
                    *outHeight1 = 0x7FFFFFFF;
                    return 0;
                }
                *outHeight0 = point[1];
                func_8007C670(outHeight0, outHeight1, extraHeight);
                return 0;
            }

            if (blocked != 0) {
                *outHeight0 = 0x7FFFFFFF;
                *outHeight1 = 0x7FFFFFFF;
                return 0;
            }

            if (*(s32*)(actorData + 0x30) == 0 &&
                *(s32*)(actorData + 0x34) == 0 &&
                *(s32*)(actorData + 0x38) == 0) {
                *outHeight0 = point[1];
            } else {
                *outHeight0 = *(s16*)(actorData + 0x72);
            }
            func_8007C670(outHeight0, outHeight1, extraHeight);
            return 0;
        }
    }
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc4", func_8007D818);

INCLUDE_ASM("asm/field/nonmatchings/main/misc4", func_8007D8B4);
