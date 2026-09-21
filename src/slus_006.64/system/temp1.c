#include "common.h"
#ifdef XENO_PC_PORT
#include <assert.h>
#include <stdio.h>
#include "psx_memory.h"
#include "guest_prim_link.h"

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

extern void func_800BA8F4(void* pSpriteData);

void func_80022B2C(u8* pSpriteData)
{
    u8* p = pSpriteData;
    s32 vel, dv, pos, floor;

    if ((*(u32*)(p + 0x3C) >> 26) & 1) {
        vel = *(s32*)(p + 0x10);
        dv = (s32)((u32)func_80022CAC(p, vel >> 4) << 4);
        *(u32*)(p + 0x4) += (u32)dv;
        *(s32*)(p + 0x10) = (s32)((u32)vel + *(u32*)(p + 0x1C));
        return;
    }

    func_800BA8F4(p);

    vel = *(s32*)(p + 0x10);
    if (vel > 0 && *(s32*)(p + 0x1C) > 0) {
        floor = *(s16*)(p + 0x84);
        if (*(s16*)(p + 0x6) == (s16)floor) {
            return;
        }
        dv = (s32)((u32)func_80022CAC(p, vel >> 4) << 4);
        pos = (s32)(*(u32*)(p + 0x4) + (u32)dv);
        *(s32*)(p + 0x4) = pos;
        if ((pos >> 16) < floor) {
            *(u32*)(p + 0x10) += *(u32*)(p + 0x1C);
            return;
        }

        /* Landed: snap to the floor and bounce. */
        *(s32*)(p + 0x4) = (s32)((u32)floor << 16);
        pos = (s32)((0u - (u32)vel) * ((*(u32*)(p + 0xA8) >> 1) & 0x3FF));
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
            dv = (s32)(0u - (u32)dv);
        }
        if (pos < dv) {
            *(s32*)(p + 0x10) = 0;
        }
        return;
    }

    dv = (s32)((u32)func_80022CAC(p, vel >> 4) << 4);
    pos = (s32)(*(u32*)(p + 0x4) + (u32)dv);
    *(s32*)(p + 0x4) = pos;
    floor = *(s16*)(p + 0x84);
    if ((pos >> 16) >= floor) {
        *(s32*)(p + 0x4) = (s32)((u32)floor << 16);
    }
    *(u32*)(p + 0x10) += *(u32*)(p + 0x1C);
}

s32 func_80022CAC(void* pSpriteData, s32 value)
{
    u16 factor = *(u16*)((u8*)pSpriteData + 0x3A);
    if (factor != 0) {
        s32 adj;
        /* MULT/MFLO keeps the low word; the wide product avoids C overflow. */
        value = (s32)((s64)value * factor);
        adj = value;
        if (value < 0) adj = value + 0x3FF;
        value = adj >> 10;
    }
    return value;
}

