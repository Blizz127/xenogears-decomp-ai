/*
 * World-map scheduler callback 0x8008A72C.
 *
 * Retail boundary: [0x8008A72C, 0x8008B2BC), 2960 bytes / 740
 * instructions in world_map.bin loaded at 0x8006FAF0.
 * Slice SHA-256: 1d58efac94432cb6892462260a1d7679dfcfddc9d0568b08244b1fd7cf1b4b3f
 *
 * ABI: $a0 = scheduler slot index; every path returns 1 (s5).
 * Slot record = *(0x8009BE24) + (index << 7).  Dispatch is the 65-word
 * table at 0x80070518 on lh(slot+0x20) when that value is < 65.
 */
#ifndef WORLD_MAP_CALLBACK_8A72C_H
#define WORLD_MAP_CALLBACK_8A72C_H

#include "common.h"

s32 wm_8008A72C(s32 slot_index);

#endif /* WORLD_MAP_CALLBACK_8A72C_H */
