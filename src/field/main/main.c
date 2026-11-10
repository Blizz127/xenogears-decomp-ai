#include "common.h"
#include "psyq/libgpu.h"
#include "psyq/libetc.h"
#include "system/memory.h"
#include "system/archive.h"
#include "system/controller.h"
#include "field/actor.h"

extern int g_FrameDeltaTime;

void FieldInitializeControllers(void) {
    HeapChangeCurrentUser(HEAP_USER_YOSI, NULL);
    ArchiveSetIndex(4, 0);
    FieldInitializeControllersAndMouse();
}

void FieldRenderSync(void) {
    DrawSync(0);
    Vsync(0);
}

/* ---- FieldLoadUITextures: load UI texture CLUT from archive ------------------
 * Loads archive 0xA7, unpacks 8 TIM images via FieldLoadTIMWithClut,
 * stores combined CLUT to VRAM at D_800B004C RECT, reads back into
 * D_800AFC08 via StoreImage for later use by func_80074108.
 *
 * Guard: D_8004F344 — non-zero skips archive load (textures already loaded).
 * Texture descriptors: D_800ADC44[8] — 0xC bytes/entry (ROM data).
 * Temp buffer D_8005A4A0 is HeapAlloc'd, freed at end. */
extern s32 D_8004F344;
extern void* D_8005A4A0;
extern u16 D_800ADC44[8 * 6];
extern u16 D_800C2692, D_800C2690, D_800C38FE, D_800C38FC;
extern u16 D_800B004C, D_800B004E, D_800B0050, D_800B0052;
extern u16 D_800AFC08[0x80];
extern void ResolveArchiveEntryPointers(void* pData);

#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/main", FieldLoadUITextures);
#else
void FieldLoadUITextures(void) {
    u16* pTable;
    u16* pScan;
    u16* pTableB;
    s32 byteOff;
    u32* pDataPtr;
    s32 i;

#ifdef XENO_PC_PORT
    /* Boot streams archive 0xA7 into D_8005A4A0 and sets D_8004F344=1.
     * Movie teardown / heap reset can leave that pointer non-canonical
     * (observed: HeapUnpinBlock(0xf8000248e20000b0) SIGSEGV on title
     * entry). Reload from the archive whenever the cached buffer is not
     * a host heap pointer. */
    if (D_8005A4A0 == NULL ||
        ((uintptr_t)D_8005A4A0 >> 47) != 0) {
        D_8004F344 = 0;
        D_8005A4A0 = NULL;
    }
#endif

    if (D_8004F344 == 0) {
        s32 size = ArchiveDecodeAlignedSize(0xA7);
        void* pBuf = HeapAlloc(size, 1);
        D_8005A4A0 = pBuf;
        HeapPinBlock(pBuf);
        ArchiveReadFileToBuffer(0xA7, (u32*)pBuf, 0, 0x80);
        ArchiveCdDataSync(0);
    }

    pTable = D_800ADC44;
    pTableB = D_800ADC44 + 5;
    if (D_8005A4A0 != NULL) {
        HeapUnpinBlock(D_8005A4A0);
    }
    D_8004F344 = 0;
    D_800C2692 = 0;
    D_800C2690 = 0;
    D_800C38FE = 0;
    D_800C38FC = 0;
    ResolveArchiveEntryPointers(D_8005A4A0);

    byteOff = 0;
    pDataPtr = (u32*)((u8*)D_8005A4A0 + 4);
    pScan = pTable;

    for (i = 0; i < 8; i++) {
        FieldLoadTIMWithClut((u_long*)*pDataPtr,
            pScan[0],
            *(s16*)((u8*)pTable + byteOff + 2),
            *(s16*)((u8*)pTable + byteOff + 4),
            *(s16*)((u8*)pTable + byteOff + 6),
            *(s16*)((u8*)pTable + byteOff + 8),
            pTableB[0]);
        DrawSync(0);
        pScan += 6;
        pTableB += 6;
        byteOff += 0xC;
        pDataPtr += 1;
    }

    D_800B004C = 0;
    D_800B004E = 0xFB;
    D_800B0050 = 0x10;
    D_800B0052 = 1;
    StoreImage((RECT*)&D_800B004C, (u_long*)D_800AFC08);
    DrawSync(0);
#ifdef XENO_PC_PORT
    /* PsyCross StoreImage/GR_ReadVRAM doesn't write back to the caller's
     * buffer (VRAM data exists but the memcpy path is broken). Copy directly
     * from PsyCross's host VRAM array to D_800AFC08 as a compatibility shim.
     * Remove once GR_ReadVRAM is fixed upstream. */
    {
        extern unsigned short vram[];  /* PsyCross host VRAM (PsyX_render.cpp) */
        s32 vy = D_800B004E;
        s32 vw = D_800B0050;
        for (i = 0; i < vw && i < 0x80; i++) {
            D_800AFC08[i] = vram[i + vy * 1024];
        }
    }
#endif
    HeapFree(D_8005A4A0);
}
#endif /* XENO_PC_PORT */

extern s32 g_GameSceneMapNum;

void func_800777DC(void) {
    ArchiveCdDataSync(0);
    while (func_8001B484((g_GameSceneMapNum & 0xFFF) << 1, 0) != 0) {
    }
}

void FieldUpdateDeltaTime(void) {
    g_FrameDeltaTime = Vsync(1);
}

void func_80077844(short* dst, int a, int b, int c, int d, int e, int f, int g, int h, int i) {
    dst[0] = a;
    dst[1] = b;
    dst[2] = c;
    dst[3] = d;
    dst[4] = e;
    dst[5] = f;
    dst[6] = g;
    dst[7] = h;
    dst[8] = i;
}

extern s32 D_800B2264;
extern u8 D_800B2394;
extern s32 D_8004F370;
extern void* D_800ADB20;
extern void* D_800ADB30;
extern s16 D_800B234A;
extern u8 D_800B225C, D_800B225D, D_800B225E;
extern u8 g_FieldEffects[];
extern void* D_8005A420[];
extern void* D_8005A450[];
extern void* D_801E8644;
#ifdef XENO_PC_PORT
/* Retail 0x801E8670 is a table of PSX words; a void* declaration strides
 * eight bytes on the 64-bit host, so slots 1.. read the wrong entries and
 * the object scale table below was never written for them. */
