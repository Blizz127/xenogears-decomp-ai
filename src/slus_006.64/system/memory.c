#include "common.h"
#include "system/memory.h"
#include "psyq/pc.h"

// .sbss
u16 g_HeapCurContentType;
u16 g_HeapCurUser;
void* g_Heap;
u32 g_HeapNeedsConsolidation;
u32 g_HeapIsErrorHandlerOff;
u8* g_SymbolData;
void* g_SymbolDataEndAddress;
u32 g_HeapLastAllocSize;
u32 g_HeapLastAllocSrcAddr;
u32 D_80059344;

// .rodata
HeapDelayedFreeBlock g_HeapDelayedFreeBlocksHead;

/* HeapDebugDump(Block) format strings. They are TU-owned bytes (yaml
 * [0x49A48, .sdata, system/memory] + [0x9258, .rodata, system/memory]) kept
 * in retail order; the D_ names match the disassembly labels so relocs read
 * the same. Retail addresses every one of them absolutely (lui+addiu, never
 * %gp_rel), but cc1 -G8 emits literal loads as gp_relativizable macros, so
 * each call site pins $a0 and materializes the address explicitly (same idiom
 * as heap_debug.c's HeapDumpToFile). The small-data macro loads elsewhere in
 * this TU (HeapPrintf's sink pointer, HeapAlloc's globals) still assemble
 * gp-relative under the SdataSmall preset. */
char D_80059248[] = "%06x ";
char D_80059250[] = "%6x ";
char D_80059258[] = "%s ";
char D_8005925C[] = "%s";
char D_80059260[] = " / ";
char D_80059264[] = "\n";
char D_80059268[] = "No- ";
char D_80059270[] = "MCB--- ";
char D_80059278[] = "ADDR-- ";
char D_80059280[] = "SIZE-- ";
char D_80059288[] = "USER ";
char D_80059290[] = "GETADD ";
char D_80059298[] = "%3d ";
char D_800592A0[] = "--- ";
char D_800592A8[] = "------ ";
char D_800592B0[] = "---- ";
const char D_80018A58[] = "FUNCTION/";
const char D_80018A64[] = "CONTENTS";
const char D_80018A70[] = "\nFree %6x";

#ifdef XENO_PC_PORT
#define HEAP_FMT_REG(name) char* name
#define HEAP_FMT_REG1(name) char* name
#define HEAP_FMT_REG1U(name) u_int name
#define HEAP_FMT_PTR(dst, sym) ((dst) = (sym))
#else
#define HEAP_FMT_REG(name) register char* name asm("$4")
#define HEAP_FMT_REG1(name) register char* name asm("$5")
#define HEAP_FMT_REG1U(name) register u_int name asm("$5")
#define HEAP_FMT_PTR(dst, sym) __asm__ volatile( \
    "lui %0,%%hi(" #sym ")\n\taddiu %0,%0,%%lo(" #sym ")" : "=r" (dst))
#endif


void* HeapGetNextBlockHeader(HeapBlock* pHeapBlock) {
    return (HeapBlock*)(pHeapBlock[-1].pNext - (mem_addr)pHeapBlock) - 1;
}

u_int HeapGetBlockUser(HeapBlock* pHeapBlock) {
    return pHeapBlock[-1].userTag;
}

mem_addr HeapGetBlockSourceAddress(HeapBlock* pHeapBlock) {
    return (pHeapBlock[-1].sourceAddress * 4) + VRAM_BASE_ADDRESS;
}

u_int HeapIsBlockPinned(HeapBlock* pHeapBlock) {
    return pHeapBlock[-1].isPinned;
}

void func_800318F0() {
    // Empty function, could have contained something
    // for debugging based on defines at some point?
    // #ifdef XXX
    //    ...
    // #endif
};

int HeapLoadSymbols(char* pSymbolFilePath) {
    int hSymbolFile;
    int nSymbolFileSize;
    int nBytesToRead;
    char* pSymbolData;

    nBytesToRead = 0x8000;
    
    PCinit();
    hSymbolFile = PCopen(pSymbolFilePath, O_RDONLY, 0);
    
    if (hSymbolFile != -1) {
        nSymbolFileSize = PClseek(hSymbolFile, 0, SEEK_END);
        PClseek(hSymbolFile, 0, SEEK_SET);
        HeapSetCurrentContentType(HEAP_CONTENT_SYMBOL_DATA);
        pSymbolData = HeapAlloc(nSymbolFileSize, 0);
        g_SymbolData = pSymbolData;
        g_SymbolDataEndAddress = pSymbolData + nSymbolFileSize;

        while (1) {
            if (nSymbolFileSize <= 0)
                break;

            if (nSymbolFileSize < nBytesToRead)
                nBytesToRead = nSymbolFileSize;

            PCread(hSymbolFile, pSymbolData, nBytesToRead);
            nSymbolFileSize -= nBytesToRead;
            pSymbolData += nBytesToRead;
        }
        
        PCclose(hSymbolFile);
        if ((g_SymbolData[0] != 'S') || 
            (g_SymbolData[1] != 'Y') || 
            (g_SymbolData[2] != 'M') || 
            (g_SymbolData[3] != '1')) {
            HeapFree(g_SymbolData);
            g_SymbolData = NULL;
            g_SymbolDataEndAddress = NULL;
        }
        
        return 0;
    }
    return -1;
}

