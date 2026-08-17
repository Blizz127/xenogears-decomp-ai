/*
 * World-map 12-byte bank publish 0x80096328.
 *
 * Register-faithful transcription of retail world_map.bin
 * [0x80096328, 0x800963E4).  See world_map_helper_96328.h.
 */
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_96328.h"
#include "world_map_helper_963e4.h"

#define W328_INDEX  0x8009BE44u
#define W328_BASE   0x8009BE08u
#define W328_TABLE  0x8009D788u
#define W328_COUNT  0x8009D808u
#define W328_BLOCK  0x420u

#if defined(WM_96328_TEST_TRACE)
extern void wm_96328_test_store(u32 address, u32 value);
#define W328_TRACE_STORE(a, v) wm_96328_test_store((a), (v))
#else
#define W328_TRACE_STORE(a, v) ((void)0)
#endif

static u32 w328_lw(u32 a)
{
    u32 v;

    memcpy(&v, PSX_ADDR(a), 4);
    return v;
}

static void w328_sw(u32 a, u32 v)
{
    memcpy(PSX_ADDR(a), &v, 4);
    W328_TRACE_STORE(a, v);
}

s32 wm_80096328(void)
{
    u32 idx;
    u32 base;
    u32 block;
    u32 list;
    u32 occ;
    u32 next;

    idx = w328_lw(W328_INDEX);
    base = w328_lw(W328_BASE);
#if defined(WM_96328_MUTANT_WRONG_BLOCK)
    block = 0x400u;
#else
    block = W328_BLOCK;
#endif
    list = base + idx * block;

#if defined(WM_96328_MUTANT_SKIP_EMPTY)
    if (0 && w328_lw(list) == 0u)
#else
    if (w328_lw(list) == 0u)
#endif
        goto fail;

    occ = w328_lw(W328_TABLE + idx * 4u);
#if defined(WM_96328_MUTANT_SKIP_OCCUPIED)
    if (0 && occ != 0u)
#else
    if (occ != 0u)
#endif
        goto fail;

#if !defined(WM_96328_MUTANT_SKIP_SORT)
    wm_800963E4(list);
#endif

    w328_sw(W328_COUNT, 0u);
#if defined(WM_96328_MUTANT_WRONG_RING)
    next = idx + 1u;
#else
    next = (idx + 1u) & 0xfu;
#endif
#if !defined(WM_96328_MUTANT_SKIP_PUBLISH)
    w328_sw(W328_TABLE + idx * 4u, list);
#endif
#if defined(WM_96328_MUTANT_SKIP_INDEX)
    (void)next;
#else
    w328_sw(W328_INDEX, next);
#endif
    return 0;

fail:
#if !defined(WM_96328_MUTANT_SKIP_CLEAR)
    w328_sw(W328_COUNT, 0u);
#endif
    return -1;
}
