#include "common.h"

#include "psyq/libgpu.h"
#include "system/controller.h"
#include "system/debug.h"
#include "system/math.h"
#include "system/menu.h"
#include "main/game.h"

// Confirmation window choices
#define MENU_CHOICE_NO 0
#define MENU_CHOICE_YES 1

// Shop mode choices
#define MENU_CHOICE_EXIT 0x0
#define MENU_CHOICE_SELL 0x1
#define MENU_CHOICE_BUY 0x2

// Sell mode menu choice
#define MENU_CHOCIE_EQUIPMENT 0x0
#define MENU_CHOICE_ACCESSORIES 0x1
#define MENU_CHOICE_WEAPONS 0x2
#define MENU_CHOICE_ITEMS 0x3

#define MENU_STAT_CHANGE_INCREASE 0x0
#define MENU_STAT_CHANGE_DECREASE 0x1

#define SHOP_DATA_INITIALIZE 0x0
#define SHOP_DATA_FREE 0x10

// Selection modes
#define MENU_AUTO_ADVANCE 0
#define MENU_MANUAL_CHOICE 0xFF

// Max items to display in the window at a time
#define MAX_ITEMS_IN_VIEW 8

// Debug-related?
extern s32* D_8005917C;

extern int D_801D1F50; // Number of items
extern s32 D_801D1FD0[];
extern s32 D_801D1FD4[];
extern s32 D_801D1FE8[];
extern u16 D_801D2260;


extern s32 D_801D1F54[];
// = {
//    MENU_TEX_BALL_CURSOR_1, MENU_TEX_STRING_BUY,
//    MENU_TEX_BALL_CURSOR_2, MENU_TEX_STRING_SELL,
//    MENU_TEX_BALL_CURSOR_3, MENU_TEX_STRING_EXIT
//    }

extern s32 D_801D1FCC[];
extern s32 D_801D1FD8[];

extern u_short D_801D21F0[];

extern s32 D_801D1FCC[];

extern s32 D_801D2248;
extern s32 D_801D224C;
extern s32 D_801D2250;
extern s32 D_801D2254;
extern s32 D_801D2258;
extern s32 D_801D225C;

// Character portrait X positions
extern int D_801D21CC[];

// Cursor stuff
extern int D_801D201C[];
extern int D_801D2094[];
extern int D_801D2114[];

extern int D_801D1FF8[];
extern int D_801D2008[];

// Argument passed to the menu by the outside.
// For the shop menu, this is expected to be the index
// of the shop to load and run.
extern u8 D_80059171;

extern s32 D_801D1F50;


extern int D_801D2194[];
extern int D_801D21B0[];

// Explanations stuff
extern u_char D_801D2210[]; // Buy menu explanation textures
// = { MENU_TEX_QUANTITY_EXPLANATION, 
//     MENU_TEX_STRING_ATTACK, 
//     MENU_TEX_STRING_DEFENSE, 
//     MENU_TEX_MINUS 
//    }

extern u_char D_801D2214[]; // Sell menu explanation textures
// = { MENU_TEX_QUANTITY_EXPLANATION, MENU_TEX_PLUS }

extern int D_801D2218[]; // Buy menu explanation graphics X positions
extern int D_801D2228[]; // Sell menu explanation graphics X positions
extern int D_801D2230[]; // Buy menu explanation graphics Y positions
extern int D_801D2240[]; // Sell menu explanation graphics Y positions

u_char ShopMenuBuyMenu(void);


void func_801C5040(POLY_FT4* pPoly, short x, short y, u_char u, u_char v, short width, short height) {
    setXY4(
        pPoly,
        x,         y,
        x + width, y,
        x,         y + height,
        x + width, y + height
    );

    setUV4(
        pPoly,
        u,         v,
        u + width, v,
        u,         v + height,
        u + width, v + height
    );
}

u_short ShopMenuIsCharacterFlagSet(u_short value, u_char maskIndex) {
    return D_801D21F0[maskIndex] & value;
}

u_short ShopMenuGetCharacterBitMask(int maskIndex) {
    return D_801D21F0[maskIndex & 0xFF];
}

void ShopMenuParseNumberToString(u_int number) {
    int i;
    unsigned int curValue;

    // 10 ** 8
    curValue = 100000000;
    
    for (i = 0; i < MENU_MAX_DIGITS; i++) {
        g_Menu->digits[i] = number / curValue;
        number %= curValue;
        curValue /= 10;
    }
    
    for (i = 1; i < MENU_MAX_DIGITS; i++) {
        if (g_Menu->digits[i]) {
            if (g_Menu->digits[i - 1] == 0) {
                g_Menu->digits[i - 1] = 0xFF;
            }
            break;
        }
        g_Menu->digits[i - 1] = 0xFF;
    }
}

void ShopMenuUnk32CManager(u_char isInitialization) {
    if (isInitialization) {
        g_Menu->unk32C = HeapAlloc(sizeof(MenuUnk2), 0);
        bzero(g_Menu->unk32C, sizeof(MenuUnk2));
        return;
    }
    HeapFree(g_Menu->unk32C);
}

void ShopMenuSetManager(u_char isInitialization) {
    if (isInitialization) {
        g_Menu->pManager = HeapAlloc(sizeof(MenuManager), 0);
        bzero(g_Menu->pManager, sizeof(MenuManager));
        return;
    }
    HeapFree(g_Menu->pManager);
}

void ShopMenuSelectionMenuManager(u_char isInitialization) {
    if (isInitialization) {
        g_Menu->pSelectionMenu = HeapAlloc(sizeof(MenuSelectionMenu), 0);
        bzero(g_Menu->pSelectionMenu, sizeof(MenuSelectionMenu));
        return;
    }
    HeapFree(g_Menu->pSelectionMenu);
}

void ShopMenuUnk354Manager(u_char isInitialization) {
    if (isInitialization) {
        g_Menu->unk354 = HeapAlloc(sizeof(MenuUnk5), 0);
        bzero(g_Menu->unk354, sizeof(MenuUnk5));
        return;
    }
    HeapFree(g_Menu->unk354);
}

void ShopMenuUnk330Manager(u_char isInitialization) {
    if (isInitialization) {
        g_Menu->unk330 = HeapAlloc(sizeof(MenuUnk6), 0);
        bzero(g_Menu->unk330, sizeof(MenuUnk6));
        return;
    }
    HeapFree(g_Menu->unk330);
}

void ShopMenuUnk348Manager(u_char isInitialization) {
    if (isInitialization) {
        g_Menu->unk348 = HeapAlloc(sizeof(MenuUnk1), 0);
        bzero(g_Menu->unk348, sizeof(MenuUnk1));
        return;
    }
    HeapFree(g_Menu->unk348);
}

void ShopMenuUnk1E20Manager(u_char isInitialization) {
    if (isInitialization) {
        g_Menu->unk1E20 = HeapAlloc(sizeof(MenuUnk7), 0);
        bzero(g_Menu->unk1E20, sizeof(MenuUnk7));
        return;
    }
    HeapFree(g_Menu->unk1E20);
}

void ShopMenuShopManager(u_char isInitialization) {
    if (isInitialization) {
        g_Menu->pShop = HeapAlloc(sizeof(MenuShop), 0);
        bzero(g_Menu->pShop, sizeof(MenuShop));
        return;
    }
    HeapFree(g_Menu->pShop);
}

INCLUDE_ASM("asm/shop_menu/nonmatchings/main/misc", ShopMenuLoadResources);

void ShopMenuInitialize(void) {
    int flags;
    int characterID;
    int i;

    g_Menu->menu1Choice = 4;
    g_Menu->unk337 = 0xFF;
    g_Menu->unk326 = 0x3C;
    g_Menu->unk334 = 0;
    g_Menu->unk335 = 0;
    
    flags = (g_GameState.unk1D30 & g_GameState.FrMask) & 0x7FF;
    for (i = 0; i < 0x10; i++) {
        if (ShopMenuIsCharacterFlagSet(flags, i)) {
            g_Menu->availableCharacters[i] = TRUE;
        } else {
            g_Menu->availableCharacters[i] = FALSE;
        }
    }
    
    for (i = 0; i < MAX_PARTY_MEMBERS; i++) {
        characterID = g_GameState.partyMembers[i];
        if (characterID != 0xFF && g_Menu->availableCharacters[characterID]) {
            g_Menu->pManager->currentCharacterIDs[i] = characterID;
        } else {
            g_Menu->pManager->currentCharacterIDs[i] = 0xFF;
        }
    }
    
    ShopMenuLoadResources();
}

void ShopMenuResetRenderContext(void) {
    g_Menu->renderContext = 0;
}

INCLUDE_ASM("asm/shop_menu/nonmatchings/main/misc", func_801C5A7C);

INCLUDE_ASM("asm/shop_menu/nonmatchings/main/misc", func_801C5CBC);

void func_801C5E6C(void) {
    RECT rect;
    u_short* pVramData;
    u8 _unused[0x8];

    pVramData = HeapAlloc(0x20, 0);
    bzero(pVramData, 0x20);
    pVramData[1] = 0x7FFF;
    rect.y = 448;
    rect.w = 16;
    rect.x = 0;
    rect.h = 1;
    LoadImage(&rect, pVramData);
    DrawSync(0);
    HeapFree(pVramData);
}

extern void func_801C5CBC(void* a0, void* a1, s32 a2, s32 a3);
extern u8 D_801D2018[];

void func_801C5EE8(void) {
    SystemTransferPaletteToVRAM(0, 0x1D1);
    *(void**)((u8*)g_Menu + 0x558) = HeapAlloc(0x38E, 0);
    func_801C5CBC((u8*)g_Menu + 0x4E0, D_801D2018, 0, 4);
    func_801C5E6C();
}

void ShopMenuInitializeWindowBorders(void) {
    POLY_FT4 _unusued;
    
    func_80026338(
        g_Menu->unk2DC, MENU_TEX_WINDOW_BORDER_TOP, 
        &g_Menu->unk46C, 
        &g_Menu->texPage0, 
        &g_Menu->clutX0, &g_Menu->clutY0, 
        &g_Menu->texPageX0, &g_Menu->texPageY0
    );
    func_80026338(
        g_Menu->unk2DC, MENU_TEX_WINDOW_BORDER_BOTTOM, 
        &g_Menu->unk484, 
        &g_Menu->texPage1, 
        &g_Menu->clutX1, &g_Menu->clutY1, 
        &g_Menu->texPageX1, &g_Menu->texPageY1
    );
    func_80026338(
        g_Menu->unk2DC, MENU_TEX_WINDOW_BORDER_LEFT, 
        &g_Menu->unk49C, 
        &g_Menu->texPage2, 
        &g_Menu->clutX2, &g_Menu->clutY2, 
        &g_Menu->texPageX2, &g_Menu->texPageY2
    );
    func_80026338(
        g_Menu->unk2DC, MENU_TEX_WINDOW_BORDER_RIGHT, 
        &g_Menu->unk4B4, 
        &g_Menu->texPage3, 
        &g_Menu->clutX3, &g_Menu->clutY3, 
        &g_Menu->texPageX3, &g_Menu->texPageY3
    );
}

void ShopMenuMovePointerCursor(int index, u_char arg1) {
    func_8002675C(
        g_Menu->unk2DC, MENU_TEX_POINTER_CURSOR, 
        &g_Menu->unk348->polysPointerCursor, g_Menu->renderContext, 
        D_801D2194[index], D_801D21B0[index], 
        0x1000
    );
    g_Menu->unk348->cursorRenderContext = g_Menu->renderContext;
    
    if (arg1) {
        // A yellow -> black shaded rectangle of unk15B width, 
        // with green lines around it. Seems to be cursor-related,
        // as the pointer cursor is set to same position
        setXY4(
            &g_Menu->unk348->polyG4s[g_Menu->renderContext],
            D_801D2194[index] + 20, D_801D21B0[index] - 36,
            D_801D2194[index] + (g_Menu->unk348->unk15B + 20), D_801D21B0[index] - 36,
            D_801D2194[index] + 20, D_801D21B0[index] - 20,
            D_801D2194[index] + (g_Menu->unk348->unk15B + 20), D_801D21B0[index] - 20
        );

        setXY3(
            &g_Menu->unk348->lines1[g_Menu->renderContext],
            D_801D2194[index] + 20, D_801D21B0[index] - 36,
            D_801D2194[index] + (g_Menu->unk348->unk15B + 20), D_801D21B0[index] - 36,
            D_801D2194[index] + (g_Menu->unk348->unk15B + 20), D_801D21B0[index] - 20
        );

        setXY3(
            &g_Menu->unk348->lines2[g_Menu->renderContext],
            D_801D2194[index] + 20, D_801D21B0[index] - 36,
            D_801D2194[index] + 20, D_801D21B0[index] - 20,
            D_801D2194[index] + (g_Menu->unk348->unk15B + 20), D_801D21B0[index] - 20
        );
        
        g_Menu->unk348->unk159 = g_Menu->renderContext;
        g_Menu->pManager->unk3 = 1;
    }
}

void func_801C6430(void) {
    g_Menu->pManager->unk4 = 0;
    g_Menu->pManager->unk3 = 0;
}

void func_801C6460(POLY_G4* pPoly, u_char red, u_char green, u_char blue) {
    SetPolyG4(pPoly);
    setRGB0(pPoly, red, green, blue);
    setRGB1(pPoly, red, green, blue);
    setRGB2(pPoly, 0, 0, 0);
    setRGB3(pPoly, 0, 0, 0);
}

void ShopMenuInitializeBackgrounds(void) {
    RECT rect;
    int i;

    rect.y = 0;
    rect.x = 0;
    rect.h = 256;
    rect.w = 256;
    
    func_801C6430();

    for (i = 0; i < 2; i++) {
        // Yellow -> Black gradient
        func_801C6460(&g_Menu->unk348->polyG4s[i], 128, 128, 0);
        SetSemiTrans(&g_Menu->unk348->polyG4s[i], 1);
        
        // Green lines
        SetLineF3(&g_Menu->unk348->lines1[i]);
        setRGB0(&g_Menu->unk348->lines1[i], 0, 64, 0);
        
        SetLineF3(&g_Menu->unk348->lines2[i]);
        setRGB0(&g_Menu->unk348->lines2[i], 0, 64, 0);
        
        // Backgrounds dimming screen
        SetPolyF4(&g_Menu->unk348->polysDimEffect[i]);
        setXY4(&g_Menu->unk348->polysDimEffect[i],
            0,   0,
            320, 0,
            0,   224,
            320, 224
        );
        setRGB0(&g_Menu->unk348->polysDimEffect[i], 128, 128, 128);
        SetSemiTrans(&g_Menu->unk348->polysDimEffect[i], 1);

        SetDrawMode(&g_Menu->unk348->drModes1[i], 0, 0, GetTPage(0, 0, 0x140, 128), &rect);
        SetDrawMode(&g_Menu->unk348->drawModeDimEffect[i], 0, 0, GetTPage(0, 2, 0x180, 0), &rect);
    }
}

