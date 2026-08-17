/*
 * World-map helper 0x8008615C (vertex setup with rotation).
 */
#include <stdint.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "psyq/libgte.h"
#include "world_map_helper_8615c.h"

#define SCRATCH      0x1F800000u
#define D_8009C808   0x8009C808u  /* camera matrix */
#define D_8009A180   0x8009A180u  /* model matrix */
#define D_8009BD3C   0x8009BD3Cu  /* direction area (s16) */

static s16 v_lh(u32 a) { s16 v; memcpy(&v, PSX_ADDR(a), 2); return v; }

void wm_8008615C(void)
{
    /* Initialize vertex coordinates in scratchpad */
    /* 9 vertices: 3 sets of 3 SVECTORs */
    s16* verts = (s16*)PSX_ADDR(SCRATCH);

    /* Set 1: (-0x18, -0x48, 0), (0x18, -0x48, 0), (-0x18, 0, 0) */
    verts[0] = -0x18; verts[1] = -0x48; verts[2] = 0;
    verts[4] = 0x18;  verts[5] = -0x48; verts[6] = 0;
    verts[8] = -0x18; verts[9] = 0;     verts[10] = 0;

    /* Set 2: (0x18, 0, 0), (-0x18, 0, 0), (0x18, 0x48, 0) */
    verts[12] = 0x18;  verts[13] = 0;     verts[14] = 0;
    verts[16] = -0x18; verts[17] = 0;     verts[18] = 0;
    verts[20] = 0x18;  verts[21] = 0x48;  verts[22] = 0;

    /* Set 3: (-0x18, 0x48, 0), (0x18, 0x48, 0), (0, 0, 0) */
    verts[24] = -0x18; verts[25] = 0x48;  verts[26] = 0;
    verts[28] = 0x18;  verts[29] = 0x48;  verts[30] = 0;
    verts[32] = 0;     verts[33] = 0;     verts[34] = 0;

    /* Copy camera matrix to scratchpad+0x28 */
    {
        MATRIX* src = (MATRIX*)PSX_ADDR(D_8009C808);
        MATRIX* dst = (MATRIX*)PSX_ADDR(SCRATCH + 0x28);
        memcpy(dst, src, sizeof(MATRIX));
    }

    /* Copy model matrix to scratchpad+0x48 */
    {
        MATRIX* src = (MATRIX*)PSX_ADDR(D_8009A180);
        MATRIX* dst = (MATRIX*)PSX_ADDR(SCRATCH + 0x48);
        memcpy(dst, src, sizeof(MATRIX));
    }

    /* Apply RotMatrixZ from direction area */
    {
        s16 dir_area = v_lh(D_8009BD3C);
        MATRIX temp;
        RotMatrixZ((long)dir_area, &temp);
        /* The result overwrites part of the scratchpad matrix */
        memcpy((void*)PSX_ADDR(SCRATCH + 0x48), &temp, sizeof(MATRIX));
    }

    /* wm_80099BFC: final vertex processing (stubbed) */
    /* wm_80099BFC(SCRATCH + 0x48); */
}
