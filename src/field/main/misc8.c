#include "common.h"
#include "field/actor.h"
#include "field/main.h"
#include "field/text_box.h"
#include "field/effects.h"
#include "field/script_vm.h"
#include "system/memory.h"
#include "system/archive.h"
#ifdef XENO_PC_PORT
#include <stdio.h>
#include <stdlib.h>
#endif
#include "system/sound.h"
#include "psyq/libetc.h"
#include "psyq/libcd.h"
#ifdef XENO_PC_PORT
#include <assert.h>
#else
/* <assert.h> is unavailable under the matching build's -nostdinc MIPS
 * preprocessor. The assert(...) calls below mark unimplemented/invariant
 * checks in functions not yet byte-matched, so a no-op assert compiles
 * safely there. */
#define assert(x) ((void)0)
#endif

extern int D_800ADBFC;
extern void func_80281678(void*);
void func_8008083C(int actorIndex) {
    ActorData* pActor;

    if (actorIndex < D_800ADBFC) {
        pActor = (ActorData*)(uintptr_t)g_FieldActors[actorIndex].pActorData;
        if (pActor->flags134 & 0x80) {
            HeapFree((void*)(uintptr_t)pActor->unk110);
        }
        if (pActor->flags12C_0xD) {
            HeapFree((void*)(uintptr_t)pActor->unk114);
        }
        if (g_FieldActors[actorIndex].status & 0x2000) {
            HeapFree((void*)(uintptr_t)pActor->unk118);
        }
        if (pActor->unk124 != -1) {
            HeapFree((void*)(uintptr_t)pActor->unk120);
        }
        HeapFree(pActor);
        HeapFree((void*)(uintptr_t)g_FieldActors[actorIndex].pShadow);
        func_800230A8((void*)(uintptr_t)g_FieldActors[actorIndex].pSpriteData);
    }
}

/* ---- func_80080968: actor state table lookup -------------------------------
 * Called from func_80080A74. Reads ActorData.field_10 as a state index,
 * checks a gate bit in field_04, then looks up a pointer in two-level
 * tables (D_800AFB24 / D_800AFB20) indexed by sub-state values.
 * Returns the looked-up pointer (stored at ActorData offset 0x14). */
extern s32 D_800AFB20;
extern s32 D_800AFB24[];

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): field_actor_get_floor_attribute (src/decomp/src/field/field_motion.c:2238). */
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
#ifndef XM_S_field_actor_get_floor_attribute
#define XM_S_field_actor_get_floor_attribute
u32 xm_field_actor_get_floor_attribute(xm_FieldActor *actor) __asm__("func_80080968");
#endif
#ifndef XM_S_field_view
#define XM_S_field_view
extern xm_FieldView xm_field_view __asm__("g_CameraEye");
#endif
u32 func_80080968(xm_FieldActor *actor) {
    s16 layer = actor->layer;

    if ((actor->layer_flags >> (layer + 3)) & 1) {
        return 0;
    }
    return xm_field_view.components.collision_attributes[xm_field_view.components.collision_triangles[layer][actor->triangle[layer]].attribute].word;
}
#endif
#else
s32 func_80080968(u8* pActorData) {
    s16 stateIdx = *(s16*)(pActorData + 0x10);
    s32 gateWord = *(s32*)(pActorData + 0x04);

    /* Gate check: if bit (stateIdx+3) of field_04 is set, return 0 */
    if ((gateWord >> (stateIdx + 3)) & 1) {
        return 0;
    }

    /* Sub-state lookup: read s16 at pActorData[stateIdx].offset_08
     * (entries are 2 bytes each, starting at pActorData + 0) */
    {
        s16 subIdx = *(s16*)(pActorData + stateIdx * 2 + 0x08);
        s32 val = subIdx * 7;  /* subIdx * 8 - subIdx = subIdx * 7 */

        /* D_800AFB24[stateIdx] points to an array of 14-byte structs.
         * val * 2 indexes into it (14-byte stride). */
        u8* tableRow = (u8*)(uintptr_t)D_800AFB24[stateIdx];
        s32 byteVal;
        u32* finalTable;

        /* Native fail-closed guard. Retail assumes the relocated table owner
         * is valid here; retaining this branch is an audited port divergence. */
        if (tableRow == NULL) return 0;

        byteVal = *(u8*)(tableRow + val * 2 + 0x0C);

        /* Final lookup: D_800AFB20[byteVal] */
        finalTable = (u32*)(uintptr_t)D_800AFB20;
        /* Same native ownership guard for the contiguous field data block. */
        if (finalTable == NULL) return 0;

        return (s32)finalTable[byteVal];
    }
}
#endif /* XENO_PC_PORT */

extern s16 D_800B14AC;

s32 func_800809D0(u8* pData) {
    s32 step = *(s32*)(pData + 0x1C);
    s32 acc = (s32)0xFFEB3000;
    s32 count = 0;
    D_800B14AC = 0;
    while (acc <= 0) {
        count += acc;
        acc += step;
        D_800B14AC += 2;
    }
    return count >> 16;
}

extern s32 D_800ADB58;
extern s32 D_800ADB5C;

s32 func_80080A18(void) {
    ActorData* p = (ActorData*)(uintptr_t)g_FieldActors[D_800ADB58].pActorData;
    s32 i = D_800ADB5C++;
    return ((s32*)(uintptr_t)p->unk118)[i];
}

/* ---- func_80080A74: per-actor second-pass ActorData initialization ----------
 * Called from func_80080F44 for each actor. Initializes ActorData with
 * hardcoded defaults and state-array entries. Calls func_80080968 for
 * additional setup. Conditionally runs the func_8007B1C4 distance-init loop.
 *
 * All offsets are byte offsets into the ActorData allocation (0x138 bytes),
 * verified against MIPS asm. */
extern s16 D_800AFB54;
extern s32 D_800AFB44[];

#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc8", func_80080A74);
#else
void func_80080A74(s32 actorIndex) {
    u8* p = (u8*)(uintptr_t)g_FieldActors[actorIndex].pActorData;
    /* NOTE: deliberately NOT zero-initialised. This was `= { 0 };`, and GCC 2.7.2
     * materialises such a local aggregate initialiser as a 104-byte all-zero
     * constant template in `.rodata` ($LC0 = `.half 0` + `.space 102`, visible in
     * build/src/field/main/misc8.c.s), then copies it onto the stack. Retail's
     * `func_80080A74` (asm/field/matchings/main/misc8/func_80080A74.s) has ZERO
     * `(sh|sw|sb) $zero, N($sp)` stores and no memcpy/memset call - it never zeroes
     * this buffer. That template was the last 0x68 of field `.rodata` surplus
     * (field_RODATA_SIZE 0x360 vs retail 0x2FC), so the initialiser is removed to
     * match retail's behaviour and codegen. */
    s16 stateBuf[0x34];
    s32 i;

    /* ---- early field defaults ---- */
    *(s32*)(p + 0x00) = 0xB0;
    *(s32*)(p + 0x04) = 0x800;
    *(s16*)(p + 0x18) = 0x10;
    *(s16*)(p + 0x1C) = 0x10;
    *(s16*)(p + 0x1A) = 0x60;
    *(u8*)(p + 0x74) = 0xFF;
    *(u8*)(p + 0x75) = 0xFF;
    *(s32*)(p + 0x40) = 0;
    *(s32*)(p + 0x44) = 0;
    *(s32*)(p + 0x48) = 0;
    *(s32*)(p + 0x30) = 0;
    *(s32*)(p + 0x34) = 0;
    *(s32*)(p + 0x38) = 0;
    *(s16*)(p + 0x64) = 0;
    *(s16*)(p + 0x60) = 0;
    *(s16*)(p + 0x62) = 0;
    *(s32*)(p + 0xD0) = 0;
    *(s32*)(p + 0xD4) = 0;
    *(s32*)(p + 0xD8) = 0;
    *(s16*)(p + 0xE6) = 0;
    *(s16*)(p + 0xEA) = 0xFF;
    *(u8*)(p + 0xE2) = 0;
    *(s16*)(p + 0xCC) = 0;
    *(s16*)(p + 0x6E) = 0;

    /* flags at 0x12C / 0x130 / 0x134. Immediates are retail addiu/lui
     * values from asm/field/matchings/main/misc8/func_80080A74.s. */
#ifdef FIELD_ACTOR_INIT_MUTANT_12C_MASK
    *(s32*)(p + 0x12C) &= ~0x30000;
#else
    *(s32*)(p + 0x12C) &= -0x21; /* 0x80080B3C addiu $v1, -0x21 */
#endif
    *(s16*)(p + 0x1E) = *(s16*)(p + 0x18);
    *(s32*)(p + 0x12C) &= -0x4; /* 0x80080BA4 addiu $v0, -0x4 */
    *(s16*)(p + 0x11E) = 0x200;
    *(u8*)(p + 0x101) = 0x80;
    *(u8*)(p + 0x100) = 0x80;
    *(u8*)(p + 0xFF)  = 0x80;
    *(u8*)(p + 0xFE)  = 0x80;
    *(u8*)(p + 0xFD)  = 0x80;
    *(u8*)(p + 0xFC)  = 0x80;
    *(s16*)(p + 0x128) = 0xFFFF;
    *(s32*)(p + 0x12C) &= 0xFFFCFFFF; /* $a2 */
    *(s32*)(p + 0x130) &= 0xF007FFFF; /* $a1 */
    *(s32*)(p + 0x130) &= -0x200; /* 0x80080BD8 addiu $v0, -0x200 */
    *(s32*)(p + 0x130) &= 0xFFF801FF; /* $a3; previously missing */
    *(s32*)(p + 0x12C) &= 0xF003FFFF; /* $t0 */

    /* script slots: 8 entries of 8 bytes each at offset 0x8C */
    for (i = 0; i < 8; i++) {
        u8* e = p + 0x8C + i * 8;
        s32 flags = *(s32*)(e + 4);
        *(s16*)(e + 0) = 0xFFFF;
        *(u8*)(e + 2) = 0;
        *(u8*)(e + 3) = 0xFF;
        flags &= 0xFFFCFFFF;
        flags &= 0xFFBFFFFF;
        flags &= 0xFE7FFFFF;
        flags |= 0x3C0000;
        *(s32*)(e + 4) = flags;
        *(s16*)(e + 4) = 0xFFFF;
    }

    *(s32*)(p + 0x120) = 0;
    *(s16*)(p + 0xE4) = 0xFF;
    *(s16*)(p + 0x76) = 0x100;
    *(s32*)(p + 0x12C) &= -0x1C1; /* 0x80080C58 addiu $v0, -0x1C1 */
    *(u8*)(p + 0x83) = 0;
    *(u8*)(p + 0x82) = 0;
    *(s16*)(p + 0x8A) = 0;
    *(s16*)(p + 0x88) = 0;
    *(s32*)(p + 0x84) = 0;
    *(u8*)(p + 0xCF) = 0;
    *(u8*)(p + 0xCE) = 0;
    *(s16*)(p + 0xE8) = 0;
    *(s16*)(p + 0x10) = 0;
    *(s16*)(p + 0xEC) = 0;
    *(s32*)(p + 0x134) &= -0x81; /* 0x80080C64 addiu $v1, -0x81 */
    *(s32*)(p + 0x12C) &= -0xE01; /* 0x80080C98 addiu $v0, -0xE01 */
    *(s32*)(p + 0x12C) &= -0x1001; /* 0x80080CA0 addiu $v0, -0x1001 */
    *(s32*)(p + 0x134) &= -0x61; /* 0x80080CAC addiu $v0, -0x61 */

    *(s16*)(p + 0x102) = (s16)rand();
    *(s16*)(p + 0xF4) = 0x1000;
    *(s16*)(p + 0xF6) = 0x1000;
    *(s16*)(p + 0xF8) = 0x1000;
    *(u8*)(p + 0x10D) = 0xFF;
    *(u8*)(p + 0x80) = 0xFF;
    *(s16*)(p + 0x106) = -0x8000;
    *(s16*)(p + 0x104) = -0x8000;
    *(s16*)(p + 0x108) = -0x8000;
    *(s16*)(p + 0x124) = -1;
    *(u8*)(p + 0xE3) = 0;
    *(s16*)(p + 0x0E) = 0;
    *(s16*)(p + 0x0C) = 0;
    *(s16*)(p + 0x0A) = 0;
    *(s16*)(p + 0x08) = 0;
    *(s32*)(p + 0x12C) &= -0x1D; /* 0x80080CFC addiu $v0, -0x1D */

    /* ---- D_800AFB54 loop (blez count-1 at 0x80080D28). ----
     * Addressing from func_80080A74.s:
     *   jal pOut is $sp+0x58 + (i<<3) each iter (0x80080D70), not an
     *     induction pointer;
     *   fail zeros are sh 0x40/42/44($s2) with $s2 walking from $sp+0x18
     *     (0x80080DC8);
     *   dest is sh 0x8($s4) with $s4 starting at ActorData (0x80080D80). */
    if (D_800AFB54 - 1 > 0) {
        s16* pState = stateBuf;
        s16* pWalk = stateBuf;
        s16* pDst = (s16*)p;

        for (i = 0; i < D_800AFB54 - 1; i++) {
            s16 r = func_8007B1C4(
                *(s16*)((u8*)g_FieldActors + actorIndex * 0x5C + 0x20),
                *(s16*)((u8*)g_FieldActors + actorIndex * 0x5C + 0x28),
                i, (s16*)((u8*)stateBuf + 0x40 + (i << 3)), pState);
            pDst[4] = r;
            if (r != -1 && (u32)r >= (u32)D_800AFB44[i]) {
                D_800AFB44[i] = 0;
                ((s32*)pState)[0] = 0;
                ((s32*)pState)[1] = 0;
                ((s32*)pState)[2] = 0;
                *(s16*)((u8*)pWalk + 0x40) = 0;
                *(s16*)((u8*)pWalk + 0x42) = 0;
                *(s16*)((u8*)pWalk + 0x44) = 0;
            }
            pWalk += 4;
            pState += 8;
            pDst += 1;
        }
    }

    /* ---- post-loop: always jal func_80080968, always $sp+0x18 ---- */
    *(s32*)(p + 0x14) = func_80080968(p);
    /* Retail reloads lh 0x10($s0) for each of 0x50/54/58 (0x80080E00,
     * 0x80080E0C, 0x80080E2C) — do not CSE into a local. */
    *(s32*)(p + 0x50) =
        *(s32*)((u8*)stateBuf + (*(s16*)(p + 0x10) * 16) + 0);
    *(s32*)(p + 0x54) =
        *(s32*)((u8*)stateBuf + (*(s16*)(p + 0x10) * 16) + 4);
    *(s32*)(p + 0x58) =
        *(s32*)((u8*)stateBuf + (*(s16*)(p + 0x10) * 16) + 8);

    /* status & 0x80 / FieldActor+0x24 copy is AFTER 0x50/54/58
     * (0x80080E6C). Retail keeps actorIndex*0x5C in $a0 and reloads
     * g_FieldActors at each of 0x20/0x24/0x28/0x72 (0x80080E9C,
     * 0x80080EBC, 0x80080EDC, 0x80080EFC); 0x72 is lw 0x24 then sh. */
    if (!(*(u16*)((u8*)g_FieldActors + actorIndex * 0x5C + 0x58) & 0x80)) {
        *(s32*)((u8*)g_FieldActors + actorIndex * 0x5C + 0x24) =
            (s32)*(s16*)((u8*)stateBuf + *(s16*)(p + 0x10) * 8 + 0x42);
    }

    /* Retail 0x80080E9C / EBC / EDC / EFC: lui/lw g_FieldActors per store.
     * Each load clobbers the row in $v0 (lw 0x20($v0) etc.). 0x72 is
     * lw 0x24 then sh (0x80080F0C). */
    *(s32*)(p + 0x20) =
        *(s32*)((u8*)g_FieldActors + actorIndex * 0x5C + 0x20) << 16;
    *(s32*)(p + 0x24) =
        *(s32*)((u8*)g_FieldActors + actorIndex * 0x5C + 0x24) << 16;
    *(s32*)(p + 0x28) =
        *(s32*)((u8*)g_FieldActors + actorIndex * 0x5C + 0x28) << 16;
    *(s16*)(p + 0x72) =
        (s16)*(s32*)((u8*)g_FieldActors + actorIndex * 0x5C + 0x24);
}
#endif /* XENO_PC_PORT */

/* ---- func_80080F44: per-actor data initialization ---------------------------
 * Called from FieldLoad for each actor (0..D_800ADBFC-1).
 * Allocates and zeroes the 0x138-byte ActorData block, sets up animation
 * dispatch if status & 0x2000, allocates a 0x70-byte shadow buffer, and calls
 * func_80080A74 / func_8007AA44 for further init.
 *
 * ASM-verified FieldActor byte offsets (struct uses u32 for PSX pointer fidelity):
 *   0x00 pModelData  0x04 pSpriteData  0x08 pShadow  0x4C pActorData
 *   0x50 rotation.x  0x52 rotation.y   0x54 rotation.z
 *   0x56 flags        0x58 status
 *   sizeof(FieldActor) = 0x5C
 *
 * ActorData byte offsets (allocated 0x138 bytes):
 *   0x110 unk110  0x114 unk114  0x118 pAnimTable  0x120 unk120
 *   0x124 unk124  0x12C flags12C  0x134 flags134
 */
extern s32 D_800B2180;
extern void func_8007AA44(void*);

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): field_actor_create (src/decomp/src/field/field_motion.c:2396). */
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
#ifndef XM_S_field_actor_create
#define XM_S_field_actor_create
void xm_field_actor_create(s32 index) __asm__("func_80080F44");
#endif
#ifndef XM_S_field_event_actor_count
#define XM_S_field_event_actor_count
extern s32 xm_field_event_actor_count __asm__("D_800ADBFC");
#endif
#ifndef XM_S_field_work
#define XM_S_field_work
extern xm_FieldWork xm_field_work __asm__("g_FieldEffects");
#endif
#ifndef XM_S_heap_alloc
#define XM_S_heap_alloc
void *xm_heap_alloc(s32 size, s32 mode) __asm__("HeapAlloc");
#endif
#ifndef XM_S_field_morph_read_list_word
#define XM_S_field_morph_read_list_word
s32 xm_field_morph_read_list_word(xm_MorphChannel *channel) __asm__("func_80080A18");
#endif
#ifndef XM_S_field_actor_reset
#define XM_S_field_actor_reset
void xm_field_actor_reset(s32 index) __asm__("func_80080A74");
#endif
#ifndef XM_S_field_shadow_init_quad
#define XM_S_field_shadow_init_quad
void xm_field_shadow_init_quad(xm_FieldMarker *m) __asm__("func_8007AA44");
#endif
void func_80080F44(s32 index) {
    u8 unused[0x60];
    xm_FieldActor *actor;
    xm_FieldInstance *instance;
    s32 *word;
    s32 words = sizeof(xm_FieldActor) / 4;
    s32 i;

    if (index < xm_field_event_actor_count) {
        xm_field_work.unk2180++;
        xm_field_view.components.descriptors[index].actor = xm_heap_alloc(0x138, 0);
        word = (s32 *)xm_field_view.components.descriptors[index].actor;
        for (i = 0; i < words; i++) {
            *word++ = 0;
        }
        actor = xm_field_view.components.descriptors[index].actor;
        xm_field_view.components.descriptors[index].unk5A = 0;
        if (xm_field_view.components.descriptors[index].flags & 0x2000) {
            instance = xm_field_view.components.descriptors[index].instance;
            actor->list = xm_heap_alloc(0x80, 0);
            if (instance->anims != NULL) {
                for (i = 0; i < instance->anims->count; i++) {
                    instance->anims->channels[i].update = xm_field_morph_read_list_word;
                    actor->list[i] = 0;
                }
            }
        }
        xm_field_actor_reset(index);
        xm_field_view.components.descriptors[index].shadow = xm_heap_alloc(0x70, 0);
        xm_field_shadow_init_quad(xm_field_view.components.descriptors[index].shadow);
    }
}
#endif
#else
void func_80080F44(s32 actorIndex) {
    FieldActor* pActor;
    ActorData* pData;
    s32 i;

    if (actorIndex >= D_800ADBFC) return;

    /* 1. Allocate and zero the ActorData block (sizeof == retail 0x138 now
     * that the 0x110 pointer run is u32 — see actor.h) */
    D_800B2180++;
    pData = (ActorData*)(uintptr_t)HeapAlloc(sizeof(ActorData), 0);
    g_FieldActors[actorIndex].pActorData = (u32)(uintptr_t)pData;

    for (i = 0; i < 0x4E; i++) {
        ((s32*)pData)[i] = 0;
    }

    /* 2. Zero field_5A (halfword at actor offset 0x5A, past status) */
    *(s16*)((u8*)&g_FieldActors[actorIndex] + 0x5A) = 0;

    /* 3. If status has bit 0x2000: allocate animation dispatch table */
    if (g_FieldActors[actorIndex].status & 0x2000) {
        u32* pModel = (u32*)(uintptr_t)g_FieldActors[actorIndex].pModelData;
        void* pAnimTable = HeapAlloc(0x80, 0);
        pData->unk118 = (u32)(uintptr_t)pAnimTable;

        if (pModel != NULL) {
            void* pAnimInfo = (void*)(uintptr_t)pModel[0x14 / 4];
            if (pAnimInfo != NULL) {
                s32 count = ((s32*)pAnimInfo)[0xC / 4];
                if (count > 0) {
                    void* pEntries = (void*)(uintptr_t)((u32*)pAnimInfo)[0x10 / 4];
                    for (i = 0; i < count; i++) {
                        ((u32*)pEntries)[i * 8] = (u32)(uintptr_t)func_80080A18;
                        ((s32*)pAnimTable)[i] = 0;
                    }
                }
            }
        }
    }

    /* 4. Per-actor retail-derived init callback. */
    func_80080A74(actorIndex);

    /* 5. Allocate 0x70-byte shadow buffer */
    g_FieldActors[actorIndex].pShadow = (u32)(uintptr_t)HeapAlloc(0x70, 0);
    func_8007AA44((void*)(uintptr_t)g_FieldActors[actorIndex].pShadow);
}
#endif /* XENO_PC_PORT */

extern s32 D_800AF858;
extern s32 D_800ADC0C;
extern s32 D_800C3910;
extern s32 g_PlayerActorIndex;
extern u8 D_800B21CC;
extern char D_8006FC0C[];
extern char D_8006FC18[];
extern char D_8006FC24[];
extern char D_8006FC30[];
extern char D_8006FC3C[];
extern void func_800A2030(void);
extern void func_80281B00(void* arg0);
extern void func_80082620(s32 actorIndex, void* actor, void* actorData);
extern void func_80082BB8(s32 actorIndex, void* actor, void* actorData);
extern void func_800245D8(void* pSpriteData, s16 animIndex);
extern void func_800821F4(void* pSpriteData, s16 animIndex, void* pFieldActor);
extern void func_80084158(s32 actorIndex, void* actor, void* actorData);
extern void func_8008399C(s32 actorIndex, void* actor, void* actorData);
extern void func_800815F0(void);
extern s32 func_80084A40(s32 actorIndex, s32 y, void* pFieldActor, u8* actorData,
                         s32 targetState);
extern void* HeapAlloc(u_int size, u_int allocMode);