void ShopMenuLoadShopItemsData(u_char mode) {
    u_int* pArchive;

    if (mode < SHOP_DATA_FREE) {
        pArchive = HeapAlloc(ArchiveDecodeAlignedSize(2), 1);
        ArchiveReadFileToBuffer(2, pArchive, 0, 0x80);
        ArchiveCdDataSync(0);
        ResolveArchiveEntryPointers(pArchive);
    }

    switch (mode) {
        case SHOP_DATA_INITIALIZE:
            // Shop item definitions
            g_Menu->unk330->pWeaponsData = LZSSHeapDecompress(pArchive[2], 0);
            g_Menu->unk330->pAccessoriesData = LZSSHeapDecompress(pArchive[3], 0);
            g_Menu->unk330->pItemsData = LZSSHeapDecompress(pArchive[1], 0);

            // Shop item descriptions
            g_Menu->pShop->pWeaponDescriptions = LZSSHeapDecompress(pArchive[40], 0);
            g_Menu->pShop->pAccessoryDescriptions = LZSSHeapDecompress(pArchive[41], 0);
            g_Menu->pShop->pItemDescriptions = LZSSHeapDecompress(pArchive[42], 0);       
            break;
        case SHOP_DATA_FREE:
            HeapFree(g_Menu->unk330->pWeaponsData);
            HeapFree(g_Menu->unk330->pAccessoriesData);
            HeapFree(g_Menu->unk330->pItemsData);

            HeapFree(g_Menu->pShop->pWeaponDescriptions);
            HeapFree(g_Menu->pShop->pAccessoryDescriptions);
            HeapFree(g_Menu->pShop->pItemDescriptions);
    }

    if (mode < SHOP_DATA_FREE) {
        HeapFree(pArchive);
    }
}

void ShopMenuInitializeShopData(void) {
    int i;
    int j;
    u_char* pShopItemData;
    
    j = 0;

    // Data for the items the shop we're loading is carrying
    pShopItemData = &g_Menu->pShopEntries[D_80059171 * 0x5C];
    
    for (i = 0; i < MAX_SHOP_ITEMS; i++) {
        g_Menu->shopItemIDs[i] = 0;
        g_Menu->shopItemTypes[i] = 0;
    }

    // 0x5A ??? Arrays being set are 0x30 sized
    // NOTE: Potentially danger if there's more than 0x30 valid items in the shop item data
    for (i = 0; i < 0x5A; i++) {
        if (pShopItemData[i]) {
            g_Menu->shopItemIDs[j] = pShopItemData[i];
            g_Menu->shopItemTypes[j] = i / 30;
            j++;
        }
    }
    D_801D1F50 = j;
    
    ShopMenuLoadShopItemsData(SHOP_DATA_INITIALIZE);

    // Red highlight around character portraits
    for (i = 0; i < 9; i++) {
        for (j = 0; j < 2; j++) {
            SetLineF3(&g_Menu->pShop->linesPortraitHighlight1[i*2 + j]);
            setRGB0(&g_Menu->pShop->linesPortraitHighlight1[i*2 + j], 255, 0, 0);
            
            SetLineF3(&g_Menu->pShop->linesPortraitHighlight2[i*2 + j]);
            setRGB0(&g_Menu->pShop->linesPortraitHighlight2[i*2 + j], 255, 0, 0);

            setXY3(
                &g_Menu->pShop->linesPortraitHighlight1[i*2 + j],
                D_801D21CC[i], 0xA6,
                D_801D21CC[i] + 0x18, 0xA6,
                D_801D21CC[i] + 0x18, 0xBC
            );

            setXY3(
                &g_Menu->pShop->linesPortraitHighlight2[i*2 + j],
                D_801D21CC[i], 0xA6,
                D_801D21CC[i], 0xBC,
                D_801D21CC[i] + 0x18, 0xBC
            );
        }
        
        g_Menu->pShop->unk469C[i] = 0;
    }

    // White line between gold amounts?
    for (j = 0; j < 2; j++) {
        SetLineF2(&g_Menu->pShop->lines3BF0[j]);
        setRGB0(&g_Menu->pShop->lines3BF0[j], 255, 255, 255);
        setXY2(
            &g_Menu->pShop->lines3BF0[j],
            D_801D2250 - 8, D_801D2254 + 9,
            D_801D2250 + 0x4E, D_801D2254 + 9
        );
    }
}

// Set vertices relative to center of screen?
void ShopMenuSetVertices(SVECTOR* pVertices, u_short x, u_short y, u_short width, u_short height) {
    pVertices[0].vx = x - 160;
    pVertices[0].vy = y - 112;
    pVertices[0].vz = 0;
    
    pVertices[1].vx = x + width - 160;
    pVertices[1].vy = y - 112;
    pVertices[1].vz = 0;
    
    pVertices[2].vx = x - 160;
    pVertices[2].vy = y + height - 112;
    pVertices[2].vz = 0;
    
    pVertices[3].vx = x + width - 160;
    pVertices[3].vy = y + height - 112;
    pVertices[3].vz = 0;
}

void ShopMenuSetWindowBorderPrimitive(POLY_FT4* pPrim) {
    SetSemiTrans(pPrim, 1);
    SetShadeTex(pPrim, 0);
    setRGB0(pPrim, 128, 128, 128);
}

// scrollOffset refers to the index we're currently scrolled down at
void ShopMenuUpdateScrollBarHandle(int x, int y, int scrollHandleHeight, int numItems, int scrollOffset) {
    int yOffset = 0;
    
    if (!g_Menu->pManager->scrollHandleActive) {
        g_Menu->pScrollHandle = HeapAlloc(sizeof(MenuScrollBarHandle), 0x0);
        bzero(g_Menu->pScrollHandle, sizeof(MenuScrollBarHandle));
    }
    
    // If the total number of items in the window fits within what we can view,
    // there's no need for scrolling so we set the scroll handle height to fill
    // the entire scroll bar.
    if (numItems <= MAX_ITEMS_IN_VIEW) {
        scrollHandleHeight = 100;
    }
    // Compute the Y offset based on our current scroll offset
    else {
        yOffset = (scrollOffset * 100) / (numItems - MAX_ITEMS_IN_VIEW);
        yOffset = (yOffset * 4000) / 10000;
    }

    func_8002675C(g_Menu->unk2DC, MENU_TEX_SCROLL_BAR_HANDLE, 
        g_Menu->pScrollHandle->polys, g_Menu->renderContext, 
        x, y, 0x1000
    );
    
    ShopMenuSetVertices(
        g_Menu->pScrollHandle->vertices, 
        x, y + yOffset, 
        8, scrollHandleHeight
    );
    
    g_Menu->pScrollHandle->renderContext = g_Menu->renderContext;
    g_Menu->pManager->scrollHandleActive = TRUE;
}

void ShopMenuFreeScrollBarHandle(void) {
    g_Menu->pManager->scrollHandleActive = FALSE;
    HeapFree(g_Menu->pScrollHandle);
}

void ShopMenuInitializeArrowCursor(u_char index) {
    g_Menu->arrowCursors[index] = HeapAlloc(sizeof(MenuArrowCursor), 0x0);
    bzero(g_Menu->arrowCursors[index], sizeof(MenuArrowCursor));
    g_Menu->arrowCursors[index]->curAnimFrame = 4;
    g_Menu->arrowCursors[index]->animFrameDuration = 0;
}

void ShopMenuUpdateArrowCursor(int selectedIndex, int scrollOffset, u_char arg2, u_char cursorIndex) {
    int yOffset;
    MenuArrowCursor* pArrowCursor;
    POLY_FT4* pPoly;

    scrollOffset = 0;
    
    pArrowCursor = g_Menu->arrowCursors[cursorIndex];
    
    // Update animation
    if (++pArrowCursor->animFrameDuration >= 6) {
        pArrowCursor->curAnimFrame--;
        if (pArrowCursor->curAnimFrame < 0) {
            pArrowCursor->curAnimFrame = 4;
        }
        pArrowCursor->animFrameDuration = 0;
    }
    
    if (!arg2) {
        yOffset = (selectedIndex * FONT_LETTER_HEIGHT) + 50;
    }
    
    if (scrollOffset != 1) {
        func_8002675C(
            g_Menu->unk2DC, pArrowCursor->curAnimFrame + MENU_TEX_ARROW_CURSOR, 
            pArrowCursor->polys, g_Menu->renderContext,
            0, 0, 0x1000
        );
        
        pPoly = &pArrowCursor->polys[g_Menu->renderContext];

        ShopMenuSetVertices(
            pArrowCursor->vertices, 
            pPoly->x0 + 28, pPoly->y0 + yOffset, 
            pPoly->x1 - pPoly->x0, pPoly->y3 - pPoly->y0
        );
        pArrowCursor->renderContext = g_Menu->renderContext;
        g_Menu->pManager->shouldRenderArrowCursor[cursorIndex] = TRUE;
        return;
    }
    
    g_Menu->pManager->shouldRenderArrowCursor[cursorIndex] = FALSE;
}

void ShopMenuFreeArrowCursor(u_char index) {
    HeapFree(g_Menu->arrowCursors[index]);
    g_Menu->pManager->shouldRenderArrowCursor[index] = FALSE;
}

void ShopMenuInitializeWindowGraphics(u_char index) {
    RECT rect;
    MenuWindow* pWindow;
    u_char i;

    pWindow = g_Menu->windows[index];
    
    rect.y = 0;
    rect.x = 0;
    rect.h = 256;
    rect.w = 256;
    
    g_Menu->pManager->shouldRenderWindow[index] = FALSE;
    g_Menu->pManager->unk27[index] = FALSE;
    
    // Window background
    for (i = 0; i < 2; i++) {
        SetPolyG4(&pWindow->polysBackground[i]);
        setRGB0(&pWindow->polysBackground[i], 104, 104, 104);
        setRGB1(&pWindow->polysBackground[i], 104, 104, 104);
        setRGB2(&pWindow->polysBackground[i], 104, 104, 104);
        setRGB3(&pWindow->polysBackground[i], 104, 104, 104);
        SetSemiTrans(&pWindow->polysBackground[i], 1);
        SetDrawMode(
            &pWindow->drawModes[i], 
            0, 0, 
            GetTPage(0, 0, g_Menu->texPageX0, g_Menu->texPageY0), 
            &rect
        );
    }
    
    // Window borders
    for (i = 0; i < 4; i++) {
        SetPolyFT4(&pWindow->polysWindowBorderTop[i]);
        SetShadeTex(&pWindow->polysWindowBorderTop[i], 1);
        setRGB0(&pWindow->polysWindowBorderTop[i], 0xFF, 0xFF, 0xFF);
        pWindow->polysWindowBorderTop[i].tpage = GetTPage(g_Menu->texPage0, 0, g_Menu->texPageX0, g_Menu->texPageY0);
        pWindow->polysWindowBorderTop[i].clut = GetClut(g_Menu->clutX0, g_Menu->clutY0);
        
        SetPolyFT4(&pWindow->polysWindowBorderBottom[i]);
        SetShadeTex(&pWindow->polysWindowBorderBottom[i], 1);
        setRGB0(&pWindow->polysWindowBorderBottom[i], 0xFF, 0xFF, 0xFF);
        pWindow->polysWindowBorderBottom[i].tpage = GetTPage(g_Menu->texPage1, 0, g_Menu->texPageX1, g_Menu->texPageY1);
        pWindow->polysWindowBorderBottom[i].clut = GetClut(g_Menu->clutX1, g_Menu->clutY1);
        
        SetPolyFT4(&pWindow->polysWindowBorderLeft[i]);
        SetShadeTex(&pWindow->polysWindowBorderLeft[i], 1);
        setRGB0(&pWindow->polysWindowBorderLeft[i], 0xFF, 0xFF, 0xFF);
        pWindow->polysWindowBorderLeft[i].tpage = GetTPage(g_Menu->texPage2, 0, g_Menu->texPageX2, g_Menu->texPageY2);
        pWindow->polysWindowBorderLeft[i].clut = GetClut(g_Menu->clutX2, g_Menu->clutY2);
        
        SetPolyFT4(&pWindow->polysWindowBorderRight[i]);
        SetShadeTex(&pWindow->polysWindowBorderRight[i], 1);
        setRGB0(&pWindow->polysWindowBorderRight[i], 0xFF, 0xFF, 0xFF);
        pWindow->polysWindowBorderRight[i].tpage = GetTPage(g_Menu->texPage3, 0, g_Menu->texPageX3, g_Menu->texPageY3);
        pWindow->polysWindowBorderRight[i].clut = GetClut(g_Menu->clutX3, g_Menu->clutY3);
    }
}

void ShopMenuInitializeScrollBar(u_char index, u_short x, u_short y, u_short width, u_short height) {
    MenuWindow* pWindow = g_Menu->windows[index];
    
    // Top ornament
    func_8002675C(
        g_Menu->unk2DC, 
        MENU_TEX_SCROLL_BAR_ORNAMENT, 
        pWindow->polysScrollBarEnds, 
        g_Menu->renderContext, 
        x, y, 0x1000
    );

    // Bottom ornament
    func_800263E4(
        g_Menu->unk2DC, 
        MENU_TEX_SCROLL_BAR_ORNAMENT, 
        &pWindow->polysScrollBarEnds[2], 
        g_Menu->renderContext, 
        x, 
        y + height - 8, 
        0x1000, 0, 1
    );

    func_8002675C(
        g_Menu->unk2DC, 
        MENU_TEX_SCROLL_BAR_EMPTY, 
        pWindow->polysScrollBarEmpty, 
        g_Menu->renderContext, 
        x, 
        y + 8, 
        0x1000
    );
    
    ShopMenuSetVertices(pWindow->vertsScrollBarEnds, x, y, 8, 8);
    ShopMenuSetVertices(&pWindow->vertsScrollBarEnds[4], x, y + height, 8, -8);
    ShopMenuSetVertices(pWindow->vertsScrollBarEmpty, x, y + 8, 8, height - 8);
}

void ShopMenuInitializeWindowBorderCorners(u_char index, u_short x, u_short y, u_short width, u_short height) {
    MenuWindow* pWindow;
    int i;

    pWindow = g_Menu->windows[index];

    pWindow->unk710 = 0;
    pWindow->unk710 += func_8002675C(
        g_Menu->unk2DC, 
        MENU_TEX_WINDOW_BORDER_TOP_LEFT, 
        &pWindow->polysWindowBorderCorners, 
        g_Menu->renderContext, 
        0, 0, 0x1000
    );
    pWindow->unk710 += func_8002675C(
        g_Menu->unk2DC, 
        MENU_TEX_WINDOW_BORDER_TOP_RIGHT, 
        &pWindow->polysWindowBorderCorners[2 * pWindow->unk710], 
        g_Menu->renderContext, 
        0, 0, 0x1000
    );
    pWindow->unk710 += func_8002675C(
        g_Menu->unk2DC, 
        MENU_TEX_WINDOW_BORDER_BOTTOM_LEFT, 
        &pWindow->polysWindowBorderCorners[2 * pWindow->unk710], 
        g_Menu->renderContext, 
        0, 0, 0x1000
    );
    pWindow->unk710 += func_8002675C(
        g_Menu->unk2DC, 
        MENU_TEX_WINDOW_BORDER_BOTTOM_RIGHT, 
        &pWindow->polysWindowBorderCorners[2 * pWindow->unk710], 
        g_Menu->renderContext, 
        0, 0, 0x1000
    );
    
    ShopMenuSetVertices(
        pWindow->vertsWindowBorderCorners, 
        x - 8, 
        y + 8, 
        16, -16
    );
    ShopMenuSetVertices(
        &pWindow->vertsWindowBorderCorners[4], 
        x + width + 8, 
        y + 8, 
        -16, -16
    );
    ShopMenuSetVertices(
        &pWindow->vertsWindowBorderCorners[8], 
        x - 8, 
        y + height - 8, 
        16, 16
    );
    ShopMenuSetVertices(
        &pWindow->vertsWindowBorderCorners[0xC], 
        x + width + 8,
        y + height - 8,
        -16, 16
    );

    for (i = 0; i < 4; i++) {
        ShopMenuSetWindowBorderPrimitive(&pWindow->polysWindowBorderCorners[i * 2 + g_Menu->renderContext]);
    }
}

