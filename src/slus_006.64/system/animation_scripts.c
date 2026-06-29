#include "common.h"
#include "field/actor.h"

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/animation_scripts", func_8001FBA4);
/*
Matches on GCC 2.7.2-970404, ASPSX 2.67
Co-Authored-By: eagleflo <eagleflo@users.noreply.github.com>

void* func_8001FBA4(SpriteData* pSpriteData, u8* pIndex) {
    u8 index = *pIndex;
    if (!(index & 0x80)) {
        s32 signedIndex = (s8)index;
        return &pSpriteData->stack[pSpriteData->stackIndex + signedIndex];
    }
    return &pSpriteData->field_0x88[index & 0x7F];
}
*/

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/animation_scripts", func_8001FBE4);

s32 func_80021AD8(s32 color, s32 value) {
    color += value;
    if (color >= 0x100) {
        color = 0xFF;
    } else if (color < 0) {
        color = 0;
    }
    return color;
}

void func_80021B04(void* arg0, s16 arg1, s16 arg2, s16 arg3) {
    *(s16*)((u8*)arg0 + 0x0) = arg1;
    *(s16*)((u8*)arg0 + 0x2) = arg2;
    *(s16*)((u8*)arg0 + 0x4) = arg3;
}

void func_80021B14(void* arg0, s32 arg1, s32 arg2, s32 arg3) {
    *(s32*)((u8*)arg0 + 0x0) = arg1;
    *(s32*)((u8*)arg0 + 0x4) = arg2;
    *(s32*)((u8*)arg0 + 0x8) = arg3;
}

void func_80021B24(void* arg0, void* arg1) {
    *(u16*)((u8*)arg0 + 0x0) = *(u16*)((u8*)arg1 + 0x0);
    *(u16*)((u8*)arg0 + 0x2) = *(u16*)((u8*)arg1 + 0x2);
    *(u16*)((u8*)arg0 + 0x4) = *(u16*)((u8*)arg1 + 0x4);
}

void func_80021B48(void* arg0, void* arg1) {
    *(s32*)((u8*)arg0 + 0x0) = *(s32*)((u8*)arg1 + 0x0);
    *(s32*)((u8*)arg0 + 0x4) = *(s32*)((u8*)arg1 + 0x4);
    *(s32*)((u8*)arg0 + 0x8) = *(s32*)((u8*)arg1 + 0x8);
}

void func_80021B6C(SpriteData* pSpriteData) {
    pSpriteData->prim |= 1;
    func_8001F6B0(pSpriteData);
}

void SpriteSetColor(SpriteData *pSpriteData, u8 red, u8 green, u8 blue) {
    pSpriteData->red = red;
    pSpriteData->green = green;
    pSpriteData->blue = blue;
    pSpriteData->prim &= 0xFE;
    func_8001F6B0(pSpriteData);
}

void func_80021BCC(void* arg0, u32 arg1) {
    u32* p = (u32*)((u8*)arg0 + 0xAC);
    *p = (*p & ~0x7FF80) | ((arg1 & 0xFFF) << 7);
}

void SpriteSetSpecialAnimFile(SpriteData* pSpriteData, void* pAnimFile) {
    pSpriteData->pSpecialAnimFile = pAnimFile;
}

void func_80021BF8(void* arg0, s32 arg1) {
    *(s32*)((u8*)arg0 + 0x68) = arg1;
}

void func_80021C00(void* arg0, u32 arg1) {
    u32* p = (u32*)((u8*)arg0 + 0x40);
    *p = (*p & ~0x1F00) | ((arg1 & 0x1F) << 8);
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/animation_scripts", AnimScriptStackPopU8);
/*
Matches on GCC 2.7.2-970404, ASPSX 2.67
Co-Authored-By: Mc-muffin <Mc-muffin@users.noreply.github.com>

u_char AnimScriptStackPopU8(SpriteData* pSpriteData) {
    u_char nStackValue;

    nStackValue = pSpriteData->stack[pSpriteData->stackIndex];
    pSpriteData->stackIndex++;
    return nStackValue;
}
*/

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/animation_scripts", AnimScriptStackPopU16);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/animation_scripts", AnimScriptStackPopU24);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/animation_scripts", AnimScriptStackPushU8);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/animation_scripts", AnimScriptStackPushU16);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/animation_scripts", AnimScriptStackPushU24);

