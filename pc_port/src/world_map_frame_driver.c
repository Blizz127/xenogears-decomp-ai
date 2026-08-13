/*
 * W34B18-B: world frame-driver prologue 0x800712D0 .. 0x80071484 inclusive.
 *
 * Fresh world_map.bin listing (load base 0x8006FAF0):
 *   [0x800712D0, 0x80071488) = 110 instructions, 440 bytes,
 *   SHA-256 fccdf4bb24527fca3e26a8d91f368886ea127344a0c59c8f5d0b1ca595f0805d
 *   no JALR. Hard-cut before jal 0x80097800 at 0x80071488.
 *
 * Both the production game build and the production-linked test link this
 * same object. Do NOT duplicate this function elsewhere.
 */
#include <stdio.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_frame_driver.h"

#define WM_FP_DB_PTR        0x8009BE3Cu
#define WM_FP_ENVREC0       0x8009BBC8u
#define WM_FP_ENVREC1       0x8009BC40u
#define WM_FP_INDEX         0x8009D7F0u
#define WM_FP_D554          0x8009D554u
#define WM_FP_BD1C          0x8009BD1Cu
#define WM_FP_BD14          0x8009BD14u
#define WM_FP_CD50          0x8009CD50u
#define WM_FP_BD18          0x8009BD18u
#define WM_FP_BD10          0x8009BD10u
#define WM_FP_CD4C          0x8009CD4Cu
#define WM_FP_CDSYNC_BUF    0x8009C588u
#define WM_FP_OT_OFF        0x70u
#define WM_FP_ENV_STRIDE    0x78u

#if defined(WM_712D0_MUTANT_M2)
#undef WM_FP_DB_PTR
#define WM_FP_DB_PTR        0x8009BE38u
#endif
#if defined(WM_712D0_MUTANT_M12)
#undef WM_FP_OT_OFF
#define WM_FP_OT_OFF        0x6Cu
#endif

#if defined(WM_712D0_MUTANT_M13)
#define WM_FP_OT_COUNT      0x3FF
#else
#define WM_FP_OT_COUNT      0x400
#endif

extern u16 g_C1ButtonState;
extern u16 g_C2ButtonState;
extern u16 g_C1ButtonStateReleased;
extern u16 g_C2ButtonStateReleased;
extern u16 g_C1ButtonStatePressedOnce;
extern u16 g_C2ButtonStatePressedOnce;

extern int ControllerPopState(void);
extern int VSync(int mode);
extern int CdSync(int mode, u8* result);
extern u32* ClearOTagR(u32* ot, int n);
extern void func_800250E0(int context);
extern void func_8001D468(void);
extern u32 wm_800967E4_dispatch_cd_work(void);
#if defined(WM_712D0_MUTANT_M9) || defined(WM_712D0_MUTANT_M10)
extern void wm_80097800(void);
#endif

enum {
    WM_FP_TRACE_LW = 1,
    WM_FP_TRACE_SW = 2,
    WM_FP_TRACE_LHU = 3,
    WM_FP_TRACE_SH = 4,
    WM_FP_TRACE_CALL = 5,
    WM_FP_TRACE_RET = 6,
    WM_FP_TRACE_BR = 7
};

enum {
    WM_FP_CALL_POP = 1,
    WM_FP_CALL_967E4 = 2,
    WM_FP_CALL_VSYNC = 3,
    WM_FP_CALL_CDSYNC = 4,
    WM_FP_CALL_CLEAROTAG = 5,
    WM_FP_CALL_250E0 = 6,
    WM_FP_CALL_1D468 = 7,
    WM_FP_CALL_97800 = 8
};

#if defined(WM_712D0_TEST_TRACE)
extern void wm_712d0_test_trace(u32 pc, u32 kind, u32 address,
                                u32 width, u32 value);
#define WM_FP_TRACE(pc, kind, address, width, value) \
    wm_712d0_test_trace((pc), (kind), (address), (width), (value))
#else
#define WM_FP_TRACE(pc, kind, address, width, value) ((void)0)
#endif

