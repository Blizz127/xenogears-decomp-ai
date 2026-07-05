#include "common.h"
#include "field/actor.h"
#ifdef XENO_PC_PORT
#include <assert.h>
#else
/* <assert.h> is unavailable under the matching build's -nostdinc MIPS
 * preprocessor. The assert(0) calls below mark unimplemented paths in
 * functions not yet byte-matched, so a no-op assert compiles safely there.
 * (uintptr_t comes from include/types.h for both builds.) */
#define assert(x) ((void)0)
#endif

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

void func_8001FBE4(void* pSpriteData, u32 opcodeIndex, void* operands) {
    u32 dispatchIndex = (u8)opcodeIndex - 0x8A;

    (void)pSpriteData;
    (void)operands;

    if (dispatchIndex >= 0x73) {
        return;
    }

    if (dispatchIndex == 0x28) {
        return;
    }

    assert(0 && "func_8001FBE4 dispatch path is not implemented");
}

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

void func_80021EBC(void* arg0, void* arg1) {
    u8* src = arg0;
    u8* dst = arg1;
    u8* src7C = (u8*)(uintptr_t)*(u32*)(src + 0x7C);
    u8* src20 = (u8*)(uintptr_t)*(u32*)(src + 0x20);
    s32 flagsA8 = *(s32*)(src + 0xA8);

    *(s32*)(dst + 0x00) = *(s32*)(src + 0x00);
    *(s32*)(dst + 0x04) = *(s32*)(src + 0x04);
    *(s32*)(dst + 0x08) = *(s32*)(src + 0x08);
    *(s16*)(dst + 0x10) = *(u16*)(src + 0x80);
    *(s16*)(dst + 0x12) = (flagsA8 >> 11) & 0x3F;
    *(s16*)(dst + 0x14) = (s8)*(u8*)(src + 0xAF);
    *(s16*)(dst + 0x16) = (s8)*(u8*)(src + 0xB0);
    *(s16*)(dst + 0x18) = (flagsA8 >> 22) & 0x3F;
    *(s32*)(dst + 0x1C) = *(s32*)(src7C + 0x00);
    *(s32*)(dst + 0x20) = *(s32*)(src7C + 0x04);
    *(s16*)(dst + 0x24) = *(u16*)(src20 + 0x06);
    *(s16*)(dst + 0x26) = *(u16*)(src20 + 0x08);
    *(s16*)(dst + 0x28) = *(u16*)(src20 + 0x0A);
    *(s16*)(dst + 0x2C) = *(u16*)(src + 0x82);
    *(s16*)(dst + 0x2A) = *(u16*)(src + 0x2C);
}

void func_80021FB8(u8* arg0, u8 arg1) {
    arg0[0xB0] = arg1;
}

extern void func_80022974(void* arg0);

void func_80021FC0(void* a0, s32 a1) {
    *(s32*)((u8*)a0 + 0x18) = a1;
    func_80022974(a0);
}

void func_80021FE0(void* a0, s16 a1) {
    *(s16*)((u8*)a0 + 0x32) = a1;
    func_80022974(a0);
}

void SpriteSetScale(SpriteData* pSpriteData, short scale) {
    u8* pData = (u8*)pSpriteData;
    u8* pBase = (u8*)(uintptr_t)*(u32*)(pData + 0x20);

    if (pBase) {
        *(s16*)(pData + 0x2C) = scale;
        *(s16*)(pBase + 0xA) = scale;
        *(s16*)(pBase + 0x8) = scale;
        *(s16*)(pBase + 0x6) = scale;
        *(u32*)(pData + 0x3C) |= 0x10000000;
    }
}

// Recompute transform matrix if flag is set
void func_80022038(SpriteData* pSpriteData) {
    u8* pData = (u8*)pSpriteData;

    if ((*(u32*)(pData + 0x3C) >> 28) & 0x1) {
        SpriteComputeTransformMatrix(pSpriteData);
        *(u32*)(pData + 0x3C) &= ~0x10000000;
    }
}

extern MATRIX D_80018644;