void HeapReset(void) {
    func_8003747C(0);
    g_HeapCurContentType = HEAP_CONTENT_NONE;
    g_HeapCurUser = HEAP_USER_UNKNOWN;
    g_SymbolData = NULL;
    g_SymbolDataEndAddress = NULL;
}

void HeapInit(void* pHeapStart, void* pHeapEnd) {
    HeapBlock* startBlock = (HeapBlock*)((u32)pHeapStart & -4);
    HeapBlock* endBlock = (HeapBlock*)((u32)pHeapEnd & -4);
    g_Heap = &startBlock[1];
    startBlock->pNext = endBlock;
    
    g_HeapCurContentType = HEAP_CONTENT_NONE;
    g_HeapCurUser = HEAP_USER_UNKNOWN;
    g_HeapNeedsConsolidation = 0;
    g_SymbolData = NULL;
    g_SymbolDataEndAddress = NULL;
    
    startBlock->userTag = HEAP_USER_NONE;
    startBlock->contentTag = HEAP_CONTENT_FREE;
    
    endBlock[-1].pNext = endBlock;
    endBlock[-1].userTag = HEAP_USER_END;
    endBlock[-1].contentTag = HEAP_CONTENT_NONE;

    g_HeapDelayedFreeBlocksHead.pNext = NULL;
    HeapReset();
}

void HeapRelocate(void* pNewStartAddress) {
    HeapBlock* pNewHeap;
    HeapBlock* pPrevHeap;
    
    HeapFreeAllBlocks();
    if (g_HeapNeedsConsolidation) {
        HeapConsolidate();
    }

    pNewStartAddress = ((u32)pNewStartAddress & -4);
    pNewHeap = (HeapBlock*)pNewStartAddress;
    pPrevHeap = g_Heap;
    g_Heap = &pNewHeap[1];
    pNewHeap->pNext = pPrevHeap[-1].pNext;
    pNewHeap->userTag = HEAP_USER_NONE;
    pNewHeap->contentTag = HEAP_CONTENT_FREE;
    g_HeapDelayedFreeBlocksHead.pNext = NULL;
}

u_int HeapGetCurrentUser(void) {
    return g_HeapCurUser;
}

void HeapSetCurrentUser(u_int userTag) {
    g_HeapCurUser = userTag;
}

u_int HeapToggleErrorHandler(u_int status) {
  u_int prevStatus;
  prevStatus = g_HeapIsErrorHandlerOff;
  g_HeapIsErrorHandlerOff = status;
  return prevStatus;
}

void HeapGetAllocInformation(mem_addr* pAllocSourceAddr, u_int* pAllocSize) {
    *pAllocSourceAddr = g_HeapLastAllocSrcAddr;
    *pAllocSize = g_HeapLastAllocSize;
}

