/*
 * W2 / W3B / W4C — Native world-map initialization ladder.
 *
 * Retail WorldMapMain @ 0x80070CFC (overlay world_map.bin loaded at 0x8006FAF0).
 *
 * W2: pre-loop init through jal wm_80071B9C @ 0x80070FF4; cut before 0x80071000.
 * W3B: one-shot mode initializer 0x80071CDC (first-wave archive queue).
 * W4C: second-wave 0x80071EF0 → ArchiveDataSync poll → 0x80073530; cut before
 *      retail PC 0x800722BC (jal 0x8009766C). Does not enter full 0x80072238,
 *      wm_800712D0, or any frame path.
 *
 * Gates (deepest implies lower):
 *   XENO_WORLD_INIT=1
 *   XENO_WORLD_MODE_INIT=1
 *   XENO_WORLD_SECOND_WAVE=1
 * Default remains pure placeholder (hasOverlay=0).
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

/* GameState transition tuple offsets. Retail absolute addresses
 * (0x8006F94E..) equal g_GameState+off when g_GameState lives at 0x8006D634
 * in PSX RAM. The port's g_pGameState points at the host g_GameState blob
 * that field already wrote; consume that pointer in place (no shadow copy). */
#define GS_OFF_SELECTOR          0x231Au
#define GS_OFF_HEADING           0x231Cu
#define GS_OFF_ARG2              0x231Eu
#define GS_OFF_ENTRANCE          0x2320u
#define GS_OFF_SEED_1930         0x1930u

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
#define WM_MAIN_LOOP             0x80071034u
#define WM_POST_INIT_RESUME      0x80070FFCu
#define WM_MODE_INIT_RETAIL      0x80071CDCu
#define WM_MODE_UPDATE_RETAIL    0x80072238u
#define WM_MODE_POST_RETAIL      0x8007299Cu
#define WM_DISPATCH_TABLE        0x8009A058u
#define WM_PHASE_D7CC            0x8009D7CCu

/* Mode-init BSS / request list (overlay) */
#define WM_PTR_CD34              0x8009CD34u
#define WM_PTR_BDF8              0x8009BDF8u
#define WM_REQ_D3F8              0x8009D3F8u
#define WM_REQ_D3FC              0x8009D3FCu
#define WM_CNT_C170              0x8009C170u

/* W4C second-wave source IDs (written by wm_80071B9C / W2) */
#define WM_ID_C17C               0x8009C17Cu
#define WM_ID_C174               0x8009C174u
#define WM_ID_D3C4               0x8009D3C4u
/* Destination pointer slots */
#define WM_DST_C59C              0x8009C59Cu
#define WM_DST_BD20              0x8009BD20u
#define WM_DST_C180              0x8009C180u
/* Queue entries (8-byte stride) after D3F8 */
#define WM_REQ_D400              0x8009D400u
#define WM_REQ_D404              0x8009D404u
#define WM_REQ_D408              0x8009D408u
#define WM_REQ_D40C              0x8009D40Cu
#define WM_REQ_D410              0x8009D410u
#define WM_REQ_D414              0x8009D414u
/* 73530 fixup outputs */
#define WM_FIX_D308              0x8009D308u
#define WM_FIX_CD48              0x8009CD48u
#define WM_FIX_C7EC              0x8009C7ECu
#define WM_FIX_BD30              0x8009BD30u
#define WM_FIX_D784              0x8009D784u
#define WM_FIX_BCC0              0x8009BCC0u
#define WM_FIX_D7C8              0x8009D7C8u
#define WM_FIX_D77C              0x8009D77Cu
#define WM_FIX_D73C              0x8009D73Cu
#define WM_FIX_D3F4              0x8009D3F4u
#define WM_FIX_BD00              0x8009BD00u
/* Hard cut: first insn of broad mode-enter after second-wave */
#define WM_CUT_BEFORE_BROAD      0x800722BCu
#define WM_BROAD_9766C           0x8009766Cu

