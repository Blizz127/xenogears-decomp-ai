#include "common.h"
#ifdef XENO_PC_PORT
#include <assert.h>
#else
/* <assert.h> is unavailable under the matching build's -nostdinc MIPS
 * preprocessor. The assert(0) below marks an unimplemented path in a function
 * not yet byte-matched, so a no-op assert compiles safely there. (uintptr_t
 * comes from include/types.h for both builds.) */
#define assert(x) ((void)0)
#endif
#include "field/actor.h"
#include "psyq/libgpu.h"

// Sprite / Animation functions

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1", func_80022B2C);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1", func_80022CAC);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1", func_80022CDC);

extern void func_8001D2B0(void* pSpriteData, s16 frameIndex);

void func_80022D44(void* pSpriteData) {
    u8* pData = pSpriteData;
    u32 flagsA8 = *(u32*)(pData + 0xA8);
    u16* pFrameList = (u16*)(uintptr_t)*(u32*)(pData + 0x54);
    s32 frameIndex = (flagsA8 >> 11) & 0x3F;
    u16 frame = pFrameList[frameIndex];
    s32 tileIndex = frame & 0x1FF;
    u32 flagsAC = *(u32*)(pData + 0xAC);
    u32 flags3C;

    if (frame & 0x200) {
        flagsAC |= 0x8;
    } else {
        flagsAC &= ~0x8;
    }
    *(u32*)(pData + 0xAC) = flagsAC;

    flags3C = *(u32*)(pData + 0x3C) & ~0x8;
    flags3C |= ((((flagsAC >> 3) & 0x1) ^ ((flagsAC >> 2) & 0x1)) << 3);
    *(u32*)(pData + 0x3C) = flags3C;

    func_8001D2B0(pData, tileIndex);
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1", func_80022DF4);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1", func_80022E8C);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1", func_80022EB8);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1", func_80022FC4);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1", func_8002303C);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1", func_800230A8);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1", func_80023124);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1", func_80023170);

s32 func_800231E0(u8* a0) {
    s32 p = *(s32*)(a0 + 0xC);
    p += (s32)a0;
    return *(s32*)p;
}

s32 func_800231F8(u8* a0) {
    s32 p = *(s32*)(a0 + 0xC);
    p += (s32)a0;
    return *(s32*)(p + 4) + 1;
}

extern s32 D_80059198;
extern void func_800248D4(void* pSpriteData);

void AnimScriptTick(void* pSpriteData) {
    u8* pData = pSpriteData;
    s32 i;

    for (i = 0; i != D_80059198 + 1; i++) {
        s16 waitTimer = *(s16*)(pData + 0x9E);

        if (waitTimer != 0) {
            waitTimer--;
            *(s16*)(pData + 0x9E) = waitTimer;

            if (waitTimer == 0) {
                func_800248D4(pData);
            }
        }
    }
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1", func_80023290);

void func_80023340(void* pSpriteData, s32 count) {
    u8* pData = pSpriteData;
    u8* pBase = (u8*)(uintptr_t)*(u32*)(pData + 0x20);
    void* pFramesData;

    HeapFree((void*)(uintptr_t)*(u32*)(pBase + 0x2C));
    pFramesData = HeapAlloc(count * 0x18, 0);
    *(u32*)(pBase + 0x30) = (u32)(uintptr_t)pFramesData;
    *(u32*)(pBase + 0x2C) = (u32)(uintptr_t)pFramesData;
}

// Allocate struct stuff
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1", func_800233A4);
/*
Matches on  GCC 2.7.2-970404, ASPSX 2.67

typedef struct {
    WorkListEntry task1;
    WorkListEntry task2;
    SpriteData spriteData;
} AnimTask;

extern u8 D_800591AF;
extern WorkListCallback_t func_80022DF4[];
extern WorkListCallback_t func_80022EB8[];

AnimTask* func_800233A4(void* pData, int dataSize) {
    AnimTask* pEntry;
    WorkListEntry* pTask1;
    WorkListEntry* pTask2;

    pEntry = HeapAlloc(dataSize + 0xEC, D_800591AF);
    pTask1 = &pEntry->task1;
    pTask2 = &pEntry->task2;
    TimerWorkListAddTask(pData, pTask1);
    WorkListAddTask(pEntry, pTask2);
    func_80023804(&pEntry->spriteData);
    pTask1->unk4 = &pEntry->spriteData;
    pTask2->unk4 = &pEntry->spriteData;
    TimerWorkListSetTaskCallback(pEntry, &func_80022DF4);
    WorkListTaskSetOnFreeCallback(pEntry, &func_80022EB8);
    return pEntry;
}
*/

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1", func_80023440);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1", func_80023468);

void func_800234AC(void* pSpriteData) {
    u8* pData = pSpriteData;
    u8* pBase = (u8*)(uintptr_t)*(u32*)(pData + 0x20);
    u8* pDirTransforms = (u8*)(uintptr_t)*(u32*)(pBase + 0x34);
    s32 i;

    for (i = 0; i < 8; i++) {
        u8* pDir = pDirTransforms + i * 8;

        *(u8*)(pDir + 0x0) = 0;
        *(u8*)(pDir + 0x1) = 0;
        *(s16*)(pDir + 0x2) = 0;
        *(s16*)(pDir + 0x4) = 0;
        *(s16*)(pDir + 0x6) = 0;
    }
}

