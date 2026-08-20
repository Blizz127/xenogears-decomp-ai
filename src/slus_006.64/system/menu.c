#include "common.h"
#include "psyq/libgte.h"
#include "system/controller.h"
#include "system/menu.h"
#include "system/memory.h"

void MenuInitializeGfxEnvironment(GfxEnvironment* pGfxEnv) {
    pGfxEnv->drawEnv.dtd = 1;
    pGfxEnv->dispEnv.screen.y = 10;
    pGfxEnv->dispEnv.screen.w = 256;
    pGfxEnv->drawEnv.isbg = 0;
    pGfxEnv->drawEnv.r0 = 0;
    pGfxEnv->drawEnv.g0 = 0;
    pGfxEnv->drawEnv.b0 = 0;
    pGfxEnv->dispEnv.screen.x = 0;
    pGfxEnv->dispEnv.screen.h = 216;
}

void MenuInitializeGfxEnvironments(void) {
    SetGeomOffset(160, 112);
    SetGeomScreen(0x200);
    SetDefDispEnv(&g_Menu->gfxEnvs[0].dispEnv, 0, 0xE0, 0x140, 0xE0);
    SetDefDrawEnv(&g_Menu->gfxEnvs[0].drawEnv, 0, 0, 0x140, 0xE0);
    SetDefDispEnv(&g_Menu->gfxEnvs[1].dispEnv, 0, 0, 0x140, 0xE0);
    SetDefDrawEnv(&g_Menu->gfxEnvs[1].drawEnv, 0, 0xE0, 0x140, 0xE0);
    MenuInitializeGfxEnvironment(&g_Menu->gfxEnvs[0]);
    MenuInitializeGfxEnvironment(&g_Menu->gfxEnvs[1]);
}

void func_8001BEEC(void) {
    g_Menu->translation.vz = 0x800;
    g_Menu->unk228 = 0x800;
    g_Menu->rotation.vz = 0;
    g_Menu->rotation.vy = 0;
    g_Menu->rotation.vx = 0;
    g_Menu->translation.vy = 0;
    g_Menu->translation.vx = 0;
    g_Menu->unk21C = 0;
    g_Menu->unk21A = 0;
    g_Menu->unk218 = 0;
    g_Menu->unk224 = 0;
    g_Menu->unk220 = 0;
    g_Menu->unk2E8 = 1;
    g_Menu->transitionEffectState = 0;
}

void MenuProcessControllerInput(void) {
    u_char input = MENU_INPUT_IDLE;
    if (func_80036410() != 0) {
        ControllerResetState();
        g_Menu->input = input;
        return;
    }
    while (ControllerPopState()) {
        if (g_C1ButtonStatePressedOnce & CTRL_BTN_RIGHT) {
            input = MENU_INPUT_RIGHT;
            break;
        }
        if (g_C1ButtonStatePressedOnce & CTRL_BTN_DOWN) {
            input = MENU_INPUT_DOWN;
            break;
        }
        if (g_C1ButtonStatePressedOnce & CTRL_BTN_LEFT) {
            input = MENU_INPUT_LEFT;
            break;
        }
        if (g_C1ButtonStatePressedOnce & CTRL_BTN_UP) {
            input = MENU_INPUT_UP;
            break;
        }
        if (g_C1ButtonStatePressedOnce & CTRL_BTN_CIRCLE) {
            input = MENU_INPUT_CONFIRM;
            break;
        }
        if (g_C1ButtonStatePressedOnce & CTRL_BTN_SELECT) {
            input = 12;
            g_Menu->unk1E94 = g_Menu->unk1E94 == 0;
            break;
        }
        if (g_C1ButtonStatePressedOnce & CTRL_BTN_L1) {
            if (g_Menu->unk1E95) {
                g_Menu->unk1E95 -= 1;
            }
            break;
        }
        if (g_C1ButtonStatePressedOnce & CTRL_BTN_L2) {
            g_Menu->unk1E95 += 1;
            break;
        }
    }
    g_Menu->input = input;
}

extern s32* D_8005917C;

void func_8001C074(void) {
    void* pMenu;
    MenuProcessControllerInput();
    pMenu = g_Menu;
    {
        void* pOT = *(void**)((u8*)pMenu + 0x1D4);
        if (pOT == (u8*)pMenu + 0x6C) {
            pOT = (u8*)pMenu + 0x120;
        }
        *(void**)((u8*)pMenu + 0x1D4) = pOT;
    }
    pMenu = g_Menu;
    {
        u32 page = *(u32*)((u8*)pMenu + 0x308);
        void* pOT = *(void**)((u8*)pMenu + 0x1D4);
        *(u32*)((u8*)pMenu + 0x308) = (page < 1) ? 1 : 0;
        ClearOTagR((u8*)pOT + 0x70, 0x10);
    }
    {
        void* pDebug = D_8005917C;
        if (*(s32*)pDebug != -1) {
            pMenu = g_Menu;
            if (*(u8*)((u8*)pMenu + 0x1E94) != 0) {
                HeapDebugDump(3, *(u8*)((u8*)pMenu + 0x1E95), 0xF, 0x80AC);
            }
        }
    }
    {
        void* pDebug = D_8005917C;
        if (*(s32*)pDebug != -1) {
            pMenu = g_Menu;
            FontDrawLetters((u8*)*(void**)((u8*)pMenu + 0x1D4) + 0x70);
        }
    }
    DrawSync(0);
    Vsync(0);
    pMenu = g_Menu;
    PutDrawEnv(*(void**)((u8*)pMenu + 0x1D4));
    pMenu = g_Menu;
    PutDispEnv((u8*)*(void**)((u8*)pMenu + 0x1D4) + 0x5C);
    pMenu = g_Menu;
    DrawOTag((u8*)*(void**)((u8*)pMenu + 0x1D4) + 0xAC);
}

