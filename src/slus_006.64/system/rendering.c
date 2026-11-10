#include "common.h"
#include "field/actor.h"
#include "psyq/libgpu.h"
#include "system/memory.h"
#ifdef XENO_PC_PORT
#include <assert.h>
#include <stdio.h>   /* fflush for the env-gated player-render census */
#include <stdlib.h>
#include "guest_prim_link.h"
#include "fei_hd2d.h"
#define RENDER_ADD_PRIM(ot, prim) PcPort_AddPrimDomainAware((ot), (prim))
/* TEMP-DIAG (world-map white-quad hunt): per-sprite-poly trace. Revert. */
extern int PcPort_WorldCaptureCurFrame(void);
static int WmSpriteDiagActive(int *f0, int *f1)
{
    static int s_init = 0;
    static int s_on = 0;
    static int s_f0 = 0;
    static int s_f1 = 0;
    if (!s_init) {
        const char *e = getenv("XENO_WM_SPRITE_DIAG");
        const char *a = getenv("XENO_WM_SPRITE_DIAG_F0");
        const char *b = getenv("XENO_WM_SPRITE_DIAG_F1");
        s_on = (e != NULL && e[0] != '\0' && e[0] != '0');
        s_f0 = a != NULL ? atoi(a) : 0;
        s_f1 = b != NULL ? atoi(b) : 0;
        s_init = 1;
    }
    if (f0)
        *f0 = s_f0;
    if (f1)
        *f1 = s_f1;
    return s_on;
}
#else
/* <assert.h> is unavailable under the matching build's -nostdinc MIPS
 * preprocessor. The assert(0) below marks an unimplemented path in a function
 * not yet byte-matched, so a no-op assert compiles safely there. (uintptr_t
 * comes from include/types.h for both builds.) */
#define assert(x) ((void)0)
#define RENDER_ADD_PRIM(ot, prim) addPrim((ot), (prim))
#endif

// Rendering-related stuff

#ifdef XENO_PC_PORT
static int XenoFieldDiagEnabled(void) {
    static int s_enabled = -1;

    if (s_enabled < 0) {
        const char* env = getenv("XENO_FIELD_DIAG");
        s_enabled = (env != NULL && env[0] != '\0' && env[0] != '0');
    }
    return s_enabled;
}

/* DIAGNOSTIC / TEST TOOLING (XENO_PLAYER_RENDER_DIAG=1).
 *
 * "The player character stops rendering" has exactly one render-level
 * signature: func_8001E3D8 either is not called for the player's sprite at
 * all, or is called and emits no quad because frameCount ((flags40 >> 2) &
 * 0x3F, written by the frame builders func_8001DAE8 / func_8001D53C) is zero,
 * or because the per-frame work buffer is exhausted.  Log a transition-only
 * trace for the PLAYER actor's sprite plus a periodic heartbeat, so a menu
 * round trip yields a number rather than a screenshot judgement.  Inert unless
 * armed. */
static int XenoPlayerRenderDiagEnabled(void) {
    static int s_enabled = -1;

    if (s_enabled < 0) {
        const char* env = getenv("XENO_PLAYER_RENDER_DIAG");
        s_enabled = (env != NULL && env[0] == '1');
    }
    return s_enabled;
}
#endif

#ifdef XENO_PC_PORT
/* Port-side copies of retail .data tables (0x8004FAB8/0x8004FAF8) and
 * scratch (0x8004FB98); only the port bodies use them, so they stay out of
 * the retail link. */
static const s16 s_TPageCoords8004FAB8[] = {
    0x300, 0x000,
    0x340, 0x000,
    0x380, 0x000,
    0x3C0, 0x000,
    0x300, 0x100,
    0x340, 0x100,
    0x380, 0x100,
    0x3C0, 0x100,
};

static const u16 s_DirectionMask8004FAF8[] = {
    0x0001,
    0x0002,
    0x0004,
    0x0008,
    0x0010,
    0x0020,
    0x0040,
    0x0080,
};

static SVECTOR s_QuadWork8004FB98[4];
#endif

#ifdef XENO_PC_PORT
static s16 ScaleSpriteFrameByte(u8 value, s16 scale) {
    s32 result = value * scale;

    if (result < 0) {
        result += 0xFFF;
    }

    return result >> 12;
}
#endif

#ifdef XENO_PC_PORT
static s32 ScaleSpriteFrameByte32(s32 value, s32 scale) {
    s32 result = (s32)((u32)value * (u32)scale);

    if (result < 0) {
        result += 0xFFF;
    }

    return result >> 12;
}

/* Retail RotMatrix (8003F738) with its exact Q12 stage rounding; the host
 * trig-identity RotMatrix differs. Macro rcos is sine, rsin is cosine (see
 * AnimationC1RotMatrix in animation_scripts.c). */
static void Retail8003F738RotMatrix(const SVECTOR* angles, MATRIX* matrix) {
    s32 sx = rcos((u16)angles->vx & 0xFFF);
    s32 cx = rsin((u16)angles->vx & 0xFFF);
    s32 sy = rcos((u16)angles->vy & 0xFFF);
    s32 cy = rsin((u16)angles->vy & 0xFFF);
    s32 sz = rcos((u16)angles->vz & 0xFFF);
    s32 cz = rsin((u16)angles->vz & 0xFFF);
    s32 nsy = -sy;
    s32 t0 = (cz * nsy) >> 12;
    s32 t0b = (sz * nsy) >> 12;

    matrix->m[0][0] = (s16)((cz * cy) >> 12);
    matrix->m[1][0] = (s16)(((sz * cx) >> 12) - ((t0 * sx) >> 12));
    matrix->m[2][0] = (s16)(((t0 * cx) >> 12) + ((sz * sx) >> 12));
    matrix->m[0][1] = (s16)((-(sz * cy)) >> 12);
    matrix->m[1][1] = (s16)(((cz * cx) >> 12) + ((t0b * sx) >> 12));
    matrix->m[2][1] = (s16)(((cz * sx) >> 12) - ((t0b * cx) >> 12));
    matrix->m[0][2] = (s16)sy;
    matrix->m[1][2] = (s16)((-(cy * sx)) >> 12);
    matrix->m[2][2] = (s16)((cy * cx) >> 12);
}
#endif

/* Retail8001D4E8: allocate and initialize eight direction records once. */
extern void func_800234AC(void* pSpriteData);
void func_8001D4E8(void* pSpriteData) {
    u8* sprite = pSpriteData;
    if (*(u32*)((u8*)(uintptr_t)*(u32*)(sprite + 0x20) + 0x34) == 0) {
        void* directions = HeapAlloc(0x40, 0);
        /* Retail reloads the model after HeapAlloc returns. */
        u8* model = (u8*)(uintptr_t)*(u32*)(sprite + 0x20);
        *(u32*)(model + 0x34) = (u32)(uintptr_t)directions;
        func_800234AC(sprite);
    }
}

