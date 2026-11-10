#include "common.h"
#include "main/game.h"
#include "field/main.h"
#include "field/actor.h"
#include "field/script_vm.h"
#include "field/effects.h"
#include "system/memory.h"
#include "system/archive.h"
#ifdef XENO_PC_PORT
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#else
/* <assert.h> is unavailable under the matching build's -nostdinc MIPS
 * preprocessor. The assert(0) below marks an unimplemented path in a function
 * not yet byte-matched, so a no-op assert compiles safely there. */
#define assert(x) ((void)0)
#endif
#include "system/debug.h"
#include "psyq/libgpu.h"
#include "psyq/libcd.h"

extern s32 D_800AFFEC;
extern s32 D_800AFD1C; // Current actor index
extern s32 D_800B00C0;

extern void func_800379C8(char*, ...);
extern char D_8006FD44; // "STACKERR ACT=%d\n"
u_short FieldScriptGetBytecodeOffset(int scriptIndex, int routineIndex);

// Store instruction pointer + 5 on stack
void func_800A1730(void) {
    if (g_FieldScriptVMCurActor->flags12C_0x6 != SCRIPT_MAX_STACK_SIZE) {
        g_FieldScriptVMCurActor->scriptPointersStack[g_FieldScriptVMCurActor->flags12C_0x6] = g_FieldScriptVMCurActor->scriptInstructionPointer + 5;
        g_FieldScriptVMCurActor->scriptInstructionPointer = FieldScriptVMGetInstructionArgument(1);
        g_FieldScriptVMCurActor->flags12C_0x6++;
        return;
    }

    // Error
    if (g_FieldSystemMode == 0) {
        func_800379C8(&D_8006FD44, D_800AFD1C);
    }
    D_800B00C0 = 1;
}

// Store instruction pointer + 3 on stack
void func_800A17F4(void) {
    if (g_FieldScriptVMCurActor->flags12C_0x6 != SCRIPT_MAX_STACK_SIZE) {
        g_FieldScriptVMCurActor->scriptPointersStack[g_FieldScriptVMCurActor->flags12C_0x6] = g_FieldScriptVMCurActor->scriptInstructionPointer + 3;
        g_FieldScriptVMCurActor->scriptInstructionPointer = FieldScriptVMGetInstructionArgument(1);
        g_FieldScriptVMCurActor->flags12C_0x6++;
        return;
    }
    
    // Error
    if (g_FieldSystemMode == 0) {
        func_800379C8(&D_8006FD44, D_800AFD1C);
    }
    D_800B00C0 = 1;
}

// Restore instruction pointer from stack
void func_800A18B8(void) {
    // Error, invalid stack value
    if (!g_FieldScriptVMCurActor->flags12C_0x6) {
        if (g_FieldSystemMode == 0) {
            func_800379C8(&D_8006FD44, D_800AFD1C);
        }
        g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].flags_0x12 = 0xFF;
        g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].scriptId = 0xFF;
        D_800AFFEC = 1;
        D_800B00C0 = 1;
        return;
    }
    
    g_FieldScriptVMCurActor->flags12C_0x6--;
    g_FieldScriptVMCurActor->scriptInstructionPointer = g_FieldScriptVMCurActor->scriptPointersStack[g_FieldScriptVMCurActor->flags12C_0x6];
}

void func_800A19B0(void) {
    int i;
    
    for (i = 0; i < ACTOR_MAX_NUM_SCRIPTS; i++) {
        g_FieldScriptVMCurActor->scripts[i].waitTimer = 0;
        g_FieldScriptVMCurActor->scripts[i].state = SCRIPT_STATE_IDLE;
        g_FieldScriptVMCurActor->scripts[i].flags_0x12 = 0xF;
        g_FieldScriptVMCurActor->scripts[i].currentIP = 0xFFFF;
        g_FieldScriptVMCurActor->scripts[i].isInUse = 0;
        g_FieldScriptVMCurActor->scripts[i].scriptId = 0xFF;
        g_FieldScriptVMCurActor->scripts[i].flags_0 = 0xFFFF;
        g_FieldScriptVMCurActor->scripts[i].flags_0x17 = 0x0;
    }

    g_FieldScriptVMCurActor->curScriptIndex = 0;
    g_FieldScriptVMCurActor->unkCF = 0;
    g_FieldScriptVMCurActor->dialogFlags = 0;
    D_800B00C0 = 1;
    g_FieldScriptVMCurActor->flags12C_0x6 = 0;
}

// Yield / Stop, but change IP conditionally
void func_800A1A8C(void) {
    int i;

    for (i = 0; i < ACTOR_MAX_NUM_SCRIPTS; i++) {
        if (g_FieldScriptVMCurActor->scripts[i].flags_0x12 == 7) {
            g_FieldScriptVMCurActor->scripts[i].currentIP = FieldScriptGetBytecodeOffset(D_800AFD1C, 1);
        }
    }

    g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].flags_0x12 = 0xFF;
    g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].scriptId = 0xFF;
    D_800B00C0 = 1;
}

// Yield / Stop Handler
void func_800A1B70(void) {
    g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].flags_0x12 = 0xFF;
    g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].scriptId = 0xFF;
    D_800AFFEC = 1;
    D_800B00C0 = 1;
}

void FieldScriptVMHandlerConditionalJmp(void) {
    int nValue2;
    int nValue1;
    int nArgumentsType;
    int nCondCheck;
    int nCondResult;
    int nConditionType;

    nValue2 = 0;
    nValue1 = 0;
    nArgumentsType = ((u_char*)g_FieldScriptVMCurScriptData)[g_FieldScriptVMCurActor->scriptInstructionPointer + 5] & 0xF0;
    switch (nArgumentsType) {
    case 0x0:
        nValue1 = FieldScriptVMGetVariableValue(FieldScriptVMGetInstructionArgument(1) & 0xFFFF);
        nValue2 = FieldScriptVMGetVariableValue(FieldScriptVMGetInstructionArgument(3) & 0xFFFF);
        if (FieldScriptVMGetVariableSign(FieldScriptVMGetInstructionArgument(1) & 0xFFFF) != FIELD_SCRIPT_VM_VAR_SIGNED) {
            nValue2 = (u_short)nValue2;
        } else {
            nValue2 = (short)nValue2;
        }
        break;
    case 0x40:
        nValue1 = FieldScriptVMGetVariableValue(FieldScriptVMGetInstructionArgument(1) & 0xFFFF);
        nValue2 = FieldScriptVMGetInstructionArgumentS16(3);
        if (FieldScriptVMGetVariableSign(FieldScriptVMGetInstructionArgument(1) & 0xFFFF) != FIELD_SCRIPT_VM_VAR_SIGNED) {
            nValue2 = (u_short)nValue2;
        }
        break;
    case 0x80:
        nValue1 = FieldScriptVMGetInstructionArgumentS16(1);
        nValue2 = FieldScriptVMGetVariableValue(FieldScriptVMGetInstructionArgument(3) & 0xFFFF);
        if (FieldScriptVMGetVariableSign(FieldScriptVMGetInstructionArgument(3) & 0xFFFF) != FIELD_SCRIPT_VM_VAR_SIGNED) {
            nValue1 = (u_short)nValue1;
        }
        break;
    case 0xC0:
        nValue1 = FieldScriptVMGetInstructionArgumentS16(1);
        nValue2 = FieldScriptVMGetInstructionArgumentS16(3);
        break;
    }
    
    nCondResult = 0;
    nConditionType = ((u_char*)g_FieldScriptVMCurScriptData)[g_FieldScriptVMCurActor->scriptInstructionPointer + 5] & 0xF;
    switch (nConditionType) {
        case FIELD_SCRIPT_VM_COND_EQUAL:
            if (nValue1 == nValue2) nCondResult++;
            break;
        case FIELD_SCRIPT_VM_COND_NOT_EQUAL2:
            if (nValue1 != nValue2) nCondResult++;
            break;
        case FIELD_SCRIPT_VM_COND_LT:
            nCondCheck = nValue2 < nValue1;
            if (nCondCheck) nCondResult++;
            break;
        case FIELD_SCRIPT_VM_COND_LT2:
            nCondCheck = nValue1 < nValue2;
            if (nCondCheck) nCondResult++;
            break;
        case FIELD_SCRIPT_VM_COND_GTE:
            if (nValue1 >= nValue2) nCondResult++;
            break;
        case FIELD_SCRIPT_VM_COND_GTE2:
            if (nValue2 >= nValue1) nCondResult++;
            break;
        case FIELD_SCRIPT_VM_COND_AND:
            nCondCheck = nValue1 & nValue2;
            if (nCondCheck) nCondResult++;
            break;
        case FIELD_SCRIPT_VM_COND_NOT_EQUAL:
            if (nValue1 != nValue2) nCondResult++;
            break;
        case FIELD_SCRIPT_VM_COND_OR:
            nCondCheck = nValue1 | nValue2;
            if (nCondCheck) nCondResult++;
            break;
        case FIELD_SCRIPT_VM_COND_AND2:
            nCondCheck = nValue1 & nValue2;
            if (nCondCheck) nCondResult++;
            break;
        case FIELD_SCRIPT_VM_COND_NAND:
            nCondCheck = ~nValue1 & nValue2;
            if (nCondCheck) nCondResult++;
            break;
    }
    
    if (nCondResult == 1) {
        g_FieldScriptVMCurActor->scriptInstructionPointer = g_FieldScriptVMCurActor->scriptInstructionPointer + 8;
    } else {
        g_FieldScriptVMCurActor->scriptInstructionPointer = FieldScriptVMGetInstructionArgument(6);
    }
}

void FieldScriptVMHandlerJmp(void) {
    g_FieldScriptVMCurActor->scriptInstructionPointer = FieldScriptVMGetInstructionArgument(1);
}

void func_800A1E9C(void) {
    g_FieldScriptMaxInstructionCount += 0x20;
    g_FieldScriptVMCurActor->scriptInstructionPointer++;
}

