#include "common.h"
#include "system/menu.h"
#include "main/game.h"

/* Main-menu (menu.bin) Phase B1a: dispatcher + init allocation slice, ported.
 * NB the port's SystemMenu/MenuManager/etc are NATIVE layout (8-byte pointers
 * inflate the offsets), so these follow member_change's convention -- struct
 * FIELDS + sizeof(native type), NOT raw PSX offsets/sizes. Byte-array fields
 * that hold PSX 4-byte pointers (unk340[0]/[4], unk39C[i*4]) use 4-byte
 * truncated storage (port heap < 4GB), since native 8-byte pointers would
 * overflow the u8[] field. */
extern void func_801C5F10(void);
extern void func_801C7B0C(void);
extern void func_801C58EC(void);
extern void func_801C8694(s32 arg0);
extern void func_801D2D38(void);
extern void func_801C55A0(void);
extern void func_801C57A4(void);
extern void func_801C5FE4(void);
extern void func_801C5B54(u8 init);
extern void func_801C5BB8(u8 init);
extern void func_801C5C1C(u8 init);
extern void func_801C5C80(u8 init);
extern void func_801C5CE4(u8 init);
extern void func_801C5D48(u8 init);
extern void func_801C5DAC(u8 init);
extern void func_801C5E10(u8 init);
extern void func_801C5E74(u8 init);
/* B1b: content-build subtree.  6D4C/6D5C ported below; the POLY-setup builders
 * (6400/6AA0/6E0C/6E68/6F70) remain INCLUDE_ASM (auto-stub no-ops) until their
 * own pass -- the coordinator calls them by prototype so it links either way. */
extern void func_801C6400(void);
extern void func_801C6AA0(void);
extern void func_801C6D4C(void);
extern void func_801C6D5C(void);
extern void func_801C6E0C(void);
extern void func_801C6E68(void);
extern void func_801C6F70(void);
extern u16 func_801C865C(u16 mask, u8 index);  /* defined below */
/* B1b resource-load (func_801C65F4) callees + data.  LZSSHeapDecompress MUST
 * have a void* prototype -- the port builds with -w, so a missing decl would
 * default it to int and truncate the 64-bit host pointer. */
extern void* LZSSHeapDecompress(void* pCompressed, int flags);
extern unsigned int ResolveArchiveEntryPointers(u32* pFile);
extern void func_8002DD20(u32* pList);
extern void func_80026338(u8* table, s32 index, u32* pOut0, s32* pTPage,
                          s32* pClutX, s32* pClutY, s32* pTexX, s32* pTexY);
extern char D_801C5028[];   /* "BASLUS-00664" (migrated menu rodata) */
extern char D_801C5038[];   /* "BASLUS-01160" */
extern u8 D_801EA524[];     /* string-render descriptor (migrated) */
extern void func_801E7E68(void* dst, void* src, s32 a2, s32 a3);  /* stub (overlay) */
extern void func_801C6D90(void);                                  /* stub */
extern void* D_8006259C;    /* SEDS file pointer */
extern void* D_8005945C;    /* menu resource-pointer table (pResources) */
extern u16 D_801E96A8[];    /* migrated bit-select table {1,2,4,...} */
extern u8 D_80059460;
extern u8 D_80059171;
extern u8 D_800594CC;
extern u8 D_800594D0;

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C531C);

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C55A0);
#else
extern void func_801C8574(s32);
extern void func_801D22C4(void);
extern void func_801E8044(s32, void*);
extern s32 func_801C531C(s32);
extern void func_801E8978(s32, s32, void*);
extern s32 func_801E8070(s32, void*, void*, void*, void*, s32, s32, s32);
extern u8 D_801EA19C[];
extern u8 D_801EA528[];
extern u8 D_801E9E64[];

/* Arc A: the top-level menu input/render loop.  Draws each frame via
 * func_801C7BF4, reads the nav input, updates menu1Choice (wrap 0..6), and
 * loops until cancel (s1 == 0).  The nav/confirm/choice-change branches call
 * the object-overlay stubs (func_801C8574/D22C4/E8044/C531C/E8978/E8070 --
 * no-op in the port, never reached in the headless render harness with no
 * input, so the loop just draws each frame). */
