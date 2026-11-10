#include "common.h"
#include "main/game.h"
#include "field/main.h"
#include "field/actor.h"
#include "field/script_vm.h"
#include "field/text_box.h"
#include "system/memory.h"
#include "system/archive.h"
#ifdef XENO_PC_PORT
#include <stdio.h>
#include <stdlib.h>
#endif

extern s32 D_8005A444;
extern s32 D_8005A448;
extern s32 D_8005A44C;
extern s32 D_800AFD04;
extern s32 D_800AFD1C;
extern s32 D_800ADB2C;
extern s32 D_800ADB64;
extern s32 D_800ADB70;
extern s32 D_800C4268;
extern void* D_800ADBF0;
extern u16 D_800B2174[];

u32 FieldScriptVMGetActorIndex(int bytecodeOffset);
s32 func_8008A558(void);
s32 func_80080720(void);
s32 func_80080760(void);
s32 func_800807B4(void);
s32 func_8009C154(s32 targetId);
s32 func_8009C5A8(s32 actorIndex, s32 mode);
void func_8007F814(s32 actorIndex, s32* screenX, s32* screenY, s32 yOffset);
void func_8007F8DC(s32 x, s32 y, s32 stringIndex, s32 textBoxIndex, s32 width, s32 height,
                  s32 ownerActorIndex, s32 talkingActorIndex, s32 mode, s32 orientationFlags,
                  s32 dialogFlags);
u8 DialogGetWidth(u16* pDialogData, int dialogIndex);
u8 DialogGetHeight(u16* pDialogData, int dialogIndex);
s32 func_80033CD0(void* arg0);
void func_80034800(void* arg0, s32 r, s32 g, s32 b);

