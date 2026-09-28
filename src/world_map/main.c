#include "common.h"

#ifdef XENO_PC_PORT
/* Port view of the overlay data used by the bodies the port runs
 * (pc_port/src/world_map_port_data.h, owned by the port). */
#include "../../pc_port/src/world_map_port_data.h"
#endif

#ifndef XENO_PC_PORT
extern s32 D_8009BE24;
void HeapFree(s32);
void func_8002CBBC(s32);
extern s32 D_8009C620;
extern s16 D_8009D7E0;
s32 ArchiveDecodeAlignedSize(s32);
s32 HeapAlloc(s32, s32);
void func_80029AFC(s16 *, s32, s32);
extern s32 D_8005945C;
extern s16 D_8009D3F8;
extern s32 D_8009D3FC;
extern s16 D_8009D400;
extern s32 D_8009D404;
extern s16 D_8009D408;
extern s32 D_8009D40C;
extern s32 D_8009D528;
void func_80087904(void *, s32, u16, s32);
extern u16 D_8009B64C[];
extern u16 D_8009B64E[];
extern s32 D_8009C610;
int abs(int);
s32 SquareRoot0(s32);
s32 func_80097770(s32 arg0, s32 arg1);
void func_80084818(void);
void func_80071FEC(void);
s32 func_800879E0(s32 arg0);
s32 func_80094154(void *arg0, void *arg1);
#endif

#ifndef XENO_PC_PORT
int abs(int);
void func_800963E4(s32 *);
extern s32 D_8009BE08;
extern s32 D_8009BE44;
extern s32 D_8009D788[];
extern s32 D_8009D808;
void func_800964B0(s32 *);
extern s32 D_8009C624[];
extern s32 D_8009D3C0;
void func_80093484(void *, s32, s32, s32);
extern s16 D_8009BD38;
extern u16 D_8009BD3A;
extern s16 D_8009BD3C;
extern s32 D_8009D3F0;
s32 func_800771D8(s32 arg0, s32 arg1, s32 arg2);
void func_800809EC(u8 *arg0, s32 arg1, s8 arg2, s8 arg3, s32 arg4);
s32 func_80096328(void);
s32 func_800965A4(void);
void func_80076DA4(void *arg0, void *arg1);
void func_80076F54(void *arg0);
#endif

#ifndef XENO_PC_PORT
void func_800848B4(s32 arg0, s32 arg1);
extern u16 D_8009B674[];
extern s32 D_8009C610;
void HeapFree(s32);
extern s32 D_8009C184[];
extern u16 D_8009A450;
extern u16 D_8009A46C[];
extern s32 D_8009BE24;
extern u16 D_8009A698;
extern u16 D_8009A6AC[];
extern u16 D_8009B69C[];
extern s32 D_8009C620;
extern u16 D_8009B6B0[];
s32 func_800879E0(s32);
void func_8008BFD4(s32, s32, s32, s32);
s32 func_80087F60(void);
void func_80097D64(void);
s32 func_8007A9B4(s32 arg0);
s32 func_8008032C(s32 arg0);
s32 func_80088D64(s32 arg0);
s32 func_80088EA0(s32 arg0);
s32 func_800879A8(s32 arg0);
s32 func_80088DE4(s32 arg0);
s32 func_80088E68(s32 arg0);
s32 func_80088F1C(s32 arg0);
#endif

#ifndef XENO_PC_PORT
s32 ArchiveDecodeAlignedSize(s32);
s32 HeapAlloc(s32, s32);
void func_80029AFC(s16 *, s32, s32);
extern s32 D_8004F304;
extern s32 D_8006259C;
extern s32 D_8009BCC8;
extern s32 D_8009C614;
extern s32 D_8009C884;
extern s32 D_8009C888;
extern s32 D_8009C88C;
extern s32 D_8009CC98;
extern s32 D_8009D3C8;
extern s32 D_8009D3D0;
extern s16 D_8009D3F8;
extern s32 D_8009D3FC;
extern s16 D_8009D400;
extern s32 D_8009D404;
extern s16 D_8009D408;
extern s32 D_8009D40C;
extern s16 D_8009D410;
extern s32 D_8009D414;
extern s16 D_8009D418;
extern s32 D_8009D41C;
extern s16 D_8009D420;
extern s32 D_8009D424;
extern s32 D_8009D800;
extern s32 D_8009BC38;
extern s32 D_8009BCB0;
s32 func_80097770(s32, s32);
extern s32 D_8009C5AC;
extern s32 D_8009C5B0;
extern s32 D_8009C5B4;
void func_80089160(s32, s32, s32);
void func_80089514(s32);
void func_8003A89C(s32);
extern s32 D_80062528;
void func_80039E60(s32);
void func_8003A3B8(s32, s32, s32);
extern s32 D_8009CCA4;
extern s32 D_8009D3CC;
void HeapFree(s32);
void func_80074F04(void);
void func_800750DC(void);
void func_80084818();
void func_80086124(void);
void func_80086568(void);
void func_800866C8(void);
void func_80088FF4(void);
void func_80089128(void);
void func_800976A0(void);
void func_80097D64();
extern s16 D_8006F94E;
extern u16 D_8006F950;
extern s16 D_8006F954;
extern s32 D_8009BBC4;
extern s32 D_8009BC3C;
extern s32 D_8009BCB4;
extern u16 D_8009BD3A;
extern s32 D_8009C180;
void func_8003852C(s32);
void func_80039FF8();
extern s16 D_8009BD38;
extern s16 D_8009BD3C;
extern s32 D_8009BE0C;
extern s32 D_8009BE24;
extern s32 D_8009D144;
extern s32 D_8009D3F0;
extern s32 D_8009BE28;
extern s32 D_8009BE2C;
extern s32 D_8009BE30;
extern s32 D_8009D55C;
extern s32 D_8009D560;
extern s32 D_8009D564;
void func_80093354(void *arg0);
s32 func_80093A5C(s32, s32);
extern s32 D_8009C620;
extern s32 D_8009A758;
void DrawSync(s32);
void Vsync(s32);
s32 func_80096668(void);
void func_800967E4();
void func_80097BC0(s32 *);
extern s32 D_8009AC60;
void RotMatrix(s32, void *);
void OuterProduct12(void *, void *, void *);
void TransposeMatrix(void *, void *);
void VectorNormal(void *, void *);
extern s32 D_8009BCC0;
void SpriteSetScale(void *, s32);
void *func_80024524(s32, s32, s32, s32, s32, s32);
void func_800245D8(void *, s32);
extern s32 D_8009BE24__a_s32[] asm("D_8009BE24");
extern s32 D_8009CD34[];
extern s32 D_8009CD38;
extern s32 g_GameState;
extern s32 D_8009CD3C;
s32 func_8008C364(void *, s32);
extern s32 D_8009BE10;
void func_800848B4(s32 arg0, s32 arg1);
extern s32 D_8009BD34;
extern s32 D_8009C7E8;
extern u16 D_8009CD4C;
extern s32 D_8009CEC0;
void func_800976C8();
s32 func_80071A50(void);
void func_80072090(void);
void func_8007369C(void);
void func_80076858(s32 arg0, void *arg1, void *arg2, void *arg3, void *arg4);
s32 func_80076A14(void);
s32 func_80076C18(s32 arg0, s32 arg1, s32 arg2);
s32 func_80076C3C(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
s32 func_80076C68(s32 arg0, s16 arg1, s16 arg2, s16 arg3);
s32 func_80076C88(s32 arg0, s32 arg1);
s32 func_80076CB4(s32 arg0, s32 arg1);
s32 func_80076CD4(s32 arg0, s32 arg1);
s32 func_80076CF4(void);
s32 func_80076D1C(s32 arg0, s32 arg1);
s32 func_80076D50(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
s32 func_80076D8C(s32 arg0, s32 arg1, s32 arg2);
void func_80077480(void);
s32 func_80077954(void);
void func_80077CC0(void);
s32 func_80078948(void);
void func_80078D24(void);
s32 func_80078E2C(s32 arg0);
s32 func_8007A410(s32 arg0);
s32 func_8007A568(void);
void func_8007A8AC(void);
s32 func_8007AD34(s32 arg0);
s32 func_8007BA08(void);
void func_8007C260(void);
s32 func_8007C724(s32 arg0);
s32 func_8007D690(s32 arg0);
s32 func_8007E450(s32 arg0);
void func_80080218(void);
s32 func_80080578(s32 arg0);
s32 func_80080900(s32 arg0);
s32 func_80080944(s32 arg0);
void func_8008106C(void);
s32 func_800813E8(s32 arg0);
s32 func_80081FB4(s32 arg0);
void func_800826B4(void);
s32 func_800827C8(s32 arg0);
s32 func_800834D0(void);
s32 func_800834D8(s32 arg0);
void func_800837DC(void);
s32 func_800838E8(s32 arg0);
s32 func_80087710(s32 arg0);
s32 func_80087734(s32 arg0);
s32 func_800877E0(s32 arg0);
void func_80087B84(void *arg0, void *arg1, void *arg2);
s32 func_80088E1C(s32 arg0);
s32 func_80088F54(void);
s32 func_80088F5C(void);
void func_800894C8(s32 arg0);
s32 func_8008A52C(s32 arg0);
s32 func_8008B498(s32 arg0);
s32 func_8008BD1C(s32 arg0);
s32 func_8008C6EC(s32 arg0);
s32 func_8008D520(s32 arg0);
s32 func_8008DE9C(s32 arg0);
s32 func_800907C4(void);
void func_80090A18(void);
s32 func_80092234(s32 arg0);
void func_8009766C(void);
void func_800977E0(s32 arg0, s16 arg1);
#endif

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


s32 func_80071A50(void) {
    return 1;
}


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80071A58);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80071B9C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80071CDC);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80071EF0);


