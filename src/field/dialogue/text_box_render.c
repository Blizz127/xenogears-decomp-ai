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

extern u16 D_800C3900;
void func_80034874(void* arg0, u8 arg1);
int func_8003487C(void* arg0);

void func_8007DCF8(s32 index, void* ot, s32 renderContextIndex) {
    u8* pTextBox = (u8*)&g_FieldTextBoxes[index];
    u8* pWindow = pTextBox + 0x18;

    (void)ot;
    (void)renderContextIndex;

    if (*(s16*)(pTextBox + 0x37C) != 0 || *(s16*)(pTextBox + 0x408) != 0) {
        return;
    }

    if (*(u16*)(pTextBox + 0x410) != 0) {
        func_8003487C(pWindow);
        return;
    }

    if (D_800C3900 & 0x4000) {
        s16 value = *(s16*)(pTextBox + 0x382) + 1;

        *(s16*)(pTextBox + 0x382) = value;
        if ((*(s16*)(pTextBox + 0x380) - 1) < value) {
            *(s16*)(pTextBox + 0x382) = 0;
        }
    }

    if (D_800C3900 & 0x1000) {
        s16 value = *(s16*)(pTextBox + 0x382) - 1;

        *(s16*)(pTextBox + 0x382) = value;
        if (value < 0) {
            *(s16*)(pTextBox + 0x382) = *(s16*)(pTextBox + 0x380) - 1;
        }
    }

    func_80034874(pWindow, *(s16*)(pTextBox + 0x382) + *(s16*)(pTextBox + 0x37E));
}

// https://decomp.me/scratch/NE0tE
extern u8 D_800B1DF4[];
extern RECT D_800AFC80[];
extern s32 D_800B068C[];
extern u16 D_800B2174[];
extern s32 D_800ADE90;
extern s32 D_800ADE94;
extern s32 D_800ADE98;
extern u16 D_800C2694;
extern u16 D_800ADF54;
extern u16 D_800ADF56;
extern s16 D_800B21D6;
extern void* D_800ADBF0;

void FieldTextBoxInitializePrimitives(int index);
void* GetStringEntry(void* arg0, s32 arg1);
s32 FieldScriptVMGetVariableValue(s32 index);
void func_80032F54(void* arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5, s32 arg6, s32 arg7);
void func_800345E0(void* arg0);
void func_80034614(void* arg0);
void func_800346D4(void* arg0);
void func_80034714(void* arg0, void* arg1);
void func_80034888(void* arg0, void* arg1, s32 arg2);
void func_80034874(void* arg0, u8 arg1);
int func_8003487C(void* arg0);
int func_80033CD0(void* arg0);
void func_8007E1C0(void* arg0, s32 arg1, s32 arg2);

static void FieldTextBoxLinkPrim(void* ot, void* prim) {
    u32 old = *(u32*)ot;
    u32 addr = (u32)(uintptr_t)prim & 0x00FFFFFF;

    *(u32*)prim = (*(u32*)prim & 0xFF000000) | (old & 0x00FFFFFF);
    *(u32*)ot = (old & 0xFF000000) | addr;
}

void FieldTextBoxInitialize(void) {
    RECT rect;
    s32 i;

    for (i = 0; i < 0x10; i++) {
        RECT* pRect = &D_800AFC80[i];
        short tpage = GetTPage(0, 0, 0x380, 0x100);

        pRect->x = 0;
        pRect->y = 0;
        pRect->w = 0xFF;
        pRect->h = 0xFF;
        SetDrawMode((DR_MODE*)(D_800B1DF4 + i * 0xC), 0, 0, tpage, pRect);
        SetDrawMode((DR_MODE*)(D_800B1DF4 + 0xC0 + i * 0xC), 0, 0, tpage, pRect);
    }

    rect.x = 0;
    rect.y = 0;
    rect.w = 0xFF;
    rect.h = 0xFF;

    for (i = 0; i < 4; i++) {
        u8* pTextBox = (u8*)&g_FieldTextBoxes[i];
        short tpage;

        *(s16*)(pTextBox + 0x416) = 0xFF;
        *(s16*)(pTextBox + 0x418) = 0xFF;
        *(s16*)(pTextBox + 0x37C) = -1;
        *(s16*)(pTextBox + 0x3C4) = -1;
        *(s16*)(pTextBox + 0x40E) = -1;
        *(s16*)(pTextBox + 0x414) = -1;
        *(u16*)(pTextBox + 0x410) = 0xFFFF;
        *(s16*)(pTextBox + 0x412) = 0;
        FieldTextBoxInitializePrimitives(i);

        D_800B068C[i] = -1;
        tpage = GetTPage(0, 0, 0x300, 0x100);
        SetDrawMode(&g_FieldTextBoxes[i].drawModes[0], 0, 0, tpage, &rect);
        SetDrawMode(&g_FieldTextBoxes[i].drawModes[1], 0, 0, tpage, &rect);
    }
}

