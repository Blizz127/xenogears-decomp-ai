/* battling misc: retail functions in retail order (asm/battling/CB8.s).
 * Functions still listed as INCLUDE_ASM are not byte-matching C yet. The whole
 * file is retail-build only. */
#include "common.h"
#include "system/memory.h"

#ifndef XENO_PC_PORT
extern s32 D_800912F4;
extern u8 D_80099D98[];
extern u8 D_80099D98__s_u8 asm("D_80099D98");
s32 func_8007F97C(void);
#endif

#ifndef XENO_PC_PORT
extern s32 D_80092AD8;
void func_8007BB7C(void);
#endif

#ifndef XENO_PC_PORT
s32 GetStringEntry(s32, s32);
void func_80034714(s32 *, s32);
void func_8007191C(s32);
extern s32 D_80090F38[];
extern s32 D_800925DC[];
extern s8 D_80092604[];
extern s32 D_80092880[];
extern s8 D_80092884__a_s8[] asm("D_80092884");
extern s32 D_800928C8__a_s32[] asm("D_800928C8");
extern s32 D_80092900__a_s32[] asm("D_80092900");
extern s32 D_80092904[];
extern s32 D_80092954__a_s32[] asm("D_80092954");
extern s8 D_800929BC[];
extern s8 D_80099D9D__a_s8[] asm("D_80099D9D");
extern s8 D_80099D9E__a_s8[] asm("D_80099D9E");
void func_800719F0(void);

extern u8 D_80099DA1__a[] asm("D_80099DA1");
extern u8 D_80099D9A__a[] asm("D_80099D9A");
extern u8 D_80099D9D__a[] asm("D_80099D9D");
extern u8 D_80099D9E__a[] asm("D_80099D9E");
extern u8 D_80099D9F__a[] asm("D_80099D9F");
extern u8 D_80099DA2__a[] asm("D_80099DA2");
void func_80080090(void);
void func_800800CC(void);
void func_80080180(void);
void func_800801BC(void);
void func_800801F8(void);
void func_80080268(void);

extern u8 D_80099D98[];
extern u8 D_80099D9B[];
extern u8 D_80099D9C[];
extern s32 D_8006F980[];
void func_80080054(void);
void func_80080108(void);
void func_80080144(void);
void func_80088940(void);

void func_800346D4(s32 *);
void func_80083C0C(s32);
extern s32 D_80092954;
extern s32 D_800925F8;
extern s32 D_800925FC;
extern s32 D_80097058;
extern s32 D_800970E0;
extern s32 D_80098774;
extern s32 D_800987FC;
void func_80032F54(s32 *, s32, s32, s32, s32, s32, s32);
extern s32 D_800925D4;
extern s32 D_800925D8;
extern s32 D_8009868C;
extern s8 D_8009293C;
extern s32 D_800910C4;
extern s8 D_800928D4;
extern s32 D_80092900;
extern s32 D_80092A00;
extern s32 D_80092A10;
extern s32 D_80092A20;
extern s32 D_800970F8;
extern s32 D_80098814;
extern s8 D_80099D9D;
extern s8 D_80099D9E;
void SetGeomScreen(s32);
void func_80030988(s32, s32, s32, s32);
void func_8008976C(s32, s32);
void func_80089D5C(s32);
void func_8008A5BC(s32);
void func_8008BC04();
extern s32 D_800910F0;
extern s32 D_80092610;
void func_80074678(void *, s16, s16);
void func_80074998(void *);
void func_8008B0D8(void *);
void func_8008B730(void *, u8, u8);
void func_800732CC();
void func_80088AF8();
void func_8008DC28();
extern s32 D_8009292C;
extern s8 D_80099D9A;
extern s8 D_80099DA1;
extern s8 D_80099DA2;
extern s16 D_80099DA4;
extern s8 D_800928FC;
extern s32 D_80092918;
extern s32 D_80092944;
extern s32 D_80092950;
extern s16 D_80097102;
extern s16 D_8009881E;
extern s32 D_80092640;
extern s32 D_800928AC;
void func_8007BB7C();
void func_8007E24C();
void func_8008D580(s32);
void func_8008E620();
extern s8 D_80091150;
extern s8 D_80091151;
extern s32 D_80092644;
extern s32 D_80092890;
extern s32 D_80097014;
extern s32 D_80097080;
extern void *D_8009790C;
extern s32 D_80098730;
extern s32 D_8009879C;
extern void *D_80099028;
extern s32 D_800926DC;
extern s16 D_800926E8;
extern s16 D_800926EC;
extern s32 D_800912DC;
extern s8 D_8009273C;
extern s8 D_80092740;
extern s32 D_80092744;
extern u8 D_80099D9F;
s32 func_8007FF70(s32, s32, s32); /* retail: no andi on entry or return */
extern u8 D_80092884;
extern s32 D_80092924;
void func_800719F0();
void func_8008509C(s32, s32);
extern s32 D_80091660;
extern s32 D_80092734;
void func_80039FF8();
void func_80080964(s32);
s32 func_800891C0(s32);
extern s16 D_80091672;
extern s16 D_800916AE;
extern s8 D_80092758;
extern s32 D_800928C8;
extern s32 D_800928D8;
extern s32 D_80092940;
void ArchiveCdDataSync(s32);
extern s32 D_80092760;
void func_8007F8B4();
void func_80080B58();
void func_8008EB4C(s32);
extern s16 D_800915BE;
extern s16 D_80091636;
extern s16 D_80091762;
extern s8 D_800926FC;
extern s8 D_8009275C;
extern u16 D_8009286C;
extern s8 D_8009A1C4;
extern s8 D_8009A1C5;
extern s8 D_8009A1C6;
extern s16 D_8009A1CA;
extern s16 D_8009A1CE;
extern s8 D_8009A2BC;
extern s8 D_8009A2BD;
extern s8 D_8009A2BE;
extern s16 D_8009A2C2;
extern s16 D_8009A2C6;
extern s32 D_80092790;
void func_80080D20(s32);
void func_80086E24();
void func_8008AC0C(s32);
void func_8008AE1C(s32);
extern u8 D_800928A0;
void func_8008E3CC(s32, s32, s32);
void func_8007F258(s32, s32);
void func_8008E120();
extern s16 D_80092780;
extern s32 D_80092938;
void func_8003A838(s32, s32, s32);
void func_8008E064();
extern s32 D_80092784;
extern s32 D_80092948;
void *func_80089C54();
void func_80089C88(s32, void *);
void func_80089E2C(void *, s32);
s32 func_80089FC4();
void func_8008A184(s32, s32 *);
extern s32 D_80091FB0;
extern s32 D_80059488;
void func_80083BB4(s32);
extern u8 D_80092920;
s32 ArchiveDecodeAlignedSize(u16);
void ChangeGameState(s32);
void DrawSync(s32);
void Vsync(s32);
void func_8003852C(s32);
void func_800399D4(s32);
void func_80039C4C(s32);
void func_80088A40();
extern s8 D_8005061C;
extern s32 D_800917F0;
extern s32 D_800927C4;
void *func_8008820C();
void func_800888B0();
void func_800889C8();
extern s32 D_800927F4;
extern u16 g_C1ButtonState;
void func_800707A8(void);
void func_80070F80(s32 arg0);
void func_800718C0(void);
void func_800720C4(void);
void func_800720D4(void);
void func_800725A8(void);
void func_800726B4(void);
void func_80074AB4(void *arg0);
s32 func_8007570C(void *arg0, void *arg1, s32 arg2);
s32 func_80075738(void *arg0, void *arg1);
void func_80075748(void);
void func_8007639C(void *arg0, s8 arg1);
void func_80076424(void *arg0);
void func_80077A88(void *arg0);
void func_80078E94(void *arg0);
void func_80078ED4(void *arg0);
void func_80079A8C(void);
void func_80079B04(void);
void func_80079B0C(void);
void func_80079D08(void *arg0);
void func_80079DE0(void);
void func_8007A6D0(void *arg0);
void func_8007A730(void *arg0);
void func_8007A884(void);
s32 func_8007CD14(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
s32 func_8007E624(void);
void func_8007E894(s16 arg0, s16 arg1);
void func_8007E954(s32 arg0);
void func_8007F834(void);
void func_8007F8E4(void);
void func_80080234(void);
void func_800808F4(void);
void func_80080920(void);
s32 func_800809BC(void);
void func_800809D8(void);
void func_80080A58(void);
void func_80080AA0(s32 arg0);
void func_80080C48(s32 arg0);
void func_80080D10(void);
void func_80081E00(void);
void func_80081E6C(void);
s32 func_80083CD8(void);
s32 func_800849E0(void *arg0);
void func_80084A40(void *arg0);
s32 func_80084A64(void *arg0);
void func_80084AE0(void);
void func_80084B48(void);
void func_80084BEC(void *arg0);
void func_80084FD0(void);
void func_80085014(void);
void func_80085070(void);
void func_8008518C(void *arg0, s32 arg1);
void func_800851D4(void);
void func_80085E34(void *arg0, void *arg1);
void func_80085E60(void *arg0, void *arg1);
void func_80085E90(s32 arg0, s16 *arg1, s32 arg2);
void func_80085EAC(s32 arg0, s16 *arg1, s32 arg2);
void func_800882D4(s32 arg0, s32 arg1, s32 arg2, s16 arg3);
void func_80088BD4(void);
void func_80088C28(void);


void func_800707A8(void) {
    func_80083C0C(3);
    func_800346D4(&D_80092954);
}

s32 func_800707D8(s32 arg0, s32 arg1, s32 arg2) {
    s32 var_v0;

    var_v0 = arg0 - arg1;
    if (var_v0 < 0) {
        var_v0 += 1;
        var_v0 -= arg2;
    } else {
        var_v0 -= 1;
        var_v0 += arg2;
    }
    return var_v0 / arg2;
}

extern s32 D_800925F4;
extern s32 D_8009867C[];
extern s32 D_80098680;
extern s32 D_80098684;
s32 func_800707D8(s32 arg0, s32 arg1, s32 arg2);

void func_80070808(void *arg0, s32 arg1) {
    D_8009867C[0] += func_800707D8(*(s32 *)((s8*)(arg0) + 0), D_8009867C[0], arg1);
    D_80098684 += func_800707D8(*(s32 *)((s8*)(arg0) + 8), D_80098684, arg1);
    D_80098680 += func_800707D8(*(s32 *)((s8*)(arg0) + 4), D_80098680 + D_800925F4, arg1);
}

extern s32 D_8009871C[];
extern s32 D_80098720__s asm("D_80098720");
extern s32 D_80098724;
s32 func_800707D8(s32 arg0, s32 arg1, s32 arg2);
void func_800828F8(void *, void *, s32);

void func_800708C4(void *arg0, s32 arg1) {
    s32 sp10[3];

    sp10[0] = func_800707D8(*(s32 *)((s8*)(arg0) + 0), D_8009871C[0], arg1);
    sp10[1] = func_800707D8(*(s32 *)((s8*)(arg0) + 4), D_80098720__s, arg1);
    sp10[2] = func_800707D8(*(s32 *)((s8*)(arg0) + 8), D_80098724, arg1);
    func_800828F8(D_8009871C, sp10, 0x3D00);
    D_8009871C[0] += sp10[0];
    D_80098720__s += sp10[1];
    D_80098724 += sp10[2];
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007099C);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80070C7C);

void func_80070F80(s32 arg0) {
    D_800925F8 = arg0;
    D_80098774 = 0;
    D_80097058 = 0;
    D_800925FC = 0;
    D_800987FC &= 0xFFFF7FFF;
    D_800970E0 &= 0xFFFF7FFF;
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80070FD8);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007107C);
extern s32 D_800925E0;
extern s32 D_800925E4;
extern u8 *D_80092868;
extern u8 D_800929E4[];

void func_80071724(void *arg0) {
    *(s32 *)(D_80092868 + 0x7C) = D_800925E0 | (D_800925E4 << 0x10);
    AddPrim(arg0, D_80092868 + 0x74);
    AddPrim(arg0, D_800929E4 + D_800928A0 * 8);
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80071794);

void func_800718C0(void) {
    D_800925D8 = -1;
    D_800925D4 = 0;
    func_80032F54(&D_8009868C, 0x140, 0x30, 0x1C, 0x9A, 0x40, 4);
}

extern s32 D_8009105C[];
extern s32 D_800925E0;
extern s32 D_800925E4;
extern s32 D_800925E8;
extern s32 D_800925EC;
extern s16 D_80092600;
extern s8 D_80092608;
extern s16 D_80098692;
extern s16 D_80098698;
extern s16 D_800970C4;
extern s16 D_800970CC;
extern s16 D_800987E0;
extern s16 D_800987E8;

void func_8007191C(s32 arg0) {
    D_80092608 = arg0 == 0;
    if (arg0 == 0) {
        func_8008EB4C(0x37);
        D_80098698 = 2;
        D_80098692 = 0xB4;
    } else {
        D_80098698 = 4;
        D_80098692 = 0x9A;
    }
    func_80070F80(D_8009105C[arg0]);
    D_800925E8 = 0xA0;
    D_800925E0 = 0xA0;
    D_800925EC = 0x6D;
    D_800925E4 = 0x6D;
    D_80092600 = 0;
    D_800987E0 = D_800987E8;
    D_800970C4 = D_800970CC;
}