#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc8", func_8008110C);
#else
void func_8008110C(void) {
    s32 i;

    D_800C3910 = -1;
    func_800A2030();

    for (i = 0; i < D_800ADBFC; i++) {
        /* Retail 80081154..800811C0 reloads g_FieldActors and the actor's
         * 0x4C data pointer for each of the three stores. */
        *(s16*)((u8*)(uintptr_t)g_FieldActors[i].pActorData + 0x68) = *(s16*)((u8*)(uintptr_t)g_FieldActors[i].pActorData + 0x22);
        *(s16*)((u8*)(uintptr_t)g_FieldActors[i].pActorData + 0x6A) = *(s16*)((u8*)(uintptr_t)g_FieldActors[i].pActorData + 0x26);
        *(s16*)((u8*)(uintptr_t)g_FieldActors[i].pActorData + 0x6C) = *(s16*)((u8*)(uintptr_t)g_FieldActors[i].pActorData + 0x2A);
    }

    if (g_FieldSystemMode == 0) {
        func_80281B00(D_8006FC0C);
    }

    D_800AF858 = 0;

    for (i = 0; i < D_800ADBFC; i++) {
        u8* actor = (u8*)g_FieldActors + i * 0x5C;
        u8* actorData = (u8*)(uintptr_t)*(u32*)(actor + 0x4C);
        u16 status = *(u16*)(actor + 0x58);

        if ((status & 0x0F80) == 0x0200) {
            u32 flags0 = *(u32*)(actorData + 0x00);

            if ((flags0 & 0x00010001) == 0) {
                if ((*(u32*)(actorData + 0x04) & 0x600) != 0x200) {
                    *(s32*)(actorData + 0x14) = func_80080968(actorData);
                    func_80082620(i, actor, actorData);
                    func_80082BB8(i, actor, actorData);
                }
            } else if ((*(u32*)(actorData + 0x04) & 0x01000000) != 0 &&
                       (flags0 & 0x00010000) == 0) {
                if (*(s16*)(actorData + 0xE8) != *(s16*)(actorData + 0xEA)) {
                    *(s16*)(actorData + 0xEA) = 2;
                    *(s16*)(actorData + 0xE8) = *(u16*)(actorData + 0xEA);
                    func_800245D8((void*)(uintptr_t)*(u32*)(actor + 0x04),
                                  *(s16*)(actorData + 0xEA));
                }
            } else if ((*(u32*)(actorData + 0x04) & 0x00200000) != 0) {
                if (*(s16*)(actorData + 0xE8) != *(s16*)(actorData + 0xEA)) {
                    *(s16*)(actorData + 0xE8) = *(s16*)(actorData + 0xEA);
                    func_800821F4((void*)(uintptr_t)*(u32*)(actor + 0x04),
                                  *(s16*)(actorData + 0xEA), actor);
                }
            }
        }
    }

    if (g_FieldSystemMode == 0) {
        func_80281B00(D_8006FC18);
    }

    {
        u8* player = (u8*)g_FieldActors + g_PlayerActorIndex * 0x5C;
        func_80084158(g_PlayerActorIndex, player, (void*)(uintptr_t)*(u32*)(player + 0x4C));
    }

    if (g_FieldSystemMode == 0) {
        func_80281B00(D_8006FC24);
    }

    for (i = 0; i < D_800ADBFC; i++) {
        u8* actor = (u8*)g_FieldActors + i * 0x5C;
        u16 status = *(u16*)(actor + 0x58);

        if ((status & 0x0F00) != 0) {
            u8* actorData = (u8*)(uintptr_t)*(u32*)(actor + 0x4C);

            if ((*(u32*)(actorData + 0x04) & 0x600) != 0x200 &&
                (status & 0x0F80) == 0x0200 &&
                ((*(u32*)(actorData + 0x00) & 0x00010001) == 0) &&
                i != g_PlayerActorIndex) {
                /* Retail 800814B0 passes targetState 0 in the delay slot. */
                func_80084A40(i, 0x7FFFFFFF, actor, actorData, 0);

                {
                    u8* spriteData = (u8*)(uintptr_t)*(u32*)(actor + 0x04);
                    u8* frameData = (u8*)(uintptr_t)*(u32*)(spriteData + 0x7C);

                    if (*(u16*)(frameData + 0x0C) == 1) {
                        *(u32*)(actorData + 0x00) &= ~0x800u;
                    }
                }
            }
        }
    }

    if (g_FieldSystemMode == 0) {
        func_80281B00(D_8006FC30);
    }

#ifdef XENO_PC_PORT
/* Host route-testing switch (god_mode.c); weak so tests that link this TU
 * without it keep the retail behaviour (encounters enabled). */
extern int PcPort_RandomBattlesEnabled(void) __attribute__((weak));
#define XENO_RANDOM_BATTLES_ON() (!PcPort_RandomBattlesEnabled || PcPort_RandomBattlesEnabled())
#endif
    /* The port can suppress random encounters for route testing; the flag is
     * host-side, so the matching build keeps the retail condition. */
#ifdef XENO_PC_PORT
    if (XENO_RANDOM_BATTLES_ON() &&
        g_FieldControl.isRandomEncountersEnabled == 0 && D_800B21CC == 0) {
#else
    if (g_FieldControl.isRandomEncountersEnabled == 0 && D_800B21CC == 0) {
#endif
        u8* player = (u8*)g_FieldActors + g_PlayerActorIndex * 0x5C;
        func_8008399C(g_PlayerActorIndex, player, (void*)(uintptr_t)*(u32*)(player + 0x4C));
    }

    D_800ADC0C = 1;
    func_800815F0();

    if (g_FieldSystemMode == 0) {
        func_80281B00(D_8006FC3C);
    }
#ifdef XENO_PC_PORT
    {
        static int s_npcEventDumpEnabled = -1;
        static int s_npcEventDumpFrame;
        static int s_dumpInited;
        static u16 s_lastIp[3];
        static u8 s_lastOp[3];
        static u16 s_lastLock = 0xFFFF;
        static s16 s_lastVis = 0x7FFF;
        static u16 s_lastWaitf = 0xFFFF;
        static const int kDumpActors[3] = {16, 22, 40};
        extern u16 D_800C2694;
        extern s16 D_800B2174;
        s32 i;

        if (s_npcEventDumpEnabled < 0) {
            const char* env = getenv("XENO_NPC_EVENT_DUMP");
            s_npcEventDumpEnabled =
                (env != NULL && env[0] != '\0' && env[0] != '0');
        }
        if (s_npcEventDumpEnabled && s_npcEventDumpFrame < 240) {
            u8* scriptData = (u8*)g_FieldScriptVMCurScriptData;
            u16 lock = (u16)D_800B2174;
            s16 vis0 = g_FieldTextBoxes[0].visibility;
            u16 waitf0 = *(u16*)((u8*)&g_FieldTextBoxes[0] + 0x18 + 0x10);
            int talkedge = (D_800C2694 & 0x20) != 0;

            if (!s_dumpInited || lock != s_lastLock || vis0 != s_lastVis ||
                talkedge) {
                printf("[npc-event] frame=%d actors=%d lock=0x%x talkedge=%d pad=0x%x\n",
                       s_npcEventDumpFrame, (int)D_800ADBFC, (unsigned)lock,
                       talkedge, (unsigned)D_800C2694);
            }
            for (i = 0; i < 3; i++) {
                int idx = kDumpActors[i];
                u8* fieldActor;
                u32 pActorData;
                ActorData* actor;
                u16 ip;
                u8 op;

                if (idx >= D_800ADBFC) {
                    continue;
                }
                fieldActor = (u8*)g_FieldActors + idx * 0x5C;
                pActorData = *(u32*)(fieldActor + 0x4C);
                if (pActorData == 0) {
                    continue;
                }
                actor = (ActorData*)(uintptr_t)pActorData;
                ip = actor->scriptInstructionPointer;
                op = (scriptData != NULL) ? scriptData[ip] : 0;
                if (!s_dumpInited || ip != s_lastIp[i] || op != s_lastOp[i]) {
                    printf("[npc-event] f=%d actor=%d ip=%u op=0x%02x wait=%d "
                           "flags0=%u mode=%u pos=%d,%d anim=%d\n",
                           s_npcEventDumpFrame, idx, (unsigned)ip, (unsigned)op,
                           (int)actor->scripts[actor->curScriptIndex].waitTimer,
                           (unsigned)actor->scripts[actor->curScriptIndex].flags_0,
                           (unsigned)actor->scripts[actor->curScriptIndex].flags_0x17,
                           (int)*(s16*)((u8*)actor + 0x22),
                           (int)*(s16*)((u8*)actor + 0x2A),
                           (int)actor->curAnimationId);
                    s_lastIp[i] = ip;
                    s_lastOp[i] = op;
                }
            }
            if (!s_dumpInited || vis0 != s_lastVis || waitf0 != s_lastWaitf ||
                lock != s_lastLock) {
                printf("[npc-event] f=%d box=0 vis=%d status=%d order=%u "
                       "owner=%d waitf=0x%x\n",
                       s_npcEventDumpFrame, (int)vis0,
                       (int)g_FieldTextBoxes[0].status,
                       (unsigned)g_FieldTextBoxes[0].order,
                       (int)g_FieldTextBoxes[0].ownerActorID,
                       (unsigned)waitf0);
            }
            s_lastLock = lock;
            s_lastVis = vis0;
            s_lastWaitf = waitf0;
            s_dumpInited = 1;
        }
        if (s_npcEventDumpEnabled) {
            s_npcEventDumpFrame++;
        }
    }
#endif
}
#endif /* XENO_PC_PORT */

extern s16 D_800B234E;
extern u8 D_800B21CF;
extern s32 FieldCharacterIdToPartyId(s32 characterId);
void func_80081F80(void* pSpriteData, s16 angle, void* pFieldActor);
/* Movement-history ring labels (0x48-stride entries; the writer
 * func_80081C54 below uses the same anchors). */
extern s32 D_800B2360;
extern s32 D_800C3910;
extern u8 D_800B14F0;
extern u8 D_800B14F8;
extern u8 D_800B14FA;
extern u8 D_800B14FC;
extern u8 D_800B1500;
extern u8 D_800B1502;
extern u8 D_800B1504;
extern u8 D_800B1510;
extern u8 D_800B1514;
extern u8 D_800B1518;
extern u8 D_800B1534;

#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc8", func_800815F0);
#else
void func_800815F0(void) {
    s32 i;

    if (D_800B234E != 0) {
        /* Held-mode: followers only track the scripted anim (asm
         * 8008164C-800816F0). */
        for (i = 0; i < D_800ADBFC; i++) {
            u8* actor = (u8*)g_FieldActors + i * 0x5C;
            u8* actorData = (u8*)(uintptr_t)*(u32*)(actor + 0x4C);
            u8* spriteData;
            s16 anim;

            if ((*(u32*)(actorData + 0x00) & 0x01000000) == 0) {
                continue;
            }
            if (i == g_PlayerActorIndex ||
                (*(u16*)(actor + 0x58) & 0x20) != 0) {
                continue;
            }

            spriteData = (u8*)(uintptr_t)*(u32*)(actor + 0x04);
            anim = *(s16*)(actorData + 0xE6);
            if (*(s16*)(actorData + 0xE8) != anim) {
                *(s16*)(actorData + 0xE8) = anim;
                if (anim < 0) {
                    *(s16*)(actorData + 0xE8) = 0;
                }
                func_800821F4(spriteData, *(s16*)(actorData + 0xE8), actor);
            }
        }
        return;
    }

    /* Party-history sync (asm .L800816F8-80081C04): each follower consumes
     * the player's movement-history ring that func_80081C54 records (0x48-
     * stride entries anchored at the D_800B14F0.. labels; the ring head
     * D_800B2360[0] DECREMENTS, so followers trail the head).  Per-party-
     * slot cursors live at (&D_800B2360)[partyId]. */
    for (i = 0; i < D_800ADBFC; i++) {
        u8* actor = (u8*)g_FieldActors + i * 0x5C;
        u8* actorData = (u8*)(uintptr_t)*(u32*)(actor + 0x4C);
        u8* spriteData;
        s32 partyId;
        s32* pCursor;
        s32 k;
        u32 histFlags;
        s32 j;

        if ((*(u32*)(actorData + 0x00) & 0x01000000) == 0) {
            continue;
        }
        if (i == g_PlayerActorIndex ||
            (*(u16*)(actor + 0x58) & 0x20) != 0) {
            continue;
        }

        spriteData = (u8*)(uintptr_t)*(u32*)(actor + 0x04);
        partyId = FieldCharacterIdToPartyId(*(s16*)(actorData + 0xE4));
        if (partyId == -1) {
            continue;
        }

        pCursor = &D_800B2360 + partyId;
        k = *pCursor;

        /* Turn toward the history entry's facing. */
        func_80081F80(spriteData, *(s16*)(&D_800B1504 + ((*pCursor) * 9 * 8)), actor);

        histFlags = *(u32*)(&D_800B14F0 + ((*pCursor) * 9 * 8));

        if (D_800B21CF == 1) {
            /* Snap the follower cursor to just behind the head and consume
             * immediately (asm jumps to .L8008191C, SKIPPING the caught-up
             * check; the 0x800 flag test keeps the OLD entry's flags while
             * anim/apply reload the snapped cursor). */
            *pCursor = (D_800B2360 + 1) & 0x1F;
            k = *pCursor;
            goto consume;
        } else if (!(histFlags & 0x800)) {
            /* The history frame is an idle frame. */
            u32 flags4 = *(u32*)(actorData + 0x04) & ~0x1000u;

            *(u32*)(actorData + 0x04) = flags4;
            if ((*(u32*)(actorData + 0x14) & 0x420000) == 0) {
                if (D_800C3910 == -1) {
                    /* Grounded idle: settle back to the scripted anim. */
                    if (*(s16*)(spriteData + 0x84) !=
                        *(s16*)(actorData + 0x26)) {
                        goto apply_gate;
                    }
                    if (*(s16*)(actorData + 0xE8) == 6) {
                        *(u32*)(actorData + 0x04) = flags4 | 0x1000;
                        continue;
                    }
                    if (*(s16*)(actorData + 0xE8) ==
                        *(s16*)(actorData + 0xE6)) {
                        continue;
                    }
                    *(s16*)(actorData + 0xE8) = *(s16*)(actorData + 0xE6);
                    if (*(s16*)(actorData + 0xE8) < 0) {
                        *(s16*)(actorData + 0xE8) = 0;
                    }
                    func_800821F4(spriteData, *(s16*)(actorData + 0xE8),
                                  actor);
                    continue;
                }
                /* Airborne/track mode: hold until this follower's cursor
                 * sits exactly `step` entries behind the head (asm
                 * .L80081898; step 0xA for party slot 1, 0x14 otherwise). */
                {
                    s32 step = (partyId != 1) ? 0x14 : 0xA;

                    if (((D_800B2360 + step) & 0x1F) != k) {
                        continue;
                    }
                }
            }
            goto apply_gate;
        }

    apply_gate:
        /* asm .L800818C8: caught up to the head -> hold the scripted anim
         * and consume nothing. */
        if (k == D_800B2360) {
            *(u32*)(actorData + 0x00) &= ~0x800u;
            *(s16*)(actorData + 0xE8) = *(s16*)(actorData + 0xE6);
            if (*(s16*)(actorData + 0xE8) < 0) {
                *(s16*)(actorData + 0xE8) = 0;
            }
            func_800821F4(spriteData, *(s16*)(actorData + 0xE8), actor);
            continue;
        }

    consume:
        /* Consume one history entry (asm .L8008191C-80081C04). */
        if (histFlags & 0x800) {
            *(u32*)(actorData + 0x00) |= 0x800;
        } else {
            *(u32*)(actorData + 0x00) &= ~0x800u;
        }

        if (*(s16*)(actorData + 0xE8) != *(s16*)(&D_800B1502 + ((*pCursor) * 9 * 8))) {
            *(s16*)(actorData + 0xE8) = *(s16*)(&D_800B1502 + ((*pCursor) * 9 * 8));
            if (*(s16*)(actorData + 0xE8) < 0) {
                *(s16*)(actorData + 0xE8) = 0;
            }
            func_800821F4(spriteData, *(s16*)(actorData + 0xE8), actor);
        }

        /* The apply block: the exact inverse of func_80081C54's record. */
        for (j = 0; j < 4; j++) {
            *(u16*)(actorData + 0x08 + j * 2) =
                *(u16*)(&D_800B1510 + ((*pCursor) * 9 * 8) - 0x0A + j * 2);
        }
        *(s16*)(actorData + 0x10) = *(&D_800B1534 + ((*pCursor) * 9 * 8));
        *(u32*)(spriteData + 0x0C) = *(u32*)(&D_800B1510 + ((*pCursor) * 9 * 8));
        *(u32*)(spriteData + 0x10) = *(u32*)(&D_800B1514 + ((*pCursor) * 9 * 8));
        *(u32*)(spriteData + 0x14) = *(u32*)(&D_800B1518 + ((*pCursor) * 9 * 8));
        *(u32*)(actorData + 0x50) = *(u32*)(&D_800B1510 + ((*pCursor) * 9 * 8) + 0x10);
        *(u32*)(actorData + 0x54) = *(u32*)(&D_800B1510 + ((*pCursor) * 9 * 8) + 0x14);
        *(u32*)(actorData + 0x58) = *(u32*)(&D_800B1510 + ((*pCursor) * 9 * 8) + 0x18);
        *(u32*)(actor + 0x20) = *(s16*)(&D_800B14F8 + ((*pCursor) * 9 * 8));
        *(u32*)(actor + 0x24) = *(s16*)(&D_800B14FA + ((*pCursor) * 9 * 8));
        *(u32*)(actor + 0x28) = *(s16*)(&D_800B14FC + ((*pCursor) * 9 * 8));
        *(u32*)(spriteData + 0x00) = *(s32*)(actor + 0x20) << 16;
        *(u32*)(actorData + 0x20) = *(s32*)(actor + 0x20) << 16;
        *(u32*)(spriteData + 0x04) = *(s32*)(actor + 0x24) << 16;
        *(u32*)(actorData + 0x24) = *(s32*)(actor + 0x24) << 16;
        *(u32*)(spriteData + 0x08) = *(s32*)(actor + 0x28) << 16;
        *(u32*)(actorData + 0x28) = *(s32*)(actor + 0x28) << 16;
        *(u16*)(spriteData + 0x84) = *(u16*)(&D_800B1500 + ((*pCursor) * 9 * 8));
        *(u16*)(actorData + 0x106) = *(u16*)(&D_800B1504 + ((*pCursor) * 9 * 8));
        *(u16*)(actorData + 0x104) = *(u16*)(&D_800B1504 + ((*pCursor) * 9 * 8));
        *pCursor = (k - 1) & 0x1F;
    }
}
#endif /* XENO_PC_PORT */

extern s32 g_PlayerActorIndex;
extern u8 D_800B21CC;
extern s32 D_800B2360;
extern s32 D_800C3910;
extern u8 D_800B14F0;
extern u8 D_800B14F4;
extern u8 D_800B14F8;
extern u8 D_800B14FA;
extern u8 D_800B14FC;
extern u8 D_800B1500;
extern u8 D_800B1502;
extern u8 D_800B1504;
extern u8 D_800B1510;
extern u8 D_800B1514;
extern u8 D_800B1518;
extern u8 D_800B1530;
extern u8 D_800B1534;

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): field_record_movement_history (src/decomp/src/field/field_motion.c:2640). */
#ifndef XM_T_xm_FieldHistory
#define XM_T_xm_FieldHistory
typedef struct xm_FieldHistory {
    u32 flags;          
    u32 layer_flags;    
    s16 position[3];    
    s16 unk0E;
    u16 model84;        
    s16 unk12;          
    s16 heading;        
    s16 triangle[4];    
    u8 unk1E[2];
    s32 model_velocity[3]; 
    u8 unk2C[4];
    s32 unk30[3];       
    u8 unk3C[4];
    u32 unk40;          
    u8 layer;           
    u8 unk45[3];
} xm_FieldHistory;
#endif
#ifndef XM_S_field_record_movement_history
#define XM_S_field_record_movement_history
void xm_field_record_movement_history(s32 index) __asm__("func_80081C54");
#endif
#ifndef XM_S_field_movement_history_records
#define XM_S_field_movement_history_records
extern xm_FieldHistory xm_field_movement_history_records[32] __asm__("D_800B14F0");
#endif
#ifndef XM_S_field_movement_history_indices
#define XM_S_field_movement_history_indices
extern s32 xm_field_movement_history_indices[3] __asm__("D_800B2360");
#endif
#ifndef XM_S_field_movement_history_not_recorded
#define XM_S_field_movement_history_not_recorded
extern s32 xm_field_movement_history_not_recorded __asm__("D_800C3910");
#endif
void func_80081C54(s32 index) {
    xm_Sprite *model;
    xm_FieldActor *actor;
    s32 i;
    xm_FieldDescriptor *descriptor;

    descriptor = &xm_field_view.components.descriptors[index];
    actor = descriptor->actor;
    model = descriptor->model;
    if (index == xm_field_work.controlled && xm_field_work.party_processing_mode == 0) {
        copyVector((VECTOR *)xm_field_movement_history_records[xm_field_movement_history_indices[0]].model_velocity, (VECTOR *)&model->speed_x);
        copyVector((VECTOR *)xm_field_movement_history_records[xm_field_movement_history_indices[0]].unk30, (VECTOR *)actor->unk50);
        xm_field_movement_history_records[xm_field_movement_history_indices[0]].heading = actor->heading_goal & 0xFFF;
        xm_field_movement_history_records[xm_field_movement_history_indices[0]].model84 = model->ground;
        xm_field_movement_history_records[xm_field_movement_history_indices[0]].position[0] = actor->position[0] >> 16;
        xm_field_movement_history_records[xm_field_movement_history_indices[0]].position[1] = actor->position[1] >> 16;
        xm_field_movement_history_records[xm_field_movement_history_indices[0]].position[2] = actor->position[2] >> 16;
        xm_field_movement_history_records[xm_field_movement_history_indices[0]].unk12 = actor->unkE8;
        xm_field_movement_history_records[xm_field_movement_history_indices[0]].unk40 = actor->floor_attribute;
        xm_field_movement_history_records[xm_field_movement_history_indices[0]].flags = actor->flags;
        xm_field_movement_history_records[xm_field_movement_history_indices[0]].layer_flags = actor->layer_flags;
        for (i = 0; i < 4; i++) {
            xm_field_movement_history_records[xm_field_movement_history_indices[0]].triangle[i] = actor->triangle[i];
        }
        xm_field_movement_history_records[xm_field_movement_history_indices[0]].layer = actor->layer;
        xm_field_movement_history_not_recorded = 0;
        xm_field_movement_history_indices[0] = (xm_field_movement_history_indices[0] - 1) & 0x1F;
    }
}
#endif
#else
void func_80081C54(s32 actorIndex) {
    u8* actor = (u8*)g_FieldActors + actorIndex * 0x5C;
    u8* actorData = (u8*)(uintptr_t)*(u32*)(actor + 0x4C);
    u8* spriteData = (u8*)(uintptr_t)*(u32*)(actor + 0x04);
    s32 i;

    if (actorIndex != g_PlayerActorIndex) {
        return;
    }

    if (D_800B21CC != 0) {
        return;
    }

    /* Retail 80081C78..80081F34 reloads D_800B2360 and recomputes slot*9*8
     * (= 72) for EVERY store; the aliasing store through the ring base defeats
     * CSE, which is what gives this function its 194 instructions. Index the
     * ring by the live slot at each site instead of caching the offset. */
    *(u32*)(&D_800B1510 + D_800B2360 * 72) = *(u32*)(spriteData + 0x0C);
    *(u32*)(&D_800B1514 + D_800B2360 * 72) = *(u32*)(spriteData + 0x10);
    *(u32*)(&D_800B1518 + D_800B2360 * 72) = *(u32*)(spriteData + 0x14);

    *(u32*)(&D_800B1510 + D_800B2360 * 72 + 0x10) = *(u32*)(actorData + 0x50);
    *(u32*)(&D_800B1510 + D_800B2360 * 72 + 0x14) = *(u32*)(actorData + 0x54);
    *(u32*)(&D_800B1510 + D_800B2360 * 72 + 0x18) = *(u32*)(actorData + 0x58);

    *(u16*)(&D_800B1504 + D_800B2360 * 72) = *(u16*)(actorData + 0x106) & 0x0FFF;
    *(u16*)(&D_800B1500 + D_800B2360 * 72) = *(u16*)(spriteData + 0x84);
    *(u16*)(&D_800B14F8 + D_800B2360 * 72) = *(u16*)(actorData + 0x22);
    *(u16*)(&D_800B14FA + D_800B2360 * 72) = *(u16*)(actorData + 0x26);
    *(u16*)(&D_800B14FC + D_800B2360 * 72) = *(u16*)(actorData + 0x2A);
    *(u16*)(&D_800B1502 + D_800B2360 * 72) = *(u16*)(actorData + 0xE8);
    *(u32*)(&D_800B1530 + D_800B2360 * 72) = *(u32*)(actorData + 0x14);
    *(u32*)(&D_800B14F0 + D_800B2360 * 72) = *(u32*)(actorData + 0x00);
    *(u32*)(&D_800B14F4 + D_800B2360 * 72) = *(u32*)(actorData + 0x04);

    for (i = 0; i < 4; i++) {
        *(u16*)(&D_800B1510 - 0x0A + D_800B2360 * 72 + i * 2) = *(u16*)(actorData + 0x08 + i * 2);
    }

    *(&D_800B1534 + D_800B2360 * 72) = *(u16*)(actorData + 0x10);
    D_800C3910 = 0;
    D_800B2360 = (D_800B2360 - 1) & 0x1F;
}
#endif /* XENO_PC_PORT */

s32 func_80081F5C(u32* a0) {
    u32 a = (a0[0] >> 9) & 3;
    u32 b = a0[5] >> 3;
    return -((a & b) != 0);
}

extern int rsin(int);
extern int rcos(int);
extern void func_80021FE0(void* pSpriteData, s16 angle);
extern void* D_801E8670[];

/* func_80081F80: compute the per-frame walk-step vector into the sprite's step
 * fields (spriteData +0x0C / +0x14, plus +0x18 for the 0x80000 case) from the
 * actor's facing angle. asm 80081F80-800821F0. Branches on fieldActor->status
 * bit 0x40 and actorData->flags. Fei (status 0x40 set; flags & 0x2000 and
 * & 0x80000 both clear) takes the func_80021FE0 path, which stamps the angle
 * into the sprite so func_80022974 derives the step from angle + sprite radius.
 * Divisions are guarded against moveSpeed==0 for host safety (PSX tolerates
 * div-by-zero; the guard is a no-op whenever moveSpeed != 0). */
