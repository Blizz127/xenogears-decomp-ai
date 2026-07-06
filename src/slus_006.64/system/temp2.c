#include "common.h"
#include "psyq/libgte.h"
#include "system/memory.h"
#ifdef XENO_PC_PORT
#include <stdio.h>
#include <stdlib.h>
#endif

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8002AC24);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8002B084);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8002B2F0);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8002B5D0);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8002B8B0);

extern s32 D_8004FE00;
extern s32 D_8004FDFC;

void func_8002BA40(void) {
    D_8004FDFC = D_8004FE00;
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8002BA58);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8002BB50);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8002BF38);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8002C310);

extern u32 g_ArchiveDebugTable;

u32 func_8002C3D8(void) {
    return g_ArchiveDebugTable;
}

/* Relocates a model's internal offset fields to absolute addresses (adds the model
 * base). Runs once per model (guarded by flag bit 0x1 at +0x4). For each of the
 * `count` (at +0x0) sub-entries (0x38 stride from +0x2C): relocate the four offset
 * fields at entry-0x14/-0x10/-0xC/-0x8, and if the entry+0x0 offset is set, relocate
 * it and walk its sub-list (0xC stride) relocating each element's +0x4/+0x8. Stores
 * are `sw` (32-bit); write truncated u32 (host RAM below 4 GiB) so pointer fields
 * stay 4 bytes -- FieldLoad's model actors read these back via func_8002C8CC. */
int func_8002C3E8(u8* pModel) {
    s32 count = *(s32*)(pModel + 0x0);
    s32 flags = *(s32*)(pModel + 0x4);
    u8* pEntry;
    s32 i;

    if (flags & 0x1) {
        return count;
    }
    *(s32*)(pModel + 0x4) = flags | 0x1;
    if (count <= 0) {
        return count;
    }

    pEntry = pModel + 0x2C;
    for (i = 0; i < count; i++) {
        *(u32*)(pEntry - 0x14) = *(u32*)(pEntry - 0x14) + (u32)pModel;
        *(u32*)(pEntry - 0x10) = *(u32*)(pEntry - 0x10) + (u32)pModel;
        *(u32*)(pEntry - 0x0C) = *(u32*)(pEntry - 0x0C) + (u32)pModel;
        *(u32*)(pEntry - 0x08) = *(u32*)(pEntry - 0x08) + (u32)pModel;

        {
            u32 subOff = *(u32*)(pEntry + 0x0);
            if (subOff != 0) {
                u8* pSub = (u8*)(u32)(subOff + (u32)pModel);
                s32 subCount;
                *(u32*)(pEntry + 0x0) = (u32)pSub;
                subCount = *(s32*)(pSub + 0x0);
                if (subCount != -1) {
                    /* a1 = pSub + 4 + subCount*0xC, walked backwards by 0xC */
                    u8* a1 = (pSub + 0x4) + (((subCount << 1) + subCount) << 2);
                    do {
                        subCount -= 1;
                        *(u32*)(a1 + 0x4) = *(u32*)(a1 + 0x4) + (u32)pModel;
                        *(u32*)(a1 + 0x8) = *(u32*)(a1 + 0x8) + (u32)pModel;
                        a1 -= 0xC;
                    } while (subCount != -1);
                }
            }
        }
        pEntry += 0x38;
    }
    return count;
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8002C4BC);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8002C59C);

/* Finalizes a model control block: pins its backing buffer into the heap so it
 * won't move. Reads a flags word at +0x4; if bit 0x2 is already set the block is
 * already pinned and it returns 1. Otherwise it sets bit 0x2, computes the
 * distance from the block base (a0) to the buffer stored at +0x24, and hands
 * that to HeapInsertAlloc to register the region. */
