/*
 * World-map CD/PC work dispatch 0x800967E4.
 *
 * Retail boundary: [0x800967E4, 0x800968E0), 252 bytes / 63
 * instructions. Previous wm_800966CC ends jr $ra; nop at
 * 0x800967DC. This body restores frame 0x28 and jr $ra; nop at
 * 0x800968D8. Next is wm_800968E0.
 *
 * Overlay callees: wm_800968E0, wm_8009699C, wm_800966CC. SLUS:
 * func_8002C3D8 x2. No JALR / COP2 / GPU / OT.
 *
 * ABI: s32(void). Return is $s1 (0 unless 968E0 is nonzero).
 */
#ifndef WORLD_MAP_HELPER_967E4_H
#define WORLD_MAP_HELPER_967E4_H

#include "common.h"

s32 wm_800967E4(void);

#endif /* WORLD_MAP_HELPER_967E4_H */
