/*
 * WorldMapMain main loop 0x80071034.
 * Mode dispatch + scheduler + frame driver + sync loop. The first bounded
 * session resumes at 0x8007106C because initialization already completed the
 * slot-1 callback and 0x80071064 scheduler.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "test_input.h"
#include "world_map_capture.h"
#include "world_map_main_loop_71034.h"
#include "world_map_frame_driver_712d0.h"
#include "world_map_teardown_7299c.h"

extern void DrawSync(void (*func)(unsigned long));
extern void Vsync(long mode);
extern void PsyX_EndScene(void);
extern void ControllerResetState(void);
extern void wm_80097800(void);
extern u16 D_800AFE9C;

#define D_8009C5A8  0x8009C5A8u  /* mode index */
#define D_8009D7CC  0x8009D7CCu  /* state counter */
#define D_8009C894  0x8009C894u  /* ready flag */
#define D_8009A05C  0x8009A05Cu  /* cb0 table */
#define D_8009A060  0x8009A060u  /* cb1 table */

typedef struct MlOpenLoopContext {
    int frame_limit;
} MlOpenLoopContext;

static u32 ml_lw(u32 a) { u32 v; memcpy(&v, PSX_ADDR(a), 4); return v; }
static void ml_sw(u32 a, u32 v) { memcpy(PSX_ADDR(a), &v, 4); }

static int ml_frame_limit(void)
{
    const char* value = getenv("XENO_WORLD_FRAME_LIMIT");
    int limit = value != NULL ? atoi(value) : 600;
    return limit > 0 ? limit : 600;
}

static void ml_guest_stub(u32 address, int mode, int slot, const char* lane)
{
    fprintf(stderr,
            "[worldmap-stub] guest=0x%08x lane=%s mode=%d slot=%d "
            "default_return=0\n",
            address, lane, mode, slot);
}

/* The mode table contains guest function addresses. Never jalr a raw value.
 * The open-loop experiment deliberately turns unresolved mode handlers into
 * observable default-return stubs. */
static void ml_dispatch_guest(u32 address, int mode, int slot,
                              const char* lane)
{
    switch (address) {
    case 0x80071CDCu:
        fprintf(stderr,
                "[worldmap-stub] guest=0x%08x lane=%s mode=%d slot=%d "
                "already_initialized default_return=0\n",
                address, lane, mode, slot);
        break;
    case 0x80072238u:
        ml_guest_stub(address, mode, slot, lane);
        break;
    case 0x8007299Cu:
        wm_8007299C();
        break;
    default:
        ml_guest_stub(address, mode, slot, lane);
        break;
    }
}

#if defined(WM_7299C_PROD_TEST)
void wm_80071034_test_dispatch_slot2(void)
{
    ml_dispatch_guest(0x8007299Cu, 0, 2, "cb1");
}
#endif

static int ml_before_frame(int frame, void* user)
{
    const char* dir = getenv("XENO_CAPTURE_DIR");
    char path[512];

    (void)user;
    PcPort_TestInputAdvanceFrame();
    PcPort_TestInputInject(&D_800AFE9C);
    PcPort_WorldCaptureSetFrame(frame);

    if (dir == NULL || (frame % 60) != 0)
        return 0;
    snprintf(path, sizeof(path), "%s/world-frame-%06d.bmp", dir, frame);
    if (PcPort_WorldCaptureRequest(frame, path) != 0)
        return -1;
    fprintf(stderr, "[worldmap-open-loop] requested frame=%d path=%s\n",
            frame, path);
    return 0;
}

static int ml_after_frame(int frame, void* user)
{
    const MlOpenLoopContext* context = (const MlOpenLoopContext*)user;

    /* DrawOT opens the host scene, while retail's next Vsync would close it
     * only after taking the 0x800719C8 back-edge. Present at this bounded
     * harness seam so frame N remains the frame built and captured as N,
     * including the final frame at the limit. */
    PsyX_EndScene();
    if (PcPort_WorldCaptureFrameComplete(frame) != 0) {
        fprintf(stderr,
                "[worldmap-open-loop] capture fulfillment failed frame=%d\n",
                frame);
        return -1;
    }
    if ((frame % 60) == 0)
        fprintf(stderr, "[worldmap-open-loop] frame=%d/%d\n",
                frame, context->frame_limit);
    return 0;
}

void wm_80071034(void)
{
    int session = 1;
    const int frame_limit = ml_frame_limit();
    Wm712D0BoundedRun run;
    Wm712D0RunResult result;
    MlOpenLoopContext context;

    PcPort_WorldCaptureReset();
    context.frame_limit = frame_limit;
    run.frame_limit = frame_limit;
    run.displayed_frames = 0;
    run.before_frame = ml_before_frame;
    run.after_frame = ml_after_frame;
    run.user = &context;

    for (;;) {
        u32 mode = ml_lw(D_8009C5A8);
        u32 cb0_table = D_8009A05C;
        u32 cb1_table = D_8009A060;
        u32 cb0_addr, cb1_addr;

        /* The first session enters here from the already-completed retail
         * 0x80071064 scheduler gate. Only a later natural session repeats
         * slot 1 and that session-entry scheduler. */
        if (session != 1) {
            cb0_addr = ml_lw(cb0_table + mode * 12);
            if (cb0_addr != 0)
                ml_dispatch_guest(cb0_addr, (int)mode, session, "cb0");
            wm_80097800();
        }

        /* Retail 0x8007106C continuation. */
        DrawSync(NULL);
        Vsync(0);
        ControllerResetState();

        ml_sw(D_8009C894, ml_lw(D_8009D7CC));

        result = wm_800712D0_run_bounded(&run);
        if (result == WM_712D0_RUN_ERROR) {
            fprintf(stderr, "[worldmap-open-loop] frame driver failed\n");
            exit(EXIT_FAILURE);
        }
        if (result == WM_712D0_RUN_BOUNDED_EXIT)
            break;

        /* Retail natural session exit: slot 2 then the signed D7CC latch. */
        cb1_addr = ml_lw(cb1_table + mode * 12);
        if (cb1_addr != 0)
            ml_dispatch_guest(cb1_addr, (int)mode, session, "cb1");

        if ((s32)ml_lw(D_8009D7CC) < 2) {
            fprintf(stderr,
                    "[worldmap-open-loop] natural state exit frames=%d "
                    "D7CC=%u\n", run.displayed_frames,
                    ml_lw(D_8009D7CC));
            break;
        }
        session++;
    }

    if (PcPort_WorldCaptureFinish() != 0) {
        fprintf(stderr, "[worldmap-open-loop] capture pending at exit\n");
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, "[worldmap-open-loop] bounded exit frames=%d limit=%d\n",
            run.displayed_frames, frame_limit);
}
