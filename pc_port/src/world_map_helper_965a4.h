/*
 * World-map 16-byte bank publish 0x800965A4.
 *
 * Retail boundary: [0x800965A4, 0x80096668), 196 bytes / 49
 * instructions. Previous wm_800964B0 ends jr $ra; nop at
 * 0x8009659C. This body restores frame 0x28 and jr $ra; nop at
 * 0x80096660. Next is wm_80096668.
 *
 * Overlay callees: wm_800964B0. No JALR / COP2 / GPU / OT.
 *
 * ABI: s32. No args. $v0 = 0 after sorting bank *0x8009BE44
 * of the 0x580-stride pool at *0x8009D3C0, else -1.
 */
#ifndef WORLD_MAP_HELPER_965A4_H
#define WORLD_MAP_HELPER_965A4_H

#include "common.h"

s32 wm_800965A4(void);

#endif /* WORLD_MAP_HELPER_965A4_H */
