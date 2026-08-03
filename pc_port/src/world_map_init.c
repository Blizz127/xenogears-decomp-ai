/*
 * W2 — Native WorldMapMain pre-loop initialization slice.
 *
 * Retail WorldMapMain @ 0x80070CFC (overlay world_map.bin loaded at 0x8006FAF0).
 * This file implements only the proven Lahan path through the last init call
 * (jal wm_80071B9C @ 0x80070FF4) and stops before retail PC 0x80071000, which is
 * the first instruction of outer-loop / dispatch ownership (first path to
 * wm_800712D0 @ 0x80071094).
 *
 * Gated by XENO_WORLD_INIT=1. Default remains pure placeholder (hasOverlay=0).
 */
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "main/main.h"
#include "system/memory.h"
#include "system/archive.h"
#include "psyq/libgpu.h"
#include "psyq/libgte.h"
#include "psyq/libetc.h"
#include "psx_memory.h"

/* Retail layout */
#define WM_OVERLAY_BASE          0x8006FAF0u
#define WM_ENTRY                 0x80070CFCu
#define WM_CUT_BEFORE_LOOP       0x80071000u
#define WM_MEM_START             0x8009BBB0u
#define WM_HEAP_START            0x8009D80Cu
#define WM_OVERLAY_IMAGE_SIZE    180422u /* LZSS header + disc/world_map.bin */
#define WM_NOMINAL_BSS_SPAN      (WM_MEM_START - WM_OVERLAY_BASE) /* 180416 */
#define WM_BSS_OVERLAP_BYTES     (WM_OVERLAY_IMAGE_SIZE - WM_NOMINAL_BSS_SPAN) /* 6 */

/* GameState transition tuple (in-place; g_GameState @ 0x8006D634) */
#define GS_SELECTOR_ABS          0x8006F94Eu /* +0x231A */
#define GS_HEADING_ABS           0x8006F950u /* +0x231C */
#define GS_ARG2_ABS              0x8006F952u /* +0x231E */
#define GS_ENTRANCE_ABS          0x8006F954u /* +0x2320 */
#define GS_SEED_1930_ABS         0x8006EF64u /* +0x1930, a1 for entrance table */

/* World-map state written by init (exact retail destinations) */
#define WM_WORLD_INDEX_ABS       0x8009BD0Cu
#define WM_ENTRANCE_STATE_ABS    0x8009C5A8u
#define WM_HEADING_STATE_ABS     0x8009C584u
#define WM_ARG2_STATE_ABS        0x8009D3D4u
#define WM_FLAG_C894_ABS         0x8009C894u
#define WM_ZERO_BBC4_ABS         0x8009BBC4u

/* wm_80071B9C tables / outputs (in overlay image / BSS) */
#define WM_THRESH_TABLE_ABS      0x8009B564u
#define WM_RECORD_TABLE_ABS      0x8009B57Cu
#define WM_RECORD_GE8_ABS        0x8009B58Cu
#define WM_SLOT_C610_ABS         0x8009C610u

/* wm_8007369C / wm_80095F78 / wm_80073300 stores */
#define WM_ALLOC_BC38_ABS        0x8009BC38u
#define WM_ALLOC_BCB0_ABS        0x8009BCB0u
#define WM_ALLOC_BE08_ABS        0x8009BE08u
#define WM_ALLOC_D7D4_ABS        0x8009D7D4u
#define WM_ALLOC_D3C0_ABS        0x8009D3C0u
#define WM_MODE_BE10_ABS         0x8009BE10u
#define WM_CLR_BCB8_ABS          0x8009BCB8u
#define WM_CLR_BE44_ABS          0x8009BE44u
#define WM_CLR_CD44_ABS          0x8009CD44u
#define WM_CLR_D808_ABS          0x8009D808u
#define WM_CLR_D7C4_ABS          0x8009D7C4u
#define WM_CLR_C660_ABS          0x8009C660u
#define WM_BYTE_C58F_ABS         0x8009C58Fu

/* Below-overlay main BSS touched by entry (always) */
#define WM_FLAG_91AE_ABS         0x800691AEu

