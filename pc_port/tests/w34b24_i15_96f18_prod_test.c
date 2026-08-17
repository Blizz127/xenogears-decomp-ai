/*
 * Focused production-linked oracle for retail helper 0x80096F18.
 *
 * Expected values are hand-derived from [0x80096F18, 0x80097070).
 * PsyQ residents are recording stubs; dest/scratch stores are the
 * oracle. NEXT_CALLBACK_TARGET is not an oracle input.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_96f18.h"

#define DEST    0x8009BD40u
#define POSE    0x8009BE28u
#define ANGLES  0x8009BD38u
#define SC_VIN  0x1F800000u
#define SC_SVEC 0x1F8000A0u

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
static int s_nrot;
static int s_namlv;
static int s_napply;
static SVECTOR s_rot[2];
static VECTOR s_vin;
static SVECTOR s_apply_r;
static u32 s_apply_out_guest;

void wm_96f18_test_store(u32 address, u32 value)
{
    (void)address;
    (void)value;
    s_stores++;
}

MATRIX *RotMatrixYXZ(SVECTOR *r, MATRIX *m)
{
    if (s_nrot < 2)
        s_rot[s_nrot] = *r;
    s_nrot++;
    (void)m;
    return m;
}

VECTOR *ApplyMatrixLV(MATRIX *m, VECTOR *v0, VECTOR *v1)
{
    (void)m;
    s_vin = *v0;
    v1->vx = 0x1111;
    v1->vy = 0x2222;
    v1->vz = 0x3333;
    s_namlv++;
    return v1;
}

VECTOR *ApplyMatrix(MATRIX *m, SVECTOR *v0, VECTOR *v1)
{
    (void)m;
    s_apply_r = *v0;
    s_apply_out_guest = (u32)((uintptr_t)v1 - (uintptr_t)g_PsxRam) | 0x80000000u;
    /* Scratch 0x1F80xxxx masks to 0x0018xxxx; restore the retail window. */
    if (((uintptr_t)v1 - (uintptr_t)g_PsxRam) >= 0x180000u &&
        ((uintptr_t)v1 - (uintptr_t)g_PsxRam) < 0x180400u) {
        s_apply_out_guest =
            0x1F800000u +
            (u32)(((uintptr_t)v1 - (uintptr_t)g_PsxRam) - 0x180000u);
    }
    v1->vx = 0xAAA;
    v1->vy = 0xBBB;
    v1->vz = 0xCCC;
    s_napply++;
    return v1;
}

static void reset_calls(void)
{
    s_stores = 0;
    s_nrot = 0;
    s_namlv = 0;
    s_napply = 0;
    memset(s_rot, 0, sizeof(s_rot));
    memset(&s_vin, 0, sizeof(s_vin));
    memset(&s_apply_r, 0, sizeof(s_apply_r));
    s_apply_out_guest = 0;
}

static void fill_canaries(void)
{
    wr16(DEST + 6u, 0xDEADu);
    wr16(DEST + 0xEu, 0xBEEFu);
    wr32(DEST + 0x1Cu, 0xCAFECAFEu);
    wr32(POSE, 0x11111111u);
    wr32(POSE + 8u, 0x22222222u);
    wr16(ANGLES + 6u, 0x3333u);
}

