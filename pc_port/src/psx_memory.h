/*
 * psx_memory.h - PSX main-RAM emulation for the Xenogears PC port.
 *
 * Modeled on the Silent Hill PC port (pc_port/src/psx_memory.c there). The PS1
 * has 2 MB of RAM at 0x80000000-0x801FFFFF, and the decompiled game hardcodes
 * absolute addresses into it (e.g. the mem/heap fields of g_MainGameStates).
 * Rather than emulate the CPU, we allocate one real buffer and translate a PSX
 * address to a host pointer by masking to the 2 MB range and indexing the buffer.
 */
#ifndef XENO_PSX_MEMORY_H
#define XENO_PSX_MEMORY_H

#include <stdint.h>

/* 2 MB main RAM + 1 MB guard (some buffers near the top of RAM can overrun;
 * the original wraps, on PC the guard prevents corrupting adjacent globals). */
#define PSX_RAM_SIZE (2 * 1024 * 1024 + 1024 * 1024)
extern uint8_t g_PsxRam[];

/* Scratchpad (real PSX is 1 KB; allow headroom). */
extern uint8_t g_PsxScratchpad[];

/* Convert a PSX address (0x80XXXXXX / 0x00XXXXXX) to a host pointer into g_PsxRam. */
#define PSX_ADDR(addr) ((void*)(g_PsxRam + ((uintptr_t)(addr) & 0x1FFFFF)))

void PsxMemory_Init(void);

#endif /* XENO_PSX_MEMORY_H */
