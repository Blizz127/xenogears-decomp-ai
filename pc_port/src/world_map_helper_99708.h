/*
 * World-map helper 0x80099708 (tile coordinate processor).
 *
 * Retail boundary: [0x80099708, 0x8009980C), 65 instructions / 260 bytes.
 * Processes 8×8 tile grid entries using sine/cosine lookups for
 * coordinate transformation, writes to scratchpad. Calls wm_8009980C
 * at end (not yet implemented — stubbed).
 */
#ifndef WORLD_MAP_HELPER_99708_H
#define WORLD_MAP_HELPER_99708_H

#include "common.h"

void wm_80099708(u32 input_data);

#endif