void func_8009BB0C(void) {
    int nTextBoxIndex;
    u32 lowerFlags;
    u32 upperFlags;

    // Check if there's no visible textbox owned by current actor
    if (func_8009CD18(&nTextBoxIndex) == -1) {
        g_FieldScriptMaxInstructionCount += 8;
        FieldScriptMemoryWriteU16(0x14, g_FieldScriptVMCurActor->unk81);
        g_FieldScriptVMCurActor->scriptInstructionPointer += 1;
        return;
    }
    
    if (((ActorData*)(uintptr_t)g_FieldActors[g_FieldTextBoxes[nTextBoxIndex].talkingActorID].pActorData)->flags & 0x200) {
        upperFlags = g_FieldScriptVMCurActor->dialogFlags >> 0x10;
        if (g_FieldScriptVMCurActor->dialogFlags >> 0x10 == 0) {
            lowerFlags = g_FieldScriptVMCurActor->dialogFlags & 0xFFFF;
        } else {
            lowerFlags = upperFlags & 0xFFFF;
        }
        
        if (!(lowerFlags & 0x1)) {
            if (g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].flags_0x12 != 7) {
                func_800A1B70();
            }
            g_FieldTextBoxes[nTextBoxIndex].status = 0;
        }
    }
    D_800B00C0 = 1;
}

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): field_event_start_choice (src/decomp/src/field/field_event.c:8236). */
#ifndef XM_T_xm_WindowLine
#define XM_T_xm_WindowLine
typedef struct {
    SPRT sprite[2][2];
    RECT rect;       
    s16 width;       
    u8 plane;        
    u8 slot;
    u8 row;          
    u8 unk5D;
    u16 clut;
} xm_WindowLine;
#endif
#ifndef XM_T_xm_WindowQueue
#define XM_T_xm_WindowQueue
typedef struct xm_WindowQueue {
    struct xm_WindowQueue *next;
    s32 message;
} xm_WindowQueue;
#endif
#ifndef XM_T_xm_Window
#define XM_T_xm_Window
typedef struct xm_Window {
    s16 x;          
    s16 y;          
    s16 unk4;
    s16 unk6;
    s16 unk8;
    s16 width;
    s16 lines;
    s16 unkE;
    u16 flags;
    s16 stride;
    s16 unk14;
    s16 unk16;
    s16 unk18;      
    u8 unk1A[2];
    u8 *text;
    u8 *resume;     
    u8 unk24[4];
    xm_WindowLine *layout;
    void *image;
    u8 unk30[0xC];  
    u8 unk3C[0xC];  
    TILE tile[2];   
    u8 unk68;
    u8 unk69;
    u8 unk6A;
    u8 unk6B;
    u8 unk6C;
    u8 unk6D;
    u8 unk6E;
    u8 unk6F;
    s32 values[4];  
    s16 selection; 
    s16 queued;
    s16 unk84;      
    s16 unk86;      
    s16 unk88;      
    u8 unk8A[2];
    xm_WindowQueue *queue;
} xm_Window;
#endif
#ifndef XM_T_xm_DialogueFrame
#define XM_T_xm_DialogueFrame
typedef struct {
    RECT rect;                  
    u8 unk08[0x18 - 0x08];
    DR_MODE back_modes[2];      
    TILE back[2];               
    DR_MODE border_modes[2][10]; 
    SPRT border[2][10];         
} xm_DialogueFrame;
#endif
#ifndef XM_T_xm_DialogueChoice
#define XM_T_xm_DialogueChoice
typedef struct {
    s16 status;       
    s16 first;        
    s16 count;        
    s16 index;        
    DR_MODE modes[2]; 
    SPRT cursor[2];   
} xm_DialogueChoice;
#endif
#ifndef XM_T_xm_DialoguePrompt
#define XM_T_xm_DialoguePrompt
typedef struct {
    s16 status;       
    u8 unk02[2];
    DR_MODE modes[2]; 
    SPRT sprite[2];   
} xm_DialoguePrompt;
#endif
#ifndef XM_T_xm_Fixed
#define XM_T_xm_Fixed
typedef union {
    s32 value;
    struct {
        u16 fraction;
        s16 whole;
    } part;
} xm_Fixed;
#endif
#ifndef XM_T_xm_DialogueWindow
#define XM_T_xm_DialogueWindow
typedef struct xm_DialogueWindow {
    DR_MODE modes[2]; 
    xm_Window text;     
    s32 message;     
    xm_DialogueFrame frame; 
    xm_DialogueChoice choice; 
    xm_DialoguePrompt prompt; 
    s16 timer;       
    s16 prompt_delay; 
    u16 style;       

    s16 busy;        
    u16 age;         
    s16 unk412;      
    s16 cleared;     
    s16 owner;       
    s16 unk418;      
    u8 unk41A[2];
    xm_Fixed slide[2];  
    s32 slide_step[2]; 
    DR_MODE icon_modes[2]; 
    POLY_FT4 icon[2];  
    u8 unk494;       
    u8 unk495;       
    u8 unk496[0x498 - 0x496];
} xm_DialogueWindow;
#endif
#ifndef XM_T_xm_ScriptSlot
#define XM_T_xm_ScriptSlot
typedef struct xm_ScriptSlot {
    u16 resume_pc;      
    u8 countdown;       
    u8 tag;             
    u32 value : 16;     
    u32 unk16 : 2;
    u32 priority : 4;   
    u32 unk22 : 1;
    u32 move_mode : 2;  
    u32 unk25 : 7;
} xm_ScriptSlot;
#endif
#ifndef XM_T_xm_PlatformLink
#define XM_T_xm_PlatformLink
typedef struct xm_PlatformLink {
    SVECTOR rotation; 
    s16 radius;       
    s16 unkA;
} xm_PlatformLink;
#endif
#ifndef XM_T_xm_ActorState
#define XM_T_xm_ActorState
typedef struct {
    u32 mode : 2;   
    u32 unk2 : 3;   
    u32 unk5 : 1;   
    u32 depth : 3;  
    u32 octant : 3; 
    u32 unk12 : 1;  
    u32 layer : 3;  
    u32 unk16 : 2;  
    u32 unk18 : 10; 
    u32 unk28 : 4;
} xm_ActorState;
#endif
#ifndef XM_T_xm_FieldActor
#define XM_T_xm_FieldActor
typedef struct xm_FieldActor {
    u32 flags;           
    u32 layer_flags;     
    s16 triangle[4];     
    s16 layer;           
    u8 unk012[2];
    u32 floor_attribute; 
    s16 unk18;           
    s16 height;          
    xm_Fixed gravity;       
    s32 position[3];     
    u8 unk02C[4];
    s32 unk030[3];       
    u8 unk03C[4];
    s32 unk40[3];        
    u8 unk04C[4];
    s32 unk50[3];        
    u8 unk05C[4];
    s16 unk60;           
    s16 unk62;           
    s16 unk64;           
    u8 unk066[2];
    s16 last_position[3]; 
    u16 stuck;           
    s16 unk70;           
    s16 unk72;           
    u8 ridden_actor;     
    u8 unk075;           
    s16 unk76;           
    u16 call_stack[4];   
    u8 character;        
    u8 unk081;
    u8 unk82;            
    u8 unk83;            
    s32 unk84;           
    s16 unk88;           
    s16 unk8A;           
    xm_ScriptSlot slots[8]; 
    u16 pc;              
    u8 slot;             
    u8 unk0CF;
    s32 target[3];       
    u8 unk0DC[0xE2 - 0xDC];
    u8 unkE2;            
    u8 unk0E3;
    s16 unkE4;           
    s16 unkE6;           
    s16 unkE8;           
    s16 unk0EA;          
    s16 unkEC;           
    s16 unkEE;           
    s32 unkF0;           
    s16 scale[3];        
    u8 unk0FA[2];
    u8 color0[3];        
    u8 color1[3];        
    u16 unk102;          
    s16 heading;         
    s16 heading_goal;    
    s16 unk108;          
    u16 sound;           
    u8 sound_volume;     
    u8 sound_mode;       
    u8 unk10E[0x110 - 0x10E];
    struct xm_PlatformLink *link; 
    void *unk114;        
    s32 *list;           
    u16 unk11C;          
    s16 unk11E;          
    void *unk120;        
    s16 unk124;          
    u8 unk126;           
    u8 unk127;           
    u16 unk128;          
    u8 unk12A[2];
    union {
        u32 word;
        xm_ActorState bits;
    } state;             
    u32 unk130 : 9;      
    u32 unk130_9 : 10;   
    u32 unk130_19 : 9;   
    u32 sprite_kind : 2; 
    u32 unk130_30 : 2;
    u32 unk134;          
} xm_FieldActor;
#endif
#ifndef XM_T_EVENT_OPERAND_BYTE
#define XM_T_EVENT_OPERAND_BYTE
#define EVENT_OPERAND_BYTE(offset) (xm_field_event_bytecode[xm_field_current_event_actor->pc + (offset)])
#endif
#ifndef XM_S_field_event_start_choice
#define XM_S_field_event_start_choice
void xm_field_event_start_choice(void) __asm__("func_8009BC98");
#endif
#ifndef XM_S_field_event_find_own_window
#define XM_S_field_event_find_own_window
s32 xm_field_event_find_own_window(s32 *window) __asm__("func_8009CD18");
#endif
#ifndef XM_S_field_event_batch_limit
#define XM_S_field_event_batch_limit
extern s32 xm_field_event_batch_limit __asm__("g_FieldScriptMaxInstructionCount");
#endif
#ifndef XM_S_window_get_wait_state
#define XM_S_window_get_wait_state
s32 xm_window_get_wait_state(xm_Window *window) __asm__("func_80033CD0");
#endif
#ifndef XM_S_field_dialogue_windows
#define XM_S_field_dialogue_windows
extern xm_DialogueWindow xm_field_dialogue_windows[4] __asm__("g_FieldTextBoxes");
#endif
#ifndef XM_S_field_current_event_actor
#define XM_S_field_current_event_actor
extern xm_FieldActor *xm_field_current_event_actor __asm__("g_FieldScriptVMCurActor");
#endif
#ifndef XM_S_field_event_bytecode
#define XM_S_field_event_bytecode
extern u8 *xm_field_event_bytecode __asm__("g_FieldScriptVMCurScriptData");
#endif
#ifndef XM_S_window_set_color
#define XM_S_window_set_color
void xm_window_set_color(xm_Window *window, s32 r, s32 g, s32 b) __asm__("func_80034800");
#endif
#ifndef XM_S_field_event_yield_requested
#define XM_S_field_event_yield_requested
extern s32 xm_field_event_yield_requested __asm__("D_800B00C0");
#endif
void func_8009BC98(void) {
    s32 window;
    u32 first;

    if (xm_field_event_find_own_window(&window) == 0) {
        xm_field_event_batch_limit += 8;
        if (xm_window_get_wait_state(&xm_field_dialogue_windows[window].text) == 1 ||
            ((&xm_field_dialogue_windows[window].text)->unk84 != 0 && (&xm_field_dialogue_windows[window].text)->unk6C != 0)) {
            xm_field_dialogue_windows[window].choice.status = 0;
            xm_field_current_event_actor->unk081 = 0xFF;
            first = EVENT_OPERAND_BYTE(1) >> 4;
            xm_field_dialogue_windows[window].choice.first = first;
            xm_field_dialogue_windows[window].choice.count = (EVENT_OPERAND_BYTE(1) & 0xF) - first + 1;
            xm_field_dialogue_windows[window].choice.index = 0;
            xm_window_set_color(&xm_field_dialogue_windows[window].text, 0xEF, 0x1E, 0xF0);
            xm_field_current_event_actor->pc += 2;
        }
    } else {
        xm_field_current_event_actor->pc += 2;
    }
    xm_field_event_yield_requested = 1;
}
#endif
#else
void func_8009BC98(void) {
    s32 textBoxIndex;
    u8* pTextBox;
    s32 offset;
    s32 isReady;

    if (func_8009CD18(&textBoxIndex) != 0) {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
        D_800B00C0 = 1;
        return;
    }

    g_FieldScriptMaxInstructionCount += 8;
    pTextBox = (u8*)&g_FieldTextBoxes[textBoxIndex];
    isReady = func_80033CD0(pTextBox + 0x18);
    if (isReady != 1) {
        /* Retail 8009BD30/40: lh 0x84 / lbu 0x6C off (box + 0x18), i.e.
         * box+0x9C and box+0x84 (the port read 0x18 bytes too low). */
        if (*(s16*)(pTextBox + 0x9C) == 0 || *(u8*)(pTextBox + 0x84) == 0) {
            D_800B00C0 = 1;
            return;
        }
    }

    *(s16*)(pTextBox + 0x37C) = 0;
    g_FieldScriptVMCurActor->unk81 = 0xFF;

    offset = textBoxIndex * 0x498;
    *(s16*)((u8*)g_FieldTextBoxes + offset + 0x37E) =
        ((u8*)g_FieldScriptVMCurScriptData)[g_FieldScriptVMCurActor->scriptInstructionPointer + 1] >> 4;
    *(s16*)((u8*)g_FieldTextBoxes + offset + 0x382) = 0;
    *(s16*)((u8*)g_FieldTextBoxes + offset + 0x380) =
        (((u8*)g_FieldScriptVMCurScriptData)[g_FieldScriptVMCurActor->scriptInstructionPointer + 1] & 0x0F) -
        *(s16*)((u8*)g_FieldTextBoxes + offset + 0x37E) + 1;
    func_80034800(pTextBox + 0x18, 0xEF, 0x1E, 0xF0);

    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
    D_800B00C0 = 1;
}
#endif /* XENO_PC_PORT */

