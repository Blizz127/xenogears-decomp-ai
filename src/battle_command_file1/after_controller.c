/* battle_command_file1 after_controller: retail functions in retail order (asm/battle_command_file1/after_controller.s).
 * Functions still listed as INCLUDE_ASM are not byte-matching C yet. The whole
 * file is retail-build only: the port runs this overlay from its own bindings. */
#include "common.h"
#include "system/memory.h"

#ifndef XENO_PC_PORT
void func_800883AC(s32);
void func_80088490(s32);
void func_800BAF48(s32);
extern void *D_800C3EA4;
extern void *D_800C3EAC;
extern s8 D_800C3EB8;
extern s8 D_800CCD88;
extern u8 D_800CCE42;
extern s8 D_800D32A1;
extern s32 g_GameState;
s32 func_battle_command_file1_801E80F0(void);

s32 func_battle_command_file1_801E57F8(s32, s32, s32, s32);
s32 func_battle_command_file1_801E9AD4(s32);

extern s8 D_800C3E4C;
extern s8 D_800C3D44;
extern void *D_800D3278;
extern s8 D_800D2D50;
extern s8 D_800C48EA;
extern s8 D_800D2FC4;
extern s8 D_800C3D5C;
s32 func_8003A89C(s32, s32, s32);
extern s32 D_800C3E54;
s32 func_800245D8(s32);

INCLUDE_ASM("asm/battle_command_file1/nonmatchings/after_controller", xeno_battle_command_file1_wrapper_71D4);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/after_controller", xeno_battle_command_file1_wrapper_7230);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/after_controller", func_battle_command_file1_801E7278);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/after_controller", func_battle_command_file1_801E7314);

s32 func_battle_command_file1_801E7358(void) {
    D_800C3E4C = 2;
    return 1;
}


s32 func_battle_command_file1_801E736C(void) {
    D_800C3E4C = 1;
    return 1;
}

INCLUDE_ASM("asm/battle_command_file1/nonmatchings/after_controller", func_battle_command_file1_801E7380);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/after_controller", func_battle_command_file1_801E73D4);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/after_controller", func_battle_command_file1_801E7424);

s32 func_battle_command_file1_801E746C(void) {
    D_800C3D44 = 1;
    *(s8 *)((u8*)D_800D3278 + 0x800) = 1;
    return 1;
}


s32 func_battle_command_file1_801E748C(void) {
    D_800D2D50 = 1;
    return 1;
}


s32 func_battle_command_file1_801E74A0(void) {
    *(s8 *)((u8*)D_800D3278 + 0x801) = 2;
    return 1;
}


s32 func_battle_command_file1_801E74B8(void) {
    D_800D2FC4 = 1;
    *(s8 *)((u8*)D_800D3278 + 0x800) = 1;
    D_800C48EA = 1;
    return 1;
}

INCLUDE_ASM("asm/battle_command_file1/nonmatchings/after_controller", func_battle_command_file1_801E74E0);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/after_controller", func_battle_command_file1_801E75F0);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/after_controller", func_battle_command_file1_801E7660);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/after_controller", func_battle_command_file1_801E7684);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/after_controller", func_battle_command_file1_801E7700);

s32 func_battle_command_file1_801E775C(void) {
    D_800C3D5C = 1;
    return 1;
}

INCLUDE_ASM("asm/battle_command_file1/nonmatchings/after_controller", func_battle_command_file1_801E7770);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/after_controller", func_battle_command_file1_801E77E4);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/after_controller", func_battle_command_file1_801E786C);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/after_controller", func_battle_command_file1_801E78A8);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/after_controller", func_battle_command_file1_801E7914);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/after_controller", func_battle_command_file1_801E79E0);

