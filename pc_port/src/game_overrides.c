/*
 * Port-only functional implementations of boot-path functions discovered via
 * the oracle (running xeno-port and reading the "[stub] <name>" it stops at).
 *
 * These live here, rather than in the matching tree, when the original lives in
 * a raw asm segment that hasn't been carved into a C translation unit yet.
 * They are correct-by-inspection of the disassembly, NOT byte-matched. When a
 * function later gets a proper home in src/ (matched), delete it from here.
 *
 * Compiled into the port build by pc_port/build_port.sh; defining a symbol here
 * removes it from the auto-generated stub set.
 */

/* func_800363F0  (asm/slus_006.64/26644.s):
 *     lui $at, %hi(D_800501FC); sw $a0, %lo(D_800501FC)($at); jr $ra
 *   => D_800501FC = arg0; */
int D_800501FC;
void func_800363F0(int arg0) { D_800501FC = arg0; }

/* func_80019548 (asm/slus_006.64/9D24.s): PSX boot tail that restores the
 * hardware stack/global registers before returning. Native PC state is already
 * established by the C runtime, so the port equivalent is intentionally empty. */
void func_80019548(void) {}

/* ---------------------------------------------------------------------------
 * Game-state dispatch table (data migration, Silent-Hill style).
 *
 * On PSX this table is initialized data embedding absolute RAM addresses for
 * each state's memory/heap regions. We rebuild it at runtime: function pointers
 * to the real state mains, and PSX_ADDR() for the mem/heap regions so they live
 * in the emulated PSX RAM buffer. Values extracted from the matching ELF.
 * --------------------------------------------------------------------------- */
#include <stdio.h>
#include "common.h"
#include "main/main.h"
#include "field/actor.h"
#include "field/camera.h"
#include "field/effects.h"
#include "system/math.h"
#include "system/memory.h"
#include "system/sound.h"
#include "psyq/libcd.h"
#include "psyq/libgpu.h"
#include "psx_memory.h"

extern long DisableEvent(long event);
extern long EnableEvent(long event);

extern void KernelMenuMain(void);
extern void FieldMain(void);
extern void func_8001B6C4(void);
extern void MenuMain(void);

/* Controller button remap tables (system/controller.h declares these extern; the
 * initialisers are commented out there because the data lives in the game's
 * .data section -- which the port's stub generator zeroes since it isn't part of
 * the migrated blob). ControllerRemapButtonState() folds the face/shoulder bits
 * through these; with zeroed tables every face button (Circle/Cross/...) is
 * dropped, so KernelMenu navigation (d-pad, passed through directly) worked but
 * Circle = confirm did nothing. Provide the real values (digital pad: identity
 * mapping, masks = the CTRL_BTN_* face/shoulder bits). Real addrs: masks
 * @0x800501e8, mappings @0x80050238 in slus_006.64. */
u_char  g_ControllerButtonMappings[8] = { 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7 };
u_short g_ControllerButtonMasks[8]    = { 0x20, 0x40, 0x10, 0x80, 0x04, 0x01, 0x08, 0x02 };

/* State->overlay-archive-index table (real ROM data @0x8004EAA0, .sdata). The
 * stub generator zeroes it, so LoadGameStateOverlay(state) read archive offset 0
 * for every state -> ArchiveDecodeSize(0)=0 -> no read -> the overlay buffer was
 * never populated (LZSSDecompress then ran on stale heap garbage). Same class as
 * the button tables / D_80010000: initialised game data the port must supply.
 * Order: KernelMenu=0, Field=0xE, Battle=0x10, Worldmap=0xF, Battling=0xD,
 * Menu=0x11, Movie=0x12. */
int g_GameStateOverlayArchiveOffsets[NUM_GAME_STATE_OVERLAYS] = {
    0x0, 0xE, 0x10, 0xF, 0xD, 0x11, 0x12,
};

/* Main executable .sdata sentinels near 0x8004F308. These are initialized ROM
 * data, not BSS; leaving them as zeroed auto-stubs changes cold field state. */
