/*
 * World-map helpers 0x80096130 and 0x800967E4 — queue synchronization.
 */
#include <stdint.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_96130.h"
#include "world_map_helper_9623c.h"
#include "world_map_helper_966cc.h"

extern u32 func_8002C3D8(void);
extern void Vsync(long mode);

#define D_8009BE44  0x8009BE44u
#define D_8009D788  0x8009D788u
#define D_8009C624  0x8009C624u
#define D_8009BCB8  0x8009BCB8u
#define D_8009D808  0x8009D808u

static u32 q96_lw(u32 a) { u32 v; memcpy(&v, PSX_ADDR(a), 4); return v; }
static void q96_sw(u32 a, u32 v) { memcpy(PSX_ADDR(a), &v, 4); }

void wm_800967E4(void)
{
    u32 r1 = func_8002C3D8();
    u32 r2 = func_8002C3D8();

    /* Check if archive is ready */
    u32 ready = ((r1 < 1) | (~r2 < 1));

    if (ready) {
        /* Archive path: process D_8009D788 entries */
        u32 idx = q96_lw(D_8009BE44);
        u32* table = (u32*)PSX_ADDR(D_8009D788);
        u32 entry = table[idx];

        if (entry != 0) {
            /* Call file I/O processor */
            wm_800966CC(entry);
        }
    } else {
        /* CD path: process D_8009C624 entries */
        u32 idx = q96_lw(D_8009BCB8);
        u32* table = (u32*)PSX_ADDR(D_8009C624);
        u32 entry = table[idx];

        if (entry != 0) {
            /* Call CD processor */
            wm_8009699C(entry, 0, 0);

            /* Advance index (mod 16) */
            q96_sw(D_8009BCB8, (idx + 1) & 0xF);
            table[idx] = 0;
        }
    }
}

void wm_80096130(void)
{
    /* First check: archive debug table path */
    u32 r1 = func_8002C3D8();
    u32 r2 = func_8002C3D8();
    u32 ready = ((r1 < 1) | (~r2 < 1));

    if (ready) {
        /* Drain D_8009D788 queue */
        u32 idx = q96_lw(D_8009BE44);
        u32* table = (u32*)PSX_ADDR(D_8009D788);

        while (table[idx] != 0) {
            Vsync(0);
            wm_800967E4();
            if (table[idx] == 0) break;
        }
    } else {
        /* Drain D_8009C624 queue */
        u32 idx = q96_lw(D_8009BCB8);
        u32* table = (u32*)PSX_ADDR(D_8009C624);

        while (table[idx] != 0) {
            Vsync(0);
            wm_800967E4();
            if (table[idx] == 0) break;
        }
    }
}