#ifdef XENO_PC_PORT
/* DIAGNOSTIC / TEST TOOLING -- XENO_VM_TRACE=<actor index>|all.
 * Prints one line per DISTINCT instruction pointer the VM dispatches for the
 * selected actor(s): the base opcode byte, and for the 0xFE prefix the
 * sub-opcode and its first two operand bytes.  Wait opcodes re-dispatch the
 * same ip every frame and print once, so the output is the executed
 * instruction sequence, not a per-frame flood.  Capped per actor so a hot
 * script loop cannot fill a log.
 *
 * Three refinements make a STALLED script readable, which first-seen alone
 * cannot do -- a loop prints its body once and then goes silent, so the cycle
 * it is spinning in is invisible:
 *   XENO_VM_TRACE_REPEAT=1   print EVERY dispatch, not just first-seen, so the
 *                            repeating cycle and its branch decisions show up.
 *   XENO_VM_TRACE_MAX=<n>    per-actor line cap (default 4000); repeat mode
 *                            needs a bigger budget to cover several cycles.
 *   XENO_VM_TRACE_FIELD=<n>  only trace while that field is loaded, so the
 *                            budget is not burned on the boot/prologue fields
 *                            before the one under investigation.
 * A monotonic sequence number is printed so lines from different actors can be
 * interleaved back into dispatch order.
 * Removal: delete this function and its call in FieldScriptVMRun. */
static void PcPort_FieldVmTrace(u_short ip, u_char op) {
    static int s_mode = -1;        /* -1 unparsed, -2 off, -3 all, else actor */
    static int s_repeat;           /* 1 = print every dispatch (no dedup) */
    static int s_max;              /* per-actor line cap */
    static int s_field;            /* -1 = any field, else only this field */
    static unsigned s_seq;
    static u_char* s_seen[256];    /* per-actor first-seen bitset, 64K ips */
    static int s_lines[256];
    static const u_char* s_seenScript[256];
    extern s32 g_GameSceneMapNum;
    const u_char* script;
    int actor;

    if (s_mode == -1) {
        const char* e = getenv("XENO_VM_TRACE");
        const char* r = getenv("XENO_VM_TRACE_REPEAT");
        const char* m = getenv("XENO_VM_TRACE_MAX");
        const char* f = getenv("XENO_VM_TRACE_FIELD");
        s_repeat = (r != NULL && r[0] != '\0' && r[0] != '0');
        s_max = (m != NULL && m[0] != '\0') ? atoi(m) : 4000;
        s_field = (f != NULL && f[0] != '\0') ? atoi(f) : -1;
        if (e == NULL || e[0] == '\0') {
            s_mode = -2;
        } else if (e[0] == 'a') {
            s_mode = -3;
        } else {
            s_mode = atoi(e) & 0xFF;
        }
    }
    if (s_mode == -2) {
        return;
    }
    if (s_field >= 0 && (int)g_GameSceneMapNum != s_field) {
        return;
    }
    actor = D_800AFD1C & 0xFF;
    if (s_mode >= 0 && actor != s_mode) {
        return;
    }
    script = (const u_char*)g_FieldScriptVMCurScriptData;
    /* A new script buffer (map change) resets the actor's first-seen set. */
    if (s_seen[actor] == NULL || s_seenScript[actor] != script) {
        if (s_seen[actor] == NULL) {
            s_seen[actor] = calloc(0x10000 / 8, 1);
            if (s_seen[actor] == NULL) {
                return;
            }
        } else {
            int i;
            for (i = 0; i < 0x10000 / 8; i++) {
                s_seen[actor][i] = 0;
            }
        }
        s_seenScript[actor] = script;
        s_lines[actor] = 0;
    }
    if (s_lines[actor] >= s_max) {
        return;
    }
    if (!s_repeat && (s_seen[actor][ip >> 3] & (1u << (ip & 7)))) {
        return;
    }
    s_seen[actor][ip >> 3] |= (u_char)(1u << (ip & 7));
    s_lines[actor]++;
    s_seq++;
    if (op == 0xFE) {
        printf("[vm-trace] #%u actor=%d ip=%u op=FE%02X args=%02x %02x %02x %02x\n",
               s_seq, actor, (unsigned)ip, script[ip + 1], script[ip + 2],
               script[ip + 3], script[ip + 4], script[ip + 5]);
    } else {
        printf("[vm-trace] #%u actor=%d ip=%u op=%02X args=%02x %02x %02x %02x\n",
               s_seq, actor, (unsigned)ip, op, script[ip + 1], script[ip + 2],
               script[ip + 3], script[ip + 4]);
    }
    fflush(stdout);
}
#endif

void FieldScriptVMRun(int maxInstructionCount) {
    int nInstructionCount;
    u_short nInstructionPointer;
    u_char nHandlerIndex;

    D_800B00C0 = 0;
    g_FieldScriptMaxInstructionCount = maxInstructionCount;

    for (nInstructionCount = 0; nInstructionCount < g_FieldScriptMaxInstructionCount; nInstructionCount++) {
        /* TODO: delete this asm() when we hit 100% matching.
         *
         * The current working theory is that there was some debugging code injected here, most likely a break
         * instruction, and they just changed the define to an empty string for the release build. This caused
         * the loop optimization for this function to break, which is why you see a noop in the jump's delay
         * slot. From all testing done with code structure and compiler arguments, this appears to be the only
         * way to make the function match.
         */
        asm(ASM_BREAKPOINT);
        if (nInstructionCount > 0x400) {
            if (g_FieldSystemMode == SYSTEM_PC_HARDDRIVE) {
                func_800379C8(&D_8006FD84, D_800AFD1C); // Error printing
            }
            return;
        }
        nInstructionPointer = g_FieldScriptVMCurActor->scriptInstructionPointer;
        nHandlerIndex = ((u_char *)g_FieldScriptVMCurScriptData)[nInstructionPointer];
#ifdef XENO_PC_PORT
        PcPort_FieldVmTrace(nInstructionPointer, nHandlerIndex);
#endif
        g_FieldScriptVMHandlers[nHandlerIndex]();

        if (D_800AFFEC == 0) {
            g_FieldScriptMaxInstructionCount = 0xFFFF;
        }

        // Various exit checks
        if ((D_800ADB1C != 0 && (D_800ADBE0 == 0 ||  D_800ADBE4 == 0 ||  D_800ADBEC == 0))) {
            return;
        }

        if (D_800B00C0 == 1 && D_800AFFEC == D_800B00C0) {
            return;
        }

    }

}