s32 func_8002C644(u8* a0) {
    s32 v1 = *(s32*)(a0 + 0x4);
    if ((v1 & 0x2) == 0) {
        s32 a1 = *(s32*)(a0 + 0x24);
        *(s32*)(a0 + 0x4) = v1 | 0x2;
#ifdef XENO_PC_PORT
        /* PC-port hardening: this pin is only meaningful for heap-standalone
         * models, where a0 is a real heap block and +0x24 holds an end pointer.
         * FieldLoad package models reach here mid-blob with +0x24 = baked file
         * data (packed s16 pairs, not a pointer); on PSX the resulting wild
         * HeapInsertAlloc write lands harmlessly in mirrored RAM, on the host
         * it corrupts the heap block list / faults. Validate before pinning:
         * end must lie after base within a 2 MiB (PSX RAM) bound, and the
         * heap header HeapInsertAlloc trusts (pMem[-1].pNext) must itself be
         * plausible. No heap-end API exists in memory.c, so the 2 MiB bound is
         * a documented temporary guard. Skips keep the retail control flow
         * (flag 0x2 set above, return 0) minus the junk insert. */
        {
            u32 base = (u32)(uintptr_t)a0;
            u32 end = (u32)a1;
            u32 hdrNext = (u32)(uintptr_t)((HeapBlock*)a0)[-1].pNext;
            static s32 s_skippedPins;
            static s32 s_realPins;

            if (end <= base || (end - base) >= 0x200000 ||
                hdrNext <= base || (hdrNext - base) >= 0x200000) {
                s_skippedPins++;
                if (s_skippedPins <= 4) {
                    printf("[field-diag] func_8002C644: skip invalid pin base=0x%x end=0x%x hdrNext=0x%x (skips=%d)\n",
                           base, end, hdrNext, s_skippedPins);
                }
                return 0;
            }
            s_realPins++;
            printf("[field-diag] func_8002C644: real pin base=0x%x size=%u (pins=%d)\n",
                   base, end - base, s_realPins);
        }
#endif
        HeapInsertAlloc((HeapBlock*)a0, (u_int)(a1 - (s32)a0));
        return 0;
    }
    return 1;
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8002C68C);

extern u8 D_80059598;
extern u8 D_80059599;
extern u8 D_8005959A;

void func_8002C6E0(u8 a0, u8 a1, u8 a2) {
    D_80059598 = a0;
    D_80059599 = a1;
    D_8005959A = a2;
}

extern void func_8002CCAC(void);
extern u8* D_80059424;
extern u32 D_80059538;
extern u32 D_80059528;
extern u32 D_8005952C;
extern u32 D_8005953C;
extern u32 D_80059498;
extern u32 D_800595C0;
#ifndef XENO_PC_PORT
extern u8 D_8004FE50[];
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8002C700);
#else
typedef s32 (*ModelPrimProc)(u8* pCmd, s32 count);
/* Build-pass proc, called per packet-source record by func_8002C8CC as
 * fn(D_80059538, D_80059528, shade) — mirrors the retail dispatch; the ported
 * buildProcs only consume the first arg. */
typedef s32 (*ModelPrimBuildProc)(u32 pSrc, u32 pCmd, s32 shade);

typedef struct ModelPrimDesc {
    ModelPrimProc proc[6];
    ModelPrimBuildProc buildProc;   /* host pointer; PSX addr kept in comments */
    u32 cmdStride;
    u32 packetStride;
    u32 outputStride;
} ModelPrimDesc;

extern ModelPrimDesc D_8004FE50[];
extern s32 D_80050104;
extern s32 D_80050100;
extern s32 D_800500F8;
extern s32 D_800500FC;
extern u32 D_80059568;
extern s32 D_80059578;
extern s32 func_8003101C(void);

static u32 ModelPrimVertexIndex1(u32 word) {
    return (word >> 16) & 0xFFFF;
}

static int ModelPrimPackedOverlapsScreen(u32 xy0, u32 xy1, u32 xy2, u32 xy3) {
    u32 yMaxPacked = (u32)D_800500FC;
    u32 xMax = (u32)D_800500F8;

    if (!(xy0 > yMaxPacked || xy1 > yMaxPacked || xy2 > yMaxPacked || xy3 > yMaxPacked)) {
        return 0;
    }
    return (((xy0 & 0xFFFF) < xMax) || ((xy1 & 0xFFFF) < xMax) ||
            ((xy2 & 0xFFFF) < xMax) || ((xy3 & 0xFFFF) < xMax));
}

