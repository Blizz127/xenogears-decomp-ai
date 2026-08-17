/*
 * World-map 256-slot particle integrator 0x80089580.
 *
 * Register-faithful transcription of retail world_map.bin
 * [0x80089580, 0x80089748).  See world_map_helper_89580.h.
 */
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_89580.h"

#define W895_REC   0x8009BDF4u
#define W895_TAB   0x8009BCC0u
#define W895_STRIDE 0x4Cu
#define W895_COUNT  0x100u

#if defined(WM_89580_TEST_TRACE)
extern void wm_89580_test_store(u32 address, u32 value);
#define W895_TRACE_STORE(a, v) wm_89580_test_store((a), (v))
#else
#define W895_TRACE_STORE(a, v) ((void)0)
#endif

static u32 w895_lw(u32 a)
{
    u32 v;

    memcpy(&v, PSX_ADDR(a), 4);
    return v;
}

static s32 w895_lh(u32 a)
{
    s16 h;

    memcpy(&h, PSX_ADDR(a), 2);
    return (s32)h;
}

static u32 w895_lhu(u32 a)
{
    u16 h;

    memcpy(&h, PSX_ADDR(a), 2);
    return (u32)h;
}

static s32 w895_lb(u32 a)
{
    s8 b;

    memcpy(&b, PSX_ADDR(a), 1);
    return (s32)b;
}

static void w895_sh(u32 a, u32 v)
{
    u16 h = (u16)(v & 0xFFFFu);

    memcpy(PSX_ADDR(a), &h, 2);
    W895_TRACE_STORE(a, (u32)h);
}

static void w895_sw(u32 a, u32 v)
{
    memcpy(PSX_ADDR(a), &v, 4);
    W895_TRACE_STORE(a, v);
}

static s32 w895_bits_as_s32(u32 value)
{
    s32 result;

    memcpy(&result, &value, sizeof(result));
    return result;
}

static u32 w895_s32_as_bits(s32 value)
{
    u32 result;

    memcpy(&result, &value, sizeof(result));
    return result;
}

static s32 w895_sra(s32 value, u32 shift)
{
    u32 amount = shift & 31u;
    u32 bits = w895_s32_as_bits(value);
    u32 result;

    if (amount == 0u)
        return value;
    result = bits >> amount;
    if (value < 0)
        result |= UINT32_MAX << (32u - amount);
    return w895_bits_as_s32(result);
}

static s32 w895_clamp_u8(s32 value)
{
#if defined(WM_89580_MUTANT_SKIP_CLAMP)
    return value;
#else
    if (value < 0)
        return 0;
    if (value >= 0x100)
        return 0xFF;
    return value;
#endif
}

void wm_80089580(void)
{
    u32 rec = w895_lw(W895_REC);
    u32 slot;
    u32 i;
    u32 n;
    u32 stride;

#if defined(WM_89580_MUTANT_WRONG_REC_OFF)
    slot = rec;
#else
    slot = rec + 4u;
#endif
#if defined(WM_89580_MUTANT_WRONG_COUNT)
    n = 1u;
#else
    n = W895_COUNT;
#endif
#if defined(WM_89580_MUTANT_WRONG_STRIDE)
    stride = 0x40u;
#else
    stride = W895_STRIDE;
#endif

    for (i = 0u; i < n; i++, rec += stride, slot += stride) {
        u32 word = w895_lw(slot);
        s32 hi = w895_sra(w895_bits_as_s32(word), 16u);
        s32 lo = w895_sra(w895_bits_as_s32(word << 16), 16u);

#if defined(WM_89580_MUTANT_SKIP_HI)
        if (0 && hi == 0)
            continue;
#else
        if (hi == 0)
            continue;
#endif
#if defined(WM_89580_MUTANT_SKIP_LO_DEATH)
        if (0 && lo <= 0) {
#else
        if (lo <= 0) {
#endif
            s32 idx = w895_lh(rec);
#if !defined(WM_89580_MUTANT_SKIP_DEATH)
            u32 row;
            u32 count;

#if defined(WM_89580_MUTANT_WRONG_TABLE_MUL)
            row = w895_lw(W895_TAB) + (u32)idx * 0x50u;
#else
            row = w895_lw(W895_TAB) +
                  (u32)(((((idx << 2) + idx) << 2) + idx) << 2);
#endif
            count = w895_lhu(row + 0xAu);
            w895_sh(row + 0xAu, count - 1u);
            w895_sh(rec, 0u);
            w895_sw(slot, 0u);
#else
            (void)idx;
#endif
            continue;
        }

#if !defined(WM_89580_MUTANT_SKIP_TIMER)
        w895_sh(slot, w895_lhu(slot) - 1u);
#endif

        {
            u32 t6 = w895_lw(slot + 4u);
            u32 t7 = w895_lw(slot + 8u);
            u32 t8 = w895_lw(slot + 0xCu);
            u32 t3 = w895_lw(slot + 0x14u);
            u32 t4 = w895_lw(slot + 0x18u);
            u32 t5 = w895_lw(slot + 0x1Cu);
            u32 t2 = w895_lw(slot + 0x3Cu);
            u32 a1 = w895_lw(slot + 0x40u);
            u32 acc_x = w895_lw(slot + 0x24u);
            u32 acc_y = w895_lw(slot + 0x28u);
            u32 acc_z = w895_lw(slot + 0x2Cu);
            s32 r;
            s32 g;
            s32 b;
            u32 packed;

#if !defined(WM_89580_MUTANT_SKIP_POS)
            t6 += t3;
            t8 += t5;
            t7 += t4;
#endif
            t3 += acc_x;
            t5 += acc_z;
            t4 += acc_y;

            {
                u32 uv0 = w895_lhu(slot + 0x34u) + w895_lhu(slot + 0x38u);
                u32 uv1 = w895_lhu(slot + 0x36u) + w895_lhu(slot + 0x3Au);

                w895_sh(slot + 0x34u, uv0);
                w895_sh(slot + 0x36u, uv1);
            }

            r = w895_clamp_u8((s32)(t2 & 0xFFu) + w895_lb(slot + 0x40u));
            g = w895_clamp_u8(((s32)((t2 >> 8) & 0xFFu)) +
                              w895_sra(w895_bits_as_s32(a1 << 16), 24u));
            b = w895_clamp_u8(((s32)((t2 >> 16) & 0xFFu)) +
                              w895_sra(w895_bits_as_s32(a1 << 8), 24u));
            packed = (t2 & 0xFF000000u) | ((u32)b << 16) | ((u32)g << 8) |
                     (u32)r;
#if !defined(WM_89580_MUTANT_SKIP_RGB)
            w895_sw(slot + 0x3Cu, packed);
#else
            (void)packed;
#endif
            w895_sw(slot + 4u, t6);
            w895_sw(slot + 8u, t7);
            w895_sw(slot + 0xCu, t8);
            w895_sw(slot + 0x14u, t3);
            w895_sw(slot + 0x18u, t4);
            w895_sw(slot + 0x1Cu, t5);
        }
    }
}
