/*
 * World-map scheduler callback 0x80091C18 (slot-10 cb1).
 *
 * Retail boundary: [0x80091C18, 0x80091FF8), 992 bytes / 248
 * instructions in world_map.bin loaded at 0x8006FAF0.
 *
 * ABI:
 *   $a0 = slot index (observed 10 on the natural path)
 *   $v0 = CONSTANT 1 on every path
 *
 * Transient +4: 9 / 0xA install heading tables; 0xE parks; 0x11
 * arms state 3. States 0/1 probe via wm_80091FF8 and approach
 * 0x8009D3F0 / 0x8009BD38; state 2 copies BE2C>>12 into dest+0xA;
 * state 3 nudges BD38 by +2. Common 96F18 tail except state 2 /
 * unknown. +0x22 increments every return.
 */
#ifndef WORLD_MAP_CALLBACK_91C18_H
#define WORLD_MAP_CALLBACK_91C18_H

#include "common.h"

s32 wm_80091C18(s32 slot_idx);

#endif /* WORLD_MAP_CALLBACK_91C18_H */