extern s32 D_800ADB68;
extern s32 D_800ADB74;
extern s32 D_800ADBFC;
extern s32 D_800ADBE0;
extern s32 D_800ADBE4;
extern s32 D_800ADBEC;
extern s32 D_800ADB1C;
extern s32 D_800C4268;
extern s32 D_8005A444[];
extern u8 D_800B21CC;
extern FieldActor* D_800B06B8;

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): field_event_run_all_actors (src/decomp/src/field/field_event.c:10572). */
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
#ifndef XM_S_field_event_run_all_actors
#define XM_S_field_event_run_all_actors
s32 xm_field_event_run_all_actors(void) __asm__("func_800A2030");
#endif
#ifndef XM_S_field_movie_mode
#define XM_S_field_movie_mode
extern s32 xm_field_movie_mode __asm__("D_800ADB74");
#endif
#ifndef XM_S_field_event_actor_count
#define XM_S_field_event_actor_count
extern s32 xm_field_event_actor_count __asm__("D_800ADBFC");
#endif
#ifndef XM_S_field_player_control_polled
#define XM_S_field_player_control_polled
extern s32 xm_field_player_control_polled __asm__("D_800ADB68");
#endif
#ifndef XM_S_field_dialogue_pass_open_count
#define XM_S_field_dialogue_pass_open_count
extern s32 xm_field_dialogue_pass_open_count __asm__("D_800C4268");
#endif
#ifndef XM_S_field_view
#define XM_S_field_view
extern xm_FieldView xm_field_view __asm__("g_CameraEye");
#endif
#ifndef XM_S_field_event_runs_per_frame
#define XM_S_field_event_runs_per_frame
extern s32 xm_field_event_runs_per_frame __asm__("D_800ADB1C");
#endif
#ifndef XM_S_field_scripted_battle_not_requested
#define XM_S_field_scripted_battle_not_requested
extern s32 xm_field_scripted_battle_not_requested __asm__("D_800ADBE0");
#endif
#ifndef XM_S_field_worldmap_exit_not_requested
#define XM_S_field_worldmap_exit_not_requested
extern s32 xm_field_worldmap_exit_not_requested __asm__("D_800ADBE4");
#endif
#ifndef XM_S_field_map_change_not_requested
#define XM_S_field_map_change_not_requested
extern s32 xm_field_map_change_not_requested __asm__("D_800ADBEC");
#endif
#ifndef XM_S_field_current_event_descriptor
#define XM_S_field_current_event_descriptor
extern xm_FieldDescriptor *xm_field_current_event_descriptor __asm__("D_800B06B8");
#endif
#ifndef XM_S_field_current_event_actor_index
#define XM_S_field_current_event_actor_index
extern s32 xm_field_current_event_actor_index __asm__("D_800AFD1C");
#endif
#ifndef XM_S_field_current_event_actor
#define XM_S_field_current_event_actor
extern xm_FieldActor *xm_field_current_event_actor __asm__("g_FieldScriptVMCurActor");
#endif
#ifndef XM_S_field_work
#define XM_S_field_work
extern xm_FieldWork xm_field_work __asm__("g_FieldEffects");
#endif
#ifndef XM_S_mode_party_actors
#define XM_S_mode_party_actors
extern s32 xm_mode_party_actors[3] __asm__("D_8005A444");
#endif
#ifndef XM_S_field_event_get_entry_pc
#define XM_S_field_event_get_entry_pc
s32 xm_field_event_get_entry_pc(s32 actor, s32 event) __asm__("FieldScriptGetBytecodeOffset");
#endif
#ifndef XM_S_field_event_yield_ends_run
#define XM_S_field_event_yield_ends_run
extern s32 xm_field_event_yield_ends_run __asm__("D_800AFFEC");
#endif
#ifndef XM_S_field_event_run_instructions
#define XM_S_field_event_run_instructions
s32 xm_field_event_run_instructions(s32 limit) __asm__("FieldScriptVMRun");
#endif
s32 func_800A2030(void) {
    xm_FieldDescriptor *descriptor;
    xm_FieldActor *actor;
    s32 count;
    s32 index;
    s32 i;
    s32 priority;

    if (xm_field_movie_mode == 1) {
        count = 1;
    } else {
        count = xm_field_event_actor_count;
    }
    xm_field_player_control_polled = 0;
    xm_field_dialogue_pass_open_count = 0;
    for (index = 0; index < count; index++) {
        if (!(xm_field_view.components.descriptors[index].flags & 0xF00)
            || (xm_field_view.components.descriptors[index].actor->layer_flags & 0x100000)) {
            continue;
        }
        if (xm_field_event_runs_per_frame != 0 && (xm_field_scripted_battle_not_requested == 0 || xm_field_worldmap_exit_not_requested == 0 || xm_field_map_change_not_requested == 0)) {
            return;
        }
        descriptor = &xm_field_view.components.descriptors[index];
        xm_field_current_event_descriptor = descriptor;
        actor = descriptor->actor;
        actor->flags &= ~0x1000000;
        xm_field_current_event_actor_index = index;
        xm_field_current_event_actor = actor;
        priority = 0xF;
        if (xm_field_work.party_processing_mode != 0) {
            for (i = 0; i < 3; i++) {
                if (xm_mode_party_actors[i] != 0xFF && xm_mode_party_actors[i] == index) {
                    goto next;
                }
            }
        }
        for (i = 0; i < 8; i++) {
            if (priority >= xm_field_current_event_actor->slots[i].priority) {
                priority = xm_field_current_event_actor->slots[i].priority;
                xm_field_current_event_actor->slot = i;
            }
        }
        if (priority == 0xF) {
            xm_field_current_event_actor->slots[0].resume_pc = xm_field_event_get_entry_pc(index, 1);
            xm_field_current_event_actor->slots[0].priority = 7;
            xm_field_current_event_actor->slot = 0;
        }
        xm_field_current_event_actor->pc = xm_field_current_event_actor->slots[xm_field_current_event_actor->slot].resume_pc;
        xm_field_event_yield_ends_run = 1;
        if (!(xm_field_current_event_actor->flags & 1)) {
            xm_field_event_run_instructions(8);
        }
        xm_field_current_event_actor->slots[xm_field_current_event_actor->slot].resume_pc = xm_field_current_event_actor->pc;
    next:;
    }
}
#endif
#else
void func_800A2030(void) {
    s32 numActors;
    s32 actorIndex;

    if (D_800ADB74 == 1) {
        numActors = 1;
    } else {
        numActors = D_800ADBFC;
    }

    D_800ADB68 = 0;
    D_800C4268 = 0;

    for (actorIndex = 0; actorIndex < numActors; actorIndex++) {
        FieldActor* fieldActor = &g_FieldActors[actorIndex];
        ActorData* actor = (ActorData*)(uintptr_t)fieldActor->pActorData;
        s32 slot;
        s32 bestPriority;

        if ((fieldActor->status & 0x0F00) == 0) {
            continue;
        }

        if (*(u32*)((u8*)actor + 0x04) & 0x00100000) {
            continue;
        }

        if (D_800ADB1C != 0) {
            if (D_800ADBE0 == 0 || D_800ADBE4 == 0 || D_800ADBEC == 0) {
                return;
            }
        }

        /* asm 800A2124-800A2138: lw/and/sw 0x0(pActorData) - bit 24 of the
         * +0x0 scriptFlags word (not the +0x4 flags word). */
        actor->scriptFlags.flags &= 0xFEFFFFFF;
        D_800B06B8 = fieldActor;
        D_800AFD1C = actorIndex;
        g_FieldScriptVMCurActor = actor;

        if (D_800B21CC != 0) {
            s32 i;
            s32 shouldSkip = 0;

            for (i = 0; i < 3; i++) {
                if (D_8005A444[i] != 0xFF && D_8005A444[i] == actorIndex) {
                    shouldSkip = 1;
                    break;
                }
            }
            if (shouldSkip) {
                continue;
            }
        }

        bestPriority = 0xF;
        for (slot = 0; slot < 8; slot++) {
            u32 scriptWord = *(u32*)((u8*)g_FieldScriptVMCurActor + 0x90 + slot * 8);
            s32 priority = (scriptWord >> 18) & 0xF;

            if (priority <= bestPriority) {
                bestPriority = priority;
                *(u8*)((u8*)g_FieldScriptVMCurActor + 0xCE) = slot;
            }
        }

        if (bestPriority == 0xF) {
            u16 ip = FieldScriptGetBytecodeOffset(actorIndex, 1);

            *(u16*)((u8*)g_FieldScriptVMCurActor + 0x8C) = ip;
            *(u8*)((u8*)g_FieldScriptVMCurActor + 0xCE) = 0;
            *(u32*)((u8*)g_FieldScriptVMCurActor + 0x90) =
                (*(u32*)((u8*)g_FieldScriptVMCurActor + 0x90) & 0xFFC3FFFF) | 0x001C0000;
        }

        {
            u8 curScript = *(u8*)((u8*)g_FieldScriptVMCurActor + 0xCE);
            u16 ip = *(u16*)((u8*)g_FieldScriptVMCurActor + 0x8C + curScript * 8);

            D_800AFFEC = 1;
            *(u16*)((u8*)g_FieldScriptVMCurActor + 0xCC) = ip;

            /* asm 800A2228-800A2250: gate is ActorData+0x0 bit0 (scriptFlags_0x0 /
             * isDisabled), NOT flags+0x4. Clear's OP_27 sets this bit so ambient
             * routine-1 auto-restart cannot keep running the screen-tint pulse. */
            if (!g_FieldScriptVMCurActor->scriptFlags.fields.scriptFlags_0x0) {
                FieldScriptVMRun(8);
            }

            curScript = *(u8*)((u8*)g_FieldScriptVMCurActor + 0xCE);
            *(u16*)((u8*)g_FieldScriptVMCurActor + 0x8C + curScript * 8) =
                *(u16*)((u8*)g_FieldScriptVMCurActor + 0xCC);
        }
    }
}
#endif /* XENO_PC_PORT */

// Changes current actor to the top of the field actor array and runs a script routine on it
extern FieldActor* D_800B06B8;
extern s32 D_800ADB1C;
void func_800A22AC(int scriptRoutineIndex) {
    int i;
    ActorData* pNewActor;

    // Change current actor to the first actor in the list
    D_800B06B8 = g_FieldActors;
    g_FieldScriptVMCurActor = (ActorData*)(uintptr_t)D_800B06B8->pActorData;
    pNewActor = HeapAlloc(0x138, 0x1);
    *pNewActor = *(ActorData*)(uintptr_t)D_800B06B8->pActorData;

    // Reset all event slots
    for (i = 0; i < ACTOR_MAX_NUM_SCRIPTS; i++) {
        g_FieldScriptVMCurActor->scripts[i].waitTimer = 0;
        g_FieldScriptVMCurActor->scripts[i].state = SCRIPT_STATE_IDLE;
        g_FieldScriptVMCurActor->scripts[i].flags_0x12 = 0xF;
        g_FieldScriptVMCurActor->scripts[i].currentIP = 0xFFFF;
        g_FieldScriptVMCurActor->scripts[i].isInUse = 0x0;
        g_FieldScriptVMCurActor->scripts[i].scriptId = 0xFF;
        g_FieldScriptVMCurActor->scripts[i].flags_0 = 0xFFFF;
        g_FieldScriptVMCurActor->scripts[i].flags_0x17 = 0x0;
    }
    
    D_800AFD1C = 0;
    D_800ADB1C = 0;
    D_800AFFEC = 0;

    // Run entry point routine
    g_FieldScriptVMCurActor->scriptInstructionPointer = FieldScriptGetBytecodeOffset(0, scriptRoutineIndex);
    FieldScriptVMRun(0xFFFF);
    D_800ADB1C = 1;

    // Running VM bytecode will likely have changed some fields, so they are copied back into the top
    // of the actor array.
    *(ActorData*)(uintptr_t)D_800B06B8->pActorData = *pNewActor;
    HeapFree(pNewActor);
}


extern s32 D_800ADB8C;
extern void func_800A22AC(int scriptRoutineIndex);
#ifdef XENO_PC_PORT
extern s16 D_8006BE2C[];
extern s32 D_800B2268;
extern s32 D_8005A444[];
extern s32 D_8006F990[];
extern void func_800AD4D4(s32 partySlot);
extern void func_800ACFD0(s32 partySlot);
extern void func_800821F4(void* sprite, s16 animation, void* actor);
extern void FieldParticleActorStop(s32 actorIndex, s32 mode);
extern void func_8009FEE4(s32 partySlot);
extern void func_800A0524(s32 source, s32 destination);

/* Retail 800ACFD0..800AD4D4. Unlike boarding, disembarking selects the
 * on-foot actor through the party table, not the current script actor. */
void func_800ACFD0(s32 partySlot) {
    FieldActor* party;
    FieldActor* gear;
    u8* partyData;
    u8* data;
    u32* destination;
    u32 sprite;

    ((u8*)g_pGameState)[0x22b1 + partySlot] = 0;
    party = &g_FieldActors[D_8006F990[partySlot]];
    gear = &g_FieldActors[D_8005A444[partySlot]];
    sprite = party->pSpriteData;
    party->pSpriteData = gear->pSpriteData;
    gear->pSpriteData = sprite;
    party->status = (((u16)party->status & 0xf07fu) | 0x200u) & 0xffdfu;
    partyData = (u8*)(uintptr_t)party->pActorData;
    *(u32*)partyData &= ~1u;
    func_800A0524(D_8006F990[partySlot], D_8005A444[partySlot]);

    /* The copy routine is a callback boundary: reload the actor table. */
    gear = &g_FieldActors[D_8005A444[partySlot]];
    data = (u8*)(uintptr_t)gear->pActorData;
    destination = (u32*)(uintptr_t)gear->pSpriteData;
    destination[0] = *(u32*)(data + 0x20);
    destination[1] = *(u32*)(data + 0x24);
    destination[2] = *(u32*)(data + 0x28);
    *(u32*)data = (*(u32*)data | 0x400u) & ~0x300u;
    partyData = (u8*)(uintptr_t)g_FieldActors[D_8006F990[partySlot]].pActorData;
    *(u32*)partyData &= ~0x1800u;
    *(u32*)data &= ~0x1800u;
    *(u16*)(partyData + 0x108) = *(u16*)(data + 0x108);
    *(u16*)(partyData + 0x106) = *(u16*)(data + 0x106);
    *(u16*)(partyData + 0xe8) = *(u16*)(partyData + 0xe6);
    *(u16*)(data + 0xe8) = *(u16*)(data + 0xe6);
    party = &g_FieldActors[D_8006F990[partySlot]];
    func_800821F4((void*)(uintptr_t)party->pSpriteData, 6, party);
    gear = &g_FieldActors[D_8005A444[partySlot]];
    data = (u8*)(uintptr_t)gear->pActorData;
    func_800821F4((void*)(uintptr_t)gear->pSpriteData,
                 *(s16*)(data + 0xe6), gear);
    func_8009FEE4(partySlot);
    FieldParticleActorStop(D_8006F990[partySlot], 0);
}

