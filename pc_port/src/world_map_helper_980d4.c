/*
 * World-map X/Z wrap-and-cell helper 0x800980D4.
 *
 * Register-faithful transcription of retail world_map.bin
 * [0x800980D4, 0x800981C8).  See world_map_helper_980d4.h.
 */
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_980d4.h"

#define W0D4_FLAGS   0x8009D558u
#define W0D4_CELL_X  0x8009C838u
#define W0D4_CELL_Z  0x8009C83Cu
#define W0D4_SPAN    0x00800000u
#define W0D4_LO      ((s32)0xFF800000)

static u32 w0d4_lw(u32 a)
{
    u32 v;

    memcpy(&v, PSX_ADDR(a), 4);
    return v;
}

static u32 w0d4_lhu(u32 a)
{
    u16 h;

    memcpy(&h, PSX_ADDR(a), 2);
    return (u32)h;
}

static void w0d4_sh(u32 a, u32 v)
{
    u16 h = (u16)(v & 0xFFFFu);

    memcpy(PSX_ADDR(a), &h, 2);
}

static void w0d4_sw(u32 a, u32 v)
{
    memcpy(PSX_ADDR(a), &v, 4);
}

void wm_800980D4(u32 pos_addr)
{
    s32 x = (s32)w0d4_lw(pos_addr);
    s32 z = (s32)w0d4_lw(pos_addr + 8u);

#if defined(WM_980D4_MUTANT_SKIP_CLEAR)
    (void)0;
#else
    w0d4_sh(W0D4_FLAGS, 0u);
#endif

#if defined(WM_980D4_MUTANT_WRONG_LIMIT)
    if (x < (s32)0xFFC00000)
#else
    if (x < W0D4_LO)
#endif
    {
#if !defined(WM_980D4_MUTANT_SKIP_X_LOW)
        w0d4_sw(pos_addr, (u32)(x + (s32)W0D4_SPAN));
#if defined(WM_980D4_MUTANT_WRONG_X_FLAG)
        w0d4_sh(W0D4_FLAGS, 1u);
#else
        w0d4_sh(W0D4_FLAGS, 4u);
#endif
#endif
    } else if ((s32)W0D4_SPAN < x) {
#if !defined(WM_980D4_MUTANT_SKIP_X_HIGH)
        w0d4_sw(pos_addr, (u32)(x - (s32)W0D4_SPAN));
        w0d4_sh(W0D4_FLAGS, 8u);
#endif
    }

    if (z < W0D4_LO) {
#if !defined(WM_980D4_MUTANT_SKIP_Z_LOW)
        u32 flags = w0d4_lhu(W0D4_FLAGS);

        w0d4_sw(pos_addr + 8u, (u32)((s32)w0d4_lw(pos_addr + 8u) + (s32)W0D4_SPAN));
#if defined(WM_980D4_MUTANT_WRONG_Z_OR)
        w0d4_sh(W0D4_FLAGS, flags | 4u);
#else
        w0d4_sh(W0D4_FLAGS, flags | 1u);
#endif
#endif
    } else if ((s32)W0D4_SPAN < z) {
#if !defined(WM_980D4_MUTANT_SKIP_Z_HIGH)
        u32 flags = w0d4_lhu(W0D4_FLAGS);

        w0d4_sw(pos_addr + 8u, (u32)((s32)w0d4_lw(pos_addr + 8u) - (s32)W0D4_SPAN));
        w0d4_sh(W0D4_FLAGS, flags | 2u);
#endif
    }

#if !defined(WM_980D4_MUTANT_SKIP_CELL)
    {
        s32 cx;
        s32 cz;

#if defined(WM_980D4_MUTANT_WRONG_SRA)
        cx = ((s32)w0d4_lw(pos_addr) >> 22) + 2;
        cz = ((s32)w0d4_lw(pos_addr + 8u) >> 22) + 2;
#elif defined(WM_980D4_MUTANT_SKIP_PLUS2)
        cx = (s32)w0d4_lw(pos_addr) >> 23;
        cz = (s32)w0d4_lw(pos_addr + 8u) >> 23;
#else
        cx = ((s32)w0d4_lw(pos_addr) >> 23) + 2;
        cz = ((s32)w0d4_lw(pos_addr + 8u) >> 23) + 2;
#endif
        w0d4_sh(W0D4_CELL_X, (u32)cx);
        w0d4_sh(W0D4_CELL_Z, (u32)cz);
    }
#endif
}
