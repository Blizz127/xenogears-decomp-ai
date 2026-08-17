/*
 * World-map 16-byte key insertion pass 0x800964B0.
 *
 * Retail boundary: [0x800964B0, 0x800965A4), 244 bytes / 61
 * instructions. Previous wm_800963E4 ends jr $ra; nop at
 * 0x800964A8. This body restores frame 0x10 and jr $ra; nop at
 * 0x8009659C. Next is wm_800965A4.
 *
 * Overlay callees: none. No JAL / JALR / COP2 / GPU / OT.
 *
 * ABI: void. $a0 = record list; key is word +4; stride 16;
 * terminator is a zero word at +0 of the next record.
 */
#ifndef WORLD_MAP_HELPER_964B0_H
#define WORLD_MAP_HELPER_964B0_H

#include "common.h"

void wm_800964B0(u32 list);

#endif /* WORLD_MAP_HELPER_964B0_H */