extern s32 D_800591A8;
extern s32 D_80059198;
extern u8 D_800591AD;
extern void SpriteComputeTransformMatrix(void* pSpriteData);

void func_80023538(void* pSpriteData, void* pAnimation) {
    u8* pData = pSpriteData;
    u8* pAnim = pAnimation;
    u8* pBase;
    u8* pState;
    s32 flags0;
    s32 signedValue;
    s32 work;
    s32 tmp;
    s32 denom;
    s32 scale;
    u32 flagsA8;

    *(u32*)(pData + 0x58) = (u32)(uintptr_t)pAnim;
    *(u32*)(pData + 0x64) = (u32)(uintptr_t)(pAnim + *(u16*)(pAnim + 0x2) + 0x2);

    flagsA8 = *(u32*)(pData + 0xA8);
    flagsA8 = (flagsA8 & 0xFFCFFFFF) | ((*(u16*)pAnim & 0x3) << 20);
    *(u32*)(pData + 0xA8) = flagsA8;

    *(u32*)(pData + 0x54) = (u32)(uintptr_t)(pAnim + *(u16*)(pAnim + 0x4) + 0x4);

    flags0 = *(u16*)pAnim;
    signedValue = (flags0 >> 2) & 0x3F;
    if (signedValue & 0x20) {
        signedValue |= ~0x3F;
    }

    work = D_80059198 + 1;
    tmp = work * work * *(s16*)(pData + 0x82);
    scale = signedValue << 10;
    *(s32*)(pData + 0x1C) = scale;
    if (tmp < 0) {
        tmp += 0xFFF;
    }
    tmp = scale * (tmp >> 12);

    denom = (*(u32*)(pData + 0xAC) >> 7) & 0xFFF;
    tmp = tmp * (((0x10000 / denom) * (0x10000 / denom)) >> 8);
    if (tmp < 0) {
        tmp += 0xFF;
    }
    *(s32*)(pData + 0x1C) = tmp >> 8;

    if (!((flags0 >> 11) & 0x1)) {
        *(u32*)(pData + 0x14) = 0;
        *(u32*)(pData + 0x10) = 0;
        *(u32*)(pData + 0x0C) = 0;
        *(u32*)(pData + 0x18) = 0;
    }

    pBase = (u8*)(uintptr_t)*(u32*)(pData + 0x20);
    if (pBase != NULL) {
        if (!((flags0 >> 12) & 0x1)) {
            *(s16*)(pBase + 0x4) = 0;
            *(s16*)(pBase + 0x0) = 0;
            *(s16*)(pBase + 0x2) = 0;
            SpriteComputeTransformMatrix(pData);
        }

        if (!((flags0 >> 13) & 0x1)) {
            if (D_800591AD) {
                SpriteSetScale((SpriteData*)pData, D_800591A8);
            }
        } else if (D_800591AD) {
            SpriteComputeTransformMatrix(pData);
        }

        if ((*(u32*)(pData + 0x3C) & 0x3) == 1) {
            *(u8*)(pBase + 0x3D) = 0;
            *(u8*)(pBase + 0x3C) = 0;

            if (((*(u32*)(pData + 0x40) >> 20) & 0x1) == 0 &&
                *(u32*)(pBase + 0x34) != 0) {
                func_800234AC(pData);
            }
        }
    }

    *(u8*)(pData + 0x8C) = 0x10;
    *(s16*)(pData + 0x30) = 0;
    flagsA8 = *(u32*)(pData + 0xA8);
    flagsA8 &= 0xFFFFF801;
    flagsA8 &= 0xF03FFFFF;
    flagsA8 &= 0xCFFFFFFF;
    flagsA8 |= 0x20000000;
    flagsA8 |= 0x1F800;
    *(u32*)(pData + 0xA8) = flagsA8;
    *(s16*)(pData + 0x9E) = 1;

    pState = (u8*)(uintptr_t)*(u32*)(pData + 0x7C);
    if (pState != NULL && (flagsA8 & 0x1) == 1) {
        *(u32*)(pState + 0x4) = 0;
        *(u32*)(pState + 0x0) = 0;
        *(s16*)(pState + 0xC) = 0;
    }
}

// SpriteData stuff
extern s32 D_80059198;

void func_80023804(void* pSpriteData) {
    u8* pData = pSpriteData;
    s32 work;
    s32 scale;

    *(s32*)(pData + 0x3C) = 0;
    *(u8*)(pData + 0x2B) = 0x2D;
    *(s32*)(pData + 0x40) = 0;
    *(s16*)(pData + 0x3A) = 0;
    *(s16*)(pData + 0x30) = 0;
    *(s16*)(pData + 0x32) = 0;
    *(s16*)(pData + 0x34) = 0;
    *(s32*)(pData + 0xA8) = 0;

    *(s32*)(pData + 0x3C) &= ~0xF10001C;
    *(s32*)(pData + 0x40) &= ~0x1EFC;
    *(s32*)(pData + 0xA8 + 0x04) = 0;
    *(s32*)(pData + 0xA8 + 0x08) = 0;
    *(u8*)(pData + 0xB0) = 0;
    *(u8*)(pData + 0xAF) = 0;

    work = D_80059198 + 1;
    scale = (work * (work << 14) * *(s16*)(pData + 0x82)) >> 12;
    *(s32*)(pData + 0xAC) = (*(s32*)(pData + 0xAC) & 0xFFF8007F) | 0x8000;
    *(s32*)(pData + 0xA8) &= 0xFFF1FFFF;
    *(s32*)(pData + 0x1C) = scale;

    *(s32*)(pData + 0x64) = 0;
    *(s32*)(pData + 0x70) = 0;
    *(s32*)(pData + 0x44) = 0;
    *(s32*)(pData + 0x68) = 0;
    *(s16*)(pData + 0x80) = 0;
    *(u8*)(pData + 0x8C) = 0x10;
    *(s16*)(pData + 0x84) = 0;
    *(s32*)(pData + 0x6C) = 0;
    *(s32*)(pData + 0x50) = 0;
}



