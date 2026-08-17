/*
 * World-map scheduler callback 0x80091C18 (slot-10 cb1).
 *
 * Register-faithful transcription of retail world_map.bin
 * [0x80091C18, 0x80091FF8).  See world_map_callback_91c18.h.
 */
#include <stdint.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_callback_91c18.h"
#include "world_map_helper_91ff8.h"
#include "world_map_helper_96f18.h"

#define POOL_PTR    0x8009BE24u
#define HEAD_MIRROR 0x8009BD3Au
#define D_8009D3F0  0x8009D3F0u
#define D_8009B214  0x8009B214u
#define D_8009B224  0x8009B224u
#define D_8009B234  0x8009B234u
#define D_8009BD38  0x8009BD38u
#define D_8009BD40  0x8009BD40u
#define D_8009BD48  0x8009BD48u
#define D_8009BD4A  0x8009BD4Au
#define D_8009BD4C  0x8009BD4Cu
#define D_8009BE28  0x8009BE28u

static s16 c18_lh(u32 a) { s16 v; memcpy(&v, PSX_ADDR(a), 2); return v; }
static u16 c18_lhu(u32 a) { u16 v; memcpy(&v, PSX_ADDR(a), 2); return v; }
static s32 c18_lw(u32 a) { s32 v; memcpy(&v, PSX_ADDR(a), 4); return v; }
static void c18_sh(u32 a, u16 v) { memcpy(PSX_ADDR(a), &v, 2); }
static void c18_sw(u32 a, s32 v) { memcpy(PSX_ADDR(a), &v, 4); }

s32 wm_80091C18(s32 slot_idx)
{
    u32 pool_ptr = (u32)c18_lw(POOL_PTR);
    u32 slot = pool_ptr + (u32)(slot_idx << 7);
    u32 s1; /* table pointer from slot[+0x64] */

    /* Pre-dispatch on slot[+0x04] */
    {
        u16 sub = c18_lhu(slot + 0x04);

        if (sub == 0x0A) {
            /* Set table pointers for area 0x0A */
            c18_sw(slot + 0x64, 0x8009B224u);
            c18_sw(slot + 0x68, 0x8009B234u);
            c18_sh(slot + 0x04, 0);
            if (c18_lw(slot + 0x50) == 0) {
                c18_sw(slot + 0x50, 1);
            }
        } else if (sub == 9) {
            /* Set table pointers for area 9 */
            c18_sw(slot + 0x64, 0x8009B22Cu);
            c18_sw(slot + 0x68, 0x8009B23Cu);
            c18_sh(slot + 0x04, 0);
            if (c18_lw(slot + 0x50) == 0) {
                c18_sw(slot + 0x50, 1);
            }
        } else if (sub == 0x0E) {
            /* Reset */
            c18_sh(slot + 0x04, 0);
            c18_sh(slot + 0x20, 0);
            c18_sw(D_8009D3F0, 0);
        } else if (sub == 0x11) {
            /* Set zoom */
            c18_sw(D_8009D3F0, 0x400000);
            c18_sh(slot + 0x20, 3);
            c18_sw(slot + 0x58, -0x100);
            c18_sh(slot + 0x04, 0);
        }
    }

    s1 = (u32)c18_lw(slot + 0x64);

    /* Main dispatch on slot[+0x20] */
    {
        s16 state = c18_lh(slot + 0x20);

        if (state == 0) {
            /* State 0: find camera candidate */
            s32 max = c18_lw(slot + 0x50);
            s32 result = wm_80091FF8(max, s1, c18_lw(slot + 0x68));

            if (c18_lw(slot + 0x50) != result) {
                /* New candidate found */
                c18_sh(slot + 0x20, 1);
                c18_sh(slot + 0x22, 0);
                c18_sw(slot + 0x50, result);
                c18_sw(slot + 0x54, c18_lw(D_8009B214 + result * 4));
                c18_sw(slot + 0x58, (s32)c18_lh(s1 + result * 2));
                c18_sw(slot + 0x60, (s32)(c18_lh(D_8009BD38) << 12));
            }

        } else if (state == 1) {
            /* State 1: periodic re-check + interpolation */
            if (c18_lh(slot + 0x22) >= 5) {
                /* Re-check every 5 frames */
                s32 max = c18_lw(slot + 0x50);
                s32 result = wm_80091FF8(max, s1, c18_lw(slot + 0x68));
                if (c18_lw(slot + 0x50) != result) {
                    c18_sw(slot + 0x50, result);
                    c18_sw(slot + 0x54, c18_lw(D_8009B214 + result * 4));
                    c18_sw(slot + 0x58, (s32)c18_lh(s1 + result * 2));
                }
                c18_sh(slot + 0x22, 0);
            }

            /* Interpolate D_8009D3F0 toward slot[+0x54] */
            {
                s32 target = c18_lw(slot + 0x54);
                s32 current = c18_lw(D_8009D3F0);
                if (target != current) {
                    s32 diff = target - current;
                    if (diff > 0) {
                        s32 step = diff >> 3;
                        if (step > 0x400000) step = 0x400000;
                        if (step < 0x40) {
                            c18_sw(D_8009D3F0, target);
                        } else {
                            c18_sw(D_8009D3F0, current + step);
                        }
                    } else {
                        c18_sw(D_8009D3F0, current - 0x2000);
                    }
                }
            }

            /* Interpolate heading toward slot[+0x58] */
            {
                s16 target_h = c18_lh(D_8009BD38);
                s32 target_val = c18_lw(slot + 0x58);
                if ((s32)target_h != target_val) {
                    s32 diff = target_val - (s32)target_h;
                    s32 step = (diff << 12) >> 5;
                    s32 new_acc = c18_lw(slot + 0x60) + step;
                    c18_sw(slot + 0x60, new_acc);
                    c18_sh(D_8009BD38, (u16)(new_acc >> 12));
                }
            }

            /* Check convergence */
            {
                s32 target = c18_lw(slot + 0x54);
                s32 current = c18_lw(D_8009D3F0);
                s32 target_h = c18_lw(slot + 0x58);
                s16 cur_h = c18_lh(D_8009BD38);
                if ((target ^ current) < 1 && (target_h ^ (s32)cur_h) < 1) {
                    c18_sh(slot + 0x20, 0);
                }
            }

        } else if (state == 2) {
            /* State 2: set camera Y from pool data */
            c18_sh(D_8009BD48, 0);
            c18_sh(D_8009BD4C, 0);
            c18_sh(D_8009BD4A, (u16)((s32)c18_lw(0x8009BE2Cu) >> 12));

        } else if (state == 3) {
            /* State 3: heading increment toward target */
            s16 cur_h = c18_lh(D_8009BD38);
            s32 target_h = c18_lw(slot + 0x58);
            if ((s32)cur_h != target_h) {
                c18_sh(D_8009BD38, (u16)(cur_h + 2));
            }
        }
    }

    /* Build view matrix (all paths) */
    {
        u32 out = D_8009BD40;
        u32 pos = D_8009BE28;
        u32 height = c18_lw(D_8009D3F0);
        u32 rot = D_8009BD38;
        wm_80096F18(out, pos, (s32)height, rot);
    }

    /* Increment frame counter */
    c18_sh(slot + 0x22, c18_lhu(slot + 0x22) + 1);

    return 1;
}
