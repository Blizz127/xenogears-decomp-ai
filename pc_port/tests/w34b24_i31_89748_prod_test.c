/*
 * Focused production-linked oracle for retail helper 0x80089748.
 *
 * Expected values are hand-derived from [0x80089748, 0x80089C78).
 * PsyQ residents are recording stubs. wm_80089580 is a call counter.
 * NEXT_CALLBACK_TARGET is not an oracle input.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_89748.h"

#define TAB_PTR  0x8009BCC0u
#define REC_PTR  0x8009BDF4u
#define TAB      0x800A0000u
#define RECS     0x800AB000u
#define CANARY   0x800B0000u
#define EMIT_STR 0x54u
#define SLOT_STR 0x4Cu

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

static int s_nrot;
static int s_napply;
static int s_nnorm;
static int s_nratan;
static int s_nrand;
static int s_n89580;
static u32 s_rot_r;
static u32 s_rot_m;
static u32 s_apply0_v;
static u32 s_apply1_v;
static s32 s_ratan_y;
static s32 s_ratan_x;
static SVECTOR s_rot_in;

void wm_89748_test_store(u32 address, u32 value)
{
    (void)address;
    (void)value;
}

void wm_80089580(void)
{
    s_n89580++;
}

int rand(void)
{
    s_nrand++;
    return 0x800;
}

MATRIX *RotMatrixYXZ(SVECTOR *r, MATRIX *m)
{
    s_rot_r = guest_of(r);
    s_rot_m = guest_of(m);
    s_rot_in = *r;
    s_nrot++;
    return m;
}

VECTOR *ApplyMatrix(MATRIX *m, SVECTOR *v0, VECTOR *v1)
{
    (void)m;
    if (s_napply == 0) {
        s_apply0_v = guest_of(v0);
        v1->vx = 2;
        v1->vy = 3;
        v1->vz = 4;
    } else {
        s_apply1_v = guest_of(v0);
        v1->vx = 6;
        v1->vy = 7;
        v1->vz = 8;
    }
    s_napply++;
    return v1;
}

VECTOR *VectorNormal(VECTOR *v0, VECTOR *v1)
{
    v1->vx = v0->vx + 100;
    v1->vy = v0->vy + 200;
    v1->vz = v0->vz + 300;
    s_nnorm++;
    return v1;
}

long ratan2(long y, long x)
{
    s_ratan_y = (s32)y;
    s_ratan_x = (s32)x;
    s_nratan++;
    return 0x1234;
}

static u32 emit(u32 i)
{
    return TAB + i * EMIT_STR;
}

static u32 s1_of(u32 i)
{
    return emit(i) + 4u;
}

static u32 rec(u32 i)
{
    return RECS + i * SLOT_STR;
}

static void reset_calls(void)
{
    s_nrot = s_napply = s_nnorm = s_nratan = s_nrand = s_n89580 = 0;
    s_rot_r = s_rot_m = s_apply0_v = s_apply1_v = 0;
    s_ratan_y = s_ratan_x = 0;
    memset(&s_rot_in, 0, sizeof(s_rot_in));
}

static void clear_tables(void)
{
    u32 i;

    for (i = 0u; i < 0x200u; i++)
        memset(PSX_ADDR(emit(i)), 0, EMIT_STR);
    for (i = 0u; i < 0x100u; i++)
        memset(PSX_ADDR(rec(i)), 0, SLOT_STR);
}

static void seed_spawn_fields(u32 idx, u8 flags)
{
    u32 e = emit(idx);
    u32 s = s1_of(idx);

    wr16(s + 4u, 8);
    wr16(s + 6u, 1);
    wr32(s + 8u, 0x00030005u);
    wr16(s + 0xCu, 0);
    wr16(s + 0xEu, 0);
    wr16(s + 0x10u, 1);
    wr16(s + 0x12u, 1);
    wr16(s + 0x14u, 1);
    wr16(e + 0x1Cu, 0x10);
    wr16(e + 0x1Eu, 0x20);
    wr16(e + 0x20u, 0x30);
    wr16(e + 0x24u, 0x40);
    wr16(e + 0x26u, 0x50);
    wr16(e + 0x28u, 0x60);
    wr16(e + 0x2Cu, 0x70);
    wr16(e + 0x2Eu, 0x80);
    wr16(e + 0x30u, 0x90);
    wr32(s + 0x30u, 0x1000u);
    wr16(s + 0x34u, 7);
    wr16(s + 0x36u, 8);
    wr16(s + 0x38u, 9);
    wr16(s + 0x3Cu, 5);
    wr16(s + 0x3Eu, 13);
    wr32(s + 0x40u, 0xA1A1A1A1u);
    wr32(s + 0x44u, 0xA2A2A2A2u);
    wr32(s + 0x48u, 0xA3A3A3A3u);
    wr32(s + 0x4Cu, 0xA4A4A4A4u);
    wr8(s + 0x4Bu, flags);
}

static void seed_base(void)
{
    PsxMemory_Init();
    wr32(TAB_PTR, TAB);
    wr32(REC_PTR, RECS);
    wr32(CANARY, 0x11111111u);
    clear_tables();
    reset_calls();
}

static void expect_spawned_slot0(u32 idx)
{
    chk("p0.idx", (u32)rd16(rec(0)), idx);
    chk("p0.ang", (u32)rd16(rec(0) + 2u), 0x1234u);
    chk("p0.life", rd32(rec(0) + 4u), 0x00030005u);
    chk("p0.x", rd32(rec(0) + 8u), 12788u);
    chk("p0.y", rd32(rec(0) + 0xCu), 17384u);
    chk("p0.z", rd32(rec(0) + 0x10u), 21980u);
    chk("p0.vx", rd32(rec(0) + 0x18u), 104u);
    chk("p0.vy", rd32(rec(0) + 0x1Cu), 204u);
    chk("p0.vz", rd32(rec(0) + 0x20u), 304u);
    chk("p0.ax", rd32(rec(0) + 0x28u), 7u);
    chk("p0.ay", rd32(rec(0) + 0x2Cu), 8u);
    chk("p0.az", rd32(rec(0) + 0x30u), 9u);
    chk("p0.w38", rd32(rec(0) + 0x38u), 0xA1A1A1A1u);
    chk("p0.w3c", rd32(rec(0) + 0x3Cu), 0xA2A2A2A2u);
    /* s1+0x48 overlaps the flags byte at s1+0x4B (LE high byte). */
    chk("p0.w40", rd32(rec(0) + 0x40u), 0xECA3A3A3u);
    chk("p0.w44", rd32(rec(0) + 0x44u), 0xA4A4A4A4u);
    chk("p0.pack", (u32)rd16(rec(0) + 0x48u), 0x9Du);
    chk("count", (u32)rd16(s1_of(idx) + 6u), 2u);
    chk("nrot", (u32)s_nrot, 1u);
    chk("napply", (u32)s_napply, 2u);
    chk("nnorm", (u32)s_nnorm, 3u);
    chk("nratan", (u32)s_nratan, 1u);
    chk("ratan.y", (u32)s_ratan_y, 204u);
    chk("ratan.x", (u32)s_ratan_x, 104u);
    chk("rot.r", s_rot_r, emit(idx) + 0x1Cu);
    chk("rot.m", s_rot_m, 0x1F8000F0u);
    chk("apply0.v", s_apply0_v, emit(idx) + 0x24u);
    chk("apply1.v", s_apply1_v, emit(idx) + 0x2Cu);
    chk("rot.vx", (u32)(u16)s_rot_in.vx, 0x10u);
    chk("rot.vy", (u32)(u16)s_rot_in.vy, 0x20u);
    chk("rot.vz", (u32)(u16)s_rot_in.vz, 0x30u);
}

