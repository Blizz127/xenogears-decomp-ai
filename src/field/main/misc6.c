#include "common.h"
#include "main/game.h"
#include "system/math.h"
#include "field/script_vm.h"
#include "field/actor.h"
#include "field/main.h"
#include "field/text_box.h"
#include "field/camera.h"
#ifdef XENO_PC_PORT
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#else
/* <assert.h> is unavailable under the matching build's -nostdinc MIPS
 * preprocessor. The assert(...) calls below mark invariant checks in
 * functions not yet byte-matched, so a no-op assert compiles safely there. */
#define assert(x) ((void)0)
#endif

extern void func_80076AC0(s32, s32, void*, s32, s32, s32, s32);
extern s32 ArchiveSetIndex(s32 directoryIndex, s32 entryIndex);
extern s32 ArchiveDecodeAlignedSize(u32 entryIndex);
extern s32 func_80029AFC(void* pEntries, s32 arg1, s32 arg2);
extern void* HeapAlloc(u32 size, u32 allocMode);
extern void HeapFree(void* pBlock);
extern s32 ArchiveCdDataSync(s32 mode);
extern s32 D_800AFD1C; // Current actor index
extern s32 D_800ADB1C;
extern s32 D_800ADB2C;
extern u8 D_800B21CE;
extern u16 D_800AEA54[];
extern s16 D_800AFB54;
extern s32 D_8005A444[];
extern s32 D_800AFFEC;
extern s32 D_800B2268;
extern s16 D_800AFD20;
extern s16 D_800B233E;
extern s32 g_PlayerActorIndex;
extern void* g_PartyDataBuffers[];
extern s16 func_8007B1C4(s16, s16, s32, s16*, s32*);
extern s32 func_80080968(u8*);
extern s32 func_8008CF3C(s32);
extern s32 FieldScriptVMGetArgument(s32);
extern s32 func_8008A558(void);
extern void func_8009FA54(s32);
extern s32 D_8006F990[];
extern s32 GameCharacterGetGearID(s32 characterId);
extern u8 D_800B225F[];
extern u16 D_800B21DC[];
extern s16 D_800B21EC[];
extern s32 D_800B220C[];
extern u8 D_800B2394;
extern void* D_8005A420[];
extern void* D_8005A450[];
extern s32 D_801E8670[];
extern void func_801E8030(s32 idx);
extern void func_801E742C(s32 slot, s32 flags, void* pModelArc,
                          void* pTexArc, s32 x, s32 y, s32 z, s32 w,
                          s16* vec);

void FieldScriptVMHandlerDisableDialogActivation(void) {
    g_FieldScriptVMCurActor->scriptFlags.fields.scriptFlags_0xA = 0x1;
    g_FieldScriptVMCurActor->scriptInstructionPointer++;
}

void FieldScriptVMHandlerEnableDialogActivation(void) {
    g_FieldScriptVMCurActor->scriptFlags.fields.scriptFlags_0xA = 0x0;
    g_FieldScriptVMCurActor->scriptInstructionPointer++;
}

void func_8009DA70(void) {
    g_FieldScriptVMCurActor->scriptFlags.fields.scriptFlags_0x16 = 0x1;
    g_FieldScriptVMCurActor->scriptInstructionPointer++;
}

void func_8009DA98(void) {
    g_FieldScriptVMCurActor->scriptFlags.fields.scriptFlags_0x16 = 0;
    g_FieldScriptVMCurActor->scriptInstructionPointer++;
}