#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): field_actor_set_planar_velocity (src/decomp/src/field/field_motion.c:2681). */
#ifndef XM_T_xm_ModelTable
#define XM_T_xm_ModelTable
typedef struct xm_ModelTable {
    xm_SpriteModel **models;
    u32 count;
} xm_ModelTable;
#endif
#ifndef XM_T_xm_EffectEntry
#define XM_T_xm_EffectEntry
typedef struct {
    u8 used;
    u8 field1;     
    u8 kind;       
    u8 tag;        
    u16 params[6]; 
    u16 time;      
    u16 duration;  
} xm_EffectEntry;
#endif
#ifndef XM_T_xm_ModelPart
#define XM_T_xm_ModelPart
typedef struct xm_ModelPart {
    struct xm_ModelPart *parent; 
    u8 dirty;         
    u8 rotate;        
    u8 yxz;           
    u8 visible;       
    u16 modelId;      
    u16 index;        
    MATRIX transform; 
    MATRIX world;     
    s16 scale[3];     
    u16 field52;      


    SVECTOR rotation;        
    s32 translation[3];      
    void *packets[2];        
    xm_EffectEntry *effects[3]; 
} xm_ModelPart;
#endif
#ifndef XM_T_xm_EntryTable
#define XM_T_xm_EntryTable
typedef struct xm_EntryTable {
    s32 count;
    s32 *entries;
} xm_EntryTable;
#endif
#ifndef XM_T_xm_SoundBank
#define XM_T_xm_SoundBank
typedef struct xm_SoundBank {
    u8 unk0[0x10];
    u16 flags;
    u8 unk12[2];
    u16 id;
    u16 unk16;         
    u16 volumes;       
    u8 unk1A[2];
    struct xm_SoundBank *next;
    u16 effect[1];     
} xm_SoundBank;
#endif
#ifndef XM_T_xm_SoundOwner
#define XM_T_xm_SoundOwner
typedef struct xm_SoundOwner {
    u8 pad0[8];
    xm_SoundBank *bank;        
    void *end;              
} xm_SoundOwner;
#endif
#ifndef XM_T_xm_EffectSprite
#define XM_T_xm_EffectSprite
typedef struct {
    s16 x0, y0, z0, pad06;
    s16 x1, y1, z1;
    s16 projected; 
    s16 x2, y2, z2;
    s16 age;       
    s16 x3, y3, z3;
    s16 lifetime;  
    u16 color[3];  
    s16 fade[3];   
    POLY_FT4 packets[2]; 
} xm_EffectSprite;
#endif
#ifndef XM_T_xm_SpritePool
#define XM_T_xm_SpritePool
typedef struct xm_SpritePool {
    xm_EffectSprite *records;
    s16 count;
    s16 next;
} xm_SpritePool;
#endif
#ifndef XM_T_xm_ColorFade
#define XM_T_xm_ColorFade
typedef struct xm_ColorFade {
    s16 id;           
    u8 solid;         
    u8 semiTrans;     
    xm_SpritePool *pool; 
    xm_EffectSprite *sprite; 
    SVECTOR ends[2];  
    union {
        struct {
            DVECTOR first[8];
            DVECTOR second[8];
        } screen;
        struct {
            VECTOR first[2];
            VECTOR second[2];
        } world;
    } history; 
    s16 time;     
    s16 count;    
    s16 max;      
    s16 duration; 
    s16 color[3]; 
    s16 step[3];  
} xm_ColorFade;
#endif
#ifndef XM_T_xm_SurfaceEntry
#define XM_T_xm_SurfaceEntry
typedef struct {
    s16 h0, h2, h4, h6, h8, hA, hC, hE;
} xm_SurfaceEntry;
#endif
#ifndef XM_T_xm_SurfacePoint
#define XM_T_xm_SurfacePoint
typedef struct {
    s16 length;
    s16 sag;
    s16 pos[3];
    u16 normalCount; 
    s32 normal[3];   
} xm_SurfacePoint;
#endif
#ifndef XM_T_xm_SurfacePoly
#define XM_T_xm_SurfacePoly
typedef struct {
    s16 index[3];
    u8 pad6[2];
    POLY_GT3 prim[2];
} xm_SurfacePoly;
#endif
#ifndef XM_T_xm_Surface
#define XM_T_xm_Surface
typedef struct xm_Surface {
    u16 h0;
    u8 pad2[2];
    s16 rings;              
    s16 polys;              
    s16 points;             
    s16 entryCount;         
    u8 b[6];                
    u8 pad12[2];
    SVECTOR *centres;       
    xm_SurfaceEntry *entries;  
    xm_SurfacePoint **strands; 
    xm_SurfacePoly *polyList;  
} xm_Surface;
#endif
#ifndef XM_T_xm_ColorRow
#define XM_T_xm_ColorRow
typedef struct {
    u16 c[3];
} xm_ColorRow;
#endif
#ifndef XM_T_xm_FrameCurve
#define XM_T_xm_FrameCurve
typedef s16 (*xm_FrameCurve)();
#endif
#ifndef XM_T_xm_ImageAnim
#define XM_T_xm_ImageAnim
typedef struct xm_ImageAnim {
    struct xm_ImageAnim *target; 
    u16 *pixels;              
    u16 *pixels2;             
    u16 *work;                
    u8 mode;                  
    u8 dirty;                 
    s16 size;                 
    u16 time;                 
    u16 speed;                
    u16 frame;                
    u16 active;               
    xm_ColorRow *colors;         
    s16 divisor;              
    s16 base;                 
    xm_FrameCurve curve;         
    RECT rect;                
} xm_ImageAnim;
#endif
#ifndef XM_T_xm_Actor
#define XM_T_xm_Actor
typedef struct xm_Actor {
    xm_ModelTable *models;     
    xm_ModelPart *parts;       
    s32 *entries;           
    xm_EntryTable *shared;     
    s32 pc;                 
    s32 *locals;            
    s32 *globals;           
    s16 scale;              
    s16 h1E;                
    u8 index;               
    u8 b21;                 
    u8 b22;                 
    u8 b23;                 
    s16 size[3];            
    u8 reference;           
    u8 depth;               
    u8 queue_source[4];     
    u8 queue_entry[4];      
    u8 active;              
    u8 b35;                 
    u8 b36;                 
    u8 scaled;              
    u8 b38;                 
    u8 b39;                 
    s16 h3A;                
    u16 h3C;                
    s16 h3E;                
    u16 h40;                
    u16 h42;                
    u16 h44;                
    u16 h46;                
    s16 h48;                
    u16 flags;              



    s32 w4C;                
    s32 w50;                
    s32 w54;                
    s16 aim_actor;          
    s16 aim_node;           
    u8 parent;              
    u8 inherit;             
    s16 parent_node;        
    s16 groundY;            


    u8 b62;                 
    u8 b63;                 
    s16 aim_offset[3];      
    s16 offset[3];          
    s16 spin[3];            
    s16 spin_accel[3];      
    s16 drift[3];           
    s16 drift_accel[3];     
    s16 target[3];          
    s16 h8E;                
    s16 h90;                
    s16 h92;                
    u16 shift_x;            
    u16 shift_y;            
    s16 anim_state;         
    s16 anim_loop;          
    s16 anim_frame;         
    s16 anim_frames;        
    u8 *anim_pos;           
    u8 *anim_start;         
    void *group;            
    void *blockAC;          
    xm_SoundOwner *ownerB0;    
    xm_SoundOwner *ownerB4;    
    POLY_FT4 prims[2];      
    u8 pad108[2];
    u16 mask;               
    u8 channel_count;       
    u8 surfaceCount;        
    u8 imageCount;          
    u8 pad10F;
    struct xm_ColorFade *channels; 
    struct xm_Surface *surfaces;   
    struct xm_ImageAnim *images;   
    s32 previous[3];        
    s32 moved[3];           

} xm_Actor;
#endif
#ifndef XM_S_field_actor_set_planar_velocity
#define XM_S_field_actor_set_planar_velocity
void xm_field_actor_set_planar_velocity(xm_Sprite *sprite, s16 heading, xm_FieldDescriptor *descriptor) __asm__("func_80081F80");
#endif
#ifndef XM_S_gpu_get_cos
#define XM_S_gpu_get_cos
s32 xm_gpu_get_cos(s32 angle) __asm__("rsin");
#endif
#ifndef XM_S_gpu_get_sin
#define XM_S_gpu_get_sin
s32 xm_gpu_get_sin(s32 angle) __asm__("rcos");
#endif
#ifndef XM_S_sprite_set_direction
#define XM_S_sprite_set_direction
void xm_sprite_set_direction(xm_Sprite *sprite, s32 direction) __asm__("func_80021FE0");
#endif
#ifndef XM_S_gear_model_actors
#define XM_S_gear_model_actors
extern xm_Actor *xm_gear_model_actors[10] __asm__("D_801E8670");
#endif
void xm_field_actor_set_planar_velocity(xm_Sprite *sprite, s16 heading, xm_FieldDescriptor *descriptor) __asm__("func_80081F80");
void xm_field_actor_set_planar_velocity(xm_Sprite *sprite, s16 heading, xm_FieldDescriptor *descriptor) {
    xm_FieldActor *actor;
    s32 layer;
    s32 speed;
    s32 angle;

    if (!(descriptor->flags & 0x40)) {
        speed = ((0x40000 / (u16)descriptor->actor->unk76) >> 8) << 5;
        angle = heading & 0xFFF;
        if (!(heading & 0x8000)) {
            sprite->speed_x = ((xm_gpu_get_cos(angle) * speed) >> 12) * descriptor->actor->scale[0];
            sprite->speed_z = (-(xm_gpu_get_sin(angle) * speed) >> 12) * descriptor->actor->scale[2];
        } else {
            sprite->speed_x = 0;
            sprite->speed_z = 0;
        }
    } else if (!(heading & 0x8000)) {
        actor = descriptor->actor;
        if (!(actor->layer_flags & 0x2000)) {
            if (!(actor->layer_flags & 0x80000)) {
                xm_sprite_set_direction(sprite, heading);
            } else {
                speed = ((0x40000 / (u16)actor->unk76) >> 8) << 5;
                angle = heading & 0xFFF;
                sprite->speed_x = ((xm_gpu_get_cos(angle) * speed) >> 12) * descriptor->actor->scale[0];
                sprite->speed_z = (-(xm_gpu_get_sin(angle) * speed) >> 12) * descriptor->actor->scale[2];
                sprite->speed = 0x4000000 / (u16)descriptor->actor->unk76;
            }
        } else if (!(actor->layer_flags & 0x20000)) {
            speed = ((0x80000 / (u16)actor->unk76) >> 8) << 5;
            angle = heading & 0xFFF;
            sprite->speed_x = ((xm_gpu_get_cos(angle) * speed) >> 12) * descriptor->actor->scale[0];
            sprite->speed_z = (-(xm_gpu_get_sin(angle) * speed) >> 12) * descriptor->actor->scale[2];
        } else {
            layer = actor->state.bits.layer;
            sprite->speed_x = -xm_gear_model_actors[layer]->moved[0] << 16;
            sprite->speed_z = -xm_gear_model_actors[layer]->moved[2] << 16;
        }
    } else {
        sprite->speed_x = 0;
        sprite->speed_z = 0;
    }
    sprite->speed_x &= ~0xFFF;
    sprite->speed_z &= ~0xFFF;
}
#endif
#else
void func_80081F80(void* pSpriteData, s16 angle, void* pFieldActor) {
    u8* pSprite = (u8*)pSpriteData;
    u8* pFA = (u8*)pFieldActor;
    u16 status = *(u16*)(pFA + 0x58);
    u8* pAD = NULL;
    u16 moveSpeed = 0;
    s32 stepMag = 0;
    s32 zeroStep = 0;
    s32 doStep = 0;

    /* Retail compiles `0x40000 / moveSpeed` as a bare MIPS `div` (which does
     * not fault on a zero divisor); the host needs the guard, the matching
     * build must not carry it (it adds branches retail does not have). */
#ifdef XENO_PC_PORT
#define FIELD_DIV(n, d) ((d) ? (s32)((n) / (d)) : 0)
#else
#define FIELD_DIV(n, d) ((s32)((n) / (d)))
#endif

    if ((status & 0x40) == 0) {
        pAD = (u8*)(uintptr_t)*(u32*)(pFA + 0x4C);
        moveSpeed = *(u16*)(pAD + 0x76);
        stepMag = (FIELD_DIV(0x40000, moveSpeed) >> 8) << 5;
        if ((u16)angle & 0x8000) {
            zeroStep = 1;
        } else {
            doStep = 1;
        }
    } else if ((u16)angle & 0x8000) {
        zeroStep = 1;
    } else {
        u32 flags;
        pAD = (u8*)(uintptr_t)*(u32*)(pFA + 0x4C);
        flags = *(u32*)(pAD + 0x04);
        if (flags & 0x2000) {
            if (flags & 0x20000) {
                /* asm .L80082158: step comes from a special-actor work object
                 * indexed by flags12C (D_801E8670 table). */
                u32 f12C = *(u32*)(pAD + 0x12C);
                u8* pEntry = (u8*)&D_801E8670 + ((f12C >> 11) & 0x1C);
                u8* pObj = (u8*)(uintptr_t)*(u32*)pEntry;
                *(s32*)(pSprite + 0xC) = (s32)(-*(s32*)(pObj + 0x128)) << 16;
                *(s32*)(pSprite + 0x14) = (s32)(-*(s32*)(pObj + 0x130)) << 16;
                goto clear;
            }
            moveSpeed = *(u16*)(pAD + 0x76);
            stepMag = (FIELD_DIV(0x80000, moveSpeed) >> 8) << 5;
            doStep = 1;
        } else if (flags & 0x80000) {
            /* asm .L8008203C: its own copy of the rsin/rcos step block
             * (retail does not share it with the paths above) plus pSprite+0x18. */
            s32 am = angle & 0xFFF;
            moveSpeed = *(u16*)(pAD + 0x76);
            stepMag = (FIELD_DIV(0x40000, moveSpeed) >> 8) << 5;
            *(s32*)(pSprite + 0xC) = ((rsin(am) * stepMag) >> 12) * *(s16*)(pAD + 0xF4);
            *(s32*)(pSprite + 0x14) = ((-(rcos(am) * stepMag)) >> 12) * *(s16*)(pAD + 0xF8);
            *(s32*)(pSprite + 0x18) = FIELD_DIV(0x4000000, moveSpeed);
            goto clear;
        } else {
            /* Fei's path: stamp the angle; func_80021FE0 -> func_80022974
             * computes the step from angle + sprite radius (spriteData+0x18). */
            func_80021FE0(pSpriteData, angle);
            goto clear;
        }
    }

    if (zeroStep) {
        *(s32*)(pSprite + 0xC) = 0;
        *(s32*)(pSprite + 0x14) = 0;
    } else if (doStep) {
        s32 am = angle & 0xFFF;
        *(s32*)(pSprite + 0xC) = ((rsin(am) * stepMag) >> 12) * *(s16*)(pAD + 0xF4);
        *(s32*)(pSprite + 0x14) = ((-(rcos(am) * stepMag)) >> 12) * *(s16*)(pAD + 0xF8);
    }

clear:
    /* asm .L800821BC: clear the low 12 (sub-pixel) bits of the X/Z step. */
    *(s32*)(pSprite + 0xC) &= ~0xFFF;
    *(s32*)(pSprite + 0x14) &= ~0xFFF;

#undef FIELD_DIV
}
#endif /* XENO_PC_PORT */

extern s16 D_800B2344;
extern s16 D_800B2346;
extern u8 D_800ADFB8[];
/* Field archive 0x6B9 object-animation entry. The native owner adapter keeps
 * the retail slot/animation contract while translating PSX pointer ownership. */
extern void func_801E8330(s32 objSlot, s32 unused, s32 animValue);

#ifndef XENO_PC_PORT
/* func_800821F4: retail treats animIndex as a full int (no sign-extension on entry
 * or at the func_800245D8 call), so the definition and that call go through
 * asm aliases with s32 parameters (the TU prototypes say s16; changing both TU
 * prototypes to s32 is the clean alternative).  D_800B2346 is not gp-small (its
 * address is kept in s2 and the anim-state base is s2-0x162, recomputed into a
 * fresh temp after each func_801E8330 call); actorData+0x4C is re-read after
 * each &= ~0x800 store (offset-0 store invalidates memory). */
extern s16 D_800B2346__a[] asm("D_800B2346");
void func_800245D8__i(void* pSpriteData, s32 animIndex) asm("func_800245D8");
void func_800821F4__i(void* pSpriteData, s32 animIndex, void* pFieldActor) asm("func_800821F4");

void func_800821F4__i(void* pSpriteData, s32 animIndex, void* pFieldActor) {
    u8* actor = pFieldActor;
    u8* actorData;
    u8* animState1;
    u8* animState2;
    u32 flags4;

    if (*(u16*)(actor + 0x58) & 0x40) {
        if (animIndex != 3 && D_800B2344 == 0) {
            *(u32*)(uintptr_t)*(u32*)(actor + 0x4C) &= ~0x800;
        }
        if (animIndex == 0xFF) {
            animIndex = 0;
        }
        if (animIndex != D_800B2346__a[0]) {
            *(u32*)(uintptr_t)*(u32*)(actor + 0x4C) &= ~0x800;
        }
        actorData = (u8*)(uintptr_t)*(u32*)(actor + 0x4C);
        flags4 = *(u32*)(actorData + 4);
        if (!(flags4 & 0x2000)) {
            if (!(flags4 & 0x01000000)) {
                func_800245D8__i(pSpriteData, animIndex);
            }
            return;
        }
        if (animIndex < 0x10) {
            func_801E8330((*(u32*)(actorData + 0x12C) >> 13) & 7, 0, D_800ADFB8[animIndex]);
            animState1 = (u8*)D_800B2346__a - 0x162;
            *(s16*)(((*(u32*)((u8*)(uintptr_t)*(u32*)(actor + 0x4C) + 0x12C) >> 12) & 0xE) + animState1) = D_800ADFB8[animIndex];
        } else {
            animIndex -= 0x10;
            func_801E8330((*(u32*)(actorData + 0x12C) >> 13) & 7, 0, animIndex);
            animState2 = (u8*)D_800B2346__a - 0x162;
            *(s16*)(((*(u32*)((u8*)(uintptr_t)*(u32*)(actor + 0x4C) + 0x12C) >> 12) & 0xE) + animState2) = animIndex;
        }
    }
}
#else
void func_800821F4(void* pSpriteData, s16 animIndex, void* pFieldActor) {
    u8* actor = pFieldActor;
    u8* actorData;
    s16 nextAnim = animIndex;
    u32 flags4;

    if ((*(u16*)(actor + 0x58) & 0x40) == 0) {
        return;
    }

    actorData = (u8*)(uintptr_t)*(u32*)(actor + 0x4C);

    if (nextAnim != 3 && D_800B2344 == 0) {
        *(u32*)(actorData + 0x00) &= ~0x800u;
    }

    if (nextAnim == 0xFF) {
        nextAnim = 0;
    }

    if (nextAnim != D_800B2346) {
        *(u32*)(actorData + 0x00) &= ~0x800u;
    }

    flags4 = *(u32*)(actorData + 0x04);
    if ((flags4 & 0x2000) == 0) {
        if ((flags4 & 0x01000000) == 0) {
            func_800245D8(pSpriteData, nextAnim);
        }
        return;
    }

    /* asm .L800822D8-.L80082360: battle-animation branch (flags4 & 0x2000 set).
     * The actor carries an object slot in bits 13..15 of its +0x12C word (the
     * slot func_800A1364 stamped). Drive that object's anim through the native
     * archive-0x6B9 owner entry func_801E8330, then
     * record the resolved anim index into the per-slot anim-state array at
     * &D_800B2346 - 0x162, indexed by bits 12..15 (even) of the same word.
     * animIndex < 0x10 uses the D_800ADFB8[] remap; >= 0x10 subtracts 0x10. */
    {
        u8* animState = (u8*)&D_800B2346 - 0x162;
        u32 objSlot = (*(u32*)(actorData + 0x12C) >> 13) & 7;

        if (nextAnim < 0x10) {
            u8 mapped = D_800ADFB8[nextAnim];
            func_801E8330(objSlot, 0, mapped);
            actorData = (u8*)(uintptr_t)*(u32*)(actor + 0x4C);
            *(u16*)(animState + ((*(u32*)(actorData + 0x12C) >> 12) & 0xE)) =
                mapped;
        } else {
            s16 anim2 = nextAnim - 0x10;
            func_801E8330(objSlot, 0, anim2);
            actorData = (u8*)(uintptr_t)*(u32*)(actor + 0x4C);
            *(u16*)(animState + ((*(u32*)(actorData + 0x12C) >> 12) & 0xE)) =
                (u16)anim2;
        }
    }
}
#endif /* XENO_PC_PORT */

s32 func_8008237C(s32 x, s32 z, void* pActorData, s32 extraRadius) {
#ifdef XENO_PC_PORT
    u8* actorData = pActorData;
    s32 xMin;
    s32 xMax;
    s32 zMin;
    s32 zMax;
    long actorPos;
    long p0;
    long p1;
    long p2;
    long p3;

    actorPos = (x << 16) + z;
    xMin = *(s16*)(actorData + 0x22) - *(u16*)(actorData + 0x18) - extraRadius;
    xMax = *(s16*)(actorData + 0x22) + *(u16*)(actorData + 0x18) + extraRadius;
    zMin = *(s16*)(actorData + 0x2A) - *(u16*)(actorData + 0x1C) - extraRadius;
    zMax = *(s16*)(actorData + 0x2A) + *(u16*)(actorData + 0x1C) + extraRadius;

    p0 = (xMin << 16) + zMax;
    p1 = (xMax << 16) + zMax;
    p2 = (xMax << 16) + zMin;
    p3 = (xMin << 16) + zMin;

    if (NormalClip(p0, p1, actorPos) < 0 ||
        NormalClip(p1, p2, actorPos) < 0 ||
        NormalClip(p2, p3, actorPos) < 0 ||
        NormalClip(p3, p0, actorPos) < 0) {
        return -1;
    }

    if (g_FieldSystemMode == 0) {
        func_80281678(actorData);
    }
    return 0;
#else
    s16 temp_v0;
    s16 temp_v1;
    s32 temp_a1;
    s32 temp_s0;
    s32 temp_s1;
    s32 temp_s3;
    s32 temp_s4;
    s32 temp_s5;
    s32 temp_t0;
    s32 temp_v0_2;
    s32 temp_v1_2;
    u16 temp_t1;
    u16 temp_t2;

    temp_s3 = (x << 0x10) + z;
    temp_v1 = *(s16 *)((s8*)(pActorData) + 0x22);
    temp_t1 = *(u16 *)((s8*)(pActorData) + 0x18);
    temp_v0 = *(s16 *)((s8*)(pActorData) + 0x2A);
    temp_t2 = *(u16 *)((s8*)(pActorData) + 0x1C);
    temp_t0 = ((temp_v1 - temp_t1) - extraRadius) << 0x10;
    temp_a1 = temp_v0 + temp_t2 + extraRadius;
    temp_s5 = temp_t0 + temp_a1;
    temp_v1_2 = (temp_v1 + temp_t1 + extraRadius) << 0x10;
    temp_s0 = temp_v1_2 + temp_a1;
    temp_v0_2 = (temp_v0 - temp_t2) - extraRadius;
    temp_s1 = temp_v1_2 + temp_v0_2;
    temp_s4 = temp_t0 + temp_v0_2;
    if ((NormalClip(temp_s5, temp_s0, temp_s3) < 0) || (NormalClip(temp_s0, temp_s1, temp_s3) < 0) || (NormalClip(temp_s1, temp_s4, temp_s3) < 0) || (NormalClip(temp_s4, temp_s5, temp_s3) < 0)) {
        return -1;
    }
    if (g_FieldSystemMode == 0) {
        func_80281678(pActorData);
    }
    return 0;
#endif
}

s32 func_80082494(s32* pVec, u8* actorData) {
    s16* clipPoints;
    s32 x;
    s32 z;
    s32 position;
    s32 point0;
    s32 point1;
    s32 point2;
    s32 point3;

    if ((*(u32*)(actorData + 0x12C) & 0x1000) == 0) {
        return 0;
    }

    x = (*(s32*)(actorData + 0x20) + pVec[0]) >> 16;
    z = (*(s32*)(actorData + 0x28) + pVec[2]) >> 16;
    position = (x << 16) + z;

    clipPoints = (s16*)(uintptr_t)*(u32*)(actorData + 0x114);
    point0 = ((s32)clipPoints[0] << 16) + clipPoints[1];
    point1 = ((s32)clipPoints[2] << 16) + clipPoints[3];
    point2 = ((s32)clipPoints[4] << 16) + clipPoints[5];
    point3 = ((s32)clipPoints[6] << 16) + clipPoints[7];

    if (NormalClip(point0, point1, position) < 0 ||
        NormalClip(point1, point2, position) < 0 ||
        NormalClip(point2, point3, position) < 0) {
        return -1;
    }
    return NormalClip(point3, point0, position) >> 31;
}

extern long FieldGetVec2Magnitude(long x, long y);

/* ---- func_800825AC: 2D distance between two field actors ------------------
 * Faithful port of asm/.../misc8/func_800825AC.s (0x74 bytes). Reads each
 * actor's ActorData (FieldActor stride 0x5C, pActorData at +0x4C) integer XZ
 * position (+0x22 / +0x2A) and returns FieldGetVec2Magnitude of the delta.
 * Used by func_80082620's actor-follow branch. */
#ifndef XENO_PC_PORT
s32 func_800825AC(s32 actorA, s32 actorB) {
    u8 *dataA;
    u8 *dataB;
    s32 bx;
    s32 bz;

    dataB = *(u8 **)((u8 *)g_FieldActors + actorB * 0x5C + 0x4C);
    dataA = *(u8 **)((u8 *)g_FieldActors + actorA * 0x5C + 0x4C);
    bx = *(s16 *)(dataB + 0x22);
    bz = *(s16 *)(dataB + 0x2A);
    return FieldGetVec2Magnitude(bx - *(s16 *)(dataA + 0x22), bz - *(s16 *)(dataA + 0x2A));
}
#else
s32 func_800825AC(s32 actorA, s32 actorB) {
    u8* dataA = (u8*)(uintptr_t)*(u32*)((u8*)g_FieldActors + actorA * 0x5C + 0x4C);
    u8* dataB = (u8*)(uintptr_t)*(u32*)((u8*)g_FieldActors + actorB * 0x5C + 0x4C);
    s32 dx = *(s16*)(dataB + 0x22) - *(s16*)(dataA + 0x22);
    s32 dz = *(s16*)(dataB + 0x2A) - *(s16*)(dataA + 0x2A);
    return FieldGetVec2Magnitude(dx, dz);
}
#endif /* XENO_PC_PORT */

extern u16 D_800ADFA8[];
extern s16 D_800ADFC4[];
extern void func_8007B614(s32* pOut, s16 scale, s16 angle);

/* ---- func_80082620: environmental / collision-aware auto-move -------------
 * Faithful port of asm/.../misc8/func_80082620.s (0x598 bytes). Beyond the
 * simple auto-move (LUT scale/angle -> func_8007B614), it now implements the
 * tail branches that the corrected walkmesh material (moveFlags = +0x14) drives
 * actors into: the slope-normal projected move (.L800826FC/.L80082820), the
 * actor-follow branch (.L800828E0: HeapAlloc work object + func_800825AC +
 * ratan2/rsin/rcos), and the D_801E8670 work-object divide path (.L80082AF4).
 * gotos mirror the shared asm labels; the previous "unsupported branch" assert
 * is now covered by real retail behavior. */
