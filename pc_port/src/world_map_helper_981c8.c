/*
 * World-map 8x8 cell-index helper 0x800981C8.
 *
 * Register-faithful transcription of retail world_map.bin
 * [0x800981C8, 0x800983A0).  See world_map_helper_981c8.h.
 */
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_981c8.h"

#define W1C8_DIM_X   0x8009D160u
#define W1C8_DIM_Z   0x8009D2B4u
#define W1C8_CELL_X  0x8009C838u
#define W1C8_CELL_Z  0x8009C83Cu
#define W1C8_DST     0x8009D318u
#define W1C8_SRC     0x8009D570u
#define W1C8_COPY    0xA0u

static u32 w1c8_lw(u32 a)
{
    u32 v;

    memcpy(&v, PSX_ADDR(a), 4);
    return v;
}

static s32 w1c8_lh(u32 a)
{
    s16 h;

    memcpy(&h, PSX_ADDR(a), 2);
    return (s32)h;
}

static void w1c8_sh(u32 a, u32 v)
{
    u16 h = (u16)(v & 0xFFFFu);

    memcpy(PSX_ADDR(a), &h, 2);
}

static s32 w1c8_div8(s32 v)
{
#if defined(WM_981C8_MUTANT_SKIP_NEG_ADJUST)
    (void)0;
#else
    if (v < 0)
        v += 7;
#endif
#if defined(WM_981C8_MUTANT_WRONG_DIV)
    return v >> 2;
#else
    return v >> 3;
#endif
}

static s32 w1c8_wrap(s32 v, s32 span)
{
    if (v < 0)
        return v + span;
    if (span < v)
        return v - span;
    return v;
}

static void w1c8_copy_block(u32 dst, u32 src)
{
    u32 i;

    if (((src | dst) & 3u) != 0u) {
        for (i = 0; i < 10u; i++) {
            memcpy(PSX_ADDR(dst + i * 16u), PSX_ADDR(src + i * 16u), 16);
        }
    } else {
        for (i = 0; i < 10u; i++) {
            u32 w0 = w1c8_lw(src + i * 16u);
            u32 w1 = w1c8_lw(src + i * 16u + 4u);
            u32 w2 = w1c8_lw(src + i * 16u + 8u);
            u32 w3 = w1c8_lw(src + i * 16u + 12u);

            memcpy(PSX_ADDR(dst + i * 16u), &w0, 4);
            memcpy(PSX_ADDR(dst + i * 16u + 4u), &w1, 4);
            memcpy(PSX_ADDR(dst + i * 16u + 8u), &w2, 4);
            memcpy(PSX_ADDR(dst + i * 16u + 12u), &w3, 4);
        }
    }
}

void wm_800981C8(u32 pos_addr)
{
    s32 dim_x = (s32)w1c8_lw(W1C8_DIM_X);
    s32 dim_z = (s32)w1c8_lw(W1C8_DIM_Z);
    s32 x;
    s32 z;
    s32 a3;
    s32 t1;
    s32 bias_x;
    s32 bias_z;
    u32 dst = W1C8_DST;
    u32 src = W1C8_SRC;

#if defined(WM_981C8_MUTANT_WRONG_SHIFT12)
    x = (s32)w1c8_lw(pos_addr) >> 11;
    z = (s32)w1c8_lw(pos_addr + 8u) >> 11;
#else
    x = (s32)w1c8_lw(pos_addr) >> 12;
    z = (s32)w1c8_lw(pos_addr + 8u) >> 12;
#endif
    x = w1c8_div8(x);
    z = w1c8_div8(z);

    bias_x = (w1c8_lh(W1C8_CELL_X) + 2) << 8;
    bias_z = (w1c8_lh(W1C8_CELL_Z) + 2) << 8;
    a3 = x - bias_x;
    t1 = z - bias_z;

#if defined(WM_981C8_MUTANT_SKIP_X_WRAP)
    (void)dim_x;
#else
    a3 = w1c8_wrap(a3, (s32)((u32)dim_x << 8));
#endif
#if defined(WM_981C8_MUTANT_SKIP_Z_WRAP)
    (void)dim_z;
#else
    t1 = w1c8_wrap(t1, (s32)((u32)dim_z << 8));
#endif
    a3 >>= 8;
    t1 >>= 8;

#if defined(WM_981C8_MUTANT_SKIP_COPY)
    if (0)
        w1c8_copy_block(dst, src);
#else
    w1c8_copy_block(dst, src);
#endif
#if !defined(WM_981C8_MUTANT_SKIP_HALF)
    w1c8_sh(dst + W1C8_COPY, (u32)w1c8_lh(src + W1C8_COPY));
#endif

#if !defined(WM_981C8_MUTANT_SKIP_FILL)
    {
        s32 saved_x = a3;
        s32 row;
        s32 col;
        u32 out = W1C8_SRC;
        s32 width = dim_x;

#if defined(WM_981C8_MUTANT_WRONG_DIM)
        width += 1;
#endif

        for (row = 0; row < 8; row++) {
#if !defined(WM_981C8_MUTANT_SKIP_ROW_WRAP)
            if (!(t1 < dim_z))
                t1 = 0;
#endif
            {
                s32 acc = (s32)((u32)t1 * (u32)width);
                s32 cx = saved_x;

                for (col = 0; col < 8; col++) {
#if !defined(WM_981C8_MUTANT_SKIP_COL_WRAP)
                    if (!(cx < width))
                        cx = 0;
#endif
                    w1c8_sh(out, (u32)(acc + cx));
                    out += 2u;
                    cx += 1;
                }
            }
            t1 += 1;
        }
    }
#endif
}
