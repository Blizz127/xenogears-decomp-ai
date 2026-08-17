/*
 * World-map slot-10 height-probe helper 0x80091FF8.
 *
 * Retail boundary: [0x80091FF8, 0x80092234), 572 bytes / 143
 * instructions in world_map.bin loaded at 0x8006FAF0.
 *
 * ABI:
 *   $a0 = threshold index (saved; compared to the walk index)
 *   $a1 = guest halfword table (3 headings; +2 per accepted sample)
 *   $a2 = guest halfword table (3 heights; +2 per accepted sample)
 *   $v0 = s32 walk index, or $a0 when the close-abs snap fires
 *
 * JALs: wm_80096F18 (once per outer), wm_80093354 + wm_80093660
 * (6×6 grid). No JALR / COP2 / GPU / OT.
 */
#ifndef WORLD_MAP_HELPER_91FF8_H
#define WORLD_MAP_HELPER_91FF8_H

#include "common.h"

s32 wm_80091FF8(s32 threshold, u32 headings_addr, u32 heights_addr);

#endif /* WORLD_MAP_HELPER_91FF8_H */
