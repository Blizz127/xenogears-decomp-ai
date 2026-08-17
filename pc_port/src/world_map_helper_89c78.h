/*
 * World-map helper 0x80089C78 (scaled object renderer).
 *
 * Retail boundary: [0x80089C78, 0x8008A2C8), 404 instructions / 1616 bytes.
 * Copies camera and model matrices to scratchpad, iterates objects
 * applying ScaleMatrix + RotMatrixZ transformations, renders via
 * wm_80093534.
 */
#ifndef WORLD_MAP_HELPER_89C78_H
#define WORLD_MAP_HELPER_89C78_H

#include "common.h"

void wm_80089C78(u32 input_addr);

#endif
