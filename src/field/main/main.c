#include "common.h"
#include "psyq/libgpu.h"
#include "psyq/libetc.h"
#include "system/memory.h"
#include "system/controller.h"

extern int g_FrameDeltaTime;


INCLUDE_ASM("asm/field/nonmatchings/main/main", FieldInitializeControllers);
/*
void FieldInitializeControllers(void) {
    FieldSetControllerBuffers(&g_C1Buffer, &g_C2Buffer);
    FieldSetMouseSpeed(3, 4);
    func_8007ADA4(0, 0x140, 0, 0xE0); // SetMouseArea?
    FieldSetMousePosition(0, 0x50, 100);
    FieldSetMousePosition(1, 0xFA, 100);
    func_8007ADA4(0, 300, 10, 0xDC); // SetMouseArea?
}
*/

void FieldRenderSync(void) {
    DrawSync(0);
    Vsync(0);
}

INCLUDE_ASM("asm/field/nonmatchings/main/main", FieldLoadUITextures);

extern s32 g_GameSceneMapNum;

void func_800777DC(void) {
    ArchiveCdDataSync(0);
    while (func_8001B484((g_GameSceneMapNum & 0xFFF) << 1, 0) != 0) {
    }
}

void FieldUpdateDeltaTime(void) {
    g_FrameDeltaTime = Vsync(1);
}

void func_80077844(short* dst, short a, short b, short c, short d, short e, short f, short g, short h, short i) {
    dst[0] = a;
    dst[1] = b;
    dst[2] = c;
    dst[3] = d;
    dst[4] = e;
    dst[5] = f;
    dst[6] = g;
    dst[7] = h;
    dst[8] = i;
}

INCLUDE_ASM("asm/field/nonmatchings/main/main", func_80077884);

INCLUDE_ASM("asm/field/nonmatchings/main/main", func_80077AB4);

void func_80077C60(void) {
    func_80077884();
    func_80077AB4();
}

extern void* g_PartyDataBuffers[];

void FieldPartyAllocateSkinDataBuffers(void) {
    HeapChangeCurrentUser(HEAP_USER_YOSI, NULL);
    g_PartyDataBuffers[0] = HeapAlloc(0x14000, 0);
    g_PartyDataBuffers[1] = HeapAlloc(0x14000, 0);
    g_PartyDataBuffers[2] = HeapAlloc(0x14000, 0);
    HeapPinBlock(g_PartyDataBuffers[0]);
    HeapPinBlock(g_PartyDataBuffers[1]);
    HeapPinBlock(g_PartyDataBuffers[2]);
}

void FieldPartyFreeSkinDataBuffers(void) {
    HeapUnpinBlock(g_PartyDataBuffers[0]);
    HeapUnpinBlock(g_PartyDataBuffers[1]);
    HeapUnpinBlock(g_PartyDataBuffers[2]);
    HeapFree(g_PartyDataBuffers[0]);
    HeapFree(g_PartyDataBuffers[1]);
    HeapFree(g_PartyDataBuffers[2]);
}

INCLUDE_ASM("asm/field/nonmatchings/main/main", func_80077DAC);

INCLUDE_ASM("asm/field/nonmatchings/main/main", func_80077E10);

/* ---- FieldMain: field game-state driver (Phase C gateway) -------------------
 * Functional decompile (port-first; not yet byte-matched). Control flow mirrors
 * asm/field/nonmatchings/main/main/FieldMain.s 1:1 via labels/gotos; every call
 * is preserved even where the callee is still INCLUDE_ASM (a no-op stub in the
 * port). g_FieldSystemMode comes from D_80010000 (-1 in retail rodata) exactly as
 * the original does -> SYSTEM_MODE_CD_ROM, which skips the mode-0-only `break 1`
 * and the raw-0x80280000 dev read. Port-only instrumentation is guarded. */
#ifdef XENO_PC_PORT
extern int printf(const char*, ...);
#define FM_LOG(...) printf("[FieldMain] " __VA_ARGS__)
#else
#define FM_LOG(...) ((void)0)
#endif

