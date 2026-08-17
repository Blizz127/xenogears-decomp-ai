/*
 * World-map scheduler callback 0x80092C70 (slot-12 cb1).
 *
 * Retail boundary: [0x80092C70, 0x80092DD0), 352 bytes / 88
 * instructions in world_map.bin loaded at 0x8006FAF0.
 * Slice SHA-256:
 * 264e5f7d7b1d9bf228470253d45c650afd943eabd573fe9b53c336846f5e4815
 *
 * ABI:
 *   $a0 = slot index (Table A slot 12 on the natural path)
 *   $v0 = 1
 *
 * Internal +0x20: 0 waits for a valid 0x8009BD24 selection, then
 * enqueues that string and arms 1; 1 clears on -1 or re-enqueues
 * when the cached +0x50 word differs. Every path ends in
 * func_80034888. No JALR / COP2 / scratchpad / GPU / OT of its own.
 */
#ifndef WORLD_MAP_CALLBACK_92C70_H
#define WORLD_MAP_CALLBACK_92C70_H

#include "common.h"

s32 wm_80092C70(s32 slot_idx);

#endif /* WORLD_MAP_CALLBACK_92C70_H */