void SpriteComputeTransformMatrix(SpriteData* pSpriteData) {
    u8* pData = (u8*)pSpriteData;
    u8* pBase = (u8*)(uintptr_t)*(u32*)(pData + 0x20);
    VECTOR scale;
    VECTOR scale2;
    MATRIX rotationMatrix;
    MATRIX transformMatrix;
    
    if (!(*(u32*)(pData + 0x40) & 1)) {
        scale.vx = *(s16*)(pBase + 0x6);
        scale.vy = *(s16*)(pBase + 0x8);
        scale.vz = *(s16*)(pBase + 0xA);
        RotMatrix((SVECTOR*)pBase, (MATRIX*)(pBase + 0xC));
        ScaleMatrixL((MATRIX*)(pBase + 0xC), &scale);
    } else {
        transformMatrix = D_80018644;
        scale2.vx = *(s16*)(pBase + 0x6);
        scale2.vy = *(s16*)(pBase + 0x8);
        scale2.vz = *(s16*)(pBase + 0xA);
        ScaleMatrixL(&transformMatrix, &scale2);
        RotMatrix((SVECTOR*)pBase, &rotationMatrix);
        MulMatrix0(&rotationMatrix, &transformMatrix, (MATRIX*)(pBase + 0xC));
    }
    if (*(u16*)(pData + 0x3A)) {
        scale.vx = *(u16*)(pData + 0x3A) >> 1;
        scale.vy = *(u16*)(pData + 0x3A) >> 1;
        scale.vz = *(u16*)(pData + 0x3A) >> 1;
        ScaleMatrix((MATRIX*)(pBase + 0xC), &scale);
    }
}

extern u8 D_800591AD;
extern u8 D_800591B0;
extern u8 D_800591B3;

extern s32 func_8001EE68(void*);

// Set animation package
void func_80022224(SpriteAnimPackage* pAnimPackage, void* pAnimPackageFile, SVEC2 tex, SVEC2 clut, u32 arg4) {
    u8* pPackage = (u8*)pAnimPackage;
    u8* pFile = pAnimPackageFile;

    *(SVEC2*)(pPackage + 0x4) = tex;
    *(SVEC2*)(pPackage + 0x8) = clut;
    *(u32*)(pPackage + 0xC) = (u32)(uintptr_t)(pFile + *(u32*)(pFile + 0xC));
    *(u32*)(pPackage + 0x0) = (u32)(uintptr_t)(pFile + *(u32*)(pFile + 0x8));
    D_800591B0 = 0;
    *(u32*)(pPackage + 0x10) = (u32)(uintptr_t)(pFile + *(u32*)(pFile + 0x4));

    if (D_800591AD) {
        u8 numAnimations = (*(u16*)(uintptr_t)*(u32*)(pPackage + 0x10) >> 6) & 0x3F;
        if (numAnimations != 0) {
            D_800591B3 = numAnimations;
        }
    }
}

void func_800222BC(SpriteData* pSprite, SpriteAnimPackageFileHeader* pAnimPackageFile) {
    u8* pSpriteData = (u8*)pSprite;
    u8* pAnimPackage;

    pAnimPackage = (u8*)(uintptr_t)*(u32*)(pSpriteData + 0x24);
    if (pAnimPackageFile) {
        if (pAnimPackageFile != (void*)(uintptr_t)*(u32*)(pSpriteData + 0x44)) {
            func_80022224((SpriteAnimPackage*)pAnimPackage,
                          pAnimPackageFile,
                          *(SVEC2*)(pAnimPackage + 0x4),
                          *(SVEC2*)(pAnimPackage + 0x8),
                          (*(u32*)(pSpriteData + 0x3C) >> 20) & 0xF);
            *(u32*)(pSpriteData + 0x44) = (u32)(uintptr_t)pAnimPackageFile;
            *(u32*)(pSpriteData + 0x3C) |= 0x40000000;
        }

        // Is battle?
        if (D_800591AD) {
            // Is not VRAM Pre-backed?
            if (!func_8001EE68((void*)(uintptr_t)*(u32*)(pAnimPackage + 0x0))) {
                *(s16*)(pAnimPackage + 0x6) = 0x100;
                *(s16*)(pAnimPackage + 0x4) = 0x300;
                return;
            }
            *(u32*)(pAnimPackage + 0x4) = *(u32*)((u8*)(uintptr_t)*(u32*)(pSpriteData + 0x7C) + 0xE);
        }
    }
}

