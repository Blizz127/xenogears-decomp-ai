/*
 * W2 / W3B / W4C / W5B / W6B / W7B / W8B / W10A — Native world-map init ladder.
 *
 * Retail WorldMapMain @ 0x80070CFC (overlay world_map.bin loaded at 0x8006FAF0).
 *
 * W2: pre-loop init through jal wm_80071B9C @ 0x80070FF4; cut before 0x80071000.
 * W3B: one-shot mode initializer 0x80071CDC (first-wave archive queue).
 * W4C: second-wave 0x80071EF0 → ArchiveDataSync poll → 0x80073530; cut before
 *      retail PC 0x800722BC.
 * W5B: object-pool 0x8009766C / 0x800976C8; cut before 0x800722C4.
 * W6B: identity copy 8×u32 0x8009A180 → 0x8009BE4C; cut before 0x80072314.
 * W7B: ten mode-enter u32 stores 0x80072314–0x80072374; cut before 0x80072378.
 * W8B: four OuterProduct0 via 0x80098044; cut before 0x80072380.
 * W10A: wm_8008440C (+ wm_800931D8) GPU/CLUT from W4C BD20; cut before
 *       0x80072444 (jal 0x800979C8).
 * W10B: wm_800979C8 GPU/CLUT/TPage from W4C C59C; cut before 0x8007244C.
 * W11B: wm_80084580 object/matrix table from W4C fixups; cut before
 *       0x80072454 (jal 0x80072090).
 * W12B: wm_80072090 third-wave archive submit (5×Decode/Alloc → D3F8 +
 *       func_80029AFC); cut before 0x8007245C (jal 0x800736DC). Submit only —
 *       no poll/fixup, no residual fan-out, no full 0x80072238 / frames.
 * W13B: wm_800736DC unrolled BSS constant paint (0x110); cut before
 *       0x80072464 (jal 0x80073E30). No third-wave consumption, no poll,
 *       no GPU/sprite setup.
 * W14B: wm_80073E30 primitive-template packer (DR_TPAGE + POLY_FT4×2 +
 *       POLY_G3×8 + TILE×64); cut before 0x8007246C (jal 0x80085F58).
 *       No VRAM upload, DrawOTag, 85F58, GfxAllocate, or third-wave poll.
 * W15B: wm_80085F58 relocate 256×8 records via C7EC + 16× GetClut → D478;
 *       cut before 0x80072478 (GfxAllocateWorkBuffers). Non-idempotent
 *       relocation: strict process-local one-shot guard.
 *
 * Gates (deepest implies lower):
 *   XENO_WORLD_INIT=1
 *   XENO_WORLD_MODE_INIT=1
 *   XENO_WORLD_SECOND_WAVE=1
 *   XENO_WORLD_OBJECT_POOL=1
 *   XENO_WORLD_STATE_TEMPLATE=1
 *   XENO_WORLD_MODE_ENTER_STATE=1
 *   XENO_WORLD_CROSS_PRODUCTS=1
 *   XENO_WORLD_GPU_ASSET_A=1
 *   XENO_WORLD_GPU_ASSET_B=1
 *   XENO_WORLD_OBJECT_MATRIX=1
 *   XENO_WORLD_THIRD_WAVE=1
 *   XENO_WORLD_BSS_CONSTANTS=1
 *   XENO_WORLD_PRIMITIVE_TEMPLATES=1
 *   XENO_WORLD_RECORD_CLUT_INIT=1
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
/* Hard cuts */
#define WM_CUT_BEFORE_BROAD      0x800722BCu /* after W4C: jal 0x8009766C */
#define WM_CUT_BEFORE_A180_COPY  0x800722C4u /* after W5B: A180→BE4C */
#define WM_CUT_BEFORE_CONST_BLK  0x80072314u /* after W6B: mode-enter consts */
#define WM_CUT_BEFORE_98044      0x80072378u /* after W7B: jal OuterProduct setup */
#define WM_CUT_AFTER_98044       0x80072380u /* after W8B return */
#define WM_CUT_BEFORE_979C8      0x80072444u /* after W10A: jal 0x800979C8 */
#define WM_CUT_BEFORE_84580      0x8007244Cu /* after W10B: jal 0x80084580 */
#define WM_CUT_BEFORE_72090      0x80072454u /* after W11B: jal 0x80072090 */
#define WM_CUT_BEFORE_736DC      0x8007245Cu /* after W12B: jal 0x800736DC */
#define WM_CUT_BEFORE_73E30      0x80072464u /* after W13B: jal 0x80073E30 */
#define WM_CUT_BEFORE_85F58      0x8007246Cu /* after W14B: jal 0x80085F58 */
#define WM_CUT_BEFORE_GFX_WORK   0x80072478u /* after W15B: jal GfxAllocate */

/* W15B record table + CLUT destinations (retail 0x80085F58). */
#define WM_REC_COUNT             256u
#define WM_REC_STRIDE            8u
#define WM_REC_BYTES             (WM_REC_COUNT * WM_REC_STRIDE) /* 2048 */
#define WM_CLUT_D478             0x8009D478u
#define WM_CLUT_COUNT            16u
#define WM_CLUT_BYTES            (WM_CLUT_COUNT * 2u) /* 32 */

/* W14B primitive-template BSS destinations (retail 0x80073E30). */
#define WM_PRIM_DR_TPAGE         0x8009C5A0u
#define WM_PRIM_FT4_0            0x8009C5C0u
#define WM_PRIM_FT4_1            0x8009C5E8u
#define WM_PRIM_G3_BASE          0x8009C664u
#define WM_PRIM_TILE_BASE        0x8009C898u
#define WM_PRIM_FT4_BYTES        40u
#define WM_PRIM_G3_STRIDE        28u
#define WM_PRIM_G3_COUNT         8u
#define WM_PRIM_TILE_STRIDE      16u
#define WM_PRIM_TILE_COUNT       64u
#define WM_PRIM_G3_BYTES         (WM_PRIM_G3_COUNT * WM_PRIM_G3_STRIDE)
#define WM_PRIM_TILE_BYTES       (WM_PRIM_TILE_COUNT * WM_PRIM_TILE_STRIDE)
#define WM_TW_ID_CC98            0x8009CC98u
#define WM_TW_ID_D3D0            0x8009D3D0u
#define WM_TW_ID_D3C8            0x8009D3C8u
#define WM_TW_ID_D800            0x8009D800u
#define WM_TW_ID_BCC8            0x8009BCC8u
#define WM_TW_MIRROR_C88C        0x8009C88Cu
#define WM_TW_MIRROR_C884        0x8009C884u
#define WM_TW_MIRROR_C888        0x8009C888u
#define WM_TW_MIRROR_C614        0x8009C614u
#define WM_REQ_D418              0x8009D418u
#define WM_REQ_D41C              0x8009D41Cu
#define WM_REQ_D420              0x8009D420u
#define WM_REQ_D424              0x8009D424u
#define WM_BROAD_9766C           0x8009766Cu
#define WM_OBJ_C620              0x8009C620u
#define WM_OBJ_BD28              0x8009BD28u
#define WM_OBJ_D7E0              0x8009D7E0u
#define WM_OBJ_C16C              0x8009C16Cu
#define WM_OBJ_C840              0x8009C840u
#define WM_OBJ_MAT_A140          0x8009A140u
#define WM_OBJ_MAT_A160          0x8009A160u
#define WM_OBJ_RECORD_STRIDE     84u
#define WM_GPU_8440C             0x8008440Cu
#define WM_GPU_979C8             0x800979C8u
#define WM_GPU_931D8             0x800931D8u
/* W10A/B: CLUT/TPage tables + scale constants (BD20/C59C already defined). */
#define WM_CLUT_BCE0             0x8009BCE0u
#define WM_CLUT_CCB4             0x8009CCB4u
#define WM_TPAGE_CD54            0x8009CD54u
#define WM_TPAGE_CD5C            0x8009CD5Cu
#define WM_SCALE_IMG_704DC       0x800704DCu /* 4-byte RGB scale (W10A) */
#define WM_SCALE_IMG_BB48        0x8009BB48u /* 4-byte RGB scale (W10B) */
/* W8B OuterProduct0 inputs (overlay image) / outputs (world BSS). */
#define WM_XP_BB4C               0x8009BB4Cu
#define WM_XP_BB5C               0x8009BB5Cu
#define WM_XP_BB6C               0x8009BB6Cu
#define WM_XP_BB7C               0x8009BB7Cu
#define WM_XP_BB8C               0x8009BB8Cu
#define WM_XP_BB9C               0x8009BB9Cu
#define WM_XP_C828               0x8009C828u
#define WM_XP_C844               0x8009C844u
#define WM_XP_C874               0x8009C874u
#define WM_XP_C7F0               0x8009C7F0u
#define WM_POOL_BE24             0x8009BE24u
#define WM_POOL_ALLOC_SIZE       8192u
#define WM_POOL_SLOT_COUNT       64
#define WM_POOL_SLOT_STRIDE      0x80u
#define WM_POOL_OFF_18           0x18u
#define WM_POOL_OFF_1C           0x1Cu
#define WM_POOL_OFF_4C           0x4Cu
/* W6B template: overlay image src → world BSS dest (8 words, identity). */
#define WM_TMPL_SRC_A180         0x8009A180u
#define WM_TMPL_DST_BE4C         0x8009BE4Cu
#define WM_TMPL_WORD_COUNT       8
#define WM_TMPL_BYTE_COUNT       (WM_TMPL_WORD_COUNT * 4u)
/* W7B mode-enter destinations (retail 0x80072314–0x80072374). */
#define WM_MES_CCA4              0x8009CCA4u
#define WM_MES_D3CC              0x8009D3CCu
#define WM_MES_D804              0x8009D804u
#define WM_MES_CEC0              0x8009CEC0u
#define WM_MES_C7E8              0x8009C7E8u
#define WM_MES_BD34              0x8009BD34u
#define WM_MES_D144              0x8009D144u
#define WM_MES_C178              0x8009C178u
#define WM_MES_CD40              0x8009CD40u
#define WM_MES_FN_86700          0x80086700u
#define WM_MES_STORE_COUNT       10

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
extern void func_8002DD20(u32* pList);
extern int StoreImage(RECT* rect, u_long* p);
extern int LoadImage(RECT* rect, u_long* p);
extern u_short GetClut(int x, int y);
extern u_short GetTPage(int tp, int abr, int x, int y);
extern void SetSemiTrans(void* p, int abe);
extern void SetDrawTPage(DR_TPAGE* p, int dfe, int dtd, int tpage);
extern int func_8002C3E8(u8* pModel);
extern void func_8002CB54(u8* modelData, u32* out1, u32* out2);
extern void func_8002C8CC(u8* a0, void* a1, s32 a2);
extern VECTOR* ApplyMatrix(MATRIX* m, SVECTOR* v0, VECTOR* v1);
extern SVECTOR* ApplyMatrixSV(MATRIX* m, SVECTOR* v0, SVECTOR* v1);
extern MATRIX* RotMatrix(SVECTOR* r, MATRIX* m);
extern s32 D_80050100;
extern s32 D_8004F304; /* main BSS counter; retail 0x8004F304 — host authority */
extern void* D_8006259C; /* main SEDS-style host pointer; retail 0x8006259C */
extern void* g_pGameState;
/* Main-executable global written by retail 0x80072364 (field init also sets 1). */
extern s32 D_80059198;
extern void OuterProduct0(VECTOR* v0, VECTOR* v1, VECTOR* v2);

#define GS_U8(off)  (*(u8*)((u8*)g_pGameState + (off)))
#define GS_U16(off) (*(u16*)((u8*)g_pGameState + (off)))
#define GS_S16(off) (*(s16*)((u8*)g_pGameState + (off)))

/* Retail registers a vsync IRQ callback (libetc). Host has no PSX IRQ table;
 * accepting the function pointer is enough for init ordering. */
void func_8004B7D0(void (*fn)(void))
{
    (void)fn;
}

/* Local memcpy / memeq — avoid string.h vs game strlen conflict. */
static void wm_memcpy(void* dst, const void* src, unsigned n)
{
    u8* d = (u8*)dst;
    const u8* s = (const u8*)src;
    while (n--)
        *d++ = *s++;
}

