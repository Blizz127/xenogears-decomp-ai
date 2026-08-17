/*
 * World-map helper 0x80098CC0 (asset loader with queue management).
 *
 * Retail boundary: [0x80098CC0, 0x8009932C), 411 instructions / 1644 bytes.
 * Manages world-map asset loading: frees unused heap slots,
 * decodes compressed sectors via ArchiveDecodeSector, allocates
 * buffers via HeapAlloc, copies data via queue functions.
 */
#ifndef WORLD_MAP_HELPER_98CC0_H
#define WORLD_MAP_HELPER_98CC0_H

#include "common.h"

void wm_80098CC0(void);

#endif
