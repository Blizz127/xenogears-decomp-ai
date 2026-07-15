/*
 * Port-only functional implementations of boot-path functions discovered via
 * the oracle (running xeno-port and reading the "[stub] <name>" it stops at).
 *
 * These live here, rather than in the matching tree, when the original lives in
 * a raw asm segment that hasn't been carved into a C translation unit yet.
 * They are correct-by-inspection of the disassembly, NOT byte-matched. When a
 * function later gets a proper home in src/ (matched), delete it from here.
 *
 * Compiled into the port build by pc_port/build_port.sh; defining a symbol here
 * removes it from the auto-generated stub set.
 */

/* func_800363F0  (asm/slus_006.64/26644.s):
 *     lui $at, %hi(D_800501FC); sw $a0, %lo(D_800501FC)($at); jr $ra
 *   => D_800501FC = arg0; */
int D_800501FC;
void func_800363F0(int arg0) { D_800501FC = arg0; }

/* func_80019548 (asm/slus_006.64/9D24.s): PSX boot tail that restores the
 * hardware stack/global registers before returning. Native PC state is already
 * established by the C runtime, so the port equivalent is intentionally empty. */
void func_80019548(void) {}

/* ---------------------------------------------------------------------------
 * Game-state dispatch table (data migration, Silent-Hill style).
 *
 * On PSX this table is initialized data embedding absolute RAM addresses for
 * each state's memory/heap regions. We rebuild it at runtime: function pointers
 * to the real state mains, and PSX_ADDR() for the mem/heap regions so they live
 * in the emulated PSX RAM buffer. Values extracted from the matching ELF.
 * --------------------------------------------------------------------------- */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include "common.h"
#include "main/main.h"
#include "field/actor.h"
#include "field/camera.h"
#include "field/effects.h"
#include "field/main.h"
#include "system/controller.h"
#include "system/font.h"
#include "system/kernel.h"
#include "system/math.h"
#include "system/memory.h"
#include "system/sound.h"
#include "psyq/libcd.h"
#include "psyq/libgte.h"
#include "psyq/libgpu.h"
#include <psx/inline_c.h>
#include <psx/gtereg.h>
#include "psx_memory.h"

extern long DisableEvent(long event);
extern long EnableEvent(long event);

extern void KernelMenuMain(void);
extern void KernelMenuInitialize(void);
extern void FieldMain(void);
extern void func_8001B6C4(void);
extern void MenuMain(void);
extern void ChangeGameState(unsigned int state);
extern void FontPrintf(char*, ...);
extern void FontDrawLetters(void*);
extern void SetDispMask(int mask);
extern unsigned short g_C1ButtonStatePressedOnce;
extern unsigned short g_C1ButtonStateReleased;

/* Controller button remap tables (system/controller.h declares these extern; the
 * initialisers are commented out there because the data lives in the game's
 * .data section -- which the port's stub generator zeroes since it isn't part of
 * the migrated blob). ControllerRemapButtonState() folds the face/shoulder bits
 * through these; with zeroed tables every face button (Circle/Cross/...) is
 * dropped, so KernelMenu navigation (d-pad, passed through directly) worked but
 * Circle = confirm did nothing. Provide the real values (digital pad: identity
 * mapping, masks = the CTRL_BTN_* face/shoulder bits). Real addrs: masks
 * @0x800501e8, mappings @0x80050238 in slus_006.64. */
u_char  g_ControllerButtonMappings[8] = { 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7 };
u_short g_ControllerButtonMasks[8]    = { 0x20, 0x40, 0x10, 0x80, 0x04, 0x01, 0x08, 0x02 };

/* State->overlay-archive-index table (real ROM data @0x8004EAA0, .sdata). The
 * stub generator zeroes it, so LoadGameStateOverlay(state) read archive offset 0
 * for every state -> ArchiveDecodeSize(0)=0 -> no read -> the overlay buffer was
 * never populated (LZSSDecompress then ran on stale heap garbage). Same class as
 * the button tables / D_80010000: initialised game data the port must supply.
 * Order: KernelMenu=0, Field=0xE, Battle=0x10, Worldmap=0xF, Battling=0xD,
 * Menu=0x11, Movie=0x12. */
int g_GameStateOverlayArchiveOffsets[NUM_GAME_STATE_OVERLAYS] = {
    0x0, 0xE, 0x10, 0xF, 0xD, 0x11, 0x12,
};

/* Main executable .sdata sentinels near 0x8004F308. These are initialized ROM
 * data, not BSS; leaving them as zeroed auto-stubs changes cold field state. */
s32 D_8004F308 = -1;
s32 D_8004F324 = 0xFF;
s32 D_8004F328 = 0xFF;
s32 D_8004F330 = -1;
s32 D_8004F334 = -1;
s32 D_8004F338 = -1;
s32 D_8004F340 = -1;
s32 g_GameSceneMapNum = -1;

/* Main executable .sdata @0x8005917C. Retail stores a pointer to D_80010000;
 * func_8001B6C4/shop setup test *D_8005917C for the boot/media sentinel. */
extern s32 D_80010000;
s32* D_8005917C = &D_80010000;

/* Main executable .sdata @0x8004F32C. This gates whether the field SEDS data is
 * already present in the streamed party-data buffer. Retail initializes it to
 * -1; a zeroed data stub makes func_80085890 memcpy from an absent stream. */
s32 D_8004F32C = -1;

/* Main executable .sdata @0x8004F33C. Current streamed audio bank id; retail
 * starts at -1 so the first field request is not mistaken for already loaded. */
s32 D_8004F33C = -1;

/* Main executable .sdata @0x8004FE50: model primitive dispatch descriptors.
 * The PSX table stores raw RAM addresses, including internal entry points such
 * as 0x8002E04C/0x8002E688. Native PC needs callable host pointers, so migrate
 * only descriptor entries reached by the field harness. */
typedef s32 (*ModelPrimProc)(u8* pCmd, s32 count);
/* Build-pass proc (func_8002C8CC dispatch): fn(D_80059538, D_80059528, shade);
 * must stay layout-identical to the typedef in src/slus_006.64/system/temp2.c. */
typedef s32 (*ModelPrimBuildProc)(u32 pSrc, u32 pCmd, s32 shade);

typedef struct ModelPrimDesc {
    ModelPrimProc proc[6];
    ModelPrimBuildProc buildProc;   /* host pointer; PSX addr kept in comments */
    u32 cmdStride;
    u32 packetStride;
    u32 outputStride;
} ModelPrimDesc;

extern s32 func_8002E688(u8* pCmd, s32 count);
/* Build-pass handlers ported in temp2.c; only the first arg is consumed, so the
 * (ModelPrimBuildProc) casts are ABI-safe truncated-u32 host-pointer calls. */
extern s32 func_8002CF34(s32* a0);
extern s32 func_8002CF58(u8* pSrc, u8* pCmd, s32 shade);
extern s32 func_8002D0C0(s32* a0);
extern s32 func_8002D984(u8* pSrc);
extern s32 func_8002D0E4(u8* pSrc);
static s32 ModelPrimQuadVariant0(u8* pCmd, s32 count);
static s32 ModelPrimQuadF4Variant0(u8* pCmd, s32 count);
static s32 ModelPrimQuadFT4Variant0(u8* pCmd, s32 count);
static s32 ModelPrimQuadF4Variant2(u8* pCmd, s32 count);
static s32 ModelPrimTriSmallAverageVariant0(u8* pCmd, s32 count);
static s32 ModelPrimTriSmallMinimumVariant2(u8* pCmd, s32 count);
static s32 ModelPrimTriAverageVariant0(u8* pCmd, s32 count);
static s32 ModelPrimTriMinimumVariant2(u8* pCmd, s32 count);

ModelPrimDesc D_8004FE50[15] = {
    [0x04] = {
        .proc = { ModelPrimTriSmallAverageVariant0, NULL,
                  ModelPrimTriSmallMinimumVariant2, NULL, NULL, NULL },
        .buildProc = (ModelPrimBuildProc)func_8002CF34,   /* PSX 0x8002CF34 */
        .cmdStride = 0x08,
        .packetStride = 0x04,
        .outputStride = 0x14,
    },
    [0x05] = {
        .proc = { ModelPrimTriAverageVariant0, NULL, ModelPrimTriMinimumVariant2,
                  NULL, NULL, NULL },
        .buildProc = (ModelPrimBuildProc)func_8002D984,   /* PSX 0x8002D984 */
        .cmdStride = 0x08,
        .packetStride = 0x08,
        .outputStride = 0x20,
    },
    [0x08] = {
        /* Retail D_8004FE50[0x08].proc[0] = 0x8002E254: 4-vert F4 walker (RTPT+RTPS,
         * AVSZ4, screen-overlap over all 4 SXY, writes xy0..xy3). Was wrongly wired
         * to the 3-vert Medium tri walker — dropped cmd+6 from cull/depth/packet. */
        .proc = { ModelPrimQuadF4Variant0, NULL, NULL, NULL, NULL, NULL },
        .buildProc = (ModelPrimBuildProc)func_8002CF58,   /* PSX 0x8002CF58 */
        .cmdStride = 0x08,
        .packetStride = 0x04,
        .outputStride = 0x18,
    },
    [0x0D] = {
        /* Retail table: variant 0/1 enter 0x8002E268 (AVSZ4); variant 2 enters
         * func_8002E688 (minimum-SZ depth).  They share the FT4 packet layout
         * but not depth ordering, so routing variant 0 through E688 makes room
         * surfaces overwrite each other in the wrong OT buckets. */
        .proc = { ModelPrimQuadFT4Variant0, ModelPrimQuadFT4Variant0,
                  func_8002E688, NULL, NULL, NULL },
        .buildProc = (ModelPrimBuildProc)func_8002D0E4,   /* PSX 0x8002D0E4 */
        .cmdStride = 0x08,
        .packetStride = 0x0C,
        .outputStride = 0x28,
    },
    [0x0C] = {
        .proc = { ModelPrimQuadF4Variant0, NULL, ModelPrimQuadF4Variant2, NULL, NULL, NULL },
        .buildProc = (ModelPrimBuildProc)func_8002D0C0,   /* PSX 0x8002D0C0 */
        .cmdStride = 0x08,
        .packetStride = 0x04,
        .outputStride = 0x18,
    },
};

/* Initialized renderer bounds/depth-shift globals adjacent to D_8004FE50. */
s32 D_800500F8 = 0x13F;
s32 D_800500FC = 0x00EE0000;
s32 D_80050100 = 2;
s32 D_80050104 = 1;

/* Main-exe BSS consumed by the decompiled encounter roll func_80079288
 * (src/field/main/misc4.c). D_80065ADC = the 16 per-formation encounter WEIGHTS;
 * it is populated at runtime by a not-yet-ported encounter/map-data load path,
 * so as a zero-init stub the weighted roll is degenerate (sum==0 -> no encounter
 * fires). See ACTIVE_HANDOFF.md. D_80059508/D_800594F8 = battle-transition params. */
u8 D_80065ADC[16];
u8 D_80059508;
u8 D_800594F8;

/* Battle-transition entry the encounter roll (func_80079288) calls when a
 * formation is selected. The field->battle handoff and the battle system are
 * out of scope; this no-op stub lets the roll link and run so we can validate
 * it up to the battle boundary (the named next blocker). */
extern void xeno_port_stub(const char* name);
void func_80281204(s32 formationIndex) { (void)formationIndex; xeno_port_stub("func_80281204"); }

extern u8* D_80059424;
extern u32 D_8005953C;
extern u32 D_80059568;
extern s32 D_80059578;

static u32 ModelPrimVertexIndex1(u32 word) {
    return (word >> 16) & 0xFFFF;
}

/* PSX GPU hardware rule: polygons wider than 1023 or taller than 511 in
 * projected screen space are silently rejected by the rasterizer. PsyX does
 * not implement this, so oversized overflow projections (e.g. near/behind
 * geometry from a ground-level camera) would smear giant triangles across
 * the frame. Reject them here, sibling to the screen-overlap checks. */
static int ModelPrimTriOversized(u32 xy0, u32 xy1, u32 xy2) {
    s32 x0 = (s16)(xy0 & 0xFFFF), y0 = (s16)(xy0 >> 16);
    s32 x1 = (s16)(xy1 & 0xFFFF), y1 = (s16)(xy1 >> 16);
    s32 x2 = (s16)(xy2 & 0xFFFF), y2 = (s16)(xy2 >> 16);
    s32 xmin = x0 < x1 ? x0 : x1, xmax = x0 > x1 ? x0 : x1;
    s32 ymin = y0 < y1 ? y0 : y1, ymax = y0 > y1 ? y0 : y1;
    if (x2 < xmin) xmin = x2;
    if (x2 > xmax) xmax = x2;
    if (y2 < ymin) ymin = y2;
    if (y2 > ymax) ymax = y2;
    return (xmax - xmin) > 1023 || (ymax - ymin) > 511;
}

static int ModelPrimQuadOversized(u32 xy0, u32 xy1, u32 xy2, u32 xy3) {
    if (ModelPrimTriOversized(xy0, xy1, xy2)) {
        return 1;
    }
    return ModelPrimTriOversized(xy1, xy2, xy3);
}

/* Retail screen cull (e.g. asm 8002E15C-8002E1A0): keep a prim only when at
 * least one vertex has packed SXY <u D_800500FC (y within [0, screenH-1], so
 * negative or below-screen y fails) AND at least one vertex has
 * x <u D_800500F8. An earlier port transcription inverted the first
 * comparison, culling exactly the on-screen prims. */
static int ModelPrimTriOverlapsScreen(u32 xy0, u32 xy1, u32 xy2) {
    u32 yMaxPacked = (u32)D_800500FC;
    u32 xMax = (u32)D_800500F8;

    if (!(xy0 < yMaxPacked || xy1 < yMaxPacked || xy2 < yMaxPacked)) {
        return 0;
    }
    return (((xy0 & 0xFFFF) < xMax) || ((xy1 & 0xFFFF) < xMax) ||
            ((xy2 & 0xFFFF) < xMax));
}

static int ModelPrimQuadOverlapsScreen(u32 xy0, u32 xy1, u32 xy2, u32 xy3) {
    u32 yMaxPacked = (u32)D_800500FC;
    u32 xMax = (u32)D_800500F8;

    if (!(xy0 < yMaxPacked || xy1 < yMaxPacked || xy2 < yMaxPacked || xy3 < yMaxPacked)) {
        return 0;
    }
    return (((xy0 & 0xFFFF) < xMax) || ((xy1 & 0xFFFF) < xMax) ||
            ((xy2 & 0xFFFF) < xMax) || ((xy3 & 0xFFFF) < xMax));
}

/* -------------------------------------------------------------------------
 * XENO_CULL_CAM_LOG — temporary interactive camera+cull logger (Lane A).
 * Enable with XENO_CULL_CAM_LOG=1. Writes gated lines to
 *   captures/render_diag/cullcam_<timestamp>.log
 * (override with XENO_CULL_CAM_LOG_PATH). Press SELECT to stamp a MARK line
 * at the moment of a repro. Off by default; remove after the session.
 *
 * Log field "nclip_backface" counts GTE NCLIP / NormalClip backface-winding
 * drops (OPZ < 0 on the three screen-space verts). It is NOT near-plane clip
 * (that lives in the FLAG / otz guards). Optional NCLIP_SXY / FLAG_SAMPLE
 * lines dump a few drops at the known well pose (or after MARK).
 * ------------------------------------------------------------------------- */
enum {
    CC_SEEN = 0,
    CC_EMIT,
    CC_FLAG,
    CC_OTZ,
    /* CC_NCLIP_BACKFACE: NormalClip/NCLIP OPZ<0 (screen winding), not near-Z. */
    CC_NCLIP_BACKFACE,
    CC_OVERLAP,
    CC_OVERSIZE,
    CC_GTE31,       /* FLAG bit 31 set (sign-extend path relevant) */
    CC_QF4_SEEN,    /* ModelPrimQuadF4Variant0 = retail 0x08/0x0C walker */
    CC_QF4_FLAG,
    CC_QF4_EMIT,
    CC_N
};

