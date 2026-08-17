/*
 * World-map helper 0x800747DC (GTE_OT renderer B — 1081 insns).
 *
 * Retail boundary: [0x800747DC, 0x800758C0), 1081 instructions / 4324 bytes.
 * Large GTE-based renderer. Computes terrain normals via VectorNormal,
 * applies scale/rotation via ScaleMatrix/RotMatrixY/MulMatrix,
 * processes 8 terrain sections with OuterProduct12. Uses wm_80093740
 * and wm_80093978 for terrain data.
 */
#ifndef WORLD_MAP_HELPER_747DC_H
#define WORLD_MAP_HELPER_747DC_H

#include "common.h"

void wm_800747DC(void);

#endif
