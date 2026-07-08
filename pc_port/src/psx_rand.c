/*
 * psx_rand.c - PlayStation-compatible rand/srand for the native PC port.
 *
 * Retail field scripts (and several other game TUs) call rand() expecting the
 * PsyQ / SLUS_006.64 range 0..32767 (RAND_MAX). Handlers such as
 * FieldScriptVMHandlerMulVariableWithRand then scale with
 *   (rand() * (n + 1)) >> 15
 * which only yields 0..n when rand() is 15-bit.
 *
 * The decomp already has this LCG in src/slus_006.64/psyq/libc.c, but that TU
 * is not compiled into the PC port (INCLUDE_ASM surface / host libc conflict).
 * Without a local definition, the linker imports glibc rand (often 31-bit),
 * which blows script states (e.g. Map1 actor 20 var 0x0408 writes like 36875).
 *
 * This file supplies the retail algorithm so every existing rand() call site
 * gets the correct range without per-caller Map1 special cases.
 */
#include <stdint.h>

/* Retail global at 0x8005A1FC; host-only storage is fine for the port LCG. */
static uint32_t s_randomSeed;

int rand(void)
{
    uint32_t nNext;

    /* Same LCG as src/slus_006.64/psyq/libc.c (PsyQ-compatible). */
    nNext = (s_randomSeed * 0x41C64E6Du) + 0x3039u;
    s_randomSeed = nNext;
    return (int)((nNext >> 16) & 0x7FFF);
}

void srand(unsigned int seed)
{
    s_randomSeed = (uint32_t)seed;
}
