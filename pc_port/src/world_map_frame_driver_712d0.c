/*
 * World-map frame driver 0x800712D0.
 * Session prologue, recurring render/update loop, and natural session exit.
 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "system/controller.h"
#include "world_map_frame_driver_712d0.h"
#include "world_map_image_transfer_25044.h"
#include "world_map_helper_96130.h"
#include "world_map_helper_97244.h"
#include "world_map_helper_9623c.h"
#include "world_map_helper_981c8.h"
#include "world_map_helper_983a0.h"
#include "world_map_helper_98cc0.h"
#include "world_map_helper_9932c.h"
#include "world_map_helper_737ec.h"
#include "world_map_helper_740b8.h"
#include "world_map_helper_747dc.h"
#include "world_map_helper_848f4.h"
#include "world_map_helper_85cdc.h"
#include "world_map_helper_8615c.h"
#include "world_map_helper_86798.h"
#include "world_map_helper_89748.h"
#include "world_map_helper_89c78.h"
#include "world_map_helper_93f18.h"
#include "world_map_helper_73b04.h"
#include "world_map_helper_75d4c.h"
#include "world_map_helper_762fc.h"
#include "world_map_menu_lifecycle.h"
#include "world_map_r4world_71a58.h"
#include "world_map_ot_adapter.h"
#include "world_map_upload_pump_74f2c.h"
#include "world_map_upload_pump_75104.h"

/* PsyQ functions */
extern void DrawSync(void (*func)(unsigned long));
extern void Vsync(long mode);
extern void PutDrawEnv(void *env);
extern void PutDispEnv(void *env);
extern void SetGeomOffset(long ofx, long ofy);
extern void MoveImage(void *rect, long x, long y);
extern long CdSync(long mode, u_char *result);
extern void wm_80097800(void);

/* Unresolved leaf stubs: preserve control flow, return the retail-neutral
 * default, and leave an address-tagged work queue in the log. */
static void wm_712d0_stub(const char* name, u32 guest_pc)
{
    fprintf(stderr,
            "[worldmap-stub] guest=0x%08x name=%s default_return=0\n",
            guest_pc, name);
}

static void wm_80075E7C(void) { wm_712d0_stub("80075E7C", 0x80075E7Cu); }
extern void func_800250E0(int context);
extern void func_8001D468(void);
extern int ControllerPopState(int port);
extern int ControllerGetType(int port);
extern void ResetGraph(int mode);
extern void GameCheckAndHandleSoftReset(void);
extern void MenuMain(void);

/* Global addresses */
#define D_8009BE3C  0x8009BE3Cu
#define D_8009D7F0  0x8009D7F0u
#define D_8009D554  0x8009D554u
#define D_8009BD1C  0x8009BD1Cu
#define D_8009BD14  0x8009BD14u
#define D_8009CD50  0x8009CD50u
#define D_8009BD18  0x8009BD18u
#define D_8009BD10  0x8009BD10u
#define D_8009CD4C  0x8009CD4Cu
#define D_8009BD34  0x8009BD34u
#define D_8009C178  0x8009C178u
#define D_8009D804  0x8009D804u
#define D_8009BD24  0x8009BD24u
#define D_8009CE68  0x8009CE68u
#define D_8009BE10  0x8009BE10u
#define D_8009BE0C  0x8009BE0Cu
#define D_8009D80C  0x8009D80Cu
#define D_8009D558  0x8009D558u
#define D_8006954C  0x8006954Cu
#define D_80069179  0x80069179u
#define D_80069178  0x80069178u
#define D_80069171  0x80069171u
#define D_80069460  0x80069460u
#define D_8006EE76  0x8006EE76u
#define D_8007EE70  0x8007EE70u
#define D_8007EE68  0x8007EE68u
#define D_8009D7CC  0x8009D7CCu
#define D_8009D7D8  0x8009D7D8u
#define D_8009D55C  0x8009D55Cu
#define D_8006F8E5  0x8006F8E5u
#define D_8009B6E4  0x8009B6E4u
#define D_8009BC9C  0x8009BC9Cu
#define D_8009C588  0x8009C588u
#define D_8009B6E4  0x8009B6E4u
#define D_8009EF64  0x8009EF64u