// _pad[0x4A] is possibly a RECT?
void func_8007E114(int index, int arg1, int arg2, int arg3, s32 arg4) {
    g_FieldTextBoxes[index]._pad[0x4A] = arg1;
    g_FieldTextBoxes[index]._pad[0x4B] = arg2;
    g_FieldTextBoxes[index]._pad[0x4C] = arg3;
    g_FieldTextBoxes[index]._pad[0x4D] = arg4;
}

void func_8007E16C(void* arg0, s32 x, s32 y, s32 w, s32 h, s32 flipX) {
    u8* pPrim = arg0;
    s32 x1;
    s32 y1;

    if (flipX == 0) {
        x--;
        x1 = x + w;
        *(s16*)(pPrim + 0x08) = x;
        *(s16*)(pPrim + 0x10) = x1;
        *(s16*)(pPrim + 0x18) = x;
        *(s16*)(pPrim + 0x20) = x1;
    } else {
        x1 = x + w;
        *(s16*)(pPrim + 0x10) = x;
        *(s16*)(pPrim + 0x08) = x1;
        *(s16*)(pPrim + 0x20) = x;
        *(s16*)(pPrim + 0x18) = x1;
    }

    y1 = y + h;
    *(s16*)(pPrim + 0x0A) = y;
    *(s16*)(pPrim + 0x12) = y;
    *(s16*)(pPrim + 0x1A) = y1;
    *(s16*)(pPrim + 0x22) = y1;
}