/* Keep old enumerator name as alias so call sites stay readable if any remain. */
#define CC_NCLIP CC_NCLIP_BACKFACE

static int s_ccOn = -1;
static u32 s_cc[CC_N];
static u32 s_ccFrame;
static FILE* s_ccFile;
static u32 s_ccNclipSamples;
static u32 s_ccFlagSamples;
static u32 s_ccMarkArmFrames; /* after SELECT, allow samples for a few summary frames */

enum { CC_NCLIP_SAMPLE_MAX = 48, CC_FLAG_SAMPLE_MAX = 48 };

static int CullCamOn(void) {
    if (s_ccOn < 0) {
        const char* e = getenv("XENO_CULL_CAM_LOG");
        s_ccOn = (e != NULL && e[0] != '\0' && e[0] != '0') ? 1 : 0;
    }
    return s_ccOn;
}

/* Known failing pose from MARK session (cullcam_20260711_184135). */
static int CullCamAtWellPose(void) {
    extern VECTOR g_CameraEye2;
    extern CameraInterpolation g_CamInterpolation;
    int ex = (int)(g_CameraEye2.vx >> 16);
    int ey = (int)(g_CameraEye2.vy >> 16);
    int ez = (int)(g_CameraEye2.vz >> 16);
    int ay = (int)g_CamInterpolation.curAngleY;
    if (ay != -1536) {
        return 0;
    }
    if (ex < -564 - 12 || ex > -564 + 12) {
        return 0;
    }
    if (ey < 1491 - 24 || ey > 1491 + 24) {
        return 0;
    }
    if (ez < 512 - 12 || ez > 512 + 12) {
        return 0;
    }
    return 1;
}

static void CullCamSampleNclipDrop(long xy0, long xy1, long xy2, long opz,
                                   const SVECTOR* v0, const SVECTOR* v1,
                                   const SVECTOR* v2, int isQf4) {
    extern VECTOR g_CameraEye2;
    extern CameraInterpolation g_CamInterpolation;
    int sx0, sy0, sx1, sy1, sx2, sy2;

    if (!CullCamOn() || s_ccFile == NULL) {
        return;
    }
    if (s_ccNclipSamples >= CC_NCLIP_SAMPLE_MAX) {
        return;
    }
    if (!CullCamAtWellPose() && s_ccMarkArmFrames == 0) {
        return;
    }

    sx0 = (int)(short)(xy0 & 0xFFFF);
    sy0 = (int)(short)((xy0 >> 16) & 0xFFFF);
    sx1 = (int)(short)(xy1 & 0xFFFF);
    sy1 = (int)(short)((xy1 >> 16) & 0xFFFF);
    sx2 = (int)(short)(xy2 & 0xFFFF);
    sy2 = (int)(short)((xy2 >> 16) & 0xFFFF);

    fprintf(s_ccFile,
            "NCLIP_SXY f=%u qf4=%d opz=%ld "
            "sxy0=%d,%d sxy1=%d,%d sxy2=%d,%d "
            "w0=%d,%d,%d w1=%d,%d,%d w2=%d,%d,%d "
            "eye2=%d,%d,%d angY=%d\n",
            s_ccFrame, isQf4, (long)opz,
            sx0, sy0, sx1, sy1, sx2, sy2,
            v0 ? (int)v0->vx : 0, v0 ? (int)v0->vy : 0, v0 ? (int)v0->vz : 0,
            v1 ? (int)v1->vx : 0, v1 ? (int)v1->vy : 0, v1 ? (int)v1->vz : 0,
            v2 ? (int)v2->vx : 0, v2 ? (int)v2->vy : 0, v2 ? (int)v2->vz : 0,
            (int)(g_CameraEye2.vx >> 16), (int)(g_CameraEye2.vy >> 16),
            (int)(g_CameraEye2.vz >> 16),
            (int)g_CamInterpolation.curAngleY);
    s_ccNclipSamples++;
    if (s_ccNclipSamples == 1 || (s_ccNclipSamples % 8) == 0) {
        fprintf(stderr, "[cull-cam] NCLIP_SXY sample %u/%u opz=%ld sxy=(%d,%d)(%d,%d)(%d,%d)\n",
                s_ccNclipSamples, (unsigned)CC_NCLIP_SAMPLE_MAX, (long)opz,
                sx0, sy0, sx1, sy1, sx2, sy2);
    }
}

/* Compact FLAG bit tags (PsyCross / nocash GTE FLAG). Bit31 = retail bltz. */
static void CullCamFormatFlagBits(u32 flag, char* out, size_t outSz) {
    static const struct { u32 bit; const char* name; } kBits[] = {
        { 31, "E" },    { 30, "MAC3" }, { 29, "MAC2" }, { 28, "MAC1" },
        { 27, "A1lo" }, { 26, "A2lo" }, { 25, "A3lo" },
        { 24, "IR1" },  { 23, "IR2" },  { 22, "IR3" },
        { 18, "SZ3" },  { 17, "DIV" },  { 16, "MAC0+" }, { 15, "MAC0-" },
        { 14, "SX" },   { 13, "SY" },   { 12, "H?" },
    };
    size_t n = 0;
    size_t i;
    out[0] = '\0';
    for (i = 0; i < sizeof(kBits) / sizeof(kBits[0]); i++) {
        if (flag & (1u << kBits[i].bit)) {
            int wrote = snprintf(out + n, outSz > n ? outSz - n : 0, "%s%s",
                                 n ? "," : "", kBits[i].name);
            if (wrote > 0) {
                n += (size_t)wrote;
            }
        }
    }
    if (n == 0) {
        snprintf(out, outSz, "-");
    }
}

static void CullCamSampleFlagDrop(long flag, long otz,
                                  long xy0, long xy1, long xy2, long xy3,
                                  const SVECTOR* v0, const SVECTOR* v1,
                                  const SVECTOR* v2, const SVECTOR* v3,
                                  const char* gteOp, int isQf4, int nVert) {
    extern VECTOR g_CameraEye2;
    extern CameraInterpolation g_CamInterpolation;
    char bits[96];
    int sx0, sy0, sx1, sy1, sx2, sy2, sx3, sy3;
    u32 f = (u32)flag;

    if (!CullCamOn() || s_ccFile == NULL) {
        return;
    }
    if (s_ccFlagSamples >= CC_FLAG_SAMPLE_MAX) {
        return;
    }
    if (!CullCamAtWellPose() && s_ccMarkArmFrames == 0) {
        return;
    }

    CullCamFormatFlagBits(f, bits, sizeof(bits));
    sx0 = (int)(short)(xy0 & 0xFFFF);
    sy0 = (int)(short)((xy0 >> 16) & 0xFFFF);
    sx1 = (int)(short)(xy1 & 0xFFFF);
    sy1 = (int)(short)((xy1 >> 16) & 0xFFFF);
    sx2 = (int)(short)(xy2 & 0xFFFF);
    sy2 = (int)(short)((xy2 >> 16) & 0xFFFF);
    sx3 = (int)(short)(xy3 & 0xFFFF);
    sy3 = (int)(short)((xy3 >> 16) & 0xFFFF);

    fprintf(s_ccFile,
            "FLAG_SAMPLE f=%u qf4=%d gte=%s flag=0x%08x bits=%s otz=%ld "
            "sxy0=%d,%d sxy1=%d,%d sxy2=%d,%d sxy3=%d,%d "
            "w0=%d,%d,%d w1=%d,%d,%d w2=%d,%d,%d w3=%d,%d,%d "
            "eye2=%d,%d,%d angY=%d\n",
            s_ccFrame, isQf4, gteOp ? gteOp : "?", f, bits, (long)otz,
            sx0, sy0, sx1, sy1, sx2, sy2,
            nVert >= 4 ? sx3 : 0, nVert >= 4 ? sy3 : 0,
            v0 ? (int)v0->vx : 0, v0 ? (int)v0->vy : 0, v0 ? (int)v0->vz : 0,
            v1 ? (int)v1->vx : 0, v1 ? (int)v1->vy : 0, v1 ? (int)v1->vz : 0,
            v2 ? (int)v2->vx : 0, v2 ? (int)v2->vy : 0, v2 ? (int)v2->vz : 0,
            (nVert >= 4 && v3) ? (int)v3->vx : 0,
            (nVert >= 4 && v3) ? (int)v3->vy : 0,
            (nVert >= 4 && v3) ? (int)v3->vz : 0,
            (int)(g_CameraEye2.vx >> 16), (int)(g_CameraEye2.vy >> 16),
            (int)(g_CameraEye2.vz >> 16),
            (int)g_CamInterpolation.curAngleY);
    s_ccFlagSamples++;
    if (s_ccFlagSamples == 1 || (s_ccFlagSamples % 8) == 0) {
        fprintf(stderr, "[cull-cam] FLAG_SAMPLE %u/%u flag=0x%08x bits=%s gte=%s otz=%ld\n",
                s_ccFlagSamples, (unsigned)CC_FLAG_SAMPLE_MAX, f, bits,
                gteOp ? gteOp : "?", (long)otz);
    }
}

static void CullCamSeen(int isQf4, long flag) {
    if (!CullCamOn()) {
        return;
    }
    s_cc[CC_SEEN]++;
    if (isQf4) {
        s_cc[CC_QF4_SEEN]++;
    }
    if ((u32)flag & 0x80000000u) {
        s_cc[CC_GTE31]++;
    }
}

static void CullCamDrop(int reason, int isQf4) {
    if (!CullCamOn()) {
        return;
    }
    s_cc[reason]++;
    if (isQf4 && reason == CC_FLAG) {
        s_cc[CC_QF4_FLAG]++;
    }
}

static void CullCamEmit(int isQf4) {
    if (!CullCamOn()) {
        return;
    }
    s_cc[CC_EMIT]++;
    if (isQf4) {
        s_cc[CC_QF4_EMIT]++;
    }
}

void PcPort_CullCamLogOnVsync(void) {
    extern VECTOR g_CameraEye2;
    extern VECTOR g_CameraAt2;
    extern VECTOR g_CameraEye;
    extern VECTOR g_CameraAt;
    extern CameraInterpolation g_CamInterpolation;
    extern FieldActor* volatile g_FieldActors;
    extern s32 g_PlayerActorIndex;
    extern u_short g_C1ButtonStateReleased;
    extern FieldScene g_Scene;
    static u16 s_prevSelect;
    u16 selectEdge;
    s32 feiX = 0, feiY = 0, feiZ = 0;
    u32 feiStatus = 0, feiFlags4 = 0;
    int mark;

    if (!CullCamOn()) {
        return;
    }

    if (s_ccFile == NULL) {
        const char* path = getenv("XENO_CULL_CAM_LOG_PATH");
        char autoPath[256];
        if (path == NULL || path[0] == '\0') {
            time_t now = time(NULL);
            struct tm* t = localtime(&now);
            snprintf(autoPath, sizeof(autoPath),
                     "/home/blizz/Projects/xenogears-decomp/captures/render_diag/"
                     "cullcam_%04d%02d%02d_%02d%02d%02d.log",
                     t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
                     t->tm_hour, t->tm_min, t->tm_sec);
            path = autoPath;
            mkdir("/home/blizz/Projects/xenogears-decomp/captures", 0755);
            mkdir("/home/blizz/Projects/xenogears-decomp/captures/render_diag", 0755);
        }
        s_ccFile = fopen(path, "w");
        if (s_ccFile == NULL) {
            fprintf(stderr, "[cull-cam] FAILED to open log path '%s'\n", path);
            s_ccOn = 0;
            return;
        }
        setvbuf(s_ccFile, NULL, _IOLBF, 0);
        fprintf(stderr, "[cull-cam] logging to %s (SELECT=MARK)\n", path);
        fprintf(s_ccFile,
                "# cull-cam log: f eye2XYZ(>>16) eyeXYZ(>>16) at2XYZ(>>16) angY camRotXYZ "
                "feiXYZ(>>16) feiStatus feiFlags4 otEmit | "
                "seen emit flag otz nclip_backface(=NCLIP/OPZ winding, NOT near-Z) "
                "overlap oversize gte31 qf4seen qf4flag qf4emit\n"
                "# NCLIP_SXY lines: sample of backface drops at well pose "
                "(eye2~-564,1491,512 angY=-1536) or after MARK — SXY + OPZ + model-space verts\n"
                "# FLAG_SAMPLE lines: sample of flag<0 drops at same pose — raw FLAG bits + "
                "preceding RotTransPers* (RTPT3/RTPT4) + SXY + model verts\n"
                "# CAMZ lines: camera-zone / clamp gate — unk48, bit4000 (skip eye-Y "
                "mesh clamp), sceneDIP/SCRZ/scale, camMode, meshY at eye2 XZ, enc\n");
    }

    selectEdge = (u16)(g_C1ButtonStateReleased & CTRL_BTN_SELECT);
    mark = (selectEdge != 0 && s_prevSelect == 0);
    s_prevSelect = selectEdge;

    /* Field busy-waits on Vsync(-1) and also hits Vsync(1) pre-draw; logging every
     * call produced multi-GB logs of all-zero cull rows. Only emit when the user
     * MARKs or the instrumented walkers actually saw prims this frame. */
    if (!mark && s_cc[CC_SEEN] == 0 && s_cc[CC_QF4_SEEN] == 0) {
        if (s_ccMarkArmFrames > 0) {
            s_ccMarkArmFrames--;
        }
        return;
    }

    if (g_FieldActors != NULL && g_PlayerActorIndex >= 0) {
        FieldActor* fa = &g_FieldActors[g_PlayerActorIndex];
        feiStatus = (u32)(u16)fa->status;
        if (fa->pActorData != 0) {
            u8* ad = (u8*)(uintptr_t)fa->pActorData;
            feiFlags4 = *(u32*)(ad + 0x04);
            feiX = *(s32*)(ad + 0x20) >> 16;
            feiY = *(s32*)(ad + 0x24) >> 16;
            feiZ = *(s32*)(ad + 0x28) >> 16;
        }
    }

    if (mark) {
        fprintf(s_ccFile, "MARK f=%u <<< user SELECT — repro moment\n", s_ccFrame);
        fprintf(stderr, "[cull-cam] MARK f=%u\n", s_ccFrame);
        s_ccMarkArmFrames = 120; /* allow NCLIP_SXY for a short window after MARK */
    }
    if (s_ccMarkArmFrames > 0) {
        s_ccMarkArmFrames--;
    }

    /* CAMZ: once per well-pose (or MARK) — clamp-gate + sceneDIP/SCRZ + mesh probe.
     * Does NOT modify camera state; re-probes the same path as func_80073230. */
    {
        static int s_camzLogged;
        int atPose = CullCamAtWellPose();
        /* Also sample the first few activity frames so spawn/approach state is
         * visible even before the well pose (unk48 / DIP / meshY). */
        if ((atPose || mark || s_camzLogged < 4) && s_camzLogged < 12) {
            extern s16 g_FieldCameraMode;
            extern s16 D_800AFB54;
            extern FieldControl g_FieldControl;
            extern s16 func_8007B1C4(s16 a0, s16 a1, s32 a2, void* a3, void* a4);
            s16 meshPoint[4];
            s32 meshNormal[4];
            s32 unk48 = g_Scene.unk48;
            int bit4000 = (unk48 & 0x4000) != 0;
            s16 dip = *(s16*)((u8*)&g_Scene + 0x6C);
            s32 scrz = *(s32*)((u8*)&g_Scene + 0x68);
            s16 scale = *(s16*)((u8*)&g_Scene + 0x6E);
            s16 sceneAng56 = *(s16*)((u8*)&g_Scene + 0x56);
            int e2x = (int)(g_CameraEye2.vx >> 16);
            int e2y = (int)(g_CameraEye2.vy >> 16);
            int e2z = (int)(g_CameraEye2.vz >> 16);
            int meshY = 0x7FFF;
            int meshRet = -1;
            int wouldSnap = 0;
            meshPoint[0] = meshPoint[1] = meshPoint[2] = meshPoint[3] = 0;
            meshNormal[0] = meshNormal[1] = meshNormal[2] = meshNormal[3] = 0;
            meshRet = (int)func_8007B1C4((s16)e2x, (s16)e2z, D_800AFB54 - 1,
                                         meshPoint, meshNormal);
            meshY = (int)meshPoint[1];
            wouldSnap = (!bit4000 && meshY < e2y) ? 1 : 0;
            fprintf(s_ccFile,
                    "CAMZ f=%u unk48=0x%08x bit4000=%d camMode=%d AFB54=%d "
                    "DIP=%d SCRZ=%d scale=%d scene56=%d "
                    "eye2=%d,%d,%d meshY=%d meshRet=%d wouldSnap=%d "
                    "eye=%d,%d,%d at2=%d,%d,%d angY=%d enc=%d\n",
                    s_ccFrame, (unsigned)unk48, bit4000, (int)g_FieldCameraMode,
                    (int)D_800AFB54, (int)dip, (int)scrz, (int)scale,
                    (int)sceneAng56, e2x, e2y, e2z, meshY, meshRet, wouldSnap,
                    (int)(g_CameraEye.vx >> 16), (int)(g_CameraEye.vy >> 16),
                    (int)(g_CameraEye.vz >> 16),
                    (int)(g_CameraAt2.vx >> 16), (int)(g_CameraAt2.vy >> 16),
                    (int)(g_CameraAt2.vz >> 16),
                    (int)g_CamInterpolation.curAngleY,
                    (int)g_FieldControl.isRandomEncountersEnabled);
            fprintf(stderr,
                    "[cull-cam] CAMZ bit4000=%d DIP=%d SCRZ=%d meshY=%d eye2y=%d "
                    "wouldSnap=%d\n",
                    bit4000, (int)dip, (int)scrz, meshY, e2y, wouldSnap);
            s_camzLogged++;
        }
    }

    fprintf(s_ccFile,
            "f=%u eye2=%d,%d,%d eye=%d,%d,%d at2=%d,%d,%d angY=%d camRot=%d,%d,%d "
            "fei=%d,%d,%d st=%04x f4=%08x otEmit=%d | "
            "seen=%u emit=%u flag=%u otz=%u nclip_backface=%u overlap=%u oversize=%u "
            "gte31=%u qf4seen=%u qf4flag=%u qf4emit=%u\n",
            s_ccFrame,
            (int)(g_CameraEye2.vx >> 16), (int)(g_CameraEye2.vy >> 16),
            (int)(g_CameraEye2.vz >> 16),
            (int)(g_CameraEye.vx >> 16), (int)(g_CameraEye.vy >> 16),
            (int)(g_CameraEye.vz >> 16),
            (int)(g_CameraAt2.vx >> 16), (int)(g_CameraAt2.vy >> 16),
            (int)(g_CameraAt2.vz >> 16),
            (int)g_CamInterpolation.curAngleY,
            (int)g_Scene.camRotation.vx, (int)g_Scene.camRotation.vy,
            (int)g_Scene.camRotation.vz,
            feiX, feiY, feiZ, (unsigned)feiStatus, feiFlags4,
            (int)D_80059578,
            s_cc[CC_SEEN], s_cc[CC_EMIT], s_cc[CC_FLAG], s_cc[CC_OTZ],
            s_cc[CC_NCLIP_BACKFACE], s_cc[CC_OVERLAP], s_cc[CC_OVERSIZE],
            s_cc[CC_GTE31], s_cc[CC_QF4_SEEN], s_cc[CC_QF4_FLAG],
            s_cc[CC_QF4_EMIT]);

    {
        int i;
        for (i = 0; i < CC_N; i++) {
            s_cc[i] = 0;
        }
    }
    s_ccFrame++;
}

