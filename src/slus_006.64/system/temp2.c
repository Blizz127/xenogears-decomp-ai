#include "common.h"
#include "system/memory.h"

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

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8002C3E8);

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

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8002C700);

extern void func_8002CCAC(void);
extern u8* D_80059424;
extern u32 D_80059538;
extern u32 D_80059528;
extern u32 D_8005952C;
extern u32 D_8005953C;
extern u32 D_80059498;
extern u32 D_800595C0;
extern u8 D_8004FE50[];

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

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8002CD64);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8002CDCC);

extern u8* D_80059424;

s32 func_8002CF34(s32* a0) {
    u8* p = D_80059424;
    p[3] = 4;
    *(s32*)(p + 4) = *a0;
    return 1;
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8002CF58);

s32 func_8002D0C0(s32* a0) {
    u8* p = D_80059424;
    p[3] = 5;
    *(s32*)(p + 4) = *a0;
    return 1;
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8002D0E4);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8002D180);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8002D244);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8002D354);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8002D420);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8002D530);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8002D6AC);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8002D77C);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8002D814);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8002D984);

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

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_80030A30);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_80030B14);

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

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_80031798);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_800317BC);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_800317E0);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_80031804);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_80031828);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_8003184C);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/temp2", func_80031870);
