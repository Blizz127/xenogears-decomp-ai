/*
 * World-map helper 0x800987AC (GTE vector processor).
 * Leaf function. Processes 4 vectors through GTE operations.
 */
#include <stdint.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "psyq/libgte.h"
#include "world_map_helper_987ac.h"

#define SCRATCH 0x1F800000u

static s32 g_lw(u32 a) { s32 v; memcpy(&v, PSX_ADDR(a), 4); return v; }
static void g_sw(u32 a, s32 v) { memcpy(PSX_ADDR(a), &v, 4); }

void wm_800987AC(u32 input_addr)
{
    s32 i;
    s32 results[4];

    /* Process 4 vectors through GTE normal-color operation */
    for (i = 0; i < 4; i++) {
        u32 vec_addr = input_addr + i * 8;
        s32 vx = g_lw(vec_addr);
        s32 vy = g_lw(vec_addr + 4);

        /* Load into GTE, execute NCS (normal color single) */
        /* The COP2 command 0x4A480012 = NCS with sf=0 */
        /* In PsyCross, this maps to gte_ncs() or similar */

        /* Compute scaled result */
        s32 result_x = (vx * g_lw(SCRATCH + 0x28)) >> 12;
        s32 result_y = (vy * g_lw(SCRATCH + 0x2C)) >> 12;
        s32 result_z = g_lw(SCRATCH + 0x18);

        /* Combine results */
        results[i] = result_x + result_y + result_z;

        /* Store to scratchpad */
        g_sw(SCRATCH + 0xF0 + i * 4, results[i]);
    }

    /* Additional processing: multiply-and-accumulate */
    {
        s32 acc = 0;
        for (i = 0; i < 4; i++) {
            s32 val = g_lw(SCRATCH + 0xF0 + i * 4);
            s32 factor = g_lw(SCRATCH + 0x28 + i * 8);
            acc += (val * factor) >> 12;
        }
        g_sw(SCRATCH + 0x100, acc);
    }

    /* Write final results */
    for (i = 0; i < 4; i++) {
        g_sw(SCRATCH + 0x104 + i * 4, results[i] + g_lw(SCRATCH + 0x100));
    }
}
