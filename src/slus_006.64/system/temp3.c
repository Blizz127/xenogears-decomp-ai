#include "common.h"
#include "main/game.h"
#include "system/menu.h"
#include "system/archive.h"
#include "system/memory.h"
#include "system/sound.h"
#include "psyq/libgpu.h"
#include "psyq/libcd.h"

extern s32 D_8004F330;
extern s32 D_8004F334;
extern s32 g_GamePartySkinsInitialized;
extern s32 D_8004F320;
extern s32 g_GameSceneMapNum;
extern s32 D_8004F31C;
extern s32 D_8004F32C;
extern s32 D_8004F344;
extern void *D_8005A4A0;
extern void *D_8005A4BC;
extern StreamDataQueueEntry g_PartyStreamDataQueue[4];
extern int g_GamePartyMemberSkins[3];
extern int g_PartyIsWaitingForStreamData;
extern void* g_PartyDataBuffers[];
extern void* g_PartyStreamDataPointers[];

extern u8* D_800592DC;
extern u8* D_800592E0;
extern u8* D_800592D4;
extern u8* D_800592D8;

void func_8001A5CC(void) {
    s32 row, col;
    D_800592DC = HeapAlloc(0x3480, 1);
    D_800592E0 = HeapAlloc(0x460, 1);
    D_800592D4 = HeapAlloc(0x460, 1);
    D_800592D8 = HeapAlloc(0x460, 1);
    for (row = 0; row < 0x1C; row++) {
        for (col = 0; col < 0x28; col++) {
            D_800592D4[row * 0x28 + col] = 0;
            D_800592D8[row * 0x28 + col] = 0;
        }
    }
}

extern u8* D_800592D8;

