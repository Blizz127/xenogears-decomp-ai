#include "common.h"
#include "system/menu.h"
#include "main/game.h"

/* These title-overlay tables retain their retail 32-bit pointer slots inside
 * the native-inflated SystemMenu.  Keep access centralized so an 8-byte host
 * pointer cannot overwrite the adjacent menu field. */
static u8* MenuRawPointer(u32 offset);
static void MenuStoreRawPointer(u32 offset, void* pointer);

#ifdef XENO_PC_PORT
#include <stdio.h>
#include "../../../pc_port/src/krom_rom.h"
#include <stdlib.h>
#include <string.h>
#include <psx/kernel.h>
extern struct DIRENTRY* firstfile(char*, struct DIRENTRY*);
extern struct DIRENTRY* nextfile(struct DIRENTRY*);
#endif

extern s32 func_801C8D78(u8 port);

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

/* The retail fields at 0x42C and 0x440 are four-byte pointer slots.  Keep
 * their PSX width in the native-inflated SystemMenu and explicitly truncate
 * the port heap address, matching the established unk340 convention.
 * Shared with the matching build: func_801D14B0 / teardown paths call these. */
static ItemMenuWork* MenuItemWork(void) {
    return (ItemMenuWork*)(uintptr_t)g_Menu->unk42C[0];
}

static void MenuSetItemWork(ItemMenuWork* p) {
    g_Menu->unk42C[0] = (u32)(uintptr_t)p;
}

static void* MenuUnk440Pointer(void) {
    return (void*)(uintptr_t)*(u32*)&g_Menu->unk440[0];
}

#ifdef XENO_PC_PORT
extern s32 func_801D9808(void);
extern s32 func_801D9F98(s32, s32);
extern s32 func_801E23CC(void);
extern s32 func_801DE29C(s32, s32);
extern s32 func_801DBE54(void);
extern s32 func_801E0F78(u8, u8);
extern s32 func_801E2BE4(void);
extern void func_8001B970(void);
extern void func_801D1EB0(void);
extern void func_801D29A8(u8, u8);
extern void func_801E3088(s32);
extern void func_801D3674(void);
/* unprototyped: retail call sites use both the 3-arg shape and a 4-arg shape
 * whose trailing arg the retail definition ignores (see func_801E8018 below) */
extern void func_801E8018();
extern u8 func_801CACF8(u8, u8, u8);
extern void func_801E7C50(MenuString*, s32, s32, s32);
extern u8 D_801E96A4;
extern u8 D_801E977A;
extern u8 D_801E9784;
extern u8 D_801EA530[];

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
extern void PcPort_NotifyUnsupportedFileMenu(void);
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
#ifdef XENO_PC_PORT
    printf("[xeno-port][menu] func_801C55A0 input loop shouldDrawMenu=%d\n",
           (int)g_Menu->shouldDrawMenu);
    fflush(stdout);
#endif

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
            /* File still reaches the unimplemented native card screen
             * 801D9F98. Reject entry before nav teardown or card cleanup;
             * otherwise its stub return frees never-created card objects. */
            if (g_Menu->menu1Choice == 1) {
                PcPort_NotifyUnsupportedFileMenu();
                ok = 0;
            }
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
#ifdef XENO_PC_PORT
            printf("[xeno-port][menu] func_801C55A0 cancel/close choice=%d\n",
                   (int)g_Menu->menu1Choice);
            fflush(stdout);
#endif
            break;
        }
    }
}
#endif

extern u8 D_80059171;
extern u8 D_801E977A;
extern u8 D_801EA8FC;
extern u8 D_801E96A5;
extern u8 D_801E96A4;

void func_801C57A4(void) {
    u8 result = 1;
    D_80059171 = 1;
    D_801E977A = 0;
    func_801D1E80();
    {
        void* pMenu = g_Menu;
        if (*(u8*)((u8*)pMenu + 0x329) != 0) {
            while (*(u8*)((u8*)g_Menu + 0x329) != 0) {
                func_801C7BF4();
            }
        }
    }
    func_801D22F4(0);
    if (func_801CACF8(0x7D, 0xFF, 1) == 0) {
        if (D_801EA8FC == 0) goto skip;
    }
    if (func_801CACF8(0x80, 0xFF, 1) != 0) {
        result = 0;
    }
skip:
    func_801D2484();
    if (result) {
        D_801E96A5 = 1;
        D_801E96A4 = 1;
        func_801C531C(0);
        D_801E96A4 = 0;
        D_801E96A5 = 0;
    }
    {
        void* pMenu = g_Menu;
        ((u8*)g_Menu->pManager)[4] = 0;
        pMenu = g_Menu;
        ((u8*)g_Menu->pManager)[3] = 0;
    }
    func_801C8694(D_80059171);
}

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C58EC);
#else
extern void func_801E8474(s32, void*);
extern void func_801E8018();
extern void func_801D22C4(void);
extern void func_801E8044(s32, void*);
extern s32 func_801C531C(s32);
extern void func_801E8978(s32, s32, void*);
extern void func_801E8070(s32, void*, void*, void*, void*, s32, s32, s32);
extern int ArchiveGetDiscNumber(void);
extern u8 D_801EA1D4[];
extern u8 D_801EA530[];
extern u8 D_801E9E84[];
extern u8 D_801E9784;
extern u8 D_800594D0;

/* Title / New Game / Continue input loop (asm 0x801C58EC-0x801C5B50).
 * Opened when the field title map (490) hits FE57 → D_800ADB64=2 →
 * MenuExecute jtbl slot 2 → func_801C62A8 with D_80059460==2.
 * Choice wrap is 0..2; confirm calls func_801C531C(7) so:
 *   choice 0 → entry 7 (options stub)
 *   choice 1 → entry 8 (Continue: D_800594D0=2)
 *   choice 2 → entry 9 (New Game: func_8001B970)
 * Disc-1 idle timeout (unk2D8 >= 0x259) sets D_800594D0=1 and exits. */
void func_801C58EC(void) {
    s32 running = 1;

    printf("[xeno-port][menu] func_801C58EC title loop enter "
           "choice=%d D_80059460=%d\n",
           (int)g_Menu->menu1Choice, (int)D_80059460);
    fflush(stdout);

    func_801E8474(4, D_801EA1D4);
    func_801E8018(8, g_Menu->unk6E0, D_801EA530, (u8*)g_Menu->pManager + 0xC);
    g_Menu->unk2D8 = 0;

    while (1) {
        u8 input;

        func_801C7BF4();
        input = g_Menu->input;

#ifdef TITLE_CHAIN_MUTANT_TITLE_UP_IS_DOWN
        /* Deliberate mutant (pc_port/tests/run_title_newgame_chain.sh):
         * swap the two vertical nav codes so UP moves Continue -> Options. */
        if (input == 3) {
            input = 1;
        } else if (input == 1) {
            input = 3;
        }
#endif
        if (input == 3) {
            g_Menu->menu1Choice++;
            if ((u8)g_Menu->menu1Choice >= 3) {
                g_Menu->menu1Choice = 0;
            }
            g_Menu->unk2D8 = 0;
        } else if (input < 4) {
            if (input == 1) {
                if (g_Menu->menu1Choice == 0) {
                    g_Menu->menu1Choice = 2;
                } else {
                    g_Menu->menu1Choice--;
                }
                g_Menu->unk2D8 = 0;
            }
        } else if (input == 4) {
            g_Menu->pSelectionMenu->unk1192 = 1;
            func_801D22C4();
            func_801E8044(8, (u8*)g_Menu->pManager + 0xC);
            g_Menu->unk348->unk15B = 0x40;
            running = func_801C531C(7);
            D_801E9784 = 0;
            g_Menu->unk2D8 = 0;
            printf("[xeno-port][menu] title confirm choice=%d keep=%d "
                   "D_800594D0=%d\n",
                   (int)g_Menu->menu1Choice, (int)running, (int)D_800594D0);
            fflush(stdout);
        }

        if (g_Menu->menu1Choice != g_Menu->unk337) {
            func_801E8978(3, g_Menu->menu1Choice, D_801EA1D4);
            func_801E8070(8, g_Menu->unk6E0, D_801EA530, D_801E9E84,
                          (u8*)g_Menu->pManager + 0xC, g_Menu->menu1Choice, 0,
                          0);
            g_Menu->unk337 = g_Menu->menu1Choice;
        }

        if (ArchiveGetDiscNumber() == 1) {
            if (g_Menu->unk2D8 >= 0x259) {
                running = 0;
                D_800594D0 = 1;
                printf("[xeno-port][menu] title idle timeout -> D_800594D0=1\n");
                fflush(stdout);
            }
        }

        if ((running & 0xFF) == 0) {
            break;
        }
    }

    func_801E8044(8, (u8*)g_Menu->pManager + 0xC);
}
#endif

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

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C5FE4);
#else
extern void func_801D22C4(void);
extern void func_801D29A8(u8 open, u8 noSettle);
extern void func_801C7BF4(void);
extern void func_8003A094(void*);
extern void func_8003852C(void*);

/* Teardown after the root input loop: close-anim, drop draw flags, free the
 * init-slice allocations. Field names — SystemMenu is inflated on the host. */
void func_801C5FE4(void) {
    if (D_80059460 == 0) {
        func_801D22C4();
        func_801D29A8(0, 0);
        if (g_Menu->pManager != NULL) {
            g_Menu->pManager->unk5[1] = 0;
            g_Menu->pManager->unk5[0] = 0;
        }
        if (D_80059171 == 0) {
            D_800594CC = g_Menu->menu1Choice;
        }
    }
    func_801C7BF4();
    func_801C7BF4();
    g_Menu->shouldDrawMenu = 0;
    func_801C7BF4();
    while (g_Menu->renderContext != 0) {
        func_801C7BF4();
    }
    func_801C5BB8(0);
    func_801C5C1C(0);
    func_801C5C80(0);
    func_801C5CE4(0);
    func_801C5E10(0);
    HeapFree(g_Menu->pCursors);
    HeapFree(g_Menu->unk2DC);
    HeapFree(g_Menu->unk2E0);
    HeapFree(g_Menu->unk4E0[0].pVramBuffer);
    if (g_MenuDebugEnabled) {
        func_8003A094(g_Menu->unk2E4);
        func_801C7BF4();
        func_8003852C(g_Menu->unk2E4);
        func_801C7BF4();
        HeapFree(g_Menu->unk2E4);
    }
    if (D_80059460 == 0) {
        func_801C5B54(0);
        func_801C5D48(0);
        func_801C5DAC(0);
        func_801C5E74(0);
        HeapFree(g_Menu->windows[0]);
        HeapFree(g_Menu->windowParameters[0]);
        HeapFree(g_Menu->windows[1]);
        HeapFree(g_Menu->windowParameters[1]);
    } else if (D_80059460 == 2 || D_80059460 == 6) {
        func_801C5B54(0);
    }
    HeapFree(g_Menu);
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C62A8);
#else
void func_801C62A8(void) {
    u8 s0;
    func_801C5F10();
    func_801C7B0C();
    g_Menu->shouldDrawMenu = 1;
    g_Menu->unk32A = 1;
#ifdef XENO_PC_PORT
    printf("[xeno-port][menu] func_801C62A8 sel=%d shouldDrawMenu=%d\n",
           (int)D_80059460, (int)g_Menu->shouldDrawMenu);
    fflush(stdout);
#endif
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

void func_801C6400(void) {
    u8* pData;
    s32 i;
    s32 j;
    s32 pos;
    u8* buffer;
    u16 nameIndex;

    /* Native SystemMenu is inflated; PSX +0x32C is g_Menu->unk32C. */
    pData = (u8*)g_Menu->unk32C;
    if (pData == NULL) {
        return;
    }

    for (i = 0; i < 2; i++) {
        pData[i + 0x4F88] = 0;
        pData[i + 0x4F8A] = 0;
        pData[i + 0x4F8C] = 0xFF;
    }

    pData[0x4FE6] = 0;

    j = 0;
    for (i = 0; i < 0x20; i++) {
        pData[j + 0x58] = 0;
        pData[i + 0x4FAE] = 0xFF;
        j += 0x5C;
    }

    nameIndex = *(u16*)((u8*)&g_GameState + 0x1930);
    ArchiveSetIndex(0x10, 1);
    buffer = (u8*)HeapAlloc(ArchiveDecodeAlignedSize(1), 1);
    ArchiveReadFileToBuffer(1, buffer, 0, 0x80);
    ArchiveCdDataSync(0);

    pos = 0;
    if (nameIndex != 0) {
        u16 count = nameIndex;
        while (1) {
            u8 val = buffer[pos];
            if (val >= 0x80) {
                pos += 2;
                continue;
            }
            if (val == 0x0A) {
                count--;
                if ((count & 0xFFFF) == 0) {
                    pos++;
                    break;
                }
            }
            pos++;
        }
    }

    for (i = 0; i < 0x1E; i++) {
        pData[i + 0x4FFC] = buffer[pos + i];
    }
    pData[0x501A] = 0;
    pData[0x501B] = 0;

    ArchiveSetIndex(0x10, 0);
    HeapFree(buffer);
}

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
void func_801C6D5C(void) {
    SystemMenu* menu = g_Menu;
    menu->unk4CC[0xC] = 0;
    {
        SystemMenu* menu2 = g_Menu;
        *(u32*)&menu->unk4CC[0] = 0;
        *(u32*)&menu->unk4CC[4] = 0;
        menu2->unk4CC[0xD] = 0;
    }
    *(u32*)&g_Menu->unk4CC[8] = 0;
}
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

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C6F70);
#else
extern void func_801C8164(POLY_G4* p, u8 r, u8 g, u8 b);

/* B1b: init MenuUnk1 dim-overlay + highlight-bar primitives (both buffers).
 * Full-screen semi-trans POLY_F4 + DR_MODE (drawn by func_801D1258), and the
 * POLY_G4 / LINE_F3 shells that func_801D1EE0 positions on the selected row. */
void func_801C6F70(void) {
    RECT tw;
    MenuUnk1* p;
    s32 i;
    u16 tpage;

    tw.x = 0;
    tw.y = 0;
    tw.w = 0x100;
    tw.h = 0x100;
    func_801D22C4();

    p = g_Menu->unk348;
    for (i = 0; i < 2; i++) {
        func_801C8164(&p->polyG4s[i], 0x80, 0x80, 0);
        SetSemiTrans(&p->polyG4s[i], 1);

        SetLineF3(&p->lines1[i]);
        p->lines1[i].r0 = 0;
        p->lines1[i].g0 = 0x40;
        p->lines1[i].b0 = 0;

        SetLineF3(&p->lines2[i]);
        p->lines2[i].r0 = 0;
        p->lines2[i].g0 = 0x40;
        p->lines2[i].b0 = 0;

        SetPolyF4(&p->polysDimEffect[i]);
        p->polysDimEffect[i].x0 = 0;
        p->polysDimEffect[i].y0 = 0;
        p->polysDimEffect[i].x1 = 0x140;
        p->polysDimEffect[i].y1 = 0;
        p->polysDimEffect[i].x2 = 0;
        p->polysDimEffect[i].y2 = 0xE0;
        p->polysDimEffect[i].x3 = 0x140;
        p->polysDimEffect[i].y3 = 0xE0;
        p->polysDimEffect[i].r0 = 0x80;
        p->polysDimEffect[i].g0 = 0x80;
        p->polysDimEffect[i].b0 = 0x80;
        SetSemiTrans(&p->polysDimEffect[i], 1);

        tpage = (u16)GetTPage(0, 0, 0x140, 0x80);
        SetDrawMode(&p->drModes1[i], 0, 0, tpage, &tw);

        tpage = (u16)GetTPage(0, 2, 0x180, 0);
        SetDrawMode(&p->drawModeDimEffect[i], 0, 0, tpage, &tw);
    }
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C72BC);
#else
/* Retail resource dispatcher: Items, Abilities and Equip lifecycle modes.
 * Remaining modes retain an explicit unresolved boundary. */
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
    } else if (resourceMode == 3) {
        /* Retail 801C748C..801C7518. Native pointer members are widened;
         * the unnamed resource slots in unk8 remain packed 32-bit values. */
        g_Menu->unk330->pWeaponsData =
            LZSSHeapDecompress((void*)(uintptr_t)archive[2], 0);
        g_Menu->unk330->pAccessoriesData =
            LZSSHeapDecompress((void*)(uintptr_t)archive[3], 0);
        *(u32*)&g_Menu->unk330->unk8[0x10] = (u32)(uintptr_t)
            LZSSHeapDecompress((void*)(uintptr_t)archive[0x2B], 0);
        *(u32*)&g_Menu->unk330->unk8[0x0C] = (u32)(uintptr_t)
            LZSSHeapDecompress((void*)(uintptr_t)archive[0x14], 0);
    } else if (resourceMode == 0x13) {
        HeapFree(g_Menu->unk330->pWeaponsData);
        HeapFree(g_Menu->unk330->pAccessoriesData);
        HeapFree((void*)(uintptr_t)*(u32*)&g_Menu->unk330->unk8[0x10]);
        HeapFree((void*)(uintptr_t)*(u32*)&g_Menu->unk330->unk8[0x0C]);
    } else if (resourceMode == 7) {
        /* Retail 801C76FC..801C7788: Equip description banks into the
         * list work buffer at menu+0x434 (unk42C[2], PSX-width). */
        u8* listBuf = MenuRawPointer(0x434);
        u32 token;
        token = (u32)(uintptr_t)LZSSHeapDecompress(
            (void*)(uintptr_t)archive[0x35], 0);
        memcpy(listBuf + 0xA00, &token, 4);
        token = (u32)(uintptr_t)LZSSHeapDecompress(
            (void*)(uintptr_t)archive[0x36], 0);
        memcpy(listBuf + 0xA04, &token, 4);
        token = (u32)(uintptr_t)LZSSHeapDecompress(
            (void*)(uintptr_t)archive[0x37], 0);
        memcpy(listBuf + 0xA08, &token, 4);
        token = (u32)(uintptr_t)LZSSHeapDecompress(
            (void*)(uintptr_t)archive[0x38], 0);
        memcpy(listBuf + 0xA0C, &token, 4);
    } else if (resourceMode == 0x17) {
        /* Retail 801C7A54..801C7ACC: free the four description banks. */
        u8* listBuf = MenuRawPointer(0x434);
        u32 token;
        memcpy(&token, listBuf + 0xA00, 4);
        HeapFree((void*)(uintptr_t)token);
        memcpy(&token, listBuf + 0xA04, 4);
        HeapFree((void*)(uintptr_t)token);
        memcpy(&token, listBuf + 0xA08, 4);
        HeapFree((void*)(uintptr_t)token);
        memcpy(&token, listBuf + 0xA0C, 4);
        HeapFree((void*)(uintptr_t)token);
    } else {
        static u32 warnedModes;
        u32 bit = (resourceMode < 32) ? (1u << resourceMode) : 0;
        if (bit == 0 || (warnedModes & bit) == 0) {
            warnedModes |= bit;
            printf("[xeno-port][stub-path] func_801C72BC mode %u not ported "
                   "(ported: 0, 2, 3, 7, 0x10, 0x12, 0x13, 0x17)\n",
                   resourceMode);
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
extern void func_801C7F34(u32 ticks); /* Seven menu-owned time fields. */
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
    func_801C7F34((u32)D_80059488);
    func_801D2968();
    func_801D1CA0();
    s0 = (g_Menu->renderContext == 0);
    DrawSync(0);
    Vsync(0);
    PutDrawEnv(&g_Menu->pGfxEnv->drawEnv);
    PutDispEnv(&g_Menu->pGfxEnv->dispEnv);
    /* Retail: field snapshot at (704,256) (filled by func_800A4748 before
     * MenuMain) under the OT. PsyCross MoveImage materializes that rect into
     * the GL backbuffer when dest overlaps the active draw clip. */
    MoveImage(&g_Menu->pSelectionMenu->unk1180, 0, s0 * 224);
#ifdef XENO_PC_PORT
    {
        static int s_miDone;
        if (!s_miDone) {
            /* DIAGNOSTIC: CPU VRAM sample after backdrop blit. Remove once
             * title art source (Map 490 -> 704,256) is non-black. */
            extern unsigned short vram[];
            enum { VW = 1024 };
            int y0 = s0 * 224;
            unsigned long sum_dst = 0, sum_src = 0;
            int nz_dst = 0, nz_src = 0;
            int y, x;
            for (y = y0; y < y0 + 224; y += 8) {
                for (x = 0; x < 320; x += 8) {
                    unsigned short p = vram[y * VW + x];
                    sum_dst += (unsigned)(p & 0x7FFF);
                    if (p & 0x7FFF)
                        nz_dst++;
                }
            }
            for (y = 0x100; y < 0x100 + 224; y += 8) {
                for (x = 0x2C0; x < 0x2C0 + 320; x += 8) {
                    unsigned short p = vram[y * VW + x];
                    sum_src += (unsigned)(p & 0x7FFF);
                    if (p & 0x7FFF)
                        nz_src++;
                }
            }
            {
                /* Also sample horizon TIM page at (512,0) and FB at (0,0). */
                unsigned long sum_tex = 0, sum_fb0 = 0;
                int nz_tex = 0, nz_fb0 = 0;
                for (y = 0; y < 224; y += 8) {
                    for (x = 512; x < 512 + 256; x += 8) {
                        unsigned short p = vram[y * VW + x];
                        sum_tex += (unsigned)(p & 0x7FFF);
                        if (p & 0x7FFF)
                            nz_tex++;
                    }
                    for (x = 0; x < 320; x += 8) {
                        unsigned short p = vram[y * VW + x];
                        sum_fb0 += (unsigned)(p & 0x7FFF);
                        if (p & 0x7FFF)
                            nz_fb0++;
                    }
                }
                s_miDone = 1;
                printf("[xeno-port][menu] DIAG backdrop isbg=%d dest_nz=%d/%d "
                       "src704_nz=%d/%d dest_sum=%lu src_sum=%lu "
                       "tex512_nz=%d fb0_nz=%d tex_sum=%lu fb0_sum=%lu\n",
                       (int)g_Menu->pGfxEnv->drawEnv.isbg, nz_dst, 1120, nz_src,
                       1120, sum_dst, sum_src, nz_tex, nz_fb0, sum_tex, sum_fb0);
            }
            fflush(stdout);
        }
    }
#endif
    DrawOTag(&g_Menu->pGfxEnv->ot[15]);
    func_801C8BEC();
    func_801C8EE8();
}
#endif

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
#ifdef XENO_PC_PORT
int g_XenoMenuNavReaderTicks = 0;
#endif

void func_801C7D78(void) {
    s32 present = 1;
    s32 input = 0;
    s32 savedD59488;

#ifdef XENO_PC_PORT
    g_XenoMenuNavReaderTicks++;
#endif

    for (;;) {
        if (ControllerGetType(0) == 0) {
            if ((input & 0xFF) == 0) {
                SoundMuteAllSpuChannels();
                input += 1;
                savedD59488 = D_80059488;
            }
        } else {
            present -= 1;
            if ((input & 0xFF) != 0) {
                SoundEnableAllSpuChannels();
                D_80059488 = savedD59488;
            }
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

            if (pressed & 0x2000) { input = 0; func_801C8574(1); break; }
            if (pressed & 0x4000) { input = 1; func_801C8574(1); break; }
            if (pressed & 0x8000) { input = 2; func_801C8574(1); break; }
            if (pressed & 0x1000) { input = 3; func_801C8574(1); break; }
            released = g_C1ButtonStateReleased;
#ifdef TITLE_CHAIN_MUTANT_CONFIRM_ON_PRESS
            /* Deliberate mutant (pc_port/tests/run_title_newgame_chain.sh):
             * confirm on the Circle press edge instead of its release. */
            released = (u16)(released & ~0x20u) | (pressed & 0x20);
#endif
            if (released & 0x20) {                            /* CIRCLE */
                input = 4;
                func_801C8574(2);
                break;
            }
            if (released & 0x40) {                            /* CROSS */
                input = 5;
                func_801C8574(3);
                break;
            }
            if (released & 0x80) { input = 6; break; }
            if (released & 0x10) { input = 7; break; }
            if (pressed & 0x4)  { input = 0xA; func_801C8574(1); break; }
            if (pressed & 0x8)  { input = 9; func_801C8574(1); break; }
            if (released & 0x100) { input = 0xC; break; }
        }
    }
    g_Menu->input = (u8)input;
}

void func_801C7F34(
#ifdef XENO_PC_PORT
    u32 ticks) {
    /* Retail decomposes an unsigned 60 Hz count into seven word fields.
     * Preserve the full leading quotient; do not clamp it to one digit. */
    static const u32 divisors[7] = {
        21600000, 2160000, 216000, 36000, 3600, 600, 60
    };
    s32 i;
    for (i = 0; i < 7; i++) {
        u32 value = ticks / divisors[i];
        ticks %= divisors[i];
        memcpy(g_Menu->unk2EC + i * 4, &value, sizeof(value));
    }
#else
    s32 frames, u8* pOut) {
    s32 total = frames;
    s32 mins, secs, frac;
    mins = total / 14400;
    total -= mins * 14400;
    secs = total / 240;
    total -= secs * 240;
    frac = total;
    pOut[0] = (u8)(mins / 10);
    pOut[1] = (u8)(mins % 10);
    pOut[2] = (u8)(secs / 10);
    pOut[3] = (u8)(secs % 10);
    pOut[4] = (u8)(frac / 10);
    pOut[5] = (u8)(frac % 10);
#endif
}

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

extern void func_801C80B8(u32 value);

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

extern void func_801C851C(SVECTOR* verts, s32 x, s32 y, s32 w, s32 h);

#ifndef XENO_PC_PORT
extern void func_80039DB8(s32 packedId);

void func_801C8574(s32 soundId) {
    if (g_Menu->unk32A) {
        func_80039DB8(((s32)*(u16*)((u8*)g_Menu->unk2E4 + 0x14) << 16) |
                      (soundId & 0xFF));
    }
}
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

extern u16 D_801E96C8[];

s32 func_801C8640(s32 mask, s32 index) {
    return D_801E96C8[index & 0xFF] & mask;
}
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
#ifdef XENO_PC_PORT
    /* Retail 801C86CC sets the wait flag in the branch delay slot even when
     * transition state is already zero.  Only the animation wait is optional.
     * 801C8704 masks the input before adding one, so target 256 is retained. */
    s32 targetDisc = (u8)discNum + 1;
    u8 param = (u8)((u8)discNum * 3 - 0x7D);
    func_801D1E80();
    while (g_Menu->transitionEffectState != 0) {
        func_801C7BF4();
    }
    func_801D22F4(0);
    for (;;) {
        if (ArchiveGetDiscNumber() == targetDisc) break;
        func_801E92CC();
        func_801D2F4C(param);
        if (func_801E93A0(targetDisc) == 0) {
            func_801D32B4(0);
            break;
        }
        func_801D32B4(0);
        func_801D2F4C(0x89);
        for (s32 delay = 0x1D; delay != 0; --delay) {
            func_801C7BF4();
        }
        func_801D32B4(0);
        func_801C7BF4();
    }
    func_801D2484();
#else
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
#endif
}

void func_801C87C4(void) {
    UnDeliverEvent(0xF4000001, 0x4);
    UnDeliverEvent(0xF4000001, 0x8000);
    UnDeliverEvent(0xF4000001, 0x100);
    UnDeliverEvent(0xF4000001, 0x2000);
}

static u32 MenuCardEvent(s32 slot);

