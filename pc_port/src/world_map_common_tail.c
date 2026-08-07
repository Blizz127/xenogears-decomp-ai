/*
 * World-map common-tail prefix + 0x80089160 initializer (W34B5A).
 *
 * Common-tail prefix: 0x8007290C–0x80072938.
 * Selector-dependent logic: C610=0 calls wm_80089160(14,0,0).
 * Reconvergence at 0x8007293C (first excluded = next-phase helper).
 *
 * 0x80089160: bounded table/state initializer 0x80089160–0x800893D4.
 * Leaf function, no direct calls. Record stride 672 bytes.
 * Sets bit 0x80 at record+0x4F, initializes 8 sub-records (stride 0x54).
 */
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_common_tail.h"

#define WM_U8(a)  (*(u8*)PSX_ADDR(a))
#define WM_U16(a) (*(u16*)PSX_ADDR(a))
#define WM_S16(a) (*(s16*)PSX_ADDR(a))
#define WM_U32(a) (*(u32*)PSX_ADDR(a))

/* PsyQ GPU helpers (linked from PsyCross in the full port). */
extern u_short GetTPage(int tp, int abr, int x, int y);
extern u_short GetClut(int x, int y);

/* ---- PSX unaligned load/store helpers (little-endian) ----
 *
 * Exact byte-level emulation of MIPS lwl/lwr/swl/swr for all alignments.
 * The retail binary uses these for 8-byte copies in the record init loop. */

static inline u32 psx_lwl(u32 addr)
{
    /* lwl loads bytes from aligned base up to addr into the HIGH portion
     * of rt.  MIPS R3000A little-endian: loaded bytes occupy the high
     * register positions in NORMAL order (lowest address → lowest
     * replaced position).  Combined with lwr(addr) via OR, the pair
     * produces the correct little-endian word. */
    unsigned off = addr & 3;
    u8 *base = (u8*)PSX_ADDR(addr - off);
    u32 result = 0;
    for (unsigned j = 0; j <= off; j++)
        result |= (u32)base[j] << ((3 - off + j) * 8);
    return result;
}

static inline u32 psx_lwr(u32 addr)
{
    /* lwr loads bytes from addr through end of aligned word.
     * MIPS R3000A little-endian: loaded bytes occupy the LOW register
     * positions starting at 0.  When paired with lwl(addr+3), the OR
     * merge produces the correct full word with no bit conflicts. */
    unsigned off = addr & 3;
    u8 *base = (u8*)PSX_ADDR(addr - off);
    u32 result = 0;
    for (unsigned j = off; j < 4; j++)
        result |= (u32)base[j] << ((j - off) * 8);
    return result;
}

static inline u32 psx_lwl_lwr(u32 lwl_addr, u32 lwr_addr)
{
    return psx_lwl(lwl_addr) | psx_lwr(lwr_addr);
}

static inline void psx_swl(u32 addr, u32 val)
{
    /* swl stores the HIGH bytes of val into memory from the aligned
     * boundary up to addr.  MIPS R3000A little-endian: stores in the
     * same normal order as lwl loads.  swl(addr+3)|swr(addr) stores
     * the full word in correct little-endian memory order. */
    unsigned off = addr & 3;
    u8 *base = (u8*)PSX_ADDR(addr - off);
    for (unsigned j = 0; j <= off; j++)
        base[j] = (u8)(val >> ((3 - off + j) * 8));
}

static inline void psx_swr(u32 addr, u32 val)
{
    /* swr stores bytes from addr through end of aligned word.
     * MIPS R3000A little-endian: val byte (j-off) goes to position j.
     * swl(addr+3)|swr(addr) stores the full word in memory-byte order. */
    unsigned off = addr & 3;
    u8 *base = (u8*)PSX_ADDR(addr - off);
    for (unsigned j = off; j < 4; j++)
        base[j] = (u8)(val >> ((j - off) * 8));
}

/* C610 selector address (same as in convergence module). */
#define WM_SLOT_C610_ABS  0x8009C610u

/* ---- 0x80089160 instrumentation ---- */

static int s_wm_89160_calls;
static int s_wm_89160_iterations;

int  wm_89160_get_calls(void)      { return s_wm_89160_calls; }
int  wm_89160_get_iterations(void) { return s_wm_89160_iterations; }
void wm_89160_reset(void)          { s_wm_89160_calls = 0; s_wm_89160_iterations = 0; }

/* ---- Common-tail P0 instrumentation ---- */

