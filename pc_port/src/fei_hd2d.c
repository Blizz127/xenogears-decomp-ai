/* Optional field Fei art. All simulation and retail sprite generation still run.
 * Only the linked presentation primitives change. Missing art fails to retail. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "common.h"
#include "field/actor.h"
#include "psyq/libgpu.h"
#include "guest_prim_link.h"
#include "fei_hd2d.h"
#include "quick_checkpoint.h"
#include "psx_memory.h"

/* HD-2D lane sprite hooks (hd2d_sprite.h): weak, so this file links and
 * behaves as before when pc_port/src/hd2d defines none of them. */
#pragma weak PcPort_Hd2dSpriteBegin
#pragma weak PcPort_Hd2dSpriteDraw
#pragma weak PcPort_Hd2dSpriteEnd

extern unsigned int GR_CreateRGBATexture(int, int, unsigned char *);
extern void *g_GfxCurWorkBuffer, *g_GfxCurWorkBufferEnd;
extern s32 g_PlayerActorIndex, g_FieldNumActors;
extern u8 D_800ADB04;

#define ATLAS_SIZE 1254
#define EXTRA_BYTES (sizeof(POLY_FT4) + 2 * sizeof(DR_PSYX_TEX))
static int enabled = -1, loadAttempted;
static unsigned int texture;
static struct { int x0, y0, x1, y1; } cells[8];

int PcPort_FeiHd2dEnabled(void)
{
    if (enabled < 0) {
        const char *e = getenv("XENO_FEI_HD2D");
        enabled = !(e && strcmp(e, "0") == 0);
    }
    return enabled;
}

void PcPort_FeiHd2dToggle(void)
{
    enabled = !PcPort_FeiHd2dEnabled();
    fprintf(stderr, "[fei-hd2d] %s (field idle/walk experiment)\n",
            enabled ? "enabled" : "disabled");
}

/* The dev menu offers the HD-2D toggle only when the atlas file exists. */
int PcPort_FeiHd2dAssetPresent(void)
{
    const char *path = getenv("XENO_FEI_HD2D_ASSET");
    FILE *f;
    if (!path) path = "pc_port/assets/fei_hd2d/fei.rgba";
    f = fopen(path, "rb");
    if (!f) return 0;
    fclose(f);
    return 1;
}

static int LoadTexture(void)
{
    const size_t bytes = (size_t)ATLAS_SIZE * ATLAS_SIZE * 4;
    const char *path = getenv("XENO_FEI_HD2D_ASSET");
    unsigned char *pixels;
    FILE *f;
    int x, y, c;
    if (loadAttempted) return texture != 0;
    loadAttempted = 1;
    if (!path) path = "pc_port/assets/fei_hd2d/fei.rgba";
    f = fopen(path, "rb");
    if (!f) goto unavailable;
    pixels = malloc(bytes);
    if (!pixels) { fclose(f); goto unavailable; }
    if (fread(pixels, 1, bytes, f) != bytes || fgetc(f) != EOF) {
        free(pixels); fclose(f); goto unavailable;
    }
    fclose(f);
    for (c = 0; c < 8; ++c) {
        cells[c].x0 = cells[c].y0 = ATLAS_SIZE;
        cells[c].x1 = cells[c].y1 = -1;
    }
    /* Read bounds from alpha, without modifying the authored image. */
    for (y = 0; y < ATLAS_SIZE; ++y) for (x = 0; x < ATLAS_SIZE; ++x) {
        if (pixels[((size_t)y * ATLAS_SIZE + x) * 4 + 3] < 128) continue;
        c = (y * 2 / ATLAS_SIZE) * 4 + x * 4 / ATLAS_SIZE;
        if (x < cells[c].x0) cells[c].x0 = x;
        if (x > cells[c].x1) cells[c].x1 = x;
        if (y < cells[c].y0) cells[c].y0 = y;
        if (y > cells[c].y1) cells[c].y1 = y;
    }
    for (c = 0; c < 8; ++c) if (cells[c].x1 <= cells[c].x0 || cells[c].y1 <= cells[c].y0) {
        free(pixels); goto unavailable;
    }
    texture = GR_CreateRGBATexture(ATLAS_SIZE, ATLAS_SIZE, pixels);
    free(pixels);
    fprintf(stderr, "[fei-hd2d] loaded %s texture=%u\n", path, texture);
    return texture != 0;
unavailable:
    fprintf(stderr, "[fei-hd2d] unavailable asset %s; retaining retail sprites\n", path);
    return 0;
}

/* A guest pointer (u32 in retail structs) that lies inside guest RAM, else
 * NULL, so a stale or foreign pointer is never dereferenced. */