// Only matches on GCC 2.7.2
void* HeapAlloc(u_int allocSize, u_int allocFlags) {
    mem_addr nCallerAddr;
    u_int nFreeSize;
    int nRemainingSize;
    u_int bOutOfMemory;
    u_int nMinFreeSize;
    HeapBlock* pSmallBlock;
    HeapBlock* pCurBlockHeader;
    HeapBlock* pFreeBlock;
    HeapBlock* pFreeBlockHeader;
    HeapBlock* pNewBlock;
    HeapBlock* pNewBlock2;
    HeapBlock* pCurBlock;

    // Save address the called HeapAlloc
#ifndef XENO_PC_PORT
    asm volatile(
        "move $t7, %0\n\t"
        "sw $ra, 0($t7)\n\t"
    :: "r"(&nCallerAddr));
#else
    nCallerAddr = 0; /* native port: no MIPS $ra capture (debug source-addr only) */
#endif

    nCallerAddr -= 8;
    g_HeapLastAllocSrcAddr = nCallerAddr;
    nCallerAddr = ((nCallerAddr << 7) >> 9);
    
    if (g_HeapNeedsConsolidation != 0) {
        HeapConsolidate();
    }

    bOutOfMemory = 1; 
    g_HeapLastAllocSize = allocSize;
    allocSize = (allocSize + 3);
    allocSize &= -4;
    pFreeBlock = NULL;
    nMinFreeSize = 0x800000;
    pCurBlock = g_Heap;
    pSmallBlock = pFreeBlockHeader = NULL;
    pCurBlockHeader = pCurBlock - 1;
    
    while (1) {
        while (pCurBlockHeader->userTag != HEAP_USER_NONE) {
            if (pCurBlockHeader->userTag == HEAP_USER_END) {
                goto l_0x1CC_EndOfHeap;
            }

            pCurBlock = pCurBlockHeader->pNext;
            pCurBlockHeader = pCurBlock - 1;
        }
        
        nFreeSize = (u32)pCurBlockHeader->pNext - (u32)pCurBlockHeader - 0x10;
        nRemainingSize = nFreeSize - allocSize;

        if (nRemainingSize == 4 || nRemainingSize == 0) {
            bOutOfMemory = 0;

            // Small block allocation
            if (allocFlags == 1) {
                pSmallBlock = pCurBlockHeader; 
            } else {
                l_0xFC_AllocSmallBlock:
                pCurBlockHeader->userTag = g_HeapCurUser;
                pCurBlockHeader->contentTag = g_HeapCurContentType;
                pCurBlockHeader->isPinned = 0;
                pCurBlockHeader->sourceAddress = nCallerAddr;
                g_HeapCurContentType = HEAP_CONTENT_NONE;
                return pCurBlockHeader + 1;
            }
        } else if (nRemainingSize >= 5) {
            bOutOfMemory = 0;
    
            if (allocFlags != 1) {
                if (allocFlags != 2) {
                    goto l_0x1B4;
                }

                // Have we found a smaller block?
                if (nFreeSize < nMinFreeSize) {
                    pFreeBlock = pCurBlock;
                    pFreeBlockHeader = pCurBlockHeader;
                    nMinFreeSize = nFreeSize;
                }
                 goto l_0x1C0;
            }
            pFreeBlockHeader = pCurBlockHeader;
            goto l_0x1C0;
    
            l_0x1B4:
            pFreeBlock = pCurBlock;
            pFreeBlockHeader = pCurBlockHeader;
            goto l_0x28C;
        }

        l_0x1C0:
        pCurBlock = pCurBlockHeader->pNext;
        pCurBlockHeader = pCurBlock - 1;
    }

    l_0x1CC_EndOfHeap:
    if (bOutOfMemory) {
        if (g_HeapIsErrorHandlerOff) {
            return 0;
        }

        // Goes into error handler, does not return.
        MainLoop(ERR_HEAP_OUT_OF_MEMORY);
    }

    if (allocFlags == 1) {
        // Small block allocation
        if (pFreeBlockHeader < pSmallBlock) {
            pCurBlockHeader = pSmallBlock;
            goto l_0xFC_AllocSmallBlock;
        }

        pNewBlock = pFreeBlockHeader->pNext - (allocSize + 8);
        pNewBlock[-1].pNext = pFreeBlockHeader->pNext;
        pNewBlock[-1].userTag = g_HeapCurUser;
        pNewBlock[-1].contentTag = g_HeapCurContentType;
        pNewBlock[-1].isPinned = 0;
        pNewBlock[-1].sourceAddress = nCallerAddr;
        pFreeBlockHeader->pNext = pNewBlock;
        g_HeapCurContentType = HEAP_CONTENT_NONE;
        return pNewBlock;
    }
    
    l_0x28C:
    pNewBlock2 = (HeapBlock*)((u32)pFreeBlock + allocSize); 
    pNewBlock2->pNext = pFreeBlockHeader->pNext;
    pNewBlock2->userTag = pFreeBlockHeader->userTag;
    pNewBlock2->contentTag = pFreeBlockHeader->contentTag;
    pNewBlock2->isPinned = pFreeBlockHeader->isPinned;
    pNewBlock2->sourceAddress = pFreeBlockHeader->sourceAddress;
    
    pFreeBlockHeader->pNext = pNewBlock2 + 1;
    pFreeBlockHeader->contentTag = g_HeapCurContentType;
    g_HeapCurContentType = HEAP_CONTENT_NONE;
    pFreeBlockHeader->userTag = g_HeapCurUser;
    pFreeBlockHeader->isPinned = 0;
    pFreeBlockHeader->sourceAddress = nCallerAddr;
    return pFreeBlock;
}

