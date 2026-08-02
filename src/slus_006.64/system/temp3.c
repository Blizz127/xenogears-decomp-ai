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

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp3", func_8001A5CC);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp3", func_8001A684);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp3", func_8001A6E8);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp3", func_8001AADC);

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

u8 GameCharacterGetGearID(int characterIndex) {
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
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp3", GamePartyGearsInitializeSkins);
/*
Matches on https://decomp.me/scratch/AmSUB, but not when compiled in, need to find reason for the diff

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
*/

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

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp3", func_8001B5E8);

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

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp3", func_8001B6C4);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp3", func_8001B844);

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

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp3", func_8001BB0C);

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

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp3", func_8001BBAC);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp3", func_8001BD40);