static int s_wm_ctp0_entry;
static int s_wm_ctp0_c610_zero;
static int s_wm_ctp0_c610_nonzero;
static int s_wm_ctp0_89160_calls;
static u32 s_wm_ctp0_last_cut;
static int s_wm_ctp0_forbidden_978fc;
static int s_wm_ctp0_forbidden_scheduler;
static int s_wm_ctp0_forbidden_world_loop;
static int s_wm_ctp0_forbidden_8901c;
static int s_wm_ctp0_forbidden_865a0;
static int s_wm_ctp0_forbidden_85fe0;
static int s_wm_ctp0_forbidden_75228;

int  wm_ctp0_get_entry(void)              { return s_wm_ctp0_entry; }
int  wm_ctp0_get_c610_zero(void)          { return s_wm_ctp0_c610_zero; }
int  wm_ctp0_get_c610_nonzero(void)       { return s_wm_ctp0_c610_nonzero; }
int  wm_ctp0_get_89160_calls(void)        { return s_wm_ctp0_89160_calls; }
u32  wm_ctp0_get_last_cut(void)           { return s_wm_ctp0_last_cut; }
int  wm_ctp0_get_forbidden_978fc(void)    { return s_wm_ctp0_forbidden_978fc; }
int  wm_ctp0_get_forbidden_scheduler(void){ return s_wm_ctp0_forbidden_scheduler; }
int  wm_ctp0_get_forbidden_world_loop(void){ return s_wm_ctp0_forbidden_world_loop; }
int  wm_ctp0_get_forbidden_8901c(void)    { return s_wm_ctp0_forbidden_8901c; }
int  wm_ctp0_get_forbidden_865a0(void)    { return s_wm_ctp0_forbidden_865a0; }
int  wm_ctp0_get_forbidden_85fe0(void)    { return s_wm_ctp0_forbidden_85fe0; }
int  wm_ctp0_get_forbidden_75228(void)    { return s_wm_ctp0_forbidden_75228; }

void wm_common_tail_p0_reset(void)
{
    s_wm_ctp0_entry = 0;
    s_wm_ctp0_c610_zero = 0;
    s_wm_ctp0_c610_nonzero = 0;
    s_wm_ctp0_89160_calls = 0;
    s_wm_ctp0_last_cut = 0;
    s_wm_ctp0_forbidden_978fc = 0;
    s_wm_ctp0_forbidden_scheduler = 0;
    s_wm_ctp0_forbidden_world_loop = 0;
    s_wm_ctp0_forbidden_8901c = 0;
    s_wm_ctp0_forbidden_865a0 = 0;
    s_wm_ctp0_forbidden_85fe0 = 0;
    s_wm_ctp0_forbidden_75228 = 0;
    wm_89160_reset();
}

/* ---- Forbidden-path stubs ---- */

void wm_800978fc_should_not_run(void)  { s_wm_ctp0_forbidden_978fc++; }
void wm_80097800_should_not_run(void)  { s_wm_ctp0_forbidden_scheduler++; }
void wm_p0_800712D0_should_not_run(void) { s_wm_ctp0_forbidden_world_loop++; }
void wm_8008901c_should_not_run(void)  { s_wm_ctp0_forbidden_8901c++; }
void wm_800865a0_should_not_run(void)  { s_wm_ctp0_forbidden_865a0++; }
void wm_80085fe0_should_not_run(void)  { s_wm_ctp0_forbidden_85fe0++; }
void wm_80075228_should_not_run(void)  { s_wm_ctp0_forbidden_75228++; }

/* ---- 0x80089160 production implementation ---- */

/* Exact native transcription of retail 0x80089160–0x800893D8.
 *
 * Leaf function: no direct calls, no stack frame.
 * Record stride: 672 bytes (0x2A0).
 * Table base: *(0x8009BCC0).
 * Inner loop: 8 sub-records, stride 0x54.
 * Flag: bit 0x80 at record+0x4F.
 *
 * Four dispatch paths based on a0/a1/a2:
 *   Path A: t6 & v1 != 0 → record+0x18 loop, aligned stores
 *   Path B: (0<a1) & v1 != 0 → record+0x20 loop, lwl/lwr+swl/swr
 *   Path C: t6 & (0<a2) != 0 → record+0x20 loop, lhu+negu+sh
 *   Path D: none of above → record+0x20 loop, lwl/lwr+swl/swr+lhu+negu+sh
 *
 * Natural call (14,0,0): Path A (t6=1, v1=1, and=1≠0).
 *
 * Dirty-path parity: when Phase 1 finds an existing 0x80 flag, retail
 * sets t2=1 and CONTINUES (never returns early).  In ALL four paths,
 * t2!=0 skips only the flag OR/store block but ALWAYS performs the
 * remaining stores. */
