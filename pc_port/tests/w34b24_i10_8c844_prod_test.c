/*
 * Focused production-linked oracle for retail world callback 0x8008C844.
 *
 * Expected values are hand-derived from the retail disassembly of
 * [0x8008C844, 0x8008D3F0).  All callees are routed through recording
 * seams; callee results are forced per case.  NEXT_CALLBACK_TARGET is
 * a runtime measurement, not an oracle input.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_callback_8c844.h"

#define POOL_GUEST   0x800D7538u
#define SLOT4        (POOL_GUEST + 0x200u)
#define SLOT7        (POOL_GUEST + 0x380u)
#define SC           0x1F800000u
#define OBJ_BITS     0x00200000u

#define G_POOL_PTR   0x8009BE24u
#define G_MODE_WORD  0x8009C170u
#define G_MODE_FLAG  0x8009BE10u
#define G_BD04       0x8009BD04u
#define G_AREA       0x8009BD60u
#define G_BOUND      0x8009D738u
#define G_CRUMB      0x8009B180u
#define G_RING_IDX   0x8009D154u
#define G_RING       0x8009CEC4u
#define G_POSE       0x8009D55Cu
#define G_HEADM      0x8009D52Cu
#define G_D554       0x8009D554u
#define G_D7CC       0x8009D7CCu
#define G_RESYNC     0x8006F8E5u
#define G_BUSY2      0x8006F8E6u
#define G_BUSY3      0x8006F8E7u
#define G_S4PRES     0x8006F368u
#define G_S2PRES     0x8006F369u
#define G_S3PRES     0x8006F36Au
#define G_PUBX       0x8006EF90u
#define G_PUBZ       0x8006EF92u
#define G_PUBH       0x8006EE5Au

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

void wm_8c844_test_store(u32 address, u32 width, u32 value)
{
    (void)address;
    (void)width;
    (void)value;
}

static s32 s_f_90c68;
static s32 s_f_95414[2];
static u32 s_n_95414;
static s32 s_f_97770;
static s32 s_f_8bec8;
static s32 s_f_93f18;
static s32 s_f_94060;
static long s_f_rcos = 100;
static long s_f_rsin = 60;

s32 wm_8c844_test_90c68(u32 a) { log_call("90c68", a, 0, 0, 0, 0); return s_f_90c68; }
void wm_8c844_test_245d8(u32 o, s16 n) { log_call("245d8", o, (u32)(s32)n, 0, 0, 0); }
void wm_8c844_test_894c8(u32 i) { log_call("894c8", i, 0, 0, 0, 0); }
void wm_8c844_test_8c1dc(u32 c, u32 o, u32 u) { log_call("8c1dc", c, o, u, 0, 0); }
s32 wm_8c844_test_95414(u32 p, u32 d, u32 o, s32 s, s32 m)
{
    s32 r = s_f_95414[(s_n_95414 < 2u) ? s_n_95414 : 1u];

    log_call("95414", p, d, o, (u32)s, (u32)m);
    s_n_95414++;
    return r;
}
void wm_8c844_test_8c040(u32 v, s32 a1, s32 a2, u32 o1, u32 o2)
{
    log_call("8c040", v, (u32)a1, (u32)a2, o1, o2);
}
void wm_8c844_test_7528c(void) { log_call("7528c", 0, 0, 0, 0, 0); }
s32 wm_8c844_test_94238(u32 p, u32 i) { log_call("94238", p, i, 0, 0, 0); return 0; }
s32 wm_8c844_test_97770(u32 s, s32 v) { log_call("97770", s, (u32)v, 0, 0, 0); return s_f_97770; }
s32 wm_8c844_test_941c4(u32 a, u32 b, u32 o, u32 g)
{
    log_call("941c4", a, b, o, g, 0);
    return 0;
}
s32 wm_8c844_test_8bec8(u32 o) { log_call("8bec8", o, 0, 0, 0, 0); return s_f_8bec8; }
s32 wm_8c844_test_93f18(u32 v) { log_call("93f18", v, 0, 0, 0, 0); return s_f_93f18; }
s32 wm_8c844_test_94060(s32 a0, s32 a1)
{
    log_call("94060", (u32)a0, (u32)a1, 0, 0, 0);
    return s_f_94060;
}
void wm_8c844_test_8dff4(u32 o) { log_call("8dff4", o, 0, 0, 0, 0); }
void wm_8c844_test_89160(u32 a0, u32 a1, u32 a2)
{
    log_call("89160", a0, a1, a2, 0, 0);
}
long wm_8c844_test_rcos(long a) { log_call("rcos", (u32)a, 0, 0, 0, 0); return s_f_rcos; }
long wm_8c844_test_rsin(long a) { log_call("rsin", (u32)a, 0, 0, 0, 0); return s_f_rsin; }
void wm_8c844_test_74794(s32 t, u32 p) { log_call("74794", (u32)t, p, 0, 0, 0); }

static void set_anim(u8 v)
{
    u8* p = (u8*)PSX_ADDR(OBJ_BITS) + 0xAF;

    *p = v;
}

static void reset(void)
{
    u32 i;

    s_ncalls = 0;
    s_n_95414 = 0;
    s_f_90c68 = 0;
    s_f_95414[0] = 1;
    s_f_95414[1] = 1;
    s_f_97770 = 1;
    s_f_8bec8 = 0;
    s_f_93f18 = 0;
    s_f_94060 = 0;
    wr32(G_POOL_PTR, POOL_GUEST);
    for (i = 0; i < 0x400u; i += 4u)
        wr32(POOL_GUEST + i, 0u);
    for (i = 0; i < 0x30u * 0x14u; i += 4u)
        wr32(G_RING + i, 0u);
    wr32(G_MODE_WORD, 0u);
    wr32(G_MODE_FLAG, 0x1234u);
    wr16(G_BD04, 0x7777u);
    wr8(G_AREA, 0u);
    wr8(G_BOUND, 0u);
    wr16(G_CRUMB, 1u);
    wr16(G_RING_IDX, 0u);
    wr8(G_RESYNC, 0u);
    wr8(G_BUSY2, 0u);
    wr8(G_BUSY3, 0u);
    wr8(G_S4PRES, 0u);
    wr8(G_S2PRES, 0u);
    wr8(G_S3PRES, 0u);
    wr16(G_PUBX, 0xBEEFu);
    wr16(G_PUBZ, 0xB0B0u);
    wr16(G_PUBH, 0xB1B1u);
    wr16(SLOT4 + 4u, 0u);
    wr16(SLOT4 + 0x20u, 0u);
    wr16(SLOT4 + 0x22u, 0u);
    wr16(SLOT4 + 0x24u, 0u);
    wr32(SLOT4 + 0x28u, 0x5000u << 12);
    wr32(SLOT4 + 0x2Cu, 0x100u);
    wr32(SLOT4 + 0x30u, 0x3000u << 12);
    wr32(SLOT4 + 0x34u, 0xA5A5A5A5u);
    wr32(SLOT4 + 0x38u, 0u);
    wr32(SLOT4 + 0x3Cu, 0u);
    wr32(SLOT4 + 0x40u, 0u);
    wr32(SLOT4 + 0x44u, 0u);
    wr16(SLOT4 + 0x48u, 0x400u);
    wr16(SLOT4 + 0x4Au, 6u);
    wr32(SLOT4 + 0x4Cu, OBJ_BITS);
    wr32(SLOT4 + 0x58u, 0u);
    set_anim(0);
}

static s32 run(void)
{
    return wm_8008C844(4);
}

static void expect_pub(const char* tag)
{
    char n[64];

    snprintf(n, sizeof(n), "%s.pubx", tag);
    chk(n, rd16(G_PUBX), (u16)(rd32(SLOT4 + 0x28u) >> 12));
    snprintf(n, sizeof(n), "%s.pubz", tag);
    chk(n, rd16(G_PUBZ), (u16)(rd32(SLOT4 + 0x30u) >> 12));
    snprintf(n, sizeof(n), "%s.pubh", tag);
    chk(n, rd16(G_PUBH), rd16(SLOT4 + 0x48u));
}

static void expect_74794(const char* tag, u32 want_n)
{
    char n[64];
    const Call* c;

    snprintf(n, sizeof(n), "%s.n74794", tag);
    chk(n, count_calls("74794"), want_n);
    if (want_n != 0u) {
        c = find_call("74794", 0);
        chk(tag, (u32)(c != 0), 1u);
        if (c) {
            snprintf(n, sizeof(n), "%s.74794.tag", tag);
            chk(n, c->a[0], 1u);
            snprintf(n, sizeof(n), "%s.74794.pose", tag);
            chk(n, c->a[1], SLOT4 + 0x28u);
        }
    }
}

int main(void)
{
    s32 r;
    const Call* c;

    PsxMemory_Init();

    /* ---- substate 4 -> state 0x10, resync=1, lap=1 ---- */
    reset();
    wr16(SLOT4 + 4u, 4u);
    wr16(SLOT4 + 0x20u, 0x13u);
    wr32(G_MODE_WORD, 9u);
    r = run();
    chk("sub4.ret", (u32)r, 1u);
    chk("sub4.sub", rd16(SLOT4 + 4u), 0u);
    chk("sub4.state", rd16(SLOT4 + 0x20u), 0x10u);
    chk("sub4.lap", rd32(SLOT4 + 0x58u), 1u);
    chk("sub4.resync", rd8(G_RESYNC), 1u);
    expect_pub("sub4");
    expect_74794("sub4", 1u);

    /* ---- substate 3 -> state 0x30 (runs warp, falls into 0x31) ---- */
    reset();
    wr16(SLOT4 + 4u, 3u);
    wr32(POOL_GUEST + 0x3F8u, 0x800u);
    r = run();
    chk("sub3.ret", (u32)r, 1u);
    chk("sub3.state", rd16(SLOT4 + 0x20u), 0x31u);
    chk("sub3.n8dff4", count_calls("8dff4"), 1u);
    chk("sub3.fx", rd32(SC + 0u), (0x5000u << 12) + (u32)s_f_rcos * 96u);
    chk("sub3.fz", rd32(SC + 8u),
        (0x3000u << 12) + (0u - (u32)s_f_rsin) * 96u);

    /* ---- substate 7: lap gate closed / open ---- */
    reset();
    wr16(SLOT4 + 4u, 7u);
    wr32(SLOT4 + 0x58u, 4u);
    wr32(G_MODE_WORD, 9u);
    wr16(SLOT4 + 0x20u, 0x13u);
    r = run();
    chk("sub7c.lap", rd32(SLOT4 + 0x58u), 5u);
    chk("sub7c.state", rd16(SLOT4 + 0x20u), 0x13u);
    chk("sub7c.flag", rd32(G_MODE_FLAG), 0x1234u);

    reset();
    wr16(SLOT4 + 4u, 7u);
    wr32(SLOT4 + 0x58u, 4u);
    wr32(G_MODE_WORD, 5u);
    wr16(SLOT4 + 0x20u, 0x13u);
    wr8(G_RESYNC, 1u);
    s_f_90c68 = 1;
    r = run();
    chk("sub7o.lap", rd32(SLOT4 + 0x58u), 5u);
    chk("sub7o.flag", rd32(G_MODE_FLAG), 2u);
    chk("sub7o.state", rd16(SLOT4 + 0x20u), 1u);
    chk("sub7o.d554", rd32(G_D554), 0u);

    /* ---- substate 8 -> state 0x18 ---- */
    reset();
    wr16(SLOT4 + 4u, 8u);
    wr8(G_S4PRES, 0u);
    r = run();
    chk("sub8.state", rd16(SLOT4 + 0x20u), 0x19u);
    chk("sub8.timer", rd16(SLOT4 + 0x22u), 8u);
    chk("sub8.n89160", count_calls("89160"), 1u);

    /* ---- JT1 OOR 0x41 ---- */
    reset();
    wr16(SLOT4 + 0x20u, 0x41u);
    r = run();
    chk("oor.ret", (u32)r, 1u);
    chk("oor.n74794", count_calls("74794"), 1u);
    expect_pub("oor");

    /* ---- parked 0x13 ---- */
    reset();
    wr16(SLOT4 + 0x20u, 0x13u);
    r = run();
    chk("park.ret", (u32)r, 1u);
    chk("park.calls", s_ncalls, 1u);
    expect_74794("park", 1u);

    /* ---- state 0 resync != 1, AF != 3 ---- */
    reset();
    wr8(G_RESYNC, 0u);
    set_anim(1);
    r = run();
    chk("rs0.n90c68", count_calls("90c68"), 0u);
    chk("rs0.n245d8", count_calls("245d8"), 1u);
    c = find_call("245d8", 0);
    chk("rs0.anim", c ? c->a[1] : 0u, 3u);
    c = find_call("894c8", 0);
    chk("rs0.rec", c ? c->a[0] : 0u, 0x2Cu);

    /* ---- state 0 resync != 1, AF == 3: skip anim ---- */
    reset();
    set_anim(3);
    r = run();
    chk("rs3.n245d8", count_calls("245d8"), 0u);

    /* ---- class 1: zero D554/D7CC, pre-tail ---- */
    reset();
    wr8(G_RESYNC, 1u);
    s_f_90c68 = 1;
    wr32(G_D554, 0x111u);
    wr32(G_D7CC, 0x222u);
    r = run();
    chk("c1.n90c68", count_calls("90c68"), 1u);
    chk("c1.d554", rd32(G_D554), 0u);
    chk("c1.d7cc", rd32(G_D7CC), 0u);
    chk("c1.bd04", rd16(G_BD04), 0u);
    chk("c1.n95414", count_calls("95414"), 0u);
    expect_74794("c1", 1u);

    /* ---- class 3 -> state 8 ---- */
    reset();
    wr8(G_RESYNC, 1u);
    s_f_90c68 = 3;
    r = run();
    chk("c3.state", rd16(SLOT4 + 0x20u), 8u);
    chk("c3.n95414", count_calls("95414"), 0u);

    /* ---- class 4: 93F18/94060, 94060==0 holds ---- */
    reset();
    wr8(G_RESYNC, 1u);
    s_f_90c68 = 4;
    s_f_93f18 = 0x123;
    s_f_94060 = 0;
    r = run();
    chk("c4h.state", rd16(SLOT4 + 0x20u), 0u);
    c = find_call("94060", 0);
    chk("c4h.94060.a0", c ? c->a[0] : 0u, 1u);
    chk("c4h.94060.a1", c ? c->a[1] : 0u, 0x123u);

    reset();
    wr8(G_RESYNC, 1u);
    s_f_90c68 = 4;
    s_f_94060 = 1;
    r = run();
    chk("c4a.state", rd16(SLOT4 + 0x20u), 0x20u);

    /* ---- movement: r=1, breadcrumb open, vel nonzero ---- */
    reset();
    wr8(G_RESYNC, 1u);
    s_f_90c68 = 0;
    wr32(SLOT4 + 0x38u, 0x10u);
    wr32(SLOT4 + 0x40u, 0x20u);
    wr32(SC + 0x90u, 0x11111000u);
    wr32(SC + 0x94u, 0x22222000u);
    wr32(SC + 0x98u, 0x33333000u);
    wr32(SC + 0x9Cu, 0x44444000u);
    set_anim(0);
    r = run();
    chk("mv1.ret", (u32)r, 1u);
    chk("mv1.n95414", count_calls("95414"), 1u);
    c = find_call("8c040", 0);
    chk("mv1.8c040.a1", c ? c->a[1] : 0u, 0x18u);
    chk("mv1.8c040.a2", c ? c->a[2] : 0u, 0x30u);
    c = find_call("94238", 0);
    chk("mv1.94238.i", c ? c->a[1] : 99u, 1u);
    c = find_call("8c1dc", 0);
    chk("mv1.8c1dc.rec", c ? c->a[0] : 0u, 0x2Cu);
    chk("mv1.posx", rd32(SLOT4 + 0x28u), 0x11111000u);
    chk("mv1.ring", rd16(G_RING_IDX), 1u);
    chk("mv1.ring.x", rd32(G_RING + 0x14u), 0x11111000u);
    chk("mv1.ring.h", rd16(G_RING + 0x14u + 0x10u), 0x400u);
    chk("mv1.n7528c", count_calls("7528c"), 1u);
    chk("mv1.bd04", rd16(G_BD04), 0u);

    /* ---- movement: r=0x10000 is nonzero (no u16 collapse) ---- */
    reset();
    wr8(G_RESYNC, 1u);
    s_f_90c68 = 0;
    wr32(SLOT4 + 0x38u, 0x10u);
    s_f_95414[0] = 0x10000;
    s_f_95414[1] = 1;
    r = run();
    chk("r16.n95414", count_calls("95414"), 1u);
    c = find_call("8c040", 0);
    chk("r16.8c040.vec", c ? c->a[0] : 0u, SLOT4 + 0x28u);

    /* ---- movement: retry r=0 then r=1 ---- */
    reset();
    wr8(G_RESYNC, 1u);
    s_f_90c68 = 0;
    wr32(SLOT4 + 0x38u, 0x10u);
    wr32(SLOT4 + 0x40u, 0x20u);
    s_f_95414[0] = 0;
    s_f_95414[1] = 1;
    wr32(SC + 0x90u, 0xABCDu);
    wr32(SC + 0x94u, 0x111u);
    wr32(SC + 0x98u, 0x222u);
    wr32(SC + 0x9Cu, 0x333u);
    r = run();
    chk("retry.n95414", count_calls("95414"), 2u);
    chk("retry.posx", rd32(SLOT4 + 0x28u), 0xABCDu);

    /* ---- movement: double zero zeroes +0x40/+0x38 only ---- */
    reset();
    wr8(G_RESYNC, 1u);
    s_f_90c68 = 0;
    wr32(SLOT4 + 0x38u, 0x10u);
    wr32(SLOT4 + 0x3Cu, 0x99u);
    wr32(SLOT4 + 0x40u, 0x20u);
    wr32(SLOT4 + 0x44u, 0x77u);
    wr32(SC + 0x90u, 0u);
    wr32(SC + 0x94u, 0x55u);
    wr32(SC + 0x98u, 0u);
    wr32(SC + 0x9Cu, 0x66u);
    s_f_95414[0] = 0;
    s_f_95414[1] = 0;
    r = run();
    chk("zfb.38", rd32(SLOT4 + 0x38u), 0u);
    chk("zfb.3c", rd32(SLOT4 + 0x3Cu), 0u);
    chk("zfb.40", rd32(SLOT4 + 0x40u), 0u);
    chk("zfb.44", rd32(SLOT4 + 0x44u), 0x66u);

    /* ---- ring wrap 0x1F -> 0 ---- */
    reset();
    wr8(G_RESYNC, 1u);
    s_f_90c68 = 0;
    wr32(SLOT4 + 0x38u, 0x10u);
    wr32(SLOT4 + 0x40u, 0x20u);
    wr16(G_RING_IDX, 0x1Fu);
    wr32(SC + 0x90u, 0x1000u);
    wr32(SC + 0x94u, 0u);
    wr32(SC + 0x98u, 0x2000u);
    wr32(SC + 0x9Cu, 0u);
    r = run();
    chk("wrap.ring", rd16(G_RING_IDX), 0u);
    chk("wrap.entry", rd32(G_RING), 0x1000u);

    /* ---- state 2: copy X/Z/heading, skip Y, skip 74794 ---- */
    reset();
    wr16(SLOT4 + 0x20u, 2u);
    wr32(SLOT4 + 0x2Cu, 0xDEADBEEFu);
    wr32(SLOT7 + 0x28u, 0xAAA000u); /* pool+0x3A8 */
    wr32(SLOT7 + 0x2Cu, 0xBBB000u);
    wr32(SLOT7 + 0x30u, 0xCCC000u);
    wr16(SLOT7 + 0x48u, 0x777u);
    r = run();
    chk("s2.x", rd32(SLOT4 + 0x28u), 0xAAA000u);
    chk("s2.y", rd32(SLOT4 + 0x2Cu), 0xDEADBEEFu);
    chk("s2.z", rd32(SLOT4 + 0x30u), 0xCCC000u);
    chk("s2.h", rd16(SLOT4 + 0x48u), 0x777u);
    expect_74794("s2", 0u);
    expect_pub("s2");

    /* ---- 74794 skip when SLOT4_PRES==7 and MODE_FLAG!=2 ---- */
    reset();
    wr16(SLOT4 + 0x20u, 0x13u);
    wr8(G_S4PRES, 7u);
    r = run();
    expect_74794("pres7", 0u);
    expect_pub("pres7");

    /* ---- 74794 called when MODE_FLAG==2 even if PRES==7 ---- */
    reset();
    wr16(SLOT4 + 0x20u, 0x13u);
    wr8(G_S4PRES, 7u);
    wr32(G_MODE_FLAG, 2u);
    r = run();
    expect_74794("mf2", 1u);

    /* ---- state 8 busy==1: 97770(5,1), store area at slot+0x86 ---- */
    reset();
    wr16(SLOT4 + 0x20u, 8u);
    wr8(G_S2PRES, 1u);
    wr8(G_BUSY2, 1u);
    wr8(G_AREA, 4u);
    r = run();
    c = find_call("97770", 0);
    chk("s8b.a0", c ? c->a[0] : 0u, 5u);
    chk("s8b.a1", c ? c->a[1] : 0u, 1u);
    chk("s8b.area", rd16(SLOT4 + 0x86u), 4u);
    chk("s8b.state", rd16(SLOT4 + 0x20u), 9u);

    /* ---- state 8 busy!=1: 97770(2,1) then (5,8) to pool+0x106 ---- */
    reset();
    wr16(SLOT4 + 0x20u, 8u);
    wr8(G_S2PRES, 1u);
    wr8(G_BUSY2, 0u);
    wr8(G_AREA, 4u);
    r = run();
    c = find_call("97770", 0);
    chk("s8f.0.a0", c ? c->a[0] : 0u, 2u);
    chk("s8f.0.a1", c ? c->a[1] : 0u, 1u);
    c = find_call("97770", 1);
    chk("s8f.1.a0", c ? c->a[0] : 0u, 5u);
    chk("s8f.1.a1", c ? c->a[1] : 0u, 8u);
    chk("s8f.nbr", rd16(POOL_GUEST + 0x106u), 4u);
    chk("s8f.state", rd16(SLOT4 + 0x20u), 9u);

    /* ---- state 0x0A falls into 0x0B ---- */
    reset();
    wr16(SLOT4 + 0x20u, 0xAu);
    wr8(G_AREA, 1u);
    wr32(POOL_GUEST + 0x80u + 0x28u, 0xABC000u);
    wr32(POOL_GUEST + 0x80u + 0x30u, 0xDEF000u);
    s_f_8bec8 = 3;
    r = run();
    chk("sa.state", rd16(SLOT4 + 0x20u), 0xCu);
    chk("sa.t50", rd32(SLOT4 + 0x50u), 0xABCu);
    chk("sa.n8bec8", count_calls("8bec8"), 1u);
    c = find_call("8c1dc", 0);
    chk("sa.8c1dc", c ? c->a[0] : 0u, 0x2Cu);

    /* ---- state 0x0C handoff ---- */
    reset();
    wr16(SLOT4 + 0x20u, 0xCu);
    wr8(G_AREA, 2u);
    r = run();
    c = find_call("97770", 0);
    chk("sc.a0", c ? c->a[0] : 0u, 2u);
    chk("sc.a1", c ? c->a[1] : 0u, 4u);
    chk("sc.state", rd16(SLOT4 + 0x20u), 2u);
    chk("sc.24", rd16(SLOT4 + 0x24u), 1u);
    c = find_call("894c8", 0);
    chk("sc.rec", c ? c->a[0] : 0u, 0x2Cu);

    /* ---- state 0x10 mode 3 AND busy ---- */
    reset();
    wr16(SLOT4 + 0x20u, 0x10u);
    wr32(G_MODE_WORD, 3u);
    wr8(G_BUSY2, 1u);
    wr8(G_BUSY3, 0u);
    r = run();
    chk("m3h.state", rd16(SLOT4 + 0x20u), 0x10u);

    reset();
    wr16(SLOT4 + 0x20u, 0x10u);
    wr32(G_MODE_WORD, 3u);
    wr8(G_BUSY2, 1u);
    wr8(G_BUSY3, 1u);
    r = run();
    chk("m3a.state", rd16(SLOT4 + 0x20u), 0x11u);

    reset();
    wr16(SLOT4 + 0x20u, 0x10u);
    wr32(G_MODE_WORD, 1u);
    r = run();
    chk("m1.state", rd16(SLOT4 + 0x20u), 1u);

    /* ---- state 0x12 -> 0x40 ---- */
    reset();
    wr16(SLOT4 + 0x20u, 0x12u);
    wr8(G_S3PRES, 0xFFu);
    r = run();
    chk("s12.state", rd16(SLOT4 + 0x20u), 0x40u);

    /* ---- state 0x20 falls into 0x21 ---- */
    reset();
    wr16(SLOT4 + 0x20u, 0x20u);
    wr8(G_S2PRES, 1u);
    wr8(G_S3PRES, 0xFFu);
    r = run();
    chk("s20.state", rd16(SLOT4 + 0x20u), 0x22u);
    chk("s20.n97770", count_calls("97770"), 2u);
    c = find_call("97770", 0);
    chk("s20.0.a0", c ? c->a[0] : 0u, 5u);
    chk("s20.0.a1", c ? c->a[1] : 0u, 2u);
    c = find_call("97770", 1);
    chk("s20.1.a0", c ? c->a[0] : 0u, 1u);
    chk("s20.1.a1", c ? c->a[1] : 0u, 2u);
    chk("s20.idx", rd16(POOL_GUEST + 0x86u), 4u);

    /* ---- state 0x22 -> 0 ---- */
    reset();
    wr16(SLOT4 + 0x20u, 0x22u);
    r = run();
    chk("s22.state", rd16(SLOT4 + 0x20u), 0u);

    /* ---- state 0x32 ring fill ---- */
    reset();
    wr16(SLOT4 + 0x20u, 0x32u);
    wr16(G_RING_IDX, 0x15u);
    r = run();
    chk("s32.state", rd16(SLOT4 + 0x20u), 1u);
    chk("s32.flag", rd32(G_MODE_FLAG), 2u);
    chk("s32.idx", rd16(G_RING_IDX), 0u);
    chk("s32.e0x", rd32(G_RING), 0x5000u << 12);
    chk("s32.e0h", rd16(G_RING + 0x10u), 0x400u);
    chk("s32.e31x", rd32(G_RING + 31u * 0x14u), 0x5000u << 12);
    expect_74794("s32", 1u);

    if (s_failures != 0) {
        fprintf(stderr, "%d assertion(s) failed\n", s_failures);
        return 1;
    }
    printf("W34B24-I10 0x8008C844 focused oracle PASS\n");
    return 0;
}