void func_800719F0(void) {
    D_80099D9D__a_s8[0] = 0;
    D_80099D9E__a_s8[0] = 0;
    func_80083C0C(7);
    D_800928C8__a_s32[0] = 5;
    D_80092884__a_s8[0] = 0;
    func_80032F54(D_80092954__a_s32, 0x140, 0x70, 0xA2, 0x2A, 0x1C, 8);
    func_80034714(D_80092954__a_s32, GetStringEntry(D_80092880[0], 0x42));
    D_800929BC[0] = 0x1E;
    D_800925DC[0] = 0;
    func_80070F80(D_80090F38);
    D_80092604[0] = 0;
    D_80092904[0] = 0;
    D_80092900__a_s32[0] = 0;
    func_8007191C(9);
}

void func_800346A4(s32 *);
void func_80034800(s32 *, s32, s32, s32);
void func_80034874(s32 *, s32);
void func_80034888(s32 *, s32, s32);
void func_80036420();
void func_8007107C();
void func_8007099C(s32);
void func_80079DF0(void *, void *);
s32 func_800707D8(s32 arg0, s32 arg1, s32 arg2);
void func_80071724(void *arg0);
extern s32 D_800925E0;
extern s32 D_800925E4;
extern s32 D_800925E8;
extern s32 D_800925EC;
extern u8 D_800925F0;
extern s32 D_800928E8;
extern u16 g_C1ButtonStatePressedOnce;
extern u16 g_C1ButtonStateReleased;
extern u8 D_80092604__u8 asm("D_80092604");
extern s32 D_8009868C__a[] asm("D_8009868C");
extern s32 D_80097010;
extern s32 D_8009872C;

void func_80071AD0(void) {
    s32 *p;

    if ((D_800925F0 != 0) && (D_800928E8 & 4)) {
        func_80071724((void *)D_80092938);
    }
    func_80036420();
    p = D_8009868C__a;
    if ((*(u8*)&D_80092608) != 0) {
        if (g_C1ButtonStatePressedOnce & 0x1000) {
            func_8008EB4C(0x1E);
            D_80092604__u8 -= 1;
        }
        if (g_C1ButtonStatePressedOnce & 0x4000) {
            func_8008EB4C(0x1E);
            D_80092604__u8 += 1;
        }
        if ((s8)D_80092604__u8 >= 8) {
            D_80092604__u8 = 0;
        }
        if ((s8)D_80092604__u8 < 0) {
            D_80092604__u8 = 7;
        }
        func_80034800(D_80092954__a_s32, (D_800928E8 * 7) & 0x3F, 0xC0, 0x10);
        func_80034874(D_80092954__a_s32, (s8)D_80092604__u8);
        if (g_C1ButtonStateReleased & 0x20) {
            func_8008EB4C(0x21);
            func_8007191C((s8)D_80092604__u8 + 1);
            func_800346A4(p);
        }
    }
    if (*(u8 *)D_800925F8 == 0) {
        func_8007191C(0);
    }
    func_8007107C();
    D_800925E0 += func_800707D8(D_800925E8, D_800925E0, 4);
    D_800925E4 += func_800707D8(D_800925EC, D_800925E4, 4);
    if ((*(u8*)&D_80092608) != 0) {
        func_80034888(D_80092954__a_s32, D_80092938, D_800928A0);
    }
    if (D_800925D4 != D_800925D8) {
        *((s8 *)p + 0x68) = 3;
        func_800346A4(p);
        func_80034714(p, GetStringEntry(D_80092880[0], D_800925D4));
        D_800925D8 = D_800925D4;
    }
    func_80079DF0(&D_8009872C, &D_80097010);
    func_8007099C(D_80092904[0]);
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80071DA4);
void func_80071DA4(s32 *);
extern s32 D_8009284C;
extern s32 D_80097010;
extern s16 D_800970C4;
extern s16 D_800970CC;
extern s32 D_80098720[];
extern s32 D_8009872C;
extern s16 D_800987E0;
extern s16 D_800987E8;

s32 func_80071F8C(s32 arg0) {
    switch (arg0) {
    case 0:
        func_80039C4C(D_80092948);
        func_80039FF8();
        return;
    case 1:
        func_80071DA4(&D_8009872C);
        if (((D_800987E0 << 8) / D_800987E8) > ((D_800970C4 << 8) / D_800970CC)) {
            D_800925D4 = 0x43;
            return;
        }
        D_800925D4 = 0x44;
        return;
    case 2:
        func_80071DA4(&D_80097010);
        return;
    case 3:
        D_80098720[0] = -D_8009284C;
        if (D_80098720[0] < -0x600) {
            D_80098720[0] = -0x600;
        }
        return;
    }
}


void func_800720C4(void) {
    D_8009293C = 0;
}


void func_800720D4(void) {
    if ((*(u8*)&D_8009293C) == 0) {
        func_80070F80(&D_800910C4);
        (*(u8*)&D_8009293C) = 1;
        D_800928D4 = 0;
        D_80099D9D = 0;
        D_80099D9E = 0;
        D_80092900 = 0;
        D_800925D4 = 0;
        D_800925D8 = 0;
        D_80092A00 = 1;
        D_80092A10 = 1;
        D_80092A20 = 1;
        D_800970F8 = 0;
        D_80098814 = 0;
    }
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80072170);

void func_800725A8(void) {

}

s32 func_8008A2B8(s32);
s32 func_8008A3E0(s32);
s32 func_8008C2C0(s32);
void func_80078F00(void *);
extern void *D_80092614;
extern s8 D_80092618;
extern s32 D_8009261C;
extern s32 D_80092620;
extern s32 D_80092624;
extern s32 D_80092628;
extern s32 D_8009262C;
extern u16 D_80092632;
extern s16 D_800928D0;
extern s32 D_80096FA8;
extern s32 D_80096FAC;
extern s32 D_80096FB0;

typedef struct {
    u8 pad0[0x14];
    s16 unk14;
    s16 unk16;
    s16 unk18;
    s16 unk1A;
    s16 unk1C;
    u16 unk1E;
} func_800725B0_s;

typedef struct {
    u8 pad0[0x5C];
    s32 unk5C;
    u8 pad60[0x89C];
    func_800725B0_s *unk8FC;
} func_800725B0_arg;

void func_800725B0(void *arg0) {
    func_800725B0_s *temp_v0;

    func_80030988(5, 4, 0x40, 0x40);
    D_800910F0 = func_8008A3E0(func_8008A2B8(0x10));
    D_80092610 = func_8008C2C0(((func_800725B0_arg *)arg0)->unk5C);
    func_8008976C(0x280, 0xDA);
    SetGeomScreen(0x400);
    D_800928D0 = 0;
    D_80092614 = arg0;
    func_80078F00(arg0);
    D_80096FB0 = 0;
    D_80096FAC = 0;
    D_80096FA8 = 0;
    temp_v0 = *(func_800725B0_s **)((s8*)(arg0) + 0x8FC);
    D_80092618 = 1;
    D_8009261C = temp_v0->unk14;
    D_80092620 = temp_v0->unk16;
    D_80092624 = temp_v0->unk18;
    D_80092628 = temp_v0->unk1A;
    D_8009262C = temp_v0->unk1C;
    D_80092632 = temp_v0->unk1E;
}


void func_800726B4(void) {
    func_8008BC04();
    func_8007F834();
    func_80030988(1, 1, 0x40, 0x40);
    func_8008A5BC(D_800910F0);
    func_80089D5C(D_80092610);
    func_8008976C(0x140, 0xDA);
    SetGeomScreen(0xC0);
    func_80083C0C(3);
    func_8007E954(0x100);
    func_80080D10();
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007273C);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80072858);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80072D18);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80073064);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_800730AC);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_800730F4);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007313C);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_800731F8);
/* The retail copy loop uses t0; preserve its register and byte-count contract. */
void func_800732AC(void *dst, void *src, s32 bytes) {
    register s32 word asm("$8");
    s32 end = bytes + (s32)src;
    do {
        word = *(s32 *)src;
        src = (u8 *)src + 4;
        *(s32 *)dst = word;
        dst = (u8 *)dst + 4;
    } while ((s32)src != end);
}

void func_800732CC(void) {
    void *temp_v0;

    temp_v0 = func_8008D3F4(3, 0);
    (*(u8 *)((u8 *)(temp_v0) + 0x74)) = 0x80;
    (*(u8 *)((u8 *)(temp_v0) + 0x75)) = 0x80;
    (*(u8 *)((u8 *)(temp_v0) + 0x76)) = 0xC0;
    func_8008D5C0(temp_v0, 0x60);
    (*(s16 *)((u8 *)(temp_v0) + 0)) = 4;
    (*(s16 *)((u8 *)(temp_v0) + 0x44)) = 0x300;
    (*(s16 *)((u8 *)(temp_v0) + 0x48)) = 8;
    (*(s16 *)((u8 *)(temp_v0) + 0x4A)) = 0x20;
    (*(s16 *)((u8 *)(temp_v0) + 0x68)) = 0;
    (*(s16 *)((u8 *)(temp_v0) + 0x6A)) = 0x20;
    D_80092644 = temp_v0;
}
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007334C);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80073424);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80073644);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80073B7C);
void func_80073CA4(s32 arg0) {
    s32 var_a1;
    s32 var_a2;
    void *temp_v1;

    var_a2 = 0;
    do {
        var_a1 = var_a2 * 0x54 + 0x104;
        temp_v1 = (void *)(arg0 + var_a1);
        if (*(u8 *)((s8*)(temp_v1) + 0x45) == 2) {
            *(u8 *)((s8*)(temp_v1) + 0x45) = 1U;
        } else {
            *(u8 *)((s8*)(temp_v1) + 0x45) = 0U;
        }
        var_a2 += 1;
    } while (var_a2 < 0x10);
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80073CEC);
s32 func_80073DE4(void *arg0, s32 arg1) {
    s32 temp_a1;
    register s32 var_v0 asm("$2");

    temp_a1 = *(s16 *)((s8*)(arg0) + 0xB6) + arg1;
    var_v0 = 1;
    if (temp_a1 >= 0x1001) {
        var_v0 = ((temp_a1 - 0xFF1) / 20) < *(s16 *)((s8*)(arg0) + 0xB4);
    }
    return var_v0;
}

s32 func_80073E2C(void *arg0, s32 arg1, s32 arg2) {
    s32 q;

    if (D_800928C8 == 4 || D_800928C8 == 6) {
        return 1;
    }
    *(s16 *)((s8*)(arg0) + 0xB6) += arg1;
    if (*(s16 *)((s8*)(arg0) + 0xB6) > 0x1000) {
        q = (*(s16 *)((s8*)(arg0) + 0xB6) - 0x1000) / 20;
        if (q >= *(s16 *)((s8*)(arg0) + 0xB4)) {
            *(s16 *)((s8*)(arg0) + 0xB6) -= arg1;
            *(s16 *)((s8*)(arg0) + 0xBA) = 0;
            if (func_80083CD8() != 4) {
                func_8008EB4C(0x2E);
            }
            return 0;
        }
        *(s16 *)((s8*)(arg0) + 0xB6) = 0x1000;
        *(s16 *)((s8*)(arg0) + 0xBA) = q;
        switch (arg2) {
        case 1:
            *(s32 *)((s8*)(arg0) + 0x1654) += *(s16 *)((s8*)(arg0) + 0xBA);
            break;
        case 2:
            *(s32 *)((s8*)(arg0) + 0x1658) += *(s16 *)((s8*)(arg0) + 0xBA);
            break;
        }
    }
    return 1;
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80073F34);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_800740E4);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80074678);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80074998);

void func_80074AB4(void *arg0) {
    u8 temp_a2;
    void *temp_s1;
    void *temp_s2;
    void *temp_v1;

    temp_v1 = *(void **)((s8*)(arg0) + 0x15CC);
    temp_s2 = *(void **)((s8*)(arg0) + 0x15D0);
    *(s32 *)((s8*)(arg0) + 0) = (s32) *(s16 *)((s8*)(temp_v1) + 0);
    *(s32 *)((s8*)(arg0) + 4) = (s32) *(s16 *)((s8*)(temp_v1) + 2);
    *(s32 *)((s8*)(arg0) + 8) = (s32) *(s16 *)((s8*)(temp_v1) + 4);
    *(u8 *)((s8*)(arg0) + 0x4C) = (u8) *(u8 *)((s8*)(temp_s2) + 8);
    *(s32 *)((s8*)(arg0) + 0x54) = (s32) ((s32) (*(u16 *)((s8*)(temp_v1) + 6) << 0x14) >> 0x14);
    temp_s1 = *(s32 *)((s8*)(*(void **)((s8*)(*(void **)((s8*)(arg0) + 0x5C)) + 4)) + 8) + (*(u8 *)((s8*)(temp_s2) + 8) * 0x14);
    if (*(u16 *)((s8*)(temp_s2) + 6) & 0x1000) {
        func_80074998(arg0);
        func_8008B0D8(temp_s1);
    }
    func_80074678(arg0, *(s16 *)((s8*)(temp_s1) + 0x12), *(u8 *)((s8*)(*(void **)((s8*)(arg0) + 0x15D0)) + 9));
    temp_a2 = *(u8 *)((s8*)(temp_s2) + 0xA);
    if (temp_a2 != 0) {
        func_8008B730(temp_s1, *(u8 *)((s8*)(temp_s2) + 9), temp_a2);
    }
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80074BA4);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80075060);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_800751C8);

