/*
 * WorldMapMain main loop 0x80071034.
 * Mode dispatch + scheduler + frame driver + sync loop.
 */
#include <stdint.h>
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

void wm_80071034(void)
{
    while (1) {
        u32 mode = ml_lw(D_8009C5A8);
        u32 cb0_table = D_8009A05C;
        u32 cb1_table = D_8009A060;
        u32 cb0_addr, cb1_addr;
        void (*cb0)(void);
        void (*cb1)(void);

        /* Dispatch cb0 via table lookup */
        cb0_addr = ml_lw(cb0_table + mode * 12);
        if (cb0_addr != 0) {
            cb0 = (void(*)(void))(uintptr_t)cb0_addr;
            cb0();
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
            cb1 = (void(*)(void))(uintptr_t)cb1_addr;
            cb1();
        }

        /* Check state counter — exit if < 2 */
        if (ml_lw(D_8009D7CC) < 2) {
            break;
        }
    }
}