/* Retail 800AD4D4..800AD898. Boarding swaps sprite ownership with the
 * selected gear actor, preserves its position and updates both actor states.
 * Keep the two animation calls and subsequent particle/save updates ordered. */
void func_800AD4D4(s32 partySlot) {
    FieldActor* current = &g_FieldActors[D_800AFD1C];
    FieldActor* gear = &g_FieldActors[D_8005A444[partySlot]];
    u32 sprite = current->pSpriteData;
    u8* data;
    u8* partyData;
    u32* destination;
    current->pSpriteData = gear->pSpriteData;
    gear->pSpriteData = sprite;
    data = (u8*)(uintptr_t)gear->pActorData;
    destination = (u32*)(uintptr_t)gear->pSpriteData;
    destination[0] = *(u32*)(data + 0x20);
    destination[1] = *(u32*)(data + 0x24);
    destination[2] = *(u32*)(data + 0x28);
    current->status = (u16)current->status | 0x20;
    *(u32*)data = (*(u32*)data | 0x200u) & ~0x500u;
    ((u8*)g_pGameState)[0x22b1 + partySlot] = 1;
    partyData = (u8*)(uintptr_t)g_FieldActors[D_8006F990[partySlot]].pActorData;
    *(u32*)partyData &= ~0x1800u;
    *(u32*)data &= ~0x1800u;
    *(u16*)(partyData + 0xe8) = *(u16*)(partyData + 0xe6);
    *(u16*)(data + 0xe8) = *(u16*)(data + 0xe6);
    {
        FieldActor* actor = &g_FieldActors[D_8006F990[partySlot]];
        u8* actorData = (u8*)(uintptr_t)actor->pActorData;
        func_800821F4((void*)(uintptr_t)actor->pSpriteData,
                     *(s16*)(actorData + 0xe6), actor);
    }
    /* Retail reloads the actor table after the first animation callback. */
    {
        FieldActor* actor = &g_FieldActors[D_8005A444[partySlot]];
        u8* actorData = (u8*)(uintptr_t)actor->pActorData;
        func_800821F4((void*)(uintptr_t)actor->pSpriteData,
                     *(s16*)(actorData + 0xe6), actor);
    }
    FieldParticleActorStop(D_8006F990[partySlot], 0);
    func_8009FEE4(partySlot);
}

/* Retail 0x800AD978 reconciles each changed party slot with its field actor.
 * The boolean comparison is the two-way retail branch at
 * 0x800A8F78-0x800A8FB4: unequal presence calls the add/swap path, while an
 * equal state calls the removal/refresh path. */
void func_800AD978(s32 mode) {
    s32 i;

    if (!D_800B2268) {
        return;
    }

    for (i = 0; i < 3; i++) {
        s32 present;

        if (D_8005A444[i] == 0xFF || D_8006BE2C[i] != 1) {
            continue;
        }

        D_800AFD1C = D_8006F990[i];
        present = ((u8*)g_pGameState)[0x22B1 + i] != 0;
        if (present != (mode != 0)) {
            func_800AD4D4(i);
        } else {
            func_800ACFD0(i);
        }
    }
}

/* Retail 0x800ACE24 converts the three pre-script party snapshots into
 * changed/not-changed flags, then refreshes party presentation state. */
void func_800ACE24(void) {
    s32 i;

    for (i = 0; i < 3; i++) {
        D_8006BE2C[i] =
            D_8006BE2C[i] == ((u8*)g_pGameState)[0x22B1 + i] ? 0 : 1;
    }
    func_800AD978(0);
}
#else
extern void func_800ACE24(void);
#endif

void func_800A2488(void) {
    D_800ADB8C = 1;
    func_800A22AC(3);
    func_800ACE24();
    D_800ADB8C = 0;
}

extern s32 g_GamePartySkinsInitialized;
extern s32 D_800ADBFC;
extern s32 D_800AFC74;
extern s32 D_800B2268;
extern s32 D_8005A444[];
extern s32 D_8006F990[];
extern void* g_PartyDataBuffers[];
extern void func_800A3474(void);
extern void func_8002303C(void*, s32, s32);
extern void func_80076AC0(s32, s32, void*, s32, s32, s32, s32);

extern s32 D_800B2264;
extern u8 D_800B21D2;
extern s32 D_800B21BC;
extern s32 D_800B21C0;
extern s32 D_800B21C4;
extern s32 g_FieldNumActors;
extern s32 g_PlayerActorIndex;
extern void func_800AD898(void);
extern void func_801E8330(s32 slot, s32 arg1, s32 objId);

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): field_finish_return_to_field (src/decomp/src/field/field_event.c:10674). */
#ifndef XM_T_PIECE_DRIFT_TOTAL
#define XM_T_PIECE_DRIFT_TOTAL
#define PIECE_DRIFT_TOTAL ((s32 *)xm_field_work.unk21BC)
#endif
#ifndef XM_S_field_finish_return_to_field
#define XM_S_field_finish_return_to_field
void xm_field_finish_return_to_field(void) __asm__("func_800A24C4");
#endif
#ifndef XM_S_mode_field_return_pending
#define XM_S_mode_field_return_pending
extern s32 xm_mode_field_return_pending __asm__("g_GamePartySkinsInitialized");
#endif
#ifndef XM_S_field_event_run_actor0_event
#define XM_S_field_event_run_actor0_event
void xm_field_event_run_actor0_event(s32 event) __asm__("func_800A22AC");
#endif
#ifndef XM_S_field_party_set_gear_rider_flags
#define XM_S_field_party_set_gear_rider_flags
void xm_field_party_set_gear_rider_flags(void) __asm__("func_800AD898");
#endif
#ifndef XM_S_gear_model_select_and_call_entry
#define XM_S_gear_model_select_and_call_entry
void xm_gear_model_select_and_call_entry(u16 index, u16 mask, s32 entry) __asm__("func_801E8330");
#endif
void func_800A24C4(void) {
    u8 unused[0x10]; 
    s32 i;

    if (xm_mode_field_return_pending != 0) {
        xm_field_event_run_actor0_event(2);
        xm_field_party_set_gear_rider_flags();
        for (i = 0; i < xm_field_work.unk2264; i++) {
            xm_gear_model_select_and_call_entry((u16)i, 0, xm_field_work.unk21E4[i]);
        }
        i = xm_field_view.components.descriptors[xm_field_work.controlled].actor->ridden_actor;
        if (i != 0xFF) {
            xm_field_view.components.descriptors[xm_field_work.controlled].actor->position[1] -= 8;
        }
        for (i = 0; i < xm_field_view.components.descriptor_count; i++) {
            if (i < xm_field_event_actor_count) {
                if (xm_field_view.components.descriptors[i].actor->state.word & 3) {
                    continue;
                }
            } else if ((xm_field_work.piece_drift_mode & 0x7F) == 0) {
                xm_field_view.components.descriptors[i].matrix.t[0] += PIECE_DRIFT_TOTAL[0];
                xm_field_view.components.descriptors[i].matrix.t[1] += PIECE_DRIFT_TOTAL[1];
                xm_field_view.components.descriptors[i].matrix.t[2] += PIECE_DRIFT_TOTAL[2];
            }
            if ((xm_field_work.piece_drift_mode & 0x7F) == 1) {
                xm_field_view.components.descriptors[i].matrix.t[0] += PIECE_DRIFT_TOTAL[0];
                xm_field_view.components.descriptors[i].matrix.t[1] += PIECE_DRIFT_TOTAL[1];
                xm_field_view.components.descriptors[i].matrix.t[2] += PIECE_DRIFT_TOTAL[2];
            }
        }
    }
}
#endif
#else
void func_800A24C4(void) {
    u8* playerData;
    s32 i;

    if (g_GamePartySkinsInitialized == 0) {
        return;
    }

    /* asm 800A24E4-800A26F8: the party-skin per-frame update. */
    func_800A22AC(2);
    func_800AD898();

    /* Refresh the registered object-slot sprites through the field archive
     * 0x6B9 native-owner entry func_801E8330. */
    for (i = 0; i < D_800B2264; i++) {
        func_801E8330(i & 0xFFFF, 0,
                      *(s16*)((u8*)&D_800B2264 - 0x80 + i * 2));
    }

    playerData = (u8*)(uintptr_t)
        *(u32*)((u8*)g_FieldActors + g_PlayerActorIndex * 0x5C + 0x4C);
    if (*(u8*)(playerData + 0x74) != 0xFF) {
        /* asm 800A257C: addiu/sw wraps (u32, no signed-overflow UB). */
        *(u32*)(playerData + 0x24) -= 8;
    }

    /* Apply the global scroll-drift accumulators (D_800B21BC/C0/C4, the
     * func_800748E8 counters), asm 800A25AC-800A26F4.  Mode byte
     * D_800B21D2 & 0x7F:
     *  - i < D_800ADBFC (real actor): skipped when (+0x12C & 3) != 0, else
     *    ONLY the mode==1 block runs (800A25E8 branches straight to 800A266C);
     *  - i >= D_800ADBFC: the mode==0 block runs, then the mode==1 block
     *    (which re-reads the byte; a 0 never double-applies).
     * Adds are addu (u32 wrap). */
    for (i = 0; i < g_FieldNumActors; i++) {
        u8* actor = (u8*)g_FieldActors + i * 0x5C;

        if (i < D_800ADBFC) {
            u8* actorData = (u8*)(uintptr_t)*(u32*)(actor + 0x4C);

            if (*(u32*)(actorData + 0x12C) & 0x3) {
                continue;
            }
        } else if ((D_800B21D2 & 0x7F) == 0) {
            *(u32*)(actor + 0x20) += (u32)D_800B21BC;
            *(u32*)(actor + 0x24) += (u32)D_800B21C0;
            *(u32*)(actor + 0x28) += (u32)D_800B21C4;
        }

        if ((D_800B21D2 & 0x7F) == 1) {
            *(u32*)(actor + 0x20) += (u32)D_800B21BC;
            *(u32*)(actor + 0x24) += (u32)D_800B21C0;
            *(u32*)(actor + 0x28) += (u32)D_800B21C4;
        }
    }
}
#endif /* XENO_PC_PORT */