s32 func_8007570C(void *arg0, void *arg1, s32 arg2) {
    s32 var_v0;

    var_v0 = *(s16 *)((s8*)(arg1) + 0x15EC) * arg2;
    if (var_v0 < 0) {
        var_v0 += 0x1F;
    }
    return (var_v0 >> 5) - *(s16 *)((s8*)(arg0) + 0x15EE);
}


s32 func_80075738(void *arg0, void *arg1) {
    return *(s16 *)((s8*)(arg1) + 0x15EA) - *(s16 *)((s8*)(arg0) + 0x15E8);
}


void func_80075748(void) {

}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80075750);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80075888);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80075A4C);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80075B50);

void func_8007639C(void *arg0, s8 arg1) {
    u8 temp_v0;

    if ((u8) *(u8 *)((s8*)(arg0) + 0x9C2) < 0x20U) {
        temp_v0 = *(u8 *)((s8*)(arg0) + 0x9C0);
        *(u8 *)((s8*)(arg0) + 0x9C0) = (u8) (temp_v0 + 1);
        *(s8 *)((s8*)((arg0 + (temp_v0 & 0x1F))) + 0x9A0) = arg1;
        *(u8 *)((s8*)(arg0) + 0x9C2) = (u8) (*(u8 *)((s8*)(arg0) + 0x9C2) + 1);
    }
}

u8 func_800763E4(u8 *arg0) {
    u8 idx;

    if (arg0[0x9C2] == 0) {
        return 0;
    }
    idx = arg0[0x9C1]++;
    idx = (arg0 + (idx & 0x1F))[0x9A0];
    arg0[0x9C2]--;
    return idx;
}

void func_80076424(void *arg0) {
    *(s8 *)((s8*)(arg0) + 0x9C2) = 0;
    *(s8 *)((s8*)(arg0) + 0x9C0) = 0;
    *(s8 *)((s8*)(arg0) + 0x9C1) = 0;
    *(s8 *)((s8*)(arg0) + 0x9C3) = 0;
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80076438);
extern s32 D_80091198[];
extern u8 D_80096FB8[];
extern u8 D_80096FBC[];

void func_800764CC(void *arg0) {
    s32 temp_lo;
    u32 temp_v1;
    u32 f;
    s32 var_a0;
    u8 idx;
    s16 *p;

    *(s32 *)((s8*)(arg0) + 0x40) = 0;
    *(s32 *)((s8*)(arg0) + 0x100) = 0;
    if (func_80083CD8() != 7) {
        f = *(u32 *)((s8*)(arg0) + 0xD0) & 0xFFFF7FFF;
        *(u32 *)((s8*)(arg0) + 0xD0) = f & ~2;
    }
    temp_lo = *(s16 *)((s8*)(arg0) + 0x15F4) * D_8009292C;
    temp_v1 = *(u32 *)((s8*)(arg0) + 0xD0) & 0xFFF7FFFF;
    *(u32 *)((s8*)(arg0) + 0xD0) = temp_v1;
    *(s16 *)((s8*)(arg0) + 0x15F2) = temp_lo >> 8;
    var_a0 = 0xF0;
    if (!(temp_v1 & 0x08000000)) {
        var_a0 = 0x28;
    }
    func_8007E894(var_a0, 0x28);
    idx = *(u8 *)((s8*)(arg0) + 0x9C3);
    if (idx != 0) {
        *(s32 *)(D_80096FB8 + ((*(u32 *)((s8*)(arg0) + 0xD0) >> 0x1B) & 1) * 0x14) = D_80091198[idx];
        p = *(s16 **)((s8*)(arg0) + 0x1600);
        D_80096FBC[((*(u32 *)((s8*)(arg0) + 0xD0) >> 0x1B) & 1) * 0x14] = (*((u8 *)p + *(u8 *)((s8*)(arg0) + 0x9C3) + 9) * *p) / 100;
    }
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007661C);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_800767C8);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80076884);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80077038);
s32 rcos(s32);
s32 rsin(s32);

s32 func_80077584(arg0, arg1, arg2, arg3)
    void *arg0;
    s32 arg1;
    s32 arg2;
    s32 arg3;
{
    *(s32 *)((s8*)(arg0) + 0x20) += rcos(arg1) >> arg2;
    *(s32 *)((s8*)(arg0) + 0x28) += rsin(arg1) >> arg2;
    if (*(s32 *)((s8*)(arg0) + 0x14) >= -0x8B) {
        *(s32 *)((s8*)(arg0) + 0x14) -= arg3;
    }
    if (*(s32 *)((s8*)(arg0) + 0x14) < -0x82) {
        *(s32 *)((s8*)(arg0) + 0x14) = -0x82;
    }
}

void func_8007762C(void *arg0) {
    *(u8 *)((u8 *)arg0 + 0x4C) = 8;
    *(u8 *)((u8 *)arg0 + 0x4E) = 0xFF;
    *(s16 *)((u8 *)arg0 + 0xCA) = 0x3C;
    *(s16 *)((u8 *)arg0 + 0x916) = 0x28;
    *(s32 *)((u8 *)arg0 + 0xE8) = 0x28;
    *(u8 *)((u8 *)arg0 + 0xC4) = 4;
    *(u8 *)((u8 *)arg0 + 0xC5) = 0;
    *(s32 *)((u8 *)arg0 + 0xD0) |= 0x1000;
    *(s32 *)((u8 *)arg0 + 0xD4) &= ~0x10;
    *(s32 *)((u8 *)arg0 + 0xD0) &= ~0x400;
    func_80077584();
}
void func_80036258(s32, s32);

s32 func_800776A8(void *arg0, s32 arg1) {
    s32 flags;

    if (func_80083CD8() != 4) {
        flags = *(s32 *)((s8*)(arg0) + 0xD0);
        if (flags & 0x08000000) {
            if ((D_800928C8 == 2 || D_800928C8 == 4) && (D_80099D9C[0] & 1) && !(flags & 0x40)) {
                func_80036258(1, arg1);
            }
        } else {
            if ((D_80099D9B[0] & 1) && !(flags & 0x40)) {
                func_80036258(0, arg1);
            }
        }
    }
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80077770);
extern u8 D_80091178[];
extern u8 D_80091179[];

s32 func_80077A38(void *arg0, s32 arg1) {
    s32 temp_v0;

    temp_v0 = *(u8 *)((s8*)(arg0) + 0x9C3) * 2;
    if (arg1 != 0) {
        *(u8 *)((s8*)(arg0) + 0x9C3) = D_80091178[temp_v0];
    } else {
        *(u8 *)((s8*)(arg0) + 0x9C3) = D_80091179[temp_v0];
    }
    return *(s32 *)((s8*)(arg0) + 0x80) + (*(u8 *)((s8*)(arg0) + 0x9C3) * 4);
}


void func_80077A88(void *arg0) {
    *(s32 *)((s8*)(arg0) + 0xD0) = (s32) (*(s32 *)((s8*)(arg0) + 0xD0) | 0x80000);
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80077A9C);
typedef struct {
    s32 unk[4];
} func_80078154_copy16;

void func_80078154(void *arg0) {
    *(func_80078154_copy16 *)arg0 = *(func_80078154_copy16 *)((s8*)(arg0) + 0x93C);
    *(s8 *)((s8*)(arg0) + 0x4C) = 0;
    *(s8 *)((s8*)(arg0) + 0xC4) = 0;
    *(s8 *)((s8*)(arg0) + 0xC5) = 0;
    *(s32 *)((s8*)(arg0) + 0xD0) = (s32) (*(s32 *)((s8*)(arg0) + 0xD0) | 0x02000000);
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80078194);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80078704);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80078920);
void func_80078D20(void *arg0) {
    s32 temp_a0;
    s32 temp_a1;
    s32 temp_v1;
    u8 unused[16];

    (*(s32 *)((u8 *)(arg0) + 0xB0)) = func_80082488(arg0, 1);
    temp_v1 = ((*(s32 *)((u8 *)(arg0) + 0xD0)) & 0x9FFFFFFF) | ((((u32)func_800828C4(arg0) >> 0x18) & 3) << 0x1D);
    (*(s32 *)((u8 *)(arg0) + 0xD0)) = temp_v1;
    if ((temp_v1 & 0x60000000) == 0x60000000) {
        (*(s8 *)((u8 *)(arg0) + 0x90B)) = 0xF;
    }
    func_800828F8(arg0, arg0 + 0x10, 0x3E80);
    temp_a1 = (*(s32 *)((u8 *)(arg0) + 0xD0)) & 0xFFFBFFFF;
    (*(s32 *)((u8 *)(arg0) + 0xD0)) = temp_a1;
    if ((*(s32 *)((u8 *)(arg0) + 0xB0)) < ((*(s32 *)((u8 *)(arg0) + 4)) + (*(s32 *)((u8 *)(arg0) + 0x14)))) {
        (*(s32 *)((u8 *)(arg0) + 0xD0)) = (s32) (temp_a1 | 0x40000);
        if ((*(u8 *)((u8 *)(arg0) + 0xC4)) == 4) {
            if (!((*(s32 *)((u8 *)(arg0) + 0xD4)) & 0x10)) {
                func_8008EBD0(arg0, 0xD, arg0, 2);
                func_800776A8(arg0, 6);
            }
            temp_a0 = (*(s32 *)((u8 *)(arg0) + 0x14));
            (*(s32 *)((u8 *)(arg0) + 0xD4)) = (s32) ((*(s32 *)((u8 *)(arg0) + 0xD4)) | 0x10);
            if (temp_a0 >= 0x40) {
                (*(s32 *)((u8 *)(arg0) + 0x14)) = (s32) (-temp_a0 / 3);
            } else {
                goto block_8;
            }
        } else {
block_8:
            (*(s32 *)((u8 *)(arg0) + 0x14)) = 0;
        }
        (*(s32 *)((u8 *)(arg0) + 4)) = (s32) (*(s32 *)((u8 *)(arg0) + 0xB0));
    }
    (*(s32 *)((u8 *)(arg0) + 0)) = (s32) ((*(s32 *)((u8 *)(arg0) + 0)) + (*(s32 *)((u8 *)(arg0) + 0x10)));
    (*(s32 *)((u8 *)(arg0) + 4)) = (s32) ((*(s32 *)((u8 *)(arg0) + 4)) + (*(s32 *)((u8 *)(arg0) + 0x14)));
    (*(s32 *)((u8 *)(arg0) + 8)) = (s32) ((*(s32 *)((u8 *)(arg0) + 8)) + (*(s32 *)((u8 *)(arg0) + 0x18)));
}

void func_80078E94(void *arg0) {
    *(s32 *)((s8*)(*(void **)((s8*)(arg0) + 0x5C)) + 0x34) = (s32) *(s32 *)((s8*)(arg0) + 0);
    *(s32 *)((s8*)(*(void **)((s8*)(arg0) + 0x5C)) + 0x38) = (s32) *(s32 *)((s8*)(arg0) + 4);
    *(s32 *)((s8*)(*(void **)((s8*)(arg0) + 0x5C)) + 0x3C) = (s32) *(s32 *)((s8*)(arg0) + 8);
    *(s16 *)((s8*)(*(void **)((s8*)(arg0) + 0x5C)) + 0x46) = (s16) *(s32 *)((s8*)(arg0) + 0x54);
}


void func_80078ED4(void *arg0) {
    *(s16 *)((s8*)(arg0) + 0) = 0x100;
    *(s16 *)((s8*)(arg0) + 4) = 0x10;
    *(s16 *)((s8*)(arg0) + 2) = 0;
    *(s16 *)((s8*)(arg0) + 6) = 0;
    *(s16 *)((s8*)(arg0) + 8) = 0;
    *(s16 *)((s8*)(arg0) + 0xA) = 0x30;
    *(s16 *)((s8*)(arg0) + 0xC) = 0x30;
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80078F00);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007920C);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_800796B8);

void func_80079A8C(void) {
    func_800732CC();
    func_8008DC28();
    func_80088AF8();
    D_80099D9D = 1;
    D_80099DA1 = 3;
    D_8009292C = 0x100;
    D_80099D9E = 0;
    D_80099D9A = 0;
    D_80099DA2 = 0;
    D_80099DA4 = 0x100;
}


void func_80079B04(void) {

}


void func_80079B0C(void) {
    D_80092950 = 0;
    D_8009881E = 0;
    D_80097102 = 0;
    D_80092918 = 0;
    D_80092944 = 0;
    D_800928FC = 0;
}

void func_800346A4(s32 *);                   /* extern */
void func_8008DCA8(s32);                     /* extern */
extern s8 D_80050622;
extern s8 D_80091144;
extern s8 D_80091145;
extern s32 D_80092638;
extern s32 D_8009263C;
extern s32 D_80092648;
extern s8 D_80092664;
extern s8 D_800928B4;
extern s8 D_800928F0;
extern s8 D_800928F4;
extern s32 D_8009290C;
extern s32 D_8009294C;
extern u8 D_80097010__a[] asm("D_80097010");
extern u8 D_8009872C__a[] asm("D_8009872C");