void wm_80089160(u32 a0, u32 a1, u32 a2)
{
    u32 table_base;
    u32 record_addr;
    u32 flag_byte;
    u32 a0_ptr;     /* record-derived pointer for each path */
    u32 a3;         /* record + 0x20 pointer (Path D) */
    u32 src_word0, src_word1;
    u16 hw0, hw1, hw2;
    int t0, i;
    int t2;         /* dirty-flag: 0 = clean, 1 = pre-existing 0x80 found */
    int t6, v1_cond;
    u32 a2_cur;
    u32 t3;         /* saved a1 (source pointer) */

    s_wm_89160_calls++;
    t3 = a1;  /* retail: move $t3, $a1 at 0x80089160 */

    /* Compute record address: table_base + a0 * 672 */
    {
        u32 v0 = a0;
        v0 = (v0 << 2) + a0;   /* a0*5 */
        v0 = (v0 << 2) + a0;   /* a0*21 */
        v0 = v0 << 5;           /* a0*672 */
        table_base = WM_U32(WM_89160_TABLE_BASE_PTR);
        record_addr = table_base + v0;
    }

    fprintf(stderr,
            "[wm-80089160] entry a0=%u a1=0x%08x a2=0x%08x "
            "table_base=0x%08x record=0x%08x\n",
            a0, a1, a2, table_base, record_addr);

    /* Phase 1: check bit 0x80 at record+0x4F (0x8008918C–0x800891A4).
     * Retail: t2=0, t0=7.  Reads same byte each iteration.
     * If bit 0x80 found: t2=1, branch to Phase 2 (0x80089238→0x800891A8).
     * If bit clear: decrement t0, loop until t0==-1.
     * Key: retail NEVER returns here — it always continues to dispatch. */
    t2 = 0;
    flag_byte = WM_U8(record_addr + WM_89160_FLAG_BYTE_OFFSET);
    for (t0 = 7; t0 >= 0; t0--) {
        if (flag_byte & WM_89160_FLAG_BIT) {
            /* 0x80089198: bnez → 0x80089238: t2++, j 0x800891A8 */
            t2 = 1;
            fprintf(stderr,
                    "[wm-80089160] bit 0x80 found, t2=1, continuing\n");
            break;
        }
    }

    /* Phase 2: conditional dispatch (0x800891A8–0x80089348).
     *
     * Recompute record (retail does this after the first loop). */
    {
        u32 v0 = a0;
        v0 = (v0 << 2) + a0;
        v0 = (v0 << 2) + a0;
        v0 = v0 << 5;
        table_base = WM_U32(WM_89160_TABLE_BASE_PTR);
        record_addr = table_base + v0;
    }

    /* 0x800891C4: sltiu $a0, $t3, 1 — NOTE: $a0 is OVERWRITTEN here.
     * The subsequent and at 0x800891D0 uses this overwritten $a0, NOT
     * the original record index. */
    t6 = (t3 < 1) ? 1 : 0;    /* retail: sltiu $a0, $t3, 1 */
    v1_cond = (a2 < 1) ? 1 : 0; /* retail: sltiu $v1, $a2, 1 */

    /* 0x800891D0: and $v0, $a0, $v1; beqz → 0x80089240
     * Uses the OVERWRITTEN $a0 (= t6), NOT the original record index.
     * For natural (14,0,0): t6=1, v1=1 → and=1 → NOT taken → Path A. */
    if ((t6 & v1_cond) == 0) {
        goto dispatch_check_2;
    }

    /* ---- Path A (0x800891DC–0x80089234) ----
     * Natural path for (a1 < 1) & (a2 < 1).
     * Uses $a0 = record + 0x18 as loop pointer, $a1 = -1 as sentinel.
     * Flag block + 4 unconditional aligned stores per iteration. */
    {
        u32 a0_loop = record_addr + 0x18;  /* addiu $a0, $t1, 0x18 */
        int sentinel = -1;                   /* addiu $a1, $zero, -1 */

        fprintf(stderr,
                "[wm-80089160] Path A t2=%d\n", t2);

        for (i = 0; i < WM_89160_SUBRECORD_COUNT; i++) {
            s_wm_89160_iterations++;

            if (t2 == 0) {
                /* Flag block (0x800891EC–0x8008920C):
                 * Flag byte at a0+0x37, hw from a0-8, word from (t1),
                 * zero hw at a0-0x0E, hw at a0-0x06, word at a0-0x14 */
                u8 byte_4f = WM_U8(a0_loop + 0x37);
                byte_4f |= WM_89160_FLAG_BIT;
                WM_U8(a0_loop + 0x37) = byte_4f;

                WM_U16(a0_loop - 0x06) = WM_U16(a0_loop - 0x08);
                WM_U32(a0_loop - 0x14) = WM_U32(record_addr);
                WM_U16(a0_loop - 0x0E) = 0;
            }

            /* Unconditional stores (0x80089210–0x8008921C) */
            WM_U32(a0_loop + 4) = 0;    /* sw zero, 4($a0) */
            WM_U32(a0_loop - 4) = 0;    /* sw zero, -4($a0) */
            WM_U16(a0_loop + 8) = 0;    /* sh zero, 8($a0) */
            WM_U16(a0_loop + 0) = 0;    /* sh zero, ($a0) */

            a0_loop += WM_89160_SUBRECORD_STRIDE;
            record_addr += WM_89160_SUBRECORD_STRIDE;
        }
    }

    fprintf(stderr,
            "[wm-80089160] exit t2=%d iterations=%d\n",
            t2, s_wm_89160_iterations);
    return;

dispatch_check_2:
    /* 0x80089240: sltu $v0, $zero, $t3; and $v0, $v0, $v1; beqz → 0x800892C0 */
    if (((0 < t3) ? 1 : 0) & v1_cond) {
        /* ---- Path B (0x80089250–0x800892BC) ---- */
        u32 a2_loop = record_addr + 0x20;

        fprintf(stderr,
                "[wm-80089160] Path B t2=%d\n", t2);

        for (i = 0; i < WM_89160_SUBRECORD_COUNT; i++) {
            s_wm_89160_iterations++;

            if (t2 == 0) {
                u8 byte_4f = WM_U8(a2_loop + 0x2F);
                byte_4f |= WM_89160_FLAG_BIT;
                WM_U8(a2_loop + 0x2F) = byte_4f;

                WM_U16(a2_loop - 0x0E) = WM_U16(a2_loop - 0x10);
                WM_U32(a2_loop - 0x1C) = WM_U32(record_addr);
                WM_U16(a2_loop - 0x16) = 0;
            }

            /* lwl/lwr from t3, swl/swr to a2_loop.
             * MIPS retail convention: lwl at addr+3, lwr at addr+0. */
            src_word0 = psx_lwl_lwr(t3 + 3, t3);
            src_word1 = psx_lwl_lwr(t3 + 7, t3 + 4);
            psx_swl(a2_loop - 9, src_word0);
            psx_swr(a2_loop - 12, src_word0);
            psx_swl(a2_loop - 5, src_word1);
            psx_swr(a2_loop - 8, src_word1);

            WM_U32(a2_loop - 4) = 0;
            WM_U16(a2_loop + 0) = 0;

            a2_loop += WM_89160_SUBRECORD_STRIDE;
            record_addr += WM_89160_SUBRECORD_STRIDE;
        }

        fprintf(stderr,
                "[wm-80089160] exit t2=%d iterations=%d\n",
                t2, s_wm_89160_iterations);
        return;
    }

dispatch_check_3:
    /* 0x800892C0: sltu $v0, $zero, $a2; and $v0, $a0, $v0; beqz → 0x8008934C
     * $a0 at this point = t6 (the sltiu result from 0x800891C4). */
    if ((t6 & ((0 < a2) ? 1 : 0)) != 0) {
        /* ---- Path C (0x800892D0–0x80089344) ---- */
        u32 a0_loop = record_addr + 0x20;
        int sentinel = -1;

        fprintf(stderr,
                "[wm-80089160] Path C t2=%d\n", t2);

        for (i = 0; i < WM_89160_SUBRECORD_COUNT; i++) {
            s_wm_89160_iterations++;

            if (t2 == 0) {
                u8 byte_4f = WM_U8(a0_loop + 0x2F);
                byte_4f |= WM_89160_FLAG_BIT;
                WM_U8(a0_loop + 0x2F) = byte_4f;

                WM_U16(a0_loop - 0x0E) = WM_U16(a0_loop - 0x10);
                WM_U32(a0_loop - 0x1C) = WM_U32(record_addr);
                WM_U16(a0_loop - 0x16) = 0;
            }

            /* Unconditional stores */
            WM_U32(a0_loop - 0x0C) = 0;
            WM_U16(a0_loop - 0x08) = 0;

            hw0 = WM_U16(a2);
            WM_S16(a0_loop - 4) = (s16)(-((s16)hw0));
            hw1 = WM_U16(a2 + 2);
            WM_S16(a0_loop - 2) = (s16)(-((s16)hw1));
            hw2 = WM_U16(a2 + 4);
            WM_S16(a0_loop + 0) = (s16)(-((s16)hw2));

            record_addr += WM_89160_SUBRECORD_STRIDE;
            a0_loop += WM_89160_SUBRECORD_STRIDE;
        }

        fprintf(stderr,
                "[wm-80089160] exit t2=%d iterations=%d\n",
                t2, s_wm_89160_iterations);
        return;
    }

    /* ---- Path D (0x8008934C–0x800893D4) ----
     * Uses $a3 = record+0x20 as loop pointer.
     * Flag block + lwl/lwr/swl/swr + lhu/negu/sh per iteration. */
    {
        u32 t4 = (u32)-1;  /* sentinel */
        a3 = record_addr + 0x20;
        a2_cur = a2;

        fprintf(stderr,
                "[wm-80089160] Path D t2=%d\n", t2);

        for (i = 0; i < WM_89160_SUBRECORD_COUNT; i++) {
            s_wm_89160_iterations++;

            if (t2 == 0) {
                /* Flag block (0x8008935C–0x8008937C) */
                u8 byte_4f = WM_U8(a3 + 0x2F);
                byte_4f |= WM_89160_FLAG_BIT;
                WM_U8(a3 + 0x2F) = byte_4f;

                WM_U16(a3 - 0x0E) = WM_U16(record_addr + 0x10);
                WM_U32(a3 - 0x1C) = WM_U32(record_addr);
                WM_U16(a3 - 0x16) = 0;
            }

            /* Unconditional: lwl/lwr from t3, swl/swr to a3.
             * MIPS retail convention: lwl at addr+3, lwr at addr+0. */
            src_word0 = psx_lwl_lwr(t3 + 3, t3);
            src_word1 = psx_lwl_lwr(t3 + 7, t3 + 4);
            psx_swl(a3 - 9, src_word0);
            psx_swr(a3 - 12, src_word0);
            psx_swl(a3 - 5, src_word1);
            psx_swr(a3 - 8, src_word1);

            /* sw zero at a3-4 */
            WM_U32(a3 - 4) = 0;

            /* lhu from a2, negu, sh */
            hw0 = WM_U16(a2_cur);
            WM_S16(a3 - 4) = (s16)(-((s16)hw0));
            hw1 = WM_U16(a2_cur + 2);
            WM_S16(a3 - 2) = (s16)(-((s16)hw1));
            hw2 = WM_U16(a2_cur + 4);
            WM_S16(a3 + 0) = (s16)(-((s16)hw2));

            a3 += WM_89160_SUBRECORD_STRIDE;
            record_addr += WM_89160_SUBRECORD_STRIDE;
            a2_cur += WM_89160_SUBRECORD_STRIDE;
        }
    }

    fprintf(stderr,
            "[wm-80089160] exit t2=%d iterations=%d\n",
            t2, s_wm_89160_iterations);
}