int main(void)
{
    seed_base();
    seed_spawn_fields(0, 0x00);
    seed_spawn_fields(1, 0xECu);
    wm_80089748();
    expect_spawned_slot0(1u);
    chk("e0.flags", (u32)rd8(s1_of(0) + 0x4Bu), 0u);
    chk("e0.word", rd32(s1_of(0)), 0u);
    chk("e1.word", rd32(s1_of(1)), 0u);
    chk("p1.life", rd32(rec(1) + 4u), 0u);
    chk("n89580.spawn", (u32)s_n89580, 1u);
    chk("canary.spawn", rd32(CANARY), 0x11111111u);

    seed_base();
    wr8(s1_of(0) + 0x4Bu, 0x80);
    wr16(s1_of(0) + 0xEu, 3);
    wr32(s1_of(0), 0xDEADu);
    wm_80089748();
    chk("int.left", (u32)rd16(s1_of(0) + 0xEu), 2u);
    chk("int.word", rd32(s1_of(0)), 0u);
    chk("int.nrot", (u32)s_nrot, 0u);
    chk("int.p0", rd32(rec(0) + 4u), 0u);
    chk("int.89580", (u32)s_n89580, 1u);

    seed_base();
    wr8(s1_of(0) + 0x4Bu, 0x90);
    wr32(s1_of(0), 0x00020005u);
    wm_80089748();
    chk("d10.word", rd32(s1_of(0)), 0x00020004u);
    chk("d10.flags", (u32)rd8(s1_of(0) + 0x4Bu), 0x90u);
    chk("d10.nrot", (u32)s_nrot, 0u);

    seed_base();
    wr8(s1_of(0) + 0x4Bu, 0x90);
    wr32(s1_of(0), 0u);
    wm_80089748();
    chk("deact.flags", (u32)rd8(s1_of(0) + 0x4Bu), 0x10u);
    chk("deact.word", rd32(s1_of(0)), 0u);
    chk("deact.nrot", (u32)s_nrot, 0u);

    seed_base();
    seed_spawn_fields(0, 0xECu);
    wr16(rec(0) + 6u, 1);
    wm_80089748();
    chk("occ.p0.x", rd32(rec(0) + 8u), 0u);
    chk("occ.p0.idx", (u32)rd16(rec(0)), 0u);
    chk("occ.p1.idx", (u32)rd16(rec(1)), 0u);
    chk("occ.p1.x", rd32(rec(1) + 8u), 12788u);
    chk("occ.p1.pack", (u32)rd16(rec(1) + 0x48u), 0x9Du);

    seed_base();
    seed_spawn_fields(0, 0xECu);
    wr16(s1_of(0) + 6u, 8);
    wm_80089748();
    chk("full.nrot", (u32)s_nrot, 0u);
    chk("full.p0", rd32(rec(0) + 4u), 0u);
    chk("full.count", (u32)rd16(s1_of(0) + 6u), 8u);
    chk("full.int", (u32)rd16(s1_of(0) + 0xEu), 0u);

    seed_base();
    wm_80089748();
    chk("empty.89580", (u32)s_n89580, 1u);
    chk("empty.nrot", (u32)s_nrot, 0u);
    chk("canary.empty", rd32(CANARY), 0x11111111u);

    if (s_failures) {
        fprintf(stderr, "ASSERTION failures=%d\n", s_failures);
        return 1;
    }
    printf("W34B24-I31 0x80089748 focused oracle PASS\n");
    return 0;
}
