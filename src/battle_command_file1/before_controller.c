/* battle_command_file1 before_controller: retail functions in retail order (asm/battle_command_file1/before_controller.s).
 * Functions still listed as INCLUDE_ASM are not byte-matching C yet. The whole
 * file is retail-build only: the port runs this overlay from its own bindings. */
#include "common.h"
#include "system/memory.h"

#ifndef XENO_PC_PORT
s32 func_8003852C(void *);
s32 func_800399D4(s32);
s32 func_80039C4C(s32);
s32 func_8003A094(void *);
s32 func_800716D8();
extern s32 D_800C3E54;
extern void *D_800D2DAC;
extern void *D_800D3278;
extern void *D_800D3340;
extern void *D_800D39D0;
extern void *D_battle_command_file1_801E9C38;

INCLUDE_ASM("asm/battle_command_file1/nonmatchings/before_controller", xeno_battle_command_file1_init_5160);

s32 func_battle_command_file1_801E563C(void) {
    s32 var_s0;

    HeapFree(D_800D3278);
    var_s0 = 0;
    HeapFree(D_800D2DAC);
    HeapFree(D_800D39D0);
    HeapFree(D_800D3340);
    HeapFree(D_battle_command_file1_801E9C38);
    if (*(u8 *)((u8*)D_800D3278 + 0x81F) != 0) {
        var_s0 = 1;
        func_80039C4C(D_800C3E54);
        func_800716D8();
        func_800399D4(D_800C3E54);
        func_800716D8();
    }
    if (*(u8 *)((u8*)D_800D3278 + 0x820) != 0) {
        func_8003A094(*(void **)((u8*)D_800D3278 + 0x818));
        func_8003852C(*(void **)((u8*)D_800D3278 + 0x818));
        func_800716D8();
        HeapFree(*(void **)((u8*)D_800D3278 + 0x818));
        *(u8 *)((u8*)D_800D3278 + 0x820) = 0U;
    }
    return var_s0;
}

INCLUDE_ASM("asm/battle_command_file1/nonmatchings/before_controller", func_battle_command_file1_801E5768);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/before_controller", func_battle_command_file1_801E57C4);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/before_controller", func_battle_command_file1_801E57F8);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/before_controller", func_battle_command_file1_801E58EC);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/before_controller", func_battle_command_file1_801E5A98);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/before_controller", xeno_battle_command_file1_helper_5B00);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/before_controller", func_battle_command_file1_801E5C1C);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/before_controller", func_battle_command_file1_801E5CE4);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/before_controller", func_battle_command_file1_801E5D24);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/before_controller", func_battle_command_file1_801E5DCC);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/before_controller", func_battle_command_file1_801E5EF8);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/before_controller", func_battle_command_file1_801E5F8C);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/before_controller", func_battle_command_file1_801E6084);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/before_controller", func_battle_command_file1_801E60E8);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/before_controller", func_battle_command_file1_801E6118);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/before_controller", func_battle_command_file1_801E6144);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/before_controller", func_battle_command_file1_801E61B4);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/before_controller", func_battle_command_file1_801E6224);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/before_controller", func_battle_command_file1_801E6294);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/before_controller", func_battle_command_file1_801E6304);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/before_controller", func_battle_command_file1_801E633C);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/before_controller", func_battle_command_file1_801E6374);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/before_controller", func_battle_command_file1_801E63E4);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/before_controller", func_battle_command_file1_801E6454);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/before_controller", func_battle_command_file1_801E64C4);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/before_controller", func_battle_command_file1_801E6534);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/before_controller", func_battle_command_file1_801E65A4);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/before_controller", func_battle_command_file1_801E65FC);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/before_controller", func_battle_command_file1_801E6660);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/before_controller", func_battle_command_file1_801E66D8);
INCLUDE_ASM("asm/battle_command_file1/nonmatchings/before_controller", xeno_battle_command_file1_helper_6750);
#endif
