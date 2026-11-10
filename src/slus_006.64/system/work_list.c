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


/* Retail SBSS globals.  Their definitions are kept in the extracted BSS
 * segment; declaring the corresponding commons here lets the -G8 compiler
 * emit the retail gp-relative accesses. */
s32 D_80059184;
int g_NumTimerWorkListEntries;
int g_NumWorkListEntries;
WorkListEntry* D_800594C0;
WorkListEntry* g_TimerWorkList;
WorkListEntry* D_80059590;
WorkListEntry* g_WorkList;


void WorkListsFreeAllEntries(void) {
    WorkListEntry* pList;

    for (pList = g_TimerWorkList; g_TimerWorkList != NULL; pList = g_TimerWorkList) {
        pList->onFreeCallback(pList);
    }

    for (pList = g_WorkList; g_WorkList != NULL; pList = g_WorkList) {
        pList->onFreeCallback(pList);
    }
}

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
    WorkListEntry* pNext;

    if (g_WorkListCurTimer) {
        g_WorkListCurTimer--;
        if (g_WorkListCurTimer == 0) {
            D_80059494 = 0;
        }
    } else {
        D_80059590 = g_TimerWorkList;
        while (D_80059590 != NULL) {
            /* Retail reads the link and the callback before publishing the
             * cursor globals. */
            pEntry = (u8*)D_80059590;
            pNext = (WorkListEntry*)(uintptr_t)*(u32*)(pEntry + 0x18);
            pFnCallback = (WorkListCallback_t)(uintptr_t)*(u32*)(pEntry + 0x8);
            D_800594C0 = (WorkListEntry*)pEntry;
            D_80059590 = pNext;
            if (pFnCallback) {
                pFnCallback(pEntry);
            }
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
    WorkListEntry* pNext;

    D_80059590 = g_WorkList;
    while (D_80059590 != NULL) {
        pEntry = (u8*)D_80059590;
        pNext = (WorkListEntry*)(uintptr_t)*(u32*)(pEntry + 0x18);
        pFnCallback = (WorkListCallback_t)(uintptr_t)*(u32*)(pEntry + 0x8);
        D_800594C0 = (WorkListEntry*)pEntry;
        D_80059590 = pNext;
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

extern s32 D_80059184;
extern void WorkListRemoveTask(WorkListEntry* pEntry);

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): task_link_draw_node (src/decomp/src/resident/sprite.c:116). */
#ifndef XM_T_TaskLink
#define XM_T_TaskLink
typedef union {
    u32 word;
    struct {
        unsigned owner_serial : 29;  
        unsigned flag29 : 1;
        unsigned flag30 : 1;
        unsigned active : 1;
    } bits;
} TaskLink;
#endif
#ifndef XM_T_Task
#define XM_T_Task
typedef struct Task {
    struct Task *owner;
    void *data;                          
    void (*update)(struct Task *task);  
    void (*destroy)(struct Task *task); 
    union {
        u32 word;
        struct {
            unsigned serial : 29;        
            unsigned flags : 3;
        } bits;
    } id;                                
    TaskLink link;                       
    struct Task *next;                   
} Task;
#endif
#ifndef XM_S_task_link_draw_node
#define XM_S_task_link_draw_node
void xm_task_link_draw_node(Task *owner, Task *node) __asm__("WorkListAddTask");
#endif
#ifndef XM_S_task_draw_list
#define XM_S_task_draw_list
extern Task *xm_task_draw_list __asm__("g_WorkList");
#endif
#ifndef XM_S_task_next_serial
#define XM_S_task_next_serial
extern u32 xm_task_next_serial __asm__("D_80059184");
#endif
#ifndef XM_S_task_unlink_draw_node
#define XM_S_task_unlink_draw_node
void xm_task_unlink_draw_node(Task *task) __asm__("WorkListRemoveTask");
#endif
#ifndef XM_S_task_draw_count
#define XM_S_task_draw_count
extern s32 xm_task_draw_count __asm__("g_NumWorkListEntries");
#endif
void WorkListAddTask(Task *owner, Task *node) {
    TaskLink link;
    Task *head = xm_task_draw_list;

    node->owner = owner;
    node->next = head;
    xm_task_draw_list = node;
    link.word = node->link.word;
    link.bits.owner_serial = owner->id.bits.serial;
    node->id.bits.serial = xm_task_next_serial++;
    link.bits.flag29 = 0;
    link.bits.flag30 = 0;
    link.bits.active = 0;
    node->link = link;
    node->update = NULL;
    node->destroy = xm_task_unlink_draw_node;
    xm_task_draw_count++;
}
#endif
#else
void WorkListAddTask(void* data, WorkListEntry* pEntry) {
    u32 old14 = *(u32*)((u8*)pEntry + 0x14);
    u32 old10 = *(u32*)((u8*)pEntry + 0x10);
    s32 timer = D_80059184;
    u32 taskFlags = *(u32*)((u8*)data + 0x10);

    pEntry->unk0 = (WorkListEntry*)data;
    pEntry->pNext = g_WorkList;
    g_WorkList = pEntry;

    /* Set entry+0x10: merge timer counter into low 29 bits */
    *(u32*)((u8*)pEntry + 0x10) = (old10 & 0xE0000000) | (timer & 0x1FFFFFFF);

    /* Set entry+0x14: merge data task flags, clear bits 29-31 */
    old14 = (old14 & 0xE0000000) | (taskFlags & 0x1FFFFFFF);
    old14 &= 0xDFFFFFFF;
    old14 &= 0xBFFFFFFF;
    old14 &= 0x7FFFFFFF;
    *(u32*)((u8*)pEntry + 0x14) = old14;

    pEntry->onTriggerCallback = NULL;
    pEntry->onFreeCallback = WorkListRemoveTask;
    D_80059184 = timer + 1;
    g_NumWorkListEntries++;
}
#endif

extern u8 D_800591AF;
void WorkListDeleteTask(WorkListEntry* pTask);

WorkListEntry* WorkListAllocateTask(void* data, int dataSize) {
    WorkListEntry* pEntry;

    pEntry = HeapAlloc(dataSize + sizeof(WorkListEntry), D_800591AF);
    WorkListAddTask(data, pEntry);
    pEntry->onFreeCallback = WorkListDeleteTask;
    return pEntry;
}

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

void WorkListDeleteTask(WorkListEntry* pTask) {
    WorkListRemoveTask(pTask);
    HeapFree(pTask);
}

extern void TimerWorkListRemoveTask(WorkListEntry* pEntry);
extern u8 D_800591AC;
extern s32 D_80059464;

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): task_link_main_node (src/decomp/src/resident/sprite.c:180). */
#ifndef XM_S_task_link_main_node
#define XM_S_task_link_main_node
void xm_task_link_main_node(Task *owner, Task *node) __asm__("TimerWorkListAddTask");
#endif
#ifndef XM_S_task_unlink_main_node
#define XM_S_task_unlink_main_node
void xm_task_unlink_main_node(Task *task) __asm__("TimerWorkListRemoveTask");
#endif
#ifndef XM_S_task_main_list
#define XM_S_task_main_list
extern Task *xm_task_main_list __asm__("g_TimerWorkList");
#endif
#ifndef XM_S_task_new_tasks_active
#define XM_S_task_new_tasks_active
extern u8 xm_task_new_tasks_active __asm__("D_800591AC");
#endif
#ifndef XM_S_task_active_main_count
#define XM_S_task_active_main_count
extern s32 xm_task_active_main_count __asm__("D_80059464");
#endif
#ifndef XM_S_task_main_count
#define XM_S_task_main_count
extern s32 xm_task_main_count __asm__("g_NumTimerWorkListEntries");
#endif
void TimerWorkListAddTask(Task *owner, Task *node) {
    Task *self = &node[0];
    u32 serial;

    self->owner = owner;
    serial = owner->id.bits.serial;
    self->destroy = xm_task_unlink_main_node;
    self->update = NULL;
    self->next = xm_task_main_list;
    xm_task_main_list = self;
    self->link.bits.owner_serial = serial;
    self->link.bits.flag29 = 0;
    self->link.bits.flag30 = 0;
    self->link.bits.active = 0;
    self->id.bits.serial = xm_task_next_serial++;
    if (xm_task_new_tasks_active != 0) {
        xm_task_active_main_count++;
        self->link.bits.active = 1;
    } else {
        self->link.bits.active = 0;
    }
    xm_task_main_count++;
}
#endif
#else
void TimerWorkListAddTask(void* data, WorkListEntry* pEntry) {
    u32 old14, old10, taskFlags;
    s32 timer;
    u32* pTimerList;
    u8 isMainList;

    pEntry->unk0 = (WorkListEntry*)data;
    taskFlags = *(u32*)((u8*)data + 0x10);
    pTimerList = (u32*)g_TimerWorkList;
    pEntry->onFreeCallback = TimerWorkListRemoveTask;
    pEntry->onTriggerCallback = NULL;
    g_TimerWorkList = pEntry;

    old14 = *(u32*)((u8*)pEntry + 0x14);
    old10 = *(u32*)((u8*)pEntry + 0x10);
    timer = D_80059184;

    *(u32*)((u8*)pEntry + 0x14) = (old14 & 0xE0000000) | (taskFlags & 0x1FFFFFFF);
    old10 = (old10 & 0xE0000000) | (timer & 0x1FFFFFFF);
    *(u32*)((u8*)pEntry + 0x10) = old10;
    *(u32*)((u8*)pEntry + 0x18) = (u32)pTimerList;
    D_80059184 = timer + 1;

    isMainList = D_800591AC;
    if (isMainList) {
        D_80059464++;
        *(u32*)((u8*)pEntry + 0x14) |= 0x80000000;
    } else {
        *(u32*)((u8*)pEntry + 0x14) &= 0x7FFFFFFF;
    }
    g_NumTimerWorkListEntries++;
}
#endif

extern u8 D_800591AF;
void TimerWorkListDeleteTask(WorkListEntry* pTask);

WorkListEntry* TimerWorkListAllocateTask(void* data, int dataSize) {
    WorkListEntry* pEntry;
    pEntry = HeapAlloc(dataSize + sizeof(WorkListEntry), D_800591AF);
    TimerWorkListAddTask(data, pEntry);
    pEntry->onFreeCallback = &TimerWorkListDeleteTask;
    pEntry->unk4 = 0;
    return pEntry;
}

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

void TimerWorkListDeleteTask(WorkListEntry* pTask) {
    TimerWorkListRemoveTask(pTask);
    HeapFree(pTask);
}

// Unlink target entry from lists if certain flags are met
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

// Set pCurEntry->unk4->unk70 of target entry if unk14_1 flag is set
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

void* func_8001D0A4(void* pTask, void* pCallback) {
    void* pCur = g_TimerWorkList;
    while (pCur != NULL) {
        if (*(void**)pCur == pTask) {
            u32 curFlags = *(u32*)((u8*)pCur + 0x14) & 0x1FFFFFFF;
            u32 taskFlags = *(u32*)((u8*)pTask + 0x10) & 0x1FFFFFFF;
            if (curFlags == taskFlags) {
                if (*(void**)((u8*)pCur + 0x08) == pCallback) return pCur;
            }
        }
        pCur = *(void**)((u8*)pCur + 0x18);
    }
    return NULL;
}

void* func_8001D10C(void* pTask) {
    void* pCur = g_TimerWorkList;
    while (pCur != NULL) {
        if (*(void**)pCur == pTask) {
            u32 curFlags = *(u32*)((u8*)pCur + 0x14) & 0x1FFFFFFF;
            u32 taskFlags = *(u32*)((u8*)pTask + 0x10) & 0x1FFFFFFF;
            if (curFlags == taskFlags) return pCur;
        }
        pCur = *(void**)((u8*)pCur + 0x18);
    }
    return NULL;
}

/* Returns the first timer-list entry whose trigger callback is pCallback,
 * or NULL when there is none (retail returns NULL, not the last entry). */
void* func_8001D164(void* pCallback) {
    void* pCur;
    for (pCur = g_TimerWorkList; pCur != NULL; pCur = *(void**)((u8*)pCur + 0x18)) {
        if (*(void**)((u8*)pCur + 0x08) == pCallback) {
            return pCur;
        }
    }
    return NULL;
}

void WorkListsDeleteTasks(WorkListEntry* pTasks) {
    WorkListRemoveTask(pTasks + 1);
    TimerWorkListRemoveTask(pTasks);
    HeapFree(pTasks);
}

#ifndef XENO_PC_PORT
WorkListEntry* WorkListsAddTasks__ret(u32 size, void* owner, void* timerCb, void* workCb, void* freeCb) asm("WorkListsAddTasks");
WorkListEntry* WorkListsAddTasks__ret(u32 size, void* owner, void* timerCb, void* workCb, void* freeCb) {
    WorkListEntry* entry;
    WorkListEntry* work;
    entry = HeapAlloc(size, D_800591AF);
    TimerWorkListAddTask(owner, entry);
    work = (WorkListEntry*)((u8*)entry + 0x1C);
    WorkListAddTask(entry, work);
    TimerWorkListSetTaskCallback(entry, timerCb);
    WorkListSetTaskCallback(work, workCb);
    if (freeCb != NULL) WorkListTaskSetOnFreeCallback(entry, freeCb);
    else WorkListTaskSetOnFreeCallback(entry, &WorkListsDeleteTasks);
    entry->unk4 = entry;
    entry[1].unk4 = entry;
    return entry;
}
#else
WorkListEntry* WorkListsAddTasks(void* timerData, void* timerCb, void* workCb, void* onFreeCb) {
    WorkListEntry* pEntry;
    WorkListEntry* pWorkEntry;
    pEntry = HeapAlloc(sizeof(WorkListEntry) * 2, D_800591AF);
    TimerWorkListAddTask(timerData, pEntry);
    pWorkEntry = (WorkListEntry*)((u8*)pEntry + sizeof(WorkListEntry));
    WorkListAddTask(pEntry, pWorkEntry);
    TimerWorkListSetTaskCallback(pEntry, timerCb);
    WorkListSetTaskCallback(pWorkEntry, workCb);
    if (onFreeCb != NULL) {
        WorkListTaskSetOnFreeCallback(pEntry, onFreeCb);
    } else {
        WorkListTaskSetOnFreeCallback(pEntry, &WorkListsDeleteTasks);
    }
    pEntry->unk4 = pEntry;
    pWorkEntry->unk4 = pEntry;
    return pEntry;
}
#endif



// This may be the start of a new TU
#ifdef XENO_PC_PORT
extern s32 D_80059190;
#else
/* This TU's own .sbss word: retail reaches it %gp_rel, which cc1 only emits
 * for a same-TU definition (plain externs assemble absolute here). */
s32 D_80059190;
#endif
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

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): sprite_request_frame (src/decomp/src/resident/sprite.c:401). */
#ifndef XM_T_MATRIX
#define XM_T_MATRIX
typedef struct {
    short m[3][3];
    long t[3];
} MATRIX;
#endif
#ifndef XM_T_SpritePart
#define XM_T_SpritePart
typedef struct {
    s16 x, y;              
    u8 u, v;               
    u8 w, h;               
    u8 byte8, byte9;       
    u16 tpage;             
    u16 clut;              
    u8 unknowne[2];
    u32 colour;            
    u32 flags;             
} SpritePart;
#endif
#ifndef XM_T_SpriteRendererEntry
#define XM_T_SpriteRendererEntry
typedef struct {
    s8 byte0;              
    s8 byte1;              
    s16 half2;
    s16 half4;
    s16 half6;
} SpriteRendererEntry;
#endif
#ifndef XM_T_SpriteOffset
#define XM_T_SpriteOffset
typedef struct {
    s8 x, y;
} SpriteOffset;
#endif
#ifndef XM_T_SVECTOR
#define XM_T_SVECTOR
typedef struct {
    short vx, vy, vz, pad;
} SVECTOR;
#endif
#ifndef XM_T_SpriteRenderer
#define XM_T_SpriteRenderer
typedef struct {
    s16 angle_x, angle_y, angle_z; 
    s16 scale_x, scale_y, scale_z; 
    MATRIX matrix;                 
    SpritePart *parts[2];          
    SpriteRendererEntry *pointer34; 
    struct Sprite *next_pending;   
    SpriteOffset offset;           
    u8 unknown3e[2];
    s32 word40;                    
    SVECTOR light_angles;          
    u16 light_colour[3];           
} SpriteRenderer;
#endif
#ifndef XM_T_Sprite
#define XM_T_Sprite
typedef struct Sprite {
    s32 x, y, z;                
    s32 speed_x, speed_y, speed_z; 
    s32 speed;                  
    s32 gravity;                
    SpriteRenderer *renderer;   
    void *image;                
    u8 red, green, blue;     
    u8 colour_flags;         
    s16 scale;               
    s16 depth;               
    s16 half30;              
    s16 direction;           
    u16 frame;               
    u16 height;              
    u16 extent_depth;        
    u16 rate;                
    union {
        u32 word;
        struct {
        unsigned sides : 2;      
        unsigned unknown2 : 1;
        unsigned flip : 1;       
        unsigned flip_y : 1;
        unsigned blend : 3;      
        unsigned unknown8 : 8;
        unsigned field16 : 4;
        unsigned mode : 4;       
        unsigned no_view : 1;    
        unsigned unknown25 : 3;
        unsigned dirty : 1;      
        unsigned unknown29 : 3;
        } bits;
        u8 bytes[4];         
    } render;                
    u32 flags;               
    s32 *resource_block;     
    s32 *animations;         
    s32 resource;            
    s32 word50;              
    u16 *frame_table;        
    u16 *animation;          
    u16 *facings;            
    u16 *word60;             
    u8 *script;              
    void (*callback)(struct Sprite *sprite); 
    void *block;             
    struct Sprite *parent;   
    struct Sprite *partner;  
    s32 word78;              
    void *sequencer;         
    u16 word80;              
    u16 word82;              
    s16 ground;              
    u16 size;                
    u8 *frames;              
    s8 stack_top;            
    u8 unknown8d;
    u8 stack[0x10];          
    s16 countdown;           
    s16 target_x, target_y, target_z; 
    u8 unknowna6[2];
    struct {
        unsigned sequencer_owned : 1; 
        unsigned bounce : 10;    
        unsigned frame : 6;      
        unsigned step : 3;       
        unsigned phase : 2;      
        unsigned field22 : 6;    
        unsigned field28 : 2;
        unsigned unknown30 : 2;
    } frame_bits;            
    union {
        u32 word;
        struct {
        unsigned unknown0 : 2;
        unsigned mirror : 1;     
        unsigned frame_flip : 1; 
        unsigned unknown4 : 1;
        unsigned owns_children : 1; 
        unsigned double_step : 1;
        unsigned divisor : 12;   
        unsigned unknown19 : 13;
        } bits;
        u8 bytes[4];
    } motion;                
    union {
        u32 wordb0;          
        u8 byteb0;
        struct {
            unsigned unknown0 : 8;
            unsigned passive_children : 1; 
            unsigned share_rate : 1;       
            unsigned unknown10 : 22;
        } bits;
    } b0;                    
} Sprite;
#endif
#ifndef XM_T_DVECTOR
#define XM_T_DVECTOR
typedef struct {
    short vx, vy;
} DVECTOR;
#endif
#ifndef XM_T_SpriteSource
#define XM_T_SpriteSource
typedef struct {
    u16 *frames;           
    DVECTOR origin;        
    s16 clut_x;            
    s16 clut_y;            
    u16 *palette;          
    u16 *animations;       
} SpriteSource;
#endif
#ifndef XM_S_sprite_request_frame
#define XM_S_sprite_request_frame
void xm_sprite_request_frame(Sprite *sprite, s32 frame) __asm__("func_8001D2B0");
#endif
#ifndef XM_S_sprite_clear_group_entries
#define XM_S_sprite_clear_group_entries
void xm_sprite_clear_group_entries(Sprite *sprite) __asm__("func_800234AC");
#endif
#ifndef XM_S_sprite_pending_list
#define XM_S_sprite_pending_list
extern Sprite *xm_sprite_pending_list __asm__("D_80059190");
#endif
#ifndef XM_S_sprite_effect_source
#define XM_S_sprite_effect_source
extern u8 xm_sprite_effect_source[] __asm__("D_8005A474");
#endif
#ifndef XM_S_sprite_shared_source
#define XM_S_sprite_shared_source
extern u8 xm_sprite_shared_source[] __asm__("D_8006BE10");
#endif
#ifndef XM_S_sprite_apply_frame_controls
#define XM_S_sprite_apply_frame_controls
void xm_sprite_apply_frame_controls(Sprite *sprite, s32 frame, SpriteSource *source) __asm__("func_8001F8E8");
#endif
void func_8001D2B0(Sprite *sprite, s32 frame) {
    Sprite *pending;

    if ((sprite->render.word & 3) != 1) {
        sprite->frame = 0;
        return;
    }
    if ((sprite->flags >> 20) & 1) {
        sprite->flags &= ~0x100000;
        if (sprite->renderer->pointer34 != NULL) {
            xm_sprite_clear_group_entries(sprite);
        }
    }
    if ((sprite->flags >> 17) & 1) {
        for (pending = xm_sprite_pending_list; pending != NULL; pending = pending->renderer->next_pending) {
            if (pending == sprite) {
                if (sprite->image != xm_sprite_effect_source && sprite->image != xm_sprite_shared_source && !((sprite->flags >> 19) & 1)) {
                    xm_sprite_apply_frame_controls(sprite, sprite->frame, sprite->image);
                }
                sprite->frame = frame;
                return;
            }
        }
    }
    sprite->frame = frame;
    sprite->flags |= 0x20000;
    sprite->renderer->next_pending = xm_sprite_pending_list;
    xm_sprite_pending_list = sprite;
}
#endif
#else
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
#endif