/* Channel ID tables live in host g_GameState (retail abs inside GS span). */
#define GS_OFF_CH_ID0            0x1D34u /* retail 0x8006F368 */
#define GS_OFF_SEC_BASE          0x030Cu /* retail 0x8006D940; stride 164 per id */

#define WM_U8(a)  (*(u8*)PSX_ADDR(a))
#define WM_U16(a) (*(u16*)PSX_ADDR(a))
#define WM_S16(a) (*(s16*)PSX_ADDR(a))
#define WM_U32(a) (*(u32*)PSX_ADDR(a))

/* Safety ceiling for ArchiveDataSync poll (never silent hang). */
#define WM_SECOND_WAVE_POLL_MAX  100000

extern void PcPort_WorldMapPlaceholderMain(void);
extern void func_8003634C(void);
extern void EnterCriticalSection(void);
extern void ExitCriticalSection(void);
extern void FlushCache(void);
extern int VSync(int mode);
extern u32 g_ArchiveDebugTable;
extern int ArchiveDecodeSize(int entryIndex);
extern int ArchiveDecodeAlignedSize(unsigned int entryIndex);
extern int func_80029AFC(void* pEntries, int arg1, int arg2);
extern int ArchiveDataSync(void);
extern void* HeapAlloc(u_int allocSize, u_int allocFlags);
extern u_int HeapFree(void* pMem);
extern void* LZSSHeapDecompress(void* pCompressed, int flags);
extern void* g_pGameState;

#define GS_U8(off)  (*(u8*)((u8*)g_pGameState + (off)))
#define GS_U16(off) (*(u16*)((u8*)g_pGameState + (off)))
#define GS_S16(off) (*(s16*)((u8*)g_pGameState + (off)))

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

/* HeapAlloc returns a host pointer into g_PsxRam; store retail-style KUSEG. */
static u32 host_ptr_to_psx_u32(void* p)
{
    uintptr_t host;
    uintptr_t base;
    if (p == NULL)
        return 0;
    host = (uintptr_t)p;
    base = (uintptr_t)g_PsxRam;
    if (host >= base && host < base + (uintptr_t)PSX_RAM_SIZE)
        return 0x80000000u | (u32)(host - base);
    /* Truncate host pointer (legacy path); still usable if in low 4G. */
    return (u32)host;
}

/* Resolve a 32-bit guest/host value to a host pointer for Heap/LZSS APIs. */
static void* psx_u32_to_host(u32 p)
{
    if (p == 0)
        return NULL;
    if (p >= 0x80000000u && p < 0x80200000u)
        return PSX_ADDR(p);
    return (void*)(uintptr_t)p;
}

static int s_wm712d0_hits;
static int s_wm_drawotag_hits;
static int s_wm9766c_hits;
static int s_wm72238_hits;
static int s_wm7299c_hits;

/* Instrumentation targets (never called on the init path). */
void wm_800712D0_should_not_run(void)
{
    s_wm712d0_hits++;
    fprintf(stderr, "[worldmap-init] ERROR: wm_800712D0 reached (hit=%d)\n",
            s_wm712d0_hits);
}

void wm_8009766C_should_not_run(void)
{
    s_wm9766c_hits++;
    fprintf(stderr, "[worldmap-second-wave] ERROR: 0x8009766C reached (hit=%d)\n",
            s_wm9766c_hits);
}

void wm_80072238_should_not_run(void)
{
    s_wm72238_hits++;
    fprintf(stderr, "[worldmap-second-wave] ERROR: full 0x80072238 reached (hit=%d)\n",
            s_wm72238_hits);
}

void wm_8007299C_should_not_run(void)
{
    s_wm7299c_hits++;
    fprintf(stderr, "[worldmap-second-wave] ERROR: 0x8007299C reached (hit=%d)\n",
            s_wm7299c_hits);
}

static int env_flag_is_one(const char* name)
{
    const char* v = getenv(name);
    return v != NULL && v[0] == '1' && v[1] == '\0';
}