extern void func_800251C8(u_long* addr, int x, int y, int width, int height);
extern void func_800234AC(void* pSpriteData);
extern void func_8001F530(void* arg0, s32 arg1);

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): sprite_build_cell_frame (src/decomp/src/resident/sprite.c:477). */
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
#ifndef XM_T_TexturePosition
#define XM_T_TexturePosition
typedef struct {
    s16 x, y;
} TexturePosition;
#endif
#ifndef XM_T_SpriteImageSize
#define XM_T_SpriteImageSize
typedef struct {
    u16 width;
    u16 height;
} SpriteImageSize;
#endif
#ifndef XM_T_SpriteSequencer
#define XM_T_SpriteSequencer
typedef struct {
    s32 word0;
    s32 word4;
    s32 word8;
    s16 halfc;
    SpriteImageSize size;  
    u8 unknown12[2];
    s16 actor;             
    u8 unknown16[2];
    u16 *buffer;           
} SpriteSequencer;
#endif
#ifndef XM_S_sprite_build_cell_frame
#define XM_S_sprite_build_cell_frame
void xm_sprite_build_cell_frame(Sprite *sprite, s32 frame, SpriteSource *source) __asm__("func_8001D53C");
#endif
#ifndef XM_S_heap_alloc
#define XM_S_heap_alloc
void *xm_heap_alloc(s32 size, s32 mode) __asm__("HeapAlloc");
#endif
#ifndef XM_S_sprite_clear_group_entries
#define XM_S_sprite_clear_group_entries
void xm_sprite_clear_group_entries(Sprite *sprite) __asm__("func_800234AC");
#endif
#ifndef XM_S_sprite_cell_page_positions
#define XM_S_sprite_cell_page_positions
extern TexturePosition xm_sprite_cell_page_positions[8] __asm__("D_8004FAB8");
#endif
#ifndef XM_S_GetTPage
#define XM_S_GetTPage
u_short xm_GetTPage(int tp, int abr, int x, int y) __asm__("GetTPage");
#endif
#ifndef XM_S_GetClut
#define XM_S_GetClut
u_short xm_GetClut(int x, int y) __asm__("GetClut");
#endif
void func_8001D53C(Sprite *sprite, s32 frame, SpriteSource *source) {
    u16 *table;
    u8 *record;
    u16 *cells;
    u16 v_base;
    u16 blend;
    u32 colour;
    s32 count;
    u8 wide;
    Sprite *self;
    u8 *p;
    SpritePart *parts;
    s32 group;
    s32 i;
    u8 control;
    u8 *cell;
    s32 kind;
    u16 u_base;
    s16 rate;
    s32 palette;
    TexturePosition *page;
    TexturePosition *pages;

    table = source->frames;
    record = (u8 *)(table[frame] + (s32)table);
    cells = (u16 *)(record + 4);
    wide = *record & 0x80;
    count = *record & 0x3F;
    p = record + (count * 2 + 4);
    parts = sprite->renderer->parts[1];
    self = sprite;
    sprite->height = record[3] * sprite->scale / 4096;
    sprite->extent_depth = record[1] * sprite->scale / 4096;
    v_base = source->origin.vy & 0xFF;
    blend = sprite->render.bits.blend;
    group = 4;
    colour = *(u32 *)&sprite->red;
    for (i = 0; i != count; i++, p += 3) {
        parts[i].byte9 = 0;
        parts[i].byte8 = 0;
        parts[i].flags &= ~0x20;
    next:
        control = *p;
        if (control & 0x80) {
            p++;
            if (control & 0x40) {
                group = control & 7;
                if (sprite->renderer->pointer34 == NULL) {
                    sprite->renderer->pointer34 = xm_heap_alloc(0x40, 0);
                    xm_sprite_clear_group_entries(sprite);
                }
                if (control & 0x20) {
                    sprite->renderer->pointer34[group].byte0 = *p++;
                    sprite->renderer->pointer34[group].byte1 = *p++;
                }
                if (control & 0x10) {
                    s16 depth = *p++ << 4;

                    sprite->renderer->pointer34[group].half6 = depth;
                } else {
                    sprite->renderer->pointer34[group].half6 = 0;
                }
            } else {
                if (control & 4) {
                    parts[i].flags |= 0x20;
                }
                if (control & 1) {
                    parts[i].byte8 = *p++;
                }
                if (control & 2) {
                    parts[i].byte9 = *p++;
                }
            }
            goto next;
        }
        cell = (u8 *)(*cells + (s32)table);
        kind = *cell;
        cells++;
        if (kind & 1) {
            parts[i].flags |= 8;
            u_base = (source->origin.vx & 0x3F) >> 1;
        } else {
            parts[i].flags &= ~8;
            u_base = (source->origin.vx & 0x3F) >> 2;
        }
        rate = (*p >> 4) & 3;
        palette = *p & 0xF;
        parts[i].colour = colour;
        if (rate != 0 || (rate = blend) != 0) {
            rate--;
            ((u8 *)&parts[i].colour)[3] |= 2;
        }
        if ((kind >> 4) & 1) {
            TexturePosition *resident_pages;
            TexturePosition *resident_page;

            cell++;
            kind |= *cell << 8;
            resident_pages = xm_sprite_cell_page_positions;
            resident_page = (TexturePosition *)(((kind << 1) & 0x1C) + (s32)resident_pages);
            parts[i].tpage = xm_GetTPage(kind & 1, rate, resident_page->x, resident_page->y);
            parts[i].clut = xm_GetClut((kind >> 1) & 0xF0, ((kind >> 9) & 0xF) + 0x1CC);
        } else {
            if (self->frame_bits.sequencer_owned == 1 &&
                (pages = (TexturePosition *)((SpriteSequencer *)self->sequencer)->buffer) != NULL) {
                page = (TexturePosition *)((kind << 1 & 0x1C) + (s32)pages);
                v_base = page->y & 0xFF;
                u_base = (page->x & 0x3F) >> 2;
                parts[i].tpage = getTPage(kind & 1, rate, page->x, page->y);
            } else {
                parts[i].tpage = getTPage(kind & 1, rate, source->origin.vx + ((kind << 5) & 0x1C0), source->origin.vy);
            }
            parts[i].clut = getClut(source->clut_x + palette * 16, source->clut_y);
        }
        parts[i].flags = (parts[i].flags & ~7) | group;
        parts[i].u = u_base + cell[1];
        parts[i].v = v_base + cell[2];
        parts[i].w = cell[3];
        parts[i].h = cell[4];
        parts[i].flags = (parts[i].flags & ~0x10) | ((*p >> 2) & 0x10);
        if (wide) {
            parts[i].x = p[1] | ((s8)p[2] << 8);
            parts[i].y = p[3] | ((s8)p[4] << 8);
            p += 2;
        } else {
            parts[i].x = (s8)p[1];
            parts[i].y = (s8)p[2];
        }
    }
    sprite->flags = (sprite->flags & ~0xFC) | ((i & 0x3F) << 2);
}
#endif
#else
void func_8001D53C(void* pSpriteData, u16 frameIndex, void* pAnimPackage) {
    u8* pData = pSpriteData;
    u8* pPackage = pAnimPackage;
    u8* pFrames = (u8*)(uintptr_t)*(u32*)(pPackage + 0x0);
    u8* pFrame = pFrames + *(u16*)(pFrames + frameIndex * 2);
    u8 header = pFrame[0];
    s32 hasWideOffsets = header & 0x80;
    s32 partCount = header & 0x3F;
    u8* pFrameOffsets = pFrame + 0x4;
    u8* pStream = pFrame + partCount * 2 + 0x4;
    u8* pBase = (u8*)(uintptr_t)*(u32*)(pData + 0x20);
    u8* pPrim = (u8*)(uintptr_t)*(u32*)(pBase + 0x30);
    s32 baseClutY = *(u8*)(pPackage + 0x6);
    s32 spriteMode = (*(u8*)(pData + 0x3C) >> 5);
    u32 colorAndCode = *(u32*)(pData + 0x28);
    s32 directionIndex = 4;
    s32 i;

    *(s16*)(pData + 0x36) = ScaleSpriteFrameByte(pFrame[3], *(s16*)(pData + 0x2C));
    *(s16*)(pData + 0x38) = ScaleSpriteFrameByte(pFrame[1], *(s16*)(pData + 0x2C));

    for (i = 0; i < partCount; i++, pPrim += 0x18) {
        u8* pTile;
        u8 command;
        u8 texByte;
        u32 tileHeader;
        s32 abr;
        s32 clutXNibble;
        s32 texXOffset;
        u32 flags;

        *(u8*)(pPrim + 0x8) = 0;
        *(u8*)(pPrim + 0x9) = 0;
        *(u32*)(pPrim + 0x14) &= ~0x20;

        while (1) {
            command = *pStream;
            if ((command & 0x80) == 0) {
                break;
            }

            if (command & 0x40) {
                u8* pDirTransforms = (u8*)(uintptr_t)*(u32*)(pBase + 0x34);
                u8* pDirection;

                pStream++;
                directionIndex = command & 0x7;
                if (pDirTransforms == NULL) {
                    pDirTransforms = HeapAlloc(0x40, 0);
                    *(u32*)(pBase + 0x34) = (u32)(uintptr_t)pDirTransforms;
                    func_800234AC(pData);
                }

                pDirection = pDirTransforms + directionIndex * 8;
                if (command & 0x20) {
                    pDirection[0] = *pStream++;
                    pDirection[1] = *pStream++;
                }

                if (command & 0x10) {
                    *(u16*)(pDirection + 0x6) = (u16)(*pStream++ << 4);
                } else {
                    *(u16*)(pDirection + 0x6) = 0;
                }
            } else {
                pStream++;
                if (command & 0x4) {
                    *(u32*)(pPrim + 0x14) |= 0x20;
                }
                if (command & 0x1) {
                    *(u8*)(pPrim + 0x8) = *pStream++;
                }
                if (command & 0x2) {
                    *(u8*)(pPrim + 0x9) = *pStream++;
                }
            }
        }

        pTile = pFrames + *(u16*)pFrameOffsets;
        pFrameOffsets += 2;
        tileHeader = pTile[0];

        if (tileHeader & 0x1) {
            *(u32*)(pPrim + 0x14) |= 0x8;
            texXOffset = (*(u16*)(pPackage + 0x4) & 0x3F) >> 1;
        } else {
            *(u32*)(pPrim + 0x14) &= ~0x8;
            texXOffset = (*(u16*)(pPackage + 0x4) & 0x3F) >> 2;
        }

        texByte = *pStream;
        abr = (texByte >> 4) & 0x3;
        clutXNibble = texByte & 0xF;
        *(u32*)(pPrim + 0x10) = colorAndCode;

        if (abr == 0) {
            abr = spriteMode;
        }
        if (abr != 0) {
            *(u8*)(pPrim + 0x13) |= 0x2;
            abr--;
        }

        if ((tileHeader >> 4) & 0x1) {
            const s16* pCoords;

            pTile++;
            tileHeader |= pTile[0] << 8;
            pCoords = (const s16*)((const u8*)s_TPageCoords8004FAB8 + ((tileHeader << 1) & 0x1C));
            *(u16*)(pPrim + 0xA) = GetTPage(tileHeader & 0x1, abr, pCoords[0], pCoords[1]);
            *(u16*)(pPrim + 0xC) = GetClut((tileHeader >> 1) & 0xF0,
                                           ((tileHeader >> 9) & 0xF) + 0x1CC);
        } else {
            u8* pTableHolder = (u8*)(uintptr_t)*(u32*)(pData + 0x7C);
            u8* pTexTable = NULL;

            if ((*(u32*)(pData + 0xA8) & 0x1) && pTableHolder != NULL) {
                pTexTable = (u8*)(uintptr_t)*(u32*)(pTableHolder + 0x18);
            }

            if (pTexTable != NULL) {
                u8* pEntry = pTexTable + ((tileHeader << 1) & 0x1C);
                u16 word0 = *(u16*)(pEntry + 0x0);
                u16 word2 = *(u16*)(pEntry + 0x2);
                u32 tpage;

                baseClutY = word2 & 0xFF;
                texXOffset = (word0 & 0x3F) >> 2;
                tpage = ((tileHeader & 0x1) << 7) |
                        ((abr & 0x3) << 5) |
                        (((word2 & 0x100) << 16) >> 20) |
                        ((word0 & 0x3FF) >> 6) |
                        ((word2 & 0x200) << 2);
                *(u16*)(pPrim + 0xA) = tpage;
            } else {
                u16 packageTex = *(u16*)(pPackage + 0x4);
                u16 packageClut = *(u16*)(pPackage + 0x6);
                u32 tpage = ((tileHeader & 0x1) << 7) |
                             ((abr & 0x3) << 5) |
                             (((packageClut & 0x100) << 16) >> 20) |
                             (((packageTex + ((tileHeader << 5) & 0x1C0)) & 0x3FF) >> 6) |
                             ((packageClut & 0x200) << 2);

                *(u16*)(pPrim + 0xA) = tpage;
            }

            *(u16*)(pPrim + 0xC) = (*(u16*)(pPackage + 0xA) << 6) |
                                   (((*(s16*)(pPackage + 0x8) + (clutXNibble << 4)) >> 4) & 0x3F);
        }

        flags = *(u32*)(pPrim + 0x14);
        flags &= ~0x7;
        flags |= directionIndex;
        *(u32*)(pPrim + 0x14) = flags;

        *(u8*)(pPrim + 0x4) = texXOffset + pTile[1];
        *(u8*)(pPrim + 0x5) = baseClutY + pTile[2];
        *(u8*)(pPrim + 0x6) = pTile[3];
        *(u8*)(pPrim + 0x7) = pTile[4];

        flags = *(u32*)(pPrim + 0x14);
        flags &= ~0x10;
        flags |= ((*pStream >> 2) & 0x10);
        *(u32*)(pPrim + 0x14) = flags;

        if (hasWideOffsets) {
            *(s16*)(pPrim + 0x0) = (s16)(pStream[1] | ((s8)pStream[2] << 8));
            *(s16*)(pPrim + 0x2) = (s16)(pStream[3] | ((s8)pStream[4] << 8));
            pStream += 2;
        } else {
            *(s16*)(pPrim + 0x0) = (s8)pStream[1];
            *(s16*)(pPrim + 0x2) = (s8)pStream[2];
        }

        pStream += 3;
    }

    *(u32*)(pData + 0x40) = (*(u32*)(pData + 0x40) & ~0xFC) | ((i & 0x3F) << 2);
}
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/rendering", func_8001DAE8);
#else
void func_8001DAE8(void* pSpriteData, u16 frameIndex, u32 animPackageAddr) {
    u8* pData = pSpriteData;
    u8* pAnimPackage = (u8*)(uintptr_t)animPackageAddr;
    u8* pFrames;
    u16 frameHeader;

    *(u32*)(pData + 0x40) &= ~0xA0000;

    pFrames = (u8*)(uintptr_t)*(u32*)(pAnimPackage + 0x0);
    frameHeader = *(u16*)pFrames;
    if (frameIndex >= ((frameHeader & 0x1FF) + 1)) {
        return;
    }

    if (((*(u32*)(pData + 0x3C) >> 30) & 0x1) != 0) {
        u8* pTransfer = (u8*)(uintptr_t)*(u32*)(pAnimPackage + 0xC);
        u16 count;

        *(u32*)(pData + 0x3C) &= ~0x40000000;
        count = *(u16*)pTransfer;
        if (count != 0) {
            s32 offset = ((count * (*(u16*)(pData + 0x3E) & 0xF0)) << 1) + 4;
            func_800251C8((u_long*)(pTransfer + offset),
                          *(s16*)(pAnimPackage + 0x8),
                          *(s16*)(pAnimPackage + 0xA),
                          count << 4,
                          1);
        }
    }

    if (frameHeader & 0x8000) {
        func_8001D53C(pData, frameIndex, pAnimPackage);
        return;
    }

    {
        u8* pFrame = pFrames + *(u16*)(pFrames + frameIndex * 2);
        u8* pVramData = (u8*)(uintptr_t)*(u32*)(pData + 0x24);
        /* asm 8001DC60-8001DC94: the {+4,+6} halfword pair is copied to a
         * 4-byte stack slot that func_8001F530 rewrites as one word. */
        u16 vramXY[2];
#define vramX vramXY[0]
#define vramYWord vramXY[1]
        u8 header = pFrame[0];
        s32 hasWideOffsets = header & 0x80;
        s32 partCount = header & 0x3F;
        u8* pDescriptors = pFrame + 0x6;
        u8* pStream = pFrame + partCount * 4 + 0x6;
        u8* pBase = (u8*)(uintptr_t)*(u32*)(pData + 0x20);
        u8* pPrim = (u8*)(uintptr_t)*(u32*)(pBase + 0x30);
        s32 baseClutY;
        s32 spriteMode = (*(u8*)(pData + 0x3C) >> 5);
        u32 colorAndCode = *(u32*)(pData + 0x28);
        s32 directionIndex = 4;
        s32 i;

        s32 texUBase4;
        s32 texUBase8;

        vramXY[0] = *(u16*)(pVramData + 0x4);
        vramXY[1] = *(u16*)(pVramData + 0x6);
        if (((*(u32*)(pData + 0x40) >> 13) & 0xF) == 0xE) {
            func_8001F530(vramXY, pFrame[4]);
        }
        /* The V base is the low byte of the (possibly rewritten) Y word. */
        baseClutY = (u8)vramYWord;

        /* asm 8001DD0C-8001DD2C: the part U coordinate is based at the
         * sprite band's texel origin within its 64-word texture page
         * ((vramX & 0x3F) scaled per bpp); computed after the func_8001F530
         * vramX rewrite, once for the whole frame. */
        texUBase4 = (vramX & 0x3F) << 2;
        texUBase8 = (vramX & 0x3F) << 1;

        *(s16*)(pData + 0x36) = ScaleSpriteFrameByte(pFrame[3], *(s16*)(pData + 0x2C));
        *(s16*)(pData + 0x38) = ScaleSpriteFrameByte(pFrame[1], *(s16*)(pData + 0x2C));

        for (i = 0; i < partCount; i++, pPrim += 0x18, pDescriptors += 4) {
            u8* pTile;
            u8 command;
            u16 descriptor1;
            u16 tileHeader;
            s32 texX;
            s32 texU;
            s32 texY;
            s32 width;
            s32 abr;
            s32 clutXNibble;
            u32 flags;

            *(u8*)(pPrim + 0x8) = 0;
            *(u8*)(pPrim + 0x9) = 0;
            *(u32*)(pPrim + 0x14) &= ~0x20;

            while (1) {
                command = *pStream;
                if ((command & 0x80) == 0) {
                    break;
                }

                if (command & 0x40) {
                    u8* pDirTransforms = (u8*)(uintptr_t)*(u32*)(pBase + 0x34);
                    u8* pDirection;

                    pStream++;
                    directionIndex = command & 0x7;
                    if (pDirTransforms == NULL) {
                        pDirTransforms = HeapAlloc(0x40, 0);
                        *(u32*)(pBase + 0x34) = (u32)(uintptr_t)pDirTransforms;
                        func_800234AC(pData);
                    }

                    pDirection = pDirTransforms + directionIndex * 8;
                    if (command & 0x20) {
                        pDirection[0] = *pStream++;
                        pDirection[1] = *pStream++;
                    }

                    if (command & 0x10) {
                        *(u16*)(pDirection + 0x6) = (u16)(*pStream++ << 4);
                    } else {
                        *(u16*)(pDirection + 0x6) = 0;
                    }
                } else {
                    pStream++;
                    if (command & 0x4) {
                        *(u32*)(pPrim + 0x14) |= 0x20;
                    }
                    if (command & 0x1) {
                        *(u8*)(pPrim + 0x8) = *pStream++;
                    }
                    if (command & 0x2) {
                        *(u8*)(pPrim + 0x9) = *pStream++;
                    }
                }
            }

            pTile = pFrames + (*(u16*)pDescriptors << 2);
            descriptor1 = *(u16*)(pDescriptors + 2);
            texX = descriptor1 & 0x1F;
            texY = (descriptor1 >> 5) & 0x3F;
            tileHeader = *(u16*)(pTile + 2);

            /* asm 8001DED4-8001DF20: the sampled U is the band-origin base
             * plus the bpp-scaled tile column; texX itself stays in RAW word
             * units for the VRAM upload below (sp 0x20 in the asm). An
             * earlier transcription shifted texX in place and reused it for
             * the upload x, planting every part's texels up to 3*31 words
             * right of where the quad samples. */
            if (tileHeader & 0x1) {
                *(u32*)(pPrim + 0x14) |= 0x8;
                texU = texUBase8 + (texX << 1);
                width = pTile[0] >> 1;
            } else {
                *(u32*)(pPrim + 0x14) &= ~0x8;
                texU = texUBase4 + (texX << 2);
                width = pTile[0] >> 2;
            }

            flags = *(u32*)(pPrim + 0x14);
            flags &= ~0x7;
            flags |= directionIndex;
            *(u32*)(pPrim + 0x14) = flags;

            *(u8*)(pPrim + 0x4) = texU;
            *(u8*)(pPrim + 0x5) = texY + baseClutY;
            *(u8*)(pPrim + 0x6) = pTile[0];
            *(u8*)(pPrim + 0x7) = pTile[1];

            flags = *(u32*)(pPrim + 0x14);
            flags &= ~0x10;
            flags |= ((*pStream >> 2) & 0x10);
            *(u32*)(pPrim + 0x14) = flags;

            abr = (*pStream >> 4) & 0x3;
            clutXNibble = *pStream & 0xF;
            *(u32*)(pPrim + 0x10) = colorAndCode;
            if (abr == 0) {
                abr = spriteMode;
            }
            if (abr != 0) {
                *(u8*)(pPrim + 0x13) |= 0x2;
                abr--;
            }

            *(u16*)(pPrim + 0xA) =
                ((tileHeader & 0x1) << 7) |
                ((abr & 0x3) << 5) |
                (((vramYWord & 0x100) << 16) >> 20) |
                ((vramX & 0x3FF) >> 6) |
                ((vramYWord & 0x200) << 2);
            *(u16*)(pPrim + 0xC) =
                GetClut(*(s16*)(pAnimPackage + 0x8) + (clutXNibble << 4),
                        *(s16*)(pAnimPackage + 0xA));

            func_800251C8((u_long*)(pTile + 4),
                          (s16)(vramX + texX),
                          (s16)(vramYWord + texY),
                          width,
                          pTile[1]);

            if (hasWideOffsets) {
                *(s16*)(pPrim + 0x0) = (s16)(pStream[1] | ((s8)pStream[2] << 8));
                *(s16*)(pPrim + 0x2) = (s16)(pStream[3] | ((s8)pStream[4] << 8));
                pStream += 2;
            } else {
                *(s16*)(pPrim + 0x0) = (s8)pStream[1];
                *(s16*)(pPrim + 0x2) = (s8)pStream[2];
            }

            pStream += 3;
        }

        *(u32*)(pData + 0x40) = (*(u32*)(pData + 0x40) & ~0xFC) | ((i & 0x3F) << 2);
        func_800251C8(NULL, (s16)vramX, (s16)vramYWord, pFrame[4], pFrame[5]);
    }
#undef vramX
#undef vramYWord
}
#endif

