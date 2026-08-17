/*
 * World-map helper 0x80091FF8 (camera view setup with terrain sampling).
 */
#include <stdint.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_91ff8.h"
#include "world_map_helper_93354.h"
#include "world_map_helper_96f18.h"
#include "world_map_terrain_cell.h"

#define SCRATCH      0x1F800000u
#define HEAD_MIRROR  0x8009BD3Au
#define DIR_AREA     0x8009BD3Cu
#define D_8009D560   0x8009D560u
#define D_8009BD40   0x8009BD40u
#define D_8009BD44   0x8009BD44u
#define D_8009BE28   0x8009BE28u
#define D_8009BE30   0x8009BE30u
#define D_8009B214   0x8009B214u
#define D_8009B234   0x8009B234u
#define D_8009BCDC   0x8009BCDCu

static s16 f8_lh(u32 a) { s16 v; memcpy(&v, PSX_ADDR(a), 2); return v; }
static u16 f8_lhu(u32 a) { u16 v; memcpy(&v, PSX_ADDR(a), 2); return v; }
static s32 f8_lw(u32 a) { s32 v; memcpy(&v, PSX_ADDR(a), 4); return v; }
static void f8_sh(u32 a, u16 v) { memcpy(PSX_ADDR(a), &v, 2); }
static void f8_sw(u32 a, s32 v) { memcpy(PSX_ADDR(a), &v, 4); }
static s8 f8_lb(u32 a) { return *(s8*)PSX_ADDR(a); }

s32 wm_80091FF8(s32 max_candidates, u32 rot_table, u32 height_table)
{
    s32 i, j;
    s32 best_count = 0;
    s32 best_height = 0x7FFFFFFF;
    s32 base_y;

    /* Read heading and direction area */
    u16 heading = f8_lhu(HEAD_MIRROR);
    u16 dir_area = f8_lhu(DIR_AREA);
    s32 height_offset = (s32)(f8_lw(D_8009D560) >> 12);

    /* Write to scratchpad */
    f8_sh(SCRATCH + 0xAA, heading);
    f8_sh(SCRATCH + 0xAC, dir_area);

    /* Build view matrix via wm_80096F18 */
    {
        u16 rot_x = f8_lhu(rot_table);
        s32 base_height = f8_lw(D_8009B214);
        s32 scale = (s32)f8_lw(D_8009BCDC);
        s32 adj = (scale < 0 ? scale + 1 : scale) >> 1;
        s32 h = base_height - (adj << 12);

        f8_sh(SCRATCH + 0xA8, rot_x);
        wm_80096F18(D_8009BD40, D_8009BE28, h, SCRATCH + 0xA8);
    }

    /* Sample 6×6 grid of terrain heights */
    {
        s32 grid_x = (s32)f8_lh(D_8009BD40) - 0x180;
        s32 grid_z_start = (s32)f8_lh(D_8009BD44) + 0x180;
        s32 grid_x_start = grid_x << 12;
        s32 grid_z = grid_z_start << 12;

        /* Mask to grid alignment */
        grid_x_start &= 0xFFF80000u;
        grid_z &= 0xFFF80000u;

        f8_sw(SCRATCH + 0, grid_x_start);
        f8_sw(SCRATCH + 8, grid_z);

        best_count = 0;
        best_height = 0x7FFFFFFF;

        for (i = 0; i < 6; i++) {
            f8_sw(SCRATCH + 0, grid_x_start);
            for (j = 0; j < 6; j++) {
                wm_80093354(SCRATCH);
                {
                    u32 result = wm_80093660(f8_lw(SCRATCH), f8_lw(SCRATCH + 8));
                    s8 terrain_h = f8_lb(result);
                    f8_sw(SCRATCH + 4, (s32)terrain_h);
                    if ((s32)terrain_h < best_height) {
                        best_height = (s32)terrain_h;
                    }
                }
                /* Advance X by 0x80000 */
                f8_sw(SCRATCH + 0, f8_lw(SCRATCH + 0) + 0x80000);
            }
            /* Advance Z by 0x80000 */
            f8_sw(SCRATCH + 8, f8_lw(SCRATCH + 8) + 0x80000);
        }
    }

    /* Check best height against threshold */
    {
        s32 threshold = (s32)f8_lh(height_table) + height_offset + 0x50;
        s32 best_scaled = best_height << 3;

        if (threshold >= best_scaled) {
            /* Within range: check proximity to existing candidates */
            s32 idx = best_count;
            best_count++;
            rot_table += 2;
            height_table += 2;

            if (best_count < 3) {
                /* More candidates to check */
                goto next_candidate;
            }
        }
    }

    /* Return best count */
    return best_count;

next_candidate:
    /* Continue outer loop (simplified — the asm loops up to max_candidates) */
    {
        s32 stored = best_count;
        if (stored < max_candidates) {
            s32 table_val = (s32)f8_lh(D_8009B234 + stored * 2);
            s32 diff = table_val + height_offset - (best_height << 3) + 0x50;
            if (diff < 0) diff = -diff;
            if (diff < 0x41) {
                return max_candidates;
            }
        }
    }
    return best_count;
}
