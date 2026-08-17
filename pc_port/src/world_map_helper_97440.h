/*
 * World-map camera-from-angles helper 0x80097440.
 *
 * Retail boundary: [0x80097440, 0x8009766C), 556 bytes / 139
 * instructions in world_map.bin loaded at 0x8006FAF0.
 *
 * ABI:
 *   $a0 = eye SVECTOR guest ptr (lhu +0/+2/+4, each negated).
 *   $v0 unused (void).
 *
 * Reads the 32-byte MATRIX at 0x8009A180, the s16 triple at
 * 0x8009BD38/3A/3C, and writes the camera MATRIX at 0x8009C808.
 * PsyQ: RotMatrixX/Y/Z, MulMatrix0 x2, ApplyMatrix, TransMatrix.
 * No overlay JAL / JALR / COP2 / GPU / OT.
 */
#ifndef WORLD_MAP_HELPER_97440_H
#define WORLD_MAP_HELPER_97440_H

#include "common.h"

void wm_80097440(u32 eye_addr);

#endif /* WORLD_MAP_HELPER_97440_H */
