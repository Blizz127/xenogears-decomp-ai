/*
 * World-map bank fill 0x80098CC0.
 *
 * Retail boundary: [0x80098CC0, 0x8009932C), 1644 bytes / 411
 * instructions. Previous body ends jr $ra; nop at 0x80098CB8.
 * This body restores frame 0x38 and jr $ra; nop at 0x80099324.
 * Next is wm_8009932C (identity end still UNRESOLVED).
 *
 * Overlay callees: wm_8009623C, wm_800962B0, wm_80096328,
 * wm_800965A4 (all ACCEPTED). No JALR / COP2 / GPU / OT.
 *
 * SLUS: ArchiveGetFilePath 0x80028998, ArchiveDecodeSector
 * 0x800289D0, func_8002C3D8 0x8002C3D8, HeapAlloc 0x80031BDC,
 * HeapFree 0x800320E8.
 *
 * ABI: void. No incoming args. Same debug-table predicate as
 * 96130/967E4. CD path enqueues 12-byte records via 9623C and
 * publishes with 96328. PC path enqueues 16-byte records via
 * 962B0 and publishes with 965A4.
 */
#ifndef WORLD_MAP_HELPER_98CC0_H
#define WORLD_MAP_HELPER_98CC0_H

#include "common.h"

void wm_80098CC0(void);

#endif /* WORLD_MAP_HELPER_98CC0_H */
