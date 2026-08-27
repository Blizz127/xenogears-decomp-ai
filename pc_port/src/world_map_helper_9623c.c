/*
 * World-map primitive submission queue helpers.
 * Small leaf functions that manage a rendering primitive queue.
 */
#include <stdint.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_9623c.h"

#define D_8009D808  0x8009D808u  /* queue counter */
#define D_8009BE44  0x8009BE44u  /* record stride multiplier */
#define D_8009BE08  0x8009BE08u  /* queue base pointer A */
#define D_8009D3C0  0x8009D3C0u  /* queue base pointer B */
#define QUEUE_MAX   0x58

static u32 q_lw(u32 a) { u32 v; memcpy(&v, PSX_ADDR(a), 4); return v; }
static void q_sw(u32 a, u32 v) { memcpy(PSX_ADDR(a), &v, 4); }

/* wm_8009623C: 3-word record writer (stride 12) */
s32 wm_8009623C(u32 word0, u32 word1, u32 word2)
{
    u32 counter = q_lw(D_8009D808);
    if (counter >= QUEUE_MAX) return -1;

    u32 stride_mul = q_lw(D_8009BE44);
    u32 base = q_lw(D_8009BE08);
    u32 record_idx = (stride_mul * 33) << 5; /* stride_mul * 32 + stride_mul */
    u32 addr = base + record_idx + counter * 12;

    q_sw(D_8009D808, counter + 1);
    q_sw(addr + 0, word0);
    q_sw(addr + 4, word1);
    q_sw(addr + 8, word2);
    return 0;
}

/* wm_800962B0: 4-word record writer (different stride) */
s32 wm_800962B0(u32 word0, u32 word1, u32 word2, u32 word3)
{
    u32 counter = q_lw(D_8009D808);
    if (counter >= QUEUE_MAX) return -1;

    u32 stride_mul = q_lw(D_8009BE44);
    u32 base = q_lw(D_8009D3C0);
    u32 record_idx = ((stride_mul * 3 - stride_mul) << 7);
    u32 addr = base + record_idx + counter * 16;

    q_sw(D_8009D808, counter + 1);
    q_sw(addr + 0, word0);
    q_sw(addr + 4, word1);
    q_sw(addr + 8, word2);
    q_sw(addr + 12, word3);
    return 0;
}

/* wm_800963E4: record initializer (zero-fill pattern) */
void wm_800963E4(u32 record_addr)
{
    q_sw(record_addr + 0, 0x09000000);
    q_sw(record_addr + 4, 0);
    q_sw(record_addr + 8, 0);
    q_sw(record_addr + 12, 0);
    q_sw(record_addr + 16, 0);
    q_sw(record_addr + 20, 0);
    q_sw(record_addr + 24, 0);
    q_sw(record_addr + 28, 0);
    q_sw(record_addr + 32, 0);
    q_sw(record_addr + 36, 0);
    q_sw(record_addr + 40, 0);
    q_sw(record_addr + 44, 0);
    q_sw(record_addr + 48, 0);
}

/* wm_800964B0: record copier (40-byte records) */
void wm_800964B0(u32 src, u32 dst)
{
    s32 i;
    for (i = 0; i < 10; i++) {
        q_sw(dst + i * 4, q_lw(src + i * 4));
    }
}

/* wm_800968E0: zero-fill N records */
void wm_800968E0(u32 addr, u32 count)
{
    u32 i;
    for (i = 0; i < count * 10; i++) {
        q_sw(addr + i * 4, 0);
    }
}

/* wm_80086700: simple 2-word record writer */
void wm_80086700(u32 addr, u32 val)
{
    q_sw(addr, val);
    q_sw(addr + 4, 0);
}
