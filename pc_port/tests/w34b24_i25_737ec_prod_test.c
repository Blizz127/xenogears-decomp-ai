/*
 * Focused production-linked oracle for retail helper 0x800737EC.
 *
 * Expected values are hand-derived from [0x800737EC, 0x800739B8).
 * PsyQ residents are recording stubs. NEXT_CALLBACK_TARGET is not
 * an oracle input.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_737ec.h"

#define ANGLE   0x8009BD3Au
#define INDEX   0x8009D7F0u
#define VERTEX  0x8009A280u
#define PACKET  0x8009D194u
#define CAMERA  0x8009C808u
#define DB_PTR  0x8009BE3Cu
#define DB      0x8009BBC8u
#define OT      0x800F0000u
#define SHIFT   0x80050100u
#define SC_ANG  0x1F800000u
#define SC_COMP 0x1F800008u
#define SC_ROT  0x1F800028u
#define SC_P    0x1F800048u
#define SC_FLAG 0x1F80004Cu

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
static int s_ncomp;
static int s_nsetrot;
static int s_nsettrans;
static int s_nrtp;
static SVECTOR s_ang;
static u32 s_comp_a;
static u32 s_comp_b;
static u32 s_comp_c;
static u32 s_rot_t0;
static u32 s_setrot;
static u32 s_settrans;
static u32 s_rtp_v0[4];
static u32 s_rtp_sxy0[4];
static s32 s_rtp_otz[4];

static u32 guest_of(const void *p)
{
    uintptr_t off = (uintptr_t)p - (uintptr_t)g_PsxRam;

    if (off >= 0x180000u && off < 0x180400u)
        return 0x1F800000u + (u32)(off - 0x180000u);
    if (off < 0x400u)
        return 0x1F800000u + (u32)off;
    return 0x80000000u | (u32)off;
}

MATRIX *RotMatrixYXZ(SVECTOR *r, MATRIX *m)
{
    s_ang = *r;
    s_nrot++;
    memset(m, 0, sizeof(*m));
    m->m[0][0] = 0x1000;
    m->t[0] = 0x11111111;
    m->t[1] = 0x22222222;
    m->t[2] = 0x33333333;
    return m;
}

MATRIX *CompMatrix(MATRIX *m0, MATRIX *m1, MATRIX *m2)
{
    s_comp_a = guest_of(m0);
    s_comp_b = guest_of(m1);
    s_comp_c = guest_of(m2);
    s_rot_t0 = (u32)m1->t[0];
    s_ncomp++;
    memset(m2, 0, sizeof(*m2));
    m2->m[0][0] = 0x2000;
    return m2;
}

void SetRotMatrix(MATRIX *m)
{
    s_setrot = guest_of(m);
    s_nsetrot++;
}

void SetTransMatrix(MATRIX *m)
{
    s_settrans = guest_of(m);
    s_nsettrans++;
}

s32 RotTransPers4(SVECTOR *v0, SVECTOR *v1, SVECTOR *v2, SVECTOR *v3,
                  s32 *sxy0, s32 *sxy1, s32 *sxy2, s32 *sxy3, s32 *p,
                  s32 *flag)
{
    int n = s_nrtp;

    (void)v1;
    (void)v2;
    (void)v3;
    if (n < 4) {
        s_rtp_v0[n] = guest_of(v0);
        s_rtp_sxy0[n] = guest_of(sxy0);
        s_rtp_otz[n] = 0x20 + n * 4;
    }
    *sxy0 = 0x100 + n;
    *sxy1 = 0x200 + n;
    *sxy2 = 0x300 + n;
    *sxy3 = 0x400 + n;
    *p = 0x50 + n;
    /* Clip the last quad so SKIP_FLAG is observable. */
    if (n == 3)
        *flag = (s32)0x80000000;
    else
        *flag = 0;
    s_nrtp++;
    return 0x20 + n * 4;
}

static void reset(void)
{
    s_nrot = s_ncomp = s_nsetrot = s_nsettrans = s_nrtp = 0;
    memset(&s_ang, 0, sizeof(s_ang));
    s_comp_a = s_comp_b = s_comp_c = s_rot_t0 = 0;
    s_setrot = s_settrans = 0;
    memset(s_rtp_v0, 0, sizeof(s_rtp_v0));
    memset(s_rtp_sxy0, 0, sizeof(s_rtp_sxy0));
    memset(s_rtp_otz, 0, sizeof(s_rtp_otz));
}

