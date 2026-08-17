/*
 * World-map scheduler callback 0x800907F4 (slot-8 cb1).
 *
 * Retail boundary: [0x800907F4, 0x80090A18), 548 bytes / 137
 * instructions in world_map.bin loaded at 0x8006FAF0.
 *
 * ABI:
 *   $a0 = slot index (observed 8 on the natural path)
 *   $v0 = CONSTANT 1 on every path
 *
 * Transient substate: lh +4 == 9 -> state 1; == 0xA -> state 2.
 * State 0 zeros +0x58/+0x5C. State 1 ramps them up (step 4, 5C lags
 * until 58>=0x11, clamp 0x80, both-max -> state 3). State 2 ramps
 * down (5C lags until 58<0x70, floor 0, both-zero -> state 0).
 * Common tail always adds 58 into +50, subtracts 5C from +54, then
 * RotMatrixZYX of two scratch SVECTORs into context records 2 and 3.
 */
#ifndef WORLD_MAP_CALLBACK_907F4_H
#define WORLD_MAP_CALLBACK_907F4_H

#include "common.h"

s32 wm_800907F4(s32 slot_idx);

#endif /* WORLD_MAP_CALLBACK_907F4_H */
