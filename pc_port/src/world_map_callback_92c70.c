/*
 * World-map scheduler callback 0x80092C70 (slot-12 cb1).
 * UI-related callback with area-dependent string lookup.
 */
#include <stdint.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_callback_92c70.h"

/* PsyQ/data function stubs */
extern void func_80034614(void* object);
extern s32 func_80033728(s32 table, s32 index);
extern void func_80034714(void* object, s32 entry);
extern void func_80034888(void* object, s32 arg1, s32 arg2);

#define POOL_PTR   0x8009BE24u
#define AREA       0x8009BD24u
#define UI_OBJECT  0x8009D498u
#define D_8009D784 0x8009D784u
#define D_8009D7F0 0x8009D7F0u

static s16 c70_lh(u32 a) { s16 v; memcpy(&v, PSX_ADDR(a), 2); return v; }
static void c70_sh(u32 a, u16 v) { memcpy(PSX_ADDR(a), &v, 2); }
static s32 c70_lw(u32 a) { s32 v; memcpy(&v, PSX_ADDR(a), 4); return v; }
static void c70_sw(u32 a, s32 v) { memcpy(PSX_ADDR(a), &v, 4); }

s32 wm_80092C70(s32 slot_idx)
{
    u32 pool_ptr = (u32)c70_lw(POOL_PTR);
    u32 slot = pool_ptr + (u32)(slot_idx << 7);

    /* Main dispatch on slot[+0x20] */
    s16 state = c70_lh(slot + 0x20);

    if (state == 0) {
        /* State 0: init — check area, do string lookup */
        s16 area = c70_lh(AREA);
        if (area == -1) {
            return 1;
        }

        /* Clear UI object */
        func_80034614((void*)(uintptr_t)UI_OBJECT);

        /* String lookup: GetStringEntry(D_8009D784, area) */
        {
            s32 table = c70_lw(D_8009D784);
            s32 entry = func_80033728(table, (s32)area);
            func_80034714((void*)(uintptr_t)UI_OBJECT, entry);
        }

        /* Store area, advance to state 1 */
        c70_sw(slot + 0x50, (s32)area);
        c70_sh(slot + 0x20, 1);

    } else if (state == 1) {
        /* State 1: active — monitor area changes */
        s16 area = c70_lh(AREA);
        s32 stored_area = c70_lw(slot + 0x50);

        if (area == -1) {
            /* Area cleared: close and return to state 0 */
            func_80034614((void*)(uintptr_t)UI_OBJECT);
            c70_sh(slot + 0x20, 0);
        } else if ((s32)area != stored_area) {
            /* Area changed: update string */
            func_80034614((void*)(uintptr_t)UI_OBJECT);
            {
                s32 table = c70_lw(D_8009D784);
                s32 entry = func_80033728(table, (s32)area);
                func_80034714((void*)(uintptr_t)UI_OBJECT, entry);
            }
            c70_sw(slot + 0x50, (s32)area);
        }
    }

    /* Common: palette/texture update */
    {
        u32 palette_source = (u32)c70_lw(/* D_8009BE3C */ 0x8009BE3Cu);
        s32 pal_data = c70_lw(palette_source + 0x70);
        s32 pal_table = c70_lw(D_8009D7F0);
        func_80034888((void*)(uintptr_t)UI_OBJECT, pal_data, pal_table);
    }

    return 1;
}
