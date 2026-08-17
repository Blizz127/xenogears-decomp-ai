/*
 * Focused production-linked oracle for retail world callback 0x8008B644.
 *
 * Expected values are hand-derived from the retail disassembly
 * (docs/evidence/w34b24-pre4-8b644 + this rung's re-derivation).  All
 * callees are routed through recording seams; callee results are forced
 * per case.  NEXT_CALLBACK_TARGET is a runtime measurement, not an
 * oracle input.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_callback_8b644.h"

#define POOL_GUEST   0x800D7538u
#define SLOT2        (POOL_GUEST + 0x100u)
#define SLOT3        (POOL_GUEST + 0x180u)
#define SLOT1        (POOL_GUEST + 0x80u)
#define FOLLOW_SRC2  (POOL_GUEST + 0x180u + (2u << 7))
#define SC           0x1F800000u
#define OBJ_BITS     0x00200000u

#define G_POOL_PTR   0x8009BE24u
#define G_RING_IDX   0x8009D154u
#define G_RING       0x8009CEC4u
#define G_FOLLOW     0x8006F8E4u
#define G_WARP_XZ    0x8006EF8Au
#define G_WARP_HEAD  0x8006EE58u
#define G_MIRX       0x8006EE54u
#define G_MIRZ       0x8006EE56u
#define G_MIRH       0x8006EE58u

#define CAN_X        0xBEEFu
#define CAN_Z        0xB0B0u
#define CAN_H        0xB1B1u

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
static void wr8(u32 a, u8 v) { memcpy(PSX_ADDR(a), &v, 1); }
static u8 rd8(u32 a) { u8 v; memcpy(&v, PSX_ADDR(a), 1); return v; }

typedef struct {
    char tag[8];
    u32 a[5];
} Call;

static Call s_calls[64];
static u32 s_ncalls;

static void log_call(const char* tag, u32 a0, u32 a1, u32 a2, u32 a3, u32 a4)
{
    if (s_ncalls < 64u) {
        snprintf(s_calls[s_ncalls].tag, sizeof(s_calls[s_ncalls].tag), "%s",
                 tag);
        s_calls[s_ncalls].a[0] = a0;
        s_calls[s_ncalls].a[1] = a1;
        s_calls[s_ncalls].a[2] = a2;
        s_calls[s_ncalls].a[3] = a3;
        s_calls[s_ncalls].a[4] = a4;
    }
    s_ncalls++;
}

static const Call* find_call(const char* tag, u32 nth)
{
    u32 i, seen = 0;

    for (i = 0; i < s_ncalls && i < 64u; i++)
        if (strcmp(s_calls[i].tag, tag) == 0) {
            if (seen == nth)
                return &s_calls[i];
            seen++;
        }
    return 0;
}

static u32 count_calls(const char* tag)
{
    u32 i, n = 0;

    for (i = 0; i < s_ncalls && i < 64u; i++)
        if (strcmp(s_calls[i].tag, tag) == 0)
            n++;
    return n;
}

void wm_8b644_test_store(u32 address, u32 width, u32 value)
{
    (void)address;
    (void)width;
    (void)value;
}

static s32 s_f_97770;
static s32 s_f_8bec8;
static s32 s_f_93978 = 0x777;
static long s_f_rcos = 100;
static long s_f_rsin = 60;

void wm_8b644_test_245d8(u32 o, s16 n) { log_call("245d8", o, (u32)(s32)n, 0, 0, 0); }
void wm_8b644_test_894c8(u32 i) { log_call("894c8", i, 0, 0, 0, 0); }
void wm_8b644_test_8c1dc(u32 c, u32 o, u32 u) { log_call("8c1dc", c, o, u, 0, 0); }
s32 wm_8b644_test_941c4(u32 a, u32 b, u32 o, u32 g)
{
    log_call("941c4", a, b, o, g, 0);
    return 0;
}
s32 wm_8b644_test_8bec8(u32 o) { log_call("8bec8", o, 0, 0, 0, 0); return s_f_8bec8; }
s32 wm_8b644_test_97770(u32 s, s32 v) { log_call("97770", s, (u32)v, 0, 0, 0); return s_f_97770; }
s32 wm_8b644_test_93978(s32 x, s32 z) { log_call("93978", (u32)x, (u32)z, 0, 0, 0); return s_f_93978; }
long wm_8b644_test_rcos(long a) { log_call("rcos", (u32)a, 0, 0, 0, 0); return s_f_rcos; }
long wm_8b644_test_rsin(long a) { log_call("rsin", (u32)a, 0, 0, 0, 0); return s_f_rsin; }
void wm_8b644_test_74794(s32 t, u32 p) { log_call("74794", (u32)t, p, 0, 0, 0); }

static void set_anim(u8 v)
{
    u8* p = (u8*)PSX_ADDR(OBJ_BITS) + 0xAF;

    *p = v;
}

static void reset(void)
{
    u32 i;

    s_ncalls = 0;
    s_f_97770 = 1;
    s_f_8bec8 = 0;
    wr32(G_POOL_PTR, POOL_GUEST);
    for (i = 0; i < 0x400u; i += 4u)
        wr32(POOL_GUEST + i, 0u);
    for (i = 0; i < 0x30u * 0x14u; i += 4u)
        wr32(G_RING + i, 0u);
    wr16(G_RING_IDX, 0u);
    wr8(G_FOLLOW + 0u, 0u);
    wr8(G_FOLLOW + 1u, 0u);
    wr8(G_FOLLOW + 2u, 0u);
    wr8(G_FOLLOW + 3u, 0u);
    wr16(G_MIRX, CAN_X);
    wr16(G_MIRZ, CAN_Z);
    wr16(G_MIRH, CAN_H);
    wr16(G_WARP_XZ + 8u, 0u);
    wr16(G_WARP_XZ + 10u, 0u);
    wr16(G_WARP_XZ + 12u, 0u);
    wr16(G_WARP_XZ + 14u, 0u);
    wr16(G_WARP_HEAD + 4u, 0u);
    wr16(SLOT2 + 4u, 0u);
    wr16(SLOT2 + 6u, 0u);
    wr16(SLOT2 + 0x20u, 0u);
    wr16(SLOT2 + 0x24u, 0u);
    wr32(SLOT2 + 0x28u, 0x5000u << 12);
    wr32(SLOT2 + 0x2Cu, 0x100u);
    wr32(SLOT2 + 0x30u, 0x3000u << 12);
    wr16(SLOT2 + 0x48u, 0x400u);
    wr32(SLOT2 + 0x4Cu, OBJ_BITS);
    wr32(SLOT2 + 0x58u, 0u);
    wr16(SLOT3 + 4u, 0u);
    wr16(SLOT3 + 0x20u, 0u);
    wr16(SLOT3 + 0x24u, 0u);
    wr32(SLOT3 + 0x28u, 0x111u << 12);
    wr32(SLOT3 + 0x30u, 0x222u << 12);
    wr16(SLOT3 + 0x48u, 0x50u);
    wr32(SLOT3 + 0x4Cu, OBJ_BITS);
    wr32(SLOT3 + 0x58u, 0u);
    set_anim(0);
}

static s32 run2(void)
{
    return wm_8008B644(2);
}

static s32 run3(void)
{
    return wm_8008B644(3);
}

static void expect_no_publish(const char* tag)
{
    char n[64];

    snprintf(n, sizeof(n), "%s.mirx", tag);
    chk(n, rd16(G_MIRX), CAN_X);
    snprintf(n, sizeof(n), "%s.mirz", tag);
    chk(n, rd16(G_MIRZ), CAN_Z);
    snprintf(n, sizeof(n), "%s.mirh", tag);
    chk(n, rd16(G_MIRH), CAN_H);
}

static void expect_74794(const char* tag, u32 want)
{
    char n[64];

    snprintf(n, sizeof(n), "%s.n74794", tag);
    chk(n, count_calls("74794"), want);
    if (want != 0u) {
        const Call* c = find_call("74794", 0);

        chk(n, (u32)(c != 0), 1u);
        if (c) {
            snprintf(n, sizeof(n), "%s.74794.tag", tag);
            chk(n, c->a[0], 0u);
            snprintf(n, sizeof(n), "%s.74794.pose", tag);
            chk(n, c->a[1], SLOT2 + 0x28u);
        }
    }
}

int main(void)
{
    s32 r;

    PsxMemory_Init();

    /* ---- substate 3 -> state 1, then follow/ring (flag clear, match) ---- */
    reset();
    wr16(SLOT2 + 4u, 3u);
    wr16(SLOT2 + 0x20u, 0x13u);
    wr32(SLOT2 + 0x28u, 0xAAu);
    wr32(SLOT2 + 0x2Cu, 0xBBu);
    wr32(SLOT2 + 0x30u, 0xCCu);
    wr32(G_RING + 0u, 0xAAu);
    wr32(G_RING + 4u, 0xBBu);
    wr32(G_RING + 8u, 0xCCu);
    wr16(G_RING + 0x10u, 0x321u);
    set_anim(2);
    r = run2();
    chk("sub3.ret", (u32)r, 1u);
    chk("sub3.sub", rd16(SLOT2 + 4u), 0u);
    chk("sub3.state", rd16(SLOT2 + 0x20u), 1u);
    chk("sub3.head", rd16(SLOT2 + 0x48u), 0x321u);
    chk("sub3.flag24", rd16(SLOT2 + 0x24u), 0u);
    chk("sub3.245d8", count_calls("245d8"), 1u);
    chk("sub3.894c8", count_calls("894c8"), 1u);
    expect_74794("sub3", 1u);
    expect_no_publish("sub3");

    /* ---- substate 2 -> state 0x28 warp-in, join 0x30 anim tail ---- */
    reset();
    wr16(SLOT2 + 4u, 2u);
    wr16(G_WARP_XZ + 12u, 0x123u);
    wr16(G_WARP_XZ + 14u, 0x456u);
    wr16(G_WARP_XZ + 8u, 0x333u);
    wr16(G_WARP_XZ + 10u, 0x444u);
    wr16(G_WARP_HEAD + 4u, 0x800u);
    r = run2();
    chk("sub2.ret", (u32)r, 1u);
    chk("sub2.sub", rd16(SLOT2 + 4u), 0u);
    chk("sub2.state", rd16(SLOT2 + 0x20u), 0x29u);
    chk("sub2.posx", rd32(SLOT2 + 0x28u), 0x123u << 12);
    chk("sub2.posz", rd32(SLOT2 + 0x30u), 0x456u << 12);
    chk("sub2.posy", rd32(SLOT2 + 0x2Cu), (u32)s_f_93978);
    chk("sub2.5c", rd32(SLOT2 + 0x5Cu), 0x800u);
    chk("sub2.head", rd16(SLOT2 + 0x48u), 0x800u);
    chk("sub2.fx", rd32(SC + 0u), (0x123u << 12) + (u32)s_f_rcos * 48u);
    chk("sub2.fz", rd32(SC + 8u),
        (0x456u << 12) + (0u - (u32)s_f_rsin) * 48u);
    chk("sub2.t50", rd32(SLOT2 + 0x50u), rd32(SC + 0u) >> 12);
    chk("sub2.t54", rd32(SLOT2 + 0x54u), rd32(SC + 8u) >> 12);
    chk("sub2.flag24", rd16(SLOT2 + 0x24u), 0u);
    chk("sub2.n93978", count_calls("93978"), 1u);
    chk("sub2.n941c4", count_calls("941c4"), 1u);
    chk("sub2.n8c1dc", count_calls("8c1dc"), 0u);
    chk("sub2.n8bec8", count_calls("8bec8"), 1u);
    {
        const Call* c = find_call("941c4", 0);

        chk("sub2.941c4", (u32)(c != 0), 1u);
        if (c) {
            chk("sub2.941c4.a", c->a[0], SLOT2 + 0x28u);
            chk("sub2.941c4.b", c->a[1], SC);
            chk("sub2.941c4.o", c->a[2], SLOT2 + 0x38u);
            chk("sub2.941c4.g", c->a[3], SLOT2 + 0x48u);
        }
        c = find_call("245d8", 0);
        chk("sub2.245d8", (u32)(c != 0), 1u);
        if (c)
            chk("sub2.245d8.anim", c->a[1], 1u);
    }
    expect_74794("sub2", 1u);
    expect_no_publish("sub2");

    /* ---- substate 1 -> state 8 + neighbor, fall into 9 ---- */
    reset();
    wr16(SLOT2 + 4u, 1u);
    wr16(SLOT2 + 6u, 1u);
    wr32(SLOT1 + 0x28u, 0xABC000u);
    wr32(SLOT1 + 0x30u, 0xDEF000u);
    s_f_8bec8 = 0;
    r = run2();
    chk("sub1.ret", (u32)r, 1u);
    chk("sub1.sub", rd16(SLOT2 + 4u), 0u);
    chk("sub1.state", rd16(SLOT2 + 0x20u), 9u);
    chk("sub1.t50", rd32(SLOT2 + 0x50u), 0xABCu);
    chk("sub1.t54", rd32(SLOT2 + 0x54u), 0xDEFu);
    chk("sub1.n941c4", count_calls("941c4"), 1u);
    chk("sub1.n8bec8", count_calls("8bec8"), 1u);
    chk("sub1.n8c1dc", count_calls("8c1dc"), 1u);
    {
        const Call* c = find_call("941c4", 0);

        if (c) {
            chk("sub1.941c4.a", c->a[0], SLOT2 + 0x28u);
            chk("sub1.941c4.b", c->a[1], SLOT1 + 0x28u);
            chk("sub1.941c4.o", c->a[2], SLOT2 + 0x38u);
            chk("sub1.941c4.g", c->a[3], SLOT2 + 0x48u);
        }
        c = find_call("8c1dc", 0);
        if (c) {
            chk("sub1.8c1dc.ctx", c->a[0], 0x30u);
            chk("sub1.8c1dc.obj", c->a[1], SLOT2);
            chk("sub1.8c1dc.sc", c->a[2], SC);
        }
        c = find_call("245d8", 0);
        if (c)
            chk("sub1.245d8.anim", c->a[1], 1u);
    }
    expect_no_publish("sub1");

    /* ---- substate 1 + 8BEC8==3 advances through 9 into 0xA ---- */
    reset();
    wr16(SLOT2 + 4u, 1u);
    wr16(SLOT2 + 6u, 1u);
    s_f_8bec8 = 3;
    r = run2();
    chk("sub1a.state", rd16(SLOT2 + 0x20u), 0xAu);
    chk("sub1a.n8bec8", count_calls("8bec8"), 1u);

    /* ---- substate 5 -> state 0x30 vs player pose ---- */
    reset();
    wr16(SLOT2 + 4u, 5u);
    wr32(POOL_GUEST + 0xA8u, 0x111000u);
    wr32(POOL_GUEST + 0xB0u, 0x222000u);
    r = run2();
    chk("sub5.ret", (u32)r, 1u);
    chk("sub5.state", rd16(SLOT2 + 0x20u), 0x31u);
    chk("sub5.t50", rd32(SLOT2 + 0x50u), 0x111u);
    chk("sub5.t54", rd32(SLOT2 + 0x54u), 0x222u);
    chk("sub5.n941c4", count_calls("941c4"), 1u);
    chk("sub5.n8c1dc", count_calls("8c1dc"), 0u);
    {
        const Call* c = find_call("941c4", 0);

        if (c)
            chk("sub5.941c4.b", c->a[1], POOL_GUEST + 0xA8u);
    }
    expect_no_publish("sub5");

    /* ---- JT1 OOR 0x41 parks ---- */
    reset();
    wr16(SLOT2 + 0x20u, 0x41u);
    r = run2();
    chk("oor.ret", (u32)r, 1u);
    chk("oor.calls", s_ncalls, 1u);
    expect_74794("oor", 1u);
    expect_no_publish("oor");

    /* ---- parked 0x13, +0x24=1 suppresses 74794 ---- */
    reset();
    wr16(SLOT2 + 0x20u, 0x13u);
    wr16(SLOT2 + 0x24u, 1u);
    r = run2();
    chk("park.ret", (u32)r, 1u);
    chk("park.calls", s_ncalls, 0u);
    chk("park.state", rd16(SLOT2 + 0x20u), 0x13u);
    expect_74794("park", 0u);
    expect_no_publish("park");

    /* ---- follow flag set: copy pool+0x180+(slot<<7), suppress 74794 ---- */
    reset();
    wr8(G_FOLLOW + 2u, 1u);
    wr8(G_FOLLOW + 1u, 0u);
    wr32(FOLLOW_SRC2 + 0x28u, 0xAAA000u);
    wr32(FOLLOW_SRC2 + 0x2Cu, 0xBBB000u);
    wr32(FOLLOW_SRC2 + 0x30u, 0xCCC000u);
    wr16(FOLLOW_SRC2 + 0x48u, 0x777u);
    r = run2();
    chk("ff.ret", (u32)r, 1u);
    chk("ff.x", rd32(SLOT2 + 0x28u), 0xAAA000u);
    chk("ff.y", rd32(SLOT2 + 0x2Cu), 0xBBB000u);
    chk("ff.z", rd32(SLOT2 + 0x30u), 0xCCC000u);
    chk("ff.h", rd16(SLOT2 + 0x48u), 0x777u);
    chk("ff.flag24", rd16(SLOT2 + 0x24u), 1u);
    chk("ff.894c8", count_calls("894c8"), 1u);
    {
        const Call* c = find_call("894c8", 0);

        if (c)
            chk("ff.894c8.ctx", c->a[0], 0x30u);
    }
    chk("ff.n8c1dc", count_calls("8c1dc"), 0u);
    expect_74794("ff", 0u);
    expect_no_publish("ff");

    /* ---- ring miss + mask: (0x21-1)&0x1F = 0, not 0x20 ---- */
    reset();
    wr16(G_RING_IDX, 0x21u);
    wr32(SLOT2 + 0x58u, 1u);
    wr32(G_RING + 0u, 0x1000u);
    wr32(G_RING + 4u, 0x2000u);
    wr32(G_RING + 8u, 0x3000u);
    wr16(G_RING + 0x10u, 0xABC);
    /* Entry 0x20 starts at ring+0x280; its +0x10 would land on D154
     * (the ring index). Write only the XYZ words so the mutant can
     * still be distinguished without clobbering the index. */
    wr32(G_RING + 0x20u * 0x14u, 0xDEAD000u);
    wr32(G_RING + 0x20u * 0x14u + 4u, 0xBEEF000u);
    wr32(G_RING + 0x20u * 0x14u + 8u, 0xFEED000u);
    set_anim(0);
    r = run2();
    chk("ring.ret", (u32)r, 1u);
    chk("ring.x", rd32(SLOT2 + 0x28u), 0x1000u);
    chk("ring.y", rd32(SLOT2 + 0x2Cu), 0x2000u);
    chk("ring.z", rd32(SLOT2 + 0x30u), 0x3000u);
    chk("ring.h", rd16(SLOT2 + 0x48u), 0xABCu);
    chk("ring.flag24", rd16(SLOT2 + 0x24u), 0u);
    chk("ring.n8c1dc", count_calls("8c1dc"), 1u);
    chk("ring.n245d8", count_calls("245d8"), 1u);
    {
        const Call* c = find_call("245d8", 0);

        if (c)
            chk("ring.245d8.anim", c->a[1], 1u);
        c = find_call("8c1dc", 0);
        if (c)
            chk("ring.8c1dc.ctx", c->a[0], 0x30u);
    }
    expect_74794("ring", 1u);

    /* ---- ring match, actor already idle: no 245D8/894C8 ---- */
    reset();
    wr32(SLOT2 + 0x28u, 0x10u);
    wr32(SLOT2 + 0x2Cu, 0x20u);
    wr32(SLOT2 + 0x30u, 0x30u);
    wr32(G_RING + 0u, 0x10u);
    wr32(G_RING + 4u, 0x20u);
    wr32(G_RING + 8u, 0x30u);
    wr16(G_RING + 0x10u, 0x55u);
    set_anim(0);
    r = run2();
    chk("hit0.n245d8", count_calls("245d8"), 0u);
    chk("hit0.n894c8", count_calls("894c8"), 0u);
    chk("hit0.h", rd16(SLOT2 + 0x48u), 0x55u);

    /* ---- state 2: copy slot-7 pose ---- */
    reset();
    wr16(SLOT2 + 0x20u, 2u);
    wr32(POOL_GUEST + 0x3A8u, 0x777000u);
    wr32(POOL_GUEST + 0x3ACu, 0x888000u);
    wr32(POOL_GUEST + 0x3B0u, 0x999000u);
    wr16(POOL_GUEST + 0x3C8u, 0x123u);
    r = run2();
    chk("s2.ret", (u32)r, 1u);
    chk("s2.x", rd32(SLOT2 + 0x28u), 0x777000u);
    chk("s2.y", rd32(SLOT2 + 0x2Cu), 0x888000u);
    chk("s2.z", rd32(SLOT2 + 0x30u), 0x999000u);
    chk("s2.h", rd16(SLOT2 + 0x48u), 0x123u);
    chk("s2.n8c1dc", count_calls("8c1dc"), 0u);
    expect_74794("s2", 1u);

    /* ---- state 9 hold / advance ---- */
    reset();
    wr16(SLOT2 + 0x20u, 9u);
    s_f_8bec8 = 2;
    r = run2();
    chk("s9h.state", rd16(SLOT2 + 0x20u), 9u);
    chk("s9h.n8c1dc", count_calls("8c1dc"), 1u);
    reset();
    wr16(SLOT2 + 0x20u, 9u);
    s_f_8bec8 = 3;
    r = run2();
    chk("s9a.state", rd16(SLOT2 + 0x20u), 0xAu);
    chk("s9a.n8c1dc", count_calls("8c1dc"), 1u);

    /* ---- state 0xA hold / go ---- */
    reset();
    wr16(SLOT2 + 0x20u, 0xAu);
    wr16(SLOT2 + 6u, 4u);
    s_f_97770 = 0;
    r = run2();
    chk("sah.state", rd16(SLOT2 + 0x20u), 0xAu);
    chk("sah.n894c8", count_calls("894c8"), 0u);
    {
        const Call* c = find_call("97770", 0);

        chk("sah.97770", (u32)(c != 0), 1u);
        if (c) {
            chk("sah.97770.a0", c->a[0], 4u);
            chk("sah.97770.a1", c->a[1], 4u);
        }
    }
    reset();
    wr16(SLOT2 + 0x20u, 0xAu);
    wr16(SLOT2 + 6u, 4u);
    s_f_97770 = 1;
    r = run2();
    chk("sag.ret", (u32)r, 1u);
    chk("sag.state", rd16(SLOT2 + 0x20u), 2u);
    chk("sag.flag24", rd16(SLOT2 + 0x24u), 1u);
    chk("sag.n894c8", count_calls("894c8"), 1u);
    expect_74794("sag", 0u);

    /* ---- state 0x28 with 8BEC8==3 advances to 0x2A ---- */
    reset();
    wr16(SLOT2 + 0x20u, 0x28u);
    wr16(G_WARP_XZ + 12u, 0x10u);
    wr16(G_WARP_XZ + 14u, 0x20u);
    wr16(G_WARP_HEAD + 4u, 0x100u);
    s_f_8bec8 = 3;
    r = run2();
    chk("s28a.state", rd16(SLOT2 + 0x20u), 0x2Au);
    chk("s28a.n8c1dc", count_calls("8c1dc"), 0u);

    /* ---- state 0x2A: clear follow flag, state=0x40 ---- */
    reset();
    wr16(SLOT2 + 0x20u, 0x2Au);
    wr8(G_FOLLOW + 2u, 0x55u);
    r = run2();
    chk("s2a.ret", (u32)r, 1u);
    chk("s2a.state", rd16(SLOT2 + 0x20u), 0x40u);
    chk("s2a.flag", rd8(G_FOLLOW + 2u), 0u);
    expect_74794("s2a", 1u);

    /* ---- state 0x30 live (also kills a 0x2D JT1 guard) ---- */
    reset();
    wr16(SLOT2 + 0x20u, 0x30u);
    wr32(POOL_GUEST + 0xA8u, 0xABC000u);
    wr32(POOL_GUEST + 0xB0u, 0xDEF000u);
    r = run2();
    chk("s30.ret", (u32)r, 1u);
    chk("s30.state", rd16(SLOT2 + 0x20u), 0x31u);
    chk("s30.t50", rd32(SLOT2 + 0x50u), 0xABCu);
    chk("s30.n941c4", count_calls("941c4"), 1u);
    chk("s30.n8c1dc", count_calls("8c1dc"), 0u);
    expect_no_publish("s30");

    /* ---- state 0x29 / 0x31 shared 8BEC8 step ---- */
    reset();
    wr16(SLOT2 + 0x20u, 0x29u);
    s_f_8bec8 = 3;
    r = run2();
    chk("s29.state", rd16(SLOT2 + 0x20u), 0x2Au);
    chk("s29.n8c1dc", count_calls("8c1dc"), 0u);
    reset();
    wr16(SLOT2 + 0x20u, 0x31u);
    s_f_8bec8 = 3;
    r = run2();
    chk("s31.state", rd16(SLOT2 + 0x20u), 0x32u);

    /* ---- state 0x32 hold / go ---- */
    reset();
    wr16(SLOT2 + 0x20u, 0x32u);
    s_f_97770 = 0;
    r = run2();
    chk("s32h.state", rd16(SLOT2 + 0x20u), 0x32u);
    {
        const Call* c = find_call("97770", 0);

        chk("s32h.97770", (u32)(c != 0), 1u);
        if (c) {
            chk("s32h.97770.a0", c->a[0], 1u);
            chk("s32h.97770.a1", c->a[1], 6u);
        }
    }
    reset();
    wr16(SLOT2 + 0x20u, 0x32u);
    s_f_97770 = 1;
    r = run2();
    chk("s32g.state", rd16(SLOT2 + 0x20u), 0u);

    /* ---- slot 3: same body, obj id 0x31, follow flag +3 ---- */
    reset();
    wr8(G_FOLLOW + 3u, 1u);
    wr32(POOL_GUEST + 0x180u + (3u << 7) + 0x28u, 0x135000u);
    wr32(POOL_GUEST + 0x180u + (3u << 7) + 0x2Cu, 0x246000u);
    wr32(POOL_GUEST + 0x180u + (3u << 7) + 0x30u, 0x357000u);
    wr16(POOL_GUEST + 0x180u + (3u << 7) + 0x48u, 0x99u);
    r = run3();
    chk("s3.ret", (u32)r, 1u);
    chk("s3.x", rd32(SLOT3 + 0x28u), 0x135000u);
    chk("s3.h", rd16(SLOT3 + 0x48u), 0x99u);
    chk("s3.flag24", rd16(SLOT3 + 0x24u), 1u);
    {
        const Call* c = find_call("894c8", 0);

        chk("s3.894c8", (u32)(c != 0), 1u);
        if (c)
            chk("s3.894c8.ctx", c->a[0], 0x31u);
    }
    chk("s3.n74794", count_calls("74794"), 0u);
    expect_no_publish("s3");

    if (s_failures != 0) {
        fprintf(stderr, "FAILURES=%d\n", s_failures);
        return 1;
    }
    printf("W34B24-I9 0x8008B644 focused oracle PASS\n");
    return 0;
}
