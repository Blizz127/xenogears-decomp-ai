/*
 * Focused production-linked oracle for retail helper 0x800740B8.
 *
 * Expected values are hand-derived from [0x800740B8, 0x80074594).
 * PsyQ/GTE residents are recording stubs. NEXT_CALLBACK_TARGET is not
 * an oracle input.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_740b8.h"

#define ANGLE   0x8009BD3Au
#define INDEX   0x8009D7F0u
#define POS     0x8009D55Cu
#define POS_Z   0x8009D564u
#define BCDC    0x8009BCDCu
#define BE0C    0x8009BE0Cu
#define DB_PTR  0x8009BE3Cu
#define DB      0x8009BBC8u
#define OT      0x800F0000u
#define VERTS   0x8009A340u
#define G3      0x8009C664u
#define C5A0    0x8009C5A0u
#define C5C0    0x8009C5C0u
#define F3      0x8009C898u
#define XY_TAB  0x8009B6F4u
#define BITS    0x8007F160u
#define CASE18  0x8007EE60u
#define CASE19  0x8007EE82u
#define CASE1A  0x8007EE78u
#define CANARY  0x80090000u
#define SC_ANG  0x1F8000B8u
#define SC_MTX  0x1F8000F0u
#define SC_X    0x1F800104u
#define SC_Y    0x1F800108u
#define SC_Z    0x1F80010Cu

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

typedef struct {
    s16 vx;
    s16 vy;
    s16 vz;
    s16 pad;
} SVECTOR;

typedef struct {
    s16 m[3][3];
    s32 t[3];
} MATRIX;

static int s_nrot;
static int s_nsetrot;
static int s_nsettrans;
static int s_nrtpt;
static SVECTOR s_ang;
static u32 s_rot_m;
static u32 s_rtpt_v0[4];
static u32 s_rtpt_sxy0[4];

static u32 guest_of(const void *p)
{
    uintptr_t off = (uintptr_t)p - (uintptr_t)g_PsxRam;

    if (off >= 0x180000u && off < 0x180400u)
        return 0x1F800000u + (u32)(off - 0x180000u);
    if (off < 0x400u)
        return 0x1F800000u + (u32)off;
    return 0x80000000u | (u32)off;
}

MATRIX *RotMatrix(SVECTOR *r, MATRIX *m)
{
    s_ang = *r;
    s_rot_m = guest_of(m);
    s_nrot++;
    memset(m, 0, sizeof(*m));
    m->m[0][0] = 0x1000;
    m->t[0] = 0x11;
    m->t[1] = 0x22;
    m->t[2] = 0x33;
    return m;
}

void SetRotMatrix(MATRIX *m)
{
    (void)m;
    s_nsetrot++;
}

void SetTransMatrix(MATRIX *m)
{
    (void)m;
    s_nsettrans++;
}

void wm_740b8_test_rtpt(u32 v0, u32 v1, u32 v2, u32 sxy0, u32 sxy1, u32 sxy2)
{
    u32 i = (u32)s_nrtpt;

    (void)v1;
    (void)v2;
    if (i < 4u) {
        s_rtpt_v0[i] = v0;
        s_rtpt_sxy0[i] = sxy0;
    }
    wr32(sxy0, 0x100u + i);
    wr32(sxy1, 0x200u + i);
    wr32(sxy2, 0x300u + i);
    s_nrtpt++;
}

void wm_740b8_test_store(u32 address, u32 value)
{
    (void)address;
    (void)value;
}

static void reset(void)
{
    s_nrot = s_nsetrot = s_nsettrans = s_nrtpt = 0;
    memset(&s_ang, 0, sizeof(s_ang));
    s_rot_m = 0;
    memset(s_rtpt_v0, 0, sizeof(s_rtpt_v0));
    memset(s_rtpt_sxy0, 0, sizeof(s_rtpt_sxy0));
}

static void seed(u32 index, u32 bits)
{
    u32 i;

    PsxMemory_Init();
    reset();
    wr32(INDEX, index);
    wr16(ANGLE, 0x02C0u);
    wr32(POS, 0x7FFF0000u);
    wr32(POS_Z, 0x7FFF0000u);
    wr32(BCDC, 0xAABBCCDDu);
    wr32(BE0C, 0x10u);
    wr32(DB_PTR, DB);
    wr32(DB + 0x70u, OT);
    wr32(OT, 0xA5002008u);
    wr32(BITS, bits);
    wr32(CANARY, 0x11111111u);
    wr16(XY_TAB, 0x0010u);
    wr16(XY_TAB + 2u, 0x0020u);
    wr16(CASE18, 0x1234u);
    wr16(CASE18 + 4u, 0x1234u);
    wr16(CASE19, 0x0200u);
    wr16(CASE19 + 4u, 0x0200u);
    wr16(CASE1A, 0x03E8u);
    wr16(CASE1A + 2u, 0x03E8u);
    for (i = 0u; i < 4u; i++) {
        wr32(G3 + index * 0x70u + i * 0x1Cu, 0x06000100u + i);
        wr32(VERTS + i * 0x18u, 0x00010000u + i);
    }
    wr32(C5A0, 0x04000011u);
    wr32(C5C0 + index * 0x28u, 0x09000022u);
    for (i = 0u; i < 0x20u; i++)
        wr32(F3 + (index << 9) + i * 0x10u, 0x03000000u + i);
}

static u32 lo(u32 a)
{
    return a & 0x00FFFFFFu;
}

int main(void)
{
    u32 i;
    u32 g3;
    u32 f3;
    u32 ft4;
    u32 prev;

    seed(1u, 0x07000001u);
    wm_800740B8();

    chk("nrot", (u32)s_nrot, 4u);
    chk("nsetrot", (u32)s_nsetrot, 4u);
    chk("nsettrans", (u32)s_nsettrans, 4u);
    chk("nrtpt", (u32)s_nrtpt, 4u);
    chk("ang.vx", (u32)(u16)s_ang.vx, 0u);
    chk("ang.vy", (u32)(u16)s_ang.vy, 0u);
    chk("ang.vz", (u32)(u16)s_ang.vz, 0x02C0u);
    chk("rot.m", s_rot_m, SC_MTX);
    chk("sc.ang0", (u32)rd16(SC_ANG), 0u);
    chk("sc.ang2", (u32)rd16(SC_ANG + 2u), 0u);
    chk("sc.ang4", (u32)rd16(SC_ANG + 4u), 0x02C0u);
    chk("sc.x", rd32(SC_X), 0x6B0u);
    chk("sc.y", rd32(SC_Y), 0x669u);
    chk("sc.z", rd32(SC_Z), 0xAABBCCDDu);
    chk("canary", rd32(CANARY), 0x11111111u);

    g3 = G3 + 0x70u;
    for (i = 0u; i < 4u; i++) {
        char n[32];

        snprintf(n, sizeof n, "v0.%u", i);
        chk(n, s_rtpt_v0[i], VERTS + i * 0x18u);
        snprintf(n, sizeof n, "sxy0.%u", i);
        chk(n, s_rtpt_sxy0[i], g3 + i * 0x1Cu + 8u);
        snprintf(n, sizeof n, "xy0.%u", i);
        chk(n, rd32(g3 + i * 0x1Cu + 8u), 0x100u + i);
        snprintf(n, sizeof n, "xy1.%u", i);
        chk(n, rd32(g3 + i * 0x1Cu + 0x10u), 0x200u + i);
        snprintf(n, sizeof n, "xy2.%u", i);
        chk(n, rd32(g3 + i * 0x1Cu + 0x18u), 0x300u + i);
    }

    f3 = F3 + 0x200u;
    chk("f3.0.x", (u32)rd16(f3 + 8u), 0x00E0u);
    chk("f3.0.y", (u32)rd16(f3 + 0xAu), 0x0098u);
    chk("f3.24.x", (u32)rd16(f3 + 24u * 0x10u + 8u), 0x00DDu);
    chk("f3.24.y", (u32)rd16(f3 + 24u * 0x10u + 0xAu), 0x0084u);
    chk("f3.25.x", (u32)rd16(f3 + 25u * 0x10u + 8u), 0x00D0u);
    chk("f3.25.y", (u32)rd16(f3 + 25u * 0x10u + 0xAu), 0x0078u);
    chk("f3.26.x", (u32)rd16(f3 + 26u * 0x10u + 8u), 0x00D2u);
    chk("f3.26.y", (u32)rd16(f3 + 26u * 0x10u + 0xAu), 0x0079u);
    chk("f3.1.tag", rd32(f3 + 0x10u), 0x03000001u);

    ft4 = C5C0 + 0x28u;
    chk("ot", rd32(OT), 0xA5000000u | lo(ft4));
    chk("ft4.tag", rd32(ft4), 0x09000000u | lo(f3 + 26u * 0x10u));
    chk("f3.26.tag", rd32(f3 + 26u * 0x10u), 0x03000000u | lo(f3 + 25u * 0x10u));
    chk("f3.25.tag", rd32(f3 + 25u * 0x10u), 0x03000000u | lo(f3 + 24u * 0x10u));
    chk("f3.24.tag", rd32(f3 + 24u * 0x10u), 0x03000000u | lo(f3));
    chk("f3.0.tag", rd32(f3), 0x03000000u | lo(C5A0));
    chk("c5a0.tag", rd32(C5A0), 0x04000000u | lo(g3 + 3u * 0x1Cu));
    prev = 0xA5002008u;
    chk("g3.0.tag", rd32(g3), 0x06000000u | lo(prev));
    chk("g3.1.tag", rd32(g3 + 0x1Cu), 0x06000000u | lo(g3));
    chk("g3.2.tag", rd32(g3 + 0x38u), 0x06000000u | lo(g3 + 0x1Cu));
    chk("g3.3.tag", rd32(g3 + 0x54u), 0x06000000u | lo(g3 + 0x38u));

    seed(0u, 0u);
    wm_800740B8();
    chk("z.nrot", (u32)s_nrot, 4u);
    chk("z.nrtpt", (u32)s_nrtpt, 4u);
    chk("z.sc.x", rd32(SC_X), 0x6B0u);
    chk("z.f3.0.tag", rd32(F3), 0x03000000u);
    chk("z.ot", rd32(OT), 0xA5000000u | lo(C5C0));
    chk("z.ft4.tag", rd32(C5C0), 0x09000000u | lo(C5A0));
    chk("z.c5a0.tag", rd32(C5A0), 0x04000000u | lo(G3 + 3u * 0x1Cu));
    chk("z.canary", rd32(CANARY), 0x11111111u);

    if (s_failures) {
        fprintf(stderr, "ASSERTION failures=%d\n", s_failures);
        return 1;
    }
    printf("W34B24-I29 0x800740B8 focused oracle PASS\n");
    return 0;
}