void func_battle_command_file1_801E7A5C(s32 arg0, s32 arg1) {
    s32 temp_s0;
    void *temp_a0;

    func_battle_command_file1_801E57F8(arg1, 1, 0, 1);
    temp_s0 = ((*(u8 *)((s8*)(D_800D3278) + 0x380) + 0xD) & 0xFF) * 0x38;
    temp_a0 = D_800D3278 + temp_s0;
    if (*(u8 *)((s8*)(temp_a0) + 0x35) != 0) {
        func_battle_command_file1_801E9AD4(*(s32 *)((s8*)(temp_a0) + 0x30));
        HeapFree(*(s32 *)((s8*)((D_800D3278 + temp_s0)) + 0x2C));
        *(s8 *)((s8*)((D_800D3278 + temp_s0)) + 0x35) = 0;
    }
}

INCLUDE_ASM("asm/battle_command_file1/nonmatchings/after_controller", func_battle_command_file1_801E7B08);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/after_controller", func_battle_command_file1_801E7B2C);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/after_controller", func_battle_command_file1_801E7B58);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/after_controller", func_battle_command_file1_801E7C0C);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/after_controller", func_battle_command_file1_801E7CD0);

void func_battle_command_file1_801E7DE4(s32 arg0, s32 arg1) {
    func_8003A89C(D_800C3E54, arg0, arg1);
}

INCLUDE_ASM("asm/battle_command_file1/nonmatchings/after_controller", func_battle_command_file1_801E7E14);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/after_controller", func_battle_command_file1_801E7E5C);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/after_controller", func_battle_command_file1_801E7EA4);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/after_controller", func_battle_command_file1_801E7F08);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/after_controller", func_battle_command_file1_801E7F70);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/after_controller", func_battle_command_file1_801E7FF4);

s32 func_battle_command_file1_801E8074(void) {
    return 1;
}

INCLUDE_ASM("asm/battle_command_file1/nonmatchings/after_controller", func_battle_command_file1_801E807C);

s32 func_battle_command_file1_801E80E8(void) {
    return 1;
}


s32 func_battle_command_file1_801E80F0(void) {
    D_800CCD88 = 0;
    *(s8 *)((s8*)(&g_GameState) + 0x30C) = 0;
    func_80088490(0);
    func_800BAF48(0);
    *(s8 *)((s8*)(D_800C3EAC) + 0x2EB) = 1;
    D_800CCE42 |= 0x80;
    func_800883AC(0);
    D_800D32A1 = 2;
    *(s8 *)((s8*)(D_800C3EA4) + 0x853D) = 2;
    D_800C3EB8 = 1;
    return 1;
}

INCLUDE_ASM("asm/battle_command_file1/nonmatchings/after_controller", func_battle_command_file1_801E818C);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/after_controller", func_battle_command_file1_801E81EC);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/after_controller", func_battle_command_file1_801E823C);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/after_controller", func_battle_command_file1_801E828C);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/after_controller", func_battle_command_file1_801E82DC);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/after_controller", func_battle_command_file1_801E83C0);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/after_controller", func_battle_command_file1_801E84A4);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/after_controller", func_battle_command_file1_801E8600);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/after_controller", func_battle_command_file1_801E86AC);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/after_controller", func_battle_command_file1_801E86D0);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/after_controller", func_battle_command_file1_801E86F4);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/after_controller", func_battle_command_file1_801E8718);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/after_controller", func_battle_command_file1_801E8750);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/after_controller", func_battle_command_file1_801E879C);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/after_controller", func_battle_command_file1_801E93E8);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/after_controller", func_battle_command_file1_801E9430);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/after_controller", func_battle_command_file1_801E950C);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/after_controller", func_battle_command_file1_801E9550);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/after_controller", func_battle_command_file1_801E958C);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/after_controller", func_battle_command_file1_801E95B0);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/after_controller", func_battle_command_file1_801E95E4);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/after_controller", func_battle_command_file1_801E9694);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/after_controller", func_battle_command_file1_801E9700);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/after_controller", func_battle_command_file1_801E9760);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/after_controller", func_battle_command_file1_801E9894);

void func_battle_command_file1_801E9958(s32 arg0) {
    func_800245D8(arg0 + 0x38);
}

INCLUDE_ASM("asm/battle_command_file1/nonmatchings/after_controller", func_battle_command_file1_801E9978);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/after_controller", func_battle_command_file1_801E9AD4);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/after_controller", func_battle_command_file1_801E9B2C);
#endif