/* ---- Common-tail P0 production implementation ---- */

/* Bounded common-tail prefix starting at 0x8007290C.
 *
 * Reads C610 selector, dispatches:
 *   C610=0: calls wm_80089160(14,0,0), sets D_80059179=1
 *   C610≠0: skips wm_80089160, leaves D_80059179=0
 *
 * Returns the exact reconvergence cut PC: 0x8007293C.
 * Does NOT execute any instruction at or after 0x8007293C. */
u32 wm_8007290C_common_tail_p0(void)
{
    u32 c610;
    u32 cut;

    s_wm_ctp0_entry++;

    /* 0x8007290C–0x80072910: load C610 selector. */
    c610 = WM_U32(WM_SLOT_C610_ABS);
    fprintf(stderr,
            "[worldmap-common-tail-p0] entry C610=%u\n", c610);

    /* 0x80072914–0x80072918: D_80059179 = 0 (unconditional). */
    WM_U8(WM_D_80059179_ABS) = 0;

    /* 0x8007291C: bnez $v0, 0x8007293C */
    if (c610 != 0) {
        /* C610 ≠ 0: skip 0x80089160 call, go to reconvergence. */
        s_wm_ctp0_c610_nonzero++;
        cut = WM_COMMON_TAIL_P0_CUT;
        s_wm_ctp0_last_cut = cut;
        fprintf(stderr,
                "[worldmap-common-tail-p0] C610!=0 → skip 0x80089160, "
                "cut=0x%08x\n", cut);
        return cut;
    }

    /* C610 == 0: natural path. */
    s_wm_ctp0_c610_zero++;

    /* 0x80072920: a0 = 14 [delay slot]
     * 0x80072924: a1 = 0
     * 0x80072928: jal 0x80089160
     * 0x8007292C: a2 = 0 [delay slot] */
    fprintf(stderr,
            "[worldmap-common-tail-p0] C610=0 → calling wm_80089160(14,0,0)\n");
    wm_80089160(14, 0, 0);
    s_wm_ctp0_89160_calls++;

    /* 0x80072930: v0 = 1
     * 0x80072934–0x80072938: D_80059179 = 1 */
    WM_U8(WM_D_80059179_ABS) = 1;

    /* Reconvergence at 0x8007293C. */
    cut = WM_COMMON_TAIL_P0_CUT;
    s_wm_ctp0_last_cut = cut;
    fprintf(stderr,
            "[worldmap-common-tail-p0] exit cut=0x%08x\n", cut);
    return cut;
}


