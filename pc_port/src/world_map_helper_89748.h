/*
 * World-map helper 0x80089748 (particle effect spawner/updater).
 *
 * Retail boundary: [0x80089748, 0x80089C78), 332 instructions / 1328 bytes.
 * Iterates particle entries (stride 0x4C), checks flags, builds
 * rotation matrices, generates random angles via rand(), calls
 * ApplyMatrix for transformation, ticks particles via wm_80089580.
 */
#ifndef WORLD_MAP_HELPER_89748_H
#define WORLD_MAP_HELPER_89748_H

#include "common.h"

void wm_80089748(void);

#endif
