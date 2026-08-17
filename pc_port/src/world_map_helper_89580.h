/*
 * World-map helper 0x80089580 (particle system tick).
 *
 * Retail boundary: [0x80089580, 0x80089748), 114 instructions / 456 bytes.
 * Leaf function. Iterates 256 particle entries (stride 0x4C),
 * updating position (pos += vel), velocity (vel += accel),
 * and color (color += delta, clamped to 0-255). Decrements
 * per-entry counter; when zero, deactivates particle and
 * decrements slot record counter.
 */
#ifndef WORLD_MAP_HELPER_89580_H
#define WORLD_MAP_HELPER_89580_H

#include "common.h"

void wm_80089580(void);

#endif
