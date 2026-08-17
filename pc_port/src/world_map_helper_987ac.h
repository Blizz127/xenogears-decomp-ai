/*
 * World-map helper 0x800987AC (GTE vector processor).
 *
 * Retail boundary: [0x800987AC, 0x80098CC0), 325 instructions / 1300 bytes.
 * Leaf function. Processes 4 vectors through GTE (COP2) operations:
 * loads vectors into GTE registers via lwc2, executes GTE commands
 * (opcode 0x4A480012 = NCS/normal color single), stores results via swc2.
 * Computes dot products and scaling factors from GTE output.
 */
#ifndef WORLD_MAP_HELPER_987AC_H
#define WORLD_MAP_HELPER_987AC_H

#include "common.h"

void wm_800987AC(u32 input_addr);

#endif
