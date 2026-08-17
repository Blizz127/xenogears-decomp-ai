/*
 * Focused production-linked oracle for retail helper 0x80091FF8.
 *
 * Expected values are hand-derived from [0x80091FF8, 0x80092234).
 * 96F18 / 93354 / 93660 are seam-forced. NEXT_CALLBACK_TARGET is not
 * an oracle input.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_91ff8.h"

#define HEAD    0x800D0100u
#define HGT     0x800D0120u
#define CELL    0x800D0200u
#define ANG     0x1F8000A8u
#define SCR     0x1F800000u

#define G_BD3A  0x8009BD3Au
#define G_BD3C  0x8009BD3Cu
#define G_BD40  0x8009BD40u
#define G_BE28  0x8009BE28u
#define G_BE30  0x8009BE30u
#define G_BCDC  0x8009BCDCu
#define G_B214  0x8009B214u
#define G_B234  0x8009B234u
#define G_D560  0x8009D560u

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

static void wr32(u32 a, u32 v) { memcpy(PSX_ADDR(a), &v, 4); }
static void wr16(u32 a, u16 v) { memcpy(PSX_ADDR(a), &v, 2); }
static void wr8(u32 a, u8 v) { memcpy(PSX_ADDR(a), &v, 1); }

static int s_n96;
static int s_n54;
static int s_n60;
static s32 s_scale[4];
static u32 s_dest[4];
static u32 s_ang[4];
static s16 s_ang_vy;
static s16 s_ang_vz;
static s32 s_first_x;
static s32 s_first_z;
static s32 s_second_x;

void wm_91ff8_test_96f18(u32 dest, u32 pose, s32 scale, u32 angles)
{
    (void)pose;
    if (s_n96 < 4) {
        s_scale[s_n96] = scale;
        s_dest[s_n96] = dest;
        s_ang[s_n96] = angles;
    }
    if (s_n96 == 0) {
        memcpy(&s_ang_vy, PSX_ADDR(angles + 2u), 2);
        memcpy(&s_ang_vz, PSX_ADDR(angles + 4u), 2);
    }
    wr16(dest, 0x0180u);
    wr16(dest + 4u, (u16)(s16)-0x180);
    s_n96++;
}

void wm_91ff8_test_93354(u32 vec)
{
    (void)vec;
    s_n54++;
}

u32 wm_91ff8_test_93660(s32 x, s32 z)
{
    if (s_n60 == 0) {
        s_first_x = x;
        s_first_z = z;
    } else if (s_n60 == 1) {
        s_second_x = x;
    }
    s_n60++;
    return CELL;
}

static void reset_calls(void)
{
    s_n96 = 0;
    s_n54 = 0;
    s_n60 = 0;
    memset(s_scale, 0, sizeof(s_scale));
    memset(s_dest, 0, sizeof(s_dest));
    memset(s_ang, 0, sizeof(s_ang));
    s_ang_vy = 0;
    s_ang_vz = 0;
    s_first_x = 0;
    s_first_z = 0;
    s_second_x = 0;
}

static void base_globals(void)
{
    wr16(G_BD3A, 0x0A00u);
    wr16(G_BD3C, 0x0B00u);
    wr32(G_D560, 0u);
    wr32(G_BCDC, 0u);
    wr32(G_BE28, 0x00101234u);
    wr32(G_BE30, 0x00204567u);
    wr32(G_B214, 0x00003000u);
    wr32(G_B214 + 4u, 0x00004000u);
    wr32(G_B214 + 8u, 0x00005000u);
    wr16(G_B234, 0u);
    wr16(G_B234 + 2u, 0u);
    wr16(G_B234 + 4u, (u16)(s16)-0x10);
    wr16(HEAD, 0x0001u);
    wr16(HEAD + 2u, 0x0002u);
    wr16(HEAD + 4u, 0x0003u);
    wr16(HGT, 0u);
    wr16(HGT + 2u, 0u);
    wr16(HGT + 4u, 0u);
    wr8(CELL, 0u);
    wr16(G_BD40, 0u);
    wr16(G_BD40 + 4u, 0u);
    wr16(ANG, 0xDEADu);
    wr16(ANG + 2u, 0xBEEFu);
    wr16(ANG + 4u, 0xCAFEu);
}

static void run_walk3(void)
{
    s32 ret;

    reset_calls();
    base_globals();
    wr32(G_BCDC, (u32)(s32)-5);

    ret = wm_80091FF8(2, HEAD, HGT);
    chk("w3.ret", (u32)ret, 3u);
    chk("w3.n96", (u32)s_n96, 3u);
    chk("w3.n54", (u32)s_n54, 108u);
    chk("w3.n60", (u32)s_n60, 108u);
    chk("w3.dest0", s_dest[0], G_BD40);
    chk("w3.ang0", s_ang[0], ANG);
    chk("w3.vy", (u32)(u16)s_ang_vy, 0x0A00u);
    chk("w3.vz", (u32)(u16)s_ang_vz, 0x0B00u);
    /* BCDC=-5 → toward-zero /2 = -2 → <<12 = -0x2000; scale = 0x3000 - (-0x2000) */
    chk("w3.scale0", (u32)s_scale[0], 0x5000u);
    chk("w3.x0", (u32)s_first_x, 0x00100000u);
    chk("w3.z0", (u32)s_first_z, 0x00200000u);
    chk("w3.x1", (u32)s_second_x, 0x00180000u);
}

static void run_snap(void)
{
    s32 ret;

    reset_calls();
    base_globals();
    wr16(HGT, (u16)(s16)-0x60);

    ret = wm_80091FF8(2, HEAD, HGT);
    chk("snap.ret", (u32)ret, 2u);
    chk("snap.n96", (u32)s_n96, 1u);
    chk("snap.n60", (u32)s_n60, 36u);
}

static void run_nosnap(void)
{
    s32 ret;

    reset_calls();
    base_globals();
    wr16(HGT, (u16)(s16)-0x60);
    wr16(G_B234 + 4u, 0x1000u);

    ret = wm_80091FF8(2, HEAD, HGT);
    chk("ns.ret", (u32)ret, 0u);
    chk("ns.n96", (u32)s_n96, 1u);
}

int main(void)
{
    PsxMemory_Init();
    run_walk3();
    run_snap();
    run_nosnap();

    if (s_failures != 0) {
        fprintf(stderr, "%d assertion(s) failed\n", s_failures);
        return 1;
    }
    printf("W34B24-I16 0x80091FF8 focused oracle PASS\n");
    return 0;
}
