#include "common.h"

#ifndef XENO_PC_PORT
s32 ArchiveDecodeAlignedSize(s32);
void ArchiveReadFileToBuffer(s32, s32, s32, s32);
s32 HeapAlloc(s32, s32);
extern s32 D_8006259C;
extern s32 D_8009D3C8;
void HeapFree(s32);
extern s32 D_8009BE14;
extern s32 D_8009BE18;
extern s32 D_8009D30C;
extern s32 D_8009D780;
extern s32 D_8009D7D0;
void DrawSync(s32);
void EnterCriticalSection();
void ExitCriticalSection();
void FlushCache();
void Vsync(s32);
extern s32 D_8009D554;
extern s32 D_8009D7CC;
extern s32 D_8009C620;
s16 func_80084DB8(s32, s16);
extern s16 D_8009D7E0;
extern s32 D_8009D7E8;
extern s32 D_8009D7EC;
extern s32 D_8009CEB4;
extern s32 D_8009D150;
extern s32 D_8009D7F8;
extern s32 D_8009D7FC;
extern s32 D_8009BDF4;
extern s32 D_8009BE1C;
extern s32 D_8009BE20;
extern s32 g_GameState;
void func_800346D4(s32 *);
extern s32 D_8009D498;
extern s32 D_8009BD64;
extern s32 D_8009D160;
extern s32 D_8009D2B4;
s32 func_80093E8C();
void *func_80093660(s32, s32);
s32 ratan2(s32, s32, s32);
s32 rcos(s16);
s32 rsin(s16);
s32 func_8002C3D8();
extern s32 D_8009BE08;
extern s32 D_8009D3C0;
extern s32 D_8009D7D4;
extern s32 D_8009BCB8;
extern s32 D_8009BE44;
void func_800967E4();
extern s32 D_8009BE24;
void OuterProduct0(s32 *, s32 *, s32 *);
extern s32 D_8009BB4C[];
extern s32 D_8009BB5C[];
extern s32 D_8009BB6C[];
extern s32 D_8009BB7C[];
extern s32 D_8009BB8C[];
extern s32 D_8009BB9C[];
extern s32 D_8009C7F0[];
extern s32 D_8009C828[];
extern s32 D_8009C844[];
extern s32 D_8009C874[];
void func_800721E4(void);
void func_8007474C(void);
void func_80074F04(void);
void func_800750DC(void);
void func_800762FC(void);
s32 func_80076BC4(void);
s32 func_80076BDC(void *arg0, s16 arg1);
void func_800848B4(s32 arg0, s32 arg1);
s16 func_80084D00(s32 arg0, s16 *arg1);
void func_80086124(void);
void func_80086568(void);
void func_800866C8(void);
void func_80088FF4(void);
void func_80089128(void);
void func_8008DFF4(void *arg0);
void func_8008E034(void *arg0);
void func_80092DD0(void);
void func_800931B0(void);
void func_80093354(void *arg0);
void func_800933EC(void *arg0);
void func_80093534(void *arg0);
void func_800935DC(void *arg0, void *arg1, void *arg2);
s32 func_80093FE4(void);
u32 func_80094004(void);
s32 func_80094028(void *arg0);
void func_800941C4(void *arg0, void *arg1, void *arg2, s16 *arg3);
void func_800952B0(void *arg0, void *arg1, void *arg2);
void func_800960BC(void);
s32 func_80096668(void);
void func_80096694(void);
void func_800976A0(void);
void func_800976FC(s32 arg0, s32 arg1);
void func_80097718(s32 arg0, s32 arg1);
void func_80098044(void);
#endif

INCLUDE_ASM("asm/world_map/nonmatchings/main", WorldMapMain);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800712D0);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80071A50);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80071A58);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80071B9C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80071CDC);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80071EF0);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80071FEC);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80072090);


