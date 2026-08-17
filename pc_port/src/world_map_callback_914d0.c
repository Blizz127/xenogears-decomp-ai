/*
 * World-map scheduler callback 0x800914D0 (slot-9 cb1).
 *
 * Register-faithful transcription of retail world_map.bin
 * [0x800914D0, 0x80091B54).  See world_map_callback_914d0.h.
 */
#include <stdint.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_callback_914d0.h"
#include "world_map_helper_93354.h"
#include "world_map_helper_93484.h"
#include "world_map_helper_97770.h"

#define POOL_PTR    0x8009BE24u
#define HEAD_MIRROR 0x8009BD3Au
#define DIR_WORD    0x8009CD4Cu
#define D_8009D55C  0x8009D55Cu
#define D_8009D560  0x8009D560u
#define D_8009D564  0x8009D564u
#define D_8009BBB4  0x8009BBB4u
#define D_8009BBBC  0x8009BBBCu

static s16 d0_lh(u32 a) { s16 v; memcpy(&v, PSX_ADDR(a), 2); return v; }
static u16 d0_lhu(u32 a) { u16 v; memcpy(&v, PSX_ADDR(a), 2); return v; }
static s32 d0_lw(u32 a) { s32 v; memcpy(&v, PSX_ADDR(a), 4); return v; }
static void d0_sh(u32 a, u16 v) { memcpy(PSX_ADDR(a), &v, 2); }
static void d0_sw(u32 a, s32 v) { memcpy(PSX_ADDR(a), &v, 4); }

/* Heading interpolation helper: move current toward target by step */
static s32 heading_interp(s32 current, s32 target, s32 max_step)
{
    s32 diff = target - current;
    s32 abs_diff = diff < 0 ? -diff : diff;

    /* Wrap to [-0x800, 0x800] range */
    if (abs_diff > 0xC00) {
        if (diff < 0) diff += 0x1000;
        else diff -= 0x1000;
    }

    /* Clamp step */
    abs_diff = diff < 0 ? -diff : diff;
    if (abs_diff > max_step) {
        if (diff < 0) diff = -max_step;
        else diff = max_step;
    }

    return (current + diff) & 0xFFF;
}

