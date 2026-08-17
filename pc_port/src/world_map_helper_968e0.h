/*
 * World-map CD44 state dispatch 0x800968E0.
 *
 * Retail boundary: [0x800968E0, 0x8009699C), 188 bytes / 47
 * instructions. Previous wm_800967E4 ends jr $ra; nop at
 * 0x800968D8. This body is jr $ra; nop at 0x80096994. Next is
 * wm_8009699C.
 *
 * Overlay callees: none. jr $v0 at 0x80096908 is a 6-entry switch
 * at 0x80070CA0, all targets inside this body:
 *   0 -> 0x80096910, 1/2/3 -> 0x80096950, 4 -> 0x80096918,
 *   5 -> 0x80096958. Guard is sltiu state, 6. No JAL / JALR /
 *   COP2 / GPU / OT.
 *
 * ABI: s32(void). Reads *0x8009CD44.
 */
#ifndef WORLD_MAP_HELPER_968E0_H
#define WORLD_MAP_HELPER_968E0_H

#include "common.h"

s32 wm_800968E0(void);

#endif /* WORLD_MAP_HELPER_968E0_H */