#ifndef XENO_PC_PORT
void func_800721E4(void) {
    s32 temp_v0;

    temp_v0 = HeapAlloc(ArchiveDecodeAlignedSize(D_8009D3C8), 0);
    D_8006259C = temp_v0;
    ArchiveReadFileToBuffer(D_8009D3C8, temp_v0, 0, 0);
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80072238);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007299C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80072BB0);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80072DB4);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80073300);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80073398);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80073448);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80073530);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007369C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800736DC);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800737EC);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800739B8);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80073B04);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80073E30);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800740B8);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80074594);


#ifndef XENO_PC_PORT
void func_8007474C(void) {
    HeapFree(D_8009BE18);
    HeapFree(D_8009BE14);
    HeapFree(D_8009D30C);
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80074794);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800747DC);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80074E58);


#ifndef XENO_PC_PORT
void func_80074F04(void) {
    HeapFree(D_8009D780);
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80074F2C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80075030);


#ifndef XENO_PC_PORT
void func_800750DC(void) {
    HeapFree(D_8009D7D0);
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80075104);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80075228);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007528C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80075460);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007565C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800758C0);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80075B58);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80075D4C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80075E7C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80076098);


#ifndef XENO_PC_PORT
void func_800762FC(void) {
    DrawSync(0);
    Vsync(0);
    EnterCriticalSection();
    DrawSync(0);
    Vsync(0);
    FlushCache();
    ExitCriticalSection();
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007634C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80076594);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800767D4);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80076858);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80076954);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80076A14);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80076A1C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80076B34);


#ifndef XENO_PC_PORT
s32 func_80076BC4(void) {
    D_8009D554 = 0;
    D_8009D7CC = 0;
    return 0;
}
#endif



#ifndef XENO_PC_PORT
s32 func_80076BDC(void *arg0, s16 arg1) {
    s16 temp_v0;
    s16 temp_v0_2;

    temp_v0_2 = *(s16 *)((s8*)(arg0) + 0x22);
    if (temp_v0_2 == 0) {
        *(s16 *)((s8*)(arg0) + 0x22) = arg1;
        return 0;
    }
    temp_v0 = temp_v0_2 - 1;
    *(s16 *)((s8*)(arg0) + 0x22) = temp_v0;
    return ((temp_v0 << 0x10) < 1) * 2;
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80076C18);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80076C3C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80076C68);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80076C88);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80076CB4);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80076CD4);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80076CF4);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80076D1C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80076D50);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80076D8C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80076DA4);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80076F54);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80076FA8);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800771D8);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80077214);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80077480);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007756C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800776E0);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80077954);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007795C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80077A64);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80077CC0);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80077DC8);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80077E68);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007828C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800783E8);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80078948);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80078950);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80078A60);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80078D24);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80078E2C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80078EA4);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800794D8);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80079538);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800795E4);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80079778);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007A06C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007A144);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007A1B4);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007A410);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007A430);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007A568);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007A570);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007A5DC);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007A8AC);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007A9B4);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007A9F8);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007AD34);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007ADD4);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007B200);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007B394);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007B604);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007B798);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007BA08);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007BA10);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007BB60);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007BBEC);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007BF50);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007C260);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007C36C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007C3B8);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007C724);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007C7D8);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007CC6C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007CD20);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007CE84);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007CF18);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007D078);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007D110);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007D228);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007D2B8);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007D414);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007D4A4);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007D600);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007D690);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007D774);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007D7FC);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007D918);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007DCE0);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007DE14);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007DE98);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007E450);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007E4E4);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007EBBC);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007ECA4);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007EE34);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007F8AC);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007F968);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007FC8C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007FD30);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007FF70);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80080218);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008032C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80080370);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80080578);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80080600);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80080900);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80080944);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800809EC);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80080A28);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80080AC4);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80080D00);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008106C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80081174);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800811C0);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800813E8);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80081470);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800816DC);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800817A0);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80081868);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800819C8);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80081B24);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80081C3C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80081D80);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80081FB4);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80081FD8);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80082324);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800826B4);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800827C8);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800827EC);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800828DC);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80082F64);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80083108);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800831D8);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80083214);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80083264);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800834D0);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800834D8);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008355C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800837DC);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800838E8);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008390C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80083A00);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80083FE4);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80084068);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008440C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80084580);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80084818);