void ShopMenuSetWindowBorderTop(u_char index, u_short x, u_short y, u_short width) {
    MenuWindow* pWindow;
    int i;
    int innerWidth;
    u_short halfInnerWidth;

    pWindow = g_Menu->windows[index];

    setUV4(
        &pWindow->polysWindowBorderTop[g_Menu->renderContext],
        0, 132,
        7, 132,
        0, 148,
        7, 148
    );
    setUV4(
        &pWindow->polysWindowBorderTop[2 + g_Menu->renderContext],
        0, 132,
        7, 132,
        0, 148,
        7, 148
    );
    
    innerWidth = width - (MENU_WINDOW_BORDER_SIZE * 2);
    halfInnerWidth = innerWidth / 2;
    
    ShopMenuSetVertices(
        pWindow->vertsWindowBorderTop1, 
        x + MENU_WINDOW_BORDER_SIZE,
        y - MENU_WINDOW_BORDER_SIZE,
        halfInnerWidth,
        MENU_WINDOW_BORDER_SIZE * 2
    );
    
    ShopMenuSetVertices(
        pWindow->vertsWindowBorderTop2, 
        x + MENU_WINDOW_BORDER_SIZE + halfInnerWidth, 
        y - MENU_WINDOW_BORDER_SIZE,
        halfInnerWidth, 
        MENU_WINDOW_BORDER_SIZE * 2
    );

    for (i = 0; i < 2; i++) {
        ShopMenuSetWindowBorderPrimitive(&pWindow->polysWindowBorderTop[i * 2 + g_Menu->renderContext]);
    }
}

void ShopMenuSetWindowBorderBottom(u_char index, u_short x, u_short y, u_short width, u_short height) {
    MenuWindow* pWindow;
    int i;
    int innerWidth;

    pWindow = g_Menu->windows[index];

    setUV4(
        &pWindow->polysWindowBorderBottom[g_Menu->renderContext],
        8, 132,
        15, 132,
        8, 148,
        15, 148
    );

    setUV4(
        &pWindow->polysWindowBorderBottom[2 + g_Menu->renderContext],
        8, 132,
        15, 132,
        8, 148,
        15, 148
    );
    
    innerWidth = width - 16;
    width = innerWidth / 2;
    height = y + height - 8;
    
    ShopMenuSetVertices(
        pWindow->vertsWindowBorderBottom1, 
        x + 8, 
        height, 
        width, 16
    );
    ShopMenuSetVertices(
        pWindow->vertsWindowBorderBottom2, 
        x + 8 + width, 
        height, 
        width, 16
    );

    for (i = 0; i < 2; i++) {
        ShopMenuSetWindowBorderPrimitive(&pWindow->polysWindowBorderBottom[i * 2 + g_Menu->renderContext]);
    }
}

void ShopMenuSetWindowBorderLeft(u_char index, u_short x, u_short y, u_short height) {
    MenuWindow* pWindow;
    int i;
    int innerHeight;
    u_short halfInnerHeight;
    
    pWindow = g_Menu->windows[index];

    setUV4(
        &pWindow->polysWindowBorderLeft[g_Menu->renderContext],
        16, 132,
        32, 132,
        16, 139,
        32, 139
    );

    setUV4(
        &pWindow->polysWindowBorderLeft[2 + g_Menu->renderContext],
        16, 132,
        32, 132,
        16, 139,
        32, 139
    );

    innerHeight = height - 16;
    halfInnerHeight = innerHeight / 2;
    
    ShopMenuSetVertices(
        pWindow->vertsWindowBorderLeft1, 
        x - 8, y + 8, 
        16, halfInnerHeight
    );
    ShopMenuSetVertices(
        pWindow->vertsWindowBorderLeft2, 
        x - 8, y + 8 + halfInnerHeight, 
        16, halfInnerHeight
    );

    for (i = 0; i < 2; i++) {
        ShopMenuSetWindowBorderPrimitive(&pWindow->polysWindowBorderLeft[i * 2 + g_Menu->renderContext]);
    }
}

void ShopMenuSetWindowBorderRight(u_char index, u_short x, u_short y, u_short width, u_short height) {
    MenuWindow* pWindow;
    int i;
    int innerHeight;
    u_short halfInnerHeight;

    pWindow = g_Menu->windows[index];

    setUV4(
        &pWindow->polysWindowBorderRight[g_Menu->renderContext],
        16, 140,
        32, 140,
        16, 147,
        32, 147
    );

    setUV4(
        &pWindow->polysWindowBorderRight[2 + g_Menu->renderContext],
        16, 140,
        32, 140,
        16, 147,
        32, 147
    );
    
    width = x + width - 8;    
    innerHeight = height - 16;
    halfInnerHeight = innerHeight / 2;
    
    ShopMenuSetVertices(
        pWindow->vertsWindowBorderRight1, 
        width, y + 8, 
        16, halfInnerHeight
    );
    ShopMenuSetVertices(
        pWindow->vertsWindowBorderRight2, 
        width, y + 8 + halfInnerHeight,
        16, halfInnerHeight
    );

    for (i = 0; i < 2; i++) {
        ShopMenuSetWindowBorderPrimitive(&pWindow->polysWindowBorderRight[i * 2 + g_Menu->renderContext]);
    }
}

void ShopMenuSetWindow(u_char index, u_short x, u_short y, u_short width, u_short height, u_char arg5, int zIndex, u_char hasScrollBar) {
    MenuWindow* pWindow;

    pWindow = g_Menu->windows[index];
    g_Menu->pManager->shouldRenderWindow[index] = FALSE;
    ShopMenuSetVertices(pWindow->vertsBackground, x, y, width, height);
    ShopMenuInitializeWindowBorderCorners(index, x, y, width, height);
    ShopMenuSetWindowBorderTop(index, x, y, width);
    ShopMenuSetWindowBorderBottom(index, x, y, width, height);
    ShopMenuSetWindowBorderLeft(index, x, y, height);
    ShopMenuSetWindowBorderRight(index, x, y, width, height);
    if (hasScrollBar) {
        ShopMenuInitializeScrollBar(index, x, y, width, height);
    }
    pWindow->hasScrollBar = hasScrollBar;
    pWindow->unk714 = arg5;
    pWindow->zIndex = zIndex;
    pWindow->renderContext = g_Menu->renderContext;
    g_Menu->pManager->shouldRenderWindow[index] = TRUE;
}

void ShopMenuFreeWindow(u_char index) {
    g_Menu->pManager->shouldRenderWindow[index] = FALSE;
    g_Menu->pManager->unk27[index] = FALSE;
    HeapFree(g_Menu->windows[index]);
    HeapFree(g_Menu->windowParameters[index]);
}

void ShopMenuInitializeWindow(u_char windowIndex, u_short x, u_short y, u_short width, u_short height, u_char shouldInitializeHandle, u8 arg6, int zIndex, u_char hasScrollBar) {
    MenuWindowParameters* pWindowParams;

    if (windowIndex >= 2) {
        g_Menu->windows[windowIndex] = HeapAlloc(sizeof(MenuWindow), 0);
        bzero(g_Menu->windows[windowIndex], sizeof(MenuWindow));
        g_Menu->windowParameters[windowIndex] = HeapAlloc(sizeof(MenuWindowParameters), 0);
        bzero(g_Menu->windowParameters[windowIndex] , sizeof(MenuWindowParameters));
        ShopMenuInitializeWindowGraphics(windowIndex);
    }
    
    pWindowParams = g_Menu->windowParameters[windowIndex];
    if (shouldInitializeHandle) {
        pWindowParams->index = windowIndex;
        pWindowParams->unk11 = 0;
        pWindowParams->x = x;
        pWindowParams->y = y;
        pWindowParams->width = width;
        pWindowParams->height = height;
        pWindowParams->unk8 = 0;
        pWindowParams->unkA = 0;
        g_Menu->pManager->unk27[windowIndex] = 1;
        pWindowParams->unk12 = arg6;
        pWindowParams->zIndex = zIndex;
        return;
    }
    
    ShopMenuSetWindow(windowIndex, x, y, width, height, arg6, zIndex, hasScrollBar);
}

void ShopMenuUpdateWindows(void) {
    u_char flag;
    int i;
    MenuWindowParameters* pWindowInfo;

    for (i = 0; i < MENU_MAX_NUM_WINDOWS; i++) {
        pWindowInfo = g_Menu->windowParameters[i];
        if ((g_Menu->pManager->unk27[i]) && pWindowInfo->unk11 == 0) {
            
            flag = 0;
            
            if ((pWindowInfo->unk8 + 32) >= pWindowInfo->width) {
                pWindowInfo->unk8 = pWindowInfo->width;
                flag += 1;
            } else {
                pWindowInfo->unk8 += 32;
            }
            
            if ((pWindowInfo->unkA + 32) >= pWindowInfo->height) {
                pWindowInfo->unkA = pWindowInfo->height;
                flag += 1;
            } else {
                pWindowInfo->unkA += 32;
            }
            
            if (flag == 2) {
                pWindowInfo->unk11 = 1;
            }
            
            ShopMenuSetWindow(
                pWindowInfo->index, 
                (pWindowInfo->x + (pWindowInfo->width / 2)) - (pWindowInfo->unk8 / 2), 
                (pWindowInfo->y + (pWindowInfo->height / 2)) - (pWindowInfo->unkA / 2), 
                pWindowInfo->unk8,  
                pWindowInfo->unkA, 
                pWindowInfo->unk12, 
                pWindowInfo->zIndex, 
                pWindowInfo->hasScrollBar
            );
        }
    }
}

// Project and draw polygons
void ShopMenuRenderPolygons(int numPolygons, SVECTOR* pVertices, POLY_FT4* pPolys, int renderContext) {
    long interpolated;
    long flag;
    int i;

    for (i = 0; i < numPolygons; i++) {
        RotTransPers4(
            &pVertices[i*4 + 0], 
            &pVertices[i*4 + 1], 
            &pVertices[i*4 + 2], 
            &pVertices[i*4 + 3], 
            &pPolys[i*2 + renderContext].x0, 
            &pPolys[i*2 + renderContext].x1,
            &pPolys[i*2 + renderContext].x2,
            &pPolys[i*2 + renderContext].x3,
            &interpolated, 
            &flag
        );

        AddPrim(&g_Menu->pGfxEnv->ot[4], &pPolys[i*2 + renderContext]);
    }
}

void ShopMenuRenderString(int stringLength, POLY_FT4* pPolys, int renderContext) {
    int i;

    for (i = 0; i < stringLength; i++) {
        AddPrim(
            &g_Menu->pGfxEnv->ot[4], 
            &pPolys[i * 2 + renderContext]
        );
    }
}

void ShopMenuRenderScrollBarHandle(void) {
    if (g_Menu->pManager->scrollHandleActive) {
        ShopMenuRenderPolygons(1, g_Menu->pScrollHandle->vertices, g_Menu->pScrollHandle->polys, g_Menu->pScrollHandle->renderContext);
    }
}

void func_801C8E28(void) {
    AddPrim(
        &g_Menu->pGfxEnv->ot[4], 
        &g_Menu->unk348->drModes1[g_Menu->unk348->unk159]
    );
    
    if (g_Menu->pManager->unk4) {
        AddPrim(
            &g_Menu->pGfxEnv->ot[4], 
            &g_Menu->unk348->polysPointerCursor[g_Menu->unk348->cursorRenderContext]
        );
    }
}

void ShopMenuRenderTopWindowBorder(int index) {
    long interpolated;
    long flag;
    MenuWindow* pWindow;

    pWindow = g_Menu->windows[index];
    
    RotTransPers4(
        &pWindow->vertsWindowBorderTop1[0], 
        &pWindow->vertsWindowBorderTop1[1], 
        &pWindow->vertsWindowBorderTop1[2], 
        &pWindow->vertsWindowBorderTop1[3], 
        (long*) &pWindow->polysWindowBorderTop[pWindow->renderContext].x0, 
        (long*) &pWindow->polysWindowBorderTop[pWindow->renderContext].x1, 
        (long*) &pWindow->polysWindowBorderTop[pWindow->renderContext].x2, 
        (long*) &pWindow->polysWindowBorderTop[pWindow->renderContext].x3, 
        &interpolated, 
        &flag
    );
    AddPrim(
        &g_Menu->pGfxEnv->ot[pWindow->zIndex], 
        &pWindow->polysWindowBorderTop[pWindow->renderContext]
    );
    
    RotTransPers4(
        &pWindow->vertsWindowBorderTop2[0], 
        &pWindow->vertsWindowBorderTop2[1], 
        &pWindow->vertsWindowBorderTop2[2], 
        &pWindow->vertsWindowBorderTop2[3], 
        (long*) &pWindow->polysWindowBorderTop[2 + pWindow->renderContext].x0, 
        (long*) &pWindow->polysWindowBorderTop[2 + pWindow->renderContext].x1, 
        (long*) &pWindow->polysWindowBorderTop[2 + pWindow->renderContext].x2, 
        (long*) &pWindow->polysWindowBorderTop[2 + pWindow->renderContext].x3, 
        &interpolated, 
        &flag
    );
    AddPrim(
        &g_Menu->pGfxEnv->ot[pWindow->zIndex], 
        &pWindow->polysWindowBorderTop[2 + pWindow->renderContext]
    );
}

void ShopMenuRenderBottomWindowBorder(int index) {
    long interpolated;
    long flag;
    MenuWindow* pWindow;

    pWindow = g_Menu->windows[index];
    
    RotTransPers4(
        &pWindow->vertsWindowBorderBottom1[0], 
        &pWindow->vertsWindowBorderBottom1[1], 
        &pWindow->vertsWindowBorderBottom1[2], 
        &pWindow->vertsWindowBorderBottom1[3], 
        (long*) &pWindow->polysWindowBorderBottom[pWindow->renderContext].x0, 
        (long*) &pWindow->polysWindowBorderBottom[pWindow->renderContext].x1, 
        (long*) &pWindow->polysWindowBorderBottom[pWindow->renderContext].x2, 
        (long*) &pWindow->polysWindowBorderBottom[pWindow->renderContext].x3, 
        &interpolated, 
        &flag
    );
    AddPrim(
        &g_Menu->pGfxEnv->ot[pWindow->zIndex], 
        &pWindow->polysWindowBorderBottom[pWindow->renderContext]
    );
    
    RotTransPers4(
        &pWindow->vertsWindowBorderBottom2[0], 
        &pWindow->vertsWindowBorderBottom2[1], 
        &pWindow->vertsWindowBorderBottom2[2], 
        &pWindow->vertsWindowBorderBottom2[3], 
        (long*) &pWindow->polysWindowBorderBottom[2 + pWindow->renderContext].x0, 
        (long*) &pWindow->polysWindowBorderBottom[2 + pWindow->renderContext].x1, 
        (long*) &pWindow->polysWindowBorderBottom[2 + pWindow->renderContext].x2, 
        (long*) &pWindow->polysWindowBorderBottom[2 + pWindow->renderContext].x3, 
        &interpolated, 
        &flag
    );
    AddPrim(
        &g_Menu->pGfxEnv->ot[pWindow->zIndex], 
        &pWindow->polysWindowBorderBottom[2 + pWindow->renderContext]
    );
}

