/*
 * Focused production-linked oracle for retail helper 0x800981C8.
 *
 * Expected values are hand-derived from [0x800981C8, 0x800983A0).
 * NEXT_CALLBACK_TARGET is not an oracle input.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_981c8.h"

#define POS     0x8009BE28u
#define DIM_X   0x8009D160u
#define DIM_Z   0x8009D2B4u
#define CELL_X  0x8009C838u
#define CELL_Z  0x8009C83Cu
#define DST     0x8009D318u
#define SRC     0x8009D570u

static int s_failures;

static void chk(const char *n, u32 got, u32 want)
{
    if (got != want) {
        fprintf(stderr, "ASSERTION %s: got=0x%08x expected=0x%08x\n", n, got,
                want);
        if (++s_failures > 40)
            exit(1);
    }
}

static u32 rd32(u32 a)
{
    u32 v;

    memcpy(&v, PSX_ADDR(a), 4);
    return v;
}

static u16 rd16(u32 a)
{
    u16 v;

    memcpy(&v, PSX_ADDR(a), 2);
    return v;
}

static void wr32(u32 a, u32 v) { memcpy(PSX_ADDR(a), &v, 4); }
static void wr16(u32 a, u16 v) { memcpy(PSX_ADDR(a), &v, 2); }

static void paint_src(void)
{
    u32 i;

    for (i = 0; i < 0xA2u; i++)
        ((u8 *)PSX_ADDR(SRC))[i] = (u8)(0xA0u + (i & 0x7Fu));
    memset(PSX_ADDR(DST), 0x5A, 0xA4);
}

static void setup(s32 x, s32 z, s32 dim_x, s32 dim_z, s16 cx, s16 cz)
{
    wr32(POS, (u32)x);
    wr32(POS + 4u, 0x22222222u);
    wr32(POS + 8u, (u32)z);
    wr32(DIM_X, (u32)dim_x);
    wr32(DIM_Z, (u32)dim_z);
    wr16(CELL_X, (u16)cx);
    wr16(CELL_Z, (u16)cz);
    paint_src();
}

static void chk_copy(const char *tag)
{
    u32 i;
    char n[64];

    for (i = 0; i < 0xA2u; i++) {
        u8 got = ((u8 *)PSX_ADDR(DST))[i];
        u8 want = (u8)(0xA0u + (i & 0x7Fu));

        if (got != want) {
            snprintf(n, sizeof n, "%s.copy.%u", tag, i);
            chk(n, got, want);
            break;
        }
    }
}

static void chk_h(const char *tag, u32 i, u32 want)
{
    char n[64];

    snprintf(n, sizeof n, "%s.g%u", tag, i);
    chk(n, rd16(SRC + i * 2u), want);
}

int main(void)
{
    PsxMemory_Init();

    /* cells=-2 → bias 0; dim 4; pos 0 → start (0,0) */
    setup(0, 0, 4, 4, -2, -2);
    wm_800981C8(POS);
    chk("zero.y", rd32(POS + 4u), 0x22222222u);
    chk_copy("zero");
    chk_h("zero", 0, 0);
    chk_h("zero", 1, 1);
    chk_h("zero", 3, 3);
    chk_h("zero", 4, 0);
    chk_h("zero", 8, 4);
    chk_h("zero", 32, 0);
    chk_h("zero", 56, 12);

    /* x=-1 → (>>12)+7>>3 = 0? -1+7=6>>3=0. Use for neg-adjust:
     * -1>>12 = -1; without +7, -1>>3 = -1; wrap -1+0x400=0x3FF>>8=3 */
    setup(-1, 0, 4, 4, -2, -2);
    wm_800981C8(POS);
    chk_copy("neg");
    chk_h("neg", 0, 0);
    chk_h("neg", 1, 1);

    /* Same dims; x=-1 with SKIP_NEG_ADJUST yields start_x=3.
     * Production must stay at 0. Already checked.
     * Dedicated wrap-from-negative via cell bias. */
    setup(0, 0, 4, 4, 0, 0);
    wm_800981C8(POS);
    chk_copy("bias");
    /* start (2,2): row0 = 10,11,8,9,10,11,8,9 */
    chk_h("bias", 0, 10);
    chk_h("bias", 1, 11);
    chk_h("bias", 2, 8);
    chk_h("bias", 3, 9);
    chk_h("bias", 8, 14);
    chk_h("bias", 16, 2);
    chk_h("bias", 17, 3);
    chk_h("bias", 18, 0);

    /* x=0x800000, cells=-2 → start_x=1 */
    setup((s32)0x00800000, 0, 4, 4, -2, -2);
    wm_800981C8(POS);
    chk_copy("bigx");
    chk_h("bigx", 0, 1);
    chk_h("bigx", 1, 2);
    chk_h("bigx", 3, 0);
    chk_h("bigx", 4, 1);

    /* z bias only */
    setup(0, 0, 4, 4, -2, 0);
    wm_800981C8(POS);
    chk_h("zbias", 0, 8);
    chk_h("zbias", 8, 12);
    chk_h("zbias", 16, 0);

    if (s_failures) {
        fprintf(stderr, "ASSERTION failures=%d\n", s_failures);
        return 1;
    }
    printf("W34B24-I24 0x800981C8 focused oracle PASS\n");
    return 0;
}
