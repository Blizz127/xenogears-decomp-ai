/*
 * World-map helper 0x80089C78 (scaled object renderer).
 *
 * Retail function boundary: [0x80089C78, 0x8008A2C8).  The current ported
 * boundary ends after the per-record matrix/vector setup and retail wrap call
 * at 0x80089F38.  Projection and FT4 publication remain to be transcribed.
 */
#ifndef WORLD_MAP_HELPER_89C78_H
#define WORLD_MAP_HELPER_89C78_H

#include "common.h"

void wm_80089C78(u32 input_addr);

#endif
