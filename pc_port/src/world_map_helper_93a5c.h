/*
 * World-map helper 0x80093A5C (terrain surface normal computation).
 *
 * Retail boundary: [0x80093A5C, 0x80093E8C), 268 instructions / 1072 bytes.
 * Computes terrain surface normal from packed X/Z coordinates using
 * height map lookups, cross product (OuterProduct0), and normalization
 * (VectorNormal).
 */
#ifndef WORLD_MAP_HELPER_93A5C_H
#define WORLD_MAP_HELPER_93A5C_H

#include "common.h"

s32 wm_80093A5C(u32 packed_xz, u32 data_ptr);

#endif