/* Retail 0x8002E038 -> shared 0x8002E058: compact three-vertex packet
 * walker using AVSZ3 for OT ordering. */
static s32 ModelPrimTriSmallAverageVariant0(u8* pCmd, s32 count) {
    const s32 packetStep = 0x14;
    const u32 tagLen = 0x04000000;
    u8* vertexBase = (u8*)(uintptr_t)D_8005953C;
    u8* out = D_80059424 - packetStep;
    u32* ot = (u32*)(uintptr_t)D_80059568;
    s32 emitted = D_80059578;

    while (count != 0) {
        u32 cmd = *(u32*)pCmd;
        SVECTOR* v0 = (SVECTOR*)(vertexBase + ((cmd & 0xFFFF) << 3));
        SVECTOR* v1 = (SVECTOR*)(vertexBase + (ModelPrimVertexIndex1(cmd) << 3));
        SVECTOR* v2 = (SVECTOR*)(vertexBase + (*(u16*)(pCmd + 0x04) << 3));
        long xy0 = 0;
        long xy1 = 0;
        long xy2 = 0;
        long p = 0;
        long sz3 = 0;
        long flag = 0;
        long nclipOpz;
        u16 averageZ;
        s32 otIndex;
        u32 oldTag;

        count--;
        pCmd += 8;
        out += packetStep;

        sz3 = RotTransPers3(v0, v1, v2, &xy0, &xy1, &xy2, &p, &flag);
        CullCamSeen(0, flag);
        if (flag < 0) {
            CullCamSampleFlagDrop(flag, sz3, xy0, xy1, xy2, 0, v0, v1, v2, NULL,
                                  "RTPT3", 0, 3);
            CullCamDrop(CC_FLAG, 0);
            continue;
        }

        nclipOpz = NormalClip(xy0, xy1, xy2);
        if (!ModelPrimTriOverlapsScreen((u32)xy0, (u32)xy1, (u32)xy2)) {
            CullCamDrop(CC_OVERLAP, 0);
            continue;
        }

        gte_avsz3();
        if (nclipOpz <= 0) {
            CullCamDrop(CC_NCLIP_BACKFACE, 0);
            CullCamSampleNclipDrop(xy0, xy1, xy2, nclipOpz, v0, v1, v2, 0);
            continue;
        }

        *(u32*)(out + 0x08) = (u32)xy0;
        *(u32*)(out + 0x0C) = (u32)xy1;
        *(u32*)(out + 0x10) = (u32)xy2;

        averageZ = (u16)C2_OTZ;
        emitted++;
        if (averageZ == 0) {
            CullCamDrop(CC_OTZ, 0);
            continue;
        }

        otIndex = (s32)averageZ >> D_80050100;
        oldTag = ot[otIndex];
        ot[otIndex] = (u32)(uintptr_t)out & 0x00FFFFFF;
        *(u32*)(out + 0x00) = (oldTag & 0x00FFFFFF) | tagLen;
        CullCamEmit(0);
    }

    D_80059578 = emitted;
    D_80059424 = out + packetStep;
    return 1;
}

/* Retail 0x8002E470 -> shared 0x8002E490: compact three-vertex packet
 * walker using the nearest of SZ1/SZ2/SZ3 for OT ordering. */
static s32 ModelPrimTriSmallMinimumVariant2(u8* pCmd, s32 count) {
    const s32 packetStep = 0x14;
    const u32 tagLen = 0x04000000;
    u8* vertexBase = (u8*)(uintptr_t)D_8005953C;
    u8* out = D_80059424 - packetStep;
    u32* ot = (u32*)(uintptr_t)D_80059568;
    s32 emitted = D_80059578;

    while (count != 0) {
        u32 cmd = *(u32*)pCmd;
        SVECTOR* v0 = (SVECTOR*)(vertexBase + ((cmd & 0xFFFF) << 3));
        SVECTOR* v1 = (SVECTOR*)(vertexBase + (ModelPrimVertexIndex1(cmd) << 3));
        SVECTOR* v2 = (SVECTOR*)(vertexBase + (*(u16*)(pCmd + 0x04) << 3));
        long xy0 = 0;
        long xy1 = 0;
        long xy2 = 0;
        long p = 0;
        long sz3Result = 0;
        long flag = 0;
        long nclipOpz;
        u16 sz1;
        u16 sz2;
        u16 sz3;
        u16 minSz;
        s32 otIndex;
        u32 oldTag;

        count--;
        pCmd += 8;
        out += packetStep;

        sz3Result = RotTransPers3(v0, v1, v2, &xy0, &xy1, &xy2, &p, &flag);
        CullCamSeen(0, flag);
        if (flag < 0) {
            CullCamSampleFlagDrop(flag, sz3Result, xy0, xy1, xy2, 0,
                                  v0, v1, v2, NULL, "RTPT3", 0, 3);
            CullCamDrop(CC_FLAG, 0);
            continue;
        }

        nclipOpz = NormalClip(xy0, xy1, xy2);
        if (!ModelPrimTriOverlapsScreen((u32)xy0, (u32)xy1, (u32)xy2)) {
            CullCamDrop(CC_OVERLAP, 0);
            continue;
        }

        *(u32*)(out + 0x08) = (u32)xy0;
        if (nclipOpz <= 0) {
            CullCamDrop(CC_NCLIP_BACKFACE, 0);
            CullCamSampleNclipDrop(xy0, xy1, xy2, nclipOpz, v0, v1, v2, 0);
            continue;
        }
        *(u32*)(out + 0x0C) = (u32)xy1;
        *(u32*)(out + 0x10) = (u32)xy2;

        sz1 = (u16)C2_SZ1;
        sz2 = (u16)C2_SZ2;
        sz3 = (u16)C2_SZ3;
        minSz = sz2;
        if (sz1 < minSz) minSz = sz1;
        if (sz3 < minSz) minSz = sz3;

        emitted++;
        if (minSz == 0) {
            CullCamDrop(CC_OTZ, 0);
            continue;
        }

        otIndex = (s32)minSz >> (D_80050100 + 2);
        oldTag = ot[otIndex];
        ot[otIndex] = (u32)(uintptr_t)out & 0x00FFFFFF;
        *(u32*)(out + 0x00) = (oldTag & 0x00FFFFFF) | tagLen;
        CullCamEmit(0);
    }

    D_80059578 = emitted;
    D_80059424 = out + packetStep;
    return 1;
}

static s32 ModelPrimQuadVariant0(u8* pCmd, s32 count) {
    const s32 packetStep = 0x34;
    const u32 tagLen = 0x0C000000;
    u8* vertexBase = (u8*)(uintptr_t)D_8005953C;
    u8* out = D_80059424 - packetStep;
    u32* ot = (u32*)(uintptr_t)D_80059568;
    s32 emitted = D_80059578;

    while (count != 0) {
        u32 cmd = *(u32*)pCmd;
        SVECTOR* v0 = (SVECTOR*)(vertexBase + ((cmd & 0xFFFF) << 3));
        SVECTOR* v1 = (SVECTOR*)(vertexBase + (ModelPrimVertexIndex1(cmd) << 3));
        SVECTOR* v2 = (SVECTOR*)(vertexBase + (*(u16*)(pCmd + 0x04) << 3));
        SVECTOR* v3 = (SVECTOR*)(vertexBase + (*(u16*)(pCmd + 0x06) << 3));
        long xy0 = 0;
        long xy1 = 0;
        long xy2 = 0;
        long xy3 = 0;
        long p = 0;
        long otz = 0;
        long flag = 0;

        count--;
        pCmd += 8;
        out += packetStep;

        otz = RotTransPers4(v0, v1, v2, v3, &xy0, &xy1, &xy2, &xy3, &p, &flag);
        CullCamSeen(0, flag);
        if (flag < 0 || otz <= 0) {
            if (flag < 0) {
                CullCamSampleFlagDrop(flag, otz, xy0, xy1, xy2, xy3, v0, v1, v2, v3,
                                      "RTPT4", 0, 4);
            }
            CullCamDrop(flag < 0 ? CC_FLAG : CC_OTZ, 0);
            continue;
        }
        {
            long nclipOpz = NormalClip(xy0, xy1, xy2);
            if (nclipOpz < 0) {
                CullCamDrop(CC_NCLIP_BACKFACE, 0);
                CullCamSampleNclipDrop(xy0, xy1, xy2, nclipOpz, v0, v1, v2, 0);
                continue;
            }
        }
        if (!ModelPrimQuadOverlapsScreen((u32)xy0, (u32)xy1, (u32)xy2, (u32)xy3)) {
            CullCamDrop(CC_OVERLAP, 0);
            continue;
        }
        if (ModelPrimQuadOversized((u32)xy0, (u32)xy1, (u32)xy2, (u32)xy3)) {
            CullCamDrop(CC_OVERSIZE, 0);
            continue;
        }

        {
            /* Depth-bucket from OTZ (=SZ3>>2, the RotTransPers* return), matching
             * the original asm's SZ3 >> (D_80050100 + 2). The p out-param is the
             * GTE depth-cue (IR0), NOT a depth -- it is 0 with DQ regs unset. */
            s32 otIndex = (s32)otz >> D_80050100;
            u32 oldTag;
            /* XENO_PC_PORT: retail func_8002E010 skips a background poly only when
             * raw OTZ==0 (already guarded above by `otz <= 0`) and writes ot[otIndex]
             * even for otIndex==0. `<= 0` here additionally DROPPED the nearest depth
             * bucket (otz 1..3 => otIndex 0), removing the geometry closest to the eye
             * -- visible only when the camera is jammed against geometry (e.g. the
             * Lahan well pose: near polys vanish, distant ones survive => scattered
             * geometry in black). otz>0 is guaranteed above, so `< 0` never fires and
             * matches retail's unconditional ot[otIndex] write. */
            if (otIndex < 0) {
                continue;
            }
            oldTag = ot[otIndex];
            ot[otIndex] = (u32)(uintptr_t)out & 0x00FFFFFF;
            *(u32*)(out + 0x00) = (oldTag & 0x00FFFFFF) | tagLen;
            *(u32*)(out + 0x08) = (u32)xy0;
            *(u32*)(out + 0x14) = (u32)xy1;
            *(u32*)(out + 0x20) = (u32)xy2;
            *(u32*)(out + 0x2C) = (u32)xy3;
            emitted++;
            CullCamEmit(0);
        }
    }

    D_80059578 = emitted;
    D_80059424 = out + packetStep;
    return 1;
}

/* Retail 0x8002E254 -> shared 0x8002E274: compact POLY_F4 walker used by
 * 0x08/0 and 0x0C/0.  Retail transforms three vertices with RTPT, performs
 * NCLIP, transforms the fourth with RTPS, then uses AVSZ4 for OT ordering. */