void ShopMenuRenderLeftWindowBorder(int index) {
    long interpolated;
    long flag;
    MenuWindow* pWindow;

    pWindow = g_Menu->windows[index];
    
    RotTransPers4(
        &pWindow->vertsWindowBorderLeft1[0], 
        &pWindow->vertsWindowBorderLeft1[1], 
        &pWindow->vertsWindowBorderLeft1[2], 
        &pWindow->vertsWindowBorderLeft1[3], 
        (long*) &pWindow->polysWindowBorderLeft[pWindow->renderContext].x0, 
        (long*) &pWindow->polysWindowBorderLeft[pWindow->renderContext].x1, 
        (long*) &pWindow->polysWindowBorderLeft[pWindow->renderContext].x2, 
        (long*) &pWindow->polysWindowBorderLeft[pWindow->renderContext].x3, 
        &interpolated, 
        &flag
    );
    AddPrim(
        &g_Menu->pGfxEnv->ot[pWindow->zIndex], 
        &pWindow->polysWindowBorderLeft[pWindow->renderContext]
    );
    
    RotTransPers4(
        &pWindow->vertsWindowBorderLeft2[0], 
        &pWindow->vertsWindowBorderLeft2[1], 
        &pWindow->vertsWindowBorderLeft2[2], 
        &pWindow->vertsWindowBorderLeft2[3], 
        (long*) &pWindow->polysWindowBorderLeft[2 + pWindow->renderContext].x0, 
        (long*) &pWindow->polysWindowBorderLeft[2 + pWindow->renderContext].x1, 
        (long*) &pWindow->polysWindowBorderLeft[2 + pWindow->renderContext].x2, 
        (long*) &pWindow->polysWindowBorderLeft[2 + pWindow->renderContext].x3, 
        &interpolated, 
        &flag
    );
    AddPrim(
        &g_Menu->pGfxEnv->ot[pWindow->zIndex], 
        &pWindow->polysWindowBorderLeft[2 + pWindow->renderContext]
    );
}

void ShopMenuRenderRightWindowBorder(int index) {
    long interpolated;
    long flag;
    MenuWindow* pWindow;

    pWindow = g_Menu->windows[index];
    
    RotTransPers4(
        &pWindow->vertsWindowBorderRight1[0], 
        &pWindow->vertsWindowBorderRight1[1], 
        &pWindow->vertsWindowBorderRight1[2], 
        &pWindow->vertsWindowBorderRight1[3], 
        (long*) &pWindow->polysWindowBorderRight[pWindow->renderContext].x0, 
        (long*) &pWindow->polysWindowBorderRight[pWindow->renderContext].x1, 
        (long*) &pWindow->polysWindowBorderRight[pWindow->renderContext].x2, 
        (long*) &pWindow->polysWindowBorderRight[pWindow->renderContext].x3, 
        &interpolated, 
        &flag
    );
    AddPrim(
        &g_Menu->pGfxEnv->ot[pWindow->zIndex], 
        &pWindow->polysWindowBorderRight[pWindow->renderContext]
    );
    
    RotTransPers4(
        &pWindow->vertsWindowBorderRight2[0], 
        &pWindow->vertsWindowBorderRight2[1], 
        &pWindow->vertsWindowBorderRight2[2], 
        &pWindow->vertsWindowBorderRight2[3], 
        (long*) &pWindow->polysWindowBorderRight[2 + pWindow->renderContext].x0, 
        (long*) &pWindow->polysWindowBorderRight[2 + pWindow->renderContext].x1, 
        (long*) &pWindow->polysWindowBorderRight[2 + pWindow->renderContext].x2, 
        (long*) &pWindow->polysWindowBorderRight[2 + pWindow->renderContext].x3, 
        &interpolated, 
        &flag
    );
    AddPrim(
        &g_Menu->pGfxEnv->ot[pWindow->zIndex], 
        &pWindow->polysWindowBorderRight[2 + pWindow->renderContext]
    );
}

void ShopMenuRenderWindowBackground(int index) {
    long interpolated;
    long flag;
    MenuWindow* pWindow;

    pWindow = g_Menu->windows[index];
    
    RotTransPers4(
        &pWindow->vertsBackground[0], 
        &pWindow->vertsBackground[1],
        &pWindow->vertsBackground[2], 
        &pWindow->vertsBackground[3],
        (long*) &pWindow->polysBackground[pWindow->renderContext].x0,
        (long*) &pWindow->polysBackground[pWindow->renderContext].x1,
        (long*) &pWindow->polysBackground[pWindow->renderContext].x2,
        (long*) &pWindow->polysBackground[pWindow->renderContext].x3,
        &interpolated, 
        &flag
    );
    AddPrim(
        &g_Menu->pGfxEnv->ot[pWindow->zIndex], 
        &pWindow->polysBackground[pWindow->renderContext]
    );
    AddPrim(
        &g_Menu->pGfxEnv->ot[pWindow->zIndex], 
        &pWindow->drawModes[pWindow->renderContext]
    );
}

void ShopMenuRenderWindowBorderCorners(int index) {
    long interpolated;
    long flag;
    int i;
    MenuWindow* pWindow;

    pWindow = g_Menu->windows[index];

    for (i = 0; i < 4; i++) {
        // Project vertices to x0/y0 coordinates of the polygon
        RotTransPers4(
            &pWindow->vertsWindowBorderCorners[i * 4], 
            &pWindow->vertsWindowBorderCorners[i * 4 + 1],
            &pWindow->vertsWindowBorderCorners[i * 4 + 2], 
            &pWindow->vertsWindowBorderCorners[i * 4 + 3],
            (long*) &pWindow->polysWindowBorderCorners[i * 2 + pWindow->renderContext].x0, 
            (long*) &pWindow->polysWindowBorderCorners[i * 2 + pWindow->renderContext].x1,
            (long*) &pWindow->polysWindowBorderCorners[i * 2 + pWindow->renderContext].x2,
            (long*) &pWindow->polysWindowBorderCorners[i * 2 + pWindow->renderContext].x3,
            &interpolated, 
            &flag
        );

        // Queue polygon for rendering
        AddPrim(
            &g_Menu->pGfxEnv->ot[pWindow->zIndex], 
            &pWindow->polysWindowBorderCorners[i * 2 + pWindow->renderContext]
        );
    }
}

void ShopMenuRenderScrollBar(int index) {
    long interpolated;
    long flag;
    MenuWindow* pWindow;
    int i;

    pWindow = g_Menu->windows[index];
    
    for (i = 0; i < 2; i++) {
        RotTransPers4(
            &pWindow->vertsScrollBarEnds[i * 4], 
            &pWindow->vertsScrollBarEnds[i * 4 + 1],
            &pWindow->vertsScrollBarEnds[i * 4 + 2],
            &pWindow->vertsScrollBarEnds[i * 4 + 3],
            &pWindow->polysScrollBarEnds[i * 2 + pWindow->renderContext].x0,
            &pWindow->polysScrollBarEnds[i * 2 + pWindow->renderContext].x1,
            &pWindow->polysScrollBarEnds[i * 2 + pWindow->renderContext].x2,
            &pWindow->polysScrollBarEnds[i * 2 + pWindow->renderContext].x3,
            &interpolated, 
            &flag
        );
        AddPrim(
            &g_Menu->pGfxEnv->ot[pWindow->zIndex], 
            &pWindow->polysScrollBarEnds[i * 2 + pWindow->renderContext]
        );
    }
    
    RotTransPers4(
        &pWindow->vertsScrollBarEmpty[0],
        &pWindow->vertsScrollBarEmpty[1],
        &pWindow->vertsScrollBarEmpty[2],
        &pWindow->vertsScrollBarEmpty[3],
        &pWindow->polysScrollBarEmpty[pWindow->renderContext].x0,
        &pWindow->polysScrollBarEmpty[pWindow->renderContext].x1,
        &pWindow->polysScrollBarEmpty[pWindow->renderContext].x2,
        &pWindow->polysScrollBarEmpty[pWindow->renderContext].x3,
        &interpolated, 
        &flag
    );
    AddPrim(
        &g_Menu->pGfxEnv->ot[pWindow->zIndex], 
        &pWindow->polysScrollBarEmpty[pWindow->renderContext]
    );
}

void ShopMenuRenderWindows(void) {
    SVECTOR rotation;
    VECTOR translation;
    MATRIX matTransform;
    SVECTOR _unused;
    MenuWindow* pWindow;
    int i;

    // Project and render all active windows
    for (i = 0; i < MENU_MAX_NUM_WINDOWS; i++) {
        if (g_Menu->pManager->shouldRenderWindow[i]) {
            pWindow = g_Menu->windows[i];
            if (pWindow->unk714 == 0) {
                PushMatrix();
                rotation.vz = 0;
                rotation.vy = 0;
                rotation.vx = 0;
                translation.vy = 0;
                translation.vx = 0;
                translation.vz = 512;
                RotMatrix(&rotation, &matTransform);
                TransMatrix(&matTransform, &translation);
                SetRotMatrix(&matTransform);
                SetTransMatrix(&matTransform);

                ShopMenuRenderWindowBorderCorners(i);
                if (pWindow->hasScrollBar) { 
                    ShopMenuRenderScrollBar(i); 
                }
                ShopMenuRenderTopWindowBorder(i);
                ShopMenuRenderBottomWindowBorder(i);
                ShopMenuRenderLeftWindowBorder(i);
                ShopMenuRenderRightWindowBorder(i);
                ShopMenuRenderWindowBackground(i);

                PopMatrix();
            } else {
                ShopMenuRenderWindowBorderCorners(i);
                if (pWindow->hasScrollBar) { 
                    ShopMenuRenderScrollBar(i); 
                }
                ShopMenuRenderTopWindowBorder(i);
                ShopMenuRenderBottomWindowBorder(i);
                ShopMenuRenderLeftWindowBorder(i);
                ShopMenuRenderRightWindowBorder(i);
                ShopMenuRenderWindowBackground(i);
            }
        }
    }
}

void ShopMenuRenderPointerCursors(void) {
    int i;

    if (g_Menu->pManager->shouldRenderPointerCursors) {
        for (i = 0; i < MENU_MAX_NUM_CURSORS; i++) {
            if (g_Menu->pCursors->shouldRender[i]) {
                if (g_Menu->pCursors->unk144[i]) {
                    setXY4(
                        &g_Menu->pCursors->polysCursor[i * 2 + g_Menu->pCursors->renderContexts[i]],
                        D_801D2094[D_801D201C[g_Menu->unk32C->unk4F7C]] + 8,
                        D_801D2114[D_801D201C[g_Menu->unk32C->unk4F7C]] - 6,
                        D_801D2094[D_801D201C[g_Menu->unk32C->unk4F7C]] + 24,
                        D_801D2114[D_801D201C[g_Menu->unk32C->unk4F7C]] - 6,
                        D_801D2094[D_801D201C[g_Menu->unk32C->unk4F7C]] + 8,
                        D_801D2114[D_801D201C[g_Menu->unk32C->unk4F7C]] + 10,
                        D_801D2094[D_801D201C[g_Menu->unk32C->unk4F7C]] + 24,
                        D_801D2114[D_801D201C[g_Menu->unk32C->unk4F7C]] + 10  
                    );
                }
                
                AddPrim(
                    &g_Menu->pGfxEnv->ot[4], 
                    &g_Menu->pCursors->polysCursor[i * 2 + g_Menu->pCursors->renderContexts[i]]
                );
            }
        }
    }
}

void func_801C9F7C(void) {
    int i;

    for (i = 0; i < 4; i++) {
        if (g_Menu->pManager->unk34[i]) {
            AddPrim(
                &g_Menu->pGfxEnv->ot[4],
                &g_Menu->unk4E0[i].polys[g_Menu->unk4E0[i].renderContext]
            );
        }
    }
}

void func_801CA00C(void) {
    int i;

    for (i = 0; i < 8; i++) {
        if (g_Menu->pManager->unkC[i]) {
            AddPrim(
                &g_Menu->pGfxEnv->ot[4], 
                &g_Menu->unk6E0[i].polys[g_Menu->unk6E0[i].renderContext]
            );
        }
    }
}

void func_801CA09C(void) {
    long interpolated;
    long flag;
    int i;

    for (i = 0; i < 6; i++) {
        if (g_Menu->pManager->unk14[i]) {
            if (g_Menu->unkAE0[i].unk7F) {
                RotTransPers4(
                    &g_Menu->unkAE0[i].vertices[0], 
                    &g_Menu->unkAE0[i].vertices[1],
                    &g_Menu->unkAE0[i].vertices[2],
                    &g_Menu->unkAE0[i].vertices[3],
                    (long*)&g_Menu->unkAE0[i].polys[g_Menu->unkAE0[i].renderContext].x0, 
                    (long*)&g_Menu->unkAE0[i].polys[g_Menu->unkAE0[i].renderContext].x1,
                    (long*)&g_Menu->unkAE0[i].polys[g_Menu->unkAE0[i].renderContext].x2,
                    (long*)&g_Menu->unkAE0[i].polys[g_Menu->unkAE0[i].renderContext].x3,
                    &interpolated, 
                    &flag
                );
                AddPrim(
                    &g_Menu->pGfxEnv->ot[4], 
                    &g_Menu->unkAE0[i].polys[g_Menu->unkAE0[i].renderContext]
                );
            } else {
                AddPrim(
                    &g_Menu->pGfxEnv->ot[4], 
                    &g_Menu->unkAE0[i].polys[g_Menu->unkAE0[i].renderContext]
                );
            }

        }
    }
}

int func_801CA214(void) {
    int i;
    for (i = 5; i >= 0; i--) {}
    return i;
}

// Render confirmation window text?
void func_801CA22C(void) {
    long interpolated;
    long flag;
    MenuString* pString;
    int i;

    if (g_Menu->pManager->unk2E) {
        for (i = 0 ; i < 3; i++) {
            pString = g_Menu->unk1DE0[i];
            if (pString->unk7F) {
                RotTransPers4(
                    &pString->vertices[0], 
                    &pString->vertices[1], 
                    &pString->vertices[2],
                    &pString->vertices[3], 
                    (long*) &pString->polys[pString->renderContext].x0, 
                    (long*) &pString->polys[pString->renderContext].x1, 
                    (long*) &pString->polys[pString->renderContext].x2, 
                    (long*) &pString->polys[pString->renderContext].x3, 
                    &interpolated, 
                    &flag
                );
                AddPrim(&g_Menu->pGfxEnv->ot[4], &pString->polys[pString->renderContext]);
            } else {
                AddPrim(&g_Menu->pGfxEnv->ot[4], &pString->polys[pString->renderContext]);
            }
            
        }
    }
}

