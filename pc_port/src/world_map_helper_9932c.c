/*
 * World-map helper 0x8009932C (rendering context setup).
 */
#include <stdint.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "psyq/libgte.h"
#include "world_map_helper_9932c.h"
#include "world_map_helper_99708.h"

#define SCRATCH      0x1F800000u
#define D_8009CCB4   0x8009CCB4u  /* terrain data source (128 bytes) */
#define D_8009CD54   0x8009CD54u  /* secondary data (14 bytes) */
#define D_8009D534   0x8009D534u  /* matrix data (32 bytes) */
#define D_8009C808   0x8009C808u  /* camera matrix source */

static u16 r_lhu(u32 a) { u16 v; memcpy(&v, PSX_ADDR(a), 2); return v; }
static void r_sh(u32 a, u16 v) { memcpy(PSX_ADDR(a), &v, 2); }
static void r_sw(u32 a, s32 v) { memcpy(PSX_ADDR(a), &v, 4); }

void wm_8009932C(u32 camera_data, u32 terrain_data, u32 output_data)
{
    s32 i;

    /* Copy 64 halfwords from D_8009CCB4 to scratchpad+0x288 */
    for (i = 0; i < 64; i++) {
        r_sh(SCRATCH + 0x288 + i * 2, r_lhu(D_8009CCB4 + i * 2));
    }

    /* Copy 7 halfwords from D_8009CD54 to scratchpad+0x308 */
    for (i = 0; i < 7; i++) {
        r_sh(SCRATCH + 0x308 + i * 2, r_lhu(D_8009CD54 + i * 2));
    }

    /* Copy 32 bytes from D_8009D534 to scratchpad+0x350 */
    {
        u8* src = (u8*)PSX_ADDR(D_8009D534);
        u8* dst = (u8*)PSX_ADDR(SCRATCH + 0x350);
        memcpy(dst, src, 32);
    }

    /* CompMatrix: camera × rotation → scratch+0x370 */
    CompMatrix((MATRIX*)PSX_ADDR(D_8009C808),
               (MATRIX*)PSX_ADDR(SCRATCH + 0x350),
               (MATRIX*)PSX_ADDR(SCRATCH + 0x370));

    /* SetRotMatrix + SetTransMatrix */
    SetRotMatrix((MATRIX*)PSX_ADDR(SCRATCH + 0x370));
    SetTransMatrix((MATRIX*)PSX_ADDR(SCRATCH + 0x370));

    /* Process tiles */
    wm_80099708(terrain_data);
}
