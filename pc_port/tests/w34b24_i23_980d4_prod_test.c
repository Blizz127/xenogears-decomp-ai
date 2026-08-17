/*
 * Focused production-linked oracle for retail helper 0x800980D4.
 *
 * Expected values are hand-derived from [0x800980D4, 0x800981C8).
 * NEXT_CALLBACK_TARGET is not an oracle input.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_980d4.h"

#define POS     0x8009BBB4u
#define FLAGS   0x8009D558u
#define CELL_X  0x8009C838u
#define CELL_Z  0x8009C83Cu

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

static void setup(s32 x, s32 z)
{
    memset(PSX_ADDR(0x8009BBB0u), 0xCC, 0x20);
    memset(PSX_ADDR(0x8009C830u), 0xEE, 0x10);
    wr16(FLAGS, 0xFFFFu);
    wr16(CELL_X, 0xBEEFu);
    wr16(CELL_Z, 0xBEEFu);
    wr32(POS, (u32)x);
    wr32(POS + 4u, 0x11111111u);
    wr32(POS + 8u, (u32)z);
}

static void expect(const char *tag, s32 x, s32 z, u32 flags, u32 cx, u32 cz)
{
    char n[64];

    snprintf(n, sizeof n, "%s.x", tag);
    chk(n, rd32(POS), (u32)x);
    snprintf(n, sizeof n, "%s.y", tag);
    chk(n, rd32(POS + 4u), 0x11111111u);
    snprintf(n, sizeof n, "%s.z", tag);
    chk(n, rd32(POS + 8u), (u32)z);
    snprintf(n, sizeof n, "%s.flags", tag);
    chk(n, rd16(FLAGS), flags);
    snprintf(n, sizeof n, "%s.cx", tag);
    chk(n, (u32)(s16)rd16(CELL_X), cx);
    snprintf(n, sizeof n, "%s.cz", tag);
    chk(n, (u32)(s16)rd16(CELL_Z), cz);
}

int main(void)
{
    PsxMemory_Init();

    setup(0, 0);
    wm_800980D4(POS);
    expect("zero", 0, 0, 0u, 2u, 2u);

    setup((s32)0xFF7FFFFF, 0);
    wm_800980D4(POS);
    expect("xlo", -1, 0, 4u, 1u, 2u);

    setup((s32)0x00800001, 0);
    wm_800980D4(POS);
    expect("xhi", 1, 0, 8u, 2u, 2u);

    setup(0, (s32)0xFF7FFFFF);
    wm_800980D4(POS);
    expect("zlo", 0, -1, 1u, 2u, 1u);

    setup(0, (s32)0x00800001);
    wm_800980D4(POS);
    expect("zhi", 0, 1, 2u, 2u, 2u);

    setup((s32)0xFF7FFFFF, (s32)0x00800001);
    wm_800980D4(POS);
    expect("both", -1, 1, 6u, 1u, 2u);

    setup((s32)0xFF800000, (s32)0x00800000);
    wm_800980D4(POS);
    expect("edge", (s32)0xFF800000, (s32)0x00800000, 0u, 1u, 3u);

    setup((s32)0x00400000, (s32)0x00400000);
    wm_800980D4(POS);
    expect("mid", (s32)0x00400000, (s32)0x00400000, 0u, 2u, 2u);

    setup((s32)0xFFA00000, 0);
    wm_800980D4(POS);
    expect("negmid", (s32)0xFFA00000, 0, 0u, 1u, 2u);

    if (s_failures) {
        fprintf(stderr, "ASSERTION failures=%d\n", s_failures);
        return 1;
    }
    printf("W34B24-I23 0x800980D4 focused oracle PASS\n");
    return 0;
}
