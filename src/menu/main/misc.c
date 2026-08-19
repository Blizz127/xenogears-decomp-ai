#include "common.h"
#include "system/menu.h"
#include "main/game.h"
#ifdef XENO_PC_PORT
#include <stdio.h>
#endif

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
extern void func_801E7E68(MenuString* strings, u8* descriptorIds,
                          s32 yOffset, s32 count);
extern void func_801C6D90(void);                                  /* stub */
extern void* D_8006259C;    /* SEDS file pointer */
extern void* D_8005945C;    /* menu resource-pointer table (pResources) */
extern u16 D_801E96A8[];    /* migrated bit-select table {1,2,4,...} */
extern u8 D_80059460;
extern u8 D_80059171;
extern u8 D_800594CC;
extern u8 D_800594D0;

#ifdef XENO_PC_PORT
extern s32 func_801D9808(void);
extern s32 func_801D9F98(s32, s32);
extern s32 func_801E23CC(void);
extern s32 func_801DE29C(s32, s32);
extern s32 func_801DBE54(void);
extern s32 func_801E0F78(s32, s32);
extern s32 func_801E2BE4(void);
extern void func_8001B970(void);
extern void func_801D1EB0(void);
extern void func_801D29A8(u8, u8);
extern void func_801E3088(s32);
extern void func_801D3674(void);
extern void func_801E8018(s32 count, MenuString* strings, u8* descriptorIds);
extern void func_801E7C50(MenuString*, s32, s32, s32);
extern u8 D_801E96A4;
extern u8 D_801E977A;
extern u8 D_801E9784;
extern u8 D_801EA530[];

/* The retail fields at 0x42C and 0x440 are four-byte pointer slots.  Keep
 * their PSX width in the native-inflated SystemMenu and explicitly truncate
 * the port heap address, matching the established unk340 convention. */
static ItemMenuWork* MenuItemWork(void) {
    return (ItemMenuWork*)(uintptr_t)g_Menu->unk42C[0];
}

static void MenuSetItemWork(ItemMenuWork* p) {
    g_Menu->unk42C[0] = (u32)(uintptr_t)p;
}

static void* MenuUnk440Pointer(void) {
    return (void*)(uintptr_t)*(u32*)&g_Menu->unk440[0];
}

/* Live N2a capture marker: 1 = Items windows settled open, 2 = common-exit
 * teardown finished.  Port-only and inert outside the explicit harness. */
int g_XenoMenuN2Phase = 0;
/* Nav N2c-2a capture marker: 1 = initial/default target prompt built,
 * 2 = prompt cancel teardown complete. */
int g_XenoMenuN2c2Phase = 0;

/* Nav N3a-A1a capture marker: 1 = Abilities windows built + open animation
 * done, 2 = screen loop exited (teardown runs in func_801DC2CC via E3088). */
int g_XenoMenuN3aPhase = 0;

/* Nav N3a-A1b-1: the 14 per-row flag bytes from the latest func_801DC3D8
 * build (rowFlags[0..0xB] + the unk1090[0..1] spill rows 12/13), exported so
 * the headless harness can dump them and pick a boundary row without
 * including menu.h. */
unsigned char g_XenoMenuN3aRowFlags[0xE];
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C531C);
#else
/* Nav N2a: dispatch the ten retail menu entries, then run the shared close /
 * cleanup / main-menu-rebuild tail.  Only Items (entry 4) has a real native
 * screen in this slice; the remaining entry points retain their overlay stubs. */
s32 func_801C531C(s32 arg0) {
    s32 keepMainMenu = 1;
    s32 screenResult = 1;
    s32 entry = (u8)g_Menu->menu1Choice + (arg0 & 0xFF);

    D_801E977A = 1;

    switch (entry) {
    case 0:
        screenResult = 0;
        keepMainMenu = 0;
        break;
    case 1:
        screenResult = func_801D9F98(0, D_801E96A4);
        break;
    case 2:
        screenResult = func_801E23CC();
        break;
    case 3:
        screenResult = func_801DE29C(g_Menu->selectedPartySlot, 1);
        break;
    case 4:
        screenResult = func_801DBE54();
        break;
    case 5:
        screenResult = func_801E0F78(g_Menu->selectedPartySlot, 1);
        break;
    case 6:
        screenResult = func_801E2BE4();
        break;
    case 7:
        screenResult = func_801D9808();
        break;
    case 8:
        screenResult = func_801D9F98(1, 0);
        if ((screenResult & 0xFF) != 0) {
            D_800594D0 = 2;
            keepMainMenu = 0;
        }
        break;
    case 9:
        func_8001B970();
        screenResult = 0;
        keepMainMenu = 0;
        break;
    default:
        break;
    }

    g_Menu->unk32C->unk4F80[0x66] = 0;
    if ((screenResult & 0xFF) != 0) {
        func_801D1EB0();
        if (D_80059460 == 0) {
            func_801D29A8(1, 0);
        } else if (D_80059460 == 2) {
            func_801E8018(8, g_Menu->unk6E0, D_801EA530);
            g_Menu->unk348->unk15B = 0x4C;
        }
    }

    func_801E3088(arg0 & 0xFF);
    func_801D3674();
    g_Menu->pSelectionMenu->unk1192 = 0;
    g_Menu->pSelectionMenu->unk1193 = 1;
    g_Menu->pManager->unk4 = 1;
    g_Menu->pManager->unk3 = 1;
    g_Menu->unk337 = 0xFF;
    g_Menu->pManager->unkA = 0;
    D_801E9784 = 1;
    if (entry == 4) {
        g_XenoMenuN2Phase = 2;
        printf("[xeno-port][test] Nav N2a: Items teardown complete; main nav rebuilt\n");
        fflush(stdout);
    }
    return keepMainMenu;
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C55A0);
#else
extern void func_801C8574(s32);
extern void func_801D22C4(void);
extern void func_801E8044(s32, void*);
extern s32 func_801C531C(s32);
extern void func_801E8978(s32, s32, void*);
extern void func_801E8070(s32, void*, void*, void*, void*, s32, s32, s32);
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
            /* NB retail passes menu1Choice to func_801E8978 via $a1 register
             * residue (the lbu above) -- PSX idiom; pass it explicitly. */
            func_801E8978(7, g_Menu->menu1Choice, D_801EA19C);
            func_801E8070(8, g_Menu->unk6E0, D_801EA528, D_801E9E64,
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
            g_Menu->selectedPartySlot = (u8)i;
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

void func_801C6D90(void) {
    RECT rect;
    u16* pBuf = HeapAlloc(0x20, NULL);
    bzero(pBuf, 0x20);
    pBuf[1] = 0x7FFF;
    rect.x = 0;
    rect.y = 0;
    rect.w = 0x1C0;
    rect.h = 1;
    LoadImage(&rect, (u8*)pBuf);
    DrawSync(0);
    HeapFree(pBuf);
}

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

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C72BC);
#else
/* Nav N2a resource engine slice.  Retail has 24 modes; Items reaches only
 * mode 0 (archive file 2 -> item data + description bundle) and mode 0x10
 * (their matching frees).  Every other mode is deliberately fail-visible. */
void func_801C72BC(s32 mode) {
    u32* archive = NULL;
    u8 resourceMode = (u8)mode;

    if (resourceMode < 0x10) {
        ArchiveSetIndex(0x10, 0);
        archive = HeapAlloc(ArchiveDecodeAlignedSize(2), 1);
        ArchiveReadFileToBuffer(2, archive, 0, 0x80);
        ArchiveCdDataSync(0);
        ResolveArchiveEntryPointers(archive);
    }

    if (resourceMode == 0) {
        ItemMenuWork* work = MenuItemWork();
        g_Menu->unk330->pItemsData =
            LZSSHeapDecompress((void*)(uintptr_t)archive[1], 0);
        work->descriptionBundle = (u32)(uintptr_t)
            LZSSHeapDecompress((void*)(uintptr_t)archive[0xF], 0);
    } else if (resourceMode == 0x10) {
        ItemMenuWork* work = MenuItemWork();
        HeapFree(g_Menu->unk330->pItemsData);
        HeapFree((void*)(uintptr_t)work->descriptionBundle);
    } else if (resourceMode == 2 || resourceMode == 0x12) {
        /* Nav N3a-A1a: the Abilities bank.  Mode 2 decompresses one per-record
         * blob for every PRESENT party slot (0xFF = absent) plus the shared
         * ability bank; 0x12 is the exact mirror and takes no archive, since
         * the archive is only opened for modes < 0x10. */
        AbilityMenuWork* work =
            (AbilityMenuWork*)(uintptr_t)g_Menu->unk42C[1];
        s32 i;

        for (i = 0; i < 3; i++) {
            u8 id = g_Menu->pManager->currentCharacterIDs[i];

            if (id != 0xFF) {
                u32* pSlot = (u32*)&g_Menu->unk330->unk20[id * 4];

                if (resourceMode == 2) {
                    *pSlot = (u32)(uintptr_t)LZSSHeapDecompress(
                        (void*)(uintptr_t)archive[4 + id], 0);
                } else {
                    HeapFree((void*)(uintptr_t)*pSlot);
                }
            }
        }
        if (resourceMode == 2) {
            work->abilityBank = (u32)(uintptr_t)LZSSHeapDecompress(
                (void*)(uintptr_t)archive[0x10], 0);
        } else {
            HeapFree((void*)(uintptr_t)work->abilityBank);
        }
    } else {
        static u32 warnedModes;
        u32 bit = (resourceMode < 32) ? (1u << resourceMode) : 0;
        if (bit == 0 || (warnedModes & bit) == 0) {
            warnedModes |= bit;
            printf("[xeno-port][stub-path] func_801C72BC mode %u not ported "
                   "(ported: 0, 2, 0x10, 0x12)\n", resourceMode);
            fflush(stdout);
        }
    }

    if (resourceMode < 0x10) {
        HeapFree(archive);
    }
}
#endif

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

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C7D78);
#else
extern int ControllerGetType(int controllerIndex);
extern int ControllerPopState(void);
extern void ControllerResetState(void);
extern void SoundMuteAllSpuChannels(void);
extern void SoundEnableAllSpuChannels(void);
extern s32 func_80036410(void);
extern u16 g_C1ButtonStatePressedOnce;
extern u16 g_C1ButtonStateReleased;
extern s32 D_80059488;

/* Nav N1: the main-menu input reader (per frame, from func_801C7BF4).
 * Spins while the controller is absent (muting the SPU, saving/restoring
 * D_80059488), then drains the pad-state queue and maps button edges to the
 * MENU_INPUT_* code in g_Menu->input.  Directional inputs play nav blip 1;
 * confirm/cancel play 2/3 via func_801C8574. */
/* Port-only harness instrumentation: bumped once per reader execution so the
 * headless nav test (psyq_compat.c, XENO_MENU_NAV_TEST) can gate its synthetic
 * input on the menu actually reaching its interactive loop, independent of
 * Vsync frame count (the field's open/close phases run at a different cadence
 * and reset the pad queue).  Inert unless that env harness is armed. */
int g_XenoMenuNavReaderTicks = 0;

void func_801C7D78(void) {
    s32 input = 0;
    s32 present = 1;
    s32 savedD59488 = 0;

    g_XenoMenuNavReaderTicks++;

    for (;;) {
        if (ControllerGetType(0) != 0) {
            if ((input & 0xFF) != 0) {
                SoundEnableAllSpuChannels();
                D_80059488 = savedD59488;
            }
            present -= 1;
        } else if ((input & 0xFF) == 0) {
            SoundMuteAllSpuChannels();
            input += 1;
            savedD59488 = D_80059488;
        }
        if ((present & 0xFF) == 0) {
            break;
        }
    }

    input = 8;  /* MENU_INPUT_IDLE */
    if (func_80036410() != 0) {
        ControllerResetState();
    } else {
        while (ControllerPopState()) {
            u16 pressed = g_C1ButtonStatePressedOnce;
            u16 released;

            if (pressed & 0x2000) { input = 0; goto blip; }   /* RIGHT */
            if (pressed & 0x4000) { input = 1; goto blip; }   /* DOWN */
            if (pressed & 0x8000) { input = 2; goto blip; }   /* LEFT */
            if (pressed & 0x1000) { input = 3; goto blip; }   /* UP */
            released = g_C1ButtonStateReleased;
            if (released & 0x20) {                            /* CIRCLE */
                input = 4;
                func_801C8574(2);
                goto store;
            }
            if (released & 0x40) {                            /* CROSS */
                input = 5;
                func_801C8574(3);
                goto store;
            }
            if (released & 0x80) { input = 6; goto store; }
            if (released & 0x10) { input = 7; goto store; }
            if (pressed & 0x4)  { input = 0xA; goto blip; }
            if (pressed & 0x8)  { input = 9; goto blip; }
            if (released & 0x100) { input = 0xC; goto store; }
        }
        goto store;
    blip:
        func_801C8574(1);
    }
store:
    g_Menu->input = (u8)input;
}
#endif

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C7F34);

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C80B8);
#else
/* Arc A portraits: parse a value into g_Menu->digits[9] (decimal, MSD first),
 * blanking leading zeros to 0xFF (the quad builders skip 0xFF; digit glyphs
 * are atlas entries 0-9). */
void func_801C80B8(u32 value) {
    u32 divisor = 100000000;
    s32 i;

    for (i = 0; i < 9; i++) {
        g_Menu->digits[i] = (u8)(value / divisor);
        value %= divisor;
        divisor /= 10;
    }
    for (i = 1; i < 9; i++) {
        if (g_Menu->digits[i] != 0) {
            if (g_Menu->digits[i - 1] == 0) {
                g_Menu->digits[i - 1] = 0xFF;
            }
            break;
        }
        g_Menu->digits[i - 1] = 0xFF;
    }
}
#endif

void func_801C8164(POLY_G4* p, u8 r, u8 g, u8 b) {
    SetPolyG4(p);
    p->r0 = r; p->g0 = g; p->b0 = b;
    p->r1 = r; p->g1 = g; p->b1 = b;
    p->r2 = 0; p->g2 = 0; p->b2 = 0;
    p->r3 = 0; p->g3 = 0; p->b3 = 0;
}

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C81E0);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C8324);
#else
/* Arc A: the menu-open animation state -- three 0x24-byte slots living in
 * g_Menu->unk0[0x6C] (3 x 0x24), one per party-member portrait frame. */
typedef struct {
    /* 0x00 */ s32 curX;
    /* 0x04 */ s32 toX;
    /* 0x08 */ s32 curY;
    /* 0x0C */ s32 toY;
    /* 0x10 */ s32 stepX;   /* 8.8 fixed-point per-tick step */
    /* 0x14 */ s32 stepY;
    /* 0x18 */ s32 accX;    /* 8.8 accumulated delta from curX/curY */
    /* 0x1C */ s32 accY;
    /* 0x20 */ u8 dirX;     /* 1 = moving toward smaller X */
    /* 0x21 */ u8 dirY;
    /* 0x22 */ u8 speed;    /* ticks applied per step call */
    /* 0x23 */ u8 done;     /* set when the dominant axis passes its target */
} MenuOpenAnim;
#define MENU_OPEN_ANIM(slot) (&((MenuOpenAnim*)g_Menu->unk0)[slot])

/* Initialize anim slot: from (fromX,fromY) toward (toX,toY); the dominant axis
 * steps 1.0/tick (0x100), the other axis proportionally. */
void func_801C81E0(s32 fromX, s32 fromY, s32 toX, s32 toY, s32 speed, u8 slot) {
    MenuOpenAnim* s = MENU_OPEN_ANIM(slot);
    s32 dx, dy;

    s->curX = fromX;
    s->curY = fromY;
    s->toX = toX;
    s->toY = toY;
    if (toX < fromX) {
        dx = fromX - toX;
        s->dirX = 1;
    } else {
        dx = toX - fromX;
        s->dirX = 0;
    }
    if (toY < fromY) {
        dy = fromY - toY;
        s->dirY = 1;
    } else {
        dy = toY - fromY;
        s->dirY = 0;
    }
    if (dy < dx || dy == dx) {
        s->stepX = 0x100;
        /* retail divides unconditionally; dx==0 implies dy==0 (from==to),
         * which the menu never requests -- guard the host SIGFPE anyway. */
        s->stepY = (dx != 0) ? ((dy << 8) / dx) : 0x100;
    } else {
        s->stepY = 0x100;
        s->stepX = (dx << 8) / dy;
    }
    s->speed = (u8)speed;
    s->accX = 0;
    s->accY = 0;
    s->done = 0;
}

/* Step anim slot by `speed` ticks and mark done once the dominant axis has
 * passed its target. */
void func_801C8324(u8 slot) {
    MenuOpenAnim* s = MENU_OPEN_ANIM(slot);
    s32 i;

    for (i = 0; i < s->speed; i++) {
        if (s->dirX) {
            s->accX -= s->stepX;
        } else {
            s->accX += s->stepX;
        }
        if (s->dirY) {
            s->accY -= s->stepY;
        } else {
            s->accY += s->stepY;
        }
    }

    if (s->stepX == 0x100) {
        if (s->dirX) {
            if (s->curX + (s->accX >> 8) < s->toX) {
                s->done = 1;
            }
        } else {
            if (s->toX < s->curX + (s->accX >> 8)) {
                s->done = 1;
            }
        }
    } else {
        if (s->dirY) {
            if (s->curY + (s->accY >> 8) < s->toY) {
                s->done = 1;
            }
        } else {
            if (s->toY < s->curY + (s->accY >> 8)) {
                s->done = 1;
            }
        }
    }
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C851C);
#else
/* Arc A verts: write one screen-space quad into a 4-SVECTOR strip.
 * (x,y) is the top-left in 320x224 screen coords; the GTE projection origin
 * is the screen center, hence the -0xA0/-0x70 (160/112) rebase.  w/h may be
 * negative (the corner pieces mirror by flipping extent signs). */
void func_801C851C(SVECTOR* verts, s32 x, s32 y, s32 w, s32 h) {
    verts[0].vx = (s16)(x - 0xA0);
    verts[0].vy = (s16)(y - 0x70);
    verts[0].vz = 0;
    verts[1].vx = (s16)(x + w - 0xA0);
    verts[1].vy = (s16)(y - 0x70);
    verts[1].vz = 0;
    verts[2].vx = (s16)(x - 0xA0);
    verts[2].vy = (s16)(y + h - 0x70);
    verts[2].vz = 0;
    verts[3].vx = (s16)(x + w - 0xA0);
    verts[3].vy = (s16)(y + h - 0x70);
    verts[3].vz = 0;
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C8574);
#else
extern void func_80039DB8(s32 packedId);

/* Nav N1: the menu SFX player -- gated on unk32A (menu sounds enabled),
 * packs the resource SoundFile's bank id (unk2E4->unk14) with the effect id. */
void func_801C8574(s32 soundId) {
    if (g_Menu->unk32A) {
        func_80039DB8(((s32)*(u16*)((u8*)g_Menu->unk2E4 + 0x14) << 16) |
                      (soundId & 0xFF));
    }
}
#endif

extern u16 D_801E96C8[];

u16 func_801C85C0(u8 idx) {
    return D_801E96C8[idx];
}

extern u16 D_801E96A8[];

u16 func_801C85DC(u8 idx) {
    return D_801E96A8[idx];
}

u16 func_801C85F8(u8 idx) {
    return (u16)(~D_801E96C8[idx]);
}

u16 func_801C861C(u8 idx) {
    return (u16)(~D_801E96A8[idx]);
}

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C8640);
#else

extern u16 D_801E96C8[];

/* Nav N3a-A1a: mask lookup -- D_801E96C8[index] & mask.  Leaf, no frame. */
s32 func_801C8640(s32 mask, s32 index) {
    return D_801E96C8[index & 0xFF] & mask;
}
#endif

/* Bit-select: is character `index` present in party mask `mask`?
 * (D_801E96A8 is the {1,2,4,8,...} table.) */
u16 func_801C865C(u16 mask, u8 index) {
    return D_801E96A8[index & 0xFF] & mask;
}

extern u32 D_801E96E8[];

u32 func_801C8678(u32 mask, u8 idx) {
    return mask & D_801E96E8[idx];
}

extern void func_801D1E80(void);
extern void func_801D22F4(s32);
extern s32 ArchiveGetDiscNumber(void);
extern void func_801E92CC(void);
extern s32 func_801E93A0(s32);

void func_801C8694(s32 discNum) {
    u8 waiting = 0;
    func_801D1E80();
    if (*(u8*)((u8*)g_Menu + 0x329) != 0) {
        waiting = 1;
        while (*(u8*)((u8*)g_Menu + 0x329) != 0) {
            func_801C7BF4();
        }
    }
    func_801D22F4(0);
    if (waiting) {
        u8 targetDisc = discNum + 1;
        u8 param = (u8)(discNum * 3 - 0x7D);
        while (waiting) {
            if (ArchiveGetDiscNumber() == targetDisc) {
                waiting = 0;
            } else {
                func_801E92CC();
                func_801D2F4C(param);
                if (func_801E93A0(targetDisc)) {
                    u8 delay;
                    func_801D32B4(0);
                    func_801D2F4C(0x89);
                    for (delay = 0x1D; (delay & 0xFF) != 0; delay--) {
                        func_801C7BF4();
                    }
                    func_801D32B4(0);
                    func_801C7BF4();
                } else {
                    func_801D32B4(0);
                    waiting = 0;
                }
            }
        }
    }
    func_801D2484();
}

void func_801C87C4(void) {
    UnDeliverEvent(0xF4000001, 0x4);
    UnDeliverEvent(0xF4000001, 0x8000);
    UnDeliverEvent(0xF4000001, 0x100);
    UnDeliverEvent(0xF4000001, 0x2000);
}

s32 func_801C881C(s32 port) {
    void* pMenu;
    void* pData;
    while (1) {
        pMenu = g_Menu;
        pData = *(void**)((u8*)pMenu + 0x32C);
        if (TestEvent(*(void**)((u8*)pData + 0x4FF8)) == 1) {
            func_801C87C4();
            return 3;
        }
        pMenu = g_Menu;
        pData = *(void**)((u8*)pMenu + 0x32C);
        if (TestEvent(*(void**)((u8*)pData + 0x4FF0)) == 1) {
            func_801C87C4();
            return 1;
        }
        pMenu = g_Menu;
        pData = *(void**)((u8*)pMenu + 0x32C);
        if (TestEvent(*(void**)((u8*)pData + 0x4FEC)) == 1) {
            func_801C87C4();
            return 0;
        }
        pMenu = g_Menu;
        pData = *(void**)((u8*)pMenu + 0x32C);
        if (TestEvent(*(void**)((u8*)pData + 0x4FF4)) == 1) {
            func_801C87C4();
            return 2;
        }
    }
}

extern u32 D_801E9768[];
extern s32 _card_info(s32);

s32 func_801C891C(s32 port) {
    if (_card_info(port) == 0) return -1;
    {
        u8 result = (u8)func_801C881C(port);
        return (s32)D_801E9768[result];
    }
}

extern void func_801C7BF4(void);

void func_801C8960(void) {
    void* pMenu;
    void* pData;
    func_801C7BF4();
    EnterCriticalSection();
    pMenu = g_Menu;
    pData = *(void**)((u8*)pMenu + 0x32C);
    CloseEvent(*(void**)((u8*)pData + 0x4FEC));
    pMenu = g_Menu;
    pData = *(void**)((u8*)pMenu + 0x32C);
    CloseEvent(*(void**)((u8*)pData + 0x4FF0));
    pMenu = g_Menu;
    pData = *(void**)((u8*)pMenu + 0x32C);
    CloseEvent(*(void**)((u8*)pData + 0x4FF4));
    pMenu = g_Menu;
    pData = *(void**)((u8*)pMenu + 0x32C);
    CloseEvent(*(void**)((u8*)pData + 0x4FF8));
    ExitCriticalSection();
}

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C8A10);

extern u8 D_801E9779;

void func_801C8BEC(void) {
    void* pMenu = g_Menu;
    void* pData = *(void**)((u8*)pMenu + 0x32C);
    if (*(u8*)((u8*)pData + 0x4FE6) != 0) {
        u8 counter = *(u8*)((u8*)pMenu + 0x326) + 1;
        *(u8*)((u8*)pMenu + 0x326) = counter;
        if (counter > D_801E9779) {
            func_801C8A10(0);
            func_801C8A10(1);
            {
                void* pMenu2 = g_Menu;
                void* pData2 = *(void**)((u8*)pMenu2 + 0x32C);
                if (*(s32*)((u8*)pData2 + 0x4F74) == -1 &&
                    *(s32*)((u8*)pData2 + 0x4F78) == -1) {
                    *(u8*)((u8*)pMenu2 + 0x334) = 0;
                }
            }
            *(u8*)((u8*)g_Menu + 0x326) = 0;
        }
    }
}

void func_801C8CA4(u8 idx) {
    void* pMenu = g_Menu;
    void* pData = *(void**)((u8*)pMenu + 0x32C);
    s32 val = *(s32*)((u8*)pData + 0x4F74 + idx * 4);
    if (val != -2) {
        s32 i;
        *(u8*)((u8*)pData + 0x4FE6) = 2;
        for (i = 0x3B; i != 0; i--) {
            func_801C7BF4();
        }
        pMenu = g_Menu;
        *(u8*)(*(void**)((u8*)pMenu + 0x32C) + 0x4FE6) = 0;
    }
}