void func_8009DAC4(void) {
    int nTextBoxIndex;
    ActorData* pActor;
    FieldActor* pFieldActor;

    if (FieldScriptVMGetActorIndex(1) != ACTOR_ID_INVALID) {
        pActor = (ActorData*)(uintptr_t)g_FieldActors[FieldScriptVMGetActorIndex(1)].pActorData;
        pActor->scriptFlags.fields.scriptFlags_0x0 = 0x1;
        pActor->flags |= 0x100000;
        pFieldActor = &g_FieldActors[FieldScriptVMGetActorIndex(1)];
        pFieldActor->status |= ACTOR_STATUS_INVISIBLE;
        if (func_8009CD18(&nTextBoxIndex) == 0) {
            g_FieldTextBoxes[nTextBoxIndex].status = 0;
        }
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
}

void FieldScriptVMHandlerEnableActorVM(void) {
    ActorData* pActor;

    if (FieldScriptVMGetActorIndex(1) != ACTOR_ID_INVALID) {
        pActor = (ActorData*)(uintptr_t)g_FieldActors[FieldScriptVMGetActorIndex(1)].pActorData;
        pActor->scriptFlags.fields.scriptFlags_0x0 = 0;
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
}

void func_8009DC4C(void) {
    ActorData* pActor;
    int nTextBoxIndex;
    short nNewRotation;

    if (FieldScriptVMGetActorIndex(1) != ACTOR_ID_INVALID) {
        pActor = (ActorData*)(uintptr_t)g_FieldActors[FieldScriptVMGetActorIndex(1)].pActorData;
        pActor->moveModified.vx = 0;
        pActor->moveModified.vy = 0;
        pActor->moveModified.vz = 0;
        pActor->move.vx = 0;
        pActor->move.vy = 0;
        pActor->move.vz = 0;
        pActor->scriptFlags.fields.scriptFlags_0x0 = 1;
        nNewRotation = pActor->rotation.vx | 0x8000;
        pActor->rotation.vy = nNewRotation;
        pActor->rotation.vx = nNewRotation;
        if (func_8009CD18(&nTextBoxIndex) == 0) {
            g_FieldTextBoxes[nTextBoxIndex].status = 0;
        }
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
}

void FieldScriptVMHandlerSleep(void) {
    u_char nScriptId;

    nScriptId = g_FieldScriptVMCurActor->curScriptIndex;

    // Initialize the value with the argument as the time to sleep, else count down
    if (g_FieldScriptVMCurActor->scripts[nScriptId].waitTimer == 0) {
        g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].waitTimer = FieldScriptVMGetArgument(1);
    } else {
        g_FieldScriptVMCurActor->scripts[nScriptId].waitTimer--;
    }
    
    // When the timer has reached 0, we move to the next instruction
    if (g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].waitTimer == 0) {
#ifndef NPC_EVENT_MUTANT_NO_IP
        g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
#endif
    }

    D_800B00C0 = 1;
}

void FieldScriptVMHandlerShowActorById(void) {
    FieldActor* pFieldActor;

    if (FieldScriptVMGetActorIndex(1) != ACTOR_ID_INVALID) {
        pFieldActor = &g_FieldActors[FieldScriptVMGetActorIndex(1)];
        if (!(((ActorData*)(uintptr_t)pFieldActor->pActorData)->flags & 0x100000)) {
            pFieldActor->status &= ~ACTOR_STATUS_INVISIBLE;
            ((ActorData*)(uintptr_t)pFieldActor->pActorData)->flags &= ~0x2000000;
        }
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
}

void FieldScriptVMHandlerHideActorById(void) {
    FieldActor* pFieldActor;

    if (FieldScriptVMGetActorIndex(1) != ACTOR_ID_INVALID) {
        pFieldActor = &g_FieldActors[FieldScriptVMGetActorIndex(1)];
        pFieldActor->status |= ACTOR_STATUS_INVISIBLE;
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
}

void FieldScriptVMHandlerShowActor(void) {
    FieldActor* pActor = &g_FieldActors[D_800AFD1C];
    pActor->status &= ~ACTOR_STATUS_INVISIBLE;
    g_FieldScriptVMCurActor->curAnimationId = 0xFF;
    g_FieldScriptVMCurActor->flags &= 0xFDFFFFFF;
    g_FieldScriptVMCurActor->scriptInstructionPointer++;
}

void func_8009DF78(void) {
    FieldActor* pFieldActor;

    if (FieldScriptVMGetActorIndex(1) != ACTOR_ID_INVALID) {
        pFieldActor = &g_FieldActors[FieldScriptVMGetActorIndex(1)];
        ((ActorData*)(uintptr_t)pFieldActor->pActorData)->flags |= 0x02000000;
        ((ActorData*)(uintptr_t)pFieldActor->pActorData)->flags |= 0x800;
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
}

void func_8009E014(void) {
    g_FieldScriptVMCurActor->flags |= 0x02000800;
    g_FieldScriptVMCurActor->scriptInstructionPointer++;
}

void FieldScriptVMHandlerHideActor(void) {
    FieldActor* pActor = &g_FieldActors[D_800AFD1C];
    pActor->status |=  ACTOR_STATUS_INVISIBLE;
    g_FieldScriptVMCurActor->scriptInstructionPointer++;
}

// Update sprite movement or animation speed?
void func_8009E094(void) {
    unsigned short nMoveSpeed;

    nMoveSpeed = FieldScriptVMGetArgument(1);
    g_FieldScriptVMCurActor->moveSpeed = nMoveSpeed;
    func_80021BCC((void*)(uintptr_t)g_FieldActors[D_800AFD1C].pSpriteData, nMoveSpeed);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void func_8009E10C(void) {
    int nValue;
    int nUnkFlags;

    nValue = FieldScriptVMGetArgument(1);
    nUnkFlags = (nValue & 0x1) << 7; // if (nValue & 0x1) nUnkFlags |= 0x80;
    if (nValue & 0x4) nUnkFlags |= 0x20;
    if (nValue & 0x8) nUnkFlags |= 0x10;
    if (nValue & 0x10) nUnkFlags |= 8;
    if (nValue & 0x20) nUnkFlags |= 4;
    if (nValue & 0x40) nUnkFlags |= 0x08000000;
    
    g_FieldScriptVMCurActor->scriptFlags.flags &= 0xF7FFFF43;
    g_FieldScriptVMCurActor->scriptFlags.flags |= nUnkFlags;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void func_8009E1A0(void) {
    u32 nValue;

    nValue = SCRIPT_READ_U8_REL(1) & 7;
    g_FieldScriptVMCurActor->flags &= ~7;
    g_FieldScriptVMCurActor->flags |= nValue;
    
    nValue = (SCRIPT_READ_U8_REL(1) >> 1) & 0x38;
    g_FieldScriptVMCurActor->flags &= ~0x38;
    g_FieldScriptVMCurActor->flags |= nValue;
    
    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
}

void func_8009E208(void) {
    g_FieldScriptVMCurActor->unkEC = 0;
    g_FieldScriptVMCurActor->scriptFlags.fields.scriptFlags_0x10 = 0x0;
    g_FieldScriptVMCurActor->scriptFlags.fields.scriptFlags_0x15 = 0x1;
    g_FieldScriptVMCurActor->scriptInstructionPointer++;
    g_FieldScriptVMCurActor->curYPos = CONV_TO_GTE(g_FieldScriptVMCurActor->position.vy);
}

void func_8009E248(void) {
    func_8009E574(
        FieldScriptVMGetInstructionArgumentS16(1),
        FieldScriptVMGetInstructionArgumentS16(3)
    );
    func_8009E810(FieldScriptVMGetInstructionArgumentS16(5));
    g_FieldScriptVMCurActor->scriptFlags.fields.scriptFlags_0x10 = 0x1;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 7;
}

void func_8009E2C8(void) {
    func_8009E810(FieldScriptArgument1(1, SCRIPT_READ_U8_REL(3)));
    g_FieldScriptVMCurActor->scriptFlags.fields.scriptFlags_0x10 = 0x1;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 4;
}

// Read short from bytecode, absolute offset
short func_8009E330(int offset) {
    return SCRIPT_READ_U8(offset) + (SCRIPT_READ_U8(offset + 1) << 8);
}

void func_8009E35C(void) {
    g_FieldScriptVMCurActor->walkmeshId = SCRIPT_READ_U8_REL(5);
    func_8009E574(
        FieldScriptArgument1(1, SCRIPT_READ_U8_REL(6)), 
        FieldScriptArgument2(3, SCRIPT_READ_U8_REL(6))
    );
    g_FieldScriptVMCurActor->flags &= ~0x200000;
    g_FieldScriptVMCurActor->scriptFlags.fields.scriptFlags_0xX = 0x0;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 7;
}

void func_8009E428(void) {
    ActorData* pActor;

    g_FieldScriptVMCurActor->walkmeshId = SCRIPT_READ_U8_REL(1);
    pActor = (ActorData*)(uintptr_t)g_FieldActors[D_800AFD1C].pActorData;
    func_8009E574(
        CONV_TO_GTE(pActor->position.vx), 
        CONV_TO_GTE(pActor->position.vz)
    );
    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
}

void func_8009E4BC(void) {
    func_8009E574(
        FieldScriptArgument1(1, SCRIPT_READ_U8_REL(5)), 
        FieldScriptArgument2(3, SCRIPT_READ_U8_REL(5))
    );
    g_FieldScriptVMCurActor->flags &= ~0x200000;
    g_FieldScriptVMCurActor->scriptFlags.fields.scriptFlags_0xX = 0x0;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 6;
}

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): field_event_place_actor_on_floor (src/decomp/src/field/field_event.c:9314). */
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
#ifndef XM_S_field_event_place_actor_on_floor
#define XM_S_field_event_place_actor_on_floor
void xm_field_event_place_actor_on_floor(s32 x, s32 z) __asm__("func_8009E574");
#endif
#ifndef XM_S_field_view
#define XM_S_field_view
extern xm_FieldView xm_field_view __asm__("g_CameraEye");
#endif
#ifndef XM_S_field_current_event_actor_index
#define XM_S_field_current_event_actor_index
extern s32 xm_field_current_event_actor_index __asm__("D_800AFD1C");
#endif
#ifndef XM_S_field_current_event_actor
#define XM_S_field_current_event_actor
extern xm_FieldActor *xm_field_current_event_actor __asm__("g_FieldScriptVMCurActor");
#endif
#ifndef XM_S_field_collision_find_floor_triangle
#define XM_S_field_collision_find_floor_triangle
s32 xm_field_collision_find_floor_triangle(s32 x, s32 z, s32 layer, SVECTOR *point, VECTOR *normal) __asm__("func_8007B1C4");
#endif
#ifndef XM_S_field_actor_get_floor_attribute
#define XM_S_field_actor_get_floor_attribute
u32 xm_field_actor_get_floor_attribute(struct xm_FieldActor *actor) __asm__("func_80080968");
#endif
void func_8009E574(s32 x, s32 z) {
    VECTOR normals[4];
    SVECTOR points[4];
    xm_Sprite *model;
    s32 layer;

    model = xm_field_view.components.descriptors[xm_field_current_event_actor_index].model;
    for (layer = 0; layer < xm_field_view.components.layer_count - 1; layer++) {
        xm_field_current_event_actor->triangle[layer] = xm_field_collision_find_floor_triangle(x, z, layer, &points[layer], &normals[layer]);
    }
    xm_field_current_event_actor->floor_attribute = xm_field_actor_get_floor_attribute(xm_field_current_event_actor);
    xm_field_current_event_actor->unk50[0] = (normals + xm_field_current_event_actor->layer)->vx;
    xm_field_current_event_actor->unk50[1] = (normals + xm_field_current_event_actor->layer)->vy;
    xm_field_current_event_actor->unk50[2] = (normals + xm_field_current_event_actor->layer)->vz;
    xm_field_view.components.descriptors[xm_field_current_event_actor_index].transform.t[0] = xm_field_view.components.descriptors[xm_field_current_event_actor_index].matrix.t[0] = x;
    xm_field_view.components.descriptors[xm_field_current_event_actor_index].transform.t[1] = xm_field_view.components.descriptors[xm_field_current_event_actor_index].matrix.t[1] = points[xm_field_current_event_actor->layer].vy;
    xm_field_view.components.descriptors[xm_field_current_event_actor_index].transform.t[2] = xm_field_view.components.descriptors[xm_field_current_event_actor_index].matrix.t[2] = z;
    model->ground = points[xm_field_current_event_actor->layer].vy;
    xm_field_current_event_actor->position[0] = x << 16;
    xm_field_current_event_actor->position[1] = points[xm_field_current_event_actor->layer].vy << 16;
    xm_field_current_event_actor->position[2] = z << 16;
    xm_field_current_event_actor->unk72 = points[xm_field_current_event_actor->layer].vy;
    model->x = xm_field_current_event_actor->position[0];
    model->y = xm_field_current_event_actor->position[1];
    model->z = xm_field_current_event_actor->position[2];
    xm_field_current_event_actor->unk40[0] = 0;
    xm_field_current_event_actor->unk40[1] = 0;
    xm_field_current_event_actor->unk40[2] = 0;
    xm_field_current_event_actor->unk030[0] = 0;
    xm_field_current_event_actor->unk030[1] = 0;
    xm_field_current_event_actor->unk030[2] = 0;
    xm_field_current_event_actor->target[0] = 0;
    xm_field_current_event_actor->target[1] = 0;
    xm_field_current_event_actor->target[2] = 0;
    xm_field_current_event_actor->unk62 = 0;
    xm_field_current_event_actor->unk60 = 0;
    xm_field_current_event_actor->unk64 = 0;
    model->speed_x = 0;
    model->speed_y = 0;
    model->speed_z = 0;
    xm_field_current_event_actor->unkF0 = 0;
    xm_field_current_event_actor->unkEC = 0;
    xm_field_current_event_actor->unk72 = xm_field_current_event_actor->position[1] >> 16;
    xm_field_current_event_actor->flags = (xm_field_current_event_actor->flags & ~0x40000) | 0x400000;
}
#endif
#else
/* Retail 8009E574-8009E80C, re-translated instruction-faithfully.  x/z are
 * the caller's full argument words: retail stores them unmodified into the
 * FieldActor translation words (+0x20/+0x40, +0x28/+0x48) and shifts them for
 * the ActorData position.  (Port note: func_8007B1C4 is still called through
 * its s16 port prototype, so it sees only the low halfwords of x/z where
 * retail passes the full words -- see the f2 misc6 differential.) */
void func_8009E574(s32 x, s32 z) {
    u8* pFieldActor;
    ActorData* pActor;
    u8* pSpriteData;
    s32 state[4][4];
    s16 out[4][4];
    s32 i;
    s16 y;

#ifdef XENO_PC_PORT
    if (getenv("XENO_FIELD_DIAG") != NULL) {
        u8* ipb = (u8*)g_FieldScriptVMCurScriptData;
        ActorData* pDiag = g_FieldScriptVMCurActor;
        unsigned ip = pDiag != NULL ? (unsigned)pDiag->scriptInstructionPointer : 0u;
        printf("[field-diag] setpos actor=%d ip=%u x=%d z=%d bytes=%02x %02x %02x %02x %02x %02x %02x %02x\n",
               (int)D_800AFD1C, ip, (int)x, (int)z,
               ipb != NULL ? ipb[ip] : 0, ipb != NULL ? ipb[ip + 1] : 0,
               ipb != NULL ? ipb[ip + 2] : 0, ipb != NULL ? ipb[ip + 3] : 0,
               ipb != NULL ? ipb[ip + 4] : 0, ipb != NULL ? ipb[ip + 5] : 0,
               ipb != NULL ? ipb[ip + 6] : 0, ipb != NULL ? ipb[ip + 7] : 0);
    }
#endif
    /* 8009E580-8009E5CC: the sprite pointer is taken once, up front. */
    pSpriteData = (u8*)(uintptr_t)g_FieldActors[D_800AFD1C].pSpriteData;

    /* 8009E5C4-8009E62C: bound re-read every iteration. */
    for (i = 0; i < D_800AFB54 - 1; i++) {
        s16 tri = func_8007B1C4(x, z, i, out[i], state[i]);
        g_FieldScriptVMCurActor->walkmeshTriIds[i] = tri;
    }

    {
        u32 material = (u32)func_80080968((u8*)g_FieldScriptVMCurActor);
        pActor = g_FieldScriptVMCurActor;
        pActor->curWalkmeshTriMaterial = material;
    }
    pActor->curTriNormal.vx = state[pActor->walkmeshId][0];
    pActor->curTriNormal.vy = state[pActor->walkmeshId][1];
    pActor->curTriNormal.vz = state[pActor->walkmeshId][2];

    /* 8009E690-8009E700: FieldActor re-derived from D_800AFD1C. */
    pFieldActor = (u8*)&g_FieldActors[D_800AFD1C];
    *(s32*)(pFieldActor + 0x20) = x;
    *(s32*)(pFieldActor + 0x40) = x;
    pFieldActor = (u8*)&g_FieldActors[D_800AFD1C];
    *(s32*)(pFieldActor + 0x24) = out[pActor->walkmeshId][1];
    *(s32*)(pFieldActor + 0x44) = out[pActor->walkmeshId][1];
    pFieldActor = (u8*)&g_FieldActors[D_800AFD1C];
    *(s32*)(pFieldActor + 0x28) = z;
    *(s32*)(pFieldActor + 0x48) = z;

    *(u16*)(pSpriteData + 0x84) = (u16)out[pActor->walkmeshId][1];
    pActor->position.vx = (s32)((u32)x << 16);
    y = out[pActor->walkmeshId][1];
    pActor->position.vz = (s32)((u32)z << 16);
    pActor->position.vy = (s32)((u32)(s32)y << 16);
    pActor->curYPos = (u16)out[pActor->walkmeshId][1];

    *(s32*)(pSpriteData + 0x00) = pActor->position.vx;
    *(s32*)(pSpriteData + 0x04) = pActor->position.vy;
    *(s32*)(pSpriteData + 0x08) = pActor->position.vz;

    pActor->move.vx = 0;
    pActor->move.vy = 0;
    pActor->move.vz = 0;
    pActor->moveModified.vx = 0;
    pActor->moveModified.vy = 0;
    pActor->moveModified.vz = 0;
    pActor->unkD0.vx = 0;
    pActor->unkD0.vy = 0;
    pActor->unkD0.vz = 0;
    pActor->unk60.vy = 0;
    pActor->unk60.vx = 0;
    pActor->unk60.vz = 0;
    *(u32*)(pSpriteData + 0x0C) = 0;
    *(u32*)(pSpriteData + 0x10) = 0;
    *(u32*)(pSpriteData + 0x14) = 0;

    /* 8009E7C0-8009E7E4 */
    pActor->unkF0 = 0;
    pActor->unkEC = 0;
    pActor->curYPos = (u16)*(s16*)((u8*)pActor + 0x26);
    pActor->scriptFlags.flags = (pActor->scriptFlags.flags & 0xFFFBFFFF) | 0x400000;
}
#endif /* XENO_PC_PORT */

#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc6", func_8009E810);
#else
void func_8009E810(s16 arg0) {
    /* Retail 8009E810-8009E838: sw a0<<16 -> +0x24, sh a0 -> +0xEC, +0x72
     * (only the low halfword of a0 reaches memory, so s16 is exact). */
    g_FieldScriptVMCurActor->position.vy = (s32)((u32)(s32)arg0 << 16);
    g_FieldScriptVMCurActor->unkEC = arg0;
    g_FieldScriptVMCurActor->curYPos = arg0;
#ifdef XENO_PC_PORT
    /* PORT-ONLY DIVERGENCE (kept deliberately, not retail): four extra
     * stores -- FieldActor[D_800AFD1C] +0x24 and +0x44 (s32 arg0), sprite
     * +0x84 (u16 arg0) and sprite +0x04 (position.vy) -- see the f2 misc6
     * retail differential, which checks exactly these four. */
    {
        FieldActor* pFieldActor = &g_FieldActors[D_800AFD1C];
        u8* pSpriteData = (u8*)(uintptr_t)pFieldActor->pSpriteData;

        /* Opcode 0x1D sets XZ through func_8009E574 then overrides Y here.
         * Mirror the FieldActor/sprite Y writes 9E574 already did for the
         * walkmesh height so follow-cam and model matrices stay on the
         * scripted height (MAP16 player was left at faPos.y=0). */
        pFieldActor->transformMatrix.t[1] = arg0;
        pFieldActor->childMatrix.t[1] = arg0;
        if (pSpriteData != NULL) {
            *(s16*)(pSpriteData + 0x84) = (u16)arg0;
            *(u32*)(pSpriteData + 0x04) = (u32)g_FieldScriptVMCurActor->position.vy;
        }
    }
#endif
}
#endif /* XENO_PC_PORT */

void func_8009E83C(void) {
    unsigned char width;
    unsigned char zWidth;
    unsigned char height;
    unsigned char solidRange;

    width = SCRIPT_READ_U8_REL(1);
    if (width) {
        g_FieldScriptVMCurActor->width = width * 2;
    }
    
    zWidth = SCRIPT_READ_U8_REL(2);
    if (zWidth) {
        g_FieldScriptVMCurActor->zWidth = zWidth * 2;
    }
    
    height = SCRIPT_READ_U8_REL(3);
    if (height) {
        g_FieldScriptVMCurActor->height = height * 2;
    }
    
    solidRange = SCRIPT_READ_U8_REL(4);
    if (solidRange) {
        g_FieldScriptVMCurActor->solidRange = SCRIPT_READ_U8_REL(4) * 2;
    }
    
    g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
}

/* Field-script opcode 0x17: preserve eight decoded script arguments in the
 * actor-owned 16-byte state block.  The block is also included in the field
 * save/restore image when bit 0x1000 of ActorData+0x12C is set.
 * asm/field/nonmatchings/main/misc6/func_8009E91C.s */
typedef struct {
    u16 args[8];
} FieldScriptArgs;

/* ActorData's recovered bitfields split the original word at +0x12C.  Keep a
 * tiny overlay for this opcode, which operates on the whole word. */
typedef struct {
    u8 pad[0x12C];
    u32 flags;
} FieldScriptActorFlags;

void func_8009E91C(void) {
    if (!(((FieldScriptActorFlags*)g_FieldScriptVMCurActor)->flags & 0x1000)) {
        g_FieldScriptVMCurActor->unk114 =
            (u32)(uintptr_t)HeapAlloc(0x10, 0);
    }

    ((FieldScriptActorFlags*)g_FieldScriptVMCurActor)->flags |= 0x1000;
    ((FieldScriptArgs*)(uintptr_t)g_FieldScriptVMCurActor->unk114)->args[0] =
        FieldScriptArgument1(1, SCRIPT_READ_U8_REL(0x11));
    ((FieldScriptArgs*)(uintptr_t)g_FieldScriptVMCurActor->unk114)->args[1] =
        FieldScriptArgument2(3, SCRIPT_READ_U8_REL(0x11));
    ((FieldScriptArgs*)(uintptr_t)g_FieldScriptVMCurActor->unk114)->args[2] =
        FieldScriptArgument3(5, SCRIPT_READ_U8_REL(0x11));
    ((FieldScriptArgs*)(uintptr_t)g_FieldScriptVMCurActor->unk114)->args[3] =
        FieldScriptArgument4(7, SCRIPT_READ_U8_REL(0x11));
    ((FieldScriptArgs*)(uintptr_t)g_FieldScriptVMCurActor->unk114)->args[4] =
        FieldScriptArgument5(9, SCRIPT_READ_U8_REL(0x11));
    ((FieldScriptArgs*)(uintptr_t)g_FieldScriptVMCurActor->unk114)->args[5] =
        FieldScriptArgument6(0xB, SCRIPT_READ_U8_REL(0x11));
    ((FieldScriptArgs*)(uintptr_t)g_FieldScriptVMCurActor->unk114)->args[6] =
        FieldScriptArgument7(0xD, SCRIPT_READ_U8_REL(0x11));
    ((FieldScriptArgs*)(uintptr_t)g_FieldScriptVMCurActor->unk114)->args[7] =
        FieldScriptArgument8(0xF, SCRIPT_READ_U8_REL(0x11));
    g_FieldScriptVMCurActor->scriptInstructionPointer += 0x12;
}

// -1: Actor has a script slot occupied w/ target ID
//  0: Actor has no script slot w/ target ID
int FieldActorGetScriptStatus(ActorData* pActor, int targetId) {
    int i;

    for (i = 0; i < ACTOR_MAX_NUM_SCRIPTS; i++) {
        if (targetId == pActor->scripts[i].scriptId) {
            return -1;
        }
    }
    return 0;
}

void func_8009EB78(void) {
    ActorData* pActor;
    int actorIndex;
    int i;
    int curScriptId;

    if (FieldScriptVMGetActorIndex(1) != 0xFF) {
        actorIndex = FieldScriptVMGetActorIndex(1);
        pActor = (ActorData*)(uintptr_t)g_FieldActors[actorIndex].pActorData;
        if (pActor->flags & 0x100000) {
            curScriptId = g_FieldScriptVMCurActor->curScriptIndex;
            g_FieldScriptVMCurActor->scripts[curScriptId].state = SCRIPT_STATE_IDLE;
            pActor->scripts[g_FieldScriptVMCurActor->unkCF].isInUse = 0;
        } else if (FieldActorGetScriptStatus(pActor, SCRIPT_READ_U8_REL(2) & 0x1F) != ACTOR_SCRIPT_EXISTS) {
            for (i = 0; i < ACTOR_MAX_NUM_SCRIPTS; i++) {
                if (pActor->scripts[i].flags_0x12 != 0xF || pActor->scripts[i].isInUse) {
                    continue;
                }
                
                pActor->scripts[i].currentIP = FieldScriptGetBytecodeOffset(actorIndex, SCRIPT_READ_U8_REL(2) & 0x1F);
                pActor->scripts[i].flags_0x12 = SCRIPT_READ_U8_REL(2) >> 0x5;
                pActor->scripts[i].scriptId = SCRIPT_READ_U8_REL(2) & 0x1F;
                g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
                return;
            }
            return;
        }
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void func_8009ED68(void) {
    ActorData* pActor;
    int actorIndex;
    int i;

    if (FieldScriptVMGetActorIndex(1) == ACTOR_ID_INVALID) {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
        return;
    }
    
    actorIndex = FieldScriptVMGetActorIndex(1);
    pActor = (ActorData*)(uintptr_t)g_FieldActors[actorIndex].pActorData;
    if (pActor->flags & 0x100000) {
        g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].state = SCRIPT_STATE_IDLE;
        pActor->scripts[g_FieldScriptVMCurActor->unkCF].isInUse = 0;
    } else {
        switch (g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].state) {
        case 0:
            if (FieldActorGetScriptStatus(pActor, SCRIPT_READ_U8_REL(2) & 0x1F) == ACTOR_SCRIPT_EXISTS) {
                break;
            }
            
            for (i = 0; i < ACTOR_MAX_NUM_SCRIPTS; i++) {
                if (pActor->scripts[i].flags_0x12 != 0xF || pActor->scripts[i].isInUse) {
                    continue;
                }
                pActor->scripts[i].currentIP = FieldScriptGetBytecodeOffset(actorIndex, SCRIPT_READ_U8_REL(2) & 0x1F);
                pActor->scripts[i].flags_0x12 = SCRIPT_READ_U8_REL(2) >> 5;
                pActor->scripts[g_FieldScriptVMCurActor->unkCF].isInUse = 1;
                pActor->scripts[i].scriptId = SCRIPT_READ_U8_REL(2) & 0x1F;
                g_FieldScriptVMCurActor->unkCF = i;
                g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].state = 1;
                break;
            }
            return;
        case 1:
            if ((pActor->curScriptIndex == g_FieldScriptVMCurActor->unkCF) || pActor->scripts[g_FieldScriptVMCurActor->unkCF].flags_0x12 == 0xF) {
                g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
                g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].state = SCRIPT_STATE_IDLE;
                pActor->scripts[g_FieldScriptVMCurActor->unkCF].isInUse = 0;
            } else {
                D_800B00C0 = g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].state;
            }
            return;
        default:
            return;
        }
    }
    
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void func_8009F0A0(void) {
    ActorData* pActor;
    s32 actorIndex;
    int i;

    if (FieldScriptVMGetActorIndex(1) == ACTOR_ID_INVALID) {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
        return;
    }
    
    actorIndex = FieldScriptVMGetActorIndex(1);
    pActor = (ActorData*)(uintptr_t)g_FieldActors[actorIndex].pActorData;
    if (pActor->flags & 0x100000) {
        g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].state = SCRIPT_STATE_IDLE;
        pActor->scripts[g_FieldScriptVMCurActor->unkCF].isInUse = 0;
    } else {
        switch (g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].state) {
        case 0:
            if (FieldActorGetScriptStatus(pActor, SCRIPT_READ_U8_REL(2) & 0x1F) == ACTOR_SCRIPT_EXISTS) {
                g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
                break;
            }
            
            for (i = 0; i < ACTOR_MAX_NUM_SCRIPTS; i++) {
                if (pActor->scripts[i].flags_0x12 != 0xF || pActor->scripts[i].isInUse) {
                    continue;
                }
                pActor->scripts[i].currentIP = FieldScriptGetBytecodeOffset(actorIndex, SCRIPT_READ_U8_REL(2) & 0x1F);
                pActor->scripts[i].flags_0x12 = SCRIPT_READ_U8_REL(2) >> 5;
                pActor->scripts[g_FieldScriptVMCurActor->unkCF].isInUse = 1;
                g_FieldScriptVMCurActor->unkCF = i;
                g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].state = 1;
                pActor->scripts[i].scriptId = SCRIPT_READ_U8_REL(2) & 0x1F;
                return;
            }
            break;
        case 1:
            if ((pActor->curScriptIndex == g_FieldScriptVMCurActor->unkCF) || pActor->scripts[g_FieldScriptVMCurActor->unkCF].flags_0x12 == 0xF) {
                g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].state = 2;
                return;
            }
            D_800B00C0 = 1;
            break;
        case 2:
            if (pActor->scripts[g_FieldScriptVMCurActor->unkCF].flags_0x12 == 0xF) {
                g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].state = SCRIPT_STATE_IDLE;
                pActor->scripts[g_FieldScriptVMCurActor->unkCF].isInUse = 0;
                g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
                return;
            }
            D_800B00C0 = 1;
            break;
        }
        return;
    }
    
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

