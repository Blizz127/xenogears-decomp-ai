/*
 * World-map update helper 0x80090C68.
 *
 * Register-faithful transcription of retail world_map.bin
 * [0x80090C68, 0x80090E14).  See world_map_helper_90c68.h.
 *
 * JT @0x80070B84, 12 entries (index = (lhu 0x8009CD4C >> 12) - 1,
 * sltiu 0xC; OOR and phases 5/7/10/11 -> common heading test):
 *   1 -> store angle; 2 +0x400; 3 +0x200; 4 +0x800; 6 +0x600;
 *   8 -0x400; 9 -0x200; 12 -0x600; then andi 0xFFF except phase 1.
 * If heading & 0xF000: +0x38 = rcos(lh +0x48), +0x40 = -rsin
 * (rcos lands in the rsin jal delay slot).
 * Flag 0x20: byte 0x8009D738 != 0 -> return 3; lh 0x8009BD24 != -1
 * -> return 1; else join the flag-0x10 publish / 90A18 tail.
 * Flag 0x20 clear skips the object+0x0E test that 90A84 performs.
 */
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_90a84.h"
#include "world_map_helper_90c68.h"

extern long rcos(long a);
extern long rsin(long a);

#define WM_90C68_HEADING        0x8009CD4Cu
#define WM_90C68_ANGLE          0x8009BD3Au
#define WM_90C68_FLAGS          0x8009BD10u
#define WM_90C68_BYTE_STATE     0x8009D738u
#define WM_90C68_SELECTION      0x8009BD24u
#define WM_90C68_SELECTION_ALT  0x8009CE68u
#define WM_90C68_STATE_PUBLISH  0x8009D804u

#define WM_90C68_SLOT_ANGLE     0x48u
#define WM_90C68_SLOT_COS       0x38u
#define WM_90C68_SLOT_SIN       0x40u

static u8 wm_90c68_lbu(u32 address)
{
    u8 value;

    memcpy(&value, PSX_ADDR(address), sizeof(value));
    return value;
}

static u16 wm_90c68_lhu(u32 address)
{
    u16 value;

    memcpy(&value, PSX_ADDR(address), sizeof(value));
    return value;
}

static s16 wm_90c68_lh(u32 address)
{
    s16 value;

    memcpy(&value, PSX_ADDR(address), sizeof(value));
    return value;
}

static void wm_90c68_sh(u32 address, u16 value)
{
    memcpy(PSX_ADDR(address), &value, sizeof(value));
}

static void wm_90c68_sw(u32 address, u32 value)
{
    memcpy(PSX_ADDR(address), &value, sizeof(value));
}

static u16 wm_90c68_adjust(u16 angle, s32 delta)
{
    return (u16)(((u32)angle + (u32)delta) & 0x0FFFu);
}

static void wm_90c68_select_heading(u32 slot_addr, u32 phase)
{
    u16 angle;

    if (phase < 1u || phase > 12u)
        return;

    angle = wm_90c68_lhu(WM_90C68_ANGLE);
    switch (phase) {
    case 1u:
        wm_90c68_sh(slot_addr + WM_90C68_SLOT_ANGLE, angle);
        break;
    case 2u:
        wm_90c68_sh(slot_addr + WM_90C68_SLOT_ANGLE,
                    wm_90c68_adjust(angle, 0x400));
        break;
    case 3u:
        wm_90c68_sh(slot_addr + WM_90C68_SLOT_ANGLE,
                    wm_90c68_adjust(angle, 0x200));
        break;
    case 4u:
        wm_90c68_sh(slot_addr + WM_90C68_SLOT_ANGLE,
                    wm_90c68_adjust(angle, 0x800));
        break;
    case 6u:
        wm_90c68_sh(slot_addr + WM_90C68_SLOT_ANGLE,
                    wm_90c68_adjust(angle, 0x600));
        break;
    case 8u:
        wm_90c68_sh(slot_addr + WM_90C68_SLOT_ANGLE,
                    wm_90c68_adjust(angle, -0x400));
        break;
    case 9u:
        wm_90c68_sh(slot_addr + WM_90C68_SLOT_ANGLE,
                    wm_90c68_adjust(angle, -0x200));
        break;
    case 12u:
        wm_90c68_sh(slot_addr + WM_90C68_SLOT_ANGLE,
                    wm_90c68_adjust(angle, -0x600));
        break;
    default:
        /* Retail table entries 5, 7, 10, and 11 target the common tail. */
        break;
    }
}

static s32 wm_90c68_update_result(void)
{
    u16 flags = wm_90c68_lhu(WM_90C68_FLAGS);
    s16 selection;

    if ((flags & 0x20u) != 0u) {
        if (wm_90c68_lbu(WM_90C68_BYTE_STATE) != 0u)
            return 3;
        selection = wm_90c68_lh(WM_90C68_SELECTION);
        if (selection != (s16)-1)
            return 1;
    }

    if ((flags & 0x10u) != 0u) {
        selection = wm_90c68_lh(WM_90C68_SELECTION_ALT);
        if (selection == (s16)-1) {
            selection = wm_90c68_lh(WM_90C68_SELECTION);
            if (selection == (s16)-1)
                wm_90c68_sw(WM_90C68_STATE_PUBLISH, 1u);
        }
    }

    wm_80090A18();
    return 0;
}

s32 wm_80090C68(u32 slot_addr)
{
    u16 heading = wm_90c68_lhu(WM_90C68_HEADING);
    u32 phase = (u32)heading >> 12;
    long cosine;
    long sine;

    wm_90c68_select_heading(slot_addr, phase);

    if ((heading & 0xF000u) != 0u) {
        s32 angle = (s32)wm_90c68_lh(slot_addr + WM_90C68_SLOT_ANGLE);

        cosine = rcos((long)angle);
        sine = rsin((long)angle);
        /* jal rsin delay slot stores the still-live rcos result. */
        wm_90c68_sw(slot_addr + WM_90C68_SLOT_COS, (u32)cosine);
        wm_90c68_sw(slot_addr + WM_90C68_SLOT_SIN, 0u - (u32)sine);
    }

    return wm_90c68_update_result();
}