static void seed(u16 theta, u32 index, u32 shift)
{
    u32 i;

    PsxMemory_Init();
    reset();
    wr16(ANGLE, theta);
    wr32(INDEX, index);
    wr32(SHIFT, shift);
    wr32(DB_PTR, DB);
    wr32(DB + 0x70u, OT);
    wr32(CAMERA, 0xCAFE0001u);
    wr32(SC_ROT + 0x14u, 0xDEAD0001u);
    wr32(SC_ROT + 0x18u, 0xDEAD0002u);
    wr32(SC_ROT + 0x1Cu, 0xDEAD0003u);
    for (i = 0; i < 4u; i++) {
        u32 pkt = PACKET + index * 36u + i * 0x48u;

        wr32(pkt, 0x09000000u | (0x10u + i));
        wr32(pkt + 8u, 0xA1A1A1A1u);
        wr32(VERTEX + i * 0x20u, 0x00010000u + i);
    }
    for (i = 0; i < 0x40u; i++)
        wr32(OT + i * 4u, 0xA5000000u | (0x2000u + i));
}

int main(void)
{
    u32 i;

    seed(0x02C0u, 1u, 1u);
    wm_800737EC();

    chk("nrot", (u32)s_nrot, 1u);
    chk("ncomp", (u32)s_ncomp, 1u);
    chk("nsetrot", (u32)s_nsetrot, 1u);
    chk("nsettrans", (u32)s_nsettrans, 1u);
    chk("nrtp", (u32)s_nrtp, 4u);
    chk("ang.vx", (u32)(u16)s_ang.vx, 0u);
    chk("ang.vy", (u32)(u16)s_ang.vy, 0x02C0u);
    chk("ang.vz", (u32)(u16)s_ang.vz, 0u);
    chk("comp.a", s_comp_a, CAMERA);
    chk("comp.b", s_comp_b, SC_ROT);
    chk("comp.c", s_comp_c, SC_COMP);
    chk("rot.t0", s_rot_t0, 0u);
    chk("setrot", s_setrot, SC_COMP);
    chk("settrans", s_settrans, SC_COMP);
    chk("sc.ang0", rd16(SC_ANG), 0u);
    chk("sc.ang2", rd16(SC_ANG + 2u), 0x02C0u);

    for (i = 0; i < 4u; i++) {
        char n[32];
        u32 pkt = PACKET + 36u + i * 0x48u;

        snprintf(n, sizeof n, "v0.%u", i);
        chk(n, s_rtp_v0[i], VERTEX + i * 0x20u);
        snprintf(n, sizeof n, "sxy0.%u", i);
        chk(n, s_rtp_sxy0[i], pkt + 8u);
        snprintf(n, sizeof n, "xy0.%u", i);
        chk(n, rd32(pkt + 8u), 0x100u + i);
    }

    /* otz 0x20,0x24,0x28 >> 1 = 0x10,0x12,0x14. Quad 3 is clipped. */
    chk("ot16", rd32(OT + 0x10u * 4u),
        0xA5000000u | ((PACKET + 36u) & 0x00FFFFFFu));
    chk("ot18", rd32(OT + 0x12u * 4u),
        0xA5000000u | ((PACKET + 36u + 0x48u) & 0x00FFFFFFu));
    chk("ot20", rd32(OT + 0x14u * 4u),
        0xA5000000u | ((PACKET + 36u + 0x90u) & 0x00FFFFFFu));
    chk("ot8", rd32(OT + 8u * 4u), 0xA5000000u | 0x2008u);
    chk("tag0", rd32(PACKET + 36u),
        0x09000000u | ((0xA5000000u | 0x2010u) & 0x00FFFFFFu));
    chk("tag3", rd32(PACKET + 36u + 0xD8u), 0x09000013u);
    chk("clip.xy3", rd32(PACKET + 36u + 0xD8u + 8u), 0x103u);

    if (s_failures) {
        fprintf(stderr, "ASSERTION failures=%d\n", s_failures);
        return 1;
    }
    printf("W34B24-I25 0x800737EC focused oracle PASS\n");
    return 0;
}