void func_801C8D1C(u8 idx) {
    void* pMenu = g_Menu;
    void* pData = *(void**)((u8*)pMenu + 0x32C);
    s32 val = *(s32*)((u8*)pData + 0x4F74 + idx * 4);
    if (val != -2) {
        s32 i;
        for (i = 0x3B; i != 0; i--) {
            Vsync(0);
        }
    }
}

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C8D78);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C8EE8);

s32 func_801C9038(char* path, void* pBuf) {
    s32 fd = open(path, 3);
    if (fd == -1) return -1;
    if (read(fd, pBuf, 0x200) == 0x200) {
        close(fd);
        return 0;
    }
    close(fd);
    return -1;
}

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

extern void func_801D2F4C(u8);
extern u32 func_801CAA38(u8);
extern u32 func_801D32B4(u32);

u8 func_801CACF8(u8 arg0, u8 arg1, u8 arg2) {
    u32 result;
    void* pMenu;
    func_801D2F4C(arg0);
    pMenu = g_Menu;
    *(u8*)(*(void**)((u8*)pMenu + 0x428) + 0x143) = 1;
    result = func_801CAA38(arg2);
    result = func_801D32B4(result);
    if (arg1 != 0xFF && (result & 0xFF) != 0) {
        func_801D2F4C(arg0);
        pMenu = g_Menu;
        *(u8*)(*(void**)((u8*)pMenu + 0x428) + 0x143) = 1;
        result = func_801CAA38(arg2);
        result = func_801D32B4(result);
    }
    return (u8)(result & 0xFF);
}

void func_801CADB0(void) {
    void* pMenu = g_Menu;
    void* pData = *(void**)((u8*)pMenu + 0x32C);
    *(u32*)((u8*)pData + 0x4F80) = 0xFF;
    pData = *(void**)((u8*)pMenu + 0x32C);
    *(u32*)((u8*)pData + 0x4F7C) = 0;
    pData = *(void**)((u8*)pMenu + 0x32C);
    if (*(u8*)((u8*)pData + 0x4FE4) == 0 && *(u8*)((u8*)pData + 0x4FE5) != 0) {
        *(u32*)((u8*)pData + 0x4F7C) = 0xF;
    }
}

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CAE08);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CB184);

extern u16 D_8006F958[];
extern u16 D_8005A3A0[];
extern s32 D_80059488;
extern void* func_801E4D10(s32, s32);

void func_801CB28C(s32 arg0) {
    void* pMenu = g_Menu;
    void* pData = func_801E4D10(arg0, *(s32*)((u8*)pMenu + 0x330));
    s32 i;
    D_80059488 = *(s32*)pData;
    for (i = 0; i < 0x10; i++) {
        D_8005A3A0[i] = D_8006F958[i];
    }
    func_801CB184();
}

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CB304);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CB8AC);

extern u8 D_801EA6D0[];

u8 func_801CB9E8(u8 idx, u8 startIdx) {
    s32 result = 0;
    s32 i = 0;
    if (startIdx == 0xFF) {
        u8* pEntry = &D_801EA6D0[idx * 16];
        for (i = 0; i < 0xF; i++, pEntry++) {
            if (*pEntry == 0) {
                result = i;
                break;
            }
        }
        return (u8)result;
    }
    return startIdx;
}

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CBA4C);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CBD90);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CC6D8);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CD2AC);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CD710);

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CD81C);
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CDB1C);
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CDC6C);
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CE0CC);
#else
extern s32 func_8002675C(u8*, s32, void*, s32, s32, s32, s32);
extern void func_801C80B8(u32);
extern void func_801E920C(POLY_FT4*, s32, s32, s32, s32, s32, s32);
extern void func_801E927C(POLY_FT4*);
extern u16 g_SystemPalette1;
extern u16 g_SystemPalette2;
extern s32 D_801EA054[];
extern s32 D_801EA098[];
extern s32 D_801EA0DC[];
extern s32 D_801EA120[];
extern s32 D_801EA4DC[];
extern u8 D_801EA578[];
extern u8 D_801EA5C4[];

static void MenuBuildPanelDigits(MenuCharacter* panel, POLY_FT4* polys,
                                 u8* count, u32 value, s32 firstDigit,
                                 s32 digitCount, s32 x, s32 y,
                                 s32 compact) {
    s32 i = 0;
    s32 built = 0;

    func_801C80B8(value);
    *count = 0;
    while (i < digitCount) {
        s32 digitIndex = i;
        u8 digit = g_Menu->digits[firstDigit + digitIndex];

        if (compact) {
            i++;
        }

        if (digit != 0xFF) {
            s32 column = compact ? built : digitIndex;
            *count += (u8)func_8002675C(
                g_Menu->unk2DC, digit, &polys[(*count) * 2],
                g_Menu->renderContext, x + column * 8, y, 0x1000);
            built++;
        }
        if (!compact) {
            i++;
        }
    }
    (void)panel;
}

void func_801CD81C(MenuCharacter* panel, u8 charId, u8 slot,
                   s32* xTable, s32* yTable, u8 mode) {
    s32 i = 0;
    s32 rowOffset = slot * 0x38;
    POLY_FT4* face;

    panel->descriptionStringsLength = 0;
    while (i < 9) {
        s32 glyphIndex = i;
        s32 glyph = D_801EA4DC[mode * 9 + glyphIndex];

        i++;

        if (glyph != 0xFFFF) {
            panel->descriptionStringsLength += (u8)func_8002675C(
                g_Menu->unk2DC, glyph,
                &panel->polysDescriptionStrings[panel->descriptionStringsLength * 2],
                g_Menu->renderContext, xTable[glyphIndex],
                yTable[glyphIndex] + rowOffset,
                0x1000);
        }
    }

    func_8002675C(g_Menu->unk2DC, 0x14B + slot,
                  panel->polysPortraitSmall, g_Menu->renderContext,
                  xTable[9], yTable[9] + rowOffset, 0x1000);
    face = &panel->polys4B0[g_Menu->renderContext];
    func_801E927C(face);
    face->tpage = GetTPage(0, 0, 0x180, 0);
    if ((mode == 0 && !(charId & 1)) ||
        (mode != 0 && (g_GameState.characters[charId].gearId & 1))) {
        face->clut = g_SystemPalette1;
    } else {
        face->clut = g_SystemPalette2;
    }
    func_801E920C(face, xTable[16], yTable[16] + rowOffset,
                  (*(s32*)(D_801EA578 + (mode * 3 + slot) * 4) << 2) & 0xFC,
                  D_801EA5C4[mode * 12 + slot * 4], mode * 0x18 + 0x48,
                  0xD);
}

void func_801CDB1C(MenuCharacter* panel, u8 charId, u8 slot,
                   s32* xTable, s32* yTable) {
    MenuBuildPanelDigits(panel, panel->polysLevelString,
                         &panel->levelStringLength,
                         g_GameState.characters[charId].level, 6, 3,
                         xTable[10], yTable[10] + slot * 0x38, 0);
    func_801C80B8(g_GameState.characters[charId].unk63);
    panel->unkBE1 = 0;
}

void func_801CDC6C(MenuCharacter* panel, u8 charId, u8 slot,
                   s32* xTable, s32* yTable, u8 mode) {
    u32 hp;
    u32 maxHp;
    s32 firstDigit;
    s32 digitCount;

    if (mode == 0) {
        hp = g_GameState.characters[charId].hp;
        maxHp = g_GameState.characters[charId].maxHp;
        firstDigit = 6;
        digitCount = 3;
    } else {
        u8 gearId = g_GameState.characters[charId].gearId;
        hp = g_GameState.gears[gearId].hp;
        maxHp = g_GameState.gears[gearId].maxHp;
        firstDigit = 4;
        digitCount = 5;
    }

    MenuBuildPanelDigits(panel, panel->polysHpString, &panel->hpStringLength,
                         hp, firstDigit, digitCount, xTable[12],
                         yTable[12] + slot * 0x38, 0);
    MenuBuildPanelDigits(panel, panel->polysMaxHpString,
                         &panel->maxHpStringLength, maxHp, firstDigit,
                         digitCount, xTable[13], yTable[13] + slot * 0x38, 1);

    if (mode == 0) {
        MenuBuildPanelDigits(panel, panel->polysMpString,
                             &panel->mpStringLength,
                             g_GameState.characters[charId].mp, 7, 2,
                             xTable[14], yTable[14] + slot * 0x38, 0);
        MenuBuildPanelDigits(panel, panel->polysMaxMpString,
                             &panel->maxMpStringLength,
                             g_GameState.characters[charId].maxMp, 7, 2,
                             xTable[15], yTable[15] + slot * 0x38, 1);
    }
}

void func_801CE0CC(MenuCharacter* panel, u8 charId, u8 slot,
                   s32* xTable, s32* yTable, u8 mode) {
    func_801CD81C(panel, charId, slot, xTable, yTable, mode);
    func_801CDB1C(panel, charId, slot, xTable, yTable);
    func_801CDC6C(panel, charId, slot, xTable, yTable, mode);
    panel->unkBE7 = 1;
    panel->renderContext = (u8)g_Menu->renderContext;
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CE198);
#else
/* Project and queue `count` double-buffered textured quads.  Retail advances
 * the render-context index by two per logical string, selecting the same half
 * of each MenuString's POLY_FT4 pair. */
void func_801CE198(s32 count, SVECTOR* vertices, POLY_FT4* polys,
                   s32 renderContext) {
    s32 i;

    for (i = 0; i < count; i++) {
        POLY_FT4* poly = &polys[renderContext + i * 2];
        SVECTOR* quad = &vertices[i * 4];
        long interpolated;
        long flag;

        RotTransPers4(&quad[0], &quad[1], &quad[2], &quad[3],
                      (long*)&poly->x0, (long*)&poly->x1,
                      (long*)&poly->x2, (long*)&poly->x3,
                      &interpolated, &flag);
        AddPrim(&g_Menu->pGfxEnv->ot[4], poly);
    }
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CE2B4);
#else
/* Arc A portraits: AddPrim `count` double-buffered polys from a portrait list
 * (each entry is a pair; draw the renderCtx half). */
void func_801CE2B4(s32 count, u8* pList, s32 renderCtx) {
    s32 i;

    for (i = 0; i < count; i++) {
        AddPrim(&g_Menu->pGfxEnv->ot[4],
                pList + (renderCtx + i * 2) * sizeof(POLY_FT4));
    }
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CE338);
#else
/* Nav N1: the pointer-cursor sub-renderer (func_801D1B20's list).  Always
 * AddPrims the cursor draw-mode; the pointer sprite itself is gated on
 * pManager->unk4 (armed by func_801E8978). */
void func_801CE338(void) {
    AddPrim(&g_Menu->pGfxEnv->ot[4],
            &g_Menu->unk348->drModes1[g_Menu->unk348->unk159]);
    if (g_Menu->pManager->unk4) {
        AddPrim(&g_Menu->pGfxEnv->ot[4],
                &g_Menu->unk348->polysPointerCursor[g_Menu->unk348->cursorRenderContext]);
    }
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CE3C8);
#else
void func_801CE3C8(void) {
    s32 i;

    if (g_Menu->pManager->shouldRenderPointerCursors) {
        for (i = 0; i < MENU_MAX_NUM_CURSORS; i++) {
            if (g_Menu->pCursors->shouldRender[i]) {
                AddPrim(&g_Menu->pGfxEnv->ot[4],
                        &g_Menu->pCursors->polysCursor[
                            i * 2 + g_Menu->pCursors->renderContexts[i]]);
            }
        }
    }
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CE464);
#else
/* Arc A content: the GOLD-window + ICON-STRIP sub-renderer (func_801D1B20's
 * list).  unk5[0] gates the gold digits (unk340[0] buffer, count at +0x320,
 * rc at +0x324) + the "G" unit glyph at +0x2D0; unk5[1] gates the icon strip
 * (unk340[4] buffer: 7 icons + the two separators at +0x230). */
void func_801CE464(void) {
    if (g_Menu->pManager->unk5[0]) {
        u8* buf = (u8*)(uintptr_t)*(u32*)&g_Menu->unk340[0];
        s32 rc = buf[0x324];

        func_801CE2B4(*(s32*)(buf + 0x320), buf, rc);
        AddPrim(&g_Menu->pGfxEnv->ot[4], buf + 0x2D0 + rc * sizeof(POLY_FT4));
    }
    if (g_Menu->pManager->unk5[1]) {
        u8* buf = (u8*)(uintptr_t)*(u32*)&g_Menu->unk340[4];
        s32 rc = buf[0x370];

        func_801CE2B4(7, buf, rc);
        func_801CE2B4(4, buf + 0x230, rc);
    }
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CE540);
#else
extern void func_801CE2B4(s32 count, u8* pList, s32 renderCtx);

/* Arc A portraits: THE portrait sub-renderer (one of func_801D1B20's 22).
 * For each visible party slot (pManager->unk0[i], set by func_801D5A50):
 * AddPrim the frame + face quads, then the counted lists (icons, level,
 * HP/MP/EXP digits) -- all to ot[4] via the proven pipeline. */
void func_801CE540(void) {
    s32 i;

    for (i = 0; i < 3; i++) {
        u8* buf = (u8*)(uintptr_t)*(u32*)&g_Menu->unk39C[i * 4];

        if (g_Menu->pManager->unk0[i]) {
            s32 rc = ((u8*)buf)[0x1270];

            AddPrim(&g_Menu->pGfxEnv->ot[4], buf + rc * sizeof(POLY_FT4));
            AddPrim(&g_Menu->pGfxEnv->ot[4], buf + 0x50 + rc * sizeof(POLY_FT4));
            func_801CE2B4(buf[0x1279], buf + 0xA0, rc);
            func_801CE2B4(buf[0x1273], buf + 0xAF0, rc);
            func_801CE2B4(buf[0x1274], buf + 0xBE0, rc);
            func_801CE2B4(buf[0x1275], buf + 0xCD0, rc);
            func_801CE2B4(buf[0x1276], buf + 0xD70, rc);
            func_801CE2B4(buf[0x1277], buf + 0xE10, rc);
            func_801CE2B4(buf[0x1271], buf + 0x910, rc);
        }
    }
}
#endif

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CE660);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CE860);

extern void func_801CE198(s32, void*, u8);
extern void func_801CE860(void);

void func_801CEB5C(void) {
    void* pMenu = g_Menu;
    void* pManager = *(void**)((u8*)pMenu + 0x33C);
    if (*(u8*)((u8*)pManager + 8) != 0) {
        void* pData = *(void**)((u8*)pMenu + 0x35C);
        u8 val1 = *(u8*)((u8*)pData + 0x32F3);
        u8 val2 = *(u8*)((u8*)pData + 0x32F1);
        func_801CE198(val1, (u8*)pData + 0x2420, val2);
        func_801CE860();
    }
}

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CEBB4);

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CEC40);
#else
/* Arc A content: THE selection-menu sub-renderer (func_801D1B20's list).
 * Gated on pManager->shouldRenderSelectionMenu.  When the dim state (unk1192,
 * set by the confirm) differs from the applied state (unk1193), restyles both
 * label sets -- dim: semi-trans + RGB 0x20; undim: opaque + RGB 0x80 (both OR
 * the tpage blend bits) -- then syncs 1193.  Always batch-AddPrims the normal
 * set (polysTexts) + the highlight set (polysCursors) to ot[4].
 * NB retail reads g_Menu from $a0 register residue at entry; the port uses
 * g_Menu directly. */
void func_801CEC40(void) {
    MenuSelectionMenu* pSel;
    s32 i;

    if (!g_Menu->pManager->shouldRenderSelectionMenu) {
        return;
    }
    pSel = g_Menu->pSelectionMenu;

    if (pSel->unk1192 != pSel->unk1193) {
        u8 rgb = pSel->unk1192 ? 0x20 : 0x80;
        s32 semi = pSel->unk1192 ? 1 : 0;

        for (i = 0; i < pSel->numTexts; i++) {
            POLY_FT4* p = &pSel->polysTexts[pSel->textsRenderCtx + i * 2];

            SetSemiTrans(p, semi);
            SetShadeTex(p, 0);
            p->tpage |= 0x20;
            p->r0 = rgb;
            p->g0 = rgb;
            p->b0 = rgb;
        }
        for (i = 0; i < pSel->numCursors; i++) {
            POLY_FT4* p = &pSel->polysCursors[pSel->cursorsRenderCtx + i * 2];

            SetSemiTrans(p, semi);
            SetShadeTex(p, 0);
            p->tpage |= 0x20;
            p->r0 = rgb;
            p->g0 = rgb;
            p->b0 = rgb;
        }
        pSel->unk1193 = pSel->unk1192;
    }

    func_801CE2B4(pSel->numTexts, (u8*)pSel + 0x8C0, pSel->textsRenderCtx);
    func_801CE2B4(pSel->numCursors, (u8*)pSel, pSel->cursorsRenderCtx);
}
#endif

void func_801CF308(void) {
    void* pMenu = g_Menu;
    void* pManager = *(void**)((u8*)pMenu + 0x33C);
    if (*(u8*)((u8*)pManager + 0xA) != 0) {
        void* pData = *(void**)((u8*)pMenu + 0x354);
        func_801CE2B4(*(s32*)((u8*)pData + 0x1404), (u8*)pData + 0x500, *(u8*)((u8*)pData + 0x1409));
        pMenu = g_Menu;
        pData = *(void**)((u8*)pMenu + 0x354);
        func_801CE2B4(*(s32*)((u8*)pData + 0x1400), (u8*)pData, *(u8*)((u8*)pData + 0x1408));
    }
}

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CF37C);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CF5E4);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CF8D8);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CFB48);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CFF64);

extern void func_801CF37C(void);
extern void func_801CF5E4(s32, s32, s32);
extern void func_801CFB48(void);
extern void func_801CF8D8(void);

void func_801D01D0(void) {
    void* pMenu;
    func_801CF37C();
    func_801CF5E4(0, 0, 0x10);
    func_801CF5E4(0x10, 0x10, 0x20);
    func_801CFB48();
    func_801CF8D8();
    func_801CFF64();
    pMenu = g_Menu;
    {
        s32 val = *(s32*)((u8*)pMenu + 0x4D0) + 1;
        *(s32*)((u8*)pMenu + 0x4D0) = val;
        if (val == 0xF) {
            *(s32*)((u8*)pMenu + 0x4D0) = 0;
            {
                s32 val2 = *(s32*)((u8*)pMenu + 0x4CC) + 1;
                *(s32*)((u8*)pMenu + 0x4CC) = val2;
                if (val2 == 6) {
                    *(s32*)((u8*)pMenu + 0x4CC) = 0;
                }
            }
        }
    }
    {
        void* pMenu2 = g_Menu;
        if (*(u8*)((u8*)pMenu2 + 0x4D9) == 0) {
            s32 val = *(s32*)((u8*)pMenu2 + 0x4D4) + 4;
            *(s32*)((u8*)pMenu2 + 0x4D4) = val;
            if (val >= 0x81) {
                *(u8*)((u8*)pMenu2 + 0x4D9) = 1;
                pMenu2 = g_Menu;
                *(s32*)((u8*)pMenu2 + 0x4D4) = 0x7C;
            }
        } else {
            s32 val = *(s32*)((u8*)pMenu2 + 0x4D4) - 4;
            if (val < 0) {
                *(s32*)((u8*)pMenu2 + 0x4D4) = val;
                *(u8*)((u8*)pMenu2 + 0x4D9) = 0;
                pMenu2 = g_Menu;
                *(s32*)((u8*)pMenu2 + 0x4D4) = 4;
            } else {
                *(s32*)((u8*)pMenu2 + 0x4D4) = val;
            }
        }
    }
}

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

s32 func_801D0E20(void) {
    s32 i;
    for (i = 6; i >= 0; i--) {}
    return i + 1;
}

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D0E38);

s32 func_801D0EBC(void) {
    s32 i;
    for (i = 4; i >= 0; i--) {}
    return i + 1;
}

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D0ED4);
#else
/* Nav N3a-A1b-2: draw the eight shared label strings whose visibility
 * latches live at MenuManager+0x38.  DCE60 arms category/target labels plus
 * slots 6/7 (the MP/max-MP labels); this renderer is reached through the
 * retail D11F0 draw-pass aggregator rather than through DCE60's call tree. */
void func_801D0ED4(void) {
    s32 i;

    for (i = 0; i < 8; i++) {
        if (g_Menu->pManager->unk38[i] != 0) {
            MenuString* string = &g_Menu->itemMenuStrings[i];

            func_801CE198(1, string->vertices, string->polys,
                          string->renderContext);
        }
    }
}
#endif

void func_801D0F54(void) {
    s32 i;
    u32 offset = 0x14E0;
    for (i = 0; i < 6; i++) {
        void* pMenu = g_Menu;
        void* pManager = *(void**)((u8*)pMenu + 0x33C);
        if (*(u8*)((u8*)pManager + 0x40 + i) != 0) {
            u8* pData = (u8*)pMenu + offset + 0x50;
            u8 val = *(u8*)((u8*)pMenu + i * 0x80 + 0x155D);
            func_801CE198(1, pData, val);
        }
        offset += 0x80;
    }
}

void func_801D0FD4(void) {
    void* pMenu = g_Menu;
    void* pManager = *(void**)((u8*)pMenu + 0x33C);
    if (*(u8*)((u8*)pManager + 0x4E) != 0) {
        u8 idx = *(u8*)((u8*)pMenu + 0x185D);
        u8* pOT = *(u8**)((u8*)pMenu + 0x1D4) + 0x80;
        u8* pPrim = (u8*)pMenu + 0x17E0 + idx * 0x28;
        AddPrim(pOT, pPrim);
    }
}

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D1030);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D10DC);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D1160);

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D11F0);
#else
/* Retail draw-pass aggregator, kept in its exact call order.  A1b-2 ports
 * only func_801D0ED4; the other nine existing PC stubs stay fail-visible
 * until their own screens reach them. */
void func_801D11F0(void) {
    func_801D0D90();
    func_801D0E20();
    func_801D0E38();
    func_801D0EBC();
    func_801D10DC();
    func_801D1160();
    func_801D0ED4();
    func_801D0F54();
    func_801D0FD4();
    func_801D1030();
}
#endif

void func_801D1258(void) {
    void* pMenu = g_Menu;
    u8* pOT = *(u8**)((u8*)pMenu + 0x1D4) + 0x90;
    s32 idx = *(s32*)((u8*)pMenu + 0x308);
    u8* pData = *(u8**)((u8*)pMenu + 0x348);
    AddPrim(pOT, pData + idx * 0x18 + 0x98);
    pMenu = g_Menu;
    pOT = *(u8**)((u8*)pMenu + 0x1D4) + 0x90;
    idx = *(s32*)((u8*)pMenu + 0x308);
    pData = *(u8**)((u8*)pMenu + 0x348);
    AddPrim(pOT, pData + idx * 0xC + 0x140);
}

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D12D4);
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D13F8);
#else
void func_801D12D4(MenuCharacter* panel, s32 drawFixedLabels) {
    if (!panel->unkBE7) {
        return;
    }

    AddPrim(&g_Menu->pGfxEnv->ot[4],
            &panel->polysPortraitSmall[panel->renderContext]);
    AddPrim(&g_Menu->pGfxEnv->ot[4],
            &panel->polys4B0[panel->renderContext]);
    func_801CE2B4(panel->descriptionStringsLength,
                  (u8*)panel->polysDescriptionStrings, panel->renderContext);
    func_801CE2B4(panel->levelStringLength,
                  (u8*)panel->polysLevelString, panel->renderContext);
    func_801CE2B4(panel->unkBE1, (u8*)panel->polys5F0,
                  panel->renderContext);
    func_801CE2B4(panel->hpStringLength, (u8*)panel->polysHpString,
                  panel->renderContext);
    func_801CE2B4(panel->maxHpStringLength,
                  (u8*)panel->polysMaxHpString, panel->renderContext);
    func_801CE2B4(panel->mpStringLength, (u8*)panel->polysMpString,
                  panel->renderContext);
    func_801CE2B4(panel->maxMpStringLength,
                  (u8*)panel->polysMaxMpString, panel->renderContext);
    if (drawFixedLabels & 0xFF) {
        func_801CE2B4(5, (u8*)panel->polys2D0,
                      panel->renderContext);
    }
}

void func_801D13F8(void) {
    s32 i;

    if (g_Menu->pManager->unk46) {
        i = 0;
        do {
            MenuCharacter* panel = g_Menu->currentCharacters[i];
            i++;
            func_801D12D4(panel, 1);
        } while (i < MAX_PARTY_MEMBERS);
    }
}
#endif

extern void func_801CE198(s32, void*, u8);

void func_801D1464(void) {
    void* pMenu = g_Menu;
    void* pManager = *(void**)((u8*)pMenu + 0x33C);
    if (*(u8*)((u8*)pManager + 0x49) != 0) {
        void* pData = *(void**)((u8*)pMenu + 0x43C);
        func_801CE198(1, (u8*)pData + 0x50, *(u8*)((u8*)pData + 0x70));
    }
}

