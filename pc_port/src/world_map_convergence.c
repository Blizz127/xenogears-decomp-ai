/*
 * World-map convergence P1 module (W34B1).
 *
 * Extracted from world_map_init.c — the one authoritative implementation
 * of wm_800726C0_convergence_p1 and wm_pool_register.
 *
 * Both the production game build and the production-linked test link
 * this same object.  Do NOT duplicate these functions elsewhere.
 *
 * Retail slice: 0x800726C0–0x80072728.
 * Provenance: exact transcription of retail dispatch + first-table loop.
 * Never reads 0x8009C610 / 0x8009A034, never executes 0x8007272C+.
 */
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_convergence.h"

/* Retail layout constants (absolute PSX addresses). */
#define WM_FLAG_C894_ABS         0x8009C894u
#define WM_CONV_TABLE_A_BASE     0x80099E8Cu
#define WM_POOL_BE24             0x8009BE24u
#define WM_POOL_SLOT_COUNT       64
#define WM_POOL_SLOT_STRIDE      0x80u
#define WM_POOL_OFF_18           0x18u
#define WM_POOL_OFF_1C           0x1Cu

#define WM_CONV_P1_CUT_SECOND_TABLE 0x8007272Cu
#define WM_CONV_P1_CUT_FLAG1_ARC    0x80072784u
#define WM_CONV_P1_CUT_COMMON_TAIL 0x8007290Cu

#define WM_U32(a) (*(u32*)PSX_ADDR(a))

/* Resolve a 32-bit guest value to a host pointer. */
static void* psx_u32_to_host(u32 p)
{
    if (p == 0)
        return NULL;
    if (p >= 0x80000000u && p < 0x80200000u)
        return PSX_ADDR(p);
    return (void*)(uintptr_t)p;
}

/* Per-world-init instrumentation counters.
 * All reset at each new world init boundary via wm_conv_p1_reset(). */
static int s_wm_conv_p1_entry;
static int s_wm_conv_p1_flag0;
static int s_wm_conv_p1_flag1_cut;
static int s_wm_conv_p1_other_cut;
static int s_wm_conv_p1_empty;
static int s_wm_conv_p1_iterations;
static int s_wm_conv_p1_helper_calls;
static int s_wm_conv_p1_cut_second_table;
static int s_wm_conv_p1_forbidden_c610_read;
static int s_wm_conv_p1_forbidden_a034_read;
static int s_wm_conv_p1_forbidden_976fc;
static int s_wm_conv_p1_forbidden_common_tail;
static int s_wm_conv_p1_forbidden_excluded_instr;
static u32  s_wm_conv_p1_last_next;
static int s_wm_pool_alloc;

/* ---- Counter accessors ---- */

int  wm_conv_p1_get_entry(void)                 { return s_wm_conv_p1_entry; }
int  wm_conv_p1_get_flag0(void)                  { return s_wm_conv_p1_flag0; }
int  wm_conv_p1_get_flag1_cut(void)              { return s_wm_conv_p1_flag1_cut; }
int  wm_conv_p1_get_other_cut(void)              { return s_wm_conv_p1_other_cut; }
int  wm_conv_p1_get_empty(void)                  { return s_wm_conv_p1_empty; }
int  wm_conv_p1_get_iterations(void)             { return s_wm_conv_p1_iterations; }
int  wm_conv_p1_get_helper_calls(void)           { return s_wm_conv_p1_helper_calls; }
int  wm_conv_p1_get_cut_second_table(void)       { return s_wm_conv_p1_cut_second_table; }
u32  wm_conv_p1_get_last_next(void)              { return s_wm_conv_p1_last_next; }
int  wm_conv_p1_get_forbidden_c610_read(void)    { return s_wm_conv_p1_forbidden_c610_read; }
int  wm_conv_p1_get_forbidden_a034_read(void)    { return s_wm_conv_p1_forbidden_a034_read; }
int  wm_conv_p1_get_forbidden_976fc(void)        { return s_wm_conv_p1_forbidden_976fc; }
int  wm_conv_p1_get_forbidden_common_tail(void)  { return s_wm_conv_p1_forbidden_common_tail; }
int  wm_conv_p1_get_forbidden_excluded_instr(void) { return s_wm_conv_p1_forbidden_excluded_instr; }
int  wm_conv_p1_get_pool_alloc(void)             { return s_wm_pool_alloc; }

/* ---- Counter reset ---- */

void wm_conv_p1_reset(void)
{
    s_wm_conv_p1_entry = 0;
    s_wm_conv_p1_flag0 = 0;
    s_wm_conv_p1_flag1_cut = 0;
    s_wm_conv_p1_other_cut = 0;
    s_wm_conv_p1_empty = 0;
    s_wm_conv_p1_iterations = 0;
    s_wm_conv_p1_helper_calls = 0;
    s_wm_conv_p1_cut_second_table = 0;
    s_wm_conv_p1_forbidden_c610_read = 0;
    s_wm_conv_p1_forbidden_a034_read = 0;
    s_wm_conv_p1_forbidden_976fc = 0;
    s_wm_conv_p1_forbidden_common_tail = 0;
    s_wm_conv_p1_forbidden_excluded_instr = 0;
    s_wm_conv_p1_last_next = 0;
    s_wm_pool_alloc = 0;
}

