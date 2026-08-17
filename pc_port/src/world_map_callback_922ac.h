/*
 * World-map scheduler callback 0x800922AC (slot-11 cb1).
 *
 * Retail boundary: [0x800922AC, 0x800923A8), 252 bytes / 63
 * instructions in world_map.bin loaded at 0x8006FAF0.
 *
 * ABI:
 *   $a0 = slot index (observed 11 on the natural path)
 *   $v0 = 3 when +20 is 0, else 1
 *
 * Leaf: no JAL / JALR / COP2 / GPU / OT. Sub 9 arms state 1; sub 0xA
 * arms state 2. State 1 decrements +50 by 0x1000 and publishes >>12
 * to 0x8009BE0C until < 0x78 (clamp 0x78000 / 0x78, state 0). State 2
 * increments until >= 0x8C (clamp 0x8C000 / 0x8C, state 0).
 */
#ifndef WORLD_MAP_CALLBACK_922AC_H
#define WORLD_MAP_CALLBACK_922AC_H

#include "common.h"

s32 wm_800922AC(s32 slot_idx);

#endif /* WORLD_MAP_CALLBACK_922AC_H */
