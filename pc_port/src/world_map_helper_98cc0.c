/*
 * World-map bank fill 0x80098CC0.
 *
 * Register-faithful transcription of retail world_map.bin
 * [0x80098CC0, 0x8009932C).  See world_map_helper_98cc0.h.
 */
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_9623c.h"
#include "world_map_helper_962b0.h"
#include "world_map_helper_96328.h"
#include "world_map_helper_965a4.h"
#include "world_map_helper_98cc0.h"

#define WCC0_PTRS     0x8009C184u
#define WCC0_KEEP     0x8009D570u
#define WCC0_CHECK    0x8009D318u
#define WCC0_BBAC     0x8009BBACu
#define WCC0_ARC_A    0x8009BCD8u
#define WCC0_ARC_B    0x8009BD08u
#define WCC0_DIV      0x8009D160u
#define WCC0_MUL      0x8009D2B4u
#define WCC0_LIMIT    0x51
#define WCC0_SIZE     0x710
#define WCC0_INNER    6
#define WCC0_THIRD    4
#define WCC0_STRIDE1  2u
#define WCC0_STRIDE2  0x12u
#define WCC0_SHIFT    11
#define WCC0_FLAG2    2u
#define WCC0_FLAG1    1u

char *ArchiveGetFilePath(int entryIndex);
int ArchiveDecodeSector(int entryIndex);
u32 func_8002C3D8(void);
void *HeapAlloc(u_int allocSize, u_int allocFlags);
u_int HeapFree(void *pMem);

#if defined(WM_98CC0_TEST_TRACE)
extern void wm_98cc0_test_store(u32 address, u32 value);
#define WCC0_TRACE_STORE(a, v) wm_98cc0_test_store((a), (v))
#else
#define WCC0_TRACE_STORE(a, v) ((void)0)
#endif

static u32 wcc0_lw(u32 a)
{
    u32 v;

    memcpy(&v, PSX_ADDR(a), 4);
    return v;
}

static s32 wcc0_lh(u32 a)
{
    s16 h;

    memcpy(&h, PSX_ADDR(a), 2);
    return (s32)h;
}

static void wcc0_sw(u32 a, u32 v)
{
    memcpy(PSX_ADDR(a), &v, 4);
    WCC0_TRACE_STORE(a, v);
}

static u32 wcc0_host_to_psx(void *p)
{
    uintptr_t host;
    uintptr_t base;

    if (p == NULL)
        return 0u;
    host = (uintptr_t)p;
    base = (uintptr_t)g_PsxRam;
    if (host >= base && host < base + (uintptr_t)PSX_RAM_SIZE)
        return 0x80000000u | (u32)(host - base);
    return (u32)host;
}

static void *wcc0_psx_to_host(u32 psx)
{
    if (psx >= 0x80000000u && psx < 0x80200000u)
        return PSX_ADDR(psx);
    if (psx == 0u)
        return NULL;
    return (void *)(uintptr_t)psx;
}

static u32 wcc0_size(void)
{
#if defined(WM_98CC0_MUTANT_WRONG_SIZE)
    return 0x700u;
#else
    return (u32)WCC0_SIZE;
#endif
}

static s32 wcc0_limit(void)
{
#if defined(WM_98CC0_MUTANT_WRONG_COUNT)
    return 0x50;
#else
    return WCC0_LIMIT;
#endif
}

static u32 wcc0_shift(void)
{
#if defined(WM_98CC0_MUTANT_WRONG_SHIFT)
    return 10u;
#else
    return (u32)WCC0_SHIFT;
#endif
}

static u32 wcc0_walk_stride(u32 stride)
{
#if defined(WM_98CC0_MUTANT_WRONG_STRIDE)
    if (stride == WCC0_STRIDE2)
        return WCC0_STRIDE1;
#endif
    return stride;
}

static u32 wcc0_alloc(void)
{
    return wcc0_host_to_psx(HeapAlloc((u_int)wcc0_size(), 0u));
}

static void wcc0_div_parts(s32 dividend, s32 *quot, s32 *rem)
{
    s32 divisor;

    divisor = (s32)wcc0_lw(WCC0_DIV);
    if (divisor == 0)
        abort();
    if (divisor == -1 && dividend == (s32)0x80000000)
        abort();
    *quot = dividend / divisor;
    *rem = dividend % divisor;
}

static u32 wcc0_cd_off(s32 id)
{
    s32 quot;
    s32 rem;

    wcc0_div_parts(id, &quot, &rem);
    return (u32)rem * wcc0_lw(WCC0_MUL) + (u32)quot;
}

static u32 wcc0_pc_off(s32 id)
{
    s32 quot;
    s32 rem;
    u32 sh;

    sh = wcc0_shift();
    wcc0_div_parts(id, &quot, &rem);
    return ((u32)rem << sh) * wcc0_lw(WCC0_MUL) + ((u32)quot << sh);
}