/* ---- Pool registration helper ---- */

/* Native transcription of retail 0x80097718.
 * Pool registration: searches WM_POOL_BE24 for a free slot (occupancy at
 * +0x1C == 0), zeroes status halfwords, stores a0 at +0x18 and a1 at +0x1C.
 * Bounded to 64 iterations; silent no-op if pool full.
 * Leaf — no callees, no stack frame. */
void wm_pool_register(u32 a0, u32 a1)
{
    u32 pool_psx = WM_U32(WM_POOL_BE24);
    u8* base;
    int i;

    fprintf(stderr, "[worldmap-pool-register] entry a0=0x%08x a1=0x%08x\n",
            a0, a1);

    if (pool_psx == 0)
        return;
    base = (u8*)psx_u32_to_host(pool_psx);
    if (base == NULL)
        return;

    for (i = 0; i < WM_POOL_SLOT_COUNT; i++) {
        u8* slot = base + (u32)i * WM_POOL_SLOT_STRIDE;
        u32 occupancy = *(u32*)(slot + WM_POOL_OFF_1C);
        if (occupancy == 0) {
            *(u16*)(slot + 0x00) = 0;
            *(u16*)(slot + 0x02) = 0;
            *(u16*)(slot + 0x04) = 0;
            *(u32*)(slot + WM_POOL_OFF_18) = a0;
            *(u32*)(slot + WM_POOL_OFF_1C) = a1;
            *(u16*)(slot + 0x20) = 0;
            *(u16*)(slot + 0x22) = 0;
            s_wm_pool_alloc++;
            fprintf(stderr, "[worldmap-pool-register] "
                    "slot=%d inserted a0=0x%08x a1=0x%08x\n",
                    i, a0, a1);
            return;
        }
    }
    /* Pool full — retail silently drops. */
    fprintf(stderr, "[worldmap-pool-register] "
            "no free slot (dropped)\n");
}

/* ---- Convergence P1 (production) ---- */

/* W34B1: first convergence caller slice 0x800726C0–0x80072728.
 * Exact transcription of retail dispatch + first-table loop.
 * Provenance-preserving name wm_800726C0_convergence_p1.
 * Never reads 0x8009C610 / 0x8009A034, never executes 0x8007272C+.
 * Returns the retail PC where control would continue (cut). */
wm_conv_p1_next_t wm_800726C0_convergence_p1(void)
{
    u32 flag;

    s_wm_conv_p1_entry++;
    flag = WM_U32(WM_FLAG_C894_ABS);
    fprintf(stderr,
            "[worldmap-convergence-p1] entry flag=0x%08x\n",
            flag);

    if (flag == 1) {
        s_wm_conv_p1_flag1_cut++;
        s_wm_conv_p1_last_next = WM_CONV_P1_CUT_FLAG1_ARC;
        fprintf(stderr,
                "[worldmap-convergence-p1] flag==1 cut retail_pc=0x%08x\n",
                WM_CONV_P1_CUT_FLAG1_ARC);
        return WM_CONV_P1_CUT_FLAG1_ARC;
    }
    if (flag != 0) {
        s_wm_conv_p1_other_cut++;
        s_wm_conv_p1_last_next = WM_CONV_P1_CUT_COMMON_TAIL;
        fprintf(stderr,
                "[worldmap-convergence-p1] flag other (0x%08x) cut retail_pc=0x%08x\n",
                flag, WM_CONV_P1_CUT_COMMON_TAIL);
        return WM_CONV_P1_CUT_COMMON_TAIL;
    }

    /* flag == 0 : first-table path. */
    s_wm_conv_p1_flag0++;

    {
        u32 *base = (u32*)PSX_ADDR(WM_CONV_TABLE_A_BASE);
        if (*base == 0) {
            s_wm_conv_p1_empty++;
            s_wm_conv_p1_cut_second_table++;
            s_wm_conv_p1_last_next = WM_CONV_P1_CUT_SECOND_TABLE;
            fprintf(stderr,
                    "[worldmap-convergence-p1] empty table cut retail_pc=0x%08x\n",
                    WM_CONV_P1_CUT_SECOND_TABLE);
            return WM_CONV_P1_CUT_SECOND_TABLE;
        }

        {
            u32 *s0 = base;
            do {
                u32 a0 = s0[0];
                u32 a1 = s0[1];
                s_wm_conv_p1_iterations++;
                s_wm_conv_p1_helper_calls++;
                fprintf(stderr,
                        "[worldmap-convergence-p1] iter a0=0x%08x a1=0x%08x (helper @0x80072714)\n",
                        a0, a1);
                wm_pool_register(a0, a1);
                s0 += 2; /* 8-byte stride = 2×u32 */
            } while (*s0 != 0);
        }

        s_wm_conv_p1_cut_second_table++;
        s_wm_conv_p1_last_next = WM_CONV_P1_CUT_SECOND_TABLE;
        fprintf(stderr,
                "[worldmap-convergence-p1] cut retail_pc=0x%08x iterations=%d\n",
                WM_CONV_P1_CUT_SECOND_TABLE, s_wm_conv_p1_iterations);
        return WM_CONV_P1_CUT_SECOND_TABLE;
    }
}