void func_801C55A0(void) {
    s32 s1 = 1;

    while (1) {
        u8 input;

        func_801C7BF4();
        input = g_Menu->input;

        if (input == 3) {
            g_Menu->menu1Choice++;
            if ((u8)g_Menu->menu1Choice >= 7) {
                g_Menu->menu1Choice = 0;
            }
        } else if (input < 4) {
            if (input == 1) {
                if (g_Menu->menu1Choice == 0) {
                    g_Menu->menu1Choice = 6;
                } else {
                    g_Menu->menu1Choice--;
                }
            }
        } else if (input == 4) {
            u8 ok = 1;
            if (g_Menu->menu1Choice == 2 && g_Menu->unk33B == 0) {
                ok = 0;
                func_801C8574(4);
            }
            if (ok) {
                g_Menu->pSelectionMenu->unk1192 = 1;
                func_801D22C4();
                func_801E8044(8, (u8*)g_Menu->pManager + 0xC);
                s1 = func_801C531C(0);
            }
        } else if (input == 5) {
            s1 = 0;
        }

        if (g_Menu->menu1Choice != g_Menu->unk337) {
            func_801E8978(7, 0, D_801EA19C);
            func_801E8070(8, (u8*)g_Menu + 0x6E0, D_801EA528, D_801E9E64,
                          (u8*)g_Menu->pManager + 0xC, g_Menu->menu1Choice, 0, 0);
            g_Menu->unk337 = g_Menu->menu1Choice;
        }

        if (s1 == 0) {
            break;
        }
    }
}
#endif

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C57A4);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C58EC);

void func_801C5B54(u8 init) {
    if (init) {
        g_Menu->unk32C = HeapAlloc(sizeof(MenuUnk2), 0);
        bzero(g_Menu->unk32C, sizeof(MenuUnk2));
        return;
    }
    HeapFree(g_Menu->unk32C);
}

void func_801C5BB8(u8 init) {
    if (init) {
        g_Menu->pManager = HeapAlloc(sizeof(MenuManager), 0);
        bzero(g_Menu->pManager, sizeof(MenuManager));
        return;
    }
    HeapFree(g_Menu->pManager);
}

void func_801C5C1C(u8 init) {
    if (init) {
        g_Menu->pSelectionMenu = HeapAlloc(sizeof(MenuSelectionMenu), 0);
        bzero(g_Menu->pSelectionMenu, sizeof(MenuSelectionMenu));
        return;
    }
    HeapFree(g_Menu->pSelectionMenu);
}

void func_801C5C80(u8 init) {
    if (init) {
        g_Menu->unk354 = HeapAlloc(sizeof(MenuUnk5), 0);
        bzero(g_Menu->unk354, sizeof(MenuUnk5));
        return;
    }
    HeapFree(g_Menu->unk354);
}

void func_801C5CE4(u8 init) {
    if (init) {
        g_Menu->unk330 = HeapAlloc(sizeof(MenuUnk6), 0);
        bzero(g_Menu->unk330, sizeof(MenuUnk6));
        return;
    }
    HeapFree(g_Menu->unk330);
}

void func_801C5D48(u8 init) {
    if (init) {
        void* p = HeapAlloc(0x328, 0);
        *(u32*)&g_Menu->unk340[0] = (u32)(uintptr_t)p;
        bzero(p, 0x328);
        return;
    }
    HeapFree((void*)(uintptr_t)*(u32*)&g_Menu->unk340[0]);
}

void func_801C5DAC(u8 init) {
    if (init) {
        void* p = HeapAlloc(0x374, 0);
        *(u32*)&g_Menu->unk340[4] = (u32)(uintptr_t)p;
        bzero(p, 0x374);
        return;
    }
    HeapFree((void*)(uintptr_t)*(u32*)&g_Menu->unk340[4]);
}

void func_801C5E10(u8 init) {
    if (init) {
        g_Menu->unk348 = HeapAlloc(sizeof(MenuUnk1), 0);
        bzero(g_Menu->unk348, sizeof(MenuUnk1));
        return;
    }
    HeapFree(g_Menu->unk348);
}

void func_801C5E74(u8 init) {
    int i;
    if (init) {
        for (i = 0; i < 3; i++) {
            void* p = HeapAlloc(0x127C, 0);
            *(u32*)&g_Menu->unk39C[i * 4] = (u32)(uintptr_t)p;
            bzero(p, 0x127C);
        }
        return;
    }
    for (i = 0; i < 3; i++) {
        HeapFree((void*)(uintptr_t)*(u32*)&g_Menu->unk39C[i * 4]);
    }
}

/* These earlier menu ports are functional native C but are not yet MIPS
 * matches.  Keep retail assembly in the matching build so their accumulated
 * size drift cannot move the field-visible object region at 0x801E71B4. */