static int world_second_wave_enabled(void)
{
    return env_flag_is_one("XENO_WORLD_SECOND_WAVE");
}

static int world_mode_init_enabled(void)
{
    /* Second-wave implies mode-init. */
    return env_flag_is_one("XENO_WORLD_MODE_INIT") || world_second_wave_enabled();
}

static int world_init_enabled(void)
{
    /* Mode-init / second-wave imply W2 overlay/init path. */
    return env_flag_is_one("XENO_WORLD_INIT") || world_mode_init_enabled();
}

int PcPort_WorldMapInitEnabled(void)
{
    return world_init_enabled();
}

static void log_enabled_slices(void)
{
    int w2 = world_init_enabled();
    int w3 = world_mode_init_enabled();
    int w4 = world_second_wave_enabled();
    fprintf(stderr, "[worldmap] enabled slices:");
    if (!w2 && !w3 && !w4) {
        fprintf(stderr, " (none — placeholder only)\n");
        return;
    }
    if (w2)
        fprintf(stderr, " W2");
    if (w3)
        fprintf(stderr, ",W3B");
    if (w4)
        fprintf(stderr, ",W4C");
    fprintf(stderr, "\n");
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
    u16 entrance_hw;
    u16 selector, heading, arg2, entrance;
    s32 world_index;
    s32 seed_1930;

    if (g_pGameState == NULL) {
        fprintf(stderr, "[worldmap-init] ERROR: g_pGameState is NULL\n");
        return -1;
    }

    entrance_hw = GS_U16(GS_OFF_ENTRANCE);

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

    /* Tuple normalize @ 0x80070F90+ — read host g_pGameState in place. */
    seed_1930 = (s32)GS_S16(GS_OFF_SEED_1930);
    entrance = GS_U16(GS_OFF_ENTRANCE);
    selector = GS_U16(GS_OFF_SELECTOR);
    arg2 = GS_U16(GS_OFF_ARG2);
    heading = GS_U16(GS_OFF_HEADING);

    WM_U32(WM_ZERO_BBC4_ABS) = 0;
    entrance &= 0x7FFF;
    world_index = (s32)(selector & 0x3FFF) - 0x400;

    WM_U32(WM_WORLD_INDEX_ABS) = (u32)world_index;
    WM_U32(WM_ARG2_STATE_ABS) = (u32)arg2;
    WM_U32(WM_HEADING_STATE_ABS) = (u32)heading;
    WM_U32(WM_ENTRANCE_STATE_ABS) = (u32)entrance;
    GS_U16(GS_OFF_ENTRANCE) = entrance;

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
            "[worldmap-init] w2-complete retail_resume=0x%08x "
            "(W2 cut docs 0x%08x; main loop 0x%08x)\n",
            WM_POST_INIT_RESUME, WM_CUT_BEFORE_LOOP, WM_MAIN_LOOP);
    return 0;
}

/*
 * W3B — native transcription of retail mode initializer 0x80071CDC–0x80071EE8.
 * One-shot only; does not enter 0x80071034.
 */
