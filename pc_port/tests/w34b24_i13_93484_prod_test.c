/*
 * Focused production-linked oracle for retail wrap helper 0x80093484.
 *
 * Expected values are hand-derived from [0x80093484, 0x80093534).
 * NEXT_CALLBACK_TARGET is not an oracle input.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_93484.h"

#define VEC     0x1F800100u
#define G_WRAPX 0x8009D160u
#define G_WRAPZ 0x8009D2B4u

static int s_failures;
static u32 s_stores;

static void chk(const char* n, u32 got, u32 want)
{
    if (got != want) {
        fprintf(stderr, "ASSERTION %s: got=0x%08x expected=0x%08x\n", n, got,
                want);
        if (++s_failures > 40)
            exit(1);
    }
}

static u32 rd32(u32 a) { u32 v; memcpy(&v, PSX_ADDR(a), 4); return v; }
static void wr32(u32 a, u32 v) { memcpy(PSX_ADDR(a), &v, 4); }

void wm_93484_test_store(u32 address, u32 value)
{
    (void)address;
    (void)value;
    s_stores++;
}

static void reset(void)
{
    s_stores = 0;
    wr32(VEC, 0u);
    wr32(VEC + 4u, 0xDEADBEEFu);
    wr32(VEC + 8u, 0u);
    wr32(G_WRAPX, 1u);
    wr32(G_WRAPZ, 2u);
}

int main(void)
{
    PsxMemory_Init();

    /* period_x = 1<<23 = 0x800000; period_z = 2<<23 = 0x1000000 */

    reset();
    wr32(VEC, 0xFBFFFFFFu); /* < 0xFC000000 */
    wm_80093484(VEC);
    chk("xlo.x", rd32(VEC), 0xFBFFFFFFu + 0x800000u);
    chk("xlo.y", rd32(VEC + 4u), 0xDEADBEEFu);
    chk("xlo.z", rd32(VEC + 8u), 0u);

    reset();
    wr32(VEC, 0xFC000000u); /* equal: no low wrap */
    wm_80093484(VEC);
    chk("xeq.x", rd32(VEC), 0xFC000000u);
    chk("xeq.nstore", s_stores, 0u);

    reset();
    wr32(VEC, 0x04000001u);
    wm_80093484(VEC);
    chk("xhi.x", rd32(VEC), 0x04000001u - 0x800000u);

    reset();
    wr32(VEC, 0x04000000u);
    wm_80093484(VEC);
    chk("xhi_eq.x", rd32(VEC), 0x04000000u);

    reset();
    wr32(VEC + 8u, 0xFBFFFFFFu);
    wm_80093484(VEC);
    chk("zlo.z", rd32(VEC + 8u), 0xFBFFFFFFu + 0x1000000u);
    chk("zlo.x", rd32(VEC), 0u);

    reset();
    wr32(VEC + 8u, 0x04000001u);
    wm_80093484(VEC);
    chk("zhi.z", rd32(VEC + 8u), 0x04000001u - 0x1000000u);

    reset();
    wr32(VEC, 0u);
    wr32(VEC + 8u, 0u);
    wm_80093484(VEC);
    chk("zero.x", rd32(VEC), 0u);
    chk("zero.z", rd32(VEC + 8u), 0u);
    chk("zero.y", rd32(VEC + 4u), 0xDEADBEEFu);
    chk("zero.nstore", s_stores, 0u);

    reset();
    wr32(VEC, 0xFBFFFFFFu);
    wr32(VEC + 8u, 0x04000001u);
    wm_80093484(VEC);
    chk("both.x", rd32(VEC), 0xFBFFFFFFu + 0x800000u);
    chk("both.z", rd32(VEC + 8u), 0x04000001u - 0x1000000u);
    chk("both.y", rd32(VEC + 4u), 0xDEADBEEFu);

    if (s_failures != 0) {
        fprintf(stderr, "%d assertion(s) failed\n", s_failures);
        return 1;
    }
    printf("W34B24-I13 0x80093484 focused oracle PASS\n");
    return 0;
}
