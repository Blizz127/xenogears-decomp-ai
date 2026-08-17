/*
 * World-map fixed-threshold X/Z wrap helper 0x80093484.
 *
 * Register-faithful transcription of retail world_map.bin
 * [0x80093484, 0x80093534).  See world_map_helper_93484.h.
 *
 * X at +0, then Z at +8:
 *   if signed word <  0xFC000000: word += (period_word << 23)
 *   else if 0x04000000 < signed word: word -= (period_word << 23)
 */
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_93484.h"

#define W484_WRAP_X  0x8009D160u
#define W484_WRAP_Z  0x8009D2B4u
#define W484_LO      0xFC000000u
#define W484_HI      0x04000000u

#if defined(WM_93484_TEST_TRACE)
extern void wm_93484_test_store(u32 address, u32 value);
#define W484_TRACE_STORE(a, v) wm_93484_test_store((a), (v))
#else
#define W484_TRACE_STORE(a, v) ((void)0)
#endif

static u32 w484_lw(u32 a)
{
    u32 v;

    memcpy(&v, PSX_ADDR(a), 4);
    return v;
}

static void w484_sw(u32 a, u32 v)
{
    memcpy(PSX_ADDR(a), &v, 4);
    W484_TRACE_STORE(a, v);
}

static s32 w484_s32(u32 v)
{
    s32 r;

    memcpy(&r, &v, 4);
    return r;
}

static void w484_wrap_word(u32 word_addr, u32 global_addr)
{
    u32 bits = w484_lw(word_addr);
    s32 val = w484_s32(bits);
#if defined(WM_93484_MUTANT_WRONG_SHIFT)
    u32 period = w484_lw(global_addr) << 11;
#else
    u32 period = w484_lw(global_addr) << 23;
#endif

#if defined(WM_93484_MUTANT_LO_THRESHOLD)
    if (val < (s32)0xFE000000)
#elif defined(WM_93484_MUTANT_UNSIGNED_LO)
    if (bits < W484_LO)
#else
    if (val < w484_s32(W484_LO))
#endif
    {
#if defined(WM_93484_MUTANT_ADD_SUB_SWAP)
        w484_sw(word_addr, bits - period);
#else
        w484_sw(word_addr, bits + period);
#endif
        return;
    }
#if !defined(WM_93484_MUTANT_SKIP_HI)
#if defined(WM_93484_MUTANT_HI_THRESHOLD)
    if (w484_s32(0x02000000u) < val)
#else
    if (w484_s32(W484_HI) < val)
#endif
    {
#if defined(WM_93484_MUTANT_ADD_SUB_SWAP)
        w484_sw(word_addr, bits + period);
#else
        w484_sw(word_addr, bits - period);
#endif
    }
#endif
}

void wm_80093484(u32 vec_addr)
{
#if defined(WM_93484_MUTANT_SKIP_X)
    (void)vec_addr;
#else
#if defined(WM_93484_MUTANT_WRONG_X_GLOBAL)
    w484_wrap_word(vec_addr, W484_WRAP_Z);
#else
    w484_wrap_word(vec_addr, W484_WRAP_X);
#endif
#endif
#if defined(WM_93484_MUTANT_SKIP_Z)
    return;
#elif defined(WM_93484_MUTANT_WRONG_Z_OFFSET)
    w484_wrap_word(vec_addr + 4u, W484_WRAP_Z);
#elif defined(WM_93484_MUTANT_WRONG_Z_GLOBAL)
    w484_wrap_word(vec_addr + 8u, W484_WRAP_X);
#else
    w484_wrap_word(vec_addr + 8u, W484_WRAP_Z);
#endif
#if defined(WM_93484_MUTANT_TOUCH_Y)
    w484_sw(vec_addr + 4u, 0u);
#endif
}
