#include "common.h"
#include "system/memory.h"
#include "field/effects.h"
#ifdef XENO_PC_PORT
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#else
/* <assert.h> is unavailable under the matching build's -nostdinc MIPS
 * preprocessor. The assert(0) below marks an unimplemented path in a function
 * not yet byte-matched, so a no-op assert compiles safely there. */
#define assert(x) ((void)0)
#endif

extern int D_800ADB24;
extern u16 D_800AEB24[];
extern int FieldScriptVMGetArgument(int);

void FieldDistortionSetTarget(int, int, int, int, int, int, int);

/* The native build keeps field state in its retail 32-bit layout.  These
 * macros leave the matching source as the original typed field accesses while
 * recovering typed heap pointers only at the native boundary. */
#ifdef XENO_PC_PORT
#define FIELD_DISTORTION_BUFFER(member, type) \
    ((type*)(uintptr_t)g_FieldEffects.distortion.member)
#define FIELD_DISTORTION_STORE_BUFFER(member, value) \
    (g_FieldEffects.distortion.member = (u32)(uintptr_t)(value))
#else
#define FIELD_DISTORTION_BUFFER(member, type) \
    (g_FieldEffects.distortion.member)
#define FIELD_DISTORTION_STORE_BUFFER(member, value) \
    (g_FieldEffects.distortion.member = (value))
#endif

#ifdef XENO_PC_PORT
/* DIAGNOSTIC -- XENO_DISTORTION_DIAG=1.
 * Script opcode FE27 case 1 waits for distortion.isActive to clear, and the
 * ONLY writer that clears it on completion is FieldDistortionDraw below
 * (duration expired AND isFinished set).  A field that hangs on that wait is
 * therefore either not drawing, not counting down, or not marked finished, and
 * those three are indistinguishable from the script side -- hence the counter.
 * Removal: delete these two symbols and their three uses. */
unsigned long g_XenoDistortionDrawCalls = 0;
int g_XenoDistortionDiag = -1;

int PcPort_DistortionDiagEnabled(void) {
    if (g_XenoDistortionDiag < 0) {
        const char* e = getenv("XENO_DISTORTION_DIAG");
        g_XenoDistortionDiag = (e != NULL && e[0] != '\0' && e[0] != '0');
    }
    return g_XenoDistortionDiag;
}
#endif

