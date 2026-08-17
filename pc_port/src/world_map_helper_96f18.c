/*
 * World-map helper 0x80096F18 (GTE view matrix builder).
 */
#include <stdint.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "psyq/libgte.h"
#include "world_map_helper_96f18.h"

#define SCRATCH 0x1F800000u

static s16 f18_lh(u32 a) { s16 v; memcpy(&v, PSX_ADDR(a), 2); return v; }
static u16 f18_lhu(u32 a) { u16 v; memcpy(&v, PSX_ADDR(a), 2); return v; }
static s32 f18_lw(u32 a) { s32 v; memcpy(&v, PSX_ADDR(a), 4); return v; }
static void f18_sh(u32 a, u16 v) { memcpy(PSX_ADDR(a), &v, 2); }
static void f18_sw(u32 a, s32 v) { memcpy(PSX_ADDR(a), &v, 4); }

void wm_80096F18(u32 out_matrix, u32 pos_vec, s32 height, u32 rot_svec)
{
    SVECTOR rot;
    VECTOR pos;
    MATRIX *m = (MATRIX*)PSX_ADDR(out_matrix);
    MATRIX temp;

    /* Set output matrix column 0: (0, pos.Y>>12, 0) */
    f18_sh(out_matrix + 8, 0);
    f18_sh(out_matrix + 0xC, 0);
    f18_sh(out_matrix + 0xA, (u16)((s32)f18_lw(pos_vec + 4) >> 12));

    /* Build rotation SVECTOR from rot_svec */
    rot.vx = (s16)f18_lhu(rot_svec);
    rot.vy = (s16)f18_lhu(rot_svec + 2);
    rot.vz = 0;

    /* RotMatrixYXZ → scratch+0xA0 */
    RotMatrixYXZ(&rot, (MATRIX*)PSX_ADDR(SCRATCH + 0xA0));

    /* ApplyMatrixLV: transform (0, 0, -height>>12) through matrix */
    {
        s32 h = -(height >> 12);
        pos.vx = 0;
        pos.vy = 0;
        pos.vz = h;
    }
    ApplyMatrixLV((MATRIX*)PSX_ADDR(SCRATCH + 0xF0), &pos,
                  (VECTOR*)PSX_ADDR(SCRATCH + 0x10));

    /* Store first column result */
    f18_sh(out_matrix + 0, (u16)f18_lw(SCRATCH + 0x10));
    f18_sh(out_matrix + 2, (u16)(f18_lw(SCRATCH + 0x14) + f18_lw(out_matrix + 0xA)));
    f18_sh(out_matrix + 4, (u16)f18_lw(SCRATCH + 0x18));

    /* Build second rotation */
    f18_sh(SCRATCH + 0xA0, 0);
    f18_sh(SCRATCH + 0xA2, f18_lhu(rot_svec + 2));
    f18_sh(SCRATCH + 0xA4, f18_lhu(rot_svec + 4));
    RotMatrixYXZ((SVECTOR*)PSX_ADDR(SCRATCH + 0xA0),
                 (MATRIX*)PSX_ADDR(SCRATCH + 0xF0));

    /* ApplyMatrix: transform (0, -0x1000, 0) through matrix */
    f18_sh(SCRATCH + 0xA0, 0);
    f18_sh(SCRATCH + 0xA2, (u16)(s16)(-0x1000));
    f18_sh(SCRATCH + 0xA4, 0);
    ApplyMatrix((MATRIX*)PSX_ADDR(SCRATCH + 0xF0),
                (SVECTOR*)PSX_ADDR(SCRATCH + 0xA0),
                (VECTOR*)PSX_ADDR(out_matrix + 0x10));
}
