/* Retail TU part 2 of main40.c (see the BATTLE_TU_PART note there): the
 * still-assembly run below, then that file's part-2 C bodies. */
#define BATTLE_TU_PART 2
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/main40_p2", func_80088B80);
INCLUDE_ASM("asm/battle/nonmatchings/main40_p2", func_80089038);
INCLUDE_ASM("asm/battle/nonmatchings/main40_p2", func_80089110);
INCLUDE_ASM("asm/battle/nonmatchings/main40_p2", func_800891E4);
INCLUDE_ASM("asm/battle/nonmatchings/main40_p2", func_80089348);
INCLUDE_ASM("asm/battle/nonmatchings/main40_p2", func_8008946C);
INCLUDE_ASM("asm/battle/nonmatchings/main40_p2", func_8008963C);
INCLUDE_ASM("asm/battle/nonmatchings/main40_p2", func_800897CC);
INCLUDE_ASM("asm/battle/nonmatchings/main40_p2", func_800898F0);
#endif
#include "main40.c"
