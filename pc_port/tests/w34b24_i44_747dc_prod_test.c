/*
 * Focused production-linked oracle for retail helper 0x800747DC.
 *
 * Expected values are hand-derived from [0x800747DC, 0x80074E58).
 * Overlay 93978/93740 and PsyQ/GTE residents are recording stubs.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_747dc.h"

#define COUNT   0x8009BE38u
#define CAM     0x8009C808u
#define RECS_P  0x8009D30Cu
#define INDEX   0x8009D7F0u
#define POS_X   0x8009BE28u
#define POS_Z   0x8009BE30u
#define DESTS   0x8009BE14u
#define MODE2   0x8009A180u
#define ANGLE   0x8006EE66u
#define DB_PTR  0x8009BE3Cu
#define DB      0x8009BBC8u
#define OT      0x800F0000u
#define RECS    0x8009D400u
#define PKT     0x80100000u
#define CANARY  0x80090000u
#define SC      0x1F800000u

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

static int s_failures;
static int s_n93978;
static int s_n93740;
static int s_nop;
static int s_nvn;
static int s_nroty;
static int s_nmul;
static int s_nscale;
static int s_nsetrot;
static int s_nsettrans;
static int s_nrtir;
static int s_nrt;
static int s_nrtpt;
static int s_nrtps;
static s32 s_flag;
static u32 s_sz0;
static u32 s_sz1;
static u32 s_sz2;
static u32 s_sz_rtps;
static s32 s_yret[4];
static s32 s_93978_x[4];
static s32 s_93978_z[4];
static u32 s_93740_dst[4];
static u32 s_scale_x[4];
static u32 s_roty_ang;
static u32 s_roty_m;
static u32 s_mul_m0;
static u32 s_rtir_src[8];
static u32 s_rtpt_v0[4];

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

static u32 guest_of(const void *p)
{
    uintptr_t off = (uintptr_t)p - (uintptr_t)g_PsxRam;

    if (off >= 0x180000u && off < 0x180400u)
        return 0x1F800000u + (u32)(off - 0x180000u);
    if (off < 0x400u)
        return 0x1F800000u + (u32)off;
    return 0x80000000u | (u32)off;
}

s32 wm_80093978(s32 x, s32 z)
{
    u32 i = (u32)s_n93978;

    if (i < 4u) {
        s_93978_x[i] = x;
        s_93978_z[i] = z;
    }
    s_n93978++;
    return s_yret[i < 4u ? i : 3u];
}

s32 wm_80093740(u32 out_normal_addr, s32 x, s32 z)
{
    u32 i = (u32)s_n93740;

    (void)x;
    (void)z;
    if (i < 4u)
        s_93740_dst[i] = out_normal_addr;
    wr32(out_normal_addr, 0x70u);
    wr32(out_normal_addr + 4u, 0x80u);
    wr32(out_normal_addr + 8u, 0x90u);
    s_n93740++;
    return 0;
}

void OuterProduct12(VECTOR *v0, VECTOR *v1, VECTOR *v2)
{
    (void)v0;
    (void)v1;
    v2->vx = 0x11;
    v2->vy = 0x22;
    v2->vz = 0x33;
    s_nop++;
}

long VectorNormal(VECTOR *v0, VECTOR *v1)
{
    (void)v0;
    v1->vx = 0x44;
    v1->vy = 0x55;
    v1->vz = 0x66;
    s_nvn++;
    return 1;
}

MATRIX *RotMatrixY(int r, MATRIX *m)
{
    s_roty_ang = (u32)(u16)r;
    s_roty_m = guest_of(m);
    s_nroty++;
    return m;
}

MATRIX *MulMatrix(MATRIX *m0, MATRIX *m1)
{
    (void)m1;
    s_mul_m0 = guest_of(m0);
    s_nmul++;
    return m0;
}

MATRIX *ScaleMatrix(MATRIX *m, VECTOR *v)
{
    u32 i = (u32)s_nscale;

    (void)m;
    if (i < 4u)
        s_scale_x[i] = (u32)v->vx;
    s_nscale++;
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

void wm_747dc_test_store(u32 address, u32 value)
{
    (void)address;
    (void)value;
}

s32 wm_747dc_test_flag(void)
{
    return s_flag;
}

void wm_747dc_test_rtir(u32 src, u32 dst)
{
    u32 i = (u32)s_nrtir;

    if (i < 8u)
        s_rtir_src[i] = src;
    wr16(dst, rd16(src));
    wr16(dst + 6u, rd16(src + 6u));
    wr16(dst + 0xCu, rd16(src + 0xCu));
    s_nrtir++;
}

void wm_747dc_test_rt(u32 vec, u32 dst)
{
    (void)vec;
    wr32(dst, 0x111u);
    wr32(dst + 4u, 0x222u);
    wr32(dst + 8u, 0x333u);
    s_nrt++;
}

void wm_747dc_test_rtpt(u32 v0, u32 v1, u32 v2)
{
    u32 i = (u32)s_nrtpt;

    (void)v1;
    (void)v2;
    if (i < 4u)
        s_rtpt_v0[i] = v0;
    s_nrtpt++;
}

void wm_747dc_test_rtpt_store(u32 pkt)
{
    wr32(pkt + 8u, 0x100u + (u32)s_nrtpt - 1u);
    wr32(pkt + 0x10u, 0x200u + (u32)s_nrtpt - 1u);
    wr32(pkt + 0x18u, 0x300u + (u32)s_nrtpt - 1u);
    wr32(SC + 0x80u, s_sz0);
    wr32(SC + 0x84u, s_sz1);
    wr32(SC + 0x88u, s_sz2);
}

void wm_747dc_test_rtps(u32 vec, u32 pkt)
{
    (void)vec;
    wr32(pkt + 0x20u, 0x400u + (u32)s_nrtps);
    wr32(SC + 0x80u, s_sz_rtps);
    s_nrtps++;
}

static void reset(void)
{
    s_n93978 = s_n93740 = s_nop = s_nvn = 0;
    s_nroty = s_nmul = s_nscale = 0;
    s_nsetrot = s_nsettrans = 0;
    s_nrtir = s_nrt = s_nrtpt = s_nrtps = 0;
    s_flag = 0;
    s_sz0 = 0x900u;
    s_sz1 = 0xA00u;
    s_sz2 = 0xB00u;
    s_sz_rtps = 0x880u;
    s_yret[0] = 0x5000;
    s_yret[1] = 0x6000;
    s_yret[2] = 0;
    s_yret[3] = 0;
    memset(s_93978_x, 0, sizeof(s_93978_x));
    memset(s_93978_z, 0, sizeof(s_93978_z));
    memset(s_93740_dst, 0, sizeof(s_93740_dst));
    memset(s_scale_x, 0, sizeof(s_scale_x));
    memset(s_rtir_src, 0, sizeof(s_rtir_src));
    memset(s_rtpt_v0, 0, sizeof(s_rtpt_v0));
    s_roty_ang = s_roty_m = s_mul_m0 = 0;
}

static void seed_common(void)
{
    u32 i;

    PsxMemory_Init();
    reset();
    wr32(RECS_P, RECS);
    wr32(INDEX, 0u);
    wr32(DESTS, PKT);
    wr32(POS_X, 0u);
    wr32(POS_Z, 0u);
    wr16(ANGLE, 0x1234u);
    wr32(DB_PTR, DB);
    wr32(DB + 0x70u, OT);
    wr32(OT + 0x220u, 0xA5000001u);
    wr32(CANARY, 0x11111111u);
    for (i = 0u; i < 8u; i++) {
        wr32(CAM + i * 4u, 0xC8000000u + i);
        wr32(MODE2 + i * 4u, 0xA1800000u + i);
    }
    wr32(PKT, 0x06000000u);
    wr32(PKT + 0x28u, 0x06000000u);
}

static u32 lo(u32 a)
{
    return a & 0x00FFFFFFu;
}

int main(void)
{
    seed_common();
    wr32(COUNT, 0u);
    wm_800747DC();
    chk("z.count", rd32(COUNT), 0u);
    chk("z.n93978", (u32)s_n93978, 0u);
    chk("z.nrtpt", (u32)s_nrtpt, 0u);
    chk("z.sc.a0", (u32)rd16(SC + 0xA0u), 0u);
    chk("z.canary", rd32(CANARY), 0x11111111u);

    seed_common();
    wr32(COUNT, 2u);
    wr16(RECS, 1u);
    wr16(RECS + 2u, 1u);
    wr16(RECS + 4u, 2u);
    wr16(RECS + 8u, 3u);
    wr16(RECS + 0xAu, 2u);
    wr16(RECS + 0xCu, 4u);
    wm_800747DC();

    chk("n93978", (u32)s_n93978, 2u);
    chk("n93740", (u32)s_n93740, 2u);
    chk("nop", (u32)s_nop, 4u);
    chk("nvn", (u32)s_nvn, 4u);
    chk("nroty", (u32)s_nroty, 1u);
    chk("nmul", (u32)s_nmul, 1u);
    chk("nscale", (u32)s_nscale, 2u);
    chk("nsetrot", (u32)s_nsetrot, 4u);
    chk("nsettrans", (u32)s_nsettrans, 4u);
    chk("nrtir", (u32)s_nrtir, 6u);
    chk("nrt", (u32)s_nrt, 2u);
    chk("nrtpt", (u32)s_nrtpt, 2u);
    chk("nrtps", (u32)s_nrtps, 2u);
    chk("x0", (u32)s_93978_x[0], 0x1000u);
    chk("z0", (u32)s_93978_z[0], 0x2000u);
    chk("x1", (u32)s_93978_x[1], 0x3000u);
    chk("z1", (u32)s_93978_z[1], 0x4000u);
    chk("n40.0", s_93740_dst[0], SC + 0x30u);
    chk("scale0", s_scale_x[0], 0x1800u);
    chk("scale1", s_scale_x[1], 0x1800u);
    chk("roty.ang", s_roty_ang, 0x1234u);
    chk("roty.m", s_roty_m, SC + 0x150u);
    chk("mul.m0", s_mul_m0, SC + 0xF0u);
    chk("rtir0", s_rtir_src[0], SC + 0xF0u);
    chk("rtpt0", s_rtpt_v0[0], SC + 0xA0u);
    chk("sc.a0", (u32)(u16)rd16(SC + 0xA0u), 0xFFF0u);
    chk("sc.a4", (u32)rd16(SC + 0xA4u), 0x10u);
    chk("sc.d8", (u32)rd16(SC + 0xD8u), 0x10u);
    chk("sc.48", rd32(SC + 0x48u), 0x1000u);
    chk("cam0", rd32(SC + 0x130u), 0xC8000000u);
    chk("pack.f0", (u32)rd16(SC + 0xF0u), 0x44u);
    chk("pack.f6", (u32)rd16(SC + 0xF6u), 0x70u);
    chk("pack.fc", (u32)rd16(SC + 0xFCu), 0x44u);
    chk("rel.x", rd32(SC + 0x104u), 3u);
    chk("rel.y", rd32(SC + 0x108u), 6u);
    chk("rel.z", rd32(SC + 0x10Cu), 0xFFFFFFFCu);
    chk("sxy0", rd32(PKT + 8u), 0x100u);
    chk("sxy1", rd32(PKT + 0x10u), 0x200u);
    chk("sxy2", rd32(PKT + 0x18u), 0x300u);
    chk("sxy3", rd32(PKT + 0x20u), 0x400u);
    chk("sxy0b", rd32(PKT + 0x28u + 8u), 0x101u);
    chk("sxy3b", rd32(PKT + 0x28u + 0x20u), 0x401u);
    chk("pkt0.tag", rd32(PKT), 0x06000001u);
    chk("pkt1.tag", rd32(PKT + 0x28u), 0x06000000u | lo(PKT));
    chk("ot", rd32(OT + 0x220u), 0xA5000000u | lo(PKT + 0x28u));
    chk("count", rd32(COUNT), 0u);
    chk("canary", rd32(CANARY), 0x11111111u);

    seed_common();
    wr32(COUNT, 1u);
    wr16(RECS, 5u);
    wr16(RECS + 2u, 0u);
    wr16(RECS + 4u, 6u);
    s_flag = -1;
    wm_800747DC();
    chk("clip.n93978", (u32)s_n93978, 1u);
    chk("clip.nrtpt", (u32)s_nrtpt, 1u);
    chk("clip.nrtps", (u32)s_nrtps, 0u);
    chk("clip.sxy0", rd32(PKT + 8u), 0u);
    chk("clip.tag", rd32(PKT), 0x06000000u);
    chk("clip.ot", rd32(OT + 0x220u), 0xA5000001u);
    chk("clip.count", rd32(COUNT), 0u);
    chk("clip.nscale", (u32)s_nscale, 0u);

    seed_common();
    wr32(COUNT, 1u);
    wr16(RECS, 1u);
    wr16(RECS + 2u, 1u);
    wr16(RECS + 4u, 2u);
    s_sz0 = s_sz1 = s_sz2 = s_sz_rtps = 0x2000u;
    wm_800747DC();
    chk("far.nrtps", (u32)s_nrtps, 1u);
    chk("far.tag", rd32(PKT), 0x06000000u);
    chk("far.ot", rd32(OT + 0x220u), 0xA5000001u);
    chk("far.nscale", (u32)s_nscale, 1u);
    chk("far.count", rd32(COUNT), 0u);

    if (s_failures) {
        fprintf(stderr, "ASSERTION failures=%d\n", s_failures);
        return 1;
    }
    printf("W34B24-I44 0x800747DC focused oracle PASS\n");
    return 0;
}