extern MATRIX D_8004FBB8;
extern u8 D_800591AD;
extern u8 D_800591AE;
extern void func_80022038(void* pSpriteData);

#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/rendering", func_8001E148);
#else
void func_8001E148(void* pSpriteData) {
    u8* pData = pSpriteData;
    u8* pBase;
    SVECTOR position;
    VECTOR vec;
    MATRIX* pMatrix;
    s32 var_n;
    int xOffset;
    int yOffset;
    
    if ((D_800591AD != 0) || (D_800591AE != 0)) {
        func_80022038(pSpriteData);
    }

    pBase = (u8*)(uintptr_t)*(u32*)(pData + 0x20);
    var_n = *(u32*)(pData + 0x40) & 0x1F;
    yOffset = *(u8*)(pBase + 0x3D);
    yOffset <<= var_n;
    xOffset = *(u8*)(pBase + 0x3C);
    xOffset <<= var_n;

    if ((*(u32*)(pData + 0xAC) >> 2) & 0x1) {
        xOffset = -xOffset;
    }
    
    yOffset = (yOffset * *(s16*)(pData + 0x2C)) / 0x1000;
    xOffset = (xOffset * *(s16*)(pData + 0x2C)) / 0x1000;
    
    position.vx = *(s32*)(pData + 0x0) >> 0x10;
    position.vy = *(s32*)(pData + 0x4) >> 0x10;
    position.vz = *(s32*)(pData + 0x8) >> 0x10;
    ApplyMatrix(&D_8004FBB8, &position, &vec);
    pMatrix = (MATRIX*)(pBase + 0x0C);
    pMatrix->t[0] = D_8004FBB8.t[0] + vec.vx + xOffset;
    pMatrix->t[1] = D_8004FBB8.t[1] + vec.vy + yOffset;
    pMatrix->t[2] = D_8004FBB8.t[2] + vec.vz;
    SetRotMatrix(pMatrix);
    SetTransMatrix(pMatrix);
}
#endif