#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C5F10);
#else
void func_801C5F10(void) {
    u8 v1;
    func_801C5BB8(1);
    func_801C5C1C(1);
    func_801C5C80(1);
    func_801C5CE4(1);
    func_801C5E10(1);
    g_Menu->pCursors = HeapAlloc(sizeof(MenuPointerCursors), 0);
    bzero(g_Menu->pCursors, sizeof(MenuPointerCursors));
    v1 = D_80059460;
    if (v1 == 2) {
        func_801C5B54(1);
    } else if (v1 < 3) {
        if (v1 == 0) {
            func_801C5B54(1);
            func_801C5D48(1);
            func_801C5DAC(1);
            func_801C5E74(1);
        }
    } else if (v1 == 6) {
        func_801C5B54(1);
    }
}
#endif

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C5FE4);

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C62A8);
#else
void func_801C62A8(void) {
    u8 s0;
    func_801C5F10();
    func_801C7B0C();
    g_Menu->shouldDrawMenu = 1;
    g_Menu->unk32A = 1;
    s0 = D_80059460;
    if (s0 == 2) {
        u8 v0;
        D_800594D0 = 0;
        func_801C58EC();
        g_Menu->pManager->shouldRenderSelectionMenu = 0;
        ((u8*)g_Menu->pManager)[0x4] = 0;
        ((u8*)g_Menu->pManager)[0x3] = 0;
        v0 = D_800594D0;
        if (v0 == 0) {
            func_801C8694(0);
        } else if (v0 == s0) {
            func_801C8694(((u8*)&g_GameState)[0x19D4]);
        }
    } else if (s0 < 3) {
        if (s0 == 0) {
            func_801D2D38();
            func_801C55A0();
        }
    } else if (s0 == 6) {
        func_801C57A4();
    }
    func_801C5FE4();
}
#endif

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C6400);

/* B1b: the main-menu resource-load (twin of MemberChangeMenuLoadResources).
 * Decompresses the menu resources -- the Xenogears icon TIM, the menu TIM
 * textures (func_8002DD20), the texture-UV atlas -> unk2DC (what the window/text
 * builders read), a second atlas -> unk2E0, and the 3 party-portrait TIMs
 * (uploaded to VRAM via LoadImage at positions from func_80026338).  Also stores
 * the two memory-card save-file names.  Called by func_801C6AA0. */
#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C65F4);
#else
void func_801C65F4(void) {
    u32* pResources = (u32*)D_8005945C;
    void* pTim;
    void* pCharTims;
    TIM_IMAGE charTim;
    /* func_80026338 output slots for the 3 portrait positions (stride 0x18);
     * only clutX/clutY/texX/texY are consumed (the VRAM CLUT/texture coords). */
    struct { u32 uv; s32 tpage, clutX, clutY, texX, texY; } pos[3];
    s32 i;

    ResolveArchiveEntryPointers(pResources);

    /* Xenogears icon TIM (pResources[1]). */
    pTim = LZSSHeapDecompress((void*)pResources[1], 1);
    OpenTIM(pTim);
    ReadTIM(&g_Menu->unk32C->tim);

    /* Two memory-card save-file names (BASLUS-00664 @ 0x4FCE, -01160 @ 0x501C). */
    memcpy(&g_Menu->unk32C->unk4F80[0x4E], D_801C5028, 13);
    memcpy(&g_Menu->unk32C->unk4F80[0x9C], D_801C5038, 13);

    g_Menu->unk32C->unk4B94 = 0x53;
    g_Menu->unk32C->unk4B95 = 0x43;
    g_Menu->unk32C->unk4B96 = 0x11;
    g_Menu->unk32C->unk4B97 = 1;
    bzero(g_Menu->unk32C->unk4B98, 0x5C);
    memmove(g_Menu->unk32C->unk4BF4, g_Menu->unk32C->tim.caddr, 0x20);
    memmove(g_Menu->unk32C->unk4C14, g_Menu->unk32C->tim.paddr, 0x80);
    HeapFree(pTim);

    /* Menu TIM textures (pResources[2]). */
    pTim = LZSSHeapDecompress((void*)pResources[2], 1);
    func_8002DD20(pTim);
    HeapFree(pTim);

    /* Texture-UV atlases: unk2DC is read by the window/text builders. */
    g_Menu->unk2DC = LZSSHeapDecompress((void*)pResources[3], 0);
    g_Menu->unk2E0 = LZSSHeapDecompress((void*)pResources[4], 0);

    /* VRAM CLUT/texture positions for the 3 portrait slots.  (The 0xE0 call is
     * immediately overwritten by the 0x14B call in retail -- kept for fidelity.) */
    func_80026338(g_Menu->unk2DC, 0xE0,  &pos[0].uv, &pos[0].tpage, &pos[0].clutX, &pos[0].clutY, &pos[0].texX, &pos[0].texY);
    func_80026338(g_Menu->unk2DC, 0x14B, &pos[0].uv, &pos[0].tpage, &pos[0].clutX, &pos[0].clutY, &pos[0].texX, &pos[0].texY);
    func_80026338(g_Menu->unk2DC, 0x14C, &pos[1].uv, &pos[1].tpage, &pos[1].clutX, &pos[1].clutY, &pos[1].texX, &pos[1].texY);
    func_80026338(g_Menu->unk2DC, 0x14D, &pos[2].uv, &pos[2].tpage, &pos[2].clutX, &pos[2].clutY, &pos[2].texX, &pos[2].texY);
    pos[1].texX += 0xC;

    /* Party portraits (pResources[5]): upload each character's TIM to VRAM. */
    pCharTims = LZSSHeapDecompress((void*)pResources[5], 1);
    for (i = 0; i < 3; i++) {
        u8 charId = g_Menu->pManager->currentCharacterIDs[i];
        if (charId == 0xFF) {
            continue;
        }
        OpenTIM((u_long*)((u8*)pCharTims + charId * 0xB20));
        ReadTIM(&charTim);
        charTim.crect->x = (s16)pos[i].clutX;
        charTim.crect->y = (s16)pos[i].clutY;
        charTim.prect->x = (s16)pos[i].texX;
        charTim.prect->y = (s16)pos[i].texY;
        LoadImage(charTim.crect, charTim.caddr);
        LoadImage(charTim.prect, charTim.paddr);
    }
    DrawSync(0);
    HeapFree(pCharTims);

    if (g_MenuDebugEnabled) {
        ArchiveSetIndex(0x10, 2);
        D_8006259C = HeapAlloc(ArchiveDecodeAlignedSize(5), 0);
        ArchiveReadFileToBuffer(5, D_8006259C, 0, 0x80);
        ArchiveCdDataSync(0);
        ArchiveSetIndex(0x10, 0);
        SoundAddSedsEntry(D_8006259C);
    }
    g_Menu->unk2E4 = (SoundFile*)D_8006259C;
    HeapFree(pResources);
}
#endif