void* HeapInsertAlloc(HeapBlock* pMem, u_int allocSize) {
    HeapBlock* pBlockHeader;
    HeapBlock* pNewBlock;
    HeapBlock* pNextBlock;
    u_int nFreeSize;

    pNextBlock = (HeapBlock*)pMem[-1].pNext;
    pBlockHeader = &pMem[-1];

    nFreeSize = (u32)pNextBlock - (u32)pBlockHeader - sizeof(HeapBlock)*2;
    if (allocSize + sizeof(HeapBlock)*2 >= nFreeSize)
        return NULL;
    
    pNewBlock = (u32)pMem + allocSize;
    pNewBlock->pNext = pNextBlock;
    pNewBlock->userTag = HEAP_USER_NONE;
    pNewBlock->contentTag = HEAP_CONTENT_FREE;
    pNewBlock->sourceAddress = 0;
    pNewBlock->isPinned = 0;
    pMem[-1].pNext = &pNewBlock[1];
    g_HeapNeedsConsolidation = 1;
    return pBlockHeader;
}

void HeapConsolidate(void) {
    HeapBlock* pCurrent;
    HeapBlock* pOther;
    
    pCurrent = (HeapBlock*)g_Heap - 1;

    while (pCurrent->userTag != HEAP_USER_END) {
        if (pCurrent->userTag == HEAP_USER_NONE) {
            pOther = pCurrent->pNext;
            while (
                (pCurrent->userTag == HEAP_USER_NONE) &&
                (pOther[-1].userTag == HEAP_USER_NONE)
            ) {
                pOther = pOther[-1].pNext;
                pCurrent->pNext = pOther;
            }
        }
        pCurrent = (HeapBlock*)pCurrent->pNext - 1;
    }
    
    g_HeapNeedsConsolidation = 0;
}

void HeapPinBlock(HeapBlock* pBlock) {
    pBlock[-1].isPinned = 1;
}

void HeapUnpinBlock(HeapBlock* pBlock) {
#ifdef XENO_PC_PORT
    if (pBlock == NULL || ((unsigned long)pBlock >> 47) != 0)
        return;
#endif
    pBlock[-1].isPinned = 0;
}

void HeapUnpinBlockCopy(HeapBlock* pBlock) {
    pBlock[-1].isPinned = 0;
}

u_int HeapFree(void* pMem) {
    mem_addr nCallerAddr;
    HeapBlock* pBlock;  

    if (pMem == NULL) {
        if (g_HeapIsErrorHandlerOff) {
            return 1;
        }
        
#ifndef XENO_PC_PORT
        asm volatile(
            "move $t7, %0\n\t"
            "sw $ra, 0($t7)\n\t"
        :: "r"(&nCallerAddr));
#else
        nCallerAddr = 0; /* native port: no MIPS $ra capture (debug source-addr only) */
#endif
        g_HeapLastAllocSize = 0;
        g_HeapLastAllocSrcAddr = nCallerAddr - 8;
        MainLoop(ERR_HEAP_FREE_NULL);
    }

    pBlock = ((HeapBlock*)pMem);
    if (pBlock[-1].isPinned == 0) {
        pBlock[-1].contentTag = 0x21;
        pBlock[-1].userTag = 0x0;
        pBlock[-1].sourceAddress = 0;
        g_HeapNeedsConsolidation = 1;
        return 0;
    } else if (pBlock[-1].isPinned == 1) {
        return -1;
    }
}

void HeapFreeBlocksWithFlag(u_char targetFlag) {
    void* pMem;
    HeapBlock* pCurBlock;

    pCurBlock = (HeapBlock*)g_Heap - 1;
    while (pCurBlock->userTag != HEAP_USER_END) {
        if (pCurBlock->userTag == targetFlag) {
            pMem = pCurBlock;
            pCurBlock = (HeapBlock*)pCurBlock->pNext - 1;
            HeapFree(pMem + sizeof(HeapBlock));
        } else {
            pCurBlock = (HeapBlock*)pCurBlock->pNext - 1;
        }
    }
}

void HeapFreeAllBlocks(void) {
    HeapBlock* pCurBlock;
    void* pMem;

    pCurBlock = (HeapBlock*)g_Heap - 1;
    while (pCurBlock->userTag != HEAP_USER_END) {
        pMem = pCurBlock;
        pCurBlock = (HeapBlock*)pCurBlock->pNext - 1;
        HeapFree(pMem + 8);
    }
}

void HeapForceFreeAllBlocks(void) {
    HeapBlock* pCurBlock;
    void* pMem;

    pCurBlock = (HeapBlock*)g_Heap - 1;
    while (pCurBlock->userTag != HEAP_USER_END) {
        pMem = pCurBlock;
        pCurBlock = (HeapBlock*)pCurBlock->pNext - 1;
        HeapUnpinBlock(pMem + 8);
        HeapFree(pMem + 8);
    }
}

u_int HeapGetTotalFreeSize(void) {
    u_int nTotalFreeSize;
    HeapBlock* pCurBlock;

    nTotalFreeSize = 0;
    pCurBlock = (HeapBlock*)g_Heap - 1;
    while (pCurBlock->userTag != HEAP_USER_END) {
        if (pCurBlock->userTag == HEAP_USER_NONE) {
            nTotalFreeSize += ((mem_addr)pCurBlock->pNext - (mem_addr)pCurBlock) - sizeof(HeapBlock)*2;
        }
        pCurBlock = (HeapBlock*)pCurBlock->pNext - 1;
    }
    return nTotalFreeSize;
}

