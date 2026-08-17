/*
 * Focused production-linked oracle for retail helper 0x80096130.
 *
 * Expected values are hand-derived from [0x80096130, 0x8009623C).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_96130.h"

#define INDEX  0x8009BE44u
#define D788   0x8009D788u
#define C624   0x8009C624u
#define CANARY 0x8009B000u

static int s_failures;
static u32 s_dbg[2];
static int s_dbg_i;
static int s_vsync;
static int s_step;

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

u32 func_8002C3D8(void)
{
    u32 v = s_dbg[s_dbg_i];
    if (s_dbg_i < 1)
        s_dbg_i += 1;
    return v;
}

int Vsync(int mode)
{
    chk("vsync.mode", (u32)mode, 0u);
    s_vsync += 1;
    return 0;
}

s32 wm_800967E4(void)
{
    s_step += 1;
    if (s_step >= 2) {
        wr32(D788 + rd32(INDEX) * 4u, 0u);
        wr32(C624 + rd32(INDEX) * 4u, 0u);
    }
    return 0;
}

static void reset_calls(void)
{
    s_dbg_i = 0;
    s_vsync = 0;
    s_step = 0;
}

int main(void)
{
    PsxMemory_Init();
    wr32(CANARY, 0x11111111u);
    wr32(INDEX, 1u);

    s_dbg[0] = 0;
    s_dbg[1] = 0;
    wr32(D788 + 4u, 0u);
    wr32(C624 + 4u, 0u);
    reset_calls();
    wm_80096130();
    chk("empty.vs", (u32)s_vsync, 0u);
    chk("empty.st", (u32)s_step, 0u);

    wr32(D788 + 4u, 0x80090100u);
    reset_calls();
    wm_80096130();
    chk("cd.vs", (u32)s_vsync, 2u);
    chk("cd.st", (u32)s_step, 2u);
    chk("cd.slot", rd32(D788 + 4u), 0u);

    s_dbg[0] = 5;
    s_dbg[1] = 5;
    wr32(C624 + 4u, 0x80090200u);
    wr32(D788 + 4u, 0u);
    reset_calls();
    wm_80096130();
    chk("pc.vs", (u32)s_vsync, 2u);
    chk("pc.st", (u32)s_step, 2u);
    chk("pc.slot", rd32(C624 + 4u), 0u);
    chk("canary", rd32(CANARY), 0x11111111u);

    if (s_failures) {
        fprintf(stderr, "ASSERTION failures=%d\n", s_failures);
        return 1;
    }
    printf("W34B24-I42 0x80096130 focused oracle PASS\n");
    return 0;
}
