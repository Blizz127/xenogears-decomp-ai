/*
 * World-map scheduler callback 0x8008B644 (slot-2/3 companion handler).
 *
 * Retail boundary: [0x8008B644, 0x8008BB40), 1276 bytes / 319
 * instructions in world_map.bin loaded at 0x8006FAF0.
 *
 * ABI:
 *   $a0 = slot index (observed 2 on the natural route after 8A72C)
 *   $v0 = CONSTANT 1 on every path ($s4 preset in the prologue)
 *
 * Transient substate lh slot[+0x04], then 65-way JT1 @0x80070670
 * (index = lh slot[+0x20], sltiu 0x41; OOR and 53 parked slots ->
 * common tail). Companion/follower: no 95414 / 8C040 / 94238, and no
 * 8A72C-style sra12 publish. Common tail calls wm_80074794 only while
 * slot[+0x24] == 0.
 *
 * See docs/evidence/w34b24-pre4-8b644/.
 */
#ifndef WORLD_MAP_CALLBACK_8B644_H
#define WORLD_MAP_CALLBACK_8B644_H

#include "common.h"

s32 wm_8008B644(s32 slot_idx);

#endif /* WORLD_MAP_CALLBACK_8B644_H */
