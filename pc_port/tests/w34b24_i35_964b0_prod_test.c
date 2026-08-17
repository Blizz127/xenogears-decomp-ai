/*
 * Focused production-linked oracle for retail helper 0x800964B0.
 *
 * Expected values are hand-derived from [0x800964B0, 0x800965A4).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_964b0.h"

#define LIST   0x80090000u
#define CANARY 0x80091000u

static int s_failures;

static void chk(const char *n, u32 got, u32 want)
{
    if (got != want) {
        fprintf(stderr, "ASSERTION %s: got=0x%08x expected=0x%08x\n", n, got,
                want);
        if (++s_failures > 40)
            exit(1);
    }
}

static u32 rd32(u32 a)
{
    u32 v;

    memcpy(&v, PSX_ADDR(a), 4);
    return v;
}

static void wr32(u32 a, u32 v) { memcpy(PSX_ADDR(a), &v, 4); }

void wm_964b0_test_store(u32 address, u32 value)
{
    (void)address;
    (void)value;
}

static void rec(u32 i, u32 w0, u32 key, u32 x, u32 y)
{
    wr32(LIST + i * 16u, w0);
    wr32(LIST + i * 16u + 4u, key);
    wr32(LIST + i * 16u + 8u, x);
    wr32(LIST + i * 16u + 12u, y);
}

int main(void)
{
    PsxMemory_Init();
    wr32(CANARY, 0x11111111u);

    rec(0, 0xAu, 5u, 0xBu, 0xCu);
    rec(1, 0u, 0u, 0u, 0u);
    wm_800964B0(LIST);
    chk("empty.w0", rd32(LIST), 0xAu);
    chk("empty.k0", rd32(LIST + 4u), 5u);
    chk("empty.w1", rd32(LIST + 16u), 0u);

    rec(0, 0x10u, 5u, 1u, 2u);
    rec(1, 0x20u, 3u, 8u, 9u);
    rec(2, 0x30u, 4u, 6u, 7u);
    rec(3, 0u, 0u, 0u, 0u);
    wm_800964B0(LIST);
    chk("s.w0", rd32(LIST), 0x20u);
    chk("s.k0", rd32(LIST + 4u), 3u);
    chk("s.x0", rd32(LIST + 8u), 8u);
    chk("s.w1", rd32(LIST + 16u), 0x30u);
    chk("s.k1", rd32(LIST + 20u), 4u);
    chk("s.w2", rd32(LIST + 32u), 0x10u);
    chk("s.k2", rd32(LIST + 36u), 5u);
    chk("s.w3", rd32(LIST + 48u), 0u);

    rec(0, 1u, 5u, 1u, 1u);
    rec(1, 2u, 4u, 2u, 2u);
    rec(2, 3u, 3u, 3u, 3u);
    rec(3, 0u, 0u, 0u, 0u);
    wm_800964B0(LIST);
    chk("r.k0", rd32(LIST + 4u), 3u);
    chk("r.k1", rd32(LIST + 20u), 4u);
    chk("r.k2", rd32(LIST + 36u), 5u);

    rec(0, 1u, 1u, 1u, 1u);
    rec(1, 2u, 0x80000000u, 2u, 2u);
    rec(2, 0u, 0u, 0u, 0u);
    wm_800964B0(LIST);
    chk("u.k0", rd32(LIST + 4u), 1u);
    chk("u.k1", rd32(LIST + 20u), 0x80000000u);
    chk("canary", rd32(CANARY), 0x11111111u);

    if (s_failures) {
        fprintf(stderr, "ASSERTION failures=%d\n", s_failures);
        return 1;
    }
    printf("W34B24-I35 0x800964B0 focused oracle PASS\n");
    return 0;
}