static s32 ModelPrimQuadF4Variant0(u8* pCmd, s32 count) {
    const s32 packetStep = 0x18;
    const u32 tagLen = 0x05000000;
    u8* vertexBase = (u8*)(uintptr_t)D_8005953C;
    u8* out = D_80059424 - packetStep;
    u32* ot = (u32*)(uintptr_t)D_80059568;
    s32 emitted = D_80059578;

    while (count != 0) {
        u32 cmd = *(u32*)pCmd;
        SVECTOR* v0 = (SVECTOR*)(vertexBase + ((cmd & 0xFFFF) << 3));
        SVECTOR* v1 = (SVECTOR*)(vertexBase + (ModelPrimVertexIndex1(cmd) << 3));
        SVECTOR* v2 = (SVECTOR*)(vertexBase + (*(u16*)(pCmd + 0x04) << 3));
        SVECTOR* v3 = (SVECTOR*)(vertexBase + (*(u16*)(pCmd + 0x06) << 3));
        long xy0 = 0;
        long xy1 = 0;
        long xy2 = 0;
        long xy3 = 0;
        long rtptFlag = 0;
        long rtpsFlag = 0;
        long nclipOpz = 0;
        u16 averageZ;
        s32 otIndex;
        u32 oldTag;

        count--;
        pCmd += 8;
        out += packetStep;

        gte_ldv3(v0, v1, v2);
        gte_rtpt();
        gte_stflg(&rtptFlag);
        gte_stsxy3(&xy0, &xy1, &xy2);

        /* Retail issues NCLIP before testing RTPT's saved FLAG. */
        gte_nclip();
        gte_stopz(&nclipOpz);
        rtptFlag = (long)(s32)(u32)rtptFlag;
        nclipOpz = (long)(s32)(u32)nclipOpz;
        if (rtptFlag < 0) {
            CullCamSeen(1, rtptFlag);
            CullCamSampleFlagDrop(rtptFlag, (u16)C2_SZ3 >> 2,
                                  xy0, xy1, xy2, 0, v0, v1, v2, v3,
                                  "RTPT4", 1, 4);
            CullCamDrop(CC_FLAG, 1);
            continue;
        }
        if (nclipOpz <= 0) {
            CullCamSeen(1, rtptFlag);
            CullCamDrop(CC_NCLIP_BACKFACE, 1);
            CullCamSampleNclipDrop(xy0, xy1, xy2, nclipOpz, v0, v1, v2, 1);
            continue;
        }

        gte_ldv0(v3);
        gte_rtps();
        gte_stflg(&rtpsFlag);
        gte_stsxy(&xy3);
        /* AVSZ4 is retail's branch-delay instruction and therefore executes
         * even when RTPS's FLAG rejects the primitive. */
        gte_avsz4();
        rtpsFlag = (long)(s32)(u32)rtpsFlag;
        CullCamSeen(1, rtpsFlag);
        if (rtpsFlag < 0) {
            CullCamSampleFlagDrop(rtpsFlag, (u16)C2_SZ3 >> 2,
                                  xy0, xy1, xy2, xy3, v0, v1, v2, v3,
                                  "RTPT4", 1, 4);
            CullCamDrop(CC_FLAG, 1);
            continue;
        }

        if (!ModelPrimQuadOverlapsScreen((u32)xy0, (u32)xy1, (u32)xy2, (u32)xy3)) {
            CullCamDrop(CC_OVERLAP, 1);
            continue;
        }

        averageZ = (u16)C2_OTZ;
        emitted++;
        if (averageZ == 0) {
            CullCamDrop(CC_OTZ, 1);
            continue;
        }

        otIndex = (s32)averageZ >> D_80050100;
        oldTag = ot[otIndex];
        ot[otIndex] = (u32)(uintptr_t)out & 0x00FFFFFF;
        *(u32*)(out + 0x00) = (oldTag & 0x00FFFFFF) | tagLen;
        *(u32*)(out + 0x08) = (u32)xy0;
        *(u32*)(out + 0x0C) = (u32)xy1;
        *(u32*)(out + 0x10) = (u32)xy2;
        *(u32*)(out + 0x14) = (u32)xy3;
        CullCamEmit(1);
    }

    D_80059578 = emitted;
    D_80059424 = out + packetStep;
    return 1;
}

/* Retail 0x8002E268 -> shared 0x8002E274: POLY_FT4 variant-0/1 walker.
 * NCLIP occurs between the first-three RTPT and fourth-vertex RTPS; AVSZ4 is
 * issued in RTPS's FLAG-branch delay slot. */
static s32 ModelPrimQuadFT4Variant0(u8* pCmd, s32 count) {
    const s32 packetStep = 0x28;
    const u32 tagLen = 0x09000000;
    u8* vertexBase = (u8*)(uintptr_t)D_8005953C;
    u8* out = D_80059424 - packetStep;
    u32* ot = (u32*)(uintptr_t)D_80059568;
    s32 emitted = D_80059578;

    while (count != 0) {
        u32 cmd = *(u32*)pCmd;
        SVECTOR* v0 = (SVECTOR*)(vertexBase + ((cmd & 0xFFFF) << 3));
        SVECTOR* v1 = (SVECTOR*)(vertexBase + (ModelPrimVertexIndex1(cmd) << 3));
        SVECTOR* v2 = (SVECTOR*)(vertexBase + (*(u16*)(pCmd + 0x04) << 3));
        SVECTOR* v3 = (SVECTOR*)(vertexBase + (*(u16*)(pCmd + 0x06) << 3));
        long xy0 = 0;
        long xy1 = 0;
        long xy2 = 0;
        long xy3 = 0;
        long rtptFlag = 0;
        long rtpsFlag = 0;
        long nclipOpz = 0;
        u16 averageZ;
        s32 otIndex;
        u32 oldTag;

        count--;
        pCmd += 8;
        out += packetStep;

        gte_ldv3(v0, v1, v2);
        gte_rtpt();
        gte_stflg(&rtptFlag);
        gte_stsxy3(&xy0, &xy1, &xy2);

        gte_nclip();
        gte_stopz(&nclipOpz);
        rtptFlag = (long)(s32)(u32)rtptFlag;
        nclipOpz = (long)(s32)(u32)nclipOpz;
        if (rtptFlag < 0) {
            continue;
        }
        if (nclipOpz <= 0) {
            continue;
        }

        gte_ldv0(v3);
        gte_rtps();
        gte_stflg(&rtpsFlag);
        gte_stsxy(&xy3);
        gte_avsz4();
        rtpsFlag = (long)(s32)(u32)rtpsFlag;
        if (rtpsFlag < 0) {
            continue;
        }

        if (!ModelPrimQuadOverlapsScreen((u32)xy0, (u32)xy1, (u32)xy2, (u32)xy3)) {
            continue;
        }

        averageZ = (u16)C2_OTZ;
        emitted++;
        if (averageZ == 0) {
            continue;
        }

        otIndex = (s32)averageZ >> D_80050100;
        oldTag = ot[otIndex];
        ot[otIndex] = (u32)(uintptr_t)out & 0x00FFFFFF;
        *(u32*)(out + 0x00) = (oldTag & 0x00FFFFFF) | tagLen;
        *(u32*)(out + 0x08) = (u32)xy0;
        *(u32*)(out + 0x10) = (u32)xy1;
        *(u32*)(out + 0x18) = (u32)xy2;
        *(u32*)(out + 0x20) = (u32)xy3;
    }

    D_80059578 = emitted;
    D_80059424 = out + packetStep;
    return 1;
}

/* POLY_F4 variant-2 walker, retail entry 0x8002E674.  This shares retail's
 * four-vertex/min-SZ path with func_8002E688 but uses the F4 packet layout:
 * tag length 5, 0x18-byte packets, and packed SXY words at +8/+C/+10/+14. */
static s32 ModelPrimQuadF4Variant2(u8* pCmd, s32 count) {
    const s32 packetStep = 0x18;
    const u32 tagLen = 0x05000000;
    u8* vertexBase = (u8*)(uintptr_t)D_8005953C;
    u8* out = D_80059424 - packetStep;
    u32* ot = (u32*)(uintptr_t)D_80059568;
    s32 emitted = D_80059578;

    while (count != 0) {
        u32 cmd = *(u32*)pCmd;
        SVECTOR* v0 = (SVECTOR*)(vertexBase + ((cmd & 0xFFFF) << 3));
        SVECTOR* v1 = (SVECTOR*)(vertexBase + (ModelPrimVertexIndex1(cmd) << 3));
        SVECTOR* v2 = (SVECTOR*)(vertexBase + (*(u16*)(pCmd + 0x04) << 3));
        SVECTOR* v3 = (SVECTOR*)(vertexBase + (*(u16*)(pCmd + 0x06) << 3));
        long xy0 = 0;
        long xy1 = 0;
        long xy2 = 0;
        long xy3 = 0;
        long p = 0;
        long otz = 0;
        long flag = 0;

        count--;
        pCmd += 8;
        out += packetStep;

        otz = RotTransPers4(v0, v1, v2, v3, &xy0, &xy1, &xy2, &xy3, &p, &flag);
        CullCamSeen(1, flag);
        if (flag < 0) {
            CullCamSampleFlagDrop(flag, otz, xy0, xy1, xy2, xy3, v0, v1, v2, v3,
                                  "RTPT4", 1, 4);
            CullCamDrop(CC_FLAG, 1);
            continue;
        }
        {
            long nclipOpz = NormalClip(xy0, xy1, xy2);
            if (nclipOpz <= 0) {
                CullCamDrop(CC_NCLIP_BACKFACE, 1);
                CullCamSampleNclipDrop(xy0, xy1, xy2, nclipOpz, v0, v1, v2, 1);
                continue;
            }
        }
        if (!ModelPrimQuadOverlapsScreen((u32)xy0, (u32)xy1, (u32)xy2, (u32)xy3)) {
            CullCamDrop(CC_OVERLAP, 1);
            continue;
        }

        /* Retail stores all four projected coordinates before testing the SZ
         * FIFO.  A zero depth leaves an updated but unlinked packet. */
        *(u32*)(out + 0x08) = (u32)xy0;
        *(u32*)(out + 0x0C) = (u32)xy1;
        *(u32*)(out + 0x10) = (u32)xy2;
        *(u32*)(out + 0x14) = (u32)xy3;

        {
            u16 sz0 = (u16)C2_SZ0;
            u16 sz1 = (u16)C2_SZ1;
            u16 sz2 = (u16)C2_SZ2;
            u16 sz3 = (u16)C2_SZ3;
            u16 minSz;
            s32 otIndex;
            u32 oldTag;

            /* Retail 0x8002E82C-0x8002E894 rejects zero SZ values and derives
             * this variant's bucket from the nearest of all four vertices. */
            if (sz0 == 0 || sz1 == 0 || sz2 == 0 || sz3 == 0) {
                CullCamDrop(CC_OTZ, 1);
                continue;
            }
            minSz = sz0;
            if (sz1 < minSz) minSz = sz1;
            if (sz2 < minSz) minSz = sz2;
            if (sz3 < minSz) minSz = sz3;
            otIndex = (s32)minSz >> D_80050100;

            oldTag = ot[otIndex];
            ot[otIndex] = (u32)(uintptr_t)out & 0x00FFFFFF;
            *(u32*)(out + 0x00) = (oldTag & 0x00FFFFFF) | tagLen;
            emitted++;
            CullCamEmit(1);
        }
    }

    D_80059578 = emitted;
    D_80059424 = out + packetStep;
    return 1;
}

/* Retail 0x8002E04C -> shared 0x8002E058: three-vertex packet walker using
 * AVSZ3 for OT ordering.  This is deliberately separate from proc 2: retail's
 * 0x8002E484 path orders the same packet format by the minimum SZ instead. */
static s32 ModelPrimTriAverageVariant0(u8* pCmd, s32 count) {
    const s32 packetStep = 0x20;
    const u32 tagLen = 0x07000000;
    u8* vertexBase = (u8*)(uintptr_t)D_8005953C;
    u8* out = D_80059424 - packetStep;
    u32* ot = (u32*)(uintptr_t)D_80059568;
    s32 emitted = D_80059578;

    while (count != 0) {
        u32 cmd = *(u32*)pCmd;
        SVECTOR* v0 = (SVECTOR*)(vertexBase + ((cmd & 0xFFFF) << 3));
        SVECTOR* v1 = (SVECTOR*)(vertexBase + (ModelPrimVertexIndex1(cmd) << 3));
        SVECTOR* v2 = (SVECTOR*)(vertexBase + (*(u16*)(pCmd + 0x04) << 3));
        long xy0 = 0;
        long xy1 = 0;
        long xy2 = 0;
        long p = 0;
        long sz3 = 0;
        long flag = 0;
        long nclipOpz;
        u16 averageZ;
        s32 otIndex;
        u32 oldTag;

        count--;
        pCmd += 8;
        out += packetStep;

        sz3 = RotTransPers3(v0, v1, v2, &xy0, &xy1, &xy2, &p, &flag);
        CullCamSeen(0, flag);
        if (flag < 0) {
            CullCamSampleFlagDrop(flag, sz3, xy0, xy1, xy2, 0, v0, v1, v2, NULL,
                                  "RTPT3", 0, 3);
            CullCamDrop(CC_FLAG, 0);
            continue;
        }

        /* Retail issues NCLIP before evaluating the screen-overlap result. */
        nclipOpz = NormalClip(xy0, xy1, xy2);
        if (!ModelPrimTriOverlapsScreen((u32)xy0, (u32)xy1, (u32)xy2)) {
            CullCamDrop(CC_OVERLAP, 0);
            continue;
        }

        /* Retail starts AVSZ3 before testing OPZ, so preserve that GTE state
         * transition even for a front-face rejection. */
        gte_avsz3();
        if (nclipOpz <= 0) {
            CullCamDrop(CC_NCLIP_BACKFACE, 0);
            CullCamSampleNclipDrop(xy0, xy1, xy2, nclipOpz, v0, v1, v2, 0);
            continue;
        }

        *(u32*)(out + 0x08) = (u32)xy0;
        *(u32*)(out + 0x10) = (u32)xy1;
        *(u32*)(out + 0x18) = (u32)xy2;

        averageZ = (u16)C2_OTZ;
        emitted++;
        if (averageZ == 0) {
            CullCamDrop(CC_OTZ, 0);
            continue;
        }

        /* C2_OTZ is the AVSZ3 result in the port just as it is on retail. */
        otIndex = (s32)averageZ >> D_80050100;
        oldTag = ot[otIndex];
        ot[otIndex] = (u32)(uintptr_t)out & 0x00FFFFFF;
        *(u32*)(out + 0x00) = (oldTag & 0x00FFFFFF) | tagLen;
        CullCamEmit(0);
    }

    D_80059578 = emitted;
    D_80059424 = out + packetStep;
    return 1;
}

/* Retail 0x8002E484 -> shared 0x8002E490: three-vertex packet walker using
 * the nearest of SZ1/SZ2/SZ3 for OT ordering. */
static s32 ModelPrimTriMinimumVariant2(u8* pCmd, s32 count) {
    const s32 packetStep = 0x20;
    const u32 tagLen = 0x07000000;
    u8* vertexBase = (u8*)(uintptr_t)D_8005953C;
    u8* out = D_80059424 - packetStep;
    u32* ot = (u32*)(uintptr_t)D_80059568;
    s32 emitted = D_80059578;

    while (count != 0) {
        u32 cmd = *(u32*)pCmd;
        SVECTOR* v0 = (SVECTOR*)(vertexBase + ((cmd & 0xFFFF) << 3));
        SVECTOR* v1 = (SVECTOR*)(vertexBase + (ModelPrimVertexIndex1(cmd) << 3));
        SVECTOR* v2 = (SVECTOR*)(vertexBase + (*(u16*)(pCmd + 0x04) << 3));
        long xy0 = 0;
        long xy1 = 0;
        long xy2 = 0;
        long p = 0;
        long sz3Result = 0;
        long flag = 0;
        long nclipOpz;
        u16 sz1;
        u16 sz2;
        u16 sz3;
        u16 minSz;
        s32 otIndex;
        u32 oldTag;

        count--;
        pCmd += 8;
        out += packetStep;

        sz3Result = RotTransPers3(v0, v1, v2, &xy0, &xy1, &xy2, &p, &flag);
        CullCamSeen(0, flag);
        if (flag < 0) {
            CullCamSampleFlagDrop(flag, sz3Result, xy0, xy1, xy2, 0,
                                  v0, v1, v2, NULL, "RTPT3", 0, 3);
            CullCamDrop(CC_FLAG, 0);
            continue;
        }

        nclipOpz = NormalClip(xy0, xy1, xy2);
        if (!ModelPrimTriOverlapsScreen((u32)xy0, (u32)xy1, (u32)xy2)) {
            CullCamDrop(CC_OVERLAP, 0);
            continue;
        }

        /* The branch-delay store at retail 0x8002E5EC updates xy0 even when
         * OPZ rejects the primitive; xy1/xy2 are written only on the live path. */
        *(u32*)(out + 0x08) = (u32)xy0;
        if (nclipOpz <= 0) {
            CullCamDrop(CC_NCLIP_BACKFACE, 0);
            CullCamSampleNclipDrop(xy0, xy1, xy2, nclipOpz, v0, v1, v2, 0);
            continue;
        }
        *(u32*)(out + 0x10) = (u32)xy1;
        *(u32*)(out + 0x18) = (u32)xy2;

        sz1 = (u16)C2_SZ1;
        sz2 = (u16)C2_SZ2;
        sz3 = (u16)C2_SZ3;
        minSz = sz2;
        if (sz1 < minSz) minSz = sz1;
        if (sz3 < minSz) minSz = sz3;

        emitted++;
        if (minSz == 0) {
            CullCamDrop(CC_OTZ, 0);
            continue;
        }

        /* Retail 0x8002E4F0 adds two to the configured shift before using the
         * raw SZ FIFO.  RotTransPers3's return and AVSZ3's OTZ are already
         * quarter-scale, but C2_SZ1..3 are not, so the +2 is required here. */
        otIndex = (s32)minSz >> (D_80050100 + 2);
        oldTag = ot[otIndex];
        ot[otIndex] = (u32)(uintptr_t)out & 0x00FFFFFF;
        *(u32*)(out + 0x00) = (oldTag & 0x00FFFFFF) | tagLen;
        CullCamEmit(0);
    }

    D_80059578 = emitted;
    D_80059424 = out + packetStep;
    return 1;
}

