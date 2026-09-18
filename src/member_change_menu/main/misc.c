#include "common.h"

#include "psyq/libgpu.h"
#include "system/controller.h"
#include "system/menu.h"
#include "main/game.h"

// tmp
u_char MemberChangeMenuSwapCharacters(u_char, s32, s32, u_char, s32, s32);

// Bit masks
extern u_short D_801CB57C[];

extern int D_801CB180[]; // X positions for cursor?
// = { 0, 0, 180, 276 }
extern int D_801CB190[]; // Y positions for cursor?
// = { 0, 0, 200, 200}


// First 3 indices = party members
// Last 6 indices = benched members
extern int g_MemberChangeMenuCurserPositionsX[];
// = { 32, 32, 32, 144, 144, 144, 144, 144, 144 }
extern int g_MemberChangeMenuCurserPositionsY[];
// = { 38, 94, 150, 22,  54, 86, 118, 150, 182}

// Cursor position indices
extern s32 D_801CB404[];

// Cursor X positions
extern s32 D_801CB47C[];
// = { 0x20, 0x28, 0x30, 0x38, 0x40, 0x48, 0x50, 0x58,
//     0x60, 0x68, 0x70, 0x78, 0x80, 0x88, 0x90, 0x140,
//     0xB0, 0xB8, 0xC0, 0xC8, 0xD0, 0xD8, 0xE0, 0xE8,
//     0xF0, 0xF8, 0x100, 0x108, 0x110, 0x118, 0x120, 0x140 };

// Cursor Y positions
extern s32 D_801CB4FC[];
// = { 0xE, 0x22, 0x36, 0xE, 0x22, 0x36, 0xE, 0x22,
//     0x36, 0xE, 0x22, 0x36, 0xE, 0x22, 0x36, 0x100,
//     0xE, 0x22, 0x36, 0xE, 0x22, 0x36, 0xE, 0x22,
//     0x36, 0xE, 0x22, 0x36, 0xE, 0x22, 0x36, 0x100 };

extern int g_MemberChangeMenuBenchedCharPositionsX[];
extern int g_MemberChangeMenuCurCharPositionsX[];
extern int g_MemberChangeMenuBenchedCharPositionsY[];
extern int g_MemberChangeMenuCurCharPositionsY[];
extern int g_MemberChangeMenuCharTexcoordsU[];
extern int g_MemberChangeMenuCharTexcoordsV[];

extern s32 D_801CB3DC[];
// = { 0x15, 0x1F, 0xFFFF, 0x11, 0x19, 0x3E, 0x16, 0x19, 0x3E }
// = { 'L', 'V',   0xFFFF, 'H',  'P',  '/',  'M'   'P'   '/'  }

extern void* D_8005945C; // Pointer to menu resources file
extern void* D_8006259C; // SEDS file pointer
extern u8 g_MenuDebugEnabled;

extern u16 g_SystemPalette1;
extern u16 g_SystemPalette2;

static inline void MenuSetSystemPalette(void* pPrim, u8 flag) {
    POLY_FT4* pPoly_ = pPrim;
    pPoly_->clut = flag ? g_SystemPalette2 : g_SystemPalette1;
}

u_short MemberChangeMenuIsCharacterFlagSet(u_short value, u_char maskIndex) {
    return D_801CB57C[maskIndex] & value;
}

void func_801C5034(u_char isInitialization) {
    if (isInitialization) {
        g_Menu->unk32C = HeapAlloc(sizeof(MenuUnk2), 0);
        bzero(g_Menu->unk32C, sizeof(MenuUnk2));
        return;
    }
    HeapFree(g_Menu->unk32C);
}

// Initialize or free the manager which handles the menu logic
void MemberChangeMenuSetManager(u_char isInitialization) {
    if (isInitialization) {
        g_Menu->pManager = HeapAlloc(sizeof(MenuManager), 0);
        bzero(g_Menu->pManager, sizeof(MenuManager));
        return;
    }
    HeapFree(g_Menu->pManager);
}

void MemberChangeMenuSetSelectionMenu(u_char bIsInitialization) {
    if (bIsInitialization) {
        g_Menu->pSelectionMenu = HeapAlloc(sizeof(MenuSelectionMenu), 0);
        bzero(g_Menu->pSelectionMenu, sizeof(MenuSelectionMenu));
        return;
    }
    HeapFree(g_Menu->pSelectionMenu);
}

void func_801C5160(u_char isInitialization) {
    if (isInitialization) {
        g_Menu->unk354 = HeapAlloc(sizeof(MenuUnk5), 0);
        bzero(g_Menu->unk354, sizeof(MenuUnk5));
        return;
    }
    HeapFree(g_Menu->unk354);
}

void func_801C51C4(u_char isInitialization) {
    if (isInitialization) {
        g_Menu->unk330 = HeapAlloc(sizeof(MenuUnk6), 0);
        bzero(g_Menu->unk330, sizeof(MenuUnk6));
        return;
    }
    HeapFree(g_Menu->unk330);
}

void func_801C5228(u_char isInitialization) {
    if (isInitialization) {
        g_Menu->unk348 = HeapAlloc(sizeof(MenuUnk1), 0);
        bzero(g_Menu->unk348, sizeof(MenuUnk1));
        return;
    }
    HeapFree(g_Menu->unk348);
}

// Initialize or free character data used by the menu
void MemberChangeMenuSetCharacters(u_char isInitialization) {
    int i;

    if (isInitialization) {
        for (i = 0; i < MAX_BENCHED_PARTY_MEMBERS; i++) {
            g_Menu->benchedCharacters[i] = HeapAlloc(sizeof(MenuCharacter), 0);
            bzero(g_Menu->benchedCharacters[i], sizeof(MenuCharacter));
        }

        for (i = 0; i < MAX_PARTY_MEMBERS; i++) {
            g_Menu->currentCharacters[i] = HeapAlloc(sizeof(MenuCharacter), 0);
            bzero(g_Menu->currentCharacters[i], sizeof(MenuCharacter));
        }
        return;
    }

    for (i = 0; i < MAX_BENCHED_PARTY_MEMBERS; i++) {
        HeapFree(g_Menu->benchedCharacters[i]);
    }

    for (i = 0; i < MAX_PARTY_MEMBERS; i++) {
        HeapFree(g_Menu->currentCharacters[i]);
    }
}

void MemberChangeMenuLoadResources(void) {
    void* pResourceEntry;
    u32* pResources;
    u8 _unused[0x60];

    pResources = D_8005945C;
    ResolveArchiveEntryPointers(pResources);
    
    // Xenogears Icon TIM
    pResourceEntry = LZSSHeapDecompress(pResources[1], 1);
    OpenTIM(pResourceEntry);
    ReadTIM(&g_Menu->unk32C->tim);
    g_Menu->unk32C->unk4B94 = 0x53;
    g_Menu->unk32C->unk4B95 = 0x43;
    g_Menu->unk32C->unk4B96 = 0x11;
    g_Menu->unk32C->unk4B97 = 1;
    bzero(g_Menu->unk32C->unk4B98, 0x5C);
    memmove(&g_Menu->unk32C->unk4BF4, g_Menu->unk32C->tim.caddr, 0x20);
    memmove(&g_Menu->unk32C->unk4C14, g_Menu->unk32C->tim.paddr, 0x80);
    HeapFree(pResourceEntry);
    
    // Menu TIM Textures
    pResourceEntry = LZSSHeapDecompress(pResources[2], 1);
    func_8002DD20(pResourceEntry);
    HeapFree(pResourceEntry);
    
    // Table for where to find textures (UVs, sizes etc.) for the TIMs loaded above
    g_Menu->unk2DC = LZSSHeapDecompress(pResources[3], 0);

    g_Menu->unk2E0 = LZSSHeapDecompress(pResources[4], 0);
    
    if (g_MenuDebugEnabled) {
        ArchiveSetIndex(0x10, 0x2);
        D_8006259C = HeapAlloc(ArchiveDecodeAlignedSize(0x5), 0);
        ArchiveReadFileToBuffer(0x5, D_8006259C, 0, 0x80);
        ArchiveCdDataSync(0);
        ArchiveSetIndex(0x10, 0x0);
        SoundAddSedsEntry(D_8006259C);
    }
    g_Menu->unk2E4 = D_8006259C;
    
    HeapFree(pResources);
}