s32 func_8002E688(u8* pCmd, s32 count) {
    const s32 packetStep = 0x28;
    const u32 tagLen = 0x09000000;
    u8* vertexBase = (u8*)(uintptr_t)D_8005953C;
    u8* out = D_80059424 - packetStep;
    u32* ot = (u32*)(uintptr_t)D_80059568;
    s32 emitted = D_80059578;

    while (count != 0) {
        u32 cmd = *(u32*)pCmd;
        SVECTOR* v0 = (SVECTOR*)(vertexBase + ((cmd & 0xFFFF) << 3));
        SVECTOR* v1 = (SVECTOR*)(vertexBase + (ModelPrimVertexIndex1(cmd) << 3));
        SVECTOR* v2 = (SVECTOR*)(vertexBase + (*(u16*)(pCmd + 0x04) << 3));
        SVECTOR* v3 = (SVECTOR*)(vertexBase + (*(u16*)(pCmd + 0x06) << 3));
        long xy0 = 0;
        long xy1 = 0;
        long xy2 = 0;
        long xy3 = 0;
        long p = 0;
        long flag = 0;

        count--;
        pCmd += 8;
        out += packetStep;

        RotTransPers4(v0, v1, v2, v3, &xy0, &xy1, &xy2, &xy3, &p, &flag);
        if (flag < 0 || p <= 0) {
            continue;
        }
        if (NormalClip(xy0, xy1, xy2) < 0) {
            continue;
        }
        if (!ModelPrimPackedOverlapsScreen((u32)xy0, (u32)xy1, (u32)xy2, (u32)xy3)) {
            continue;
        }

        {
            s32 otIndex = (s32)p >> (D_80050100 + 2);
            u32 oldTag;
            if (otIndex <= 0) {
                continue;
            }
            oldTag = ot[otIndex];
            ot[otIndex] = (u32)(uintptr_t)out & 0x00FFFFFF;
            *(u32*)(out + 0x00) = (oldTag & 0x00FFFFFF) | tagLen;
            *(u32*)(out + 0x08) = (u32)xy0;
            *(u32*)(out + 0x10) = (u32)xy1;
            *(u32*)(out + 0x18) = (u32)xy2;
            *(u32*)(out + 0x20) = (u32)xy3;
            emitted++;
        }
    }

    D_80059578 = emitted;
    D_80059424 = out + packetStep;
    return 1;
}

s32 func_8002C700(u8* a0, u8* a1, u32* a2, s32 a3) {
    s32 groupCount;

    if (D_80050104 != 0 && func_8003101C() != 0) {
        return 0;
    }

    D_80059424 = a1;
    D_80059568 = (u32)(uintptr_t)a2;
    D_800595C0 += *(u16*)(a0 + 0x4);
    D_80059528 = *(u32*)(a0 + 0x10);
    D_80059498 = *(u32*)(a0 + 0x18);
    D_8005952C = *(u32*)(a0 + 0x0C);
    D_8005953C = *(u32*)(a0 + 0x08);

    groupCount = *(u16*)(a0 + 0x06) - 1;
    if (groupCount != -1) {
        do {
            u8* pGroup = (u8*)(uintptr_t)D_80059528;
            u8 prim = pGroup[0];
            s32 count = *(s16*)(pGroup + 0x02);
            ModelPrimDesc* desc = &D_8004FE50[prim];
            ModelPrimProc proc = (a3 < 6) ? desc->proc[a3] : NULL;

            D_80059528 += 4;
            if (proc == NULL) {
                fprintf(stderr, "[xeno-port] missing D_8004FE50 prim=%u variant=%d\n", prim, a3);
                abort();
            }

            proc((u8*)(uintptr_t)D_80059528, count);
            D_80059528 += count * desc->cmdStride;
            groupCount--;
        } while (groupCount != -1);
    }

    return 1;
}
#endif