static u32 fd_lw(u32 a) { u32 v; memcpy(&v, PSX_ADDR(a), 4); return v; }
static void fd_sw(u32 a, u32 v) { memcpy(PSX_ADDR(a), &v, 4); }
static u16 fd_lhu(u32 a) { u16 v; memcpy(&v, PSX_ADDR(a), 2); return v; }
static s16 fd_lh(u32 a) { s16 v; memcpy(&v, PSX_ADDR(a), 2); return v; }
static void fd_sh(u32 a, u16 v) { memcpy(PSX_ADDR(a), &v, 2); }
static u8 fd_lbu(u32 a) { return *(u8*)PSX_ADDR(a); }
static void fd_sb(u32 a, u8 v) { *(u8*)PSX_ADDR(a) = v; }

/* Retail 0x80071488: the post-clear scheduler pass which republishes the
 * current frame's packets before DrawOTag. Kept as a narrow test seam so the
 * actual linked driver cannot silently regress to a local no-op again. */
void wm_712d0_run_second_scheduler(void)
{
#if defined(WM_712D0_MUTANT_NO_SECOND_SCHEDULER)
    return;
#elif defined(WM_712D0_MUTANT_DOUBLE_SECOND_SCHEDULER)
    wm_80097800();
    wm_80097800();
#else
    wm_80097800();
#endif
}

static void* wm_712d0_map_guest(u32 value, const char* call, u32 pc)
{
    if (value == 0u || value < 0x80000000u || value >= 0x80300000u) {
        fprintf(stderr,
                "[worldmap-safety] %s unknown guest=0x%08x at 0x%08x "
                "skip\n", call, value, pc);
        return NULL;
    }
    return PSX_ADDR(value);
}

