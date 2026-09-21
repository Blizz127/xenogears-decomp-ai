#include "common.h"

/* Retail SN-library shims are raw MIPS words; the host port links PsyCross's
 * own PCopen/PCclose/PClseek/PCcreate/PCinit instead (libgte.c precedent). */
#ifndef XENO_PC_PORT
__asm__(
        ".globl PCopen\n\t"
        ".ent PCopen\n\t"
        "PCopen:\n\t"
        ".word 0x00a03021\n\t"
        ".word 0x00802821\n\t"
        ".word 0x000040cd\n\t"
        ".word 0x10400002\n\t"
        ".word 0x00601021\n\t"
        ".word 0x2402ffff\n\t"
        ".word 0x03e00008\n\t"
        ".word 0x00000000\n\t"
        ".end PCopen");

__asm__(
        ".globl PCclose\n\t"
        ".ent PCclose\n\t"
        "PCclose:\n\t"
        ".word 0x00802821\n\t"
        ".word 0x0000410d\n\t"
        ".word 0x03e00008\n\t"
        ".word 0x00000000\n\t"
        ".end PCclose");

__asm__(
        ".globl PClseek\n\t"
        ".ent PClseek\n\t"
        "PClseek:\n\t"
        ".word 0x00c03821\n\t"
        ".word 0x00a03021\n\t"
        ".word 0x00802821\n\t"
        ".word 0x000041cd\n\t"
        ".word 0x10400002\n\t"
        ".word 0x00601021\n\t"
        ".word 0x2402ffff\n\t"
        ".word 0x03e00008\n\t"
        ".word 0x00000000\n\t"
        ".end PClseek");

__asm__(
        ".globl PCcreate\n\t"
        ".ent PCcreate\n\t"
        "PCcreate:\n\t"
        ".word 0x00802821\n\t"
        ".word 0x00003021\n\t"
        ".word 0x0000408d\n\t"
        ".word 0x10400002\n\t"
        ".word 0x00601021\n\t"
        ".word 0x2402ffff\n\t"
        ".word 0x03e00008\n\t"
        ".word 0x00000000\n\t"
        ".end PCcreate");

__asm__(
        ".globl PCinit\n\t"
        ".ent PCinit\n\t"
        "PCinit:\n\t"
        ".word 0x0000404d\n\t"
        ".word 0x03e00008\n\t"
        ".word 0x00000000\n\t"
        ".end PCinit");
#endif /* XENO_PC_PORT */

/* Transcribed from asm/slus_006.64/nonmatchings/psyq/libsn/PCread.s
 * (0x8004C398-0x8004C458). Chunked read: up to 0x8000 bytes per call through
 * func_8004C458(0, fd, size, buff); a -1 result aborts with -1 (the partial total
 * is still accumulated first, as retail's delay slot does), a short read stops
 * the loop, otherwise it continues until the requested length is satisfied.
 * The port links PsyCross's own PCread, so this definition is weak there. */
extern int func_8004C458(int, int, int, char*);

#if defined(SKIP_ASM) || defined(XENO_PC_PORT)
#ifdef XENO_PC_PORT
__attribute__((weak))
#endif
int PCread(int fd, char* buff, int len) {
    int total = 0;
    int chunk;
    int ret;

    if (len == 0) {
        return 0;
    }
    do {
#ifdef LIBSN_PCREAD_MUTANT_CHUNK4
        chunk = (0x4000 < len) ? 0x4000 : len;
#else
        chunk = (0x8000 < len) ? 0x8000 : len;
#endif
        ret = func_8004C458(0, fd, chunk, buff);
        total += ret;
        if (ret == -1) {
            return -1;
        }
        buff += ret;
        len -= ret;
        if (ret < chunk) {
            break;
        }
    } while (len != 0);
    return total;
}
#else
__asm__(
        ".globl PCread\n\t"
        ".ent\tPCread\n\t"
        "PCread:\n\t"
        ".word 0x27bdffd0\n\t"
        ".word 0xafb40020\n\t"
        ".word 0x0080a021\n\t"
        ".word 0xafb3001c\n\t"
        ".word 0x00a09821\n\t"
        ".word 0xafb00010\n\t"
        ".word 0x00c08021\n\t"
        ".word 0xafb20018\n\t"
        ".word 0x00009021\n\t"
        ".word 0xafbf002c\n\t"
        ".word 0xafb60028\n\t"
        ".word 0xafb50024\n\t"
        ".word 0x12000017\n\t"
        ".word 0xafb10014\n\t"
        ".word 0x34168000\n\t"
        ".word 0x2415ffff\n\t"
        ".word 0x02d0102b\n\t"
        ".word 0x10400002\n\t"
        ".word 0x02008821\n\t"
        ".word 0x34118000\n\t"
        ".word 0x00002021\n\t"
        ".word 0x02802821\n\t"
        ".word 0x02203021\n\t"
        ".set\tnoreorder\n\t"
        "jal func_8004C458\n\t"
        "addu $a3, $s3, $zero\n\t"
        ".set\treorder\n\t"
        ".word 0x14550003\n\t"
        ".word 0x02429021\n\t"
        ".reloc ., R_MIPS_26, .LPCreadFail\n\t"
        ".word 0x08000000\n\t"
        ".word 0x2402ffff\n\t"
        ".word 0x02629821\n\t"
        ".word 0x02028023\n\t"
        ".word 0x0051102a\n\t"
        ".word 0x14400003\n\t"
        ".word 0x00000000\n\t"
        ".word 0x1600ffee\n\t"
        ".word 0x02d0102b\n\t"
        ".word 0x02401021\n\t"
        ".LPCreadFail:\n\t"
        ".word 0x8fbf002c\n\t"
        ".word 0x8fb60028\n\t"
        ".word 0x8fb50024\n\t"
        ".word 0x8fb40020\n\t"
        ".word 0x8fb3001c\n\t"
        ".word 0x8fb20018\n\t"
        ".word 0x8fb10014\n\t"
        ".word 0x8fb00010\n\t"
        ".word 0x27bd0030\n\t"
        ".word 0x03e00008\n\t"
        ".word 0x00000000\n\t"
        ".end\tPCread");
