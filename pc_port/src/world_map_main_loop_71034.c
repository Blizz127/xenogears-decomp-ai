/*
 * WorldMapMain main loop 0x80071034.
 * Mode dispatch + scheduler + frame driver + sync loop.
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

extern void DrawSync(void (*func)(unsigned long));
extern void Vsync(long mode);
extern void wm_80097800(void);
extern u16 D_800AFE9C;

#define D_8009C5A8  0x8009C5A8u  /* mode index */
#define D_8009D7CC  0x8009D7CCu  /* state counter */
#define D_8009C894  0x8009C894u  /* ready flag */
#define D_8009A05C  0x8009A05Cu  /* cb0 table */
#define D_8009A060  0x8009A060u  /* cb1 table */

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
    case 0x8007299Cu:
    default:
        ml_guest_stub(address, mode, slot, lane);
        break;
    }
}

static int ml_request_capture(int frame)
{
    const char* dir = getenv("XENO_CAPTURE_DIR");
    char path[512];

    if (dir == NULL || (frame % 60) != 0)
        return 0;
    snprintf(path, sizeof(path), "%s/world-frame-%06d.bmp", dir, frame);
    if (PcPort_WorldCaptureRequest(frame, path) != 0)
        return -1;
    fprintf(stderr, "[worldmap-open-loop] requested frame=%d path=%s\n",
            frame, path);
    return 0;
}

void wm_80071034(void)
{
    int frame;
    const int frame_limit = ml_frame_limit();

    PcPort_WorldCaptureReset();

    for (frame = 1; frame <= frame_limit; frame++) {
        u32 mode = ml_lw(D_8009C5A8);
        u32 cb0_table = D_8009A05C;
        u32 cb1_table = D_8009A060;
        u32 cb0_addr, cb1_addr;

        /* Continue the declarative input clock after the field-to-world handoff. */
        PcPort_TestInputAdvanceFrame();
        PcPort_TestInputInject(&D_800AFE9C);
        PcPort_WorldCaptureSetFrame(frame);

        /* Dispatch cb0 via table lookup */
        cb0_addr = ml_lw(cb0_table + mode * 12);
        if (cb0_addr != 0) {
            ml_dispatch_guest(cb0_addr, (int)mode, frame, "cb0");
        }

        /* Scheduler */
        wm_80097800();

        /* DrawSync + Vsync */
        DrawSync(NULL);
        Vsync(0);

        /* Copy state */
        ml_sw(D_8009C894, ml_lw(D_8009D7CC));

        if (ml_request_capture(frame) != 0) {
            fprintf(stderr, "[worldmap-open-loop] capture request failed\n");
            exit(EXIT_FAILURE);
        }

        /* Frame driver */
        wm_800712D0();

        if (PcPort_WorldCaptureFrameComplete(frame) != 0) {
            fprintf(stderr,
                    "[worldmap-open-loop] capture fulfillment failed frame=%d\n",
                    frame);
            exit(EXIT_FAILURE);
        }

        /* Dispatch cb1 via table lookup */
        cb1_addr = ml_lw(cb1_table + mode * 12);
        if (cb1_addr != 0) {
            ml_dispatch_guest(cb1_addr, (int)mode, frame, "cb1");
        }

        /* Check state counter — exit if < 2 */
        if (ml_lw(D_8009D7CC) < 2) {
            fprintf(stderr,
                    "[worldmap-open-loop] natural state exit frame=%d "
                    "D7CC=%u\n", frame, ml_lw(D_8009D7CC));
            break;
        }

        if ((frame % 60) == 0)
            fprintf(stderr, "[worldmap-open-loop] frame=%d/%d\n",
                    frame, frame_limit);
    }

    if (PcPort_WorldCaptureFinish() != 0) {
        fprintf(stderr, "[worldmap-open-loop] capture pending at exit\n");
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, "[worldmap-open-loop] bounded exit frames=%d limit=%d\n",
            frame > frame_limit ? frame_limit : frame, frame_limit);
}