#ifndef XENO_PC_PORT
void func_80071FEC(void) {
    s32 temp_v0;

    D_8005945C = HeapAlloc(ArchiveDecodeAlignedSize(0x26), 1);
    temp_v0 = HeapAlloc(ArchiveDecodeAlignedSize(0x25), 1);
    D_8009D3F8 = 0x25;
    D_8009D528 = temp_v0;
    D_8009D3FC = temp_v0;
    D_8009D400 = 0x26;
    D_8009D408 = 0;
    D_8009D40C = 0;
    D_8009D404 = D_8005945C;
    func_80029AFC(&D_8009D3F8, 0, 0);
}
#endif



#ifndef XENO_PC_PORT
void func_80072090(void) {
    s32 temp_v0;
    s32 temp_v0_2;
    s32 temp_v0_3;
    s32 temp_v0_4;
    s32 temp_v0_5;

    D_8004F304 += 1;
    D_8009D3F8 = (s16) D_8009CC98;
    temp_v0 = HeapAlloc(ArchiveDecodeAlignedSize(D_8009CC98), 1);
    D_8009C88C = temp_v0;
    D_8009D3FC = temp_v0;
    D_8009D400 = (s16) D_8009D3D0;
    temp_v0_2 = HeapAlloc(ArchiveDecodeAlignedSize(D_8009D3D0), 0);
    D_8009C884 = temp_v0_2;
    D_8009D404 = temp_v0_2;
    D_8009D408 = (s16) D_8009D3C8;
    temp_v0_3 = HeapAlloc(ArchiveDecodeAlignedSize(D_8009D3C8), 0);
    D_8006259C = temp_v0_3;
    D_8009D40C = temp_v0_3;
    D_8009D410 = (s16) D_8009D800;
    temp_v0_4 = HeapAlloc(ArchiveDecodeAlignedSize(D_8009D800), 0);
    D_8009C888 = temp_v0_4;
    D_8009D414 = temp_v0_4;
    D_8009D418 = (s16) D_8009BCC8;
    temp_v0_5 = HeapAlloc(ArchiveDecodeAlignedSize(D_8009BCC8), 0);
    D_8009C614 = temp_v0_5;
    D_8009D41C = temp_v0_5;
    D_8009D420 = 0;
    D_8009D424 = 0;
    func_80029AFC(&D_8009D3F8, 0, 0);
}
#endif



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


#ifndef XENO_PC_PORT
void func_8007369C(void) {
    D_8009BC38 = HeapAlloc(0x1000, 0);
    D_8009BCB0 = HeapAlloc(0x1000, 0);
}
#endif


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


#ifndef XENO_PC_PORT
void func_80076858(s32 arg0, void *arg1, void *arg2, void *arg3, void *arg4) {
    s32 temp_a0;
    s32 temp_t0;
    s32 temp_v0;
    s32 temp_v1;

    temp_v0 = 0x1000 - arg0;
    temp_t0 = (s32) (temp_v0 * temp_v0 * 8) >> 0xC;
    temp_a0 = ((s32) (temp_v0 * arg0) >> 8) + 0x8000;
    temp_v1 = (s32) (arg0 * arg0 * 8) >> 0xC;
    *(s32 *)((s8*)(arg4) + 0) = (s32) ((*(s16 *)((s8*)(arg1) + 0) * temp_t0) + (*(s16 *)((s8*)(arg2) + 0) * temp_a0) + (*(s16 *)((s8*)(arg3) + 0) * temp_v1));
    *(s32 *)((s8*)(arg4) + 4) = (s32) ((*(s16 *)((s8*)(arg1) + 2) * temp_t0) + (*(s16 *)((s8*)(arg2) + 2) * temp_a0) + (*(s16 *)((s8*)(arg3) + 2) * temp_v1));
    *(s32 *)((s8*)(arg4) + 8) = (s32) ((*(s16 *)((s8*)(arg1) + 4) * temp_t0) + (*(s16 *)((s8*)(arg2) + 4) * temp_a0) + (*(s16 *)((s8*)(arg3) + 4) * temp_v1));
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80076954);


#ifndef XENO_PC_PORT
s32 func_80076A14(void) {
    return 1;
}
#endif


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



#ifndef XENO_PC_PORT
s32 func_80076C18(s32 arg0, s32 arg1, s32 arg2) {
    func_80097770(arg1, arg2);
    return 4;
}
#endif



#ifndef XENO_PC_PORT
s32 func_80076C3C(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    D_8009C5AC = arg1 << 0xC;
    D_8009C5B0 = arg2 << 0xC;
    D_8009C5B4 = arg3 << 0xC;
    return 4;
}
#endif



#ifndef XENO_PC_PORT
s32 func_80076C68(s32 arg0, s16 arg1, s16 arg2, s16 arg3) {
    *(s16 *)0x1F8000A0 = arg1;
    *(s16 *)0x1F8000A2 = arg2;
    *(s16 *)0x1F8000A4 = arg3;
    return 4;
}
#endif



#ifndef XENO_PC_PORT
s32 func_80076C88(s32 arg0, s32 arg1) {
    func_80089160(arg1, 0x1F8000A0, 0);
    return 2;
}
#endif



#ifndef XENO_PC_PORT
s32 func_80076CB4(s32 arg0, s32 arg1) {
    func_800894C8(arg1);
    return 2;
}
#endif