extern u32 D_801E8670[];
#else
extern void* D_801E8670[];
#endif
extern void FieldRenderSyncAndFlush(void);
extern void func_801E738C(s32 arg0);
extern void func_801E742C(s32 arg0, s32 arg1, void* arg2, void* arg3,
                          s32 arg4, s32 arg5, s32 arg6, s32 arg7, void* arg8);
extern void func_800A90B4(s32 arg0);

static inline void FieldSetArchiveQueueEntry(s32 index, u16 archiveIndex, void* pData) {
    u8* entry = &D_800B2394 + index * 8;

    *(u16*)entry = archiveIndex;
    *(u32*)(entry + 4) = (u32)(unsigned long)pData;
}

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): field_layer_load (src/decomp/src/field/field.c:2755). */
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
#ifndef XM_T_FileRequest
#define XM_T_FileRequest
typedef struct FileRequest {
    u16 file;
    void *destination;
} FileRequest;
#endif
#ifndef XM_S_field_layer_load
#define XM_S_field_layer_load
void xm_field_layer_load(void) __asm__("func_80077884");
#endif
#ifndef XM_S_field_work
#define XM_S_field_work
extern FieldWork xm_field_work __asm__("g_FieldEffects");
#endif
#ifndef XM_S_field_music_wait_stream_and_disc_idle
#define XM_S_field_music_wait_stream_and_disc_idle
void xm_field_music_wait_stream_and_disc_idle(void) __asm__("func_8008A520");
#endif
#ifndef XM_S_cd_select_directory
#define XM_S_cd_select_directory
s32 xm_cd_select_directory(s32 group, s32 index) __asm__("ArchiveSetIndex");
#endif
#ifndef XM_S_field_vram_column_relocate
#define XM_S_field_vram_column_relocate
void xm_field_vram_column_relocate(s32 flags) __asm__("func_800A90B4");
#endif
#ifndef XM_S_field_heap_top
#define XM_S_field_heap_top
extern u32 xm_field_heap_top __asm__("D_800ADB30");
#endif
#ifndef XM_S_mode_field_standalone
#define XM_S_mode_field_standalone
extern s32 xm_mode_field_standalone __asm__("D_8004F370");
#endif
#ifndef XM_S_field_layer_module
#define XM_S_field_layer_module
extern void *xm_field_layer_module __asm__("D_800ADB20");
#endif
#ifndef XM_S_heap_alloc
#define XM_S_heap_alloc
void *xm_heap_alloc(s32 size, s32 mode) __asm__("HeapAlloc");
#endif
#ifndef XM_S_cd_get_aligned_file_size
#define XM_S_cd_get_aligned_file_size
s32 xm_cd_get_aligned_file_size(s32 file) __asm__("ArchiveDecodeAlignedSize");
#endif
#ifndef XM_S_field_layer_file_requests
#define XM_S_field_layer_file_requests
extern FileRequest xm_field_layer_file_requests[10] __asm__("D_800B2394");
#endif
#ifndef XM_S_mode_field_layer_model_files
#define XM_S_mode_field_layer_model_files
extern void *xm_mode_field_layer_model_files[4] __asm__("D_8005A450");
#endif
#ifndef XM_S_mode_field_layer_script_files
#define XM_S_mode_field_layer_script_files
extern void *xm_mode_field_layer_script_files[4] __asm__("D_8005A420");
#endif
#ifndef XM_S_cd_read_file_list
#define XM_S_cd_read_file_list
s32 xm_cd_read_file_list(FileRequest *list, s32 mode, s32 unused) __asm__("func_80029AFC");
#endif
void func_80077884(void) {
    u32 end;
    s32 i;

    if (xm_field_work.unk2264 != 0) {
        xm_field_music_wait_stream_and_disc_idle();
        xm_cd_select_directory(4, 0);
        xm_field_vram_column_relocate(0);
        end = xm_field_heap_top;
        if (xm_mode_field_standalone == 0) {
            xm_field_layer_module = xm_heap_alloc((end & 0xFFFFFF) - 0x1DC008, 1);
        } else {
            xm_field_layer_module = xm_heap_alloc(xm_cd_get_aligned_file_size(0x6B9), 1);
        }
        xm_field_vram_column_relocate(1);
        for (i = 0; i < xm_field_work.unk2264; i++) {
            xm_field_layer_file_requests[i * 2 + 1].file = xm_field_work.unk21DC[i] + 0x6BB;
            xm_mode_field_layer_model_files[i] = xm_heap_alloc(xm_cd_get_aligned_file_size(xm_field_work.unk21DC[i] + 0x6BB), 1);
            xm_field_layer_file_requests[i * 2 + 1].destination = xm_mode_field_layer_model_files[i];
        }
        for (i = 0; i < xm_field_work.unk2264; i++) {
            xm_field_layer_file_requests[i * 2].file = xm_field_work.unk21DC[i] + 0x6BA;
            xm_mode_field_layer_script_files[i] = xm_heap_alloc(xm_cd_get_aligned_file_size(xm_field_work.unk21DC[i] + 0x6BA), 0);
            xm_field_layer_file_requests[i * 2].destination = xm_mode_field_layer_script_files[i];
        }
        xm_field_layer_file_requests[i * 2].file = 0x6B9;
        xm_field_layer_file_requests[i * 2].destination = xm_field_layer_module;
        xm_field_layer_file_requests[i * 2 + 1].file = 0;
        xm_field_layer_file_requests[i * 2 + 1].destination = 0;
        xm_field_music_wait_stream_and_disc_idle();
        xm_cd_read_file_list(xm_field_layer_file_requests, 0, 0);
    }
}
#endif
#else
void func_80077884(void) {
    s32 i;
    s16* archiveIds;
    s32 size;

    if (D_800B2264 == 0) {
        return;
    }

    func_8008A520();
    ArchiveSetIndex(4, 0);
    func_800A90B4(0);

#ifdef XENO_PC_PORT
    /* Retail's D_8004F370==0 branch sizes this buffer as the fixed-map gap
     * [0x801DC008, D_800ADB30) -- meaningless host-pointer arithmetic in the
     * port (bogus ~5MB -> HeapAlloc failure -> GameHandleError(130) spin,
     * MAP3 repro).  Size by the archive's decoded size instead -- the same
     * derivation retail's own else-branch uses for the same buffer.  Same
     * pattern as the menu-overlay fix in misc4.c (func_80078E90). */
    size = ArchiveDecodeAlignedSize(0x6B9);
#else
    if (D_8004F370 == 0) {
        size = ((u32)(unsigned long)D_800ADB30 & 0xFFFFFF) + (s32)0xFFE23FF8;
    } else {
        size = ArchiveDecodeAlignedSize(0x6B9);
    }
#endif
    D_800ADB20 = HeapAlloc(size, 1);

    func_800A90B4(1);

    archiveIds = (s16*)((u8*)&D_800B2264 - 0x88);
    for (i = 0; i < D_800B2264; i++) {
        s32 archiveIndex = (u16)archiveIds[i] + 0x6BB;
        D_8005A450[i] = HeapAlloc(ArchiveDecodeAlignedSize(archiveIndex), 1);
        FieldSetArchiveQueueEntry(i * 2 + 1, archiveIndex, D_8005A450[i]);
    }

    for (i = 0; i < D_800B2264; i++) {
        s32 archiveIndex = (u16)archiveIds[i] + 0x6BA;
        D_8005A420[i] = HeapAlloc(ArchiveDecodeAlignedSize(archiveIndex), 0);
        FieldSetArchiveQueueEntry(i * 2, archiveIndex, D_8005A420[i]);
    }

    FieldSetArchiveQueueEntry(D_800B2264 * 2, 0x6B9, D_800ADB20);
    FieldSetArchiveQueueEntry(D_800B2264 * 2 + 1, 0, NULL);
    func_8008A520();
    func_80029AFC((StreamDataQueueEntry*)&D_800B2394, 0, 0);
}
#endif /* XENO_PC_PORT */

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): field_layer_start (src/decomp/src/field/field.c:2792). */
#ifndef XM_T_ScriptBlock
#define XM_T_ScriptBlock
typedef struct ScriptBlock {
    u8 pad0[4];
    s32 *locals;            
    s32 entries[1];         
} ScriptBlock;
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
#ifndef XM_T_SoundOwner
#define XM_T_SoundOwner
typedef struct SoundOwner {
    u8 pad0[8];
    SoundBank *bank;        
    void *end;              
} SoundOwner;
#endif
#ifndef XM_T_ActorScript
#define XM_T_ActorScript
typedef struct ActorScript {
    u8 pad0[4];
    ScriptBlock *script;    
    SoundOwner *owner;      
} ActorScript;
#endif
#ifndef XM_T_ObjectDesc
#define XM_T_ObjectDesc
typedef struct {
    u8 pad0[2];
    s16 size[3];       


    s16 scale;         
    u8 field2A;        


    u8 padB;
    u16 flags;         
    u8 channelCount;   
    u8 padF;
    u8 imageAnimCount; 
    u8 pad11;
    u8 meshCount;      
    u8 pad13;
    s16 meshes[1];     


} ObjectDesc;
#endif
#ifndef XM_T_ObjectHeader
#define XM_T_ObjectHeader
typedef struct {
    u8 pad0[4];
    ObjectDesc *desc;  
    void *meshData[1]; 
} ObjectHeader;
#endif
#ifndef XM_T_ObjectModelFile
#define XM_T_ObjectModelFile
typedef struct {
    u8 pad0[4];
    void *images;         
    u8 *models;           
    u16 *hierarchy;       

    ObjectHeader *header; 
} ObjectModelFile;
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
#ifndef XM_T_ModelTable
#define XM_T_ModelTable
typedef struct ModelTable {
    SpriteModel **models;
    u32 count;
} ModelTable;
#endif
#ifndef XM_T_EffectEntry
#define XM_T_EffectEntry
typedef struct {
    u8 used;
    u8 field1;     
    u8 kind;       
    u8 tag;        
    u16 params[6]; 
    u16 time;      
    u16 duration;  
} EffectEntry;
#endif
#ifndef XM_T_ModelPart
#define XM_T_ModelPart
typedef struct ModelPart {
    struct ModelPart *parent; 
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
    EffectEntry *effects[3]; 
} ModelPart;
#endif
#ifndef XM_T_EntryTable
#define XM_T_EntryTable
typedef struct EntryTable {
    s32 count;
    s32 *entries;
} EntryTable;
#endif
#ifndef XM_T_EffectSprite
#define XM_T_EffectSprite
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
} EffectSprite;
#endif
#ifndef XM_T_SpritePool
#define XM_T_SpritePool
typedef struct SpritePool {
    EffectSprite *records;
    s16 count;
    s16 next;
} SpritePool;
#endif
#ifndef XM_T_ColorFade
#define XM_T_ColorFade
typedef struct ColorFade {
    s16 id;           
    u8 solid;         
    u8 semiTrans;     
    SpritePool *pool; 
    EffectSprite *sprite; 
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
} ColorFade;
#endif
#ifndef XM_T_SurfaceEntry
#define XM_T_SurfaceEntry
typedef struct {
    s16 h0, h2, h4, h6, h8, hA, hC, hE;
} SurfaceEntry;
#endif
#ifndef XM_T_SurfacePoint
#define XM_T_SurfacePoint
typedef struct {
    s16 length;
    s16 sag;
    s16 pos[3];
    u16 normalCount; 
    s32 normal[3];   
} SurfacePoint;
#endif
#ifndef XM_T_SurfacePoly
#define XM_T_SurfacePoly
typedef struct {
    s16 index[3];
    u8 pad6[2];
    POLY_GT3 prim[2];
} SurfacePoly;
#endif
#ifndef XM_T_Surface
#define XM_T_Surface
typedef struct Surface {
    u16 h0;
    u8 pad2[2];
    s16 rings;              
    s16 polys;              
    s16 points;             
    s16 entryCount;         
    u8 b[6];                
    u8 pad12[2];
    SVECTOR *centres;       
    SurfaceEntry *entries;  
    SurfacePoint **strands; 
    SurfacePoly *polyList;  
} Surface;
#endif
#ifndef XM_T_ColorRow
#define XM_T_ColorRow
typedef struct {
    u16 c[3];
} ColorRow;
#endif
#ifndef XM_T_FrameCurve
#define XM_T_FrameCurve
typedef s16 (*FrameCurve)();
#endif
#ifndef XM_T_ImageAnim
#define XM_T_ImageAnim
typedef struct ImageAnim {
    struct ImageAnim *target; 
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
    ColorRow *colors;         
    s16 divisor;              
    s16 base;                 
    FrameCurve curve;         
    RECT rect;                
} ImageAnim;
#endif
#ifndef XM_T_Actor
#define XM_T_Actor
typedef struct Actor {
    ModelTable *models;     
    ModelPart *parts;       
    s32 *entries;           
    EntryTable *shared;     
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
    SoundOwner *ownerB0;    
    SoundOwner *ownerB4;    
    POLY_FT4 prims[2];      
    u8 pad108[2];
    u16 mask;               
    u8 channel_count;       
    u8 surfaceCount;        
    u8 imageCount;          
    u8 pad10F;
    struct ColorFade *channels; 
    struct Surface *surfaces;   
    struct ImageAnim *images;   
    s32 previous[3];        
    s32 moved[3];           

} Actor;
#endif
#ifndef XM_S_field_layer_start
#define XM_S_field_layer_start
void xm_field_layer_start(void) __asm__("func_80077AB4");
#endif
#ifndef XM_S_field_sync_and_flush_cache
#define XM_S_field_sync_and_flush_cache
void xm_field_sync_and_flush_cache(void) __asm__("FieldRenderSyncAndFlush");
#endif
#ifndef XM_S_gear_model_init
#define XM_S_gear_model_init
void xm_gear_model_init(s32 slot_count) __asm__("func_801E738C");
#endif
#ifndef XM_S_gear_model_color_matrix
#define XM_S_gear_model_color_matrix
extern MATRIX *xm_gear_model_color_matrix __asm__("D_801E8644");
#endif
#ifndef XM_S_SetBackColor
#define XM_S_SetBackColor
void xm_SetBackColor(long rbk, long gbk, long bbk) __asm__("SetBackColor");
#endif
#ifndef XM_S_gear_model_create_actor
#define XM_S_gear_model_create_actor
void xm_gear_model_create_actor(s32 index, u16 flags, ActorScript *script, ObjectModelFile *file, s16 x, s16 y,
                   s16 z, s16 w, s16 *pos) __asm__("func_801E742C");
