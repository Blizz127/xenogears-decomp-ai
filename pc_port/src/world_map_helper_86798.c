/*
 * World-map helper 0x80086798 (OT dispatcher).
 * Main world-map ordering-table dispatcher.
 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "psyq/libgte.h"
#include "world_map_helper_86798.h"

#define SCRATCH      0x1F800000u
#define D_8009ADB0   0x8009ADB0u  /* source block A (48×8 bytes) */
#define D_8009AD50   0x8009AD50u  /* source block B (12×8 bytes) */
#define D_8009AD40   0x8009AD40u  /* halfword table (8 entries) */
#define D_8009BE28   0x8009BE28u  /* camera position */
#define D_8009BE30   0x8009BE30u
#define D_8009CD40   0x8009CD40u  /* callback pointer */

static u16 o_lhu(u32 a) { u16 v; memcpy(&v, PSX_ADDR(a), 2); return v; }
static s16 o_lh(u32 a) { s16 v; memcpy(&v, PSX_ADDR(a), 2); return v; }
static s32 o_lw(u32 a) { s32 v; memcpy(&v, PSX_ADDR(a), 4); return v; }
static void o_sh(u32 a, u16 v) { memcpy(PSX_ADDR(a), &v, 2); }
static void o_sw(u32 a, s32 v) { memcpy(PSX_ADDR(a), &v, 4); }

#define D_8009D150   0x8009D150u  /* -> table A: 80 records x 16 bytes */
#define D_8009CEB4   0x8009CEB4u  /* -> table B: 80 records x 8 bytes */

/* W34B25: exact native of retail 0x80086700-0x80086794 (world_map.bin,
 * 38 insns, leaf).  CD40 callback installed by world_map_init.c
 * (WM_MES_FN_86700).  For 80 records: A[i].w0 += (s16)B[i].h0,
 * A[i].w2 += (s16)B[i].h2, each wrapped into [0, 0x2000000):
 *   if (0x1FFFFFF < v) v += 0xFE000000;  if (v < 0) v += 0x2000000; */
static void wm_80086700_cd40_init(void)
{
    u32 a = (u32)o_lw(D_8009D150);
    u32 b = (u32)o_lw(D_8009CEB4);
    s32 i;
    for (i = 0; i < 80; i++) {
        s32 v1 = o_lw(a + 0) + o_lh(b + 0);
        s32 v0 = o_lw(a + 8) + o_lh(b + 4);
        if (0x1FFFFFF < v1) v1 += (s32)0xFE000000;
        if (v1 < 0) v1 += 0x2000000;
        if (0x1FFFFFF < v0) v0 += (s32)0xFE000000;
        if (v0 < 0) v0 += 0x2000000;
        o_sw(a + 0, v1);
        o_sw(a + 8, v0);
        a += 16;
        b += 8;
    }
}

static int s_wm_86798_cd40_boundary_hits;

void wm_80086798(void)
{
    s32 i;
    u32 callback;

    /* Indirect call via function pointer (retail jalr [CD40]).  The slot
     * holds a GUEST address; never jump to it on the host.  W34B25: map the
     * only known value to its native, anything else is a counted boundary. */
    callback = (u32)o_lw(D_8009CD40);
    if (callback == 0x80086700u) {
        wm_80086700_cd40_init();
    } else if (callback != 0) {
        s_wm_86798_cd40_boundary_hits++;
        fprintf(stderr, "[worldmap-86798] BOUNDARY: CD40 callback 0x%08x "
                "has no native; skipped\n", callback);
    }

    /* Copy 48 blocks of 8 bytes from D_8009ADB0 to scratchpad+0x60 */
    for (i = 0; i < 48; i++) {
        u32 src = D_8009ADB0 + (u32)i * 8;
        u32 dst = SCRATCH + 0x60 + (u32)i * 8;
        memcpy(PSX_ADDR(dst), PSX_ADDR(src), 8);
    }

    /* Copy 12 blocks of 8 bytes from D_8009AD50 to scratchpad */
    for (i = 0; i < 12; i++) {
        u32 src = D_8009AD50 + (u32)i * 8;
        u32 dst = SCRATCH + (u32)i * 8;
        memcpy(PSX_ADDR(dst), PSX_ADDR(src), 8);
    }

    /* Copy 8 halfwords from D_8009AD40 to scratchpad+0x1E0 */
    for (i = 0; i < 8; i++) {
        o_sh(SCRATCH + 0x1E0 + i * 2, o_lhu(D_8009AD40 + i * 2));
    }

    /* Store camera position (masked to 25 bits) */
    {
        u32 cam_x = (u32)o_lw(D_8009BE28) & 0x1FFFFFF;
        u32 cam_z = (u32)o_lw(D_8009BE30) & 0x1FFFFFF;
        s32 cam_tile = (o_lw(D_8009BE28) >> 12) & 0x7FF;

        o_sw(SCRATCH + 0x210, cam_x);
        o_sw(SCRATCH + 0x218, cam_z);
        o_sw(SCRATCH + 0x220, cam_tile);
    }

    /* Store camera Z tile */
    {
        s32 cam_z_tile = (o_lw(D_8009BE30) >> 12) & 0x7FF;
        o_sw(SCRATCH + 0x220, cam_z_tile);
    }

    /* Process objects with rcos/rsin transformations */
    {
        u32 obj_table = SCRATCH + 0x250;
        u32 out_table = SCRATCH + 0x270;

        for (i = 0; i < 48; i++) {
            u32 entry = SCRATCH + 0x60 + (u32)i * 8;
            s16 heading = o_lh(entry + 4);

            if (heading != 0) {
                /* Apply rcos/rsin rotation */
                s32 cos_val = (s32)rcos((long)heading);
                s32 sin_val = (s32)rsin((long)heading);

                /* Compute rotated position */
                s32 px = o_lw(entry);
                s32 pz = o_lw(entry + 8);

                s32 rx = (px * cos_val - pz * sin_val) >> 12;
                s32 rz = (px * sin_val + pz * cos_val) >> 12;

                o_sw(obj_table + 0, rx);
                o_sw(obj_table + 4, rz);
            }

            obj_table += 8;
            out_table += 8;
        }
    }
}