s32 func_801C881C(s32 port) {
    while (1) {
        if (TestEvent(MenuCardEvent(3)) == 1) {
            func_801C87C4();
            return 3;
        }
        if (TestEvent(MenuCardEvent(1)) == 1) {
            func_801C87C4();
            return 1;
        }
        if (TestEvent(MenuCardEvent(0)) == 1) {
            func_801C87C4();
            return 0;
        }
        if (TestEvent(MenuCardEvent(2)) == 1) {
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

/* BIOS event IDs are four-byte integers, not host pointers. These slots
 * live in the byte tail after TIM_IMAGE, which expands in native builds. */
static u32 MenuCardEvent(s32 slot) {
    u8* p = &g_Menu->unk32C->unk4F80[0x6C + slot * 4];
    return (u32)p[0] | ((u32)p[1] << 8) | ((u32)p[2] << 16) | ((u32)p[3] << 24);
}

static void MenuSetCardEvent(s32 slot, u32 event) {
    u8* p = &g_Menu->unk32C->unk4F80[0x6C + slot * 4];
    p[0] = (u8)event;
    p[1] = (u8)(event >> 8);
    p[2] = (u8)(event >> 16);
    p[3] = (u8)(event >> 24);
}

void func_801C8960(void) {
    func_801C7BF4();
    EnterCriticalSection();
    CloseEvent(MenuCardEvent(0));
    CloseEvent(MenuCardEvent(1));
    CloseEvent(MenuCardEvent(2));
    CloseEvent(MenuCardEvent(3));
    ExitCriticalSection();
}

extern s32 D_801EA900;

s32 func_801C8A10(u8 port) {
#ifdef XENO_PC_PORT
    extern s32 D_801EA904;
    MenuUnk2* card;
    s32 previous, result, stored;
    s32 i;
    g_Menu->unk32C->unk4F80[0x64 + port] = 1;
    result = func_801C891C((port != 0) << 4);
    card = g_Menu->unk32C;
    stored = result;
    /* 8A84..8A8C skips the normal status store: a newly present card
     * reports the transition internally but keeps persistent status zero. */
    if (result == 0) {
        memcpy(&previous, &card->unk4C94[0x2E0 + port * 4], sizeof(previous));
        if (previous == -1) {
            result = 1;
            stored = 0;
        }
    }
    memcpy(&card->unk4C94[0x2E0 + port * 4], &stored, sizeof(stored));
    if (result == -1) {
        card->unk4F80[0x0A + port] = 0;
        card->unk4F80[0x64 + port] = 0;
        if (port == 0) D_801EA900 = 0;
        else D_801EA904 = 0;
        for (i = 0; i < 16; ++i) {
            s32 index = port * 16 + i;
            card->unk4F80[0x2E + index] = 0xFF;
            card->unk4F80[0x0E + index] = 0;
            card->unk0[index * 0x5C + 0x58] = 0;
        }
    }
    if (card->unk4F80[0x68 + port] != card->unk4F80[0x64 + port]) {
        card->unk4F80[8 + port] = 0;
        card->unk4F80[0x68 + port] = card->unk4F80[0x64 + port];
    }
    return result != -2;
#else
    void* pMenu;
    void* pData;
    s32 result;
    s32 ret = 1;
    u8 portIdx = port;
    s32 off1;
    s32 i;

    pMenu = g_Menu;
    pData = *(void**)((u8*)pMenu + 0x32C);
    *(u8*)(pData + portIdx + 0x4FE4) = 1;

    result = func_801C891C((portIdx != 0) << 4);

    if (result == 0) {
        pMenu = g_Menu;
        pData = *(void**)((u8*)pMenu + 0x32C);
        if (*(s32*)(pData + portIdx * 4 + 0x4F74) == -1) {
            result = 1;
            *(s32*)(pData + portIdx * 4 + 0x4F74) = 0;
        }
    }

    pMenu = g_Menu;
    pData = *(void**)((u8*)pMenu + 0x32C);
    *(s32*)(pData + portIdx * 4 + 0x4F74) = result;

    if (result == -1) {
        off1 = portIdx << 4;
        pMenu = g_Menu;
        pData = *(void**)((u8*)pMenu + 0x32C);
        *(u8*)(pData + portIdx + 0x4F8A) = 0;
        pMenu = g_Menu;
        pData = *(void**)((u8*)pMenu + 0x32C);
        *(u8*)(pData + portIdx + 0x4FE4) = 0;
        D_801EA900 = 0;
        for (i = 0; i < 0x10; i++) {
            pMenu = g_Menu;
            pData = *(void**)((u8*)pMenu + 0x32C);
            *(u8*)(pData + off1 + i + 0x4FAE) = 0xFF;
            pMenu = g_Menu;
            pData = *(void**)((u8*)pMenu + 0x32C);
            *(u8*)(pData + off1 + i + 0x4F8E) = 0;
            pMenu = g_Menu;
            pData = *(void**)((u8*)pMenu + 0x32C);
            off1 += 0x3C;
            *(u8*)(pData + off1 + 0x58) = 0;
        }
    }

    if (result == -2) {
        ret = 0;
    }

    pMenu = g_Menu;
    pData = *(void**)((u8*)pMenu + 0x32C);
    if (*(u8*)(pData + portIdx + 0x4FE8) != *(u8*)(pData + portIdx + 0x4FE4)) {
        pMenu = g_Menu;
        pData = *(void**)((u8*)pMenu + 0x32C);
        *(u8*)(pData + portIdx + 0x4F88) = 0;
        pMenu = g_Menu;
        pData = *(void**)((u8*)pMenu + 0x32C);
        *(u8*)(pData + portIdx + 0x4FE8) = *(u8*)(pData + portIdx + 0x4FE4);
    }

    return ret;
#endif
}

extern u8 D_801E9779;

void func_801C8BEC(void) {
#ifdef XENO_PC_PORT
    s32 status;
    if (g_Menu->unk32C == NULL || g_Menu->unk32C->unk4F80[0x66] == 0) return;
    g_Menu->unk326 = (u8)(g_Menu->unk326 + 1);
    if (g_Menu->unk326 > D_801E9779) {
        func_801C8A10(0);
        func_801C8A10(1);
        /* Both polls may replace the menu; inspect and reset the current
         * owner, retaining the retail byte-counter wrap and strict test. */
        memcpy(&status, &g_Menu->unk32C->unk4C94[0x2E0], sizeof(status));
        if (status == -1) {
            memcpy(&status, &g_Menu->unk32C->unk4C94[0x2E4], sizeof(status));
            if (status == -1) g_Menu->unk334 = 0;
        }
        g_Menu->unk326 = 0;
    }
#else
    void* pMenu = g_Menu;
    void* pData = g_Menu->unk32C;
    if (pData != NULL && *(u8*)((u8*)pData + 0x4FE6) != 0) {
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
#endif
}

void func_801C8CA4(u8 idx) {
#ifdef XENO_PC_PORT
    s32 status;
    s32 i;
    memcpy(&status, &g_Menu->unk32C->unk4C94[0x2E0 + idx * 4], sizeof(status));
    if (status == -2) return;
    g_Menu->unk32C->unk4F80[0x66] = 2;
    for (i = 0; i < 59; ++i) func_801C7BF4();
    /* Menu pumping may replace g_Menu; clear the current owner's state. */
    g_Menu->unk32C->unk4F80[0x66] = 0;
#else
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
#endif
}

void func_801C8D1C(u8 idx) {
#ifdef XENO_PC_PORT
    s32 status;
    s32 i;
    memcpy(&status, &g_Menu->unk32C->unk4C94[0x2E0 + idx * 4], sizeof(status));
    if (status == -2) return;
    for (i = 0; i < 59; ++i) Vsync(0);
#else
    void* pMenu = g_Menu;
    void* pData = *(void**)((u8*)pMenu + 0x32C);
    s32 val = *(s32*)((u8*)pData + 0x4F74 + idx * 4);
    if (val != -2) {
        s32 i;
        for (i = 0x3B; i != 0; i--) {
            Vsync(0);
        }
    }
#endif
}

void func_801C8EE8(void) {
#ifdef XENO_PC_PORT
    u8 state;
    s32 port;
    if (g_Menu->unk32C == NULL) return;
    state = g_Menu->unk32C->unk4F80[0x66];
    if (state != 1 && state != 2) return;

    /* Retail 8EE8..9034 keeps the entry state but reloads menu ownership
     * and the next port's ready byte after each enumeration callback. */
    for (port = 0; port < 2; ++port) {
        if (g_Menu->unk32C->unk4F80[8 + port] == 0) {
            s32 count = func_801C8D78((u8)port);
            if (state == 1 && (u8)count != 0) g_Menu->unk334 = 1;
            g_Menu->unk32C->unk4F80[8 + port] = 1;
        }
    }
#else
    void* pMenu = g_Menu;
    void* pData = g_Menu->unk32C;
    u8 state;
    if (pData == NULL) {
        return;
    }
    state = *(u8*)((u8*)pData + 0x4FE6);
    if (state == 1) {
        if (*(u8*)((u8*)pData + 0x4F88) == 0) {
            if (func_801C8D78(0) != 0) {
                *(u8*)((u8*)g_Menu + 0x334) = 1;
            }
            *(u8*)(*(void**)((u8*)g_Menu + 0x32C) + 0x4F88) = 1;
        }
        pMenu = g_Menu;
        pData = *(void**)((u8*)pMenu + 0x32C);
        if (*(u8*)((u8*)pData + 0x4F89) == 0) {
            func_801C8D78(1);
            *(u8*)(*(void**)((u8*)g_Menu + 0x32C) + 0x4F89) = 1;
            *(u8*)((u8*)g_Menu + 0x334) = 1;
        }
    } else if (state == 2) {
        if (*(u8*)((u8*)pData + 0x4F88) == 0) {
            func_801C8D78(0);
            *(u8*)(*(void**)((u8*)g_Menu + 0x32C) + 0x4F88) = 1;
        }
        pMenu = g_Menu;
        pData = *(void**)((u8*)pMenu + 0x32C);
        if (*(u8*)((u8*)pData + 0x4F89) == 0) {
            func_801C8D78(1);
            *(u8*)(*(void**)((u8*)g_Menu + 0x32C) + 0x4F89) = 1;
        }
    }
#endif
}

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

extern char D_801C50A8[];
extern char D_801C50AC;
extern char D_801C50B0[];
extern char D_801C50B4;

void func_801C90B0(u8 port, u8 slot) {
#ifdef XENO_PC_PORT
    char pathBuf[0x48];
    char prefix[6];
    s32 index = port * 16 + slot;
    s32 blockOffset = index * 0x200;
    s32 i;

    /* Retail 90B0..926C: one read attempt, even on failure. The TIM_IMAGE
     * preceding the block data grows on the host; use native members. */
    memcpy(prefix, port == 0 ? D_801C50A8 : D_801C50B0, sizeof(prefix));
    strcpy(pathBuf, prefix);
    strcat(pathBuf, (char*)&g_Menu->unk32C->unk0[index * 0x5C + 0x18]);
    func_801C9038(pathBuf, &g_Menu->unk32C->unkB94[blockOffset]);

    for (i = 0; i < g_Menu->unk32C->unkB94[blockOffset + 3]; ++i) {
        u8* tail = g_Menu->unk32C->unk4F80;
        u32 writeIndex;
        memcpy(&writeIndex, &tail[4], sizeof(writeIndex));
        tail[0x2E + port * 16 + writeIndex] = (u8)index;
        ++writeIndex;
        memcpy(&tail[4], &writeIndex, sizeof(writeIndex));
    }
#else
    char pathBuf[0x58];
    char prefix[6];
    void* pMenu;
    void* pData;
    s32 retries = 1;
    u8 portU = port;
    u8 slotU = slot;
    s32 idx = portU * 16 + slotU;
    s32 dataOff = idx * 0x5C;
    s32 blockOff = idx * 0x200 + 0xB94;
    s32 result;
    s32 i;

    if (port == 0) {
        *(s32*)(prefix) = *(s32*)D_801C50A8;
        *(s16*)(prefix + 4) = *(s16*)D_801C50AC;
    } else {
        *(s32*)(prefix) = *(s32*)D_801C50B0;
        *(s16*)(prefix + 4) = *(s16*)D_801C50B4;
    }
    strcpy(pathBuf, prefix);
    pMenu = g_Menu;
    strcat(pathBuf, (char*)(*(void**)((u8*)pMenu + 0x32C) + dataOff + 0x18));

    do {
        pMenu = g_Menu;
        result = func_801C9038(pathBuf, *(void**)((u8*)pMenu + 0x32C) + blockOff);
        if (result != -1) break;
        retries--;
        if (retries == 0) break;
        func_801C8CA4(portU);
    } while (1);

    pMenu = g_Menu;
    pData = *(void**)((u8*)pMenu + 0x32C);
    if (*(u8*)(pData + blockOff + 3) > 0) {
        for (i = 0; i < *(u8*)(pData + blockOff + 3); i++) {
            pMenu = g_Menu;
            pData = *(void**)((u8*)pMenu + 0x32C);
            {
                s32 off = portU * 16;
                s32 writeIdx = *(s32*)(pData + 0x4F84);
                *(u8*)(pData + off + writeIdx + 0x4FAE) = (u8)(slotU + off);
                *(s32*)(pData + 0x4F84) = writeIdx + 1;
            }
        }
    }
#endif
}

extern u8 D_801EA6D0[];
extern s32 D_801EA6F4;
extern s32 D_801EA900;
extern s32 D_801EA904;

s32 func_801C8D78(u8 port) {
#ifdef XENO_PC_PORT
    char path[6];
    struct DIRENTRY entry;
    u8 count = 0;
    s32 port16 = port * 16;
    s32 i;

    /* 8D78..8EE4: per-port status, one enumeration, exact entry-pointer
     * success test, and byte count returned to the caller. */
    if (port == 0) D_801EA900 = 0;
    else D_801EA904 = 0;
    for (i = 0; i < 16; ++i) {
        g_Menu->unk32C->unk4F80[0x2E + port16 + i] = 0xFF;
        D_801EA6D0[port16 + i] = 0;
    }
    memcpy(path, port == 0 ? D_801C50A8 : D_801C50B0, sizeof(path));
    if (firstfile(path, &entry) == &entry) {
        do {
            strcpy((char*)&g_Menu->unk32C->unk0[(port16 + count) * 0x5C + 0x18], entry.name);
            ++count;
        } while (nextfile(&entry) == &entry);
    }
    g_Menu->unk32C->unk4F80[0x0A + port] = count;
    return count;
#else
    char pathBuf[0x38];
    char entryBuf[0x28];
    void* pMenu;
    void* pData;
    u8 portU = port;
    s32 port16 = portU << 4;
    u8* pTable = D_801EA6D0 + port16;
    s32 retries = 5;
    u8 count = 0;
    s32 i;
    void* pResult;

    D_801EA900 = 0;

    for (i = 0; i < 0x10; i++) {
        pMenu = g_Menu;
        pData = *(void**)((u8*)pMenu + 0x32C);
        *(u8*)(pData + port16 + i + 0x4FAE) = 0xFF;
        pTable[i] = 0;
    }

    if (portU == 0) {
        *(s32*)(pathBuf) = *(s32*)D_801C50A8;
        *(s16*)(pathBuf + 4) = *(s16*)D_801C50AC;
        retries--;
    } else {
        *(s32*)(pathBuf) = *(s32*)D_801C50B0;
        *(s16*)(pathBuf + 4) = *(s16*)D_801C50B4;
        retries--;
    }

    if (retries != 0) {
        pResult = firstfile(pathBuf, entryBuf);
        if (pResult == (void*)entryBuf) {
            do {
                s32 idx = port16 + count;
                s32 off = idx * 0x5C;
                pMenu = g_Menu;
                strcpy((char*)(*(void**)((u8*)pMenu + 0x32C) + off + 0x18), entryBuf);
                count++;
                pResult = nextfile(entryBuf);
            } while (pResult == (void*)entryBuf);
        }
        pMenu = g_Menu;
        pData = *(void**)((u8*)pMenu + 0x32C);
        *(u8*)(pData + portU + 0x4F8A) = count;
    }
    return count;
#endif
}

void func_801C9270(u8 port) {
    void* pMenu;
    void* pData;
    s32 i, j;
    u8* pTable;
    for (i = 0; i < 0x10; i++) {
        pMenu = g_Menu;
        pData = *(void**)((u8*)pMenu + 0x32C);
        *(u8*)(pData + port * 0x10 + i + 0x4F8E) = 0;
    }
    pTable = D_801EA6D0 + port * 0x10;
    for (i = 0; i < 0xF; i++) {
        u8 charIdx;
        u8* pSlot;
        u8 match;
        pMenu = g_Menu;
        pData = *(void**)((u8*)pMenu + 0x32C);
        charIdx = *(u8*)(pData + port * 0x10 + i + 0x4FAE);
        pSlot = pData + charIdx * 0x28;
        match = 1;
        for (j = 0; j < 0xC; j++) {
            if (*(u8*)(pSlot + 0x18 + j) != *(u8*)(pData + port * 0x10 + i + 0x4FCE)) {
                match = 0;
                break;
            }
        }
        if (match) {
            pMenu = g_Menu;
            *(u8*)(*(void**)((u8*)pMenu + 0x32C) + port * 0x10 + i + 0x4F8E) = 1;
            pMenu = g_Menu;
            pData = *(void**)((u8*)pMenu + 0x32C);
            {
                u8 idx = *(u8*)(pData + port * 0x10 + i + 0x4FAE);
                s32 offset = idx * 0x200 + 0xB94;
                D_801EA6F4 = (s32)(pData + offset + 0x100);
                *(u8*)(pTable + *(u8*)(pData + offset + 0x123)) = 1;
            }
        }
    }
}

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C93A8);
#endif

extern u32 D_801E981C[];

s32 func_801C9BCC(s32 mode) {
    void* pMenu = g_Menu;
    void* pData = *(void**)((u8*)pMenu + 0x32C);
    s32 selector = *(s32*)((u8*)pData + 0x4F7C);
    s32 result = 1;
    if (mode == 0) {
        u32 flags = *(u32*)((u8*)pData + 0x4F88);
        if (!(flags & 0xFFFF0000)) {
            result = 0;
        } else {
            s32 offset = D_801E981C[selector];
            s32 group = (offset < 0) ? (offset + 0xF) : offset;
            group >>= 4;
            if (*(u8*)(pData + group + 0x4FE4) == 0) {
                result = 0;
            } else if (*(u8*)(pData + offset + 0x4FAE) == 0xFF) {
                result = 0;
            }
        }
    } else if (mode == 1) {
        u32 flags = *(u32*)((u8*)pData + 0x4F88);
        if (!(flags & 0xFFFF0000)) {
            result = 0;
        } else {
            s32 offset = D_801E981C[selector];
            s32 group = (offset < 0) ? (offset + 0xF) : offset;
            group >>= 4;
            if (*(u8*)(pData + group + 0x4FE4) == 0) {
                result = 0;
            } else if (*(u8*)(pData + offset + 0x4FAE) == 0xFF) {
                if (*(u8*)(pData + offset + 0x4F8E) == 0) {
                    result = 0;
                }
            }
        }
    } else if (mode == 2) {
        s32 offset = D_801E981C[selector];
        s32 group = (offset < 0) ? (offset + 0xF) : offset;
        group >>= 4;
        if (*(u8*)(pData + group + 0x4FE4) == 0) {
            result = 0;
        }
    }
    if (result) {
        pMenu = g_Menu;
        ((u8*)g_Menu->pManager)[0x4D8] = 2;
    }
    return result;
}

s32 func_801C9D34(s32 mode) {
    void* pMenu;
    void* pData;
    s32 result = 0xFF;
    s32 found = 1;
    s32 limit = 0x1E;
    s32 start;
    s32 i;

    pMenu = g_Menu;
    pData = *(void**)((u8*)pMenu + 0x32C);
    start = (*(u8*)(pData + 0x4FE4) == 0) ? 0xF : 0;
    if (*(u8*)(pData + 0x4FE5) == 0) {
        limit = 0xF;
    }

    for (i = start; i < limit && found; i++) {
        s32 offset = D_801E981C[i];
        s32 group = (offset < 0) ? (offset + 0xF) : offset;
        group >>= 4;

        pMenu = g_Menu;
        pData = *(void**)((u8*)pMenu + 0x32C);

        if (mode == 0) {
            if (!(*(u32*)((u8*)pData + 0x4F88) & 0xFFFF0000)) {
                found = 0;
                break;
            }
            if (*(u8*)(pData + group + 0x4FE4) == 0) continue;
            if (*(u8*)(pData + offset + 0x4FAE) == 0xFF) continue;
            result = i;
            found = 0;
        } else if (mode == 1) {
            if (!(*(u32*)((u8*)pData + 0x4F88) & 0xFFFF0000)) {
                found = 0;
                break;
            }
            if (*(u8*)(pData + group + 0x4FE4) == 0) continue;
            if (*(u8*)(pData + offset + 0x4FAE) == 0xFF) continue;
            if (*(u8*)(pData + offset + 0x4F8E) == 0) continue;
            result = i;
            found = 0;
        } else if (mode == 2) {
            if (*(u8*)(pData + group + 0x4FE4) == 0) continue;
            result = i;
            found = 0;
        }
    }

    pMenu = g_Menu;
    ((u8*)g_Menu->pManager)[0x4D8] = 2;
    return result;
}

#ifdef XENO_PC_PORT
extern u32 D_801E9820[];
void func_801C9EF4(s32 mode, s32 selected) {
    MenuUnk2* card;
    s32 next, probe;
    if (mode < 0 || mode > 2) return;
    card = g_Menu->unk32C;
    next = (s32)((u32)selected + 3);
    if (mode == 2) {
        /* Retail reads this presence/state byte before the range branch. */
        if (card->unk4F80[0x64 + next / 15] == 0) return;
        if (next < 30) {
            card->unk4F7C = next;
        } else {
            probe = (s32)((u32)card->unk4F7C + 3);
            next = (s32)((u32)card->unk4F7C + 4);
            if (probe < 30 && next < 30) card->unk4F7C = next;
        }
        return;
    }
    /* D_801E9828 aliases D_801E9820 + two four-byte entries. */
    for (probe = selected; next < 30;
         probe = (s32)((u32)probe + 3), next = (s32)((u32)next + 3)) {
        u32 slot = D_801E9820[probe + 2];
        if (card->unk4F80[0x2E + slot] != 0xFF &&
            (mode == 0 || card->unk4F80[0x0E + slot] != 0)) {
            card->unk4F7C = next;
            return;
        }
    }
    probe = (s32)((u32)card->unk4F7C + 3);
    next = (s32)((u32)card->unk4F7C + 4);
    if (probe >= 30) return;
    for (; next < 30; probe = (s32)((u32)probe + 1), next = (s32)((u32)next + 1)) {
        u32 slot = D_801E9820[probe];
        if (card->unk4F80[0x2E + slot] != 0xFF &&
            (mode == 0 || card->unk4F80[0x0E + slot] != 0)) {
            card->unk4F7C = next;
            return;
        }
    }
}
#else
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801C9EF4);
#endif

#ifdef XENO_PC_PORT
void func_801CA1D4(s32 mode, s32 selected) {
    MenuUnk2* card;
    s32 next, probe;
    if (mode < 0 || mode > 2) return;
    card = g_Menu->unk32C;
    next = (s32)((u32)selected - 3);
    if (mode == 2) {
        /* Retail signed division truncates -3..-1 to zero before testing
         * the candidate range. Do not clamp or skip the presence read. */
        if (card->unk4F80[0x64 + next / 15] == 0) return;
        if (next >= 0) {
            card->unk4F7C = next;
        } else {
            probe = (s32)((u32)card->unk4F7C - 3);
            next = (s32)((u32)card->unk4F7C - 4);
            if (probe >= 0 && next >= 0) card->unk4F7C = next;
        }
        return;
    }
    /* Retail D_801E9810[selected] equals D_801E981C[selected-3];
     * fallback D_801E9818[probe] likewise equals the candidate entry. */
    for (; next >= 0; next = (s32)((u32)next - 3)) {
        u32 slot = D_801E981C[next];
        if (card->unk4F80[0x2E + slot] != 0xFF &&
            (mode == 0 || card->unk4F80[0x0E + slot] != 0)) {
            card->unk4F7C = next;
            return;
        }
    }
    probe = (s32)((u32)card->unk4F7C - 3);
    next = (s32)((u32)card->unk4F7C - 4);
    if (probe < 0) return;
    for (; next >= 0; next = (s32)((u32)next - 1)) {
        u32 slot = D_801E981C[next];
        if (card->unk4F80[0x2E + slot] != 0xFF &&
            (mode == 0 || card->unk4F80[0x0E + slot] != 0)) {
            card->unk4F7C = next;
            return;
        }
    }
}
#else
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CA1D4);
#endif

extern u32 D_801E9820[];

void func_801CA480(s32 direction, s32 startIdx, s32 limit) {
#ifdef XENO_PC_PORT
    MenuUnk2* card;
    s32 next;
    (void)limit; /* Retail replaces a2 with startIdx+1 before using it. */
    if (direction < 0 || direction > 2) return;
    next = (s32)((u32)startIdx + 1);
    if (direction == 2) {
        card = g_Menu->unk32C;
        if (card->unk4F80[0x64 + next / 15] != 0 && next < 30)
            card->unk4F7C = next;
        return;
    }
    if (next >= 30) return;
    card = g_Menu->unk32C;
    for (; next < 30; next = (s32)((u32)next + 1)) {
        u32 slot = D_801E981C[next];
        if (card->unk4F80[0x2E + slot] != 0xFF &&
            (direction == 0 || card->unk4F80[0x0E + slot] != 0)) {
            card->unk4F7C = next;
            return;
        }
    }
#else
    void* pMenu = g_Menu;
    void* pData = *(void**)((u8*)pMenu + 0x32C);
    s32 idx;
    if (direction == 0) {
        if (limit >= 0x1E) return;
        for (idx = startIdx; limit < 0x1E; limit++) {
            u32 offset = D_801E9820[limit];
            if (*(u8*)(pData + offset + 0x4FAE) != 0xFF) {
                *(s32*)(pData + 0x4F7C) = limit;
                return;
            }
        }
    } else if (direction == 1) {
        idx = startIdx - 1;
        if (idx < 0) return;
        for (; idx >= 0; idx--) {
            u32 offset = D_801E9820[idx];
            if (*(u8*)(pData + offset + 0x4FAE) != 0xFF) {
                *(s32*)(pData + 0x4F7C) = idx;
                return;
            }
        }
    } else if (direction == 2) {
        idx = startIdx + 1;
        if (idx < 0x1E) {
            u32 offset;
            u8 entryByte;
            pMenu = g_Menu;
            pData = *(void**)((u8*)pMenu + 0x32C);
            offset = D_801E9820[idx / 15];
            entryByte = *(u8*)(pData + offset + 0x4FAE);
            if (entryByte == 0) {
                if (idx < 0x1E) {
                    *(s32*)(pData + 0x4F7C) = idx;
                }
            }
        }
    }
#endif
}

extern u32 D_801E9818[];

void func_801CA5F0(s32 direction, s32 startIdx, s32 limit) {
#ifdef XENO_PC_PORT
    MenuUnk2* card;
    s32 next;
    (void)limit; /* Retail replaces a2 with startIdx-1 before using it. */
    if (direction < 0 || direction > 2) return;
    next = (s32)((u32)startIdx - 1);
    if (direction == 2) {
        card = g_Menu->unk32C;
        if (card->unk4F80[0x64 + next / 15] != 0 && next >= 0)
            card->unk4F7C = next;
        return;
    }
    if (next < 0) return;
    card = g_Menu->unk32C;
    for (; next >= 0; next = (s32)((u32)next - 1)) {
        u32 slot = D_801E981C[next];
        if (card->unk4F80[0x2E + slot] != 0xFF &&
            (direction == 0 || card->unk4F80[0x0E + slot] != 0)) {
            card->unk4F7C = next;
            return;
        }
    }
#else
    void* pMenu;
    void* pData;
    s32 idx;
    if (direction == 0) {
        if (limit < 0) return;
        pMenu = g_Menu;
        pData = *(void**)((u8*)pMenu + 0x32C);
        for (idx = startIdx - 1; idx >= 0; idx--) {
            u32 offset = D_801E9818[idx];
            if (*(u8*)(pData + offset + 0x4FAE) != 0xFF) {
                *(s32*)(pData + 0x4F7C) = limit;
                return;
            }
        }
    } else if (direction == 1) {
        idx = startIdx - 1;
        if (idx < 0) return;
        pMenu = g_Menu;
        pData = *(void**)((u8*)pMenu + 0x32C);
        for (; idx >= 0; idx--) {
            u32 offset = D_801E9818[idx];
            if (*(u8*)(pData + offset + 0x4FAE) != 0xFF) {
                if (*(u8*)(pData + offset + 0x4F8E) != 0) {
                    *(s32*)(pData + 0x4F7C) = idx;
                    return;
                }
            }
            idx--;
            if (idx < 0) return;
        }
    } else if (direction == 2) {
        idx = startIdx - 1;
        pMenu = g_Menu;
        pData = *(void**)((u8*)pMenu + 0x32C);
        {
            s32 group = idx / 15;
            u32 offset = D_801E9818[group];
            if (*(u8*)(pData + offset + 0x4FE4) != 0 && idx >= 0) {
                *(s32*)(pData + 0x4F7C) = idx;
            }
        }
    }
#endif
}

#ifndef XENO_PC_PORT
INCLUDE_RODATA("../asm/menu/nonmatchings/main/misc", D_801C50A8);
INCLUDE_RODATA("../asm/menu/nonmatchings/main/misc", D_801C50AC);
INCLUDE_RODATA("../asm/menu/nonmatchings/main/misc", D_801C50B0);
INCLUDE_RODATA("../asm/menu/nonmatchings/main/misc", D_801C50B4);
INCLUDE_RODATA("../asm/menu/nonmatchings/main/misc", D_801C50B8);
#endif

extern void func_801C9EF4(s32 direction, s32 selection);
extern void func_801CA1D4(s32 direction, s32 selection);
#ifdef XENO_PC_PORT
extern void func_801E781C(s32 screenId, u8 animFlag);
#endif

s32 func_801CA750(s32 direction, s32 unused, s32 limit) {
#ifdef XENO_PC_PORT
    s32 result = 0;
    s32 previous;
    MenuUnk2* card;
    (void)unused;
    switch (g_Menu->input) {
    case MENU_INPUT_CONFIRM: result = 1; break;
    case MENU_INPUT_BACK: result = 2; break;
    case MENU_INPUT_RIGHT:
        func_801C9EF4(direction, g_Menu->unk32C->unk4F7C);
        break;
    case MENU_INPUT_LEFT:
        func_801CA1D4(direction, g_Menu->unk32C->unk4F7C);
        break;
    case MENU_INPUT_DOWN:
        func_801CA480(direction, g_Menu->unk32C->unk4F7C, limit);
        break;
    case MENU_INPUT_UP:
        func_801CA5F0(direction, g_Menu->unk32C->unk4F7C, limit);
        break;
    }
    card = g_Menu->unk32C;
    memcpy(&previous, card->unk4F80, sizeof(previous));
    if (card->unk4F7C != previous) {
        u32 slot = D_801E981C[card->unk4F7C];
        func_801E781C(card->unk4F80[0x2E + slot], card->unk4F80[0x0E + slot]);
        /* The refresh callback may change both ownership and selection. */
        card = g_Menu->unk32C;
        memcpy(card->unk4F80, &card->unk4F7C, sizeof(card->unk4F7C));
    }
    return result;
#else
    s32 result = 0;
    u8* data;
    s32 selection;

    switch (*((u8*)g_Menu + 0x325)) {
    case 4:
        result = 1;
        break;
    case 5:
        result = 2;
        break;
    case 0:
        func_801C9EF4(direction, *(s32*)((u8*)g_Menu->unk32C + 0x4F7C));
        break;
    case 2:
        func_801CA1D4(direction, *(s32*)((u8*)g_Menu->unk32C + 0x4F7C));
        break;
    case 1:
        func_801CA480(direction, *(s32*)((u8*)g_Menu->unk32C + 0x4F7C), limit);
        break;
    case 3:
        func_801CA5F0(direction, *(s32*)((u8*)g_Menu->unk32C + 0x4F7C), limit);
        break;
    }

    data = (u8*)g_Menu->unk32C;
    selection = *(s32*)(data + 0x4F7C);
    if (selection != *(s32*)(data + 0x4F80)) {
        u8* entry = data + D_801E981C[selection];

        func_801E781C(entry[0x4FAE], entry[0x4F8E]);
        data = (u8*)g_Menu->unk32C;
        *(s32*)(data + 0x4F80) = *(s32*)(data + 0x4F7C);
    }
    return result;
#endif
}

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CA8C0);
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CAA38);
#else
extern u8 D_801E9778;

/* Retail 801CAA38..801CACF4. The card bytes are within the unchanged byte
 * tail of the native-inflated MenuUnk2, not at g_Menu->unk32C + 0x4FE4. */
u32 func_801CAA38(u8 interactive) {
    u8 card0 = g_Menu->unk32C->unk4F80[0x64];
    u8 card1 = g_Menu->unk32C->unk4F80[0x65];
    u8 remaining = 0xB4;
    u32 selected = 0;
    s32 running = 1;

    if (D_801E977A != 0) {
        g_Menu->unk32C->unk4F80[0x66] = 2;
    }
    D_801EA8FC = 0;
    while (running) {
        if (interactive == 0) {
            g_Menu->pCursors->shouldRender[2] = 0;
            g_Menu->pCursors->shouldRender[3] = 0;
            if (g_Menu->input != MENU_INPUT_IDLE) break;
            if (--remaining == 0) break;
        }
        func_801C7BF4();
        if (interactive != 0 && D_801E977A != 0) {
            if (card0 != g_Menu->unk32C->unk4F80[0x64]) {
                g_Menu->unk32C->unk4F80[8] = 0;
                g_Menu->input = MENU_INPUT_BACK;
                D_801E9778 = 1;
            }
            if (card1 != g_Menu->unk32C->unk4F80[0x65]) {
                g_Menu->unk32C->unk4F80[9] = 0;
                g_Menu->input = MENU_INPUT_BACK;
                D_801E9778 = 1;
            }
        }
        switch (g_Menu->input) {
        case MENU_INPUT_CONFIRM:
            running = 0;
            break;
        case MENU_INPUT_BACK:
            selected = 0;
            D_801EA8FC = 1;
            running = 0;
            break;
        case MENU_INPUT_LEFT:
            g_Menu->pCursors->shouldRender[2] = 1;
            g_Menu->pCursors->shouldRender[3] = 0;
            selected = 1;
            break;
        case MENU_INPUT_RIGHT:
            g_Menu->pCursors->shouldRender[2] = 0;
            g_Menu->pCursors->shouldRender[3] = 1;
            selected = 0;
            break;
        }
    }
    g_Menu->pCursors->shouldRender[2] = 0;
    g_Menu->pCursors->shouldRender[3] = 0;
    g_Menu->unk32C->unk4F80[0x66] = 0;
    return selected;
}
#endif

extern void func_801D2F4C(u8);
extern u32 func_801CAA38(u8);
extern s32 func_801D32B4(s32);

u8 func_801CACF8(u8 arg0, u8 arg1, u8 arg2) {
    u32 result;
    func_801D2F4C(arg0);
    g_Menu->pCursors->shouldRender[3] = 1;
    result = func_801CAA38(arg2);
    func_801D32B4(result);
    if (arg1 != 0xFF && (result & 0xFF) != 0) {
        func_801D2F4C(arg1);
        g_Menu->pCursors->shouldRender[3] = 1;
        result = func_801CAA38(arg2);
        func_801D32B4(result);
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

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CAE08);
#endif

#ifndef XENO_PC_PORT
extern void func_80033B34(u8*, u8*, s32);
#else
extern void func_80033B34(u16*, u8*, s32);  /* true sig: system/system.c */
#endif

void func_801CB184(void) {
    u8* pState = (u8*)&g_GameState;
    s32 entry;
    for (entry = 0; entry < 0x26C; entry += 0x14) {
        u8 srcBuf[0x18];
        u8 dstBuf[0x28];
        u8* pSrc = srcBuf;
        u8* pDst = dstBuf;
        u8* pEntry = pState + entry;
        s32 i;
        s32 count = 0;
        for (i = 0; i < 0x14; i += 2) {
            u8 v1 = pEntry[i];
            u8 v2 = pEntry[i + 1];
            *pSrc++ = v1;
            *pDst++ = v2;
            if (v1 == 0 && v2 == 0) break;
            count += 2;
        }
        func_80033B34(srcBuf, dstBuf, (count + (count >> 31)) >> 1);
        {
            u8* pOut = dstBuf;
            u8* pDest = pState + entry;
            s32 j;
            for (j = 0; j < 0x14; j++) {
                *pDest++ = *pOut++;
            }
        }
    }
}

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

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CB304);
#endif