// Randomize X rotation
void func_8009F424(void) {
    int nRotation;
    
    nRotation = g_FieldScriptVMCurActor->rotation.vy;
    g_FieldScriptVMCurActor->unk102++;
    if (!(g_FieldScriptVMCurActor->unk102 & 0xF)) {
        if (!(rand() & 0x1)) {
            nRotation = PSX_ANGLE(g_FieldScriptVMCurActor->rotation.vy + PSX_DEGREES(45));
        } else {
            nRotation = PSX_ANGLE(g_FieldScriptVMCurActor->rotation.vy - PSX_DEGREES(45));
        }
    }
    D_800B00C0 = 1;
    g_FieldScriptVMCurActor->rotation.vx = nRotation;
    g_FieldScriptVMCurActor->scriptInstructionPointer++;
}

void func_8009F4CC(void) {
    short nSpecialRotation;
    int nRotation;
    int nRand;
    int nDeltaRotation;
    int delta;

    nRotation = g_FieldScriptVMCurActor->rotation.vy;
    g_FieldScriptVMCurActor->unk102++;
    if (!(g_FieldScriptVMCurActor->unk102 & 0xF)) {
        nRand = rand();
        if (nRand & 0x30) {
            nSpecialRotation = g_FieldScriptVMCurActor->rotation.vy | ~0x7FFF;
            nRotation = nSpecialRotation;
            g_FieldScriptVMCurActor->rotation.vy = nSpecialRotation;
        } else {
            delta = PSX_DEGREES(45);
            if (!(nRand & 1)) {
                nDeltaRotation = g_FieldScriptVMCurActor->rotation.vy + delta;
            } else {
                nDeltaRotation = g_FieldScriptVMCurActor->rotation.vy - delta;
            }
            nRotation = PSX_ANGLE(nDeltaRotation);
        }
    }
    D_800B00C0 = 1;
    g_FieldScriptVMCurActor->rotation.vx = nRotation;
    g_FieldScriptVMCurActor->scriptInstructionPointer++;
}

void func_8009F5A8(void) {
    unsigned short nStoredIP = g_FieldScriptVMCurActor->scriptInstructionPointer;
    func_8009F5F4(); // Do Encounter
    D_800B00C0 = 1;
    g_FieldScriptVMCurActor->scriptInstructionPointer = nStoredIP;
}


extern u16 D_800AFE9C;      /* held-button field mask (d-pad in top nibble) */
extern s32 D_800ADB68;      /* playerCanRun flag */
extern s16 D_800ADB02;      /* stuck-frame counter */
extern s32 D_800ADB64;      /* active field/menu owner; 0xFF means none */
extern s32 D_800ADB28;      /* interaction trail cursor snapshot */
extern s16 D_800B2340;      /* alternate-interaction cooldown reload */
extern s16 D_800B2342;      /* alternate-interaction cooldown */
extern s16 D_800B2344;      /* alternate interaction mode */
extern s32 D_800B2360;      /* player trail cursor */
extern u8  D_800B2354;      /* selects which d-pad->angle table */
extern u16 D_800ADF68[];    /* d-pad -> angle table 1 (16 entries) */
extern u16 D_800ADF88[];    /* d-pad -> angle table 2 (16 entries) */
extern u16 D_800C2694;      /* newly pressed field-button mask */
extern void func_80079288(void); /* checkForRandomEncounter (side behavior) */
#ifdef XENO_PC_PORT
/* Host route-testing switch (god_mode.c); weak so tests that link this TU
 * without it keep the retail behaviour (encounters enabled). */
extern int PcPort_RandomBattlesEnabled(void) __attribute__((weak));
#define XENO_RANDOM_BATTLES_ON() (!PcPort_RandomBattlesEnabled || PcPort_RandomBattlesEnabled())
#endif
extern s32 func_80081F5C(u32*);   /* interaction eligibility */
#ifdef XENO_PC_PORT
#include "test_input.h"
#endif

/* func_8009F5F4 = OP_UPDATE_CHARACTER (opcode 0xA7). Called every frame for the
 * player via the idle-hold wrapper func_8009F5A8. Retail asm 8009F5F4-8009F9FC;
 * Noah OP_UPDATE_CHARACTER is the readable reference. The 0x4000 test is on
 * scriptFlags at offset 0x00 (asm lw 0x0($a0)), NOT the ->flags field at 0x04
 * that the prior partial port checked. */
