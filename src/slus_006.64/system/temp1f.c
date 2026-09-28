#include "common.h"
#include "psyq/libgpu.h"
#include "psyq/libgte.h"
#include "system/memory.h"

/* Retail TU 0x80025C04..0x80027D64 (GTE colour, atlas and line-scroll helpers), split out of system/temp1.c: retail temp1
 * was several TUs, which is visible in how each group reaches the shared Gfx
 * globals (gp-relative for its own .sbss, absolute for other TUs'). The port
 * still builds these functions from temp1.c, so this TU is empty there. */
#ifndef XENO_PC_PORT

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1f", func_80025C04);

/* Handwritten GTE color-processing routine retained as assembly. */
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1f", func_80025D4C);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1f", func_80025FA8);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1f", func_80026338);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1f", func_800263E4);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1f", func_8002675C);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1f", func_80026A0C);

void func_80026B9C(void) {
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1f", func_80026BA4);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1f", func_80026DCC);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1f", func_80026F44);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1f", func_80026FE8);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1f", func_8002709C);
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1f", func_800273C4);
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp1f", func_800278F8);

void func_80027D40(void* ptr) {
    if (ptr != NULL) {
        HeapFree(ptr);
    }
}

#endif /* !XENO_PC_PORT */