u8 func_801CB8AC(u8 arg0) {
    void* pMenu = g_Menu;
    void* pData = *(void**)((u8*)pMenu + 0x32C);
    u8 savedE4 = *(u8*)((u8*)pData + 0x4FE4);
    u8 savedE5 = *(u8*)((u8*)pData + 0x4FE5);
    u8 param = (u8)((arg0 & 0xFF) * 3 + 0x29);
    u8 result = 0;
    func_801D2F4C(param);
    pMenu = g_Menu;
    *(u8*)((u8*)pMenu + 0x325) = 8;
    pMenu = g_Menu;
    *(u8*)(*(void**)((u8*)pMenu + 0x32C) + 0x4FE6) = 2;
    pMenu = g_Menu;
    {
        void* pManager = *(void**)((u8*)pMenu + 0x32C);
        if (*(u8*)((u8*)pMenu + 0x325) == 8) {
            s32 i;
            for (i = 0; i < 0x80; i++) {
                func_801C7BF4();
                pMenu = g_Menu;
                pData = *(void**)((u8*)pMenu + 0x32C);
                if (savedE4 != *(u8*)((u8*)pData + 0x4FE4) ||
                    savedE5 != *(u8*)((u8*)pData + 0x4FE5)) {
                    result = 1;
                    break;
                }
                if (*(u8*)((u8*)pMenu + 0x325) != 8) break;
            }
        }
    }
    func_801D32B4(0);
    if (result == 0) {
        result = func_801CACF8(0x2F, 0xFF, 1);
    }
    return result;
}

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

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CBA4C);
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CBD90);
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CC6D8);
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CD2AC);
#endif

extern void func_801CC6D8(void);
extern void func_801CD2AC(void);
extern void func_801CBD90(u8);
extern void func_801CB304(void);
extern void func_801D2484(void);

s32 func_801CD710(u8 arg0) {
    void* pMenu;
    s32 result = 1;
    func_801D22F4(0);
    pMenu = g_Menu;
    ((u8*)g_Menu->pManager)[0x2F] = 0;
    {
        u8 state = *(u8*)((u8*)g_Menu + 0x338);
        u8* pData = (u8*)g_Menu;
        switch (state) {
            case 0:
                *(u8*)(pData + 0x334) = 1;
                func_801CD2AC();
                break;
            case 1:
                *(u8*)(pData + 0x334) = 6;
                func_801CC6D8();
                break;
            case 2:
                if (D_80059460 == 2) {
                    *(u8*)(pData + 0x334) = 2;
                    func_801CB304();
                } else {
                    *(u8*)(pData + 0x334) = 3;
                    func_801CBD90(arg0);
                }
                break;
        }
    }
    func_801D2484();
    return result;
}

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CD81C);
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CDB1C);
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CDC6C);

extern void func_801CD81C(MenuCharacter* panel, u8 charId, u8 slot,
                          s32* xTable, s32* yTable, u8 mode);
extern void func_801CDB1C(MenuCharacter* panel, u8 charId, u8 slot,
                          s32* xTable, s32* yTable);
extern void func_801CDC6C(MenuCharacter* panel, u8 charId, u8 slot,
                          s32* xTable, s32* yTable, u8 mode);

void func_801CE0CC(MenuCharacter* panel, u8 charId, u8 slot,
                   s32* xTable, s32* yTable, u8 mode) {
    func_801CD81C(panel, charId, slot, xTable, yTable, mode);
    func_801CDB1C(panel, charId, slot, xTable, yTable);
    func_801CDC6C(panel, charId, slot, xTable, yTable, mode);
    panel->unkBE7 = 1;
    panel->renderContext = *(u8*)&g_Menu->renderContext;
}
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
void func_801CE198(s32 count, SVECTOR* vertices, POLY_FT4* polys,
                   s32 renderContext) {
    s32 i;

    for (i = 0; i < count; i++) {
        long interpolated;
        long flag;

        RotTransPers4(&vertices[i * 4], &vertices[i * 4 + 1],
                      &vertices[i * 4 + 2], &vertices[i * 4 + 3],
                      (long*)&polys[renderContext + i * 2].x0,
                      (long*)&polys[renderContext + i * 2].x1,
                      (long*)&polys[renderContext + i * 2].x2,
                      (long*)&polys[renderContext + i * 2].x3,
                      &interpolated, &flag);
        AddPrim(&g_Menu->pGfxEnv->ot[4], &polys[renderContext + i * 2]);
    }
}
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

extern void func_801CE198(s32 count, SVECTOR* vertices, POLY_FT4* polys,
                          s32 renderContext);

#ifndef XENO_PC_PORT
void func_801CE2B4(s32 count, u8* pList, s32 renderCtx) {
    s32 i;

    for (i = 0; i < count; i++) {
        AddPrim(&g_Menu->pGfxEnv->ot[4],
                pList + (renderCtx + i * 2) * sizeof(POLY_FT4));
    }
}
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

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CE660);
#else
/* Retail 0x801CE660: submit the twelve counted lists owned by menu buffer
 * 0x358.  The third list has its own render context at 0x2AEE; every other
 * list uses the shared context at 0x2AE0. */
void func_801CE660(void) {
    u8* data;

    if (!g_Menu->pManager || !((u8*)g_Menu->pManager)[7]) {
        return;
    }

    data = MenuRawPointer(0x358);
    func_801CE198(1, (SVECTOR*)(data + 0x1EA0),
                  (POLY_FT4*)data, data[0x2AE0]);
    func_801CE198(1, (SVECTOR*)(data + 0x1EC0),
                  (POLY_FT4*)(data + 0x50), data[0x2AE0]);
    func_801CE198(data[0x2AED], (SVECTOR*)(data + 0x2AA0),
                  (POLY_FT4*)(data + 0x1E00), data[0x2AEE]);
    func_801CE198(data[0x2AEC], (SVECTOR*)(data + 0x1EE0),
                  (POLY_FT4*)(data + 0xA0), data[0x2AE0]);
    func_801CE198(data[0x2AE7], (SVECTOR*)(data + 0x2780),
                  (POLY_FT4*)(data + 0x1630), data[0x2AE0]);
    func_801CE198(data[0x2AE8], (SVECTOR*)(data + 0x2820),
                  (POLY_FT4*)(data + 0x17C0), data[0x2AE0]);
    func_801CE198(data[0x2AE9], (SVECTOR*)(data + 0x28C0),
                  (POLY_FT4*)(data + 0x1950), data[0x2AE0]);
    func_801CE198(data[0x2AEA], (SVECTOR*)(data + 0x2960),
                  (POLY_FT4*)(data + 0x1AE0), data[0x2AE0]);
    func_801CE198(data[0x2AE1], (SVECTOR*)(data + 0x2300),
                  (POLY_FT4*)(data + 0xAF0), data[0x2AE0]);
    func_801CE198(data[0x2AE3], (SVECTOR*)(data + 0x23C0),
                  (POLY_FT4*)(data + 0xCD0), data[0x2AE0]);
    func_801CE198(data[0x2AE5], (SVECTOR*)(data + 0x25C0),
                  (POLY_FT4*)(data + 0x11D0), data[0x2AE0]);
    func_801CE198(data[0x2AEB], (SVECTOR*)(data + 0x2A00),
                  (POLY_FT4*)(data + 0x1C70), data[0x2AE0]);
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CE860);
#else
/* Retail 801CE860..801CEB5C: project and queue the seven Equip-row glyph
 * strips from the 0x35C work buffer.  When preview flag 0x32F2 is set, also
 * submit the comparison (delta) strips.
 *
 * Bar POLY_G4s are double-buffered at stride 0x24 inside a per-row slot of
 * 0x48 (D8644's polyRow).  Retail increments those bases each loop
 * (801CEB28/801CEB38).  Using only renderContext*0x24 AddPrims the same G4
 * seven times and cycles the OT. */
void func_801CE860(void) {
    s32 i;
    long interpolated;
    long flag;
    u8* data = MenuRawPointer(0x35C);

    for (i = 0; i < 7; i++) {
        u8* row = data + i;
        u8* verts;
        u8* poly;
        u8 ctx;

        if (row[0x32EA] == 0) {
            continue;
        }

        if (data[0x32F2] != 0) {
            verts = data + 0x31E0 + i * 0x20;
            ctx = row[0x32E3];
            poly = data + 0x2228 + i * 0x48 + ctx * 0x24;
            RotTransPers4((SVECTOR*)(verts + 0x00), (SVECTOR*)(verts + 0x08),
                          (SVECTOR*)(verts + 0x10), (SVECTOR*)(verts + 0x18),
                          (long*)(poly + 0x08), (long*)(poly + 0x10),
                          (long*)(poly + 0x18), (long*)(poly + 0x20),
                          &interpolated, &flag);
            AddPrim(&g_Menu->pGfxEnv->ot[4], poly);
            func_801CE198(row[0x32CE], (SVECTOR*)(data + 0x2D80 + i * 0x80),
                          (POLY_FT4*)(data + 0x1770 + i * 0x140),
                          row[0x32D5]);
        }

        verts = data + 0x3100 + i * 0x20;
        ctx = row[0x32DC];
        poly = data + 0x2030 + i * 0x48 + ctx * 0x24;
        RotTransPers4((SVECTOR*)(verts + 0x00), (SVECTOR*)(verts + 0x08),
                      (SVECTOR*)(verts + 0x10), (SVECTOR*)(verts + 0x18),
                      (long*)(poly + 0x08), (long*)(poly + 0x10),
                      (long*)(poly + 0x18), (long*)(poly + 0x20),
                      &interpolated, &flag);
        AddPrim(&g_Menu->pGfxEnv->ot[4], poly);
        func_801CE198(row[0x32C0], (SVECTOR*)(data + 0x2A00 + i * 0x80),
                      (POLY_FT4*)(data + 0xEB0 + i * 0x140), row[0x32C7]);
    }
}
#endif

extern void func_801CE860(void);

void func_801CEB5C(void) {
    void* pMenu = g_Menu;
    void* pManager = g_Menu->pManager;
    if (*(u8*)((u8*)pManager + 8) != 0) {
        void* pData = MenuRawPointer(0x35C);
        u8 val1 = *(u8*)((u8*)pData + 0x32F3);
        u8 val2 = *(u8*)((u8*)pData + 0x32F1);
        func_801CE198(val1, (u8*)pData + 0x2420, pData, val2);
        func_801CE860();
    }
}

void func_801CEBB4(void) {
    void* pMenu = g_Menu;
    void* pManager = g_Menu->pManager;
    if (*(u8*)((u8*)pManager + 0x4B) != 0) {
        s32 i;
        for (i = 0; i < 5; i++) {
            pMenu = g_Menu;
            {
                void* pData = MenuRawPointer(0x360);
                if (*(u8*)((u8*)pData + 0x294 + i) != 0) {
                    func_801CE198(1, (u8*)pData + i * 0x80 + 0x50,
                                  (u8*)pData + i * 0x80,
                                  *(u8*)((u8*)pData + 0x299));
                }
            }
        }
    }
}

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
    void* pManager = g_Menu->pManager;
    if (*(u8*)((u8*)pManager + 0xA) != 0) {
        void* pData = *(void**)((u8*)pMenu + 0x354);
        func_801CE2B4(*(s32*)((u8*)pData + 0x1404), (u8*)pData + 0x500, *(u8*)((u8*)pData + 0x1409));
        pMenu = g_Menu;
        pData = *(void**)((u8*)pMenu + 0x354);
        func_801CE2B4(*(s32*)((u8*)pData + 0x1400), (u8*)pData, *(u8*)((u8*)pData + 0x1408));
    }
}

/* Retail g_Menu+off is a 4-byte pointer slot.  Native SystemMenu inflates, so
 * raw byte offsets land in unrelated fields (e.g. +0x434 hits unk2E4, +0x360
 * hits unk220).  Map the known PSX-width slots onto their struct members. */
static u32* MenuPsxPointerSlot(u32 retailOffset) {
    switch (retailOffset) {
    case 0x340:
        return (u32*)&g_Menu->unk340[0];
    case 0x344:
        return (u32*)&g_Menu->unk340[4];
    case 0x34C:
        return (u32*)&g_Menu->unk34C[0];
    case 0x358:
        return (u32*)&g_Menu->unk358[0];
    case 0x35C:
        return (u32*)&g_Menu->unk358[4];
    case 0x360:
        return (u32*)&g_Menu->unk358[8];
    case 0x42C:
        return &g_Menu->unk42C[0];
    case 0x430:
        return &g_Menu->unk42C[1];
    case 0x434:
        return &g_Menu->unk42C[2];
    case 0x438:
        return &g_Menu->unk42C[3];
    case 0x440:
        return (u32*)&g_Menu->unk440[0];
    case 0x44C:
        return (u32*)&g_Menu->unk44C[0];
    default:
        if (retailOffset >= 0x3A8 && retailOffset < 0x3C8 &&
            ((retailOffset - 0x3A8) & 3) == 0) {
            /* Title string slots: retail unk39C + 0xC + i*4. */
            return (u32*)&g_Menu->unk39C[0xC + (retailOffset - 0x3A8)];
        }
        return (u32*)((u8*)g_Menu + retailOffset);
    }
}

static u8* MenuRawPointer(u32 offset) {
    return (u8*)(uintptr_t)*MenuPsxPointerSlot(offset);
}

static void MenuStoreRawPointer(u32 offset, void* pointer) {
    *MenuPsxPointerSlot(offset) = (u32)(uintptr_t)pointer;
}

static u8* MenuTitleSlot(s32 index) {
    return MenuRawPointer(0x3A8 + index * 4);
}

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CF37C);
#else
void func_801CF37C(void) {
    u8* data = (u8*)g_Menu->unk32C;
    s32 i;
    if (data == NULL || ((u8*)g_Menu)[0x4D8] != 2) return;
    for (i = 0; i < 32; i++) {
        s32 selected = *(s32*)(data + 0x4F7C);
        u8 wanted = data[D_801E981C[selected] + 0x4FAE];
        u8 current = data[0x4FAE + i];
        if (current != wanted) continue;
        if (current == 0xFF && i != D_801E981C[selected]) continue;
        {
            u8* slot = MenuTitleSlot(i);
            if (slot == NULL) continue;
            u8* line = slot + 0xB0 + g_Menu->renderContext * 24;
            long p;
            long flag;
            line[4] = ((u8*)g_Menu)[0x4D4];
            line[5] = ((u8*)g_Menu)[0x4D4];
            line[6] = ((u8*)g_Menu)[0x4D4];
            RotTransPers4((SVECTOR*)(slot + 0xE0), (SVECTOR*)(slot + 0xE8),
                          (SVECTOR*)(slot + 0xF0), (SVECTOR*)(slot + 0xF8),
                          (long*)(line + 0x08), (long*)(line + 0x0C),
                          (long*)(line + 0x10), (long*)(line + 0x14),
                          &p, &flag);
            AddPrim(&g_Menu->pGfxEnv->ot[4], line);
            AddPrim(&g_Menu->pGfxEnv->ot[4],
                    slot + 0x140 + g_Menu->renderContext * 12);
        }
    }
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CF5E4);
#else
void func_801CF5E4(s32 first, s32 slotIndex, s32 limit) {
    u8* data = (u8*)g_Menu->unk32C;
    s32 i;
    s32 slot = slotIndex;
    for (i = first; i < limit; i++) {
        u8* entry = data + i * 0x5C;
        s32 s3;
        if (entry[0x58] == 0) continue;
        for (s3 = 0; s3 < data[i * 0x200 + 0xB97]; s3++, slot++) {
            u8* dst = MenuTitleSlot(slot);
            u8* poly = dst + g_Menu->renderContext * 40;
            s32 value = i * 0x10;
            u8 source = entry[(*(s32*)((u8*)g_Menu + 0x4CC)) * 4];
            poly[0x0C] = (u8)value;
            poly[0x0D] = source;
            poly[0x14] = (u8)(value + 0x10);
            poly[0x15] = source;
            poly[0x1C] = (u8)value;
            poly[0x1D] = (u8)(source + 0x10);
            poly[0x24] = (u8)(value + 0x10);
            poly[0x25] = (u8)(source + 0x10);
            *(u16*)(poly + 0x0E) = GetClut(value, (i >> 4) + 0x1C1);
            func_801CE198(1, (SVECTOR*)(dst + 0xE0), dst, g_Menu->renderContext);
        }
    }
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CF8D8);
#else
void func_801CF8D8(void) {
    u8* data = (u8*)g_Menu->unk32C;
    s32 i;
    if (data == NULL || ((u8*)g_Menu)[0x4D8] == 0) return;
    for (i = 0; i < 2; i++) {
        s32 selected = *(s32*)(data + 0x4F7C);
        s32 group = selected < 0 ? (selected + 15) >> 4 : selected >> 4;
        if (!data[0x4FE4 + i] || ((u8*)g_Menu->pManager)[0x68] == 0) continue;
        {
            s32 glyph = (((u8*)g_Menu->pManager)[0x2F] && i != group)
                            ? 0x115 : 0x122;
            s32 count = func_8002675C(g_Menu->unk2DC, glyph,
                                      data + 0x4D94 + i * 0xF0,
                                      g_Menu->renderContext, 0x1E + i * 0x90, 0x36, 0x1000);
            count = func_8002675C(g_Menu->unk2DC, i + 0x162,
                      data + 0x4DE4 + i * 0xF0, g_Menu->renderContext,
                      0x1B + i * 0x90, 0x36, 0x1000);
            {
                s32 j;
                for (j = 0; j < count; j++) {
                    AddPrim(&g_Menu->pGfxEnv->ot[4],
                            data + 0x4D94 + i * 0xF0 + 0x50 +
                            (g_Menu->renderContext + j * 2) * 40);
                }
                AddPrim(&g_Menu->pGfxEnv->ot[4],
                        data + 0x4D94 + i * 0xF0 + g_Menu->renderContext * 40);
            }
        }
    }
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CFB48);
#else
void func_801CFB48(void) {
    u8* data = (u8*)g_Menu->unk32C;
    s32 i;
    if (data == NULL || !((u8*)g_Menu)[0x4D8]) return;
    {
        u8 menuState = ((u8*)g_Menu)[0x4D8];
    for (i = 0; i < 32; i++) {
        u8* slot = MenuTitleSlot(i);
        s32 group = i >> 4;
        if (!data[0x4FE4 + group] || (group << 4) == i - 15) continue;
        if (data[0x4FAE + i] != data[D_801E981C[*(s32*)(data + 0x4F7C)] + 0x4FAE]) continue;
        {
            u8* firstPoly = slot + 0x50 + g_Menu->renderContext * 24;
            u8* secondPoly = slot + 0x80 + g_Menu->renderContext * 24;
            s32 special = menuState == 2 && data[0x4FAE + i] == 0xFF &&
                          D_801E981C[*(s32*)(data + 0x4F7C)] == i;
            firstPoly[4] = special ? 0xFF : 0;
            firstPoly[5] = special ? 0 : 0xFF;
            firstPoly[6] = 0;
            secondPoly[4] = firstPoly[4];
            secondPoly[5] = firstPoly[5];
            secondPoly[6] = 0;
        }
        {
            long p;
            long flag;
        RotTransPers3((SVECTOR*)(slot + 0x100), (SVECTOR*)(slot + 0x108),
                      (SVECTOR*)(slot + 0x118),
                      (long*)(slot + 0x58 + g_Menu->renderContext * 24),
                      (long*)(slot + 0x5C + g_Menu->renderContext * 24),
                      (long*)(slot + 0x60 + g_Menu->renderContext * 24), &p, &flag);
        }
        AddPrim(&g_Menu->pGfxEnv->ot[4], slot + 0x50 + g_Menu->renderContext * 24);
        {
            long p;
            long flag;
        RotTransPers3((SVECTOR*)(slot + 0x120), (SVECTOR*)(slot + 0x130),
                      (SVECTOR*)(slot + 0x138),
                      (long*)(slot + 0x88 + g_Menu->renderContext * 24),
                      (long*)(slot + 0x8C + g_Menu->renderContext * 24),
                      (long*)(slot + 0x90 + g_Menu->renderContext * 24), &p, &flag);
        }
        AddPrim(&g_Menu->pGfxEnv->ot[4], slot + 0x80 + g_Menu->renderContext * 24);
    }
    }
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801CFF64);
#else
void func_801CFF64(void) {
    MenuManager* manager = g_Menu->pManager;
    u8* arrows = MenuRawPointer(0x44C);
    u8 state;
    u8 index;
    u8* arrow;
    if (!manager || !arrows) return;
    state = ((u8*)manager)[0x52];
    if (!state) return;
    index = arrows[0x7B8];
    arrow = arrows + index * 24;
    *(u16*)(arrow + 8) = 0x20;
    *(u16*)(arrow + 0xA) = 0x61;
    *(u16*)(arrow + 0xC) = *(u16*)(arrows + 0x7B0) + 0x20;
    *(u16*)(arrow + 0xE) = 0x61;
    *(u16*)(arrow + 0x10) = 0x20;
    *(u16*)(arrow + 0x12) = 0x68;
    *(u16*)(arrow + 0x14) = *(u16*)(arrows + 0x7B0) + 0x20;
    *(u16*)(arrow + 0x16) = 0x68;
    if ((*(u32*)(arrows + 0x7B0) % 6) < 4 || state == 2) {
        func_801CE2B4(2, arrows + 0x3F0, index);
    }
    func_801CE2B4(0xC, arrows + 0x30, index);
    AddPrim(&g_Menu->pGfxEnv->ot[4], arrow);
    if (state == 1) {
        *(u32*)(arrows + 0x7B0) += *(u32*)(arrows + 0x7B4);
        if (*(u32*)(arrows + 0x7B0) >= 0x101) *(u32*)(arrows + 0x7B0) = 0;
    }
}
#endif

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

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D02D8);
#else
void func_801D02D8(void) {
    u8* data = MenuRawPointer(0x34C);
    s32 i;
    if (!g_Menu->pManager || !g_Menu->pManager->unkB) return;
    if (!data) return;
    if (!data[0x2DBC]) {
        func_801CE2B4(0x20, data, g_Menu->renderContext);
        {
            MenuUnk2* manager = g_Menu->unk32C;
            s32 selected = manager->unk4F7C;
            s32 wanted = manager->unk4F80[D_801E981C[selected] + 0x2E];
            s32 animation;
            u16 clut;
            memcpy(&animation, g_Menu->unk4CC, sizeof(animation));
            u8 source = manager->unk0[wanted * 0x5C + animation * 4];
            data = MenuRawPointer(0x34C);
            u8* poly = data + g_Menu->renderContext * 40;
            s32 value = wanted << 4;
            poly[0xA0C] = (u8)value;
            poly[0xA0D] = source;
            poly[0xA14] = (u8)(value + 15);
            poly[0xA15] = source;
            poly[0xA1C] = (u8)value;
            poly[0xA1D] = (u8)(source + 15);
            poly[0xA24] = (u8)(value + 15);
            poly[0xA25] = (u8)(source + 15);
            clut = GetClut(value, (wanted >> 4) + 0x1C1);
            /* Retail 801D067C..06A4 reloads owner, context and destination
             * after GetClut; do not retain the pre-call packet pointer. */
            data = MenuRawPointer(0x34C);
            poly = data + g_Menu->renderContext * 40;
            *(u16*)(poly + 0xA0E) = clut;
            AddPrim(&g_Menu->pGfxEnv->ot[4], data + 0xA00 + g_Menu->renderContext * 40);
        }
    } else {
        for (i = 0; i < 3; i++) {
            static const u16 countOffsets[] = {0x1312,0x1308,0x1309,0x130A,0x130B,0x130C,0x130D};
            static const u16 listOffsets[] = {0x50,0x320,0x410,0x500,0x5F0,0x6E0,0x780};
            s32 list;
            data = MenuRawPointer(0x34C); /* detail: next character owner */
            u8* meta = data + i * 0x87C;
            u8* slot = meta + 0xA98;
            if (!meta[0x1310]) continue;
            /* Retail reloads the owner before every list call, but checks
             * the character-enable byte only once at the start of its run. */
            for (list = 0; list < 7; list++) {
                data = MenuRawPointer(0x34C); /* detail: list owner */
                meta = data + i * 0x87C;
                slot = meta + 0xA98;
                func_801CE2B4(meta[countOffsets[list]], slot + listOffsets[list],
                             meta[list == 0 ? 0x130E : 0x130F]);
            }
            /* Retail 801D0844..54 / 801D087C..90 select 40-byte FT4s,
             * not 24-byte packets, for both saved context markers. */
            /* 801D083C loads the first quad's marker from the base panel,
             * before 801D0840 adds this character's primitive offset. */
            data = MenuRawPointer(0x34C); /* detail: first quad owner */
            slot = data + i * 0x87C + 0xA98;
            AddPrim(&g_Menu->pGfxEnv->ot[4], slot + data[0x130F] * sizeof(POLY_FT4));
            data = MenuRawPointer(0x34C); /* detail: name quad owner */
            meta = data + i * 0x87C;
            slot = meta + 0xA98;
            AddPrim(&g_Menu->pGfxEnv->ot[4], slot + 0x820 + meta[0x1311] * sizeof(POLY_FT4));
        }
        data = MenuRawPointer(0x34C); /* detail: summary owner */
        func_801CE2B4(0xB, data + 0x240C, data[0x130F]);
        data = MenuRawPointer(0x34C); /* detail: suffix owner */
        func_801CE2B4(4, data + 0x2C7C, data[0x130F]);
        data = MenuRawPointer(0x34C); /* detail: strip owner */
        func_801CE2B4(0x10, data + 0x277C, data[0x130F]);
    }
    data = MenuRawPointer(0x34C); /* Retail shared tail reload at 801D0918. */
    AddPrim(&g_Menu->pGfxEnv->ot[4], data + 0xA50 + g_Menu->renderContext * 36);
}
#endif

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

void func_801D0D90(void) {
    s32 i;
    u32 offset = 0x4E0;
    for (i = 0; i < 4; i++) {
        /* Host Menu is inflated — do not use raw PSX 0x33C for pManager. */
        MenuManager* pManager = g_Menu->pManager;
        if (pManager != NULL && pManager->unk34[i] != 0) {
            u8 idx = *(u8*)((u8*)g_Menu + i * 0x80 + 0x55D);
            u8* pOT = *(u8**)((u8*)g_Menu + 0x1D4) + 0x80;
            u8* pPrim = (u8*)g_Menu + offset + idx * 0x28;
            AddPrim(pOT, pPrim);
        }
        offset += 0x80;
    }
}

s32 func_801D0E20(void) {
    s32 i;
    for (i = 6; i >= 0; i--) {}
    return i + 1;
}

void func_801D0E38(void) {
    s32 i;
    for (i = 0; i < 6; i++) {
        void* pMenu = g_Menu;
        MenuManager* pManager = g_Menu->pManager;
        if (pManager != NULL && pManager->unk14[i] != 0) {
            u8* pSlot = (u8*)pMenu + i * 0x80;
            if (*(u8*)(pSlot + 0xB5F) != 0) {
                u8* pData = (u8*)pMenu + i * 0x80 + 0xAE0;
                func_801CE198(1, pData + 0x50, pData, *(u8*)(pSlot + 0xB5D));
            }
        }
    }
}

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
    /* unk14E0 is a packed retail MenuString bank (0x80/slot).  Native
     * SystemMenu inflates earlier members, so g_Menu+0x14E0 is NOT
     * &unk14E0[0] — always index the member. */
    for (i = 0; i < 6; i++) {
        if (g_Menu->pManager != NULL &&
            ((u8*)g_Menu->pManager)[0x40 + i] != 0) {
            u8* pData = &g_Menu->unk14E0[i * 0x80];
            func_801CE198(1, (SVECTOR*)(pData + 0x50), (POLY_FT4*)pData,
                          pData[0x7D]);
        }
    }
}

void func_801D0FD4(void) {
    void* pMenu = g_Menu;
    void* pManager = g_Menu->pManager;
    if (*(u8*)((u8*)pManager + 0x4E) != 0) {
        u8 idx = *(u8*)((u8*)pMenu + 0x185D);
        u8* pOT = *(u8**)((u8*)pMenu + 0x1D4) + 0x80;
        u8* pPrim = (u8*)pMenu + 0x17E0 + idx * 0x28;
        AddPrim(pOT, pPrim);
    }
}

void func_801D1030(void) {
    if (g_Menu->pManager->unk2E != 0) {
        s32 i;
        for (i = 0; i < 3; i++) {
            MenuString* string = g_Menu->unk1DE0[i];
            if (string->unk7F != 0) {
                func_801CE198(1, string->vertices, string->polys,
                              string->renderContext);
            } else {
                AddPrim(&g_Menu->pGfxEnv->ot[4],
                        &string->polys[string->renderContext]);
            }
        }
    }
}

void func_801D10DC(void) {
    s32 i;
    for (i = 0; i < 6; i++) {
        void* pMenu = g_Menu;
        void* pManager = g_Menu->pManager;
        if (*(u8*)((u8*)pManager + 0x54 + i) != 0) {
            u8* pSlot = (u8*)pMenu + i * 0x80;
            if (*(u8*)(pSlot + 0x195F) != 0) {
                u8* pData = (u8*)pMenu + i * 0x80 + 0x18E0;
                func_801CE198(1, pData + 0x50, pData, *(u8*)(pSlot + 0x195D));
            }
        }
    }
}

void func_801D1160(void) {
    s32 i;
    u32 offset = 0x1BE0;
    for (i = 0; i < 4; i++) {
        void* pMenu = g_Menu;
        void* pManager = g_Menu->pManager;
        if (*(u8*)((u8*)pManager + 0x5C + i) != 0) {
            u8 idx = *(u8*)((u8*)pMenu + i * 0x80 + 0x1C5D);
            u8* pOT = *(u8**)((u8*)pMenu + 0x1D4) + 0x80;
            u8* pPrim = (u8*)pMenu + offset + idx * 0x28;
            AddPrim(pOT, pPrim);
        }
        offset += 0x80;
    }
}

#ifndef XENO_PC_PORT
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
    /* Semi-trans full-screen dim + its DR_MODE (built by func_801C6F70). */
    AddPrim(&g_Menu->pGfxEnv->ot[4],
            &g_Menu->unk348->polysDimEffect[g_Menu->renderContext]);
    AddPrim(&g_Menu->pGfxEnv->ot[4],
            &g_Menu->unk348->drawModeDimEffect[g_Menu->renderContext]);
}

#ifndef XENO_PC_PORT
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

void func_801D1464(void) {
    if (g_Menu->pManager->scrollHandleActive != 0) {
        MenuScrollBarHandle* handle = g_Menu->pScrollHandle;
        func_801CE198(1, handle->vertices, handle->polys, handle->renderContext);
    }
}

