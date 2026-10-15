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
/* func_80086F98.s, byte-exact: confirm command `index` for the battle
 * character in slot `actor` of the command menu context D_800C3EAC; the
 * caller func_80083948 returns the result from one call site and stores
 * (result == 0) in +0x2E4 at the other.
 * - With bit 7 of the actor's D_800CCE42 byte (0x170-byte records) clear
 *   and D_800D2C34 != 4: +0x2DC = D_800C34CC[index], return 1.
 * - Unless (D_800D2C34 != 0 and index < D_800D2C34) or +0x2E7 is set:
 *   +0x2DC = index, return 1.
 * - Otherwise three 0x1E text buffers (func_8008AC00) go into D_800C3A70,
 *   index is appended to the +0x2CC list (count +0x2D6; with +0x2E4 set the
 *   new tail byte is written 0xFF then index, and sel = index + (+0x2CC +
 *   1) * 3), the D_800D2DB4 prim counts +0x5D80..+0x5D82 are cleared and
 *   one glyph per listed entry is drawn at x = 0x80 + i * 16 (0 -> 0x5E,
 *   1 -> 0x5F, 2 -> 0x5D, 0xFF skipped; any other byte reuses the previous
 *   glyph, which retail leaves unset on the first entry).  With D_800D2C34
 *   == 4 sel is index + 12 (12 for index 0xFF).  When func_80089C6C allows
 *   D_800C34CC[sel] for the actor (g_GameState +0x16C4 mask), ret = 1 and
 *   func_80086C88 lists entry sel: directly with +0x2E4 set, else after
 *   glyph 8 and only while actor +0x1C is zero.  With +0x2E4 set and ret =
 *   1: ret = 0 and +0x2DC = D_800C34CC[sel] (sel = index + 12 when +0x2CC
 *   is 0xFF).  Otherwise +0x2DC = index and entries (index + 1) * 3 + 1 /
 *   + 2 (13 / 14 in mode 4) are offered with glyphs 9 / 7 (ret = 1 each)
 *   when allowed and actor +0x1E / +0x20 is zero.  Both exits copy the
 *   D_800CCB34 low byte into +0x5D8F..+0x5D91, set D_800D2D28[0xA8] = 1,
 *   run func_800716D8, free the three buffers and return ret.
 * Codegen: `(actor << 6)` keeps retail's base-first addu; D_800C3A70 is a
 * struct array so the loops keep a scaled index with symbol addressing; the
 * mode-4 sel is an if/else (the single-if form lets reorg steal the actor
 * mask into the first branch's delay slot); the first follow-up sets ret = 1
 * once after its inner if/else (both func_80086C88 calls then cross-jump and
 * reorg moves ret = 1 into the shared jal delay slot), the other two set it
 * after their calls (sched2 hoists it into the lbu delay). */
extern u8 D_800CCE42[];
typedef struct {
    u32 buf;
} M38TextBuf;
extern M38TextBuf D_800C3A70[];
extern u8* D_800D2D28;
extern s32 D_800CCB34__a[] asm("D_800CCB34");
extern void* func_8008AC00(s32 size);
extern s32 func_80076A10(s32 index, u8* polys, s16 x, s16 y);
extern s32 func_80086C88(u8 actor, s32 slot, s32 row, u8 id, u32* buf);
extern void func_800716D8(void);