// Draw the black dimming effect
void ShopMenuRenderBackgroundDim(void) {
    AddPrim(&g_Menu->pGfxEnv->ot[8], &g_Menu->unk348->polysDimEffect[g_Menu->renderContext]);
    AddPrim(&g_Menu->pGfxEnv->ot[8], &g_Menu->unk348->drawModeDimEffect[g_Menu->renderContext]);
}

void func_801CA404(void) {
    func_801C9F7C();
    func_801CA00C();
    func_801CA09C();
    func_801CA214();
    func_801CA22C();
}

void ShopMenuRenderSelectionMenu(void) {
    int i;

    if (g_Menu->pManager->shouldRenderSelectionMenu) {
        if (g_Menu->pSelectionMenu->unk1192 != g_Menu->pSelectionMenu->unk1193) {

            // Render the selection menu as disabled
            if (g_Menu->pSelectionMenu->unk1192) {
                for (i = 0; i < g_Menu->pSelectionMenu->numTexts; i++) {
                    SetSemiTrans(&g_Menu->pSelectionMenu->polysTexts[i*2 + g_Menu->pSelectionMenu->textsRenderCtx], 1);
                    SetShadeTex(&g_Menu->pSelectionMenu->polysTexts[i*2 + g_Menu->pSelectionMenu->textsRenderCtx], 0);
                    g_Menu->pSelectionMenu->polysTexts[i*2 + g_Menu->pSelectionMenu->textsRenderCtx].tpage |= 0x20;
                    setRGB0(&g_Menu->pSelectionMenu->polysTexts[i*2 + g_Menu->pSelectionMenu->textsRenderCtx], 32, 32, 32);
                }

                for (i = 0; i < g_Menu->pSelectionMenu->numCursors; i++) {
                    SetSemiTrans(&g_Menu->pSelectionMenu->polysCursors[i*2 + g_Menu->pSelectionMenu->cursorsRenderCtx], 1);
                    SetShadeTex(&g_Menu->pSelectionMenu->polysCursors[i*2 + g_Menu->pSelectionMenu->cursorsRenderCtx], 0);
                    g_Menu->pSelectionMenu->polysCursors[i*2 + g_Menu->pSelectionMenu->cursorsRenderCtx].tpage |= 0x20;
                    setRGB0(&g_Menu->pSelectionMenu->polysCursors[i*2 + g_Menu->pSelectionMenu->cursorsRenderCtx], 32, 32, 32);
                }

            // Render the seleciton menu as active
            } else {
                for (i = 0; i < g_Menu->pSelectionMenu->numTexts; i++) {
                    SetSemiTrans(&g_Menu->pSelectionMenu->polysTexts[i*2 + g_Menu->pSelectionMenu->textsRenderCtx], 0);
                    SetShadeTex(&g_Menu->pSelectionMenu->polysTexts[i*2 + g_Menu->pSelectionMenu->textsRenderCtx], 0);
                    g_Menu->pSelectionMenu->polysTexts[i*2 + g_Menu->pSelectionMenu->textsRenderCtx].tpage |= 0x20;
                    setRGB0(&g_Menu->pSelectionMenu->polysTexts[i*2 + g_Menu->pSelectionMenu->textsRenderCtx], 128, 128, 128);
                }

                for (i = 0; i < g_Menu->pSelectionMenu->numCursors; i++) {
                    SetSemiTrans(&g_Menu->pSelectionMenu->polysCursors[i*2 + g_Menu->pSelectionMenu->cursorsRenderCtx], 0);
                    SetShadeTex(&g_Menu->pSelectionMenu->polysCursors[i*2 + g_Menu->pSelectionMenu->cursorsRenderCtx], 0);
                    g_Menu->pSelectionMenu->polysCursors[i*2 + g_Menu->pSelectionMenu->cursorsRenderCtx].tpage |= 0x20;
                    setRGB0(&g_Menu->pSelectionMenu->polysCursors[i*2 + g_Menu->pSelectionMenu->cursorsRenderCtx], 128, 128, 128);
                }
            }
            
            g_Menu->pSelectionMenu->unk1193 = g_Menu->pSelectionMenu->unk1192;
        }

        ShopMenuRenderString(g_Menu->pSelectionMenu->numTexts, g_Menu->pSelectionMenu->polysTexts, g_Menu->pSelectionMenu->textsRenderCtx);
        ShopMenuRenderString(g_Menu->pSelectionMenu->numCursors, g_Menu->pSelectionMenu->polysCursors, g_Menu->pSelectionMenu->cursorsRenderCtx);
    }
}

void func_801CAB0C(void) {
    if (g_Menu->pManager->unkA) {
        ShopMenuRenderString(g_Menu->unk354->unk1404, g_Menu->unk354->polys500, g_Menu->unk354->unk1409);
        ShopMenuRenderString(g_Menu->unk354->unk1400, g_Menu->unk354->polys0, g_Menu->unk354->unk1408);
    }
}

void ShopMenuRenderArrowCursors(void) {
    int i;
    
    for (i = 0; i < MENU_MAX_NUM_ARROW_CURSORS; i++) {
        if (g_Menu->pManager->shouldRenderArrowCursor[i]) {
            ShopMenuRenderPolygons(1, 
                g_Menu->arrowCursors[i]->vertices, 
                g_Menu->arrowCursors[i]->polys, 
                g_Menu->arrowCursors[i]->renderContext
            );
        }
    }
}

void ShopMenuRender(void) {
    if (g_Menu->shouldDrawMenu) {
        ShopMenuUpdateWindows();
        ShopMenuRenderPointerCursors();
        func_801CA404();
        func_801C8E28();
        ShopMenuRenderArrowCursors();
        func_801CCFF4();
        ShopMenuRenderScrollBarHandle();
        ShopMenuRenderSelectionMenu();
        func_801CAB0C();
        ShopMenuRenderWindows();
    }
    ShopMenuRenderBackgroundDim();
}

void ShopMenuPlaySoundEffect(u_char soundEffectId) {
    if (g_Menu->unk32A) {
        func_80039DB8((g_Menu->unk2E4->sedId << 16) | soundEffectId);
    }
}

void ShopMenuPollInput(void) {
    u_char wasControllerUnplugged;
    u_char isLooping;
    int savedValue;
    u_char input;

    input = MENU_INPUT_IDLE;
    isLooping = TRUE;
    wasControllerUnplugged = FALSE;
    while (isLooping) {
        // Is the main controller not plugged in?
        if (ControllerGetType(0) == CONTROLLER_TYPE_NONE) {
            if (wasControllerUnplugged == 0) {
                wasControllerUnplugged++;
                SoundMuteAllSpuChannels();
                savedValue = D_80059488;
            }
            continue;
        }
        
        isLooping--;
        if (wasControllerUnplugged) {
            SoundEnableAllSpuChannels();
            D_80059488 = savedValue;
        }
    }
    
    if (func_80036410()) {
        ControllerResetState();
    } else {
        while (ControllerPopState()) {
            if (g_C1ButtonStatePressedOnce & CTRL_BTN_RIGHT) {
                input = MENU_INPUT_RIGHT;
                ShopMenuPlaySoundEffect(1);
                break;
            }
            if (g_C1ButtonStatePressedOnce & CTRL_BTN_DOWN) {
                input = MENU_INPUT_DOWN;
                ShopMenuPlaySoundEffect(1);
                break;
            }
            if (g_C1ButtonStatePressedOnce & CTRL_BTN_LEFT) {
                input = MENU_INPUT_LEFT;
                ShopMenuPlaySoundEffect(1);
                break;
            }
            if (g_C1ButtonStatePressedOnce & CTRL_BTN_UP) {
                input = MENU_INPUT_UP;
                ShopMenuPlaySoundEffect(1);
                break;
            }
            if (g_C1ButtonStateReleased & CTRL_BTN_CIRCLE) {
                input = MENU_INPUT_CONFIRM;
                break;
            }
            if (g_C1ButtonStateReleased & CTRL_BTN_CROSS) {
                input = MENU_INPUT_BACK;
                ShopMenuPlaySoundEffect(3);
                break;
            }
            if (g_C1ButtonStateReleased & CTRL_BTN_SQUARE) {
                input = 6;
                break;
            }
            if (g_C1ButtonStateReleased & CTRL_BTN_TRIANGLE) {
                input = 7;
                break;
            }
            if (g_C1ButtonStateReleased & CTRL_BTN_L1) {
                input = 10;
                break;
            }
            if (g_C1ButtonStateReleased & CTRL_BTN_R1) {
                input = 9;
                break;
            }
            if (g_C1ButtonStateReleased & CTRL_BTN_START) {
                input = 11;
                break;
            }
            if (g_C1ButtonStateReleased & CTRL_BTN_SELECT) {
                g_Menu->unk1E94 = g_Menu->unk1E94 == 0;
                input = 12;
                break;
            }
            if (g_C1ButtonStateReleased & CTRL_BTN_L2) {
                g_Menu->unk1E95 += 1;
                break;
            }
        }
    }
    g_Menu->input = input;
}

// Updates the effect where the menu is rotated and zoomed in/out
void ShopMenuUpdateTransitionEffect(void) {
    switch (g_Menu->transitionEffectState) {
        case MENU_CLOSE_ANIMATION_START:
            g_Menu->translation.vz = 512;
            g_Menu->rotation.vz = 0;
            g_Menu->rotation.vy = 0;
            g_Menu->rotation.vx = 0;
            g_Menu->translation.vy = 0;
            g_Menu->translation.vx = 0;
            g_Menu->transitionEffectState = MENU_CLOSE_ANIMATION;
            break;
        case MENU_OPEN_ANIMATION_START:
            // Set window to the far back
            g_Menu->translation.vz = 2048;
            g_Menu->rotation.vz = 0;
            g_Menu->rotation.vy = 0;
            g_Menu->rotation.vx = 0;
            g_Menu->translation.vy = 0;
            g_Menu->translation.vx = 0;
            g_Menu->transitionEffectState = MENU_OPEN_ANIMATION;
            break;
        case MENU_CLOSE_ANIMATION:
            g_Menu->rotation.vy -= 96;
            g_Menu->translation.vz += 64;
            if (g_Menu->translation.vz >= PSX_DEGREES(315)) {
                g_Menu->transitionEffectState = MENU_ANIMATION_DONE;
            }
            break;
        case MENU_OPEN_ANIMATION:
            // Flip the window around the X-axis while
            // bring it closer to the viewer, giving and
            // effect of zooming in.
            g_Menu->rotation.vx += 124;
            g_Menu->translation.vz -= 48;
            if (g_Menu->translation.vz < 512) {
                g_Menu->translation.vz = 512;
                g_Menu->rotation.vz = 0;
                g_Menu->rotation.vx = 0;
                g_Menu->rotation.vy = 0;
                g_Menu->transitionEffectState = MENU_ANIMATION_DONE;
            }
            break;
    }

    RotMatrix(&g_Menu->rotation, &g_Menu->matTransform);
    TransMatrix(&g_Menu->matTransform, &g_Menu->translation);
    SetRotMatrix(&g_Menu->matTransform);
    SetTransMatrix(&g_Menu->matTransform);
}

void ShopMenuUpdateAndRender(void) {
    GfxEnvironment* pNextGfxEnv;
    int renderContext;

    // Debugging
    if (*D_8005917C != -1) {
#ifndef XENO_PC_PORT
        asm("break 0x400");   /* MIPS trap; host assembler can't emit it (debug-only) */
#endif
    }
    
    ShopMenuPollInput();
    GameCheckAndHandleSoftReset();

    // Rotate graphics environment
    pNextGfxEnv = &g_Menu->gfxEnvs[0];
    if (g_Menu->pGfxEnv == &g_Menu->gfxEnvs[0]) {
        pNextGfxEnv = &g_Menu->gfxEnvs[1];
    }
    g_Menu->pGfxEnv = pNextGfxEnv;
    
    g_Menu->renderContext = g_Menu->renderContext == 0;
    ClearOTagR(g_Menu->pGfxEnv->ot, 0x10);
    ShopMenuUpdateTransitionEffect();
    ShopMenuRender();
    renderContext = g_Menu->renderContext == 0;
    DrawSync(0);
    Vsync(0);
    PutDrawEnv(&g_Menu->pGfxEnv->drawEnv);
    PutDispEnv(&g_Menu->pGfxEnv->dispEnv);
    MoveImage(&g_Menu->pSelectionMenu->unk1180, 0, renderContext * 0xE0);
    DrawOTag(&g_Menu->pGfxEnv->ot[0xF]);
}

void ShopMenuInitializePointerCursors(u_char mode) {
    int i;

    g_Menu->pCursors = HeapAlloc(sizeof(MenuPointerCursors), 0);
    bzero(g_Menu->pCursors, sizeof(MenuPointerCursors));
    
    switch (mode) {
        case 0:
            g_Menu->pManager->shouldRenderPointerCursors = TRUE;
            g_Menu->pCursors->unk144[0] = TRUE;
            g_Menu->pCursors->unk144[1] = TRUE;
            /* fallthrough */
        case 2:
            for (i = 0; i < MENU_MAX_NUM_CURSORS; i++) {
                func_8002675C(
                    g_Menu->unk2DC, MENU_TEX_POINTER_CURSOR, 
                    &g_Menu->pCursors->polysCursor[i * 2], g_Menu->renderContext, 
                    D_801D1FF8[i], D_801D2008[i],
                    0x800
                );
                g_Menu->pCursors->renderContexts[i] = g_Menu->renderContext;
            }
            return;
        case 3:
            func_8002675C(
                g_Menu->unk2DC, MENU_TEX_POINTER_CURSOR, 
                &g_Menu->pCursors->polysCursor[0], g_Menu->renderContext, 
                0, 0, 
                0x800
            );
            g_Menu->pCursors->renderContexts[0] = g_Menu->renderContext;
            g_Menu->pManager->shouldRenderPointerCursors = TRUE;
            return;
        case 1:
            return;
    }
}

void ShopMenuFreePointerCursors(void) {
    g_Menu->pManager->shouldRenderPointerCursors = FALSE;
    ShopMenuUpdateAndRender();
    HeapFree(g_Menu->pCursors);
}

void ShopMenuStartOpenMenuTransition(void) {
    g_Menu->transitionEffectState = MENU_OPEN_ANIMATION_START;
    ShopMenuPlaySoundEffect(0x5B);
}

void ShopMenuStartCloseMenuTransition(void) {
    g_Menu->transitionEffectState = MENU_CLOSE_ANIMATION_START;
}