static int wm_80071CDC_mode_init(void)
{
    int ch;
    u8 channel_id[3];
    u8 secondary_id[3];
    u32 aligned_size[3];
    u32 aligned_size_sec[3];
    void* allocation[3];
    void* allocation_sec[3];
    int request_count = 0;
    int queue_result;
    u8* pReq;
    u32* pCd34;
    u32* pBdf8;

    fprintf(stderr, "[worldmap-mode-init] entry\n");

    if (g_pGameState == NULL) {
        fprintf(stderr, "[worldmap-mode-init] ERROR: g_pGameState NULL\n");
        return -1;
    }

    pCd34 = (u32*)PSX_ADDR(WM_PTR_CD34);
    pBdf8 = (u32*)PSX_ADDR(WM_PTR_BDF8);

    for (ch = 0; ch < 3; ch++) {
        channel_id[ch] = GS_U8(GS_OFF_CH_ID0 + ch);
        secondary_id[ch] = 0xFF;
        aligned_size[ch] = 0;
        aligned_size_sec[ch] = 0;
        allocation[ch] = NULL;
        allocation_sec[ch] = NULL;

        if (channel_id[ch] != 0xFF) {
            aligned_size[ch] = (u32)ArchiveDecodeAlignedSize((u32)channel_id[ch] + 2u);
            allocation[ch] = HeapAlloc(aligned_size[ch], 0);
            pCd34[ch] = host_ptr_to_psx_u32(allocation[ch]);

            secondary_id[ch] = GS_U8(GS_OFF_SEC_BASE + (u32)channel_id[ch] * 164u);
            if (secondary_id[ch] != 0xFF) {
                aligned_size_sec[ch] =
                    (u32)ArchiveDecodeAlignedSize((u32)secondary_id[ch] + 0x13u);
                allocation_sec[ch] = HeapAlloc(aligned_size_sec[ch], 0);
                pBdf8[ch] = host_ptr_to_psx_u32(allocation_sec[ch]);
            } else {
                pBdf8[ch] = 0;
            }
        } else {
            pCd34[ch] = 0;
            pBdf8[ch] = 0;
        }

        fprintf(stderr,
                "[worldmap-mode-init] channel_id[%d]=0x%02x aligned_size[%d]=%u "
                "allocation[%d]=host:%p psx_u32=0x%08x "
                "secondary_id=0x%02x sec_size=%u sec_alloc=host:%p sec_psx=0x%08x\n",
                ch, channel_id[ch], ch, aligned_size[ch], ch, allocation[ch],
                pCd34[ch], secondary_id[ch], aligned_size_sec[ch],
                allocation_sec[ch], pBdf8[ch]);
    }

    /* Build retail 8-byte-stride request list at 0x8009D3F8. */
    WM_U32(WM_CNT_C170) = 0;
    pReq = (u8*)PSX_ADDR(WM_REQ_D3F8);
    request_count = 0;

    for (ch = 0; ch < 3; ch++) {
        if (channel_id[ch] == 0xFF)
            continue;

        *(u16*)(pReq + request_count * 8) = (u16)(channel_id[ch] + 2);
        *(u32*)(pReq + request_count * 8 + 4) = pCd34[ch];
        request_count++;
        /* Retail C170 increments only for primary channel entries. */
        WM_U32(WM_CNT_C170) = WM_U32(WM_CNT_C170) + 1u;

        if (secondary_id[ch] != 0xFF) {
            *(u16*)(pReq + request_count * 8) = (u16)(secondary_id[ch] + 0x13);
            *(u32*)(pReq + request_count * 8 + 4) = pBdf8[ch];
            request_count++;
        }
    }

    /* Terminator entry */
    *(u16*)(pReq + request_count * 8) = 0;
    *(u32*)(pReq + request_count * 8 + 4) = 0;

    fprintf(stderr, "[worldmap-mode-init] request_count=%d (primary_counter C170=%u)\n",
            request_count, WM_U32(WM_CNT_C170));
    {
        int r;
        for (r = 0; r < request_count; r++) {
            u16 idx = *(u16*)(pReq + r * 8);
            u32 dat = *(u32*)(pReq + r * 8 + 4);
            fprintf(stderr,
                    "[worldmap-mode-init] request[%d] archive_index=%u "
                    "pData_u32=0x%08x host=%p\n",
                    r, idx, dat, (void*)(uintptr_t)dat);
        }
    }

    queue_result = func_80029AFC(PSX_ADDR(WM_REQ_D3F8), 0, 0);
    fprintf(stderr, "[worldmap-mode-init] queue_submit=%d\n", queue_result);
    fprintf(stderr, "[worldmap-mode-init] exit\n");
    return queue_result;
}

/*
 * W4C — native transcription of retail second-wave setup 0x80071EF0–0x80071FE8.
 * Reads IDs from W2 wm_80071B9C stores; builds three-entry queue; submits via
 * func_80029AFC. Does not hard-code Lahan archive indices.
 */
