/*
 * World-map scheduler callback 0x800914D0 (slot-9 cb1).
 *
 * Retail boundary: [0x800914D0, 0x80091B54), 1668 bytes / 417
 * instructions in world_map.bin loaded at 0x8006FAF0.
 *
 * ABI:
 *   $a0 = slot index (observed 9 on the natural path)
 *   $v0 = CONSTANT 1 on every path
 *
 * Transient substate JT0 @0x80070BE4, index = (s16)(lhu +4 - 9),
 * sltiu 9. State JT1 @0x80070C0C, index = lh +0x20, sltiu 0x11.
 * Heading chase (states 2 / 0x10) then approach-or-snap toward
 * 0x8009D55C, wrap via wm_80093354(slot+0x28), publish four words
 * to 0x8009BE28.
 */
#ifndef WORLD_MAP_CALLBACK_914D0_H
#define WORLD_MAP_CALLBACK_914D0_H

#include "common.h"

s32 wm_800914D0(s32 slot_idx);

#endif /* WORLD_MAP_CALLBACK_914D0_H */
