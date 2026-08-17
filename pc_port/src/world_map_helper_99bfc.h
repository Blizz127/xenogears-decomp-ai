/*
 * World-map helper 0x80099BFC (coordinate wrapper for render entries).
 *
 * Retail boundary: [0x80099BFC, 0x80099E88), 163 instructions / 652 bytes.
 * Leaf function. Iterates entries, wraps X/Z coordinates to stay
 * within world boundaries using camera-relative thresholds.
 */
#ifndef WORLD_MAP_HELPER_99BFC_H
#define WORLD_MAP_HELPER_99BFC_H

#include "common.h"

void wm_80099BFC(u32 entries);

#endif
