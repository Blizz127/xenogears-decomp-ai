/* xg_plat/timing.h -- timing interface: game speed, fast-forward and the
 * vertical-blank clock.  Backend: PsyCross (src/plat/xg_plat_psycross.c).
 * Speed is a host wall-clock multiplier only; no emulated register or game
 * state changes. */
#ifndef XG_PLAT_TIMING_H
#define XG_PLAT_TIMING_H

#ifdef __cplusplus
extern "C" {
#endif

#define XG_PLAT_SPEED_MIN 1
#define XG_PLAT_SPEED_MAX 5

int  xg_plat_timing_speed(void);              /* current multiplier, 1..5 */
void xg_plat_timing_set_speed(int multiplier); /* clamped to 1..5 */
void xg_plat_timing_fast_forward(int held);    /* held: run at the fast-forward speed */
void xg_plat_timing_set_fast_forward_speed(int multiplier); /* 1..5, default 5 */
int  xg_plat_timing_vblank_count(void);        /* host vblanks since start */

#ifdef __cplusplus
}
#endif

#endif