void func_8001A684(s32 row, s32 col) {
    u8* pTable;
    if (row < 0) row = 0x1C;
    if (row >= 0x1D) row = 0;
    if (col < 0) col = 0x28;
    if (col >= 0x29) col = 0;
    pTable = D_800592D8 + (row * 5 + row) * 8 + col;
    pTable[0]++;
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp3", func_8001A6E8);

extern s32 D_8005A444[];
extern s32 D_8004F364;
extern s32 D_8004F328;
extern s32 D_8004F324;
extern s32 D_8004F2FC;
extern s32 D_8004F36C;
extern s32 D_8004F2F8;
extern s32 D_8004F31C;
extern s32 D_8004F320;
extern s32 D_8004F314;
extern s32 D_8004F310;
extern s32 D_8004F370;
extern s32 D_8004F35C;
extern s32 D_8004F358;
extern s32 D_8004F354;
extern s32 D_8004F350;
extern s32 D_8004F2F4;
extern s32 D_8004F344;
extern s32 D_8004F348;
extern s32 D_8004F304;
extern s32 D_8004F368;
extern s32 D_8004F300;
extern s32 D_8004F380;
extern s32 D_8004F37C;
extern s32 D_8004F378;
extern u8 D_8005942C;
extern u8 D_800594D0;
extern s16 D_8004F384;
extern s32 D_8004F318;
extern s32 D_8004F334;
extern s32 D_8004F33C;
extern s32 D_8004F338;
extern s32 D_8004F330;
extern s32 D_8004F32C;
extern s32 D_8004F340;
extern s32 D_8004F308;
extern s32 D_80062524[];
extern s32 D_8006F990[];
extern s32 g_GamePartySkinsInitialized;
extern s32 g_GameHasLoadedWDS;
extern s32 g_PartyIsWaitingForStreamData;

void func_8001AADC(void) {
    s32 i;
    s32* pSkins = (s32*)g_GamePartyMemberSkins;
    s32* pSlots = D_8005A444;
    s32* pD8006F990 = D_8006F990;
    s32* pMembers = (s32*)g_GamePartyMembers;

    D_8004F364 = 1;
    D_8004F328 = 0xFF;
    D_8004F324 = 0xFF;
    D_8004F2FC = 0;
    D_8004F36C = 0;
    D_8004F2F8 = 0;
    D_8004F31C = 0;
    D_8004F320 = 0;
    D_8004F314 = 0;
    D_8004F310 = 0;
    g_GamePartySkinsInitialized = 0;
    D_8004F370 = 0;
    D_8004F35C = 0;
    g_GameHasLoadedWDS = 0;
    g_PartyIsWaitingForStreamData = 0;
    D_8004F358 = 0;
    D_8004F354 = 0;
    D_8004F350 = 0;
    D_8004F2F4 = 0;
    D_8004F344 = 0;
    D_8004F348 = 0;
    D_8004F304 = 0;
    D_8004F368 = 0;
    D_8004F300 = 0;
    D_8004F380 = 0;
    D_8004F37C = 0;
    D_8004F378 = 0;
    D_8005942C = 0;
    D_800594D0 = 0;
    D_8004F384 = 0;
    D_8004F318 = 0;
    D_8004F334 = -1;
    D_8004F33C = -1;
    D_8004F338 = -1;
    D_8004F330 = -1;
    D_8004F32C = -1;
    D_8004F340 = -1;
    D_8004F308 = -1;
    D_8004F380 = -1;
    D_8004F37C = -1;
    D_8004F378 = -1;
    for (i = 0; i < 3; i++) {
        pSkins[i] = 0;
        pD8006F990[i] = 0;
        pSlots[i] = 0;
        pMembers[i] = 0;
    }
    for (i = 3; i >= 0; i--) {
        D_80062524[i] = 0;
    }
}

void GamePartySignalReinitialize(void) {
    g_GamePartySkinsInitialized = 0;
}

void func_8001ACA4(void) {
    D_8004F334 = -1;
    D_8004F330 = -1;
    HeapChangeCurrentUser(HEAP_USER_YOSI, 0);
    ArchiveSetIndex(4, 0);
    GamePartyStreamLoadSkinData(1);
}

s32 GameCharacterGetGearID(int characterIndex) {
    return g_pGameState->characters[characterIndex].gearId;
}

void GameWaitForCdData(void) {
    while (ArchiveDataSync()) {};
    ArchiveCdDataSync(0);
}

// Set GameState pointer to g_GameState, set party members accordingly 
// to it and stream load character skins for them.
void GamePartyCharactersInitializeSkins(void) {
    int i;
    int curPartyMemberIndex;
    int entryCount;
    void* pBuffer;

    g_pGameState = &g_GameState;
    curPartyMemberIndex = 0;
    for (i = 0; i < MAX_PARTY_MEMBERS; i++) {
        g_GamePartyMembers[i] = CHARACTER_ID_NONE;
        if (g_pGameState->partyMembers[i] != CHARACTER_ID_NONE) {
            g_GamePartyMembers[curPartyMemberIndex++] = g_pGameState->partyMembers[i];
        }
    }
    
    for (i = 0, entryCount = 0; i < MAX_PARTY_MEMBERS; i++) {
        if (g_GamePartyMembers[i] != CHARACTER_ID_NONE) {
            g_PartyStreamDataQueue[entryCount].archiveIndex = g_GamePartyMembers[i] + 5;
            g_GamePartyMemberSkins[i] = g_GamePartyMembers[i];
            pBuffer = HeapAlloc(ArchiveDecodeAlignedSize(g_GamePartyMembers[i] + 5), 0x0);
            g_PartyStreamDataPointers[entryCount] = pBuffer;
            g_PartyStreamDataQueue[entryCount].pData = pBuffer;
            HeapPinBlock(g_PartyStreamDataPointers[entryCount]);
            entryCount++;
        }
    }
    g_PartyStreamDataQueue[entryCount].pData = NULL;
    g_PartyStreamDataQueue[entryCount].archiveIndex = 0;
    func_80029AFC(g_PartyStreamDataQueue, 0, 0);
    D_8004F31C = 1;
}

// Set GameState pointer to g_GameState, set party members accordingly 
// to it and stream load gear skins for them.
void GamePartyGearsInitializeSkins(void) {
    int i;
    int curPartyMemberIndex;
    int entryCount;
    int gearId;
    void* pBuffer;

    g_pGameState = &g_GameState;
    curPartyMemberIndex = 0;
    for (i = 0; i < MAX_PARTY_MEMBERS; i++) {
        g_GamePartyMembers[i] = CHARACTER_ID_NONE;
        if (g_pGameState->partyMembers[i] != CHARACTER_ID_NONE) {
            g_GamePartyMembers[curPartyMemberIndex++] = g_pGameState->partyMembers[i];
        }
    }

    for (i = 0, entryCount = 0; i < MAX_PARTY_MEMBERS; i++) {
        if (g_GamePartyMembers[i] != CHARACTER_ID_NONE) {
            gearId = GameCharacterGetGearID(g_GamePartyMembers[i]);
            if (gearId == 0xFF) {
                gearId = 0x0;
            }
            gearId += 0x10;
            g_PartyStreamDataQueue[entryCount].archiveIndex = gearId + 5;
            g_GamePartyMemberSkins[i] = gearId;
            pBuffer = HeapAlloc(ArchiveDecodeAlignedSize(gearId + 5), 0x0);
            g_PartyStreamDataPointers[entryCount] = pBuffer;
            g_PartyStreamDataQueue[entryCount].pData = pBuffer;
            HeapPinBlock(g_PartyStreamDataPointers[entryCount]);
            entryCount++;
        }
    }
    g_PartyStreamDataQueue[entryCount].pData = NULL;
    g_PartyStreamDataQueue[entryCount].archiveIndex = 0;
    func_80029AFC(g_PartyStreamDataQueue, 0, 0);
    D_8004F31C = 2;
}

void GamePartySyncSkinData(void) {
    GameWaitForCdData();
    if (g_PartyIsWaitingForStreamData == 1) {
        GamePartySyncStreamedData();
        if (g_GamePartySkinsInitialized) return;
    } else if (g_GamePartySkinsInitialized) {
        GamePartyStreamLoadSkinData(0);
        return;
    }

    if (!g_GamePartySkinsInitialized) {
        if (!(g_GameSceneMapNum & 0xC000)) D_8004F320 = 0;
        else D_8004F320 = 1;
        
        g_PartyIsWaitingForStreamData = 0;
        if (D_8004F320 == 0) {
            if (D_8004F31C != 1) {
                GamePartyCharactersInitializeSkins();
                g_PartyIsWaitingForStreamData = 1;
            }
        } else if (D_8004F31C != 2) {
            GamePartyGearsInitializeSkins();
            g_PartyIsWaitingForStreamData = 1;
        }
    }
}

void GamePartyStreamLoadSkinData(int arg0) {
    int i;
    int entryIndex;

    HeapToggleErrorHandler(1);

    if (g_PartyIsWaitingForStreamData == 1) {
        GamePartySyncStreamedData();
    }
    GameWaitForCdData();
    
    for (i = 0, entryIndex = 0; i < MAX_PARTY_MEMBERS; i++) {
        if (g_GamePartyMemberSkins[i] == CHARACTER_ID_NONE) {
            continue;
        }

        g_PartyStreamDataQueue[entryIndex].archiveIndex = g_GamePartyMemberSkins[i] + 5;
        g_PartyStreamDataPointers[entryIndex] = HeapAlloc(ArchiveDecodeAlignedSize(g_GamePartyMemberSkins[i] + 5), 0x1);
        g_PartyStreamDataQueue[entryIndex].pData = g_PartyStreamDataPointers[entryIndex];
        if (g_PartyStreamDataQueue[entryIndex].pData == 0) {
            for (i = 0; i < entryIndex; i++) {
                HeapUnpinBlock(g_PartyStreamDataPointers[i]);
                HeapFree(g_PartyStreamDataPointers[i]);
            }
            
            HeapToggleErrorHandler(0);
            return;
        }

        HeapPinBlock(g_PartyStreamDataPointers[entryIndex]);
        entryIndex++;
    }
        
    if (arg0 != 0) {
        D_8005A4A0 = HeapAlloc(ArchiveDecodeAlignedSize(0xA7), 0x1);
        g_PartyStreamDataQueue[entryIndex].pData = D_8005A4A0;
        if (g_PartyStreamDataQueue[entryIndex].pData != 0) {
            HeapPinBlock(g_PartyStreamDataQueue[entryIndex].pData);
            g_PartyStreamDataQueue[entryIndex].archiveIndex = 0xA7;
            entryIndex++;
            D_8004F344 = 1;
        }

        D_8005A4BC = HeapAlloc(ArchiveDecodeAlignedSize(0xA8), 0x1);
        g_PartyStreamDataQueue[entryIndex].pData = D_8005A4BC;
        if (g_PartyStreamDataQueue[entryIndex].pData != 0) {
            HeapPinBlock(g_PartyStreamDataQueue[entryIndex].pData);
            g_PartyStreamDataQueue[entryIndex].archiveIndex = 0xA8;
            entryIndex++;
            D_8004F32C = 0;
        }
    }

    g_PartyStreamDataQueue[entryIndex].pData = 0;
    g_PartyStreamDataQueue[entryIndex].archiveIndex = 0;
    func_80029AFC(g_PartyStreamDataQueue, 0, 0); // Start streaming the queued requests
    g_PartyIsWaitingForStreamData = 1;
    HeapToggleErrorHandler(0);
}

void GamePartySyncStreamedData(void) {
    int i;
    
    if (g_PartyIsWaitingForStreamData) {
        GameWaitForCdData();
        for (i = 0; i < MAX_PARTY_MEMBERS; i++) {
            HeapUnpinBlock(g_PartyDataBuffers[i]);
            if (g_GamePartyMembers[i] != CHARACTER_ID_NONE) {
                HeapUnpinBlock(g_PartyStreamDataPointers[i]);
                LZSSDecompress(g_PartyStreamDataPointers[i], g_PartyDataBuffers[i]);
                HeapFree(g_PartyStreamDataPointers[i]);
            }
        }
        g_PartyIsWaitingForStreamData = 0;
    }
}


extern int D_8005A4C0; // File size
extern void* D_8005A4E0; // File buffer

extern s32 D_8004F330;
extern s32 D_8004F334;

int func_8001B484(int fileIndex, s32 arg1) {
    if (D_8004F334 == arg1 && D_8004F330 == fileIndex) {
        return 0;
    }

    if (ArchiveDataSync() == 0) {
        ArchiveCdDataSync(0);
        if (D_8004F334 != -1) {
            HeapUnpinBlock(D_8005A4E0);
            HeapFree(D_8005A4E0);
        }
        func_8001B53C(fileIndex);
        D_8004F334 = arg1;
        D_8004F330 = fileIndex;
    }
    
    return -1;
}

void func_8001B53C(int index) {
    D_8005A4C0 = ArchiveDecodeAlignedSize(index + 0xB8);
    D_8005A4E0 = HeapAlloc(D_8005A4C0, 1);
    HeapPinBlock(D_8005A4E0);
    ArchiveReadFileToBuffer(index + 0xB8, D_8005A4E0, 0, CdlModeSpeed);
}

extern int g_GameHasLoadedWDS;
extern SoundWDSEntry* g_GameCurLoadedWDS;

void func_8001B5A8(void) {
    if (g_GameHasLoadedWDS == 1) {
        SoundFreeWdsEntry(g_GameCurLoadedWDS);
        g_GameHasLoadedWDS = 0;
    }
}

extern s32 D_8004F35C;
extern s32 D_8004F348;
extern s32 D_8004F2FC;
extern void* D_80062528;
extern void func_80039C4C(void* handle);
extern void func_800399D4(void* handle);

void func_8001B5E8(void) {
    if (D_8004F35C != 1) return;
    func_80039C4C(D_80062528);
    if (D_8004F348 == 0) {
        func_800399D4(D_80062528);
    } else {
        D_8004F2FC = (s32)D_80062528;
    }
    D_8004F35C = 0;
    D_8004F348 = 0;
}

extern s32 D_8004F338;
extern s32 D_8004F33C;
extern s32 D_8004F36C;
extern void func_8001B5E8(void);

void func_8001B66C(void) {
    if (D_8004F36C != 0) {
        func_8001B5E8();
        func_8001B5A8();
    }

    D_8004F33C = -1;
    D_8004F338 = -1;
    D_8004F36C = 0;
}

void func_8001B6BC(void) {}

extern s32* D_8005917C;
extern u8 D_800C48EA;
extern u8 D_800D3338;
extern u8 D_8005947C;
extern u8 D_800594F8;
extern u16 D_8006F94E;
extern u16 D_8006F950;
extern u16 D_8006F952;
extern u16 D_8006F954;
extern u8 D_8005959C;
extern void func_8003747C(void*);
extern void* FontLoadFont(int sx, int sy, int w, int h, s32 f1, s32 f2, s32 f3, s32 f4, s32 f5, s32 f6);
extern void func_80070F40(void);
extern void ChangeGameState(unsigned int state);
extern void MainLoop(int errorCode);

void func_8001B6C4(void) {
    s32 state;
    D_8005959C = 1;
    ArchiveCdDataSync(0);
    ArchiveSetIndex(0xC, 0);
    if (D_8005917C[0] != -1) {
        func_8003747C(0x80200000);
        FontLoadFont(0x10, 0x10, 0x140, 0x100, 0x3E8, 0, 0x340, 0x340, 0x20, 0);
    }
    func_8001B844();
    func_80070F40();

    state = D_800C48EA;
    if (state == 1 || state == 0x40 || state == 0x21) {
        if (D_800D3338 == 0) {
            if (D_8005947C == 0) {
                u16 tmp = D_8006F94E & 0x7FF;
                if (tmp < 0x400) {
                    state = 1;
                } else {
                    state = 3;
                }
            } else {
                state = 2;
            }
        }
    } else if (state == 0x81) {
        GamePartySignalReinitialize();
        D_8006F94E = 0x1EA;
        D_8006F950 = 0;
        D_8006F952 = 0;
        D_8006F954 = 0;
        state = 1;
    }
    ChangeGameState(state);

    if (D_8005947C == 0) {
        D_800594F8 = 1;
    }
    MainLoop(0);
}

extern u8 D_800C4A7C[];
extern void func_800379D0(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f);
extern void func_8001B94C(DRAWENV* pDrawEnv);

void func_8001B844(void) {
    u8* pBase = D_800C4A7C;
    ResetGraph(1);
    func_800379D0(0x300, 0, 0, 0, 0, 0);
    func_800379D0(8, 0x10, 0x140, 0xF0, 0, 0x1000);
    func_800379D0(0, 0, 0, 0, 0, 0);
    InitGeom();
    SetGeomOffset(0xA0, 0xB4);
    SetGeomScreen(0x200);
    SetDefDispEnv((DISPENV*)pBase, 0, 0xE0, 0x140, 0xE0);
    SetDefDrawEnv((DRAWENV*)(pBase - 0x5C), 0, 0, 0x140, 0xE0);
    SetDefDispEnv((DISPENV*)(pBase + 0x4070), 0, 0xE0, 0x140, 0xE0);
    SetDefDrawEnv((DRAWENV*)(pBase + 0x4014), 0, 0xE0, 0x140, 0xE0);
    func_8001B94C((DRAWENV*)(pBase - 0x5C));
    func_8001B94C((DRAWENV*)(pBase + 0x4014));
}

void func_8001B94C(DRAWENV* pDrawEnv) {
    pDrawEnv->isbg = 1;
    pDrawEnv->dtd = 1;
    pDrawEnv->r0 = 60;
    pDrawEnv->g0 = 120;
    pDrawEnv->b0 = 120;
}

/* New Game gamestate init (asm 8001B970): load the new-game save TEMPLATE
 * from archive index 0x10, file 3 (0x2358 bytes) into g_GameState, then
 * decode the template's 31 character-name records (0x14-byte blocks over
 * the first 0x26C; save-format u16 codes -> glyph pairs via func_80033B34,
 * terminator pair 0x0F,0x00), reset the sound-volume block, and set the
 * two mode bytes.  Retail's volume reset zeroes 20 halfwords DOWNWARD from
 * g_SoundVolumeController+6 -- i.e. the controller's first 8 bytes plus
 * 0x20 bytes of UNNAMED BSS below it (0x8005A3A0-0x8005A3BF, no symbol).
 * The port zeroes the controller part; the unnamed region has no host
 * symbol and is only ever nonzero after a return-to-title flow the port
 * does not have yet (fresh BSS is already zero) -- documented divergence,
 * revisit with the title-return flow. */
extern void* g_SystemDataEntries;
extern void func_80033B34(u16* src, u8* dst, s32 count);
extern u8 D_800594CC;
extern u8 D_8005947C;

void func_8001B970(void) {
    void* buf;
    u8* block;
    s32 base;

    ArchiveSetIndex(0x10, 0);
    HeapChangeCurrentUser(2, 0);
    buf = HeapAlloc(ArchiveDecodeAlignedSize(3), 1);
    ArchiveReadFileToBuffer(3, buf, 0, 0x80);
    ArchiveCdDataSync(0);
    memmove(&g_GameState, buf, 0x2358);
    HeapFree(buf);

    block = (u8*)&g_GameState;
    for (base = 0; base < 0x26C; base += 0x14, block += 0x14) {
        u8 raw[0x18];
        u8 decoded[0x18];
        s32 j;
        s32 k;

        for (k = 0; k < 0x18; k++) {
            decoded[k] = 0; /* retail copies stack garbage past the NUL;
                               zeroed for determinism */
        }
        for (j = 0; j < 0x14; j += 2) {
            raw[j] = block[j];
            raw[j + 1] = block[j + 1];
            if (block[j] == 0xF && block[j + 1] == 0) {
                break;
            }
        }
        func_80033B34((u16*)raw, decoded, j >> 1);
        for (k = 0; k < 0x14; k++) {
            block[k] = decoded[k];
        }
    }

    *(u16*)((u8*)&g_SoundVolumeController + 0x0) = 0;
    *(u16*)((u8*)&g_SoundVolumeController + 0x2) = 0;
    *(u16*)((u8*)&g_SoundVolumeController + 0x4) = 0;
    *(u16*)((u8*)&g_SoundVolumeController + 0x6) = 0;

    D_800594CC = 6;
    D_8005947C = 0;
}

extern u8 D_8006F9DE;
extern s32 D_80059470;
extern s32 D_8005949C;
extern s32 D_80059520;
extern void func_800379D8(s32 a, s32 b, s32 c, s32 d, s32 e);

void func_8001BB0C(void) {
    func_800379D8((s32)&D_8005949C, 0, (s32)&D_80059470, (s32)&D_80059520, (s32)D_8006F9DE);
}

/* Retail boot/reset helper used by func_8007954C exit 3 when D_800B0064 bit 7
 * is set. Keep the assembly in the matching build and provide its complete,
 * bounded 23-instruction behavior to the native port. */
#ifdef XENO_PC_PORT
extern u8 D_800594F8, D_8005946C;
extern u8 D_800594D4, D_800594D5, D_800594D6;
extern s32 D_800595A0;

void func_8001BB50(void) {
    D_800594F8 = 1;
    D_8005946C = 0;
    func_8001B970();
    ArchiveCdDataSync(0);
    D_800594D4 = 0x88;
    D_800594D5 = 0x76;
    D_800594D6 = 0x54;
    D_800595A0 = 2;
}
#else
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp3", func_8001BB50);
#endif

extern u8* D_800595D0;
extern u8* D_800595A8;
extern u8* D_80059480;
extern u8* D_800594AC;
extern u8 D_8005954C;
extern s16 D_8006F9BC;
extern s16 D_8006F9C0;
extern s16 D_8006F9C4;
extern s16 D_8006F9CC;
extern u8* D_8006F9C8;
extern u32 D_8006F9D0;
extern s16 D_8006F9D4;
extern s32 D_8006F9D8;
extern u8 D_8004F388[];
extern void SoundAddSedsEntry(void* pData);
extern void func_80039DB8(s32 a0);

void func_8001BBAC(void) {
    s32 i;
    HeapChangeCurrentUser(2, NULL);
    ArchiveSetIndex(0xC, 0);
    D_80059480 = HeapAlloc(4, 1);
    D_800594AC = HeapAlloc((u32)D_80059480 + 0x7FE1C000, 1);
    D_800595D0 = HeapAlloc(ArchiveDecodeAlignedSize(2), 1);
    D_800595A8 = HeapAlloc(ArchiveDecodeAlignedSize(3), 1);
    D_8006F9BC = 2;
    D_8006F9C4 = 3;
    D_8006F9CC = 4;
    D_8006F9D0 = 0x801E4000;
    D_8006F9D4 = 0;
    D_8006F9D8 = 0;
    D_8006F9C0 = (s16)(s32)D_800595D0;
    D_8006F9C8 = D_800595D0;
    func_80029AFC((u8*)&D_8006F9BC, 0, 0x80);
    while (ArchiveDataSync() == 3) {}
    SoundAddSedsEntry(D_800595D0);
    if (D_8005954C != 4) {
        for (i = 0; i < 3; i++) {
            u8 val = D_8004F388[D_8005954C * 3 + i];
            if (val != 0xFF) {
                func_80039DB8((s32)D_800595D0[0x14] << 16 | val);
            }
        }
    }
}

u8 func_8001BD40(u8 min, u8 max) {
    u8 range;
    if (min == 0xFF) return 0xFF;
    if (max == 0) return 0;
    if (min == max) return min;
    range = max - min;
    if (range < 0xFF) {
        return (u8)(min + (rand() & 0xFF) % (range + 1));
    }
    return (u8)(rand() & 0xFF);
}
