/* Production-linked certificate for the retail world-session/frame cadence. */
#define _POSIX_C_SOURCE 200809L

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "system/controller.h"
#include "world_map_capture.h"
#include "world_map_main_loop_71034.h"

#define D_8009C5A8 UINT32_C(0x8009C5A8)
#define D_8009D7CC UINT32_C(0x8009D7CC)
#define OT_A UINT32_C(0x800A2228)
#define OT_B UINT32_C(0x800A3230)

uint8_t g_PsxRam[PSX_RAM_SIZE];
uint8_t g_PsxScratchpad[4096];

u16 D_800AFE9C;
u_short g_C1ButtonState;
u_short g_C2ButtonState;
u_short g_C1ButtonStateReleased;
u_short g_C2ButtonStateReleased;
u_short g_C1ButtonStatePressedOnce;
u_short g_C2ButtonStatePressedOnce;

static int s_scheduler_calls;
static int s_outer_scheduler_calls;
static int s_inner_scheduler_calls;
static int s_scheduler_since_draw;
static int s_scheduler_order_errors;
static int s_draw_calls;
static int s_ot_errors;
static int s_input_advances;
static int s_input_injections;
static int s_controller_resets;
static int s_reset_graph_calls;
static int s_slot1_calls;
static int s_slot2_calls;
static int s_queue_barrier_calls;
static int s_250e0_calls;
static int s_1d468_calls;
static int s_25044_calls;
static int s_74f2c_calls;
static int s_75104_calls;
static int s_frame_seam_stage;
static int s_frame_seam_order_errors;
static int s_scene_open;
static int s_effective_presents;
static int s_screenshot_calls;
static int s_screenshot_frames[2];
static char s_screenshot_paths[2][512];

/* The cadence certificate never enters menu modes; satisfy the production
 * driver's now-shared lifecycle symbols without exercising that separate
 * certificate here. */
void wm_800758C0(void) {}
void wm_80075B58(void) {}

static void write_u32(u32 address, u32 value)
{
    memcpy(PSX_ADDR(address), &value, sizeof(value));
}

static int assertion_int(const char* name, int expected, int actual)
{
    if (expected != actual) {
        fprintf(stderr, "ASSERTION %s expected=%d actual=%d\n",
                name, expected, actual);
        return 0;
    }
    return 1;
}

static int assertion_true(const char* name, int condition)
{
    if (condition == 0) {
        fprintf(stderr, "ASSERTION %s\n", name);
        return 0;
    }
    return 1;
}

void wm_80097800(void)
{
    s_scheduler_calls++;
    s_scheduler_since_draw++;
    if (s_input_advances == s_draw_calls)
        s_outer_scheduler_calls++;
    else if (s_input_advances == s_draw_calls + 1)
        s_inner_scheduler_calls++;
    else
        s_scheduler_order_errors++;
}

void PcPort_TestInputAdvanceFrame(void)
{
    s_input_advances++;
}

void PcPort_TestInputInject(u16* held_buttons)
{
    s_input_injections++;
    *held_buttons = UINT16_C(0x5A5A);
}

void PsyX_TakeScreenshotPath(const char* path)
    __asm__("_Z23PsyX_TakeScreenshotPathPKc");

void PsyX_TakeScreenshotPath(const char* path)
{
    if (s_screenshot_calls < 2) {
        s_screenshot_frames[s_screenshot_calls] = s_input_advances;
        (void)snprintf(s_screenshot_paths[s_screenshot_calls],
                       sizeof(s_screenshot_paths[s_screenshot_calls]),
                       "%s", path);
    }
    s_screenshot_calls++;
}

int DrawSync(int mode)
{
    (void)mode;
    return 0;
}

void PsyX_EndScene(void)
{
    if (s_scene_open != 0) {
        PcPort_WorldCaptureFulfillAtPresent();
        s_scene_open = 0;
        s_effective_presents++;
    }
}

void Vsync(long mode)
{
    (void)mode;
    PsyX_EndScene();
}

int VSync(int mode)
{
    (void)mode;
    return 0;
}

void EnterCriticalSection(void)
{
}

void FlushCache(void)
{
}

void ExitCriticalSection(void)
{
}

void ControllerResetState(void)
{
    s_controller_resets++;
}

int ControllerPopState(int port)
{
    return port == 0 ? 0 : -1;
}

long CdSync(long mode, u_char* result)
{
    (void)mode;
    (void)result;
    return 0;
}

void GameCheckAndHandleSoftReset(void) {}
void MenuMain(void) {}
void PutDispEnv(void* env) { (void)env; }
void PutDrawEnv(void* env) { (void)env; }
void SetGeomOffset(long ofx, long ofy)
{
    (void)ofx;
    (void)ofy;
}
void MoveImage(void* rect, long x, long y)
{
    (void)rect;
    (void)x;
    (void)y;
}
void ResetGraph(int mode)
{
    (void)mode;
    s_reset_graph_calls++;
    PsyX_EndScene();
}

void wm_800967E4(void) {}
void wm_80096694(void) { s_queue_barrier_calls++; }
void func_800250E0(int context)
{
    (void)context;
    if (s_frame_seam_stage != 0)
        s_frame_seam_order_errors++;
    s_frame_seam_stage = 1;
    s_250e0_calls++;
}
void func_8001D468(void)
{
    if (s_frame_seam_stage != 1)
        s_frame_seam_order_errors++;
    s_frame_seam_stage = 2;
    s_1d468_calls++;
}
void wm_80025044_guest_safe(void)
{
    if (s_frame_seam_stage != 2)
        s_frame_seam_order_errors++;
    s_frame_seam_stage = 3;
    s_25044_calls++;
}
int wm_80074F2C(void)
{
    if (s_frame_seam_stage != 3)
        s_frame_seam_order_errors++;
    s_frame_seam_stage = 4;
    s_74f2c_calls++;
    return 0;
}
int wm_80075104(void)
{
    if (s_frame_seam_stage != 4)
        s_frame_seam_order_errors++;
    s_frame_seam_stage = 5;
    s_75104_calls++;
    return 0;
}
int wm_80072238(void)
{
    s_slot1_calls++;
    return 0;
}
void wm_8007299C(void) { s_slot2_calls++; }
s32 wm_80093F18(u32 vec_addr)
{
    (void)vec_addr;
    return 0;
}

