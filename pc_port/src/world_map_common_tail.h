/*
 * World-map common-tail prefix + 0x80089160 initializer (W34B5A).
 *
 * Common-tail prefix: 0x8007290C–0x80072938.
 * Selector-dependent logic: C610=0 calls wm_80089160(14,0,0).
 * Reconvergence at 0x8007293C (first excluded = next-phase helper).
 *
 * 0x80089160: bounded table/state initializer 0x80089160–0x800893D4.
 * Leaf function, no direct calls. Record stride 672 bytes.
 */
#ifndef WORLD_MAP_COMMON_TAIL_H
#define WORLD_MAP_COMMON_TAIL_H

#include "common.h"

/* Retail cut PC constants. */
#define WM_COMMON_TAIL_P0_START      0x8007290Cu
#define WM_COMMON_TAIL_P0_CUT        0x8007293Cu  /* reconvergence / first excluded */
#define WM_COMMON_TAIL_P1_START      0x8007293Cu
#define WM_COMMON_TAIL_P1_CUT        0x80072944u  /* first excluded after wm_800978FC */
#define WM_80089160_START            0x80089160u
#define WM_80089160_END_EXCLUSIVE    0x800893D8u
#define WM_800978FC_START            0x800978FCu
#define WM_800978FC_END_EXCLUSIVE    0x800979C8u

/* Next-phase forbidden targets. */
#define WM_NEXT_HELPER_978FC         0x800978FCu
#define WM_NEXT_HELPER_8901C         0x8008901Cu
#define WM_NEXT_HELPER_865A0         0x800865A0u
#define WM_NEXT_HELPER_85FE0         0x80085FE0u
#define WM_NEXT_HELPER_75228         0x80075228u
#define WM_SCHEDULER_97800           0x80097800u
#define WM_WORLD_LOOP_712D0          0x800712D0u

/* D_80059179 written by the prefix. */
#define WM_D_80059179_ABS            0x80059179u

/* 0x80089160 constants. */
#define WM_89160_TABLE_BASE_PTR      0x8009BCC0u
#define WM_89160_RECORD_STRIDE       672u    /* 0x2A0 */
#define WM_89160_SUBRECORD_STRIDE    0x54u   /* 84 bytes */
#define WM_89160_SUBRECORD_COUNT     8
#define WM_89160_FLAG_BYTE_OFFSET    0x4Fu
#define WM_89160_FLAG_BIT            0x80u

/* Production functions. */

/* wm_80089160: full native implementation of retail 0x80089160.
 * Leaf function. Arguments: a0=record index, a1=src ptr, a2=src ptr.
 * For natural C610=0 path: (14, 0, 0). */
void wm_80089160(u32 a0, u32 a1, u32 a2);

/* wm_8007290C_common_tail_p0: bounded common-tail prefix.
 * Reads C610, dispatches to wm_80089160 for C610=0.
 * Returns the exact new cut PC (0x8007293C). */
u32 wm_8007290C_common_tail_p0(void);

/* Per-world-init reset. Call at each new world init boundary. */
void wm_common_tail_p0_reset(void);

/* Counter accessors. */
int  wm_ctp0_get_entry(void);
int  wm_ctp0_get_c610_zero(void);
int  wm_ctp0_get_c610_nonzero(void);
int  wm_ctp0_get_89160_calls(void);
u32  wm_ctp0_get_last_cut(void);
int  wm_ctp0_get_forbidden_978fc(void);
int  wm_ctp0_get_forbidden_scheduler(void);
int  wm_ctp0_get_forbidden_world_loop(void);
int  wm_ctp0_get_forbidden_8901c(void);
int  wm_ctp0_get_forbidden_865a0(void);
int  wm_ctp0_get_forbidden_85fe0(void);
int  wm_ctp0_get_forbidden_75228(void);

/* wm_800978FC: world-map graphics buffer allocator.
 * Allocates two 64 KB buffers, initializes first with repeating
 * byte pattern (2048 records × 32 bytes), copies to second.
 * Self-contained leaf — only calls HeapAlloc. */
void wm_800978FC(void);

/* wm_8007293C_common_tail_p1: caller slice from accepted P0 frontier.
 * Calls wm_800978FC exactly once.
 * Returns exact new cut PC (0x80072944).
 * Requires P0 to have executed and returned 0x8007293C. */
u32 wm_8007293C_common_tail_p1(void);

/* P1 per-world-init reset. */
void wm_common_tail_p1_reset(void);

/* P1 counter accessors. */
int  wm_ctp1_get_entry(void);
int  wm_ctp1_get_978fc_calls(void);
u32  wm_ctp1_get_last_cut(void);

/* 0x80089160 instrumentation. */
int  wm_89160_get_calls(void);
int  wm_89160_get_iterations(void);
void wm_89160_reset(void);

#endif /* WORLD_MAP_COMMON_TAIL_H */
