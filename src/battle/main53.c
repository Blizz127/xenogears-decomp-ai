#include "common.h"


#ifndef XENO_PC_PORT
void func_80099CF0(void *arg0, void *arg1, void *arg2);
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/main53", func_80097D5C);
extern u8* D_800C34B0;
extern u8* D_800D2D6C;
extern u8* D_800C3E00;
extern u8 D_800D2D24[];
extern u8 D_800D2D10[];
extern u16 D_800C3AA4[];
extern u8 g_GameState__b[] asm("g_GameState");

/* For each of the first three D_800D2D24 slots not set to 0x7F: select its
 * 0x170-byte record (D_800C3E00, D_800D2D6C = record + 0xA4), keep its +0x7A
 * halfword in D_800C3AA4, and for every nonzero D_800D2D10[j] subtract it
 * from the battle part's +0x98 byte and add the part's +0x50 + j byte
 * instead, capping +0x98 at 0x10.  Then raw g_GameState patches: 7 at
 * +0x7E6, bit 0x800 of +0x17C2 when +0x17DA has bit 0x2000, and a fixed set
 * of byte values while the +0x1930 halfword is below 0xBB. */
void func_8009892C(void) {
    u8 i;
    u8 j;
    u8* rec;

    for (i = 0; i < 3; i++) {
        if (D_800D2D24[i] != 0x7F) {
            rec = D_800C34B0 + i * 0x170;
            D_800C3E00 = rec;
            D_800D2D6C = rec + 0xA4;
            D_800C3AA4[i] = ((u16*)rec)[0x7A / 2];
            for (j = 0; j < 4; j++) {
                if (D_800D2D10[j] != 0) {
                    D_800D2D6C[0x98] -= D_800D2D10[j];
                    D_800D2D6C[0x98] += (D_800D2D6C + j)[0x50];
                }
            }
            if (D_800D2D6C[0x98] > 0x10) {
                D_800D2D6C[0x98] = 0x10;
            }
        }
    }
    g_GameState__b[0x7E6] = 7;
    if (*(u16*)(g_GameState__b + 0x17DA) & 0x2000) {
        *(u16*)(g_GameState__b + 0x17C2) |= 0x800;
    }
    if (*(u16*)(g_GameState__b + 0x1930) < 0xBB) {
        g_GameState__b[0x9EC] = 0xA;
        g_GameState__b[0xA90] = 0xA;
        g_GameState__b[0x10F8] = 9;
        g_GameState__b[0x119C] = 9;
        g_GameState__b[0x1240] = 8;
        g_GameState__b[0x12E4] = 0xC;
        g_GameState__b[0x1388] = 0xC;
        g_GameState__b[0x11CE] = 0x58;
        g_GameState__b[0xDF7] = 0;
        g_GameState__b[0x131C] = 0x28;
    }
}
extern u8* D_800C34B0;
extern u8* D_800D2D6C;
extern u8* D_800C3E00;
extern u8* D_800C3DFC;
extern s32 rand(void);
extern void func_80094D24(void);

/* Base value for actor `idx` (0x170-byte records at D_800C34B0: a
 * GameCharacter copy, then the battle part at +0xA4).  Selects the actor
 * (D_800C3E00, D_800D2D6C).  For idx < 3, the 0x28-byte entry picked by the
 * D_800C34B0 +0x5FC2 byte in the idx block at +0x1058 (0x5F0 apart), or at
 * +0x2228 (0x690 apart) while record +0x15A bit 7 is set, goes to D_800C3DFC
 * and the value is 9 * (stat - entry +0x27), at least 9, where stat is
 * agility (+0x5A) or the battle part's +0x98 byte respectively; for idx >= 3
 * it is 9 * agility.  The value, capped at 0xA0 above 0xA5, becomes
 * 0xA5 - value with a random -3..+4 spread; func_80094D24 runs before the
 * return. */
u8 func_80098AF8(u8 idx) {
    u8* rec;
    u16 v;

    rec = D_800C34B0 + idx * 0x170;
    D_800D2D6C = rec + 0xA4;
    D_800C3E00 = rec;
    if (idx < 3) {
        if (!(rec[0x15A] & 0x80)) {
            D_800C3DFC = (idx * 0x5F0 + 0x1058) + D_800C34B0 + D_800C34B0[0x5FC2] * 0x28;
            if (rec[0x5A] > (s8)D_800C3DFC[0x27]) {
                v = (rec[0x5A] - (s8)D_800C3DFC[0x27]) * 9;
            } else {
                v = 9;
            }
        } else {
            D_800C3DFC = (idx * 0x690 + 0x2228) + D_800C34B0 + D_800C34B0[0x5FC2] * 0x28;
            if (rec[0x13C] > (s8)D_800C3DFC[0x27]) {
                v = (rec[0x13C] - (s8)D_800C3DFC[0x27]) * 9;
            } else {
                v = 9;
            }
        }
    } else {
        v = rec[0x5A] * 9;
    }
    if (v > 0xA5) {
        v = 0xA0;
    }
    v = 0xA5 - v;
    v -= rand() % 8 - 4;
    func_80094D24();
    return v;
}
void func_80097D08();                            /* extern */
extern u8* D_800C34B0;
extern u16 D_800D220E[];
extern s16 D_800D2C98;
extern u16 D_800D2C9E;