void ShopMenuConfirmationWindowInitialize(u_char stringIndex) {
    MenuWindowParameters* pWindowParams;
    MenuString* pString;
    int i;
    int xPosition;
    
    xPosition = 0x50; 
    
    ShopMenuInitializeWindow(4, 0x42, 0x46, 0xBC, 0x40, 1, 1, 4, 0);
    pWindowParams = g_Menu->windowParameters[4];
    while (!pWindowParams->unk11) {
        ShopMenuUpdateAndRender();
    }

    for (i = 0; i < 4; i++) {
        g_Menu->unk1DE0[i] = HeapAlloc(sizeof(MenuString), 0);
        bzero(g_Menu->unk1DE0[i], sizeof(MenuString));

        // Even indices
        if (!(i & 1)) {
            // Buffer size = FONT_LETTER_HEIGHT * 57 * 2
            g_Menu->unk1DE0[i]->pVramBuffer = HeapAlloc(0x5CA, 0);
            g_Menu->unk1DE0[i]->vramDest.x = 320;
            g_Menu->unk1DE0[i]->vramDest.y = ((i / 2) * FONT_LETTER_HEIGHT) + 78;
            g_Menu->unk1DE0[i]->vramDest.w = 58;
            g_Menu->unk1DE0[i]->vramDest.h = FONT_LETTER_HEIGHT;
            continue;
        }

        // Odd indices, share VRAM buffer w/ prev entry
        g_Menu->unk1DE0[i]->pVramBuffer = g_Menu->unk1DE0[i - 1]->pVramBuffer;
    }

    // Render strings to text areas
    for (i = 0; i < 3; i++) {
        pString = g_Menu->unk1DE0[i];
        pString->width = SystemRenderStringEntry(
            GetStringEntry(g_Menu->unk2E0, stringIndex + i), 
            pString->pVramBuffer, 0x36, i % 2
        );
        func_801C5A7C(pString, i, 0, 0);
        ShopMenuSetVertices(pString->vertices, xPosition, (i * 16) + 0x50, pString->width, FONT_LETTER_HEIGHT);
        setUV4(
            &pString->polys[g_Menu->renderContext],
            0,                (i / 2) * FONT_LETTER_HEIGHT + 0x4E,
            pString->width,   (i / 2) * FONT_LETTER_HEIGHT + 0x4E,
            0,                (i / 2) * FONT_LETTER_HEIGHT + 0x5B,
            pString->width,   (i / 2) * FONT_LETTER_HEIGHT + 0x5B
        );
        pString->renderContext = g_Menu->renderContext;
        pString->unk7F = 1;
    }

    // Transfer text areas to VRAM
    LoadImage(&g_Menu->unk1DE0[0]->vramDest, g_Menu->unk1DE0[0]->pVramBuffer);
    LoadImage(&g_Menu->unk1DE0[2]->vramDest, g_Menu->unk1DE0[2]->pVramBuffer);
    DrawSync(0);
    
    g_Menu->pManager->unk2E = 1;
    HeapFree(g_Menu->unk1DE0[0]->pVramBuffer);
    HeapFree(g_Menu->unk1DE0[2]->pVramBuffer);
    if (g_Menu->pManager->unk5B == 2) {
        g_Menu->pManager->unk5B = 1;
    }
    ShopMenuUpdateAndRender();
    ShopMenuUpdateAndRender();
}

void ShopMenuConfirmationWindowFree(void) {
    int i;

    if (g_Menu->pManager->shouldRenderWindow[4]) {
        ShopMenuFreeWindow(4);
        g_Menu->pManager->unk2E = 0;
        for (i = 0; i < 4; i++) {
            HeapFree(g_Menu->unk1DE0[i]);
        }
    }
    g_Menu->pManager->unk5B = 0;
    ShopMenuUpdateAndRender();
}

u_char ShopMenuConfirmationWindowGetChoice(u_char mode) {
    u_char choice;
    u_char isRunning;
    u_char curTimer;
    
    isRunning = TRUE;
    choice = MENU_CHOICE_NO;
    curTimer = 60;

    while (isRunning) {
        if (mode == MENU_AUTO_ADVANCE) {
            g_Menu->pCursors->shouldRender[2] = FALSE;
            g_Menu->pCursors->shouldRender[3] = FALSE;

            // Was any button pressed?
            if (g_Menu->input != MENU_INPUT_IDLE) 
                break;
            
            curTimer--;
            if (curTimer == 0) 
                break;
        }

        ShopMenuUpdateAndRender();
        switch (g_Menu->input) {
            case MENU_INPUT_CONFIRM:
                ShopMenuPlaySoundEffect(2);
                isRunning = FALSE;
                break;
            case MENU_INPUT_BACK:
                choice = MENU_CHOICE_NO;
                isRunning = FALSE;
                break;
            case MENU_INPUT_LEFT:
                g_Menu->pCursors->shouldRender[2] = TRUE;
                g_Menu->pCursors->shouldRender[3] = FALSE;
                choice = MENU_CHOICE_YES;
                break;
            case MENU_INPUT_RIGHT:
                g_Menu->pCursors->shouldRender[2] = FALSE;
                g_Menu->pCursors->shouldRender[3] = TRUE;
                choice = MENU_CHOICE_NO;
                break;
        }    
    }
    
    g_Menu->pCursors->shouldRender[2] = FALSE;
    g_Menu->pCursors->shouldRender[3] = FALSE;

    POSSIBLE_DEBUG_CODE;
    
    return choice;
}

int ShopMenuConfirmationWindow(u_char stringIndex, u_char stringIndex2, u_char mode) {
    u_char choice;

    ShopMenuConfirmationWindowInitialize(stringIndex);
    g_Menu->pCursors->shouldRender[3] = TRUE;
    choice = ShopMenuConfirmationWindowGetChoice(mode);
    ShopMenuConfirmationWindowFree();
    
    // If we chose yes and have another window, run that confirmation window as well
    if (stringIndex2 != 0xFF) {
        if (choice != MENU_CHOICE_NO) {
            ShopMenuConfirmationWindowInitialize(stringIndex2);
            g_Menu->pCursors->shouldRender[3] = TRUE;
            choice = ShopMenuConfirmationWindowGetChoice(mode);
            ShopMenuConfirmationWindowFree();
        }
    } 
    return choice;
}

void ShopMenuFree(void) {
    ShopMenuUpdateAndRender();
    ShopMenuUpdateAndRender();
    g_Menu->shouldDrawMenu = FALSE;
    ShopMenuUpdateAndRender();
    do {
        ShopMenuUpdateAndRender();
    } while (g_Menu->renderContext);
    ShopMenuUnk32CManager(MENU_DATA_FREE);
    ShopMenuSetManager(MENU_DATA_FREE);
    ShopMenuSelectionMenuManager(MENU_DATA_FREE);
    ShopMenuUnk354Manager(MENU_DATA_FREE);
    ShopMenuUnk330Manager(MENU_DATA_FREE);
    ShopMenuUnk348Manager(MENU_DATA_FREE);
    ShopMenuLoadShopItemsData(0x10);
    ShopMenuShopManager(MENU_DATA_FREE);
    HeapFree(g_Menu->unk2DC);
    HeapFree(g_Menu->unk2E0);
    HeapFree(g_Menu->unk4E0[0].pVramBuffer);
    if (g_MenuDebugEnabled) {
        func_8003A094(g_Menu->unk2E4);
        ShopMenuUpdateAndRender();
        func_8003852C(g_Menu->unk2E4);
        ShopMenuUpdateAndRender();
        HeapFree(g_Menu->unk2E4);
    }
    HeapFree(g_Menu->pShopEntries);
    ShopMenuUnk1E20Manager(MENU_DATA_FREE);
    HeapFree(g_Menu);
}

void func_801CBC88(u8 a0, u8 a1, void* a2, void* a3, u8* pClear) {
    s32 i;
    if (a0) {
        func_801C5CBC(a2, a3, 2, a1);
    } else {
        for (i = 0; i < a1; i++) {
            pClear[i] = 0;
        }
    }
}

INCLUDE_ASM("asm/shop_menu/nonmatchings/main/misc", func_801CBCF0);

void ShopMenuInitializeShopModeSelectionMenu(int numTextures, int* pTextureIDs) {
    int i;
    int j;

    // Render the selection menu as active
    g_Menu->pSelectionMenu->unk1192 = FALSE;
    g_Menu->pSelectionMenu->unk1193 = FALSE;

    g_Menu->pManager->shouldRenderSelectionMenu = TRUE;
    
    // Another weird loop case similar to the one in ShopMenuUpdateCharacterPortraits.
    // We loop one and one cursor & text texture entry up to our current counter,
    // rebuild the cursor and text primitives and rerender the whole menu.
    for (i = 1; i <= numTextures; i++) {   
        
        // Cursors for each menu option
        if (i != numTextures) {
            g_Menu->pSelectionMenu->numCursors = 0;
            for (j = 0; j < i; j++) {
                g_Menu->pSelectionMenu->numCursors += func_8002675C(
                    g_Menu->unk2DC, pTextureIDs[j * 2], 
                    &g_Menu->pSelectionMenu->polysCursors[g_Menu->pSelectionMenu->numCursors * 2], 
                    g_Menu->renderContext, 
                    160, 150, 
                    0x1000
                );
            }
            g_Menu->pSelectionMenu->cursorsRenderCtx = g_Menu->renderContext;
        }

        // Text / Content for each menu option
        g_Menu->pSelectionMenu->numTexts = 0;
        if (i != 1) {
            for (j = 0; j < i - 1; j++) {
                g_Menu->pSelectionMenu->numTexts += func_8002675C(
                    g_Menu->unk2DC, pTextureIDs[j * 2 + 1],
                    &g_Menu->pSelectionMenu->polysTexts[g_Menu->pSelectionMenu->numTexts * 2], 
                    g_Menu->renderContext, 
                    160, 150, 
                    0x1000
                );
            }
            g_Menu->pSelectionMenu->textsRenderCtx = g_Menu->renderContext;
        }

        for (j = 0; j < 2; j++) {
            ShopMenuUpdateAndRender();
        }
    }
}

INCLUDE_ASM("asm/shop_menu/nonmatchings/main/misc", func_801CC278);

void ShopMenuUpdateShopModeSelectionMenu(u_char numOptions, u_char selectedIndex, int* pTextureIDs) {
    int cursorTextureID;
    int i;

    g_Menu->pSelectionMenu->numCursors = 0;
    g_Menu->pSelectionMenu->numTexts = 0;

    for (i = 0; i < numOptions; i++) {
        // If we're on the currently selected option, add 0xD to the cursor texture ID
        // which is expected to be the texture ID of the active cursor
        if (i == selectedIndex) {
            cursorTextureID = pTextureIDs[i * 2] + 0xD;
        } else {
            cursorTextureID = pTextureIDs[i * 2];
        }
        
        g_Menu->pSelectionMenu->numCursors += func_8002675C(
            g_Menu->unk2DC, cursorTextureID, 
            &g_Menu->pSelectionMenu->polysCursors[g_Menu->pSelectionMenu->numCursors * 2],
            g_Menu->renderContext, 
            160, 150, 
            0x1000
        );
        
        g_Menu->pSelectionMenu->numTexts += func_8002675C(
            g_Menu->unk2DC, pTextureIDs[i * 2 + 1], 
            &g_Menu->pSelectionMenu->polysTexts[g_Menu->pSelectionMenu->numTexts * 2], 
            g_Menu->renderContext,
            160, 150, 
            0x1000
        );
    }
    
    g_Menu->pSelectionMenu->cursorsRenderCtx = g_Menu->renderContext;
    g_Menu->pSelectionMenu->textsRenderCtx = g_Menu->renderContext;

    ShopMenuMovePointerCursor(selectedIndex, 1);

    g_Menu->pManager->unk4 = TRUE;
}

INCLUDE_ASM("asm/shop_menu/nonmatchings/main/misc", func_801CC720);

u_char ShopMenuShopModeMenuHandleSelectedOption() {
    u_char isRunning;
    u_char result;

    isRunning = TRUE;
    switch (g_Menu->menu1Choice) {
        case MENU_CHOICE_EXIT:
            isRunning = FALSE;
            break;

        case MENU_CHOICE_SELL:
            result = ShopMenuSellModeMenu();
            break;

        case MENU_CHOICE_BUY:
            result = ShopMenuBuyMenu();
            break;
    }
    
    if (result) {
        ShopMenuStartCloseMenuTransition();
        func_801CBC88(1, 4, g_Menu->unk6E0, &D_801D1FCC, g_Menu->pManager->unkC);
    }
    
    func_801D1F10();

    // Render the selection menu as active again
    g_Menu->pSelectionMenu->unk1192 = FALSE;
    g_Menu->pSelectionMenu->unk1193 = TRUE;

    g_Menu->pManager->unk4 = TRUE;
    g_Menu->pManager->unk3 = TRUE;
    g_Menu->unk337 = 0xFF;
    g_Menu->pManager->unkA = FALSE;
    
    return isRunning;
}

// Buy / Sell / Exit menu
void ShopMenuShopModeMenuMain(void) {
    u_char isRunning;

    isRunning = TRUE;
    g_Menu->menu1Choice = 2;

    ShopMenuInitializeShopModeSelectionMenu(4, &D_801D1F54);

    func_801CBC88(1, 4, g_Menu->unk6E0, &D_801D1FCC, g_Menu->pManager->unkC);
    
    while (isRunning) {
        ShopMenuUpdateAndRender();

        switch (g_Menu->input) {
            case MENU_INPUT_CONFIRM:
                ShopMenuPlaySoundEffect(2);

                // Render the selection menu as disabled
                g_Menu->pSelectionMenu->unk1192 = TRUE;

                func_801C6430();
                func_801CBC88(0, 4, g_Menu->unk6E0, &D_801D1FCC, g_Menu->pManager->unkC);
                g_Menu->unk348->unk15B = 0x4C;
                isRunning = ShopMenuShopModeMenuHandleSelectedOption();
                g_Menu->unk348->unk15B = 0x40;
                break;

            case MENU_INPUT_BACK:
                isRunning = FALSE;
                break;

            case MENU_INPUT_DOWN:
                if (g_Menu->menu1Choice) {
                    g_Menu->menu1Choice--;
                } else {
                    g_Menu->menu1Choice = 2;
                }
                break;

            case MENU_INPUT_UP:
                if (++g_Menu->menu1Choice >= 3) {
                    g_Menu->menu1Choice = 0;
                }
                break;
        }

        // Update the selected option to the active one if we need to
        if (g_Menu->menu1Choice != g_Menu->unk337) {
            ShopMenuUpdateShopModeSelectionMenu(3, g_Menu->menu1Choice, &D_801D1F54);
            func_801CBCF0(4, g_Menu->unk6E0, &D_801D1FCC, &D_801D1FD8, 
                g_Menu->pManager->unkC, g_Menu->menu1Choice, 0, 0
            );
            g_Menu->unk337 = g_Menu->menu1Choice;
        }
    }
}