static int wm_80071EF0_second_wave(void)
{
    u32 id0, id1, id2;
    u32 size0, size1, size2;
    void* alloc0;
    void* alloc1;
    void* alloc2;
    u32 psx0, psx1, psx2;
    int queue_result;
    u8* pReq;

    fprintf(stderr, "[worldmap-second-wave] entry\n");

    id0 = WM_U32(WM_ID_C17C);
    id1 = WM_U32(WM_ID_C174);
    id2 = WM_U32(WM_ID_D3C4);

    fprintf(stderr,
            "[worldmap-second-wave] source_id[0]=0x%08x (%u)\n"
            "[worldmap-second-wave] source_id[1]=0x%08x (%u)\n"
            "[worldmap-second-wave] source_id[2]=0x%08x (%u)\n",
            id0, id0, id1, id1, id2, id2);

    /* Retail: ArchiveDecodeAlignedSize(id) then HeapAlloc(size, 1). Order: C17C,
     * C174, D3C4. Archive indices are the stored IDs themselves (base0+N). */
    size0 = (u32)ArchiveDecodeAlignedSize(id0);
    alloc0 = HeapAlloc(size0, 1);
    psx0 = host_ptr_to_psx_u32(alloc0);
    WM_U32(WM_DST_C59C) = psx0;

    size1 = (u32)ArchiveDecodeAlignedSize(id1);
    alloc1 = HeapAlloc(size1, 1);
    psx1 = host_ptr_to_psx_u32(alloc1);
    WM_U32(WM_DST_BD20) = psx1;

    size2 = (u32)ArchiveDecodeAlignedSize(id2);
    alloc2 = HeapAlloc(size2, 1);
    psx2 = host_ptr_to_psx_u32(alloc2);
    WM_U32(WM_DST_C180) = psx2;

    fprintf(stderr,
            "[worldmap-second-wave] archive_id[0]=%u aligned_size[0]=%u "
            "destination_psx[0]=0x%08x host=%p\n"
            "[worldmap-second-wave] archive_id[1]=%u aligned_size[1]=%u "
            "destination_psx[1]=0x%08x host=%p\n"
            "[worldmap-second-wave] archive_id[2]=%u aligned_size[2]=%u "
            "destination_psx[2]=0x%08x host=%p\n",
            id0, size0, psx0, alloc0,
            id1, size1, psx1, alloc1,
            id2, size2, psx2, alloc2);

    if (alloc0 == NULL || alloc1 == NULL || alloc2 == NULL) {
        fprintf(stderr, "[worldmap-second-wave] ERROR: HeapAlloc failed\n");
        return -1;
    }

    /* Retail queue layout at 0x8009D3F8 (8-byte stride):
     *   [0] index=D3C4  pData=C180
     *   [1] index=C17C  pData=C59C
     *   [2] index=C174  pData=BD20
     *   [3] index=0     pData=0
     */
    pReq = (u8*)PSX_ADDR(WM_REQ_D3F8);
    WM_U16(WM_REQ_D410) = 0;
    WM_U32(WM_REQ_D3FC) = psx2;
    WM_U32(WM_REQ_D414) = 0;
    *(u16*)(pReq + 0) = (u16)id2;
    WM_U16(WM_REQ_D400) = (u16)id0;
    WM_U16(WM_REQ_D408) = (u16)id1;
    WM_U32(WM_REQ_D404) = psx0;
    WM_U32(WM_REQ_D40C) = psx1;

    fprintf(stderr,
            "[worldmap-second-wave] request_count=3\n"
            "[worldmap-second-wave] request[0] archive=%u pData_psx=0x%08x\n"
            "[worldmap-second-wave] request[1] archive=%u pData_psx=0x%08x\n"
            "[worldmap-second-wave] request[2] archive=%u pData_psx=0x%08x\n",
            (unsigned)id2, psx2,
            (unsigned)id0, psx0,
            (unsigned)id1, psx1);

    queue_result = func_80029AFC(PSX_ADDR(WM_REQ_D3F8), 0, 0);
    fprintf(stderr, "[worldmap-second-wave] queue_submit=%d\n", queue_result);
    fprintf(stderr, "[worldmap-second-wave] submitted\n");
    return queue_result;
}