void func_80098C6C(s32 arg0) {
    s32 var_s0;
    s32 var_s1;

    func_80097D08();
    var_s1 = 1;
    var_s0 = 0;
    do {
        if (var_s1 & *(u16 *)(D_800C34B0 + 0x5FAC)) {
            func_80098D2C(var_s0 & 0xFF, arg0 & 0xFF);
        }
        var_s1 <<= 1;
        var_s0 += 1;
    } while ((u32) (var_s0 & 0xFF) < 0xBU);
    *(u16 *)(D_800C34B0 + 0x5FB0) = D_800D220E[(arg0 & 0xFF) * 8];
    if (D_800D2C9E & 0x8000) {
        D_800D2C98 = 0xC2;
    }
}

INCLUDE_ASM("asm/battle/nonmatchings/main53", func_80098D2C);
extern u8 D_800C34AD;
extern u8* D_800C34B0;

s32 func_80099498(void) {
    u8 i;
    u8 count;
    s32 result;

    result = 0;
    switch (D_800C34AD) {
    case 2:
        count = 0;
        for (i = 0; i < 3; i++) {
            if (*(u16 *)((u8 *)((s32)D_800C34B0 - (-(i * 0x170))) + 0x7C) & 0xC002) {
                count++;
            }
        }
        if (count == 0) {
            result = 1;
        }
        break;
    case 3:
        count = 0;
        for (i = 0; i < 3; i++) {
            if (*(u16 *)((u8 *)((s32)D_800C34B0 - (-(i * 0x170))) + 0x7C) & 0xC002) {
                count++;
            }
        }
        if (count != 2) {
            result = 1;
        }
        break;
    case 1:
        break;
    }
    return result;
}

INCLUDE_ASM("asm/battle/nonmatchings/main53", func_800995A0);
INCLUDE_ASM("asm/battle/nonmatchings/main53", func_80099890);

#ifndef XENO_PC_PORT
void func_80099CF0(void *arg0, void *arg1, void *arg2) {
    if (*(u16 *)((s8*)(arg0) + 0x7C) & 0x200) {
        *(u8 *)((s8*)(arg2) + 1) = (u8) (*(u8 *)((s8*)(arg2) + 1) + 0xFF);
        if (*(u8 *)((s8*)(arg2) + 1) == 0) {
            *(u16 *)((s8*)(arg0) + 0x7C) = (u16) (*(u16 *)((s8*)(arg0) + 0x7C) & 0xFDFF);
        }
    }
    if (*(u16 *)((s8*)(arg0) + 0x7C) & 0x100) {
        *(u8 *)((s8*)(arg2) + 2) = (u8) (*(u8 *)((s8*)(arg2) + 2) + 0xFF);
        if (*(u8 *)((s8*)(arg2) + 2) == 0) {
            *(u16 *)((s8*)(arg0) + 0x7C) = (u16) (*(u16 *)((s8*)(arg0) + 0x7C) & 0xFEFF);
        }
    }
    if (*(u16 *)((s8*)(arg0) + 0x7C) & 0x80) {
        *(u8 *)((s8*)(arg2) + 3) = (u8) (*(u8 *)((s8*)(arg2) + 3) + 0xFF);
        if (*(u8 *)((s8*)(arg2) + 3) == 0) {
            *(u16 *)((s8*)(arg0) + 0x7C) = (u16) (*(u16 *)((s8*)(arg0) + 0x7C) & 0xFF7F);
        }
    }
    if (*(u16 *)((s8*)(arg0) + 0x7C) & 0x20) {
        *(u8 *)((s8*)(arg2) + 4) = (u8) (*(u8 *)((s8*)(arg2) + 4) + 0xFF);
        if (*(u8 *)((s8*)(arg2) + 4) == 0) {
            *(u16 *)((s8*)(arg0) + 0x7C) = (u16) (*(u16 *)((s8*)(arg0) + 0x7C) & 0xFFDF);
            *(u16 *)((s8*)(arg1) + 0x7C) = (u16) (*(u16 *)((s8*)(arg1) + 0x7C) & 0xEFFF);
        }
    }
    if (*(u16 *)((s8*)(arg0) + 0x7C) & 0x10) {
        *(u8 *)((s8*)(arg2) + 5) = (u8) (*(u8 *)((s8*)(arg2) + 5) + 0xFF);
        if (*(u8 *)((s8*)(arg2) + 5) == 0) {
            *(u16 *)((s8*)(arg0) + 0x7C) = (u16) (*(u16 *)((s8*)(arg0) + 0x7C) & 0xFFEF);
        }
    }
    if (*(u16 *)((s8*)(arg0) + 0x7C) & 0xF000) {
        *(u8 *)((s8*)(arg2) + 7) = (u8) (*(u8 *)((s8*)(arg2) + 7) + 0xFF);
        if (*(u8 *)((s8*)(arg2) + 7) == 0) {
            *(u16 *)((s8*)(arg0) + 0x7C) = (u16) (*(u16 *)((s8*)(arg0) + 0x7C) & 0xFFF);
        }
    }
    if (*(u16 *)((s8*)(arg0) + 0x7C) & 0xF00) {
        *(u8 *)((s8*)(arg2) + 8) = (u8) (*(u8 *)((s8*)(arg2) + 8) + 0xFF);
        if (*(u8 *)((s8*)(arg2) + 8) == 0) {
            *(u16 *)((s8*)(arg0) + 0x7C) = (u16) (*(u16 *)((s8*)(arg0) + 0x7C) & 0xF0FF);
        }
    }
    if (*(u16 *)((s8*)(arg0) + 0x80) & 0x1000) {
        *(u8 *)((s8*)(arg2) + 0xA) = (u8) (*(u8 *)((s8*)(arg2) + 0xA) + 0xFF);
        if (*(u8 *)((s8*)(arg2) + 0xA) == 0) {
            *(u16 *)((s8*)(arg0) + 0x80) = (u16) (*(u16 *)((s8*)(arg0) + 0x80) & 0xEFFF);
        }
    }
    if (*(u16 *)((s8*)(arg0) + 0x80) & 0x40) {
        *(u8 *)((s8*)(arg2) + 0xB) = (u8) (*(u8 *)((s8*)(arg2) + 0xB) + 0xFF);
        if (*(u8 *)((s8*)(arg2) + 0xB) == 0) {
            *(u16 *)((s8*)(arg0) + 0x80) = (u16) (*(u16 *)((s8*)(arg0) + 0x80) & 0xFFBF);
        }
    }
    if (*(u16 *)((s8*)(arg0) + 0x80) & 0x20) {
        *(u8 *)((s8*)(arg2) + 0xC) = (u8) (*(u8 *)((s8*)(arg2) + 0xC) + 0xFF);
        if (*(u8 *)((s8*)(arg2) + 0xC) == 0) {
            *(u16 *)((s8*)(arg0) + 0x80) = (u16) (*(u16 *)((s8*)(arg0) + 0x80) & 0xFFDF);
        }
    }
}
#endif