extern void func_8001E9BC(void* pSpriteData, void* ot);

void func_8001E298(void* pSpriteData, void* ot) {
    u8* pData = pSpriteData;

    func_8001E148(pData);
    func_8001E3D8(pData, ot);
    if (((*(u32*)(pData + 0x3C) >> 2) & 0x1) != 0) {
        func_8001E9BC(pData, ot);
    }
}

/* Retail takes the split value as a plain int (it is passed through without
 * re-extension); the port keeps the s16 that field/main/misc2.c declares. */
#ifdef XENO_PC_PORT
#define RENDER_SPLIT_T s16
#else
#define RENDER_SPLIT_T s32
#endif

extern void func_8001EE88(void* pSpriteData, void* ot, RENDER_SPLIT_T angle);


void func_8001E2F8(void* pSpriteData, void* ot, RENDER_SPLIT_T angle) {
    u8* pData = pSpriteData;

    func_8001E148(pData);
    func_8001EE88(pData, ot, angle);
    if (((*(u32*)(pData + 0x3C) >> 2) & 0x1) != 0) {
        func_8001E9BC(pData, ot);
    }
}

extern void func_8001F1D4(void* pSpriteData, void* ot, RENDER_SPLIT_T angle);

void func_8001E368(void* pSpriteData, void* ot, RENDER_SPLIT_T angle) {
    u8* pData = pSpriteData;

    func_8001E148(pData);
    func_8001F1D4(pData, ot, angle);
    if (((*(u32*)(pData + 0x3C) >> 2) & 0x1) != 0) {
        func_8001E9BC(pData, ot);
    }
}

extern void* g_GfxCurWorkBuffer;
extern void* g_GfxCurWorkBufferEnd;