/*
 * Retail glue 0x800722A0–0x800722B0:
 *   do { v = ArchiveDataSync(); } while (v >= 3);
 * Native sync archive typically exits on first poll (v==0).
 */
static int wm_second_wave_poll(void)
{
    int first = -1;
    int last = -1;
    int count = 0;
    int v;

    for (;;) {
        v = ArchiveDataSync();
        if (first < 0)
            first = v;
        last = v;
        count++;
        if (v < 3)
            break;
        if (count >= WM_SECOND_WAVE_POLL_MAX) {
            fprintf(stderr,
                    "[worldmap-second-wave] ERROR: poll safety ceiling "
                    "(%d) hit last=%d\n",
                    WM_SECOND_WAVE_POLL_MAX, last);
            return -1;
        }
        /* Yield so a pathological non-zero path cannot hard-lock the host. */
        if ((count & 0x3FF) == 0)
            VSync(0);
    }

    fprintf(stderr, "[worldmap-second-wave] poll_first=%d\n", first);
    fprintf(stderr, "[worldmap-second-wave] poll_count=%d\n", count);
    fprintf(stderr, "[worldmap-second-wave] poll_final=%d\n", last);
    return 0;
}

/*
 * W4C — native transcription of retail 0x80073530–0x80073698.
 * LZSSHeapDecompress(*C180), HeapFree(compressed), relative→KUSEG fixups.
 */
