/*
 * Focused production-linked oracle for retail callback 0x800922AC.
 *
 * Expected values are hand-derived from [0x800922AC, 0x800923A8).
 * NEXT_CALLBACK_TARGET is not an oracle input.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_callback_922ac.h"

#define POOL_GUEST 0x800D7538u
#define SLOT11     (POOL_GUEST + 0x580u)
#define SLOT10     (POOL_GUEST + 0x500u)
#define SLOT12     (POOL_GUEST + 0x600u)
#define G_POOL     0x8009BE24u
#define G_BE0C     0x8009BE0Cu
#define G_BE10     0x8009BE10u

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

void wm_922ac_test_store(u32 address, u32 value)
{
    (void)address;
    (void)value;
}

static void reset(void)
{
    wr32(G_POOL, POOL_GUEST);
    wr32(SLOT10, 0x5A5A5A5Au);
    wr32(SLOT12, 0xA5A5A5A5u);
    wr16(SLOT11 + 4u, 0);
    wr16(SLOT11 + 0x20u, 0);
    wr32(SLOT11 + 0x50u, 0x00080000u);
    wr32(G_BE0C, 0xDEADu);
    wr32(G_BE10, 0xBEEFu);
}

static s32 run(void)
{
    return wm_800922AC(11);
}

int main(void)
{
    s32 r;

    PsxMemory_Init();

    reset();
    wr16(SLOT11 + 0x20u, 0);
    r = run();
    chk("s0.ret", (u32)r, 3u);
    chk("s0.be0c", rd32(G_BE0C), 0xDEADu);
    chk("s0.iso10", rd32(SLOT10), 0x5A5A5A5Au);
    chk("s0.iso12", rd32(SLOT12), 0xA5A5A5A5u);

    reset();
    wr16(SLOT11 + 0x20u, 1);
    wr32(SLOT11 + 0x50u, 0x00080000u); /* 0x80 << 12 */
    r = run();
    chk("s1.ret", (u32)r, 1u);
    chk("s1.50", rd32(SLOT11 + 0x50u), 0x0007F000u);
    chk("s1.be0c", rd32(G_BE0C), 0x7Fu);
    chk("s1.state", (u32)rd16(SLOT11 + 0x20u), 1u);

    reset();
    wr16(SLOT11 + 0x20u, 1);
    wr32(SLOT11 + 0x50u, 0x00078000u);
    r = run();
    chk("s1c.50", rd32(SLOT11 + 0x50u), 0x00078000u);
    chk("s1c.be0c", rd32(G_BE0C), 0x78u);
    chk("s1c.state", (u32)rd16(SLOT11 + 0x20u), 0u);

    reset();
    wr16(SLOT11 + 0x20u, 2);
    wr32(SLOT11 + 0x50u, 0x00080000u);
    r = run();
    chk("s2.50", rd32(SLOT11 + 0x50u), 0x00081000u);
    chk("s2.be0c", rd32(G_BE0C), 0x81u);
    chk("s2.state", (u32)rd16(SLOT11 + 0x20u), 2u);

    reset();
    wr16(SLOT11 + 0x20u, 2);
    wr32(SLOT11 + 0x50u, 0x0008B000u);
    r = run();
    chk("s2c.50", rd32(SLOT11 + 0x50u), 0x0008C000u);
    chk("s2c.be0c", rd32(G_BE0C), 0x8Cu);
    chk("s2c.state", (u32)rd16(SLOT11 + 0x20u), 0u);

    reset();
    wr16(SLOT11 + 4u, 9);
    wr32(SLOT11 + 0x50u, 0x00080000u);
    r = run();
    chk("sub9.sub", (u32)rd16(SLOT11 + 4u), 0u);
    chk("sub9.50", rd32(SLOT11 + 0x50u), 0x0007F000u);
    chk("sub9.be0c", rd32(G_BE0C), 0x7Fu);

    reset();
    wr16(SLOT11 + 4u, 0xA);
    wr32(SLOT11 + 0x50u, 0x00080000u);
    r = run();
    chk("suba.50", rd32(SLOT11 + 0x50u), 0x00081000u);
    chk("suba.be0c", rd32(G_BE0C), 0x81u);

    reset();
    wr16(SLOT11 + 0x20u, 4);
    r = run();
    chk("unk.ret", (u32)r, 1u);
    chk("unk.be0c", rd32(G_BE0C), 0xDEADu);

    if (s_failures != 0) {
        fprintf(stderr, "%d assertion(s) failed\n", s_failures);
        return 1;
    }
    printf("W34B24-I18 0x800922AC focused oracle PASS\n");
    return 0;
}