#endif
#ifndef XM_S_heap_free
#define XM_S_heap_free
s32 xm_heap_free(void *data) __asm__("HeapFree");
#endif
#ifndef XM_S_gear_model_actors
#define XM_S_gear_model_actors
extern Actor *xm_gear_model_actors[10] __asm__("D_801E8670");
#endif
#ifndef XM_S_heap_select_owner_tag
#define XM_S_heap_select_owner_tag
void xm_heap_select_owner_tag(s32 tag, char **names) __asm__("HeapChangeCurrentUser");
#endif
void func_80077AB4(void) {
    SVECTOR *position;
    s32 row;
    s32 i;

    if (xm_field_work.unk2264 != 0) {
        xm_field_music_wait_stream_and_disc_idle();
        xm_field_sync_and_flush_cache();
        xm_gear_model_init(xm_field_work.unk234A);
        xm_gear_model_color_matrix = (MATRIX *)xm_field_work.unk223C;
        xm_SetBackColor(xm_field_work.unk225C[0], xm_field_work.unk225C[1], xm_field_work.unk225C[2]);
        for (i = 0; i < xm_field_work.unk2264; i++) {
            position = &xm_field_work.layer_positions[i];
            position->vx = 0;
            position->vy = 0;
            position->vz = 0;
            row = xm_field_work.unk225F[i];
            xm_gear_model_create_actor(i, 0, xm_mode_field_layer_script_files[i], xm_mode_field_layer_model_files[i],
                          (s16)(0x240 - ((i + row) << 6)), 0x100, 0, (s16)(i + 0xFC),
                          &position->vx);
            xm_heap_free(xm_mode_field_layer_model_files[i]);
            xm_field_work.layer_scales[i] = xm_gear_model_actors[i]->scale;
        }
        xm_heap_select_owner_tag(8, 0);
    }
}
#endif
#else
void func_80077AB4(void) {
    s32 i;
    u8* work;
    s32 top;

    work = (u8*)&D_800B2264 - 0x78;
    if (D_800B2264 == 0) {
        return;
    }

    func_8008A520();
    FieldRenderSyncAndFlush();
    func_801E738C(D_800B234A);
    D_801E8644 = (u8*)&D_800B2264 - 0x28;
    SetBackColor(D_800B225C, D_800B225D, D_800B225E);

    top = 0xFC;
    for (i = 0; i < D_800B2264; i++) {
        s16* vec = (s16*)(work + i * 8);
        s32 effect = ((u8*)g_FieldEffects)[0x1E7 + i];
        s32 x = (s16)(0x240 - ((i + effect) << 6));

        vec[0] = 0;
        vec[1] = 0;
        vec[2] = 0;
        func_801E742C(i, 0, D_8005A420[i], D_8005A450[i],
                      x, 0x100, 0, top, vec);
        HeapFree(D_8005A450[i]);
#ifdef XENO_PC_PORT
        /* Native owner guard: func_801E742C normally publishes the instantiated
         * archive-0x6B9 object here. Retail assumes that owner succeeded; the
         * fail-closed branch remains an explicit, audited port divergence. */
        if (D_801E8670[i] == 0) {
            top += 1;
            continue;
        }
#endif
        *(s32*)(work + 0x20 + i * 4) = *(s16*)((u8*)(uintptr_t)D_801E8670[i] + 0x1C);
        top += 1;
    }

    HeapChangeCurrentUser(HEAP_USER_YOSI, NULL);
}
#endif /* XENO_PC_PORT */