extern void func_80022660(void* pSpriteData, void* pBytecode, s32 arg2);
extern void func_80022D44(void* pSpriteData);

void func_800223B0(void* pSpriteData, s16 arg1) {
    u8* pData = pSpriteData;
    u8* pAnim;
    s32 oldDirection;
    s32 direction;
    s32 mode;
    u32 flagsA8;
    u32 flagsAC;
    u32 flags3C;

    *(s16*)(pData + 0x80) = arg1;
    oldDirection = (*(u32*)(pData + 0xA8) >> 17) & 0x7;

    if (((arg1 + 0x400) & 0x1) != 0) {
        *(u32*)(pData + 0xAC) |= 0x4;
    } else {
        *(u32*)(pData + 0xAC) &= ~0x4;
    }

    if (*(u32*)(pData + 0x48) == 0) {
        return;
    }

    mode = (*(u32*)(pData + 0xA8) >> 20) & 0x3;
    pAnim = (u8*)(uintptr_t)*(u32*)(pData + 0x58);

    if (mode == 0) {
        if (((arg1 + 0x400) & 0xFFF) < 0x801) {
            *(u32*)(pData + 0xAC) &= ~0x4;
        } else {
            *(u32*)(pData + 0xAC) |= 0x4;
        }

        *(u32*)(pData + 0xA8) &= 0xFFF1FFFF;
        *(u32*)(pData + 0x5C) = (u32)(uintptr_t)(pAnim + 0x6);
        *(u32*)(pData + 0x54) = (u32)(uintptr_t)(pAnim + *(u16*)(pAnim + 0x4) + 0x4);
    } else if (mode == 1) {
        direction = ((arg1 + 0x600) >> 10) & 0x3;
        if (direction < 3) {
            *(u32*)(pData + 0xAC) &= ~0x4;
            *(u32*)(pData + 0x54) =
                (u32)(uintptr_t)(pAnim + (direction * 2) + 0x4 +
                                 *(u16*)(pAnim + (direction * 2) + 0x4));
        } else {
            *(u32*)(pData + 0xAC) |= 0x4;
            *(u32*)(pData + 0x54) = (u32)(uintptr_t)(pAnim + *(u16*)(pAnim + 0x6) + 0x6);
        }

        *(u32*)(pData + 0xA8) =
            (*(u32*)(pData + 0xA8) & 0xFFF1FFFF) | ((direction & 0x7) << 17);
    } else if (mode == 2) {
        direction = ((arg1 + 0x500) >> 9) & 0x7;
        if (direction < 5) {
            *(u32*)(pData + 0xAC) &= ~0x4;
            *(u32*)(pData + 0x54) =
                (u32)(uintptr_t)(pAnim + (direction * 2) + 0x4 +
                                 *(u16*)(pAnim + (direction * 2) + 0x4));
        } else {
            direction = (direction - 5) ^ 0x3;
            *(u32*)(pData + 0xAC) |= 0x4;
            *(u32*)(pData + 0x54) =
                (u32)(uintptr_t)(pAnim + (direction * 2) + 0x4 +
                                 *(u16*)(pAnim + (direction * 2) + 0x4));
        }

        *(u32*)(pData + 0xA8) =
            (*(u32*)(pData + 0xA8) & 0xFFF1FFFF) | ((direction & 0x7) << 17);
    }

    flagsA8 = *(u32*)(pData + 0xA8);
    if (oldDirection != ((flagsA8 >> 17) & 0x7)) {
        void* pOldBytecode = (void*)(uintptr_t)*(u32*)(pData + 0x64);
        s16 waitTimer = *(s16*)(pData + 0x9E);

        *(u32*)(pData + 0xA8) = (flagsA8 | 0x1F800) & 0xF03FFFFF;
        pAnim = (u8*)(uintptr_t)*(u32*)(pData + 0x58);
        *(u32*)(pData + 0x64) = (u32)(uintptr_t)(pAnim + *(u16*)(pAnim + 0x2) + 0x2);
        func_80022660(pData, pOldBytecode, (flagsA8 >> 22) & 0x3F);
        *(s16*)(pData + 0x9E) = waitTimer;
    }

    flags3C = *(u32*)(pData + 0x3C) & ~0x8;
    flagsAC = *(u32*)(pData + 0xAC);
    flags3C |= ((((flagsAC >> 3) & 0x1) ^ ((flagsAC >> 2) & 0x1)) << 3);
    *(u32*)(pData + 0x3C) = flags3C;
}