void func_8007E1C0(void* ot, s32 renderContextIndex, s32 textBoxIndex) {
    u8* pTextBox = (u8*)&g_FieldTextBoxes[textBoxIndex];
    s32 x;
    s32 y;
    s32 w;
    s32 h;
    s32 flags;
    FieldTextBoxBackground* pBackground;
    FieldTextBoxBorders* pBorders;
    DR_MODE* borderDrawModes;
    SPRT* borderSprites;
    s32 i;

    if (*(s16*)(pTextBox + 0x40E) != 0) {
        return;
    }

    x = *(s16*)(pTextBox + 0xAC);
    y = *(s16*)(pTextBox + 0xAE);
    w = *(s16*)(pTextBox + 0xB0);
    h = *(s16*)(pTextBox + 0xB2);

    if (*(s16*)(pTextBox + 0x408) != 0) {
        s32 openTimer = *(s16*)(pTextBox + 0x408);
        s32 baseTimer = D_800B21D6;
        s32 halfW;
        s32 halfH;
        s32 animW;
        s32 animH;

        animW = ((w << 16) / (baseTimer * 2)) * (baseTimer - openTimer);
        animH = ((h << 16) / (baseTimer * 2)) * (baseTimer - openTimer);
        halfW = (w + ((u32)w >> 31)) >> 1;
        halfH = (h + ((u32)h >> 31)) >> 1;
        w = (animW << 1) >> 16;
        h = (animH << 1) >> 16;
        x = x + halfW - (animW >> 16);
        y = y + halfH - (animH >> 16);
        if (w < 0x10) {
            x -= (0x10 - w) / 2;
            w = 0x10;
        }
        if (h < 0x10) {
            y -= (0x10 - h) / 2;
            h = 0x10;
        }

        *(s32*)(pTextBox + 0x41C) += *(s32*)(pTextBox + 0x424);
        *(s32*)(pTextBox + 0x420) += *(s32*)(pTextBox + 0x428);
        x += *(s16*)(pTextBox + 0x41E);
        y += *(s32*)(pTextBox + 0x420) >> 16;
    }

    if (*(s16*)(pTextBox + 0x3C4) != 0 || *(u16*)(pTextBox + 0x410) != 0 ||
        *(s16*)(pTextBox + 0x408) != 0 || (*(u16*)(pTextBox + 0x40C) & 0x40) ||
        *(s16*)(pTextBox + 0x37C) == 0) {
        *(s16*)(pTextBox + 0x40A) = 2;
    } else if (*(s16*)(pTextBox + 0x40A) != 0) {
        *(s16*)(pTextBox + 0x40A) -= 1;
    }

    pBorders = &g_FieldTextBoxes[textBoxIndex].borders;
    if (renderContextIndex == 0) {
        borderDrawModes = pBorders->drawModes1;
        borderSprites = pBorders->sprites1;
    } else {
        borderDrawModes = pBorders->drawModes2;
        borderSprites = pBorders->sprites2;
    }

    borderSprites[0].x0 = x - 8;
    borderSprites[0].y0 = y - 7;
    borderSprites[1].x0 = x + w - 8;
    borderSprites[1].y0 = y + 9;
    borderSprites[2].x0 = x + w - 8;
    borderSprites[2].y0 = y - 7;
    borderSprites[3].x0 = x - 8;
    borderSprites[3].y0 = y + 9;
    borderSprites[4].x0 = x - 8;
    borderSprites[4].y0 = y + h - 9;
    borderSprites[5].x0 = x + 8;
    borderSprites[5].y0 = y - 7;
    borderSprites[6].x0 = x + w - 8;
    borderSprites[6].y0 = y + h - 9;
    borderSprites[7].x0 = x + 8;
    borderSprites[7].y0 = y + h - 9;

    borderSprites[1].h = h - 0x12;
    borderSprites[3].h = h - 0x12;
    if ((s16)borderSprites[1].h < 0) {
        borderSprites[1].h = 0;
        borderSprites[3].h = 0;
    }
    borderSprites[5].w = w - 0x10;
    borderSprites[7].w = w - 0x10;

    flags = *(u16*)(pTextBox + 0x40C);
    if (!(flags & 0x40)) {
        for (i = 0; i < 8; i++) {
            FieldTextBoxLinkPrim(ot, &borderSprites[i]);
            FieldTextBoxLinkPrim(ot, &borderDrawModes[i]);
        }
    }

    pBackground = &g_FieldTextBoxes[textBoxIndex].background;
    pBackground->tiles[renderContextIndex].x0 = x;
    pBackground->tiles[renderContextIndex].y0 = y + 1;
    pBackground->tiles[renderContextIndex].w = w;
    pBackground->tiles[renderContextIndex].h = h - 2;
    if (!(flags & 0x40)) {
        FieldTextBoxLinkPrim(ot, &pBackground->tiles[renderContextIndex]);
        FieldTextBoxLinkPrim(ot, &pBackground->drawModes[renderContextIndex]);
    }
}

extern u8 D_800594D4;
extern u8 D_800594D5;
extern u8 D_800594D6;
extern RECT D_800ADE9C[];
extern RECT D_800ADEDC;
extern RECT D_800ADF04;