#endif

#ifndef XENO_PC_PORT
__asm__(
        ".globl func_8004C458\n\t"
        ".ent func_8004C458\n\t"
        "func_8004C458:\n\t"
        ".word 0x0000414d\n\t"
        ".word 0x10400002\n\t"
        ".word 0x00601021\n\t"
        ".word 0x2402ffff\n\t"
        ".word 0x03e00008\n\t"
        ".word 0x00000000\n\t"
        ".end func_8004C458");
#endif /* XENO_PC_PORT */

/* Transcribed from asm/slus_006.64/nonmatchings/psyq/libsn/PCwrite.s
 * (0x8004C470-0x8004C530). Same chunked loop as PCread but through
 * func_8004C530(0, fd, size, buff): up to 0x8000 bytes per call, -1 aborts with
 * -1, a short write stops the loop, otherwise it runs until the length is
 * satisfied. Weak under XENO_PC_PORT because the port links PsyCross's own. */
extern int func_8004C530(int, int, int, char*);

#if defined(SKIP_ASM) || defined(XENO_PC_PORT)
#ifdef XENO_PC_PORT
__attribute__((weak))
#endif
int PCwrite(int fd, char* buff, int len) {
    int total = 0;
    int chunk;
    int ret;

    if (len == 0) {
        return 0;
    }
    do {
        chunk = (0x8000 < len) ? 0x8000 : len;
        ret = func_8004C530(0, fd, chunk, buff);
        total += ret;
        if (ret == -1) {
            return -1;
        }
        buff += ret;
        len -= ret;
        if (ret < chunk) {
            break;
        }
    } while (len != 0);
    return total;
}
#else
__asm__(
        ".globl PCwrite\n\t"
        ".ent\tPCwrite\n\t"
        "PCwrite:\n\t"
        ".word 0x27bdffd0\n\t"
        ".word 0xafb40020\n\t"
        ".word 0x0080a021\n\t"
        ".word 0xafb3001c\n\t"
        ".word 0x00a09821\n\t"
        ".word 0xafb00010\n\t"
        ".word 0x00c08021\n\t"
        ".word 0xafb20018\n\t"
        ".word 0x00009021\n\t"
        ".word 0xafbf002c\n\t"
        ".word 0xafb60028\n\t"
        ".word 0xafb50024\n\t"
        ".word 0x12000017\n\t"
        ".word 0xafb10014\n\t"
        ".word 0x34168000\n\t"
        ".word 0x2415ffff\n\t"
        ".word 0x02d0102b\n\t"
        ".word 0x10400002\n\t"
        ".word 0x02008821\n\t"
        ".word 0x34118000\n\t"
        ".word 0x00002021\n\t"
        ".word 0x02802821\n\t"
        ".word 0x02203021\n\t"
        ".set\tnoreorder\n\t"
        "jal func_8004C530\n\t"
        "addu $a3, $s3, $zero\n\t"
        ".set\treorder\n\t"
        ".word 0x14550003\n\t"
        ".word 0x02429021\n\t"
        ".reloc ., R_MIPS_26, .LPCwriteFail\n\t"
        ".word 0x08000000\n\t"
        ".word 0x2402ffff\n\t"
        ".word 0x02629821\n\t"
        ".word 0x02028023\n\t"
        ".word 0x0051102a\n\t"
        ".word 0x14400003\n\t"
        ".word 0x00000000\n\t"
        ".word 0x1600ffee\n\t"
        ".word 0x02d0102b\n\t"
        ".word 0x02401021\n\t"
        ".LPCwriteFail:\n\t"
        ".word 0x8fbf002c\n\t"
        ".word 0x8fb60028\n\t"
        ".word 0x8fb50024\n\t"
        ".word 0x8fb40020\n\t"
        ".word 0x8fb3001c\n\t"
        ".word 0x8fb20018\n\t"
        ".word 0x8fb10014\n\t"
        ".word 0x8fb00010\n\t"
        ".word 0x27bd0030\n\t"
        ".word 0x03e00008\n\t"
        ".word 0x00000000\n\t"
        ".end\tPCwrite");
#endif
