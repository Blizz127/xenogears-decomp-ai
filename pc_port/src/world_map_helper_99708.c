/*
 * World-map helper 0x80099708 (tile coordinate processor).
 */
#include <stdint.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_99708.h"

#define SCRATCH      0x1F800000u
#define D_8009C618   0x8009C618u  /* table pointer */
#define D_8009C5BC   0x8009C5BCu  /* height table */
#define D_800523F0   0x800523F0u  /* sine table base */

static s16 t_lh(u32 a) { s16 v; memcpy(&v, PSX_ADDR(a), 2); return v; }
static u16 t_lhu(u32 a) { u16 v; memcpy(&v, PSX_ADDR(a), 2); return v; }
static s32 t_lw(u32 a) { s32 v; memcpy(&v, PSX_ADDR(a), 4); return v; }
static void t_sh(u32 a, u16 v) { memcpy(PSX_ADDR(a), &v, 2); }
static void t_sw(u32 a, s32 v) { memcpy(PSX_ADDR(a), &v, 4); }

void wm_80099708(u32 input_data)
{
    s32 i, j;
    u32 scratch_ptr = SCRATCH;
    u32 data_ptr = input_data;
    u32 tbl_b = (u32)t_lw(D_8009C618);
    s32 heading = t_lw(D_8009C5BC) & 0xFFF;
    s32 sub_val;
    s16 counter = -0x80;

    /* Pre-compute sine lookup index */
    u32 sine_base = D_800523F0;

    for (i = 0; i < 8; i++) {
        u32 data_cur = data_ptr;
        s32 heading_cur = heading;
        s16 counter_cur = counter;

        for (j = 0; j < 8; j++) {
            u32 entry = (u32)t_lw(data_cur);
            u32 sub = (u32)t_lw(tbl_b + (u32)(heading_cur & 0xFFF) * 4);
            s32 result;

            if (entry & 0x1000) {
                /* Compute with sine lookup */
                u32 h_idx = (u32)(heading_cur & 0xFFF);
                s16 sine_val = t_lh(sine_base + h_idx * 2);
                s32 scaled = (s32)((s16)(entry >> 16)) * (s32)sine_val;
                result = (scaled >> 20) + ((s32)(s8)(entry >> 24) << 5);
            } else {
                /* Direct computation */
                result = (s32)(entry << 24) >> 5;
            }

            /* Write to scratchpad */
            t_sw(scratch_ptr, (u32)result | ((u32)(u16)counter_cur & 0xFFFF));
            t_sh(scratch_ptr + 4, t_lhu(data_cur + 4));

            scratch_ptr += 8;
            data_cur += 4;
            heading_cur += 0x200;
        }

        counter -= 0x80;
        data_ptr += 0x80; /* stride per row */
        tbl_b += 0x200;
    }

    /* Final processing (stubbed — calls wm_8009980C which is not implemented) */
    /* wm_8009980C(input_data); */
}
