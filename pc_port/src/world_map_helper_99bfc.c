/*
 * World-map helper 0x80099BFC (coordinate wrapper for render entries).
 * Leaf function, no external calls.
 */
#include <stdint.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_99bfc.h"

#define D_8009BE04  0x8009BE04u  /* entry count */
#define D_8009BE28  0x8009BE28u  /* camera position X */
#define D_8009BE30  0x8009BE30u  /* camera position Z */
#define D_8009D160  0x8009D160u  /* X threshold */
#define D_8009D2B4  0x8009D2B4u  /* Z threshold */

static s16 w_lh(u32 a) { s16 v; memcpy(&v, PSX_ADDR(a), 2); return v; }
static u16 w_lhu(u32 a) { u16 v; memcpy(&v, PSX_ADDR(a), 2); return v; }
static s32 w_lw(u32 a) { s32 v; memcpy(&v, PSX_ADDR(a), 4); return v; }
static void w_sh(u32 a, u16 v) { memcpy(PSX_ADDR(a), &v, 2); }
static void w_sw(u32 a, s32 v) { memcpy(PSX_ADDR(a), &v, 4); }

void wm_80099BFC(u32 entries)
{
    s32 count = w_lw(D_8009BE04);
    if (count >= 0x200) return;

    s32 cam_x = w_lw(D_8009BE28) >> 12;
    s32 cam_z = w_lw(D_8009BE30) >> 12;
    s32 threshold_x = w_lw(D_8009D160) << 11;
    s32 threshold_z = w_lw(D_8009D2B4) << 11;

    u32 entry = entries;
    s32 i;

    for (i = 0; i < count; i++) {
        s32 packed = w_lw(entry);
        s16 ez = w_lh(entry + 4);

        /* Extract signed X from packed word */
        s16 ex = (s16)(u16)(packed & 0xFFFF);
        s32 dx = (s32)ex - cam_x;
        s32 dz = (s32)ez - cam_z;

        /* Wrap X */
        if (dx < -0x4000) dx += threshold_x;
        if (dx >= 0x4000) dx -= threshold_x;

        /* Wrap Z */
        if (dz < -0x4000) dz += threshold_z;
        if (dz >= 0x4000) dz -= threshold_z;

        /* Store wrapped coordinates */
        w_sh(entry, (u16)(dx + cam_x));
        w_sh(entry + 4, (u16)(dz + cam_z));

        entry += 8;
    }
}
