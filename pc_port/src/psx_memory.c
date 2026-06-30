/*
 * psx_memory.c - PSX main-RAM emulation for the Xenogears PC port.
 * See psx_memory.h. Mirrors the Silent Hill port's approach.
 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "psx_memory.h"

uint8_t g_PsxRam[PSX_RAM_SIZE];
uint8_t g_PsxScratchpad[4096];

void PsxMemory_Init(void)
{
    memset(g_PsxRam, 0, sizeof(g_PsxRam));
    memset(g_PsxScratchpad, 0, sizeof(g_PsxScratchpad));
    printf("[xeno-port] PSX RAM emulation: %d KB at %p\n",
           (int)(PSX_RAM_SIZE / 1024), (void*)g_PsxRam);
}
