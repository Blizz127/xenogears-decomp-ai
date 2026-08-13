/*
 * World-map frame-driver prologue (W34B18-B).
 *
 * Bounded retail slice 0x800712D0 .. 0x80071484 inclusive.
 * Hard-cut before 0x80071488 (jal 0x80097800, second scheduler pass).
 */
#ifndef WORLD_MAP_FRAME_DRIVER_H
#define WORLD_MAP_FRAME_DRIVER_H

#include "common.h"

#define WM_FRAME_PROLOGUE_ENTRY 0x800712D0u
#define WM_FRAME_PROLOGUE_LAST  0x80071484u
#define WM_FRAME_PROLOGUE_CUT   0x80071488u

void wm_800712D0_frame_prologue(void);

void wm_fp_reset(void);
int  wm_fp_get_entry(void);
int  wm_fp_get_cd_work_calls(void);
int  wm_fp_get_vsync_retries(void);
int  wm_fp_get_pad_iters(void);
int  wm_fp_get_scheduler_calls(void);
u32  wm_fp_get_cut_pc(void);

#endif /* WORLD_MAP_FRAME_DRIVER_H */
