/*
 * World-map helper 0x8008615C (vertex setup with rotation).
 *
 * Retail boundary: [0x8008615C, 0x800863EC), 161 instructions / 644 bytes.
 * Initializes vertex coordinates in scratchpad, copies camera and
 * model matrices, applies RotMatrixZ from direction area, then
 * calls wm_80099BFC (stubbed — not yet implemented).
 */
#ifndef WORLD_MAP_HELPER_8615C_H
#define WORLD_MAP_HELPER_8615C_H

#include "common.h"

void wm_8008615C(void);

#endif
