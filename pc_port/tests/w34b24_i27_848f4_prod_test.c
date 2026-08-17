/*
 * Focused production-linked oracle for retail helper 0x800848F4.
 *
 * Expected values are hand-derived from [0x800848F4, 0x80084D00).
 * PsyQ/SLUS/GTE residents are recording stubs. Real wm_80093534 is
 * linked. NEXT_CALLBACK_TARGET is not an oracle input.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_848f4.h"

#define POS_X   0x8009BE28u
#define POS_Z   0x8009BE30u
#define COUNT   0x8009D7E0u
#define ROOTP   0x8009C620u
#define INDEX   0x8009D7F0u
#define DB_PTR  0x8009BE3Cu
#define DB      0x8009BBC8u
#define OT      0x800F0000u
#define CAMERA  0x8009C808u
#define TABLE   0x8009AD2Cu
#define D104    0x80050104u
#define C0      0x800595C0u
#define C78     0x80059578u
#define WRAP_X  0x8009D160u
#define WRAP_Z  0x8009D2B4u
#define ROOT    0x80090000u
#define PACKET  0x80094000u
#define WORK    0x80094100u
#define SC      0x1F800000u
#define SC_SCALE 0x1F800010u
#define SC_FLAG  0x1F800020u
#define SC_SZ    0x1F800028u
#define SC_SVEC  0x1F8000A0u
#define SC_MTX   0x1F8000F0u
#define SC_COMP  0x1F800110u

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

static int s_nscale;
static int s_ncomp;
static int s_nsetrot;
static int s_nsettrans;
static int s_napply;
static int s_nrot;
static int s_nrtps;
static int s_nsubmit;
static u32 s_scale_m;
static u32 s_scale_v;
static u32 s_comp_a;
static u32 s_comp_b;
static u32 s_comp_c;
static u32 s_setrot;
static u32 s_settrans;
static u32 s_submit_a0;
static u32 s_submit_a1;
static u32 s_submit_a2;
static u32 s_submit_a3;
static u32 s_rtps_flag;
static u32 s_rtps_sz;

MATRIX *ScaleMatrix(MATRIX *m, VECTOR *v)
{
    s_scale_m = guest_of(m);
    s_scale_v = guest_of(v);
    s_nscale++;
    return m;
}

MATRIX *CompMatrix(MATRIX *m0, MATRIX *m1, MATRIX *m2)
{
    s_comp_a = guest_of(m0);
    s_comp_b = guest_of(m1);
    s_comp_c = guest_of(m2);
    s_ncomp++;
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

SVECTOR *ApplyMatrixSV(MATRIX *m, SVECTOR *v0, SVECTOR *v1)
{
    (void)m;
    *v1 = *v0;
    s_napply++;
    return v1;
}

void RotTrans(SVECTOR *v0, VECTOR *v1, long *flag)
{
    v1->vx = v0->vx;
    v1->vy = v0->vy;
    v1->vz = v0->vz;
    *flag = 0;
    s_nrot++;
}

s32 func_8002C700(void *a0, void *a1, void *a2, s32 a3)
{
    s_submit_a0 = guest_of(a0);
    s_submit_a1 = guest_of(a1);
    s_submit_a2 = guest_of(a2);
    s_submit_a3 = (u32)a3;
    s_nsubmit++;
    return 1;
}

u32 wm_848f4_test_rtps(u32 svec_addr, u32 *flag_out)
{
    (void)svec_addr;
    s_nrtps++;
    *flag_out = s_rtps_flag;
    return s_rtps_sz;
}

void wm_848f4_test_store(u32 address, u32 value)
{
    (void)address;
    (void)value;
}

static void reset(void)
{
    s_nscale = s_ncomp = s_nsetrot = s_nsettrans = 0;
    s_napply = s_nrot = s_nrtps = s_nsubmit = 0;
    s_scale_m = s_scale_v = s_comp_a = s_comp_b = s_comp_c = 0;
    s_setrot = s_settrans = 0;
    s_submit_a0 = s_submit_a1 = s_submit_a2 = s_submit_a3 = 0;
}

static void seed(s32 nreg, u16 enable0, u32 flag, u32 sz)
{
    u32 i;

    PsxMemory_Init();
    reset();
    s_rtps_flag = flag;
    s_rtps_sz = sz;
    wr32(POS_X, 0x1000u);
    wr32(POS_Z, 0x2000u);
    wr16(COUNT, (u16)nreg);
    wr32(ROOTP, ROOT);
    wr32(INDEX, 0u);
    wr32(DB_PTR, DB);
    wr32(DB + 0x70u, OT);
    wr32(CAMERA, 0xCAFE0001u);
    wr32(WRAP_X, 1u);
    wr32(WRAP_Z, 1u);
    wr32(C0, 0xDEADu);
    wr32(C78, 0xBEEFu);
    wr16(TABLE, 4u);
    for (i = 0u; i < 8u; i++)
        wr32(ROOT + 0x20u + i * 4u, 0x11110000u + i);
    wr16(ROOT, enable0);
    wr16(ROOT + 4u, 0u);
    wr32(ROOT + 8u, 0x10u);
    wr32(ROOT + 0xCu, 0x20u);
    wr32(ROOT + 0x10u, 0x30u);
    wr32(ROOT + 0x40u, PACKET);
    wr32(ROOT + 0x48u, WORK);
    wr32(ROOT + 0x50u, 0u);
}

int main(void)
{
    seed(0, 0, 0, 0x800u);
    wm_800848F4();
    chk("zero.scale0", rd32(SC_SCALE), 0x800u);
    chk("zero.scale4", rd32(SC_SCALE + 4u), 0x800u);
    chk("zero.scale8", rd32(SC_SCALE + 8u), 0x800u);
    chk("zero.d104", rd32(D104), 3u);
    chk("zero.c0", rd32(C0), 0u);
    chk("zero.c78", rd32(C78), 0u);
    chk("zero.svec0", rd16(SC_SVEC), 0u);
    chk("zero.nsubmit", (u32)s_nsubmit, 0u);
    chk("zero.nscale", (u32)s_nscale, 0u);

    seed(1, 0, 0, 0x800u);
    wm_800848F4();
    chk("en.nscale", (u32)s_nscale, 1u);
    chk("en.ncomp", (u32)s_ncomp, 1u);
    chk("en.nsetrot", (u32)s_nsetrot, 1u);
    chk("en.nsettrans", (u32)s_nsettrans, 1u);
    chk("en.nrtps", (u32)s_nrtps, 1u);
    chk("en.nsubmit", (u32)s_nsubmit, 1u);
    chk("en.scale_m", s_scale_m, SC_MTX);
    chk("en.scale_v", s_scale_v, SC_SCALE);
    chk("en.comp_a", s_comp_a, CAMERA);
    chk("en.comp_b", s_comp_b, SC_MTX);
    chk("en.comp_c", s_comp_c, SC_COMP);
    chk("en.setrot", s_setrot, SC_COMP);
    chk("en.settrans", s_settrans, SC_COMP);
    /* t0=0x10, posX>>12=1 => 0x0F; t2=-0x30, -t2=0x30, posZ>>12=2 => 0x2E */
    chk("en.t0", rd32(SC_MTX + 0x14u), 0x0Fu);
    chk("en.t2", rd32(SC_MTX + 0x1Cu), (0u - 0x2Eu));
    chk("en.sz", rd32(SC_SZ), 0x800u);
    chk("en.sub.a0", s_submit_a0, PACKET);
    chk("en.sub.a1", s_submit_a1, WORK);
    chk("en.sub.a2", s_submit_a2, OT);
    chk("en.sub.a3", s_submit_a3, 4u);

    seed(1, 1, 0, 0x800u);
    wm_800848F4();
    chk("dis.nsubmit", (u32)s_nsubmit, 0u);
    chk("dis.nscale", (u32)s_nscale, 0u);

    seed(1, 0, 0x80000000u, 0x800u);
    wm_800848F4();
    chk("clip.nsubmit", (u32)s_nsubmit, 0u);
    chk("clip.nrtps", (u32)s_nrtps, 1u);

    seed(1, 0, 0, 0xD80u);
    wm_800848F4();
    chk("far.nsubmit", (u32)s_nsubmit, 0u);
    chk("far.sz", rd32(SC_SZ), 0xD80u);

    if (s_failures) {
        fprintf(stderr, "ASSERTION failures=%d\n", s_failures);
        return 1;
    }
    printf("W34B24-I27 0x800848F4 focused oracle PASS\n");
    return 0;
}