/* Builds the render command list for one model into the destination double
 * buffer half (a0=dst header, a1=src buffer, a2=mode).
 *
 * The mode arg selects a "shade/priority" code (s5) that gets packed into the
 * high halfword of the per-primitive callback's third argument; the exact code
 * is derived from `mode` combined with flag bits in *(u16*)dst (bits 0x1/0x2).
 * If the header lacks the 0x1 "packet allocated" flag it first HeapAllocs a
 * packet buffer sized by *(u32*)(dst+0x30) and stores it at dst+0x18.
 *
 * It then walks *(u16*)(dst+0x6) mesh groups; for each group it reads a run of
 * primitives from the parse cursor (D_80059528). Each primitive's first byte
 * indexes a 0x28-byte descriptor table at D_8004FE50; descriptor+0x18 is a
 * function pointer invoked as fn(D_80059538, D_80059528, s5<<16>>16). A nonzero
 * return advances the output/packet cursors by descriptor fields; a zero return
 * advances the packet cursor by 4. func_8002CCAC finalizes the pass. */
void func_8002C8CC(u8* a0, void* a1, s32 a2) {
    u8* s0 = a0;
    s32 s1 = a2;
    s32 s2;
    s32 s4;
    s32 s5 = 0;
    u16 v1_flags;

    D_80059424 = a1;

    if (((*(u16*)(s0 + 0x0) & 0x1) == 0) && (*(s32*)(s0 + 0x30) != 0) &&
        (s1 != 0)) {
        HeapSetCurrentContentType(0x26);
        *(void**)(s0 + 0x18) = HeapAlloc(*(s32*)(s0 + 0x30), 0);
        *(u16*)(s0 + 0x0) = *(u16*)(s0 + 0x0) | 0x1;
    }

    D_80059538 = *(u32*)(s0 + 0x14);
    D_80059528 = *(u32*)(s0 + 0x10);
    D_8005952C = *(u32*)(s0 + 0xC);
    D_8005953C = *(u32*)(s0 + 0x8);
    D_80059498 = *(u32*)(s0 + 0x18);
    v1_flags = *(u16*)(s0 + 0x0);

    /* mode -> s5 shade code, mirroring the branch table in the asm */
    if (s1 == 1) {
        s5 = 1;
    } else if (s1 < 2) {
        if (s1 == 0) {
            s5 = 0;
        }
        /* s1 < 0: fall through, s5 left as-is */
    } else if (s1 == 2) {
        if (v1_flags & 0x2) {
            if (v1_flags & 0x1) {
                s5 = 4;
            } else {
                s5 = 1;
            }
        } else {
            if (v1_flags & 0x1) {
                s5 = 3;
                *(u16*)(s0 + 0x0) = v1_flags | 0x2;
            } else {
                s5 = 1;
            }
        }
    } else if (s1 == 3) {
        if (v1_flags & 0x1) {
            s5 = 3;
            *(u16*)(s0 + 0x0) = v1_flags | 0x2;
        }
        /* (v1_flags & 1) == 0: s5 left as-is */
    }

    s2 = (s32)*(u16*)(s0 + 0x6) - 1;
    D_800595C0 = D_800595C0 + *(u16*)(s0 + 0x4);

    s4 = -1;
#ifdef XENO_PC_PORT
    /* Port dispatch: the host ModelPrimDesc is 0x40 bytes (64-bit proc
     * pointers), so the retail byte math below (+prim*0x28, field reads at
     * +0x18/+0x1C/+0x20/+0x24) would misindex it. Use structural access and
     * the typed host buildProc pointer instead (same migration as
     * func_8002C700's render dispatch). */
    if (s2 != s4) {
        do {
            u8* pCur = (u8*)D_80059528;
            s32 s0_cnt = (s32)*(s16*)(pCur + 0x2) - 1;
            ModelPrimDesc* desc;
            ModelPrimBuildProc fn;

            D_80059528 = (u32)(pCur + 0x4);
            {
                u32 prim = *(u8*)(pCur + 0x0);
                desc = &D_8004FE50[prim];
                if (desc->buildProc == NULL) {
                    fprintf(stderr, "[xeno-port] missing D_8004FE50 buildProc prim=%u\n", prim);
                    abort();
                }
            }
            fn = desc->buildProc;

            if (s0_cnt != s4) {
                do {
                    s32 ret = fn(D_80059538, D_80059528, (s5 << 16) >> 16);
                    if (ret != 0) {
                        D_80059528 = D_80059528 + desc->cmdStride;
                        D_80059424 = D_80059424 + desc->outputStride;
                        D_80059538 = D_80059538 + desc->packetStride;
                        s0_cnt = s0_cnt - 1;
                    } else {
                        D_80059538 = D_80059538 + 4;
                        /* s0 += 1; s0 -= 1  (net no change to counter) */
                    }
                } while (s0_cnt != s4);
            }
            s2 = s2 - 1;
        } while (s2 != s4);
    }
#else
    if (s2 != s4) {
        do {
            u8* pCur = (u8*)D_80059528;
            s32 s0_cnt = (s32)*(s16*)(pCur + 0x2) - 1;
            u8* s1_desc;
            u32 s3_fn;

            D_80059528 = (u32)(pCur + 0x4);
            {
                u32 prim = *(u8*)(pCur + 0x0);
                s1_desc = D_8004FE50 + (((prim << 2) + prim) << 3); /* *0x28 */
            }
            s3_fn = *(u32*)(s1_desc + 0x18);

            if (s0_cnt != s4) {
                do {
                    s32 (*fn)(u32, u32, s32) = (s32 (*)(u32, u32, s32))s3_fn;
                    s32 ret = fn(D_80059538, D_80059528, (s5 << 16) >> 16);
                    if (ret != 0) {
                        D_80059528 = D_80059528 + *(u32*)(s1_desc + 0x1C);
                        D_80059424 = D_80059424 + *(u32*)(s1_desc + 0x24);
                        D_80059538 = D_80059538 + *(u32*)(s1_desc + 0x20);
                        s0_cnt = s0_cnt - 1;
                    } else {
                        D_80059538 = D_80059538 + 4;
                        /* s0 += 1; s0 -= 1  (net no change to counter) */
                    }
                } while (s0_cnt != s4);
            }
            s2 = s2 - 1;
        } while (s2 != s4);
    }
#endif

    func_8002CCAC();
}

