/*
 * LP64 proofs for the type-2/7/15 animation-render callbacks
 * (func_80025718 / func_800257F0 / func_80025A88).
 *
 * WorkListUpdate invokes these with a packed 0x1C WorkListEntry:
 *   +0x04 SpriteData* (u32)
 *   +0x08 onTriggerCallback (u32)  -- the function being invoked
 * SpriteData +0x20 is pBase and +0x24 is pVramData, both 4-byte slots.
 * SpriteDataI1 +0x2C/+0x30 are the per-context frame buffers.
 *
 * An 8-byte `*(u8**)` / `*(void**)` load concatenates the neighbouring word.
 * Trigger: any child sprite of types 2, 7, or 15 on the render work list.
 *
 * Build/run via pc_port/tests/run_anim_render_packed_slots_test.sh
 */
#include "common.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    short vx, vy, vz, pad;
} SVECTOR;
typedef struct {
    long vx, vy, vz, pad;
} VECTOR;
typedef struct {
    short m[3][3];
    long t[3];
} MATRIX;

void func_80025718(u8* pEntry);
void func_800257F0(u8* pArg);
void func_80025A88(u8* pEntry);

s32 g_GfxCurContext;
s32 D_80050100;
u_long* g_GfxCurOT;
MATRIX D_8004FBB8;
u16 D_8004FD80[16];
MATRIX D_8004FDA0;

static void* g_seen_sprite;
static void* g_seen_hdr;
static void* g_seen_buf;
static int g_draw_calls;
static int g_f6c_calls;

void func_80022038(void* pSprite)
{
    g_seen_sprite = pSprite;
}

void func_8002C700(void* hdr, void* buf, u_long* ot, s32 flags)
{
    (void)ot;
    (void)flags;
    g_seen_hdr = hdr;
    g_seen_buf = buf;
    g_draw_calls++;
}

void func_800B1F6C(void* a, void* b, u_long* ot, s32 c, s32 d, s32 e, s32 f)
{
    (void)ot;
    (void)c;
    (void)d;
    (void)e;
    (void)f;
    g_seen_hdr = a;
    g_seen_buf = b;
    g_f6c_calls++;
}

MATRIX* TransMatrix(MATRIX* m, VECTOR* t)
{
    (void)t;
    return m;
}

MATRIX* CompMatrix(MATRIX* a, MATRIX* b, MATRIX* out)
{
    (void)a;
    (void)b;
    return out;
}

void SetRotMatrix(MATRIX* m) { (void)m; }
void SetTransMatrix(MATRIX* m) { (void)m; }

VECTOR* ApplyMatrix(MATRIX* m, SVECTOR* in, VECTOR* out)
{
    (void)m;
    (void)in;
    out->vx = 0;
    out->vy = 0;
    out->vz = 0;
    return out;
}

void PushMatrix(void) {}
void PopMatrix(void) {}
MATRIX* RotMatrix(void* r, MATRIX* m)
{
    (void)r;
    return m;
}
MATRIX* MulMatrix0(MATRIX* a, MATRIX* b, MATRIX* out)
{
    (void)a;
    (void)b;
    return out;
}
void SetBackColor(long r, long g, long b)
{
    (void)r;
    (void)g;
    (void)b;
}
void SetColorMatrix(u32* m) { (void)m; }
void SetLightMatrix(MATRIX* m) { (void)m; }
void ReadGeomOffset(long* x, long* y)
{
    *x = 0;
    *y = 0;
}
void SetGeomOffset(long x, long y)
{
    (void)x;
    (void)y;
}

static int g_failures;

static void expect(int cond, const char* msg)
{
    if (!cond) {
        printf("FAIL: %s\n", msg);
        g_failures++;
    } else {
        printf("ok:   %s\n", msg);
    }
}

static void test_worklist_load_concat(void)
{
    unsigned char entry[0x1C];
    void* as_ptr;
    uint32_t as_u32;
    const uint32_t sprite = 0x00A12300u;
    const uint32_t callback = 0x0040BEEFu;

    memset(entry, 0, sizeof(entry));
    *(uint32_t*)(entry + 0x04) = sprite;
    *(uint32_t*)(entry + 0x08) = callback;

    memcpy(&as_ptr, entry + 0x04, sizeof(as_ptr));
    expect((uintptr_t)as_ptr == (((uintptr_t)callback << 32) | sprite),
           "8-byte load of work-list +0x4 concatenates the trigger callback");
    expect(as_ptr != (void*)(uintptr_t)sprite,
           "concatenated pointer is not the SpriteData slot");

    as_u32 = *(uint32_t*)(entry + 0x04);
    expect(as_u32 == sprite, "4-byte load of work-list +0x4 is just SpriteData*");
}

static void test_pbase_load_concat(void)
{
    unsigned char sprite[0x30];
    void* as_ptr;
    const uint32_t pbase = 0x00A20000u;
    const uint32_t pvram = 0x00A30000u;

    memset(sprite, 0, sizeof(sprite));
    *(uint32_t*)(sprite + 0x20) = pbase;
    *(uint32_t*)(sprite + 0x24) = pvram;

    memcpy(&as_ptr, sprite + 0x20, sizeof(as_ptr));
    expect((uintptr_t)as_ptr == (((uintptr_t)pvram << 32) | pbase),
           "8-byte load of SpriteData +0x20 concatenates pVramData");
    expect(*(uint32_t*)(sprite + 0x20) == pbase,
           "4-byte load of SpriteData +0x20 is just pBase");
}

