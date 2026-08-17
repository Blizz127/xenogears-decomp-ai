/*
 * Focused production-linked oracle for retail world callback 0x8008D678.
 *
 * Expected values are hand-derived from the retail disassembly of
 * [0x8008D678, 0x8008DD6C).  All callees are routed through recording
 * seams; callee results are forced per case.  NEXT_CALLBACK_TARGET is
 * a runtime measurement, not an oracle input.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_callback_8d678.h"

#define POOL_GUEST   0x800D7538u
#define SLOT5        (POOL_GUEST + 0x280u)
#define SLOT6        (POOL_GUEST + 0x300u)
#define SLOT2        (POOL_GUEST + 0x100u)
#define SLOT4        (POOL_GUEST + 0x200u)
#define SLOT7        (POOL_GUEST + 0x380u)
#define SC           0x1F800000u
#define OBJ_BITS     0x00200000u

#define G_POOL_PTR   0x8009BE24u
#define G_MODE_FLAG  0x8009BE10u
#define G_RING_IDX   0x8009D154u
#define G_RING       0x8009CEC4u
#define G_FOLLOW     0x8006F8E1u
#define G_SLOT_BYTE  0x8006F364u
#define G_PUBX       0x8006EF96u
#define G_PUBZ       0x8006EF98u
#define G_PUBH       0x8006EE5Cu
#define G_PUBX4      0x8006EF90u
#define G_PUBZ4      0x8006EF92u
#define G_PUBH4      0x8006EE5Au

#define CAN_X        0xBEEFu
#define CAN_Z        0xB0B0u
#define CAN_H        0xB1B1u
#define CAN4_X       0xCA11u
#define CAN4_Z       0xCA22u
#define CAN4_H       0xCA33u

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

void wm_8d678_test_store(u32 address, u32 width, u32 value)
{
    (void)address;
    (void)width;
    (void)value;
}

static s32 s_f_97770;
static s32 s_f_8bec8;
static long s_f_rcos = 100;
static long s_f_rsin = 60;
static long s_f_ratan2 = 0x100;

void wm_8d678_test_245d8(u32 o, s16 n) { log_call("245d8", o, (u32)(s32)n, 0, 0, 0); }
void wm_8d678_test_894c8(u32 i) { log_call("894c8", i, 0, 0, 0, 0); }
void wm_8d678_test_8c1dc(u32 c, u32 o, u32 u) { log_call("8c1dc", c, o, u, 0, 0); }
s32 wm_8d678_test_941c4(u32 a, u32 b, u32 o, u32 g)
{
    log_call("941c4", a, b, o, g, 0);
    return 0;
}
s32 wm_8d678_test_8bec8(u32 o) { log_call("8bec8", o, 0, 0, 0, 0); return s_f_8bec8; }
s32 wm_8d678_test_97770(u32 s, s32 v) { log_call("97770", s, (u32)v, 0, 0, 0); return s_f_97770; }
void wm_8d678_test_8dff4(u32 o) { log_call("8dff4", o, 0, 0, 0, 0); }
void wm_8d678_test_89160(u32 a0, u32 a1, u32 a2)
{
    log_call("89160", a0, a1, a2, 0, 0);
}
long wm_8d678_test_rcos(long a) { log_call("rcos", (u32)a, 0, 0, 0, 0); return s_f_rcos; }
long wm_8d678_test_rsin(long a) { log_call("rsin", (u32)a, 0, 0, 0, 0); return s_f_rsin; }
long wm_8d678_test_ratan2(long y, long x)
{
    log_call("ratan2", (u32)y, (u32)x, 0, 0, 0);
    return s_f_ratan2;
}
void wm_8d678_test_74794(s32 t, u32 p) { log_call("74794", (u32)t, p, 0, 0, 0); }

static void set_anim(u8 v)
{
    u8* p = (u8*)PSX_ADDR(OBJ_BITS) + 0xAF;

    *p = v;
}

static void reset_slot(u32 slot)
{
    wr16(slot + 4u, 0u);
    wr16(slot + 6u, 0u);
    wr16(slot + 0x20u, 0u);
    wr16(slot + 0x22u, 0u);
    wr16(slot + 0x24u, 0u);
    wr32(slot + 0x28u, 0x5000u << 12);
    wr32(slot + 0x2Cu, 0x100u);
    wr32(slot + 0x30u, 0x3000u << 12);
    wr32(slot + 0x38u, 0x11u);
    wr32(slot + 0x3Cu, 0x22u);
    wr32(slot + 0x40u, 0x33u);
    wr16(slot + 0x48u, 0x400u);
    wr32(slot + 0x4Cu, OBJ_BITS);
    wr32(slot + 0x50u, 0u);
    wr32(slot + 0x54u, 0u);
    wr32(slot + 0x58u, 0u);
}

static void reset(void)
{
    u32 i;

    s_ncalls = 0;
    s_f_97770 = 1;
    s_f_8bec8 = 0;
    s_f_rcos = 100;
    s_f_rsin = 60;
    s_f_ratan2 = 0x100;
    wr32(G_POOL_PTR, POOL_GUEST);
    for (i = 0; i < 0x400u; i += 4u)
        wr32(POOL_GUEST + i, 0u);
    for (i = 0; i < 0x30u * 0x14u; i += 4u)
        wr32(G_RING + i, 0u);
    wr32(G_MODE_FLAG, 0x1234u);
    wr16(G_RING_IDX, 0u);
    wr8(G_FOLLOW + 5u, 0u);
    wr8(G_FOLLOW + 6u, 0u);
    wr8(0x8006F8E4u, 0u);
    wr8(G_SLOT_BYTE + 5u, 0u);
    wr8(G_SLOT_BYTE + 6u, 0u);
    wr16(G_PUBX, CAN_X);
    wr16(G_PUBZ, CAN_Z);
    wr16(G_PUBH, CAN_H);
    wr16(G_PUBX4, CAN4_X);
    wr16(G_PUBZ4, CAN4_Z);
    wr16(G_PUBH4, CAN4_H);
    reset_slot(SLOT5);
    reset_slot(SLOT6);
    reset_slot(SLOT2);
    reset_slot(SLOT4);
    reset_slot(SLOT7);
    wr32(SLOT7 + 0x28u, 0xAAA000u);
    wr32(SLOT7 + 0x2Cu, 0xBBB000u);
    wr32(SLOT7 + 0x30u, 0xCCC000u);
    wr16(SLOT7 + 0x48u, 0x777u);
    wr32(SLOT4 + 0x28u, 0x111000u);
    wr32(SLOT4 + 0x30u, 0x222000u);
    set_anim(0);
}

static s32 run5(void)
{
    return wm_8008D678(5);
}

static s32 run6(void)
{
    return wm_8008D678(6);
}

static void expect_pub5(const char* tag)
{
    char n[64];

    snprintf(n, sizeof(n), "%s.pubx", tag);
    chk(n, rd16(G_PUBX), (u16)(rd32(SLOT5 + 0x28u) >> 12));
    snprintf(n, sizeof(n), "%s.pubz", tag);
    chk(n, rd16(G_PUBZ), (u16)(rd32(SLOT5 + 0x30u) >> 12));
    snprintf(n, sizeof(n), "%s.pubh", tag);
    chk(n, rd16(G_PUBH), rd16(SLOT5 + 0x48u));
    snprintf(n, sizeof(n), "%s.pubx4", tag);
    chk(n, rd16(G_PUBX4), CAN4_X);
    snprintf(n, sizeof(n), "%s.pubz4", tag);
    chk(n, rd16(G_PUBZ4), CAN4_Z);
    snprintf(n, sizeof(n), "%s.pubh4", tag);
    chk(n, rd16(G_PUBH4), CAN4_H);
}

static void expect_74794(const char* tag, u32 want_n, u32 pose)
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
            chk(n, c->a[1], pose);
        }
    }
}

int main(void)
{
    s32 r;
    const Call* c;

    PsxMemory_Init();

    /* ---- substate 4 -> state 0x40 (parked) + follow=1 ---- */
    reset();
    wr16(SLOT5 + 4u, 4u);
    wr16(SLOT5 + 0x20u, 0x13u);
    r = run5();
    chk("sub4.ret", (u32)r, 1u);
    chk("sub4.sub", rd16(SLOT5 + 4u), 0u);
    chk("sub4.state", rd16(SLOT5 + 0x20u), 0x40u);
    chk("sub4.follow", rd8(G_FOLLOW + 5u), 1u);
    expect_pub5("sub4");
    expect_74794("sub4", 1u, SLOT5 + 0x28u);

    /* ---- substate 1 -> state 8, neighbor from +6, falls into 9 ---- */
    reset();
    wr16(SLOT5 + 4u, 1u);
    wr16(SLOT5 + 6u, 2u);
    wr32(SLOT2 + 0x28u, 0xABC000u);
    wr32(SLOT2 + 0x30u, 0xDEF000u);
    s_f_8bec8 = 3;
    r = run5();
    chk("sub1.ret", (u32)r, 1u);
    chk("sub1.sub", rd16(SLOT5 + 4u), 0u);
    chk("sub1.state", rd16(SLOT5 + 0x20u), 0xAu);
    chk("sub1.t50", rd32(SLOT5 + 0x50u), 0xABCu);
    chk("sub1.t54", rd32(SLOT5 + 0x54u), 0xDEFu);
    chk("sub1.n941c4", count_calls("941c4"), 1u);
    c = find_call("941c4", 0);
    chk("sub1.941c4.b", c ? c->a[1] : 0u, SLOT2 + 0x28u);
    chk("sub1.n8bec8", count_calls("8bec8"), 1u);
    c = find_call("8c1dc", 0);
    chk("sub1.rec", c ? c->a[0] : 0u, 0x2Du);
    c = find_call("245d8", 0);
    chk("sub1.anim", c ? c->a[1] : 99u, 1u);

    /* ---- substate 2 -> 0x20 (does not fall into 0x21) ---- */
    reset();
    wr16(SLOT5 + 4u, 2u);
    r = run5();
    chk("sub2.state", rd16(SLOT5 + 0x20u), 0x21u);
    chk("sub2.n245d8", count_calls("245d8"), 1u);
    chk("sub2.n97770", count_calls("97770"), 0u);
    chk("sub2.38", rd32(SLOT5 + 0x38u), 0u);

    /* ---- substate 3 -> 0x30 (does not fall into 0x31) ---- */
    reset();
    wr16(SLOT5 + 4u, 3u);
    wr32(POOL_GUEST + 0x3F8u, 0x800u);
    r = run5();
    chk("sub3.state", rd16(SLOT5 + 0x20u), 0x31u);
    chk("sub3.n8dff4", count_calls("8dff4"), 1u);
    chk("sub3.n8bec8", count_calls("8bec8"), 0u);
    chk("sub3.fx", rd32(SC + 0u), (0x5000u << 12) + (u32)s_f_rcos * 96u);
    chk("sub3.fz", rd32(SC + 8u),
        (0x3000u << 12) + (0u - (u32)s_f_rsin) * 96u);

    /* ---- substate 5 -> 0x10, falls into 0x11 ---- */
    reset();
    wr16(SLOT5 + 4u, 5u);
    wr32(SLOT4 + 0x28u, 0x7000u << 12);
    wr32(SLOT4 + 0x30u, 0x8000u << 12);
    r = run5();
    chk("sub5.state", rd16(SLOT5 + 0x20u), 0x11u);
    chk("sub5.head", rd16(SLOT5 + 0x48u), 0x500u);
    chk("sub5.n8bec8", count_calls("8bec8"), 1u);
    c = find_call("ratan2", 0);
    chk("sub5.ratan2.y", c ? c->a[0] : 0u,
        (0x8000u << 12) - (0x3000u << 12));
    chk("sub5.ratan2.x", c ? c->a[1] : 0u,
        (0x7000u << 12) - (0x5000u << 12));
    chk("sub5.velx", rd32(SLOT5 + 0x38u), (u32)s_f_rcos);
    chk("sub5.velz", rd32(SLOT5 + 0x40u), 0u - (u32)s_f_rsin);

    /* ---- substate 8 -> 0x18 ---- */
    reset();
    wr16(SLOT5 + 4u, 8u);
    r = run5();
    chk("sub8.state", rd16(SLOT5 + 0x20u), 0x19u);
    chk("sub8.timer", rd16(SLOT5 + 0x22u), 8u);
    c = find_call("89160", 0);
    chk("sub8.89160.a0", c ? c->a[0] : 0u, 6u);
    chk("sub8.89160.a1", c ? c->a[1] : 0u, SC + 0xA0u);

    /* ---- substate 6/7 are no-ops ---- */
    reset();
    wr16(SLOT5 + 4u, 6u);
    wr16(SLOT5 + 0x20u, 0x13u);
    r = run5();
    chk("sub6.state", rd16(SLOT5 + 0x20u), 0x13u);
    chk("sub6.sub", rd16(SLOT5 + 4u), 6u);

    /* ---- JT1 OOR 0x41 ---- */
    reset();
    wr16(SLOT5 + 0x20u, 0x41u);
    r = run5();
    chk("oor.ret", (u32)r, 1u);
    expect_74794("oor", 1u, SLOT5 + 0x28u);
    expect_pub5("oor");

    /* ---- parked 0x13 ---- */
    reset();
    wr16(SLOT5 + 0x20u, 0x13u);
    r = run5();
    chk("park.ret", (u32)r, 1u);
    chk("park.calls", s_ncalls, 1u);
    expect_74794("park", 1u, SLOT5 + 0x28u);

    /* ---- follow==1, xyz match, AF!=0: 245D8(0)+894C8(slot+0x28) ---- */
    reset();
    wr8(G_FOLLOW + 5u, 1u);
    wr32(SLOT5 + 0x28u, 0xAAu);
    wr32(SLOT5 + 0x2Cu, 0xBBu);
    wr32(SLOT5 + 0x30u, 0xCCu);
    wr32(G_RING + 0u, 0xAAu);
    wr32(G_RING + 4u, 0xBBu);
    wr32(G_RING + 8u, 0xCCu);
    wr16(G_RING + 0x10u, 0x321u);
    set_anim(2);
    r = run5();
    chk("fm.ret", (u32)r, 1u);
    chk("fm.head", rd16(SLOT5 + 0x48u), 0x321u);
    chk("fm.n245d8", count_calls("245d8"), 1u);
    c = find_call("245d8", 0);
    chk("fm.anim", c ? c->a[1] : 99u, 0u);
    c = find_call("894c8", 0);
    chk("fm.rec", c ? c->a[0] : 0u, 0x2Du);
    chk("fm.n8c1dc", count_calls("8c1dc"), 0u);
    expect_pub5("fm");

    /* ---- follow==1, match, AF==0: skip 245D8/894C8, still copy ---- */
    reset();
    wr8(G_FOLLOW + 5u, 1u);
    wr32(SLOT5 + 0x28u, 0xAAu);
    wr32(SLOT5 + 0x2Cu, 0xBBu);
    wr32(SLOT5 + 0x30u, 0xCCu);
    wr32(G_RING + 0u, 0xAAu);
    wr32(G_RING + 4u, 0xBBu);
    wr32(G_RING + 8u, 0xCCu);
    wr16(G_RING + 0x10u, 0x321u);
    set_anim(0);
    r = run5();
    chk("fm0.n245d8", count_calls("245d8"), 0u);
    chk("fm0.n894c8", count_calls("894c8"), 0u);
    chk("fm0.head", rd16(SLOT5 + 0x48u), 0x321u);

    /* ---- follow==1, mismatch: 8C1DC + copy ring onto slot ---- */
    reset();
    wr8(G_FOLLOW + 5u, 1u);
    wr32(SLOT5 + 0x28u, 0x111u);
    wr32(SLOT5 + 0x2Cu, 0x222u);
    wr32(SLOT5 + 0x30u, 0x333u);
    wr32(G_RING + 0u, 0xAAu);
    wr32(G_RING + 4u, 0xBBu);
    wr32(G_RING + 8u, 0xCCu);
    wr16(G_RING + 0x10u, 0x321u);
    set_anim(0);
    r = run5();
    chk("fmm.x", rd32(SLOT5 + 0x28u), 0xAAu);
    chk("fmm.y", rd32(SLOT5 + 0x2Cu), 0xBBu);
    chk("fmm.z", rd32(SLOT5 + 0x30u), 0xCCu);
    chk("fmm.head", rd16(SLOT5 + 0x48u), 0x321u);
    chk("fmm.n245d8", count_calls("245d8"), 1u);
    c = find_call("8c1dc", 0);
    chk("fmm.rec", c ? c->a[0] : 0u, 0x2Du);
    chk("fmm.obj", c ? c->a[1] : 0u, SLOT5);
    chk("fmm.out", c ? c->a[2] : 0u, SC);

    /* ---- follow==1, mismatch, AF==1: skip 245D8, still 8C1DC ---- */
    reset();
    wr8(G_FOLLOW + 5u, 1u);
    wr32(SLOT5 + 0x28u, 0x111u);
    wr32(G_RING + 0u, 0xAAu);
    set_anim(1);
    r = run5();
    chk("fmm1.n245d8", count_calls("245d8"), 0u);
    chk("fmm1.n8c1dc", count_calls("8c1dc"), 1u);

    /* ---- ring mask: (0 - 0x20) & 0x1F == 0, not 0x20 ---- */
    reset();
    wr8(G_FOLLOW + 5u, 1u);
    wr16(G_RING_IDX, 0u);
    wr32(SLOT5 + 0x58u, 0x20u);
    wr32(SLOT5 + 0x28u, 0xAAu);
    wr32(SLOT5 + 0x2Cu, 0xBBu);
    wr32(SLOT5 + 0x30u, 0xCCu);
    wr32(G_RING + 0u, 0xAAu);
    wr32(G_RING + 4u, 0xBBu);
    wr32(G_RING + 8u, 0xCCu);
    wr16(G_RING + 0x10u, 0x321u);
    set_anim(2);
    r = run5();
    chk("ring.head", rd16(SLOT5 + 0x48u), 0x321u);
    chk("ring.n894c8", count_calls("894c8"), 1u);
    chk("ring.n8c1dc", count_calls("8c1dc"), 0u);

    /* ---- follow!=1, AF!=3: 245D8(3)+894C8(slot+0x28) ---- */
    reset();
    set_anim(1);
    r = run5();
    chk("dorm.n90", count_calls("245d8"), 1u);
    c = find_call("245d8", 0);
    chk("dorm.anim", c ? c->a[1] : 99u, 3u);
    c = find_call("894c8", 0);
    chk("dorm.rec", c ? c->a[0] : 0u, 0x2Du);

    /* ---- follow!=1, AF==3: skip ---- */
    reset();
    set_anim(3);
    r = run5();
    chk("d3.n245d8", count_calls("245d8"), 0u);
    chk("d3.n894c8", count_calls("894c8"), 0u);

    /* ---- slot 6 uses record 0x2E and publish +12 ---- */
    reset();
    set_anim(1);
    r = run6();
    chk("s6.ret", (u32)r, 1u);
    c = find_call("894c8", 0);
    chk("s6.rec", c ? c->a[0] : 0u, 0x2Eu);
    chk("s6.pubx", rd16(0x8006EF9Cu), (u16)(rd32(SLOT6 + 0x28u) >> 12));
    chk("s6.pubh", rd16(0x8006EE5Eu), rd16(SLOT6 + 0x48u));
    expect_74794("s6", 1u, SLOT6 + 0x28u);

    /* ---- state 2: copy X/Z/heading, skip Y, skip 74794 ---- */
    reset();
    wr16(SLOT5 + 0x20u, 2u);
    wr32(SLOT5 + 0x2Cu, 0xDEADBEEFu);
    r = run5();
    chk("s2.x", rd32(SLOT5 + 0x28u), 0xAAA000u);
    chk("s2.y", rd32(SLOT5 + 0x2Cu), 0xDEADBEEFu);
    chk("s2.z", rd32(SLOT5 + 0x30u), 0xCCC000u);
    chk("s2.h", rd16(SLOT5 + 0x48u), 0x777u);
    expect_74794("s2", 0u, SLOT5 + 0x28u);
    expect_pub5("s2");

    /* ---- 74794 skip when slot-byte==7 and MODE_FLAG!=2 ---- */
    reset();
    wr16(SLOT5 + 0x20u, 0x13u);
    wr8(G_SLOT_BYTE + 5u, 7u);
    r = run5();
    expect_74794("pres7", 0u, SLOT5 + 0x28u);
    expect_pub5("pres7");

    /* ---- 74794 called when MODE_FLAG==2 even if byte==7 ---- */
    reset();
    wr16(SLOT5 + 0x20u, 0x13u);
    wr8(G_SLOT_BYTE + 5u, 7u);
    wr32(G_MODE_FLAG, 2u);
    r = run5();
    expect_74794("mf2", 1u, SLOT5 + 0x28u);

    /* ---- state 9 only (no sub1): 8BEC8 + 8C1DC, no 941C4 ---- */
    reset();
    wr16(SLOT5 + 0x20u, 9u);
    s_f_8bec8 = 3;
    r = run5();
    chk("s9.state", rd16(SLOT5 + 0x20u), 0xAu);
    chk("s9.n941c4", count_calls("941c4"), 0u);
    chk("s9.n8bec8", count_calls("8bec8"), 1u);

    /* ---- state 0x0A handoff ---- */
    reset();
    wr16(SLOT5 + 0x20u, 0xAu);
    wr16(SLOT5 + 6u, 2u);
    r = run5();
    c = find_call("97770", 0);
    chk("sa.a0", c ? c->a[0] : 0u, 2u);
    chk("sa.a1", c ? c->a[1] : 0u, 4u);
    chk("sa.state", rd16(SLOT5 + 0x20u), 2u);
    chk("sa.24", rd16(SLOT5 + 0x24u), 1u);
    c = find_call("894c8", 0);
    chk("sa.rec", c ? c->a[0] : 0u, 0x2Du);
    expect_74794("sa", 0u, SLOT5 + 0x28u);

    reset();
    wr16(SLOT5 + 0x20u, 0xAu);
    s_f_97770 = 0;
    r = run5();
    chk("sah.state", rd16(SLOT5 + 0x20u), 0xAu);
    chk("sah.n894c8", count_calls("894c8"), 0u);

    /* ---- state 0x12: 97770(4,7) -> 1 ---- */
    reset();
    wr16(SLOT5 + 0x20u, 0x12u);
    r = run5();
    c = find_call("97770", 0);
    chk("s12.a0", c ? c->a[0] : 0u, 4u);
    chk("s12.a1", c ? c->a[1] : 0u, 7u);
    chk("s12.state", rd16(SLOT5 + 0x20u), 1u);

    /* ---- state 0x18 byte==7 -> 0x1A timer 1 ---- */
    reset();
    wr16(SLOT5 + 0x20u, 0x18u);
    wr8(G_SLOT_BYTE + 5u, 7u);
    r = run5();
    chk("s18b.state", rd16(SLOT5 + 0x20u), 0x1Au);
    chk("s18b.timer", rd16(SLOT5 + 0x22u), 1u);
    chk("s18b.n89160", count_calls("89160"), 0u);

    /* ---- state 0x19 timer expire ---- */
    reset();
    wr16(SLOT5 + 0x20u, 0x19u);
    wr16(SLOT5 + 0x22u, 1u);
    r = run5();
    chk("s19.state", rd16(SLOT5 + 0x20u), 0x1Au);
    chk("s19.24", rd16(SLOT5 + 0x24u), 1u);
    chk("s19.timer", rd16(SLOT5 + 0x22u), 0x10u);

    /* ---- state 0x1A expire -> anim 3, state 2 ---- */
    reset();
    wr16(SLOT5 + 0x20u, 0x1Au);
    wr16(SLOT5 + 0x22u, 1u);
    r = run5();
    chk("s1a.state", rd16(SLOT5 + 0x20u), 2u);
    c = find_call("245d8", 0);
    chk("s1a.anim", c ? c->a[1] : 99u, 3u);
    expect_74794("s1a", 0u, SLOT5 + 0x28u);

    /* ---- state 0x21: 97770(slot-3,2), publish idx ---- */
    reset();
    wr16(SLOT5 + 0x20u, 0x21u);
    r = run5();
    c = find_call("97770", 0);
    chk("s21.a0", c ? c->a[0] : 0u, 2u);
    chk("s21.a1", c ? c->a[1] : 0u, 2u);
    chk("s21.idx", rd16(POOL_GUEST + 0x106u), 5u);
    chk("s21.state", rd16(SLOT5 + 0x20u), 0x22u);

    /* ---- state 0x22 -> 0 ---- */
    reset();
    wr16(SLOT5 + 0x20u, 0x22u);
    r = run5();
    chk("s22.state", rd16(SLOT5 + 0x20u), 0u);

    /* ---- state 0x31 8BEC8==3 -> idle, state 0x32 ---- */
    reset();
    wr16(SLOT5 + 0x20u, 0x31u);
    s_f_8bec8 = 3;
    r = run5();
    chk("s31.state", rd16(SLOT5 + 0x20u), 0x32u);
    chk("s31.38", rd32(SLOT5 + 0x38u), 0u);
    c = find_call("245d8", 0);
    chk("s31.anim", c ? c->a[1] : 99u, 0u);

    /* ---- state 0x32 -> 1 ---- */
    reset();
    wr16(SLOT5 + 0x20u, 0x32u);
    r = run5();
    chk("s32.state", rd16(SLOT5 + 0x20u), 1u);
    expect_74794("s32", 1u, SLOT5 + 0x28u);

    if (s_failures != 0) {
        fprintf(stderr, "%d assertion(s) failed\n", s_failures);
        return 1;
    }
    printf("W34B24-I11 0x8008D678 focused oracle PASS\n");
    return 0;
}