void FieldTextBoxInitializePrimitives(int index) {
    int i;
    RECT rect;
    POLY_FT4* pPoly;
    POLY_FT4* pPoly2;
    TILE* pBackgroundTile;
    TILE* pBackgroundTile2;
    SPRT* pBorderSprite1;
    SPRT* pBorderSprite2;
    SPRT* pArrowSprite;
    SPRT* pArrowSprite2;
    SPRT* pCursorSprite2;
    u16* pWidth;
    u16* pHeight;

    FieldTextBoxBackground* pBackground;
    FieldTextBoxCursor* pCursor;
    FieldTextBoxContinueArrow* pArrow;
    FieldTextBoxBorders* pBorders;
    FieldTextBoxPortrait* pPortrait;

    // Background
    pBackground = &g_FieldTextBoxes[index].background;
    SetDrawMode(&pBackground->drawModes[0], NULL, 0, GetTPage(0, 2, 0x280, 0x1F0), NULL);
    SetDrawMode(&pBackground->drawModes[1], NULL, 0, GetTPage(0, 2, 0x280, 0x1F0), NULL);
    pBackgroundTile = &pBackground->tiles[0];
    SetTile(pBackgroundTile);
    setRGB0(pBackgroundTile, D_800594D4, D_800594D5, D_800594D6);
    SetSemiTrans(pBackgroundTile, 1);
    pBackgroundTile2 =  &pBackground->tiles[1];
    *pBackgroundTile2 = *pBackgroundTile;

    // Arrow
    pArrow = &g_FieldTextBoxes[index].continueArrow;
    rect.x = D_800ADF04.x;
    rect.y = D_800ADF04.y;
    rect.w = D_800ADF04.w;
    rect.h = D_800ADF04.h;
    SetDrawMode(&pArrow->drawModes[0], NULL, 0, GetTPage(0, 0, 0x298, 0x1C0), &rect);
    SetDrawMode(&pArrow->drawModes[1], NULL, 0, GetTPage(0, 0, 0x298, 0x1C0), &rect);
    SetSprt(&pArrow->sprites[0]);
    pArrowSprite = &pArrow->sprites[0];
    setRGB0(pArrowSprite, 0x80, 0x80, 0x80);
    setWH(pArrowSprite, 0xC, 0x8);
    pArrowSprite->clut = GetClut(0x100, 0xF6);
    pArrowSprite2 = &pArrow->sprites[1];
    setUV0(pArrowSprite, 0x80, 0xC0);
    setXY0(pArrowSprite, 0x0, 0x0);
    *pArrowSprite2 = *pArrowSprite;

    // Cursor
    pCursor = &g_FieldTextBoxes[index].cursor;
    rect.x = D_800ADEDC.x;
    rect.y = D_800ADEDC.y;
    rect.w = D_800ADEDC.w;
    rect.h = D_800ADEDC.h;
    SetDrawMode(&pCursor->drawModes[0], NULL, 0, GetTPage(0, 0, 0x288, 0x1C0), &rect);
    SetDrawMode(&pCursor->drawModes[1], NULL, 0, GetTPage(0, 0, 0x288, 0x1C0), &rect);
    SetSprt(&pCursor->sprites[0]);
    setRGB0(&pCursor->sprites[0], 0x80, 0x80, 0x80);
    pCursor->sprites[0].clut = GetClut(0x100, 0xF6);
    pCursorSprite2 = &pCursor->sprites[1];
    setWH(&pCursor->sprites[0], 0x8, 0xC);
    setUV0(&pCursor->sprites[0], 0x80, 0xC0);
    setXY0(&pCursor->sprites[0], 0x0, 0x0);
    *pCursorSprite2 = pCursor->sprites[0];
    g_FieldTextBoxes[index].continueArrowTimer = 2;

    // Borders
    pBorders = &g_FieldTextBoxes[index].borders;
    for (i = 0; i < 8; i++) {
        pWidth = &D_800ADE9C[i].w;
        pHeight = &D_800ADE9C[i].h;
        
        rect.x = D_800ADE9C[i].x;
        rect.y = D_800ADE9C[i].y;
        rect.w = *pWidth;
        rect.h = *pHeight;
        
        SetDrawMode(&pBorders->drawModes1[i], NULL, 0, GetTPage(0, 2, 0x280, 0x1F0), &rect);
        SetDrawMode(&pBorders->drawModes2[i], NULL, 0, GetTPage(0, 2, 0x280, 0x1F0), &rect);
        pBorderSprite1 = &pBorders->sprites1[i];
        SetSprt(pBorderSprite1);
        setRGB0(pBorderSprite1, 0x80, 0x80, 0x80);
        pBorderSprite1->clut = GetClut(0x100, 0xF4);
        SetSemiTrans(pBorderSprite1, 1);
        setUV0(pBorderSprite1, 0x80, 0xC0);
        setWH(pBorderSprite1, *pWidth, *pHeight);
        setXY0(pBorderSprite1, 0x0, 0x0);
        pBorderSprite2 =  &pBorders->sprites2[i];
        *pBorderSprite2 = *pBorderSprite1;
    }

    // Portrait
    pPortrait = &g_FieldTextBoxes[index].portrait;
    rect.y = 0;
    rect.x = 0;
    rect.h = 0xFF;
    rect.w = 0xFF;
    SetDrawMode(&pPortrait->drawModes[0], NULL, 0, GetTPage(1, 0, 0x2C0, 0x100), &rect);
    SetDrawMode(&pPortrait->drawModes[1], NULL, 0, GetTPage(1, 0, 0x2C0, 0x100), &rect);
    pPoly = &pPortrait->polys[0];
    SetPolyFT4(pPoly);
    setRGB0(pPoly, 0x80, 0x80, 0x80);
    pPoly->clut = GetClut(0, 0xE0);
    pPoly2 = &pPortrait->polys[1];
    pPoly->tpage = GetTPage(1, 0, 0x2C0, 0x100);
    *pPoly2 = *pPoly;
}

