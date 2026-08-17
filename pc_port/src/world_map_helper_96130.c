/*
 * World-map D788/C624 wait 0x80096130.
 *
 * Register-faithful transcription of retail world_map.bin
 * [0x80096130, 0x8009623C).  See world_map_helper_96130.h.
 */
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_96130.h"
#include "world_map_helper_967e4.h"

#define W130_INDEX  0x8009BE44u
#define W130_D788   0x8009D788u
#define W130_C624   0x8009C624u

u32 func_8002C3D8(void);
int Vsync(int mode);

static u32 w130_lw(u32 a)
{
    u32 v;

    memcpy(&v, PSX_ADDR(a), 4);
    return v;
}

static u32 w130_slot(u32 table)
{
    return w130_lw(table + w130_lw(W130_INDEX) * 4u);
}

void wm_80096130(void)
{
    u32 first;
    u32 second;
    u32 skip_c624;
    u32 table;

    first = func_8002C3D8();
    second = func_8002C3D8();
#if defined(WM_96130_MUTANT_OR_AND)
    skip_c624 = (first == 0u) && (second == 0xFFFFFFFFu);
#else
    skip_c624 = (first == 0u) || (second == 0xFFFFFFFFu);
#endif
    table = skip_c624 ? W130_D788 : W130_C624;
#if defined(WM_96130_MUTANT_WRONG_TABLE)
    table = skip_c624 ? W130_C624 : W130_D788;
#endif
#if defined(WM_96130_MUTANT_SKIP_EMPTY)
    if (0 && w130_slot(table) == 0u)
#else
    if (w130_slot(table) == 0u)
#endif
        return;

    do {
#if !defined(WM_96130_MUTANT_SKIP_VSYNC)
        (void)Vsync(0);
#endif
#if !defined(WM_96130_MUTANT_SKIP_STEP)
        (void)wm_800967E4();
#endif
    } while (w130_slot(table) != 0u);
}
