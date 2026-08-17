/*
 * World-map helper 0x800747DC (GTE_OT renderer B).
 * Large GTE-based terrain renderer.
 */
#include <stdint.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "psyq/libgte.h"
#include "world_map_helper_747dc.h"
#include "world_map_terrain_sampler.h"

#define SCRATCH      0x1F800000u
#define D_8009C808   0x8009C808u  /* camera matrix */
#define D_8009BD3A   0x8009BD3Au  /* heading */
#define D_8009D7F0   0x8009D7F0u  /* palette index */

extern void wm_80093740(u32 addr);

static s16 t_lh(u32 a) { s16 v; memcpy(&v, PSX_ADDR(a), 2); return v; }
static u16 t_lhu(u32 a) { u16 v; memcpy(&v, PSX_ADDR(a), 2); return v; }
static s32 t_lw(u32 a) { s32 v; memcpy(&v, PSX_ADDR(a), 4); return v; }
static void t_sh(u32 a, u16 v) { memcpy(PSX_ADDR(a), &v, 2); }
static void t_sw(u32 a, s32 v) { memcpy(PSX_ADDR(a), &v, 4); }

void wm_800747DC(void)
{
    s32 i;
    VECTOR edge1, edge2, cross, normal;
    MATRIX temp;

    /* Process 8 terrain sections */
    for (i = 0; i < 8; i++) {
        u32 section = SCRATCH + 0x28 + (u32)i * 0x80;
        s32 heading = (s32)t_lh(D_8009BD3A);

        /* Compute terrain height at section position */
        s32 sx = t_lw(section);
        s32 sz = t_lw(section + 8);
        s32 terrain_y = wm_80093978(sx, sz);

        /* Build edge vectors from terrain samples */
        s32 y_n = wm_80093978(sx + 0x1000, sz);
        s32 y_e = wm_80093978(sx, sz + 0x1000);

        edge1.vx = 0x1000;
        edge1.vy = y_n - terrain_y;
        edge1.vz = 0;

        edge2.vx = 0;
        edge2.vy = y_e - terrain_y;
        edge2.vz = 0x1000;

        /* Cross product for surface normal */
        OuterProduct12(&edge1, &edge2, &cross);

        /* Normalize */
        VectorNormal(&cross, &normal);

        /* Build rotation matrix from heading */
        RotMatrixY((long)heading, &temp);

        /* Apply scale */
        {
            SVECTOR scale;
            scale.vx = 0x1000;
            scale.vy = 0x1000;
            scale.vz = 0x1000;
            ScaleMatrix(&temp, &scale);
        }

        /* MulMatrix with camera */
        MulMatrix0((MATRIX*)PSX_ADDR(D_8009C808), &temp, &temp);

        /* Store result */
        memcpy((void*)PSX_ADDR(section + 0x20), &temp, sizeof(MATRIX));
    }

    /* Final terrain processing */
    wm_80093740(SCRATCH + 0x28);
}
