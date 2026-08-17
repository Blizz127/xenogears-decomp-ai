/*
 * Focused production-linked oracle for retail helper 0x80089580.
 *
 * Expected values are hand-derived from [0x80089580, 0x80089748).
 * NEXT_CALLBACK_TARGET is not an oracle input.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_89580.h"

#define REC_PTR 0x8009BDF4u
#define TAB_PTR 0x8009BCC0u
#define RECS    0x80090000u
#define TAB     0x80091000u
#define CANARY  0x80092000u
#define STRIDE  0x4Cu

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

static u16 rd16(u32 a)
{
    u16 v;

    memcpy(&v, PSX_ADDR(a), 2);
    return v;
}

static void wr32(u32 a, u32 v) { memcpy(PSX_ADDR(a), &v, 4); }
static void wr16(u32 a, u16 v) { memcpy(PSX_ADDR(a), &v, 2); }
static void wr8(u32 a, u8 v) { memcpy(PSX_ADDR(a), &v, 1); }

void wm_89580_test_store(u32 address, u32 value)
{
    (void)address;
    (void)value;
}

static u32 rec(u32 i)
{
    return RECS + i * STRIDE;
}

static u32 slot(u32 i)
{
    return rec(i) + 4u;
}

static void seed(void)
{
    u32 i;

    PsxMemory_Init();
    wr32(REC_PTR, RECS);
    wr32(TAB_PTR, TAB);
    wr32(CANARY, 0x11111111u);
    for (i = 0u; i < 0x100u; i++) {
        wr16(rec(i), 0);
        wr32(slot(i), 0);
    }

    wr16(rec(0), 2);
    wr32(slot(0), 0x00010005u);
    wr32(slot(0) + 4u, 10u);
    wr32(slot(0) + 8u, 20u);
    wr32(slot(0) + 0xCu, 30u);
    wr32(slot(0) + 0x14u, 1u);
    wr32(slot(0) + 0x18u, 2u);
    wr32(slot(0) + 0x1Cu, 3u);
    wr32(slot(0) + 0x24u, 4u);
    wr32(slot(0) + 0x28u, 5u);
    wr32(slot(0) + 0x2Cu, 6u);
    wr16(slot(0) + 0x34u, 0x10u);
    wr16(slot(0) + 0x36u, 0x20u);
    wr16(slot(0) + 0x38u, 0x01u);
    wr16(slot(0) + 0x3Au, 0x02u);
    wr32(slot(0) + 0x3Cu, 0xAA1122FFu);
    wr8(slot(0) + 0x40u, 1);
    wr8(slot(0) + 0x41u, 0xFFu);
    wr8(slot(0) + 0x42u, 2);
    wr8(slot(0) + 0x43u, 0);

    wr16(rec(1), 0xBEEFu);
    wr32(slot(1), 0x00000007u);
    wr32(slot(1) + 4u, 0xDEADu);

    wr16(rec(2), 3);
    wr32(slot(2), 0x0001FFFFu);
    wr16(TAB + 3u * 0x54u + 0xAu, 7u);
}

int main(void)
{
    seed();
    wm_80089580();

    chk("s0.life", (u32)rd16(slot(0)), 4u);
    chk("s0.x", rd32(slot(0) + 4u), 11u);
    chk("s0.y", rd32(slot(0) + 8u), 22u);
    chk("s0.z", rd32(slot(0) + 0xCu), 33u);
    chk("s0.vx", rd32(slot(0) + 0x14u), 5u);
    chk("s0.vy", rd32(slot(0) + 0x18u), 7u);
    chk("s0.vz", rd32(slot(0) + 0x1Cu), 9u);
    chk("s0.u", (u32)rd16(slot(0) + 0x34u), 0x11u);
    chk("s0.v", (u32)rd16(slot(0) + 0x36u), 0x22u);
    chk("s0.rgb", rd32(slot(0) + 0x3Cu), 0xAA1321FFu);
    chk("s1.word", rd32(slot(1)), 7u);
    chk("s1.x", rd32(slot(1) + 4u), 0xDEADu);
    chk("s1.idx", (u32)rd16(rec(1)), 0xBEEFu);
    chk("s2.word", rd32(slot(2)), 0u);
    chk("s2.idx", (u32)rd16(rec(2)), 0u);
    chk("tab.count", (u32)rd16(TAB + 3u * 0x54u + 0xAu), 6u);
    chk("canary", rd32(CANARY), 0x11111111u);

    if (s_failures) {
        fprintf(stderr, "ASSERTION failures=%d\n", s_failures);
        return 1;
    }
    printf("W34B24-I30 0x80089580 focused oracle PASS\n");
    return 0;
}
