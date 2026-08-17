/*
 * World-map helper 0x800981C8 (tile coordinate table builder).
 * Leaf function, no external calls.
 */
#include <stdint.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_981c8.h"

#define D_8009D160  0x8009D160u  /* X wrap threshold */
#define D_8009D2B4  0x8009D2B4u  /* Z wrap threshold */
#define D_8009C838  0x8009C838u  /* camera tile X offset (s16) */
#define D_8009C83C  0x8009C83Cu  /* camera tile Z offset (s16) */
#define D_8009D318  0x8009D318u  /* destination record (160 bytes) */
#define D_8009D570  0x8009D570u  /* tile lookup table (8x8) */
#define D_8009D560  0x8009D560u  /* Y value source */

static s16 t8_lh(u32 a) { s16 v; memcpy(&v, PSX_ADDR(a), 2); return v; }
static u16 t8_lhu(u32 a) { u16 v; memcpy(&v, PSX_ADDR(a), 2); return v; }
static s32 t8_lw(u32 a) { s32 v; memcpy(&v, PSX_ADDR(a), 4); return v; }
static void t8_sh(u32 a, u16 v) { memcpy(PSX_ADDR(a), &v, 2); }
static void t8_sw(u32 a, s32 v) { memcpy(PSX_ADDR(a), &v, 4); }

void wm_800981C8(u32 pos_vec)
{
    s32 x_tile, z_tile;
    s32 x_wrapped, z_wrapped;
    s32 i, j;
    u32 threshold_x = t8_lw(D_8009D160);
    u32 threshold_z = t8_lw(D_8009D2B4);
    s16 cam_x = t8_lh(D_8009C838);
    s16 cam_z = t8_lh(D_8009C83C);

    /* Compute tile coordinates: (pos >> 12 + 7) >> 3 */
    x_tile = (s32)t8_lw(pos_vec);
    if (x_tile < 0) x_tile += 7;
    x_tile >>= 3;

    z_tile = (s32)t8_lw(pos_vec + 8);
    if (z_tile < 0) z_tile += 7;
    z_tile >>= 3;

    /* Compute wrapped coordinates */
    x_wrapped = x_tile - (s32)(cam_x + 2) * 256;
    z_wrapped = z_tile - (s32)(cam_z + 2) * 256;

    /* Wrap X */
    if (x_wrapped < 0) {
        x_wrapped += (s32)(threshold_x << 8);
    } else if (x_wrapped >= (s32)(threshold_x << 8)) {
        x_wrapped -= (s32)(threshold_x << 8);
    }

    /* Wrap Z */
    if (z_wrapped < 0) {
        z_wrapped += (s32)(threshold_z << 8);
    } else if (z_wrapped >= (s32)(threshold_z << 8)) {
        z_wrapped -= (s32)(threshold_z << 8);
    }

    /* Copy 160-byte record from D_8009D570 to D_8009D318 */
    {
        u8* src = (u8*)PSX_ADDR(D_8009D570);
        u8* dst = (u8*)PSX_ADDR(D_8009D318);
        memcpy(dst, src, 160);
    }

    /* Fill 8x8 tile lookup table at D_8009D570 */
    {
        s32 tz = z_wrapped >> 8;
        for (i = 0; i < 8; i++) {
            s32 tx = x_wrapped >> 8;
            for (j = 0; j < 8; j++) {
                s32 tile_val;
                if (tx < 0) tx = 0;
                if (tx >= (s32)threshold_x) tx = 0;
                tile_val = (s32)tz * (s32)threshold_x + tx;
                t8_sh(D_8009D570 + (i * 8 + j) * 2, (u16)tile_val);
                tx++;
            }
            tz++;
        }
    }
}
