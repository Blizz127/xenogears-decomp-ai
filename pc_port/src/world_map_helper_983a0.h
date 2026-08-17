/*
 * World-map helper 0x800983A0 (matrix-composed GTE processor).
 *
 * Retail boundary: [0x800983A0, 0x800987AC), 259 instructions / 1036 bytes.
 * Compares camera and model matrices via CompMatrix, sets GTE
 * matrices, iterates objects calling wm_800987AC for GTE processing.
 */
#ifndef WORLD_MAP_HELPER_983A0_H
#define WORLD_MAP_HELPER_983A0_H

#include "common.h"

void wm_800983A0(u32 input_addr);

#endif
