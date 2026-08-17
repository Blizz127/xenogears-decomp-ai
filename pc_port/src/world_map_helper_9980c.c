/*
 * World-map helper 0x8009980C (GTE quad renderer).
 * Leaf function. Processes quads through GTE with color blending.
 */
#include <stdint.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_9980c.h"

#define SCRATCH 0x1F800000u
#define D_8009D7DC 0x8009D7DCu  /* table pointer */

static u32 q_lw(u32 a) { u32 v; memcpy(&v, PSX_ADDR(a), 4); return v; }
static u16 q_lhu(u32 a) { u16 v; memcpy(&v, PSX_ADDR(a), 2); return v; }
static void q_sw(u32 a, u32 v) { memcpy(PSX_ADDR(a), &v, 4); }
static void q_sh(u32 a, u16 v) { memcpy(PSX_ADDR(a), &v, 2); }

void wm_8009980C(u32 input_addr)
{
    u32 table_base = q_lw(D_8009D7DC);
    u32 scratch = SCRATCH;
    u32 entry = input_addr;

    /* Process while table offset < 0x7FE */
    while ((u32)q_lw(D_8009D7DC) < 0x7FE) {
        u32 packed = q_lw(entry);
        u32 color_mode = (packed >> 13) & 3;
        u32 base_color = ((packed >> 4) & 0xF0) | ((packed >> 8) & 0xF000);
        u32 colors[4];

        /* Compute 4 color variants based on mode */
        switch (color_mode) {
        case 0:
            colors[0] = base_color;
            colors[1] = base_color + 0x0F;
            colors[2] = base_color + 0xF00;
            colors[3] = base_color + 0xF0F;
            break;
        case 1:
            colors[0] = base_color + 0x0F;
            colors[1] = base_color;
            colors[2] = base_color + 0xF0F;
            colors[3] = base_color + 0xF00;
            break;
        case 2:
            colors[0] = base_color + 0xF00;
            colors[1] = base_color + 0xF0F;
            colors[2] = base_color;
            colors[3] = base_color + 0x0F;
            break;
        case 3:
        default:
            colors[0] = base_color + 0xF0F;
            colors[1] = base_color + 0xF00;
            colors[2] = base_color + 0x0F;
            colors[3] = base_color;
            break;
        }

        /* Check if entry has valid data */
        u32 flags = packed & 0xFFFF0000;
        if ((s32)flags >= 0) {
            /* Write colors to scratchpad for GTE processing */
            q_sw(scratch + 0, (u32)colors[0]);
            q_sw(scratch + 4, (u32)colors[1]);
            q_sw(scratch + 8, (u32)colors[2]);
            q_sw(scratch + 12, (u32)colors[3]);
        }

        entry += 8;
        scratch += 0x10;
    }
}