void ShopMenuMain(void) {
    ShopMenuUnk32CManager(MENU_DATA_INITIALIZE);
    ShopMenuSetManager(MENU_DATA_INITIALIZE);
    ShopMenuSelectionMenuManager(MENU_DATA_INITIALIZE);
    ShopMenuUnk354Manager(MENU_DATA_INITIALIZE);
    ShopMenuUnk330Manager(MENU_DATA_INITIALIZE);
    ShopMenuUnk348Manager(MENU_DATA_INITIALIZE);
    ShopMenuUnk1E20Manager(MENU_DATA_INITIALIZE);
    ShopMenuShopManager(MENU_DATA_INITIALIZE);
    g_Menu->pSelectionMenu->unk1180.x = 0x2C0;
    g_Menu->pSelectionMenu->unk1180.y = 0x100;
    g_Menu->pSelectionMenu->unk1180.w = 0x140;
    g_Menu->pSelectionMenu->unk1180.h = 0xE0;
    g_Menu->unk348->unk15B = 0x40;
    ShopMenuInitialize();
    ShopMenuResetRenderContext();
    func_801C5EE8();
    ShopMenuInitializeBackgrounds();
    ShopMenuInitializeWindowBorders();
    ShopMenuInitializeShopData();
    g_Menu->shouldDrawMenu = TRUE;
    g_Menu->unk32A = TRUE;
    ShopMenuShopModeMenuMain();
    ShopMenuFree();
}

void func_801CCE1C(void* output, u8 characterId) {
    u8* out = output;
    u8* stats = (u8*)&g_GameState + 0x26C + characterId * 0xA4;

    if (stats[0x56] == 4) {
        *(u16*)(out + 0xB8) = stats[0x04] + stats[0x1C];
    } else {
        *(u16*)(out + 0xB8) = stats[0x58] + stats[0x28] + stats[0x04];
    }

    *(u16*)(out + 0xB8) = stats[0x58] + stats[0x28] + stats[0x04];
    *(u16*)(out + 0xBA) = stats[0x5E] + stats[0x2E];
    *(u16*)(out + 0xBC) = stats[0x59] + stats[0x29] + stats[0x2D];
    *(u16*)(out + 0xBE) = stats[0x5F] + stats[0x2F];
    *(u16*)(out + 0xC0) = stats[0x5B] + stats[0x2B];
    *(u16*)(out + 0xC2) = 99;
    *(u16*)(out + 0xC4) = stats[0x5C] + stats[0x2C];
    *(u16*)(out + 0xC6) = 10;
    *(u16*)(out + 0xC8) = stats[0x5A] + stats[0x2A];

    if (*(u16*)(out + 0xB8) >= 1000) *(u16*)(out + 0xB8) = 999;
    if (*(u16*)(out + 0xBA) >= 100) *(u16*)(out + 0xBA) = 99;
    if (*(u16*)(out + 0xBC) >= 1000) *(u16*)(out + 0xBC) = 999;
    if (*(u16*)(out + 0xBE) >= 100) *(u16*)(out + 0xBE) = 99;
    if (*(u16*)(out + 0xC0) >= 1000) *(u16*)(out + 0xC0) = 999;
    if (*(u16*)(out + 0xC2) >= 100) *(u16*)(out + 0xC2) = 99;
    if (*(u16*)(out + 0xC4) >= 1000) *(u16*)(out + 0xC4) = 999;
    if (*(u16*)(out + 0xC6) >= 100) *(u16*)(out + 0xC6) = 99;
    if (*(u16*)(out + 0xC8) >= 100) *(u16*)(out + 0xC8) = 99;
}

// Render Shop Stuff
INCLUDE_ASM("asm/shop_menu/nonmatchings/main/misc", func_801CCFF4);

// Set the color for the stat change a piece of gear provides
void ShopMenuSetStatChangeColor(int numPolygons, POLY_FT4* pPolys, u_char mode) {
    int i;

    for (i = 0; i < numPolygons; i++) {
        SetShadeTex(&pPolys[i*2 + g_Menu->renderContext], 0);
        switch (mode) {
            case MENU_STAT_CHANGE_INCREASE:
                // Red
                setRGB0(&pPolys[i*2 + g_Menu->renderContext], 128, 64, 64);
                break;
            case MENU_STAT_CHANGE_DECREASE:
                // Blue
                setRGB0(&pPolys[i*2 + g_Menu->renderContext], 64, 64, 128);
                break;
        }
    }
}

void ShopMenuUpdateCharacterPortraits(void) {
    int i;
    int characterIndex;
    int numCharacters;

    g_Menu->pManager->unk5A = 1;
    
    // This loop seems horribly inefficient, because for each character index, all
    // portraits are recomputed and the entire menu redrawn...
    for (i = 1; i < MAX_GAME_CHARACTERS + 1; i++) {
        g_Menu->pShop->numPortraits = 0;
        
        numCharacters = 0;
        for (characterIndex = 0; characterIndex < i; characterIndex++) {
            if (g_Menu->availableCharacters[characterIndex]) {
                g_Menu->pShop->numPortraits += func_8002675C(
                    g_Menu->unk2DC, characterIndex + MENU_TEX_CHARACTER_PORTRAITS_SMALL, 
                    &g_Menu->pShop->polysCharacterPortraits[numCharacters * 2], 
                    g_Menu->renderContext, 
                    D_801D21CC[numCharacters], 166, 
                    0x1000
                );
                numCharacters++;
            }
        }
        
        g_Menu->pShop->portraitsRenderCtx = g_Menu->renderContext;
        ShopMenuUpdateAndRender();
    }
}

// Explanations are strings such as "Attack", "Defense", the "- <==> +" graphics.
void ShopMenuUpdateBuyMenuExplanationGraphics(void) {
    int i;

    g_Menu->pShop->explanationsLen = 0;
    for (i = 0; i < 4; i++) {
        g_Menu->pShop->explanationsLen += func_8002675C(
            g_Menu->unk2DC, D_801D2210[i], 
            &g_Menu->pShop->polysExplanations[g_Menu->pShop->explanationsLen * 2],
            g_Menu->renderContext, 
            D_801D2218[i], D_801D2230[i], 
            0x1000
        );
    }
    g_Menu->pShop->explanationsRenderCtx = g_Menu->renderContext;
}

void ShopMenuUpdateSellMenuExplanationGraphics(void) {
    int i;

    g_Menu->pShop->explanationsLen = 0;
    for (i = 0; i < 2; i++) {
        g_Menu->pShop->explanationsLen += func_8002675C(
            g_Menu->unk2DC, D_801D2214[i], 
            &g_Menu->pShop->polysExplanations[g_Menu->pShop->explanationsLen * 2],
            g_Menu->renderContext, 
            D_801D2228[i], D_801D2240[i], 
            0x1000
        );
    }
    g_Menu->pShop->explanationsRenderCtx = g_Menu->renderContext;
}

void ShopMenuUpdateGoldGraphics(u_int goldBefore, u_int totalPrice, u_int goldAfter) {
    int i;
    u_char digit;

    ShopMenuParseNumberToString(goldBefore);
    g_Menu->pShop->goldBeforeStrLen = 0;
    for (i = 0; i < MENU_MAX_DIGITS; i++) {
        digit = g_Menu->digits[i];
        if (digit != 0xFF) {
            g_Menu->pShop->goldBeforeStrLen += func_8002675C(
                g_Menu->unk2DC, digit, 
                &g_Menu->pShop->polysGoldBefore[g_Menu->pShop->goldBeforeStrLen * 2], 
                g_Menu->renderContext, 
                (i * 8) + D_801D2248, D_801D224C, 
                0x1000
            );
        }
    }
    g_Menu->pShop->goldBeforeRenderCtx = g_Menu->renderContext;
    
    ShopMenuParseNumberToString(totalPrice);
    g_Menu->pShop->totalPriceStrLen = 0;
    for (i = 0; i < MENU_MAX_DIGITS; i++) {
        digit = g_Menu->digits[i];
        if (digit != 0xFF) {
            g_Menu->pShop->totalPriceStrLen += func_8002675C(
                g_Menu->unk2DC, digit, 
                &g_Menu->pShop->polysTotalPrice[g_Menu->pShop->totalPriceStrLen * 2], 
                g_Menu->renderContext, 
                (i * 8) + D_801D2250, D_801D2254, 
                0x1000
            );
        }
    }
    g_Menu->pShop->totalPriceRenderCtx = g_Menu->renderContext;
    
    ShopMenuParseNumberToString(goldAfter);
    g_Menu->pShop->goldAfterStrLen = 0;
    for (i = 0; i < MENU_MAX_DIGITS; i++) {
        digit = g_Menu->digits[i];
        if (digit != 0xFF) {
            g_Menu->pShop->goldAfterStrLen += func_8002675C(
                g_Menu->unk2DC, digit, 
                &g_Menu->pShop->polysGoldAfter[g_Menu->pShop->goldAfterStrLen * 2], 
                g_Menu->renderContext, 
                (i * 8) + D_801D2258, D_801D225C, 
                0x1000
            );
        }
    }
    g_Menu->pShop->goldAfterRenderCtx = g_Menu->renderContext;
    
    g_Menu->pShop->unk46B2 = 1;
}

u_short ShopMenuGetCharacterEquippedItemFlags(u_char itemID, u_char itemType) {
    int i;
    int j;
    u_char isItemEquipped;
    u_short equippedFlags;
    
    equippedFlags = 0;
    
    if (itemID) {
        if (itemType != ITEM_TYPE_ITEM) {
            for (i = 0; i < 0x10; i++) {
                isItemEquipped = FALSE;

                if (g_Menu->availableCharacters[i]) {
                    switch (itemType) {
                        case ITEM_TYPE_WEAPON:
                            for (j = 0; j < 5; j++) {
                                if (itemID < 50) {
                                    if (g_GameState.characters[i].unk6A[j] == itemID) {
                                        isItemEquipped = TRUE;
                                        break;
                                    }
                                } else {
                                    if (g_GameState.characters[i].unk6F[j] == itemID) {
                                        isItemEquipped = TRUE;
                                        break;
                                    }
                                }
                            }
                            break;
                        
                        case ITEM_TYPE_ACCESSORY:
                            for (j = 0; j < 3; j++) {
                                if (g_GameState.characters[i].unk74[j] == itemID) {
                                    isItemEquipped = TRUE;
                                    break;
                                }
                            }
                            break;
                    }
                }
                
                if (isItemEquipped) {
                    equippedFlags |= ShopMenuGetCharacterBitMask(i);
                }
            }
        }   
    }

    return equippedFlags;
}

INCLUDE_ASM("asm/shop_menu/nonmatchings/main/misc", func_801CDD14);

INCLUDE_ASM("asm/shop_menu/nonmatchings/main/misc", func_801CE480);

u8 func_801CE8D8(u8* pKeys, u8* pValues, s32 count, u8 target) {
    s32 result = 0;
    u8* pEnd = pValues + count;
    while (pValues < pEnd) {
        if (*pKeys == target) {
            result = *pValues;
            break;
        }
        pValues++;
        pKeys++;
    }
    return (u8)result;
}

INCLUDE_ASM("asm/shop_menu/nonmatchings/main/misc", func_801CE91C);

INCLUDE_ASM("asm/shop_menu/nonmatchings/main/misc", func_801CEB3C);

void ShopMenuHandleBoughtItems(u_int newGoldnewWeaponAmount) {
    int i;
    int j;
    u_char itemNotInInventory;
    u_char newWeaponAmount;
    u_char newAccessorynewWeaponAmount;
    u_char newItemnewWeaponAmount;

    ShopMenuPlaySoundEffect(0xD1);
    
    g_GameState.gold = newGoldnewWeaponAmount;
    if (newGoldnewWeaponAmount > 9999999) {
        g_GameState.gold = 9999999;
    }
    
    for (i = 0; i < MAX_SHOP_ITEMS; i++) {
        // Did we buy any of this item?
        if (g_Menu->shopItemIDs[i] && g_Menu->pShop->curItemQuantities[i]) {
            switch (g_Menu->shopItemTypes[i]) {
                case ITEM_TYPE_WEAPON:
                    itemNotInInventory = TRUE;
                    for (j = 0; j < MAX_INVENTORY_WEAPONS; j++) {
                        if (g_GameState.weaponIDs[j] == g_Menu->shopItemIDs[i]) {
                            itemNotInInventory = FALSE;
                            newWeaponAmount = g_GameState.weaponQuantities[j] + g_Menu->pShop->curItemQuantities[i];
                            g_GameState.weaponQuantities[j] = newWeaponAmount;
                            if (newWeaponAmount > MAX_ITEM_QUANTITY) {
                                g_GameState.weaponQuantities[j] = MAX_ITEM_QUANTITY;
                            }
                        }
                    }

                    // Find an empty inventory slot and set it to the bought item
                    if (itemNotInInventory) {
                        for (j = 0; j < MAX_INVENTORY_WEAPONS; j++) {
                            if (g_GameState.weaponIDs[j] == 0) {
                                g_GameState.weaponIDs[j] = g_Menu->shopItemIDs[i];
                                g_GameState.weaponQuantities[j] = g_Menu->pShop->curItemQuantities[i];
                                break;
                            }
                        }
                    }
                    break;

                case ITEM_TYPE_ACCESSORY:
                    itemNotInInventory = TRUE;
                    for (j = 0; j < MAX_INVENTORY_ACCESSORIES; j++) {
                        if (g_GameState.accessoryIDs[j] == g_Menu->shopItemIDs[i]) {
                            itemNotInInventory = FALSE;
                            newAccessorynewWeaponAmount = g_GameState.accessoryQuantities[j] + g_Menu->pShop->curItemQuantities[i];
                            g_GameState.accessoryQuantities[j] = newAccessorynewWeaponAmount;
                            if (newAccessorynewWeaponAmount > MAX_ITEM_QUANTITY) {
                                g_GameState.accessoryQuantities[j] = MAX_ITEM_QUANTITY;
                            }
                        }
                    }

                    // Find an empty inventory slot and set it to the bought item
                    if (itemNotInInventory) {
                        for (j = 0; j < MAX_INVENTORY_ACCESSORIES; j++) {
                            if (g_GameState.accessoryIDs[j] == 0) {
                                g_GameState.accessoryIDs[j] = g_Menu->shopItemIDs[i];
                                g_GameState.accessoryQuantities[j] = g_Menu->pShop->curItemQuantities[i];
                                break;
                            }
                        }
                    }
                    break;

                case ITEM_TYPE_ITEM:
                    itemNotInInventory = TRUE;
                    for (j = 0; j < MAX_INVENTORY_ITEMS; j++) {
                        if (g_GameState.itemIDs[j] == g_Menu->shopItemIDs[i]) {
                            itemNotInInventory = FALSE;
                            newItemnewWeaponAmount = g_GameState.itemQuantities[j] + g_Menu->pShop->curItemQuantities[i];
                            g_GameState.itemQuantities[j] = newItemnewWeaponAmount;
                            if (newItemnewWeaponAmount > MAX_ITEM_QUANTITY) {
                                g_GameState.itemQuantities[j] = MAX_ITEM_QUANTITY;
                            }
                        }
                    }

                    // Find an empty inventory slot and set it to the bought item
                    if (itemNotInInventory) {
                        for (j = 0; j < MAX_INVENTORY_ITEMS; j++) {
                            if (g_GameState.itemIDs[j] == 0) {
                                g_GameState.itemIDs[j] = g_Menu->shopItemIDs[i];
                                g_GameState.itemQuantities[j] = g_Menu->pShop->curItemQuantities[i];
                                break;
                            }
                        }
                    }
                    break;
            }
        }
    }
}