extern int g_FieldSystemMode;
extern int g_FieldCurRenderContextIndex;
#ifndef SYSTEM_MODE_PC_HDD
#define SYSTEM_MODE_PC_HDD 0
#define SYSTEM_MODE_CD_ROM 1
#endif
extern int D_80010000;
extern void *g_pGameState;
extern u8 g_GameState[];
extern s32 g_PlayerActorIndex;
extern void *g_FieldActors;          /* FieldActor* (0x5C stride; +0x4C = pActorData) */
extern u8 g_FieldEffects[];
extern u8 g_FieldDefaultParticleBanks[];
extern s32 g_GamePartySkinsInitialized;
extern s32 g_GamePartyMemberSkins[];

/* word globals */
extern s32 D_8004F370, D_8004F2F8, D_8004F324, D_8004F320, D_8004F308, D_8004F338;
extern s32 D_8004F348, D_8004F334, D_8004F310, D_8004F31C, D_8004F354, D_8004F358;
extern s32 D_8004F378, D_8004F37C, D_8004F380;
extern s32 D_800ADBE8, D_800ADBE4, D_800ADBE0, D_800ADBDC, D_800ADBD8, D_800ADBD4, D_800ADBD0;
extern s32 D_800ADBEC, D_800ADBC4, D_800ADB90, D_800ADB68, D_800ADB64, D_800ADB70, D_800ADB18;
extern s32 D_800ADC10, D_800ADC04, D_800ADB60, D_800ADB34, D_800ADB7C;
extern s32 D_800AFC78;
/* pointer globals */
extern void *D_80059560, *D_800595AC, *D_8006251C, *D_80062524, *D_800ADB30, *D_8005A4E0, *D_80062528;
/* halfword globals */
extern u16 D_8006F94E, D_8006F954, D_8006F950, D_8006F956, D_800C3900, D_800AFE9C, D_800C3908, D_800B236C;
extern s16 D_800B2290;
/* byte globals */
extern u8 D_800594D0, D_8005954C, D_800B2358, D_800B2355, D_800ADB04, D_800B21D0, D_80059171, D_80059179;
extern s32 D_80059488;

extern int ControllerGetType();
extern int func_80078BC8();
extern int func_80077E10();
extern int func_8001B484();
extern int ArchiveDataSync(void);
extern void FieldRenderSyncAndFlush();
extern void func_80085890(), func_802811EC(), func_80078D44(), func_80077DAC(), func_8007554C();
extern void func_800A5924(), func_8001B66C(), func_80085B20(), func_800A3F4C(), func_8003A89C();
extern void func_8007FFE8(), func_800A30FC(), func_800A5C40(), func_800798BC(), func_800ABA98();
extern void func_800A7C58(), func_800799D4(), func_800ACE90(), func_80078B5C(), func_800A91F0();
extern void func_800A31E8(), func_800864F0(), func_80085988(), func_8007954C();
extern void FieldInitializeDefaultParticleBanks(), FieldInitializeControllers(), FieldLoadUITextures();
extern void GamePartySyncSkinData(), GamePartySyncStreamedData(), GraphicsDrawPauseLetters();
extern void FieldPollControllers(), SoundMuteAllSpuChannels(), SoundEnableAllSpuChannels();
extern void GameCheckAndHandleSoftReset(), FieldParticlesFreeAll(), FieldFree();
extern void FieldScriptMemoryWriteU16();

/* g_FieldActors[idx].pActorData->flags. pActorData is a 4-byte field read as u32,
 * widened via long so this compiles on both the 32-bit matching build and the
 * 64-bit port. */
#define FIELD_ACTOR_FLAGS(idx) \
    (*(s32*)(long)(*(u32*)((u8*)g_FieldActors + (idx) * 0x5C + 0x4C)))