extern char D_8001833C[];
extern char D_80018350[];
extern char D_80018364[];
extern char D_80018378[];
extern char D_80018390[];
extern void* D_8004FA9C[];
extern u8 D_80059460;
extern u8 D_80059171;
extern void* D_8005945C;
extern void* D_800658CC;
extern void* D_8006BE24;
extern void* D_8005A4AC;
extern void* D_8005A4B0;
extern GameState g_GameState;

extern void func_8001C074(void);
extern void func_801C62A8(void);
#ifdef XENO_PC_PORT
/* The retail executable names these overlay entry points by address.  In the
 * matching overlay TUs, the same functions have descriptive C names.  Route
 * the native all-in-one link to those real bodies instead of generating
 * address-named oracle stubs; the matching builds remain separate images. */
extern void MemberChangeMenuMain(void);
extern void ShopMenuMain(void);
#define func_801CB0A8 MemberChangeMenuMain
#define func_801CCD28 ShopMenuMain
#else
extern void func_801CB0A8(void);
extern void func_801CCD28(void);
#endif
extern void func_801CBDBC(void);
extern void func_801CE024(void);

void MenuExecute(void) {
    int i = 0;
    int j = 0;
    int running = 1;
    void* pBuf0;
    void* pBuf1;

    if (g_MenuDebugEnabled) {
        do {
            FontPrintf(&D_8001833C, i, D_8004FA9C[i]);

            if (i < 4) {
                if (j < 0xB) {
                    FontPrintf(&D_80018350, j);
                } else {
                    FontPrintf(&D_80018364, j - 0xB);
                }
            } else if (i == 6) {
                FontPrintf(&D_80018390, j);
            } else {
                FontPrintf(&D_80018378, j);
            }

            switch (g_Menu->input) {
                case 0:
                    running = 0;
                    break;
                case 1:
                    i++;
                    j = 0;
                    if (i >= 7) {
                        i = 0;
                    }
                    break;
                case 2:
                    i--;
                    j = 0;
                    if (i < 0) {
                        i = 6;
                    }
                    break;
                case 3:
                    if (i < 4) {
                        j++;
                        if (j >= 0x1F) {
                            j = 0;
                        }
                    } else if (i == 6) {
                        j = (j == 0);
                    } else {
                        j++;
                    }
                    break;
                case 4:
                    j--;
                    if (j < 0) {
                        if (i < 4) {
                            j = 0x1E;
                        } else if (i == 6) {
                            j = (j == 0);
                        } else {
                            j = 0xFF;
                        }
                    }
                    break;
            }

            func_8001C074();
        } while (running & 0xFF);

        D_80059460 = i;
        D_80059171 = j;
        *(u8*)((u8*)g_Menu + 0x84) = 0;
        *(u8*)((u8*)g_Menu + 0x138) = 0;
        SetDispMask(0);
        g_Menu->pGfxEnv = (GfxEnvironment*)((u8*)g_Menu + 0x120);
        SetDispMask(1);
    }

    ArchiveSetIndex(0x10, 0);

    if (g_MenuDebugEnabled) {
        g_GameState.gold = 0x3B9AC9FF;
        HeapChangeCurrentUser(0x2, NULL);
        D_8005945C = HeapAlloc(ArchiveDecodeAlignedSize(0x1), 0);
        ArchiveReadFileToBuffer(0x1, D_8005945C, 0, 0x80);
        ArchiveCdDataSync(0);

        if (D_80059460 == 5) {
            ArchiveSetIndex(0x4, 0);
            D_800658CC = HeapAlloc(0x4, 0x1);
            D_8006BE24 = HeapAlloc((u32)D_800658CC + 0x7FE24000, 0x1);
            ArchiveReadFileToBuffer(0x6B9, (void*)0x801DC000, 0, 0x80);
            ArchiveCdDataSync(0);
            ArchiveSetIndex(0x10, 0);
            D_8005A4AC = HeapAlloc(0x4000, 0);
            D_8005A4B0 = HeapAlloc(0x4000, 0);
        }

        pBuf0 = HeapAlloc(0x4, 0x1);
        pBuf1 = HeapAlloc((u32)pBuf0 + 0x7FE3B000, 0x1);
        ArchiveReadFileToBuffer(D_80059460 + 5, (void*)0x801C5000, 0, 0x80);
        ArchiveCdDataSync(0);
    }

    ArchiveSetIndex(0x10, 0);

    switch (D_80059460) {
        case 0:
            func_801C62A8();
            break;
        case 1:
            func_801CB0A8();
            break;
        case 2:
            func_801CBDBC();
            break;
        case 3:
            func_801CCD28();
            break;
        case 4:
            func_801C62A8();
            ChangeGameState(1);
            break;
        case 5:
            func_801CE024();
            break;
    }

    if (g_MenuDebugEnabled) {
        HeapFree(pBuf0);
        HeapFree(pBuf1);
        if (D_80059460 == 5) {
            HeapFree(D_800658CC);
            HeapFree(D_8006BE24);
            HeapFree(D_8005A4AC);
            HeapFree(D_8005A4B0);
        }
        g_MenuDebugEnabled = 1;
        MainLoop(0);
    }
}