/* ---- 0x800978FC: world-map graphics buffer allocator ---- */

/* HeapAlloc declared in port_main.c / world_map_init.c */
extern void* HeapAlloc(u_int allocSize, u_int allocFlags);

/* Constants from retail decode. */
#define WM_978FC_ALLOC_SIZE     0x10000u   /* 64 KB */
#define WM_978FC_RECORD_COUNT   2048
#define WM_978FC_RECORD_STRIDE  32
#define WM_978FC_COPY_CHUNK     16

/* Global addresses written by this helper. */
#define WM_D_8009BC3C_ABS       0x8009BC3Cu
#define WM_D_8009BCB4_ABS       0x8009BCB4u

/* Convert host pointer (from HeapAlloc) to PSX KUSEG address. */
static u32 wm_host_to_psx(void *p)
{
    if (!p) return 0;
    uintptr_t host = (uintptr_t)p;
    uintptr_t base = (uintptr_t)g_PsxRam;
    if (host >= base && host < base + PSX_RAM_SIZE)
        return 0x80000000u | (u32)(host - base);
    return (u32)host;
}

void wm_800978FC(void)
{
    /* Retail 0x800978FC–0x800979C4.
     * Allocates two 64 KB buffers, initializes the first with a
     * repeating byte pattern (2048 records × 32 bytes), then copies
     * the first buffer to the second.
     *
     * Record initialization (per 32-byte record):
     *   byte[-3] = 7
     *   byte[-2] = 0x80
     *   byte[-1] = 0x80
     *   byte[+0] = 0x80
     *   byte[+1] = 0x24
     *   (remaining 27 bytes left as HeapAlloc zero-fill) */

    void *p1_host, *p2_host;
    u32 ptr1, ptr2;
    u32 v1;
    int i;

    /* 0x80097900–0x80097910: first HeapAlloc(0x10000, 1) */
    p1_host = HeapAlloc(WM_978FC_ALLOC_SIZE, 1);
    ptr1 = wm_host_to_psx(p1_host);

    /* 0x80097914–0x80097928: second HeapAlloc(0x10000, 1)
     * Delay slot stores ptr1 before the second call. */
    WM_U32(WM_D_8009BC3C_ABS) = ptr1;

    p2_host = HeapAlloc(WM_978FC_ALLOC_SIZE, 1);
    ptr2 = wm_host_to_psx(p2_host);
    WM_U32(WM_D_8009BCB4_ABS) = ptr2;

    fprintf(stderr,
            "[wm-800978FC] alloc ptr1=0x%08x ptr2=0x%08x\n",
            ptr1, ptr2);

    /* 0x8009792C–0x8009796C: initialize ptr1 records.
     * v1 starts at ptr1 + 6, increments by 32 each iteration.
     * 2048 iterations. */
    v1 = ptr1 + 6;
    for (i = 0; i < WM_978FC_RECORD_COUNT; i++) {
        WM_U8(v1 - 3) = 7;
        WM_U8(v1 - 2) = 0x80;
        WM_U8(v1 - 1) = 0x80;
        WM_U8(v1 + 0) = 0x80;
        WM_U8(v1 + 1) = 0x24;
        v1 += WM_978FC_RECORD_STRIDE;
    }

    /* 0x80097970–0x800979B0: copy ptr1 → ptr2.
     * 65536 / 16 = 4096 iterations, 16 bytes per iteration. */
    {
        u32 src = ptr1;
        u32 dst = ptr2;
        u32 end = ptr1 + WM_978FC_ALLOC_SIZE;
        while (src != end) {
            WM_U32(dst + 0)  = WM_U32(src + 0);
            WM_U32(dst + 4)  = WM_U32(src + 4);
            WM_U32(dst + 8)  = WM_U32(src + 8);
            WM_U32(dst + 12) = WM_U32(src + 12);
            src += WM_978FC_COPY_CHUNK;
            dst += WM_978FC_COPY_CHUNK;
        }
    }

    fprintf(stderr,
            "[wm-800978FC] init+copy done\n");
}


