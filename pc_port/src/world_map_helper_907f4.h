/*
 * World-map helper 0x800907F4 (rotation animation with RotMatrixZYX).
 *
 * Retail boundary: [0x800907F4, 0x80090A18), 137 instructions / 548 bytes.
 * Three-state animation: state 0 clears, state 1 increments rotation
 * counters (slot[+0x58], slot[+0x5C]) by 4/frame up to 0x80, state 2
 * decrements back to 0.  Applies rotation via two RotMatrixZYX calls.
 */
#ifndef WORLD_MAP_HELPER_907F4_H
#define WORLD_MAP_HELPER_907F4_H

#include "common.h"

s32 wm_800907F4(s32 slot_idx);

#endif
