/*
 * World-map CD/PC work dispatch 0x800967E4.
 *
 * Register-faithful transcription of retail world_map.bin
 * [0x800967E4, 0x800968E0).  See world_map_helper_967e4.h.
 */
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_966cc.h"
#include "world_map_helper_967e4.h"
#include "world_map_helper_968e0.h"
#include "world_map_helper_9699c.h"

#define W7E4_RING   0x8009BCB8u
#define W7E4_D788   0x8009D788u
#define W7E4_C624   0x8009C624u

u32 func_8002C3D8(void);

#if defined(WM_967E4_TEST_TRACE)
extern void wm_967e4_test_store(u32 address, u32 value);
#define W7E4_TRACE_STORE(a, v) wm_967e4_test_store((a), (v))
#else
#define W7E4_TRACE_STORE(a, v) ((void)0)
#endif

static u32 w7e4_lw(u32 a)
{
    u32 v;

    memcpy(&v, PSX_ADDR(a), 4);
    return v;
}

static void __attribute__((unused)) w7e4_sw(u32 a, u32 v)
{
    memcpy(PSX_ADDR(a), &v, 4);
    W7E4_TRACE_STORE(a, v);
}

s32 wm_800967E4(void)
{
    u32 first;
    u32 second;
    u32 skip_c624;
    s32 s1;
    u32 ring;
    u32 rec;

    s1 = 0;
    first = func_8002C3D8();
    second = func_8002C3D8();
#if defined(WM_967E4_MUTANT_OR_AND)
    skip_c624 = (first == 0u) && (second == 0xFFFFFFFFu);
#else
    skip_c624 = (first == 0u) || (second == 0xFFFFFFFFu);
#endif
    if (skip_c624) {
        s1 = wm_800968E0();
        if (s1 != 0)
            return s1;
        ring = w7e4_lw(W7E4_RING);
        rec = w7e4_lw(W7E4_D788 + ring * 4u);
        if (rec != 0u) {
#if !defined(WM_967E4_MUTANT_SKIP_D788)
            wm_8009699C(rec);
#endif
        }
        return s1;
    }

    ring = w7e4_lw(W7E4_RING);
    rec = w7e4_lw(W7E4_C624 + ring * 4u);
    if (rec != 0u) {
        wm_800966CC(rec);
#if !defined(WM_967E4_MUTANT_SKIP_ADVANCE)
        w7e4_sw(W7E4_C624 + ring * 4u, 0u);
        w7e4_sw(W7E4_RING, (ring + 1u) & 0xfu);
#endif
    }
    return s1;
}