void MemberChangeMenuInitialize(void) {
    int flags;
    int i;
    int partyMemberID;

    g_Menu->menu1Choice = 4;
    g_Menu->unk337 = 0xFF;
    g_Menu->unk326 = 0x3C;
    g_Menu->unk334 = 0;
    g_Menu->unk335 = 0;
    
    // Check flags to see which characters are available and not
    flags = (g_GameState.unk1D30 & g_GameState.FrMask) & 0x7FF;
    for (i = 0; i < 0x10; i++) {
        if (MemberChangeMenuIsCharacterFlagSet(flags, i)) {
            g_Menu->availableCharacters[i] = TRUE;
        } else {
            g_Menu->availableCharacters[i] = FALSE;
        }
    }

    // Set our current characters in the menu
    for (i = 0; i < MAX_PARTY_MEMBERS; i++) {
        partyMemberID = g_GameState.partyMembers[i];
        if (partyMemberID != 0xFF && g_Menu->availableCharacters[partyMemberID]) {
            g_Menu->pManager->currentCharacterIDs[i] = partyMemberID;
        } else {
            g_Menu->pManager->currentCharacterIDs[i] = 0xFF;
        }
    }

    MemberChangeMenuLoadResources();
}

void MemberChangeMenuResetRenderContext(void) {
    g_Menu->renderContext = 0;
}

