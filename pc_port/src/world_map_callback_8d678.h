/*
 * World-map scheduler callback 0x8008D678 (slot-5/6 handler).
 *
 * Retail boundary: [0x8008D678, 0x8008DD6C), 1780 bytes / 445
 * instructions in world_map.bin loaded at 0x8006FAF0.
 *
 * ABI:
 *   $a0 = slot index (observed 5 on the natural path; also slot-6 cb1)
 *   $v0 = CONSTANT 1 on every path
 *
 * Transient substate (lhu +4) - 1 via 8-way JT @0x800708D4, then
 * 65-way JT1 @0x800708F4 (lh +0x20, sltiu 0x41). Follow flag is
 * lbu[0x8006F8E1+slot]. 894C8/8C1DC record is slot+0x28. Common tail
 * calls wm_80074794(1, slot+0x28) unless state==2 or (MODE_FLAG!=2 &&
 * lbu[0x8006F364+slot]==7), then sra12-publishes X/Z to
 * 0x8006EF90+(slot-4)*6 and heading to 0x8006EE52+slot*2.
 */
#ifndef WORLD_MAP_CALLBACK_8D678_H
#define WORLD_MAP_CALLBACK_8D678_H

#include "common.h"

s32 wm_8008D678(s32 slot_idx);

#endif /* WORLD_MAP_CALLBACK_8D678_H */
