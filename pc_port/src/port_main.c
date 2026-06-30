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
extern unsigned char D_80010000[];  /* build/mode flag: -1 in retail ROM        */
extern unsigned char D_80010004[];  /* archive table buffer  (g_ArchiveTable)  */
extern unsigned char D_80018004[];  /* archive header buffer (g_ArchiveHeader) */

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

            /* Field debug-entry harness (opt-in via XENO_FIELD_TEST). The KernelMenu
             * "Field" option jumps straight into the field without the new-game /
             * worldmap setup that normally (a) fills g_GameState.partyMembers and
             * (b) selects the party-skin archive directory (#4, as the skin loader
             * func_8001ACA4 does) before field entry. Without (b),
             * GamePartyCharactersInitializeSkins resolves ArchiveDecodeAlignedSize
             * against the wrong directory -> bogus ~6MB -> HeapAlloc fail. Setting
             * the real directory here lets the field's party-skin init proceed so we
             * can drive past it and find the next frontier. This is a stand-in for
             * the not-yet-ported new-game init, not a permanent solution. The
             * LoadGameStateOverlay save/restore preserves this index through the
             * field overlay load. */
            if (getenv("XENO_FIELD_TEST")) {
                extern int ArchiveSetIndex(int directoryIndex, int entryIndex);
                ArchiveSetIndex(4, 0);
                printf("[xeno-port][field-test] ArchiveSetIndex(4,0): party-skin dir\n");
            }
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
