/*
 * Focused production-linked oracle for retail helper 0x8009623C.
 *
 * Expected values are hand-derived from [0x8009623C, 0x800962B0).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_9623c.h"

#define COUNT  0x8009D808u
#define INDEX  0x8009BE44u
#define BASEP  0x8009BE08u
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

void wm_9623c_test_store(u32 address, u32 value)
{
    (void)address;
    (void)value;
}

int main(void)
{
    s32 rc;

    PsxMemory_Init();
    wr32(COUNT, 2u);
    wr32(INDEX, 1u);
    wr32(BASEP, POOL);
    wr32(CANARY, 0x11111111u);
    wr32(POOL + 0x420u + 24u, 0u);
    wr32(POOL + 0x420u + 28u, 0u);
    wr32(POOL + 0x420u + 32u, 0u);

    rc = wm_8009623C(0x11u, 0x22u, 0x33u);
    chk("rc", (u32)rc, 0u);
    chk("count", rd32(COUNT), 3u);
    chk("w0", rd32(POOL + 0x420u + 24u), 0x11u);
    chk("w1", rd32(POOL + 0x420u + 28u), 0x22u);
    chk("w2", rd32(POOL + 0x420u + 32u), 0x33u);
    chk("canary", rd32(CANARY), 0x11111111u);

    wr32(COUNT, 80u);
    rc = wm_8009623C(0xAAu, 0xBBu, 0xCCu);
    chk("mid.rc", (u32)rc, 0u);
    chk("mid.count", rd32(COUNT), 81u);
    chk("mid.w0", rd32(POOL + 0x420u + 80u * 12u), 0xAAu);

    wr32(COUNT, 88u);
    wr32(POOL + 0x420u + 88u * 12u, 0xDEADu);
    rc = wm_8009623C(0x44u, 0x55u, 0x66u);
    chk("full.rc", (u32)rc, 0xFFFFFFFFu);
    chk("full.count", rd32(COUNT), 88u);
    chk("full.slot", rd32(POOL + 0x420u + 88u * 12u), 0xDEADu);

    if (s_failures) {
        fprintf(stderr, "ASSERTION failures=%d\n", s_failures);
        return 1;
    }
    printf("W34B24-I32 0x8009623C focused oracle PASS\n");
    return 0;
}