extern u8 D_800ADF34[];

void func_8007F5AC(s32 textBoxIndex, s32 faceDirection) {
    FieldTextBoxPortrait* pPortrait;
    u8* pUv;
    u8 u;
    u8 v;
    u16 clut;
    s32 i;

    pPortrait = &g_FieldTextBoxes[textBoxIndex].portrait;
    pUv = &D_800ADF34[faceDirection << 2];
    u = pUv[0];
    v = pUv[2];
    clut = GetClut(0, faceDirection + 0xE0);

    for (i = 0; i < 2; i++) {
        POLY_FT4* pPoly = &pPortrait->polys[i];

        pPoly->u0 = u;
        pPoly->v0 = v;
        pPoly->u1 = u + 0x40;
        pPoly->v1 = v;
        pPoly->u2 = u;
        pPoly->v2 = v + 0x40;
        pPoly->u3 = u + 0x40;
        pPoly->v3 = v + 0x40;
        pPoly->clut = clut;
    }
}

void func_8007F6F8(s16 index) {
    u8* pTextBox;
    u32 mask;

    if (g_FieldTextBoxes[index].visibility != 0) {
        return;
    }

    pTextBox = (u8*)&g_FieldTextBoxes[index];
    func_80034614(pTextBox + 0x18);
    func_800345E0(pTextBox + 0x18);
    func_800346D4(pTextBox + 0x18);

    g_FieldTextBoxes[index].cursor.visibility = -1;
    g_FieldTextBoxes[index].visibility = -1;
    g_FieldTextBoxes[index].status = -1;
    g_FieldTextBoxes[index].order = 0xFFFF;
    D_800B068C[index] = -1;
    mask = 1 << index;
    D_800B2174[0] &= (u16)(mask ^ 0xFF);
    g_FieldTextBoxes[index].ownerActorID = 0xFF;
    g_FieldTextBoxes[index].unk412 = 0;
}

// Project (0, Y, 0) from actor's model/local space to screen
void func_8007F814(s32 actorIndex, s32* screenX, s32* screenY, s32 yOffset) {
    MATRIX matrix;
    SVECTOR localPos;
    long screenXY;
    long depth;
    long flag;

    CompMatrix(&g_Scene.worldToScreenMatrix, &g_FieldActors[actorIndex].transformMatrix, &matrix);
    SetRotMatrix(&matrix);
    SetTransMatrix(&matrix);

    localPos.vx = 0;
    localPos.vy = yOffset;
    localPos.vz = 0;
    RotTransPers(&localPos, &screenXY, &depth, &flag);

    *screenX = (s16)screenXY;
    *screenY = (s16)(screenXY >> 16);
}

