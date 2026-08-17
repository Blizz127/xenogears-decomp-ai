/*
 * Focused production-linked oracle for retail world callback 0x800914D0.
 *
 * Expected values are hand-derived from the retail disassembly of
 * [0x800914D0, 0x80091B54).  93354 / 93484 / 97770 are seam-forced.
 * NEXT_CALLBACK_TARGET is a runtime measurement, not an oracle input.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_callback_914d0.h"

#define POOL_GUEST  0x800D7538u
#define SLOT9       (POOL_GUEST + 0x480u)
#define SLOT8       (POOL_GUEST + 0x400u)
#define SLOT10      (POOL_GUEST + 0x500u)
#define DELTA       0x1F800010u

#define G_POOL      0x8009BE24u
#define G_CD4C      0x8009CD4Cu
#define G_BD3A      0x8009BD3Au
#define G_D52C      0x8009D52Cu
#define G_POSE      0x8009D55Cu
#define G_BBB4      0x8009BBB4u
#define G_BBBC      0x8009BBBCu
#define G_BE28      0x8009BE28u

static int s_failures;

static void chk(const char* n, u32 got, u32 want)
{
    if (got != want) {
        fprintf(stderr, "ASSERTION %s: got=0x%08x expected=0x%08x\n", n, got,
                want);
        if (++s_failures > 50)
            exit(1);
    }
}

static u32 rd32(u32 a)
{
    u32 v;

    memcpy(&v, PSX_ADDR(a), 4);
    return v;
}

static void wr32(u32 a, u32 v)
{
    memcpy(PSX_ADDR(a), &v, 4);
}

static u16 rd16(u32 a)
{
    u16 v;

    memcpy(&v, PSX_ADDR(a), 2);
    return v;
}

static void wr16(u32 a, u16 v)
{
    memcpy(PSX_ADDR(a), &v, 2);
}

typedef struct {
    char tag[8];
    u32 a[2];
} Call;

static Call s_calls[16];
static u32 s_ncalls;

static void log_call(const char* tag, u32 a0, u32 a1)
{
    if (s_ncalls < 16u) {
        snprintf(s_calls[s_ncalls].tag, sizeof(s_calls[s_ncalls].tag), "%s",
                 tag);
        s_calls[s_ncalls].a[0] = a0;
        s_calls[s_ncalls].a[1] = a1;
    }
    s_ncalls++;
}

static const Call* find_call(const char* tag, u32 nth)
{
    u32 i;
    u32 seen = 0;

    for (i = 0; i < s_ncalls && i < 16u; i++)
        if (strcmp(s_calls[i].tag, tag) == 0) {
            if (seen == nth)
                return &s_calls[i];
            seen++;
        }
    return 0;
}

void wm_914d0_test_store(u32 address, u32 width, u32 value)
{
    (void)address;
    (void)width;
    (void)value;
}

void wm_914d0_test_93354(u32 vec)
{
    log_call("93354", vec, 0u);
}

void wm_914d0_test_93484(u32 vec)
{
    log_call("93484", vec, 0u);
}

s32 wm_914d0_test_97770(u32 slot, s32 value)
{
    log_call("97770", slot, (u32)value);
    return 1;
}

static void reset(void)
{
    u32 i;

    s_ncalls = 0;
    wr32(G_POOL, POOL_GUEST);
    wr16(G_CD4C, 0u);
    wr16(G_BD3A, 0x40u);
    wr16(G_D52C, 0x40u);
    wr32(G_POSE, 0x1000u);
    wr32(G_POSE + 4u, 0x2000u);
    wr32(G_POSE + 8u, 0x3000u);
    wr32(G_POSE + 12u, 0x4000u);
    wr32(G_BBB4, 0x10u);
    wr32(G_BBBC, 0x20u);
    wr32(G_BE28, 0xDEAD0001u);
    wr32(G_BE28 + 4u, 0xDEAD0002u);
    wr32(G_BE28 + 8u, 0xDEAD0003u);
    wr32(G_BE28 + 12u, 0xDEAD0004u);
    for (i = 0; i < 0x80u; i += 4u) {
        wr32(SLOT8 + i, 0xA5A5A5A5u);
        wr32(SLOT9 + i, 0u);
        wr32(SLOT10 + i, 0x5A5A5A5Au);
    }
    wr32(SLOT9 + 0x28u, 0x1000u);
    wr32(SLOT9 + 0x2Cu, 0x2000u);
    wr32(SLOT9 + 0x30u, 0x3000u);
    wr32(SLOT9 + 0x34u, 0x4000u);
    wr32(SLOT9 + 0x50u, 0x40u);
    wr32(SLOT9 + 0x58u, 0x40u << 12);
    wr32(SLOT9 + 0x60u, 1u);
    wr32(DELTA, 0u);
    wr32(DELTA + 4u, 0u);
    wr32(DELTA + 8u, 0u);
}

static s32 run(void)
{
    return wm_800914D0(9);
}

static void chk_tail(const char* pfx, u32 n93354)
{
    const Call* c;
    char n[64];

    snprintf(n, sizeof(n), "%s.ret_via_tail", pfx);
    (void)n;
    chk("tail.n93354", s_ncalls >= n93354 ? 1u : 0u, 1u);
    c = find_call("93354", 0);
    chk("tail.93354.vec", c ? c->a[0] : 0u, SLOT9 + 0x28u);
    chk("tail.be28x", rd32(G_BE28), rd32(SLOT9 + 0x28u));
    chk("tail.be28y", rd32(G_BE28 + 4u), rd32(SLOT9 + 0x2Cu));
    chk("tail.be28z", rd32(G_BE28 + 8u), rd32(SLOT9 + 0x30u));
    chk("tail.be28w", rd32(G_BE28 + 12u), rd32(SLOT9 + 0x34u));
}

int main(void)
{
    s32 r;
    const Call* c;

    PsxMemory_Init();

    /* ---- idle: pose equal, heading equal, state 0, pad 0 ---- */
    reset();
    r = run();
    chk("idle.ret", (u32)r, 1u);
    chk("idle.state", rd16(SLOT9 + 0x20u), 0u);
    chk("idle.n97770", find_call("97770", 0) ? 1u : 0u, 0u);
    chk("idle.n93484", find_call("93484", 0) ? 1u : 0u, 0u);
    chk("idle.n93354", find_call("93354", 0) ? 1u : 0u, 1u);
    chk("idle.bbb4", rd32(G_BBB4), 0x10u);
    chk("idle.bbbc", rd32(G_BBBC), 0x20u);
    chk("idle.slot8", rd32(SLOT8), 0xA5A5A5A5u);
    chk("idle.slot10", rd32(SLOT10), 0x5A5A5A5Au);
    chk_tail("idle", 1u);

    /* ---- sub 9: state 2, +50=D52C, +58=BD3A<<12; +60=1 blocks 97770 ---- */
    reset();
    wr16(SLOT9 + 4u, 9u);
    wr16(SLOT9 + 0x20u, 0u);
    wr16(G_D52C, 0x40u);
    wr16(G_BD3A, 0x40u);
    r = run();
    chk("sub9.ret", (u32)r, 1u);
    chk("sub9.sub", rd16(SLOT9 + 4u), 0u);
    chk("sub9.state", rd16(SLOT9 + 0x20u), 2u);
    chk("sub9.50", rd32(SLOT9 + 0x50u), 0x40u);
    chk("sub9.58", rd32(SLOT9 + 0x58u), 0x40u << 12);
    chk("sub9.n97770", find_call("97770", 0) ? 1u : 0u, 0u);

    /* ---- sub 0xA: state 0, +50=BD3A ---- */
    reset();
    wr16(SLOT9 + 4u, 0xAu);
    wr16(SLOT9 + 0x20u, 2u);
    wr16(G_BD3A, 0x55u);
    r = run();
    chk("suba.state", rd16(SLOT9 + 0x20u), 0u);
    chk("suba.50", rd32(SLOT9 + 0x50u), 0x55u);
    chk("suba.58", rd32(SLOT9 + 0x58u), 0x55u << 12);

    /* ---- sub 0xF / 0x10 / 0x11 set state 0x10 and fixed headings ---- */
    reset();
    wr16(SLOT9 + 4u, 0xFu);
    wr16(G_BD3A, 0x800u);
    wr32(SLOT9 + 0x50u, 0x800u);
    wr32(SLOT9 + 0x58u, 0x800u << 12);
    r = run();
    chk("subf.state", rd16(SLOT9 + 0x20u), 0x10u);
    chk("subf.50", rd32(SLOT9 + 0x50u), 0x800u);

    reset();
    wr16(SLOT9 + 4u, 0x10u);
    wr16(G_BD3A, 0xA00u);
    wr32(SLOT9 + 0x50u, 0xA00u);
    wr32(SLOT9 + 0x58u, 0xA00u << 12);
    r = run();
    chk("sub10.50", rd32(SLOT9 + 0x50u), 0xA00u);

    reset();
    wr16(SLOT9 + 4u, 0x11u);
    wr16(G_BD3A, 0xC00u);
    wr32(SLOT9 + 0x50u, 0xC00u);
    wr32(SLOT9 + 0x58u, 0xC00u << 12);
    r = run();
    chk("sub11.50", rd32(SLOT9 + 0x50u), 0xC00u);

    /* ---- state 0 pad bits 2-3 of CD4C ---- */
    reset();
    wr16(SLOT9 + 0x20u, 0u);
    wr32(SLOT9 + 0x50u, 0x400u);
    wr16(G_BD3A, 0x400u);
    wr16(G_CD4C, (u16)(1u << 2)); /* bits 2-3 == 1 */
    r = run();
    chk("pad1.state", rd16(SLOT9 + 0x20u), 1u);
    chk("pad1.54", rd32(SLOT9 + 0x54u), 0xFFFFFFC0u);
    chk("pad1.5c", rd32(SLOT9 + 0x5Cu), (0x400u - 0x200u) & 0xFFFu);

    reset();
    wr16(SLOT9 + 0x20u, 0u);
    wr32(SLOT9 + 0x50u, 0x400u);
    wr16(G_BD3A, 0x400u);
    wr16(G_CD4C, (u16)(2u << 2));
    r = run();
    chk("pad2.state", rd16(SLOT9 + 0x20u), 1u);
    chk("pad2.54", rd32(SLOT9 + 0x54u), 0x40u);
    chk("pad2.5c", rd32(SLOT9 + 0x5Cu), (0x400u + 0x200u) & 0xFFFu);

    reset();
    wr16(SLOT9 + 0x20u, 0u);
    wr32(SLOT9 + 0x50u, 0x400u);
    wr16(G_BD3A, 0x400u);
    wr16(G_CD4C, (u16)(3u << 2));
    r = run();
    chk("pad3.54", rd32(SLOT9 + 0x54u), 0xFFFFFFC0u);
    chk("pad3.5c", rd32(SLOT9 + 0x5Cu), (0x400u - 0x200u) & 0xFFFu);

    /* ---- state 0 heading sync: +50=0x100, BD3A=0, acc += 0x20000 ---- */
    reset();
    wr16(SLOT9 + 0x20u, 0u);
    wr32(SLOT9 + 0x50u, 0x100u);
    wr32(SLOT9 + 0x58u, 0u);
    wr16(G_BD3A, 0u);
    r = run();
    chk("sync.58", rd32(SLOT9 + 0x58u), 0x20000u);
    chk("sync.bd3a", rd16(G_BD3A), 0x20u);

    /* ---- state 0 0xC01 decision: +50=0xA00 is below wrap, above 0x801 ---- */
    reset();
    wr16(SLOT9 + 0x20u, 0u);
    wr32(SLOT9 + 0x50u, 0xA00u);
    wr32(SLOT9 + 0x58u, 0u);
    wr16(G_BD3A, 0u);
    r = run();
    chk("wrap.58", rd32(SLOT9 + 0x58u), 0x140000u);
    chk("wrap.bd3a", rd16(G_BD3A), 0x140u);

    /* ---- state 0 wrap path: +50=0xD00, abs>=0xC01, delta -= 0x1000 ---- */
    reset();
    wr16(SLOT9 + 0x20u, 0u);
    wr32(SLOT9 + 0x50u, 0xD00u);
    wr32(SLOT9 + 0x58u, 0u);
    wr16(G_BD3A, 0u);
    r = run();
    chk("wraphi.58", rd32(SLOT9 + 0x58u), 0x00FA0000u);
    chk("wraphi.bd3a", rd16(G_BD3A), 0xFA0u);

    /* ---- state 1 interpolates +50 toward +5C ---- */
    reset();
    wr16(SLOT9 + 0x20u, 1u);
    wr32(SLOT9 + 0x50u, 0x400u);
    wr32(SLOT9 + 0x54u, 0x40u);
    wr32(SLOT9 + 0x5Cu, 0x600u);
    wr16(G_BD3A, 0x400u);
    wr32(SLOT9 + 0x58u, 0x400u << 12);
    r = run();
    chk("s1.50", rd32(SLOT9 + 0x50u), 0x440u);
    chk("s1.state", rd16(SLOT9 + 0x20u), 1u);

    reset();
    wr16(SLOT9 + 0x20u, 1u);
    wr32(SLOT9 + 0x50u, 0x5C0u);
    wr32(SLOT9 + 0x54u, 0x40u);
    wr32(SLOT9 + 0x5Cu, 0x600u);
    wr16(G_BD3A, 0x600u);
    wr32(SLOT9 + 0x58u, 0x600u << 12);
    r = run();
    chk("s1b.50", rd32(SLOT9 + 0x50u), 0x600u);
    chk("s1b.state", rd16(SLOT9 + 0x20u), 0u);

    /* ---- state 2 chase clamps to 0x180, sra 3; +60!=0 blocks 97770 ---- */
    reset();
    wr16(SLOT9 + 0x20u, 2u);
    wr32(SLOT9 + 0x50u, 0x200u);
    wr32(SLOT9 + 0x58u, 0u);
    wr16(G_BD3A, 0u);
    wr32(SLOT9 + 0x60u, 1u);
    r = run();
    chk("s2.58", rd32(SLOT9 + 0x58u), 0x30000u);
    chk("s2.bd3a", rd16(G_BD3A), 0x30u);
    chk("s2.state", rd16(SLOT9 + 0x20u), 2u);
    chk("s2.n97770", find_call("97770", 0) ? 1u : 0u, 0u);

    /* ---- state 2 arrival: 97770(7, 0xB) then snap ---- */
    reset();
    wr16(SLOT9 + 0x20u, 2u);
    wr32(SLOT9 + 0x50u, 0u);
    wr32(SLOT9 + 0x58u, 0u);
    wr16(G_BD3A, 0u);
    wr32(SLOT9 + 0x60u, 0u);
    wr32(SLOT9 + 0x28u, 0x1000u);
    wr32(SLOT9 + 0x2Cu, 0x2000u);
    wr32(SLOT9 + 0x30u, 0x3000u);
    wr32(G_POSE, 0x1100u);
    wr32(G_POSE + 4u, 0x2080u);
    wr32(G_POSE + 8u, 0x3100u);
    r = run();
    chk("s2arr.state", rd16(SLOT9 + 0x20u), 3u);
    c = find_call("97770", 0);
    chk("s2arr.97770.slot", c ? c->a[0] : 0u, 7u);
    chk("s2arr.97770.val", c ? c->a[1] : 0u, 0xBu);
    c = find_call("93484", 0);
    chk("s2arr.93484.vec", c ? c->a[0] : 0u, DELTA);
    chk("s2arr.x", rd32(SLOT9 + 0x28u), 0x1100u);
    chk("s2arr.z", rd32(SLOT9 + 0x30u), 0x3100u);
    /* Y += dy>>4 = 0x80>>4 = 8 */
    chk("s2arr.y", rd32(SLOT9 + 0x2Cu), 0x2000u + 8u);
    chk("s2arr.bbb4", rd32(G_BBB4), 0x10u + 0x100u);
    chk("s2arr.bbbc", rd32(G_BBBC), 0x20u + 0x100u);

    /* ---- state 0x10 uses sra 5; arrival does not call 97770 ---- */
    reset();
    wr16(SLOT9 + 0x20u, 0x10u);
    wr32(SLOT9 + 0x50u, 0x200u);
    wr32(SLOT9 + 0x58u, 0u);
    wr16(G_BD3A, 0u);
    wr32(SLOT9 + 0x60u, 1u);
    r = run();
    chk("s10.58", rd32(SLOT9 + 0x58u), 0xC000u);
    chk("s10.bd3a", rd16(G_BD3A), 0xCu);
    chk("s10.n97770", find_call("97770", 0) ? 1u : 0u, 0u);

    reset();
    wr16(SLOT9 + 0x20u, 0x10u);
    wr32(SLOT9 + 0x50u, 0u);
    wr32(SLOT9 + 0x58u, 0u);
    wr16(G_BD3A, 0u);
    wr32(SLOT9 + 0x60u, 0u);
    r = run();
    chk("s10arr.state", rd16(SLOT9 + 0x20u), 3u);
    chk("s10arr.n97770", find_call("97770", 0) ? 1u : 0u, 0u);

    /* ---- approach far: step >>3, +60=1 ---- */
    reset();
    wr16(SLOT9 + 0x20u, 0u);
    wr32(SLOT9 + 0x28u, 0u);
    wr32(SLOT9 + 0x2Cu, 0u);
    wr32(SLOT9 + 0x30u, 0u);
    wr32(G_POSE, 0x800u);
    wr32(G_POSE + 4u, 0x400u);
    wr32(G_POSE + 8u, 0x800u);
    r = run();
    chk("far.60", rd32(SLOT9 + 0x60u), 1u);
    chk("far.x", rd32(SLOT9 + 0x28u), 0x100u);
    chk("far.y", rd32(SLOT9 + 0x2Cu), 0x80u);
    chk("far.z", rd32(SLOT9 + 0x30u), 0x100u);
    chk("far.bbb4", rd32(G_BBB4), 0x10u + 0x100u);
    chk("far.bbbc", rd32(G_BBBC), 0x20u + 0x100u);
    c = find_call("93484", 0);
    chk("far.93484", c ? c->a[0] : 0u, DELTA);

    /* ---- approach close: abs(step)<0x40 snaps X/Z, Y still += step ---- */
    reset();
    wr16(SLOT9 + 0x20u, 0u);
    wr32(SLOT9 + 0x28u, 0x1000u);
    wr32(SLOT9 + 0x2Cu, 0x2000u);
    wr32(SLOT9 + 0x30u, 0x3000u);
    wr32(G_POSE, 0x1100u);
    wr32(G_POSE + 4u, 0x2080u);
    wr32(G_POSE + 8u, 0x3100u);
    r = run();
    chk("close.60", rd32(SLOT9 + 0x60u), 0u);
    chk("close.x", rd32(SLOT9 + 0x28u), 0x1100u);
    chk("close.z", rd32(SLOT9 + 0x30u), 0x3100u);
    chk("close.y", rd32(SLOT9 + 0x2Cu), 0x2000u + 0x10u);
    chk("close.bbb4", rd32(G_BBB4), 0x10u + 0x100u);
    chk("close.bbbc", rd32(G_BBBC), 0x20u + 0x100u);

    /* ---- parked state 4: wrap+publish only ---- */
    reset();
    wr16(SLOT9 + 0x20u, 4u);
    wr32(SLOT9 + 0x28u, 0x111u);
    wr32(G_POSE, 0x999u);
    wr32(G_BBB4, 0xABCDu);
    r = run();
    chk("park.state", rd16(SLOT9 + 0x20u), 4u);
    chk("park.x", rd32(SLOT9 + 0x28u), 0x111u);
    chk("park.bbb4", rd32(G_BBB4), 0xABCDu);
    chk("park.n93484", find_call("93484", 0) ? 1u : 0u, 0u);
    chk("park.n97770", find_call("97770", 0) ? 1u : 0u, 0u);
    chk("park.n93354", find_call("93354", 0) ? 1u : 0u, 1u);

    /* ---- JT1 OOR state 0x11 skips chase ---- */
    reset();
    wr16(SLOT9 + 0x20u, 0x11u);
    wr32(SLOT9 + 0x50u, 0x200u);
    wr32(SLOT9 + 0x58u, 0u);
    wr16(G_BD3A, 0u);
    r = run();
    chk("oor.58", rd32(SLOT9 + 0x58u), 0u);
    chk("oor.bd3a", rd16(G_BD3A), 0u);
    chk("oor.state", rd16(SLOT9 + 0x20u), 0x11u);

    /* ---- noninterference canaries ---- */
    reset();
    wr32(SLOT8 + 0x20u, 0x11223344u);
    wr32(SLOT10 + 0x28u, 0x55667788u);
    wr16(G_D52C, 0x1234u);
    wr16(G_CD4C, 0u);
    r = run();
    chk("ni.slot8", rd32(SLOT8 + 0x20u), 0x11223344u);
    chk("ni.slot10", rd32(SLOT10 + 0x28u), 0x55667788u);
    chk("ni.d52c", rd16(G_D52C), 0x1234u);
    chk("ni.cd4c", rd16(G_CD4C), 0u);
    chk("ni.pose", rd32(G_POSE), 0x1000u);

    if (s_failures != 0) {
        fprintf(stderr, "%d assertion(s) failed\n", s_failures);
        return 1;
    }
    printf("W34B24-I14 0x800914D0 focused oracle PASS\n");
    return 0;
}
