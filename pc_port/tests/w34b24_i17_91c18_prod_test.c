/*
 * Focused production-linked oracle for retail callback 0x80091C18.
 *
 * Expected values are hand-derived from [0x80091C18, 0x80091FF8).
 * 91FF8 / 96F18 are seam-forced. NEXT_CALLBACK_TARGET is not an
 * oracle input.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_callback_91c18.h"

#define POOL_GUEST 0x800D7538u
#define SLOT10     (POOL_GUEST + 0x500u)
#define SLOT9      (POOL_GUEST + 0x480u)
#define SLOT11     (POOL_GUEST + 0x580u)

#define G_POOL  0x8009BE24u
#define G_D144  0x8009D144u
#define G_D3F0  0x8009D3F0u
#define G_B214  0x8009B214u
#define G_B224  0x8009B224u
#define G_B22C  0x8009B22Cu
#define G_B234  0x8009B234u
#define G_B23C  0x8009B23Cu
#define G_BD38  0x8009BD38u
#define G_BD48  0x8009BD48u
#define G_BD4A  0x8009BD4Au
#define G_BD4C  0x8009BD4Cu
#define G_BE2C  0x8009BE2Cu

static int s_failures;

static void chk(const char *n, u32 got, u32 want)
{
    if (got != want) {
        fprintf(stderr, "ASSERTION %s: got=0x%08x expected=0x%08x\n", n, got,
                want);
        if (++s_failures > 50)
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

static int s_nff8;
static int s_n618;
static s32 s_ff8_ret = 1;
static s32 s_ff8_a0;
static u32 s_ff8_a1;
static u32 s_ff8_a2;

void wm_91c18_test_store(u32 address, u32 width, u32 value)
{
    (void)address;
    (void)width;
    (void)value;
}

s32 wm_91c18_test_91ff8(s32 threshold, u32 headings, u32 heights)
{
    s_ff8_a0 = threshold;
    s_ff8_a1 = headings;
    s_ff8_a2 = heights;
    s_nff8++;
    return s_ff8_ret;
}

void wm_91c18_test_96f18(u32 dest, u32 pose, s32 scale, u32 angles)
{
    (void)dest;
    (void)pose;
    (void)scale;
    (void)angles;
    s_n618++;
}

static void reset(void)
{
    s_nff8 = 0;
    s_n618 = 0;
    s_ff8_ret = 1;
    wr32(G_POOL, POOL_GUEST);
    wr32(SLOT9, 0x5A5A5A5Au);
    wr32(SLOT11, 0xA5A5A5A5u);
    wr16(SLOT10 + 4u, 0);
    wr16(SLOT10 + 0x20u, 0);
    wr16(SLOT10 + 0x22u, 0);
    wr32(SLOT10 + 0x50u, 0);
    wr32(SLOT10 + 0x54u, 0);
    wr32(SLOT10 + 0x58u, 0);
    wr32(SLOT10 + 0x60u, 0);
    wr32(SLOT10 + 0x64u, G_B22C);
    wr32(SLOT10 + 0x68u, G_B23C);
    wr32(G_D144, 0x11111111u);
    wr32(G_D3F0, 0);
    wr16(G_BD38, 0);
    wr16(G_BD48, 0xDEADu);
    wr16(G_BD4A, 0xBEEFu);
    wr16(G_BD4C, 0xCAFEu);
    wr32(G_BE2C, 0x12345000u);
    wr32(G_B214, 0x1000u);
    wr32(G_B214 + 4u, 0x2000u);
    wr32(G_B214 + 8u, 0x3000u);
    wr16(G_B22C, 0x10u);
    wr16(G_B22C + 2u, 0x20u);
    wr16(G_B22C + 4u, 0x30u);
    wr16(G_B224, 0x40u);
}

static s32 run(void)
{
    return wm_80091C18(10);
}

int main(void)
{
    s32 r;

    PsxMemory_Init();

    reset();
    wr16(SLOT10 + 4u, 9u);
    wr32(SLOT10 + 0x50u, 0u);
    s_ff8_ret = 0;
    r = run();
    chk("s9.ret", (u32)r, 1u);
    chk("s9.sub", (u32)rd16(SLOT10 + 4u), 0u);
    chk("s9.64", rd32(SLOT10 + 0x64u), G_B22C);
    chk("s9.68", rd32(SLOT10 + 0x68u), G_B23C);
    chk("s9.50", rd32(SLOT10 + 0x50u), 0u);
    chk("s9.n96", (u32)s_n618, 1u);
    chk("s9.iso9", rd32(SLOT9), 0x5A5A5A5Au);
    chk("s9.iso11", rd32(SLOT11), 0xA5A5A5A5u);

    reset();
    wr16(SLOT10 + 4u, 0xAu);
    r = run();
    chk("sa.64", rd32(SLOT10 + 0x64u), G_B224);
    chk("sa.68", rd32(SLOT10 + 0x68u), G_B234);
    chk("sa.50", rd32(SLOT10 + 0x50u), 1u);

    reset();
    wr16(SLOT10 + 4u, 0xEu);
    wr16(SLOT10 + 0x20u, 4u);
    wr32(SLOT10 + 0x50u, 1u);
    s_ff8_ret = 1;
    r = run();
    chk("se.state", (u32)rd16(SLOT10 + 0x20u), 0u);
    chk("se.d144", rd32(G_D144), 0u);
    chk("se.n96", (u32)s_n618, 1u);
    chk("se.22", (u32)rd16(SLOT10 + 0x22u), 1u);

    reset();
    wr16(SLOT10 + 4u, 0x11u);
    wr16(G_BD38, 0u);
    wr32(SLOT10 + 0x58u, 0u);
    r = run();
    chk("s11.d3f0", rd32(G_D3F0), 0x00400000u);
    chk("s11.state", (u32)rd16(SLOT10 + 0x20u), 3u);
    chk("s11.58", rd32(SLOT10 + 0x58u), (u32)(s32)-0x100);
    chk("s11.bd38", (u32)rd16(G_BD38), 2u);
    chk("s11.n96", (u32)s_n618, 1u);

    reset();
    wr16(SLOT10 + 0x20u, 0u);
    wr32(SLOT10 + 0x50u, 0u);
    s_ff8_ret = 1;
    r = run();
    chk("s0.ret", (u32)r, 1u);
    chk("s0.state", (u32)rd16(SLOT10 + 0x20u), 1u);
    chk("s0.50", rd32(SLOT10 + 0x50u), 1u);
    chk("s0.54", rd32(SLOT10 + 0x54u), 0x2000u);
    chk("s0.58", rd32(SLOT10 + 0x58u), 0x20u);
    chk("s0.22", (u32)rd16(SLOT10 + 0x22u), 1u);
    chk("s0.nff8", (u32)s_nff8, 1u);
    chk("s0.n96", (u32)s_n618, 1u);

    reset();
    wr16(SLOT10 + 0x20u, 1u);
    wr16(SLOT10 + 0x22u, 3u);
    wr32(SLOT10 + 0x54u, 0x40000u);
    wr32(G_D3F0, 0u);
    wr32(SLOT10 + 0x58u, 0u);
    wr16(G_BD38, 0u);
    r = run();
    chk("s1.nff8", (u32)s_nff8, 0u);
    chk("s1.d3f0", rd32(G_D3F0), 0x8000u); /* min(0x40000,0x40000)>>3 */
    chk("s1.22", (u32)rd16(SLOT10 + 0x22u), 4u);
    chk("s1.n96", (u32)s_n618, 1u);

    reset();
    wr16(SLOT10 + 0x20u, 1u);
    wr16(SLOT10 + 0x22u, 4u);
    wr32(SLOT10 + 0x50u, 1u);
    wr32(SLOT10 + 0x54u, 0u);
    wr32(G_D3F0, 0u);
    wr32(SLOT10 + 0x58u, 0u);
    wr16(G_BD38, 0u);
    r = run();
    chk("s1b.nff8", (u32)s_nff8, 0u);
    chk("s1b.state", (u32)rd16(SLOT10 + 0x20u), 0u);

    reset();
    wr16(SLOT10 + 0x20u, 1u);
    wr16(SLOT10 + 0x22u, 5u);
    wr32(SLOT10 + 0x50u, 0u);
    s_ff8_ret = 1;
    wr32(SLOT10 + 0x54u, 0u);
    wr32(G_D3F0, 0u);
    wr32(SLOT10 + 0x58u, 0u);
    wr16(G_BD38, 0u);
    r = run();
    chk("s1c.nff8", (u32)s_nff8, 1u);
    chk("s1c.a0", (u32)s_ff8_a0, 0u);
    chk("s1c.22", (u32)rd16(SLOT10 + 0x22u), 1u);

    reset();
    wr16(SLOT10 + 0x20u, 2u);
    r = run();
    chk("s2.bd48", (u32)rd16(G_BD48), 0u);
    chk("s2.bd4c", (u32)rd16(G_BD4C), 0u);
    chk("s2.bd4a", (u32)rd16(G_BD4A), 0x2345u);
    chk("s2.n96", (u32)s_n618, 0u);
    chk("s2.22", (u32)rd16(SLOT10 + 0x22u), 1u);

    reset();
    wr16(SLOT10 + 0x20u, 1u);
    wr32(SLOT10 + 0x54u, 0x100u);
    wr32(G_D3F0, 0x2100u);
    wr32(SLOT10 + 0x58u, 0u);
    wr16(G_BD38, 0u);
    r = run();
    chk("neg.d3f0", rd32(G_D3F0), 0x100u); /* 0x2100 - 0x2000 */

    reset();
    wr16(SLOT10 + 0x20u, 1u);
    wr32(SLOT10 + 0x50u, 0u);
    wr32(SLOT10 + 0x54u, 0u);
    wr32(G_D3F0, 0u);
    wr32(SLOT10 + 0x58u, 0x20u);
    wr16(G_BD38, 0u);
    wr32(SLOT10 + 0x60u, 0u);
    r = run();
    chk("hd.60", rd32(SLOT10 + 0x60u), 0xF32u);

    if (s_failures != 0) {
        fprintf(stderr, "%d assertion(s) failed\n", s_failures);
        return 1;
    }
    printf("W34B24-I17 0x80091C18 focused oracle PASS\n");
    return 0;
}
