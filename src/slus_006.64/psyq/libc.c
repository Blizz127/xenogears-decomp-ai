#include "common.h"

extern u_long g_RandomSeed;

#if defined(SKIP_ASM) || defined(XENO_PC_PORT)
void bzero(void* dst, int n) {
    u8* d = (u8*)dst;
    if (dst == NULL || n <= 0) return;
    while (n-- > 0) {
        *d++ = 0;
    }
}
#else
__asm__(
        ".globl bzero\n\t"
        ".ent\tbzero\n\t"
        "bzero:\n\t"
        ".word 0x10800009\n\t"
        ".word 0x00001021\n\t"
        ".word 0x1ca00003\n\t"
        ".word 0x00801021\n\t"
        ".reloc ., R_MIPS_26, .LbzeroEnd\n\t"
        ".word 0x08000000\n\t"
        ".word 0x00001021\n\t"
        ".word 0xa0800000\n\t"
        ".word 0x24a5ffff\n\t"
        ".word 0x1ca0fffd\n\t"
        ".word 0x24840001\n\t"
        ".LbzeroEnd:\n\t"
        ".word 0x03e00008\n\t"
        ".word 0x00000000\n\t"
        ".end\tbzero");
#endif

#if defined(SKIP_ASM) || defined(XENO_PC_PORT)
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
#else
__asm__(
        ".globl memcpy\n\t"
        ".ent\tmemcpy\n\t"
        "memcpy:\n\t"
        ".word 0x1080000a\n\t"
        ".word 0x00001021\n\t"
        ".word 0x18c00007\n\t"
        ".word 0x00801821\n\t"
        ".word 0x90a20000\n\t"
        ".word 0x24a50001\n\t"
        ".word 0x24c6ffff\n\t"
        ".word 0xa0820000\n\t"
        ".word 0x1cc0fffb\n\t"
        ".word 0x24840001\n\t"
        ".word 0x00601021\n\t"
        ".word 0x03e00008\n\t"
        ".word 0x00000000\n\t"
        ".end\tmemcpy");
#endif

void* memchr(unsigned char* ptr, unsigned char value, int count) {
    if (ptr == NULL || count <= 0) {
        return NULL;
    }

    count--;
    while (count >= 0) {
        if (*ptr++ == value) {
            return ptr - 1;
        }
        count--;
    }

    return NULL;
}

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

#if defined(SKIP_ASM) || defined(XENO_PC_PORT)
void* memset(void* dst, int val, int n) {
    u8* d = (u8*)dst;
    if (dst == NULL) return NULL;
    if (n <= 0) return NULL;
    while (n-- > 0) {
        *d++ = (u8)val;
    }
    return dst;
}
#else
__asm__(
        ".globl memset\n\t"
        ".ent\tmemset\n\t"
        "memset:\n\t"
        ".word 0x10800009\n\t"
        ".word 0x00001021\n\t"
        ".word 0x1cc00003\n\t"
        ".word 0x00801021\n\t"
        ".reloc ., R_MIPS_26, .LmemsetEnd\n\t"
        ".word 0x08000000\n\t"
        ".word 0x00001021\n\t"
        ".word 0xa0850000\n\t"
        ".word 0x24c6ffff\n\t"
        ".word 0x1cc0fffd\n\t"
        ".word 0x24840001\n\t"
        ".LmemsetEnd:\n\t"
        ".word 0x03e00008\n\t"
        ".word 0x00000000\n\t"
        ".end\tmemset");
#endif

int rand(void) {
    u_long nNext;

    nNext = (g_RandomSeed * 0x41C64E6D) + 0x3039;
    g_RandomSeed = nNext;
    return (nNext >> 0x10) & 0x7FFF;
}

void srand(u_long seed) {
    g_RandomSeed = seed;
}

/* Transcribed from asm/slus_006.64/nonmatchings/psyq/libc/strcat.s
 * (0x8003FA78-0x8003FB20). Appends src to the end of dst and returns dst; a NULL
 * dst or src returns NULL, and so does the (odd) case where the two strings'
 * computed end addresses coincide. Both lengths come from strlen, but the walk
 * to the terminator is then redone byte-by-byte before the copy. */
#if defined(SKIP_ASM) || defined(XENO_PC_PORT)
char* strcat(char* dst, char* src) {
    char* pDstEnd;
    char* pSrcEnd;
    char* pStart;

    if (dst == NULL) {
        return NULL;
    }
    if (src == NULL) {
        return NULL;
    }
    pStart = dst;
    pDstEnd = dst + strlen(dst);
    pSrcEnd = src + strlen(src);
    if (pDstEnd == pSrcEnd) {
        return NULL;
    }
    pDstEnd = dst;
    while (*pDstEnd != 0) {
        pDstEnd++;
    }
#ifdef LIBC_STRCAT_MUTANT
    pStart += 1;
#endif
    while ((*pDstEnd++ = *src++) != 0) {
        ;
    }
    return pStart;
}
#else
__asm__(
        ".globl strcat\n\t"
        ".ent\tstrcat\n\t"
        "strcat:\n\t"
        ".word 0x27bdffe0\n\t"
        ".word 0xafb10014\n\t"
        ".word 0x00808821\n\t"
        ".word 0xafb20018\n\t"
        ".word 0x00a09021\n\t"
        ".word 0xafbf001c\n\t"
        ".word 0x1220001b\n\t"
        ".word 0xafb00010\n\t"
        ".word 0x1240001a\n\t"
        ".word 0x00001021\n\t"
        ".set\tnoreorder\n\t"
        "jal strlen\n\t"
        "addu $a0, $s1, $zero\n\t"
        ".word 0x02402021\n\t"
        "jal strlen\n\t"
        "addu $s0, $s1, $v0\n\t"
        ".set\treorder\n\t"
        ".word 0x02421021\n\t"
        ".word 0x12020011\n\t"
        ".word 0x02201821\n\t"
        ".word 0x90620000\n\t"
        ".word 0x00000000\n\t"
        ".word 0x10400005\n\t"
        ".word 0x24710001\n\t"
        ".word 0x92220000\n\t"
        ".word 0x00000000\n\t"
        ".word 0x1440fffd\n\t"
        ".word 0x26310001\n\t"
        ".word 0x2631ffff\n\t"
        ".word 0x92420000\n\t"
        ".word 0x26520001\n\t"
        ".word 0xa2220000\n\t"
        ".word 0x1440fffc\n\t"
        ".word 0x26310001\n\t"
        ".reloc ., R_MIPS_26, .LstrcatEnd\n\t"
        ".word 0x08000000\n\t"
        ".word 0x00601021\n\t"
        ".word 0x00001021\n\t"
        ".LstrcatEnd:\n\t"
        ".word 0x8fbf001c\n\t"
        ".word 0x8fb20018\n\t"
        ".word 0x8fb10014\n\t"
        ".word 0x8fb00010\n\t"
        ".word 0x27bd0020\n\t"
        ".word 0x03e00008\n\t"
        ".word 0x00000000\n\t"
        ".end\tstrcat");
