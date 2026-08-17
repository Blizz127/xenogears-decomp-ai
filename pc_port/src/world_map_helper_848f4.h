/*
 * World-map helper 0x800848F4 (rendering structure initializer).
 *
 * Retail boundary: [0x800848F4, 0x80084DB8), 305 instructions / 1220 bytes.
 * Leaf function. Initializes rendering constants in scratchpad,
 * clears global counters, iterates over object entries loading
 * transform data into scratchpad for GTE processing.
 */
#ifndef WORLD_MAP_HELPER_848F4_H
#define WORLD_MAP_HELPER_848F4_H

#include "common.h"

void wm_800848F4(void);

#endif
