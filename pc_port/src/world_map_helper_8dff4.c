/*
 * World-map fixed-point vector initializer 0x8008DFF4.
 *
 * Register-faithful, finite-width transcription of retail world_map.bin
 * [0x8008DFF4, 0x8008E034).  Loads three signed halfwords from the global
 * world-vector source at 0x8006EE60/62/64, sign-extends each to 32 bits,
 * shifts left by 12 (fixed-point scale), and stores the three resulting
 * words to the caller-supplied output buffer.
 */
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_8dff4.h"

#define WM_DFF4_SRC_X  0x8006EE60u
#define WM_DFF4_SRC_Y  0x8006EE62u
#define WM_DFF4_SRC_Z  0x8006EE64u

void wm_8008DFF4(u32 out_addr)
{
    s16 raw;
    s32 extended;
    u32 bits;
    u32 result;

    /* X component */
    memcpy(&raw, PSX_ADDR(WM_DFF4_SRC_X), sizeof(raw));
    extended = (s32)raw;
    bits = (u32)extended;
    result = bits << 12;
    memcpy(PSX_ADDR(out_addr + 0u), &result, sizeof(result));

    /* Y component */
    memcpy(&raw, PSX_ADDR(WM_DFF4_SRC_Y), sizeof(raw));
    extended = (s32)raw;
    bits = (u32)extended;
    result = bits << 12;
    memcpy(PSX_ADDR(out_addr + 4u), &result, sizeof(result));

    /* Z component */
    memcpy(&raw, PSX_ADDR(WM_DFF4_SRC_Z), sizeof(raw));
    extended = (s32)raw;
    bits = (u32)extended;
    result = bits << 12;
    memcpy(PSX_ADDR(out_addr + 8u), &result, sizeof(result));
}
