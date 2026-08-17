/*
 * World-map helper 0x80085CDC (object position updater).
 */
#include <stdint.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "psyq/libgte.h"
#include "world_map_helper_85cdc.h"
#include "world_map_helper_93484.h"

#define D_8009BE24  0x8009BE24u  /* pool pointer */
#define D_8009BE28  0x8009BE28u  /* camera position */
#define D_8009C808  0x8009C808u  /* camera matrix */
#define SCRATCH     0x1F800000u
#define OBJ_COUNT   64
#define OBJ_STRIDE  0x80

static s16 o_lh(u32 a) { s16 v; memcpy(&v, PSX_ADDR(a), 2); return v; }
static s32 o_lw(u32 a) { s32 v; memcpy(&v, PSX_ADDR(a), 4); return v; }
static void o_sh(u32 a, u16 v) { memcpy(PSX_ADDR(a), &v, 2); }
static void o_sw(u32 a, s32 v) { memcpy(PSX_ADDR(a), &v, 4); }

void wm_80085CDC(void)
{
    s32 i;
    u32 pool = o_lw(D_8009BE24);
    u32 entry = pool + 0x2C;

    /* Phase 1: Compute camera-relative deltas for 64 objects */
    for (i = 0; i < OBJ_COUNT; i++) {
        s16 state = o_lh(entry - 0x28);
        u32 transform_ptr = (u32)o_lw(entry + 0x20);

        if (state == 0 && transform_ptr != 0) {
            /* Compute delta: object pos - camera pos */
            s32 dx = o_lw(entry - 4) - o_lw(D_8009BE28);
            s32 dz = o_lw(entry + 4) - o_lw(D_8009BE28 + 8);

            /* Write delta to scratchpad */
            o_sw(SCRATCH + 8, dx);
            o_sw(SCRATCH + 0x10, dz);

            /* Wrap delta */
            wm_80093484(SCRATCH + 8);

            /* Write scaled delta to transform */
            o_sw(transform_ptr + 0, o_lw(SCRATCH + 8) << 4);
            o_sw(transform_ptr + 8, (s32)(-(s32)o_lw(SCRATCH + 0x10)) << 4);
            o_sw(transform_ptr + 4, o_lw(entry) << 4);
        }

        entry += OBJ_STRIDE;
    }

    /* Phase 2: Set GTE matrices */
    {
        MATRIX* cam = (MATRIX*)PSX_ADDR(D_8009C808);
        SetRotMatrix(cam);
        SetTransMatrix(cam);
    }

    /* Phase 3: Load vertex data into COP2 for rendering */
    entry = pool + 0x4C;
    for (i = 0; i < OBJ_COUNT; i++) {
        s16 state = o_lh(entry - 0x28);
        u32 obj_ptr = o_lw(entry);

        if (state == 0 && obj_ptr != 0) {
            /* Load 3 halfwords into COP2 registers 0,1 */
            s16 vx = o_lh(obj_ptr + 2);
            s16 vy = o_lh(obj_ptr + 6);
            s16 vz = o_lh(obj_ptr + 0x0A);
            o_sh(SCRATCH, (u16)vx);
            o_sh(SCRATCH + 2, (u16)vy);
            o_sh(SCRATCH + 4, (u16)vz);
            /* lwc2 $0, scratch; lwc2 $1, scratch+4 — handled by GTE */
        }

        entry += OBJ_STRIDE;
    }
}
