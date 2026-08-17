/*
 * World-map 16-byte key insertion pass 0x800964B0.
 *
 * Register-faithful transcription of retail world_map.bin
 * [0x800964B0, 0x800965A4).  See world_map_helper_964b0.h.
 */
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_964b0.h"

#define W4B0_STRIDE 16u

#if defined(WM_964B0_TEST_TRACE)
extern void wm_964b0_test_store(u32 address, u32 value);
#define W4B0_TRACE_STORE(a, v) wm_964b0_test_store((a), (v))
#else
#define W4B0_TRACE_STORE(a, v) ((void)0)
#endif

static u32 w4b0_lw(u32 a)
{
    u32 v;

    memcpy(&v, PSX_ADDR(a), 4);
    return v;
}

static void w4b0_sw(u32 a, u32 v)
{
    memcpy(PSX_ADDR(a), &v, 4);
    W4B0_TRACE_STORE(a, v);
}

static void w4b0_swap16(u32 cur, u32 nxt)
{
    u32 t0 = w4b0_lw(cur);
    u32 t1 = w4b0_lw(nxt - 12u);
    u32 t2 = w4b0_lw(nxt - 8u);
    u32 t3 = w4b0_lw(nxt - 4u);

    w4b0_sw(cur, w4b0_lw(nxt));
    w4b0_sw(nxt - 12u, w4b0_lw(nxt + 4u));
    w4b0_sw(nxt - 8u, w4b0_lw(nxt + 8u));
    w4b0_sw(nxt - 4u, w4b0_lw(nxt + 12u));
    w4b0_sw(nxt, t0);
    w4b0_sw(nxt + 4u, t1);
    w4b0_sw(nxt + 8u, t2);
    w4b0_sw(nxt + 12u, t3);
}

void wm_800964B0(u32 list)
{
    u32 a2 = list;
    u32 a3;
    u32 a1;

#if defined(WM_964B0_MUTANT_SKIP_EMPTY)
    if (0 && w4b0_lw(a2 + W4B0_STRIDE) == 0u)
#else
    if (w4b0_lw(a2 + W4B0_STRIDE) == 0u)
#endif
        return;

    a3 = a2;
    a1 = list + W4B0_STRIDE;

    do {
#if defined(WM_964B0_MUTANT_WRONG_KEY)
        u32 cur = w4b0_lw(a2);
        u32 nxt = w4b0_lw(a1);
#else
        u32 cur = w4b0_lw(a1 - 12u);
        u32 nxt = w4b0_lw(a1 + 4u);
#endif

#if defined(WM_964B0_MUTANT_SIGNED_CMP)
        if ((s32)nxt < (s32)cur) {
#else
        if (nxt < cur) {
#endif
            w4b0_swap16(a2, a1);
            if (a3 < a2) {
#if defined(WM_964B0_MUTANT_WRONG_BACK)
                a1 += W4B0_STRIDE;
                a2 += W4B0_STRIDE;
#else
                a1 -= W4B0_STRIDE;
                a2 -= W4B0_STRIDE;
#endif
            }
        } else {
            a1 += W4B0_STRIDE;
            a2 += W4B0_STRIDE;
        }
    } while (w4b0_lw(a1) != 0u);
}