// Parse and set graphics for final price for confirmation window
void ShopMenuSetFinalPriceGraphics(unsigned int number) {
    int i;
    u_char digit;

    ShopMenuParseNumberToString(number);
    g_Menu->pShop->finalPriceStrLen = 0;
    for (i = 0; i < MENU_MAX_DIGITS; i++) {
        digit = g_Menu->digits[i];
        if (digit != 0xFF) {
            g_Menu->pShop->finalPriceStrLen += func_8002675C(
                g_Menu->unk2DC, digit, 
                &g_Menu->pShop->polysFinalPrice[g_Menu->pShop->finalPriceStrLen * 2],
                g_Menu->renderContext, 
                107 + i * 8, 84, 
                0x1000
            );
        }
    }
    g_Menu->pShop->finalPriceRenderCtx = g_Menu->renderContext;
    g_Menu->pManager->unk5B = 2;
}

u_char ShopMenuBuyMenu(void) {
    u8 sp28[0x4]; // ???
    u_char isRunning;
    u_char shouldInitialize;
    u_char refreshGoldGraphics;
    int initialGold;
    int newGoldAmount;
    int totalPrice;
    int itemPrice;
    int curChoice;
    int prevChoice;
    int scrollOffset;
    int prevScrollOffset;
    int i;

    shouldInitialize = TRUE;
    isRunning = TRUE;
    refreshGoldGraphics = TRUE;
    
    curChoice = 0;
    prevChoice = 0xFF;
    scrollOffset = 0;
    prevScrollOffset = 0xFF;
    
    initialGold = g_GameState.gold;
    totalPrice = 0;
    newGoldAmount = g_GameState.gold;
    
    for (i = 0; i < MAX_GAME_CHARACTERS; i++) {
        func_801CCE1C(g_Menu->unk330, i);
        g_Menu->pShop->unk46E0[i] = g_Menu->unk330->unkB8;
        g_Menu->pShop->unk4700[i] = g_Menu->unk330->unkBC;
    }
    
    bzero(g_Menu->pShop->curItemQuantities, MAX_SHOP_ITEMS);
    g_Menu->pSelectionMenu->unk1192 = TRUE;
    
    ShopMenuInitializeArrowCursor(0);
    
    while (isRunning) {
        ShopMenuUpdateAndRender();
        
        if ((scrollOffset != prevScrollOffset) || refreshGoldGraphics) {
            func_801CDD14(scrollOffset, newGoldAmount, &sp28);
            ShopMenuUpdateScrollBarHandle(12, 50, 60, D_801D1F50, scrollOffset);
        }

        // Did we change our currently selected item in any way?
        if ((curChoice != prevChoice) || (scrollOffset != prevScrollOffset)) {
            itemPrice = func_801CEB3C(curChoice, scrollOffset, &sp28);
            prevChoice = curChoice;
            prevScrollOffset = scrollOffset;
            g_Menu->pManager->unk5A = TRUE;
        }
        
        ShopMenuUpdateArrowCursor(curChoice, scrollOffset, 0, 0);
        
        if (shouldInitialize) {
            func_801CBC88(1, 2, g_Menu->unk6E0, &D_801D1FD4, g_Menu->pManager->unkC);
            func_801CBCF0(2, g_Menu->unk6E0, &D_801D1FD4, &D_801D1FE8, g_Menu->pManager->unkC, 0, 0, 1);
            g_Menu->pManager->unkC[0] = FALSE;
            ShopMenuInitializeWindow(2, 0xC, 0x2A, 0xC4, 0x74, 0, 1, 4, 1);
            ShopMenuInitializeWindow(3, 0x20, 0xE, 0xFC, 0x14, 0, 1, 4, 0);
            ShopMenuInitializeWindow(5, 0xE0, 0x7A, 0x40, 0x24, 0, 1, 4, 0);
            ShopMenuStartOpenMenuTransition();
            ShopMenuUpdateCharacterPortraits();
            ShopMenuUpdateBuyMenuExplanationGraphics();
            shouldInitialize = FALSE;
            while (g_Menu->transitionEffectState != MENU_ANIMATION_DONE) {
                ShopMenuUpdateAndRender();
            }
            g_Menu->pManager->unkC[0] = TRUE;
        }
        
        if (refreshGoldGraphics) {
            ShopMenuUpdateGoldGraphics(initialGold, totalPrice, newGoldAmount);
            refreshGoldGraphics = FALSE;
        }
        
        switch (g_Menu->input) {
            case MENU_INPUT_CONFIRM:
                if (totalPrice != 0) {
                    ShopMenuPlaySoundEffect(2);
                    g_Menu->pManager->unk5A = FALSE;
                    g_Menu->pManager->shouldRenderWindow[2] = FALSE;
                    g_Menu->pManager->shouldRenderWindow[3] = FALSE;
                    g_Menu->pManager->shouldRenderWindow[5] = FALSE;
                    g_Menu->pManager->scrollHandleActive = FALSE;
                    g_Menu->pManager->shouldRenderArrowCursor[0] = FALSE;
                    isRunning = FALSE;
                    g_Menu->pManager->unkC[0] = FALSE;
                    ShopMenuInitializePointerCursors(0);
                    ShopMenuSetFinalPriceGraphics(totalPrice);
                    if (ShopMenuConfirmationWindow(0x8F, 0xFF, 1) != MENU_CHOICE_NO) {
                        ShopMenuHandleBoughtItems(newGoldAmount);
                    } else {
                        isRunning = TRUE;
                        g_Menu->pManager->shouldRenderWindow[2] = TRUE;
                        g_Menu->pManager->shouldRenderWindow[3] = TRUE;
                        g_Menu->pManager->shouldRenderWindow[5] = TRUE;
                        g_Menu->pManager->scrollHandleActive = TRUE;
                        g_Menu->pManager->shouldRenderArrowCursor[0] = TRUE;
                        prevScrollOffset = 0xFF;
                        prevChoice = 0xFF;
                        g_Menu->pManager->unkC[0] = TRUE;
                    }
                    ShopMenuFreePointerCursors();
                } else {
                    ShopMenuPlaySoundEffect(4);
                }
                break;
            
            case MENU_INPUT_BACK:
                isRunning = FALSE;
                g_Menu->pManager->unkC[0] = 0;
                if (totalPrice != 0) {
                    g_Menu->pManager->unk5A = FALSE;
                    g_Menu->pManager->shouldRenderWindow[2] = FALSE;
                    g_Menu->pManager->shouldRenderWindow[3] = FALSE;
                    g_Menu->pManager->shouldRenderWindow[5] = FALSE;
                    g_Menu->pManager->scrollHandleActive = FALSE;
                    g_Menu->pManager->shouldRenderArrowCursor[0] = FALSE;
                    ShopMenuInitializePointerCursors(0);
                    if (ShopMenuConfirmationWindow(0x8C, 0xFF, 1) == MENU_CHOICE_NO) {
                        isRunning = TRUE;
                        g_Menu->pManager->shouldRenderWindow[2] = TRUE;
                        g_Menu->pManager->shouldRenderWindow[3] = TRUE;
                        g_Menu->pManager->shouldRenderWindow[5] = TRUE;
                        g_Menu->pManager->scrollHandleActive = TRUE;
                        g_Menu->pManager->shouldRenderArrowCursor[0] = TRUE;
                        prevScrollOffset = 0xFF;
                        prevChoice = 0xFF;
                        g_Menu->pManager->unkC[0] = TRUE;
                    }
                    ShopMenuFreePointerCursors();
                }
                ShopMenuUpdateAndRender();
                break;
            
            case MENU_INPUT_DOWN:
                curChoice++;

                // If we move beyond the items visible at a time, we need to scroll down
                if (curChoice >= MAX_ITEMS_IN_VIEW) {
                    curChoice = MAX_ITEMS_IN_VIEW - 1;
                    scrollOffset++;

                    // If we're at the bottom of the window, we undo our scrolling,
                    // since there's nothing mroe to scroll to
                    if ((D_801D1F50 - MAX_ITEMS_IN_VIEW) < scrollOffset) {
                        scrollOffset--;
                    }
                }
                break;
            
            case MENU_INPUT_UP:
                curChoice--;
                if (curChoice < 0) {
                    scrollOffset--;
                    curChoice = 0;
                    if (scrollOffset < 0) {
                        scrollOffset = 0;
                    }
                }
                break;

            // Increase item quantity
            case MENU_INPUT_RIGHT:
                if (sp28[curChoice] != 0) {
                    if (g_Menu->pShop->curItemQuantities[scrollOffset + curChoice] + 1 + D_801D2260 < 100) {
                        g_Menu->pShop->curItemQuantities[scrollOffset + curChoice] += 1;
                        totalPrice += itemPrice;
                        newGoldAmount -= itemPrice;
                        refreshGoldGraphics = TRUE;
                    }
                }
                break;

            // Decrease item quantity
            case MENU_INPUT_LEFT:
                if (g_Menu->pShop->curItemQuantities[scrollOffset + curChoice] != 0) {
                    g_Menu->pShop->curItemQuantities[scrollOffset + curChoice] -= 1;
                    totalPrice -= itemPrice;
                    newGoldAmount += itemPrice;
                    refreshGoldGraphics = TRUE;
                }
                break;
        }
    }
    
    g_Menu->pShop->unk4785 = 0;
    return TRUE;
}

INCLUDE_ASM("asm/shop_menu/nonmatchings/main/misc", func_801CFF58);

INCLUDE_ASM("asm/shop_menu/nonmatchings/main/misc", func_801D05BC);

INCLUDE_ASM("asm/shop_menu/nonmatchings/main/misc", ShopMenuHandleSoldItems);

INCLUDE_ASM("asm/shop_menu/nonmatchings/main/misc", ShopMenuSellMenu);

INCLUDE_ASM("asm/shop_menu/nonmatchings/main/misc", ShopMenuSellEquipmentMenu);

void ShopMenuSellAccessoriesMenu(void) {
    ShopMenuSellMenu(
        MAX_INVENTORY_ACCESSORIES, 
        g_GameState.accessoryIDs, 
        g_GameState.accessoryQuantities, 
        ITEM_TYPE_ACCESSORY, 
        1, 
        g_GameState.accessoryQuantities, 
        0
    );
}

void ShopMenuSellWeaponsMenu(void) {
    ShopMenuSellMenu(
        MAX_INVENTORY_WEAPONS, 
        g_GameState.weaponIDs, 
        g_GameState.weaponQuantities, 
        ITEM_TYPE_WEAPON, 
        1, 
        g_GameState.weaponQuantities, 
        0
    );
}

void ShopMenuSellItemsMenu(void) {
    ShopMenuSellMenu(
        MAX_INVENTORY_ITEMS, 
        g_GameState.itemIDs, 
        g_GameState.itemQuantities, 
        ITEM_TYPE_ITEM, 
        1, 
        g_GameState.itemQuantities, 
        0
    );
}

void func_801D1968(u_char arg0, u_char arg1) {
    int i;

    g_Menu->pManager->unk5A = 0;
    g_Menu->pShop->unk46A7 = 0;
    g_Menu->pShop->unk46B5 = 0;
    g_Menu->pShop->unk46B2 = 0;
    g_Menu->pShop->explanationsLen = 0;
    g_Menu->pShop->unk46A9 = 0;
    g_Menu->pShop->numPortraits = 0;

    for (i = 0; i < 9; i++) {
        g_Menu->pShop->unk469C[i] = 0;
        g_Menu->pShop->unk46BC[i] = 0;
        g_Menu->pShop->unk46C5[i] = 0;
    }

    for (i = 0; i < 8; i++) {
        g_Menu->pShop->unk4684[i] = 0;
        g_Menu->pShop->unk468C[i] = 0;
    }
    
    if (arg0) {
        ShopMenuFreeWindow(2);
        ShopMenuFreeWindow(3);
        if (arg1) {
            ShopMenuFreeWindow(5);
        }
        ShopMenuFreeScrollBarHandle();
        ShopMenuFreeArrowCursor(0);
    }
}

void ShopMenuSellModeMenuHandleSelectedOption(void) {
    u_char var_a0;

    g_Menu->pManager->unk4 = 0;
    g_Menu->pManager->unk3 = 0;
    g_Menu->pManager->unkA = 0;
    func_801CBC88(0, 4, g_Menu->unk6E0, &D_801D1FD0, g_Menu->pManager->unkC);
    
    var_a0 = 1;
    switch (g_Menu->menu2Choice) {
        case MENU_CHOCIE_EQUIPMENT:
            var_a0 = ShopMenuSellEquipmentMenu();
            break;
        
        case MENU_CHOICE_ACCESSORIES:
            ShopMenuSellAccessoriesMenu();
            break;

        case MENU_CHOICE_WEAPONS:
            ShopMenuSellWeaponsMenu();
            break;

        case MENU_CHOICE_ITEMS:
            ShopMenuSellItemsMenu();
            break;
    }
    
    func_801D1968(var_a0, 0);
    g_Menu->pManager->unkA = 1;
    g_Menu->pManager->unk4 = 1;
    g_Menu->pManager->unk3 = 1;
    func_801CBC88(1, 4, g_Menu->unk6E0, &D_801D1FD0, g_Menu->pManager->unkC);
}

// Items / Weapons / Accessories / Equipment
int ShopMenuSellModeMenu() {
    u_char shouldInitialize;
    u_char isRunning;

    isRunning = TRUE;
    shouldInitialize = TRUE;
    g_Menu->menu2Choice = MENU_CHOICE_ITEMS;
    g_Menu->unk339 = 0xFF;
    
    while (isRunning) {
        ShopMenuUpdateAndRender();
        
        if (shouldInitialize) {
            func_801CBC88(1, 4, g_Menu->unk6E0, &D_801D1FD0, g_Menu->pManager->unkC);
            ShopMenuStartOpenMenuTransition();
            func_801CC278(0);
            shouldInitialize = FALSE;
        }
        
        if (g_Menu->menu2Choice != g_Menu->unk339) {
            func_801CBCF0(4, g_Menu->unk6E0, &D_801D1FD0, &D_801D1FE8, g_Menu->pManager->unkC, g_Menu->menu2Choice, 3, 0);
            func_801CC720(0);
            g_Menu->unk339 = g_Menu->menu2Choice;
        }
        
        switch (g_Menu->input) {
            case MENU_INPUT_CONFIRM:
                ShopMenuPlaySoundEffect(2);
                ShopMenuSellModeMenuHandleSelectedOption();
                g_Menu->unk339 = 0xFF;
                break;

            case MENU_INPUT_BACK:
                isRunning = FALSE;
                break;
            
            case MENU_INPUT_DOWN:
                if (g_Menu->menu2Choice != 0) {
                    g_Menu->menu2Choice--;
                } else {
                    g_Menu->menu2Choice = g_Menu->unk33A - 1;
                }
                break;

            case MENU_INPUT_UP:
                if (++g_Menu->menu2Choice >= g_Menu->unk33A) {
                    g_Menu->menu2Choice = 0;
                }
                break;
        }
    }
    
    g_Menu->pManager->unk4 = 0;
    g_Menu->pManager->unk3 = 0;
    func_801CBC88(0, 4, g_Menu->unk6E0, &D_801D1FD0, g_Menu->pManager->unkC);
    return TRUE;
}

void func_801D1F10(void) {
    if (g_Menu->menu1Choice == 1) {
    } else if (g_Menu->menu1Choice == 2) {
        func_801D1968(1, 1);
    }
}