s32 func_8007F8DC(s32 x, s32 y, s32 stringIndex, s32 textBoxIndex, s32 width, s32 height,
                  s32 ownerActorIndex, s32 talkingActorIndex, s32 mode, s32 orientationFlags,
                  s32 dialogFlags) {
    u8* pTextBox;
    ActorData* pTalkingActor;
    s32 flags;
    s32 slot;
    s32 i;
    s32 targetX;
    s32 targetY;
    s32 boxPixelWidth;
    s32 boxPixelHeight;
    s32 boxOffset;
    s32 openTimer;
    s32 portraitFlags;

    pTalkingActor = (ActorData*)(uintptr_t)g_FieldActors[talkingActorIndex].pActorData;
    flags = pTalkingActor->dialogFlags >> 16;
    if (flags == 0) {
        flags = pTalkingActor->dialogFlags & 0xFFFF;
    }
    flags |= orientationFlags;

    slot = D_800ADE90 & 3;
    for (i = 0; i < 4; i++) {
        slot = D_800ADE90 & 3;
        D_800ADE90++;
        if (D_800B068C[slot] == -1) {
            D_800B068C[slot] = 0;
            break;
        }
    }

    pTextBox = (u8*)&g_FieldTextBoxes[textBoxIndex];
    *(s32*)(pTextBox + 0x88) = FieldScriptVMGetVariableValue(0x16);
    *(s32*)(pTextBox + 0x8C) = FieldScriptVMGetVariableValue(0x18);
    *(s32*)(pTextBox + 0x90) = FieldScriptVMGetVariableValue(0x1A);
    *(s32*)(pTextBox + 0x94) = FieldScriptVMGetVariableValue(0x1C);
    *(s16*)(pTextBox + 0x98) = *(s32*)(pTextBox + 0x94);

    if (mode == 2) {
        targetX = 0xA0;
        targetY = y + 0x20;
    } else if (mode == 3) {
        targetX = x + 8 + width * 2;
        targetY = y + 8 + height * 7;
    } else {
        func_8007F814(talkingActorIndex, &targetX, &targetY, -0x40);
    }

    if (pTalkingActor->faceId != 0xFF) {
        if ((flags & 0x402) == 0) {
            func_8007F5AC(textBoxIndex, ((pTalkingActor->direction << 1) & 0xE) | 1);
        } else {
            func_8007F5AC(textBoxIndex, (pTalkingActor->direction << 1) & 0xE);
        }
        g_FieldTextBoxes[textBoxIndex].portrait.shouldRenderPortrait = 1;
        g_FieldTextBoxes[textBoxIndex].portrait.portraitID = pTalkingActor->faceId;
    } else {
        g_FieldTextBoxes[textBoxIndex].portrait.shouldRenderPortrait = 0;
        g_FieldTextBoxes[textBoxIndex].portrait.portraitID = 0x80;
    }

    boxPixelWidth = width * 4 + 0x10;
    boxPixelHeight = height * 14 + 0x10;
    g_FieldTextBoxes[textBoxIndex].cursor.visibility = -1;
    func_8007E114(textBoxIndex, x, y, boxPixelWidth, boxPixelHeight);

    portraitFlags = 0;
    if (pTalkingActor->faceId != 0xFF && (flags & 0x402) == 0) {
        portraitFlags = 0x44;
    }

    /* Asm 8007FCB4-8007FCE0: separate lhu of D_800ADF54 / D_800ADF56 at
     * textBoxIndex*4. Window width is DialogGetWidth as-is (width*2+8 is
     * only for boxOffset below). */
    func_80032F54(pTextBox + 0x18, *(&D_800ADF54 + textBoxIndex * 2),
                  *(&D_800ADF56 + textBoxIndex * 2), x + portraitFlags + 8, y + 8, width, mode,
                  height);

    if (flags & 0x400) {
        g_FieldTextBoxes[textBoxIndex].flags |= 0x20;
    }

    /* Asm 8007FD10-8007FD30: sb speed at box+0x80 (window+0x68). 1 when
     * D_800B21D6 == 8, else 2. `_pad[0x68]` is a short index and missed. */
    *(u8*)(pTextBox + 0x80) = (D_800B21D6 == 8) ? 1 : 2;
    /* XENO_PC_PORT: the string-entry pointer at 0xA8 is a 32-bit field on PSX;
     * storing a native 64-bit pointer here spills into 0xAC/0xAE (the box's
     * border x/y set by func_8007E114), collapsing the box to (0,0). Store only
     * the low 32 bits (port heap is in low 4GB). Match-preserving: the casts are
     * no-ops on 32-bit PSX. Paired read at the func_80034714 call below. */
    *(u32*)(pTextBox + 0xA8) = (u32)(uintptr_t)GetStringEntry(D_800ADBF0, stringIndex);
    g_FieldTextBoxes[textBoxIndex].visibility = 0;
    *(u16*)(pTextBox + 0x28) |= 2;
    g_FieldTextBoxes[textBoxIndex].ownerActorID = ownerActorIndex;
    g_FieldTextBoxes[textBoxIndex].talkingActorID = talkingActorIndex;
    g_FieldTextBoxes[textBoxIndex].windowOpenTimer = D_800B21D6;
    g_FieldTextBoxes[textBoxIndex].unk412 = (dialogFlags & 0x800) ? 1 : 0;

    boxOffset = width * 2 + 8;
    boxPixelHeight = height * 7 + 8;
    g_FieldTextBoxes[textBoxIndex].positionOffsetX = (targetX - boxOffset - x) << 16;
    g_FieldTextBoxes[textBoxIndex].positionOffsetY = (targetY - boxPixelHeight - y) << 16;

    if (dialogFlags & 0x100) {
        g_FieldTextBoxes[textBoxIndex].windowOpenTimer = 1;
        g_FieldTextBoxes[textBoxIndex].positionOffsetDeltaX = -g_FieldTextBoxes[textBoxIndex].positionOffsetX;
        g_FieldTextBoxes[textBoxIndex].positionOffsetDeltaY = -g_FieldTextBoxes[textBoxIndex].positionOffsetY;
    } else {
        openTimer = D_800B21D6;
        g_FieldTextBoxes[textBoxIndex].positionOffsetDeltaX = -g_FieldTextBoxes[textBoxIndex].positionOffsetX / openTimer;
        g_FieldTextBoxes[textBoxIndex].positionOffsetDeltaY = -g_FieldTextBoxes[textBoxIndex].positionOffsetY / openTimer;
    }

    if ((pTalkingActor->flags & 0x200) && !(flags & 1)) {
        g_FieldTextBoxes[textBoxIndex].status = 0;
        return -1;
    }

    return 0;
}