#ifndef XENO_PC_PORT
s32 func_80076CD4(s32 arg0, s32 arg1) {
    func_80089514(arg1);
    return 2;
}
#endif



#ifndef XENO_PC_PORT
s32 func_80076CF4(void) {
    func_8003A89C(D_80062528);
    return 4;
}
#endif



#ifndef XENO_PC_PORT
s32 func_80076D1C(s32 arg0, s32 arg1) {
    func_80039E60((*(u16 *)((s8*)((*(void **)&D_8006259C)) + 0x14) << 0x10) | arg1);
    return 2;
}
#endif



#ifndef XENO_PC_PORT
s32 func_80076D50(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    func_8003A3B8((*(u16 *)((s8*)((*(void **)&D_8006259C)) + 0x14) << 0x10) | arg1, arg2, arg3);
    return 4;
}
#endif



#ifndef XENO_PC_PORT
s32 func_80076D8C(s32 arg0, s32 arg1, s32 arg2) {
    D_8009CCA4 = arg1;
    D_8009D3CC = arg2;
    return 4;
}
#endif



#ifndef XENO_PC_PORT
void func_80076DA4(void *arg0, void *arg1) {
    s32 temp_a0;
    s32 temp_a1;
    s32 temp_a2;
    s32 temp_a3;
    s32 temp_v1;
    s32 temp_v1_2;
    s32 var_v0;
    s32 var_v0_2;
    s32 var_v0_3;

    temp_a3 = *(s32 *)((s8*)(arg0) + 0x38);
    temp_a2 = *(s32 *)((s8*)(arg0) + 0x50);
    temp_a1 = *(s32 *)((s8*)(arg0) + 0x58);
    if (((temp_a3 != temp_a2) | (*(s32 *)((s8*)(arg0) + 0x3C) != *(s32 *)((s8*)(arg0) + 0x54)) | (*(s32 *)((s8*)(arg0) + 0x40) != temp_a1)) != 0) {
        *(s32 *)((s8*)(arg1) + 0) = (s32) (temp_a2 - temp_a3);
        *(s32 *)((s8*)(arg1) + 4) = (s32) (*(s32 *)((s8*)(arg0) + 0x54) - *(s32 *)((s8*)(arg0) + 0x3C));
        *(s32 *)((s8*)(arg1) + 8) = (s32) (*(s32 *)((s8*)(arg0) + 0x58) - *(s32 *)((s8*)(arg0) + 0x40));
        func_80093484(arg1, temp_a1, temp_a2, temp_a3);
        *(s32 *)((s8*)(arg1) + 0x10) = (s32) ((s32) *(s32 *)((s8*)(arg1) + 0) >> 3);
        *(s32 *)((s8*)(arg1) + 0x14) = (s32) ((s32) *(s32 *)((s8*)(arg1) + 4) >> 3);
        temp_v1 = *(s32 *)((s8*)(arg1) + 0x10);
        *(s32 *)((s8*)(arg1) + 0x18) = (s32) ((s32) *(s32 *)((s8*)(arg1) + 8) >> 3);
        var_v0 = abs(temp_v1);
        if (var_v0 < 0x40) {
            *(s32 *)((s8*)(arg0) + 0x38) = (s32) *(s32 *)((s8*)(arg0) + 0x50);
        } else {
            *(s32 *)((s8*)(arg0) + 0x38) = (s32) (*(s32 *)((s8*)(arg0) + 0x38) + temp_v1);
        }
        temp_v1_2 = *(s32 *)((s8*)(arg1) + 0x14);
        var_v0_2 = abs(temp_v1_2);
        if (var_v0_2 < 0x40) {
            *(s32 *)((s8*)(arg0) + 0x3C) = (s32) *(s32 *)((s8*)(arg0) + 0x54);
        } else {
            *(s32 *)((s8*)(arg0) + 0x3C) = (s32) (*(s32 *)((s8*)(arg0) + 0x3C) + temp_v1_2);
        }
        temp_a0 = *(s32 *)((s8*)(arg1) + 0x18);
        var_v0_3 = abs(temp_a0);
        if (var_v0_3 < 0x40) {
            *(s32 *)((s8*)(arg0) + 0x40) = (s32) *(s32 *)((s8*)(arg0) + 0x58);
        } else {
            *(s32 *)((s8*)(arg0) + 0x40) = (s32) (*(s32 *)((s8*)(arg0) + 0x40) + temp_a0);
        }
    }
    D_8009BD38 = (s16) ((s32) *(s32 *)((s8*)(arg0) + 0x38) >> 0xC);
    (*(s16*)&D_8009BD3A) = (s16) ((s32) *(s32 *)((s8*)(arg0) + 0x3C) >> 0xC);
    D_8009BD3C = (s16) ((s32) *(s32 *)((s8*)(arg0) + 0x40) >> 0xC);
}
#endif



#ifndef XENO_PC_PORT
void func_80076F54(void *arg0) {
    s32 temp_a0;
    s32 temp_a1;
    s32 var_v0;

    temp_a0 = *(s32 *)((s8*)(arg0) + 0x5C);
    if (temp_a0 != D_8009D3F0) {
        temp_a1 = (s32) (temp_a0 - D_8009D3F0) >> 3;
        var_v0 = abs(temp_a1);
        if (var_v0 < 0x40) {
            D_8009D3F0 = temp_a0;
            return;
        }
        D_8009D3F0 += temp_a1;
    }
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80076FA8);


#ifndef XENO_PC_PORT
s32 func_800771D8(s32 arg0, s32 arg1, s32 arg2) {
    if (arg0 != arg1) {
        if (abs(arg1 - arg0) < abs(arg2)) {
            arg0 = arg1;
        } else {
            arg0 += arg2;
        }
    }
    return arg0;
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80077214);


#ifndef XENO_PC_PORT
void func_80077480(void) {
    func_80084818();
    func_80086124();
    func_80086568();
    func_800866C8();
    func_80074F04();
    func_800750DC();
    func_80088FF4();
    func_80089128();
    func_80097D64();
    HeapFree(D_8009BC38);
    HeapFree(D_8009BCB0);
    HeapFree(D_8009BC3C);
    HeapFree(D_8009BCB4);
    HeapFree(D_8009C180);
    func_800976A0();
    D_8006F94E = 0x11;
    D_8006F954 = 7;
    D_8009BBC4 = 1;
    D_8006F950 = D_8009BD3A;
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007756C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800776E0);


#ifndef XENO_PC_PORT
s32 func_80077954(void) {
    return 1;
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007795C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80077A64);


#ifndef XENO_PC_PORT
void func_80077CC0(void) {
    func_80039FF8();
    func_8003852C(D_8006259C);
    HeapFree(D_8006259C);
    func_80084818();
    func_80086568();
    func_800866C8();
    func_80074F04();
    func_800750DC();
    func_80088FF4();
    func_80089128();
    func_80097D64();
    HeapFree(D_8009BC38);
    HeapFree(D_8009BCB0);
    HeapFree(D_8009BC3C);
    HeapFree(D_8009BCB4);
    HeapFree(D_8009C180);
    func_800976A0();
    D_8006F94E = 0x10E;
    D_8006F954 = 0;
    D_8009BBC4 = 1;
    D_8006F950 = D_8009BD3A;
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80077DC8);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80077E68);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007828C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800783E8);


#ifndef XENO_PC_PORT
s32 func_80078948(void) {
    return 1;
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80078950);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80078A60);