static int wm_memeq(const void* a, const void* b, unsigned n)
{
    const u8* x = (const u8*)a;
    const u8* y = (const u8*)b;
    while (n--) {
        if (*x++ != *y++)
            return 0;
    }
    return 1;
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

static int world_record_clut_enabled(void)
{
    return env_flag_is_one("XENO_WORLD_RECORD_CLUT_INIT");
}

static int world_primitive_templates_enabled(void)
{
    /* Record/CLUT init implies primitive-templates. */
    return env_flag_is_one("XENO_WORLD_PRIMITIVE_TEMPLATES") ||
           world_record_clut_enabled();
}

static int world_bss_constants_enabled(void)
{
    /* Primitive-templates imply BSS-constants. */
    return env_flag_is_one("XENO_WORLD_BSS_CONSTANTS") ||
           world_primitive_templates_enabled();
}

static int world_third_wave_enabled(void)
{
    /* BSS-constants imply third-wave. */
    return env_flag_is_one("XENO_WORLD_THIRD_WAVE") ||
           world_bss_constants_enabled();
}

static int world_object_matrix_enabled(void)
{
    /* Third-wave submit implies object-matrix. */
    return env_flag_is_one("XENO_WORLD_OBJECT_MATRIX") ||
           world_third_wave_enabled();
}

static int world_gpu_asset_b_enabled(void)
{
    /* Object-matrix implies GPU-asset-B. */
    return env_flag_is_one("XENO_WORLD_GPU_ASSET_B") ||
           world_object_matrix_enabled();
}

static int world_gpu_asset_a_enabled(void)
{
    /* GPU-asset-B implies GPU-asset-A. */
    return env_flag_is_one("XENO_WORLD_GPU_ASSET_A") ||
           world_gpu_asset_b_enabled();
}

static int world_cross_products_enabled(void)
{
    /* GPU-asset-A implies cross-products. */
    return env_flag_is_one("XENO_WORLD_CROSS_PRODUCTS") ||
           world_gpu_asset_a_enabled();
}

static int world_mode_enter_state_enabled(void)
{
    /* Cross-products implies mode-enter-state. */
    return env_flag_is_one("XENO_WORLD_MODE_ENTER_STATE") ||
           world_cross_products_enabled();
}

static int world_state_template_enabled(void)
{
    /* Mode-enter-state / cross-products imply state-template. */
    return env_flag_is_one("XENO_WORLD_STATE_TEMPLATE") ||
           world_mode_enter_state_enabled();
}

static int world_object_pool_enabled(void)
{
    /* State-template / mode-enter imply object-pool. */
    return env_flag_is_one("XENO_WORLD_OBJECT_POOL") || world_state_template_enabled();
}

static int world_second_wave_enabled(void)
{
    /* Object-pool / template imply second-wave. */
    return env_flag_is_one("XENO_WORLD_SECOND_WAVE") || world_object_pool_enabled();
}

static int world_mode_init_enabled(void)
{
    /* Second-wave ladder implies mode-init. */
    return env_flag_is_one("XENO_WORLD_MODE_INIT") || world_second_wave_enabled();
}

static int world_init_enabled(void)
{
    /* Mode-init ladder implies W2 overlay/init path. */
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
    int w5 = world_object_pool_enabled();
    int w6 = world_state_template_enabled();
    int w7 = world_mode_enter_state_enabled();
    int w8 = world_cross_products_enabled();
    int w10a = world_gpu_asset_a_enabled();
    int w10b = world_gpu_asset_b_enabled();
    int w11 = world_object_matrix_enabled();
    int w12 = world_third_wave_enabled();
    int w13 = world_bss_constants_enabled();
    int w14 = world_primitive_templates_enabled();
    int w15 = world_record_clut_enabled();
    fprintf(stderr, "[worldmap] enabled slices:");
    if (!w2 && !w3 && !w4 && !w5 && !w6 && !w7 && !w8 && !w10a && !w10b &&
        !w11 && !w12 && !w13 && !w14 && !w15) {
        fprintf(stderr, " (none — placeholder only)\n");
        return;
    }
    if (w2)
        fprintf(stderr, " W2");
    if (w3)
        fprintf(stderr, ",W3B");
    if (w4)
        fprintf(stderr, ",W4C");
    if (w5)
        fprintf(stderr, ",W5B");
    if (w6)
        fprintf(stderr, ",W6B");
    if (w7)
        fprintf(stderr, ",W7B");
    if (w8)
        fprintf(stderr, ",W8B");
    if (w10a)
        fprintf(stderr, ",W10A");
    if (w10b)
        fprintf(stderr, ",W10B");
    if (w11)
        fprintf(stderr, ",W11B");
    if (w12)
        fprintf(stderr, ",W12B");
    if (w13)
        fprintf(stderr, ",W13B");
    if (w14)
        fprintf(stderr, ",W14B");
    if (w15)
        fprintf(stderr, ",W15B");
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
 * W5B — native transcription of retail 0x800976C8.
 * Store widths are all sw (u32): +0x4C, +0x18, +0x1C per 0x80-byte slot.
 * Iteration: a0=0..63, base advances by 0x80 in the bne delay slot.
 */
static void wm_800976C8_clear_pool_slots(void)
{
    u32 pool_psx = WM_U32(WM_POOL_BE24);
    u8* base;
    int i;

    base = (u8*)psx_u32_to_host(pool_psx);
    if (base == NULL)
        return;

    for (i = 0; i < WM_POOL_SLOT_COUNT; i++) {
        u8* slot = base + (u32)i * WM_POOL_SLOT_STRIDE;
        /* Retail order: +0x4C, then +0x18, then +0x1C (all word stores). */
        *(u32*)(slot + WM_POOL_OFF_4C) = 0;
        *(u32*)(slot + WM_POOL_OFF_18) = 0;
        *(u32*)(slot + WM_POOL_OFF_1C) = 0;
    }
}

/*
 * W5B — native transcription of retail 0x8009766C.
 * HeapAlloc(8192, 0) → BE24 (KUSEG) → clear slots via 976C8.
 */
static int wm_8009766C_object_pool(void)
{
    void* pool_host;
    u32 pool_psx;
    u32 prior;
    int i;
    int ok = 0;
    u8* base;

    fprintf(stderr, "[worldmap-object-pool] entry\n");

    prior = WM_U32(WM_POOL_BE24);
    if (prior != 0) {
        /* Repeat invocation in one process would leak without free (976A0).
         * Fail loud rather than silent double-alloc. */
        fprintf(stderr,
                "[worldmap-object-pool] ERROR: BE24 already set "
                "psx=0x%08x (refuse re-init; retail free is 0x800976A0)\n",
                prior);
        return -1;
    }

    fprintf(stderr, "[worldmap-object-pool] alloc_size=%u\n", WM_POOL_ALLOC_SIZE);
    pool_host = HeapAlloc(WM_POOL_ALLOC_SIZE, 0);
    pool_psx = host_ptr_to_psx_u32(pool_host);
    /* Retail stores v0 then always calls clear (NULL would fault on PSX). */
    WM_U32(WM_POOL_BE24) = pool_psx;

    fprintf(stderr,
            "[worldmap-object-pool] pool_host=%p\n"
            "[worldmap-object-pool] pool_psx=0x%08x\n"
            "[worldmap-object-pool] slot_count=%d\n"
            "[worldmap-object-pool] slot_stride=0x%02x\n"
            "[worldmap-object-pool] zero_offsets=0x%02x,0x%02x,0x%02x\n",
            pool_host, pool_psx, WM_POOL_SLOT_COUNT, WM_POOL_SLOT_STRIDE,
            WM_POOL_OFF_18, WM_POOL_OFF_1C, WM_POOL_OFF_4C);

    if (pool_host == NULL || pool_psx == 0) {
        fprintf(stderr, "[worldmap-object-pool] ERROR: HeapAlloc failed\n");
        return -1;
    }

    /* Bounds: KUSEG pool must lie fully inside emulated 2 MiB RAM. */
    if (pool_psx < 0x80000000u ||
        pool_psx > 0x80200000u - WM_POOL_ALLOC_SIZE) {
        fprintf(stderr,
                "[worldmap-object-pool] ERROR: pool_psx 0x%08x out of RAM\n",
                pool_psx);
        return -1;
    }

    wm_800976C8_clear_pool_slots();

    /* Validate all 64 slots. */
    base = (u8*)psx_u32_to_host(WM_U32(WM_POOL_BE24));
    for (i = 0; i < WM_POOL_SLOT_COUNT; i++) {
        u8* slot = base + (u32)i * WM_POOL_SLOT_STRIDE;
        if (*(u32*)(slot + WM_POOL_OFF_18) == 0 &&
            *(u32*)(slot + WM_POOL_OFF_1C) == 0 &&
            *(u32*)(slot + WM_POOL_OFF_4C) == 0)
            ok++;
    }

    fprintf(stderr,
            "[worldmap-object-pool] slot_check=%d/%d\n"
            "[worldmap-object-pool] slot0 +0x18=0x%08x +0x1C=0x%08x +0x4C=0x%08x\n"
            "[worldmap-object-pool] slot1 +0x18=0x%08x +0x1C=0x%08x +0x4C=0x%08x\n"
            "[worldmap-object-pool] slot63 +0x18=0x%08x +0x1C=0x%08x +0x4C=0x%08x\n",
            ok, WM_POOL_SLOT_COUNT,
            *(u32*)(base + 0 * WM_POOL_SLOT_STRIDE + WM_POOL_OFF_18),
            *(u32*)(base + 0 * WM_POOL_SLOT_STRIDE + WM_POOL_OFF_1C),
            *(u32*)(base + 0 * WM_POOL_SLOT_STRIDE + WM_POOL_OFF_4C),
            *(u32*)(base + 1 * WM_POOL_SLOT_STRIDE + WM_POOL_OFF_18),
            *(u32*)(base + 1 * WM_POOL_SLOT_STRIDE + WM_POOL_OFF_1C),
            *(u32*)(base + 1 * WM_POOL_SLOT_STRIDE + WM_POOL_OFF_4C),
            *(u32*)(base + 63 * WM_POOL_SLOT_STRIDE + WM_POOL_OFF_18),
            *(u32*)(base + 63 * WM_POOL_SLOT_STRIDE + WM_POOL_OFF_1C),
            *(u32*)(base + 63 * WM_POOL_SLOT_STRIDE + WM_POOL_OFF_4C));

    if (ok != WM_POOL_SLOT_COUNT) {
        fprintf(stderr, "[worldmap-object-pool] ERROR: slot zero-check failed\n");
        return -1;
    }

    fprintf(stderr, "[worldmap-object-pool] exit\n");
    fprintf(stderr,
            "[worldmap-object-pool] cut-before-next-step retail_pc=0x%08x\n",
            WM_CUT_BEFORE_A180_COPY);
    return 0;
}

/*
 * W6B — retail 0x800722C4–0x80072310: unrolled identity copy of 8 u32 words
 * from overlay image 0x8009A180 → world BSS 0x8009BE4C. No transforms.
 * Does not hard-code source values; does not touch the object pool at *BE24.
 */
static int wm_state_template_copy(void)
{
    u32* src;
    u32* dst;
    u32 i;
    int match;

    fprintf(stderr, "[worldmap-state-template] entry\n");
    fprintf(stderr,
            "[worldmap-state-template] src_psx=0x%08x dst_psx=0x%08x "
            "words=%u bytes=%u\n",
            WM_TMPL_SRC_A180, WM_TMPL_DST_BE4C, WM_TMPL_WORD_COUNT,
            WM_TMPL_BYTE_COUNT);

    /* Source must sit in the planted overlay image (below BSS start). */
    if (WM_TMPL_SRC_A180 < WM_OVERLAY_BASE ||
        WM_TMPL_SRC_A180 + WM_TMPL_BYTE_COUNT > WM_MEM_START) {
        fprintf(stderr,
                "[worldmap-state-template] ERROR: source not in overlay image\n");
        return -1;
    }
    if (WM_TMPL_DST_BE4C < WM_MEM_START) {
        fprintf(stderr,
                "[worldmap-state-template] ERROR: dest not in world BSS\n");
        return -1;
    }
    /* Dest must not overlap the pool pointer slot or be confused with heap. */
    if (WM_TMPL_DST_BE4C == WM_POOL_BE24) {
        fprintf(stderr,
                "[worldmap-state-template] ERROR: dest is pool pointer slot\n");
        return -1;
    }

    src = (u32*)PSX_ADDR(WM_TMPL_SRC_A180);
    dst = (u32*)PSX_ADDR(WM_TMPL_DST_BE4C);

    fprintf(stderr, "[worldmap-state-template] src_host=%p dst_host=%p\n",
            (void*)src, (void*)dst);
    fprintf(stderr, "[worldmap-state-template] src_words:");
    for (i = 0; i < WM_TMPL_WORD_COUNT; i++)
        fprintf(stderr, " 0x%08x", src[i]);
    fprintf(stderr, "\n");

    /* Retail unrolled lw/sw pairs — identity word copy. */
    for (i = 0; i < WM_TMPL_WORD_COUNT; i++)
        dst[i] = src[i];

    match = 0;
    for (i = 0; i < WM_TMPL_WORD_COUNT; i++) {
        if (dst[i] == src[i])
            match++;
    }

    fprintf(stderr, "[worldmap-state-template] dst_words:");
    for (i = 0; i < WM_TMPL_WORD_COUNT; i++)
        fprintf(stderr, " 0x%08x", dst[i]);
    fprintf(stderr, "\n");
    fprintf(stderr, "[worldmap-state-template] word_match=%d/%u\n",
            match, WM_TMPL_WORD_COUNT);
    fprintf(stderr,
            "[worldmap-state-template] pool_BE24_unchanged=0x%08x "
            "(not written by this step)\n",
            WM_U32(WM_POOL_BE24));

    if (match != (int)WM_TMPL_WORD_COUNT) {
        fprintf(stderr, "[worldmap-state-template] ERROR: copy verify failed\n");
        return -1;
    }

    fprintf(stderr, "[worldmap-state-template] exit\n");
    fprintf(stderr,
            "[worldmap-state-template] cut-before-const-block retail_pc=0x%08x\n",
            WM_CUT_BEFORE_CONST_BLK);
    return 0;
}

/*
 * W7B — retail 0x80072314–0x80072374: ten immediate u32 mode-enter stores.
 * Order matches assembly. D_80059198 is the host main-exe global (not g_PsxRam
 * alone). Independent of 0x80098044 / OuterProduct0.
 */
static int PcPort_WorldMapInitializeModeEnterState(void)
{
    u32 before[WM_MES_STORE_COUNT];
    u32 after[WM_MES_STORE_COUNT];
    u32 expected[WM_MES_STORE_COUNT] = {
        2u, 4u, 0u, 0u, 0u, 0u, 0u, 1u, 1u, WM_MES_FN_86700
    };
    u32 dsts[WM_MES_STORE_COUNT] = {
        WM_MES_CCA4, WM_MES_D3CC, WM_MES_D804, WM_MES_CEC0, WM_MES_C7E8,
        WM_MES_BD34, WM_MES_D144, 0x80059198u, WM_MES_C178, WM_MES_CD40
    };
    int i;
    int match = 0;
    u32 be24_before;
    u32 tmpl_before[WM_TMPL_WORD_COUNT];
    u32 be24_after;
    int tmpl_ok = 1;
    int pool_ok = 1;

    fprintf(stderr, "[worldmap-mode-enter-state] entry\n");
    fprintf(stderr, "[worldmap-mode-enter-state] store_count=%d\n",
            WM_MES_STORE_COUNT);

    be24_before = WM_U32(WM_POOL_BE24);
    for (i = 0; i < (int)WM_TMPL_WORD_COUNT; i++)
        tmpl_before[i] = ((u32*)PSX_ADDR(WM_TMPL_DST_BE4C))[i];

    /* Snapshot before (retail order destinations). */
    before[0] = WM_U32(WM_MES_CCA4);
    before[1] = WM_U32(WM_MES_D3CC);
    before[2] = WM_U32(WM_MES_D804);
    before[3] = WM_U32(WM_MES_CEC0);
    before[4] = WM_U32(WM_MES_C7E8);
    before[5] = WM_U32(WM_MES_BD34);
    before[6] = WM_U32(WM_MES_D144);
    before[7] = (u32)D_80059198;
    before[8] = WM_U32(WM_MES_C178);
    before[9] = WM_U32(WM_MES_CD40);

    /* Retail store order (immediates / constructed fn VA). Idempotent. */
    WM_U32(WM_MES_CCA4) = 2u;                 /* 0x80072320 */
    WM_U32(WM_MES_D3CC) = 4u;                 /* 0x8007232C */
    WM_U32(WM_MES_D804) = 0u;                 /* 0x8007233C */
    WM_U32(WM_MES_CEC0) = 0u;                 /* 0x80072344 */
    WM_U32(WM_MES_C7E8) = 0u;                 /* 0x8007234C */
    WM_U32(WM_MES_BD34) = 0u;                 /* 0x80072354 */
    WM_U32(WM_MES_D144) = 0u;                 /* 0x8007235C */
    D_80059198 = 1;                           /* 0x80072364 main global */
    WM_U32(WM_MES_C178) = 1u;                 /* 0x8007236C */
    WM_U32(WM_MES_CD40) = WM_MES_FN_86700;    /* 0x80072374 */

    after[0] = WM_U32(WM_MES_CCA4);
    after[1] = WM_U32(WM_MES_D3CC);
    after[2] = WM_U32(WM_MES_D804);
    after[3] = WM_U32(WM_MES_CEC0);
    after[4] = WM_U32(WM_MES_C7E8);
    after[5] = WM_U32(WM_MES_BD34);
    after[6] = WM_U32(WM_MES_D144);
    after[7] = (u32)D_80059198;
    after[8] = WM_U32(WM_MES_C178);
    after[9] = WM_U32(WM_MES_CD40);

    for (i = 0; i < WM_MES_STORE_COUNT; i++) {
        fprintf(stderr,
                "[worldmap-mode-enter-state] store[%d] dst=0x%08x "
                "before=0x%08x value=0x%08x expected=0x%08x\n",
                i, dsts[i], before[i], after[i], expected[i]);
        if (after[i] == expected[i])
            match++;
    }

    be24_after = WM_U32(WM_POOL_BE24);
    if (be24_after != be24_before)
        pool_ok = 0;
    for (i = 0; i < (int)WM_TMPL_WORD_COUNT; i++) {
        if (((u32*)PSX_ADDR(WM_TMPL_DST_BE4C))[i] != tmpl_before[i])
            tmpl_ok = 0;
    }

    fprintf(stderr,
            "[worldmap-mode-enter-state] match=%d/%d pool_preserved=%d "
            "template_preserved=%d\n",
            match, WM_MES_STORE_COUNT, pool_ok, tmpl_ok);

    if (match != WM_MES_STORE_COUNT || !pool_ok || !tmpl_ok) {
        fprintf(stderr, "[worldmap-mode-enter-state] ERROR: validation failed\n");
        return -1;
    }

    fprintf(stderr, "[worldmap-mode-enter-state] exit\n");
    fprintf(stderr,
            "[worldmap-mode-enter-state] cut-before-0x80098044 "
            "retail_pc=0x%08x\n",
            WM_CUT_BEFORE_98044);
    return 0;
}

/* Compile-time layout check: retail VECTOR is 4×s32 (vx,vy,vz,pad). */
typedef char wm_assert_vector_16[(sizeof(VECTOR) == 16) ? 1 : -1];

/*
 * W8B — native transcription of retail 0x80098044–0x800980D4.
 * Exactly four OuterProduct0 calls; inputs overlay image, outputs world BSS.
 *
 * Instruction audit (retail world_map.bin @ 0x80098044–0x800980D4):
 *   98044  addiu sp,sp,-24          prologue
 *   98048  lui a0,0x800a / addiu a0,-17556  → a0 = 0x8009BB6C
 *   98050  sw s0,16(sp)
 *   98054  lui s0,0x800a / addiu s0,-17588  → s0 = 0x8009BB4C
 *   9805c  lui a2,0x800a / addiu a2,-14296  → a2 = 0x8009C828
 *   98064  sw ra,20(sp)
 *   98068  jal OuterProduct0 @ 0x8004A4D8
 *   9806c  addu a1,s0,zero                 delay: a1 = 0x8009BB4C
 *   98070  lui a1,0x800a / addiu a1,-17540  → a1 = 0x8009BB7C
 *   98078  lui a2,0x800a / addiu a2,-14268  → a2 = 0x8009C844
 *   98080  jal OuterProduct0
 *   98084  addu a0,s0,zero                 delay: a0 = 0x8009BB4C
 *   98088  lui a0,0x800a / addiu a0,-17524  → a0 = 0x8009BB8C
 *   98090  lui s0,0x800a / addiu s0,-17572  → s0 = 0x8009BB5C
 *   98098  lui a2,0x800a / addiu a2,-14220  → a2 = 0x8009C874
 *   980a0  jal OuterProduct0
 *   980a4  addu a1,s0,zero                 delay: a1 = 0x8009BB5C
 *   980a8  lui a1,0x800a / addiu a1,-17508  → a1 = 0x8009BB9C
 *   980b0  lui a2,0x800a / addiu a2,-14352  → a2 = 0x8009C7F0
 *   980b8  jal OuterProduct0
 *   980bc  addu a0,s0,zero                 delay: a0 = 0x8009BB5C
 *   980c0  lw ra,20(sp) / lw s0,16(sp)     epilogue
 *   980c8  addiu sp,sp,24
 *   980cc  jr ra / nop
 * Exactly four calls; no other stores or side effects.
 */
static int wm_80098044_cross_product_init(void)
{
    VECTOR* in_bb4c = (VECTOR*)PSX_ADDR(WM_XP_BB4C);
    VECTOR* in_bb5c = (VECTOR*)PSX_ADDR(WM_XP_BB5C);
    VECTOR* in_bb6c = (VECTOR*)PSX_ADDR(WM_XP_BB6C);
    VECTOR* in_bb7c = (VECTOR*)PSX_ADDR(WM_XP_BB7C);
    VECTOR* in_bb8c = (VECTOR*)PSX_ADDR(WM_XP_BB8C);
    VECTOR* in_bb9c = (VECTOR*)PSX_ADDR(WM_XP_BB9C);
    VECTOR* out0 = (VECTOR*)PSX_ADDR(WM_XP_C828);
    VECTOR* out1 = (VECTOR*)PSX_ADDR(WM_XP_C844);
    VECTOR* out2 = (VECTOR*)PSX_ADDR(WM_XP_C874);
    VECTOR* out3 = (VECTOR*)PSX_ADDR(WM_XP_C7F0);
    /* Oracle: integer cross product matching OuterProduct0 (writes vx,vy,vz only). */
    long exp0[3], exp1[3], exp2[3], exp3[3];
    u32 be24_before = WM_U32(WM_POOL_BE24);
    u32 tmpl_words[WM_TMPL_WORD_COUNT];
    u32 mes_before[WM_MES_STORE_COUNT];
    u8 in_snap[6][16];
    /* 4-byte canaries immediately before/after each 16-byte output VECTOR. */
    u32 canary_pre[4];
    u32 canary_post[4];
    long pad_before[4];
    const u32 out_addrs[4] = {
        WM_XP_C828, WM_XP_C844, WM_XP_C874, WM_XP_C7F0
    };
    int i;
    int vec_ok = 0;
    int comp_ok = 0;
    int boundary_ok = 1;
    int inputs_ok = 1;
    int lower_ok = 1;

    fprintf(stderr, "[worldmap-cross-products] entry\n");
    fprintf(stderr,
            "[worldmap-cross-products] call0 srcA=0x%08x srcB=0x%08x dst=0x%08x\n"
            "[worldmap-cross-products] call1 srcA=0x%08x srcB=0x%08x dst=0x%08x\n"
            "[worldmap-cross-products] call2 srcA=0x%08x srcB=0x%08x dst=0x%08x\n"
            "[worldmap-cross-products] call3 srcA=0x%08x srcB=0x%08x dst=0x%08x\n",
            WM_XP_BB6C, WM_XP_BB4C, WM_XP_C828,
            WM_XP_BB4C, WM_XP_BB7C, WM_XP_C844,
            WM_XP_BB8C, WM_XP_BB5C, WM_XP_C874,
            WM_XP_BB5C, WM_XP_BB9C, WM_XP_C7F0);

    /* Inputs must sit in overlay image (before BSS start). */
    if (WM_XP_BB4C < WM_OVERLAY_BASE || WM_XP_BB9C + 16u > WM_MEM_START) {
        fprintf(stderr, "[worldmap-cross-products] ERROR: inputs not in image\n");
        return -1;
    }
    if (WM_XP_C7F0 < WM_MEM_START || WM_XP_C874 + 16u <= WM_MEM_START) {
        fprintf(stderr, "[worldmap-cross-products] ERROR: outputs not in BSS\n");
        return -1;
    }

    fprintf(stderr,
            "[worldmap-cross-products] in BB4C=(%d,%d,%d) BB5C=(%d,%d,%d) "
            "BB6C=(%d,%d,%d)\n",
            (int)in_bb4c->vx, (int)in_bb4c->vy, (int)in_bb4c->vz,
            (int)in_bb5c->vx, (int)in_bb5c->vy, (int)in_bb5c->vz,
            (int)in_bb6c->vx, (int)in_bb6c->vy, (int)in_bb6c->vz);
    fprintf(stderr,
            "[worldmap-cross-products] in BB7C=(%d,%d,%d) BB8C=(%d,%d,%d) "
            "BB9C=(%d,%d,%d)\n",
            (int)in_bb7c->vx, (int)in_bb7c->vy, (int)in_bb7c->vz,
            (int)in_bb8c->vx, (int)in_bb8c->vy, (int)in_bb8c->vz,
            (int)in_bb9c->vx, (int)in_bb9c->vy, (int)in_bb9c->vz);

    /* Snapshot inputs, lower-slice state, and output neighborhoods. */
    wm_memcpy(in_snap[0], in_bb4c, 16);
    wm_memcpy(in_snap[1], in_bb5c, 16);
    wm_memcpy(in_snap[2], in_bb6c, 16);
    wm_memcpy(in_snap[3], in_bb7c, 16);
    wm_memcpy(in_snap[4], in_bb8c, 16);
    wm_memcpy(in_snap[5], in_bb9c, 16);
    for (i = 0; i < WM_TMPL_WORD_COUNT; i++)
        tmpl_words[i] = ((u32*)PSX_ADDR(WM_TMPL_DST_BE4C))[i];
    /* W7B ten-store snapshot (same order as mode-enter-state). */
    mes_before[0] = WM_U32(WM_MES_CCA4);
    mes_before[1] = WM_U32(WM_MES_D3CC);
    mes_before[2] = WM_U32(WM_MES_D804);
    mes_before[3] = WM_U32(WM_MES_CEC0);
    mes_before[4] = WM_U32(WM_MES_C7E8);
    mes_before[5] = WM_U32(WM_MES_BD34);
    mes_before[6] = WM_U32(WM_MES_D144);
    mes_before[7] = (u32)D_80059198;
    mes_before[8] = WM_U32(WM_MES_C178);
    mes_before[9] = WM_U32(WM_MES_CD40);

    /* Capture pad + canaries around each 16-byte output record. */
    pad_before[0] = out0->pad;
    pad_before[1] = out1->pad;
    pad_before[2] = out2->pad;
    pad_before[3] = out3->pad;
    for (i = 0; i < 4; i++) {
        canary_pre[i] = WM_U32(out_addrs[i] - 4u);
        canary_post[i] = WM_U32(out_addrs[i] + 16u);
    }

    exp0[0] = in_bb6c->vy * in_bb4c->vz - in_bb6c->vz * in_bb4c->vy;
    exp0[1] = in_bb6c->vz * in_bb4c->vx - in_bb6c->vx * in_bb4c->vz;
    exp0[2] = in_bb6c->vx * in_bb4c->vy - in_bb6c->vy * in_bb4c->vx;
    exp1[0] = in_bb4c->vy * in_bb7c->vz - in_bb4c->vz * in_bb7c->vy;
    exp1[1] = in_bb4c->vz * in_bb7c->vx - in_bb4c->vx * in_bb7c->vz;
    exp1[2] = in_bb4c->vx * in_bb7c->vy - in_bb4c->vy * in_bb7c->vx;
    exp2[0] = in_bb8c->vy * in_bb5c->vz - in_bb8c->vz * in_bb5c->vy;
    exp2[1] = in_bb8c->vz * in_bb5c->vx - in_bb8c->vx * in_bb5c->vz;
    exp2[2] = in_bb8c->vx * in_bb5c->vy - in_bb8c->vy * in_bb5c->vx;
    exp3[0] = in_bb5c->vy * in_bb9c->vz - in_bb5c->vz * in_bb9c->vy;
    exp3[1] = in_bb5c->vz * in_bb9c->vx - in_bb5c->vx * in_bb9c->vz;
    exp3[2] = in_bb5c->vx * in_bb9c->vy - in_bb5c->vy * in_bb9c->vx;

    /* Retail order — four OuterProduct0 only. Do not reorder or precompute. */
    OuterProduct0(in_bb6c, in_bb4c, out0);
    OuterProduct0(in_bb4c, in_bb7c, out1);
    OuterProduct0(in_bb8c, in_bb5c, out2);
    OuterProduct0(in_bb5c, in_bb9c, out3);

    fprintf(stderr,
            "[worldmap-cross-products] out0=(%d,%d,%d) exp=(%d,%d,%d)\n"
            "[worldmap-cross-products] out1=(%d,%d,%d) exp=(%d,%d,%d)\n"
            "[worldmap-cross-products] out2=(%d,%d,%d) exp=(%d,%d,%d)\n"
            "[worldmap-cross-products] out3=(%d,%d,%d) exp=(%d,%d,%d)\n",
            (int)out0->vx, (int)out0->vy, (int)out0->vz,
            (int)exp0[0], (int)exp0[1], (int)exp0[2],
            (int)out1->vx, (int)out1->vy, (int)out1->vz,
            (int)exp1[0], (int)exp1[1], (int)exp1[2],
            (int)out2->vx, (int)out2->vy, (int)out2->vz,
            (int)exp2[0], (int)exp2[1], (int)exp2[2],
            (int)out3->vx, (int)out3->vy, (int)out3->vz,
            (int)exp3[0], (int)exp3[1], (int)exp3[2]);

    if (out0->vx == exp0[0] && out0->vy == exp0[1] && out0->vz == exp0[2]) {
        vec_ok++;
        comp_ok += 3;
    }
    if (out1->vx == exp1[0] && out1->vy == exp1[1] && out1->vz == exp1[2]) {
        vec_ok++;
        comp_ok += 3;
    }
    if (out2->vx == exp2[0] && out2->vy == exp2[1] && out2->vz == exp2[2]) {
        vec_ok++;
        comp_ok += 3;
    }
    if (out3->vx == exp3[0] && out3->vy == exp3[1] && out3->vz == exp3[2]) {
        vec_ok++;
        comp_ok += 3;
    }

    /* Inputs unchanged. */
    if (!wm_memeq(in_snap[0], in_bb4c, 16) ||
        !wm_memeq(in_snap[1], in_bb5c, 16) ||
        !wm_memeq(in_snap[2], in_bb6c, 16) ||
        !wm_memeq(in_snap[3], in_bb7c, 16) ||
        !wm_memeq(in_snap[4], in_bb8c, 16) ||
        !wm_memeq(in_snap[5], in_bb9c, 16)) {
        inputs_ok = 0;
    }

    /* OuterProduct0 writes only vx/vy/vz; pad + neighbor canaries must hold. */
    if (out0->pad != pad_before[0] || out1->pad != pad_before[1] ||
        out2->pad != pad_before[2] || out3->pad != pad_before[3]) {
        boundary_ok = 0;
    }
    for (i = 0; i < 4; i++) {
        if (WM_U32(out_addrs[i] - 4u) != canary_pre[i] ||
            WM_U32(out_addrs[i] + 16u) != canary_post[i]) {
            boundary_ok = 0;
        }
    }

    /* W5B / W6B / W7B preservation. */
    if (WM_U32(WM_POOL_BE24) != be24_before)
        lower_ok = 0;
    for (i = 0; i < WM_TMPL_WORD_COUNT; i++) {
        if (((u32*)PSX_ADDR(WM_TMPL_DST_BE4C))[i] != tmpl_words[i])
            lower_ok = 0;
    }
    if (WM_U32(WM_MES_CCA4) != mes_before[0] ||
        WM_U32(WM_MES_D3CC) != mes_before[1] ||
        WM_U32(WM_MES_D804) != mes_before[2] ||
        WM_U32(WM_MES_CEC0) != mes_before[3] ||
        WM_U32(WM_MES_C7E8) != mes_before[4] ||
        WM_U32(WM_MES_BD34) != mes_before[5] ||
        WM_U32(WM_MES_D144) != mes_before[6] ||
        (u32)D_80059198 != mes_before[7] ||
        WM_U32(WM_MES_C178) != mes_before[8] ||
        WM_U32(WM_MES_CD40) != mes_before[9]) {
        lower_ok = 0;
    }

    fprintf(stderr,
            "[worldmap-cross-products] output_match=%d/4 component_match=%d/12 "
            "inputs_ok=%d boundary_ok=%d lower_ok=%d "
            "pool_BE24=0x%08x template0=0x%08x mes_CCA4=0x%08x\n",
            vec_ok, comp_ok, inputs_ok, boundary_ok, lower_ok,
            WM_U32(WM_POOL_BE24),
            ((u32*)PSX_ADDR(WM_TMPL_DST_BE4C))[0], WM_U32(WM_MES_CCA4));

    if (vec_ok != 4 || !inputs_ok || !boundary_ok || !lower_ok) {
        fprintf(stderr, "[worldmap-cross-products] ERROR: validation failed\n");
        return -1;
    }

    fprintf(stderr, "[worldmap-cross-products] exit\n");
    fprintf(stderr,
            "[worldmap-cross-products] cut-before-next-step retail_pc=0x%08x\n",
            WM_CUT_AFTER_98044);
    return 0;
}

/*
 * W10A nested helper — retail 0x800931D8–0x80093350 (0x178 / 376 bytes to jr).
 * Expand one VRAM-fetched BGR555 row into `rows` blended rows.
 * a0=src u16*, a1=dst u16*, a2=row count, a3=3 scale bytes (R,G,B).
 */
static void wm_800931d8_expand(u16* src, u16* dst, int rows, const u8* scales)
{
    u8 scale_r = scales[0];
    u8 scale_g = scales[1];
    u8 scale_b = scales[2];
    int t7;
    u32 out_words;

    fprintf(stderr, "[worldmap-gpu-asset-a] expand_entry\n");
    if (rows <= 0) {
        fprintf(stderr,
                "[worldmap-gpu-asset-a] expand_exit rows=0 output_size=0\n");
        return;
    }

    for (t7 = 0; t7 < rows; t7++) {
        s32 t1 = ((s32)t7 << 12) / rows;
        s32 t3 = 4096 - t1;
        u16* row_src = src;
        int t4;

        for (t4 = 0; t4 < 256; t4++) {
            u16 pix = *row_src;
            u16 out;

            if (pix == 0) {
                out = 0;
            } else {
                s32 r = (s32)((pix & 0x1Fu) << 3);
                s32 g = (s32)((pix >> 2) & 0xF8u);
                s32 b = (s32)((pix >> 7) & 0xF8u);
                s32 or_ = (r * t3 + (s32)scale_r * t1) >> 15;
                s32 og = (g * t3 + (s32)scale_g * t1) >> 15;
                s32 ob = (b * t3 + (s32)scale_b * t1) >> 15;
                out = (u16)((pix & 0x8000u) | (u16)or_ | ((u16)og << 5) |
                            ((u16)ob << 10));
            }
            *dst++ = out;
            row_src++;
        }
    }

    out_words = (u32)rows * 256u;
    fprintf(stderr,
            "[worldmap-gpu-asset-a] expand_exit rows=%d width=256 "
            "output_size=%u halfwords scales=(0x%02x,0x%02x,0x%02x)\n",
            rows, out_words, scale_r, scale_g, scale_b);
}

/*
 * W10A — native transcription of retail 0x8008440C–0x8008457C (0x174 / 372 B).
 * Consumes W4C second-wave buffer at 0x8009BD20 (TIM multi-list after LZSS),
 * uploads via func_8002DD20, expands one CLUT strip, writes 16 GetClut ids
 * to 0x8009BCE0. One-shot: frees the compressed BD20 allocation.
 */
static int s_wm8440c_ran;

static int wm_8008440c_gpu_asset_a(void)
{
    u32 compressed_psx;
    void* compressed_host;
    void* decomp_host;
    void* img_host;
    void* exp_host;
    u8 scale_bytes[4];
    RECT rect;
    u16* clut_tbl;
    int i;
    int clut_ok = 0;
    u32 pool_be24;
    u32 mes_cca4;
    u32 xp0;

    fprintf(stderr, "[worldmap-gpu-asset-a] entry\n");
    fprintf(stderr, "[worldmap-gpu-asset-a] source_slot=0x%08x\n",
            WM_DST_BD20);

    if (s_wm8440c_ran) {
        fprintf(stderr,
                "[worldmap-gpu-asset-a] ERROR: already ran this process "
                "(BD20 is one-shot; reload lower ladder)\n");
        return -1;
    }

    compressed_psx = WM_U32(WM_DST_BD20);
    compressed_host = psx_u32_to_host(compressed_psx);
    if (compressed_psx == 0 || compressed_host == NULL) {
        fprintf(stderr,
                "[worldmap-gpu-asset-a] ERROR: BD20 empty/null "
                "psx=0x%08x\n",
                compressed_psx);
        return -1;
    }

    pool_be24 = WM_U32(WM_POOL_BE24);
    mes_cca4 = WM_U32(WM_MES_CCA4);
    xp0 = WM_U32(WM_XP_C828);

    /* Scale constants: unaligned 4-byte load from overlay image @ 0x800704DC. */
    wm_memcpy(scale_bytes, PSX_ADDR(WM_SCALE_IMG_704DC), 4);

    fprintf(stderr,
            "[worldmap-gpu-asset-a] compressed_psx=0x%08x host=%p "
            "scales=(0x%02x,0x%02x,0x%02x)\n",
            compressed_psx, compressed_host, scale_bytes[0], scale_bytes[1],
            scale_bytes[2]);

    /* 1–2: LZSSHeapDecompress(BD20, flags=1). */
    decomp_host = LZSSHeapDecompress(compressed_host, 1);
    if (decomp_host == NULL) {
        fprintf(stderr,
                "[worldmap-gpu-asset-a] ERROR: LZSSHeapDecompress failed\n");
        return -1;
    }
    fprintf(stderr, "[worldmap-gpu-asset-a] decompressed_host=%p\n",
            decomp_host);

    /* 3: TIM multi-list upload (CLUT + pixel LoadImage). */
    func_8002DD20((u32*)decomp_host);

    /* 4–5: DrawSync; free decompressed TIM list. */
    DrawSync(0);
    HeapFree(decomp_host);

    /* 6: free original compressed BD20 allocation (retail leaves slot stale). */
    compressed_host = psx_u32_to_host(WM_U32(WM_DST_BD20));
    if (compressed_host != NULL)
        HeapFree(compressed_host);

    /* 7–8: work buffers — 512 B store target, 8192 B expand output. */
    img_host = HeapAlloc(512, 1);
    exp_host = HeapAlloc(8192, 1);
    if (img_host == NULL || exp_host == NULL) {
        fprintf(stderr, "[worldmap-gpu-asset-a] ERROR: HeapAlloc failed\n");
        return -1;
    }
    fprintf(stderr,
            "[worldmap-gpu-asset-a] work_img=%p work_exp=%p\n",
            img_host, exp_host);

    /* 9: StoreImage RECT (0,496,256,1) — read one VRAM line. */
    rect.x = 0;
    rect.y = 496;
    rect.w = 256;
    rect.h = 1;
    fprintf(stderr,
            "[worldmap-gpu-asset-a] store_rect=(%d,%d,%d,%d)\n",
            rect.x, rect.y, rect.w, rect.h);
    StoreImage(&rect, (u_long*)img_host);
    DrawSync(0);

    /* 10: expand 1 fetched row → 16 blended rows (a2=16). */
    wm_800931d8_expand((u16*)img_host, (u16*)exp_host, 16, scale_bytes);

    /* 11: LoadImage RECT (0,496,256,15). */
    rect.x = 0;
    rect.y = 496;
    rect.w = 256;
    rect.h = 15;
    fprintf(stderr,
            "[worldmap-gpu-asset-a] load_rect=(%d,%d,%d,%d)\n",
            rect.x, rect.y, rect.w, rect.h);
    LoadImage(&rect, (u_long*)exp_host);
    DrawSync(0);

    /* 12: GetClut(0, 496+i) × 16 → 0x8009BCE0. */
    clut_tbl = (u16*)PSX_ADDR(WM_CLUT_BCE0);
    fprintf(stderr,
            "[worldmap-gpu-asset-a] clut_count=16 clut_table=0x%08x\n",
            WM_CLUT_BCE0);
    for (i = 0; i < 16; i++) {
        int gx = 0;
        int gy = 496 + i;
        /* Retail GetClut / getClut(x,y) = (y<<6)|((x>>4)&0x3f). */
        u16 formula = (u16)(((u32)gy << 6) | (((u32)gx >> 4) & 0x3fu));
        u16 got = GetClut(gx, gy);
        clut_tbl[i] = got;
        fprintf(stderr,
                "[worldmap-gpu-asset-a] clut[%d] xy=(%d,%d) value=0x%04x "
                "formula=0x%04x\n",
                i, gx, gy, got, formula);
        if (got == formula)
            clut_ok++;
    }
    fprintf(stderr, "[worldmap-gpu-asset-a] clut_match=%d/16\n", clut_ok);

    /* 13: free expand then store buffer (retail order). */
    HeapFree(exp_host);
    HeapFree(img_host);

    s_wm8440c_ran = 1;

    /* Lower-rung preservation (W5B/W7B/W8B samples). */
    if (WM_U32(WM_POOL_BE24) != pool_be24 || WM_U32(WM_MES_CCA4) != mes_cca4 ||
        WM_U32(WM_XP_C828) != xp0 || clut_ok != 16) {
        fprintf(stderr,
                "[worldmap-gpu-asset-a] ERROR: validation failed "
                "clut=%d/16 pool/mes/xp preserve\n",
                clut_ok);
        return -1;
    }

    fprintf(stderr, "[worldmap-gpu-asset-a] exit\n");
    fprintf(stderr,
            "[worldmap-gpu-asset-a] cut-before-next-head retail_pc=0x%08x\n",
            WM_CUT_BEFORE_979C8);
    return 0;
}

/*
 * W10B — native transcription of retail 0x800979C8–0x80097BBC (0x1F8 / 504 B).
 * Consumes W4C first-wave buffer at 0x8009C59C; larger VRAM path; two expand
 * passes (rows=32 each); 64 GetClut → CCB4; GetTPage → CD54 (×4) + CD5C (×3).
 */
static int s_wm979c8_ran;

static int wm_800979c8_gpu_asset_b(void)
{
    u32 compressed_psx;
    void* compressed_host;
    void* decomp_host;
    void* img_host;
    void* exp_host;
    const u8* scales;
    RECT rect;
    u16* clut_tbl;
    u16* tpage_cd54;
    u16* tpage_cd5c;
    int i;
    int clut_ok = 0;
    int tpage_ok = 0;
    u16 bce0_snap[16];
    u32 pool_be24;
    u32 mes_cca4;
    int tpage_x;
    int tpage_y;

    fprintf(stderr, "[worldmap-gpu-asset-b] entry\n");
    fprintf(stderr, "[worldmap-gpu-asset-b] source_slot=0x%08x\n",
            WM_DST_C59C);

    if (s_wm979c8_ran) {
        fprintf(stderr,
                "[worldmap-gpu-asset-b] ERROR: already ran this process "
                "(C59C is one-shot; reload lower ladder)\n");
        return -1;
    }
    if (!s_wm8440c_ran) {
        fprintf(stderr,
                "[worldmap-gpu-asset-b] ERROR: W10A did not run (required)\n");
        return -1;
    }

    compressed_psx = WM_U32(WM_DST_C59C);
    compressed_host = psx_u32_to_host(compressed_psx);
    if (compressed_psx == 0 || compressed_host == NULL) {
        fprintf(stderr,
                "[worldmap-gpu-asset-b] ERROR: C59C empty/null psx=0x%08x\n",
                compressed_psx);
        return -1;
    }

    /* Snapshot W10A CLUT table — must remain unchanged. */
    wm_memcpy(bce0_snap, PSX_ADDR(WM_CLUT_BCE0), sizeof(bce0_snap));
    pool_be24 = WM_U32(WM_POOL_BE24);
    mes_cca4 = WM_U32(WM_MES_CCA4);

    scales = (const u8*)PSX_ADDR(WM_SCALE_IMG_BB48);
    fprintf(stderr,
            "[worldmap-gpu-asset-b] compressed_psx=0x%08x host=%p "
            "scales=(0x%02x,0x%02x,0x%02x) expand_passes=2\n",
            compressed_psx, compressed_host, scales[0], scales[1], scales[2]);

    decomp_host = LZSSHeapDecompress(compressed_host, 1);
    if (decomp_host == NULL) {
        fprintf(stderr,
                "[worldmap-gpu-asset-b] ERROR: LZSSHeapDecompress failed\n");
        return -1;
    }
    fprintf(stderr, "[worldmap-gpu-asset-b] decompressed_host=%p\n",
            decomp_host);

    func_8002DD20((u32*)decomp_host);
    DrawSync(0);
    HeapFree(decomp_host);

    compressed_host = psx_u32_to_host(WM_U32(WM_DST_C59C));
    if (compressed_host != NULL)
        HeapFree(compressed_host);

    /* 1024 B store (2 rows × 256 × u16); 0x8000 B expand (64 rows × 256 × u16). */
    img_host = HeapAlloc(1024, 1);
    exp_host = HeapAlloc(0x8000, 1);
    if (img_host == NULL || exp_host == NULL) {
        fprintf(stderr, "[worldmap-gpu-asset-b] ERROR: HeapAlloc failed\n");
        return -1;
    }
    fprintf(stderr,
            "[worldmap-gpu-asset-b] work_img=%p work_exp=%p\n",
            img_host, exp_host);

    /* StoreImage (0,480,256,2). */
    rect.x = 0;
    rect.y = 480;
    rect.w = 256;
    rect.h = 2;
    fprintf(stderr,
            "[worldmap-gpu-asset-b] store_rect=(%d,%d,%d,%d)\n",
            rect.x, rect.y, rect.w, rect.h);
    StoreImage(&rect, (u_long*)img_host);
    DrawSync(0);

    /* Two expand passes: rows 0–31 from first store row; 32–63 from second. */
    wm_800931d8_expand((u16*)img_host, (u16*)exp_host, 32, scales);
    wm_800931d8_expand((u16*)((u8*)img_host + 512),
                       (u16*)((u8*)exp_host + 16384), 32, scales);

    /* LoadImage (0,432,256,64). */
    rect.x = 0;
    rect.y = 432;
    rect.w = 256;
    rect.h = 64;
    fprintf(stderr,
            "[worldmap-gpu-asset-b] load_rect=(%d,%d,%d,%d)\n",
            rect.x, rect.y, rect.w, rect.h);
    LoadImage(&rect, (u_long*)exp_host);
    DrawSync(0);

    /* GetClut(0, 432+i) × 64 → 0x8009CCB4. */
    clut_tbl = (u16*)PSX_ADDR(WM_CLUT_CCB4);
    fprintf(stderr,
            "[worldmap-gpu-asset-b] clut_count=64 clut_table=0x%08x\n",
            WM_CLUT_CCB4);
    for (i = 0; i < 64; i++) {
        int gx = 0;
        int gy = 432 + i;
        u16 formula = (u16)(((u32)gy << 6) | (((u32)gx >> 4) & 0x3fu));
        u16 got = GetClut(gx, gy);
        clut_tbl[i] = got;
        if (got == formula)
            clut_ok++;
    }
    fprintf(stderr, "[worldmap-gpu-asset-b] clut_match=%d/64\n", clut_ok);

    /* GetTPage(1,0,x,y): first 4 → CD54 (x=512..896 step 128, y=0). */
    tpage_cd54 = (u16*)PSX_ADDR(WM_TPAGE_CD54);
    tpage_x = 512;
    tpage_y = 0;
    for (i = 0; i < 4; i++) {
        u16 tp = GetTPage(1, 0, tpage_x, tpage_y);
        tpage_cd54[i] = tp;
        fprintf(stderr,
                "[worldmap-gpu-asset-b] tpage_cd54[%d] GetTPage(1,0,%d,%d)="
                "0x%04x\n",
                i, tpage_x, tpage_y, tp);
        tpage_ok++;
        tpage_x += 128;
    }

    /* Next 3 → CD5C (x=384..640 step 128, y=256). */
    tpage_cd5c = (u16*)PSX_ADDR(WM_TPAGE_CD5C);
    tpage_x = 384;
    tpage_y = 256;
    for (i = 0; i < 3; i++) {
        u16 tp = GetTPage(1, 0, tpage_x, tpage_y);
        tpage_cd5c[i] = tp;
        fprintf(stderr,
                "[worldmap-gpu-asset-b] tpage_cd5c[%d] GetTPage(1,0,%d,%d)="
                "0x%04x\n",
                i, tpage_x, tpage_y, tp);
        tpage_ok++;
        tpage_x += 128;
    }
    fprintf(stderr,
            "[worldmap-gpu-asset-b] tpage_writes=%d/7 "
            "persistent_store_cd54=0x%04x persistent_store_cd5c=0x%04x\n",
            tpage_ok, tpage_cd54[0], tpage_cd5c[0]);

    /* Free expand then store buffer (retail order). */
    HeapFree(exp_host);
    HeapFree(img_host);

    s_wm979c8_ran = 1;

    if (clut_ok != 64 || tpage_ok != 7 ||
        !wm_memeq(bce0_snap, PSX_ADDR(WM_CLUT_BCE0), sizeof(bce0_snap)) ||
        WM_U32(WM_POOL_BE24) != pool_be24 ||
        WM_U32(WM_MES_CCA4) != mes_cca4) {
        fprintf(stderr,
                "[worldmap-gpu-asset-b] ERROR: validation failed "
                "clut=%d/64 tpage=%d/7 w10a_preserved=%d\n",
                clut_ok, tpage_ok,
                wm_memeq(bce0_snap, PSX_ADDR(WM_CLUT_BCE0), sizeof(bce0_snap)));
        return -1;
    }

    fprintf(stderr, "[worldmap-gpu-asset-b] exit\n");
    fprintf(stderr,
            "[worldmap-gpu-asset-b] cut-before-next-head retail_pc=0x%08x\n",
            WM_CUT_BEFORE_84580);
    return 0;
}

/*
 * W11B — native transcription of retail 0x80084580–0x80084814 (0x298 / 664 B).
 * Builds n×84 object/matrix records from W4C fixup streams into heap @ C620.
 */
static int s_wm84580_ran;

static int wm_80084580_object_matrix(void)
{
    u32 cd48_psx;
    u32 d308_psx;
    u32 d308_final;
    u32 bd30_psx;
    u8* cd48_host;
    u8* bd30_host;
    u32* reloc;
    s32 reloc_count;
    s32 i;
    u16 entry_count;
    u32 alloc_size;
    void* table_host;
    u32 table_psx;
    VECTOR apply_out;
    SVECTOR apply_sv_out;
    u16 bce0_snap[16];
    u16 ccb4_0;
    u16 cd54_0;
    u32 pool_be24;
    u32 mes_cca4;
    u32 xp0;
    int final_ok = 1;

    fprintf(stderr, "[worldmap-object-matrix] entry\n");

    if (s_wm84580_ran) {
        fprintf(stderr,
                "[worldmap-object-matrix] ERROR: already ran this process "
                "(non-idempotent relocate/alloc)\n");
        return -1;
    }
    if (!s_wm979c8_ran) {
        fprintf(stderr,
                "[worldmap-object-matrix] ERROR: W10B did not run (required)\n");
        return -1;
    }

    cd48_psx = WM_U32(WM_FIX_CD48);
    d308_psx = WM_U32(WM_FIX_D308);
    bd30_psx = WM_U32(WM_FIX_BD30);
    cd48_host = (u8*)psx_u32_to_host(cd48_psx);
    bd30_host = (u8*)psx_u32_to_host(bd30_psx);

    if (cd48_psx == 0 || d308_psx == 0 || bd30_psx == 0 || cd48_host == NULL ||
        bd30_host == NULL) {
        fprintf(stderr,
                "[worldmap-object-matrix] ERROR: invalid W4C inputs "
                "CD48=0x%08x D308=0x%08x BD30=0x%08x\n",
                cd48_psx, d308_psx, bd30_psx);
        return -1;
    }

    wm_memcpy(bce0_snap, PSX_ADDR(WM_CLUT_BCE0), sizeof(bce0_snap));
    ccb4_0 = *(u16*)PSX_ADDR(WM_CLUT_CCB4);
    cd54_0 = *(u16*)PSX_ADDR(WM_TPAGE_CD54);
    pool_be24 = WM_U32(WM_POOL_BE24);
    mes_cca4 = WM_U32(WM_MES_CCA4);
    xp0 = WM_U32(WM_XP_C828);

    fprintf(stderr,
            "[worldmap-object-matrix] relocate_entry asset_base_psx=0x%08x "
            "relocation_table_psx=0x%08x\n",
            cd48_psx, d308_psx);

    /* 1: model graph relocate in-place. */
    reloc_count = func_8002C3E8(cd48_host);
    WM_S16(WM_OBJ_BD28) = (s16)reloc_count;

    /* 2: relocate `reloc_count` words at *D308+4; advance D308 by 4. */
    {
        u32 base = d308_psx;
        WM_U32(WM_FIX_D308) = base + 4u;
        d308_final = base + 4u;
        if (reloc_count > 0) {
            reloc = (u32*)psx_u32_to_host(base + 4u);
            if (reloc == NULL) {
                fprintf(stderr,
                        "[worldmap-object-matrix] ERROR: bad reloc table\n");
                return -1;
            }
            for (i = 0; i < reloc_count; i++)
                reloc[i] = base + reloc[i];
        }
    }

    fprintf(stderr,
            "[worldmap-object-matrix] relocate_exit relocation_count=%d "
            "relocation_table_final_psx=0x%08x bd28=%d\n",
            reloc_count, d308_final, (int)WM_S16(WM_OBJ_BD28));

    /* 3–4: entry count and allocation. */
    entry_count = *(u16*)bd30_host;
    if ((u32)entry_count > 0x10000u / WM_OBJ_RECORD_STRIDE) {
        fprintf(stderr,
                "[worldmap-object-matrix] ERROR: entry_count %u overflow\n",
                entry_count);
        return -1;
    }
    alloc_size = (u32)entry_count * WM_OBJ_RECORD_STRIDE;
    WM_U16(WM_OBJ_D7E0) = entry_count;

    table_host = HeapAlloc(alloc_size, 0);
    if (table_host == NULL && alloc_size != 0) {
        fprintf(stderr, "[worldmap-object-matrix] ERROR: HeapAlloc failed\n");
        return -1;
    }
    table_psx = host_ptr_to_psx_u32(table_host);
    WM_U32(WM_OBJ_C620) = table_psx;

    fprintf(stderr,
            "[worldmap-object-matrix] entry_count=%u record_stride=84 "
            "allocation_size=%u table_host=%p table_psx=0x%08x\n",
            entry_count, alloc_size, table_host, table_psx);

    /* 5: ApplyMatrix / ApplyMatrixSV with image matrices (GTE side effects;
     * retail leaves a1/a2 mostly unset — use BD30+2 as SVECTOR input and
     * stack outputs to avoid NULL deref on host). */
    {
        SVECTOR* vin = (SVECTOR*)(bd30_host + 2);
        ApplyMatrix((MATRIX*)PSX_ADDR(WM_OBJ_MAT_A140), vin, &apply_out);
        ApplyMatrixSV((MATRIX*)PSX_ADDR(WM_OBJ_MAT_A160), vin, &apply_sv_out);
    }

    /* 6: per-entry loop. */
    if (entry_count > 0 && table_host != NULL) {
        u8* s0 = bd30_host + 16; /* first 16-byte source record end */
        u16* s3 = (u16*)(bd30_host + 2);
        u32 s1 = 0; /* byte offset into table */

        for (i = 0; i < (int)entry_count; i++) {
            u8* e = (u8*)table_host + s1;
            s16 idx;
            u32 model_psx;
            u8* model_host;
            s32 copy_sz;

            *(u16*)(e + 0) = 0;
            *(u16*)(e + 2) = *s3;
            *(u16*)(e + 4) = *(u16*)(s0 - 12);
            *(s32*)(e + 8) = (s32) * (s16*)(s0 - 10);
            *(s32*)(e + 12) = (s32) * (s16*)(s0 - 8);
            *(s32*)(e + 16) = -(s32) * (s16*)(s0 - 6);
            *(u16*)(e + 24) = *(u16*)(s0 - 4);
            *(u16*)(e + 26) = *(u16*)(s0 - 2);
            *(u16*)(e + 28) = *(u16*)(s0 + 0);

            RotMatrix((SVECTOR*)(e + 24), (MATRIX*)(e + 32));

            idx = *(s16*)(e + 2);
            model_psx = cd48_psx + (u32)((s32)idx * 56) + 16u;
            *(u32*)(e + 64) = model_psx;
            model_host = (u8*)psx_u32_to_host(model_psx);
            if (model_host == NULL) {
                fprintf(stderr,
                        "[worldmap-object-matrix] ERROR: model null entry=%d "
                        "idx=%d\n",
                        i, (int)idx);
                return -1;
            }

            func_8002CB54(model_host, (u32*)(e + 72), (u32*)(e + 76));
            {
                void* out1 = (void*)(uintptr_t)(*(u32*)(e + 72));
                func_8002C8CC(model_host, out1, 1);
            }
            {
                void* out1 = (void*)(uintptr_t)(*(u32*)(e + 72));
                void* out2 = (void*)(uintptr_t)(*(u32*)(e + 76));
                copy_sz = *(s32*)(model_host + 0x34);
                if (copy_sz > 0 && out1 != NULL && out2 != NULL)
                    wm_memcpy(out2, out1, (unsigned)copy_sz);
            }

            /* Reloc table lookup: D308 + index*4 → ptr → entry+68; bump +4. */
            {
                u32 d308_cur = WM_U32(WM_FIX_D308);
                u32* slot =
                    (u32*)psx_u32_to_host(d308_cur + (u32)((s32)idx << 2));
                u32 p_psx;
                u8* p_host;
                u32 off;

                if (slot == NULL) {
                    fprintf(stderr,
                            "[worldmap-object-matrix] ERROR: reloc slot "
                            "entry=%d\n",
                            i);
                    return -1;
                }
                p_psx = *slot;
                *(u32*)(e + 68) = p_psx;
                p_host = (u8*)psx_u32_to_host(p_psx);
                if (p_host != NULL) {
                    off = *(u32*)(p_host + 4);
                    *(u32*)(p_host + 4) = p_psx + off;
                }
            }

            *(u32*)(e + 80) = 0;

            if (i == 0 || i == 1 || i == (int)entry_count - 1) {
                fprintf(stderr,
                        "[worldmap-object-matrix] entry[%d] src_off=%d "
                        "dst_off=%u idx=%d model_psx=0x%08x copy_sz=%d\n",
                        i, (int)((u8*)s3 - bd30_host), s1, (int)idx, model_psx,
                        copy_sz);
            }

            s3 = (u16*)((u8*)s3 + 16);
            s0 += 16;
            s1 += WM_OBJ_RECORD_STRIDE;
        }
    }

    /* 7: final persistent state. */
    D_80050100 = 2;
    WM_U32(WM_OBJ_C16C) = 0xFFFFFFFFu;
    WM_U32(WM_OBJ_C840) = 0xFFFFFFFFu;
    if (table_host != NULL)
        *(u16*)((u8*)table_host + 336) = 1;

    s_wm84580_ran = 1;

    if (WM_U16(WM_OBJ_D7E0) != entry_count || WM_U32(WM_OBJ_C620) != table_psx ||
        D_80050100 != 2 || WM_U32(WM_OBJ_C16C) != 0xFFFFFFFFu ||
        WM_U32(WM_OBJ_C840) != 0xFFFFFFFFu)
        final_ok = 0;
    if (table_host != NULL && *(u16*)((u8*)table_host + 336) != 1)
        final_ok = 0;
    if (!wm_memeq(bce0_snap, PSX_ADDR(WM_CLUT_BCE0), sizeof(bce0_snap)) ||
        *(u16*)PSX_ADDR(WM_CLUT_CCB4) != ccb4_0 ||
        *(u16*)PSX_ADDR(WM_TPAGE_CD54) != cd54_0 ||
        WM_U32(WM_POOL_BE24) != pool_be24 || WM_U32(WM_MES_CCA4) != mes_cca4 ||
        WM_U32(WM_XP_C828) != xp0)
        final_ok = 0;

    fprintf(stderr,
            "[worldmap-object-matrix] entry_count=%u table_psx=0x%08x "
            "final_flags_ok=%d\n",
            entry_count, table_psx, final_ok);

    if (!final_ok) {
        fprintf(stderr, "[worldmap-object-matrix] ERROR: validation failed\n");
        return -1;
    }

    fprintf(stderr, "[worldmap-object-matrix] exit\n");
    fprintf(stderr,
            "[worldmap-object-matrix] cut-before-third-wave retail_pc=0x%08x\n",
            WM_CUT_BEFORE_72090);
    return 0;
}

/*
 * W12B — retail 0x80072090–0x800721E0 (0x154 / 340 B): third-wave archive
 * submit. Five IDs from W2 BSS seed → DecodeAlignedSize → HeapAlloc → D3F8
 * queue + mirrors → func_80029AFC. No poll, no decompress, no residual after
 * return. HeapAlloc flags: first a1=1, remaining a1=0 (retail exact).
 */
static int s_wm72090_ran;

static int wm_80072090_third_wave(void)
{
    u32 ids[5];
    u32 sizes[5];
    void* hosts[5];
    u32 psx[5];
    s32 f304_before;
    s32 f304_after;
    int queue_result;
    int i;
    u32 c620_snap;
    u16 d7e0_snap;
    u32 pool_snap;
    u32 be4c0_snap;
    u16 bce0_0;

    fprintf(stderr, "[worldmap-third-wave] entry\n");

    if (s_wm72090_ran) {
        fprintf(stderr,
                "[worldmap-third-wave] ERROR: already ran this process "
                "(non-idempotent queue rebuild)\n");
        return -1;
    }
    if (!s_wm84580_ran) {
        fprintf(stderr,
                "[worldmap-third-wave] ERROR: W11B did not run (required)\n");
        return -1;
    }

    /* Preservation snapshots (W11B / earlier must not change). */
    c620_snap = WM_U32(WM_OBJ_C620);
    d7e0_snap = WM_U16(WM_OBJ_D7E0);
    pool_snap = WM_U32(WM_POOL_BE24);
    be4c0_snap = WM_U32(WM_TMPL_DST_BE4C);
    bce0_0 = *(u16*)PSX_ADDR(WM_CLUT_BCE0);

    /* 1. Counter ++ (host main BSS). */
    f304_before = D_8004F304;
    D_8004F304 = f304_before + 1;
    f304_after = D_8004F304;
    fprintf(stderr,
            "[worldmap-third-wave] D_8004F304_before=%d D_8004F304_after=%d\n",
            (int)f304_before, (int)f304_after);

    /* 2. Read five source IDs from world BSS (W2 seed) — not hard-coded. */
    ids[0] = WM_U32(WM_TW_ID_CC98);
    ids[1] = WM_U32(WM_TW_ID_D3D0);
    ids[2] = WM_U32(WM_TW_ID_D3C8);
    ids[3] = WM_U32(WM_TW_ID_D800);
    ids[4] = WM_U32(WM_TW_ID_BCC8);

    for (i = 0; i < 5; i++) {
        fprintf(stderr, "[worldmap-third-wave] source_id[%d]=0x%08x (%u)\n", i,
                ids[i], ids[i]);
    }

    /* 3–6. Decode / alloc / store in retail interleave order.
     * Queue pData always KUSEG. World-BSS mirrors KUSEG. D_8006259C is host
     * void* (SEDS-style main global), matching menu/field writers. */
    sizes[0] = (u32)ArchiveDecodeAlignedSize(ids[0]);
    hosts[0] = HeapAlloc(sizes[0], 1);
    psx[0] = host_ptr_to_psx_u32(hosts[0]);
    WM_U16(WM_REQ_D3F8) = (u16)ids[0];
    WM_U32(WM_TW_MIRROR_C88C) = psx[0];
    WM_U32(WM_REQ_D3FC) = psx[0];

    sizes[1] = (u32)ArchiveDecodeAlignedSize(ids[1]);
    hosts[1] = HeapAlloc(sizes[1], 0);
    psx[1] = host_ptr_to_psx_u32(hosts[1]);
    WM_U16(WM_REQ_D400) = (u16)ids[1];
    WM_U32(WM_TW_MIRROR_C884) = psx[1];
    WM_U32(WM_REQ_D404) = psx[1];

    sizes[2] = (u32)ArchiveDecodeAlignedSize(ids[2]);
    hosts[2] = HeapAlloc(sizes[2], 0);
    psx[2] = host_ptr_to_psx_u32(hosts[2]);
    WM_U16(WM_REQ_D408) = (u16)ids[2];
    D_8006259C = hosts[2]; /* host pointer authority */
    WM_U32(WM_REQ_D40C) = psx[2];

    sizes[3] = (u32)ArchiveDecodeAlignedSize(ids[3]);
    hosts[3] = HeapAlloc(sizes[3], 0);
    psx[3] = host_ptr_to_psx_u32(hosts[3]);
    WM_U16(WM_REQ_D410) = (u16)ids[3];
    WM_U32(WM_TW_MIRROR_C888) = psx[3];
    WM_U32(WM_REQ_D414) = psx[3];

    sizes[4] = (u32)ArchiveDecodeAlignedSize(ids[4]);
    hosts[4] = HeapAlloc(sizes[4], 0);
    psx[4] = host_ptr_to_psx_u32(hosts[4]);
    WM_U16(WM_REQ_D418) = (u16)ids[4];
    WM_U32(WM_TW_MIRROR_C614) = psx[4];
    WM_U32(WM_REQ_D41C) = psx[4];

    /* Terminator */
    WM_U16(WM_REQ_D420) = 0;
    WM_U32(WM_REQ_D424) = 0;

    for (i = 0; i < 5; i++) {
        fprintf(stderr,
                "[worldmap-third-wave] archive_id[%d]=%u aligned_size[%d]=%u "
                "destination_psx[%d]=0x%08x host=%p\n",
                i, ids[i], i, sizes[i], i, psx[i], hosts[i]);
    }

    for (i = 0; i < 5; i++) {
        if (hosts[i] == NULL && sizes[i] != 0) {
            fprintf(stderr,
                    "[worldmap-third-wave] ERROR: HeapAlloc failed entry=%d "
                    "size=%u\n",
                    i, sizes[i]);
            return -1;
        }
    }

    fprintf(stderr, "[worldmap-third-wave] request_count=5\n");
    for (i = 0; i < 5; i++) {
        fprintf(stderr,
                "[worldmap-third-wave] request[%d] archive=%u pData_psx=0x%08x\n",
                i, (unsigned)(u16)ids[i], psx[i]);
    }
    fprintf(stderr,
            "[worldmap-third-wave] mirrors C88C=0x%08x C884=0x%08x "
            "D_8006259C_host=%p C888=0x%08x C614=0x%08x\n",
            WM_U32(WM_TW_MIRROR_C88C), WM_U32(WM_TW_MIRROR_C884), D_8006259C,
            WM_U32(WM_TW_MIRROR_C888), WM_U32(WM_TW_MIRROR_C614));

    /* 7. Submit — same PSX-layout queue as W3B/W4C. */
    queue_result = func_80029AFC(PSX_ADDR(WM_REQ_D3F8), 0, 0);
    fprintf(stderr, "[worldmap-third-wave] queue_submit=%d\n", queue_result);

    /* Preserve prior rungs. */
    if (WM_U32(WM_OBJ_C620) != c620_snap || WM_U16(WM_OBJ_D7E0) != d7e0_snap ||
        WM_U32(WM_POOL_BE24) != pool_snap ||
        WM_U32(WM_TMPL_DST_BE4C) != be4c0_snap ||
        *(u16*)PSX_ADDR(WM_CLUT_BCE0) != bce0_0) {
        fprintf(stderr,
                "[worldmap-third-wave] ERROR: prior rung state corrupted\n");
        return -1;
    }
    if (f304_after != f304_before + 1) {
        fprintf(stderr, "[worldmap-third-wave] ERROR: D_8004F304 not +1\n");
        return -1;
    }

    s_wm72090_ran = 1;
    fprintf(stderr, "[worldmap-third-wave] exit\n");
    fprintf(stderr,
            "[worldmap-third-wave] cut-before-next-step retail_pc=0x%08x\n",
            WM_CUT_BEFORE_736DC);
    return 0;
}

/*
 * W13B — retail 0x800736DC–0x800737E8 (0x110 / 272 B): unrolled BSS constant
 * paint. Leaf, no loads, no callees. Caller delay at 0x80072460 is nop.
 * 48 stores (32×sw + 16×sb); unique 136 bytes 0x8009D197–0x8009D2AF with
 * intentional later-sb overlays on earlier word stores. Neutral names only.
 */
#define WM_BSS_CONST_A           0x00FF7A70u
#define WM_BSS_CONST_B           0x00FFF5E0u
#define WM_BSS_CONST_C           0x00C03745u
#define WM_BSS_CONST_K8          0x08u
#define WM_BSS_CONST_K56         0x38u
#define WM_BSS_STORE_COUNT       48
#define WM_BSS_UNIQUE_BYTES      136
#define WM_BSS_LOWEST            0x8009D197u
#define WM_BSS_HIGHEST           0x8009D2AFu

/* Expected final bytes at unique addresses (ascending) after all 48 stores. */
static const u32 k_bss_const_addrs[WM_BSS_UNIQUE_BYTES] = {
    0x8009D197u, 0x8009D198u, 0x8009D199u, 0x8009D19Au, 0x8009D19Bu,
    0x8009D1A0u, 0x8009D1A1u, 0x8009D1A2u, 0x8009D1A3u, 0x8009D1A8u,
    0x8009D1A9u, 0x8009D1AAu, 0x8009D1ABu, 0x8009D1B0u, 0x8009D1B1u,
    0x8009D1B2u, 0x8009D1B3u, 0x8009D1BBu, 0x8009D1BCu, 0x8009D1BDu,
    0x8009D1BEu, 0x8009D1BFu, 0x8009D1C4u, 0x8009D1C5u, 0x8009D1C6u,
    0x8009D1C7u, 0x8009D1CCu, 0x8009D1CDu, 0x8009D1CEu, 0x8009D1CFu,
    0x8009D1D4u, 0x8009D1D5u, 0x8009D1D6u, 0x8009D1D7u, 0x8009D1DFu,
    0x8009D1E0u, 0x8009D1E1u, 0x8009D1E2u, 0x8009D1E3u, 0x8009D1E8u,
    0x8009D1E9u, 0x8009D1EAu, 0x8009D1EBu, 0x8009D1F0u, 0x8009D1F1u,
    0x8009D1F2u, 0x8009D1F3u, 0x8009D1F8u, 0x8009D1F9u, 0x8009D1FAu,
    0x8009D1FBu, 0x8009D203u, 0x8009D204u, 0x8009D205u, 0x8009D206u,
    0x8009D207u, 0x8009D20Cu, 0x8009D20Du, 0x8009D20Eu, 0x8009D20Fu,
    0x8009D214u, 0x8009D215u, 0x8009D216u, 0x8009D217u, 0x8009D21Cu,
    0x8009D21Du, 0x8009D21Eu, 0x8009D21Fu, 0x8009D227u, 0x8009D228u,
    0x8009D229u, 0x8009D22Au, 0x8009D22Bu, 0x8009D230u, 0x8009D231u,
    0x8009D232u, 0x8009D233u, 0x8009D238u, 0x8009D239u, 0x8009D23Au,
    0x8009D23Bu, 0x8009D240u, 0x8009D241u, 0x8009D242u, 0x8009D243u,
    0x8009D24Bu, 0x8009D24Cu, 0x8009D24Du, 0x8009D24Eu, 0x8009D24Fu,
    0x8009D254u, 0x8009D255u, 0x8009D256u, 0x8009D257u, 0x8009D25Cu,
    0x8009D25Du, 0x8009D25Eu, 0x8009D25Fu, 0x8009D264u, 0x8009D265u,
    0x8009D266u, 0x8009D267u, 0x8009D26Fu, 0x8009D270u, 0x8009D271u,
    0x8009D272u, 0x8009D273u, 0x8009D278u, 0x8009D279u, 0x8009D27Au,
    0x8009D27Bu, 0x8009D280u, 0x8009D281u, 0x8009D282u, 0x8009D283u,
    0x8009D288u, 0x8009D289u, 0x8009D28Au, 0x8009D28Bu, 0x8009D293u,
    0x8009D294u, 0x8009D295u, 0x8009D296u, 0x8009D297u, 0x8009D29Cu,
    0x8009D29Du, 0x8009D29Eu, 0x8009D29Fu, 0x8009D2A4u, 0x8009D2A5u,
    0x8009D2A6u, 0x8009D2A7u, 0x8009D2ACu, 0x8009D2ADu, 0x8009D2AEu,
    0x8009D2AFu,
};
static const u8 k_bss_const_expect[WM_BSS_UNIQUE_BYTES] = {
    0x08u, 0x70u, 0x7Au, 0xFFu, 0x38u, 0x70u, 0x7Au, 0xFFu, 0x00u, 0xE0u, 0xF5u,
    0xFFu, 0x00u, 0xE0u, 0xF5u, 0xFFu, 0x00u, 0x08u, 0x70u, 0x7Au, 0xFFu, 0x38u,
    0x70u, 0x7Au, 0xFFu, 0x00u, 0xE0u, 0xF5u, 0xFFu, 0x00u, 0xE0u, 0xF5u, 0xFFu,
    0x00u, 0x08u, 0x45u, 0x37u, 0xC0u, 0x38u, 0x45u, 0x37u, 0xC0u, 0x00u, 0x70u,
    0x7Au, 0xFFu, 0x00u, 0x70u, 0x7Au, 0xFFu, 0x00u, 0x08u, 0x45u, 0x37u, 0xC0u,
    0x38u, 0x45u, 0x37u, 0xC0u, 0x00u, 0x70u, 0x7Au, 0xFFu, 0x00u, 0x70u, 0x7Au,
    0xFFu, 0x00u, 0x08u, 0x45u, 0x37u, 0xC0u, 0x38u, 0x45u, 0x37u, 0xC0u, 0x00u,
    0x45u, 0x37u, 0xC0u, 0x00u, 0x45u, 0x37u, 0xC0u, 0x00u, 0x08u, 0x45u, 0x37u,
    0xC0u, 0x38u, 0x45u, 0x37u, 0xC0u, 0x00u, 0x45u, 0x37u, 0xC0u, 0x00u, 0x45u,
    0x37u, 0xC0u, 0x00u, 0x08u, 0x45u, 0x37u, 0xC0u, 0x38u, 0x45u, 0x37u, 0xC0u,
    0x00u, 0x45u, 0x37u, 0xC0u, 0x00u, 0x45u, 0x37u, 0xC0u, 0x00u, 0x08u, 0x45u,
    0x37u, 0xC0u, 0x38u, 0x45u, 0x37u, 0xC0u, 0x00u, 0x45u, 0x37u, 0xC0u, 0x00u,
    0x45u, 0x37u, 0xC0u, 0x00u,
};
#define WM_BSS_EXPECT_FNV 0x4323F4C5u

static int s_wm736dc_ran;

static u32 wm_bss_footprint_hash(void)
{
    /* FNV-1a 32-bit over unique footprint bytes in ascending address order. */
    u32 h = 2166136261u;
    int i;
    for (i = 0; i < WM_BSS_UNIQUE_BYTES; i++) {
        h ^= (u32)WM_U8(k_bss_const_addrs[i]);
        h *= 16777619u;
    }
    return h;
}

static int wm_800736DC_init_constants(void)
{
    u32 pre_hash;
    u32 post_hash;
    u32 guard_lo[4];
    u32 guard_hi[4];
    u32 c88c_snap;
    u32 c620_snap;
    u32 pool_snap;
    u32 be4c0_snap;
    int match;
    int i;

    fprintf(stderr, "[worldmap-bss-constants] entry\n");

    if (s_wm736dc_ran) {
        fprintf(stderr,
                "[worldmap-bss-constants] ERROR: already ran this process\n");
        return -1;
    }
    if (!s_wm72090_ran) {
        fprintf(stderr,
                "[worldmap-bss-constants] ERROR: W12B did not run (required)\n");
        return -1;
    }

    /* Guards + prior-rung snaps. */
    for (i = 0; i < 4; i++) {
        guard_lo[i] = WM_U32(WM_BSS_LOWEST - 16u + (u32)i * 4u);
        guard_hi[i] = WM_U32(WM_BSS_HIGHEST + 1u + (u32)i * 4u);
    }
    c88c_snap = WM_U32(WM_TW_MIRROR_C88C);
    c620_snap = WM_U32(WM_OBJ_C620);
    pool_snap = WM_U32(WM_POOL_BE24);
    be4c0_snap = WM_U32(WM_TMPL_DST_BE4C);
    pre_hash = wm_bss_footprint_hash();

    /* Retail store order — unrolled; later sb overlays word tails. */
    WM_U32(0x8009D1C4u) = WM_BSS_CONST_A;
    WM_U32(0x8009D1BCu) = WM_BSS_CONST_A;
    WM_U32(0x8009D1A0u) = WM_BSS_CONST_A;
    WM_U32(0x8009D198u) = WM_BSS_CONST_A;
    WM_U32(0x8009D1D4u) = WM_BSS_CONST_B;
    WM_U32(0x8009D1CCu) = WM_BSS_CONST_B;
    WM_U32(0x8009D1B0u) = WM_BSS_CONST_B;
    WM_U32(0x8009D1A8u) = WM_BSS_CONST_B;
    WM_U8(0x8009D197u) = WM_BSS_CONST_K8;
    WM_U8(0x8009D19Bu) = WM_BSS_CONST_K56;
    WM_U8(0x8009D1BBu) = WM_BSS_CONST_K8;
    WM_U8(0x8009D1BFu) = WM_BSS_CONST_K56;
    WM_U32(0x8009D20Cu) = WM_BSS_CONST_C;
    WM_U32(0x8009D204u) = WM_BSS_CONST_C;
    WM_U32(0x8009D1E8u) = WM_BSS_CONST_C;
    WM_U32(0x8009D1E0u) = WM_BSS_CONST_C;
    WM_U32(0x8009D21Cu) = WM_BSS_CONST_A;
    WM_U32(0x8009D214u) = WM_BSS_CONST_A;
    WM_U32(0x8009D1F8u) = WM_BSS_CONST_A;
    WM_U32(0x8009D1F0u) = WM_BSS_CONST_A;
    WM_U8(0x8009D1DFu) = WM_BSS_CONST_K8;
    WM_U8(0x8009D1E3u) = WM_BSS_CONST_K56;
    WM_U8(0x8009D203u) = WM_BSS_CONST_K8;
    WM_U8(0x8009D207u) = WM_BSS_CONST_K56;
    WM_U32(0x8009D264u) = WM_BSS_CONST_C;
    WM_U32(0x8009D25Cu) = WM_BSS_CONST_C;
    WM_U32(0x8009D254u) = WM_BSS_CONST_C;
    WM_U32(0x8009D24Cu) = WM_BSS_CONST_C;
    WM_U32(0x8009D240u) = WM_BSS_CONST_C;
    WM_U32(0x8009D238u) = WM_BSS_CONST_C;
    WM_U32(0x8009D230u) = WM_BSS_CONST_C;
    WM_U32(0x8009D228u) = WM_BSS_CONST_C;
    WM_U8(0x8009D227u) = WM_BSS_CONST_K8;
    WM_U8(0x8009D22Bu) = WM_BSS_CONST_K56;
    WM_U8(0x8009D24Bu) = WM_BSS_CONST_K8;
    WM_U8(0x8009D24Fu) = WM_BSS_CONST_K56;
    WM_U32(0x8009D2ACu) = WM_BSS_CONST_C;
    WM_U32(0x8009D2A4u) = WM_BSS_CONST_C;
    WM_U32(0x8009D29Cu) = WM_BSS_CONST_C;
    WM_U32(0x8009D294u) = WM_BSS_CONST_C;
    WM_U32(0x8009D288u) = WM_BSS_CONST_C;
    WM_U32(0x8009D280u) = WM_BSS_CONST_C;
    WM_U32(0x8009D278u) = WM_BSS_CONST_C;
    WM_U32(0x8009D270u) = WM_BSS_CONST_C;
    WM_U8(0x8009D26Fu) = WM_BSS_CONST_K8;
    WM_U8(0x8009D273u) = WM_BSS_CONST_K56;
    WM_U8(0x8009D293u) = WM_BSS_CONST_K8;
    WM_U8(0x8009D297u) = WM_BSS_CONST_K56; /* jr delay slot */

    post_hash = wm_bss_footprint_hash();
    match = 0;
    for (i = 0; i < WM_BSS_UNIQUE_BYTES; i++) {
        if (WM_U8(k_bss_const_addrs[i]) == k_bss_const_expect[i])
            match++;
    }

    fprintf(stderr,
            "[worldmap-bss-constants] store_count=%d unique_bytes=%d "
            "lowest_address=0x%08x highest_address=0x%08x\n",
            WM_BSS_STORE_COUNT, WM_BSS_UNIQUE_BYTES, WM_BSS_LOWEST,
            WM_BSS_HIGHEST);
    fprintf(stderr,
            "[worldmap-bss-constants] pre_hash=0x%08x post_hash=0x%08x "
            "expected_fnv=0x%08x expected_bytes=%d/%d\n",
            pre_hash, post_hash, WM_BSS_EXPECT_FNV, match, WM_BSS_UNIQUE_BYTES);

    if (match != WM_BSS_UNIQUE_BYTES || post_hash != WM_BSS_EXPECT_FNV) {
        fprintf(stderr,
                "[worldmap-bss-constants] ERROR: expected constant mismatch\n");
        return -1;
    }

    for (i = 0; i < 4; i++) {
        if (WM_U32(WM_BSS_LOWEST - 16u + (u32)i * 4u) != guard_lo[i] ||
            WM_U32(WM_BSS_HIGHEST + 1u + (u32)i * 4u) != guard_hi[i]) {
            fprintf(stderr,
                    "[worldmap-bss-constants] ERROR: neighbor guard changed\n");
            return -1;
        }
    }
    if (WM_U32(WM_TW_MIRROR_C88C) != c88c_snap ||
        WM_U32(WM_OBJ_C620) != c620_snap ||
        WM_U32(WM_POOL_BE24) != pool_snap ||
        WM_U32(WM_TMPL_DST_BE4C) != be4c0_snap) {
        fprintf(stderr,
                "[worldmap-bss-constants] ERROR: prior rung state corrupted\n");
        return -1;
    }

    s_wm736dc_ran = 1;
    fprintf(stderr, "[worldmap-bss-constants] exit\n");
    fprintf(stderr,
            "[worldmap-bss-constants] cut-before-next-step retail_pc=0x%08x\n",
            WM_CUT_BEFORE_73E30);
    return 0;
}

/* Compile-time layout locks for PsyQ packet structs used by 0x80073E30. */
typedef char wm_assert_dr_tpage_8[(sizeof(DR_TPAGE) == 8) ? 1 : -1];
typedef char wm_assert_poly_ft4_40[(sizeof(POLY_FT4) == 40) ? 1 : -1];
typedef char wm_assert_poly_g3_28[(sizeof(POLY_G3) == 28) ? 1 : -1];
typedef char wm_assert_tile_16[(sizeof(TILE) == 16) ? 1 : -1];

static int s_wm73e30_ran;

static u32 wm_prim_fnv1a(const u8* p, unsigned n)
{
    u32 h = 2166136261u;
    unsigned i;
    for (i = 0; i < n; i++) {
        h ^= (u32)p[i];
        h *= 16777619u;
    }
    return h;
}

/*
 * Retail 0x80073E30 — one-shot world BSS GPU packet-template initializer.
 * Leaf: GetTPage / GetClut / SetSemiTrans / SetDrawTPage only.
 * Cut residual before 0x8007246C (jal 0x80085F58).
 */
static int wm_80073E30_primitive_templates(void)
{
    POLY_FT4* ft4_0;
    POLY_FT4* ft4_1;
    DR_TPAGE* dr;
    u16 tpage0;
    u16 tpage1;
    u16 clut;
    u32 dr_mode;
    int i;
    int j;
    int match_ft4;
    int match_g3;
    int match_tile;
    int unexpected;
    u32 pre_dr_h;
    u32 pre_ft4_0_h;
    u32 pre_ft4_1_h;
    u32 pre_g3_h;
    u32 pre_tile_h;
    u32 post_dr_h;
    u32 post_ft4_0_h;
    u32 post_ft4_1_h;
    u32 post_g3_h;
    u32 post_tile_h;
    u8 snap_dr[8];
    u8 snap_ft4_0[40];
    u8 snap_ft4_1[40];
    u8 snap_g3[WM_PRIM_G3_BYTES];
    u8 snap_tile[WM_PRIM_TILE_BYTES];
    u8 expect_dr[8];
    u8 expect_ft4[40];
    u8 expect_g3[WM_PRIM_G3_BYTES];
    u8 expect_tile[WM_PRIM_TILE_BYTES];
    /* Guard / gap / prior-rung snaps */
    u32 guard_before_dr[4];
    u32 gap_c5a8;
    u32 gap_c614;
    u32 gap_c620;
    u32 gap_c660;
    u32 gap_c7ec;
    u32 gap_c88c;
    u32 gap_c894;
    u32 bss_d198;
    u32 pool_snap;
    u32 be4c0_snap;
    u32 c620_snap;

    fprintf(stderr, "[worldmap-primitive-templates] entry\n");

    if (s_wm73e30_ran) {
        fprintf(stderr,
                "[worldmap-primitive-templates] ERROR: already ran this "
                "process\n");
        return -1;
    }
    if (!s_wm736dc_ran) {
        fprintf(stderr,
                "[worldmap-primitive-templates] ERROR: W13B did not run "
                "(required)\n");
        return -1;
    }

    ft4_0 = (POLY_FT4*)PSX_ADDR(WM_PRIM_FT4_0);
    ft4_1 = (POLY_FT4*)PSX_ADDR(WM_PRIM_FT4_1);
    dr = (DR_TPAGE*)PSX_ADDR(WM_PRIM_DR_TPAGE);

    /* Snapshots (full regions + sensitive gaps + prior rungs). */
    wm_memcpy(snap_dr, PSX_ADDR(WM_PRIM_DR_TPAGE), 8);
    wm_memcpy(snap_ft4_0, PSX_ADDR(WM_PRIM_FT4_0), 40);
    wm_memcpy(snap_ft4_1, PSX_ADDR(WM_PRIM_FT4_1), 40);
    wm_memcpy(snap_g3, PSX_ADDR(WM_PRIM_G3_BASE), WM_PRIM_G3_BYTES);
    wm_memcpy(snap_tile, PSX_ADDR(WM_PRIM_TILE_BASE), WM_PRIM_TILE_BYTES);
    for (i = 0; i < 4; i++)
        guard_before_dr[i] =
            WM_U32(WM_PRIM_DR_TPAGE - 16u + (u32)i * 4u);
    gap_c5a8 = WM_U32(0x8009C5A8u);
    gap_c614 = WM_U32(0x8009C614u);
    gap_c620 = WM_U32(0x8009C620u);
    gap_c660 = WM_U32(0x8009C660u);
    gap_c7ec = WM_U32(WM_FIX_C7EC);
    gap_c88c = WM_U32(WM_TW_MIRROR_C88C);
    gap_c894 = WM_U32(WM_FLAG_C894_ABS);
    bss_d198 = WM_U32(0x8009D198u);
    pool_snap = WM_U32(WM_POOL_BE24);
    be4c0_snap = WM_U32(WM_TMPL_DST_BE4C);
    c620_snap = WM_U32(WM_OBJ_C620);

    pre_dr_h = wm_prim_fnv1a(snap_dr, 8);
    pre_ft4_0_h = wm_prim_fnv1a(snap_ft4_0, 40);
    pre_ft4_1_h = wm_prim_fnv1a(snap_ft4_1, 40);
    pre_g3_h = wm_prim_fnv1a(snap_g3, WM_PRIM_G3_BYTES);
    pre_tile_h = wm_prim_fnv1a(snap_tile, WM_PRIM_TILE_BYTES);

    /* ---- Retail store order (0x80073E30) ---- */

    /* POLY_FT4[0] header + geometry + UV + RGB (before helpers). */
    WM_U8(WM_PRIM_FT4_0 + 3u) = 9;       /* len */
    WM_U8(WM_PRIM_FT4_0 + 7u) = 0x2C;    /* code POLY_FT4 */
    WM_U16(WM_PRIM_FT4_0 + 0x0Au) = 120; /* y0 */
    WM_U16(WM_PRIM_FT4_0 + 0x12u) = 120; /* y1 */
    WM_U16(WM_PRIM_FT4_0 + 0x1Au) = 215; /* y2 */
    WM_U16(WM_PRIM_FT4_0 + 0x22u) = 215; /* y3 */
    WM_U16(WM_PRIM_FT4_0 + 0x08u) = 208; /* x0 */
    WM_U16(WM_PRIM_FT4_0 + 0x18u) = 208; /* x2 */
    WM_U16(WM_PRIM_FT4_0 + 0x10u) = 311; /* x1 */
    WM_U16(WM_PRIM_FT4_0 + 0x20u) = 311; /* x3 */
    WM_U8(WM_PRIM_FT4_0 + 0x0Cu) = 0;    /* u0 */
    WM_U8(WM_PRIM_FT4_0 + 0x0Du) = 128;  /* v0 */
    WM_U8(WM_PRIM_FT4_0 + 0x14u) = 127;  /* u1 */
    WM_U8(WM_PRIM_FT4_0 + 0x15u) = 128;  /* v1 */
    WM_U8(WM_PRIM_FT4_0 + 0x1Cu) = 0;    /* u2 */
    WM_U8(WM_PRIM_FT4_0 + 0x1Du) = 255;  /* v2 */
    WM_U8(WM_PRIM_FT4_0 + 0x24u) = 127;  /* u3 */
    WM_U8(WM_PRIM_FT4_0 + 0x25u) = 255;  /* v3 */
    WM_U8(WM_PRIM_FT4_0 + 4u) = 128;     /* r0 */
    WM_U8(WM_PRIM_FT4_0 + 5u) = 128;     /* g0 */
    WM_U8(WM_PRIM_FT4_0 + 6u) = 128;     /* b0 */

    tpage0 = GetTPage(0, 0, 896, 256);
    WM_U16(WM_PRIM_FT4_0 + 0x16u) = tpage0; /* tpage */

    clut = GetClut(256, 510);
    WM_U16(WM_PRIM_FT4_0 + 0x0Eu) = clut; /* clut */

    SetSemiTrans(ft4_0, 1);

    /* Retail 40-byte copy: 16 + 16 + 8 words from C5C0 → C5E8. */
    {
        u32* src = (u32*)PSX_ADDR(WM_PRIM_FT4_0);
        u32* dst = (u32*)PSX_ADDR(WM_PRIM_FT4_1);
        dst[0] = src[0];
        dst[1] = src[1];
        dst[2] = src[2];
        dst[3] = src[3];
        dst[4] = src[4];
        dst[5] = src[5];
        dst[6] = src[6];
        dst[7] = src[7];
        dst[8] = src[8];
        dst[9] = src[9];
    }

    tpage1 = GetTPage(0, 1, 896, 256);
    SetDrawTPage(dr, 1, 0, (int)(tpage1 & 0xFFFFu));
    dr_mode = ((u32*)dr)[1];

    /* POLY_G3 × 8 — identical entries; SetSemiTrans each. */
    for (i = 0; i < (int)WM_PRIM_G3_COUNT; i++) {
        u32 base = WM_PRIM_G3_BASE + (u32)i * WM_PRIM_G3_STRIDE;
        POLY_G3* g3 = (POLY_G3*)PSX_ADDR(base);
        WM_U8(base + 3u) = 6;    /* len */
        WM_U8(base + 7u) = 0x30; /* code POLY_G3 */
        WM_U8(base + 4u) = 255;  /* r0 */
        WM_U8(base + 5u) = 64;   /* g0 */
        WM_U8(base + 6u) = 64;   /* b0 */
        WM_U8(base + 12u) = 0;   /* r1 */
        WM_U8(base + 13u) = 0;   /* g1 */
        WM_U8(base + 14u) = 0;   /* b1 */
        WM_U8(base + 20u) = 0;   /* r2 */
        WM_U8(base + 21u) = 0;   /* g2 */
        WM_U8(base + 22u) = 0;   /* b2 */
        SetSemiTrans(g3, 1);
    }

    /* TILE × 64 */
    for (i = 0; i < (int)WM_PRIM_TILE_COUNT; i++) {
        u32 base = WM_PRIM_TILE_BASE + (u32)i * WM_PRIM_TILE_STRIDE;
        WM_U8(base + 3u) = 3;    /* len */
        WM_U8(base + 7u) = 0x60; /* code TILE */
        WM_U8(base + 4u) = 128;  /* r0 */
        WM_U8(base + 5u) = 128;  /* g0 */
        WM_U8(base + 6u) = 16;   /* b0 */
        WM_U16(base + 12u) = 2;  /* w */
        WM_U16(base + 14u) = 2;  /* h */
    }

    /* ---- Build expected image from pre-snap + retail writes ---- */
    wm_memcpy(expect_dr, snap_dr, 8);
    expect_dr[3] = 1;
    {
        u32 mode = 0xE1000000u | 0x0400u | (0x003Eu & 0x9FFu);
        expect_dr[4] = (u8)(mode);
        expect_dr[5] = (u8)(mode >> 8);
        expect_dr[6] = (u8)(mode >> 16);
        expect_dr[7] = (u8)(mode >> 24);
    }

    wm_memcpy(expect_ft4, snap_ft4_0, 40);
    expect_ft4[3] = 9;
    expect_ft4[4] = 128;
    expect_ft4[5] = 128;
    expect_ft4[6] = 128;
    expect_ft4[7] = 0x2E;
    expect_ft4[8] = (u8)(208);
    expect_ft4[9] = (u8)(208 >> 8);
    expect_ft4[10] = (u8)(120);
    expect_ft4[11] = (u8)(120 >> 8);
    expect_ft4[12] = 0;
    expect_ft4[13] = 128;
    expect_ft4[14] = (u8)(0x7F90);
    expect_ft4[15] = (u8)(0x7F90 >> 8);
    expect_ft4[16] = (u8)(311);
    expect_ft4[17] = (u8)(311 >> 8);
    expect_ft4[18] = (u8)(120);
    expect_ft4[19] = (u8)(120 >> 8);
    expect_ft4[20] = 127;
    expect_ft4[21] = 128;
    expect_ft4[22] = (u8)(0x001E);
    expect_ft4[23] = (u8)(0x001E >> 8);
    expect_ft4[24] = (u8)(208);
    expect_ft4[25] = (u8)(208 >> 8);
    expect_ft4[26] = (u8)(215);
    expect_ft4[27] = (u8)(215 >> 8);
    expect_ft4[28] = 0;
    expect_ft4[29] = 255;
    /* +30,+31 pad1 untouched (from snap) */
    expect_ft4[32] = (u8)(311);
    expect_ft4[33] = (u8)(311 >> 8);
    expect_ft4[34] = (u8)(215);
    expect_ft4[35] = (u8)(215 >> 8);
    expect_ft4[36] = 127;
    expect_ft4[37] = 255;
    /* +38,+39 pad2 untouched */

    wm_memcpy(expect_g3, snap_g3, WM_PRIM_G3_BYTES);
    for (i = 0; i < (int)WM_PRIM_G3_COUNT; i++) {
        u8* e = expect_g3 + i * (int)WM_PRIM_G3_STRIDE;
        e[3] = 6;
        e[4] = 255;
        e[5] = 64;
        e[6] = 64;
        e[7] = 0x32;
        e[12] = 0;
        e[13] = 0;
        e[14] = 0;
        e[20] = 0;
        e[21] = 0;
        e[22] = 0;
    }

    wm_memcpy(expect_tile, snap_tile, WM_PRIM_TILE_BYTES);
    for (i = 0; i < (int)WM_PRIM_TILE_COUNT; i++) {
        u8* e = expect_tile + i * (int)WM_PRIM_TILE_STRIDE;
        e[3] = 3;
        e[4] = 128;
        e[5] = 128;
        e[6] = 16;
        e[7] = 0x60;
        e[12] = 2;
        e[13] = 0;
        e[14] = 2;
        e[15] = 0;
    }

    post_dr_h = wm_prim_fnv1a((const u8*)PSX_ADDR(WM_PRIM_DR_TPAGE), 8);
    post_ft4_0_h = wm_prim_fnv1a((const u8*)PSX_ADDR(WM_PRIM_FT4_0), 40);
    post_ft4_1_h = wm_prim_fnv1a((const u8*)PSX_ADDR(WM_PRIM_FT4_1), 40);
    post_g3_h =
        wm_prim_fnv1a((const u8*)PSX_ADDR(WM_PRIM_G3_BASE), WM_PRIM_G3_BYTES);
    post_tile_h = wm_prim_fnv1a((const u8*)PSX_ADDR(WM_PRIM_TILE_BASE),
                                WM_PRIM_TILE_BYTES);

    match_ft4 = 0;
    for (j = 0; j < 40; j++) {
        if (((const u8*)PSX_ADDR(WM_PRIM_FT4_0))[j] == expect_ft4[j])
            match_ft4++;
    }
    match_g3 = 0;
    for (j = 0; j < (int)WM_PRIM_G3_BYTES; j++) {
        if (((const u8*)PSX_ADDR(WM_PRIM_G3_BASE))[j] == expect_g3[j])
            match_g3++;
    }
    match_tile = 0;
    for (j = 0; j < (int)WM_PRIM_TILE_BYTES; j++) {
        if (((const u8*)PSX_ADDR(WM_PRIM_TILE_BASE))[j] == expect_tile[j])
            match_tile++;
    }

    unexpected = 0;
    for (j = 0; j < 8; j++) {
        if (((const u8*)PSX_ADDR(WM_PRIM_DR_TPAGE))[j] != expect_dr[j])
            unexpected++;
    }
    for (j = 0; j < 40; j++) {
        if (((const u8*)PSX_ADDR(WM_PRIM_FT4_0))[j] != expect_ft4[j])
            unexpected++;
        if (((const u8*)PSX_ADDR(WM_PRIM_FT4_1))[j] !=
            ((const u8*)PSX_ADDR(WM_PRIM_FT4_0))[j])
            unexpected++;
    }
    for (j = 0; j < (int)WM_PRIM_G3_BYTES; j++) {
        if (((const u8*)PSX_ADDR(WM_PRIM_G3_BASE))[j] != expect_g3[j])
            unexpected++;
    }
    for (j = 0; j < (int)WM_PRIM_TILE_BYTES; j++) {
        if (((const u8*)PSX_ADDR(WM_PRIM_TILE_BASE))[j] != expect_tile[j])
            unexpected++;
    }

    fprintf(stderr,
            "[worldmap-primitive-templates] helpers GetTPage(0,0,896,256)="
            "0x%04x GetClut(256,510)=0x%04x GetTPage(0,1,896,256)=0x%04x "
            "DR_mode=0x%08x\n",
            (unsigned)tpage0, (unsigned)clut, (unsigned)tpage1,
            (unsigned)dr_mode);
    fprintf(stderr,
            "[worldmap-primitive-templates] FT4[0] code=0x%02x clut=0x%04x "
            "tpage=0x%04x rgb=(%u,%u,%u) xy0=(%d,%d) xy3=(%d,%d) "
            "uv0=(%u,%u) uv3=(%u,%u)\n",
            (unsigned)WM_U8(WM_PRIM_FT4_0 + 7u),
            (unsigned)WM_U16(WM_PRIM_FT4_0 + 0x0Eu),
            (unsigned)WM_U16(WM_PRIM_FT4_0 + 0x16u),
            (unsigned)WM_U8(WM_PRIM_FT4_0 + 4u),
            (unsigned)WM_U8(WM_PRIM_FT4_0 + 5u),
            (unsigned)WM_U8(WM_PRIM_FT4_0 + 6u),
            (int)WM_S16(WM_PRIM_FT4_0 + 0x08u),
            (int)WM_S16(WM_PRIM_FT4_0 + 0x0Au),
            (int)WM_S16(WM_PRIM_FT4_0 + 0x20u),
            (int)WM_S16(WM_PRIM_FT4_0 + 0x22u),
            (unsigned)WM_U8(WM_PRIM_FT4_0 + 0x0Cu),
            (unsigned)WM_U8(WM_PRIM_FT4_0 + 0x0Du),
            (unsigned)WM_U8(WM_PRIM_FT4_0 + 0x24u),
            (unsigned)WM_U8(WM_PRIM_FT4_0 + 0x25u));
    fprintf(stderr,
            "[worldmap-primitive-templates] FT4 equal=%d G3[0] code=0x%02x "
            "TILE[0] code=0x%02x w=%u h=%u\n",
            wm_memeq(PSX_ADDR(WM_PRIM_FT4_0), PSX_ADDR(WM_PRIM_FT4_1), 40),
            (unsigned)WM_U8(WM_PRIM_G3_BASE + 7u),
            (unsigned)WM_U8(WM_PRIM_TILE_BASE + 7u),
            (unsigned)WM_U16(WM_PRIM_TILE_BASE + 12u),
            (unsigned)WM_U16(WM_PRIM_TILE_BASE + 14u));
    fprintf(stderr,
            "[worldmap-primitive-templates] hashes "
            "DR pre=0x%08x post=0x%08x exp=0x%08x | "
            "FT4 pre=0x%08x post=0x%08x exp=0x%08x | "
            "G3 pre=0x%08x post=0x%08x exp=0x%08x | "
            "TILE pre=0x%08x post=0x%08x exp=0x%08x\n",
            pre_dr_h, post_dr_h, wm_prim_fnv1a(expect_dr, 8), pre_ft4_0_h,
            post_ft4_0_h, wm_prim_fnv1a(expect_ft4, 40), pre_g3_h, post_g3_h,
            wm_prim_fnv1a(expect_g3, WM_PRIM_G3_BYTES), pre_tile_h, post_tile_h,
            wm_prim_fnv1a(expect_tile, WM_PRIM_TILE_BYTES));
    fprintf(stderr,
            "[worldmap-primitive-templates] match FT4=%d/40 G3=%d/%u "
            "TILE=%d/%u unexpected=%d ft4_1_hash=0x%08x\n",
            match_ft4, match_g3, WM_PRIM_G3_BYTES, match_tile,
            WM_PRIM_TILE_BYTES, unexpected, post_ft4_1_h);

    if (tpage0 != 0x001Eu || clut != 0x7F90u || tpage1 != 0x003Eu ||
        dr_mode != 0xE100043Eu) {
        fprintf(stderr,
                "[worldmap-primitive-templates] ERROR: helper/result mismatch "
                "tpage0=0x%04x clut=0x%04x tpage1=0x%04x mode=0x%08x\n",
                (unsigned)tpage0, (unsigned)clut, (unsigned)tpage1,
                (unsigned)dr_mode);
        return -1;
    }
    if (WM_U8(WM_PRIM_FT4_0 + 7u) != 0x2Eu ||
        WM_U16(WM_PRIM_FT4_0 + 0x0Eu) != 0x7F90u ||
        WM_U16(WM_PRIM_FT4_0 + 0x16u) != 0x001Eu) {
        fprintf(stderr,
                "[worldmap-primitive-templates] ERROR: FT4 field checks "
                "failed\n");
        return -1;
    }
    if (!wm_memeq(PSX_ADDR(WM_PRIM_FT4_0), PSX_ADDR(WM_PRIM_FT4_1), 40)) {
        fprintf(stderr,
                "[worldmap-primitive-templates] ERROR: FT4[1] != FT4[0]\n");
        return -1;
    }
    for (i = 0; i < (int)WM_PRIM_G3_COUNT; i++) {
        if (WM_U8(WM_PRIM_G3_BASE + (u32)i * WM_PRIM_G3_STRIDE + 7u) !=
            0x32u) {
            fprintf(stderr,
                    "[worldmap-primitive-templates] ERROR: G3[%d] code\n", i);
            return -1;
        }
    }
    for (i = 0; i < (int)WM_PRIM_TILE_COUNT; i++) {
        u32 base = WM_PRIM_TILE_BASE + (u32)i * WM_PRIM_TILE_STRIDE;
        if (WM_U8(base + 7u) != 0x60u || WM_U16(base + 12u) != 2u ||
            WM_U16(base + 14u) != 2u) {
            fprintf(stderr,
                    "[worldmap-primitive-templates] ERROR: TILE[%d] fields\n",
                    i);
            return -1;
        }
    }
    if (match_ft4 != 40 || match_g3 != (int)WM_PRIM_G3_BYTES ||
        match_tile != (int)WM_PRIM_TILE_BYTES || unexpected != 0 ||
        post_dr_h != wm_prim_fnv1a(expect_dr, 8) ||
        post_ft4_0_h != wm_prim_fnv1a(expect_ft4, 40) ||
        post_g3_h != wm_prim_fnv1a(expect_g3, WM_PRIM_G3_BYTES) ||
        post_tile_h != wm_prim_fnv1a(expect_tile, WM_PRIM_TILE_BYTES)) {
        fprintf(stderr,
                "[worldmap-primitive-templates] ERROR: oracle mismatch "
                "unexpected=%d\n",
                unexpected);
        return -1;
    }

    /* Gaps + prior rungs unchanged. */
    for (i = 0; i < 4; i++) {
        if (WM_U32(WM_PRIM_DR_TPAGE - 16u + (u32)i * 4u) != guard_before_dr[i]) {
            fprintf(stderr,
                    "[worldmap-primitive-templates] ERROR: pre-DR guard "
                    "changed\n");
            return -1;
        }
    }
    if (WM_U32(0x8009C5A8u) != gap_c5a8 || WM_U32(0x8009C614u) != gap_c614 ||
        WM_U32(0x8009C620u) != gap_c620 || WM_U32(0x8009C660u) != gap_c660 ||
        WM_U32(WM_FIX_C7EC) != gap_c7ec ||
        WM_U32(WM_TW_MIRROR_C88C) != gap_c88c ||
        WM_U32(WM_FLAG_C894_ABS) != gap_c894 ||
        WM_U32(0x8009D198u) != bss_d198 ||
        WM_U32(WM_POOL_BE24) != pool_snap ||
        WM_U32(WM_TMPL_DST_BE4C) != be4c0_snap ||
        WM_U32(WM_OBJ_C620) != c620_snap) {
        fprintf(stderr,
                "[worldmap-primitive-templates] ERROR: gap/prior-rung "
                "corruption\n");
        return -1;
    }

    (void)ft4_1;
    (void)pre_ft4_1_h;
    s_wm73e30_ran = 1;
    fprintf(stderr, "[worldmap-primitive-templates] exit\n");
    fprintf(stderr,
            "[worldmap-primitive-templates] cut-before-next-step "
            "retail_pc=0x%08x\n",
            WM_CUT_BEFORE_85F58);
    return 0;
}

static int s_wm85f58_ran;

/*
 * Retail 0x80085F58 — one-shot record-table relocation + CLUT id paint.
 * Non-idempotent on field_00: process-local s_wm85f58_ran is the only
 * ownership (reset only on process restart, same as prior world rungs).
 * Cut residual before 0x80072478 (jal GfxAllocateWorkBuffers).
 */
static int wm_80085F58_relocate_records_and_init_cluts(void)
{
    u32 table_psx;
    void* table_host;
    u32 c180_psx;
    u8* table;
    u8 pre_recs[WM_REC_BYTES];
    u8 pre_clut[WM_CLUT_BYTES];
    u8 expect_recs[WM_REC_BYTES];
    u8 expect_clut[WM_CLUT_BYTES];
    u32 pre_rec_h;
    u32 post_rec_h;
    u32 exp_rec_h;
    u32 pre_clut_h;
    u32 post_clut_h;
    u32 exp_clut_h;
    u32 max_rel;
    u32 first_nz_idx;
    u32 last_nz_idx;
    int nonzero;
    int relocated;
    int field04_nz;
    int i;
    int unexpected;
    int match_rec;
    int match_clut;
    u32 guard_c7ec_lo[2];
    u32 guard_c7ec_hi[2];
    u32 guard_d478_lo[2];
    u32 guard_d478_hi[2];
    u32 ft4_code_snap;
    u32 dr_mode_snap;
    u32 c88c_snap;
    u32 bss_d198_snap;
    u16 first_clut;
    u16 last_clut;

    fprintf(stderr, "[worldmap-record-clut] entry\n");

    if (s_wm85f58_ran) {
        fprintf(stderr,
                "[worldmap-record-clut] ERROR: already ran this process "
                "(non-idempotent relocation; blocked)\n");
        fprintf(stderr,
                "[worldmap-record-clut] second_call_detected=1 "
                "second_call_blocked=1\n");
        return -1;
    }
    if (!s_wm73e30_ran) {
        fprintf(stderr,
                "[worldmap-record-clut] ERROR: W14B did not run (required)\n");
        return -1;
    }

    table_psx = WM_U32(WM_FIX_C7EC);
    c180_psx = WM_U32(WM_DST_C180);
    table_host = psx_u32_to_host(table_psx);

    fprintf(stderr,
            "[worldmap-record-clut] table_psx=0x%08x table_host=%p "
            "record_count=%u c180_psx=0x%08x\n",
            table_psx, table_host, WM_REC_COUNT, c180_psx);

    if (table_psx == 0 || table_host == NULL) {
        fprintf(stderr,
                "[worldmap-record-clut] ERROR: C7EC null or unresolvable\n");
        return -1;
    }
    if (table_psx < 0x80000000u || table_psx >= 0x80200000u) {
        fprintf(stderr,
                "[worldmap-record-clut] ERROR: C7EC not KUSEG "
                "0x%08x\n",
                table_psx);
        return -1;
    }
    if ((table_psx & 3u) != 0) {
        fprintf(stderr,
                "[worldmap-record-clut] ERROR: table not 4-byte aligned\n");
        return -1;
    }
    /* Full 2048-byte table must stay inside emulated PSX RAM. */
    if (table_psx + WM_REC_BYTES > 0x80200000u ||
        table_psx + WM_REC_BYTES < table_psx) {
        fprintf(stderr,
                "[worldmap-record-clut] ERROR: table span out of PSX RAM\n");
        return -1;
    }
    /* Table should live at/after the W4C decompressed blob base. */
    if (c180_psx != 0 &&
        (c180_psx < 0x80000000u || table_psx < c180_psx)) {
        fprintf(stderr,
                "[worldmap-record-clut] ERROR: table not inside W4C blob "
                "bounds (c180=0x%08x table=0x%08x)\n",
                c180_psx, table_psx);
        return -1;
    }

    table = (u8*)table_host;

    /* Snapshots. */
    wm_memcpy(pre_recs, table, WM_REC_BYTES);
    wm_memcpy(pre_clut, PSX_ADDR(WM_CLUT_D478), WM_CLUT_BYTES);
    guard_c7ec_lo[0] = WM_U32(WM_FIX_C7EC - 8u);
    guard_c7ec_lo[1] = WM_U32(WM_FIX_C7EC - 4u);
    guard_c7ec_hi[0] = WM_U32(WM_FIX_C7EC + 4u);
    guard_c7ec_hi[1] = WM_U32(WM_FIX_C7EC + 8u);
    guard_d478_lo[0] = WM_U32(WM_CLUT_D478 - 8u);
    guard_d478_lo[1] = WM_U32(WM_CLUT_D478 - 4u);
    guard_d478_hi[0] = WM_U32(WM_CLUT_D478 + WM_CLUT_BYTES);
    guard_d478_hi[1] = WM_U32(WM_CLUT_D478 + WM_CLUT_BYTES + 4u);
    ft4_code_snap = WM_U8(WM_PRIM_FT4_0 + 7u);
    dr_mode_snap = WM_U32(WM_PRIM_DR_TPAGE + 4u);
    c88c_snap = WM_U32(WM_TW_MIRROR_C88C);
    bss_d198_snap = WM_U32(0x8009D198u);

    pre_rec_h = wm_prim_fnv1a(pre_recs, WM_REC_BYTES);
    pre_clut_h = wm_prim_fnv1a(pre_clut, WM_CLUT_BYTES);

    nonzero = 0;
    field04_nz = 0;
    max_rel = 0;
    first_nz_idx = 0xFFFFFFFFu;
    last_nz_idx = 0xFFFFFFFFu;
    for (i = 0; i < (int)WM_REC_COUNT; i++) {
        u32 f0 = *(u32*)(pre_recs + (u32)i * WM_REC_STRIDE);
        u32 f4 = *(u32*)(pre_recs + (u32)i * WM_REC_STRIDE + 4u);
        if (f0 != 0) {
            nonzero++;
            if (f0 > max_rel)
                max_rel = f0;
            if (first_nz_idx == 0xFFFFFFFFu)
                first_nz_idx = (u32)i;
            last_nz_idx = (u32)i;
            /* Diagnostic: relative should not already be KUSEG (W4C leaves
             * relatives). Fail if high bit set — not a range heuristic for
             * "skip", only preflight consistency. */
            if (f0 >= 0x80000000u) {
                fprintf(stderr,
                        "[worldmap-record-clut] ERROR: field_00[%d]=0x%08x "
                        "looks already absolute (expected relative)\n",
                        i, f0);
                return -1;
            }
            if (table_psx + f0 < table_psx ||
                table_psx + f0 >= 0x80200000u) {
                fprintf(stderr,
                        "[worldmap-record-clut] ERROR: reloc[%d] out of "
                        "PSX RAM (base=0x%08x rel=0x%08x)\n",
                        i, table_psx, f0);
                return -1;
            }
        }
        if (f4 != 0)
            field04_nz++;
    }

    fprintf(stderr,
            "[worldmap-record-clut] nonzero_records=%d field04_nz=%d "
            "max_rel=0x%08x first_nz=%u last_nz=%u table_span=0x%08x..0x%08x\n",
            nonzero, field04_nz, max_rel, first_nz_idx, last_nz_idx, table_psx,
            table_psx + WM_REC_BYTES - 1u);

    /* ---- Retail Phase A: relocate field_00 only ---- */
    relocated = 0;
    for (i = 0; i < (int)WM_REC_COUNT; i++) {
        u32 off = (u32)i * WM_REC_STRIDE;
        u32 relative = WM_U32(table_psx + off);
        if (relative != 0) {
            /* Store 32-bit KUSEG absolute; never host pointer. */
            WM_U32(table_psx + off) = table_psx + relative;
            relocated++;
        }
    }

    /* ---- Retail Phase B: CLUT table ---- */
    for (i = 0; i < (int)WM_CLUT_COUNT; i++) {
        u16 clut = GetClut(240, 496 + i);
        WM_U16(WM_CLUT_D478 + (u32)i * 2u) = clut;
    }
    first_clut = WM_U16(WM_CLUT_D478);
    last_clut = WM_U16(WM_CLUT_D478 + (WM_CLUT_COUNT - 1u) * 2u);

    /* Build expected oracle from pre-snap. */
    wm_memcpy(expect_recs, pre_recs, WM_REC_BYTES);
    for (i = 0; i < (int)WM_REC_COUNT; i++) {
        u32* f0 = (u32*)(expect_recs + (u32)i * WM_REC_STRIDE);
        if (*f0 != 0)
            *f0 = table_psx + *f0;
        /* field_04 already from pre */
    }
    for (i = 0; i < (int)WM_CLUT_COUNT; i++) {
        u16 c = GetClut(240, 496 + i);
        expect_clut[i * 2] = (u8)(c & 0xFFu);
        expect_clut[i * 2 + 1] = (u8)((c >> 8) & 0xFFu);
    }

    post_rec_h = wm_prim_fnv1a(table, WM_REC_BYTES);
    post_clut_h = wm_prim_fnv1a((const u8*)PSX_ADDR(WM_CLUT_D478), WM_CLUT_BYTES);
    exp_rec_h = wm_prim_fnv1a(expect_recs, WM_REC_BYTES);
    exp_clut_h = wm_prim_fnv1a(expect_clut, WM_CLUT_BYTES);

    match_rec = 0;
    for (i = 0; i < (int)WM_REC_BYTES; i++) {
        if (table[i] == expect_recs[i])
            match_rec++;
    }
    match_clut = 0;
    for (i = 0; i < (int)WM_CLUT_BYTES; i++) {
        if (((const u8*)PSX_ADDR(WM_CLUT_D478))[i] == expect_clut[i])
            match_clut++;
    }

    unexpected = 0;
    for (i = 0; i < (int)WM_REC_BYTES; i++) {
        if (table[i] != expect_recs[i])
            unexpected++;
    }
    for (i = 0; i < (int)WM_CLUT_BYTES; i++) {
        if (((const u8*)PSX_ADDR(WM_CLUT_D478))[i] != expect_clut[i])
            unexpected++;
    }
    /* field_04 must equal pre for every record */
    for (i = 0; i < (int)WM_REC_COUNT; i++) {
        u32 off = (u32)i * WM_REC_STRIDE + 4u;
        if (*(u32*)(table + off) != *(u32*)(pre_recs + off))
            unexpected++;
    }

    fprintf(stderr,
            "[worldmap-record-clut] relocated_records=%d clut_count=%u "
            "first_clut=0x%04x last_clut=0x%04x\n",
            relocated, WM_CLUT_COUNT, (unsigned)first_clut,
            (unsigned)last_clut);
    fprintf(stderr,
            "[worldmap-record-clut] hashes rec pre=0x%08x post=0x%08x "
            "exp=0x%08x | clut pre=0x%08x post=0x%08x exp=0x%08x\n",
            pre_rec_h, post_rec_h, exp_rec_h, pre_clut_h, post_clut_h,
            exp_clut_h);
    fprintf(stderr,
            "[worldmap-record-clut] match rec=%d/%u clut=%d/%u "
            "unexpected=%d\n",
            match_rec, WM_REC_BYTES, match_clut, WM_CLUT_BYTES, unexpected);

    /* Representative entries */
    {
        u32 idx40 = 40;
        u32 pre0 = *(u32*)(pre_recs + 0);
        u32 post0 = WM_U32(table_psx + 0);
        u32 pre40 = *(u32*)(pre_recs + idx40 * WM_REC_STRIDE);
        u32 post40 = WM_U32(table_psx + idx40 * WM_REC_STRIDE);
        u32 f4_40 = WM_U32(table_psx + idx40 * WM_REC_STRIDE + 4u);
        u32 pre255 = *(u32*)(pre_recs + 255u * WM_REC_STRIDE);
        u32 post255 = WM_U32(table_psx + 255u * WM_REC_STRIDE);
        fprintf(stderr,
                "[worldmap-record-clut] rep idx0 pre=0x%08x post=0x%08x | "
                "idx40 pre=0x%08x post=0x%08x f4=0x%08x | "
                "idx255 pre=0x%08x post=0x%08x\n",
                pre0, post0, pre40, post40, f4_40, pre255, post255);
        if (first_nz_idx != 0xFFFFFFFFu) {
            u32 p =
                *(u32*)(pre_recs + first_nz_idx * WM_REC_STRIDE);
            u32 q = WM_U32(table_psx + first_nz_idx * WM_REC_STRIDE);
            fprintf(stderr,
                    "[worldmap-record-clut] first_nz idx=%u pre=0x%08x "
                    "post=0x%08x\n",
                    first_nz_idx, p, q);
        }
    }

    if (relocated != nonzero || match_rec != (int)WM_REC_BYTES ||
        match_clut != (int)WM_CLUT_BYTES || unexpected != 0 ||
        post_rec_h != exp_rec_h || post_clut_h != exp_clut_h) {
        fprintf(stderr,
                "[worldmap-record-clut] ERROR: oracle mismatch "
                "relocated=%d nonzero=%d unexpected=%d\n",
                relocated, nonzero, unexpected);
        return -1;
    }
    if (first_clut != 0x7C0Fu || last_clut != 0x7FCFu) {
        fprintf(stderr,
                "[worldmap-record-clut] ERROR: CLUT endpoints "
                "first=0x%04x last=0x%04x\n",
                (unsigned)first_clut, (unsigned)last_clut);
        return -1;
    }

    /* Guards + prior rungs / W14B preservation. */
    if (WM_U32(WM_FIX_C7EC - 8u) != guard_c7ec_lo[0] ||
        WM_U32(WM_FIX_C7EC - 4u) != guard_c7ec_lo[1] ||
        WM_U32(WM_FIX_C7EC + 4u) != guard_c7ec_hi[0] ||
        WM_U32(WM_FIX_C7EC + 8u) != guard_c7ec_hi[1] ||
        WM_U32(WM_CLUT_D478 - 8u) != guard_d478_lo[0] ||
        WM_U32(WM_CLUT_D478 - 4u) != guard_d478_lo[1] ||
        WM_U32(WM_CLUT_D478 + WM_CLUT_BYTES) != guard_d478_hi[0] ||
        WM_U32(WM_CLUT_D478 + WM_CLUT_BYTES + 4u) != guard_d478_hi[1]) {
        fprintf(stderr,
                "[worldmap-record-clut] ERROR: neighbor guards changed\n");
        return -1;
    }
    if (WM_U32(WM_FIX_C7EC) != table_psx) {
        fprintf(stderr,
                "[worldmap-record-clut] ERROR: C7EC slot mutated\n");
        return -1;
    }
    if (WM_U8(WM_PRIM_FT4_0 + 7u) != (u8)ft4_code_snap ||
        WM_U32(WM_PRIM_DR_TPAGE + 4u) != dr_mode_snap ||
        WM_U32(WM_TW_MIRROR_C88C) != c88c_snap ||
        WM_U32(0x8009D198u) != bss_d198_snap) {
        fprintf(stderr,
                "[worldmap-record-clut] ERROR: W14B/prior-rung corrupted\n");
        return -1;
    }

    s_wm85f58_ran = 1;
    fprintf(stderr, "[worldmap-record-clut] exit\n");
    fprintf(stderr,
            "[worldmap-record-clut] cut-before-gfx-work-buffers "
            "retail_pc=0x%08x\n",
            WM_CUT_BEFORE_GFX_WORK);

    /* Optional diagnostic: prove second call is blocked without writes. */
    if (env_flag_is_one("XENO_WORLD_RECORD_CLUT_DOUBLE_TEST")) {
        u8 snap_after[WM_REC_BYTES];
        u8 snap_clut_after[WM_CLUT_BYTES];
        int rc2;
        int changed = 0;
        wm_memcpy(snap_after, table, WM_REC_BYTES);
        wm_memcpy(snap_clut_after, PSX_ADDR(WM_CLUT_D478), WM_CLUT_BYTES);
        rc2 = wm_80085F58_relocate_records_and_init_cluts();
        for (i = 0; i < (int)WM_REC_BYTES; i++) {
            if (table[i] != snap_after[i])
                changed++;
        }
        for (i = 0; i < (int)WM_CLUT_BYTES; i++) {
            if (((const u8*)PSX_ADDR(WM_CLUT_D478))[i] != snap_clut_after[i])
                changed++;
        }
        fprintf(stderr,
                "[worldmap-record-clut] double_test rc2=%d "
                "changed_bytes_after_blocked_call=%d\n",
                rc2, changed);
        if (rc2 == 0 || changed != 0) {
            fprintf(stderr,
                    "[worldmap-record-clut] ERROR: double-run guard failed\n");
            return -1;
        }
    }

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
                    "[worldmap-init] second-wave: 0x80071EF0 → poll → "
                    "0x80073530\n");
            if (world_map_second_wave_once() != 0) {
                fprintf(stderr,
                        "[worldmap-second-wave] failed; still entering "
                        "placeholder\n");
            } else if (world_object_pool_enabled()) {
                fprintf(stderr,
                        "[worldmap-init] object-pool: 0x8009766C pool init\n");
                if (wm_8009766C_object_pool() != 0) {
                    fprintf(stderr,
                            "[worldmap-object-pool] failed; still entering "
                            "placeholder\n");
                } else if (world_state_template_enabled()) {
                    fprintf(stderr,
                            "[worldmap-init] state-template: A180→BE4C "
                            "8-word copy\n");
                    if (wm_state_template_copy() != 0) {
                        fprintf(stderr,
                                "[worldmap-state-template] failed; still "
                                "entering placeholder\n");
                    } else if (world_mode_enter_state_enabled()) {
                        fprintf(stderr,
                                "[worldmap-init] mode-enter-state: ten u32 "
                                "stores\n");
                        if (PcPort_WorldMapInitializeModeEnterState() != 0) {
                            fprintf(stderr,
                                    "[worldmap-mode-enter-state] failed; still "
                                    "entering placeholder\n");
                        } else if (world_cross_products_enabled()) {
                            fprintf(stderr,
                                    "[worldmap-init] "
                                    "XENO_WORLD_CROSS_PRODUCTS=1: "
                                    "0x80098044 four OuterProduct0\n");
                            if (wm_80098044_cross_product_init() != 0) {
                                fprintf(stderr,
                                        "[worldmap-cross-products] failed; "
                                        "still entering placeholder\n");
                            } else if (world_gpu_asset_a_enabled()) {
                                fprintf(stderr,
                                        "[worldmap-init] "
                                        "XENO_WORLD_GPU_ASSET_A=1: "
                                        "0x8008440C TIM→CLUT GPU asset A\n");
                                if (wm_8008440c_gpu_asset_a() != 0) {
                                    fprintf(stderr,
                                            "[worldmap-gpu-asset-a] failed; "
                                            "still entering placeholder\n");
                                } else if (world_gpu_asset_b_enabled()) {
                                    fprintf(stderr,
                                            "[worldmap-init] "
                                            "XENO_WORLD_GPU_ASSET_B=1: "
                                            "0x800979C8 TIM→CLUT/TPage "
                                            "GPU asset B\n");
                                    if (wm_800979c8_gpu_asset_b() != 0) {
                                        fprintf(stderr,
                                                "[worldmap-gpu-asset-b] "
                                                "failed; still entering "
                                                "placeholder\n");
                                    } else if (world_object_matrix_enabled()) {
                                        fprintf(stderr,
                                                "[worldmap-init] "
                                                "XENO_WORLD_OBJECT_MATRIX=1: "
                                                "0x80084580 object/matrix "
                                                "table\n");
                                        if (wm_80084580_object_matrix() != 0) {
                                            fprintf(stderr,
                                                    "[worldmap-object-matrix] "
                                                    "failed; still entering "
                                                    "placeholder\n");
                                        } else if (world_third_wave_enabled()) {
                                            fprintf(stderr,
                                                    "[worldmap-init] "
                                                    "XENO_WORLD_THIRD_WAVE=1: "
                                                    "0x80072090 third-wave "
                                                    "submit\n");
                                            if (wm_80072090_third_wave() !=
                                                0) {
                                                fprintf(stderr,
                                                        "[worldmap-third-wave] "
                                                        "failed; still "
                                                        "entering "
                                                        "placeholder\n");
                                            } else if (
                                                world_bss_constants_enabled()) {
                                                fprintf(stderr,
                                                        "[worldmap-init] "
                                                        "XENO_WORLD_BSS_"
                                                        "CONSTANTS=1: "
                                                        "0x800736DC constant "
                                                        "paint\n");
                                                if (wm_800736DC_init_constants()
                                                    != 0) {
                                                    fprintf(stderr,
                                                            "[worldmap-bss-"
                                                            "constants] "
                                                            "failed; still "
                                                            "entering "
                                                            "placeholder\n");
                                                } else if (
                                                    world_primitive_templates_enabled()) {
                                                    fprintf(stderr,
                                                            "[worldmap-init] "
                                                            "XENO_WORLD_"
                                                            "PRIMITIVE_"
                                                            "TEMPLATES=1: "
                                                            "0x80073E30 "
                                                            "packet templates\n");
                                                    if (wm_80073E30_primitive_templates() !=
                                                        0) {
                                                        fprintf(stderr,
                                                                "[worldmap-"
                                                                "primitive-"
                                                                "templates] "
                                                                "failed; still "
                                                                "entering "
                                                                "placeholder\n");
                                                    } else if (
                                                        world_record_clut_enabled()) {
                                                        fprintf(stderr,
                                                                "[worldmap-init] "
                                                                "XENO_WORLD_"
                                                                "RECORD_CLUT_"
                                                                "INIT=1: "
                                                                "0x80085F58 "
                                                                "relocate+CLUT\n");
                                                        if (wm_80085F58_relocate_records_and_init_cluts() !=
                                                            0) {
                                                            fprintf(stderr,
                                                                    "[worldmap-"
                                                                    "record-"
                                                                    "clut] "
                                                                    "failed; "
                                                                    "still "
                                                                    "entering "
                                                                    "placeholder\n");
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        {
            u32 cut_pc = WM_MAIN_LOOP;
            if (world_record_clut_enabled())
                cut_pc = WM_CUT_BEFORE_GFX_WORK;
            else if (world_primitive_templates_enabled())
                cut_pc = WM_CUT_BEFORE_85F58;
            else if (world_bss_constants_enabled())
                cut_pc = WM_CUT_BEFORE_73E30;
            else if (world_third_wave_enabled())
                cut_pc = WM_CUT_BEFORE_736DC;
            else if (world_object_matrix_enabled())
                cut_pc = WM_CUT_BEFORE_72090;
            else if (world_gpu_asset_b_enabled())
                cut_pc = WM_CUT_BEFORE_84580;
            else if (world_gpu_asset_a_enabled())
                cut_pc = WM_CUT_BEFORE_979C8;
            else if (world_cross_products_enabled())
                cut_pc = WM_CUT_AFTER_98044;
            else if (world_mode_enter_state_enabled())
                cut_pc = WM_CUT_BEFORE_98044;
            else if (world_state_template_enabled())
                cut_pc = WM_CUT_BEFORE_CONST_BLK;
            else if (world_object_pool_enabled())
                cut_pc = WM_CUT_BEFORE_A180_COPY;
            else if (world_second_wave_enabled())
                cut_pc = WM_CUT_BEFORE_BROAD;
            fprintf(stderr,
                    "[worldmap-init] cut-before-main-loop retail_pc=0x%08x\n",
                    cut_pc);
        }
    } else {
        fprintf(stderr,
                "[worldmap-init] cut-before-loop retail_pc=0x%08x "
                "(mode-init gate off; next retail would be 0x%08x)\n",
                WM_POST_INIT_RESUME, WM_MAIN_LOOP);
    }

    /* s_wm9766c_hits only counts accidental entry into the forbidden stub
     * symbol; the real W5B body is wm_8009766C_object_pool. */
    if (s_wm712d0_hits != 0 || s_wm_drawotag_hits != 0 || s_wm9766c_hits != 0 ||
        s_wm72238_hits != 0 || s_wm7299c_hits != 0) {
        fprintf(stderr,
                "[worldmap-init] ERROR: forbidden path hit "
                "wm712d0=%d drawotag=%d f9766c_stub=%d f72238=%d f7299c=%d\n",
                s_wm712d0_hits, s_wm_drawotag_hits, s_wm9766c_hits,
                s_wm72238_hits, s_wm7299c_hits);
    }

    /* Known-safe hollow UI — W2–W5B intentionally still show NOT YET PORTED. */
    fprintf(stderr, "[worldmap-placeholder] enter\n");
    PcPort_WorldMapPlaceholderMain();
}
