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
#include "PsyX/PsyX_public.h"

#define WINDOW_TITLE  "Xenogears (PC port - Phase 0 scaffold)"
#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480

int main(int argc, char** argv) {
    (void)argc;
    (void)argv;

    printf("[xeno-port] Phase 0 scaffold\n");
    printf("[xeno-port] Bringing up PsyCross (PSX HAL: GTE/GPU/SPU/CD)...\n");

    PsyX_Initialise(WINDOW_TITLE, SCREEN_WIDTH, SCREEN_HEIGHT, 0);

    /* TODO(phase 1): hand off to the game.
     *   - initialise the PsyQ runtime state the game expects
     *   - load the extracted disc image / assets
     *   - call the decompiled entry point (e.g. the game's main loop)
     * For now we just confirm the HAL came up and tear it down. */

    printf("[xeno-port] PsyCross initialised. (No game loop yet.)\n");

    PsyX_Shutdown();
    printf("[xeno-port] Clean shutdown.\n");
    return 0;
}