#ifndef XENO_PC_PORT
void func_80078D24(void) {
    func_80039FF8();
    func_8003852C(D_8006259C);
    HeapFree(D_8006259C);
    func_80084818();
    func_80086568();
    func_800866C8();
    func_80074F04();
    func_800750DC();
    func_80088FF4();
    func_80089128();
    func_80097D64();
    HeapFree(D_8009BC38);
    HeapFree(D_8009BCB0);
    HeapFree(D_8009BC3C);
    HeapFree(D_8009BCB4);
    HeapFree(D_8009C180);
    func_800976A0();
    D_8006F94E = 0x110;
    D_8006F954 = 0;
    D_8009BBC4 = 1;
    D_8006F950 = D_8009BD3A;
}
#endif



s32 func_80078E2C(s32 arg0) {
    void *temp_v1;

    D_8009BE0C = 0x78;
    D_8009D3F0 = 0x200000;
    D_8009D144 = 0;
    temp_v1 = D_8009BE24 + (arg0 << 7);
    *(s16 *)((s8*)(temp_v1) + 0x20) = 0x10;
    *(s32 *)((s8*)(temp_v1) + 0x58) = 0;
    *(s32 *)((s8*)(temp_v1) + 0x54) = 0;
    *(s32 *)((s8*)(temp_v1) + 0x50) = 0;
    D_8009BD38 = -0xC0;
    (*(s16*)&D_8009BD3A) = 0x680;
    D_8009BD3C = 0;
    *(s16 *)((s8*)(temp_v1) + 0x22) = 0x40;
    *(s32 *)((s8*)(temp_v1) + 0x74) = 0;
    return 1;
}


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80078EA4);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800794D8);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80079538);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800795E4);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80079778);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007A06C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007A144);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007A1B4);


#ifndef XENO_PC_PORT
s32 func_8007A410(s32 arg0) {
    *(s16 *)((s8*)((D_8009BE24 + (arg0 << 7))) + 0x22) = 0x60;
    return 3;
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007A430);


#ifndef XENO_PC_PORT
s32 func_8007A568(void) {
    return 3;
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007A570);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007A5DC);


#ifndef XENO_PC_PORT
void func_8007A8AC(void) {
    func_80039FF8();
    func_8003852C(D_8006259C);
    HeapFree(D_8006259C);
    func_80084818();
    func_80086568();
    func_800866C8();
    func_80074F04();
    func_800750DC();
    func_80088FF4();
    func_80089128();
    func_80097D64();
    HeapFree(D_8009BC38);
    HeapFree(D_8009BCB0);
    HeapFree(D_8009BC3C);
    HeapFree(D_8009BCB4);
    HeapFree(D_8009C180);
    func_800976A0();
    D_8006F94E = 0x11A;
    D_8006F954 = 0;
    D_8009BBC4 = 1;
    D_8006F950 = D_8009BD3A;
}
#endif



#ifndef XENO_PC_PORT
s32 func_8007A9B4(s32 arg0) {
    void *temp_v1;

    temp_v1 = D_8009BE24 + (arg0 << 7);
    *(s32 *)((s8*)(temp_v1) + 0x50) = 0;
    *(u16 *)((s8*)(temp_v1) + 0x20) = (u16) D_8009A450;
    *(u16 *)((s8*)(temp_v1) + 0x22) = D_8009A46C[*(s32 *)((s8*)(temp_v1) + 0x50)];
    return 1;
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007A9F8);


#ifndef XENO_PC_PORT
s32 func_8007AD34(s32 arg0) {
    D_8009BE0C = 0x78;
    D_8009D3F0 = 0x1C0000;
    D_8009BD38 = 0x40;
    (*(s16*)&D_8009BD3A) = 0x480;
    D_8009BD3C = 0;
    D_8009BE28 = D_8009C5AC;
    D_8009D55C = D_8009C5AC;
    D_8009BE2C = D_8009C5B0;
    D_8009D560 = D_8009C5B0;
    D_8009BE30 = D_8009C5B4;
    D_8009D564 = D_8009C5B4;
    *(s32 *)((s8*)((D_8009BE24 + (arg0 << 7))) + 0x50) = 0x1000;
    return 1;
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007ADD4);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007B200);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007B394);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007B604);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007B798);


#ifndef XENO_PC_PORT
s32 func_8007BA08(void) {
    return 3;
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007BA10);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007BB60);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007BBEC);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007BF50);


#ifndef XENO_PC_PORT
void func_8007C260(void) {
    func_80039FF8();
    func_8003852C(D_8006259C);
    HeapFree(D_8006259C);
    func_80084818();
    func_80086568();
    func_800866C8();
    func_80074F04();
    func_800750DC();
    func_80088FF4();
    func_80089128();
    func_80097D64();
    HeapFree(D_8009BC38);
    HeapFree(D_8009BCB0);
    HeapFree(D_8009BC3C);
    HeapFree(D_8009BCB4);
    HeapFree(D_8009C180);
    func_800976A0();
    D_8006F94E = 0x111;
    D_8006F954 = 2;
    D_8009BBC4 = 1;
    D_8006F950 = D_8009BD3A;
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007C36C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007C3B8);


#ifndef XENO_PC_PORT
s32 func_8007C724(s32 arg0) {
    void *temp_v1;

    D_8009BE0C = 0x78;
    D_8009D3F0 = 0x400000;
    D_8009D144 = 1;
    temp_v1 = D_8009BE24 + (arg0 << 7);
    *(s32 *)((s8*)(temp_v1) + 0x7C) = 0x1000;
    D_8009BD38 = -0x40;
    (*(s16*)&D_8009BD3A) = 0;
    D_8009BD3C = 0;
    D_8009BE28 = D_8009C5AC;
    D_8009D55C = D_8009C5AC;
    D_8009BE2C = D_8009C5B0;
    D_8009D560 = D_8009C5B0;
    D_8009BE30 = D_8009C5B4;
    D_8009D564 = D_8009C5B4;
    *(s32 *)((s8*)(temp_v1) + 0x58) = 0x40;
    *(s32 *)((s8*)(temp_v1) + 0x50) = 0;
    return 1;
}
#endif


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


#ifndef XENO_PC_PORT
s32 func_8007D690(s32 arg0) {
    void *temp_s0;
    void *temp_s1;

    temp_s1 = (*(void **)&D_8009C620);
    temp_s0 = D_8009BE24 + (arg0 << 7);
    *(s32 *)((s8*)(temp_s0) + 0x30) = (s32) (*(s32 *)((s8*)(temp_s0) + 0x30) + *(s32 *)((s8*)(temp_s0) + 0x40));
    func_80093354(temp_s0 + 0x28);
    *(s32 *)((s8*)(temp_s0) + 0x2C) = (s32) (func_80093A5C(*(s32 *)((s8*)(temp_s0) + 0x28), *(s32 *)((s8*)(temp_s0) + 0x30)) - 0x4000);
    *(s32 *)((s8*)(temp_s1) + 0x44C) = (s32) ((s32) *(s32 *)((s8*)(temp_s0) + 0x28) >> 0xC);
    *(s32 *)((s8*)(temp_s1) + 0x450) = (s32) ((s32) *(s32 *)((s8*)(temp_s0) + 0x2C) >> 0xC);
    *(s32 *)((s8*)(temp_s1) + 0x454) = (s32) ((s32) *(s32 *)((s8*)(temp_s0) + 0x30) >> 0xC);
    *(s16 *)0x1F8000A0 = (s16) ((s32) *(s32 *)((s8*)(temp_s0) + 0x28) >> 0xC);
    *(s16 *)0x1F8000A2 = (s16) ((s32) *(s32 *)((s8*)(temp_s0) + 0x2C) >> 0xC);
    *(s16 *)0x1F8000A4 = (s16) ((s32) *(s32 *)((s8*)(temp_s0) + 0x30) >> 0xC);
    func_80089160(0x19, 0x1F8000A0, 0);
    return 1;
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007D774);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007D7FC);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007D918);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007DCE0);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007DE14);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007DE98);


