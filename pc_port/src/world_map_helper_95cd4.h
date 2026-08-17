/*
 * World-map helper 0x80095CD4 (collision-aware movement probe).
 *
 * Retail boundary: [0x80095CD4, 0x80095F78), 169 instructions / 676 bytes.
 * Computes target = pos + vel*scale>>12, clamps Y to terrain, probes
 * nav-mesh. If collision, returns halved negated velocity. Otherwise
 * calls wm_800951A8 for full collision resolution.
 */
#ifndef WORLD_MAP_HELPER_95CD4_H
#define WORLD_MAP_HELPER_95CD4_H

#include "common.h"

s32 wm_80095CD4(u32 pos, u32 vel, u32 out, s32 scale);

#endif
