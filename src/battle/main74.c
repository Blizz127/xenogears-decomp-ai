#include "common.h"


#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/main74", func_800B00F4);
INCLUDE_ASM("asm/battle/nonmatchings/main74", func_800B0164);
INCLUDE_ASM("asm/battle/nonmatchings/main74", func_800B026C);
INCLUDE_ASM("asm/battle/nonmatchings/main74", func_800B0AB4);
INCLUDE_ASM("asm/battle/nonmatchings/main74", func_800B0B14);
INCLUDE_ASM("asm/battle/nonmatchings/main74", func_800B0D70);
INCLUDE_ASM("asm/battle/nonmatchings/main74", func_800B0FF4);
INCLUDE_ASM("asm/battle/nonmatchings/main74", func_800B10EC);
INCLUDE_ASM("asm/battle/nonmatchings/main74", func_800B12D0);
INCLUDE_ASM("asm/battle/nonmatchings/main74", func_800B136C);
#endif


#ifndef XENO_PC_PORT
extern u32 D_800D3344;
#endif
#ifndef XENO_PC_PORT
extern u32 D_800D39CC;
#endif
#ifndef XENO_PC_PORT
extern u8 D_800C3B74;
#endif
#ifndef XENO_PC_PORT
extern u8 D_800C3D6C;
#endif
#ifndef XENO_PC_PORT
extern u8* D_800C3610;
#endif


/* func_800B14B8.s */
void func_800B14B8(void) {
    D_800C3D6C = 1;
}


#ifdef XENO_PC_PORT
#include "battle_host_guest.h"
/* Native owner: src/slus_006.64/system/asset_loader.c stores a host pointer. */
extern u8* D_800658C8;

/* func_800B0AB4.s: is the (x, _, z) halfword point strictly inside the
 * rectangle at D_800658C8 + 0x34C (x max, x min, z min, z max)? */
u32 func_800B0AB4(s16* point) {
    u8* area = D_800658C8;
    s32 x = point[0];
    s32 z;

    if (!(*(s16*)(area + 0x34E) < x)) {
        return 0;
    }
    if (!(x < *(s16*)(area + 0x34C))) {
        return 0;
    }
    z = point[2];
    if (!(*(s16*)(area + 0x350) < z)) {
        return 0;
    }
    return z < *(s16*)(area + 0x352);
}

/* func_800B12D0.s: 1 when the command-mask bit selected by the current
 * actor's (D_800C360C - 1) 0x48-byte record at D_800C4000, byte `index`, is
 * clear in `mask`; the jtbl_800707C4 cases are 0,1 -> bit 0, 2,3 -> bit 3,
 * 4 -> bit 2, 5 -> bit 1.  Any other kind tests the whole low byte. */
u32 func_800B12D0(u32 index, u32 mask) {
    u32 kind = BG_U8(0x800C4000 + (BG_U32(0x800C360C) - 1) * 0x48 + index);
    u32 bit;

    switch (kind) {
    case 0:
    case 1:
        bit = 1;
        break;
    case 2:
    case 3:
        bit = 8;
        break;
    case 4:
        bit = 4;
        break;
    case 5:
        bit = 2;
        break;
    default:
        return (mask & 0xFF) == 0;
    }
    return (mask & bit) == 0;
}
#endif