#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): field_event_request_player_control (src/decomp/src/field/field_event.c:9626). */
#ifndef XM_T_WindowLine
#define XM_T_WindowLine
typedef struct {
    SPRT sprite[2][2];
    RECT rect;       
    s16 width;       
    u8 plane;        
    u8 slot;
    u8 row;          
    u8 unk5D;
    u16 clut;
} WindowLine;
#endif
#ifndef XM_T_WindowQueue
#define XM_T_WindowQueue
typedef struct WindowQueue {
    struct WindowQueue *next;
    s32 message;
} WindowQueue;
#endif
#ifndef XM_T_Window
#define XM_T_Window
typedef struct Window {
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
    WindowLine *layout;
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
    WindowQueue *queue;
} Window;
#endif
#ifndef XM_T_DialogueFrame
#define XM_T_DialogueFrame
typedef struct {
    RECT rect;                  
    u8 unk08[0x18 - 0x08];
    DR_MODE back_modes[2];      
    TILE back[2];               
    DR_MODE border_modes[2][10]; 
    SPRT border[2][10];         
} DialogueFrame;
#endif
#ifndef XM_T_DialogueChoice
#define XM_T_DialogueChoice
typedef struct {
    s16 status;       
    s16 first;        
    s16 count;        
    s16 index;        
    DR_MODE modes[2]; 
    SPRT cursor[2];   
} DialogueChoice;
#endif
#ifndef XM_T_DialoguePrompt
#define XM_T_DialoguePrompt
typedef struct {
    s16 status;       
    u8 unk02[2];
    DR_MODE modes[2]; 
    SPRT sprite[2];   
} DialoguePrompt;
#endif
#ifndef XM_T_Fixed
#define XM_T_Fixed
typedef union {
    s32 value;
    struct {
        u16 fraction;
        s16 whole;
    } part;
} Fixed;
#endif
#ifndef XM_T_DialogueWindow
#define XM_T_DialogueWindow
typedef struct DialogueWindow {
    DR_MODE modes[2]; 
    Window text;     
    s32 message;     
    DialogueFrame frame; 
    DialogueChoice choice; 
    DialoguePrompt prompt; 
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
    Fixed slide[2];  
    s32 slide_step[2]; 
    DR_MODE icon_modes[2]; 
    POLY_FT4 icon[2];  
    u8 unk494;       
    u8 unk495;       
    u8 unk496[0x498 - 0x496];
} DialogueWindow;
#endif
#ifndef XM_T_FadeChannel
#define XM_T_FadeChannel
typedef struct {
    DR_MODE modes[2]; 
    TILE tiles[2];   
    s32 level[3];
    s32 step[3];
    u16 abr;
    s16 active;
    s16 steps;
    u16 pad;
} FadeChannel;
#endif
#ifndef XM_T_FieldWork
#define XM_T_FieldWork
typedef struct FieldWork {
    s16 unk2078;               
    s16 unk207A;               
    s16 effect_steps;          
    u8 unk207E[2];
    s32 effect_value[6];       
    s32 effect_step[6];        
    s16 unk20B0[2];            
    void *effect_buffers[4];   
    FadeChannel fades[2];      
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
} FieldWork;
#endif
#ifndef XM_T_ACTOR_CACHED_POSITION
#define XM_T_ACTOR_CACHED_POSITION
#define ACTOR_CACHED_POSITION(actor) ((s16 *)((u8 *)(actor) + 0x68))
#endif
#ifndef XM_T_WHOLE
#define XM_T_WHOLE
#define WHOLE(value) (((Fixed *)&(value))->part.whole)
#endif
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
#ifndef XM_S_field_event_request_player_control
#define XM_S_field_event_request_player_control
void xm_field_event_request_player_control(void) __asm__("func_8009F5F4");
#endif
#ifndef XM_S_field_dialogue_windows
#define XM_S_field_dialogue_windows
extern DialogueWindow xm_field_dialogue_windows[4] __asm__("g_FieldTextBoxes");
#endif
#ifndef XM_S_field_work
#define XM_S_field_work
extern FieldWork xm_field_work __asm__("g_FieldEffects");
#endif
#ifndef XM_S_field_pad_port0_held
#define XM_S_field_pad_port0_held
extern u16 xm_field_pad_port0_held __asm__("D_800AFE9C");
#endif
#ifndef XM_S_field_encounter_count_down
#define XM_S_field_encounter_count_down
s32 xm_field_encounter_count_down(void) __asm__("func_80079288");
#endif
#ifndef XM_S_field_player_control_polled
#define XM_S_field_player_control_polled
extern s32 xm_field_player_control_polled __asm__("D_800ADB68");
#endif
#ifndef XM_S_field_player_stuck_frames
#define XM_S_field_player_stuck_frames
extern s16 xm_field_player_stuck_frames __asm__("D_800ADB02");
#endif
#ifndef XM_S_field_menu_request
#define XM_S_field_menu_request
extern s32 xm_field_menu_request __asm__("D_800ADB64");
#endif
#ifndef XM_S_field_pad_port0_pressed
#define XM_S_field_pad_port0_pressed
extern u16 xm_field_pad_port0_pressed __asm__("D_800C2694");
#endif
#ifndef XM_S_field_actor_is_jump_blocked_by_floor
#define XM_S_field_actor_is_jump_blocked_by_floor
s32 xm_field_actor_is_jump_blocked_by_floor(FieldActor *actor) __asm__("func_80081F5C");
#endif
#ifndef XM_S_field_unread_jump_start_history_index
#define XM_S_field_unread_jump_start_history_index
extern s32 xm_field_unread_jump_start_history_index __asm__("D_800ADB28");
#endif
#ifndef XM_S_field_movement_history_indices
#define XM_S_field_movement_history_indices
extern s32 xm_field_movement_history_indices[3] __asm__("D_800B2360");
#endif
#ifndef XM_S_field_dpad_headings
#define XM_S_field_dpad_headings
extern u16 xm_field_dpad_headings[16] __asm__("D_800ADF68");
#endif
#ifndef XM_S_field_dpad_alt_headings
#define XM_S_field_dpad_alt_headings
extern u16 xm_field_dpad_alt_headings[16] __asm__("D_800ADF88");
#endif
void func_8009F5F4(void) {
    u8 unused[0x48]; 
    s32 i;
    s32 idle;
    s32 direction = 0;

    if (xm_field_current_event_actor->flags & 0x4000) {
        for (i = 0; i < 4; i++) {
            if (xm_field_dialogue_windows[i].choice.status == 0) {
                break;
            }
        }
        idle = (i == 4) ? -1 : 0;
        if (idle == -1 && xm_field_work.encounter_inhibition == 0) {
            if (xm_field_pad_port0_held >> 12) {
                xm_field_encounter_count_down();
            }
            xm_field_player_control_polled = 1;
            if (xm_field_current_event_actor->floor_attribute & 0x400000) {
                if (ACTOR_CACHED_POSITION(xm_field_current_event_actor)[0] == WHOLE(xm_field_current_event_actor->position[0])
                    && ACTOR_CACHED_POSITION(xm_field_current_event_actor)[1] == WHOLE(xm_field_current_event_actor->position[1])
                    && ACTOR_CACHED_POSITION(xm_field_current_event_actor)[2] == WHOLE(xm_field_current_event_actor->position[2])) {
                    xm_field_player_stuck_frames++;
                }
            } else {
                xm_field_player_stuck_frames = 0;
            }
            if (xm_field_player_stuck_frames > 32 && (xm_field_player_stuck_frames = 32, xm_field_pad_port0_held & 0x80) && !(xm_field_current_event_actor->flags & 0x1800) && xm_field_menu_request == 0xFF) {
                goto jump;
            }
            if (xm_field_work.jump_mode == 0) {
                if ((xm_field_pad_port0_pressed & 0x80) && !(xm_field_current_event_actor->flags & 0x1800) && !(xm_field_current_event_actor->floor_attribute & 0x400000) && xm_field_menu_request == 0xFF) {
                jump:
                    if (xm_field_actor_is_jump_blocked_by_floor(xm_field_current_event_actor) == 0) {
                        xm_field_current_event_actor->flags |= 0x800;
                        xm_field_unread_jump_start_history_index = xm_field_movement_history_indices[0];
                    }
                }
            } else {
                if (xm_field_pad_port0_pressed & 0x80) {
                    if (xm_field_work.repeat_remaining != 0) {
                        goto count;
                    }
                    if (xm_field_menu_request == 0xFF && xm_field_actor_is_jump_blocked_by_floor(xm_field_current_event_actor) == 0) {
                        xm_field_current_event_actor->flags |= 0x800;
                        xm_field_unread_jump_start_history_index = xm_field_movement_history_indices[0];
                        xm_field_current_event_actor->unkE8 = 0xFF;
                        xm_field_work.repeat_remaining = xm_field_work.repeat_delay;
                    }
                }
                if (xm_field_work.repeat_remaining != 0) {
                count:
                    xm_field_work.repeat_remaining--;
                }
            }
            if (xm_field_work.unk2354 == 0) {
                direction = xm_field_dpad_headings[(xm_field_pad_port0_held >> 12) ^ 0xF];
            } else {
                direction = xm_field_dpad_alt_headings[(xm_field_pad_port0_held >> 12) ^ 0xF];
            }
            if (!(direction & 0x8000)) {
                direction = (direction - xm_field_view.angle) & 0xFFF;
            }
            xm_field_current_event_actor->heading = direction;
        } else {
            xm_field_current_event_actor->heading = direction | 0x8000;
        }
    } else if (xm_field_work.preserve_nonplayer_motion == 0) {
        xm_field_current_event_actor->flags |= 0x1000000;
    }
    xm_field_current_event_actor->pc += 1;
}
#endif
#else
void func_8009F5F4(void) {
#ifdef XENO_PC_PORT
    /* TEST TOOLING (XENO_TEST_INPUT scripted held-button schedule): inject the
     * held mask and synthesize its newly-pressed edges into D_800C2694.  Retail
     * has no such prologue, so it runs only while that schedule is active
     * (PcPort_TestInputInit enables injection iff XENO_TEST_INPUT is set; an
     * invalid schedule aborts startup).  With it unset this opcode is
     * retail-faithful: no D_800C2694 write and no edge latch state. */
    {
        static int s_testInputActive = -1;

        if (s_testInputActive < 0) {
            s_testInputActive = getenv("XENO_TEST_INPUT") != NULL;
        }
        if (s_testInputActive) {
            static u16 s_prevHeld;
            u16 rising;
            PcPort_TestInputInject(&D_800AFE9C);
            rising = (u16)(D_800AFE9C & (u16)~s_prevHeld);
            D_800C2694 |= rising;
            s_prevHeld = D_800AFE9C;
        }
    }
#endif
    u8* p = (u8*)(uintptr_t)g_FieldScriptVMCurActor;
    u32 scriptFlags = *(u32*)(p + 0x00);

    if (scriptFlags & 0x4000) {
        /* ---- player-controlled branch (asm 8009F614-8009F9A4) ---- */
        s32 i;
        s32 interactionResult;

        /* Dialog-window gate: idle if any text box is in use (m37C == 0) or the
         * field battle/control var is set (asm 8009F620-8009F66C). */
        for (i = 0; i < 4; i++) {
            if (*(s16*)((u8*)&g_FieldTextBoxes + 0x37C + i * 0x498) == 0) {
                break;
            }
        }
        if (i != 4 || *(s16*)((u8*)&g_FieldControl) != 0) {
            *(s16*)(p + 0x104) = (s16)0x8000; /* idle angle (asm .L8009F9A8, a1=0) */
            g_FieldScriptVMCurActor->scriptInstructionPointer++;
            return;
        }

        /* Random-encounter step roll when a direction is held (asm 8009F674-90).
         * The port can suppress it for route testing (host-side flag only, so
         * the matching build keeps the retail call). */
#ifdef XENO_PC_PORT
        if (XENO_RANDOM_BATTLES_ON() && (D_800AFE9C >> 12) != 0) {
#else
        if ((D_800AFE9C >> 12) != 0) {
#endif
            func_80079288();
        }

        D_800ADB68 = 1; /* playerCanRun (asm 8009F6A0) */

        /* Stuck detection (asm 8009F6A8-8009F71C): count frames where a
         * 0x400000 move is requested but the position did not change. */
        if (*(u32*)(p + 0x14) & 0x00400000) {
            if (*(s16*)(p + 0x68) == *(s16*)(p + 0x22) &&
                *(s16*)(p + 0x6A) == *(s16*)(p + 0x26) &&
                *(s16*)(p + 0x6C) == *(s16*)(p + 0x2A)) {
                D_800ADB02 = (s16)((u16)D_800ADB02 + 1);
            }
        } else {
            D_800ADB02 = 0;
        }

        /* Stuck-clamp + jump/talk interaction (asm .L8009F720-.L8009F90C).
         * A held interaction button is retried after 33 blocked frames.  The
         * ordinary path consumes a newly-pressed edge and rejects actors that
         * are already script-owned, moving, or under another field owner. */
        if (D_800ADB02 >= 0x21) {
            D_800ADB02 = 0x20;
            if ((D_800AFE9C & 0x80) != 0 &&
                (g_FieldScriptVMCurActor->scriptFlags.flags & 0x1800) == 0 &&
                D_800ADB64 == 0xFF) {
                goto attemptInteraction;
            }
        }

        if (D_800B2344 != 0) {
            if ((D_800C2694 & 0x80) != 0) {
                if (D_800B2342 != 0) {
                    goto decrementInteractionCooldown;
                }
                if (D_800ADB64 == 0xFF) {
                    interactionResult =
                        func_80081F5C((u32*)g_FieldScriptVMCurActor);
#ifdef FIELD_VM_AUDIT_MUTANT_UPDATE_CHARACTER_INVERT_RESULT
                    interactionResult = interactionResult == 0 ? -1 : 0;
#endif
                    if (interactionResult == 0) {
                        g_FieldScriptVMCurActor->scriptFlags.flags |= 0x800;
                        D_800ADB28 = D_800B2360;
                        g_FieldScriptVMCurActor->curAnimationId = 0xFF;
                        D_800B2342 = (s16)(u16)D_800B2340;
                    }
                }
            }
            if (D_800B2342 != 0) {
decrementInteractionCooldown:
#ifndef FIELD_VM_AUDIT_MUTANT_UPDATE_CHARACTER_SKIP_COOLDOWN
                D_800B2342 = (s16)((u16)D_800B2342 - 1);
#endif
            }
            goto updateDirection;
        }

        if ((D_800C2694 & 0x80) == 0 ||
            (g_FieldScriptVMCurActor->scriptFlags.flags & 0x1800) != 0 ||
#ifndef FIELD_VM_AUDIT_MUTANT_UPDATE_CHARACTER_ALLOW_MATERIAL
            (g_FieldScriptVMCurActor->curWalkmeshTriMaterial & 0x400000) != 0 ||
#endif
            D_800ADB64 != 0xFF) {
            goto updateDirection;
        }

attemptInteraction:
        interactionResult = func_80081F5C((u32*)g_FieldScriptVMCurActor);
#ifdef FIELD_VM_AUDIT_MUTANT_UPDATE_CHARACTER_INVERT_RESULT
        interactionResult = interactionResult == 0 ? -1 : 0;
#endif
        if (interactionResult != 0) {
            goto updateDirection;
        }
        g_FieldScriptVMCurActor->scriptFlags.flags |= 0x800;
        D_800ADB28 = D_800B2360;

updateDirection:

        /* Direction from d-pad + camera angle (asm .L8009F910-8009F9A4). */
        {
            u16 nibble = (u16)(D_800AFE9C >> 12);
            u16 angle;
            if (D_800B2354 == 0) {
                angle = D_800ADF68[nibble ^ 0xF];
            } else {
                angle = D_800ADF88[nibble ^ 0xF];
            }
            if ((angle & 0x8000) == 0) {
                angle = (u16)((angle - *(s16*)((u8*)&g_CamInterpolation + 0x8)) & 0xFFF);
            }
            *(s16*)(p + 0x104) = (s16)angle;
        }
        g_FieldScriptVMCurActor->scriptInstructionPointer++;
        return;
    }

    /* ---- not-player-controlled idle branch (asm .L8009F9BC) ----
     * Writes scriptFlags at 0x00 (asm sw 0x0($a0)), not ->flags at 0x04. */
    if (D_800B21CE == 0) {
        *(u32*)(p + 0x00) = scriptFlags | 0x1000000;
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer++;
}
#endif /* XENO_PC_PORT */

int FieldCharacterIdToPartyId(int characterId) {
    int i;

    if (characterId == CHARACTER_ID_NONE) {
        return -1;
    }
    
    for (i = 0; i < 3; i++) {
        if (g_GamePartyMembers[i] == CHARACTER_ID_NONE)
            return -1;
        
        if (g_GamePartyMembers[i] == characterId) {
            return i;
        }
    }

    return -1;
}

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): field_event_place_at_map_entry (src/decomp/src/field/field_event.c:9724). */
#ifndef XM_S_field_event_place_at_map_entry
#define XM_S_field_event_place_at_map_entry
s32 xm_field_event_place_at_map_entry(s32 entry) __asm__("func_8009FA54");
#endif
#ifndef XM_S_field_event_bytecode
#define XM_S_field_event_bytecode
extern u8 *xm_field_event_bytecode __asm__("g_FieldScriptVMCurScriptData");
#endif
#ifndef XM_S_field_event_read_code_half
#define XM_S_field_event_read_code_half
s16 xm_field_event_read_code_half(s32 offset) __asm__("func_8009E330");
#endif
#ifndef XM_S_field_event_read_variable
#define XM_S_field_event_read_variable
s32 xm_field_event_read_variable(s32 reference) __asm__("FieldScriptVMGetVariableValue");
#endif
s32 xm_field_event_place_at_map_entry(s32 entry) __asm__("func_8009FA54");
s32 xm_field_event_place_at_map_entry(s32 entry) {
    s32 marker;
    s32 record;
    s32 x;
    s32 angle;

    marker = xm_field_event_bytecode[0];
    if (marker != 0xFF) {
        return 0;
    }
    record = entry * 7;
    xm_field_current_event_actor->layer = xm_field_event_bytecode[record + 5];
    x = xm_field_event_read_code_half(record + 1);
    xm_field_event_place_actor_on_floor(x, xm_field_event_read_code_half(record + 3));
    angle = ((xm_field_event_bytecode[record + 6] + 4) & 7) << 9;
    if (xm_field_event_bytecode[record + 6] == marker) {
        angle = ((xm_field_event_read_variable(8) + 4) & 7) << 9;
    }
    xm_field_view.heading_angles.vy = angle;
    xm_field_view.heading = (s16)angle;
    xm_field_view.heading_high = angle << 16;
    angle = (((xm_field_event_bytecode[record + 7] - 2) & 7) << 9) | 0x8000;
    if (xm_field_event_bytecode[record + 7] == marker) {
        angle = (((xm_field_event_read_variable(6) - 2) & 7) << 9) | 0x8000;
    }
    xm_field_current_event_actor->unk108 = xm_field_current_event_actor->heading_goal = xm_field_current_event_actor->heading = angle;
    return 0;
}
#endif
#else
void func_8009FA54(s32 scriptEntryIndex) {
    u8* scriptData = (u8*)g_FieldScriptVMCurScriptData;
    s32 entryOffset;
    s16 x;
    s16 z;
    u8 rotByte;
    s16 angle;

    if (scriptData[0] != 0xFF) {
#ifdef XENO_PC_PORT
        if (getenv("XENO_FIELD_DIAG") != NULL) {
            printf("[field-diag] spawn-skip marker=0x%02x entry=%d\n",
                   scriptData[0], (int)scriptEntryIndex);
        }
#endif
        return;
    }

    entryOffset = scriptEntryIndex * 7;
    g_FieldScriptVMCurActor->walkmeshId = scriptData[entryOffset + 5];

    x = func_8009E330(entryOffset + 1);
    z = func_8009E330(entryOffset + 3);
#ifdef XENO_PC_PORT
    if (getenv("XENO_FIELD_DIAG") != NULL) {
        printf("[field-diag] spawn-apply actor=%d entry=%d x=%d z=%d wm=%d\n",
               (int)D_800AFD1C, (int)scriptEntryIndex, (int)x, (int)z,
               (int)scriptData[entryOffset + 5]);
    }
#endif
    func_8009E574(x, z);

    rotByte = scriptData[entryOffset + 6];
    if (rotByte == 0xFF) {
        rotByte = FieldScriptVMGetVariableValue(0x8) + 4;
    } else {
        rotByte += 4;
    }
    angle = (rotByte & 0x7) << 9;
    *(s16*)((u8*)&g_Scene + 0x56) = angle;
    *(s32*)((u8*)&g_Scene + 0x7C) = angle;
    *(s32*)((u8*)&g_Scene + 0x60) = angle << 16;

    rotByte = scriptData[entryOffset + 7];
    if (rotByte == 0xFF) {
        rotByte = FieldScriptVMGetVariableValue(0x6) - 2;
    } else {
        rotByte -= 2;
    }
    angle = ((rotByte & 0x7) << 9) | 0x8000;
    g_FieldScriptVMCurActor->rotation.vx = angle;
    g_FieldScriptVMCurActor->rotation.vy = angle;
    g_FieldScriptVMCurActor->rotation.vz = angle;
}
#endif /* XENO_PC_PORT */

extern int g_GameSceneMapNum;

void func_8009FB98(void) {
#ifdef XENO_PC_PORT
    g_GameSceneMapNum |= 0xC000;
    GameWaitForCdData();
    GamePartySyncSkinData();
    GamePartySyncStreamedData();
    D_800B2268 = SCRIPT_READ_U8_REL(1);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
#else
    /* Plain-pointer IP access: as a struct field the IP store is scheduled above
     * the D_800B2268 store (gcc 2.7.2 struct/non-struct alias rule). */
    g_GameSceneMapNum |= 0xC000;
    GameWaitForCdData();
    GamePartySyncSkinData();
    GamePartySyncStreamedData();
    D_800B2268 = SCRIPT_READ_U8_REL_RAW(1);
    *(u16*)((u8*)g_FieldScriptVMCurActor + 0xCC) += 2;
#endif
}

extern s32 D_8006F990[];
int func_8009FC10(int actorIndex) {
    int i;

    for (i = 0; i < 3; i++) {
        if (D_8006F990[i] == actorIndex) {
            return i;
        }
    }
    
    return 0xFF;
}

void FieldScriptPartyMemberRideGear(void) {
    int index = FieldScriptVMGetArgument(1);
    if (index >= 3) {
        index = 2;
    }
    g_pGameState->gearRide[index] = 1;
    func_8009FD10(index);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void FieldScriptPartyMemberDisembarkGear(void) {
    int index = FieldScriptVMGetArgument(1);
    if (index >= 3) {
        index = 2;
    }
    g_pGameState->gearRide[index] = 0;
    func_8009FD10(index);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

extern int g_GameSceneMapNum;
void func_8009FD10(int partyMemberIndex) {
    switch (partyMemberIndex) { 
        case 0:
            FieldScriptMemoryWriteU16(0x2A, g_GameSceneMapNum & 0xFFF);
            FieldScriptMemoryWriteU16(0x2C, 0);
            FieldScriptMemoryWriteU16(0x2E, 0);
            break;
        case 1:
            FieldScriptMemoryWriteU16(0x30, g_GameSceneMapNum & 0xFFF);
            FieldScriptMemoryWriteU16(0x32, 0);
            FieldScriptMemoryWriteU16(0x34, 0);
            break;
        case 2:
            FieldScriptMemoryWriteU16(0x36, g_GameSceneMapNum & 0xFFF);
            FieldScriptMemoryWriteU16(0x38, 0);
            FieldScriptMemoryWriteU16(0x3A, 0);
            break;
    }
}

void func_8009FDD4(void) {
    s32 result = func_8009FC10(D_800AFD1C);
    if (result != 0xFF) {
        u8* pGS = (u8*)g_pGameState;
        if ((result + pGS)[0x22B1] == 0) {
            func_800AD4D4(result);
        }
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 1;
}

void func_8009FE4C(void) {
    u8 slot = SCRIPT_READ_U8_REL(1);
    if (g_GamePartyMembers[slot] != 0xFF) {
        u8* pGS = (u8*)g_pGameState;
        if ((slot + pGS)[0x22B1] != 0) {
            func_800ACFD0(slot);
        }
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
}

extern s32 D_8005A448;
extern s32 D_8005A44C;

void func_8009FEE4(s32 partyMemberIndex) {
    s32* pPartyActors = D_8005A444;
    s32 actorIndex = pPartyActors[partyMemberIndex];
    s32 rotation;
    u8* pActorData;

    if (actorIndex != 0xFF) {
        __asm__ volatile("" : : : "memory");
        pActorData = (u8*)(uintptr_t)g_FieldActors[actorIndex].pActorData;
        rotation = *(s16*)(pActorData + 0x10) << 14;

        switch (partyMemberIndex) {
        case 0:
            FieldScriptMemoryWriteU16(0x2A, (g_GameSceneMapNum & 0xFFF) | rotation);
            pActorData = (u8*)(uintptr_t)g_FieldActors[pPartyActors[0]].pActorData;
            FieldScriptMemoryWriteU16(0x2C, *(s16*)(pActorData + 0x22));
            pActorData = (u8*)(uintptr_t)g_FieldActors[pPartyActors[0]].pActorData;
            FieldScriptMemoryWriteU16(0x2E, *(s16*)(pActorData + 0x2A));
            break;
        case 1:
            FieldScriptMemoryWriteU16(0x30, (g_GameSceneMapNum & 0xFFF) | rotation);
            pActorData = (u8*)(uintptr_t)g_FieldActors[D_8005A448].pActorData;
            FieldScriptMemoryWriteU16(0x32, *(s16*)(pActorData + 0x22));
            pActorData = (u8*)(uintptr_t)g_FieldActors[D_8005A448].pActorData;
            FieldScriptMemoryWriteU16(0x34, *(s16*)(pActorData + 0x2A));
            break;
        case 2:
            FieldScriptMemoryWriteU16(0x36, (g_GameSceneMapNum & 0xFFF) | rotation);
            pActorData = (u8*)(uintptr_t)g_FieldActors[D_8005A44C].pActorData;
            FieldScriptMemoryWriteU16(0x38, *(s16*)(pActorData + 0x22));
            pActorData = (u8*)(uintptr_t)g_FieldActors[D_8005A44C].pActorData;
            FieldScriptMemoryWriteU16(0x3A, *(s16*)(pActorData + 0x2A));
            break;
        }
    }
}

void func_800A0158(s32 arg0, s32* arg1, s32* arg2, s32* arg3) {
    switch (arg0) {
        case 0:
            *arg1 = FieldScriptVMGetVariableValue(0x2A);
            *arg2 = FieldScriptVMGetVariableValue(0x2C);
            *arg3 = FieldScriptVMGetVariableValue(0x2E);
            break;
        case 1:
            *arg1 = FieldScriptVMGetVariableValue(0x30);
            *arg2 = FieldScriptVMGetVariableValue(0x32);
            *arg3 = FieldScriptVMGetVariableValue(0x34);
            break;
        case 2:
            *arg1 = FieldScriptVMGetVariableValue(0x36);
            *arg2 = FieldScriptVMGetVariableValue(0x38);
            *arg3 = FieldScriptVMGetVariableValue(0x3A);
            break;
    }
}

/* Party/gear sprite-load opcode (retail 800A0228-800A051C).
 *
 * The first argument selects one of the three party slots.  A party member
 * with no gear (or a map/ride-state mismatch) takes the ordinary sprite path;
 * otherwise the gear's package is bound to the current actor and the three
 * saved position words are applied.  Keep the raw ActorData/FieldActor
 * offsets here: these are PSX pointer-slot structures and using host-layout
 * fields would move the status, script flags, and instruction pointer. */
#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): field_event_become_party_slot (src/decomp/src/field/field_event.c:9913). */
#ifndef XM_T_xm_CharacterEntry
#define XM_T_xm_CharacterEntry
typedef struct {
    u16 field0; 
    u8 value2;
    u8 value3;  
    u8 value4;  
    u8 pad5;
    u8 id;      
    u8 pad7;
} xm_CharacterEntry;
#endif
#ifndef XM_T_xm_StatusPair
#define XM_T_xm_StatusPair
typedef union {
    u32 word;
    struct {
        u16 active;
        u16 permanent;
    } half;
} xm_StatusPair;
#endif
#ifndef XM_T_xm_CharacterRecord
#define XM_T_xm_CharacterRecord
typedef struct {
    xm_CharacterEntry entries[4]; 
    u8 pad20[0x28 - 0x20];
    u8 equipAttack;       
    u8 equipDefense;      
    u8 equipSpeed;        
    u8 equipEther;        
    u8 equipEtherDefense; 
    u8 bodyDefense;       
    u8 equip5E;           
    u8 equip5F;           
    u8 hpBonus;           
    u8 epBonus;           
    u16 flags32;          
    u16 flags34;          
    u16 flags36;          
    u16 weakness;         
    u16 field3A;          
    u32 expTotalA;        
    u32 expTotalB;        
    u32 expNextA;         
    u32 expNextB;         
    u16 hp;               
    u16 maxHp;            
    u16 ep;               
    u16 maxEp;            
    u8 field54;           
    u8 field55;
    u8 characterId;       
    u8 pad57;
    u8 attack;            
    u8 defense;           
    u8 speed;             
    u8 ether;             

















    u8 etherDefense;      
    u8 field5D;
    u8 field5E;           
    u8 field5F;           
    u8 field60;           
    u8 field61;
    u8 level;             
    u8 level2;            
    u8 field64[4];        
    u8 pad68[0x6A - 0x68];
    u8 weapons[5];        
    u8 entryItems[5];     

    u8 accessories[3];    
    u8 field77;           

    u8 field78;           
    u8 field79;           
    u16 status7A;         

    u16 status7C;         


    u16 status7E;         
    u16 status80;         
    u16 status82;         
    xm_StatusPair status84;  
    xm_StatusPair status88;  
    xm_StatusPair status8C;  
    u16 useCounts[7];     
    u8 pad9E[0xA0 - 0x9E];
    u8 gearId;            
    u8 fieldA1;           
    u8 padA2[0xA4 - 0xA2];
} xm_CharacterRecord;
#endif
#ifndef XM_T_xm_GearEntry
#define XM_T_xm_GearEntry
typedef struct {
    u16 field0; 
    u8 valueE;  
    u8 value10; 
    u8 value11; 
    u8 id;      
    u8 pad6[2];
} xm_GearEntry;
#endif
#ifndef XM_T_xm_GearRecord
#define XM_T_xm_GearRecord
typedef struct xm_GearRecord {
    u8 pad0[2];
    u8 engine;            
    u8 field3;            
    u8 partItems[4];      

    u8 frame;             
    u8 parts[3];          
    u8 weapons[4];        
    xm_GearEntry entries[4]; 
    u8 pad30[0x38 - 0x30];
    u16 fuel;             
    u16 maxFuel;          
    u8 attack;            
    u8 pad3D;
    u8 field3E;
    u8 attackScale;       
    u16 equipBodyDefense; 
    u16 equipArmor;       
    u16 equip68a;         
    u16 equip68b;         
    u16 field48;          
    u8 speedPenalty;      
    u8 pad4B;
    u8 equipGuard;        
    u8 equipHitBonus;     
    u8 equipSpeed;        
    u8 field4F;           
    u8 speedBonus[4];     
    u8 equipFrameFactor;  
    u8 pad55;
    u8 equipAttackScale;  
    u8 chargeRate;        
    u8 pad58[0x5C - 0x58];
    u8 fileVariant;       
    u8 spriteVariants[3]; 
    u32 hp;               
    u32 maxHp;            
    u16 field68;
    u16 field6A;
    u16 field6C;
    u16 field6E;          
    u16 bodyDefense;      
    u16 armor;            
    u8 field74;
    u8 field75;
    u8 pad76[0x7C - 0x76];
    u16 status7C;         
    u16 field7E;          
    u16 status80;
    u16 status82;         
    xm_StatusPair status84;  
    u8 resistances[16];   
    u8 speed;             
    u8 defense;           
    u8 pad9A[0x9C - 0x9A];
    u8 guard;             
    u8 field9D;
    u8 frameFactor;       
    s8 hitBonus;          

    u8 padA0[0xA4 - 0xA0];
} xm_GearRecord;
#endif
#ifndef XM_T_xm_CharacterSkills
#define XM_T_xm_CharacterSkills
typedef struct {
    u16 counterSkills; 
    u16 levelSkills;   
    u16 unlocksA;      
    u16 unlocksB;      
    u8 pad8[0x17 - 0x8];
    u8 tier;           
    u8 pad18[0x1A - 0x18];
    u16 flags1A;       


    u8 pad1C[0x20 - 0x1C];
} xm_CharacterSkills;
#endif
#ifndef XM_T_xm_WorldmapReturn
#define XM_T_xm_WorldmapReturn
typedef struct {
    u16 x;
    u16 z;
    u16 heading;
    u16 unk5A;
    u16 unk5C;
    u16 unk5E;
    u16 unk60;           
    u16 unk62;
    u16 unk64;
    u16 vehicle_heading; 
    u16 flags;           
    s16 unk6A;
    u16 unk6C;
    u16 unk6E;
    u16 unk70;
    u16 unk72;
    u16 unk74;
    u16 unk76;           
} xm_WorldmapReturn;
#endif
#ifndef XM_T_xm_FlightSave
#define XM_T_xm_FlightSave
typedef struct {
    u16 x_frac;
    s16 x;
    u16 z_frac;
    s16 z;
    u16 count; 
} xm_FlightSave;
#endif
#ifndef XM_T_xm_SystemOptions
#define XM_T_xm_SystemOptions
typedef struct {
    u32 version : 4;   
    u32 option4 : 1;
    u32 option5 : 1;
    u32 option6 : 7;
    u32 option13 : 3;  
    u32 complete : 1;  
    u32 unused : 15;
} xm_SystemOptions;
#endif
#ifndef XM_T_xm_GameData
#define XM_T_xm_GameData
typedef struct xm_GameData {
    u8 names[31][0x14];           

    xm_CharacterRecord characters[11]; 
    xm_GearRecord gears[20];           
    u8 unk1648[0x16C0 - 0x1648];
    xm_CharacterSkills skills[11];     
    xm_WorldmapReturn worldmap;        
    u16 unk1844[3];                 
    u16 unk184A;                    
    xm_FlightSave flight;              
    u16 unk1856;                    
    u8 unk1858[0x1924 - 0x1858];
    u32 gold;                       
    u8 unk1928[0x1930 - 0x1928];
    u16 vars[0x200];              

    u16 joined;                   
    u16 available;                
    u8 party[3];                  
    u8 unk1D37;
    u8 weaponCounts[100];         
    u8 weaponIds[100];            
    u8 accessoryCounts[200];      
    u8 accessoryIds[200];         
    u8 itemCounts[150];           
    u8 itemIds[150];              
    u8 gearPartCounts[100];       
    u8 gearPartIds[100];          
    u8 gearAccessoryCounts[150];  
    u8 gearAccessoryIds[150];     
    u8 unk22B0;
    u8 inGear[3];                 
    u8 unk22B4[2];
    u16 flags;                    

    













    u8 ammo[48];                  
    u8 gearAmmo[48];              
    u16 locked;                   
    u16 map;                      
    u16 entry[3];                 

    u16 flagWords[17];            
    u8 progress[8];               
    xm_SystemOptions options;        
    u8 unk2350[0x2355 - 0x2350];
    u8 flags2355;                 
    u8 unk2356[0x2358 - 0x2356];
} xm_GameData;
#endif
#ifndef XM_S_field_event_become_party_slot
#define XM_S_field_event_become_party_slot
void xm_field_event_become_party_slot(void) __asm__("func_800A0228");
#endif
#ifndef XM_S_field_event_read_imm_or_var
#define XM_S_field_event_read_imm_or_var
s32 xm_field_event_read_imm_or_var(s32 offset) __asm__("FieldScriptVMGetArgument");
#endif
#ifndef XM_S_mode_party_stand_in_actors
#define XM_S_mode_party_stand_in_actors
extern s32 xm_mode_party_stand_in_actors[3] __asm__("D_8006F990");
#endif
#ifndef XM_S_mode_party_members
#define XM_S_mode_party_members
extern s32 xm_mode_party_members[3] __asm__("g_GamePartyMembers");
#endif
#ifndef XM_S_mode_get_character_gear_id
#define XM_S_mode_get_character_gear_id
s32 xm_mode_get_character_gear_id(s32 index) __asm__("GameCharacterGetGearID");
#endif
#ifndef XM_S_field_party_read_slot_position
#define XM_S_field_party_read_slot_position
void xm_field_party_read_slot_position(s32 slot, s32 *a, s32 *b, s32 *c) __asm__("func_800A0158");
#endif
#ifndef XM_S_mode_field_map_id
#define XM_S_mode_field_map_id
extern s32 xm_mode_field_map_id __asm__("g_GameSceneMapNum");
#endif
#ifndef XM_S_game_current_data
#define XM_S_game_current_data
extern xm_GameData *xm_game_current_data __asm__("g_pGameState");
#endif
#ifndef XM_S_field_actor_create_sprite
#define XM_S_field_actor_create_sprite
void xm_field_actor_create_sprite(s32 index, s32 slot, void *data, s32 kind, s32 bank, s32 unk, s32 flag) __asm__("func_80076AC0");
#endif
#ifndef XM_S_mode_party_sprite_blocks
#define XM_S_mode_party_sprite_blocks
extern void *xm_mode_party_sprite_blocks[3] __asm__("g_PartyDataBuffers");
#endif
#ifndef XM_S_field_event_sync_actor_position
#define XM_S_field_event_sync_actor_position
void xm_field_event_sync_actor_position(void) __asm__("func_800A0C94");
#endif
#ifndef XM_S_field_event_show_first_sprite
#define XM_S_field_event_show_first_sprite
void xm_field_event_show_first_sprite(void) __asm__("func_800A0D3C");
#endif
void func_800A0228(void) {
    xm_FieldDescriptor *descriptor;
    s32 slot;
    s32 shown;
    s32 map;
    s32 x;
    s32 z;

    descriptor = &xm_field_view.components.descriptors[xm_field_current_event_actor_index];
    slot = xm_field_event_read_imm_or_var(1);
    if (slot >= 3) {
        slot = 2;
    }
    shown = 1;
    xm_mode_party_stand_in_actors[slot] = xm_field_current_event_actor_index;
    if (xm_mode_party_members[slot] != 0xFF && xm_mode_get_character_gear_id(xm_mode_party_members[slot]) != 0xFF) {
        xm_field_party_read_slot_position(slot, &map, &x, &z);
        xm_field_current_event_actor->layer = (map >> 14) & 3;
        if ((xm_mode_field_map_id & 0xFFF) != (map & 0x3FFF)) {
            shown = 0;
            x = 0;
            z = 0;
            xm_field_current_event_actor->layer = 0;
        }
        if (xm_game_current_data->inGear[slot] != 0) {
            shown = 0;
        } else if (xm_mode_party_members[slot] == 7) {
            shown = 0;
        }
        descriptor->flags = (descriptor->flags & 0xF07F) | 0x200;
        xm_field_actor_create_sprite(xm_field_current_event_actor_index, slot, xm_mode_party_sprite_blocks[slot], 1, 0, slot, 1);
        xm_field_view.components.descriptors[xm_field_current_event_actor_index].flags &= 0xFFDF;
        if ((xm_mode_field_map_id & 0xFFF) != (map & 0x3FFF)) {
            xm_field_current_event_actor->layer = 0;
        }
        xm_field_event_place_actor_on_floor(x, z);
        xm_field_event_sync_actor_position();
        xm_field_current_event_actor->flags = (xm_field_current_event_actor->flags | 0x400) & ~0x300;
        if (shown == 0) {
            xm_field_view.components.descriptors[xm_field_current_event_actor_index].flags |= 0x20;
        }
    } else {
        xm_field_event_show_first_sprite();
        xm_field_current_event_actor->pc += 2;
        xm_field_current_event_actor->layer_flags |= 0x800;
        return;
    }
    if (xm_field_view.components.layer_count - 1 < xm_field_current_event_actor->layer) {
        xm_field_current_event_actor->layer = 0;
    }
    xm_field_current_event_actor->flags |= 0x20000;
    xm_field_current_event_actor->layer_flags |= 0xC00;
    xm_field_current_event_actor->pc += 3;
}
#endif
#else
void func_800A0228(void) {
    /* Re-translated instruction-faithfully from retail 800A0228-800A051C.
     * The FieldActor cached at entry (s4) is used only for the 0xF07F|0x200
     * status store; the later status updates re-derive FA[D_800AFD1C] and
     * every ActorData access goes through g_FieldScriptVMCurActor, as retail. */
    FieldActor* fieldActor = &g_FieldActors[D_800AFD1C];
    s32 partySlot;
    s32 useGear;
    s32 x;
    s32 y;
    s32 z;

    partySlot = FieldScriptVMGetArgument(1);
    if (partySlot >= 3) {
        partySlot = 2;
    }
    D_8006F990[partySlot] = D_800AFD1C;
    useGear = 1;

    if (g_GamePartyMembers[partySlot] == CHARACTER_ID_NONE ||
        GameCharacterGetGearID(g_GamePartyMembers[partySlot]) == CHARACTER_ID_NONE) {
        /* .L800A0474 */
        func_800A0D3C();
        g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
        g_FieldScriptVMCurActor->flags |= 0x800;
        return;
    }

    func_800A0158(partySlot, &x, &y, &z);
    g_FieldScriptVMCurActor->walkmeshId = (s16)((x >> 14) & 3);
    if ((g_GameSceneMapNum & 0xFFF) != (x & 0x3FFF)) {
        useGear = 0;
        y = 0;
        z = 0;
        g_FieldScriptVMCurActor->walkmeshId = 0;
    }
    if (((u8*)g_pGameState)[0x22B1 + partySlot] != 0 ||
        g_GamePartyMembers[partySlot] == 7) {
        useGear = 0;
    }

    fieldActor->status = (fieldActor->status & 0xF07F) | 0x200;
    func_80076AC0(D_800AFD1C, partySlot, g_PartyDataBuffers[partySlot],
                  1, 0, partySlot, 1);
    g_FieldActors[D_800AFD1C].status &= 0xFFDF;
    if ((g_GameSceneMapNum & 0xFFF) != (x & 0x3FFF)) {
        g_FieldScriptVMCurActor->walkmeshId = 0;
    }

    /* 800A03F8: full argument words, as retail (lw a0/a1 from the frame). */
    func_8009E574(y, z);
    func_800A0C94();
    /* 800A0420: addiu a0,zero,-0x301 -> mask 0xFFFFFCFF (~0x300). */
    g_FieldScriptVMCurActor->scriptFlags.flags =
        (g_FieldScriptVMCurActor->scriptFlags.flags | 0x400) & ~0x300;
    if (useGear == 0) {
        g_FieldActors[D_800AFD1C].status |= 0x20;
    }

    /* .L800A04A4 */
    if ((D_800AFB54 - 1) < g_FieldScriptVMCurActor->walkmeshId) {
        g_FieldScriptVMCurActor->walkmeshId = 0;
    }
    /* 800A04DC: lui v1,0x2 -> 0x20000.  Retail stores no D_800B00C0 here. */
    g_FieldScriptVMCurActor->scriptFlags.flags |= 0x20000;
    g_FieldScriptVMCurActor->flags |= 0xC00;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}
#endif /* XENO_PC_PORT */

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): field_actor_copy_position_state (src/decomp/src/field/field_event.c:9970). */
#ifndef XM_S_field_actor_copy_position_state
#define XM_S_field_actor_copy_position_state
void xm_field_actor_copy_position_state(s32 to, s32 from) __asm__("func_800A0524");
#endif
#ifndef XM_S_field_matrix_copy_rotation
#define XM_S_field_matrix_copy_rotation
void xm_field_matrix_copy_rotation(MATRIX *to, MATRIX *from) __asm__("FieldMatrixCopyTransform");
#endif
#ifndef XM_S_field_matrix_copy_translation
#define XM_S_field_matrix_copy_translation
void xm_field_matrix_copy_translation(MATRIX *to, MATRIX *from) __asm__("FieldMatrixCopyTranslation");
#endif
void func_800A0524(s32 to, s32 from) {
    xm_FieldActor *target;
    xm_FieldActor *source;
    s32 i;

    target = xm_field_view.components.descriptors[to].actor;
    source = xm_field_view.components.descriptors[from].actor;
    for (i = 0; i < 4; i++) {
        target->triangle[i] = source->triangle[i];
    }
    target->layer = source->layer;
    target->unkEC = source->unkEC;
    target->unk72 = source->unk72;
    target->unk50[0] = source->unk50[0];
    target->unk50[1] = source->unk50[1];
    target->unk50[2] = source->unk50[2];
    target->position[0] = source->position[0];
    target->position[1] = source->position[1];
    target->position[2] = source->position[2];
    xm_field_matrix_copy_rotation(&xm_field_view.components.descriptors[to].matrix, &xm_field_view.components.descriptors[from].matrix);
    xm_field_matrix_copy_translation(&xm_field_view.components.descriptors[to].matrix, &xm_field_view.components.descriptors[from].matrix);
    xm_field_view.components.descriptors[to].model->x = xm_field_view.components.descriptors[from].actor->position[0];
    xm_field_view.components.descriptors[to].model->y = xm_field_view.components.descriptors[from].actor->position[1];
    xm_field_view.components.descriptors[to].model->z = xm_field_view.components.descriptors[from].actor->position[2];
}
#endif
#else
void func_800A0524(s32 srcIdx, s32 dstIdx) {
    u8* pSrcField = (u8*)g_FieldActors + srcIdx * 92;
    u8* pDstField = (u8*)g_FieldActors + dstIdx * 92;
    /* FieldActor pointer members are PSX u32 slots.  Reconstruct host
     * pointers only after the four-byte guest load, preserving +0x50. */
    u8* pSrcData = (u8*)(uintptr_t)*(u32*)(pSrcField + 0x4C);
    u8* pDstData = (u8*)(uintptr_t)*(u32*)(pDstField + 0x4C);
    s32 i;
    u16* ps = (u16*)(pSrcData + 0x08);
    u16* pd = (u16*)(pDstData + 0x08);
    for (i = 0; i < 4; i++) {
        *pd++ = *ps++;
    }
    *(u16*)(pDstData + 0x10) = *(u16*)(pSrcData + 0x10);
    *(u16*)(pDstData + 0xEC) = *(u16*)(pSrcData + 0xEC);
    *(u16*)(pDstData + 0x72) = *(u16*)(pSrcData + 0x72);
    *(u32*)(pDstData + 0x50) = *(u32*)(pSrcData + 0x50);
    *(u32*)(pDstData + 0x54) = *(u32*)(pSrcData + 0x54);
    *(u32*)(pDstData + 0x58) = *(u32*)(pSrcData + 0x58);
    *(u32*)(pDstData + 0x20) = *(u32*)(pSrcData + 0x20);
    *(u32*)(pDstData + 0x24) = *(u32*)(pSrcData + 0x24);
    *(u32*)(pDstData + 0x28) = *(u32*)(pSrcData + 0x28);
    FieldMatrixCopyTransform((u8*)g_FieldActors + srcIdx * 92 + 0xC,
                              (u8*)g_FieldActors + dstIdx * 92 + 0xC);
    FieldMatrixCopyTranslation((u8*)g_FieldActors + srcIdx * 92 + 0xC,
                                (u8*)g_FieldActors + dstIdx * 92 + 0xC);
    {
        u8* pSrcSub = (u8*)(uintptr_t)*(u32*)(pSrcField + 0x4C);
        u8* pDstSprite = (u8*)(uintptr_t)*(u32*)(pDstField + 0x04);
        *(u32*)(pDstSprite + 0x00) = *(u32*)(pSrcSub + 0x20);
        *(u32*)(pDstSprite + 0x04) = *(u32*)(pSrcSub + 0x24);
        *(u32*)(pDstSprite + 0x08) = *(u32*)(pSrcSub + 0x28);
    }
}
#endif /* XENO_PC_PORT */

extern void func_800A0C94(void);

// Sprite-load script opcode (0x121): bind the current actor's sprite from its
// character id -> party id, then set up status/flags. Sibling of
// func_800A08B8; the non-party path uses sprite mode 1 and the party path
// mode 2 (with the extra func_800A0C94 setup + status 0x20 clears). Stubbed
// before this -> the actor's pSpriteData stayed NULL and its later
// boot-script sprite writes crashed the host (MAP2 actor 3).
/* Transcribed from asm/field/nonmatchings/main/misc6/func_800A06E8.s
 * (0x800A06E8-0x800A08B4). Opcode 0x121: bind current actor sprite from
 * character id -> party id. Status (actor+0x58) = (status & 0xF07F) | 0x200
 * before the partyId==-1 split. Party path: func_80076AC0 mode 2, D_800AFD20
 * = -0xC0, status &= 0xFFDF, func_800A0C94, scriptFlags ( | 0x100) & ~0x80,
 * status &= 0xFFDF again. Non-party: mode 1, scriptFlags |= 1, flags |=
 * 0x100000, D_800AFFEC = D_800B00C0 = 1. Both paths IP += 3. Frame 0x28. */
#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): field_event_set_party_sprite (src/decomp/src/field/field_event.c:9998). */
#ifndef XM_S_field_event_set_party_sprite
#define XM_S_field_event_set_party_sprite
void xm_field_event_set_party_sprite(void) __asm__("func_800A06E8");
#endif
#ifndef XM_S_field_party_find_character_slot
#define XM_S_field_party_find_character_slot
s32 xm_field_party_find_character_slot(s32 id) __asm__("FieldCharacterIdToPartyId");
#endif
#ifndef XM_S_field_event_resolve_character
#define XM_S_field_event_resolve_character
s32 xm_field_event_resolve_character(s32 id) __asm__("func_8008CF3C");
#endif
#ifndef XM_S_field_unread_party_sprite_take_mark
#define XM_S_field_unread_party_sprite_take_mark
extern s16 xm_field_unread_party_sprite_take_mark __asm__("D_800AFD20");
#endif
#ifndef XM_S_field_event_yield_ends_run
#define XM_S_field_event_yield_ends_run
extern s32 xm_field_event_yield_ends_run __asm__("D_800AFFEC");
#endif
#ifndef XM_S_field_event_yield_requested
#define XM_S_field_event_yield_requested
extern s32 xm_field_event_yield_requested __asm__("D_800B00C0");
#endif
void func_800A06E8(void) {
    xm_FieldDescriptor *descriptor;
    s32 slot;

    descriptor = &xm_field_view.components.descriptors[xm_field_current_event_actor_index];
    slot = xm_field_party_find_character_slot(xm_field_event_resolve_character(xm_field_event_read_imm_or_var(1)));
    descriptor->flags = (descriptor->flags & 0xF07F) | 0x200;
    if (slot != -1) {
        xm_field_actor_create_sprite(xm_field_current_event_actor_index, slot, xm_mode_party_sprite_blocks[slot], 2, 0, slot, 1);
        xm_field_unread_party_sprite_take_mark = -0xC0;
        xm_field_view.components.descriptors[xm_field_current_event_actor_index].flags &= 0xFFDF;
        xm_field_event_sync_actor_position();
        xm_field_current_event_actor->flags = (xm_field_current_event_actor->flags | 0x100) & ~0x80;
        xm_field_view.components.descriptors[xm_field_current_event_actor_index].flags &= 0xFFDF;
    } else {
        xm_field_actor_create_sprite(xm_field_current_event_actor_index, 0, xm_mode_party_sprite_blocks[0], 1, 0, 0, 1);
        xm_field_current_event_actor->flags |= 1;
        xm_field_event_yield_ends_run = 1;
        xm_field_event_yield_requested = 1;
        xm_field_current_event_actor->layer_flags |= 0x100000;
    }
    xm_field_current_event_actor->pc += 3;
}
#endif
#else
void func_800A06E8(void) {
    FieldActor* fieldActor = &g_FieldActors[D_800AFD1C];
    s32 characterId = func_8008CF3C(FieldScriptVMGetArgument(1));
    s32 partyId = FieldCharacterIdToPartyId(characterId);

    fieldActor->status = (fieldActor->status & 0xF07F) | 0x200;

    if (partyId == -1) {
        func_80076AC0(D_800AFD1C, 0, g_PartyDataBuffers[0], 1, 0, 0, 1);
        g_FieldScriptVMCurActor->scriptFlags.flags |= 0x1;
        g_FieldScriptVMCurActor->flags |= 0x100000;
        D_800AFFEC = 1;
        D_800B00C0 = 1;
    } else {
        func_80076AC0(D_800AFD1C, partyId, g_PartyDataBuffers[partyId], 2, 0,
                      partyId, 1);
        fieldActor = &g_FieldActors[D_800AFD1C];
#ifdef FIELD_A06E8_MUTANT_SKIP_AFD20
        /* skip D_800AFD20 latch */
#else
        D_800AFD20 = -0xC0;
#endif
        fieldActor->status &= 0xFFDF;
        func_800A0C94();
        g_FieldScriptVMCurActor->scriptFlags.flags =
            (g_FieldScriptVMCurActor->scriptFlags.flags | 0x100) & ~0x80;
        fieldActor = &g_FieldActors[D_800AFD1C];
        fieldActor->status &= 0xFFDF;
    }

    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}
#endif /* XENO_PC_PORT */

extern u8 D_800AE294[];

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): field_event_become_party_character (src/decomp/src/field/field_event.c:10027). */
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
#ifndef XM_S_field_event_become_party_character
#define XM_S_field_event_become_party_character
void xm_field_event_become_party_character(void) __asm__("func_800A08B8");
#endif
#ifndef XM_S_mode_party_actors
#define XM_S_mode_party_actors
extern s32 xm_mode_party_actors[3] __asm__("D_8005A444");
#endif
#ifndef XM_S_field_character_sprite_ids
#define XM_S_field_character_sprite_ids
extern u8 xm_field_character_sprite_ids[11] __asm__("D_800AE294");
#endif
void func_800A08B8(void) {
    xm_FieldDescriptor *descriptor;
    s32 character;
    s32 slot;
    xm_Sprite *model;
    s32 *sprites;
    s32 *entry;

    descriptor = &xm_field_view.components.descriptors[xm_field_current_event_actor_index];
    character = xm_field_event_resolve_character(xm_field_event_read_imm_or_var(1));
    slot = xm_field_party_find_character_slot(character);
    xm_field_current_event_actor->unkE4 = character;
    descriptor->flags = (descriptor->flags & 0xF07F) | 0x200;
    if (slot != -1) {
        if (slot == 0) {
            xm_field_work.controlled = xm_field_current_event_actor_index;
            xm_field_work.unk233E = xm_field_current_event_actor_index;
            xm_field_current_event_actor->flags = (xm_field_current_event_actor->flags | 0x4400) & ~0x80;
        }
        xm_mode_party_actors[slot] = xm_field_current_event_actor_index;
        if (xm_field_work.unk2268 != 0) {
            sprites = xm_field_view.components.sprites;
            entry = &sprites[xm_field_character_sprite_ids[character] + 1] + xm_field_work.unk2268;
            xm_field_actor_create_sprite(xm_field_current_event_actor_index, xm_field_character_sprite_ids[character] + xm_field_work.unk2268, (u8 *)(*entry + (s32)sprites),
                          0, 0, (xm_field_character_sprite_ids[character] + xm_field_work.unk2268) | 0x80, 1);
            xm_field_current_event_actor->flags = (xm_field_current_event_actor->flags | 0x400) & ~0x300;
            if (xm_game_current_data->inGear[slot] != 0) {
                model = xm_field_view.components.descriptors[xm_field_current_event_actor_index].model;
                xm_field_view.components.descriptors[xm_field_current_event_actor_index].model = xm_field_view.components.descriptors[xm_mode_party_stand_in_actors[slot]].model;
                xm_field_view.components.descriptors[xm_mode_party_stand_in_actors[slot]].model = model;
                xm_field_current_event_actor->flags = (xm_field_current_event_actor->flags | 0x200) & ~0x500;
            }
        } else {
            xm_field_actor_create_sprite(xm_field_current_event_actor_index, slot, xm_mode_party_sprite_blocks[slot], 1, 0, slot, 1);
            xm_field_current_event_actor->flags = (xm_field_current_event_actor->flags | 0x400) & ~0x300;
        }
        xm_field_unread_party_sprite_take_mark = -0xC0;
        xm_field_view.components.descriptors[xm_field_current_event_actor_index].flags &= 0xFFDF;
        xm_field_event_place_at_map_entry(xm_field_event_read_variable(2));
        xm_field_event_sync_actor_position();
        xm_field_current_event_actor->layer_flags &= ~0x800;
    } else {
        xm_field_actor_create_sprite(xm_field_current_event_actor_index, 0, xm_mode_party_sprite_blocks[0], 1, 0, 0, 1);
        xm_field_current_event_actor->flags |= 1;
        xm_field_event_yield_ends_run = 1;
        xm_field_event_yield_requested = 1;
        xm_field_current_event_actor->layer_flags |= 0x100000;
    }
    xm_field_current_event_actor->flags |= 0x20000;
    xm_field_current_event_actor->layer_flags |= 0x400;
    xm_field_current_event_actor->pc += 3;
}
#endif
#else
void func_800A08B8(void) {
    /* Re-translated from retail 800A08B8-800A0C48.  Retail has no invariant
     * on D_800B2268 (800A099C branches on it), so there is no assert here. */
    FieldActor* fieldActor = &g_FieldActors[D_800AFD1C];
    s32 characterId = func_8008CF3C(FieldScriptVMGetArgument(1));
    s32 partyId = FieldCharacterIdToPartyId(characterId);
    ActorData* actor = g_FieldScriptVMCurActor;
    u32 flagWord;

    actor->characterId = characterId;
    fieldActor->status = (fieldActor->status & 0xF07F) | 0x200;

    if (partyId == -1) {
        goto bindWithoutParty;
    }

    /* 800A0944: partyId 0 latches the player-actor index and 0x4400/~0x80. */
    if (partyId == 0) {
        g_PlayerActorIndex = D_800AFD1C;
        D_800B233E = D_800AFD1C;
        actor->scriptFlags.flags = (actor->scriptFlags.flags | 0x4400) & ~0x80;
    }
    D_8005A444[partyId] = D_800AFD1C;

    if (D_800B2268 != 0) {
        /* 800A09A4-800A0A00: skin package bind. */
        s32 skinBase = D_800B2268;
        u8* pBase = (u8*)g_FieldSpriteData;
        u8* pPackage = pBase + *(u32*)(pBase + D_800AE294[characterId] * 4 + 4 + skinBase * 4);

        func_80076AC0(D_800AFD1C, D_800AE294[characterId] + skinBase, pPackage, 0, 0,
                      (D_800AE294[characterId] + D_800B2268) | 0x80, 1);
        /* 800A0A04-800A0A28: stored unconditionally, before 0x22B1 is read. */
        actor = g_FieldScriptVMCurActor;
        actor->scriptFlags.flags = (actor->scriptFlags.flags | 0x400) & ~0x300;
        if (((u8*)g_pGameState)[0x22B1 + partyId] != 0) {
            /* 800A0A3C-800A0AD8: trade sprite pointers with the paired actor. */
            u32 savedSprite = g_FieldActors[D_800AFD1C].pSpriteData;

            g_FieldActors[D_800AFD1C].pSpriteData =
                g_FieldActors[D_8006F990[partyId]].pSpriteData;
            g_FieldActors[D_8006F990[partyId]].pSpriteData = savedSprite;
            actor->scriptFlags.flags = (actor->scriptFlags.flags | 0x200) & ~0x500;
        }
    } else {
        /* .L800A0ADC */
        func_80076AC0(D_800AFD1C, partyId, g_PartyDataBuffers[partyId],
                      1, 0, partyId, 1);
        g_FieldScriptVMCurActor->scriptFlags.flags =
            (g_FieldScriptVMCurActor->scriptFlags.flags | 0x400) & ~0x300;
    }

    /* .L800A0B24 */
    D_800AFD20 = -0xC0;
    g_FieldActors[D_800AFD1C].status &= 0xFFDF;
    func_8009FA54(FieldScriptVMGetVariableValue(2));
    func_800A0C94();
    actor = g_FieldScriptVMCurActor;
    flagWord = actor->flags & ~0x800;
    goto finalizeFlags;

bindWithoutParty:
    /* .L800A0B98: no party slot (character not in the party). */
    func_80076AC0(D_800AFD1C, 0, g_PartyDataBuffers[0], 1, 0, 0, 1);
    actor = g_FieldScriptVMCurActor;
    actor->scriptFlags.flags |= 0x1;
    flagWord = actor->flags | 0x100000;
    D_800AFFEC = 1;
    D_800B00C0 = 1;

finalizeFlags:
    /* .L800A0BF8 */
    actor->flags = flagWord;
    actor = g_FieldScriptVMCurActor;
    actor->scriptFlags.flags |= 0x20000;
    actor->flags |= 0x400;
    actor->scriptInstructionPointer += 3;
}
#endif /* XENO_PC_PORT */

void func_800A0C4C(void) {
    ((ActorData*)(uintptr_t)g_FieldActors[g_PlayerActorIndex].pActorData)->scriptFlags.flags |= 0x80;
}

// FieldResetActorPosition, set translation and sprite position of actor based on current actor data position
#ifndef XENO_PC_PORT
/* func_800A0C94: retail re-reads g_FieldActors once per x/y/z group (three loads)
 * while keeping the actor pointer and D_800AFD1C*0x5C in registers: that is a
 * volatile g_FieldActors (as the PC-port header already declares it) read into a
 * fresh per-group pointer; the address is int+int (off first) and y is read once
 * for the two final halfword stores. */
extern FieldActor* volatile g_FieldActors__v asm("g_FieldActors");

#endif
void func_800A0C94(void) {
#ifdef XENO_PC_PORT
    u8* actorData = (u8*)g_FieldScriptVMCurActor;
    u8* fieldActor = (u8*)g_FieldActors + D_800AFD1C * 0x5C;
    u8* spriteData = (u8*)(uintptr_t)*(u32*)(fieldActor + 0x04);
    s16 x = *(s16*)(actorData + 0x22);
    s16 y = *(s16*)(actorData + 0x26);
    s16 z = *(s16*)(actorData + 0x2A);

    *(s32*)(fieldActor + 0x20) = x;
    *(s32*)(fieldActor + 0x40) = x;
    *(s32*)(fieldActor + 0x24) = y;
    *(s32*)(fieldActor + 0x44) = y;
    *(s32*)(fieldActor + 0x28) = z;
    *(s32*)(fieldActor + 0x48) = z;

    *(s32*)(spriteData + 0x00) = *(s32*)(actorData + 0x20);
    *(s32*)(spriteData + 0x04) = *(s32*)(actorData + 0x24);
    *(s32*)(spriteData + 0x08) = *(s32*)(actorData + 0x28);
    *(s32*)(spriteData + 0x10) = 0;
    *(s16*)(spriteData + 0x84) = y;
    *(s16*)(actorData + 0x72) = y;
#else
    u8* actorData = (u8*)g_FieldScriptVMCurActor;
    s32 off = D_800AFD1C * 0x5C;
    u8* fa1;
    u8* fa2;
    u8* fa3;
    u8* spriteData;
    s16 v;
    u16 y;

    fa1 = (u8*)(off + (s32)g_FieldActors__v);
    v = *(s16*)(actorData + 0x22);
    spriteData = (u8*)(uintptr_t)*(u32*)(fa1 + 0x04);
    *(s32*)(fa1 + 0x20) = v;
    *(s32*)(fa1 + 0x40) = v;
    fa2 = (u8*)(off + (s32)g_FieldActors__v);
    v = *(s16*)(actorData + 0x26);
    *(s32*)(fa2 + 0x24) = v;
    *(s32*)(fa2 + 0x44) = v;
    fa3 = (u8*)(off + (s32)g_FieldActors__v);
    v = *(s16*)(actorData + 0x2A);
    *(s32*)(fa3 + 0x28) = v;
    *(s32*)(fa3 + 0x48) = v;
    *(s32*)(spriteData + 0x00) = *(s32*)(actorData + 0x20);
    *(s32*)(spriteData + 0x04) = *(s32*)(actorData + 0x24);
    *(s32*)(spriteData + 0x08) = *(s32*)(actorData + 0x28);
    *(s32*)(spriteData + 0x10) = 0;
    y = *(u16*)(actorData + 0x26);
    *(u16*)(spriteData + 0x84) = y;
    *(u16*)(actorData + 0x72) = y;
#endif
}

void func_800A0D3C(void) {
    func_80076AC0(D_800AFD1C, 0, (void*)((*(s32*)(g_FieldSpriteData + 4)) + (s32)g_FieldSpriteData), 0, 0, 0x80, 1);
    func_800A0C94();
    g_FieldScriptVMCurActor->scriptFlags.fields.scriptFlags_0x8 = 0x1;
    g_FieldScriptVMCurActor->flags |= 0x800;
    g_FieldScriptVMCurActor->scriptInstructionPointer++;
}

extern s16 D_800B234A[];

void func_800A0DC0(void) {
    D_800B234A[0] = FieldScriptVMGetArgument(1);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void func_800A0DFC(void) {
    FieldScriptMemoryWriteU16(SCRIPT_IMM_ARG(1), ArchiveGetDiscNumber());
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

extern s32 D_800ADB74;

void func_800A0E54(void) {
    if (D_800ADB74 == 0) {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 1;
    } else {
        g_FieldScriptVMCurActor->scriptInstructionPointer -= 1;
    }
    D_800B00C0 = 1;
}

extern s32 D_800ADB84;

void func_800A0EB0(void) {
    D_800B00C0 = 1;
    D_800ADB84++;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 1;
}

extern s32 D_800B2264;
extern s32 D_801E8670[];
extern void func_801E8030(s32 idx);

#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc6", func_800A0EE8);
#else
void func_800A0EE8(void) {
    ActorData* pActor = g_FieldScriptVMCurActor;
    u32 flags12C = *(u32*)((u8*)pActor + 0x12C);
    u32 motionIdx = (flags12C >> 13) & 7;
    u8 subOp = SCRIPT_READ_U8_REL(1);

    *(u32*)((u8*)pActor + 0x04) &= 0xFFFFDFFF;

    switch (subOp) {
        case 0: {
            u32* pTable = (u32*)(uintptr_t)D_801E8670;
            u32 pEntry = pTable[motionIdx];
            *(u8*)(pEntry + 0x34) = 0;
            g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
            break;
        }
        case 1: {
            func_801E8030((*(u32*)((u8*)pActor + 0x12C) >> 13) & 7);
            D_800B2264--;
            g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
            break;
        }
    }
    D_800B00C0 = 1;
}
#endif /* XENO_PC_PORT */

/* Object animation archive/state opcode (retail 800A0FD8-800A1360).
 * Sub-op 0 stops the current object animation, sub-op 1 queues the two
 * animation archives, and sub-op 2 consumes that queue and installs the
 * decoded object model.  The queue is deliberately written in its retail
 * eight-byte layout; StreamDataQueueEntry is not a safe substitute on the
 * 64-bit host. */
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc6", func_800A0FD8);
#else
/* D_800B2394 is the 0x18-byte archive request block (u16 idx, u32 buf, u16
 * idx, u32 buf, u16 0, u32 0); an array view of the same symbol keeps the
 * multi-field stores within the object (UBSan object-size). */
extern u8 D_800B2394_buf[] __asm__("D_800B2394");
void func_800A0FD8(void) {
    ActorData* actor = g_FieldScriptVMCurActor;
    s32 slot;
    u8 subOp;

    if (D_800ADB2C != 0 || func_8008A558() != 0) {
        actor->scriptInstructionPointer--;
        D_800B00C0 = 1;
        return;
    }

    slot = (s32)((*(u32*)((u8*)actor + 0x12C) >> 13) & 7);
    actor->flags &= ~0x2000;
    ArchiveSetIndex(4, 0);
    subOp = SCRIPT_READ_U8_REL(1);

    if (subOp == 0) {
        /* D_801E8670 is populated by the field object overlay.  A null entry
         * is possible while that overlay is still unavailable in the PC
         * port; retain the proven IP/VM effects without turning that known
         * boundary into a host crash. */
        if (D_801E8670[slot] != 0) {
            *(u8*)((u8*)(uintptr_t)(u32)D_801E8670[slot] + 0x34) = 0;
        }
        actor->scriptInstructionPointer += 2;
        D_800B00C0 = 1;
        return;
    }

    if (subOp == 1) {
        func_801E8030(slot);
        /* 800A1114: sll v0,v0,1 (u32 shift: no host UB on negative args). */
        D_800B21DC[slot] = (u16)((u32)FieldScriptVMGetArgument(5) << 1);

        *(u16*)(void*)D_800B2394_buf =
            (u16)D_800B21DC[slot] + 0x6BA;
        /* 800A112C/800A1170: lh -- the archive index is the SIGN-extended
         * halfword + 0x6BA/0x6BB, as retail. */
        *(u32*)(D_800B2394_buf + 4) =
            (u32)(uintptr_t)HeapAlloc(
                (u32)ArchiveDecodeAlignedSize((u32)((s16)D_800B21DC[slot] + 0x6BA)),
                0);
        *(u16*)(D_800B2394_buf + 8) =
            (u16)D_800B21DC[slot] + 0x6BB;
        *(u32*)(D_800B2394_buf + 12) =
            (u32)(uintptr_t)HeapAlloc(
                (u32)ArchiveDecodeAlignedSize((u32)((s16)D_800B21DC[slot] + 0x6BB)),
                1);
        *(u16*)(D_800B2394_buf + 16) = 0;
        *(u32*)(D_800B2394_buf + 20) = 0;
        D_8005A420[slot] = (void*)(uintptr_t)
            *(u32*)(D_800B2394_buf + 4);
        D_8005A450[slot] = (void*)(uintptr_t)
            *(u32*)(D_800B2394_buf + 12);
        func_80029AFC((void*)D_800B2394_buf, 0, 0);
        actor->scriptInstructionPointer += 2;
        D_800B00C0 = 1;
        return;
    }

    if (subOp == 2) {
        if (ArchiveCdDataSync(1) != 0) {
            actor->scriptInstructionPointer--;
            D_800B00C0 = 1;
            return;
        }

        /* Retail consumes argument 2 here as part of the setup sequence;
         * its value is intentionally unused by the following call. */
        (void)FieldScriptVMGetArgument(2);

        {
            s32 effect = D_800B225F[slot];
            s16* vec = (s16*)((u8*)D_800B21EC + slot * 8);
            s32 x = (s16)(0x240 - ((slot + effect) << 6));
            s32 w = (s16)(slot + 0xFC);
            s32* object;

            func_801E742C(slot, 0, D_8005A420[slot], D_8005A450[slot],
                          x, 0x100, 0, w, vec);
            /* u32 slot: (uintptr_t) of the s32 would sign-extend guest 0x80xxxxxx. */
            object = (s32*)(uintptr_t)(u32)D_801E8670[slot];
            if (object != NULL) {
                D_800B220C[slot] = *(s16*)((u8*)object + 0x1C);
            }
            HeapFree(D_8005A450[slot]);
            actor->scriptInstructionPointer += 4;
            actor->flags |= 0x2000;

            if (object != NULL) {
                s16 scale = *(s16*)((u8*)actor + 0xF4);
                *(s16*)((u8*)object + 0x1C) = (scale * 5) >> 6;
                *(s16*)((u8*)object + 0x60) = *(s16*)((u8*)actor + 0x26);
                *(s32*)((u8*)(uintptr_t)(u32)object[1] + 0x5C) =
                    *(s16*)((u8*)actor + 0x22);
                *(s32*)((u8*)(uintptr_t)(u32)object[1] + 0x64) =
                    *(s16*)((u8*)actor + 0x2A);
            }
        }
    } else {
        D_800B00C0 = 1;
        return;
    }

    D_800B00C0 = 1;
}
#endif /* XENO_PC_PORT */

extern s32 D_800B2264;
extern u16 D_800B21DC[];
extern u8 D_800B225F[];

// Object-sprite load opcode: bind the current actor's sprite from the map's
// sprite package (g_FieldSpriteData + offset[1], the same package slot the
// sibling func_800A0D3C binds), reset its position, then register the
// argument (<<1) into the D_800B2264-indexed object-slot pair
// (D_800B21DC[]/D_800B225F[]) and stamp (slot&7)<<13 into the actor's +0x12C
// word.  Stubbed before this -> the actor's pSpriteData stayed NULL and its
// later sprite writes crashed the host (MAP3 boot SEGV at func_8009EB78).
/* Byte-exact against retail 31874-319FC (99 instructions, 0x30 frame) when
 * compiled through the GCC 2.7.2 field preset; compiled by both the matching
 * build and the port (the former XENO_FIELD_OBJECT_OVERLAY staging is gone).
 * Source-shape facts that are load-bearing for parity, none of which change
 * behaviour: (1) the two object-slot stores go through pointer arithmetic on
 * the array symbols rather than D_800B21DC[i] / D_800B225F[i]; an indexed
 * array store does not make GCC 2.7.2 discard its cached copies of the
 * scalar globals, whereas retail re-reads both D_800B2264 and
 * g_FieldScriptVMCurActor after each store.  (2) The slot stamp reads
 * D_800B2264 through a volatile lvalue so that the read and the following
 * increment are two loads, as retail has them, even though the +0x12C store
 * in between cannot alias the counter.  (3) The IP/flag writes go through
 * the global pointer and the actor mask through g_FieldActors[D_800AFD1C]
 * directly; using a cached local for either reorders retail's loads.
 * (4) pad[2] is never referenced; retail's frame is 8 bytes larger than the
 * call area plus ra/s0 need, which only an unused non-register local
 * reproduces.  Evidence and the differential test:
 * docs/evidence/blackmoon-object-overlay-verification-20260906/README.md,
 * pc_port/tests/run_field_object_register_retail_test.sh.
 * Registering an object arms the pipeline: D_800B2264 != 0 ->
 * func_80077884/func_80077AB4 stream the per-slot model archives and
 * instantiate them through the archive-0x6B9 owners in
 * pc_port/src/field_object_overlay.c, and func_800821F4's object-animation
 * branch in misc8.c. */
void func_800A1364(void) {
    u8* actorData;
    u8* fieldActor;
    s32 spriteId;
    s32 pad[2];

    fieldActor = (u8*)g_FieldActors + D_800AFD1C * 0x5C;
    *(u16*)(fieldActor + 0x58) = (*(u16*)(fieldActor + 0x58) & 0xF07F) | 0x200;

    spriteId = FieldScriptVMGetArgument(1);
    func_80076AC0(D_800AFD1C, 0,
                  (void*)((*(s32*)(g_FieldSpriteData + 4)) + (s32)g_FieldSpriteData),
                  0, 0, 0x80, 1);
    func_800A0C94();

    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
    g_FieldScriptVMCurActor->scriptFlags.flags |= 0x100;
    g_FieldActors[D_800AFD1C].status &= 0xFFDF;
    actorData = (u8*)g_FieldScriptVMCurActor;
    *(u32*)(actorData + 0x4) = (*(u32*)(actorData + 0x4) | 0x2000) & ~0x800;

    *(u16*)((u8*)D_800B21DC + D_800B2264 * 2) = (u32)spriteId << 1;
    *((u8*)D_800B225F + D_800B2264) = 0;
    actorData = (u8*)g_FieldScriptVMCurActor;
    *(u32*)(actorData + 0x12C) =
        (*(u32*)(actorData + 0x12C) & 0xFFFF1FFF) | ((*(volatile s32*)&D_800B2264 & 7) << 13);
    D_800B2264++;
}

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): field_event_set_sprite_parameter (src/decomp/src/field/field_event.c:10272). */
#ifndef XM_S_field_event_set_sprite_parameter
#define XM_S_field_event_set_sprite_parameter
void xm_field_event_set_sprite_parameter(void) __asm__("func_800A14F0");
#endif
void func_800A14F0(void) {
    xm_FieldDescriptor *descriptor;
    xm_FieldActor *actor;
    s32 sprite;
    s32 *sprites;
    u8 *data;

    descriptor = &xm_field_view.components.descriptors[xm_field_current_event_actor_index];
    descriptor->flags = (descriptor->flags & 0xF07F) | 0x200;
    sprite = xm_field_event_read_imm_or_var(1);
    sprites = xm_field_view.components.sprites;
    data = (u8 *)(sprites[sprite + 1] + (s32)sprites);
    xm_field_actor_create_sprite(xm_field_current_event_actor_index, sprite, data, 0, xm_field_event_read_imm_or_var(3), sprite | 0x80, 1);
    xm_field_event_sync_actor_position();
    actor = xm_field_current_event_actor;
    actor->pc += 5;
    actor->flags = (actor->flags | 0x100) & ~0x80;
    actor->layer_flags &= ~0x800;
    xm_field_view.components.descriptors[xm_field_current_event_actor_index].flags &= 0xFFDF;
}
#endif
#else
void func_800A14F0(void) {
    u8* actorData;
    u8* fieldActor;
    u8* spritePackage;
    s32 skinIndex;
    s32 arg3;

    fieldActor = (u8*)g_FieldActors + D_800AFD1C * 0x5C;
    *(u16*)(fieldActor + 0x58) = (*(u16*)(fieldActor + 0x58) & 0xF07F) | 0x200;

    skinIndex = FieldScriptVMGetArgument(1);
    spritePackage = (u8*)g_FieldSpriteData +
                    *(u32*)((u8*)g_FieldSpriteData + (skinIndex * 4) + 4);
    arg3 = FieldScriptVMGetArgument(3);
    func_80076AC0(D_800AFD1C, skinIndex, spritePackage, 0, arg3, skinIndex | 0x80, 1);
    func_800A0C94();

    actorData = (u8*)g_FieldScriptVMCurActor;
    *(u16*)(actorData + 0xCC) += 5;
    *(u32*)(actorData + 0x0) = (*(u32*)(actorData + 0x0) | 0x100) & ~0x80;
    *(u32*)(actorData + 0x4) &= ~0x800;

    fieldActor = (u8*)g_FieldActors + D_800AFD1C * 0x5C;
    *(u16*)(fieldActor + 0x58) &= ~0x20;
}
#endif /* XENO_PC_PORT */

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): field_event_set_sprite (src/decomp/src/field/field_event.c:10298). */
#ifndef XM_S_field_event_set_sprite
#define XM_S_field_event_set_sprite
void xm_field_event_set_sprite(void) __asm__("func_800A1624");
#endif
void func_800A1624(void) {
    xm_FieldDescriptor *descriptor;
    xm_FieldActor *actor;
    s32 sprite;
    s32 *sprites;

    descriptor = &xm_field_view.components.descriptors[xm_field_current_event_actor_index];
    descriptor->flags = (descriptor->flags & 0xF07F) | 0x200;
    sprite = xm_field_event_read_imm_or_var(1);
    sprites = xm_field_view.components.sprites;
    xm_field_actor_create_sprite(xm_field_current_event_actor_index, sprite, (u8 *)(sprites[sprite + 1] + (s32)sprites), 0, 0, sprite | 0x80, 0);
    xm_field_event_sync_actor_position();
    actor = xm_field_current_event_actor;
    actor->pc += 3;
    actor->flags = (actor->flags | 0x100) & ~0x80;
    actor->layer_flags &= ~0x800;
    xm_field_view.components.descriptors[xm_field_current_event_actor_index].flags &= 0xFFDF;
}
#endif
#else
void func_800A1624(void) {
    u8* actorData;
    u8* fieldActor;
    u8* spritePackage;
    s32 skinIndex;

    fieldActor = (u8*)g_FieldActors + D_800AFD1C * 0x5C;
    *(u16*)(fieldActor + 0x58) = (*(u16*)(fieldActor + 0x58) & 0xF07F) | 0x200;

    skinIndex = FieldScriptVMGetArgument(1);
    spritePackage = (u8*)g_FieldSpriteData +
                    *(u32*)((u8*)g_FieldSpriteData + (skinIndex * 4) + 4);
    func_80076AC0(D_800AFD1C, skinIndex, spritePackage, 0, 0, skinIndex | 0x80, 0);
    func_800A0C94();

    actorData = (u8*)g_FieldScriptVMCurActor;
    *(u16*)(actorData + 0xCC) += 3;
    *(u32*)(actorData + 0x0) = (*(u32*)(actorData + 0x0) | 0x100) & ~0x80;
    *(u32*)(actorData + 0x4) &= ~0x800;

    fieldActor = (u8*)g_FieldActors + D_800AFD1C * 0x5C;
    *(u16*)(fieldActor + 0x58) &= ~0x20;
}
#endif /* XENO_PC_PORT */