static u32 wcc0_slot(s32 id)
{
    return WCC0_PTRS + (u32)id * 4u;
}

static void wcc0_or_flag(u32 *flags, u32 flag)
{
#if defined(WM_98CC0_MUTANT_SKIP_FLAG2)
    if (flag == WCC0_FLAG2)
        return;
#elif defined(WM_98CC0_MUTANT_SKIP_FLAG1)
    if (flag == WCC0_FLAG1)
        return;
#endif
    *flags |= flag;
}

static s32 wcc0_occupied(u32 ptr)
{
#if defined(WM_98CC0_MUTANT_SKIP_ALLOC_CHECK)
    (void)ptr;
    return 0;
#else
    return ptr != 0u;
#endif
}

static void wcc0_cleanup(void)
{
    s32 i;
    s32 id;
    s32 scan;
    s32 limit;
    u32 slot;
    u32 ptr;

    limit = wcc0_limit();
    for (i = 0; i < limit; i++) {
        id = wcc0_lh(WCC0_CHECK + (u32)i * 2u);
        slot = wcc0_slot(id);
        ptr = wcc0_lw(slot);
        if (ptr == 0u)
            continue;
        for (scan = 0; scan < limit; scan++) {
            if (wcc0_lh(WCC0_KEEP + (u32)scan * 2u) == id)
                break;
        }
        if (scan < limit)
            continue;
#if defined(WM_98CC0_MUTANT_SKIP_FREE)
        (void)wcc0_psx_to_host(ptr);
#else
        (void)HeapFree(wcc0_psx_to_host(ptr));
        wcc0_sw(slot, 0u);
#endif
    }
}

static u32 wcc0_use_cd(void)
{
    u32 first;
    u32 second;
    u32 use_cd;

    first = func_8002C3D8();
    second = func_8002C3D8();
#if defined(WM_98CC0_MUTANT_OR_AND)
    use_cd = (first == 0u) && (second == 0xFFFFFFFFu);
#else
    use_cd = (first == 0u) || (second == 0xFFFFFFFFu);
#endif
#if defined(WM_98CC0_MUTANT_SWAP_PATH)
    return use_cd ? 0u : 1u;
#else
    return use_cd;
#endif
}

static void wcc0_walk_cd(s32 start_a, s32 start_b, u32 stride, u32 flag,
                         u32 sector, u32 use_div, u32 *flags)
{
    s32 outer;
    s32 inner;
    s32 start;
    s32 id;
    u32 cursor;
    u32 slot;
    u32 ptr;

    stride = wcc0_walk_stride(stride);
    outer = 1;
    start = start_a;
    do {
        inner = WCC0_INNER;
        cursor = WCC0_KEEP + (u32)start * 2u;
        do {
            id = wcc0_lh(cursor);
            slot = wcc0_slot(id);
            ptr = wcc0_lw(slot);
            if (!wcc0_occupied(ptr)) {
                ptr = wcc0_alloc();
                wcc0_sw(slot, ptr);
                if (use_div != 0u)
                    (void)wm_8009623C(sector + wcc0_cd_off(id), wcc0_size(),
                                      ptr);
                else
                    (void)wm_8009623C(sector + (u32)id, wcc0_size(), ptr);
                wcc0_or_flag(flags, flag);
            }
            inner -= 1;
            cursor += stride;
        } while (inner != -1);
        outer -= 1;
        start = start_b;
    } while (outer != -1);
}

static void wcc0_walk_pc(s32 start_a, s32 start_b, u32 stride, u32 flag,
                         u32 path, u32 use_div, u32 *flags)
{
    s32 outer;
    s32 inner;
    s32 start;
    s32 id;
    u32 cursor;
    u32 slot;
    u32 ptr;

    stride = wcc0_walk_stride(stride);
    outer = 1;
    start = start_a;
    do {
        inner = WCC0_INNER;
        cursor = WCC0_KEEP + (u32)start * 2u;
        do {
            id = wcc0_lh(cursor);
            slot = wcc0_slot(id);
            ptr = wcc0_lw(slot);
            if (!wcc0_occupied(ptr)) {
                ptr = wcc0_alloc();
                wcc0_sw(slot, ptr);
                if (use_div != 0u)
                    (void)wm_800962B0(path, wcc0_pc_off(id), wcc0_size(), ptr);
                else
                    (void)wm_800962B0(path, (u32)id << wcc0_shift(),
                                      wcc0_size(), ptr);
                wcc0_or_flag(flags, flag);
            }
            inner -= 1;
            cursor += stride;
        } while (inner != -1);
        outer -= 1;
        start = start_b;
    } while (outer != -1);
}

