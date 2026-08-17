/*
 * World-map 8x8 cell-index helper 0x800981C8.
 *
 * Retail boundary: [0x800981C8, 0x800983A0), 472 bytes / 118
 * instructions. No callees.
 *
 * ABI: $a0 = position (s32 x at +0, s32 z at +8). Void.
 * Copies 0xA2 bytes 0x8009D570 → 0x8009D318, then fills an 8x8
 * halfword index grid at 0x8009D570 from wrapped cell coords.
 */
#ifndef WORLD_MAP_HELPER_981C8_H
#define WORLD_MAP_HELPER_981C8_H

#include "common.h"

void wm_800981C8(u32 pos_addr);

#endif /* WORLD_MAP_HELPER_981C8_H */