static void test_buffer_load_concat(void)
{
    unsigned char base[0x40];
    void* as_ptr;
    const uint32_t buf0 = 0x00B10000u;
    const uint32_t buf1 = 0x00B20000u;

    memset(base, 0, sizeof(base));
    *(uint32_t*)(base + 0x2C) = buf0;
    *(uint32_t*)(base + 0x30) = buf1;

    memcpy(&as_ptr, base + 0x2C, sizeof(as_ptr));
    expect((uintptr_t)as_ptr == (((uintptr_t)buf1 << 32) | buf0),
           "8-byte load of I1 +0x2C concatenates the other context buffer");
    expect(*(uint32_t*)(base + 0x2C) == buf0,
           "4-byte load of I1 +0x2C is just pFramesData");
}

static void reset_obs(void)
{
    g_seen_sprite = NULL;
    g_seen_hdr = NULL;
    g_seen_buf = NULL;
    g_draw_calls = 0;
    g_f6c_calls = 0;
    g_GfxCurContext = 0;
    g_GfxCurOT = (u_long*)(uintptr_t)0x00C00000u;
    D_80050100 = 2;
    memset(&D_8004FBB8, 0, sizeof(D_8004FBB8));
}

/* Live packed work-list entry: callback canary at +0x8, pVramData canary at
 * sprite+0x24, other-context buffer canary at I1+0x30. The 8-byte mutant
 * walks off the sprite; the fixed body must see the real SpriteData and the
 * 4-byte frame buffer. */
static void plant_entry(unsigned char* entry, unsigned char* sprite,
                        unsigned char* base, unsigned char* buf0)
{
    memset(entry, 0, 0x1C);
    memset(sprite, 0, 0x50);
    memset(base, 0, 0x50);
    assert((uintptr_t)sprite <= UINT32_MAX);
    assert((uintptr_t)base <= UINT32_MAX);
    assert((uintptr_t)buf0 <= UINT32_MAX);

    *(uint32_t*)(entry + 0x04) = (uint32_t)(uintptr_t)sprite;
    *(uint32_t*)(entry + 0x08) = 0xDEADBEEFu; /* planted trigger callback */

    *(uint32_t*)(sprite + 0x20) = (uint32_t)(uintptr_t)base;
    *(uint32_t*)(sprite + 0x24) = 0xCAFEBABEu; /* planted pVramData */
    sprite[0x3F] = 1; /* skip CompMatrix path */

    *(uint32_t*)(base + 0x2C) = (uint32_t)(uintptr_t)buf0;
    *(uint32_t*)(base + 0x30) = 0xAABBCCDDu; /* planted other-context buffer */
    *(uint32_t*)(base + 0x34) = 0x00001000u; /* non-NULL hdr so we don't return */
}

static unsigned char s_entry[0x1C];
static unsigned char s_sprite[0x50];
static unsigned char s_base[0x50];
static unsigned char s_buf0[0x10];

static void test_func_80025718_live(void)
{
    reset_obs();
    plant_entry(s_entry, s_sprite, s_base, s_buf0);
    func_80025718(s_entry);

    expect(g_seen_sprite == s_sprite,
           "func_80025718 sees SpriteData, not SpriteData|callback");
    expect(g_draw_calls == 1, "func_80025718 reached func_8002C700");
    expect(g_seen_buf == s_buf0,
           "func_80025718 passes the 4-byte context-0 buffer");
    expect(*(uint32_t*)(s_sprite + 0x24) == 0xCAFEBABEu,
           "pVramData canary intact");
    expect(*(uint32_t*)(s_base + 0x30) == 0xAABBCCDDu,
           "other-context buffer canary intact");
}

static void test_func_800257F0_live(void)
{
    reset_obs();
    plant_entry(s_entry, s_sprite, s_base, s_buf0);
    func_800257F0(s_entry);

    expect(g_seen_sprite == s_sprite,
           "func_800257F0 sees SpriteData, not SpriteData|callback");
    expect(g_f6c_calls == 1, "func_800257F0 reached func_800B1F6C");
    expect(g_seen_buf == s_buf0,
           "func_800257F0 passes the 4-byte context-0 buffer");
}

static void test_func_80025A88_live(void)
{
    reset_obs();
    plant_entry(s_entry, s_sprite, s_base, s_buf0);
    func_80025A88(s_entry);

    expect(g_seen_sprite == s_sprite,
           "func_80025A88 sees SpriteData, not SpriteData|callback");
    expect(g_f6c_calls == 1, "func_80025A88 reached func_800B1F6C");
    expect(g_seen_buf == s_buf0,
           "func_80025A88 passes the 4-byte context-0 buffer");
}

int main(void)
{
    expect(sizeof(void*) == 8, "LP64 host");
    test_worklist_load_concat();
    test_pbase_load_concat();
    test_buffer_load_concat();
    test_func_80025718_live();
    test_func_800257F0_live();
    test_func_80025A88_live();

    if (g_failures) {
        printf("%d failure(s)\n", g_failures);
        return 1;
    }
    printf("all anim-render packed-slot proofs passed\n");
    return 0;
}