static void wcc0_third_cd(u32 flags)
{
    s32 i;
    s32 keep_idx;
    s32 id;
    u32 slot;
    u32 ptr;
    u32 sector;
    u32 bit2;
    u32 bit1;

#if defined(WM_98CC0_MUTANT_SKIP_THIRD)
    (void)flags;
    return;
#endif
    bit2 = flags & WCC0_FLAG2;
    bit1 = flags & WCC0_FLAG1;
    for (i = 0; i < WCC0_THIRD; i++) {
        keep_idx = wcc0_lh(WCC0_BBAC + (u32)i * 2u);
        id = wcc0_lh(WCC0_KEEP + (u32)keep_idx * 2u);
        slot = wcc0_slot(id);
        ptr = wcc0_lw(slot);
        if (wcc0_occupied(ptr))
            continue;
        ptr = wcc0_alloc();
        wcc0_sw(slot, ptr);
        if (bit2 != 0u) {
            sector = (u32)ArchiveDecodeSector((int)wcc0_lw(WCC0_ARC_A));
            (void)wm_8009623C(sector + (u32)id, wcc0_size(), ptr);
            continue;
        }
        if (bit1 == 0u)
            continue;
        sector = (u32)ArchiveDecodeSector((int)wcc0_lw(WCC0_ARC_B));
        (void)wm_8009623C(sector + wcc0_cd_off(id), wcc0_size(), ptr);
    }
}

static void wcc0_third_pc(u32 flags)
{
    s32 i;
    s32 keep_idx;
    s32 id;
    u32 slot;
    u32 ptr;
    u32 path;
    u32 bit2;
    u32 bit1;

#if defined(WM_98CC0_MUTANT_SKIP_THIRD)
    (void)flags;
    return;
#endif
    bit2 = flags & WCC0_FLAG2;
    bit1 = flags & WCC0_FLAG1;
    for (i = 0; i < WCC0_THIRD; i++) {
        keep_idx = wcc0_lh(WCC0_BBAC + (u32)i * 2u);
        id = wcc0_lh(WCC0_KEEP + (u32)keep_idx * 2u);
        slot = wcc0_slot(id);
        ptr = wcc0_lw(slot);
        if (wcc0_occupied(ptr))
            continue;
        ptr = wcc0_alloc();
        wcc0_sw(slot, ptr);
        if (bit2 != 0u) {
            path = wcc0_host_to_psx(
                ArchiveGetFilePath((int)wcc0_lw(WCC0_ARC_A)));
            (void)wm_800962B0(path, (u32)id << wcc0_shift(), wcc0_size(),
                              ptr);
            continue;
        }
        if (bit1 == 0u)
            continue;
        path = wcc0_host_to_psx(ArchiveGetFilePath((int)wcc0_lw(WCC0_ARC_B)));
        (void)wm_800962B0(path, wcc0_pc_off(id), wcc0_size(), ptr);
    }
}

static void wcc0_cd_path(void)
{
    u32 flags;
    u32 sector;

    flags = 0u;
#if !defined(WM_98CC0_MUTANT_SKIP_CD_GROUP1)
    sector = (u32)ArchiveDecodeSector((int)wcc0_lw(WCC0_ARC_A));
    wcc0_walk_cd(1, 0x49, WCC0_STRIDE1, WCC0_FLAG2, sector, 0u, &flags);
#endif
#if !defined(WM_98CC0_MUTANT_SKIP_CD_GROUP2)
    sector = (u32)ArchiveDecodeSector((int)wcc0_lw(WCC0_ARC_B));
    wcc0_walk_cd(9, 0x11, WCC0_STRIDE2, WCC0_FLAG1, sector, 1u, &flags);
#endif
    wcc0_third_cd(flags);
#if !defined(WM_98CC0_MUTANT_SKIP_TERM_CD)
    (void)wm_8009623C(0u, 0u, 0u);
    (void)wm_80096328();
#endif
}

static void wcc0_pc_path(void)
{
    u32 flags;
    u32 path;

    flags = 0u;
#if !defined(WM_98CC0_MUTANT_SKIP_CD_GROUP1)
    path = wcc0_host_to_psx(ArchiveGetFilePath((int)wcc0_lw(WCC0_ARC_A)));
    wcc0_walk_pc(1, 0x49, WCC0_STRIDE1, WCC0_FLAG2, path, 0u, &flags);
#endif
#if !defined(WM_98CC0_MUTANT_SKIP_CD_GROUP2)
    path = wcc0_host_to_psx(ArchiveGetFilePath((int)wcc0_lw(WCC0_ARC_B)));
    wcc0_walk_pc(9, 0x11, WCC0_STRIDE2, WCC0_FLAG1, path, 1u, &flags);
#endif
    wcc0_third_pc(flags);
#if !defined(WM_98CC0_MUTANT_SKIP_TERM_PC)
    (void)wm_800962B0(0u, 0u, 0u, 0u);
    (void)wm_800965A4();
#endif
}

void wm_80098CC0(void)
{
    wcc0_cleanup();
    if (wcc0_use_cd() != 0u)
        wcc0_cd_path();
    else
        wcc0_pc_path();
}
