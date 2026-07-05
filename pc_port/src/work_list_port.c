#include "common.h"
#include <stdint.h>

typedef void (*WorkListCallback_t)(void*);

typedef struct WorkListEntry {
    struct WorkListEntry* unk0;
    void* unk4;
    WorkListCallback_t onTriggerCallback;
    WorkListCallback_t onFreeCallback;
    u32 unk10 : 29;
    u32 unk10_1 : 1;
    u32 unk10_2 : 1;
    u32 unk10_3 : 1;
    u32 unk14 : 29;
    u32 unk14_1 : 1;
    u32 unk14_2 : 1;
    u32 unk14_3 : 1;
    struct WorkListEntry* pNext;
} WorkListEntry;

extern s32 D_80059190;
extern s32 g_NumTimerWorkListEntries;
extern s32 g_NumWorkListEntries;
extern s32 g_WorkListCurTimer;
extern WorkListEntry* D_800594C0;
extern WorkListEntry* g_TimerWorkList;
extern WorkListEntry* D_80059590;
extern WorkListEntry* g_WorkList;
extern short D_80059494;
extern void func_8001DAE8(void* arg0, u16 arg1, u32 arg2);
extern void func_800234AC(void* pSpriteData);
extern void func_8001F8E8(void* pSpriteData, u16 frameIndex, u32 animPackageAddr);
extern u8 D_8005A474[];
extern u8 D_8006BE10[];

void WorkListsReset(void) {
    g_TimerWorkList = NULL;
    g_WorkList = NULL;
    g_NumTimerWorkListEntries = 0;
    g_NumWorkListEntries = 0;
    g_WorkListCurTimer = 0;
}

void TimerWorkListUpdate(void) {
    WorkListCallback_t pFnCallback;
    u8* pEntry;

    if (g_WorkListCurTimer) {
        g_WorkListCurTimer--;
        if (g_WorkListCurTimer == 0) {
            D_80059494 = 0;
        }
        return;
    }

    D_80059590 = g_TimerWorkList;
    while (D_80059590 != NULL) {
        pEntry = (u8*)D_80059590;
        D_800594C0 = (WorkListEntry*)pEntry;
        D_80059590 = (WorkListEntry*)(uintptr_t)*(u32*)(pEntry + 0x18);
        pFnCallback = (WorkListCallback_t)(uintptr_t)*(u32*)(pEntry + 0x8);
        if (pFnCallback) {
            pFnCallback(pEntry);
        }
    }
}

void WorkListUpdate(void) {
    WorkListCallback_t pFnCallback;
    u8* pEntry;

    D_80059590 = g_WorkList;
    while (D_80059590 != NULL) {
        pEntry = (u8*)D_80059590;
        D_800594C0 = (WorkListEntry*)pEntry;
        D_80059590 = (WorkListEntry*)(uintptr_t)*(u32*)(pEntry + 0x18);
        pFnCallback = (WorkListCallback_t)(uintptr_t)*(u32*)(pEntry + 0x8);
        if (pFnCallback) {
            pFnCallback(pEntry);
        }
    }
}

void func_8001D298(void) {
    D_80059190 = 0;
}

void func_8001D2B0(void* pSpriteData, s16 frameIndex) {
    u8* pData = pSpriteData;

    if ((*(u32*)(pData + 0x3C) & 0x3) != 1) {
        *(s16*)(pData + 0x34) = 0;
        return;
    }

    if (((*(u32*)(pData + 0x40) >> 20) & 0x1) != 0) {
        u8* pBase = (u8*)(uintptr_t)*(u32*)(pData + 0x20);

        *(u32*)(pData + 0x40) &= ~0x100000;
        if (*(u32*)(pBase + 0x34) != 0) {
            func_800234AC(pData);
        }
    }

    if (((*(u32*)(pData + 0x40) >> 17) & 0x1) != 0 && D_80059190 != 0) {
        u8* pEntry;

        for (pEntry = (u8*)(uintptr_t)D_80059190; pEntry != NULL;
             pEntry = (u8*)(uintptr_t)*(u32*)((u8*)(uintptr_t)*(u32*)(pEntry + 0x20) + 0x38)) {
            if (pEntry == pData) {
                u8* pPackage = (u8*)(uintptr_t)*(u32*)(pData + 0x24);

                if (pPackage != D_8005A474 && pPackage != D_8006BE10 &&
                    (((*(u32*)(pData + 0x40) >> 19) & 0x1) == 0)) {
                    func_8001F8E8(pData, *(u16*)(pData + 0x34), *(u32*)(pData + 0x24));
                }

                *(s16*)(pData + 0x34) = frameIndex;
                return;
            }
        }
    }

    *(s16*)(pData + 0x34) = frameIndex;
    *(u32*)(pData + 0x40) |= 0x20000;
    *(u32*)((u8*)(uintptr_t)*(u32*)(pData + 0x20) + 0x38) = (u32)D_80059190;
    D_80059190 = (s32)(uintptr_t)pData;
}

void func_8001D468(void) {
    u8* pEntry = (u8*)(uintptr_t)D_80059190;

    while (pEntry != NULL) {
        u16 value34 = *(u16*)(pEntry + 0x34);

        if (value34 == 0) {
            *(u32*)(pEntry + 0x40) &= 0xFFFFFF03;
        } else {
            func_8001DAE8(pEntry, value34, *(u32*)(pEntry + 0x24));
        }

        pEntry = (u8*)(uintptr_t)*(u32*)((u8*)(uintptr_t)*(u32*)(pEntry + 0x20) + 0x38);
    }

    D_80059190 = 0;
}
