/*
 * World-map helper 0x800907F4 (rotation animation with RotMatrixZYX).
 */
#include <stdint.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "psyq/libgte.h"
#include "world_map_helper_907f4.h"

/* PsyCross has RotMatrixZYX_gte but not RotMatrixZYX */
extern MATRIX* RotMatrixZYX_gte(SVECTOR* r, MATRIX* m);

#define POOL_PTR  0x8009BE24u
#define TABLE_PTR 0x8009C620u  /* -0x39E0: matrix table pointer */

static s16 f4_lh(u32 a) { s16 v; memcpy(&v, PSX_ADDR(a), 2); return v; }
static u16 f4_lhu(u32 a) { u16 v; memcpy(&v, PSX_ADDR(a), 2); return v; }
static s32 f4_lw(u32 a) { s32 v; memcpy(&v, PSX_ADDR(a), 4); return v; }
static void f4_sh(u32 a, u16 v) { memcpy(PSX_ADDR(a), &v, 2); }
static void f4_sw(u32 a, s32 v) { memcpy(PSX_ADDR(a), &v, 4); }

s32 wm_800907F4(s32 slot_idx)
{
    u32 pool_ptr = (u32)f4_lw(POOL_PTR);
    u32 table_ptr = (u32)f4_lw(TABLE_PTR);
    u32 slot = pool_ptr + (u32)(slot_idx << 7);
    u32 mat_a = table_ptr + 0xA8;
    u32 mat_b = table_ptr + 0xFC;
    u32 scratch = 0x1F800000u;

    /* Pre-dispatch on slot[+0x04] */
    s16 sub = f4_lh(slot + 0x04);
    if (sub == 9) {
        f4_sh(slot + 0x04, 0);
        f4_sh(slot + 0x20, 1);
    } else if (sub == 0x0A) {
        f4_sh(slot + 0x04, 0);
        f4_sh(slot + 0x20, 2);
    }

    /* Main dispatch on slot[+0x20] */
    {
        s16 state = f4_lh(slot + 0x20);

        if (state == 0) {
            /* Clear counters */
            f4_sw(slot + 0x58, 0);
            f4_sw(slot + 0x5C, 0);
        } else if (state == 1) {
            /* Increment counters */
            s32 c1 = f4_lw(slot + 0x58) + 4;
            f4_sw(slot + 0x58, c1);

            if (c1 >= 0x11) {
                s32 c2 = f4_lw(slot + 0x5C) + 4;
                f4_sw(slot + 0x5C, c2);
            }

            /* Clamp to 0x80 */
            if (f4_lw(slot + 0x58) >= 0x80) {
                f4_sw(slot + 0x58, 0x80);
            }
            if (f4_lw(slot + 0x5C) >= 0x80) {
                f4_sw(slot + 0x5C, 0x80);
            }

            /* Advance to state 3 when both >= 0x80 */
            if (f4_lw(slot + 0x58) >= 0x80 && f4_lw(slot + 0x5C) >= 0x80) {
                f4_sh(slot + 0x20, 3);
            }
        } else if (state == 2) {
            /* Decrement counters */
            s32 c1 = f4_lw(slot + 0x58) - 4;
            f4_sw(slot + 0x58, c1);

            if (c1 < 0x70) {
                s32 c2 = f4_lw(slot + 0x5C) - 4;
                f4_sw(slot + 0x5C, c2);
            }

            /* Clamp to 0 */
            if (f4_lw(slot + 0x58) < 0) f4_sw(slot + 0x58, 0);
            if (f4_lw(slot + 0x5C) < 0) f4_sw(slot + 0x5C, 0);

            /* Return to state 0 when both reach 0 */
            if (f4_lw(slot + 0x58) == 0 && f4_lw(slot + 0x5C) == 0) {
                f4_sh(slot + 0x20, 0);
            }
        }
    }

    /* Apply rotation: slot[+0x50] += counter1, slot[+0x54] -= counter2 */
    {
        s32 c1 = f4_lw(slot + 0x58);
        s32 c2 = f4_lw(slot + 0x5C);
        f4_sw(slot + 0x50, f4_lw(slot + 0x50) + c1);
        f4_sw(slot + 0x54, f4_lw(slot + 0x54) - c2);
    }

    /* Build rotation SVECTORs and call RotMatrixZYX twice */
    {
        SVECTOR rot1, rot2;

        /* First rotation: (0, slot[+0x50], 0) */
        rot1.vx = 0;
        rot1.vy = (s16)f4_lw(slot + 0x50);
        rot1.vz = 0;
        RotMatrixZYX_gte(&rot1, (MATRIX*)PSX_ADDR(scratch + 0xA0));

        /* Second rotation: (0, slot[+0x54], 0) */
        rot2.vx = 0;
        rot2.vy = (s16)f4_lw(slot + 0x54);
        rot2.vz = 0;
        RotMatrixZYX_gte(&rot2, (MATRIX*)PSX_ADDR(scratch + 0xA8));
    }

    return 1;
}