void func_801D14B0(void) {
    void* pManager = g_Menu->pManager;
    if (*(u8*)((u8*)pManager + 0x53) != 0) {
        /* menu+0x440 is a PSX-width slot; use the field helper, not a raw
         * +0x440 peek into the inflated SystemMenu. */
        void* pData = MenuUnk440Pointer();
        func_801CE198(4, (u8*)pData + 0x140, pData, *(u8*)((u8*)pData + 0x1C0));
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

void func_801D17C4(void) {
    void* pManager = g_Menu->pManager;
    if (*(u8*)((u8*)pManager + 0x4C) != 0) {
        s32 i;
        u32 offset = 0x400;
        for (i = 0; i < 8; i++) {
            u8* pData = MenuRawPointer(0x434);
            if (*(u8*)(pData + 0xA10 + i) != 0) {
                u8* pSlot = pData + i * 0x80;
                func_801CE198(1, pSlot + 0x50, pSlot, *(u8*)(pSlot + 0x7D));
                pData = MenuRawPointer(0x434);
                func_801CE198(1, pData + offset + 0x50, pData + offset,
                              *(u8*)(pSlot + 0x47D));
            }
            offset += 0x80;
        }
        {
            u8* pData = MenuRawPointer(0x434);
            func_801CE198(1, pData + 0x850, pData + 0x800, *(u8*)(pData + 0x87D));
            pData = MenuRawPointer(0x434);
            if (*(u8*)(pData + 0xA18) != 0) {
                u32 off2 = 0x880;
                for (i = 0; i < 3; i++) {
                    pData = MenuRawPointer(0x434);
                    func_801CE198(1, pData + off2 + 0x50, pData + off2,
                                  *(u8*)(pData + i * 0x80 + 0x8FD));
                    off2 += 0x80;
                }
            }
        }
    }
}

void func_801D1914(void) {
    void* pMenu = g_Menu;
    void* pManager = g_Menu->pManager;
    if (*(u8*)((u8*)pManager + 0x4D) != 0) {
        s32 i;
        u32 off1 = 0x680;
        for (i = 0; i < 0xD; i++) {
            pMenu = g_Menu;
            {
                u8* pData = *(u8**)((u8*)pMenu + 0x438);
                if (*(u8*)(pData + 0x2596 + i) != 0) {
                    u8* pSlot = pData + i * 0x80;
                    u8 idx = *(u8*)(pSlot + 0x7D);
                    u8* pOT = *(u8**)((u8*)pMenu + 0x1D4) + 0x80;
                    AddPrim(pOT, pSlot + idx * 0x28);
                    pMenu = g_Menu;
                    pData = *(u8**)((u8*)pMenu + 0x438);
                    pSlot = pData + i * 0x80;
                    idx = *(u8*)(pSlot + 0x7D);
                    pOT = *(u8**)((u8*)pMenu + 0x1D4) + 0x80;
                    AddPrim(pOT, pData + off1 + idx * 0x28);
                }
            }
            off1 += 0x80;
        }
        pMenu = g_Menu;
        {
            u8* pData = *(u8**)((u8*)pMenu + 0x438);
            func_801CE198(1, pData + 0xD50, pData + 0xD00, *(u8*)(pData + 0xD7D));
            pMenu = g_Menu;
            pData = *(u8**)((u8*)pMenu + 0x438);
            {
                u32 off2 = 0xD80;
                u32 off3 = 0x21D0;
                for (i = 0; i < 0xD; i++) {
                    pMenu = g_Menu;
                    pData = *(u8**)((u8*)pMenu + 0x438);
                    {
                        u8* pSlot2 = pData + i;
                        u8 a = *(u8*)(pSlot2 + 0x257C);
                        u8 b = *(u8*)(pSlot2 + 0x2589);
                        func_801CE2B4(a, pData + off2, b);
                    }
                    pMenu = g_Menu;
                    pData = *(u8**)((u8*)pMenu + 0x438);
                    {
                        u8* pSlot3 = pData + i;
                        if (*(u8*)(pSlot3 + 0x25A3) != 0) {
                            u8 idx2 = *(u8*)(pSlot3 + 0x25B0);
                            u8* pOT2 = *(u8**)((u8*)pMenu + 0x1D4) + 0x80;
                            AddPrim(pOT2, pData + off3 + idx2 * 0x28);
                        }
                    }
                    off2 += 0x190;
                    off3 += 0x48;
                }
            }
        }
    }
}

#ifndef XENO_PC_PORT
void func_801D1AAC(void) {
    s32 i;

    i = 0;
    do {
        if (g_Menu->pManager->shouldRenderArrowCursor[i]) {
            MenuArrowCursor* cursor = g_Menu->arrowCursors[i];

            func_801CE198(1, cursor->vertices, cursor->polys,
                          cursor->renderContext);
        }
        i++;
    } while (i < MENU_MAX_NUM_ARROW_CURSORS);
}
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

/* Run the per-frame menu draw passes in retail order. */
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
void func_801D1E80(void) {
    g_Menu->transitionEffectState = MENU_OPEN_ANIMATION_START;
    func_801C8574(0x5B);
}
#else
void func_801D1E80(void) {
    g_Menu->transitionEffectState = MENU_OPEN_ANIMATION_START;
    func_801C8574(0x5B);
}
#endif

#ifndef XENO_PC_PORT
void func_801D1EB0(void) {
    g_Menu->transitionEffectState = MENU_CLOSE_ANIMATION_START;
    func_801C8574(0x5C);
}
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
    if (g_Menu->pManager == NULL) {
        return;
    }
    g_Menu->pManager->unk4 = 0;
    g_Menu->pManager->unk3 = 0;
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
void func_801D2484(void) {
    g_Menu->pManager->shouldRenderPointerCursors = 0;
}
#else
void func_801D2484(void) {
    g_Menu->pManager->shouldRenderPointerCursors = 0;
}
#endif

extern u16 D_801EA534;
extern u16 D_801E9E4C[];
extern u16 D_801E9E58[];

void func_801D249C(s32 enable) {
    if (enable) {
        s32 i;
        u16* pSrc = D_801E9E4C;
        u32 primOff = 0xC60;
        u32 slotOff = 0x180;
        void* pMenu = g_Menu;
        func_801E7E68((u8*)pMenu + 0xAE0, &D_801EA534, 4, 6);
        for (i = 0; i < 3; i++) {
            u16 tblVal = D_801E9E58[i];
            pMenu = g_Menu;
            func_801C851C((u8*)pMenu + primOff + 0x50, *pSrc, tblVal,
                          *(u8*)((u8*)pMenu + slotOff + 0xB5E), 0xD);
            pMenu = g_Menu;
            *(u8*)((u8*)pMenu + slotOff + 0xB5D) = *(u8*)((u8*)pMenu + 0x308);
            pMenu = g_Menu;
            *(u8*)((u8*)pMenu + slotOff + 0xB5F) = 1;
            pSrc++;
            pMenu = g_Menu;
            ((u8*)g_Menu->pManager)[i + 0x17] = 1;
            primOff += 0x80;
            slotOff += 0x80;
        }
    } else {
        s32 i;
        for (i = 0; i < 3; i++) {
            void* pMenu = g_Menu;
            ((u8*)g_Menu->pManager)[i + 0x14] = 0;
        }
    }
}

void func_801D25E4(void) {
    void* pManager;
    s32 i;
    pManager = g_Menu->pManager;
    for (i = 0; i < 6; i++) {
        *(u8*)((u8*)pManager + i + 0x14) = 0;
    }
}

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D261C);
#endif

#ifndef XENO_PC_PORT
extern void func_801D5BA4(s32 x, s32 y);

void func_801D28A8(void) {
    func_801D397C(0, 0xD4, 0xB2, 0x60, 0x10, 0, 0, 4, 0);
    func_801D5BA4(0xD8, 0xB6);
}
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
                          u8 directParams, u8 unk714, s32 zIndex,
                          u8 hasScrollBar);
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
    if (g_Menu->pManager != NULL && g_Menu->pManager->unk5[1] != 0) {
        func_801D5CF8(0xD0, 0xCA);
    }
}

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D29A8);

INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D2D38);
#else
extern void func_801E8018();  /* unprototyped: 3- and 4-arg retail call shapes */
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

extern void func_801D7CFC(s32, s32, s32);
extern void func_801D8EA4(s32, s32, s32, s32);

void func_801D2EC0(u8 arg0, u8 arg1) {
    func_801D7C3C(arg0, arg1);
    {
        s32 gameStateByte = *(u8*)((u8*)&g_GameState + 0x22B1 + arg0);
        func_801D7CFC(arg0, arg1, gameStateByte);
    }
    func_801C7BF4();
    func_801D8DE4(arg0, 0, 0, arg1);
    func_801C7BF4();
    func_801D8EA4(arg0, 0, 0, arg1);
}

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D2F4C);
#else
extern void func_801E920C(POLY_FT4*, s32, s32, s32, s32, s32, s32);
extern void* GetStringEntry(void*, s32);
extern s32 SystemRenderStringEntry(void*, void*, s32, s32);

/* Retail 801D2F4C..801D32B0: four descriptors share two upload buffers.
 * Only three rows are rendered. Use native MenuString sizes and pointer
 * fields throughout; the fourth descriptor still owns a shared pointer. */
void func_801D2F4C(u8 firstString) {
    s32 i;
    MenuWindowParameters* parameters;

    func_801D397C(2, 0x7A, 0x96, 0xBC, 0x40, 1, 1, 4, 0);
    parameters = g_Menu->windowParameters[2];
    while (parameters->unk11 == 0) {
        func_801C7BF4();
    }
    for (i = 0; i < 4; i++) {
        void* allocation = HeapAlloc(sizeof(MenuString), 0);
        g_Menu->unk1DE0[i] = allocation;
        bzero(allocation, sizeof(MenuString));
        if ((i & 1) == 0) {
            void* pixels = HeapAlloc(0x5CA, 0);
            g_Menu->unk1DE0[i]->pVramBuffer = pixels;
            g_Menu->unk1DE0[i]->vramDest.x = 0x140;
            g_Menu->unk1DE0[i]->vramDest.y = 0x4E + (i / 2) * 0xD;
            g_Menu->unk1DE0[i]->vramDest.w = 0x3A;
            g_Menu->unk1DE0[i]->vramDest.h = 0xD;
        } else {
            g_Menu->unk1DE0[i]->pVramBuffer = g_Menu->unk1DE0[i - 1]->pVramBuffer;
        }
    }
    for (i = 0; i < 3; i++) {
        MenuString* string = g_Menu->unk1DE0[i];
        void* entry = GetStringEntry(g_Menu->unk2E0, firstString + i);
        string->width = SystemRenderStringEntry(entry, string->pVramBuffer,
                                                0x36, i & 1);
        func_801E7C50(string, i, 0, 0);
        func_801E920C(&string->polys[g_Menu->renderContext],
                      0x84, 0xA0 + i * 0x10, 0, 0x4E + (i / 2) * 0xD,
                      string->width, 0xD);
        func_801C851C(string->vertices, 0x84, 0xA0 + i * 0x10,
                      string->width, 0xD);
        string->unk7F = 1;
        string->renderContext = (u8)g_Menu->renderContext;
    }
    LoadImage(&g_Menu->unk1DE0[0]->vramDest, g_Menu->unk1DE0[0]->pVramBuffer);
    LoadImage(&g_Menu->unk1DE0[2]->vramDest, g_Menu->unk1DE0[2]->pVramBuffer);
    DrawSync(0);
    g_Menu->pManager->unk2E = 1;
    HeapFree(g_Menu->unk1DE0[0]->pVramBuffer);
    HeapFree(g_Menu->unk1DE0[2]->pVramBuffer);
    func_801C7BF4();
    func_801C7BF4();
}
#endif

s32 func_801D32B4(s32 arg0) {
    if (g_Menu->pManager->shouldRenderWindow[2] != 0) {
        s32 i;
        func_801D4EA0(2);
        g_Menu->pManager->unk2E = 0;
        for (i = 0; i < 4; i++) {
            HeapFree(g_Menu->unk1DE0[i]);
        }
    }
    func_801C7BF4();
    return 0;
}

void func_801D3344(s32 x, s32 y, s32 width) {
    if (g_Menu->pManager->scrollHandleActive == 0) {
        g_Menu->pScrollHandle = HeapAlloc(sizeof(MenuScrollBarHandle), 0);
        bzero(g_Menu->pScrollHandle, sizeof(MenuScrollBarHandle));
    }
    func_8002675C(g_Menu->unk2DC, 0x107, g_Menu->pScrollHandle,
                  g_Menu->renderContext, x, y, 0x1000);
    func_801C851C(g_Menu->pScrollHandle->vertices,
                  x & 0xFFFF, y & 0xFFFF, 8, width & 0xFFFF);
    g_Menu->pScrollHandle->renderContext = *(u8*)&g_Menu->renderContext;
    g_Menu->pManager->scrollHandleActive = 1;
}

#ifndef XENO_PC_PORT
void func_801D3444(void) {
    g_Menu->pManager->scrollHandleActive = 0;
    HeapFree(g_Menu->pScrollHandle);
}
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
void func_801D3674(void) {
    if (g_Menu->pManager->unk5C[0xB] != 0) {
        g_Menu->pManager->unk52[1] = 0;
        g_Menu->pManager->unk5C[0xB] = 0;
        HeapFree((void*)(uintptr_t)*(u32*)&g_Menu->unk440[0]);
    }
}
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
extern u16 D_801EA5D0[];   /* variant-1 v reads bytes (lbu, stride 4); u16 to
                            * agree with the lhu-width extern later in the TU */
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
        v = ((u8*)D_801EA5D0)[sl * 4];  /* retail lbu, stride 4 */
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
void func_801D397C(u8 windowIndex, s32 x, s32 y, s32 w, s32 h, u8 directParams,
                   u8 unk714, s32 zIndex, u8 hasScrollBar) {
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

#ifndef XENO_PC_PORT
extern void func_801D4D1C(u8, s32, s32, s32, s32, u8, s32, u8);
#endif

void func_801D3B00(void) {
    s32 i;
    /* Host Menu is larger than retail (pManager lives at 0x3F8, not 0x33C).
     * Always use typed members — raw PSX offsets SEGV on the port. */
    for (i = 0; i < MENU_MAX_NUM_WINDOWS; i++) {
        MenuManager* pManager = g_Menu->pManager;
        MenuWindowParameters* pData = g_Menu->windowParameters[i];
        u32 scrollX = 0, scrollY = 0;
        if (pManager == NULL || pData == NULL) {
            continue;
        }
        if (pManager->unk27[i] != 0 && pData->unk11 == 0) {
            u16 curW = pData->unk8;
            u16 maxW = pData->width;
            u16 curH, maxH;
            if (curW + 0x20 >= maxW) {
                pData->unk8 = maxW;
                scrollX = 1;
            } else {
                pData->unk8 = curW + 0x20;
            }
            curH = pData->unkA;
            maxH = pData->height;
            if (curH + 0x20 >= maxH) {
                pData->unkA = maxH;
                scrollY = 1;
            } else {
                pData->unkA = curH + 0x20;
            }
            /* Retail increments a completion count once per clamped axis. */
            if (scrollX + scrollY == 2) {
                pData->unk11 = 1;
            }
            {
                u16 h = pData->unkA;
                u16 w = pData->width;
                u16 x0 = pData->x;
                u16 y0 = pData->y;
                u16 curW2 = pData->unk8;
                u16 curH2 = pData->height;
                s32 x = (s32)(x0 + w / 2 - curW2 / 2) & 0xFFFF;
                s32 y = (s32)(y0 + curH2 / 2 - h / 2) & 0xFFFF;
                func_801D4D1C(pData->index, x, y, curW2, h,
                              pData->unk12, pData->zIndex, pData->hasScrollBar);
            }
        }
    }
}

extern s32 func_8002675C(u8*, s32, void*, s32, s32, s32, s32);
extern s32 func_800263E4(u8*, s32, void*, s32, s32, s32, s32, s32, s32);

void func_801D3C4C(u8 slotIdx, s32 x, s32 y, s32 w, s32 h) {
    MenuWindow* window = g_Menu->windows[slotIdx];
    u32 ux = (u16)x;
    u32 uy = (u16)y;
    (void)w; /* Retail ignores a3 and reads height from the fifth argument. */
    func_8002675C(g_Menu->unk2DC, 0x105, &window->polysScrollBarEnds[0],
                 g_Menu->renderContext, ux, uy, 0x1000);
    func_800263E4(g_Menu->unk2DC, 0x105, &window->polysScrollBarEnds[2],
                 g_Menu->renderContext, ux, (s32)(uy + (u16)h) - 8,
                 0x1000, 0, 1);
    func_8002675C(g_Menu->unk2DC, 0x106, window->polysScrollBarEmpty,
                 g_Menu->renderContext, ux, uy + 8, 0x1000);
    func_801C851C(&window->vertsScrollBarEnds[0], ux, uy, 8, 8);
    func_801C851C(&window->vertsScrollBarEnds[4], ux,
                 (u16)((u32)y + (u32)h), 8, 0xFFF8);
    func_801C851C(window->vertsScrollBarEmpty, ux,
                 (u16)((u32)y + 8), 8, (u16)((u32)h - 8));
}

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
void func_801D4EA0(s32 windowIndex) {
    u8 index = (u8)windowIndex;
    g_Menu->pManager->shouldRenderWindow[index] = 0;
    g_Menu->pManager->unk27[index] = 0;
    HeapFree(g_Menu->windows[index]);
    HeapFree(g_Menu->windowParameters[index]);
}
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

extern s32 D_801E9B48;
extern s32 D_801E9B4C;
extern s32 D_801E9B50;
extern s32 D_801E9B54;

/* Arc A portraits: the EXP / next-EXP digit quads (7 digits each,
 * digits[2..8]).  Lists buf+0xE10 (count 0x1277) and buf+0x1040 (0x1278).
 * The exp values live in the GameCharacter head blob (+0x44 / +0x48). */
void func_801D55B4(u8 slot, u8 charId, s32 x, s32 y) {
    u8* buf = (u8*)(uintptr_t)*(u32*)&g_Menu->unk39C[slot * 4];
    s32 i;

    func_801C80B8(*(u32*)((u8*)&g_GameState + 0x2B0 + charId * sizeof(GameCharacter)));
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

    func_801C80B8(*(u32*)((u8*)&g_GameState + 0x2B4 + charId * sizeof(GameCharacter)));
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

extern s32 D_801E9B18;
extern s32 D_801E9B1C;
extern s32 D_801E9B20;
extern s32 D_801E9B24;

/* Arc A portraits: the LEVEL digit quads (3 digits, digits[6..8]) into
 * buf+0x910 (count 0x1271), and the second level number (char+0x63) into
 * buf+0xA00 (count 0x1272) -- tinted green in a post-pass. */
void func_801D5794(u8 slot, u8 charId, s32 x, s32 y) {
    u8* buf = (u8*)(uintptr_t)*(u32*)&g_Menu->unk39C[slot * 4];
    s32 i;

    func_801C80B8(*(u8*)((u8*)&g_GameState + 0x2CE + charId * sizeof(GameCharacter)));
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

    func_801C80B8(*(u8*)((u8*)&g_GameState + 0x2CF + charId * sizeof(GameCharacter)));
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
        SetShadeTex((POLY_FT4*)(buf + 0xA00 +
            (i * 2 + g_Menu->renderContext) * sizeof(POLY_FT4)), 0);
        ((POLY_FT4*)(buf + 0xA00 +
            (i * 2 + g_Menu->renderContext) * sizeof(POLY_FT4)))->r0 = 0;
        ((POLY_FT4*)(buf + 0xA00 +
            (i * 2 + g_Menu->renderContext) * sizeof(POLY_FT4)))->g0 = 0x80;
        ((POLY_FT4*)(buf + 0xA00 +
            (i * 2 + g_Menu->renderContext) * sizeof(POLY_FT4)))->b0 = 0;
    }
}

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

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D5ED4);
#endif

extern s32 D_801EA39C[];
extern s32 D_801E9B60[];
extern s32 D_801E9C20[];

void func_801D6194(u8 slotType) {
    void* pMenu;
    void* pData;
    s32 base = slotType * 24;
    s32 i;

    pMenu = g_Menu;
    pData = MenuRawPointer(0x358);
    *(u8*)(pData + 0x2AEC) = 0;

    for (i = 0; i < 0x18; i++) {
        s32 tblIdx = base + i;
        s32 val = D_801EA39C[tblIdx];
        if (val == 0xFFFF) continue;

        pMenu = g_Menu;
        {
            void* pRender = MenuRawPointer(0x358);
            u8 count = *(u8*)(pRender + 0x2AEC);
            s32 off = count * 0xA0 + 0xA0;
            s32 stackArg;
            pMenu = g_Menu;
            {
                void* pM2 = g_Menu;
                func_8002675C(
                    *(u8**)((u8*)pM2 + 0x2DC),
                    val,
                    pRender,
                    *(s32*)((u8*)pM2 + 0x308),
                    D_801E9B60[tblIdx],
                    D_801E9C20[tblIdx],
                    0x1000
                );
            }
            pMenu = g_Menu;
            pData = MenuRawPointer(0x358);
            *(u8*)(pData + 0x2AEC) = *(u8*)(pData + 0x2AEC) + (u8)val;
        }
    }

    pMenu = g_Menu;
    pData = MenuRawPointer(0x358);
    if (*(u8*)(pData + 0x2AEC) > 0) {
        s32 primOff = 0x1EE0;
        for (i = 0; i < *(u8*)(pData + 0x2AEC); i++) {
            s32 entryOff;
            pMenu = g_Menu;
            {
                s32 tmp = *(s32*)((u8*)g_Menu + 0x308);
                tmp = (tmp + i * 2);
                entryOff = tmp * 0x28;
            }
            pData = MenuRawPointer(0x358);
            {
                u16 x0 = *(u16*)(pData + entryOff + 0xA8);
                u16 y0 = *(u16*)(pData + entryOff + 0xAA);
                u16 x1 = *(u16*)(pData + entryOff + 0xB0);
                u16 y1 = *(u16*)(pData + entryOff + 0xC2);
                s32 w = (u16)(x1 - x0);
                s32 h = (u16)(y1 - y0);
                func_801C851C((SVECTOR*)((u8*)g_Menu + primOff), x0, y0, w, h);
            }
            primOff += 0x20;
        }
    }
}

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D6338);
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D680C);
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D6CF4);
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D7154);
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D74EC);
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D7884);
#endif

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
        void* pManager = g_Menu->pManager;
        *(u8*)((u8*)pManager + 7) = 1;
    }
    {
        void* pMenu = g_Menu;
        void* pData = MenuRawPointer(0x358);
        *(u8*)((u8*)pData + 0x2AE0) = *(u8*)((u8*)pMenu + 0x308);
    }
}

extern s32 D_801E977C[];

void func_801D7CFC(s32 arg0, s32 arg1, s32 arg2) {
    void* pMenu;
    void* pData;
    s32 inverted = (arg2 == 0);
    s32 count = 0;
    s32 primOff = 0x78;
    s32 renderOff = 0x1E00;
    s32 i;

    pMenu = g_Menu;
    pData = MenuRawPointer(0x358);
    *(u8*)(pData + 0x2AED) = 0;

    if (arg1 == 0) return;

    for (i = 0; i < arg1; i++) {
        s32 entryOff;
        pMenu = g_Menu;
        {
            void* pM = g_Menu;
            func_8002675C(
                *(u8**)((u8*)pM + 0x2DC),
                D_801E977C[i],
                MenuRawPointer(0x358),
                *(s32*)((u8*)pM + 0x308),
                0x78,
                0x5A,
                0x1000
            );
        }
        renderOff += 0x50;

        pMenu = g_Menu;
        {
            s32 rc = *(s32*)((u8*)pMenu + 0x308);
            s32 tmp = (rc + i * 2);
            entryOff = tmp * 0x28 + 0x1E00;
        }
        pData = MenuRawPointer(0x358);
        {
            u16 x0 = *(u16*)(pData + entryOff + 0x08);
            u16 y0 = *(u16*)(pData + entryOff + 0x0A);
            u16 x1 = *(u16*)(pData + entryOff + 0x10);
            u16 y1 = *(u16*)(pData + entryOff + 0x22);
            s32 w = (u16)(x1 - x0);
            s32 h = (u16)(y1 - y0);
            func_801C851C((SVECTOR*)((u8*)g_Menu + primOff), x0, y0, w, h);
        }
        primOff += 0x20;
    }

    if (arg1 >= 2) {
        s32 entryOff;
        pMenu = g_Menu;
        {
            s32 rc = *(s32*)((u8*)pMenu + 0x308);
            s32 tmp = (rc + inverted * 2);
            entryOff = tmp * 0x28 + 0x1E00;
        }
        func_801E91C4((POLY_FT4*)(MenuRawPointer(0x358) + entryOff));

        pMenu = g_Menu;
        pData = MenuRawPointer(0x358);
        {
            s32 rc = *(s32*)((u8*)pMenu + 0x308);
            s32 tmp = (rc + inverted * 2);
            s32 off = tmp * 0x28;
            *(u16*)(pData + off + 0x1E16) |= 0x20;
            *(u8*)(pData + off + 0x1E04) = 0x20;
            *(u8*)(pData + off + 0x1E05) = 0x20;
            *(u8*)(pData + off + 0x1E06) = 0x20;
        }

        pMenu = g_Menu;
        pData = MenuRawPointer(0x358);
        *(u8*)(pData + 0x2AEE) = *(u8*)((u8*)pMenu + 0x308);
        pMenu = g_Menu;
        pData = MenuRawPointer(0x358);
        *(u8*)(pData + 0x2AED) = 2;
    }
}

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D7F50);
#else
extern s32 D_801E9D40[];
extern s32 D_801E9D5C[];
extern s32 D_801EA45C[];
extern s32 func_8002675C(u8*, s32, void*, s32, s32, s32, s32);

/* Retail 801D7F50..801D827C: seed Equip-row ornament glyphs into the 0x35C
 * buffer from texture table index 0xE0 / D_801EA45C, then project verts. */
void func_801D7F50(u8 w, u8 h, u8 gearMode) {
    u32 uv;
    s32 tpage, clutX, clutY, texX, texY;
    u8* data;
    s32 limit;
    s32 i;
    s32 j;
    s32 rc = g_Menu->renderContext;

    func_80026338((u8*)g_Menu->unk2DC, 0xE0, &uv, &tpage, &clutX, &clutY,
                  &texX, &texY);
    data = MenuRawPointer(0x35C);
    data[0x32F3] = 0;
    limit = 7 - (gearMode & 0xFF);
    for (i = 0; i < limit; i++) {
        u8 before = data[0x32F3];
        s32 glyph = D_801EA45C[(gearMode & 0xFF) * 7 + i];
        s32 added = func_8002675C(
            (u8*)g_Menu->unk2DC, glyph, data + before * 0x50, rc,
            (s32)w + D_801E9D40[i], (s32)h + D_801E9D5C[i], 0x1000);
        data[0x32F3] = (u8)(before + added);
        if (i & 1) {
            for (j = before; j < data[0x32F3]; j++) {
                u8* poly = data + (j * 2 + rc) * 0x28;
                SetShadeTex(poly, 0);
                poly[4] = 0x40;
                poly[5] = 0x40;
                poly[6] = 0x40;
            }
        }
    }
    for (i = 0; i < data[0x32F3]; i++) {
        u8* poly = data + (i * 2 + rc) * 0x28;
        u16 x0 = *(u16*)(poly + 0x8);
        u16 y0 = *(u16*)(poly + 0xA);
        u16 x1 = *(u16*)(poly + 0x10);
        u16 y3 = *(u16*)(poly + 0x22);
        func_801C851C((SVECTOR*)(data + 0x2420 + i * 0x20), x0, y0,
                      (u16)(x1 - x0), (u16)(y3 - y0));
    }
}
#endif

void func_801D827C(void* pPrims, u8 mode) {
    u8 color[3];
    s32 i;
    switch (mode) {
        case 0: color[0] = 0xFF; color[1] = 0x80; color[2] = 0x80; break;
        case 1: color[0] = 0x80; color[1] = 0xFF; color[2] = 0x80; break;
        case 2: color[0] = 0xFF; color[1] = 0; color[2] = 0; break;
        case 3: color[0] = 0; color[1] = 0; color[2] = 0xFF; break;
    }
    for (i = 0; i < 2; i++) {
        u8* pPrim = (u8*)pPrims + i * 0x24;
        SetPolyG4(pPrim);
        pPrim[4] = color[0]; pPrim[5] = color[1]; pPrim[6] = color[2];
        pPrim[0xC] = color[0]; pPrim[0xD] = color[1]; pPrim[0xE] = color[2];
        pPrim[0x14] = 0; pPrim[0x15] = 0; pPrim[0x16] = 0;
        pPrim[0x1C] = 0; pPrim[0x1D] = 0; pPrim[0x1E] = 0;
    }
}

void func_801D83AC(void* pPrims, u8 mode, u8 count, u8 startIdx) {
    u8 r = 0x40, g = 0x40, b = 0x40;
    s32 i;
    switch (mode) {
        case 0: g = 0x80; b = 0x10; break;
        case 1: r = 0x40; g = 0x80; break;
        case 2: r = 0x40; g = 0x40; b = 0x40; break;
    }
    for (i = 0; i < count; i++) {
        u8* pPrim = (u8*)pPrims + (startIdx + i * 2) * 0x28;
        SetShadeTex(pPrim, 0);
        pPrim[4] = r;
        pPrim[5] = g;
        pPrim[6] = b;
    }
}

extern s32 D_801EA6FC;
extern s32 D_801EA700;
extern s32 D_801EA704;
extern s32 D_801EA708;
extern s32 D_801EA70C;
extern u8 D_801EA710;
extern u8 D_801EA714;

void func_801D84B4(u16 start, u16 target, s32 frames) {
    s32 delta;
    D_801EA6FC = start;
    D_801EA700 = target;
    delta = (s32)target - (s32)start;
    D_801EA704 = delta;
    D_801EA708 = (delta * 25600 / frames * 1000) >> 12;
    if (delta >= 0) {
        D_801EA710 = 2;
        D_801EA714 = 0xE3;
    } else {
        D_801EA710 = 3;
        D_801EA714 = 0xE5;
        D_801EA704 = (s32)start - (s32)target;
    }
    D_801EA70C = (D_801EA704 * 25600 / frames * 1000) >> 12;
}

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

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D8644);
#else
extern s32 D_801E9D78;
extern s32 D_801E9D7C;
extern s32 D_801E9D80;
extern s32 D_801E9D84;
extern s32 D_801EA6FC;
extern s32 D_801EA704;
extern s32 D_801EA708;
extern s32 D_801EA70C;
extern u8 D_801EA710;
extern u8 D_801EA714;
extern void func_801C80B8(u32 value);
extern void func_801C851C(SVECTOR* vertices, s32 x, s32 y, s32 w, s32 h);

/* Retail 801D8644..801D8DE4: per-row Equip stat bars + digit glyphs into the
 * 0x35C buffer.  `listing` selects preview/compare (work+0x280 vs unk330+0xB8). */