extern s32 D_8004F2F4;
extern s32 D_8004F2F8;
extern s32 D_8004F2FC;
extern s32 D_8004F300;
extern s32 D_8004F304;
extern s32 D_8004F310;
extern s32 D_8004F314;
extern s32 D_8004F318;
extern s32 D_8004F31C;
extern s32 D_8004F320;
extern s32 D_8004F344;
extern s32 D_8004F348;
extern s32 D_8004F350;
extern s32 D_8004F354;
extern s32 D_8004F358;
extern s32 D_8004F35C;
extern s32 g_GameHasLoadedWDS;
extern s32 D_8004F364;
extern s32 D_8004F368;
extern s32 D_8004F36C;
extern s32 D_8004F370;
extern s32 D_8004F378;
extern s32 D_8004F37C;
extern s32 D_8004F380;
extern s16 D_8004F384;
extern u8 D_8005942C;
extern u8 D_800594D0;
extern s32 D_8005A444[3];
extern s32 D_8006F990[3];
extern s32 D_80062518;
extern s32 D_8006251C;
extern s32 D_80062524;
extern s32 g_GamePartySkinsInitialized;
extern s32 g_GamePartyMembers[3];
extern s32 g_GamePartyMemberSkins[3];
extern s32 g_PartyIsWaitingForStreamData;

/* func_8001AADC (asm/slus_006.64/system/temp3.s): original boot global-state
 * initializer called by func_80019578 before MainLoop. The native port enters
 * MainLoop directly, so keep this small reset here until temp3.c is buildable. */
void func_8001AADC(void)
{
    s32 i;

    D_8004F364 = 1;
    D_8004F328 = 0xFF;
    D_8004F324 = 0xFF;
    D_8004F2FC = 0;
    D_8004F36C = 0;
    D_8004F2F8 = 0;
    D_8004F31C = 0;
    D_8004F320 = 0;
    D_8004F314 = 0;
    D_8004F310 = 0;
    g_GamePartySkinsInitialized = 0;
    D_8004F370 = 0;
    D_8004F35C = 0;
    g_GameHasLoadedWDS = 0;
    g_PartyIsWaitingForStreamData = 0;
    D_8004F358 = 0;
    D_8004F354 = 0;
    D_8004F350 = 0;
    D_8004F2F4 = 0;
    D_8004F344 = 0;
    D_8004F348 = 0;
    D_8004F304 = 0;
    D_8004F368 = 0;
    D_8004F300 = 0;
    D_8004F380 = 0;
    D_8004F37C = 0;
    D_8004F378 = 0;
    D_8005942C = 0;
    D_800594D0 = 0;
    D_8004F384 = 0;
    D_8004F318 = 0;
    D_8004F334 = -1;
    g_GameSceneMapNum = -1;
    D_8004F33C = -1;
    D_8004F338 = -1;
    D_8004F330 = -1;
    D_8004F32C = -1;
    D_8004F340 = -1;
    D_8004F308 = -1;

    for (i = 0; i < 3; i++) {
        g_GamePartyMemberSkins[i] = 0;
        D_8006F990[i] = 0;
        D_8005A444[i] = 0;
        g_GamePartyMembers[i] = 0;
    }

    D_80062524 = 0;
    D_8006251C = 0;
    D_80062518 = 0;
}

/* Fixed destination the per-state overlay decompresses to (ROM: .word D_8006FAF0
 * @0x80018084 -> PSX 0x8006FAF0, the low scratch region below each state's
 * relocated heap). Zeroed stub -> NULL -> LZSSDecompress(overlay, NULL) segfaults.
 * Set to the real emulated-RAM address in PcPort_HeapBoot (needs g_PsxRam base). */
void* g_MainGameStateOverlayBuffer;

/* ClearMemory(pStart, pEnd): zero a word range. Real one is asm/BIOS (bypassed);
 * main_loop.c calls it to wipe a game state's memory region before entering it,
 * so the stub (no-op) left uninitialised state -> crash on state change. */
void ClearMemory(u32* pStart, u32* pEnd)
{
    while (pStart < pEnd)
        *pStart++ = 0;
}

MainGameState g_MainGameStates[7];

/*
 * Port-side stand-in for the retail shipping boot path.
 *
 * Retail (Noah/Ghidra): shipping builds enter movie mode (state 6) with the
 * intro STR queued, then land on the title/new-game menu overlay. Neither the
 * movie decoder nor the save/title menu overlay is ported yet, so this harness
 * approximates the player-facing sequence with KernelMenu-style font UI:
 *   title screen -> intro movie skip -> new-game menu -> Field (state 1).
 *
 * Field-test / smoke runs keep KernelMenuMain as boot state 0 (see
 * PcPort_InitGameStates) so XENO_KERNEL_SEL continues to drive the debug menu.
 */
enum {
    PORT_BOOT_TITLE = 0,
    PORT_BOOT_MOVIE = 1,
    PORT_BOOT_MENU  = 2
};

static int PcPort_BootPhaseFrames(int phase)
{
    const char* e = getenv("XENO_BOOT_DELAY");
    int base = (e && *e) ? atoi(e) : 60;
    if (base < 1)
        base = 1;
    if (phase == PORT_BOOT_MOVIE)
        return base / 2 > 0 ? base / 2 : 1; /* brief skip card */
    return base;
}

void PcPort_BootMain(void)
{
    static char sTitle[] =
        "\n\n\n\n"
        "          XENOGEARS\n\n"
        "       PRESS START BUTTON\n";
    static char sMovie[] =
        "\n\n\n\n"
        "         INTRO MOVIE\n\n"
        "   (skipped -- decoder not ported)\n\n"
        "       PRESS START TO CONTINUE\n";
    static char sMenu[] =
        "\n\n\n\n"
        "          NEW GAME\n"
        "          CONTINUE\n\n"
        "   (Continue requires save system)\n";
    void* pOtag;
    int phase = PORT_BOOT_TITLE;
    int phaseFrames = 0;
    int menuChoice = 0; /* 0 = New Game, 1 = Continue (unavailable) */
    int running = 1;
    int advance;

    KernelMenuInitialize();
    SetDispMask(1);
    g_KernelMenuIsRunning = 0;
    D_800592C8 = 0;
    printf("[xeno-port][boot] title screen\n");

    while (running) {
        D_800592C4++;
        D_800592C8 = D_800592C4 & 1;
        g_KernelMenuCurRenderEnvironment =
            &g_KernelMenuRenderEnvironments[D_800592C8];
        pOtag = &g_KernelMenuCurRenderEnvironment->ot;
        TermPrim(pOtag);
        FontDrawLetters(pOtag);

        advance = 0;
        phaseFrames++;

        switch (phase) {
        case PORT_BOOT_TITLE:
            FontPrintf(sTitle);
            if ((g_C1ButtonStatePressedOnce & CTRL_BTN_START) ||
                (g_C1ButtonStateReleased & CTRL_BTN_CIRCLE) ||
                phaseFrames >= PcPort_BootPhaseFrames(phase))
                advance = 1;
            break;

        case PORT_BOOT_MOVIE:
            FontPrintf(sMovie);
            if ((g_C1ButtonStatePressedOnce &
                 (CTRL_BTN_START | CTRL_BTN_CROSS)) ||
                (g_C1ButtonStateReleased & CTRL_BTN_CIRCLE) ||
                phaseFrames >= PcPort_BootPhaseFrames(phase))
                advance = 1;
            break;

        case PORT_BOOT_MENU:
            FontPrintf(sMenu);
            if (g_C1ButtonStatePressedOnce & CTRL_BTN_UP) {
                menuChoice = 0;
            } else if (g_C1ButtonStatePressedOnce & CTRL_BTN_DOWN) {
                menuChoice = 1;
            }
            setXY0Fast(&g_KernelMenuCurRenderEnvironment->cursor,
                       0x38, menuChoice * 8 + 0x38);
            setXY1Fast(&g_KernelMenuCurRenderEnvironment->cursor,
                       0x3F, menuChoice * 8 + 0x3C);
            setXY2Fast(&g_KernelMenuCurRenderEnvironment->cursor,
                       0x38, menuChoice * 8 + 0x40);
            AddPrim(pOtag, &g_KernelMenuCurRenderEnvironment->cursor);

            /* Confirm = Circle/Start/Cross. Continue is unavailable (no saves),
             * so refuse it by snapping back to New Game instead of no-op'ing
             * (which looked like a freeze with the cursor on CONTINUE). */
            if ((g_C1ButtonStateReleased & CTRL_BTN_CIRCLE) ||
                (g_C1ButtonStatePressedOnce &
                 (CTRL_BTN_START | CTRL_BTN_CROSS)) ||
                phaseFrames >= PcPort_BootPhaseFrames(phase)) {
                if (menuChoice != 0 &&
                    phaseFrames < PcPort_BootPhaseFrames(phase)) {
                    printf("[xeno-port][boot] Continue unavailable "
                           "(save system not ported) — use New Game\n");
                    menuChoice = 0;
                } else {
                    printf("[xeno-port][boot] New Game -> Field\n");
                    ChangeGameState(1);
                    running = 0;
                }
            }
            break;
        }

        if (running && advance) {
            phaseFrames = 0;
            if (phase == PORT_BOOT_TITLE) {
                phase = PORT_BOOT_MOVIE;
                printf("[xeno-port][boot] intro movie (skipped)\n");
            } else if (phase == PORT_BOOT_MOVIE) {
                phase = PORT_BOOT_MENU;
                menuChoice = 0;
                printf("[xeno-port][boot] new game menu\n");
            }
        }

        DrawSync(0);
        Vsync(0);
        PutDrawEnv(&g_KernelMenuCurRenderEnvironment->drawEnv);
        PutDispEnv(&g_KernelMenuCurRenderEnvironment->dispEnv);
        DrawOTag(pOtag);
    }

    DrawSync(0);
    MainLoop(0);
}

void PcPort_InitGameStates(void)
{
    const char* fieldTest = getenv("XENO_FIELD_TEST");
    int useKernelMenu = (fieldTest && fieldTest[0] == '1');

    /* [idx] = { pFnMain, pMemStart, pHeapStart, hasOverlay } */
    /* Normal boot: title/movie-skip/new-game stand-in. Field-test/smokes keep
     * the debug KernelMenu so XENO_KERNEL_SEL can still drive Field/etc. */
    g_MainGameStates[0].pFnMain    = useKernelMenu ? KernelMenuMain
                                                   : PcPort_BootMain;
    g_MainGameStates[0].pMemStart  = PSX_ADDR(0x000592b8);
    g_MainGameStates[0].pHeapStart = PSX_ADDR(0x0006faec);
    g_MainGameStates[0].hasOverlay = 0;

    if (useKernelMenu)
        printf("[xeno-port][boot] field-test: KernelMenu boot state\n");
    else
        printf("[xeno-port][boot] normal: title/menu boot state\n");

    g_MainGameStates[1].pFnMain    = FieldMain;
    g_MainGameStates[1].pMemStart  = PSX_ADDR(0x000af5e4);
    g_MainGameStates[1].pHeapStart = PSX_ADDR(0x000c426c);
    g_MainGameStates[1].hasOverlay = 1;

    g_MainGameStates[2].pFnMain    = func_8001B6C4;
    g_MainGameStates[2].pMemStart  = PSX_ADDR(0x000c3a6c);
    g_MainGameStates[2].pHeapStart = PSX_ADDR(0x000d39f0);
    g_MainGameStates[2].hasOverlay = 1;

    /* states 3, 4, 6 are field/battle overlay mains not yet symbol-named;
     * left NULL until the oracle reaches them. */

    g_MainGameStates[5].pFnMain    = MenuMain;
    g_MainGameStates[5].pMemStart  = PSX_ADDR(0x000592b8);
    g_MainGameStates[5].pHeapStart = PSX_ADDR(0x0006faec);
    g_MainGameStates[5].hasOverlay = 0;
}

/* ---------------------------------------------------------------------------
 * Heap bootstrap.
 *
 * On hardware the boot routine func_80019578 (asm/.../main/main) calls
 * HeapInit(func_8002DFE0(), 0x801FC000) once before falling into MainLoop, where
 * func_8002DFE0 returns &D_8006FAF0. MainLoop never re-inits the heap; it only
 * HeapRelocate()s within it, which walks g_Heap and crashes if it was never set
 * up. The oracle enters MainLoop() directly (the asm `start`/boot is not yet C),
 * so the port performs that one-time HeapInit here, translated into emulated RAM.
 * --------------------------------------------------------------------------- */
void PcPort_HeapBoot(void)
{
    HeapInit(PSX_ADDR(0x8006FAF0), PSX_ADDR(0x801FC000));
    /* Overlay decompress target (see the extern def above): 0x8006FAF0 in
     * emulated RAM, resolvable only now that g_PsxRam exists. */
    g_MainGameStateOverlayBuffer = PSX_ADDR(0x8006FAF0);
}

int SoundFileComputeChecksum(SoundFile* pSoundFile)
{
    int nResult = 0;
    int* pCurrent = (int*)pSoundFile;
    unsigned int nCount = (pSoundFile->unk8 + 3) / 4;

    do {
        nResult += *pCurrent++;
    } while (--nCount);

    return nResult;
}

int SoundValidateFile(SoundFile* pSoundFile, u32 magicBytes, unsigned short targetValue)
{
    unsigned char bIsError;

    if (pSoundFile->magic != magicBytes) {
        return SOUND_ERR_INVALID_SIGNATURE;
    }

    if (SoundFileComputeChecksum(pSoundFile) == 0) {
        bIsError = (pSoundFile->unkC != targetValue);
        return bIsError * SOUND_ERR_UNK_0X4;
    }

    return SOUND_ERR_INVALID_CHECKSUM;
}

void SoundAddSedsEntry(SoundFile* pSoundFile)
{
    SoundFile* pEntry;
    short nSedsStatus;
    SoundFile** pList;

    if (!(g_SoundControlFlags & 0x80)) {
        for (pEntry = g_SoundSedsLinkedList; pEntry != NULL; pEntry = pEntry->pNext) {
            if (pSoundFile->sedId == pEntry->sedId) {
                SoundHandleError(SOUND_ERR_ENTRY_ALREADY_EXISTS);
                return;
            }
        }
    }

    nSedsStatus = SoundValidateFile(pSoundFile, FILE_SIGNATURE('s','e','d','s'), 0x101);
    if (nSedsStatus != SOUND_STATUS_OK) {
        SoundHandleError(nSedsStatus);
        return;
    }

    DisableEvent(g_unk_SoundEvent);
    pList = &g_SoundSedsLinkedList;
    while (*pList != NULL) {
        pList = &((*pList)->pNext);
    }
    *pList = pSoundFile;
    pSoundFile->pNext = NULL;
    EnableEvent(g_unk_SoundEvent);
}