#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): field_actor_apply_additive_motion (src/decomp/src/field/field_motion.c:2838). */
#ifndef XM_S_field_actor_apply_additive_motion
#define XM_S_field_actor_apply_additive_motion
void xm_field_actor_apply_additive_motion(s32 index, xm_FieldDescriptor *descriptor, xm_FieldActor *actor) __asm__("func_80082620");
#endif
#ifndef XM_S_field_terrain_push_speeds
#define XM_S_field_terrain_push_speeds
extern s16 xm_field_terrain_push_speeds[4] __asm__("D_800ADFC4");
#endif
#ifndef XM_S_field_compute_offset_at_angle
#define XM_S_field_compute_offset_at_angle
void xm_field_compute_offset_at_angle(VECTOR *out, s32 distance, s32 angle) __asm__("func_8007B614");
#endif
#ifndef XM_S_field_terrain_push_angles
#define XM_S_field_terrain_push_angles
extern u16 xm_field_terrain_push_angles[8] __asm__("D_800ADFA8");
#endif
#ifndef XM_S_VectorNormal
#define XM_S_VectorNormal
long xm_VectorNormal(VECTOR *v0, VECTOR *v1) __asm__("VectorNormal");
#endif
#ifndef XM_S_field_actor_get_planar_distance
#define XM_S_field_actor_get_planar_distance
s32 xm_field_actor_get_planar_distance(s32 from, s32 to) __asm__("func_800825AC");
#endif
#ifndef XM_S_ratan2
#define XM_S_ratan2
long xm_ratan2(long y, long x) __asm__("ratan2");
#endif
#ifndef XM_S_field_layer_next_index
#define XM_S_field_layer_next_index
extern s32 xm_field_layer_next_index __asm__("D_800AF858");
#endif
void xm_field_actor_apply_additive_motion(s32 index, xm_FieldDescriptor *descriptor, xm_FieldActor *actor) __asm__("func_80082620");
void xm_field_actor_apply_additive_motion(s32 index, xm_FieldDescriptor *descriptor, xm_FieldActor *actor) {
    u32 terrain;
    VECTOR conveyor;
    VECTOR slope;
    VECTOR normal;
    VECTOR unused;
    SVECTOR turn;
    xm_Sprite *model;
    xm_FieldActor *platform;
    xm_Actor **entry;
    s32 push_x;
    s32 push_z;
    s32 speed;
    s32 limit;
    s32 self_x;
    s32 self_z;
    s32 other_x;
    s32 other_z;
    s32 radius;
    s32 angle;
    s32 dir;
    s32 motion_value;
    s16 rotation_delta;

    push_x = 0;
    push_z = 0;
    terrain = 0;
    if (!((actor->layer_flags >> (actor->layer + 3)) & 1) && xm_field_work.party_processing_mode == 0) {
        terrain = actor->floor_attribute;
    }
    model = descriptor->model;
    motion_value = xm_field_terrain_push_speeds[(terrain >> 9) & 3];
    xm_field_compute_offset_at_angle(&conveyor, motion_value,
                  (xm_field_terrain_push_angles[(terrain >> 11) & 7] + (u16)xm_field_work.terrain_angle) & 0xFFF);
    if (!(actor->flags & 0x41800)) {
        speed = actor->unkF0;
        if (terrain & 0x420000) {
            slope.vx = -(actor->unk50[0] * actor->unk50[1]) >> 15;
            slope.vy = 0;
            slope.vz = -(actor->unk50[2] * actor->unk50[1]) >> 15;
            if (slope.vx == 0) {
                slope.vx = 1;
            }
            slope.vy = 1;
            if (slope.vz == 0) {
                slope.vz = 1;
            }
            xm_VectorNormal(&slope, &normal);
            if (normal.vx == 0) {
                normal.vx = 1;
            }
            if (normal.vy == 0) {
                normal.vy = 1;
            }
            if (normal.vz == 0) {
                normal.vz = 1;
            }
            push_x = normal.vx * (speed >> 17) << 4;
            push_z = normal.vz * (speed >> 17) << 4;
            limit = 0xC;
            if (terrain & 0x400000) {
                limit = 0x18;
            }
            if ((actor->unkF0 >> 16) >= limit) {
                actor->unkF0 = limit << 16;
            } else {
                actor->unkF0 += model->gravity;
            }
            model->speed_y = actor->unkF0 >> 1;
        }
        if (terrain & 0x400000) {
            actor->unk40[0] += push_x;
            actor->unk40[2] += push_z;
            actor->heading |= 0x8000;
        }
        if (actor->ridden_actor != 0xFF) {
            goto linked;
        }
        if (terrain & 0x20000) {
            actor->unk40[0] += push_x;
            actor->unk40[2] += push_z;
        }
        if (!(terrain & 0x8000)) {
            goto conveyed;
        }
    } else if (!(terrain & 0x4000)) {
        goto conveyed;
    }
    actor->unk40[0] += conveyor.vx;
    actor->unk40[1] += conveyor.vy;
    actor->unk40[2] += conveyor.vz;
conveyed:
    if (actor->ridden_actor != 0xFF) {
    linked:
        if ((xm_field_view.components.descriptors[actor->ridden_actor].actor->layer_flags & 0xC0) == 0xC0) {
            if (!(actor->unk134 & 0x80)) {
                actor->link = xm_heap_alloc(sizeof(xm_PlatformLink), 0);
                actor->unk134 |= 0x80;
            }
            turn.vx = xm_field_view.components.descriptors[actor->ridden_actor].rotation.vx - actor->link->rotation.vx;
            turn.vy = rotation_delta = xm_field_view.components.descriptors[actor->ridden_actor].rotation.vy - actor->link->rotation.vy;
            turn.vz = xm_field_view.components.descriptors[actor->ridden_actor].rotation.vz - actor->link->rotation.vz;
            actor->link->rotation.vy = xm_field_view.components.descriptors[actor->ridden_actor].rotation.vy;
            self_x = actor->position[0];
            self_z = actor->position[2];
            platform = xm_field_view.components.descriptors[actor->ridden_actor].actor;
            other_x = platform->position[0];
            other_z = platform->position[2];
            angle = rotation_delta;
            if (!(actor->heading & 0x8000)) {
                actor->link->radius = xm_field_actor_get_planar_distance(index, actor->ridden_actor);
            }
            radius = actor->link->radius;
            motion_value = (s16)xm_ratan2(other_z - self_z, other_x - self_x) - angle;
            dir = motion_value - 0x800;
            actor->unk40[0] += other_x + xm_gpu_get_cos(dir) * radius * 16 - self_x;
            actor->unk40[2] += other_z + xm_gpu_get_sin(dir) * radius * 16 - self_z;
        }
    }
    if ((actor->layer_flags & 0x22000) == 0x22000) {
        entry = &xm_gear_model_actors[xm_field_layer_next_index];
        actor->unk40[0] -= (((*entry)->moved[0] << 16) / (u16)actor->unk76) << 8;
        actor->unk40[2] -= (((*entry)->moved[2] << 16) / (u16)actor->unk76) << 8;
        xm_field_layer_next_index++;
    }
}
#endif
#else
void func_80082620(s32 actorIndex, void* pFieldActor, void* pActorData) {
    u8* actor = pFieldActor;
    u8* actorData = pActorData;
    u8* spriteData = (u8*)(uintptr_t)*(u32*)(actor + 0x04);
    s32 moveVec[3];
    u32 moveFlags = 0;
    u32 flags0 = *(u32*)(actorData + 0x00);
    u32 flags4 = *(u32*)(actorData + 0x04);
    s32 shift = *(s16*)(actorData + 0x10) + 3;
    s16 scale;
    u16 angle;
    s32 s0move = 0;   /* $s0: X slope-move accumulator */
    s32 s3move = 0;   /* $s3: Z slope-move accumulator */
    s32 f0val;        /* $s4: actorData+0xF0 snapshot */
    s32 addTest;      /* moveVec-add gate (0x4000 via flags0 path, else 0x8000) */

    if (((flags4 >> shift) & 1) == 0) {
        if (D_800B21CC == 0) {
            moveFlags = *(u32*)(actorData + 0x14);
        }
    }

    scale = D_800ADFC4[((moveFlags >> 8) & 0x6) >> 1];
    angle = (D_800ADFA8[((moveFlags >> 10) & 0xE) >> 1] +
             *(u16*)((u8*)&g_FieldControl + 0x2)) & 0x0FFF;
    func_8007B614(moveVec, scale, angle);

    if ((flags0 & 0x00041800) != 0) {
        /* asm 800826E4: skip the slope path; gate the moveVec add on 0x4000 */
        addTest = moveFlags & 0x00004000;
        goto apply_movevec;
    }

    /* ---- main path (asm .L800826EC) ---- */
    f0val = *(s32*)(actorData + 0xF0);
    if ((moveFlags & 0x00420000) != 0) {
        /* slope-normal projected move (asm .L800826FC-.L8008281C) */
        VECTOR nIn;
        VECTOR nOut;
        s32 nvx = *(s32*)(actorData + 0x50);
        s32 nvy = *(s32*)(actorData + 0x54);
        s32 nvz = *(s32*)(actorData + 0x58);
        s32 vshift;
        s32 bound;
        s32 nf0;

        nIn.vx = (-(nvx * nvy)) >> 15;
        nIn.vz = (-(nvz * nvy)) >> 15;
        if (nIn.vx == 0) {
            nIn.vx = 1;
        }
        nIn.vy = 1; /* asm zeroes sp+0x24 then unconditionally stores 1 */
        if (nIn.vz == 0) {
            nIn.vz = 1;
        }
        VectorNormal(&nIn, &nOut);
        if (nOut.vx == 0) {
            nOut.vx = 1;
        }
        if (nOut.vy == 0) {
            nOut.vy = 1;
        }
        if (nOut.vz == 0) {
            nOut.vz = 1;
        }

        vshift = f0val >> 17;
        s0move = (nOut.vx * vshift) << 4;
        s3move = (nOut.vz * vshift) << 4;
        bound = ((moveFlags & 0x00400000) != 0) ? 0x18 : 0xC;

        if ((f0val >> 16) < bound) {
            nf0 = f0val + *(s32*)(spriteData + 0x1C);
        } else {
            nf0 = bound << 16;
        }
        *(s32*)(actorData + 0xF0) = nf0;
        *(s32*)(spriteData + 0x10) = *(s32*)(actorData + 0xF0) >> 1;
    }

    /* asm .L80082820 */
    if ((moveFlags & 0x00400000) != 0) {
        *(s32*)(actorData + 0x40) += s0move;
        *(u16*)(actorData + 0x104) |= 0x8000;
        *(s32*)(actorData + 0x48) += s3move;
    }

    /* asm .L80082850 */
    if (*(u8*)(actorData + 0x74) == 0xFF) {
        if ((moveFlags & 0x00020000) != 0) {
            *(s32*)(actorData + 0x40) += s0move;
            *(s32*)(actorData + 0x48) += s3move;
        }
        addTest = moveFlags & 0x00008000;
        goto apply_movevec;
    }
    goto follow_actor;

apply_movevec: /* asm .L8008288C */
    if (addTest != 0) {
        *(s32*)(actorData + 0x40) += moveVec[0];
        *(s32*)(actorData + 0x44) += moveVec[1];
        *(s32*)(actorData + 0x48) += moveVec[2];
    }
    /* asm .L800828D0 */
    if (*(u8*)(actorData + 0x74) == 0xFF) {
        goto work_object;
    }
    /* else fall through to the actor-follow branch */

follow_actor: /* asm .L800828E0 */
    {
        s32 followIdx = *(u8*)(actorData + 0x74);
        u8* followFA = (u8*)g_FieldActors + followIdx * 0x5C;
        u8* followData = (u8*)(uintptr_t)*(u32*)(followFA + 0x4C);
        u8* work;
        s32 myX;
        s32 myZ;
        s32 followX;
        s32 followZ;
        s32 s0y;   /* $s0: change in the followed actor's +0x52 bound */
        s32 dist;  /* $s1 */
        s32 followAngle;

        if ((*(u32*)(followData + 0x04) & 0xC0) != 0xC0) {
            goto work_object;
        }

        if ((*(u32*)(actorData + 0x134) & 0x80) == 0) {
            *(u32*)(actorData + 0x110) = (u32)(uintptr_t)HeapAlloc(0xC, 0);
            *(u32*)(actorData + 0x134) |= 0x80;
        }

        /* asm .L80082944 */
        work = (u8*)(uintptr_t)*(u32*)(actorData + 0x110);
        s0y = (s16)(*(u16*)(followFA + 0x52) - *(u16*)(work + 0x2));
        *(u16*)(work + 0x2) = *(u16*)(followFA + 0x52);

        myX = *(s32*)(actorData + 0x20);
        myZ = *(s32*)(actorData + 0x28);
        followX = *(s32*)(followData + 0x20);
        followZ = *(s32*)(followData + 0x28);

        if ((*(s16*)(actorData + 0x104) & 0x8000) == 0) {
            *(u16*)(work + 0x8) = (u16)func_800825AC(actorIndex, followIdx);
        }
        dist = *(s16*)(work + 0x8);

        followAngle = ratan2(followZ - myZ, followX - myX);
        followAngle = ((s16)followAngle - s0y) - 0x800;

        *(s32*)(actorData + 0x40) +=
            (followX + ((rsin(followAngle) * dist) << 4)) - myX;
        *(s32*)(actorData + 0x48) +=
            (followZ + ((rcos(followAngle) * dist) << 4)) - myZ;
    }

work_object: /* asm .L80082AF4 */
    if ((flags4 & 0x00022000) == 0x00022000) {
        s32 idx = D_800AF858;
        u8* obj = (u8*)(uintptr_t)D_801E8670[idx];
        u16 speed = *(u16*)(actorData + 0x76);

        *(s32*)(actorData + 0x40) -=
            ((*(s32*)(obj + 0x128) << 16) / speed) << 8;
        D_800AF858 = idx + 1;
        *(s32*)(actorData + 0x48) -=
            ((*(s32*)(obj + 0x130) << 16) / speed) << 8;
    }
}
#endif /* XENO_PC_PORT */

extern s32 D_8005A448;
extern s32 D_8005A44C;
extern s16 D_800B2344;
extern s16 D_800B2346;
extern s32 D_800ADB68;
extern s32 D_800ADB98;
extern u16 D_800AFE9C;
extern void* func_8007B814(s32* pVec, void* pActorData, s32* pOut, s16 angle);
extern void* func_8007BAC0(s32* pVec, void* pActorData, s32* pOut, s16 angle);

extern s32 D_80065B08;

#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc8", func_80082BB8);
#else
void func_80082BB8(s32 actorIndex, void* pFieldActor, void* pActorData) {
    u8* actor = pFieldActor;
    u8* actorData = pActorData;
    u8* spriteData = (u8*)(uintptr_t)*(u32*)(actor + 0x04);
    s16 angle = *(s16*)(actorData + 0x104);
    s32 anim = 1;
    s32 moveOrBlocked;
    s32 vec[3];
    s32 out[4];
    void* result = (void*)-1;
    u32 flags0;
    u32 flags4;

    D_80065B08 = actorIndex;

    flags0 = *(u32*)(actorData + 0x00);
    if ((flags0 & 0x01000000) != 0) {
        return;
    }

    if ((flags0 & 0x4000) != 0 &&
        (D_800AFE9C & 0x40) != 0 &&
        D_800ADB68 == 1) {
        anim = 2;
    }

    if ((flags0 & 0x1800) != 0) {
        s16 curAnim = *(s16*)(actorData + 0xE8);
        if (curAnim != anim) {
            if (curAnim == 1) {
                anim = 1;
            } else if (curAnim == 2) {
                anim = 2;
            }
        }
    }

    if (*(u8*)(actorData + 0xE3) >= 9) {
        *(u8*)(actorData + 0xE3) -= 1;
    }

    moveOrBlocked = *(s32*)(actorData + 0x40) |
                    *(s32*)(actorData + 0x44) |
                    *(s32*)(actorData + 0x48);
    if (func_8008492C(actorData) == -1) {
        moveOrBlocked = 1;
    }

    if ((angle & 0x8000) != 0 && moveOrBlocked == 0) {
        if ((*(u32*)(actorData + 0x00) & 0x00040800) == 0) {
            *(u32*)(actorData + 0xF0) = 0x00010000;
            *(s32*)(actorData + 0x40) = 0;
            *(s32*)(actorData + 0x44) = 0;
            *(s32*)(actorData + 0x48) = 0;
            vec[0] = 0;
            vec[1] = 0;
            vec[2] = 0;
            *(s32*)(spriteData + 0x0C) = 0;
            *(s32*)(spriteData + 0x14) = 0;
            *(u16*)(actorData + 0x106) |= 0x8000;
            goto update_animation;
        }

        anim = *(u16*)(actorData + 0xE6);
        *(u16*)(actorData + 0x104) |= 0x8000;
        *(u32*)(actorData + 0xF0) = 0x00010000;
        *(s32*)(actorData + 0x40) = 0;
        *(s32*)(actorData + 0x44) = 0;
        *(s32*)(actorData + 0x48) = 0;
        vec[0] = 0;
        vec[1] = 0;
        vec[2] = 0;
        *(s32*)(spriteData + 0x0C) = 0;
        *(s32*)(spriteData + 0x14) = 0;
        *(u16*)(actorData + 0x106) |= 0x8000;
        goto update_animation;
    }

    if ((angle & 0x8000) == 0) {
        func_80081F80(spriteData, angle, actor);
        vec[0] = *(s32*)(spriteData + 0x0C) + *(s32*)(actorData + 0x40);
        vec[1] = *(s32*)(spriteData + 0x10) + *(s32*)(actorData + 0x44);
        vec[2] = *(s32*)(spriteData + 0x14) + *(s32*)(actorData + 0x48);
        *(s16*)(actorData + 0x106) = angle;
    } else {
        vec[0] = *(s32*)(actorData + 0x40);
        vec[1] = *(s32*)(actorData + 0x44);
        vec[2] = *(s32*)(actorData + 0x48);
        angle = *(u16*)(actorData + 0x106) & 0x0FFF;
    }

    if (func_80082494(vec, actorData) != 0) {
        *(u32*)(actorData + 0xF0) = 0x00010000;
        *(s32*)(actorData + 0x40) = 0;
        *(s32*)(actorData + 0x44) = 0;
        *(s32*)(actorData + 0x48) = 0;
        vec[0] = 0;
        vec[1] = 0;
        vec[2] = 0;
        *(s32*)(spriteData + 0x0C) = 0;
        *(s32*)(spriteData + 0x14) = 0;
        *(u16*)(actorData + 0x106) |= 0x8000;
        goto update_animation;
    }

    if (vec[0] != 0 || vec[2] != 0) {
        angle = (-ratan2(vec[2], vec[0])) & 0x0FFF;
    }

    {
        s16 stateIdx = *(s16*)(actorData + 0x10);
        s16 stateVal = *(s16*)(actorData + 0x08 + stateIdx * 2);

        if (stateVal != -1) {
            u32 savedFlags0;

            if (actorIndex == g_PlayerActorIndex) {
                if (D_8005A448 != 0xFF) {
                    u8* other = (u8*)g_FieldActors + D_8005A448 * 0x5C;
                    u8* otherData = (u8*)(uintptr_t)*(u32*)(other + 0x4C);
                    *(u32*)(actorData + 0x00) |= *(u32*)(otherData + 0x00) & 0x600;
                }
                if (D_8005A44C != 0xFF) {
                    u8* other = (u8*)g_FieldActors + D_8005A44C * 0x5C;
                    u8* otherData = (u8*)(uintptr_t)*(u32*)(other + 0x4C);
                    *(u32*)(actorData + 0x00) |= *(u32*)(otherData + 0x00) & 0x600;
                }
            }

            savedFlags0 = *(u32*)(actorData + 0x00);
            if ((savedFlags0 & 0x00041800) != 0 ||
                *(u8*)(actorData + 0x74) != 0xFF ||
                D_800ADB98 != 0) {
                result = func_8007B814(vec, actorData, out, angle);
            } else {
                result = func_8007BAC0(vec, actorData, out, angle);
            }

            *(u32*)(actorData + 0x00) =
                (*(u32*)(actorData + 0x00) & ~0x600u) | (savedFlags0 & 0x600);
        }
    }

    if (result != (void*)-1) {
        *(u32*)(actorData + 0x04) &= ~0x1000u;
        goto update_animation;
    }

    *(u32*)(actorData + 0xF0) = 0x00010000;
    *(s32*)(actorData + 0x40) = 0;
    *(s32*)(actorData + 0x44) = 0;
    *(s32*)(actorData + 0x48) = 0;
    vec[0] = 0;
    vec[1] = 0;
    vec[2] = 0;
    *(s32*)(spriteData + 0x0C) = 0;
    *(s32*)(spriteData + 0x14) = 0;
    *(u16*)(actorData + 0x106) |= 0x8000;

update_animation:
    flags4 = *(u32*)(actorData + 0x04) & ~0x1000u;
    *(u32*)(actorData + 0x04) = flags4;

    if ((*(u32*)(actorData + 0x00) & 0x800) != 0) {
        if (D_800B2344 == 0 &&
            *(s16*)(spriteData + 0x06) != *(s16*)(spriteData + 0x84)) {
            if ((s16)anim == 2) {
                *(s32*)(spriteData + 0x18) =
                    ((s32)*(s16*)(spriteData + 0x82) * 3) << 5;
            } else {
                *(s32*)(spriteData + 0x18) =
                    ((s32)*(s16*)(spriteData + 0x82) * 3) << 4;
            }
        } else if (D_800B2344 == 0) {
            *(s32*)(spriteData + 0x18) = 0;
        }
        anim = D_800B2346;
    } else {
        if ((*(s16*)(actorData + 0x104) & 0x8000) != 0) {
            anim = *(u16*)(actorData + 0xE6);
        }

        if ((func_80080968(actorData) & 0x00200000) != 0) {
            if ((*(s16*)(actorData + 0x104) & 0x8000) == 0 ||
                *(s16*)(actorData + 0xE8) != 6) {
                anim = 6;
            } else {
                *(u32*)(actorData + 0x04) |= 0x1000u;
                anim = 6;
            }
        }
    }

    if (*(s16*)(actorData + 0xEA) != 0xFF) {
        anim = *(s16*)(actorData + 0xEA);
    }

    if (*(s16*)(actorData + 0xE8) != (s16)anim &&
        (*(u32*)(actorData + 0x00) & 0x02000000) == 0) {
        *(s16*)(actorData + 0xE8) = anim;
        func_800821F4(spriteData, anim, actor);
    }

    if ((*(u32*)(actorData + 0x14) & 0x100) != 0) {
        vec[0] >>= 1;
        vec[2] >>= 1;
    }

    *(s32*)(actorData + 0x30) = vec[0];
    *(s32*)(actorData + 0x34) = vec[1];
    *(s32*)(actorData + 0x38) = vec[2];
    *(s32*)(actorData + 0x40) = 0;
    *(s32*)(actorData + 0x44) = 0;
    *(s32*)(actorData + 0x48) = 0;
}
#endif /* XENO_PC_PORT */

s32 func_80083178(s32 current, s32 target) {
    if (current < 0) {
        current += 0x4000;
        target = -target;
        if (current < target) current = target;
        if (current > 0) current = 0;
    } else {
        current -= 0x4000;
        if (current > target) current = target;
        if (current < 0) current = 0;
    }
    return current;
}

/* 16.16 fixed-point VECTOR -> SVECTOR integer parts (retail loads the high
 * halves with lh). */
void func_800831D0(SVECTOR* out, s32* in) {
    out->vx = in[0] >> 16;
    out->vy = in[1] >> 16;
    out->vz = in[2] >> 16;
}

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): field_actor_turn_quarter (src/decomp/src/field/field_motion.c:3153). */
#ifndef XM_S_field_actor_turn_quarter
#define XM_S_field_actor_turn_quarter
void xm_field_actor_turn_quarter(void *owner, xm_FieldActor *actor, xm_FieldDescriptor *descriptor, s32 flags) __asm__("func_800831F4");
#endif
void func_800831F4(void *owner, xm_FieldActor *actor, xm_FieldDescriptor *descriptor, s32 flags) {
    s16 heading;
    s16 turned;

    if (actor->unk030[0] != 0 || actor->unk030[2] != 0) {
        heading = actor->heading_goal;
        if (!(heading & 0x8000)) {
            if (flags & 1) {
                turned = heading - 0x400;
                actor->heading = turned & 0xFFF;
                actor->heading_goal = turned & 0xFFF;
            } else {
                turned = heading + 0x400;
                actor->heading = turned & 0xFFF;
                actor->heading_goal = turned & 0xFFF;
            }
            xm_field_actor_set_planar_velocity(owner, actor->heading, descriptor);
            actor->heading_goal = actor->heading = actor->heading_goal | 0x8000;
        }
    }
}
#endif
#else
void func_800831F4(void* pSpriteData, u8* pActor, void* pFieldActor, s32 flags) {
    u32 val30 = *(u32*)(pActor + 0x30);
    u32 val38 = *(u32*)(pActor + 0x38);
    if (val30 != 0 || val38 != 0) {
        s16 curAngle = *(s16*)(pActor + 0x106);
        if (!(curAngle & 0x8000)) {
            u16 newAngle;
            if (flags & 1) {
                newAngle = (u16)(curAngle - 0x400);
            } else {
                newAngle = (u16)(curAngle + 0x400);
            }
            newAngle &= 0xFFF;
            *(u16*)(pActor + 0x104) = newAngle;
            *(u16*)(pActor + 0x106) = newAngle;
            func_80081F80(pSpriteData, *(s16*)(pActor + 0x104), pFieldActor);
            {
                u16 final = *(u16*)(pActor + 0x106) | 0x8000;
                *(u16*)(pActor + 0x104) = final;
                *(u16*)(pActor + 0x106) = final;
            }
        }
    }
}
#endif /* XENO_PC_PORT */

extern void* func_8007CD3C(s32 arg0);
extern void func_8007CD60(s32 arg0);
extern void func_8007B07C(s16* arg0, s16* arg1, s16* arg2, s16* arg3, VECTOR* arg4);
extern MATRIX D_800AFC30;
/* GTE prototypes come through field/actor.h -> psyq/libgte.h (shimmed to
 * PsyCross's on the port build). */