s32 func_80086F98(u8 index, u8 actor) {
    u8 sel = (index + 1) * 3;
    s32 n = 0;
    s32 ret = 0;
    u8 id;
    u8 glyph;
    s32 i;
    s32 j;

    if (!(D_800CCE42[(D_800C3EAC + (actor << 6))[0x3C] * 0x170] & 0x80) && D_800D2C34[0] != 4) {
        D_800C3EAC[0x2DC] = D_800C34CC[index];
        return 1;
    }
    if ((D_800D2C34[0] != 0 && D_800D2C34[0] - 1 >= index) || D_800C3EAC[0x2E7] != 0) {
        for (j = 0; j < 3; j++) {
            D_800C3A70[j].buf = (u32)func_8008AC00(0x1E);
        }
        (D_800C3EAC + D_800C3EAC[0x2D6])[0x2CC] = index;
        D_800C3EAC[0x2D6]++;
        if (D_800C3EAC[0x2E4] != 0) {
            (D_800C3EAC + D_800C3EAC[0x2D6])[0x2CB] = 0xFF;
            (D_800C3EAC + D_800C3EAC[0x2D6])[0x2CB] = index;
            sel = index + (D_800C3EAC[0x2CC] + 1) * 3;
        }
        D_800D2DB4[0x5D81] = 0;
        D_800D2DB4[0x5D80] = 0;
        D_800D2DB4[0x5D82] = 0;
        for (i = 0; i < D_800C3EAC[0x2D6]; i++) {
            if ((D_800C3EAC + i)[0x2CC] != 0xFF) {
                switch ((D_800C3EAC + i)[0x2CC]) {
                case 0:
                    glyph = 0x5E;
                    break;
                case 1:
                    glyph = 0x5F;
                    break;
                case 2:
                    glyph = 0x5D;
                    break;
                }
                D_800D2DB4[0x5D81] += func_80076A10(glyph, D_800D2DB4 + (D_800D2DB4[0x5D81] * 0x50 + 0x5640),
                                                    i * 16 + 0x80, 0xD0 - n * 16);
            }
        }
        if (D_800D2C34[0] == 4) {
            if (index == 0xFF) {
                sel = 12;
            } else {
                sel = index + 12;
            }
        }
        id = sel;
        if ((u16)func_80089C6C(*(u16*)(g_GameState + 0x16C4 + D_800D2D24[actor] * 32), D_800C34CC[id]) != 0) {
            if (D_800C3EAC[0x2E4] == 0) {
                if (*(u16*)(D_800C3EAC + (actor << 6) + 0x1C) == 0) {
                    D_800D2DB4[0x5D81] += func_80076A10(8, D_800D2DB4 + (D_800D2DB4[0x5D81] * 0x50 + 0x5640),
                                                        i * 16 + 0x80, 0xD0 - n * 16);
                    n = func_80086C88(actor, n, i - 1, id, &D_800C3A70[n / 2].buf);
                }
            } else {
                n = func_80086C88(actor, n, i - 1, id, &D_800C3A70[n / 2].buf);
            }
            ret = 1;
        }
        if (D_800C3EAC[0x2E4] != 0 && ret != 0) {
            ret = 0;
            if (D_800C3EAC[0x2CC] == 0xFF) {
                sel = index + 12;
            }
            D_800D2DB4[0x5D90] = *(u8*)D_800CCB34__a;
            D_800C3EAC[0x2DC] = D_800C34CC[sel];
            D_800D2DB4[0x5D8F] = *(u8*)D_800CCB34__a;
            D_800D2DB4[0x5D91] = *(u8*)D_800CCB34__a;
            D_800D2D28[0xA8] = 1;
            func_800716D8();
            for (j = 0; j < 3; j++) {
                HeapFree((void*)D_800C3A70[j].buf);
            }
        } else {
            D_800C3EAC[0x2DC] = index;
            if (D_800D2C34[0] != 4) {
                id = (index + 1) * 3 + 1;
            } else {
                id = 13;
            }
            if ((u16)func_80089C6C(*(u16*)(g_GameState + 0x16C4 + D_800D2D24[actor] * 32), D_800C34CC[id]) != 0 &&
                *(u16*)(D_800C3EAC + (actor << 6) + 0x1E) == 0) {
                D_800D2DB4[0x5D81] += func_80076A10(9, D_800D2DB4 + (D_800D2DB4[0x5D81] * 0x50 + 0x5640),
                                                    i * 16 + 0x84, 0xD0 - n * 16);
                n = func_80086C88(actor, n, i, id, &D_800C3A70[n / 2].buf);
                ret = 1;
            }
            if (D_800D2C34[0] != 4) {
                id = (index + 1) * 3 + 2;
            } else {
                id = 14;
            }
            if ((u16)func_80089C6C(*(u16*)(g_GameState + 0x16C4 + D_800D2D24[actor] * 32), D_800C34CC[id]) != 0 &&
                *(u16*)(D_800C3EAC + (actor << 6) + 0x20) == 0) {
                D_800D2DB4[0x5D81] += func_80076A10(7, D_800D2DB4 + (D_800D2DB4[0x5D81] * 0x50 + 0x5640),
                                                    i * 16 + 0x88, 0xD0 - n * 16);
                func_80086C88(actor, n, i, id, &D_800C3A70[n / 2].buf);
                ret = 1;
            }
            D_800D2DB4[0x5D90] = *(u8*)D_800CCB34__a;
            D_800D2DB4[0x5D8F] = *(u8*)D_800CCB34__a;
            D_800D2DB4[0x5D91] = *(u8*)D_800CCB34__a;
            D_800D2D28[0xA8] = 1;
            func_800716D8();
            for (j = 0; j < 3; j++) {
                HeapFree((void*)D_800C3A70[j].buf);
            }
        }
    } else {
        D_800C3EAC[0x2DC] = index;
        ret = 1;
    }
    return ret;
}
INCLUDE_ASM("asm/battle/nonmatchings/main38_p2", func_800877E0);
#endif
#include "main38.c"