/* Cut-line proof: first instruction of loop ownership (must never execute). */
#define WM_LOOP_JAL_712D0        0x80071094u

#define WM_U8(a)  (*(u8*)PSX_ADDR(a))
#define WM_U16(a) (*(u16*)PSX_ADDR(a))
#define WM_S16(a) (*(s16*)PSX_ADDR(a))
#define WM_U32(a) (*(u32*)PSX_ADDR(a))

extern void PcPort_WorldMapPlaceholderMain(void);
extern void func_8003634C(void);
extern void EnterCriticalSection(void);
extern void ExitCriticalSection(void);
extern void FlushCache(void);
extern int VSync(int mode);
extern u32 g_ArchiveDebugTable;
extern int ArchiveDecodeSize(int entryIndex);

/* Retail registers a vsync IRQ callback (libetc). Host has no PSX IRQ table;
 * accepting the function pointer is enough for init ordering. */
void func_8004B7D0(void (*fn)(void))
{
    (void)fn;
}

/* Local memcpy — avoid string.h vs game strlen conflict. */
static void wm_memcpy(void* dst, const void* src, unsigned n)
{
    u8* d = (u8*)dst;
    const u8* s = (const u8*)src;
    while (n--)
        *d++ = *s++;
}

static int s_wm712d0_hits;
static int s_wm_drawotag_hits;

/* Instrumentation targets (never called on the init path). */
void wm_800712D0_should_not_run(void)
{
    s_wm712d0_hits++;
    fprintf(stderr, "[worldmap-init] ERROR: wm_800712D0 reached (hit=%d)\n",
            s_wm712d0_hits);
}

static int world_init_enabled(void)
{
    const char* v = getenv("XENO_WORLD_INIT");
    return v != NULL && v[0] == '1' && v[1] == '\0';
}

int PcPort_WorldMapInitEnabled(void)
{
    return world_init_enabled();
}

/* Retail: DrawSync/Vsync around critical + FlushCache. */
static void wm_800762FC(void)
{
    DrawSync(0);
    VSync(0);
    EnterCriticalSection();
    DrawSync(0);
    VSync(0);
    FlushCache();
    ExitCriticalSection();
}

/* Retail wm_80095F78: HeapAlloc pair + table clears. g_ArchiveDebugTable==0 is
 * the normal path (Lahan / retail CD). */
static void wm_80095F78(void)
{
    u32 dbg0 = g_ArchiveDebugTable;
    u32 dbg1 = g_ArchiveDebugTable;
    int use_debug_path = (dbg0 != 0) && (dbg1 != 0);
    void* p;
    int i;

    WM_U32(WM_CLR_BCB8_ABS) = 0;
    WM_U32(WM_CLR_BE44_ABS) = 0;
    WM_U32(WM_CLR_CD44_ABS) = 0;

    if (!use_debug_path) {
        u32* pClr = (u32*)PSX_ADDR(WM_CLR_D7C4_ABS);
        for (i = 0; i < 0x10; i++)
            pClr[-i] = 0; /* retail walks 16 words downward from D7C4 */

        p = HeapAlloc(0x4200, 0);
        WM_U32(WM_ALLOC_BE08_ABS) = (u32)(uintptr_t)p;
        p = HeapAlloc(0x800, 0);
        WM_U32(WM_ALLOC_D7D4_ABS) = (u32)(uintptr_t)p;
    } else {
        u32* pClr = (u32*)PSX_ADDR(WM_CLR_C660_ABS);
        for (i = 0; i < 0x10; i++)
            pClr[-i] = 0;
        p = HeapAlloc(0x5800, 0);
        WM_U32(WM_ALLOC_D3C0_ABS) = (u32)(uintptr_t)p;
        p = HeapAlloc(0x800, 0);
        WM_U32(WM_ALLOC_D7D4_ABS) = (u32)(uintptr_t)p;
    }

    WM_U32(WM_CLR_D808_ABS) = 0;
    {
        u8* pB = (u8*)PSX_ADDR(WM_BYTE_C58F_ABS);
        for (i = 0; i < 8; i++)
            pB[-i] = 0;
    }
}

