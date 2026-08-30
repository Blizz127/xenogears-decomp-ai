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

/* wm_800963E4 — retail [0x800963E4, 0x800964B0): sort a 12-byte record
 * block {sector, size, buf} ascending by sector (bubble sort with the
 * record swap at 0x80096418-0x80096470, walking back one record after a
 * swap at 0x80096480-88, forward otherwise at 0x8009648C-90, until a record
 * with sector == 0 (0x80096494-9C). Nothing is written if the second record
 * is empty (0x800963EC-F4). W34C11: the previous body wrote 0x09000000 and
 * zeros over the block (no retail counterpart). */
void wm_800963E4(u32 record_addr)
{
    u32 cur = record_addr;
    u32 next = record_addr + 12u;

#if defined(WM_963E4_MUTANT_M6)             /* pre-W34C11 shape */
    { u32 i; q_sw(record_addr, 0x09000000u); for (i = 4u; i < 52u; i += 4u) q_sw(record_addr + i, 0u); return; }
#endif
    if (q_lw(next) == 0u)
        return;
    for (;;) {
        u32 cs = q_lw(cur), ns = q_lw(next);
#if defined(WM_963E4_MUTANT_M7)             /* no ordering */
        if (0) {
#elif defined(WM_963E4_MUTANT_M8)           /* descending */
        if (ns > cs) {
#else
        if (ns < cs) {                      /* 0x8009640C sltu next < cur */
#endif
            u32 t1 = q_lw(cur + 4u), t2 = q_lw(cur + 8u);
            q_sw(cur, ns); q_sw(cur + 4u, q_lw(next + 4u)); q_sw(cur + 8u, q_lw(next + 8u));
            q_sw(next, cs); q_sw(next + 4u, t1); q_sw(next + 8u, t2);
            if (record_addr < cur) {        /* 0x80096474 sltu a2 < a1 */
                cur -= 12u; next -= 12u;    /* 0x80096480-88 */
            }
        } else {
            cur += 12u; next += 12u;        /* 0x8009648C-90 */
        }
        if (q_lw(next) == 0u)               /* 0x80096494-9C */
            break;
    }
}

/* wm_800964B0: record copier (40-byte records) */
void wm_800964B0(u32 src, u32 dst)
{
    s32 i;
    for (i = 0; i < 10; i++) {
        q_sw(dst + i * 4, q_lw(src + i * 4));
    }
}

/* wm_80086700: simple 2-word record writer */
void wm_80086700(u32 addr, u32 val)
{
    q_sw(addr, val);
    q_sw(addr + 4, 0);
}