#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/rendering", func_8001E3D8);
#else
void func_8001E3D8(void* pSpriteData, void* ot) {
    u8* pData = pSpriteData;
    u8* pBase = (u8*)(uintptr_t)*(u32*)(pData + 0x20);
    u32 flags40 = *(u32*)(pData + 0x40);
    u32 flags3C = *(u32*)(pData + 0x3C);
    u32 flagsAC = *(u32*)(pData + 0xAC);
    s32 scaleShift = (flags40 >> 8) & 0x1F;
    s32 baseYOffset = (s8)*(u8*)(pBase + 0x3D) << scaleShift;
    s32 baseXOffset = (s8)*(u8*)(pBase + 0x3C) << scaleShift;
    u8* pFramePrim = (u8*)(uintptr_t)*(u32*)(pBase + 0x30);
    s32 frameCount = (flags40 >> 2) & 0x3F;
    s32 currentDirection = -1;
    s32 drawDirection = 0;
    s32 i;
#ifdef XENO_PC_PORT
    static s32 s_diagCalls;
    static s32 s_diagLinked;
    s32 linkedThisCall = 0;
    PcPortFeiHd2dBatch hdFei;
#endif

    if ((flagsAC >> 2) & 1) {
        baseXOffset = -baseXOffset;
    }

#ifdef XENO_PC_PORT
    /* Player-sprite render census -- see XenoPlayerRenderDiagEnabled above. */
    if (XenoPlayerRenderDiagEnabled()) {
        extern s32 g_PlayerActorIndex;
        static u32 s_prevSprite;
        static s32 s_prevFrameCount = -1;
        static s32 s_calls;
        u32 playerSprite = 0;

        if (g_FieldActors != NULL) {
            playerSprite = *(u32*)((u8*)g_FieldActors +
                                   g_PlayerActorIndex * 0x5C + 0x4);
        }
        if (playerSprite != 0 &&
            (u32)(uintptr_t)pSpriteData == playerSprite) {
            int exhausted = (((u8*)g_GfxCurWorkBuffer + frameCount * 0x28) >=
                             (u8*)g_GfxCurWorkBufferEnd);

            s_calls++;
            if (playerSprite != s_prevSprite ||
                frameCount != s_prevFrameCount || exhausted ||
                (s_calls % 120) == 0) {
                printf("[xeno-port][player-render] call=%d sprite=%08x "
                       "frameCount=%d %s%sbase=%08x base30=%08x "
                       "flags40=%08x flags3c=%08x mask3d=%02x\n",
                       (int)s_calls, (unsigned)playerSprite, (int)frameCount,
                       frameCount == 0 ? "NO-QUADS(frameCount==0) " : "",
                       exhausted ? "NO-QUADS(work-buffer-full) " : "",
                       (unsigned)(uintptr_t)pBase,
                       (unsigned)(uintptr_t)pFramePrim,
                       (unsigned)flags40, (unsigned)flags3C,
                       (unsigned)*(u8*)(pData + 0x3D));
                fflush(stdout);
                s_prevSprite = playerSprite;
                s_prevFrameCount = frameCount;
            }
        }
    }

    if (XenoFieldDiagEnabled() && s_diagCalls < 8) {
        printf("[field-diag] func_8001E3D8 call=%d sprite=%p ot=%p frames=%d flags3c=%08x flags40=%08x base30=%p mask3d=%02x work=%p end=%p\n",
               (int)s_diagCalls, pSpriteData, ot, (int)frameCount,
               (unsigned int)flags3C, (unsigned int)flags40,
               (void*)pFramePrim, (unsigned int)*(u8*)(pData + 0x3D),
               g_GfxCurWorkBuffer, g_GfxCurWorkBufferEnd);
    }
    s_diagCalls++;
#endif

    if (((u8*)g_GfxCurWorkBuffer + frameCount * 0x28) >= (u8*)g_GfxCurWorkBufferEnd) {
        return;
    }
    if (frameCount == 0) {
        return;
    }

#ifdef XENO_PC_PORT
    PcPort_FeiHd2dBegin(&hdFei, pSpriteData, frameCount);
#endif
    for (i = 0; i < frameCount; i++, pFramePrim += 0x18) {
        s32 direction = *(u32*)(pFramePrim + 0x14) & 0x7;

        if (currentDirection != direction) {
            u8* pDirTransforms = (u8*)(uintptr_t)*(u32*)(pBase + 0x34);
            u16 mask = s_DirectionMask8004FAF8[direction];

            currentDirection = direction;
            drawDirection = (mask & *(u8*)(pData + 0x3D)) == 0;

            if (pDirTransforms != NULL) {
                u8* pDirection = pDirTransforms + direction * 8;
                s32 dirYOffset;
                s32 dirXOffset;

                if ((*(u16*)pDirection != 0) || (*(s16*)(pDirection + 0x6) != 0)) {
                    SVECTOR rotation;
                    MATRIX matrix;

                    dirYOffset = (s8)pDirection[1] << scaleShift;
                    dirXOffset = (s8)pDirection[0] << scaleShift;
                    if ((flags3C >> 3) & 1) {
                        dirXOffset = -dirXOffset;
                    }
                    /* asm 8001E54C-8001E5C8: negative products are rounded
                     * toward zero (+0xFFF) before the >>12. */
                    dirYOffset = ScaleSpriteFrameByte32(dirYOffset, *(s16*)(pData + 0x2C));
                    dirXOffset = ScaleSpriteFrameByte32(dirXOffset, *(s16*)(pData + 0x2C));

                    rotation.vx = *(u16*)(pDirection + 0x2);
                    rotation.vy = *(u16*)(pDirection + 0x4);
                    rotation.vz = *(u16*)(pDirection + 0x6);
                    /* asm 8001E5B4-8001E5D0: flip-X negates the Z angle. */
                    if ((flags3C >> 3) & 1) {
                        rotation.vz = (s16)-(s32)(u16)rotation.vz;
                    }
                    Retail8003F738RotMatrix(&rotation, &matrix);
                    matrix.t[0] = *(s32*)(pBase + 0x20) + dirXOffset;
                    matrix.t[1] = *(s32*)(pBase + 0x24) + dirYOffset;
                    matrix.t[2] = *(s32*)(pBase + 0x28);
                    SetMulMatrix((MATRIX*)(pBase + 0x0C), &matrix);
                    SetTransMatrix(&matrix);
                } else {
                    SetRotMatrix((MATRIX*)(pBase + 0x0C));
                    SetTransMatrix((MATRIX*)(pBase + 0x0C));
                }
            } else {
                SetRotMatrix((MATRIX*)(pBase + 0x0C));
                SetTransMatrix((MATRIX*)(pBase + 0x0C));
            }
        }

        if (drawDirection != 0) {
            POLY_FT4* poly = (POLY_FT4*)g_GfxCurWorkBuffer;
            s32 texU = *(u8*)(pFramePrim + 0x4);
            s32 texV = *(u8*)(pFramePrim + 0x5);
            s32 width = *(u8*)(pFramePrim + 0x6);
            s32 height = *(u8*)(pFramePrim + 0x7);
            s32 xOffset = (s8)*(u8*)(pFramePrim + 0x8);
            s32 yOffset = (s8)*(u8*)(pFramePrim + 0x9);
            s32 x0 = *(s16*)(pFramePrim + 0x0) << scaleShift;
            s32 y0 = *(s16*)(pFramePrim + 0x2) << scaleShift;
            s32 x1 = (width + xOffset) << scaleShift;
            s32 y1 = (height + yOffset) << scaleShift;
            s32 texU1;
            s32 texV1;
            long p;
            long flag;

            g_GfxCurWorkBuffer = (u8*)g_GfxCurWorkBuffer + 0x28;
            setlen(poly, 9);
            *(u32*)((u8*)poly + 0x4) = *(u32*)(pFramePrim + 0x10);
            poly->tpage = *(u16*)(pFramePrim + 0xA);
            poly->clut = *(u16*)(pFramePrim + 0xC);

            if ((flags3C >> 3) & 1) {
                x1 = -x1;
                x0 = -x0;
            }
            if ((flags3C >> 4) & 1) {
                y1 = -y1;
                y0 = -y0;
            }

            if (((*(u32*)(pFramePrim + 0x14) >> 4) & 1) == 0) {
                s_QuadWork8004FB98[0].vx = x0;
                s_QuadWork8004FB98[1].vx = x0 + x1;
                s_QuadWork8004FB98[2].vx = x0 + x1;
                s_QuadWork8004FB98[3].vx = x0;
            } else {
                s_QuadWork8004FB98[0].vx = x0 + x1;
                s_QuadWork8004FB98[1].vx = x0;
                s_QuadWork8004FB98[2].vx = x0;
                s_QuadWork8004FB98[3].vx = x0 + x1;
            }

            if (((*(u32*)(pFramePrim + 0x14) >> 5) & 1) == 0) {
                s_QuadWork8004FB98[0].vy = y0;
                s_QuadWork8004FB98[1].vy = y0;
                s_QuadWork8004FB98[2].vy = y0 + y1;
                s_QuadWork8004FB98[3].vy = y0 + y1;
            } else {
                s_QuadWork8004FB98[0].vy = y0 + y1;
                s_QuadWork8004FB98[1].vy = y0 + y1;
                s_QuadWork8004FB98[2].vy = y0;
                s_QuadWork8004FB98[3].vy = y0;
            }

            s_QuadWork8004FB98[0].vy -= baseYOffset;
            s_QuadWork8004FB98[1].vy -= baseYOffset;
            s_QuadWork8004FB98[2].vy -= baseYOffset;
            s_QuadWork8004FB98[3].vy -= baseYOffset;
            s_QuadWork8004FB98[0].vx -= baseXOffset;
            s_QuadWork8004FB98[1].vx -= baseXOffset;
            s_QuadWork8004FB98[2].vx -= baseXOffset;
            s_QuadWork8004FB98[3].vx -= baseXOffset;

            RotTransPers4(&s_QuadWork8004FB98[0],
                          &s_QuadWork8004FB98[1],
                          &s_QuadWork8004FB98[2],
                          &s_QuadWork8004FB98[3],
                          (long*)&poly->x0,
                          (long*)&poly->x1,
                          (long*)&poly->x3,
                          (long*)&poly->x2,
                          &p,
                          &flag);

            texU1 = width - 1;
            texV1 = height - 1;
            if (poly->x3 < poly->x0) {
                texU--;
                if (texU < 0) {
                    texU = 0;
                    texU1 = width - 2;
                }
            }

            poly->u0 = texU;
            poly->v0 = texV;
            poly->u1 = texU + texU1;
            poly->v1 = texV;
            poly->u2 = texU;
            poly->v2 = texV + texV1;
            poly->u3 = texU + texU1;
            poly->v3 = texV + texV1;

#ifdef XENO_PC_PORT
            if (!PcPort_FeiHd2dCapture(&hdFei, poly,
                    ((flags3C >> 27) & 1) ? (u8*)ot - direction * 4 : ot))
#endif
            if ((flags3C >> 27) & 1) {
                RENDER_ADD_PRIM((u8*)ot - direction * 4, poly);
            } else {
                RENDER_ADD_PRIM(ot, poly);
            }
#ifdef XENO_PC_PORT
            linkedThisCall++;
            {
                int df0 = 0;
                int df1 = 0;
                if (WmSpriteDiagActive(&df0, &df1)) {
                    int wf = PcPort_WorldCaptureCurFrame();
                    if (wf >= df0 && wf <= df1) {
                        printf("[wm-sprite] wf=%d sprite=%p xy=(%d,%d)-(%d,%d)-(%d,%d)-(%d,%d) uv=(%u,%u)+(%u,%u) tpage=%04x clut=%04x\n",
                               wf, pSpriteData,
                               (int)poly->x0, (int)poly->y0,
                               (int)poly->x1, (int)poly->y1,
                               (int)poly->x2, (int)poly->y2,
                               (int)poly->x3, (int)poly->y3,
                               (unsigned int)poly->u0,
                               (unsigned int)poly->v0,
                               (unsigned int)((int)poly->u3 - (int)poly->u0),
                               (unsigned int)((int)poly->v3 - (int)poly->v0),
                               (unsigned int)poly->tpage,
                               (unsigned int)poly->clut);
                    }
                }
            }
            if (XenoFieldDiagEnabled() && s_diagLinked < 8) {
                printf("[field-diag] func_8001E3D8 link=%d poly=%p ot=%p tag=%08x code=%02x xy0=(%d,%d) uv0=(%u,%u) tpage=%04x clut=%04x dir=%d\n",
                       (int)s_diagLinked, (void*)poly, ot,
                       (unsigned int)*(u32*)poly, (unsigned int)poly->code,
                       (int)poly->x0, (int)poly->y0,
                       (unsigned int)poly->u0, (unsigned int)poly->v0,
                       (unsigned int)poly->tpage, (unsigned int)poly->clut,
                       (int)direction);
                s_diagLinked++;
            }
#endif
        }
    }

#ifdef XENO_PC_PORT
    PcPort_FeiHd2dEnd(&hdFei);
    if (XenoFieldDiagEnabled() && s_diagCalls <= 8) {
        printf("[field-diag] func_8001E3D8 done linked=%d workNow=%p\n",
               (int)linkedThisCall, g_GfxCurWorkBuffer);
    }
#endif
}
#endif

#ifdef XENO_PC_PORT
/* Retail 8004FAD8: four initially-zero vectors, separate from the sprite
 * XY scratch at 8004FB98. The shadow renderer writes X/Z, preserving Y. */
static SVECTOR s_ShadowQuad8004FAD8[4];
extern MATRIX* ScaleMatrixL(MATRIX* matrix, VECTOR* scale);