/* ---- Common-tail P1 instrumentation ---- */

static int  s_wm_ctp1_entry;
static int  s_wm_ctp1_978fc_calls;
static u32  s_wm_ctp1_last_cut;

int  wm_ctp1_get_entry(void)        { return s_wm_ctp1_entry; }
int  wm_ctp1_get_978fc_calls(void)  { return s_wm_ctp1_978fc_calls; }
u32  wm_ctp1_get_last_cut(void)     { return s_wm_ctp1_last_cut; }

void wm_common_tail_p1_reset(void)
{
    s_wm_ctp1_entry = 0;
    s_wm_ctp1_978fc_calls = 0;
    s_wm_ctp1_last_cut = 0;
}


/* ---- Common-tail P1: caller slice ---- */

u32 wm_8007293C_common_tail_p1(void)
{
    /* P1 slice: 0x8007293C..0x80072940
     * jal 0x800978FC + delay slot (nop).
     * First excluded: 0x80072944 (jal 0x8008901C).
     *
     * Requires P0 to have executed and returned 0x8007293C. */
    u32 cut;

    s_wm_ctp1_entry++;

    /* 0x8007293C: jal 0x800978FC */
    fprintf(stderr,
            "[worldmap-common-tail-p1] calling wm_800978FC\n");
    wm_800978FC();
    s_wm_ctp1_978fc_calls++;

    /* 0x80072940: nop (delay slot — no effect)
     * Return cut at 0x80072944 (first excluded = next helper). */
    cut = WM_COMMON_TAIL_P1_CUT;
    s_wm_ctp1_last_cut = cut;

    fprintf(stderr,
            "[worldmap-common-tail-p1] exit cut=0x%08x\n", cut);
    return cut;
}


