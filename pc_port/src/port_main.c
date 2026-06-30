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

    /* 5. One-time HeapInit the asm boot (func_80019578) runs before MainLoop;
     * MainLoop only HeapRelocate()s and would crash on an uninitialised heap. */
    PcPort_HeapBoot();

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
