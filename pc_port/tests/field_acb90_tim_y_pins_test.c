/* Pins func_800ACB90 against two port reconstruction bugs:
 *   1. FieldLoadTIMWithClut y must be 0x100 (retail); y=0 uploaded the TIM
 *      into framebuffer row 0 while the palette LoadImage is at y=0x100.
 *   2. The 0x200-byte fill must stay inside HeapAlloc(0x200). The old
 *      `pBuf + i*4 + 0x1FC` index wrote 0x1FC bytes past the block.
 *
 * Body extracted from src/field/main/misc9.c by the runner. Self-contained:
 * no PsyCross / game headers, so it runs without disc data. */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef int32_t s32;
typedef uint8_t u8;
typedef uint32_t u_int;
typedef uint32_t u_long;
typedef struct {
    short x, y, w, h;
} RECT;

void* D_800AF784;
static unsigned char s_tim[4];
static unsigned char s_fill[0x200 + 16];
static int s_tim_calls;
static int s_tim_x, s_tim_y, s_tim_clutx, s_tim_cluty, s_tim_clutw, s_tim_cluth;
static int s_alloc_calls;
static int s_load_calls;
static RECT s_fill_rect;
static void* s_fill_src;
static int s_free_tim;
static int s_free_fill;
static int s_drawsync;

void FieldLoadTIMWithClut(void* pTimData, short x, short y, short clutX,
                          short clutY, short clutWidth, short clutHeight)
{
    s_tim_calls++;
    assert(pTimData == s_tim);
    s_tim_x = x;
    s_tim_y = y;
    s_tim_clutx = clutX;
    s_tim_cluty = clutY;
    s_tim_clutw = clutWidth;
    s_tim_cluth = clutHeight;
}

int DrawSync(int mode)
{
    (void)mode;
    s_drawsync++;
    return 0;
}

void* HeapAlloc(u_int size, u_int flags)
{
    s_alloc_calls++;
    assert(size == 0x200);
    assert(flags == 1);
    memset(s_fill, 0xA5, sizeof(s_fill));
    return s_fill;
}

int LoadImage(RECT* rect, void* p)
{
    s_load_calls++;
    assert(p == s_fill);
    s_fill_rect = *rect;
    s_fill_src = p;
    return 0;
}

u_int HeapFree(void* p)
{
    if (p == D_800AF784) {
        s_free_tim++;
        return 0;
    }
    assert(p == s_fill);
    s_free_fill++;
    return 0;
}

#include "acb90.inc"

int main(void)
{
    unsigned i;

    D_800AF784 = s_tim;
    func_800ACB90();

    assert(s_tim_calls == 1);
    assert(s_tim_x == 0x380);
    assert(s_tim_y == 0x100);
    assert(s_tim_clutx == 0);
    assert(s_tim_cluty == 0x1FF);
    assert(s_tim_clutw == 0);
    assert(s_tim_cluth == 0);
    assert(s_alloc_calls == 1);
    assert(s_load_calls == 1);
    assert(s_fill_rect.x == 0x3C0);
    assert(s_fill_rect.y == 0x100);
    assert(s_fill_rect.w == 0x40);
    assert(s_fill_rect.h == 0x4);
    assert(s_fill_src == s_fill);
    assert(s_free_tim == 1);
    assert(s_free_fill == 1);
    assert(s_drawsync == 2);

    for (i = 0; i < 0x200; i += 4) {
        assert(*(s32*)(s_fill + i) == -1);
    }
    for (i = 0; i < 16; i++) {
        assert(s_fill[0x200 + i] == 0xA5);
    }

    printf("PASS func_800ACB90 TIM y=0x100 and in-bounds 0x200 fill\n");
    return 0;
}
