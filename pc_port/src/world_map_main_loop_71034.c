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
#include "world_map_main_loop_71034.h"
#include "world_map_frame_driver_712d0.h"

extern void DrawSync(void (*func)(unsigned long));
extern void Vsync(long mode);
extern void wm_80097800(void);

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

void wm_80071034(void)
{
    int frame;
    const int frame_limit = ml_frame_limit();

    for (frame = 1; frame <= frame_limit; frame++) {
        u32 mode = ml_lw(D_8009C5A8);
        u32 cb0_table = D_8009A05C;
        u32 cb1_table = D_8009A060;
        u32 cb0_addr, cb1_addr;

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

        /* Frame driver */
        wm_800712D0();

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

    fprintf(stderr, "[worldmap-open-loop] bounded exit frames=%d limit=%d\n",
            frame > frame_limit ? frame_limit : frame, frame_limit);
}
