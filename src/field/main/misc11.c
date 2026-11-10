#include "common.h"
#include "main/game.h"
#include "system/math.h"
#include "field/main.h"
#include "field/camera.h"
#include "field/actor.h"
#include "field/script_vm.h"
#include "field/text_box.h"
#include "field/particles.h"

extern s32 D_800AFD1C;
extern s32 g_PlayerActorIndex;
extern FieldActor* D_800B06B8;

extern u8 D_800B2190;
extern u8 D_800B2191;
extern u8 D_800B2192;
extern u8 D_800B2194;
extern u8 D_800B2195;
extern u8 D_800B2196;
extern s16 D_800B2198;
extern s16 D_800B219A;
extern s16 D_800B218E;
extern void func_80073E38(void);

/* Opcode 0xE6 — SETUP_FOG: near RGB, far RGB, near/far Z, enable depth-cue. */
void func_80091944(void) {
    D_800B2190 = FieldScriptVMGetArgument(1);
    D_800B2191 = FieldScriptVMGetArgument(3);
    D_800B2192 = FieldScriptVMGetArgument(5);
    D_800B2194 = FieldScriptVMGetArgument(7);
    D_800B2195 = FieldScriptVMGetArgument(9);
    D_800B2196 = FieldScriptVMGetArgument(0xB);
    D_800B2198 = FieldScriptVMGetArgument(0xD);
    D_800B219A = FieldScriptVMGetArgument(0xF);
    D_800B218E = 1;
    func_80073E38();
    g_FieldScriptVMCurActor->scriptInstructionPointer += 0x11;
}

/* Typed array view of g_Scene: retail schedules these stores like stores to
 * an array-typed extern. */
extern s16 g_Scene_s16[] asm("g_Scene");

void func_80091A08(void) {
    g_Scene_s16[0x4C / 2] = FieldScriptVMGetInstructionArgumentS16(1);
    g_Scene_s16[0x4E / 2] = FieldScriptVMGetInstructionArgumentS16(3);
    g_Scene_s16[0x50 / 2] = FieldScriptVMGetInstructionArgumentS16(5);
    g_Scene_s16[0x52 / 2] = -FieldScriptVMGetInstructionArgumentS16(7);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 9;
}

extern u8 D_800B219C;
extern u8 D_800B219D;
extern u8 D_800B219E;

void func_80091A78(void) {
#ifdef XENO_PC_PORT
    D_800B219C = FieldScriptVMGetArgument(1);
    D_800B219D = FieldScriptVMGetArgument(3);
    D_800B219E = FieldScriptVMGetArgument(5);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 7;
#else
    (*(s8*)&D_800B219C) = FieldScriptVMGetArgument(1);
    (*(s8*)&D_800B219D) = FieldScriptVMGetArgument(3);
    (*(s8*)&D_800B219E) = FieldScriptVMGetArgument(5);
    *(u16 *)((s8*)(g_FieldScriptVMCurActor) + 0xCC) = (u16) (*(u16 *)((s8*)(g_FieldScriptVMCurActor) + 0xCC) + 7);
#endif
}

void func_80091AD4(void) {
}

extern s16 D_800AF5E8[];
extern s16 D_800AF6E8[];
extern s16 D_800AF728[];
extern u16 D_800AF768;

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): field_vram_move_or_clear_rect (src/decomp/src/field/field_event.c:4458). */
#ifndef XM_S_field_vram_move_or_clear_rect
#define XM_S_field_vram_move_or_clear_rect
void xm_field_vram_move_or_clear_rect(s32 x, s32 y, s32 w, s32 h, s32 dx, s32 dy, s32 clear) __asm__("func_80091ADC");
#endif
#ifndef XM_S_field_vram_ring_count
#define XM_S_field_vram_ring_count
extern u16 xm_field_vram_ring_count __asm__("D_800AF768");
#endif
#ifndef XM_S_field_vram_ring_rects
#define XM_S_field_vram_ring_rects
extern RECT xm_field_vram_ring_rects[32] __asm__("D_800AF5E8");
#endif
#ifndef XM_S_field_vram_ring_destination_x_table
#define XM_S_field_vram_ring_destination_x_table
extern s16 xm_field_vram_ring_destination_x_table[32] __asm__("D_800AF6E8");
#endif
#ifndef XM_S_field_vram_ring_unread_destination_y_table
#define XM_S_field_vram_ring_unread_destination_y_table
extern s16 xm_field_vram_ring_unread_destination_y_table[32] __asm__("D_800AF728");
#endif
#ifndef XM_S_MoveImage
#define XM_S_MoveImage
int xm_MoveImage(RECT *rect, int x, int y) __asm__("MoveImage");
#endif
#ifndef XM_S_ClearImage
#define XM_S_ClearImage
int xm_ClearImage(RECT *rect, u_char r, u_char g, u_char b) __asm__("ClearImage");
#endif
void func_80091ADC(s32 x, s32 y, s32 w, s32 h, s32 dx, s32 dy, s32 clear) {
    s32 slot;

    slot = xm_field_vram_ring_count & 0x1F;
    xm_field_vram_ring_rects[slot].y = y;
    xm_field_vram_ring_rects[slot].x = x;
    xm_field_vram_ring_rects[slot].w = w;
    xm_field_vram_ring_rects[slot].h = h;
    xm_field_vram_ring_destination_x_table[slot] = dx;
    xm_field_vram_ring_unread_destination_y_table[slot] = dy;
    if (clear == 0) {
        xm_MoveImage(&xm_field_vram_ring_rects[slot], xm_field_vram_ring_destination_x_table[slot], (s16)dy);
    } else {
        xm_ClearImage(&xm_field_vram_ring_rects[slot], 0, 0, 0);
    }
    xm_field_vram_ring_count++;
}
#endif
#else
void func_80091ADC(s16 x, s16 y, s16 w, s16 h, s16 extra, s16 mode, s32 doMove) {
    u16 idx = D_800AF768 & 0x1F;
    s16* rect = (s16*)((u8*)D_800AF5E8 + idx * 8);
    rect[0] = x;
    rect[1] = y;
    rect[2] = w;
    rect[3] = h;
    D_800AF6E8[idx] = extra;
    D_800AF728[idx] = mode;
    if (doMove) {
        ClearImage((RECT*)rect, 0, 0, 0);
    } else {
        MoveImage((RECT*)rect, D_800AF6E8[idx], (s16)mode);
    }
    D_800AF768++;
}
#endif /* XENO_PC_PORT */

