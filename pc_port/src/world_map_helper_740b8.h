/*
 * World-map helper 0x800740B8 (GTE_OT renderer A).
 *
 * Retail boundary: [0x800740B8, 0x80074594), 311 instructions / 1244 bytes.
 * Renders world-map overlay primitives. Sets up rotation matrix via
 * RotMatrix, computes screen coordinates with fixed-point arithmetic,
 * builds display primitives in scratchpad.
 */
#ifndef WORLD_MAP_HELPER_740B8_H
#define WORLD_MAP_HELPER_740B8_H

#include "common.h"

void wm_800740B8(void);

#endif
