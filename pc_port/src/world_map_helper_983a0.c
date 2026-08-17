/*
 * World-map helper 0x800983A0 (matrix-composed GTE processor).
 */
#include <stdint.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "psyq/libgte.h"
#include "world_map_helper_983a0.h"
#include "world_map_helper_987ac.h"

#define SCRATCH      0x1F800000u
#define D_8009D534   0x8009D534u  /* model matrix source */
#define D_8009C808   0x8009C808u  /* camera matrix source */
#define D_8009D618   0x8009D618u  /* object table */
#define D_8009D650   0x8009D650u  /* secondary table */

static s32 m_lw(u32 a) { s32 v; memcpy(&v, PSX_ADDR(a), 4); return v; }
static void m_sw(u32 a, s32 v) { memcpy(PSX_ADDR(a), &v, 4); }
static u16 m_lhu(u32 a) { u16 v; memcpy(&v, PSX_ADDR(a), 2); return v; }

void wm_800983A0(u32 input_addr)
{
    s32 i;

    /* Copy model matrix to scratchpad+0xF0 */
    memcpy((void*)PSX_ADDR(SCRATCH + 0xF0),
           (void*)PSX_ADDR(D_8009D534), 32);

    /* CompMatrix: camera × model → scratch+0x110 */
    CompMatrix((MATRIX*)PSX_ADDR(D_8009C808),
               (MATRIX*)PSX_ADDR(SCRATCH + 0xF0),
               (MATRIX*)PSX_ADDR(SCRATCH + 0x110));

    /* SetRotMatrix + SetTransMatrix */
    SetRotMatrix((MATRIX*)PSX_ADDR(SCRATCH + 0x110));
    SetTransMatrix((MATRIX*)PSX_ADDR(SCRATCH + 0x110));

    /* Process objects via wm_800987AC */
    {
        u32 table = D_8009D618;
        u32 secondary = D_8009D650;

        /* Iterate and call wm_800987AC for each valid entry */
        for (i = 0; i < 64; i++) {
            u32 entry = table + (u32)i * 4;
            u32 obj_ptr = (u32)m_lw(entry);

            if (obj_ptr != 0) {
                wm_800987AC(obj_ptr);
            }
        }
    }
}
