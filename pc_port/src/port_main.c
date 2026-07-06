/*
 * Phase 0 entry point for the Xenogears native PC port.
 *
 * Right now this only proves the integration boundary: it links the port
 * executable against PsyCross (the PSX hardware abstraction layer) and brings
 * the runtime up and down. In Phase 1 this will hand control to the game's own
 * entry point after asset/disc setup, with PsyCross standing in for the PSX
 * GPU/SPU/GTE/CD hardware.
 */

#include <stdio.h>
#include <stdlib.h>

#include "xeno_pc.h"
#include "psx_memory.h"
#include "PsyX/PsyX_public.h"

/* Forward-declared to avoid pulling the full PsyQ headers (libgpu needs libgte
 * first, etc.). Signatures match PsyCross. */
extern int ResetCallback(void);
extern int ResetGraph(int mode);

#define WINDOW_TITLE  "Xenogears (PC port)"
#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480

/* Decompiled game entry (src/slus_006.64/main/main_loop.c). */
extern void MainLoop(int errorCode);
/* Port-side runtime build of the game-state dispatch table (game_overrides.c). */
extern void PcPort_InitGameStates(void);
/* Port-side one-time HeapInit the asm boot would have done (game_overrides.c). */
extern void PcPort_HeapBoot(void);
/* Original boot global-state initializer called by func_80019578 before MainLoop. */
extern void func_8001AADC(void);
/* "Published by Square" splash, decompressed + drawn from the migrated EXE data. */
extern void GameShowSplashScreen(void);

/* Input wiring. The game reads its BIOS controller buffer g_C1Buffer directly
 * (system/controller.c: ControllerGetButtonState reads [status,type,btn,btn] at
 * stride 0x22). On PSX the BIOS auto-fills it each vblank after InitPAD/StartPAD;
 * those are asm (bypassed boot) and PsyCross's InitPAD/PadRead are unimplemented.
 * PsyCross's PADRAW layout (status,id,buttons[2],analog[4]) matches the game's
 * buffer byte-for-byte, so we register g_C1Buffer's two pad slots with PsyX_Pad
 * and enable pad comms here. The per-frame refresh (PsyX_UpdateInput +
 * ControllerPoll) is driven from the Vsync shim in psyq_compat.c. */
extern unsigned char g_C1Buffer[];
extern void PsyX_Pad_InitPad(int slot, unsigned char* padData);
extern int g_padCommEnable;
#define PORT_CONTROLLER_BUFFER_SIZE 0x22

/* Disc / archive bring-up. The asm boot (func_80019578) calls
 * ArchiveInit(&D_80010004 [table buf], &D_80018004 [header buf], 0 [CD path])
 * after HeapInit; it CdInit()s and reads the archive index off the disc (sectors
 * 0x18/0x28). The port replaces the async CD path with synchronous PsyCross-libcd
 * reads (archive_port.c), so we just point PsyCross at the disc image and make the
 * same call. ArchiveReadFileToBuffer (used by the menu/field overlay loads) then
 * pulls real file data straight from disc1.bin. Gated on the image being present
 * so a disc-less run still boots to the menu as before. */
extern void PsyX_CDFS_Init(const char* imageFileName, int track, int sectorSize);
extern void ArchiveInit(unsigned int pArchiveTable, unsigned int pHeaderTable,
                        unsigned int pDebugTable);
extern int ArchiveSetIndex(int directoryIndex, int entryIndex);
extern int ArchiveDecodeSize(int fileIndex);
extern void ArchiveReadFileToBuffer(int fileIndex, void* pBuffer, int arg2, int arg3);
extern int ArchiveCdDataSync(int mode);
extern void* HeapAlloc(int size, int flags);
extern void HeapFree(void* pMemory);
extern void HeapSetCurrentContentType(int contentType);
extern void* LZSSHeapDecompress(void* pCompressed, int flags);
extern void SystemInitializeFont(void* pSystemFont);
extern void SystemInitializeData(void* pSystemData);
extern void func_8001ACA4(void);
extern unsigned short D_8006F954;    /* field entrance/spawn index (sister of D_8006F94E) */
extern unsigned char D_80010000[];  /* build/mode flag: -1 in retail ROM        */
extern unsigned char D_80010004[];  /* archive table buffer  (g_ArchiveTable)  */
extern unsigned char D_80018004[];  /* archive header buffer (g_ArchiveHeader) */
extern unsigned short D_8006F94E;    /* field map selected by FieldMain         */

/* MODE2/2352 image; PsyCross extracts the 2048-byte data payload per sector. */
#define PORT_CD_SECTOR_SIZE 2352

