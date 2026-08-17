/*
 * World-map 16-byte bank publish 0x800965A4.
 *
 * Register-faithful transcription of retail world_map.bin
 * [0x800965A4, 0x80096668).  See world_map_helper_965a4.h.
 */
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_965a4.h"
#include "world_map_helper_964b0.h"

#define W5A4_INDEX  0x8009BE44u
#define W5A4_BASE   0x8009D3C0u
#define W5A4_TABLE  0x8009C624u
#define W5A4_COUNT  0x8009D808u
#define W5A4_BLOCK  0x580u

#if defined(WM_965A4_TEST_TRACE)
extern void wm_965a4_test_store(u32 address, u32 value);
#define W5A4_TRACE_STORE(a, v) wm_965a4_test_store((a), (v))
#else
#define W5A4_TRACE_STORE(a, v) ((void)0)
#endif

static u32 w5a4_lw(u32 a)
{
    u32 v;

    memcpy(&v, PSX_ADDR(a), 4);
    return v;
}

static void w5a4_sw(u32 a, u32 v)
{
    memcpy(PSX_ADDR(a), &v, 4);
    W5A4_TRACE_STORE(a, v);
}

s32 wm_800965A4(void)
{
    u32 idx;
    u32 base;
    u32 block;
    u32 list;
    u32 occ;
    u32 next;

    idx = w5a4_lw(W5A4_INDEX);
    base = w5a4_lw(W5A4_BASE);
#if defined(WM_965A4_MUTANT_WRONG_BLOCK)
    block = 0x500u;
#else
    block = W5A4_BLOCK;
#endif
    list = base + idx * block;

#if defined(WM_965A4_MUTANT_SKIP_EMPTY)
    if (0 && w5a4_lw(list) == 0u)
#else
    if (w5a4_lw(list) == 0u)
#endif
        goto fail;

    occ = w5a4_lw(W5A4_TABLE + idx * 4u);
#if defined(WM_965A4_MUTANT_SKIP_OCCUPIED)
    if (0 && occ != 0u)
#else
    if (occ != 0u)
#endif
        goto fail;

#if !defined(WM_965A4_MUTANT_SKIP_SORT)
    wm_800964B0(list);
#endif

    w5a4_sw(W5A4_COUNT, 0u);
#if defined(WM_965A4_MUTANT_WRONG_RING)
    next = idx + 1u;
#else
    next = (idx + 1u) & 0xfu;
#endif
#if !defined(WM_965A4_MUTANT_SKIP_PUBLISH)
    w5a4_sw(W5A4_TABLE + idx * 4u, list);
#endif
#if defined(WM_965A4_MUTANT_SKIP_INDEX)
    (void)next;
#else
    w5a4_sw(W5A4_INDEX, next);
#endif
    return 0;

fail:
#if !defined(WM_965A4_MUTANT_SKIP_CLEAR)
    w5a4_sw(W5A4_COUNT, 0u);
#endif
    return -1;
}
