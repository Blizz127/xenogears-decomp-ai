#include "common.h"
#include "psyq/libgpu.h"
#include "system/memory.h"
#include "system/math.h"
#include "field/main.h"
#include "field/actor.h"
#include "field/effects.h"
#include "field/graphics.h"
#include "field/particles.h"


INCLUDE_ASM("asm/field/nonmatchings/main/misc9", func_800AA9DC);

INCLUDE_ASM("asm/field/nonmatchings/main/misc9", func_800AAA74);

extern SpriteList* D_800AFC68;

void func_800AABD8(void) {
    HeapFree(D_800AFC68);
    DrawSync(0);
}

void func_800AAC08(void) {
    RECT rect;
    SPRT* pSprite;
    SPRT* pSprite2;
    int i;

    D_800AFC68 = HeapAlloc(0x840, 0x0);
    
    rect.x = 0;
    rect.y = 0;
    rect.w = 0xFF;
    rect.h = 0xFF;
    
    for (i = 0; i < 0x21; i++) {
        SetDrawMode(&D_800AFC68->drModes[i][0], 0, 0, GetTPage(0, 0, 0x3C0, 0x100) & 0xFFFF, &rect);
        SetDrawMode(&D_800AFC68->drModes[i][1], 0, 0, GetTPage(0, 0, 0x3C0, 0x140) & 0xFFFF, &rect);
        pSprite = &D_800AFC68->sprites[i][0];
        pSprite2 = &D_800AFC68->sprites[i][1];
        
        SetSprt(pSprite);
        setRGB0(pSprite, 0x80, 0x80, 0x80);
        if (i == 0) {
            setUV0(pSprite, 0xE0, 0x70);
            setWH(pSprite, 0x10, 0x10);
        } else {
            setUV0(pSprite, 0xE0, 0x60);
            setWH(pSprite, 8, 8);
        }
        setXY0(pSprite, 0xA0, 0x70);
        pSprite->clut = GetClut(0x100, 0xF7);
        *pSprite2 = *pSprite;
    };
}

// Set RGB of sprites
void func_800AADC8(int index, int red, int green, int blue) {
    setRGB0(&D_800AFC68->sprites[index][0], red, green, blue);
    setRGB0(&D_800AFC68->sprites[index][1], red, green, blue);
}

void func_800AAE4C(int index, int x, int y, int type) {
    switch (type) {
        case 0:
            y -= 0xC;
            x -= 4;
            break;
        case 1:
            y -= 4;
            x -= 4;
            break;
    }

    D_800AFC68->sprites[index][g_FieldCurRenderContextIndex].x0 = x;
    D_800AFC68->sprites[index][g_FieldCurRenderContextIndex].y0 = y;
    addPrim(g_FieldCurRenderContext->ot3, &D_800AFC68->sprites[index][g_FieldCurRenderContextIndex]);
    addPrim(g_FieldCurRenderContext->ot3, &D_800AFC68->drModes[index][g_FieldCurRenderContextIndex]);
}

extern SpriteList2* D_800B1DF0;
extern PolyList2* D_800C3A3C;