s32 wm_800914D0(s32 slot_idx)
{
    u32 pool_ptr = (u32)d0_lw(POOL_PTR);
    u32 slot = pool_ptr + (u32)(slot_idx << 7);

    /* Pre-dispatch on slot[+0x04]-9 */
    {
        u16 sub = d0_lhu(slot + 0x04);
        s32 idx = (s32)(s16)(sub - 9);

        if (idx >= 0 && idx < 9) {
            switch (idx) {
            case 0: /* set main=2, target heading */
                d0_sh(slot + 0x04, 0);
                d0_sh(slot + 0x20, 2);
                d0_sw(slot + 0x50, (s32)d0_lh(HEAD_MIRROR));
                d0_sw(slot + 0x58, (s32)(d0_lh(HEAD_MIRROR) << 12));
                break;
            case 1: /* set main=0, store heading */
                d0_sh(slot + 0x04, 0);
                d0_sh(slot + 0x20, 0);
                d0_sw(slot + 0x50, (s32)d0_lh(HEAD_MIRROR));
                break;
            case 6: /* set main=0x10, heading offset -0x200 */
                d0_sh(slot + 0x20, 0x10);
                d0_sh(slot + 0x04, 0);
                d0_sw(slot + 0x50, 0x800);
                break;
            case 7: /* set main=0x10, heading offset +0x200 */
                d0_sh(slot + 0x20, 0x10);
                d0_sh(slot + 0x04, 0);
                d0_sw(slot + 0x50, 0xA00);
                break;
            case 8: /* set main=0x10, heading offset +0x400 */
                d0_sh(slot + 0x20, 0x10);
                d0_sh(slot + 0x04, 0);
                d0_sw(slot + 0x50, 0xC00);
                break;
            default: /* 2,3,4,5: fall through */
                break;
            }
        }
    }

    /* Main dispatch on slot[+0x20] */
    {
        s16 state = d0_lh(slot + 0x20);

        if (state == 0) {
            /* State 0: D-pad direction check */
            u16 dir = d0_lhu(DIR_WORD);
            u32 dir_bits = (dir >> 2) & 3;

            if (dir_bits == 1 || dir_bits == 3) {
                d0_sh(slot + 0x20, 1);
                d0_sw(slot + 0x54, -0x40);
                d0_sw(slot + 0x5C, (d0_lw(slot + 0x50) - 0x200) & 0xFFF);
            } else if (dir_bits == 2) {
                d0_sh(slot + 0x20, 1);
                d0_sw(slot + 0x54, 0x40);
                d0_sw(slot + 0x5C, (d0_lw(slot + 0x50) + 0x200) & 0xFFF);
            }

            /* Heading interpolation */
            {
                s32 target = d0_lw(slot + 0x50);
                s32 current = (s32)d0_lh(HEAD_MIRROR);
                if (current != target) {
                    s32 diff = target - current;
                    s32 abs_diff = diff < 0 ? -diff : diff;
                    if (abs_diff > 0xC00) {
                        if (diff < 0) diff += 0x1000;
                        else diff -= 0x1000;
                    }
                    /* Apply step: diff << 12 >> 3 = diff << 9 */
                    s32 step = (diff << 12) >> 3;
                    s32 new_acc = (d0_lw(slot + 0x58) + step) & 0xFFFFFFFFu;
                    d0_sw(slot + 0x58, new_acc);
                    d0_sh(HEAD_MIRROR, (u16)(new_acc >> 12));
                } else {
                    d0_sw(slot + 0x58, current << 12);
                }
            }

        } else if (state == 1) {
            /* State 1: smooth heading interpolation */
            s32 target = d0_lw(slot + 0x50);
            s32 speed = d0_lw(slot + 0x54);
            s32 new_heading = (target + speed) & 0xFFF;
            d0_sw(slot + 0x50, new_heading);

            if (new_heading != d0_lw(slot + 0x5C)) {
                /* Continue interpolation */
                s32 current = (s32)d0_lh(HEAD_MIRROR);
                s32 diff = new_heading - current;
                s32 abs_diff = diff < 0 ? -diff : diff;
                if (abs_diff > 0xC00) {
                    if (diff < 0) diff += 0x1000;
                    else diff -= 0x1000;
                }
                s32 step = (diff << 12) >> 3;
                s32 new_acc = (d0_lw(slot + 0x58) + step) & 0xFFFFFFFFu;
                d0_sw(slot + 0x58, new_acc);
                d0_sh(HEAD_MIRROR, (u16)(new_acc >> 12));
            } else {
                d0_sh(slot + 0x20, 0);
            }

        } else if (state == 2) {
            /* State 2: approach step */
            s32 target = d0_lw(slot + 0x50);
            s32 current = (s32)d0_lh(HEAD_MIRROR);
            s32 diff = target - current;
            s32 abs_diff = diff < 0 ? -diff : diff;

            /* Wrap */
            if (abs_diff > 0x800) {
                if (diff < 0) diff += 0x1000;
                else diff -= 0x1000;
            }

            /* Clamp to ±0x180 */
            abs_diff = diff < 0 ? -diff : diff;
            if (abs_diff > 0x180) {
                diff = diff < 0 ? 0x180 : -0x180;
            }

            /* Apply step: diff << 12 >> 3 */
            s32 step = (diff << 12) >> 3;
            s32 new_acc = (d0_lw(slot + 0x58) + step) & 0xFFFFFFFFu;
            d0_sw(slot + 0x58, new_acc);
            d0_sh(HEAD_MIRROR, (u16)(new_acc >> 12));

            /* Check if target reached and speed zero */
            if (((u32)d0_lh(HEAD_MIRROR) ^ (u32)target) < 1 &&
                d0_lw(slot + 0x60) == 0) {
                d0_sh(slot + 0x20, 3);
                wm_80097770(7, 0x0B);
            }

        } else if (state == 16) {
            /* State 16: fast heading interpolation */
            s32 target = d0_lw(slot + 0x50);
            s32 current = (s32)d0_lh(HEAD_MIRROR);
            s32 diff = target - current;
            s32 abs_diff = diff < 0 ? -diff : diff;

            if (abs_diff > 0x800) {
                if (diff < 0) diff += 0x1000;
                else diff -= 0x1000;
            }

            abs_diff = diff < 0 ? -diff : diff;
            if (abs_diff > 0x180) {
                diff = diff < 0 ? 0x180 : -0x180;
            }

            /* Apply step: diff << 12 >> 5 (faster) */
            s32 step = (diff << 12) >> 5;
            s32 new_acc = (d0_lw(slot + 0x58) + step) & 0xFFFFFFFFu;
            d0_sw(slot + 0x58, new_acc);
            d0_sh(HEAD_MIRROR, (u16)(new_acc >> 12));

            if (((u32)d0_lh(HEAD_MIRROR) ^ (u32)target) < 1 &&
                d0_lw(slot + 0x60) == 0) {
                d0_sh(slot + 0x20, 3);
            }
        }
    }

    /* Post-dispatch: position update for states 3 and 0x10 */
    {
        s16 state = d0_lh(slot + 0x20);
        if (state == 3 || state == 0x10) {
            u32 pose_block = D_8009D55C;
            s32 px = d0_lw(slot + 0x28);
            s32 py = d0_lw(slot + 0x2C);
            s32 pz = d0_lw(slot + 0x30);
            s32 bx = d0_lw(pose_block + 0);
            s32 by = d0_lw(pose_block + 4);
            s32 bz = d0_lw(pose_block + 8);

            if (px != bx || py != by || pz != bz) {
                /* Compute delta */
                s32 dx = bx - px;
                s32 dz = bz - pz;
                u32 delta_vec[3];
                delta_vec[0] = (u32)dx;
                delta_vec[1] = 0;
                delta_vec[2] = (u32)dz;

                /* Wrap delta */
                wm_80093484((u32)(uintptr_t)delta_vec);

                /* Scale delta >> 3 */
                dx = (s32)delta_vec[0] >> 3;
                dz = (s32)delta_vec[2] >> 3;

                if (dx < 0) dx = -dx;
                if (dz < 0) dz = -dz;

                if (dx < 0x40 && dz < 0x40) {
                    /* Close enough: snap to target */
                    d0_sw(slot + 0x60, 0);
                    d0_sw(D_8009BBB4, d0_lw(D_8009BBB4) + (s32)delta_vec[0]);
                    d0_sw(D_8009BBBC, d0_lw(D_8009BBBC) + (s32)delta_vec[2]);
                    d0_sw(slot + 0x28, d0_lw(pose_block + 0));
                    d0_sw(slot + 0x30, d0_lw(pose_block + 8));
                } else {
                    /* Still far: set flag */
                    d0_sw(slot + 0x60, 1);
                }

                /* Update Y from pose block */
                d0_sw(slot + 0x2C, d0_lw(pose_block + 4));
            }
        }
    }

    return 1;
}