static void run_pos(void)
{
    reset_calls();
    fill_canaries();
    wr16(ANGLES, 0x0100u);
    wr16(ANGLES + 2u, 0x0200u);
    wr16(ANGLES + 4u, 0x0300u);
    wr32(POSE + 4u, 0x01234000u);
    wr16(DEST, 0xFFFFu);
    wr16(DEST + 2u, 0xFFFFu);
    wr16(DEST + 4u, 0xFFFFu);
    wr16(DEST + 8u, 0xFFFFu);
    wr16(DEST + 0xAu, 0xFFFFu);
    wr16(DEST + 0xCu, 0xFFFFu);
    wr32(DEST + 0x10u, 0u);
    wr32(DEST + 0x14u, 0u);
    wr32(DEST + 0x18u, 0u);

    wm_80096F18(DEST, POSE, 0x5000, ANGLES);

    chk("pos.nrot", (u32)s_nrot, 2u);
    chk("pos.namlv", (u32)s_namlv, 1u);
    chk("pos.napply", (u32)s_napply, 1u);
    chk("pos.rot0.vx", (u32)(u16)s_rot[0].vx, 0x0100u);
    chk("pos.rot0.vy", (u32)(u16)s_rot[0].vy, 0x0200u);
    chk("pos.rot0.vz", (u32)(u16)s_rot[0].vz, 0u);
    chk("pos.vin.x", (u32)s_vin.vx, 0u);
    chk("pos.vin.y", (u32)s_vin.vy, 0u);
    chk("pos.vin.z", (u32)s_vin.vz, (u32)-5);
    chk("pos.rot1.vx", (u32)(u16)s_rot[1].vx, 0u);
    chk("pos.rot1.vy", (u32)(u16)s_rot[1].vy, 0x0200u);
    chk("pos.rot1.vz", (u32)(u16)s_rot[1].vz, 0x0300u);
    chk("pos.apply.vx", (u32)(u16)s_apply_r.vx, 0u);
    chk("pos.apply.vy", (u32)(u16)s_apply_r.vy, 0xF000u);
    chk("pos.apply.vz", (u32)(u16)s_apply_r.vz, 0u);
    chk("pos.dest0", (u32)rd16(DEST), 0x1111u);
    chk("pos.dest2", (u32)rd16(DEST + 2u), 0x3456u); /* 0x1234 + 0x2222 */
    chk("pos.dest4", (u32)rd16(DEST + 4u), 0x3333u);
    chk("pos.dest6", (u32)rd16(DEST + 6u), 0xDEADu);
    chk("pos.dest8", (u32)rd16(DEST + 8u), 0u);
    chk("pos.destA", (u32)rd16(DEST + 0xAu), 0x1234u);
    chk("pos.destC", (u32)rd16(DEST + 0xCu), 0u);
    chk("pos.destE", (u32)rd16(DEST + 0xEu), 0xBEEFu);
    chk("pos.outx", rd32(DEST + 0x10u), 0xAAAu);
    chk("pos.outy", rd32(DEST + 0x14u), 0xBBBu);
    chk("pos.outz", rd32(DEST + 0x18u), 0xCCCu);
    chk("pos.out_canary", rd32(DEST + 0x1Cu), 0xCAFECAFEu);
    chk("pos.pose0", rd32(POSE), 0x11111111u);
    chk("pos.pose8", rd32(POSE + 8u), 0x22222222u);
    chk("pos.ang6", (u32)rd16(ANGLES + 6u), 0x3333u);
    chk("pos.apply_dst", s_apply_out_guest, DEST + 0x10u);
    chk("pos.sc_vin_z", rd32(SC_VIN + 8u), (u32)-5);
    chk("pos.sc_last_vy", (u32)rd16(SC_SVEC + 2u), 0xF000u);
}

static void run_neg_lhu(void)
{
    reset_calls();
    fill_canaries();
    wr16(ANGLES, 0x0001u);
    wr16(ANGLES + 2u, 0x0002u);
    wr16(ANGLES + 4u, 0x0003u);
    /* >>12 = -1 → sh 0xFFFF; lhu + 1 = 0x10000 → sh 0 */
    wr32(POSE + 4u, 0xFFFFF000u);
    wr16(DEST, 0u);
    wr16(DEST + 2u, 0u);
    wr16(DEST + 4u, 0u);
    wr16(DEST + 8u, 0xFFFFu);
    wr16(DEST + 0xAu, 0xFFFFu);
    wr16(DEST + 0xCu, 0xFFFFu);
    wr32(DEST + 0x10u, 0u);
    wr32(DEST + 0x14u, 0u);
    wr32(DEST + 0x18u, 0u);

    wm_80096F18(DEST, POSE, -0x2000, ANGLES);

    chk("neg.nrot", (u32)s_nrot, 2u);
    chk("neg.vin.z", (u32)s_vin.vz, 2u); /* -((-0x2000)>>12) = -(-2) */
    chk("neg.destA", (u32)rd16(DEST + 0xAu), 0xFFFFu);
    /* lhu(0xFFFF)+0x2222 = 0x12221 → sh 0x2221 */
    chk("neg.dest2", (u32)rd16(DEST + 2u), 0x2221u);
    chk("neg.dest8", (u32)rd16(DEST + 8u), 0u);
    chk("neg.destC", (u32)rd16(DEST + 0xCu), 0u);
    chk("neg.dest6", (u32)rd16(DEST + 6u), 0xDEADu);
}

int main(void)
{
    PsxMemory_Init();
    run_pos();
    run_neg_lhu();

    if (s_failures != 0) {
        fprintf(stderr, "%d assertion(s) failed\n", s_failures);
        return 1;
    }
    printf("W34B24-I15 0x80096F18 focused oracle PASS\n");
    return 0;
}