/* Allocates the per-model double buffer. modelData+0x34 is the model's byte
 * size; it allocates 2x that, hands out1 (first half) via out1 and out2 (second
 * half, base+size) via out2. Callers fill out1 then memcpy into out2. */
/* out1/out2 point at PSX 4-byte pointer slots in the caller's model control block
 * (misc3.c pModel+0x8/+0xC). The asm stores them with `sw` (32-bit); write u32
 * (truncated host pointer, lossless below 4 GiB) so we don't clobber the adjacent
 * slot the way an 8-byte `void*` store would. */
void func_8002CB54(u8* modelData, u32* out1, u32* out2) {
    void* buf;
    HeapSetCurrentContentType(0x25);
    buf = HeapAlloc(*(s32*)(modelData + 0x34) << 1, 0);
    *out1 = (u32)buf;
    *out2 = (u32)((u8*)buf + *(s32*)(modelData + 0x34));
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8002CBBC);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8002CC10);

extern s32 D_80059310;
extern s32 D_80050108;

void func_8002CC54(u16 a0) {
    D_80059310 = a0;
    D_80050108 = 2;
}

extern u_short GetClut(int x, int y);
extern s32 D_80059314;
extern s32 D_8005010C;

void func_8002CC74(u16 a0, u16 a1) {
    D_80059314 = GetClut(a0, a1) & 0xFFF0;
    D_8005010C = 0;
}

extern s32 D_8005010C;

void func_8002CCAC(void) {
    D_80050108 = 0;
    D_8005010C = 1;
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8002CCC8);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8002CD24);

/* Gate shared by the textured model buildProcs (func_8002D984/func_8002D0E4):
 * packet-source records whose code byte (+0x3) is a 0xC0-class inline state
 * command dispatch to a state handler and emit no primitive (return 0);
 * anything else emits (return 1). func_8002CCC8/func_8002CD24 are still
 * INCLUDE_ASM -> auto-stubs; a "[stub]" log on those names means Map1 models
 * carry 0xC4/0xC8 inline state commands and the handlers must be ported. */
extern void func_8002CCC8(void);
extern void func_8002CD24(void);

s32 func_8002CD64(u8* pSrc) {
    u8 code = pSrc[0x3];

    if ((code & 0xF0) != 0xC0) {
        return 1;
    }
    if (code == 0xC4) {
        func_8002CCC8();
        return 0;
    }
    if (code == 0xC8) {
        func_8002CD24();
        return 0;
    }
    return 1;
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8002CDCC);

