/*
 * World-map C624 PC-file record pass 0x800966CC.
 *
 * Retail boundary: [0x800966CC, 0x800967E4), 280 bytes / 70
 * instructions. Previous wm_80096668 ends before this prologue.
 * This body restores frame 0x28 and jr $ra; nop at 0x800967DC.
 * Next is wm_800967E4.
 *
 * Overlay callees: none. PsyQ: PCopen 0x8004C318, PClseek
 * 0x8004C348, PCread 0x8004C398, PCclose 0x8004C338. No JALR /
 * COP2 / GPU / OT.
 *
 * ABI: void. $a0 = 16-byte record list; terminator is word +0 == 0.
 * PCread dest is word +12; count is word +8 (not the W29B swap).
 */
#ifndef WORLD_MAP_HELPER_966CC_H
#define WORLD_MAP_HELPER_966CC_H

#include "common.h"

void wm_800966CC(u32 list);

#endif /* WORLD_MAP_HELPER_966CC_H */