void FieldDistortionFree(void) {
    g_FieldEffects.distortion.isActive = 0;
    if (D_800ADB24) {
#ifdef XENO_PC_PORT
        HeapFree((void*)(uintptr_t)g_FieldEffects.distortion.buffer0);
        HeapFree((void*)(uintptr_t)g_FieldEffects.distortion.buffer1);
        HeapFree((void*)(uintptr_t)g_FieldEffects.distortion.buffer2);
        HeapFree((void*)(uintptr_t)g_FieldEffects.distortion.buffer3);
#else
        HeapFree(g_FieldEffects.distortion.buffer0);
        HeapFree(g_FieldEffects.distortion.buffer1);
        HeapFree(g_FieldEffects.distortion.buffer2);
        HeapFree(g_FieldEffects.distortion.buffer3);
#endif
        D_800ADB24 = 0;
    }
}

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): field_distortion_start (src/decomp/src/field/field_screen.c:192). */
#ifndef XM_T_FadeChannel
#define XM_T_FadeChannel
typedef struct {
    DR_MODE modes[2]; 
    TILE tiles[2];   
    s32 level[3];
    s32 step[3];
    u16 abr;
    s16 active;
    s16 steps;
    u16 pad;
} FadeChannel;
#endif
#ifndef XM_T_FieldWork
#define XM_T_FieldWork
typedef struct FieldWork {
    s16 unk2078;               
    s16 unk207A;               
    s16 effect_steps;          
    u8 unk207E[2];
    s32 effect_value[6];       
    s32 effect_step[6];        
    s16 unk20B0[2];            
    void *effect_buffers[4];   
    FadeChannel fades[2];      
    u16 open_windows;          

    s16 encounter_inhibition;  
    s16 terrain_angle;         
    u16 input_mask;            
    s32 unk217C;               
    s32 unk2180;               
    SVECTOR sprite_angles;     
    s16 scale;                 
    s16 sprite_gate;           
    u8 fog_color[4];           
    u8 far_color[4];           
    s16 fog_range[2];          
    u8 clear_color[3];         
    u8 party_bits;             
    s16 unk21A0[6];            
    s16 emitter_range;         
    s16 piece_drift[3];        
    s16 unk21B4;               
    u8 unk21B6[2];
    s32 last_sound_effect;     
    s32 unk21BC[3];            
    u8 unk21C8[0x21CC - 0x21C8];
    u8 party_processing_mode;  
    u8 camera_floor_fixed;     
    u8 preserve_nonplayer_motion; 
    u8 forced_position;        
    u8 script_control[2];      
    u8 piece_drift_mode;       
    u8 unk21D3;
    s16 unk21D4;               
    s16 text_speed;            
    s32 camera_counter;        
    s16 unk21DC[4];            
    s16 unk21E4[4];            
    SVECTOR layer_positions[4]; 
    s32 layer_scales[4];       
    s16 unk221C[5][3];         
    u8 unk223A[2];
    s16 unk223C[3][3];         
    u8 unk224E[0x225C - 0x224E];
    u8 unk225C[3];             
    u8 unk225F[0x2264 - 0x225F]; 
    s32 unk2264;               
    s32 unk2268;               
    s32 controlled;            
    u16 encounter_music[16];   
    s16 battle_music;          
    u8 unk2292[2];
    s32 unk2294;               
    s32 unk2298;               
    s32 unk229C;               
    u16 unk22A0[32];           
    s16 unk22E0;               
    s16 emitter_descriptor[3]; 
    s16 unk22E8[3][4];         
    s16 unk2300[3][4];         
    s32 unk2318[3];            
    s16 emitter_position[3][4]; 
    u16 effects_kept;          
    u16 unk233E;               
    s16 repeat_delay;          
    s16 repeat_remaining;      
    s16 jump_mode;             
    s16 animation_mode;        
    u16 unk2348;               
    s16 unk234A;               
    s16 gear_riding_lock_override; 
    s16 followers_idle;        
    u32 unk2350;               
    u8 unk2354;                
    u8 unk2355;                
    u8 unk2356;                
    u8 unk2357;                
    u8 unk2358;                
    u8 unk2359[0x235C - 0x2359];
} FieldWork;
#endif
#ifndef XM_S_field_distortion_start
#define XM_S_field_distortion_start
void xm_field_distortion_start(s32 resume) __asm__("FieldDistortionInitialize");
#endif
#ifndef XM_S_field_work
#define XM_S_field_work
extern FieldWork xm_field_work __asm__("g_FieldEffects");
#endif
#ifndef XM_S_field_distortion_buffers_allocated
#define XM_S_field_distortion_buffers_allocated
extern s32 xm_field_distortion_buffers_allocated __asm__("D_800ADB24");
#endif
#ifndef XM_S_heap_alloc
#define XM_S_heap_alloc
void *xm_heap_alloc(s32 size, s32 mode) __asm__("HeapAlloc");
#endif
#ifndef XM_S_SetPolyFT4
#define XM_S_SetPolyFT4
void xm_SetPolyFT4(POLY_FT4 *p) __asm__("SetPolyFT4");
#endif
#ifndef XM_S_SetSemiTrans
#define XM_S_SetSemiTrans
void xm_SetSemiTrans(void *p, int abe) __asm__("SetSemiTrans");
#endif
#ifndef XM_S_GetTPage
#define XM_S_GetTPage
u_short xm_GetTPage(int tp, int abr, int x, int y) __asm__("GetTPage");
#endif
#ifndef XM_S_SetDrawMove
#define XM_S_SetDrawMove
void xm_SetDrawMove(DR_MOVE *p, RECT *rect, int x, int y) __asm__("SetDrawMove");
#endif
#ifndef XM_S_field_distortion_strip_sources
#define XM_S_field_distortion_strip_sources
extern DVECTOR xm_field_distortion_strip_sources[15] __asm__("D_800AEB24");
#endif
#ifndef XM_S_field_distortion_set_targets
#define XM_S_field_distortion_set_targets
void xm_field_distortion_set_targets(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f, s32 steps) __asm__("FieldDistortionSetTarget");
#endif
#ifndef XM_S_field_event_read_imm_or_var
#define XM_S_field_event_read_imm_or_var
s32 xm_field_event_read_imm_or_var(s32 offset) __asm__("FieldScriptVMGetArgument");
#endif
void FieldDistortionInitialize(s32 resume) {
    RECT rect;
    s32 row;
    s32 column;
    POLY_FT4 *poly;
    DVECTOR *strips;
    POLY_FT4 *copy;
    s32 u;

    xm_field_work.unk2078 = 1;
    if (xm_field_distortion_buffers_allocated == 0) {
        xm_field_work.effect_buffers[0] = xm_heap_alloc(0x180, 0);
        xm_field_work.effect_buffers[1] = xm_heap_alloc(0x180, 0);
        xm_field_work.effect_buffers[2] = xm_heap_alloc(0x3840, 0);
        xm_field_work.effect_buffers[3] = xm_heap_alloc(0x3840, 0);
        xm_field_distortion_buffers_allocated = 1;
        for (row = 0; row < 17; row++) {
            for (column = 0; column < 20; column++) {
                poly = (POLY_FT4 *)xm_field_work.effect_buffers[2] + (column + row * 20);
                copy = (POLY_FT4 *)xm_field_work.effect_buffers[3] + (column + row * 20);
                xm_SetPolyFT4(poly);
                xm_SetSemiTrans(poly, 0);
                poly->r0 = 0x80;
                poly->g0 = 0x80;
                poly->b0 = 0x80;
                poly->x0 = column * 16;
                poly->y0 = row * 16;
                poly->x1 = column * 16 + 16;
                poly->y1 = row * 16;
                poly->x2 = column * 16;
                poly->y2 = row * 16 + 16;
                poly->x3 = column * 16 + 16;
                poly->y3 = row * 16 + 16;
                if (row >= 14) {
                    u = (column * 16) & 0x3F;
                    poly->u0 = u;
                    poly->v0 = (column >> 2) * 16 + (row - 14) * 80;
                    poly->u1 = u + 16;
                    poly->v1 = (column >> 2) * 16 + (row - 14) * 80;
                    poly->u2 = u;
                    poly->v2 = (column >> 2) * 16 + ((row - 14) * 80 + 16);
                    poly->u3 = u + 16;
                    poly->v3 = (column >> 2) * 16 + ((row - 14) * 80 + 16);
                    poly->tpage = xm_GetTPage(2, 0, 0x3C0, 0);
                    *copy = *poly;
                } else {
                    poly->u0 = (column * 16) & 0x3F;
                    poly->v0 = row * 16;
                    poly->u1 = ((column * 16) & 0x3F) + 16;
                    poly->v1 = row * 16;
                    poly->u2 = (column * 16) & 0x3F;
                    poly->v2 = row * 16 + 16;
                    poly->u3 = ((column * 16) & 0x3F) + 16;
                    poly->v3 = row * 16 + 16;
                    poly->tpage = xm_GetTPage(2, 0, (column * 16) & 0xFFC0, 0);
                    *copy = *poly;
                    copy->tpage = xm_GetTPage(2, 0, (column * 16) & 0xFFC0, 0x100);
                }
            }
        }
        rect.x = 0;
        rect.y = 0x20;
        rect.w = 0x140;
        rect.h = 0xC0;
        xm_SetDrawMove(xm_field_work.effect_buffers[0], &rect, 0, 0);
        rect.y = 0x120;
        xm_SetDrawMove(xm_field_work.effect_buffers[1], &rect, 0, 0x100);
        rect.w = 0x40;
        rect.h = 0x10;
        strips = xm_field_distortion_strip_sources;
        for (row = 0; row < 15; row++) {
            rect.x = strips[row].vx;
            rect.y = xm_field_distortion_strip_sources[row].vy;
            xm_SetDrawMove((DR_MOVE *)xm_field_work.effect_buffers[0] + (row + 1), &rect, 0x3C0, row * 16);
            rect.y = xm_field_distortion_strip_sources[row].vy + 0x100;
            xm_SetDrawMove((DR_MOVE *)xm_field_work.effect_buffers[1] + (row + 1), &rect, 0x3C0, row * 16);
        }
    }
    if (resume == 0) {
        xm_field_distortion_set_targets(xm_field_event_read_imm_or_var(1), xm_field_event_read_imm_or_var(3), xm_field_event_read_imm_or_var(5), xm_field_event_read_imm_or_var(7),
                      xm_field_event_read_imm_or_var(9), xm_field_event_read_imm_or_var(11), xm_field_event_read_imm_or_var(13));
    }
    xm_field_work.unk207A = 0;
}
#endif
#else
void FieldDistortionInitialize(int useExistingTarget) {
    POLY_FT4* pPoly;
    POLY_FT4* pPoly2;
    RECT rect;
    int i;
    int j;

    g_FieldEffects.distortion.isActive = 1;

    if (D_800ADB24 == 0) {
        FIELD_DISTORTION_STORE_BUFFER(buffer0, HeapAlloc(0x180, 0));
        FIELD_DISTORTION_STORE_BUFFER(buffer1, HeapAlloc(0x180, 0));
        FIELD_DISTORTION_STORE_BUFFER(buffer2, HeapAlloc(0x3840, 0));
        FIELD_DISTORTION_STORE_BUFFER(buffer3, HeapAlloc(0x3840, 0));
        D_800ADB24 = 1;

        for (i = 0; i < 0x11; i++) {
            for (j = 0; j < 0x14; j++) {
                pPoly = &FIELD_DISTORTION_BUFFER(buffer2, POLY_FT4)[i * 0x14 + j];
                pPoly2 = &FIELD_DISTORTION_BUFFER(buffer3, POLY_FT4)[i * 0x14 + j];

                SetPolyFT4(pPoly);
                SetSemiTrans(pPoly, 0);
                setRGB0(pPoly, 0x80, 0x80, 0x80);
                setXYWH(pPoly, j * 0x10, i * 0x10, 0x10, 0x10);

                /* asm 800A4970/800A4990: i < 0xE falls to .L800A4A44, the
                 * per-column framebuffer pages (context 0 at y=0, context 1
                 * at y=0x100); rows 14-16 sample the 0x3C0 capture strips,
                 * with strip index (j>>2) + (i-0xE)*5 shared by both
                 * contexts (the strips themselves are re-captured per
                 * context by the DR_MOVE lists). */
                if (i < 0xE) {
                    setUVWH(pPoly,
                        (j * 0x10) & 0x3F,
                        i * 0x10,
                        0x10, 0x10);
                    pPoly->tpage = GetTPage(2, 0, (j * 0x10) & 0xFFC0, 0);
                    *pPoly2 = *pPoly;
                    pPoly2->tpage = GetTPage(2, 0, (j * 0x10) & 0xFFC0, 0x100);
                } else {
                    setUVWH(pPoly,
                        (j * 0x10) & 0x3F,
                        ((j >> 2) + (i - 0xE) * 5) << 4,
                        0x10, 0x10);
                    pPoly->tpage = GetTPage(2, 0, 0x3C0, 0);
                    *pPoly2 = *pPoly;
                }
            }
        }

        rect.x = 0;
        rect.y = 0x20;
        rect.w = 0x140;
        rect.h = 0xC0;
        SetDrawMove(FIELD_DISTORTION_BUFFER(buffer0, DR_MOVE), &rect, 0, 0);

        rect.y = 0x120;
        SetDrawMove(FIELD_DISTORTION_BUFFER(buffer1, DR_MOVE), &rect, 0, 0x100);

        rect.w = 0x40;
        rect.h = 0x10;
        for (i = 0; i < 0xF; i++) {
            rect.x = D_800AEB24[i * 2];
            rect.y = D_800AEB24[i * 2 + 1];
            SetDrawMove(&FIELD_DISTORTION_BUFFER(buffer0, DR_MOVE)[i + 1], &rect, 0x3C0, i * 0x10);
            rect.y += 0x100;
            SetDrawMove(&FIELD_DISTORTION_BUFFER(buffer1, DR_MOVE)[i + 1], &rect, 0x3C0, i * 0x10);
        }
    }

    if (useExistingTarget == 0) {
        FieldDistortionSetTarget(
            FieldScriptVMGetArgument(1),
            FieldScriptVMGetArgument(3),
            FieldScriptVMGetArgument(5),
            FieldScriptVMGetArgument(7),
            FieldScriptVMGetArgument(9),
            FieldScriptVMGetArgument(0xB),
            FieldScriptVMGetArgument(0xD));
    }

    g_FieldEffects.distortion.isFinished = 0;
}
#endif /* XENO_PC_PORT */