#endif

/* Transcribed from asm/slus_006.64/nonmatchings/psyq/libc/strcmp.s
 * (0x8003FB20-0x8003FB84, 25 instructions). NULL-tolerant compare: both NULL (or
 * the same pointer) returns 0, a NULL second string returns 1 and a NULL first
 * string returns -1; otherwise bytes are compared as unsigned and the first
 * difference is returned as a signed subtraction. */
#if defined(SKIP_ASM) || defined(XENO_PC_PORT)
int strcmp(char* s1, char* s2) {
    if (s1 == NULL || s2 == NULL) {
        if (s1 == s2) {
            return 0;
        }
#ifdef LIBC_STRCMP_MUTANT_SIGN
        return (s1 != NULL) ? -1 : 1;
#else
        return (s1 != NULL) ? 1 : -1;
#endif
    }
    while (*s1 == *s2) {
        if (*s1 == 0) {
            return 0;
        }
        s1++;
        s2++;
    }
    return (u8)*s1 - (u8)*s2;
}
#else
__asm__(
        ".globl strcmp\n\t"
        ".ent\tstrcmp\n\t"
        "strcmp:\n\t"
        ".word 0x10800003\n\t"
        ".word 0x00000000\n\t"
        ".word 0x14a0000b\n\t"
        ".word 0x00000000\n\t"
        ".word 0x14850003\n\t"
        ".word 0x00000000\n\t"
        ".reloc ., R_MIPS_26, .LstrcmpEnd\n\t"
        ".word 0x08000000\n\t"
        ".word 0x00001021\n\t"
        ".word 0x1480000e\n\t"
        ".word 0x24020001\n\t"
        ".reloc ., R_MIPS_26, .LstrcmpEnd\n\t"
        ".word 0x08000000\n\t"
        ".word 0x2402ffff\n\t"
        ".word 0x10c0fff9\n\t"
        ".word 0x24840001\n\t"
        ".word 0x90860000\n\t"
        ".word 0x90a30000\n\t"
        ".word 0x30c200ff\n\t"
        ".word 0x1043fffa\n\t"
        ".word 0x24a50001\n\t"
        ".word 0x90830000\n\t"
        ".word 0x90a2ffff\n\t"
        ".word 0x00000000\n\t"
        ".word 0x00621023\n\t"
        ".LstrcmpEnd:\n\t"
        ".word 0x03e00008\n\t"
        ".word 0x00000000\n\t"
        ".end\tstrcmp");
#endif

/* Transcribed from asm/slus_006.64/nonmatchings/psyq/libc/strcpy.s
 * (0x8003FB84-0x8003FBC8, 17 instructions). Copies the source string into the
 * destination and returns the destination; either NULL argument returns NULL.
 * The first byte is stored before the loop test, which is why retail shows the
 * load/store pair twice. */
#if defined(SKIP_ASM) || defined(XENO_PC_PORT)
char* strcpy(char* dst, char* src) {
    char* pStart;
    char c;

    if (dst == NULL) {
        return NULL;
    }
    if (src == NULL) {
        return NULL;
    }
    pStart = dst;
    c = *src++;
    *dst++ = c;
    if (c != 0) {
        do {
            c = *src++;
            *dst++ = c;
        } while (c != 0);
    }
#ifdef LIBC_STRCPY_MUTANT_RETURN_SRC
    return src;
#else
    return pStart;
#endif
}
#else
__asm__(
        ".globl strcpy\n\t"
        ".ent\tstrcpy\n\t"
        "strcpy:\n\t"
        ".word 0x1080000e\n\t"
        ".word 0x00001021\n\t"
        ".word 0x10a0000c\n\t"
        ".word 0x00801821\n\t"
        ".word 0x90a20000\n\t"
        ".word 0x24a50001\n\t"
        ".word 0x24640001\n\t"
        ".word 0x10400006\n\t"
        ".word 0xa0620000\n\t"
        ".word 0x90a20000\n\t"
        ".word 0x24a50001\n\t"
        ".word 0xa0820000\n\t"
        ".word 0x1440fffc\n\t"
        ".word 0x24840001\n\t"
        ".word 0x00601021\n\t"
        ".word 0x03e00008\n\t"
        ".word 0x00000000\n\t"
        ".end\tstrcpy");
#endif

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

#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libc", Sprintf);
#endif
