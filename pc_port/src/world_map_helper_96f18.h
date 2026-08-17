/*
 * World-map heading/pose transform helper 0x80096F18.
 *
 * Retail boundary: [0x80096F18, 0x80097070), 344 bytes / 86
 * instructions in world_map.bin loaded at 0x8006FAF0.
 *
 * ABI:
 *   $a0 = dest   : guest ptr. Writes SVECTOR at +0, pad zeros at
 *                  +8/+0xC, s16 (lw pose+4 >> 12) at +0xA, and
 *                  ApplyMatrix VECTOR at +0x10.
 *   $a1 = pose   : guest ptr; only +4 is read (lw, then sra 12).
 *   $a2 = scale  : s32; sra 12 then negu becomes ApplyMatrixLV Z.
 *   $a3 = angles : guest SVECTOR (lhu +0/+2/+4).
 *   $v0 unused (void).
 *
 * Scratch: 0x1F8000A0 SVECTOR, 0x1F8000F0 MATRIX, 0x1F800000 /
 * 0x1F800010 VECTORs. PsyQ: RotMatrixYXZ x2, ApplyMatrixLV,
 * ApplyMatrix. No JALR / COP2 / GPU / OT.
 */
#ifndef WORLD_MAP_HELPER_96F18_H
#define WORLD_MAP_HELPER_96F18_H

#include "common.h"

void wm_80096F18(u32 dest_addr, u32 pose_addr, s32 scale, u32 angles_addr);

#endif /* WORLD_MAP_HELPER_96F18_H */
