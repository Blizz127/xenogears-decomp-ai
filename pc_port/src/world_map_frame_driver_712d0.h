/*
 * World-map frame driver 0x800712D0.
 *
 * Retail boundary: [0x800712D0, 0x80071A50), 643 instructions / 2572 bytes.
 * Per-frame render/update orchestrator. Manages controller input,
 * queue processing, CD sync, OT management, state-dependent rendering,
 * DrawOTag submission, and display environment setup.
 */
#ifndef WORLD_MAP_FRAME_DRIVER_712D0_H
#define WORLD_MAP_FRAME_DRIVER_712D0_H

#include "common.h"

void wm_800712D0(void);
void wm_712d0_run_second_scheduler(void);

#endif
