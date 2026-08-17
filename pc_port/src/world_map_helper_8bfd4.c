/*
 * World-map helper 0x8008BFD4 (ring buffer write).
 * Leaf function, no external calls.
 */
#include <string.h>
#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_8bfd4.h"

#define BFD4_TABLE   0x8009BE6Cu  /* -0x4194: 32 x 24-byte entries */
#define BFD4_INDEX   0x8009BD04u  /* -0x42FC: u16 index */
#define BFD4_STRIDE  24u
#define BFD4_MASK    0x1Fu

static void bfd4_sh(u32 a, u16 v) { memcpy(PSX_ADDR(a), &v, 2); }
static void bfd4_sw(u32 a, u32 v) { memcpy(PSX_ADDR(a), &v, 4); }
static u16 bfd4_lhu(u32 a) { u16 v; memcpy(&v, PSX_ADDR(a), 2); return v; }
static u32 bfd4_lw(u32 a) { u32 v; memcpy(&v, PSX_ADDR(a), 4); return v; }

void wm_8008BFD4(u16 tag, u32 src_vec, u16 half2, s16 sh_val)
{
    u16 idx = bfd4_lhu(BFD4_INDEX);
    u32 entry = BFD4_TABLE + (u32)idx * BFD4_STRIDE;

    bfd4_sh(entry + 0x00, tag);
    bfd4_sw(entry + 0x04, bfd4_lw(src_vec + 0));
    bfd4_sw(entry + 0x08, bfd4_lw(src_vec + 4));
    bfd4_sw(entry + 0x0C, bfd4_lw(src_vec + 8));
    bfd4_sw(entry + 0x10, (u32)sh_val);
    bfd4_sh(entry + 0x14, half2);

    idx = (idx + 1) & BFD4_MASK;
    bfd4_sh(BFD4_INDEX, idx);
}
