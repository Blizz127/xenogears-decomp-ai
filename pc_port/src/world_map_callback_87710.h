/*
 * World-map scheduler callback 0x80087710 (slot 15, Table-B cb0).
 *
 * Retail boundary: [0x80087710, 0x80087734), 36 bytes / 9 instructions.
 * Slice SHA-256:
 * a3f62bc691ef9e4fecea300f902b7be0a48beb84b8ac5406f3e439ea95909e78
 */
#ifndef WORLD_MAP_CALLBACK_87710_H
#define WORLD_MAP_CALLBACK_87710_H

#include "common.h"

/* Test-only exact guest-access observer. Ordinary production builds do not
 * define WM_87710_TEST_TRACE and therefore have no observer dependency. */
#define WM_87710_TRACE_LW 1u
#define WM_87710_TRACE_SW 2u

#if defined(WM_87710_TEST_TRACE)
void wm_87710_test_trace(u32 pc, u32 kind, u32 address,
                         u32 width, u32 value);
#endif

s32 wm_80087710(s32 slot_index);

#endif /* WORLD_MAP_CALLBACK_87710_H */