/* Return the first readable disc-image path, or NULL. XENO_DISC overrides; the
 * defaults assume the binary is run from pc_port/build_native (repo disc/ dir). */
static const char* PcPort_FindDiscImage(void) {
    static const char* defaults[] = {
        "../../disc/disc1.bin", "disc/disc1.bin", "../disc/disc1.bin",
    };
    const char* env = getenv("XENO_DISC");
    unsigned i;
    FILE* f;
    if (env && *env) {
        f = fopen(env, "rb");
        if (f) { fclose(f); return env; }
    }
    for (i = 0; i < sizeof(defaults) / sizeof(defaults[0]); i++) {
        f = fopen(defaults[i], "rb");
        if (f) { fclose(f); return defaults[i]; }
    }
    return NULL;
}

static void PcPort_LoadSystemTextData(void) {
    void* pCompressed;
    void* pDecoded;

    ArchiveSetIndex(0, 1);

    pCompressed = HeapAlloc(ArchiveDecodeSize(6), 0);
    ArchiveReadFileToBuffer(6, pCompressed, 0, 0);
    ArchiveCdDataSync(0);
    HeapSetCurrentContentType(0x30);
    pDecoded = LZSSHeapDecompress(pCompressed, 1);
    SystemInitializeFont(pDecoded);
    HeapFree(pCompressed);

    pCompressed = HeapAlloc(ArchiveDecodeSize(7), 0);
    ArchiveReadFileToBuffer(7, pCompressed, 0, 0);
    ArchiveCdDataSync(0);
    HeapSetCurrentContentType(0x31);
    pDecoded = LZSSHeapDecompress(pCompressed, 1);
    SystemInitializeData(pDecoded);
    HeapFree(pCompressed);
}