#ifndef XENO_PC_PORT
s32 func_8007E450(s32 arg0) {
    void *temp_v1;

    D_8009D144 = 0;
    temp_v1 = D_8009BE24 + (arg0 << 7);
    *(s32 *)((s8*)(temp_v1) + 0x7C) = 0x1000;
    D_8009BE0C = 0x78;
    D_8009BE28 = D_8009C5AC;
    D_8009D55C = D_8009C5AC;
    D_8009BE2C = D_8009C5B0;
    D_8009D560 = D_8009C5B0;
    D_8009BE30 = D_8009C5B4;
    D_8009D564 = D_8009C5B4;
    *(s16 *)((s8*)(temp_v1) + 4) = 1;
    *(s32 *)((s8*)(temp_v1) + 0x58) = 0x40;
    *(s16 *)((s8*)(temp_v1) + 0x20) = 0;
    *(s32 *)((s8*)(temp_v1) + 0x50) = 0;
    return 1;
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007E4E4);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007EBBC);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007ECA4);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007EE34);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007F8AC);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007F968);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007FC8C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007FD30);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8007FF70);


#ifndef XENO_PC_PORT
void func_80080218(void) {
    func_80039FF8();
    func_8003852C(D_8006259C);
    HeapFree(D_8006259C);
    func_80084818();
    func_80086124();
    func_80086568();
    func_800866C8();
    func_80074F04();
    func_800750DC();
    func_80088FF4();
    func_80089128();
    func_80097D64();
    HeapFree(D_8009BC38);
    HeapFree(D_8009BCB0);
    HeapFree(D_8009BC3C);
    HeapFree(D_8009BCB4);
    HeapFree(D_8009C180);
    func_800976A0();
    D_8006F94E = 0x84;
    D_8006F954 = 2;
    D_8009BBC4 = 1;
    D_8006F950 = D_8009BD3A;
}
#endif



#ifndef XENO_PC_PORT
s32 func_8008032C(s32 arg0) {
    void *temp_v1;

    temp_v1 = D_8009BE24 + (arg0 << 7);
    *(s32 *)((s8*)(temp_v1) + 0x50) = 0;
    *(u16 *)((s8*)(temp_v1) + 0x20) = (u16) D_8009A698;
    *(u16 *)((s8*)(temp_v1) + 0x22) = D_8009A6AC[*(s32 *)((s8*)(temp_v1) + 0x50)];
    return 1;
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80080370);


#ifndef XENO_PC_PORT
s32 func_80080578(s32 arg0) {
    void *temp_v1;

    D_8009D144 = 0;
    temp_v1 = D_8009BE24 + (arg0 << 7);
    *(s32 *)((s8*)(temp_v1) + 0x7C) = 0x1000;
    D_8009BE0C = 0x78;
    D_8009BE28 = D_8009C5AC;
    D_8009D55C = D_8009C5AC;
    D_8009BE2C = D_8009C5B0;
    D_8009D560 = D_8009C5B0;
    D_8009BE30 = D_8009C5B4;
    D_8009D564 = D_8009C5B4;
    *(s16 *)((s8*)(temp_v1) + 4) = 1;
    *(s16 *)((s8*)(temp_v1) + 0x20) = 0;
    return 1;
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80080600);


s32 func_80080900(s32 arg0) {
    void *temp_v1;

    temp_v1 = D_8009BE24 + (arg0 << 7);
    *(s32 *)((s8*)(temp_v1) + 0x28) = (s32) D_8009C5AC;
    *(s32 *)((s8*)(temp_v1) + 0x2C) = (s32) D_8009C5B0;
    *(s32 *)((s8*)(temp_v1) + 0x30) = (s32) D_8009C5B4;
    return 1;
}



#ifndef XENO_PC_PORT
s32 func_80080944(s32 arg0) {
    void *temp_a2;

    temp_a2 = D_8009BE24 + (arg0 << 7);
    if (*(s16 *)((s8*)(temp_a2) + 4) == 1) {
        *(s16 *)((s8*)(temp_a2) + 4) = 0;
        *(s16 *)0x1F8000A0 = (s16) ((s32) *(s32 *)((s8*)(temp_a2) + 0x28) >> 0xC);
        *(s16 *)0x1F8000A2 = (s16) ((s32) *(s32 *)((s8*)(temp_a2) + 0x2C) >> 0xC);
        *(s16 *)0x1F8000A4 = (s16) ((s32) *(s32 *)((s8*)(temp_a2) + 0x30) >> 0xC);
        func_80089160(0x28, 0x1F8000A0, 0);
        func_80089160(0x29, 0x1F8000A0, 0);
        func_80089160(0x2A, 0x1F8000A0, 0);
    }
    return 1;
}
#endif



#ifndef XENO_PC_PORT
void func_800809EC(u8 *arg0, s32 arg1, s8 arg2, s8 arg3, s32 arg4) {
    u8 pad[4];
    s32 i = 0;

    if (arg1 > 0) {
        do {
            i++;
            arg0[4] = arg2;
            arg0[5] = arg3;
            arg0[6] = arg4;
            arg0 += 0x28;
        } while (i < arg1);
    }
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80080A28);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80080AC4);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80080D00);


#ifndef XENO_PC_PORT
void func_8008106C(void) {
    func_80039FF8();
    func_8003852C(D_8006259C);
    HeapFree(D_8006259C);
    func_80084818();
    func_80086568();
    func_800866C8();
    func_80074F04();
    func_800750DC();
    func_80088FF4();
    func_80089128();
    func_80097D64();
    HeapFree(D_8009BC38);
    HeapFree(D_8009BCB0);
    HeapFree(D_8009BC3C);
    HeapFree(D_8009BCB4);
    HeapFree(D_8009C180);
    func_800976A0();
    D_8006F94E = 0x1FA;
    D_8006F954 = 0;
    D_8009BBC4 = 1;
    D_8006F950 = D_8009BD3A;
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80081174);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800811C0);


#ifndef XENO_PC_PORT
s32 func_800813E8(s32 arg0) {
    void *temp_v1;

    D_8009D144 = 0;
    temp_v1 = D_8009BE24 + (arg0 << 7);
    *(s32 *)((s8*)(temp_v1) + 0x7C) = 0x1000;
    D_8009BE0C = 0x78;
    D_8009BE28 = D_8009C5AC;
    D_8009D55C = D_8009C5AC;
    D_8009BE2C = D_8009C5B0;
    D_8009D560 = D_8009C5B0;
    D_8009BE30 = D_8009C5B4;
    D_8009D564 = D_8009C5B4;
    *(s16 *)((s8*)(temp_v1) + 4) = 1;
    *(s16 *)((s8*)(temp_v1) + 0x20) = 0;
    return 1;
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80081470);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800816DC);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800817A0);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80081868);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800819C8);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80081B24);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80081C3C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80081D80);


#ifndef XENO_PC_PORT
s32 func_80081FB4(s32 arg0) {
    void *temp_v0;

    temp_v0 = D_8009BE24 + (arg0 << 7);
    *(s16 *)((s8*)(temp_v0) + 0x20) = 0;
    *(s32 *)((s8*)(temp_v0) + 0x50) = 1;
    return 1;
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80081FD8);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80082324);


