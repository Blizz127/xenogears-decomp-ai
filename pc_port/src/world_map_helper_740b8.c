/*
 * World-map helper 0x800740B8 (GTE_OT renderer A).
 */
#include <stdint.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "psyq/libgte.h"
#include "world_map_helper_740b8.h"

#define SCRATCH      0x1F800000u
#define D_8009A340   0x8009A340u  /* object table */
#define D_8009D55C   0x8009D55Cu  /* pose block */
#define D_8009BD3A   0x8009BD3Au  /* heading mirror */
#define D_8009D7F0   0x8009D7F0u  /* palette index */
#define D_8009BCDC   0x8009BCDCu  /* scale factor */
#define D_8009BE0C   0x8009BE0Cu  /* scroll value */

static u16 g_lhu(u32 a) { u16 v; memcpy(&v, PSX_ADDR(a), 2); return v; }
static s16 g_lh(u32 a) { s16 v; memcpy(&v, PSX_ADDR(a), 2); return v; }
static s32 g_lw(u32 a) { s32 v; memcpy(&v, PSX_ADDR(a), 4); return v; }
static void g_sh(u32 a, u16 v) { memcpy(PSX_ADDR(a), &v, 2); }
static void g_sw(u32 a, s32 v) { memcpy(PSX_ADDR(a), &v, 4); }

void wm_800740B8(void)
{
    s32 i, count;
    u32 palette_idx;
    u32 obj_base;
    u32 obj_entry;
    SVECTOR rot;

    /* Initialize scratchpad constants */
    g_sw(SCRATCH + 0x10, 0x800);
    g_sw(SCRATCH + 0x14, 0x800);
    g_sw(SCRATCH + 0x18, 0x800);

    /* Set rendering mode */
    g_sw(0x80050104, 3);

    /* Compute object table entry */
    palette_idx = (u32)g_lw(D_8009D7F0);
    obj_base = (u32)g_lw(0x8009C664u);
    obj_entry = obj_base + (palette_idx * 112); /* stride = (8*idx - idx)*16 */

    /* Set up rotation from heading */
    rot.vx = 0;
    rot.vy = 0;
    rot.vz = g_lhu(D_8009BD3A);

    g_sh(SCRATCH + 0xBA, 0);
    g_sh(SCRATCH + 0xB8, 0);
    g_sh(SCRATCH + 0xBC, (u16)rot.vz);

    /* Build rotation matrix */
    RotMatrix(&rot, (MATRIX*)PSX_ADDR(SCRATCH + 0xB8));

    /* Compute screen coordinates from pose block */
    {
        s32 px = g_lw(D_8009D55C);
        s32 pz = g_lw(D_8009D55C + 8);
        s32 scale = g_lw(D_8009BCDC);
        s32 scroll = g_lw(D_8009BE0C);

        /* Fixed-point position computation */
        s32 sx = (px >> 12) * 0xD00D00D1;
        s32 sx_adj = ((sx >> 8) + (px >> 12)) - (px >> 31) + 0x30;
        g_sw(SCRATCH + 0x104, sx_adj);

        s32 sz = (pz >> 12) * 0x300C0301;
        s32 sz_adj = ((sz >> 6) - (pz >> 31)) + 0x78 - scroll;
        g_sw(SCRATCH + 0x108, sz_adj);

        g_sw(SCRATCH + 0x10C, g_lw(0x8009BCDCu));
    }

    /* Process objects — simplified iteration */
    count = g_lh(0x8009D7E0u);
    if (count > 0) {
        u32 data_base = (u32)g_lw(0x8009C620u);
        for (i = 0; i < count; i++) {
            u32 entry = data_base + (u32)i * 4;
            s16 state = g_lh(entry);

            if (state == 0) {
                /* Load object transform into scratchpad */
                u32 obj_ptr = (u32)g_lw(entry + 0x20);
                if (obj_ptr != 0) {
                    memcpy((void*)PSX_ADDR(SCRATCH + 0xF0),
                           (void*)PSX_ADDR(obj_ptr), 32);
                }
            }
        }
    }
}
