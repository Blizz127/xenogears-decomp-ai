/*
 * World-map helper 0x8008E0F0 (angle search via wm_80095414).
 */
#include <string.h>
#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_8e0f0.h"
#include "world_map_func_95414.h"

extern long rcos(long a);
extern long rsin(long a);

s32 wm_8008E0F0(u32 pos, u32 unused, u32 out)
{
    s32 angle;
    for (angle = 0; angle < 0x1000; angle += 0x100) {
        /* Build direction vector from angle */
        long cos_val = rcos((long)angle);
        long sin_val = rsin((long)angle);

        /* Direction: (rcos, 0, -rsin) */
        u32 dir[3];
        dir[0] = (u32)cos_val;
        dir[1] = 0;
        dir[2] = (u32)(-(s32)sin_val);

        /* Test movement: mode=2 */
        s32 result = wm_80095414(pos, (u32)(uintptr_t)dir, out, 0, 2);
        if (result == 1) {
            return angle;
        }
    }
    return -1;
}
