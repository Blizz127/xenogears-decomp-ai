/*
 * World-map helper 0x80089748 (particle effect spawner/updater).
 */
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "common.h"
#include "psx_memory.h"
#include "psyq/libgte.h"
#include "world_map_helper_89748.h"
#include "world_map_helper_89580.h"

#define D_8009BCC0  0x8009BCC0u  /* slot table base */
#define D_8009BDF4  0x8009BDF4u  /* particle table base */
#define D_8009A280  0x8009A280u  /* effect data base */
#define SCRATCH     0x1F800000u
#define PARTICLE_COUNT 256
#define PARTICLE_STRIDE 0x4C

static u8 p_lbu(u32 a) { return *(u8*)PSX_ADDR(a); }
static s16 p_lh(u32 a) { s16 v; memcpy(&v, PSX_ADDR(a), 2); return v; }
static u16 p_lhu(u32 a) { u16 v; memcpy(&v, PSX_ADDR(a), 2); return v; }
static s32 p_lw(u32 a) { s32 v; memcpy(&v, PSX_ADDR(a), 4); return v; }
static void p_sh(u32 a, u16 v) { memcpy(PSX_ADDR(a), &v, 2); }
static void p_sw(u32 a, s32 v) { memcpy(PSX_ADDR(a), &v, 4); }

void wm_80089748(void)
{
    s32 i;
    u32 slot_base = p_lw(D_8009BCC0);
    u32 particle_base = p_lw(D_8009BDF4);
    u32 entry = particle_base + 4;

    for (i = 0; i < PARTICLE_COUNT; i++) {
        u8 flags = p_lbu(entry + 0x4B);

        if (!(flags & 0x80)) {
            /* Inactive particle — skip */
            entry += PARTICLE_STRIDE;
            continue;
        }

        /* Check timer countdown */
        {
            u32 packed = (u32)(u16)p_lhu(entry + 0x00);
            s16 counter_hi = (s16)(packed >> 16);
            s16 counter_lo = (s16)(packed & 0xFFFF);

            if (flags & 0x10) {
                /* Has coordinates: check sub-timer */
                s16 timer = p_lh(entry + 0x0E);
                if (timer != 0) {
                    p_sh(entry + 0x0E, timer - 1);
                    entry += PARTICLE_STRIDE;
                    continue;
                }
            }
        }

        /* Check spawn conditions */
        {
            s16 x = p_lh(entry + 0x04);
            s16 z = p_lh(entry + 0x06);
            s16 y = p_lh(entry + 0x0A);

            if (x > 0 && z < x && y > 0) {
                /* Find free particle slot */
                u32 slot_ptr = particle_base + 0x48;
                s32 slot_idx;
                for (slot_idx = 0; slot_idx < PARTICLE_COUNT; slot_idx++) {
                    if (p_lh(slot_ptr - 0x42) == 0) break;
                    slot_ptr += PARTICLE_STRIDE;
                }

                if (slot_idx < PARTICLE_COUNT) {
                    MATRIX rot_mat;
                    SVECTOR rot;
                    VECTOR pos, out;

                    /* Initialize particle */
                    p_sh(slot_ptr, 0);

                    /* Build rotation from entry data */
                    rot.vx = p_lh(entry + 0x08);
                    rot.vy = 0;
                    rot.vz = 0;
                    RotMatrixYXZ(&rot, &rot_mat);

                    /* Apply matrix to position */
                    pos.vx = p_lw(entry + 0x24);
                    pos.vy = p_lw(entry + 0x28);
                    pos.vz = p_lw(entry + 0x2C);
                    ApplyMatrix(&rot_mat, (SVECTOR*)&pos, &out);

                    /* Store transformed position */
                    p_sw(slot_ptr + 0x04, out.vx);
                    p_sw(slot_ptr + 0x08, out.vy);
                    p_sw(slot_ptr + 0x0C, out.vz);

                    /* Random heading if flag set */
                    if (flags & 0x20) {
                        s16 heading = (s16)(rand() & 0xFFF);
                        p_sh(slot_ptr + 0x34, heading);
                    }

                    /* Copy color/flags */
                    p_sw(slot_ptr + 0x3C, p_lw(entry + 0x3C));
                    p_sh(slot_ptr + 0x40, p_lhu(entry + 0x40));

                    /* Set active flag */
                    p_sh(slot_ptr + 0x4B, 0x80);
                }
            }
        }

        entry += PARTICLE_STRIDE;
    }

    /* Tick all particles */
    wm_80089580();
}