int func_8003BDFC(int flags)
{
    if (flags & 0x10) {
        while (g_SoundControlFlags & 0x10) {
        }
    }

    if (g_SoundControlFlags & 0x10) {
        return g_SoundTransferQueue[g_SoundTransferQueueReadIndex].commandType;
    }
    return 0;
}

extern void* g_FieldScriptMemory;

void FieldScriptMemoryWriteU16(int index, int value)
{
    ((u16*)&g_FieldScriptMemory)[index >> 1] = value;
}

extern s32 g_GamePartySkinsInitialized;
extern s32 D_800ADBFC;
extern FieldActor* volatile g_FieldActors;
extern s32 g_PlayerActorIndex;
extern ActorData* g_FieldScriptVMCurActor;
extern s32 ArchiveSetIndex(s32 directoryIndex, s32 entryIndex);
extern s32 ArchiveDecodeAlignedSize(u32 entryIndex);
extern s32 ArchiveReadFileToBuffer(s32 index, void* pBuffer, u32 arg2, u32 flags);
extern s32 ArchiveCdDataSync(s32 mode);
extern void SpriteSetSpecialAnimFile(SpriteData* pSpriteData, void* pAnimFile);
extern void func_800A3C8C(void);
extern void FieldDistortionInitialize(s32 arg0);
extern void FieldScriptWritePartyMemberIDs(void);
extern void func_80072254(s32 actorIndex);

/* ---------------------------------------------------------------------------
 * LZSS decompressor.
 *   LZSSHeapDecompress @ 0x80032E88, LZSSDecompress @ 0x80032EB4
 *   (asm/slus_006.64/util/lzss.s -- pure asm, no C translation unit yet).
 *
 * Functional re-implementation, correct-by-inspection of the disassembly (not
 * byte-matched). Stream format: the first 4 bytes of the source are the
 * decompressed size. The remainder is a token stream of one flag byte (8 bits
 * consumed LSB-first) followed by that many tokens:
 *   bit 0 -> literal: copy the next source byte verbatim.
 *   bit 1 -> back-reference from two bytes b0,b1:
 *              offset = b0 | ((b1 & 0xF) << 8)   (12-bit window)
 *              length = (b1 >> 4) + 3
 *            copy `length` bytes from (dst - offset).
 * The original checks the output-end only at each 8-token group boundary, so a
 * group is always processed in full; this mirrors that exactly.
 * --------------------------------------------------------------------------- */
void* LZSSDecompress(void* pSrc, void* pDst)
{
    u8* src = (u8*)pSrc;
    u8* dst = (u8*)pDst;
    u8* dstStart = dst;
    u32 nDecompressedSize = *(u32*)src;
    u8* dstEnd;

    /* XENO_PC_PORT stopgap. A per-state overlay whose archive entry the port's
     * disc/overlay path can't yet resolve (e.g. the field overlay: the archive
     * directory the overlay index lives in isn't set up, so ArchiveDecodeSize
     * returns 0) makes LoadGameStateOverlay return an UNWRITTEN buffer -- so this
     * size header is heap garbage (megabytes) and decompressing it walks straight
     * off emulated RAM. The overlay is redundant in the port anyway (field/menu
     * code is statically linked), so until the overlay archive read is implemented,
     * treat an implausible size (larger than all of emulated RAM) as an empty /
     * no-op overlay rather than crashing. Real streams (splash ~4KB, overlays
     * <=~345KB) are far under this bound. */
    if (nDecompressedSize > (u32)PSX_RAM_SIZE) {
        fprintf(stderr, "[xeno-port] LZSSDecompress: implausible size 0x%x "
                        "(overlay not resolved?) -> skipping\n", nDecompressedSize);
        return pDst;
    }

    dstEnd = dst + nDecompressedSize;
    src += 4;

    while (dst != dstEnd) {
        u8 flags = *src++;
        int n;
        for (n = 0; n < 8; n++, flags >>= 1) {
            if (flags & 1) {
                u32 b0 = *src++;
                u32 b1 = *src++;
                u32 offset = b0 | ((b1 & 0xF) << 8);
                u32 length = (b1 >> 4) + 3;
                u8* ref = dst - offset;
                u32 i;
                for (i = 0; i < length; i++) {
                    *dst++ = *ref++;
                }
            } else {
                *dst++ = *src++;
            }
        }
    }
    return dstStart;
}

void* LZSSHeapDecompress(void* pCompressed, int flags)
{
    u32 size = *(u32*)pCompressed;
    void* pDst = HeapAlloc(size, (u_int)flags);
    if (pDst == NULL) {
        return NULL;
    }
    return LZSSDecompress(pCompressed, pDst);
}

/* ---------------------------------------------------------------------------
 * func_80036718 (asm/slus_006.64/26644.s:655) -- the font's printf-style text
 * formatter that FontPrintf delegates to. The original is a 422-line custom
 * printf that, per output character, calls func_800366F0 -> (*D_80050594) =
 * FontAddLetterPrimitive (the glyph queuer, already decompiled in font.c).
 *
 * Functional re-implementation (not byte-matched): format with vsprintf, then
 * queue each character's glyph directly via FontAddLetterPrimitive. FontPrintf
 * does va_start then passes the va_list as the 3rd argument, so it arrives here
 * as `args` (x86-64 passes a va_list by reference, matching the variadic decl).
 * --------------------------------------------------------------------------- */
#include <stdarg.h>
#include <stdio.h>
extern void FontAddLetterPrimitive(int letter);

void func_80036718(int mode, char* format, va_list args)
{
    char buf[512];
    char* p;
    (void)mode;
    vsprintf(buf, format, args);
    for (p = buf; *p != '\0'; p++) {
        FontAddLetterPrimitive((unsigned char)*p);
    }
}

/* ---------------------------------------------------------------------------
 * func_800317E0 / func_80031804 (asm/slus_006.64/.../temp2 -- byte-identical):
 * a fast addPrim that threads a primitive onto the head of an ordering table
 * with tag length 3:  old = *ot;  *ot = addr(prim) & 0xFFFFFF;  *prim = old | (3<<24);
 * FontDrawLetters' per-glyph loop calls this to link each queued letter's SPRT
 * into the OT that DrawOTag walks. PSX OT links are 24-bit addresses; the
 * emulated RAM is linked below 16 MiB (-no-pie) so the mask is lossless.
 * --------------------------------------------------------------------------- */
void func_800317E0(void* ot, void* prim)
{
    u32 old = *(u32*)ot;
    *(u32*)ot = (u32)(uintptr_t)prim & 0xFFFFFF;
    *(u32*)prim = old | 0x03000000;
}

void func_80031804(void* ot, void* prim)
{
    u32 old = *(u32*)ot;
    *(u32*)ot = (u32)(uintptr_t)prim & 0xFFFFFF;
    *(u32*)prim = old | 0x03000000;
}

/* ---------------------------------------------------------------------------
 * Anim-script opcode 0xFC image-upload chain: func_8001FBE4 (0xFC handler in
 * animation_scripts.c) -> func_8001FB30 -> func_8002DDE4 -> LoadImage.
 *
 * D_800592E4/E8/EA (sbss 0x800592E4-EA; "800592E4 -> EA is bss local" in
 * rendering.c) carry the image-blob pointer and VRAM base x/y between the
 * handler and func_8001FB30, which the matching build keeps as INCLUDE_ASM.
 * --------------------------------------------------------------------------- */
u32 D_800592E4;
s16 D_800592E8;
s16 D_800592EA;

/* func_8002DDE4 (asm/slus_006.64/nonmatchings/system/temp2/func_8002DDE4.s):
 * multi-block VRAM uploader. pImageData = { s32 count; u32 skipped[count];
 * blocks... }, each block = { u32 magic; u16 baseX, baseY, offsX, offsY;
 * u16 w, h; u16 pixels[w*h] }. magic 0x1100 = pixel data (positioned by
 * texMode), 0x1101 = CLUT (positioned by clutMode); any other magic returns 1.
 * mode 1: caller xy + block offs; mode 2: block base + caller xy + block offs;
 * other: block base + block offs. Each block is LoadImage'd into VRAM.
 * clutX/clutY are read as u16 (retail lhu), the rest stay 32-bit signed. */
s32 func_8002DDE4(void* pImageData, s32 texMode, s32 texX, s32 texY,
                  s32 clutMode, s32 clutX, s32 clutY)
{
    s32 count = *(s32*)pImageData;
    u8* pBlock = (u8*)pImageData + count * 4 + 4;
    s16 texMode16 = (s16)texMode;
    s16 clutMode16 = (s16)clutMode;
    u16 cx = (u16)clutX;
    u16 cy = (u16)clutY;
    RECT rect;
    s32 i;

    for (i = 0; i < count; i++) {
        u32 magic = *(u32*)pBlock;
        pBlock += 4;

        if (magic == 0x1100) {
            if (texMode16 == 1) {
                rect.x = texX + *(u16*)(pBlock + 4);
                rect.y = texY + *(u16*)(pBlock + 6);
            } else if (texMode16 == 2) {
                rect.x = *(u16*)(pBlock + 0) + texX + *(u16*)(pBlock + 4);
                rect.y = *(u16*)(pBlock + 2) + texY + *(u16*)(pBlock + 6);
            } else {
                rect.x = *(u16*)(pBlock + 0) + *(u16*)(pBlock + 4);
                rect.y = *(u16*)(pBlock + 2) + *(u16*)(pBlock + 6);
            }
        } else if (magic == 0x1101) {
            if (clutMode16 == 1) {
                rect.x = cx + *(u16*)(pBlock + 4);
                rect.y = cy + *(u16*)(pBlock + 6);
            } else if (clutMode16 == 2) {
                rect.x = *(u16*)(pBlock + 0) + cx + *(u16*)(pBlock + 4);
                rect.y = *(u16*)(pBlock + 2) + cy + *(u16*)(pBlock + 6);
            } else {
                rect.x = *(u16*)(pBlock + 0) + *(u16*)(pBlock + 4);
                rect.y = *(u16*)(pBlock + 2) + *(u16*)(pBlock + 6);
            }
        } else {
            return 1;
        }

        pBlock += 8;
        rect.w = *(u16*)pBlock;
        pBlock += 2;
        rect.h = *(u16*)pBlock;
        pBlock += 2;
        LoadImage(&rect, (u_long*)pBlock);
        pBlock += (s16)rect.w * (s16)rect.h * 2;
    }
    return 0;
}

/* func_8001FB30 (asm/slus_006.64/nonmatchings/system/rendering/func_8001FB30.s):
 * trampoline that retail runs on a heap-allocated 0x2000-byte stack (sp is
 * repointed to scratch+0x1EFC around the call). The native stack needs no
 * switch; the alloc/free pair is kept so heap state stays retail-exact.
 * Forwards the D_800592E4/E8/EA handoff into func_8002DDE4 with the CLUT
 * args zeroed (CLUT blocks then use their raw embedded coordinates). */
void func_8001FB30(void)
{
    void* scratch = HeapAlloc(0x2000, 1);
    func_8002DDE4((void*)(uintptr_t)D_800592E4, 1, D_800592E8, D_800592EA,
                  0, 0, 0);
    HeapFree(scratch);
}

/* ---------------------------------------------------------------------------
 * Anim-script opcode 0xE0 child-sprite spawn chain.
 *
 * func_8001FBE4's 0xE0 case calls func_80023B84(parentSprite, script, pkg),
 * which allocates a heap AnimTask { WorkListEntry task1(timer); task2(render);
 * SpriteData @+0x38 } via func_800233A4, copies a large slice of the parent's
 * state into the child, binds the child's animation script (func_80023538,
 * already real in temp1.c), and registers per-type post-init + render
 * callbacks (func_80024730 / func_80025224). All of these are INCLUDE_ASM in
 * temp1.c for the matching build; ported here from asm, correct-by-inspection.
 * Work-list add/remove/setter machinery lives in work_list_port.c.
 * --------------------------------------------------------------------------- */

extern void AnimScriptTick(void* pSpriteData);
extern void func_80023804(void* pSpriteData);
extern void func_80023538(void* pSpriteData, void* pAnimation);
extern void func_8002393C(void* arg0);
extern void func_80023950(void* arg0);
extern int func_8001EE74(void* arg0);
extern void func_8001CE74(void* pTargetEntry);
extern void func_8001D034(void* pTargetEntry);
extern void func_8001D3F4(void* pTargetSprite);
extern void TimerWorkListAddTask(void* pOwner, void* pEntry);
extern void WorkListAddTask(void* pOwner, void* pEntry);
extern void TimerWorkListSetTaskCallback(void* pTask, void (*callback)(void*));
extern void WorkListSetTaskCallback(void* pTask, void (*callback)(void*));
extern void WorkListTaskSetOnFreeCallback(void* pTask, void (*callback)(void*));
extern void TimerWorkListRemoveTask(void* pTargetEntry);
extern void WorkListRemoveTask(void* pTargetEntry);
extern u8 D_800591AC;
extern u8 D_800591AF;
extern void func_80022038(void* pSpriteData);
extern s32 func_8002C700(u8* a0, u8* a1, u32* a2, s32 a3);
extern MATRIX D_8004FBB8;
extern u_long* g_GfxCurOT;
extern void func_80025710(void);
extern u8 D_8006BE10[];
extern u8 D_8005A474[];
extern u32 g_GfxCurWorkBuffer;
extern s32 g_GfxCurContext;
extern u32 D_80059300[];
extern u32 D_801E8670[];

/* Model-resource overlay archive 0x6B9, retail 0x801E72CC-0x801E7374.
 *
 * D_801E8670 is a packed table of 32-bit PSX pointers. selector chooses an
 * entry, whose +4 word points at the model resource. index zero copies the
 * resource's base matrix at +0x0C. Nonzero indices compose that base matrix
 * with the matrix at resource + index*0x7C + 0x2C. Retail never reads its
 * second argument, which makes the func_800748E8 src==dst call safe.
 */
void func_801E72CC(MATRIX* dst, MATRIX* unused, s32 selector, s32 index)
{
    u8* entry;
    u8* resource;
    u32* srcWords;
    u32* dstWords;
    u32 word0;
    u32 word1;
    u32 word2;
    u32 word3;

    (void)unused;
    entry = (u8*)(uintptr_t)D_801E8670[selector];
    if (entry == NULL) {
        return;
    }

    resource = (u8*)(uintptr_t)*(u32*)(entry + 4);
    if (index != 0) {
        CompMatrix((MATRIX*)(resource + 0x0C),
                   (MATRIX*)(resource + index * 0x7C + 0x2C), dst);
        return;
    }

    srcWords = (u32*)(resource + 0x0C);
    dstWords = (u32*)dst;
    word0 = srcWords[0];
    word1 = srcWords[1];
    word2 = srcWords[2];
    word3 = srcWords[3];
    dstWords[0] = word0;
    dstWords[1] = word1;
    dstWords[2] = word2;
    dstWords[3] = word3;
    word0 = srcWords[4];
    word1 = srcWords[5];
    word2 = srcWords[6];
    word3 = srcWords[7];
    dstWords[4] = word0;
    dstWords[5] = word1;
    dstWords[6] = word2;
    dstWords[7] = word3;
}

/* .sbss @0x800592EC: counts func_80022E8C ticks (type-7 timer callback). */
s32 D_800592EC;
/* .bss @0x8006F99C / 0x8006F9AC (asm/slus_006.64/data/49AC0.bss.s, 0x10 each,
 * zero-initialized like retail): default position vectors copied into type
 * 10-13 child sprites by func_80024730. */
