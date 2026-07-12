#include "common.h"
#include "system/memory.h"
#include "field/effects.h"
#ifdef XENO_PC_PORT
#include <assert.h>
#else
/* <assert.h> is unavailable under the matching build's -nostdinc MIPS
 * preprocessor. The assert(0) below marks an unimplemented path in a function
 * not yet byte-matched, so a no-op assert compiles safely there. */
#define assert(x) ((void)0)
#endif

extern int D_800ADB24;

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

INCLUDE_ASM("asm/field/nonmatchings/effects/distortion", FieldDistortionInitialize);

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

void FieldDistortionDraw(void) {
    if (g_FieldEffects.distortion.isActive == 0) {
        return;
    }

    assert(0 && "FieldDistortionDraw active distortion path is not migrated");
}