extern u8* D_80059424;

s32 func_8002CF34(s32* a0) {
    u8* p = D_80059424;
    p[3] = 4;
    *(s32*)(p + 4) = *a0;
    return 1;
}

/* buildProc for prim 0x08 (lit flat tri template, tag len 4). Four paths by
 * shade mode: unlit (copy rgb|code word), lit (face normal via func_8002DB84 +
 * NormalLightCol into packet color), lit+stream (also store rgb and normals to
 * the D_80059498 work stream), and pre-lit stream (shade&4). func_8002DB84 and
 * NormalLightCol are still INCLUDE_ASM -> auto-stubs; "[stub]" logs on those
 * names mean a lit model path is active and they must be ported for correct
 * colors (non-fatal until then). asm: func_8002CF58.s. */
extern void func_8002DB84(void* n0, void* n1, void* n2);
extern void NormalLightCol(void* a0, void* a1, void* a2);

s32 func_8002CF58(u8* pSrc, u8* pCmd, s32 shade) {
    u8* p = D_80059424;
    u8 tmpNormal[8]; /* asm passes uninitialized sp+0x10 here; kept faithful */

    p[0x3] = 0x4;
    if (shade & 0x1) {
        u8* vb = (u8*)(uintptr_t)D_8005953C;
        void* n0 = vb + ((s32)*(s16*)(pCmd + 0x0) << 3);
        void* n1 = vb + ((s32)*(s16*)(pCmd + 0x2) << 3);
        void* n2 = vb + ((s32)*(s16*)(pCmd + 0x4) << 3);

        if (shade & 0x2) {
            *(u32*)(uintptr_t)D_80059498 = *(u32*)pSrc;
            D_80059498 += 4;
            func_8002DB84(n0, n1, n2);
            NormalLightCol((void*)(uintptr_t)D_80059498, pSrc, p + 0x4);
            D_80059498 += 8;
        } else {
            func_8002DB84(n0, n1, n2);
            NormalLightCol(tmpNormal, pSrc, p + 0x4);
        }
        p[0x7] = pSrc[0x3];
    } else if (shade & 0x4) {
        D_80059498 += 4;
        NormalLightCol((void*)(uintptr_t)D_80059498, pSrc, p + 0x4);
        D_80059498 += 8;
        p[0x7] = pSrc[0x3];
    } else {
        *(u32*)(p + 0x4) = *(u32*)pSrc;
    }
    return 1;
}

s32 func_8002D0C0(s32* a0) {
    u8* p = D_80059424;
    p[3] = 5;
    *(s32*)(p + 4) = *a0;
    return 1;
}

/* buildProc for prim 0x0D (textured quad, POLY_FT4 template, tag len 9).
 * Reads one 0x0C-byte packet-source record (a0 = D_80059538 cursor) and writes
 * the static packet fields at the D_80059424 output cursor; the per-frame render
 * proc (func_8002E688) later patches the projected xy words in. asm:
 * func_8002D984.s sibling — p[3]=9; +0x4 = rgb|code word; +0xC = uv0 | clut<<16;
 * +0x14 = uv1 | tpage<<16; +0x1C = uv2; +0x24 = uv3. */
extern u16 D_80059308;  /* current model texture page (hi half of packet +0x14) */
extern u16 D_8005930C;  /* current model CLUT id      (hi half of packet +0x0C) */

s32 func_8002D0E4(u8* pSrc) {
    u8* p;

    if (func_8002CD64(pSrc) == 0) {
        return 0;
    }
    p = D_80059424;
    p[0x3] = 0x9;
    *(u32*)(p + 0x04) = *(u32*)(pSrc + 0x0);
    *(u32*)(p + 0x0C) = *(u16*)(pSrc + 0x4) | ((u32)D_8005930C << 16);
    *(u32*)(p + 0x14) = *(u16*)(pSrc + 0x6) | ((u32)D_80059308 << 16);
    *(u16*)(p + 0x1C) = *(u16*)(pSrc + 0x8);
    *(u16*)(p + 0x24) = *(u16*)(pSrc + 0xA);
    return 1;
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8002D180);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8002D244);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8002D354);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8002D420);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8002D530);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8002D6AC);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8002D77C);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8002D814);

