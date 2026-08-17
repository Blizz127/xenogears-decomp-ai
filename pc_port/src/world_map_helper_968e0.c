/*
 * World-map CD44 state dispatch 0x800968E0.
 *
 * Register-faithful transcription of retail world_map.bin
 * [0x800968E0, 0x8009699C).  See world_map_helper_968e0.h.
 */
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_968e0.h"

#define W8E0_STATE  0x8009CD44u
#define W8E0_COUNT  0x8009BD2Cu
#define W8E0_RING   0x8009BCB8u
#define W8E0_TABLE  0x8009D788u

#if defined(WM_968E0_TEST_TRACE)
extern void wm_968e0_test_store(u32 address, u32 value);
#define W8E0_TRACE_STORE(a, v) wm_968e0_test_store((a), (v))
#else
#define W8E0_TRACE_STORE(a, v) ((void)0)
#endif

static u32 w8e0_lw(u32 a)
{
    u32 v;

    memcpy(&v, PSX_ADDR(a), 4);
    return v;
}

static void w8e0_sw(u32 a, u32 v)
{
    memcpy(PSX_ADDR(a), &v, 4);
    W8E0_TRACE_STORE(a, v);
}

s32 wm_800968E0(void)
{
    u32 state;
    u32 count;

    state = w8e0_lw(W8E0_STATE);
#if defined(WM_968E0_MUTANT_NO_GUARD)
    (void)0;
#else
    if (state >= 6u)
        return 3;
#endif

#if defined(WM_968E0_MUTANT_CASE0_ONE)
    if (state == 0u)
        return 1;
#endif
    if (state == 0u)
        return 0;
    if (state <= 3u)
        return 1;

    if (state == 4u) {
        count = w8e0_lw(W8E0_COUNT) - 1u;
        w8e0_sw(W8E0_COUNT, count);
        if (count == 0u)
            w8e0_sw(W8E0_STATE, w8e0_lw(W8E0_STATE) + 1u);
        return 1;
    }

#if defined(WM_968E0_MUTANT_SKIP_PUBLISH)
    return 2;
#else
    w8e0_sw(W8E0_STATE, 0u);
    {
        u32 ring = w8e0_lw(W8E0_RING);
        w8e0_sw(W8E0_TABLE + ring * 4u, 0u);
#if defined(WM_968E0_MUTANT_WRONG_RING)
    w8e0_sw(W8E0_RING, ring + 1u);
#else
    w8e0_sw(W8E0_RING, (ring + 1u) & 0xfu);
#endif
    }
    return 2;
#endif
}