void func_801D14B0(void) {
    void* pMenu = g_Menu;
    void* pManager = *(void**)((u8*)pMenu + 0x33C);
    if (*(u8*)((u8*)pManager + 0x53) != 0) {
        void* pData = *(void**)((u8*)pMenu + 0x440);
        func_801CE198(4, (u8*)pData + 0x140, *(u8*)((u8*)pData + 0x1C0));
    }
}

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D14FC);
#else
extern void func_801CE198(s32, SVECTOR*, POLY_FT4*, s32);

/* Items content draw pass.  N2b-1 activates the sixteen name/count pairs;
 * the three named description fields stay dormant until N2b-2 arms them. */
void func_801D14FC(void) {
    ItemMenuWork* work;
    s32 i;

    if (!g_Menu->pManager->unk48) {
        return;
    }
    work = MenuItemWork();
    for (i = 0; i < 16; i++) {
        if (work->rowVisible[i]) {
            MenuString* name = &work->itemNames[i];
            MenuString* count = &work->itemCounts[i];

            func_801CE198(1, name->vertices, name->polys,
                          name->renderContext);
            func_801CE198(1, count->vertices, count->polys,
                          count->renderContext);
        }
    }
    if (work->descriptionVisible) {
        MenuString* name = &work->selectedItemName;
        MenuString* count = &work->selectedItemCount;
        MenuString* description = &work->selectedItemDescription;

        func_801CE198(1, name->vertices, name->polys,
                      name->renderContext);
        func_801CE198(1, count->vertices, count->polys,
                      count->renderContext);
        func_801CE198(1, description->vertices, description->polys,
                      description->renderContext);
    }
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D1640);
#else
/* Nav N3a-A1b-1: the Abilities content draw pass -- the analogue of the
 * Items pass above, gated on pManager->unk4A[0], the content-ready latch
 * func_801DC3D8's epilogue sets.  (This latch connection is why it escaped
 * the A1b-1 callee walk: it is a renderer armed by the builder, not a
 * callee of it.)  Rows whose rowFlags byte is nonzero draw BOTH their name
 * and value strings; absent rows skip both.  The description block
 * (strings[28..31]) is gated on work->unk1092, func_801DCE60's flag, and
 * stays dark while DCE60 is stubbed.  The 33rd string draws unconditionally
 * under the latch -- it is unk1000String's observed reader. */
void func_801D1640(void) {
    AbilityMenuWork* work;
    s32 i;

    if (g_Menu->pManager->unk4A[0] == 0) {
        return;
    }
    work = (AbilityMenuWork*)(uintptr_t)g_Menu->unk42C[1];
    for (i = 0; i < 0xE; i++) {
        /* Retail reads 0x1084+i for i in 0..0xD, so rows 12/13 read the
         * unk1090[0..1] spill -- same indexing as the builder. */
        if (work->rowFlags[i] != 0) {
            MenuString* name = &work->strings[i];
            MenuString* value = &work->strings[14 + i];

            func_801CE198(1, name->vertices, name->polys,
                          name->renderContext);
            func_801CE198(1, value->vertices, value->polys,
                          value->renderContext);
        }
    }
    if (work->unk1092 != 0) {
        for (i = 28; i < 32; i++) {
            func_801CE198(1, work->strings[i].vertices, work->strings[i].polys,
                          work->strings[i].renderContext);
        }
    }
    func_801CE198(1, work->unk1000String.vertices, work->unk1000String.polys,
                  work->unk1000String.renderContext);
}
#endif

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D17C4);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D1914);

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D1AAC);
#else
/* Draw the two animated arrow cursors after their per-frame builders have
 * selected a frame and positioned their primitive vertices. */
void func_801D1AAC(void) {
    s32 i;

    for (i = 0; i < MENU_MAX_NUM_ARROW_CURSORS; i++) {
        if (g_Menu->pManager->shouldRenderArrowCursor[i]) {
            MenuArrowCursor* cursor = g_Menu->arrowCursors[i];

            func_801CE198(1, cursor->vertices, cursor->polys,
                          cursor->renderContext);
        }
    }
}
#endif

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

extern void func_801D3B00(void);
extern void func_801D11F0(void);
extern void func_801CE3C8(void);
extern void func_801CE338(void);
extern void func_801D02D8(void);
extern void func_801D01D0(void);
extern void func_801CEC40(void);
extern void func_801CF308(void);
extern void func_801D0C78(void);

void func_801D1BE8(void) {
    func_801D3B00();
    func_801D11F0();
    func_801CE3C8();
    func_801CE338();
    func_801D02D8();
    func_801D01D0();
    func_801CEC40();
    func_801CF308();
    func_801D0C78();
}

void func_801D1C48(void) {
    func_801D3B00();
    func_801CE3C8();
    func_801D11F0();
    func_801CE338();
    func_801D02D8();
    func_801D01D0();
    func_801D0C78();
    func_801CF308();
}

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

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D1D40);
#else
/* Advance the shared open/close transform, then publish it to the GTE.  Items
 * windows request this caller-owned transform (unk714 == 1), so leaving this
 * as an overlay stub arms their draw flags but projects their geometry with a
 * stale matrix. */
void func_801D1D40(void) {
    switch (g_Menu->transitionEffectState) {
    case 1:
        g_Menu->rotation.vx = (s16)(g_Menu->rotation.vx + 0x7C);
        g_Menu->translation.vz -= 0x30;
        if (g_Menu->translation.vz < 0x200) {
            g_Menu->translation.vz = 0x200;
            g_Menu->rotation.vx = 0;
            g_Menu->rotation.vy = 0;
            g_Menu->rotation.vz = 0;
            g_Menu->transitionEffectState = 0;
        }
        break;
    case 2:
        g_Menu->rotation.vy = (s16)(g_Menu->rotation.vy - 0x60);
        g_Menu->translation.vz += 0x40;
        if (g_Menu->translation.vz >= 0xE00) {
            g_Menu->transitionEffectState = 0;
        }
        break;
    case 3:
        g_Menu->translation.vz = 0x800;
        g_Menu->rotation.vx = 0;
        g_Menu->rotation.vy = 0;
        g_Menu->rotation.vz = 0;
        g_Menu->transitionEffectState = 1;
        break;
    case 4:
        g_Menu->translation.vz = 0x200;
        g_Menu->rotation.vx = 0;
        g_Menu->rotation.vy = 0;
        g_Menu->rotation.vz = 0;
        g_Menu->transitionEffectState = 2;
        break;
    default:
        break;
    }

    RotMatrix(&g_Menu->rotation, &g_Menu->matTransform);
    TransMatrix(&g_Menu->matTransform, &g_Menu->translation);
    SetRotMatrix(&g_Menu->matTransform);
    SetTransMatrix(&g_Menu->matTransform);
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D1E80);
#else
void func_801D1E80(void) {
    g_Menu->transitionEffectState = MENU_OPEN_ANIMATION_START;
    func_801C8574(0x5B);
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D1EB0);
#else
void func_801D1EB0(void) {
    g_Menu->transitionEffectState = MENU_CLOSE_ANIMATION_START;
    func_801C8574(0x5C);
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D1EE0);
#else
extern u8 D_801E9A00[];   /* per-option cursor x table (u32 each, migrated) */
extern u8 D_801E9A2C[];   /* per-option cursor y table */

/* Nav N1: build the pointer cursor at option `selected` -- the pointer glyph
 * (atlas 0x108) into unk348, and (when buildBar) the highlight bar: a G4 quad
 * plus its two LINE_F3 outlines, width unk15B (set by the coordinator), at
 * the option's table position.  Arms pManager->unk3 (the bar flag). */
void func_801D1EE0(s32 selected, s32 buildBar) {
    u16 x = *(u16*)(D_801E9A00 + selected * 4);
    u16 y = *(u16*)(D_801E9A2C + selected * 4);
    MenuUnk1* pCur = g_Menu->unk348;
    s32 rc = g_Menu->renderContext;

    func_8002675C(g_Menu->unk2DC, 0x108, pCur, rc,
                  *(s32*)(D_801E9A00 + selected * 4),
                  *(s32*)(D_801E9A2C + selected * 4), 0x1000);
    pCur->cursorRenderContext = (u8)rc;

    if (buildBar & 0xFF) {
        POLY_G4* pBar = &pCur->polyG4s[rc];
        LINE_F3* pLine1 = &pCur->lines1[rc];
        LINE_F3* pLine2 = &pCur->lines2[rc];
        s32 w = pCur->unk15B;

        pBar->x0 = (s16)(x + 0x14);
        pBar->y0 = (s16)(y - 0x24);
        pBar->x1 = (s16)(x + w + 0x14);
        pBar->y1 = (s16)(y - 0x24);
        pBar->x2 = (s16)(x + 0x14);
        pBar->y2 = (s16)(y - 0x14);
        pBar->x3 = (s16)(x + w + 0x14);
        pBar->y3 = (s16)(y - 0x14);

        pLine1->x0 = (s16)(x + 0x14);
        pLine1->y0 = (s16)(y - 0x24);
        pLine1->x1 = (s16)(x + w + 0x14);
        pLine1->y1 = (s16)(y - 0x24);
        pLine1->x2 = (s16)(x + w + 0x14);
        pLine1->y2 = (s16)(y - 0x14);

        pLine2->x0 = (s16)(x + 0x14);
        pLine2->y0 = (s16)(y - 0x24);
        pLine2->x1 = (s16)(x + 0x14);
        pLine2->y1 = (s16)(y - 0x14);
        pLine2->x2 = (s16)(x + w + 0x14);
        pLine2->y2 = (s16)(y - 0x14);

        pCur->unk159 = (u8)rc;
        g_Menu->pManager->unk3 = 1;
    }
}
#endif

void func_801D22C4(void) {
    void* pMenu = g_Menu;
    void* pManager = *(void**)((u8*)pMenu + 0x33C);
    *(u8*)((u8*)pManager + 4) = 0;
    pMenu = g_Menu;
    pManager = *(void**)((u8*)pMenu + 0x33C);
    *(u8*)((u8*)pManager + 3) = 0;
}

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D22F4);
#else
extern u8 D_801E9A58[];
extern u8 D_801E9A68[];

/* Configure the shared pointer-cursor bank.  Items uses mode 2, which rebuilds
 * all four cursor sprites without arming their live-position flags. */
void func_801D22F4(s32 mode) {
    s32 i;
    s32 offset;
    u8 m = (u8)mode;

    g_Menu->pManager->shouldRenderPointerCursors = 0;
    if (m == 1) {
        return;
    }
    if (m == 0) {
        g_Menu->pManager->shouldRenderPointerCursors = 1;
        g_Menu->pCursors->unk144[0] = 1;
        g_Menu->pCursors->unk144[1] = 1;
    }
    if (m == 0 || m == 2) {
        offset = 0;
        for (i = 0; i < MENU_MAX_NUM_CURSORS; i++, offset += 0x50) {
            func_8002675C(g_Menu->unk2DC, 0x108,
                          (u8*)g_Menu->pCursors + offset,
                          g_Menu->renderContext,
                          *(s32*)(D_801E9A58 + i * 4),
                          *(s32*)(D_801E9A68 + i * 4), 0x800);
            g_Menu->pCursors->renderContexts[i] = (u8)g_Menu->renderContext;
        }
    } else if (m == 3) {
        func_8002675C(g_Menu->unk2DC, 0x108, g_Menu->pCursors,
                      g_Menu->renderContext, 0, 0, 0x800);
        g_Menu->pCursors->renderContexts[0] = (u8)g_Menu->renderContext;
    }
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D2484);
#else
void func_801D2484(void) {
    g_Menu->pManager->shouldRenderPointerCursors = 0;
}
#endif

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D249C);

void func_801D25E4(void) {
    void* pManager;
    s32 i;
    pManager = *(void**)((u8*)g_Menu + 0x33C);
    for (i = 0; i < 6; i++) {
        *(u8*)((u8*)pManager + i + 0x14) = 0;
    }
}

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D261C);

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D28A8);
#else
extern void func_801D5BA4(s32 x, s32 y);

/* Arc A content: the open-settle -- build WINDOW 0 (the GOLD window, a 96x16
 * bar at 0xD4,0xB2; the same geometry path as window 1, which also arms its
 * shouldRenderWindow[0]) + the gold digit quads. */
void func_801D28A8(void) {
    func_801D397C(0, 0xD4, 0xB2, 0x60, 0x10, 0, 0, 4, 0);
    func_801D5BA4(0xD8, 0xB6);
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D28FC);
#else
extern void func_801D397C(u8 windowIndex, s32 x, s32 y, s32 w, s32 h,
                          u8 directParams, u8 unk714, s32 zIndex, u8 hasScrollBar);
extern void func_801D5CF8(s32 x, s32 y);

/* Arc A verts: post-setup for the main menu -- build window 1's geometry
 * (rect 0xCC,0xC6 80x16, zIndex 4) + the icon strip, and arm unk5[1]. */
void func_801D28FC(void) {
    func_801D397C(1, 0xCC, 0xC6, 0x50, 0x10, 0, 0, 4, 0);
    func_801D5CF8(0xD0, 0xCA);
    g_Menu->pManager->unk5[1] = 1;
}
#endif

extern void func_801D5CF8(s32, s32);

void func_801D2968(void) {
    void* pMenu = g_Menu;
    void* pManager = *(void**)((u8*)pMenu + 0x33C);
    if (*(u8*)((u8*)pManager + 6) != 0) {
        func_801D5CF8(0xD0, 0xCA);
    }
}

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D29A8);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D2D38);
#else
extern void func_801E8018(s32 count, MenuString* strings, u8* descriptorIds);
extern void func_801E53CC(u8 windowIndex);            /* frame-primitive init (below) */
/* NB retail passes charId in $a1 as register residue from the caller's lbu of
 * currentCharacterIDs[i] -- the port passes it explicitly. */
extern void func_801D5A50(u8 slot, u8 charId);        /* portrait panel rebuild (below) */
extern void func_801D28A8(void);                      /* stub */
extern u8 D_801EA528[];
extern u8 D_801EA19C[];
extern void func_801E8474(s32, void*);                /* content build (stub for now) */
extern void func_801E8DA8(s32, s32);                  /* portrait build (stub for now) */
extern void func_801D28FC(void);                      /* post-setup (stub for now) */

/* Arc A: the menu open/close animation.  Slides the three portrait frames
 * between off-screen and their final positions (anim slots 0..2), drawing each
 * frame via func_801C7BF4, then arms the draw guards: open sets
 * shouldRenderWindow[1] (the main window) + unk5[1]; close clears the window
 * flags.  THIS is what makes func_801D0C78 actually draw the window. */
void func_801D29A8(u8 open, u8 noSettle) {
    s32 i;

    if (open) {
        func_801E8018(8, g_Menu->unk6E0, D_801EA528);
        func_801C81E0(0x100, 0x86, 0x60, 0x06, 8, 0);
        func_801C81E0(0x108, 0x3E, 0x68, 0x3E, 8, 1);
        func_801C81E0(0x110, -0xA, 0x70, 0x76, 8, 2);
    } else {
        func_801E8044(8, (u8*)g_Menu->pManager + 0xC);
        func_801C81E0(0x60, 0x06, 0x100, 0x86, 8, 0);
        func_801C81E0(0x68, 0x3E, 0x108, 0x3E, 8, 1);
        func_801C81E0(0x70, 0x76, 0x110, -0xA, 8, 2);
    }

    while (MENU_OPEN_ANIM(0)->done == 0 && MENU_OPEN_ANIM(1)->done == 0 &&
           MENU_OPEN_ANIM(2)->done == 0) {
        for (i = 0; i < 3; i++) {
            if (g_Menu->pManager->currentCharacterIDs[i] != 0xFF) {
                func_801D5A50((u8)i, g_Menu->pManager->currentCharacterIDs[i]);
            }
        }
        func_801C7BF4();
        for (i = 0; i < 3; i++) {
            if (g_Menu->pManager->currentCharacterIDs[i] != 0xFF) {
                func_801C8324((u8)i);
            }
        }
    }

    if (open) {
        /* Settle the slots exactly on their final positions. */
        MENU_OPEN_ANIM(0)->curX = 0x60;
        MENU_OPEN_ANIM(0)->curY = 0x06;
        MENU_OPEN_ANIM(1)->curX = 0x68;
        MENU_OPEN_ANIM(1)->curY = 0x3E;
        MENU_OPEN_ANIM(2)->curX = 0x70;
        MENU_OPEN_ANIM(2)->curY = 0x76;
        MENU_OPEN_ANIM(0)->accX = 0;
        MENU_OPEN_ANIM(0)->accY = 0;
        MENU_OPEN_ANIM(1)->accX = 0;
        MENU_OPEN_ANIM(1)->accY = 0;
        MENU_OPEN_ANIM(2)->accX = 0;
        MENU_OPEN_ANIM(2)->accY = 0;
        for (i = 0; i < 3; i++) {
            if (g_Menu->pManager->currentCharacterIDs[i] != 0xFF) {
                func_801D5A50((u8)i, g_Menu->pManager->currentCharacterIDs[i]);
            }
        }
        func_801C8574(0x5D);
        if (noSettle == 0) {
            func_801D28A8();
        }
        g_Menu->pManager->shouldRenderWindow[1] = 1;
        g_Menu->pManager->unk5[1] = 1;
    } else {
        g_Menu->pManager->unk0[2] = 0;
        g_Menu->pManager->unk0[1] = 0;
        g_Menu->pManager->unk0[0] = 0;
        if (noSettle == 0) {
            g_Menu->pManager->shouldRenderWindow[0] = 0;
            g_Menu->pManager->unk5[0] = 0;
        }
    }
    func_801C7BF4();
}

/* Arc A: the main-menu render setup.  Allocates + initializes the two menu
 * windows (frame primitives via func_801E53CC), builds the portrait/content
 * pieces, then runs the open animation (which arms the window draw guard). */
void func_801D2D38(void) {
    s32 i;

    if (D_80059460 == 0) {
        for (i = 0; i < 2; i++) {
            g_Menu->windows[i] = HeapAlloc(sizeof(MenuWindow), 0);
            bzero(g_Menu->windows[i], sizeof(MenuWindow));
            g_Menu->windowParameters[i] = HeapAlloc(sizeof(MenuWindowParameters), 0);
            bzero(g_Menu->windowParameters[i], sizeof(MenuWindowParameters));
            func_801E53CC((u8)i);
        }
        func_801C8574(0x5E);
    }

    for (i = 0; i < 3; i++) {
        u8 charId = g_Menu->pManager->currentCharacterIDs[i];

        if (charId != 0xFF) {
            u8 gearArg;

            func_801E8DA8(charId, (i * 2) & 0xFE);
            if (g_GameState.characters[charId].gearId != 0xFF) {
                gearArg = (u8)(g_GameState.characters[charId].gearId + 0xB);
            } else {
                gearArg = 0xFF;
            }
            func_801E8DA8(gearArg, 6 + i * 2);
        }
    }

    func_801E8474(8, D_801EA19C);
    func_801D29A8(1, 0);
    func_801D28FC();
}
#endif

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D2EC0);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D2F4C);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D32B4);

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D3344);
#else
void func_801D3344(s32 x, s32 y, s32 width) {
    if (g_Menu->pManager->scrollHandleActive == 0) {
        g_Menu->pScrollHandle = HeapAlloc(sizeof(MenuScrollBarHandle), 0);
        bzero(g_Menu->pScrollHandle, sizeof(MenuScrollBarHandle));
    }
    func_8002675C(g_Menu->unk2DC, 0x107, g_Menu->pScrollHandle,
                  g_Menu->renderContext, x, y, 0x1000);
    func_801C851C(g_Menu->pScrollHandle->vertices,
                  x & 0xFFFF, y & 0xFFFF, 8, width & 0xFFFF);
    g_Menu->pScrollHandle->renderContext = (u8)g_Menu->renderContext;
    g_Menu->pManager->scrollHandleActive = 1;
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D3444);
#else
void func_801D3444(void) {
    g_Menu->pManager->scrollHandleActive = 0;
    HeapFree(g_Menu->pScrollHandle);
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D3488);
#else

extern s32 D_801EA164[];
extern s32 D_801EA16C[];

/* Nav N3a-A1a: build the shared four-quad panel set into SystemMenu.unk440.
 * Allocated once (guarded by MenuManager.unk5C[0xB], retail +0x67) and reused
 * by all four screens; `bank` selects the sprite bank via D_801EA16C, and the
 * category picks which readiness byte gates the build.  Retail reads the quad
 * geometry back out of polys[i*2 + renderContext] -- the double-buffered pair
 * selection -- and derives each vertex group's w/h from x1-x0 / y3-y0. */
void func_801D3488(s32 bank, u8 category) {
    MenuUnk440Work* work;
    s32 i;
    u8 gate;

    if ((category & 0xFF) != 0) {
        gate = g_Menu->unk33B;
    } else {
        gate = g_Menu->unk32B;
    }
    if (gate == 1) {
        return;
    }

    if (g_Menu->pManager->unk5C[0xB] == 0) {
        work = HeapAlloc(sizeof(MenuUnk440Work), 0);
        *(u32*)&g_Menu->unk440[0] = (u32)(uintptr_t)work;
        bzero(work, sizeof(MenuUnk440Work));
        g_Menu->pManager->unk5C[0xB] = 1;
    }

    work = (MenuUnk440Work*)(uintptr_t)*(u32*)&g_Menu->unk440[0];
    for (i = 0; i < 2; i++) {
        func_8002675C(g_Menu->unk2DC, 0x164 + i, &work->polys[i * 4],
                      g_Menu->renderContext, D_801EA164[i],
                      D_801EA16C[bank & 0xFF], 0x1000);
    }
    for (i = 0; i < 4; i++) {
        POLY_FT4* pPoly = &work->polys[i * 2 + g_Menu->renderContext];

        func_801C851C(&work->vertices[i * 4], pPoly->x0, pPoly->y0,
                      (u16)(pPoly->x1 - pPoly->x0),
                      (u16)(pPoly->y3 - pPoly->y0));
    }
    work->renderContext = (u8)g_Menu->renderContext;
    g_Menu->pManager->unk52[1] = 1;
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D3674);
#else
void func_801D3674(void) {
    /* manager+0x67 is the owner flag; +0x53 is its render guard. */
    if (g_Menu->pManager->unk5C[0xB] != 0) {
        g_Menu->pManager->unk52[1] = 0;
        g_Menu->pManager->unk5C[0xB] = 0;
        HeapFree(MenuUnk440Pointer());
    }
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D36E0);
#else
extern u16 g_SystemPalette1;
extern u16 g_SystemPalette2;
extern u8 D_801EA17C[];    /* per-style base x (lw, stride 4) */
extern u8 D_801EA18C[];    /* per-style y (lhu, stride 4) */
extern u8 D_801EA578[];    /* variant-0 u (lw, stride 4) */
extern u8 D_801EA584[];    /* variant-1 u (lw, stride 4) */
extern u8 D_801EA5C4[];    /* variant-0 v (lbu, stride 4) */
extern u8 D_801EA5D0[];    /* variant-1 v (lbu, stride 4) */
extern void func_801E920C(POLY_FT4* p, s32 x, s32 y, s32 u, s32 v, s32 w, s32 h);
extern void func_801E927C(POLY_FT4* p);

/* Nav N3a-A1b-1: sprite/tpage setup for one Abilities row string.  Resets the
 * row's double-buffered poly, gives it the shared 0x180 tpage, picks the CLUT
 * from a parity bit, then sizes the quad and its vertex group from the
 * per-style / per-slot tables.
 *
 * Two variants selected by `variant`:
 *   0 -> width 0x48, parity from MenuManager.currentCharacterIDs[slot] bit 0,
 *        u/v from D_801EA578 / D_801EA5C4, x bias -0x24
 *   1 -> width 0x60, parity from (characters[charId].gearId + 0xB) bit 0,
 *        u/v from D_801EA584 / D_801EA5D0, x bias -0x30
 * The charId->GameCharacter index uses retail's *0xA4 strength-reduced chain
 * (x*5 <<3 +x <<2); expressed here as the natural struct index. */