static int wm_80073530_fixup(void)
{
    u32 compressed_psx;
    u32 decompressed_psx;
    void* compressed_host;
    void* decompressed_host;
    u32 base;
    u32 rel;
    u32 abs;
    u32 s0_psx;
    u32 a0_slot;
    int i;
    int relocated = 0;
    u32 decomp_hdr_size = 0;

    fprintf(stderr, "[worldmap-second-wave] fixup_entry\n");

    compressed_psx = WM_U32(WM_DST_C180);
    compressed_host = psx_u32_to_host(compressed_psx);
    if (compressed_host == NULL) {
        fprintf(stderr, "[worldmap-second-wave] ERROR: C180 compressed NULL\n");
        return -1;
    }

    /* LZSS stream begins with u32 decompressed size (retail / port API). */
    decomp_hdr_size = *(u32*)compressed_host;

    fprintf(stderr,
            "[worldmap-second-wave] compressed_psx=0x%08x host=%p "
            "lzss_hdr_size=%u\n",
            compressed_psx, compressed_host, decomp_hdr_size);

    /* Retail: a1=0, a0=compressed; LZSSHeapDecompress(p, flags=0). */
    decompressed_host = LZSSHeapDecompress(compressed_host, 0);
    decompressed_psx = host_ptr_to_psx_u32(decompressed_host);
    WM_U32(WM_DST_C180) = decompressed_psx;

    fprintf(stderr,
            "[worldmap-second-wave] decompressed_psx=0x%08x host=%p\n",
            decompressed_psx, decompressed_host);

    /* Free compressed allocation (retail order: store new C180, then free old). */
    HeapFree(compressed_host);

    if (decompressed_host == NULL || decompressed_psx == 0) {
        fprintf(stderr, "[worldmap-second-wave] ERROR: LZSSHeapDecompress failed\n");
        return -1;
    }

    /* All further arithmetic is in KUSEG space; memory ops via PSX_ADDR. */
    base = decompressed_psx;

    /* Header slots: base + *(base+off) → named BSS. Retail order. */
    rel = *(u32*)((u8*)PSX_ADDR(base) + 4);
    s0_psx = base + rel; /* used later */

    rel = *(u32*)((u8*)PSX_ADDR(base) + 12);
    WM_U32(WM_FIX_D308) = base + rel;
    relocated++;

    rel = *(u32*)((u8*)PSX_ADDR(base) + 8);
    WM_U32(WM_FIX_CD48) = base + rel;
    relocated++;

    rel = *(u32*)((u8*)PSX_ADDR(base) + 20);
    WM_U32(WM_FIX_C7EC) = base + rel;
    relocated++;

    rel = *(u32*)((u8*)PSX_ADDR(base) + 16);
    WM_U32(WM_FIX_BD30) = base + rel;
    relocated++;

    rel = *(u32*)((u8*)PSX_ADDR(base) + 28);
    WM_U32(WM_FIX_D784) = base + rel;
    relocated++;

    rel = *(u32*)((u8*)PSX_ADDR(base) + 24);
    WM_U32(WM_FIX_BCC0) = base + rel;
    relocated++;

    rel = *(u32*)((u8*)PSX_ADDR(base) + 36);
    WM_U32(WM_FIX_D7C8) = base + rel;
    relocated++;

    rel = *(u32*)((u8*)PSX_ADDR(base) + 32);
    WM_U32(WM_FIX_D77C) = base + rel;
    relocated++;

    /* 16-entry table at D73C: for i in 0..15: D73C[i] = base + *(base+0x2C+4*i) */
    for (i = 0; i < 16; i++) {
        rel = *(u32*)((u8*)PSX_ADDR(base) + 0x2C + (u32)i * 4u);
        abs = base + rel;
        WM_U32(WM_FIX_D73C + (u32)i * 4u) = abs;
        relocated++;
    }

    /* s0 = base + *(base+4); D3F4 = s0 + *s0; BD00 = s0 + *(s0+4); then
     * rewrite four relative words at the BD00 structure in place. */
    {
        u32* p_s0 = (u32*)PSX_ADDR(s0_psx);
        u32 d3f4 = s0_psx + p_s0[0];
        u32 bd00 = s0_psx + p_s0[1];
        u32* p_v1;
        u32* p_a0;
        u32 w0, w1, w2, w3;

        WM_U32(WM_FIX_D3F4) = d3f4;
        WM_U32(WM_FIX_BD00) = bd00;
        relocated += 2;

        /* Snapshot relative words before any write (retail loads from $v1). */
        p_v1 = (u32*)PSX_ADDR(bd00);
        w0 = p_v1[0];
        w1 = p_v1[1];
        w2 = p_v1[2];
        w3 = p_v1[3];

        p_v1[0] = s0_psx + w0;
        a0_slot = WM_U32(WM_FIX_BD00);
        p_a0 = (u32*)PSX_ADDR(a0_slot);
        p_a0[1] = s0_psx + w1;
        p_a0[2] = s0_psx + w2;
        p_a0[3] = s0_psx + w3;
        relocated += 4;
    }

    fprintf(stderr,
            "[worldmap-second-wave] decompressed_size=%u (lzss header)\n"
            "[worldmap-second-wave] pointer_table_count=16\n"
            "[worldmap-second-wave] relocated_entries=%d\n"
            "[worldmap-second-wave] D73C[0]=0x%08x D73C[1]=0x%08x "
            "D308=0x%08x BD00=0x%08x\n",
            decomp_hdr_size, relocated,
            WM_U32(WM_FIX_D73C), WM_U32(WM_FIX_D73C + 4),
            WM_U32(WM_FIX_D308), WM_U32(WM_FIX_BD00));
    fprintf(stderr, "[worldmap-second-wave] fixup_exit\n");
    return 0;
}

/* One-shot W4C ladder after W3B. */
static int world_map_second_wave_once(void)
{
    if (wm_80071EF0_second_wave() != 0) {
        fprintf(stderr, "[worldmap-second-wave] submit failed\n");
        return -1;
    }
    if (wm_second_wave_poll() != 0)
        return -1;
    if (wm_80073530_fixup() != 0)
        return -1;
    fprintf(stderr,
            "[worldmap-second-wave] cut-before-broad-update retail_pc=0x%08x\n",
            WM_CUT_BEFORE_BROAD);
    return 0;
}

