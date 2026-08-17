/*
 * Focused production-linked oracle for retail world callback 0x800907F4.
 *
 * Expected values are hand-derived from the retail disassembly of
 * [0x800907F4, 0x80090A18).  RotMatrixZYX is seam-forced.
 * NEXT_CALLBACK_TARGET is a runtime measurement, not an oracle input.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_callback_907f4.h"

#define POOL_GUEST   0x800D7538u
#define SLOT8        (POOL_GUEST + 0x400u)
#define CTX          0x800E0000u
#define REC2         (CTX + 0xA8u)
#define REC3         (CTX + 0xFCu)
#define SC           0x1F800000u

#define G_POOL_PTR   0x8009BE24u
#define G_CTX_PTR    0x8009C620u

static int s_failures;

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
static u16 rd16(u32 a) { u16 v; memcpy(&v, PSX_ADDR(a), 2); return v; }
static void wr16(u32 a, u16 v) { memcpy(PSX_ADDR(a), &v, 2); }

typedef struct {
    char tag[8];
    u32 a[4];
} Call;

static Call s_calls[16];
static u32 s_ncalls;

static void log_call(const char* tag, u32 a0, u32 a1)
{
    if (s_ncalls < 16u) {
        snprintf(s_calls[s_ncalls].tag, sizeof(s_calls[s_ncalls].tag), "%s",
                 tag);
        s_calls[s_ncalls].a[0] = a0;
        s_calls[s_ncalls].a[1] = a1;
    }
    s_ncalls++;
}

static const Call* find_call(const char* tag, u32 nth)
{
    u32 i, seen = 0;

    for (i = 0; i < s_ncalls && i < 16u; i++)
        if (strcmp(s_calls[i].tag, tag) == 0) {
            if (seen == nth)
                return &s_calls[i];
            seen++;
        }
    return 0;
}

void wm_907f4_test_store(u32 address, u32 width, u32 value)
{
    (void)address;
    (void)width;
    (void)value;
}

void wm_907f4_test_rotmatrixzyx(u32 vec, u32 mat)
{
    log_call("rotzyx", vec, mat);
}

static void reset(void)
{
    u32 i;

    s_ncalls = 0;
    wr32(G_POOL_PTR, POOL_GUEST);
    wr32(G_CTX_PTR, CTX);
    for (i = 0; i < 0x80u; i += 4u)
        wr32(SLOT8 + i, 0u);
    for (i = 0; i < 0x200u; i += 4u)
        wr32(CTX + i, 0u);
    wr16(REC2 + 0x1Cu, 0x111u);
    wr16(REC3 + 0x1Cu, 0x222u);
    wr32(SLOT8 + 0x50u, 0x10u);
    wr32(SLOT8 + 0x54u, 0x40u);
}

static s32 run(void)
{
    return wm_800907F4(8);
}

int main(void)
{
    s32 r;
    const Call* c;

    PsxMemory_Init();

    /* ---- substate 9 -> state 1, then one ramp-up step ---- */
    reset();
    wr16(SLOT8 + 4u, 9u);
    wr16(SLOT8 + 0x20u, 0u);
    r = run();
    chk("sub9.ret", (u32)r, 1u);
    chk("sub9.sub", rd16(SLOT8 + 4u), 0u);
    chk("sub9.state", rd16(SLOT8 + 0x20u), 1u);
    chk("sub9.58", rd32(SLOT8 + 0x58u), 4u);
    chk("sub9.5c", rd32(SLOT8 + 0x5Cu), 0u);
    chk("sub9.50", rd32(SLOT8 + 0x50u), 0x14u);
    chk("sub9.54", rd32(SLOT8 + 0x54u), 0x40u);
    chk("sub9.nrot", s_ncalls, 2u);
    c = find_call("rotzyx", 0);
    chk("sub9.rot0.vec", c ? c->a[0] : 0u, SC + 0xA0u);
    chk("sub9.rot0.mat", c ? c->a[1] : 0u, REC2 + 0x20u);
    c = find_call("rotzyx", 1);
    chk("sub9.rot1.vec", c ? c->a[0] : 0u, SC + 0xA8u);
    chk("sub9.rot1.mat", c ? c->a[1] : 0u, REC3 + 0x20u);
    chk("sub9.vy0", rd16(SC + 0xA2u), 0x14u);
    chk("sub9.vz0", rd16(SC + 0xA4u), 0x111u);
    chk("sub9.vy1", rd16(SC + 0xAAu), 0x40u);
    chk("sub9.vz1", rd16(SC + 0xACu), 0x222u);

    /* ---- substate 0xA -> state 2 ---- */
    reset();
    wr16(SLOT8 + 4u, 0xAu);
    wr32(SLOT8 + 0x58u, 0x20u);
    wr32(SLOT8 + 0x5Cu, 0x20u);
    r = run();
    chk("suba.state", rd16(SLOT8 + 0x20u), 2u);
    chk("suba.58", rd32(SLOT8 + 0x58u), 0x1Cu);
    chk("suba.5c", rd32(SLOT8 + 0x5Cu), 0x1Cu);

    /* ---- state 0 zeros 58/5C ---- */
    reset();
    wr16(SLOT8 + 0x20u, 0u);
    wr32(SLOT8 + 0x58u, 0x55u);
    wr32(SLOT8 + 0x5Cu, 0x66u);
    r = run();
    chk("s0.ret", (u32)r, 1u);
    chk("s0.58", rd32(SLOT8 + 0x58u), 0u);
    chk("s0.5c", rd32(SLOT8 + 0x5Cu), 0u);
    chk("s0.50", rd32(SLOT8 + 0x50u), 0x10u);
    chk("s0.54", rd32(SLOT8 + 0x54u), 0x40u);

    /* ---- state 1: 5C lags until 58 >= 0x11 ---- */
    reset();
    wr16(SLOT8 + 0x20u, 1u);
    wr32(SLOT8 + 0x58u, 0xCu);
    wr32(SLOT8 + 0x5Cu, 0u);
    r = run();
    chk("s1a.58", rd32(SLOT8 + 0x58u), 0x10u);
    chk("s1a.5c", rd32(SLOT8 + 0x5Cu), 0u);

    reset();
    wr16(SLOT8 + 0x20u, 1u);
    wr32(SLOT8 + 0x58u, 0x10u);
    wr32(SLOT8 + 0x5Cu, 0u);
    r = run();
    chk("s1b.58", rd32(SLOT8 + 0x58u), 0x14u);
    chk("s1b.5c", rd32(SLOT8 + 0x5Cu), 4u);

    /* ---- state 1 clamp + advance to 3 ---- */
    reset();
    wr16(SLOT8 + 0x20u, 1u);
    wr32(SLOT8 + 0x58u, 0x7Cu);
    wr32(SLOT8 + 0x5Cu, 0x7Cu);
    r = run();
    chk("s1c.58", rd32(SLOT8 + 0x58u), 0x80u);
    chk("s1c.5c", rd32(SLOT8 + 0x5Cu), 0x80u);
    chk("s1c.state", rd16(SLOT8 + 0x20u), 3u);
    chk("s1c.50", rd32(SLOT8 + 0x50u), 0x10u + 0x80u);
    chk("s1c.54", rd32(SLOT8 + 0x54u), 0x40u - 0x80u);

    /* ---- state 2: 5C lags until 58 < 0x70 ---- */
    reset();
    wr16(SLOT8 + 0x20u, 2u);
    wr32(SLOT8 + 0x58u, 0x74u);
    wr32(SLOT8 + 0x5Cu, 0x80u);
    r = run();
    chk("s2a.58", rd32(SLOT8 + 0x58u), 0x70u);
    chk("s2a.5c", rd32(SLOT8 + 0x5Cu), 0x80u);

    reset();
    wr16(SLOT8 + 0x20u, 2u);
    wr32(SLOT8 + 0x58u, 0x70u);
    wr32(SLOT8 + 0x5Cu, 0x80u);
    r = run();
    chk("s2b.58", rd32(SLOT8 + 0x58u), 0x6Cu);
    chk("s2b.5c", rd32(SLOT8 + 0x5Cu), 0x7Cu);

    reset();
    wr16(SLOT8 + 0x20u, 2u);
    wr32(SLOT8 + 0x58u, 0x73u);
    wr32(SLOT8 + 0x5Cu, 0x80u);
    r = run();
    chk("s2e.58", rd32(SLOT8 + 0x58u), 0x6Fu);
    chk("s2e.5c", rd32(SLOT8 + 0x5Cu), 0x7Cu);

    /* ---- state 2 floor + advance to 0 ---- */
    reset();
    wr16(SLOT8 + 0x20u, 2u);
    wr32(SLOT8 + 0x58u, 4u);
    wr32(SLOT8 + 0x5Cu, 4u);
    r = run();
    chk("s2c.58", rd32(SLOT8 + 0x58u), 0u);
    chk("s2c.5c", rd32(SLOT8 + 0x5Cu), 0u);
    chk("s2c.state", rd16(SLOT8 + 0x20u), 0u);

    /* ---- state 2 floors a negative 58 ---- */
    reset();
    wr16(SLOT8 + 0x20u, 2u);
    wr32(SLOT8 + 0x58u, 2u);
    wr32(SLOT8 + 0x5Cu, 0x10u);
    r = run();
    chk("s2d.58", rd32(SLOT8 + 0x58u), 0u);
    chk("s2d.5c", rd32(SLOT8 + 0x5Cu), 0xCu);

    /* ---- parked state 3: no ramp, still RotMatrix ---- */
    reset();
    wr16(SLOT8 + 0x20u, 3u);
    wr32(SLOT8 + 0x58u, 0x80u);
    wr32(SLOT8 + 0x5Cu, 0x80u);
    r = run();
    chk("s3.state", rd16(SLOT8 + 0x20u), 3u);
    chk("s3.58", rd32(SLOT8 + 0x58u), 0x80u);
    chk("s3.50", rd32(SLOT8 + 0x50u), 0x10u + 0x80u);
    chk("s3.nrot", s_ncalls, 2u);

    if (s_failures != 0) {
        fprintf(stderr, "%d assertion(s) failed\n", s_failures);
        return 1;
    }
    printf("W34B24-I12 0x800907F4 focused oracle PASS\n");
    return 0;
}
