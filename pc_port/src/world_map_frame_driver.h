/*
 * World-map frame-driver (W34B18-B prologue + W34B19-B second scheduler).
 *
 * Bounded retail slice 0x800712D0 .. 0x800714C4 inclusive.
 * Executes jal 0x80097800 / nop at 0x80071488 / 0x8007148C.
 * Executes post-pass sync/display setup through 0x800714C4.
 * Hard-cut before the gated update branch at 0x800714D4.
 */
#ifndef WORLD_MAP_FRAME_DRIVER_H
#define WORLD_MAP_FRAME_DRIVER_H

#include "common.h"

#define WM_FRAME_PROLOGUE_ENTRY 0x800712D0u
#define WM_FRAME_PROLOGUE_LAST  0x800714C4u
#define WM_FRAME_PROLOGUE_CUT   0x800714D4u

void wm_800712D0_frame_prologue(void);

void wm_fp_reset(void);
int  wm_fp_get_entry(void);
int  wm_fp_get_cd_work_calls(void);
int  wm_fp_get_vsync_retries(void);
int  wm_fp_get_pad_iters(void);
int  wm_fp_get_scheduler_calls(void);
u32  wm_fp_get_cut_pc(void);

#endif /* WORLD_MAP_FRAME_DRIVER_H */