#ifndef XENO_PC_PORT
void func_800848B4(s32 arg0, s32 arg1) {
    *(s32 *)((s8*)(((arg1 * 0x54) + D_8009C620)) + 0x50) = (s32) (D_8009C620 + (arg0 * 0x54));
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800848F4);


#ifndef XENO_PC_PORT
s16 func_80084D00(s32 arg0, s16 *arg1) {
    s16 temp_v0;
    s16 temp_v0_2;
    s16 var_s0;
    void *var_s1;

    var_s1 = (*(void **)&D_8009C620);
    var_s0 = 0;
    if (D_8009D7E0 > 0) {
loop_1:
        if (*(u16 *)((s8*)(var_s1) + 4) & 1) {
            temp_v0 = func_80084DB8(arg0, var_s0);
            if (temp_v0 != 0) {
                *arg1 = var_s0;
                return temp_v0;
            }
        }
        temp_v0_2 = var_s0 + 1;
        var_s0 = temp_v0_2;
        var_s1 += 0x54;
        if (temp_v0_2 >= D_8009D7E0) {
            goto block_5;
        }
        goto loop_1;
    }
block_5:
    return 0;
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80084DB8);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80085158);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80085418);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80085760);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80085CDC);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80085F58);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80085FE0);


#ifndef XENO_PC_PORT
void func_80086124(void) {
    HeapFree(D_8009D7EC);
    HeapFree(D_8009D7E8);
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008615C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800863E0);


#ifndef XENO_PC_PORT
void func_80086568(void) {
    HeapFree(D_8009CEB4);
    HeapFree(D_8009D150);
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800865A0);


#ifndef XENO_PC_PORT
void func_800866C8(void) {
    HeapFree(D_8009D7FC);
    HeapFree(D_8009D7F8);
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80086700);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80086798);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80087710);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80087734);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800877E0);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80087804);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80087904);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800879A8);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800879E0);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80087A8C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80087B84);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80087C6C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80087F60);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80087FD0);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80088570);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008868C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80088720);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80088B40);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80088C90);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80088D00);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80088D64);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80088DE4);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80088E1C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80088E68);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80088EA0);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80088F1C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80088F54);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80088F5C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80088F64);


#ifndef XENO_PC_PORT
void func_80088FF4(void) {
    HeapFree(D_8009BDF4);
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008901C);


#ifndef XENO_PC_PORT
void func_80089128(void) {
    HeapFree(D_8009BE1C);
    HeapFree(D_8009BE20);
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80089160);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800893E0);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800894C8);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80089514);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80089580);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80089748);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80089C78);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008A2C8);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008A52C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008A5B8);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008A72C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008B2BC);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008B498);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008B54C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008B644);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008BB40);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008BD1C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008BDD0);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008BEC8);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008BFD4);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008C040);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008C1DC);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008C28C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008C364);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008C530);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008C6EC);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008C75C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008C844);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008D3F0);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008D520);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008D590);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008D678);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008DD6C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008DE9C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008DF0C);


#ifndef XENO_PC_PORT
void func_8008DFF4(void *arg0) {
    *(s32 *)((s8*)(arg0) + 0) = (s32) (*(s16 *)((s8*)(&g_GameState) + 0x182C) << 0xC);
    *(s32 *)((s8*)(arg0) + 4) = (s32) (*(s16 *)((s8*)(&g_GameState) + 0x182E) << 0xC);
    *(s32 *)((s8*)(arg0) + 8) = (s32) (*(s16 *)((s8*)(&g_GameState) + 0x1830) << 0xC);
}
#endif