/* buildProc for prim 0x05 (textured tri, POLY_FT3 template, tag len 7).
 * Reads one 0x08-byte packet-source record and writes the static packet fields;
 * the render proc (ModelPrimTriVariant0) later patches the xy words. asm:
 * p[3]=7; +0xC = uv0 | clut<<16; +0x14 = uv1 | tpage<<16; +0x1C = uv2;
 * p[0x7] = code byte (rgb bytes at +0x4..+0x6 are owned by the render pass). */
s32 func_8002D984(u8* pSrc) {
    u8* p;

    if (func_8002CD64(pSrc) == 0) {
        return 0;
    }
    p = D_80059424;
    p[0x3] = 0x7;
    *(u32*)(p + 0x0C) = *(u16*)(pSrc + 0x4) | ((u32)D_8005930C << 16);
    *(u32*)(p + 0x14) = *(u16*)(pSrc + 0x6) | ((u32)D_80059308 << 16);
    *(u16*)(p + 0x1C) = *(u16*)(pSrc + 0x0);
    p[0x7] = pSrc[0x3];
    return 1;
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8002DA14);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8002DAFC);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8002DB84);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8002DC9C);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8002DD20);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8002DDE4);

extern u8 D_8006FAF0[];

void* func_8002DFE0(void) {
    return D_8006FAF0;
}

extern s32 D_800500FC;
extern s32 D_800500F8;

void func_8002DFF0(s32 a0, s32 a1) {
    D_800500FC = (a1 - 1) << 16;
    D_800500F8 = a0;
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8002E010);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8002E448);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8002E64C);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8002E8B4);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8002EAB8);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8002ED20);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8002EEF8);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8002F0E4);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8002F2E0);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8002F4B4);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8002F6B4);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8002F8D0);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8002FAE8);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8002FCFC);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8002FF0C);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8003014C);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_800301C8);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_80030228);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_800302D4);

extern void func_8003014C();

/* Allocates and populates the "animation/joint" work block for a model that has
 * the 0x2000 (skeletal) status bit. a0=modelData, a1=heap flags (0).
 *
 * Reads a descriptor pointer at modelData+0x1C; if null, returns 0. Otherwise
 * allocs a control block sized (desc[0]<<5)|0x14 and back-links modelData into
 * it (+0x0), copies two source pointers (+0x4,+0x8) and the count word (+0xC).
 * A vertex table of (*(u16*)(modelData+2)) 8-byte entries is allocated at
 * modelData+0x8 and its first three u16 fields are copied from the block's +0x4
 * array; if status bit 0x10 is set the same is done for a normal table at
 * modelData+0xC from the block's +0x8 array. Finally it initializes desc[0]
 * per-item 0x20-byte slots (starting at block+0x14) with the func_8003014C
 * callback and three zeroed words. Returns the control block. */