// Before calling this, it's expected that the caller have loaded the correct
// menu overlay to the correct address (0x801C5000).
void MenuMain() {
    g_Menu = HeapAlloc(sizeof(SystemMenu), 0);
    bzero(g_Menu, sizeof(SystemMenu));
    g_Menu->input = 8;
    HeapChangeCurrentUser(HEAP_USER_HIG, NULL);
    g_Menu->pGfxEnv = &g_Menu->gfxEnvs[1];
    g_Menu->unk1E94 = 0;
    g_Menu->unk1E95 = 1;
    g_Menu->unk2D8 = 0;
    g_Menu->shouldDrawMenu = FALSE;
    MenuInitializeGfxEnvironments();
    if (g_MenuDebugEnabled) {
        g_Menu->gfxEnvs[0].drawEnv.isbg = 1;
        g_Menu->gfxEnvs[1].drawEnv.isbg = 1;
    }
    func_8001BEEC();
    Vsync(0);
    PutDrawEnv(&g_Menu->gfxEnvs[0].drawEnv);
    PutDrawEnv(&g_Menu->gfxEnvs[1].drawEnv);
    PutDispEnv(&g_Menu->gfxEnvs[0].dispEnv);
    PutDispEnv(&g_Menu->gfxEnvs[1].dispEnv);
    SetDispMask(1);

    // This call takes over control flow and runs the loaded menu until it returns
    MenuExecute();

    g_MenuDebugEnabled = 1;
}

__asm__(
        ".globl func_8001C76C\n\t"
        ".ent func_8001C76C\n\t"
        "func_8001C76C:\n\t"
        ".word 0x00000041, 0x00000000, 0x00000001, 0x000000cc\n\t"
        ".word 0x00000006, 0x000000fc, 0x0000000d, 0x0000001c\n\t"
        ".word 0x00000008, 0x00000003, 0x21010304, 0x210000ff\n\t"
        ".word 0x00010000, 0x00000002, 0x21010304, 0x210000ff\n\t"
        ".word 0x00010003, 0x00000000, 0x31010506, 0x3162b6c1\n\t"
        ".word 0x0062b6c1, 0x002954e4, 0x00050004, 0x00000000\n\t"
        ".word 0x31010506, 0x312954e4, 0x0062b6c1, 0x002954e4\n\t"
        ".word 0x00050000, 0x00000003, 0x31010506, 0x31445bdf\n\t"
        ".word 0x0048bde3, 0x004bb4d6, 0x00050002, 0x00000004\n\t"
        ".word 0x31010506, 0x31445bdf, 0x0048bde3, 0x00445bdf\n\t"
        ".word 0x00050001, 0x00000002, 0x31010506, 0x31439eab\n\t"
        ".word 0x000000ff, 0x002954e4, 0x00010005, 0x00000003\n\t"
        ".word 0x31010506, 0x31273aeb, 0x003cc5cc, 0x002954e4\n\t"
        ".word 0x00040002, 0x00000000, 0xfff10011, 0x00000002\n\t"
        ".word 0xfff1ffef, 0x0000fffe, 0xfff1ffef, 0x00000002\n\t"
        ".word 0xfff10011, 0x0000fffe, 0x00130000, 0x00000002\n\t"
        ".word 0x00130000, 0x0000fffe, 0xf1d70000, 0x00000774\n\t"
        ".word 0xf0010000, 0x0000006b, 0xf4d60000, 0x00000b76\n\t"
        ".word 0xf0010000, 0x0000006b, 0x04eb0a22, 0x00000b5d\n\t"
        ".word 0x06f80e67, 0x0000fffb, 0x06f80e67, 0x0000fffb\n\t"
        ".word 0x04e30a22, 0x0000f49f, 0x06f8f199, 0x0000fffb\n\t"
        ".word 0x06f8f199, 0x0000fffb, 0x06fcf19b, 0x00000024\n\t"
        ".word 0x04e3f5de, 0x0000f49f, 0x00000000, 0x0000f000\n\t"
        ".end func_8001C76C");