void FieldMain(void) {
    int exitCode = 0;   /* s0 at teardown (set 0/1/2/3 on the exit paths) */
    int savedSound;     /* s0 reuse: D_80059488 save/restore in the wait loops */
    int waterInit = 0;  /* s4 */
    int s5flag = 0;     /* s5: one-time D_800AFC78 latch */

    FM_LOG("entry. D_80010000=0x%08x\n", (unsigned)D_80010000);
    if (D_80010000 == -1) {
        g_FieldSystemMode = SYSTEM_MODE_CD_ROM;
    } else {
        g_FieldSystemMode = SYSTEM_MODE_PC_HDD;
    }
    FM_LOG("g_FieldSystemMode=%d (%s)\n", g_FieldSystemMode,
           g_FieldSystemMode == SYSTEM_MODE_CD_ROM ? "CD_ROM" : "PC_HDD");
    FieldRenderSyncAndFlush();
    if (g_FieldSystemMode == 0) {
        DrawSyncCallback(FieldUpdateDeltaTime);
    }
    D_8006251C = D_80059560;
    D_80062524 = D_800595AC;
    HeapChangeCurrentUser(HEAP_USER_YOSI, NULL);

    if (g_FieldSystemMode == 0 && D_8004F370 == 0) {
        /* PC-HDD dev path (not taken in the port; raw 0x80280000 is a PSX addr). */
        ArchiveSetIndex(4, 0);
        FM_LOG("dev field read: archive id 0xAD -> 0x80280000\n");
        ArchiveReadFileToBuffer(0xAD, (s32)0x80280000, 0, 0x80);
        ArchiveCdDataSync(0);
        FieldRenderSyncAndFlush();
    }

    if (!g_GamePartySkinsInitialized) {
        g_GamePartyMemberSkins[2] = 0xFF;
        g_GamePartyMemberSkins[1] = 0xFF;
        g_GamePartyMemberSkins[0] = 0xFF;
    }
    func_80085890(0);
    FieldPartyAllocateSkinDataBuffers();
    D_800ADBE8 = -1; D_800ADBE4 = -1; D_800ADBE0 = -1; D_800ADBDC = -1; D_800ADBD8 = -1;
    D_8004F358 = 0; D_8004F354 = 0;
    D_800ADC10 = 0; D_800ADB60 = 0; D_800ADB34 = 0; D_800ADB7C = 0;
    D_800ADC04 = 2;
    if (g_FieldSystemMode == 0) {
        func_802811EC();
    }
    FM_LOG("FieldInitializeControllers\n");
    FieldInitializeControllers();

    g_pGameState = &g_GameState;
    g_GameSceneMapNum = D_8006F94E;
    *(s16*)(g_GameState + 0x1932) = D_8006F954;
    *(s16*)(g_GameState + 0x1938) = D_8006F950 >> 9;
    if (D_8004F2F8 == 0) {
        D_800594D0 = 0;
        D_8004F324 = 0xFF;
    } else {
        D_8004F324 = D_8006F956;
    }
    FM_LOG("g_pGameState=%p g_GameSceneMapNum=%d\n", g_pGameState, (int)g_GameSceneMapNum);
    if (g_FieldSystemMode == SYSTEM_MODE_CD_ROM) {
        *(s16*)((u8*)g_pGameState + 0x1980) = 1;
        FieldScriptMemoryWriteU16(0x50, 1);
    }
    FM_LOG("FieldLoadUITextures\n");
    FieldLoadUITextures();
    D_8004F320 = 0;
    GamePartySyncSkinData();
    GamePartySyncStreamedData();
    D_800ADB30 = HeapAlloc(4, 1);
    if (g_FieldSystemMode == 0) {
        /* mode-0 only: the original has a `break 1` trap here. */
        FieldInitializeDefaultParticleBanks(g_PlayerActorIndex);
        *(s16*)(g_FieldDefaultParticleBanks + 0) = 1;
        *(s16*)(g_FieldDefaultParticleBanks + 6) = 0x10;
    }
    func_80078D44();
    D_800ADB04 = 1;
    FM_LOG("entering main loop\n");

    for (;;) {  /* .L80078174 */
        /* wait for a controller if none present */
        if (ControllerGetType(0) == 0) {
            savedSound = D_80059488;
            SoundMuteAllSpuChannels();
            GraphicsDrawPauseLetters(0x88, (((g_FieldCurRenderContextIndex + 1) & 1) << 8) | 0x64);
            do {
                DrawSync(0); Vsync(2); FieldPollControllers(); GameCheckAndHandleSoftReset();
            } while (ControllerGetType(0) == 0);
            SoundEnableAllSpuChannels();
            D_80059488 = savedSound;
        }
        /* paused/menu hold */
        if ((D_800C3900 & 0x800) && !(D_800AFE9C & 0x40) && !D_800B2358) {
            savedSound = D_80059488;
            SoundMuteAllSpuChannels();
            GraphicsDrawPauseLetters(0x88, (((g_FieldCurRenderContextIndex + 1) & 1) << 8) | 0x64);
            do {
                DrawSync(0); Vsync(2); FieldPollControllers(); GameCheckAndHandleSoftReset();
            } while (D_800C3900 & 0x800);
            SoundEnableAllSpuChannels();
            D_80059488 = savedSound;
        }
        if (g_FieldSystemMode == SYSTEM_MODE_CD_ROM) {
            FieldScriptMemoryWriteU16(0x50, 1);
        }
        GameCheckAndHandleSoftReset();
        func_80077DAC();
        func_8007554C();
        func_800A5924();

        /* render-context dispatch (g_FieldCurRenderContextIndex == 1) */
        if (g_FieldCurRenderContextIndex == 1 && D_800ADBDC == 0
            && func_80078BC8() == 0 && func_80077E10() == 0) {
            if (D_8004F334 != -1) {
                HeapUnpinBlock(D_8005A4E0);
                HeapFree(D_8005A4E0);
            }
            if (s5flag == 0) {
                D_800AFC78 = (s32)D_8004F324;
                s5flag = 1;
            }
            func_8007FFE8();
            if (D_800ADBD0 == 1) {
                D_8005954C = (u8)D_800B2355;
                D_800AFC78 = (s32)D_8004F324;
                if (D_8004F338 != D_800B2290) {
                    if (D_8004F338 != -1) {
                        D_8004F348 = 1;
                    }
                    func_8001B66C();
                    D_8004F308 = -1;
                    D_8004F324 = D_800B2290;
                    func_80085B20(D_800B2290, 1);
                }
                D_800ADBD0 = 0;
                D_800ADBD4 = 1;
                goto after_ctx;
            } else {
                if (D_800ADB18 == 0) {
                    g_GamePartySkinsInitialized++;
                    func_800A3F4C();
                }
                exitCode = 0;
                if (D_800ADBD4 == 1) {
                    func_8003A89C(D_80062528, 0x7F, 0);
                }
                D_800ADBD4 = 0;
                goto teardown;
            }
        }

    after_ctx:  /* .L80078494 */
        if (D_800ADBEC == 0 && D_8004F308 == 0 && D_800ADBC4 == 0xFF && D_800ADB90 == 0
            && func_8001B484((g_GameSceneMapNum & 0xFFF) << 1, 0) == 0
            && ArchiveDataSync() == 0
            && *(s16*)(g_FieldEffects + 0xA0) == 0) {
            D_800ADB04 = 0;
            func_800A30FC();
            ArchiveCdDataSync(0);
            func_800A5C40();
            ControllerResetState();
            D_800ADB04 = 1;
        }

        /* .L80078558: render-context teardown ladder (exit codes 1/2/3) */
        if (g_FieldCurRenderContextIndex == 1) {
            if (D_800ADBE4 == 0 && func_80078BC8() == 0) {
                ArchiveCdDataSync(0);
                exitCode = 1;
                if (D_8004F334 != -1) {
                    HeapUnpinBlock(D_8005A4E0);
                    HeapFree(D_8005A4E0);
                }
                goto teardown;
            }
            if (g_FieldCurRenderContextIndex == 1) {  /* re-checked in asm */
                if (D_800ADBE8 == 0 && func_80078BC8() == 0) {
                    ArchiveCdDataSync(0);
                    if (D_8004F334 != -1) {
                        HeapUnpinBlock(D_8005A4E0);
                        HeapFree(D_8005A4E0);
                    }
                    D_8004F310++;
                    exitCode = 2;
                    func_800A3F4C();
                    goto teardown;
                }
                if (g_FieldCurRenderContextIndex == 1 && D_800ADBD8 == 0 && func_80078BC8() == 0) {
                    ArchiveCdDataSync(0);
                    if (D_8004F334 != -1) {
                        HeapUnpinBlock(D_8005A4E0);
                        HeapFree(D_8005A4E0);
                    }
                    exitCode = 3;
                    func_8001B66C();
                    goto teardown;
                }
            }
        }

        /* .L800786F4: per-frame field update/render */
        if (g_FieldSystemMode == 0) {
            u16 f3908 = D_800C3908;
            if (f3908 & 0x40) D_8004F378 = (D_8004F378 + 1) & 1;
            if (f3908 & 0x10) D_8004F37C = (D_8004F37C + 1) & 1;
            if (f3908 & 0x80) D_8004F380 = (D_8004F380 + 1) & 1;
            if ((D_800AFE9C & 0x40) && (D_800C3900 & 0x100) && D_800ADBEC == -1
                && D_8004F308 == 0 && D_800ADB34 == 0) {
                g_GameSceneMapNum = 0;
                D_800ADBEC = 0;
                FieldScriptMemoryWriteU16(2, 0);
            }
        }

        /* .L80078810 */
        if (D_800ADBD8 == -1 && D_800ADBDC == -1 && D_800ADBE4 == -1
            && func_80078BC8() == 0 && D_800ADBEC == -1) {
            u16 fe9c = D_800AFE9C;
            if ((fe9c & 3) == 0) {
                waterInit = 0;
            }
            if ((fe9c & 1) && D_800ADB68 == 1 && (fe9c & 2) && waterInit == 0) {
                func_800798BC();
                waterInit = 1;
                if (D_800ADB64 == 0xFF && (FIELD_ACTOR_FLAGS(g_PlayerActorIndex) & 0x1800) == 0
                    && D_80059179 == 0) {
                    func_800ACE90();
                }
            }
        }
        if ((D_800C3900 & 0x100) && D_800ADBEC == -1 && D_800ADB68 == 1) {
            func_800ABA98();
        }
        if (D_800ADB70 && g_FieldCurRenderContextIndex == 1) {
            func_800A7C58();
            D_800ADB70 = 0;
        }
        if (D_800ADB64 != 0xFF && g_FieldCurRenderContextIndex == 0
            && (FIELD_ACTOR_FLAGS(g_PlayerActorIndex) & 0x1800) == 0) {
            func_8007FFE8();
            func_800799D4();
            D_800ADB64 = 0xFF;
        }
        if ((D_800C3900 & 0x10) && !D_800B21D0 && D_800ADB64 == 0xFF && D_800ADB68 == 1) {
            D_800ADB64 = 0x80;
            D_80059171 = (u8)D_800B236C;
        }

        /* .L80078AAC */
        func_80078B5C();
    }

teardown:  /* .L80078ABC */
    FM_LOG("teardown exitCode=%d\n", exitCode);
    func_800798BC();
    func_800A91F0();
    func_800A31E8();
    FieldParticlesFreeAll();
    func_800864F0();
    func_8007FFE8();
    DrawSync(0);
    Vsync(0);
    FieldFree();
    FieldPartyFreeSkinDataBuffers();
    func_80085988();
    D_8004F31C = 0;
    HeapFree(D_800ADB30);
    func_8007954C(exitCode);
}
