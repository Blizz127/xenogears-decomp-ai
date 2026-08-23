/*
 * World-map helper 0x80089580 (particle system tick).
 * Leaf function, no external calls.
 */
#include <stdint.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_89580.h"

#define D_8009BCC0  0x8009BCC0u  /* slot table base pointer */
#define D_8009BDF4  0x8009BDF4u  /* particle table base pointer */
#define PARTICLE_COUNT 256
#define PARTICLE_STRIDE 0x4C

static u32 pt_lw(u32 a) { u32 v; memcpy(&v, PSX_ADDR(a), 4); return v; }
static void pt_sw(u32 a, u32 v) { memcpy(PSX_ADDR(a), &v, 4); }
static u16 pt_lhu(u32 a) { u16 v; memcpy(&v, PSX_ADDR(a), 2); return v; }
static void pt_sh(u32 a, u16 v) { memcpy(PSX_ADDR(a), &v, 2); }
static s16 pt_lh(u32 a) { s16 v; memcpy(&v, PSX_ADDR(a), 2); return v; }
static s8 pt_lb(u32 a) { return *(s8*)PSX_ADDR(a); }
static u8 pt_lbu(u32 a) { return *(u8*)PSX_ADDR(a); }

static u32 pt_slot_table_base(void)
{
#if defined(WM_89580_MUTANT_WRONG_SLOT_GLOBAL)
    return pt_lw(0x8009BDE0u);
#elif defined(WM_89580_MUTANT_PARTICLE_GLOBAL)
    return pt_lw(D_8009BDF4);
#elif defined(WM_89580_MUTANT_SLOT_OFFSET)
    return pt_lw(D_8009BCC0 + 4u);
#else
    /* Retail 0x800896FC–0x80089708: lui/lw 0x8009BCC0. */
    return pt_lw(D_8009BCC0);
#endif
}

void wm_80089580(void)
{
    s32 i;
    u32 table_base = pt_lw(D_8009BDF4);
    u32 entry = table_base + 4; /* skip first 4 bytes (global counter?) */

    for (i = 0; i < PARTICLE_COUNT; i++) {
        /* Read packed count/counter from first word */
        u32 packed = pt_lw(entry);
        s16 counter = (s16)(u16)(packed >> 16);  /* high halfword */
        s16 delta = (s16)(u16)(packed & 0xFFFF); /* low halfword */

        if (counter == 0) {
            /* Inactive particle: check if slot needs cleanup */
            s16 slot_idx = pt_lh(table_base);
            if (slot_idx >= 0) {
                u32 slot_base = pt_slot_table_base();
                /* Decrement slot's particle counter */
                u32 slot_record = slot_base + (u32)slot_idx * 672;
                u16 pc = pt_lhu(slot_record + 0x0A);
                pt_sh(slot_record + 0x0A, pc - 1);
            }
            pt_sh(table_base, 0);
            pt_sw(entry, 0);
        } else if (delta > 0) {
            /* Active particle with remaining ticks */
            u32 vel_x = pt_lw(entry + 0x04);
            u32 vel_y = pt_lw(entry + 0x08);
            u32 vel_z = pt_lw(entry + 0x0C);
            u32 accel_x = pt_lw(entry + 0x14);
            u32 accel_y = pt_lw(entry + 0x18);
            u32 accel_z = pt_lw(entry + 0x1C);
            u32 pos_x = pt_lw(entry + 0x24);
            u32 pos_y = pt_lw(entry + 0x28);
            u32 pos_z = pt_lw(entry + 0x2C);
            u32 color = pt_lw(entry + 0x3C);
            s8 color_delta = pt_lb(entry + 0x40);
            u16 uv_u = pt_lhu(entry + 0x34);
            u16 uv_v = pt_lhu(entry + 0x36);
            u16 uv_u2 = pt_lhu(entry + 0x38);
            u16 uv_v2 = pt_lhu(entry + 0x3A);

            /* Update position: pos += vel */
            pt_sw(entry + 0x04, vel_x + accel_x);
            pt_sw(entry + 0x08, vel_y + accel_y);
            pt_sw(entry + 0x0C, vel_z + accel_z);

            /* Update velocity: vel += accel */
            pt_sw(entry + 0x14, accel_x + pos_x);
            pt_sw(entry + 0x18, accel_y + pos_y);
            pt_sw(entry + 0x1C, accel_z + pos_z);

            /* Update UVs */
            pt_sh(entry + 0x34, uv_u + uv_u2);
            pt_sh(entry + 0x36, uv_v + uv_v2);

            /* Update color components with clamping */
            {
                u32 r = (color & 0xFF) + (u32)(s32)color_delta;
                u32 g = ((color >> 8) & 0xFF) + (u32)((s32)(s8)(packed >> 8));
                u32 b = ((color >> 16) & 0xFF) + (u32)((s32)(s8)(packed >> 16));
                u32 a = (color >> 24) & 0xFF;

                /* Clamp to 0-255 */
                if ((s32)r < 0) r = 0; else if (r >= 0x100) r = 0xFF;
                if ((s32)g < 0) g = 0; else if (g >= 0x100) g = 0xFF;
                if ((s32)b < 0) b = 0; else if (b >= 0x100) b = 0xFF;

                pt_sw(entry + 0x3C, (a << 24) | (b << 16) | (g << 8) | r);
            }

            /* Decrement counter */
            pt_sh(entry, counter - 1);
        }

        /* Advance to next particle */
        entry += PARTICLE_STRIDE;
        table_base += PARTICLE_STRIDE;
    }
}