#endif
#ifndef XENO_PC_PORT
extern u8* D_800C34B0;
#endif
#ifndef XENO_PC_PORT
extern u8* D_800C3E00;
#endif
#ifndef XENO_PC_PORT
extern u8* D_800C3DFC;
#endif
#ifdef XENO_PC_PORT
/* Coexistence: decompiled and logic-faithful, but not byte-exact yet, so the
 * matching build assembles retail bytes below and the port uses this body. */
/* func_80099FB0.s: copy the four D_800C3DFC status bytes into the
 * D_800C34B0 +0x5FBC block, mirror +0x5FC2 into +0x5FC0, and when the
 * copied +0x5FBE byte has no low-6 bits, fold in the top nibble of the
 * combined D_800C3E00 +0x8C/+0x8E halfwords. */
void func_80099FB0(void) {
    u32 m = *(u16*)(D_800C3E00 + 0x8C) | *(u16*)(D_800C3E00 + 0x8E);
    u8* p;
    u8 v;

    D_800C34B0[0x5FBC] = D_800C3DFC[0x20];
    D_800C34B0[0x5FBD] = D_800C3DFC[0x21];
    D_800C34B0[0x5FBE] = D_800C3DFC[0x22];
    D_800C34B0[0x5FBF] = D_800C3DFC[0x23];
    D_800C34B0[0x5FC0] = D_800C34B0[0x5FC2];
    p = D_800C34B0;
    v = p[0x5FBE];
    if ((v & 0x3F) == 0) {
        p[0x5FBE] = (m >> 12) | v;
    }
}
#else
INCLUDE_ASM("asm/battle/nonmatchings/main53", func_80099FB0);
#endif
#ifndef XENO_PC_PORT
extern u8* D_800C34B0;
#endif


/* func_8009A074.s */
void func_8009A074(void) {
    if ((*(u16*)(D_800C34B0 + 0x5FAC) & 7) != 0) {
        *(u16*)(D_800C34B0 + 0x5FAC) = *(u16*)(D_800C34B0 + 0x5FAE) & 7;
    }
    if ((*(u16*)(D_800C34B0 + 0x5FAC) & 0x7F8) != 0) {
        *(u16*)(D_800C34B0 + 0x5FAC) = *(u16*)(D_800C34B0 + 0x5FAE) & 0x7F8;
    }
}