s32 D_8004F308 = -1;
s32 D_8004F324 = 0xFF;
s32 D_8004F328 = 0xFF;
s32 D_8004F330 = -1;
s32 D_8004F334 = -1;
s32 D_8004F338 = -1;
s32 D_8004F340 = -1;
s32 g_GameSceneMapNum = -1;

/* Main executable .sdata @0x8005917C. Retail stores a pointer to D_80010000;
 * func_8001B6C4/shop setup test *D_8005917C for the boot/media sentinel. */
extern s32 D_80010000;
s32* D_8005917C = &D_80010000;

/* Main executable .sdata @0x8004F32C. This gates whether the field SEDS data is
 * already present in the streamed party-data buffer. Retail initializes it to
 * -1; a zeroed data stub makes func_80085890 memcpy from an absent stream. */
s32 D_8004F32C = -1;

/* Main executable .sdata @0x8004F33C. Current streamed audio bank id; retail
 * starts at -1 so the first field request is not mistaken for already loaded. */
s32 D_8004F33C = -1;

/* Main executable .sdata @0x8004FE50: model primitive dispatch descriptors.
 * The PSX table stores raw RAM addresses, including internal entry points such
 * as 0x8002E688. Native PC needs callable host pointers, so migrate only the
 * currently reached descriptor entry (primitive 0x0D, render variant 2). */
typedef s32 (*ModelPrimProc)(u8* pCmd, s32 count);

typedef struct ModelPrimDesc {
    ModelPrimProc proc[6];
    u32 buildProc;
    u32 cmdStride;
    u32 packetStride;
    u32 outputStride;
} ModelPrimDesc;

extern s32 func_8002E688(u8* pCmd, s32 count);

ModelPrimDesc D_8004FE50[15] = {
    [0x0D] = {
        .proc = { NULL, NULL, func_8002E688, NULL, NULL, NULL },
        .buildProc = 0x8002D0E4,
        .cmdStride = 0x08,
        .packetStride = 0x0C,
        .outputStride = 0x28,
    },
};

/* Initialized renderer bounds/depth-shift globals adjacent to D_8004FE50. */
s32 D_800500F8 = 0x13F;
s32 D_800500FC = 0x00EE0000;
s32 D_80050100 = 2;
s32 D_80050104 = 1;

extern s32 D_8004F2F4;
extern s32 D_8004F2F8;
extern s32 D_8004F2FC;
extern s32 D_8004F300;
extern s32 D_8004F304;
extern s32 D_8004F310;
extern s32 D_8004F314;
extern s32 D_8004F318;
extern s32 D_8004F31C;
extern s32 D_8004F320;
extern s32 D_8004F344;
extern s32 D_8004F348;
extern s32 D_8004F350;
extern s32 D_8004F354;
extern s32 D_8004F358;
extern s32 D_8004F35C;
extern s32 g_GameHasLoadedWDS;
extern s32 D_8004F364;
extern s32 D_8004F368;
extern s32 D_8004F36C;
extern s32 D_8004F370;
extern s32 D_8004F378;
extern s32 D_8004F37C;
extern s32 D_8004F380;
extern s16 D_8004F384;
extern u8 D_8005942C;
extern u8 D_800594D0;
extern s32 D_8005A444[3];
extern s32 D_8006F990[3];
extern s32 D_80062518;
extern s32 D_8006251C;
extern s32 D_80062524;
extern s32 g_GamePartySkinsInitialized;
extern s32 g_GamePartyMembers[3];
extern s32 g_GamePartyMemberSkins[3];
extern s32 g_PartyIsWaitingForStreamData;

/* func_8001AADC (asm/slus_006.64/system/temp3.s): original boot global-state
 * initializer called by func_80019578 before MainLoop. The native port enters
 * MainLoop directly, so keep this small reset here until temp3.c is buildable. */
void func_8001AADC(void)
{
    s32 i;

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
    g_GameSceneMapNum = -1;
    D_8004F33C = -1;
    D_8004F338 = -1;
    D_8004F330 = -1;
    D_8004F32C = -1;
    D_8004F340 = -1;
    D_8004F308 = -1;

    for (i = 0; i < 3; i++) {
        g_GamePartyMemberSkins[i] = 0;
        D_8006F990[i] = 0;
        D_8005A444[i] = 0;
        g_GamePartyMembers[i] = 0;
    }

    D_80062524 = 0;
    D_8006251C = 0;
    D_80062518 = 0;
}