void func_800A2714(void) {
    ActorData* pActor;
    FieldActor* pFieldActors;
    int i;
    void* pData;

    if (g_GamePartySkinsInitialized) {
        // Read animation files
        for (i = 0; i < D_800ADBFC; i++) {
            ArchiveSetIndex(0x4, 0x0);
            pActor = (ActorData*)(uintptr_t)g_FieldActors[i].pActorData;
            if (pActor->unk124 != -1) {
                g_FieldScriptVMCurActor = pActor;
                pData = HeapAlloc(ArchiveDecodeAlignedSize(pActor->unk124, pActor) + 8, 0x0);
                g_FieldScriptVMCurActor->unk120 = (u32)(uintptr_t)pData;
                ArchiveReadFileToBuffer(g_FieldScriptVMCurActor->unk124, pData, 0, CdlModeSpeed); // Read from disc into buffer
                ArchiveCdDataSync(0);
            }
        }

        // Set Special animation file
        for (i = 0; i < D_800ADBFC; i++) {
            pFieldActors = g_FieldActors;
            pActor = (ActorData*)(uintptr_t)pFieldActors[i].pActorData;
            if (pActor->unk124 != -1) {
                SpriteSetSpecialAnimFile((void*)(uintptr_t)pFieldActors[i].pSpriteData, pActor->unk120);
            }
        }
        
        func_800A3C8C();
        
        if (g_FieldEffects.distortion.isActive) {
            FieldDistortionInitialize(1);
        }
        
        FieldScriptMemoryWriteU16(0x10, 0x0);
        FieldScriptWritePartyMemberIDs();

        // Apply rotation and scale
        for (i = 0; i < D_800ADBFC; i++) {
            func_80072254(i);
        }
    }
}

#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/scripts/virtual_machine", func_800A28D4);
#else
void func_800A28D4(void) {
    int i;

    if (g_GamePartySkinsInitialized) {
        func_800A3474();

        /* asm 800A2914-800A2BC0.  The skin&0x80 arm re-reads
         * g_FieldActors[i].pActorData / .pSpriteData and the +0x12C/+0x130
         * words after func_80076AC0 and after func_8002303C (retail lw at
         * 800A2A44, 800A2A94-800A2AD0, 800A2B00-800A2B94), so callee writes
         * to those fields are honoured exactly as on retail. */
#define A28D4_ADATA(i) ((u8*)(uintptr_t)g_FieldActors[i].pActorData)
#define A28D4_STATE(i) ((u8*)(uintptr_t)*(u32*)((u8*)(uintptr_t) \
        *(u32*)((u8*)(uintptr_t)g_FieldActors[i].pSpriteData + 0x7C) + 0x18))
        for (i = 0; i < D_800ADBFC; i++) {
            u8* pActor = A28D4_ADATA(i);
            u8 skinId = *(u8*)(pActor + 0x126);
            u8 spriteId = *(u8*)(pActor + 0x127);
            u32 flags130 = *(u32*)(pActor + 0x130);
            u32 flags134 = *(u32*)(pActor + 0x134);

            if (!(skinId & 0x80)) {
                func_80076AC0(i, spriteId, g_PartyDataBuffers[skinId],
                              (flags130 >> 28) & 0x3, flags134 & 0xF,
                              skinId, (flags134 >> 4) & 0x1);
            } else {
                u8* pSpriteData = (u8*)g_FieldSpriteData;
                u8* pAnimData = pSpriteData + *(u32*)(pSpriteData + 4 + (skinId & 0x7F) * 4);
                u32 mode;

                func_80076AC0(i, spriteId, pAnimData, (flags130 >> 28) & 0x3,
                              flags134 & 0xF, skinId, (flags134 >> 4) & 0x1);

                mode = *(u16*)(A28D4_ADATA(i) + 0x12E) & 0x3;
                /* 800A2A74 (mode 1, a1=2) / 800A2AE4 (mode 2, a1=3). */
                if (mode == 1 || mode == 2) {
                    func_8002303C((void*)(uintptr_t)g_FieldActors[i].pSpriteData,
                                  mode == 1 ? 2 : 3, 0);
                    *(u16*)(A28D4_STATE(i) + 0x4) = (*(u32*)(A28D4_ADATA(i) + 0x12C) >> 18) & 0x3FF;
                    *(u16*)(A28D4_STATE(i) + 0x6) = *(u32*)(A28D4_ADATA(i) + 0x130) & 0x1FF;
                    if (mode == 2) {
                        *(u16*)(A28D4_STATE(i) + 0x8) = (*(u32*)(A28D4_ADATA(i) + 0x130) >> 9) & 0x3FF;
                        *(u16*)(A28D4_STATE(i) + 0xA) = (*(u32*)(A28D4_ADATA(i) + 0x130) >> 19) & 0x1FF;
                    }
                }
            }
        }
#undef A28D4_STATE
#undef A28D4_ADATA

        if (D_800B2268) {
            for (i = 0; i < 3; i++) {
                s32 actorIndex = D_8005A444[i];

                if (actorIndex != 0xFF) {
                    s32 swapActorIndex = D_8006F990[i];
                    ActorData* pSwapActor;

                    if (((u8*)g_pGameState)[0x22B1 + i]) {
                        u32 savedSpriteData = g_FieldActors[actorIndex].pSpriteData;

                        g_FieldActors[actorIndex].pSpriteData = g_FieldActors[swapActorIndex].pSpriteData;
                        g_FieldActors[swapActorIndex].pSpriteData = savedSpriteData;

                        pSwapActor = (ActorData*)(uintptr_t)g_FieldActors[swapActorIndex].pActorData;
                        /* asm 800A2CB0-800A2D04: lw/sw 0x0 (scriptFlags word). */
                        pSwapActor->scriptFlags.flags |= 0x200;
                        pSwapActor->scriptFlags.flags &= ~0x500;
                        g_FieldActors[swapActorIndex].status |= 0x20;
                    } else {
                        pSwapActor = (ActorData*)(uintptr_t)g_FieldActors[swapActorIndex].pActorData;
                        /* asm 800A2D6C-800A2DC0: lw/sw 0x0 (scriptFlags word). */
                        pSwapActor->scriptFlags.flags |= 0x400;
                        pSwapActor->scriptFlags.flags &= ~0x300;
                    }
                }
            }
        }

        return;
    }

    FieldScriptMemoryWriteU16(0x10, 0);
    FieldScriptWritePartyMemberIDs();

    for (i = 0; i < D_800ADBFC; i++) {
        D_800AFD1C = i;
        D_800B06B8 = &g_FieldActors[i];
        g_FieldScriptVMCurActor = (ActorData*)(uintptr_t)D_800B06B8->pActorData;
        g_FieldScriptVMCurActor->scriptInstructionPointer = FieldScriptGetBytecodeOffset(i, 2);
        if (((u8*)g_FieldScriptVMCurScriptData)[g_FieldScriptVMCurActor->scriptInstructionPointer] == 0) {
            g_FieldScriptVMCurActor->flags |= 0x4000000;
        }

        D_800AFD1C = i;
        D_800B06B8 = &g_FieldActors[i];
        g_FieldScriptVMCurActor = (ActorData*)(uintptr_t)D_800B06B8->pActorData;
        g_FieldScriptVMCurActor->scriptInstructionPointer = FieldScriptGetBytecodeOffset(i, 0);
    }

    for (i = 0; i < D_800ADBFC; i++) {
        D_800AFD1C = i;
        D_800AFC74 = 0;
        D_800AFFEC = 0;
        D_800B06B8 = &g_FieldActors[i];
        g_FieldScriptVMCurActor = (ActorData*)(uintptr_t)D_800B06B8->pActorData;
        FieldScriptVMRun(0xFFFF);

        if (D_800AFC74 == 0) {
            u8* pSpriteData = (u8*)g_FieldSpriteData;
            u8* pAnimData = pSpriteData + *(s32*)(pSpriteData + 4);
#ifdef XENO_PC_PORT
            /* Diagnostic-only static; see the matching-build note in
             * FieldAddPrimitives (misc2.c) -- confined to the port build so no
             * .sbss storage for it is emitted for the matching target. */
            {
                static s32 s_loggedFirstAttach;
                if (!s_loggedFirstAttach) {
                    printf("[field-diag] func_80076AC0 first: actor=%d actorPtr=%p actorSpriteSlot=%p curSprite=%p args=(%d,%d,%p,%d,%d,%d,%d)\n",
                           i, &g_FieldActors[i], &g_FieldActors[i].pSpriteData,
                           (void*)(uintptr_t)g_FieldActors[i].pSpriteData,
                           i, 0, pAnimData, 0, 0, 0x80, 0);
                    s_loggedFirstAttach = 1;
                }
            }
#endif
            func_80076AC0(i, 0, pAnimData, 0, 0, 0x80, 0);
            g_FieldScriptVMCurActor->flags |= 0x800;
        }
    }
}
#endif /* XENO_PC_PORT */

void FieldScriptVMHandlerNop(void) {
    g_FieldScriptVMCurActor->scriptInstructionPointer++;
}

// Script files contains a sections of sign bits for variables. 
// This function is a bit of a convoluted way to check if a bit in section of data is set or not.
int FieldScriptVMGetVariableSign(int index) {
    return -((g_FieldCurScriptFile->signBits[index >> 6] & (1 << ((index >> 1) & 0x1F))) != 0);
}

int FieldScriptVMGetVariableValue(int index) {
    if (!(g_FieldCurScriptFile->signBits[index >> 6] & (1 << ((index >> 1) & 0x1F)))) {
        return ((short*)&g_FieldScriptMemory)[index >> 1];
    } else {
        return ((u_short*)&g_FieldScriptMemory)[index >> 1];
    }
}

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): field_event_write_variable (src/decomp/src/field/field_event.c:10859). */
#ifndef XM_T_EventPackage
#define XM_T_EventPackage
typedef struct {
    u32 unsigned_bits[32]; 
    s32 count;             
    u16 entries[32];       
} EventPackage;
#endif
#ifndef XM_S_field_event_write_variable
#define XM_S_field_event_write_variable
void xm_field_event_write_variable(s32 reference, s32 value) __asm__("FieldScriptMemoryWriteU16");
#endif
#ifndef XM_S_field_event_package
#define XM_S_field_event_package
extern EventPackage *xm_field_event_package __asm__("g_FieldCurScriptFile");
#endif
#ifndef XM_S_field_event_variables
#define XM_S_field_event_variables
extern s16 xm_field_event_variables[0x400] __asm__("g_FieldScriptMemory");
#endif
void FieldScriptMemoryWriteU16(s32 reference, s32 value) {
    if (xm_field_event_package->unsigned_bits[reference >> 6] & (1 << ((reference >> 1) & 0x1F))) {
        xm_field_event_variables[reference >> 1] = (u16)value;
    } else {
        xm_field_event_variables[reference >> 1] = value;
    }
}
#endif
#else
/* g_FieldScriptMemory is declared `void*` (an 8-byte object) in field/actor.h
 * but is the 0x800-byte script variable buffer (g_FieldBss_800C38F8+0x170).
 * Address it through a correctly typed alias of the same symbol: indexing an
 * 8-byte object past its end is host UB (UBSan object-size). */