static const u8 *GuestRamPtr(u32 addr, size_t bytes)
{
    uintptr_t p = (uintptr_t)addr, base = (uintptr_t)g_PsxRam;
    if (!addr || p < base || p + bytes > base + (uintptr_t)PSX_RAM_SIZE) return NULL;
    return (const u8 *)p;
}

static u32 Fnv1a(u32 h, const u8 *p, size_t n)
{
    while (n--) { h ^= *p++; h *= 0x01000193u; }
    return h;
}

/* See hd2d_sprite.h: FNV-1a 32 over the package frame table (the u16 offset
 * table, content not address) then the TIM and CLUT VRAM origins. */
unsigned int PcPort_Hd2dSpriteIdentity(const void *sprite)
{
    const u8 *pkg, *frames;
    u32 h = 0x811C9DC5u;
    u8 xy[8];
    int n, i;
    if (!sprite) return h;
    pkg = GuestRamPtr(*(const u32 *)((const u8 *)sprite + 0x24), 0x14);
    if (!pkg) return h;
    frames = GuestRamPtr(*(const u32 *)pkg, 2);
    if (frames) {
        n = ((frames[0] | frames[1] << 8) & 0x1FF) + 1;
        if (n > 512) n = 512;
        if (GuestRamPtr(*(const u32 *)pkg, (size_t)n * 2)) h = Fnv1a(h, frames, (size_t)n * 2);
    }
    for (i = 0; i < 8; ++i) xy[i] = pkg[4 + i]; /* tex x, tex y, clut x, clut y */
    return Fnv1a(h, xy, sizeof xy);
}

static void Hd2dSpriteBegin(PcPortFeiHd2dBatch *b, void *sprite)
{
    int i;
    b->hookState = 0;
    b->part = 0;
    if (!PcPort_Hd2dSpriteBegin && !PcPort_Hd2dSpriteDraw && !PcPort_Hd2dSpriteEnd) return;
    b->info.identity = PcPort_Hd2dSpriteIdentity(sprite);
    b->info.frame = *(s16 *)((u8 *)sprite + 0x34);
    b->info.actor = -1;
    b->info.sprite = sprite;
    if (g_FieldActors)
        for (i = 0; i < g_FieldNumActors; ++i)
            if ((uintptr_t)sprite == g_FieldActors[i].pSpriteData) { b->info.actor = i; break; }
    b->hookState = 1;
    if (PcPort_Hd2dSpriteDraw && (!PcPort_Hd2dSpriteBegin || PcPort_Hd2dSpriteBegin(&b->info)))
        b->hookState = 2;
}

/* Offer one retail part to the lane first; 1 = it drew the part. */
static int Hd2dSpriteDraw(PcPortFeiHd2dBatch *b, void *poly, void *ot)
{
    const POLY_FT4 *p = poly;
    PcPortHd2dRetailQuad q;
    int part = b->part++;
    if (b->hookState != 2) return 0;
    q.x[0] = p->x0; q.y[0] = p->y0; q.u[0] = p->u0; q.v[0] = p->v0;
    q.x[1] = p->x1; q.y[1] = p->y1; q.u[1] = p->u1; q.v[1] = p->v1;
    q.x[2] = p->x2; q.y[2] = p->y2; q.u[2] = p->u2; q.v[2] = p->v2;
    q.x[3] = p->x3; q.y[3] = p->y3; q.u[3] = p->u3; q.v[3] = p->v3;
    q.tpage = p->tpage;
    q.clut = p->clut;
    q.ot = ot;
    q.poly = poly;
    return PcPort_Hd2dSpriteDraw(&b->info, part, &q) != 0;
}

void PcPort_FeiHd2dBegin(PcPortFeiHd2dBatch *b, void *sprite, int parts)
{
    ActorData *actor;
    b->active = b->count = 0;
    b->sprite = sprite;
    Hd2dSpriteBegin(b, sprite);
    if (!PcPort_FeiHd2dEnabled() || !PcPort_QuickCheckpointFieldIsActive() ||
        !D_800ADB04 || !g_FieldActors || g_PlayerActorIndex < 0 ||
        g_PlayerActorIndex >= g_FieldNumActors) return;
    if ((uintptr_t)sprite != g_FieldActors[g_PlayerActorIndex].pSpriteData) return;
    actor = (ActorData *)(uintptr_t)g_FieldActors[g_PlayerActorIndex].pActorData;
    if (!actor || actor->characterId != 0) return;
    /* Animations 0/1/2 are normal idle/walk/run; preserve all other authored poses. */
    if (actor->curAnimationId < 0 || actor->curAnimationId > 2) return;
    b->walking = actor->curAnimationId != 0;
    if ((size_t)((u8 *)g_GfxCurWorkBufferEnd - (u8 *)g_GfxCurWorkBuffer) <=
        (size_t)parts * sizeof(POLY_FT4) + EXTRA_BYTES) return;
    b->active = LoadTexture();
}