void func_801D36E0(MenuString* pStr, s32 slot, s32 variant, s32 style) {
    POLY_FT4* pPoly;
    s32 rc = g_Menu->renderContext;
    s32 sl = slot & 0xFF;
    s32 st = style & 0xFF;
    s32 width;
    s32 x;
    s32 u;
    s32 v;
    u8 parity;

    func_801E927C(&pStr->polys[rc]);

    pPoly = &pStr->polys[rc];
    pPoly->tpage = GetTPage(0, 0, 0x180, 0);

    if ((variant & 0xFF) != 0) {
        u8 charId = g_Menu->pManager->currentCharacterIDs[sl];

        width = 0x60;
        parity = (u8)((g_GameState.characters[charId].gearId + 0xB) & 1);
        pPoly = &pStr->polys[g_Menu->renderContext];
        pPoly->clut = parity ? g_SystemPalette2 : g_SystemPalette1;
        u = (*(s32*)(D_801EA584 + sl * 4) << 2) & 0xFC;
        v = D_801EA5D0[sl * 4];
        x = (u16)(*(s32*)(D_801EA17C + st * 4) - 0x30);
    } else {
        width = 0x48;
        parity = (u8)(g_Menu->pManager->currentCharacterIDs[sl] & 1);
        pPoly = &pStr->polys[g_Menu->renderContext];
        pPoly->clut = parity ? g_SystemPalette2 : g_SystemPalette1;
        u = (*(s32*)(D_801EA578 + sl * 4) << 2) & 0xFC;
        v = D_801EA5C4[sl * 4];
        x = (u16)(*(s32*)(D_801EA17C + st * 4) - 0x24);
    }

    func_801E920C(&pStr->polys[g_Menu->renderContext], x,
                  *(u16*)(D_801EA18C + st * 4), u, v, width, 0xD);
    func_801C851C(pStr->vertices, x, *(u16*)(D_801EA18C + st * 4), width, 0xD);
    pStr->renderContext = (u8)g_Menu->renderContext;
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D397C);
#else
extern void func_801D4D1C(u8 windowIndex, s32 x, s32 y, s32 w, s32 h,
                          u8 unk714, s32 zIndex, u8 hasScrollBar);

/* Arc A verts: (re)create window `windowIndex` -- windows 2+ get allocated on
 * demand (0/1 come from func_801D2D38).  directParams != 0 stores the raw rect
 * into windowParameters (deferred build); 0 runs the geometry build now
 * (func_801D4D1C). */
void func_801D397C(u8 windowIndex, s32 x, s32 y, s32 w, s32 h,
                   u8 directParams, u8 unk714, s32 zIndex, u8 hasScrollBar) {
    MenuWindowParameters* pParams;

    if (windowIndex >= 2) {
        g_Menu->windows[windowIndex] = HeapAlloc(sizeof(MenuWindow), 0);
        bzero(g_Menu->windows[windowIndex], sizeof(MenuWindow));
        g_Menu->windowParameters[windowIndex] =
            HeapAlloc(sizeof(MenuWindowParameters), 0);
        bzero(g_Menu->windowParameters[windowIndex],
              sizeof(MenuWindowParameters));
        func_801E53CC(windowIndex);
    }

    pParams = g_Menu->windowParameters[windowIndex];
    if (directParams != 0) {
        ((u8*)pParams)[0x10] = windowIndex;
        ((u8*)pParams)[0x11] = 0;
        *(s16*)((u8*)pParams + 0x0) = (s16)x;
        *(s16*)((u8*)pParams + 0x2) = (s16)y;
        *(s16*)((u8*)pParams + 0x4) = (s16)w;
        *(s16*)((u8*)pParams + 0x6) = (s16)h;
        *(s16*)((u8*)pParams + 0x8) = 0;
        *(s16*)((u8*)pParams + 0xA) = 0;
        g_Menu->pManager->unk27[windowIndex] = 1;
        ((u8*)pParams)[0x12] = unk714;
        *(s32*)((u8*)pParams + 0xC) = zIndex;
    } else {
        func_801D4D1C(windowIndex, x & 0xFFFF, y & 0xFFFF, w & 0xFFFF,
                      h, unk714, zIndex, hasScrollBar);
    }
}
#endif

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D3B00);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D3C4C);

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D3DB0);
#else
extern s32 func_8002675C(u8* table, s32 index, void* polys, s32 renderCtx,
                         s32 x, s32 y, s32 scale);
extern void func_801C851C(SVECTOR* verts, s32 x, s32 y, s32 w, s32 h);
extern void func_801E91C4(POLY_FT4* p);

/* Arc A verts: the four window-corner pieces.  Builds the corner POLY_FT4
 * pairs from the atlas (TL/TR/BL/BR border-corner texture ids) and writes the
 * corner vert quads (16x16, extents mirrored via sign). */
void func_801D3DB0(u8 windowIndex, s32 x, s32 y, s32 w, s32 h) {
    MenuWindow* pWindow = g_Menu->windows[windowIndex];

    pWindow->unk710 = 0;
    pWindow->unk710 += func_8002675C(g_Menu->unk2DC, MENU_TEX_WINDOW_BORDER_TOP_LEFT,
                                     &pWindow->polysWindowBorderCorners[pWindow->unk710 * 2],
                                     g_Menu->renderContext, 0, 0, 0x1000);
    pWindow->unk710 += func_8002675C(g_Menu->unk2DC, MENU_TEX_WINDOW_BORDER_TOP_RIGHT,
                                     &pWindow->polysWindowBorderCorners[pWindow->unk710 * 2],
                                     g_Menu->renderContext, 0, 0, 0x1000);
    pWindow->unk710 += func_8002675C(g_Menu->unk2DC, MENU_TEX_WINDOW_BORDER_BOTTOM_LEFT,
                                     &pWindow->polysWindowBorderCorners[pWindow->unk710 * 2],
                                     g_Menu->renderContext, 0, 0, 0x1000);
    pWindow->unk710 += func_8002675C(g_Menu->unk2DC, MENU_TEX_WINDOW_BORDER_BOTTOM_RIGHT,
                                     &pWindow->polysWindowBorderCorners[pWindow->unk710 * 2],
                                     g_Menu->renderContext, 0, 0, 0x1000);

    func_801C851C(&pWindow->vertsWindowBorderCorners[0],  (u16)(x - 8),     (u16)(y + 8),  0x10, -0x10);
    func_801C851C(&pWindow->vertsWindowBorderCorners[4],  (u16)(x + w + 8), (u16)(y + 8), -0x10, -0x10);
    func_801C851C(&pWindow->vertsWindowBorderCorners[8],  (u16)(x - 8),     (u16)(y + h - 8), 0x10, 0x10);
    func_801C851C(&pWindow->vertsWindowBorderCorners[12], (u16)(x + w + 8), (u16)(y + h - 8), -0x10, 0x10);

    {
        s32 i;
        for (i = 0; i < 4; i++) {
            func_801E91C4(&pWindow->polysWindowBorderCorners[i * 2 + g_Menu->renderContext]);
        }
    }
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D3FF8);
#else
/* Arc A verts: the TOP border -- two halves of (w-16)/2 each, 16 tall, at
 * y-8, with the 8px corner caps on either side.  UV strip u=0..7, v=0x84..0x94. */
void func_801D3FF8(u8 windowIndex, s32 x, s32 y, s32 w) {
    MenuWindow* pWindow = g_Menu->windows[windowIndex];
    s32 rc = g_Menu->renderContext;
    POLY_FT4* p1 = &pWindow->polysWindowBorderTop[rc];
    POLY_FT4* p2 = &pWindow->polysWindowBorderTop[2 + rc];
    s32 halfW;
    s32 i;

    p1->u0 = 0;    p1->v0 = 0x84;
    p1->u1 = 7;    p1->v1 = 0x84;
    p1->u2 = 0;    p1->v2 = 0x94;
    p1->u3 = 7;    p1->v3 = 0x94;
    p2->u0 = 0;    p2->v0 = 0x84;
    p2->u1 = 7;    p2->v1 = 0x84;
    p2->u2 = 0;    p2->v2 = 0x94;
    p2->u3 = 7;    p2->v3 = 0x94;

    halfW = ((w & 0xFFFF) - 0x10) / 2;
    func_801C851C(pWindow->vertsWindowBorderTop1, (u16)(x + 8), (u16)(y - 8),
                  (u16)halfW, 0x10);
    func_801C851C(pWindow->vertsWindowBorderTop2, (u16)(x + halfW + 8),
                  (u16)(y - 8), (u16)halfW, 0x10);

    for (i = 0; i < 2; i++) {
        func_801E91C4(&pWindow->polysWindowBorderTop[i * 2 + rc]);
    }
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D433C);
#else
/* Arc A verts: the BOTTOM border -- two halves at y+h-8.  UV u=8..0xF,
 * v=0x84..0x94. */
void func_801D433C(u8 windowIndex, s32 x, s32 y, s32 w, s32 h) {
    MenuWindow* pWindow = g_Menu->windows[windowIndex];
    s32 rc = g_Menu->renderContext;
    POLY_FT4* p1 = &pWindow->polysWindowBorderBottom[rc];
    POLY_FT4* p2 = &pWindow->polysWindowBorderBottom[2 + rc];
    s32 halfW;
    s32 i;

    p1->u0 = 0x8;  p1->v0 = 0x84;
    p1->u1 = 0xF;  p1->v1 = 0x84;
    p1->u2 = 0x8;  p1->v2 = 0x94;
    p1->u3 = 0xF;  p1->v3 = 0x94;
    p2->u0 = 0x8;  p2->v0 = 0x84;
    p2->u1 = 0xF;  p2->v1 = 0x84;
    p2->u2 = 0x8;  p2->v2 = 0x94;
    p2->u3 = 0xF;  p2->v3 = 0x94;

    halfW = ((w & 0xFFFF) - 0x10) / 2;
    func_801C851C(pWindow->vertsWindowBorderBottom1, (u16)(x + 8),
                  (u16)(y + h - 8), (u16)halfW, 0x10);
    func_801C851C(pWindow->vertsWindowBorderBottom2, (u16)(x + halfW + 8),
                  (u16)(y + h - 8), (u16)halfW, 0x10);

    for (i = 0; i < 2; i++) {
        func_801E91C4(&pWindow->polysWindowBorderBottom[i * 2 + rc]);
    }
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D4688);
#else
/* Arc A verts: the LEFT border -- two vertical halves of (h-16)/2 each,
 * 16 wide, at x-8.  UV u=0x10..0x20, v=0x84..0x8B. */
void func_801D4688(u8 windowIndex, s32 x, s32 y, s32 h) {
    MenuWindow* pWindow = g_Menu->windows[windowIndex];
    s32 rc = g_Menu->renderContext;
    POLY_FT4* p1 = &pWindow->polysWindowBorderLeft[rc];
    POLY_FT4* p2 = &pWindow->polysWindowBorderLeft[2 + rc];
    s32 halfH;
    s32 i;

    p1->u0 = 0x10; p1->v0 = 0x84;
    p1->u1 = 0x20; p1->v1 = 0x84;
    p1->u2 = 0x10; p1->v2 = 0x8B;
    p1->u3 = 0x20; p1->v3 = 0x8B;
    p2->u0 = 0x10; p2->v0 = 0x84;
    p2->u1 = 0x20; p2->v1 = 0x84;
    p2->u2 = 0x10; p2->v2 = 0x8B;
    p2->u3 = 0x20; p2->v3 = 0x8B;

    halfH = ((h & 0xFFFF) - 0x10) / 2;
    func_801C851C(pWindow->vertsWindowBorderLeft1, (u16)(x - 8), (u16)(y + 8),
                  0x10, (u16)halfH);
    func_801C851C(pWindow->vertsWindowBorderLeft2, (u16)(x - 8),
                  (u16)(y + halfH + 8), 0x10, (u16)halfH);

    for (i = 0; i < 2; i++) {
        func_801E91C4(&pWindow->polysWindowBorderLeft[i * 2 + rc]);
    }
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D49D0);
#else
/* Arc A verts: the RIGHT border -- two vertical halves at x+w-8.
 * UV u=0x10..0x20, v=0x8C..0x93. */
void func_801D49D0(u8 windowIndex, s32 x, s32 y, s32 w, s32 h) {
    MenuWindow* pWindow = g_Menu->windows[windowIndex];
    s32 rc = g_Menu->renderContext;
    POLY_FT4* p1 = &pWindow->polysWindowBorderRight[rc];
    POLY_FT4* p2 = &pWindow->polysWindowBorderRight[2 + rc];
    s32 halfH;
    s32 i;

    p1->u0 = 0x10; p1->v0 = 0x8C;
    p1->u1 = 0x20; p1->v1 = 0x8C;
    p1->u2 = 0x10; p1->v2 = 0x93;
    p1->u3 = 0x20; p1->v3 = 0x93;
    p2->u0 = 0x10; p2->v0 = 0x8C;
    p2->u1 = 0x20; p2->v1 = 0x8C;
    p2->u2 = 0x10; p2->v2 = 0x93;
    p2->u3 = 0x20; p2->v3 = 0x93;

    halfH = ((h & 0xFFFF) - 0x10) / 2;
    func_801C851C(pWindow->vertsWindowBorderRight1, (u16)(x + w - 8),
                  (u16)(y + 8), 0x10, (u16)halfH);
    func_801C851C(pWindow->vertsWindowBorderRight2, (u16)(x + w - 8),
                  (u16)(y + halfH + 8), 0x10, (u16)halfH);

    for (i = 0; i < 2; i++) {
        func_801E91C4(&pWindow->polysWindowBorderRight[i * 2 + rc]);
    }
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D4D1C);
#else
extern void func_801D3C4C(u8, s32, s32, s32, s32);  /* scrollbar verts (stub) */

/* Arc A verts: build window `windowIndex`'s full frame geometry from its rect
 * -- the background quad, the four corner pieces, the four split borders, and
 * (optionally) the scrollbar -- then set the window props and ARM the draw
 * guard.  THIS is what turns the zero-size quads into a real window. */
void func_801D4D1C(u8 windowIndex, s32 x, s32 y, s32 w, s32 h,
                   u8 unk714, s32 zIndex, u8 hasScrollBar) {
    MenuWindow* pWindow = g_Menu->windows[windowIndex];

    x &= 0xFFFF;
    y &= 0xFFFF;
    w &= 0xFFFF;

    g_Menu->pManager->shouldRenderWindow[windowIndex] = 0;
    func_801C851C(pWindow->vertsBackground, x, y, w, h);
    func_801D3DB0(windowIndex, x, y, w, h);
    func_801D3FF8(windowIndex, x, y, w);
    func_801D433C(windowIndex, x, y, w, h);
    func_801D4688(windowIndex, x, y, h);
    func_801D49D0(windowIndex, x, y, w, h);
    if (hasScrollBar) {
        func_801D3C4C(windowIndex, x, y, w, h);
    }
    pWindow->hasScrollBar = hasScrollBar;
    pWindow->unk714 = unk714;
    pWindow->zIndex = zIndex;
    pWindow->renderContext = (u8)g_Menu->renderContext;
    g_Menu->pManager->shouldRenderWindow[windowIndex] = 1;
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D4EA0);
#else
/* Window destruction belongs to the dispatcher cleanup, not the Items screen. */
void func_801D4EA0(s32 windowIndex) {
    u8 index = (u8)windowIndex;
    g_Menu->pManager->shouldRenderWindow[index] = 0;
    g_Menu->pManager->unk27[index] = 0;
    HeapFree(g_Menu->windows[index]);
    HeapFree(g_Menu->windowParameters[index]);
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D4F2C);
#else
extern void func_801E927C(POLY_FT4* p);
extern void func_801E920C(POLY_FT4* p, s32 x, s32 y, s32 u, s32 v, s32 w, s32 h);
extern u16 g_SystemPalette1;
extern u16 g_SystemPalette2;
extern s32 D_801E9B58;
extern s32 D_801E9B5C;
extern u8 D_801EA578[];   /* per-slot portrait VRAM x table (also used by func_801E8DA8) */
extern u8 D_801EA5C4[];   /* per-slot portrait VRAM y table */

#define PORTRAIT_BUF(slot) ((u8*)(uintptr_t)*(u32*)&g_Menu->unk39C[(slot) * 4])
#define PORTRAIT_POLY(buf, base, idx) ((POLY_FT4*)((buf) + (base) + (idx) * sizeof(POLY_FT4)))

/* Arc A portraits: the frame + face quads for party slot `slot` at animated
 * (x,y).  The frame comes from the atlas (0x14B + slot -- the same ids whose
 * VRAM rects the resource-load filled); the face quad samples the portrait
 * VRAM directly (tpage x=0x180, clut by charId parity, rect from the
 * D_801EA578/D_801EA5C4 slot tables, 0x48x0xD). */
void func_801D4F2C(u8 slot, u8 charId, s32 x, s32 y) {
    u8* buf = PORTRAIT_BUF(slot);
    s32 rc = g_Menu->renderContext;
    POLY_FT4* pFace;

    func_8002675C(g_Menu->unk2DC, 0x14B + slot, buf, rc, x, y, 0x1000);
    pFace = PORTRAIT_POLY(buf, 0x50, rc);
    func_801E927C(pFace);
    pFace->tpage = GetTPage(0, 0, 0x180, 0);
    if (g_Menu->pManager->currentCharacterIDs[slot] & 1) {
        pFace->clut = g_SystemPalette2;
    } else {
        pFace->clut = g_SystemPalette1;
    }
    func_801E920C(pFace,
                  (u16)((u16)D_801E9B58 + x), (u16)((u16)D_801E9B5C + y),
                  (*(u32*)(D_801EA578 + slot * 4) << 2) & 0xFC,
                  *(u8*)(D_801EA5C4 + slot * 4), 0x48, 0xD);
    (void)charId;
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D50EC);
#else
extern u8 D_801EA34C[];   /* 20 status-icon atlas ids (u32 each; 0xFFFF = empty) */
extern u8 D_801E9A78[];   /* 20 per-icon x offsets (u32 each) */
extern u8 D_801E9AC8[];   /* 20 per-icon y offsets (u32 each) */

/* Arc A portraits: the status-icon row -- up to 20 icons at per-icon offsets,
 * built into the buf+0xA0 list (count at 0x1279). */
void func_801D50EC(u8 slot, s32 x, s32 y) {
    u8* buf = PORTRAIT_BUF(slot);
    s32 i;

    buf[0x1279] = 0;
    for (i = 0; i < 0x14; i++) {
        s32 id = *(s32*)(D_801EA34C + i * 4);

        if (id != 0xFFFF) {
            buf[0x1279] += (u8)func_8002675C(
                g_Menu->unk2DC, id, buf + 0xA0 + buf[0x1279] * 0x50,
                g_Menu->renderContext,
                x + *(s32*)(D_801E9A78 + i * 4),
                y + *(s32*)(D_801E9AC8 + i * 4), 0x1000);
        }
    }
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D51EC);
#else
extern void func_801C80B8(u32 value);
extern s32 D_801E9B28;
extern s32 D_801E9B2C;
extern s32 D_801E9B30;
extern s32 D_801E9B34;

/* Arc A portraits: the HP / maxHP digit quads (3 digits each, digits[6..8],
 * 0xFF = blank).  Lists buf+0xAF0 (count 0x1273) and buf+0xBE0 (0x1274). */
void func_801D51EC(u8 slot, u8 charId, s32 x, s32 y) {
    u8* buf = PORTRAIT_BUF(slot);
    s32 i;

    func_801C80B8(g_GameState.characters[charId].hp);
    buf[0x1273] = 0;
    for (i = 0; i < 3; i++) {
        u8 d = g_Menu->digits[6 + i];

        if (d != 0xFF) {
            buf[0x1273] += (u8)func_8002675C(
                g_Menu->unk2DC, d, buf + 0xAF0 + buf[0x1273] * 0x50,
                g_Menu->renderContext,
                x + D_801E9B28 + i * 8, y + D_801E9B2C, 0x1000);
        }
    }

    func_801C80B8(g_GameState.characters[charId].maxHp);
    buf[0x1274] = 0;
    for (i = 0; i < 3; i++) {
        u8 d = g_Menu->digits[6 + i];

        /* NB: retail positions max-value digits by BUILD count (left-packed),
         * not by digit index -- the index bump lives in a branch delay slot. */
        if (d != 0xFF) {
            buf[0x1274] += (u8)func_8002675C(
                g_Menu->unk2DC, d, buf + 0xBE0 + buf[0x1274] * 0x50,
                g_Menu->renderContext,
                x + D_801E9B30 + buf[0x1274] * 8, y + D_801E9B34, 0x1000);
        }
    }
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D53D0);
#else
extern s32 D_801E9B38;
extern s32 D_801E9B3C;
extern s32 D_801E9B40;
extern s32 D_801E9B44;

/* Arc A portraits: the MP / maxMP digit quads (2 digits each, digits[7..8]).
 * Lists buf+0xCD0 (count 0x1275) and buf+0xD70 (0x1276; left-packed). */
void func_801D53D0(u8 slot, u8 charId, s32 x, s32 y) {
    u8* buf = PORTRAIT_BUF(slot);
    s32 i;

    func_801C80B8(g_GameState.characters[charId].mp);
    buf[0x1275] = 0;
    for (i = 0; i < 2; i++) {
        u8 d = g_Menu->digits[7 + i];

        if (d != 0xFF) {
            buf[0x1275] += (u8)func_8002675C(
                g_Menu->unk2DC, d, buf + 0xCD0 + buf[0x1275] * 0x50,
                g_Menu->renderContext,
                x + D_801E9B38 + i * 8, y + D_801E9B3C, 0x1000);
        }
    }

    func_801C80B8(g_GameState.characters[charId].maxMp);
    buf[0x1276] = 0;
    for (i = 0; i < 2; i++) {
        u8 d = g_Menu->digits[7 + i];

        if (d != 0xFF) {
            buf[0x1276] += (u8)func_8002675C(
                g_Menu->unk2DC, d, buf + 0xD70 + buf[0x1276] * 0x50,
                g_Menu->renderContext,
                x + D_801E9B40 + buf[0x1276] * 8, y + D_801E9B44, 0x1000);
        }
    }
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D55B4);
#else
extern s32 D_801E9B48;
extern s32 D_801E9B4C;
extern s32 D_801E9B50;
extern s32 D_801E9B54;

/* Arc A portraits: the EXP / next-EXP digit quads (7 digits each,
 * digits[2..8]).  Lists buf+0xE10 (count 0x1277) and buf+0x1040 (0x1278).
 * The exp values live in the GameCharacter head blob (+0x44 / +0x48). */
void func_801D55B4(u8 slot, u8 charId, s32 x, s32 y) {
    u8* buf = PORTRAIT_BUF(slot);
    u8* pChar = (u8*)&g_GameState.characters[charId];
    s32 i;

    func_801C80B8(*(u32*)(pChar + 0x44));
    buf[0x1277] = 0;
    for (i = 0; i < 7; i++) {
        u8 d = g_Menu->digits[2 + i];

        if (d != 0xFF) {
            buf[0x1277] += (u8)func_8002675C(
                g_Menu->unk2DC, d, buf + 0xE10 + buf[0x1277] * 0x50,
                g_Menu->renderContext,
                x + D_801E9B48 + i * 8, y + D_801E9B4C, 0x1000);
        }
    }

    func_801C80B8(*(u32*)(pChar + 0x48));
    buf[0x1278] = 0;
    for (i = 0; i < 7; i++) {
        u8 d = g_Menu->digits[2 + i];

        if (d != 0xFF) {
            buf[0x1278] += (u8)func_8002675C(
                g_Menu->unk2DC, d, buf + 0x1040 + buf[0x1278] * 0x50,
                g_Menu->renderContext,
                x + D_801E9B50 + i * 8, y + D_801E9B54, 0x1000);
        }
    }
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D5794);
#else
extern s32 D_801E9B18;
extern s32 D_801E9B1C;
extern s32 D_801E9B20;
extern s32 D_801E9B24;

/* Arc A portraits: the LEVEL digit quads (3 digits, digits[6..8]) into
 * buf+0x910 (count 0x1271), and the second level number (char+0x63) into
 * buf+0xA00 (count 0x1272) -- tinted green in a post-pass. */
void func_801D5794(u8 slot, u8 charId, s32 x, s32 y) {
    u8* buf = PORTRAIT_BUF(slot);
    u8* pChar = (u8*)&g_GameState.characters[charId];
    s32 i;

    func_801C80B8(g_GameState.characters[charId].level);
    buf[0x1271] = 0;
    for (i = 0; i < 3; i++) {
        u8 d = g_Menu->digits[6 + i];

        if (d != 0xFF) {
            buf[0x1271] += (u8)func_8002675C(
                g_Menu->unk2DC, d, buf + 0x910 + buf[0x1271] * 0x50,
                g_Menu->renderContext,
                x + D_801E9B18 + i * 8, y + D_801E9B1C, 0x1000);
        }
    }

    func_801C80B8(pChar[0x63]);
    buf[0x1272] = 0;
    for (i = 0; i < 3; i++) {
        u8 d = g_Menu->digits[6 + i];

        if (d != 0xFF) {
            buf[0x1272] += (u8)func_8002675C(
                g_Menu->unk2DC, d, buf + 0xA00 + buf[0x1272] * 0x50,
                g_Menu->renderContext,
                x + D_801E9B20 + i * 8, y + D_801E9B24, 0x1000);
        }
    }

    for (i = 0; i < buf[0x1272]; i++) {
        POLY_FT4* p = PORTRAIT_POLY(buf, 0xA00, i * 2 + g_Menu->renderContext);

        SetShadeTex(p, 0);
        p->r0 = 0;
        p->g0 = 0x80;
        p->b0 = 0;
    }
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D5A50);
#else
extern void func_801D4F2C(u8 slot, u8 charId, s32 x, s32 y);
extern void func_801D50EC(u8 slot, s32 x, s32 y);
extern void func_801D51EC(u8 slot, u8 charId, s32 x, s32 y);
extern void func_801D53D0(u8 slot, u8 charId, s32 x, s32 y);
extern void func_801D55B4(u8 slot, u8 charId, s32 x, s32 y);
extern void func_801D5794(u8 slot, u8 charId, s32 x, s32 y);

