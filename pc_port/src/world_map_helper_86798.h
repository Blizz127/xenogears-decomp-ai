/*
 * World-map helper 0x80086798 (OT dispatcher — 1115 insns).
 *
 * Retail boundary: [0x80086798, 0x80087904), 1115 instructions / 4460 bytes.
 * Main world-map ordering-table dispatcher. Initializes scratchpad
 * structures via bulk copy, sets up camera data, iterates 48 object
 * entries applying rotation/scale transformations via rcos/rsin,
 * MulMatrix0, RotMatrixX/Y. Uses indirect call (jalr) for
 * initialization callback.
 */
#ifndef WORLD_MAP_HELPER_86798_H
#define WORLD_MAP_HELPER_86798_H

#include "common.h"

void wm_80086798(void);

#endif