/* B1b: party/character setup for the main menu, then the resource-load.
 * Computes availableCharacters[] from the party flag mask, resolves the 3
 * active party slots (currentCharacterIDs / gear flags), records the first
 * active slot, then calls func_801C65F4 to stream the menu resources. */
#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C6AA0);
#else
void func_801C6AA0(void) {
    s32 i;
    u16 frMask;

    /* menu1Choice: restore the saved choice for a plain main-menu open,
     * else start at entry 1. */
    if (D_80059460 == 0 && D_80059171 == 0) {
        g_Menu->menu1Choice = D_800594CC;
    } else {
        g_Menu->menu1Choice = 1;
    }
    g_Menu->unk337 = 0xFF;
    g_Menu->unk326 = 0x3C;
    g_Menu->unk334 = 0;
    g_Menu->unk335 = 0;
    g_Menu->unk32B = 0;

    /* availableCharacters[i] = character i present in the party flag mask. */
    frMask = (g_GameState.unk1D30 & g_GameState.FrMask) & 0x7FF;
    for (i = 0; i < 0x10; i++) {
        if (func_801C865C(frMask, (u8)i) != 0) {
            g_Menu->availableCharacters[i] = 1;
        } else {
            g_Menu->availableCharacters[i] = 0;
        }
    }

    /* Resolve the 3 active party slots: present + available members become
     * currentCharacterIDs; a member with a gear also sets the unk5C flag. */
    for (i = 0; i < MAX_PARTY_MEMBERS; i++) {
        u8 member = g_GameState.partyMembers[i];
        g_Menu->pManager->unk5C[4 + i] = 0;
        if (member == 0xFF || g_Menu->availableCharacters[member] == 0) {
            g_Menu->pManager->currentCharacterIDs[i] = 0xFF;
        } else {
            g_Menu->pManager->currentCharacterIDs[i] = member;
            g_Menu->unk32B++;
            if (g_GameState.characters[member].gearId != 0xFF) {
                g_Menu->pManager->unk5C[4 + i] = 1;
                g_Menu->unk33B++;
            }
        }
    }

    /* Record the first active party slot (leave unset if none). */
    for (i = 0; i < MAX_PARTY_MEMBERS; i++) {
        if (g_Menu->pManager->currentCharacterIDs[i] != 0xFF) {
            g_Menu->unk4CC[0x10] = (u8)i;
            break;
        }
    }

    func_801C65F4();
}
#endif

