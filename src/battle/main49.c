#include "common.h"


#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/main49", func_8008CDE4);
INCLUDE_ASM("asm/battle/nonmatchings/main49", func_8008CFB8);
INCLUDE_ASM("asm/battle/nonmatchings/main49", func_8008D328);
INCLUDE_ASM("asm/battle/nonmatchings/main49", func_8008D598);
extern u32 D_800D2E38[];
extern u8 D_800C3E4C;
extern s32 D_800CCB34;
extern s32 func_80076A10(u8 glyph, u8* prims, s16 x, s16 y);
extern void func_80076B00(u8* prim);

/* Window frame corners: restart the window block's glyph count (+0x5A0)
 * and lay the four 8x8 corner glyphs (one set per D_800C3E4C mode) at the
 * corners of (x, y, w, h) through func_80076A10, each taking the next
 * 0x50-byte prim pair; then func_80076B00 on the current buffer's prim of
 * the four corner pairs. */
void func_8008DC34(u32 kind, u16 x, u16 y, u16 w, u16 h) {
    u8* blk;
    u8 glyph[4];
    s32 i;

    blk = (u8*)D_800D2E38[kind & 0xFF];
    if (D_800C3E4C != 0) {
        glyph[0] = 0x4A;
        glyph[1] = 0x4C;
        glyph[2] = 0x4F;
        glyph[3] = 0x51;
    } else {
        glyph[0] = 0xF0;
        glyph[1] = 0xF2;
        glyph[2] = 0xF5;
        glyph[3] = 0xF7;
    }
    *(s32*)(blk + 0x5A0) = 0;
    *(s32*)(blk + 0x5A0) += func_80076A10(glyph[0], blk + *(s32*)(blk + 0x5A0) * 0x50, x, y);
    *(s32*)(blk + 0x5A0) += func_80076A10(glyph[1], blk + *(s32*)(blk + 0x5A0) * 0x50, x + w - 8, y);
    *(s32*)(blk + 0x5A0) += func_80076A10(glyph[2], blk + *(s32*)(blk + 0x5A0) * 0x50, x, y + h - 8);
    *(s32*)(blk + 0x5A0) += func_80076A10(glyph[3], blk + *(s32*)(blk + 0x5A0) * 0x50, x + w - 8, y + h - 8);
    for (i = 0; i < 4; i++) {
        func_80076B00(blk + (i * 2 + D_800CCB34) * 0x28);
    }
}
extern s32 D_800CCB34__a[] asm("D_800CCB34");
extern u8* D_800C3EA4__a[] asm("D_800C3EA4");

/* Current-buffer prim field at block offset `off` (prims are 0x28 apart per
 * buffer), and a byte of the battle UI state (frame texture cells). */
#define M49_PRIM(off) (blk + D_800CCB34__a[0] * 0x28 + (off))
#define M49_UI(off) (D_800C3EA4__a[0][off])

/* Window top edge: two POLY_FT4 (block +0x140 / +0x190, per buffer) span
 * the 16-pixel band between the corners, half the inner width each, with
 * the frame texture's 8x16 edge cell; then func_80076B00 on both. */
void func_8008DE04(u32 kind, u16 x, u16 y, u16 w) {
    u8* blk;
    s32 i;

    blk = (u8*)D_800D2E38[kind & 0xFF];
    *(s16*)M49_PRIM(0x148) = x + 8;
    *(s16*)M49_PRIM(0x14A) = y - 8;
    *(s16*)M49_PRIM(0x150) = x + 8 + (w - 16) / 2;
    *(s16*)M49_PRIM(0x152) = y - 8;
    *(s16*)M49_PRIM(0x158) = x + 8;
    *(s16*)M49_PRIM(0x15A) = y + 8;
    *(s16*)M49_PRIM(0x160) = x + 8 + (w - 16) / 2;
    *(s16*)M49_PRIM(0x162) = y + 8;
    *(s16*)M49_PRIM(0x198) = x + 8 + (w - 16) / 2;
    *(s16*)M49_PRIM(0x19A) = y - 8;
    *(s16*)M49_PRIM(0x1A0) = x + 8 + (w - 16) / 2 + (w - 16) / 2;
    *(s16*)M49_PRIM(0x1A2) = y - 8;
    *(s16*)M49_PRIM(0x1A8) = x + 8 + (w - 16) / 2;
    *(s16*)M49_PRIM(0x1AA) = y + 8;
    *(s16*)M49_PRIM(0x1B0) = x + 8 + (w - 16) / 2 + (w - 16) / 2;
    *(s16*)M49_PRIM(0x1B2) = y + 8;
    *M49_PRIM(0x14C) = (M49_UI(0xA25C) & 0x3F) * 2;
    *M49_PRIM(0x14D) = M49_UI(0xA260);
    *M49_PRIM(0x154) = (M49_UI(0xA25C) & 0x3F) * 2 + 7;
    *M49_PRIM(0x155) = M49_UI(0xA260);
    *M49_PRIM(0x15C) = (M49_UI(0xA25C) & 0x3F) * 2;
    *M49_PRIM(0x15D) = M49_UI(0xA260) + 0x10;
    *M49_PRIM(0x164) = (M49_UI(0xA25C) & 0x3F) * 2 + 7;
    *M49_PRIM(0x165) = M49_UI(0xA260) + 0x10;
    *M49_PRIM(0x19C) = (M49_UI(0xA25C) & 0x3F) * 2;
    *M49_PRIM(0x19D) = M49_UI(0xA260);
    *M49_PRIM(0x1A4) = (M49_UI(0xA25C) & 0x3F) * 2 + 7;
    *M49_PRIM(0x1A5) = M49_UI(0xA260);
    *M49_PRIM(0x1AC) = (M49_UI(0xA25C) & 0x3F) * 2;
    *M49_PRIM(0x1AD) = M49_UI(0xA260) + 0x10;
    *M49_PRIM(0x1B4) = (M49_UI(0xA25C) & 0x3F) * 2 + 7;
    *M49_PRIM(0x1B5) = M49_UI(0xA260) + 0x10;
    for (i = 0; i < 2; i++) {
        func_80076B00(blk + ((i * 2 + D_800CCB34__a[0]) * 0x28 + 0x140));
    }
}
INCLUDE_ASM("asm/battle/nonmatchings/main49", func_8008E430);
/* Window left edge: two POLY_FT4 (block +0x280 / +0x2D0) span the 16-pixel
 * band between the corners, half the inner height each, with the frame
 * texture's left cell; then func_80076B00 on both. */