/* Arc A portraits: rebuild party slot `slot`'s portrait panel at its CURRENT
 * animated position (anim slot cur + acc>>8) -- frame+face, status icons,
 * HP/MP/EXP/level digits -- and mark it visible (pManager->unk0[slot], which
 * the portrait sub-renderer func_801CE540 checks and the close-anim clears).
 * NB retail passes charId as $a1 register residue from the caller's lbu. */
void func_801D5A50(u8 slot, u8 charId) {
    MenuOpenAnim* pAnim;
    u8* buf;
    s32 x;
    s32 y;

    if (slot == 0xFF) {
        return;
    }
    pAnim = MENU_OPEN_ANIM(slot);
    buf = PORTRAIT_BUF(slot);
    /* retail rounds the 8.8 acc toward zero (adds 0xFF before asr on negatives) */
    x = pAnim->curX + ((pAnim->accX < 0 ? pAnim->accX + 0xFF : pAnim->accX) >> 8);
    y = pAnim->curY + ((pAnim->accY < 0 ? pAnim->accY + 0xFF : pAnim->accY) >> 8);

    func_801D4F2C(slot, charId, x, y);
    func_801D50EC(slot, x, y);
    func_801D51EC(slot, charId, x, y);
    func_801D53D0(slot, charId, x, y);
    func_801D55B4(slot, charId, x, y);
    func_801D5794(slot, charId, x, y);

    g_Menu->pManager->unk0[slot] = 1;
    buf[0x1270] = (u8)g_Menu->renderContext;
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D5BA4);
#else
/* Arc A content: the GOLD readout -- parse g_GameState.gold into digits
 * (leading zeros 0xFF-blanked), build up to 9 digit quads into the unk340[0]
 * buffer (count at +0x320) at x+i*8, plus the "G" unit glyph (atlas 0x10) at
 * +0x2D0, x+0x50.  Arms the gold-window content flag unk5[0]; rc at +0x324. */
void func_801D5BA4(s32 x, s32 y) {
    u8* buf = (u8*)(uintptr_t)*(u32*)&g_Menu->unk340[0];
    s32 i;

    func_801C80B8(g_GameState.gold);
    *(s32*)(buf + 0x320) = 0;
    for (i = 0; i < 9; i++) {
        u8 d = g_Menu->digits[i];

        if (d != 0xFF) {
            *(s32*)(buf + 0x320) += func_8002675C(
                g_Menu->unk2DC, d, buf + *(s32*)(buf + 0x320) * 0x50,
                g_Menu->renderContext, x + i * 8, y, 0x1000);
        }
    }
    func_8002675C(g_Menu->unk2DC, 0x10, buf + 0x2D0, g_Menu->renderContext,
                  x + 0x50, y, 0x1000);
    g_Menu->pManager->unk5[0] = 1;
    buf[0x324] = (u8)g_Menu->renderContext;
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D5CF8);
#else
/* Arc A verts: build the menu icon strip into the unk340[4] side buffer
 * (7 atlas ids from unk2EC + two 0xEE separators), rows of 8px steps. */
void func_801D5CF8(s32 x, s32 y) {
    u8* buf = (u8*)(uintptr_t)*(u32*)&g_Menu->unk340[4];
    s32 i;
    s32 xa;

    for (i = 0, xa = x; i < 3; i++, xa += 8) {
        func_8002675C(g_Menu->unk2DC, *(s32*)&g_Menu->unk2EC[i * 4],
                      buf + i * 0x50, g_Menu->renderContext, xa, y, 0x1000);
    }
    for (i = 3, xa = x + 0x20; i < 5; i++, xa += 8) {
        func_8002675C(g_Menu->unk2DC, *(s32*)&g_Menu->unk2EC[i * 4],
                      buf + 0xF0 + (i - 3) * 0x50, g_Menu->renderContext, xa, y, 0x1000);
    }
    for (i = 5, xa = x + 0x38; i < 7; i++, xa += 8) {
        func_8002675C(g_Menu->unk2DC, *(s32*)&g_Menu->unk2EC[i * 4],
                      buf + 0x190 + (i - 5) * 0x50, g_Menu->renderContext, xa, y, 0x1000);
    }
    func_8002675C(g_Menu->unk2DC, 0xEE, buf + 0x230, g_Menu->renderContext,
                  x + 0x18, y, 0x1000);
    func_8002675C(g_Menu->unk2DC, 0xEE, buf + 0x2D0, g_Menu->renderContext,
                  x + 0x30, y, 0x1000);
    buf[0x370] = (u8)g_Menu->renderContext;
}
#endif

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D5ED4);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D6194);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D6338);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D680C);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D6CF4);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D7154);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D74EC);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D7884);

void func_801D7C3C(u8 arg0, u8 arg1) {
    func_801D5ED4(arg0, arg1);
    func_801D6194(arg1);
    func_801D6338(arg0, arg1);
    func_801D680C(arg0, arg1);
    func_801D6CF4(arg0, arg1);
    func_801D74EC(arg0, arg1);
    func_801D7884(arg0, arg1);
    func_801D7154(arg0, arg1);
    {
        void* pMenu = g_Menu;
        void* pManager = *(void**)((u8*)pMenu + 0x33C);
        *(u8*)((u8*)pManager + 7) = 1;
    }
    {
        void* pMenu = g_Menu;
        void* pData = *(void**)((u8*)pMenu + 0x358);
        *(u8*)((u8*)pData + 0x2AE0) = *(u8*)((u8*)pMenu + 0x308);
    }
}

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D7CFC);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D7F50);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D827C);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D83AC);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D84B4);

s32 func_801D85DC(s32 arg0, u16* pArr1, u16* pArr2) {
    u32 maxVal = 0;
    s32 i, count;
    for (count = 0; count < 7; count++) {
        if (pArr1[count] > maxVal) maxVal = pArr1[count];
    }
    pArr1 += 7;
    for (count = 0; count < 7; count++) {
        if (pArr2[count] > maxVal) maxVal = pArr2[count];
    }
    return (s32)maxVal;
}

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D8644);

extern void func_801D7F50(u8, u8, u8);
extern void func_801D8644(u8, u8, u8, u8, u8);

void func_801D8DE4(u8 arg0, u8 arg1, u8 arg2, u8 arg3) {
    u8 w = 0x98;
    u8 h = 0x26;
    u8 val = arg3;
    if (arg1 != 0) {
        w = 0x80;
        h = 0x90;
    }
    func_801D7F50(w, h, val);
    func_801D8644(arg0, w, h, arg2, val);
    {
        void* pMenu = g_Menu;
        void* pManager = *(void**)((u8*)pMenu + 0x33C);
        *(u8*)((u8*)pManager + 8) = 1;
    }
    {
        void* pMenu = g_Menu;
        void* pData = *(void**)((u8*)pMenu + 0x35C);
        *(u8*)((u8*)pData + 0x32F1) = *(u8*)((u8*)pMenu + 0x308);
    }
}

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D8EA4);

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D9704);
#else
/* Nav N2c-2b: the target-slot navigator.  Advances `current` with wrap
 * (forward >=3 -> 0, backward <0 -> 2), skipping ineligible slots; any
 * direction other than 0 (forward) / 1 (backward) returns `current`
 * unchanged.  Eligibility is MODE-SPLIT: gearMode == 0 accepts a present
 * member (currentCharacterIDs[slot] != 0xFF); gearMode != 0 accepts a slot
 * whose has-gear byte unk5C[4+slot] (pManager+0x60, written by
 * func_801C6AA0) is nonzero.
 *
 * NO TERMINATION GUARD, by retail design: zero eligible slots would loop
 * forever.  Callers guarantee >= 1 eligible; do not "fix" this.
 *
 * Callsite catalogue (seven; only DB920 is ported as of N2c-2b):
 *   func_801DB920  (Items use-prompt: (target, 0, 0) DOWN / (target, 1, 0) UP)
 *   func_801DD790, func_801DDF24 (Abilities family)
 *   func_801E05D0  (Equip core)
 *   func_801E20C8, func_801E2BE4 (Status family)
 *   func_801E23CC  (Gear) -- KNOWN RESIDUE CASE: passes caller-$s2 (the
 *     dispatcher's saved arg0) as `current`, and a2 = 1 (GEAR mode) -- the
 *     mode whose compensation below is unexercisable from Items.  Flagged
 *     for N3-Gear; catalogue only. */
u8 func_801D9704(u8 current, s32 backward, s32 gearMode) {
    s32 slot = current;
    s32 mode;

    backward &= 0xFF;
    if (backward == 0) {
        mode = gearMode & 0xFF;
        slot += 1;
        for (;;) {
            if (slot >= 3) {
                slot = 0;
            }
            if (mode == 0) {
                if (g_Menu->pManager->currentCharacterIDs[slot] !=
                    CHARACTER_ID_NONE) {
                    return (u8)slot;
                }
                slot += 1;
            } else {
                u8 hasGear = g_Menu->pManager->unk5C[4 + slot];

                /* Retail's skip-increment sits in the branch DELAY SLOT and
                 * executes on BOTH outcomes; the accept path compensates
                 * with -1 (mirrored +1 on the backward side).  A linear
                 * reading returns candidate+1 -- an off-by-one that still
                 * navigates plausibly and only misbehaves in gear mode. */
                slot += 1;
                if (hasGear != 0) {
                    return (u8)(slot - 1);
                }
            }
        }
    }
    if (backward == 1) {
        mode = gearMode & 0xFF;
        slot -= 1;
        for (;;) {
            if (slot < 0) {
                slot = 2;
            }
            if (mode == 0) {
                if (g_Menu->pManager->currentCharacterIDs[slot] !=
                    CHARACTER_ID_NONE) {
                    return (u8)slot;
                }
                slot -= 1;
            } else {
                u8 hasGear = g_Menu->pManager->unk5C[4 + slot];

                /* Same delay-slot compensation, mirrored. */
                slot -= 1;
                if (hasGear != 0) {
                    return (u8)(slot + 1);
                }
            }
        }
    }
    return current;
}
#endif

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D9808);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D9B08);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D9C84);

extern s32 D_801EA904;
extern s32 D_801EA900;
extern void* D_801EA718;
extern void* D_801EA71C;
extern void* D_801EA720;

void func_801D9E3C(void) {
    void* pMenu;
    s32 i;
    pMenu = g_Menu;
    *(u8*)((u8*)pMenu + 0x4D8) = 0;
    func_801E64E0();
    func_801E5B3C();
    func_801E649C();
    for (i = 0; i < 0x20; i++) {
        void* pData = *(void**)((u8*)g_Menu + 0x32C);
        *(u8*)((u8*)pData + i * 0x5C + 0x58) = 0;
    }
    pMenu = g_Menu;
    *(u8*)(*(void**)((u8*)pMenu + 0x32C) + 0x4F8C) = 0xFF;
    pMenu = g_Menu;
    *(u8*)(*(void**)((u8*)pMenu + 0x32C) + 0x4F8D) = 0xFF;
    D_801EA904 = 0;
    D_801EA900 = 0;
    DrawSync(0);
    Vsync(0);
    EnterCriticalSection();
    CdSyncCallback(D_801EA718);
    CdReadyCallback(D_801EA71C);
    CdReadCallback(D_801EA720);
    ExitCriticalSection();
}

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D9F34);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D9F98);

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801DA4A8);
#else
extern u8 D_801EA548[];

void func_801DA4A8(void) {
    ItemMenuWork* work;

    func_801D22F4(2);
    func_801E8018(8, g_Menu->itemMenuStrings, D_801EA548);
    work = HeapAlloc(sizeof(ItemMenuWork), 0);
    MenuSetItemWork(work);
    bzero(work, sizeof(ItemMenuWork));
    func_801C72BC(0);
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801DA518);
#else
void func_801DA518(void) {
    ItemMenuWork* work = MenuItemWork();

    func_801D3444();
    func_801D4EA0(3);
    func_801D4EA0(4);
    g_Menu->pManager->unk48 = 0;
    func_801C72BC(0x10);

    /* Retail repeats the two payload frees after the resource-engine cleanup;
     * the Xenogears heap free operation is idempotent for an unpinned block. */
    HeapFree((void*)(uintptr_t)work->descriptionBundle);
    HeapFree(work);
    HeapFree(g_Menu->unk330->pItemsData);
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801DA5BC);
#else
extern void* GetItemName(s32);
extern s32 SystemRenderStringEntry(void*, void*, s32, s32);
extern void func_80033B34(u16*, u8*, s32);

/* Build one retail Items page: sixteen interleaved inventory slots, each with
 * its item name and two-digit quantity.  The page stride is two because the
 * visible list is laid out as two columns of eight rows. */
void func_801DA5BC(s32 page) {
    ItemMenuWork* work = MenuItemWork();
    u8* renderBuffer = HeapAlloc(0x3F6, 0);
    /* Retail's quantity source lives on the 32-bit PSX stack.  Keep the
     * native equivalent in the port's below-4GB heap because the shared text
     * descriptor intentionally preserves its four-byte pointer slot. */
    u8* quantityString = HeapAlloc(8, 0);
    s32 row;

    for (row = 0; row < 16; row++) {
        s32 inventoryIndex = page * 2 + row;
        u8 itemId = g_GameState.itemIDs[inventoryIndex];
        u8* quantity = &g_GameState.itemQuantities[inventoryIndex];

        if (itemId == 0) {
            *quantity = 0;
            work->rowVisible[row] = 0;
            continue;
        }
        if (*quantity == 0) {
            g_GameState.itemIDs[inventoryIndex] = 0;
            work->rowVisible[row] = 0;
            continue;
        }
        if (*quantity >= 100) {
            *quantity = 99;
        }

        work->itemNames[row].width = (u8)SystemRenderStringEntry(
            GetItemName(itemId), renderBuffer, 0x24, 0);
        {
            u16 quantityCodes[2];
            s32 tens = *quantity / 10;
            s32 ones = *quantity - tens * 10;

            quantityCodes[0] = (u16)(tens == 0 ? 0xC3 : tens + 0x10);
            quantityCodes[1] = (u16)(ones + 0x10);
            func_80033B34(quantityCodes, quantityString, 2);
            work->itemCounts[row].width = (u8)SystemRenderStringEntry(
                quantityString, renderBuffer, 0x24, 1);
        }
        {
            RECT upload;
            s32 halfRow = row / 2;
            s32 column = row - halfRow * 2;
            u8 itemFlags = g_Menu->unk330->pItemsData[itemId].flags;
            s32 style = ((itemFlags & 0x20) && D_80059171 == 0)
                            ? 0 : (itemFlags & 0x80);
            s32 xBase = column * 0x88;
            s32 y = halfRow * 0x10 + 0xE;

            upload.x = (s16)(0x180 + column * 0x18);
            upload.y = (s16)(0x80 + halfRow * 0xD);
            upload.w = 0x28;
            upload.h = 0xD;
            LoadImage(&upload, (u_long*)renderBuffer);
            DrawSync(0);

            func_801E7C50(&work->itemNames[row], row, 0x80, style | 1);
            func_801E7C50(&work->itemCounts[row], row, 0x80, style | 2);
            func_801C851C(work->itemNames[row].vertices,
                          (xBase + 0x28) & ~7, y,
                          work->itemNames[row].width, 0xD);
            func_801C851C(work->itemCounts[row].vertices,
                          (xBase + 0x90) & ~7, y,
                          work->itemCounts[row].width, 0xD);
        }
        work->itemNames[row].renderContext = (u8)g_Menu->renderContext;
        work->itemCounts[row].renderContext = (u8)g_Menu->renderContext;
        work->rowVisible[row] = 1;
    }
    HeapFree(quantityString);
    HeapFree(renderBuffer);
    g_Menu->pManager->unk48 = 1;
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801DA9A8);
#else
extern void* GetStringEntry(void* bundle, s32 index);
extern u8 D_801EA550[];
extern u8 D_801E9EA0[];

static void ItemMenuSetOpaqueWhite(POLY_FT4* poly) {
    poly->r0 = 0x80;
    poly->g0 = 0x80;
    poly->b0 = 0x80;
    SetSemiTrans(poly, 0);
}

/* Build the selected row's description panel and its conditional category
 * labels.  descriptionBundle is intentionally converted from a four-byte
 * retail pointer slot instead of being read through a native void**. */
void func_801DA9A8(s32 row, s32 page) {
    ItemMenuWork* work = MenuItemWork();
    s32 inventoryIndex = page * 2 + row;
    u8 itemId = g_GameState.itemIDs[inventoryIndex];
    s32 rc = g_Menu->renderContext;

    if (itemId == 0) {
        func_801E8044(8, g_Menu->pManager->unk38);
        work->descriptionVisible = 0;
        return;
    }

    {
        void* renderBuffer = HeapAlloc(0x618, 0);
        RECT upload;

        bzero(renderBuffer, 0x618);
        work->selectedItemDescription.width = (u8)SystemRenderStringEntry(
            GetStringEntry((void*)(uintptr_t)work->descriptionBundle, itemId),
            renderBuffer, 0x39, 0);

        upload.x = 0x140;
        upload.y = 0x4E;
        upload.w = 0x3C;
        upload.h = 0xD;
        LoadImage(&upload, (u_long*)renderBuffer);
        DrawSync(0);

        func_801E7C50(&work->selectedItemDescription, 0, 0, 0);
        func_801E920C(
            &work->selectedItemDescription.polys[rc],
            0x1C, 0xA1, 0, 0x4E,
            work->selectedItemDescription.width, 0xD);
        func_801C851C(
            work->selectedItemDescription.vertices,
            0x1C, 0xA1, work->selectedItemDescription.width, 0xD);
        HeapFree(renderBuffer);
    }

    memmove(&work->selectedItemName, &work->itemNames[row],
            sizeof(MenuString));
    memmove(&work->selectedItemCount, &work->itemCounts[row],
            sizeof(MenuString));

    func_801C851C(work->selectedItemName.vertices,
                  0x10, 0x93, work->selectedItemName.width, 0xD);
    func_801C851C(work->selectedItemCount.vertices,
                  (work->itemCounts[row].width == 0x10 ? 4 : 0) | 0x78,
                  0x93, work->selectedItemCount.width, 0xD);

    ItemMenuSetOpaqueWhite(&work->selectedItemName.polys[rc]);
    ItemMenuSetOpaqueWhite(&work->selectedItemCount.polys[rc]);
    func_801E8044(8, g_Menu->pManager->unk38);

    {
        MenuShopItem* item = &g_Menu->unk330->pItemsData[itemId];

        if (item->flags & 0xC0) {
            u16 categoryFlags = item->categoryFlags;
            s32 firstLabel;
            s32 secondLabel = ((u8)categoryFlags & 3) + 3;
            MenuString* first;
            MenuString* second;

            if (categoryFlags & 0x4000) {
                firstLabel = 2;
            } else {
                firstLabel = (categoryFlags & 0x1000) ? 0 : 1;
            }

            func_801E8070(8, g_Menu->itemMenuStrings, D_801EA550,
                          D_801E9EA0, g_Menu->pManager->unk38,
                          firstLabel, 0, 1);
            func_801E8070(8, g_Menu->itemMenuStrings, D_801EA550,
                          D_801E9EA0, g_Menu->pManager->unk38,
                          secondLabel, 0, 1);

            first = &g_Menu->itemMenuStrings[firstLabel];
            second = &g_Menu->itemMenuStrings[secondLabel];
            ItemMenuSetOpaqueWhite(&first->polys[rc]);
            ItemMenuSetOpaqueWhite(&second->polys[rc]);
        }
    }

    work->selectedItemName.renderContext = (u8)rc;
    work->selectedItemCount.renderContext = (u8)rc;
    work->selectedItemDescription.renderContext = (u8)rc;
    work->descriptionVisible = 1;
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801DB02C);
#else
void func_801DB02C(s32 cursorIndex) {
    u8 index = (u8)cursorIndex;
    g_Menu->arrowCursors[index] = HeapAlloc(sizeof(MenuArrowCursor), 0);
    bzero(g_Menu->arrowCursors[index], sizeof(MenuArrowCursor));
    g_Menu->arrowCursors[index]->curAnimFrame = 4;
    g_Menu->arrowCursors[index]->animFrameDuration = 0;
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801DB0A8);
#else
/* Animate, position, and arm one of the two Items arrow cursors.  Visibility
 * starts true before the mode dispatch, matching the retail branch delay
 * slot; mode 1 can then suppress an off-page selection cursor. */
void func_801DB0A8(s32 row, s32 page, s32 mode, s32 cursorIndex) {
    u8 index = (u8)cursorIndex;
    MenuArrowCursor* cursor = g_Menu->arrowCursors[index];
    s32 visible = 1;
    s32 x = 0;
    s32 y = 0;

    cursor->animFrameDuration++;
    if (cursor->animFrameDuration >= 6) {
        cursor->curAnimFrame--;
        if (cursor->curAnimFrame < 0) {
            cursor->curAnimFrame = 4;
        }
        cursor->animFrameDuration = 0;
    }

    switch ((u8)mode) {
    case 0: {
        s32 half = row / 2;
        s32 column = row - half * 2;
        x = column * 0x88 + 0x1C;
        y = half * 0x10 + 0x11;
        break;
    }
    case 1: {
        s32 pageStart = page * 2;
        if (row < pageStart || row >= pageStart + 0x10) {
            visible = 0;
        } else {
            s32 half = row / 2;
            s32 column = row - half * 2;
            x = column * 0x88 + 0x18;
            y = ((row - pageStart) / 2) * 0x10 + 0x11;
        }
        break;
    }
    case 2: {
        s32 half = row / 2;
        s32 column = row - half * 2;
        x = column * 0x88 + 0x18;
        y = half * 0x10 + 0x14;
        break;
    }
    case 3:
        x = 0xA0;
        y = row * 0xD + 0x14;
        visible = 2;
        break;
    default:
        /* Retail has no valid caller outside modes 0..3.  Keep an unexpected
         * mode from projecting the cursor through undefined coordinates. */
        visible = 0;
        break;
    }

    if (visible) {
        POLY_FT4* poly;
        u16 polyX;
        u16 polyY;
        u16 width;
        u16 height;

        func_8002675C(g_Menu->unk2DC, cursor->curAnimFrame + 0x15B,
                      cursor, g_Menu->renderContext, 0, 0, 0x1000);
        poly = &cursor->polys[g_Menu->renderContext];
        polyX = (u16)poly->x0;
        polyY = (u16)poly->y0;
        width = (u16)(poly->x1 - poly->x0);
        height = (u16)(poly->y3 - poly->y0);
        func_801C851C(cursor->vertices,
                      (u16)(polyX + x), (u16)(polyY + y), width, height);
        cursor->renderContext = (u8)g_Menu->renderContext;
        g_Menu->pManager->shouldRenderArrowCursor[index] = 1;
    } else {
        g_Menu->pManager->shouldRenderArrowCursor[index] = 0;
    }
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801DB340);
#else
void func_801DB340(s32 cursorIndex) {
    u8 index = (u8)cursorIndex;
    HeapFree(g_Menu->arrowCursors[index]);
    g_Menu->pManager->shouldRenderArrowCursor[index] = 0;
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801DB39C);
#else
extern void func_801E8EAC(POLY_FT4*, s32);
extern void func_801E8F60(s32, s32);

static void MenuTintString(MenuString* string, s32 mode) {
    func_801E8EAC(&string->polys[string->renderContext], mode);
}

void func_801DB39C(s32 mode) {
    ItemMenuWork* work = MenuItemWork();
    s32 tint = mode & 0xFF;
    s32 i;

    func_801E8F60(3, tint);
    func_801E8F60(4, tint);
    func_801E8EAC(&g_Menu->pScrollHandle->polys[
                      g_Menu->pScrollHandle->renderContext], tint);

    i = 0;
    while (i < 16) {
        POLY_FT4* name = &work->itemNames[i].polys[
            work->itemNames[i].renderContext];
        s32 row = i;

        i++;

        /* Retail advances the row on this skip as well.  A blank string's
         * neutral first colour byte is the sentinel used by the original. */
        if (name->r0 != 0x20) {
            func_801E8EAC(name, tint);
            MenuTintString(&work->itemCounts[row], tint);
        }
    }
    i = 0;
    while (i < MENU_MAX_NUM_ARROW_CURSORS) {
        MenuArrowCursor* cursor = g_Menu->arrowCursors[i];
        i++;
        func_801E8EAC(&cursor->polys[cursor->renderContext], tint);
    }
    MenuTintString(&work->selectedItemName, tint);
    MenuTintString(&work->selectedItemCount, tint);
    MenuTintString(&work->selectedItemDescription, tint);
    i = 0;
    do {
        MenuString* string = &g_Menu->itemMenuStrings[i];
        i++;
        MenuTintString(string, tint);
    } while (i < 8);
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801DB5E4);
#else
extern u8 D_801E9785;
extern s32 D_801EA054[];
extern s32 D_801EA098[];
extern s32 D_801EA0DC[];
extern s32 D_801EA120[];
extern void func_801CE0CC(MenuCharacter*, u8, u8, s32*, s32*, u8);