#ifndef XENO_PC_PORT
void func_800826B4(void) {
    func_80039FF8();
    func_8003852C(D_8006259C);
    HeapFree(D_8006259C);
    func_80084818();
    func_80086124();
    func_80086568();
    func_800866C8();
    func_80074F04();
    func_800750DC();
    func_80088FF4();
    func_80089128();
    func_80097D64();
    HeapFree(D_8009BC38);
    HeapFree(D_8009BCB0);
    HeapFree(D_8009BC3C);
    HeapFree(D_8009BCB4);
    HeapFree(D_8009C180);
    func_800976A0();
    D_8006F94E = 0x269;
    D_8006F954 = 2;
    D_8009BBC4 = 1;
    D_8006F950 = D_8009BD3A;
}
#endif



#ifndef XENO_PC_PORT
s32 func_800827C8(s32 arg0) {
    *(s32 **)((s8*)(((arg0 << 7) + D_8009BE24)) + 0x50) = &D_8009A758;
    return 1;
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800827EC);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800828DC);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80082F64);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80083108);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800831D8);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80083214);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80083264);


s32 func_800834D0(void) {
    return 1;
}



#ifndef XENO_PC_PORT
s32 func_800834D8(s32 arg0) {
    void *temp_a0;

    temp_a0 = D_8009BE24 + (arg0 << 7);
    if (*(s16 *)((s8*)(temp_a0) + 4) == 1) {
        *(s16 *)((s8*)(temp_a0) + 4) = 0;
        DrawSync(0);
        Vsync(0);
        func_80097D64();
        func_80097BC0(&D_8009C5AC);
        do {
            func_800967E4();
            Vsync(0);
        } while (func_80096668() > 0);
    }
    return 1;
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008355C);


#ifndef XENO_PC_PORT
void func_800837DC(void) {
    func_80039FF8();
    func_8003852C(D_8006259C);
    HeapFree(D_8006259C);
    func_80084818();
    func_80086568();
    func_800866C8();
    func_80074F04();
    func_800750DC();
    func_80088FF4();
    func_80089128();
    func_80097D64();
    HeapFree(D_8009BC38);
    HeapFree(D_8009BCB0);
    HeapFree(D_8009BC3C);
    HeapFree(D_8009BCB4);
    HeapFree(D_8009C180);
    func_800976A0();
    D_8006F94E = 0x269;
    D_8006F954 = 4;
    D_8009BBC4 = 1;
    D_8006F950 = D_8009BD3A;
}
#endif



#ifndef XENO_PC_PORT
s32 func_800838E8(s32 arg0) {
    *(s32 **)((s8*)(((arg0 << 7) + D_8009BE24)) + 0x50) = &D_8009AC60;
    return 1;
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008390C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80083A00);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80083FE4);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80084068);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008440C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80084580);


#ifndef XENO_PC_PORT
void func_80084818(void) {
    s32 temp_a0;
    s32 var_s0;
    s32 var_s1;
    u8 pad[4];

    var_s1 = 0;
    if (D_8009D7E0 > 0) {
        var_s0 = 0;
        do {
            var_s1 += 1;
            HeapFree(*(s32 *)((s8*)((var_s0 + D_8009C620)) + 0x48));
            temp_a0 = *(s32 *)((s8*)((var_s0 + D_8009C620)) + 0x40);
            var_s0 += 0x54;
            func_8002CBBC(temp_a0);
        } while (var_s1 < D_8009D7E0);
    }
    HeapFree(D_8009C620);
}
#endif



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


s32 func_80087710(s32 arg0) {
    void *temp_v0;

    temp_v0 = D_8009BE24 + (arg0 << 7);
    *(s32 *)((s8*)(temp_v0) + 0x50) = 0;
    *(s32 *)((s8*)(temp_v0) + 0x54) = 8;
    return 1;
}



#ifndef XENO_PC_PORT
s32 func_80087734(s32 arg0) {
    u16 temp_v0;
    void *temp_a1;
    void *temp_s0;

    temp_a1 = D_8009BE24 + (arg0 << 7);
    temp_s0 = (*(void **)&D_8009C620);
    *(s32 *)((s8*)(temp_a1) + 0x50) = (s32) ((*(s32 *)((s8*)(temp_a1) + 0x50) + *(s32 *)((s8*)(temp_a1) + 0x54)) & 0xFFF);
    *(s16 *)0x1F8000A8 = 0;
    *(s16 *)0x1F8000A0 = 0;
    *(u16 *)0x1F8000A2 = *(u16 *)((s8*)(temp_s0) + 0x40A);
    *(u16 *)0x1F8000AA = *(u16 *)((s8*)(temp_s0) + 0x4B2);
    temp_v0 = (u16) *(s32 *)((s8*)(temp_a1) + 0x50);
    *(u16 *)0x1F8000AC = temp_v0;
    *(u16 *)0x1F8000A4 = temp_v0;
    RotMatrix(0x1F8000A0, temp_s0 + 0x410);
    RotMatrix(0x1F8000A8, temp_s0 + 0x4B8);
    return 1;
}
#endif



#ifndef XENO_PC_PORT
s32 func_800877E0(s32 arg0) {
    void *temp_v0;

    temp_v0 = D_8009BE24 + (arg0 << 7);
    *(s32 *)((s8*)(temp_v0) + 0x50) = 0;
    *(s32 *)((s8*)(temp_v0) + 0x54) = 8;
    return 1;
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80087804);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80087904);


#ifndef XENO_PC_PORT
s32 func_800879A8(s32 arg0) {
    void *temp_v0;

    temp_v0 = D_8009BE24 + (arg0 << 7);
    *(s32 *)((s8*)(temp_v0) + 0x50) = 0;
    *(s32 *)((s8*)(temp_v0) + 0x54) = 0x10;
    func_800879E0(arg0);
    return 1;
}
#endif



#ifndef XENO_PC_PORT
s32 func_800879E0(s32 arg0) {
    s32 temp_v1;
    void *temp_a0;
    void *temp_s0;
    u8 pad[4];

    temp_v1 = D_8009C610 * 2;
    temp_a0 = D_8009C620 + (D_8009B64C[temp_v1] * 0x54);
    temp_s0 = D_8009C620 + (D_8009B64E[temp_v1] * 0x54);
    func_80087904(temp_a0, *(s32 *)((s8*)(temp_a0) + 0x48), *(u16 *)((s8*)(*(void **)((s8*)(temp_a0) + 0x40)) + 4), 3);
    func_80087904(temp_s0, *(s32 *)((s8*)(temp_s0) + 0x48), *(u16 *)((s8*)(*(void **)((s8*)(temp_s0) + 0x40)) + 4), 3);
    return 1;
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80087A8C);


