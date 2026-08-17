/*
 * World-map slot-10 height-probe helper 0x80091FF8.
 *
 * Register-faithful transcription of retail world_map.bin
 * [0x80091FF8, 0x80092234).  See world_map_helper_91ff8.h.
 */
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_91ff8.h"
#include "world_map_helper_93354.h"
#include "world_map_helper_96f18.h"
#include "world_map_terrain_cell.h"

#define W8_BD3A   0x8009BD3Au
#define W8_BD3C   0x8009BD3Cu
#define W8_BD40   0x8009BD40u
#define W8_BD44   0x8009BD44u
#define W8_BE28   0x8009BE28u
#define W8_BE30   0x8009BE30u
#define W8_BCDC   0x8009BCDCu
#define W8_B214   0x8009B214u
#define W8_B234   0x8009B234u
#define W8_D560   0x8009D560u
#define W8_SCR    0x1F800000u
#define W8_ANG    0x1F8000A8u

#if defined(WM_91FF8_TEST_TRACE)
extern void wm_91ff8_test_96f18(u32 dest, u32 pose, s32 scale, u32 angles);
extern void wm_91ff8_test_93354(u32 vec);
extern u32 wm_91ff8_test_93660(s32 x, s32 z);
#define W8_CALL_96F18(d, p, s, a) wm_91ff8_test_96f18((d), (p), (s), (a))
#define W8_CALL_93354(v)          wm_91ff8_test_93354(v)
#define W8_CALL_93660(x, z)       wm_91ff8_test_93660((x), (z))
#else
#define W8_CALL_96F18(d, p, s, a) wm_80096F18((d), (p), (s), (a))
#define W8_CALL_93354(v)          wm_80093354(v)
#define W8_CALL_93660(x, z)       wm_80093660((x), (z))
#endif

static u32 w8_lw(u32 a)
{
    u32 v;

    memcpy(&v, PSX_ADDR(a), 4);
    return v;
}

static u32 w8_lhu(u32 a)
{
    u16 h;

    memcpy(&h, PSX_ADDR(a), 2);
    return (u32)h;
}

static s32 w8_lh(u32 a)
{
    s16 h;

    memcpy(&h, PSX_ADDR(a), 2);
    return (s32)h;
}

static s32 w8_lb(u32 a)
{
    s8 b;

    memcpy(&b, PSX_ADDR(a), 1);
    return (s32)b;
}

static void w8_sh(u32 a, u32 v)
{
    u16 h = (u16)(v & 0xFFFFu);

    memcpy(PSX_ADDR(a), &h, 2);
}

static void w8_sw(u32 a, u32 v)
{
    memcpy(PSX_ADDR(a), &v, 4);
}

static s32 w8_s32(u32 v)
{
    s32 r;

    memcpy(&r, &v, 4);
    return r;
}

static u32 w8_bits(s32 v)
{
    u32 r;

    memcpy(&r, &v, 4);
    return r;
}

static s32 w8_sll(s32 v, u32 sh)
{
    return w8_s32(w8_bits(v) << sh);
}

