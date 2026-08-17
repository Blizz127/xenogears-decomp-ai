/*
 * World-map D788 CD start 0x8009699C.
 *
 * Retail boundary: [0x8009699C, 0x80096A6C), 208 bytes / 52
 * instructions. Previous wm_800968E0 ends jr $ra; nop at
 * 0x80096994. This body restores frame 0x18 and jr $ra; nop at
 * 0x80096A64. Next is the CdSync handler at 0x80096A6C.
 *
 * Overlay callees: none. PsyQ: CdIntToPos 0x80041430,
 * CdSyncCallback 0x80040FB4, CdControlF 0x8004111C. No JALR /
 * COP2 / GPU / OT.
 *
 * ABI: void. $a0 = 12-byte D788 record.
 */
#ifndef WORLD_MAP_HELPER_9699C_H
#define WORLD_MAP_HELPER_9699C_H

#include "common.h"

void wm_8009699C(u32 record);

#endif /* WORLD_MAP_HELPER_9699C_H */
