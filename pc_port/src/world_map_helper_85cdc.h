/*
 * World-map helper 0x80085CDC (object position updater).
 *
 * Retail boundary: [0x80085CDC, 0x80085FE0), 193 instructions / 772 bytes.
 * Iterates 64 world objects (stride 0x80), computes camera-relative
 * position delta via wm_80093484, writes scaled transform to object
 * records. Sets GTE matrices for subsequent rendering.
 */
#ifndef WORLD_MAP_HELPER_85CDC_H
#define WORLD_MAP_HELPER_85CDC_H

#include "common.h"

void wm_80085CDC(void);

#endif