void func_801DB5E4(s32 mode) {
    s32* xTable;
    s32* yTable;
    u8 gearMode = 0;
    s32 i;

    if (!D_801E9785) {
        i = 0;
        do {
            s32 slot = i;
            MenuCharacter* panel = HeapAlloc(sizeof(MenuCharacter), 0);
            i++;
            g_Menu->currentCharacters[slot] = panel;
            bzero(panel, sizeof(MenuCharacter));
        } while (i < MAX_PARTY_MEMBERS);
        D_801E9785 = 1;
    }
    i = 0;
    do {
        MenuCharacter* panel = g_Menu->currentCharacters[i];
        i++;
        bzero(panel, sizeof(MenuCharacter));
    } while (i < MAX_PARTY_MEMBERS);

    if ((mode & 0xFF) == 2) {
        xTable = D_801EA098;
        yTable = D_801EA120;
        gearMode = 1;
    } else {
        xTable = D_801EA054;
        yTable = D_801EA0DC;
    }

    i = 0;
    while (i < MAX_PARTY_MEMBERS) {
        u8 charId = g_Menu->pManager->currentCharacterIDs[i];
        MenuCharacter* panel = g_Menu->currentCharacters[i];
        u8 eligible = 1;

        if (charId == CHARACTER_ID_NONE) {
            panel->unkBE7 = 0;
            i++;
            continue;
        }
        if (gearMode) {
            eligible = g_GameState.characters[charId].gearId != 0xFF;
        }
        if (eligible) {
            func_801CE0CC(panel, charId, (u8)i, xTable, yTable, gearMode);
        }
        i++;
    }

    g_Menu->pManager->unk46 = 1;
    i = 0;
    {
        s16 top = 0x30;
        s16 bottom = 0x40;
        do {
            POLY_FT4* cursor = &g_Menu->pCursors->polysCursor[
                i * 2 + g_Menu->pCursors->renderContexts[i]];

            cursor->x0 = cursor->x2 = 0x90;
            cursor->x1 = cursor->x3 = 0xA0;
            cursor->y0 = cursor->y1 = top;
            cursor->y2 = bottom;
            top += 0x38;
            i++;
            cursor->y3 = bottom;
            bottom += 0x38;
        } while (i < MAX_PARTY_MEMBERS);
    }
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801DB920);
#else
extern u8 D_80059171;
extern u8 D_801E9785;
extern void func_801C7BF4(void);
extern void func_801C8574(s32);
extern void func_801D397C(u8, s32, s32, s32, s32, u8, u8, s32, u8);
extern void func_801D4EA0(s32);
extern u8 func_801D9704(u8, s32, s32);
extern void func_801DA5BC(s32);
extern void func_801DA9A8(s32, s32);
extern void func_801DB39C(s32);
extern void func_801DB5E4(s32);
extern s32 func_801E31C0(MenuUnk6*, u8, u8);

s32 func_801DB920(s32 page, s32 row) {
    s32 inventoryIndex = page * 2 + row;
    u8 selectedTarget = g_Menu->selectedPartySlot;
    MenuShopItem* item = &g_Menu->unk330->pItemsData[
        g_GameState.itemIDs[inventoryIndex]];
    u8* quantity = &g_GameState.itemQuantities[inventoryIndex];
    s32 targetMask = 0;
    s32 running = 1;
    s32 rebuild = 1;
    s32 multiTarget;
    s32 usable = 0;
    s32 i;

    D_801E9785 = 0;
    g_XenoMenuN2c2Phase = 0;
    if (item->flags & 0x80) {
        usable = 1;
        if (item->flags & 0x20) {
            usable = D_80059171 != 0;
        }
    }
    multiTarget = item->categoryFlags & 1;
    if (!usable) {
        return 0;
    }

    func_801D397C(2, 0x10, 0xE, 0x90, 0xB0, 0, 0, 4, 0);
    while (running) {
        func_801C7BF4();
        targetMask = 0;

        if (rebuild) {
            rebuild = 0;
            func_801DA5BC(page);
            func_801DA9A8(row, page);
            func_801DB39C(1);
            func_801DB5E4(0);
            if (!g_XenoMenuN2c2Phase) {
                g_XenoMenuN2c2Phase = 1;
                printf("[xeno-port][test] N2C2A PROMPT: window 2, tint, "
                       "party panels, and default target cursor built\n");
                fflush(stdout);
            }
        }

        g_Menu->pCursors->shouldRender[2] = 0;
        g_Menu->pCursors->shouldRender[1] = 0;
        g_Menu->pCursors->shouldRender[0] = 0;
        if (multiTarget) {
            for (i = 0; i < MAX_PARTY_MEMBERS; i++) {
                if (g_Menu->pManager->currentCharacterIDs[i] !=
                    CHARACTER_ID_NONE) {
                    targetMask |= 1 << i;
                    g_Menu->pCursors->shouldRender[i] = 1;
                }
            }
        } else {
            targetMask = 1 << selectedTarget;
            g_Menu->pCursors->shouldRender[selectedTarget] = 1;
        }
        g_Menu->pManager->shouldRenderPointerCursors = 1;

        if (*quantity == 0) {
            running = 0;
        }
        if (!running) {
            break;
        }

        switch (g_Menu->input) {
        case MENU_INPUT_DOWN:
            selectedTarget = func_801D9704(selectedTarget, 0, 0);
            break;
        case MENU_INPUT_UP:
            selectedTarget = func_801D9704(selectedTarget, 1, 0);
            break;
        case MENU_INPUT_CONFIRM: {
            /* Retail accumulates func_801E31C0's ZERO returns: 0 = the item
             * effect applied to that target.  Any application consumes one
             * unit (chime 0x37); no application at all is the buzzer path. */
            s32 anyEffectApplied = 0;

            for (i = 0; i < MAX_PARTY_MEMBERS; i++) {
                if (func_801C865C((u16)targetMask, (u8)i) &&
                    func_801E31C0(g_Menu->unk330,
                                  g_Menu->pManager->currentCharacterIDs[i],
                                  g_GameState.itemIDs[inventoryIndex]) == 0) {
                    anyEffectApplied |= 1;
                }
            }
            if (anyEffectApplied) {
                func_801C8574(0x37);
                (*quantity)--;
                rebuild = 1;
                if (*quantity == 0) {
                    g_GameState.itemIDs[inventoryIndex] = 0;
                }
            } else {
                func_801C8574(4);
                rebuild = 1;
            }
            break;
        }
        case MENU_INPUT_BACK:
            running = 0;
            targetMask = 0;
            break;
        }
    }

    g_Menu->pCursors->shouldRender[2] = 0;
    g_Menu->pCursors->shouldRender[1] = 0;
    g_Menu->pCursors->shouldRender[0] = 0;
    func_801DB39C(0);
    g_Menu->pManager->unk46 = 0;
    func_801C7BF4();
    i = 0;
    do {
        MenuCharacter* panel = g_Menu->currentCharacters[i];
        i++;
        HeapFree(panel);
    } while (i < MAX_PARTY_MEMBERS);
    D_801E9785 = 0;
    func_801D4EA0(2);
    g_XenoMenuN2c2Phase = 2;
    printf("[xeno-port][test] N2C2A PROMPT: cancel teardown complete\n");
    fflush(stdout);
    return targetMask & 0xFF;
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801DBD4C);
#else
void func_801DBD4C(s32 inventoryIndexA, s32 inventoryIndexB) {
    u8 temp;

    temp = g_GameState.itemIDs[inventoryIndexA];
    g_GameState.itemIDs[inventoryIndexA] =
        g_GameState.itemIDs[inventoryIndexB];
    g_GameState.itemIDs[inventoryIndexB] = temp;

    temp = g_GameState.itemQuantities[inventoryIndexA];
    g_GameState.itemQuantities[inventoryIndexA] =
        g_GameState.itemQuantities[inventoryIndexB];
    g_GameState.itemQuantities[inventoryIndexB] = temp;
}

/* Harness-only N2c-1 readback.  Packing the first pair keeps the diagnostic
 * boundary scalar while all save-state access remains through GameState's
 * named native fields: id0, qty0, id1, qty1 from low to high byte. */
u32 PcPort_N2c1ReadItemPair(void) {
    return (u32)g_GameState.itemIDs[0] |
           ((u32)g_GameState.itemQuantities[0] << 8) |
           ((u32)g_GameState.itemIDs[1] << 16) |
           ((u32)g_GameState.itemQuantities[1] << 24);
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801DBDB4);
#else
extern u16 D_801EA724;
extern s32 D_801EA728;
extern u16 D_801EA72C;

void func_801DBDB4(void) {
    s32 i;
    s32 lastOccupied = 0;

    for (i = 0; i < MAX_INVENTORY_ITEMS; i++) {
        if (g_GameState.itemIDs[i] != 0) {
            lastOccupied = i;
        }
    }

    if (lastOccupied < 0x10) {
        D_801EA724 = 0x74;
        D_801EA728 = 0;
        D_801EA72C = 0;
    } else {
        s32 pages = ((lastOccupied - 0x10) / 2) + 1;
        D_801EA724 = 0x4A;
        D_801EA728 = pages;
        D_801EA72C = 0x1068 / pages;
    }
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801DBE54);
#else
extern void func_801DA5BC(s32);
extern void func_801DB0A8(s32, s32, s32, s32);
extern void func_801DA9A8(s32, s32);
extern s32 func_801DB920(s32, s32);
extern void func_801DBD4C(s32, s32);

/* Nav N2a Items screen.  The resource/content builders remain overlay stubs;
 * this ports the real lifecycle: init, two windows, open SFX, panel-out,
 * per-frame input loop, and the in-screen cursor teardown. */
s32 func_801DBE54(void) {
    s32 running = 1;
    s32 firstFrame = 1;
    s32 page = 0;
    s32 row = 0;
    s32 lastPage = -1;
    s32 lastRow = -1;
    s32 selected = -1;

    g_XenoMenuN2Phase = 0;
    func_801DA4A8();
    func_801DBDB4();
    func_801DB02C(0);
    func_801DB02C(1);

    while (running) {
        s32 newRow;

        func_801C7BF4();
        if (!firstFrame && g_XenoMenuN2Phase == 0 &&
            g_Menu->transitionEffectState == 0) {
            g_XenoMenuN2Phase = 1;
            printf("[xeno-port][test] Nav N2a: Items windows 3/4 settled open\n");
            fflush(stdout);
        }

        if (page != lastPage) {
            func_801DA5BC(page);
            func_801D3344(0xC, ((D_801EA72C * page) / 100) + 0x12,
                          D_801EA724);
            lastPage = page;
        }

        func_801DB0A8(row, page, 0, 0);
        if (row != lastRow) {
            func_801DA9A8(row, page);
            lastRow = row;
        }

        if (firstFrame) {
            func_801D397C(3, 0xC, 0xA, 0x124, 0x84, 0, 1, 4, 1);
            func_801D397C(4, 0x8, 0x8F, 0x130, 0x22, 0, 1, 4, 0);
            firstFrame = 0;
            func_801D1E80();
            func_801D29A8(0, 0);
        }

        func_801DB0A8(selected, page, 1, 1);

        switch (g_Menu->input) {
        case MENU_INPUT_RIGHT:
            newRow = row + 1;
            if (newRow < 0x10) {
                row = newRow;
            } else if (++page > D_801EA728) {
                page--;
            } else {
                row = 0xE;
            }
            lastRow = -1;
            break;
        case MENU_INPUT_DOWN:
            newRow = row + 2;
            if (newRow < 0x10) {
                row = newRow;
            } else if (++page > D_801EA728) {
                page--;
            }
            lastRow = -1;
            break;
        case MENU_INPUT_LEFT:
            newRow = row - 1;
            if (newRow >= 0) {
                row = newRow;
            } else if (--page < 0) {
                page++;
            } else {
                row = 1;
            }
            lastRow = -1;
            break;
        case MENU_INPUT_UP:
            newRow = row - 2;
            if (newRow >= 0) {
                row = newRow;
            } else if (--page < 0) {
                page++;
            }
            lastRow = -1;
            break;
        case MENU_INPUT_CONFIRM: {
            s32 current = page * 2 + row;
            if (selected == -1) {
                selected = current;
            } else {
                /* Retail refreshes the page/selection content only when the
                 * prompt reports a change (nonzero return: an item was used
                 * or depleted).  A cancelled prompt drops the pick alone. */
                if (current == selected) {
                    if (func_801DB920(page, row) != 0) {
                        lastPage = -1;
                        lastRow = -1;
                    }
                } else {
                    func_801DBD4C(current, selected);
                    lastPage = -1;
                    lastRow = -1;
                }
                selected = -1;
            }
            break;
        }
        case MENU_INPUT_BACK:
            if (selected == -1) {
                running = 0;
            } else {
                selected = -1;
            }
            break;
        case 9:
            page += 8;
            if (page > D_801EA728) {
                page = D_801EA728;
            }
            lastRow = -1;
            break;
        case 0xA:
            page -= 8;
            if (page < 0) {
                page = 0;
            }
            lastRow = -1;
            break;
        default:
            break;
        }
    }

    func_801D2484();
    func_801DB340(0);
    func_801DB340(1);
    func_801E8044(8, (u8*)g_Menu->pManager + 0x38);
    return 1;
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801DC1D4);
#else

/* Nav N3a-A1a: Abilities resource + text setup.  Allocates AbilityMenuWork
 * into the PSX-width slot unk42C[1], seeds the eight menu strings from the
 * bank-selected descriptor table, then loads the category's resource bank.
 * The category switch {0,1,2} -> mode {2,5,6} has a provably-dead default arm
 * (every caller passes 0/1/2), so only the reached arms are expressed. */
void func_801DC1D4(u8 category) {
    AbilityMenuWork* work;
    s32 mode;
    s32 bank = 0;
    u8 cat = category & 0xFF;

    work = HeapAlloc(sizeof(AbilityMenuWork), 0);
    g_Menu->unk42C[1] = (u32)(uintptr_t)work;
    bzero(work, sizeof(AbilityMenuWork));

    if (cat == 1) {
        mode = 5;
    } else if (cat == 2) {
        mode = 6;
        bank = 1;
    } else {
        mode = 2;
    }

    func_801E8018(8, g_Menu->itemMenuStrings, &D_801EA548[bank * 8]);
    func_801C72BC(mode & 0xFF);
    func_801D22F4(2);
    func_801D3488(2, category & 0xFF);
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801DC2CC);
#else

/* Nav N3a-A1a: Abilities teardown, reached from the ported common-exit
 * dispatcher func_801E3088 case 3.  Destroys the four windows, drops the
 * cursor state, frees the category's resource bank (mode | 0x10) and the
 * AbilityMenuWork allocation, then re-arms the main-menu window.
 * Same category switch as func_801DC1D4; the unwritten-$s1 default arm is
 * unreachable (E3088 passes 0, Status 0, Gear 1 and 2). */
void func_801DC2CC(s32 category) {
    AbilityMenuWork* work;
    s32 mode;
    u8 cat;

    func_801D4EA0(3);
    func_801D4EA0(4);
    func_801D4EA0(5);
    func_801D4EA0(6);
    func_801D2484();
    cat = category & 0xFF;
    g_Menu->pManager->unk4A[0] = 0;
    func_801C7BF4();

    if (cat == 1) {
        mode = 5;
    } else if (cat == 2) {
        mode = 6;
    } else {
        mode = 2;
    }

    func_801C72BC((mode | 0x10) & 0xFF);
    work = (AbilityMenuWork*)(uintptr_t)g_Menu->unk42C[1];
    HeapFree(work);
    g_Menu->pManager->unk5[1] = 1;
    g_Menu->pManager->shouldRenderWindow[1] = 1;
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801DC3D8);
#else
extern void* func_80033908(s32 index);
extern u8 D_801E97F0[];    /* per-character cat-0 confirm masks (lhu, stride 2) */
extern u8 D_801E9DDC[];    /* value-string x per row (lhu, stride 4) */
extern u8 D_801E9E14[];    /* value-string y per row (lhu, stride 4) */

/* A1b-1 stub guard for the deferred category arms. */
static void MenuAbilityStubArm(const char* what, s32 category) {
    static u32 warned;
    u32 bit = (category >= 0 && category < 32) ? (1u << category) : 0;

    if (bit != 0 && (warned & bit) == 0) {
        warned |= bit;
        printf("[xeno-port][stub-path] func_801DC3D8 %s (category %d) not "
               "ported (ported: category 0)\n", what, category);
        fflush(stdout);
    }
}

/* Nav N3a-A1b-1: the Abilities content builder, category-0 path (the other
 * two arms are Gear/Status credit and stay fail-visible stubs).
 *
 * Builds 14 rows (0..0xD): rows 0-0xB are the ability list (name + MP-cost
 * value), rows 0xC/0xD are the character's MP / maxMP stat rows (value only;
 * their labels are func_801DCE60's, still stubbed).
 *
 * Facts established by the A1b-1 frame map that are invisible in a linear
 * read of the retail asm:
 *
 *  - PRESENCE PREDICATE (delay-slot trap at 0x175C4-0x175D0): the `andi
 *    $v0,$s0,0xFF` in the branch shadow ALWAYS overwrites the slti result,
 *    so populated = (func_801C8640(word, i) != 0) || (i >= 0xC).  Rows
 *    12/13 are unconditionally populated.
 *  - CONFIRM THRESHOLD (delay-slot write at 0x17458): the character's
 *    current MP is saved across HeapAlloc in the call's branch shadow and
 *    compared against each row's stat value 1.5KB later (0x17BB4) -- a row
 *    is confirmable iff known AND cost <= current MP.
 *  - $s2 RECYCLING (0x17A5C): $s2 is the digit count (2 for cat-0) entering
 *    the tail and is REUSED as the confirmable flag 0x80 after the digit
 *    loop.  Same register, two unrelated meanings; they are two variables
 *    here (digitCount / confirmable).
 *  - sp+0x78 E8070 MODE BIAS: zeroed at entry, rewritten to 3 only on the
 *    category-2 row-12 arm (stubbed), so cat-0 provably calls func_801E8070
 *    with mode 2 at both sites.  The mode is logged below as
 *    belt-and-braces.
 *  - rowFlags WRITE: retail writes 0x1084+i for i in 0..0xD, so rows 12/13
 *    spill into unk1090[0..1]; the struct field is indexed past 0xB here
 *    deliberately (see AbilityMenuWork).
 *
 * Register cursors that retail threads through the dispatch ($s6 name-entry
 * cursor i*0x80, sp+0x90 value-entry cursor 0x700+i*0x80, $fp blob cursor
 * i*0x28) become native array indices / the explicit blobCursor below; the
 * geometry cursor (sp+0x88, stride 4 into D_801E9DDC/D_801E9E14) stays a
 * byte offset because those tables are untyped blobs. */
void func_801DC3D8(u8 ch, u8 category) {
    AbilityMenuWork* work = (AbilityMenuWork*)(uintptr_t)g_Menu->unk42C[1];
    u8 charId = g_Menu->pManager->currentCharacterIDs[ch & 0xFF];
    u8 cat = category & 0xFF;
    /* Retail: delay slot of the HeapAlloc below (0x17458). */
    u32 mpThreshold = g_GameState.characters[charId].mp;
    u8* renderBuffer = (u8*)HeapAlloc(0x3F6, 0);
    /* Retail's number string lives on the 32-bit PSX stack (sp+0x30).  Keep
     * the native equivalent in the port's below-4GB heap: the shared text
     * descriptor intentionally preserves its four-byte pointer slot, so a
     * native stack address would truncate (the item-menu precedent). */
    u8* numString = (u8*)HeapAlloc(16, 0);
    static const s32 divisors[5] = { 1, 10, 100, 1000, 10000 };
    u16 digitCodes[5] = { 0, 0, 0, 0, 0 };
    RECT upload;
    s32 row;
    s32 blobCursor = 0;    /* $fp: stat-blob row cursor, row * 0x28 */
    s32 geomCursor = 0;    /* sp+0x88: byte cursor into D_801E9DDC/D_801E9E14 */
    u8 modeBias = 0;       /* sp+0x78: E8070 mode = (modeBias + 2) & 0xFF */

    for (row = 0; row < 0xE; row++) {
        s32 populated;
        s32 digitCount = 2;  /* $s2 on entry to the arm dispatch (0x175E8) */
        s32 statValue = 0;   /* $s5 */
        s32 confirmable;     /* $s2 recycled at 0x17A5C */
        MenuString* nameStr = &work->strings[row];
        MenuString* valueStr = &work->strings[14 + row];

        /* Presence dispatch (0x174D4-0x175D8).  The per-category words live
         * in the unmapped GameState span unk1648: +0x7A cat 0, +0x7E cat 1,
         * +0x92 cat 2, each charId * 0x20. */
        if (cat == 0) {
            u16 word = *(u16*)&g_GameState.unk1648[0x7A + charId * 0x20];
            populated = ((u16)func_801C8640(word, row & 0xFF) != 0) ||
                        (row >= 0xC);
        } else if (cat == 1) {
            u16 word = *(u16*)&g_GameState.unk1648[0x7E + charId * 0x20];
            populated = ((u16)func_801C8640(word, row & 0xFF) != 0) ||
                        (row >= 0xC);
        } else if (cat == 2) {
            if (row & 1) {
                populated = 0;      /* odd rows absent (0x1757C) */
            } else {
                u16 word = *(u16*)&g_GameState.unk1648[0x92 + charId * 0x20];
                populated = ((u16)func_801C8640(word, (row >> 1) & 0xFF) != 0) ||
                            (row >= 0xC);
            }
        } else {
            populated = 0;          /* category >= 3: absent (0x174F8) */
        }

        if (!populated) {
            work->rowFlags[row] = 0;    /* 0x17D2C; spills for rows 12/13 */
            goto nextRow;
        }

        if (row < 0xC) {
            if (cat == 0) {
                /* Category-0 arm (0x17620-0x176B8): ability name from the
                 * string bundle, MP cost from the C72BC mode-2 per-character
                 * blob at +0x383. */
                u8* bank;
                nameStr->width = (u8)SystemRenderStringEntry(
                    func_80033908(((s32)charId << 4) + row),
                    renderBuffer, 0x24, 0);
                bank = (u8*)(uintptr_t)
                    *(u32*)&g_Menu->unk330->unk20[charId * 4];
                statValue = *(u8*)(bank + blobCursor + 0x383);
            } else {
                MenuAbilityStubArm("category arm", cat);
                work->rowFlags[row] = 0;
                goto nextRow;
            }
        } else {
            /* Rows 12/13 (0x178B0-0x179B4): the character's MP / maxMP.
             * The category-2 row-12 arm (fuel, 5 digits, modeBias = 3) is
             * Gear/Status credit and stays stubbed. */
            if (row == 0xC) {
                if (cat == 2) {
                    MenuAbilityStubArm("row-12 arm", cat);
                    work->rowFlags[row] = 0;
                    goto nextRow;
                }
                statValue = g_GameState.characters[charId].mp;
            } else {
                statValue = g_GameState.characters[charId].maxMp;
            }
        }

        /* Shared digit/render tail (0x179C0-0x17D10).  Digit extraction:
         * leading positions suppressed as glyph 0xC3 until the first nonzero
         * digit ($t1 flag), ones digit always emitted.  Retail stores the
         * ones digit in the delay slot of the func_80033B34 call, i.e.
         * before the conversion runs -- same order here. */
        {
            s32 value = statValue;
            s32 written = 0;
            s32 started = 0;
            s32 d;

            for (d = digitCount - 1; d > 0; d--) {
                s32 digit = value / divisors[d];
                if (digit != 0 || started != 0) {
                    digitCodes[written] = (u16)(digit + 0x10);
                    value -= digit * divisors[d];
                    started = 1;
                } else {
                    digitCodes[written] = 0xC3;
                }
                written++;
            }
            digitCodes[written] = (u16)(value % 10 + 0x10);
            func_80033B34(digitCodes, numString, digitCount);
        }

        confirmable = 0x80;     /* $s2 recycled (0x17A5C) -- see header */

        valueStr->width = (u8)SystemRenderStringEntry(numString, renderBuffer,
                                                      0x24, 1);

        upload.x = (s16)(0x180 + (row & 1) * 0x18);
        upload.y = (s16)(0x80 + (row >> 1) * 0xD);
        upload.w = 0x28;
        upload.h = 0xD;
        LoadImage(&upload, (u_long*)renderBuffer);
        DrawSync(0);

        /* Name-side setup + confirm test: rows 0-0xB only (0x17B20). */
        if (row < 0xC) {
            if (cat == 0) {
                if (func_801C865C(*(u16*)(D_801E97F0 + charId * 2),
                                  row & 0xFF) == 0) {
                    confirmable = 0;
                } else if ((s32)mpThreshold < statValue) {
                    confirmable = 0;
                }
            } else if (cat == 1) {
                confirmable = 0;                    /* 0x17BC8 */
            } else if (cat == 2) {
                if (row != 0 || (s32)mpThreshold < statValue) {
                    confirmable = 0;                /* 0x17BAC-0x17BC8 */
                }
            }
            func_801E7C50(nameStr, row, 0x80, confirmable | 1);
            func_801C851C(nameStr->vertices,
                          ((row & 1) * 0x88 + 0x24) & 0xFFFC,
                          ((row >> 1) * 0x10 + 0x12) & 0xFFFE,
                          nameStr->width, 0xD);
        }

        /* Value-side setup: all populated rows (0x17C4C-0x17CB8). */
        func_801E7C50(valueStr, row, 0x80, confirmable | 2);
        func_801C851C(valueStr->vertices,
                      *(u16*)(D_801E9DDC + geomCursor),
                      *(u16*)(D_801E9E14 + geomCursor),
                      valueStr->width, 0xD);

        nameStr->renderContext = (u8)g_Menu->renderContext;
        valueStr->renderContext = (u8)g_Menu->renderContext;
        work->rowFlags[row] = (u8)(confirmable | 1);    /* 0x17D0C */

    nextRow:
        blobCursor += 0x28;
        geomCursor += 4;
    }

    /* A1b-1 harness snapshot: the 14 flag bytes this build produced. */
    memcpy(g_XenoMenuN3aRowFlags, work->rowFlags, 0xC);
    g_XenoMenuN3aRowFlags[0xC] = work->unk1090[0];
    g_XenoMenuN3aRowFlags[0xD] = work->unk1090[1];

    HeapFree(numString);
    HeapFree(renderBuffer);

    /* Epilogue (0x17D64-0x17E28): two label draws, the 33rd-string sprite
     * setup, and the content-ready latch. */
    {
        s32 mode = (modeBias + 2) & 0xFF;

        printf("[xeno-port][test] A1B1 DC3D8 E8070 mode at call site 1 "
               "(retail 0x17DB4): %d\n", mode);
        fflush(stdout);
        func_801E8070(8, g_Menu->itemMenuStrings, D_801EA550, D_801E9EA0,
                      g_Menu->pManager->unk38, 6, 1, mode);
        printf("[xeno-port][test] A1B1 DC3D8 E8070 mode at call site 2 "
               "(retail 0x17DEC): %d\n", mode);
        fflush(stdout);
        func_801E8070(8, g_Menu->itemMenuStrings, D_801EA550, D_801E9EA0,
                      g_Menu->pManager->unk38, 7, 1, mode);
    }
    func_801D36E0(&work->unk1000String, ch & 0xFF, cat, 1);
    g_Menu->pManager->unk4A[0] = 1;
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801DCE60);
#else
static void MenuAbilityDescriptionStubArm(s32 category) {
    static u32 warned;
    u32 bit = (category >= 0 && category < 32) ? (1u << category) : 0;

    if (bit != 0 && (warned & bit) == 0) {
        warned |= bit;
        printf("[xeno-port][stub-path] func_801DCE60 category %d not ported "
               "(ported: category 0)\n", category);
        fflush(stdout);
    }
}