void func_80078F00(void *);
void func_800831C8(void);

s32 func_80079B44(void) {
    *(s16 *)(D_8009872C__a + 0xCC) = 0x400;
    *(s16 *)(D_80097010__a + 0xCC) = -0x400;
    func_80078F00(D_8009872C__a);
    func_80078F00(D_80097010__a);
    func_8008D580(D_80092644);
    func_8007E24C();
    func_8007BB7C();
    func_800831C8();
    D_800928F4 = 1;
    D_8009290C = -1;
    func_8008DCA8(0);
    D_80092638 = 0;
    D_80092640 = 0;
    D_80092890 = 0;
    D_8009263C = 0x5A;
    D_8009294C = 0;
    D_80092648 = 0;
    func_8007F834();
    D_80050622 = 0;
    func_8008E620();
    func_800720C4();
    D_800928D4 = 0;
    D_800928F0 = 0;
    D_80091144 = 0;
    D_80091145 = 0;
    D_80092664 = 0;
    D_80092950 += 1;
    func_800346A4(&D_8009868C);
    D_80099D9A = 0;
    if (*(u8 *)&D_8005061C != 0) {
        switch ((D_80092950 - 1) % 5) {
        case 3:
            D_800928B4 = 1;
            return;
        case 4:
            D_800928B4 = 2;
            return;
        default:
            D_800928B4 = 0;
            break;
        }
    }
}


void func_80079D08(void *arg0) {
    void *temp_a1;
    void *temp_a1_2;
    void *temp_a1_3;

    temp_a1 = *(void **)((s8*)(arg0) + 0x15CC);
    *(u16 *)((s8*)(temp_a1) + 0xA) = (u16) ((*(u16 *)((s8*)(temp_a1) + 0xA) & 0xFEFF) | ((*(u32 *)((s8*)(arg0) + 0xD0) << 6) & 0x100));
    temp_a1_2 = *(void **)((s8*)(arg0) + 0x15CC);
    *(u16 *)((s8*)(temp_a1_2) + 6) = (u16) ((*(u16 *)((s8*)(temp_a1_2) + 6) & 0xDFFF) | (((u32) *(u32 *)((s8*)(arg0) + 0xD0) >> 2) & 0x2000));
    temp_a1_3 = *(void **)((s8*)(arg0) + 0x15CC);
    *(u16 *)((s8*)(temp_a1_3) + 6) = (u16) ((*(u16 *)((s8*)(temp_a1_3) + 6) & 0xBFFF) | (((u32) *(u32 *)((s8*)(arg0) + 0xD0) >> 5) & 0x4000));
}

typedef struct {
    u32 pad0 : 2;
    u32 b2 : 1;
    u32 pad3 : 12;
    u32 b15 : 1;
    u32 pad16 : 3;
    u32 b19 : 1;
    u32 pad20 : 12;
} func_80079D6C_d0;
typedef struct {
    u16 pad0 : 8;
    u16 b8 : 1;
    u16 pad9 : 7;
} func_80079D6C_fa;
typedef struct {
    u16 pad0 : 13;
    u16 b13 : 1;
    s16 b14 : 1;
    u16 pad15 : 1;
} func_80079D6C_f6;

void func_80079D6C(void *arg0) {
    ((func_80079D6C_d0 *)((s8*)(arg0) + 0xD0))->b2 = ((func_80079D6C_fa *)((s8*)(*(void **)((s8*)(arg0) + 0x15CC)) + 0xA))->b8;
    ((func_80079D6C_d0 *)((s8*)(arg0) + 0xD0))->b15 = ((func_80079D6C_f6 *)((s8*)(*(void **)((s8*)(arg0) + 0x15CC)) + 6))->b13;
    ((func_80079D6C_d0 *)((s8*)(arg0) + 0xD0))->b19 = ((func_80079D6C_f6 *)((s8*)(*(void **)((s8*)(arg0) + 0x15CC)) + 6))->b14;
}


void func_80079DE0(void) {
    D_80092640 = 0;
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80079DF0);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007A21C);
extern u16 g_C1ButtonStateReleased;
extern s32 D_8006FD4C;
extern s32 D_80092648;
extern u8 D_8009287C;
extern u8 D_800928C0;
extern s32 D_800928E8;
extern u16 g_C2ButtonState;
extern u16 g_C2ButtonStateReleased;

typedef struct {
    s32 unk[4];
} func_8007A344_copy16;
typedef struct {
    u8 pad0[0x15CC];
    s32 p15CC;
    void *p15D0;
} func_8007A344_s;
void func_80036420();
void func_8007D65C(s32, s32, s32);

void func_8007A344(void *arg0, void *arg1) {
    s32 temp_v0;
    s32 temp_v0_2;
    s32 temp_v0_3;
    s32 temp_v1;
    u16 var_a0;
    u8 temp_a0;
    void *temp_v1_2;
    void *temp_v1_3;

    temp_a0 = D_800928C0 + 1;
    temp_v0 = (D_800928C0 * 0xC) + 0x9CC;
    temp_v1 = arg0 + temp_v0;
    temp_v0_2 = arg1 + temp_v0;
    D_800928C0 = temp_a0;
    ((func_8007A344_s *)arg0)->p15CC = temp_v1;
    ((func_8007A344_s *)arg1)->p15CC = temp_v0_2;
    temp_v0_3 = ((temp_a0 & 0xFF) * 0xC) + 0x9CC;
    ((func_8007A344_s *)arg0)->p15D0 = (void *) (arg0 + temp_v0_3);
    ((func_8007A344_s *)arg1)->p15D0 = (void *) (arg1 + temp_v0_3);
    if (D_800928AC == 0xFF) {
        temp_v1_2 = *(void **)((s8*)(arg0) + 0x15D0);
        *(u16 *)((s8*)(temp_v1_2) + 6) = (u16) (*(u16 *)((s8*)(temp_v1_2) + 6) | 0x1000);
        temp_v1_3 = *(void **)((s8*)(arg1) + 0x15D0);
        *(u16 *)((s8*)(temp_v1_3) + 6) = (u16) (*(u16 *)((s8*)(temp_v1_3) + 6) | 0x1000);
        *(s8 *)((s8*)(*(void **)((s8*)(arg0) + 0x15D0)) + 0xA) = 1;
        *(s8 *)((s8*)(*(void **)((s8*)(arg1) + 0x15D0)) + 0xA) = 1;
        *(s8 *)((s8*)(*(void **)((s8*)(arg0) + 0x15D0)) + 9) = 1;
        *(s8 *)((s8*)(*(void **)((s8*)(arg1) + 0x15D0)) + 9) = 1;
    }
    if (D_800928E8 & 8) {
        func_8007E894(0x10, 0x10);
        func_8007EBE0(&D_8006FD4C);
    }
    func_80073644(arg0);
    func_80073644(arg1);
    func_800764CC(arg0);
    func_800764CC(arg1);
    *(func_8007A344_copy16 *)((s8*)(arg0) + 0x95C) = *(func_8007A344_copy16 *)((s8*)(arg0) + 0x93C);
    *(func_8007A344_copy16 *)((s8*)(arg1) + 0x95C) = *(func_8007A344_copy16 *)((s8*)(arg1) + 0x93C);
    *(func_8007A344_copy16 *)((s8*)(arg0) + 0x6C) = *(func_8007A344_copy16 *)((s8*)(arg0) + 0);
    *(func_8007A344_copy16 *)((s8*)(arg1) + 0x6C) = *(func_8007A344_copy16 *)((s8*)(arg1) + 0);
    D_80092648 = 0;
    if ((u8) D_8009287C >= 2U) {
        D_8009287C = 1;
    }
    func_80036420();
    if (*(u8 *)&D_800928FC == 1) {
        if (g_C2ButtonStateReleased & 0x20) {
            func_80083C0C(8);
        }
        var_a0 = g_C2ButtonState;
    } else {
        if (g_C1ButtonStateReleased & 0x20) {
            func_80083C0C(8);
        }
        var_a0 = g_C1ButtonState;
    }
    func_800832C0(var_a0);
    func_80074AB4(arg0);
    func_80074AB4(arg1);
    func_80078E94(arg0);
    func_80078E94(arg1);
    *(s32 *)((s8*)(arg0) + 0xD0) = (s32) ((*(s32 *)((s8*)(arg0) + 0xD0) & 0x9FFFFFFF) | ((func_800828C4(arg0) & 0x03000000) << 5));
    *(s32 *)((s8*)(arg1) + 0xD0) = (s32) ((*(s32 *)((s8*)(arg1) + 0xD0) & 0x9FFFFFFF) | ((func_800828C4(arg1) & 0x03000000) << 5));
    func_80079D6C(arg0);
    func_80079D6C(arg1);
    func_8007920C(arg0);
    func_8007920C(arg1);
    func_80075B50(arg0);
    func_80075B50(arg1);
    func_8007BACC();
    if (D_80092884 != 0) {
        func_8007D65C((s32)arg0 + 0x94C, (s32)arg1 + 0x94C, 0x13);
    }
    if (D_800928AC == 0xFF) {
        func_8008369C();
    }
    if (D_800928AC == 0) {
        func_80083C0C(8);
    }
    D_800928AC--;
}


void func_8007A6D0(void *arg0) {
    D_8009292C = 0x100;
    if (D_800928AC < 0x78) {
        *(s8 *)((s8*)(arg0) + 0x4F) = 0x10;
        *(s8 *)((s8*)(arg0) + 0x52) = 0;
        *(s8 *)((s8*)(arg0) + 0x4C) = 0x10;
        *(s32 *)((s8*)(arg0) + 0xD0) = (s32) (*(s32 *)((s8*)(arg0) + 0xD0) | 0x400);
        return;
    }
    *(s8 *)((s8*)(arg0) + 0x4F) = 0x10;
    *(s8 *)((s8*)(arg0) + 0x52) = 0;
    *(s8 *)((s8*)(arg0) + 0x4C) = 0;
    *(s32 *)((s8*)(arg0) + 0xD0) = (s32) (*(s32 *)((s8*)(arg0) + 0xD0) | 0x400);
}


void func_8007A730(void *arg0) {
    D_8009292C = 0x100;
    *(s8 *)((s8*)(arg0) + 0x4F) = 0x10;
    *(s8 *)((s8*)(arg0) + 0x52) = 0;
    *(s8 *)((s8*)(arg0) + 0x4C) = 9;
    *(s32 *)((s8*)(arg0) + 0xD0) = (s32) (*(s32 *)((s8*)(arg0) + 0xD0) | 0x02000400);
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007A768);

void func_8007A884(void) {
    s8 var_v0_2;
    void *var_v0;

    func_8007E24C();
    func_8008D580(D_80092644);
    func_8007BB7C();
    func_8007F834();
    func_8008E620();
    if (D_80092890 != 0) {
        var_v0 = D_8009790C;
    } else {
        var_v0 = D_80099028;
    }
    if (*(s16 *)((s8*)(var_v0) + 0x2C) != 0) {
        D_80091150 = 1;
        var_v0_2 = 0x10;
    } else {
        D_80091150 = 2;
        var_v0_2 = -1;
    }
    D_80091151 = var_v0_2;
    D_80098730 = 0x100;
    D_80097014 = 0x100;
    D_8009879C = 0x100;
    D_80097080 = 0x100;
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007A958);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007AC3C);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007AE10);
void func_8007B210(void *arg0, s8 arg1) {
    *(s32 *)((u8 *)arg0 + 0x15CC) = (s32)((u8 *)arg0 + 0x9CC);
    *(u8 *)((u8 *)arg0 + 0x4F) = 0x10;
    *(s8 *)((u8 *)arg0 + 0x4C) = arg1;
    *(u8 *)((u8 *)arg0 + 0x52) = 0;
    D_8009292C = 0x100;
    func_80074BA4();
    func_80074678(arg0, *(s16 *)((u8 *)arg0 + 0x998), *(s16 *)((u8 *)arg0 + 0x99A));
}
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007B270);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007B388);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007BACC);

#ifndef XENO_PC_PORT
void func_8007BB7C(void) {
    u8 *var_v1;
    s32 var_v0;

    var_v1 = (u8 *) &D_80092AD8;
    var_v0 = 0x3B;
    do {
        *(s8 *)((s8*)(var_v1) + 0x50) = 0;
        var_v0 -= 1;
        var_v1 += 0x7C;
    } while (var_v0 >= 0);
}
#endif

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007BBA0);
extern u8 D_800926B8[];
void func_8007C100(void* arg0) {
    struct Bytes { u8 bytes[4]; };
    *(struct Bytes*)D_800926B8 = *(struct Bytes*)arg0;
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007C124);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007C280);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007C880);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007CAA4);

