/*
 * World-map scheduler callback 0x80092FD8 (slot-13 cb1).
 *
 * Retail boundary: [0x80092FD8, 0x800931B0), 472 bytes / 118
 * instructions in world_map.bin loaded at 0x8006FAF0.
 * Slice SHA-256:
 * 48ea7bdc89adc8380c26447944ffdd4ae81d8e0cbd620c55e6abc743ca06f7a4
 *
 * ABI:
 *   $a0 = slot index (Table A slot 13 on the natural path)
 *   $v0 = 1
 *
 * Same enqueue machine as 0x80092C70, but the window object is
 * 0x8009BD64 and the selection word is 0x8009CE68. After
 * func_80034888, state +20==1 also AddPrims 0x8009D2B8+ctx*0x28
 * into *(*(0x8009BE3C)+0x70). No JALR / COP2 / scratchpad.
 */
#ifndef WORLD_MAP_CALLBACK_92FD8_H
#define WORLD_MAP_CALLBACK_92FD8_H

#include "common.h"

s32 wm_80092FD8(s32 slot_idx);

#endif /* WORLD_MAP_CALLBACK_92FD8_H */