void func_8007FFE8(void) {
    int i;

    for (i = 0; i < 4; i++) {
        if (!g_FieldTextBoxes[i].visibility) {
            func_8007F6F8((s16) i); // TODO: Clean up cast w/ function sig
        }
    }
}

void func_8008004C(void* ot, s32 renderContextIndex) {
    s32 skipIndex;
    s32 orders[4];
    s32 order;
    s32 i;

    D_800ADE98++;
    if ((D_800ADE98 & 3) == 0) {
        D_800ADE94++;
    }
    if (D_800ADE94 >= 5) {
        D_800ADE94 = 0;
    }

    skipIndex = 0xFF;
    for (i = 0; i < 4; i++) {
        if (*(s16*)((u8*)&g_FieldTextBoxes[i] + 0x412) != 0) {
            skipIndex = i;
        }
    }

    for (i = 0; i < 4; i++) {
        orders[i] = 0xFFFF;
    }

    for (order = 0; order < 4; order++) {
        for (i = 0; i < 4; i++) {
            u8* pTextBox = (u8*)&g_FieldTextBoxes[i];
            u8* pWindow = pTextBox + 0x18;

            if (*(u16*)(pTextBox + 0x410) != order) {
                continue;
            }

            orders[i] = order;
            if (*(s16*)(pTextBox + 0x40E) != 0 || i == skipIndex) {
                continue;
            }

            *(s16*)(pTextBox + 0x3C4) = -1;
            if (*(s16*)(pTextBox + 0x408) == 0) {
                if ((D_800C2694 & 0x20) != 0 && order == 0) {
                    s32 actorIndex = *(s16*)(pTextBox + 0x416);
                    ActorData* pActor = (ActorData*)(uintptr_t)g_FieldActors[actorIndex].pActorData;

                    *(s16*)(pTextBox + 0x37C) = -1;
                    pActor->unk81 = *(u8*)(pTextBox + 0x382) + *(u8*)(pTextBox + 0x37E);
                    func_800345E0(pWindow);
                }

                if (*(s16*)(pWindow + 0x82) == 0) {
                    /* XENO_PC_PORT: paired 32-bit read of the 0xA8 string pointer
                     * (see the store in func_8007F8DC). */
                    func_80034714(pWindow, (void*)(uintptr_t)*(u32*)(pTextBox + 0xA8));
                }

                func_80034888(pWindow, ot, renderContextIndex);
                if (func_80033CD0(pWindow) != 0) {
                    if (*(s16*)(pTextBox + 0x37C) == 0) {
                        continue;
                    }
                    *(s16*)(pTextBox + 0x3C4) = 0;
                }
            }

            FieldTextBoxLinkPrim(ot, (u8*)pTextBox + renderContextIndex * sizeof(DR_MODE));
            func_8007E1C0(ot, renderContextIndex, i);
            func_8007DCF8(i, ot, renderContextIndex);
        }
    }

    for (i = 0; i < 4; i++) {
        g_FieldTextBoxes[i].order = orders[i];
    }

    FieldTextBoxLinkPrim(ot, D_800B1DF4 + g_FieldCurRenderContextIndex * 0xC0);
}