/* Nav N3a-A1b-2: build the selected category-0 ability's description panel
 * and shared labels.
 *
 * The retail work-buffer roles are:
 *   strings[28]    selected ability name (copied from strings[cursor])
 *   strings[29]    intentionally blank/reserved; only renderContext is set
 *   strings[30/31] the two description lines from abilityBank entries n/n+1
 *
 * D1640 draws all four slots while unk1092 is set.  The label strings are a
 * separate bank at SystemMenu.itemMenuStrings: E8070 positions them and arms
 * MenuManager.unk38[], then D0ED4 draws the armed entries through D11F0.
 * All MenuString access is by native array index; no retail 0x80 byte-stride
 * arithmetic crosses the native 0x98 MenuString layout. */
void func_801DCE60(u8 ch, u8 cursorArg, u8 category) {
    AbilityMenuWork* work =
        (AbilityMenuWork*)(uintptr_t)g_Menu->unk42C[1];
    s32 slot = ch & 0xFF;
    s32 cursor = cursorArg & 0xFF;
    s32 cat = category & 0xFF;
    s32 charId;
    s32 entryIndex;
    u8* abilityData;
    u16 flags;
    s32 firstLabel;
    s32 secondLabel;
    s32 rc;

    if (cat != 0) {
        MenuAbilityDescriptionStubArm(cat);
        return;
    }

    charId = g_Menu->pManager->currentCharacterIDs[slot];
    entryIndex = charId * 0x20 + cursor * 2;

    if (work->rowFlags[cursor] == 0) {
        work->unk1092 = 0;
        /* Retail clears only labels 0..5 here.  DC3D8's persistent MP/max-MP
         * labels are slots 6/7 and deliberately survive an empty cursor. */
        func_801E8044(6, g_Menu->pManager->unk38);
        return;
    }

    {
        u8* renderBuffer = HeapAlloc(0x618, 0);
        MenuString* line0 = &work->strings[30];
        MenuString* line1 = &work->strings[31];
        RECT upload;

        bzero(renderBuffer, 0x618);
        line0->width = (u8)SystemRenderStringEntry(
            GetStringEntry((void*)(uintptr_t)work->abilityBank, entryIndex),
            renderBuffer, 0x39, 0);
        line1->width = (u8)SystemRenderStringEntry(
            GetStringEntry((void*)(uintptr_t)work->abilityBank, entryIndex + 1),
            renderBuffer, 0x39, 1);

        upload.x = 0x140;
        upload.y = 0x4E;
        upload.w = 0x3C;
        upload.h = 0xD;
        LoadImage(&upload, (u_long*)renderBuffer);
        DrawSync(0);

        rc = g_Menu->renderContext;
        func_801E7C50(line0, 0, 0, 0);
        func_801E920C(&line0->polys[rc], 0x1C, 0x9E, 0, 0x4E,
                      line0->width, 0xD);
        func_801C851C(line0->vertices, 0x1C, 0x9E, line0->width, 0xD);

        func_801E7C50(line1, 1, 0, 0);
        func_801E920C(&line1->polys[rc], 0x1C, 0xAE, 0, 0x4E,
                      line1->width, 0xD);
        func_801C851C(line1->vertices, 0x1C, 0xAE, line1->width, 0xD);
        HeapFree(renderBuffer);
    }

    /* Retail memmoves one complete 0x80 PSX MenuString.  The native
     * equivalent must copy the complete expanded MenuString, including its
     * eight-byte host pointer, rather than copying a retail-sized prefix. */
    memmove(&work->strings[28], &work->strings[cursor],
            sizeof(MenuString));
    func_801C851C(work->strings[28].vertices, 0x12, 0x8E,
                  work->strings[28].width, 0xD);

    rc = g_Menu->renderContext;
    work->strings[28].polys[rc].r0 = 0x80;
    work->strings[28].polys[rc].g0 = 0x80;
    work->strings[28].polys[rc].b0 = 0x80;
    SetSemiTrans(&work->strings[28].polys[rc], 0);

    func_801E8044(8, g_Menu->pManager->unk38);

    abilityData = (u8*)(uintptr_t)
        *(u32*)&g_Menu->unk330->unk20[charId * 4];
    abilityData += cursor * 0x28 + 0x370;
    flags = *(u16*)abilityData;

    if (flags & 0x4000) {
        firstLabel = 2;
    } else {
        firstLabel = (flags & 0x1000) ? 0 : 1;
    }
    secondLabel = (abilityData[0] & 3) + 3;

    func_801E8070(8, g_Menu->itemMenuStrings, D_801EA550, D_801E9EA0,
                  g_Menu->pManager->unk38, firstLabel, 0, 2);
    func_801E8070(8, g_Menu->itemMenuStrings, D_801EA550, D_801E9EA0,
                  g_Menu->pManager->unk38, secondLabel, 0, 2);

    work->strings[28].renderContext = (u8)rc;
    work->strings[29].renderContext = (u8)rc;
    work->strings[30].renderContext = (u8)rc;
    work->strings[31].renderContext = (u8)rc;
    work->unk1092 = 1;
    g_Menu->pManager->unk38[6] = 1;
    g_Menu->pManager->unk38[7] = 1;
}
#endif

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801DD5E8);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801DD790);

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801DDF24);
#else

extern u16 D_801E9788[];
extern u16 D_801E9794[];
extern u16 D_801E97A0[];

/* Nav N3a-A1a: the Abilities screen core.  Resource setup, then a frame loop
 * that rebuilds content only when the selected character or cursor changed
 * (the 0xFF sentinels), builds its four windows once (the buildWindows latch)
 * and optionally runs the open animation, then dispatches input through the
 * eleven-entry table.
 *
 * A1a scope: the two content builders (func_801DC3D8, func_801DCE60) and the
 * interaction writer (func_801DD790) are A1b/A2 and remain unported; the
 * harness cancels without interacting so none is reached.  The confirm arm's
 * guard -- work->rowFlags[cursor] & 0x80 -- is ported faithfully because it is
 * exactly what keeps func_801DD790 unreached. */
void func_801DDF24(u8 charSel, u8 openAnim, u8 category) {
    AbilityMenuWork* work;
    s32 running = 1;
    s32 buildWindows = 1;
    s32 cursor = 0;
    s32 lastCursor = 0xFF;
    s32 curChar = charSel;
    s32 lastChar = 0xFF;
    u8 cat = category & 0xFF;
    s32 ch;

    func_801DC1D4(cat);
    func_801DB02C(0);

    do {
        func_801C7BF4();
        ch = curChar & 0xFF;
        if (ch != (lastChar & 0xFF)) {
            func_801DC3D8(ch, category & 0xFF);
            lastChar = curChar;
            lastCursor = 0xFF;
        }
        func_801DB0A8(cursor, 0, 2, 0);
        if (cursor != lastCursor) {
            func_801DCE60(ch, cursor & 0xFF, category & 0xFF);
            lastCursor = cursor;
        }

        if ((buildWindows & 0xFF) != 0) {
            func_801D397C(6, 0x10, 0xA, D_801E9788[cat * 2], 0x70, 0, 1, 4, 0);
            func_801D397C(5, 0xC, 0x86, 0xAC, 0x38, 0, 1, 4, 0);
            func_801D397C(4, D_801E9794[cat * 2], 0xA6, D_801E97A0[cat * 2],
                          0x18, 0, 1, 4, 0);
            func_801D397C(3, 0xC8, 0x86, 0x50, 0x18, 0, 1, 4, 0);
            /* retail clears the latch in the branch shadow -- unconditional */
            buildWindows = 0;
            if (openAnim != 0) {
                func_801D1E80();
                func_801D29A8(0, 0);
                func_801C7BF4();
            }
            g_Menu->pManager->unk5[1] = 0;
            g_Menu->pManager->shouldRenderWindow[1] = 0;
            {
                extern int g_XenoMenuN3aPhase;
                g_XenoMenuN3aPhase = 1;
                printf("[xeno-port][test] Nav N3a: Abilities windows built + "
                       "open animation done (content is A1b, window empty)\n");
                fflush(stdout);
            }
        }

        switch (g_Menu->input) {
        case 0:                                     /* RIGHT: cursor + 1 */
            if (cat != 2) {
                cursor++;
                if (cursor >= 0xC) {
                    cursor = 0xB;
                }
            }
            break;
        case 2:                                     /* LEFT: cursor - 1 */
            if (cat != 2) {
                cursor--;
                if (cursor < 0) {
                    cursor = 0;
                }
            }
            break;
        case 1:                                     /* DOWN: cursor + 2 */
            if (cursor + 2 < 0xC) {
                cursor = cursor + 2;
            }
            break;
        case 3:                                     /* UP: cursor - 2 */
            if (cursor - 2 >= 0) {
                cursor = cursor - 2;
            }
            break;
        case 4:                                     /* CONFIRM */
            work = (AbilityMenuWork*)(uintptr_t)g_Menu->unk42C[1];
            if (work->rowFlags[cursor] & 0x80) {
                func_801DD790(curChar & 0xFF, cursor, category & 0xFF);
                lastChar = 0xFF;
                lastCursor = 0xFF;
            }
            break;
        case 5:                                     /* BACK: leave the screen */
            running = 0;
            break;
        case 9:
            curChar = func_801D9704(curChar & 0xFF, 0, category & 0xFF);
            break;
        case 0xA:
            curChar = func_801D9704(curChar & 0xFF, 1, category & 0xFF);
            break;
        default:
            break;
        }
    } while (running != 0);

    func_801E8044(8, &g_Menu->pManager->unk38[0]);
    func_801DB340(0);
    {
        extern int g_XenoMenuN3aPhase;
        g_XenoMenuN3aPhase = 2;
        printf("[xeno-port][test] Nav N3a: Abilities loop exited; teardown "
               "next via func_801E3088 case 3 -> func_801DC2CC\n");
        fflush(stdout);
    }
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801DE29C);
#else

/* Nav N3a-A1a: Abilities dispatch entry (dispatch case 3).  Retail masks both
 * bytes, runs the screen with category 0, and always reports "keep the menu". */
s32 func_801DE29C(s32 charSel, s32 openAnim) {
    func_801DDF24(charSel & 0xFF, openAnim & 0xFF, 0);
    return 1;
}
#endif

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801DE2C8);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801DE36C);

void func_801DE400(void) {
    void* pMenu = g_Menu;
    void* pManager = *(void**)((u8*)pMenu + 0x33C);
    *(u8*)((u8*)pManager + 8) = 0;
    pMenu = g_Menu;
    pManager = *(void**)((u8*)pMenu + 0x33C);
    *(u8*)((u8*)pManager + 0x4B) = 0;
    func_801C7BF4();
    pMenu = g_Menu;
    HeapFree(*(void**)((u8*)pMenu + 0x35C));
    pMenu = g_Menu;
    HeapFree(*(void**)((u8*)pMenu + 0x360));
}

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

extern void func_801D3674(void);
extern void func_801D4EA0(s32);
extern void func_801E8044(s32, void*);
extern void func_801C72BC(s32);

void func_801E1398(void) {
    void* pMenu;
    func_801D3674();
    pMenu = g_Menu;
    *(u8*)(*(void**)((u8*)pMenu + 0x33C) + 0x4D) = 0;
    func_801D4EA0(2);
    func_801C7BF4();
    pMenu = g_Menu;
    func_801E8044(2, (u8*)(*(void**)((u8*)pMenu + 0x33C)) + 0x4E);
    func_801C72BC(0x14);
    pMenu = g_Menu;
    HeapFree(*(void**)((u8*)pMenu + 0x438));
}

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E1418);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E1544);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E1AC8);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E20C8);

extern void func_801C72BC(s32);
extern void func_801D3488(s32, s32);

u8 func_801E2250(void) {
    void* pMenu;
    void* pManager;
    s32 i;
    void* p1 = HeapAlloc(0x2AF0, NULL);
    pMenu = g_Menu;
    *(void**)((u8*)pMenu + 0x358) = p1;
    bzero(p1, 0x2AF0);
    {
        void* p2 = HeapAlloc(0x32F4, NULL);
        pMenu = g_Menu;
        *(void**)((u8*)pMenu + 0x35C) = p2;
        bzero(p2, 0x32F4);
    }
    {
        void* p3 = HeapAlloc(0x2AC, NULL);
        pMenu = g_Menu;
        *(void**)((u8*)pMenu + 0x360) = p3;
        bzero(p3, 0x2AC);
    }
    func_801C72BC(3);
    pMenu = g_Menu;
    pManager = *(void**)((u8*)pMenu + 0x33C);
    i = 0;
    while (*(u8*)((u8*)pManager + 0x60 + i) == 0) {
        i++;
    }
    func_801D3488(0, 1);
    return (u8)(i & 0xFF);
}

extern u8 D_801EA568[];
extern void func_801E8018(s32, u8*, s32, void*);

void func_801E2324(u8 arg0) {
    void* pMenu = g_Menu;
    void* pManager = *(void**)((u8*)pMenu + 0x33C);
    func_801E8018(6, (u8*)pMenu + 0x18E0, D_801EA568[arg0], (u8*)pManager + 0x54);
}

void func_801E2368(void) {
    void* pMenu = g_Menu;
    HeapFree(*(void**)((u8*)pMenu + 0x358));
    pMenu = g_Menu;
    HeapFree(*(void**)((u8*)pMenu + 0x35C));
    pMenu = g_Menu;
    HeapFree(*(void**)((u8*)pMenu + 0x360));
    func_801C72BC(0x13);
}

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E23CC);

extern void func_801D249C(s32);
extern void func_801D3488(s32, s32);

void func_801E2AE0(void) {
    void* pMenu;
    func_801D249C(1);
    {
        void* p1 = HeapAlloc(0x2AF0, NULL);
        pMenu = g_Menu;
        *(void**)((u8*)pMenu + 0x358) = p1;
        bzero(p1, 0x2AF0);
    }
    {
        void* p2 = HeapAlloc(0x32F4, NULL);
        pMenu = g_Menu;
        *(void**)((u8*)pMenu + 0x35C) = p2;
        bzero(p2, 0x32F4);
    }
    {
        void* p3 = HeapAlloc(0x2AC, NULL);
        pMenu = g_Menu;
        *(void**)((u8*)pMenu + 0x360) = p3;
        bzero(p3, 0x2AC);
    }
    func_801C72BC(3);
    func_801D3488(0, 0);
}

void func_801E2B80(void) {
    void* pMenu = g_Menu;
    HeapFree(*(void**)((u8*)pMenu + 0x358));
    pMenu = g_Menu;
    HeapFree(*(void**)((u8*)pMenu + 0x35C));
    pMenu = g_Menu;
    HeapFree(*(void**)((u8*)pMenu + 0x360));
    func_801C72BC(0x13);
}

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E2BE4);

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E3088);
#else
extern void func_801D9E3C(void);
extern void func_801E2368(void);
extern void func_801DC2CC(s32);
extern void func_801DE36C(void);
extern void func_801DE400(void);
extern void func_801D25E4(void);
extern void func_801E2B80(void);

/* Shared post-screen cleanup dispatcher.  Case 4 owns Items window/resource
 * destruction; the other cases preserve their existing overlay entry points. */
void func_801E3088(s32 arg0) {
    switch ((u8)g_Menu->menu1Choice + (arg0 & 0xFF)) {
    case 1:
    case 8:
        func_801D9E3C();
        break;
    case 2:
        g_Menu->pManager->unk5[2] = 0; /* offsets 7 and 8 */
        g_Menu->pManager->unk5[3] = 0;
        g_Menu->pManager->unk4A[1] = 0;
        func_801E2368();
        break;
    case 3:
        func_801DC2CC(0);
        break;
    case 4:
        func_801DA518();
        break;
    case 5:
        func_801DE36C();
        func_801DE400();
        break;
    case 6:
        g_Menu->pManager->unk5[2] = 0;
        g_Menu->pManager->unk5[3] = 0;
        g_Menu->pManager->unk4A[1] = 0;
        func_801D25E4();
        func_801E2B80();
        break;
    default:
        break;
    }
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E31C0);
#else
/* Nav N2c-3 fail-visible marker: counts entries into the guarded special
 * dispatch (effectFlags bit 0, magnitude 1 -> func_801E5058, magnitude 2 ->
 * func_801E5178 -- neither ported; N2c-4 / parked).  Must stay 0 for every
 * ordinary item; the stubs are NOT called so their break-counters stay 0. */
int g_XenoMenuN2c3SpecialHits = 0;

void func_801E5058(void);   /* the mag-1 bulk special, defined below (N2c-4) */

/* Nav N2c-3: the item-effect engine.  Applies pItemsData[itemId]'s effect to
 * g_GameState.characters[charId] (SAVE-BACKED writes, retail-clamped) and
 * returns 0 iff anything applied -- the polarity DB920's anyEffectApplied
 * accumulator documents (0 = applied -> chime 0x37 + consume one; nonzero =
 * a restore item whose target was already full -> buzzer 4).
 *
 * Retail seeds both restore multiplicands in DELAY SLOTS: the HP x50
 * (`ori $v0,0x32` in the full-HP compare's shadow, riding into the mult) and
 * the MP x10 (`ori $a1,0xA` in the HP-section entry beqz's shadow).  The
 * return tree likewise assigns v0 in branch shadows; the C below reproduces
 * the resulting values, not the register dance. */
s32 func_801E31C0(MenuUnk6* pItemBank, u8 charId, u8 itemId) {
    GameCharacter* target = &g_GameState.characters[charId];
    MenuShopItem* item = &pItemBank->pItemsData[itemId];
    s32 hpWasFull = 0;
    s32 mpWasFull = 0;

    if (item->effectFlags & 0x8000) {
        if (target->hp == target->maxHp) {
            hpWasFull = 1;
        } else {
            target->hp += item->effectMagnitude * 50;
        }
    }
    if (item->effectFlags & 0x4000) {
        if (target->mp == target->maxMp) {
            mpWasFull = 1;
        } else {
            target->mp += 10 * item->effectMagnitude;
        }
    }
    if (target->maxHp < target->hp) {
        target->hp = target->maxHp;
    }
    if (target->maxMp < target->mp) {
        target->mp = target->maxMp;
    }

    if (item->effectFlags & 0x4) {
        if (item->statEffectFlags & 0x8000) {
            target->attack += item->effectMagnitude;
        }
        if (item->statEffectFlags & 0x4000) {
            target->defense += item->effectMagnitude;
        }
        if (item->statEffectFlags & 0x2000) {
            target->ether += item->effectMagnitude;
        }
        if (item->statEffectFlags & 0x1000) {
            target->etherDefence += item->effectMagnitude;
        }
        if (item->statEffectFlags & 0x800) {
            target->maxHp += item->effectMagnitude;
        }
        if (item->statEffectFlags & 0x400) {
            target->maxMp += item->effectMagnitude;
        }
        /* The clamp block runs only inside the stat branch (retail). */
        if (target->attack > 0xC8) {
            target->attack = 0xC8;
        }
        if (target->defense > 0xC8) {
            target->defense = 0xC8;
        }
        if (target->ether > 0xC8) {
            target->ether = 0xC8;
        }
        if (target->etherDefence > 0xC8) {
            target->etherDefence = 0xC8;
        }
        if (target->maxHp > 999) {
            target->maxHp = 999;
        }
        if (target->maxMp > 99) {
            target->maxMp = 99;
        }
    }

    if (item->effectFlags & 0x2) {
        u8 amount = (u8)item->statEffectFlags;   /* lbu of the low byte */

        if (item->statEffectFlags & 0x8000) {
            target->unk78 += amount;
            if (target->unk78 > 0xC8) {
                target->unk78 = 0xC8;
            }
        } else if (target->unk78 < amount) {
            target->unk78 = 0;
        } else {
            target->unk78 -= amount;
        }
    }

    if (item->effectFlags & 0x1) {
        if (item->effectMagnitude == 1) {
            /* N2c-4: the bulk special is real. */
            func_801E5058();
        } else if (item->effectMagnitude == 2) {
            /* GUARDED, fail-visible: magnitude 2 (func_801E5178) stays
             * parked and structurally absent. */
            g_XenoMenuN2c3SpecialHits++;
            printf("[xeno-port][stub-path] func_801E31C0 special dispatch "
                   "mag=2 (func_801E5178) NOT PORTED\n");
            fflush(stdout);
        }
    }

    if (item->effectFlags & 0x8000) {
        if (item->effectFlags & 0x4000) {
            if (hpWasFull == 0) {
                return 0;
            }
            return mpWasFull;
        }
        return hpWasFull;
    }
    if (item->effectFlags & 0x4000) {
        return mpWasFull;
    }
    return 0;
}
#endif

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E35BC);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E36D4);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E3A80);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E3C2C);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E3ECC);

extern void func_801E41C0(s32, u8);
extern void func_801E42AC(s32, u8);

void func_801E4170(s32 arg0, u8 arg1) {
    func_801E41C0(arg0, arg1);
    func_801E42AC(arg0, arg1);
    func_801E4258(arg0, arg1);
}

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E41C0);

void func_801E4258(void* pCtx, u8 idx) {
    s32 i = idx;
    u8* pBase = (u8*)&g_GameState;
    u8* pEntry = pBase + 0x978 + i * 0x28;
    s32 tableIdx = pEntry[8];
    u8* pTbl = *(u8**)((u8*)pCtx + 0x10);
    *(u16*)(pEntry + 0x70) = *(u16*)(pTbl + tableIdx * 0x14 + 8);
    *(u16*)(pEntry + 0x72) = *(u16*)(pTbl + tableIdx * 0x14 + 0xA);
}

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E42AC);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E433C);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E4754);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E4928);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E4998);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E4A28);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E4D10);

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E5058);
#else
/* Nav N2c-4: the magnitude-1 bulk special (func_801E31C0's effectFlags&0x1
 * dispatch).  Populates all five inventory families with sequential IDs at
 * quantity 10 -- a bulk inventory initializer/unlocker.  SAVE-BACKED, the
 * widest single mutation in the menu arc.
 *
 * Retail loop shape (identical x5): the ID store uses the pre-increment
 * index, the increment sits BETWEEN the two stores, and the next
 * iteration's index is recomputed in the loop-back DELAY SLOT
 * (`andi $v0,$v1,0xFF`).  In C that collapses to ids[i]=i; qty[i]=10 --
 * the scheduling is register dance, the slot coverage is what matters:
 * every loop starts at i=1, so SLOT 0 of every family is untouched, and
 * the items loop additionally writes at [i+2] (slots 3..0x4D get IDs
 * 1..0x4B), leaving item slots 0-2 -- where the special item itself
 * lives -- intact. */