/* B1b: reset the menu's active render context. */
void func_801C6D4C(void) {
    g_Menu->renderContext = 0;
}

/* B1b: zero the 14-byte scratch block at unk4CC (three words + two bytes,
 * transcribed in asm order). */
#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C6D5C);
#else
void func_801C6D5C(void) {
    g_Menu->unk4CC[0xC] = 0;
    *(u32*)&g_Menu->unk4CC[0] = 0;
    *(u32*)&g_Menu->unk4CC[4] = 0;
    g_Menu->unk4CC[0xD] = 0;
    *(u32*)&g_Menu->unk4CC[8] = 0;
}
#endif

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C6D90);

/* B1b: upload the menu palette to VRAM, allocate the first string's work
 * buffer, and pre-render its content.  Twin of member_change's func_801C5B90.
 * func_801E7E68 (content) + func_801C6D90 (image) are still INCLUDE_ASM stubs. */
void func_801C6E0C(void) {
    SystemTransferPaletteToVRAM(0, 0x1D1);
    g_Menu->unk4E0[0].pVramBuffer = HeapAlloc(0x38E, 0);
    func_801E7E68(&g_Menu->unk4E0[0], D_801EA524, 0, 4);
    func_801C6D90();
}

/* B1b: unpack the 4 window-frame border textures (top/bottom/left/right) from
 * the atlas (unk2DC) into g_Menu's border texPage/clut/UV fields -- the coords
 * the border POLY builder + B2 render read.  Verbatim twin of member_change's
 * MemberChangeMenuInitializeWindowBorders (identical atlas indices). */
void func_801C6E68(void) {
    POLY_FT4 _unused;

    func_80026338(g_Menu->unk2DC, MENU_TEX_WINDOW_BORDER_TOP,
                  &g_Menu->unk46C, &g_Menu->texPage0,
                  &g_Menu->clutX0, &g_Menu->clutY0,
                  &g_Menu->texPageX0, &g_Menu->texPageY0);
    func_80026338(g_Menu->unk2DC, MENU_TEX_WINDOW_BORDER_BOTTOM,
                  &g_Menu->unk484, &g_Menu->texPage1,
                  &g_Menu->clutX1, &g_Menu->clutY1,
                  &g_Menu->texPageX1, &g_Menu->texPageY1);
    func_80026338(g_Menu->unk2DC, MENU_TEX_WINDOW_BORDER_LEFT,
                  &g_Menu->unk49C, &g_Menu->texPage2,
                  &g_Menu->clutX2, &g_Menu->clutY2,
                  &g_Menu->texPageX2, &g_Menu->texPageY2);
    func_80026338(g_Menu->unk2DC, MENU_TEX_WINDOW_BORDER_RIGHT,
                  &g_Menu->unk4B4, &g_Menu->texPage3,
                  &g_Menu->clutX3, &g_Menu->clutY3,
                  &g_Menu->texPageX3, &g_Menu->texPageY3);
}

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C6F70);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C72BC);

/* B1b: content-build coordinator.  Sets the selection menu's VRAM rect
 * (320x224 @ 704,256) and a per-menu context byte, then drives the builder
 * sequence.  For the main menu (D_80059460 == 0) the trailing 6400/6D5C pair
 * runs; sel 2/6 also run it (sel 2 bumps the context byte to 0x4C); sel 1 and
 * sel >= 3 (!= 6) skip it.  Preconditions (pSelectionMenu, unk348) are both
 * allocated by func_801C5F10's init slice. */
#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C7B0C);
#else
void func_801C7B0C(void) {
    u8 sel;

    g_Menu->pSelectionMenu->unk1180.x = 0x2C0;
    g_Menu->pSelectionMenu->unk1180.y = 0x100;
    g_Menu->pSelectionMenu->unk1180.w = 0x140;
    g_Menu->pSelectionMenu->unk1180.h = 0xE0;
    g_Menu->unk348->unk15B = 0x40;

    func_801C6AA0();
    func_801C6D4C();
    func_801C6E0C();
    func_801C6F70();
    func_801C6E68();

    sel = D_80059460;
    if (sel == 2) {
        g_Menu->unk348->unk15B = 0x4C;
        func_801C6400();
        func_801C6D5C();
    } else if (sel < 3) {
        if (sel == 0) {
            func_801C6400();
            func_801C6D5C();
        }
    } else if (sel == 6) {
        func_801C6400();
        func_801C6D5C();
    }
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C7BF4);
#else
extern s32* D_8005917C;
extern u8 D_801E9784;
extern s32 D_80059488;
extern void func_801C7D78(void);      /* input (stub) */
extern void func_8001BD40(s32, s32);
extern void func_801C7F34(s32);       /* view matrix (stub for now) */
extern void func_801D1D40(void);      /* view matrix (stub for now) */
extern void func_801D2968(void);      /* sub-draw (stub) */
extern void func_801D1CA0(void);      /* the render -> window (ported) */
extern void func_801C8BEC(void);
extern void func_801C8EE8(void);
extern void GameCheckAndHandleSoftReset(void);

