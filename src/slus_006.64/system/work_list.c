#include "common.h"
#include "system/memory.h"

#ifdef XENO_PC_PORT
#include <stdint.h>
#endif

/* A lot of this TU has been matched already, but due to SBSS/SDATA symbols
 * issues it's currently unable to be compiled in. */

// TU is compiled on GCC 2.7.2 or 2.6.0

typedef void (*WorkListCallback_t)(void*);

typedef struct {
    struct WorkListEntry* unk0; // 
    void* unk4; // pSpriteData, at least in some cases. Could be a more general pointer to data
    WorkListCallback_t onTriggerCallback;
    WorkListCallback_t onFreeCallback;

    // Flags, shrug
    u32 unk10: 29;
    u32 unk10_1: 1;
    u32 unk10_2: 1;
    u32 unk10_3 : 1;

    u32 unk14: 29;
    u32 unk14_1: 1;
    u32 unk14_2: 1;
    u32 unk14_3 : 1;

    struct WorkListEntry* pNext;
} WorkListEntry;


/*
// .sbss
s32 D_80059184;
int g_NumTimerWorkListEntries;
int g_NumWorkListEntries;
WorkListEntry* D_800594C0; // Last processed entry?
WorkListEntry* g_TimerWorkList;
WorkListEntry* D_80059590; // Next entry?
WorkListEntry* g_WorkList;

// other
extern u8 D_800591AF; // Heap alloc flag
extern int g_WorkListCurTimer; // Timer?
extern s32 D_80059464;
extern short D_80059494; // Set to 0 when timer above reaches 0

*/


INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/work_list", WorkListsFreeAllEntries);
/*
void WorkListFreeAllEntries(void) {
    WorkListEntry* pList;

    for (pList = g_TimerWorkList; g_TimerWorkList != NULL; pList = g_TimerWorkList) {
        pList->onFreeCallback(pList);
    }

    for (pList = g_WorkList; g_WorkList != NULL; pList = g_WorkList) {
        pList->onFreeCallback(pList);
    }
}
*/

extern s32 g_NumTimerWorkListEntries;
extern s32 g_NumWorkListEntries;
extern s32 g_WorkListCurTimer;
extern WorkListEntry* D_800594C0;
extern WorkListEntry* g_TimerWorkList;
extern WorkListEntry* D_80059590;
extern WorkListEntry* g_WorkList;
extern short D_80059494;