/* ---- Common-tail P2 instrumentation ---- */

static int  s_wm_ctp2_entry;
static int  s_wm_ctp2_8901c_calls;
static u32  s_wm_ctp2_last_cut;
static int  s_wm_ctp2_alloc_calls;
static int  s_wm_ctp2_forbidden_865a0;
static int  s_wm_ctp2_forbidden_85fe0;
static int  s_wm_ctp2_forbidden_scheduler;
static int  s_wm_ctp2_forbidden_world_loop;

int  wm_ctp2_get_entry(void)              { return s_wm_ctp2_entry; }
int  wm_ctp2_get_8901c_calls(void)        { return s_wm_ctp2_8901c_calls; }
u32  wm_ctp2_get_last_cut(void)           { return s_wm_ctp2_last_cut; }
int  wm_ctp2_get_alloc_calls(void)        { return s_wm_ctp2_alloc_calls; }
int  wm_ctp2_get_forbidden_865a0(void)    { return s_wm_ctp2_forbidden_865a0; }
int  wm_ctp2_get_forbidden_85fe0(void)    { return s_wm_ctp2_forbidden_85fe0; }
int  wm_ctp2_get_forbidden_scheduler(void){ return s_wm_ctp2_forbidden_scheduler; }
int  wm_ctp2_get_forbidden_world_loop(void){ return s_wm_ctp2_forbidden_world_loop; }

void wm_common_tail_p2_reset(void)
{
    s_wm_ctp2_entry = 0;
    s_wm_ctp2_8901c_calls = 0;
    s_wm_ctp2_last_cut = 0;
    s_wm_ctp2_alloc_calls = 0;
    s_wm_ctp2_forbidden_865a0 = 0;
    s_wm_ctp2_forbidden_85fe0 = 0;
    s_wm_ctp2_forbidden_scheduler = 0;
    s_wm_ctp2_forbidden_world_loop = 0;
}

/* ---- P2 forbidden-path stubs ---- */

void wm_p2_800865a0_should_not_run(void) { s_wm_ctp2_forbidden_865a0++; }
void wm_p2_80085fe0_should_not_run(void) { s_wm_ctp2_forbidden_85fe0++; }
void wm_p2_80097800_should_not_run(void) { s_wm_ctp2_forbidden_scheduler++; }
void wm_p2_800712D0_should_not_run(void) { s_wm_ctp2_forbidden_world_loop++; }


/* ---- 0x8008901C production implementation ---- */

/* Exact native transcription of retail 0x8008901C–0x80089128.
 *
 * Allocates two 10240-byte (0x2800) buffers via HeapAlloc.
 * Stores pointers at D_8009BE1C and D_8009BE20.
 *
 * Initializes 256 records (40 bytes each) in the first buffer.
 * Record base = alloc_ptr + 7.  Per-record writes:
 *   byte[-4] = 9    (record type marker)
 *   byte[+0] = 0x2C (44, then OR'd with 0x02 → 0x2E after first pass)
 *   hw[+7]   = GetTPage(1, 1, 0x340, 0x100) = 0x00BD
 *   hw[+15]  = GetClut(0x100, 0x1FF) = 0x7FD0
 *
 * Then copies first buffer → second buffer (10240 bytes, 16-byte chunks).
 *
 * Only calls: HeapAlloc, PsyQ GetTPage, PsyQ GetClut. */