/* Arc A: the per-frame menu draw. Flips the double-buffered gfx env, clears the
 * OT, runs the render (func_801D1CA0 -> ... -> the ported window-draw core),
 * then DrawOTag + present + MoveImage the selection-menu VRAM rect. The window
 * draws when func_801D1CA0's chain has built window POLYs (needs the setup/
 * window-build func_801D2D38 -> func_801E8474, not yet ported). */
void func_801C7BF4(void) {
    s32 s0;

    if (*D_8005917C != -1) {
        /* retail `break 1`: menu-active sentinel guard -- no-op in the port. */
    }
    func_801C7D78();
    if (D_801E9784 != 0) {
        GameCheckAndHandleSoftReset();
    }
    if (g_Menu->pGfxEnv == &g_Menu->gfxEnvs[0]) {
        g_Menu->pGfxEnv = &g_Menu->gfxEnvs[1];
    } else {
        g_Menu->pGfxEnv = &g_Menu->gfxEnvs[0];
    }
    g_Menu->renderContext = (g_Menu->renderContext == 0);
    ClearOTagR(g_Menu->pGfxEnv->ot, 0x10);
    func_8001BD40(0, 0xFF);
    func_801D1D40();
    g_Menu->unk2D8 += 1;
    func_801C7F34(D_80059488);
    func_801D2968();
    func_801D1CA0();
    s0 = (g_Menu->renderContext == 0);
    DrawSync(0);
    Vsync(0);
    PutDrawEnv(&g_Menu->pGfxEnv->drawEnv);
    PutDispEnv(&g_Menu->pGfxEnv->dispEnv);
    MoveImage(&g_Menu->pSelectionMenu->unk1180, 0, s0 * 224);
    DrawOTag(&g_Menu->pGfxEnv->ot[15]);
    func_801C8BEC();
    func_801C8EE8();
}
#endif

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C7D78);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C7F34);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C80B8);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C8164);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C81E0);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C8324);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C851C);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C8574);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C85C0);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C85DC);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C85F8);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C861C);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C8640);

/* Bit-select: is character `index` present in party mask `mask`?
 * (D_801E96A8 is the {1,2,4,8,...} table.) */
u16 func_801C865C(u16 mask, u8 index) {
    return D_801E96A8[index & 0xFF] & mask;
}

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C8678);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C8694);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C87C4);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C881C);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C891C);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C8960);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C8A10);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C8BEC);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C8CA4);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C8D1C);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C8D78);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C8EE8);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C9038);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C90B0);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C9270);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C93A8);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C9BCC);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C9D34);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C9EF4);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CA1D4);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CA480);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CA5F0);

INCLUDE_RODATA("../asm/menu/nonmatchings/main/misc", D_801C50A8);

INCLUDE_RODATA("../asm/menu/nonmatchings/main/misc", D_801C50AC);

INCLUDE_RODATA("../asm/menu/nonmatchings/main/misc", D_801C50B0);

INCLUDE_RODATA("../asm/menu/nonmatchings/main/misc", D_801C50B4);

INCLUDE_RODATA("../asm/menu/nonmatchings/main/misc", D_801C50B8);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CA750);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CA8C0);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CAA38);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CACF8);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CADB0);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CAE08);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CB184);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CB28C);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CB304);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CB8AC);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CB9E8);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CBA4C);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CBD90);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CC6D8);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CD2AC);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CD710);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CD81C);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CDB1C);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CDC6C);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CE0CC);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CE198);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CE2B4);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CE338);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CE3C8);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CE464);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CE540);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CE660);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CE860);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CEB5C);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CEBB4);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CEC40);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CF308);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CF37C);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CF5E4);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CF8D8);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CFB48);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CFF64);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D01D0);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D02D8);

/* Arc A (menu window render): project one window quad's 4 vertices via
 * RotTransPers4 into its POLY_FT4 xy coords, then AddPrim it to the gfxEnv OT.
 * The RotTransPers4+AddPrim window-draw pattern (member_change func_801C7EC8
 * precedent). Leaf of the window-draw chain -- func_801D09F0 loops this. */