Wm712D0RunResult wm_800712D0_run_bounded(Wm712D0BoundedRun* run)
{
    u32 draw_env_ptr;
    u32 ot_ptr;
    s32 controller_result;
    s32 queue_result;
    int frame;

    if (run == NULL || run->frame_limit <= 0 ||
            run->displayed_frames < 0 ||
            run->displayed_frames >= run->frame_limit) {
        return WM_712D0_RUN_ERROR;
    }

    /* Retail 0x800712D0..0x80071308: once per world session. */
    draw_env_ptr = 0x8009BC40;
    fd_sw(D_8009BE3C, draw_env_ptr);
    fd_sw(D_8009D7F0, 1);
    fd_sw(D_8009D554, 1);

    for (;;) {
        frame = run->displayed_frames + 1;
        if (run->before_frame != NULL &&
                run->before_frame(frame, run->user) != 0) {
            return WM_712D0_RUN_ERROR;
        }

        /* Retail 0x8007130C: recurring inner-frame head. */
        fd_sh(D_8009BD1C, 0);
        fd_sh(D_8009BD14, 0);
        fd_sh(D_8009CD50, 0);
        fd_sh(D_8009BD18, 0);
        fd_sh(D_8009BD10, 0);
        fd_sh(D_8009CD4C, 0);

        /* Controller polling loop */
        do {
            controller_result = ControllerPopState(0);
            if (controller_result != 0) {
                /* Merge controller input into global state */
                u16 buttons = fd_lhu(D_8009CD4C);
                u16 raw_btn = (u16)g_C1ButtonState;
                u16 sticks = fd_lhu(D_8009CD50);
                u16 raw_stick = (u16)g_C2ButtonState;

                fd_sh(D_8009CD4C, buttons | raw_btn);
                fd_sh(D_8009CD50, sticks | raw_stick);

                buttons = fd_lhu(D_8009BD10);
                raw_btn = (u16)g_C1ButtonStateReleased;
                sticks = fd_lhu(D_8009BD14);
                raw_stick = (u16)g_C2ButtonStateReleased;

                fd_sh(D_8009BD10, buttons | raw_btn);
                fd_sh(D_8009BD14, sticks | raw_stick);

                buttons = fd_lhu(D_8009BD18);
                raw_btn = (u16)g_C1ButtonStatePressedOnce;
                sticks = fd_lhu(D_8009BD1C);
                raw_stick = (u16)g_C2ButtonStatePressedOnce;

                fd_sh(D_8009BD18, buttons | raw_btn);
                fd_sh(D_8009BD1C, sticks | raw_stick);
            }
        } while (controller_result != 0);

        /* Queue processing with Vsync wait */
        do {
            wm_800967E4();
            queue_result = 0;
            if (queue_result == 3) {
                Vsync(0);
            }
        } while (queue_result == 3);

        /* CD sync */
        CdSync(1, NULL);

        /* OT pointer management */
        {
            u32 env = fd_lw(D_8009BE3C);
            u32 alt_env = 0x8009BBC8;
            if (env == alt_env) {
                alt_env += 0x78;
            }
            ot_ptr = fd_lw(alt_env + 0x70);
            fd_sw(D_8009BE3C, alt_env);

            /* Toggle double-buffer flag */
            fd_sw(D_8009D7F0, fd_lw(D_8009D7F0) < 1 ? 1 : 0);
        }

        /* Clear OT. PsyCross's ClearOTagR writes host-pointer links at its own
         * OT_TAG stride; the OT lives in guest RAM and is walked by the
         * guest-link adapter, so clear it in retail format. */
        {
            void* host_ot = wm_712d0_map_guest(ot_ptr, "ClearOTagR", 0x80071468u);
            if (host_ot != NULL)
                wm_ot_clear_r_guest(ot_ptr, 0x400u);
        }

        /* Process input */
        fd_lw(D_8009D7F0);
        func_800250E0((int)fd_lw(D_8009D7F0));

        /* Game state processing */
        func_8001D468();

        /* Scheduler */
        wm_712d0_run_second_scheduler();

        /* DrawSync */
        DrawSync(NULL);

        /* Vsync */
        Vsync(2);

        /* Soft reset check */
        GameCheckAndHandleSoftReset();

        /* Display environment */
        {
            u32 env = fd_lw(D_8009BE3C);
            void* host_env = wm_712d0_map_guest(env, "PutDrawEnv", 0x800714ACu);
            if (host_env != NULL) {
                PutDispEnv((u8*)host_env + 0x5Cu);
                PutDrawEnv(host_env);
            }
        }

        /* State-dependent rendering */
        if (fd_lbu(D_80069179) == 0 && fd_lw(D_8009BD34) != 0 &&
                fd_lw(D_8009C178) == 0 && fd_lw(D_8009D804) == 0 &&
                fd_lh(D_8009BD24) == -1 && fd_lh(D_8009CE68) == -1 &&
                fd_lw(D_8009D554) != 0 && fd_lw(D_8009D80C) == 0) {

            /* World-map R4_WORLD callback */
            s32 area_result = wm_80093F18(D_8009D55C);
            if ((s16)(area_result & 0xFFFF) != 4) {
                /* Copy presence bytes */
                s32 i;
                for (i = 0; i < 3; i++) {
                    u8 pres = fd_lbu(D_8006F8E5 + (u32)i);
                    fd_sh(D_8007EE70 + i * 2, (u16)pres);
                }

                /* Check animation state */
                {
                    u8 anim = fd_lbu(D_8006F8E5);
                    if (anim == 0) {
                        fd_sb(D_8006F8E5 + 2, 0);
                        fd_sb(D_8006F8E5 + 1, 0);
                        fd_sb(D_8006F8E5, 0);
                    } else {
                        /* Check table entries */
                        u32 table_base = 0x8007D940;
                        u8 entry = fd_lbu(table_base + (u32)anim * 0x154);
                        if (entry != 0xFF) {
                            fd_sb(D_8006F8E5, 1);
                        }
                        entry = fd_lbu(table_base + (u32)(anim + 1) * 0x154);
                        if (entry != 0xFF) {
                            fd_sb(D_8006F8E5 + 1, 1);
                        }
                        entry = fd_lbu(table_base + (u32)(anim + 2) * 0x154);
                        if (entry != 0xFF) {
                            fd_sb(D_8006F8E5 + 2, 1);
                        }
                    }
                }

                /* Call wm_80075D4C */
                wm_80075D4C();
            }

            fd_sw(D_8009BD34, 0);
        }

        /* Menu check */
        if (fd_lw(D_8009C178) == 0 && fd_lw(D_8009D804) != 0 &&
                fd_lw(D_8009D554) != 0 && fd_lw(D_8009D80C) == 0) {

            if (fd_lhu(D_8009BD10) & 0x800) {
                /* Toggle menu */
                u16* toggle = (u16*)PSX_ADDR(D_8006EE76);
                *toggle ^= 1;
            }
        }

        /* State machine dispatch */
        if (fd_lw(D_8009C178) == 0 && fd_lw(D_8009D804) != 0 &&
                fd_lw(D_8009D554) != 0) {
            s32 mode = fd_lw(D_8009BE10);

            if (mode <= 0) {
                /* Idle: do nothing */
            } else if (mode < 4) {
                /* Active rendering modes */
                wm_800758C0();
                fd_sb(D_80069460, 0);
                fd_sb(D_80069178, 0);
                fd_sb(D_80069171, 1);
                wm_800762FC();
                MenuMain();
                wm_800762FC();
                wm_80075B58();
            } else if (mode < 8) {
                /* Transition modes */
                u16 flags = fd_lhu(D_8007EE68);
                u32 table = 0x8009B6E4;
                fd_sw(D_8009D554, 0);
                fd_sw(D_8009D7CC, 0);
                fd_sw(D_8009D7D8, table);
                flags |= 0x2000;
                fd_sh(D_8007EE68, flags);
            }
        }

        /* Reset boundary flag */
        fd_sw(D_8009D804, 0);

        /* Render pipeline */
        wm_80025044_guest_safe();
        (void)wm_80074F2C();
        (void)wm_80075104();

        /* Geometry offset */
        SetGeomOffset(0xA0, fd_lw(D_8009BE0C));

        /* DrawOTag */
        {
            u32 env = fd_lw(D_8009BE3C);
            u32 guest_ot = fd_lw(env + 0x70);
            void* host_ot = wm_712d0_map_guest(guest_ot + 0xFFCu,
                    "DrawOTag", 0x800719B4u);
            if (host_ot != NULL)
                (void)wm_ot_draw_otag_guest(guest_ot + 0xFFCu);
        }

        /* Retail 0x800719C8: the only recurring-frame back-edge. */
        run->displayed_frames = frame;
        if (run->after_frame != NULL &&
                run->after_frame(frame, run->user) != 0) {
            return WM_712D0_RUN_ERROR;
        }
        if (fd_lw(D_8009D554) == 0u)
            break;
        if (run->displayed_frames >= run->frame_limit)
            return WM_712D0_RUN_BOUNDED_EXIT;
    }

    /* Retail 0x800719D0..0x80071A4C: natural session exit only. */
    ResetGraph(1);

    /* Final display environment */
    if (fd_lw(D_8009D7F0) == 0) {
        /* Move image for double buffering — RECT at stack */
        s16 rect[4] = {0, 0xD8, 0x140, 0xD8};
        MoveImage((void*)rect, 0, 0);
    }

    /* Final sync */
    wm_80096694();
    DrawSync(NULL);
    Vsync(0);

    /* Final display environment */
    {
        void* host_env = wm_712d0_map_guest(0x8009BC9Cu,
                "PutDispEnv", 0x80071A00u);
        if (host_env != NULL)
            PutDispEnv(host_env);
    }

    return WM_712D0_RUN_NATURAL_EXIT;
}

void wm_800712D0(void)
{
    Wm712D0BoundedRun run;

    run.frame_limit = 600;
    run.displayed_frames = 0;
    run.before_frame = NULL;
    run.after_frame = NULL;
    run.user = NULL;
    (void)wm_800712D0_run_bounded(&run);
}
