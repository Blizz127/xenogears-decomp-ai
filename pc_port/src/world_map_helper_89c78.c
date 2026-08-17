/*
 * World-map helper 0x80089C78 (scaled object renderer).
 */
#include <stdint.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "psyq/libgte.h"
#include "world_map_helper_89c78.h"
#include "world_map_helper_93534.h"

#define SCRATCH      0x1F800000u
#define D_8009C808   0x8009C808u  /* camera matrix */
#define D_8009A180   0x8009A180u  /* model matrix */
#define D_8009B040   0x8009B040u  /* object table */
#define D_8009BE28   0x8009BE28u  /* camera position */
#define D_8009BE30   0x8009BE30u  /* camera position Z */

static s16 s_lh(u32 a) { s16 v; memcpy(&v, PSX_ADDR(a), 2); return v; }
static u16 s_lhu(u32 a) { u16 v; memcpy(&v, PSX_ADDR(a), 2); return v; }
static s32 s_lw(u32 a) { s32 v; memcpy(&v, PSX_ADDR(a), 4); return v; }
static void s_sh(u32 a, u16 v) { memcpy(PSX_ADDR(a), &v, 2); }

void wm_80089C78(u32 input_addr)
{
    s32 i;

    /* Copy camera matrix to scratchpad+0x28 */
    memcpy((void*)PSX_ADDR(SCRATCH + 0x28),
           (void*)PSX_ADDR(D_8009C808), 32);

    /* Copy model matrix to scratchpad+0x68 */
    memcpy((void*)PSX_ADDR(SCRATCH + 0x68),
           (void*)PSX_ADDR(D_8009A180), 32);

    /* Process objects */
    {
        u32 table = D_8009B040;
        u32 cam_x = (u32)s_lw(D_8009BE28);
        u32 cam_z = (u32)s_lw(D_8009BE30);

        for (i = 0; i < 256; i++) {
            u32 entry = table + (u32)i * 0x4C;
            s16 state = s_lh(entry);

            if (state != 0) continue;

            /* Check if object has valid transform */
            u32 obj_ptr = (u32)s_lw(entry + 0x20);
            if (obj_ptr == 0) continue;

            /* Apply ScaleMatrix + RotMatrixZ */
            {
                SVECTOR scale;
                MATRIX temp;

                scale.vx = s_lh(entry + 0x34);
                scale.vy = s_lh(entry + 0x36);
                scale.vz = s_lh(entry + 0x3A);

                /* ScaleMatrix from scratchpad matrix */
                ScaleMatrix((MATRIX*)PSX_ADDR(SCRATCH + 0x48), &scale);

                /* RotMatrixZ from heading */
                s16 heading = s_lhu(entry + 0x38);
                RotMatrixZ((long)heading, &temp);
            }

            /* Render via wm_80093534 */
            wm_80093534(entry + 0x28);
        }
    }
}