void func_80077C60(void) {
    func_80077884();
    func_80077AB4();
}

extern void* g_PartyDataBuffers[];

void FieldPartyAllocateSkinDataBuffers(void) {
    HeapChangeCurrentUser(HEAP_USER_YOSI, NULL);
    g_PartyDataBuffers[0] = HeapAlloc(0x14000, 0);
    g_PartyDataBuffers[1] = HeapAlloc(0x14000, 0);
    g_PartyDataBuffers[2] = HeapAlloc(0x14000, 0);
    HeapPinBlock(g_PartyDataBuffers[0]);
    HeapPinBlock(g_PartyDataBuffers[1]);
    HeapPinBlock(g_PartyDataBuffers[2]);
}

void FieldPartyFreeSkinDataBuffers(void) {
    HeapUnpinBlock(g_PartyDataBuffers[0]);
    HeapUnpinBlock(g_PartyDataBuffers[1]);
    HeapUnpinBlock(g_PartyDataBuffers[2]);
    HeapFree(g_PartyDataBuffers[0]);
    HeapFree(g_PartyDataBuffers[1]);
    HeapFree(g_PartyDataBuffers[2]);
}

extern s32 D_800ADB9C;
extern int g_FieldSystemMode;
extern void FieldClearAndSwapOTag(void);
extern void FieldPollControllers(void);
extern void func_80281B00(void* arg0);
extern u8 D_8006FB80;
extern void func_800A31E8(void);