/* Point-vs-actor-collision-mesh test (asm 80083288, "POLYCHECK"). Composes
 * the owning actor's model matrix - rotated about one axis by +0x70 when
 * +0x12C mode is 1/2/3, else the static/parented composition through the
 * camera matrix D_800AFC30 (and the +0x75 parent actor's +0x2C matrix when
 * set) - loads it into the GTE, then walks the mesh's primitive groups
 * (header word: type byte, count in the top half; types 0xC4/0xC8 skipped;
 * type bit 3 selects quads). Each tri/quad is transformed with RotTransSV
 * and tested with NormalClip winding checks against the packed (x<<16)|z
 * query point; for containing polygons func_8007B07C interpolates the
 * surface Y at (x, z) (also filling pOut2, which the caller consumes as the
 * contact data). Returns 0 with *pOutY = the minimum such Y, or -1 when no
 * polygon contains the point. All locals live in a func_8007CD3C(0xB8)
 * scratch block, mirroring the retail layout. */
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc8", func_80083288);
#else
s32 func_80083288(s32 ownIndex, void* pMesh, s32 curX, s32 curZ, s32* pOutY,
                  void* pOut2) {
    /* Retail places the workspace in the PSX scratchpad via
     * func_8007CD3C(0xB8) (returns 0x1F800000-based pointers, unmapped on
     * the PC port). Follow func_80084158's port convention: keep the arena
     * push/pop balanced but back the workspace with a local. */
    s32 wsBuf[0xB8 / 4];
    u8* ws = (u8*)wsBuf;
    u8* mesh = (u8*)pMesh;
    u8* prim;
    u32 rotMode;
    s32 groupCount;

    func_8007CD3C(0xB8);
    *(s32*)(ws + 0xA0) = 0x7FFFFFFF;
    *(u32*)(ws + 0xA4) = *(u32*)(mesh + 0x8);
    *(s32*)(ws + 0x10) = (curX << 16) + curZ;

    /* Retail 800833E4..8008345C reloads g_FieldActors and recomputes the 0x5C
     * stride for every actor/actorData access, so index the array per site. */
    rotMode = *(u32*)((u8*)(uintptr_t)g_FieldActors[ownIndex].pActorData + 0x12C) & 0x3;

    if (rotMode != 0) {
        SVECTOR* rot = (SVECTOR*)(ws + 0xB0);

        if (rotMode == 1) {
            rot->vx = *(u16*)(((u8*)(uintptr_t)g_FieldActors[ownIndex].pActorData) + 0x70);
            rot->vy = 0;
            rot->vz = 0;
        } else if (rotMode == 2) {
            rot->vx = 0;
            rot->vy = *(u16*)(((u8*)(uintptr_t)g_FieldActors[ownIndex].pActorData) + 0x70);
            rot->vz = 0;
        } else {
            rot->vx = 0;
            rot->vy = 0;
            rot->vz = *(u16*)(((u8*)(uintptr_t)g_FieldActors[ownIndex].pActorData) + 0x70);
        }
        RotMatrix(rot, (MATRIX*)(ws + 0x60));
        MulMatrix2((MATRIX*)(((u8*)&g_FieldActors[ownIndex]) + 0xC), (MATRIX*)(ws + 0x60));
        *(s32*)(ws + 0x74) = *(s32*)(((u8*)&g_FieldActors[ownIndex]) + 0x20);
        *(s32*)(ws + 0x78) = *(s32*)(((u8*)&g_FieldActors[ownIndex]) + 0x24);
        *(s32*)(ws + 0x7C) = *(s32*)(((u8*)&g_FieldActors[ownIndex]) + 0x28);
        CompMatrix(&g_Scene.worldRotationMatrix, (MATRIX*)(ws + 0x60),
                   (MATRIX*)(ws + 0x40));
    } else {
        u8 parentIdx;

        /* Retail zeroes both work matrices' translation columns first. */
        *(s32*)(ws + 0x54) = 0;
        *(s32*)(ws + 0x58) = 0;
        *(s32*)(ws + 0x5C) = 0;
        *(s32*)(ws + 0x94) = 0;
        *(s32*)(ws + 0x98) = 0;
        *(s32*)(ws + 0x9C) = 0;

        parentIdx = *(u8*)(((u8*)(uintptr_t)g_FieldActors[ownIndex].pActorData) + 0x75);
        CompMatrix(&g_Scene.worldRotationMatrix, &D_800AFC30,
                   (MATRIX*)(ws + 0x80));
        if (parentIdx != 0xFF) {
            CompMatrix((MATRIX*)(ws + 0x80),
                       (MATRIX*)((u8*)g_FieldActors + parentIdx * 0x5C + 0x2C),
                       (MATRIX*)(ws + 0x60));
            CompMatrix((MATRIX*)(ws + 0x60), (MATRIX*)(((u8*)&g_FieldActors[ownIndex]) + 0xC),
                       (MATRIX*)(ws + 0x40));
        } else {
            CompMatrix((MATRIX*)(ws + 0x80), (MATRIX*)(((u8*)&g_FieldActors[ownIndex]) + 0xC),
                       (MATRIX*)(ws + 0x40));
        }
    }
    SetRotMatrix((MATRIX*)(ws + 0x40));
    SetTransMatrix((MATRIX*)(ws + 0x40));

    groupCount = *(u16*)(mesh + 0x6);
    prim = (u8*)(uintptr_t)*(u32*)(mesh + 0x10);

    for (; groupCount > 0; groupCount--) {
        u32 header = *(u32*)prim;
        u32 type = header & 0xFF;
        s32 primCount = header >> 16;
        s32 n;

        prim += 4;
        *(u32*)(ws + 0xAC) = type;
        if (type == 0xC4 || type == 0xC8) {
            continue;
        }

        if ((header & 0x8) == 0) {
            /* Triangles: 3 vertex indices + 1 pad halfword per primitive. */
            for (n = 0; n < primCount; n++) {
                u8* verts = (u8*)(uintptr_t)*(u32*)(ws + 0xA4);

                RotTransSV((SVECTOR*)(verts + *(u16*)(prim + 0x0) * 8),
                           (SVECTOR*)(ws + 0x14), (long*)(ws + 0x3C));
                RotTransSV((SVECTOR*)(verts + *(u16*)(prim + 0x2) * 8),
                           (SVECTOR*)(ws + 0x1C), (long*)(ws + 0x3C));
                prim += 4;
                RotTransSV((SVECTOR*)(verts + *(u16*)(prim + 0x0) * 8),
                           (SVECTOR*)(ws + 0x24), (long*)(ws + 0x3C));
                prim += 4;

                /* Packed (screenX<<16)|screenZ per vertex, ground plane. */
                *(s32*)(ws + 0x0) =
                    (*(s16*)(ws + 0x14) << 16) + *(s16*)(ws + 0x18);
                *(s32*)(ws + 0x4) =
                    (*(s16*)(ws + 0x1C) << 16) + *(s16*)(ws + 0x20);
                *(s32*)(ws + 0x8) =
                    (*(s16*)(ws + 0x24) << 16) + *(s16*)(ws + 0x28);

                if (NormalClip(*(s32*)(ws + 0x0), *(s32*)(ws + 0x4),
                               *(s32*)(ws + 0x10)) < 0 ||
                    NormalClip(*(s32*)(ws + 0x4), *(s32*)(ws + 0x8),
                               *(s32*)(ws + 0x10)) < 0 ||
                    NormalClip(*(s32*)(ws + 0x8), *(s32*)(ws + 0x0),
                               *(s32*)(ws + 0x10)) < 0 ||
                    NormalClip(*(s32*)(ws + 0x0), *(s32*)(ws + 0x4),
                               *(s32*)(ws + 0x8)) < 0) {
                    continue;
                }

                *(u16*)(ws + 0x34) = (u16)curX;
                *(u16*)(ws + 0x38) = (u16)curZ;
                func_8007B07C((s16*)(ws + 0x14), (s16*)(ws + 0x1C),
                              (s16*)(ws + 0x24), (s16*)(ws + 0x34),
                              (VECTOR*)pOut2);
                if (*(s16*)(ws + 0x36) < *(s32*)(ws + 0xA0)) {
                    *(s32*)(ws + 0xA0) = *(s16*)(ws + 0x36);
                }
            }
        } else {
            /* Quads: 4 vertex indices per primitive, wound (0,1,3,2); the
             * final diagonal test picks which half-triangle interpolates. */
            for (n = 0; n < primCount; n++) {
                u8* verts = (u8*)(uintptr_t)*(u32*)(ws + 0xA4);

                RotTransSV((SVECTOR*)(verts + *(u16*)(prim + 0x0) * 8),
                           (SVECTOR*)(ws + 0x14), (long*)(ws + 0x3C));
                RotTransSV((SVECTOR*)(verts + *(u16*)(prim + 0x2) * 8),
                           (SVECTOR*)(ws + 0x1C), (long*)(ws + 0x3C));
                prim += 4;
                RotTransSV((SVECTOR*)(verts + *(u16*)(prim + 0x0) * 8),
                           (SVECTOR*)(ws + 0x24), (long*)(ws + 0x3C));
                RotTransSV((SVECTOR*)(verts + *(u16*)(prim + 0x2) * 8),
                           (SVECTOR*)(ws + 0x2C), (long*)(ws + 0x3C));
                prim += 4;

                *(s32*)(ws + 0x0) =
                    (*(s16*)(ws + 0x14) << 16) + *(s16*)(ws + 0x18);
                *(s32*)(ws + 0x4) =
                    (*(s16*)(ws + 0x1C) << 16) + *(s16*)(ws + 0x20);
                *(s32*)(ws + 0x8) =
                    (*(s16*)(ws + 0x24) << 16) + *(s16*)(ws + 0x28);
                *(s32*)(ws + 0xC) =
                    (*(s16*)(ws + 0x2C) << 16) + *(s16*)(ws + 0x30);

                if (NormalClip(*(s32*)(ws + 0x0), *(s32*)(ws + 0x4),
                               *(s32*)(ws + 0x10)) < 0 ||
                    NormalClip(*(s32*)(ws + 0x4), *(s32*)(ws + 0xC),
                               *(s32*)(ws + 0x10)) < 0 ||
                    NormalClip(*(s32*)(ws + 0xC), *(s32*)(ws + 0x8),
                               *(s32*)(ws + 0x10)) < 0 ||
                    NormalClip(*(s32*)(ws + 0x8), *(s32*)(ws + 0x0),
                               *(s32*)(ws + 0x10)) < 0 ||
                    NormalClip(*(s32*)(ws + 0x0), *(s32*)(ws + 0x4),
                               *(s32*)(ws + 0x8)) < 0) {
                    continue;
                }

                *(u16*)(ws + 0x34) = (u16)curX;
                *(u16*)(ws + 0x38) = (u16)curZ;
                if (NormalClip(*(s32*)(ws + 0x4), *(s32*)(ws + 0x8),
                               *(s32*)(ws + 0x10)) >= 0) {
                    func_8007B07C((s16*)(ws + 0x14), (s16*)(ws + 0x1C),
                                  (s16*)(ws + 0x24), (s16*)(ws + 0x34),
                                  (VECTOR*)pOut2);
                } else {
                    func_8007B07C((s16*)(ws + 0x1C), (s16*)(ws + 0x2C),
                                  (s16*)(ws + 0x24), (s16*)(ws + 0x34),
                                  (VECTOR*)pOut2);
                }
                if (*(s16*)(ws + 0x36) < *(s32*)(ws + 0xA0)) {
                    *(s32*)(ws + 0xA0) = *(s16*)(ws + 0x36);
                }
            }
        }
    }

    if (*(s32*)(ws + 0xA0) != 0x7FFFFFFF) {
        *pOutY = *(s32*)(ws + 0xA0);
        func_8007CD60(0xB8);
        return 0;
    }
    func_8007CD60(0xB8);
    return -1;
}
#endif /* XENO_PC_PORT */

void func_80083994(void) {
}

extern u16 D_800C2694;
extern s16 D_800B2174;
extern s32 D_80285988;
extern s32 D_800ADF64;
extern u_short FieldScriptGetBytecodeOffset(int scriptIndex, int routineIndex);

/* Retail 8008399C..80084158. Rectangular, radial, and whole-map
 * interactions have distinct gates; keep their enqueue join explicit. */
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc8", func_8008399C);
#else
void func_8008399C(s32 actorIndex, void* pFieldActor, void* pActorData) {
    u8* actorData = pActorData;
    s32 playerY = *(s16*)(actorData + 0x26);
    s32 playerFloor = playerY - *(u16*)(actorData + 0x1A);
    s32 innerRadius = *(u16*)(actorData + 0x1E) + 8;
    s32 outerRadius = *(u16*)(actorData + 0x1E) + 0x20;
    s32 playerRot = *(u16*)(actorData + 0x106) & 0xFFF;
    s32 playerX = *(s16*)(actorData + 0x22);
    s32 playerZ = *(s16*)(actorData + 0x2A);
    s32 found = 0;
    s32 scriptRoutine = 7;
    s32 i;

    for (i = 0; i < D_800ADBFC; i++) {
        u8* otherData = (u8*)(uintptr_t)*(u32*)((u8*)g_FieldActors + i * 0x5C + 0x4C);
        u32 flags0 = *(u32*)otherData;
        u32 flags4;
        s32 otherY, dx, dz, angle, dir, delta;
        s32 scriptId = 0xFF;
        s32 rectangular;
        s32 distance = 0;
        s32 limits[3], squaredLimits[3];
        s32 vec[3], squared[3];

        if ((flags0 & 1) || *(u8*)(actorData + 0x74) == i) {
            continue;
        }
        otherY = *(s16*)(otherData + 0x26) + *(s16*)(otherData + 0x62);
        flags4 = *(u32*)(otherData + 4);
        if (flags4 & 0x180) {
            if (flags4 & 0x100) {
                if ((D_800C2694 & 0x20) && !found && !(flags4 & 0x4000000)) {
                    if (!(flags0 & 0x220000) && !D_800B2174) {
                        found = 1;
                        scriptId = 2;
                        scriptRoutine = 3;
                        dx = *(s16*)(otherData + 0x22) - playerX + *(s16*)(otherData + 0x60);
                        dz = *(s16*)(otherData + 0x2A) - playerZ + *(s16*)(otherData + 0x64);
                        angle = ratan2(dz, dx);
                        *(u32*)(otherData + 0x12C) =
                            (*(u32*)(otherData + 0x12C) & ~0xE00u) | ((0u - angle) & 0xE00);
                    }
                } else if (!(flags0 & 0xA20000)) {
                    scriptId = 3;
                    scriptRoutine = 4;
                    dx = *(s16*)(otherData + 0x22) - playerX + *(s16*)(otherData + 0x60);
                    dz = *(s16*)(otherData + 0x2A) - playerZ + *(s16*)(otherData + 0x64);
                    angle = ratan2(dz, dx);
                    dir = (-(angle >> 9)) & 7;
                    *(u32*)(otherData + 0x12C) =
                        (*(u32*)(otherData + 0x12C) & ~0xE00u) | (dir << 9);
                    if (!D_800ADF64 && (*(u32*)otherData & 0x8000000)) {
                        u8* sprite = (u8*)(uintptr_t)*(u32*)((u8*)pFieldActor + 4);
                        D_800ADF64 = 1;
                        *(u32*)(sprite + 0x10) = 0;
                    }
                }
            } else {
                D_800ADF64 = 0;
            }
        }

        rectangular = (*(u32*)otherData & 0x2000) != 0;
        if (rectangular) {
            if (otherY < playerFloor || playerY < otherY - *(u16*)(otherData + 0x1A) ||
                i == actorIndex || func_8008237C(playerX, playerZ, otherData, 0x10) != 0) {
                goto enqueue;
            }
        } else {
            vec[0] = *(s16*)(otherData + 0x22) - playerX + *(s16*)(otherData + 0x60);
            vec[2] = *(s16*)(otherData + 0x2A) - playerZ + *(s16*)(otherData + 0x64);
            vec[1] = outerRadius + *(u16*)(otherData + 0x1E);
            Square0((VECTOR*)vec, (VECTOR*)squared);
            if ((s32)((u32)squared[0] + (u32)squared[2]) >= squared[1] || otherY < playerFloor ||
                playerY < otherY - *(u16*)(otherData + 0x1A) || i == actorIndex) {
                goto enqueue;
            }
            /* Retail builds the vector twice but only squares it twice total
             * (jals at 80083EE8 and 80083F20, with sp+0x90 == &vec and
             * sp+0x88 == &squared set at 80083A2C/80083A38). The previous
             * duplicate Square0 here re-squared identical inputs into the same
             * buffer, so it was a redundant third call. */
            distance = (s32)((u32)squared[0] + (u32)squared[2]);
            limits[0] = innerRadius + *(u16*)(otherData + 0x1E);
            limits[1] = 0; /* Retail's unused middle result is not consumed. */
            limits[2] = outerRadius + *(u16*)(otherData + 0x1E);
            Square0((VECTOR*)limits, (VECTOR*)squaredLimits);
        }

        if (rectangular) {
            /* Retail 80083C40-80083D68 (rectangular arm). Retail keeps a
             * SEPARATE copy of this block per arm rather than merging them, and
             * the rectangular copy additionally requires `otherData+4 & 0x40000`
             * before the delta test. Splitting the previously-merged block is
             * behaviour-preserving; the conditions below are the exact
             * rectangular projections of the old merged ones. */
            if ((D_800C2694 & 0x20) && !found &&
                !(*(u32*)(otherData + 4) & 0x4000000)) {
                if (*(u32*)otherData & 0x220000) {
                    goto enqueue;
                }
                if (D_800B2174) {
                    goto enqueue;
                }
                dx = *(s16*)(otherData + 0x22) - playerX + *(s16*)(otherData + 0x60);
                dz = *(s16*)(otherData + 0x2A) - playerZ + *(s16*)(otherData + 0x64);
                angle = ratan2(dz, dx);
                dir = ((s32)(0u - angle) >> 9) & 7;
                delta = (playerRot - ((0u - angle) & 0xFFF)) & 0xFFF;
                if ((*(u32*)(otherData + 4) & 0x40000) &&
                    (u32)(delta - 0x2BC) < 0xA89u) {
                    goto enqueue;
                }
                found = 1;
                scriptId = 2;
                scriptRoutine = 3;
            } else {
                if (*(u32*)otherData & 0xA20000) {
                    goto enqueue;
                }
                scriptId = 3;
                scriptRoutine = 4;
                dx = *(s16*)(otherData + 0x22) - playerX + *(s16*)(otherData + 0x60);
                dz = *(s16*)(otherData + 0x2A) - playerZ + *(s16*)(otherData + 0x64);
                angle = ratan2(dz, dx);
                dir = (-(angle >> 9)) & 7;
            }
        } else {
            /* Retail 80083F40-80083FE0 (non-rectangular arm): the same block
             * without the `0x40000` requirement - the delta test is bare. */
            if (distance < squaredLimits[2] && (D_800C2694 & 0x20) && !found &&
                !(*(u32*)(otherData + 4) & 0x4000000)) {
                if (*(u32*)otherData & 0x220000) {
                    goto enqueue;
                }
                dx = *(s16*)(otherData + 0x22) - playerX + *(s16*)(otherData + 0x60);
                dz = *(s16*)(otherData + 0x2A) - playerZ + *(s16*)(otherData + 0x64);
                angle = ratan2(dz, dx);
                dir = ((s32)(0u - angle) >> 9) & 7;
                delta = (playerRot - ((0u - angle) & 0xFFF)) & 0xFFF;
                if ((u32)(delta - 0x2BC) < 0xA89u) {
                    goto enqueue;
                }
                if (D_800B2174) {
                    goto enqueue;
                }
                found = 1;
                scriptId = 2;
                scriptRoutine = 3;
            } else {
                if ((*(u32*)otherData & 0xA20000) || distance >= squaredLimits[0]) {
                    goto enqueue;
                }
                scriptId = 3;
                scriptRoutine = 4;
                dx = *(s16*)(otherData + 0x22) - playerX + *(s16*)(otherData + 0x60);
                dz = *(s16*)(otherData + 0x2A) - playerZ + *(s16*)(otherData + 0x64);
                angle = ratan2(dz, dx);
                dir = (-(angle >> 9)) & 7;
            }
        }
        *(u32*)(otherData + 0x12C) =
            (*(u32*)(otherData + 0x12C) & ~0xE00u) | (dir << 9);
        /* Only the rectangular arm raises D_80285988 (confirm at
         * 80083D5C-80083D60, passive at 80083DE8-80083DEC), each gated on
         * g_FieldSystemMode == 0. The non-rectangular arm has no such store:
         * its confirm/passive converge on .L80084074, which writes only the
         * 0x12C word. 80083DDC-80083DEC is the RECTANGULAR passive, not a
         * non-rectangular flag set. */
        if (rectangular && g_FieldSystemMode == 0) {
            D_80285988 = 1;
        }

enqueue:
        if (scriptId != 0xFF) {
            s32 slot;
            u8* slotBase;
            for (slot = 0, slotBase = otherData; slot < 8; slot++, slotBase += 8) {
                if (*(u8*)(slotBase + 0x8F) == (u8)scriptId) {
                    break;
                }
            }
            if (slot == 8) {
                for (slot = 0, slotBase = otherData; slot < 8; slot++, slotBase += 8) {
                    u32 word = *(u32*)(slotBase + 0x90);
                    if (((word >> 18) & 0xF) == 0xF && ((word >> 22) & 1) == 0) {
                        u16 ip = FieldScriptGetBytecodeOffset(i, scriptId);
#ifdef XENO_PC_PORT
                        { extern void PcPort_RouteLogTrigger(int, int, int);
                          PcPort_RouteLogTrigger(i, scriptId, scriptRoutine); } /* inert unless XENO_ROUTE_LOG */
#endif
                        *(u16*)(slotBase + 0x8C) = ip;
                        *(u8*)(slotBase + 0x8F) = scriptId;
                        *(u32*)(slotBase + 0x90) =
                            (word & 0xFFC3FFFFu) | (scriptRoutine << 18);
                        *(u16*)(otherData + 0x106) |= 0x8000;
                        *(u16*)(otherData + 0x104) = *(u16*)(otherData + 0x106);
                        break;
                    }
                }
            }
        }
    }
}
#endif /* XENO_PC_PORT */

extern void* func_8007CD3C(s32 arg0);
extern void func_8007CD60(s32 arg0);
extern void func_800379C8(char*, ...);
extern char D_8006FC48[];
extern char D_8006FC58[];
extern s32 D_800ADB98;

#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc8", func_80084158);
#else
void func_80084158(s32 actorIndex, void* pFieldActor, void* pActorData) {
    u8* actor = (u8*)pFieldActor;
    u8* actorData = (u8*)pActorData;
    s32 predicted[3];
    SVECTOR currentPos;
    s32 targetYMin;
    u32 actorFlags0;
    u32 actorFlags4;
    u32 actorFlags14;
    u8 oldInteractActor;
    s32 selectedY;
    s32 hasTarget;
    s32 targetState;
    s32 i;
    s32 scratch[8];

    func_8007CD3C(0x20);

    predicted[0] = *(s32*)(actorData + 0x20) + *(s32*)(actorData + 0x30);
    predicted[1] = *(s32*)(actorData + 0x24) + *(s32*)(actorData + 0x34);
    predicted[2] = *(s32*)(actorData + 0x28) + *(s32*)(actorData + 0x38);
    func_800831D0(&currentPos, predicted);

    targetYMin = *(s16*)(actorData + 0x26) - *(u16*)(actorData + 0x1A);
    actorFlags0 = *(u32*)(actorData + 0x00);
    actorFlags4 = *(u32*)(actorData + 0x04);
    actorFlags14 = *(u32*)(actorData + 0x14);
    oldInteractActor = *(u8*)(actorData + 0x74);
    selectedY = 0x7FFFFFFF;
    hasTarget = 0;
    targetState = 0;

    for (i = 0; i < D_800ADBFC; i++) {
        u8* otherActor;
        u8* otherData;
        u32 otherFlags0;
        u32 otherFlags4;
        s32 floorY;

        if (i == actorIndex) {
            continue;
        }

        otherActor = (u8*)g_FieldActors + i * 0x5C;
        otherData = (u8*)(uintptr_t)*(u32*)(otherActor + 0x4C);
        otherFlags0 = *(u32*)(otherData + 0x00);
        if (otherFlags0 & 0x1) {
            continue;
        }

        otherFlags4 = *(u32*)(otherData + 0x04);
        *(u32*)(otherData + 0x04) = otherFlags4 & 0xFFFF3EFF;

        if (otherFlags4 & 0x80) {
            /* asm 800842B8-80084380: interaction-region actor. Test whether
             * our predicted (x, z) lies inside the other actor's collision
             * mesh (model header +0x4, via the FieldActor +0x0 pointer);
             * regionY[0] receives the surface Y under us, contact[] the
             * func_8007B07C contact data. Miss -> clear 0x400000|0x800000
             * and skip the actor, exactly like the far case below. */
            s32 regionY[4];
            s32 contact[4];
            u8* otherModel = (u8*)(uintptr_t)*(u32*)(otherActor + 0x0);
            s32 regionTop;

            if (func_80083288(i,
                              (void*)(uintptr_t)*(u32*)(otherModel + 0x4),
                              currentPos.vx, currentPos.vz,
                              regionY, contact) != 0) {
                *(u32*)(otherData + 0x04) &= 0xFF3FFFFF;
                continue;
            }

            /* Hit ("POLYCHECK"): asm 80084304-80084380 + tail joins. */
            if (g_FieldSystemMode == 0) {
                func_800379C8(D_8006FC48, i);
            }
            *(u32*)(otherData + 0x04) |= 0x100;
            regionTop = regionY[0] + *(u16*)(otherData + 0x1A);

            if (*(u8*)(actorData + 0x74) == i) {
                /* Already interacting with this actor: latch the contact
                 * data and mark it (asm 8008434C-80084380)... */
                *(u32*)(actorData + 0x50) = (u32)contact[0];
                *(u32*)(actorData + 0x54) = (u32)contact[1];
                *(u32*)(actorData + 0x58) = (u32)contact[2];
                *(u32*)(otherData + 0x04) |= 0x4000;
                /* ...then asm .L800844B8 routes straight to the
                 * select-target machinery unless flags0 has 0x40800 set. */
                if ((actorFlags0 & 0x40800) == 0) {
                    goto select_target;
                }
            }

            /* asm .L800844D8 dispatch: regionTop plays the raw-top role,
             * regionY[0] the floor-height role. */
            if (regionTop < targetYMin ||
                *(s16*)(actorData + 0x26) < regionY[0]) {
                /* .L80084660/.L8008465C track-height + .L800846A8 tail. */
                u32 v = *(u32*)(otherData + 0x04) & ~0x100u;

                if (*(s16*)(actorData + 0x26) < regionY[0]) {
                    v |= 0x00800000;
                    if (regionY[0] < selectedY) {
                        selectedY = regionY[0];
                    }
                } else {
                    v &= ~0x00800000u;
                }
                *(u32*)(otherData + 0x04) = v | 0x00400000;
                continue;
            }
            if (*(s16*)(actorData + 0x26) < regionY[0] + 0x10) {
                goto select_target;
            }
            if (*(u32*)(otherData + 0x04) & 0x00800000) {
                goto select_target;
            }

            /* .L80084570 standing-on-top machinery: platforms without the
             * 0x10 no-carry flag ramp a contact counter (+2/frame, cap
             * 0x30); once it passes 0x20 the player's X/Z velocity bleeds
             * into the platform (v/4, round toward zero -- retail's
             * bgez/addiu/sra) and the player's velocity + carried-velocity
             * latches are zeroed.  Below threshold (or 0x10 flagged) both
             * sides' velocities and latches reset (asm .L80084628). */
            if (!(otherFlags4 & 0x10)) {
                u8 ramp = *(u8*)(otherData + 0xE3);

                if (ramp < 0x30) {
                    *(u8*)(otherData + 0xE3) = (u8)(ramp + 2);
                }
                if (*(u8*)(otherData + 0xE3) >= 0x21) {
                    s32 v = *(s32*)(actorData + 0x30);

                    if (v < 0) {
                        v += 3;
                    }
                    *(s32*)(otherData + 0x40) += v >> 2;
                    v = *(s32*)(actorData + 0x38);
                    if (v < 0) {
                        v += 3;
                    }
                    *(s32*)(otherData + 0x48) += v >> 2;
                    /* Retail then re-derives v/4 from the just-zeroed
                     * velocities -- provably zero, so plain zero stores. */
                    *(s32*)(actorData + 0x30) = 0;
                    *(s32*)(actorData + 0x34) = 0;
                    *(s32*)(actorData + 0x38) = 0;
                    *(s32*)(actorData + 0x40) = 0;
                    *(s32*)(actorData + 0x44) = 0;
                    *(s32*)(actorData + 0x48) = 0;
                    continue;
                }
            }
            *(s32*)(otherData + 0x40) = 0;
            *(s32*)(otherData + 0x44) = 0;
            *(s32*)(otherData + 0x48) = 0;
            *(s32*)(otherData + 0x30) = 0;
            *(s32*)(otherData + 0x34) = 0;
            *(s32*)(otherData + 0x38) = 0;
            *(s32*)(actorData + 0x30) = 0;
            *(s32*)(actorData + 0x34) = 0;
            *(s32*)(actorData + 0x38) = 0;
            *(s32*)(actorData + 0x40) = 0;
            *(s32*)(actorData + 0x44) = 0;
            *(s32*)(actorData + 0x48) = 0;
            continue;

        select_target:
            /* asm .L80084520: ride/select this region actor.  selectedY
             * takes the surface Y unconditionally, the region is marked
             * 0x800000, the platform's velocity (+0x30..0x38) is latched
             * into the player's carried-velocity block (+0x40..0x48), and
             * targetState arms (2).  Unless flags0 has 0x40800, the
             * interact-actor commits and hasTarget latches. */
            selectedY = regionY[0];
            *(u32*)(otherData + 0x04) |= 0x00800000;
            *(u32*)(actorData + 0x40) = *(u32*)(otherData + 0x30);
            *(u32*)(actorData + 0x44) = *(u32*)(otherData + 0x34);
            *(u32*)(actorData + 0x48) = *(u32*)(otherData + 0x38);
            targetState = 2;
            if ((actorFlags0 & 0x40800) == 0) {
                *(u8*)(actorData + 0x74) = (u8)i;
                hasTarget = 1;
            }
            continue;
        }

        if (otherFlags0 & 0x2000) {
            if (func_8008237C(currentPos.vx, currentPos.vz, otherData, 0) != 0) {
                *(u32*)(otherData + 0x04) &= 0xFF3FFFFF;
                continue;
            }
            scratch[4] = 0;
            scratch[5] = 1;
            scratch[6] = 0;
        } else {
            scratch[0] = ((*(s32*)(otherData + 0x20) + *(s32*)(otherData + 0x30)) >> 16) - currentPos.vx;
            scratch[1] = *(u16*)(actorData + 0x1E) + *(u16*)(otherData + 0x1E);
            scratch[2] = ((*(s32*)(otherData + 0x28) + *(s32*)(otherData + 0x38)) >> 16) - currentPos.vz;
            Square0((VECTOR*)scratch, (VECTOR*)&scratch[4]);
        }

        if (scratch[4] + scratch[6] < scratch[5]) {
            if (actorFlags14 & 0x00400000) {
                if (g_FieldSystemMode == 0) {
                    func_800379C8(D_8006FC58);
                }
                continue;
            }

            if ((otherFlags0 | actorFlags0) & 0x80) {
                continue;
            }

            if (D_800B21CC != 0) {
                continue;
            }

            floorY = *(s16*)(otherData + 0x26) - *(u16*)(otherData + 0x1A);
        } else {
            floorY = 0x7FFFFFFF;
        }

        if (floorY < targetYMin) {
            *(u32*)(otherData + 0x04) &= ~0x100;
            if (*(s16*)(actorData + 0x26) < floorY) {
                *(u32*)(otherData + 0x04) |= 0x00800000;
                if (floorY < selectedY) {
                    selectedY = floorY;
                }
            } else {
                *(u32*)(otherData + 0x04) &= ~0x00800000;
            }
        } else {
            *(u32*)(otherData + 0x04) |= 0x00400000;
        }
    }

    if (D_800ADB98 != 0) {
        extern s32 D_800ADB94;
        selectedY = D_800ADB94;
        hasTarget = 0;
        targetState++;
    }

    if (hasTarget == 0) {
        *(u8*)(actorData + 0x74) = 0xFF;
    } else {
        /* asm .L80084718: commit the selected target -- mark it 0x8000 and,
         * on a FRESH commit only (previous interact-actor was 0xFF),
         * snapshot the target FieldActor's +0x50/52/54 trio plus the
         * func_800825AC pairing value into a one-shot 12-byte record at
         * actorData+0x110 (allocated once, latched by flags 0x134&0x80). */
        u8 t = *(u8*)(actorData + 0x74);
        u8* tData = (u8*)(uintptr_t)
            *(u32*)((u8*)g_FieldActors + t * 0x5C + 0x4C);

        *(u32*)(tData + 0x04) |= 0x8000;
        if (oldInteractActor == 0xFF) {
            u8* tActor;
            u16* rec;

            if (!(*(u32*)(actorData + 0x134) & 0x80)) {
                *(u32*)(actorData + 0x110) =
                    (u32)(uintptr_t)HeapAlloc(0xC, 0);
                *(u32*)(actorData + 0x134) |= 0x80;
            }
            tActor = (u8*)g_FieldActors + *(u8*)(actorData + 0x74) * 0x5C;
            rec = (u16*)(uintptr_t)*(u32*)(actorData + 0x110);
            rec[0] = *(u16*)(tActor + 0x50);
            rec[1] = *(u16*)(tActor + 0x52);
            rec[2] = *(u16*)(tActor + 0x54);
            rec[4] = (u16)func_800825AC(actorIndex,
                                        *(u8*)(actorData + 0x74));
        }
    }

#ifdef XENO_PC_PORT
    /* DIAGNOSTIC (XENO_MOVE_DIAG=1): the INTERACTION-target result for the
     * player -- which actor (if any) the player is standing on//next to.
     *
     * Do NOT read hasTarget==0 as "no floor under the player". Measured on
     * map 1, which walks perfectly: hasTarget is 0 and selectedY stays at its
     * 0x7FFFFFFF sentinel on every sample there too. This search is about
     * ride/talk targets, not ordinary ground, so a 0 here is the normal case
     * and says nothing about walkability. (Recorded because the opposite
     * reading looked briefly like a root cause.) Removal: delete this block. */
    {
        static int s_on = -1;
        static unsigned long s_calls;

        if (s_on < 0) {
            const char* e = getenv("XENO_MOVE_DIAG");
            s_on = (e != NULL && e[0] != '\0' && e[0] != '0');
        }
        if (s_on && actorIndex == g_PlayerActorIndex && (s_calls++ % 30) == 0) {
            printf("[xeno-port][move] ground actor=%d pos=(%d,%d) "
                   "hasTarget=%d selectedY=%d targetState=%d scanned=%d\n",
                   (int)actorIndex, (int)currentPos.vx, (int)currentPos.vz,
                   (int)hasTarget, (int)selectedY, (int)targetState,
                   (int)D_800ADBFC);
            fflush(stdout);
        }
    }
#endif

    if ((*(u32*)(actorData + 0x00) & 0x00010000) == 0 &&
        (*(u32*)(actorData + 0x04) & 0x00200000) == 0) {
        func_80084A40(actorIndex, selectedY, actor, actorData, targetState);
    }

    {
        u8* spriteData = (u8*)(uintptr_t)*(u32*)(actor + 0x04);
        u8* spriteInner = (u8*)(uintptr_t)*(u32*)(spriteData + 0x7C);

        if (*(u16*)(spriteInner + 0x0C) == 1) {
            ControllerResetState();
            *(u32*)(actorData + 0x00) &= ~0x800;
        }
    }

    (void)oldInteractActor;
    (void)targetState;
    func_8007CD60(0x20);
}
#endif /* XENO_PC_PORT */

