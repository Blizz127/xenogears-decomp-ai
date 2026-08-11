/*
 * World-map scheduler callback 0x80091430.
 *
 * The leaf copies the shared four-word position and heading into slot 9,
 * applies the signed mode 6..7 control arm, and returns scheduler state 1.
 */
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_callback_91430.h"

#define WM_91430_POOL_PTR       0x8009BE24u
#define WM_91430_MODE           0x8009BE10u
#define WM_91430_POSITION       0x8009D55Cu
#define WM_91430_HEADING_SOURCE 0x8009D52Cu
#define WM_91430_HEADING_X      0x8009BD3Au
#define WM_91430_HEADING_Y      0x8009BD3Cu

#define WM_91430_SLOT_CONTROL 0x20u
#define WM_91430_SLOT_X       0x28u
#define WM_91430_SLOT_Y       0x2Cu
#define WM_91430_SLOT_Z       0x30u
#define WM_91430_SLOT_W       0x34u
#define WM_91430_SLOT_HEADING 0x50u
#define WM_91430_SLOT_ROTATION 0x58u

/* Opaque access wrappers preserve retail ordering in optimized builds. They
 * also expose both reads and writes to the production-linked oracle. */
static u16 wm_91430_load_u16(u32 pc, u32 address)
    __attribute__((noinline));
static u16 wm_91430_load_u16(u32 pc, u32 address)
{
    u16 value;
    memcpy(&value, PSX_ADDR(address), sizeof(value));
#if defined(WM_91430_TEST_TRACE)
    wm_91430_test_trace(pc, WM_91430_TRACE_LHU, address, 2u, (u32)value);
#else
    (void)pc;
#endif
    return value;
}

static s16 wm_91430_load_s16(u32 pc, u32 address)
    __attribute__((noinline));
static s16 wm_91430_load_s16(u32 pc, u32 address)
{
    s16 value;
    memcpy(&value, PSX_ADDR(address), sizeof(value));
#if defined(WM_91430_TEST_TRACE)
    wm_91430_test_trace(pc, WM_91430_TRACE_LH, address, 2u,
                        (u32)(s32)value);
#else
    (void)pc;
#endif
    return value;
}

static u32 wm_91430_load_u32(u32 pc, u32 address)
    __attribute__((noinline));
static u32 wm_91430_load_u32(u32 pc, u32 address)
{
    u32 value;
    memcpy(&value, PSX_ADDR(address), sizeof(value));
#if defined(WM_91430_TEST_TRACE)
    wm_91430_test_trace(pc, WM_91430_TRACE_LW, address, 4u, value);
#else
    (void)pc;
#endif
    return value;
}

static void wm_91430_store_u16(u32 pc, u32 address, u16 value)
    __attribute__((noinline));
static void wm_91430_store_u16(u32 pc, u32 address, u16 value)
{
    memcpy(PSX_ADDR(address), &value, sizeof(value));
#if defined(WM_91430_TEST_TRACE)
    wm_91430_test_trace(pc, WM_91430_TRACE_SH, address, 2u, (u32)value);
#else
    (void)pc;
#endif
}

static void wm_91430_store_u32(u32 pc, u32 address, u32 value)
    __attribute__((noinline));
static void wm_91430_store_u32(u32 pc, u32 address, u32 value)
{
    memcpy(PSX_ADDR(address), &value, sizeof(value));
#if defined(WM_91430_TEST_TRACE)
    wm_91430_test_trace(pc, WM_91430_TRACE_SW, address, 4u, value);
#else
    (void)pc;
#endif
}

static s32 wm_91430_u32_as_s32(u32 value)
{
    s32 result;
    memcpy(&result, &value, sizeof(result));
    return result;
}

static u32 wm_91430_s32_as_u32(s32 value)
{
    u32 result;
    memcpy(&result, &value, sizeof(result));
    return result;
}

static s16 wm_91430_u16_as_s16(u16 value)
{
    s16 result;
    memcpy(&result, &value, sizeof(result));
    return result;
}

s32 wm_80091430(s32 slot_index)
{
    u32 pool;
    u32 slot;
    u32 x;
    u32 y;
    u32 z;
    u32 w;
    u16 heading_raw;
    s16 heading;
    s32 mode;

    pool = wm_91430_load_u32(0x80091434u, WM_91430_POOL_PTR);
    slot = pool + ((u32)slot_index << 7);

    /* Retail loads X/Y/Z before publishing any of them. */
    x = wm_91430_load_u32(0x80091448u, WM_91430_POSITION + 0u);
    y = wm_91430_load_u32(0x8009144Cu, WM_91430_POSITION + 4u);
    z = wm_91430_load_u32(0x80091450u, WM_91430_POSITION + 8u);
    wm_91430_store_u32(0x80091454u, slot + WM_91430_SLOT_X, x);
    wm_91430_store_u32(0x80091458u, slot + WM_91430_SLOT_Y, y);
    wm_91430_store_u32(0x8009145Cu, slot + WM_91430_SLOT_Z, z);
    w = wm_91430_load_u32(0x80091460u, WM_91430_POSITION + 12u);
    wm_91430_store_u32(0x80091468u, slot + WM_91430_SLOT_W, w);

    heading_raw = wm_91430_load_u16(0x80091470u,
                                    WM_91430_HEADING_SOURCE);
    /* Fresh retail bytes store BD3C at 0x80091480 before BD3A at 91484. */
    wm_91430_store_u16(0x80091480u, WM_91430_HEADING_Y, 0u);
    wm_91430_store_u16(0x80091484u, WM_91430_HEADING_X, heading_raw);
    wm_91430_store_u32(0x80091490u, slot + WM_91430_SLOT_HEADING,
                       wm_91430_s32_as_u32(
                           (s32)wm_91430_u16_as_s16(heading_raw)));

    /* 0x80091494 is a mandatory fresh signed halfword load from BD3A. */
    heading = wm_91430_load_s16(0x80091494u, WM_91430_HEADING_X);
    mode = wm_91430_u32_as_s32(
        wm_91430_load_u32(0x8009149Cu, WM_91430_MODE));
    wm_91430_store_u32(0x800914A4u, slot + WM_91430_SLOT_ROTATION,
                       (u32)(s32)heading << 12);

    if (mode < 8 && mode >= 6) {
        wm_91430_store_u16(0x800914C4u, slot + WM_91430_SLOT_CONTROL,
                           3u);
    }

    return 1;
}
