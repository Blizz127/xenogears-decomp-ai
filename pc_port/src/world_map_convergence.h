/*
 * World-map convergence P1 module (W34B1).
 *
 * Extracted from world_map_init.c so that both the production game build
 * and the production-linked test link the same authoritative object.
 *
 * Retail slice: 0x800726C0–0x80072728.
 */
#ifndef WORLD_MAP_CONVERGENCE_H
#define WORLD_MAP_CONVERGENCE_H

#include "common.h"

typedef u32 wm_conv_p1_next_t;

/* Production function — the one authoritative implementation. */
wm_conv_p1_next_t wm_800726C0_convergence_p1(void);

/* Pool registration helper (called by P1). */
void wm_pool_register(u32 a0, u32 a1);

/* Reset all per-world-init counters. Call at each new world init boundary. */
void wm_conv_p1_reset(void);

/* Counter accessors for instrumentation dump and test assertions. */
int wm_conv_p1_get_entry(void);
int wm_conv_p1_get_flag0(void);
int wm_conv_p1_get_flag1_cut(void);
int wm_conv_p1_get_other_cut(void);
int wm_conv_p1_get_empty(void);
int wm_conv_p1_get_iterations(void);
int wm_conv_p1_get_helper_calls(void);
int wm_conv_p1_get_cut_second_table(void);
u32 wm_conv_p1_get_last_next(void);
int wm_conv_p1_get_forbidden_c610_read(void);
int wm_conv_p1_get_forbidden_a034_read(void);
int wm_conv_p1_get_forbidden_976fc(void);
int wm_conv_p1_get_forbidden_common_tail(void);
int wm_conv_p1_get_forbidden_excluded_instr(void);
int wm_conv_p1_get_pool_alloc(void);

#endif /* WORLD_MAP_CONVERGENCE_H */
