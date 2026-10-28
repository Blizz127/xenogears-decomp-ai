#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef int s32;
typedef short s16;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned int u_int;
typedef signed char s8;
typedef unsigned char u8;

extern unsigned char g_FieldBss_800AFE9C[];
extern s16 D_800AFEA8;
extern s16 D_800AFFAC[];
extern u32 D_800AFB20[];

s32 D_800ADB8C;

typedef struct {
    u16 scriptInstructionPointer;
} ActorData;
static ActorData s_actor;
ActorData* g_FieldScriptVMCurActor = &s_actor;

static int s_imm[0x10];
static u8 s_heap_bufs[4][0x20];
static uintptr_t s_allocs[8];
static unsigned s_alloc_count;
static uintptr_t s_init_line;
static uintptr_t s_init_buf;
static s16 s_init_count;

int FieldScriptVMGetInstructionArgument(int index)
{
    if (index < 0 || index >= 0x10) {
        return 0;
    }
    return s_imm[index];
}

int FieldScriptVMGetArgument(int index)
{
    return FieldScriptVMGetInstructionArgument(index);
}

void* HeapAlloc(u_int allocSize, u_int allocFlags)
{
    void* p;
    (void)allocFlags;
    if (s_alloc_count >= 4 || allocSize > sizeof(s_heap_bufs[0])) {
        return NULL;
    }
    p = s_heap_bufs[s_alloc_count];
    memset(p, 0, sizeof(s_heap_bufs[0]));
    if ((uintptr_t)p > 0xFFFFFFFFu) {
        return NULL;
    }
    s_allocs[s_alloc_count] = (uintptr_t)p;
    ++s_alloc_count;
    return p;
}

void* GfxLineScrollInitialize(void* pLineScroll, u16 x, u16 y, u16 modulus, s16 width,
                              s16 numLines, u16 destX, u16 destY, s8* pData)
{
    (void)x;
    (void)y;
    (void)modulus;
    (void)width;
    (void)destX;
    (void)destY;
    s_init_line = (uintptr_t)pLineScroll;
    s_init_buf = (uintptr_t)pData;
    s_init_count = numLines;
    return pLineScroll;
}

extern void func_800921E8(void);
extern void func_80092148(void);

static int fail(const char* msg)
{
    fprintf(stderr, "LINE SCROLL SLOT FAIL %s\n", msg);
    return 1;
}

int main(void)
{
    u32* pLines;
    u32* pBufs;
    s16* pSizes;

    if ((uintptr_t)&D_800AFEA8 - (uintptr_t)g_FieldBss_800AFE9C != 0x00C) {
        return fail("D_800AFEA8 alias");
    }
    if ((uintptr_t)D_800AFFAC - (uintptr_t)g_FieldBss_800AFE9C != 0x110) {
        return fail("D_800AFFAC alias");
    }
    if ((uintptr_t)D_800AFFAC - (uintptr_t)&D_800AFEA8 != 0x104) {
        return fail("size table is not D_800AFEA8+0x104");
    }

    memset(g_FieldBss_800AFE9C, 0, 0x150);
    D_800AFB20[0] = 0xABCDEF01u;
    D_800ADB8C = 0;
    D_800AFEA8 = 0;
    s_actor.scriptInstructionPointer = 0x40;
    s_alloc_count = 0;
    memset(s_imm, 0, sizeof(s_imm));
    s_imm[9] = 4;
    s_imm[0xF] = 0x5A;
    s_imm[1] = 1;
    s_imm[3] = 2;
    s_imm[5] = 3;
    s_imm[7] = 8;
    s_imm[0xB] = 4;
    s_imm[0xD] = 5;

    /* Walk the shared BSS object, not &D_800AFEA8: GCC treats the s16
     * alias as a 2-byte object and rejects the packed-table offsets at -O2. */
    pLines = (u32*)(g_FieldBss_800AFE9C + 0x010);
    pBufs = (u32*)(g_FieldBss_800AFE9C + 0x090);
    pSizes = (s16*)(g_FieldBss_800AFE9C + 0x110);
    pLines[1] = 0xCAFE0001u;
    pBufs[1] = 0xCAFE0002u;

    func_800921E8();

    if (D_800AFEA8 != 1) {
        return fail("count increment");
    }
    if (pSizes[0] != 4 || D_800AFFAC[0] != 4) {
        return fail("size table write");
    }
    if (pLines[0] != (u32)s_allocs[1] || pBufs[0] != (u32)s_allocs[0]) {
        return fail("truncated pointer store");
    }
    if (pLines[1] != 0xCAFE0001u || pBufs[1] != 0xCAFE0002u) {
        return fail("8-byte store clobbered neighbour slot");
    }
    if (D_800AFB20[0] != 0xABCDEF01u) {
        return fail("size write overflowed into D_800AFB20");
    }
    if (s_init_line != s_allocs[1] || s_init_buf != s_allocs[0] || s_init_count != 4) {
        return fail("GfxLineScrollInitialize args");
    }
    if (s_actor.scriptInstructionPointer != 0x51) {
        return fail("instruction pointer");
    }

    s_imm[9] = 2;
    s_imm[0xF] = 0xA5;
    func_800921E8();
    if (D_800AFEA8 != 2 || pSizes[1] != 2 || D_800AFFAC[1] != 2) {
        return fail("second slot size");
    }
    if (pLines[1] != (u32)s_allocs[3] || pBufs[1] != (u32)s_allocs[2]) {
        return fail("second slot 4-byte stride");
    }
    if (pLines[0] != (u32)s_allocs[1] || pBufs[0] != (u32)s_allocs[0]) {
        return fail("second store clobbered slot 0");
    }

    s_imm[1] = 0;
    s_imm[3] = 2;
    s_imm[5] = 0x2B;
    func_80092148();
    if (((u8*)s_allocs[0])[2] != 0x2B) {
        return fail("func_80092148 buffer poke");
    }
    if (s_actor.scriptInstructionPointer != 0x51 + 0x11 + 7) {
        return fail("func_80092148 ip");
    }

    puts("LINE SCROLL SLOT PASS packed u32 stores, D_800AFFAC alias, neighbour canaries");
    return 0;
}