void func_8001E9BC(void* sprite, void* ot) {
    u8* data = sprite;
    MATRIX matrix = D_8004FBB8;
    SVECTOR position;
    VECTOR scale, translated;
    u8* base;
    u8* frame;
    u32 count, i;
    s32 direction = -1;
    int visible = 0;

    position.vx = *(s16*)(data + 2);
    position.vy = *(s16*)(data + 6);
    position.vz = *(s16*)(data + 10);
    scale.vx = *(s16*)(data + 0x2C);
    scale.vy = scale.vx / 2;
    scale.vz = 0;
    ScaleMatrixL(&matrix, &scale);
    position.vy = *(s16*)(data + 0x84);
    ApplyMatrix(&D_8004FBB8, &position, &translated);
    matrix.t[0] = (u32)matrix.t[0] + (u32)translated.vx;
    matrix.t[1] = (u32)matrix.t[1] + (u32)translated.vy;
    matrix.t[2] = (u32)matrix.t[2] + (u32)translated.vz;
    SetRotMatrix(&matrix);
    SetTransMatrix(&matrix);

    count = data[0x40] >> 2;
    base = (u8*)(uintptr_t)*(u32*)(data + 0x20);
    frame = (u8*)(uintptr_t)*(u32*)(base + 0x30);
    if ((u32)(uintptr_t)g_GfxCurWorkBuffer + count * 40 >=
        (u32)(uintptr_t)g_GfxCurWorkBufferEnd || count == 0) return;
    for (i = 0; i < (data[0x40] >> 2); ++i, frame += 24) {
        u32 frameFlags = *(u32*)(frame + 20);
        u32 flags = *(u32*)(data + 0x3C);
        u32 shift = (*(u32*)(data + 0x40) >> 8) & 31;
        s32 x, z, dx, dz, other;
        u8* poly;
        long xy[4] = {0, 0, 0, 0}, p, flag;
        s16 average;
        if (direction != (frameFlags & 7)) {
            direction = frameFlags & 7;
            visible = (s_DirectionMask8004FAF8[direction] & data[0x3D]) == 0;
        }
        if (!visible) continue;
        dx = (s32)((u32)(s32)(s16)(frame[6] + (s8)frame[8]) << shift);
        dz = (s32)((u32)(s32)(s16)(frame[7] + (s8)frame[9]) << shift);
        x = (s32)((u32)(s32)*(s16*)frame << shift);
        z = (s32)((u32)(s32)*(s16*)(frame + 2) << shift);
        if (flags & 8) { dx = (s32)(0u - (u32)dx); x = (s32)(0u - (u32)x); }
        if (((flags >> 4) ^ (frameFlags >> 5)) & 1) {
            dz = (s32)(0u - (u32)dz); z = (s32)(0u - (u32)z);
        }
        other = (s32)((u32)x + (u32)dx);
        s_ShadowQuad8004FAD8[0].vx = (frameFlags & 16) ? other : x;
        s_ShadowQuad8004FAD8[1].vx = (frameFlags & 16) ? x : other;
        s_ShadowQuad8004FAD8[2].vx = s_ShadowQuad8004FAD8[1].vx;
        s_ShadowQuad8004FAD8[3].vx = s_ShadowQuad8004FAD8[0].vx;
        other = (s32)((u32)z + (u32)dz);
        s_ShadowQuad8004FAD8[0].vz = (frameFlags & 32) ? other : z;
        s_ShadowQuad8004FAD8[1].vz = s_ShadowQuad8004FAD8[0].vz;
        s_ShadowQuad8004FAD8[2].vz = (frameFlags & 32) ? z : other;
        s_ShadowQuad8004FAD8[3].vz = s_ShadowQuad8004FAD8[2].vz;
        poly = g_GfxCurWorkBuffer;
        g_GfxCurWorkBuffer = poly + 40;
        poly[3] = 9;
        *(u32*)(poly + 4) = 0x2C000000u;
        RotTransPers4(&s_ShadowQuad8004FAD8[0], &s_ShadowQuad8004FAD8[1],
                      &s_ShadowQuad8004FAD8[2], &s_ShadowQuad8004FAD8[3],
                      &xy[0], &xy[1], &xy[2], &xy[3], &p, &flag);
        *(u32*)(poly + 8) = (u32)xy[0]; *(u32*)(poly + 16) = (u32)xy[1];
        *(u32*)(poly + 32) = (u32)xy[2]; *(u32*)(poly + 24) = (u32)xy[3];
        /* The retail sum wraps to a signed halfword BEFORE division by two. */
        average = (s16)(*(u16*)(poly + 10) + *(u16*)(poly + 18));
        *(s16*)(poly + 10) = *(s16*)(poly + 18) = average / 2;
        average = (s16)(*(u16*)(poly + 26) + *(u16*)(poly + 34));
        *(s16*)(poly + 26) = *(s16*)(poly + 34) = average / 2;
        *(u16*)(poly + 22) = *(u16*)(frame + 10);
        *(u16*)(poly + 14) = *(u16*)(frame + 12);
        poly[12] = poly[28] = frame[4];
        poly[20] = poly[36] = frame[4] + frame[6] - 1;
        poly[13] = poly[21] = frame[5];
        poly[29] = poly[37] = frame[5] + frame[7] - 1;
        *(u32*)poly = (*(u32*)poly & 0xFF000000u) | (*(u32*)ot & 0xFFFFFFu);
        *(u32*)ot = (*(u32*)ot & 0xFF000000u) | ((u32)(uintptr_t)poly & 0xFFFFFFu);
    }
}
#else
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/rendering", func_8001E9BC);
#endif

int func_8001EE68(u8* arg0) {
    return arg0[1] >> 7;
}

int func_8001EE74(u16* arg0) {
    return (arg0[0] >> 9) & 0x3F;
}

#ifdef XENO_PC_PORT
/* Retail 8001EE88 / 8001F1D4 split each frame primitive at a vertical
 * coordinate, not an angle. Keep MIPS wrapping shifts/additions explicit.
 * Even rejected primitives consume a packet and update the scratch Xs. */
static void RenderSpriteVerticalSplit(void* sprite, void* ot, s16 splitY,
                                      int lowerPart) {
    u8* data = sprite;
    u8* base = (u8*)(uintptr_t)*(u32*)(data + 0x20);
    u8* frame = (u8*)(uintptr_t)*(u32*)(base + 0x30);
    u32 flags = *(u32*)(data + 0x40);
    u32 count = (flags >> 2) & 0x3F;
    s32 split = (s32)((u32)(s32)splitY << ((flags >> 8) & 31));
    u32 cursor = (u32)(uintptr_t)g_GfxCurWorkBuffer;
    u32 end = (u32)(uintptr_t)g_GfxCurWorkBufferEnd;
    u32 i;

    if (cursor + count * 40 >= end || count == 0) return;
    for (i = 0; i < ((u8*)data)[0x40] >> 2; ++i, frame += 24) {
        u8* poly = g_GfxCurWorkBuffer;
        u32 shift = (*(u32*)(data + 0x40) >> 8) & 31;
        u32 mirror = *(u32*)(data + 0x3C);
        u32 frameFlags = *(u32*)(frame + 0x14);
        s32 x = (s32)((u32)(s32)*(s16*)frame << shift);
        s32 y = (s32)((u32)(s32)*(s16*)(frame + 2) << shift);
        s32 dx = (s32)((u32)((s32)frame[6] + (s8)frame[8]) << shift);
        s32 dy = (s32)((u32)((s32)frame[7] + (s8)frame[9]) << shift);
        s32 other, low, high, clipped = 0;
        s32 texU, texV, texWidth, texHeight;
        long xy[4] = {0, 0, 0, 0}, p, flag;

        g_GfxCurWorkBuffer = poly + 40;
        poly[3] = 9;
        *(u32*)(poly + 4) = *(u32*)(frame + 16);
        *(u16*)(poly + 22) = *(u16*)(frame + 10);
        *(u16*)(poly + 14) = *(u16*)(frame + 12);
        if (mirror & 8) { x = (s32)(0u - (u32)x); dx = (s32)(0u - (u32)dx); }
        if (mirror & 16) { y = (s32)(0u - (u32)y); dy = (s32)(0u - (u32)dy); }
        other = (s32)((u32)x + (u32)dx);
        s_QuadWork8004FB98[0].vx = (frameFlags & 16) ? other : x;
        s_QuadWork8004FB98[1].vx = (frameFlags & 16) ? x : other;
        s_QuadWork8004FB98[2].vx = s_QuadWork8004FB98[1].vx;
        s_QuadWork8004FB98[3].vx = s_QuadWork8004FB98[0].vx;
        other = (s32)((u32)y + (u32)dy);
        low = dy > 0 ? y : other;
        high = dy > 0 ? other : y;
        if (lowerPart) {
            if (high < split) continue;
            if (low < split) clipped = (s32)((u32)split - (u32)low);
            y = (s32)((u32)y + (u32)clipped);
            dy = dy > 0 ? (s32)((u32)dy - (u32)clipped)
                        : (s32)((u32)dy + (u32)clipped);
        } else {
            if (split < low) continue;
            if (split < high) clipped = (s32)((u32)high - (u32)split);
            dy = (s32)((u32)dy - (u32)clipped);
            if (dy < 0) y = (s32)((u32)y - (u32)clipped);
        }
        other = (s32)((u32)y + (u32)dy);
        s_QuadWork8004FB98[0].vy = (frameFlags & 32) ? other : y;
        s_QuadWork8004FB98[1].vy = s_QuadWork8004FB98[0].vy;
        s_QuadWork8004FB98[2].vy = (frameFlags & 32) ? y : other;
        s_QuadWork8004FB98[3].vy = s_QuadWork8004FB98[2].vy;
        RotTransPers4(&s_QuadWork8004FB98[0], &s_QuadWork8004FB98[1],
                      &s_QuadWork8004FB98[2], &s_QuadWork8004FB98[3],
                      &xy[0], &xy[1], &xy[2], &xy[3], &p, &flag);
        *(u32*)(poly + 8) = (u32)xy[0];
        *(u32*)(poly + 16) = (u32)xy[1];
        *(u32*)(poly + 32) = (u32)xy[2];
        *(u32*)(poly + 24) = (u32)xy[3];
        shift = (*(u32*)(data + 0x40) >> 8) & 31;
        clipped >>= shift;
        texU = frame[4]; texV = frame[5]; texWidth = frame[6];
        texHeight = (s32)((u32)frame[7] - (u32)clipped);
        if (lowerPart) texV = dy > 0 ? (s32)((u32)texV + (u32)clipped)
                                    : (s32)((u32)texV - (u32)clipped);
        else if (dy <= 0) texV = (s32)((u32)texV - (u32)clipped);
        if (*(s16*)(poly + 32) < *(s16*)(poly + 8)) {
            if (--texU < 0) { texU = 0; --texWidth; }
        }
        poly[12] = poly[28] = texU;
        poly[20] = poly[36] = texU + texWidth;
        poly[13] = poly[21] = texV;
        poly[29] = poly[37] = (u32)texV + (u32)texHeight;
        *(u32*)poly = (*(u32*)poly & 0xFF000000u) | (*(u32*)ot & 0xFFFFFFu);
        *(u32*)ot = (*(u32*)ot & 0xFF000000u) | ((u32)(uintptr_t)poly & 0xFFFFFFu);
    }
}