s32 func_8007CD14(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    return (arg3 & 0x7F) | ((arg0 << 7) & 0x80) | (arg1 << 0x18) | ((arg2 << 8) & 0xFFFF00);
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007CD44);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007CF78);
void func_8007E3CC(void*);
void func_8007D068(void* arg0) {
    struct Vector { s32 x,y,z,pad; };
    *(struct Vector*)0x1F800000 = *(struct Vector*)&D_80096FA8;
    func_8007E3CC(arg0);
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007D0B4);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007D190);
s32 func_8007D25C(s32 arg0) {
    if (arg0 < 0x10) {
        return 0;
    }
    return arg0 < 0x20;
}
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007D274);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007D334);
void func_8007D65C(s32 arg0, s32 arg1, s32 arg2) {
    switch (arg2) {
    case 0x11:
        arg2 = 0;
        break;
    case 0x12:
        arg2 = 1;
        break;
    case 0x13:
        arg2 = 2;
        break;
    default:
        return;
    }
    func_8007D334(arg0, arg1, arg2);
}
extern s32 D_800926BC;
extern s32 D_800926C0;
extern s32 D_800926C4;

void func_8007D6B8(void) {
    register s16 *var_a1 asm("$5");
    register u8 *var_v1 asm("$3");
    s32 allocation;
    register s32 tag asm("$8");
    register s32 color asm("$7");
    register s32 var_a0 asm("$4");
    register s32 var_a2 asm("$6");

    D_800926BC = HeapAlloc(0x9F6, 0);
    D_800926C0 = HeapAlloc(0xBF4, 0);
    allocation = (s32)HeapAlloc(0xBF4, 0);
    var_a2 = 0;
    tag = 2;
    color = 0x6880B0F0;
    var_a1 = D_800926BC;
    var_a0 = 0;
    D_800926C4 = allocation;
    var_v1 = (u8*)var_a1 + 4;
    do {
        *(s8 *)((s8*)((var_a0 + D_800926C0)) + 3) = tag;
        *(s32 *)((s8*)((var_a0 + D_800926C0)) + 4) = color;
        *(s8 *)((s8*)((var_a0 + D_800926C4)) + 3) = tag;
        var_a2 += 1;
        *(s32 *)((s8*)((var_a0 + D_800926C4)) + 4) = color;
        *(s16 *)((s8*)(var_v1) + 0) = 0;
        *(s16 *)((s8*)(var_v1) + -2) = 0;
        *var_a1 = 0;
        *(s8 *)((s8*)(var_v1) + 2) = 0;
        *(s16 *)((s8*)(var_v1) + 0) = 0;
        var_v1 += 10;
        *var_a1 = 0;
        var_a1 += 5;
        var_a0 += 0xC;
    } while (var_a2 < 0xFF);
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007D7A8);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007D918);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007DB28);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007DC74);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007E020);
extern s32 D_800926BC;
extern void *D_800926C8;
extern s8 D_8009791B;
extern s8 D_80099037;

typedef struct {
    s16 unk0;
    s16 unk2;
    s16 unk4;
    s16 unk6;
    s16 unk8;
    s16 unkA;
} func_8007E24C_s;

void func_8007E24C(void) {
    s32 var_a0;
    s32 var_a0_2;
    s32 var_v1_2;
    void *temp_v0;
    func_8007E24C_s *var_v1;

    var_v1 = D_800926C8;
    for (var_a0 = 0; var_a0 < 0x21C; var_a0++) {
        var_v1[var_a0].unk6 = 0;
        var_v1[var_a0].unk4 = 0;
        var_v1[var_a0].unk2 = 0;
        var_v1[var_a0].unk0 = 0;
    }
    var_a0_2 = 0;
    var_v1_2 = 0;
    D_8009791B = 0;
    D_80099037 = 0;
    do {
        *(s8 *)((s8*)((var_v1_2 + D_800926BC)) + 6) = 0;
        var_a0_2 += 1;
        temp_v0 = var_v1_2 + D_800926BC;
        *(s16 *)((s8*)(temp_v0) + 4) = 0;
        *(s16 *)((s8*)(temp_v0) + 2) = 0;
        *(s16 *)((s8*)(temp_v0) + 0) = 0;
        var_v1_2 += 0xA;
    } while (var_a0_2 < 0xFF);
}

extern s32 D_800926B0;
extern s32 D_800926B4;
extern s32 D_80094818;

typedef struct {
    u8 pad0[3];
    s8 unk3;
    u8 pad4[3];
    s8 unk7;
    u8 pad8[0x18];
} func_8007E2D8_s;

void func_8007E2D8(void) {
    s32 i;

    for (i = 0; i < 100; i++) {
        ((func_8007E2D8_s *)&D_80094818)[i].unk3 = 3;
        ((func_8007E2D8_s *)&D_80094818)[i].unk7 = 0x40;
    }
    D_800926B0 = 0;
    D_800926B4 = 0;
}

extern s32 D_800926B0;
extern s32 D_80094818;
void func_8007E31C(void *arg0, void *arg1, void *arg2) {
    void *temp_v1;

    if (D_800926B0 < 0x64) {
        temp_v1 = (D_800926B0 << 5) + (u8 *)&D_80094818;
        (*(s16 *)((u8 *)(temp_v1) + 0x10)) = (s16) (*(s32 *)((u8 *)(arg0) + 0));
        (*(s16 *)((u8 *)(temp_v1) + 0x12)) = (s16) (*(s32 *)((u8 *)(arg0) + 4));
        (*(s16 *)((u8 *)(temp_v1) + 0x14)) = (s16) (*(s32 *)((u8 *)(arg0) + 8));
        (*(s16 *)((u8 *)(temp_v1) + 0x18)) = (s16) (*(s32 *)((u8 *)(arg1) + 0));
        (*(s16 *)((u8 *)(temp_v1) + 0x1A)) = (s16) (*(s32 *)((u8 *)(arg1) + 4));
        (*(s16 *)((u8 *)(temp_v1) + 0x1C)) = (s16) (*(s32 *)((u8 *)(arg1) + 8));
        (*(u8 *)((u8 *)(temp_v1) + 4)) = (u8) (*(u8 *)((u8 *)(arg2) + 0));
        (*(u8 *)((u8 *)(temp_v1) + 5)) = (u8) (*(u8 *)((u8 *)(arg2) + 1));
        (*(u8 *)((u8 *)(temp_v1) + 6)) = (u8) (*(u8 *)((u8 *)(arg2) + 2));
        D_800926B0 += 1;
    }
}
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007E3CC);
extern u8 D_80092708;
void func_8007E528(s32 arg0) {
    if (D_80092708 == 0 && arg0 == 0xA) {
        func_8008EB4C(0x24);
    }
    D_80092708 = arg0;
}
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007E574);

s32 func_8007E624(void) {
    return D_800926DC;
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007E634);

void func_8007E894(s16 arg0, s16 arg1) {
    D_800926E8 = arg0;
    D_800926EC = arg1;
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007E8AC);

void func_8007E954(s32 arg0) {
    D_800912DC = arg0;
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007E964);
u8 *func_8007E8AC(u8);

s32 func_8007EB6C(u8 *arg0) {
    s32 width;
    s32 total;
    u8 c;

    c = *arg0;
    total = 0;
    while (c != 0) {
        arg0++;
        width = func_8007E8AC(c)[2] * D_800912DC;
        c = *arg0;
        total += (width >> 8) + 2;
    }
    return total;
}

void func_8007EBE0(u8 *arg0) {
    s32 savedX;
    u8 c;

    c = *arg0;
    savedX = D_800926E8;
    while (c != 0) {
        arg0++;
        func_8007E964(c);
        c = *arg0;
    }
    D_800926E8 = savedX;
    D_800926EC += 0x14;
}
s32 func_8007EB6C(u8 *);

void func_8007EC54(u8 *arg0) {
    s32 savedX;
    u8 c;

    savedX = D_800926E8;
    D_800926E8 -= func_8007EB6C(arg0) / 2;
    c = *arg0;
    while (c != 0) {
        arg0++;
        func_8007E964(c);
        c = *arg0;
    }
    D_800926E8 = savedX;
    D_800926EC += 0x14;
}

s32 func_8007EB6C(u8 *);

void func_8007ECF0(u8 *arg0) {
    s32 savedX;
    u8 c;

    savedX = D_800926E8;
    D_800926E8 -= func_8007EB6C(arg0);
    c = *arg0;
    while (c != 0) {
        arg0++;
        func_8007E964(c);
        c = *arg0;
    }
    D_800926E8 = savedX;
    D_800926EC += 0x14;
}

void func_8007ED84(u8 *arg0, s32 arg1) {
    s32 savedX;
    u8 unused[8]; /* retail frame has 8 bytes of locals no code touches */
    u8 c;

    savedX = D_800926E8;
    D_800926E8 = savedX - arg1;
    c = *arg0;
    while (c != 0) {
        arg0++;
        func_8007E964(c);
        c = *arg0;
    }
    D_800926E8 = savedX;
    D_800926EC += 0x14;
}
extern u8 D_800926F0;
extern u8 D_800926F4;
extern u8 D_800926F8;
void func_8007EE08(s32 arg0) {
    s32 unset = 0xFF;

    if (arg0 != 0) {
        D_800926F4 = 0xFF;
        D_800926F8 = 0;
        D_800926F0 = D_80059488 * 0x14;
    } else {
        D_800926F0 = unset;
        D_800926F4 = unset;
        D_800926F8 = unset;
    }
}
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007EE68);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007EEE8);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007EFB4);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007F05C);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007F258);

void func_8007F834(void) {
    D_80092740 = 0;
    D_8009273C = 0;
    D_80092744 = 0;
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007F854);
void func_8007F8B4(void) {
    u8 unused[8]; /* retail frame has 8 bytes of locals no code touches */

    func_80083C0C(1);
    D_80092734 = 0;
    func_8007F834();
}


void func_8007F8E4(void) {
    s32 var_a0;

    func_80080C48(0);
    if ((D_80099D9F != 0) || (D_80092950 == 1)) {
        var_a0 = 3;
    } else {
        D_80092950 -= 1;
        var_a0 = 6;
    }
    func_80083C0C(var_a0);
}

void func_8007F948(void *arg0, s32 arg1) {
    if (*(s16 *)((s8*)(arg0) + 0x12) == arg1) {
        func_8007EE08(1);
    } else {
        func_8007EE08(0);
    }
}


#ifndef XENO_PC_PORT
s32 func_8007F97C(void) {
    return *(s32 *)((u8 *) &D_800912F4 + (D_80099D98__s_u8 * 4));
}
#endif

extern char D_8006FF5C[];
extern char D_8006FF60[];
extern char D_8006FF68[];
extern char D_8006FF6C[];
extern char D_8006FF74[];
extern u8 D_80091300[];
extern s32 D_8009130C[];
void func_8007E894__int(s32, s32) asm("func_8007E894");

void func_8007F9A0(void *arg0) {
    char buf[8];
    char *str;

    func_8007E894__int((*(s16 *)((s8*)(arg0) + 0x24) + *(s16 *)((s8*)(arg0) + 0x28)) - 0xA, *(s16 *)((s8*)(arg0) + 0x14));
    func_8007EE08(0);
    func_8007F948(arg0, 0);
    func_8007ECF0((u8 *)func_8007F97C());
    func_8007F948(arg0, 1);
    Sprintf(buf, D_8006FF5C, D_80099DA1__a[0] + 1);
    D_8009292C = D_8009130C[D_80099DA1__a[0]];
    D_80099DA4 = D_8009292C;
    func_8007ECF0((u8 *)buf);
    func_8007F948(arg0, 2);
    Sprintf(buf, D_8006FF60, D_80091300[D_80099D9A__a[0]]);
    func_8007ECF0((u8 *)buf);
    func_8007F948(arg0, 3);
    str = D_8006FF6C;
    if (D_80099D9D__a[0] != 0) {
        str = D_8006FF68;
    }
    func_8007ECF0((u8 *)str);
    func_8007F948(arg0, 4);
    str = D_8006FF74;
    if (D_80099D9E__a[0] != 0) {
        str = D_8006FF68;
    }
    func_8007ECF0((u8 *)str);
    func_8007EE08(0);
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007FB0C);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007FBEC);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8007FE48);
extern s32 D_8009274C;
extern s32 D_80092750;
s32 func_8007FF70(s32 arg0, s32 arg1, s32 arg2) {
    s32 var_s0;
    s32 step;
    s32 sfx;
    s32 buttons;

    var_s0 = arg0;
    step = 1;
    if (arg2 & 4) {
        step = -1;
    }
    sfx = 0;
    if (arg2 & 2) {
        buttons = D_8009274C;
    } else {
        buttons = D_80092750;
    }
    if (buttons & 0x2000) {
        var_s0 += step;
    }
    if (buttons & 0x8000) {
        var_s0 -= step;
    }
    if (buttons & 0xA000) {
        sfx = 1;
    }
    if (arg2 & 1) {
        if (var_s0 == -1) {
            var_s0 = arg1;
        }
        if (arg1 < var_s0) {
            var_s0 = 0;
        }
    } else {
        if (var_s0 == -1) {
            sfx = 0;
            var_s0 = 0;
        }
        if (arg1 < var_s0) {
            sfx = 0;
            var_s0 = arg1;
        }
    }
    if (sfx != 0) {
        func_8008EB4C(0x20);
    }
    return var_s0;
}


void func_80080054(void) {
    D_80099D98[0] = func_8007FF70(D_80099D98[0], 2, 0);
}


void func_80080090(void) {
    D_80099DA1__a[0] = func_8007FF70(D_80099DA1__a[0], 7, 2);
}