int PcPort_FeiHd2dCapture(PcPortFeiHd2dBatch *b, void *poly, void *ot)
{
    if (b->hookState && Hd2dSpriteDraw(b, poly, ot)) return 1;
    if (!b->active) return 0;
    b->prims[b->count] = poly;
    b->ots[b->count++] = ot;
    return 1;
}

static void FeiEnd(PcPortFeiHd2dBatch *b);

void PcPort_FeiHd2dEnd(PcPortFeiHd2dBatch *b)
{
    FeiEnd(b);
    if (b->hookState && PcPort_Hd2dSpriteEnd) PcPort_Hd2dSpriteEnd(&b->info);
}

static void FeiEnd(PcPortFeiHd2dBatch *b)
{
    int i, j, minX = INT_MAX, minY = INT_MAX, maxX = INT_MIN, maxY = INT_MIN;
    int cell, angle, height, width, cx, walking;
    u8 *sprite = b->sprite;
    POLY_FT4 *p, *original;
    DR_PSYX_TEX *bind, *reset;
    if (!b->count) return;
    for (i = 0; i < b->count; ++i) {
        if (b->ots[i] != b->ots[0]) goto retail;
        original = b->prims[i];
        for (j = 0; j < 4; ++j) {
            short *xy = (short *)((u8 *)original + 8 + j * 8);
            if (xy[0] < minX) minX = xy[0];
            if (xy[0] > maxX) maxX = xy[0];
            if (xy[1] < minY) minY = xy[1];
            if (xy[1] > maxY) maxY = xy[1];
        }
    }
    height = maxY - minY;
    if (height < 2 || height > 512 || maxX - minX > 512) goto retail;
    /* Retail's camera-relative facing used by SpriteSetDirection. */
    angle = *(u16 *)(sprite + 0x80) & 4095;
    cell = angle < 512 || angle >= 3584 ? 6 :
           angle < 1536 ? 0 : angle < 2560 ? 4 : 2;
    walking = b->walking;
    /* The animation interpreter owns this frame cursor, so speed and pauses
     * follow the game rather than wall time. */
    if (walking && ((*(u16 *)(sprite + 0x34) / 2) & 1)) ++cell;
    width = height * (cells[cell].x1 - cells[cell].x0 + 1) /
                     (cells[cell].y1 - cells[cell].y0 + 1);
    cx = (minX + maxX) / 2;
    p = g_GfxCurWorkBuffer;
    bind = (DR_PSYX_TEX *)(p + 1);
    reset = bind + 1;
    g_GfxCurWorkBuffer = reset + 1;
    memset(p, 0, EXTRA_BYTES);
    setPolyFT4(p);
    setSemiTrans(p, 1); /* RGBA shader supplies source alpha, including holes. */
    original = b->prims[0];
    p->r0 = (original->code & 1) || original->r0 > 127 ? 255 : original->r0 * 2;
    p->g0 = (original->code & 1) || original->g0 > 127 ? 255 : original->g0 * 2;
    p->b0 = (original->code & 1) || original->b0 > 127 ? 255 : original->b0 * 2;
    setXY4(p, cx-width/2, minY, cx+(width+1)/2, minY,
              cx-width/2, maxY, cx+(width+1)/2, maxY);
    setUV4(p, cells[cell].x0*256/ATLAS_SIZE, cells[cell].y0*256/ATLAS_SIZE,
              cells[cell].x1*256/ATLAS_SIZE, cells[cell].y0*256/ATLAS_SIZE,
              cells[cell].x0*256/ATLAS_SIZE, cells[cell].y1*256/ATLAS_SIZE,
              cells[cell].x1*256/ATLAS_SIZE, cells[cell].y1*256/ATLAS_SIZE);
    SetPsyXTexture(bind, texture, 256, 256);
    SetPsyXTexture(reset, 0, 0, 0);
    /* OT insertion prepends: bind -> sprite -> reset -> prior content. */
    PcPort_AddPrimDomainAware(b->ots[0], reset);
    PcPort_AddPrimDomainAware(b->ots[0], p);
    PcPort_AddPrimDomainAware(b->ots[0], bind);
    return;
retail:
    for (i = 0; i < b->count; ++i) PcPort_AddPrimDomainAware(b->ots[i], b->prims[i]);
}
