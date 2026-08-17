/*
 * World-map update helper 0x80090C68.
 *
 * Retail boundary: [0x80090C68, 0x80090E14), 428 bytes / 107
 * instructions in world_map.bin loaded at 0x8006FAF0.
 *
 * Sibling of wm_80090A84 (JT @0x80070B54).  This copy uses JT
 * @0x80070B84 with the same 12-way phase map, then a shortened
 * flag-0x20 result path that does not inspect object+0x0E.
 *
 * ABI: $a0 = slot pointer; $v0 in {0, 1, 3}.  Calls wm_80090A18
 * on the return-0 path.  Not a scheduler callback.
 */
#ifndef WORLD_MAP_HELPER_90C68_H
#define WORLD_MAP_HELPER_90C68_H

#include "common.h"

s32 wm_80090C68(u32 slot_addr);

#endif /* WORLD_MAP_HELPER_90C68_H */
