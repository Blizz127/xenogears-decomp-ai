#include "common.h"


#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/main101", func_800B8098);
INCLUDE_ASM("asm/battle/nonmatchings/main101", func_800B81BC);
INCLUDE_ASM("asm/battle/nonmatchings/main101", func_800B8284);
#endif


extern s32 ArchiveDataSync(void);
extern void func_800BE790(void);


/* func_800B8354.s */
void func_800B8354(void) {
    while (ArchiveDataSync() != 0) {
        func_800BE790();
    }
}
