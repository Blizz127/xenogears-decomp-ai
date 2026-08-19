#include "common.h"

extern u_long g_RandomSeed;

void bzero(void* dst, int n) {
    u8* d = (u8*)dst;
    if (dst == NULL || n <= 0) return;
    while (n-- > 0) {
        *d++ = 0;
    }
}

void* memcpy(void* dst, const void* src, int n) {
    u8* d = (u8*)dst;
    const u8* s = (const u8*)src;
    if (dst == NULL) return NULL;
    if (n > 0) {
        while (n-- > 0) {
            *d++ = *s++;
        }
    }
    return dst;
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libc", memchr);

/* PSYQ gcc 2.7 often lowers memcpy() to jal bcopy (BSD arg order: src,dst,n).
 * Retail has no standalone bcopy; provide a small non-builtin loop so matching
 * links when decompiled C still goes through that lowering. */
void bcopy(void* src, void* dst, int n) {
    u_char* s = (u_char*)src;
    u_char* d = (u_char*)dst;
    while (n-- > 0) {
        *d++ = *s++;
    }
}

void* memmove(u_char* pDst, u_char* pSrc, int size) {
    if (pDst >= pSrc) {
        while (size-- > 0) {
            pDst[size] = pSrc[size];
        }
    } else {
        while (size-- > 0) {
            *pDst++ = *pSrc++;
        }
    }

    return pDst;
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libc", memset);

int rand(void) {
    u_long nNext;

    nNext = (g_RandomSeed * 0x41C64E6D) + 0x3039;
    g_RandomSeed = nNext;
    return (nNext >> 0x10) & 0x7FFF;
}

void srand(u_long seed) {
    g_RandomSeed = seed;
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libc", strcat);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libc", strcmp);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libc", strcpy);

int strlen(char* pString) {
    int nLen;
    char chCur;

    nLen = 0;
    if (pString == NULL)
        return 0;

    while (chCur = *pString, pString++, chCur != NULL)
        nLen++;
    return nLen;
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libc", Sprintf);