static int s_fp_entry;
static int s_fp_cd_work_calls;
static int s_fp_vsync_retries;
static int s_fp_pad_iters;
static int s_fp_scheduler_calls;
static u32 s_fp_cut_pc;

int wm_fp_get_entry(void) { return s_fp_entry; }
int wm_fp_get_cd_work_calls(void) { return s_fp_cd_work_calls; }
int wm_fp_get_vsync_retries(void) { return s_fp_vsync_retries; }
int wm_fp_get_pad_iters(void) { return s_fp_pad_iters; }
int wm_fp_get_scheduler_calls(void) { return s_fp_scheduler_calls; }
u32 wm_fp_get_cut_pc(void) { return s_fp_cut_pc; }

void wm_fp_reset(void)
{
    s_fp_entry = 0;
    s_fp_cd_work_calls = 0;
    s_fp_vsync_retries = 0;
    s_fp_pad_iters = 0;
    s_fp_scheduler_calls = 0;
    s_fp_cut_pc = 0;
}

static u16 wm_fp_load_u16(u32 address)
{
    u16 value;
    value = *(volatile u16*)PSX_ADDR(address);
    return value;
}

static void wm_fp_store_u16(u32 address, u16 value)
{
    *(volatile u16*)PSX_ADDR(address) = value;
}

static u32 wm_fp_load_u32(u32 address)
{
    return *(volatile u32*)PSX_ADDR(address);
}

static void wm_fp_store_u32(u32 address, u32 value)
{
    *(volatile u32*)PSX_ADDR(address) = value;
}

static u16 wm_fp_merge_u16(u16 acc, u16 src)
{
#if defined(WM_712D0_MUTANT_M4)
    /* Signedness mutant: treat bit 15 as a signed negative and drop the merge. */
    if ((src & 0x8000u) != 0u)
        return acc;
    return (u16)(acc | src);
#else
    return (u16)(acc | src);
#endif
}

