/*
 * World-map helper 0x8009980C (GTE quad renderer).
 *
 * Retail boundary: [0x8009980C, 0x80099BFC), 252 instructions / 1008 bytes.
 * Leaf function. Processes quads through GTE, handles 4 color blending
 * modes, builds display list entries in scratchpad.
 */
#ifndef WORLD_MAP_HELPER_9980C_H
#define WORLD_MAP_HELPER_9980C_H

#include "common.h"

void wm_8009980C(u32 input_addr);

#endif