#ifndef XENO_PC_PORT
void func_8008E034(void *arg0) {
    *(s16 *)((s8*)(&g_GameState) + 0x182C) = (s16) ((s32) *(s32 *)((s8*)(arg0) + 0) >> 0xC);
    *(s16 *)((s8*)(&g_GameState) + 0x182E) = (s16) ((s32) *(s32 *)((s8*)(arg0) + 4) >> 0xC);
    *(s16 *)((s8*)(&g_GameState) + 0x1830) = (s16) ((s32) *(s32 *)((s8*)(arg0) + 8) >> 0xC);
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008E078);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008E0F0);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008E190);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008E4F4);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008E680);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008E76C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800906E0);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800907C4);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800907F4);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80090A18);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80090A84);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80090C68);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80090E14);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80090FB4);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80091430);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800914D0);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80091B54);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80091C18);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80091FF8);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80092234);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800922AC);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800923A8);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800925A0);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80092BE4);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80092C70);


#ifndef XENO_PC_PORT
void func_80092DD0(void) {
    func_800346D4(&D_8009D498);
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80092DF8);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80092FD8);


#ifndef XENO_PC_PORT
void func_800931B0(void) {
    func_800346D4(&D_8009BD64);
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800931D8);


#ifndef XENO_PC_PORT
void func_80093354(void *arg0) {
    s32 temp_a1;
    s32 temp_a1_2;
    s32 temp_v1;
    s32 temp_v1_2;
    s32 temp_v1_3;
    s32 temp_v1_4;

    temp_a1 = *(s32 *)((s8*)(arg0) + 0);
    temp_v1 = D_8009D160 << 0x17;
    if (temp_a1 >= temp_v1) {
        *(s32 *)((s8*)(arg0) + 0) = (s32) (temp_a1 - temp_v1);
    }
    temp_v1_2 = *(s32 *)((s8*)(arg0) + 0);
    if (temp_v1_2 < 0) {
        *(s32 *)((s8*)(arg0) + 0) = (s32) (temp_v1_2 + (D_8009D160 << 0x17));
    }
    temp_a1_2 = *(s32 *)((s8*)(arg0) + 8);
    temp_v1_3 = D_8009D2B4 << 0x17;
    if (temp_a1_2 >= temp_v1_3) {
        *(s32 *)((s8*)(arg0) + 8) = (s32) (temp_a1_2 - temp_v1_3);
    }
    temp_v1_4 = *(s32 *)((s8*)(arg0) + 8);
    if (temp_v1_4 < 0) {
        *(s32 *)((s8*)(arg0) + 8) = (s32) (temp_v1_4 + (D_8009D2B4 << 0x17));
    }
}
#endif



#ifndef XENO_PC_PORT
void func_800933EC(void *arg0) {
    s32 temp_a1;
    s32 temp_a1_2;
    s32 temp_v1;
    s32 temp_v1_2;
    s32 temp_v1_3;
    s32 temp_v1_4;

    temp_a1 = *(s32 *)((s8*)(arg0) + 0);
    temp_v1 = D_8009D160 << 0xB;
    if (temp_a1 >= temp_v1) {
        *(s32 *)((s8*)(arg0) + 0) = (s32) (temp_a1 - temp_v1);
    }
    temp_v1_2 = *(s32 *)((s8*)(arg0) + 0);
    if (temp_v1_2 < 0) {
        *(s32 *)((s8*)(arg0) + 0) = (s32) (temp_v1_2 + (D_8009D160 << 0xB));
    }
    temp_a1_2 = *(s32 *)((s8*)(arg0) + 8);
    temp_v1_3 = D_8009D2B4 << 0xB;
    if (temp_a1_2 >= temp_v1_3) {
        *(s32 *)((s8*)(arg0) + 8) = (s32) (temp_a1_2 - temp_v1_3);
    }
    temp_v1_4 = *(s32 *)((s8*)(arg0) + 8);
    if (temp_v1_4 < 0) {
        *(s32 *)((s8*)(arg0) + 8) = (s32) (temp_v1_4 + (D_8009D2B4 << 0xB));
    }
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80093484);