void* func_800303C8(u8* a0, s32 a1) {
    u8* s0 = a0;
    s32 s4 = a1;
    u8* s2 = (u8*)*(s32*)(s0 + 0x1C);
    u8* s1;
    u8* s3;
    s32 a5;

    if (s2 == NULL) {
        return NULL;
    }

    HeapSetCurrentContentType(0x2B);
    s1 = HeapAlloc((*(s32*)(s2 + 0x0) << 5) | 0x14, s4);
    *(s32*)(s1 + 0x0) = (s32)s0;
    *(s32*)(s1 + 0x4) = *(s32*)(s0 + 0x8);
    s3 = s1 + 0x14;
    *(s32*)(s1 + 0x10) = (s32)s3;
    *(s32*)(s1 + 0x8) = *(s32*)(s0 + 0xC);
    *(s32*)(s1 + 0xC) = *(s32*)(s2 + 0x0);

    HeapSetCurrentContentType(0x2C);
    *(s32*)(s0 + 0x8) = (s32)HeapAlloc(*(u16*)(s0 + 0x2) << 3, s4);
    {
        s32 i = (s32)*(u16*)(s0 + 0x2) - 1;
        for (; i != -1; i--) {
            s32 off = i << 3;
            u8* src = (u8*)*(s32*)(s1 + 0x4) + off;
            u8* dst = (u8*)*(s32*)(s0 + 0x8) + off;
            *(u16*)(dst + 0x0) = *(u16*)(src + 0x0);
            *(u16*)(dst + 0x2) = *(u16*)(src + 0x2);
            *(u16*)(dst + 0x4) = *(u16*)(src + 0x4);
        }
    }

    if (*(u16*)(s0 + 0x0) & 0x10) {
        HeapSetCurrentContentType(0x2D);
        *(s32*)(s0 + 0xC) = (s32)HeapAlloc(*(u16*)(s0 + 0x2) << 3, s4);
        {
            s32 i = (s32)*(u16*)(s0 + 0x2) - 1;
            for (; i != -1; i--) {
                s32 off = i << 3;
                u8* src = (u8*)*(s32*)(s1 + 0x8) + off;
                u8* dst = (u8*)*(s32*)(s0 + 0xC) + off;
                *(u16*)(dst + 0x0) = *(u16*)(src + 0x0);
                *(u16*)(dst + 0x2) = *(u16*)(src + 0x2);
                *(u16*)(dst + 0x4) = *(u16*)(src + 0x4);
            }
        }
    }

    if (*(s32*)(s1 + 0xC) > 0) {
        for (a5 = 0; a5 < *(s32*)(s1 + 0xC); a5++) {
            *(void**)(s3 + 0x0) = (void*)func_8003014C;
            *(s32*)(s3 + 0x4) = 0;
            *(s32*)(s3 + 0x8) = 0;
            *(s32*)(s3 + 0xC) = 0;
            s3 += 0x20;
        }
    }

    return s1;
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_800305D8);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_800306D0);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_80030750);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_80030988);

MATRIX D_80059F64;
MATRIX D_80059F84;

static s16 func_80030A18(s64 value) {
    value >>= 12;
    if (value > 0x7FFF) {
        return 0x7FFF;
    }
    if (value < -0x8000) {
        return -0x8000;
    }
    return value;
}

void func_80030A30(s32 lightId, void* pLight) {
    VECTOR lightDirection;
    u16 id = lightId;
    s16* pLightMatrix = (s16*)&D_80059F64;
    s16* pColorMatrix = (s16*)&D_80059F84;

    lightDirection.vx = -*(s32*)((u8*)pLight + 0x0);
    lightDirection.vy = -*(s32*)((u8*)pLight + 0x4);
    lightDirection.vz = -*(s32*)((u8*)pLight + 0x8);
    VectorNormalS(&lightDirection, (SVECTOR*)(pLightMatrix + (id * 3)));

    pColorMatrix[id] = *(u16*)((u8*)pLight + 0xC);
    pColorMatrix[id + 3] = *(u16*)((u8*)pLight + 0xE);
    pColorMatrix[id + 6] = *(u16*)((u8*)pLight + 0x10);
    SetColorMatrix(&D_80059F84);
}

void func_80030B14(MATRIX* pMatrix) {
    MATRIX matrix;
    s32 row;
    s32 col;

    for (row = 0; row < 3; row++) {
        for (col = 0; col < 3; col++) {
            matrix.m[row][col] = func_80030A18(
                (s64)D_80059F64.m[row][0] * pMatrix->m[0][col] +
                (s64)D_80059F64.m[row][1] * pMatrix->m[1][col] +
                (s64)D_80059F64.m[row][2] * pMatrix->m[2][col]);
        }
    }
    SetLightMatrix(&matrix);
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_80030C40);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_80030C78);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_80030C98);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_80030EE8);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8003101C);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_800315A0);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_800315C4);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_800315E8);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8003160C);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_80031630);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_80031654);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_80031678);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8003169C);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_800316C0);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_800316E4);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_80031708);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8003172C);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_80031750);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_80031774);

void func_80031798(void* ot, void* prim) {
    u32 primAddr = (u32)(uintptr_t)prim & 0x00FFFFFF;
    u32 old = *(u32*)ot;

    *(u32*)ot = primAddr;
    *(u32*)(uintptr_t)primAddr = old | 0x04000000;
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_800317BC);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_800317E0);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_80031804);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_80031828);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8003184C);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_80031870);
