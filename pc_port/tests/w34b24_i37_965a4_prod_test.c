/*
 * Focused production-linked oracle for retail helper 0x800965A4.
 *
 * Expected values are hand-derived from [0x800965A4, 0x80096668).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_965a4.h"

#define COUNT  0x8009D808u
#define INDEX  0x8009BE44u
#define BASEP  0x8009D3C0u
#define TABLE  0x8009C624u
#define POOL   0x80090000u
#define CANARY 0x8009B000u

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

void wm_965a4_test_store(u32 address, u32 value)
{
    (void)address;
    (void)value;
}

static void rec(u32 list, u32 i, u32 w0, u32 key, u32 x, u32 y)
{
    wr32(list + i * 16u, w0);
    wr32(list + i * 16u + 4u, key);
    wr32(list + i * 16u + 8u, x);
    wr32(list + i * 16u + 12u, y);
}

int main(void)
{
    s32 rc;
    u32 list;

    PsxMemory_Init();
    wr32(CANARY, 0x11111111u);
    wr32(BASEP, POOL);

    wr32(INDEX, 1u);
    wr32(COUNT, 7u);
    wr32(TABLE + 4u, 0u);
    rec(POOL + 0x580u, 0, 0u, 0u, 0u, 0u);
    rec(POOL + 0x580u, 1, 0u, 0u, 0u, 0u);
    rc = wm_800965A4();
    chk("empty.rc", (u32)rc, 0xFFFFFFFFu);
    chk("empty.count", rd32(COUNT), 0u);
    chk("empty.idx", rd32(INDEX), 1u);
    chk("empty.tab", rd32(TABLE + 4u), 0u);

    wr32(INDEX, 1u);
    wr32(COUNT, 7u);
    wr32(TABLE + 4u, 0xDEADu);
    rec(POOL + 0x580u, 0, 0x10u, 5u, 1u, 2u);
    rec(POOL + 0x580u, 1, 0x20u, 3u, 8u, 9u);
    rec(POOL + 0x580u, 2, 0x30u, 4u, 6u, 7u);
    rec(POOL + 0x580u, 3, 0u, 0u, 0u, 0u);
    rc = wm_800965A4();
    chk("occ.rc", (u32)rc, 0xFFFFFFFFu);
    chk("occ.count", rd32(COUNT), 0u);
    chk("occ.tab", rd32(TABLE + 4u), 0xDEADu);
    chk("occ.k0", rd32(POOL + 0x580u + 4u), 5u);

    wr32(INDEX, 1u);
    wr32(COUNT, 7u);
    wr32(TABLE + 4u, 0u);
    wr32(POOL + 0x500u, 0u);
    rec(POOL + 0x580u, 0, 0x10u, 5u, 1u, 2u);
    rec(POOL + 0x580u, 1, 0x20u, 3u, 8u, 9u);
    rec(POOL + 0x580u, 2, 0x30u, 4u, 6u, 7u);
    rec(POOL + 0x580u, 3, 0u, 0u, 0u, 0u);
    rc = wm_800965A4();
    list = POOL + 0x580u;
    chk("ok.rc", (u32)rc, 0u);
    chk("ok.count", rd32(COUNT), 0u);
    chk("ok.idx", rd32(INDEX), 2u);
    chk("ok.tab", rd32(TABLE + 4u), list);
    chk("ok.w0", rd32(list), 0x20u);
    chk("ok.k0", rd32(list + 4u), 3u);
    chk("ok.w1", rd32(list + 16u), 0x30u);
    chk("ok.k1", rd32(list + 20u), 4u);
    chk("ok.w2", rd32(list + 32u), 0x10u);
    chk("ok.k2", rd32(list + 36u), 5u);

    wr32(INDEX, 15u);
    wr32(COUNT, 3u);
    wr32(TABLE + 15u * 4u, 0u);
    list = POOL + 15u * 0x580u;
    rec(list, 0, 0xAAu, 2u, 9u, 8u);
    rec(list, 1, 0xBBu, 1u, 7u, 6u);
    rec(list, 2, 0u, 0u, 0u, 0u);
    rc = wm_800965A4();
    chk("wrap.rc", (u32)rc, 0u);
    chk("wrap.idx", rd32(INDEX), 0u);
    chk("wrap.tab", rd32(TABLE + 15u * 4u), list);
    chk("wrap.k0", rd32(list + 4u), 1u);
    chk("wrap.k1", rd32(list + 20u), 2u);
    chk("canary", rd32(CANARY), 0x11111111u);

    if (s_failures) {
        fprintf(stderr, "ASSERTION failures=%d\n", s_failures);
        return 1;
    }
    printf("W34B24-I37 0x800965A4 focused oracle PASS\n");
    return 0;
}