#ifndef XENO_PC_PORT
void func_80087B84(void *arg0, void *arg1, void *arg2) {
    *(s32 *)((s8*)(arg1) + 8) = 0;
    *(s32 *)((s8*)(arg1) + 0) = 0;
    *(s32 *)((s8*)(arg1) + 4) = 0x1000;
    OuterProduct12(arg1, arg0, arg1);
    VectorNormal(arg1, arg1);
    *(s16 *)((s8*)(arg2) + 0) = (s16) *(s32 *)((s8*)(arg1) + 0);
    *(s16 *)((s8*)(arg2) + 2) = (s16) *(s32 *)((s8*)(arg1) + 4);
    *(s16 *)((s8*)(arg2) + 4) = (s16) *(s32 *)((s8*)(arg1) + 8);
    OuterProduct12(arg0, arg1, arg1);
    VectorNormal(arg1, arg1);
    *(s16 *)((s8*)(arg2) + 6) = (s16) *(s32 *)((s8*)(arg1) + 0);
    *(s16 *)((s8*)(arg2) + 8) = (s16) *(s32 *)((s8*)(arg1) + 4);
    *(s16 *)((s8*)(arg2) + 0xA) = (s16) *(s32 *)((s8*)(arg1) + 8);
    *(s16 *)((s8*)(arg2) + 0xC) = (s16) *(s32 *)((s8*)(arg0) + 0);
    *(s16 *)((s8*)(arg2) + 0xE) = (s16) *(s32 *)((s8*)(arg0) + 4);
    *(s16 *)((s8*)(arg2) + 0x10) = (s16) *(s32 *)((s8*)(arg0) + 8);
    TransposeMatrix(arg2, arg2);
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80087C6C);


#ifndef XENO_PC_PORT
s32 func_80087F60(void) {
    u16 temp_s0;

    temp_s0 = D_8009B674[D_8009C610];
    func_800848B4(temp_s0, temp_s0 - 4);
    func_800848B4(temp_s0, temp_s0 - 3);
    func_800848B4(temp_s0, temp_s0 - 2);
    func_800848B4(temp_s0, temp_s0 - 1);
    return 1;
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80087FD0);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80088570);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008868C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80088720);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80088B40);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80088C90);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80088D00);


#ifndef XENO_PC_PORT
s32 func_80088D64(s32 arg0) {
    void *temp_a1;
    void *temp_a2;

    temp_a2 = D_8009C620 + (D_8009B69C[D_8009C610] * 0x54);
    temp_a1 = D_8009BE24 + (arg0 << 7);
    *(s32 *)((s8*)(temp_a1) + 0x28) = (s32) (*(s32 *)((s8*)(temp_a2) + 8) << 0xC);
    *(s32 *)((s8*)(temp_a1) + 0x2C) = (s32) ((*(s32 *)((s8*)(temp_a2) + 0xC) << 0xC) + 0x40000);
    *(s32 *)((s8*)(temp_a1) + 0x30) = (s32) (*(s32 *)((s8*)(temp_a2) + 0x10) << 0xC);
    return 1;
}
#endif



#ifndef XENO_PC_PORT
s32 func_80088DE4(s32 arg0) {
    func_8008BFD4(arg0, D_8009BE24 + (arg0 << 7) + 0x28, 0x68, 0x60);
    return 1;
}
#endif



#ifndef XENO_PC_PORT
s32 func_80088E1C(s32 arg0) {
    void *temp_v1;

    temp_v1 = D_8009BE24 + (arg0 << 7);
    *(s32 *)((s8*)(temp_v1) + 0x28) = (s32) (*(s32 *)((s8*)((*(void **)&D_8009C620)) + 0x18A4) << 0xC);
    *(s32 *)((s8*)(temp_v1) + 0x2C) = (s32) (*(s32 *)((s8*)((*(void **)&D_8009C620)) + 0x18A8) << 0xC);
    *(s32 *)((s8*)(temp_v1) + 0x30) = (s32) (*(s32 *)((s8*)((*(void **)&D_8009C620)) + 0x18AC) << 0xC);
    return 1;
}
#endif



#ifndef XENO_PC_PORT
s32 func_80088E68(s32 arg0) {
    func_8008BFD4(arg0, D_8009BE24 + (arg0 << 7) + 0x28, 0x10C, 0x1A6);
    return 1;
}
#endif



#ifndef XENO_PC_PORT
s32 func_80088EA0(s32 arg0) {
    void *temp_a1;
    void *temp_v1;

    temp_a1 = D_8009C620 + (D_8009B6B0[D_8009C610] * 0x54);
    temp_v1 = D_8009BE24 + (arg0 << 7);
    *(s32 *)((s8*)(temp_v1) + 0x28) = (s32) (*(s32 *)((s8*)(temp_a1) + 8) << 0xC);
    *(s32 *)((s8*)(temp_v1) + 0x2C) = (s32) (*(s32 *)((s8*)(temp_a1) + 0xC) << 0xC);
    *(s32 *)((s8*)(temp_v1) + 0x30) = (s32) (*(s32 *)((s8*)(temp_a1) + 0x10) << 0xC);
    return 1;
}
#endif



#ifndef XENO_PC_PORT
s32 func_80088F1C(s32 arg0) {
    func_8008BFD4(arg0, D_8009BE24 + (arg0 << 7) + 0x28, 0xC9, 0x392);
    return 1;
}
#endif



#ifndef XENO_PC_PORT
s32 func_80088F54(void) {
    return 3;
}
#endif



#ifndef XENO_PC_PORT
s32 func_80088F5C(void) {
    return 3;
}
#endif


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


#ifndef XENO_PC_PORT
void func_800894C8(s32 arg0) {
    s32 var_a1;
    u8 *var_v1;

    var_a1 = 7;
    var_v1 = D_8009BCC0 + (arg0 * 0x2A0) + 0x4F;
    do {
        var_a1 -= 1;
        *var_v1 &= 0x7F;
        var_v1 += 0x54;
    } while (var_a1 != -1);
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80089514);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80089580);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80089748);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80089C78);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008A2C8);


#ifndef XENO_PC_PORT
s32 func_8008A52C(s32 arg0) {
    void *temp_a1;
    void *temp_s0;
    void *temp_v0;

    temp_s0 = D_8009BE24__a_s32[0] + (arg0 << 7);
    temp_v0 = func_80024524(D_8009CD34[0], 0x100, 0x1E0, 0x140, 0x100, 0x40);
    *(void **)((s8*)(temp_s0) + 0x4C) = temp_v0;
    func_800245D8(temp_v0, 0);
    SpriteSetScale(*(void **)((s8*)(temp_s0) + 0x4C), 0x1800);
    temp_a1 = *(void **)((s8*)(temp_s0) + 0x4C);
    *(s32 *)((s8*)(temp_a1) + 0x3C) = (s32) (*(s32 *)((s8*)(temp_a1) + 0x3C) & ~4);
    return 1;
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008A5B8);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008A72C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008B2BC);


#ifndef XENO_PC_PORT
s32 func_8008B498(s32 arg0) {
    s32 var_s1;
    void *temp_a0;
    void *temp_s0;
    void *temp_v0;

    temp_s0 = D_8009BE24 + (arg0 << 7);
    var_s1 = 1;
    if (*(u8 *)((s8*)(&g_GameState) + 0x1D35) != 0xFF) {
        temp_v0 = func_80024524(D_8009CD38, 0x110, 0x1E0, 0x150, 0x100, 0x40);
        *(void **)((s8*)(temp_s0) + 0x4C) = temp_v0;
        func_800245D8(temp_v0, 0);
        SpriteSetScale(*(void **)((s8*)(temp_s0) + 0x4C), 0x1800);
        temp_a0 = *(void **)((s8*)(temp_s0) + 0x4C);
        *(s32 *)((s8*)(temp_a0) + 0x3C) = (s32) (*(s32 *)((s8*)(temp_a0) + 0x3C) & ~4);
    } else {
        var_s1 = 3;
    }
    return var_s1;
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008B54C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008B644);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008BB40);


