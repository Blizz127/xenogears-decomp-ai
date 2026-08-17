/*
 * Focused production-linked oracle for retail helper 0x800968E0.
 *
 * Expected values are hand-derived from [0x800968E0, 0x8009699C)
 * and the six-word switch at 0x80070CA0.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_968e0.h"

#define STATE  0x8009CD44u
#define COUNT  0x8009BD2Cu
#define RING   0x8009BCB8u
#define TABLE  0x8009D788u
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

void wm_968e0_test_store(u32 address, u32 value)
{
    (void)address;
    (void)value;
}

int main(void)
{
    s32 rc;

    PsxMemory_Init();
    wr32(CANARY, 0x11111111u);
    wr32(COUNT, 3u);
    wr32(RING, 1u);
    wr32(TABLE + 4u, 0xDEADu);

    wr32(STATE, 0u);
    rc = wm_800968E0();
    chk("c0.rc", (u32)rc, 0u);
    chk("c0.st", rd32(STATE), 0u);

    wr32(STATE, 1u);
    rc = wm_800968E0();
    chk("c1.rc", (u32)rc, 1u);
    wr32(STATE, 2u);
    rc = wm_800968E0();
    chk("c2.rc", (u32)rc, 1u);
    wr32(STATE, 3u);
    rc = wm_800968E0();
    chk("c3.rc", (u32)rc, 1u);

    wr32(STATE, 4u);
    wr32(COUNT, 3u);
    rc = wm_800968E0();
    chk("c4a.rc", (u32)rc, 1u);
    chk("c4a.cnt", rd32(COUNT), 2u);
    chk("c4a.st", rd32(STATE), 4u);

    wr32(STATE, 4u);
    wr32(COUNT, 1u);
    rc = wm_800968E0();
    chk("c4b.rc", (u32)rc, 1u);
    chk("c4b.cnt", rd32(COUNT), 0u);
    chk("c4b.st", rd32(STATE), 5u);

    wr32(STATE, 5u);
    wr32(RING, 15u);
    wr32(TABLE + 15u * 4u, 0xBEEFu);
    rc = wm_800968E0();
    chk("c5.rc", (u32)rc, 2u);
    chk("c5.st", rd32(STATE), 0u);
    chk("c5.tab", rd32(TABLE + 15u * 4u), 0u);
    chk("c5.ring", rd32(RING), 0u);

    wr32(STATE, 6u);
    wr32(TABLE + 4u, 0xDEADu);
    rc = wm_800968E0();
    chk("c6.rc", (u32)rc, 3u);
    chk("c6.tab", rd32(TABLE + 4u), 0xDEADu);
    chk("canary", rd32(CANARY), 0x11111111u);

    if (s_failures) {
        fprintf(stderr, "ASSERTION failures=%d\n", s_failures);
        return 1;
    }
    printf("W34B24-I39 0x800968E0 focused oracle PASS\n");
    return 0;
}