void func_801D8644(u8 slot, u8 w, u8 h, u8 listing, u8 gearMode) {
    u16* currentStats;
    u16* baseStats;
    s32 maxVal;
    s32 row;
    s32 limit = 7 - (gearMode & 0xFF);
    s32 polyOff = 0;
    s32 rowY = 0;
    s32 strOffA = 0x2A00;
    s32 strOffB = 0x2D80;
    s32 polyRow = 0;
    u8* data;

    (void)slot;
    if (listing == 0) {
        baseStats = (u16*)((u8*)g_Menu->unk330 + 0xB8);
        currentStats = baseStats;
    } else {
        currentStats = (u16*)(MenuRawPointer(0x360) + 0x280);
        baseStats = (u16*)((u8*)g_Menu->unk330 + 0xB8);
    }
    maxVal = func_801D85DC(0, currentStats, baseStats);
    if (maxVal <= 0) {
        /* Harness cold-boot stats can be all zero; retail PSX div-by-zero is
         * undefined — host SIGFPE if we call func_801D84B4 with maxVal==0. */
        return;
    }
    if (limit <= 0) {
        return;
    }

    for (row = 0; row < limit; row++) {
        s32 i;
        u16 cur = currentStats[row];
        u16 base = baseStats[row];
        u8* rowData;

        data = MenuRawPointer(0x35C);
        rowData = data + row;
        rowData[0x32C0] = 0;
        rowData[0x32CE] = 0;

        /* Retail 801D8778: a0=currentStats[row] (0x38), a1=baseStats[row]
         * (0x40), a2=maxVal. */
        func_801D84B4(cur, base, maxVal);
        func_801D827C(data + polyRow + 0x2030, 0);
        func_801C851C((SVECTOR*)(data + 0x3100 + row * 0x20),
                      (u16)((u16)D_801E9D78 + w),
                      (u16)((u16)D_801E9D7C + h + rowY),
                      (u16)D_801EA708, 6);
        rowData[0x32DC] = (u8)g_Menu->renderContext;
        func_801C80B8((u32)D_801EA6FC);

        for (i = 0; i < 4; i++) {
            u8 d = g_Menu->digits[5 + i];
            if (d == 0xFF) {
                continue;
            }
            {
                s32 before = rowData[0x32C0];
                s32 added = func_8002675C(
                    (u8*)g_Menu->unk2DC, d,
                    data + 0xEB0 + polyOff + before * 0x50,
                    g_Menu->renderContext,
                    w + D_801E9D80 + i * 8, h + D_801E9D84 + rowY, 0x1000);
                rowData[0x32C0] = (u8)(before + added);
            }
        }

        if (rowData[0x32C0] > 0) {
            for (i = 0; i < rowData[0x32C0]; i++) {
                u8* poly = data + 0xEB0 + polyOff +
                           (i * 2 + g_Menu->renderContext) * 0x28;
                u16 x0 = *(u16*)(poly + 0x8);
                u16 y0 = *(u16*)(poly + 0xA);
                u16 x1 = *(u16*)(poly + 0x10);
                u16 y3 = *(u16*)(poly + 0x22);
                func_801C851C((SVECTOR*)(data + strOffA + i * 0x20), x0, y0,
                              (u16)(x1 - x0), (u16)(y3 - y0));
            }
        }
        if (row & 1) {
            func_801D83AC(data + 0xEB0 + polyOff, 2, rowData[0x32C0],
                          (u8)g_Menu->renderContext);
        }
        rowData[0x32C7] = (u8)g_Menu->renderContext;

        if (listing != 0) {
            s32 barX;
            func_801D827C(data + polyRow + 0x2228, D_801EA710);
            if (D_801EA710 == 2) {
                barX = w + D_801E9D78 + D_801EA708;
            } else {
                barX = w + D_801E9D78 + D_801EA708 - D_801EA70C;
            }
            func_801C851C((SVECTOR*)(data + 0x31E0 + row * 0x20), (u16)barX,
                          (u16)(h + (u16)D_801E9D7C + rowY), (u16)D_801EA70C,
                          6);
            rowData[0x32E3] = (u8)g_Menu->renderContext;

            if (D_801EA704 != 0) {
                s32 before = rowData[0x32CE];
                s32 added = func_8002675C(
                    (u8*)g_Menu->unk2DC, D_801EA714,
                    data + 0x1770 + polyOff + before * 0x50,
                    g_Menu->renderContext, w + D_801E9D80 + 0x20,
                    h + D_801E9D84 + rowY, 0x1000);
                s32 xAdvance = 0x28;
                rowData[0x32CE] = (u8)(before + added);
                func_801C80B8((u32)D_801EA704);
                for (i = 0; i < 3; i++) {
                    u8 d = g_Menu->digits[6 + i];
                    if (d != 0xFF) {
                        before = rowData[0x32CE];
                        added = func_8002675C(
                            (u8*)g_Menu->unk2DC, d,
                            data + 0x1770 + polyOff + before * 0x50,
                            g_Menu->renderContext, w + D_801E9D80 + xAdvance,
                            h + D_801E9D84 + rowY, 0x1000);
                        rowData[0x32CE] = (u8)(before + added);
                        xAdvance += 8;
                    }
                }

                if (rowData[0x32CE] > 0) {
                    for (i = 0; i < rowData[0x32CE]; i++) {
                        u8* poly = data + 0x1770 + polyOff +
                                   (i * 2 + g_Menu->renderContext) * 0x28;
                        u16 x0 = *(u16*)(poly + 0x8);
                        u16 y0 = *(u16*)(poly + 0xA);
                        u16 x1 = *(u16*)(poly + 0x10);
                        u16 y3 = *(u16*)(poly + 0x22);
                        func_801C851C((SVECTOR*)(data + strOffB + i * 0x20), x0,
                                      y0, (u16)(x1 - x0), (u16)(y3 - y0));
                    }
                }
                func_801D83AC(data + 0x1770 + polyOff,
                              (u8)(D_801EA710 - 2), rowData[0x32CE],
                              (u8)g_Menu->renderContext);
                rowData[0x32D5] = (u8)g_Menu->renderContext;
                data[0x32F2] = 1;
            }
        }

        rowData[0x32EA] = 1;
        polyOff += 0x140;
        rowY += 8;
        strOffA += 0x80;
        strOffB += 0x80;
        polyRow += 0x48;
    }
}
#endif

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
        void* pManager = g_Menu->pManager;
        *(u8*)((u8*)pManager + 8) = 1;
    }
    {
        void* pMenu = g_Menu;
        void* pData = MenuRawPointer(0x35C);
        *(u8*)((u8*)pData + 0x32F1) = *(u8*)((u8*)pMenu + 0x308);
    }
}

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D8EA4);
#else
extern void* GetWeaponName(s32 index);
extern void* GetAccessoryName(s32 index);
extern void* func_80033A2C(s32 index);
extern void* func_80033A5C(s32 index);
extern s32 SystemRenderStringEntry(void* string, void* work, s32 height,
                                   s32 field);
extern u16 D_801E9D88[];

/* Retail menu.elf 0x801D83B4..0x801D8C14: build the five character/gear
 * equipment-name rows (four for a nonzero mode), upload their temporary text
 * strips, and initialize the paired quads and vertices in the 0x2AC work
 * block owned by g_Menu's retail pointer slot 0x360.
 *
 * The visible s32 parameters retain the declaration used by the existing
 * callers; every argument is narrowed exactly where retail applies andi
 * 0xFF.  Work-block accesses deliberately use retail byte offsets because
 * this allocation contains PSX-width MenuString records, not native-inflated
 * MenuString structs. */
void func_801D8EA4(s32 slotArg, s32 modeArg, s32 buildTextArg,
                   s32 renderSlotArg) {
    u8 slot = (u8)slotArg;
    u8 mode = (u8)modeArg;
    u8 buildText = (u8)buildTextArg;
    u8 renderSlot = (u8)renderSlotArg;
    u8* work = MenuRawPointer(0x360);
    u8 characterId = g_Menu->pManager->currentCharacterIDs[slot];
    u8* character = (u8*)&g_GameState.characters[characterId];
    u8* firstEquipment = character + 0x6A;
    u8* secondEquipment = character + 0x6F;
    u8* accessories = character + 0x74;
    u8* renderBuffer;
    s32 rowCount = 5;
    s32 baseX = 0xD0;
    s32 labelOffset = 0;
    s32 row;
    /* Retail leaves the first stack byte indeterminate on render-slot paths.
     * Normalize only that first value; all later assignments/carry behavior
     * below remains identical. Exact caller-stack residue is UNRESOLVED until
     * it can be captured from retail hardware/emulation, so this branch is
     * not claimed as bit-exact parity. */
    u8 upload = 0;

    if (mode != 0) {
        rowCount = 4;
        baseX = 0x28;
        labelOffset = 9;
        if (mode == 1) {
            labelOffset = 5;
        }
        work[0x298] = 0;
    }

    if (renderSlot != 0) {
        u8* gear = (u8*)&g_GameState.gears[character[0xA0]];

        firstEquipment = gear + 0xC;
        secondEquipment = gear + 4;
        accessories = gear + 9;
    }

    if (buildText != 0) {
        firstEquipment = work + 0x29C;
        secondEquipment = work + 0x2A1;
        accessories = work + 0x2A6;
    }

    renderBuffer = HeapAlloc(0x3F6, 0);
    for (row = 0; row < rowCount; row++) {
        u8* line = work + row * 0x80;
        void* name;
        s32 textField = row & 1;
        s32 renderName = 1;

        if (row == 0) {
            u8 equipment = mode == 2 ? secondEquipment[0]
                                     : firstEquipment[0];

            name = renderSlot == 0 ? GetWeaponName(equipment)
                                   : func_80033A5C(equipment);
        } else if (row == 4 && renderSlot == 0) {
            renderName = 0;
        } else {
            if (mode < 2) {
                if (renderSlot == 0) {
                    name = GetAccessoryName(accessories[row - 1]);
                } else if (mode == 0) {
                    if (row == 1) {
                        name = func_80033A5C(firstEquipment[3]);
                    } else {
                        name = func_80033A2C(accessories[row - 2]);
                    }
                } else {
                    name = func_80033A2C(accessories[row - 1]);
                }
            } else if (renderSlot == 0) {
                name = GetWeaponName(secondEquipment[row]);
            } else {
                name = func_80033A5C(secondEquipment[row]);
            }
        }

        if (renderName != 0) {
            line[0x7E] = (u8)SystemRenderStringEntry(
                name, renderBuffer, 0x24, textField);
        }

        /* After the normalized first value, this is retail's assignment and
         * carry graph: even render-slot rows reuse the preceding stack byte. */
        if ((row & 1) != 0) {
            upload = 1;
        } else if (renderSlot == 0) {
            upload = 0;
        } else if (mode == 0 && row == 4) {
            upload = 1;
        }

        if (upload != 0) {
            RECT rect;

            rect.x = (s16)(0x140 + ((row << 4) & 0x20));
            rect.y = (s16)(0x27 + (row / 4) * 0xD);
            rect.w = 0x28;
            rect.h = 0xD;
            LoadImage(&rect, (u_long*)renderBuffer);
            DrawSync(0);
        }

        /* func_801E7C50 is a source-backed dependency whose native MenuString
         * has an eight-byte pointer at retail offset 0x78.  Stage only the
         * fields that routine consumes, then copy its two retail 0x28-byte
         * polygons and trailing bytes back into this PSX-width work record. */
        {
            MenuString nativeLine;
            u8* nativeBytes = (u8*)&nativeLine;
            s32 byte;

            for (byte = 0; byte < 0x50; byte++) {
                nativeBytes[byte] = line[byte];
            }
            nativeLine.width = line[0x7E];
            nativeLine.unk7C = line[0x7C];
            func_801E7C50(&nativeLine, row, 0xC, 0);
            for (byte = 0; byte < 0x50; byte++) {
                line[byte] = nativeBytes[byte];
            }
            line[0x7C] = nativeLine.unk7C;
            line[0x7F] = nativeLine.unk7F;
        }

        if (row == 4 && renderSlot == 0) {
            u8* poly = work + 0x200 + g_Menu->renderContext * 0x28;
            s32 atlasX = (*(s32*)(D_801EA584 + slot * 4) << 2) & 0xFC;
            u8 atlasY = ((u8*)D_801EA5D0)[slot * 4];

            *(u16*)(poly + 0x16) = GetTPage(0, 0, 0x180, 0);
            *(u16*)(poly + 0x0E) =
                ((character[0xA0] + 0xB) & 1) != 0
                    ? g_SystemPalette2
                    : g_SystemPalette1;
            poly[0x0C] = (u8)atlasX;
            poly[0x0D] = atlasY;
            poly[0x14] = (u8)(atlasX + 0x60);
            poly[0x15] = atlasY;
            poly[0x1C] = (u8)atlasX;
            poly[0x1D] = (u8)(atlasY + 0xD);
            poly[0x24] = (u8)(atlasX + 0x60);
            poly[0x25] = (u8)(atlasY + 0xD);
            line[0x7E] = 0x60;
        }

        /* Retail 0x801D9630..0x801D9640: `sll (labelOffset+row), 2` then
         * `lhu` -- the row-Y table is a word-stride array whose low
         * halfword holds the Y (data.s: .short Y / .short 0 pairs).  A
         * halfword-stride read returns 0 for every odd index and the wrong
         * row for the rest (mode 1 rows landed at 0/159/0/174 instead of
         * 30/57/70/83). */
        func_801C851C((SVECTOR*)(line + 0x50), baseX,
                      D_801E9D88[(labelOffset + row) * 2], line[0x7E], 0xD);
        work[0x294 + row] = 1;
    }

    work[0x299] = (u8)g_Menu->renderContext;
    g_Menu->pManager->unk4A[1] = 1;
    HeapFree(renderBuffer);
}
#endif

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
/* K&R-style (unprototyped) on purpose: the Status-family retail callers in
 * func_801E20C8 pass only 2 args, with gearMode as $a2 residue (the KNOWN
 * RESIDUE CASE catalogue above) — a prototyped def would reject those calls. */
u8 func_801D9704(current, backward, gearMode)
u8 current;
s32 backward, gearMode;
{
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

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D9808);
#endif

void func_801D9B08(void) {
    func_801C8960();
    Vsync(0);
    InitCARD(1);
    StartCARD();
    _bu_init();
    DrawSync(0);
    Vsync(0);
    EnterCriticalSection();
    MenuSetCardEvent(0, OpenEvent(0xF4000001, 0x4, 0x2000, NULL));
    MenuSetCardEvent(1, OpenEvent(0xF4000001, 0x8000, 0x2000, NULL));
    MenuSetCardEvent(2, OpenEvent(0xF4000001, 0x100, 0x2000, NULL));
    MenuSetCardEvent(3, OpenEvent(0xF4000001, 0x2000, 0x2000, NULL));
    EnableEvent(MenuCardEvent(0));
    EnableEvent(MenuCardEvent(1));
    EnableEvent(MenuCardEvent(2));
    EnableEvent(MenuCardEvent(3));
    ExitCriticalSection();
}

extern void* D_801EA718;
extern void* D_801EA71C;
extern void* D_801EA720;

s32 func_801D9C84(void) {
    void* pMenu;
    void* pData;
    s32 result = 1;

    func_801D2F4C(0x20);
    pMenu = g_Menu;
    ((u8*)g_Menu->pManager)[0x33] = 1;

    while (1) {
        pMenu = g_Menu;
        if (*(u8*)((u8*)pMenu + 0x329) == 0) break;
        func_801C7BF4();
    }

    pMenu = g_Menu;
    pData = *(void**)((u8*)pMenu + 0x32C);
    *(u8*)(pData + 0x4FE7) = 1;
    func_801C7BF4();
    func_801D9B08();
    DrawSync(0);
    Vsync(0);

    EnterCriticalSection();
    D_801EA718 = CdSyncCallback(0);
    D_801EA71C = CdReadyCallback(0);
    D_801EA720 = CdReadCallback(0);
    ExitCriticalSection();

    pMenu = g_Menu;
    pData = *(void**)((u8*)pMenu + 0x32C);
    *(u8*)(pData + 0x4FE9) = 0xFF;
    *(u8*)(pData + 0x4FE8) = 0xFF;
    pMenu = g_Menu;
    pData = *(void**)((u8*)pMenu + 0x32C);
    *(u8*)(pData + 0x4F88) = 0;
    pMenu = g_Menu;
    pData = *(void**)((u8*)pMenu + 0x32C);
    *(u8*)(pData + 0x4F89) = 0;
    pMenu = g_Menu;
    pData = *(void**)((u8*)pMenu + 0x32C);
    *(u8*)(pData + 0x4FE6) = 2;
    pMenu = g_Menu;
    *(u8*)((u8*)pMenu + 0x326) = 0x3C;

    func_801C7BF4();
    func_801C7BF4();

    if ((u8)func_801C93A8() == 0) {
        pMenu = g_Menu;
        pData = g_Menu->pManager;
        if (*(u8*)(pData + 0x33) != 0) {
            result = 0;
            func_801D32B4(0);
            pMenu = g_Menu;
            ((u8*)g_Menu->pManager)[0x33] = 0;
        }
    }

    return result;
}

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

extern u8 D_801EA53C[];
extern u8 D_801EA542[];

void func_801D9F34(void) {
    void* pMenu;
    u8* pTable;
    if (D_80059460 == 2) {
        pTable = D_801EA542;
    } else {
        pTable = D_801EA53C;
    }
    pMenu = g_Menu;
    func_801E8018(6, (u8*)pMenu + 0xDE0, pTable, (u8*)(g_Menu->pManager) + 0x1A);
}

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801D9F98);
#endif

extern u8 D_801EA548[];

void func_801DA4A8(void) {
    ItemMenuWork* work;

    func_801D22F4(2);
    func_801E8018(8, g_Menu->itemMenuStrings, D_801EA548,
                   g_Menu->pManager->unk38);
    /* Retail allocates/clears 0x1198 bytes and publishes at +0x42C.
     * Embedded MenuStrings expand on the host; use their owning types. */
    work = HeapAlloc(sizeof(ItemMenuWork), 0);
    g_Menu->unk42C[0] = (u32)(uintptr_t)work;
    bzero(work, sizeof(ItemMenuWork));
    func_801C72BC(0);
}

void func_801DA518(void) {
    func_801D3444();
    func_801D4EA0(3);
    func_801D4EA0(4);
    g_Menu->pManager->unk48 = 0;
    func_801C72BC(0x10);
    /* Preserve retail's explicit releases after the resource-mode call. */
    HeapFree((void*)(uintptr_t)
        ((ItemMenuWork*)(uintptr_t)g_Menu->unk42C[0])->descriptionBundle);
    HeapFree((void*)(uintptr_t)g_Menu->unk42C[0]);
    HeapFree(g_Menu->unk330->pItemsData);
}

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
void func_801DB02C(s32 cursorIndex) {
    u8 index = (u8)cursorIndex;
    g_Menu->arrowCursors[index] = HeapAlloc(sizeof(MenuArrowCursor), 0);
    bzero(g_Menu->arrowCursors[index], sizeof(MenuArrowCursor));
    g_Menu->arrowCursors[index]->curAnimFrame = 4;
    g_Menu->arrowCursors[index]->animFrameDuration = 0;
}
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
void func_801DB340(s32 cursorIndex) {
    u8 index = (u8)cursorIndex;
    HeapFree(g_Menu->arrowCursors[index]);
    g_Menu->pManager->shouldRenderArrowCursor[index] = 0;
}
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
extern void func_801D397C(u8 windowIndex, s32 x, s32 y, s32 w, s32 h,
                          u8 directParams, u8 unk714, s32 zIndex,
                          u8 hasScrollBar);
extern void func_801D4EA0(s32);
extern u8 func_801D9704();  /* unprototyped: see the K&R def above */
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

    switch (cat) {
    case 0:
        mode = 2;
        break;
    case 1:
        mode = 5;
        break;
    case 2:
        mode = 6;
        break;
    }

    func_801C72BC((mode | 0x10) & 0xFF);
    work = (AbilityMenuWork*)(uintptr_t)g_Menu->unk42C[1];
    HeapFree(work);
    g_Menu->pManager->unk5[1] = 1;
    g_Menu->pManager->shouldRenderWindow[1] = 1;
}

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

#ifndef XENO_PC_PORT
extern void func_801E8F60(s32, u8);
#endif

void func_801DD5E8(u8 charIdx) {
    void* pMenu;
    u8* pData;
    s32 i;
    func_801E8F60(5, charIdx);
    func_801E8F60(6, charIdx);
    pMenu = g_Menu;
    pData = *(u8**)((u8*)pMenu + 0x444);
    {
        u8 tableIdx = pData[0x75];
        func_801E8EAC(pData + tableIdx * 0x28, charIdx);
    }
    pMenu = g_Menu;
    pData = *(u8**)((u8*)pMenu + 0x430);
    {
        u8 tableIdx = pData[0xE7D];
        func_801E8EAC(pData + 0xE00 + tableIdx * 0x28, charIdx);
    }
    for (i = 0; i < 2; i++) {
        pMenu = g_Menu;
        pData = *(u8**)((u8*)pMenu + 0x430);
        {
            u8 tableIdx = pData[i * 0x80 + 0xF7D];
            func_801E8EAC(pData + 0xF00 + i * 0x80 + tableIdx * 0x28, charIdx);
        }
    }
}

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801DD790);
#endif

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

extern u8 D_801EA558[];
extern void func_801D22F4(s32);
extern void func_801DB02C(s32);
#ifdef XENO_PC_PORT
extern void* GetStringEntry(void* bundle, s32 index);
extern s32 SystemRenderStringEntry(void* string, void* work, s32 height,
                                   s32 field);
extern void func_801E7C50(MenuString*, s32, s32, s32);
#endif

#ifdef XENO_PC_PORT
/* Build descriptor pairs into a packed retail MenuString bank (0x80/slot).
 * Native E7E68 strides sizeof(MenuString)==152; Equip's unk14E0 is the
 * packed u8[0x900] bank that E8070 mode 3 / D0F54 also address at 0x80. */
static void MenuPackedE7E68(u8* strings, u8* descriptorIds, s32 yOffset,
                            s32 count) {
    s32 index = 0;

    while (index < count) {
        u8* firstLine = strings + index * 0x80;
        u8* secondLine = strings + (index + 1) * 0x80;
        s32 row = (index + yOffset) / 4;
        MenuString nativeFirst;
        MenuString nativeSecond;
        RECT vramDest;
        s32 byte;

        firstLine[0x7E] = (u8)SystemRenderStringEntry(
            GetStringEntry(g_Menu->unk2E0, descriptorIds[0]),
            g_Menu->unk4E0[0].pVramBuffer, 0x18, 0);
        secondLine[0x7E] = (u8)SystemRenderStringEntry(
            GetStringEntry(g_Menu->unk2E0, descriptorIds[1]),
            g_Menu->unk4E0[0].pVramBuffer, 0x18, 1);

        vramDest.x = (s16)(0x140 + ((index << 4) & 0x20));
        vramDest.y = (s16)(row * 0xD);
        vramDest.w = 0x1C;
        vramDest.h = 0xD;

        memset(&nativeFirst, 0, sizeof(nativeFirst));
        memset(&nativeSecond, 0, sizeof(nativeSecond));
        for (byte = 0; byte < 0x50; byte++) {
            ((u8*)&nativeFirst)[byte] = firstLine[byte];
            ((u8*)&nativeSecond)[byte] = secondLine[byte];
        }
        nativeFirst.width = firstLine[0x7E];
        nativeSecond.width = secondLine[0x7E];
        nativeFirst.unk7C = firstLine[0x7C];
        nativeSecond.unk7C = secondLine[0x7C];
        func_801E7C50(&nativeFirst, index, yOffset, 0);
        func_801E7C50(&nativeSecond, index + 1, yOffset, 0);
        for (byte = 0; byte < 0x50; byte++) {
            firstLine[byte] = ((u8*)&nativeFirst)[byte];
            secondLine[byte] = ((u8*)&nativeSecond)[byte];
        }
        firstLine[0x7C] = nativeFirst.unk7C;
        firstLine[0x7F] = nativeFirst.unk7F;
        secondLine[0x7C] = nativeSecond.unk7C;
        secondLine[0x7F] = nativeSecond.unk7F;

        LoadImage(&vramDest, (u_long*)g_Menu->unk4E0[0].pVramBuffer);
        descriptorIds += 2;
        index += 2;
        DrawSync(0);
    }
}
#endif

void func_801DE2C8(u8 arg0) {
    void* pBuf = HeapAlloc(0xA1C, NULL);
    void* pMenu = g_Menu;
    /* menu+0x434 → unk42C[2] via MenuStoreRawPointer remap. */
    MenuStoreRawPointer(0x434, pBuf);
    bzero(pBuf, 0xA1C);
    {
        u8* pTable = D_801EA558 + arg0 * 6;
        pMenu = g_Menu;
        {
#ifdef XENO_PC_PORT
            (void)pMenu;
            MenuPackedE7E68(g_Menu->unk14E0, pTable, 4, 6);
#else
            void* pManager = g_Menu->pManager;
            func_801E8018(6, g_Menu->unk14E0, pTable, (u8*)pManager + 0x40);
#endif
        }
    }
    func_801C72BC(7);
    func_801D22F4(3);
    func_801DB02C(0);
    func_801D3488(1, arg0);
}

void func_801DE36C(void) {
    void* pMenu = g_Menu;
    ((u8*)g_Menu->pManager)[0x4C] = 0;
    func_801D4EA0(2);
    func_801D4EA0(3);
    func_801D4EA0(4);
    func_801D4EA0(5);
    func_801C7BF4();
    pMenu = g_Menu;
    func_801E8044(6, (u8*)(g_Menu->pManager) + 0x40);
    func_801C72BC(0x17);
    pMenu = g_Menu;
    HeapFree(MenuRawPointer(0x434));
    func_801D3444();
#ifdef XENO_PC_PORT
    /* TEST TOOLING: remove with Equip acceptance harness. */
    {
        const char* ft = getenv("XENO_FIELD_TEST");
        if (ft && ft[0] == '1') {
            printf("[xeno-port][test] Equip teardown complete; main nav rebuilt\n");
            fflush(stdout);
        }
    }
#endif
}

void func_801DE400(void) {
    void* pMenu = g_Menu;
    void* pManager = g_Menu->pManager;
    *(u8*)((u8*)pManager + 8) = 0;
    pMenu = g_Menu;
    pManager = g_Menu->pManager;
    *(u8*)((u8*)pManager + 0x4B) = 0;
    func_801C7BF4();
    pMenu = g_Menu;
    HeapFree(MenuRawPointer(0x35C));
    pMenu = g_Menu;
    HeapFree(MenuRawPointer(0x360));
}

extern u8 D_801EA558[];
extern u8 D_801E9EA0[];
extern void func_801E8070(s32, void*, void*, void*, void*, s32, s32, s32);

void func_801DE474(u8 arg0, u8 arg1) {
    s32 i;
    s32 start, end;
    s32 yOff;
    for (i = 0; i < 6; i++) {
        ((u8*)g_Menu->pManager)[0x40 + i] = 0;
    }
    if (arg0) {
        start = 2;
        end = 6;
        yOff = 0x72;
    } else {
        start = 0;
        end = 2;
        yOff = 0x5A;
    }
    /* Retail 801DE4FC..: E8070(count=6, strings@menu+0x14E0, idTable,
     * offTable, flags@manager+0x40, selected=i, arg6=i, mode=3). */
    for (i = start; i < end; i++) {
        u8* pTable = D_801EA558 + arg1 * 6;
        func_801E8070(6, g_Menu->unk14E0, pTable, D_801E9EA0,
                      (u8*)g_Menu->pManager + 0x40, i, i, 3);
    }
    /* Retail 801DE56C: beqz shouldRenderWindow[4] has delay-slot ori a0,4
     * so BOTH arms fall into D397C(4,...).  The nonzero arm frees first.
     * (An earlier port used window 6 from the E8070 loop's a0 residue.) */
    if (g_Menu->pManager->shouldRenderWindow[4] != 0) {
        func_801D4EA0(4);
    }
    func_801D397C(4, 0x10, 0xC, 0x80, yOff, 0, 1, 4, 0);
}

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801DE5CC);
#else
extern u8 D_801EA730[];
extern u8 D_801EA7F8[];
extern void* GetWeaponName(s32 index);
extern void* GetAccessoryName(s32 index);
extern void* func_80033A2C(s32 index);
extern void* func_80033A5C(s32 index);
extern s32 SystemRenderStringEntry(void* string, void* work, s32 height,
                                   s32 field);
extern void func_80033B34(u16* src, u8* dst, s32 count);
extern void func_801D36E0(MenuString* pStr, s32 slot, s32 variant, s32 style);

/* Retail Equip list builder/renderer 801DE5CC..801DF0D0 (2824 bytes).
 * Fills D_801EA730 / D_801EA7F8, paints eight rows from `page`, returns
 * max(0, filled-8). The 0x434 work buffer keeps PSX MenuString byte layout. */
static void EquipListStageE7C50(u8* line, s32 row, s32 u0, s32 u1) {
    MenuString nativeLine;
    u8* nativeBytes = (u8*)&nativeLine;
    s32 byte;
    for (byte = 0; byte < 0x50; byte++) nativeBytes[byte] = line[byte];
    nativeLine.width = line[0x7E];
    nativeLine.unk7C = line[0x7C];
    func_801E7C50(&nativeLine, row, u0, u1);
    for (byte = 0; byte < 0x50; byte++) line[byte] = nativeBytes[byte];
    line[0x7C] = nativeLine.unk7C;
    line[0x7F] = nativeLine.unk7F;
}

