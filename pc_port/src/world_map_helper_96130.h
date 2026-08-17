/*
 * World-map helpers 0x80096130 and 0x800967E4 — queue synchronization.
 *
 * 0x80096130 [0x80096130, 0x8009623C): 67 insns. Queue drain loop.
 * Polls archive debug table, calls Vsync(0) + wm_800967E4 until
 * queue entries are consumed.
 *
 * 0x800967E4 [0x800967E4, 0x800968E0): 63 insns. Queue processor.
 * Checks archive status, calls file I/O (966CC) or CD (9699C)
 * depending on available data.
 */
#ifndef WORLD_MAP_HELPER_96130_H
#define WORLD_MAP_HELPER_96130_H

#include "common.h"

void wm_80096130(void);
void wm_800967E4(void);

#endif