void func_80021D3C(void* arg0, s32 arg1, s32 arg2) {
    *(s32*)((u8*)arg0 + 0x8) = arg2 << 16;
    *(s32*)((u8*)arg0 + 0x0) = arg1 << 16;
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/animation_scripts", func_80021D50);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/animation_scripts", func_80021EBC);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/animation_scripts", func_80021FB8);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/animation_scripts", func_80021FC0);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/animation_scripts", func_80021FE0);

void SpriteSetScale(SpriteData* pSpriteData, short scale) {
    SpriteDataI1* pBase = pSpriteData->pBase;
    if (pBase) {
        pSpriteData->scale = scale;
        pBase->scale.z = scale;
        pBase->scale.y = scale;
        pBase->scale.x = scale;
        pSpriteData->field_0x3C_6 = 0x1;
    }
}

// Recompute transform matrix if flag is set
void func_80022038(SpriteData* pSpriteData) {
    if (pSpriteData->field_0x3C_6 & 0x1) {
        SpriteComputeTransformMatrix(pSpriteData);
        pSpriteData->field_0x3C_6 = 0x0;
    }
}

extern MATRIX D_80018644;

void SpriteComputeTransformMatrix(SpriteData* pSpriteData) {
    VECTOR unused;
    VECTOR scale;
    VECTOR scale2;
    MATRIX rotationMatrix;
    MATRIX transformMatrix;
    
    if (!(pSpriteData->field_0x40 & 1)) {
        scale.vx = pSpriteData->pBase->scale.x;
        scale.vy = pSpriteData->pBase->scale.y;
        scale.vz = pSpriteData->pBase->scale.z;
        RotMatrix(&pSpriteData->pBase->rotation, &pSpriteData->pBase->transformMatrix);
        ScaleMatrixL(&pSpriteData->pBase->transformMatrix, &scale);
    } else {
        transformMatrix = D_80018644;
        scale2.vx = pSpriteData->pBase->scale.x;
        scale2.vy = pSpriteData->pBase->scale.y;
        scale2.vz = pSpriteData->pBase->scale.z;
        ScaleMatrixL(&transformMatrix, &scale2);
        RotMatrix(&pSpriteData->pBase->rotation, &rotationMatrix);
        MulMatrix0(&rotationMatrix, &transformMatrix, &pSpriteData->pBase->transformMatrix);
    }
    if (pSpriteData->field_0x3A) {
        scale.vx = pSpriteData->field_0x3A >> 1;
        scale.vy = pSpriteData->field_0x3A >> 1;
        scale.vz = pSpriteData->field_0x3A >> 1;
        ScaleMatrix(&pSpriteData->pBase->transformMatrix, &scale);
    }
}

// Set animation package
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/animation_scripts", func_80022224);

extern s32 func_8001EE68(void*);
extern void func_80022224(SpriteAnimPackage*, void*, SVEC2, SVEC2, u32);
extern u8 D_800591AD;

void func_800222BC(SpriteData* pSprite, SpriteAnimPackageFileHeader* pAnimPackageFile) {
    SpriteAnimPackage* pAnimPackage;
    SVEC2 unused[2];

    pAnimPackage = pSprite->pVramData;
    if (pAnimPackageFile) {
        if (pAnimPackageFile != pSprite->pCurAnimFile) {
            func_80022224(pAnimPackage, pAnimPackageFile, pAnimPackage->tex, pAnimPackage->clut, pSprite->field_0x3C_1);
            pSprite->pCurAnimFile = pAnimPackageFile;
            pSprite->field_0x3C_3 = 1;
        }

        // Is battle?
        if (D_800591AD) {
            // Is not VRAM Pre-backed?
            if (!func_8001EE68(pAnimPackage->pFrames)) {
                pAnimPackage->tex.y = 0x100;
                pAnimPackage->tex.x = 0x300;
                return;
            }
            pAnimPackage->tex = *(SVEC2*)(pSprite->field_0x7C + 0xE);
        }
    }
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/animation_scripts", func_800223B0);

// Run Sprite Animation VM
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/animation_scripts", func_80022660);

// Recompute animation speed
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/animation_scripts", func_80022974);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/animation_scripts", func_80022A00);
