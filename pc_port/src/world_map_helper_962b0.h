/*
 * World-map 16-byte queue push 0x800962B0.
 *
 * Retail boundary: [0x800962B0, 0x80096328), 120 bytes / 30
 * instructions. Previous wm_8009623C ends jr $ra; nop at
 * 0x800962A8. This body is jr $ra; nop at 0x80096320. Next is
 * wm_80096328.
 *
 * Overlay callees: none. No JAL / JALR / COP2 / GPU / OT.
 *
 * ABI: s32. $a0/$a1/$a2/$a3 stored; $v0 = 0 or -1 if
 * *0x8009D808 >= 88.
 */
#ifndef WORLD_MAP_HELPER_962B0_H
#define WORLD_MAP_HELPER_962B0_H

#include "common.h"

s32 wm_800962B0(u32 a0, u32 a1, u32 a2, u32 a3);

#endif /* WORLD_MAP_HELPER_962B0_H */
