/*
 * World-map scheduler callback 0x80092C70 (slot-12 cb1).
 *
 * Register-faithful transcription of retail world_map.bin
 * [0x80092C70, 0x80092DD0).  See world_map_callback_92c70.h.
 */
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_callback_92c70.h"

#define C70_POOL  0x8009BE24u
#define C70_SEL   0x8009BD24u
#define C70_OBJ   0x8009D498u
#define C70_TABLE 0x8009D784u
#define C70_DB    0x8009BE3Cu
#define C70_CTX   0x8009D7F0u

void func_80034614(void* object);
void* GetStringEntry(void* table, s32 index);
void func_80034714(void* object, void* entry);
void func_80034888(void* object, void* ot, s32 render_context_index);

static u32 c70_lw(u32 address) __attribute__((unused));
static void c70_sw(u32 address, u32 value) __attribute__((unused));
static s16 c70_lh(u32 address) __attribute__((unused));
static void c70_sh(u32 address, u16 value) __attribute__((unused));

static u32 c70_lw(u32 address)
{
    u32 value;

    memcpy(&value, PSX_ADDR(address), 4);
    return value;
}

static void c70_sw(u32 address, u32 value)
{
    memcpy(PSX_ADDR(address), &value, 4);
}

static s16 c70_lh(u32 address)
{
    s16 value;

    memcpy(&value, PSX_ADDR(address), 2);
    return value;
}

static void c70_sh(u32 address, u16 value)
{
    memcpy(PSX_ADDR(address), &value, 2);
}

static void c70_enqueue_selection(u32 slot)
{
    void* const object = PSX_ADDR(C70_OBJ);
    s32 before_helpers = (s32)c70_lh(C70_SEL);

#if !defined(WM_92C70_MUTANT_SKIP_34614)
    func_80034614(object);
#endif

#if !defined(WM_92C70_MUTANT_SKIP_STRING)
    {
    void* entry;
#if defined(WM_92C70_MUTANT_WRONG_TABLE)
    entry = GetStringEntry(PSX_ADDR(c70_lw(0x8009D780u)),
                           (s32)c70_lh(C70_SEL));
#elif defined(WM_92C70_MUTANT_STALE_INDEX)
    entry = GetStringEntry(PSX_ADDR(c70_lw(C70_TABLE)), before_helpers);
#else
    entry = GetStringEntry(PSX_ADDR(c70_lw(C70_TABLE)),
                           (s32)c70_lh(C70_SEL));
#endif
    func_80034714(object, entry);
    }
#endif

#if !defined(WM_92C70_MUTANT_SKIP_STATE_ARM)
    if (c70_lh(slot + 0x20u) == 0)
        c70_sh(slot + 0x20u, 1);
#endif
#if !defined(WM_92C70_MUTANT_SKIP_CACHE)
#if defined(WM_92C70_MUTANT_STALE_CACHE)
    c70_sw(slot + 0x50u, (u32)before_helpers);
#else
    c70_sw(slot + 0x50u, (u32)(s32)c70_lh(C70_SEL));
#endif
#endif
#if !defined(WM_92C70_MUTANT_STALE_INDEX) && \
    !defined(WM_92C70_MUTANT_STALE_CACHE)
    (void)before_helpers;
#endif
}

s32 wm_80092C70(s32 slot_idx)
{
    u32 slot;
    s16 state;
    s16 sel;

#if defined(WM_92C70_MUTANT_IGNORE_SLOT)
    (void)slot_idx;
    slot = c70_lw(C70_POOL);
#else
    slot = c70_lw(C70_POOL) + ((u32)slot_idx << 7);
#endif
    state = c70_lh(slot + 0x20u);

    if (state == 0) {
        sel = c70_lh(C70_SEL);
#if defined(WM_92C70_MUTANT_STATE0_SKIP_SEL)
        c70_enqueue_selection(slot);
#else
        if (sel != -1)
            c70_enqueue_selection(slot);
#endif
    } else if (state == 1) {
        sel = c70_lh(C70_SEL);
        if (sel == -1) {
#if !defined(WM_92C70_MUTANT_SKIP_34614)
            func_80034614(PSX_ADDR(C70_OBJ));
#endif
#if !defined(WM_92C70_MUTANT_STATE1_CLEAR_SKIP)
            c70_sh(slot + 0x20u, 0);
#endif
        } else {
            u32 cached = c70_lw(slot + 0x50u);
#if defined(WM_92C70_MUTANT_HALF_COMPARE)
            if ((u16)sel != (u16)cached)
#elif defined(WM_92C70_MUTANT_SKIP_CHANGED)
            (void)cached;
            if (0)
#else
            if ((s32)sel != (s32)cached)
#endif
            {
                c70_enqueue_selection(slot);
            }
        }
    }

#if !defined(WM_92C70_MUTANT_SKIP_34888)
    {
        u32 db = c70_lw(C70_DB);
#if defined(WM_92C70_MUTANT_WRONG_OT_OFF)
        u32 ot = c70_lw(db + 0x6Cu);
#else
        u32 ot = c70_lw(db + 0x70u);
#endif
        s32 ctx = (s32)c70_lw(C70_CTX);
#if defined(WM_92C70_MUTANT_WRONG_OBJECT)
        func_80034888(PSX_ADDR(0x8009BD64u), (void*)(uintptr_t)ot, ctx);
#else
        func_80034888(PSX_ADDR(C70_OBJ), (void*)(uintptr_t)ot, ctx);
#endif
    }
#endif

#if defined(WM_92C70_MUTANT_WRONG_RETURN)
    return 3;
#else
    return 1;
#endif
}