extern s32 D_800ADB98;
extern s32 D_800ADC0C;

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): field_actor_has_pending_motion (src/decomp/src/field/field_motion.c:3679). */
#ifndef XM_S_field_actor_has_pending_motion
#define XM_S_field_actor_has_pending_motion
s32 xm_field_actor_has_pending_motion(xm_FieldActor *actor) __asm__("func_8008492C");
#endif
#ifndef XM_S_field_ground_override_enabled
#define XM_S_field_ground_override_enabled
extern s32 xm_field_ground_override_enabled __asm__("D_800ADB98");
#endif
#ifndef XM_S_field_update_ran
#define XM_S_field_update_ran
extern s32 xm_field_update_ran __asm__("D_800ADC0C");
#endif
s32 func_8008492C(xm_FieldActor *actor) {
    if (!(actor->floor_attribute & 0x420000) && xm_field_ground_override_enabled == 0 && actor->unk030[0] == 0 && actor->unk030[1] == 0 &&
        actor->unk030[2] == 0 && xm_field_update_ran == 1 && actor->ridden_actor == 0xFF && !(actor->flags & 0x401800)) {
        if ((actor->layer_flags & 1) && actor->layer == 0) {
            return -1;
        }
        if ((actor->layer_flags & 2) && actor->layer == 1) {
            return -1;
        }
        if ((actor->layer_flags & 4) && actor->layer == 2) {
            return -1;
        }
        return 0;
    }
    return -1;
}
#endif
#else
s32 func_8008492C(u8* actorData) {
    if (*(u32*)(actorData + 0x14) & 0x00420000) {
        return -1;
    }

    if (D_800ADB98 != 0) {
        return -1;
    }

    if (*(u32*)(actorData + 0x30) != 0) {
        return -1;
    }

    if (*(u32*)(actorData + 0x34) != 0) {
        return -1;
    }

    if (*(u32*)(actorData + 0x38) != 0) {
        return -1;
    }

    if (D_800ADC0C != 1) {
        return -1;
    }

    if (*(u8*)(actorData + 0x74) != 0xFF) {
        return -1;
    }

    if (*(u32*)(actorData + 0x00) & 0x00401800) {
        return -1;
    }

    if ((*(u32*)(actorData + 0x04) & 0x1) && *(s16*)(actorData + 0x10) == 0) {
        return -1;
    }

    if ((*(u32*)(actorData + 0x04) & 0x2) && *(s16*)(actorData + 0x10) == 1) {
        return -1;
    }

    if (*(u32*)(actorData + 0x04) & 0x4) {
        return -((*(s16*)(actorData + 0x10) == 2) != 0);
    }

    return 0;
}
#endif /* XENO_PC_PORT */

extern s16 D_800ADB00;
extern u8 D_800B21CF;
extern s32 g_FieldSystemMode;
extern char D_8006FC60[];
extern char D_8006FC74[];
/* (func_800379C8 is the field's variadic printf; declared once at the top
 * of this file's interaction section.) */
extern s32 func_8007D3D4(u8* actorData, s32 idx, s32* outHeight0,
                         VECTOR* outNormal, s16* outTriangle, s32* outHeight1);

static inline void func_80084A40_RestoreActorState(u8* actorData, u8* spriteData,
                                            s32 actorIndex, s32 origX,
                                            s32 origZ, s16 origState,
                                            s16 savedStates[4]) {
    s32 i;
    u8* actor = (u8*)g_FieldActors + actorIndex * 0x5C;

    *(s32*)(actorData + 0x20) = origX;
    *(s32*)(actorData + 0x28) = origZ;
    *(s16*)(actorData + 0x10) = origState;
    *(s32*)(actorData + 0xF0) = 0;

    for (i = 0; i < 4; i++) {
        *(s16*)(actorData + 0x08 + i * 2) = savedStates[i];
    }

    if (*(s16*)(spriteData + 0x84) != *(s16*)(actorData + 0x26)) {
        *(s32*)(spriteData + 0x10) += *(s32*)(spriteData + 0x1C);
    }

    if (*(s32*)(spriteData + 0x10) < 0) {
        *(s32*)(spriteData + 0x10) = 0;
        *(s32*)(actorData + 0x24) = *(s32*)(spriteData + 0x04);
    }

    *(s32*)(spriteData + 0x00) = *(s32*)(actorData + 0x20);
    *(s32*)(spriteData + 0x04) = *(s32*)(actorData + 0x24);
    *(s32*)(spriteData + 0x08) = *(s32*)(actorData + 0x28);
    *(s32*)(actor + 0x24) = *(s16*)(actorData + 0x26);
}

#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc8", func_80084A40);
#else
/* Instruction-faithful translation of retail 80084A40..800854CC.
 * Stack arrays mirror the retail frame (low sp+0x18, high sp+0x28, stateIds
 * sp+0x38, triIds sp+0x48, normals sp+0x50); like retail, triIds/normals are
 * only defined for layers func_8007D3D4 wrote.  All adds/shifts are u32 so
 * they wrap like addu/sll without host signed-overflow UB. */
/* Guest fields are reached through casts of differently-typed views of the
 * same bytes (e.g. the s32 Y at +0x24 and its s16 half at +0x26); may_alias
 * views keep host type-based alias analysis from reordering those accesses. */
typedef s16 __attribute__((may_alias)) s16_alias;
typedef u16 __attribute__((may_alias)) u16_alias;
typedef s32 __attribute__((may_alias)) s32_alias;
typedef u32 __attribute__((may_alias)) u32_alias;
s32 func_80084A40(s32 actorIndex, s32 y, void* pFieldActor, u8* actorData,
                  s32 targetState) {
    u8* spriteData;
    s32 low[4];
    s32 high[4];
    s32 stateIds[4];
    s16 triIds[4];
    VECTOR normals[4];
    s32 origX;
    s32 origY;
    s32 origZ;
    s16 savedStates[4];
    u16 origState;
    s32 layer;
    s32 i;
    s32 currentLow;
    s32 flags;
    s16 cur;
    s16 actorY;
    s16 spriteY;

    (void)pFieldActor;
    spriteData = (u8*)(uintptr_t)*(u32_alias*)((u8*)g_FieldActors + actorIndex * 0x5C + 0x04);

    if (actorIndex == g_PlayerActorIndex) {
        D_800ADB00 = -1;
    }
    if (*(u32_alias*)(actorData + 0x00) & 0x01000000) {
        return -1;
    }
    if (*(u32_alias*)(actorData + 0x04) & 0x00200000) {
        return -1;
    }
    /* 80084ADC: flag 0x10000 set -> return -1 (no collision pass). */
    if (*(u32_alias*)(actorData + 0x00) & 0x00010000) {
        return -1;
    }
    if (!(actorIndex == g_PlayerActorIndex && D_800B21CF == 1) &&
        *(s32_alias*)(spriteData + 0x10) == 0 && func_8008492C(actorData) == 0 &&
        *(s16_alias*)(spriteData + 0x84) == *(s16_alias*)(actorData + 0x26)) {
        return -1;
    }

    origX = *(s32_alias*)(actorData + 0x20);
    origY = *(s32_alias*)(actorData + 0x24);
    origZ = *(s32_alias*)(actorData + 0x28);
    origState = *(u16_alias*)(actorData + 0x10);
    for (i = 0; i < 4; i++) {
        savedStates[i] = *(s16_alias*)(actorData + 0x08 + i * 2);
    }
    for (i = 0; i < 4; i++) {
        stateIds[i] = i;
        low[i] = 0x7FFFFFFF;
        high[i] = 0x7FFFFFFF;
    }

    layer = 0;
    if (D_800AFB54 - 1 > 0) {
        do {
            if (func_8007D3D4(actorData, layer, &low[layer], &normals[layer],
                              &triIds[layer], &high[layer]) != 0) {
                break;
            }
            layer++;
        } while (layer < D_800AFB54 - 1);
    }

    if (*(u32_alias*)(actorData + 0x04) & 0x1) {
        low[0] = 0x7FFFFFFF;
        high[0] = 0x7FFFFFFF;
    }
    if (*(u32_alias*)(actorData + 0x04) & 0x2) {
        low[1] = 0x7FFFFFFF;
        high[1] = 0x7FFFFFFF;
    }
    if (*(u32_alias*)(actorData + 0x04) & 0x4) {
        low[2] = 0x7FFFFFFF;
        high[2] = 0x7FFFFFFF;
    }

    currentLow = low[*(s16_alias*)(actorData + 0x10)];

    for (i = 0; i < 2; i++) {
        s32 j;
        for (j = 0; j < 2; j++) {
            if (low[j + 1] < low[j]) {
                s32 tmp;

                tmp = low[j]; low[j] = low[j + 1]; low[j + 1] = tmp;
                tmp = high[j]; high[j] = high[j + 1]; high[j + 1] = tmp;
                tmp = stateIds[j]; stateIds[j] = stateIds[j + 1]; stateIds[j + 1] = tmp;
            }
        }
    }

    if (layer == D_800AFB54 - 1) {
        if (layer > 0) {
            i = 0;
            do {
                *(s16_alias*)(actorData + 0x08 + i * 2) = triIds[i];
                i++;
            } while (i < D_800AFB54 - 1);
        }

        actorY = *(s16_alias*)(actorData + 0x26);
        i = 0;
        if (actorY < currentLow || (*(u32_alias*)(actorData + 0x00) & 0x1800) != 0) {
            s32 limit = D_800AFB54 - 1;
            for (i = 0; i < limit; i++) {
                if (!(low[i] < actorY)) {
                    *(s16_alias*)(actorData + 0x10) = (s16)stateIds[i];
                    break;
                }
            }
        } else {
            s32 limit = D_800AFB54 - 1;
            if (limit > 0) {
                cur = *(s16_alias*)(actorData + 0x10);
                for (i = 0; i < limit; i++) {
                    if (cur == stateIds[i]) {
                        break;
                    }
                }
            }
        }

        /* 80084E44: only when the state does NOT exceed count-1. */
        if ((func_80080968(actorData) & 4) != 0 && i != 0 &&
            !((D_800AFB54 - 1) < *(s16_alias*)(actorData + 0x10))) {
            *(s16_alias*)(actorData + 0x10) = (s16)stateIds[i - 1];
        }

        flags = func_80080968(actorData);
        if ((((*(u32_alias*)(actorData + 0x00) >> 8) & 7) & ((u32)flags >> 5)) != 0) {
            if (g_FieldSystemMode == 0) {
                func_800379C8(D_8006FC60, actorIndex);
            }
            goto blocked;
        }
        if (flags & 0x800000) {
            if (g_FieldSystemMode == 0) {
                func_800379C8(D_8006FC74, actorIndex);
            }
            goto blocked;
        }

        *(s32_alias*)(actorData + 0x20) = (s32)((u32)*(s32_alias*)(actorData + 0x20) + (u32)*(s32_alias*)(actorData + 0x30));
        *(s32_alias*)(actorData + 0x28) = (s32)((u32)*(s32_alias*)(actorData + 0x28) + (u32)*(s32_alias*)(actorData + 0x38));
        if (D_800AFB54 - 1 > 0) {
            s32 limit = D_800AFB54 - 1;
            cur = *(s16_alias*)(actorData + 0x10);
            for (i = 0; i < limit; i++) {
                if (cur == stateIds[i]) {
                    *(s16_alias*)(spriteData + 0x84) = (s16)low[i];
                    break;
                }
            }
        }
        VectorNormal(&normals[*(s16_alias*)(actorData + 0x10)], (VECTOR*)(actorData + 0x50));
    } else {
        *(s32_alias*)(actorData + 0xF0) = 0;
    }

    /* 80084FD0: shared tail.  Retail conditions on the FIFTH argument
     * (targetState, incoming slot 0x110(sp)); the writes use y ($s7). */
    if (D_800ADB98 != 0) {
        if ((u32)targetState < 2) {
            *(s16_alias*)(spriteData + 0x84) = (s16)y;
        }
    } else if (targetState != 0) {
        if (*(s16_alias*)(spriteData + 0x84) < (s32)((u32)y + 10u)) {
            *(u8*)(actorData + 0x74) = 0xFF;
        }
        *(s16_alias*)(spriteData + 0x84) = (s16)y;
        *(s32_alias*)(actorData + 0x24) = (s32)((u32)y << 16);
    }

    if (*(u32_alias*)(actorData + 0x00) & 0x00040000) {
        *(s32_alias*)(actorData + 0x24) = (s32)((u32)(s32)*(s16_alias*)(actorData + 0xEC) << 16);
        *(s32_alias*)(spriteData + 0x10) = 0;
    }

    *(s32_alias*)(actorData + 0x24) = (s32)((u32)*(s32_alias*)(actorData + 0x24) + (u32)*(s32_alias*)(spriteData + 0x10));
    flags = func_80080968(actorData);

    if (*(s16_alias*)(actorData + 0x10) != (s16)origState) {
        *(u32_alias*)(actorData + 0x00) &= 0xFBFFFFFF;
    }

    spriteY = *(s16_alias*)(spriteData + 0x84);
    actorY = *(s16_alias*)(actorData + 0x26);
    if ((*(u32_alias*)(actorData + 0x00) & 0x04000000) == 0 && actorY < spriteY) {
        if (spriteY != actorY) {
            *(s32_alias*)(spriteData + 0x10) = (s32)((u32)*(s32_alias*)(spriteData + 0x10) + (u32)*(s32_alias*)(spriteData + 0x1C));
        }
        *(u32_alias*)(actorData + 0x00) |= 0x1000;
        *(s32_alias*)(actorData + 0xF0) = *(s32_alias*)(spriteData + 0x10);
    } else {
        if ((flags & 0x420000) == 0) {
            *(s32_alias*)(actorData + 0xF0) = 0;
        }
        if (*(s32_alias*)(spriteData + 0x10) > 0) {
            *(s32_alias*)(spriteData + 0x10) = 0;
        }
        *(u32_alias*)(actorData + 0x00) &= 0xFFBFEFFF;
        *(s32_alias*)(actorData + 0x24) = (s32)((u32)(s32)*(s16_alias*)(spriteData + 0x84) << 16);
    }

    *(u32_alias*)(actorData + 0x00) &= 0xFBFFFFFF;

    i = 0;
    if (D_800AFB54 - 1 > 0) {
        s32 limit = D_800AFB54 - 1;
        actorY = *(s16_alias*)(actorData + 0x26);
        for (i = 0; i < limit; i++) {
            if (low[i] < actorY &&
                actorY - (s32)*(u16_alias*)(actorData + 0x1A) < high[i] &&
                low[i] != high[i]) {
                break;
            }
        }
    }

    if (i == D_800AFB54 - 1) {
        s16 tri;
        u8* triTable;
        s32 extra;

        cur = *(s16_alias*)(actorData + 0x10);
        tri = *(s16_alias*)(actorData + 0x08 + cur * 2);
        triTable = (u8*)(uintptr_t)(u32)D_800AFB24[cur];
        extra = (s32)((u32)(s32)*(s8*)(triTable + tri * 14 + 0x0D) << 2);
        if (extra < 0 &&
            *(s16_alias*)(actorData + 0x26) - (s32)*(u16_alias*)(actorData + 0x1A) <
                extra + *(s16_alias*)(spriteData + 0x84)) {
            goto restore_settle;
        }
        *(s32_alias*)(spriteData + 0x00) = *(s32_alias*)(actorData + 0x20);
        *(s32_alias*)(spriteData + 0x04) = *(s32_alias*)(actorData + 0x24);
        *(s32_alias*)(spriteData + 0x08) = *(s32_alias*)(actorData + 0x28);
        *(s32_alias*)((u8*)g_FieldActors + actorIndex * 0x5C + 0x20) = *(s16_alias*)(actorData + 0x22);
        *(s32_alias*)((u8*)g_FieldActors + actorIndex * 0x5C + 0x24) = *(s16_alias*)(actorData + 0x26);
        *(s32_alias*)((u8*)g_FieldActors + actorIndex * 0x5C + 0x28) = *(s16_alias*)(actorData + 0x2A);
        *(s32_alias*)(actorData + 0x14) = func_80080968(actorData);
        goto finish;
    }

restore_settle: /* 800852C0 */
    *(s32_alias*)(actorData + 0x20) = origX;
    *(s16_alias*)(actorData + 0x10) = (s16)origState;
    *(s32_alias*)(actorData + 0xF0) = 0;
    *(s32_alias*)(actorData + 0x28) = origZ;
    for (i = 0; i < 4; i++) {
        *(s16_alias*)(actorData + 0x08 + i * 2) = savedStates[i];
    }
    if (*(s16_alias*)(spriteData + 0x84) != *(s16_alias*)(actorData + 0x26)) {
        *(s32_alias*)(spriteData + 0x10) = (s32)((u32)*(s32_alias*)(spriteData + 0x10) + (u32)*(s32_alias*)(spriteData + 0x1C));
    }
    if (*(s32_alias*)(spriteData + 0x10) < 0) {
        *(s32_alias*)(spriteData + 0x10) = 0;
        *(s32_alias*)(actorData + 0x24) = origY;
    }
    goto store_back;

blocked: /* 80084F0C, then 80085398 */
    if (actorIndex == g_PlayerActorIndex) {
        D_800ADB00 = 0x0FFF;
    }
    *(s32_alias*)(actorData + 0x24) = (s32)((u32)*(s32_alias*)(actorData + 0x24) + (u32)*(s32_alias*)(spriteData + 0x10));
    *(s32_alias*)(actorData + 0x20) = origX;
    *(s16_alias*)(actorData + 0x10) = (s16)origState;
    *(s32_alias*)(actorData + 0xF0) = 0;
    *(s32_alias*)(actorData + 0x28) = origZ;
    for (i = 0; i < 4; i++) {
        *(s16_alias*)(actorData + 0x08 + i * 2) = savedStates[i];
    }
    spriteY = *(s16_alias*)(spriteData + 0x84);
    actorY = *(s16_alias*)(actorData + 0x26);
    if (actorY < spriteY) {
        if (spriteY != actorY) {
            *(s32_alias*)(spriteData + 0x10) = (s32)((u32)*(s32_alias*)(spriteData + 0x10) + (u32)*(s32_alias*)(spriteData + 0x1C));
        }
    } else {
        if (*(s32_alias*)(spriteData + 0x10) > 0) {
            *(s32_alias*)(spriteData + 0x10) = 0;
        }
        *(u32_alias*)(actorData + 0x00) &= 0xFFBFEFFF;
        *(s32_alias*)(actorData + 0x24) = (s32)((u32)(s32)*(s16_alias*)(spriteData + 0x84) << 16);
    }

store_back:
    *(s32_alias*)(spriteData + 0x00) = *(s32_alias*)(actorData + 0x20);
    *(s32_alias*)(spriteData + 0x04) = *(s32_alias*)(actorData + 0x24);
    *(s32_alias*)(spriteData + 0x08) = *(s32_alias*)(actorData + 0x28);
    *(s32_alias*)((u8*)g_FieldActors + actorIndex * 0x5C + 0x24) = *(s16_alias*)(actorData + 0x26);

finish:
    func_80081C54(actorIndex);
    return 0;
}
#endif /* XENO_PC_PORT */

extern s32 D_800ADBBC;
extern s32 D_800ADBB8;
extern s32 D_800ADB2C;
extern s32 D_800AFEA4;
extern s32 func_80028B14(void);

// Per-frame CD-stream pump for the in-flight archive read kicked off by
// func_80085560: func_80028B14 reports the buffered sector address that is
// ready, or 0 when no chunk transfer is in flight. While a chunk is ready,
// forward it to
// the read's registered per-chunk callback (D_800AFEA4, e.g. func_800859DC)
// and report busy. Once idle and ArchiveDataSync confirms the CD/archive
// subsystem is fully caught up, free the streaming buffer and clear
// D_800ADB2C -- the flag func_800932D0 (CHANGE_FIELD) gates on.
s32 func_800854D0(void) {
    s32 chunkIndex;

    chunkIndex = func_80028B14();
    D_800ADBBC = chunkIndex;
    if (chunkIndex != 0) {
        ((void (*)(s32))(uintptr_t)D_800AFEA4)(chunkIndex);
        return 0;
    }

    if (ArchiveDataSync() != 0) {
        return 0;
    }
    if (D_800ADBBC != 0) {
        return 0;
    }

    HeapFree((void*)(uintptr_t)D_800ADBB8);
    D_800ADB2C = 0;
    return -1;
}

