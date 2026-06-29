#include "common.h"
#include "field/main.h"
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
    if (g_FieldSystemMode == SYSTEM_MODE_PC_HDD)
        asm("break 0x400");

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

INCLUDE_ASM("asm/field/nonmatchings/main/misc2", func_80074108);

INCLUDE_ASM("asm/field/nonmatchings/main/misc2", func_8007469C);

INCLUDE_ASM("asm/field/nonmatchings/main/misc2", FieldPollControllers);

INCLUDE_ASM("asm/field/nonmatchings/main/misc2", func_800748E8);

INCLUDE_ASM("asm/field/nonmatchings/main/misc2", func_8007520C);

INCLUDE_ASM("asm/field/nonmatchings/main/misc2", func_800752C8);

void FieldAddPrimitives(u_long* ot, u_long* pPrimList, int size) {
    AddPrims(ot, pPrimList + size, pPrimList);
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc2", func_80075484);

INCLUDE_ASM("asm/field/nonmatchings/main/misc2", func_8007554C);

INCLUDE_ASM("asm/field/nonmatchings/main/misc2", func_80075910);

INCLUDE_ASM("asm/field/nonmatchings/main/misc2", func_800759E4);

INCLUDE_ASM("asm/field/nonmatchings/main/misc2", func_80075B08);

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
