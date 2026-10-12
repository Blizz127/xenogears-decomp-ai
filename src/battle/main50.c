#include "common.h"


#ifndef XENO_PC_PORT
typedef struct {
    u16 x;
    u16 y;
    u16 w;
    u16 h;
    u16 curW;
    u16 curH;
    u8 kind;
} BattleWindowRect;

extern u8* D_800D2D28;
extern BattleWindowRect* D_800D2D90[];
extern void func_8008F6E4(u8 kind, u16 x, u16 y, u16 w, s32 h);

/* Open animation of the seven battle windows (D_800D2D90): every window
 * that is shown (D_800D2D28 +0xB8 + i) and not yet open (+0xBF + i) grows its
 * current size by 0x20 per frame up to its full size, is marked open once
 * both axes are full, and is redrawn centred (func_8008F6E4). */
void func_8008FAD8(void) {
    s32 i;
    BattleWindowRect* r;
    u8* f;
    u8 done;

    for (i = 0; i < 7; i++) {
        r = D_800D2D90[i];
        if ((D_800D2D28 + i)[0xB8] != 0 && (D_800D2D28 + i)[0xBF] == 0) {
            done = 0;
            if (r->curW + 0x20 >= r->w) {
                r->curW = r->w;
                done = 1;
            } else {
                r->curW += 0x20;
            }
            if (r->curH + 0x20 >= r->h) {
                r->curH = r->h;
                done++;
            } else {
                r->curH += 0x20;
            }
            if (done == 2) {
                (D_800D2D28 + i)[0xBF] = 1;
            }
            func_8008F6E4(r->kind, r->x + r->w / 2 - r->curW / 2, r->y + r->h / 2 - r->curH / 2, r->curW, r->curH);
        }
    }
}
INCLUDE_ASM("asm/battle/nonmatchings/main50", func_8008FC1C);
#endif


extern void func_8008FC1C(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4);


/* func_8008FDE4.s */
void func_8008FDE4(void) {
    func_8008FC1C(0x20, 0x5C, 0xCC, 0x60, 0xE);
}