void func_80022CDC(u8* pSprite) {
    s32 val;
    val = func_80022CAC(pSprite, *(s32*)(pSprite + 0x0C) >> 4);
    *(u32*)(pSprite + 0x00) += (u32)val << 4;
    val = func_80022CAC(pSprite, *(s32*)(pSprite + 0x14) >> 4);
    *(u32*)(pSprite + 0x08) += (u32)val << 4;
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
/* Transcribed from asm/slus_006.64/nonmatchings/system/temp1/func_800233A4.s
 * (0x800233A4-0x80023440). Allocates a dataSize+0xEC wrapper for pOwner, links
 * it into both the timer and the work list (inner node at +0x1C), initialises the
 * +0x38 sub-structure, points both nodes' +4 field at it, then installs the
 * timer callback func_80022DF4 and the free callback func_80022EB8, returning the
 * wrapper. The port keeps its own owner in pc_port/src/game_overrides.c, so this
 * definition is weak under XENO_PC_PORT. */
extern u8 D_800591AF;
extern void func_80023804(void* p);
extern void func_80022DF4(void* pWork);
extern void func_80022EB8(void* pWork);
extern void TimerWorkListAddTask(void* pOwner, void* pTask);
extern void WorkListAddTask(void* pTask, void* pNode);
extern void TimerWorkListSetTaskCallback(void* pTask, void (*pCallback)(void*));
extern void WorkListTaskSetOnFreeCallback(void* pTask, void (*pCallback)(void*));

#ifdef XENO_PC_PORT
__attribute__((weak))
#endif
void* func_800233A4(void* pOwner, s32 dataSize) {
    u8* pWrapper;
    u8* pInner;

    pWrapper = HeapAlloc(dataSize + 0xEC, D_800591AF);
    TimerWorkListAddTask(pOwner, pWrapper);
    pInner = pWrapper + 0x1C;
    WorkListAddTask(pWrapper, pInner);
    func_80023804(pWrapper + 0x38);
    *(u32*)(pWrapper + 4) = (u32)(uintptr_t)(pWrapper + 0x38);
    *(u32*)(pInner + 4) = (u32)(uintptr_t)(pWrapper + 0x38);
    TimerWorkListSetTaskCallback(pWrapper, func_80022DF4);
    WorkListTaskSetOnFreeCallback(pWrapper, func_80022EB8);
    return pWrapper;
}
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

/* Transcribed from asm/slus_006.64/nonmatchings/system/temp1/func_80023468.s
 * (0x80023468-0x800234AC, 17 instructions). Small dispatch on `type`: retail
 * rejects `type >= 0x10` up front, then jtbl_80018664 maps cases 0/5/6/10-14 to
 * 1, cases 1/4/8/9 to 0 and cases 2/7/15 to 2 — while **case 3 falls through to
 * the same exit as the out-of-range path, which returns the caller's a1
 * register**. That register value is modelled here as the second parameter
 * (retail's callers pass whatever a1 holds, and the C callers in this repo pass
 * an explicit value); a one-argument call therefore yields that slot's content,
 * exactly like retail. */
s32 func_80023468(s32 type, s32 arg1) {
    switch (type) {
    case 0:
    case 5:
    case 6:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
        return 1;
    case 1:
    case 4:
    case 8:
    case 9:
        return 0;
    case 2:
    case 7:
    case 15:
#ifdef TEMP1_23468_MUTANT_SWAP_GROUPS
        return 1;
#else
        return 2;
#endif
    default:
        return arg1;
    }
}

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
        }
        if (D_800591AD) {
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

extern u8 D_800591AC;
extern void func_80024730(u8* pOwner);

void* func_80023B84(void* pSpriteData, void* pScript, void* pAnimPackage) {
    u8* pSprite = pSpriteData;
    void* pEntry = pScript;
    u8* pExtra = pAnimPackage;
    u8* pWrapper;
    u8* pNode;
    s32 kind;
    s32 mode;
    u8 prevFlag;
    u32 b0;
    u32 u24;
    u32 r40;
    u32 r3c;
    u32 vMix;
    u32 ac;
    u32 bit9;

    b0 = *(u32*)(pSprite + 0xB0) | 0x800;
    *(u32*)(pSprite + 0xB0) = b0;
    prevFlag = D_800591AC;
    if (((b0 >> 8) & 1) != 0) {
        D_800591AC = 0;
    }
    kind = func_80023440(pEntry);
    if (kind == 3) {
        kind = (*(u32*)(pSprite + 0x40) >> 13) & 0xF;
    }
    /* Retail leaves entry a1 (pScript/pEntry) in place: func_80023440 never
     * writes a1, and nothing between the two jals touches it. kind is always
     * 0-15 here so the default arm (and thus arg1) is dead, but pass it
     * explicitly per this TU's contract. */
    mode = func_80023468(kind, (s32)(uintptr_t)pEntry);
    pWrapper = func_80023A48(kind, mode, pExtra, 0, *(u8**)(pSprite + 0x6C));
    pNode = pWrapper + 0x38;
    *(u32*)(pWrapper + 0x14) |= 0x20000000;
    r40 = (*(u32*)(pNode + 0x40) & 0xFFFE1FFF) | ((kind & 0xF) << 13);
    *(u32*)(pNode + 0x40) = r40;
    u24 = *(u32*)(pNode + 0x24);
    *(u32*)(pNode + 0x3C) = (*(u32*)(pNode + 0x3C) & ~3) | (mode & 3);
    {
        u32 r40b = r40;
        r40b = (r40b & ~0x1F00) | (*(u32*)(pSprite + 0x40) & 0x1F00);
        *(u32*)(pNode + 0x40) = r40b;
    }
    *(u32*)(pNode + 0x3C) = (*(u32*)(pNode + 0x3C) & ~8) | (*(u32*)(pSprite + 0x3C) & 8);
    *(u32*)(pNode + 0x3C) = (*(u32*)(pNode + 0x3C) & ~0x10) | (*(u32*)(pSprite + 0x3C) & 0x10);
    *(u8*)(pNode + 0x3D) = *(u8*)(pSprite + 0x3D);
    r40 = (*(u32*)(pNode + 0x40) & 0xFFFBFFFF) | (*(u32*)(pSprite + 0x40) & 0x40000);
    *(u32*)(pNode + 0x40) = r40;
    *(u32*)(pNode + 0x3C) = (*(u32*)(pNode + 0x3C) | 0x4000000) & ~4;
    *(u32*)(pNode + 0x18) = *(u32*)(pSprite + 0x18);
    *(u16*)(pNode + 0x32) = *(u16*)(pSprite + 0x32);
    *(u16*)(pNode + 0x2C) = *(u16*)(pSprite + 0x2C);
    *(u16*)(pNode + 0x34) = *(u16*)(pSprite + 0x34);
    {
        u32 b0s = *(u32*)(pNode + 0xB0);
        bit9 = (*(u32*)(pSprite + 0xB0) >> 9) & 1;
        b0s = (b0s & ~0x200) | (bit9 << 9);
        *(u32*)(pNode + 0xB0) = b0s;
        if (bit9 != 0) {
            *(u32*)(pNode + 0x40) = (*(u32*)(pNode + 0x40) & 0xFFFE1FFF) | 0x300;
            *(u16*)(pNode + 0x3A) = *(u16*)(pSprite + 0x3A);
        }
    }
    vMix = ((*(u32*)(pSprite + 0xAC) & 3) << 2) | (*(u32*)(pSprite + 0xA8) >> 30);
    *(u32*)(pNode + 0xA8) = (*(u32*)(pNode + 0xA8) & 0x3FFFFFFF) | (vMix << 30);
    *(u32*)(pNode + 0xAC) = (*(u32*)(pNode + 0xAC) & ~3) | (vMix >> 2);
    *(u32*)(pNode + 0xB0) = (*(u32*)(pNode + 0xB0) & ~0x100) | (*(u32*)(pSprite + 0xB0) & 0x100);
    *(u32*)(pNode + 0xAC) = (*(u32*)(pNode + 0xAC) & ~0x40) | (*(u32*)(pSprite + 0xAC) & 0x40);
    ac = (*(u32*)(pNode + 0xAC) & 0xFFF8007F) | (*(u32*)(pSprite + 0xAC) & 0x7FF80);
    *(u32*)(pNode + 0xAC) = ac;
    ac &= ~4;
    *(u32*)(pNode + 0xA8) &= ~1;
    ac |= (*(u32*)(pSprite + 0xAC) & 4);
    *(u32*)(pNode + 0xAC) = ac;
    if ((*(u32*)(pSprite + 0xA8) & 1) != 0) {
        *(u32*)(pNode + 0x7C) = 0;
    } else {
        *(u32*)(pNode + 0x7C) = *(u32*)(pSprite + 0x7C);
    }
    *(u32*)(pNode + 0x70) = (u32)pSprite;
    *(u32*)(pNode + 0x44) = *(u32*)(pSprite + 0x44);
    *(u32*)(pNode + 0x48) = *(u32*)(pSprite + 0x48);
    *(u32*)(pNode + 0x74) = *(u32*)(pSprite + 0x74);
    *(u16*)(pNode + 0x82) = *(u16*)(pSprite + 0x82);
    *(u32*)(pNode + 0x50) = *(u32*)(pSprite + 0x50);
    *(u8*)(pNode + 0x8D) = *(u8*)(pSprite + 0xAF);
    *(u32*)(pNode + 0x78) = *(u32*)(pSprite + 0x78);
    *(u32*)(pNode + 0x0) = *(u32*)(pSprite + 0x0);
    *(u32*)(pNode + 0x4) = *(u32*)(pSprite + 0x4);
    *(u32*)(pNode + 0x8) = *(u32*)(pSprite + 0x8);
    *(u32*)(pNode + 0xC) = *(u32*)(pSprite + 0xC);
    *(u32*)(pNode + 0x10) = *(u32*)(pSprite + 0x10);
    *(u32*)(pNode + 0x14) = *(u32*)(pSprite + 0x14);
    if (mode != 0) {
        *(u16*)(*(u8**)(pNode + 0x20) + 0x0) = *(u16*)(*(u8**)(pSprite + 0x20) + 0x0);
        *(u16*)(*(u8**)(pNode + 0x20) + 0x2) = *(u16*)(*(u8**)(pSprite + 0x20) + 0x2);
        *(u16*)(*(u8**)(pNode + 0x20) + 0x4) = *(u16*)(*(u8**)(pSprite + 0x20) + 0x4);
        *(u16*)(*(u8**)(pNode + 0x20) + 0x6) = *(u16*)(*(u8**)(pSprite + 0x20) + 0x6);
        *(u16*)(*(u8**)(pNode + 0x20) + 0x8) = *(u16*)(*(u8**)(pSprite + 0x20) + 0x8);
        *(u16*)(*(u8**)(pNode + 0x20) + 0xA) = *(u16*)(*(u8**)(pSprite + 0x20) + 0xA);
    }
    func_80023538(pNode, pEntry);
    /* Keep u24 (dead retail load) and kind live across the call above so the
     * allocator places them in s3/s1; both asms emit zero bytes. */
    __asm__ volatile("" :: "r"(kind));
    __asm__ volatile("" :: "r"(u24));
    func_80024730(pWrapper);
    D_800591AC = prevFlag;
    return pNode;
}

/* Transcribed from asm/slus_006.64/nonmatchings/system/temp1/func_80023FD8.s
 * (0x80023FD8-0x80024294, 175 instructions). Sprite-script spawn: resolves the
 * script entry `index` (u16 byte offset at package+0x10, indexed two bytes per
 * entry, read from +2), classifies it with func_80023440 / func_80023468
 * (retail passes the still-live a1 = package to the dispatcher, which is only
 * observable for the case-3 fall-through), allocates through func_80023A48,
 * then tags wrapper+0x14 in place. The optional block runs only when
 * D_800591AD is set and the player sprite D_800C3E1C is non-NULL: retail
 * copies the player's render/tint/colour state into the new node, keeps **two
 * separate** +0x7C transfers, and re-joins the (player+0xA8 >> 30) with
 * (player+0xAC & 3) into node+0xA8 bits 30-31 / node+0xAC bits 0-1. The tail
 * re-stores +0x24 (loaded after func_80023A48 returned), clears
 * +0x44/+0x48/+0x34, folds type into +0x40 bits 13-16 and mode into +0x3C
 * bits 0-1, writes the D_800591A8 duration to +0x82, stores the three s16
 * script values <<16 into +0x0/+0x4/+0x8, then runs func_80023538(node, entry)
 * and func_80024730(wrapper). */
extern void func_80024730(u8* pOwner);
extern u32 D_800C3E1C;

/* The port keeps its host-pointer constructor in pc_port/src/sprite_constructor.c
 * (field_object_overlay.c calls it with host-translated pointers); this matching
 * owner writes the retail 32-bit pointer words, so it is weak under the port. */
#ifdef XENO_PC_PORT
__attribute__((weak))
#endif
u8* func_80023FD8(s32 index, u8* pAnimData, s16* pPosition, s32 extraSize) {
    u8* pScriptTable;
    u8* pEntry;
    u8* pWrapper;
    u8* pNode;
    u8* pParent;
    u32 source;
    u32 bits40;
    u32 bits3c;
    u32 bitsAC;
    u32 split;
    u16 duration;
    s32 type;
    s32 mode;

    pScriptTable = (u8*)(uintptr_t)*(u32*)(pAnimData + 0x10);
    pEntry = pScriptTable + *(u16*)(pScriptTable + index * 2 + 2);
    type = func_80023440(pEntry);
    mode = func_80023468(type, (s32)(uintptr_t)pAnimData);
    pWrapper = (u8*)func_80023A48(type, mode, pAnimData, extraSize, NULL);
    pNode = pWrapper + 0x38;

    *(u32*)(pWrapper + 0x14) |= 0x20000000;
    source = *(u32*)(pNode + 0x24);
    *(u32*)(pNode + 0x70) = 0;
    *(u32*)(pNode + 0x74) = 0;

    if (D_800591AD != 0) {
        pParent = (u8*)(uintptr_t)D_800C3E1C;
        if (pParent != NULL) {
            bits40 = *(u32*)(pNode + 0x40);
            *(u32*)(pNode + 0x44) = *(u32*)(pParent + 0x44);
            *(u32*)(pNode + 0x48) = *(u32*)(pParent + 0x48);
            *(u32*)(pNode + 0x74) = *(u32*)(pParent + 0x74);
            *(u32*)(pNode + 0x18) = *(u32*)(pParent + 0x18);
            *(u16*)(pNode + 0x32) = *(u16*)(pParent + 0x32);
            bits40 = (bits40 & 0xFFFFE0FF) | (*(u32*)(pParent + 0x40) & 0x1F00);
            *(u32*)(pNode + 0x40) = bits40;
            bits3c = *(u32*)(pNode + 0x3C);
            bits3c = (bits3c & ~0x8) | (*(u32*)(pParent + 0x3C) & 0x8);
            *(u32*)(pNode + 0x3C) = bits3c;
            bits3c = (bits3c & ~0x10) | (*(u32*)(pParent + 0x3C) & 0x10);
            *(u32*)(pNode + 0x3C) = bits3c;
            *(u8*)(pNode + 0x3D) = *(u8*)(pParent + 0x3D);
            *(u16*)(pNode + 0x2C) = *(u16*)(pParent + 0x2C);
            bits3c = (*(u32*)(pNode + 0x3C) | 0x04000000) & ~0x4;
            *(u32*)(pNode + 0x3C) = bits3c;
            bitsAC = (*(u32*)(pNode + 0xAC) & ~0x4) | (*(u32*)(pParent + 0xAC) & 0x4);
            *(u32*)(pNode + 0xAC) = bitsAC;
            bitsAC = (bitsAC & 0xFFF8007F) | (*(u32*)(pParent + 0xAC) & 0x7FF80);
            *(u32*)(pNode + 0xAC) = bitsAC;
            *(u32*)(pNode + 0x7C) = *(u32*)(pParent + 0x7C);
            *(u32*)(pNode + 0x7C) = *(u32*)(pParent + 0x7C);
            split = (*(u32*)(pParent + 0xA8) >> 30) | ((*(u32*)(pParent + 0xAC) & 0x3) << 2);
            *(u32*)(pNode + 0xA8) = (*(u32*)(pNode + 0xA8) & 0x3FFFFFFF) | (split << 30);
            *(u32*)(pNode + 0xAC) = (*(u32*)(pNode + 0xAC) & ~0x3) | (split >> 2);
            *(u32*)(pNode + 0x50) = *(u32*)(pParent + 0x50);
            *(u8*)(pNode + 0x8D) = *(u8*)(pParent + 0xAF);
        }
    }

    bits40 = *(u32*)(pNode + 0x40) & 0xFFFE1FFF;
    bits3c = *(u32*)(pNode + 0x3C);
    duration = (u16)D_800591A8;
    *(u32*)(pNode + 0x44) = 0;
    *(u32*)(pNode + 0x48) = 0;
    *(u16*)(pNode + 0x34) = 0;
    *(u32*)(pNode + 0x24) = source;
    bits40 |= ((u32)type & 0xF) << 13;
    *(u32*)(pNode + 0x40) = bits40;
    bits3c = (bits3c & ~0x3) | ((u32)mode & 0x3);
    *(u32*)(pNode + 0x3C) = bits3c;
    *(u16*)(pNode + 0x82) = duration;
    *(u32*)(pNode + 0x0) = (u32)pPosition[0] << 16;
    *(u32*)(pNode + 0x4) = (u32)pPosition[1] << 16;
    *(u32*)(pNode + 0x8) = (u32)pPosition[2] << 16;
    func_80023538(pNode, pEntry);
    func_80024730(pWrapper);
    return pWrapper;
}

extern s32 D_800591B8;
extern void* func_80024524(void* pAnimPackage, s16 texX, s16 texY, s16 clutX, s16 clutY, s16 arg5);
void* func_8002435C(void* pSpriteData, void* pAnimPackage, s16 texX, s16 texY,
                    s16 clutX, s16 clutY, s16 arg6);

void* func_80024294(void* pAnimPackage, s16 texX, s16 texY, s16 clutX, s16 clutY, s16 arg5, s32 arg6) {
    void* pSpriteData;

    D_800591B8 = arg6;
    pSpriteData = func_80024524(pAnimPackage, texX, texY, clutX, clutY, arg5);
    D_800591B8 = 0;

    return pSpriteData;
}

void* func_800242F4(void* pSpriteData, void* pAnimPackage, s16 texX, s16 texY, s16 clutX, s16 clutY, s16 arg6, s32 flags) {
    void* pResult;

    /* Retail 800242F4 preserves a0/a1 and forwards five signed halfwords;
     * the eighth argument (caller SP+1C) supplies the temporary flags. */
    D_800591B8 = flags;
    pResult = func_8002435C(pSpriteData, pAnimPackage, texX, texY, clutX, clutY, arg6);
    D_800591B8 = 0;
    return pResult;
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

/* Transcribed from asm/slus_006.64/nonmatchings/system/temp1/func_80024730.s
 * (0x80024730-0x800248D4; the next retail symbol func_800248D4 starts there, so
 * the earlier "0x800248C0" note here understated the body by the tail's last
 * three instructions). Animation-state dispatch keyed on bits 13-16 of the
 * state word at pOwner+0x38+0x40; retail's jtbl_800186A4 maps cases 0-6 and 14 to
 * "no state change", 7 to re-arming the timer callback with func_80022E8C, 8/9 to
 * writing a new sprite byte (0x68 / +0x36=3 with 0x60), 10/11 to clearing the
 * +0x34 counter and copying the three-word state block from D_8006F99C /
 * D_8006F9AC, and 12/13 to stepping the mode down by two (rewriting the flag
 * word's bits 13-16) plus the D_8006F99C block. Every path ends in
 * func_80025224(pOwner+0x1C, mode) with the post-transition mode.
 *
 * Case 7 binds the timer callback on **pOwner itself** (the wrapper), not on the
 * inner node: at .L80024878 the `jal TimerWorkListSetTaskCallback` still has the
 * incoming a0 = pOwner, and `addu $a0, $s2, $zero` (s2 = pOwner+0x1C) only sits
 * in the delay slot of the *following* `j .L800248B0`, i.e. it feeds the tail
 * func_80025224 call. The first version of this body passed pOwner+0x1C here;
 * the retail differential caught it.
 * The func_800BC158 cases likewise store before the call, because retail puts
 * those stores in the `jal` delay slot: cases 10/11 `sh $zero,0x34($s0)` and
 * 12/13 both `sh $v1,0x34($s0)` and the masked `sw $a1,0x40($s0)` execute before
 * func_800BC158 runs (the first version stored them afterwards). */
extern u32 D_8006F99C[];
extern u32 D_8006F9AC[];

void func_80024730(u8* pOwner) {
    u8* pState = pOwner + 0x38;
    u8* pInner = pOwner + 0x1C;
    s32 mode = (*(u32*)(pState + 0x40) >> 13) & 0xF;
    u32 flags;

    switch (mode) {
    case 7:
        TimerWorkListSetTaskCallback(pOwner, func_80022E8C);
        break;
    case 8:
        *(u8*)(pState + 0x2B) = 0x68;
        *(u16*)(pState + 0x34) = 1;
        break;
    case 9:
        *(u16*)(pState + 0x36) = 3;
        *(u8*)(pState + 0x2B) = 0x60;
        *(u16*)(pState + 0x34) = 1;
        break;
    case 10:
        *(u16*)(pState + 0x34) = 0;
        func_800BC158(pOwner);
        *(u32*)(pState + 0x00) = D_8006F99C[0];
        *(u32*)(pState + 0x04) = D_8006F99C[1];
        *(u32*)(pState + 0x08) = D_8006F99C[2];
        break;
    case 11:
        *(u16*)(pState + 0x34) = 0;
        func_800BC158(pOwner);
        *(u32*)(pState + 0x00) = D_8006F9AC[0];
        *(u32*)(pState + 0x04) = D_8006F9AC[1];
        *(u32*)(pState + 0x08) = D_8006F9AC[2];
        break;
    case 12:
    case 13:
        flags = *(u32*)(pState + 0x40);
        *(u16*)(pState + 0x34) = 1;
        mode = (((flags >> 13) & 0xF) - 2) & 0xF;
        *(u32*)(pState + 0x40) = (flags & 0xFFFE1FFF) | ((u32)mode << 13);
        func_800BC158(pOwner);
        *(u32*)(pState + 0x00) = D_8006F99C[0];
        *(u32*)(pState + 0x04) = D_8006F99C[1];
        *(u32*)(pState + 0x08) = D_8006F99C[2];
        break;
    default:
        break;
    }

    func_80025224(pInner, mode);
}

extern u8 D_800591AD;
extern s32 g_WorkListCurTimer;
extern void func_800C11CC(void* pSpriteData);
extern void func_80022D44(void* pSpriteData);
extern void func_8001FBE4(void* pSpriteData, u32 opcodeIndex, void* operands);

void func_800248D4(void* pSpriteData) {
    u8* pData = pSpriteData;
    u8* pc;
    u8 opcode;

    if (D_800591AD) {
        func_800C11CC(pData);
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
#ifdef XENO_PC_PORT
        fprintf(stderr, "{\"event\":\"sprite_animation_dedicated_unimplemented\",\"opcode\":%u,\"pc\":\"%p\"}\n", (unsigned)opcode, (void*)pc);
        fflush(stderr);
#endif
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

#ifdef XENO_PC_PORT
    fprintf(stderr, "{\"event\":\"sprite_animation_opcode_unimplemented\",\"opcode\":%u,\"pc\":\"%p\"}\n", (unsigned)opcode, (void*)pc);
    fflush(stderr);
#endif
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

/* Transcribed from asm/slus_006.64/nonmatchings/system/temp1/GfxFreeWorkBuffers.s
 * (0x80024FB8-0x80024FE4, 11 instructions): free the work buffers and reset the
 * sprite/image lists. The port keeps its own owner in
 * pc_port/src/world_map_teardown_7299c.c, so the matching definition is weak
 * under XENO_PC_PORT and the port's strong one wins the link. */
extern void func_8001D2A4(void);

#ifdef XENO_PC_PORT
__attribute__((weak))
#endif
void GfxFreeWorkBuffers(void) {
    HeapFree(g_GfxWorkBuffers);
    func_8001D2A4();
}
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
#ifdef XENO_PC_PORT
static u8* SpriteRenderAddress(uintptr_t address);
#endif

void func_800250E0(int context) {
    u8* pPrimBuffer = context ? (u8*)g_GfxWorkBuffer2 : (u8*)g_GfxWorkBuffers;
    u32* pListHead = context ? &D_80059304 : &D_80059300;
    u8* pCurEntry = (u8*)(uintptr_t)*pListHead;
#ifdef XENO_PC_PORT
    /* Battle effects serialize guest pointers into the deferred-free list.
     * Resolve each packed slot independently, including mixed native nodes. */
    pCurEntry = SpriteRenderAddress((uintptr_t)pCurEntry);
#endif

    g_GfxCurContext = context;
    g_GfxCurWorkBuffer = pPrimBuffer;
    D_80059524 = (uintptr_t)pPrimBuffer;
    g_GfxCurWorkBufferEnd = pPrimBuffer + g_GfxWorkBufferSize;

    while (pCurEntry != NULL) {
#ifdef XENO_PC_PORT
        HeapFree(SpriteRenderAddress(*(u32*)(pCurEntry + 0x0)));
        pCurEntry = SpriteRenderAddress(*(u32*)(pCurEntry + 0x4));
#else
        HeapFree((void*)(uintptr_t)*(u32*)(pCurEntry + 0x0));
        pCurEntry = (u8*)(uintptr_t)*(u32*)(pCurEntry + 0x4);
#endif
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


/* func_80025180: GfxQueueWorkEntry -- bump an 8-byte node off
 * g_GfxCurWorkBuffer (retail advances the head by 8 even when it is NULL) and
 * push it onto the D_80059300[g_GfxCurContext] list, pData at +0 and the
 * previous head at +4.  Not decompiled here on purpose: retail reaches
 * g_GfxCurWorkBuffer and g_GfxCurContext through $gp in one instruction each
 * because they are DEFINED small objects in its TU, while they are `extern`
 * here, so a C body costs three extra instructions (84 vs 72 bytes).  The PORT
 * already owns this symbol as a host-safe override in
 * pc_port/src/game_overrides.c (g_GfxCurWorkBuffer is a u32 there, not a
 * pointer), so adding a body here would be a duplicate definition. */
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1", func_80025180);
#endif

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
/* Transcribed from asm/slus_006.64/nonmatchings/system/temp1/func_80025224.s
 * (0x80025224-0x80025258). Loads the handler pointer from the retail callback
 * table at D_8004FD40 (16 entries; see the table listing below) and hands it to
 * WorkListSetTaskCallback. NOTE: in the current build that table symbol is
 * parked at 0x8004FDBC instead of 0x8004FD40 (data-symbol placement, see
 * ACTIVE_HANDOFF), so the emitted immediate is not yet retail-identical. */
extern void (*D_8004FD40[])(void*);
void func_80025224(void* pTask, int handlerIndex) {
#ifdef TEMP1_25224_MUTANT_INDEX_PLUS1
    WorkListSetTaskCallback(pTask, D_8004FD40[handlerIndex + 1]);
#else
    WorkListSetTaskCallback(pTask, D_8004FD40[handlerIndex]);
#endif
}
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

static u8* SpriteRenderAddress(uintptr_t address) {
#ifdef XENO_PC_PORT
    if ((address & ~(uintptr_t)0x1FFFFFu) == 0x80000000u ||
        (address & ~(uintptr_t)0x1FFFFFu) == 0xA0000000u) {
        return PSX_ADDR(address);
    }
#endif
    return (u8*)address;
}

/* Retail 80025258..80025418: shared render callback for types 0/5/6/14.
 * Task fields and OT entries remain four bytes on the native host. */
void func_80025258(u8* pEntry) {
    u8* pSprite = SpriteRenderAddress(*(u32*)(pEntry + 0x04));
    u32 flagsB0 = *(u32*)(pSprite + 0xB0);
    s32 depth;
    u32 flags3C;
    SVECTOR pos;
    long pxy = 0;
    long flg = 0;

    if ((flagsB0 >> 8) & 1) {
#ifdef XENO_PC_PORT
        if (*(u8*)PSX_ADDR(0x800C3664u) != 0) return;
#else
        if (D_800C3664 != 0) return;
#endif
    }

    pos.vx = *(s16*)(pSprite + 0x02);
    pos.vy = *(s16*)(pSprite + 0x06);
    pos.vz = *(s16*)(pSprite + 0x0A);
    SetRotMatrix(&D_8004FBB8);
    SetTransMatrix(&D_8004FBB8);
    depth = (s32)RotTransPers(&pos, &pxy, &pxy, &flg) >> (D_80050100 & 31);
    depth = (s32)((u32)depth + (u32)(s32)*(s16*)(pSprite + 0x30));
    if (flg & 0x8000) {
        depth = 0;
    }

    flags3C = *(u32*)(pSprite + 0x3C);
    *(u16*)(pSprite + 0x2E) = (u16)depth;
    if ((flags3C >> 24) & 1) {
        VECTOR trans;
        func_80022038(pSprite);
        trans.vx = *(s16*)(pSprite + 0x02);
        trans.vy = *(s16*)(pSprite + 0x06);
        trans.vz = *(s16*)(pSprite + 0x0A);
        TransMatrix((MATRIX*)(SpriteRenderAddress(*(u32*)(pSprite + 0x20)) + 0xC), &trans);
        SetRotMatrix((MATRIX*)(SpriteRenderAddress(*(u32*)(pSprite + 0x20)) + 0xC));
        SetTransMatrix((MATRIX*)(SpriteRenderAddress(*(u32*)(pSprite + 0x20)) + 0xC));
        /* The transform helper can change flags: retail reloads them here. */
        depth = (*(u32*)(pSprite + 0x3C) & 0x02000000u)
                    ? 0xFFF : *(s16*)(pSprite + 0x30);
        if ((u32)depth - 1u < 0xFFFu) {
            func_8001E3D8(pSprite,
                SpriteRenderAddress((uintptr_t)g_GfxCurOT) + ((u32)depth << 2));
        }
    } else {
        if ((flags3C >> 29) & 1) {
            u8* pSub = SpriteRenderAddress(*(u32*)(pSprite + 0x70));
            depth = *(s16*)(pSub + 0x2E);
        }
        if ((u32)depth - 1u < 0xFFFu) {
            func_8001E298(pSprite,
                SpriteRenderAddress((uintptr_t)g_GfxCurOT) + ((u32)depth << 2));
        }
    }
}

extern s32 D_80050100;

#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1", func_8002541C);
#else
/* Retail 8002541C..80025540: projected TILE_1 (12-byte packet, tag len 2)
 * followed by an 8-byte draw-mode prim. Sibling of func_80025544. */
void func_8002541C(u8* pEntry) {
    u8* pSprite = SpriteRenderAddress(*(u32*)(pEntry + 0x04));
    u32 cursor, next, otOffset, otAddress, color;
    u8* pTile;
    u8* pMode;
    SVECTOR v0;
    long flag = 0;
    s32 depth;

    if (*(u16*)(pSprite + 0x34) != 0) {
        return;
    }
    cursor = (u32)(uintptr_t)g_GfxCurWorkBuffer;
    next = cursor + 0xCu;
    if (next >= (u32)(uintptr_t)g_GfxCurWorkBufferEnd) {
        return;
    }
    pTile = SpriteRenderAddress(cursor);
    v0.vx = *(s16*)(pSprite + 0x02);
    v0.vy = *(s16*)(pSprite + 0x06);
    v0.vz = *(s16*)(pSprite + 0x0A);
    v0.pad = 0;
    g_GfxCurWorkBuffer = (void*)(uintptr_t)next;
    SetRotMatrix(&D_8004FBB8);
    SetTransMatrix(&D_8004FBB8);
    depth = (s32)RotTransPers(&v0, (long*)(pTile + 8), &flag, &flag);
    depth >>= (D_80050100 & 31);
    *(u16*)(pSprite + 0x2E) = (u16)depth;
    pTile[3] = 2;
    color = *(u32*)(pSprite + 0x28);
    *(u32*)(pTile + 4) = color;
    otAddress = (u32)(uintptr_t)g_GfxCurOT;
    otOffset = (u32)depth << 2;
    PcPort_AddPrimDomainAware(SpriteRenderAddress(otAddress + otOffset), pTile);

    cursor = (u32)(uintptr_t)g_GfxCurWorkBuffer;
    next = cursor + 8u;
    if (next >= (u32)(uintptr_t)g_GfxCurWorkBufferEnd) {
        return;
    }
    g_GfxCurWorkBuffer = (void*)(uintptr_t)next;
    pMode = SpriteRenderAddress(cursor);
    pMode[3] = 1;
    *(u32*)(pMode + 4) = 0xE1000000u | (*(u32*)(pSprite + 0x3C) & 0x60u);
    PcPort_AddPrimDomainAware(SpriteRenderAddress(otAddress + otOffset), pMode);
}
#endif

/* Retail 80025544..80025710: projected square TILE followed by draw mode. */
void func_80025544(u8* pEntry) {
    u8* pSprite = SpriteRenderAddress(*(u32*)(pEntry + 0x04));
    u16 size;
    u32 cursor, next, otOffset, otAddress, color;
    u32 originX, originY;
    s32 depth, dimension, halfSize;
    u8* pTile;
    u8* pMode;
    SVECTOR v0, v1;
    long xy0 = 0, xy1 = 0, xy2 = 0, sharedFlag = 0;

    if (*(u16*)(pSprite + 0x34) != 0) return;
    size = *(u16*)(pSprite + 0x36);
    cursor = (u32)(uintptr_t)g_GfxCurWorkBuffer;
    next = cursor + 0x10u;
    if (next >= (u32)(uintptr_t)g_GfxCurWorkBufferEnd) return;
    pTile = SpriteRenderAddress(cursor);

    v0.vx = *(s16*)(pSprite + 0x02);
    v0.vy = *(s16*)(pSprite + 0x06);
    v0.vz = *(s16*)(pSprite + 0x0A);
    v0.pad = 0;
    g_GfxCurWorkBuffer = (void*)(uintptr_t)next;
    SetRotMatrix(&D_8004FBB8);
    SetTransMatrix(&D_8004FBB8);
    v1 = v0;
    v1.vx = (s16)((u16)v0.vx + size);
    depth = (s32)RotTransPers3(&v0, &v1, &v0, &xy0, &xy1, &xy2,
                              &sharedFlag, &sharedFlag);
    *(u32*)(pTile + 0x08) = (u32)xy0;
    depth >>= (D_80050100 & 31);
    *(u16*)(pSprite + 0x2E) = (u16)depth;

    dimension = (s32)(s16)(u32)xy1 - (s32)*(s16*)(pTile + 0x08);
    originX = *(u16*)(pTile + 0x08);
    if (dimension == 0) dimension = 1;
    if (dimension < 0) dimension = -dimension;
    halfSize = dimension >> 1;
    originY = *(u16*)(pTile + 0x0A);
    *(u16*)(pTile + 0x08) = (u16)(originX - (u32)halfSize);
    *(u16*)(pTile + 0x0A) = (u16)(originY - (u32)halfSize);
    color = *(u32*)(pSprite + 0x28);
    pTile[3] = 3;
    *(u32*)(pTile + 4) = color;
    otAddress = (u32)(uintptr_t)g_GfxCurOT;
    otOffset = (u32)depth << 2;
    *(u16*)(pTile + 0x0E) = (u16)dimension;
    *(u16*)(pTile + 0x0C) = (u16)dimension;
#ifdef XENO_PC_PORT
    PcPort_AddPrimDomainAware(SpriteRenderAddress(otAddress + otOffset), pTile);
#else
    AddPrim(SpriteRenderAddress(otAddress + otOffset), pTile);
#endif

    cursor = (u32)(uintptr_t)g_GfxCurWorkBuffer;
    next = cursor + 8u;
    if (next >= (u32)(uintptr_t)g_GfxCurWorkBufferEnd) return;
    g_GfxCurWorkBuffer = (void*)(uintptr_t)next;
    pMode = SpriteRenderAddress(cursor);
    pMode[3] = 1;
    otAddress = (u32)(uintptr_t)g_GfxCurOT;
    *(u32*)(pMode + 4) = 0xE1000000u | (*(u32*)(pSprite + 0x3C) & 0x60u);
#ifdef XENO_PC_PORT
    PcPort_AddPrimDomainAware(SpriteRenderAddress(otAddress + otOffset), pMode);
#else
    AddPrim(SpriteRenderAddress(otAddress + otOffset), pMode);
#endif
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

/* Transcribed from asm/slus_006.64/nonmatchings/system/temp1/func_800257F0.s
 * (0x800257F0-0x800258A0 ... 0x80025A6C). Sprite transform/render setup:
 * func_80022038 on the sprite, an optional colour/light matrix block built from
 * the +0x20 data (RotMatrix + two MulMatrix0 with D_8004FDA0, SetBackColor
 * 0x202020, SetColorMatrix/SetLightMatrix then PopMatrix), the +2/+6/+0xA
 * translation via TransMatrix, either CompMatrix(D_8004FBB8) or the raw matrix
 * for SetRotMatrix, SetTransMatrix, an optional geom-offset override
 * (ReadGeomOffset/SetGeomOffset 0xA0,0x70) and finally func_800B1F6C with either
 * the +0x30 half-word and D_80050100 unchanged, or D_80050100 = 0x10 with 0xFEC
 * (restored afterwards); the geom offset is restored when the flag word was
 * negative. */
extern void func_80022038(void* p);
extern u16 D_8004FD80[];
extern void func_800B1F6C();
extern MATRIX D_8004FDA0;

void func_800257F0(u8* pArg) {
    u8* pSprite = *(u8**)(pArg + 4);
    u8* pData;
    MATRIX matA;
    MATRIX matB;
    VECTOR trans;
    MATRIX comp;
    s32 saved;
    s32 geomX;
    s32 geomY;
    u8 flag;
    s32 halved;

    func_80022038(pSprite);
    pData = *(u8**)(pSprite + 0x20);
    if (*(u32*)(pData + 0x34) == 0) {
        return;
    }

    if (((*(u32*)(pSprite + 0x40) >> 1) & 1) != 0) {
        PushMatrix();
        D_8004FD80[0] = *(u16*)(pData + 0x4C);
        D_8004FD80[3] = *(u16*)(pData + 0x4E);
        D_8004FD80[6] = *(u16*)(pData + 0x50);
        RotMatrix(pData + 0x44, &matA);
        MulMatrix0(&matA, pData + 0x0C, &matA);
        MulMatrix0(&D_8004FDA0, &matA, &matB);
        SetBackColor(0x20, 0x20, 0x20);
        SetColorMatrix((u32*)D_8004FD80);
        SetLightMatrix(&matB);
        PopMatrix();
    }

    trans.vx = *(s16*)(pSprite + 0x02);
    trans.vy = *(s16*)(pSprite + 0x06);
    trans.vz = *(s16*)(pSprite + 0x0A);
    TransMatrix(pData + 0x0C, &trans);

    if ((*(u8*)(pSprite + 0x3F) & 1) == 0) {
        CompMatrix(&D_8004FBB8, pData + 0x0C, &comp);
        SetRotMatrix(&comp);
        SetTransMatrix(&comp);
    } else {
        SetRotMatrix(pData + 0x0C);
        SetTransMatrix(pData + 0x0C);
    }

    flag = *(u8*)(pSprite + 0x3C);
    if (*(s32*)(pSprite + 0x3C) < 0) {
        ReadGeomOffset(&geomX, &geomY);
        SetGeomOffset(0xA0, 0x70);
    }
    halved = (s32)((*(u32*)(pSprite + 0x3C)) >> 25) & 1;

    if (halved == 0) {
        ((void (*)(s32, void*, void*, s32, s32, s32))func_800B1F6C)(*(s32*)(pData + 0x34),
                      *(void**)((u8*)pData + (g_GfxCurContext << 2) + 0x2C),
                      g_GfxCurOT, 0,
                      (s32)*(s16*)(pSprite + 0x30), (s32)(flag >> 5));
    } else {
        saved = D_80050100;
        D_80050100 = 0x10;
        ((void (*)(s32, void*, void*, s32, s32, s32))func_800B1F6C)(*(s32*)(pData + 0x34),
                      *(void**)((u8*)pData + (g_GfxCurContext << 2) + 0x2C),
                      g_GfxCurOT, 0, 0xFEC, (s32)(flag >> 5));
        D_80050100 = saved;
    }

    if (*(s32*)(pSprite + 0x3C) < 0) {
        SetGeomOffset(geomX, geomY);
    }
}

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

#ifdef XENO_PC_PORT
#include "psx_memory.h"
/* Retail 80025FA8..80026338: atlas entry to double-buffered FT4 packets.
 * The two fixed data addresses refer to the loaded retail EXE image, not
 * replacement artwork. Keep hardware calls and their ordering intact. */
s32 func_80025FA8(u8* table, s32 index, u8* packets, s32 buffer,
                  s32 screenX, s32 screenY, s32 scaleX, s32 scaleY, s32 angle)
{
    extern MATRIX* ScaleMatrixL(MATRIX*, VECTOR*);
    extern void ReadGeomOffset(long*, long*);
    extern s32 ReadGeomScreen(void);
    MATRIX matrix;
    VECTOR scale;
    SVECTOR* corners = PSX_ADDR(0x8004FDC0u);
    u8* entry;
    long oldX = 0, oldY = 0, screen;
    s32 i;

    _Static_assert(sizeof(MATRIX) == 32, "retail atlas matrix size");
    _Static_assert(sizeof(POLY_FT4) == 40, "retail atlas packet size");
    __builtin_memcpy(&matrix, PSX_ADDR(0x800188CCu), 32);
    scale.vx = (s16)scaleX; scale.vy = (s16)scaleY; scale.vz = 0x1000;
    PushMatrix();
    ScaleMatrixL(&matrix, &scale);
    RotMatrixZ((s16)angle, &matrix);
    ReadGeomOffset(&oldX, &oldY);
    screen = ReadGeomScreen();
    SetGeomOffset((s16)screenX, (s16)screenY);
    SetGeomScreen(0x1000);
    SetRotMatrix(&matrix);
    SetTransMatrix(&matrix);
    entry = table + *(u16*)(table + index * 2 + 4);
    for (i = 0; i != *(s16*)entry; i++) {
        u8* item = entry + 4 + i * 28;
        POLY_FT4* poly = (POLY_FT4*)(packets + i * 80 + buffer * 40);
        s32 u, v, width, height, x, y, rotation;
        long interpolation = 0, flag = 0;
        SetPolyFT4(poly);
        SetSemiTrans(poly, 0);
        SetShadeTex(poly, 1);
        poly->tpage = GetTPage(*(s16*)(item+16), 0, *(s16*)(item+22), *(s16*)(item+24));
        poly->clut = GetClut(*(s16*)(item+18), *(s16*)(item+20));
        width = *(u16*)(item+4); height = *(u16*)(item+6);
        x = *(u16*)(item+8); y = *(u16*)(item+10);
        corners[0].vx = corners[3].vx = item[26] ? x + width : x;
        corners[1].vx = corners[2].vx = item[26] ? x : x + width;
        corners[0].vy = corners[1].vy = item[27] ? y + height : y;
        corners[2].vy = corners[3].vy = item[27] ? y : y + height;
        /* Projection order is perimeter order; FT4 storage is grid order.
         * XY outputs are packed words; p/FLAG are actual host long objects. */
        RotTransPers4(corners, corners+1, corners+2, corners+3,
            (long*)&poly->x0, (long*)&poly->x1, (long*)&poly->x3, (long*)&poly->x2,
            &interpolation, &flag);
        u = *(u16*)item; v = *(u16*)(item+2);
        width = *(u16*)(item+4); height = *(u16*)(item+6);
        rotation = (u16)angle & 0xfff;
        if (rotation == 0xc00) u--;
        if (rotation == 0) {
            if (poly->x3 < poly->x0) {
                u--;
                if ((s16)u < 0) { u = 0; width--; }
            }
            if (poly->y3 < poly->y0) {
                v--;
                if ((s16)v < 0) { v = 0; height--; }
            }
        }
        poly->u0 = u; poly->v0 = v;
        poly->u1 = u + width; poly->v1 = v;
        poly->u2 = u; poly->v2 = v + height;
        poly->u3 = u + width; poly->v3 = v + height;
    }
    SetGeomOffset(oldX, oldY);
    SetGeomScreen(screen);
    PopMatrix();
    return *(s16*)entry;
}
#else
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1", func_80025FA8);
#endif

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
#ifdef TEMP1_26338_MUTANT_SHIFT3
    shift++;
#endif
    *pTPage = *(s16*)(item + 0x10);
    *pClutX = *(s16*)(item + 0x12);
    *pClutY = *(s16*)(item + 0x14);
    *pTexX = (s32)(s16)(*(u16*)(item + 0x16) & 0xFFC0) + shift;
    *pTexY = (s32)(s16)(*(u16*)(item + 0x18) & 0xFF00) + *(s16*)(item + 2);
}
#else
/* Transcribed from asm/slus_006.64/nonmatchings/system/temp1/func_80026338.s
 * (0x80026338-0x800263E4). Record getter: `index` selects a u16 offset from
 * `base`; the resulting record holds a signed first field, a u16 at +4 whose
 * (s16) value is shifted right by 2 or 4 depending on whether the signed +0x14
 * field is non-zero, four more signed fields, and two masked sums (0xFFC0 at
 * +0x1A plus the shift, 0xFF00 at +0x1C plus the signed +6 field). All six
 * results go to the caller's out-pointers. */
void func_80026338(u8* base, s32 index, s32* pOut0, s32* pOut1, s32* pOut2,
                   s32* pOut3, s32* pOut4, s32* pOut5) {
    u16* pEntry = (u16*)(base + index * 2);
    u8* pRec = base + pEntry[2];
    u16 n;
    s32 shift;

    *pOut0 = *(s16*)(pRec + 0);
    n = *(u16*)(pRec + 4);
#ifdef TEMP1_26338_MUTANT_SHIFT3
    shift = (s16)n >> 3;
#else
    if (*(s16*)(pRec + 0x14) != 0) {
        shift = (s16)n >> 2;
    } else {
        shift = (s16)n >> 4;
    }
#endif
    *pOut1 = *(s16*)(pRec + 0x14);
    *pOut2 = *(s16*)(pRec + 0x16);
    *pOut3 = *(s16*)(pRec + 0x18);
    *pOut4 = (s16)(*(u16*)(pRec + 0x1A) & 0xFFC0) + shift;
    *pOut5 = (s16)(*(u16*)(pRec + 0x1C) & 0xFF00) + *(s16*)(pRec + 6);
}
#endif

/* Retail 800263E4..8002675C. The last two arguments are independent
 * byte-sized axis flips; packet coordinates narrow before the UV test. */
s32 func_800263E4(u8* table, s32 index, void* packets, s32 buffer,
                  s32 x, s32 y, s32 scale, s32 flipX, s32 flipY) {
    u8* entry = table + *(u16*)(table + index * 2 + 4);
    u8* output = packets;
    s32 i = 0;
    s32 itemOffset = 4;
    s32 factor = (u16)scale;
    if (*(s16*)entry != 0) {
        do {
            u8* item = entry + itemOffset;
            POLY_FT4* poly = (POLY_FT4*)(output + buffer * 40);
            s32 dx, dy, width, height, product;
            s32 u, v, uw, vh;
            s32 left, right, top, bottom;
            product = *(s16*)(item + 8) * factor;
            if (product < 0) product += 0xFFF;
            dx = product >> 12;
            product = *(s16*)(item + 10) * factor;
            if (product < 0) product += 0xFFF;
            dy = product >> 12;
            product = *(s16*)(item + 4) * factor;
            if (product < 0) product += 0xFFF;
            width = product >> 12;
            product = *(s16*)(item + 6) * factor;
            if (product < 0) product += 0xFFF;
            height = product >> 12;
            SetPolyFT4(poly);
            SetSemiTrans(poly, 0);
            SetShadeTex(poly, 1);
            poly->tpage = GetTPage(*(s16*)(item + 16), 0,
                                   *(s16*)(item + 22), *(s16*)(item + 24));
            poly->clut = GetClut(*(s16*)(item + 18), *(s16*)(item + 20));
            if ((u8)flipX) { dx = -dx; width = -width; }
            if ((u8)flipY) { dy = -dy; height = -height; }
            u = *(u16*)item; v = *(u16*)(item + 2);
            uw = *(u16*)(item + 4); vh = *(u16*)(item + 6);
            left = (u16)x + dx; right = left + width;
            if (item[26]) {
                poly->x0 = poly->x2 = right;
                poly->x1 = poly->x3 = left;
            } else {
                poly->x0 = poly->x2 = left;
                poly->x1 = poly->x3 = right;
            }
            top = (u16)y + dy; bottom = top + height;
            if (item[27]) {
                poly->y0 = poly->y1 = bottom;
                poly->y2 = poly->y3 = top;
            } else {
                poly->y0 = poly->y1 = top;
                poly->y2 = poly->y3 = bottom;
            }
            if (poly->x3 < poly->x0) {
                --u;
                if ((s16)u < 0) { u = 0; --uw; }
            }
            if (poly->y3 < poly->y0) {
                --v;
                if ((s16)v < 0) { v = 0; --vh; }
            }
            poly->u0 = poly->u2 = u; poly->u1 = poly->u3 = u + uw;
            poly->v0 = poly->v1 = v; poly->v2 = poly->v3 = v + vh;
            itemOffset += 28;
            output += 80;
            ++i;
        } while (i != *(s16*)entry);
    }
    return *(s16*)entry;
}

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

/* Retail 80026BA4..80026DCC takes five arguments. Packets come from the
 * current graphics work buffer; the fifth argument is the ordering-table
 * entry itself. A sixth stack word belongs to the caller, not this API. */
void func_80026BA4(u8* pTable, s32 index, s32 ofsX, s32 ofsY, void* ot) {
    u8* pDesc = pTable + *(u16*)(pTable + index * 2 + 4);
    s32 count = *(s16*)pDesc;
    s32 i;

    /* Retail uses an unsigned strict comparison, rejecting an exact fit. */
    if ((u32)((uintptr_t)g_GfxCurWorkBuffer + (u32)count * 40u) >=
        (u32)(uintptr_t)g_GfxCurWorkBufferEnd || count == 0) {
        return;
    }
    for (i = 0; i != count; i++) {
        u8* item = pDesc + 4 + i * 28;
        u8* poly = g_GfxCurWorkBuffer;
        s32 u = *(s16*)item;
        s32 v = *(s16*)(item + 2);
        s32 width = *(s16*)(item + 4);
        s32 height = *(s16*)(item + 6);
        u32 x = (u32)*(s16*)(item + 8) + (u32)ofsX;
        u32 y = (u32)*(s16*)(item + 10) + (u32)ofsY;
        s32 mode = *(s16*)(item + 16);
        s32 shift = mode == 0 ? u >> 4 : u >> 2;
        s32 texX = (s16)(*(u16*)(item + 22) & 0xFFC0) + shift;
        s32 texY = (s16)(*(u16*)(item + 24) & 0xFF00) + v;

        g_GfxCurWorkBuffer = poly + 40;
        poly[3] = 9;
        poly[7] = 0x2D;
        *(u16*)(poly + 14) = GetClut(*(s16*)(item + 18), *(s16*)(item + 20));
        *(u16*)(poly + 22) = GetTPage(mode, 0, texX, texY);
        *(u16*)(poly + 8) = *(u16*)(poly + 24) = x;
        *(u16*)(poly + 16) = *(u16*)(poly + 32) = x + (u32)width;
        *(u16*)(poly + 10) = *(u16*)(poly + 18) = y;
        *(u16*)(poly + 26) = *(u16*)(poly + 34) = y + (u32)height;
        poly[12] = poly[28] = u;
        poly[20] = poly[36] = u + width;
        poly[13] = poly[21] = v;
        poly[29] = poly[37] = v + height;
        AddPrim(ot, poly);
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
