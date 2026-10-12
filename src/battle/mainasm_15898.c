/* battle mainasm_15898: the retail-asm run at file offset 0x15898, in retail order.
 * Functions still listed as INCLUDE_ASM are not byte-matching C yet. The whole
 * file is retail-build only; any port body for these functions lives in the
 * port's own TU. */
#include "common.h"

#ifndef XENO_PC_PORT
/* One of the 32 72-byte action rows at D_800C3FE8. */
typedef struct {
    u16 unk0[12];
    u8 unk18[12];
    u16 unk24[12];
    u8 unk3C[12];
} BattleActionRow;

extern BattleActionRow D_800C3FE8[];
extern u8* D_800C3EAC;

/* func_80085388.s: clear entries 0..10 of the current action row
 * (D_800C3EAC[0x2DA]): the two halfword arrays to 0, the two byte arrays to
 * 0xFF.  The row index is re-read for every store, as retail does. */
void func_80085388(void) {
    s32 i;

    for (i = 0; i < 11; i++) {
        D_800C3FE8[D_800C3EAC[0x2DA]].unk0[i] = 0;
        D_800C3FE8[D_800C3EAC[0x2DA]].unk18[i] = 0xFF;
        D_800C3FE8[D_800C3EAC[0x2DA]].unk24[i] = 0;
        D_800C3FE8[D_800C3EAC[0x2DA]].unk3C[i] = 0xFF;
    }
}

INCLUDE_ASM("asm/battle/nonmatchings/mainasm_15898", func_80085454);
INCLUDE_ASM("asm/battle/nonmatchings/mainasm_15898", func_80085618);

extern u8 D_800CCD34[];
extern u8 D_800CCD36[];
extern u8 D_800CCD68[];
extern u8 D_800CCD74[];

/* func_80085AC4.s: for actor a0 (368-byte stride) copy the halfword at
 * D_800CCD36 to D_800CCD34, then zero halfwords backwards, 4 bytes apart:
 * two from D_800CCD68 and three from D_800CCD74.  Each counter is set
 * before its pointer.  sched1 then keeps the D_800CCD68 address live across
 * the halfword copy, as retail does, and the copy lands in $a0, so the
 * parameter moves to $a2.  Setting the counter after the pointer is one
 * instruction short. */
void func_80085AC4(u8 a0) {
    s32 i;
    u16* p;

    i = 2;
    p = (u16*)(D_800CCD68 + a0 * 368);
    *(u16*)(D_800CCD34 + a0 * 368) = *(u16*)(D_800CCD36 + a0 * 368);
    do {
        *p = 0;
        p -= 2;
        i -= 2;
    } while (i >= 0);
    i = 4;
    p = (u16*)(D_800CCD74 + a0 * 368);
    do {
        *p = 0;
        p -= 2;
        i -= 2;
    } while (i >= 0);
}
#endif