void func_8001EE88(void* sprite, void* ot, s16 splitY) {
    RenderSpriteVerticalSplit(sprite, ot, splitY, 0);
}

void func_8001F1D4(void* sprite, void* ot, s16 splitY) {
    RenderSpriteVerticalSplit(sprite, ot, splitY, 1);
}
#else
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/rendering", func_8001EE88);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/rendering", func_8001F1D4);
#endif

extern s16 D_80059194;
extern s16 D_80059196;

#ifndef XENO_PC_PORT
typedef struct { s16 x; s16 y; } PtF530;
void *func_8001F530__r(PtF530 *arg0, s32 arg1) asm("func_8001F530");
void *func_8001F530__r(PtF530 *arg0, s32 arg1) {
    PtF530 sp;
    u16 cy;
    s32 cx;

    if ((D_80059196 + arg1) >= 0x41) {
        D_80059196 = 0;
        D_80059194 = D_80059194 + 1;
        if (D_80059194 >= 3) {
            D_80059194 = 0;
        }
    }
    cy = D_80059196;
    cx = D_80059194;
    sp.x = cy + 0x300;
    sp.y = (cx << 6) + 0x140;
    *arg0 = sp;
    D_80059196 = cy + arg1;
    return arg0;
}
#else
void func_8001F530(void* pOut, s32 advance) {
    u16* p = (u16*)pOut;
    s16 y = D_80059196;
    if (y + advance >= 0x41) {
        D_80059196 = 0;
        D_80059194++;
        if (D_80059194 >= 3) {
            D_80059194 = 0;
        }
    }
    {
        u16 curY = D_80059196;
        s16 curX = D_80059194;
        s16 outY = curY + 0x300;
        s16 outX = curX * 64 + 0x140;
        p[0] = outY;
        p[1] = outX;
    }
    D_80059196 += advance;
}
#endif

#ifdef XENO_PC_PORT
static s32 ScaleFrameExtent(s32 value, s16 scale) {
    s32 result = value * scale;

    if (result < 0) {
        result += 0xFFF;
    }

    return result >> 12;
}
#endif

#ifndef XENO_PC_PORT
void func_8001F5BC(void *pSpriteData, s32 arg1, s32 *outZ, s32 *outX, s32 *volatile outY) {
    u8 *pData = pSpriteData;
    u8 *pVram;
    u8 *pAnims;
    u8 *pAnim;
    u8 *pDir;
    s32 fi;
    u8 *pFrames;
    u8 *pFrame;
    s32 ty;

    pVram = *(u8 **)(pData + 0x24);
    pAnims = *(u8 **)(pVram + 0x10);
    pAnim = (u8 *)((s32)*(u16 *)(pAnims + 2) + (s32)pAnims);
    pDir = (u8 *)((s32)*(u16 *)(pAnim + 4) + (s32)pAnim);
    fi = *(u8 *)(pDir + 4);
    pFrames = *(u8 **)pVram;
    if (fi != 0) {
        fi -= 1;
    }
    if (*(u16 *)(pFrames + fi * 2) < fi) {
        fi = 0;
    }
    pFrame = (u8 *)((s32)*(u16 *)(pFrames + fi * 2 + 2) + (s32)pFrames);
    *outX = (*(u8 *)(pFrame + 3) * *(s16 *)(pData + 0x2C)) / 0x1000;
    ty = *(u8 *)(pFrame + 1);
    ty = (ty * *(s16 *)(pData + 0x2C)) / 0x1000;
    *outY = ty;
    *outZ = (*(u8 *)(pFrame + 2) * *(s16 *)(pData + 0x2C)) / 0x1000;
}
#else
void func_8001F5BC(void* pSpriteData, s32 arg1, s32* outZ, s32* outX, s32* outY) {
    u8* pData = pSpriteData;
    u8* pVramData = (u8*)(uintptr_t)*(u32*)(pData + 0x24);
    u8* pAnimations = (u8*)(uintptr_t)*(u32*)(pVramData + 0x10);
    u8* pAnim = pAnimations + *(u16*)(pAnimations + 0x2);
    u8* pDirAnim = pAnim + *(u16*)(pAnim + 0x4);
    u8 frameIndex = *(u8*)(pDirAnim + 0x4);
    u8* pFrames = (u8*)(uintptr_t)*(u32*)(pVramData + 0x0);
    u8* pFrame;
    s16 scale = *(s16*)(pData + 0x2C);

    (void)arg1;

    if (frameIndex != 0) {
        frameIndex--;
    }

    if (*(u16*)(pFrames + frameIndex * 2) < frameIndex) {
        frameIndex = 0;
    }

    pFrame = pFrames + *(u16*)(pFrames + frameIndex * 2 + 0x2);
    *outX = ScaleFrameExtent(*(u8*)(pFrame + 0x3), scale);
    *outY = ScaleFrameExtent(*(u8*)(pFrame + 0x1), scale);
    *outZ = ScaleFrameExtent(*(u8*)(pFrame + 0x2), scale);
}
#endif

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): sprite_recolor_parts (src/decomp/src/resident/sprite.c:1368). */
#ifndef XM_T_SpriteImage
#define XM_T_SpriteImage
typedef struct {
    u8 unknown0[4];
    SpriteImageSize size;  
} SpriteImage;
#endif
#ifndef XM_S_sprite_recolor_parts
#define XM_S_sprite_recolor_parts
void xm_sprite_recolor_parts(Sprite *sprite) __asm__("func_8001F6B0");
#endif
void func_8001F6B0(Sprite *sprite) {
    SpriteImageSize size; 
    SpritePart *part;
    u32 colour;
    s32 blend;
    s32 i;

    if ((sprite->render.word & 3) != 1) {
        return;
    }
    blend = sprite->render.bits.blend;
    if (blend != 0) {
        blend--;
    }
    size = ((SpriteImage *)sprite->image)->size;
    part = sprite->renderer->parts[1];
    colour = *(u32 *)&sprite->red;
    for (i = 0; i != (u8)sprite->flags >> 2; i++) {
        part[i].colour = colour;
        part[i].tpage = (part[i].tpage & 0xFF9F) | (blend << 5);
    }
}
#endif
#else
void func_8001F6B0(void* pSpriteData) {
    u8* pData = pSpriteData;
    u32 flags3C = *(u32*)(pData + 0x3C);
    s32 abr = (flags3C >> 5) & 0x7;
    u32 color = *(u32*)(pData + 0x28);
    u8 frameCount;
    u8* pBase;
    u8* pPrim;
    s32 i;

    if ((flags3C & 0x3) != 1) {
        return;
    }

    if (abr != 0) {
        abr--;
    }

    frameCount = *(u8*)(pData + 0x40) >> 2;
    pBase = (u8*)(uintptr_t)*(u32*)(pData + 0x20);
    pPrim = (u8*)(uintptr_t)*(u32*)(pBase + 0x30);

    for (i = 0; i < frameCount; i++, pPrim += 0x18) {
        u16 tpage = *(u16*)(pPrim + 0xA);

        *(u32*)(pPrim + 0x10) = color;
        tpage &= 0xFF9F;
        tpage |= abr << 5;
        *(u16*)(pPrim + 0xA) = tpage;
    }
}
#endif

