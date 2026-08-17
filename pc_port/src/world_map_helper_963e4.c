/*
 * World-map 12-byte key insertion pass 0x800963E4.
 *
 * Register-faithful transcription of retail world_map.bin
 * [0x800963E4, 0x800964B0).  See world_map_helper_963e4.h.
 */
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_963e4.h"

#define W3E4_STRIDE 12u

#if defined(WM_963E4_TEST_TRACE)
extern void wm_963e4_test_store(u32 address, u32 value);
#define W3E4_TRACE_STORE(a, v) wm_963e4_test_store((a), (v))
#else
#define W3E4_TRACE_STORE(a, v) ((void)0)
#endif

static u32 w3e4_lw(u32 a)
{
    u32 v;

    memcpy(&v, PSX_ADDR(a), 4);
    return v;
}

static void w3e4_sw(u32 a, u32 v)
{
    memcpy(PSX_ADDR(a), &v, 4);
    W3E4_TRACE_STORE(a, v);
}

static void w3e4_swap12(u32 left, u32 right)
{
    u32 t0 = w3e4_lw(left);
    u32 t1 = w3e4_lw(right - 8u);
    u32 t2 = w3e4_lw(right - 4u);

    w3e4_sw(left, w3e4_lw(right));
    w3e4_sw(right - 8u, w3e4_lw(right + 4u));
    w3e4_sw(right - 4u, w3e4_lw(right + 8u));
    w3e4_sw(right, t0);
    w3e4_sw(right + 4u, t1);
    w3e4_sw(right + 8u, t2);
}

void wm_800963E4(u32 list)
{
    u32 a1 = list;
    u32 a2;
    u32 a0;

#if defined(WM_963E4_MUTANT_SKIP_EMPTY)
    if (0 && w3e4_lw(a1 + W3E4_STRIDE) == 0u)
#else
    if (w3e4_lw(a1 + W3E4_STRIDE) == 0u)
#endif
        return;

    a2 = a1;
    a0 = list + W3E4_STRIDE;

    do {
#if defined(WM_963E4_MUTANT_WRONG_KEY)
        u32 cur = w3e4_lw(a1 + 4u);
        u32 nxt = w3e4_lw(a0 + 4u);
#else
        u32 cur = w3e4_lw(a1);
        u32 nxt = w3e4_lw(a0);
#endif

#if defined(WM_963E4_MUTANT_SIGNED_CMP)
        if ((s32)nxt < (s32)cur) {
#else
        if (nxt < cur) {
#endif
            w3e4_swap12(a1, a0);
            if (a2 < a1) {
#if defined(WM_963E4_MUTANT_WRONG_BACK)
                a0 += W3E4_STRIDE;
                a1 += W3E4_STRIDE;
#else
                a0 -= W3E4_STRIDE;
                a1 -= W3E4_STRIDE;
#endif
            }
        } else {
            a0 += W3E4_STRIDE;
            a1 += W3E4_STRIDE;
        }
    } while (w3e4_lw(a0) != 0u);
}