s32 func_8009BE58(void) {
    return ((g_FieldScriptVMCurActor->direction - (FieldGetCameraDirection() & 0xffff)) & MASK_8DIR_MOVEMENT_NUM_DIRECTIONS) < 5;
}

// Reset text box?
void func_8009BE9C(void) {
    int index;
    
    if (SCRIPT_READ_U8_REL(1) == 0) {
        if (func_8009CD18(&index) == 0) {
            g_FieldTextBoxes[index].status = 0;
            g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
        } else {
            g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
        }
    } else {
        g_FieldScriptVMCurActor->dialogPixelWidth = 0;
        g_FieldScriptVMCurActor->dialogPixelHeight = 0;
        g_FieldScriptVMCurActor->dialogWidth = 0;
        g_FieldScriptVMCurActor->dialogHeight = 0;
        g_FieldScriptVMCurActor->dialogFlags = 0;
        g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
    }
    D_800B00C0 = 1;
}

// modifies the face id.. is this changing out the portrait?
void func_8009BF8C(void) {
    if (FieldScriptVMGetActorIndex(1) != ACTOR_ID_INVALID) {
        g_FieldScriptVMCurActor->faceId = ((ActorData*)(uintptr_t)g_FieldActors[FieldScriptVMGetActorIndex(1)].pActorData)->faceId;
        func_8009C01C();
        return;
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 6;
}

void func_8009C01C(void) {
    s32 actorIndex;

    if (FieldScriptVMGetActorIndex(1) != ACTOR_ID_INVALID) {
        /* Keep the retail branch-delay slot empty before resolving the target. */
        asm volatile("" : : : "memory");
        actorIndex = FieldScriptVMGetActorIndex(1);
        g_FieldScriptVMCurActor->scriptInstructionPointer++;
        if (func_8009C5A8(actorIndex, 0) == -1) {
            /* The retry branch likewise has an empty retail delay slot. */
            asm volatile("" : : : "memory");
            g_FieldScriptVMCurActor->scriptInstructionPointer--;
        }
        return;
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 6;
}

void func_8009C0B4(void) {
    func_8009C5A8(D_800AFD1C, 0);
}

void func_8009C0DC(void) {
    func_8009C5A8(D_800AFD1C, 1);
}

void func_8009C104(void) {
    func_8009C5A8(D_800AFD1C, 2);
}

void func_8009C12C(void) {
    func_8009C5A8(D_800AFD1C, 3);
}

/* Portrait TIM load slots: 3 entries of {faceId, state, dualTim}, stride 6. */
extern s16 D_800B06A4[];
extern s16 D_800B06A6[];
extern s16 D_800B06A8[];
extern s32 D_800ADB0C;
extern void* D_800ADB10;
extern void* D_800ADB14;
extern s16 D_800AEAE4[];
extern u8 D_800AE1E0[];
extern StreamDataQueueEntry D_800B00C8[];

void FieldLoadTIMWithClut(u_long* pTimData, short x, short y, short clutX, short clutY,
                          short clutWidth, short clutHeight);
int ArchiveSetIndex(int directoryIndex, int entryIndex);
int ArchiveDecodeAlignedSize(unsigned int entryIndex);
int ArchiveDataSync(void);
int ArchiveCdDataSync(int mode);
int func_80029AFC(StreamDataQueueEntry* pEntries, int arg1, int arg2);
int func_8009C538(int targetId);

// https://decomp.me/scratch/tL6mE
#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): field_dialogue_show_portrait (src/decomp/src/field/field_event.c:8366). */
#ifndef XM_T_xm_FieldSlot6
#define XM_T_xm_FieldSlot6
typedef struct xm_FieldSlot6 {
    s16 a;
    s16 b;
    s16 c;
} xm_FieldSlot6;
#endif
#ifndef XM_T_xm_PortraitPlace
#define XM_T_xm_PortraitPlace
typedef struct {
    s16 x;
    s16 y;
    s16 clut_x;
    s16 clut_y;
} xm_PortraitPlace;
#endif
#ifndef XM_T_PLACE
#define XM_T_PLACE
#define PLACE(i, k) (((s16 *)xm_field_dialogue_portrait_places)[(i) * 8 + (k)])
#endif
#ifndef XM_T_FILES
#define XM_T_FILES
#define FILES(c, k) (((u8 *)xm_field_dialogue_portrait_files)[(c) * 2 + (k)])
#endif
#ifndef XM_T_xm_FileRequest
#define XM_T_xm_FileRequest
typedef struct xm_FileRequest {
    u16 file;
    void *destination;
} xm_FileRequest;
#endif
#ifndef XM_S_field_dialogue_show_portrait
#define XM_S_field_dialogue_show_portrait
s32 xm_field_dialogue_show_portrait(s32 character) __asm__("func_8009C154");
#endif
#ifndef XM_S_field_dialogue_portrait_slots
#define XM_S_field_dialogue_portrait_slots
extern xm_FieldSlot6 xm_field_dialogue_portrait_slots[3] __asm__("D_800B06A4");
#endif
#ifndef XM_S_cd_sync_reads
#define XM_S_cd_sync_reads
s32 xm_cd_sync_reads(s32 mode) __asm__("ArchiveCdDataSync");
#endif
#ifndef XM_S_field_load_tim_at
#define XM_S_field_load_tim_at
void xm_field_load_tim_at(u32 *tim, s16 x, s16 y, s16 clut_x, s16 clut_y, s16 clut_w, s16 clut_h) __asm__("FieldLoadTIMWithClut");
#endif
#ifndef XM_S_field_dialogue_portrait_first_image
#define XM_S_field_dialogue_portrait_first_image
extern void *xm_field_dialogue_portrait_first_image __asm__("D_800ADB10");
#endif
#ifndef XM_S_field_dialogue_portrait_places
#define XM_S_field_dialogue_portrait_places
extern xm_PortraitPlace xm_field_dialogue_portrait_places[4][2] __asm__("D_800AEAE4");
#endif
#ifndef XM_S_field_dialogue_portrait_second_image
#define XM_S_field_dialogue_portrait_second_image
extern void *xm_field_dialogue_portrait_second_image __asm__("D_800ADB14");
#endif
#ifndef XM_S_heap_free
#define XM_S_heap_free
s32 xm_heap_free(void *data) __asm__("HeapFree");
#endif
#ifndef XM_S_field_dialogue_portrait_last_slot
#define XM_S_field_dialogue_portrait_last_slot
extern s32 xm_field_dialogue_portrait_last_slot __asm__("D_800ADB0C");
#endif
#ifndef XM_S_field_dialogue_is_portrait_shown
#define XM_S_field_dialogue_is_portrait_shown
s32 xm_field_dialogue_is_portrait_shown(s32 id) __asm__("func_8009C538");
#endif
#ifndef XM_S_cd_select_directory
#define XM_S_cd_select_directory
s32 xm_cd_select_directory(s32 group, s32 index) __asm__("ArchiveSetIndex");
#endif
#ifndef XM_S_field_dialogue_portrait_files
#define XM_S_field_dialogue_portrait_files
extern u8 xm_field_dialogue_portrait_files[90][2] __asm__("D_800AE1E0");
#endif
#ifndef XM_S_field_dialogue_portrait_file_requests
#define XM_S_field_dialogue_portrait_file_requests
extern xm_FileRequest xm_field_dialogue_portrait_file_requests[3] __asm__("D_800B00C8");
#endif
#ifndef XM_S_heap_alloc
#define XM_S_heap_alloc
void *xm_heap_alloc(s32 size, s32 mode) __asm__("HeapAlloc");
#endif
#ifndef XM_S_cd_get_aligned_file_size
#define XM_S_cd_get_aligned_file_size
s32 xm_cd_get_aligned_file_size(s32 file) __asm__("ArchiveDecodeAlignedSize");
#endif
#ifndef XM_S_cd_read_file_list
#define XM_S_cd_read_file_list
s32 xm_cd_read_file_list(xm_FileRequest *list, s32 mode, s32 unused) __asm__("func_80029AFC");
#endif
s32 func_8009C154(s32 character) {
    s32 i;
    s32 found;
    s32 file;

    for (i = 0; i < 3; i++) {
        if (xm_field_dialogue_portrait_slots[i].b == 1) {
            if (xm_cd_sync_reads(1) == 0) {
                xm_field_dialogue_portrait_slots[i].b = 2;
                xm_field_load_tim_at(xm_field_dialogue_portrait_first_image, PLACE(i, 0), PLACE(i, 1), PLACE(i, 2),
                              PLACE(i, 3), 0x100, 1);
                if (xm_field_dialogue_portrait_slots[i].c == 0) {
                    xm_field_load_tim_at(xm_field_dialogue_portrait_first_image, PLACE(i, 4), PLACE(i, 5), PLACE(i, 6),
                                  PLACE(i, 7), 0x100, 1);
                } else {
                    xm_field_load_tim_at(xm_field_dialogue_portrait_second_image, PLACE(i, 4), PLACE(i, 5), PLACE(i, 6),
                                  PLACE(i, 7), 0x100, 1);
                }
            }
            return -1;
        }
        if (xm_field_dialogue_portrait_slots[i].b == 2) {
            xm_field_dialogue_portrait_slots[i].b = 0;
            xm_heap_free(xm_field_dialogue_portrait_first_image);
            if (xm_field_dialogue_portrait_slots[i].c == 1) {
                xm_heap_free(xm_field_dialogue_portrait_second_image);
            }
            return -1;
        }
    }
    for (i = 0; i < 3; i++) {
        if (xm_field_dialogue_portrait_slots[i].a == character) {
            xm_field_current_event_actor->state.bits.unk2 = i;
            return 0;
        }
    }
    found = 0;
    for (i = 0; i < 3; i++) {
        xm_field_dialogue_portrait_last_slot++;
        if (xm_field_dialogue_portrait_last_slot >= 3) {
            xm_field_dialogue_portrait_last_slot = 0;
        }
        if (xm_field_dialogue_is_portrait_shown(xm_field_dialogue_portrait_slots[xm_field_dialogue_portrait_last_slot].a) == 0) {
            found++;
            break;
        }
    }
    if (found == 0) {
        return -1;
    }
    xm_field_current_event_actor->state.bits.unk2 = xm_field_dialogue_portrait_last_slot;
    xm_cd_select_directory(4, 0);
    xm_field_dialogue_portrait_slots[xm_field_dialogue_portrait_last_slot].a = character;
    xm_field_dialogue_portrait_slots[xm_field_dialogue_portrait_last_slot].b = 1;
    xm_field_dialogue_portrait_slots[xm_field_dialogue_portrait_last_slot].c = 0;
    i = 0;
    file = FILES(character, 0);
    xm_field_dialogue_portrait_file_requests[i].file = file + 0x46;
    xm_field_dialogue_portrait_file_requests[i].destination = xm_field_dialogue_portrait_first_image = xm_heap_alloc(xm_cd_get_aligned_file_size(file + 0x46), 0);
    i++;
    if (FILES(character, 1) != FILES(character, 0)) {
        xm_field_dialogue_portrait_slots[xm_field_dialogue_portrait_last_slot].c = 1;
        file = FILES(character, 1);
        xm_field_dialogue_portrait_file_requests[i].file = file + 0x46;
        xm_field_dialogue_portrait_file_requests[i].destination = xm_field_dialogue_portrait_second_image = xm_heap_alloc(xm_cd_get_aligned_file_size(file + 0x46), 0);
        i++;
    }
    xm_field_dialogue_portrait_file_requests[i].file = 0;
    xm_field_dialogue_portrait_file_requests[i].destination = NULL;
    xm_cd_read_file_list(xm_field_dialogue_portrait_file_requests, 0, 0);
    return -1;
}
#endif
#else
s32 func_8009C154(s32 faceId) {
    s32 slot;
    s32 attempt;
    s32 found;
    s32 entryCount;
    u8* pFacePair;
    u8 archive0;
    u8 archive1;
    s16* pCoords;
    u32* pFlags12C;
    void* pTim;

    for (slot = 0; slot < 3; slot++) {
        s16 state = D_800B06A6[slot * 3];

        if (state == 1) {
            /* Retail: ArchiveCdDataSync(1); busy => return -1. */
            if (ArchiveCdDataSync(1) != 0) {
                return -1;
            }

            D_800B06A6[slot * 3] = 2;
            pCoords = &D_800AEAE4[slot * 8];
            FieldLoadTIMWithClut((u_long*)D_800ADB10, pCoords[0], pCoords[1], pCoords[2], pCoords[3],
                                0x100, 1);
            pTim = (D_800B06A8[slot * 3] != 0) ? D_800ADB14 : D_800ADB10;
            FieldLoadTIMWithClut((u_long*)pTim, pCoords[4], pCoords[5], pCoords[6], pCoords[7], 0x100,
                                1);
            return -1;
        }

        if (state == 2) {
            D_800B06A6[slot * 3] = 0;
            HeapFree(D_800ADB10);
            if (D_800B06A8[slot * 3] == 1) {
                HeapFree(D_800ADB14);
            }
            return -1;
        }
    }

    for (slot = 0; slot < 3; slot++) {
        if (D_800B06A4[slot * 3] == faceId) {
            pFlags12C = (u32*)((u8*)g_FieldScriptVMCurActor + 0x12C);
            *pFlags12C = (*pFlags12C & ~0x1Cu) | ((slot & 7) << 2);
            return 0;
        }
    }

    found = 0;
    for (attempt = 0; attempt < 3; attempt++) {
        D_800ADB0C++;
        if (D_800ADB0C >= 3) {
            D_800ADB0C = 0;
        }
        if (func_8009C538(D_800B06A4[D_800ADB0C * 3]) == 0) {
            found = 1;
            break;
        }
    }

    if (!found) {
        return -1;
    }

    pFlags12C = (u32*)((u8*)g_FieldScriptVMCurActor + 0x12C);
    *pFlags12C = (*pFlags12C & ~0x1Cu) | ((D_800ADB0C & 7) << 2);

    ArchiveSetIndex(4, 0);

    pFacePair = &D_800AE1E0[faceId << 1];
    D_800B06A4[D_800ADB0C * 3] = (s16)faceId;
    D_800B06A6[D_800ADB0C * 3] = 1;
    D_800B06A8[D_800ADB0C * 3] = 0;

    archive0 = pFacePair[0];
    D_800B00C8[0].archiveIndex = (s16)(archive0 + 0x46);
    D_800ADB10 = HeapAlloc(ArchiveDecodeAlignedSize(archive0 + 0x46), 0);
    D_800B00C8[0].pData = D_800ADB10;

    archive1 = pFacePair[1];
    entryCount = 1;
    if (archive1 != archive0) {
        D_800B06A8[D_800ADB0C * 3] = 1;
        D_800B00C8[1].archiveIndex = (s16)(archive1 + 0x46);
        D_800ADB14 = HeapAlloc(ArchiveDecodeAlignedSize(archive1 + 0x46), 0);
        D_800B00C8[1].pData = D_800ADB14;
        entryCount = 2;
    }

    D_800B00C8[entryCount].archiveIndex = 0;
    D_800B00C8[entryCount].pData = NULL;
    func_80029AFC(D_800B00C8, 0, 0);
    return -1;
}
#endif /* XENO_PC_PORT */

// Check if there is a portrait w/ targetId that's free to use?
int func_8009C538(int targetId) {
    int i;
    
    for (i = 0; i < 4; i++) {
        if (g_FieldTextBoxes[i].visibility || g_FieldTextBoxes[i].portrait.shouldRenderPortrait != 1) {
            continue;
        }
        
        if (g_FieldTextBoxes[i].portrait.portraitID == targetId) {
            return -1;
        }
    }

    return 0;
}


#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/dialogue/text_box", func_8009C5A8);
#else
s32 func_8009C5A8(s32 actorIndex, s32 mode) {
    s32 existingTextBoxIndex;
    s32 textBoxIndex;
    s32 stringIndex;
    s32 width;
    s32 height;
    s32 screenX = 0xA0;
    s32 screenY = 0;
    s32 x;
    s32 y;
    s32 usedBoxCount;
    s32 usedBoxFlags;
    s32 dialogFlags;
    s32 placementType;
    s32 orientationFlags = 0;
    s32 i;
    ActorData* targetActor;

    g_FieldScriptMaxInstructionCount += 0x20;

    if (D_800ADB2C != 0 || D_800AFD04 != 0 || D_800C4268 != 0 || D_800ADB64 != 0xFF ||
        (D_800ADB70 == 0 && func_8008A558() != 0)) {
        D_800B00C0 = 1;
        return -1;
    }

    if (g_FieldScriptVMCurActor->faceId != 0xFF && func_8009C154(g_FieldScriptVMCurActor->faceId) == -1) {
        D_800B00C0 = 1;
        return -1;
    }

    D_800C4268++;
    if (func_8009CD18(&existingTextBoxIndex) != -1) {
        D_800B00C0 = 1;
        g_FieldTextBoxes[existingTextBoxIndex].status = 0;
        return -1;
    }

    g_FieldScriptMaxInstructionCount += 8;
    stringIndex = FieldScriptVMGetInstructionArgument(1) & 0xFFFF;

    if (func_80080720() != 0) {
        textBoxIndex = func_80080760();
        if (textBoxIndex != 0xFFFF) {
            g_FieldTextBoxes[textBoxIndex].status = 0;
            D_800B00C0 = 1;
            return -1;
        }
    } else {
        textBoxIndex = func_800807B4();
    }

    usedBoxCount = 0;
    usedBoxFlags = 0;
    for (i = 0; i < 4; i++) {
        if (g_FieldTextBoxes[i].visibility == 0) {
            usedBoxCount++;
            usedBoxFlags |= g_FieldTextBoxes[i].flags;
        }
    }

    width = DialogGetWidth(D_800ADBF0, stringIndex);
    height = DialogGetHeight(D_800ADBF0, stringIndex);

    if (mode == 0 || mode == 3) {
        if (g_FieldScriptVMCurActor->dialogWidth != 0) {
            width = g_FieldScriptVMCurActor->dialogWidth;
        }
        if (g_FieldScriptVMCurActor->dialogHeight != 0) {
            height = g_FieldScriptVMCurActor->dialogHeight;
        }
    }

    /* Retail 8009C824..8009C858: the low half is ALWAYS written back
     * (clearing the upper half) before the script byte is tested. */
    dialogFlags = g_FieldScriptVMCurActor->dialogFlags & 0xFFFF;
    g_FieldScriptVMCurActor->dialogFlags = dialogFlags;
    if (SCRIPT_READ_U8_REL(3) != 0) {
        dialogFlags = (dialogFlags & 0xFF00) | SCRIPT_READ_U8_REL(3);
        g_FieldScriptVMCurActor->dialogFlags = ((u32)dialogFlags << 16) | (g_FieldScriptVMCurActor->dialogFlags & 0xFFFF);
    }

    /* Retail 8009C85C..8009CA60: two exclusive arms (flags 1 / flags 0x81),
     * each with ONE func_8007F814 call for modes 0/3; placement type 3 stores
     * no flags and goes straight to the clamp with y = 0x10 and retail's
     * (uninitialised) screenX -- the port keeps 0xA0 there (DIVERGES). */
    placementType = (dialogFlags >> 4) & 3;
    y = 0x10;
    if (placementType != 3) {
        s32 below;

        if (placementType == 0) {
            s32 facingCamera;

            facingCamera = ((g_FieldScriptVMCurActor->direction - (FieldGetCameraDirection() & 0xFFFF)) &
                            MASK_8DIR_MOVEMENT_NUM_DIRECTIONS) < 5;
            if (!facingCamera) {
                below = (usedBoxFlags & 0x80) != 0 || usedBoxCount != 0;
            } else {
                below = (usedBoxFlags & 0x80) == 0;
            }
        } else {
            below = placementType == 2;
        }

        g_FieldTextBoxes[textBoxIndex].flags = below ? 0x81 : 1;
        if (mode == 0 || mode == 3) {
            func_8007F814(actorIndex, &screenX, &screenY, -0x40);
            if (mode == 0) {
                y = below ? (s32)((u32)screenY + 0x30) : (s32)((u32)screenY - (u32)(height * 14) - 0x24);
            } else {
                y = below ? 0x94 : 0x14;
                screenX = 0xA0;
            }
            if (g_FieldScriptVMCurActor->faceId != 0xFF && !(dialogFlags & 2)) {
                if (width < 0x18) {
                    width = 0x18;
                }
                width += 0x11;
                height = 4;
                y = below ? 0x94 : 0x10;
            }
        } else {
            width = 0x48;
            height = 4;
            y = below ? 0x94 : 0x10;
            screenX = 0xA0;
        }
    }

    /* 8009CA64..8009CAD4: addiu/subu wrap, then signed slt -- u32
     * arithmetic (no host signed-overflow UB for any screen position). */
    x = (s32)((u32)screenX - 8u - ((u32)width << 1));
    if (x < 0xC) {
        x = 0xC;
    }
    if ((s32)((u32)x + 0x10u + ((u32)width << 2)) >= 0x135) {
        x = (s32)(0x124u - ((u32)width << 2));
    }

    if (y < 0x10) {
        y = 0x10;
    }
    if ((s32)((u32)y + 8u + (u32)height * 14u) >= 0xD5) {
        y = (s32)(0xCCu - (u32)height * 14u);
    }

    if (mode == 0 || mode == 3) {
        if (g_FieldScriptVMCurActor->dialogPixelWidth != 0) {
            x = g_FieldScriptVMCurActor->dialogPixelWidth;
        }
        if (g_FieldScriptVMCurActor->dialogPixelHeight != 0) {
            y = g_FieldScriptVMCurActor->dialogPixelHeight;
        }
        if (g_FieldScriptVMCurActor->dialogWidth != 0) {
            width = g_FieldScriptVMCurActor->dialogWidth;
        }
        if (g_FieldScriptVMCurActor->dialogHeight != 0) {
            height = g_FieldScriptVMCurActor->dialogHeight;
        }
        if (g_FieldScriptVMCurActor->faceId != 0xFF && !(dialogFlags & 2)) {
            height = 4;
        }
    }

    if (dialogFlags & 0x40) {
        g_FieldTextBoxes[textBoxIndex].flags |= 0x40;
    }

    if ((dialogFlags & 0xC) == 0) {
        targetActor = (ActorData*)(uintptr_t)g_FieldActors[actorIndex].pActorData;
        orientationFlags = (((((s16)targetActor->rotation.vy >> 9) - (FieldGetCameraDirection() & 0xFFFF) + 1) &
                             MASK_8DIR_MOVEMENT_NUM_DIRECTIONS) < 4) ? 0 : 0x400;
    } else if (dialogFlags & 4) {
        orientationFlags = 0x400;
    }

    func_8007F8DC((s16)x, (s16)y, stringIndex, textBoxIndex, width, height, D_800AFD1C, actorIndex, mode,
                  orientationFlags, dialogFlags);
    func_8009CCF8(textBoxIndex);
    g_FieldScriptVMCurActor->rotation.vx |= 0x8000;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 4;
#ifdef XENO_PC_PORT
    {
        const char* dump = getenv("XENO_NPC_EVENT_DUMP");
        if (dump != NULL && dump[0] != '\0' && dump[0] != '0') {
            printf("[npc-event] dialog-open box=%d str=%d lock=0x%x vis=%d\n",
                   (int)textBoxIndex, (int)stringIndex,
                   (unsigned)D_800B2174[0],
                   (int)g_FieldTextBoxes[textBoxIndex].visibility);
        }
    }
#endif
    return 0;
}
#endif /* XENO_PC_PORT */

extern u16 D_800B2174[];

void func_8009CCF8(int arg0) {
    D_800B2174[0] |= 1 << arg0;
}
// find_first_unused()?
int func_8009CD18(int* pTextBoxIndex) {
    int i;

    for (i = 0; i < 4; i++) {
        if (g_FieldTextBoxes[i].ownerActorID == D_800AFD1C && g_FieldTextBoxes[i].visibility == 0) {
            *pTextBoxIndex = i;
            return 0;
        }
    }

    return -1;
}

u32 func_8009CD7C(int bytecodeOffset) {
    u32 nActorIndex = FieldScriptVMGetActorIndex(bytecodeOffset);
    if (nActorIndex == ACTOR_ID_INVALID) {
        return D_8005A444;
    }
    return nActorIndex;
}

u32 FieldScriptVMGetActorIndex(int bytecodeOffset) {
    u32 actorID = SCRIPT_READ_U8_REL(bytecodeOffset);
    if (actorID == ACTOR_ID_INVALID) {
        actorID = D_8005A444;  
    } else if (actorID == 0xFE) {
        actorID = D_8005A448;
    } else if (actorID == 0xFD) {
        actorID = D_8005A44C;
    } else if (actorID == 0xFB) {
        actorID = D_800AFD1C;
    }
    return actorID;
}


// Set up dialog window / text box
void func_8009CE48(void) {
    g_FieldScriptVMCurActor->dialogPixelWidth = SCRIPT_READ_U8_REL(1) * 2;
    g_FieldScriptVMCurActor->dialogPixelHeight = SCRIPT_READ_U8_REL(2);
    g_FieldScriptVMCurActor->dialogWidth = SCRIPT_READ_U8_REL(3) * 3;
    g_FieldScriptVMCurActor->dialogHeight = SCRIPT_READ_U8_REL(4);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
}


// Set up dialog window / text box
void func_8009CEE0(void) {
    g_FieldScriptVMCurActor->dialogPixelWidth = FieldScriptVMGetArgument(1);
    g_FieldScriptVMCurActor->dialogPixelHeight = FieldScriptVMGetArgument(3);
    g_FieldScriptVMCurActor->dialogWidth = FieldScriptVMGetArgument(5) * 3;
    g_FieldScriptVMCurActor->dialogHeight = FieldScriptVMGetArgument(7);
    g_FieldScriptVMCurActor->dialogFlags = FieldScriptVMGetArgument(9);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 0xB;
}