extern u16 g_FieldScriptMemoryU16[] __asm__("g_FieldScriptMemory");
void FieldScriptMemoryWriteU16(s32 address, u16 value) {
    s32 index = address >> 1;

    /* DIAGNOSTIC (port only, logging only): XENO_FIELD_VAR_WATCH=<byte
     * offset> logs every write to that script variable with the writer and
     * the clocks (pc_port/src/psyq_compat.c PcPort_FieldVarWatch). */
    {
        extern void PcPort_FieldVarWatch(s32 address, u16 old_value, u16 new_value,
                                         s32 actor, s32 ip) __attribute__((weak));
        if (PcPort_FieldVarWatch)
            PcPort_FieldVarWatch(address, g_FieldScriptMemoryU16[index], value, D_800AFD1C,
                                 g_FieldScriptVMCurActor
                                     ? (s32)g_FieldScriptVMCurActor->scriptInstructionPointer : -1);
    }
    g_FieldScriptMemoryU16[index] = value;
}
#endif /* XENO_PC_PORT */

// scriptIndex here refers to the index of the script, which will (always?) correspond to an entity index
// routineIndex is an index into the offset table, which points to a bytecode routine in that script.
u_short FieldScriptGetBytecodeOffset(int scriptIndex, int routineIndex) {
    int nOffset;
    u_short* pScriptData;

    pScriptData = (u_short*)((u8*)g_FieldCurScriptFile + 0x84);
    nOffset = (scriptIndex * (SCRIPT_OFFSET_TABLE_SIZE / sizeof(u_short)) + routineIndex);
    return *(pScriptData + nOffset);
}

void FieldScriptWritePartyMemberIDs(void) {
    FieldScriptMemoryWriteU16(0x3E, g_GamePartyMembers[0]);
    FieldScriptMemoryWriteU16(0x40, g_GamePartyMembers[1]);
    FieldScriptMemoryWriteU16(0x42, g_GamePartyMembers[2]);
}

extern u16 D_8005941C;
extern u8 D_80059418, D_80059420, D_80059484, D_800594D0;
extern s32 D_8004F2F4, D_8004F318, D_8004F324, D_8004F328;
extern s32 g_GameSceneMapNum, g_PlayerActorIndex;
extern u8 D_800B02C8;
extern u16 D_800AFC6C, D_800AFE9C;
extern int FieldGetPlayerActorDirection(void);
extern int FieldGetCameraDirection(void);
extern void func_8009FEE4(s32 arg0);
#ifdef XENO_PC_PORT
extern u32 g_PcPortOpcode56Func8009FEE4ConditionCount;
#endif

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): field_event_save_map_and_variables (src/decomp/src/field/field_event.c:10883). */
#ifndef XM_T_CharacterEntry
#define XM_T_CharacterEntry
typedef struct {
    u16 field0; 
    u8 value2;
    u8 value3;  
    u8 value4;  
    u8 pad5;
    u8 id;      
    u8 pad7;
} CharacterEntry;
#endif
#ifndef XM_T_StatusPair
#define XM_T_StatusPair
typedef union {
    u32 word;
    struct {
        u16 active;
        u16 permanent;
    } half;
} StatusPair;
#endif
#ifndef XM_T_CharacterRecord
#define XM_T_CharacterRecord
typedef struct {
    CharacterEntry entries[4]; 
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
    StatusPair status84;  
    StatusPair status88;  
    StatusPair status8C;  
    u16 useCounts[7];     
    u8 pad9E[0xA0 - 0x9E];
    u8 gearId;            
    u8 fieldA1;           
    u8 padA2[0xA4 - 0xA2];
} CharacterRecord;
#endif
#ifndef XM_T_GearEntry
#define XM_T_GearEntry
typedef struct {
    u16 field0; 
    u8 valueE;  
    u8 value10; 
    u8 value11; 
    u8 id;      
    u8 pad6[2];
} GearEntry;
#endif
#ifndef XM_T_GearRecord
#define XM_T_GearRecord
typedef struct GearRecord {
    u8 pad0[2];
    u8 engine;            
    u8 field3;            
    u8 partItems[4];      

    u8 frame;             
    u8 parts[3];          
    u8 weapons[4];        
    GearEntry entries[4]; 
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
    StatusPair status84;  
    u8 resistances[16];   
    u8 speed;             
    u8 defense;           
    u8 pad9A[0x9C - 0x9A];
    u8 guard;             
    u8 field9D;
    u8 frameFactor;       
    s8 hitBonus;          

    u8 padA0[0xA4 - 0xA0];
} GearRecord;
#endif
#ifndef XM_T_CharacterSkills
#define XM_T_CharacterSkills
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
} CharacterSkills;
#endif
#ifndef XM_T_WorldmapReturn
#define XM_T_WorldmapReturn
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
} WorldmapReturn;
#endif
#ifndef XM_T_FlightSave
#define XM_T_FlightSave
typedef struct {
    u16 x_frac;
    s16 x;
    u16 z_frac;
    s16 z;
    u16 count; 
} FlightSave;
#endif
#ifndef XM_T_SystemOptions
#define XM_T_SystemOptions
typedef struct {
    u32 version : 4;   
    u32 option4 : 1;
    u32 option5 : 1;
    u32 option6 : 7;
    u32 option13 : 3;  
    u32 complete : 1;  
    u32 unused : 15;
} SystemOptions;
#endif
#ifndef XM_T_GameData
#define XM_T_GameData
typedef struct GameData {
    u8 names[31][0x14];           

    CharacterRecord characters[11]; 
    GearRecord gears[20];           
    u8 unk1648[0x16C0 - 0x1648];
    CharacterSkills skills[11];     
    WorldmapReturn worldmap;        
    u16 unk1844[3];                 
    u16 unk184A;                    
    FlightSave flight;              
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
    SystemOptions options;        
    u8 unk2350[0x2355 - 0x2350];
    u8 flags2355;                 
    u8 unk2356[0x2358 - 0x2356];
} GameData;
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
#ifndef XM_S_field_event_save_map_and_variables
#define XM_S_field_event_save_map_and_variables
void xm_field_event_save_map_and_variables(void) __asm__("func_800A30FC");
#endif
#ifndef XM_S_game_current_data
#define XM_S_game_current_data
extern GameData *xm_game_current_data __asm__("g_pGameState");
#endif
#ifndef XM_S_mode_field_map_id
#define XM_S_mode_field_map_id
extern s32 xm_mode_field_map_id __asm__("g_GameSceneMapNum");
#endif
#ifndef XM_S_mode_music_selected_track
#define XM_S_mode_music_selected_track
extern s32 xm_mode_music_selected_track __asm__("D_8004F324");
#endif
#ifndef XM_S_mode_battle_turn_count
#define XM_S_mode_battle_turn_count
extern u16 xm_mode_battle_turn_count __asm__("D_8005941C");
#endif
#ifndef XM_S_mode_result_code
#define XM_S_mode_result_code
extern u8 xm_mode_result_code __asm__("D_800594D0");
#endif
#ifndef XM_S_field_actor_get_controlled_facing_octant
#define XM_S_field_actor_get_controlled_facing_octant
s32 xm_field_actor_get_controlled_facing_octant(void) __asm__("FieldGetPlayerActorDirection");
#endif
#ifndef XM_S_field_camera_get_octant
#define XM_S_field_camera_get_octant
s32 xm_field_camera_get_octant(void) __asm__("FieldGetCameraDirection");
#endif
#ifndef XM_S_field_event_store_party_members
#define XM_S_field_event_store_party_members
void xm_field_event_store_party_members(void) __asm__("FieldScriptWritePartyMemberIDs");
#endif
void func_800A30FC(void) {
    s32 i;

    xm_game_current_data->map = xm_mode_field_map_id;
    xm_game_current_data->flagWords[0] = xm_mode_music_selected_track;
    xm_game_current_data->entry[2] = xm_game_current_data->vars[1];
    xm_game_current_data->entry[0] = xm_game_current_data->vars[4] << 9;
    xm_field_event_write_variable(0x44, xm_mode_battle_turn_count);
    xm_field_event_write_variable(0x46, xm_mode_result_code);
    xm_field_event_write_variable(6, xm_field_actor_get_controlled_facing_octant() & 0xFFFF);
    xm_field_event_write_variable(8, xm_field_camera_get_octant() & 0xFFFF);
    xm_field_event_write_variable(0x24, (s16)xm_field_view.elevation);
    xm_field_event_write_variable(0x3C, xm_mode_field_map_id);
    xm_field_event_store_party_members();
    for (i = 0; i < 0x200; i++) {
        xm_game_current_data->vars[i] = xm_field_event_variables[i];
    }
}
#endif
#else
void func_800A30FC(void) {
    s32 i;
    u16* src;
    u16* dst;

    *(s16*)((u8*)g_pGameState + 0x231A) = g_GameSceneMapNum;
    *(s16*)((u8*)g_pGameState + 0x2322) = D_8004F324;
    *(s16*)((u8*)g_pGameState + 0x2320) = *(u16*)((u8*)g_pGameState + 0x1932);
    *(s16*)((u8*)g_pGameState + 0x231C) = *(u16*)((u8*)g_pGameState + 0x1938) << 9;

    FieldScriptMemoryWriteU16(0x44, D_8005941C);
    FieldScriptMemoryWriteU16(0x46, D_800594D0);
    FieldScriptMemoryWriteU16(0x6, FieldGetPlayerActorDirection() & 0xFFFF);
    FieldScriptMemoryWriteU16(0x8, FieldGetCameraDirection() & 0xFFFF);
    FieldScriptMemoryWriteU16(0x24, *(s16*)((u8*)&g_Scene + 0x6C));
    FieldScriptMemoryWriteU16(0x3C, g_GameSceneMapNum);
    FieldScriptWritePartyMemberIDs();

    src = g_FieldScriptMemoryU16; /* typed alias, see FieldScriptMemoryWriteU16 */
    dst = (u16*)((u8*)g_pGameState + 0x1930);
    for (i = 0; i < 0x200; i++) {
        *dst++ = *src++;
    }
}
#endif /* XENO_PC_PORT */

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): field_update_play_record (src/decomp/src/field/field_event.c:10907). */
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
#ifndef XM_T_WHOLE
#define XM_T_WHOLE
#define WHOLE(value) (((Fixed *)&(value))->part.whole)
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
#ifndef XM_S_field_update_play_record
#define XM_S_field_update_play_record
void xm_field_update_play_record(void) __asm__("func_800A31E8");
#endif
#ifndef XM_S_field_play_record_stopped
#define XM_S_field_play_record_stopped
extern u8 xm_field_play_record_stopped __asm__("D_800B02C8");
#endif
#ifndef XM_S_field_play_record_buttons
#define XM_S_field_play_record_buttons
extern u16 xm_field_play_record_buttons __asm__("D_800AFC6C");
#endif
#ifndef XM_S_field_pad_port0_held
#define XM_S_field_pad_port0_held
extern u16 xm_field_pad_port0_held __asm__("D_800AFE9C");
#endif
#ifndef XM_S_mode_party_members
#define XM_S_mode_party_members
extern s32 xm_mode_party_members[3] __asm__("g_GamePartyMembers");
#endif
#ifndef XM_S_mode_unread_play_record_word
#define XM_S_mode_unread_play_record_word
extern s32 xm_mode_unread_play_record_word __asm__("D_8004F2F4");
#endif
#ifndef XM_S_mode_play_clock_frame_count
#define XM_S_mode_play_clock_frame_count
extern s32 xm_mode_play_clock_frame_count __asm__("D_8004F318");
#endif
#ifndef XM_S_field_party_record_slot_position
#define XM_S_field_party_record_slot_position
s32 xm_field_party_record_slot_position(s32 slot) __asm__("func_8009FEE4");
#endif
#ifndef XM_S_mode_play_clock_flags
#define XM_S_mode_play_clock_flags
extern s32 xm_mode_play_clock_flags __asm__("D_8004F328");
#endif
#ifndef XM_S_field_event_read_variable
#define XM_S_field_event_read_variable
s32 xm_field_event_read_variable(s32 reference) __asm__("FieldScriptVMGetVariableValue");
#endif
#ifndef XM_S_pad_play_time_seconds
#define XM_S_pad_play_time_seconds
extern u8 xm_pad_play_time_seconds __asm__("D_80059418");
#endif
#ifndef XM_S_pad_play_time_minutes
#define XM_S_pad_play_time_minutes
extern u8 xm_pad_play_time_minutes __asm__("D_80059420");
#endif
#ifndef XM_S_pad_play_time_hours
#define XM_S_pad_play_time_hours
extern u8 xm_pad_play_time_hours __asm__("D_80059484");
#endif
void func_800A31E8(void) {
    s32 i;
    s32 value;
    s32 seconds;
    s32 minutes;

    if (xm_field_play_record_stopped == 1) {
        return;
    }
    xm_field_play_record_buttons |= xm_field_pad_port0_held;
    for (i = 0; i < 3; i++) {
        xm_game_current_data->party[i] = xm_mode_party_members[i];
    }
    xm_field_event_save_map_and_variables();
    xm_mode_unread_play_record_word = 0;
    xm_mode_play_clock_frame_count++;
    for (i = 0; i < 3; i++) {
        if (xm_game_current_data->inGear[i] == 1) {
            xm_field_party_record_slot_position(i);
        }
    }
    if (xm_mode_play_clock_frame_count > 30) {
        xm_mode_play_clock_frame_count = 0;
        if (!(xm_mode_play_clock_flags & 0x80)) {
            value = xm_field_event_read_variable(0xA);
            seconds = value & 0xFF;
            minutes = (value >> 8) & 0xFF;
            if (!(xm_mode_play_clock_flags & 4)) {
                if (seconds != 0xFF3B) { 
                    seconds++;
                    if (seconds > 60) {
                        seconds = 0;
                        minutes++;
                    }
                }
            } else if (seconds == 0) {
                if (minutes != 0) {
                    seconds = 59;
                    minutes--;
                }
            } else {
                seconds--;
            }
            xm_field_event_write_variable(0xA, (minutes << 8) | (seconds & 0xFF));
        }
    }
    xm_field_event_write_variable(0xC, xm_pad_play_time_seconds | (xm_pad_play_time_minutes << 8));
    xm_field_event_write_variable(0xE, xm_pad_play_time_hours);
    xm_field_event_write_variable(0x1E, WHOLE(xm_field_view.components.descriptors[xm_field_work.controlled].actor->position[0]));
    xm_field_event_write_variable(0x20, WHOLE(xm_field_view.components.descriptors[xm_field_work.controlled].actor->position[2]));
    xm_field_event_write_variable(0x22, WHOLE(xm_field_view.components.descriptors[xm_field_work.controlled].actor->position[1]));
}
#endif
#else
void func_800A31E8(void) {
    s32 i;

    if (D_800B02C8 == 1) {
        return;
    }

    D_800AFC6C |= D_800AFE9C;

    for (i = 0; i < 3; i++) {
        ((u8*)g_pGameState)[0x1D34 + i] = g_GamePartyMembers[i];
    }

    func_800A30FC();

    D_8004F2F4 = 0;
    D_8004F318 = (s32)((u32)D_8004F318 + 1); /* addiu: wraps, no signed-overflow UB */

    for (i = 0; i < 3; i++) {
        if (((u8*)g_pGameState)[0x22B1 + i] == 1) {
#ifdef XENO_PC_PORT
            /* F14 instrumentation: opcode 0x56 samples this counter around its
             * outgoing-state snapshot so the conditional stub path is never
             * silently mistaken for a complete snapshot. */
            g_PcPortOpcode56Func8009FEE4ConditionCount += 1;
#endif
            func_8009FEE4(i);
        }
    }

    if (D_8004F318 >= 0x1F) {
        D_8004F318 = 0;
        if (!(D_8004F328 & 0x80)) {
            s32 value = FieldScriptVMGetVariableValue(0xA);
            s32 seconds = value & 0xFF;
            s32 minutes = (value >> 8) & 0xFF;

            if (!(D_8004F328 & 0x4)) {
                seconds += 1;
                if (seconds >= 0x3D) {
                    seconds = 0;
                    minutes += 1;
                }
            } else if (seconds == 0) {
                if (minutes != 0) {
                    seconds = 0x3B;
                    minutes -= 1;
                }
            } else {
                seconds -= 1;
            }
            FieldScriptMemoryWriteU16(0xA, (minutes << 8) | (seconds & 0xFF));
        }
    }

    FieldScriptMemoryWriteU16(0xC, D_80059418 | (D_80059420 << 8));
    FieldScriptMemoryWriteU16(0xE, D_80059484);
    FieldScriptMemoryWriteU16(0x1E, *(s16*)((u8*)g_FieldActors[g_PlayerActorIndex].pActorData + 0x22));
    FieldScriptMemoryWriteU16(0x20, *(s16*)((u8*)g_FieldActors[g_PlayerActorIndex].pActorData + 0x2A));
    FieldScriptMemoryWriteU16(0x22, *(s16*)((u8*)g_FieldActors[g_PlayerActorIndex].pActorData + 0x26));
}
#endif /* XENO_PC_PORT */

