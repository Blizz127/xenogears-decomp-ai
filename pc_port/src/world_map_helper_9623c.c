/*
 * World-map 12-byte queue push 0x8009623C.
 *
 * Register-faithful transcription of retail world_map.bin
 * [0x8009623C, 0x800962B0).  See world_map_helper_9623c.h.
 */
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_9623c.h"

#define W23C_COUNT  0x8009D808u
#define W23C_INDEX  0x8009BE44u
#define W23C_BASE   0x8009BE08u
#define W23C_LIMIT  88
#define W23C_BLOCK  0x420u
#define W23C_SLOT   12u

#if defined(WM_9623C_TEST_TRACE)
extern void wm_9623c_test_store(u32 address, u32 value);
#define W23C_TRACE_STORE(a, v) wm_9623c_test_store((a), (v))
#else
#define W23C_TRACE_STORE(a, v) ((void)0)
#endif

static u32 w23c_lw(u32 a)
{
    u32 v;

    memcpy(&v, PSX_ADDR(a), 4);
    return v;
}

static void w23c_sw(u32 a, u32 v)
{
    memcpy(PSX_ADDR(a), &v, 4);
    W23C_TRACE_STORE(a, v);
}

s32 wm_8009623C(u32 a0, u32 a1, u32 a2)
{
    s32 count;
    u32 dest;
    u32 idx;
    u32 block;
    u32 slot;
    s32 limit;

#if defined(WM_9623C_MUTANT_WRONG_LIMIT)
    limit = 80;
#else
    limit = W23C_LIMIT;
#endif

    count = (s32)w23c_lw(W23C_COUNT);
#if defined(WM_9623C_MUTANT_SKIP_FULL)
    if (0 && count >= limit)
#else
    if (count >= limit)
#endif
        return -1;

#if defined(WM_9623C_MUTANT_WRONG_BLOCK)
    block = 0x400u;
#else
    block = W23C_BLOCK;
#endif
#if defined(WM_9623C_MUTANT_WRONG_SLOT)
    slot = 8u;
#else
    slot = W23C_SLOT;
#endif
#if defined(WM_9623C_MUTANT_WRONG_BASE)
    dest = w23c_lw(W23C_INDEX);
#else
    dest = w23c_lw(W23C_BASE);
#endif
    idx = w23c_lw(W23C_INDEX);
#if !defined(WM_9623C_MUTANT_SKIP_INC)
    w23c_sw(W23C_COUNT, (u32)(count + 1));
#endif
    dest += idx * block + (u32)count * slot;
    w23c_sw(dest, a0);
    w23c_sw(dest + 4u, a1);
#if defined(WM_9623C_MUTANT_SKIP_A2)
    (void)a2;
#else
    w23c_sw(dest + 8u, a2);
#endif
    return 0;
}
