/*
 * Focused production-linked oracle for retail helper 0x80089C78.
 *
 * Expected values are hand-derived from [0x80089C78, 0x8008A2C8).
 * PsyQ/GTE residents are recording stubs. Real wm_80093534 is linked.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_89c78.h"

#define POS_X    0x8009BE28u
#define POS_Z    0x8009BE30u
#define INDEX    0x8009D7F0u
#define PKT_TAB  0x8009BE1Cu
#define REC_BASE 0x8009BDF4u
#define DB_PTR   0x8009BE3Cu
#define DB       0x8009BBC8u
#define OT       0x800F0000u
#define CAMERA   0x8009C808u
#define TEMPLATE 0x8009A180u
#define VERTS    0x8009B040u
#define UV       0x8009AFF0u
#define WRAP_X   0x8009D160u
#define WRAP_Z   0x8009D2B4u
#define RECS     0x80090000u
#define PACKET   0x80094000u
#define SC       0x1F800000u
#define SC_SVEC  0x1F800020u
#define SC_CAM   0x1F800028u
#define SC_MTX   0x1F800048u
#define SC_SCALE 0x1F800098u
#define SC_FLAG  0x1F8000ACu
#define SC_SZ    0x1F8000B0u

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

static u8 rd8(u32 a)
{
    u8 v;

    memcpy(&v, PSX_ADDR(a), 1);
    return v;
}

static void wr32(u32 a, u32 v) { memcpy(PSX_ADDR(a), &v, 4); }
static void wr16(u32 a, u16 v) { memcpy(PSX_ADDR(a), &v, 2); }
static void wr8(u32 a, u8 v) { memcpy(PSX_ADDR(a), &v, 1); }

static u32 guest_of(const void *p)
{
    uintptr_t off = (uintptr_t)p - (uintptr_t)g_PsxRam;

    if (off >= 0x180000u && off < 0x180400u)
        return 0x1F800000u + (u32)(off - 0x180000u);
    if (off < 0x400u)
        return 0x1F800000u + (u32)off;
    return 0x80000000u | (u32)off;
}

typedef struct {
    s16 vx;
    s16 vy;
    s16 vz;
    s16 pad;
} SVECTOR;

typedef struct {
    s32 vx;
    s32 vy;
    s32 vz;
    s32 pad;
} VECTOR;

typedef struct {
    s16 m[3][3];
    s32 t[3];
} MATRIX;

static int s_nrotz;
static int s_nscale;
static int s_napply;
static int s_nsetrot;
static int s_nsettrans;
static int s_nrtpt;
static int s_nrtps;
static s32 s_rotz_ang;
static u32 s_rotz_m;
static u32 s_scale_m;
static u32 s_scale_v;
static u32 s_rtpt_flag;
static u32 s_rtps_flag;
static u32 s_rtps_sz;
static u32 s_sxy0;
static u32 s_sxy1;
static u32 s_sxy2;
static u32 s_sxy3;

MATRIX *RotMatrixZ(s32 r, MATRIX *m)
{
    s_rotz_ang = r;
    s_rotz_m = guest_of(m);
    s_nrotz++;
    return m;
}

MATRIX *ScaleMatrix(MATRIX *m, VECTOR *v)
{
    s_scale_m = guest_of(m);
    s_scale_v = guest_of(v);
    s_nscale++;
    return m;
}

VECTOR *ApplyMatrix(MATRIX *m, SVECTOR *v0, VECTOR *v1)
{
    (void)m;
    v1->vx = v0->vx;
    v1->vy = v0->vy;
    v1->vz = v0->vz;
    s_napply++;
    return v1;
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

u32 wm_89c78_test_rtpt(u32 v0, u32 v1, u32 v2)
{
    (void)v0;
    (void)v1;
    (void)v2;
    s_nrtpt++;
    return s_rtpt_flag;
}

u32 wm_89c78_test_rtps(u32 v3, u32 *sz_out)
{
    (void)v3;
    s_nrtps++;
    *sz_out = s_rtps_sz;
    return s_rtps_flag;
}

void wm_89c78_test_write_sxy3(u32 sxy0, u32 sxy1, u32 sxy2)
{
    wr32(sxy0, s_sxy0);
    wr32(sxy1, s_sxy1);
    wr32(sxy2, s_sxy2);
}

void wm_89c78_test_write_sxy(u32 sxy3) { wr32(sxy3, s_sxy3); }

void wm_89c78_test_store(u32 address, u32 value)
{
    (void)address;
    (void)value;
}

static u32 slot_addr(u32 i)
{
    return RECS + 6u + i * 0x4Cu;
}

static void reset(void)
{
    s_nrotz = s_nscale = s_napply = s_nsetrot = s_nsettrans = 0;
    s_nrtpt = s_nrtps = 0;
    s_rotz_ang = 0;
    s_rotz_m = s_scale_m = s_scale_v = 0;
}

static void seed_common(void)
{
    u32 i;

    PsxMemory_Init();
    reset();
    wr32(POS_X, 0x1000u);
    wr32(POS_Z, 0x2000u);
    wr32(INDEX, 0u);
    wr32(PKT_TAB, PACKET);
    wr32(REC_BASE, RECS);
    wr32(DB_PTR, DB);
    wr32(DB + 0x70u, OT);
    wr32(WRAP_X, 1u);
    wr32(WRAP_Z, 1u);
    wr32(CAMERA + 0x14u, 10u);
    wr32(CAMERA + 0x18u, 20u);
    wr32(CAMERA + 0x1Cu, 30u);
    for (i = 0u; i < 8u; i++)
        wr32(TEMPLATE + i * 4u, 0x11110000u + i);
    wr32(OT + 0x200u, 0x12345678u);
    wr32(PACKET, 0xDEADBEEFu);
    wr16(UV + 8u, 0x0102u);
    wr16(UV + 0xAu, 0x0304u);
    wr16(UV + 0xCu, 0x0506u);
    wr16(UV + 0xEu, 0x0708u);
    for (i = 0u; i < 0x100u; i++)
        wr16(slot_addr(i), 0);
}

static void seed_slot0(u16 enable, u8 rotz_bit, u32 flag_pt, u32 flag_ps,
                       u32 sz, u32 sxy)
{
    u32 s0 = slot_addr(0);

    seed_common();
    s_rtpt_flag = flag_pt;
    s_rtps_flag = flag_ps;
    s_rtps_sz = sz;
    s_sxy0 = s_sxy1 = s_sxy2 = s_sxy3 = sxy;
    wr16(s0, enable);
    wr16(s0 - 4u, 0x123u);
    /* 16387<<12: after pos_x=1 this is 16386, which 93534 wraps by 0x800. */
    wr32(s0 + 2u, 0x4003000u);
    wr32(s0 + 6u, 0x3000u);
    wr32(s0 + 0xAu, 0x7000u);
    wr16(s0 + 0x32u, 0x800u);
    wr16(s0 + 0x34u, 0x900u);
    wr8(s0 + 0x3Au, 0x11u);
    wr8(s0 + 0x3Bu, 0x22u);
    wr8(s0 + 0x3Cu, 0x33u);
    wr8(s0 + 0x41u, rotz_bit);
    wr16(s0 + 0x42u, 0xABCDu);
}