void WorkListsReset(void) {
    g_TimerWorkList = NULL;
    g_WorkList = NULL;
    g_NumTimerWorkListEntries = 0;
    g_NumWorkListEntries = 0;
    g_WorkListCurTimer = 0;
}
/*
void WorkListsReset(void) {
    g_TimerWorkList = NULL;
    g_WorkList = NULL;
    g_NumTimerWorkListEntries = 0;
    g_NumWorkListEntries = 0;
    g_WorkListCurTimer = 0;
}
*/

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
/*
void TimerWorkListUpdate(void) {
    WorkListCallback_t pFnCallback;
    WorkListEntry* pEntry;

    // Count down timer
    if (g_WorkListCurTimer) {
        g_WorkListCurTimer--;
        if (g_WorkListCurTimer == 0) {
            D_80059494 = 0;
        }
    
    // Timer has reached zero
    } else {
        D_80059590 = g_TimerWorkList;
        while (D_80059590) {
            pEntry = D_80059590;
            D_800594C0 = pEntry;
            D_80059590 = pEntry->pNext;
            pFnCallback = pEntry->onTriggerCallback;
            if (pFnCallback) {
                pFnCallback(pEntry);
            }
        }  
    }
}
*/

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
/*
void WorkListUpdate(void) {
    WorkListCallback_t pFnCallback;
    WorkListEntry* pEntry;

    D_80059590 = g_WorkList;
    while (D_80059590) {
        pEntry = D_80059590;
        D_800594C0 = pEntry;
        D_80059590 = pEntry->pNext;
        pFnCallback = pEntry->onTriggerCallback; 
        if (pFnCallback) {
            pFnCallback(pEntry);
        }
    }
}
*/

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/work_list", WorkListAddTask);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/work_list", WorkListAllocateTask);
/*
WorkListEntry* WorkListAllocateTask(void* data, int dataSize) {
    WorkListEntry* pEntry;

    pEntry = HeapAlloc(dataSize + sizeof(WorkListEntry), D_800591AF);
    func_8001CA58(data, pEntry);
    pEntry->onFreeCallback = &TimerWorkListDeleteTask;
    return pEntry;
}
*/

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/work_list", WorkListRemoveTask);
/*
void WorkListRemoveTask(WorkListEntry* pTargetEntry) {
    WorkListEntry* pPrevEntry;
    WorkListEntry* pCurEntry;

    pPrevEntry = NULL;
    for (pCurEntry = g_WorkList; pCurEntry != NULL; pCurEntry = pCurEntry->pNext) {
        if (pCurEntry == pTargetEntry) {
            if (pPrevEntry) {
                pPrevEntry->pNext = pCurEntry->pNext;
            } else {
                g_WorkList = pCurEntry->pNext;
            }
            
            if (D_80059590 == pTargetEntry) {
                D_80059590 = pTargetEntry->pNext;
            }

            break;
        }

        pPrevEntry = pCurEntry;
    } 
    
    // Target entry was not found
    if (pCurEntry == NULL) {
        g_NumWorkListEntries++;
    }
    
    g_NumWorkListEntries--;
}
*/

void WorkListDeleteTask(WorkListEntry* pTask) {
    WorkListRemoveTask(pTask);
    HeapFree(pTask);
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/work_list", TimerWorkListAddTask);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/work_list", TimerWorkListAllocateTask);

void WorkListSetTaskCallback(WorkListEntry* pTask, WorkListCallback_t callback) {
    pTask->onTriggerCallback = callback;
}

void TimerWorkListSetTaskCallback(WorkListEntry* pTask, WorkListCallback_t callback) {
    pTask->onTriggerCallback = callback;
}

void WorkListTaskSetOnFreeCallback(WorkListEntry* pTask, WorkListCallback_t callback) {
    pTask->onFreeCallback = callback;
}

WorkListCallback_t WorkListTaskGetTaskCallback(WorkListEntry* pTask) {
    return pTask->onTriggerCallback;
}