u_int HeapWalkUntilEnd(void) {
    HeapBlock* pCurBlock;

    pCurBlock = (HeapBlock*)g_Heap - 1;
    while (pCurBlock->userTag != HEAP_USER_END) {
        pCurBlock = (HeapBlock*)pCurBlock->pNext - 1;
    }
    
    return 0;
}

u_int HeapGetLargestFreeBlockSize(void) {
    u_int nLargestFreeBlockSize;
    HeapBlock* pCurBlock;
    u_int nBlockSize;

    pCurBlock = (HeapBlock*)g_Heap - 1;
    nLargestFreeBlockSize = 0;
    while (pCurBlock->userTag != HEAP_USER_END) {
        if (pCurBlock->userTag == HEAP_USER_NONE) {
            nBlockSize = ((mem_addr)pCurBlock->pNext - (mem_addr)pCurBlock) - sizeof(HeapBlock)*2;
            if (nLargestFreeBlockSize < nBlockSize) {
                nLargestFreeBlockSize = nBlockSize;
            }
        }
        pCurBlock = (HeapBlock*)pCurBlock->pNext - 1;
    }
    
    if (nLargestFreeBlockSize < sizeof(HeapBlock)) {
        nLargestFreeBlockSize = sizeof(HeapBlock);
    }
    return nLargestFreeBlockSize - sizeof(HeapBlock);
}

void HeapChangeCurrentUser(u_int userTag, char** pContentTypes) {
    g_HeapCurUser = userTag;
    g_HeapUserContentNames[userTag] = pContentTypes;
    g_HeapIsErrorHandlerOff = 0;
}

void HeapSetCurrentContentType(u_short contentTag) {
    g_HeapCurContentType = contentTag;
}

// Symbol:
// 0x0: u32 address
// 0x4: u8 nameLen
// 0x5: u8 name[nameLen]
void HeapGetSymbolNameFromAddress(mem_addr address, u_char* pString) {
    u32 byte0;
    u32 byte1;
    u32 byte2;
    u32 byte3;
    u8* pSymbol;
    u8 chSymbolNameLetter;
    u8* pSymbolName;
    s32 nSymbolNameLen;

    pSymbol = g_SymbolData + 4;
    if (pSymbol != 0) {
        while ((void*)pSymbol < g_SymbolDataEndAddress) {
            byte0 = *pSymbol++;
            byte1 = *pSymbol++;
            byte2 = *pSymbol++;
            byte3 = *pSymbol++;

            if (!((byte0 | (byte1 << 0x8) | (byte2 << 0x10) | ( byte3 << 0x18)) < address))
                break;

            pSymbolName = pSymbol;
            nSymbolNameLen = *pSymbolName;
            pSymbol += 1;
            pSymbol += nSymbolNameLen;
        }
        
        nSymbolNameLen = *pSymbolName;
        nSymbolNameLen -= 1;
        pSymbolName = pSymbolName + 1;
        while (nSymbolNameLen != -1) {
            chSymbolNameLetter = *pSymbolName++;
            nSymbolNameLen -= 1;
            *pString++ = chSymbolNameLetter;
        }    
    }
    
    *pString = 0;
}