u32 D_8006F99C[4];
u32 D_8006F9AC[4];

/* Field-overlay hooks called from the main executable's sprite code. Neither
 * has decompiled source anywhere in the repo (0x800BAxxx/0x800BCxxx live in
 * the field overlay's address space); until they are ported, log once and
 * no-op so the gap is visible instead of silent. */
void func_800BA8F4(void* pSpriteData)
{
    static int logged;
    (void)pSpriteData;
    if (!logged) {
        logged = 1;
        fprintf(stderr, "[port] func_800BA8F4 (field-overlay hook in sprite "
                        "gravity path) not ported; no-op\n");
    }
}

void func_800BC158(void* pWrapper)
{
    static int logged;
    (void)pWrapper;
    if (!logged) {
        logged = 1;
        fprintf(stderr, "[port] func_800BC158 (field-overlay hook for type "
                        "10-13 child sprites) not ported; no-op\n");
    }
}

/* Type-2/7 child render callback (asm 80025718-800257EC): refresh the sprite
 * transform if dirty, bail when the model header is absent, apply the sprite's
 * position halfwords through TransMatrix, optionally compose with the camera
 * matrix at D_8004FBB8 unless +0x3F bit 0 is set, load the composite into the
 * GTE, then dispatch the model packet through func_8002C700. */
void func_80025718(void* pTask)
{
    u8* task = pTask;
    u8* sprite = (u8*)(uintptr_t)*(u32*)(task + 0x4);
    u8* pBase;
    u32 modelHdr;
    u32 modelBuf;
    VECTOR trans;
    MATRIX composite;
    MATRIX* pDraw;

    func_80022038(sprite);
    pBase = (u8*)(uintptr_t)*(u32*)(sprite + 0x20);
    if (pBase == NULL) {
        return;
    }
    modelHdr = *(u32*)(pBase + 0x34);
    if (modelHdr == 0) {
        return;
    }

    trans.vx = *(s16*)(sprite + 0x2);
    trans.vy = *(s16*)(sprite + 0x6);
    trans.vz = *(s16*)(sprite + 0xA);
    TransMatrix((MATRIX*)(pBase + 0xC), &trans);

    pDraw = (MATRIX*)(pBase + 0xC);
    if ((*(u8*)(sprite + 0x3F) & 1) == 0) {
        CompMatrix(&D_8004FBB8, (MATRIX*)(pBase + 0xC), &composite);
        pDraw = &composite;
    }
    SetRotMatrix(pDraw);
    SetTransMatrix(pDraw);

    modelBuf = *(u32*)(pBase + 0x2C + (u32)g_GfxCurContext * 4);

    /* TEMPORARY DIAGNOSTIC (child visibility audit, 2026-07-09): with
     * XENO_CHILD_DRAW_DIAG=1, log the OT-emit counter (D_80059578) delta
     * across func_8002C700 for the first child dispatches plus a sparse
     * tail, and the leading prim group ids. Off by default; remove once
     * child pixels are confirmed. */
    {
        static int s_diag = -1;
        static int s_calls;
        s32 emitBefore = 0;
        int doLog = 0;

        if (s_diag < 0) {
            const char* env = getenv("XENO_CHILD_DRAW_DIAG");
            s_diag = (env != NULL && env[0] != '\0' && env[0] != '0');
        }
        if (s_diag) {
            doLog = (s_calls < 12) || (s_calls % 100) == 0;
            emitBefore = D_80059578;
        }

        {
            s32 ret = func_8002C700((u8*)(uintptr_t)modelHdr,
                                    (u8*)(uintptr_t)modelBuf, (u32*)g_GfxCurOT,
                                    *(u16*)(sprite + 0x42) & 0x4);
            if (doLog) {
                u8* hdr = (u8*)(uintptr_t)modelHdr;
                u8* grp = (u8*)(uintptr_t)*(u32*)(hdr + 0x10);
                fprintf(stderr,
                        "[child-draw] #%d sprite=%p hdr=%p buf=%08x ctx=%d "
                        "var=%d groups=%u prim0=%u cnt0=%d trans=(%d,%d,%d) "
                        "drawT=(%d,%d,%d) ret=%d emits=%d->%d\n",
                        s_calls, (void*)sprite, (void*)hdr, modelBuf,
                        (int)g_GfxCurContext, (int)(*(u16*)(sprite + 0x42) & 0x4),
                        *(u16*)(hdr + 0x6), grp[0], (int)*(s16*)(grp + 0x2),
                        (int)trans.vx, (int)trans.vy, (int)trans.vz,
                        (int)pDraw->t[0], (int)pDraw->t[1], (int)pDraw->t[2],
                        ret, emitBefore, D_80059578);
            }
            if (s_diag) {
                s_calls++;
            }
        }
    }
}

/* Retail .data callback table @0x8004FD40 (temp1.c CALLBACK_TABLE comment):
 * per-type render callbacks WorkListSetTaskCallback'd onto the child's task2.
 * Retail entries: 0/5/6/14=func_80025258, 1=func_80025710, 2/7=func_80025718,
 * 8=func_8002541C, 9=func_80025544, 15=func_800257F0, 3/4/10-13=NULL. Only
 * func_80025710 (dummy) is decompiled so far; the others stay NULL here and
 * func_80025224 logs when a retail-non-NULL slot is requested — those sprites
 * exist and animate but do not render until their callback is ported. */
static void (*const D_8004FD40[16])(void*) = {
    NULL,                          /* 0: func_80025258 (unported) */
    (void (*)(void*))func_80025710,/* 1: dummy */
    func_80025718,                 /* 2: type-2 child model draw */
    NULL, NULL,                    /* 3,4: NULL in retail */
    NULL, NULL,                    /* 5,6: func_80025258 (unported) */
    func_80025718,                 /* 7: type-7 child model draw */
    NULL,                          /* 8: func_8002541C (unported) */
    NULL,                          /* 9: func_80025544 (unported) */
    NULL, NULL, NULL, NULL,        /* 10-13: NULL in retail */
    NULL,                          /* 14: func_80025258 (unported) */
    NULL,                          /* 15: func_800257F0 (unported) */
};

/* asm 80025224: bind the per-type render callback onto the child's render
 * task. The extra logging below is port-only visibility for the still-NULL
 * table slots; retail semantics (including setting NULL) are unchanged. */
void func_80025224(void* pTask, int handlerIndex)
{
    static const u16 retailNonNull = 0xC3E7; /* bits 0,1,2,5,6,7,8,9,14,15 */
    static u16 loggedMask;

    if (D_8004FD40[handlerIndex & 0xF] == NULL &&
        (retailNonNull >> (handlerIndex & 0xF)) & 1 &&
        !((loggedMask >> (handlerIndex & 0xF)) & 1)) {
        loggedMask |= 1 << (handlerIndex & 0xF);
        fprintf(stderr, "[port] anim render callback D_8004FD40[%d] not "
                        "ported; child sprite will not render\n",
                handlerIndex & 0xF);
    }
    WorkListSetTaskCallback(pTask, D_8004FD40[handlerIndex]);
}

/* asm 80023440: sprite type from the animation-script header halfword:
 * bits 8-10, plus 8 if bit 14 is set. */
s32 func_80023440(void* pScript)
{
    u16 header = *(u16*)pScript;
    s32 type = (header >> 8) & 0x7;

    if ((header >> 14) & 1) {
        type += 8;
    }
    return type;
}

/* asm 80023468 via jtbl_80018664: sprite type -> transform-block mode.
 * mode 0 = no transform block, 1 = transform block + per-frame buffer,
 * 2 = transform block only. Type 3 (and >= 0x10) has no table entry in
 * retail and returns an uninitialized register — a dead path (the only
 * caller re-derives type 3 from sprite flags first); -1 here makes
 * func_80023A48's assert catch it loudly if it ever becomes live. */
s32 func_80023468(s32 type)
{
    static const s8 modes[16] = { 1, 0, 2, -1, 0, 1, 1, 2,
                                  0, 0, 1, 1, 1, 1, 1, 2 };

    if ((u32)type >= 0x10) {
        return -1;
    }
    return modes[type];
}

/* asm 800233A4 (matched-C comment in temp1.c): allocate the AnimTask
 * (0xEC header = two WorkListEntry + SpriteData, plus dataSize of per-mode
 * trailing buffers), register task1 on the timer list under pOwner and task2
 * on the render list under task1, init the SpriteData, and bind the tick
 * (func_80022DF4) and free (func_80022EB8) callbacks. */
void func_80022DF4(void* pTask);
void func_80022E8C(void* pTask);
void func_80022EB8(void* pTask);

void* func_800233A4(void* pOwner, int dataSize)
{
    u8* pEntry = HeapAlloc(dataSize + 0xEC, D_800591AF);
    u8* pTask2 = pEntry + 0x1C;
    u8* pSprite = pEntry + 0x38;

    TimerWorkListAddTask(pOwner, pEntry);
    WorkListAddTask(pEntry, pTask2);
    func_80023804(pSprite);
    *(u32*)(pEntry + 0x4) = (u32)(uintptr_t)pSprite;
    *(u32*)(pTask2 + 0x4) = (u32)(uintptr_t)pSprite;
    TimerWorkListSetTaskCallback(pEntry, func_80022DF4);
    WorkListTaskSetOnFreeCallback(pEntry, func_80022EB8);
    return pEntry;
}

/* asm 80023958 (mode 2): transform block at sprite+0xB4; no per-frame or
 * direction buffers. */
void func_80023958(void* pSpriteData)
{
    u8* p = pSpriteData;
    u8* pBase = p + 0xB4;

    *(u32*)(p + 0x20) = (u32)(uintptr_t)pBase;
    func_8002393C(pBase);
    *(u32*)(pBase + 0x34) = 0;
    *(u32*)(pBase + 0x40) = 0;
}

/* asm 800239F4 (mode 1): transform block at sprite+0xB4 with the per-frame
 * work buffer at sprite+0xF4 (sized by func_80023A48's frame-count math). */
void func_800239F4(void* pSpriteData)
{
    u8* p = pSpriteData;
    u8* pBase = p + 0xB4;

    *(u32*)(p + 0x20) = (u32)(uintptr_t)pBase;
    func_8002393C(pBase);
    *(u32*)(pBase + 0x30) = (u32)(uintptr_t)(p + 0xF4);
    *(u32*)(pBase + 0x34) = 0;
    *(u32*)(pBase + 0x38) = 0;
}

/* asm 80023A48: allocate + shape the child AnimTask by mode. Mode 1 sizes a
 * trailing per-frame buffer from the package's frame count ((n-1)*24+0x58)
 * and types 5/6 swap in the global default packages. Sprite+0x86 records the
 * sprite-local allocation size (mode extra + 0xEC); +0x6C points back at the
 * wrapper AnimTask; +0x24 is the (possibly overridden) anim package. */
void* func_80023A48(s32 type, s32 mode, void* pAnimPackage, s32 dataSize,
                    void* pOwner)
{
    u8* pkg = pAnimPackage;
    s32 extra;
    u8* pWrapper;
    u8* pSprite;

    if (mode == 1) {
        if (type == 5) {
            pkg = D_8006BE10;
        }
        if (type == 6) {
            pkg = D_8005A474;
        }
        extra = (func_8001EE74((void*)(uintptr_t)*(u32*)pkg) - 1) * 24 + 0x58;
        pWrapper = func_800233A4(pOwner, extra + dataSize);
        func_800239F4(pWrapper + 0x38);
    } else if (mode == 0) {
        extra = 0;
        pWrapper = func_800233A4(pOwner, dataSize);
        func_80023950(pWrapper + 0x38);
    } else if (mode == 2) {
        extra = 0x54;
        pWrapper = func_800233A4(pOwner, dataSize + 0x54);
        func_80023958(pWrapper + 0x38);
    } else {
        /* Retail reaches here only via the type-3/invalid dead path in
         * func_80023468 and would run on uninitialized registers. */
        assert(0 && "func_80023A48: invalid sprite transform mode");
        return NULL;
    }

    pSprite = pWrapper + 0x38;
    *(u32*)(pSprite + 0x6C) = (u32)(uintptr_t)pWrapper;
    *(u16*)(pSprite + 0x86) = (u16)(extra + 0xEC);
    *(u32*)(pSprite + 0x24) = (u32)(uintptr_t)pkg;
    return pWrapper;
}

/* asm 80024730 via jtbl_800186A4: per-type post-init after the spawn copy.
 * Types 0-6/14 (and >= 0xF) just bind the render callback; 7 swaps the tick
 * callback for the counting variant; 8/9 preset frame fields; 10/11 zero the
 * frame and take a default position vector; 12/13 demote themselves to type
 * 10/11 (type-2) before doing the same. */
void func_80024730(void* pWrapper)
{
    u8* w = pWrapper;
    u8* sp = w + 0x38;
    s32 idx = (*(u32*)(sp + 0x40) >> 13) & 0xF;
    u32* src = NULL;

    if (idx < 0xF) {
        switch (idx) {
        case 7:
            TimerWorkListSetTaskCallback(w, func_80022E8C);
            break;
        case 8:
            *(u8*)(sp + 0x2B) = 0x68;
            *(u16*)(sp + 0x34) = 1;
            break;
        case 9:
            *(u16*)(sp + 0x36) = 3;
            *(u8*)(sp + 0x2B) = 0x60;
            *(u16*)(sp + 0x34) = 1;
            break;
        case 10:
            *(u16*)(sp + 0x34) = 0; /* delay slot: precedes the callee body */
            func_800BC158(w);
            src = D_8006F99C;
            break;
        case 11:
            *(u16*)(sp + 0x34) = 0; /* delay slot: precedes the callee body */
            func_800BC158(w);
            src = D_8006F9AC;
            break;
        case 12:
        case 13: {
            u32 v = *(u32*)(sp + 0x40);

            *(u16*)(sp + 0x34) = 1;
            idx = (idx - 2) & 0xF;
            *(u32*)(sp + 0x40) = (v & 0xFFFE1FFF) | (idx << 13);
            func_800BC158(w);
            src = D_8006F99C;
            break;
        }
        default:
            break;
        }
    }
    if (src) {
        *(u32*)(sp + 0x0) = src[0];
        *(u32*)(sp + 0x4) = src[1];
        *(u32*)(sp + 0x8) = src[2];
    }
    func_80025224(w + 0x1C, idx);
}

/* asm 80022CAC: scale value by the sprite's slow-motion timer (+0x3A,
 * 10-bit fixed point); passthrough when the timer is zero. */
s32 func_80022CAC(void* pSpriteData, s32 value)
{
    s32 t = *(u16*)((u8*)pSpriteData + 0x3A);
    s32 v;

    if (t == 0) {
        return value;
    }
    v = value * t;
    if (v < 0) {
        v += 0x3FF;
    }
    return v >> 10;
}

/* asm 80022B2C: vertical motion integrator. Bit 26 of +0x3C selects the
 * simple path (no floor); otherwise position +0x4 advances by scaled
 * velocity +0x10, clamps to the floor height +0x84, bounces by the
 * A8-encoded coefficient when falling onto it, and gains gravity +0x1C. */
