/*
 * World-map helper 0x800965A4 (queue record allocator variant B).
 */
#include <stdint.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_965a4.h"
#include "world_map_helper_9623c.h"

#define D_8009BE44  0x8009BE44u
#define D_8009D3C0  0x8009D3C0u
#define D_8009D808  0x8009D808u
#define D_8009C624  0x8009C624u

static u32 a5a4_lw(u32 a) { u32 v; memcpy(&v, PSX_ADDR(a), 4); return v; }
static void a5a4_sw(u32 a, u32 v) { memcpy(PSX_ADDR(a), &v, 4); }

s32 wm_800965A4(void)
{
    u32 idx = a5a4_lw(D_8009BE44);
    u32 base = a5a4_lw(D_8009D3C0);

    /* Stride: (idx*3*4 - idx) << 7 = idx * 11 * 128 = idx * 1408 */
    u32 stride = ((idx * 12 - idx) << 7);
    u32 record = base + stride;

    /* Check if record already in use */
    if (a5a4_lw(record) != 0) {
        if (a5a4_lw(D_8009C624 + idx * 4) != 0) {
            a5a4_sw(D_8009D808, 0);
            return -1;
        }
    }

    /* Copy record (40 bytes via wm_800964B0) */
    wm_800964B0(record, record); /* self-copy = initialize */

    /* Reset queue counter, store pointer, advance index (mod 16) */
    a5a4_sw(D_8009D808, 0);
    a5a4_sw(D_8009C624 + idx * 4, record);
    a5a4_sw(D_8009BE44, (idx + 1) & 0xF);

    return 0;
}
