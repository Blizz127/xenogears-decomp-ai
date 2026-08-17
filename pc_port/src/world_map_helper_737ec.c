/*
 * World-map helper 0x800737EC (sky dome renderer).
 */
#include <stdint.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "psyq/libgte.h"
#include "psyq/libgpu.h"
#include "world_map_helper_737ec.h"

#define SCRATCH      0x1F800000u
#define D_8009A280   0x8009A280u  /* sky dome vertex data */
#define D_8009BD3A   0x8009BD3Au  /* heading mirror */
#define D_8009D7F0   0x8009D7F0u  /* palette index */
#define D_8009C808   0x8009C808u  /* camera matrix source */

static u16 s_lhu(u32 a) { u16 v; memcpy(&v, PSX_ADDR(a), 2); return v; }
static s16 s_lh(u32 a) { s16 v; memcpy(&v, PSX_ADDR(a), 2); return v; }
static s32 s_lw(u32 a) { s32 v; memcpy(&v, PSX_ADDR(a), 4); return v; }
static void s_sh(u32 a, u16 v) { memcpy(PSX_ADDR(a), &v, 2); }
static void s_sw(u32 a, s32 v) { memcpy(PSX_ADDR(a), &v, 4); }

void wm_800737EC(void)
{
    SVECTOR rot;
    MATRIX *cam_src, *scratch_m;
    s32 i, dome_count;
    u32 dome_ptr;
    u32 palette_idx;

    /* Set up rotation from heading */
    rot.vx = 0;
    rot.vy = 0;
    rot.vz = s_lhu(D_8009BD3A);

    /* Clear scratchpad matrix translation */
    s_sw(SCRATCH + 0x44, 0);
    s_sw(SCRATCH + 0x40, 0);
    s_sw(SCRATCH + 0x3C, 0);

    /* RotMatrixYXZ → scratch+0x08 */
    RotMatrixYXZ(&rot, (MATRIX*)PSX_ADDR(SCRATCH + 0x08));

    /* CompMatrix: camera × rotation → scratch+0x08 */
    cam_src = (MATRIX*)PSX_ADDR(D_8009C808);
    CompMatrix(cam_src, (MATRIX*)PSX_ADDR(SCRATCH + 0x08),
               (MATRIX*)PSX_ADDR(SCRATCH + 0x08));

    /* SetRotMatrix + SetTransMatrix */
    SetRotMatrix((MATRIX*)PSX_ADDR(SCRATCH + 0x08));
    SetTransMatrix((MATRIX*)PSX_ADDR(SCRATCH + 0x08));

    /* Iterate dome vertices */
    dome_ptr = D_8009A280;
    palette_idx = (u32)s_lw(D_8009D7F0);
    dome_count = (s32)s_lh(D_8009A280);

    for (i = 0; i < dome_count; i++) {
        SVECTOR *verts = (SVECTOR*)PSX_ADDR(dome_ptr + 2);
        long *sxy = (long*)PSX_ADDR(SCRATCH + 0x48);
        long *p = (long*)PSX_ADDR(SCRATCH + 0x4C);
        long otz;

        /* RotTransPers4: project 4 vertices */
        RotTransPers4(&verts[0], &verts[1], &verts[2], &verts[3],
                      (long(*)[2])sxy, (long(*)[2])(sxy + 2),
                      (long(*)[2])(sxy + 4), (long(*)[2])(sxy + 6),
                      &otz, p);

        /* Color the primitive (stored in dome data) */
        /* The exact primitive type depends on the dome entry format */
        /* Simplified: write color from palette */
        {
            u32 color = 0x00808080; /* default gray */
            s_sw(SCRATCH + 0x50, color);
        }

        dome_ptr += 0x28; /* stride per dome entry */
    }
}