static void wm_8007369C(void)
{
    void* p = HeapAlloc(0x1000, 0);
    WM_U32(WM_ALLOC_BC38_ABS) = (u32)(uintptr_t)p;
    p = HeapAlloc(0x1000, 0);
    WM_U32(WM_ALLOC_BCB0_ABS) = (u32)(uintptr_t)p;
}

/* Lahan: halfword @ 0x8006EE68 is not cold-defaulted; path selects mode 1/2. */
static void wm_80073300(void)
{
    u16 hw = WM_U16(0x8006EE68u);
    u32 mode;

    if (hw & 0x4000) {
        u32 idx = hw & 0x1FFF;
        /* Jump-table path needs cold-default table; not on Lahan. */
        if (idx < 5) {
            fprintf(stderr,
                    "[worldmap-init] ERROR: wm_80073300 jump-table idx=%u "
                    "requires cold-default (not Lahan path)\n",
                    idx);
            mode = 1;
        } else {
            return;
        }
    } else {
        u8 b0 = WM_U8(0x8006F8E5u);
        u8 b1 = WM_U8(0x8006F8E6u);
        u8 b2 = WM_U8(0x8006F8E7u);
        if ((b0 | b1 | b2) != 0)
            mode = 2;
        else
            mode = 1;
    }
    WM_U32(WM_MODE_BE10_ABS) = mode;
}

/* Retail wm_80071B9C(entrance, seed_from_gs_1930). Instruction-faithful. */
static void wm_80071B9C(u32 entrance, s32 seed_a1)
{
    s16* pRec;
    s16 base0, y, z, w;
    u32 a0;
    u8* pV1;

    if ((s32)entrance < 8) {
        /* v1 = 0x8009B564; first threshold load from +2 */
        pV1 = (u8*)PSX_ADDR(WM_THRESH_TABLE_ABS);
        {
            u16 thr = *(u16*)(pV1 + 2);
            a0 = 1;
            if (!((s32)seed_a1 < (s32)(s16)thr)) {
                /* fallthrough: v1 += 2; then loop v1 += 2, a0++ */
                pV1 += 2;
                for (;;) {
                    pV1 += 2;
                    thr = *(u16*)pV1;
                    if ((s32)seed_a1 < (s32)(s16)thr)
                        break;
                    a0++;
                }
            }
        }
        pRec = (s16*)((u8*)PSX_ADDR(WM_RECORD_TABLE_ABS) + (a0 << 3));
        WM_U32(WM_SLOT_C610_ABS) = a0 - 1;
    } else {
        pRec = (s16*)((u8*)PSX_ADDR(WM_RECORD_GE8_ABS) + (entrance << 3));
    }

    base0 = pRec[0];
    y = pRec[1];
    z = pRec[2];
    w = pRec[3];

    WM_U32(0x8009D3C4u) = (u32)(base0 + 1);
    WM_U32(0x8009C174u) = (u32)(base0 + 3);
    WM_U32(0x8009C17Cu) = (u32)(base0 + 2);
    WM_U32(0x8009D3D0u) = (u32)(base0 + 5);
    WM_U32(0x8009CC98u) = (u32)(base0 + 4);
    WM_U32(0x8009D800u) = (u32)(base0 + 7);
    WM_U32(0x8009D3C8u) = (u32)(base0 + 6);
    WM_U32(0x8009BCD8u) = (u32)(base0 + 9);
    WM_U32(0x8009BCC8u) = (u32)(base0 + 8);
    WM_U32(0x8009D2B4u) = (u32)(s32)z;
    WM_U32(0x8009D160u) = (u32)(s32)y;
    WM_U32(0x8009BD08u) = (u32)(base0 + 10);
    WM_U32(0x8009D7CCu) = (u32)(s32)w;
}