WorkListCallback_t WorkListTaskGetOnFreeCallback(WorkListEntry* pTask) {
    return pTask->onFreeCallback;
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/work_list", TimerWorkListRemoveTask);
/*
void TimerWorkListRemoveTask(WorkListEntry* pTargetEntry) {
    WorkListEntry* pPrevEntry;
    WorkListEntry* pCurEntry;

    pPrevEntry = NULL;
    for (pCurEntry = g_TimerWorkList; pCurEntry != NULL; pCurEntry = pCurEntry->pNext) {
        if (pCurEntry == pTargetEntry) {
            if (pPrevEntry) {
                pPrevEntry->pNext = pCurEntry->pNext;
            } else {
                g_TimerWorkList = pCurEntry->pNext;
            }
            
            if (D_80059590 == pTargetEntry) {
                D_80059590 = pTargetEntry->pNext;
            }

            break;
        }

        pPrevEntry = pCurEntry;
    } 
    
    if (pTargetEntry->unk14_3) {
        D_80059464--;
    }
    
    g_NumTimerWorkListEntries--;
}
*/

void TimerWorkListDeleteTask(WorkListEntry* pTask) {
    TimerWorkListRemoveTask(pTask);
    HeapFree(pTask);
}

// Unlink target entry from lists if certain flags are met
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/work_list", func_8001CE74);
/*
void func_8001CE74(WorkListEntry* pTargetEntry) {
    WorkListCallback_t pFnOnDeleteCallback;
    WorkListEntry* pCurEntry;
    WorkListEntry* pPrevEntry;

    pPrevEntry = NULL;
    for (pCurEntry = g_WorkList; pCurEntry != NULL; pCurEntry = pCurEntry->pNext) {
        if (
            pCurEntry->unk0 == pTargetEntry && 
            (pCurEntry->unk14_2 & 1) == 0 && 
            pCurEntry->unk14 == pTargetEntry->unk10
        ) {
            if (pPrevEntry) {
                pPrevEntry->pNext = pCurEntry->pNext;
            } else {
                g_WorkList = pCurEntry->pNext;
            }
            if (D_80059590 == pCurEntry) {
                D_80059590 = pCurEntry->pNext;
            }
            
            pFnOnDeleteCallback = pCurEntry->onFreeCallback;
            if (pFnOnDeleteCallback) {
                pFnOnDeleteCallback(pCurEntry);
            }
        } else {
            pPrevEntry = pCurEntry;
        }
    }

    pPrevEntry = NULL;
    for (pCurEntry = g_TimerWorkList; pCurEntry != NULL; pCurEntry = pCurEntry->pNext) {
        if (
            pCurEntry->unk0 == pTargetEntry && 
            (pCurEntry->unk14_2 & 1) == 0 && 
            (pCurEntry->unk14 & 0x1FFFFFFF) == pTargetEntry->unk10
        ) {
            if (pPrevEntry) {
                pPrevEntry->pNext = pCurEntry->pNext;
            } else {
                g_TimerWorkList = pCurEntry->pNext;
            }
            if (D_80059590 == pCurEntry) {
                D_80059590 = pCurEntry->pNext;
            }
            
            pFnOnDeleteCallback = pCurEntry->onFreeCallback;
            if (pFnOnDeleteCallback) {
                pFnOnDeleteCallback(pCurEntry);
            }
        } else {
            pPrevEntry = pCurEntry;
        }
    }
}
*/

// Set pCurEntry->unk4->unk70 of target entry if unk14_1 flag is set
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/work_list", func_8001D034);
/*
void func_8001D034(WorkListEntry* pTargetEntry) {
    WorkListEntry* pCurEntry;

    for (pCurEntry = g_TimerWorkList; pCurEntry != NULL; pCurEntry = pCurEntry->pNext) {
        if (pCurEntry->unk0 == pTargetEntry) {
            if (
                pCurEntry->unk14 == pTargetEntry->unk10 &&
                (pCurEntry->unk14_1 & 1)
            ) {
                *(u32*)(pCurEntry->unk4 + 0x70) = 0;
            }
        }   
    }
}
*/

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/work_list", func_8001D0A4);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/work_list", func_8001D10C);

void* func_8001D164(void* pCallback) {
    void* pPrev = NULL;
    void* pCur = g_TimerWorkList;
    while (pCur != NULL) {
        void* pNext = *(void**)((u8*)pCur + 0x08);
        if (pNext == pCallback) return pCur;
        pPrev = pCur;
        pCur = *(void**)((u8*)pCur + 0x18);
    }
    return pPrev;
}

void WorkListsDeleteTasks(WorkListEntry* pTasks) {
    WorkListRemoveTask(pTasks + 1);
    TimerWorkListRemoveTask(pTasks);
    HeapFree(pTasks);
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/work_list", WorkListsAddTasks);



// This may be the start of a new TU
extern s32 D_80059190;
extern void func_8001DAE8(void* arg0, u16 arg1, u32 arg2);
extern void func_800234AC(void* pSpriteData);
extern void func_8001F8E8(void* pSpriteData, u16 frameIndex, u32 animPackageAddr);
extern u8 D_8005A474[];
extern u8 D_8006BE10[];

void func_8001D298(void) {
    D_80059190 = 0;
}

void func_8001D2A4(void) {
    D_80059190 = 0;
}

// Change Sprite Animation Frame maybe?
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

// Unlink SpriteData entry from list
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/work_list", func_8001D3F4);

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