void func_80077DAC(void) {
    D_800ADB9C = Vsync(1);
    FieldClearAndSwapOTag();
    FieldPollControllers();

    if (g_FieldSystemMode == 0) {
        func_80281B00(&D_8006FB80);
    }

    func_800A31E8();
}

extern FieldActor* volatile g_FieldActors;
extern s32 D_800ADBD0;
extern s16 D_800B2344;
extern s32 g_PlayerActorIndex;

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): field_is_encounter_held_by_jump (src/decomp/src/field/field.c:2865). */
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
#ifndef XM_S_field_is_encounter_held_by_jump
#define XM_S_field_is_encounter_held_by_jump
s32 xm_field_is_encounter_held_by_jump(void) __asm__("func_80077E10");
#endif
#ifndef XM_S_field_encounter_battle_pending
#define XM_S_field_encounter_battle_pending
extern s32 xm_field_encounter_battle_pending __asm__("D_800ADBD0");
#endif
#ifndef XM_S_field_view
#define XM_S_field_view
extern xm_FieldView xm_field_view __asm__("g_CameraEye");
#endif
s32 func_80077E10(void) {
    if (xm_field_encounter_battle_pending == 1 && xm_field_work.jump_mode == 0
        && (xm_field_view.components.descriptors[xm_field_work.controlled].actor->flags & 0x800)) {
        return -1;
    }
    return 0;
}
#endif
#else
s32 func_80077E10(void) {
    if (D_800ADBD0 == 1 && D_800B2344 == 0) {
        /* FieldActor is 0x5C on the PSX.  Its +0x4C pActorData member is a
         * 32-bit PSX pointer slot; reconstruct it before reading ActorData. */
        ActorData* pActorData =
            (ActorData*)(uintptr_t)g_FieldActors[g_PlayerActorIndex].pActorData;
        u32 flags = pActorData->scriptFlags.flags;
        return -(s32)((flags & 0x800) != 0);
    }
    return 0;
}
#endif /* XENO_PC_PORT */

/* ---- FieldMain: field game-state driver (Phase C gateway) -------------------
 * Functional decompile (port-first; not yet byte-matched). Control flow mirrors
 * asm/field/nonmatchings/main/main/FieldMain.s 1:1 via labels/gotos; every call
 * is preserved. The native dependency audit identifies any call that still
 * resolves to generated storage. g_FieldSystemMode comes from D_80010000
 * (-1 in retail rodata) exactly as
 * the original does -> SYSTEM_MODE_CD_ROM, which skips the mode-0-only `break 1`
 * and the raw-0x80280000 dev read. Port-only instrumentation is guarded. */
#ifdef XENO_PC_PORT
extern int printf(const char*, ...);
#include <stdio.h>
#include <stdlib.h>
#define FM_LOG(...) printf("[FieldMain] " __VA_ARGS__)
/* Isolated integrator diagnostic; inert unless explicitly requested. */
static void field_transition_stage(const char *stage, int exitCode) {
    const char *enabled = getenv("XENO_FIELD_TRANSITION_DIAG");
    if (enabled && enabled[0] == '1') {
        fprintf(stderr, "[field-transition] exit=%d %s\n", exitCode, stage);
        fflush(stderr);
    }
}
#define FT_STAGE(stage) field_transition_stage(stage, exitCode)
#else
#define FM_LOG(...) ((void)0)
#define FT_STAGE(stage) ((void)0)
#endif

extern int g_FieldSystemMode;
extern int g_FieldCurRenderContextIndex;
#ifndef SYSTEM_MODE_PC_HDD
#define SYSTEM_MODE_PC_HDD 0
#define SYSTEM_MODE_CD_ROM 1
#endif
extern int D_80010000;
extern void *g_pGameState;
extern u8 g_GameState[];
extern s32 g_PlayerActorIndex;
extern FieldActor *volatile g_FieldActors; /* 0x5C stride; +0x4C = pActorData */
extern u8 g_FieldEffects[];
extern u8 g_FieldDefaultParticleBanks[];
extern s32 g_GamePartySkinsInitialized;
extern s32 g_GamePartyMemberSkins[];

