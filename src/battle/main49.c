#include "common.h"


#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/main49", func_8008CDE4);
INCLUDE_ASM("asm/battle/nonmatchings/main49", func_8008CFB8);
INCLUDE_ASM("asm/battle/nonmatchings/main49", func_8008D328);
INCLUDE_ASM("asm/battle/nonmatchings/main49", func_8008D598);
INCLUDE_ASM("asm/battle/nonmatchings/main49", func_8008DC34);
INCLUDE_ASM("asm/battle/nonmatchings/main49", func_8008DE04);
INCLUDE_ASM("asm/battle/nonmatchings/main49", func_8008E430);
INCLUDE_ASM("asm/battle/nonmatchings/main49", func_8008EA70);
INCLUDE_ASM("asm/battle/nonmatchings/main49", func_8008F0A8);
extern u8* D_800D2D28;
extern u32 D_800D2E38[];
extern s32 D_800CCB34__a[] asm("D_800CCB34");
void func_8008DC34(u32 kind, u16 x, u16 y, u16 w, u16 h);
void func_8008DE04(u32 kind, u16 x, u16 y, u16 w);
void func_8008E430(u32 kind, u16 x, u16 y, u16 w, u16 h);
void func_8008EA70(u32 kind, u16 x, u16 y, u16 h);
void func_8008F0A8(u32 kind, u16 x, u16 y, u16 w, u16 h);

/* Draw battle window `kind` at (x, y), w x h: with its D_800D2D28 +0xB0
 * flag dropped, set the corners of the current buffer's background quad
 * (POLY_G4 at block +0x3C0, 0x24 per buffer), rebuild the frame parts
 * (func_8008DC34 .. func_8008F0A8), record the buffer at +0x5A4 and raise
 * the flag again.  The buffer index is re-read for every corner. */
void func_8008F6E4(u8 kind, u16 x, u16 y, u16 w, s32 h) {
    u32 i = kind;
    u8* blk = (u8*)D_800D2E38[i];

    *(D_800D2D28 + i + 0xB0) = 0;
    *(s16*)(blk + D_800CCB34__a[0] * 0x24 + 0x3C8) = x;
    *(s16*)(blk + D_800CCB34__a[0] * 0x24 + 0x3CA) = y;
    *(s16*)(blk + D_800CCB34__a[0] * 0x24 + 0x3D0) = x + w;
    *(s16*)(blk + D_800CCB34__a[0] * 0x24 + 0x3D2) = y;
    *(s16*)(blk + D_800CCB34__a[0] * 0x24 + 0x3D8) = x;
    *(s16*)(blk + D_800CCB34__a[0] * 0x24 + 0x3DA) = y + h;
    *(s16*)(blk + D_800CCB34__a[0] * 0x24 + 0x3E0) = x + w;
    *(s16*)(blk + D_800CCB34__a[0] * 0x24 + 0x3E2) = y + h;
    func_8008DC34(i, x, y, w, h);
    func_8008DE04(i, x, y, w);
    func_8008E430(i, x, y, w, h);
    func_8008EA70(i, x, y, h);
    func_8008F0A8(i, x, y, w, h);
    blk[0x5A4] = *(u8*)D_800CCB34__a;
    *(D_800D2D28 + i + 0xB0) = 1;
}
extern u8* D_800D2D28;
extern u32 D_800D2E38[];
/* Battle window rectangle (D_800D2D90), as main50's open animation reads
 * it. */
typedef struct {
    u16 x;
    u16 y;
    u16 w;
    u16 h;
    u16 curW;
    u16 curH;
    u8 kind;
} BattleWindowRect;
extern BattleWindowRect* D_800D2D90__p[] asm("D_800D2D90");
extern void* func_8008ABB8(s32 size, s32 mode);
extern void bzero(void* p, s32 n);
extern void func_80077454(u32 index);
extern void func_8008F6E4(u8 kind, u16 x, u16 y, u16 w, s32 h);
extern void func_800716D8(void);

/* Open battle window `index` at (x, y) with size w x h.  On first use
 * (D_800D2D28 +0xB0 + index clear) allocate and clear its 0x5A8-byte
 * D_800D2E38 block and 0xE-byte rectangle, then run func_80077454.
 * `animate` stores the rectangle with a zero current size and marks the
 * window shown but not yet open (+0xB8 / +0xBF), for func_8008FAD8 to grow;
 * otherwise it is drawn at full size now (func_8008F6E4), followed by
 * func_800716D8 when `refresh` is set. */
void func_8008F8F4(u8 index, u16 x, u16 y, u16 w, u16 h, u8 animate, u8 refresh) {
    u32 i = index;
    void* p;
    BattleWindowRect* r;

    if (*(D_800D2D28 + i + 0xB0) == 0) {
        p = func_8008ABB8(0x5A8, 0);
        D_800D2E38[i] = (u32)p;
        bzero(p, 0x5A8);
        p = func_8008ABB8(0xE, 0);
        D_800D2D90__p[i] = p;
        bzero(p, 0xE);
        func_80077454(i);
    }
    if (animate != 0) {
        r = D_800D2D90__p[i];
        r->kind = index;
        r->x = x;
        r->y = y;
        r->w = w;
        r->h = h;
        r->curW = 0;
        r->curH = 0;
        *(D_800D2D28 + i + 0xBF) = 0;
        *(D_800D2D28 + i + 0xB8) = 1;
    } else {
        func_8008F6E4(i, x, y, w, h);
        if (refresh != 0) {
            func_800716D8();
        }
    }
}
#endif


#ifndef XENO_PC_PORT
extern u8* D_800D2D28;
#endif
#ifndef XENO_PC_PORT
extern u32 D_800D2E38[];
#endif
#ifndef XENO_PC_PORT
extern u32 D_800D2D90[];
#endif
extern void func_800716D8(void);
extern void HeapFree(u32 p);


/* func_8008FA60.s */
void func_8008FA60(u8 index) {
    u32 i = (u32)index & 0xFF;
    u8* p = D_800D2D28;

    *(u8*)((int)(unsigned int)p + i + 0xB0) = 0;
    p = D_800D2D28;
    *(u8*)((int)(unsigned int)p + i + 0xB8) = 0;
    func_800716D8();
    HeapFree(D_800D2E38[i]);
    HeapFree(D_800D2D90[i]);
}
