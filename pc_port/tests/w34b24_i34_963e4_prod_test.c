/*
 * Focused production-linked oracle for retail helper 0x800963E4.
 *
 * Expected values are hand-derived from [0x800963E4, 0x800964B0).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_963e4.h"

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

void wm_963e4_test_store(u32 address, u32 value)
{
    (void)address;
    (void)value;
}

static void rec(u32 i, u32 k, u32 b, u32 c)
{
    wr32(LIST + i * 12u, k);
    wr32(LIST + i * 12u + 4u, b);
    wr32(LIST + i * 12u + 8u, c);
}

int main(void)
{
    PsxMemory_Init();
    wr32(CANARY, 0x11111111u);

    rec(0, 5u, 0xAu, 0xBu);
    rec(1, 0u, 0u, 0u);
    wm_800963E4(LIST);
    chk("empty.k0", rd32(LIST), 5u);
    chk("empty.b0", rd32(LIST + 4u), 0xAu);
    chk("empty.k1", rd32(LIST + 12u), 0u);

    rec(0, 5u, 1u, 2u);
    rec(1, 3u, 8u, 9u);
    rec(2, 4u, 6u, 7u);
    rec(3, 0u, 0u, 0u);
    wm_800963E4(LIST);
    chk("s.k0", rd32(LIST), 3u);
    chk("s.b0", rd32(LIST + 4u), 8u);
    chk("s.c0", rd32(LIST + 8u), 9u);
    chk("s.k1", rd32(LIST + 12u), 4u);
    chk("s.b1", rd32(LIST + 16u), 6u);
    chk("s.k2", rd32(LIST + 24u), 5u);
    chk("s.b2", rd32(LIST + 28u), 1u);
    chk("s.k3", rd32(LIST + 36u), 0u);

    rec(0, 5u, 1u, 1u);
    rec(1, 4u, 2u, 2u);
    rec(2, 3u, 3u, 3u);
    rec(3, 0u, 0u, 0u);
    wm_800963E4(LIST);
    chk("r.k0", rd32(LIST), 3u);
    chk("r.k1", rd32(LIST + 12u), 4u);
    chk("r.k2", rd32(LIST + 24u), 5u);

    rec(0, 1u, 1u, 1u);
    rec(1, 0x80000000u, 2u, 2u);
    rec(2, 0u, 0u, 0u);
    wm_800963E4(LIST);
    chk("u.k0", rd32(LIST), 1u);
    chk("u.k1", rd32(LIST + 12u), 0x80000000u);
    chk("canary", rd32(CANARY), 0x11111111u);

    if (s_failures) {
        fprintf(stderr, "ASSERTION failures=%d\n", s_failures);
        return 1;
    }
    printf("W34B24-I34 0x800963E4 focused oracle PASS\n");
    return 0;
}
