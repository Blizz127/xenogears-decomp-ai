#ifndef _XENO_FIELD_EFFECTS_H
#define _XENO_FIELD_EFFECTS_H

#include "psyq/libgpu.h"

typedef struct {
    short isActive;
    short isFinished;
    short duration;
    char _pad[2];
    int v1;
    int v2;
    int v3;
    int v4;
    int v5;
    int v6;
    int delta1;
    int delta2;
    int delta3;
    int delta4;
    int delta5;
    int delta6;
    short unk38;
    short unk3C;
    /* PSX 4-byte pointer slots. Host `DR_MOVE*`/`POLY_FT4*` here would be
     * 8 bytes (and 8-aligned) on LP64, inflating FieldDistortion by 0x1C so
     * FieldEffects.fades[] overlaps the packed D_800B2174 block at +0xFC
     * (g_FieldControl, D_800B218E, etc.). Retail FieldEffects is 0xFC.
     * Reconstruct via (void*)(uintptr_t). */
    /* 0x3C */ u32 buffer0;
    /* 0x40 */ u32 buffer1;
    /* 0x44 */ u32 buffer2;
    /* 0x48 */ u32 buffer3;
} FieldDistortion;

typedef struct {
    DR_MODE drawModes[2];
    TILE tiles[2];
    int r0;
    int g0;
    int b0;
    int redDelta;
    int greenDelta;
    int blueDelta;
    short semitransparency;
    short isVisible;
    short duration;
    short _pad;
} FieldFade;

typedef struct {
    FieldDistortion distortion;
    FieldFade fades[2];
} FieldEffects;

extern FieldEffects g_FieldEffects;

#endif