/*
 * One-shot outer dispatch glue: entrance*12 → table slot0 → mode init.
 * Does not enter 0x80071034.
 */
static int world_map_dispatch_mode_init_once(void)
{
    u32 entrance;
    u32 slot0;
    u32* pTable;

    entrance = WM_U32(WM_ENTRANCE_STATE_ABS);
    pTable = (u32*)PSX_ADDR(WM_DISPATCH_TABLE);
    slot0 = pTable[entrance * 3 + 0];

    fprintf(stderr,
            "[worldmap-mode-init] dispatch entrance=%u slot0_retail=0x%08x "
            "phase_D7CC=%u\n",
            entrance, slot0, WM_U32(WM_PHASE_D7CC));

    if (entrance > 7) {
        fprintf(stderr,
                "[worldmap-mode-init] ERROR: unsupported entrance row %u "
                "(W3B is Lahan 0..7 only)\n",
                entrance);
        return -1;
    }
    if (slot0 == 0) {
        fprintf(stderr, "[worldmap-mode-init] ERROR: slot0 is NULL\n");
        return -1;
    }
    if (slot0 != WM_MODE_INIT_RETAIL) {
        fprintf(stderr,
                "[worldmap-mode-init] ERROR: slot0 0x%08x != expected 0x%08x\n",
                slot0, WM_MODE_INIT_RETAIL);
        return -1;
    }

    /* Never call loaded MIPS; resolve known retail address to native body. */
    return wm_80071CDC_mode_init();
}

void PcPort_WorldMapInitMain(void)
{
    int decoded_size;
    u32 final_write = 0;
    u8 bss_before[16];
    u8 bss_after[16];

    s_wm712d0_hits = 0;
    s_wm_drawotag_hits = 0;
    s_wm9766c_hits = 0;
    s_wm72238_hits = 0;
    s_wm7299c_hits = 0;

    fprintf(stderr, "[worldmap-init] entry\n");
    log_enabled_slices();

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

    if (world_mode_init_enabled()) {
        fprintf(stderr,
                "[worldmap-init] mode-init: one-shot slot0 dispatch\n");
        if (world_map_dispatch_mode_init_once() != 0) {
            fprintf(stderr,
                    "[worldmap-mode-init] failed; still entering placeholder\n");
        } else if (world_second_wave_enabled()) {
            fprintf(stderr,
                    "[worldmap-init] XENO_WORLD_SECOND_WAVE=1: second-wave "
                    "0x80071EF0 → poll → 0x80073530\n");
            if (world_map_second_wave_once() != 0) {
                fprintf(stderr,
                        "[worldmap-second-wave] failed; still entering "
                        "placeholder\n");
            }
        }
        fprintf(stderr,
                "[worldmap-init] cut-before-main-loop retail_pc=0x%08x\n",
                world_second_wave_enabled() ? WM_CUT_BEFORE_BROAD : WM_MAIN_LOOP);
    } else {
        fprintf(stderr,
                "[worldmap-init] cut-before-loop retail_pc=0x%08x "
                "(mode-init gate off; next retail would be 0x%08x)\n",
                WM_POST_INIT_RESUME, WM_MAIN_LOOP);
    }

    if (s_wm712d0_hits != 0 || s_wm_drawotag_hits != 0 || s_wm9766c_hits != 0 ||
        s_wm72238_hits != 0 || s_wm7299c_hits != 0) {
        fprintf(stderr,
                "[worldmap-init] ERROR: forbidden path hit "
                "wm712d0=%d drawotag=%d f9766c=%d f72238=%d f7299c=%d\n",
                s_wm712d0_hits, s_wm_drawotag_hits, s_wm9766c_hits,
                s_wm72238_hits, s_wm7299c_hits);
    }

    /* Known-safe hollow UI — W2/W3B/W4C intentionally still show NOT YET PORTED. */
    fprintf(stderr, "[worldmap-placeholder] enter\n");
    PcPort_WorldMapPlaceholderMain();
}