/* Fixed destination the per-state overlay decompresses to (ROM: .word D_8006FAF0
 * @0x80018084 -> PSX 0x8006FAF0, the low scratch region below each state's
 * relocated heap). Zeroed stub -> NULL -> LZSSDecompress(overlay, NULL) segfaults.
 * Set to the real emulated-RAM address in PcPort_HeapBoot (needs g_PsxRam base). */
void* g_MainGameStateOverlayBuffer;

/* ClearMemory(pStart, pEnd): zero a word range. Real one is asm/BIOS (bypassed);
 * main_loop.c calls it to wipe a game state's memory region before entering it,
 * so the stub (no-op) left uninitialised state -> crash on state change. */
void ClearMemory(u32* pStart, u32* pEnd)
{
    while (pStart < pEnd)
        *pStart++ = 0;
}

MainGameState g_MainGameStates[7];

void PcPort_InitGameStates(void)
{
    /* [idx] = { pFnMain, pMemStart, pHeapStart, hasOverlay } */
    g_MainGameStates[0].pFnMain    = KernelMenuMain;            /* boot state */
    g_MainGameStates[0].pMemStart  = PSX_ADDR(0x000592b8);
    g_MainGameStates[0].pHeapStart = PSX_ADDR(0x0006faec);
    g_MainGameStates[0].hasOverlay = 0;

    g_MainGameStates[1].pFnMain    = FieldMain;
    g_MainGameStates[1].pMemStart  = PSX_ADDR(0x000af5e4);
    g_MainGameStates[1].pHeapStart = PSX_ADDR(0x000c426c);
    g_MainGameStates[1].hasOverlay = 1;

    g_MainGameStates[2].pFnMain    = func_8001B6C4;
    g_MainGameStates[2].pMemStart  = PSX_ADDR(0x000c3a6c);
    g_MainGameStates[2].pHeapStart = PSX_ADDR(0x000d39f0);
    g_MainGameStates[2].hasOverlay = 1;

    /* states 3, 4, 6 are field/battle overlay mains not yet symbol-named;
     * left NULL until the oracle reaches them. */

    g_MainGameStates[5].pFnMain    = MenuMain;
    g_MainGameStates[5].pMemStart  = PSX_ADDR(0x000592b8);
    g_MainGameStates[5].pHeapStart = PSX_ADDR(0x0006faec);
    g_MainGameStates[5].hasOverlay = 0;
}

/* ---------------------------------------------------------------------------
 * Heap bootstrap.
 *
 * On hardware the boot routine func_80019578 (asm/.../main/main) calls
 * HeapInit(func_8002DFE0(), 0x801FC000) once before falling into MainLoop, where
 * func_8002DFE0 returns &D_8006FAF0. MainLoop never re-inits the heap; it only
 * HeapRelocate()s within it, which walks g_Heap and crashes if it was never set
 * up. The oracle enters MainLoop() directly (the asm `start`/boot is not yet C),
 * so the port performs that one-time HeapInit here, translated into emulated RAM.
 * --------------------------------------------------------------------------- */
void PcPort_HeapBoot(void)
{
    HeapInit(PSX_ADDR(0x8006FAF0), PSX_ADDR(0x801FC000));
    /* Overlay decompress target (see the extern def above): 0x8006FAF0 in
     * emulated RAM, resolvable only now that g_PsxRam exists. */
    g_MainGameStateOverlayBuffer = PSX_ADDR(0x8006FAF0);
}

int SoundFileComputeChecksum(SoundFile* pSoundFile)
{
    int nResult = 0;
    int* pCurrent = (int*)pSoundFile;
    unsigned int nCount = (pSoundFile->unk8 + 3) / 4;

    do {
        nResult += *pCurrent++;
    } while (--nCount);

    return nResult;
}

