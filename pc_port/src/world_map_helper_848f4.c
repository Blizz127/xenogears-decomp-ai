/*
 * World-map helper 0x800848F4 (rendering structure initializer).
 * Leaf function, no external calls.
 */
#include <stdint.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_848f4.h"

#define SCRATCH      0x1F800000u
#define D_8009BE28   0x8009BE28u  /* camera position X */
#define D_8009BE30   0x8009BE30u  /* camera position Z */
#define D_8009D7E0   0x8009D7E0u  /* object count */
#define D_8009C620   0x8009C620u  /* object table pointer */
#define D_800695C0   0x800695C0u  /* counter A */
#define D_80069578   0x80069578u  /* counter B */

static s16 r_lh(u32 a) { s16 v; memcpy(&v, PSX_ADDR(a), 2); return v; }
static s32 r_lw(u32 a) { s32 v; memcpy(&v, PSX_ADDR(a), 4); return v; }
static void r_sh(u32 a, u16 v) { memcpy(PSX_ADDR(a), &v, 2); }
static void r_sw(u32 a, s32 v) { memcpy(PSX_ADDR(a), &v, 4); }

void wm_800848F4(void)
{
    s32 i, count;
    s32 cam_x, cam_z;

    /* Initialize scratchpad constants */
    r_sw(SCRATCH + 0x10, 0x800);
    r_sw(SCRATCH + 0x14, 0x800);
    r_sw(SCRATCH + 0x18, 0x800);

    /* Set rendering mode */
    r_sw(0x80050104, 3);

    /* Read camera position */
    cam_x = r_lw(D_8009BE28) >> 12;
    cam_z = r_lw(D_8009BE30) >> 12;

    /* Clear global counters */
    r_sw(D_800695C0, 0);
    r_sw(D_80069578, 0);
    r_sh(SCRATCH + 0xA0, 0);
    r_sh(SCRATCH + 0xA2, 0);
    r_sh(SCRATCH + 0xA4, 0);

    /* Iterate object entries */
    count = r_lh(D_8009D7E0);
    if (count <= 0) return;

    {
        u32 table = (u32)r_lw(D_8009C620);
        u32 scratch_f0 = SCRATCH + 0xF0;
        u32 scratch_f2 = SCRATCH + 0xF2;

        for (i = 0; i < count; i++) {
            u32 entry = table + (u32)i * 4;
            s16 state = r_lh(entry);

            if (state == 0) {
                u32 obj_ptr = (u32)r_lw(entry + 0x20);
                if (obj_ptr != 0) {
                    /* Load 4 words of transform data */
                    r_sw(scratch_f0 + 0, r_lw(obj_ptr + 0x20));
                    r_sw(scratch_f0 + 4, r_lw(obj_ptr + 0x24));
                    r_sw(scratch_f0 + 8, r_lw(obj_ptr + 0x28));
                    r_sw(scratch_f0 + 12, r_lw(obj_ptr + 0x2C));

                    /* Load 4 words of secondary data */
                    r_sw(scratch_f0 + 0x10, r_lw(obj_ptr + 0x30));
                    r_sw(scratch_f0 + 0x14, r_lw(obj_ptr + 0x34));
                    r_sw(scratch_f0 + 0x18, r_lw(obj_ptr + 0x38));
                    r_sw(scratch_f0 + 0x1C, r_lw(obj_ptr + 0x3C));

                    /* Store camera-relative offset */
                    r_sw(scratch_f0 + 0x14, r_lw(entry + 8));
                    r_sw(scratch_f0 + 0x18, r_lw(entry + 0xC));
                    r_sw(scratch_f0 + 0x1C, r_lw(entry + 0x10));
                }
            }

            scratch_f0 += 0x80;
            scratch_f2 += 0x80;
        }
    }
}