int main(int argc, char** argv) {
    (void)argc;
    (void)argv;

    printf("[xeno-port] booting (Silent-Hill-style: PSX RAM emu + runtime dispatch table)\n");

    /* 1. PSX main-RAM emulation must come first (PSX_ADDR targets live here). */
    PsxMemory_Init();

    /* 2. Data migration: build the game-state dispatch table at runtime. */
    PcPort_InitGameStates();

    /* 3. Bring up PsyCross (SDL2 window + OpenGL context). */
    PsyX_Initialise(WINDOW_TITLE, SCREEN_WIDTH, SCREEN_HEIGHT, 0);

    /* 4. PsyQ subsystem init normally done by the asm `start` before MainLoop. */
    ResetCallback();
    ResetGraph(0);

    /* 4b. Wire controller input into the game's BIOS pad buffer (see note above). */
    PsyX_Pad_InitPad(0, &g_C1Buffer[0]);
    PsyX_Pad_InitPad(1, &g_C1Buffer[PORT_CONTROLLER_BUFFER_SIZE]);
    g_padCommEnable = 1;

    /* 5. One-time HeapInit the asm boot (func_80019578) runs before MainLoop;
     * MainLoop only HeapRelocate()s and would crash on an uninitialised heap. */
    PcPort_HeapBoot();

    /* 5a. Original boot state reset normally performed by func_80019578 before
     * entering MainLoop. The native oracle bypasses that raw asm entry point. */
    func_8001AADC();

    /* 5b. Disc / archive init (see notes above). Only when the image is found,
     * so a disc-less run still reaches the menu instead of hanging in
     * ArchiveInit's `while (CdInit() == 0)`. */
    {
        const char* disc = PcPort_FindDiscImage();
        /* D_80010000 is static read-only ROM data (asm/.../data/800.rodata.s) with
         * value 0xFFFFFFFF; nothing ever writes it. The auto-generated stub leaves it
         * zeroed, which makes FieldMain compute g_FieldSystemMode = SYSTEM_MODE_PC_HDD
         * (0) and hit a `break 1` trap meant only for the PC-HDD dev path. Restoring
         * the real value (-1) lets the original control flow pick SYSTEM_MODE_CD_ROM
         * (1) -- not a forced mode, just the correct constant. ArchiveInit treats the
         * value as pDebugTable: -1, like 0, selects the CD path (g_ArchiveDebugTable
         * = NULL), so disc loading is unchanged. */
        *(int*)D_80010000 = -1;
        if (disc) {
            printf("[xeno-port] CD image: %s\n", disc);
            PsyX_CDFS_Init(disc, 0, PORT_CD_SECTOR_SIZE);
            /* pDebugTable MUST be 0 here, not D_80010000's -1. Retail passes -1, but
             * ArchiveInit only reads the archive table/header from CD when
             * pDebugTable == 0 (`if (!pDebugTable)`); with -1 it relies on the table
             * being statically baked into the EXE at D_80010004/D_80018004, which the
             * port's data migration does not provide. So the port reads the index
             * from disc (pDebugTable = 0); g_ArchiveDebugTable still ends up NULL. */
            ArchiveInit((unsigned int)D_80010004, (unsigned int)D_80018004, 0);
            printf("[xeno-port] ArchiveInit done (archive index loaded from disc).\n");

            /* The KernelMenu "Field" option jumps straight into the field without
             * the new-game / worldmap setup that normally (a) fills
             * g_GameState.partyMembers and (b) selects the party-skin archive
             * directory (#4, as the skin loader func_8001ACA4 does) before field
             * entry. Without (b), GamePartyCharactersInitializeSkins resolves
             * ArchiveDecodeAlignedSize against the wrong directory -> bogus ~6MB ->
             * HeapAlloc fail. Setting the real directory here lets the field's
             * party-skin init proceed. This is a stand-in for the not-yet-ported
             * new-game init, not a permanent solution. The LoadGameStateOverlay
             * save/restore preserves this index through the field overlay load.
             *
             * These initializations are needed for BOTH the XENO_FIELD_TEST
             * harness AND the normal KernelMenu path, so they run unconditionally
             * once the archive is available. */
            PcPort_LoadSystemTextData();
            func_8001ACA4();
            {
                const char* fieldMap = getenv("XENO_FIELD_MAP");
                if (fieldMap != NULL && fieldMap[0] != '\0') {
                    D_8006F94E = (unsigned short)strtoul(fieldMap, NULL, 0);
                    printf("[xeno-port][field] XENO_FIELD_MAP=%u\n",
                           (unsigned int)D_8006F94E);
                }
                /* Field entrance/spawn index. D_8006F954 is the real field
                 * entrance/spawn transition input, the sister global of the map
                 * selector D_8006F94E (set just above the same way). The value
                 * propagates: FieldMain copies D_8006F954 -> g_GameState+0x1932
                 * (main.c:408); FieldLoad copies g_GameState+0x1930 ->
                 * g_FieldScriptMemory (misc3.c:390-396); the field-load script
                 * (func_800A08B8 -> func_8009FA54) reads field-script variable 2
                 * at g_FieldScriptMemory+2 to pick a spawn-table entry. Direct
                 * XENO_FIELD_MAP entry skips the transition, leaving var 2 = 0 ->
                 * entrance 0, which on some maps is an edge spawn outside the
                 * walkmesh (camera can't frame the player). Writing D_8006F954
                 * here -- the top of that copy chain, not an intermediate buffer
                 * that gets overwritten -- stands in for the missing transition.
                 * Coordinates still come from the game's own spawn table; only
                 * the index is selected. */
                const char* fieldEntrance = getenv("XENO_FIELD_ENTRANCE");
                if (fieldEntrance != NULL && fieldEntrance[0] != '\0') {
                    char* end = NULL;
                    long entrance = strtol(fieldEntrance, &end, 0);
                    if (end != fieldEntrance && entrance >= 0 &&
                        entrance <= 0xFFFF) {
                        D_8006F954 = (unsigned short)entrance;
                        printf("[xeno-port][field] XENO_FIELD_ENTRANCE=%ld "
                               "(D_8006F954 -> field-script var 2)\n", entrance);
                    } else {
                        printf("[xeno-port][field] ignoring invalid "
                               "XENO_FIELD_ENTRANCE=%s\n", fieldEntrance);
                    }
                }
            }
            printf("[xeno-port][field] font + party-skin init done\n");
        } else {
            printf("[xeno-port] WARNING: no disc image found "
                   "(set XENO_DISC or place disc/disc1.bin); archive reads disabled.\n");
        }
    }

    /* 6. Boot splash: the asm boot shows the "Published by Square" logo before
     * handing off to the game. It is fully self-contained (LZSS-decompress the
     * migrated EXE blob, LoadImage CLUT+texture, DrawPrim a sprite with a
     * fade-in/hold/fade-out via Vsync), and bypasses the game ordering table. */
    GameShowSplashScreen();

    /* Oracle bootstrap: the real entry `start` (0x80019524) is still raw MIPS
     * asm, so we call the decompiled MainLoop() directly. It will run real game
     * code until it reaches the first not-yet-decompiled function on the live
     * path, which the stub layer logs as "[stub] <name>". That name is the next
     * thing to decompile. Expect crashes/loops until the boot chain is filled in. */
    printf("[xeno-port] entering decompiled MainLoop() (oracle)...\n");
    MainLoop(0);

    PsyX_Shutdown();
    printf("[xeno-port] Clean shutdown.\n");
    return 0;
}