void func_8002393C(void* arg0) {
    *(s16*)((u8*)arg0 + 0x0) = 0;
    *(s16*)((u8*)arg0 + 0x2) = 0;
    *(s16*)((u8*)arg0 + 0x4) = 0;
    *(s32*)((u8*)arg0 + 0x2C) = 0;
}

void func_80023950(void* arg0) {
    *(s32*)((u8*)arg0 + 0x20) = 0;
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1", func_80023958);

void func_800239A0(void* pSpriteData) {
    u8* pData = pSpriteData;
    u8* pBase = pData + 0xB4;

    *(u32*)(pData + 0x20) = (u32)(uintptr_t)pBase;
    func_8002393C(pBase);
    *(u32*)(pData + 0x7C) = (u32)(uintptr_t)(pData + 0xF4);
    *(u32*)(pBase + 0x34) = (u32)(uintptr_t)(pData + 0x124);
    *(u32*)(pData + 0x24) = (u32)(uintptr_t)(pData + 0x110);
    *(u32*)(pBase + 0x38) = 0;
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1", func_800239F4);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1", func_80023A48);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1", func_80023B84);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1", func_80023FD8);

extern s32 D_800591B8;
extern void* func_80024524(void* pAnimPackage, s16 texX, s16 texY, s16 clutX, s16 clutY, s16 arg5);

