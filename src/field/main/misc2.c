#include "common.h"
#include "field/main.h"
#include "field/actor.h"
#include "field/camera.h"
#include "system/math.h"
#include "psyq/libgpu.h"
#include "psyq/libgte.h"

void FieldMatrixResetTranslation(MATRIX *matrix) {
    matrix->t[2] = 0;
    matrix->t[1] = 0;
    matrix->t[0] = 0;
}

void FieldComputeSceneMatrices(void) {
    VECTOR scale;
    MATRIX _unused;
    MATRIX sp40;
    long flag;

    RotMatrix(&g_Scene.camRotation, &g_Scene.camRotationMatrix);
    FieldMatrixResetTranslation(&g_Scene.camRotationMatrix);
    CompMatrix(&g_Scene.camRotationMatrix, &g_Scene.viewMatrix, &sp40);
    FieldMatrixCopy(&g_Scene.viewMatrix, &sp40);
    RotMatrix(&g_Scene.worldRotation, &g_Scene.worldRotationMatrix);
    FieldMatrixResetTranslation(&g_Scene.worldRotationMatrix);
    RotMatrix(&g_Scene.worldRotation, &g_Scene.worldToScreenMatrix);
    MulMatrix2(&g_Scene.viewMatrix, &g_Scene.worldToScreenMatrix);
    SetRotMatrix(&g_Scene.viewMatrix);
    SetTransMatrix(&g_Scene.viewMatrix);
    RotTrans(&g_Scene.worldTranslation, &g_Scene.worldToScreenMatrix.t, &flag);
    scale.vx = g_WorldScale;
    scale.vy = g_WorldScale;
    scale.vz = g_WorldScale;
    ScaleMatrix(&g_Scene.worldToScreenMatrix, &scale);
    SetRotMatrix(&g_Scene.worldToScreenMatrix);
    SetTransMatrix(&g_Scene.worldToScreenMatrix);
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc2", func_80072254);

void FieldMatrixCreateWorldToScreen(void) {
    FieldComputeSceneMatrices();
    SetRotMatrix(&g_Scene.worldToScreenMatrix);
    SetTransMatrix(&g_Scene.worldToScreenMatrix);

    if (g_FieldSystemMode == SYSTEM_MODE_PC_HDD) {
        func_802815B0();
    }
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc2", func_8007234C);

INCLUDE_ASM("asm/field/nonmatchings/main/misc2", func_80072398);

INCLUDE_ASM("asm/field/nonmatchings/main/misc2", func_800723E4);

INCLUDE_ASM("asm/field/nonmatchings/main/misc2", func_8007254C);

INCLUDE_ASM("asm/field/nonmatchings/main/misc2", func_800726E8);

INCLUDE_ASM("asm/field/nonmatchings/main/misc2", func_80072A38);

INCLUDE_ASM("asm/field/nonmatchings/main/misc2", func_80072D74);

INCLUDE_ASM("asm/field/nonmatchings/main/misc2", func_80073230);

void func_80073684(VECTOR* pEye, VECTOR* pAt) {
    MATRIX matSceneRotation;
    VECTOR distance;
    VECTOR transformed;

    PushMatrix();
    RotMatrix(&g_Scene.sceneAngle, &matSceneRotation);
    distance.vx = pAt->vx - pEye->vx;
    distance.vy = pAt->vy - pEye->vy;
    distance.vz = pAt->vz - pEye->vz;
    ApplyMatrixLV(&matSceneRotation, &distance, &transformed);
    pEye->vx = transformed.vx + pAt->vx;
    pEye->vz = transformed.vz + pAt->vz;
    PopMatrix();
}

void func_80073734(void* arg0) {
    *(s32*)((u8*)arg0 + 0x0) = *(s16*)((u8*)arg0 + 0x2);
    *(s32*)((u8*)arg0 + 0x4) = *(s16*)((u8*)arg0 + 0x6);
    *(s32*)((u8*)arg0 + 0x8) = *(s16*)((u8*)arg0 + 0xA);
}

// Computes a LookAt / View Matrix
// Eye: The position of the camera
// At: The point we're looking at
void FieldMatrixLookAt(MATRIX* pMatLookAt, VECTOR* pEye, VECTOR* pAt, VECTOR* pUp) {
    VECTOR vecWork;
    VECTOR zAxis;
    VECTOR xAxis;
    VECTOR yAxis;
    SVECTOR translation;

    // Forward direction (X-Axis) is defined by the line going from the eye and
    // the point we're looking at
    vecWork.vx = CONV_TO_GTE(pAt->vx - pEye->vx);
    vecWork.vy = CONV_TO_GTE(pAt->vy - pEye->vy);
    vecWork.vz = CONV_TO_GTE(pAt->vz - pEye->vz);
    
    yAxis.vx = pUp->vx;
    yAxis.vy = pUp->vy;
    yAxis.vz = pUp->vz;
    yAxis.vx >>= 0x10;
    yAxis.vy >>= 0x10;
    yAxis.vz >>= 0x10;
    
    VectorNormal(&vecWork, &zAxis);

    // Given a vector pointing in the upwards direction, we use the cross product
    // to get our right direction (Z-Axis)
    OuterProduct12(&yAxis, &zAxis, &vecWork);
    VectorNormal(&vecWork, &xAxis);

    // Finally, knowing our two other basis vectors a cross product between them
    // gives the remaining basis vector for the Y-Axis
    OuterProduct12(&zAxis, &xAxis, &vecWork);
    VectorNormal(&vecWork, &yAxis);
    
    pMatLookAt->m[0][0] = xAxis.vx;
    pMatLookAt->m[0][1] = xAxis.vy;
    pMatLookAt->m[0][2] = xAxis.vz;
    pMatLookAt->m[1][0] = yAxis.vx;
    pMatLookAt->m[1][1] = yAxis.vy;
    pMatLookAt->m[1][2] = yAxis.vz;
    pMatLookAt->m[2][0] = zAxis.vx;
    pMatLookAt->m[2][1] = zAxis.vy;
    pMatLookAt->m[2][2] = zAxis.vz;
    
    translation.vx = CONV_TO_GTE(pEye->vx) * 3;
    translation.vy = CONV_TO_GTE(pEye->vy) * 3;
    translation.vz = CONV_TO_GTE(pEye->vz) * 3;

    // We need to translate the eye position back to the origin first,
    // and combine the translation and rotation operation into a single matrix.
    ApplyMatrix(pMatLookAt, &translation, &vecWork);
    pMatLookAt->t[0] = -vecWork.vx;
    pMatLookAt->t[1] = -vecWork.vy;
    pMatLookAt->t[2] = -vecWork.vz;
}

int FieldMathInterpolateAngle(int from, int to, int delta) {
    int difference;
    int nextAngle;

    difference = PSX_ANGLE(from - to);

    // If we have less than 180 degrees difference we subtract the delta
    if (difference < PSX_DEGREES(180)) {
        nextAngle = from - delta;
        difference = PSX_ANGLE(nextAngle - to);
        if (difference >= PSX_DEGREES(180)) {
            nextAngle = to;
        }
        return PSX_ANGLE(nextAngle);
    } 
    
    // If our difference is greater than 180 degrees we add our delta
    nextAngle = from + delta;
    difference = PSX_ANGLE(nextAngle - to);
    if (difference < PSX_DEGREES(180)) {
        nextAngle = to;
    }
    return PSX_ANGLE(nextAngle);
}

// If 0, the angle will be updated in steps rather than directly set to the target
extern s32 D_800ADC18;

int FieldMathUpdateAngle(int curAngle, int targetAngle, int delta) {
    int result;
    if (D_800ADC18 == 0) {
        result = FieldMathInterpolateAngle(curAngle, targetAngle, delta);
    } else {
        result = PSX_ANGLE(targetAngle);
    }
    return result;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc2", func_800739C0);

INCLUDE_ASM("asm/field/nonmatchings/main/misc2", func_80073E38);

void FieldClearAndSwapOTagInternal(void) {
    if (g_FieldSystemMode == SYSTEM_MODE_PC_HDD) {
#ifndef XENO_PC_PORT
        asm("break 0x400");   /* MIPS trap; host assembler can't emit it. PC-HDD
                                 dev path only -- never taken in the CD_ROM port. */
#endif
    }

    g_FieldCurRenderContextIndex = (g_FieldCurRenderContextIndex + 1) % 2;
    g_FieldCurRenderContext = &g_FieldRenderContexts[g_FieldCurRenderContextIndex];
    ClearOTagR(g_FieldCurRenderContext->ot3, 0x8);
}

void FieldClearAndSwapOTag(void) {
    FieldClearAndSwapOTagInternal();
    ClearOTagR(g_FieldCurRenderContext->ot1, 0x1000);
    if (g_FieldRenderContextUseOT2) {
        ClearOTagR(g_FieldCurRenderContext->ot2, 0x1000);
    }
}

void FieldMatrixCopy(MATRIX* dest, MATRIX* source) {
    FieldMatrixCopyTransform(dest, source);
    FieldMatrixCopyTranslation(dest, source);
}

void FieldMatrixCopyTranslation(MATRIX* dest, MATRIX* source) {
    dest->t[0] = source->t[0];
    dest->t[1] = source->t[1];
    dest->t[2] = source->t[2];
}

void FieldMatrixCopyTransform(MATRIX* dest, MATRIX* source) {
    dest->m[0][0] = source->m[0][0];
    dest->m[0][1] = source->m[0][1];
    dest->m[0][2] = source->m[0][2];
    dest->m[1][0] = source->m[1][0];
    dest->m[1][1] = source->m[1][1];
    dest->m[1][2] = source->m[1][2];
    dest->m[2][0] = source->m[2][0];
    dest->m[2][1] = source->m[2][1];
    dest->m[2][2] = source->m[2][2];
}

/* ---- func_80074108: background/camera draw dispatch -------------------------
 * Full decompile from ASM (371 lines). Three phases:
 *   1. CLUT/color table populate: 8-iteration loop filling D_800AFD24
 *   2. Camera matrix setup: LookAt, angle update, matrix chain for bg layer
 *   3. Quad rendering: multiple FieldRenderQuad loops for background/tiles */
extern u16 D_800AFD24[0x80];
extern u16 D_800AFC08[0x80];
extern u16 D_800ADC24[8];
extern u8 D_800B0050[];  /* base symbol; RECT is at D_800B0050 - 4 */
extern VECTOR g_CameraEye;
extern VECTOR g_CameraAt;
extern u16 D_800B233E;
extern s16 D_800ADB48;
extern s16 D_800ADB4A;
extern u8 D_800B21D1;
extern s32 D_8004F378;
extern u8 D_800B0F7C[];
extern u8 D_800B06BC[];
extern u8 D_800B0FEC[];
extern u8 D_800B1E00[];
extern CameraInterpolation g_CamInterpolation;
extern void func_80070594(MATRIX* dest);
extern s32 FieldGetVec2Magnitude(s32 dx, s32 dy);
extern void func_8007AC58(u_long* ot, void* pQuad, MATRIX* pMat, s32 renderCtx);
extern void SetTransMatrix(MATRIX* m);

void func_80074108(void) {
    s32 i, j;
    u16* pDst;
    u16* pSrc;
    u16* pFlags;
    u8 sceneFlag;
    s32 idx;
    s32 newAngle;
    s32 unused6C;
    s32 unused64;
    u8* pRenderCtx;
    s32 renderCtxIdx;
    u8* pQuadData;
    u32 mask24;
    u32 maskHi;
    MATRIX matView;
    MATRIX matTemp;
    MATRIX matComposite;
    MATRIX matWork;
    SVECTOR svRot;

    /* ======== PHASE 1: CLUT / color table population (loop 0..7) ======== */
    pDst = D_800AFD24;
    pSrc = D_800AFC08;
    pFlags = D_800ADC24;
    sceneFlag = *(u8*)((u8*)&g_Scene + 0x65);
    idx = 0;

    for (i = 0; i < 8; i++) {
        if (sceneFlag & pFlags[i]) {
            for (j = 0; j < 0x10; j++) {
                pDst[idx + j] = 0;
            }
        } else {
            for (j = 0; j < 0x10; j++) {
                pDst[idx + j] = pSrc[j];
            }
            pSrc += 0x10;
        }
        idx += 0x10;
    }

    /* Upload CLUT via LoadImage. RECT is at D_800B0050-4, w field at D_800B0050+0. */
    {
        u8* pRect = (u8*)D_800B0050 - 4;
        *(u16*)(pRect + 4) = 0x80;  /* sh 0x80, 0($v1) where $v1 = D_800B0050 */
#ifdef XENO_PC_PORT
        if (*(s16*)(pRect + 6) != 0)  /* guard: stubbed h=0 would crash LoadImage */
#endif
        LoadImage((RECT*)pRect, (u_long*)D_800AFD24);
    }

    /* ======== PHASE 2: Camera / matrix setup ======== */
    SetGeomScreen(0x80);
    SetGeomOffset(0x10A, 0xA6);

    /* Build view matrix: FieldMatrixLookAt(matView, eye, at, up)
     * eye = {0, g_CameraEye.vy - g_CameraAt.vy, -mag<<16}
     * at  = {0, 0, 0}
     * up  = (VECTOR*)((u8*)&g_CameraEye + 0x20) = &g_CameraAt (adjacent globals) */
    {
        s32 dx = (g_CameraEye.vx - g_CameraAt.vx) >> 16;
        s32 dy = (g_CameraEye.vz - g_CameraAt.vz) >> 16;
        s32 mag = FieldGetVec2Magnitude(dx, dy);
        VECTOR eye; eye.vx = 0; eye.vy = g_CameraEye.vy - g_CameraAt.vy; eye.vz = (-mag) << 16;
        VECTOR at;  at.vx = 0;  at.vy = 0;                         at.vz = 0;
        FieldMatrixLookAt(&matView, &eye, &at, (VECTOR*)((u8*)&g_CameraEye + 0x20));
    }

    /* Setup matWork for composite matrix chain */
    func_80070594(&matWork);
    SetRotMatrix(&matWork);
    SetTransMatrix(&matWork);

    /* Actor-based camera angle */
    {
        u16 actorIdx = D_800B233E;
        s16 curAngle = D_800ADB48;
        u8* pActor = (u8*)g_FieldActors + actorIdx * 0x5C;
        s16 actorAngle = *(s16*)(*(u32*)(pActor + 0x4C) + 0x106);
        s32 targetAngle = actorAngle + (*(u16*)((u8*)&g_CamInterpolation + 0xA) + 0x400);
        D_800ADB4A = (s16)targetAngle;
        newAngle = FieldMathUpdateAngle(curAngle, (s16)(targetAngle >> 16), 0x40);
    }
    D_800ADB48 = (s16)newAngle;

    /* Build Y-rotation matrix, chain into view */
    FieldMatrixResetTranslation(&matTemp);
    svRot.vx = 0; svRot.vy = (s16)newAngle; svRot.vz = 0;
    RotMatrix(&svRot, &matTemp);
    MulMatrix2(&matView, &matTemp);
    unused6C = 0x1000;
    CompMatrix(&matWork, &matTemp, &matComposite);

    /* ---- First quad loop: D_800B0F7C, 0x14 iterations ---- */
    if (!D_800B21D1 && D_800ADC18 == 0 && D_8004F378 == 0) {
        pQuadData = D_800B0F7C;
        pRenderCtx = (u8*)g_FieldCurRenderContext;
        renderCtxIdx = g_FieldCurRenderContextIndex;
        for (i = 0; i < 0x14; i++) {
            FieldRenderQuad((u_long*)(pRenderCtx + 0x80D4),
                           pQuadData, &matComposite, renderCtxIdx);
            pQuadData += 0x70;
        }
    }

    /* ---- Second matrix chain ---- */
    func_80070594(&matTemp);
    MulMatrix2(&matView, &matTemp);
    unused6C = 0x1000;
    CompMatrix(&matWork, &matTemp, &matComposite);
    MulMatrix0(&matWork, &matTemp, (MATRIX*)((u8*)&g_Scene + 0xF4));
    SetRotMatrix(&matWork);
    SetTransMatrix(&matWork);

    /* ---- Third matrix chain ---- */
    func_80070594(&matTemp);
    MulMatrix2((MATRIX*)&g_Scene, &matTemp);
    unused6C = 0x1000;
    CompMatrix(&matWork, &matTemp, &matComposite);
    FieldMatrixCopy(&matWork, &matComposite);

    /* ---- Rotation for trigger-zone quad loop ---- */
    svRot.vx = 0x400; svRot.vy = 0; svRot.vz = 0;
    {
        MATRIX matRot;
        RotMatrix(&svRot, &matRot);

        if (!D_800B21D1 && D_800ADC18 == 0 && D_8004F378 == 0) {
            u8* pTrigger = (u8*)(uintptr_t)g_pFieldTriggerZones;
            pRenderCtx = (u8*)g_FieldCurRenderContext;
            renderCtxIdx = g_FieldCurRenderContextIndex;

            /* Trigger-zone loop: 0x10 iterations */
            for (i = 0; i < 0x10; i++) {
                func_80070594(&matTemp);
                unused64 = *(s16*)(pTrigger + 0x40 + i * 4);
                unused6C = *(s16*)(pTrigger + 0x42 + i * 4);
                CompMatrix(&matWork, &matTemp, &matComposite);
                FieldMatrixCopyTransform(&matComposite, &matRot);
                func_8007AC58((u_long*)(pRenderCtx + 0x80D4),
                             D_800B06BC + i * 0x70, &matComposite, renderCtxIdx);
            }

            /* Third quad loop: D_800B06BC, 0x10 iterations */
            pQuadData = D_800B06BC;
            for (i = 0; i < 0x10; i++) {
                FieldRenderQuad((u_long*)(pRenderCtx + 0x80D4),
                               pQuadData, &matWork, renderCtxIdx);
                pQuadData += 0x70;
            }
        }
    }

    /* ---- Fourth quad loop: D_800B0FEC, 4 iterations ---- */
    if (!D_800B21D1 && D_800ADC18 == 0 && D_8004F378 == 0) {
        pQuadData = D_800B0FEC;
        pRenderCtx = (u8*)g_FieldCurRenderContext;
        renderCtxIdx = g_FieldCurRenderContextIndex;
        for (i = 0; i < 4; i++) {
            FieldRenderQuad((u_long*)(pRenderCtx + 0x80D4),
                           pQuadData, &matWork, renderCtxIdx);
            pQuadData += 0x70;
        }
    }

    /* ---- Final: D_800B1E00 word swap + GTE reset ---- */
    {
        s32* pEntry = (s32*)(D_800B1E00 + g_FieldCurRenderContextIndex * 0xC0);
        u8* pRC = (u8*)g_FieldCurRenderContext;
        s32 valA = pEntry[0];
        s32 valB = *(s32*)(pRC + 0x80D4);
        pEntry[0] = (valA & 0xFF000000) | (valB & 0x00FFFFFF);
        *(s32*)(pRC + 0x80D4) = (valB & 0xFF000000) | (pEntry[0] & 0x00FFFFFF);
    }

    SetGeomOffset(0xA0, 0x70);
    SetGeomScreen(*(s32*)((u8*)&g_Scene + 0x68));
}


INCLUDE_ASM("asm/field/nonmatchings/main/misc2", func_8007469C);

INCLUDE_ASM("asm/field/nonmatchings/main/misc2", FieldPollControllers);

INCLUDE_ASM("asm/field/nonmatchings/main/misc2", func_800748E8);

INCLUDE_ASM("asm/field/nonmatchings/main/misc2", func_8007520C);

INCLUDE_ASM("asm/field/nonmatchings/main/misc2", func_800752C8);

void FieldAddPrimitives(u_long* ot, u_long* pPrimList, int size) {
    /* XENO_PC_PORT: skip if primitive chain is empty (all draw funcs stubbed).
     * Check raw lower 32 bits of first entry: addr:24|len:8 = 0 means empty. */
    if (*(u32*)pPrimList == 0) return;
    AddPrims(ot, pPrimList + size, pPrimList);
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc2", func_80075484);

/* ---- func_8007554C: per-frame field render pipeline ------------------------
 * Port-first functional decompile (control flow mirrors the asm). This is the
 * main render function called every frame from FieldMain's loop (via
 * func_80078D44's fade loops and the per-frame dispatch). It:
 *   - Vsync for frame timing,
 *   - updates field/actor/particle state,
 *   - renders fade/background/particles/distortion/actors,
 *   - swaps render contexts, clears the framebuffer, uploads DrawEnv/DispEnv,
 *   - adds primitives to the OT and calls DrawOTag to present the frame.
 * Many callees are still stubs in the port; the key path is DrawOTag at the end. */
extern s32 D_800ADB9C, D_800ADBA0, D_800ADC18, D_800ADBB4;
extern s32 g_FieldRenderContextUseOT2;
extern s16 D_800B21D4;
extern u8 D_800B219C, D_800B219D, D_800B219E;
extern s32 D_800B0048;
extern s32 D_800B217C;
extern void* D_800AF87C;
extern u8 D_800AFC58[];
extern void func_800739C0(void);
extern void func_80086908(void);
extern void func_80281B00(void*);
extern void FieldFadeUpdateAndDraw(void* arg0, int arg1);
extern void func_80074108(void);
extern void func_800748E8(void);
extern void func_800752C8(void);
extern void func_8007520C(void);
extern void func_80075484(void);
extern void func_80281450(void);
extern void func_80281400(void);
extern void func_800A84C0(void);
extern void func_800ABEC8(void);
extern void func_800805F4(void);
extern void func_8008004C(void* arg0, int arg1);
extern void func_80025044(void);
extern void func_800920D8(void);
extern char D_8006FB34, D_8006FB40, D_8006FB4C, D_8006FB58, D_8006FB64;
extern void HeapTickDelayedFree(void);

void func_8007554C(void) {
    s32 s1;
    s32 s0 = 0x80D4; /* offset into RenderContext for fade draw args */

    D_800ADB9C = Vsync(1);
    s1 = Vsync(-1);
    func_800739C0();
    func_80086908();

    if (g_FieldSystemMode == 0) {
        func_80281B00(&D_8006FB34);
    }

    /* Fade update + draw (args: renderContext + 0x80D4, renderContextIndex) */
    FieldFadeUpdateAndDraw((u8*)g_FieldCurRenderContext + s0,
                           g_FieldCurRenderContextIndex);

    if (g_FieldSystemMode == 0) {
        func_80281B00(&D_8006FB40);
    }

    func_80074108();

    /* Scratchpad stack save (0x1F8003FC area — PSX-specific; harmless on host) */
    {
        /* The asm stores $sp to the scratchpad top and allocates 4 bytes.
         * On the host this is a no-op (no scratchpad); skip it. */
    }

    func_800748E8();
    func_800752C8();
    FieldParticlesTickAndRender();

    if (g_FieldSystemMode == 0) {
        func_80281450();
    }

    FieldDistortionDraw();

    /* Restore scratchpad stack (no-op on host) */
    func_800A84C0();
    func_80075484();
    func_8007520C();
    func_800ABEC8();

    if (g_FieldSystemMode == 0) {
        func_80281400();
        func_80281B00(&D_8006FB4C);
    }

    D_800ADBA0 = Vsync(1);
    DrawSync(0);
    func_800805F4();

    /* Render context swap */
    func_8008004C((u8*)g_FieldCurRenderContext + s0,
                  g_FieldCurRenderContextIndex);

    Vsync(0);
    HeapTickDelayedFree();

    /* Framebuffer clear / display setup */
    if (D_800ADC18 == 0) {
        /* Normal path: clear the framebuffer area */
        ClearImage(&g_FieldCurRenderContext->drawEnvs[0].clip, 0, 0, 0);
    } else if (D_800B0048 == D_800ADC18) {
        /* MoveImage path: scroll the framebuffer */
        RECT rect;
        rect.x = 0x2C0; rect.y = 0x100; rect.w = 0x140; rect.h = 0xE0;
        MoveImage(&rect, 0, g_FieldCurRenderContextIndex << 8);
    } else {
        ClearImage(&g_FieldCurRenderContext->drawEnvs[0].clip, 0, 0, 0);
    }

    PutDispEnv(&g_FieldCurRenderContext->dispEnv);
    PutDrawEnv(&g_FieldCurRenderContext->drawEnvs[0]);

    if (g_FieldSystemMode == 0) {
        D_800ADB9C = Vsync(1);
    }

    func_80025044();

    if (g_FieldSystemMode == 0) {
        func_80281B00(&D_8006FB58);
    }

    func_800920D8();

    /* Conditional VRAM upload */
    if (D_800ADBB4 != 0) {
        LoadImage((RECT*)D_800AFC58, D_800AF87C);
        D_800ADBB4 = 0;
    }

    if (g_FieldSystemMode == 0) {
        func_80281B00(&D_8006FB64);
    }

    /* Add primitives to OT and draw. All offsets are BYTE offsets (MIPS addu).
     * XENO_PC_PORT: the C struct places ot1 at ~0x6C (host layout), but the ASM
     * accesses it at byte offset 0x80F0. ClearOTagR(->ot1) never touches the
     * ASM-offset OT, so we explicitly clear it here. This is a narrow workaround
     * until the RenderContext struct layout matches the PSX offsets. */
    if (D_800ADC18 == 0) {
        u_long* ot1_asm = (u_long*)((u8*)g_FieldCurRenderContext + 0x80F0);
        ClearOTagR(ot1_asm, 0x1000);
        if (g_FieldRenderContextUseOT2) {
            u_long* ot2_asm = (u_long*)((u8*)g_FieldCurRenderContext + D_800B21D4 * 4 + 0xCC);
            ClearOTagR(ot2_asm, 0x1000);
            FieldAddPrimitives(ot2_asm,
                               (u_long*)((u8*)g_FieldCurRenderContext + 0x40D0), 0);
        }
        FieldAddPrimitives(ot1_asm,
                           (u_long*)((u8*)g_FieldCurRenderContext + D_800B21D4 * 4 + 0xCC), 0);
    }

    /* The actual draw call (byte offset 0x80F0 into RenderContext = ot1) */
    DrawOTag((u_long*)((u8*)g_FieldCurRenderContext + 0x80F0));

    /* Frame timing wait loop */
    {
        s32 target = s1 + D_800B217C + 2;
        while (Vsync(-1) < target) {
        }
    }
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc2", func_80075910);

INCLUDE_ASM("asm/field/nonmatchings/main/misc2", func_800759E4);

extern s16 D_800B218E;

void func_80075B08(void* sprite, u8* color) {
    if (D_800B218E == 0) {
        SpriteSetColor(sprite, color[0], color[1], color[2]);
    }
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc2", func_80075B44);

INCLUDE_ASM("asm/field/nonmatchings/main/misc2", func_800764B4);

INCLUDE_ASM("asm/field/nonmatchings/main/misc2", func_80076A74);

INCLUDE_ASM("asm/field/nonmatchings/main/misc2", func_80076AC0);

INCLUDE_ASM("asm/field/nonmatchings/main/misc2", func_800771B0);

void FieldLoadTIM(u_long* pTimData) {
    TIM_IMAGE timImage;

    OpenTIM(pTimData);
    while (ReadTIM(&timImage) ) {
        if (timImage.caddr != NULL) {
            LoadImage(timImage.crect, timImage.caddr);
        }
        if (timImage.paddr != NULL) {
            LoadImage(timImage.prect, timImage.paddr);
        }
    }
}


INCLUDE_ASM("asm/field/nonmatchings/main/misc2", func_80077268);

INCLUDE_ASM("asm/field/nonmatchings/main/misc2", func_80077544);
