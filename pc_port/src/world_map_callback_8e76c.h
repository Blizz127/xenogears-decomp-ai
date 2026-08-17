/*
 * World-map scheduler callback 0x8008E76C (slot-7 cb1).
 *
 * Retail boundary: [0x8008E76C, 0x800906E0), 8052 bytes / 2013
 * instructions in world_map.bin loaded at 0x8006FAF0.
 *
 * ABI: $a0 = slot index (observed 7), $v0 = CONSTANT 1.
 * Frame: 0x48, saves $s0-$s6, $ra.
 *
 * This is the world-map main gameplay handler for slot 7. It handles:
 * - Terrain height following (wm_80093978)
 * - Camera orientation (wm_80090FB4)
 * - Movement with collision (wm_80095CD4)
 * - Surface normal computation (wm_80093A5C)
 * - Sound effects (func_80039E60, func_8003A89C)
 * - Draw packet construction (wm_8008C040)
 * - Ring buffer management (wm_8008BFD4)
 * - Area-dependent table selection (wm_8008E078)
 * - Angle search (wm_8008E0F0)
 *
 * Two-level dispatch:
 *   1. Pre-dispatch on slot[+0x04] (values 1,4,5,6,7,8)
 *   2. Main dispatch on button-derived state s0 (values 1,2,3)
 */
#ifndef WORLD_MAP_CALLBACK_8E76C_H
#define WORLD_MAP_CALLBACK_8E76C_H

#include "common.h"

s32 wm_8008E76C(s32 slot_idx);

#endif /* WORLD_MAP_CALLBACK_8E76C_H */