void func_801D0954(SVECTOR* vertices, POLY_FT4* polys, s32 polyIndex, s32 otIndex) {
    long padA;
    long padB;

    RotTransPers4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                  (long*)&polys[polyIndex].x0, (long*)&polys[polyIndex].x1,
                  (long*)&polys[polyIndex].x2, (long*)&polys[polyIndex].x3,
                  &padA, &padB);
    AddPrim(&g_Menu->pGfxEnv->ot[otIndex], &polys[polyIndex]);
}

/* Project and queue every primitive that makes up one menu window.  The
 * textured border pieces share func_801D0954; the untextured background needs
 * its own RotTransPers4 because it is a POLY_G4 rather than a POLY_FT4. */
void func_801D09F0(s32 windowIndex, u8 hasScrollBar) {
    long interpolated;
    long flag;
    MenuWindow* pWindow;
    s32 i;

    pWindow = g_Menu->windows[windowIndex];

    for (i = 0; i < 4; i++) {
        func_801D0954(&pWindow->vertsWindowBorderCorners[i * 4],
                      &pWindow->polysWindowBorderCorners[i * 2],
                      pWindow->renderContext, pWindow->zIndex);
    }

    if (hasScrollBar) {
        for (i = 0; i < 2; i++) {
            func_801D0954(&pWindow->vertsScrollBarEnds[i * 4],
                          &pWindow->polysScrollBarEnds[i * 2],
                          pWindow->renderContext, pWindow->zIndex);
        }
        func_801D0954(pWindow->vertsScrollBarEmpty,
                      pWindow->polysScrollBarEmpty,
                      pWindow->renderContext, pWindow->zIndex);
    }

    func_801D0954(pWindow->vertsWindowBorderTop1,
                  pWindow->polysWindowBorderTop,
                  pWindow->renderContext, pWindow->zIndex);
    func_801D0954(pWindow->vertsWindowBorderTop2,
                  &pWindow->polysWindowBorderTop[2],
                  pWindow->renderContext, pWindow->zIndex);
    func_801D0954(pWindow->vertsWindowBorderBottom1,
                  pWindow->polysWindowBorderBottom,
                  pWindow->renderContext, pWindow->zIndex);
    func_801D0954(pWindow->vertsWindowBorderBottom2,
                  &pWindow->polysWindowBorderBottom[2],
                  pWindow->renderContext, pWindow->zIndex);
    func_801D0954(pWindow->vertsWindowBorderLeft1,
                  pWindow->polysWindowBorderLeft,
                  pWindow->renderContext, pWindow->zIndex);
    func_801D0954(pWindow->vertsWindowBorderLeft2,
                  &pWindow->polysWindowBorderLeft[2],
                  pWindow->renderContext, pWindow->zIndex);
    func_801D0954(pWindow->vertsWindowBorderRight1,
                  pWindow->polysWindowBorderRight,
                  pWindow->renderContext, pWindow->zIndex);
    func_801D0954(pWindow->vertsWindowBorderRight2,
                  &pWindow->polysWindowBorderRight[2],
                  pWindow->renderContext, pWindow->zIndex);

    RotTransPers4(&pWindow->vertsBackground[0],
                  &pWindow->vertsBackground[1],
                  &pWindow->vertsBackground[2],
                  &pWindow->vertsBackground[3],
                  (long*)&pWindow->polysBackground[pWindow->renderContext].x0,
                  (long*)&pWindow->polysBackground[pWindow->renderContext].x1,
                  (long*)&pWindow->polysBackground[pWindow->renderContext].x2,
                  (long*)&pWindow->polysBackground[pWindow->renderContext].x3,
                  &interpolated, &flag);
    /* Keep the native OT stride while preserving retail's address-add order. */
    AddPrim((void*)((pWindow->zIndex * sizeof(g_Menu->pGfxEnv->ot[0])) +
                    (uintptr_t)g_Menu->pGfxEnv->ot),
            &pWindow->polysBackground[pWindow->renderContext]);
    AddPrim((void*)((pWindow->zIndex * sizeof(g_Menu->pGfxEnv->ot[0])) +
                    (uintptr_t)g_Menu->pGfxEnv->ot),
            &pWindow->drawModes[pWindow->renderContext]);
}

/* Render each active window under either its caller-provided transform or the
 * retail default: no rotation and a +512 Z translation. */