#ifndef XENO_PC_PORT
void func_80093534(void *arg0) {
    s32 temp_v1;
    s32 temp_v1_2;
    s32 var_v0;
    s32 var_v0_2;

    temp_v1 = *(s32 *)((s8*)(arg0) + 0);
    if (temp_v1 < -0x4000) {
        var_v0 = temp_v1 + (D_8009D160 << 0xB);
        goto block_4;
    }
    if (temp_v1 >= 0x4001) {
        var_v0 = temp_v1 - (D_8009D160 << 0xB);
block_4:
        *(s32 *)((s8*)(arg0) + 0) = var_v0;
    }
    temp_v1_2 = *(s32 *)((s8*)(arg0) + 8);
    if (temp_v1_2 < -0x4000) {
        var_v0_2 = temp_v1_2 + (D_8009D2B4 << 0xB);
        goto block_9;
    }
    if (temp_v1_2 >= 0x4001) {
        var_v0_2 = temp_v1_2 - (D_8009D2B4 << 0xB);
block_9:
        *(s32 *)((s8*)(arg0) + 8) = var_v0_2;
    }
}
#endif



#ifndef XENO_PC_PORT
void func_800935DC(void *arg0, void *arg1, void *arg2) {
    s32 temp_lo;

    temp_lo = (s32) (-(*(s32 *)((s8*)(arg2) + 0) * (*(s32 *)((s8*)(arg0) + 0) - *(s32 *)((s8*)(arg1) + 0))) - (*(s32 *)((s8*)(arg2) + 8) * (*(s32 *)((s8*)(arg0) + 8) - *(s32 *)((s8*)(arg1) + 8)))) / (s32) *(s32 *)((s8*)(arg2) + 4);
    *(s32 *)((s8*)(arg0) + 4) = temp_lo;
    *(s32 *)((s8*)(arg0) + 4) = (s32) (temp_lo + *(s32 *)((s8*)(arg1) + 4));
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80093660);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80093740);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80093978);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80093A5C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80093E8C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80093F18);


#ifndef XENO_PC_PORT
s32 func_80093FE4(void) {
    return func_80093E8C() & 0xF;
}
#endif



#ifndef XENO_PC_PORT
u32 func_80094004(void) {
    return (u32) (func_80093E8C() << 0x10) >> 0x1A;
}
#endif



#ifndef XENO_PC_PORT
s32 func_80094028(void *arg0) {
    return ((u8) *(u8 *)((s8*)(func_80093660(*(s32 *)((s8*)(arg0) + 0), *(s32 *)((s8*)(arg0) + 8))) + 3) >> 2) & 0xF;
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80094060);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80094088);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80094154);


#ifndef XENO_PC_PORT
void func_800941C4(void *arg0, void *arg1, void *arg2, s16 *arg3) {
    s16 temp_a0;
    s32 temp_a2;

    temp_a2 = *(s32 *)((s8*)(arg1) + 0);
    temp_a0 = (ratan2(*(s32 *)((s8*)(arg1) + 8) - *(s32 *)((s8*)(arg0) + 8), temp_a2 - *(s32 *)((s8*)(arg0) + 0), temp_a2) + 0x400) & 0xFFF;
    *arg3 = temp_a0;
    *(s32 *)((s8*)(arg2) + 0) = rcos(temp_a0);
    *(s32 *)((s8*)(arg2) + 8) = (s32) -rsin(*arg3);
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80094238);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80094364);

void func_80094434(void) {
}

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8009443C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800945C8);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80094750);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800948D8);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80094A5C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800951A8);