/* word globals */
extern s32 D_8004F370, D_8004F2F8, D_8004F324, D_8004F320, D_8004F308, D_8004F338;
extern s32 D_8004F348, D_8004F334, D_8004F310, D_8004F31C, D_8004F354, D_8004F358;
extern s32 D_8004F378, D_8004F37C, D_8004F380;
extern s32 D_800ADBE8, D_800ADBE4, D_800ADBE0, D_800ADBDC, D_800ADBD8, D_800ADBD4, D_800ADBD0;
extern s32 D_800ADBEC, D_800ADBC4, D_800ADB90, D_800ADB68, D_800ADB64, D_800ADB70, D_800ADB18;
extern s32 D_800ADC10, D_800ADC04, D_800ADB60, D_800ADB34, D_800ADB7C;
extern s32 D_800AFC78;
/* pointer globals */
extern void *D_80059560, *D_800595AC, *D_8006251C, *D_80062524, *D_800ADB30, *D_8005A4E0, *D_80062528;
/* halfword globals */
extern u16 D_8006F94E, D_8006F954, D_8006F950, D_8006F956, D_800C3900, D_800AFE9C, D_800C3908, D_800B236C;
extern s16 D_800B2290;
/* byte globals */
extern u8 D_800594D0, D_8005954C, D_800B2358, D_800B2355, D_800ADB04, D_800B21D0, D_80059171, D_80059179;
extern s32 D_80059488;

extern int ControllerGetType();
extern int func_80078BC8();
extern int func_80077E10();
extern int func_8001B484();
extern int ArchiveDataSync(void);
extern void FieldRenderSyncAndFlush();
extern void func_80085890(), func_802811EC(), func_80078D44(), func_80077DAC(), func_8007554C();
extern void func_800A5924(), func_8001B66C(), func_80085B20(), func_800A3F4C(), func_8003A89C();
extern void func_8007FFE8(), func_800A30FC(), func_800A5C40(), func_800798BC(), func_800ABA98();
extern void func_800A7C58(), func_800799D4(), func_800ACE90(), func_80078B5C(), func_800A91F0();
extern void func_800A31E8(), func_800864F0(), func_80085988(), func_8007954C();
extern void FieldInitializeDefaultParticleBanks(), FieldInitializeControllers(), FieldLoadUITextures();
extern void GamePartySyncSkinData(), GamePartySyncStreamedData(), GraphicsDrawPauseLetters();
extern void FieldPollControllers(), SoundMuteAllSpuChannels(), SoundEnableAllSpuChannels();
extern void GameCheckAndHandleSoftReset(), FieldParticlesFreeAll(), FieldFree();
extern void FieldScriptMemoryWriteU16();
#ifdef XENO_PC_PORT
extern void PcPort_QuickCheckpointSetFieldActive(int active);
extern int PcPort_QuickCheckpointPoll(void);
extern void PcPort_QuickCheckpointCommitLoad(void);
extern void PcPort_QuickCheckpointRestorePlayer(void);
extern void ChangeGameState(unsigned int state);
extern void MainLoop(int errorCode) __attribute__((noreturn));
#endif

/* g_FieldActors[idx].pActorData->flags. pActorData is a u32 slot (== pointer
 * size on MIPS, so matching-safe). On the 64-bit port the u32 is widened to a
 * host pointer via uintptr_t before dereferencing. sizeof(FieldActor)==0x5C on
 * both builds, so the raw offset 0x4C is correct. The native null return is a
 * fail-closed ownership guard; retail assumes func_80080F44 published a valid
 * actor-data pointer, so the guard remains an audited divergence. */
#ifdef XENO_PC_PORT
#define FIELD_ACTOR_FLAGS(idx) \
    ({ u32 _p = *(u32*)((u8*)g_FieldActors + (idx) * 0x5C + 0x4C); \
       _p ? *(s32*)(uintptr_t)_p : 0; })
