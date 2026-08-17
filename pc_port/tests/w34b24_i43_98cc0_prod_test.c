/*
 * Focused production-linked oracle for retail helper 0x80098CC0.
 *
 * Expected values are hand-derived from [0x80098CC0, 0x8009932C).
 * Overlay callees and SLUS residents are recording stubs.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_98cc0.h"

#define PTRS    0x8009C184u
#define KEEP    0x8009D570u
#define CHECK   0x8009D318u
#define BBAC    0x8009BBACu
#define ARC_A   0x8009BCD8u
#define ARC_B   0x8009BD08u
#define DIVISOR 0x8009D160u
#define MULT    0x8009D2B4u
#define CANARY  0x8009B000u
#define JALR    0x8009CD40u
#define COUNTER 0x8009C5BCu
#define LIMIT   0x51
#define SIZE    0x710u

typedef struct {
    u32 a0;
    u32 a1;
    u32 a2;
} Call3;

typedef struct {
    u32 a0;
    u32 a1;
    u32 a2;
    u32 a3;
} Call4;

static int s_failures;
static u32 s_dbg[2];
static int s_dbg_i;
static int s_dec_n;
static int s_path_n;
static int s_alloc_n;
static int s_free_n;
static int s_c3_n;
static int s_b0_n;
static int s_pub12;
static int s_pub16;
static u32 s_dec_in[8];
static u32 s_path_in[8];
static u32 s_alloc_sz[32];
static u32 s_alloc_fl[32];
static u32 s_alloc_psx[32];
static u32 s_free_psx[8];
static Call3 s_c3[32];
static Call4 s_b0[32];
static u32 s_heap_next;

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

static void wr32(u32 a, u32 v) { memcpy(PSX_ADDR(a), &v, 4); }
static void wr16(u32 a, u16 v) { memcpy(PSX_ADDR(a), &v, 2); }

static u32 guest_of(const void *p)
{
    return 0x80000000u | (u32)((uintptr_t)p - (uintptr_t)g_PsxRam);
}

u32 func_8002C3D8(void)
{
    u32 v = s_dbg[s_dbg_i];

    if (s_dbg_i < 1)
        s_dbg_i += 1;
    return v;
}

int ArchiveDecodeSector(int entryIndex)
{
    s_dec_in[s_dec_n] = (u32)entryIndex;
    if (s_dec_n < 7)
        s_dec_n += 1;
    return 1000 + entryIndex;
}

char *ArchiveGetFilePath(int entryIndex)
{
    s_path_in[s_path_n] = (u32)entryIndex;
    if (s_path_n < 7)
        s_path_n += 1;
    return (char *)(uintptr_t)(0x80070000u + (u32)entryIndex * 4u);
}

void *HeapAlloc(u_int allocSize, u_int allocFlags)
{
    u32 kuseg;

    kuseg = s_heap_next;
    s_heap_next += 0x800u;
    if (s_alloc_n < 32) {
        s_alloc_sz[s_alloc_n] = (u32)allocSize;
        s_alloc_fl[s_alloc_n] = (u32)allocFlags;
        s_alloc_psx[s_alloc_n] = kuseg;
        s_alloc_n += 1;
    }
    return PSX_ADDR(kuseg);
}

u_int HeapFree(void *pMem)
{
    if (s_free_n < 8)
        s_free_psx[s_free_n++] = guest_of(pMem);
    return 0u;
}

s32 wm_8009623C(u32 a0, u32 a1, u32 a2)
{
    if (s_c3_n < 32) {
        s_c3[s_c3_n].a0 = a0;
        s_c3[s_c3_n].a1 = a1;
        s_c3[s_c3_n].a2 = a2;
        s_c3_n += 1;
    }
    return 0;
}

s32 wm_800962B0(u32 a0, u32 a1, u32 a2, u32 a3)
{
    if (s_b0_n < 32) {
        s_b0[s_b0_n].a0 = a0;
        s_b0[s_b0_n].a1 = a1;
        s_b0[s_b0_n].a2 = a2;
        s_b0[s_b0_n].a3 = a3;
        s_b0_n += 1;
    }
    return 0;
}

s32 wm_80096328(void)
{
    s_pub12 += 1;
    return 0;
}

s32 wm_800965A4(void)
{
    s_pub16 += 1;
    return 0;
}

void wm_98cc0_test_store(u32 address, u32 value)
{
    (void)address;
    (void)value;
}

static void reset_calls(void)
{
    s_dbg_i = 0;
    s_dec_n = 0;
    s_path_n = 0;
    s_alloc_n = 0;
    s_free_n = 0;
    s_c3_n = 0;
    s_b0_n = 0;
    s_pub12 = 0;
    s_pub16 = 0;
    s_heap_next = 0x80100000u;
    memset(s_dec_in, 0, sizeof(s_dec_in));
    memset(s_path_in, 0, sizeof(s_path_in));
    memset(s_c3, 0, sizeof(s_c3));
    memset(s_b0, 0, sizeof(s_b0));
}

static void fill_keep_check(void)
{
    s32 i;

    for (i = 0; i < LIMIT; i++) {
        wr16(KEEP + (u32)i * 2u, (u16)i);
        wr16(CHECK + (u32)i * 2u, (u16)i);
        wr32(PTRS + (u32)i * 4u, 0x80090000u + (u32)i * 4u);
    }
    wr16(BBAC + 0u, 0);
    wr16(BBAC + 2u, 1);
    wr16(BBAC + 4u, 2);
    wr16(BBAC + 6u, 3);
    wr32(ARC_A, 5u);
    wr32(ARC_B, 7u);
    wr32(DIVISOR, 5u);
    wr32(MULT, 3u);
    wr32(CANARY, 0x11111111u);
    wr32(JALR, 0x22222222u);
    wr32(COUNTER, 0x33333333u);
}

static void expect_canaries(void)
{
    chk("canary", rd32(CANARY), 0x11111111u);
    chk("jalr", rd32(JALR), 0x22222222u);
    chk("counter", rd32(COUNTER), 0x33333333u);
}

static u32 cd_off(s32 id)
{
    return (u32)(id % 5) * 3u + (u32)(id / 5);
}

static u32 pc_off(s32 id)
{
    return ((u32)(id % 5) << 11) * 3u + ((u32)(id / 5) << 11);
}

int main(void)
{
    PsxMemory_Init();

    /* CD occupied: decode both archives, terminator only. */
    fill_keep_check();
    s_dbg[0] = 0;
    s_dbg[1] = 0;
    reset_calls();
    wm_80098CC0();
    chk("occ.dec", (u32)s_dec_n, 2u);
    chk("occ.dec0", s_dec_in[0], 5u);
    chk("occ.dec1", s_dec_in[1], 7u);
    chk("occ.alloc", (u32)s_alloc_n, 0u);
    chk("occ.c3", (u32)s_c3_n, 1u);
    chk("occ.c3.a0", s_c3[0].a0, 0u);
    chk("occ.c3.a1", s_c3[0].a1, 0u);
    chk("occ.c3.a2", s_c3[0].a2, 0u);
    chk("occ.pub12", (u32)s_pub12, 1u);
    chk("occ.pub16", (u32)s_pub16, 0u);
    chk("occ.b0", (u32)s_b0_n, 0u);
    expect_canaries();

    /* Cleanup frees an ID that is not in the keep list. */
    fill_keep_check();
    wr16(CHECK + 80u * 2u, 99);
    wr32(PTRS + 99u * 4u, 0x801F0000u);
    s_dbg[0] = 0;
    s_dbg[1] = 0;
    reset_calls();
    wm_80098CC0();
    chk("free.n", (u32)s_free_n, 1u);
    chk("free.psx", s_free_psx[0], 0x801F0000u);
    chk("free.slot", rd32(PTRS + 99u * 4u), 0u);
    chk("free.keep3", rd32(PTRS + 3u * 4u), 0x80090000u + 12u);

    /* CD group1 alloc of keep[3]=3. */
    fill_keep_check();
    wr32(PTRS + 3u * 4u, 0u);
    s_dbg[0] = 0;
    s_dbg[1] = 1;
    reset_calls();
    wm_80098CC0();
    chk("g1.alloc", (u32)s_alloc_n, 1u);
    chk("g1.sz", s_alloc_sz[0], SIZE);
    chk("g1.fl", s_alloc_fl[0], 0u);
    chk("g1.slot", rd32(PTRS + 3u * 4u), 0x80100000u);
    chk("g1.c3n", (u32)s_c3_n, 2u);
    chk("g1.c3.a0", s_c3[0].a0, 1005u + 3u);
    chk("g1.c3.a1", s_c3[0].a1, SIZE);
    chk("g1.c3.a2", s_c3[0].a2, 0x80100000u);
    chk("g1.term", s_c3[1].a0, 0u);
    chk("g1.pub12", (u32)s_pub12, 1u);

    /* CD group2 alloc of keep[27]=27 (stride 0x12). */
    fill_keep_check();
    wr32(PTRS + 27u * 4u, 0u);
    s_dbg[0] = 0;
    s_dbg[1] = 0xFFFFFFFFu;
    reset_calls();
    wm_80098CC0();
    chk("g2.alloc", (u32)s_alloc_n, 1u);
    chk("g2.c3n", (u32)s_c3_n, 2u);
    chk("g2.c3.a0", s_c3[0].a0, 1007u + cd_off(27));
    chk("g2.c3.a1", s_c3[0].a1, SIZE);
    chk("g2.slot", rd32(PTRS + 27u * 4u), 0x80100000u);

    /* CD third with flag2: empty id 0, group1 also empty. */
    fill_keep_check();
    wr32(PTRS + 3u * 4u, 0u);
    wr32(PTRS + 0u * 4u, 0u);
    s_dbg[0] = 0;
    s_dbg[1] = 0;
    reset_calls();
    wm_80098CC0();
    chk("t2.alloc", (u32)s_alloc_n, 2u);
    chk("t2.c3n", (u32)s_c3_n, 3u);
    chk("t2.third.a0", s_c3[1].a0, 1005u + 0u);
    chk("t2.third.a1", s_c3[1].a1, SIZE);
    chk("t2.decn", (u32)s_dec_n, 3u);

    /* CD third with flag1 only. */
    fill_keep_check();
    wr32(PTRS + 27u * 4u, 0u);
    wr32(PTRS + 0u * 4u, 0u);
    s_dbg[0] = 0;
    s_dbg[1] = 0;
    reset_calls();
    wm_80098CC0();
    chk("t1.alloc", (u32)s_alloc_n, 2u);
    chk("t1.c3n", (u32)s_c3_n, 3u);
    chk("t1.third.a0", s_c3[1].a0, 1007u + cd_off(0));

    /* CD third with no flags still allocates. */
    fill_keep_check();
    wr32(PTRS + 0u * 4u, 0u);
    s_dbg[0] = 0;
    s_dbg[1] = 0;
    reset_calls();
    wm_80098CC0();
    chk("t0.alloc", (u32)s_alloc_n, 1u);
    chk("t0.slot", rd32(PTRS + 0u * 4u), 0x80100000u);
    chk("t0.c3n", (u32)s_c3_n, 1u);
    chk("t0.term", s_c3[0].a0, 0u);

    /* PC occupied + group1 alloc. */
    fill_keep_check();
    wr32(PTRS + 3u * 4u, 0u);
    s_dbg[0] = 5;
    s_dbg[1] = 5;
    reset_calls();
    wm_80098CC0();
    chk("pc.dec", (u32)s_dec_n, 0u);
    chk("pc.pathn", (u32)s_path_n, 2u);
    chk("pc.path0", s_path_in[0], 5u);
    chk("pc.path1", s_path_in[1], 7u);
    chk("pc.b0n", (u32)s_b0_n, 2u);
    chk("pc.b0.a0", s_b0[0].a0, 0x80070000u + 20u);
    chk("pc.b0.a1", s_b0[0].a1, 3u << 11);
    chk("pc.b0.a2", s_b0[0].a2, SIZE);
    chk("pc.b0.a3", s_b0[0].a3, 0x80100000u);
    chk("pc.term.a0", s_b0[1].a0, 0u);
    chk("pc.pub16", (u32)s_pub16, 1u);
    chk("pc.pub12", (u32)s_pub12, 0u);
    chk("pc.c3", (u32)s_c3_n, 0u);

    /* PC group2 + third flag1. */
    fill_keep_check();
    wr32(PTRS + 27u * 4u, 0u);
    wr32(PTRS + 0u * 4u, 0u);
    s_dbg[0] = 5;
    s_dbg[1] = 5;
    reset_calls();
    wm_80098CC0();
    chk("pc2.b0n", (u32)s_b0_n, 3u);
    chk("pc2.g2.a0", s_b0[0].a0, 0x80070000u + 28u);
    chk("pc2.g2.a1", s_b0[0].a1, pc_off(27));
    chk("pc2.th.a0", s_b0[1].a0, 0x80070000u + 28u);
    chk("pc2.th.a1", s_b0[1].a1, pc_off(0));
    chk("pc2.term", s_b0[2].a0, 0u);

    expect_canaries();

    if (s_failures) {
        fprintf(stderr, "ASSERTION failures=%d\n", s_failures);
        return 1;
    }
    printf("W34B24-I43 0x80098CC0 focused oracle PASS\n");
    return 0;
}