void func_801E5058(void) {
    u8 i;   /* the u8 index IS the byte-match shape: retail masks it to
             * 0xFF at every use (andi), which a u8 var reproduces exactly
             * (72/72 opcodes vs retail via the codegen-scratch flow). */

    for (i = 1; i < 0x48; i++) {
        g_GameState.weaponIDs[i] = i;
        g_GameState.weaponQuantities[i] = 10;
    }
    for (i = 1; i < 0x96; i++) {
        g_GameState.accessoryIDs[i] = i;
        g_GameState.accessoryQuantities[i] = 10;
    }
    for (i = 1; i < 0x4C; i++) {
        g_GameState.itemIDs[i + 2] = i;
        g_GameState.itemQuantities[i + 2] = 10;
    }
    for (i = 1; i < 0x48; i++) {
        g_GameState.unk2120IDs[i] = i;
        g_GameState.unk20BCQuantities[i] = 10;
    }
    for (i = 1; i < 0x69; i++) {
        g_GameState.unk221AIDs[i] = i;
        g_GameState.unk2184Quantities[i] = 10;
    }
}
#endif

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E5178);

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E53CC);
#else
/* Arc A pixels slice: initialize window `windowIndex`'s frame primitives.
 * Sets up the double-buffered background G4 pair (semi-trans, RGB 0x68) +
 * their draw modes, and the 4x4 border POLY_FT4s (top/bottom/left/right,
 * shade-tex, white) with tpage/clut from the border-texture fields that the
 * B1b builder func_801C6E68 unpacked from the atlas.  Poly TYPES + colors +
 * tpage/clut only -- the vertex geometry is computed by the window-size
 * family (func_801D5A50 et al.) during the open animation.  Also clears this
 * window's shouldRenderWindow flag (the open animation re-arms it). */
void func_801E53CC(u8 windowIndex) {
    MenuWindow* pWindow = g_Menu->windows[windowIndex];
    RECT texWindow;
    s32 i;

    texWindow.x = 0;
    texWindow.y = 0;
    texWindow.w = 0x100;
    texWindow.h = 0x100;

    g_Menu->pManager->shouldRenderWindow[windowIndex] = 0;
    g_Menu->pManager->unk27[windowIndex] = 0;

    for (i = 0; i < 2; i++) {
        POLY_G4* pBg = &pWindow->polysBackground[i];

        SetPolyG4(pBg);
        pBg->r0 = 0x68; pBg->g0 = 0x68; pBg->b0 = 0x68;
        pBg->r1 = 0x68; pBg->g1 = 0x68; pBg->b1 = 0x68;
        pBg->r2 = 0x68; pBg->g2 = 0x68; pBg->b2 = 0x68;
        pBg->r3 = 0x68; pBg->g3 = 0x68; pBg->b3 = 0x68;
        SetSemiTrans(pBg, 1);
        SetDrawMode(&pWindow->drawModes[i], 0, 0,
                    GetTPage(0, 0, g_Menu->texPageX0, g_Menu->texPageY0),
                    &texWindow);
    }

    for (i = 0; i < 4; i++) {
        POLY_FT4* p;

        p = &pWindow->polysWindowBorderTop[i];
        SetPolyFT4(p);
        SetShadeTex(p, 1);
        p->r0 = 0xFF; p->g0 = 0xFF; p->b0 = 0xFF;
        p->tpage = GetTPage(g_Menu->texPage0, 0, g_Menu->texPageX0, g_Menu->texPageY0);
        p->clut = GetClut(g_Menu->clutX0, g_Menu->clutY0);

        p = &pWindow->polysWindowBorderBottom[i];
        SetPolyFT4(p);
        SetShadeTex(p, 1);
        p->r0 = 0xFF; p->g0 = 0xFF; p->b0 = 0xFF;
        p->tpage = GetTPage(g_Menu->texPage1, 0, g_Menu->texPageX1, g_Menu->texPageY1);
        p->clut = GetClut(g_Menu->clutX1, g_Menu->clutY1);

        p = &pWindow->polysWindowBorderLeft[i];
        SetPolyFT4(p);
        SetShadeTex(p, 1);
        p->r0 = 0xFF; p->g0 = 0xFF; p->b0 = 0xFF;
        p->tpage = GetTPage(g_Menu->texPage2, 0, g_Menu->texPageX2, g_Menu->texPageY2);
        p->clut = GetClut(g_Menu->clutX2, g_Menu->clutY2);

        p = &pWindow->polysWindowBorderRight[i];
        SetPolyFT4(p);
        SetShadeTex(p, 1);
        p->r0 = 0xFF; p->g0 = 0xFF; p->b0 = 0xFF;
        p->tpage = GetTPage(g_Menu->texPage3, 0, g_Menu->texPageX3, g_Menu->texPageY3);
        p->clut = GetClut(g_Menu->clutX3, g_Menu->clutY3);
    }
}
#endif

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E56E8);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E5924);

extern void func_801E56E8(s32);

void func_801E5ACC(void) {
    s32 i;
    for (i = 0; i < 0x20; i++) {
        void* pBuf = HeapAlloc(0x158, NULL);
        void* pMenu = g_Menu;
        *(void**)((u8*)pMenu + 0x3A8 + i * 4) = pBuf;
        bzero(pBuf, 0x158);
        func_801E56E8(i);
        func_801E5924(i);
    }
}

void func_801E5B3C(void) {
    s32 i;
    for (i = 0; i < 0x20; i++) {
        void* pMenu = g_Menu;
        HeapFree(*(void**)((u8*)pMenu + 0x3A8 + i * 4));
    }
}

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E5B88);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E5E4C);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E61B0);

extern void func_801E5B88(void);
extern void func_801E5E4C(void);

void func_801E6450(void) {
    void* pBuf = HeapAlloc(0x2DC0, NULL);
    void* pMenu = g_Menu;
    *(void**)((u8*)pMenu + 0x34C) = pBuf;
    bzero(pBuf, 0x2DC0);
    func_801E5B88();
    func_801E5E4C();
}

void func_801E649C(void) {
    void* pMenu = g_Menu;
    HeapFree(*(void**)((u8*)pMenu + 0x34C));
    pMenu = g_Menu;
    *(u8*)(*(void**)((u8*)pMenu + 0x33C) + 0xB) = 0;
}

void func_801E64E0(void) {
    RECT rect;
    rect.x = 0;
    rect.y = 0;
    rect.w = 0x140;
    rect.h = 0xE0;
    ClearImage(&rect, 0x40, 0x20, 0x20);
    {
        void* pMenu = g_Menu;
        *(u8*)(*(void**)((u8*)pMenu + 0x33C) + 0xB) = 0;
    }
}

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

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E7C50);
#else
/* Initialize both buffered textured quads for a MenuString.  `style` selects
 * the text atlas/palette and disabled-state treatment; `index` selects the
 * atlas cell while preserving retail's pair-wise loop progression. */
void func_801E7C50(MenuString* string, s32 index, s32 yOffset, s32 style) {
    s32 parity = index & 1;
    s32 pair = index / 2;
    s32 atlasX = (pair & 1) * 0x80;
    s32 bufferIndex;

    for (bufferIndex = 0; bufferIndex < 2; bufferIndex++) {
        POLY_FT4* poly = &string->polys[bufferIndex];
        s32 colorMask = 0;
        s32 u;
        s32 v;

        func_801E927C(poly);
        if ((u8)style == 0) {
            string->unk7C = (u8)parity;
            poly->tpage = GetTPage(0, 0, 0x140, 0);
            u = atlasX;
            v = ((index + yOffset) / 4) * 0xD;
        } else {
            if (((u8)style & 0x80) == 0) {
                colorMask = 0x20;
                SetSemiTrans(poly, 1);
                poly->r0 = 0x20;
                poly->g0 = 0x20;
                poly->b0 = 0x20;
            }
            string->unk7C = (u8)(((u8)style & 0x7F) - 1);
            poly->tpage = (u16)(colorMask | GetTPage(0, 0, 0x180, 0x80));
            u = parity * 0x60;
            v = pair * 0xD + yOffset;
        }

        poly->u0 = (u8)u;
        poly->v0 = (u8)v;
        poly->u1 = (u8)(u + string->width);
        poly->v1 = (u8)v;
        poly->u2 = (u8)u;
        poly->v2 = (u8)(v + 0xD);
        poly->u3 = (u8)(u + string->width);
        poly->v3 = (u8)(v + 0xD);
        poly->clut = string->unk7C ? g_SystemPalette2 : g_SystemPalette1;
    }
    string->unk7F = 0;
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E7E68);
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E8018);
#else
/* Render descriptor pairs into their two buffered MenuString quads.  The
 * index and descriptor pointer advance before DrawSync, preserving the retail
 * delay-slot update order used by the loop test. */
void func_801E7E68(MenuString* strings, u8* descriptorIds,
                   s32 yOffset, s32 count) {
    s32 index = 0;

    while (index < count) {
        MenuString* first = &strings[index];
        MenuString* second = &strings[index + 1];
        s32 row = (index + yOffset) / 4;

        first->width = (u8)SystemRenderStringEntry(
            GetStringEntry(g_Menu->unk2E0, descriptorIds[0]),
            g_Menu->unk4E0[0].pVramBuffer, 0x18, 0);
        second->width = (u8)SystemRenderStringEntry(
            GetStringEntry(g_Menu->unk2E0, descriptorIds[1]),
            g_Menu->unk4E0[0].pVramBuffer, 0x18, 1);

        first->vramDest.x = (s16)(0x140 + ((index << 4) & 0x20));
        first->vramDest.y = (s16)(row * 0xD);
        first->vramDest.w = 0x1C;
        first->vramDest.h = 0xD;
        second->vramDest = first->vramDest;

        func_801E7C50(first, index, yOffset, 0);
        func_801E7C50(second, index + 1, yOffset, 0);
        LoadImage(&first->vramDest,
                  (u_long*)g_Menu->unk4E0[0].pVramBuffer);

        descriptorIds += 2;
        index += 2;
        DrawSync(0);
    }
}

void func_801E8018(s32 count, MenuString* strings, u8* descriptorIds) {
    func_801E7E68(strings, descriptorIds, 4, count & 0xFF);
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E8044);
#else
/* Nav N1: zero `count` visibility flags (the pManager->unkC string flags). */
void func_801E8044(s32 count, void* pFlags) {
    u8* p = (u8*)pFlags;
    s32 i;

    for (i = 0; i < (count & 0xFF); i++) {
        p[i] = 0;
    }
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E8070);
#else
extern u8 D_801E9E64[];   /* per-option string x-offset table (u16 at *4) */
extern u8 D_801E9EC4[];   /* Items description label x positions */
extern u8 D_801E9EE4[];   /* Items description label y position */
extern u8 D_801E9EE8[];   /* Abilities row x positions (E8070 mode 2/5) */
extern u8 D_801E9F28[];   /* Abilities row y positions, indexed by arg6 */

/* Nav N1: position the selected option's MenuString quad (the rendered text
 * strip) at the cursor's table position, mark it visible.  Retail is a 7-case
 * jump table.  Mode 0 serves the main menu and mode 1 serves the Items
 * description labels; all remaining modes stay deliberately unported. */
void func_801E8070(s32 count, void* pStrings, void* pIdTable, void* pOffTable,
                   void* pFlags, s32 selected, s32 arg6, s32 mode) {
    MenuString* pStr;
    POLY_FT4* p;
    u16 x;
    u16 y;
    u16 xo;
    s32 rc = g_Menu->renderContext;
    s32 sel = selected & 0xFF;

    (void)pIdTable;
    if ((mode & 0xFF) == 1) {
        pStr = &((MenuString*)pStrings)[sel];
        x = *(u16*)(D_801E9EC4 + sel * 4);
        y = *(u16*)D_801E9EE4;
        func_801C851C(pStr->vertices, x, y, pStr->width, 0xD);
        pStr->renderContext = (u8)rc;
        ((u8*)pFlags)[sel] = 1;
        return;
    }

    /* Nav N3a-A1b-1: the Abilities row draw.  Structurally identical to mode 1
     * -- same func_801C851C call and the same shared tail at .L801E8428 -- and
     * differs only in where x and y come from.
     *
     * Retail's mode 5 is a ONE-INSTRUCTION arm (`ori $a1,$zero,8`) that falls
     * through into this body, biasing the x table by 8; it is the category-2
     * variant used by func_801DC3D8's deferred arm.  It is deliberately NOT
     * folded in here -- mode 5 stays in the guard below and lands with
     * Gear/Status, so this port cannot silently absorb its entry. */
    if ((mode & 0xFF) == 2) {
        pStr = &((MenuString*)pStrings)[sel];
        x = *(u16*)(D_801E9EE8 + sel * 4);
        y = *(u16*)(D_801E9F28 + (arg6 & 0xFF) * 4);
        func_801C851C(pStr->vertices, x, y, pStr->width, 0xD);
        pStr->renderContext = (u8)rc;
        ((u8*)pFlags)[sel] = 1;
        return;
    }

    if ((mode & 0xFF) != 0) {
        static int warned;
        if (!warned) {
            warned = 1;
            printf("[xeno-port][stub-path] func_801E8070 mode %d not ported "
                   "(ported: 0, 1, 2)\n", mode & 0xFF);
        }
        return;
    }

    func_801E8044(count, pFlags);

    pStr = &((MenuString*)pStrings)[sel];
    p = &pStr->polys[rc];
    x = *(u16*)(D_801E9A00 + sel * 4);
    y = *(u16*)(D_801E9A2C + sel * 4);
    xo = *(u16*)((u8*)pOffTable + sel * 4);
    (void)arg6;

    p->x0 = (s16)(x + xo + 0x16);
    p->y0 = (s16)(y - 0x22);
    p->x1 = (s16)(x + xo + 0x16 + pStr->width);
    p->y1 = (s16)(y - 0x22);
    p->x2 = (s16)(x + xo + 0x16);
    p->y2 = (s16)(y - 0x15);
    p->x3 = (s16)(x + xo + 0x16 + pStr->width);
    p->y3 = (s16)(y - 0x15);

    pStr->renderContext = (u8)rc;
    ((u8*)pFlags)[sel] = 1;
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E8474);
#else
/* Arc A content: the OPTION-LABEL REVEAL.  Staged open animation: each stage
 * rebuilds the highlight set (polysCursors, pTable[i*2]) and the normal set
 * (polysTexts, pTable[i*2+1]) from the atlas id pairs (D_801EA19C), arms
 * pManager->shouldRenderSelectionMenu, and draws two frames -- the options
 * appear one by one.  The label glyphs land at a fixed 0xA0,0x96 base (the
 * per-option offsets come from the atlas entries themselves). */
void func_801E8474(s32 count, void* pTable) {
    u32* pIds = (u32*)pTable;
    s32 stage;
    s32 prev;
    s32 i;

    g_Menu->pSelectionMenu->unk1192 = 0;
    g_Menu->pSelectionMenu->unk1193 = 0;
    g_Menu->pManager->shouldRenderSelectionMenu = 1;

    if (count <= 0) {
        return;
    }
    prev = 0;
    for (stage = 1; stage <= count; stage++, prev++) {
        if (stage != count) {
            g_Menu->pSelectionMenu->numCursors = 0;
            for (i = 0; i < stage; i++) {
                g_Menu->pSelectionMenu->numCursors += func_8002675C(
                    g_Menu->unk2DC, pIds[i * 2],
                    (u8*)g_Menu->pSelectionMenu +
                        g_Menu->pSelectionMenu->numCursors * 0x50,
                    g_Menu->renderContext, 0xA0, 0x96, 0x1000);
            }
            g_Menu->pSelectionMenu->cursorsRenderCtx = (u8)g_Menu->renderContext;
        }

        g_Menu->pSelectionMenu->numTexts = 0;
        if (stage != 1 && prev > 0) {
            for (i = 0; i < prev; i++) {
                g_Menu->pSelectionMenu->numTexts += func_8002675C(
                    g_Menu->unk2DC, pIds[i * 2 + 1],
                    (u8*)g_Menu->pSelectionMenu + 0x8C0 +
                        g_Menu->pSelectionMenu->numTexts * 0x50,
                    g_Menu->renderContext, 0xA0, 0x96, 0x1000);
            }
            g_Menu->pSelectionMenu->textsRenderCtx = (u8)g_Menu->renderContext;
        } else if (stage != 1) {
            g_Menu->pSelectionMenu->textsRenderCtx = (u8)g_Menu->renderContext;
        }

        for (i = 0; i < 2; i++) {
            func_801C7BF4();
        }
    }
}
#endif

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E86C8);

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E8978);
#else
extern void func_801D1EE0(s32 selected, s32 buildBar);

/* Nav N1: the selection-aware option-label rebuild.  Rebuilds both label sets
 * from the id pairs -- the SELECTED option's highlight glyph uses id+0xD (the
 * bright variant) -- then rebuilds the pointer cursor at the selection and
 * arms pManager->unk4 (the pointer-visible flag).  Called on every
 * menu1Choice change (and once at open). */
void func_801E8978(s32 count, s32 selected, void* pTable) {
    u32* pIds = (u32*)pTable;
    s32 i;

    g_Menu->pSelectionMenu->numCursors = 0;
    g_Menu->pSelectionMenu->numTexts = 0;
    for (i = 0; i < (count & 0xFF); i++, pIds += 2) {
        s32 id0 = (i == (selected & 0xFF)) ? (s32)pIds[0] + 0xD : (s32)pIds[0];

        g_Menu->pSelectionMenu->numCursors += func_8002675C(
            g_Menu->unk2DC, id0,
            (u8*)g_Menu->pSelectionMenu +
                g_Menu->pSelectionMenu->numCursors * 0x50,
            g_Menu->renderContext, 0xA0, 0x96, 0x1000);
        g_Menu->pSelectionMenu->numTexts += func_8002675C(
            g_Menu->unk2DC, pIds[1],
            (u8*)g_Menu->pSelectionMenu + 0x8C0 +
                g_Menu->pSelectionMenu->numTexts * 0x50,
            g_Menu->renderContext, 0xA0, 0x96, 0x1000);
    }
    g_Menu->pSelectionMenu->cursorsRenderCtx = (u8)g_Menu->renderContext;
    g_Menu->pSelectionMenu->textsRenderCtx = (u8)g_Menu->renderContext;
    func_801D1EE0(selected & 0xFF, 1);
    g_Menu->pManager->unk4 = 1;
}
#endif

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E8B4C);

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E8DA8);
#else
extern s32 SystemRenderStringEntry(void* pString, void* pWork, s32 height, s32 flag);
extern u8 D_801EA578[];   /* per-slot name-plate VRAM x coords (u16, migrated) */
extern u8 D_801EA5C4[];   /* per-slot name-plate VRAM y coords (u16, migrated) */

/* Arc A content: the party NAME-PLATE renderer.  Rasterizes the character's
 * name (two SystemRenderStringEntry rows) into a work buffer and uploads it
 * as a 0x28x0xD VRAM rect at the slot's plate coords (+0x180 x).  charArg
 * 0xFF uploads a blank plate.  The FACE TIMs are uploaded separately by the
 * resource-load (func_801C65F4); the quads that sample plate+face are the
 * portrait-frame geometry (func_801D5A50 family). */
void func_801E8DA8(s32 charArg, s32 slot) {
    u8* pWork = HeapAlloc(0x3F6, 0);
    RECT rect;
    s32 byteOff;

    bzero(pWork, 0x3F6);
    if ((charArg & 0xFF) != 0xFF) {
        u8* pName = (u8*)&g_GameState + ((charArg & 0xFF) >> 1) * 0x28;

        SystemRenderStringEntry(pName, pWork, 0x24, 0);
        SystemRenderStringEntry(pName + 0x14, pWork, 0x24, 1);
    }
    byteOff = (slot << 1) & 0x1FC;
    rect.x = (s16)(*(u16*)(D_801EA578 + byteOff) + 0x180);
    rect.y = (s16)*(u16*)(D_801EA5C4 + byteOff);
    rect.w = 0x28;
    rect.h = 0xD;
    LoadImage(&rect, (u_long*)pWork);
    DrawSync(0);
    HeapFree(pWork);
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E8EAC);
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E8F60);
#else
void func_801E8EAC(POLY_FT4* poly, s32 mode) {
    SetShadeTex(poly, 0);
    switch (mode & 0xFF) {
    case 0:
        SetSemiTrans(poly, 0);
        poly->r0 = poly->g0 = poly->b0 = 0x80;
        break;
    case 1:
        poly->tpage |= 0x20;
        SetSemiTrans(poly, 1);
        poly->r0 = poly->g0 = poly->b0 = 0x21;
        break;
    case 2:
        poly->r0 = poly->g0 = poly->b0 = 0x80;
        break;
    case 3:
        poly->r0 = poly->g0 = poly->b0 = 0x21;
        break;
    }
}

void func_801E8F60(s32 windowIndex, s32 dim) {
    MenuWindow* window = g_Menu->windows[windowIndex & 0xFF];
    s32 mode = (dim & 0xFF) ? 3 : 2;
    s32 i;

    i = 0;
    do {
        POLY_FT4* poly = &window->polysWindowBorderCorners[
            i * 2 + window->renderContext];
        i++;
        func_801E8EAC(poly, mode);
    } while (i < 4);
    i = 0;
    do {
        POLY_FT4* poly = &window->polysWindowBorderTop[
            i * 2 + window->renderContext];
        i++;
        func_801E8EAC(poly, mode);
    } while (i < 4);
    i = 0;
    do {
        POLY_FT4* poly = &window->polysWindowBorderBottom[
            i * 2 + window->renderContext];
        i++;
        func_801E8EAC(poly, mode);
    } while (i < 4);
    i = 0;
    do {
        POLY_FT4* poly = &window->polysWindowBorderLeft[
            i * 2 + window->renderContext];
        i++;
        func_801E8EAC(poly, mode);
    } while (i < 4);
    i = 0;
    do {
        POLY_FT4* poly = &window->polysWindowBorderRight[
            i * 2 + window->renderContext];
        i++;
        func_801E8EAC(poly, mode);
    } while (i < 4);
    i = 0;
    do {
        POLY_FT4* poly = &window->polysScrollBarEnds[
            i * 2 + window->renderContext];
        i++;
        func_801E8EAC(poly, mode);
    } while (i < 2);
    func_801E8EAC(&window->polysScrollBarEmpty[window->renderContext], mode);
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E91C4);
#else
/* Arc A verts: border-poly display fixup -- semi-transparent, shading ENABLED
 * (SetShadeTex 0) with the neutral 0x80 modulate color. */
void func_801E91C4(POLY_FT4* p) {
    SetSemiTrans(p, 1);
    SetShadeTex(p, 0);
    p->r0 = 0x80;
    p->g0 = 0x80;
    p->b0 = 0x80;
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E920C);
#else
/* Arc A portraits: set an FT4's screen quad + UV rect directly (absolute
 * screen coords -- no projection). */
void func_801E920C(POLY_FT4* p, s32 x, s32 y, s32 u, s32 v, s32 w, s32 h) {
    p->x0 = (s16)x;
    p->y0 = (s16)y;
    p->y1 = (s16)y;
    p->x2 = (s16)x;
    p->u0 = (u8)u;
    p->u2 = (u8)u;
    p->x1 = (s16)(x + w);
    p->y2 = (s16)(y + h);
    p->x3 = (s16)(x + w);
    p->y3 = (s16)(y + h);
    p->v0 = (u8)v;
    p->u1 = (u8)(u + w);
    p->v1 = (u8)v;
    p->v2 = (u8)(v + h);
    p->u3 = (u8)(u + w);
    p->v3 = (u8)(v + h);
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E927C);
#else
/* Arc A portraits: init the face quad -- opaque, shading enabled, neutral
 * 0x80 modulate. */
void func_801E927C(POLY_FT4* p) {
    SetPolyFT4(p);
    SetSemiTrans(p, 0);
    SetShadeTex(p, 0);
    p->r0 = 0x80;
    p->g0 = 0x80;
    p->b0 = 0x80;
}
#endif

extern s32 func_8002C3D8(void);
extern void func_8002A498(s32);
extern void ArchiveCdDataSync(s32);
extern void ArchiveCdSetMode(s32);
extern u8 D_801EA8F4[];

void func_801E92CC(void) {
    if (func_8002C3D8() == 0) {
        func_8002A498(0);
        ArchiveCdDataSync(0);
        ArchiveCdSetMode(0);
        ArchiveCdDataSync(0);
        Vsync(3);
        do {
            Vsync(3);
        } while (CdControlB(8, NULL, D_801EA8F4) == 0);
    }
}

void func_801E9340(char* path, void* pBuf, s32 size) {
    s32 fd = PCopen(path, 0, 0);
    PCread(fd, pBuf, size);
    PCclose(fd);
}

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E93A0);
