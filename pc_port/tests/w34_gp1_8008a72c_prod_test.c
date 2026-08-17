/*
 * Focused production-linked oracle for retail world callback 0x8008A72C.
 *
 * Callees are scripted recording seams.  Expectations are hand-derived
 * from the retail disassembly (entry +4 machine, 65-way table bound,
 * state 0/1 movement arm through both 95414 sites, epilogue sra12,
 * selected other table arms).
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_callback_8a72c.h"

#define POOL      0x80020000u
#define SLOT1     (POOL + 0x80u)
#define BE24      0x8009BE24u
#define MODE      0x8009BE10u
#define BD04      0x8009BD04u
#define BD60      0x8009BD60u
#define C170      0x8009C170u
#define D554      0x8009D554u
#define D7CC      0x8009D7CCu
#define D738      0x8009D738u
#define B180      0x8009B180u
#define D154      0x8009D154u
#define POSE      0x8009CEC4u
#define D55C      0x8009D55Cu
#define D52C      0x8009D52Cu
#define F8E5      0x8006F8E5u
#define F8E6      0x8006F8E6u
#define F8E7      0x8006F8E7u
#define F368      0x8006F368u
#define F369      0x8006F369u
#define F36A      0x8006F36Au
#define EE54      0x8006EE54u
#define EE56      0x8006EE56u
#define EE58      0x8006EE58u
#define EE5A      0x8006EE5Au
#define EF90      0x8006EF90u
#define EF92      0x8006EF92u
#define SCRATCH   0x1F800000u

#define OFF_CLAIM    0x04u
#define OFF_CONTROL  0x20u
#define OFF_FLAG     0x24u
#define OFF_X        0x28u
#define OFF_Y        0x2Cu
#define OFF_Z        0x30u
#define OFF_AUX      0x34u
#define OFF_VX       0x38u
#define OFF_VY       0x3Cu
#define OFF_VZ       0x40u
#define OFF_VW       0x44u
#define OFF_STATE    0x48u
#define OFF_CONST    0x4Au
#define OFF_OBJECT   0x4Cu
#define OFF_COUNTER  0x58u

static int s_failures;

static void check(const char *name, u32 got, u32 want)
{
    if (got != want) {
        fprintf(stderr, "ASSERTION %s: got=0x%08x expected=0x%08x\n",
                name, got, want);
        if (++s_failures > 40)
            exit(1);
    }
}

static u32 rd(u32 a) { u32 v; memcpy(&v, PSX_ADDR(a), 4); return v; }
static u16 rd16(u32 a) { u16 v; memcpy(&v, PSX_ADDR(a), 2); return v; }
static void wr(u32 a, u32 v) { memcpy(PSX_ADDR(a), &v, 4); }
static void wr16(u32 a, u16 v) { memcpy(PSX_ADDR(a), &v, 2); }
static void wr8(u32 a, u8 v) { *(u8 *)PSX_ADDR(a) = v; }

typedef struct {
    char kind[12];
    u32 a, b, c, d, e;
} Ev;

static Ev s_log[64];
static u32 s_nlog;
static u8 s_object[0xC0];
static s32 s_90a84_ret;
static s32 s_95414_script[4];
static u32 s_95414_n, s_95414_i;
static s32 s_97770_script[8];
static u32 s_97770_n, s_97770_i;
static s32 s_8bec8_ret;
static s32 s_93978_ret;
static long s_rcos_ret;
static long s_rsin_ret;

static void logev(const char *k, u32 a, u32 b, u32 c, u32 d, u32 e)
{
    if (s_nlog < 64u) {
        snprintf(s_log[s_nlog].kind, sizeof(s_log[s_nlog].kind), "%s", k);
        s_log[s_nlog].a = a;
        s_log[s_nlog].b = b;
        s_log[s_nlog].c = c;
        s_log[s_nlog].d = d;
        s_log[s_nlog].e = e;
    }
    s_nlog++;
}

static const Ev *ev(u32 i)
{
    if (i >= s_nlog || i >= 64u) {
        fprintf(stderr, "ASSERTION eventlog: index %u >= count %u\n", i, s_nlog);
        exit(1);
    }
    return &s_log[i];
}

static void expect_ev(const char *name, u32 i, const char *kind,
                      u32 a, u32 b, u32 c, u32 d, u32 e)
{
    const Ev *v = ev(i);
    char nm[96];

    snprintf(nm, sizeof(nm), "%s.kind", name);
    if (strcmp(v->kind, kind) != 0) {
        fprintf(stderr, "ASSERTION %s: got=%s expected=%s\n", nm, v->kind, kind);
        s_failures++;
        return;
    }
    snprintf(nm, sizeof(nm), "%s.a", name); check(nm, v->a, a);
    snprintf(nm, sizeof(nm), "%s.b", name); check(nm, v->b, b);
    snprintf(nm, sizeof(nm), "%s.c", name); check(nm, v->c, c);
    snprintf(nm, sizeof(nm), "%s.d", name); check(nm, v->d, d);
    snprintf(nm, sizeof(nm), "%s.e", name); check(nm, v->e, e);
}

static void overrun(const char *who)
{
    fprintf(stderr, "ASSERTION %s: seam script overrun\n", who);
    exit(1);
}

s32 wm_8a72c_test_90a84(u32 slot_addr)
{
    logev("90a84", slot_addr, 0, 0, 0, 0);
    return s_90a84_ret;
}

s32 wm_8a72c_test_95414(u32 pos, u32 dir, u32 out, s32 scale, s32 mode)
{
    logev("95414", pos, dir, out, (u32)scale, (u32)mode);
    if (s_95414_i >= s_95414_n)
        overrun("95414");
    return s_95414_script[s_95414_i++];
}

void wm_8a72c_test_894c8(u32 record_index)
{
    logev("894c8", record_index, 0, 0, 0, 0);
}

void wm_8a72c_test_8c1dc(u32 ctx, u32 obj, u32 out)
{
    logev("8c1dc", ctx, obj, out, 0, 0);
}

void wm_8a72c_test_8c040(u32 vec, s32 a1, s32 a2, u32 out1, u32 out2)
{
    logev("8c040", vec, (u32)a1, (u32)a2, out1, out2);
}

s32 wm_8a72c_test_94238(u32 pos, u32 list_index)
{
    logev("94238", pos, list_index, 0, 0, 0);
    return 0;
}

void wm_8a72c_test_7528c(void)
{
    logev("7528c", 0, 0, 0, 0, 0);
}

void wm_8a72c_test_74794(s32 tag, u32 pose_addr)
{
    logev("74794", (u32)tag, pose_addr, 0, 0, 0);
}

s32 wm_8a72c_test_97770(u32 slot_idx, s32 value)
{
    logev("97770", slot_idx, (u32)value, 0, 0, 0);
    if (s_97770_i >= s_97770_n)
        overrun("97770");
    return s_97770_script[s_97770_i++];
}

s32 wm_8a72c_test_941c4(u32 a, u32 b, u32 out, u32 angle)
{
    logev("941c4", a, b, out, angle, 0);
    return 0;
}

s32 wm_8a72c_test_8bec8(u32 obj)
{
    logev("8bec8", obj, 0, 0, 0, 0);
    return s_8bec8_ret;
}

s32 wm_8a72c_test_93978(s32 x, s32 z)
{
    logev("93978", (u32)x, (u32)z, 0, 0, 0);
    return s_93978_ret;
}

void wm_8a72c_test_245d8(void* object, s16 animation)
{
    logev("245d8", (u32)(uintptr_t)object, (u32)(u16)animation, 0, 0, 0);
}

long wm_8a72c_test_rcos(long a)
{
    logev("rcos", (u32)a, 0, 0, 0, 0);
    return s_rcos_ret;
}

long wm_8a72c_test_rsin(long a)
{
    logev("rsin", (u32)a, 0, 0, 0, 0);
    return s_rsin_ret;
}

static u32 object_bits(void)
{
    uintptr_t bits = (uintptr_t)(void *)s_object;
    if (bits > (uintptr_t)UINT32_MAX)
        abort();
    return (u32)bits;
}

static void reset_world(void)
{
    memset(PSX_ADDR(POOL), 0, 0x800u);
    memset(PSX_ADDR(SCRATCH), 0, 0x200u);
    memset(PSX_ADDR(POSE), 0, 32u * 0x14u);
    memset(s_object, 0, sizeof(s_object));
    memset(s_log, 0, sizeof(s_log));
    s_nlog = 0;
    s_90a84_ret = 0;
    s_95414_n = s_95414_i = 0;
    s_97770_n = s_97770_i = 0;
    s_8bec8_ret = 0;
    s_93978_ret = 0x1234000;
    s_rcos_ret = 0x1000;
    s_rsin_ret = 0x0100;
    wr(BE24, POOL);
    wr(MODE, 0);
    wr16(BD04, 0xABCDu);
    wr8(BD60, 0);
    wr(C170, 0);
    wr(D554, 0x11111111u);
    wr(D7CC, 0x22222222u);
    wr8(D738, 0);
    wr16(B180, 0);
    wr16(D154, 0);
    wr8(F8E5, 0);
    wr8(F8E6, 0);
    wr8(F8E7, 0);
    wr8(F368, 0xFF);
    wr8(F369, 0xFF);
    wr8(F36A, 0xFF);
    wr16(EE54, 0);
    wr16(EE56, 0);
    wr16(EE58, 0);
    wr16(EE5A, 0);
    wr16(EF90, 0);
    wr16(EF92, 0);
}

static void prep_slot(u16 control, u16 claim)
{
    wr16(SLOT1 + OFF_CLAIM, claim);
    wr16(SLOT1 + OFF_CONTROL, control);
    wr16(SLOT1 + OFF_FLAG, 0);
    wr(SLOT1 + OFF_X, 0x00100000u);
    wr(SLOT1 + OFF_Y, 0x00002000u);
    wr(SLOT1 + OFF_Z, 0x00300000u);
    wr(SLOT1 + OFF_AUX, 0x00000007u);
    wr(SLOT1 + OFF_VX, 0);
    wr(SLOT1 + OFF_VY, 0);
    wr(SLOT1 + OFF_VZ, 0);
    wr16(SLOT1 + OFF_STATE, 0x0040u);
    wr16(SLOT1 + OFF_CONST, 8u);
    wr(SLOT1 + OFF_OBJECT, object_bits());
    wr(SLOT1 + OFF_COUNTER, 0);
}

static void test_plus4_case2(void)
{
    reset_world();
    prep_slot(0, 2);
    wr16(EF90, 0x0010u);
    wr16(EF92, 0x0030u);
    wr16(EE5A, 0x0040u);
    s_93978_ret = 0x00002000;
    s_90a84_ret = 2;
    check("plus4-2.ret", (u32)wm_8008A72C(1), 1u);
    check("plus4-2.claim", rd16(SLOT1 + OFF_CLAIM), 0u);
    check("plus4-2.control", rd16(SLOT1 + OFF_CONTROL), 0x29u);
    expect_ev("plus4-2.93978", 0, "93978", 0x10000u, 0x30000u, 0, 0, 0);
    expect_ev("plus4-2.rcos", 1, "rcos", 0x40u, 0, 0, 0, 0);
    expect_ev("plus4-2.rsin", 2, "rsin", 0x40u, 0, 0, 0, 0);
    expect_ev("plus4-2.941c4", 3, "941c4", SLOT1 + OFF_X, SCRATCH,
              SLOT1 + OFF_VX, SLOT1 + OFF_STATE, 0);
    expect_ev("plus4-2.245d8", 4, "245d8", object_bits(), 1u, 0, 0, 0);
    expect_ev("plus4-2.74794", 5, "74794", 0, SLOT1 + OFF_X, 0, 0, 0);
    check("plus4-2.nlog", s_nlog, 6u);
}

static void test_plus4_case3(void)
{
    reset_world();
    prep_slot(9, 3);
    s_object[0xAF] = 0;
    s_90a84_ret = 0;
    s_95414_script[0] = 2;
    s_95414_n = 1;
    check("plus4-3.ret", (u32)wm_8008A72C(1), 1u);
    check("plus4-3.claim", rd16(SLOT1 + OFF_CLAIM), 0u);
    check("plus4-3.control", rd16(SLOT1 + OFF_CONTROL), 1u);
}

static void test_plus4_case6(void)
{
    reset_world();
    prep_slot(7, 6);
    wr(SLOT1 + OFF_COUNTER, 4u);
    wr(C170, 5u);
    s_90a84_ret = 2;
    check("plus4-6.ret", (u32)wm_8008A72C(1), 1u);
    check("plus4-6.claim", rd16(SLOT1 + OFF_CLAIM), 0u);
    check("plus4-6.counter", rd(SLOT1 + OFF_COUNTER), 5u);
    check("plus4-6.control", rd16(SLOT1 + OFF_CONTROL), 0u);
    check("plus4-6.mode", rd(MODE), 1u);
    check("plus4-6.bd04", rd16(BD04), 0u);
}

static void test_bound_65(void)
{
    reset_world();
    prep_slot(65, 0);
    check("bound65.ret", (u32)wm_8008A72C(1), 1u);
    check("bound65.control", rd16(SLOT1 + OFF_CONTROL), 65u);
    expect_ev("bound65.74794", 0, "74794", 0, SLOT1 + OFF_X, 0, 0, 0);
    check("bound65.nlog", s_nlog, 1u);
}

static void test_state0_idle_95414(void)
{
    reset_world();
    prep_slot(0, 0);
    s_object[0xAF] = 1;
    s_90a84_ret = 0;
    s_95414_script[0] = 0;
    s_95414_script[1] = 1;
    s_95414_n = 2;
    wr(SCRATCH + 0x90u, 0x00A00000u);
    wr(SCRATCH + 0x94u, 0x00B00000u);
    wr(SCRATCH + 0x98u, 0x00C00000u);
    wr(SCRATCH + 0x9Cu, 0x00D00000u);
    wr8(BD60, 0);
    wr8(D738, 1);
    wr16(B180 + 2u, 0x0001u);
    wr(SLOT1 + OFF_VX, 0x10u);
    wr(SLOT1 + OFF_VZ, 0x20u);
    wr16(D154, 3u);

    check("s0.ret", (u32)wm_8008A72C(1), 1u);
    expect_ev("s0.90a84", 0, "90a84", SLOT1, 0, 0, 0, 0);
    expect_ev("s0.8c1dc", 1, "8c1dc", 0x2Fu, SLOT1, SCRATCH, 0, 0);
    expect_ev("s0.95414a", 2, "95414", SLOT1 + OFF_X, SLOT1 + OFF_VX,
              SCRATCH + 0x90u, 0x8000u, 0);
    expect_ev("s0.95414b", 3, "95414", SLOT1 + OFF_X, SLOT1 + OFF_VX,
              SCRATCH + 0x90u, 0x8000u, 0);
    expect_ev("s0.8c040", 4, "8c040", SCRATCH + 0x90u, 0x10u, 0x20u,
              D738, BD60);
    expect_ev("s0.7528c", 5, "7528c", 0, 0, 0, 0, 0);
    expect_ev("s0.94238", 6, "94238", SLOT1 + OFF_X, 0, 0, 0, 0);
    expect_ev("s0.74794", 7, "74794", 0, SLOT1 + OFF_X, 0, 0, 0);
    check("s0.nlog", s_nlog, 8u);
    check("s0.x", rd(SLOT1 + OFF_X), 0x00A00000u);
    check("s0.y", rd(SLOT1 + OFF_Y), 0x00B00000u);
    check("s0.z", rd(SLOT1 + OFF_Z), 0x00C00000u);
    check("s0.vx", rd(SLOT1 + OFF_VX), 0u);
    check("s0.vz", rd(SLOT1 + OFF_VZ), 0u);
    check("s0.d55c", rd(D55C), 0x00A00000u);
    check("s0.d52c", rd16(D52C), 0x40u);
    check("s0.bd04", rd16(BD04), 0u);
    check("s0.flag", rd16(SLOT1 + OFF_FLAG), 0u);
    check("s0.d154", rd16(D154), 4u);
    check("s0.pose.x", rd(POSE + 4u * 0x14u), 0x00A00000u);
    check("s0.ee54", rd16(EE54), 0xA00u);
}

static void test_state0_f8e5(void)
{
    reset_world();
    prep_slot(1, 0);
    wr8(F8E5, 1);
    wr(POOL + 0x228u, 0x11110000u);
    wr(POOL + 0x22Cu, 0x22220000u);
    wr(POOL + 0x230u, 0x33330000u);
    wr16(POOL + 0x248u, 0x0055u);
    check("f8e5.ret", (u32)wm_8008A72C(1), 1u);
    check("f8e5.x", rd(SLOT1 + OFF_X), 0x11110000u);
    check("f8e5.y", rd(SLOT1 + OFF_Y), 0x22220000u);
    check("f8e5.z", rd(SLOT1 + OFF_Z), 0x33330000u);
    check("f8e5.state", rd16(SLOT1 + OFF_STATE), 0x55u);
    check("f8e5.flag", rd16(SLOT1 + OFF_FLAG), 1u);
    expect_ev("f8e5.894c8", 0, "894c8", 0x2Fu, 0, 0, 0, 0);
    check("f8e5.nlog", s_nlog, 1u);
}

static void test_90a84_ret1(void)
{
    reset_world();
    prep_slot(0, 0);
    s_90a84_ret = 1;
    check("r1.ret", (u32)wm_8008A72C(1), 1u);
    check("r1.control", rd16(SLOT1 + OFF_CONTROL), 0x40u);
    check("r1.d554", rd(D554), 0u);
    check("r1.d7cc", rd(D7CC), 0u);
    check("r1.bd04", rd16(BD04), 0u);
    check("r1.flag", rd16(SLOT1 + OFF_FLAG), 0u);
    expect_ev("r1.90a84", 0, "90a84", SLOT1, 0, 0, 0, 0);
    expect_ev("r1.74794", 1, "74794", 0, SLOT1 + OFF_X, 0, 0, 0);
    check("r1.nlog", s_nlog, 2u);
}

static void test_state2_copy(void)
{
    reset_world();
    prep_slot(2, 0);
    wr(POOL + 0x3A8u, 0xAAA00000u);
    wr(POOL + 0x3ACu, 0xBBB00000u);
    wr(POOL + 0x3B0u, 0xCCC00000u);
    wr16(POOL + 0x3C8u, 0x0077u);
    check("s2.ret", (u32)wm_8008A72C(1), 1u);
    check("s2.x", rd(SLOT1 + OFF_X), 0xAAA00000u);
    check("s2.y", rd(SLOT1 + OFF_Y), 0xBBB00000u);
    check("s2.z", rd(SLOT1 + OFF_Z), 0xCCC00000u);
    check("s2.state", rd16(SLOT1 + OFF_STATE), 0x77u);
    expect_ev("s2.74794", 0, "74794", 0, SLOT1 + OFF_X, 0, 0, 0);
}

static void test_state10_ff(void)
{
    reset_world();
    prep_slot(10, 0);
    wr8(F368, 0xFF);
    check("s10.ret", (u32)wm_8008A72C(1), 1u);
    check("s10.control", rd16(SLOT1 + OFF_CONTROL), 0xDu);
    check("s10.nlog", s_nlog, 1u);
}

static void test_always_one(void)
{
    reset_world();
    prep_slot(4, 0);
    check("def.ret", (u32)wm_8008A72C(1), 1u);
    check("def.control", rd16(SLOT1 + OFF_CONTROL), 4u);
    expect_ev("def.74794", 0, "74794", 0, SLOT1 + OFF_X, 0, 0, 0);
    check("def.nlog", s_nlog, 1u);
}

static void test_negative_sra12(void)
{
    reset_world();
    prep_slot(4, 0);
    wr(SLOT1 + OFF_X, 0xFFFFF000u);
    wr(SLOT1 + OFF_Z, 0xFFFFE000u);
    check("sra.ret", (u32)wm_8008A72C(1), 1u);
    check("sra.ee54", rd16(EE54), 0xFFFFu);
    check("sra.ee56", rd16(EE56), 0xFFFEu);
}

static void test_95414_nonzero_first(void)
{
    reset_world();
    prep_slot(0, 0);
    s_object[0xAF] = 1;
    s_90a84_ret = 0;
    s_95414_script[0] = 2;
    s_95414_n = 1;
    wr(SLOT1 + OFF_VX, 0x10u);
    wr(SLOT1 + OFF_VZ, 0x20u);
    check("nz.ret", (u32)wm_8008A72C(1), 1u);
    expect_ev("nz.95414", 2, "95414", SLOT1 + OFF_X, SLOT1 + OFF_VX,
              SCRATCH + 0x90u, 0x8000u, 0);
    expect_ev("nz.8c040", 3, "8c040", SLOT1 + OFF_X, 0x10u, 0x20u,
              D738, BD60);
    expect_ev("nz.94238", 4, "94238", SLOT1 + OFF_X, 0, 0, 0, 0);
    check("nz.nlog", s_nlog, 6u);
}

static void test_flag_skips_74794(void)
{
    reset_world();
    prep_slot(4, 0);
    wr16(SLOT1 + OFF_FLAG, 1u);
    check("flag.ret", (u32)wm_8008A72C(1), 1u);
    check("flag.nlog", s_nlog, 0u);
    check("flag.ee54", rd16(EE54), 0x100u);
}

int main(void)
{
    printf("W34-GP1 0x8008A72C focused oracle\n");
    PsxMemory_Init();
    check("object-u32", (uintptr_t)(void *)s_object <= (uintptr_t)UINT32_MAX ? 1u : 0u, 1u);
    test_plus4_case2();
    test_plus4_case3();
    test_plus4_case6();
    test_bound_65();
    test_state0_idle_95414();
    test_state0_f8e5();
    test_90a84_ret1();
    test_state2_copy();
    test_state10_ff();
    test_always_one();
    test_negative_sra12();
    test_95414_nonzero_first();
    test_flag_skips_74794();
    if (s_failures != 0) {
        fprintf(stderr, "RESULT: FAIL (%d)\n", s_failures);
        return 1;
    }
    printf("W34-GP1 0x8008A72C focused oracle PASS\n");
    return 0;
}