extern s32 D_800ADB2C;
extern s32 D_800ADBB8;
extern s32 D_800AFEA4;
extern int* ArchiveAllocStreamFile(int numEntries, int allocMode);

void func_80085560(s32 a0, s32 a1, s32 a2) {
    int* r;
    D_800ADB2C = 1;
    r = ArchiveAllocStreamFile(8, a1);
    D_800ADBB8 = (s32)(uintptr_t)r;
    ArchiveReadFileToBuffer(a0, r, 0, 0x100);
    D_800AFEA4 = a2;
}

extern void func_80039F9C(s32 packedId, s32 slot, s32 volume, s32 pan);

// Field SFX cue (script opcodes, transition sounds): stop whatever is on
// the channel's element pair, then fire the packed effect through the S1
// play chain (func_80039F9C -> func_8003B644 arm -> the tick plays it).
// Un-shimmed: the whole downstream is real as of S1.
#ifndef XENO_PC_PORT
void func_800855C8(s32 soundId, s32 volume, s32 pan, s32 channel)
{
    s32 slot = channel & 0x7;

    func_8003A20C(slot << 1);
    /* No-op pair: retail recomputes slot << 1 after the call instead of
     * keeping it in a saved register; this is the form that reproduces it
     * (found with decomp-permuter). */
    slot++;
    slot--;
    func_80039F9C(soundId, slot << 1, (s16)volume, (s16)pan);
}
#else
void func_800855C8(s32 soundId, s32 volume, s32 pan, s32 channel)
{
    s32 slot = channel & 0x7;
#ifdef XENO_PC_PORT
    if (getenv("XENO_FIELD_DIAG") != NULL) {
        printf("[field-sfx] cue id=0x%x vol=%d pan=%d chan=%d\n",
               soundId, volume, pan, channel);
    }
#endif
    func_8003A20C(slot << 1);
    func_80039F9C(soundId, slot << 1, (s16)volume, (s16)pan);
}
#endif /* XENO_PC_PORT */

extern s32 D_800B21B8;

// Field SFX cue (simple form): id 0 stops the channel pair; otherwise fire
// at default volume/pan. The channel is the retail fourth argument
// (a1 & 7, previously dropped in transcription -- latent while
// func_800855C8 was a no-op shim).
void func_80085634(int a0, int a1) {
    s32 channel = a1 & 7;

    if (a0 == 0) {
        func_8003A20C(channel * 2);
    } else {
        D_800B21B8 = a0;
        func_800855C8(a0, 0x7F, 0x40, channel);
    }
}

extern s16 D_800C3A38;
extern u16 D_800AE060[];
extern s32 D_800C3A64;
extern u16 D_800C3A2C;
extern s32 D_800B06A0;
extern void* D_800B235C;
extern void func_80039EC4(s32 a0, s32 channel);


void func_80085678(void) {
#ifdef XENO_PC_PORT
    u16* pTable = D_800AE060;
    if (D_800C3A38 == 0xFF) return;
    while (1) {
        s32 idx = D_800C3A64;
        u16 val = pTable[idx * 2];
        if ((s32)(val + D_800C3A2C) > D_800B06A0) break;
        {
            u16 data = pTable[idx * 2 + 1];
            u32 lo = data & 0xFF;
            u32 hi = (data >> 7) & 0xE;
            /* The WDS bank ID is inline; channel bits are a separate ABI argument. */
            u32 param = lo | ((u32)*(u16*)((u8*)D_800B235C + 0x14) << 16);
            func_80039EC4(param, hi);
            *(u32*)(&D_800C3A64) = D_800C3A64 + 1;
        }
    }
#else
    void func_80039EC4__ret(s32, s32) asm("func_80039EC4");
    s32 temp_a1;
    register u8 *base asm("$16");
    register u8 *values asm("$17");
    u16 temp_a1_2;

    if (D_800C3A38 != 0xFF) {
        base = (u8 *)D_800AE060;
        values = base + 2;
loop_2:
        temp_a1 = D_800C3A64 * 4;
        if (D_800B06A0 >= (*(u16 *)(temp_a1 + (u32)base) + D_800C3A2C)) {
            temp_a1_2 = *(u16 *)(temp_a1 + (u32)values);
            func_80039EC4__ret((temp_a1_2 & 0xFF) | (*(u16 *)((s8*)(D_800B235C) + 0x14) << 0x10), (temp_a1_2 >> 7) & 0xE);
            D_800C3A64 += 1;
            goto loop_2;
        }
    }
#endif
}

extern s16 D_800C3A38;
extern void* D_800B235C;

void func_80085738(void) {
    if (D_800C3A38 != 0xFF) {
        func_80039FF8();
        func_8003852C(D_800B235C);
        HeapFree(D_800B235C);
    }
}

extern void func_80039FF8(void);
extern void func_8003BDFC(s32);
extern void SoundAddSedsEntry(void* pData);

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): field_movie_load_sound_bank (src/decomp/src/field/field_event.c:167). */
#ifndef XM_T_FieldMovieRequest
#define XM_T_FieldMovieRequest
typedef struct FieldMovieRequest {
    s16 file;        
    u16 x;           
    u16 y;           
    u16 source_x;    
    u16 source_y;    
    u16 unk2A;       
    u16 sound_start; 
    u16 unk2E;       
    u16 mode;        
    u16 width;       
    u16 height;      
    u16 depth24;     
    s16 sound_bank;  
    u16 unk3A;       
} FieldMovieRequest;
#endif
#ifndef XM_T_FIELD_MOVIE
#define XM_T_FIELD_MOVIE
#define FIELD_MOVIE xm_field_movie_request
#endif
#ifndef XM_T_SoundBank
#define XM_T_SoundBank
typedef struct SoundBank {
    u8 unk0[0x10];
    u16 flags;
    u8 unk12[2];
    u16 id;
    u16 unk16;         
    u16 volumes;       
    u8 unk1A[2];
    struct SoundBank *next;
    u16 effect[1];     
} SoundBank;
#endif
#ifndef XM_S_field_movie_load_sound_bank
#define XM_S_field_movie_load_sound_bank
void xm_field_movie_load_sound_bank(void) __asm__("func_80085788");
#endif
#ifndef XM_S_field_movie_request
#define XM_S_field_movie_request
extern FieldMovieRequest xm_field_movie_request __asm__("D_800C3A20");
#endif
#ifndef XM_S_sound_stop_all_effects
#define XM_S_sound_stop_all_effects
void xm_sound_stop_all_effects(void) __asm__("func_80039FF8");
#endif
#ifndef XM_S_cd_select_directory
#define XM_S_cd_select_directory
s32 xm_cd_select_directory(s32 group, s32 index) __asm__("ArchiveSetIndex");
#endif
#ifndef XM_S_field_movie_sound_bank
#define XM_S_field_movie_sound_bank
extern SoundBank *xm_field_movie_sound_bank __asm__("D_800B235C");
#endif
#ifndef XM_S_cd_get_aligned_file_size
#define XM_S_cd_get_aligned_file_size
s32 xm_cd_get_aligned_file_size(s32 file) __asm__("ArchiveDecodeAlignedSize");
#endif
#ifndef XM_S_cd_read_file
#define XM_S_cd_read_file
s32 xm_cd_read_file(s32 file, void *destination, s32 mode, s32 flags) __asm__("ArchiveReadFileToBuffer");
#endif
#ifndef XM_S_cd_sync_reads
#define XM_S_cd_sync_reads
s32 xm_cd_sync_reads(s32 mode) __asm__("ArchiveCdDataSync");
#endif
#ifndef XM_S_sound_add_effect_bank
#define XM_S_sound_add_effect_bank
void xm_sound_add_effect_bank(SoundBank *bank) __asm__("SoundAddSedsEntry");
#endif
#ifndef XM_S_sound_sync_transfer
#define XM_S_sound_sync_transfer
s16 xm_sound_sync_transfer(s32 wait) __asm__("func_8003BDFC");
#endif
#ifndef XM_S_field_movie_sound_timelines
#define XM_S_field_movie_sound_timelines
extern u16 xm_field_movie_sound_timelines[][2] __asm__("D_800AE060");
#endif
#ifndef XM_S_field_movie_sound_timeline_index
#define XM_S_field_movie_sound_timeline_index
extern s32 xm_field_movie_sound_timeline_index __asm__("D_800C3A64");
#endif
void func_80085788(void) {
    s32 bank;
    s32 file;
    s32 pos;
    s32 i;

    file = FIELD_MOVIE.sound_bank;
    if (file != 0xFF) {
        bank = file;
        xm_sound_stop_all_effects();
        xm_cd_select_directory(0x1C, 0);
        file = bank + 0x115;
        xm_field_movie_sound_bank = xm_heap_alloc(xm_cd_get_aligned_file_size(file), 1);
        xm_cd_read_file(file, xm_field_movie_sound_bank, 0, 0x80);
        xm_cd_sync_reads(0);
        xm_sound_add_effect_bank(xm_field_movie_sound_bank);
        xm_sound_sync_transfer(0x10);
        xm_cd_select_directory(4, 0);
        pos = 0;
        for (i = 0; i < bank + 1; i++) {
            while (1) {
                if (((u16 *)xm_field_movie_sound_timelines)[pos * 2] == 0xFFFF) {
                    break;
                }
                pos++;
            }
            pos++;
            xm_field_movie_sound_timeline_index = pos;
        }
    }
}
#endif
#else
void func_80085788(void) {
    s32 musicId = D_800C3A38;
    if (musicId == 0xFF) return;
    func_80039FF8();
    ArchiveSetIndex(0x1C, 0);
    {
        s32 fileId = musicId + 0x115;
        D_800B235C = HeapAlloc(ArchiveDecodeAlignedSize(fileId), 1);
        ArchiveReadFileToBuffer(fileId, D_800B235C, 0, 0x80);
        ArchiveCdDataSync(0);
        SoundAddSedsEntry(D_800B235C);
        func_8003BDFC(0x10);
    }
    ArchiveSetIndex(4, 0);
    {
        s32 i;
        s32 idx = 0;
        s32 count = musicId + 1;
        if (count > 0) {
            u16* pTable = D_800AE060;
            for (i = 0; i < count; i++) {
                while (pTable[idx * 2] != 0xFFFF) {
                    idx++;
                }
                idx++;
                D_800C3A64 = idx;
            }
        }
    }
}
#endif /* XENO_PC_PORT */

extern void* D_8006259C;
extern s32 D_8004F32C;
extern void* D_8005A4BC;

void func_80085890(void) {
    s32 size;

    ArchiveSetIndex(4, 0);
    size = ArchiveDecodeAlignedSize(0xA8);
    D_8006259C = HeapAlloc(size, 0);
    HeapPinBlock(D_8006259C);

    if (D_8004F32C == -1) {
        ArchiveReadFileToBuffer(0xA8, D_8006259C, 0, CdlModeSpeed);
        ArchiveCdDataSync(0);
    } else {
        memcpy(D_8006259C, D_8005A4BC, size);
        HeapUnpinBlock(D_8005A4BC);
        HeapFree(D_8005A4BC);
    }

    SoundAddSedsEntry(D_8006259C);
    func_8003BDFC(0x10);
    ArchiveSetIndex(4, 0);
    D_8004F32C = -1;
}

void func_80085988(void) {
    func_8003852C(D_8006259C);
    HeapUnpinBlock(D_8006259C);
    HeapFree(D_8006259C);
    D_8004F32C = -1;
}

extern s32 D_800B2370;
extern void* D_800C3A1C;
extern void* g_GameCurLoadedWDS;
extern s32 func_800380D0(u8* data, s32 size, s32 a2);
extern void SoundTransferWdsPart(u8* data, s32 size);
extern s32 func_8002945C(u8* pSlot);

#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc8", func_800859DC);
#else
void func_800859DC(u8* pSrc) {
    if (D_800B2370 < 0) return;
    if (D_800B2370 < 4) {
        u8* pDst = D_800C3A1C + D_800B2370 * 0x800;
        s32 i;
        for (i = 0; i < 0x800; i += 16) {
            *(s32*)(pDst + i) = *(s32*)(pSrc + i);
            *(s32*)(pDst + i + 4) = *(s32*)(pSrc + i + 4);
            *(s32*)(pDst + i + 8) = *(s32*)(pSrc + i + 8);
            *(s32*)(pDst + i + 0xC) = *(s32*)(pSrc + i + 0xC);
        }
        D_800B2370++;
        func_8002945C(pSrc);
        if (D_800B2370 == 4) {
            g_GameCurLoadedWDS = func_800380D0(D_800C3A1C, 0x2000, 0);
        }
    } else if (D_800B2370 == 4) {
        u8* pDst = D_800C3A1C;
        s32 i;
        func_8003BDFC(0x10);
        for (i = 0; i < 0x800; i += 16) {
            *(s32*)(pDst + i) = *(s32*)(pSrc + i);
            *(s32*)(pDst + i + 4) = *(s32*)(pSrc + i + 4);
            *(s32*)(pDst + i + 8) = *(s32*)(pSrc + i + 8);
            *(s32*)(pDst + i + 0xC) = *(s32*)(pSrc + i + 0xC);
        }
        SoundTransferWdsPart(D_800C3A1C, 0x800);
        func_8002945C(pSrc);
    }
}
#endif /* XENO_PC_PORT */

extern u8 D_800ADFCC[];
extern s32 D_8004F308;
extern s32 D_8004F33C;
extern s32 D_8004F354;
extern s32 D_800AFC54;
extern s32 D_800B2370;
extern void* D_800C3A1C;
extern void func_8001B66C(void);
extern void func_800859DC(u8* pSrc);

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): field_music_change_track (src/decomp/src/field/field_event.c:256). */
#ifndef XM_T_WaveChunk
#define XM_T_WaveChunk
typedef struct {
    u32 words[0x200];
} WaveChunk;
#endif
#ifndef XM_S_field_music_change_track
#define XM_S_field_music_change_track
void xm_field_music_change_track(s32 music, s32 unused) __asm__("func_80085B20");
#endif
#ifndef XM_S_mode_stop_music
#define XM_S_mode_stop_music
void xm_mode_stop_music(void) __asm__("func_8001B66C");
#endif
#ifndef XM_S_mode_music_load_pending
#define XM_S_mode_music_load_pending
extern s32 xm_mode_music_load_pending __asm__("D_8004F308");
#endif
#ifndef XM_S_field_music_wave_table
#define XM_S_field_music_wave_table
extern u8 xm_field_music_wave_table[73 * 2] __asm__("D_800ADFCC");
#endif
#ifndef XM_S_field_music_release_shared_wave_bank
#define XM_S_field_music_release_shared_wave_bank
void xm_field_music_release_shared_wave_bank(void) __asm__("func_80086024");
#endif
#ifndef XM_S_mode_music_loaded_wave
#define XM_S_mode_music_loaded_wave
extern s32 xm_mode_music_loaded_wave __asm__("D_8004F33C");
#endif
#ifndef XM_S_field_music_start_stream
#define XM_S_field_music_start_stream
void xm_field_music_start_stream(s32 file, s32 unused, void (*callback)(s32)) __asm__("func_80085560");
#endif
#ifndef XM_S_field_music_gather_wave_chunk
#define XM_S_field_music_gather_wave_chunk
void xm_field_music_gather_wave_chunk(WaveChunk *chunk) __asm__("func_800859DC");
#endif
#ifndef XM_S_mode_music_wave_streaming
#define XM_S_mode_music_wave_streaming
extern s32 xm_mode_music_wave_streaming __asm__("D_8004F354");
#endif
#ifndef XM_S_field_music_chunk_count
#define XM_S_field_music_chunk_count
extern s32 xm_field_music_chunk_count __asm__("D_800B2370");
#endif
#ifndef XM_S_field_music_gather_buffer
#define XM_S_field_music_gather_buffer
extern void *xm_field_music_gather_buffer __asm__("D_800C3A1C");
#endif
#ifndef XM_S_field_music_seq_read_pending
#define XM_S_field_music_seq_read_pending
extern s32 xm_field_music_seq_read_pending __asm__("D_800AFC54");
#endif
void func_80085B20(s32 music, s32 unused) {
    u8 wave;
    s32 file;

    xm_cd_sync_reads(0);
    xm_mode_stop_music();
    if (music == 0xFF) {
        xm_mode_music_load_pending = 0;
        return;
    }
    xm_cd_select_directory(0x1C, 0);
    if (xm_field_music_wave_table[music * 2 + 1] == 1) {
        xm_field_music_release_shared_wave_bank();
    }
    wave = xm_field_music_wave_table[music * 2];
    if (wave != 0xFF) {
        file = wave * 2 + 0x13;
        if (xm_mode_music_loaded_wave != wave) {
            xm_field_music_start_stream(file, 1, (void (*)(s32))xm_field_music_gather_wave_chunk);
            xm_mode_music_wave_streaming = 1;
            xm_field_music_chunk_count = 0;
            xm_field_music_gather_buffer = xm_heap_alloc(0x2000, 1);
        }
    }
    xm_cd_select_directory(4, 0);
    xm_mode_music_load_pending = -1;
    xm_field_music_seq_read_pending = 1;
}
#endif
#else
void func_80085B20(s32 a0) {
    u8 archiveFile;

    ArchiveCdDataSync(0);
    func_8001B66C();

    if (a0 == 0xFF) {
        D_8004F308 = 0;
        return;
    }

    ArchiveSetIndex(0x1C, 0);

    if (D_800ADFCC[a0 * 2 + 1] == 1) {
        func_80086024();
    }

    archiveFile = D_800ADFCC[a0 * 2];
    if (archiveFile != 0xFF && D_8004F33C != archiveFile) {
        func_80085560(archiveFile * 2 + 0x13, 1, (s32)func_800859DC);
        D_8004F354 = 1;
        D_800B2370 = 0;
        D_800C3A1C = HeapAlloc(0x2000, 1);
    }

    ArchiveSetIndex(4, 0);
    D_8004F308 = -1;
    D_800AFC54 = 1;
}
#endif /* XENO_PC_PORT */

s32 func_80085C3C(void) {
    s32 count;
    for (count = 0; count < 5; count++) {
        if (func_800854D0() == -1) {
            return 0;
        }
    }
    return -1;
}

extern s32 D_8004F338;
extern s32 D_8004F36C;
extern s32 D_8004F354;
extern s32 D_8004F364;
extern int func_80085F30(void);
extern void func_80085FB8(void);
extern void* SoundLoadWdsFile(void* pWdsFile, s32 mode);

extern s32 D_8004F33C;
extern s32 D_8004F340;
extern s32 D_8004F348;
extern s32 D_8004F358;
extern s32 D_8004F35C;
extern s32 D_800AFC54;
extern void* D_8004F2FC;
extern void* D_80062528;
extern u8 D_80062648[];
extern s32 g_GameHasLoadedWDS;
extern u8 D_800ADFCC[];
extern void* D_800C3A1C;
extern void func_8003BDFC(s32);

/* Song-start M3: the retail per-frame music poller.  The bank-swap leg is
 * completed by the eight-slot archive sector ring and func_800859DC, matching
 * the original streamed WDS path; no whole-file host staging is used. */
s32 func_80085C90(s32 a0) {
    extern void* func_80039850(void* pSongFile);
    extern void func_80039A80(void* manager, s32 level, s32 steps);
    extern void func_80039B68(void* manager, s32 level, s32 steps);
    extern void func_8003A89C(void* manager, s32 level, s32 steps);

    if (D_8004F354 == 1) {
        if (func_80085C3C() == -1) {
            return -1;
        }
        func_8003BDFC(0x10);
        HeapFree(D_800C3A1C);
        D_8004F354 = 0;
        g_GameHasLoadedWDS = 1;
        D_8004F33C = D_800ADFCC[a0 * 2];
    }

    /* Common-bank leg: maps flagged 0 at D_800ADFCC[idx*2+1] use the shared
     * field bank (dir 0x1C file 3) -- kick its read, then complete it. */
    if (D_800ADFCC[a0 * 2 + 1] == 0) {
        s32 state = D_8004F364;
        if (state == 0) {
            func_80085FB8();
            return -1;
        }
        if (state & 0x80) {
            if (func_80085F30() == -1) {
                return -1;
            }
        }
    }

    /* Song-file leg: on the first frame after a music change, queue the
     * 'smds' song read into the static song buffer (D_80062648). */
    if (D_800AFC54 == 1) {
        if (D_8004F338 != a0) {
            ArchiveSetIndex(0x1C, 0);
            ArchiveReadFileToBuffer(a0 * 2 + 0x14, D_80062648, 0, 0x80);
            D_8004F358 = 1;
            ArchiveSetIndex(4, 0);
        }
        D_800AFC54 = 0;
        return -1;
    }
    if (ArchiveDataSync() != 0) {
        return -1;
    }

    /* Song landed: create/start the music manager (the M1 arming chain). */
    if (D_8004F358 == 1) {
        if (D_8004F348 == 0) {
            void* manager = func_80039850(D_80062648);
            D_80062528 = manager;
            if (D_8004F340 == -1) {
                func_80039A80(manager, 0x7F, 0);
            } else {
                /* Muted start: a pending scripted fade (FE 0E) raises it. */
                func_80039A80(manager, 0, 0);
                func_8003A89C(D_80062528, 0, 0);
            }
        } else {
            /* Same song, bank re-landed: resume the saved manager. */
            D_80062528 = D_8004F2FC;
            func_80039B68(D_8004F2FC, 0x7F, 0xF0);
            D_8004F348 = 0;
            D_8004F2FC = 0;
        }
        D_8004F358 = 0;
        D_8004F35C = 1;
        D_8004F338 = a0;
    }

    D_8004F340 = -1;
    D_8004F36C = 1;
    return 0;
}

extern void* D_8004F2FC;

void func_80085EEC(void) {
    if (D_8004F2FC != NULL) {
        func_80039C4C(D_8004F2FC);
        func_800399D4(D_8004F2FC);
        D_8004F2FC = NULL;
    }
}

extern s32 D_8004F364;
extern s32 D_8004F368;
extern s16 D_8004F384;
extern SoundWDSEntry* D_80059560;
extern SoundWDSEntry* D_8006251C;
extern void* D_800B00E0; // WDS File Buffer

int func_80085F30(void) {
    void* pWdsEntry;

    if (ArchiveDataSync()) {
        return -1;
    }
    
    pWdsEntry = SoundLoadWdsFile(D_800B00E0, 0);
    D_8006251C = pWdsEntry;
    D_80059560 = pWdsEntry;
    func_8003BDFC(0x10);
    HeapFree(D_800B00E0);
    D_8004F364 = 1;
    D_8004F384 = 0;
    D_8004F368 = 0;
    return 0;
}

void func_80085FB8(void) {
    void* pWdsFileBuffer;

    ArchiveSetIndex(0x1C, 0x0);
    pWdsFileBuffer = HeapAlloc(ArchiveDecodeAlignedSize(3), 1);
    D_800B00E0 = pWdsFileBuffer;
    ArchiveReadFileToBuffer(3, pWdsFileBuffer, 0, CdlModeSpeed);
    ArchiveSetIndex(4, 0);
    D_8004F364 = 0x80;
}

void func_80086024(void) {
    if (D_8004F368 == 0) {
        D_8004F384 = 1;
        SoundFreeWdsEntry(D_8006251C);
        D_8004F368 = 1;
    }
    D_8004F364 = 0;
}


extern s16 D_800B21AC;
extern u16 D_800AFE88[];
extern u16 D_800AFE8A[];
extern void func_8003A20C(s32);
extern void func_8003A344(s32, s32);
extern void func_8003A55C(s32, s32);
extern void func_80039F9C(s32, s32, s32, s32);

/* Attenuation from distance vs D_800B21AC, scaled by mode. Writes volume to *out. */
#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): field_sound_compute_emitter_volume (src/decomp/src/field/field_event.c:411). */
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
#ifndef XM_S_field_sound_compute_emitter_volume
#define XM_S_field_sound_compute_emitter_volume
void xm_field_sound_compute_emitter_volume(s32 distance, u32 *out, s32 volume) __asm__("func_80086078");
#endif
void func_80086078(s32 distance, u32 *out, s32 volume) {
    s32 level;

    if (distance > xm_field_work.emitter_range) {
        distance = xm_field_work.emitter_range;
    }
    level = 0x80 - (((0x7F0000 / xm_field_work.emitter_range) * distance) >> 16);
    *out = ((u32)(level << 16) / 127 * volume) >> 16;
}
#endif
#else
void func_80086078(s32 distance, s32* outVolume, s32 mode) {
    s32 maxDist = D_800B21AC;
    u32 scaled;
    u32 inv;
    u32 t;

    if (maxDist < distance) {
        distance = maxDist;
    }

    /* div+mult pair: quotient (mflo), then the wrapping low 32 of
     * quotient*distance (mult+mflo). The u32 multiply wraps exactly like
     * mflo, so no host signed-overflow UB. Retail div never traps: a zero
     * divisor yields lo=-1 for this non-negative dividend (emulator models
     * the same), and 0x7F0000/-1 cannot overflow. */
    scaled = (maxDist == 0 ? 0xFFFFFFFFu : (u32)(0x7F0000 / maxDist)) * (u32)distance;
    inv = 0x80u - (u32)((s32)scaled >> 16);
    inv <<= 16;
    /* Unsigned reciprocal multiply by 0x02040811 ≈ 1/127, then >> 6. */
    t = (u32)(((u64)inv * 0x02040811u) >> 32);
    inv = inv - t;
    inv = (inv >> 1) + t;
    inv >>= 6;
    /* Final mult is SIGNED, but only mflo (low 32) survives the srl >> 16,
     * so a wrapping u32 multiply followed by a logical shift is exact. */
    *outVolume = (s32)((inv * (u32)mode) >> 16);
}
#endif /* XENO_PC_PORT */

/* Asm multiply chain: (((x*3)*17)*257*2)>>16 == (x * 0x6666)>>16. */
static inline s32 FieldPositionalSfxScreenPan(s32 screenX) {
    s32 t = screenX;
    t = (t << 1) + t;
    t = t + (t << 4);
    t = t + (t << 8);
    t <<= 1;
    return t >> 16;
}

void FieldActorWorldToScreenPosition(s32 actorIndex, s32* outX, s32* outY);