extern u8 D_8005A4E4[];
extern s32 D_8005A408[];
extern u8* D_800AFC50;
extern u8 D_800B007C[];
extern u32 D_800AFB20[];
extern VECTOR g_CameraEye;
extern char D_8006FD9C;
extern void func_80021EBC(void*, void*);

extern void* memcpy(void*, const void*, size_t);

#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/scripts/virtual_machine", func_800A3474);
#else
void func_800A3474(void) {
    int i;

    D_800AFC50 = D_8005A4E4;
    g_FieldNumActors = D_8005A4E4[0];
    D_800AFC50 = D_8005A4E4 + 4;

    memcpy(D_800B007C, D_800AFC50, 0x38);
    D_800AFC50 += 0x38;
    memcpy((u8*)&g_Scene + 0xC4, D_800AFC50, 0x74);
    D_800AFC50 += 0x74;
    memcpy((void*)(uintptr_t)D_800AFB20[0], D_800AFC50, 0x400);
    D_800AFC50 += 0x400;
    memcpy((u8*)&g_FieldEffects, D_800AFC50, 0x2E4);
    D_800AFC50 += 0x2E4;
    memcpy((u8*)&g_CameraEye, D_800AFC50, 0x1C8);
    D_800AFC50 += 0x1C8;

    for (i = 0; i < D_800ADBFC; i++) {
        u8* actor = (u8*)&g_FieldActors[i];
        u8* actorData = (u8*)(uintptr_t)*(u32*)(actor + 0x4C);
        u8* actorSave = D_800AFC50;
        u32 saved118;

        memcpy(actor + 0x50, D_800AFC50, 0x8);
        D_800AFC50 += 0x8;
        *(u16*)(actor + 0x58) = *(u16*)D_800AFC50;
        D_800AFC50 = actorSave + 0x3C;

        saved118 = *(u32*)(actorData + 0x118);
        memcpy(actorData, D_800AFC50, 0x138);
        D_800AFC50 += 0x138;
        *(u32*)(actorData + 0x118) = saved118;

        if (*(u32*)(actorData + 0x134) & 0x80) {
            *(u32*)(actorData + 0x110) = (u32)(uintptr_t)HeapAlloc(0xC, 0);
            memcpy((void*)(uintptr_t)*(u32*)(actorData + 0x110), D_800AFC50, 0xC);
            D_800AFC50 += 0xC;
        }

        if (*(u32*)(actorData + 0x12C) & 0x1000) {
            *(u32*)(actorData + 0x114) = (u32)(uintptr_t)HeapAlloc(0x10, 0);
            memcpy((void*)(uintptr_t)*(u32*)(actorData + 0x114), D_800AFC50, 0x10);
            D_800AFC50 += 0x10;
        }
    }

    memcpy(&g_FieldScriptMemory, D_800AFC50, 0x800);
    D_800AFC50 += 0x800;
}
#endif /* XENO_PC_PORT */

extern s32 D_800B2268;
extern void func_80021D50(void* pSprite, void* pSave);

