/*
 * Focused production-linked oracle for retail helper 0x80097440.
 *
 * Expected values are hand-derived from [0x80097440, 0x8009766C).
 * PsyQ residents are recording stubs; dest/scratch stores are the
 * oracle. NEXT_CALLBACK_TARGET is not an oracle input.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_97440.h"

#define EYE     0x8009BD40u
#define SRC     0x8009A180u
#define DST     0x8009C808u
#define ANG     0x8009BD38u
#define SC_MX   0x1F8000F0u
#define SC_MY   0x1F800110u
#define SC_MZ   0x1F800130u
#define SC_MT   0x1F800150u
#define SC_SV   0x1F8000A0u
#define SC_VO   0x1F800000u

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
    s32 vx;
    s32 vy;
    s32 vz;
    s32 pad;
} VECTOR;

typedef struct {
    s16 m[3][3];
    s32 t[3];
} MATRIX;

static u32 s_stores;
static int s_nrotx;
static int s_nroty;
static int s_nrotz;
static int s_nmul;
static int s_napply;
static int s_ntrans;
static s32 s_rotx_r;
static s32 s_roty_r;
static s32 s_rotz_r;
static u32 s_rotx_m;
static u32 s_roty_m;
static u32 s_rotz_m;
static u32 s_mul0_a;
static u32 s_mul0_b;
static u32 s_mul0_c;
static u32 s_mul1_a;
static u32 s_mul1_b;
static u32 s_mul1_c;
static SVECTOR s_apply_r;
static u32 s_apply_m;
static u32 s_apply_out;
static u32 s_trans_m;
static u32 s_trans_v;
static u32 s_rotx_in0;
static u32 s_roty_in0;
static u32 s_rotz_in0;
static u32 s_rotx_in_t2;
static u32 s_apply_in0;

void wm_97440_test_store(u32 address, u32 value)
{
    (void)address;
    (void)value;
    s_stores++;
}

static u32 guest_of(const void *p)
{
    uintptr_t off = (uintptr_t)p - (uintptr_t)g_PsxRam;

    /* 0x1F80xxxx aliases to the low 2 MB window under PSX_ADDR's 0x1FFFFF mask. */
    if (off < 0x400u)
        return 0x1F800000u + (u32)off;
    return 0x80000000u | (u32)off;
}

static void paint_matrix(MATRIX *m, u16 tag)
{
    int r;
    int c;

    for (r = 0; r < 3; r++) {
        for (c = 0; c < 3; c++)
            m->m[r][c] = (s16)(tag + (u16)(r * 3 + c));
    }
    m->t[0] = (s32)tag + 0x100;
    m->t[1] = (s32)tag + 0x200;
    m->t[2] = (s32)tag + 0x300;
}

MATRIX *RotMatrixX(s32 r, MATRIX *m)
{
    s_rotx_r = r;
    s_rotx_m = guest_of(m);
    s_rotx_in0 = (u32)(u16)m->m[0][0];
    s_rotx_in_t2 = (u32)m->t[2];
    paint_matrix(m, 0x1110u);
    s_nrotx++;
    return m;
}

MATRIX *RotMatrixY(s32 r, MATRIX *m)
{
    s_roty_r = r;
    s_roty_m = guest_of(m);
    s_roty_in0 = (u32)(u16)m->m[0][0];
    paint_matrix(m, 0x2220u);
    s_nroty++;
    return m;
}

MATRIX *RotMatrixZ(s32 r, MATRIX *m)
{
    s_rotz_r = r;
    s_rotz_m = guest_of(m);
    s_rotz_in0 = (u32)(u16)m->m[0][0];
    paint_matrix(m, 0x3330u);
    s_nrotz++;
    return m;
}

MATRIX *MulMatrix0(MATRIX *m0, MATRIX *m1, MATRIX *m2)
{
    u32 a = guest_of(m0);
    u32 b = guest_of(m1);
    u32 c = guest_of(m2);

    if (s_nmul == 0) {
        s_mul0_a = a;
        s_mul0_b = b;
        s_mul0_c = c;
        paint_matrix(m2, 0x4440u);
    } else {
        s_mul1_a = a;
        s_mul1_b = b;
        s_mul1_c = c;
        paint_matrix(m2, 0x5550u);
    }
    s_nmul++;
    return m2;
}

VECTOR *ApplyMatrix(MATRIX *m, SVECTOR *v0, VECTOR *v1)
{
    s_apply_m = guest_of(m);
    s_apply_in0 = (u32)(u16)m->m[0][0];
    s_apply_r = *v0;
    s_apply_out = guest_of(v1);
    v1->vx = 10;
    v1->vy = 20;
    v1->vz = 30;
    s_napply++;
    return v1;
}

MATRIX *TransMatrix(MATRIX *m, VECTOR *v)
{
    s_trans_m = guest_of(m);
    s_trans_v = guest_of(v);
    m->t[0] = v->vx;
    m->t[1] = v->vy;
    m->t[2] = v->vz;
    s_ntrans++;
    return m;
}