void wm_ot_clear_r_guest(u32 ot_guest, u32 count)
{
    (void)ot_guest;
    (void)count;
}

int wm_ot_draw_otag_guest(u32 entry_guest)
{
    u32 expected = (s_draw_calls & 1) == 0 ? OT_A + UINT32_C(0xFFC)
                                           : OT_B + UINT32_C(0xFFC);
    int expected_schedulers = s_draw_calls == 0 ? 2 : 1;

    if (entry_guest != expected)
        s_ot_errors++;
    if (s_scheduler_since_draw != expected_schedulers)
        s_scheduler_order_errors++;
    if (s_frame_seam_stage != 5)
        s_frame_seam_order_errors++;
    s_frame_seam_stage = 0;
    s_scheduler_since_draw = 0;
    s_draw_calls++;
    s_scene_open = 1;
    return 1;
}

static void reset_fixture(void)
{
    memset(g_PsxRam, 0, sizeof(g_PsxRam));
    memset(g_PsxScratchpad, 0, sizeof(g_PsxScratchpad));
    write_u32(D_8009C5A8, 0u);
    write_u32(D_8009D7CC, 1u);
    write_u32(UINT32_C(0x8009A05C), UINT32_C(0x80072238));
    write_u32(UINT32_C(0x8009A060), UINT32_C(0x8007299C));
    write_u32(UINT32_C(0x8009BC38), OT_A);
    write_u32(UINT32_C(0x8009BCB0), OT_B);
}

int main(void)
{
    int ok = 1;

    reset_fixture();
    if (setenv("XENO_WORLD_FRAME_LIMIT", "120", 1) != 0 ||
        setenv("XENO_CAPTURE_DIR", "/tmp/w34c1-cadence", 1) != 0) {
        perror("setenv");
        return EXIT_FAILURE;
    }

    wm_80071034();

    ok &= assertion_int("session.slot1.exactly_once", 1, s_slot1_calls);
    ok &= assertion_int("cadence.outer_scheduler.exactly_once",
                        1, s_outer_scheduler_calls);
    ok &= assertion_int("frame_limit.displayed_frames.exactly_120",
                        120, s_draw_calls);
    ok &= assertion_int("cadence.inner_scheduler.once_per_displayed_frame",
                        120, s_inner_scheduler_calls);
    ok &= assertion_int("cadence.scheduler.total_outer_plus_inner",
                        121, s_scheduler_calls);
    ok &= assertion_int("cadence.event_order.outer_then_inner_draw",
                        0, s_scheduler_order_errors);
    ok &= assertion_int("frame_seams.250e0.once_per_frame",
                        120, s_250e0_calls);
    ok &= assertion_int("frame_seams.1d468.once_per_frame",
                        120, s_1d468_calls);
    ok &= assertion_int("frame_seams.25044.once_per_frame",
                        120, s_25044_calls);
    ok &= assertion_int("frame_seams.74f2c.once_per_frame",
                        120, s_74f2c_calls);
    ok &= assertion_int("frame_seams.75104.once_per_frame",
                        120, s_75104_calls);
    ok &= assertion_int("frame_seams.retail_order",
                        0, s_frame_seam_order_errors);
    ok &= assertion_int("driver.entry.once.ot_buffers_alternate",
                        0, s_ot_errors);
    ok &= assertion_int("input.clock.advance_once_per_inner_frame",
                        120, s_input_advances);
    ok &= assertion_int("input.clock.inject_once_per_inner_frame",
                        120, s_input_injections);
    ok &= assertion_int("session.outer_sync_reset.exactly_once",
                        1, s_controller_resets);
    ok &= assertion_int("bounded_exit.skips_natural_epilogue",
                        0, s_reset_graph_calls);
    ok &= assertion_int("bounded_exit.skips_slot2_teardown",
                        0, s_slot2_calls);
    ok &= assertion_int("bounded_exit.skips_queue_barrier",
                        0, s_queue_barrier_calls);
    ok &= assertion_int("capture.only_frames_60_and_120",
                        2, s_screenshot_calls);
    ok &= assertion_int("capture.frame60.request_equals_present60",
                        60, s_screenshot_frames[0]);
    ok &= assertion_int("capture.frame120.request_equals_present120",
                        120, s_screenshot_frames[1]);
    ok &= assertion_true("capture.frame60.path",
                         strstr(s_screenshot_paths[0],
                                "world-frame-000060.bmp") != NULL);
    ok &= assertion_true("capture.frame120.path",
                         strstr(s_screenshot_paths[1],
                                "world-frame-000120.bmp") != NULL);
    ok &= assertion_int("capture.last_fulfilled_frame_120", 120,
                        PcPort_WorldCaptureLastFulfilledFrame());
    ok &= assertion_int("capture.no_pending_at_bounded_exit", 0,
                        PcPort_WorldCapturePending());
    ok &= assertion_int("presentation.one_effective_swap_per_frame", 120,
                        s_effective_presents);
    ok &= assertion_int("bounded_exit.final_frame_presented", 0,
                        s_scene_open);

    if (ok == 0)
        return EXIT_FAILURE;
    puts("W34C1 scheduler cadence certificate PASS");
    return EXIT_SUCCESS;
}
