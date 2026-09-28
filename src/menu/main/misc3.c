/* Retail TU misc3 of menu.bin (0x801E8070..end).  Its source lives in misc.c, part 3;
 * the port builds it from misc.c, so this TU is empty there. */
/* Never compiled: splat decides which per-function .s files to emit from the
 * INCLUDE_ASM uses it finds in this file, and the real ones live in the
 * MENU_PART == 3 half of misc.c.  Keep this list in sync with them. */
#if 0
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc3", func_801E8070);
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc3", func_801E8474);
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc3", func_801E86C8);
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc3", func_801E8978);
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc3", func_801E8B4C);
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc3", func_801E8DA8);
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc3", func_801E8EAC);
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc3", func_801E8F60);
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc3", func_801E920C);
INCLUDE_ASM("../asm/menu/nonmatchings/main/misc3", func_801E93A0);
#endif

#ifndef XENO_PC_PORT
#define MENU_PART 3
#include "misc.c"
#endif