// Unlink SpriteData entry from list

void func_8001D3F4(u8* pTarget) {
#ifdef XENO_PC_PORT
    u8* pPrev = NULL;
    u8* pCur = (u8*)(uintptr_t)D_80059190;
    while (pCur != NULL) {
        u32 pNext;
        if (pCur == pTarget) {
            u32 pTargetSub = *(u32*)(pCur + 0x20);
            if (pPrev != NULL) {
                u32 pPrevSub = *(u32*)(pPrev + 0x20);
                *(u32*)(pPrevSub + 0x38) = *(u32*)(pTargetSub + 0x38);
            } else {
                D_80059190 = *(u32*)(pTargetSub + 0x38);
            }
        } else {
            pPrev = pCur;
        }
        pNext = *(u32*)(*(u32*)(pCur + 0x20) + 0x38);
        pCur = (u8*)(uintptr_t)pNext;
    }
#else
    void *var_a1;
    void *var_a2;

    var_a1 = D_80059190;
    var_a2 = NULL;
    if (var_a1 != NULL) {
        do {
            if (var_a1 == pTarget) {
                if (var_a2 != NULL) {
                    *(void **)((s8*)(*(void **)((s8*)(var_a2) + 0x20)) + 0x38) = (void *) *(void **)((s8*)(*(void **)((s8*)(var_a1) + 0x20)) + 0x38);
                } else {
                    D_80059190 = *(void **)((s8*)(*(void **)((s8*)(var_a1) + 0x20)) + 0x38);
                }
            } else {
                var_a2 = var_a1;
            }
            var_a1 = *(void **)((s8*)(*(void **)((s8*)(var_a1) + 0x20)) + 0x38);
        } while (var_a1 != NULL);
    }
#endif
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
