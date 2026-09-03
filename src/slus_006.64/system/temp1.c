#include "common.h"
#ifdef XENO_PC_PORT
#include <assert.h>
#include <stdio.h>

extern char* getenv(const char*);
extern int printf(const char*, ...);

static int XenoWalkAnimDumpEnabled(void)
{
    static int s_enabled = -1;

    if (s_enabled < 0) {
        const char* env = getenv("XENO_WALK_ANIM_DUMP");
        s_enabled = (env != NULL && env[0] != '\0' && env[0] != '0');
    }
    return s_enabled;
}
#else
/* <assert.h> is unavailable under the matching build's -nostdinc MIPS
 * preprocessor. The assert(0) below marks an unimplemented path in a function
 * not yet byte-matched, so a no-op assert compiles safely there. (uintptr_t
 * comes from include/types.h for both builds.) */
#define assert(x) ((void)0)
#endif
#include "field/actor.h"
#include "psyq/libgpu.h"
#include "psyq/libgte.h"
#include "system/memory.h"

// Sprite / Animation functions

extern void func_800BA8F4(void);

void func_80022B2C(u8* pSprite) {
    u32 flags = *(u32*)(pSprite + 0x3C);
    s32 vel, accel, curPos, target;

    if ((flags >> 26) & 1) {
        /* Path 3: steady movement with acceleration */
        vel = *(s32*)(pSprite + 0x10);
        accel = *(s32*)(pSprite + 0x1C);
        curPos = *(s32*)(pSprite + 0x04);
        curPos += func_80022CAC(pSprite, vel >> 4) << 4;
        *(s32*)(pSprite + 0x04) = curPos;
        *(s32*)(pSprite + 0x10) = vel + accel;
        return;
    }

    func_800BA8F4();
    vel = *(s32*)(pSprite + 0x10);
    accel = *(s32*)(pSprite + 0x1C);
    target = *(s16*)(pSprite + 0x84);

    if (vel <= 0 || accel <= 0) {
        /* Path 2: simple forward movement */
        s32 scaled = func_80022CAC(pSprite, vel >> 4) << 4;
        curPos = *(s32*)(pSprite + 0x04) + scaled;
        *(s32*)(pSprite + 0x04) = curPos;
        if ((curPos >> 16) >= target) {
            *(s32*)(pSprite + 0x04) = target << 16;
        }
        *(s32*)(pSprite + 0x10) += accel;
        return;
    }

    /* Path 1: deceleration with bounce */
    {
        s32 scaled = func_80022CAC(pSprite, vel >> 4) << 4;
        s16 y = *(s16*)(pSprite + 0x06);
        curPos = *(s32*)(pSprite + 0x04) + scaled;
        *(s32*)(pSprite + 0x04) = curPos;
        if ((curPos >> 16) < target) {
            /* Not yet at target, add acceleration */
            *(s32*)(pSprite + 0x10) += accel;
            return;
        }
        /* Reached target, bounce */
        *(s32*)(pSprite + 0x04) = target << 16;
        {
            s32 bounceVel = -vel;
            s32 bounceFactor = (*(u32*)(pSprite + 0xA8) >> 1) & 0x3FF;
            s32 newVel = bounceVel * bounceFactor;
            if (newVel < 0) {
                *(s32*)(pSprite + 0x04) = target << 16;
                newVel += 0xFF;
            }
            *(s32*)(pSprite + 0x10) = newVel >> 8;
        }
        vel = *(s32*)(pSprite + 0x10);
        accel = *(s32*)(pSprite + 0x1C);
        if (vel < 0) vel = -vel;
        if (accel < 0) accel = -accel;
        if (vel < accel) {
            *(s32*)(pSprite + 0x10) = 0;
        }
    }
}

s32 func_80022CAC(void* pSpriteData, s32 value)
{
    u16 factor = *(u16*)((u8*)pSpriteData + 0x3A);
    if (factor == 0) return value;
    {
        s32 product = value * factor;
        s32 adj = product;
        if (product < 0) adj = product + 0x3FF;
        return adj >> 10;
    }
}

void func_80022CDC(u8* pSprite) {
    s32 val;
    val = func_80022CAC(pSprite, *(s32*)(pSprite + 0x0C) >> 4);
    *(s32*)(pSprite + 0x00) += val << 4;
    val = func_80022CAC(pSprite, *(s32*)(pSprite + 0x14) >> 4);
    *(s32*)(pSprite + 0x08) += val << 4;
    func_80022B2C(pSprite);
}

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

#ifndef XENO_PC_PORT
void func_80022DF4(pWork)
void* pWork;
{
    u8* pWorkData = pWork;
    u8* pSprite = *(u8**)(pWorkData + 4);
    void (*onFreeCallback)(void*);

    AnimScriptTick(pSprite);
    func_80022CDC(pSprite);
    if (*(u32*)(pSprite + 0x64) != 0) {
        if (((*(u32*)(pSprite + 0xAC) >> 6) & 1) == 0) {
            return;
        }
        AnimScriptTick(pSprite);
        func_80022CDC(pSprite);
        if (*(u32*)(pSprite + 0x64) != 0) {
            return;
        }
    }
    onFreeCallback = *(void (**)(void*))(pWorkData + 0xC);
    onFreeCallback(pWork);
}
#endif

extern s32 D_800592EC;

void func_80022E8C(void)
{
    D_800592EC++;
    func_80022DF4();
}

void func_80022EB8(void* pWork)
{
    u8* pWorkData = (u8*)pWork;
    u32* pSprite = (u32*)(uintptr_t)*(u32*)(pWorkData + 0x04);
    u32* pSub;
    void* pTask;

    pSub = (u32*)(uintptr_t)pSprite[0x20/4];
    if (pSub != NULL) {
        pTask = (void*)(uintptr_t)pSub[0x2C/4];
        if (pTask != NULL) {
            func_80025180(pTask);
        }
    }

    if ((pSprite[0x3C/4] & 3) == 1) {
        u32* pParent = (u32*)(uintptr_t)pSprite[0x20/4];
        void* pFree = (void*)(uintptr_t)pParent[0x34/4];
        if (pFree != NULL) {
            HeapFree(pFree);
        }
    }

    if ((pSprite[0xAC/4] >> 5) & 1) {
        func_8001CE74(pWorkData);
    }

    if ((pSprite[0xB0/4] >> 11) & 1) {
        func_8001D034(pWorkData);
    }

    if ((pSprite[0x3C/4] & 3) == 1) {
        func_8001D3F4(pSprite);
    }

    TimerWorkListRemoveTask(pWorkData);
    WorkListRemoveTask(pWorkData + 0x1C);
    HeapFree(pWork);
}

void func_80022FC4(u8* pSprite, s32 count, s32 tag) {
    u32 pSub = *(u32*)(pSprite + 0x20);
    void* pExisting = *(void**)(pSub + 0x2C);
    if (pExisting) {
        HeapFree(pExisting);
    }
    {
        void* pNew = HeapAlloc(count * 24, tag);
        u32 pSub2 = *(u32*)(pSprite + 0x20);
        *(void**)(pSub2 + 0x2C) = pNew;
        *(void**)(pSub2 + 0x30) = pNew;
    }
}

void func_8002303C(void* pSpriteData, s32 size, s32 flags) {
    u8* pData = (u8*)pSpriteData;
    void* pAlloc = HeapAlloc(size * 4, flags);
    u32* pSub = (u32*)(uintptr_t)*(u32*)(pData + 0x7C);
    u32* pSrc;
    pSub[0x18/4] = (u32)(uintptr_t)pAlloc;
    pSrc = (u32*)(uintptr_t)*(u32*)(pData + 0x7C);
    {
        u32* pSrcData = (u32*)(uintptr_t)*(u32*)(pData + 0x24);
        u16* pAllocH = (u16*)(uintptr_t)pSrc[0x18/4];
        pAllocH[1] = *(u16*)(pData + 0x24 + 6);
        pAllocH[0] = *(u16*)(pData + 0x24 + 4);
    }
}

void func_800230A8(void* pSpriteData)
{
    u32* sprite = (u32*)pSpriteData;

    if ((sprite[0xA8 / 4] & 1) != 0) {
        u32* pSub = (u32*)(uintptr_t)sprite[0x7C / 4];
        if (pSub != NULL && pSub[0x18 / 4] != 0) {
            HeapFree((void*)(uintptr_t)pSub[0x18 / 4]);
        }
    }

    func_8001D3F4(pSpriteData);

    {
        u32* pParent = (u32*)(uintptr_t)sprite[0x20 / 4];
        if (pParent != NULL) {
            HeapFree((void*)(uintptr_t)pParent[0x2C / 4]);
        }
    }

    HeapFree(pSpriteData);
}

s32 func_80023124(s32 pointA, s32 pointB) {
    s16 ax = (s16)(pointA & 0xFFFF);
    s16 ay = (s16)((pointA >> 16) & 0xFFFF);
    s16 bx = (s16)(pointB & 0xFFFF);
    s16 by = (s16)((pointB >> 16) & 0xFFFF);
    return (-ratan2(ay - by, ax - bx)) & 0xFFF;
}

void func_80023170(void* pSpriteData, s16 animFrame, s32 flagA, s32 flagB) {
    u8* pData = (u8*)pSpriteData;
    u32 flags3C = *(u32*)(pData + 0x3C);
    u32 flagsA8 = *(u32*)(pData + 0xA8);
    s32 bitB = (flagB & 1) << 4;
    s32 bitA = (flagA & 1) << 3;

    *(u16*)(pData + 0x9E) = 0;
    flags3C = (flags3C & ~0x30) | bitB;
    flags3C = (flags3C & ~0x08) | bitA;
    *(u32*)(pData + 0x3C) = flags3C;

    flagsA8 = (flagsA8 & 0xFFCFFFFF) & 0xFFF1FFFF;
    *(u32*)(pData + 0xA8) = flagsA8;

    func_8001D2B0(pSpriteData, animFrame);
}

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