extern int FieldScriptArgument5(int index, int mask);
extern int FieldScriptArgument6(int index, int mask);

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): field_event_vram_rectangle (src/decomp/src/field/field_event.c:4479). */
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
#ifndef XM_S_field_event_vram_rectangle
#define XM_S_field_event_vram_rectangle
void xm_field_event_vram_rectangle(void) __asm__("func_80091BBC");
#endif
#ifndef XM_S_field_event_read_selected_operand_80
#define XM_S_field_event_read_selected_operand_80
s32 xm_field_event_read_selected_operand_80(s32 offset, s32 flags) __asm__("FieldScriptArgument1");
#endif
#ifndef XM_S_field_event_bytecode
#define XM_S_field_event_bytecode
extern u8 *xm_field_event_bytecode __asm__("g_FieldScriptVMCurScriptData");
#endif
#ifndef XM_S_field_current_event_actor
#define XM_S_field_current_event_actor
extern xm_FieldActor *xm_field_current_event_actor __asm__("g_FieldScriptVMCurActor");
#endif
#ifndef XM_S_field_event_read_selected_operand_40
#define XM_S_field_event_read_selected_operand_40
s32 xm_field_event_read_selected_operand_40(s32 offset, s32 flags) __asm__("FieldScriptArgument2");
#endif
#ifndef XM_S_field_event_read_selected_operand_20
#define XM_S_field_event_read_selected_operand_20
s32 xm_field_event_read_selected_operand_20(s32 offset, s32 flags) __asm__("FieldScriptArgument3");
#endif
#ifndef XM_S_field_event_read_selected_operand_10
#define XM_S_field_event_read_selected_operand_10
s32 xm_field_event_read_selected_operand_10(s32 offset, s32 flags) __asm__("FieldScriptArgument4");
#endif
#ifndef XM_S_field_event_read_selected_operand_08
#define XM_S_field_event_read_selected_operand_08
s32 xm_field_event_read_selected_operand_08(s32 offset, s32 flags) __asm__("FieldScriptArgument5");
#endif
#ifndef XM_S_field_event_read_selected_operand_04
#define XM_S_field_event_read_selected_operand_04
s32 xm_field_event_read_selected_operand_04(s32 offset, s32 flags) __asm__("FieldScriptArgument6");
#endif
void func_80091BBC(void) {
    s32 x;
    s32 y;
    RECT unused; 

    x = xm_field_event_read_selected_operand_80(1, EVENT_OPERAND_BYTE(0xD));
    y = xm_field_event_read_selected_operand_40(3, EVENT_OPERAND_BYTE(0xD));
    if (x == 0 && y == 0) {
        xm_field_vram_move_or_clear_rect(xm_field_event_read_selected_operand_20(5, EVENT_OPERAND_BYTE(0xD)), xm_field_event_read_selected_operand_10(7, EVENT_OPERAND_BYTE(0xD)),
                      xm_field_event_read_selected_operand_08(9, EVENT_OPERAND_BYTE(0xD)), xm_field_event_read_selected_operand_04(0xB, EVENT_OPERAND_BYTE(0xD)), 0, 0, 1);
    } else {
        xm_field_vram_move_or_clear_rect(x, y, xm_field_event_read_selected_operand_20(5, EVENT_OPERAND_BYTE(0xD)), xm_field_event_read_selected_operand_10(7, EVENT_OPERAND_BYTE(0xD)),
                      xm_field_event_read_selected_operand_08(9, EVENT_OPERAND_BYTE(0xD)), xm_field_event_read_selected_operand_04(0xB, EVENT_OPERAND_BYTE(0xD)), 0);
    }
    xm_field_current_event_actor->pc += 0xE;
}
#endif
#else
void func_80091BBC(void) {
    u8 mask;
    s32 arg1, arg2, arg3, arg4, arg5, arg6;

    mask = SCRIPT_READ_U8_REL(0xD);
    arg1 = FieldScriptArgument1(1, mask);
    mask = SCRIPT_READ_U8_REL(0xD);
    arg2 = FieldScriptArgument2(3, mask);

    if (arg1 == 0 && arg2 == 0) {
        mask = SCRIPT_READ_U8_REL(0xD);
        arg3 = FieldScriptArgument3(5, mask);
        mask = SCRIPT_READ_U8_REL(0xD);
        arg4 = FieldScriptArgument4(7, mask);
        mask = SCRIPT_READ_U8_REL(0xD);
        arg5 = FieldScriptArgument5(9, mask);
        mask = SCRIPT_READ_U8_REL(0xD);
        arg6 = FieldScriptArgument6(0xB, mask);
        func_80091ADC(arg3, arg4, arg5, arg6, 0, 0, 1);
    } else {
        mask = SCRIPT_READ_U8_REL(0xD);
        arg3 = FieldScriptArgument3(5, mask);
        mask = SCRIPT_READ_U8_REL(0xD);
        arg4 = FieldScriptArgument4(7, mask);
        mask = SCRIPT_READ_U8_REL(0xD);
        arg5 = FieldScriptArgument5(9, mask);
        mask = SCRIPT_READ_U8_REL(0xD);
        arg6 = FieldScriptArgument6(0xB, mask);
        func_80091ADC(arg1, arg2, arg3, arg4, arg5, arg6, 0);
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 0xE;
}
#endif /* XENO_PC_PORT */

#ifndef XENO_PC_PORT
/* func_80091E00: struct-field flags134 store (in-struct) lets the script-data
 * load for the second operand hoist above it. */
#endif
void func_80091E00(void) {
#ifdef XENO_PC_PORT
    u8 mask = SCRIPT_READ_U8_REL(5);
    s32 arg1 = FieldScriptArgument1(1, mask);
    s32 motionType = arg1 & 3;
    u32 flags = *(u32*)((u8*)g_FieldScriptVMCurActor + 0x134);
    *(u32*)((u8*)g_FieldScriptVMCurActor + 0x134) = (flags & 0xFFFFFF9F) | (motionType << 5);
    mask = SCRIPT_READ_U8_REL(5);
    *(u16*)((u8*)g_FieldScriptVMCurActor + 0xEE) = (u16)FieldScriptArgument2(3, mask);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 6;
#else
    s32 arg1 = FieldScriptArgument1(1, SCRIPT_READ_U8_REL(5));
    g_FieldScriptVMCurActor->flags134 = (g_FieldScriptVMCurActor->flags134 & ~0x60) | ((arg1 & 3) << 5);
    g_FieldScriptVMCurActor->unkEE = FieldScriptArgument2(3, SCRIPT_READ_U8_REL(5));
    g_FieldScriptVMCurActor->scriptInstructionPointer += 6;
#endif
}

extern int FieldScriptVMGetActorIndex(int);
extern int FieldScriptArgument1(int index, int mask);
extern int FieldScriptArgument2(int index, int mask);

void func_80091E98(void) {
#ifdef XENO_PC_PORT
    u8 mask;
    FieldActor* pActor;
    void* pActorData;
    s32 motionType;

    if (FieldScriptVMGetActorIndex(1) == ACTOR_ID_INVALID) {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 7;
        return;
    }
    pActor = &g_FieldActors[FieldScriptVMGetActorIndex(1)];
    pActorData = (void*)(uintptr_t)pActor->pActorData;
    mask = SCRIPT_READ_U8_REL(6);
    motionType = FieldScriptArgument1(2, mask) & 3;
    *(u32*)((u8*)pActorData + 0x134) = (*(u32*)((u8*)pActorData + 0x134) & 0xFFFFFF9F) | (motionType << 5);
    mask = SCRIPT_READ_U8_REL(6);
    *(s16*)((u8*)pActorData + 0xEE) = (s16)FieldScriptArgument2(4, mask);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 7;
#else
    ActorData* pActorData;

    if (FieldScriptVMGetActorIndex(1) != 0xFF) {
        pActorData = (ActorData*)(uintptr_t)g_FieldActors[FieldScriptVMGetActorIndex(1)].pActorData;
        pActorData->flags134 = (pActorData->flags134 & ~0x60) | ((FieldScriptArgument1(2, SCRIPT_READ_U8_REL(6)) & 3) << 5);
        pActorData->unkEE = FieldScriptArgument2(4, SCRIPT_READ_U8_REL(6));
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 7;
#endif
}

/* ---- func_80091F84: VM opcode 0xDB — write clamped value into the actor's
 * animation dispatch table.
 * asm 80091F84-80092040: arg(1) = table index, arg(3) = value, clamped to
 * 0xFFF (slti 0x1000 / ori 0xFFF). If g_FieldActors[D_800AFD1C].status has
 * bit 0x2000, stores it at ((s32*)actorData->unk118)[index] — the 0x80-byte
 * table func_80080A74 allocates under the same status gate. IP += 5
 * unconditionally (sh at 8009202C). */
void func_80091F84(void) {
    s32 index = FieldScriptVMGetArgument(1);
    s32 value = FieldScriptVMGetArgument(3);

    if (value >= 0x1000) {
        value = 0xFFF;
    }
    if (g_FieldActors[D_800AFD1C].status & 0x2000) {
        ((s32*)(uintptr_t)g_FieldScriptVMCurActor->unk118)[index] = value;
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
}

void func_80092044(void) {
    u16 addr1, addr2, addr3, addr4;
    s32 val1, val2;
    addr1 = (u16)FieldScriptVMGetInstructionArgument(1);
    val1 = FieldScriptVMGetVariableValue(addr1);
    addr2 = (u16)FieldScriptVMGetInstructionArgument(3);
    val2 = FieldScriptVMGetVariableValue(addr2);
    addr3 = (u16)FieldScriptVMGetInstructionArgument(3);
    FieldScriptMemoryWriteU16(addr3, val1);
    addr4 = (u16)FieldScriptVMGetInstructionArgument(1);
    FieldScriptMemoryWriteU16(addr4, val2);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
}

extern s16 D_800AFEA8;
extern void GfxLineScrollUpdate(void* pLineScroll);

#ifndef XENO_PC_PORT
/* func_800920D8: D_800AFEA8 is a count followed by an entry table (count re-read
 * as entries[-1] each iteration); reached through a >8-byte struct alias so it
 * is not gp-small (la + lh 0(v1)).  No separate guard: the for test is it. */
struct func_800920D8_s {
    s16 count;
    s16 pad;
    void* entries[16];
};
extern struct func_800920D8_s D_800AFEA8__s asm("D_800AFEA8");

#endif
void func_800920D8(void) {
#ifdef XENO_PC_PORT
    s32 i;
    u32* entries;

    if (D_800AFEA8 <= 0) {
        return;
    }

    entries = (u32*)((u8*)&D_800AFEA8 + 4);
    for (i = 0; i < D_800AFEA8; i++) {
        GfxLineScrollUpdate((void*)(uintptr_t)entries[i]);
    }
#else
    s32 i;

    for (i = 0; i < D_800AFEA8__s.count; i++) {
        GfxLineScrollUpdate(D_800AFEA8__s.entries[i]);
    }
#endif
}

extern s16 D_800AFFAC[];

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): field_event_set_scroll_byte (src/decomp/src/field/field_event.c:4561). */
#ifndef XM_T_xm_TextureScroll
#define XM_T_xm_TextureScroll
typedef struct xm_TextureScroll {
    u16 x, y;          
    u16 w, h;
    u16 step;          
    u16 count;         
    u16 source_x;      
    u16 source_y;      
    s8 *speeds;        
    u16 *phases;
} xm_TextureScroll;
#endif
#ifndef XM_T_xm_WindowList
#define XM_T_xm_WindowList
typedef struct xm_WindowList {
    s16 count;
    xm_TextureScroll *scrolls[32];
    u8 *buffers[32];
    s16 lengths[32];
} xm_WindowList;
#endif
#ifndef XM_S_field_event_set_scroll_byte
#define XM_S_field_event_set_scroll_byte
void xm_field_event_set_scroll_byte(void) __asm__("func_80092148");
#endif
#ifndef XM_S_field_event_read_imm_or_var
#define XM_S_field_event_read_imm_or_var
s32 xm_field_event_read_imm_or_var(s32 offset) __asm__("FieldScriptVMGetArgument");
#endif
#ifndef XM_S_field_event_read_u16
#define XM_S_field_event_read_u16
s32 xm_field_event_read_u16(s32 offset) __asm__("FieldScriptVMGetInstructionArgument");
#endif
#ifndef XM_S_field_texture_scrolls
#define XM_S_field_texture_scrolls
extern xm_WindowList xm_field_texture_scrolls __asm__("D_800AFEA8");
#endif
void func_80092148(void) {
    s32 table;
    s32 index;
    s32 value;

    table = xm_field_event_read_imm_or_var(1);
    index = xm_field_event_read_imm_or_var(3);
    value = xm_field_event_read_u16(5) & 0xFFFF;
    if (index < xm_field_texture_scrolls.lengths[table]) {
        xm_field_texture_scrolls.buffers[table][index] = value;
    }
    xm_field_current_event_actor->pc += 7;
}
#endif
#else
/* Retail reaches the line-scroll table through D_800AFFAC (== D_800AFEA8+0x104,
 * the s16 size[] table) and D_800AFFAC-0x80 (== D_800AFEA8+0x84, the u32
 * buffer[] slots).  On PC D_800AFFAC is a separate stub, so address both
 * through D_800AFEA8 + offset with retail 4-byte pointer slots.  The byte
 * view is an incomplete array so the host compiler does not take D_800AFEA8's
 * declared s16 size as the object size. */
extern u8 D_800AFEA8_tbl[] asm("D_800AFEA8");

void func_80092148(void) {
    s32 arg1 = FieldScriptVMGetArgument(1);
    s32 arg3 = FieldScriptVMGetArgument(3);
    u8 val = (u8)FieldScriptVMGetInstructionArgument(5);
    u8* tbl = D_800AFEA8_tbl;

    if (arg3 < ((s16*)(tbl + 0x104))[arg1]) {
        *(u8*)(uintptr_t)(((u32*)(tbl + 0x84))[arg1] + (u32)arg3) = val;
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 7;
}
#endif /* XENO_PC_PORT */

extern s32 D_800ADB8C;
extern s16 D_800AFEA8;

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): field_event_add_texture_scroll (src/decomp/src/field/field_event.c:4579). */
#ifndef XM_S_field_event_add_texture_scroll
#define XM_S_field_event_add_texture_scroll
void xm_field_event_add_texture_scroll(void) __asm__("func_800921E8");
#endif
#ifndef XM_S_field_party_rebuilding
#define XM_S_field_party_rebuilding
extern s32 xm_field_party_rebuilding __asm__("D_800ADB8C");
#endif
#ifndef XM_S_heap_alloc
#define XM_S_heap_alloc
void *xm_heap_alloc(s32 size, s32 mode) __asm__("HeapAlloc");
#endif
#ifndef XM_S_gpu_init_texture_scroll
#define XM_S_gpu_init_texture_scroll
void xm_gpu_init_texture_scroll(xm_TextureScroll *scroll, s16 x, s16 y, s16 w, s16 h, s16 count, s16 source_x, s16 source_y,
                   u8 *speeds) __asm__("GfxLineScrollInitialize");
#endif
void func_800921E8(void) {
    s32 length;
    s32 fill;
    s32 i;
    s32 x;
    s32 y;
    s32 width;
    s32 height;
    s32 a;

    if (xm_field_party_rebuilding == 0 && xm_field_texture_scrolls.count < 32) {
        length = xm_field_event_read_u16(9) & 0xFFFF;
        xm_field_texture_scrolls.lengths[xm_field_texture_scrolls.count] = length;
        xm_field_texture_scrolls.buffers[xm_field_texture_scrolls.count] = xm_heap_alloc(length + 1, 0);
        xm_field_texture_scrolls.scrolls[xm_field_texture_scrolls.count] = xm_heap_alloc(0x18, 0);
        fill = xm_field_event_read_u16(15) & 0xFFFF;
        for (i = 0; i < length; i++) {
            xm_field_texture_scrolls.buffers[xm_field_texture_scrolls.count][i] = fill;
        }
        x = (s16)xm_field_event_read_u16(1);
        y = (s16)xm_field_event_read_u16(3);
        width = (s16)xm_field_event_read_u16(5);
        height = (s16)xm_field_event_read_u16(7);
        a = (s16)xm_field_event_read_u16(11);
        xm_gpu_init_texture_scroll(xm_field_texture_scrolls.scrolls[xm_field_texture_scrolls.count], x, y, width, height, length, a,
                      xm_field_event_read_u16(13), xm_field_texture_scrolls.buffers[xm_field_texture_scrolls.count]);
        xm_field_texture_scrolls.count++;
    }
    xm_field_current_event_actor->pc += 17;
}
#endif
#else
/* Retail table at D_800AFEA8: s16 count @+0, u32 lineScroll[32] @+4,
 * u32 buffer[32] @+0x84, s16 size[32] @+0x104 (== D_800AFFAC).  The pointer
 * tables are 4-byte guest slots (consumers func_800920D8/func_80092148 read
 * u32[idx]); a host void* store would be 8 bytes wide and overrun the next
 * table, so store the pointers as u32 slots like retail's sw. */
void func_800921E8(void) {
    u8* tbl = D_800AFEA8_tbl;
    u32 count;
    u8 initVal;
    s32 i;
    s16 x, y, modulus, width, destX, destY;

    if (D_800ADB8C == 0 && *(s16*)tbl < 0x20) {
        count = FieldScriptVMGetInstructionArgument(9) & 0xFFFF;
        ((s16*)(tbl + 0x104))[*(s16*)tbl] = (s16)count;
        ((u32*)(tbl + 0x84))[*(s16*)tbl] = (u32)(uintptr_t)HeapAlloc(count + 1, 0);
        ((u32*)(tbl + 0x4))[*(s16*)tbl] = (u32)(uintptr_t)HeapAlloc(0x18, 0);
        initVal = (u8)FieldScriptVMGetInstructionArgument(0xF);
        for (i = 0; i < (s32)count; i++) {
            *(u8*)(uintptr_t)(((u32*)(tbl + 0x84))[*(s16*)tbl] + (u32)i) = initVal;
        }

        x = FieldScriptVMGetInstructionArgument(1);
        y = FieldScriptVMGetInstructionArgument(3);
        modulus = FieldScriptVMGetInstructionArgument(5);
        width = FieldScriptVMGetInstructionArgument(7);
        destX = FieldScriptVMGetInstructionArgument(0xB);
        destY = FieldScriptVMGetInstructionArgument(0xD);

        GfxLineScrollInitialize(
            (void*)(uintptr_t)((u32*)(tbl + 0x4))[*(s16*)tbl],
            x, y, modulus, width, (s16)count, destX, destY,
            (s8*)(uintptr_t)((u32*)(tbl + 0x84))[*(s16*)tbl]);

        *(u16*)tbl = *(u16*)tbl + 1;
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 0x11;
}
#endif /* XENO_PC_PORT */

void func_800923E4(void) {
    g_FieldScriptVMCurActor->scriptInstructionPointer++;
}

void func_80092404(void) {
    g_FieldScriptVMCurActor->scriptInstructionPointer++;
}

#ifdef XENO_PC_PORT
/* FieldLoad stores packed 32-bit addresses here, including the next layer
 * pointer at +4. A native pointer load would combine those two words. */
extern u32 D_800AFB20[];
#define FIELD_COLLISION_FLAGS ((u8*)(uintptr_t)D_800AFB20[0])
#else
extern u32* D_800AFB20;
#define FIELD_COLLISION_FLAGS ((u8*)D_800AFB20)
#endif

u32 func_80092424(s32 index, s32 component) {
    switch (component) {
        case 0: return FIELD_COLLISION_FLAGS[index * 4];
        case 1: return FIELD_COLLISION_FLAGS[index * 4 + 1];
        case 2: return FIELD_COLLISION_FLAGS[index * 4 + 2];
        case 3: return FIELD_COLLISION_FLAGS[index * 4 + 3];
    }
    return 0;
}

void func_800924D4(s32 index, s32 component, s32 value) {
#ifdef XENO_PC_PORT
    u32* entry;
    u32 mask;

    switch (component) {
        case 0:
            entry = (u32*)(FIELD_COLLISION_FLAGS + index * 4);
            *entry = (*entry & ~0xFFu) | (u32)value;
            break;
        case 1:
            entry = (u32*)(FIELD_COLLISION_FLAGS + index * 4);
            mask = 0xFFFF00FF;
            *entry = (*entry & mask) | ((u32)value << 8);
            break;
        case 2:
            entry = (u32*)(FIELD_COLLISION_FLAGS + index * 4);
            mask = 0xFF00FFFF;
            *entry = (*entry & mask) | ((u32)value << 16);
            break;
        case 3:
            entry = (u32*)(FIELD_COLLISION_FLAGS + index * 4);
            mask = 0x00FFFFFF;
            *entry = (*entry & mask) | ((u32)value << 24);
            break;
    }
#else
    switch (component) {
        case 0:
            D_800AFB20[index] = (D_800AFB20[index] & 0xFFFFFF00) | value;
            break;
        case 1:
            value = (u32)value << 8; D_800AFB20[index] = (D_800AFB20[index] & 0xFFFF00FF) | value;
            break;
        case 2:
            value = (u32)value << 16; D_800AFB20[index] = (D_800AFB20[index] & 0xFF00FFFF) | value;
            break;
        case 3:
            value = (u32)value << 24; D_800AFB20[index] = (D_800AFB20[index] & 0x00FFFFFF) | value;
            break;
    }
#endif
}

extern s32 D_800B217C;
extern s16 D_800B21D6;

void func_800925A0(void) {
    s32 arg1 = FieldScriptVMGetArgument(1);
    D_800B217C = arg1;
    switch (arg1) {
        case 0: D_800B21D6 = 8; break;
        case 1: D_800B21D6 = 6; break;
        case 2: D_800B21D6 = 4; break;
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void FieldScriptVMHandlerSetControllerBtnMask(void) {
    g_FieldControl.controllerBtnMask = FieldScriptVMGetInstructionArgument(1);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void func_80092664(void) {
    func_800924D4(SCRIPT_READ_U8_REL(1), SCRIPT_READ_U8_REL(2), FieldScriptVMGetArgument(3));
    g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
}

void func_800926C8(void) {
    u32 cur = func_80092424(SCRIPT_READ_U8_REL(1), SCRIPT_READ_U8_REL(2));
    cur |= FieldScriptVMGetArgument(3);
    func_800924D4(SCRIPT_READ_U8_REL(1), SCRIPT_READ_U8_REL(2), cur);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
}

void func_80092768(void) {
    u32 cur = func_80092424(SCRIPT_READ_U8_REL(1), SCRIPT_READ_U8_REL(2));
    cur &= FieldScriptVMGetArgument(3);
    func_800924D4(SCRIPT_READ_U8_REL(1), SCRIPT_READ_U8_REL(2), cur);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
}

void func_80092808(void) {
#ifdef XENO_PC_PORT
    u8* actorData = (u8*)g_FieldScriptVMCurActor;
    s16 angle = *(s16*)((u8*)D_800B06B8 + 0x52);
    s32 sinTerm = rsin(angle) * 9;
    s32 cosTerm;
    s32 zMove;

    *(s16*)(actorData + 0x60) = (s16)(u16)((u32)sinTerm >> 10);

    cosTerm = rcos(angle) * 9;
    zMove = -(cosTerm * 4);
    *(s16*)(actorData + 0x64) = (s16)(zMove >> 12);

    *(u32*)(actorData + 0x4) |= 0x800;
    *(u16*)(actorData + 0xCC) += 1;
#else
    g_FieldScriptVMCurActor->unk60.vx = (u32)(rsin(D_800B06B8->rotation.y) * 9) >> 10;
    g_FieldScriptVMCurActor->unk60.vz = -(rcos(D_800B06B8->rotation.y) * 36) >> 12;
    g_FieldScriptVMCurActor->flags |= 0x800;
    g_FieldScriptVMCurActor->scriptInstructionPointer++;
#endif
}

extern FieldActor* D_800B06B8;
extern long FieldGetVec1Magnitude(long x);
extern long FieldGetVec2Magnitude(long x, long y);
extern s32 func_8007B694(s32* arg0);
extern void func_800821F4(void* pSpriteData, s16 animIndex, void* pFieldActor);
extern s32 g_GameSceneMapNum;
extern s32 D_800ADBDC;
extern s32 D_800ADBE4;
extern s32 D_800ADBEC;

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): field_event_walk_player (src/decomp/src/field/field_event.c:4731). */
#ifndef XM_T_xm_SpritePart
#define XM_T_xm_SpritePart
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
} xm_SpritePart;
#endif
#ifndef XM_T_xm_SpriteRendererEntry
#define XM_T_xm_SpriteRendererEntry
typedef struct {
    s8 byte0;              
    s8 byte1;              
    s16 half2;
    s16 half4;
    s16 half6;
} xm_SpriteRendererEntry;
#endif
#ifndef XM_T_xm_SpriteOffset
#define XM_T_xm_SpriteOffset
typedef struct {
    s8 x, y;
} xm_SpriteOffset;
#endif
#ifndef XM_T_xm_SpriteRenderer
#define XM_T_xm_SpriteRenderer
typedef struct {
    s16 angle_x, angle_y, angle_z; 
    s16 scale_x, scale_y, scale_z; 
    MATRIX matrix;                 
    xm_SpritePart *parts[2];          
    xm_SpriteRendererEntry *pointer34; 
    struct xm_Sprite *next_pending;   
    xm_SpriteOffset offset;           
    u8 unknown3e[2];
    s32 word40;                    
    SVECTOR light_angles;          
    u16 light_colour[3];           
} xm_SpriteRenderer;
#endif
#ifndef XM_T_xm_Sprite
#define XM_T_xm_Sprite
typedef struct xm_Sprite {
    s32 x, y, z;                
    s32 speed_x, speed_y, speed_z; 
    s32 speed;                  
    s32 gravity;                
    xm_SpriteRenderer *renderer;   
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
    void (*callback)(struct xm_Sprite *sprite); 
    void *block;             
    struct xm_Sprite *parent;   
    struct xm_Sprite *partner;  
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
} xm_Sprite;
#endif
#ifndef XM_T_xm_FadeChannel
#define XM_T_xm_FadeChannel
typedef struct {
    DR_MODE modes[2]; 
    TILE tiles[2];   
    s32 level[3];
    s32 step[3];
    u16 abr;
    s16 active;
    s16 steps;
    u16 pad;
} xm_FadeChannel;
#endif
#ifndef XM_T_xm_FieldWork
#define XM_T_xm_FieldWork
typedef struct xm_FieldWork {
    s16 unk2078;               
    s16 unk207A;               
    s16 effect_steps;          
    u8 unk207E[2];
    s32 effect_value[6];       
    s32 effect_step[6];        
    s16 unk20B0[2];            
    void *effect_buffers[4];   
    xm_FadeChannel fades[2];      
    u16 open_windows;          

    s16 encounter_inhibition;  
    s16 terrain_angle;         
    u16 input_mask;            
    s32 unk217C;               
    s32 unk2180;               
    SVECTOR sprite_angles;     
    s16 scale;                 
    s16 sprite_gate;           
    u8 fog_color[4];           
    u8 far_color[4];           
    s16 fog_range[2];          
    u8 clear_color[3];         
    u8 party_bits;             
    s16 unk21A0[6];            
    s16 emitter_range;         
    s16 piece_drift[3];        
    s16 unk21B4;               
    u8 unk21B6[2];
    s32 last_sound_effect;     
    s32 unk21BC[3];            
    u8 unk21C8[0x21CC - 0x21C8];
    u8 party_processing_mode;  
    u8 camera_floor_fixed;     
    u8 preserve_nonplayer_motion; 
    u8 forced_position;        
    u8 script_control[2];      
    u8 piece_drift_mode;       
    u8 unk21D3;
    s16 unk21D4;               
    s16 text_speed;            
    s32 camera_counter;        
    s16 unk21DC[4];            
    s16 unk21E4[4];            
    SVECTOR layer_positions[4]; 
    s32 layer_scales[4];       
    s16 unk221C[5][3];         
    u8 unk223A[2];
    s16 unk223C[3][3];         
    u8 unk224E[0x225C - 0x224E];
    u8 unk225C[3];             
    u8 unk225F[0x2264 - 0x225F]; 
    s32 unk2264;               
    s32 unk2268;               
    s32 controlled;            
    u16 encounter_music[16];   
    s16 battle_music;          
    u8 unk2292[2];
    s32 unk2294;               
    s32 unk2298;               
    s32 unk229C;               
    u16 unk22A0[32];           
    s16 unk22E0;               
    s16 emitter_descriptor[3]; 
    s16 unk22E8[3][4];         
    s16 unk2300[3][4];         
    s32 unk2318[3];            
    s16 emitter_position[3][4]; 
    u16 effects_kept;          
    u16 unk233E;               
    s16 repeat_delay;          
    s16 repeat_remaining;      
    s16 jump_mode;             
    s16 animation_mode;        
    u16 unk2348;               
    s16 unk234A;               
    s16 gear_riding_lock_override; 
    s16 followers_idle;        
    u32 unk2350;               
    u8 unk2354;                
    u8 unk2355;                
    u8 unk2356;                
    u8 unk2357;                
    u8 unk2358;                
    u8 unk2359[0x235C - 0x2359];
} xm_FieldWork;
#endif
#ifndef XM_T_xm_SavedView
#define XM_T_xm_SavedView
typedef struct {
    s16 octant;
    s32 projection;
    s16 elevation;
} xm_SavedView;
#endif
#ifndef XM_T_xm_ModelLight
#define XM_T_xm_ModelLight
typedef struct {
    s32 vx, vy, vz;
    u16 r, g, b;
} xm_ModelLight;
#endif
#ifndef XM_T_xm_MorphDelta
#define XM_T_xm_MorphDelta
typedef struct {
    s16 dx, dy, dz;
    s16 index;
} xm_MorphDelta;
#endif
#ifndef XM_T_xm_MorphTarget
#define XM_T_xm_MorphTarget
typedef struct {
    s32 count;
    void *deltas;
    xm_MorphDelta *normals;
} xm_MorphTarget;
#endif
#ifndef XM_T_xm_MorphTable
#define XM_T_xm_MorphTable
typedef struct {
    s32 count;
    xm_MorphTarget targets[1];
} xm_MorphTable;
#endif
#ifndef XM_T_xm_SpriteModel
#define XM_T_xm_SpriteModel
typedef struct {
    u16 flags;          
    u16 vertex_count;
    u16 primitive_count;
    u16 group_count;    
    SVECTOR *vertices;
    SVECTOR *normals;
    u8 *unk10;          
    u8 *unk14;
    u8 *unk18;
    xm_MorphTable *morphs; 
    SVECTOR box_min;    
    SVECTOR box_max;
    s32 aux_size;       
    s32 packet_size;    
} xm_SpriteModel;
#endif
#ifndef XM_T_xm_MorphChannel
#define XM_T_xm_MorphChannel
typedef struct xm_MorphChannel {
    s32 (*update)(struct xm_MorphChannel *channel);
    s32 target;
    s32 weight;
    s32 step;
    u8 unk10[0x10];
} xm_MorphChannel;
#endif
#ifndef XM_T_xm_MorphState
#define XM_T_xm_MorphState
typedef struct {
    xm_SpriteModel *model;
    SVECTOR *vertices;
    SVECTOR *normals;
    s32 count;
    xm_MorphChannel *channels;
} xm_MorphState;
#endif
#ifndef XM_T_xm_FieldInstance
#define XM_T_xm_FieldInstance
typedef struct {
    u8 unk00[4];
    xm_SpriteModel *mesh; 
    void *packets[2]; 
    u8 unk10[2];
    s16 mode;        
    xm_MorphState *anims; 
    s16 center[3];   
    s16 unk1E;
    s16 radius;      
} xm_FieldInstance;
#endif
#ifndef XM_T_xm_FieldMarker
#define XM_T_xm_FieldMarker
typedef struct xm_FieldMarker {
    SVECTOR v[4];
    POLY_FT4 poly[2];
} xm_FieldMarker;
#endif
#ifndef XM_T_xm_FieldDescriptor
#define XM_T_xm_FieldDescriptor
typedef struct xm_FieldDescriptor {
    xm_FieldInstance *instance; 
    xm_Sprite *model;           
    struct xm_FieldMarker *shadow; 
    MATRIX matrix;           
    MATRIX transform;        
    xm_FieldActor *actor;       
    SVECTOR rotation;        
    u16 flags;               
    u16 unk5A;               
} xm_FieldDescriptor;
#endif
#ifndef XM_T_xm_Attribute
#define XM_T_xm_Attribute
typedef union {
    u32 word;
    u8 bytes[4];
} xm_Attribute;
#endif
#ifndef XM_T_xm_CollisionTriangle
#define XM_T_xm_CollisionTriangle
typedef struct {
    s16 unk00[6];
    u8 attribute;
    u8 unk0D;
} xm_CollisionTriangle;
#endif
#ifndef XM_T_xm_FieldComponents
#define XM_T_xm_FieldComponents
typedef struct {
    s32 descriptor_count;                      
    xm_FieldDescriptor *descriptors;              
    s32 *geometry;                             
    s32 *collision;                            
    s32 *sprites;                              
    xm_Attribute *collision_attributes;           
    xm_CollisionTriangle *collision_triangles[4]; 
    void *collision_vertices[4];               
    s32 triangle_counts[4];                    
    s16 layer_count;                           
} xm_FieldComponents;
#endif
#ifndef XM_T_xm_FieldView
#define XM_T_xm_FieldView
typedef struct xm_FieldView {
    VECTOR eye;              
    VECTOR target;           
    VECTOR up;               
    VECTOR eye_goal;         
    VECTOR target_goal;      
    VECTOR unk050;           
    VECTOR shake_offset;     
    VECTOR saved_target;     
    VECTOR point_actor_a;    
    VECTOR saved_eye;        
    VECTOR point_actor_b;    
    s32 scripted_zoom;       
    s16 mode;                
    s16 scripted_elevation;  
    s16 scripted_heading;    
    s16 scripted_scale;      
    u16 scripted;            
    s16 target_steps;        
    VECTOR scripted_target;  
    VECTOR target_step;      
    s16 eye_steps;           
    s32 scripted_eye[3];     
    s32 unk0F0;              
    s32 eye_step[3];         
    s32 unk100;              
    s32 target_a;            
    s32 target_b;            
    s16 angle;               
    s16 view_angle;          
    MATRIX previous_view;    
    MATRIX orbit;            
    SVECTOR orbit_angles;    
    u32 flags;               
    s16 bounds[4];           
    SVECTOR heading_angles;  
    s32 heading_velocity;    
    s32 heading_high;        
    u8 heading_blocks[2];    
    s16 heading_steps;       
    s32 projection;          
    u16 elevation;           
    s16 distance;            
    s16 elevation_steps;     
    s32 elevation_value;     
    s32 elevation_step;      
    u32 heading;             
    s16 projection_steps;    
    s32 projection_value;    
    s32 projection_step;     
    s16 steps;               
    s32 start;               
    s32 step;                
    s16 shake;               
    s16 shake_time;          
    s16 shake_stop;          
    s32 shake_amplitude[3];  
    s32 shake_step[3];       
    xm_SavedView saved_view;    
    SVECTOR world_angles;    
    SVECTOR anchor;          
    MATRIX scaled_world;     
    MATRIX unk204;           
    MATRIX world_matrix;     
    s32 scale;               
    xm_ModelLight lights[3];    
    s16 back_color[3];       
    u8 unk28A[2];
    xm_FieldComponents components; 
} xm_FieldView;
#endif
#ifndef XM_T_WHOLE
#define XM_T_WHOLE
#define WHOLE(value) (((xm_Fixed *)&(value))->part.whole)
#endif
#ifndef XM_S_field_event_walk_player
#define XM_S_field_event_walk_player
s32 xm_field_event_walk_player(s32 angle, s32 mode, s32 x, s32 z) __asm__("func_80092894");
#endif
#ifndef XM_S_field_work
#define XM_S_field_work
extern xm_FieldWork xm_field_work __asm__("g_FieldEffects");
#endif
#ifndef XM_S_field_view
#define XM_S_field_view
extern xm_FieldView xm_field_view __asm__("g_CameraEye");
#endif
#ifndef XM_S_field_compute_abs_via_gte
#define XM_S_field_compute_abs_via_gte
s32 xm_field_compute_abs_via_gte(s32 x) __asm__("FieldGetVec1Magnitude");
#endif
#ifndef XM_S_field_battle_not_requested
#define XM_S_field_battle_not_requested
extern s32 xm_field_battle_not_requested __asm__("D_800ADBDC");
#endif
#ifndef XM_S_field_worldmap_exit_not_requested
#define XM_S_field_worldmap_exit_not_requested
extern s32 xm_field_worldmap_exit_not_requested __asm__("D_800ADBE4");
#endif
#ifndef XM_S_field_event_yield_requested
#define XM_S_field_event_yield_requested
extern s32 xm_field_event_yield_requested __asm__("D_800B00C0");
#endif
#ifndef XM_S_field_map_change_not_requested
#define XM_S_field_map_change_not_requested
extern s32 xm_field_map_change_not_requested __asm__("D_800ADBEC");
#endif
#ifndef XM_S_field_event_record_departure
#define XM_S_field_event_record_departure
void xm_field_event_record_departure(void) __asm__("func_80092F44");
#endif
#ifndef XM_S_field_event_write_variable
#define XM_S_field_event_write_variable
void xm_field_event_write_variable(s32 reference, s32 value) __asm__("FieldScriptMemoryWriteU16");
#endif
#ifndef XM_S_mode_field_map_id
#define XM_S_mode_field_map_id
extern s32 xm_mode_field_map_id __asm__("g_GameSceneMapNum");
#endif
#ifndef XM_S_field_current_event_descriptor
#define XM_S_field_current_event_descriptor
extern xm_FieldDescriptor *xm_field_current_event_descriptor __asm__("D_800B06B8");
#endif
#ifndef XM_S_gpu_get_cos
#define XM_S_gpu_get_cos
s32 xm_gpu_get_cos(s32 angle) __asm__("rsin");
#endif
#ifndef XM_S_gpu_get_sin
#define XM_S_gpu_get_sin
s32 xm_gpu_get_sin(s32 angle) __asm__("rcos");
#endif
#ifndef XM_S_field_compute_planar_length
#define XM_S_field_compute_planar_length
s32 xm_field_compute_planar_length(s32 dx, s32 dz) __asm__("FieldGetVec2Magnitude");
#endif
#ifndef XM_S_field_actor_start_animation
#define XM_S_field_actor_start_animation
void xm_field_actor_start_animation(void *model, s32 animation, xm_FieldDescriptor *descriptor) __asm__("func_800821F4");
#endif
#ifndef XM_S_field_compute_xz_heading
#define XM_S_field_compute_xz_heading
s32 xm_field_compute_xz_heading(VECTOR *v) __asm__("func_8007B694");
#endif
s32 func_80092894(s32 angle, s32 mode, s32 x, s32 z) {
    xm_FieldActor *player;
    xm_Sprite *model;
    s32 from_x;
    s32 from_z;
    s32 reach;
    s32 value;
    s32 field;
    s32 direction;
    s32 dx;
    s32 dz;
    s32 goal_x;
    s32 goal_z;
    VECTOR delta;

    xm_field_work.encounter_inhibition = -1;
    player = xm_field_view.components.descriptors[xm_field_work.controlled].actor;
    model = xm_field_view.components.descriptors[xm_field_work.controlled].model;
    player->layer_flags |= 0x38;
    model->speed = 0x80000;
    reach = xm_field_compute_abs_via_gte(8) * 2;
    from_x = WHOLE(player->position[0]);
    from_z = WHOLE(player->position[2]);
    if (mode == 0) {
        if (xm_field_battle_not_requested == 0 || xm_field_worldmap_exit_not_requested == 0) {
            xm_field_event_yield_requested = 1;
        }
        if (xm_field_map_change_not_requested != 0) {
            value = xm_field_event_read_imm_or_var(4);
            field = xm_field_event_read_imm_or_var(2);
            xm_field_event_record_departure();
            xm_field_map_change_not_requested = 0;
            xm_field_event_write_variable(2, value);
            xm_mode_field_map_id = field;
        }
        direction = xm_field_current_event_descriptor->rotation.vy + angle - 0x400;
        goal_x = xm_field_current_event_actor->unk60 + WHOLE(xm_field_current_event_actor->position[0]) + ((xm_gpu_get_cos(direction) * 40) >> 12);
        goal_z = xm_field_current_event_actor->unk64 + WHOLE(xm_field_current_event_actor->position[2]) + (-(xm_gpu_get_sin(direction) * 40) >> 12);
    } else {
        goal_x = x;
        goal_z = z;
    }
    dx = goal_x - from_x;
    dz = goal_z - from_z;
    delta.vx = dx;
    delta.vy = 0;
    delta.vz = dz;
    if (reach < xm_field_compute_planar_length(dx, dz)) {
        goto walk;
    }
arrive:
    player->heading_goal = player->heading = player->heading_goal | 0x8000;
    model->speed = 0;
    player->unkE8 = 0;
    xm_field_actor_start_animation(model, 0, &xm_field_view.components.descriptors[xm_field_work.controlled]);
    xm_field_event_yield_requested = 1;
    player->slots[player->slot].value = 0xFFFF;
    player->slots[player->slot].move_mode = 0;
    player->flags &= ~0x200000;
    if (mode == 1) {
        player->layer_flags &= ~0x38;
    }
    player->stuck = 0;
    xm_field_current_event_actor->pc += 6;
    return 0;
walk:
    if (player->last_position[0] == WHOLE(player->position[0]) &&
        player->last_position[1] == WHOLE(player->position[1]) &&
        player->last_position[2] == WHOLE(player->position[2])) {
        player->stuck++;
    } else {
        player->stuck = 0;
    }
    if ((s16)player->stuck > 0x40) {
        goto arrive;
    }
    player->heading_goal = player->heading = xm_field_compute_xz_heading(&delta);
    xm_field_event_yield_requested = 1;
    if (mode != 0) {
        xm_field_current_event_actor->pc -= 1;
    }
    return -1;
}
#endif
#else
s32 func_80092894(s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    s32 vec[3];
    s32 targetX, targetZ;
    s32 angle;
    s32 dist;
    s32 threshold;
    s16 savedX, savedZ;
    u8* pActorData;
    u8* pSpriteData;
    u8* cur;
    u8* slot;
    u32 v;

    g_FieldControl.isRandomEncountersEnabled = -1;

    pActorData = (u8*)(uintptr_t)g_FieldActors[g_PlayerActorIndex].pActorData;
    pSpriteData = (u8*)(uintptr_t)g_FieldActors[g_PlayerActorIndex].pSpriteData;

    *(u32*)(pActorData + 0x4) |= 0x38;
    *(u32*)(pSpriteData + 0x18) = 0x80000;

    threshold = (s32)((u32)FieldGetVec1Magnitude(8) << 1);
    savedX = *(s16*)(pActorData + 0x22);
    savedZ = *(s16*)(pActorData + 0x2A);

    /* arg2 != 0: walk straight to (arg3, arg4); no exit-point or angle math. */
    targetX = arg3;
    targetZ = arg4;
    if (arg2 == 0) {
        if (D_800ADBDC == 0 || D_800ADBE4 == 0) {
            D_800B00C0 = 1;
        }
        if (D_800ADBEC != 0) {
            s32 entrance = FieldScriptVMGetArgument(4);
            s32 mapNum = FieldScriptVMGetArgument(2);
            func_80092F44();
            D_800ADBEC = 0;
            FieldScriptMemoryWriteU16(2, entrance);
            g_GameSceneMapNum = mapNum;
        }

        /* Target is 40 units ahead of the script's current actor (not the
         * player), along D_800B06B8's facing + arg1 - 0x400. */
        angle = (s32)((u32)*(s16*)((u8*)D_800B06B8 + 0x52) + (u32)arg1 - 0x400u);
        v = (u32)rsin(angle);
        cur = (u8*)g_FieldScriptVMCurActor;
        targetX = (s32)((u32)(*(s16*)(cur + 0x60) + *(s16*)(cur + 0x22)) + (u32)((s32)(v * 40u) >> 12));
        v = (u32)rcos(angle);
        cur = (u8*)g_FieldScriptVMCurActor;
        targetZ = (s32)((u32)(*(s16*)(cur + 0x64) + *(s16*)(cur + 0x2A)) + (u32)((s32)(0u - v * 40u) >> 12));
    }

    /* func_8007B694 reads the retail 3-word stack vector {dx, 0, dz}. */
    vec[0] = (s32)((u32)targetX - (u32)savedX);
    vec[1] = 0;
    vec[2] = (s32)((u32)targetZ - (u32)savedZ);
    dist = (s32)FieldGetVec2Magnitude(vec[0], vec[2]);

    if (threshold < dist) {
        /* Player hasn't reached target yet */
        if (*(s16*)(pActorData + 0x68) == *(s16*)(pActorData + 0x22) &&
            *(s16*)(pActorData + 0x6A) == *(s16*)(pActorData + 0x26) &&
            *(s16*)(pActorData + 0x6C) == *(s16*)(pActorData + 0x2A)) {
            *(u16*)(pActorData + 0x6E) = *(u16*)(pActorData + 0x6E) + 1;
        } else {
            *(u16*)(pActorData + 0x6E) = 0;
        }
        if (*(s16*)(pActorData + 0x6E) < 0x41) {
            s32 heading = func_8007B694(vec);
            *(u16*)(pActorData + 0x104) = (u16)heading;
            *(u16*)(pActorData + 0x106) = (u16)heading;
            D_800B00C0 = 1;
            if (arg2 != 0) {
                g_FieldScriptVMCurActor->scriptInstructionPointer--;
            }
            return -1;
        }
    }

    /* Player reached target (or got stuck) — finalize transition */
    {
        u16 flags = *(u16*)(pActorData + 0x106) | 0x8000;
        *(u16*)(pActorData + 0x104) = flags;
        *(u16*)(pActorData + 0x106) = flags;
    }
    *(u32*)(pSpriteData + 0x18) = 0;
    *(s16*)(pActorData + 0xE8) = 0;
    func_800821F4(pSpriteData, 0, &g_FieldActors[g_PlayerActorIndex]);
    D_800B00C0 = 1;
    *(u16*)(pActorData + 0x90 + *(u8*)(pActorData + 0xCE) * 8) = 0xFFFF;
    /* retail: lw/and/sw of the same slot after the sh; byte copies keep the
     * host compiler from reordering the u16 store and the u32 load. */
    slot = pActorData + 0x90 + *(u8*)(pActorData + 0xCE) * 8;
    __builtin_memcpy(&v, slot, 4);
    v &= 0xFE7FFFFF;
    __builtin_memcpy(slot, &v, 4);
    *(u32*)(pActorData + 0x0) &= 0xFFDFFFFF;
    if (arg2 == 1) {
        *(u32*)(pActorData + 0x4) &= ~0x38u;
    }
    *(s16*)(pActorData + 0x6E) = 0;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 6;
    return 0;
}
#endif /* XENO_PC_PORT */

extern s32 D_800B2350;
extern s32 D_800ADBDC;
extern s32 D_800ADBE4;
extern s32 D_800ADB2C;
extern s32 D_8004F308;
extern s32 D_800ADB90;

#ifndef XENO_PC_PORT
/* func_80092C20: D_800B2350 is not gp-small in retail (its address lives in s0
 * across the calls), so it is read/written through an incomplete-array alias;
 * func_800A0C4C/func_80092894 are only declared after this site. */
extern s32 D_800B2350__a[] asm("D_800B2350");
extern void func_800A0C4C(void);
extern s32 func_80092894(s32, s32, s32, s32);

#endif
void func_80092C20(void) {
#ifdef XENO_PC_PORT
    s32 arg1, arg2;
    s32 result;

    if (D_800ADBDC == 0 || D_800ADBE4 == 0 || D_800ADB2C != 0 ||
        D_8004F308 == -1 || D_800ADB90 != 0) {
        D_800B00C0 = 1;
        g_FieldScriptVMCurActor->scriptInstructionPointer--;
        return;
    }

    arg1 = FieldScriptArgument1(1, SCRIPT_READ_U8_REL(5));
    arg2 = FieldScriptArgument2(3, SCRIPT_READ_U8_REL(5));

    if (D_800B2350 == 0) {
        FieldActor* pPlayer = &g_FieldActors[g_PlayerActorIndex];
        D_800B2350 = *(s32*)(void*)(uintptr_t)pPlayer->pActorData;
    }

    func_800A0C4C();
    result = func_80092894(0, 1, arg1, arg2);
    if (result != 0) {
        return;
    }

    if (!(D_800B2350 & 0x80)) {
        FieldActor* pPlayer = &g_FieldActors[g_PlayerActorIndex];
        u32* pData = (u32*)(void*)(uintptr_t)pPlayer->pActorData;
        *pData = *pData & ~0x80u;
    }
    D_800B2350 = 0;
#else
    s32 arg1, arg2;

    if (D_800ADBDC == 0 || D_800ADBE4 == 0 || D_800ADB2C != 0 || D_8004F308 == -1 || D_800ADB90 != 0) {
        D_800B00C0 = 1;
        g_FieldScriptVMCurActor->scriptInstructionPointer--;
        return;
    }
    arg1 = FieldScriptArgument1(1, SCRIPT_READ_U8_REL(5));
    arg2 = FieldScriptArgument2(3, SCRIPT_READ_U8_REL(5));
    if (D_800B2350__a[0] == 0) {
        D_800B2350__a[0] = *(s32*)(uintptr_t)g_FieldActors[g_PlayerActorIndex].pActorData;
    }
    func_800A0C4C();
    if (func_80092894(0, 1, arg1, arg2) == 0) {
        if (!(D_800B2350__a[0] & 0x80)) {
            *(u32*)(uintptr_t)g_FieldActors[g_PlayerActorIndex].pActorData &= ~0x80;
        }
        D_800B2350__a[0] = 0;
    }
#endif
}

extern s32 D_800ADBDC;
extern s32 D_800ADBE4;
extern s32 D_800ADB2C;
extern s32 D_8004F308;
extern s32 D_800ADB90;
extern void func_800A0C4C(void);
extern s32 func_80092894(s32, s32, s32, s32);

void func_80092DFC(void) {
    if (D_800ADBDC == 0 || D_800ADBE4 == 0 || D_800ADB2C != 0 ||
        D_8004F308 == -1 || D_800ADB90 != 0) {
        D_800B00C0 = 1;
    } else {
        func_800A0C4C();
        func_80092894(0, 0, 0, 0);
    }
}

void func_80092EA0(void) {
    if (D_800ADBDC == 0 || D_800ADBE4 == 0 || D_800ADB2C != 0 ||
        D_8004F308 == -1 || D_800ADB90 != 0) {
        D_800B00C0 = 1;
    } else {
        func_800A0C4C();
        func_80092894(0x3E0, 0, 0, 0);
    }
}

extern s32 g_GameSceneMapNum;

void func_80092F44(void) {
    FieldScriptMemoryWriteU16(4, g_GameSceneMapNum & 0x3FFF);
    FieldScriptMemoryWriteU16(6, FieldGetPlayerActorDirection() & 0xFFFF);
    FieldScriptMemoryWriteU16(8, FieldGetCameraDirection() & 0xFFFF);
    FieldScriptMemoryWriteU16(0x12, (s16)(FieldScriptVMGetVariableValue(0x12) + 1));
}

extern s32 D_800ADBD8;
extern s32 D_800B0064;

void func_80092FB4(void) {
    if (D_800ADBD8 != 0) {
        g_FieldControl.isRandomEncountersEnabled = -1;
        D_800ADBD8 = 0;
        D_800B0064 = FieldScriptVMGetArgument(1);
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

extern u8 D_800B02C8;
extern void func_800A31E8(void);
extern int FieldScriptArgument1(int index, int mask);
extern int FieldScriptArgument2(int index, int mask);
extern int FieldScriptArgument3(int index, int mask);
extern int FieldScriptArgument4(int index, int mask);
void func_800931F8(void);

#ifdef XENO_PC_PORT
extern u32 g_PcPortOpcode56Func8009FEE4ConditionCount;
extern void PcPort_FieldOpcode56TransitionIntercept(
    s32 readinessBefore, u32 refreshConditionCountBefore,
    s32 opcodeActor, u16 opcodeIp);
extern void PcPort_FieldOpcode56RecordControlLock(
    s16 controlBefore, s32 actor, u16 lockIp);

/* VM opcode 0x56 -- snapshot the outgoing field and arm the four-halfword
 * field/world-map transition tuple.  Retail 0x80093014-0x800931F4.
 *
 * This is deliberately separate from the port harness's transition hold. The
 * body below performs every retail write, including clearing D_800ADBE4. The
 * port harness observes retail's complete FE54 -> 0x56 departure sequence:
 * FE54 records the pre-lock control value, and this body's final hook logs the
 * fully armed state before restoring anything. With
 * XENO_FIELD_HOLD_TRANSITION=1, the hook restores D_800ADBE4 to prevent
 * FieldMain teardown into the still-stubbed func_8007954C exit-1 arm and, only
 * after an actor/IP-adjacent FE54 snapshot, restores the pre-lock control value
 * so the artificially held field remains playable. A real F15 departure keeps
 * retail's deliberate lock -> arm -> teardown choreography. */
void func_80093014(void) {
    s32 readinessBefore;
    u32 refreshConditionCountBefore;
    s32 opcodeActor;
    u16 opcodeIp;
    s32 arg3;
    s32 heading;
    s32 mask;

    if (D_800ADBDC == 0 || D_800ADBE4 == 0 || D_800ADB2C != 0 ||
        D_8004F308 == -1 || D_800ADB90 != 0) {
        D_800B00C0 = 1;
        return;
    }

    readinessBefore = D_800ADBE4;
    opcodeActor = D_800AFD1C;
    opcodeIp = g_FieldScriptVMCurActor->scriptInstructionPointer;
    refreshConditionCountBefore =
        g_PcPortOpcode56Func8009FEE4ConditionCount;

    func_800A31E8();
    g_FieldControl.isRandomEncountersEnabled = -1;
    D_800ADBE4 = 0;

    mask = SCRIPT_READ_U8_REL(9);
    *(s16*)((u8*)g_pGameState + 0x231A) =
        (s16)FieldScriptArgument1(1, mask);

    /* Retail stores this halfword but no reader is currently identified. */
    *(s16*)((u8*)g_pGameState + 0x231E) =
        (s16)FieldScriptArgument2(3, mask);

    arg3 = FieldScriptArgument3(5, mask);
    if (arg3 == -1 || (u16)arg3 == 0xFFFF) {
        heading = *(u16*)((u8*)&g_Scene + 0x56) + 0x800;
    } else {
        heading = arg3 + 0x800;
    }
    *(s16*)((u8*)g_pGameState + 0x231C) = (s16)(heading & 0xFFF);

    /* This store is the retail jal func_800931F8 delay-slot side effect. */
    *(s16*)((u8*)g_pGameState + 0x2320) =
        (s16)FieldScriptArgument4(7, mask);
    func_800931F8();

    D_800B02C8 = 1;
    D_800B00C0 = 1;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 0xA;

    PcPort_FieldOpcode56TransitionIntercept(
        readinessBefore, refreshConditionCountBefore, opcodeActor, opcodeIp);
}
#else
/* func_80093014: (arg3 & 0xFFFF) == 0xFFFF | arg3 == -1 is a non-short-circuit
 * |, the 0x231C store is written in both arms (cross-jumped tail, g_pGameState
 * load stays per arm) through the raw form, the 0x231A/0x231E stores are pointer
 * indexed, and the IP update is raw-read/struct-store before D_800B00C0 = 1.
 * This retail definition also replaces the former NON_MATCHING draft. */
void func_80093014(void) {
    s32 arg3;

    if (D_800ADBDC == 0 || D_800ADBE4 == 0 || D_800ADB2C != 0 || D_8004F308 == -1 || D_800ADB90 != 0) {
        D_800B00C0 = 1;
        return;
    }
    func_800A31E8();
    g_FieldControl.isRandomEncountersEnabled = -1;
    D_800ADBE4 = 0;
    ((s16*)g_pGameState)[0x231A / 2] = FieldScriptArgument1(1, SCRIPT_READ_U8_REL(9));
    ((s16*)g_pGameState)[0x231E / 2] = FieldScriptArgument2(3, SCRIPT_READ_U8_REL(9));
    arg3 = FieldScriptArgument3(5, SCRIPT_READ_U8_REL(9));
    if (((arg3 & 0xFFFF) == 0xFFFF) | (arg3 == -1)) {
        *(s16*)((u8*)g_pGameState + 0x231C) = (*(u16*)((u8*)&g_Scene + 0x56) + 0x800) & 0xFFF;
    } else {
        *(s16*)((u8*)g_pGameState + 0x231C) = (arg3 + 0x800) & 0xFFF;
    }
    ((s16*)g_pGameState)[0x2320 / 2] = FieldScriptArgument4(7, SCRIPT_READ_U8_REL(9));
    func_800931F8();
    D_800B02C8 = 1;
    g_FieldScriptVMCurActor->scriptInstructionPointer = *(u16*)((u8*)g_FieldScriptVMCurActor + 0xCC) + 0xA;
    D_800B00C0 = 1;
}

#endif

void func_800931F8(void) {
}

extern s32 D_800B0048;
extern s32 D_800AFD14;
void func_800932D0(void);

void func_80093200(void) {
    if (D_800ADBDC == 0 || D_800ADBE4 == 0 || D_800ADB2C != 0 ||
        D_8004F308 == -1 || D_800ADB90 != 0) {
        D_800B00C0 = 1;
    } else {
        func_800932D0();
        func_800931F8();
        D_800B0048 = FieldScriptVMGetArgument(0);
        D_800AFD14 = FieldScriptVMGetArgument(2);
        g_FieldScriptVMCurActor->scriptInstructionPointer += 4;
    }
}

extern s32 D_800ADB70;
extern s32 D_800ADBEC;
extern s32 g_GameSceneMapNum;

// VM opcode 152, CHANGE_FIELD: request a map transition to arg(1)/entrance
// arg(3). Gated on six readiness flags (archive/CD-load and camera-cut state);
// while any is unsatisfied the instruction does not advance and the VM yields
// to retry it next frame. Once clear, it snapshots the outgoing scene via
// func_80092F44, clears the pending-load flag, writes the entrance into VM
// variable slot 2 and the new map number, then kicks off the load
// (func_800931F8 is a genuine empty function in retail).
void func_800932D0(void) {
    s32 entranceArg;
    s32 mapArg;

    if (D_800ADBDC == 0 || D_800ADBE4 == 0 || D_800ADB2C != 0 ||
        D_8004F308 == -1 || D_800ADB90 != 0 || D_800ADB70 != 0) {
        D_800B00C0 = 1;
        return;
    }

    g_FieldControl.isRandomEncountersEnabled = -1;

    if (D_800ADBEC != 0) {
        entranceArg = FieldScriptVMGetArgument(3);
        mapArg = FieldScriptVMGetArgument(1);
#ifdef XENO_PC_PORT
        { extern void PcPort_RouteLogChangeField(int, int);
          PcPort_RouteLogChangeField(mapArg, entranceArg); } /* inert unless XENO_ROUTE_LOG */
#endif
        func_80092F44();
        D_800ADBEC = 0;
        FieldScriptMemoryWriteU16(2, entranceArg);
        g_GameSceneMapNum = mapArg;
        func_800931F8();
    }

    D_800B00C0 = 1;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
}

extern s32 D_800ADB18;
extern s32 D_800ADBE0;
extern s32 D_800ADB88;
extern u8 D_8005954C;
extern u8 D_80059508;
extern u8 D_800594F8;
extern u8 D_800B2356;

void func_800933F8(void) {
    if (D_800ADBDC == 0 || D_800ADBE4 == 0 || D_800ADBEC == 0 ||
        D_800ADB2C != 0 || D_8004F308 == -1 || D_800ADB90 != 0) {
        D_800B00C0 = 1;
        g_FieldScriptVMCurActor->scriptInstructionPointer--;
        return;
    }
    D_8005954C = D_800B2356;
    D_80059508 = (u8)FieldScriptVMGetArgument(1);
    D_800594F8 = 0;
    D_800ADBDC = 0;
    D_800ADBE0 = 0;
    D_800ADB88 = 1;
    {
        s32 mapNum = FieldScriptVMGetArgument(5);
        if (mapNum != 0x7FFF) {
            s32 entrance = FieldScriptVMGetArgument(7);
#ifdef XENO_PC_PORT
            { extern void PcPort_RouteLogChangeField(int, int);
              PcPort_RouteLogChangeField(mapNum, entrance); } /* inert unless XENO_ROUTE_LOG */
#endif
            func_80092F44();
            FieldScriptMemoryWriteU16(2, entrance);
            g_GameSceneMapNum = mapNum;
            D_800ADB18 = 1;
        }
    }
    D_800B00C0 = 1;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 9;
}

extern u8 D_800B2356;
extern u8 D_8005954C;
extern u8 D_80059508;
extern u8 D_800594F8;
extern s32 D_800ADBE0;
extern s32 D_800ADB88;

void func_80093568(void) {
    if (D_800ADBDC == 0 || D_800ADBE4 == 0 || D_800ADBEC == 0 ||
        D_800ADB2C != 0 || D_8004F308 == -1 || D_800ADB90 != 0) {
        D_800B00C0 = 1;
    } else {
        D_8005954C = D_800B2356;
        D_80059508 = (u8)FieldScriptVMGetArgument(1);
        D_800594F8 = 0;
        D_800ADBDC = 0;
        D_800ADBE0 = 0;
        D_800ADB88 = 1;
        D_800B00C0 = 1;
        g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
    }
}

void func_80093664(void) {
    u8 b1 = SCRIPT_READ_U8_REL(1);
    u8 b2 = SCRIPT_READ_U8_REL(2);
    s32 result = func_80092424(b1, b2);
    u16 addr = (u16)FieldScriptVMGetInstructionArgument(3);
    FieldScriptMemoryWriteU16(addr, result);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
}

extern s32 D_8004F350;

void func_800936E4(void) {
    if (D_8004F350 == 0) {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 1;
    } else {
        g_FieldScriptVMCurActor->scriptInstructionPointer -= 1;
    }
    D_800B00C0 = 1;
}

extern s32 D_8004F350;
extern s8 D_80059171;
extern s32 D_800ADB64;
extern u16 D_800B236C;

void func_80093740(void) {
    D_800B00C0 = 1;
    D_800ADB64 = 0;
    D_80059171 = D_800B236C;
    D_8004F350++;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 1;
}

void func_80093790(void) {
    D_80059171 = 1;
    D_800ADB64 = 6;
    D_800B00C0 = 1;
    D_8004F350++;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 1;
}

/* FE57: request the title menu (MenuExecute slot 2 -> func_801C62A8 ->
 * func_801C58EC).  FieldMain's opener func_800799D4 consumes D_800ADB64. */
void func_800937E0(void) {
#ifdef TITLE_CHAIN_MUTANT_FE57_WRONG_MENU
    /* Deliberate mutant (pc_port/tests/run_title_newgame_chain.sh): request
     * the field system menu instead of the title menu. */
    D_800ADB64 = 0;
#else
    D_800ADB64 = 2;
#endif
    D_800B00C0 = 1;
    D_8004F350++;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 1;
}

void func_80093824(void) {
    D_80059171 = FieldScriptVMGetArgument(1);
    D_800ADB64 = 0x3;
    D_800B00C0 = 1;
    D_8004F350++;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void func_80093888(void) {
    s32 arg3, arg1;
    g_FieldControl.isRandomEncountersEnabled = -1;
    arg3 = FieldScriptVMGetArgument(3);
    arg1 = FieldScriptVMGetArgument(1);
    func_80092F44();
    FieldScriptMemoryWriteU16(2, arg3);
    g_GameSceneMapNum = arg1;
    func_800931F8();
    D_800ADB64 = 1;
    D_800B00C0 = 1;
    D_8004F350++;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
}

extern s32 D_800ADB64;
extern s32 D_800C3A6A;
extern s32 D_8004F350;

#ifndef XENO_PC_PORT
/* func_80093930: the actor pointer is taken into a local before the two raw
 * g_pGameState stores (which keep the D_800C3A6A store behind them); D_800C3A6A
 * is a halfword in retail (sh) though the TU declares it s32. */
#endif
void func_80093930(void) {
#ifdef XENO_PC_PORT
    s32 arg1 = FieldScriptVMGetArgument(1);
    D_800ADB64 = 1;
    D_800B00C0 = 1;
    *(s16*)((u8*)g_pGameState + 0x1932) = (s16)arg1;
    *(s16*)((u8*)g_pGameState + 0x2320) = (s16)arg1;
    D_800C3A6A = (s16)arg1;
    D_8004F350++;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
#else
    s32 arg1 = FieldScriptVMGetArgument(1);
    ActorData* actor;
    D_800ADB64 = 1;
    D_800B00C0 = 1;
    actor = g_FieldScriptVMCurActor;
    *(s16*)((u8*)g_pGameState + 0x1932) = arg1;
    *(s16*)((u8*)g_pGameState + 0x2320) = arg1;
    *(s16*)&D_800C3A6A = arg1;
    D_8004F350++;
    actor->scriptInstructionPointer += 3;
#endif
}

void func_800939A0(void) {
    D_80059171 = FieldScriptVMGetArgument(1);
    D_800ADB64 = 0x4;
    D_800B00C0 = 1;
    D_8004F350++;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void func_80093A04(void) {
    D_80059171 = FieldScriptVMGetArgument(1);
    D_800ADB64 = 0x5;
    D_800B00C0 = 1;
    D_8004F350++;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}


// Random encounter stuff

// These are likely part of a struct
extern s8 D_800B21D0[];
extern s8 D_800B21D1[];

void func_80093A68(void) {
    g_Scene.unk48 &= 0x7FFF;
    g_FieldScriptVMCurActor->scriptInstructionPointer++;
}

void func_80093A98(void) {
    g_Scene.unk48 |= 0x8000;
    g_FieldScriptVMCurActor->scriptInstructionPointer++;
}

void func_80093AC8(void) {
    g_FieldControl.isRandomEncountersEnabled = 0;
    D_800B21D0[0] = 0;
    D_800B21D1[0] = 0;
    g_Scene.unk48 &= 0x3FFF;
    g_FieldScriptVMCurActor->scriptInstructionPointer++;
}

void func_80093B10(void) {
#ifdef XENO_PC_PORT
    /* FE54 is retail's departure input lock. Record its prior value without
     * predicting what follows; opcode 0x56 consumes it only when actor and IP
     * prove this was the immediately preceding instruction. */
    PcPort_FieldOpcode56RecordControlLock(
        g_FieldControl.isRandomEncountersEnabled,
        D_800AFD1C,
        g_FieldScriptVMCurActor->scriptInstructionPointer);
#endif
    g_FieldControl.isRandomEncountersEnabled = -1;
    D_800B21D0[0] = 1;
    D_800B21D1[0] = 1;
    g_Scene.unk48 |= 0xC000;
    if ((D_800ADBDC == 0) || (D_800ADBE4 == 0)) {
        D_800B00C0 = 1; // Stop script VM execution?
        g_FieldScriptVMCurActor->scriptInstructionPointer--;
        return;
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer++;
}

void func_80093BB0(void) {
    D_800B21D0[0] = 0x0;
    g_FieldScriptVMCurActor->scriptInstructionPointer++;
}

void func_80093BD4(void) {
    D_800B21D0[0] = 0x1;
    g_FieldScriptVMCurActor->scriptInstructionPointer++;
}

void FieldScriptEnableCompass(void) {
    D_800B21D1[0] = 0x0;
    g_FieldScriptVMCurActor->scriptInstructionPointer++;
}

void FieldScriptDisableCompass(void) {
    D_800B21D1[0] = 0x1;
    g_FieldScriptVMCurActor->scriptInstructionPointer++;
}

void FieldScriptVMHandlerDisableRandomEncounters(void) {
    g_FieldControl.isRandomEncountersEnabled = 0;
    g_FieldScriptVMCurActor->scriptInstructionPointer++;
}

void func_80093C6C(void) {
    if (D_800ADBDC == 0 || D_800ADBE4 == 0) {
        D_800B00C0 = 1;
        return;
    }
    g_FieldControl.isRandomEncountersEnabled = -1;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 1;
}
// End of random encounter stuff


// Write U8 Handler
// Arg1 + Arg3 = Offset to U8 value in script
// Arg2 = Write address
void func_80093CD0(void) {
    unsigned short offset;
    unsigned short arg1;

    arg1 = FieldScriptVMGetInstructionArgument(1);
    offset = arg1 + FieldScriptVMGetArgument(5);
    FieldScriptMemoryWriteU16(
        SCRIPT_IMM_ARG(2), 
        SCRIPT_READ_U8(offset)
    );
    g_FieldScriptVMCurActor->scriptInstructionPointer += 7;
}

// Write U16/S16 handler
// Arg1 + Arg3 = Offset in script to short to write
// Arg3: Write address
void func_80093D48(void) {
    int arg1;
    int nAddress;
    unsigned short nOffset;
    int nValue;
    int arg3;

    arg1 = FieldScriptVMGetInstructionArgument(1);
    nOffset = arg1 + FieldScriptVMGetArgument(5);
    if (SCRIPT_READ_U8_REL(7) == 0) {
        // W/O carry
        FieldScriptMemoryWriteU16(
            SCRIPT_IMM_ARG(2),
            SCRIPT_READ_U8(nOffset) | (SCRIPT_READ_U8(nOffset + 1) << 8)
        );
    } else {
        // With carry
        FieldScriptMemoryWriteU16(
            SCRIPT_IMM_ARG(2),
            (short)(SCRIPT_READ_U8(nOffset) + (SCRIPT_READ_U8(nOffset + 1) << 8))
        );
    }
    
    g_FieldScriptVMCurActor->scriptInstructionPointer += 8;
}

// The two functions seems to be related to handling room transitions
void func_80093E30(void) {
    if (!g_FieldScriptVMCurActor->scriptFlags.fields.scriptFlags_0x13) {
        if (!(g_FieldScriptVMCurActor->flags12C_0x5)) {
            g_FieldScriptVMCurActor->flags12C_0x5 = 1;
            g_FieldScriptVMCurActor->curDoorStep = 0;
            func_80085634(8, 3);
        } else {
            g_FieldScriptVMCurActor->curDoorStep++;
            if (g_FieldScriptVMCurActor->curDoorStep < 0x1F) {
                if (SCRIPT_READ_U8_REL(1) == 0) {
                    g_FieldActors[D_800AFD1C].rotation.y += 0x20;
                } else {
                    g_FieldActors[D_800AFD1C].rotation.y -= 0x20;
                }
            } else {
                g_FieldScriptVMCurActor->scriptFlags.fields.scriptFlags_0x13 = 0x1;
                g_FieldScriptVMCurActor->flags12C_0x5 = 0;
                g_FieldScriptVMCurActor->curDoorStep = 0;
                g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
            }
        }
    } else {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
    }
    
    func_80072254(D_800AFD1C);
}

void func_80093FC0(void) {
    if (g_FieldScriptVMCurActor->scriptFlags.fields.scriptFlags_0x13) {
        if (!(g_FieldScriptVMCurActor->flags12C_0x5)) {
            g_FieldScriptVMCurActor->flags12C_0x5 = 1;
            g_FieldScriptVMCurActor->curDoorStep = 0;
            func_80085634(8, 3);
        } else {
            g_FieldScriptVMCurActor->curDoorStep++;
            if (g_FieldScriptVMCurActor->curDoorStep < 0x1F) {
                if (SCRIPT_READ_U8_REL(1) == 0) {
                    g_FieldActors[D_800AFD1C].rotation.y -= 0x20;
                } else {
                    g_FieldActors[D_800AFD1C].rotation.y += 0x20;
                }
            } else {
                g_FieldScriptVMCurActor->scriptFlags.fields.scriptFlags_0x13 = 0;
                g_FieldScriptVMCurActor->flags12C_0x5 = 0;
                g_FieldScriptVMCurActor->curDoorStep = 0;
                g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
            }
        }
    } else {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
    }
    func_80072254(D_800AFD1C);
}

extern void func_80072254(int);
extern void func_80085634(int,int);
extern s32 D_800AFD1C;
extern FieldActor* D_800B06B8;

void func_80094158() {
    ActorData* pActor;
    int angle;
    int delta;

    if (!g_FieldScriptVMCurActor->scriptFlags.fields.scriptFlags_0x13) {
        if (!g_FieldScriptVMCurActor->flags12C_0x5) {
            g_FieldScriptVMCurActor->flags12C_0x5 = 0x1;
            g_FieldScriptVMCurActor->curDoorStep = 0;
            func_80085634(8, 3);
            g_FieldScriptVMCurActor->unkD0.vx = g_FieldScriptVMCurActor->position.vx;
            g_FieldScriptVMCurActor->unkD0.vy = g_FieldScriptVMCurActor->position.vy;
            g_FieldScriptVMCurActor->unkD0.vz = g_FieldScriptVMCurActor->position.vz;
        } else {
            g_FieldScriptVMCurActor->curDoorStep++;
            if (g_FieldScriptVMCurActor->curDoorStep < FieldScriptVMGetArgument(3)) {
                switch (FieldScriptVMGetArgument(5)) { 
                    case 0x1000:
                        g_FieldScriptVMCurActor->unkD0.vy -= FieldScriptVMGetArgument(1) * 0x10;
                        D_800B06B8->transformMatrix.t[1] = CONV_TO_GTE(g_FieldScriptVMCurActor->unkD0.vz);
                        break;
                    case 0x1001:
                        g_FieldScriptVMCurActor->unkD0.vy += FieldScriptVMGetArgument(1) * 0x10;
                        D_800B06B8->transformMatrix.t[1] = CONV_TO_GTE(g_FieldScriptVMCurActor->unkD0.vz);
                        break;
                    default:
                        delta = PSX_DEGREES(90);
                        angle = D_800B06B8->rotation.y + FieldScriptVMGetArgument(5) - delta;
                        g_FieldScriptVMCurActor->unkD0.vx += rsin(angle) * FieldScriptVMGetArgument(1);
                        g_FieldScriptVMCurActor->unkD0.vz -= rcos(angle) * FieldScriptVMGetArgument(1);
                        D_800B06B8->transformMatrix.t[0] = CONV_TO_GTE(g_FieldScriptVMCurActor->unkD0.vx);
                        D_800B06B8->transformMatrix.t[2] = CONV_TO_GTE(g_FieldScriptVMCurActor->unkD0.vz);
                        break;
                }
            } else {
                g_FieldScriptVMCurActor->scriptFlags.fields.scriptFlags_0x13 = 1;
                g_FieldScriptVMCurActor->flags12C_0x5 = 0;
                g_FieldScriptVMCurActor->curDoorStep = 0;   
                g_FieldScriptVMCurActor->scriptInstructionPointer += 7;
            }
        }
    } else {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 7;
    }
    func_80072254(D_800AFD1C);
}

void func_800943AC(void) {
    ActorData* temp_a1;
    int angle;
    int delta;
    
    if (g_FieldScriptVMCurActor->scriptFlags.fields.scriptFlags_0x13) {
        if (!g_FieldScriptVMCurActor->flags12C_0x5) {
            g_FieldScriptVMCurActor->flags12C_0x5 = 0x1;
            g_FieldScriptVMCurActor->curDoorStep = 0x0;
            func_80085634(8, 3);
        } else {
            g_FieldScriptVMCurActor->curDoorStep++;
            if (g_FieldScriptVMCurActor->curDoorStep < FieldScriptVMGetArgument(3)) {
                switch (FieldScriptVMGetArgument(5)) {
                    case 0x1000:
                        g_FieldScriptVMCurActor->unkD0.vy -= FieldScriptVMGetArgument(1) * 0x10;
                        D_800B06B8->transformMatrix.t[1] = CONV_TO_GTE(g_FieldScriptVMCurActor->unkD0.vz);
                        break;
                    case 0x1001:
                        g_FieldScriptVMCurActor->unkD0.vy += FieldScriptVMGetArgument(1) * 0x10;
                        D_800B06B8->transformMatrix.t[1] = CONV_TO_GTE(g_FieldScriptVMCurActor->unkD0.vz);
                        break;
                    default:
                        delta = PSX_DEGREES(90);
                        angle = D_800B06B8->rotation.y + FieldScriptVMGetArgument(5) - delta;
                        g_FieldScriptVMCurActor->unkD0.vx -= rsin(angle) * FieldScriptVMGetArgument(1);
                        g_FieldScriptVMCurActor->unkD0.vz += rcos(angle) * FieldScriptVMGetArgument(1);
                        D_800B06B8->transformMatrix.t[0] = CONV_TO_GTE(g_FieldScriptVMCurActor->unkD0.vx);
                        D_800B06B8->transformMatrix.t[2] = CONV_TO_GTE(g_FieldScriptVMCurActor->unkD0.vz);
                        break;
                }
            } else {
                g_FieldScriptVMCurActor->scriptFlags.fields.scriptFlags_0x13 = 0x0;
                g_FieldScriptVMCurActor->flags12C_0x5 = 0x0;
                g_FieldScriptVMCurActor->curDoorStep = 0;
                g_FieldScriptVMCurActor->scriptInstructionPointer += 7;
            }
        }
    } else {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 7;
    }
    func_80072254(D_800AFD1C);
}

extern s32 D_8004F318;
extern s32 D_8004F328;

void func_800945D4(void) {
    D_8004F318 = 0;
    D_8004F328 = 0xFF;
    FieldScriptMemoryWriteU16(
        0xA, 
        ((FieldScriptVMGetArgument(1) << 8) & 0xFF00) | (FieldScriptVMGetArgument(3) & 0xFF)
    );
    g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
}

void func_80094650(void) {
    D_8004F328 = SCRIPT_READ_U8_REL(1);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
}

void func_8009468C(void) {
    D_8004F318 = 0;
    D_8004F328 = 0xFF;
    g_FieldScriptVMCurActor->scriptInstructionPointer++;
}

void func_800946BC(void) {
    g_FieldScriptVMCurActor->flags12C_0 = 0x1;
    g_FieldScriptVMCurActor->unk70 = FieldScriptVMGetArgument(1);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void func_80094710(void) {
    g_FieldScriptVMCurActor->flags12C_0 = 0x2;
    g_FieldScriptVMCurActor->unk70 = FieldScriptVMGetArgument(1);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void func_80094764(void) {
    g_FieldScriptVMCurActor->flags12C_0 = 0x3;
    g_FieldScriptVMCurActor->unk70 = FieldScriptVMGetArgument(1);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

extern void func_80072254(int);

void func_800947B0(void) {
    FieldActor* pActor;

    if (FieldScriptVMGetActorIndex(2) != ACTOR_ID_INVALID) {
        pActor = &g_FieldActors[FieldScriptVMGetActorIndex(2)];
        switch (SCRIPT_READ_U8_REL(1)) {
            case 0:
                pActor->rotation.x += FieldScriptVMGetArgument(3);
                break;
            case 1:
                pActor->rotation.x -= FieldScriptVMGetArgument(3);
                break;
            case 2:
                pActor->rotation.y += FieldScriptVMGetArgument(3);
                break;
            case 3:
                pActor->rotation.y -= FieldScriptVMGetArgument(3);
                break;
            case 4:
                pActor->rotation.z += FieldScriptVMGetArgument(3);
                break;
            case 5:
                pActor->rotation.z -= FieldScriptVMGetArgument(3);
                break;
        }
        func_80072254(FieldScriptVMGetActorIndex(2));
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
}

// Set X, Y or Z rotaiton of Actor D_800AFD1C
void func_80094918(void) {
    switch (SCRIPT_READ_U8_REL(3)) { 
        case 0:
            g_FieldActors[D_800AFD1C].rotation.x = FieldScriptVMGetArgument(1);
            break;
        case 1:
            g_FieldActors[D_800AFD1C].rotation.y = FieldScriptVMGetArgument(1);
            break;
        case 2:
            g_FieldActors[D_800AFD1C].rotation.z = FieldScriptVMGetArgument(1);
            break;
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 4;
    func_80072254(D_800AFD1C);
}

// Increase X Rotation of Actor D_800AFD1C
void func_80094A5C(void) {
    (&g_FieldActors[D_800AFD1C])->rotation.x += FieldScriptVMGetArgument(1);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
    func_80072254(D_800AFD1C);
}

// Decrease X Rotation of Actor D_800AFD1C
void func_80094ACC(void) {
    (&g_FieldActors[D_800AFD1C])->rotation.x -= FieldScriptVMGetArgument(1);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
    func_80072254(D_800AFD1C);
}

// Increase Y Rotation of Actor D_800AFD1C
void func_80094B3C(void) {
    (&g_FieldActors[D_800AFD1C])->rotation.y += FieldScriptVMGetArgument(1);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
    func_80072254(D_800AFD1C);
}

// Decrease Y Rotation of Actor D_800AFD1C
void func_80094BAC(void) {
    (&g_FieldActors[D_800AFD1C])->rotation.y -= FieldScriptVMGetArgument(1);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
    func_80072254(D_800AFD1C);
}

// Increase Z Rotation of Actor D_800AFD1C
void func_80094C1C(void) {
    (&g_FieldActors[D_800AFD1C])->rotation.z += FieldScriptVMGetArgument(1);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
    func_80072254(D_800AFD1C);
}

// Decrease Z Rotation of Actor D_800AFD1C
void func_80094C8C(void) {
    (&g_FieldActors[D_800AFD1C])->rotation.z -= FieldScriptVMGetArgument(1);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
    func_80072254(D_800AFD1C);
}

s32 func_80094CFC(void) {
    s32 i;
    for (i = 0; i < 0x96; i++) {
        u8* p = (u8*)g_pGameState + i;
        if (p[0x1F90] == 0) {
            return i;
        }
        if (p[0x2026] == 0) {
            return i;
        }
    }
    return -1;
}

s32 func_80094D4C(void) {
    s32 i;
    for (i = 0; i < 0x64; i++) {
        u8* p = (u8*)g_pGameState + i;
        if (p[0x1D38] == 0) {
            return i;
        }
        if (p[0x1D9C] == 0) {
            return i;
        }
    }
    return -1;
}

s32 func_80094D9C(void) {
    s32 i;
    for (i = 0; i < 0xC8; i++) {
        u8* p = (u8*)g_pGameState + i;
        if (p[0x1E00] == 0) {
            return i;
        }
        if (p[0x1EC8] == 0) {
            return i;
        }
    }
    return -1;
}

s32 func_80094DEC(void) {
    s32 i;
    for (i = 0; i < 0x64; i++) {
        u8* p = (u8*)g_pGameState + i;
        if (p[0x20BC] == 0) {
            return i;
        }
        if (p[0x2120] == 0) {
            return i;
        }
    }
    return -1;
}

s32 func_80094E3C(void) {
    s32 i;
    for (i = 0; i < 0x96; i++) {
        u8* p = (u8*)g_pGameState + i;
        if (p[0x2184] == 0) {
            return i;
        }
        if (p[0x221A] == 0) {
            return i;
        }
    }
    return -1;
}

s32 func_80094E8C(s32 a0) {
    s32 i;
    for (i = 0; i < 0x96; i++) {
        u8* p = (u8*)g_pGameState + i;
        if (p[0x2026] == a0 && p[0x1F90] != 0) {
            return i;
        }
    }
    return -1;
}

s32 func_80094EDC(s32 a0) {
    s32 i;
    for (i = 0; i < 0xC8; i++) {
        u8* p = (u8*)g_pGameState + i;
        if (p[0x1EC8] == a0 && p[0x1E00] != 0) {
            return i;
        }
    }
    return -1;
}

s32 func_80094F2C(s32 a0) {
    s32 i;
    for (i = 0; i < 0x64; i++) {
        u8* p = (u8*)g_pGameState + i;
        if (p[0x1D9C] == a0 && p[0x1D38] != 0) {
            return i;
        }
    }
    return -1;
}

s32 func_80094F7C(s32 a0) {
    s32 i;
    for (i = 0; i < 0x64; i++) {
        u8* p = (u8*)g_pGameState + i;
        if (p[0x2120] == a0 && p[0x20BC] != 0) {
            return i;
        }
    }
    return -1;
}

s32 func_80094FCC(s32 a0) {
    s32 i;
    for (i = 0; i < 0x96; i++) {
        u8* p = (u8*)g_pGameState + i;
        if (p[0x221A] == a0 && p[0x2184] != 0) {
            return i;
        }
    }
    return -1;
}

u8* func_8009501C(s32 itemId) {
    switch (itemId >> 8) {
    case 0:
        return (u8*)g_pGameState + 0x2026;
    case 1:
        return (u8*)g_pGameState + 0x1D9C;
    case 2:
        return (u8*)g_pGameState + 0x1EC8;
    case 3:
        return (u8*)g_pGameState + 0x2120;
    case 4:
        return (u8*)g_pGameState + 0x221A;
    default:
        return NULL;
    }
}

u8* func_800950A0(s32 itemId) {
    switch (itemId >> 8) {
    case 0:
        return (u8*)g_pGameState + 0x1F90;
    case 1:
        return (u8*)g_pGameState + 0x1D38;
    case 2:
        return (u8*)g_pGameState + 0x1E00;
    case 3:
        return (u8*)g_pGameState + 0x20BC;
    case 4:
        return (u8*)g_pGameState + 0x2184;
    default:
        return NULL;
    }
}

s32 func_80095124(s32 itemId) {
    switch (itemId >> 8) {
    case 0:
        return func_80094E8C(itemId);
    case 1:
        return func_80094F2C(itemId - 0x100);
    case 2:
        return func_80094EDC(itemId - 0x200);
    case 3:
        return func_80094F7C(itemId - 0x300);
    case 4:
        return func_80094FCC(itemId - 0x400);
    default:
        return 0;
    }
}

s32 func_800951B8(s32 itemId) {
    switch (itemId >> 8) {
    case 0:
        return func_80094CFC();
    case 1:
        return func_80094D4C();
    case 2:
        return func_80094D9C();
    case 3:
        return func_80094DEC();
    case 4:
        return func_80094E3C();
    default:
        return 0;
    }
}

void func_8009524C(void) {
    func_80095284();
    g_FieldScriptVMCurActor->scriptInstructionPointer += 1;
}

/* ---- func_80095284: VM opcode 0x5B — halt actor movement and hold ----------
 * asm 80095284-800952FC: zeroes ActorData moveModified (+0x30..0x38) and move
 * (+0x40..0x48), ORs 0x8000 into rotation.vx/.vy (+0x104/+0x106, vy stored
 * first), zeroes three words in the FieldActor pSpriteData object (+0xC,
 * +0x14, +0x18), and sets D_800B00C0 = 1 (yields the VM turn). Does NOT
 * advance scriptInstructionPointer — the opcode re-runs every turn (hold).
 * func_8009524C (opcode 0x5A) is the halt-then-advance variant. */
#ifndef XENO_PC_PORT
/* func_80095284: actor in a local, rot read before the D_800B00C0 store, and the
 * | 0x8000 written inside the stores (a separate rot |= 0x8000 statement lets the
 * ori float up); the sprite clears use pointer indexing. */
#endif
void func_80095284(void) {
#ifdef XENO_PC_PORT
    FieldActor* fieldActor = &g_FieldActors[D_800AFD1C];
    u8* pSprite = (u8*)(uintptr_t)fieldActor->pSpriteData;
    ActorData* actor = g_FieldScriptVMCurActor;
    u16 rot = (u16)actor->rotation.vx;

    D_800B00C0 = 1;
    actor->moveModified.vx = 0;
    actor->moveModified.vy = 0;
    actor->moveModified.vz = 0;
    actor->move.vx = 0;
    actor->move.vy = 0;
    actor->move.vz = 0;
    rot |= 0x8000;
    actor->rotation.vy = rot;
    actor->rotation.vx = rot;
    *(s32*)(pSprite + 0x0C) = 0;
    *(s32*)(pSprite + 0x14) = 0;
    *(s32*)(pSprite + 0x18) = 0;
#else
    u8* pSprite;
    ActorData* actor;
    u16 rot;

    pSprite = (u8*)(uintptr_t)g_FieldActors[D_800AFD1C].pSpriteData;
    actor = g_FieldScriptVMCurActor;
    rot = actor->rotation.vx;
    D_800B00C0 = 1;
    actor->moveModified.vx = 0;
    actor->moveModified.vy = 0;
    actor->moveModified.vz = 0;
    actor->move.vx = 0;
    actor->move.vy = 0;
    actor->move.vz = 0;
    actor->rotation.vy = rot | 0x8000;
    actor->rotation.vx = rot | 0x8000;
    ((s32*)pSprite)[3] = 0;
    ((s32*)pSprite)[5] = 0;
    ((s32*)pSprite)[6] = 0;
#endif
}

void func_80095300(void) {
    g_FieldControl.unkAngle = FieldScriptVMGetArgument(1);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

// Check if an actor is within a trigger zone when considering the entire scene projected to
// 2D, top-down. If so, call a script function.
void FieldScriptHandleTriggerZone2D(void) {
    ActorData* pActor;
    long actorPosition2D;
    long triggerPos2;
    long triggerPos3;
    long triggerPos4;
    long triggerPos1;
    int triggerIndex;
    FieldTriggerZone* pTrigger;

    triggerIndex = SCRIPT_READ_U8_REL(1);
    pActor = (ActorData*)(uintptr_t)g_FieldActors[g_PlayerActorIndex].pActorData;

    // Pack Z and X position into a long for use w/ NormalClip
    actorPosition2D = (CONV_TO_GTE(pActor->position.vz) << 0x10) + CONV_TO_GTE(pActor->position.vx);
    
    // Consider trigger zone in 2D from a top-down
    triggerPos1 = (g_pFieldTriggerZones[triggerIndex].z0 << 0x10) + g_pFieldTriggerZones[triggerIndex].x0;
    triggerPos2 = (g_pFieldTriggerZones[triggerIndex].z1 << 0x10) + g_pFieldTriggerZones[triggerIndex].x1;
    triggerPos3 = (g_pFieldTriggerZones[triggerIndex].z2 << 0x10) + g_pFieldTriggerZones[triggerIndex].x2;
    triggerPos4 = (g_pFieldTriggerZones[triggerIndex].z3 << 0x10) + g_pFieldTriggerZones[triggerIndex].x3;
    
    if (NormalClip(triggerPos1, triggerPos2, actorPosition2D) >= 0 && 
        NormalClip(triggerPos2, triggerPos3, actorPosition2D) >= 0 && 
        NormalClip(triggerPos3, triggerPos4, actorPosition2D) >= 0 && 
        NormalClip(triggerPos4, triggerPos1, actorPosition2D) >= 0
    ) {
        // If we're inside the trigger zone, call script function based on argument
        if (g_FieldScriptVMCurActor->flags12C_0x6 != 0x4) {
            g_FieldScriptVMCurActor->scriptPointersStack[g_FieldScriptVMCurActor->flags12C_0x6] = g_FieldScriptVMCurActor->scriptInstructionPointer + 4;
            g_FieldScriptVMCurActor->scriptInstructionPointer = FieldScriptVMGetInstructionArgument(2);
            g_FieldScriptVMCurActor->flags12C_0x6++;;
            return;
        }
    }
    g_FieldScriptMaxInstructionCount += 1;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 4;
}

// Check if an actor is within a trigger zone, considering height of the trigger zone as well.
// If so, call a script function.
void FieldScriptHandleTriggerZone(void) {
    ActorData* pActor;
    int actorPositionY;
    long actorPosition2D;
    long triggerPos1;
    long triggerPos2;
    long triggerPos3;
    long triggerPos4;
    int triggerIndex;
    FieldTriggerZone* pTrigger;

    triggerIndex = SCRIPT_READ_U8_REL(1);
    pActor = (ActorData*)(uintptr_t)g_FieldActors[g_PlayerActorIndex].pActorData;

    actorPositionY = CONV_TO_GTE(pActor->position.vy);
    if (g_pFieldTriggerZones[triggerIndex].y0 < actorPositionY && 
        (actorPositionY - pActor->height) < g_pFieldTriggerZones[triggerIndex].y0
    ) {
        triggerPos1 = (g_pFieldTriggerZones[triggerIndex].z0 << 0x10) + g_pFieldTriggerZones[triggerIndex].x0;
        triggerPos2 = (g_pFieldTriggerZones[triggerIndex].z1 << 0x10) + g_pFieldTriggerZones[triggerIndex].x1;
        triggerPos3 = (g_pFieldTriggerZones[triggerIndex].z2 << 0x10) + g_pFieldTriggerZones[triggerIndex].x2;
        triggerPos4 = (g_pFieldTriggerZones[triggerIndex].z3 << 0x10) + g_pFieldTriggerZones[triggerIndex].x3;
        actorPosition2D = (CONV_TO_GTE(pActor->position.vz) << 0x10) + CONV_TO_GTE(pActor->position.vx);

        if (
            NormalClip(triggerPos1, triggerPos2, actorPosition2D) >= 0 && 
            NormalClip(triggerPos2, triggerPos3, actorPosition2D) >= 0 && 
            NormalClip(triggerPos3, triggerPos4, actorPosition2D) >= 0 && 
            NormalClip(triggerPos4, triggerPos1, actorPosition2D) >= 0
        ) {
            // If we're inside the trigger zone, call script function based on argument
            if (g_FieldScriptVMCurActor->flags12C_0x6 != 0x4) {
                g_FieldScriptVMCurActor->scriptPointersStack[g_FieldScriptVMCurActor->flags12C_0x6] = g_FieldScriptVMCurActor->scriptInstructionPointer + 4;
                g_FieldScriptVMCurActor->scriptInstructionPointer = FieldScriptVMGetInstructionArgument(2);
                g_FieldScriptVMCurActor->flags12C_0x6++;;
                return;
            }
        }
    }
    g_FieldScriptMaxInstructionCount += 1;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 4;
}

void FieldScriptCheckTriggerZone2D(void) {
    ActorData* pActor;
    long actorPosition2D;
    long triggerPos2;
    long triggerPos3;
    long triggerPos4;
    long triggerPos1;
    int triggerIndex;
    FieldTriggerZone* pTrigger;

    triggerIndex = SCRIPT_READ_U8_REL(1);
    pActor = (ActorData*)(uintptr_t)g_FieldActors[g_PlayerActorIndex].pActorData;

    // Pack Z and X position into a long for use w/ NormalClip
    actorPosition2D = (CONV_TO_GTE(pActor->position.vz) << 0x10) + CONV_TO_GTE(pActor->position.vx);
    
    // Consider trigger zone in 2D from a top-down
    triggerPos1 = (g_pFieldTriggerZones[triggerIndex].z0 << 0x10) + g_pFieldTriggerZones[triggerIndex].x0;
    triggerPos2 = (g_pFieldTriggerZones[triggerIndex].z1 << 0x10) + g_pFieldTriggerZones[triggerIndex].x1;
    triggerPos3 = (g_pFieldTriggerZones[triggerIndex].z2 << 0x10) + g_pFieldTriggerZones[triggerIndex].x2;
    triggerPos4 = (g_pFieldTriggerZones[triggerIndex].z3 << 0x10) + g_pFieldTriggerZones[triggerIndex].x3;
    
    if (NormalClip(triggerPos1, triggerPos2, actorPosition2D) >= 0 && 
        NormalClip(triggerPos2, triggerPos3, actorPosition2D) >= 0 && 
        NormalClip(triggerPos3, triggerPos4, actorPosition2D) >= 0 && 
        NormalClip(triggerPos4, triggerPos1, actorPosition2D) >= 0
    ) {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 4;
        return;
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer = FieldScriptVMGetInstructionArgument(2);
    g_FieldScriptMaxInstructionCount += 1;
}

void FieldScriptCheckTriggerZone(void) {
    ActorData* pActor;
    int actorPositionY;
    long actorPosition2D;
    long triggerPos1;
    long triggerPos2;
    long triggerPos3;
    long triggerPos4;
    int triggerIndex;
    FieldTriggerZone* pTrigger;

    triggerIndex = SCRIPT_READ_U8_REL(1);
    pActor = (ActorData*)(uintptr_t)g_FieldActors[g_PlayerActorIndex].pActorData;

    actorPositionY = CONV_TO_GTE(pActor->position.vy);
    if (g_pFieldTriggerZones[triggerIndex].y0 < actorPositionY && 
        (actorPositionY - pActor->height) < g_pFieldTriggerZones[triggerIndex].y0
    ) {
        triggerPos1 = (g_pFieldTriggerZones[triggerIndex].z0 << 0x10) + g_pFieldTriggerZones[triggerIndex].x0;
        triggerPos2 = (g_pFieldTriggerZones[triggerIndex].z1 << 0x10) + g_pFieldTriggerZones[triggerIndex].x1;
        triggerPos3 = (g_pFieldTriggerZones[triggerIndex].z2 << 0x10) + g_pFieldTriggerZones[triggerIndex].x2;
        triggerPos4 = (g_pFieldTriggerZones[triggerIndex].z3 << 0x10) + g_pFieldTriggerZones[triggerIndex].x3;
        actorPosition2D = (CONV_TO_GTE(pActor->position.vz) << 0x10) + CONV_TO_GTE(pActor->position.vx);

        if (
            NormalClip(triggerPos1, triggerPos2, actorPosition2D) >= 0 && 
            NormalClip(triggerPos2, triggerPos3, actorPosition2D) >= 0 && 
            NormalClip(triggerPos3, triggerPos4, actorPosition2D) >= 0 && 
            NormalClip(triggerPos4, triggerPos1, actorPosition2D) >= 0
        ) {
            g_FieldScriptVMCurActor->scriptInstructionPointer += 4;
            return;
        }
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer = FieldScriptVMGetInstructionArgument(2);
    g_FieldScriptMaxInstructionCount += 1;
}

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): field_event_project_actor_to_screen (src/decomp/src/field/field_event.c:5973). */
#ifndef XM_T_SavedView
#define XM_T_SavedView
typedef struct {
    s16 octant;
    s32 projection;
    s16 elevation;
} SavedView;
#endif
#ifndef XM_T_ModelLight
#define XM_T_ModelLight
typedef struct {
    s32 vx, vy, vz;
    u16 r, g, b;
} ModelLight;
#endif
#ifndef XM_T_MorphDelta
#define XM_T_MorphDelta
typedef struct {
    s16 dx, dy, dz;
    s16 index;
} MorphDelta;
#endif
#ifndef XM_T_MorphTarget
#define XM_T_MorphTarget
typedef struct {
    s32 count;
    void *deltas;
    MorphDelta *normals;
} MorphTarget;
#endif
#ifndef XM_T_MorphTable
#define XM_T_MorphTable
typedef struct {
    s32 count;
    MorphTarget targets[1];
} MorphTable;
#endif
#ifndef XM_T_SpriteModel
#define XM_T_SpriteModel
typedef struct {
    u16 flags;          
    u16 vertex_count;
    u16 primitive_count;
    u16 group_count;    
    SVECTOR *vertices;
    SVECTOR *normals;
    u8 *unk10;          
    u8 *unk14;
    u8 *unk18;
    MorphTable *morphs; 
    SVECTOR box_min;    
    SVECTOR box_max;
    s32 aux_size;       
    s32 packet_size;    
} SpriteModel;
#endif
#ifndef XM_T_MorphChannel
#define XM_T_MorphChannel
typedef struct MorphChannel {
    s32 (*update)(struct MorphChannel *channel);
    s32 target;
    s32 weight;
    s32 step;
    u8 unk10[0x10];
} MorphChannel;
#endif
#ifndef XM_T_MorphState
#define XM_T_MorphState
typedef struct {
    SpriteModel *model;
    SVECTOR *vertices;
    SVECTOR *normals;
    s32 count;
    MorphChannel *channels;
} MorphState;
#endif
#ifndef XM_T_FieldInstance
#define XM_T_FieldInstance
typedef struct {
    u8 unk00[4];
    SpriteModel *mesh; 
    void *packets[2]; 
    u8 unk10[2];
    s16 mode;        
    MorphState *anims; 
    s16 center[3];   
    s16 unk1E;
    s16 radius;      
} FieldInstance;
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
#ifndef XM_T_FieldMarker
#define XM_T_FieldMarker
typedef struct FieldMarker {
    SVECTOR v[4];
    POLY_FT4 poly[2];
} FieldMarker;
#endif
#ifndef XM_T_FieldDescriptor
#define XM_T_FieldDescriptor
typedef struct FieldDescriptor {
    FieldInstance *instance; 
    Sprite *model;           
    struct FieldMarker *shadow; 
    MATRIX matrix;           
    MATRIX transform;        
    FieldActor *actor;       
    SVECTOR rotation;        
    u16 flags;               
    u16 unk5A;               
} FieldDescriptor;
#endif
#ifndef XM_T_Attribute
#define XM_T_Attribute
typedef union {
    u32 word;
    u8 bytes[4];
} Attribute;
#endif
#ifndef XM_T_CollisionTriangle
#define XM_T_CollisionTriangle
typedef struct {
    s16 unk00[6];
    u8 attribute;
    u8 unk0D;
} CollisionTriangle;
#endif
#ifndef XM_T_FieldComponents
#define XM_T_FieldComponents
typedef struct {
    s32 descriptor_count;                      
    FieldDescriptor *descriptors;              
    s32 *geometry;                             
    s32 *collision;                            
    s32 *sprites;                              
    Attribute *collision_attributes;           
    CollisionTriangle *collision_triangles[4]; 
    void *collision_vertices[4];               
    s32 triangle_counts[4];                    
    s16 layer_count;                           
} FieldComponents;
#endif
#ifndef XM_T_FieldView
#define XM_T_FieldView
typedef struct FieldView {
    VECTOR eye;              
    VECTOR target;           
    VECTOR up;               
    VECTOR eye_goal;         
    VECTOR target_goal;      
    VECTOR unk050;           
    VECTOR shake_offset;     
    VECTOR saved_target;     
    VECTOR point_actor_a;    
    VECTOR saved_eye;        
    VECTOR point_actor_b;    
    s32 scripted_zoom;       
    s16 mode;                
    s16 scripted_elevation;  
    s16 scripted_heading;    
    s16 scripted_scale;      
    u16 scripted;            
    s16 target_steps;        
    VECTOR scripted_target;  
    VECTOR target_step;      
    s16 eye_steps;           
    s32 scripted_eye[3];     
    s32 unk0F0;              
    s32 eye_step[3];         
    s32 unk100;              
    s32 target_a;            
    s32 target_b;            
    s16 angle;               
    s16 view_angle;          
    MATRIX previous_view;    
    MATRIX orbit;            
    SVECTOR orbit_angles;    
    u32 flags;               
    s16 bounds[4];           
    SVECTOR heading_angles;  
    s32 heading_velocity;    
    s32 heading_high;        
    u8 heading_blocks[2];    
    s16 heading_steps;       
    s32 projection;          
    u16 elevation;           
    s16 distance;            
    s16 elevation_steps;     
    s32 elevation_value;     
    s32 elevation_step;      
    u32 heading;             
    s16 projection_steps;    
    s32 projection_value;    
    s32 projection_step;     
    s16 steps;               
    s32 start;               
    s32 step;                
    s16 shake;               
    s16 shake_time;          
    s16 shake_stop;          
    s32 shake_amplitude[3];  
    s32 shake_step[3];       
    SavedView saved_view;    
    SVECTOR world_angles;    
    SVECTOR anchor;          
    MATRIX scaled_world;     
    MATRIX unk204;           
    MATRIX world_matrix;     
    s32 scale;               
    ModelLight lights[3];    
    s16 back_color[3];       
    u8 unk28A[2];
    FieldComponents components; 
} FieldView;
#endif
#ifndef XM_S_field_event_project_actor_to_screen
#define XM_S_field_event_project_actor_to_screen
void xm_field_event_project_actor_to_screen(s32 *x, s32 *y) __asm__("FieldProjectActorOriginToScreen");
#endif
#ifndef XM_S_CompMatrix
#define XM_S_CompMatrix
MATRIX *xm_CompMatrix(MATRIX *m0, MATRIX *m1, MATRIX *m2) __asm__("CompMatrix");
#endif
#ifndef XM_S_field_event_read_actor_index_or_leader
#define XM_S_field_event_read_actor_index_or_leader
s32 xm_field_event_read_actor_index_or_leader(s32 offset) __asm__("func_8009CD7C");
#endif
#ifndef XM_S_SetRotMatrix
#define XM_S_SetRotMatrix
void xm_SetRotMatrix(MATRIX *m) __asm__("SetRotMatrix");
#endif
#ifndef XM_S_SetTransMatrix
#define XM_S_SetTransMatrix
void xm_SetTransMatrix(MATRIX *m) __asm__("SetTransMatrix");
#endif
#ifndef XM_S_RotTransPers
#define XM_S_RotTransPers
long xm_RotTransPers(SVECTOR *v0, long *sxy, long *p, long *flag) __asm__("RotTransPers");
#endif
void FieldProjectActorOriginToScreen(s32 *x, s32 *y) {
    SVECTOR origin;
    MATRIX m;
    union {
        long word;
        DVECTOR xy;
    } screen;
    long depth;
    long flag;

    xm_CompMatrix(&xm_field_view.scaled_world, &xm_field_view.components.descriptors[xm_field_event_read_actor_index_or_leader(1)].transform, &m);
    origin.vx = 0;
    origin.vy = 0;
    origin.vz = 0;
    xm_SetRotMatrix(&m);
    xm_SetTransMatrix(&m);
    xm_RotTransPers(&origin, &screen.word, &depth, &flag);
    *y = screen.xy.vy;
    *x = screen.xy.vx;
}
#endif
#else
void FieldProjectActorOriginToScreen(int* screenX, int* screenY) {
    MATRIX matrix;
    SVECTOR origin;
    int screenXY;
    long depth;
    long flag;
    u32 actorIndex;

    actorIndex = func_8009CD7C(1);
    CompMatrix(&g_Scene.worldToScreenMatrix,
               &g_FieldActors[actorIndex].childMatrix, &matrix);

    origin.vx = 0;
    origin.vy = 0;
    origin.vz = 0;
    SetRotMatrix(&matrix);
    SetTransMatrix(&matrix);
    RotTransPers(&origin, &screenXY, &depth, &flag);

    *screenY = (s16)(screenXY >> 16);
    *screenX = (s16)screenXY;
}
#endif /* XENO_PC_PORT */

extern s32 D_800ADC18;

void func_80095B3C(void) {
    int screenX;
    int screenY;

    FieldProjectActorOriginToScreen(&screenX, &screenY);
    if (D_800ADC18 != 0) {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 4;
        return;
    }
    
    if ((screenY - 33) < 159U && (screenX - 33) < 255U) {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 4;
    } else {
        g_FieldScriptVMCurActor->scriptInstructionPointer = FieldScriptVMGetInstructionArgument(2);
    }
    
    D_800B00C0 = 1;
}

void FieldScriptCheckActorOnScreen(void) {
    int screenX;
    int screenY;

    FieldProjectActorOriginToScreen(&screenX, &screenY);
    if (D_800ADC18 != 0) {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 4;
        return;
    }
    
    if ((screenY - 1) < (SCREEN_HEIGHT - 1) && (screenX - 1) < (SCREEN_WIDTH - 1)) {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 4;
    } else {
        g_FieldScriptVMCurActor->scriptInstructionPointer = FieldScriptVMGetInstructionArgument(2);
    }
    
    D_800B00C0 = 1;
}

void func_80095CC4(void) {
    s32 actorIdx = FieldScriptVMGetActorIndex(1);
    if (actorIdx != ACTOR_ID_INVALID) {
        u32 actorAddr = (u32)(FieldScriptVMGetActorIndex(1) * sizeof(FieldActor));
        FieldActor* pActor;
        void* pActorData;
        s32 arg2;
        actorAddr += (u32)(uintptr_t)g_FieldActors;
        pActor = (FieldActor*)(uintptr_t)actorAddr;
        pActorData = (void*)(uintptr_t)pActor->pActorData;
        arg2 = FieldScriptVMGetArgument(2);
        if (arg2 == *(s16*)((u8*)pActorData + 0x10)) {
            g_FieldScriptVMCurActor->scriptInstructionPointer += 6;
            return;
        }
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer = FieldScriptVMGetInstructionArgument(4);
}

extern u32 D_800AFB24[];

void func_80095D6C(void) {
    s32 actorIdx = FieldScriptVMGetActorIndex(1);
    if (actorIdx != ACTOR_ID_INVALID) {
        u32 actorAddr = (u32)(FieldScriptVMGetActorIndex(1) * sizeof(FieldActor));
        FieldActor* pActor;
        u8* pActorData;
        s16 idx1;
        s16 idx2;
        u8* tableBase;
        u8 matchVal;
        s32 arg2;
        actorAddr += (u32)(uintptr_t)g_FieldActors;
        pActor = (FieldActor*)(uintptr_t)actorAddr;
        pActorData = (u8*)(uintptr_t)pActor->pActorData;
        idx1 = *(s16*)(pActorData + 0x10);
        idx2 = *(s16*)(pActorData + idx1 * 2 + 0x8);
        tableBase = (u8*)(uintptr_t)D_800AFB24[idx1];
        matchVal = tableBase[idx2 * 14 + 0xC];
        arg2 = FieldScriptVMGetArgument(2);
        if (arg2 == matchVal) {
            g_FieldScriptVMCurActor->scriptInstructionPointer += 6;
            return;
        }
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer = FieldScriptVMGetInstructionArgument(4);
}

extern FieldActor* D_800B06B8;

void FieldScriptCheckActorDistance(void) {
    ActorData* pActor;
    ActorData* pActorOther;
    FieldActor* pFieldActor;
    int distance;
    int actorIndex;

    actorIndex = FieldScriptVMGetActorIndex(1);
    if (actorIndex != 0xFF) {
        pFieldActor = &g_FieldActors[FieldScriptVMGetActorIndex(1)];
        
        pActor = (ActorData*)(uintptr_t)D_800B06B8->pActorData;
        pActorOther = (ActorData*)(uintptr_t)pFieldActor->pActorData;
        distance = FieldGetVec3Magnitude(
            CONV_TO_GTE(pActor->position.vx) - CONV_TO_GTE(pActorOther->position.vx), 
            CONV_TO_GTE(pActor->position.vy) - CONV_TO_GTE(pActorOther->position.vy), 
            CONV_TO_GTE(pActor->position.vz) - CONV_TO_GTE(pActorOther->position.vz)
        );
        
        if (distance < FieldScriptVMGetArgument(2)) {
            g_FieldScriptVMCurActor->scriptInstructionPointer += 6;
            return;
        }
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer = FieldScriptVMGetInstructionArgument(4);
}