void func_800800CC(void) {
    D_80099D9A__a[0] = func_8007FF70(D_80099D9A__a[0], 4, 2);
}


void func_80080108(void) {
    D_80099D9B[0] = func_8007FF70(D_80099D9B[0], 1, 1);
}


void func_80080144(void) {
    D_80099D9C[0] = func_8007FF70(D_80099D9C[0], 1, 1);
}


void func_80080180(void) {
    D_80099D9D__a[0] = func_8007FF70(D_80099D9D__a[0], 1, 1);
}


void func_800801BC(void) {
    D_80099D9E__a[0] = func_8007FF70(D_80099D9E__a[0], 1, 1);
}


void func_800801F8(void) {
    D_80099D9F__a[0] = func_8007FF70(D_80099D9F__a[0], 3, 2);
}


void func_80080234(void) {
    D_80092884 = func_8007FF70(D_80092884, 1, 1);
}


void func_80080268(void) {
    D_80099DA2__a[0] = func_8007FF70(D_80099DA2__a[0], 0xD, 3);
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_800802A4);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8008040C);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80080570);
s32 ArchiveDecodeSector(s32);
s32 ArchiveReadFileFromCdSector(s32, void *, s32, s32, u32);
s32 LoadImage(void *, void *);
extern s32 D_80092700;
extern s32 D_80092704;
extern s32 D_8009270C;
extern s32 D_80092710;
extern s32 D_80092714;
extern s32 D_80092720;

void func_80080644(s32 arg0, s32 arg1) {
    u8 *buf;
    u8 *buf2;
    u8 *rect;

    buf = HeapAlloc(0x2000, 0);
    ArchiveReadFileFromCdSector(ArchiveDecodeSector(6) + (arg0 * 2), buf, 0x1000, 0, 0);
    buf2 = buf + 0x1000;
    ArchiveReadFileFromCdSector(ArchiveDecodeSector(6) + (arg1 * 2), buf2, 0x1000, 0, 0);
    D_80092700 = arg0;
    D_80092704 = arg1;
    D_80092714 = arg0;
    D_80092720 = arg1;
    D_80092710 = 3;
    ArchiveCdDataSync(0);
    rect = (u8 *)D_8009270C + (arg0 * 0x14);
    LoadImage(rect, buf);
    LoadImage(rect + 8, buf + 0x100);
    rect = (u8 *)D_8009270C + (arg1 * 0x14);
    LoadImage(rect, buf2);
    LoadImage(rect + 8, buf + 0x1100);
    HeapDelayedFree(buf, 2);
}

extern s8 D_80091369;
extern s8 D_80091391;
extern s16 D_800916E2;
extern s16 D_8009171E;
extern s32 D_80092718;
extern s32 D_8009271C;
extern s32 D_80092724;
extern s32 D_80092728;

void func_80080780(s32 arg0) {
    u8 *rect;
    s32 i;
    s32 off;

    if (D_80092940 == 0) {
        ArchiveCdDataSync(0);
        rect = (u8 *)D_8009270C;
        for (i = 0; i < 0x31; i++) {
            off = i << 12;
            LoadImage(rect, (void *)(D_800928D8 + off));
            LoadImage(rect + 8, (void *)(D_800928D8 + off + 0x100));
            rect += 0x14;
        }
        HeapFree((void *)D_800928D8);
        D_80092940 = 1;
    }
    D_800928C8 = arg0;
    D_800916E2 = (arg0 == 4) ? 3 : 4;
    D_8009171E = 5;
    if (arg0 == 3) {
        D_80091369 = 0x27;
        D_80091391 = 0x28;
    } else {
        D_80091369 = 0x25;
        D_80091391 = 0x26;
    }
    func_80083C0C(1);
    D_80092710 = 0;
    D_80092728 = 0;
    D_80092724 = 0;
    D_8009271C = 0;
    D_80092718 = 0;
    D_80092714 = D_80092700;
    D_80092720 = D_80092704;
    func_80080964(5);
}


void func_800808F4(void) {
    D_80092924 = 1;
    func_8007F834();
}


void func_80080920(void) {
    D_80092924 = 1;
    func_800719F0();
    func_8008509C(0, 0);
    func_8008509C(1, 1);
}

extern s32 D_80092738;
extern u8 D_800915AC[];
void func_80080964(s32 arg0) {
    s32 prev;

    if (arg0 == 0xFF) {
        D_80092734 = D_80092738;
        return;
    }
    prev = D_80092734;
    D_80092734 = (s32)&D_800915AC[arg0 * 0x3C];
    D_80092738 = prev;
}

s32 func_800809BC(void) {
    return D_80092734 == (s32) &D_80091660;
}


void func_800809D8(void) {
    func_80039FF8();
    D_80092734 = 0;
    func_80080964(3);
    D_80091672 = 0;
    D_800916AE = 0;
    D_800928C8 = 0;
    D_80092758 = 0;
    func_8007F834();
    D_80092924 = 0;
    func_80080AA0(0);
    D_80092940 = 0;
    D_800928D8 = func_800891C0(6);
}


void func_80080A58(void) {
    if (D_80092940 == 0) {
        ArchiveCdDataSync(0);
        HeapFree(D_800928D8);
        D_80092940 = 1;
    }
}


void func_80080AA0(s32 arg0) {
    if (arg0 != 0) {
        D_80092760 = 0;
    }
    if (D_80092760 != 0) {
        HeapFree(D_80092760);
        D_80092760 = 0;
    }
}

void func_8007313C(s32, s32);                    /* extern */

void func_80080AE8(void) {
    struct { s16 x,y,w,h; } rect;
    register s32 width asm("$3");

    if (D_80092760 != 0) {
        DrawSync(0);
        width = 0x140;
        rect.x = width;
        rect.y = 0x100;
        rect.w = width;
        rect.h = 0xDA;
        func_8007313C(D_80092760, D_80092760 + 0x21E80);
        LoadImage(&rect.x, D_80092760);
    }
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80080B58);

void func_80080C48(s32 arg0) {
    func_80039FF8();
    func_8008EB4C(0x1F);
    if (arg0 == 1) {
        D_80092734 = 0;
        func_80080964(0);
        D_800915BE = 0;
        D_80091636 = 1;
        goto block_4;
    }
    if (arg0 == 2) {
        D_80092734 = 0;
        func_80080964(7);
        D_80091762 = 1;
block_4:
        func_80083C0C(0);
        D_80092758 = 1;
        D_800926FC = 0;
        D_8009275C = 1;
        func_80080B58();
        return;
    }
    func_8007F8B4();
}


void func_80080D10(void) {
    D_800926DC = 0;
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80080D20);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80080F04);
s32 SystemRenderStringEntry(s32, s32, s32, s32);

void func_80081094(void *arg0, s32 arg1, s32 arg2) {
    s32 temp_v0;

    temp_v0 = SystemRenderStringEntry(GetStringEntry(D_80092880[0], arg1), *(s32 *)((s8*)(arg0) + 0), 0x3F, arg2);
    *(s16 *)((s8*)(arg0) + 0x2C) = temp_v0;
    *(s16 *)((s8*)(arg0) + 0x2E) = (0x140 - temp_v0) / 2;
}

extern s32 D_80095510;
extern s32 D_80095540;

void func_80081100(s32 arg0, s32 arg1) {
    struct { s16 x,y,w,h; } rect;

    if (arg1 == 0) {
        if (arg0 != *(u8*)&D_8009273C) {
            D_8009273C = arg0;
            func_80081094(&D_80095510, arg0, 0);
            goto block_5;
        }
    } else if (arg0 != *(u8*)&D_80092740) {
        D_80092740 = arg0;
        func_80081094(&D_80095540, arg0, 1);
block_5:
        rect.x = 0x140;
        rect.y = 0x30;
        rect.w = 0x42;
        rect.h = 0xD;
        LoadImage(&rect, D_80095510);
    }
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_800811AC);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_800812BC);
extern s32 D_80092700;
extern s32 D_80092704;
void func_800814AC(void) {
    u8 *entry;
    u32 i;

    i = 0;
    entry = D_800915AC;
    do {
        func_800812BC(entry);
        i++;
        entry += 0x3C;
    } while (i < 8);
    D_80092734 = 0;
    D_80092700 = 0;
    D_80092704 = 1;
    func_80080F04();
}
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8008151C);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8008162C);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80081A44);
void func_80080AE8(void);
void func_80081A44(void);
void func_8008151C(s32);
void func_8008162C(s32, s32);
void func_80036420();
extern s32 D_800916D8;
extern u16 g_C1ButtonStateReleased;

void func_80081D2C(void) {
    s32 temp_s0;
    u8 unused[8]; /* retail frame has 8 bytes of locals no code touches */

    if (*(u8 *)&D_8009275C != 0) {
        func_80080AE8();
        D_8009275C = 0;
    }
    func_80036420();
    temp_s0 = D_80092734;
    if (temp_s0 == (s32) &D_800916D8) {
        func_80081A44();
        return;
    }
    if (temp_s0 != 0) {
        if ((g_C1ButtonStateReleased & 1) && ((u8) D_800926FC < 5U)) {
            D_800926FC += 1;
            func_80080AE8();
        }
        func_8008162C(temp_s0, *(u8 *)&D_800928FC);
    }
    func_8008151C(D_80092734);
}


void func_80081E00(void) {
    s16 temp_v1;

    D_8009A1CA = 0x60;
    D_8009A2C2 = 0x60;
    D_8009A1C4 = 0x7F;
    D_8009A1C5 = 0x7F;
    D_8009A1C6 = 0x7F;
    D_8009A2BC = 0x7F;
    D_8009A2BD = 0x7F;
    D_8009A2BE = 0x7F;
    temp_v1 = D_8009286C - 0x60;
    D_8009A1CE = temp_v1;
    D_8009A2C6 = temp_v1;
}


void func_80081E6C(void) {
    D_8009A1CA = 0;
    D_8009A2C2 = 0;
    D_8009A1C4 = 0;
    D_8009A1C5 = 0;
    D_8009A1C6 = 0;
    D_8009A2BC = 0;
    D_8009A2BD = 0;
    D_8009A2BE = 0;
    (*(u16*)&D_8009A1CE) = D_8009286C;
    (*(u16*)&D_8009A2C6) = D_8009286C;
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80081ECC);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80082178);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80082300);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80082458);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80082488);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80082880);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_800828C4);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_800828F8);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80082A70);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80082C4C);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80082E60);
s32 rand(void);
s32 rcos(s32);
s32 rsin(s32);
extern s32 D_80092770;
extern s32 D_80092774;
extern s32 D_8009290C;
extern s32 D_8009871C[];
extern s32 D_80098720[];
extern s32 D_80098724;

void func_800831C8(void) {
    s32 r;
    s32 ang;

    r = (rand() & 0x1FFF) + 0x800;
    ang = (rand() % 0x600) + 0x500;
    D_8009871C[0] = ((rcos(ang) * r) >> 12) + 0x4000;
    D_80098724 = ((rsin(ang) * r) >> 12) + 0x4000;
    D_80098720[0] = -((rand() & 0x7FF) + 0x400);
    D_80092770 = 0x100;
    D_80092774 = 0x40;
    (*(s8*)&D_8009287C) = 0x40;
    D_8009290C = 0x400;
}

extern s32 D_800927AC;

void func_800832C0(s32 arg0) {
    if (arg0 & 0x8000) {
        D_800927AC += 0x20;
    }
    if (arg0 & 0x2000) {
        D_800927AC -= 0x20;
    }
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80083310);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8008369C);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80083738);
void func_80083B54(void* arg0, void* arg1) {
    struct Vector { s32 x, y, z, pad; };
    *(struct Vector*)arg0 = *(struct Vector*)D_8009871C;
    *(struct Vector*)arg1 = *(struct Vector*)D_8009867C;
}

void ClearImage(s16 *, s32, s32, s32); /* extern */

void func_80083BB4(s32 arg0) {
    struct { s16 x,y,w,h; } rect;

    rect.x = 0;
    rect.y = 0;
    if (arg0 != 0) {
        rect.w = 0x280;
    } else {
        rect.w = 0x140;
    }
    rect.h = 0x1E0;
    ClearImage(&rect.x, 0, 0, 0);
    DrawSync(0);
}

extern s32 D_80092794;
extern s32 D_8009294C;
void func_8007A21C(s32);
void func_8007AC3C(void);
void func_800725B0(void *);
extern u8 D_80097010__a[] asm("D_80097010");
extern u8 D_8009872C__a[] asm("D_8009872C");

void func_80083C0C(s32 arg0) {
    D_80092794 = arg0;
    switch (arg0) {
    case 3:
        func_80081E6C();
        return;
    case 4:
        func_8007A21C(D_8009294C);
        return;
    case 8:
        func_8007AC3C();
        return;
    case 6:
        if (*(s16 *)(D_8009872C__a + 0xF2) < *(s16 *)(D_80097010__a + 0xF2)) {
            func_800725B0(D_80097010__a);
        } else {
            func_800725B0(D_8009872C__a);
        }
        return;
    }
}


s32 func_80083CD8(void) {
    return D_80092790;
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80083CE8);
void func_80030A30(s32, void *);                 /* extern */
extern void *D_8009288C;
extern s32 D_8009871C[];
extern s32 D_80098724;