#else
#define FIELD_ACTOR_FLAGS(idx) \
    (*(s32*)(uintptr_t)(*(u32*)((u8*)g_FieldActors + (idx) * 0x5C + 0x4C)))
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/main", FieldMain);
#else
void FieldMain(void) {
    int exitCode = 0;   /* s0 at teardown (set 0/1/2/3 on the exit paths) */
    int savedSound;     /* s0 reuse: D_80059488 save/restore in the wait loops */
    int waterInit = 0;  /* s4 */
    int s5flag = 0;     /* s5: one-time D_800AFC78 latch */

    FM_LOG("entry. D_80010000=0x%08x\n", (unsigned)D_80010000);
    if (D_80010000 == -1) {
        g_FieldSystemMode = SYSTEM_MODE_CD_ROM;
    } else {
        g_FieldSystemMode = SYSTEM_MODE_PC_HDD;
    }
    FM_LOG("g_FieldSystemMode=%d (%s)\n", g_FieldSystemMode,
           g_FieldSystemMode == SYSTEM_MODE_CD_ROM ? "CD_ROM" : "PC_HDD");
    FieldRenderSyncAndFlush();
    if (g_FieldSystemMode == 0) {
        DrawSyncCallback(FieldUpdateDeltaTime);
    }
    D_8006251C = D_80059560;
    D_80062524 = D_800595AC;
    HeapChangeCurrentUser(HEAP_USER_YOSI, NULL);

    if (g_FieldSystemMode == 0 && D_8004F370 == 0) {
        /* PC-HDD dev path (not taken in the port; raw 0x80280000 is a PSX addr). */
        ArchiveSetIndex(4, 0);
        FM_LOG("dev field read: archive id 0xAD -> 0x80280000\n");
        ArchiveReadFileToBuffer(0xAD, (s32)0x80280000, 0, 0x80);
        ArchiveCdDataSync(0);
        FieldRenderSyncAndFlush();
    }

    if (!g_GamePartySkinsInitialized) {
        g_GamePartyMemberSkins[2] = 0xFF;
        g_GamePartyMemberSkins[1] = 0xFF;
        g_GamePartyMemberSkins[0] = 0xFF;
    }
    func_80085890(0);
    FieldPartyAllocateSkinDataBuffers();
    D_800ADBE8 = -1; D_800ADBE4 = -1; D_800ADBE0 = -1; D_800ADBDC = -1; D_800ADBD8 = -1;
    D_8004F358 = 0; D_8004F354 = 0;
    D_800ADC10 = 0; D_800ADB60 = 0; D_800ADB34 = 0; D_800ADB7C = 0;
    D_800ADC04 = 2;
    if (g_FieldSystemMode == 0) {
        func_802811EC();
    }
    FM_LOG("FieldInitializeControllers\n");
    FieldInitializeControllers();

    g_pGameState = &g_GameState;
    g_GameSceneMapNum = D_8006F94E;
    *(s16*)(g_GameState + 0x1932) = D_8006F954;
    *(s16*)(g_GameState + 0x1938) = D_8006F950 >> 9;
    if (D_8004F2F8 == 0) {
        D_800594D0 = 0;
        D_8004F324 = 0xFF;
    } else {
        D_8004F324 = D_8006F956;
    }
    FM_LOG("g_pGameState=%p g_GameSceneMapNum=%d\n", g_pGameState, (int)g_GameSceneMapNum);
    if (g_FieldSystemMode == SYSTEM_MODE_CD_ROM) {
        *(s16*)((u8*)g_pGameState + 0x1980) = 1;
        FieldScriptMemoryWriteU16(0x50, 1);
    }
    FM_LOG("FieldLoadUITextures\n");
    FieldLoadUITextures();
    D_8004F320 = 0;
    GamePartySyncSkinData();
    GamePartySyncStreamedData();
    D_800ADB30 = HeapAlloc(4, 1);
    if (g_FieldSystemMode == 0) {
        /* mode-0 only: the original has a `break 1` trap here. */
        FieldInitializeDefaultParticleBanks(g_PlayerActorIndex);
        *(s16*)(g_FieldDefaultParticleBanks + 0) = 1;
        *(s16*)(g_FieldDefaultParticleBanks + 6) = 0x10;
    }
    func_80078D44();
#ifdef XENO_PC_PORT
    PcPort_QuickCheckpointRestorePlayer();
    PcPort_QuickCheckpointSetFieldActive(1);
#endif
    D_800ADB04 = 1;
    FM_LOG("entering main loop\n");

    for (;;) {  /* .L80078174 */
#ifdef XENO_PC_PORT
        if (PcPort_QuickCheckpointPoll()) {
            exitCode = 4;
            goto teardown;
        }
#endif
        /* wait for a controller if none present */
        if (ControllerGetType(0) == 0) {
            savedSound = D_80059488;
            SoundMuteAllSpuChannels();
            GraphicsDrawPauseLetters(0x88, (((g_FieldCurRenderContextIndex + 1) & 1) << 8) | 0x64);
            do {
                DrawSync(0); Vsync(2); FieldPollControllers(); GameCheckAndHandleSoftReset();
            } while (ControllerGetType(0) == 0);
            SoundEnableAllSpuChannels();
            D_80059488 = savedSound;
        }
        /* paused/menu hold */
        if ((D_800C3900 & 0x800) && !(D_800AFE9C & 0x40) && !D_800B2358) {
            savedSound = D_80059488;
            SoundMuteAllSpuChannels();
            GraphicsDrawPauseLetters(0x88, (((g_FieldCurRenderContextIndex + 1) & 1) << 8) | 0x64);
            do {
                DrawSync(0); Vsync(2); FieldPollControllers(); GameCheckAndHandleSoftReset();
            } while (D_800C3900 & 0x800);
            SoundEnableAllSpuChannels();
            D_80059488 = savedSound;
        }
        if (g_FieldSystemMode == SYSTEM_MODE_CD_ROM) {
            FieldScriptMemoryWriteU16(0x50, 1);
        }
        GameCheckAndHandleSoftReset();
        func_80077DAC();
        func_8007554C();
        func_800A5924();

        /* render-context dispatch (g_FieldCurRenderContextIndex == 1) */
        if (g_FieldCurRenderContextIndex == 1 && D_800ADBDC == 0
            && func_80078BC8() == 0 && func_80077E10() == 0) {
            if (D_8004F334 != -1) {
                HeapUnpinBlock(D_8005A4E0);
                HeapFree(D_8005A4E0);
            }
            if (s5flag == 0) {
                D_800AFC78 = (s32)D_8004F324;
                s5flag = 1;
            }
            func_8007FFE8();
            if (D_800ADBD0 == 1) {
                D_8005954C = (u8)D_800B2355;
                D_800AFC78 = (s32)D_8004F324;
                if (D_8004F338 != D_800B2290) {
                    if (D_8004F338 != -1) {
                        D_8004F348 = 1;
                    }
                    func_8001B66C();
                    D_8004F308 = -1;
                    D_8004F324 = D_800B2290;
                    func_80085B20(D_800B2290, 1);
                }
                D_800ADBD0 = 0;
                D_800ADBD4 = 1;
                goto after_ctx;
            } else {
                if (D_800ADB18 == 0) {
                    g_GamePartySkinsInitialized++;
                    func_800A3F4C();
                }
                exitCode = 0;
                if (D_800ADBD4 == 1) {
                    func_8003A89C(D_80062528, 0x7F, 0);
                }
                D_800ADBD4 = 0;
                goto teardown;
            }
        }

    after_ctx:  /* .L80078494 */
        if (D_800ADBEC == 0 && D_8004F308 == 0 && D_800ADBC4 == 0xFF && D_800ADB90 == 0
            && func_8001B484((g_GameSceneMapNum & 0xFFF) << 1, 0) == 0
            && ArchiveDataSync() == 0
            && *(s16*)(g_FieldEffects + 0xA0) == 0) {
            D_800ADB04 = 0;
            func_800A30FC();
            ArchiveCdDataSync(0);
            func_800A5C40();
            ControllerResetState();
            D_800ADB04 = 1;
        }

        /* .L80078558: render-context teardown ladder (exit codes 1/2/3) */
        if (g_FieldCurRenderContextIndex == 1) {
            if (D_800ADBE4 == 0 && func_80078BC8() == 0) {
                ArchiveCdDataSync(0);
                exitCode = 1;
                if (D_8004F334 != -1) {
                    HeapUnpinBlock(D_8005A4E0);
                    HeapFree(D_8005A4E0);
                }
                goto teardown;
            }
            if (g_FieldCurRenderContextIndex == 1) {  /* re-checked in asm */
                if (D_800ADBE8 == 0 && func_80078BC8() == 0) {
                    ArchiveCdDataSync(0);
                    if (D_8004F334 != -1) {
                        HeapUnpinBlock(D_8005A4E0);
                        HeapFree(D_8005A4E0);
                    }
                    D_8004F310++;
                    exitCode = 2;
                    func_800A3F4C();
                    goto teardown;
                }
                if (g_FieldCurRenderContextIndex == 1 && D_800ADBD8 == 0 && func_80078BC8() == 0) {
                    ArchiveCdDataSync(0);
                    if (D_8004F334 != -1) {
                        HeapUnpinBlock(D_8005A4E0);
                        HeapFree(D_8005A4E0);
                    }
                    exitCode = 3;
                    func_8001B66C();
                    goto teardown;
                }
            }
        }

        /* .L800786F4: per-frame field update/render */
        if (g_FieldSystemMode == 0) {
            u16 f3908 = D_800C3908;
            if (f3908 & 0x40) D_8004F378 = (D_8004F378 + 1) & 1;
            if (f3908 & 0x10) D_8004F37C = (D_8004F37C + 1) & 1;
            if (f3908 & 0x80) D_8004F380 = (D_8004F380 + 1) & 1;
            if ((D_800AFE9C & 0x40) && (D_800C3900 & 0x100) && D_800ADBEC == -1
                && D_8004F308 == 0 && D_800ADB34 == 0) {
                g_GameSceneMapNum = 0;
                D_800ADBEC = 0;
                FieldScriptMemoryWriteU16(2, 0);
            }
        }

        /* .L80078810 */
        if (D_800ADBD8 == -1 && D_800ADBDC == -1 && D_800ADBE4 == -1
            && func_80078BC8() == 0 && D_800ADBEC == -1) {
            u16 fe9c = D_800AFE9C;
            if ((fe9c & 3) == 0) {
                waterInit = 0;
            }
            if ((fe9c & 1) && D_800ADB68 == 1 && (fe9c & 2) && waterInit == 0) {
                func_800798BC();
                waterInit = 1;
                if (D_800ADB64 == 0xFF && (FIELD_ACTOR_FLAGS(g_PlayerActorIndex) & 0x1800) == 0
                    && D_80059179 == 0) {
                    func_800ACE90();
                }
            }
        }
        if ((D_800C3900 & 0x100) && D_800ADBEC == -1 && D_800ADB68 == 1) {
            func_800ABA98();
        }
        if (D_800ADB70 && g_FieldCurRenderContextIndex == 1) {
            func_800A7C58();
            D_800ADB70 = 0;
        }
        if (D_800ADB64 != 0xFF && g_FieldCurRenderContextIndex == 0
            && (FIELD_ACTOR_FLAGS(g_PlayerActorIndex) & 0x1800) == 0) {
            func_8007FFE8();
            func_800799D4();
            D_800ADB64 = 0xFF;
        }
        if ((D_800C3900 & 0x10) && !D_800B21D0 && D_800ADB64 == 0xFF && D_800ADB68 == 1) {
            D_800ADB64 = 0x80;
            D_80059171 = (u8)D_800B236C;
        }

        /* .L80078AAC */
        func_80078B5C();
    }

teardown:  /* .L80078ABC */
    FM_LOG("teardown exitCode=%d\n", exitCode);
#ifdef XENO_PC_PORT
    FT_STAGE("before PcPort_QuickCheckpointSetFieldActive(0)");
    PcPort_QuickCheckpointSetFieldActive(0);
    FT_STAGE("after PcPort_QuickCheckpointSetFieldActive(0)");
#endif
    FT_STAGE("before func_800798BC()");
    func_800798BC();
    FT_STAGE("after func_800798BC()");
    FT_STAGE("before func_800A91F0()");
    func_800A91F0();
    FT_STAGE("after func_800A91F0()");
    FT_STAGE("before func_800A31E8()");
    func_800A31E8();
    FT_STAGE("after func_800A31E8()");
    FT_STAGE("before FieldParticlesFreeAll()");
    FieldParticlesFreeAll();
    FT_STAGE("after FieldParticlesFreeAll()");
    FT_STAGE("before func_800864F0()");
    func_800864F0();
    FT_STAGE("after func_800864F0()");
    FT_STAGE("before func_8007FFE8()");
    func_8007FFE8();
    FT_STAGE("after func_8007FFE8()");
    FT_STAGE("before DrawSync(0)");
    DrawSync(0);
    FT_STAGE("after DrawSync(0)");
    FT_STAGE("before Vsync(0)");
    Vsync(0);
    FT_STAGE("after Vsync(0)");
    FT_STAGE("before FieldFree()");
    FieldFree();
    FT_STAGE("after FieldFree()");
    FT_STAGE("before FieldPartyFreeSkinDataBuffers()");
    FieldPartyFreeSkinDataBuffers();
    FT_STAGE("after FieldPartyFreeSkinDataBuffers()");
    FT_STAGE("before func_80085988()");
    func_80085988();
    FT_STAGE("after func_80085988()");
    D_8004F31C = 0;
    FT_STAGE("before HeapFree(D_800ADB30)");
    HeapFree(D_800ADB30);
    FT_STAGE("after HeapFree(D_800ADB30)");
#ifdef XENO_PC_PORT
    if (exitCode == 4) {
        FT_STAGE("before PcPort_QuickCheckpointCommitLoad()");
        PcPort_QuickCheckpointCommitLoad();
        FT_STAGE("after PcPort_QuickCheckpointCommitLoad()");
        FT_STAGE("before ChangeGameState(1)");
        ChangeGameState(1);
        FT_STAGE("after ChangeGameState(1)");
        FT_STAGE("before MainLoop(0)");
        MainLoop(0);
        FT_STAGE("after MainLoop(0)");
    }
#endif
    func_8007954C(exitCode);
}
#endif /* XENO_PC_PORT */
