/*
 * Focused production-linked oracle for retail helper 0x80097244.
 *
 * Expected values are hand-derived from [0x80097244, 0x80097440).
 * PsyQ residents are recording stubs. NEXT_CALLBACK_TARGET is not
 * an oracle input.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_97244.h"

#define LOOK    0x8009BD40u
#define DST     0x8009C808u
#define SC_V0   0x1F800000u
#define SC_N2   0x1F800010u
#define SC_N3   0x1F800020u
#define SC_N1   0x1F800030u
#define SC_SV   0x1F800040u
#define SC_MAT  0x1F800048u

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
static int s_nvn;
static int s_nop;
static int s_napply;
static int s_ntrans;
static u32 s_vn_in[3];
static u32 s_vn_out[3];
static u32 s_op_a[2];
static u32 s_op_b[2];
static u32 s_op_c[2];
static SVECTOR s_apply_r;
static u32 s_apply_m;
static u32 s_apply_in0;
static u32 s_trans_m;
static u32 s_delta0;

void wm_97244_test_store(u32 address, u32 value)
{
    (void)address;
    (void)value;
    s_stores++;
}

static u32 guest_of(const void *p)
{
    uintptr_t off = (uintptr_t)p - (uintptr_t)g_PsxRam;

    if (off < 0x400u)
        return 0x1F800000u + (u32)off;
    return 0x80000000u | (u32)off;
}

VECTOR *VectorNormal(VECTOR *v0, VECTOR *v1)
{
    int n = s_nvn;
    u32 tag = 0x1001u + (u32)n * 0x1000u;

    if (n < 3) {
        s_vn_in[n] = guest_of(v0);
        s_vn_out[n] = guest_of(v1);
    }
    if (n == 0)
        s_delta0 = (u32)v0->vx;
    v1->vx = (s32)tag;
    v1->vy = (s32)(tag + 1u);
    v1->vz = (s32)(tag + 2u);
    s_nvn++;
    return v1;
}

void OuterProduct12(VECTOR *v0, VECTOR *v1, VECTOR *v2)
{
    int n = s_nop;

    if (n < 2) {
        s_op_a[n] = guest_of(v0);
        s_op_b[n] = guest_of(v1);
        s_op_c[n] = guest_of(v2);
    }
    v2->vx = 0x10 + n;
    v2->vy = 0x20 + n;
    v2->vz = 0x30 + n;
    s_nop++;
}

VECTOR *ApplyMatrix(MATRIX *m, SVECTOR *v0, VECTOR *v1)
{
    s_apply_m = guest_of(m);
    s_apply_in0 = (u32)(u16)m->m[0][0];
    s_apply_r = *v0;
    v1->vx = 7;
    v1->vy = 8;
    v1->vz = 9;
    s_napply++;
    return v1;
}

MATRIX *TransMatrix(MATRIX *m, VECTOR *v)
{
    s_trans_m = guest_of(m);
    m->t[0] = v->vx;
    m->t[1] = v->vy;
    m->t[2] = v->vz;
    s_ntrans++;
    return m;
}

static void reset_calls(void)
{
    s_stores = 0;
    s_nvn = s_nop = s_napply = s_ntrans = 0;
    memset(s_vn_in, 0, sizeof(s_vn_in));
    memset(s_vn_out, 0, sizeof(s_vn_out));
    memset(s_op_a, 0, sizeof(s_op_a));
    memset(s_op_b, 0, sizeof(s_op_b));
    memset(s_op_c, 0, sizeof(s_op_c));
    memset(&s_apply_r, 0, sizeof(s_apply_r));
    s_apply_m = s_apply_in0 = s_trans_m = 0;
    s_delta0 = 0;
}

static void run_pos(void)
{
    reset_calls();
    wr16(LOOK, 1);
    wr16(LOOK + 2u, 2);
    wr16(LOOK + 4u, 3);
    wr16(LOOK + 6u, 0xDEADu);
    wr16(LOOK + 8u, 11);
    wr16(LOOK + 0xAu, 22);
    wr16(LOOK + 0xCu, 33);
    wr32(LOOK + 0x10u, 0x11111111u);
    wr32(DST + 20u, 0xCAFECAFEu);

    wm_80097244(LOOK);

    chk("pos.nvn", (u32)s_nvn, 3u);
    chk("pos.nop", (u32)s_nop, 2u);
    chk("pos.napply", (u32)s_napply, 1u);
    chk("pos.ntrans", (u32)s_ntrans, 1u);
    chk("pos.delta0", s_delta0, 10u); /* 11-1 */
    chk("pos.vn0_in", s_vn_in[0], SC_V0);
    chk("pos.vn0_out", s_vn_out[0], SC_N1);
    chk("pos.vn1_out", s_vn_out[1], SC_N2);
    chk("pos.vn2_out", s_vn_out[2], SC_N3);
    chk("pos.op0_a", s_op_a[0], SC_N1);
    chk("pos.op0_b", s_op_b[0], LOOK + 0x10u);
    chk("pos.op0_c", s_op_c[0], SC_V0);
    chk("pos.op1_a", s_op_a[1], SC_N1);
    chk("pos.op1_b", s_op_b[1], SC_N2);
    chk("pos.m00", (u32)rd16(DST), 0x2001u);
    chk("pos.m01", (u32)rd16(DST + 2u), 0x2002u);
    chk("pos.m02", (u32)rd16(DST + 4u), 0x2003u);
    chk("pos.m10", (u32)rd16(DST + 6u), 0x3001u);
    chk("pos.m11", (u32)rd16(DST + 8u), 0x3002u);
    chk("pos.m12", (u32)rd16(DST + 0xAu), 0x3003u);
    chk("pos.m20", (u32)rd16(DST + 0xCu), 0x1001u);
    chk("pos.m21", (u32)rd16(DST + 0xEu), 0x1002u);
    chk("pos.m22", (u32)rd16(DST + 0x10u), 0x1003u);
    chk("pos.apply.vx", (u32)(u16)s_apply_r.vx, 0xFFFFu);
    chk("pos.apply.vy", (u32)(u16)s_apply_r.vy, 0xFFFEu);
    chk("pos.apply.vz", (u32)(u16)s_apply_r.vz, 0xFFFDu);
    chk("pos.apply_m", s_apply_m, SC_MAT);
    chk("pos.apply_in0", s_apply_in0, 0x2001u);
    chk("pos.trans_m", s_trans_m, DST);
    chk("pos.t0", rd32(DST + 20u), 7u);
    chk("pos.t1", rd32(DST + 24u), 8u);
    chk("pos.t2", rd32(DST + 28u), 9u);
    chk("pos.eye6", (u32)rd16(LOOK + 6u), 0xDEADu);
    chk("pos.sc_svx", (u32)rd16(SC_SV), 0xFFFFu);
    chk("pos.stores", (u32)(s_stores != 0), 1u);
}

int main(void)
{
    PsxMemory_Init();
    run_pos();

    if (s_failures != 0) {
        fprintf(stderr, "%d assertion(s) failed\n", s_failures);
        return 1;
    }
    printf("W34B24-I22 0x80097244 focused oracle PASS\n");
    return 0;
}
