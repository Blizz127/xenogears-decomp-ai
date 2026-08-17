/*
 * World-map scheduler callback 0x8008C844 (slot-4 world handler).
 *
 * Retail boundary: [0x8008C844, 0x8008D3F0), 2988 bytes / 747
 * instructions in world_map.bin loaded at 0x8006FAF0.
 *
 * ABI:
 *   $a0 = slot index (observed 4 on the natural Lahan→world path)
 *   $v0 = CONSTANT 1 on every path ($s5 preset in the prologue)
 *
 * Transient substate lh slot[+0x04], then 65-way JT1 @0x800707CC
 * (index = lh slot[+0x20], sltiu 0x41; OOR and 45 parked slots ->
 * common tail).  Init class dispatch is a hardcoded 90C68 result
 * tree, not 8A72C's JT2.  Common tail calls wm_80074794(1, slot+0x28)
 * unless state==2 or (MODE_FLAG!=2 && lbu 0x8006F368==7), then
 * always sra12-publishes X/Z/heading to 0x8006EF90/92 and 0x8006EE5A.
 */
#ifndef WORLD_MAP_CALLBACK_8C844_H
#define WORLD_MAP_CALLBACK_8C844_H

#include "common.h"

s32 wm_8008C844(s32 slot_idx);

#endif /* WORLD_MAP_CALLBACK_8C844_H */