int SoundValidateFile(SoundFile* pSoundFile, u32 magicBytes, unsigned short targetValue)
{
    unsigned char bIsError;

    if (pSoundFile->magic != magicBytes) {
        return SOUND_ERR_INVALID_SIGNATURE;
    }

    if (SoundFileComputeChecksum(pSoundFile) == 0) {
        bIsError = (pSoundFile->unkC != targetValue);
        return bIsError * SOUND_ERR_UNK_0X4;
    }

    return SOUND_ERR_INVALID_CHECKSUM;
}

void SoundAddSedsEntry(SoundFile* pSoundFile)
{
    SoundFile* pEntry;
    short nSedsStatus;
    SoundFile** pList;

    if (!(g_SoundControlFlags & 0x80)) {
        for (pEntry = g_SoundSedsLinkedList; pEntry != NULL; pEntry = pEntry->pNext) {
            if (pSoundFile->sedId == pEntry->sedId) {
                SoundHandleError(SOUND_ERR_ENTRY_ALREADY_EXISTS);
                return;
            }
        }
    }

    nSedsStatus = SoundValidateFile(pSoundFile, FILE_SIGNATURE('s','e','d','s'), 0x101);
    if (nSedsStatus != SOUND_STATUS_OK) {
        SoundHandleError(nSedsStatus);
        return;
    }

    DisableEvent(g_unk_SoundEvent);
    pList = &g_SoundSedsLinkedList;
    while (*pList != NULL) {
        pList = &((*pList)->pNext);
    }
    *pList = pSoundFile;
    pSoundFile->pNext = NULL;
    EnableEvent(g_unk_SoundEvent);
}

int func_8003BDFC(int flags)
{
    if (flags & 0x10) {
        while (g_SoundControlFlags & 0x10) {
        }
    }

    if (g_SoundControlFlags & 0x10) {
        return g_SoundTransferQueue[g_SoundTransferQueueReadIndex].commandType;
    }
    return 0;
}

extern void* g_FieldScriptMemory;

void FieldScriptMemoryWriteU16(int index, int value)
{
    ((u16*)&g_FieldScriptMemory)[index >> 1] = value;
}

extern s32 g_GamePartySkinsInitialized;
extern s32 D_800ADBFC;
extern FieldActor* volatile g_FieldActors;
extern s32 g_PlayerActorIndex;
extern ActorData* g_FieldScriptVMCurActor;
extern s32 ArchiveSetIndex(s32 directoryIndex, s32 entryIndex);
extern s32 ArchiveDecodeAlignedSize(u32 entryIndex);
extern s32 ArchiveReadFileToBuffer(s32 index, void* pBuffer, u32 arg2, u32 flags);
extern s32 ArchiveCdDataSync(s32 mode);
extern void SpriteSetSpecialAnimFile(SpriteData* pSpriteData, void* pAnimFile);
extern void func_800A3C8C(void);
extern void FieldDistortionInitialize(s32 arg0);
extern void FieldScriptWritePartyMemberIDs(void);
extern void func_80072254(s32 actorIndex);

/* ---------------------------------------------------------------------------
 * LZSS decompressor.
 *   LZSSHeapDecompress @ 0x80032E88, LZSSDecompress @ 0x80032EB4
 *   (asm/slus_006.64/util/lzss.s -- pure asm, no C translation unit yet).
 *
 * Functional re-implementation, correct-by-inspection of the disassembly (not
 * byte-matched). Stream format: the first 4 bytes of the source are the
 * decompressed size. The remainder is a token stream of one flag byte (8 bits
 * consumed LSB-first) followed by that many tokens:
 *   bit 0 -> literal: copy the next source byte verbatim.
 *   bit 1 -> back-reference from two bytes b0,b1:
 *              offset = b0 | ((b1 & 0xF) << 8)   (12-bit window)
 *              length = (b1 >> 4) + 3
 *            copy `length` bytes from (dst - offset).
 * The original checks the output-end only at each 8-token group boundary, so a
 * group is always processed in full; this mirrors that exactly.
 * --------------------------------------------------------------------------- */
