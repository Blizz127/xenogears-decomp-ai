/*
 * World-map X/Z wrap-and-cell helper 0x800980D4.
 *
 * Retail boundary: [0x800980D4, 0x800981C8), 244 bytes / 61
 * instructions. No callees.
 *
 * ABI: $a0 = position (s32 x at +0, s32 z at +8). Void.
 * Wraps each axis at ±0x00800000, writes flags at 0x8009D558,
 * and cell indices (>>23)+2 at 0x8009C838 / 0x8009C83C.
 */
#ifndef WORLD_MAP_HELPER_980D4_H
#define WORLD_MAP_HELPER_980D4_H

#include "common.h"

void wm_800980D4(u32 pos_addr);

#endif /* WORLD_MAP_HELPER_980D4_H */