void* func_80024294(void* pAnimPackage, s16 texX, s16 texY, s16 clutX, s16 clutY, s16 arg5, s32 arg6) {
    void* pSpriteData;

    D_800591B8 = arg6;
    pSpriteData = func_80024524(pAnimPackage, texX, texY, clutX, clutY, arg5);
    D_800591B8 = 0;

    return pSpriteData;
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1", func_800242F4);

extern u8 D_800591AD;
extern s32 func_8001EE74(void* arg0);
extern void func_800222BC(void* pSpriteData, void* pAnimPackageFile);
extern void func_800245D8(void* pSpriteData, s16 animIndex);

void* func_8002435C(void* pSpriteData, void* pAnimPackage, s16 texX, s16 texY, s16 clutX, s16 clutY, s16 arg6) {
    u8* pData = pSpriteData;
    u8* pPackage = pAnimPackage;
    u8* pBase;
    u8* pVramData;
    u8* pAnimations;
    u8* pFrameWorkData;
    s32 frameCount;
    s32 packed;

    func_80023804(pData);
    func_800239A0(pData);
    SpriteSetScale((SpriteData*)pData, 0x1000);

    *(u32*)(pData + 0x3C) = (*(u32*)(pData + 0x3C) & ~0x3) | 0x1;
    *(u32*)(pData + 0x40) &= 0xFFFE1FFF;

    if (D_800591AD) {
        pVramData = (u8*)(uintptr_t)*(u32*)(pData + 0x7C);
        *(u32*)(pData + 0xA8) &= ~0x1;
        *(u32*)(pVramData + 0x8) = 0;
        *(u16*)(pVramData + 0xC) = 0;
    } else {
        pVramData = (u8*)(uintptr_t)*(u32*)(pData + 0x7C);
        *(u32*)(pData + 0xA8) |= 0x1;
        *(u32*)(pVramData + 0x18) = 0;
    }

    *(u32*)(pData + 0x6C) = (u32)(uintptr_t)pData;
    packed = D_800591B8 & 0xF;
    *(u32*)(pData + 0x3C) = (*(u32*)(pData + 0x3C) & 0xFF0FFFFF) | (packed << 20);
    *(u32*)(pData + 0x3C) = (*(u32*)(pData + 0x3C) & 0xFFF0FFFF) | (packed << 16);

    frameCount = func_8001EE74(pPackage + *(u32*)(pPackage + 0x8));
    pFrameWorkData = HeapAlloc(frameCount * 0x18, 0);
    pBase = (u8*)(uintptr_t)*(u32*)(pData + 0x20);
    *(u32*)(pBase + 0x2C) = (u32)(uintptr_t)pFrameWorkData;
    *(u32*)(pBase + 0x30) = (u32)(uintptr_t)pFrameWorkData;

    pVramData = (u8*)(uintptr_t)*(u32*)(pData + 0x24);
    *(s16*)(pVramData + 0x4) = clutX;
    *(s16*)(pVramData + 0x6) = clutY;
    *(s16*)(pVramData + 0x8) = texX;
    *(s16*)(pVramData + 0xA) = texY;

    *(u32*)(pData + 0x48) = (u32)(uintptr_t)pPackage;
    func_800222BC(pData, pPackage);

    pVramData = (u8*)(uintptr_t)*(u32*)(pData + 0x24);
    pAnimations = (u8*)(uintptr_t)*(u32*)(pVramData + 0x10);
    *(u32*)(pData + 0x60) =
        (u32)(uintptr_t)(pAnimations + (((*(u16*)pAnimations & 0x3F) + 1) << 1));

    func_800245D8(pData, 0);

    return pData;
}

void* func_80024524(void* pAnimPackage, s16 texX, s16 texY, s16 clutX, s16 clutY, s16 arg5) {
    void* pSpriteData;

    pSpriteData = HeapAlloc(0x164, 0);
    *(s16*)((u8*)pSpriteData + 0x86) = 0x164;

    return func_8002435C(pSpriteData, pAnimPackage, texX, texY, clutX, clutY, arg5);
}

extern s32 func_8001EE68(void* arg0);
extern void func_80023538(void* pSpriteData, void* pAnimation);
extern void func_800223B0(void* pSpriteData, s16 arg1);

void func_800245D8(void* pSpriteData, s16 animIndex) {
    u8* pData = pSpriteData;
    u8* pDefaultAnimFile;
    u8* pVramData;
    u8* pAnimations;
    u8* pAnimation;
    s32 flags;

    pDefaultAnimFile = (u8*)(uintptr_t)*(u32*)(pData + 0x48);
    if (pDefaultAnimFile == NULL) {
        *(u32*)(pData + 0x64) = 0;
        return;
    }

    flags = *(u32*)(pData + 0xB0);
    if ((void*)(uintptr_t)*(u32*)(pData + 0x44) == pDefaultAnimFile) {
        flags &= ~0x400;
    } else {
        flags |= 0x400;
    }
    *(u32*)(pData + 0xB0) = flags;

    if (animIndex < 0) {
        func_800222BC(pData, (void*)(uintptr_t)*(u32*)(pData + 0x4C));

        if (D_800591AD) {
            pVramData = (u8*)(uintptr_t)*(u32*)(pData + 0x24);
            if (!func_8001EE68((void*)(uintptr_t)*(u32*)(pVramData + 0x0))) {
                *(s16*)(pVramData + 0x6) = 0x100;
                *(s16*)(pVramData + 0x4) = 0x300;
            }
        }
    } else {
        func_800222BC(pData, pDefaultAnimFile);

        if (D_800591AD) {
            pVramData = (u8*)(uintptr_t)*(u32*)(pData + 0x24);
            *(u32*)(pVramData + 0x4) = *(u32*)((u8*)(uintptr_t)*(u32*)(pData + 0x7C) + 0xE);
        }
    }

    *(s8*)(pData + 0xAF) = (s8)animIndex;
    if (animIndex < 0) {
        animIndex = ~animIndex;
    }

    pVramData = (u8*)(uintptr_t)*(u32*)(pData + 0x24);
    pAnimations = (u8*)(uintptr_t)*(u32*)(pVramData + 0x10);
    pAnimation = pAnimations + *(u16*)(pAnimations + (animIndex * 2) + 0x2);

    *(u32*)(pData + 0x40) |= 0x100000;
    *(u32*)(pData + 0x58) = (u32)(uintptr_t)pAnimation;
    func_80023538(pData, pAnimation);
    func_800223B0(pData, *(s16*)(pData + 0x80));
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1", func_80024730);

extern u8 D_800591AD;
extern s32 g_WorkListCurTimer;
extern void func_800C11CC(void);
extern void func_80022D44(void* pSpriteData);
extern void func_8001FBE4(void* pSpriteData, u32 opcodeIndex, void* operands);

void func_800248D4(void* pSpriteData) {
    u8* pData = pSpriteData;
    u8* pc;
    u8 opcode;

    if (D_800591AD) {
        func_800C11CC();
        return;
    }

    if (*(s16*)(pData + 0x9E) != 0) {
        return;
    }

    pc = (u8*)(uintptr_t)*(u32*)(pData + 0x64);
    opcode = *pc;

    if (opcode < 0x10) {
        s32 delay = (opcode & 0xF) + 1;
        s32 speed = (*(u32*)(pData + 0xAC) >> 7) & 0xFFF;
        s32 scaledDelay;
        u32 flags;
        u32 subIndex;

        *(u32*)(pData + 0x64) = (u32)(uintptr_t)(pc + 1);
        func_8001D2B0(pData, *(u16*)(pData + 0x34) + 1);

        scaledDelay = delay * speed;
        if (scaledDelay < 0) {
            scaledDelay += 0xFF;
        }
        delay = scaledDelay >> 8;
        if (delay == 0) {
            delay = 1;
        }

        flags = *(u32*)(pData + 0xA8) & 0xF03FFFFF;
        subIndex = (((*(u32*)(pData + 0xA8) >> 22) & 0x3F) + 1) & 0x3F;
        *(s16*)(pData + 0x9E) = *(u16*)(pData + 0x9E) + delay;
        *(u32*)(pData + 0xA8) = flags | (subIndex << 22);

        if (subIndex == 0) {
            flags = *(u32*)(pData + 0xA8) & 0xF03FFFFF;
            subIndex = (((*(u32*)(pData + 0xA8) >> 22) & 0x3F) - 1) & 0x3F;
            *(u32*)(pData + 0xA8) = flags | (subIndex << 22);
            func_800248D4(pData);
        }
        return;
    }

    if (opcode >= 0x10 && opcode < 0x20) {
        s32 delay = (opcode & 0xF) + 1;
        s32 speed = (*(u32*)(pData + 0xAC) >> 7) & 0xFFF;
        s32 scaledDelay;
        u32 flags;
        u32 subIndex;

        *(u32*)(pData + 0x64) = (u32)(uintptr_t)(pc + 1);
        flags = *(u32*)(pData + 0xA8);
        flags = (flags & 0xFFFE07FF) |
                (((((flags >> 11) & 0x3F) + 1) & 0x3F) << 11);
        *(u32*)(pData + 0xA8) = flags;
        func_80022D44(pData);

        scaledDelay = delay * speed;
        if (scaledDelay < 0) {
            scaledDelay += 0xFF;
        }
        delay = scaledDelay >> 8;
        if (delay == 0) {
            delay = 1;
        }

        flags = *(u32*)(pData + 0xA8) & 0xF03FFFFF;
        subIndex = (((*(u32*)(pData + 0xA8) >> 22) & 0x3F) + 1) & 0x3F;
        *(s16*)(pData + 0x9E) = *(u16*)(pData + 0x9E) + delay;
        *(u32*)(pData + 0xA8) = flags | (subIndex << 22);

        if (subIndex == 0) {
            flags = *(u32*)(pData + 0xA8) & 0xF03FFFFF;
            subIndex = (((*(u32*)(pData + 0xA8) >> 22) & 0x3F) - 1) & 0x3F;
            *(u32*)(pData + 0xA8) = flags | (subIndex << 22);
            func_800248D4(pData);
        }
        return;
    }

    if (opcode >= 0x30 && opcode < 0x40) {
        s32 delay = (opcode & 0xF) + 1;
        s32 speed = (*(u32*)(pData + 0xAC) >> 7) & 0xFFF;
        s32 scaledDelay;
        u32 flags;
        u32 subIndex;

        *(u32*)(pData + 0x64) = (u32)(uintptr_t)(pc + 1);

        scaledDelay = delay * speed;
        if (scaledDelay < 0) {
            scaledDelay += 0xFF;
        }
        delay = scaledDelay >> 8;
        if (delay == 0) {
            delay = 1;
        }

        flags = *(u32*)(pData + 0xA8) & 0xF03FFFFF;
        subIndex = (((*(u32*)(pData + 0xA8) >> 22) & 0x3F) + 1) & 0x3F;
        *(s16*)(pData + 0x9E) = *(u16*)(pData + 0x9E) + delay;
        *(u32*)(pData + 0xA8) = flags | (subIndex << 22);

        if (subIndex == 0) {
            flags = *(u32*)(pData + 0xA8) & 0xF03FFFFF;
            subIndex = (((*(u32*)(pData + 0xA8) >> 22) & 0x3F) - 1) & 0x3F;
            *(u32*)(pData + 0xA8) = flags | (subIndex << 22);
            func_800248D4(pData);
        }
        return;
    }

    if (opcode == 0x80) {
        s32 packed = pc[1] | ((s32)(s8)pc[2] << 8);
        s32 delay = (((packed >> 11) & 0xF) + 1);
        s32 speed = (*(u32*)(pData + 0xAC) >> 7) & 0xFFF;
        s32 scaledDelay = delay * speed;
        s32 frame = packed & 0x1FF;

        if (scaledDelay < 0) {
            scaledDelay += 0xFF;
        }
        delay = scaledDelay >> 8;
        if (delay == 0) {
            delay = 1;
        }

        if ((*(u32*)(pData + 0x3C) & 0x3) != 1) {
            *(s16*)(pData + 0x34) = frame;
        } else {
            u32 flagsAC;
            u32 flags3C;

            if (packed < 0 && frame != 0) {
                u8* table = (u8*)(uintptr_t)*(u32*)(pData + 0x60);
                frame = table[frame - 1];
            }

            flagsAC = (*(u32*)(pData + 0xAC) & ~0x8u) | ((packed >> 6) & 0x8);
            flags3C = *(u32*)(pData + 0x3C) & ~0x8u;
            flags3C |= ((((flagsAC >> 3) & 0x1) ^ ((flagsAC >> 2) & 0x1)) << 3);
            if ((packed >> 10) & 0x1) {
                flags3C |= 0x10;
            } else {
                flags3C &= ~0x10u;
            }

            *(u32*)(pData + 0xAC) = flagsAC;
            *(u32*)(pData + 0x3C) = flags3C;
            func_8001D2B0(pData, frame);
        }

        *(s16*)(pData + 0x9E) = *(u16*)(pData + 0x9E) + delay;
        *(u32*)(pData + 0x64) = (u32)(uintptr_t)(pc + 3);
        return;
    }

    if (opcode == 0x81) {
        s8 animIndex = *(s8*)(pData + 0xAF);
        void (*callback)(void*) = (void (*)(void*))(uintptr_t)*(u32*)(pData + 0x68);
        u32 flags;

        if (animIndex == 0x3F) {
            flags = *(u32*)(pData + 0xA8) & 0xCFFFFFFF;
            *(u32*)(pData + 0xA8) = flags;
            if (callback != NULL) {
                callback(pData);
                return;
            }
            if (*(s8*)(pData + 0xB0) >= 0) {
                func_800245D8(pData, *(s8*)(pData + 0xB0));
            }
            *(u32*)(pData + 0xA8) &= 0xCFFFFFFF;
            return;
        }

        *(s16*)(pData + 0x9E) = 0;
        if (callback != NULL) {
            callback(pData);
        }

        flags = *(u32*)(pData + 0xA8) & 0xCFFFFFFF;
        *(u32*)(pData + 0xA8) = flags | 0x10000000;
        return;
    }

    if (opcode == 0xB4) {
        s32 delay;
        s32 speed;

        *(u32*)(pData + 0x64) = (u32)(uintptr_t)(pc + 4);
        if (pc[1] & 0x80) {
            *(s16*)(pData + 0x9E) = *(u16*)(pData + 0x9E) + 1;
            g_WorkListCurTimer = (pc[1] & 0x7F) + 1;
            return;
        }

        delay = pc[1] + 2;
        speed = (*(u32*)(pData + 0xAC) >> 7) & 0xFFF;
        delay *= speed;
        if (delay < 0) {
            delay += 0xFF;
        }
        delay >>= 8;
        if (delay == 0) {
            delay = 1;
        }
        *(s16*)(pData + 0x9E) = *(u16*)(pData + 0x9E) + delay;
        return;
    }

    if (opcode == 0xB3) {
        func_8001FBE4(pData, opcode - 0x80, pc + 1);
        *(u32*)(pData + 0x64) = (u32)(uintptr_t)(pc + 2);
        return;
    }

    if (opcode == 0xB2) {
        func_8001FBE4(pData, opcode, pc + 1);
        *(u32*)(pData + 0x64) = (u32)(uintptr_t)(pc + 2);
        return;
    }

    if (opcode == 0xa0) {
        /* Original asm default path .L80024EC8: call func_8001FBE4 with the
           raw opcode + operand pointer, then advance the cursor by
           D_8004FC40[0xA0] == 2 operand bytes (pc+1 from loop entry, +2). */
        func_8001FBE4(pData, opcode, pc + 1);
        *(u32*)(pData + 0x64) = (u32)(uintptr_t)(pc + 3);
        return;
    }

    assert(0 && "func_800248D4 opcode path is not implemented");
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1", func_80024F20);



extern void* g_GfxWorkBuffer2;
extern s32 g_GfxWorkBufferSize;
extern u32 D_80059300;
extern u32 D_80059304;
extern void* g_GfxWorkBuffers;
extern u32 g_GfxImageList[];
extern void func_8001D298(void);

void GfxAllocateWorkBuffers(int workBufferSize, unsigned int allocFlag) {
    void* pWorkBuffers;

    g_GfxWorkBufferSize = workBufferSize;
    pWorkBuffers = HeapAlloc(workBufferSize * 2, allocFlag);
    g_GfxWorkBuffers = pWorkBuffers;
    g_GfxWorkBuffer2 = (u8*)pWorkBuffers + workBufferSize;
    D_80059304 = 0;
    D_80059300 = 0;
    g_GfxImageList[0] = 0;
    func_8001D298();
}
/*
Matches on  GCC 2.7.2-970404, ASPSX 2.67 and GCC 2.7.2

extern void* g_GfxWorkBuffer2;

int g_GfxWorkBufferSize;
s32 D_80059300; // LinkedLists of SpriteTileData pointers
s32 D_80059304;
void* g_GfxWorkBuffers;
s32 g_GfxImageList;

#define NUM_RENDER_CONTEXTS 2

void GfxAllocateWorkBuffers(int workBufferSize, unsigned int allocFlag) {
    void* pWorkBuffers;

    g_GfxWorkBufferSize = workBufferSize;
    pWorkBuffers = HeapAlloc(workBufferSize * NUM_RENDER_CONTEXTS, allocFlag);
    g_GfxWorkBuffers = pWorkBuffers;
    g_GfxWorkBuffer2 = pWorkBuffers + workBufferSize;
    D_80059304 = 0;
    D_80059300 = 0;
    g_GfxImageList = NULL;
    func_8001D298();
}
*/

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1", GfxFreeWorkBuffers);
/*
void GfxFreeWorkBuffers(void) {
    HeapFree(g_GfxWorkBuffers);
    func_8001D2A4();
}
*/

extern u_long* g_GfxCurOT;

void GfxSetCurrentOT(u_long* ot) {
    g_GfxCurOT = ot;
}








// Maybe start of a TU
// ===========================================================


// Set D_8004FBB8 matrix
extern MATRIX D_8004FBB8;
void func_80024FF4(MATRIX* matrix) {
    D_8004FBB8 = *matrix;
}
/*
Matches on GCC 2.8.0 and GCC 2.7.2-970404, ASPSX 2.67
Co-Authored-By: dezgeg <dezgeg@users.noreply.github.com>

extern MATRIX D_8004FBB8;
void func_80024FF4(MATRIX* matrix) {
    D_8004FBB8 = *matrix;
}
*/




// Maybe start of a TU
// ===========================================================

// Loops over the linked list given by g_GfxImageList[g_GfxCurContext].
// LoadImage on entries with an address, ClearImage otherwise.
// Set the list to NULL afterwards.
extern s32 g_GfxCurContext;

void func_80025044(void) {
    u8* pImage;

    pImage = (u8*)(uintptr_t)g_GfxImageList[g_GfxCurContext];
    while (pImage != NULL) {
        u32 addr = *(u32*)(pImage + 0x8);

        if (addr != 0) {
            LoadImage((RECT*)pImage, (u_long*)(uintptr_t)addr);
        } else {
            ClearImage((RECT*)pImage, 0, 0, 0);
        }

        pImage = (u8*)(uintptr_t)*(u32*)(pImage + 0xC);
    }

    g_GfxImageList[g_GfxCurContext] = 0;
}
/*
Matches on GCC 2.7.2-970404, ASPSX 2.67
Co-Authored-By: dezgeg <dezgeg@users.noreply.github.com>

typedef struct {
    RECT rect;
    u_long* addr;
    struct Image* pNext;
} Image;

int g_GfxCurContext;
extern Image* g_GfxImageList[2];

// GfxShapeTransfer
void func_80025044(void) {
    Image* pListHead;
    Image* pImage;

    pListHead = g_GfxImageList[g_GfxCurContext];
    for (pImage = pListHead; pImage != NULL; pImage = pImage->pNext) {
        if (pImage->addr) {
            LoadImage(&pImage->rect, pImage->addr);
        } else {
            ClearImage(&pImage->rect, 0x0, 0x0, 0x0);
        }
    }

    g_GfxImageList[g_GfxCurContext] = NULL;
}
*/

// This function is in another TU than func_80024F64 due to GP Rel variables
extern void* g_GfxCurWorkBuffer;
extern void* g_GfxCurWorkBufferEnd;
extern uintptr_t D_80059524;
extern void HeapFree(void* ptr);

void func_800250E0(int context) {
    u8* pPrimBuffer = context ? (u8*)g_GfxWorkBuffer2 : (u8*)g_GfxWorkBuffers;
    u32* pListHead = context ? &D_80059304 : &D_80059300;
    u8* pCurEntry = (u8*)(uintptr_t)*pListHead;

    g_GfxCurContext = context;
    g_GfxCurWorkBuffer = pPrimBuffer;
    D_80059524 = (uintptr_t)pPrimBuffer;
    g_GfxCurWorkBufferEnd = pPrimBuffer + g_GfxWorkBufferSize;

    while (pCurEntry != NULL) {
        HeapFree((void*)(uintptr_t)*(u32*)(pCurEntry + 0x0));
        pCurEntry = (u8*)(uintptr_t)*(u32*)(pCurEntry + 0x4);
    }

    *pListHead = 0;
}
/*
Matches on GCC 2.7.2-970404, ASPSX 2.67

typedef struct {
    void* pData;
    struct LinkedListEntry* pNext;
} LinkedListEntry;

s32 g_GfxCurContext;
int g_GfxWorkBufferSize;
extern LinkedListEntry* D_80059300[2];
extern int g_GfxWorkBuffers[];
s32 D_80059524;
void* g_GfxCurWorkBufferEnd;
void* g_GfxCurWorkBuffer;

void func_800250E0(int context) {
    void* pPrimBuffer;
    LinkedListEntry* pCurEntry;

    pPrimBuffer = g_GfxWorkBuffers[context];
    pCurEntry = D_80059300[context];
    g_GfxCurContext = context;
    g_GfxCurWorkBuffer = pPrimBuffer;
    D_80059524 = pPrimBuffer;
    g_GfxCurWorkBufferEnd = pPrimBuffer + g_GfxWorkBufferSize;
    while (pCurEntry != NULL) {
        HeapFree(pCurEntry->pData);
        pCurEntry = pCurEntry->pNext;
    }
    
    D_80059300[context] = NULL;
}
*/


INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1", func_80025180);
/*
typedef struct {
    void* pData; // Sprite Tile data stuff
    struct LinkedListEntry* pNext;
} LinkedListEntry;

extern LinkedListEntry* D_80059300[2];
int g_GfxCurContext;
void* g_GfxCurWorkBuffer;

void func_80025180(void* pData) {
    LinkedListEntry* pNewEntry;

    pNewEntry = (LinkedListEntry*) g_GfxCurWorkBuffer;
    g_GfxCurWorkBuffer = pNewEntry + 1;
    if (pNewEntry) {
        pNewEntry->pData = pData;
        pNewEntry->pNext = D_80059300[g_GfxCurContext];
        D_80059300[g_GfxCurContext] = pNewEntry;
    }
}
*/

// GfxQueueShapeTransfer
// Add Image to current g_GfxImageList[g_GfxCurContext] linked list
void func_800251C8(u_long* addr, int x, int y, int width, int height) {
    u8* pCurrent = g_GfxCurWorkBuffer;
    u8* pNext = pCurrent + 0x10;

    if (pNext < (u8*)g_GfxCurWorkBufferEnd) {
        *(s16*)(pCurrent + 0x6) = height;
        *(s16*)(pCurrent + 0x0) = x;
        *(s16*)(pCurrent + 0x2) = y;
        *(s16*)(pCurrent + 0x4) = width;
        *(u32*)(pCurrent + 0x8) = (u32)(uintptr_t)addr;
        g_GfxCurWorkBuffer = pNext;
        *(u32*)(pCurrent + 0xC) = g_GfxImageList[g_GfxCurContext];
        g_GfxImageList[g_GfxCurContext] = (u32)(uintptr_t)pCurrent;
    }
}
/*
Matches, but uses variable in COMMON so can't compile in yet.

struct ImageEntry {
    RECT rect;
    u_long* addr;
    struct ImageEntry* pNext;
};

typedef struct ImageEntry ImageEntry;

extern ImageEntry* g_GfxImageList[];
int g_GfxCurContext; // Cur index?
void* g_GfxCurWorkBufferEnd;
ImageEntry* g_GfxCurWorkBuffer;

// GfxQueueShapeTransfer
void func_800251C8(u_long* addr, int x, int y, int width, int height) {
    ImageEntry* pCurrent;
    ImageEntry* pNext;

    pCurrent = (ImageEntry*) g_GfxCurWorkBuffer;
    pNext = pCurrent + 1;
    if (pNext < g_GfxCurWorkBufferEnd) {
        pCurrent->rect.h = height;
        pCurrent->rect.x = x;
        pCurrent->rect.y = y;
        pCurrent->rect.w = width;
        pCurrent->addr = addr;
        g_GfxCurWorkBuffer = (void*) pNext;

        pCurrent->pNext = g_GfxImageList[g_GfxCurContext];
        g_GfxImageList[g_GfxCurContext] = pCurrent;
    }
}
*/

// Sets pStruct->unk8 callback
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1", func_80025224);
/*
CALLBACK_TABLE
8004fd40 func_80025258
8004fd44 func_80025710 => Dummy function
8004fd48 func_80025718
8004fd4c NULL
8004fd50 NULL
8004fd54 func_80025258
8004fd58 func_80025258
8004fd5c func_80025718
8004fd60 func_8002541c
8004fd64 func_80025544
8004fd68 NULL
8004fd6c NULL
8004fd70 NULL
8004fd74 NULL
8004fd78 func_80025258
8004fd7c func_800257f0

void func_80025224(WorkListEntry* pTask, int handlerIndex) {
    WorkListSetTaskCallback(pTask, &CALLBACK_TABLE[handlerIndex]);
}
*/

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1", func_80025258);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1", func_8002541C);
/*
Matches on GCC 2.7.2-970404, ASPSX 2.67
Co-Authored-By: dezgeg <dezgeg@users.noreply.github.com>
Co-Authored-By: Mc-muffin <Mc-muffin@users.noreply.github.com>

extern MATRIX D_8004FBB8;
extern s32 D_80050100;
void* g_GfxCurWorkBufferEnd; // End of Prim buffer
extern u_long* g_GfxCurOT;
void* g_GfxCurWorkBuffer; // Prim buffer

void func_8002541C(WorkListEntry* pTask) {
    SVECTOR vec;
    int nFlag;
    int nOTOffset;
    DR_TPAGE* pPrimTPage;
    TILE_1* pPrim;
    u32 pNewBufferHead;
    u32 pNewBufferHead_2;
    SpriteData* pSpriteData;
    MATRIX* pMatrix;

    pSpriteData = pTask->unk4;
    if (pSpriteData->frameIdToRender == 0) {
        pPrim = g_GfxCurWorkBuffer;
        pNewBufferHead = pPrim + 1;
        if (pNewBufferHead < g_GfxCurWorkBufferEnd) {
            pMatrix = &D_8004FBB8;
            vec.vx = pSpriteData->unkX >> 16;
            vec.vy = pSpriteData->unkY >> 16;
            vec.vz = pSpriteData->unkZ >> 16;
            g_GfxCurWorkBuffer = pNewBufferHead;
            SetRotMatrix(pMatrix);
            SetTransMatrix(pMatrix);
            nOTOffset = RotTransPers(&vec, &pPrim->x0, &nFlag, &nFlag) >> D_80050100;
            pSpriteData->unk2E = nOTOffset;

            // SetTile1 / 8 / 16
            setlen(pPrim, 2);
            *((u32*)&pPrim->r0) =  *((u32*)&pSpriteData->primR);
            
            AddPrim(&g_GfxCurOT[nOTOffset], pPrim);

            pPrimTPage = g_GfxCurWorkBuffer;
            pNewBufferHead_2 = pPrimTPage + 1;
            if (pNewBufferHead_2 < g_GfxCurWorkBufferEnd) {
                g_GfxCurWorkBuffer = pNewBufferHead_2;

                // SpriteData->flags3C & 0x60 => tpage
                // setDrawTPage does ((u_long *)(p))[1] = _get_mode(dfe, dtd, tpage)
                // _get_mode(dfe, dtd, tpage) would OR in 0x200 and 0x400 if dfe or dtd was not 0,
                // so we're only left with the tpage as a possibility
                setDrawTPage(pPrimTPage, 0, 0, pSpriteData->flags3C & 0x60);
                AddPrim(&g_GfxCurOT[nOTOffset], pPrimTPage);
            }
        }
    }
}
*/

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1", func_80025544);

void func_80025710(void) {}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1", func_80025718);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1", func_800257F0);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1", func_80025A88);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1", func_80025C04);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1", func_80025D4C);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1", func_80025FA8);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1", func_80026338);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1", func_800263E4);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1", func_8002675C);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1", func_80026A0C);

void func_80026B9C(void) {
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1", func_80026BA4);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1", func_80026DCC);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1", func_80026F44);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1", func_80026FE8);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1", func_8002709C);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1", func_800273C4);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1", func_800278F8);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1", func_80027D40);