s32 wm_80091FF8(s32 threshold, u32 headings_addr, u32 heights_addr)
{
    s32 d560s = w8_s32(w8_lw(W8_D560)) >> 12;
    s32 s5 = 0;
    u32 s6 = W8_B214;
    u32 s7 = headings_addr;
    u32 fp = heights_addr;
    s32 min8 = 0;
    s32 s4;
    s32 s0;
    s32 s2;
    s32 s3;
    s32 z0;
    u32 bcdc;
    s32 scale;
    s32 dvx;
    s32 dvz;

#if defined(WM_91FF8_MUTANT_SKIP_93660)
    if (0)
        (void)w8_lb(0);
#endif
#if !defined(WM_91FF8_MUTANT_SKIP_HEADING)
    w8_sh(W8_ANG + 2u, w8_lhu(W8_BD3A));
    w8_sh(W8_ANG + 4u, w8_lhu(W8_BD3C));
#endif

    for (;;) {
#if defined(WM_91FF8_MUTANT_MIN_START_NEG)
        s4 = -1;
#else
        s4 = 0;
#endif
        w8_sh(W8_ANG, w8_lhu(s7));
        bcdc = w8_lw(W8_BCDC);
#if defined(WM_91FF8_MUTANT_SKIP_TOWARD_ZERO)
        scale = w8_s32(bcdc) >> 1;
#else
        scale = w8_s32(bcdc + (bcdc >> 31)) >> 1;
#endif
        scale = w8_sll(scale, 12u);
        scale = w8_s32(w8_lw(s6)) - scale;

#if !defined(WM_91FF8_MUTANT_SKIP_96F18)
#if defined(WM_91FF8_MUTANT_WRONG_96F18_DEST)
        W8_CALL_96F18(W8_BE28, W8_BE28, scale, W8_ANG);
#else
        W8_CALL_96F18(W8_BD40, W8_BE28, scale, W8_ANG);
#endif
#endif

        dvx = w8_lh(W8_BD40);
        dvz = w8_lh(W8_BD44);
        s3 = w8_s32(w8_lw(W8_BE28)) + w8_sll(dvx - 0x180, 12u);
        z0 = w8_s32(w8_lw(W8_BE30)) - w8_sll(dvz + 0x180, 12u);
#if !defined(WM_91FF8_MUTANT_SKIP_MASK)
        s3 = w8_s32((u32)s3 & 0xFFF80000u);
        z0 = w8_s32((u32)z0 & 0xFFF80000u);
#endif
        w8_sw(W8_SCR + 8u, (u32)z0);
        w8_sw(W8_SCR, (u32)s3);

#if defined(WM_91FF8_MUTANT_GRID_5)
#define W8_GRID 5
#else
#define W8_GRID 6
#endif
        for (s2 = 0; s2 < W8_GRID; s2++) {
            w8_sw(W8_SCR, (u32)s3);
            for (s0 = 0; s0 < W8_GRID; s0++) {
#if !defined(WM_91FF8_MUTANT_SKIP_93354)
                W8_CALL_93354(W8_SCR);
#endif
#if !defined(WM_91FF8_MUTANT_SKIP_93660)
                {
                    u32 cell = W8_CALL_93660(w8_s32(w8_lw(W8_SCR)),
                                             w8_s32(w8_lw(W8_SCR + 8u)));
                    s32 h = w8_lb(cell);

                    w8_sw(W8_SCR + 4u, (u32)h);
                    if (h < s4)
                        s4 = h;
                }
#endif
#if defined(WM_91FF8_MUTANT_WRONG_STEP)
                w8_sw(W8_SCR, w8_lw(W8_SCR) + 0x40000u);
#else
                w8_sw(W8_SCR, w8_lw(W8_SCR) + 0x80000u);
#endif
            }
#if defined(WM_91FF8_MUTANT_WRONG_STEP)
            w8_sw(W8_SCR + 8u, w8_lw(W8_SCR + 8u) + 0x40000u);
#else
            w8_sw(W8_SCR + 8u, w8_lw(W8_SCR + 8u) + 0x80000u);
#endif
        }
#undef W8_GRID
        min8 = w8_sll(s4, 3u);
        if ((w8_lh(fp) + d560s + 0x50) < min8)
            break;
        s5++;
        s7 += 2u;
        s6 += 4u;
        fp += 2u;
#if defined(WM_91FF8_MUTANT_OUTER_2)
        if (s5 >= 2)
            break;
#else
        if (s5 >= 3)
            break;
#endif
    }

#if defined(WM_91FF8_MUTANT_WRONG_RETURN)
    (void)threshold;
    return 0;
#else
    if (s5 < threshold) {
        s32 t = w8_lh(W8_B234 + (u32)threshold * 2u) + d560s - (min8 - 0x50);

        if (t < 0)
            t = -t;
#if defined(WM_91FF8_MUTANT_WRONG_ABS_THRESH)
        if (t < 0x40)
#else
        if (t < 0x41)
#endif
        {
#if !defined(WM_91FF8_MUTANT_SKIP_SNAP)
            s5 = threshold;
#endif
        }
    }
    return s5;
#endif
}