s32 func_801DE5CC(s32 selectedArg, s32 pageArg, s32 categoryArg, s32 groupArg,
                  s32 gearModeArg) {
    u8 selected = (u8)selectedArg;
    s32 page = pageArg;
    s32 category = categoryArg;
    u8 group = (u8)groupArg;
    u8 gearMode = (u8)gearModeArg;
    MenuManager* manager = g_Menu->pManager;
    u8* work = MenuRawPointer(0x360);
    u8* state = (u8*)&g_GameState;
    MenuUnk6* resources = g_Menu->unk330;
    u8* listBuf;
    u8 type;
    s32 limit;
    u8* equippedWeapon = NULL;
    u8* equippedGearWeapon = NULL;
    u8* equippedAccessory = NULL;
    u16 slotMask = 0;
    u16 otherMask = 0;
    s32 filled;
    s32 i;
    s32 isAccessory;
    u32 rawPtr;
    u8* weapons;
    u8* accessories;
    u8* gearAccessories;
    u8* gearWeapons;
    listBuf = MenuRawPointer(0x434);
    weapons = (u8*)resources->pWeaponsData;
    accessories = (u8*)resources->pAccessoriesData;
    memcpy(&rawPtr, resources->unk8 + 0xC, 4);
    gearAccessories = (u8*)(uintptr_t)rawPtr;
    memcpy(&rawPtr, resources->unk8 + 0x10, 4);
    gearWeapons = (u8*)(uintptr_t)rawPtr;

    if (group) {
        type = (u8)(category + 1);
        limit = 100;
        if (!gearMode) {
            u8 id = work[0x29C + category];
            equippedWeapon = weapons + (id << 4);
        } else {
            u8 id = work[0x29C + category];
            equippedGearWeapon = gearWeapons + id * 20;
        }
    } else if (!gearMode) {
        if (category) {
            type = 5;
            limit = 200;
            equippedAccessory = accessories + (work[0x2A5 + category] << 4);
            slotMask = *(u16*)(equippedAccessory + 0xE);
            otherMask = 0;
            for (i = 0; i < 3; i++) {
                u8* entry = accessories + (work[0x2A6 + i] << 4);
                otherMask |= *(u16*)(entry + 0xE);
            }
        } else {
            type = 0;
            limit = 100;
        }
    } else if (category) {
        type = 5;
        limit = 150;
        equippedAccessory = gearAccessories + work[0x2A5 + category] * 28;
        slotMask = *(u16*)(equippedAccessory + 8);
        otherMask = 0;
        for (i = 0; i < 3; i++) {
            u8* entry = gearAccessories + work[0x2A6 + i] * 28;
            otherMask |= *(u16*)(entry + 8);
        }
    } else {
        type = 0;
        limit = 100;
    }

    for (i = 0; i < 0x190; i++) D_801EA730[i] = 0;

    isAccessory = type == 5;
    filled = isAccessory;
    if (limit) {
        u8* charWeaponIds = state + 0x1D9C;
        u8* charWeaponCounts = state + 0x1D38;
        u8* charAccIds = state + 0x1EC8;
        u8* charAccCounts = state + 0x1E00;
        u8* gearWeaponIds = state + 0x2120;
        u8* gearWeaponCounts = state + 0x20BC;
        u8* gearAccIds = state + 0x221A;
        u8* gearAccCounts = state + 0x2184;
        u8* outIds = D_801EA730 + filled;
        u8* outCounts = D_801EA7F8 + filled;

        for (i = 0; i < limit; i++) {
            u8 accept = 0;
            u8 charId = manager->currentCharacterIDs[selected];
            if (!gearMode) {
                if (type < 5) {
                    u8* weapon = weapons + (charWeaponIds[i] << 4);
                    if (type == 0) {
                        if (func_801C865C(*(u16*)weapon, charId) &&
                            weapon[6] < 5 && charWeaponIds[i] < 0x32) {
                            accept = 1;
                        }
                    } else if (func_801C865C(*(u16*)weapon, charId) &&
                               weapon[6] == equippedWeapon[6] &&
                               charWeaponIds[i] >= 0x32) {
                        accept = 1;
                    }
                } else {
                    u8* item = accessories + (charAccIds[i] << 4);
                    if (func_801C865C(*(u16*)item, charId)) {
                        u16 flags = *(u16*)(item + 0xE);
                        if (!flags || (slotMask & flags)) accept = 1;
                        else if (!(otherMask & flags)) accept = 1;
                    }
                }
            } else {
                u8 gearId = state[0x30C + charId * 0xA4];
                if (type < 5) {
                    u8* weapon = gearWeapons + gearWeaponIds[i] * 20;
                    if (type == 0) {
                        if (func_801C8678(*(u32*)(weapon + 4), gearId) &&
                            weapon[0xF] < 5 && gearWeaponIds[i] < 0x32) {
                            accept = 1;
                        }
                    } else if (func_801C8678(*(u32*)(weapon + 4), gearId) &&
                               weapon[0xF] == equippedGearWeapon[0xF] &&
                               gearWeaponIds[i] >= 0x32) {
                        accept = 1;
                    }
                } else {
                    u8* item = gearAccessories + gearAccIds[i] * 28;
                    if (func_801C8678(*(u32*)item, gearId)) {
                        u16 flags = *(u16*)(item + 8);
                        if (!flags || (slotMask & flags)) accept = 1;
                        else if (!(otherMask & flags)) accept = 1;
                    }
                }
            }

            if (accept) {
                if (!gearMode) {
                    if (type == 5) {
                        outIds[0] = charAccIds[i];
                        outCounts[0] = charAccCounts[i];
                    } else {
                        outIds[0] = charWeaponIds[i];
                        outCounts[0] = charWeaponCounts[i];
                    }
                } else if (type == 5) {
                    outIds[0] = gearAccIds[i];
                    outCounts[0] = gearAccCounts[i];
                } else {
                    outIds[0] = gearWeaponIds[i];
                    outCounts[0] = gearWeaponCounts[i];
                }
                outIds++;
                outCounts++;
                filled++;
            }
        }
    }

    {
        u8* renderBuffer = HeapAlloc(0x3F6, 0);
        s32 row;
        s32 y = 0x12;
        s32 nameOff = 0;
        s32 countOff = 0x400;
        for (row = 0; row < 8; row++, page++) {
            u8 itemId = D_801EA730[page];
            if (itemId) {
                void* name;
                u16 digits[2];
                u8 digitBuf[8];
                u8 tens;
                u8 ones;
                u8* nameLine = listBuf + nameOff;
                u8* countLine = listBuf + countOff;
                s32 width;
                RECT rect;

                if (!gearMode) {
                    name = type == 5 ? GetAccessoryName(itemId)
                                     : GetWeaponName(itemId);
                } else {
                    name = type == 5 ? func_80033A2C(itemId)
                                     : func_80033A5C(itemId);
                }
                width = SystemRenderStringEntry(name, renderBuffer, 0x24, 0);
                nameLine[0x7E] = (u8)width;

                tens = (u8)(D_801EA7F8[page] / 10);
                ones = (u8)(D_801EA7F8[page] - tens * 10);
                digits[0] = tens ? (u16)(tens + 0x10) : 0xC3;
                digits[1] = (u16)(ones + 0x10);
                func_80033B34(digits, digitBuf, 2);
                width = SystemRenderStringEntry(digitBuf, renderBuffer, 0x24, 1);
                countLine[0x7E] = (u8)width;

                /* Retail: x=((row&1)*3)<<3+0x180; y=(row>>1)*0xD+0x80 */
                rect.x = (s16)((((row & 1) * 3) << 3) + 0x180);
                rect.y = (s16)((row >> 1) * 0xD + 0x80);
                rect.w = 0x28;
                rect.h = 0xD;
                LoadImage(&rect, (u_long*)renderBuffer);
                DrawSync(0);

                EquipListStageE7C50(nameLine, row, 0x80, 0x81);
                EquipListStageE7C50(countLine, row, 0x80, 0x82);
                func_801C851C((SVECTOR*)(nameLine + 0x50), 0xA8, y,
                              nameLine[0x7E], 0xD);
                func_801C851C((SVECTOR*)(countLine + 0x50), 0x10C, y,
                              countLine[0x7E], 0xD);
                nameLine[0x7D] = ((u8*)g_Menu)[0x308];
                countLine[0x7D] = ((u8*)g_Menu)[0x308];
                listBuf[0xA10 + row] = 1;
            } else {
                listBuf[0xA10 + row] = 0;
            }
            y += 0xD;
            nameOff += 0x80;
            countOff += 0x80;
        }
        HeapFree(renderBuffer);
    }

#ifndef XENO_EQUIP_LIST_TEST
    /* listBuf+0x800 is a PSX-width MenuString (0x80).  Native D36E0 writes
     * sizeof(MenuString)==152 — stage through a host MenuString and pack
     * polys/vertices/meta back so description lines at +0x880 stay intact. */
    {
        u8* line = listBuf + 0x800;
        MenuString nativeTitle;
        u8* nativeBytes = (u8*)&nativeTitle;
        s32 byte;

        memset(&nativeTitle, 0, sizeof(nativeTitle));
        for (byte = 0; byte < 0x50; byte++) {
            nativeBytes[byte] = line[byte];
        }
        memcpy(nativeTitle.vertices, line + 0x50, sizeof(SVECTOR) * 4);
        nativeTitle.width = line[0x7E];
        nativeTitle.unk7C = line[0x7C];
        nativeTitle.renderContext = line[0x7D];
        func_801D36E0(&nativeTitle, selected, gearMode, 0);
        for (byte = 0; byte < 0x50; byte++) {
            line[byte] = nativeBytes[byte];
        }
        memcpy(line + 0x50, nativeTitle.vertices, sizeof(SVECTOR) * 4);
        line[0x7C] = nativeTitle.unk7C;
        line[0x7D] = nativeTitle.renderContext;
        line[0x7E] = nativeTitle.width;
        line[0x7F] = nativeTitle.unk7F;
    }
#endif
    filled -= 8;
    ((u8*)manager)[0x4C] = 1;
    if (filled < 0) filled = 0;
    return filled;
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801DF0D4);
#else
/* The native data_game_state.c owner allocates 0x4600 bytes. GameState
 * currently describes only the prefix; this helper also accesses the tail. */
extern u8 MenuEquipmentStateStorage[0x4600] __asm__("g_GameState");

/* Retail commit of the preview selection and inventory reconciliation. */
s32 func_801DF0D4(s32 slotArg, s32 categoryArg, s32 groupArg, s32 modeArg) {
    u8* state = MenuEquipmentStateStorage;
    u8* work = MenuRawPointer(0x360);
    u8 character = g_Menu->pManager->currentCharacterIDs[(u8)slotArg];
    u8 category = (u8)categoryArg;
    u8 mode = (u8)modeArg;
    u32 base = character * 0xA4;
    u8* ids = state + (mode ? 0x2120 : 0x1D9C);
    u8* counts = ids - 100;
    s32 limit = 100, result = 0, i, found;
    u8* equipped;
    u8 previous, current;
    if (mode) base = state[0x30C + base] * 0xA4;
    if ((u8)groupArg) {
        u8* markers = state + (mode ? 0x22B6 : 0x2286);
        equipped = state + base + (mode ? 0x97C : 0x2DB) + category;
        current = *equipped;
        previous = work[0x2A1 + category];
        if (!current) {
            *equipped = previous;
            return 0;
        }
        if (markers[current] < 100) previous = 0;
        markers[current] = 100;
    } else if (!category) {
        equipped = state + base + (mode ? 0x984 : 0x2D6);
        current = *equipped;
        previous = work[0x29C];
        if (!current) {
            *equipped = previous;
            return 0;
        }
        result = character == 4;
    } else {
        ids = state + (mode ? 0x221A : 0x1EC8);
        counts = state + (mode ? 0x2184 : 0x1E00);
        limit = mode ? 150 : 200;
        current = state[base + (mode ? 0x980 : 0x2DF) + category];
        previous = work[0x2A5 + category];
    }
    if (current) {
        for (i = 0; i < limit; i++) {
            if (ids[i] == current) { counts[i]--; break; }
        }
    }
    if (previous) {
        found = 0;
        for (i = 0; i < limit; i++) {
            if (ids[i] == previous) { counts[i]++; found = 1; break; }
        }
        if (!found) {
            for (i = 0; i < limit; i++) {
                if (!ids[i]) { ids[i] = previous; counts[i] = 1; break; }
            }
        }
    }
    for (i = 0; i < limit; i++) {
        if (!counts[i]) ids[i] = 0;
        else if (counts[i] >= 100) counts[i] = 99;
    }
    return result;
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801DF5D0);
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801DF890);
#else
/* Retail Equip preview snapshot/cancel. Game-state records retain their
 * 0xA4 byte stride; the resource record contains widened native pointers. */
void func_801DF5D0(s32 slotArg, s32 modeArg) {
    u8* work = MenuRawPointer(0x360);
    u8* state = (u8*)&g_GameState;
    u8* stats = (u8*)&g_Menu->unk330->unkB8;
    u32 character = g_Menu->pManager->currentCharacterIDs[(u8)slotArg];
    u32 base = character * 0xA4;
    s32 i;
    for (i = 0; i < 18; i++) work[0x280 + i] = stats[i];
    if ((u8)modeArg == 0) {
        for (i = 0; i < 5; i++) {
            work[0x29C + i] = state[0x2D6 + base + i];
            work[0x2A1 + i] = state[0x2DB + base + i];
            work[0x2A6 + i] = state[0x2E0 + base + i];
        }
    } else {
        base = state[0x30C + base] * 0xA4;
        for (i = 0; i < 4; i++) {
            work[0x29C + i] = state[0x984 + base + i];
            work[0x2A1 + i] = state[0x97C + base + i];
            work[0x2A6 + i] = state[0x981 + base + i];
        }
    }
}

void func_801DF890(s32 slotArg, s32 modeArg) {
    u8* work = MenuRawPointer(0x360);
    u8* state = (u8*)&g_GameState;
    u32 character = g_Menu->pManager->currentCharacterIDs[(u8)slotArg];
    u32 base = character * 0xA4;
    s32 i;
    if ((u8)modeArg == 0) {
        for (i = 0; i < 5; i++) {
            state[0x2D6 + base + i] = work[0x29C + i];
            state[0x2DB + base + i] = work[0x2A1 + i];
        }
        /* Retail deliberately restores only three bytes of this bank. */
        for (i = 0; i < 3; i++) state[0x2E0 + base + i] = work[0x2A6 + i];
    } else {
        base = state[0x30C + base] * 0xA4;
        for (i = 0; i < 4; i++) {
            state[0x984 + base + i] = work[0x29C + i];
            state[0x97C + base + i] = work[0x2A1 + i];
        }
        for (i = 0; i < 3; i++) state[0x981 + base + i] = work[0x2A6 + i];
    }
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801DFB68);
#else
extern u8 D_801EA730[];
/* Write the selected preview item; the retail caller owns list bounds.
 * Category is signed/full-width, while slot, group and mode use low bytes. */
void func_801DFB68(s32 slotArg, s32 category, s32 row, s32 scroll,
                   s32 groupArg, s32 modeArg) {
    u8* state = (u8*)&g_GameState;
    u32 base;
    s32 offset;
    if ((u8)groupArg == 0 && (category < 0 || category >= 4)) return;
    base = g_Menu->pManager->currentCharacterIDs[(u8)slotArg] * 0xA4;
    if ((u8)modeArg == 0) {
        offset = (u8)groupArg ? 0x2DB + category :
                 category == 0 ? 0x2D6 : 0x2DF + category;
    } else {
        base = state[0x30C + base] * 0xA4;
        offset = (u8)groupArg ? 0x97C + category :
                 category == 0 ? 0x984 : 0x980 + category;
    }
    state[base + offset] = D_801EA730[(u32)row + (u32)scroll];
}
#endif

extern void func_801E3ECC(void*, u8);
extern void func_801E3C2C(void*, u8);

void func_801DFE2C(u8 slotIdx) {
    u8 characterId = g_Menu->pManager->currentCharacterIDs[slotIdx];
    u8 gearId = ((u8*)&g_GameState)[0x30C + characterId * 0xA4];
    MenuUnk6* resource;
    u8* source;
    u8* destination;
    func_801E3ECC(g_Menu->unk330, gearId);
    /* Retail reloads party mapping and resource ownership after each call. */
    characterId = g_Menu->pManager->currentCharacterIDs[slotIdx];
    gearId = ((u8*)&g_GameState)[0x30C + characterId * 0xA4];
    func_801E3C2C(g_Menu->unk330, gearId);
    resource = g_Menu->unk330;
    source = resource->unk20;
    destination = (u8*)&resource->unkB8;
    *(u16*)(destination + 0) = *(u16*)(source + 0x90);
    *(u16*)(destination + 2) = *(u16*)(source + 0x84);
    *(u16*)(destination + 4) = *(u16*)(source + 0x86);
    *(u16*)(destination + 6) = source[0x92];
    *(u16*)(destination + 8) = source[0x93];
    *(u16*)(destination + 10) = source[0x94];
}

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801DFF5C);
#else
extern u8 D_801EA730[];
extern void* GetStringEntry(void* bundle, s32 index);
extern void* GetAccessoryName(s32 index);
extern s32 SystemRenderStringEntry(void* string, void* work, s32 height,
                                   s32 field);

/* Retail Equip description renderer 801DFF5C..801E0430 (1240 bytes).
 * Resolves the focused item id (list row or currently equipped), pulls three
 * GetStringEntry lines from the category's description bank on listBuf+0xA00..,
 * and stages them at listBuf+0x880. listBuf+0xA18 is the visible flag. */
static void EquipDescStageE7C50(u8* line, s32 row, s32 u0, s32 u1) {
    MenuString nativeLine;
    u8* nativeBytes = (u8*)&nativeLine;
    s32 byte;
    for (byte = 0; byte < 0x50; byte++) nativeBytes[byte] = line[byte];
    nativeLine.width = line[0x7E];
    nativeLine.unk7C = line[0x7C];
    func_801E7C50(&nativeLine, row, u0, u1);
    for (byte = 0; byte < 0x50; byte++) line[byte] = nativeBytes[byte];
    line[0x7C] = nativeLine.unk7C;
    line[0x7F] = nativeLine.unk7F;
}

void func_801DFF5C(s32 categoryArg, s32 rowArg, s32 pageArg, s32 groupArg,
                   s32 gearModeArg, s32 modeFlagArg, s32 selectedArg) {
    u8 category = (u8)categoryArg;
    u8 group = (u8)groupArg;
    u8 gearMode = (u8)gearModeArg;
    u8 modeFlag = (u8)modeFlagArg;
    u8 selected = (u8)selectedArg;
    u8* listBuf;
    u8 itemId;
    u8 kind;
    u32 rawPtr;
    void* bundle = NULL;
    u8* state = (u8*)&g_GameState;
    u8 charId;
    u32 base;

    listBuf = MenuRawPointer(0x434);

    itemId = D_801EA730[rowArg + pageArg];
    if (modeFlag) itemId = 0xFF;
    if (itemId == 0) {
        listBuf[0xA18] = 0;
        return;
    }

    kind = 0;
    if (!group && category) kind = 1;
    kind = (u8)(kind + (gearMode << 1));

    charId = g_Menu->pManager->currentCharacterIDs[selected];
    base = charId * 0xA4;

    if (kind == 0) {
        memcpy(&rawPtr, listBuf + 0xA00, 4);
        bundle = (void*)(uintptr_t)rawPtr;
        if (modeFlag) {
            if (!group) {
                itemId = state[0x2D6 + base];
            } else {
                itemId = state[0x2DB + base + category];
            }
        }
    } else if (kind == 1) {
        memcpy(&rawPtr, listBuf + 0xA04, 4);
        bundle = (void*)(uintptr_t)rawPtr;
        if (modeFlag) itemId = state[0x2DF + base + category];
    } else if (kind == 2) {
        memcpy(&rawPtr, listBuf + 0xA08, 4);
        bundle = (void*)(uintptr_t)rawPtr;
        if (modeFlag) {
            u8 gearId = state[0x30C + base];
            u32 gbase = gearId * 0xA4;
            if (!group) {
                itemId = state[0x984 + gbase];
            } else {
                itemId = state[0x97C + gbase + category];
            }
        }
    } else if (kind == 3) {
        memcpy(&rawPtr, listBuf + 0xA0C, 4);
        bundle = (void*)(uintptr_t)rawPtr;
        if (modeFlag) {
            u8 gearId = state[0x30C + base];
            itemId = state[0x980 + gearId * 0xA4 + category];
        }
    }

    if (itemId == 0) {
        listBuf[0xA18] = 0;
        return;
    }

    {
        u8* renderBuffer = HeapAlloc(0x3F6, 0);
        s32 line;
        s32 descOff = 0x880;
        s32 entryBase = itemId * 3;

        bzero(renderBuffer, 0x3F6);
        for (line = 0; line < 3; line++) {
            u8* descLine = listBuf + descOff;
            s32 width;
            RECT rect;
            s32 slot = line + 8;

            width = SystemRenderStringEntry(
                GetStringEntry(bundle, entryBase + line), renderBuffer, 0x24, 0);
            descLine[0x7E] = (u8)width;
#ifdef XENO_PC_PORT
            {
                const char* ft = getenv("XENO_FIELD_TEST");
                if (ft && ft[0] == '1' && modeFlag && line == 0) {
                    u8* se = (u8*)GetStringEntry(bundle, entryBase);
                    void* an = GetAccessoryName((s32)itemId);
                    s32 anW = SystemRenderStringEntry(an, renderBuffer, 0x24, 0);
                    u16* offs = (u16*)bundle;
                    s32 i;
                    s32 nonempty = 0;
                    s32 firstRich = -1;
                    s32 richW = 0;
                    u32 nEnt = offs[0];
                    if (nEnt > 0x800) nEnt = 0x100;
                    for (i = 0; i < (s32)nEnt - 1; i++) {
                        u16 a = offs[2 + i];
                        u16 b = offs[3 + i];
                        if ((u16)(b - a) > 2) {
                            nonempty++;
                            if (firstRich < 0) {
                                u8* p = (u8*)bundle + a;
                                firstRich = i;
                                richW = SystemRenderStringEntry(
                                    p, renderBuffer, 0x24, 0);
                            }
                        }
                    }
                    printf("[xeno-port][test] Equip DFF5C render id=%u "
                           "entry=%d width=%d se0=%02x%02x "
                           "accNameW=%d hdr0=%04x nonempty=%d "
                           "firstRich=%d richW=%d\n",
                           (unsigned)itemId, entryBase, width,
                           se ? se[0] : 0, se ? se[1] : 0,
                           anW, (unsigned)offs[0], nonempty,
                           firstRich, richW);
                    {
                        s32 probeIds[] = {1, 16, 92, 31, firstRich / 3};
                        s32 pi;
                        for (pi = 0; pi < 5; pi++) {
                            s32 pid = probeIds[pi];
                            u8* p0 = (u8*)GetStringEntry(bundle, pid * 3);
                            s32 pw = SystemRenderStringEntry(
                                p0, renderBuffer, 0x24, 0);
                            void* pn = GetAccessoryName(pid);
                            s32 nw = SystemRenderStringEntry(
                                pn, renderBuffer, 0x24, 0);
                            printf("[xeno-port][test] Equip desc probe "
                                   "id=%d nameW=%d descW=%d se0=%02x%02x\n",
                                   pid, nw, pw, p0[0], p0[1]);
                        }
                    }
                    fflush(stdout);
                }
            }
#endif

            rect.x = (s16)((((slot & 1) * 3) << 3) + 0x180);
            rect.y = (s16)((slot >> 1) * 0xD + 0x80);
            rect.w = 0x28;
            rect.h = 0xD;
            LoadImage(&rect, (u_long*)renderBuffer);
            DrawSync(0);

#ifndef XENO_EQUIP_DESC_TEST
            EquipDescStageE7C50(descLine, slot, 0x80, 0x81);
            func_801C851C((SVECTOR*)(descLine + 0x50), 0x10,
                          (0x96 + (line << 4)) & 0xFFFE, descLine[0x7E], 0xD);
#endif
            descLine[0x7D] = ((u8*)g_Menu)[0x308];
            descOff += 0x80;
        }
        listBuf[0xA18] = 1;
        HeapFree(renderBuffer);
    }
}
#endif

void func_801E0434(u8 slotIdx, u8 mode) {
    u8* pTable;
    u8* pCounter;
    u8 val;
    u8 found = 1;
    s32 limit = 100;
    s32 i;

    if (mode == 0) {
        u8* pGS = (u8*)&g_GameState;
        u8 charIdx = ((u8*)g_Menu->pManager)[slotIdx + 0x30];
        pTable = pGS + 0x1D9C;
        pCounter = pGS + 0x1D38;
        val = pGS[0x2DB + charIdx * 0xA4];
        pGS[0x2DB + charIdx * 0xA4] = 0;
    } else {
        u8* pGS = (u8*)&g_GameState;
        u8 charIdx = ((u8*)g_Menu->pManager)[slotIdx + 0x30];
        u8 intermediate = pGS[0x30C + charIdx * 0xA4];
        pTable = pGS + 0x2120;
        pCounter = pGS + 0x20BC;
        val = pGS[0x97C + intermediate * 0xA4];
        pGS[0x97C + intermediate * 0xA4] = 0;
    }

    for (i = 0; i < limit; i++) {
        if (pTable[i] == val) {
            pCounter[i]++;
            if (pCounter[i] >= 100) {
                pCounter[i] = 99;
                found = 0;
            }
        }
    }

    if (found && limit > 0) {
        for (i = 0; i < limit; i++) {
            if (pTable[i] == 0) {
                pTable[i] = val;
                pCounter[i] = 1;
                break;
            }
        }
    }
}

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E05D0);
#else
extern u16 D_801E9DBC[];
extern s32 func_801DE5CC(s32, s32, s32, s32, s32);
extern void func_801DF5D0(s32, s32);
extern void func_801DFB68(s32, s32, s32, s32, s32, s32);
extern void func_801DFF5C(s32, s32, s32, s32, s32, s32, s32);
extern void func_801DF890(s32, s32);
extern s32 func_801DF0D4(s32, s32, s32, s32);
extern void func_801E36D4(void*, u8);
extern void func_801E3A80(void*, u8);
/* Full retail Equip control loop, 801E05D0..801E0F78. Rendering and
 * equipment-data helpers remain independent owners. */
void func_801E05D0(u8 selected, u8 openAnimation, u8 gearMode) {
    s32 page=0, category=0, row=0, count=0;
    s32 previousCategory=255, previousRow=255, previousPage=255;
    u8 group=0, previousSelected=255, redraw=1;
    u8 running=1, initialize=1, preview=0, showRows=0;
    u8 repeatConfirm=0, listing=0;
    ((u8*)g_Menu->pManager)[7]=0;
    func_801DE2C8(gearMode);
    func_801DF5D0(selected,gearMode);
    while(running) {
        func_801C7BF4();
        if(redraw || selected!=previousSelected) {
            func_801D8EA4(selected,(u8)(group+1),listing,gearMode);
            func_801DE474(group,gearMode);
            redraw=0;
        }
        if(page!=previousPage || selected!=previousSelected) {
            s32 scroll;
            count=func_801DE5CC(selected,page,category,group,gearMode);
#ifdef XENO_PC_PORT
            /* TEST TOOLING: remove with Equip acceptance harness. */
            {
                const char* ft = getenv("XENO_FIELD_TEST");
                if (ft && ft[0] == '1') {
                    u8* lb = MenuRawPointer(0x434);
                    printf("[xeno-port][test] Equip list DE5CC count=%d "
                           "gate4C=%u A18=%u A10=%u%u%u%u id0=%u\n",
                           count,
                           (unsigned)((u8*)g_Menu->pManager)[0x4C],
                           lb ? (unsigned)lb[0xA18] : 0u,
                           lb ? (unsigned)lb[0xA10] : 0u,
                           lb ? (unsigned)lb[0xA11] : 0u,
                           lb ? (unsigned)lb[0xA12] : 0u,
                           lb ? (unsigned)lb[0xA13] : 0u,
                           (unsigned)D_801EA730[0]);
                    fflush(stdout);
                }
            }
#endif
            if(count) {
                scroll=(page*100/count)/2;
                func_801D3344(0x94,scroll+0x12,0x32);
            } else func_801D3344(0x94,0x12,0x64);
        }
        if(showRows)func_801DB0A8(row,page,3,0);
        else ((u8*)g_Menu->pManager)[0x50]=0;
        if(row!=previousRow || selected!=previousSelected || page!=previousPage) {
            if(preview)func_801DFB68(selected,category,row,page,group,gearMode);
            preview=1;
            if(gearMode==0) {
                func_801E36D4(g_Menu->unk330,g_Menu->pManager->currentCharacterIDs[selected]);
                func_801E3A80(g_Menu->unk330,g_Menu->pManager->currentCharacterIDs[selected]);
            } else func_801DFE2C(selected);
            func_801DFF5C(category,row,page,group,gearMode,0,selected);
            func_801D8DE4(selected,1,listing,gearMode);
            previousRow=row;previousPage=page;
#ifdef XENO_PC_PORT
            /* TEST TOOLING: remove with Equip acceptance harness. */
            {
                const char* ft = getenv("XENO_FIELD_TEST");
                if (ft && ft[0] == '1') {
                    static int once;
                    if (!once) {
                        u8* lb = MenuRawPointer(0x434);
                        u32 tok = 0;
                        if (lb) memcpy(&tok, lb + 0xA00, 4);
                        printf("[xeno-port][test] Equip desc after DFF5C "
                               "A18=%u bundle=%08x itemRow0=%u flags40=%02x%02x\n",
                               lb ? (unsigned)lb[0xA18] : 0u,
                               (unsigned)tok,
                               (unsigned)D_801EA730[0],
                               (unsigned)((u8*)g_Menu->pManager)[0x40],
                               (unsigned)((u8*)g_Menu->pManager)[0x41]);
                        fflush(stdout);
                        once = 1;
                    }
                }
            }
#endif
        }
        if(category!=previousCategory || selected!=previousSelected) {
            POLY_FT4* p;
            u16 y;
            func_801DFF5C(category,row,page,group,gearMode,1,selected);
#ifdef XENO_PC_PORT
            /* TEST TOOLING: remove with Equip acceptance harness. */
            {
                const char* ft = getenv("XENO_FIELD_TEST");
                if (ft && ft[0] == '1') {
                    u8* lb = MenuRawPointer(0x434);
                    u8 charId = g_Menu->pManager->currentCharacterIDs[selected];
                    u8* st = (u8*)&g_GameState;
                    u32 base = charId * 0xA4;
                    u8 eq = category
                        ? st[0x2DF + base + category]
                        : st[0x2D6 + base];
                    printf("[xeno-port][test] Equip desc mode1 cat=%u A18=%u "
                           "equipped=%u char=%u\n",
                           (unsigned)category,
                           lb ? (unsigned)lb[0xA18] : 0u,
                           (unsigned)eq, (unsigned)charId);
                    if (lb && lb[0xA18]) {
                        s16* v0 = (s16*)(lb + 0x880 + 0x50);
                        printf("[xeno-port][test] Equip desc lines "
                               "w=%u/%u/%u rc=%u/%u/%u "
                               "xy0=(%d,%d) bundleA04=%08x\n",
                               (unsigned)lb[0x880 + 0x7E],
                               (unsigned)lb[0x900 + 0x7E],
                               (unsigned)lb[0x980 + 0x7E],
                               (unsigned)lb[0x880 + 0x7D],
                               (unsigned)lb[0x900 + 0x7D],
                               (unsigned)lb[0x980 + 0x7D],
                               (int)v0[0], (int)v0[1],
                               (unsigned)(*(u32*)(lb + 0xA04)));
                    }
                    fflush(stdout);
                }
            }
#endif
            p=&g_Menu->pCursors->polysCursor[g_Menu->pCursors->renderContexts[0]];
            y=D_801E9DBC[(group*4+category)*2];
            p->x0=0x8C;p->y0=y;p->x1=0x9C;p->y1=y;
            p->x2=0x8C;p->y2=y+0x10;p->x3=0x9C;p->y3=y+0x10;
            previousCategory=category;previousSelected=selected;
        }
        if(initialize) {
            func_801D397C(2,0x94,0xA,0x94,0x74,0,1,4,1);
            func_801D397C(3,0x6C,0x87,0xC4,0x48,0,1,4,0);
            func_801D397C(5,8,0x8E,0x60,0x40,0,1,4,0);
            if(openAnimation) {
                func_801D1E80();func_801D29A8(0,0);
                while(g_Menu->transitionEffectState)func_801C7BF4();
            }
            g_Menu->pCursors->shouldRender[0]=1;
            ((u8*)g_Menu->pManager)[6]=0;
            ((u8*)g_Menu->pManager)[0x21]=0;
            initialize=0;
        }
        ((u8*)g_Menu->pManager)[0x2F]=1;
        if(repeatConfirm){repeatConfirm=0;g_Menu->input=4;}
        if(!listing) {
            switch(g_Menu->input) {
            case 5:running=0;break;
            case 4:
                func_801DF5D0(selected,gearMode);
                page=0;listing=1;previousPage=previousRow=255;redraw=1;
                g_Menu->pCursors->shouldRender[0]=0;showRows=1;row=0;
                ((u8*)g_Menu->pManager)[0x50]=1;
                break;
            case 0:case 2:
                if(g_Menu->pManager->currentCharacterIDs[selected]==4) {
                    group^=1;previousCategory=255;redraw=1;category=0;
                }
                break;
            case 1:
                if(++category>=4)category=0;
                if(group && gearMode && (u32)(category-1)<2)category=3;
                break;
            case 3:
                if(--category<0)category=3;
                if(group && gearMode && (u32)(category-1)<2)category=0;
                break;
            case 9:case 10: {
                u8 direction=g_Menu->input==10;
                if(func_801D9704(selected,direction,gearMode)!=selected) {
                    selected=func_801D9704(selected,direction,gearMode);
                    group=0;preview=0;
                }
                break;
            }
            }
        } else {
            u8 leaveListing=0;
            switch(g_Menu->input) {
            case 5:
                func_801DF890(selected,gearMode);
                leaveListing=1;showRows=0;g_Menu->pCursors->shouldRender[0]=1;
                func_801DB0A8(0,page,3,0);
                row=0;previousSelected=255;((u8*)g_Menu->pManager)[0x50]=0;
                break;
            case 4:
                if((u8)func_801DF0D4(selected,(u8)category,group,gearMode)) {
                    group^=1;redraw=1;repeatConfirm=1;
                    func_801E0434(selected,gearMode);
                }
                previousSelected=255;
                func_801DF5D0(selected,gearMode);
                leaveListing=1;showRows=0;g_Menu->pCursors->shouldRender[0]=1;
                func_801DB0A8(0,page,3,0);
                row=0;((u8*)g_Menu->pManager)[0x50]=0;
                break;
            case 1:
                if(++row>=8) {++page;row=7;if(page>count)page=count;}
                break;
            case 3:
                if(--row<0) {--page;row=0;if(page<0)page=0;}
                break;
            }
            if(leaveListing) {
                row=page=0;listing=0;previousRow=255;redraw=1;preview=0;
                MenuRawPointer(0x35C)[0x32F2]=0;
            }
        }
    }
    func_801D2484();func_801DB340(0);
}
#endif

s32 func_801E0F78(u8 arg0, u8 arg1) {
    void* p1;
    void* pMenu;
    void* p2;
#ifdef XENO_PC_PORT
    /* TEST TOOLING: Equip acceptance harness. Prefer PAD_ON_CONTROL over
     * XENO_FIELD_TEST — FIELD_TEST takes the direct field-0 boot route and
     * breaks the natural title→NG path (run13). Remove with the harness. */
    {
        const char* ft = getenv("XENO_FIELD_TEST");
        const char* poc = getenv("XENO_PAD_ON_CONTROL");
        if ((ft && ft[0] == '1') ||
            (poc && strcmp(poc, "equip") == 0)) {
            printf("[xeno-port][test] Equip entry func_801E0F78 slot=%u "
                   "anim=%u\n",
                   (unsigned)arg0, (unsigned)arg1);
            fflush(stdout);
        }
    }
#endif
    p1 = HeapAlloc(0x32F4, NULL);
    pMenu = g_Menu;
    MenuStoreRawPointer(0x35C, p1);
    bzero(p1, 0x32F4);
    p2 = HeapAlloc(0x2AC, NULL);
    pMenu = g_Menu;
    MenuStoreRawPointer(0x360, p2);
    bzero(p2, 0x2AC);
    func_801C72BC(3);
    func_801E05D0(arg0, arg1, 0);
    func_801C72BC(0x13);
    return 1;
}

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E1014);
#endif

