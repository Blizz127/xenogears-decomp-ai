/* func_800303C8 / func_800305D8 keep PSX 4-byte pointer slots.  A native
 * 8-byte load of the work-block header merges the next packed word, so
 * FieldLoad's published +0x14 pointer would be handed to a garbage model. */
#include "common.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

void* func_800303C8(u8* a0, s32 a1);
void func_800305D8(u8* pAnimData);

static u8 g_arena[0x4000];
static size_t g_arena_off;
static int g_copy_calls;
static int g_pose_calls;
static int g_cb_calls;
static u8* g_copy_dst;
static u8* g_copy_src;
static s32 g_copy_count;
static u8* g_pose_verts;
static s32 g_pose_scale;

void HeapSetCurrentContentType(u_short contentTag) { (void)contentTag; }

void* HeapAlloc(u_int allocSize, u_int allocFlags) {
    void* p;
    (void)allocFlags;
    g_arena_off = (g_arena_off + 15u) & ~15u;
    assert(g_arena_off + allocSize <= sizeof(g_arena));
    p = g_arena + g_arena_off;
    g_arena_off += allocSize;
    memset(p, 0, allocSize);
    assert((uintptr_t)p <= UINT32_MAX);
    return p;
}

s32 func_8003014C(s32* pData) {
    g_cb_calls++;
    (void)pData;
    return 0x1234;
}

void func_800301C8(u8* pDst, u8* pSrc, s32 count, s16* pIndices) {
    g_copy_calls++;
    g_copy_dst = pDst;
    g_copy_src = pSrc;
    g_copy_count = count;
    (void)pIndices;
}

void func_80030228(u8* pVertices, s16* pDeltas, s32 count, s32 scale) {
    g_pose_calls++;
    g_pose_verts = pVertices;
    g_pose_scale = scale;
    (void)pDeltas;
    (void)count;
}

void func_800302D4(u8* pNormals, s16* pDeltas, s32 count, s32 scale) {
    (void)pNormals;
    (void)pDeltas;
    (void)count;
    (void)scale;
}

static u8* align24(void) {
    g_arena_off = (g_arena_off + 7u) & ~7u;
    return HeapAlloc(0x24, 0);
}

int main(void) {
    u8* model;
    u8* verts;
    u8* desc;
    u8* ctrl;
    u8* work;
    u32 canary;

    model = HeapAlloc(0x20, 0);
    verts = HeapAlloc(8, 0);
    desc = HeapAlloc(0x20, 0);
    *(u16*)(model + 0x0) = 0;
    *(u16*)(model + 0x2) = 1;
    *(u32*)(model + 0x8) = (u32)(uintptr_t)verts;
    *(u32*)(model + 0xC) = 0;
    *(u32*)(model + 0x1C) = (u32)(uintptr_t)desc;
    *(s32*)(desc + 0x0) = 1;
    *(s32*)(desc + 0x4) = 1;
    *(u32*)(desc + 0x8) = 0;
    *(u32*)(desc + 0xC) = 0;
    *(u32*)(desc + 0x10) = 0;

    work = func_800303C8(model, 0);
    assert(work != NULL);
    assert(*(u32*)(work + 0x0) == (u32)(uintptr_t)model);
    assert(*(u32*)(work + 0x4) == (u32)(uintptr_t)verts);
    assert(*(s32*)(work + 0xC) == 1);
    assert(*(u32*)(work + 0x10) == (u32)(uintptr_t)(work + 0x14));
    assert(*(u32*)(work + 0x14) == (u32)(uintptr_t)func_8003014C);
    assert(*(s32*)(work + 0x18) == 0);
    assert(*(s32*)(work + 0x1C) == 0);
    assert(*(u32*)(model + 0x8) != (u32)(uintptr_t)verts);

    ctrl = align24();
    *(u32*)(ctrl + 0x4) = (u32)(uintptr_t)model;
    *(u32*)(ctrl + 0x18) = 0xA5A5A5A5u;
    canary = *(u32*)(ctrl + 0x18);
    *(u32*)(ctrl + 0x14) = (u32)(uintptr_t)work;
    assert(*(u32*)(ctrl + 0x14) == (u32)(uintptr_t)work);
    assert(*(u32*)(ctrl + 0x18) == canary);

    g_copy_calls = g_pose_calls = g_cb_calls = 0;
    func_800305D8((u8*)(uintptr_t)*(u32*)(ctrl + 0x14));
    assert(g_copy_calls == 1);
    assert(g_copy_count == 1);
    assert(g_copy_dst == (u8*)(uintptr_t)*(u32*)(model + 0x8));
    assert(g_copy_src == desc + 4);
    assert(g_cb_calls == 1);
    assert(g_pose_calls == 1);
    assert(g_pose_verts == g_copy_dst);
    assert(g_pose_scale == 0x1234);

    puts("SKELETAL ANIM SLOTS PASS 4-byte work-block publish and tick");
    return 0;
}