// Run Sprite Animation VM
void func_80022660(void* pSpriteData, void* pBytecode, s32 arg2) {
    u8* pData = pSpriteData;

    while (1) {
        u8* pc = (u8*)(uintptr_t)*(u32*)(pData + 0x64);
        u8 opcode;
        s32 delay;
        u32 flags;
        u32 subIndex;

        if (pc == pBytecode && ((*(u32*)(pData + 0xA8) >> 22) & 0x3F) == (u32)arg2) {
            return;
        }

        opcode = *pc;
        if (opcode >= 0x10 && opcode < 0x20) {
            *(u32*)(pData + 0x64) = (u32)(uintptr_t)(pc + 1);
            flags = *(u32*)(pData + 0xA8);
            flags = (flags & 0xFFFE07FF) |
                    (((((flags >> 11) & 0x3F) + 1) & 0x3F) << 11);
            *(u32*)(pData + 0xA8) = flags;
            func_80022D44(pData);

            delay = (opcode & 0xF) + 1;
            *(s16*)(pData + 0x9E) = *(u16*)(pData + 0x9E) + delay;

            flags = *(u32*)(pData + 0xA8) & 0xF03FFFFF;
            subIndex = (((*(u32*)(pData + 0xA8) >> 22) & 0x3F) + 1) & 0x3F;
            *(u32*)(pData + 0xA8) = flags | (subIndex << 22);

            if (subIndex == 0) {
                flags = *(u32*)(pData + 0xA8) & 0xF03FFFFF;
                subIndex = (((*(u32*)(pData + 0xA8) >> 22) & 0x3F) - 1) & 0x3F;
                *(u32*)(pData + 0xA8) = flags | (subIndex << 22);
            }
            continue;
        }

        if (opcode >= 0x30 && opcode < 0x40) {
            *(u32*)(pData + 0x64) = (u32)(uintptr_t)(pc + 1);
            delay = (opcode & 0xF) + 1;
            *(s16*)(pData + 0x9E) = *(u16*)(pData + 0x9E) + delay;

            flags = *(u32*)(pData + 0xA8) & 0xF03FFFFF;
            subIndex = (((*(u32*)(pData + 0xA8) >> 22) & 0x3F) + 1) & 0x3F;
            *(u32*)(pData + 0xA8) = flags | (subIndex << 22);

            if (subIndex == 0) {
                flags = *(u32*)(pData + 0xA8) & 0xF03FFFFF;
                subIndex = (((*(u32*)(pData + 0xA8) >> 22) & 0x3F) - 1) & 0x3F;
                *(u32*)(pData + 0xA8) = flags | (subIndex << 22);
            }
            continue;
        }

        if (opcode >= 0x80 && opcode < 0x83) {
            return;
        }

        if (opcode == 0xB4) {
            *(u32*)(pData + 0x64) = (u32)(uintptr_t)(pc + 4);
            continue;
        }

        if (opcode == 0xB2) {
            *(u32*)(pData + 0x64) = (u32)(uintptr_t)(pc + 2);
            continue;
        }

        assert(0 && "func_80022660 bytecode path is not implemented");
    }
}

// Recompute animation speed
void func_80022974(void* pSpriteData) {
    u8* pData = pSpriteData;
    s32 radius = (*(s32*)(pData + 0x18) >> 4) << 8;
    s32 divisor = (*(u32*)(pData + 0xAC) >> 7) & 0xFFF;
    s32 scaledRadius = radius / divisor;
    s16 angle = *(s16*)(pData + 0x32);
    s32 sinValue;
    s32 cosValue;

    sinValue = rsin(angle) >> 2;
    *(s32*)(pData + 0x0C) = (sinValue * scaledRadius) >> 6;

    cosValue = rcos(angle) >> 2;
    *(s32*)(pData + 0x14) = -(cosValue * scaledRadius) >> 6;
}

s32 func_80022A00(s32* arg0) {
    return *arg0;
}
