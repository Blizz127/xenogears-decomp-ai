/*
 * World-map 12-byte key insertion pass 0x800963E4.
 *
 * Retail boundary: [0x800963E4, 0x800964B0), 204 bytes / 51
 * instructions. Previous wm_80096328 ends jr $ra; nop at
 * 0x800963DC. This body restores frame 0x10 and jr $ra; nop at
 * 0x800964A8. Next is wm_800964B0.
 *
 * Overlay callees: none. No JAL / JALR / COP2 / GPU / OT.
 *
 * ABI: void. $a0 = record list; key is word +0; stride 12;
 * terminator is a zero key.
 */
#ifndef WORLD_MAP_HELPER_963E4_H
#define WORLD_MAP_HELPER_963E4_H

#include "common.h"

void wm_800963E4(u32 list);

#endif /* WORLD_MAP_HELPER_963E4_H */
