/*
 * World-map fixed-threshold X/Z wrap helper 0x80093484.
 *
 * Retail boundary: [0x80093484, 0x80093534), 176 bytes / 44
 * instructions. Sibling of wm_80093354: same period words
 * (0x8009D160 / 0x8009D2B4 << 23) but compares against the fixed
 * signed window ±0x04000000 instead of the period itself.
 *
 * ABI: $a0 = guest VECTOR address. Y is never accessed. No callees.
 */
#ifndef WORLD_MAP_HELPER_93484_H
#define WORLD_MAP_HELPER_93484_H

#include "common.h"

void wm_80093484(u32 vec_addr);

#endif /* WORLD_MAP_HELPER_93484_H */