int main(void)
{
    seed_slot0(0, 0, 0, 0, 0x800u, 0u);
    wm_80089C78();
    chk("zero.nscale", (u32)s_nscale, 0u);
    chk("zero.nrotz", (u32)s_nrotz, 0u);
    chk("zero.nrtpt", (u32)s_nrtpt, 0u);
    chk("zero.cam0", rd32(SC_CAM), rd32(CAMERA));
    chk("zero.tmp0", rd32(SC + 0x68u), 0x11110000u);

    seed_slot0(1, 0, 0, 0, 0x800u, 0x00000120u);
    wm_80089C78();
    chk("en.nrotz", (u32)s_nrotz, 0u);
    chk("en.nscale", (u32)s_nscale, 1u);
    chk("en.napply", (u32)s_napply, 1u);
    chk("en.nsetrot", (u32)s_nsetrot, 2u);
    chk("en.nsettrans", (u32)s_nsettrans, 1u);
    chk("en.nrtpt", (u32)s_nrtpt, 1u);
    chk("en.nrtps", (u32)s_nrtps, 1u);
    chk("en.scale_m", s_scale_m, SC_MTX);
    chk("en.scale_v", s_scale_v, SC_SCALE);
    chk("en.scale_x", rd32(SC_SCALE), 0x800u);
    chk("en.scale_y", rd32(SC_SCALE + 4u), 0x900u);
    chk("en.scale_z", rd32(SC_SCALE + 8u), 0x1000u);
    chk("en.svec.x", (u32)rd16(SC_SVEC), 0x3802u);
    chk("en.svec.y", (u32)rd16(SC_SVEC + 2u), 3u);
    chk("en.svec.z", (u32)(s16)rd16(SC_SVEC + 4u), (u32)(s16)(-5));
    chk("en.t0", rd32(SC_MTX + 0x14u), 0x3802u + 10u);
    chk("en.t1", rd32(SC_MTX + 0x18u), 23u);
    chk("en.t2", rd32(SC_MTX + 0x1Cu), 25u);
    chk("en.sz", rd32(SC_SZ), 0x800u);
    chk("en.rgb0", (u32)rd8(PACKET + 4u), 0x11u);
    chk("en.rgb1", (u32)rd8(PACKET + 5u), 0x22u);
    chk("en.rgb2", (u32)rd8(PACKET + 6u), 0x33u);
    chk("en.tpage", (u32)rd16(PACKET + 0x16u), 0xABCDu);
    chk("en.uv0", (u32)rd16(PACKET + 0xCu), 0x0102u);
    chk("en.uv1", (u32)rd16(PACKET + 0x14u), 0x0304u);
    chk("en.uv2", (u32)rd16(PACKET + 0x1Cu), 0x0506u);
    chk("en.uv3", (u32)rd16(PACKET + 0x24u), 0x0708u);
    chk("en.tag", rd32(PACKET), 0xDE345678u);
    chk("en.ot", rd32(OT + 0x200u), 0x12000000u | (PACKET & 0x00FFFFFFu));

    seed_slot0(1, 1, 0, 0, 0x800u, 0u);
    wm_80089C78();
    chk("rotz.n", (u32)s_nrotz, 1u);
    chk("rotz.ang", (u32)s_rotz_ang, 0x123u);
    chk("rotz.m", s_rotz_m, SC_MTX);
    chk("rotz.ot", rd32(OT + 0x200u), 0x12000000u | (PACKET & 0x00FFFFFFu));

    seed_slot0(1, 0, 0x80000000u, 0, 0x800u, 0u);
    wm_80089C78();
    chk("ptclip.nrtpt", (u32)s_nrtpt, 1u);
    chk("ptclip.nrtps", (u32)s_nrtps, 0u);
    chk("ptclip.tag", rd32(PACKET), 0xDEADBEEFu);

    seed_slot0(1, 0, 0, 0x80000000u, 0x800u, 0u);
    wm_80089C78();
    chk("psclip.nrtps", (u32)s_nrtps, 1u);
    chk("psclip.tag", rd32(PACKET), 0xDEADBEEFu);

    seed_slot0(1, 0, 0, 0, 0x800u, 0x01400140u);
    wm_80089C78();
    chk("xclip.tag", rd32(PACKET), 0xDEADBEEFu);

    seed_slot0(1, 0, 0, 0, 0x800u, 0x00D80000u);
    wm_80089C78();
    chk("yclip.tag", rd32(PACKET), 0xDEADBEEFu);

    seed_slot0(1, 0, 0, 0, 0xC00u, 0u);
    wm_80089C78();
    chk("far.nrtps", (u32)s_nrtps, 1u);
    chk("far.sz", rd32(SC_SZ), 0xC00u);
    chk("far.tag", rd32(PACKET), 0xDEADBEEFu);

    seed_common();
    s_rtpt_flag = 0;
    s_rtps_flag = 0;
    s_rtps_sz = 0x800u;
    s_sxy0 = s_sxy1 = s_sxy2 = s_sxy3 = 0;
    wr16(slot_addr(0x80u), 1);
    wr32(slot_addr(0x80u) + 2u, 0x5000u);
    wr32(slot_addr(0x80u) + 6u, 0x3000u);
    wr32(slot_addr(0x80u) + 0xAu, 0x7000u);
    wr16(slot_addr(0x80u) + 0x32u, 0x800u);
    wr16(slot_addr(0x80u) + 0x34u, 0x900u);
    wr16(UV, 0x1111u);
    wr16(UV + 2u, 0x2222u);
    wr16(UV + 4u, 0x3333u);
    wr16(UV + 6u, 0x4444u);
    wm_80089C78();
    chk("late.nscale", (u32)s_nscale, 1u);
    chk("late.tag", rd32(PACKET), 0xDE345678u);

    if (s_failures) {
        fprintf(stderr, "ASSERTION failures=%d\n", s_failures);
        return 1;
    }
    printf("W34B24-I28 0x80089C78 focused oracle PASS\n");
    return 0;
}
