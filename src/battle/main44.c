#include "common.h"


extern u8 D_800C3CF4[];


/* func_8008AAA0.s */
/* Not yet byte-matching: the matching build assembles retail bytes for
 * this function from its own asm segment (see config/battle.yaml).  The
 * body below is kept for the port and the differential harnesses. */
#ifdef XENO_PC_PORT
void func_8008AAA0(u32 value) {
    u32 div = 100000000u;
    u32 i = 0;
    do {
        D_800C3CF4[i] = (u8)(value / div);
        value %= div;
        div = (u32)(((u64)div * 0xCCCCCCCDu >> 32) >> 3);
        i++;
    } while (i < 9);
    {
        u8 *dst = D_800C3CF4;
        u32 j = 1;
        for (;;) {
            if (D_800C3CF4[j] != 0) {
                if (*dst == 0)
                    *dst = 0xFF;
                break;
            }
            *dst = 0xFF;
            j++;
            if (j >= 9)
                break;
            dst++;
        }
    }
}
#endif /* XENO_PC_PORT */


extern int ArchiveSetIndex(int directoryIndex, int entryIndex);


/* func_8008AB4C.s */
void func_8008AB4C(void) {
    ArchiveSetIndex(0x20, 0);
}
/* func_8008AB70.s */
void func_8008AB70(void) {
    ArchiveSetIndex(0x20, 2);
}
/* func_8008AB94.s */
void func_8008AB94(void) {
    ArchiveSetIndex(0x20, 3);
}
/* func_8008ABB8.s */
void* func_8008ABB8(s32 size, s32 mode) {
    HeapChangeCurrentUser(2, 0);
    return HeapAlloc(size, mode);
}
/* func_8008AC00.s */
void* func_8008AC00(s32 size) {
    HeapChangeCurrentUser(2, 0);
    return HeapAlloc((size + 3) * 26, 0);
}
/* func_8008AC50.s */
/* Not yet byte-matching: the matching build assembles retail bytes for
 * this function from its own asm segment (see config/battle.yaml).  The
 * body below is kept for the port and the differential harnesses. */
#ifdef XENO_PC_PORT
void func_8008AC50(void) {
    while (ArchiveDataSync() != 0) {
        func_800716D8();
    }
}
#endif /* XENO_PC_PORT */