void func_8008EA70(u32 kind, u16 x, u16 y, u16 h) {
    u8* blk;
    s32 i;

    blk = (u8*)D_800D2E38[kind & 0xFF];
    *(s16*)M49_PRIM(0x288) = x - 8;
    *(s16*)M49_PRIM(0x28A) = y + 8;
    *(s16*)M49_PRIM(0x290) = x + 8;
    *(s16*)M49_PRIM(0x292) = y + 8;
    *(s16*)M49_PRIM(0x298) = x - 8;
    *(s16*)M49_PRIM(0x29A) = y + 8 + (h - 16) / 2;
    *(s16*)M49_PRIM(0x2A0) = x + 8;
    *(s16*)M49_PRIM(0x2A2) = y + 8 + (h - 16) / 2;
    *(s16*)M49_PRIM(0x2D8) = x - 8;
    *(s16*)M49_PRIM(0x2DA) = y + 8 + (h - 16) / 2;
    *(s16*)M49_PRIM(0x2E0) = x + 8;
    *(s16*)M49_PRIM(0x2E2) = y + 8 + (h - 16) / 2;
    *(s16*)M49_PRIM(0x2E8) = x - 8;
    *(s16*)M49_PRIM(0x2EA) = y + 8 + (h - 16) / 2 + (h - 16) / 2;
    *(s16*)M49_PRIM(0x2F0) = x + 8;
    *(s16*)M49_PRIM(0x2F2) = y + 8 + (h - 16) / 2 + (h - 16) / 2;
    *M49_PRIM(0x28C) = (M49_UI(0xA28C) & 0x3F) * 2 + 0xE;
    *M49_PRIM(0x28D) = M49_UI(0xA290);
    *M49_PRIM(0x294) = (M49_UI(0xA28C) & 0x3F) * 2 + 0x1E;
    *M49_PRIM(0x295) = M49_UI(0xA290);
    *M49_PRIM(0x29C) = (M49_UI(0xA28C) & 0x3F) * 2 + 0xE;
    *M49_PRIM(0x29D) = M49_UI(0xA290) + 7;
    *M49_PRIM(0x2A4) = (M49_UI(0xA28C) & 0x3F) * 2 + 0x1E;
    *M49_PRIM(0x2A5) = M49_UI(0xA290) + 7;
    *M49_PRIM(0x2DC) = (M49_UI(0xA28C) & 0x3F) * 2 + 0xE;
    *M49_PRIM(0x2DD) = M49_UI(0xA290);
    *M49_PRIM(0x2E4) = (M49_UI(0xA28C) & 0x3F) * 2 + 0x1E;
    *M49_PRIM(0x2E5) = M49_UI(0xA290);
    *M49_PRIM(0x2EC) = (M49_UI(0xA28C) & 0x3F) * 2 + 0xE;
    *M49_PRIM(0x2ED) = M49_UI(0xA290) + 7;
    *M49_PRIM(0x2F4) = (M49_UI(0xA28C) & 0x3F) * 2 + 0x1E;
    *M49_PRIM(0x2F5) = M49_UI(0xA290) + 7;
    for (i = 0; i < 2; i++) {
        func_80076B00(blk + ((i * 2 + D_800CCB34__a[0]) * 0x28 + 0x280));
    }
}
INCLUDE_ASM("asm/battle/nonmatchings/main49", func_8008F0A8);
extern u8* D_800D2D28;
extern u32 D_800D2E38[];
void func_8008DC34(u32 kind, u16 x, u16 y, u16 w, u16 h);
void func_8008DE04(u32 kind, u16 x, u16 y, u16 w);
/* func_8008E430 itself reads h as a full word; this caller passes it as u16. */
extern void func_8008E430__h16(u32 kind, u16 x, u16 y, u16 w, u16 h) asm("func_8008E430");
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
    func_8008E430__h16(i, x, y, w, h);
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
#undef M49_PRIM
#undef M49_UI
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