void wm_8008901C(void)
{
    void *p1_host, *p2_host;
    u32 ptr1, ptr2;
    u16 tpage_val, clut_val;
    u32 base;
    int i;

    /* 0x80089020–0x80089040: first HeapAlloc(0x2800, 1) */
    p1_host = HeapAlloc(WM_8901C_ALLOC_SIZE, 1);
    ptr1 = wm_host_to_psx(p1_host);
    s_wm_ctp2_alloc_calls++;

    /* 0x80089044–0x80089058: second HeapAlloc(0x2800, 1)
     * Delay slot stores ptr1 at D_8009BE1C before the call. */
    WM_U32(WM_D_8009BE1C_ABS) = ptr1;

    p2_host = HeapAlloc(WM_8901C_ALLOC_SIZE, 1);
    ptr2 = wm_host_to_psx(p2_host);
    s_wm_ctp2_alloc_calls++;

    /* 0x80089070–0x80089074: store ptr2 at D_8009BE20. */
    WM_U32(WM_D_8009BE20_ABS) = ptr2;

    fprintf(stderr,
            "[wm-8008901C] alloc ptr1=0x%08x ptr2=0x%08x\n",
            ptr1, ptr2);

    /* 0x80089084–0x80089088: GetTPage(1, 1, 0x340, 0x100) */
    tpage_val = GetTPage(1, 1, 0x340, 0x100);

    /* 0x80089098–0x8008909C: GetClut(0x100, 0x1FF) */
    clut_val = GetClut(0x100, 0x1FF);

    fprintf(stderr,
            "[wm-8008901C] tpage=0x%04x clut=0x%04x\n",
            tpage_val, clut_val);

    /* 0x80089078–0x800890C0: initialize 256 records.
     * Base = ptr1 + 7.  Stride = 40.  Count = 256. */
    base = ptr1 + WM_8901C_RECORD_BASE_OFFSET;
    for (i = 0; i < WM_8901C_RECORD_COUNT; i++) {
        /* 0x8008908C: sb $s4, -4($s0) → byte[-4] = 9 */
        WM_U8(base - 4) = 9;

        /* 0x80089094: sb $s3, 0($s0) → byte[0] = 0x2C */
        WM_U8(base + 0) = 0x2C;

        /* 0x800890A4 (delay of jal GetClut): sh $v0, 15($s0)
         * $v0 still holds GetTPage result at this point;
         * hw[15] = GetTPage, then GetClut overwrites $v0.
         * 0x800890B0: sh $v0, 7($s0) → hw[7] = GetClut result. */
        WM_U16(base + 7)  = tpage_val;
        WM_U16(base + 15) = clut_val;

        /* 0x800890B4–0x800890B8: lbu/ori/sb → byte[0] |= 0x02 */
        WM_U8(base + 0) = WM_U8(base + 0) | 0x02;

        base += WM_8901C_RECORD_STRIDE;
    }

    /* 0x800890D4–0x80089100: copy ptr1 → ptr2 (10240 bytes, 16-byte chunks). */
    {
        u32 src = ptr1;
        u32 dst = ptr2;
        u32 end = ptr1 + WM_8901C_ALLOC_SIZE;
        while (src != end) {
            WM_U32(dst + 0)  = WM_U32(src + 0);
            WM_U32(dst + 4)  = WM_U32(src + 4);
            WM_U32(dst + 8)  = WM_U32(src + 8);
            WM_U32(dst + 12) = WM_U32(src + 12);
            src += WM_8901C_COPY_CHUNK;
            dst += WM_8901C_COPY_CHUNK;
        }
    }

    fprintf(stderr,
            "[wm-8008901C] init+copy done\n");
}


/* ---- Common-tail P2: caller slice ---- */

u32 wm_80072944_common_tail_p2(void)
{
    /* P2 slice: 0x80072944..0x80072948
     * jal 0x8008901C + delay slot (nop).
     * First excluded: 0x8007294C (jal 0x800865A0).
     *
     * Requires P1 to have executed and returned 0x80072944. */
    u32 cut;

    s_wm_ctp2_entry++;

    /* 0x80072944: jal 0x8008901C */
    fprintf(stderr,
            "[worldmap-common-tail-p2] calling wm_8008901C\n");
    wm_8008901C();
    s_wm_ctp2_8901c_calls++;

    /* 0x80072948: nop (delay slot — no effect)
     * Return cut at 0x8007294C (first excluded = next helper). */
    cut = WM_COMMON_TAIL_P2_CUT;
    s_wm_ctp2_last_cut = cut;

    fprintf(stderr,
            "[worldmap-common-tail-p2] exit cut=0x%08x\n", cut);
    return cut;
}