void func_800AAF80(void) {
    RECT rect;
    POLY_FT4* pPoly;
    POLY_FT4* pPoly2;
    SPRT* pSprite;
    SPRT* pSprite2;
    int i;

    D_800C3A3C = HeapAlloc(0x2F8, 0x0);
    D_800B1DF0 = HeapAlloc(0x400, 0x0);
    
    rect.x = 0;
    rect.y = 0;
    rect.w = 0xFF;
    rect.h = 0xFF;
    
    for (i = 0; i < 4; i++) {
        SetDrawMode(&D_800B1DF0->drModes[i][0], 0, 0, GetTPage(0, 0, 0x3C0, 0x140), &rect);
        SetDrawMode(&D_800B1DF0->drModes[i][1], 0, 0, GetTPage(0, 0, 0x3C0, 0x140), &rect);
        pSprite = &D_800B1DF0->sprites[i][0];
        pSprite2 = pSprite + 1;
        SetSprt(pSprite);
        setRGB0(pSprite, 0x80, 0x80, 0x80);
        setXY0(pSprite, 0xA0, 0x70);
        if (i == 0) {
            setUV0(pSprite, 0xE0, 0x70);
            setWH(pSprite, 0x10, 0x10);
        } else {
            setUV0(pSprite, 0xE0, 0x60);
            setWH(pSprite, 0x8, 0x8);
        }
        pSprite->clut = GetClut(0x100, 0xF7);
        *pSprite2 = *pSprite;
    }

    for (i = 0; i < 3; i++) {
        pPoly = &D_800C3A3C->polys[i][0];
        pPoly2 = pPoly + 1;
        SetPolyFT4(pPoly);
        setXY4(pPoly, 
           i * 0x80, 0x0, 
           i * 0x80 + 0x80, 0x0, 
           i * 0x80, 0xDF, 
           i * 0x80 + 0x80, 0xDF
        );
        setRECT(&D_800C3A3C->rects[i][0], 0x0, 0x0, 0xFF, 0xFF);
        setRECT(&D_800C3A3C->rects[i][1], 0x0, 0x0, 0xFF, 0xFF);
        SetDrawMode(&D_800C3A3C->drModes[i][0], 0, 0, GetTPage(1, 0, 0x300 + (i * 0x40), 0x100), &D_800C3A3C->rects[i][0]);
        SetDrawMode(&D_800C3A3C->drModes[i][1], 0, 0, GetTPage(1, 0, 0x300 + (i * 0x40), 0x100), &D_800C3A3C->rects[i][1]);
        setRGB0(pPoly, 0x80, 0x80, 0x80);
        SetSemiTrans(pPoly, 0x1);
        setUV4(pPoly, 0x0, 0x0, 0x80, 0x0, 0x0, 0xDF, 0x80, 0xDF);
        pPoly->tpage = GetTPage(1, 0, 0x300 + (i * 0x40), 0x100);
        pPoly->clut = GetClut(0, 0xF6);
        *pPoly2 = *pPoly;
    }
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc9", func_800AB328);

extern SpriteList2* D_800B1DF0;
extern PolyList2* D_800C3A3C;

extern s32 D_800AFE78;
extern s32 D_800AFE7C;
extern s32 g_PlayerActorIndex;
extern s32 D_800C3914;
extern s32 D_800C3A18;

void func_800AB378(s8 color) {
    ActorData* pActor;
    int i;
    int y;
    int x;
    
    pActor = g_FieldActors[g_PlayerActorIndex].pActorData;
    x = (CONV_TO_GTE(pActor->position.vx) * D_800C3914) >> 0x10;
    y = -(CONV_TO_GTE(pActor->position.vz) * D_800C3A18) >> 0x10;
    
    for (i = 0; i < 1; i++) {
        if (i == 0) {
            y -= 0xC;
            x -= 4;
        }
        
        D_800B1DF0->sprites[i][g_FieldCurRenderContextIndex].x0 = x + D_800AFE78;
        D_800B1DF0->sprites[i][g_FieldCurRenderContextIndex].y0 = y + D_800AFE7C;
        
        setRGB0(&D_800B1DF0->sprites[i][g_FieldCurRenderContextIndex & 1], color, color, color);
        addPrim(g_FieldCurRenderContext->ot3, &D_800B1DF0->sprites[i][g_FieldCurRenderContextIndex]);
        addPrim(g_FieldCurRenderContext->ot3, &D_800B1DF0->drModes[i][g_FieldCurRenderContextIndex]);
    }

    for (i = 0; i < 3; i++) {
        setRGB0(&D_800C3A3C->polys[i][g_FieldCurRenderContextIndex & 1], color, color, color);
        addPrim(g_FieldCurRenderContext->ot3, &D_800C3A3C->polys[i][g_FieldCurRenderContextIndex]);
        addPrim(g_FieldCurRenderContext->ot3, &D_800C3A3C->drModes[i][g_FieldCurRenderContextIndex]);
    }
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc9", func_800AB748);

INCLUDE_ASM("asm/field/nonmatchings/main/misc9", func_800AB808);

INCLUDE_ASM("asm/field/nonmatchings/main/misc9", func_800ABA98);


extern SpriteList3 D_800B0188;

void func_800ABD18(void) {
    int i;
    RECT rect;
    
    rect.x = 0;
    rect.y = 0;
    rect.w = 0xFF;
    rect.h = 0xFF;
    
    for (i = 0; i < 5; i++) {
        SetDrawMode(&D_800B0188.drModes[i][0], 0, 0, GetTPage(1, 0, 0x280 + (i * 0x40), 0), &rect);
        SetDrawMode(&D_800B0188.drModes[i][1], 0, 0, GetTPage(1, 0, 0x280 + (i * 0x40), 0), &rect);
        SetSprt(&D_800B0188.sprites[i][0]);
        setRGB0(&D_800B0188.sprites[i][0], 0x80, 0x80, 0x80);
        setXY0(&D_800B0188.sprites[i][0], i * 0x80, 0x0);
        setUV0(&D_800B0188.sprites[i][0], 0x0, 0x0);
        setWH(&D_800B0188.sprites[i][0], 0x80, 0xE0);
        SetSemiTrans(&D_800B0188.sprites[i][0], 0);
        (&D_800B0188.sprites[i][0])->clut = GetClut(0, 0xE8);
        D_800B0188.sprites[i][1] = D_800B0188.sprites[i][0];
    }
}

extern s16 D_800ADB54;

void func_800ABEC8(void) {
    int i;

    if (D_800ADB54) {
        for (i = 0; i < 5; i++) {
            addPrim(g_FieldCurRenderContext->ot3, &D_800B0188.sprites[i][g_FieldCurRenderContextIndex]);
            addPrim(g_FieldCurRenderContext->ot3, &D_800B0188.drModes[i][g_FieldCurRenderContextIndex]);
        }
    }
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc9", func_800ABFDC);

INCLUDE_ASM("asm/field/nonmatchings/main/misc9", func_800AC03C);

INCLUDE_ASM("asm/field/nonmatchings/main/misc9", func_800AC0F0);

INCLUDE_ASM("asm/field/nonmatchings/main/misc9", func_800AC308);

INCLUDE_ASM("asm/field/nonmatchings/main/misc9", func_800AC3AC);

INCLUDE_ASM("asm/field/nonmatchings/main/misc9", func_800AC99C);

INCLUDE_ASM("asm/field/nonmatchings/main/misc9", func_800ACB90);

INCLUDE_ASM("asm/field/nonmatchings/main/misc9", func_800ACC58);

extern s32 D_8004F300;
extern void* D_800AF76C;
extern void* D_800AF770;

void func_800ACCB0(void) {
    if (D_8004F300 != 0) {
        HeapFree(D_800AF76C);
        HeapFree(D_800AF770);
    }
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc9", func_800ACCF4);

short FieldScriptVMGetInstructionArgumentS16(int offset) {
    u_char* pData;

    pData = g_FieldScriptVMCurScriptData + (g_FieldScriptVMCurActor->scriptInstructionPointer + offset);
    return (pData[0] + (pData[1] << 8));
}

int FieldScriptVMGetInstructionArgument(int argumentIndex) {
    u_char* pData;

    pData = g_FieldScriptVMCurScriptData + (g_FieldScriptVMCurActor->scriptInstructionPointer + argumentIndex);
    return *pData | *(pData + 1) <<  8;
}

int FieldScriptVMGetArgument(int index) {
    int nArgument;

    nArgument = FieldScriptVMGetInstructionArgument(index);
    if (!(nArgument & 0x8000)) 
        return FieldScriptVMGetVariableValue(nArgument & 0xFFFF);
    return nArgument & 0x7FFF;
}
