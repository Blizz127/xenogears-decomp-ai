/*
 * World-map 16-byte queue push 0x800962B0.
 *
 * Register-faithful transcription of retail world_map.bin
 * [0x800962B0, 0x80096328).  See world_map_helper_962b0.h.
 */
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_962b0.h"

#define W2B0_COUNT  0x8009D808u
#define W2B0_INDEX  0x8009BE44u
#define W2B0_BASE   0x8009D3C0u
#define W2B0_LIMIT  88
#define W2B0_BLOCK  0x580u
#define W2B0_SLOT   16u

#if defined(WM_962B0_TEST_TRACE)
extern void wm_962b0_test_store(u32 address, u32 value);
#define W2B0_TRACE_STORE(a, v) wm_962b0_test_store((a), (v))
#else
#define W2B0_TRACE_STORE(a, v) ((void)0)
#endif

static u32 w2b0_lw(u32 a)
{
    u32 v;

    memcpy(&v, PSX_ADDR(a), 4);
    return v;
}

static void w2b0_sw(u32 a, u32 v)
{
    memcpy(PSX_ADDR(a), &v, 4);
    W2B0_TRACE_STORE(a, v);
}

s32 wm_800962B0(u32 a0, u32 a1, u32 a2, u32 a3)
{
    s32 count;
    u32 dest;
    u32 idx;
    u32 block;
    u32 slot;
    s32 limit;

#if defined(WM_962B0_MUTANT_WRONG_LIMIT)
    limit = 80;
#else
    limit = W2B0_LIMIT;
#endif

    count = (s32)w2b0_lw(W2B0_COUNT);
#if defined(WM_962B0_MUTANT_SKIP_FULL)
    if (0 && count >= limit)
#else
    if (count >= limit)
#endif
        return -1;

#if defined(WM_962B0_MUTANT_WRONG_BLOCK)
    block = 0x400u;
#else
    block = W2B0_BLOCK;
#endif
#if defined(WM_962B0_MUTANT_WRONG_SLOT)
    slot = 12u;
#else
    slot = W2B0_SLOT;
#endif
#if defined(WM_962B0_MUTANT_WRONG_BASE)
    dest = w2b0_lw(W2B0_INDEX);
#else
    dest = w2b0_lw(W2B0_BASE);
#endif
    idx = w2b0_lw(W2B0_INDEX);
#if !defined(WM_962B0_MUTANT_SKIP_INC)
    w2b0_sw(W2B0_COUNT, (u32)(count + 1));
#endif
    dest += idx * block + (u32)count * slot;
    w2b0_sw(dest, a0);
    w2b0_sw(dest + 4u, a1);
    w2b0_sw(dest + 8u, a2);
#if defined(WM_962B0_MUTANT_SKIP_A3)
    (void)a3;
#else
    w2b0_sw(dest + 0xCu, a3);
#endif
    return 0;
}