void func_800805F4(void) {
    int i;

    for (i = 0; i < 4; i++) {
        if (!g_FieldTextBoxes[i].visibility) {
            // TODO: _pad[0x8] is probably a short holding some flags
            if (g_FieldTextBoxes[i].windowOpenTimer == 0 && !(g_FieldTextBoxes[i]._pad[0x8] & 4)) {
                func_8007F6F8( (i*0x10000) >> 0x10);
            }
            if (g_FieldTextBoxes[i].status == 0) {
                func_8007F6F8( (i*0x10000) >> 0x10);
            }
            if (g_FieldTextBoxes[i].windowOpenTimer) {
                g_FieldTextBoxes[i].windowOpenTimer--;
            }
        }
    }
}

// Get index of textbox with order 0
int func_800806E4(void) {
    int i;
    
    for (i = 0; i < 4; i++) {
        if (g_FieldTextBoxes[i].order == 0) {
            return i;
        }
    }
    return TEXT_BOX_UNINITIALIZED;
}


// Is textbox free to use?
int func_80080720(void) {
    int i;

    for (i = 0; i < 4; i++) {
        if (g_FieldTextBoxes[i].order == TEXT_BOX_UNINITIALIZED) {
            return 0;
        }
    }
    return -1;
}

// Get index of textbox with lowest order
int func_80080760(void) {
    int i;
    int nResult;
    int lowestOrder;
    
    lowestOrder = 0; // 0 = Top
    nResult = 0xFFFF;
    
    for (i = 0; i < 4; i++) {
        if (g_FieldTextBoxes[i].order != 0xFFFF && g_FieldTextBoxes[i].order >= lowestOrder) {
            lowestOrder = g_FieldTextBoxes[i].order;
            nResult = i;
        }
    }
    return nResult;
}

// Update textbox order
int func_800807B4(void) {
    int i;

    for (i = 0; i < 4; i++) {
        if (g_FieldTextBoxes[i].order != TEXT_BOX_UNINITIALIZED) {
            g_FieldTextBoxes[i].order++;
        }
    }

    for (i = 0; i < 4; i++) {
        if (g_FieldTextBoxes[i].order == TEXT_BOX_UNINITIALIZED) {
            g_FieldTextBoxes[i].order = 0;
            return i;
        }
    }

    return TEXT_BOX_UNINITIALIZED;
}
