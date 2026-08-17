/*
 * Focused production-linked oracle for retail helper 0x80085CDC.
 *
 * Expected values are hand-derived from [0x80085CDC, 0x80085F58).
 * PsyQ/SLUS residents are recording stubs. Real wm_80093484 is linked.
 * NEXT_CALLBACK_TARGET is not an oracle input.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_85cdc.h"

#define POOL_PTR  0x8009BE24u
#define POS       0x8009BE28u
#define CAMERA    0x8009C808u
#define DB_PTR    0x8009BE3Cu
#define DB        0x8009BBC8u
#define OT        0x800F0000u
#define ANGLE     0x8009BD3Au
#define WRAP_X    0x8009D160u
#define WRAP_Z    0x8009D2B4u
#define POOL      0x80090000u
#define SPRITE0   0x80093000u
#define SPRITE2   0x80093100u
#define SPRITE3   0x80093200u
#define SC        0x1F800000u

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

static int s_nsetrot;
static int s_nsettrans;
static int s_n24ff4;
static int s_nrtps;
static int s_nsubmit;
static int s_n223b0;
static int s_ntick;
static u32 s_setrot;
static u32 s_settrans;
static u32 s_24ff4;
static u32 s_rtps_vx[4];
static u32 s_sz[4];
static u32 s_submit_sp[4];
static u32 s_submit_ot[4];
static u32 s_223_sp[4];
static u32 s_223_ang[4];
static u32 s_tick_sp[4];

typedef struct {
    s16 m[3][3];
    s32 t[3];
} MATRIX;

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

void func_80024FF4(MATRIX *m)
{
    s_24ff4 = guest_of(m);
    s_n24ff4++;
}

void func_8001E298(void *sprite, void *ot)
{
    if (s_nsubmit < 4) {
        s_submit_sp[s_nsubmit] = guest_of(sprite);
        s_submit_ot[s_nsubmit] = guest_of(ot);
    }
    s_nsubmit++;
}

void func_800223B0(void *sprite, s16 angle)
{
    if (s_n223b0 < 4) {
        s_223_sp[s_n223b0] = guest_of(sprite);
        s_223_ang[s_n223b0] = (u32)(u16)angle;
    }
    s_n223b0++;
}

void AnimScriptTick(void *sprite)
{
    if (s_ntick < 4)
        s_tick_sp[s_ntick] = guest_of(sprite);
    s_ntick++;
}

u32 wm_85cdc_test_rtps_sz3(u32 svec_addr)
{
    u32 n = (u32)s_nrtps;
    u32 vx = (u32)(u16)rd16(svec_addr);

    if (n < 4u) {
        s_rtps_vx[n] = vx;
        if (vx == 4u)
            s_sz[n] = 0x0800u;
        else if (vx == 8u)
            s_sz[n] = 0x0C00u;
        else
            s_sz[n] = 0x0400u;
    }
    s_nrtps++;
    return (n < 4u) ? s_sz[n] : 0x0C00u;
}

void wm_85cdc_test_store(u32 address, u32 value)
{
    (void)address;
    (void)value;
}

static u32 slot(u32 i)
{
    return POOL + i * 0x80u;
}

static void reset(void)
{
    s_nsetrot = s_nsettrans = s_n24ff4 = s_nrtps = 0;
    s_nsubmit = s_n223b0 = s_ntick = 0;
    s_setrot = s_settrans = s_24ff4 = 0;
    memset(s_rtps_vx, 0, sizeof(s_rtps_vx));
    memset(s_sz, 0, sizeof(s_sz));
    memset(s_submit_sp, 0, sizeof(s_submit_sp));
    memset(s_submit_ot, 0, sizeof(s_submit_ot));
    memset(s_223_sp, 0, sizeof(s_223_sp));
    memset(s_223_ang, 0, sizeof(s_223_ang));
    memset(s_tick_sp, 0, sizeof(s_tick_sp));
}

static void seed(void)
{
    u32 i;

    PsxMemory_Init();
    reset();
    wr32(POOL_PTR, POOL);
    wr32(POS, 0x1000u);
    wr32(POS + 8u, 0x2000u);
    wr32(DB_PTR, DB);
    wr32(DB + 0x70u, OT);
    wr16(ANGLE, 0x0100u);
    wr32(WRAP_X, 1u);
    wr32(WRAP_Z, 1u);
    wr32(CAMERA, 0xCAFE0001u);
    for (i = 0u; i < 64u; i++) {
        wr16(slot(i) + 0x24u, 1u);
        wr32(slot(i) + 0x28u, 0x11110000u + i);
        wr32(slot(i) + 0x2Cu, 0x22220000u + i);
        wr32(slot(i) + 0x30u, 0x33330000u + i);
        wr32(slot(i) + 0x4Cu, 0u);
        wr16(slot(i) + 0x48u, 0u);
        wr32(slot(i) + 0x5Cu, 0xABCDu);
    }

    /* Slot 0: active, near SZ, angle step +0x100. */
    wr16(slot(0) + 0x24u, 0u);
    wr32(slot(0) + 0x28u, 0x5000u);
    wr32(slot(0) + 0x2Cu, 0x0030u);
    wr32(slot(0) + 0x30u, 0x6000u);
    wr32(slot(0) + 0x4Cu, SPRITE0);
    wr16(slot(0) + 0x48u, 0x0200u);
    wr32(slot(0) + 0x5Cu, 0u);
    wr32(SPRITE0, 0xDEAD0001u);
    wr32(SPRITE0 + 4u, 0xDEAD0002u);
    wr32(SPRITE0 + 8u, 0xDEAD0003u);

    /* Slot 1 stays inactive (flag=1). */

    /* Slot 2: active, far SZ (>= 0xB00), no submit. */
    wr16(slot(2) + 0x24u, 0u);
    wr32(slot(2) + 0x28u, 0x9000u);
    wr32(slot(2) + 0x2Cu, 0x0040u);
    wr32(slot(2) + 0x30u, 0xA000u);
    wr32(slot(2) + 0x4Cu, SPRITE2);
    wr16(slot(2) + 0x48u, 0x0100u);
    wr32(slot(2) + 0x5Cu, 0u);
    wr32(SPRITE2, 0xBEEF0001u);

    /* Slot 3: active, near SZ, snap-to-target (delta < 0x100). */
    wr16(slot(3) + 0x24u, 0u);
    wr32(slot(3) + 0x28u, 0x1800u);
    wr32(slot(3) + 0x2Cu, 0x0010u);
    wr32(slot(3) + 0x30u, 0x2800u);
    wr32(slot(3) + 0x4Cu, SPRITE3);
    wr16(slot(3) + 0x48u, 0x0050u);
    wr32(slot(3) + 0x5Cu, 0x0010u);
    wr32(SPRITE3, 0xCAFE0001u);
}

