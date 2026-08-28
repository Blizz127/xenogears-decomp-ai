/*
 * World-map helper 0x80095CD4 (collision-aware movement probe).
 */
#include <string.h>
#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_95cd4.h"
#include "world_map_helper_84d00.h"
#include "world_map_helper_85418.h"
#include "world_map_helper_93354.h"
#include "world_map_helper_951a8.h"
#include "world_map_terrain_sampler.h"

#define SCRATCH 0x1F800000u
#define Y_MIN   0xFFD80000u  /* -0x280000 */
#define Y_MAX   0x00020000u  /*  0x20000 */

/* Dead retail-stack region, following WM_95414_FRAME_ATTR. */
#define WM_95CD4_FRAME_ATTR 0x801FFDE0u

static u32 cd_lw(u32 a) { u32 v; memcpy(&v, PSX_ADDR(a), 4); return v; }
static void cd_sw(u32 a, u32 v) { memcpy(PSX_ADDR(a), &v, 4); }
static u16 cd_lhu(u32 a) { u16 v; memcpy(&v, PSX_ADDR(a), 2); return v; }
static void cd_sh(u32 a, u16 v) { memcpy(PSX_ADDR(a), &v, 2); }

s32 wm_80095CD4(u32 pos, u32 vel, u32 out, s32 scale)
{
    s32 terrain_y, i, collision_count;
    u32 scratch_target = SCRATCH + 0x60;
    u16 attr_buf[8]; /* stack buffer for nav-mesh attributes */

    /* Compute target = pos + vel * scale >> 12 */
    for (i = 0; i < 3; i++) {
        s32 v = (s32)cd_lw(vel + i * 4);
        s32 p = (s32)cd_lw(pos + i * 4);
        s32 delta = (v * scale) >> 12;
        cd_sw(scratch_target + i * 4, (u32)(p + delta));
    }

    /* Clamp target Y via wm_80093354 */
    wm_80093354(scratch_target);

    /* Compute out = pos + vel * scale >> 12 (same again) */
    for (i = 0; i < 3; i++) {
        s32 v = (s32)cd_lw(vel + i * 4);
        s32 p = (s32)cd_lw(pos + i * 4);
        s32 delta = (v * scale) >> 12;
        cd_sw(out + i * 4, (u32)(p + delta));
    }

    /* Clamp out Y via wm_80093354 */
    wm_80093354(out);

    /* Sample terrain height at target X/Z */
    terrain_y = wm_80093978(
        (s32)cd_lw(scratch_target),
        (s32)cd_lw(scratch_target + 8));

    /* Clamp terrain Y */
    {
        u32 ty = (u32)terrain_y;
        u32 ty_adj = ty + 0xFFFE0000u; /* ty - 0x20000 */
        if (ty_adj > 0x00040000u) {    /* outside [-0x20000, 0x20000] */
            ty = 0x00020000u;
        }
        /* Below-terrain check */
        if ((s32)ty < (s32)Y_MIN || (s32)ty < (s32)cd_lw(scratch_target + 4)) {
            cd_sw(scratch_target + 4, ty);
            cd_sw(out + 4, ty);
        }
    }

    /* Nav-mesh probe */
    {
        s32 count;

        /* F3 repair - W34C17R/W34C18.  wm_80084D00 conditionally writes
         * attr_buf; preserve the stack image on entry and copy it back only
         * when the helper reports the write-producing success path. */
        memcpy(PSX_ADDR(WM_95CD4_FRAME_ATTR), attr_buf, sizeof(attr_buf));
        count = wm_80084D00(scratch_target, WM_95CD4_FRAME_ATTR);
        if (count != 0)
            memcpy(attr_buf, PSX_ADDR(WM_95CD4_FRAME_ATTR), sizeof(attr_buf));
        if (count == 0) {
            goto resolve;
        }

        /* Check each attribute */
        collision_count = 0;
        for (i = 0; i < count; i += 2) {
            s32 result = wm_80085418(scratch_target, 0x70,
                                     (u32)attr_buf[i / 2],
                                     (u32)cd_lhu(SCRATCH + 0xD718 + i));
            if (result == 0) {
                attr_buf[i / 2] = 0xFFFF;
                collision_count += 2;
            }
        }

        /* If all collided, reflect velocity */
        if (collision_count == count) {
            cd_sw(out + 0, (u32)(-((s32)cd_lw(vel + 0)) >> 1));
            cd_sw(out + 4, (u32)(-((s32)cd_lw(vel + 4)) >> 1));
            cd_sw(out + 8, (u32)(-((s32)cd_lw(vel + 8)) >> 1));
            return 0;
        }
    }

resolve:
    return wm_800951A8(pos, vel, out, scale, (s32)cd_lw(SCRATCH + 0x60));
}
