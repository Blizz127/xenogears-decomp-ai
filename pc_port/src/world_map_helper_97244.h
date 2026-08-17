/*
 * World-map camera-from-look helper 0x80097244.
 *
 * Retail boundary: [0x80097244, 0x80097440), 508 bytes / 127
 * instructions in world_map.bin loaded at 0x8006FAF0.
 *
 * ABI:
 *   $a0 = look record: SVECTOR at +0, SVECTOR at +8, VECTOR at +0x10.
 *   $v0 unused (void).
 *
 * Writes camera MATRIX 0x8009C808. PsyQ: VectorNormal x3,
 * OuterProduct12 x2, ApplyMatrix, TransMatrix. No overlay JAL.
 */
#ifndef WORLD_MAP_HELPER_97244_H
#define WORLD_MAP_HELPER_97244_H

#include "common.h"

void wm_80097244(u32 look_addr);

#endif /* WORLD_MAP_HELPER_97244_H */