void HeapDebugDumpBlock(HeapBlock* pBlockHeader, void* pBlockMem, u_int blockSize, int debugFlags) {
    char sFunctionName[0x40];
    s32 nContentType;
    char* sContentType;
    u32 nContentFlag;

    if (debugFlags & HEAP_DEBUG_PRINT_MCB) {
        /* Retail sets up the 0xFFFFFF mask in $a1 before $a0. */
        HEAP_FMT_REG1U(mMCB);
        HEAP_FMT_REG(fMCB);
        mMCB = 0xFFFFFF;
        HEAP_FMT_PTR(fMCB, D_80059248);
        HeapPrintf(fMCB, (u32)pBlockHeader & mMCB);
    }
    if (debugFlags & HEAP_DEBUG_PRINT_ADDRESS) {
        HEAP_FMT_REG1U(mADDR);
        HEAP_FMT_REG(fADDR);
        mADDR = 0xFFFFFF;
        HEAP_FMT_PTR(fADDR, D_80059248);
        HeapPrintf(fADDR, (u32)pBlockMem & mADDR);
    }
    if (debugFlags & HEAP_DEBUG_PRINT_SIZE) {
        HEAP_FMT_REG(fSIZE);
        HEAP_FMT_PTR(fSIZE, D_80059250);
        HeapPrintf(fSIZE, blockSize);
    }
    if (debugFlags & HEAP_DEBUG_PRINT_USER) {
        /* Retail resolves the name into $a1 before materializing $a0. */
        HEAP_FMT_REG1(uName);
        HEAP_FMT_REG(fUSER);
        uName = *(&D_80050110 + pBlockHeader->userTag);
        HEAP_FMT_PTR(fUSER, D_80059258);
        HeapPrintf(fUSER, uName);
    }
    if (debugFlags & HEAP_DEBUG_PRINT_GETADD) {
        /* Retail loads the raw header word into $a1 and masks/shifts it
         * around the $a0 setup (no bitfield extraction). */
        HEAP_FMT_REG1U(wSrc);
        u_int mask;
        HEAP_FMT_REG(fGETADD);
        mask = 0x1FFFFF;
        wSrc = ((u_int*)pBlockHeader)[1];
        HEAP_FMT_PTR(fGETADD, D_80059248);
        wSrc &= mask;
        HeapPrintf(fGETADD, wSrc * 4);
    }
    if ((debugFlags & HEAP_DEBUG_PRINT_FUNCTION) && 
        (pBlockHeader->userTag != HEAP_USER_NONE)
    ) {
        HeapGetSymbolNameFromAddress((pBlockHeader->sourceAddress * 4) + VRAM_BASE_ADDRESS, sFunctionName);
        
        {
            HEAP_FMT_REG(fFUNC);
            HEAP_FMT_PTR(fFUNC, D_8005925C);
            HeapPrintf(fFUNC, sFunctionName);
        }
        if (debugFlags & HEAP_DEBUG_PRINT_CONTENTS) {
            if (pBlockHeader->contentTag & 0x1F) {
                HEAP_FMT_REG(fSEP);
                HEAP_FMT_PTR(fSEP, D_80059260);
                HeapPrintf(fSEP);
            }
        }
    } 
    
    if (debugFlags & HEAP_DEBUG_PRINT_CONTENTS) {
        nContentFlag = pBlockHeader->contentTag;
        nContentType = nContentFlag & 0x1F;
        if (nContentType != HEAP_CONTENT_NULL) {
            if (nContentFlag & HEAP_DEFAULT_CONTENT_TYPES) {
                sContentType = g_HeapContentTypeNames[nContentType];
            } else {
                sContentType = g_HeapUserContentNames[pBlockHeader->userTag][nContentFlag];
            }
            {
                HEAP_FMT_REG(fCONT);
                HEAP_FMT_PTR(fCONT, D_8005925C);
                HeapPrintf(fCONT, sContentType);
            }
        }
    }

    {
        HEAP_FMT_REG(fNL);
        HEAP_FMT_PTR(fNL, D_80059264);
        HeapPrintf(fNL);
    }
}

