/*
 * World-map helper 0x80089C78 (scaled object renderer).
 *
 * This file currently transcribes the retail prefix through the position-wrap
 * call at 0x80089F38.  The later projection/FT4 publication body remains a
 * separate frontier.  Keeping that boundary explicit is important: the old
 * placeholder invented a fixed table at 0x8009B040 and passed table storage
 * to wm_80093534, corrupting unrelated globals (notably 0x8009D7EC).
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
#define D_8009B040   0x8009B040u  /* retail vertex table, not object records */
#define D_8009BDF4   0x8009BDF4u  /* 256 x 0x4c object-record pool pointer */
#define D_8009BE28   0x8009BE28u  /* camera position */
#define D_8009BE30   0x8009BE30u  /* camera position Z */
#define OBJECT_COUNT 256
#define OBJECT_STRIDE 0x4Cu

static s16 s_lh(u32 a) { s16 v; memcpy(&v, PSX_ADDR(a), 2); return v; }
static u16 s_lhu(u32 a) { u16 v; memcpy(&v, PSX_ADDR(a), 2); return v; }
static s32 s_lw(u32 a) { s32 v; memcpy(&v, PSX_ADDR(a), 4); return v; }
static void s_sw(u32 a, s32 v) { memcpy(PSX_ADDR(a), &v, 4); }

static u32 wm_89c78_record_pool(void)
{
#if defined(WM_89C78_MUTANT_FIXED_TABLE)
    return D_8009B040;
#else
    /* Retail 0x80089D94-0x80089DB0: lw 0x8009BDF4, then record + 6. */
    return (u32)s_lw(D_8009BDF4);
#endif
}

static int wm_89c78_record_is_active(u32 record)
{
#if defined(WM_89C78_MUTANT_ZERO_IS_ACTIVE)
    return s_lh(record + 6u) == 0;
#else
    /* Retail 0x80089DB4-0x80089DC0 skips when the halfword is zero. */
    return s_lh(record + 6u) != 0;
#endif
}

void wm_80089C78(u32 input_addr)
{
    s32 i;

    /* Retail does not consume $a0 in [0x80089C78, 0x8008A2C8). */
    (void)input_addr;

    /* Copy camera matrix to scratchpad+0x28 */
    memcpy((void*)PSX_ADDR(SCRATCH + 0x28),
           (void*)PSX_ADDR(D_8009C808), 32);

    /* Copy model matrix to scratchpad+0x68 */
    memcpy((void*)PSX_ADDR(SCRATCH + 0x68),
           (void*)PSX_ADDR(D_8009A180), 32);

    /* Retail prefix: prepare each live object's scaled model matrix and its
     * camera-relative scratch VECTOR before the 0x80093534 wrap. */
    {
        u32 table = wm_89c78_record_pool();
        s32 cam_x = s_lw(D_8009BE28) >> 12;
        s32 cam_z = s_lw(D_8009BE30) >> 12;

        for (i = 0; i < OBJECT_COUNT; i++) {
            u32 entry = table + (u32)i * OBJECT_STRIDE;
            VECTOR scale;
            s32 rel_x;
            s32 rel_y;
            s32 rel_z;

            if (!wm_89c78_record_is_active(entry))
                continue;

            /* Retail copies the base model matrix anew for every live record
             * (0x80089DE0-0x80089E1C). */
            memcpy((void*)PSX_ADDR(SCRATCH + 0x48),
                   (void*)PSX_ADDR(SCRATCH + 0x68), 32);

            if ((*(u8*)PSX_ADDR(entry + 0x47u) & 1u) != 0u)
                (void)RotMatrixZ((int)(s16)s_lhu(entry + 2u),
                                 (MATRIX*)PSX_ADDR(SCRATCH + 0x48));

            /* Retail writes a VECTOR (three 32-bit components), not an
             * SVECTOR.  Z scale is the literal fixed-point 1.0. */
            scale.vx = (s32)(u16)s_lhu(entry + 0x38u);
            scale.vy = (s32)(u16)s_lhu(entry + 0x3Au);
            scale.vz = 0x1000;
            scale.pad = 0;
            (void)ScaleMatrix((MATRIX*)PSX_ADDR(SCRATCH + 0x48), &scale);

#if defined(WM_89C78_MUTANT_WRONG_POSITION_FIELDS)
            rel_x = (s_lw(entry + 4u) >> 12) - cam_x;
            rel_y = s_lw(entry + 8u) >> 12;
            rel_z = (s_lw(entry + 0x0Cu) >> 12) - cam_z;
#else
            /* Retail s0 is record+6; its +2/+6/+10 words are therefore
             * record +8/+0xc/+0x10 (0x80089F14-0x80089F38). */
            rel_x = (s_lw(entry + 8u) >> 12) - cam_x;
            rel_y = s_lw(entry + 0x0Cu) >> 12;
            rel_z = (s_lw(entry + 0x10u) >> 12) - cam_z;
#endif
#if defined(WM_89C78_MUTANT_NO_CAMERA_SUBTRACT)
            rel_x += cam_x;
            rel_z += cam_z;
#endif
            s_sw(SCRATCH + 0x88, rel_x);
            s_sw(SCRATCH + 0x8C, rel_y);
            s_sw(SCRATCH + 0x90, rel_z);

#if defined(WM_89C78_MUTANT_WRAP_RECORD)
            wm_80093534(entry + 0x28u);
#else
            /* Retail a0 = 0x1F800088.  Object storage is read-only here. */
            wm_80093534(SCRATCH + 0x88);
#endif
        }
    }
}