void func_80083DCC(void *arg0, void *arg1, s32 arg2) {
    u8 unused[0x20]; /* retail frame has 0x20 bytes of locals no code touches */
    s32 temp_a0;
    u8 temp_v1;
    void *temp_a0_10;
    void *temp_a0_2;
    void *temp_a0_3;
    void *temp_a0_4;
    void *temp_a0_5;
    void *temp_a0_6;
    void *temp_a0_7;
    void *temp_a0_8;
    void *temp_a0_9;
    void *temp_a1;
    void *temp_v0_2;

    temp_v1 = *(u8 *)((s8*)(arg1) + 0x918);
    temp_a1 = *(void **)((s8*)(((arg2 * 4) + arg0)) + 8);
    temp_a0 = temp_v1 & 0xFF;
    if (temp_a0 != 0) {
        *(u8 *)((s8*)(arg1) + 0x918) = temp_v1 - 0x18;
        if (temp_a0 < *(u8 *)((s8*)(arg1) + 0x918)) {
            *(u8 *)((s8*)(arg1) + 0x918) = 0U;
        }
    }
    if (*(s32 *)((s8*)(arg1) + 0xD4) & 0x20) {
        *(u16 *)((s8*)(*(void **)((s8*)(temp_a1) + 4)) + 0xC) = (u16) ((s32) (*(u8 *)((s8*)(*(void **)((s8*)(arg1) + 0xD8)) + 0x15D4) * temp_a0) / 16);
        *(u16 *)((s8*)(*(void **)((s8*)(temp_a1) + 4)) + 0xE) = (u16) ((s32) (*(u8 *)((s8*)(*(void **)((s8*)(arg1) + 0xD8)) + 0x15D5) * temp_a0) / 16);
        *(u16 *)((s8*)(*(void **)((s8*)(temp_a1) + 4)) + 0x10) = (u16) ((s32) (*(u8 *)((s8*)(*(void **)((s8*)(arg1) + 0xD8)) + 0x15D6) * temp_a0) / 16);
    } else {
        *(u16 *)((s8*)(*(void **)((s8*)(temp_a1) + 4)) + 0xC) = (u16) (temp_a0 * 0x10);
        *(u16 *)((s8*)(*(void **)((s8*)(temp_a1) + 4)) + 0xE) = (u16) (temp_a0 * 8);
        *(u16 *)((s8*)(*(void **)((s8*)(temp_a1) + 4)) + 0x10) = 0U;
    }
    if (*(u8 *)((s8*)(D_8009288C) + 0x10) != 0) {
        temp_a0_2 = *(void **)((s8*)(temp_a1) + 4);
        *(u16 *)((s8*)(temp_a0_2) + 0xC) = (u16) ((s16) *(u16 *)((s8*)(temp_a0_2) + 0xC) / 2);
        temp_a0_3 = *(void **)((s8*)(temp_a1) + 4);
        *(u16 *)((s8*)(temp_a0_3) + 0xE) = (u16) ((s16) *(u16 *)((s8*)(temp_a0_3) + 0xE) / 2);
        temp_a0_4 = *(void **)((s8*)(temp_a1) + 4);
        *(u16 *)((s8*)(temp_a0_4) + 0x10) = (u16) ((s16) *(u16 *)((s8*)(temp_a0_4) + 0x10) / 2);
    }
    *(s32 *)((s8*)(*(void **)((s8*)(temp_a1) + 4)) + 0) = (s32) *(s32 *)((s8*)(arg1) + 0);
    *(s32 *)((s8*)(*(void **)((s8*)(temp_a1) + 4)) + 4) = (s32) *(s32 *)((s8*)(arg1) + 4);
    *(s32 *)((s8*)(*(void **)((s8*)(temp_a1) + 4)) + 8) = (s32) *(s32 *)((s8*)(arg1) + 8);
    temp_a0_5 = *(void **)((s8*)(temp_a1) + 4);
    *(s32 *)((s8*)(temp_a0_5) + 0) = (s32) (*(s32 *)((s8*)(temp_a0_5) + 0) - *(s32 *)((s8*)(*(void **)((s8*)(arg1) + 0xD8)) + 0));
    temp_a0_6 = *(void **)((s8*)(temp_a1) + 4);
    *(s32 *)((s8*)(temp_a0_6) + 4) = (s32) (*(s32 *)((s8*)(temp_a0_6) + 4) - *(s32 *)((s8*)(*(void **)((s8*)(arg1) + 0xD8)) + 4));
    temp_a0_7 = *(void **)((s8*)(temp_a1) + 4);
    *(s32 *)((s8*)(temp_a0_7) + 8) = (s32) (*(s32 *)((s8*)(temp_a0_7) + 8) - *(s32 *)((s8*)(*(void **)((s8*)(arg1) + 0xD8)) + 8));
    func_80030A30(arg2, *(void **)((s8*)(temp_a1) + 4));
    temp_a1 = *(void **)((s8*)(arg0) + 0x10);
    temp_v0_2 = *(void **)((s8*)(temp_a1) + 4);
    *(s16 *)((s8*)(temp_v0_2) + 0x10) = 0;
    *(s16 *)((s8*)(temp_v0_2) + 0xE) = 0;
    *(s16 *)((s8*)(temp_v0_2) + 0xC) = 0;
    *(s32 *)((s8*)(*(void **)((s8*)(temp_a1) + 4)) + 0) = (s32) *(s32 *)((s8*)(arg1) + 0);
    *(s32 *)((s8*)(*(void **)((s8*)(temp_a1) + 4)) + 4) = (s32) *(s32 *)((s8*)(arg1) + 4);
    *(s32 *)((s8*)(*(void **)((s8*)(temp_a1) + 4)) + 8) = (s32) *(s32 *)((s8*)(arg1) + 8);
    temp_a0_8 = *(void **)((s8*)(temp_a1) + 4);
    *(s32 *)((s8*)(temp_a0_8) + 0) = (s32) (*(s32 *)((s8*)(temp_a0_8) + 0) - D_8009871C[0]);
    temp_a0_9 = *(void **)((s8*)(temp_a1) + 4);
    *(s32 *)((s8*)(temp_a0_9) + 4) = (s32) (*(s32 *)((s8*)(temp_a0_9) + 4) - D_80098720[0]);
    temp_a0_10 = *(void **)((s8*)(temp_a1) + 4);
    *(s32 *)((s8*)(temp_a0_10) + 8) = (s32) (*(s32 *)((s8*)(temp_a0_10) + 8) - D_80098724);
    func_80030A30(2, *(void **)((s8*)(temp_a1) + 4));
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_800840CC);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_800846A0);

s32 func_800849E0(void *arg0) {
    s32 temp_s0;

    temp_s0 = *(s32 *)((s8*)(arg0) + 0x284);
    func_8008AC0C(temp_s0);
    func_80080D20(*(s32 *)((s8*)(((D_800928A0 * 4) + temp_s0)) + 4) + 4);
    func_8008AE1C(temp_s0);
    func_80086E24();
    return 0;
}


void func_80084A40(void *arg0) {
    func_8008AC0C(*(s32 *)((s8*)(arg0) + 0x284));
}


s32 func_80084A64(void *arg0) {
    s32 temp_s0;

    temp_s0 = *(s32 *)((s8*)(arg0) + 0x284);
    func_8008E3CC(*(s32 *)((s8*)(((D_800928A0 * 4) + temp_s0)) + 4) + 8, 0xC0, 0);
    func_80080D20(*(s32 *)((s8*)(((D_800928A0 * 4) + temp_s0)) + 4) + 4);
    func_8008AE1C(temp_s0);
    func_80086E24();
    return 0;
}


void func_80084AE0(void) {
    if (D_80092780 != 0) {
        func_8008E120();
        func_8007F258(D_80092938, 0);
        func_80080D20(D_80092938);
        func_8008E3CC(D_80092938, 0xC0, 1);
        func_8008BC04();
    }
}


void func_80084B48(void) {
    s16 temp_v0;

    if (D_80092780 != 0) {
        temp_v0 = D_80092780 - 4;
        D_80092780 = temp_v0;
        if (temp_v0 <= 0) {
            D_80092780 = 0;
            D_80092784 = 0;
            func_8003A838(D_80092948, 0x100, 0);
            D_8009292C = 0x100;
            func_8008E064();
            return;
        }
        func_8008E3CC(D_80092938, temp_v0, 1);
        return;
    }
    D_80092784 = 0;
}


void func_80084BEC(void *arg0) {
    s32 temp_s2;
    s32 temp_v0;
    void *temp_s1;

    temp_s2 = *(s32 *)((s8*)(*(void **)((s8*)(*(void **)((s8*)(*(void **)((s8*)(arg0) + 0x5C)) + 4)) + 4)) + 0x30);
    temp_s1 = func_80089C54();
    temp_v0 = func_80089FC4();
    func_80089E2C(temp_s1, temp_v0);
    func_8008A184(temp_v0, &D_80091FB0);
    func_80089C88(temp_s2, temp_s1);
    *(s16 *)((s8*)(temp_s1) + 0x46) = 0xC00;
    *(s16 *)((s8*)(temp_s1) + 0x44) = 0;
    *(s16 *)((s8*)(temp_s1) + 0x48) = 0x400;
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80084C88);

void func_80084FD0(void) {
    if ((D_80092784 != 0) && (D_80059488 & 1)) {
        func_8008E120();
    }
}


void func_80085014(void) {
    D_80092920 &= 0xFE;
    func_80083BB4(0);
    func_80083C0C(5);
    D_80099D9E = 1;
    D_80099D9D = 1;
    D_800928C8 = 6;
}


void func_80085070(void) {
    D_80099D9E = 0;
    D_80099D9D = 0;
    D_80092920 |= 1;
}

void ArchiveSetIndex(s32, s32);
extern s8 D_80097919;
extern s8 D_80099035;
extern s32 D_800927B4[];

void func_8008509C(s32 arg0, s32 arg1) {
    s32 *temp_s0;
    s32 *base;

    if (arg0 != 0) {
        D_80097919 = arg1;
    } else {
        D_80099035 = arg1;
    }
    base = D_800927B4;
    temp_s0 = &base[arg0];
    if (*temp_s0 != 0) {
        HeapFree((void *)*temp_s0);
        *temp_s0 = 0;
    }
    ArchiveSetIndex(0x30, 1);
    *temp_s0 = func_800891C0(arg1 + 2);
    ArchiveSetIndex(0x30, 0);
}

extern s32 D_800927B4[];
void func_80085134(s32 arg0) {
    ArchiveCdDataSync(0);
    if (D_800927B4[arg0] != 0) {
        HeapFree(D_800927B4[arg0]);
        D_800927B4[arg0] = 0;
    }
}

void func_8008518C(void *arg0, s32 arg1) {
    *(s32 *)((s8*)(arg0) + 4) = HeapAlloc(ArchiveDecodeAlignedSize(*(u16 *)((s8*)(arg0) + 0)), arg1);
}


void func_800851D4(void) {
    func_8003852C(D_800927C4);
    if (D_800917F0 != 0) {
        func_80039C4C(D_80092948);
        func_800399D4(D_80092948);
    }
    func_80088A40();
    ChangeGameState(1);
    DrawSync(0);
    Vsync(2);
    D_8005061C = 1;
    MainLoop(0);
}

s32 func_80085264(void) {
    s32 result = 0;
    if (D_80092794 == 5 || D_80092794 == 7) {
        result = 1;
    } else if (D_80092794 == 1) {
        if (D_800928C8 == 4 && *(u8*)&D_8005061C == 0) {
            result = 1;
        }
    }
    return result;
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_800852C4);

void func_80085E34(void *arg0, void *arg1) {
    *(u16 *)((s8*)(arg1) + 0) = (u16) (*(u16 *)((s8*)(arg0) + 0) + 0x18);
    *(s16 *)((s8*)(arg1) + 2) = (s16) (*(u16 *)((s8*)(arg0) + 2) + 6);
    *(u16 *)((s8*)(arg1) + 0) = (u16) (*(u16 *)((s8*)(arg1) + 0) + 0x4F);
}


void func_80085E60(void *arg0, void *arg1) {
    *(u16 *)((s8*)(arg1) + 0) = (u16) (0x8B - *(u16 *)((s8*)(arg0) + 0));
    *(s16 *)((s8*)(arg1) + 2) = (s16) (0x20 - *(u16 *)((s8*)(arg0) + 2));
    *(u16 *)((s8*)(arg1) + 0) = (u16) (*(u16 *)((s8*)(arg1) + 0) + 0x4F);
}


void func_80085E90(s32 arg0, s16 *arg1, s32 arg2) {
    s16 var_v0;

    if (arg0 != 0) {
        var_v0 = 0xDA - arg2;
    } else {
        var_v0 = arg2 + 0x67;
    }
    *arg1 = var_v0;
}