void func_801C5724(void) {
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

void func_801C57A0(MenuString* pString, int index, s32 arg2, u32 attributes) {
    s32 var_s1;
    int i;
    POLY_FT4* pPoly;
    u32 attr  = attributes;

    for (i = 0; i < 2; i++) {
        pPoly = &pString->polys[i];
        var_s1 = 0;

        SetPolyFT4(pPoly);
        SetSemiTrans(pPoly, 0);
        SetShadeTex(pPoly, 0);
        setRGB0(pPoly, 128, 128, 128);
        
        if (!(attr & 0xFF)) {
            pString->unk7C = (index & 1);
            setTPage(pPoly, 0, 0, 320, 0);
            
            #define texCoordV  (((index + arg2) / 4) * 13)
            #define texCoordU  (((index / 2) & 1) << 7)
            setUV4(
                pPoly,
                texCoordU,                   texCoordV,
                texCoordU + pString->width,  texCoordV,
                texCoordU,                   texCoordV + 13,
                texCoordU + pString->width,  texCoordV + 13
            );
            #undef texCoordV
            #undef texCoordU
        } else {
            if (!(attr & 0x80)) {
                var_s1 = 0x20;
                SetSemiTrans(pPoly, 1);
                setRGB0(pPoly, var_s1, var_s1, var_s1);
            }
            
            pString->unk7C = (attr & 0x7F) + 0xFF;
            setTPage(pPoly, 0, 0, 0x180, 0x80);
            pPoly->tpage = var_s1 | pPoly->tpage;
            
            #define texCoordU ((index & 1) * 0x60)
            #define texCoordV (((index / 2) * 13) + arg2)
            setUV4(
                pPoly,
                texCoordU,                   texCoordV,
                texCoordU + pString->width,  texCoordV,
                texCoordU,                   texCoordV + 13,
                texCoordU + pString->width,  texCoordV + 13
            );
            #undef texCoordV
            #undef texCoordU
        }

        MenuSetSystemPalette(pPoly, pString->unk7C);
    }

    pString->unk7F = 0;
}

extern void* GetStringEntry(void* table, s32 index);
extern s32 SystemRenderStringEntry(void* pString, void* pWork, s32 height, s32 flag);

/* Retail 0x801C59E0..0x801C5BEC: draw paired menu labels and initialize
 * their storage. Each lookup completes before the work-buffer pointer reload;
 * host C argument evaluation must not move that load before GetStringEntry.
 * The typed RECT copy retains retail's unaligned copy and native field layout.
 * Index caching preserves the PSYQ loop induction-variable ordering. */
void func_801C59E0(MenuString* base, u8* strIndices, s32 arg2, s32 count) {
    s32 i;
    RECT* rect;
    void* entry;
    for (i = 0; i < count; i += 2) {
        MenuString* item;
        u8* index = &strIndices[i];
        entry = GetStringEntry(g_Menu->unk2E0, *index);
        item = &base[i];
        item->width = SystemRenderStringEntry(
            entry,
            g_Menu->unk4E0[0].pVramBuffer, 0x18, 0);
        rect = &item->vramDest;
        entry = GetStringEntry(g_Menu->unk2E0, index[1]);
        base[i + 1].width = SystemRenderStringEntry(
            entry,
            g_Menu->unk4E0[0].pVramBuffer, 0x18, 1);
        item->vramDest.x = (i / 2 & 1) * 32 + 320;
        item->vramDest.y = (i + arg2) / 4 * 13;
        item->vramDest.w = 28;
        item->vramDest.h = 13;
        base[i + 1].vramDest = item->vramDest;
        func_801C57A0(item, i, arg2, 0);
        func_801C57A0(&base[i + 1], i + 1, arg2, 0);
        LoadImage(rect, (u_long*)g_Menu->unk4E0[0].pVramBuffer);
        DrawSync(0);
    }
}

extern u8 D_801CB400[];

void func_801C5B90(void) {
    SystemTransferPaletteToVRAM(0, 0x1D1);
    g_Menu->unk4E0[0].pVramBuffer = HeapAlloc(0x38E, 0);
    func_801C59E0(g_Menu->unk4E0, D_801CB400, 0, 4);
    func_801C5724();
}

void MemberChangeMenuInitializeWindowBorders(void) {
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

void func_801C5CF4(void) {
    g_Menu->pManager->unk4 = FALSE;
    g_Menu->pManager->unk3 = FALSE;
}

// Initialize POLY_G4
void func_801C5D24(POLY_G4* pPoly, u_char red, u_char green, u_char blue) {
    SetPolyG4(pPoly);
    setRGB0(pPoly, red, green, blue);
    setRGB1(pPoly, red, green, blue);
    setRGB2(pPoly, 0, 0, 0);
    setRGB3(pPoly, 0, 0, 0);
}

void MemberChangeMenuInitializeBackground(void) {
    RECT rect;
    int i;

    rect.y = 0;
    rect.x = 0;
    rect.h = 256;
    rect.w = 256;
    
    func_801C5CF4();

    for (i = 0; i < 2; i++) {
        // Unknown effect, seems to be unused
        func_801C5D24(&g_Menu->unk348->polyG4s[i], 128, 128, 0);
        SetSemiTrans(&g_Menu->unk348->polyG4s[i], 1);
        
        SetLineF3(&g_Menu->unk348->lines1[i]);
        setRGB0(&g_Menu->unk348->lines1[i], 0, 64, 0);
        
        SetLineF3(&g_Menu->unk348->lines2[i]);
        setRGB0(&g_Menu->unk348->lines2[i], 0, 64, 0);
        
        // Background dimming screen
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

// Set vertices relative to center of screen?
void MemberChangeMenuSetVertices(SVECTOR* pVertices, u_short x, u_short y, u_short width, u_short height) {
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

void MemberChangeMenuSetWindowBorderPrimitive(POLY_FT4* pPrim) {
    SetSemiTrans(pPrim, 1);
    SetShadeTex(pPrim, 0);
    setRGB0(pPrim, 128, 128, 128);
}

void MemberChangeMenuInitializeWindowGraphics(unsigned char index) {
    RECT rect;
    MenuWindow* pWindow;
    unsigned char i;

    pWindow = g_Menu->windows[index];
    
    rect.y = 0;
    rect.x = 0;
    rect.h = 0x100;
    rect.w = 0x100;
    
    g_Menu->pManager->shouldRenderWindow[index] = FALSE;
    g_Menu->pManager->unk27[index] = 0;
    
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

void MemberChangeMenuInitializeScrollBar(unsigned char index, u_short x, u_short y, u_short width, u_short height) {
    MenuWindow* pWindow;
    
    pWindow = g_Menu->windows[index];
    
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
    
    MemberChangeMenuSetVertices(pWindow->vertsScrollBarEnds, x, y, 8, 8);
    MemberChangeMenuSetVertices(&pWindow->vertsScrollBarEnds[4], x, y + height, 8, -8);
    MemberChangeMenuSetVertices(pWindow->vertsScrollBarEmpty, x, y + 8, 8, height - 8);
}

void MemberChangeMenuInitializeWindowBorderCorners(unsigned char index, u_short x, u_short y, u_short width, u_short height) {
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
    
    MemberChangeMenuSetVertices(
        pWindow->vertsWindowBorderCorners, 
        x - 8, 
        y + 8, 
        16, -16
    );
    MemberChangeMenuSetVertices(
        &pWindow->vertsWindowBorderCorners[4], 
        x + width + 8, 
        y + 8, 
        -16, -16
    );
    MemberChangeMenuSetVertices(
        &pWindow->vertsWindowBorderCorners[8], 
        x - 8, 
        y + height - 8, 
        16, 16
    );
    MemberChangeMenuSetVertices(
        &pWindow->vertsWindowBorderCorners[0xC], 
        x + width + 8,
        y + height - 8,
        -16, 16
    );

    for (i = 0; i < 4; i++) {
        MemberChangeMenuSetWindowBorderPrimitive(&pWindow->polysWindowBorderCorners[i * 2 + g_Menu->renderContext]);
    }
}

void MemberChangeMenuSetWindowBorderTop(unsigned char index, u_short x, u_short y, u_short width) {
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
    
    MemberChangeMenuSetVertices(
        pWindow->vertsWindowBorderTop1, 
        x + MENU_WINDOW_BORDER_SIZE,
        y - MENU_WINDOW_BORDER_SIZE,
        halfInnerWidth,
        MENU_WINDOW_BORDER_SIZE * 2
    );
    
    MemberChangeMenuSetVertices(
        pWindow->vertsWindowBorderTop2, 
        x + MENU_WINDOW_BORDER_SIZE + halfInnerWidth, 
        y - MENU_WINDOW_BORDER_SIZE,
        halfInnerWidth, 
        MENU_WINDOW_BORDER_SIZE * 2
    );

    for (i = 0; i < 2; i++) {
        MemberChangeMenuSetWindowBorderPrimitive(&pWindow->polysWindowBorderTop[i * 2 + g_Menu->renderContext]);
    }
}

void MemberChangeMenuSetWindowBorderBottom(u_char index, u_short x, u_short y, u_short width, u_short height) {
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
    
    MemberChangeMenuSetVertices(
        pWindow->vertsWindowBorderBottom1, 
        x + 8, 
        height, 
        width, 16
    );
    MemberChangeMenuSetVertices(
        pWindow->vertsWindowBorderBottom2, 
        x + 8 + width, 
        height, 
        width, 16
    );

    for (i = 0; i < 2; i++) {
        MemberChangeMenuSetWindowBorderPrimitive(&pWindow->polysWindowBorderBottom[i * 2 + g_Menu->renderContext]);
    }
}

void MemberChangeMenuSetWindowBorderLeft(u_char index, u_short x, u_short y, u_short height) {
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
    
    MemberChangeMenuSetVertices(
        pWindow->vertsWindowBorderLeft1, 
        x - 8, y + 8, 
        16, halfInnerHeight
    );
    MemberChangeMenuSetVertices(
        pWindow->vertsWindowBorderLeft2, 
        x - 8, y + 8 + halfInnerHeight, 
        16, halfInnerHeight
    );

    for (i = 0; i < 2; i++) {
        MemberChangeMenuSetWindowBorderPrimitive(&pWindow->polysWindowBorderLeft[i * 2 + g_Menu->renderContext]);
    }
}

void MemberChangeMenuSetWindowBorderRight(u_char index, u_short x, u_short y, u_short width, u_short height) {
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
    
    MemberChangeMenuSetVertices(
        pWindow->vertsWindowBorderRight1, 
        width, y + 8, 
        16, halfInnerHeight
    );
    MemberChangeMenuSetVertices(
        pWindow->vertsWindowBorderRight2, 
        width, y + 8 + halfInnerHeight,
        16, halfInnerHeight
    );

    for (i = 0; i < 2; i++) {
        MemberChangeMenuSetWindowBorderPrimitive(&pWindow->polysWindowBorderRight[i * 2 + g_Menu->renderContext]);
    }
}

void MemberChangeMenuSetWindow(u_char index, u_short x, u_short y, u_short width, u_short height, u_char arg5, int zIndex, u_char hasScrollBar) {
    MenuWindow* pWindow;

    pWindow = g_Menu->windows[index];
    g_Menu->pManager->shouldRenderWindow[index] = FALSE;
    MemberChangeMenuSetVertices(pWindow->vertsBackground, x, y, width, height);
    MemberChangeMenuInitializeWindowBorderCorners(index, x, y, width, height);
    MemberChangeMenuSetWindowBorderTop(index, x, y, width);
    MemberChangeMenuSetWindowBorderBottom(index, x, y, width, height);
    MemberChangeMenuSetWindowBorderLeft(index, x, y, height);
    MemberChangeMenuSetWindowBorderRight(index, x, y, width, height);
    if (hasScrollBar) {
        MemberChangeMenuInitializeScrollBar(index, x, y, width, height);
    }
    pWindow->hasScrollBar = hasScrollBar;
    pWindow->unk714 = arg5;
    pWindow->zIndex = zIndex;
    pWindow->renderContext = g_Menu->renderContext;
    g_Menu->pManager->shouldRenderWindow[index] = TRUE;
}

void MemberChangeMenuFreeWindow(u_char index) {
    g_Menu->pManager->shouldRenderWindow[index] = FALSE;
    g_Menu->pManager->unk27[index] = 0;
    HeapFree(g_Menu->windows[index]);
    HeapFree(g_Menu->windowParameters[index]);
}

void MemberChangeMenuInitializeWindow(u_char windowIndex, u_short x, u_short y, u_short width, u_short height, u_char shouldInitializeHandle, u8 arg6, int zIndex, u_char hasScrollBar) {
    MenuWindowParameters* pWindowParams;

    if (windowIndex >= 2) {
        g_Menu->windows[windowIndex] = HeapAlloc(sizeof(MenuWindow), 0);
        bzero(g_Menu->windows[windowIndex], sizeof(MenuWindow));
        g_Menu->windowParameters[windowIndex] = HeapAlloc(sizeof(MenuWindowParameters), 0);
        bzero(g_Menu->windowParameters[windowIndex] , sizeof(MenuWindowParameters));
        MemberChangeMenuInitializeWindowGraphics(windowIndex);
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
    
    MemberChangeMenuSetWindow(windowIndex, x, y, width, height, arg6, zIndex, hasScrollBar);
}

void MemberChangeMenuUpdateWindows(void) {
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
            
            MemberChangeMenuSetWindow(
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

void MemberChangeMenuDrawCursors(void) {
    int i;

    if (g_Menu->pManager->shouldRenderPointerCursors) {
        for (i = 0; i < MENU_MAX_NUM_CURSORS; i++) {
            if (g_Menu->pCursors->shouldRender[i]) {
                if (g_Menu->pCursors->unk144[i]) {
                    setXY4(
                        &g_Menu->pCursors->polysCursor[i * 2 + g_Menu->pCursors->renderContexts[i]],
                        D_801CB47C[D_801CB404[g_Menu->unk32C->unk4F7C]] + 8,
                        D_801CB4FC[D_801CB404[g_Menu->unk32C->unk4F7C]] - 6,
                        D_801CB47C[D_801CB404[g_Menu->unk32C->unk4F7C]] + 24,
                        D_801CB4FC[D_801CB404[g_Menu->unk32C->unk4F7C]] - 6,
                        D_801CB47C[D_801CB404[g_Menu->unk32C->unk4F7C]] + 8,
                        D_801CB4FC[D_801CB404[g_Menu->unk32C->unk4F7C]] + 10,
                        D_801CB47C[D_801CB404[g_Menu->unk32C->unk4F7C]] + 24,
                        D_801CB4FC[D_801CB404[g_Menu->unk32C->unk4F7C]] + 10  
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

void func_801C7DA8(void) {
    int i;

    for (i = 0; i < 4; i++) {
        if (g_Menu->pManager->unk34[i]) {
            AddPrim(&g_Menu->pGfxEnv->ot[4],  &g_Menu->unk4E0[i].polys[g_Menu->unk4E0[i].renderContext]);
        }
    }
}

void func_801C7E38(void) {
    int i;

    for (i = 0; i < 8; i++) {
        if (g_Menu->pManager->unkC[i]) {
            AddPrim(&g_Menu->pGfxEnv->ot[4], &g_Menu->unk6E0[i].polys[g_Menu->unk6E0[i].renderContext]);
        }
    }
}

void func_801C7EC8(void) {
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

// Draw the black dimming effect
void MemberChangeMenuRenderBackgroundDim(void) {
    AddPrim(&g_Menu->pGfxEnv->ot[8], &g_Menu->unk348->polysDimEffect[g_Menu->renderContext]);
    AddPrim(&g_Menu->pGfxEnv->ot[8], &g_Menu->unk348->drawModeDimEffect[g_Menu->renderContext]);
}

void MemberChangeMenuRenderCharacter(MenuCharacter* pCharacter, u_char isBenched) {
    int i;

    if (pCharacter->unkBE7) {
        AddPrim(&g_Menu->pGfxEnv->ot[4], &pCharacter->polysPortraitSmall[pCharacter->renderContext]);
        AddPrim(&g_Menu->pGfxEnv->ot[4], &pCharacter->polys4B0[pCharacter->renderContext]);
        
        for (i = 0; i < pCharacter->descriptionStringsLength; i++) {
            AddPrim(
                &g_Menu->pGfxEnv->ot[4], 
                &pCharacter->polysDescriptionStrings[i * 2 + pCharacter->renderContext]
            );
        }

        for (i = 0; i < pCharacter->levelStringLength; i++) {
            AddPrim(
                &g_Menu->pGfxEnv->ot[4], 
                &pCharacter->polysLevelString[i * 2 + pCharacter->renderContext]
            );            
        }

        for (i = 0; i < pCharacter->hpStringLength; i++) {
            AddPrim(
                &g_Menu->pGfxEnv->ot[4], 
                &pCharacter->polysHpString[i * 2 + pCharacter->renderContext]
            );
        }

        for (i = 0; i < pCharacter->maxHpStringLength; i++) {
            AddPrim(
                &g_Menu->pGfxEnv->ot[4], 
                &pCharacter->polysMaxHpString[i * 2 + pCharacter->renderContext]
            );
        }

        for (i = 0; i < pCharacter->mpStringLength; i++) {
            AddPrim(
                &g_Menu->pGfxEnv->ot[4], 
                &pCharacter->polysMpString[i * 2 + pCharacter->renderContext]
            );
        }

        for (i = 0; i < pCharacter->maxMpStringLength; i++) {
            AddPrim(
                &g_Menu->pGfxEnv->ot[4], 
                &pCharacter->polysMaxMpString[i * 2 + pCharacter->renderContext]
            );
        }
        
        if (isBenched) {
            for (i = 0; i < pCharacter->unkBE9; i++) {
                AddPrim(
                    &g_Menu->pGfxEnv->ot[4], 
                    &pCharacter->polys2D0[i * 2 + pCharacter->renderContext]
                );
            }
        }
    }
}

void MemberChangeMenuRenderCharacters(void) {
    int i;

    if (g_Menu->pManager->unk46) {
        for (i = 0; i < MAX_BENCHED_PARTY_MEMBERS; i++) {
            MemberChangeMenuRenderCharacter(g_Menu->benchedCharacters[i], FALSE);
        }
        
        for (i = 0; i < MAX_PARTY_MEMBERS; i++) {
            MemberChangeMenuRenderCharacter(g_Menu->currentCharacters[i], TRUE);
        }
    }
}

void func_801C846C(void) {
    func_801C7DA8();
    func_801C7E38();
    func_801C7EC8();
}

void MemberChangeMenuRenderTopWindowBorder(int index) {
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

void MemberChangeMenuRenderBottomWindowBorder(int index) {
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

void MemberChangeMenuRenderLeftWindowBorder(int index) {
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

void MemberChangeMenuRenderRightWindowBorder(int index) {
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

void MemberChangeMenuRenderWindowBackground(int index) {
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

// Project and queue window border corners for drawing
void MemberChangeMenuRenderWindowBorderCorners(int index) {
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

// Project and queue scroll bar (without the scroll bar handle) for drawing
void MemberChangeMenuRenderScrollBar(int index) {
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

void MemberChangeMenuRenderWindows(void) {
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

                MemberChangeMenuRenderWindowBorderCorners(i);
                if (pWindow->hasScrollBar) { 
                    MemberChangeMenuRenderScrollBar(i); 
                }
                MemberChangeMenuRenderTopWindowBorder(i);
                MemberChangeMenuRenderBottomWindowBorder(i);
                MemberChangeMenuRenderLeftWindowBorder(i);
                MemberChangeMenuRenderRightWindowBorder(i);
                MemberChangeMenuRenderWindowBackground(i);

                PopMatrix();
            } else {
                MemberChangeMenuRenderWindowBorderCorners(i);
                if (pWindow->hasScrollBar) { 
                    MemberChangeMenuRenderScrollBar(i); 
                }
                MemberChangeMenuRenderTopWindowBorder(i);
                MemberChangeMenuRenderBottomWindowBorder(i);
                MemberChangeMenuRenderLeftWindowBorder(i);
                MemberChangeMenuRenderRightWindowBorder(i);
                MemberChangeMenuRenderWindowBackground(i);
            }
        }
    }
}

void MemberChangeMenuRender(void) {
    if (g_Menu->shouldDrawMenu) {
        MemberChangeMenuUpdateWindows();
        MemberChangeMenuDrawCursors();
        func_801C846C();
        MemberChangeMenuRenderCharacters();
        MemberChangeMenuRenderWindows();
    }
    MemberChangeMenuRenderBackgroundDim();
}

void MemberChangeMenuPlaySoundEffect(u_char soundEffectId) {
    func_80039DB8((g_Menu->unk2E4->sedId << 16) | soundEffectId);
}

void MemberChangeMenuPollInput(void) {
    u8 wasControllerUnplugged;
    u8 isLooping;
    s32 savedValue;
    u8 input;

    input = 8;
    isLooping = 1;
    wasControllerUnplugged = 0;
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
                input = 0;
                break;
            }
            if (g_C1ButtonStatePressedOnce & CTRL_BTN_DOWN) {
                input = 1;
                break;
            }
            if (g_C1ButtonStatePressedOnce & CTRL_BTN_LEFT) {
                input = 2;
                break;
            }
            if (g_C1ButtonStatePressedOnce & CTRL_BTN_UP) {
                input = 3;
                break;
            }
            if (g_C1ButtonStateReleased & CTRL_BTN_CIRCLE) {
                input = 4;
                break;
            }
            if (g_C1ButtonStateReleased & CTRL_BTN_CROSS) {
                input = 5;
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

void MemberChangeMenuUpdateAndRender(void) {
    GfxEnvironment* pNextGfxEnv;
    int renderContext;

    MemberChangeMenuPollInput();
    GameCheckAndHandleSoftReset();

    // Change current graphics buffer
    pNextGfxEnv = &g_Menu->gfxEnvs[0];
    if (g_Menu->pGfxEnv == &g_Menu->gfxEnvs[0]) {
        pNextGfxEnv = &g_Menu->gfxEnvs[1];
    }
    g_Menu->pGfxEnv = pNextGfxEnv;
    g_Menu->renderContext = g_Menu->renderContext == 0;
    
    ClearOTagR(g_Menu->pGfxEnv->ot, 0x10);
    MemberChangeMenuRender();
    renderContext = g_Menu->renderContext == 0;
    DrawSync(0);
    Vsync(0);
    PutDrawEnv(&g_Menu->pGfxEnv->drawEnv);
    PutDispEnv(&g_Menu->pGfxEnv->dispEnv);
    MoveImage(&g_Menu->pSelectionMenu->unk1180, 0, renderContext * 0xE0);
    DrawOTag(&g_Menu->pGfxEnv->ot[0xF]);
}

/* Pre-render a character's two name strings (from g_GameState +
 * (charByte>>1)*0x28, +0 and +0x14) into a temp buffer via SystemRenderString-
 * Entry, then LoadImage the result to the character's VRAM texture slot (u/v
 * from g_MemberChangeMenuCharTexcoords[slot], u offset by 0x180; 0x28 x 0xD). */
void func_801C95A0(s32 charByte, s32 slot) {
    u8* buf = (u8*)HeapAlloc(0x3F6, 0);
    s32 off;
    s32 s2;
    RECT rect;

    bzero(buf, 0x3F6);
    off = (((u32)charByte & 0xFF) >> 1) * 0x28;
    SystemRenderStringEntry((u8*)&g_GameState + off, buf, 0x24, 0);
    SystemRenderStringEntry((u8*)&g_GameState + off + 0x14, buf, 0x24, 1);
    s2 = ((u32)slot << 1) & 0x1FC;
    /* CharTexcoords are .short data; read as u16 at the byte offset s2
     * (the migrated int[] is byte-faithful -- see data_member_change_menu.c). */
    rect.x = *(u16*)((u8*)g_MemberChangeMenuCharTexcoordsU + s2) + 0x180;
    rect.y = *(u16*)((u8*)g_MemberChangeMenuCharTexcoordsV + s2);
    rect.w = 0x28;
    rect.h = 0xD;
    LoadImage(&rect, (u_long*)buf);
    DrawSync(0);
    HeapFree(buf);
}

void MemberChangeMenuParseNumberToString(unsigned int number) {
    int i;
    unsigned int curValue;

    // 10 ** 8
    curValue = 100000000;
    
    for (i = 0; i < 9; i++) {
        g_Menu->digits[i] = number / curValue;
        number %= curValue;
        curValue /= 10;
    }
    
    for (i = 1; i < 9; i++) {
        if (g_Menu->digits[i]) {
            if (g_Menu->digits[i - 1] == 0) {
                g_Menu->digits[i - 1] = 0xFF;
            }
            break;
        }
        g_Menu->digits[i - 1] = 0xFF;
    }
}

void MemberChangeMenuFree(void) {
    int i;
    int numCharactersInParty;

    // Set party in the global game state to the ones we have selected
    for (i = 0, numCharactersInParty = 0; i < MAX_PARTY_MEMBERS; i++) {
        if (g_Menu->pManager->currentCharacterIDs[i] != 0xFF) {
            g_GameState.partyMembers[numCharactersInParty] = g_Menu->pManager->currentCharacterIDs[i];
            numCharactersInParty++;
        }
    }

    for (; numCharactersInParty < MAX_PARTY_MEMBERS; numCharactersInParty++) {
        g_GameState.partyMembers[numCharactersInParty] = 0xFF;
    }
    
    MemberChangeMenuUpdateAndRender();
    MemberChangeMenuUpdateAndRender();
    g_Menu->shouldDrawMenu = FALSE;
    
    MemberChangeMenuUpdateAndRender();
    do {
        MemberChangeMenuUpdateAndRender();
    } while (g_Menu->renderContext != 0);
    
    func_801C5034(MENU_DATA_FREE);
    MemberChangeMenuSetManager(MENU_DATA_FREE);
    MemberChangeMenuSetSelectionMenu(MENU_DATA_FREE);
    func_801C5160(MENU_DATA_FREE);
    func_801C51C4(MENU_DATA_FREE);
    func_801C5228(MENU_DATA_FREE);
    
    HeapFree(g_Menu->unk2DC);
    HeapFree(g_Menu->unk2E0);
    HeapFree(g_Menu->unk4E0[0].pVramBuffer);
    if (g_MenuDebugEnabled != 0) {
        func_8003A094(g_Menu->unk2E4);
        func_8003852C(g_Menu->unk2E4);
        HeapFree(g_Menu->unk2E4);
    }
    MemberChangeMenuSetCharacters(MENU_DATA_FREE);
    HeapFree(g_Menu);
}

void func_801C9908(void) {
    int numAvailableCharacters;
    unsigned char characterNotInParty;
    int i, j;

    for (i = 0, numAvailableCharacters = 0; i < MAX_GAME_CHARACTERS; i++) {
        characterNotInParty = TRUE;

        // Is this character available?
        if (g_Menu->availableCharacters[i]) {
            for (j = 0; j < MAX_PARTY_MEMBERS; j++) {
                if (g_Menu->pManager->currentCharacterIDs[j] == i) {
                    characterNotInParty = FALSE;
                    break;
                }
            }
            
            if (characterNotInParty) {
                g_Menu->unk1E14[numAvailableCharacters] = i;
                numAvailableCharacters++;
            }
        }
    }

    for (; numAvailableCharacters < MAX_GAME_CHARACTERS; numAvailableCharacters++) {
        g_Menu->unk1E14[numAvailableCharacters] = 0xFF;
    }
    
    for (i = 0; i < MAX_GAME_CHARACTERS; i += 2) {
        numAvailableCharacters = i & 0xFF;
        func_801C95A0(numAvailableCharacters, numAvailableCharacters);
    }
}

void func_801C9A08(MenuCharacter* pCharacter, u_char characterIndex, u_char localIndex, int* pPositionsX, int* pPositionsY, int verticalSpacing) {
    int i;
    u_char uvIndex;
    int height;
    
    // LV, HP/, MP/ strings
    pCharacter->descriptionStringsLength = 0;
    for (i = 0; i < 9; i++) {
        if (D_801CB3DC[i] != 0xFFFF) {
            pCharacter->descriptionStringsLength += func_8002675C(
                g_Menu->unk2DC, D_801CB3DC[i], 
                &pCharacter->polysDescriptionStrings[pCharacter->descriptionStringsLength * 2], 
                g_Menu->renderContext, 
                pPositionsX[i], 
                (verticalSpacing * localIndex) + pPositionsY[i], 
                0x1000
            );
        }
    }

    // Small character portraits
    func_8002675C(
        g_Menu->unk2DC, characterIndex + 0x14E, 
        &pCharacter->polysPortraitSmall, g_Menu->renderContext,
        pPositionsX[9], 
        (verticalSpacing * localIndex) + pPositionsY[9], 
        0x1000
    );
    
    SetPolyFT4(&pCharacter->polys4B0[g_Menu->renderContext]);
    setRGB0(&pCharacter->polys4B0[g_Menu->renderContext], 128, 128, 128);
    SetSemiTrans(&pCharacter->polys4B0[g_Menu->renderContext], 0);
    pCharacter->polys4B0[g_Menu->renderContext].tpage = GetTPage(0, 0, 384, 0);
    pCharacter->polys4B0[g_Menu->renderContext].clut = characterIndex % 2 ? g_SystemPalette2 : g_SystemPalette1;
    
    height = verticalSpacing * localIndex;
    setXY4(
        &pCharacter->polys4B0[g_Menu->renderContext],
        pPositionsX[16],        pPositionsY[16] + height,
        pPositionsX[16] + 72,   pPositionsY[16] + height,
        pPositionsX[16],        pPositionsY[16] + height + 13,
        pPositionsX[16] + 72,   pPositionsY[16] + height + 13
    );

    uvIndex = characterIndex / 2;
    setUV4(
        &pCharacter->polys4B0[g_Menu->renderContext],
        g_MemberChangeMenuCharTexcoordsU[uvIndex] * 4,
        g_MemberChangeMenuCharTexcoordsV[uvIndex],
        (g_MemberChangeMenuCharTexcoordsU[uvIndex] * 4) + 72,
        g_MemberChangeMenuCharTexcoordsV[uvIndex],
        g_MemberChangeMenuCharTexcoordsU[uvIndex] * 4,
        g_MemberChangeMenuCharTexcoordsV[uvIndex] + 13,
        (g_MemberChangeMenuCharTexcoordsU[uvIndex] * 4) + 72,
        g_MemberChangeMenuCharTexcoordsV[uvIndex] + 13
    );
}

void MemberChangeMenuSetCharacterLevelStrings(MenuCharacter* pCharacter, u_char characterIndex, u_char localIndex, int* pPositionsX, int* pPositionsY, int verticalSpacing) {
    int i;
    int j;
    u_char digit;
    
    MemberChangeMenuParseNumberToString(g_GameState.characters[characterIndex].level);
    pCharacter->levelStringLength = 0;
    for (i = 0; i < 3; i++) {
        digit = g_Menu->digits[i + 6];
        if (digit != 0xFF) {
            pCharacter->levelStringLength += func_8002675C(
                g_Menu->unk2DC, digit, 
                &pCharacter->polysLevelString[pCharacter->levelStringLength * 2], g_Menu->renderContext, 
                pPositionsX[10] + i * 8,
                pPositionsY[10] + verticalSpacing * localIndex,
                0x1000
            );
        }
    }
    
    MemberChangeMenuParseNumberToString(g_GameState.characters[characterIndex].unk63);
    pCharacter->unkBE1 = 0;
    for (i = 0, j = 0; i < 3; i++) {
        digit = g_Menu->digits[i + 6];
        if (digit != 0xFF) {
            pCharacter->unkBE1 += func_8002675C(
                g_Menu->unk2DC, digit,
                &pCharacter->polys5F0[pCharacter->unkBE1 * 2], g_Menu->renderContext, 
                pPositionsX[11] + j * 8,
                pPositionsY[11] + verticalSpacing * localIndex,
                0x1000
            );
            j++;
        }
    }
    
    for (i = 0; i < pCharacter->unkBE1; i++) {
        SetShadeTex(&pCharacter->polys5F0[i * 2 + g_Menu->renderContext], 0);
        setRGB0(&pCharacter->polys5F0[i * 2 + g_Menu->renderContext], 0, 128, 0);
    }
}

void MemberChangeMenuSetCharacterHpAndMpStrings(MenuCharacter* pCharacter, u_char characterIndex, u_char localIndex, int* pPositionsX, int* pPositionsY, int verticalSpacing) {
    int i;
    int j;
    u_char digit;

    MemberChangeMenuParseNumberToString(g_GameState.characters[characterIndex].hp);
    pCharacter->hpStringLength = 0;
    for (i = 0; i < 3; i++) {
        digit = g_Menu->digits[i + 6];
        if (digit != 0xFF) {
            pCharacter->hpStringLength += func_8002675C(
                g_Menu->unk2DC, digit, 
                &pCharacter->polysHpString[pCharacter->hpStringLength * 2], 
                g_Menu->renderContext, 
                (i * 8) + pPositionsX[12], 
                (verticalSpacing * localIndex) + pPositionsY[12], 
                0x1000
            );
        }
    }
    
    MemberChangeMenuParseNumberToString(g_GameState.characters[characterIndex].maxHp);
    pCharacter->maxHpStringLength = 0;
    for (i = 0, j = 0; i < 3; i++) {
        digit = g_Menu->digits[i + 6];
        if (digit != 0xFF) {
            pCharacter->maxHpStringLength += func_8002675C(
                g_Menu->unk2DC, digit, 
                &pCharacter->polysMaxHpString[pCharacter->maxHpStringLength * 2], 
                g_Menu->renderContext, 
                (j * 8) + pPositionsX[13], 
                (verticalSpacing * localIndex) + pPositionsY[13], 
                0x1000
            );
            j++;
        }
    }
    
    MemberChangeMenuParseNumberToString(g_GameState.characters[characterIndex].mp);
    pCharacter->mpStringLength = 0;
    for (i = 0; i < 2; i++) {
        digit = g_Menu->digits[i + 7];
        if (digit != 0xFF) {
            pCharacter->mpStringLength += func_8002675C(
                g_Menu->unk2DC, digit, 
                &pCharacter->polysMpString[pCharacter->mpStringLength * 2], 
                g_Menu->renderContext, 
                (i * 8) + pPositionsX[14], 
                (verticalSpacing * localIndex) + pPositionsY[14], 
                0x1000
            );
        }
    }
    
    MemberChangeMenuParseNumberToString(g_GameState.characters[characterIndex].maxMp);
    pCharacter->maxMpStringLength = 0;
    for (i = 0, j = 0; i < 2; i++) {
        digit = g_Menu->digits[i + 7];
        if (digit != 0xFF) {
            pCharacter->maxMpStringLength += func_8002675C(
                g_Menu->unk2DC, digit, 
                &pCharacter->polysMaxMpString[pCharacter->maxMpStringLength * 2], 
                g_Menu->renderContext, 
                (j * 8) + pPositionsX[15], 
                (verticalSpacing * localIndex) + pPositionsY[15], 
                0x1000
            );
            j++;
        }
    }
}

// localIndex here refers to the order / position of the character 
// in either the current party or the benched members list
void MemberChangeMenuUpdateCharacter(MenuCharacter* pCharacter, u_char characterIndex, u_char localIndex, int* pPositionsX, int* pPositionsY, int verticalSpacing, u_char isPartyCharacter) {
    func_801C9A08(pCharacter, characterIndex, localIndex, pPositionsX, pPositionsY, verticalSpacing);
    MemberChangeMenuSetCharacterLevelStrings(pCharacter, characterIndex, localIndex, pPositionsX, pPositionsY, verticalSpacing);
    MemberChangeMenuSetCharacterHpAndMpStrings(pCharacter, characterIndex, localIndex, pPositionsX, pPositionsY, verticalSpacing);
    pCharacter->unkBE9 = 0;
    pCharacter->unkBE7 = 1;
    pCharacter->renderContext = g_Menu->renderContext;
}

void MemberChangeMenuUpdateCharacters(u_char offset) {
    MenuCharacter* pCharacter;
    s32 characterIndex;
    int i;
    s32* pPositionsX;
    s32* pPositionsY;

    // Active Characters
    pPositionsX = g_MemberChangeMenuCurCharPositionsX;
    pPositionsY = g_MemberChangeMenuCurCharPositionsY;
    for (i = 0; i < MAX_PARTY_MEMBERS; i++) {
        pCharacter = g_Menu->currentCharacters[i];
        if (g_Menu->pManager->currentCharacterIDs[i] != 0xFF) {
            MemberChangeMenuUpdateCharacter(
                pCharacter, 
                g_Menu->pManager->currentCharacterIDs[i], i, 
                pPositionsX, pPositionsY, 
                56,
                1
            );
        } else {
            pCharacter->unkBE7 = 0;
        }
    }
    
    g_Menu->pManager->unk46 = 1;

    // Benched Characters
    pPositionsX = g_MemberChangeMenuBenchedCharPositionsX;
    pPositionsY = g_MemberChangeMenuBenchedCharPositionsY;
    for (i = 0; i < MAX_BENCHED_PARTY_MEMBERS; i++) {
        characterIndex = offset + i;
        if (characterIndex >= MAX_GAME_CHARACTERS) {
            return;
        }

        pCharacter = g_Menu->benchedCharacters[i];
        if (g_Menu->unk1E14[characterIndex] != 0xFF) {
            MemberChangeMenuUpdateAndRender();
            MemberChangeMenuUpdateCharacter(
                pCharacter, 
                g_Menu->unk1E14[characterIndex], i, 
                pPositionsX, pPositionsY, 
                32,
                0
            );
        } else {
            pCharacter->unkBE7 = 0;
        }
    }
}

void MemberChangeMenuSetCursorToCharacter(u_char isBenchedCharWindowActive, int characterIndex, u_char cursorIndex) {
    int offset;

    // If the benched character window is active, we add and offset of 3
    offset = -(isBenchedCharWindowActive != 0) & 3;
    
    if (cursorIndex == 0) {
        func_8002675C(
            g_Menu->unk2DC, MENU_TEX_POINTER_CURSOR, 
            &g_Menu->pCursors->polysCursor[0], g_Menu->renderContext, 
            g_MemberChangeMenuCurserPositionsX[offset + characterIndex], 
            g_MemberChangeMenuCurserPositionsY[offset + characterIndex], 
            0x800
        );
    } else {
        func_8002675C(
            g_Menu->unk2DC, MENU_TEX_POINTER_CURSOR, 
            &g_Menu->pCursors->polysCursor[2], g_Menu->renderContext, 
            g_MemberChangeMenuCurserPositionsX[offset + characterIndex], 
            g_MemberChangeMenuCurserPositionsY[offset + characterIndex], 
            0x800
        );
    }
        
    g_Menu->pCursors->shouldRender[cursorIndex] = TRUE;
    g_Menu->pCursors->renderContexts[cursorIndex] = g_Menu->renderContext;
    g_Menu->pManager->shouldRenderPointerCursors = TRUE;
}

void MemberChangeMenuInitializeCursors(unsigned char mode) {
    int i;

    g_Menu->pCursors = HeapAlloc(sizeof(MenuPointerCursors), 0);
    bzero(g_Menu->pCursors, sizeof(MenuPointerCursors));
    
    switch (mode) {
        case 0:
            g_Menu->pManager->shouldRenderPointerCursors = TRUE;
            g_Menu->pCursors->unk144[0] = 1;
            g_Menu->pCursors->unk144[1] = 1;
            /* fallthrough */
        case 2:
            for (i = 0; i < MENU_MAX_NUM_CURSORS; i++) {
                func_8002675C(
                    g_Menu->unk2DC, MENU_TEX_POINTER_CURSOR, 
                    &g_Menu->pCursors->polysCursor[i * 2], g_Menu->renderContext, 
                    D_801CB180[i], D_801CB190[i],
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

void MemberChangeMenuFreeCursors(void) {
    g_Menu->pManager->shouldRenderPointerCursors = FALSE;
    MemberChangeMenuUpdateAndRender();
    HeapFree(g_Menu->pCursors);
}

/* Retail 0x801CAB48..0x801CAD14 (460 bytes). The split empty-slot and
 * restriction tests preserve the PSYQ register allocation; the fourth argument
 * is unused in retail. Restriction flags are the game-state halfword at
 * 0x8006F94C, also written by field script func_800883D4. */
u_char MemberChangeMenuSwapCharacters(u_char benched, s32 current, s32 offset,
                                     u_char selectedBenched, s32 selected, s32 selectedOffset) {
    s32 i;
    u_char count;
    u_char temp;
    u_char currentAllowed;
    u_char selectedAllowed;
    u_char result;
    u_char partyIndex;
    u_char benchIndex;

    result = 0;
    currentAllowed = 1;
    selectedAllowed = 1;

    if (!benched) {
        partyIndex = current;
        benchIndex = selected + selectedOffset;
    } else {
        partyIndex = selected;
        benchIndex = current + offset;
    }
    if (g_Menu->pManager->currentCharacterIDs[partyIndex] == 0xFF) {
        currentAllowed = 0;
    } else if (MemberChangeMenuIsCharacterFlagSet(*(u16*)((u8*)&g_GameState + 0x2318), g_Menu->pManager->currentCharacterIDs[partyIndex])) {
        currentAllowed = 0;
    }
    if (g_Menu->unk1E14[benchIndex] == 0xFF) {
        selectedAllowed = 0;
    } else if (MemberChangeMenuIsCharacterFlagSet(*(u16*)((u8*)&g_GameState + 0x2318), g_Menu->unk1E14[benchIndex])) {
        selectedAllowed = 0;
    }
    if (currentAllowed && selectedAllowed) {
        temp = g_Menu->pManager->currentCharacterIDs[partyIndex];
        g_Menu->pManager->currentCharacterIDs[partyIndex] = g_Menu->unk1E14[benchIndex];
        g_Menu->unk1E14[benchIndex] = temp;
        i = 0;
        count = 0;
        for (; i < 3; i++) {
            if (g_Menu->pManager->currentCharacterIDs[i] != 0xFF) {
                count++;
            }
        }
        result = 1;
        if (!count) {
            temp = g_Menu->pManager->currentCharacterIDs[partyIndex];
            g_Menu->pManager->currentCharacterIDs[partyIndex] = g_Menu->unk1E14[benchIndex];
            g_Menu->unk1E14[benchIndex] = temp;
            result = 0;
        }
    }
    return result;
}

void MemberChangeMenuMainLoop(void) {
    int i;
    int curCharIndex;
    int selectedCharIndex;
    int selectedCharOffset;
    int wasBenchedCharWindowActive;
    int curCharOffset;
    int updateCharactersSignal;
    u_char benchedCharWindowActive;
    u_char isRunning;
    u_char isCharSelected;
    u_char isSelectedCharOnBenchedWindow;

    
    isRunning = TRUE;
    curCharIndex = 0;
    curCharOffset = 0;
    
    func_801C9908();
    MemberChangeMenuInitializeWindow(2, 0x78, 6, 0xB8, 0xC8, 1, 0, 4, 0);
    MemberChangeMenuInitializeCursors(1);
    
    updateCharactersSignal = 0xFF;
    benchedCharWindowActive = FALSE;
    isCharSelected = FALSE;
    
    if (g_Menu->windowParameters[2]->unk11 == 0) {
        do {
            MemberChangeMenuUpdateAndRender();
        } while (g_Menu->windowParameters[2]->unk11 == 0);
    }
      
    while (isRunning) {
        MemberChangeMenuUpdateAndRender();
        
        if (curCharOffset != updateCharactersSignal) {
            MemberChangeMenuUpdateCharacters(curCharOffset);
            updateCharactersSignal = curCharOffset;
        }

        MemberChangeMenuSetCursorToCharacter(benchedCharWindowActive, curCharIndex, 0);

        switch (g_Menu->input) {
            case MENU_INPUT_BACK:
            MemberChangeMenuPlaySoundEffect(3);

                // If there's an already selected character,
                // disable the cursor which was placed on that character
                if (isCharSelected) {
                    isCharSelected = FALSE;
                    g_Menu->pCursors->shouldRender[isRunning] = FALSE;
                    break;
                }
                
                isRunning = FALSE;
                break;
            
            case MENU_INPUT_CONFIRM:
                // We haven't selected our first character yet, so place
                // a cursor on this character
                if (!isCharSelected) {
                    isCharSelected = TRUE;
                    
                    MemberChangeMenuSetCursorToCharacter(benchedCharWindowActive, curCharIndex, 1);
                    
                    selectedCharIndex = curCharIndex;
                    curCharIndex = 0;
                    
                    wasBenchedCharWindowActive = benchedCharWindowActive;
                    isSelectedCharOnBenchedWindow = benchedCharWindowActive;
                    benchedCharWindowActive = !wasBenchedCharWindowActive;
                    
                    selectedCharOffset = curCharOffset;
              
                    MemberChangeMenuPlaySoundEffect(2);
                    break;
                }
                
                if (MemberChangeMenuSwapCharacters(
                    benchedCharWindowActive, curCharIndex, curCharOffset,
                    isSelectedCharOnBenchedWindow, selectedCharIndex, selectedCharOffset
                )) {
                    isCharSelected = FALSE;
                    updateCharactersSignal = 0xFF;
                    g_Menu->pCursors->shouldRender[isRunning] = FALSE;
                    MemberChangeMenuPlaySoundEffect(2);
                    break;
                }
                
                MemberChangeMenuPlaySoundEffect(4);
                break;
            
            case MENU_INPUT_DOWN:
                if (!benchedCharWindowActive) {
                    MemberChangeMenuPlaySoundEffect(1);
                    curCharIndex++;
                    if (curCharIndex < 3) {
                        break;
                    }
                    curCharIndex = 0;
                    break;
                }
                
                curCharIndex++;
                if (curCharIndex < 6) {
                    MemberChangeMenuPlaySoundEffect(1);
                    break;
                }
                curCharIndex = 5;
                MemberChangeMenuPlaySoundEffect(1);
                break;
            
            case MENU_INPUT_UP:
                // Are we on the party member window?
                if (!benchedCharWindowActive) {
                    MemberChangeMenuPlaySoundEffect(1);
                    curCharIndex--;
                    if (curCharIndex >= 0) {
                        break;
                    }
                    curCharIndex = 2;
                    break;
                }

                // We are on the benched member window
                curCharIndex--;
                if (curCharIndex >= 0) {
                    MemberChangeMenuPlaySoundEffect(1);
                    break;
                }  
                curCharIndex = 0;
                
                curCharOffset--;
                if (curCharOffset < 0) {
                    curCharOffset = 0;
                    break;
                }
                MemberChangeMenuPlaySoundEffect(1);
                break;

            case MENU_INPUT_LEFT:
                if (isCharSelected) break;

                // Current party window is to the left, so if we're already
                // there we can't move further left
                if (!benchedCharWindowActive) break;
                
                MemberChangeMenuPlaySoundEffect(1);
                benchedCharWindowActive = FALSE;
                curCharIndex = 0;
                break;
            
            case MENU_INPUT_RIGHT:
                if (isCharSelected) break;

                // Benched character window is to the right, so if we're already
                // there we can't move further right
                if (benchedCharWindowActive) break;
                
                MemberChangeMenuPlaySoundEffect(1);
                benchedCharWindowActive = TRUE;
                curCharIndex = 0;
                break;
        }
    }
            
    MemberChangeMenuFreeCursors();
    MemberChangeMenuFreeWindow(2);

    // Update current characters to reflect the ones selected in the menu
    for (i = 0; i < MAX_PARTY_MEMBERS; i++) {
        g_GameState.partyMembers[i] = g_Menu->pManager->currentCharacterIDs[i];
    }
}

void MemberChangeMenuMain(void) {
    func_801C5034(MENU_DATA_INITIALIZE);
    MemberChangeMenuSetManager(MENU_DATA_INITIALIZE);
    MemberChangeMenuSetSelectionMenu(MENU_DATA_INITIALIZE);
    func_801C5160(MENU_DATA_INITIALIZE);
    func_801C51C4(MENU_DATA_INITIALIZE);
    func_801C5228(MENU_DATA_INITIALIZE);
    MemberChangeMenuSetCharacters(MENU_DATA_INITIALIZE);
    g_Menu->pSelectionMenu->unk1180.x = 0x2C0;
    g_Menu->pSelectionMenu->unk1180.y = 0x100;
    g_Menu->pSelectionMenu->unk1180.w = 0x140;
    g_Menu->pSelectionMenu->unk1180.h = 0xE0;
    g_Menu->unk348->unk15B = 0x40;
    MemberChangeMenuInitialize();
    MemberChangeMenuResetRenderContext();
    func_801C5B90();
    MemberChangeMenuInitializeBackground();
    MemberChangeMenuInitializeWindowBorders();
    g_Menu->shouldDrawMenu = TRUE;
    MemberChangeMenuMainLoop();
    MemberChangeMenuFree();
}