static int ensure_world_overlay_image(u32* out_final_write)
{
    u32* entry = (u32*)PSX_ADDR(WM_ENTRY);
    u32 fingerprint = entry[0];
    FILE* fp;
    size_t n;
    u8* dst = (u8*)PSX_ADDR(WM_OVERLAY_BASE);

    *out_final_write = WM_OVERLAY_BASE + WM_OVERLAY_IMAGE_SIZE;

    /* Retail entry starts with addiu $sp, $sp, -40 → 0x27BDFFD8 */
    if (fingerprint == 0x27BDFFD8u) {
        fprintf(stderr,
                "[worldmap-init] overlay resident fingerprint=0x%08x at 0x%08x\n",
                fingerprint, WM_ENTRY);
        return 0;
    }

    fprintf(stderr,
            "[worldmap-init] overlay fingerprint miss (0x%08x); planting "
            "disc/world_map.bin (%u bytes) at 0x%08x\n",
            fingerprint, WM_OVERLAY_IMAGE_SIZE, WM_OVERLAY_BASE);

    fp = fopen("disc/world_map.bin", "rb");
    if (fp == NULL) {
        fprintf(stderr, "[worldmap-init] ERROR: cannot open disc/world_map.bin\n");
        return -1;
    }
    n = fread(dst, 1, WM_OVERLAY_IMAGE_SIZE, fp);
    fclose(fp);
    if (n != WM_OVERLAY_IMAGE_SIZE) {
        fprintf(stderr, "[worldmap-init] ERROR: short read %zu\n", n);
        return -1;
    }
    if (*(u32*)PSX_ADDR(WM_ENTRY) != 0x27BDFFD8u) {
        fprintf(stderr, "[worldmap-init] ERROR: planted image bad entry\n");
        return -1;
    }
    return 0;
}

/*
 * Lahan-only native transcription of WorldMapMain from 0x80070CFC through
 * return from jal 0x80071B9C (delay slot 0x80070FF8). Does not execute
 * 0x80071000+.
 */
static int world_map_main_init_lahan(void)
{
    u16 entrance_hw = WM_U16(GS_ENTRANCE_ABS);
    u16 selector, heading, arg2, entrance;
    s32 world_index;
    s32 seed_1930;

    /* Always: flag byte @ 0x800691AE = 1 (retail before cold branch). */
    WM_U8(WM_FLAG_91AE_ABS) = 1;

    if (entrance_hw == 0) {
        fprintf(stderr,
                "[worldmap-init] ERROR: entrance halfword is 0 — cold-default "
                "branch not implemented in W2 (Lahan slice requires entrance!=0)\n");
        return -1;
    }

    /* Shared path @ 0x80070F38 */
    HeapChangeCurrentUser(3, NULL);
    ArchiveSetIndex(0x24, 0);
    wm_80095F78();
    wm_8007369C();
    wm_80073300();

    /* Bit 0x8000 on entrance → flag @ 0x8009C894 */
    if (entrance_hw & 0x8000)
        WM_U32(WM_FLAG_C894_ABS) = 1;
    else
        WM_U32(WM_FLAG_C894_ABS) = 0;

    /* Tuple normalize @ 0x80070F90+ — read GS in place, no shadow copy. */
    seed_1930 = (s32)(s16)WM_U16(GS_SEED_1930_ABS);
    entrance = WM_U16(GS_ENTRANCE_ABS);
    selector = WM_U16(GS_SELECTOR_ABS);
    arg2 = WM_U16(GS_ARG2_ABS);
    heading = WM_U16(GS_HEADING_ABS);

    WM_U32(WM_ZERO_BBC4_ABS) = 0;
    entrance &= 0x7FFF;
    world_index = (s32)(selector & 0x3FFF) - 0x400;

    WM_U32(WM_WORLD_INDEX_ABS) = (u32)world_index;
    WM_U32(WM_ARG2_STATE_ABS) = (u32)arg2;
    WM_U32(WM_HEADING_STATE_ABS) = (u32)heading;
    WM_U32(WM_ENTRANCE_STATE_ABS) = (u32)entrance;
    WM_U16(GS_ENTRANCE_ABS) = entrance;

    fprintf(stderr,
            "[worldmap-init] selector=0x%04x world_index=%d entrance=%d "
            "heading=0x%04x arg2=%d seed_1930=0x%04x\n",
            selector, world_index, entrance, heading, arg2,
            (u16)seed_1930);

    if (selector != 0x0400 || world_index != 0 || entrance != 1 ||
        heading != 0x0E00 || arg2 != 1) {
        fprintf(stderr,
                "[worldmap-init] WARN: tuple is not the Lahan proof values "
                "(continuing with natural decode)\n");
    }

    wm_80071B9C(entrance, seed_1930);

    fprintf(stderr,
            "[worldmap-init] cut-before-loop retail_pc=0x%08x "
            "(next would be loop ownership; wm_800712D0 retail=0x%08x)\n",
            WM_CUT_BEFORE_LOOP, WM_LOOP_JAL_712D0);
    return 0;
}