void FieldDistortionSetTarget(int t1, int t2, int t3, int t4, int t5, int t6, int duration) {
    if (duration == 0)
        duration = 1;
    
    g_FieldEffects.distortion.duration = duration;
    g_FieldEffects.distortion.delta1 = (t1 * 0x10000 - g_FieldEffects.distortion.v1) / duration;
    g_FieldEffects.distortion.delta2 = (t2 * 0x10000 - g_FieldEffects.distortion.v2) / duration;
    g_FieldEffects.distortion.delta3 = (t3 * 0x10000 - g_FieldEffects.distortion.v3) / duration;
    g_FieldEffects.distortion.delta4 = (t4 * 0x10000 - g_FieldEffects.distortion.v4) / duration;
    g_FieldEffects.distortion.delta5 = (t5 * 0x10000 - g_FieldEffects.distortion.v5) / duration;
    g_FieldEffects.distortion.delta6 = (t6 * 0x10000 - g_FieldEffects.distortion.v6) / duration;
}

#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/effects/distortion", FieldDistortionDraw);
#else
void FieldDistortionDraw(void) {
    FieldDistortion* distortion = &g_FieldEffects.distortion;
    DR_MOVE* moves;
    POLY_FT4* polys;
    int context;
    int amplitudeX;
    int amplitudeY;
    int columnAngleStep;
    int rowAngleStep;
    int columnPhaseStep;
    int rowPhaseStep;
    int rowAngle;
    int columnAngle;
    int waveX;
    int waveY;
    int i;
    int j;
    void* ot;
    void* dmode;

    /* These globals are deliberately kept local to this function: the field
     * effects header only needs the packed effect-state layout. */
    extern int g_FieldCurRenderContextIndex;
    extern void* g_FieldCurRenderContext;
    extern u8 D_800B1E18[];
    extern int rsin(int angle);

#ifdef XENO_PC_PORT
    g_XenoDistortionDrawCalls++;
#endif

    if (distortion->isActive == 0) {
        return;
    }

    /* 800A4DF0--800A4F24: duration drives the six 16.16 controls regardless
     * of isFinished.  Once it expires, only a finished effect is reset and
     * disabled; an unfinished zero-duration effect keeps its current state. */
    if (distortion->duration > 0) {
        distortion->v1 += distortion->delta1;
        distortion->v2 += distortion->delta2;
        distortion->v3 += distortion->delta3;
        distortion->v4 += distortion->delta4;
        distortion->v5 += distortion->delta5;
        distortion->v6 += distortion->delta6;
        distortion->duration--;
    } else if (distortion->isFinished != 0) {
        distortion->v1 = 0;
        distortion->v2 = 0;
        distortion->v3 = 0;
        distortion->v4 = 0;
        distortion->v5 = 0;
        distortion->v6 = 0;
        distortion->isActive = 0;
    }

    /* The renderer consumes the high halves of the six 16.16 values. */
    amplitudeX = (s16)((u32)distortion->v1 >> 16);
    amplitudeY = (s16)((u32)distortion->v2 >> 16);
    columnAngleStep = (s16)((u32)distortion->v3 >> 16);
    rowAngleStep = (s16)((u32)distortion->v4 >> 16);
    columnPhaseStep = (s16)((u32)distortion->v5 >> 16);
    rowPhaseStep = (s16)((u32)distortion->v6 >> 16);

    /* sh stores in the retail code deliberately wrap these phase accumulators
     * to 16 bits. */
    distortion->unk38 = (s16)((u16)distortion->unk38 + (u16)columnPhaseStep);
    distortion->unk3C = (s16)((u16)distortion->unk3C + (u16)rowPhaseStep);

    context = g_FieldCurRenderContextIndex;
#ifdef XENO_PC_PORT
    moves = (DR_MOVE*)(uintptr_t)(context == 0 ? distortion->buffer0 : distortion->buffer1);
    polys = (POLY_FT4*)(uintptr_t)(context == 0 ? distortion->buffer2 : distortion->buffer3);
#else
    moves = context == 0 ? distortion->buffer0 : distortion->buffer1;
    polys = context == 0 ? distortion->buffer2 : distortion->buffer3;
#endif
    /* +0xD0 is ot1 bucket 1 in the packed field context.  (The preceding
     * +0xCC tag is the bucket-0 terminator.) */
    ot = (u8*)g_FieldCurRenderContext + 0xD0;

    /* 800A4FC8--800A51B0: rows 14--16 occupy the wrapped lower capture
     * strip.  The writes are shared between adjacent quads, matching the
     * original packet topology rather than generating independent vertices. */
    rowAngle = distortion->unk3C + rowAngleStep * 11;
    for (i = 14; i < 17; i++) {
        waveY = (s32)((s64)rsin(rowAngle) * amplitudeY) >> 12;
        rowAngle += rowAngleStep;
        columnAngle = distortion->unk38;

        for (j = 0; j < 20; j++) {
            POLY_FT4* poly = &polys[i * 20 + j];

            waveX = (s32)((s64)rsin(columnAngle) * amplitudeX) >> 12;
            columnAngle += columnAngleStep;

            if (j == 0) {
                poly->x0 = 0;
                poly->x2 = 0;
            } else {
                int x = j * 16 + waveX;

                poly[-1].x1 = x;
                poly[-1].x3 = x;
                poly->x0 = x;
                poly->x2 = x;

                /* asm 800A509C: the wrapped strip's right edge waves with
                 * the column (addiu $v0, $s3, 0x140), unlike the fixed
                 * 0x140 edge of the rows 0-13 loop. */
                if (j == 19) {
                    poly->x1 = waveX + 0x140;
                    poly->x3 = waveX + 0x140;
                }
            }

            {
                int y = i * 16 + waveY - 0x30;

                poly[-20].y2 = y;
                poly[-20].y3 = y;
                poly->y0 = y;
                poly->y1 = y;
            }

            if (i == 16) {
                poly->y2 = 0xE0;
                poly->y3 = 0xE0;
            }

            addPrim(ot, poly);
        }
    }

    /* The first draw-move packet captures the current framebuffer into the
     * VRAM strips before the warped quads and the remaining strip moves are
     * threaded onto ot1. */
    addPrim(ot, moves);

    /* 800A5224--800A549C: rows 0--13.  The final row has an explicit lower
     * edge using the next row phase, precisely as in the retail packet code. */
    rowAngle = distortion->unk3C;
    for (i = 0; i < 14; i++) {
        waveY = (s32)((s64)rsin(rowAngle) * amplitudeY) >> 12;
        rowAngle += rowAngleStep;
        columnAngle = distortion->unk38;

        for (j = 0; j < 20; j++) {
            POLY_FT4* poly = &polys[i * 20 + j];

            waveX = (s32)((s64)rsin(columnAngle) * amplitudeX) >> 12;
            columnAngle += columnAngleStep;

            if (i == 0) {
                poly->y0 = 0x20;
                poly->y1 = 0x20;
            } else if (i == 13) {
                /* asm 800A52D8-800A533C: the top edge uses waveY, then the
                 * SAME variable is overwritten with the next row's wave
                 * ($s6 at 800A532C) for the bottom edge -- so from j=1 on,
                 * the top edge also carries the row-14 wave.  Retail packet
                 * behavior; do not "fix" with a separate local. */
                int y = waveY + 0xF0;

                poly->y0 = y;
                poly->y1 = y;
                poly[-20].y2 = y;
                poly[-20].y3 = y;
                waveY = (s32)((s64)rsin(rowAngle) * amplitudeY) >> 12;
                poly->y2 = waveY + 0xF0;
                poly->y3 = waveY + 0xF0;
            } else {
                int y = i * 16 + waveY + 0x20;

                poly[-20].y2 = y;
                poly[-20].y3 = y;
                poly->y0 = y;
                poly->y1 = y;
            }

            if (j == 0) {
                poly->x0 = 0;
                poly->x2 = 0;
            } else {
                int x = j * 16 + waveX;

                poly[-1].x1 = x;
                poly[-1].x3 = x;
                poly->x0 = x;
                poly->x2 = x;

                if (j == 19) {
                    poly->x1 = 0x140;
                    poly->x3 = 0x140;
                }
            }

            addPrim(ot, poly);
        }
    }

    for (i = 1; i < 16; i++) {
        addPrim(ot, &moves[i]);
    }

    /* This is DR_MODE[3] of the existing field text-box primitive store; it
     * restores the capture page after the fifteen DR_MOVE packets.  Keep the
     * address in a local first: PsyCross's simple-pointer setaddr macro does
     * not parenthesize its source expression, so passing the addition directly
     * would scale the context term as a u_int pointer on the native port. */
    dmode = D_800B1E18 + context * 0xC0;
    addPrim(ot, dmode);
}
#endif /* XENO_PC_PORT */