void* LZSSDecompress(void* pSrc, void* pDst)
{
    u8* src = (u8*)pSrc;
    u8* dst = (u8*)pDst;
    u8* dstStart = dst;
    u32 nDecompressedSize = *(u32*)src;
    u8* dstEnd;

    /* XENO_PC_PORT stopgap. A per-state overlay whose archive entry the port's
     * disc/overlay path can't yet resolve (e.g. the field overlay: the archive
     * directory the overlay index lives in isn't set up, so ArchiveDecodeSize
     * returns 0) makes LoadGameStateOverlay return an UNWRITTEN buffer -- so this
     * size header is heap garbage (megabytes) and decompressing it walks straight
     * off emulated RAM. The overlay is redundant in the port anyway (field/menu
     * code is statically linked), so until the overlay archive read is implemented,
     * treat an implausible size (larger than all of emulated RAM) as an empty /
     * no-op overlay rather than crashing. Real streams (splash ~4KB, overlays
     * <=~345KB) are far under this bound. */
    if (nDecompressedSize > (u32)PSX_RAM_SIZE) {
        fprintf(stderr, "[xeno-port] LZSSDecompress: implausible size 0x%x "
                        "(overlay not resolved?) -> skipping\n", nDecompressedSize);
        return pDst;
    }

    dstEnd = dst + nDecompressedSize;
    src += 4;

    while (dst != dstEnd) {
        u8 flags = *src++;
        int n;
        for (n = 0; n < 8; n++, flags >>= 1) {
            if (flags & 1) {
                u32 b0 = *src++;
                u32 b1 = *src++;
                u32 offset = b0 | ((b1 & 0xF) << 8);
                u32 length = (b1 >> 4) + 3;
                u8* ref = dst - offset;
                u32 i;
                for (i = 0; i < length; i++) {
                    *dst++ = *ref++;
                }
            } else {
                *dst++ = *src++;
            }
        }
    }
    return dstStart;
}

void* LZSSHeapDecompress(void* pCompressed, int flags)
{
    u32 size = *(u32*)pCompressed;
    void* pDst = HeapAlloc(size, (u_int)flags);
    if (pDst == NULL) {
        return NULL;
    }
    return LZSSDecompress(pCompressed, pDst);
}

/* ---------------------------------------------------------------------------
 * func_80036718 (asm/slus_006.64/26644.s:655) -- the font's printf-style text
 * formatter that FontPrintf delegates to. The original is a 422-line custom
 * printf that, per output character, calls func_800366F0 -> (*D_80050594) =
 * FontAddLetterPrimitive (the glyph queuer, already decompiled in font.c).
 *
 * Functional re-implementation (not byte-matched): format with vsprintf, then
 * queue each character's glyph directly via FontAddLetterPrimitive. FontPrintf
 * does va_start then passes the va_list as the 3rd argument, so it arrives here
 * as `args` (x86-64 passes a va_list by reference, matching the variadic decl).
 * --------------------------------------------------------------------------- */
#include <stdarg.h>
#include <stdio.h>
extern void FontAddLetterPrimitive(int letter);

void func_80036718(int mode, char* format, va_list args)
{
    char buf[512];
    char* p;
    (void)mode;
    vsprintf(buf, format, args);
    for (p = buf; *p != '\0'; p++) {
        FontAddLetterPrimitive((unsigned char)*p);
    }
}

/* ---------------------------------------------------------------------------
 * func_800317E0 / func_80031804 (asm/slus_006.64/.../temp2 -- byte-identical):
 * a fast addPrim that threads a primitive onto the head of an ordering table
 * with tag length 3:  old = *ot;  *ot = addr(prim) & 0xFFFFFF;  *prim = old | (3<<24);
 * FontDrawLetters' per-glyph loop calls this to link each queued letter's SPRT
 * into the OT that DrawOTag walks. PSX OT links are 24-bit addresses; the
 * emulated RAM is linked below 16 MiB (-no-pie) so the mask is lossless.
 * --------------------------------------------------------------------------- */
void func_800317E0(void* ot, void* prim)
{
    u32 old = *(u32*)ot;
    *(u32*)ot = (u32)(uintptr_t)prim & 0xFFFFFF;
    *(u32*)prim = old | 0x03000000;
}

void func_80031804(void* ot, void* prim)
{
    u32 old = *(u32*)ot;
    *(u32*)ot = (u32)(uintptr_t)prim & 0xFFFFFF;
    *(u32*)prim = old | 0x03000000;
}