// Only matches on GCC 2.7.2
void HeapDebugDump(u_int mode, u_int startBlockIdx, int endBlockIdx, u_int flags) {
    char _sBuffer[0x40];
    u32 bIsDone;
    u32 nBlockSize;
    u32 debugFlags;
    u32 nBlockNumber;
    u32 nBlocksToPrint;
    u32 nBlocksToSkip;
    HeapBlock* pNextBlock;
    HeapBlock* pNext;
    HeapBlock* pCurBlockHeader;
    HeapBlock* pCurBlock;

    nBlocksToSkip = startBlockIdx;
    nBlocksToPrint = endBlockIdx;
    debugFlags = flags;
    nBlockNumber = 0;
    bIsDone = 0;

    // Default flags
    if (debugFlags == 0) {
        debugFlags = HEAP_DEBUG_PRINT_TOTAL_FREE_SIZE | 
            HEAP_DEBUG_PRINT_CONTENTS | 
            HEAP_DEBUG_PRINT_SIZE | 
            HEAP_DEBUG_PRINT_ADDRESS | 
            HEAP_DEBUG_PRINT_NO;
    }
    
    if (mode) {
        HeapConsolidate();
    }
    
    if (nBlocksToPrint) {
        bIsDone = 1;
    }
    
    if (g_SymbolData == NULL) {
        debugFlags &= ~HEAP_DEBUG_PRINT_FUNCTION;
    }

    // Print table header to screen
    if (debugFlags & HEAP_DEBUG_PRINT_NO) {
        HEAP_FMT_REG(fNo);
        HEAP_FMT_PTR(fNo, D_80059268);
        HeapPrintf(fNo);
    }
    if (debugFlags & HEAP_DEBUG_PRINT_MCB) {
        HEAP_FMT_REG(fMcb);
        HEAP_FMT_PTR(fMcb, D_80059270);
        HeapPrintf(fMcb);
    }
    if (debugFlags & HEAP_DEBUG_PRINT_ADDRESS) {
        HEAP_FMT_REG(fAddr);
        HEAP_FMT_PTR(fAddr, D_80059278);
        HeapPrintf(fAddr);
    }
    if (debugFlags & HEAP_DEBUG_PRINT_SIZE) {
        HEAP_FMT_REG(fSize);
        HEAP_FMT_PTR(fSize, D_80059280);
        HeapPrintf(fSize);
    }
    if (debugFlags & HEAP_DEBUG_PRINT_USER) {
        HEAP_FMT_REG(fUser);
        HEAP_FMT_PTR(fUser, D_80059288);
        HeapPrintf(fUser);
    }
    if (debugFlags & HEAP_DEBUG_PRINT_GETADD) {
        HEAP_FMT_REG(fGetadd);
        HEAP_FMT_PTR(fGetadd, D_80059290);
        HeapPrintf(fGetadd);
    }
    if (debugFlags & HEAP_DEBUG_PRINT_FUNCTION) {
        HEAP_FMT_REG(fFunc);
        HEAP_FMT_PTR(fFunc, D_80018A58);
        HeapPrintf(fFunc);
    }
    if (debugFlags & HEAP_DEBUG_PRINT_CONTENTS) {
        HEAP_FMT_REG(fCont);
        HEAP_FMT_PTR(fCont, D_80018A64);
        HeapPrintf(fCont);
    }
    
    {
        HEAP_FMT_REG(fNl);
        HEAP_FMT_PTR(fNl, D_80059264);
        HeapPrintf(fNl);
    }
    nBlockSize = 0;
    pCurBlockHeader = (HeapBlock*)g_Heap - 1;
    pCurBlock = (HeapBlock*)g_Heap;

    while (pCurBlockHeader->userTag != HEAP_USER_END) {
        nBlockSize += ((u32)pCurBlockHeader->pNext - (u32)pCurBlockHeader) - sizeof(HeapBlock)*2;
        
        if (
            mode == 2 && 
            (pNextBlock = ((HeapBlock*)pCurBlockHeader->pNext),
            pCurBlockHeader->contentTag == pNextBlock[-1].contentTag &&
            pCurBlockHeader->userTag == pNextBlock[-1].userTag)
        ) {
            nBlockNumber += 1;
            pCurBlockHeader = &pNextBlock[-1];            
        } else if (
            mode == 3 && 
            (pNextBlock = ((HeapBlock*)pCurBlockHeader->pNext), 
            pCurBlockHeader->sourceAddress == pNextBlock[-1].sourceAddress)
        ) {
            nBlockNumber += 1;
            pCurBlockHeader = &pNextBlock[-1];
        } else {    
            if (nBlocksToSkip) {
                nBlocksToSkip -= 1;
            } else {
                if (debugFlags & HEAP_DEBUG_PRINT_NO) {
                    HEAP_FMT_REG(fNum);
                    HEAP_FMT_PTR(fNum, D_80059298);
                    HeapPrintf(fNum, nBlockNumber);
                }
                
                // Print block information
                HeapDebugDumpBlock(pCurBlockHeader, pCurBlock, nBlockSize, debugFlags);
                
                nBlocksToPrint -= 1;
            }
            
            if (!bIsDone || nBlocksToPrint) {
                nBlockNumber += 1;
                pNext = (HeapBlock*)pCurBlockHeader->pNext;
                nBlockSize = 0;
                pCurBlockHeader = &pNext[-1];
                pCurBlock = pNext;
            } else {
                break;
            }
        }
    }

    // Print table footer to screen
    if (debugFlags & HEAP_DEBUG_PRINT_NO) {
        HEAP_FMT_REG(fDash);
        HEAP_FMT_PTR(fDash, D_800592A0);
        HeapPrintf(fDash);
    }
    if (debugFlags & HEAP_DEBUG_PRINT_MCB) {
        HEAP_FMT_REG(fDash6a);
        HEAP_FMT_PTR(fDash6a, D_800592A8);
        HeapPrintf(fDash6a);
    }
    if (debugFlags & HEAP_DEBUG_PRINT_ADDRESS) {
        HEAP_FMT_REG(fDash6b);
        HEAP_FMT_PTR(fDash6b, D_800592A8);
        HeapPrintf(fDash6b);
    }
    if (debugFlags & HEAP_DEBUG_PRINT_SIZE) {
        HEAP_FMT_REG(fDash6c);
        HEAP_FMT_PTR(fDash6c, D_800592A8);
        HeapPrintf(fDash6c);
    }
    if (debugFlags & HEAP_DEBUG_PRINT_USER) {
        HEAP_FMT_REG(fDash4);
        HEAP_FMT_PTR(fDash4, D_800592B0);
        HeapPrintf(fDash4);
    }
    
    if (debugFlags & HEAP_DEBUG_PRINT_TOTAL_FREE_SIZE) {
        /* Retail calls for the size first, then sets up $a0. */
        u_int nFreeSize = HeapGetTotalFreeSize();
        HEAP_FMT_REG(fFree);
        HEAP_FMT_PTR(fFree, D_80018A70);
        HeapPrintf(fFree, nFreeSize);
    }

    {
        HEAP_FMT_REG(fNl2);
        HEAP_FMT_PTR(fNl2, D_80059264);
        HeapPrintf(fNl2);
    }
}