void wm_800712D0_frame_prologue(void)
{
    int pop;
    u32 cd_work;
    u32 db_ptr;
    u32 env;
    u32 ot;
    u32 index;
    u32 flipped;
#if defined(WM_712D0_MUTANT_M6)
    u32 stale_cd = 0;
#endif
#if defined(WM_712D0_TEST_TRACE)
    int pad_guard = 0;
    int cd_guard = 0;
#endif

    s_fp_entry++;
    fprintf(stderr, "[worldmap-frame-prologue] entry call=%d\n", s_fp_entry);

    wm_fp_store_u32(WM_FP_DB_PTR, WM_FP_ENVREC1);
    WM_FP_TRACE(0x800712ECu, WM_FP_TRACE_SW, WM_FP_DB_PTR, 4u, WM_FP_ENVREC1);

    wm_fp_store_u32(WM_FP_INDEX, 1u);
    WM_FP_TRACE(0x80071300u, WM_FP_TRACE_SW, WM_FP_INDEX, 4u, 1u);

    wm_fp_store_u32(WM_FP_D554, 1u);
    WM_FP_TRACE(0x80071308u, WM_FP_TRACE_SW, WM_FP_D554, 4u, 1u);

#if defined(WM_712D0_MUTANT_M3)
    wm_fp_store_u32(WM_FP_BD1C, 0u);
    wm_fp_store_u32(WM_FP_BD14, 0u);
    wm_fp_store_u32(WM_FP_CD50, 0u);
    wm_fp_store_u32(WM_FP_BD18, 0u);
    wm_fp_store_u32(WM_FP_BD10, 0u);
    wm_fp_store_u32(WM_FP_CD4C, 0u);
#else
    wm_fp_store_u16(WM_FP_BD1C, 0);
    WM_FP_TRACE(0x80071310u, WM_FP_TRACE_SH, WM_FP_BD1C, 2u, 0u);
    wm_fp_store_u16(WM_FP_BD14, 0);
    WM_FP_TRACE(0x80071318u, WM_FP_TRACE_SH, WM_FP_BD14, 2u, 0u);
    wm_fp_store_u16(WM_FP_CD50, 0);
    WM_FP_TRACE(0x80071320u, WM_FP_TRACE_SH, WM_FP_CD50, 2u, 0u);
    wm_fp_store_u16(WM_FP_BD18, 0);
    WM_FP_TRACE(0x80071328u, WM_FP_TRACE_SH, WM_FP_BD18, 2u, 0u);
    wm_fp_store_u16(WM_FP_BD10, 0);
    WM_FP_TRACE(0x80071330u, WM_FP_TRACE_SH, WM_FP_BD10, 2u, 0u);
    wm_fp_store_u16(WM_FP_CD4C, 0);
    WM_FP_TRACE(0x80071338u, WM_FP_TRACE_SH, WM_FP_CD4C, 2u, 0u);
#endif

    for (;;) {
        pop = ControllerPopState();
        WM_FP_TRACE(0x8007133Cu, WM_FP_TRACE_CALL, WM_FP_CALL_POP, 0u,
                    (u32)pop);
        s_fp_pad_iters++;
#if defined(WM_712D0_TEST_TRACE)
        pad_guard++;
        if (pad_guard > 32)
            break;
#endif
#if defined(WM_712D0_MUTANT_M1)
        if (pop != 0)
#else
        if (pop == 0)
#endif
        {
            WM_FP_TRACE(0x80071344u, WM_FP_TRACE_BR, 0x800713FCu, 0u, 0u);
            break;
        }
        /* Exact retail access order for one pad-merge iteration. */
        {
            u16 acc_cd4c = wm_fp_load_u16(WM_FP_CD4C);
            u16 c1 = g_C1ButtonState;
            u16 acc_cd50 = wm_fp_load_u16(WM_FP_CD50);
            u16 c2 = g_C2ButtonState;
            WM_FP_TRACE(0x80071350u, WM_FP_TRACE_LHU, WM_FP_CD4C, 2u, acc_cd4c);
            WM_FP_TRACE(0x80071358u, WM_FP_TRACE_LHU, 0x80059570u, 2u, c1);
            WM_FP_TRACE(0x80071360u, WM_FP_TRACE_LHU, WM_FP_CD50, 2u, acc_cd50);
            WM_FP_TRACE(0x80071368u, WM_FP_TRACE_LHU, 0x80059574u, 2u, c2);
            acc_cd4c = wm_fp_merge_u16(acc_cd4c, c1);
            wm_fp_store_u16(WM_FP_CD4C, acc_cd4c);
            WM_FP_TRACE(0x80071374u, WM_FP_TRACE_SH, WM_FP_CD4C, 2u, acc_cd4c);

            {
                u16 acc_bd10 = wm_fp_load_u16(WM_FP_BD10);
                u16 c1rel = g_C1ButtonStateReleased;
                WM_FP_TRACE(0x8007137Cu, WM_FP_TRACE_LHU, WM_FP_BD10, 2u,
                            acc_bd10);
                WM_FP_TRACE(0x80071384u, WM_FP_TRACE_LHU, 0x8005948Cu, 2u,
                            c1rel);
                acc_cd50 = wm_fp_merge_u16(acc_cd50, c2);
                wm_fp_store_u16(WM_FP_CD50, acc_cd50);
                WM_FP_TRACE(0x80071390u, WM_FP_TRACE_SH, WM_FP_CD50, 2u,
                            acc_cd50);

                {
                    u16 acc_bd14 = wm_fp_load_u16(WM_FP_BD14);
                    u16 c2rel = g_C2ButtonStateReleased;
                    WM_FP_TRACE(0x80071398u, WM_FP_TRACE_LHU, WM_FP_BD14, 2u,
                                acc_bd14);
                    WM_FP_TRACE(0x800713A0u, WM_FP_TRACE_LHU, 0x80059490u, 2u,
                                c2rel);
                    acc_bd10 = wm_fp_merge_u16(acc_bd10, c1rel);
                    wm_fp_store_u16(WM_FP_BD10, acc_bd10);
                    WM_FP_TRACE(0x800713ACu, WM_FP_TRACE_SH, WM_FP_BD10, 2u,
                                acc_bd10);

                    {
                        u16 acc_bd18 = wm_fp_load_u16(WM_FP_BD18);
                        u16 c1once = g_C1ButtonStatePressedOnce;
                        WM_FP_TRACE(0x800713B4u, WM_FP_TRACE_LHU, WM_FP_BD18,
                                    2u, acc_bd18);
                        WM_FP_TRACE(0x800713BCu, WM_FP_TRACE_LHU, 0x800594A4u,
                                    2u, c1once);
                        acc_bd14 = wm_fp_merge_u16(acc_bd14, c2rel);
                        wm_fp_store_u16(WM_FP_BD14, acc_bd14);
                        WM_FP_TRACE(0x800713C8u, WM_FP_TRACE_SH, WM_FP_BD14,
                                    2u, acc_bd14);

                        {
                            u16 acc_bd1c = wm_fp_load_u16(WM_FP_BD1C);
                            u16 c2once = g_C2ButtonStatePressedOnce;
                            WM_FP_TRACE(0x800713D0u, WM_FP_TRACE_LHU,
                                        WM_FP_BD1C, 2u, acc_bd1c);
                            WM_FP_TRACE(0x800713D8u, WM_FP_TRACE_LHU,
                                        0x800594A8u, 2u, c2once);
                            acc_bd18 = wm_fp_merge_u16(acc_bd18, c1once);
                            acc_bd1c = wm_fp_merge_u16(acc_bd1c, c2once);
                            wm_fp_store_u16(WM_FP_BD18, acc_bd18);
                            WM_FP_TRACE(0x800713E8u, WM_FP_TRACE_SH,
                                        WM_FP_BD18, 2u, acc_bd18);
                            wm_fp_store_u16(WM_FP_BD1C, acc_bd1c);
                            WM_FP_TRACE(0x800713F0u, WM_FP_TRACE_SH,
                                        WM_FP_BD1C, 2u, acc_bd1c);
                        }
                    }
                }
            }
        }
        WM_FP_TRACE(0x800713F4u, WM_FP_TRACE_BR, 0x8007133Cu, 0u, 1u);
    }

    for (;;) {
        cd_work = wm_800967E4_dispatch_cd_work();
        WM_FP_TRACE(0x800713FCu, WM_FP_TRACE_CALL, WM_FP_CALL_967E4, 0u,
                    cd_work);
        WM_FP_TRACE(0x800713FCu, WM_FP_TRACE_RET, WM_FP_CALL_967E4, 0u,
                    cd_work);
        s_fp_cd_work_calls++;
#if defined(WM_712D0_MUTANT_M6)
        {
            u32 used = stale_cd;
            stale_cd = cd_work;
            cd_work = used;
        }
#endif
#if defined(WM_712D0_TEST_TRACE)
        cd_guard++;
        if (cd_guard > 32)
            break;
#endif
#if defined(WM_712D0_MUTANT_M5)
        (void)cd_work;
        WM_FP_TRACE(0x80071404u, WM_FP_TRACE_BR, 0x8007141Cu, 0u, 1u);
        break;
#elif defined(WM_712D0_MUTANT_M8)
        if (cd_work != 0u)
#else
        if (cd_work != 3u)
#endif
        {
            WM_FP_TRACE(0x80071404u, WM_FP_TRACE_BR, 0x8007141Cu, 0u, 1u);
            break;
        }
        (void)VSync(0);
        WM_FP_TRACE(0x8007140Cu, WM_FP_TRACE_CALL, WM_FP_CALL_VSYNC, 0u, 0u);
        s_fp_vsync_retries++;
        WM_FP_TRACE(0x80071414u, WM_FP_TRACE_BR, 0x800713FCu, 0u, 1u);
    }

    (void)CdSync(1, (u8*)PSX_ADDR(WM_FP_CDSYNC_BUF));
    WM_FP_TRACE(0x80071424u, WM_FP_TRACE_CALL, WM_FP_CALL_CDSYNC, 4u,
                WM_FP_CDSYNC_BUF);

    db_ptr = wm_fp_load_u32(WM_FP_DB_PTR);
    WM_FP_TRACE(0x80071430u, WM_FP_TRACE_LW, WM_FP_DB_PTR, 4u, db_ptr);
    env = WM_FP_ENVREC0;
#if defined(WM_712D0_MUTANT_M11)
    if (db_ptr != env)
#else
    if (db_ptr == env)
#endif
        env += WM_FP_ENV_STRIDE;

    ot = wm_fp_load_u32(env + WM_FP_OT_OFF);
    WM_FP_TRACE(0x80071448u, WM_FP_TRACE_LW, env + WM_FP_OT_OFF, 4u, ot);
    index = wm_fp_load_u32(WM_FP_INDEX);
    WM_FP_TRACE(0x80071450u, WM_FP_TRACE_LW, WM_FP_INDEX, 4u, index);

#if defined(WM_712D0_MUTANT_M14)
    /* Invert sltiu result: index 1 must become 0, mutant stores 1. */
    flipped = (index < 1u) ? 0u : 1u;
#else
    flipped = (index < 1u) ? 1u : 0u;
#endif

#if defined(WM_712D0_MUTANT_M7)
    (void)ClearOTagR((u32*)(uintptr_t)ot, WM_FP_OT_COUNT);
    WM_FP_TRACE(0x80071468u, WM_FP_TRACE_CALL, WM_FP_CALL_CLEAROTAG, 4u, ot);
    wm_fp_store_u32(WM_FP_DB_PTR, env);
    WM_FP_TRACE(0x80071458u, WM_FP_TRACE_SW, WM_FP_DB_PTR, 4u, env);
    wm_fp_store_u32(WM_FP_INDEX, flipped);
    WM_FP_TRACE(0x80071464u, WM_FP_TRACE_SW, WM_FP_INDEX, 4u, flipped);
#else
    wm_fp_store_u32(WM_FP_DB_PTR, env);
    WM_FP_TRACE(0x80071458u, WM_FP_TRACE_SW, WM_FP_DB_PTR, 4u, env);
    wm_fp_store_u32(WM_FP_INDEX, flipped);
    WM_FP_TRACE(0x80071464u, WM_FP_TRACE_SW, WM_FP_INDEX, 4u, flipped);
    (void)ClearOTagR((u32*)(uintptr_t)ot, WM_FP_OT_COUNT);
    WM_FP_TRACE(0x80071468u, WM_FP_TRACE_CALL, WM_FP_CALL_CLEAROTAG, 4u, ot);
#endif

    index = wm_fp_load_u32(WM_FP_INDEX);
    WM_FP_TRACE(0x80071474u, WM_FP_TRACE_LW, WM_FP_INDEX, 4u, index);
    func_800250E0((int)index);
    WM_FP_TRACE(0x80071478u, WM_FP_TRACE_CALL, WM_FP_CALL_250E0, 0u, index);

    func_8001D468();
    WM_FP_TRACE(0x80071480u, WM_FP_TRACE_CALL, WM_FP_CALL_1D468, 0u, 0u);

#if defined(WM_712D0_MUTANT_M9)
    wm_80097800();
    s_fp_scheduler_calls++;
    WM_FP_TRACE(0x80071488u, WM_FP_TRACE_CALL, WM_FP_CALL_97800, 0u, 1u);
#endif
#if defined(WM_712D0_MUTANT_M10)
    wm_80097800();
    wm_80097800();
    s_fp_scheduler_calls += 2;
    WM_FP_TRACE(0x80071488u, WM_FP_TRACE_CALL, WM_FP_CALL_97800, 0u, 2u);
#endif

    s_fp_cut_pc = WM_FRAME_PROLOGUE_CUT;
    fprintf(stderr,
            "[worldmap-frame-prologue] HARD CUT before 0x%08x "
            "cd_work_calls=%d vsync_retries=%d pad_iters=%d "
            "scheduler_calls=%d\n",
            WM_FRAME_PROLOGUE_CUT, s_fp_cd_work_calls, s_fp_vsync_retries,
            s_fp_pad_iters, s_fp_scheduler_calls);
}