extern void func_801D3674(void);
extern void func_801D4EA0(s32);
extern void func_801E8044(s32, void*);
extern void func_801C72BC(s32);

void func_801E1398(void) {
    void* pMenu;
    func_801D3674();
    pMenu = g_Menu;
    ((u8*)g_Menu->pManager)[0x4D] = 0;
    func_801D4EA0(2);
    func_801C7BF4();
    pMenu = g_Menu;
    func_801E8044(2, (u8*)(g_Menu->pManager) + 0x4E);
    func_801C72BC(0x14);
    pMenu = g_Menu;
    HeapFree(*(void**)((u8*)pMenu + 0x438));
}

s32 func_801E1418(s32 slotIdx, u8 arg1) {
    s32 total = 0;
    s32 count = 0;
    s32 i;
    void* pMenu;
    void* pManager;
    u8 charIdx;
    u8* pGameData;
    u8* pEquip;
    void* pEquipData;
    u16* pEquipTable;
    if (arg1 >= 7) {
        if (!(*(u16*)((u8*)&g_GameState + 0x22B6) & 0x4000)) {
            return 0;
        }
    }
    pMenu = g_Menu;
    pManager = g_Menu->pManager;
    charIdx = *(u8*)((u8*)pManager + 0x30 + slotIdx);
    pEquipData = *(void**)((u8*)pMenu + 0x438);
    pGameData = (u8*)&g_GameState + 0x2FC + charIdx * 0x28;
    pEquip = (u8*)pEquipData + 0x2578;
    for (i = 0; i < 7; i++) {
        u16 val = *(u16*)(pGameData + i * 2);
        u8* pTableEntry = pEquip + charIdx * 0x280 + arg1 * 0xE;
        u16 equipVal = *(u16*)(pTableEntry + i * 2);
        if (val != 0) {
            if (equipVal == 0) goto next;
            if (equipVal == 0xFFFF) goto next;
            {
                s32 ratio = val * 100 / equipVal;
                if (ratio >= 100) {
                    total += 100;
                } else {
                    total += ratio;
                }
            }
            count++;
        } else {
            if (equipVal == 0) goto next;
            count++;
        }
next:
        ;
    }
    if (count != 0) {
        return total / count;
    }
    return 0;
}

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E1544);
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E1AC8);
#endif

extern void func_801E1014(void);
extern void func_801E1AC8(u8);
extern void func_801E1398(void);

s32 func_801E20C8(u8 slotIdx) {
    u8 lastSlot = 0xFF;
    u8 result = 1;
    void* pMenu = g_Menu;
    void* pManager = g_Menu->pManager;
    u8 slotType = *(u8*)((u8*)pManager + 0x30 + slotIdx);
    if (slotType - 7 < 2) {
        func_801C8574(4);
        return 1;
    }
    func_801E1014();
    while (result) {
        func_801C7BF4();
        if ((u8)slotIdx != (u8)lastSlot) {
            func_801E1AC8(slotIdx);
            lastSlot = slotIdx;
        }
        {
            u8 menuState = *(u8*)((u8*)g_Menu + 0x325);
            if (menuState == 9) {
                slotIdx = (u8)func_801D9704(0, 0);
                pMenu = g_Menu;
                pManager = g_Menu->pManager;
                while (*(u8*)((u8*)pManager + 0x30 + slotIdx) - 7 < 2) {
                    slotIdx = (u8)func_801D9704(0, 0);
                    pMenu = g_Menu;
                    pManager = g_Menu->pManager;
                }
            } else if (menuState == 10) {
                slotIdx = (u8)func_801D9704(1, 0);
                pMenu = g_Menu;
                pManager = g_Menu->pManager;
                while (*(u8*)((u8*)pManager + 0x30 + slotIdx) - 7 < 2) {
                    slotIdx = (u8)func_801D9704(1, 0);
                    pMenu = g_Menu;
                    pManager = g_Menu->pManager;
                }
            } else if (menuState == 5) {
                result = 0;
            }
        }
    }
    func_801E1398();
    return 1;
}

extern void func_801C72BC(s32);
#ifndef XENO_PC_PORT
extern void func_801D3488(s32, s32);
#endif

u8 func_801E2250(void) {
    void* pMenu;
    void* pManager;
    s32 i;
    void* p1 = HeapAlloc(0x2AF0, NULL);
    pMenu = g_Menu;
    MenuStoreRawPointer(0x358, p1);
    bzero(p1, 0x2AF0);
    {
        void* p2 = HeapAlloc(0x32F4, NULL);
        pMenu = g_Menu;
        MenuStoreRawPointer(0x35C, p2);
        bzero(p2, 0x32F4);
    }
    {
        void* p3 = HeapAlloc(0x2AC, NULL);
        pMenu = g_Menu;
        MenuStoreRawPointer(0x360, p3);
        bzero(p3, 0x2AC);
    }
    func_801C72BC(3);
    pMenu = g_Menu;
    pManager = g_Menu->pManager;
    i = 0;
    while (*(u8*)((u8*)pManager + 0x60 + i) == 0) {
        i++;
    }
    func_801D3488(0, 1);
    return (u8)(i & 0xFF);
}

extern u8 D_801EA568[];
#ifndef XENO_PC_PORT
extern void func_801E8018(s32, u8*, s32, void*);
#endif

void func_801E2324(u8 arg0) {
    void* pMenu = g_Menu;
    void* pManager = g_Menu->pManager;
    func_801E8018(6, (u8*)pMenu + 0x18E0, D_801EA568[arg0], (u8*)pManager + 0x54);
}

void func_801E2368(void) {
    void* pMenu = g_Menu;
    HeapFree(MenuRawPointer(0x358));
    pMenu = g_Menu;
    HeapFree(MenuRawPointer(0x35C));
    pMenu = g_Menu;
    HeapFree(MenuRawPointer(0x360));
    func_801C72BC(0x13);
}

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E23CC);
#endif

extern void func_801D249C(s32);
#ifndef XENO_PC_PORT
extern void func_801D3488(s32, s32);
#endif

void func_801E2AE0(void) {
    void* pMenu;
    func_801D249C(1);
    {
        void* p1 = HeapAlloc(0x2AF0, NULL);
        pMenu = g_Menu;
        MenuStoreRawPointer(0x358, p1);
        bzero(p1, 0x2AF0);
    }
    {
        void* p2 = HeapAlloc(0x32F4, NULL);
        pMenu = g_Menu;
        MenuStoreRawPointer(0x35C, p2);
        bzero(p2, 0x32F4);
    }
    {
        void* p3 = HeapAlloc(0x2AC, NULL);
        pMenu = g_Menu;
        MenuStoreRawPointer(0x360, p3);
        bzero(p3, 0x2AC);
    }
    func_801C72BC(3);
    func_801D3488(0, 0);
}

void func_801E2B80(void) {
    void* pMenu = g_Menu;
    HeapFree(MenuRawPointer(0x358));
    pMenu = g_Menu;
    HeapFree(MenuRawPointer(0x35C));
    pMenu = g_Menu;
    HeapFree(MenuRawPointer(0x360));
    func_801C72BC(0x13);
}

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E2BE4);
#else
/* The retail Status controller is not translated yet.  Calling the generated
 * port stub returned without running func_801E2AE0, but func_801C531C still
 * dispatched the retail Status teardown, which freed three uninitialized work
 * pointers.  Build the matching Status work set before returning to the root
 * menu so that the shared teardown remains balanced and deterministic.
 *
 * This is intentionally a safe-return bridge, not a claim that the Status
 * screen itself is implemented.  Replace this body with the retail controller
 * when that routine and its input/render loop are translated. */
s32 func_801E2BE4(void) {
    func_801E2AE0();
    printf("[xeno-port][menu] Status screen is not ported yet; returning to main menu\n");
    fflush(stdout);
    return 1;
}
#endif

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

void func_801E35BC(void* pCtx, u8 charIdx, u8 statIdx, u8 slotIdx, s32 isReverse) {
    u8* pChar = (u8*)&g_GameState + 0x26C + charIdx * 0x28;
    u8* pStat = (u8*)&g_GameState + 0x26C + statIdx * 0x28;
    u8 entryByte = *(u8*)((u8*)&g_GameState + 0x30C + charIdx * 0x28);
    u8* pBase = (u8*)&g_GameState + 0x30C + entryByte * 0x28;
    if (isReverse == 0) {
        u8* pSlot = *(u8**)((u8*)pCtx + slotIdx * 4 + 0x20);
        u8* pSlotData = pSlot + slotIdx * 0x28;
        u8 val = pChar[0x5B];
        u8 rate = pSlotData[0x11];
        u16 cur = *(u16*)(pStat + 0x4C);
        cur += val * rate;
        *(u16*)(pStat + 0x4C) = cur;
        if (*(u16*)(pStat + 0x4E) < cur) {
            *(u16*)(pStat + 0x4C) = *(u16*)(pStat + 0x4E);
        }
    } else {
        u32 val = *(u32*)(pBase + 0x64);
        u32 step = val / 10;
        u32 cur = *(u32*)(pBase + 0x60) + step;
        *(u32*)(pBase + 0x60) = cur;
        if (val < cur) {
            *(u32*)(pBase + 0x60) = val;
        }
    }
}

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E36D4);
#else
/* Retail character equipment effects. Access packed records by byte so the
 * native pointer width cannot change the 16-byte item or 0xA4 character stride. */
void func_801E36D4(void* resourceArg, u8 characterId) {
    MenuUnk6* resources = resourceArg;
    u8* character = (u8*)&g_GameState.characters[characterId];
    u8* accessories = (u8*)resources->pAccessoriesData;
    u8* weapons = (u8*)resources->pWeaponsData;
    static const u8 bonusOffsets[8] = {0x28,0x29,0x2A,0x2B,0x2C,0x2E,0x2F,0x2D};
    s32 i, bit;
    for (i = 0x28; i <= 0x33; i++) character[i] = 0;
    for (i = 0x7E; i <= 0x8E; i += 4) {
        character[i] = 0;
        character[i + 1] = 0;
    }
    character[0xA1] = 0;
    for (i = 0; i < 3; i++) {
        u8* item = accessories + character[0x74 + i] * 16;
        s32 target = -1;
        character[0x2D] += item[8];
        switch (item[9]) {
        case 1: target = 0x7E; break;
        case 2: target = 0x82; break;
        case 3: target = 0x86; break;
        case 4: target = 0x8A; break;
        case 5: target = 0x32; break;
        case 7: target = 0x8E; break;
        case 8: case 9: character[0x30] += item[0xA]; break;
        case 10: character[0xA1] += item[0xA]; break;
        }
        if (target >= 0) {
            character[target] |= item[0xA];
            character[target + 1] |= item[0xB];
        }
        for (bit = 0; bit < 8; bit++) {
            if (item[0xD] & (0x80 >> bit)) character[bonusOffsets[bit]] += item[0xC];
        }
    }
    /* Character type 4 replaces the primary weapon and also writes a
     * secondary weapon block. Other characters leave that block untouched. */
    for (i = 0; i < (character[0x56] == 4 ? 3 : 1); i++) {
        s32 equipment = i == 0 ? 0x6A : i == 1 ? 0x6F : 0x72;
        s32 destination = i == 2 ? 0x18 : 0;
        u8* weapon = weapons + character[equipment] * 16;
        character[destination + 4] = weapon[0xC];
        character[destination] = weapon[8];
        character[destination + 1] = weapon[9];
        character[destination + 2] = weapon[0xA];
        character[destination + 3] = weapon[0xB];
    }
    if (character[3] == 0x64) {
        character[0x8E] |= character[0];
        character[0x8F] |= character[1];
    }
}
#endif

/* Retail 801E3A80: aggregate character stats into the Equip resource block
 * (unk330+0xB8..0xC4), not into the GameState character record. */
void func_801E3A80(void* pCtx, u8 charIdx) {
    u8* pEntry = (u8*)&g_GameState.characters[charIdx];
    #ifdef XENO_PC_PORT
    u8* pDst = (u8*)&((MenuUnk6*)pCtx)->unkB8;
#else
    u8* pDst = (u8*)pCtx + 0xB8;
#endif
    s32 val;

    if (pEntry[0x56] == 4) {
        val = (s32)(pEntry[4] + pEntry[0x1C]) * 6 / 10;
        *(u16*)(pDst + 0x0) = (u16)val;
    } else {
        val = pEntry[0x58] + pEntry[0x28] + pEntry[4];
        *(u16*)(pDst + 0x0) = (u16)val;
    }
    *(u16*)(pDst + 0x2) = (u16)(pEntry[0x5E] + pEntry[0x2E]);
    *(u16*)(pDst + 0x4) = (u16)(pEntry[0x59] + pEntry[0x29] + pEntry[0x2D]);
    *(u16*)(pDst + 0x6) = (u16)(pEntry[0x5F] + pEntry[0x2F]);
    *(u16*)(pDst + 0x8) = (u16)(pEntry[0x5B] + pEntry[0x2B]);
    *(u16*)(pDst + 0xA) = (u16)(pEntry[0x5C] + pEntry[0x2C]);
    *(u16*)(pDst + 0xC) = (u16)(pEntry[0x5A] + pEntry[0x2A]);

    if (*(u16*)(pDst + 0x0) >= 0xFB) {
        *(u16*)(pDst + 0x0) = 0xFA;
    }
    if (*(u16*)(pDst + 0x2) >= 0x64) {
        *(u16*)(pDst + 0x2) = 0x63;
    }
    if (*(u16*)(pDst + 0x4) >= 0xFB) {
        *(u16*)(pDst + 0x4) = 0xFA;
    }
    if (*(u16*)(pDst + 0x6) >= 0x64) {
        *(u16*)(pDst + 0x6) = 0x63;
    }
    if (*(u16*)(pDst + 0x8) >= 0xFB) {
        *(u16*)(pDst + 0x8) = 0xFA;
    }
    if (*(u16*)(pDst + 0xA) >= 0xFB) {
        *(u16*)(pDst + 0xA) = 0xFA;
    }
    if (*(u16*)(pDst + 0xC) >= 0x15) {
        *(u16*)(pDst + 0xC) = 0x10;
    }
}

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E3C2C);
#else
extern u8 D_801E9808[];
/* Gear preview aggregate. Resource offsets 0x9C..0xB6 are inside the
 * pointer-free unk20 block; the host shifts that block after wider pointers. */
void func_801E3C2C(void* resourceArg, u8 gearId) {
    u8* state = (u8*)&g_GameState;
    u8* gear = (u8*)&g_GameState.gears[gearId];
    u8* out = ((MenuUnk6*)resourceArg)->unk20 + 0x7C;
    u8* character;
    u32 attack;
    if (*(u16*)(state + 0x22B6) & 0x1000) D_801E9808[9] = 10;
    if (gearId == 7) {
        *(u32*)(state + 0xE54) = *(u16*)(state + 0x734) * 50;
        *(u32*)(state + 0xE58) = *(u16*)(state + 0x736) * 50;
        state[0xE30] = state[0x740] + state[0x710];
        *(u16*)(state + 0xE64) = (state[0x741] + state[0x711]) * 12;
        *(u16*)(state + 0xE66) = (state[0x744] + state[0x714]) * 6;
        state[0xE8C] = state[0x742] + state[0x712];
    }
    character = (u8*)&g_GameState.characters[D_801E9808[gearId]];
    *(u32*)(out + 0) = *(u32*)(gear + 0x60);
    *(u32*)(out + 4) = *(u32*)(gear + 0x64);
    *(u16*)(out + 8) = *(u16*)(gear + 0x70) + *(u16*)(gear + 0x40);
    *(u16*)(out + 10) = character[0x5C] + character[0x2C] +
                         *(u16*)(gear + 0x42) + *(u16*)(gear + 0x72);
    *(u16*)(out + 12) = *(u16*)(gear + 0x68) + *(u16*)(gear + 0x44);
    *(u16*)(out + 14) = *(u16*)(gear + 0x6A);
    *(u16*)(out + 16) = *(u16*)(gear + 0x38);
    *(u16*)(out + 18) = *(u16*)(gear + 0x3A);
    attack = gear[0x3C] * (gear[0x74] + gear[0x56]);
    attack += (gearId == 5 || gearId == 13) ?
              (gear[0x12] + gear[0x22]) * 6 / 10 : gear[0x12];
    *(u16*)(out + 20) = attack;
    out[22] = gear[0x9F] + gear[0x4D];
    out[23] = gear[0x98] - gear[0x4A];
    out[24] = gear[0x9E] + gear[0x54];
    out[25] = gear[0x9D];
    out[26] = gear[0x9C];
}
#endif

extern void func_801E433C(void*, u8);
extern void func_801E4754(void*, u8);

void func_801E3ECC(void* arg0, u8 idx) {
    u8 i;

    func_801E433C(arg0, idx);
    func_801E4754(arg0, idx);

    switch (idx) {
    case 0:
        for (i = 0; i < 3; i++) ((u8*)&g_GameState)[0xA25 + i] = ((u8*)&g_GameState)[0x981 + i];
        func_801E433C(arg0, 1);
        break;
    case 1:
        for (i = 0; i < 3; i++) ((u8*)&g_GameState)[0x131D + i] = ((u8*)&g_GameState)[0xA25 + i];
        func_801E433C(arg0, 0xF);
        break;
    case 2:
        for (i = 0; i < 3; i++) ((u8*)&g_GameState)[0xFE9 + i] = ((u8*)&g_GameState)[0xAC9 + i];
        func_801E433C(arg0, 0xA);
        break;
    case 3:
        for (i = 0; i < 3; i++) ((u8*)&g_GameState)[0x108D + i] = ((u8*)&g_GameState)[0xB6D + i];
        func_801E433C(arg0, 0xB);
        break;
    case 4:
        for (i = 0; i < 3; i++) ((u8*)&g_GameState)[0x1131 + i] = ((u8*)&g_GameState)[0xC11 + i];
        func_801E433C(arg0, 0xC);
        ((u8*)&g_GameState)[0x1134] = ((u8*)&g_GameState)[0xC14];
        func_801E4754(arg0, 0xC);
        break;
    case 5:
        for (i = 0; i < 3; i++) ((u8*)&g_GameState)[0x11D5 + i] = ((u8*)&g_GameState)[0xCB5 + i];
        func_801E433C(arg0, 0xD);
        ((u8*)&g_GameState)[0x11D8] = ((u8*)&g_GameState)[0xCB8];
        ((u8*)&g_GameState)[0x11DB] = ((u8*)&g_GameState)[0xCBB];
        ((u8*)&g_GameState)[0x11D0] = ((u8*)&g_GameState)[0xCB0];
        ((u8*)&g_GameState)[0x11D3] = ((u8*)&g_GameState)[0xCB3];
        func_801E4754(arg0, 0xD);
        break;
    case 6:
        for (i = 0; i < 3; i++) ((u8*)&g_GameState)[0x1279 + i] = ((u8*)&g_GameState)[0xD59 + i];
        func_801E433C(arg0, 0xE);
        break;
    }
}

extern void func_801E41C0(s32, u8);
extern void func_801E42AC(s32, u8);

void func_801E4170(s32 arg0, u8 arg1) {
    func_801E41C0(arg0, arg1);
    func_801E42AC(arg0, arg1);
    func_801E4258(arg0, arg1);
}

void func_801E41C0(s32 arg0, u8 idx) {
    u8* pEntry = (u8*)&g_GameState + 0x978 + idx * 0x28;
    u8 tableIdx = pEntry[2];
    u8* pTable = *(u8**)((u8*)arg0 + 8) + tableIdx * 0x18;
    u32 val = *(u32*)(pTable + 4);
    *(u32*)(pEntry + 0x60) = val;
    *(u32*)(pEntry + 0x64) = val;
    pEntry[0x98] = pTable[0x14];
    pEntry[0x9E] = pTable[0x15];
    pEntry[0x9D] = pTable[0x16];
    {
        u32 prev = *(u32*)(pEntry + 0x64);
        if (prev < *(u32*)(pEntry + 0x60)) {
            pEntry[0x9F] = pTable[0x17];
            *(u32*)(pEntry + 0x60) = prev;
        }
    }
}

void func_801E4258(void* pCtx, u8 idx) {
    s32 i = idx;
    u8* pBase = (u8*)&g_GameState;
    u8* pEntry = pBase + 0x978 + i * 0x28;
    s32 tableIdx = pEntry[8];
    u8* pTbl = *(u8**)((u8*)pCtx + 0x10);
    *(u16*)(pEntry + 0x70) = *(u16*)(pTbl + tableIdx * 0x14 + 8);
    *(u16*)(pEntry + 0x72) = *(u16*)(pTbl + tableIdx * 0x14 + 0xA);
}

void func_801E42AC(s32 arg0, u8 idx) {
    u8* pEntry = (u8*)&g_GameState + 0x978 + idx * 0x28;
    u8 tableIdx = pEntry[3];
    u8* pTable = *(u8**)((u8*)arg0 + 0xC) + tableIdx * 0x10;
    u16 prev = *(u16*)(pEntry + 0x38);
    *(u16*)(pEntry + 0x3A) = *(u16*)(pTable + 6);
    pEntry[0x3C] = pTable[0xC];
    pEntry[0x3D] = pTable[0xD];
    pEntry[0x3E] = pTable[0xE];
    if (*(u16*)(pEntry + 0x3A) < prev) {
        pEntry[0x3F] = pTable[0xE];
        *(u16*)(pEntry + 0x38) = *(u16*)(pEntry + 0x3A);
    }
}

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E433C);
#else
extern u8 func_801E4928(u8);
/* Retail Gear accessories: resource+0x14 is a retained 32-bit pointer slot. */
void func_801E433C(void* resourceArg, u8 gearId) {
    MenuUnk6* resource = resourceArg;
    u8* state = MenuEquipmentStateStorage;
    u8* gear = state + 0x978 + gearId * 0xA4;
    u8 character = D_801E9808[gearId];
    u16* flags = (u16*)(state + 0x16C4 + character * 0x20);
    u16* status = (u16*)(state + 0x16DA + character * 0x20);
    u8* table = (u8*)(uintptr_t)*(u32*)&resource->unk8[0xC];
    s32 i, j;
    *(u16*)(gear + 0x40) = 0;
    *(u16*)(gear + 0x42) = 0;
    *(u16*)(gear + 0x44) = 0;
    *(u16*)(gear + 0x48) = 0;
    for (i = 0x4C; i <= 0x57; i++) gear[i] = 0;
    *(u16*)(gear + 0x6E) = 0;
    for (i = 0x88; i < 0x98; i++) gear[i] = 0;
    *(u16*)(gear + 0x7E) = 0;
    *(u16*)(gear + 0x82) = 0;
    *(u16*)(gear + 0x86) &= 0xF000;
    *flags &= 0xFB6F;
    for (i = 0; i < 3; i++) {
        u8* item = table + gear[9 + i] * 28;
        u16 effect = *(u16*)(item + 0x16);
        *(u16*)(gear + 0x40) += item[0xD];
        *(u16*)(gear + 0x42) += item[0xE];
        *(u16*)(gear + 0x44) += *(u16*)(item + 6);
        gear[0x4C] += item[0x18];
        gear[0x4D] += item[0x14];
        gear[0x54] += item[0x1B];
        for (j = 0; j < 4; j++) gear[0x50 + j] += item[0x10 + j];
        switch (item[0x15]) {
        case 1: *(u16*)(gear + 0x7E) |= effect; break;
        case 2: *(u16*)(gear + 0x82) |= effect; break;
        case 3: *(u16*)(gear + 0x86) |= effect; break;
        case 4:
            *(u16*)(gear + 0x6E) |= effect;
            for (j = 0; j < 16; j++)
                if (effect & (0x8000 >> j)) gear[0x88 + j] += item[0x1A];
            break;
        case 5: gear[0x4F] += item[0x16]; break;
        case 6: if ((*flags & 0x1800) == 0x1800) *flags |= 0x400; break;
        case 7: if ((*flags & 0x300) == 0x300) *flags |= 0x80; break;
        case 8: if ((*flags & 0x60) == 0x60) *flags |= 0x10; break;
        case 9:
            *(u16*)(gear + 0x48) |= effect;
            /* Retail falls through to type 10. */
        case 10: gear[0x56] += item[0x16]; break;
        case 11: gear[0x57] += item[0x16]; break;
        }
    }
    gear[0x4A] = func_801E4928(gearId);
    if (gear[0x4F]) *status |= 0x8000;
    else if (state[0x30C + D_801E9808[gearId] * 0xA4] == gearId)
        *status &= 0x7FFF;
}
#endif

void func_801E4754(void* arg0, u8 charIdx) {
    u8* pGS = (u8*)&g_GameState + 0x978 + charIdx * 0xA4;
#ifdef XENO_PC_PORT
    u32* pSrcBase = (u32*)&((MenuUnk6*)arg0)->unk8[0x10];
#else
    void** pSrcBase = (void**)((u8*)arg0 + 0x18);
#endif
    u8 slot;
    u8* pSrc;

    slot = pGS[0xC];
    pSrc = (u8*)*pSrcBase + slot * 0x14;
    pGS[0x12] = pSrc[0xE];
    *(u16*)(pGS + 0x10) = *(u16*)(pSrc + 0x12);
    pGS[0x13] = pSrc[0x10];
    pGS[0x14] = pSrc[0x11];
    pGS[0x5C] = pSrc[0];
    pGS[0x5D] = pSrc[1];
    pGS[0x5E] = pSrc[2];
    pGS[0x5F] = pSrc[3];
    if (pGS[0x14] == 0x64) {
        u16 val = *(u16*)(pGS + 0x86);
        val = (val & 0xFFF) | *(u16*)(pGS + 0x10);
        *(u16*)(pGS + 0x86) = val;
    }
    if (charIdx == 5 || charIdx == 0xD) {
        slot = pGS[0x4];
        pSrc = (u8*)*pSrcBase + slot * 0x14;
        pGS[0x12] = pSrc[0xE];
        *(u16*)(pGS + 0x10) = *(u16*)(pSrc + 0x12);
        pGS[0x13] = pSrc[0x10];
        pGS[0x14] = pSrc[0x11];
        pGS[0x5D] = pSrc[1];

        slot = pGS[0x5];
        pSrc = (u8*)*pSrcBase + slot * 0x14;
        pGS[0x1A] = pSrc[0xE];
        *(u16*)(pGS + 0x18) = *(u16*)(pSrc + 0x12);
        pGS[0x1B] = pSrc[0x10];
        pGS[0x1C] = pSrc[0x11];
        pGS[0x5E] = pSrc[2];

        slot = pGS[0x7];
        pSrc = (u8*)*pSrcBase + slot * 0x14;
        pGS[0x22] = pSrc[0xE];
        *(u16*)(pGS + 0x20) = *(u16*)(pSrc + 0x12);
        pGS[0x23] = pSrc[0x10];
        pGS[0x24] = pSrc[0x11];
        pGS[0x5F] = pSrc[3];
    }
}

u8 func_801E4928(u8 idx) {
    u8* pEntry = (u8*)&g_GameState + 0x978 + (s32)idx * 0xA4;
    u16 val = *(u16*)(pEntry + 0x44);
    u8 base = pEntry[0x75];
    s32 result = (s32)(val / 120) - base;
    result /= 2;
    if (result < 0) result = 0;
    return (u8)(result & 0xFF);
}

void func_801E4998(s32 arg0, u8 idx) {
    u8* pEntry = (u8*)&g_GameState + 0x9DC + (s32)idx * 0xA4;
    u32 val = *(u32*)pEntry;
    u32 temp1 = (u32)(((unsigned long long)val * 0xCCCCCCCDULL) >> 32) >> 3;
    u32 temp2 = (u32)(((unsigned long long)(temp1 * 2) * 0x38E38E39ULL) >> 32) >> 1;
    u8* pModel = *(u8**)((u8*)arg0 + idx * 4 + 0x4C) + 0x5C8;
    *(u16*)(pModel + 0x24) = (u16)(temp2 / 2);
    {
        u16 result = *(u16*)(pModel + 0x24);
        u32 rounded = (u32)(((unsigned long long)result * 0xCCCCCCCDULL) >> 32) >> 3;
        *(u16*)(pModel + 0x24) = (u16)(rounded * 10);
    }
}

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E4A28);
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E4D10);
#endif

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

void func_801E5178(void) {
    u8* gs = (u8*)&g_GameState;

    *(u16*)(gs + 0x1D30) = 0x7FF;

    *(u16*)(gs + 0x16C0) = 0xFFF8;
    *(u16*)(gs + 0x16C2) = 0xFF00;
    *(u16*)(gs + 0x16C4) = 0xFFF0;
    *(u16*)(gs + 0x16C6) = 0xFE00;
    *(u8*)(gs + 0x16D7) = 0x7;
    *(u16*)(gs + 0x16DA) = 0xE000;
    *(u16*)(gs + 0x16E0) = 0xFFE0;
    *(u16*)(gs + 0x16E2) = 0xFFF0;
    *(u16*)(gs + 0x16E4) = 0xFFF0;
    *(u16*)(gs + 0x16E6) = 0xFFF0;
    *(u8*)(gs + 0x16F7) = 0x7;
    *(u16*)(gs + 0x16FA) = 0xC000;
    *(u16*)(gs + 0x1700) = 0xFFE0;
    *(u16*)(gs + 0x1702) = 0xFFE0;
    *(u16*)(gs + 0x1704) = 0xFFF0;
    *(u16*)(gs + 0x1706) = 0xFF00;
    *(u8*)(gs + 0x1717) = 0x7;
    *(u16*)(gs + 0x171A) = 0x8000;
    *(u16*)(gs + 0x1720) = 0xFFE0;
    *(u16*)(gs + 0x1722) = 0xFFC0;
    *(u16*)(gs + 0x1724) = 0xFFF0;
    *(u16*)(gs + 0x1726) = 0xFF00;
    *(u8*)(gs + 0x1737) = 0x7;
    *(u16*)(gs + 0x173A) = 0xFE00;
    *(u16*)(gs + 0x1740) = 0xFFC0;
    *(u16*)(gs + 0x1742) = 0xFFC0;
    *(u16*)(gs + 0x1744) = 0xFFF0;
    *(u16*)(gs + 0x1746) = 0xFFC0;
    *(u8*)(gs + 0x1757) = 0x7;
    *(u16*)(gs + 0x175A) = 0xE000;
    *(u16*)(gs + 0x1760) = 0xFFC0;
    *(u16*)(gs + 0x1762) = 0xFE00;
    *(u16*)(gs + 0x1764) = 0xFFF0;
    *(u16*)(gs + 0x1766) = 0xFE00;
    *(u8*)(gs + 0x1777) = 0x7;
    *(u16*)(gs + 0x177A) = 0x8000;
    *(u16*)(gs + 0x1780) = 0xFFC0;
    *(u16*)(gs + 0x1782) = 0xFF00;
    *(u16*)(gs + 0x1784) = 0xFFF0;
    *(u16*)(gs + 0x1786) = 0xFF00;
    *(u8*)(gs + 0x1797) = 0x7;
    *(u16*)(gs + 0x179A) = 0x8000;
    *(u16*)(gs + 0x17A0) = 0;
    *(u16*)(gs + 0x17A2) = 0xFF00;
    *(u16*)(gs + 0x17A4) = 0;
    *(u16*)(gs + 0x17A6) = 0xFF00;
    *(u8*)(gs + 0x17B7) = 0x7;
    *(u16*)(gs + 0x17BA) = 0;
    *(u16*)(gs + 0x17C0) = 0;
    *(u16*)(gs + 0x17C2) = 0xF800;
    *(u16*)(gs + 0x17C4) = 0xFFF0;
    *(u16*)(gs + 0x17C6) = 0;
    *(u8*)(gs + 0x17D7) = 0x7;
    *(u16*)(gs + 0x17DA) = 0xE000;
    *(u16*)(gs + 0x17E0) = 0xFFE0;
    *(u16*)(gs + 0x17E2) = 0xFFE0;
    *(u16*)(gs + 0x17E4) = 0xFFF0;
    *(u16*)(gs + 0x17E6) = 0xFFE0;
    *(u8*)(gs + 0x17F7) = 0x7;
    *(u16*)(gs + 0x17FA) = 0x8000;
    *(u16*)(gs + 0x1800) = 0xFFC0;
    *(u16*)(gs + 0x1802) = 0xFF00;
    *(u16*)(gs + 0x1804) = 0xFFF0;
    *(u16*)(gs + 0x1806) = 0xFF00;
    *(u8*)(gs + 0x1817) = 0x7;
    *(u16*)(gs + 0x181A) = 0x8000;
}

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

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E56E8);
#endif