/* Update an already-bound positional SFX slot's volume/pan. */
#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): field_sound_update_emitter_voices (src/decomp/src/field/field_event.c:423). */
#ifndef XM_T_EmitterSlot
#define XM_T_EmitterSlot
typedef struct EmitterSlot {
    u16 actor;  
    u16 sound;  
    u16 unk4;
} EmitterSlot;
#endif
#ifndef XM_S_field_sound_update_emitter_voices
#define XM_S_field_sound_update_emitter_voices
void xm_field_sound_update_emitter_voices(s32 unused0, s32 volume, s32 unused2, s32 distance, s32 id) __asm__("func_800860F0");
#endif
#ifndef XM_S_field_sound_emitter_slots
#define XM_S_field_sound_emitter_slots
extern EmitterSlot xm_field_sound_emitter_slots[3] __asm__("D_800AFE88");
#endif
#ifndef XM_S_field_sound_get_descriptor_screen_xy
#define XM_S_field_sound_get_descriptor_screen_xy
void xm_field_sound_get_descriptor_screen_xy(s32 index, s32 *x, s32 *y) __asm__("FieldActorWorldToScreenPosition");
#endif
#ifndef XM_S_sound_set_effect_volume_on_channel
#define XM_S_sound_set_effect_volume_on_channel
void xm_sound_set_effect_volume_on_channel(s32 sound, s32 volume) __asm__("func_8003A344");
#endif
#ifndef XM_S_sound_set_effect_pan_on_channel
#define XM_S_sound_set_effect_pan_on_channel
void xm_sound_set_effect_pan_on_channel(s32 sound, s32 pan) __asm__("func_8003A55C");
#endif
void xm_field_sound_update_emitter_voices(s32 unused0, s32 volume, s32 unused2, s32 distance, s32 id) __asm__("func_800860F0");
void xm_field_sound_update_emitter_voices(s32 unused0, s32 volume, s32 unused2, s32 distance, s32 id) {
    s32 i;
    s32 voice;
    s32 pan;
    u32 level;
    s32 x;
    s32 y;

    for (i = 0; i < 3; i++) {
        if (xm_field_sound_emitter_slots[i].actor == id) {
            voice = i * 2;
            xm_field_sound_compute_emitter_volume(distance, &level, volume);
            xm_field_sound_get_descriptor_screen_xy(id, &x, &y);
            if (x > 0x140) {
                x = 0x13F;
            }
            if (x < 0) {
                x = 0;
            }
            pan = (x * 0x6666) >> 16;
            xm_sound_set_effect_volume_on_channel(voice, level);
            xm_sound_set_effect_pan_on_channel(voice, pan);
        }
    }
}
#endif
#else
void func_800860F0(s32 soundId, s32 mode, s16 deltaX, s32 distance, s32 actorIndex) {
    s32 i;
    s32 volume;
    s32 screenX;
    s32 screenY;
    s32 pan;
    s32 slot2;

    (void)soundId;
    (void)deltaX;

    /* Retail: lhu slot vs the full 32-bit actorIndex word (no truncation),
     * and the loop never breaks -- every matching slot is updated. */
    for (i = 0; i < 3; i++) {
        if ((u32)D_800AFE88[i * 3] == (u32)actorIndex) {
            slot2 = i * 2;
            func_80086078(distance, &volume, mode);
            FieldActorWorldToScreenPosition(actorIndex, &screenX, &screenY);
            if (screenX >= 0x141) {
                screenX = 0x13F;
            }
            if (screenX < 0) {
                screenX = 0;
            }
            pan = FieldPositionalSfxScreenPan(screenX);
            func_8003A344(slot2, volume);
            func_8003A55C(slot2, pan);
        }
    }
}
#endif /* XENO_PC_PORT */

/* Project actor origin through worldToScreen; write screen X/Y.
 * Retail addresses worldToScreen as &g_FieldActors - 0xAC (= g_Scene + 0xD4). */
#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): field_sound_get_descriptor_screen_xy (src/decomp/src/field/field_event.c:450). */
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
#ifndef XM_S_CompMatrix
#define XM_S_CompMatrix
MATRIX *xm_CompMatrix(MATRIX *m0, MATRIX *m1, MATRIX *m2) __asm__("CompMatrix");
#endif
#ifndef XM_S_field_event_read_actor_index
#define XM_S_field_event_read_actor_index
s32 xm_field_event_read_actor_index(s32 offset) __asm__("FieldScriptVMGetActorIndex");
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
void FieldActorWorldToScreenPosition(s32 index, s32 *x, s32 *y) {
    SVECTOR point;
    MATRIX m;
    long screen;
    long depth;
    long flag;

    
    xm_CompMatrix(&xm_field_view.scaled_world, (xm_field_event_read_actor_index(1), &xm_field_view.components.descriptors[index].matrix),
               &m);
    point.vx = 0;
    point.vy = 0;
    point.vz = 0;
    xm_SetRotMatrix(&m);
    xm_SetTransMatrix(&m);
    xm_RotTransPers(&point, &screen, &depth, &flag);
    *y = screen >> 16;
    *x = (s16)screen;
}
#endif
#else
void FieldActorWorldToScreenPosition(s32 actorIndex, s32* outX, s32* outY) {
    FieldActor* actors = g_FieldActors;
    MATRIX composed;
    SVECTOR local;
    long screenXY;
    long p;
    long flag;

    (void)FieldScriptVMGetActorIndex(1);

    CompMatrix(&g_Scene.worldToScreenMatrix, &actors[actorIndex].transformMatrix, &composed);
    local.vx = 0;
    local.vy = 0;
    local.vz = 0;
    SetRotMatrix(&composed);
    SetTransMatrix(&composed);
    RotTransPers(&local, &screenXY, &p, &flag);
    *outY = (s16)(screenXY >> 16);
    *outX = (s16)screenXY;
}
#endif /* XENO_PC_PORT */

/* Allocate a free positional SFX slot and start the voice with distance pan. */
#ifndef XENO_PC_PORT
/* func_800862CC: slot2 = i * 2 at the top of the hit (its own pseudo, so the
 * func_80039F9C slot argument is recomputed from i), pan computed before the
 * func_8003A20C call, explicit (s16) casts on the slot/volume arguments. */
#endif
void func_800862CC(s32 soundId, s32 mode, s16 deltaX, s32 distance, s32 actorIndex) {
#ifdef XENO_PC_PORT
    s32 i;
    s32 volume;
    s32 screenX;
    s32 screenY;
    s32 pan;
    s32 slot2;

    (void)deltaX;

    for (i = 0; i < 3; i++) {
        if (D_800AFE8A[i * 3] == 0xFFFF) {
            slot2 = i * 2;
            D_800AFE8A[i * 3] = (u16)soundId;
            D_800AFE88[i * 3] = (u16)actorIndex;
            func_80086078(distance, &volume, mode);
            FieldActorWorldToScreenPosition(actorIndex, &screenX, &screenY);
            if (screenX >= 0x141) {
                screenX = 0x13F;
            }
            if (screenX < 0) {
                screenX = 0;
            }
            pan = FieldPositionalSfxScreenPan(screenX);
            func_8003A20C(slot2);
            func_80039F9C(soundId, slot2, volume, pan);
            break;
        }
    }
#else
    s32 i;
    s32 volume;
    s32 screenX;
    s32 screenY;
    s32 pan;
    s32 slot2;

    for (i = 0; i < 3; i++) {
        if (D_800AFE8A[i * 3] == 0xFFFF) {
            slot2 = i * 2;
            D_800AFE8A[i * 3] = soundId;
            D_800AFE88[i * 3] = actorIndex;
            func_80086078(distance, &volume, mode);
            FieldActorWorldToScreenPosition(actorIndex, &screenX, &screenY);
            if (screenX >= 0x141) {
                screenX = 0x13F;
            }
            if (screenX < 0) {
                screenX = 0;
            }
            pan = (screenX * 0x6666) >> 16;
            func_8003A20C(slot2);
            func_80039F9C(soundId, (s16)(i * 2), (s16)volume, pan);
            return;
        }
    }
#endif
}

/* Release the SPU voice channel bound to actor `actorIdx` in the 3-slot
 * actor->channel table (same table as func_80086470/func_800864F0):
 * on match, stop the voice (func_8003A20C(i*2)) and free the slot.
 * asm nonmatchings/main/misc8/func_800863E8.s. */
#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): field_sound_stop_emitter (src/decomp/src/field/field_event.c:500). */
#ifndef XM_S_field_sound_stop_emitter
#define XM_S_field_sound_stop_emitter
void xm_field_sound_stop_emitter(s32 id) __asm__("func_800863E8");
#endif
#ifndef XM_S_sound_stop_effect_on_channel
#define XM_S_sound_stop_effect_on_channel
void xm_sound_stop_effect_on_channel(s32 sound) __asm__("func_8003A20C");
#endif
void func_800863E8(s32 id) {
    s32 i;

    for (i = 0; i < 3; i++) {
        if (xm_field_sound_emitter_slots[i].actor == id) {
            xm_sound_stop_effect_on_channel(i * 2);
            xm_field_sound_emitter_slots[i].sound = 0xFFFF;
            xm_field_sound_emitter_slots[i].actor = 0xFFFF;
            return;
        }
    }
}
#endif
#else
void func_800863E8(s32 actorIdx) {
    s32 i;

    for (i = 0; i < 3; i++) {
        /* Retail: lhu slot vs the full 32-bit argument (no truncation). */
        if ((u32)D_800AFE88[i * 3] == (u32)actorIdx) {
            func_8003A20C(i * 2);
            D_800AFE8A[i * 3] = 0xFFFF;
            D_800AFE88[i * 3] = 0xFFFF;
            break;
        }
    }
}
#endif /* XENO_PC_PORT */

s32 func_80086470(s32 a0, s32 a1) {
#ifdef XENO_PC_PORT
    s32 i;
    if (a0 == -1) {
        return -1;
    }
    for (i = 0; i < 3; i++) {
        if (D_800AFE88[i * 3] == a1) {
            return i;
        }
    }
    return -1;
#else
    s32 i;
    s32 offset;

    if (a0 == -1) {
        return -1;
    }
    /* Byte offset into the 6-byte rows: an element index would be
     * strength-reduced into a pointer walk; retail keeps sym + offset. */
    for (i = 0, offset = 0; i < 3; i++, offset += 6) {
        if (*(u16*)((u8*)D_800AFE88 + offset) == a1) {
            return i;
        }
    }
    return -1;
#endif
}

void func_800864B4(void) {
#ifdef XENO_PC_PORT
    s32 i;
    for (i = 0; i < 3; i++) {
        D_800AFE88[i * 3] = 0xFFFF;
        D_800AFE8A[i * 3] = 0xFFFF;
    }
#else
    s32 i;
    s32 offset;

    for (i = 0; i < 3; i++) {
        offset = i * 6;
        *(u16*)((u8*)D_800AFE88 + offset) = 0xFFFF;
        *(u16*)((u8*)D_800AFE8A + offset) = 0xFFFF;
    }
#endif
}

extern s16 D_800B233C;
extern void func_8003A20C(s32);

// Field-transition sound-channel teardown: reset the 3-slot channel table
// (same shape as func_800864B4) and release any of the 4 low D_800B233C
// channel-mask bits that are still clear, one SPU voice-release call per bit.
#ifndef XENO_PC_PORT
/* func_800864F0: first loop is the func_800864B4 byte-offset giv; D_800B233C
 * is not gp-small in retail (address held in s1 across the calls) and is re-read
 * from memory each iteration, so it goes through an incomplete-array alias. */
extern u16 D_800B233C__a[] asm("D_800B233C");
extern void func_8003A20C(s32);

#endif
void func_800864F0(void) {
#ifdef XENO_PC_PORT
    s32 i;
    s16 flags;

    for (i = 0; i < 3; i++) {
        D_800AFE8A[i * 3] = 0xFFFF;
        D_800AFE88[i * 3] = 0xFFFF;
    }

    flags = D_800B233C;
    for (i = 0; i < 4; i++) {
        if ((flags & 1) == 0) {
            func_8003A20C(i * 2);
        }
        flags = (u16)flags >> 1;
    }
    D_800B233C = flags;
#else
    s32 i;
    s32 offset;

    for (i = 0; i < 3; i++) {
        offset = i * 6;
        *(u16*)((u8*)D_800AFE8A + offset) = 0xFFFF;
        *(u16*)((u8*)D_800AFE88 + offset) = 0xFFFF;
    }
    for (i = 0; i < 4; i++) {
        if (!(D_800B233C__a[0] & 1)) {
            func_8003A20C(i * 2);
        }
        D_800B233C__a[0] = D_800B233C__a[0] >> 1;
    }
#endif
}

extern s32 D_800ADBFC;
extern long FieldGetVec3Magnitude(long, long, long);
extern void func_8003A20C(s32);
extern void func_800860F0(s32, s32, s16, s32, s32);
extern void func_800862CC(s32, s32, s16, s32, s32);

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): field_sound_keep_nearest_emitters (src/decomp/src/field/field_event.c:557). */
#ifndef XM_T_xm_EmitterSlot
#define XM_T_xm_EmitterSlot
typedef struct xm_EmitterSlot {
    u16 actor;  
    u16 sound;  
    u16 unk4;
} xm_EmitterSlot;
#endif
#ifndef XM_S_field_sound_keep_nearest_emitters
#define XM_S_field_sound_keep_nearest_emitters
void xm_field_sound_keep_nearest_emitters(VECTOR *listener) __asm__("func_80086590");
#endif
#ifndef XM_S_field_compute_vector_length
#define XM_S_field_compute_vector_length
s32 xm_field_compute_vector_length(s32 dx, s32 dy, s32 dz) __asm__("FieldGetVec3Magnitude");
#endif
#ifndef XM_S_field_sound_find_emitter_slot
#define XM_S_field_sound_find_emitter_slot
s32 xm_field_sound_find_emitter_slot(s32 owner, s32 id) __asm__("func_80086470");
#endif
#ifndef XM_S_field_sound_start_emitter
#define XM_S_field_sound_start_emitter
void xm_field_sound_start_emitter(s32 sound, s32 volume, s32 unused, s32 distance, s32 actor) __asm__("func_800862CC");
#endif
void func_80086590(VECTOR *listener) {
    struct {
        s32 distance[4];
        s32 sound[4];
        s32 volume[4];
        s32 matched[4];
        s32 kept[4];
        s32 actor[4];
        SVECTOR offset[3];
    } near;
    SVECTOR *offset;
    xm_FieldActor *emitter;
    s32 length;
    s32 far;
    s32 slot;
    s32 i;

    for (i = 0; i < 3; i++) {
        near.distance[i] = 0xFFFF;
        near.sound[i] = -1;
        near.kept[i] = 0;
        near.matched[i] = 0;
        near.actor[i] = 0;
        near.volume[i] = 0;
    }
    for (i = 0; i < xm_field_event_actor_count; i++) {
        emitter = xm_field_view.components.descriptors[i].actor;
        if (emitter->sound_mode != 0xFF) {
            length = xm_field_compute_vector_length((listener->vx >> 16) - (emitter->position[0] >> 16),
                                   (listener->vy >> 16) - (emitter->position[1] >> 16),
                                   (listener->vz >> 16) - (emitter->position[2] >> 16));
            if (near.distance[0] < near.distance[1]) {
                far = 1;
                if (near.distance[1] < near.distance[2]) {
                    far = 2;
                }
            } else {
                far = (near.distance[0] < near.distance[2]) * 2;
            }
            if (length < near.distance[far]) {
                near.actor[far] = i;
                near.distance[far] = length;
                near.sound[far] = xm_field_view.components.descriptors[i].actor->sound;
                near.volume[far] = xm_field_view.components.descriptors[i].actor->sound_volume;
                (near.offset + far)->vx = (listener->vx >> 16) - (xm_field_view.components.descriptors[i].actor->position[0] >> 16);
                (near.offset + far)->vy = (listener->vy >> 16) - (xm_field_view.components.descriptors[i].actor->position[1] >> 16);
                (near.offset + far)->vz = (listener->vz >> 16) - (xm_field_view.components.descriptors[i].actor->position[2] >> 16);
            }
        } else {
            emitter->sound_mode = 0xFF;
        }
    }
    for (i = 0; i < 3; i++) {
        slot = xm_field_sound_find_emitter_slot(near.sound[i], near.actor[i]);
        if (slot != -1) {
            near.matched[slot] = 1;
            near.kept[i] = 1;
        }
    }
    for (i = 0; i < 3; i++) {
        if (near.matched[i] == 0 && xm_field_sound_emitter_slots[i].sound != 0xFFFF) {
            xm_sound_stop_effect_on_channel(i * 2);
            xm_field_sound_emitter_slots[i].sound = 0xFFFF;
            xm_field_sound_emitter_slots[i].actor = 0xFFFF;
        }
    }
    for (i = 0, offset = near.offset; i < 3; offset++, i++) {
        if (near.sound[i] != -1) {
            if (near.kept[i] == 1) {
                xm_field_sound_update_emitter_voices(near.sound[i], near.volume[i], offset->vx, near.distance[i], near.actor[i]);
            } else {
                xm_field_sound_start_emitter(near.sound[i], near.volume[i], offset->vx, near.distance[i], near.actor[i]);
            }
        }
    }
}
#endif
#else
void func_80086590(VECTOR* pos) {
    s32 dist[3];
    s32 soundId[3];
    s32 mode[3];
    s32 existing[3];
    s32 updateExisting[3];
    s32 actorIndex[3];
    s16 delta[3][4];
    s32 i;

    for (i = 0; i < 3; i++) {
        dist[i] = 0xFFFF;
        soundId[i] = -1;
        mode[i] = 0;
        existing[i] = 0;
        updateExisting[i] = 0;
        actorIndex[i] = 0;
    }

    for (i = 0; i < D_800ADBFC; i++) {
        ActorData* actor = (ActorData*)(uintptr_t)g_FieldActors[i].pActorData;
        s32 candidate;
        s32 candidateDist;

        if (*(u8*)((u8*)actor + 0x10D) == 0xFF) {
            *(u8*)((u8*)actor + 0x10D) = 0xFF;
            continue;
        }

        candidateDist = FieldGetVec3Magnitude(
            *(s16*)((u8*)pos + 0x02) - *(s16*)((u8*)actor + 0x22),
            *(s16*)((u8*)pos + 0x06) - *(s16*)((u8*)actor + 0x26),
            *(s16*)((u8*)pos + 0x0A) - *(s16*)((u8*)actor + 0x2A));

        if (dist[0] < dist[1]) {
            candidate = (dist[1] < dist[2]) ? 2 : 1;
        } else {
            candidate = (dist[0] < dist[2]) * 2;
        }

        if (candidateDist < dist[candidate]) {
            actorIndex[candidate] = i;
            dist[candidate] = candidateDist;
            soundId[candidate] = *(u16*)((u8*)actor + 0x10A);
            mode[candidate] = *(u8*)((u8*)actor + 0x10C);
            delta[candidate][0] = *(s16*)((u8*)pos + 0x02) - *(s16*)((u8*)actor + 0x22);
            delta[candidate][1] = *(s16*)((u8*)pos + 0x06) - *(s16*)((u8*)actor + 0x26);
            delta[candidate][2] = *(s16*)((u8*)pos + 0x0A) - *(s16*)((u8*)actor + 0x2A);
        }
    }

    for (i = 0; i < 3; i++) {
        s32 slot = func_80086470(soundId[i], actorIndex[i]);
        if (slot != -1) {
            existing[slot] = 1;
            updateExisting[i] = 1;
        }
    }

    for (i = 0; i < 3; i++) {
        if (existing[i] == 0 && D_800AFE8A[i * 3] != 0xFFFF) {
            func_8003A20C(i * 2);
            D_800AFE8A[i * 3] = 0xFFFF;
            D_800AFE88[i * 3] = 0xFFFF;
        }
    }

    for (i = 0; i < 3; i++) {
        if (soundId[i] != -1) {
            if (updateExisting[i] == 1) {
                func_800860F0(soundId[i], mode[i], delta[i][0], dist[i], actorIndex[i]);
            } else {
                func_800862CC(soundId[i], mode[i], delta[i][0], dist[i], actorIndex[i]);
            }
        }
    }
}
#endif /* XENO_PC_PORT */

extern VECTOR g_CameraEye;
extern VECTOR g_CameraAt;
extern s16 D_800B22E0;
extern s32 g_PlayerActorIndex;

void func_80086908(void) {
    switch (D_800B22E0) {
        case 0:
            func_80086590(&((ActorData*)(uintptr_t)g_FieldActors[g_PlayerActorIndex].pActorData)->position);
            return;
        case 1:
            func_80086590(&g_CameraEye);
            return;
        case 2:
            func_80086590(&g_CameraAt);
            return;
    }
}

void FieldScriptVM2Run(void) {
    char *script = (char *)g_FieldScriptVMCurScriptData;
    size_t vmIP = ++(g_FieldScriptVMCurActor->scriptInstructionPointer);
    /* asm 800869E8: lbu — the extended opcode is zero-extended. A signed
     * char index sent FE 8E-style opcodes to handlers2[-0x72], i.e. into
     * the tail of the first handler table. */
    u8 opcode = (u8)script[vmIP];

    ScriptVMHandler *handler = &g_FieldScriptVMHandlers2[opcode];
    (*handler)();
}

extern s16 D_800B22E8[];
extern s16 D_800B22EA[];
extern s16 D_800B22EC[];
extern s16 D_800B2300[];
extern s16 D_800B2302[];
extern s16 D_800B2304[];
extern s32 D_800B2318[];
extern s16 D_800B2324[];
extern s16 D_800B2326[];
extern s16 D_800B2328[];

#ifndef XENO_PC_PORT
/* func_80086A1C: the three outputs are D_800B223C[idx], [idx+3], [idx+6]; retail
 * addresses them from the D_800B2318 base register (s5-0xDC, -0xD6, -0xD0), so
 * the source indexed that same symbol with negative halfword offsets.  The step
 * D_800B2318[idx] is written inline (no pointer temp) so the idx*8 offset is
 * computed first. */
#endif
void func_80086A1C(s32 idx, u8* pSub) {
#ifdef XENO_PC_PORT
    s32 step = D_800B2318[idx];
    s32 dx, dy, dz, mag;
    s32 remainder;
    s16* pOutX = &D_800B2324[idx];
    s16* pOutY = &D_800B2326[idx];
    s16* pOutZ = &D_800B2328[idx];

    dx = (D_800B22E8[idx * 4] - D_800B2300[idx * 4]) << 16;
    dy = (D_800B22EA[idx * 4] - D_800B2302[idx * 4]) << 16;
    dz = (D_800B22EC[idx * 4] - D_800B2304[idx * 4]) << 16;

    /* Compute vector from sub-structure position delta */
    {
        s32 subDx = D_800B2324[idx * 4] - *(s16*)(pSub + 0x02);
        s32 subDy = D_800B2326[idx * 4] - *(s16*)(pSub + 0x06);
        s32 subDz = D_800B2328[idx * 4] - *(s16*)(pSub + 0x0A);
        mag = FieldGetVec3Magnitude(dx, dy, dz);
    }

    remainder = step - mag;
    *pOutX = (s16)((dx * remainder) >> 16);
    *pOutY = (s16)((dy * remainder) >> 16);
    *pOutZ = (s16)((dz * remainder) >> 16);
#else
    s32 dx, dy, dz, rem;

    dx = ((D_800B22E8[idx * 4] - D_800B2300[idx * 4]) << 16) / D_800B2318[idx];
    dy = ((D_800B22EA[idx * 4] - D_800B2302[idx * 4]) << 16) / D_800B2318[idx];
    dz = ((D_800B22EC[idx * 4] - D_800B2304[idx * 4]) << 16) / D_800B2318[idx];
    rem = D_800B2318[idx] - FieldGetVec3Magnitude(D_800B2324[idx * 4] - *(s16*)(pSub + 2),
                                                  D_800B2326[idx * 4] - *(s16*)(pSub + 6),
                                                  D_800B2328[idx * 4] - *(s16*)(pSub + 0xA));
    ((s16*)D_800B2318)[idx - 0x6E] = (dx * rem) >> 16;
    ((s16*)D_800B2318)[idx - 0x6B] = (dy * rem) >> 16;
    ((s16*)D_800B2318)[idx - 0x68] = (dz * rem) >> 16;
#endif
}

extern s16 D_800B22E2[];
extern void func_80086A1C(s32 idx, u8* pSub);

void func_80086BA8(void) {
    s32 i;
    for (i = 0; i < 3; i++) {
        s16 actorIdx = D_800B22E2[i];
        if (actorIdx != -1) {
            u32 actorAddr = (u32)(actorIdx * sizeof(FieldActor));
            u8* pFieldActor;
            u32 pSub;
            actorAddr += (u32)(uintptr_t)g_FieldActors;
            pFieldActor = (u8*)(uintptr_t)actorAddr;
            pSub = *(u32*)(pFieldActor + 0x4C);
            func_80086A1C(i, (u8*)(pSub + 0x20));
        }
    }
}

extern s32 D_800AFD1C;
extern u8 g_FieldDefaultParticleBanks[];
extern void FieldInitializeDefaultParticleBanks(s32 idx);
extern void FieldInitializeParticleBanks(s32 idx);

#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc8", func_80086C34);
#else
void func_80086C34(void) {
    u8 subOp = SCRIPT_READ_U8_REL(1);
    switch (subOp) {
        case 0:
            g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
            break;
        case 1: {
            /* Retail (field.bin 80086C88..80086D08): only argument 4 is kept;
             * the halfword at bank +0x00 (not +0x52) is set to 1; +0x74 is
             * first written with arg4 and then 0x20 when arg4 == 0x27, else 0x22. */
            s32 arg4;
            u16 v;
            (void)FieldScriptVMGetArgument(2);
            arg4 = FieldScriptVMGetArgument(4);
            (void)FieldScriptVMGetArgument(6);
            FieldInitializeDefaultParticleBanks(D_800AFD1C);
            *(u16*)(g_FieldDefaultParticleBanks + 0x2A) = 0x14;
            *(u16*)(g_FieldDefaultParticleBanks + 0x00) = 1;
            *(u16*)(g_FieldDefaultParticleBanks + 0x06) = 0x10;
            *(s16*)(g_FieldDefaultParticleBanks + 0x72) = 0;
            *(u16*)(g_FieldDefaultParticleBanks + 0x74) = (u16)arg4;
            if (arg4 == 0x27) {
                v = 0x20;
            } else {
                v = 0x22;
            }
            *(u16*)(g_FieldDefaultParticleBanks + 0x74) = v;
            *(u16*)(g_FieldDefaultParticleBanks + 0x04) = 0x1000;
            FieldInitializeParticleBanks(D_800AFD1C);
            g_FieldScriptVMCurActor->scriptInstructionPointer += 8;
            break;
        }
    }
}
#endif /* XENO_PC_PORT */

void func_80086D4C(void) {
    GameSoftReset();
    D_800B00C0 = 1;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 1;
}
