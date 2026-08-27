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

/* Retail 0x800967E4 (W34C11 re-wiring, PCs inline). ready = (r1 == 0) |
 * (~r2 == 0). Ready (0x8009681C-0x80096860): if the CD machine is busy
 * (0x800968E0 pre-check returns nonzero) do nothing; else take
 * D788[lw 0x8009BCB8] (0x80096834-48) and start the CD loader 0x8009699C.
 * Not ready (0x80096868-0x800968C0): take C624[lw 0x8009BCB8], run the
 * PC-file loader 0x800966CC, clear the entry, BCB8 = (BCB8 + 1) & 0xF. */
void wm_800967E4(void)
{
    u32 r1 = func_8002C3D8();
    u32 r2 = func_8002C3D8();
    u32 ready = ((r1 < 1) | (~r2 < 1));
#if defined(WM_967E4_MUTANT_M1)                  /* pre-W34C11 wiring */
    ready = !ready;
#endif

    if (ready) {
#if defined(WM_967E4_MUTANT_M2)
        u32 idx = q96_lw(D_8009BE44);
#else
        u32 idx = q96_lw(D_8009BCB8);             /* 0x80096834 */
#endif
        u32* table = (u32*)PSX_ADDR(D_8009D788);
        if (q96_lw(0x8009CD44u) != 0u)            /* 0x800968E0: busy */
            return;
        if (table[idx] != 0) {
            wm_8009699C(table[idx]);              /* 0x80096858 */
        }
    } else {
        u32 idx = q96_lw(D_8009BCB8);             /* 0x8009686C */
        u32* table = (u32*)PSX_ADDR(D_8009C624);
        if (table[idx] != 0) {
            wm_800966CC(table[idx]);              /* 0x80096894 */
            table[idx] = 0;                       /* 0x800968B8 */
            q96_sw(D_8009BCB8, (idx + 1) & 0xF);  /* 0x800968C0 */
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