/* Transcribed from asm/field/nonmatchings/scripts/virtual_machine/func_800A3C8C.s
 * (0x800A3C8C-0x800A3F48). Restore g_FieldNumActors from D_8005A4E4[0], memcpy
 * 0x74 bytes at D_8005A4E4+0x3C into g_Scene+0xC4, count D_8005A408 vs
 * g_pGameState[0x22B1+i] mismatches, skip to actor records at +0x95C. Per
 * actor: skip 0xC header; if unk124!=-1 and unkEA!=0xFF patch save+0x20;
 * func_80021D50 unless flags&0x1000000 or (D_800B2268 && scriptFlags&0x600
 * && mismatch); advance 0x168, +0xC if flags134&0x80, +0x10 if +0x12C&0x1000.
 * Frame 0x30. */
#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): field_restore_snapshot_sprites (src/decomp/src/field/field_event.c:11012). */
#ifndef XM_T_xm_ViewSnapshot
#define XM_T_xm_ViewSnapshot
typedef struct {
    u8 bytes[0x74];
} xm_ViewSnapshot;
#endif
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
#ifndef XM_T_xm_SpriteState
#define XM_T_xm_SpriteState
typedef struct {
    s32 x, y, z;           
    u8 unknownc[4];
    u16 word80;            
    s16 frame;             
    s16 byteaf;            
    s16 byteb0;            
    s16 field22;           
    u8 unknown1a[2];
    s32 sequencer0;        
    s32 sequencer4;        
    u16 scale_x;           
    u16 scale_y;
    u16 scale_z;
    u16 scale;             
    u16 word82;            
} xm_SpriteState;
#endif
#ifndef XM_S_field_restore_snapshot_sprites
#define XM_S_field_restore_snapshot_sprites
void xm_field_restore_snapshot_sprites(void) __asm__("func_800A3C8C");
#endif
#ifndef XM_S_field_snapshot_cursor
#define XM_S_field_snapshot_cursor
extern u8 *xm_field_snapshot_cursor __asm__("D_800AFC50");
#endif
#ifndef XM_S_mode_snapshot_block
#define XM_S_mode_snapshot_block
extern u8 xm_mode_snapshot_block[] __asm__("D_8005A4E4");
#endif
#ifndef XM_S_mode_snapshot_party_in_gear
#define XM_S_mode_snapshot_party_in_gear
extern s32 xm_mode_snapshot_party_in_gear[3] __asm__("D_8005A408");
#endif
#ifndef XM_S_sprite_restore_state
#define XM_S_sprite_restore_state
void xm_sprite_restore_state(xm_Sprite *sprite, xm_SpriteState *state) __asm__("func_80021D50");
#endif
void func_800A3C8C(void) {
    s32 changed;
    s32 i;
    u8 *record;

    xm_field_snapshot_cursor = xm_mode_snapshot_block;
    xm_field_view.components.descriptor_count = *xm_field_snapshot_cursor;
    xm_field_snapshot_cursor += 0x3C;
    *(xm_ViewSnapshot *)&xm_field_view.world_angles = *(xm_ViewSnapshot *)xm_field_snapshot_cursor;
    xm_field_snapshot_cursor += 0x920;
    changed = 0;
    for (i = 0; i < 3; i++) {
        if (xm_mode_snapshot_party_in_gear[i] != xm_game_current_data->inGear[i]) {
            changed++;
        }
    }
    for (i = 0; i < xm_field_event_actor_count; i++) {
        xm_field_snapshot_cursor += 0xC;
        record = xm_field_snapshot_cursor;
        if (xm_field_view.components.descriptors[i].actor->unk124 != -1 && xm_field_view.components.descriptors[i].actor->unk0EA != 0xFF) {
            *(s16 *)(record + 0x14) = xm_field_view.components.descriptors[i].actor->unk0EA;
        }
        if (!(xm_field_view.components.descriptors[i].actor->layer_flags & 0x1000000)) {
            if (xm_field_work.unk2268 == 0 || !(xm_field_view.components.descriptors[i].actor->flags & 0x600)) {
                xm_sprite_restore_state(xm_field_view.components.descriptors[i].model, (xm_SpriteState *)xm_field_snapshot_cursor);
            } else if (changed == 0) {
                xm_sprite_restore_state(xm_field_view.components.descriptors[i].model, (xm_SpriteState *)xm_field_snapshot_cursor);
            }
        }
        record = xm_field_snapshot_cursor;
        xm_field_snapshot_cursor = record + 0x168;
        if (xm_field_view.components.descriptors[i].actor->unk134 & 0x80) {
            xm_field_snapshot_cursor = record + 0x174;
        }
        if (xm_field_view.components.descriptors[i].actor->state.word & 0x1000) {
            xm_field_snapshot_cursor += 0x10;
        }
    }
}
#endif
#else
void func_800A3C8C(void) {
    s32 i;
    s32 mismatch;
    FieldActor* pField;
    ActorData* pActor;
    u8* pSave;

    D_800AFC50 = D_8005A4E4;
    g_FieldNumActors = D_8005A4E4[0];
    D_800AFC50 = D_8005A4E4 + 0x3C;
    memcpy((u8*)&g_Scene + 0xC4, D_800AFC50, 0x74);

    mismatch = 0;
    for (i = 0; i < 3; i++) {
        if (D_8005A408[i] != ((u8*)g_pGameState)[0x22B1 + i]) {
            mismatch += 1;
        }
    }
    D_800AFC50 += 0x920;

    for (i = 0; i < D_800ADBFC; i++) {
        pField = &g_FieldActors[i];
        pActor = (ActorData*)(uintptr_t)pField->pActorData;
        pSave = D_800AFC50;
        D_800AFC50 = pSave + 0xC;
        if (pActor->unk124 != -1) {
            if (pActor->unkAnimationId != (s16)0xFF) {
#ifdef FIELD_A3C8C_MUTANT_SKIP_ANIM_PATCH
                /* skip save+0x20 patch */
#else
                *(s16*)(pSave + 0x20) = pActor->unkAnimationId;
#endif
            }
        }
        if ((pActor->flags & 0x1000000) == 0) {
            if (D_800B2268 == 0 || (pActor->scriptFlags.flags & 0x600) == 0
                || mismatch == 0) {
                func_80021D50((void*)(uintptr_t)pField->pSpriteData, D_800AFC50);
            }
        }
        if (*(u32*)((u8*)pActor + 0x134) & 0x80) {
            D_800AFC50 = pSave + 0xC + 0x174;
        } else {
            D_800AFC50 = pSave + 0xC + 0x168;
        }
        if (*(u32*)((u8*)pActor + 0x12C) & 0x1000) {
            D_800AFC50 += 0x10;
        }
    }
}
#endif /* XENO_PC_PORT */


#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/scripts/virtual_machine", func_800A3F4C);
#else
void func_800A3F4C(void) {
    int i;

    D_800AFC50 = D_8005A4E4;
    D_8005A4E4[0] = (u8)g_FieldNumActors;
    D_800AFC50 = D_8005A4E4 + 4;

    memcpy(D_800AFC50, D_800B007C, 0x38);
    D_800AFC50 += 0x38;
    memcpy(D_800AFC50, (u8*)&g_Scene + 0xC4, 0x74);
    D_800AFC50 += 0x74;
    memcpy(D_800AFC50, (void*)(uintptr_t)D_800AFB20[0], 0x400);
    D_800AFC50 += 0x400;
    memcpy(D_800AFC50, (u8*)&g_FieldEffects, 0x2E4);
    D_800AFC50 += 0x2E4;
    memcpy(D_800AFC50, (u8*)&g_CameraEye, 0x1C8);
    D_800AFC50 += 0x1C8;

    for (i = 0; i < D_800ADBFC; i++) {
        u8* actor = (u8*)&g_FieldActors[i];
        u8* actorData = (u8*)(uintptr_t)*(u32*)(actor + 0x4C);
        u8* spriteData = (u8*)(uintptr_t)*(u32*)(actor + 0x04);
        u8* actorSave = D_800AFC50;

        memcpy(D_800AFC50, actor + 0x50, 0x8);
        D_800AFC50 += 0x8;
        *(u16*)(actorSave + 0x8) = *(u16*)(actor + 0x58);
        *(u16*)(actorSave + 0xA) = 0;
        D_800AFC50 = actorSave + 0xC;

        func_80021EBC(spriteData, D_800AFC50);
        D_800AFC50 += 0x30;

        memcpy(D_800AFC50, actorData, 0x138);
        D_800AFC50 += 0x138;

        if (*(u32*)(actorData + 0x134) & 0x80) {
            memcpy(D_800AFC50, (void*)(uintptr_t)*(u32*)(actorData + 0x110), 0xC);
            D_800AFC50 += 0xC;
        }

        if (*(u32*)(actorData + 0x12C) & 0x1000) {
            memcpy(D_800AFC50, (void*)(uintptr_t)*(u32*)(actorData + 0x114), 0x10);
            D_800AFC50 += 0x10;
        }
    }

    memcpy(D_800AFC50, &g_FieldScriptMemory, 0x800);
    D_800AFC50 += 0x800;

    for (i = 0; i < 3; i++) {
        D_8005A408[i] = ((u8*)g_pGameState)[0x22B1 + i];
    }

    if (g_FieldSystemMode == 0) {
        func_800379C8(&D_8006FD9C, D_800AFC50 - D_8005A4E4, D_800AFC50 - D_8005A4E4);
    }
}
#endif /* XENO_PC_PORT */

void func_800A4748(void) {
    func_800A476C(0x2c0,0x100);
}

void func_800A476C(int x, int y) {
    RECT rect;

    rect.w = 0x140;
    rect.y = 0;
    rect.x = 0;
    rect.h = 0xe0;
    SetGeomScreen(0x200);
#ifdef XENO_PC_PORT
    /* PsyCross DrawOTag writes the GL backbuffer; retail GPU already has
     * those pixels in VRAM at (0,0). Pull them into CPU vram[] before the
     * offscreen snapshot (title/menu backdrop at 704,256).  Skip it when the
     * scene rendered since the last present never touched this rect: the
     * backbuffer then holds another VRAM buffer (after the title attract
     * the field's first returning frame is drawn at (0,256) while (0,0)
     * holds the converted last movie frame), and copying it here replaced
     * the frame retail crossfades from with black. */
    {
        extern void GR_StoreFrameBufferImmediate(int x, int y, int w, int h);
        extern int PsyX_XenoRenderedClipMisses(int x, int y, int w, int h);
        if (!PsyX_XenoRenderedClipMisses(rect.x, rect.y, rect.w, rect.h)) {
            GR_StoreFrameBufferImmediate(rect.x, rect.y, rect.w, rect.h);
        }
    }
#endif
    MoveImage(&rect, x, y);
    FieldRenderSync();
}