void PcPort_WorldMapInitMain(void)
{
    int decoded_size;
    u32 final_write = 0;
    u8 bss_before[16];
    u8 bss_after[16];

    s_wm712d0_hits = 0;
    s_wm_drawotag_hits = 0;

    fprintf(stderr, "[worldmap-init] entry\n");

    /* ArchiveDecodeSize = compressed CD payload size (alloc for LoadGameStateOverlay).
     * LZSS header / disc extract decompressed size is WM_OVERLAY_IMAGE_SIZE (180422). */
    decoded_size = ArchiveDecodeSize(0x0F);
    fprintf(stderr,
            "[worldmap-init] LoadGameStateOverlay(3), archive=0x0F\n"
            "[worldmap-init] decoded_size=%d (0x%x) compressed\n"
            "[worldmap-init] decompressed_size=%u (0x%x)\n"
            "[worldmap-init] destination=0x%08x\n"
            "[worldmap-init] final_write=0x%08x\n"
            "[worldmap-init] nominal_bss_span=%u bss_overlap_bytes=%u "
            "(retail intentional: image ends 6 bytes into cleared BSS)\n",
            decoded_size, (unsigned)decoded_size,
            WM_OVERLAY_IMAGE_SIZE, WM_OVERLAY_IMAGE_SIZE,
            WM_OVERLAY_BASE,
            WM_OVERLAY_BASE + WM_OVERLAY_IMAGE_SIZE,
            WM_NOMINAL_BSS_SPAN, WM_BSS_OVERLAP_BYTES);

    wm_memcpy(bss_before, PSX_ADDR(WM_MEM_START), 16);

    if (ensure_world_overlay_image(&final_write) != 0) {
        fprintf(stderr, "[worldmap-init] falling back to placeholder only\n");
        PcPort_WorldMapPlaceholderMain();
        return;
    }

    fprintf(stderr, "[worldmap-init] final_write=0x%08x\n", final_write);
    wm_memcpy(bss_after, PSX_ADDR(WM_MEM_START), 16);
    fprintf(stderr,
            "[worldmap-init] first 16 @ 0x8009BBB0 before_plant/load context: "
            "%02x%02x%02x%02x...\n",
            bss_before[0], bss_before[1], bss_before[2], bss_before[3]);
    fprintf(stderr,
            "[worldmap-init] first 16 @ 0x8009BBB0 after image present: "
            "%02x%02x%02x%02x%02x%02x (6-byte overlay tail into BSS start)\n",
            bss_after[0], bss_after[1], bss_after[2], bss_after[3],
            bss_after[4], bss_after[5]);

    /* GPU / geom prologue (retail 0x80070D08–0x80070D34). */
    wm_800762FC();
    DrawSync(0);
    VSync(0);
    func_8004B7D0((void (*)(void))func_8003634C);
    InitGeom();

    if (world_map_main_init_lahan() != 0) {
        fprintf(stderr, "[worldmap-init] init failed; placeholder\n");
        PcPort_WorldMapPlaceholderMain();
        return;
    }

    if (s_wm712d0_hits != 0 || s_wm_drawotag_hits != 0) {
        fprintf(stderr,
                "[worldmap-init] ERROR: forbidden path hit "
                "wm712d0=%d drawotag=%d\n",
                s_wm712d0_hits, s_wm_drawotag_hits);
    }

    /* Known-safe hollow UI — W2 intentionally still shows NOT YET PORTED. */
    PcPort_WorldMapPlaceholderMain();
}