void func_80023290(u8* pSprite, s32 animType) {
    u32 flags3C;
    animType &= 7;
    flags3C = *(u32*)(pSprite + 0x3C);
    flags3C = (flags3C & 0xFFFFFF1F) | (animType << 5);
    *(u32*)(pSprite + 0x3C) = flags3C;
    if (animType != 0) {
        pSprite[0x2B] |= 0x2;
    } else {
        pSprite[0x2B] &= ~0x2;
    }
    {
        u32 type = (*(u32*)(pSprite + 0x40) >> 13) & 0xF;
        if (type == 8 || type == 9) {
            u32 val = (*(u32*)(pSprite + 0x3C) >> 5) & 7;
            if (val != 0) {
                *(u32*)(pSprite + 0x3C) = (*(u32*)(pSprite + 0x3C) & 0xFFFFFF1F) | ((val - 1) << 5);
            }
        } else {
            func_8001F6B0(pSprite);
        }
    }
}

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

s32 func_80023440(void* pData)
{
    u16 val = *(u16*)pData;
    s32 result = (val >> 8) & 0x7;
    if ((val >> 14) & 1) {
        result += 8;
    }
    return result;
}

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

void func_80023958(void* pSpriteData) {
    u8* pData = (u8*)pSpriteData;
    void* pSub = pData + 0xB4;
    *(void**)(pData + 0x20) = pSub;
    func_8002393C(pSub);
    {
        u32* pParent = (u32*)(uintptr_t)*(u32*)(pData + 0x20);
        pParent[0x34/4] = 0;
        pParent[0x40/4] = 0;
    }
}

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

void func_800239F4(u8* pSprite) {
    u32 pSub;
    pSub = (u32)(pSprite + 0xB4);
    *(u32*)(pSprite + 0x20) = pSub;
    func_8002393C((void*)pSub);
    pSub = *(u32*)(pSprite + 0x20);
    *(u32*)(pSub + 0x30) = (u32)(pSprite + 0xF4);
    *(u32*)(pSub + 0x34) = 0;
    *(u32*)(pSub + 0x38) = 0;
}

extern u8* D_8006BE10;
extern u8* D_8005A474;
extern s32 func_8001EE74(void* arg0);
extern void func_80023950(void* arg0);
extern void func_80023958(void* pSpriteData);

void* func_80023A48(s32 type, s32 mode, u8* pAnimData, s32 extraSize, u8* pCallback) {
    u8* pResult;
    s32 allocSize;
    u8* pSource = NULL;

    switch (mode) {
        case 0:
            pResult = func_800233A4(pAnimData, extraSize);
            func_80023950(pResult + 0x38);
            allocSize = 0;
            break;
        case 1: {
            s32 frameCount;
            if (type == 5) {
                pSource = D_8006BE10;
            }
            if (type == 6) {
                pSource = D_8005A474;
            }
            frameCount = func_8001EE74(*(void**)pSource) - 1;
            allocSize = frameCount * 24 + 0x58;
            pResult = func_800233A4(pAnimData, allocSize + extraSize);
            func_800239F4(pResult + 0x38);
            break;
        }
        case 2:
            allocSize = 0x54;
            pResult = func_800233A4(pAnimData, 0x54 + extraSize);
            func_80023958(pResult + 0x38);
            break;
        default:
            pResult = NULL;
            allocSize = 0;
            break;
    }
    {
        u8* pSub = pResult + 0x38;
        *(u32*)(pSub + 0x6C) = (u32)pResult;
        *(u16*)(pSub + 0x86) = (u16)(allocSize + 0xEC);
        *(u32*)(pSub + 0x24) = (u32)pSource;
    }
    return pResult;
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1", func_80023B84);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1", func_80023FD8);

extern s32 D_800591B8;
extern void* func_80024524(void* pAnimPackage, s16 texX, s16 texY, s16 clutX, s16 clutY, s16 arg5);
#ifdef XENO_PC_PORT
/* Host GNU C has no implicit int(): prototype must precede the call at
 * func_800242F4. Matching build keeps the original implicit-decl order. */
void* func_8002435C(void* pSpriteData, void* pAnimPackage, s16 texX, s16 texY,
                    s16 clutX, s16 clutY, s16 arg6);
#endif

void* func_80024294(void* pAnimPackage, s16 texX, s16 texY, s16 clutX, s16 clutY, s16 arg5, s32 arg6) {
    void* pSpriteData;

    D_800591B8 = arg6;
    pSpriteData = func_80024524(pAnimPackage, texX, texY, clutX, clutY, arg5);
    D_800591B8 = 0;

    return pSpriteData;
}

void* func_800242F4(void* pAnimPackage, s16 texX, s16 texY, s16 clutX, s16 clutY, s16 arg5, s32 arg6) {
    void* pSpriteData;
    D_800591B8 = arg6;
#ifdef XENO_PC_PORT
    /* Seven-arg retail signature. First slot is filled with pAnimPackage
     * (the only initialized void* already in this function). No HeapAlloc
     * and no func_80024524. Matching build keeps the original six-arg call. */
    pSpriteData = func_8002435C(pAnimPackage, pAnimPackage, texX, texY, clutX,
                               clutY, arg5);
#else
    pSpriteData = func_8002435C(pAnimPackage, texX, texY, clutX, clutY, arg5);
#endif
    D_800591B8 = 0;
    return pSpriteData;
}

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
#ifdef XENO_PC_PORT
    if (XenoWalkAnimDumpEnabled()) {
        printf("[walk-anim] 245D8 sprite=%p anim=%d pose=%d wait=%d pc=%p\n",
               (void*)pData, (int)*(s8*)(pData + 0xAF),
               (int)*(s16*)(pData + 0x34), (int)*(s16*)(pData + 0x9E),
               (void*)(uintptr_t)*(u32*)(pData + 0x64));
    }
#endif
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

reenter:
    if (*(s16*)(pData + 0x9E) != 0) {
        return;
    }

    pc = (u8*)(uintptr_t)*(u32*)(pData + 0x64);
    opcode = *pc;

#ifdef XENO_PC_PORT
    if (XenoWalkAnimDumpEnabled()) {
        static unsigned s_248d4;

        s_248d4++;
        if (s_248d4 <= 80u) {
            printf("[walk-anim] 248D4 n=%u sprite=%p op=0x%02x anim=%d pose=%d wait=%d pc=%p\n",
                   s_248d4, (void*)pData, (unsigned)opcode,
                   (int)*(s8*)(pData + 0xAF), (int)*(s16*)(pData + 0x34),
                   (int)*(s16*)(pData + 0x9E), (void*)pc);
        }
    }
#endif

#ifdef XENO_DIAG_OPCODE_SWEEP
    /* DIAG ONLY: fixed 16-byte records to launcher-preopened fd 3. */
    struct {
        u32 tag;
        u32 spriteData;
        u32 scriptPc;
        s16 actorIndex;
        s8 scriptIndex;
        u8 opcode;
    } record;
    s32 actorIndex = -1;
    s8 scriptIndex = -1;
    s32 i;
    extern long write(int fd, const void* buffer, unsigned long count);

    if (g_FieldActors != NULL && g_FieldNumActors >= 0 && g_FieldNumActors <= 0x100) {
        for (i = 0; i < g_FieldNumActors; i++) {
            if (g_FieldActors[i].pSpriteData == (u32)(uintptr_t)pData) {
                actorIndex = i;
                if (g_FieldActors[i].pActorData != 0) {
                    ActorData* actorData = (ActorData*)(uintptr_t)g_FieldActors[i].pActorData;
                    scriptIndex = (s8)actorData->curScriptIndex;
                }
                break;
            }
        }
    }

    record.tag = 0x5753504F; /* "OPSW" in little-endian byte order */
    record.spriteData = (u32)(uintptr_t)pData;
    record.scriptPc = (u32)(uintptr_t)pc;
    record.actorIndex = (s16)actorIndex;
    record.scriptIndex = scriptIndex;
    record.opcode = opcode;
    write(3, &record, sizeof(record));
