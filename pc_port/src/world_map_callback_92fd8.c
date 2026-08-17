/*
 * World-map scheduler callback 0x80092FD8 (slot-13 cb1).
 *
 * Register-faithful transcription of retail world_map.bin
 * [0x80092FD8, 0x800931B0).  See world_map_callback_92fd8.h.
 */
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_callback_92fd8.h"

#define F8_POOL  0x8009BE24u
#define F8_SEL   0x8009CE68u
#define F8_OBJ   0x8009BD64u
#define F8_TABLE 0x8009D784u
#define F8_DB    0x8009BE3Cu
#define F8_CTX   0x8009D7F0u
#define F8_PRIM  0x8009D2B8u
#define F8_TAG_LEN  0xFF000000u
#define F8_TAG_PTR  0x00FFFFFFu
#define F8_PRIM_STRIDE 0x28u

void func_80034614(void* object);
void* GetStringEntry(void* table, s32 index);
void func_80034714(void* object, void* entry);
void func_80034888(void* object, void* ot, s32 render_context_index);

static u32 f8_lw(u32 address) __attribute__((unused));
static void f8_sw(u32 address, u32 value) __attribute__((unused));
static s16 f8_lh(u32 address) __attribute__((unused));
static void f8_sh(u32 address, u16 value) __attribute__((unused));

static u32 f8_lw(u32 address)
{
    u32 value;

    memcpy(&value, PSX_ADDR(address), 4);
    return value;
}

static void f8_sw(u32 address, u32 value)
{
    memcpy(PSX_ADDR(address), &value, 4);
}

static s16 f8_lh(u32 address)
{
    s16 value;

    memcpy(&value, PSX_ADDR(address), 2);
    return value;
}

static void f8_sh(u32 address, u16 value)
{
    memcpy(PSX_ADDR(address), &value, 2);
}

static void f8_enqueue_selection(u32 slot)
{
    void* const object = PSX_ADDR(F8_OBJ);
    s32 before_helpers = (s32)f8_lh(F8_SEL);

#if !defined(WM_92FD8_MUTANT_SKIP_34614)
    func_80034614(object);
#endif

#if !defined(WM_92FD8_MUTANT_SKIP_STRING)
    {
        void* entry;
#if defined(WM_92FD8_MUTANT_WRONG_TABLE)
        entry = GetStringEntry(PSX_ADDR(f8_lw(0x8009D780u)),
                               (s32)f8_lh(F8_SEL));
#elif defined(WM_92FD8_MUTANT_STALE_INDEX)
        entry = GetStringEntry(PSX_ADDR(f8_lw(F8_TABLE)), before_helpers);
#else
        entry = GetStringEntry(PSX_ADDR(f8_lw(F8_TABLE)),
                               (s32)f8_lh(F8_SEL));
#endif
        func_80034714(object, entry);
    }
#endif

#if !defined(WM_92FD8_MUTANT_SKIP_STATE_ARM)
    if (f8_lh(slot + 0x20u) == 0)
        f8_sh(slot + 0x20u, 1);
#endif
#if !defined(WM_92FD8_MUTANT_SKIP_CACHE)
#if defined(WM_92FD8_MUTANT_STALE_CACHE)
    f8_sw(slot + 0x50u, (u32)before_helpers);
#else
    f8_sw(slot + 0x50u, (u32)(s32)f8_lh(F8_SEL));
#endif
#endif
#if !defined(WM_92FD8_MUTANT_STALE_INDEX) && \
    !defined(WM_92FD8_MUTANT_STALE_CACHE)
    (void)before_helpers;
#endif
}

static void f8_insert_prim(void) __attribute__((unused));
static void f8_insert_prim(void)
{
    u32 ctx = f8_lw(F8_CTX);
    u32 db = f8_lw(F8_DB);
    u32 ot = f8_lw(db + 0x70u);
#if defined(WM_92FD8_MUTANT_WRONG_PRIM)
    u32 prim = 0x8009D498u + ctx * F8_PRIM_STRIDE;
#else
    u32 prim = F8_PRIM + ctx * F8_PRIM_STRIDE;
#endif
    u32 prim_tag = f8_lw(prim);
    u32 ot_tag = f8_lw(ot);

    f8_sw(prim, (prim_tag & F8_TAG_LEN) | (ot_tag & F8_TAG_PTR));
    ot_tag = f8_lw(ot);
    f8_sw(ot, (ot_tag & F8_TAG_LEN) | (prim & F8_TAG_PTR));
}

s32 wm_80092FD8(s32 slot_idx)
{
    u32 slot;
    s16 state;
    s16 sel;

#if defined(WM_92FD8_MUTANT_IGNORE_SLOT)
    (void)slot_idx;
    slot = f8_lw(F8_POOL);
#else
    slot = f8_lw(F8_POOL) + ((u32)slot_idx << 7);
#endif
    state = f8_lh(slot + 0x20u);

    if (state == 0) {
        sel = f8_lh(F8_SEL);
#if defined(WM_92FD8_MUTANT_STATE0_SKIP_SEL)
        f8_enqueue_selection(slot);
#else
        if (sel != -1)
            f8_enqueue_selection(slot);
#endif
    } else if (state == 1) {
        sel = f8_lh(F8_SEL);
        if (sel == -1) {
#if !defined(WM_92FD8_MUTANT_SKIP_34614)
            func_80034614(PSX_ADDR(F8_OBJ));
#endif
#if !defined(WM_92FD8_MUTANT_STATE1_CLEAR_SKIP)
            f8_sh(slot + 0x20u, 0);
#endif
        } else {
            u32 cached = f8_lw(slot + 0x50u);
#if defined(WM_92FD8_MUTANT_HALF_COMPARE)
            if ((u16)sel != (u16)cached)
#elif defined(WM_92FD8_MUTANT_SKIP_CHANGED)
            (void)cached;
            if (0)
#else
            if ((s32)sel != (s32)cached)
#endif
            {
                f8_enqueue_selection(slot);
            }
        }
    }

#if !defined(WM_92FD8_MUTANT_SKIP_34888)
    {
        u32 db = f8_lw(F8_DB);
        u32 ot = f8_lw(db + 0x70u);
        s32 ctx = (s32)f8_lw(F8_CTX);
        func_80034888(PSX_ADDR(F8_OBJ), (void*)(uintptr_t)ot, ctx);
    }
#endif

#if defined(WM_92FD8_MUTANT_OT_ALWAYS)
    f8_insert_prim();
#elif defined(WM_92FD8_MUTANT_SKIP_OT)
    (void)f8_lh(slot + 0x20u);
#else
    if (f8_lh(slot + 0x20u) == 1)
        f8_insert_prim();
#endif

#if defined(WM_92FD8_MUTANT_WRONG_RETURN)
    return 3;
#else
    return 1;
#endif
}