int main(void)
{
    seed();
    wm_80085CDC();

    chk("nsetrot", (u32)s_nsetrot, 1u);
    chk("nsettrans", (u32)s_nsettrans, 1u);
    chk("n24ff4", (u32)s_n24ff4, 1u);
    chk("setrot", s_setrot, CAMERA);
    chk("settrans", s_settrans, CAMERA);
    chk("24ff4", s_24ff4, CAMERA);
    chk("nrtps", (u32)s_nrtps, 3u);
    chk("nsubmit", (u32)s_nsubmit, 2u);
    chk("n223b0", (u32)s_n223b0, 2u);
    chk("ntick", (u32)s_ntick, 2u);

    /* Slot 0: dx=0x4000 dz=0x4000, no wrap. */
    chk("s0.x", rd32(SPRITE0), 0x4000u << 4);
    chk("s0.y", rd32(SPRITE0 + 4u), 0x0030u << 4);
    chk("s0.z", rd32(SPRITE0 + 8u), (0u - 0x4000u) << 4);
    chk("s0.sz", rd32(SC + 0x18u), 0x0800u);
    chk("s0.ang", rd32(slot(0) + 0x5Cu), 0x0100u);
    chk("s0.sub.sp", s_submit_sp[0], SPRITE0);
    chk("s0.sub.ot", s_submit_ot[0], OT + ((0x0800u >> 4) << 2));
    chk("s0.223.sp", s_223_sp[0], SPRITE0);
    /* chased=0x100, cam=0x100, (0x100-0x100-0x400)&0xFFF = 0xC00 */
    chk("s0.223.ang", s_223_ang[0], 0x0C00u);
    chk("s0.tick", s_tick_sp[0], SPRITE0);
    chk("rtps0.vx", s_rtps_vx[0], 4u);

    /* Slot 1 inactive: record and canary sprite pointer stay put. */
    chk("s1.flag", rd16(slot(1) + 0x24u), 1u);
    chk("s1.ptr", rd32(slot(1) + 0x4Cu), 0u);
    chk("s1.ang", rd32(slot(1) + 0x5Cu), 0xABCDu);
    chk("s1.x", rd32(slot(1) + 0x28u), 0x11110001u);

    /* Slot 2: written VECTOR, SZ stored, no submit. */
    chk("s2.x", rd32(SPRITE2), 0x8000u << 4);
    chk("s2.y", rd32(SPRITE2 + 4u), 0x0040u << 4);
    chk("s2.z", rd32(SPRITE2 + 8u), (0u - 0x8000u) << 4);
    chk("s2.sz", rd32(SC + 0x18u + 8u), 0x0C00u);
    chk("s2.ang", rd32(slot(2) + 0x5Cu), 0u);
    chk("rtps1.vx", s_rtps_vx[1], 8u);

    /* Slot 3: snap to target 0x50. */
    chk("s3.x", rd32(SPRITE3), 0x0800u << 4);
    chk("s3.y", rd32(SPRITE3 + 4u), 0x0010u << 4);
    chk("s3.z", rd32(SPRITE3 + 8u), (0u - 0x0800u) << 4);
    chk("s3.sz", rd32(SC + 0x18u + 12u), 0x0400u);
    chk("s3.ang", rd32(slot(3) + 0x5Cu), 0x0050u);
    chk("s3.sub.sp", s_submit_sp[1], SPRITE3);
    chk("s3.sub.ot", s_submit_ot[1], OT + ((0x0400u >> 4) << 2));
    chk("s3.223.sp", s_223_sp[1], SPRITE3);
    /* chased=0x50, cam=0x100, (0x50-0x100-0x400)&0xFFF = 0xB50 */
    chk("s3.223.ang", s_223_ang[1], 0x0B50u);
    chk("s3.tick", s_tick_sp[1], SPRITE3);
    chk("rtps2.vx", s_rtps_vx[2], 0u);

    if (s_failures) {
        fprintf(stderr, "ASSERTION failures=%d\n", s_failures);
        return 1;
    }
    printf("W34B24-I26 0x80085CDC focused oracle PASS\n");
    return 0;
}
