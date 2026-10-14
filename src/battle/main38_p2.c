/* Retail TU part 2 of main38.c (see the BATTLE_TU_PART note there): the
 * still-assembly run below, then that file's part-2 C bodies. */
#define BATTLE_TU_PART 2
#include "common.h"
#ifndef XENO_PC_PORT
extern u8 D_800D2D24[];
extern u8* D_800D2DB4;
extern s32 D_800CCB34;
extern void func_80076D58(u8* prims, u8 sel, u8 mode);
extern void* func_80033784(s32 table, s32 id);
extern s32 SystemRenderStringEntry(void* str, u32 buf, s32 h, s32 flag);
typedef struct {
    s16 x, y, w, h;
} BattleRect;
extern void LoadImage(BattleRect* rect, u32 buf);
extern void func_80076C78(u8* p, u16 x, u16 y, u8 u, u8 v, u8 w);

/* func_80086028.s, byte-exact: draw entry `id` of character
 * D_800D2D24[actor] in slot `slot` and return slot + 1.  The slot's two
 * POLY_FT4s at D_800D2DB4 + 0x5550 + slot * 80 are initialised
 * (func_80076D58(prims, slot % 2, 3)); the func_80033784 string is
 * rendered into *buf (SystemRenderStringEntry, height 0x1B, slot % 2) and
 * uploaded to the 30 x 13 VRAM rect at (0x3C0 + slot / 2 * 30, 0x1A);
 * then func_80076C78 sets the D_800CCB34 buffer's prim to x = (row + flag
 * + 1) * 16 + 0x50 + slot * 4, y = 0xC8 - slot * 16, u = slot / 2 * 120,
 * v = 0x1A and the rendered width, and the D_800D2DB4 +0x5D80 count goes
 * up by one.
 * Codegen: the prim offsets are parenthesised (offset first, then the
 * base); `flag + 1` goes through a local, so fold does not reassociate it
 * into (row + 1) + flag; `return ++slot` gives slot the extra references
 * that rank it above `odd` in local-alloc (s3 vs s4).  Matches only under
 * gcc 2.6.x (2.7.2: diff 8/106). */
s32 func_80086028(u8 actor, s32 slot, s32 row, u8 id, u32* buf, u8 flag) {
    BattleRect rect;
    s32 half = slot / 2;
    s32 odd = slot % 2;
    s32 w;
    s32 n;

    func_80076D58(D_800D2DB4 + (slot * 80 + 0x5550), odd, 3);
    w = SystemRenderStringEntry(func_80033784(D_800D2D24[actor], id), *buf, 0x1B, odd);
    rect.x = half * 30 + 0x3C0;
    rect.y = 0x1A;
    rect.w = 0x1E;
    rect.h = 0xD;
    LoadImage(&rect, *buf);
    n = flag + 1;
    func_80076C78(D_800D2DB4 + ((slot * 2 + D_800CCB34) * 40 + 0x5550),
                  (row + n) * 16 + 0x50 + slot * 4, 0xC8 - slot * 16, half * 120, 0x1A, w);
    D_800D2DB4[0x5D80]++;
    return ++slot;
}
INCLUDE_ASM("asm/battle/nonmatchings/main38_p2", func_800861D0);
extern u8* D_800C3EAC;
extern u8 D_800D2C34[];
extern u8 D_800D2D24[];
extern u8 D_800C34CC[];
extern u8 g_GameState[];
extern u32 func_80089C6C(u32 a0, u8 a1);

/* func_80086B88.s, byte-exact: 1 when entry `index` may be used in the
 * current command context (D_800C3EAC).  With +0x2D6 clear it always may.
 * Otherwise its byte of D_800C34CC (at index + (+0x2CC + 1) * 3, or
 * index + 12 when D_800D2C34 is 4) must be allowed by func_80089C6C in
 * the g_GameState +0x16C4 mask of character D_800D2D24[slot], and unless
 * +0x2CC is 0xFF or D_800D2C34 is 4, D_800D2C34 must exceed +0x2CC. */
s32 func_80086B88(u8 index, u8 slot) {
    u8 i = index + (D_800C3EAC[0x2CC] + 1) * 3;
    s32 ok = 1;

    if (D_800C3EAC[0x2D6] != 0) {
        if (D_800D2C34[0] == 4) {
            i = index + 12;
        }
        if ((u16)func_80089C6C(*(u16*)(g_GameState + 0x16C4 + D_800D2D24[slot] * 32), D_800C34CC[i]) == 0) {
            ok = 0;
        } else if (D_800C3EAC[0x2CC] != 0xFF && D_800D2C34[0] != 4 && D_800D2C34[0] < D_800C3EAC[0x2CC] + 1) {
            ok = 0;
        }
    }
    return ok;
}
INCLUDE_ASM("asm/battle/nonmatchings/main38_p2", func_80086C88);
INCLUDE_ASM("asm/battle/nonmatchings/main38_p2", func_80086F98);
INCLUDE_ASM("asm/battle/nonmatchings/main38_p2", func_800877E0);
#endif
#include "main38.c"