#endif

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

    /* asm .L80024998: opcodes 0x20-0x2F — same wait/subIndex path as
     * 0x00-0x0F, but func_8001D2B0(frameIndex - 1) instead of +1. */
    if (opcode >= 0x20 && opcode < 0x30) {
        s32 delay = (opcode & 0xF) + 1;
        s32 speed = (*(u32*)(pData + 0xAC) >> 7) & 0xFFF;
        s32 scaledDelay;
        u32 flags;
        u32 subIndex;

        *(u32*)(pData + 0x64) = (u32)(uintptr_t)(pc + 1);
        func_8001D2B0(pData, *(u16*)(pData + 0x34) - 1);

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

    /* asm 800249C0-80024A54: 0x40-0x7F fall through to the shared delay
     * tail with stale $s3 (no 1D2B0). A first-opcode stale delay of 0
     * clamps to 1, same as the 0x00-0x3F tail. */
    if (opcode >= 0x40 && opcode < 0x80) {
        s32 delay = 0;
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

    /* jtbl_800186E0[0x80] = 0x80024CA0, not the 0xBE packed-frame
     * handler. One-byte terminator: clear A8 bits 28-29, invoke +0x68 if
     * present, else 245D8(+0xB0) when that byte is non-negative. Does not
     * set wait — looping walk/run scripts use 0x82. */
    if (opcode == 0x80) {
        void (*callback)(void*) = (void (*)(void*))(uintptr_t)*(u32*)(pData + 0x68);

        *(u32*)(pData + 0xA8) &= 0xCFFFFFFF;
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

    /* jtbl_800186E0[0xBE] = 0x80024A84. Advances by 3 bytes rather than
     * D_8004FC40[0xBE] == 2. */
    if (opcode == 0xBE) {
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

    if (opcode == 0x82) {
        /* asm 80024D40-80024D7C: restart the current animation (loop
         * terminator). Consumes NO operand bytes and never advances the
         * cursor at +0x64 — func_800245D8 rewrites it to the script start.
         * pData+0x10 is saved across the restart (245D8's callees clobber
         * it), the wait timer is zeroed, and the restarted script runs
         * immediately via recursion. */
        void (*callback)(void*) = (void (*)(void*))(uintptr_t)*(u32*)(pData + 0x68);
        s8 animIndex;
        u32 saved10;

        if (callback != NULL) {
            callback(pData);
        }
        /* asm reads these AFTER the callback (80024D58/80024D5C) — the
         * callback may change the current animation. */
        animIndex = *(s8*)(pData + 0xAF);
        saved10 = *(u32*)(pData + 0x10);
        func_800245D8(pData, animIndex);
        *(u32*)(pData + 0x10) = saved10;
        *(s16*)(pData + 0x9E) = 0;
        func_800248D4(pData);
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
        /* jtbl default .L80024EC8 + FBE4 0xB3 (asm 800214AC): set +0xA8
         * speed-index from operand, advance by D_8004FC40[0xB3]==2, re-enter.
         * Old special-case passed opcode-0x8A underflow (FBE4 no-op) and
         * returned with +0x9E==0, freezing AnimScriptTick (well anim-5). */
        extern const u8 D_8004FC40[256];

        func_8001FBE4(pData, opcode, pc + 1);
        *(u32*)(pData + 0x64) =
            (u32)(uintptr_t)(pc + D_8004FC40[opcode]);
        goto reenter;
    }

    if (opcode == 0xA7) {
        /* asm 80024E10-80024EA8 (jtbl[0xA7-0x80]). Advance by
         * D_8004FC40[0xA7]==2, then delay from operand at pc+1:
         * bit7 set → +0x9E += 1 and g_WorkListCurTimer = (op&0x7F)+1;
         * else → +0x9E += max(1, ((op+2)*speed)>>8). */
        extern const u8 D_8004FC40[256];
        u8 op1 = pc[1];
        s32 delay;

        *(u32*)(pData + 0x64) =
            (u32)(uintptr_t)(pc + D_8004FC40[opcode]);
        if (op1 & 0x80) {
            *(s16*)(pData + 0x9E) = *(u16*)(pData + 0x9E) + 1;
            g_WorkListCurTimer = (op1 & 0x7F) + 1;
            return;
        }
        delay = op1 + 2;
        delay *= (*(u32*)(pData + 0xAC) >> 7) & 0xFFF;
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

    if (opcode == 0xB2 || opcode == 0xA0) {
        /* jtbl_800186E0[0xB2] and [0xA0] are the shared default .L80024EC8:
         * FBE4, D_8004FC40 stride, re-enter. Returning here left +0x9E==0
         * and froze AnimScriptTick after the first walk/run frame. */
        extern const u8 D_8004FC40[256];

        func_8001FBE4(pData, opcode, pc + 1);
        *(u32*)(pData + 0x64) =
            (u32)(uintptr_t)(pc + D_8004FC40[opcode]);
        goto reenter;
    }

    if (opcode == 0xE4) {
        /* asm 80024DC8-80024E0C: counted relative branch. Pop a byte;
         * zero consumes this three-byte instruction, while nonzero pushes
         * count-1 and joins the same signed-offset tail as opcode 0xE1.
         * Inline the exact +0x8C/+0x8E stack-helper effects because those
         * helpers are INCLUDE_ASM and therefore absent from the PC build. */
        extern const u8 D_8004FC40[256];
        u8 stackIndex = *(u8*)(pData + 0x8C);
        s8 signedStackIndex = (s8)stackIndex;
        u8 count = *(u8*)(pData + 0x8E + signedStackIndex);
        s32 offset;

        /* AnimScriptStackPopU8. */
        *(u8*)(pData + 0x8C) = (u8)(stackIndex + 1);
        if (count == 0) {
            *(u32*)(pData + 0x64) =
                (u32)(uintptr_t)(pc + D_8004FC40[opcode]);
            goto reenter;
        }

        /* AnimScriptStackPushU8(count - 1). */
        stackIndex = (u8)(*(u8*)(pData + 0x8C) - 1);
        *(u8*)(pData + 0x8C) = stackIndex;
        *(u8*)(pData + 0x8E + (s8)stackIndex) = (u8)(count - 1);

        offset = (s16)((u16)pc[1] | ((u16)pc[2] << 8));
        *(u32*)(pData + 0x64) = (u32)(uintptr_t)(pc + offset);
        goto reenter;
    }

    if (opcode == 0xE1) {
        /* asm 80024DEC-80024E0C: signed 16-bit PC-relative jump. The
         * two-byte operand is little-endian and relative to the opcode
         * address itself, then execution re-enters at .L8002490C. */
        s32 offset = (s16)((u16)pc[1] | ((u16)pc[2] << 8));

        *(u32*)(pData + 0x64) = (u32)(uintptr_t)(pc + offset);
        goto reenter;
    }

    if (opcode == 0x86) {
        /* asm 80024C68-80024C9C (jtbl[0x86-0x80]).
         * If *(s32*)(pData+0x10) < 0: set wait timer +0x9E = 1 and return
         * (PC stays on 0x86; AnimScriptTick retries next frame).
         * Else: advance by D_8004FC40[0x86]==1 and re-enter. */
        extern const u8 D_8004FC40[256];

        if ((s32)*(u32*)(pData + 0x10) < 0) {
            *(s16*)(pData + 0x9E) = 1;
            return;
        }
        *(u32*)(pData + 0x64) =
            (u32)(uintptr_t)(pc + D_8004FC40[opcode]);
        goto reenter;
    }

    if (opcode == 0x87) {
        /* asm 80024C80-80024C9C (jtbl[0x87-0x80]), shares wait-tail
         * .L80024C98 with 0x86.
         * If *(s16*)(pData+0x6) < *(s16*)(pData+0x84): wait (+0x9E=1),
         * PC unchanged. Else advance by D_8004FC40[0x87]==1 and re-enter. */
        extern const u8 D_8004FC40[256];

        if (*(s16*)(pData + 0x6) < *(s16*)(pData + 0x84)) {
            *(s16*)(pData + 0x9E) = 1;
            return;
        }
        *(u32*)(pData + 0x64) =
            (u32)(uintptr_t)(pc + D_8004FC40[opcode]);
        goto reenter;
    }

    /* jtbl_800186E0 dedicated handlers still unported — keep loud. */
    if (opcode == 0x85 || opcode == 0x8E ||
        opcode == 0x98 || opcode == 0xC8 ||
        opcode == 0xD4 || opcode == 0xE2 || opcode == 0xFA) {
        assert(0 && "func_800248D4 dedicated opcode path is not implemented");
    }

    if (opcode >= 0x80) {
        /* Shared default .L80024EC8 (jtbl entry or out-of-range >=0xFB):
         *   func_8001FBE4(pData, opcode, pc+1);
         *   pData+0x64 += D_8004FC40[opcode];
         *   re-enter (.L8002490C).
         * Map15 actor60 skin5 hits 0xC6 then 0x96 through this path. */
        extern const u8 D_8004FC40[256];

        func_8001FBE4(pData, opcode, pc + 1);
        *(u32*)(pData + 0x64) = (u32)(uintptr_t)(pc + D_8004FC40[opcode]);
        func_800248D4(pData);
        return;
    }

    assert(0 && "func_800248D4 opcode path is not implemented");
}

extern u8 D_800591AD, D_800591AE;
extern s32 D_800591A8;

void func_80024F20(void) {
    D_800591AD = 0;
    D_800591AE = 0;
    D_800591A8 = 0x2000;
    WorkListsReset();
    func_8001D298();
}



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

extern s32 D_80050100;
extern u8 D_800C3664;
extern void func_8001E3D8(void* pSpriteData, void* ot);
extern void func_8001E298(void* pSpriteData, void* ot);

void func_80025258(u8* pEntry) {
    u8* pSprite = *(u8**)(pEntry + 0x04);
    u32 flagsB0 = *(u32*)(pSprite + 0xB0);
    s32 otz;
    s32 depth;
    long pxy[2];
    long flg;

    if ((flagsB0 >> 8) & 1) {
        if (D_800C3664 != 0) return;
    }

    /* Set up GTE with global matrix */
    SetRotMatrix(&D_8004FBB8);
    SetTransMatrix(&D_8004FBB8);

    {
        SVECTOR pos;
        pos.vx = *(s16*)(pSprite + 0x02);
        pos.vy = *(s16*)(pSprite + 0x06);
        pos.vz = *(s16*)(pSprite + 0x0A);
        otz = RotTransPers(&pos, pxy, &flg, &flg);
        otz >>= D_80050100;
    }

    depth = *(s16*)(pSprite + 0x30);
    if (flg & 0x8000) {
        depth = otz + depth;
    } else {
        depth = 0;
    }

    {
        u32 flags3C = *(u32*)(pSprite + 0x3C);
        u32 flag24 = (flags3C >> 24) & 1;
        u32 flag29 = (flags3C >> 29) & 1;

        *(u16*)(pSprite + 0x2E) = (u16)depth;

        if (flag24) {
            /* Path 1: full rendering with matrix setup */
            s32 depthLimit;
            func_80022038(pSprite);
            {
                VECTOR trans;
                trans.vx = *(s16*)(pSprite + 0x02);
                trans.vy = *(s16*)(pSprite + 0x06);
                trans.vz = *(s16*)(pSprite + 0x0A);
                TransMatrix((MATRIX*)(*(u32*)(pSprite + 0x20) + 0xC), &trans);
            }
            SetRotMatrix((MATRIX*)(*(u32*)(pSprite + 0x20) + 0xC));
            SetTransMatrix((MATRIX*)(*(u32*)(pSprite + 0x20) + 0xC));

            if ((flags3C >> 25) & 1) {
                depthLimit = *(s16*)(pSprite + 0x30);
            } else {
                depthLimit = 0xFFF;
            }
            if ((u32)(depthLimit - 1) < 0xFFF) {
                func_8001E3D8(pSprite, g_GfxCurOT + depthLimit * 4);
            }
        } else if (flag29) {
            /* Path 2: use depth from +0x70 sub-structure */
            u32 pSub = *(u32*)(pSprite + 0x70);
            depth = *(s16*)(pSub + 0x2E);
            if ((u32)(depth - 1) < 0xFFF) {
                func_8001E298(pSprite, g_GfxCurOT + depth * 4);
            }
        }
    }
}

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

extern s32 D_80050100;

void func_80025544(u8* pEntry) {
    u8* pSprite = *(u8**)(pEntry + 0x04);
    u16 size;
    s32 otz;
    u8* pTile;
    s32 halfSize;
    u8* pMode;
    SVECTOR v0, v1, v2;
    long pxy0[2], pxy1[2], pxy2[2];
    long flg0, flg1, flg2;

    if (*(u16*)(pSprite + 0x34) != 0) return;
    size = *(u16*)(pSprite + 0x36);

    /* Allocate TILE primitive */
    pTile = (u8*)g_GfxCurWorkBuffer;
    if (pTile + 0x10 >= (u8*)g_GfxCurWorkBufferEnd) return;
    g_GfxCurWorkBuffer = pTile + 0x10;

    /* Set up GTE */
    SetRotMatrix(&D_8004FBB8);
    SetTransMatrix(&D_8004FBB8);

    /* Transform 3 vertices */
    v0.vx = *(s16*)(pSprite + 0x02);
    v0.vy = *(s16*)(pSprite + 0x06);
    v0.vz = *(s16*)(pSprite + 0x0A);
    v1.vx = v0.vx + size;
    v1.vy = v0.vy;
    v1.vz = v0.vz;
    v2.vx = v0.vx;
    v2.vy = v0.vy + size;
    v2.vz = v0.vz;

    otz = RotTransPers3(&v0, &v1, &v2, pxy0, pxy1, pxy2, &flg0, &flg1);
    otz >>= D_80050100;
    *(u16*)(pSprite + 0x2E) = (u16)otz;

    /* Compute tile size from transformed coords */
    halfSize = (s32)(s16)pxy1[0] - (s32)(s16)pxy0[0];
    if (halfSize == 0) halfSize = 1;
    if (halfSize < 0) halfSize = -halfSize;
    halfSize = (halfSize + 1) / 2;

    /* Build TILE primitive */
    pTile[3] = 3; /* TILE tag */
    *(u32*)(pTile + 4) = *(u32*)(pSprite + 0x28);
    *(u16*)(pTile + 8) = (u16)((s16)pxy0[0] - halfSize);
    *(u16*)(pTile + 0xA) = (u16)((s16)pxy0[1] - halfSize);
    *(u16*)(pTile + 0xC) = (u16)size;
    *(u16*)(pTile + 0xE) = (u16)size;
    AddPrim(g_GfxCurOT + otz * 4, pTile);

    /* Allocate DR_MODE primitive */
    pMode = (u8*)g_GfxCurWorkBuffer;
    if (pMode + 8 >= (u8*)g_GfxCurWorkBufferEnd) return;
    g_GfxCurWorkBuffer = pMode + 8;

    pMode[3] = 1; /* DR_MODE tag */
    *(u32*)(pMode + 4) = 0xE1000000 | (*(u32*)(pSprite + 0x3C) & 0x60);
    AddPrim(g_GfxCurOT + otz * 4, pMode);
}

void func_80025710(void) {}

extern MATRIX D_8004FBB8;
extern void func_8002C700(void* a, void* b, u_long* ot, s32 flags);

void func_80025718(u8* pEntry) {
    u8* pSprite = *(u8**)(pEntry + 0x04);
    u8* pSub;
    u8* pMatrix;
    SVECTOR trans;

    func_80022038(pSprite);
    pSub = *(u8**)(pSprite + 0x20);
    if (*(u32*)(pSub + 0x34) == 0) return;

    pMatrix = pSub + 0x0C;
    trans.vx = *(s16*)(pSprite + 0x02);
    trans.vy = *(s16*)(pSprite + 0x06);
    trans.vz = *(s16*)(pSprite + 0x0A);
    TransMatrix((MATRIX*)pMatrix, &trans);

    if (!(*(u8*)(pSprite + 0x3F) & 1)) {
        MATRIX result;
        CompMatrix(&D_8004FBB8, (MATRIX*)(pSub + 0x0C), &result);
        SetRotMatrix(&result);
        SetTransMatrix(&result);
    } else {
        SetRotMatrix((MATRIX*)(pSub + 0x0C));
        SetTransMatrix((MATRIX*)(pSub + 0x0C));
    }

    {
        s32 ctxIdx = g_GfxCurContext;
        u32 flags = *(u16*)(pSprite + 0x42) & 4;
        func_8002C700(
            *(void**)(pSub + 0x34),
            *(void**)(pSub + 0x2C + ctxIdx * 4),
            g_GfxCurOT,
            flags
        );
    }
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1", func_800257F0);

extern s32 D_80050100;
extern void func_800B1F6C(void* a, void* b, u_long* ot, s32 c, s32 d, s32 e, s32 f);

void func_80025A88(u8* pEntry) {
    u8* pSprite = *(u8**)(pEntry + 0x04);
    u8* pSub;
    SVECTOR pos;
    VECTOR result;

    func_80022038(pSprite);
    pSub = *(u8**)(pSprite + 0x20);
    if (*(u32*)(pSub + 0x34) == 0) return;

    pos.vx = *(s16*)(pSprite + 0x02);
    pos.vy = *(s16*)(pSprite + 0x06);
    pos.vz = *(s16*)(pSprite + 0x0A);
    ApplyMatrix(&D_8004FBB8, &pos, &result);

    pSub = *(u8**)(pSprite + 0x20);
    *(s32*)(pSub + 0x20) = D_8004FBB8.t[0] + result.vx;
    *(s32*)(pSub + 0x24) = D_8004FBB8.t[1] + result.vy;
    *(s32*)(pSub + 0x28) = D_8004FBB8.t[2] + result.vz;

    SetRotMatrix((MATRIX*)(pSub + 0x0C));
    SetTransMatrix((MATRIX*)(pSub + 0x0C));

    {
        s32 ctxIdx = g_GfxCurContext;
        u32 flags3C = *(u32*)(pSprite + 0x3C);
        s32 arg7;
        s32 oldShift;
        if ((flags3C >> 25) & 1) {
            oldShift = D_80050100;
            D_80050100 = 0x10;
            arg7 = 0xFEC;
        } else {
            arg7 = *(s16*)(pSprite + 0x30);
        }
        func_800B1F6C(
            *(void**)(pSub + 0x34),
            *(void**)(pSub + 0x2C + ctxIdx * 4),
            g_GfxCurOT, 0, arg7, (flags3C >> 5) & 1, 0
        );
        if ((flags3C >> 25) & 1) {
            D_80050100 = oldShift;
        }
    }
}

#ifndef XENO_PC_PORT
/* MIPS-only: .ent/.end and R_MIPS_* relocs are rejected by the host
 * assembler (same guard as menu.c func_8001C76C). */
__asm__(
        ".globl func_80025C04\n\t"
        ".ent func_80025C04\n\t"
        "func_80025C04:\n\t"
        ".word 0x3c081f80, 0x000529c0, 0x48854000, 0x2484ffff\n\t"
        ".word 0x2402ffff, 0x1082004a, 0x00000000\n\t"
        ".Lfunc_80025C04_loop:\n\t"
        ".word 0x94e20000, 0x00000000, 0x3042001f, 0xad020004\n\t"
        ".word 0x94e20000, 0x00000000, 0x304203e0, 0xad020008\n\t"
        ".word 0x94e20000, 0x00000000, 0x30427c00, 0xad02000c\n\t"
        ".reloc ., R_MIPS_LO16, D_1F800004\n\t"
        ".word 0x25020000, 0xc8490000, 0xc84a0004, 0xc84b0008\n\t"
        ".word 0x00000000, 0x00000000, 0x4b98003d\n\t"
        ".reloc ., R_MIPS_LO16, D_1F800014\n\t"
        ".word 0x25020000, 0xe8490000, 0xe84a0004, 0xe84b0008\n\t"
        ".word 0x8d020014, 0x00000000, 0x28420020, 0x10400004\n\t"
        ".word 0x2402001f, 0x95020014, 0x00000000, 0x3042001f\n\t"
        ".word 0xa5020000, 0x8d020018, 0x00000000, 0x284203e1\n\t"
        ".word 0x14400006, 0x00000000, 0x95020000, 0x00000000\n\t"
        ".word 0x344203e0\n\t"
        ".reloc ., R_MIPS_26, .Lfunc_80025C04_green_done\n\t"
        ".word 0x08000000, 0xa5020000\n\t"
        ".word 0x95020018, 0x95030000, 0x304203e0, 0x00621825\n\t"
        ".word 0xa5030000\n\t"
        ".Lfunc_80025C04_green_done:\n\t"
        ".word 0x8d02001c, 0x00000000, 0x28427c01, 0x14400006\n\t"
        ".word 0x00000000, 0x95020000, 0x00000000, 0x34427c00\n\t"
        ".reloc ., R_MIPS_26, .Lfunc_80025C04_blue_done\n\t"
        ".word 0x08000000, 0xa5020000\n\t"
        ".word 0x9502001c, 0x95030000, 0x30427c00, 0x00621825\n\t"
        ".word 0xa5030000\n\t"
        ".Lfunc_80025C04_blue_done:\n\t"
        ".word 0x94e30000, 0x24e70002, 0x2484ffff, 0x95020000\n\t"
        ".word 0x30638000, 0x00431025, 0xa5020000, 0xa4c20000\n\t"
        ".word 0x2402ffff, 0x1482ffb8, 0x24c60002\n\t"
        ".word 0x03e00008, 0x00000000\n\t"
        ".end func_80025C04");
#endif

/* Handwritten GTE color-processing routine retained as assembly. */
#ifndef XENO_PC_PORT
/* MIPS-only: .ent/.end and R_MIPS_* relocs are rejected by the host
 * assembler (same guard as func_80025C04 above). */
__asm__(
        ".globl func_80025D4C\n\t"
        ".ent func_80025D4C\n\t"
        "func_80025D4C:\n\t"
        ".word 0x27bdffe8, 0x8fb9002c, 0x8fa30038, 0x8fb80030, 0x00806821, 0xafb00010, 0x8fb00028, 0x8fae0034\n\t"
        ".word 0x00a07821, 0x28620021, 0x14400002, 0xafb10014, 0x24030020, 0x000311c0, 0x48824000, 0x0018c280\n\t"
        ".reloc ., R_MIPS_26, .Lfunc_80025D4C_loop_test\n\t"
        ".word 0x08000000, 0x0019c940, 0x95e80000, 0x11c20011, 0x29c20002, 0x10400005, 0x24020002, 0x11c00009\n\t"
        ".word 0x01901021\n\t"
        ".reloc ., R_MIPS_26, .Lfunc_80025D4C_apply\n\t"
        ".word 0x08000000, 0x00402021, 0x11c20010, 0x24020003, 0x11c20015, 0x01901021\n\t"
        ".reloc ., R_MIPS_26, .Lfunc_80025D4C_apply\n\t"
        ".word 0x08000000, 0x00402021, 0x310c001f, 0x310b03e0\n\t"
        ".reloc ., R_MIPS_26, .Lfunc_80025D4C_components_done\n\t"
        ".word 0x08000000, 0x310a7c00, 0x3102001e, 0x00026042, 0x310203c0\n\t"
        ".word 0x00025842, 0x31027800\n\t"
        ".reloc ., R_MIPS_26, .Lfunc_80025D4C_components_done\n\t"
        ".word 0x08000000, 0x00025042, 0x3102001c, 0x00026082, 0x31020380, 0x00025882\n\t"
        ".word 0x31027000\n\t"
        ".reloc ., R_MIPS_26, .Lfunc_80025D4C_components_done\n\t"
        ".word 0x08000000, 0x00025082, 0x3c055555, 0x34a55556, 0x310303e0, 0x31047c00, 0x3102001f\n\t"
        ".word 0x00031942, 0x00431021, 0x00042282, 0x00441021, 0x00450018, 0x000217c3, 0x00008810, 0x02221023\n\t"
        ".word 0x00406021, 0x00025940, 0x00025280\n\t"
        ".Lfunc_80025D4C_components_done:\n\t"
        ".word 0x01901021, 0x00402021\n\t"
        ".Lfunc_80025D4C_apply:\n\t"
        ".word 0x00021400, 0x04410002, 0x01791021\n\t"
        ".word 0x00002021, 0x00404821, 0x00021400, 0x04410002, 0x01581021, 0x00004821, 0x00402821, 0x00021400\n\t"
        ".word 0x04410002, 0x00041400, 0x00002821, 0x00021403, 0x28420020, 0x14400002, 0x00091400, 0x2404001f\n\t"
        ".word 0x00021403, 0x284203e1, 0x14400002, 0x00051400, 0x240903e0, 0x00021403, 0x28427c01, 0x14400002\n\t"
        ".word 0x00041400, 0x24057c00, 0x94c30000, 0x00021403, 0x3063001f, 0x00621823, 0x00091400, 0xafa30000\n\t"
        ".word 0x94c30000, 0x00021403, 0x306303e0, 0x00621823, 0x00051400, 0xafa30004, 0x94c30000, 0x00021403\n\t"
        ".word 0x30637c00, 0x00621823, 0xafa30008, 0xcba90000, 0xcbaa0004, 0xcbab0008, 0x00000000, 0x00000000\n\t"
        ".word 0x4b98003d, 0xeba90000, 0xebaa0004, 0xebab0008, 0x1100000e, 0x00000000, 0x97a20000, 0x97a30008\n\t"
        ".word 0x3042001f, 0x00821021, 0x34428000, 0x30637c00, 0x97a40004, 0x00a31821, 0x308403e0, 0x01242021\n\t"
        ".word 0x00441025, 0x00431025, 0xa4e20000, 0x25ef0002, 0x24c60002, 0x24e70002\n\t"
        ".Lfunc_80025D4C_loop_test:\n\t"
        ".word 0x25adffff, 0x2402ffff\n\t"
        ".word 0x15a2ff81, 0x24020001, 0x8fb10014, 0x8fb00010, 0x27bd0018, 0x03e00008, 0x00000000\n\t"
        ".end func_80025D4C");
#endif

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1", func_80025FA8);

#ifdef XENO_PC_PORT
/* Unpack a texture-atlas entry (indexed by `index` into `table`) into the
 * caller's tpage / CLUT / texcoord output fields. `table[index*2+4]` is the
 * byte offset of the entry within `table`; entry[0]=item count, entry+4 is the
 * first item's descriptor. Stubbed, the window-border sprites get zeroed
 * tpage/clut. All outputs are s32/u32 fields, matching the asm sw width. */
void func_80026338(u8* table, s32 index, u32* pOut0, s32* pTPage, s32* pClutX,
                   s32* pClutY, s32* pTexX, s32* pTexY) {
    u8* pEntry = table + *(u16*)(table + index * 2 + 4);
    u8* item = pEntry + 4;
    u16 packed = *(u16*)(pEntry + 4);
    s16 tpage = *(s16*)(item + 0x10);
    s32 shift;

    *pOut0 = (u32)(s32)*(s16*)(pEntry);
    if (tpage == 0) {
        shift = ((s32)((u32)packed << 16)) >> 20;
    } else {
        shift = ((s32)((u32)packed << 16)) >> 18;
    }
    *pTPage = *(s16*)(item + 0x10);
    *pClutX = *(s16*)(item + 0x12);
    *pClutY = *(s16*)(item + 0x14);
    *pTexX = (s32)(s16)(*(u16*)(item + 0x16) & 0xFFC0) + shift;
    *pTexY = (s32)(s16)(*(u16*)(item + 0x18) & 0xFF00) + *(s16*)(item + 2);
}
#else
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1", func_80026338);
#endif

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1", func_800263E4);

#ifdef XENO_PC_PORT
/* Build a run of POLY_FT4 sprites for atlas entry `index` (window borders,
 * scroll-bar ornaments, etc.). pEntry[0] = item count; each 0x1C-byte item
 * holds signed position/size (scaled by `scale`>>12) + raw UV/wh + per-axis
 * flip flags at +0x1A/+0x1B. Writes one 0x28-byte POLY_FT4 per item into
 * `polys` at the current renderCtx slot (+0x50 stride, two contexts). Returns
 * the item count. Stubbed, the border sprites never build (empty frame). */
s32 func_8002675C(u8* table, s32 index, void* polys, s32 renderCtx, s32 x, s32 y,
                  s32 scale) {
    u8* pEntry = table + *(u16*)(table + index * 2 + 4);
    u32 s4 = (u32)scale & 0xFFFF;
    s32 fp = 4;
    u8* poly = (u8*)polys;
    s32 slot;

    if (*(s16*)(pEntry) == 0) {
        return 0;
    }
    slot = 0;
    do {
        u8* item = pEntry + fp;
        u8* s0 = poly + renderCtx * 0x28;
        s32 t, s3, s6, s2, s5;
        s32 u0, v0r, uw, vh, u1, v1;
        s32 px0, px1, py0, py1;

        t = *(s16*)(item + 0x8) * (s32)s4; if (t < 0) t += 0xFFF; s3 = t >> 12;
        t = *(s16*)(item + 0xA) * (s32)s4; if (t < 0) t += 0xFFF; s6 = (s32)((u32)t >> 12);
        t = *(s16*)(item + 0x4) * (s32)s4; if (t < 0) t += 0xFFF; s2 = t >> 12;
        t = *(s16*)(item + 0x6) * (s32)s4; if (t < 0) t += 0xFFF; s5 = (s32)((u32)t >> 12);

        SetPolyFT4((POLY_FT4*)s0);
        SetSemiTrans((POLY_FT4*)s0, 0);
        SetShadeTex((POLY_FT4*)s0, 1);
        *(s16*)(s0 + 0x16) = GetTPage(*(s16*)(item + 0x10), 0,
                                      *(s16*)(item + 0x16), *(s16*)(item + 0x18));
        *(s16*)(s0 + 0xE) = GetClut(*(s16*)(item + 0x12), *(s16*)(item + 0x14));

        u0 = *(u16*)(item + 0x0);
        v0r = *(u16*)(item + 0x2);
        uw = *(u16*)(item + 0x4);
        vh = *(u16*)(item + 0x6);

        if (*(u8*)(item + 0x1A) == 0) {          /* no horizontal flip */
            px0 = x + s3;
            px1 = s2 + px0;
        } else {                                  /* horizontal flip: swap L/R x */
            u0 -= 1;
            px1 = x + s3;
            px0 = s2 + px1;
            if ((s16)u0 < 0) { u0 = 0; uw -= 1; }
        }
        *(s16*)(s0 + 0x8) = px0; *(s16*)(s0 + 0x10) = px1;
        *(s16*)(s0 + 0x18) = px0; *(s16*)(s0 + 0x20) = px1;

        if (*(u8*)(item + 0x1B) == 0) {          /* no vertical flip */
            py0 = y + s6;
            py1 = s5 + py0;
            *(s16*)(s0 + 0xA) = py0; *(s16*)(s0 + 0x12) = py0;
            *(s16*)(s0 + 0x1A) = py1; *(s16*)(s0 + 0x22) = py1;
        } else {                                  /* vertical flip: swap T/B y */
            v0r -= 1;
            py0 = y + s6;
            py1 = s5 + py0;
            *(s16*)(s0 + 0xA) = py1; *(s16*)(s0 + 0x12) = py1;
            *(s16*)(s0 + 0x1A) = py0; *(s16*)(s0 + 0x22) = py0;
            if ((s16)v0r < 0) { v0r = 0; vh -= 1; }
        }

        u1 = u0 + uw;
        v1 = v0r + vh;
        *(u8*)(s0 + 0xC) = u0;  *(u8*)(s0 + 0xD) = v0r;
        *(u8*)(s0 + 0x14) = u1; *(u8*)(s0 + 0x15) = v0r;
        *(u8*)(s0 + 0x1C) = u0; *(u8*)(s0 + 0x1D) = v1;
        *(u8*)(s0 + 0x24) = u1; *(u8*)(s0 + 0x25) = v1;

        fp += 0x1C;
        poly += 0x50;
        slot++;
    } while (slot != *(s16*)(pEntry));
    return *(s16*)(pEntry);
}
#else
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1", func_8002675C);
#endif

s32 func_80026A0C(u8* pTable, s32 index, u8* pPrimBuffer, s32 primStride, s16 ofsX, s16 ofsY) {
    u8* pDesc = pTable + *(u16*)(pTable + index * 2 + 4);
    s32 count = *(s16*)(pDesc);
    s32 i;
    u8* pEntry = pDesc + 4;
    s32 stride = primStride * 20; /* primStride * 0x28 per SPRT */
    u8* pPrim = pPrimBuffer + stride;

    for (i = 0; i < count; i++) {
        s32 clut = GetClut(*(s16*)(pEntry + 0x12), *(s16*)(pEntry + 0x14));
        SetSprt((void*)pPrim);
        SetSemiTrans((void*)pPrim, 0);
        SetShadeTex((void*)pPrim, 1);
        *(u16*)(pPrim + 0x0E) = (u16)clut;
        *(u16*)(pPrim + 0x08) = *(u16*)(pEntry + 0x08) + ofsX;
        *(u16*)(pPrim + 0x0A) = *(u16*)(pEntry + 0x0A) + ofsY;
        *(u8*)(pPrim + 0x0C) = *(u8*)(pEntry + 0x00);
        *(u8*)(pPrim + 0x0D) = *(u8*)(pEntry + 0x02);
        *(u16*)(pPrim + 0x10) = *(u16*)(pEntry + 0x04);
        *(u16*)(pPrim + 0x12) = *(u16*)(pEntry + 0x06);
        pEntry += 0x1C;
        pPrim += 0x28;
    }

    /* Add DR_MODE */
    {
        s32 tpage = GetTPage(0, 0, *(s16*)(pDesc + 4 + 0x10), *(s16*)(pDesc + 4 + 0x16));
        u8* pMode = pPrimBuffer + count * 20 + stride;
        SetDrawMode((void*)pMode, 0, 0, tpage & 0xFFFF, NULL);
    }
    return count + 1;
}

void func_80026B9C(void) {
}

void func_80026BA4(u8* pTable, s32 index, s16 ofsX, s16 ofsY, s16 ofsZ, u8* pPrimBuffer) {
    u8* pDesc = pTable + *(u16*)(pTable + index * 2 + 4);
    s32 count = *(s16*)(pDesc);
    s32 i;
    s32 stride = 0;
    u8* pEntry = pDesc + 4;
    u8* pCur = pPrimBuffer;

    if (count == 0) return;

    for (i = 0; i < count; i++) {
        u16 packed = *(u16*)(pEntry);
        s16 tpageFlag = *(s16*)(pEntry + 0x10);
        s32 shift;
        s16 texX, texY;
        s32 clut, tpage;
        s16 u0, v0, u1, v1;
        s16 x0, y0, x1, y1;

        if (tpageFlag == 0) {
            shift = ((s32)((u32)packed << 16)) >> 20;
        } else {
            shift = ((s32)((u32)packed << 16)) >> 18;
        }

        texX = *(s16*)(pEntry + 0x12);
        texY = *(s16*)(pEntry + 0x14);
        u0 = (s16)((s32)((u16)*(u16*)(pEntry + 0x16) & 0xFFC0) << 16 >> 16) + shift;
        v0 = (s16)((s32)((u16)*(u16*)(pEntry + 0x18) & 0xFF00) << 16 >> 16) + *(s16*)(pEntry + 0x02);
        u1 = *(s16*)(pEntry + 0x04);
        v1 = *(s16*)(pEntry + 0x06);
        x0 = *(s16*)(pEntry + 0x08);
        y0 = *(s16*)(pEntry + 0x0A);

        clut = GetClut(texX, texY);
        tpage = GetTPage(tpageFlag, 0, texX, texY);

        /* Build POLY_GT4 */
        pCur[3] = 0x09; /* GT4 tag len */
        pCur[7] = 0x2D; /* POLY_GT4 code */
        *(u16*)(pCur + 0x0E) = (u16)clut;
        *(u16*)(pCur + 0x16) = (u16)tpage;

        x0 += ofsX;
        y0 += ofsY;
        x1 = x0 + u1;
        y1 = y0 + v1;

        *(s16*)(pCur + 0x08) = x0;
        *(s16*)(pCur + 0x0A) = y0;
        *(u8*)(pCur + 0x0C) = (u8)u0;
        *(u8*)(pCur + 0x0D) = (u8)v0;
        *(s16*)(pCur + 0x10) = x1;
        *(s16*)(pCur + 0x12) = y0;
        *(u8*)(pCur + 0x14) = (u8)(u0 + u1);
        *(u8*)(pCur + 0x15) = (u8)v0;
        *(s16*)(pCur + 0x18) = x0;
        *(s16*)(pCur + 0x1A) = y1;
        *(u8*)(pCur + 0x1C) = (u8)u0;
        *(u8*)(pCur + 0x1D) = (u8)(v0 + v1);
        *(s16*)(pCur + 0x20) = x1;
        *(s16*)(pCur + 0x22) = y1;
        *(u8*)(pCur + 0x24) = (u8)(u0 + u1);
        *(u8*)(pCur + 0x25) = (u8)(v0 + v1);

        AddPrim(g_GfxCurOT + ofsZ * 4, pCur);
        pEntry += 0x1C;
        pCur += 0x28;
    }
}

s32 func_80026DCC(u8* pTable, s32 index, u8* pPrimBuffer, s16 ofsX, s16 ofsY) {
    u8* pDesc = pTable + *(u16*)(pTable + index * 2 + 4);
    s32 count = *(s16*)(pDesc);
    s32 i;
    u8* pEntry = pDesc + 4;
    u8* pPrim = pPrimBuffer;

    if (count == 0) return 0;

    for (i = 0; i < count; i++) {
        u16 packed = *(u16*)(pEntry);
        s16 tpageFlag = *(s16*)(pEntry + 0x6);
        s32 shift;
        s16 texX, texY;
        s32 clut, tpage;

        if (tpageFlag == 0) {
            shift = ((s32)((u32)packed << 16)) >> 20;
        } else {
            shift = ((s32)((u32)packed << 16)) >> 18;
        }

        texX = (s16)((s32)((u16)*(u16*)(pEntry + 0xC) & 0xFFC0) << 16 >> 16) + shift;
        texY = (s16)((s32)((u16)*(u16*)(pEntry + 0xE) & 0xFF00) << 16 >> 16) + *(s16*)(pEntry - 0x8);

        clut = GetClut(*(s16*)(pEntry + 0x8), *(s16*)(pEntry + 0xA));
        tpage = GetTPage(tpageFlag, 1, texX, texY);

        *(u16*)(pPrim + 0x08) = (u16)tpage;
        *(u16*)(pPrim + 0x0A) = (u16)tpage;
        *(u8*)(pPrim + 0x02) = *(u8*)(pEntry);
        *(u8*)(pPrim + 0x03) = *(u8*)(pEntry - 0x8);
        *(u8*)(pPrim + 0x04) = *(u8*)(pEntry - 0x6);
        *(u8*)(pPrim + 0x05) = *(u8*)(pEntry - 0x4);
        *(u16*)(pPrim + 0x00) = *(u16*)(pEntry - 0x2) + ofsX;
        *(u16*)(pPrim + 0x00) |= (*(u16*)(pEntry) + ofsY) << 16;

        pEntry += 0x1C;
        pPrim += 0x18;
    }
    return count;
}

#ifndef XENO_PC_PORT
/* MIPS-only: .ent/.end and R_MIPS_* relocs are rejected by the host
 * assembler (same guard as func_80025C04 / func_80025D4C above). */
__asm__(
        ".globl func_80026F44\n\t"
        ".ent func_80026F44\n\t"
        "func_80026F44:\n\t"
        ".word 0x28aa0020, 0x140a0002, 0x00000000, 0x20050020\n\t"
        ".word 0x000529c0, 0x48854000, 0x240affff\n\t"
        ".Lfunc_80026F44_loop:\n\t"
        ".word 0x2484ffff, 0x108a001e, 0x94ee0000, 0x24e70002\n\t"
        ".word 0x31cd001f, 0x31cc03e0, 0x31cb7c00\n\t"
        ".word 0x488d4800, 0x488c5000, 0x488b5800\n\t"
        ".word 0x00000000, 0x00000000, 0x4b98003d\n\t"
        ".word 0x480d4800, 0x480c5000, 0x480b5800\n\t"
        ".word 0x31ad001f, 0x318c03e0, 0x316b7c00\n\t"
        ".word 0x01ac6825, 0x016d6825, 0x100e0004, 0x00000000\n\t"
        ".word 0x140d0002, 0x00000000, 0x35ad0001\n\t"
        ".word 0x31ce8000, 0x01cd7025, 0xa4ce0000, 0x24c60002\n\t"
        ".reloc ., R_MIPS_26, .Lfunc_80026F44_loop\n\t"
        ".word 0x08000000, 0x00000000, 0x03e00008, 0x00000000\n\t"
        ".end func_80026F44");
#endif

#ifndef XENO_PC_PORT
/* MIPS-only: .ent/.end and R_MIPS_* relocs are rejected by the host
 * assembler (same guard as func_80025C04 / func_80025D4C / func_80026F44). */
__asm__(
        ".globl func_80026FE8\n\t"
        ".ent func_80026FE8\n\t"
        "func_80026FE8:\n\t"
        ".word 0x8fa80010, 0x28a20021, 0x14400002, 0x00c05021\n\t"
        ".word 0x24050020, 0x000511c0, 0x48824000, 0x24840001\n\t"
        ".Lfunc_80026FE8_loop:\n\t"
        ".word 0x2484ffff, 0x10800021, 0x950c0000, 0x94e20000\n\t"
        ".word 0x21080002, 0x304d001f, 0x304e03e0, 0x304f7c00\n\t"
        ".word 0x3183001f, 0x006d5823, 0x488b4800, 0x318303e0\n\t"
        ".word 0x006e5023, 0x488a5000, 0x31837c00, 0x006f4823\n\t"
        ".word 0x48895800, 0x00000000, 0x00000000, 0x4b98003d\n\t"
        ".word 0x480b4800, 0x480a5000, 0x48095800, 0x316b001f\n\t"
        ".word 0x01ab5820, 0x314a03e0, 0x01ca5020, 0x31297c00\n\t"
        ".word 0x01e94820, 0x016a5825, 0x01695825, 0xa4cb0000\n\t"
        ".word 0x20e70002\n\t"
        ".reloc ., R_MIPS_26, .Lfunc_80026FE8_loop\n\t"
        ".word 0x08000000, 0x20c60002, 0x03e00008, 0x00000000\n\t"
        ".end func_80026FE8");
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1", func_8002709C);
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1", func_800273C4);
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1", func_800278F8);
#else
/* Horizon / line-scroll backdrop (title map 490 path). Layout of the 0x34C
 * heap block matches func_8002709C.s / func_800273C4.s / func_800278F8.s. */

static s32 HorizonMulShift12(s32 a, s32 b) {
    s32 t = a * b;
    if (t < 0) {
        t += 0xFFF;
    }
    return t >> 12;
}

static s32 HorizonMulShift8(s32 a, s32 b) {
    s32 t = a * b;
    if (t < 0) {
        t += 0xFF;
    }
    return t >> 8;
}

void func_800278F8(u8* ctx, s32 scroll, s32 screenY, s32 fade, void* ot,
                   s32 renderCtx);

void* func_8002709C(s32 a0, s32 a1, s32 a2, s32 a3, s32 clutX, s32 clutY,
                    s32 abr, s32 scrollSignArg, s16* pCoords, u8* pColors,
                    s32 skyScale, s32 fadeDiv, s32 fadeSub) {
    DRAWENV drawEnv;
    u8* ctx;
    s32 i;
    s32 t;
    u16 clut;

    HeapChangeCurrentUser(HEAP_USER_MASA, NULL);
    ctx = (u8*)HeapAlloc(0x34C, 0);
    if (ctx == NULL) {
        return NULL;
    }

#ifdef XENO_PC_PORT
    /* DIAGNOSTIC: one-shot horizon init probe. Remove once title backdrop
     * dest_nz/src704_nz are non-zero. */
    {
        static int s_hzInitDiag;
        if (!s_hzInitDiag) {
            s_hzInitDiag = 1;
            printf("[xeno-port][horizon] DIAG 2709C a0=%d a1=%d w=%d h=%d "
                   "clut=(%d,%d) mode=%d sign=%d coords=(%d,%d,%d) "
                   "rgb0=%02x%02x%02x sky=%d fade=%d/%d ctx=%p\n",
                   a0, a1, a2, a3, clutX, clutY, abr, scrollSignArg,
                   pCoords ? (int)pCoords[0] : -1,
                   pCoords ? (int)pCoords[2] : -1,
                   pCoords ? (int)pCoords[4] : -1,
                   pColors ? pColors[0] : 0, pColors ? pColors[1] : 0,
                   pColors ? pColors[2] : 0, skyScale, fadeDiv, fadeSub,
                   (void*)ctx);
            fflush(stdout);
        }
    }
#endif

    GetDrawEnv(&drawEnv); /* asm calls it; result unused */

    *(s32*)(ctx + 0x328) = a2;
    *(s32*)(ctx + 0x32C) = a3;
    *(s16*)(ctx + 0x33C) = (s16)pCoords[0];
    *(s16*)(ctx + 0x33E) = (s16)pCoords[2]; /* +0x4 as halfwords */
    *(s16*)(ctx + 0x346) = (s16)skyScale;
    *(s16*)(ctx + 0x348) = (s16)fadeDiv;
    *(s16*)(ctx + 0x34A) = (s16)fadeSub;
    *(s16*)(ctx + 0x340) = (s16)pCoords[4]; /* +0x8 */

    if (*(s32*)((u8*)pCoords + 8) < 0) {
        *(s32*)(ctx + 0x330) = -scrollSignArg;
    } else {
        *(s32*)(ctx + 0x330) = scrollSignArg;
    }

    *(s16*)(ctx + 0x336) = (s16)a1;
    *(s16*)(ctx + 0x334) = (s16)a0;
    *(s16*)(ctx + 0x338) = (s16)abr;

    t = a1;
    if (a1 < 0) {
        t = a1 + 0xFF;
    }
    *(s16*)(ctx + 0x33A) = (s16)(a1 - ((t >> 8) << 8));

    clut = GetClut(clutX, clutY);
    {
        u8* p = ctx;
        for (i = 0; i < 0x10; i++) {
            SetPolyFT4((POLY_FT4*)p);
            SetShadeTex((POLY_FT4*)p, 1);
            ((POLY_FT4*)p)->clut = clut;
            p += 0x28;
        }
    }

    if (pColors == NULL) {
        *(s16*)(ctx + 0x344) = 0;
        return ctx;
    }

    *(s16*)(ctx + 0x344) = 1;

    /* Two top POLY_F4 sky fills at 0x280 (shared RGB from pColors[0..2]). */
    {
        u8* p = ctx;
        s32 off = 0x280;
        for (i = 0; i < 2; i++) {
            POLY_F4* poly = (POLY_F4*)(ctx + off);
            SetPolyF4(poly);
            poly->r0 = pColors[0];
            poly->g0 = pColors[1];
            poly->b0 = pColors[2];
            poly->x0 = 0;
            poly->y0 = 0;
            poly->x1 = 0x140;
            poly->y1 = 0;
            poly->x2 = 0;
            poly->x3 = 0x140;
            off += 0x18;
            p += 0x18;
            (void)p;
        }
    }

    pColors += 4;

    /* Two POLY_G4 gradient bands at 0x2E0. */
    {
        s32 off = 0x2E0;
        for (i = 0; i < 2; i++) {
            POLY_G4* poly = (POLY_G4*)(ctx + off);
            u8* c0 = pColors;
            u8* c1 = pColors + 4;
            SetPolyG4(poly);
            poly->r0 = c0[0];
            poly->g0 = c0[1];
            poly->b0 = c0[2];
            poly->r1 = c0[0];
            poly->g1 = c0[1];
            poly->b1 = c0[2];
            poly->r2 = c1[0];
            poly->g2 = c1[1];
            poly->b2 = c1[2];
            poly->r3 = c1[0];
            poly->g3 = c1[1];
            poly->b3 = c1[2];
            poly->x0 = 0;
            poly->x1 = 0x140;
            poly->x2 = 0;
            poly->x3 = 0x140;
            off += 0x24;
        }
    }

    pColors += 4;

    /* Two bottom POLY_F4 fills at 0x2B0 (RGB from pColors[0..2]). */
    {
        s32 off = 0x2B0;
        for (i = 0; i < 2; i++) {
            POLY_F4* poly = (POLY_F4*)(ctx + off);
            SetPolyF4(poly);
            poly->r0 = pColors[0];
            poly->g0 = pColors[1];
            poly->b0 = pColors[2];
            poly->x0 = 0;
            poly->x1 = 0x140;
            poly->x2 = 0;
            poly->y2 = 0xF0;
            poly->x3 = 0x140;
            poly->y3 = 0xF0;
            off += 0x18;
        }
    }

    return ctx;
}

s32 func_800273C4(void* ctxPtr, SVECTOR* eye, SVECTOR* at, MATRIX* mtx,
                  void* ot, s32 renderCtx) {
    u8* ctx = (u8*)ctxPtr;
    VECTOR dir;
    SVECTOR dirN;
    SVECTOR pt;
    long sxy;
    long p, flag;
    s32 fade;
    s32 scroll;
    s32 screenY;
    s32 screenY2;
    s32 yTop;
    s32 t;
    s32 dx, dy, dz;

    if (ctx == NULL) {
        return 0;
    }

    dir.vx = at->vx - eye->vx;
    dir.vy = 0;
    dir.vz = at->vz - eye->vz;
    VectorNormalS(&dir, &dirN);

    t = HorizonMulShift12(dirN.vx, *(s16*)(ctx + 0x340));
    pt.vx = (s16)(at->vx + t);
    pt.vy = *(s16*)(ctx + 0x33E);
    t = HorizonMulShift12(dirN.vz, *(s16*)(ctx + 0x340));
    pt.vz = (s16)(at->vz + t);

    SetRotMatrix(mtx);
    SetTransMatrix(mtx);
    RotTransPers(&pt, &sxy, &p, &flag);
    screenY = (s16)(sxy >> 16);

    dx = dir.vx;
    dz = dir.vz;
    if (*(s16*)(ctx + 0x348) != 0) {
        dx = at->vx - eye->vx;
        dy = at->vy - eye->vy;
        dz = at->vz - eye->vz;
        {
            s32 dist = SquareRoot0(dx * dx + dy * dy + dz * dz);
            s32 denom = *(s16*)(ctx + 0x348);
            fade = (dist - *(s16*)(ctx + 0x34A)) / denom;
            if (fade < 0) {
                fade = 0;
            } else if (fade > 0x100) {
                fade = 0x100;
            }
        }
    } else {
        fade = 0;
    }

    {
        s32 ang = ratan2(dx, dz) & 0xFFF;
        s32 prod = *(s32*)(ctx + 0x328) * *(s32*)(ctx + 0x330);
        scroll = HorizonMulShift12(prod, ang);
    }

    func_800278F8(ctx, scroll, screenY, fade, ot, renderCtx);

#ifdef XENO_PC_PORT
    /* DIAGNOSTIC: one-shot horizon draw probe. Remove with 2709C DIAG. */
    {
        static int s_hzDrawDiag;
        if (!s_hzDrawDiag) {
            s_hzDrawDiag = 1;
            printf("[xeno-port][horizon] DIAG 273C4 screenY=%d scroll=%d fade=%d "
                   "hasColors=%d h=%d stripScale=%d ot=%p rcx=%d "
                   "ft4[0]=(%d,%d)-(%d,%d) tpage=0x%x\n",
                   screenY, scroll, fade, (int)*(s16*)(ctx + 0x344),
                   *(s32*)(ctx + 0x32C), (int)*(s16*)(ctx + 0x346), ot,
                   renderCtx,
                   (int)((POLY_FT4*)ctx)->x0, (int)((POLY_FT4*)ctx)->y0,
                   (int)((POLY_FT4*)ctx)->x3, (int)((POLY_FT4*)ctx)->y3,
                   (unsigned)((POLY_FT4*)ctx)->tpage);
            fflush(stdout);
        }
    }
#endif

    if (*(s16*)(ctx + 0x344) <= 0) {
        return screenY;
    }

    yTop = screenY - *(s32*)(ctx + 0x32C);
    if (yTop > 0xF0) {
        yTop = 0xF0;
    }
    if (yTop > 0) {
        POLY_F4* poly = (POLY_F4*)(ctx + 0x280 + renderCtx * 0x18);
        poly->y2 = (s16)yTop;
        poly->y3 = (s16)yTop;
        AddPrim(ot, poly);
    }

    t = HorizonMulShift12(dirN.vx, *(s16*)(ctx + 0x340));
    t = HorizonMulShift8(t, *(s16*)(ctx + 0x346));
    pt.vx = (s16)(at->vx + t);
    t = HorizonMulShift8(*(s16*)(ctx + 0x33E), *(s16*)(ctx + 0x346));
    pt.vy = (s16)t;
    t = HorizonMulShift12(dirN.vz, *(s16*)(ctx + 0x340));
    t = HorizonMulShift8(t, *(s16*)(ctx + 0x346));
    pt.vz = (s16)(at->vz + t);

    RotTransPers(&pt, &sxy, &p, &flag);
    screenY2 = (s16)(sxy >> 16);

    if ((screenY2 - screenY) >= 0xF1) {
        screenY2 = screenY + 0xF0;
    }

    if (screenY2 >= 0 && screenY < 0xF0) {
        POLY_G4* poly = (POLY_G4*)(ctx + 0x2E0 + renderCtx * 0x24);
        poly->y0 = (s16)screenY;
        poly->y1 = (s16)screenY;
        poly->y2 = (s16)screenY2;
        poly->y3 = (s16)screenY2;
        AddPrim(ot, poly);
    }

    {
        s32 yBot = (screenY2 < 0) ? 0 : screenY2;
        if (yBot < 0xF0) {
            POLY_F4* poly = (POLY_F4*)(ctx + 0x2B0 + renderCtx * 0x18);
            poly->y0 = (s16)yBot;
            poly->y1 = (s16)yBot;
            AddPrim(ot, poly);
        }
    }

    return screenY;
}

void func_800278F8(u8* ctx, s32 scroll, s32 screenY, s32 fade, void* ot,
                   s32 renderCtx) {
    s32 width = *(s32*)(ctx + 0x328);
    s32 height = *(s32*)(ctx + 0x32C);
    s32 fadePlus = fade + 0x100;
    s32 stripH;
    s32 xCursor;
    s32 uPos;
    s32 i;
    s32 half;
    s32 t;
    s32 vFixed;
    POLY_FT4* poly;

    t = (width << 8) / fadePlus;
    t = 0x140 - t;
    half = (t + (t >> 31)) >> 1;
    t = half + HorizonMulShift8(half, fade);
    t = (s16)(scroll - t);

    if (width != 0) {
        uPos = t % width;
        if ((uPos << 16) < 0) {
            uPos += (u16)width;
        }
    } else {
        uPos = 0;
    }

    if (screenY < 0 || screenY > height + 0xF0) {
        stripH = 0;
    } else {
        stripH = (height << 8) / fadePlus;
    }
    xCursor = 0;

    poly = (POLY_FT4*)(ctx + ((renderCtx & 1) * 0x140));

    if ((s16)stripH <= 0) {
        return;
    }

    {
        s32 tpX = *(s16*)(ctx + 0x334);
        s32 abr = *(s16*)(ctx + 0x338);
        if (tpX < 0) {
            tpX += 0x3F;
        }
        vFixed = tpX - ((tpX >> 6) << 6);
        vFixed <<= (2 - abr);
    }

    for (i = 0; i < 8; i++) {
        s32 abr = *(s16*)(ctx + 0x338);
        s32 tpXbase = *(u16*)(ctx + 0x334);
        s32 shift = 2 - abr;
        s32 tpageX = tpXbase + ((s16)uPos >> shift);
        s32 mask = (0x100 >> abr) - 1;
        s32 u0 = (uPos + vFixed) & mask;
        s32 uSpan = 0x100 - u0;
        s32 spanPx;
        s32 x1;
        s32 tpY = *(s16*)(ctx + 0x336);
        s32 gx;

        if ((s16)(uPos + (s16)uSpan) > width) {
            uSpan = (s16)(width - uPos);
        }

        /* asm: sll uSpan,16; sra 8 → (s16)uSpan << 8, then / fadePlus */
        spanPx = (((s32)(s16)uSpan << 8) / fadePlus);

        if ((s16)(xCursor + spanPx) > 0x140) {
            spanPx = 0x140 - xCursor;
            {
                s32 tmp = (s16)spanPx * fadePlus;
                if (tmp < 0) {
                    tmp += 0xFF;
                }
                /* asm: srl (logical) after signed round — positive spans only */
                uSpan = (u32)tmp >> 8;
            }
        }

        x1 = xCursor + spanPx;

        {
            s32 next = uPos + (s16)uSpan;
            if (width != 0) {
                uPos = next % width;
                /* asm uses mfhi of div next/width, then if negative add width —
                 * already handled for C % with positive width after adjust: */
                if (uPos < 0) {
                    uPos += width;
                }
            }
        }

        poly->x0 = (s16)xCursor;
        poly->y0 = (s16)(screenY - stripH);
        poly->x1 = (s16)x1;
        poly->y1 = (s16)(screenY - stripH);
        poly->x2 = (s16)xCursor;
        poly->y2 = (s16)screenY;
        poly->x3 = (s16)x1;
        poly->y3 = (s16)screenY;

        poly->u0 = (u8)u0;
        poly->v0 = *(u8*)(ctx + 0x33A);
        poly->u1 = (u8)(u0 + (s16)uSpan - 1);
        poly->v1 = *(u8*)(ctx + 0x33A);
        poly->u2 = (u8)u0;
        poly->v2 = (u8)(*(u8*)(ctx + 0x33A) + *(u8*)(ctx + 0x32C));
        poly->u3 = (u8)(u0 + (s16)uSpan - 1);
        poly->v3 = (u8)(*(u8*)(ctx + 0x33A) + *(u8*)(ctx + 0x32C));

        gx = tpageX;
        if (gx < 0) {
            gx += 0x3F;
        }
        gx = (gx >> 6) << 6;
        if (tpY < 0) {
            tpY += 0xFF;
        }
        poly->tpage = GetTPage(abr, 0, gx, (tpY >> 8) << 8);

        AddPrim(ot, poly);

        xCursor = x1;
        if ((s16)xCursor >= 0x140) {
            break;
        }
        poly = (POLY_FT4*)((u8*)poly + 0x28);
    }
}
#endif

void func_80027D40(void* ptr) {
    if (ptr != NULL) {
        HeapFree(ptr);
    }
}
