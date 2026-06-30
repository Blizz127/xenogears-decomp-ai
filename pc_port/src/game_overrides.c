/*
 * Port-only functional implementations of boot-path functions discovered via
 * the oracle (running xeno-port and reading the "[stub] <name>" it stops at).
 *
 * These live here, rather than in the matching tree, when the original lives in
 * a raw asm segment that hasn't been carved into a C translation unit yet.
 * They are correct-by-inspection of the disassembly, NOT byte-matched. When a
 * function later gets a proper home in src/ (matched), delete it from here.
 *
 * Compiled into the port build by pc_port/build_port.sh; defining a symbol here
 * removes it from the auto-generated stub set.
 */

/* func_800363F0  (asm/slus_006.64/26644.s):
 *     lui $at, %hi(D_800501FC); sw $a0, %lo(D_800501FC)($at); jr $ra
 *   => D_800501FC = arg0; */
int D_800501FC;
void func_800363F0(int arg0) { D_800501FC = arg0; }

/* ---------------------------------------------------------------------------
 * Game-state dispatch table (data migration, Silent-Hill style).
 *
 * On PSX this table is initialized data embedding absolute RAM addresses for
 * each state's memory/heap regions. We rebuild it at runtime: function pointers
 * to the real state mains, and PSX_ADDR() for the mem/heap regions so they live
 * in the emulated PSX RAM buffer. Values extracted from the matching ELF.
 * --------------------------------------------------------------------------- */
#include "common.h"
#include "main/main.h"
#include "system/memory.h"
#include "psx_memory.h"

extern void KernelMenuMain(void);
extern void FieldMain(void);
extern void func_8001B6C4(void);
extern void MenuMain(void);

MainGameState g_MainGameStates[7];

void PcPort_InitGameStates(void)
{
    /* [idx] = { pFnMain, pMemStart, pHeapStart, hasOverlay } */
    g_MainGameStates[0].pFnMain    = KernelMenuMain;            /* boot state */
    g_MainGameStates[0].pMemStart  = PSX_ADDR(0x000592b8);
    g_MainGameStates[0].pHeapStart = PSX_ADDR(0x0006faec);
    g_MainGameStates[0].hasOverlay = 0;

    g_MainGameStates[1].pFnMain    = FieldMain;
    g_MainGameStates[1].pMemStart  = PSX_ADDR(0x000af5e4);
    g_MainGameStates[1].pHeapStart = PSX_ADDR(0x000c426c);
    g_MainGameStates[1].hasOverlay = 1;

    g_MainGameStates[2].pFnMain    = func_8001B6C4;
    g_MainGameStates[2].pMemStart  = PSX_ADDR(0x000c3a6c);
    g_MainGameStates[2].pHeapStart = PSX_ADDR(0x000d39f0);
    g_MainGameStates[2].hasOverlay = 1;

    /* states 3, 4, 6 are field/battle overlay mains not yet symbol-named;
     * left NULL until the oracle reaches them. */

    g_MainGameStates[5].pFnMain    = MenuMain;
    g_MainGameStates[5].pMemStart  = PSX_ADDR(0x000592b8);
    g_MainGameStates[5].pHeapStart = PSX_ADDR(0x0006faec);
    g_MainGameStates[5].hasOverlay = 0;
}

/* ---------------------------------------------------------------------------
 * Heap bootstrap.
 *
 * On hardware the boot routine func_80019578 (asm/.../main/main) calls
 * HeapInit(func_8002DFE0(), 0x801FC000) once before falling into MainLoop, where
 * func_8002DFE0 returns &D_8006FAF0. MainLoop never re-inits the heap; it only
 * HeapRelocate()s within it, which walks g_Heap and crashes if it was never set
 * up. The oracle enters MainLoop() directly (the asm `start`/boot is not yet C),
 * so the port performs that one-time HeapInit here, translated into emulated RAM.
 * --------------------------------------------------------------------------- */
void PcPort_HeapBoot(void)
{
    HeapInit(PSX_ADDR(0x8006FAF0), PSX_ADDR(0x801FC000));
}

/* ---------------------------------------------------------------------------
 * LZSS decompressor.
 *   LZSSHeapDecompress @ 0x80032E88, LZSSDecompress @ 0x80032EB4
 *   (asm/slus_006.64/util/lzss.s -- pure asm, no C translation unit yet).
 *
 * Functional re-implementation, correct-by-inspection of the disassembly (not
 * byte-matched). Stream format: the first 4 bytes of the source are the
 * decompressed size. The remainder is a token stream of one flag byte (8 bits
 * consumed LSB-first) followed by that many tokens:
 *   bit 0 -> literal: copy the next source byte verbatim.
 *   bit 1 -> back-reference from two bytes b0,b1:
 *              offset = b0 | ((b1 & 0xF) << 8)   (12-bit window)
 *              length = (b1 >> 4) + 3
 *            copy `length` bytes from (dst - offset).
 * The original checks the output-end only at each 8-token group boundary, so a
 * group is always processed in full; this mirrors that exactly.
 * --------------------------------------------------------------------------- */
void* LZSSDecompress(void* pSrc, void* pDst)
{
    u8* src = (u8*)pSrc;
    u8* dst = (u8*)pDst;
    u8* dstStart = dst;
    u8* dstEnd = dst + *(u32*)src;
    src += 4;

    while (dst != dstEnd) {
        u8 flags = *src++;
        int n;
        for (n = 0; n < 8; n++, flags >>= 1) {
            if (flags & 1) {
                u32 b0 = *src++;
                u32 b1 = *src++;
                u32 offset = b0 | ((b1 & 0xF) << 8);
                u32 length = (b1 >> 4) + 3;
                u8* ref = dst - offset;
                u32 i;
                for (i = 0; i < length; i++) {
                    *dst++ = *ref++;
                }
            } else {
                *dst++ = *src++;
            }
        }
    }
    return dstStart;
}

void* LZSSHeapDecompress(void* pCompressed, int flags)
{
    u32 size = *(u32*)pCompressed;
    void* pDst = HeapAlloc(size, (u_int)flags);
    if (pDst == NULL) {
        return NULL;
    }
    return LZSSDecompress(pCompressed, pDst);
}