void func_80085EAC(s32 arg0, s16 *arg1, s32 arg2) {
    s16 var_v0;

    if (arg0 != 0) {
        var_v0 = 0x20 - arg2;
    } else {
        var_v0 = arg2 + 6;
    }
    *arg1 = var_v0;
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80085EC8);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_800864B4);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_800866D4);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_800868E0);
extern s32 D_8009A300;
void func_80086E24(void) {
    AddPrim(D_80092938, (D_800928A0 * 0x310) + (u8 *)&D_8009A300);
}
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80086E70);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80086FF8);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80087068);
extern u8 D_8009A2F8[];
void func_80085EC8(void*);
void func_800875EC(void) {
    struct Record { u32 words[0xC4]; };
    struct Record* record = (struct Record*)D_8009A2F8;
    func_80085EC8(record);
    record[1] = record[0];
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80087650);
extern u8 D_80091834[];
extern u8 D_800918B4[];
extern u8 *D_800927CC;
extern u8 *D_800927D0;

void func_80087698(s32 x0, s32 y0, s32 x1, s32 y1) {
    s32 tmp;
    s32 x;
    s32 dx;
    s32 v;
    u8 *p;

    if (y0 != y1) {
        if (y1 < y0) {
            tmp = x1;
            x1 = x0;
            x0 = tmp;
            tmp = y1;
            y1 = y0;
            y0 = tmp;
        }
        x = x0 << 8;
        dx = ((x1 - x0) << 8) / (y1 - y0);
        for (; y0 < y1; y0++, x += dx) {
            if (y0 >= 0) {
                if (y0 >= 0x80) {
                    return;
                }
                v = x >> 8;
                p = D_800927D0 + y0;
                if (v < *p) {
                    s32 t = D_80091834[y0];
                    if (v < t) {
                        v = t;
                    }
                    *p = v;
                }
                p = D_800927CC + y0;
                if (*p < v) {
                    s32 t = D_800918B4[y0] + 1;
                    if (t < v) {
                        v = t;
                    }
                    *p = v;
                }
            }
        }
    }
}

extern s32 D_80059598;
extern s32 D_80091934;
void func_800732AC(void *, void *, s32);
void func_80072D18(s32, s32, s32);

void func_8008779C(s32 arg0, s32 arg1, s32 arg2) {
    D_80059598 = (D_80059598 & 0xFFFFFF) | 0x24000000;
    __asm__ volatile ("lwc2\t$6, 0( %0 )" : : "r"(&D_80059598)); /* gte_ldrgb(&D_80059598) */
    func_800732AC((void *)0x1F800120, &D_80091934, 0x30);
    func_80072D18(arg0, arg1, arg2);
}

extern u8 *D_800927CC;
extern u8 *D_800927D0;
extern u8 *D_80092854[];
extern u8 *D_80092858;
void func_800732AC(void *, void *, s32);

void func_80087830(void) {
    s32 i;
    u8 *p;

    D_800927CC = (u8 *)0x1F800000;
    D_800927D0 = (u8 *)0x1F800080;
    D_80092854[0] = HeapAlloc(0xE100, 0);
    D_80092858 = HeapAlloc(0xE100, 0);
    p = D_80092854[0];
    for (i = 0; i < 0x708; i++) {
        p[3] = 7;
        p[7] = 0x24;
        p += 0x20;
    }
    func_800732AC(D_80092858, D_80092854[0], 0xE100);
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_800878DC);
extern u16 D_800927D4;
extern u16 D_800927D8;
extern u8 D_800927DC;
extern u8 D_800927E0;

void func_80087AB0(void *arg0) {
    struct Packet { u32 tag, color; s16 x0,y0; u16 uv0,clut; s16 x1,y1; u16 uv1,tpage; s16 x2,y2; u16 uv2,pad2; s16 x3,y3; u16 uv3,pad3; };
    s32 u, v;
    s32 temp_a1;
    s32 temp_v0;
    s32 temp_v1;
    void *var_a3;
    void *var_t0;

    var_t0 = arg0 + 0x162C;
    *(s32 *)((s8*)(arg0) + 0x1608) = 0x2C101010;
    *(s8 *)((s8*)(arg0) + 0x1607) = 9;
    *(u8 *)((s8*)(arg0) + 0x160B) = (u8) (*(u8 *)((s8*)(arg0) + 0x160B) | 2);
    var_a3 = arg0 + 0x1604;
    ((struct Packet*)((u8*)arg0 + 0x1604))->clut = (u16) D_800927D8;
    ((struct Packet*)((u8*)arg0 + 0x1604))->tpage = (u16) D_800927D4;
    u = D_800927DC;
    v = D_800927E0;
    temp_v1 = v << 8;
    ((struct Packet*)((u8*)arg0 + 0x1604))->uv0 = (s16) (u | temp_v1);
    temp_a1 = u + 0x3F;
    temp_v0 = (v + 0x3F) << 8;
    ((struct Packet*)((u8*)arg0 + 0x1604))->uv1 = (s16) (temp_a1 | temp_v1);
    ((struct Packet*)((u8*)arg0 + 0x1604))->uv2 = (s16) (u | temp_v0);
    ((struct Packet*)((u8*)arg0 + 0x1604))->uv3 = (s16) (temp_a1 | temp_v0);
    *(struct Packet*)((u8*)arg0 + 0x162C) = *(struct Packet*)((u8*)arg0 + 0x1604);
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80087B74);
extern s32 D_800928F8;
extern s32 D_8009A988;
void func_80087E38(void *arg0) {
    s32 count = D_800928F8;
    register u8 *base asm("$4");
    s16 *p;
    if (count < 31) {
        base = (u8 *)&D_8009A988;
        p = (s16 *)(base + count * 0x68);
        p[0] = *(s32 *)arg0;
        p[1] = *(s32 *)((u8 *)arg0 + 4);
        p[2] = *(s32 *)((u8 *)arg0 + 8);
        D_800928F8 = count + 1;
    }
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80087EA0);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8008820C);

void func_800882D4(s32 arg0, s32 arg1, s32 arg2, s16 arg3) {
    void *temp_v0;

    temp_v0 = func_8008820C();
    if (temp_v0 != NULL) {
        *(s16 *)((s8*)(temp_v0) + 0x30) = arg3;
    }
}

extern u8 D_80095968[];

void func_80088308(void) {
    s32 i;

    for (i = 0x141C; i >= 0; i -= 0x34) {
        *(s16 *)(D_80095968 + i) = 0;
    }
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8008832C);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_800884E0);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8008859C);
s32 SquareRoot0(s32);                               /* extern */
s32 VectorNormalS(void *, void *);    /* extern */
s32 func_8002DC9C(s16, s16, s16);                   /* extern */

void func_80088658(void *arg0, void* arg1) {
    struct { s32 x,y,z,pad; } vector;
    s16 temp_a0;
    s16 temp_a1;
    s16 temp_a2;
    s32 temp_lo;
    s32 temp_lo_2;
    s32 temp_v0;
    s32 var_v0;

    temp_a0 = *(s16 *)((s8*)(arg0) + 0);
    vector.x = (s32) temp_a0;
    temp_a1 = *(s16 *)((s8*)(arg0) + 2);
    vector.y = (s32) temp_a1;
    temp_a2 = *(s16 *)((s8*)(arg0) + 4);
    vector.z = (s32) temp_a2;
    var_v0 = func_8002DC9C(temp_a0, temp_a1, temp_a2);
    if (var_v0 < 0) {
        var_v0 = -var_v0;
    }
    temp_v0 = SquareRoot0(var_v0);
    temp_lo = vector.x / temp_v0;
    temp_lo_2 = vector.y / temp_v0;
    vector.x = temp_lo;
    vector.y = temp_lo_2;
    vector.z = vector.z / temp_v0;
    VectorNormalS(&vector, arg1);
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_800886FC);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80088754);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_800887A4);
INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80088838);
extern u8 D_8006F978[];

void func_800888B0(arg0)
    s32 arg0;
{
    s32 bit;

    bit = 1;
    bit <<= arg0 & 7;
    D_8006F978[arg0 >> 3] |= bit;
}

s32 func_800888E4(s32 arg0) {
    s32 bit = 1;
    bit <<= arg0 & 7;
    return D_8006F978[arg0 >> 3] & bit;
}

extern u8 D_8006F978[];

void func_80088908(s32 arg0) {
    s32 bit;

    bit = 1;
    bit <<= arg0 & 7;
    D_8006F978[arg0 >> 3] &= ~bit;
}


void func_80088940(void) {
    D_8006F980[0] |= 0x10000;
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_8008895C);
s32 func_800888E4(s32);
void func_8008895C(void);
s32 func_800889C8__s32(void) asm("func_800889C8");

s32 func_800889C8__s32(void) {
    s32 i;

    for (i = 0; i < 0x31; i++) {
        if (i != 0x16 && func_800888E4(i) == 0) {
            return 0;
        }
    }
    D_8006F980[0] |= 0x10000;
    func_8008895C();
    return 0;
}

typedef struct {
    u32 unk0 : 4;
    u32 unk4 : 1;
    u32 unk5 : 1;
    u32 unk6 : 7;
    u32 unk13 : 3;
    u32 unk16 : 16;
} func_80088A40_bits;
extern func_80088A40_bits D_8006F980__bits[] asm("D_8006F980");

void func_80088A40(void) {
    if (*(u8 *)&D_8005061C != 0) {
        D_8006F980__bits[0].unk0 = 1;
        D_8006F980__bits[0].unk4 = D_80099D9B[0];
        D_8006F980__bits[0].unk5 = D_80099D9C[0];
        D_8006F980__bits[0].unk6 = D_80099D9F;
        D_8006F980__bits[0].unk13 = D_80099D98[0];
    }
}

extern s8 D_800927EC;

void func_80088AF8(void) {
    u32 *pbits;
    u32 bits;
    s32 i;
    u8 *p;

    if (*(u8 *)&D_8005061C != 0) {
        pbits = (u32 *)D_8006F980;
        bits = *pbits;
        D_800927EC = 0;
        if ((bits & 0xF) == 1) {
            D_80099D9B[0] = (bits >> 4) & 1;
            D_80099D9C[0] = (bits >> 5) & 1;
            D_80099D9F = (bits >> 6) & 0x7F;
            D_80099D98[0] = (bits >> 13) & 7;
            if (bits & 0x10000) {
                i = 7;
                p = (u8 *)pbits - 1;
                do {
                    *p = 0;
                    i--;
                    p--;
                } while (i >= 0);
            }
        } else {
            D_80099D9B[0] = 0;
            D_80099D9C[0] = 0;
            D_80099D9F = 2;
            D_80099D98[0] = 0;
            func_80088A40();
        }
    }
}


void func_80088BD4(void) {
    func_800888B0();
    func_800889C8();
}

INCLUDE_ASM("asm/battling/nonmatchings/misc", func_80088BFC);

void func_80088C28(void) {
    if (g_C1ButtonState & 4) {
        D_800927F4 += 1;
    }
    if (g_C1ButtonState & 1) {
        D_800927F4 -= 1;
    }
    if (D_800927F4 < 0) {
        D_800927F4 = 0;
    }
    HeapDebugDump(1, D_800927F4, 0xA, 0x80AD);
}

extern s32 D_8009A0D8;
void func_80088CBC(s32 arg0) {
    void *temp_s0;

    temp_s0 = (arg0 * 0xF8) + (u8 *)&D_8009A0D8;
    (*(s8 *)((u8 *)(temp_s0) + 0x77)) = 3;
    (*(s8 *)((u8 *)(temp_s0) + 0x7B)) = 0x7D;
    (*(s16 *)((u8 *)(temp_s0) + 0x80)) = 0x3000;
    (*(s16 *)((u8 *)(temp_s0) + 0x82)) = GetClut(0x3F0, 0xC0);
}
void ArchiveSetIndex(s32, s32);
void DrawSyncCallback(void *);
void FontLoadFont(s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32);
void InitGeom();
void func_8008A110(s32, s32);
void func_8008A128(s32, s32);
extern u8 func_80088BFC__data[] asm("func_80088BFC");
extern s32 D_80010000;
extern s32 D_80091BB0;
extern u8 *D_80092868;
extern void *D_80092870;
extern s16 D_80092898;
extern s32 D_8009289C;
extern s32 D_800928CC;
extern s16 D_800928D0;
extern s32 D_800928E8;
extern s32 D_80092930;
extern u8 D_8009A0D8__a[] asm("D_8009A0D8");

void func_80088D1C(void) {
    u8 unused[8]; /* retail frame has 8 bytes of locals no code touches */

    DrawSyncCallback(func_80088BFC__data + 4);
    InitGeom();
    HeapChangeCurrentUser(6, &D_80091BB0);
    ArchiveSetIndex(0x30, 0);
    FontLoadFont(4, 2, 0x138, 0xDA, 0x14, 1, 0x3C0, 0x1F0, 0x3C0, 0x1EF, 0);
    func_80088CBC(0);
    func_80088CBC(1);
    switch (D_80010000) {
    case -1:
        D_800928CC = 2;
        break;
    case 0:
        D_800928CC = 1;
        break;
    default:
        D_800928CC = 0;
        break;
    }
    D_80092868 = D_8009A0D8__a;
    D_80092870 = D_8009A0D8__a + 0xF8;
    func_8008A110(-1, -1);
    func_8008A128(-1, -1);
    D_80092898 = 2;
    D_8009289C = 1;
    D_80092930 = 0;
    D_800928E8 = 0;
    D_800928A0 = 0;
    D_80092920 = 0;
    D_80092930 = 0;
    D_800928D0 = 7;
    func_8008E620();
}

#endif