void func_80022B2C(void* pSpriteData)
{
    u8* p = pSpriteData;
    s32 vel, dv, pos, floor;

    if ((*(u32*)(p + 0x3C) >> 26) & 1) {
        vel = *(s32*)(p + 0x10);
        dv = func_80022CAC(p, vel >> 4) << 4;
        *(s32*)(p + 0x4) += dv;
        *(s32*)(p + 0x10) = vel + *(s32*)(p + 0x1C);
        return;
    }

    func_800BA8F4(p);

    vel = *(s32*)(p + 0x10);
    if (vel > 0 && *(s32*)(p + 0x1C) > 0) {
        floor = *(s16*)(p + 0x84);
        if (*(s16*)(p + 0x6) == (s16)floor) {
            return;
        }
        dv = func_80022CAC(p, vel >> 4) << 4;
        pos = *(s32*)(p + 0x4) + dv;
        *(s32*)(p + 0x4) = pos;
        if ((pos >> 16) < floor) {
            *(s32*)(p + 0x10) += *(s32*)(p + 0x1C);
            return;
        }

        /* Landed: snap to the floor and bounce. */
        *(s32*)(p + 0x4) = floor << 16;
        pos = -vel * (s32)((*(u32*)(p + 0xA8) >> 1) & 0x3FF);
        if (pos < 0) {
            pos += 0xFF;
        }
        pos >>= 8;
        *(s32*)(p + 0x10) = pos;
        if (pos < 0) {
            pos = -pos;
        }
        dv = *(s32*)(p + 0x1C);
        if (dv < 0) {
            dv = -dv;
        }
        if (pos < dv) {
            *(s32*)(p + 0x10) = 0;
        }
        return;
    }

    dv = func_80022CAC(p, vel >> 4) << 4;
    pos = *(s32*)(p + 0x4) + dv;
    *(s32*)(p + 0x4) = pos;
    floor = *(s16*)(p + 0x84);
    if ((pos >> 16) >= floor) {
        *(s32*)(p + 0x4) = floor << 16;
    }
    *(s32*)(p + 0x10) += *(s32*)(p + 0x1C);
}

/* asm 80022CDC: horizontal motion (x +0x0 by velocity +0xC, z +0x8 by
 * velocity +0x14, both slow-motion scaled) then the vertical integrator. */
void func_80022CDC(void* pSpriteData)
{
    u8* p = pSpriteData;

    *(s32*)(p + 0x0) += func_80022CAC(p, *(s32*)(p + 0xC) >> 4) << 4;
    *(s32*)(p + 0x8) += func_80022CAC(p, *(s32*)(p + 0x14) >> 4) << 4;
    func_80022B2C(p);
}

/* asm 80022DF4: AnimTask timer tick. Runs the sprite's script + motion; when
 * the script terminates (+0x64 == 0) — immediately, or after the double-tick
 * granted by AC bit 6 — invokes the task's free callback (func_80022EB8). */
void func_80022DF4(void* pTask)
{
    u8* t = pTask;
    u8* sp = (u8*)(uintptr_t)*(u32*)(t + 0x4);

    AnimScriptTick(sp);
    func_80022CDC(sp);
    if (*(u32*)(sp + 0x64) != 0) {
        if (!((*(u32*)(sp + 0xAC) >> 6) & 1)) {
            return;
        }
        AnimScriptTick(sp);
        func_80022CDC(sp);
        if (*(u32*)(sp + 0x64) != 0) {
            return;
        }
    }
    ((void (*)(void*))(uintptr_t)*(u32*)(t + 0xC))(pTask);
}

/* asm 80022E8C: type-7 tick variant — counts invocations in D_800592EC. */
void func_80022E8C(void* pTask)
{
    D_800592EC++;
    func_80022DF4(pTask);
}

/* asm 80025180: push an 8-byte node onto the current context's deferred
 * image list (D_80059300[ctx]), allocated from the gfx work buffer. Retail
 * advances the buffer head by 8 even when it is NULL; kept as-is. */
void func_80025180(void* pData)
{
    u8* node = (u8*)(uintptr_t)g_GfxCurWorkBuffer;

    g_GfxCurWorkBuffer = (u32)(uintptr_t)(node + 8);
    if (node != NULL) {
        *(u32*)(node + 0x0) = (u32)(uintptr_t)pData;
        *(u32*)(node + 0x4) = D_80059300[g_GfxCurContext];
        D_80059300[g_GfxCurContext] = (u32)(uintptr_t)node;
    }
}

/* asm 80022EB8: AnimTask free callback. Queues the transform block's +0x2C
 * buffer for deferred free, releases the direction table for mode-1 sprites,
 * unlinks owned tasks (AC bit 5) and parent links (+0xB0 bit 11), drops the
 * sprite from the pending-frame chain, removes both tasks and frees the
 * AnimTask allocation. */
void func_80022EB8(void* pTask)
{
    u8* t = pTask;
    u8* sp = (u8*)(uintptr_t)*(u32*)(t + 0x4);
    u8* pBase = (u8*)(uintptr_t)*(u32*)(sp + 0x20);
    u32 v;

    if (pBase != NULL) {
        v = *(u32*)(pBase + 0x2C);
        if (v != 0) {
            func_80025180((void*)(uintptr_t)v);
        }
    }
    if ((*(u32*)(sp + 0x3C) & 0x3) == 1) {
        pBase = (u8*)(uintptr_t)*(u32*)(sp + 0x20);
        v = *(u32*)(pBase + 0x34);
        if (v != 0) {
            HeapFree((void*)(uintptr_t)v);
        }
    }
    if ((*(u32*)(sp + 0xAC) >> 5) & 1) {
        func_8001CE74(pTask);
    }
    if ((*(u32*)(sp + 0xB0) >> 11) & 1) {
        func_8001D034(pTask);
    }
    if ((*(u32*)(sp + 0x3C) & 0x3) == 1) {
        func_8001D3F4(sp);
    }
    TimerWorkListRemoveTask(pTask);
    WorkListRemoveTask(t + 0x1C);
    HeapFree(pTask);
}

/* asm 80023B84: THE opcode-0xE0 worker — spawn a child sprite driven by the
 * script at pScript, cloning a large slice of the parent's state. Returns
 * the child SpriteData. D_800591AC is suppressed across the spawn when the
 * parent's +0xB0 bit 8 is set (the child then skips the unk14_3 timer flag
 * in TimerWorkListAddTask). */
void* func_80023B84(void* pParentSprite, void* pScript, void* pAnimPackage)
{
    u8* pSrc = pParentSprite;
    u8 savedTimerFlag = D_800591AC;
    s32 type, mode;
    u8* pWrapper;
    u8* pDst;
    u32 v, combo;

    v = *(u32*)(pSrc + 0xB0) | 0x800;
    *(u32*)(pSrc + 0xB0) = v;
    if ((v >> 8) & 1) {
        D_800591AC = 0;
    }

    type = func_80023440(pScript);
    if (type == 3) {
        type = (*(u32*)(pSrc + 0x40) >> 13) & 0xF;
    }
    mode = func_80023468(type);

    pWrapper = func_80023A48(type, mode, pAnimPackage, 0,
                             (void*)(uintptr_t)*(u32*)(pSrc + 0x6C));
    pDst = pWrapper + 0x38;

    *(u32*)(pWrapper + 0x14) |= 0x20000000; /* task1.unk14_1: child marker */

    /* Type into +0x40 bits 13-16, mode into +0x3C bits 0-1. */
    *(u32*)(pDst + 0x40) =
        (*(u32*)(pDst + 0x40) & 0xFFFE1FFF) | ((type & 0xF) << 13);
    *(u32*)(pDst + 0x3C) = (*(u32*)(pDst + 0x3C) & ~0x3u) | (mode & 0x3);

    /* Inherited flag bits. */
    *(u32*)(pDst + 0x40) =
        (*(u32*)(pDst + 0x40) & ~0x1F00u) | (*(u32*)(pSrc + 0x40) & 0x1F00);
    *(u32*)(pDst + 0x3C) =
        (*(u32*)(pDst + 0x3C) & ~0x8u) | (*(u32*)(pSrc + 0x3C) & 0x8);
    *(u32*)(pDst + 0x3C) =
        (*(u32*)(pDst + 0x3C) & ~0x10u) | (*(u32*)(pSrc + 0x3C) & 0x10);
    *(u8*)(pDst + 0x3D) = *(u8*)(pSrc + 0x3D);
    *(u32*)(pDst + 0x40) =
        (*(u32*)(pDst + 0x40) & 0xFFFBFFFF) | (*(u32*)(pSrc + 0x40) & 0x40000);
    *(u32*)(pDst + 0x3C) = (*(u32*)(pDst + 0x3C) | 0x4000000) & ~0x4u;

    *(u32*)(pDst + 0x18) = *(u32*)(pSrc + 0x18);
    *(u16*)(pDst + 0x32) = *(u16*)(pSrc + 0x32);
    *(u16*)(pDst + 0x2C) = *(u16*)(pSrc + 0x2C);
    *(u16*)(pDst + 0x34) = *(u16*)(pSrc + 0x34);

    /* +0xB0 bit 9 (scaled mode): copy; when set, also inherit the slow-motion
     * timer and force +0x40 bits 8-12 to 3. */
    v = (*(u32*)(pSrc + 0xB0) >> 9) & 1;
    *(u32*)(pDst + 0xB0) = (*(u32*)(pDst + 0xB0) & ~0x200u) | (v << 9);
    if (v) {
        *(u16*)(pDst + 0x3A) = *(u16*)(pSrc + 0x3A);
        *(u32*)(pDst + 0x40) = (*(u32*)(pDst + 0x40) & 0xFFFFE0FF) | 0x300;
    }

    /* Parent A8 top 2 bits and AC bits 0-1. */
    combo = ((*(u32*)(pSrc + 0xAC) & 0x3) << 2) | (*(u32*)(pSrc + 0xA8) >> 30);
    *(u32*)(pDst + 0xA8) = (*(u32*)(pDst + 0xA8) & 0x3FFFFFFF) | (combo << 30);
    *(u32*)(pDst + 0xAC) = (*(u32*)(pDst + 0xAC) & ~0x3u) | (combo >> 2);

    /* +0xB0 bit 8; +0xAC bit 6, bits 7-18, bit 2; clear child A8 bit 0. */
    *(u32*)(pDst + 0xB0) =
        (*(u32*)(pDst + 0xB0) & ~0x100u) | (*(u32*)(pSrc + 0xB0) & 0x100);
    *(u32*)(pDst + 0xAC) =
        (*(u32*)(pDst + 0xAC) & ~0x40u) | (*(u32*)(pSrc + 0xAC) & 0x40);
    v = (*(u32*)(pDst + 0xAC) & 0xFFF8007F) | (*(u32*)(pSrc + 0xAC) & 0x7FF80);
    *(u32*)(pDst + 0xAC) = v;
    *(u32*)(pDst + 0xA8) &= ~0x1u;
    *(u32*)(pDst + 0xAC) = (v & ~0x4u) | (*(u32*)(pSrc + 0xAC) & 0x4);

    /* Share the parent's +0x7C state block unless the parent owns it
     * exclusively (A8 bit 0). */
    if (*(u32*)(pSrc + 0xA8) & 0x1) {
        *(u32*)(pDst + 0x7C) = 0;
    } else {
        *(u32*)(pDst + 0x7C) = *(u32*)(pSrc + 0x7C);
    }

    *(u32*)(pDst + 0x70) = (u32)(uintptr_t)pSrc; /* parent back-link */
    *(u32*)(pDst + 0x44) = *(u32*)(pSrc + 0x44);
    *(u32*)(pDst + 0x48) = *(u32*)(pSrc + 0x48);
    *(u32*)(pDst + 0x74) = *(u32*)(pSrc + 0x74);
    *(u16*)(pDst + 0x82) = *(u16*)(pSrc + 0x82);
    *(u32*)(pDst + 0x50) = *(u32*)(pSrc + 0x50);
    *(u8*)(pDst + 0x8D) = *(u8*)(pSrc + 0xAF);
    *(u32*)(pDst + 0x78) = *(u32*)(pSrc + 0x78);
    *(u32*)(pDst + 0x00) = *(u32*)(pSrc + 0x00);
    *(u32*)(pDst + 0x04) = *(u32*)(pSrc + 0x04);
    *(u32*)(pDst + 0x08) = *(u32*)(pSrc + 0x08);
    *(u32*)(pDst + 0x0C) = *(u32*)(pSrc + 0x0C);
    *(u32*)(pDst + 0x10) = *(u32*)(pSrc + 0x10);
    *(u32*)(pDst + 0x14) = *(u32*)(pSrc + 0x14);

    if (mode != 0) {
        u8* srcBase = (u8*)(uintptr_t)*(u32*)(pSrc + 0x20);
        u8* dstBase = (u8*)(uintptr_t)*(u32*)(pDst + 0x20);

        *(u16*)(dstBase + 0x0) = *(u16*)(srcBase + 0x0);
        *(u16*)(dstBase + 0x2) = *(u16*)(srcBase + 0x2);
        *(u16*)(dstBase + 0x4) = *(u16*)(srcBase + 0x4);
        *(u16*)(dstBase + 0x6) = *(u16*)(srcBase + 0x6);
        *(u16*)(dstBase + 0x8) = *(u16*)(srcBase + 0x8);
        *(u16*)(dstBase + 0xA) = *(u16*)(srcBase + 0xA);
    }

    func_80023538(pDst, pScript);
    func_80024730(pWrapper);
    D_800591AC = savedTimerFlag;
    return pDst;
}

/* ---------------------------------------------------------------------------
 * func_8002CC10 (asm/slus_006.64/nonmatchings/system/temp2/func_8002CC10.s,
 * INCLUDE_ASM at temp2.c:664): anim-script opcode 0x8D worker. Latches the
 * sprite texture-page override from the anim package's VRAM x/y and switches
 * the tpage-latch mode (D_80050108, semantics documented at temp2.c:692:
 * 0 = raw latch, 1 = mask low bits and merge the D_80059310 override, 2 =
 * full override) to "merge". GetTPage(0,0,..) = 4-bit CLUT tpage, ABR 0.
 * --------------------------------------------------------------------------- */
extern s32 D_80059310;
extern s32 D_80050108;

void func_8002CC10(s32 x, s32 y)
{
    D_80059310 = GetTPage(0, 0, x & 0xFFFF, y & 0xFFFF) & 0x1F;
    D_80050108 = 1;
}

/* ---------------------------------------------------------------------------
 * func_8002C59C (asm/slus_006.64/nonmatchings/system/temp2/func_8002C59C.s,
 * INCLUDE_ASM at temp2.c:240): in-place pointer relocation of a sprite model
 * blob, sibling of the already-real func_8002C3E8 (temp2.c:193) with a
 * different header layout. One-shot (header flag bit 0x20): relocates the
 * header offset words +0x8/+0x10/+0xC/+0x14 by the blob base, then, when the
 * +0x1C sub-list offset is non-zero, relocates it and walks its 12-byte-
 * stride entries (count word first, then entries; count == -1 means none)
 * from index count down to 0, relocating each entry's +0x4/+0x8. Stores are
 * truncated u32 (host RAM below 4 GiB), matching the retail `sw` width.
 * Used by anim-script opcode 0xF5's model-data load.
 * --------------------------------------------------------------------------- */
void func_8002C59C(u8* pModel)
{
    u16 flags = *(u16*)(pModel + 0x0);
    u32 base = (u32)(uintptr_t)pModel;
    u32 off;
    u8* pEntry;
    s32 count;

    if (flags & 0x20) {
        return;
    }
    *(u16*)(pModel + 0x0) = flags | 0x20;
    *(u32*)(pModel + 0x8) += base;
    *(u32*)(pModel + 0x10) += base;
    *(u32*)(pModel + 0xC) += base;
    *(u32*)(pModel + 0x14) += base;

    off = *(u32*)(pModel + 0x1C);
    if (off == 0) {
        return;
    }
    *(u32*)(pModel + 0x1C) = base + off;

    count = *(s32*)(pModel + off);
    if (count != -1) {
        /* Decrement-first walk, entries count down to 0 (count+1 total);
         * same do-while shape as the matched sibling func_8002C3E8. */
        pEntry = pModel + off + 0x4 + count * 12;
        do {
            count--;
            *(u32*)(pEntry + 0x4) += base;
            *(u32*)(pEntry + 0x8) += base;
            pEntry -= 12;
        } while (count != -1);
    }
}
