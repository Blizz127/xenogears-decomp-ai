/*
 * World-map helper 0x80093A5C (terrain surface normal computation).
 */
#include <stdint.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_93a5c.h"

extern long rcos(long a);
extern void wm_80093660(void);
extern void wm_800935DC(u32 a, u32 b, u32 c);

/* Local vector types for GTE functions */
typedef struct { long vx, vy, vz; } Wm93Vec;
extern void OuterProduct0(Wm93Vec* v0, Wm93Vec* v1, Wm93Vec* out);
extern long VectorNormal(Wm93Vec* v0, Wm93Vec* v1);

#define SCRATCH   0x1F800000u
#define TABLE_A   0x8009C5BCu  /* terrain height table A pointer */
#define TABLE_B   0x8009C618u  /* terrain height table B pointer */

static s8 a5c_lb(u32 a) { return *(s8*)PSX_ADDR(a); }
static u8 a5c_lbu(u32 a) { return *(u8*)PSX_ADDR(a); }
static s16 a5c_lh(u32 a) { s16 v; memcpy(&v, PSX_ADDR(a), 2); return v; }
static u16 a5c_lhu(u32 a) { u16 v; memcpy(&v, PSX_ADDR(a), 2); return v; }
static s32 a5c_lw(u32 a) { s32 v; memcpy(&v, PSX_ADDR(a), 4); return v; }
static void a5c_sh(u32 a, u16 v) { memcpy(PSX_ADDR(a), &v, 2); }
static void a5c_sw(u32 a, s32 v) { memcpy(PSX_ADDR(a), &v, 4); }

s32 wm_80093A5C(u32 packed_xz, u32 data_ptr)
{
    s32 i;
    u32 tbl_a, tbl_b;
    u32 s6; /* data pointer from wm_80093660 */
    u32 tile_x, tile_z;
    u32 heights[5]; /* 5 sample heights */

    /* Get base data pointer */
    s6 = (u32)a5c_lw(/* wm_80093660 result stored somewhere */0);

    /* Extract tile indices from packed coordinates */
    tile_x = (packed_xz >> 19) & 7;
    tile_z = (packed_xz >> 19) & 7; /* from $a1 (same register, different arg) */

    /* Load terrain tables */
    tbl_a = (u32)a5c_lw(TABLE_A);
    tbl_b = (u32)a5c_lw(TABLE_B);

    /* Compute5 sample heights using rcos lookups */
    /* Each height = (rcos(tbl + tile*512) * scale_factor) >> 20 + (byte << 3) */
    for (i = 0; i < 5; i++) {
        s32 cos_a = (s32)rcos((long)(s32)(tbl_a + tile_x * 512));
        s32 cos_b = (s32)rcos((long)(s32)(tbl_b + tile_z * 512));
        s32 product = cos_a * ((cos_b << 4) >> 3);
        s8 sample_byte = a5c_lb(data_ptr + i * 4);
        heights[i] = (product >> 20) + ((s32)sample_byte << 3);

        /* Advance tile indices for next sample */
        if (i < 2) {
            tile_x++;
        } else if (i < 4) {
            tile_z++;
        }
    }

    /* Build edge vectors from heights and compute cross product */
    {
        Wm93Vec edge1, edge2, cross, normal;
        s32 h0 = heights[0], h1 = heights[1], h2 = heights[2];
        s32 h3 = heights[3], h4 = heights[4];
        u8 flag_byte = a5c_lbu(data_ptr + 1);

        /* Edge vectors depend on flag bit 0x80 */
        if (flag_byte & 0x80) {
            /* Path A */
            s32 dx1 = (s32)a5c_lh(SCRATCH + 0xA2);
            s32 dy1 = h2 - h0;
            s32 dz1 = (s32)a5c_lh(SCRATCH + 0xB2);
            s32 dx2 = (s32)a5c_lh(SCRATCH + 0xAA);
            s32 dy2 = h3 - h0;
            s32 dz2 = (s32)a5c_lh(SCRATCH + 0xB2);

            edge1.vx = 0x80;
            edge1.vy = h1 - dx1;
            edge1.vz = h4 - dx1;
            edge2.vx = 0;
            edge2.vy = dy1;
            edge2.vz = dz1;
        } else {
            /* Path B */
            s32 dx1 = (s32)a5c_lh(SCRATCH + 0xAA);
            s32 dy1 = (s32)a5c_lh(SCRATCH + 0xA2);
            s32 dz1 = (s32)a5c_lh(SCRATCH + 0xB2);

            edge1.vx = -0x80;
            edge1.vy = 0;
            edge1.vz = 0;
            edge2.vx = h2 - dy1;
            edge2.vy = h3 - dy1;
            edge2.vz = dz1;
        }

        /* Cross product */
        OuterProduct0(&edge1, &edge2, &cross);

        /* Normalize */
        VectorNormal(&cross, &normal);

        /* Store position (packed_xz >> 12 & 0x7F) */
        a5c_sw(SCRATCH + 0x10, (packed_xz >> 12) & 0x7F);
        a5c_sw(SCRATCH + 0x18, -((s32)(/* $a1 */ 0 >> 12) & 0x7F));

        /* Set up color/attribute data */
        {
            u16 attr;
            if (flag_byte & 0x80) {
                attr = a5c_lhu(SCRATCH + 0xA2);
                a5c_sw(SCRATCH + 0x20, 0);
            } else {
                attr = a5c_lhu(SCRATCH + 0xAA);
                a5c_sw(SCRATCH + 0x20, 0x80);
            }
            a5c_sw(SCRATCH + 0x28, 0);
            a5c_sw(SCRATCH + 0x24, (s32)(s16)attr);
        }

        /* Final computation */
        wm_800935DC(SCRATCH + 0x10, SCRATCH + 0x20, SCRATCH);
    }

    /* Return result: *(SCRATCH + 0x14) << 12 */
    return a5c_lw(SCRATCH + 0x14) << 12;
}