void* HeapAllocSound(u_int allocSize) {
    u32 nPrevHeapUser;
    void* pMem;

    nPrevHeapUser = g_HeapCurUser;
    g_HeapCurUser = HEAP_USER_SUZU;
    g_HeapCurContentType = HEAP_CONTENT_SOUND;
    pMem = HeapAlloc(allocSize, 1);
    HeapPinBlock(pMem);
    g_HeapCurUser = nPrevHeapUser;
    return pMem;
}

void HeapCalloc(u_int numElements, u_int elementSize) {
    u32 nPrevHeapUser;

    nPrevHeapUser = g_HeapCurUser;
    g_HeapCurUser = HEAP_USER_SUZU;
    g_HeapCurContentType = HEAP_CONTENT_FAKE_CALLOC;
    HeapAlloc(elementSize * numElements, 0);
    g_HeapCurUser = nPrevHeapUser;
}

void HeapForceFree(void* pMem) {
    HeapUnpinBlock(pMem);
    HeapFree(pMem);
}

// HeapPrintf: g_HeapDebugPrintfFn must be GP rel
// HeapDumpToFile: g_HeapDebugPrintfFn must be extern
// Because of this, HeapDumpToFile is part of another TU than HeapPrintf
// (the definition lives in heap_debug.c; see its .sdata slot).

// Variadic args may have been used instead of 
// single argument, but can't find a match w/ va
void HeapPrintf(char* pFormat, void* arg) {
    char sBuffer[1024];
    Sprintf(sBuffer, pFormat, arg);
    g_HeapDebugPrintfFn(sBuffer);
}

void HeapDelayedFree(void* pMem, u_int delay) {
    HeapDelayedFreeBlock* pFreeBlock;
    mem_addr nCallerAddr;
    
    if (pMem == NULL) {
#ifndef XENO_PC_PORT
        asm volatile(
            "move $t7, %0\n\t"
            "sw $ra, 0($t7)\n\t"
        :: "r"(&nCallerAddr));
#else
        nCallerAddr = 0; /* native port: no MIPS $ra capture (debug source-addr only) */
#endif
        g_HeapLastAllocSize = 0;
        g_HeapLastAllocSrcAddr = nCallerAddr - 8;
        MainLoop(ERR_HEAP_FREE_NULL);
    }
    
    if (delay == 0) {
        HeapFree(pMem);
        return;
    }
    
    HeapSetCurrentContentType(HEAP_CONTENT_DELAY_FREE);
    pFreeBlock = HeapAlloc(sizeof(HeapDelayedFreeBlock), 0x1);
    pFreeBlock->pNext = g_HeapDelayedFreeBlocksHead.pNext;
    pFreeBlock->pMem = pMem;
    pFreeBlock->delay = delay;
    g_HeapDelayedFreeBlocksHead.pNext = pFreeBlock;
}

// Iterates over delayed free blocks, decreasing the delay.
// If delay reaches -1, these blocks are free'd.
void HeapTickDelayedFree(void) {
    HeapDelayedFreeBlock* pNextBlock;
    HeapDelayedFreeBlock* pCurBlock;

    pCurBlock = &g_HeapDelayedFreeBlocksHead;
    pNextBlock = g_HeapDelayedFreeBlocksHead.pNext;
    
    while (pNextBlock != NULL) {
        pNextBlock->delay -= 1;
        if (pNextBlock->delay == -1) {
            HeapFree(pNextBlock->pMem);
            pCurBlock->pNext = pNextBlock->pNext;
            HeapFree(pNextBlock);
            if (pCurBlock->pNext == NULL)
                break;
        } else {
            pCurBlock = pCurBlock->pNext;
        }
        pNextBlock = pCurBlock->pNext;
    }
}

void HeapFreeAllDelayedBlocks(void) {
    HeapDelayedFreeBlock* pHead;
    HeapDelayedFreeBlock* pNextBlock;

    pHead = &g_HeapDelayedFreeBlocksHead;
    pNextBlock = g_HeapDelayedFreeBlocksHead.pNext;
    
    while (pNextBlock != NULL) {
        HeapFree(pNextBlock->pMem);
        pHead->pNext = pNextBlock->pNext;
        HeapFree(pNextBlock);
        if (pHead->pNext == NULL)
            break;
        pNextBlock = pHead->pNext;
    }
}