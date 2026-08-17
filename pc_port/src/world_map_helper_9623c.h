/*
 * World-map 12-byte queue push 0x8009623C.
 *
 * Retail boundary: [0x8009623C, 0x800962B0), 116 bytes / 29
 * instructions. Previous wm_80096130 ends jr $ra; nop at
 * 0x80096234. This body is jr $ra; nop at 0x800962A8. Next is
 * wm_800962B0.
 *
 * Overlay callees: none. No JAL / JALR / COP2 / GPU / OT.
 *
 * ABI: s32. $a0/$a1/$a2 stored; $v0 = 0 or -1 if *0x8009D808 >= 88.
 */
#ifndef WORLD_MAP_HELPER_9623C_H
#define WORLD_MAP_HELPER_9623C_H

#include "common.h"

s32 wm_8009623C(u32 a0, u32 a1, u32 a2);

#endif /* WORLD_MAP_HELPER_9623C_H */