extern void func_800234AC(void* pSpriteData);

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): sprite_apply_cell_frame_controls (src/decomp/src/resident/sprite.c:1396). */
#ifndef XM_S_sprite_apply_cell_frame_controls
#define XM_S_sprite_apply_cell_frame_controls
void xm_sprite_apply_cell_frame_controls(Sprite *sprite, s32 frame, SpriteSource *source) __asm__("func_8001F750");
#endif
void func_8001F750(Sprite *sprite, s32 frame, SpriteSource *source) {
    u16 *frames = source->frames;
    u8 *record = (u8 *)(frames[frame] + (s32)frames);
    u8 wide = *record & 0x80;
    s32 count = *record & 0x3F;
    u8 *p = record + (count * 2 + 4);
    s32 i;
    u8 control;
    s32 slot;

    for (i = 0; i != count; i++) {
    next:
        control = *p;
        if (control & 0x80) {
            p++;
            if (control & 0x40) {
                slot = control & 7;
                if (sprite->renderer->pointer34 == NULL) {
                    sprite->renderer->pointer34 = xm_heap_alloc(0x40, 0);
                    xm_sprite_clear_group_entries(sprite);
                }
                if (control & 0x20) {
                    sprite->renderer->pointer34[slot].byte0 = *p++;
                    sprite->renderer->pointer34[slot].byte1 = *p++;
                }
                if (control & 0x10) {
                    s16 depth = *p++ << 4;

                    sprite->renderer->pointer34[slot].half6 = depth;
                } else {
                    sprite->renderer->pointer34[slot].half6 = 0;
                }
            } else {
                if (control & 1) {
                    p++;
                }
                if (control & 2) {
                    p++;
                }
            }
            goto next;
        }
        if (wide) {
            p += 2;
        }
        p += 3;
    }
}
#endif
#else
void func_8001F750(void* pSpriteData, u16 frameIndex, void* pAnimPackage) {
    u8* pData = pSpriteData;
    u8* pPackage = pAnimPackage;
    u8* pFrames = (u8*)(uintptr_t)*(u32*)(pPackage + 0x0);
    u8* pFrame = pFrames + *(u16*)(pFrames + frameIndex * 2);
    u8 header = pFrame[0];
    s32 hasWideOffsets = header & 0x80;
    s32 partCount = header & 0x3F;
    u8* pStream = pFrame + partCount * 2 + 4;
    s32 i;

    for (i = 0; i < partCount; i++) {
        u8 command = *pStream;

        if (command & 0x80) {
            if (command & 0x40) {
                u8* pBase = (u8*)(uintptr_t)*(u32*)(pData + 0x20);
                u8* pDirTransforms = (u8*)(uintptr_t)*(u32*)(pBase + 0x34);
                s32 directionIndex = command & 0x7;
                u8* pDirection;

                pStream++;
                if (pDirTransforms == NULL) {
                    pDirTransforms = HeapAlloc(0x40, 0);
                    *(u32*)(pBase + 0x34) = (u32)(uintptr_t)pDirTransforms;
                    func_800234AC(pData);
                }

                pDirection = pDirTransforms + directionIndex * 8;
                if (command & 0x20) {
                    pDirection[0] = *pStream++;
                    pDirection[1] = *pStream++;
                }

                if (command & 0x10) {
                    *(u16*)(pDirection + 0x6) = (u16)(*pStream++ << 4);
                } else {
                    *(u16*)(pDirection + 0x6) = 0;
                }
            } else {
                pStream++;
                if (command & 0x1) {
                    pStream++;
                }
                if (command & 0x2) {
                    pStream++;
                }
            }
        } else {
            if (hasWideOffsets) {
                pStream += 2;
            }
            pStream += 3;
        }
    }
}
#endif

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): sprite_apply_frame_controls (src/decomp/src/resident/sprite.c:1449). */
#ifndef XM_S_sprite_apply_frame_controls
#define XM_S_sprite_apply_frame_controls
void xm_sprite_apply_frame_controls(Sprite *sprite, s32 frame, SpriteSource *source) __asm__("func_8001F8E8");
#endif
void func_8001F8E8(Sprite *sprite, s32 frame, SpriteSource *source) {
    u16 *frames = source->frames;
    u8 *record;
    u8 wide;
    s32 count;
    u8 *p;
    s32 i;
    u8 control;
    s32 slot;
    s32 unused[4]; 

    if (frame >= (*frames & 0x1FF) + 1) {
        return;
    }
    if (*frames & 0x8000) {
        xm_sprite_apply_cell_frame_controls(sprite, frame, source);
        return;
    }
    record = (u8 *)(frames[frame] + (s32)frames);
    wide = *record & 0x80;
    count = *record & 0x3F;
    p = record + (count * 4 + 6);
    for (i = 0; i != count; i++) {
    next:
        control = *p;
        if (control & 0x80) {
            p++;
            if (control & 0x40) {
                if (sprite->renderer->pointer34 == NULL) {
                    sprite->renderer->pointer34 = xm_heap_alloc(0x40, 0);
                    xm_sprite_clear_group_entries(sprite);
                }
                slot = control & 7;
                if (control & 0x20) {
                    sprite->renderer->pointer34[slot].byte0 = *p++;
                    sprite->renderer->pointer34[slot].byte1 = *p++;
                }
                if (control & 0x10) {
                    s16 depth = *p++ << 4;

                    sprite->renderer->pointer34[slot].half6 = depth;
                } else {
                    sprite->renderer->pointer34[slot].half6 = 0;
                }
            } else {
                if (control & 1) {
                    p++;
                }
                if (control & 2) {
                    p++;
                }
            }
            goto next;
        }
        if (wide) {
            p += 2;
        }
        p += 3;
    }
}
#endif
#else
// Animation frame and tile data stuff
void func_8001F8E8(void* pSpriteData, u16 frameIndex, u32 animPackageAddr) {
    u8* pAnimPackage = (u8*)(uintptr_t)animPackageAddr;
    u8* pFrames = (u8*)(uintptr_t)*(u32*)(pAnimPackage + 0x0);
    u16 frameHeader = *(u16*)pFrames;

    if (frameIndex >= ((frameHeader & 0x1FF) + 1)) {
        return;
    }

    if (frameHeader & 0x8000) {
        func_8001F750(pSpriteData, frameIndex, pAnimPackage);
        return;
    }

    /* Non-delegating frame path (asm .L8001F948-.L8001FA88). Unlike the
     * delegating sibling func_8001F750, the per-part header is stride 4 with a
     * 6-byte lead (pFrame + partCount*4 + 6), and the loop counts only the
     * non-control (0x00-0x7F) sprite parts toward partCount -- interspersed
     * control commands (0x80+) are processed without advancing the count. */
    {
        u8* pData = (u8*)pSpriteData;
        u8* pFrame = pFrames + *(u16*)(pFrames + frameIndex * 2);
        u8 header = pFrame[0];
        s32 hasWideOffsets = header & 0x80;
        s32 partCount = header & 0x3F;
        u8* pStream;
        s32 i = 0;

        if (partCount == 0) {
            return;
        }
        pStream = pFrame + partCount * 4 + 6;

        while (1) {
            u8 command = *pStream;

            if (command & 0x80) {
                pStream++;
                if (command & 0x40) {
                    u8* pBase = (u8*)(uintptr_t)*(u32*)(pData + 0x20);
                    u8* pDirTransforms = (u8*)(uintptr_t)*(u32*)(pBase + 0x34);
                    s32 directionIndex = command & 0x7;
                    u8* pDirection;

                    if (pDirTransforms == NULL) {
                        pDirTransforms = HeapAlloc(0x40, 0);
                        *(u32*)(pBase + 0x34) = (u32)(uintptr_t)pDirTransforms;
                        func_800234AC(pData);
                    }

                    pDirection = pDirTransforms + directionIndex * 8;
                    if (command & 0x20) {
                        pDirection[0] = *pStream++;
                        pDirection[1] = *pStream++;
                    }
                    if (command & 0x10) {
                        *(u16*)(pDirection + 0x6) = (u16)(*pStream++ << 4);
                    } else {
                        *(u16*)(pDirection + 0x6) = 0;
                    }
                } else {
                    if (command & 0x1) {
                        pStream++;
                    }
                    if (command & 0x2) {
                        pStream++;
                    }
                }
            } else {
                if (hasWideOffsets) {
                    pStream += 2;
                }
                pStream += 3;
                i++;
                if (i == partCount) {
                    break;
                }
            }
        }
    }
}
#endif



/* Matches retail bytes under the Default preset (verified via objdump diff
 * against asm/slus_006.64/nonmatchings/system/rendering/GraphicsDrawPauseLetters.s).
 * The native port keeps the PSX-RAM source path in pc_port/src/world_map_pause.c.
 */
extern void* g_GfxPauseLettersCompressed[];

#ifndef XENO_PC_PORT
void GraphicsDrawPauseLetters(int x, int y) {
    void* pPauseImgData;
    int dx = x;
    int dy = y;
    int iRegSteer;
    pPauseImgData = LZSSHeapDecompress(&g_GfxPauseLettersCompressed, 0x0);
    /* Empty asm pair: emits zero bytes but keeps a zero-range quantity live
     * here so gcc 2.7.2 assigns pPauseImgData to s0 (retail layout). */
    __asm__ volatile("" : "=r"(iRegSteer));
    __asm__ volatile("" :: "r"(iRegSteer));
    func_8002DDE4(pPauseImgData, 1, dx, dy, 0, 0, 0);
    DrawSync(0);
    HeapFree(pPauseImgData);
}
#endif

extern void* D_800592E4;
extern s16 D_800592E8;
extern s16 D_800592EA;

void func_8001FB30(void) {
#ifdef XENO_PC_PORT
    u8* pStack = HeapAlloc(0x2000, 1);
#ifdef XENO_PC_PORT
    /* Host cannot emit R3000 $sp move/lw. Alloc/free stay so heap state
     * matches retail; the native stack needs no switch (same as the
     * game_overrides trampoline). Matching build keeps the three asms. */
    func_8002DDE4(D_800592E4, 1, D_800592E8, D_800592EA, 0, 0, 0);
#else
    u8* pOldSp;
    __asm__ volatile("move %0, $sp" : "=r"(pOldSp));
    __asm__ volatile("move $sp, %0" : : "r"(pStack + 0x1F00 - 4));
    *(u32*)(pStack + 0x1F00 - 4) = (u32)pOldSp;
    func_8002DDE4(D_800592E4, 1, D_800592E8, D_800592EA, 0, 0, 0);
    __asm__ volatile("lw $sp, 0($sp)");
#endif
    HeapFree(pStack);
#else
    u8* pStack = HeapAlloc(0x2000, 1);
    u8 *p = pStack + 0x1F00;
    register u8 *q asm("$8");
    __asm__ volatile("move %0,%1\n\tsw $29,0(%0)\n\taddiu %0,%0,-4\n\tmove $29,%0" : "=&r"(q) : "r"(p));
    func_8002DDE4(D_800592E4, 1, D_800592E8, D_800592EA, 0, 0, 0);
    __asm__ volatile("addiu $29,$29,4\n\tlw $29,0($29)");
    HeapFree(pStack);
#endif
}