extern u16 D_801E9894[];
extern u16 D_801E9914[];

void func_801E5924(s32 slotIdx) {
    u16* pX = &D_801E9894[slotIdx];
    u16* pY = &D_801E9914[slotIdx];
    void* pMenu = g_Menu;
    u8* pBuf = MenuRawPointer(0x3A8 + slotIdx * 4);
    u8* pBase = pBuf;
    s32 i;
    u32 off1 = 0x50;
    u32 off2 = 0x80;
    for (i = 0; i < 2; i++) {
        SetLineF3(pBuf + off1);
        pBase[0x54] = 0; pBase[0x55] = 0xFF; pBase[0x56] = 0;
        pBase[0x58] = (u8)*pX; pBase[0x5A] = (u8)*pY;
        pBase[0x5C] = (u8)(*pX + 0x10); pBase[0x5E] = (u8)*pY;
        pBase[0x60] = (u8)(*pX + 0x10); pBase[0x62] = (u8)(*pY + 0x10);
        func_801C851C(pBuf + 0x100, *pX, *pY, 0x10, 0x10);
        SetLineF3(pBuf + off2);
        pBase[0x84] = 0; pBase[0x85] = 0xFF; pBase[0x86] = 0;
        pBase[0x88] = (u8)*pX; pBase[0x8A] = (u8)*pY;
        pBase[0x8C] = (u8)(*pX + 0x10); pBase[0x8E] = (u8)(*pY + 0x10);
        pBase[0x90] = (u8)(*pX + 0x10); pBase[0x92] = (u8)(*pY + 0x10);
        func_801C851C(pBuf + 0x120, *pX, *pY, 0x10, 0x10);
        off1 += 0x18;
        off2 += 0x18;
        pBase += 0x18;
    }
}

extern void func_801E56E8(s32);

void func_801E5ACC(void) {
    s32 i;
    for (i = 0; i < 0x20; i++) {
        void* pBuf = HeapAlloc(0x158, NULL);
        void* pMenu = g_Menu;
        MenuStoreRawPointer(0x3A8 + i * 4, pBuf);
        bzero(pBuf, 0x158);
        func_801E56E8(i);
        func_801E5924(i);
    }
}

void func_801E5B3C(void) {
    s32 i;
    for (i = 0; i < 0x20; i++) {
        void* pMenu = g_Menu;
        HeapFree(MenuRawPointer(0x3A8 + i * 4));
    }
}

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E5B88);
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E5E4C);
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E61B0);
#else
extern u32 D_801EA494[], D_801E9F98[], D_801E9FBC[];
extern u16 D_801EA590[], D_801EA5DC[];
extern void func_801E927C(POLY_FT4*);
extern void func_801E920C(POLY_FT4*, s32, s32, s32, s32, s32, s32);

void func_801E61B0(void) {
    s32 slot, i;
    for (slot = 0; slot < 3; slot++) {
        u32 offset = slot * 0x87C;
        u8* panel = MenuRawPointer(0x34C);
        POLY_FT4* p;
        u16 page;
        u32 atlas;
        panel[offset + 0x1312] = 0;
        for (i = 0; i < 9; i++) {
            if (D_801EA494[i] != 0xFFFF) {
                s32 written;
                panel = MenuRawPointer(0x34C);
                written = func_8002675C(
                    g_Menu->unk2DC, D_801EA494[i],
                    panel + offset + 0xA98 + 0x50 + panel[offset + 0x1312] * 0x50,
                    g_Menu->renderContext, D_801E9F98[i] + slot * 0x50,
                    D_801E9FBC[i], 0x1000);
                panel = MenuRawPointer(0x34C);
                panel[offset + 0x1312] += (u8)written;
            }
        }
        MenuRawPointer(0x34C)[offset + 0x130E] = (u8)g_Menu->renderContext;
        p = (POLY_FT4*)(MenuRawPointer(0x34C) + offset + 0x12B8) + g_Menu->renderContext;
        func_801E927C(p);
        page = GetTPage(0, 0, 384, 0);
        p = (POLY_FT4*)(MenuRawPointer(0x34C) + offset + 0x12B8) + g_Menu->renderContext;
        p->tpage = page;
        p->clut = g_SystemPalette1;
        /* Retail consumes a word and a byte at four-byte table strides,
         * despite adjacent consumers declaring these tables as halfwords. */
        memcpy(&atlas, (u8*)D_801EA590 + slot * 4, sizeof(atlas));
        func_801E920C(p, (u16)((u16)D_801E9F98[0] + slot * 0x50),
                     (u16)((u16)D_801E9FBC[0] + 7), (atlas * 4) & 0xFC,
                     ((u8*)D_801EA5DC)[slot * 4], 0x48, 0x0D);
        MenuRawPointer(0x34C)[offset + 0x1311] = (u8)g_Menu->renderContext;
    }
}
#endif

extern void func_801E5B88(void);
extern void func_801E5E4C(void);

void func_801E6450(void) {
    volatile u64 reserve;
    void* pBuf = HeapAlloc(0x2DC0, NULL);
    void* pMenu = g_Menu;
    MenuStoreRawPointer(0x34C, pBuf);
    bzero(pBuf, 0x2DC0);
    func_801E5B88();
    func_801E5E4C();
}

void func_801E649C(void) {
    void* pMenu = g_Menu;
    HeapFree(MenuRawPointer(0x34C));
    pMenu = g_Menu;
    ((u8*)g_Menu->pManager)[0xB] = 0;
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
        ((u8*)g_Menu->pManager)[0xB] = 0;
    }
}

void func_801E6544(u8* pEntries) {
    s32 i;
    for (i = 0; i < 0x10; i++) {
        u8* p = pEntries + i * 0x10;
        u8 b1 = p[1], b2 = p[2], b3 = p[3], b4 = p[4];
        u8 b5 = p[5], b6 = p[6], b7 = p[7], b8 = p[8];
        u8 b9 = p[9], bA = p[0xA], bB = p[0xB], bC = p[0xC];
        u8 bD = p[0xD], bE = p[0xE], bF = p[0xF];
        p[0] = p[0];
        p[1] = b1 | b2;
        p[2] = b3;
        p[3] = b4;
        p[4] = b5 | b6;
        p[5] = b7;
        p[6] = b8;
        p[7] = b9 | bA;
        p[8] = bB;
        p[9] = bC;
        p[0xA] = bD | bE;
        p[0xB] = bF;
    }
}

extern u8 D_801EA8C0;
extern u16 D_801EA5D0[];

void* func_801E65E4(u8* pChar) {
#ifdef XENO_PC_PORT
    u16 code;
    if (pChar[0] < 0x20) {
        /* Retail's branch delay slot supplies lead byte 0x81. */
        code = 0x8140;
        D_801EA8C0 = 0;
    } else if (pChar[0] < 0x80) {
        /* The table halfword already has the KROM code's byte order. */
        code = D_801EA5D0[pChar[0]];
        D_801EA8C0 = 0;
    } else {
        code = ((u16)pChar[0] << 8) | pChar[1];
        D_801EA8C0 = 1;
    }
    return (void*)PcPortKromFont(code, 32);
#else
    u8 lo = pChar[0];
    u8 hi = pChar[1];
    u16 code;
    D_801EA8C0 = 1;
    if (lo < 0x80) {
        if (lo < 0x20) {
            hi = 0x40;
            D_801EA8C0 = 0;
        } else {
            code = D_801EA5D0[lo];
            hi = (u8)(code >> 8);
            lo = (u8)code;
            D_801EA8C0 = 0;
        }
    }
    return Krom2RawAdd((lo << 8) | hi);
#endif
}

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E6668);
#else
/* This overlay address is also a field-object routine with two pointers. */
#define func_801E6668 MenuRenderCardTitle
void func_801E6668(s32 screenId) {
    u8 *pixels = HeapAlloc(0x100, 1);
    u16 *image = HeapAlloc(0x1000, 1);
    u8 *title;
    s32 consumed = 0, index = 0;
    RECT rect;
    bzero(image, 0x1000);
    /* TIM_IMAGE expands on the host: use the typed card-data member. */
    title = g_Menu->unk32C->unkB94 + 4 + screenId * 512;
    while (*title != 0) {
        const u8 *glyph = func_801E65E4(title);
        if (glyph != (void*)(intptr_t)-1) {
            s32 row, col;
            /* Retail reads 32 bytes even though the BIOS glyph stride is 30. */
            for (row = 0; row < 16; ++row) {
                for (col = 0; col < 8; ++col) {
                    pixels[row * 16 + col] = (glyph[row * 2] >> (7 - col)) & 1;
                    pixels[row * 16 + 8 + col] = (glyph[row * 2 + 1] >> (7 - col)) & 1;
                }
            }
            func_801E6544(pixels);
            for (row = 0; row < 16; ++row) {
                for (col = 0; col < 12; ++col) {
                    s32 word = (index / 16) * 1024 + (index % 16) * 4 + row * 64 + col / 4;
                    image[word] |= pixels[row * 16 + col] << ((col % 4) * 4);
                }
            }
        }
        ++title;
        ++consumed;
        if (D_801EA8C0 != 0) {
            ++title;
            ++consumed;
        }
        if (consumed >= 64) break;
        if (++index >= 32) break;
    }
    rect.x = 320; rect.y = 224; rect.w = 64; rect.h = 32;
    LoadImage(&rect, (u_long*)image);
    DrawSync(0);
    HeapFree(pixels);
    HeapFree(image);
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E68AC);
#else
extern u32 D_801E9FE0[];

void func_801E68AC(u8* entry) {
    u32 ticks;
    s32 i;
    for (i = 0; i < 2; i++) {
        func_8002675C(g_Menu->unk2DC, 0xEE,
                     MenuRawPointer(0x34C) + 0x240C + i * 0xA0,
                     g_Menu->renderContext, D_801E9FE0[i], 0x7A, 0x1000);
    }
    memcpy(&ticks, entry, sizeof(ticks));
    func_801C7F34(ticks);
    for (i = 0; i < 7; i++) {
        u32 glyph;
        memcpy(&glyph, g_Menu->unk2EC + i * 4, sizeof(glyph));
        func_8002675C(g_Menu->unk2DC, glyph,
                     MenuRawPointer(0x34C) + 0x254C + i * 0x50,
                     g_Menu->renderContext, D_801E9FE0[2 + i], 0x7A, 0x1000);
    }
    func_8002675C(g_Menu->unk2DC, 0x17, MenuRawPointer(0x34C) + 0x2C7C,
                 g_Menu->renderContext, 8, 0x66, 0x1000);
    func_8002675C(g_Menu->unk2DC, 0x32, MenuRawPointer(0x34C) + 0x2CCC,
                 g_Menu->renderContext, 0x10, 0x66, 0x1000);
    func_8002675C(g_Menu->unk2DC, ((u32)entry[0x23] + 1) / 10,
                 MenuRawPointer(0x34C) + 0x2D1C,
                 g_Menu->renderContext, 0x10, 0x6E, 0x1000);
    /* Retail re-reads the entry byte after the tens renderer returns. */
    func_8002675C(g_Menu->unk2DC, ((u32)entry[0x23] + 1) % 10,
                 MenuRawPointer(0x34C) + 0x2D6C,
                 g_Menu->renderContext, 0x18, 0x6E, 0x1000);
}
#endif

extern s32 D_801EA004[];
extern s32 D_801EA010[];

void func_801E6AE8(u8 slotIdx,
#ifdef XENO_PC_PORT
                     u8* entry) {
    /* Retail reads the character byte from the selected save entry;
     * each character's render panel occupies 0x87C bytes. */
    func_8002675C(g_Menu->unk2DC, entry[0x1C + slotIdx] + 0x14E,
                 MenuRawPointer(0x34C) + 0xA98 + slotIdx * 0x87C,
                 g_Menu->renderContext, D_801EA004[slotIdx],
                 D_801EA010[slotIdx], 0x1000);
#else
                     s32 arg1) {
    void* pMenu = g_Menu;
    u8* pData = MenuRawPointer(0x34C);
    u8 charIdx = arg1 + slotIdx;
    s32 arg3 = *(u32*)((u8*)pMenu + 0x308);
    u8* pSlot = pData + 0xA98 + slotIdx * 0x7C;
    s32 stackArgs[3];
    stackArgs[0] = D_801EA004[slotIdx];
    stackArgs[1] = D_801EA010[slotIdx];
    stackArgs[2] = 0x1000;
    func_8002675C(*(u32*)((u8*)pMenu + 0x2DC), charIdx + 0x14E, pSlot, arg3, stackArgs[0], stackArgs[1], stackArgs[2]);
#endif
}

extern u32 D_801EA01C;
extern u32 D_801EA020;

void func_801E6B70(u8 slotIdx, u8* pTable) {
#ifdef XENO_PC_PORT
    u32 offset = slotIdx * 0x87C;
    s32 i;
    func_801C80B8(pTable[slotIdx + 0x16]);
    MenuRawPointer(0x34C)[offset + 0x1308] = 0;
    for (i = 0; i < 3; i++) {
        u8 digit = g_Menu->digits[6 + i];
        if (digit != 0xFF) {
            u8* panel = MenuRawPointer(0x34C);
            u8 count = panel[offset + 0x1308];
            s32 written = func_8002675C(
                g_Menu->unk2DC, digit,
                panel + offset + 0xA98 + 0x320 + count * 0x50,
                g_Menu->renderContext, D_801EA01C + slotIdx * 0x50 + i * 8,
                D_801EA020, 0x1000);
            /* The font callback may replace the menu. Accumulate its
             * result into the current owner's byte counter, as retail does. */
            panel = MenuRawPointer(0x34C);
            panel[offset + 0x1308] += (u8)written;
        }
    }
    func_801C80B8(pTable[slotIdx + 0x19]);
    MenuRawPointer(0x34C)[offset + 0x1309] = 0;
#else
    s32 i;
    u8* pMenu;
    u8* pData;
    u32 dataOff;
    u8* pSlotBase;
    u8 val;
    val = pTable[slotIdx + 0x16];
    func_801C80B8(val);
    pMenu = (u8*)g_Menu;
    pData = MenuRawPointer(0x34C);
    dataOff = slotIdx * 0x7C;
    pSlotBase = pData + dataOff;
    pSlotBase[0x1308] = 0;
    for (i = 0; i < 3; i++) {
        pMenu = (u8*)g_Menu;
        if (*(u8*)(pMenu + 0x322 + i) != 0xFF) {
            u8* pBuf = MenuRawPointer(0x34C);
            u8 tblVal = *(u8*)(pBuf + dataOff + 0x1308);
            u32 tableEntry = D_801EA01C + slotIdx * 0x30 + i * 8;
            u8* pRender = pBuf + dataOff + 0xA98 + tblVal * 0x320;
            func_8002675C(*(s32*)(pMenu + 0x2DC), *(u8*)(pMenu + 0x322 + i), pRender, *(s32*)(pMenu + 0x308), tableEntry, D_801EA020, 0x1000);
            pMenu = (u8*)g_Menu;
            pData = MenuRawPointer(0x34C);
            *(u8*)(pData + dataOff + 0x1308) += 1;
        }
    }
    pMenu = (u8*)g_Menu;
    val = pTable[slotIdx + 0x19];
    func_801C80B8(val);
    pMenu = (u8*)g_Menu;
    pData = MenuRawPointer(0x34C);
    *(u8*)(pData + slotIdx * 0x7C + 0x1309) = 0;
#endif
}

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E6CFC);
#else
extern u32 D_801EA02C, D_801EA030, D_801EA034, D_801EA038;

void func_801E6CFC(u8 slotIdx, u8* entry) {
    u32 offset = slotIdx * 0x87C;
    s32 row, i;
    for (row = 0; row < 2; row++) {
        u16 value;
        u32 counter = 0x130A + row;
        u32 glyphBase = row == 0 ? 0x500 : 0x5F0;
        s32 column = 0;
        memcpy(&value, entry + 4 + row * 6 + slotIdx * 2, sizeof(value));
        func_801C80B8(value);
        MenuRawPointer(0x34C)[offset + counter] = 0;
        for (i = 0; i < 3; i++) {
            u8 digit = g_Menu->digits[6 + i];
            if (digit != 0xFF) {
                u8* panel = MenuRawPointer(0x34C);
                u8 count = panel[offset + counter];
                /* First value retains digit columns; second packs only
                 * visible digits, independently of glyphs returned. */
                u32 x = (row == 0 ? D_801EA02C : D_801EA034) +
                        slotIdx * 0x50 + (row == 0 ? i : column) * 8;
                s32 written = func_8002675C(
                    g_Menu->unk2DC, digit,
                    panel + offset + 0xA98 + glyphBase + count * 0x50,
                    g_Menu->renderContext, x,
                    row == 0 ? D_801EA030 : D_801EA038, 0x1000);
                panel = MenuRawPointer(0x34C);
                panel[offset + counter] += (u8)written;
                column++;
            }
        }
    }
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E6F5C);
#else
extern u32 D_801EA03C, D_801EA040, D_801EA044, D_801EA048;

void func_801E6F5C(u8 slotIdx, u8* entry) {
    u32 offset = slotIdx * 0x87C;
    s32 row, i;
    for (row = 0; row < 2; row++) {
        u32 counter = 0x130C + row;
        u32 glyphBase = row == 0 ? 0x6E0 : 0x780;
        s32 column = 0;
        func_801C80B8(entry[0x10 + row * 3 + slotIdx]);
        MenuRawPointer(0x34C)[offset + counter] = 0;
        for (i = 0; i < 2; i++) {
            u8 digit = g_Menu->digits[7 + i];
            if (digit != 0xFF) {
                u8* panel = MenuRawPointer(0x34C);
                u8 count = panel[offset + counter];
                u32 x = (row == 0 ? D_801EA03C : D_801EA044) +
                        slotIdx * 0x50 + (row == 0 ? i : column) * 8;
                s32 written = func_8002675C(
                    g_Menu->unk2DC, digit,
                    panel + offset + 0xA98 + glyphBase + count * 0x50,
                    g_Menu->renderContext, x,
                    row == 0 ? D_801EA040 : D_801EA048, 0x1000);
                /* Reload ownership after rendering, retaining byte wrap. */
                panel = MenuRawPointer(0x34C);
                panel[offset + counter] += (u8)written;
                column++;
            }
        }
    }
}
#endif

extern u16 D_801EA590[];
extern u16 D_801EA5DC[];

#ifdef XENO_PC_PORT
extern void func_80033B34(u16*, u8*, s32);
#endif

void func_801E71B4(u8 charIdx, void* pData, u8 screenIdx) {
#ifdef XENO_PC_PORT
    u8* entry = g_Menu->unk32C->unkB94 + 0x100 + screenIdx * 512;
    u8 character = ((u8*)pData)[0x1C + charIdx];
    u16 raw[12];
    u8 decoded[24];
    u8* bytes = (u8*)raw;
    s32 i;
    void* buffer;
    RECT rect;
    for (i = 0; i < 20; i += 2) {
        bytes[i] = entry[0x24 + character * 20 + i];
        bytes[i + 1] = entry[0x25 + character * 20 + i];
        if (bytes[i] == 0 && bytes[i + 1] == 0) break;
    }
    func_80033B34(raw, decoded, i / 2);
    buffer = HeapAlloc(0x3F6, 0);
    bzero(buffer, 0x3F6);
    SystemRenderStringEntry(decoded, buffer, 0x24, 0);
    /* Both atlas coordinates are halfwords at four-byte spacing. */
    rect.x = (s16)(D_801EA590[charIdx * 2] + 0x180);
    rect.y = (s16)D_801EA5DC[charIdx * 2];
    rect.w = 0x28;
    rect.h = 0x0D;
    LoadImage(&rect, buffer);
    DrawSync(0);
    HeapFree(buffer);
#else
    u8* pSrc = (u8*)pData + charIdx;
    void* pMenu = g_Menu;
    void* pMgrData = *(void**)((u8*)pMenu + 0x32C);
    u8* pEntry = (u8*)pMgrData + (screenIdx << 9) + 0xC94;
    u8 buf[0x18];
    u8 outBuf[0x18];
    s32 i = 0;
    u8* pDst = buf;
    u8* pDst2 = buf + 1;
    RECT rect;
    void* pRender;
    void* pAlloc;

    while (i < 0x14) {
        u8 charVal = pSrc[0x1C];
        u8 val0 = pEntry[charVal * 0x14 + i * 2 + 0x24];
        u8 val1 = pEntry[charVal * 0x14 + i * 2 + 0x25];
        *pDst = val0;
        *pDst2 = val1;
        if (val0 == 0 && val1 == 0) break;
        pDst += 2;
        pDst2 += 2;
        i += 2;
    }

    func_80033B34(buf, outBuf, i >> 1);
    pAlloc = HeapAlloc(0x3F6, 0);
    bzero(pAlloc, 0x3F6);
    SystemRenderStringEntry(outBuf, pAlloc, 0x24, 0);
    rect.x = D_801EA590[charIdx] + 0x180;
    rect.y = D_801EA5DC[charIdx];
    rect.w = 0x28;
    rect.h = 0xD;
    LoadImage(&rect, pAlloc);
    DrawSync(0);
    HeapFree(pAlloc);
#endif
}

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E733C);
#else
extern u16 D_801EA04C, D_801EA050;
extern void func_801E927C(POLY_FT4*);

void func_801E733C(void) {
    s32 i;
    for (i = 0; i < 16; i++) {
        POLY_FT4* p = (POLY_FT4*)(MenuRawPointer(0x34C) + 0x277C) +
                      i * 2 + g_Menu->renderContext;
        u16 x, y, page, clut;
        func_801E927C(p);
        p = (POLY_FT4*)(MenuRawPointer(0x34C) + 0x277C) +
            i * 2 + g_Menu->renderContext;
        x = (u16)(D_801EA04C + i * 12);
        y = D_801EA050;
        p->x0 = p->x2 = (s16)x;
        p->x1 = p->x3 = (s16)(x + 12);
        p->y0 = p->y1 = (s16)y;
        p->y2 = p->y3 = (s16)(y + 16);
        p->u0 = p->u2 = (u8)(i * 16);
        p->u1 = p->u3 = (u8)(i * 16 + 12);
        p->v0 = p->v1 = 0xF0;
        p->v2 = p->v3 = 0xFF;
        page = GetTPage(0, 0, 320, 128);
        /* Retail reloads the destination after each external call. */
        p = (POLY_FT4*)(MenuRawPointer(0x34C) + 0x277C) +
            i * 2 + g_Menu->renderContext;
        p->tpage = page;
        clut = GetClut(0, 448);
        p = (POLY_FT4*)(MenuRawPointer(0x34C) + 0x277C) +
            i * 2 + g_Menu->renderContext;
        p->clut = clut;
    }
}
#endif

#ifdef XENO_PC_PORT
extern void func_801E61B0(void);
extern void func_801E733C(void);
#endif

void func_801E76EC(s32 screenIdx) {
#ifdef XENO_PC_PORT
    /* Retail saves card+0xC94+screen*512 before the rendering initializer.
     * Its return value is not a pointer; callbacks may replace g_Menu. */
    u8* pEntry = g_Menu->unk32C->unkB94 + 0x100 + (u32)screenIdx * 512u;
    func_801E61B0();
#else
    void* pMenu = g_Menu;
    void* pData = *(void**)((u8*)pMenu + 0x32C);
    void* pTable = func_801E61B0((u8*)pData + (screenIdx << 9));
    u8* pEntry = (u8*)pTable + 0xC94;
#endif
    s32 i;
    u32 slotOff = 0;
    for (i = 0; i < 3; i++) {
        if (*(u8*)(pEntry + i + 0x1C) != 0xFF) {
            u8 idx = (u8)i;
            *(u8*)(MenuRawPointer(0x34C) + slotOff + 0x1310) = 1;
            func_801E6AE8(idx, pEntry);
            func_801E6B70(idx, pEntry);
            func_801E6CFC(idx, pEntry);
            func_801E6F5C(idx, pEntry);
            func_801E71B4(idx, pEntry, screenIdx);
        } else {
            *(u8*)(MenuRawPointer(0x34C) + slotOff + 0x1310) = 0;
        }
#ifdef XENO_PC_PORT
        *(u8*)(MenuRawPointer(0x34C) + slotOff + 0x130F) = (u8)g_Menu->renderContext;
#else
        *(u8*)(MenuRawPointer(0x34C) + slotOff + 0x130F) = *(u8*)((u8*)g_Menu + 0x308);
#endif
        slotOff += 0x87C;
    }
    func_801E68AC(pEntry);
#ifdef XENO_PC_PORT
    func_801E733C();
#else
    func_801E733C(pEntry);
#endif
}

extern void func_801E76EC(s32);
extern void func_801E6668(s32);

void func_801E781C(s32 screenId, u8 animFlag) {
    void* pMenu;
    func_801E64E0();
    if (screenId == 0xFF) return;
    if ((animFlag & 0xFF) != 0) {
        func_801E76EC(screenId);
        func_801E6668(screenId);
        pMenu = g_Menu;
        *(u8*)(MenuRawPointer(0x34C) + 0x2DBC) = 1;
    } else {
        func_801E6668(screenId);
        pMenu = g_Menu;
        *(u8*)(MenuRawPointer(0x34C) + 0x2DBC) = 0;
    }
    pMenu = g_Menu;
    ((u8*)g_Menu->pManager)[0xB] = 1;
}

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E78C8);
#endif

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

void func_801E8018(s32 count, u8* strings, s32 descriptorIds, void* unused) {
    func_801E7E68((MenuString*)strings, (u8*)descriptorIds, 4, count & 0xFF);
}
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

void func_801E8044(s32 count, void* pFlags) {
    u8* end;

    count &= 0xFF;
    if (count != 0) {
        end = (u8*)(count + (s32)pFlags);
        do {
            *(u8*)pFlags = 0;
            pFlags = (u8*)pFlags + 1;
        } while ((s32)pFlags < (s32)end);
    }
}

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E8070);
#else
extern u8 D_801E9E64[];   /* per-option string x-offset table (u16 at *4) */
extern u8 D_801E9EC4[];   /* Items description label x positions */
extern u8 D_801E9EE4[];   /* Items description label y position */
extern u8 D_801E9EE8[];   /* Abilities row x positions (E8070 mode 2/5) */
extern u8 D_801E9F28[];   /* Abilities row y positions, indexed by arg6 */
extern u8 D_801E9F30[];   /* Equip category label y positions (mode 3) */

/* Nav N1: position the selected option's MenuString quad (the rendered text
 * strip) at the cursor's table position, mark it visible.  Retail is a 7-case
 * jump table.  Mode 0 serves the main menu and mode 1 serves the Items
 * description labels; mode 2 is Abilities rows; mode 3 is Equip category
 * labels into the packed retail MenuString buffer at unk14E0. */
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

    /* Equip category labels: retail packs MenuString at 0x80 into unk14E0.
     * Mode 3 sets x=0x18 and y from D_801E9F30[arg6], then the shared
     * C851C + flag tail (.L801E82C4 -> .L801E8418). */
    if ((mode & 0xFF) == 3) {
        u8* line = (u8*)pStrings + sel * 0x80;
        x = 0x18;
        y = *(u16*)(D_801E9F30 + (arg6 & 0xFF) * 4);
        func_801C851C((SVECTOR*)(line + 0x50), x, y, line[0x7E], 0xD);
        line[0x7D] = (u8)rc;
        ((u8*)pFlags)[sel] = 1;
        return;
    }

    if ((mode & 0xFF) != 0) {
        static int warned;
        if (!warned) {
            warned = 1;
            printf("[xeno-port][stub-path] func_801E8070 mode %d not ported "
                   "(ported: 0, 1, 2, 3)\n", mode & 0xFF);
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

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E86C8);
#endif

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

#ifndef XENO_PC_PORT
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E8B4C);
#endif

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
void func_801E91C4(POLY_FT4* p) {
    SetSemiTrans(p, 1);
    SetShadeTex(p, 0);
    p->r0 = 0x80;
    p->g0 = 0x80;
    p->b0 = 0x80;
}
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
void func_801E927C(POLY_FT4* p) {
    SetPolyFT4(p);
    SetSemiTrans(p, 0);
    SetShadeTex(p, 0);
    p->r0 = 0x80;
    p->g0 = 0x80;
    p->b0 = 0x80;
}
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

#ifdef XENO_PC_PORT
extern u32 g_ArchiveTable, g_ArchiveHeader, g_ArchiveDebugTable;
extern s32 ArchiveReadFileFromCdSector(s32, void*, s32, s32, u32);

/* Retail menu 801E93A0..801E96A0. Archive globals retain their existing
 * low-address 32-bit storage; the local sector header is a full host pointer.
 * Keep drive polling unbounded as on retail: transport/cancellation belongs
 * to the CD provider, not a fabricated success or timeout in this routine. */
s32 func_801E93A0(s32 disc) {
    u32 header[4] = {0, 0, 0, 0};
    CdlLOC location;
    s32 result;

    ArchiveCdDataSync(0);
    if (func_8002C3D8() != 0) {
        if (disc == 1) {
            func_801E9340("c:\\work\\cdrom.mdg", (void*)(uintptr_t)g_ArchiveTable, 0x8000);
            func_801E9340("c:\\work\\cdrom.fid", (void*)(uintptr_t)g_ArchiveHeader, 0x7A);
            func_801E9340("c:\\work\\cdrom.fnd", (void*)(uintptr_t)g_ArchiveDebugTable, 0x40000);
        } else {
            func_801E9340("c:\\work\\cdrom2.mdg", (void*)(uintptr_t)g_ArchiveTable, 0x8000);
            func_801E9340("c:\\work\\cdrom2.fid", (void*)(uintptr_t)g_ArchiveHeader, 0x7A);
            func_801E9340("c:\\work\\cdrom2.fnd", (void*)(uintptr_t)g_ArchiveDebugTable, 0x40000);
        }
        return 0;
    }

    CdIntToPos(0, &location);
    do {
        Vsync(3);
        CdControlB(1, NULL, D_801EA8F4);
    } while ((D_801EA8F4[0] & 0x10) == 0);
    do {
        Vsync(3);
        CdControlB(1, NULL, D_801EA8F4);
    } while ((D_801EA8F4[0] & 0x10) != 0);
    do {
        Vsync(3);
        result = CdControlB(1, NULL, D_801EA8F4);
    } while ((D_801EA8F4[0] & 2) == 0 || result == 0);

    do {
        do {
            Vsync(3);
        } while (CdControlB(0x13, NULL, D_801EA8F4) == 0);
        do {
            Vsync(3);
        } while (CdControlB(2, (u8*)&location, D_801EA8F4) == 0);
        result = CdControlB(0x15, NULL, D_801EA8F4);
        if ((D_801EA8F4[0] & 1) != 0 && (D_801EA8F4[1] & 0x40) != 0 && result == 0) {
            return 2;
        }
    } while (result == 0);

    ArchiveCdSetMode(0xA0);
    ArchiveCdDataSync(0);
    Vsync(3);
    Vsync(3);
    ArchiveReadFileFromCdSector(0x17, header, 0x10, 0, 0);
    ArchiveCdDataSync(0);
    if (header[1] != 0x4E45585F) return 2;
    if (((u8*)header)[3] != (u32)disc + 0x30u) return 3;
    ArchiveReadFileFromCdSector(0x18, (void*)(uintptr_t)g_ArchiveTable, 0x8000, 0, 0);
    ArchiveCdDataSync(0);
    ArchiveReadFileFromCdSector(0x28, (void*)(uintptr_t)g_ArchiveHeader, 0x7A, 0, 0);
    ArchiveCdDataSync(0);
    return 0;
}
#else
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc", func_801E93A0);
#endif
