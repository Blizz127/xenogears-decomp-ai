/*
 * Focused production-linked oracle for retail helper 0x800967E4.
 *
 * Expected values are hand-derived from [0x800967E4, 0x800968E0).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_967e4.h"

#define RING   0x8009BCB8u
#define D788   0x8009D788u
#define C624   0x8009C624u
#define CANARY 0x8009B000u

static int s_failures;
static u32 s_dbg[2];
static int s_dbg_i;
static int s_e0_calls;
static s32 s_e0_rc;
static int s_99c_calls;
static u32 s_99c_a0;
static int s_6cc_calls;
static u32 s_6cc_a0;

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

void wm_967e4_test_store(u32 address, u32 value)
{
    (void)address;
    (void)value;
}

u32 func_8002C3D8(void)
{
    u32 v = s_dbg[s_dbg_i];
    if (s_dbg_i < 1)
        s_dbg_i += 1;
    return v;
}

s32 wm_800968E0(void)
{
    s_e0_calls += 1;
    return s_e0_rc;
}

void wm_8009699C(u32 record)
{
    s_99c_calls += 1;
    s_99c_a0 = record;
}

void wm_800966CC(u32 list)
{
    s_6cc_calls += 1;
    s_6cc_a0 = list;
}

static void reset_calls(void)
{
    s_dbg_i = 0;
    s_e0_calls = 0;
    s_99c_calls = 0;
    s_6cc_calls = 0;
    s_99c_a0 = 0;
    s_6cc_a0 = 0;
}

int main(void)
{
    s32 rc;

    PsxMemory_Init();
    wr32(CANARY, 0x11111111u);
    wr32(RING, 1u);
    wr32(D788 + 4u, 0x80090100u);
    wr32(C624 + 4u, 0x80090200u);

    s_dbg[0] = 0;
    s_dbg[1] = 0;
    s_e0_rc = 0;
    reset_calls();
    rc = wm_800967E4();
    chk("cd.rc", (u32)rc, 0u);
    chk("cd.e0", (u32)s_e0_calls, 1u);
    chk("cd.99c", (u32)s_99c_calls, 1u);
    chk("cd.99a", s_99c_a0, 0x80090100u);
    chk("cd.6cc", (u32)s_6cc_calls, 0u);

    s_e0_rc = 2;
    reset_calls();
    rc = wm_800967E4();
    chk("busy.rc", (u32)rc, 2u);
    chk("busy.e0", (u32)s_e0_calls, 1u);
    chk("busy.99c", (u32)s_99c_calls, 0u);

    s_dbg[0] = 5;
    s_dbg[1] = 5;
    s_e0_rc = 0;
    reset_calls();
    rc = wm_800967E4();
    chk("pc.rc", (u32)rc, 0u);
    chk("pc.e0", (u32)s_e0_calls, 0u);
    chk("pc.6cc", (u32)s_6cc_calls, 1u);
    chk("pc.6a", s_6cc_a0, 0x80090200u);
    chk("pc.tab", rd32(C624 + 4u), 0u);
    chk("pc.ring", rd32(RING), 2u);

    wr32(RING, 1u);
    wr32(C624 + 4u, 0x80090200u);
    s_dbg[0] = 0;
    s_dbg[1] = 0xFFFFFFFFu;
    reset_calls();
    rc = wm_800967E4();
    chk("ff.e0", (u32)s_e0_calls, 1u);
    chk("ff.6cc", (u32)s_6cc_calls, 0u);
    chk("canary", rd32(CANARY), 0x11111111u);

    if (s_failures) {
        fprintf(stderr, "ASSERTION failures=%d\n", s_failures);
        return 1;
    }
    printf("W34B24-I41 0x800967E4 focused oracle PASS\n");
    return 0;
}
