/*
 * World-map D788/C624 wait 0x80096130.
 *
 * Retail boundary: [0x80096130, 0x8009623C), 268 bytes / 67
 * instructions. Previous function ends before this prologue.
 * This body restores frame 0x28 and jr $ra; nop at 0x80096234.
 * Next is wm_8009623C.
 *
 * Overlay callees: wm_800967E4. PsyQ: Vsync 0x8004B54C. SLUS:
 * func_8002C3D8 x2. No JALR / COP2 / GPU / OT.
 *
 * ABI: void. Polls D788[*0x8009BE44] or C624[*0x8009BE44] until 0.
 */
#ifndef WORLD_MAP_HELPER_96130_H
#define WORLD_MAP_HELPER_96130_H

#include "common.h"

void wm_80096130(void);

#endif /* WORLD_MAP_HELPER_96130_H */