#ifndef XENO_PC_PORT
void func_800952B0(void *arg0, void *arg1, void *arg2) {
    s32 temp_a3;
    s32 temp_v1;

    temp_a3 = *(s32 *)((s8*)(arg2) + 0);
    temp_v1 = (temp_a3 * *(s32 *)((s8*)(arg0) + 0)) + (*(s32 *)((s8*)(arg2) + 8) * *(s32 *)((s8*)(arg0) + 8));
    if (temp_v1 < 0) {
        *(s32 *)((s8*)(arg1) + 0) = (s32) -temp_a3;
        *(s32 *)((s8*)(arg1) + 8) = (s32) -*(s32 *)((s8*)(arg2) + 8);
    } else if (temp_v1 > 0) {
        *(s32 *)((s8*)(arg1) + 0) = temp_a3;
        *(s32 *)((s8*)(arg1) + 8) = (s32) *(s32 *)((s8*)(arg2) + 8);
    } else {
        *(s32 *)((s8*)(arg1) + 8) = 0;
        *(s32 *)((s8*)(arg1) + 0) = 0;
    }
    *(s32 *)((s8*)(arg1) + 4) = 0;
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80095324);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80095414);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80095CD4);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80095F78);


#ifndef XENO_PC_PORT
void func_800960BC(void) {
    s32 temp_s0;
    s32 var_a0;

    temp_s0 = func_8002C3D8();
    if (((temp_s0 == 0) | (~func_8002C3D8() == 0)) != 0) {
        var_a0 = D_8009BE08;
    } else {
        var_a0 = D_8009D3C0;
    }
    HeapFree(var_a0);
    HeapFree(D_8009D7D4);
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80096130);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8009623C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800962B0);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80096328);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800963E4);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800964B0);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800965A4);


#ifndef XENO_PC_PORT
s32 func_80096668(void) {
    s32 var_v0;

    var_v0 = D_8009BE44 - D_8009BCB8;
    if (var_v0 < 0) {
        var_v0 += 0x10;
    }
    return var_v0;
}
#endif



#ifndef XENO_PC_PORT
void func_80096694(void) {
    do {
        Vsync(0);
        func_800967E4();
    } while (func_80096668() != 0);
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800966CC);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800967E4);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800968E0);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8009699C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80096A6C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80096C0C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80096F18);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80097070);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80097244);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80097440);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8009766C);


#ifndef XENO_PC_PORT
void func_800976A0(void) {
    HeapFree(D_8009BE24);
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800976C8);


#ifndef XENO_PC_PORT
void func_800976FC(s32 arg0, s32 arg1) {
    void *temp_a1;

    temp_a1 = (arg1 << 7) + D_8009BE24;
    *(s16 *)((s8*)(temp_a1) + 0) = 0;
    *(s32 *)((s8*)(temp_a1) + 0x18) = arg0;
}
#endif



#ifndef XENO_PC_PORT
void func_80097718(s32 arg0, s32 arg1) {
    s32 var_a2;
    void *var_v1;

    var_a2 = 0;
    var_v1 = (*(void **)&D_8009BE24);
loop_1:
    if (*(s32 *)((s8*)(var_v1) + 0x1C) == 0) {
        *(s16 *)((s8*)(var_v1) + 0) = 0;
        *(s16 *)((s8*)(var_v1) + 2) = 0;
        *(s16 *)((s8*)(var_v1) + 4) = 0;
        *(s32 *)((s8*)(var_v1) + 0x18) = arg0;
        *(s32 *)((s8*)(var_v1) + 0x1C) = arg1;
        *(s16 *)((s8*)(var_v1) + 0x20) = 0;
        *(s16 *)((s8*)(var_v1) + 0x22) = 0;
        return;
    }
    var_a2 += 1;
    var_v1 += 0x80;
    if (var_a2 >= 0x40) {
        return;
    }
    goto loop_1;
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80097770);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800977A8);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800977C4);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800977E0);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80097800);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800978FC);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800979C8);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80097BC0);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80097CB8);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80097D64);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80097DC0);


#ifndef XENO_PC_PORT
void func_80098044(void) {
    OuterProduct0(D_8009BB6C, D_8009BB4C, D_8009C828);
    OuterProduct0(D_8009BB4C, D_8009BB7C, D_8009C844);
    OuterProduct0(D_8009BB8C, D_8009BB5C, D_8009C874);
    OuterProduct0(D_8009BB5C, D_8009BB9C, D_8009C7F0);
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800980D4);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800981C8);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800983A0);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800987AC);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80098CC0);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8009932C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80099708);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8009980C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80099BFC);