#ifndef XENO_PC_PORT
s32 func_8008BD1C(s32 arg0) {
    s32 var_s1;
    void *temp_a0;
    void *temp_s0;
    void *temp_v0;

    temp_s0 = D_8009BE24 + (arg0 << 7);
    var_s1 = 1;
    if (*(u8 *)((s8*)(&g_GameState) + 0x1D36) != 0xFF) {
        temp_v0 = func_80024524(D_8009CD3C, 0x120, 0x1E0, 0x160, 0x100, 0x40);
        *(void **)((s8*)(temp_s0) + 0x4C) = temp_v0;
        func_800245D8(temp_v0, 0);
        SpriteSetScale(*(void **)((s8*)(temp_s0) + 0x4C), 0x1800);
        temp_a0 = *(void **)((s8*)(temp_s0) + 0x4C);
        *(s32 *)((s8*)(temp_a0) + 0x3C) = (s32) (*(s32 *)((s8*)(temp_a0) + 0x3C) & ~4);
    } else {
        var_s1 = 3;
    }
    return var_s1;
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008BDD0);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008BEC8);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008BFD4);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008C040);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008C1DC);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008C28C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008C364);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008C530);


#ifndef XENO_PC_PORT
s32 func_8008C6EC(s32 arg0) {
    s32 temp_a0;
    void *temp_s0;

    temp_s0 = D_8009BE24 + (arg0 << 7);
    temp_a0 = func_8008C364(temp_s0, 0);
    if (D_8009BE10 > 0) {
        if (D_8009BE10 >= 4) {
            if (D_8009BE10 < 8) {
                *(s16 *)((s8*)(temp_s0) + 0x24) = 1;
            }
        }
    }
    return temp_a0;
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008C75C);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008C844);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008D3F0);


#ifndef XENO_PC_PORT
s32 func_8008D520(s32 arg0) {
    s32 temp_a0;
    void *temp_s0;

    temp_s0 = D_8009BE24 + (arg0 << 7);
    temp_a0 = func_8008C364(temp_s0, 1);
    if (D_8009BE10 > 0) {
        if (D_8009BE10 >= 4) {
            if (D_8009BE10 < 8) {
                *(s16 *)((s8*)(temp_s0) + 0x24) = 1;
            }
        }
    }
    return temp_a0;
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008D590);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008D678);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_8008DD6C);


#ifndef XENO_PC_PORT
s32 func_8008DE9C(s32 arg0) {
    s32 temp_a0;
    void *temp_s0;

    temp_s0 = D_8009BE24 + (arg0 << 7);
    temp_a0 = func_8008C364(temp_s0, 2);
    if (D_8009BE10 > 0) {
        if (D_8009BE10 >= 4) {
            if (D_8009BE10 < 8) {
                *(s16 *)((s8*)(temp_s0) + 0x24) = 1;
            }
        }
    }
    return temp_a0;
}
#endif


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


#ifndef XENO_PC_PORT
s32 func_800907C4(void) {
    func_800848B4(0, 2);
    func_800848B4(0, 3);
    return 1;
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800907F4);


#ifndef XENO_PC_PORT
void func_80090A18(void) {
    if ((D_8009CD4C & 1) && (D_8009CD4C & 2)) {
        D_8009CEC0 = 1;
    } else {
        D_8009CEC0 = 0;
    }
    D_8009BD34 = (D_8009CEC0 ^ D_8009C7E8) & D_8009CEC0;
    D_8009C7E8 = D_8009CEC0;
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80090A84);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80090C68);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80090E14);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80090FB4);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80091430);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800914D0);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80091B54);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80091C18);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80091FF8);


#ifndef XENO_PC_PORT
s32 func_80092234(s32 arg0) {
    void *temp_a0;

    temp_a0 = D_8009BE24 + (arg0 << 7);
    if (D_8009BE10 > 0) {
        if (D_8009BE10 < 6) {
            D_8009BE0C = 0x8C;
        } else if (D_8009BE10 < 8) {
            D_8009BE0C = 0x78;
            *(s16 *)((s8*)(temp_a0) + 0x20) = 1;
        }
    }
    *(s32 *)((s8*)(temp_a0) + 0x50) = (s32) (D_8009BE0C << 0xC);
    return 1;
}
#endif


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


#ifndef XENO_PC_PORT
s32 func_80094154(void *arg0, void *arg1) {
    s32 temp_lo;
    s32 temp_v0;
    s32 temp_v0_2;
    s32 var_v0;
    s32 var_v0_2;

    var_v0 = abs(*(s32 *)((s8*)(arg0) + 0) - *(s32 *)((s8*)(arg1) + 0));
    temp_v0 = var_v0 >> 0xC;
    temp_lo = temp_v0 * temp_v0;
    var_v0_2 = abs(*(s32 *)((s8*)(arg0) + 8) - *(s32 *)((s8*)(arg1) + 8));
    temp_v0_2 = var_v0_2 >> 0xC;
    return SquareRoot0(temp_lo + (temp_v0_2 * temp_v0_2));
}
#endif



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


#ifndef XENO_PC_PORT
s32 func_80096328(void) {
    s32 *temp_s0;

    temp_s0 = D_8009BE08 + (D_8009BE44 * 0x420);
    if ((*temp_s0 != 0) && (D_8009D788[D_8009BE44] == 0)) {
        func_800963E4(temp_s0);
        D_8009D808 = 0;
        D_8009D788[D_8009BE44] = temp_s0;
        D_8009BE44 = (D_8009BE44 + 1) & 0xF;
        return 0;
    }
    D_8009D808 = 0;
    return -1;
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800963E4);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800964B0);


#ifndef XENO_PC_PORT
s32 func_800965A4(void) {
    s32 *temp_s0;

    temp_s0 = D_8009D3C0 + (D_8009BE44 * 0x580);
    if ((*temp_s0 != 0) && (D_8009C624[D_8009BE44] == 0)) {
        func_800964B0(temp_s0);
        D_8009D808 = 0;
        D_8009C624[D_8009BE44] = temp_s0;
        D_8009BE44 = (D_8009BE44 + 1) & 0xF;
        return 0;
    }
    D_8009D808 = 0;
    return -1;
}
#endif



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


#ifndef XENO_PC_PORT
void func_8009766C(void) {
    D_8009BE24 = HeapAlloc(0x2000, 0);
    func_800976C8();
}
#endif



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



#ifndef XENO_PC_PORT
s32 func_80097770(s32 arg0, s32 arg1) {
    void *temp_a0;

    temp_a0 = D_8009BE24 + (arg0 << 7);
    if (*(s16 *)((s8*)(temp_a0) + 4) != 0) {
        return 0;
    }
    *(s16 *)((s8*)(temp_a0) + 0) = 1;
    *(s16 *)((s8*)(temp_a0) + 4) = arg1;
    return 1;
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800977A8);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800977C4);


#ifndef XENO_PC_PORT
void func_800977E0(s32 arg0, s16 arg1) {
    void *temp_v0;

    temp_v0 = D_8009BE24 + (arg0 << 7);
    *(s16 *)((s8*)(temp_v0) + 0) = 2;
    *(s16 *)((s8*)(temp_v0) + 2) = arg1;
}
#endif


INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80097800);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800978FC);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_800979C8);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80097BC0);

INCLUDE_ASM("asm/world_map/nonmatchings/main", func_80097CB8);


#ifndef XENO_PC_PORT
void func_80097D64(void) {
    s32 *var_s0;
    s32 temp_a0;
    s32 var_s1;

    var_s1 = 0;
    var_s0 = D_8009C184;
    do {
        temp_a0 = *var_s0;
        if (temp_a0 != 0) {
            HeapFree(temp_a0);
        }
        var_s1 += 1;
        var_s0 += 1;
    } while (var_s1 < 0x100);
}
#endif


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
