#include "common.h"
#include "psyq/libgpu.h"
#include "system/memory.h"

RECT* D_800592F0;
u_long* D_800592F4;

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): sprite_load_image_on_own_stack (src/decomp/src/resident/sprite_construction.c:290). */
#ifndef XM_T_STACK_ENTER
#define XM_T_STACK_ENTER
#define STACK_ENTER(top)                                                                           \
    __asm__ volatile("move $8, %0\n\tsw $29, 0($8)\n\taddiu $8, $8, -4\n\tmove $29, $8"            \
                     :                                                                             \
                     : "r"(top)                                                                    \
                     : "$8", "memory")
#endif
#ifndef XM_T_STACK_LEAVE
#define XM_T_STACK_LEAVE
#define STACK_LEAVE() __asm__ volatile("addiu $29, $29, 4\n\tlw $29, 0($29)" : : : "memory")
#endif
#ifndef XM_S_sprite_load_image_on_own_stack
#define XM_S_sprite_load_image_on_own_stack
void xm_sprite_load_image_on_own_stack(void) __asm__("GfxLoadImageAccelerated");
#endif
#ifndef XM_S_heap_alloc
#define XM_S_heap_alloc
void *xm_heap_alloc(s32 size, s32 mode) __asm__("HeapAlloc");
#endif
#ifndef XM_S_LoadImage
#define XM_S_LoadImage
int xm_LoadImage(RECT *rect, u_long *p) __asm__("LoadImage");
#endif
#ifndef XM_S_sprite_load_image_rect
#define XM_S_sprite_load_image_rect
extern RECT *xm_sprite_load_image_rect __asm__("D_800592F0");
#endif
#ifndef XM_S_sprite_load_image_pixels
#define XM_S_sprite_load_image_pixels
extern u_long *xm_sprite_load_image_pixels __asm__("D_800592F4");
#endif
#ifndef XM_S_heap_free
#define XM_S_heap_free
s32 xm_heap_free(void *data) __asm__("HeapFree");
#endif
void GfxLoadImageAccelerated(void) {
    u8 *stack = xm_heap_alloc(0x2000, 1);

    STACK_ENTER(stack + 0x1F00);
    xm_LoadImage(xm_sprite_load_image_rect, xm_sprite_load_image_pixels);
    STACK_LEAVE();
    xm_heap_free(stack);
}
#endif
#else
/* Retail 0x80022A0C: HeapAlloc(0x2000, 1) scratch block, LoadImage, HeapFree, in that order.
 * Port-only (DIVERGES): retail also runs LoadImage on a guest stack inside the block (it stores
 * the caller's guest $sp at block+0x1F00 and LoadImage's frames below it); the native LoadImage
 * uses the host stack, so those scratch-block bytes are not written. */
void GfxLoadImageAccelerated(void) {
    void* pBuffer = HeapAlloc(0x2000, 1);
    LoadImage(D_800592F0, D_800592F4);
    HeapFree(pBuffer);
}
#endif
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/graphics", GfxLoadClutsAccelerated);
#else
/*
Matches on ASPSX 2.56, GCC 2.6.0

void GfxLoadImageAccelerated(void) {
    char* pBuffer;

    pBuffer = HeapAlloc(0x2000, 1);
    
    asm(""
        "\tmove  $t0, %0\n"
        "\tsw    $sp, 0($t0)\n"
        "\taddiu $t0, $t0, -4\n"
        "\taddu  $sp, $t0, $zero\n"
    :
    : "r"(pBuffer + 0x1F00)
    : "t0", "memory"
    );
    
    LoadImage(D_800592F0, D_800592F4);
    
    asm(""
        "\taddiu $sp, $sp, 4\n"
        "\tlw    $sp, 0($sp)\n"
    ::
    : "memory"
    );
    
    HeapFree(pBuffer);
}
*/

void GfxLoadClutsAccelerated(void* pClutData, s32 x, s32 y) {
    RECT rect;
    u8* pData = pClutData;
    s32 count = *(s32*)pData;
    s32 i;

    pData += 4;
    for (i = 0; i < count; i++) {
        u8* pClut = (u8*)pClutData + ((s32*)pData)[i];
        rect.x = (s16)((u32)x + ((u32)i << 6)); /* retail addu: 32-bit wrap */
        rect.y = y;
        rect.w = *(u16*)(pClut + 0x0);
        rect.h = *(u16*)(pClut + 0x2);
        D_800592F0 = &rect;
        D_800592F4 = (u_long*)(pClut + 0x4);
        GfxLoadImageAccelerated();
    }
}
#endif