static void reset_calls(void)
{
    s_stores = 0;
    s_nrotx = s_nroty = s_nrotz = 0;
    s_nmul = s_napply = s_ntrans = 0;
    s_rotx_r = s_roty_r = s_rotz_r = 0;
    s_rotx_m = s_roty_m = s_rotz_m = 0;
    s_mul0_a = s_mul0_b = s_mul0_c = 0;
    s_mul1_a = s_mul1_b = s_mul1_c = 0;
    memset(&s_apply_r, 0, sizeof(s_apply_r));
    s_apply_m = s_apply_out = 0;
    s_trans_m = s_trans_v = 0;
    s_rotx_in0 = s_roty_in0 = s_rotz_in0 = 0;
    s_rotx_in_t2 = 0;
    s_apply_in0 = 0;
}

static void seed_src(void)
{
    u32 i;

    for (i = 0; i < 8u; i++)
        wr32(SRC + i * 4u, 0xBEEFA100u + i);
}

static void run_pos(void)
{
    reset_calls();
    seed_src();
    wr16(ANG, 0x0100u);
    wr16(ANG + 2u, 0x0200u);
    wr16(ANG + 4u, 0x0300u);
    wr16(EYE, 0x0011u);
    wr16(EYE + 2u, 0x0022u);
    wr16(EYE + 4u, 0x0033u);
    wr16(EYE + 6u, 0xDEADu);
    wr32(DST + 0x1Cu, 0xCAFECAFEu);

    wm_80097440(EYE);

    chk("pos.nrotx", (u32)s_nrotx, 1u);
    chk("pos.nroty", (u32)s_nroty, 1u);
    chk("pos.nrotz", (u32)s_nrotz, 1u);
    chk("pos.nmul", (u32)s_nmul, 2u);
    chk("pos.napply", (u32)s_napply, 1u);
    chk("pos.ntrans", (u32)s_ntrans, 1u);
    chk("pos.rotx_r", (u32)s_rotx_r, (u32)-0x100);
    chk("pos.roty_r", (u32)s_roty_r, (u32)-0x200);
    chk("pos.rotz_r", (u32)s_rotz_r, (u32)-0x300);
    chk("pos.rotx_m", s_rotx_m, SC_MX);
    chk("pos.roty_m", s_roty_m, SC_MY);
    chk("pos.rotz_m", s_rotz_m, SC_MZ);
    chk("pos.rotx_in0", s_rotx_in0, 0xA100u);
    chk("pos.roty_in0", s_roty_in0, 0xA100u);
    chk("pos.rotz_in0", s_rotz_in0, 0xA100u);
    chk("pos.rotx_in_t2", s_rotx_in_t2, 0xBEEFA107u);
    chk("pos.apply_in0", s_apply_in0, 0x5550u);
    chk("pos.mul0_a", s_mul0_a, SC_MX);
    chk("pos.mul0_b", s_mul0_b, SC_MY);
    chk("pos.mul0_c", s_mul0_c, SC_MT);
    chk("pos.mul1_a", s_mul1_a, SC_MZ);
    chk("pos.mul1_b", s_mul1_b, SC_MT);
    chk("pos.mul1_c", s_mul1_c, DST);
    chk("pos.apply_m", s_apply_m, SC_MX);
    chk("pos.apply_out", s_apply_out, SC_VO);
    chk("pos.apply.vx", (u32)(u16)s_apply_r.vx, 0xFFEFu);
    chk("pos.apply.vy", (u32)(u16)s_apply_r.vy, 0xFFDEu);
    chk("pos.apply.vz", (u32)(u16)s_apply_r.vz, 0xFFCDu);
    chk("pos.trans_m", s_trans_m, DST);
    chk("pos.trans_v", s_trans_v, SC_VO);
    chk("pos.dst_t0", rd32(DST + 20u), 10u);
    chk("pos.dst_t1", rd32(DST + 24u), 20u);
    chk("pos.dst_t2", rd32(DST + 28u), 30u);
    chk("pos.eye6", (u32)rd16(EYE + 6u), 0xDEADu);
    chk("pos.sc_svx", (u32)rd16(SC_SV), 0xFFEFu);
    chk("pos.src0", rd32(SRC), 0xBEEFA100u);
    chk("pos.stores", (u32)(s_stores != 0), 1u);
}

static void run_neg(void)
{
    reset_calls();
    seed_src();
    wr16(ANG, 0x8000u);
    wr16(ANG + 2u, 0x0001u);
    wr16(ANG + 4u, 0xFFFFu);
    wr16(EYE, 0x8000u);
    wr16(EYE + 2u, 0x0001u);
    wr16(EYE + 4u, 0x0000u);

    wm_80097440(EYE);

    /* lh 0x8000 = -32768; negu → +32768 */
    chk("neg.rotx_r", (u32)s_rotx_r, 0x8000u);
    chk("neg.roty_r", (u32)s_roty_r, (u32)-1);
    chk("neg.rotz_r", (u32)s_rotz_r, 1u);
    /* lhu 0x8000; negu; sh → 0x8000 */
    chk("neg.apply.vx", (u32)(u16)s_apply_r.vx, 0x8000u);
    chk("neg.apply.vy", (u32)(u16)s_apply_r.vy, 0xFFFFu);
    chk("neg.apply.vz", (u32)(u16)s_apply_r.vz, 0u);
    chk("neg.nmul", (u32)s_nmul, 2u);
}

int main(void)
{
    PsxMemory_Init();
    run_pos();
    run_neg();

    if (s_failures != 0) {
        fprintf(stderr, "%d assertion(s) failed\n", s_failures);
        return 1;
    }
    printf("W34B24-I21 0x80097440 focused oracle PASS\n");
    return 0;
}
