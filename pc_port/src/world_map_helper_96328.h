/*
 * World-map 12-byte bank publish 0x80096328.
 *
 * Retail boundary: [0x80096328, 0x800963E4), 188 bytes / 47
 * instructions. Previous wm_800962B0 ends jr $ra; nop at
 * 0x80096320. This body restores frame 0x28 and jr $ra; nop at
 * 0x800963DC. Next is wm_800963E4.
 *
 * Overlay callees: wm_800963E4. No JALR / COP2 / GPU / OT.
 *
 * ABI: s32. No args. $v0 = 0 after sorting bank *0x8009BE44
 * of the 0x420-stride pool at *0x8009BE08, else -1.
 */
#ifndef WORLD_MAP_HELPER_96328_H
#define WORLD_MAP_HELPER_96328_H

#include "common.h"

s32 wm_80096328(void);

#endif /* WORLD_MAP_HELPER_96328_H */
