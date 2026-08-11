/*
 * World-map scheduler callback 0x80091430.
 *
 * Retail boundary: [0x80091430, 0x800914D0), 0xA0 bytes / 40
 * instructions. Slice SHA-256:
 * bfaefc8b20265995d504579d99b6eafb47838d284fe3ec9021e68ac15fe585d5
 */
#ifndef WORLD_MAP_CALLBACK_91430_H
#define WORLD_MAP_CALLBACK_91430_H

#include "common.h"

/* Test-only exact guest-access observer. Ordinary production builds do not
 * define WM_91430_TEST_TRACE, so the callback has no observer dependency. */
#define WM_91430_TRACE_LHU 1u
#define WM_91430_TRACE_LH  2u
#define WM_91430_TRACE_LW  3u
#define WM_91430_TRACE_SH  4u
#define WM_91430_TRACE_SW  5u

#if defined(WM_91430_TEST_TRACE)
void wm_91430_test_trace(u32 pc, u32 kind, u32 address,
                         u32 width, u32 value);
#endif

s32 wm_80091430(s32 slot_index);

#endif /* WORLD_MAP_CALLBACK_91430_H */