void func_801D0C78(void) {
    SVECTOR rotation;
    VECTOR translation;
    MATRIX matTransform;
    SVECTOR _unused;
    s32 i;

    for (i = 0; i < MENU_MAX_NUM_WINDOWS; i++) {
        if (g_Menu->pManager->shouldRenderWindow[i]) {
            if (g_Menu->windows[i]->unk714 == 0) {
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
                func_801D09F0(i, g_Menu->windows[i]->hasScrollBar);
                PopMatrix();
            } else {
                func_801D09F0(i, g_Menu->windows[i]->hasScrollBar);
            }
        }
    }
}

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D0D90);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D0E20);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D0E38);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D0EBC);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D0ED4);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D0F54);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D0FD4);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D1030);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D10DC);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D1160);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D11F0);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D1258);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D12D4);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D13F8);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D1464);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D14B0);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D14FC);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D1640);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D17C4);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D1914);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D1AAC);

/* Run the per-frame menu draw passes in retail order.  The native port only
 * has the window pass (func_801D0C78) live at this point in Arc A; the other
 * INCLUDE_ASM passes still resolve to generated no-op stubs there. */
void func_801D1B20(void) {
    func_801D3B00();
    func_801D11F0();
    func_801CE3C8();
    func_801CE338();
    func_801D02D8();
    func_801D01D0();
    func_801CE540();
    func_801CE660();
    func_801CEB5C();
    func_801CEBB4();
    func_801CE464();
    func_801D13F8();
    func_801D1AAC();
    func_801D14FC();
    func_801D1640();
    func_801D17C4();
    func_801D1914();
    func_801D1464();
    func_801D14B0();
    func_801D0C78();
    func_801CEC40();
    func_801CF308();
}

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D1BE8);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D1C48);

/* Gate the draw dispatcher on the menu's render flag, then select the active
 * menu-mode pass.  The common trailing pass runs even when drawing is
 * disabled, matching retail. */
void func_801D1CA0(void) {
    if (g_Menu->shouldDrawMenu) {
        switch (D_80059460) {
        case 0:
            func_801D1B20();
            break;
        case 2:
            func_801D1BE8();
            break;
        case 6:
            func_801D1C48();
            break;
        }
    }
    func_801D1258();
}

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D1D40);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D1E80);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D1EB0);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D1EE0);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D22C4);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D22F4);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D2484);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D249C);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D25E4);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D261C);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D28A8);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D28FC);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D2968);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D29A8);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D2D38);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D2EC0);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D2F4C);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D32B4);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D3344);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D3444);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D3488);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D3674);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D36E0);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D397C);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D3B00);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D3C4C);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D3DB0);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D3FF8);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D433C);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D4688);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D49D0);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D4D1C);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D4EA0);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D4F2C);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D50EC);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D51EC);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D53D0);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D55B4);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D5794);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D5A50);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D5BA4);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D5CF8);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D5ED4);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D6194);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D6338);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D680C);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D6CF4);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D7154);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D74EC);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D7884);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D7C3C);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D7CFC);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D7F50);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D827C);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D83AC);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D84B4);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D85DC);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D8644);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D8DE4);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D8EA4);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D9704);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D9808);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D9B08);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D9C84);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D9E3C);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D9F34);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D9F98);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801DA4A8);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801DA518);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801DA5BC);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801DA9A8);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801DB02C);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801DB0A8);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801DB340);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801DB39C);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801DB5E4);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801DB920);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801DBD4C);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801DBDB4);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801DBE54);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801DC1D4);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801DC2CC);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801DC3D8);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801DCE60);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801DD5E8);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801DD790);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801DDF24);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801DE29C);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801DE2C8);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801DE36C);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801DE400);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801DE474);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801DE5CC);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801DF0D4);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801DF5D0);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801DF890);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801DFB68);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801DFE2C);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801DFF5C);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E0434);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E05D0);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E0F78);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E1014);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E1398);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E1418);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E1544);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E1AC8);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E20C8);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E2250);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E2324);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E2368);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E23CC);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E2AE0);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E2B80);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E2BE4);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E3088);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E31C0);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E35BC);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E36D4);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E3A80);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E3C2C);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E3ECC);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E4170);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E41C0);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E4258);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E42AC);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E433C);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E4754);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E4928);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E4998);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E4A28);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E4D10);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E5058);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E5178);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E53CC);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E56E8);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E5924);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E5ACC);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E5B3C);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E5B88);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E5E4C);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E61B0);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E6450);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E649C);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E64E0);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E6544);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E65E4);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E6668);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E68AC);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E6AE8);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E6B70);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E6CFC);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E6F5C);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E71B4);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E733C);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E76EC);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E781C);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E78C8);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E7C50);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E7E68);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E8018);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E8044);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E8070);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E8474);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E86C8);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E8978);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E8B4C);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E8DA8);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E8EAC);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E8F60);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E91C4);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E920C);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E927C);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E92CC);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E9340);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E93A0);
