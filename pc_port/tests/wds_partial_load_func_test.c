/*
 * Runs the real func_800380D0 port arm against a planted boot WDS entry.
 * The void** mutant (injected by the runner) must clobber the neighbour
 * payload or fail to terminate; the fixed body must link with a 4-byte pNext
 * and leave the planted header word intact.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "system/sound.h"

SoundWDSEntry* g_SoundWdsLinkedList;
u_long g_unk_SoundEvent;

static unsigned char s_new_block[0x40];
static int s_alloc_calls;
static int s_error_id;

SoundWDSEntry* SoundFindWdsEntry(int targetID)
{
    (void)targetID;
    return NULL;
}

void SoundHandleError(s32 errorId)
{
    s_error_id = errorId;
}

u32 SoundSpuMemoryAllocateWDS(void* pWdsFile, int mode)
{
    (void)pWdsFile;
    (void)mode;
    return 0x1010u;
}

void SoundWdsSetTransferParamters(int transferAddress, int numBytesToTransfer)
{
    (void)transferAddress;
    (void)numBytesToTransfer;
}

void SoundTransferWdsPart(u8* data, s32 size)
{
    (void)data;
    (void)size;
}

void* func_80039024(s32 size)
{
    (void)size;
    s_alloc_calls++;
    memset(s_new_block, 0xCD, sizeof(s_new_block));
    return s_new_block;
}

int SoundSpuMemoryFreeBlock(int targetAddress)
{
    return targetAddress;
}

void SoundHeapSetBlockMemory(void* pBlockMemory, void* pSrc, int size)
{
    memcpy(pBlockMemory, pSrc, (size_t)size);
}

long DisableEvent(long ev)
{
    return ev;
}

long EnableEvent(long ev)
{
    return ev;
}

extern void* func_800380D0(void* pWds, s32 arg1, s32 arg2);

int main(void)
{
    static unsigned char existing[0x40];
    static unsigned char wds[0x40];
    SoundWDSEntry* boot;
    SoundWDSEntry* added;
    const uint32_t planted = 0xA1B2C3D4u;

    memset(existing, 0, sizeof(existing));
    memset(wds, 0, sizeof(wds));
    if ((uintptr_t)existing > 0xffffffffull || (uintptr_t)s_new_block > 0xffffffffull) {
        printf("FAIL: test fixtures must sit in the 32-bit heap range\n");
        return 1;
    }
    *(uint32_t*)(wds + 0x10) = 0x30; /* headerSizeMby */
    *(uint32_t*)(wds + 0x14) = 0x2000;
    *(uint32_t*)(wds + 0x18) = 0x30;
    *(uint16_t*)(wds + 0x20) = 0x77;

    boot = (SoundWDSEntry*)existing;
    boot->id = 1;
    boot->pNext = 0;
    *(uint32_t*)(existing + 0x30) = planted;
    g_SoundWdsLinkedList = boot;

    added = func_800380D0(wds, 0x2000, 0);
    if (added == NULL || s_error_id != 0 || s_alloc_calls != 1) {
        printf("FAIL: func_800380D0 did not register (err=%d allocs=%d)\n",
               s_error_id, s_alloc_calls);
        return 1;
    }
    if (boot->pNext != SOUND_PTR_TO_PSX(added)) {
        printf("FAIL: boot pNext=%08x want %08x (8-byte walk missed the tail)\n",
               boot->pNext, SOUND_PTR_TO_PSX(added));
        return 1;
    }
    if (*(uint32_t*)(existing + 0x30) != planted) {
        printf("FAIL: boot +0x30 payload clobbered to %08x\n",
               *(uint32_t*)(existing + 0x30));
        return 1;
    }
    if (added->spuMemoryAddress != 0x1010) {
        printf("FAIL: spuMemoryAddress=%d\n", added->spuMemoryAddress);
        return 1;
    }
    if (added->pNext != 0) {
        printf("FAIL: new pNext=%08x\n", added->pNext);
        return 1;
    }
    if (g_SoundWdsLinkedList != boot) {
        printf("FAIL: list head moved\n");
        return 1;
    }
    printf("func_800380D0 live append preserved 4-byte pNext and +0x30 payload\n");
    return 0;
}
