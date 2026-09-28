#include "common.h"


#ifndef XENO_PC_PORT
void func_80076418();
void func_800764B4();
void func_800764EC();
extern u8 D_800C3E4C;
void func_80076544(void);
#endif

#ifndef XENO_PC_PORT

#ifndef XENO_PC_PORT
void func_80076544(void) {
    switch (D_800C3E4C) {                           /* irregular */
    case 0:
        func_800764B4();
        return;
    case 1:
        func_80076418();
        return;
    case 2:
        func_800764EC();
        return;
    }
}
#endif

INCLUDE_ASM("asm/battle/nonmatchings/main3", func_800765C4);
INCLUDE_ASM("asm/battle/nonmatchings/main3", func_80076710);
#endif


#ifndef XENO_PC_PORT
extern u32 D_800D2D40;
#endif
#ifndef XENO_PC_PORT
extern u32 D_800D2D48;
#endif
#ifndef XENO_PC_PORT
extern u8 D_800C3BCC[];
#endif
extern u16 D_8005A3A0[];
#ifndef XENO_PC_PORT
extern u8 D_800D2E62[];
#endif
#ifndef XENO_PC_PORT
extern u16 D_800D39E0;
#endif
#ifndef XENO_PC_PORT
extern u8* D_800D3278;
#endif
#ifndef XENO_PC_PORT
extern u8* D_800D2D28;
#endif
#ifndef XENO_PC_PORT
extern u8 D_800C3E4C;
#endif
#ifndef XENO_PC_PORT
extern u8 D_800D3280;
#endif
#ifndef XENO_PC_PORT
extern u8 D_800C3254[];
#endif
#ifndef XENO_PC_PORT
extern u8 D_800D32A1[];
#endif
#ifndef XENO_PC_PORT
extern u8 D_800D2D24[];
#endif
extern void LoadImage(void* pRect, void* pData);
extern void DrawSync(s32 mode);


/* func_800769E8.s */
void func_800769E8(void* pRect, void* pData) {
    LoadImage(pRect, pData);
    DrawSync(0);
}


#ifdef XENO_PC_PORT
/* Port-side bodies transcribed from the retail listings; the matching build
 * still assembles the INCLUDE_ASM bytes above. */
#include "battle_guest_call.h"

/* func_80076544.s: dispatch on the D_800C3E4C phase byte (0, 1, 2).  The
 * three targets have C bodies in main2.c, which the port does not compile
 * yet, so they run as retail bytes. */
void func_80076544(void) {
    switch (D_800C3E4C) {
    case 0:
        BATTLE_GUEST_CALL0(0x800764B4u);
        break;
    case 1:
        BATTLE_GUEST_CALL0(0x80076418u);
        break;
    case 2:
        BATTLE_GUEST_CALL0(0x800764ECu);
        break;
    }
}

/* func_800765C4.s: set up the D_800D2D28 scroll window for actor `index`
 * from the D_800C3254 layout table (row D_800D3280), then consume one of the
 * actor's D_800D2D28+0x90 counters.  `index` arrives unmasked, so every
 * address is formed in 32-bit guest arithmetic exactly as retail does. */
void func_800765C4(u32 index) {
    u32 p = BATTLE_G32(0x800D2D28u);
    u32 entry = 0x800C3254u + (((u32)D_800D3280 * 3 + index) << 1);
    u32 row = index * 96;
    u32 span;
    u32 step;

    BATTLE_G32(p + 0x44) = 0x1C;
    BATTLE_G32(p + 0x34) = row + (BATTLE_G16(entry) + 0x48);
    if (BATTLE_G8(0x800D32A1u + (index << 3)) != 0 &&
        BATTLE_G8(0x800D2D24u + index) != 7) {
        BATTLE_G32(p + 0x44) = 0x24;
        BATTLE_G32(p + 0x34) = row + (BATTLE_G16(entry) + 0x44);
    }
    p = BATTLE_G32(0x800D2D28u);
    BATTLE_G32(p + 0x3C) = 0x10;
    BATTLE_G32(p + 0x4C) = 0x98;
    BATTLE_G32(p + 0x54) = BATTLE_G32(p + 0x34) - 5 - BATTLE_G32(p + 0x3C);
    span = BATTLE_G32(p + 0x4C) - (BATTLE_G32(p + 0x44) + 5);
    step = BattleDivu(BATTLE_G32(p + 0x54) << 8, span);
    BATTLE_G16(p + 0x104) = 0x800;
    BATTLE_G8(p + 0xA9) = 6;
    BATTLE_G32(p + 0x5C) = span;
    BATTLE_G32(p + 0x5C) = 0x100;
    BATTLE_G32(p + 0x64) = 0;
    BATTLE_G32(p + 0x6C) = 0;
    BATTLE_G16(p + 0x106) = 0;
    BATTLE_G32(p + 0x54) = step;
    p = BATTLE_G32(0x800D2D28u);
    BATTLE_G8(p + 0xAB) = 1;
    p = BATTLE_G32(0x800D2D28u);
    BATTLE_G8(p + 0x90 + index) = BATTLE_G8(p + 0x90 + index) - 1;
}
#endif
