#include "common.h"
#ifndef XENO_PC_PORT
void AddPrim(s32, void *);
extern void *D_800C3E24;
extern s32 D_800CCB04;
extern void * D_800D2D28;
void func_80034888(s32, s32, s32);
extern s32 D_800CCB34;
extern s32 D_800D2DAC;
extern void *D_800D3278;
void func_80074D4C(void);
void func_80074F70(void);
#endif

#ifndef XENO_PC_PORT
#include "system/memory.h"
#endif


#ifndef XENO_PC_PORT
void func_800728B8(void *prims, s32 count, s32 start);
extern void *D_800D2D28;
extern void *D_800D2DB4;
void func_80074AB8(void);
void func_80074EEC(void);
#endif

#ifndef XENO_PC_PORT
extern u8 D_800D2CAF;
extern u16 D_800D2C94;
extern u16 D_800C48E8;
extern u8* D_800C3EAC;
extern u8 D_800D3014;
extern void func_80079E18(u8 index);
extern void func_80079E4C(u8 index);
extern void func_800716D8(void);

/* func_80071AE0.s: when D_800D2CAF is set and D_800C48E8 masks nothing in
 * D_800D2C94 (func_80071964's test): func_80079E18(7), clear byte 0x2DB
 * of D_800C3EAC, run the battle frame (func_800716D8) up to 59 times while
 * D_800D3014 stays 8, set byte 0x2DB back to 1, func_80079E4C(7) and run
 * one more frame.  First draft. */
void func_80071AE0(void) {
    s32 n;

    if (D_800D2CAF != 0 && (D_800D2C94 & D_800C48E8) == 0) {
        func_80079E18(7);
        n = 0x3B;
        D_800C3EAC[0x2DB] = 0;
        do {
            func_800716D8();
            if (D_800D3014 != 8) {
                break;
            }
        } while (--n != 0);
        D_800C3EAC[0x2DB] = 1;
        func_80079E4C(7);
        func_800716D8();
    }
}
INCLUDE_ASM("asm/battle/nonmatchings/main2", func_80071B94);
extern u16 D_800D2C9E;
extern u16 D_800D2E06[];
extern u8 D_800D2DE4[];
extern u16 func_80089C9C(s32 mask, u8 idx);

/* func_80072270.s: when D_800D2C9E has any of its low three bits set, for
 * each party actor i < 3 that func_80089C9C(D_800D2C9E, i) selects: clear
 * D_800D2DE4[i], copy the u16 eleven slots back (D_800D2E06[i - 11]) into
 * D_800D2E06[i] and set byte +0x7C + i of D_800D2D28; then clear
 * D_800D2C9E.  The D_800D2D28 byte is addressed as (base + i)[0x7C], which
 * gives retail's base-first addu. */
void func_80072270(void) {
    s32 i;

    if (D_800D2C9E & 7) {
        for (i = 0; i < 3; i++) {
            if (func_80089C9C(D_800D2C9E, i) != 0) {
                D_800D2DE4[i] = 0;
                D_800D2E06[i] = D_800D2E06[i - 11];
                ((u8*)D_800D2D28 + i)[0x7C] = 1;
            }
        }
    }
    D_800D2C9E = 0;
}
extern u16 D_800D39E0;
extern u8 D_800C402F[];
extern u8 D_800D2E5C;
extern u8 D_800D2E60;
extern u16 func_80089C9C(s32 mask, u8 idx);
extern void func_80085AC4(u8 a0);
extern void func_80071B94(s32 a0);

/* func_80072324.s: for each enemy actor i in 3..10 that
 * func_80089C9C(D_800D39E0, i) selects: clear 256 bytes, reset the 32
 * D_800C402F command bytes to 0xFF (as func_800792F8), set D_800D2E5C = 4
 * and D_800D2E60 = 0x17, then call func_80085AC4(i) and func_80071B94(1).
 * ORIGINAL BUG, kept on purpose: the 256-byte clear goes through p, which
 * is never assigned (retail clears from whatever $s2 holds on entry; its
 * caller func_800723E0 does not set $s2 either), apparently meant to be
 * D_800D2E5C (func_800799C8 clears that block the same way).  The bound is
 * computed from the running copy q, so loop.c does not hoist it into
 * another saved register; 0xFF is a constant in the store, which loop.c
 * hoists after the i setup, as in retail. */
void func_80072324(void) {
    s32 i;
    s32 k;
    u8* p;
    u8* q;
    u8* end;

    for (i = 3; i < 11; i++) {
        if (func_80089C9C(D_800D39E0, i) != 0) {
            q = p;
            end = q + 0x100;
            do {
                *q++ = 0;
            } while (q < end);
            for (k = 0x8B8; k >= 0; k -= 0x48) {
                D_800C402F[k] = 0xFF;
            }
            D_800D2E5C = 4;
            D_800D2E60 = 0x17;
            func_80085AC4(i);
            func_80071B94(1);
        }
    }
}
INCLUDE_ASM("asm/battle/nonmatchings/main2", func_800723E0);
typedef struct {
    u16 unk0;
    u8 unk2[0x2E];
    u16 unk30;
    u16 unk32;
    u16 unk34;
    u8 unk36[0x82];
    u32 unkB8;
    u8 unkBC[0x18];
    u16 unkD4;
    u8 unkD6[0x9A];
} M2Actor; /* 0x170 */
typedef struct {
    u8 unk0[3];
    u8 unk3;
    u8 unk4;
    u8 unk5[0x17];
} M2Group; /* 0x1C */
extern M2Actor D_800CCD34[];
extern M2Group D_800C3EB4[];
extern u8 D_800D2DCC[];
extern u8 D_800D2DE4[];
extern u16 D_800D39DC;
extern u16 D_800C3608;
extern u8 D_800C48EA[];
extern u8 D_800C3D0C__b[] asm("D_800C3D0C");
extern u16 func_80089C08(u8 idx);
extern u16 func_80089C9C(s32 mask, u8 idx);
extern u16 func_80089C48(u8 idx);

/* Party (0..2) and enemy (3..10) status scan: sets D_800D39DC bits and the
 * battle-end flag D_800C48EA.  The enemy loop walks explicit byte offsets:
 * `ao` = i * sizeof(M2Actor), `go` = i * sizeof(M2Group), and the mask loop
 * reuses `go` as its actor offset.  Retail's registers need that reuse
 * (the shared offset outranks `k` in global alloc) and the for-init order;
 * the volatile D_800D2DE4 stores keep loop.c from turning the two stores
 * into a pointer giv. */
void func_8007252C(void) {
    s32 i;
    s32 k;
    s32 mask;
    u8* q;
    s32 ff;
    s32 go;
    s32 ao;

    D_800D39DC = 0;
    for (i = 0; i < 3; i++) {
        if (D_800D2DCC[i] != 0) {
            if (D_800CCD34[i].unk30 & 0xC000) {
                if (D_800CCD34[i].unk30 & 0x8000) {
                    D_800CCD34[i].unk0 = 0;
                }
                D_800D2DE4[i] = 0xFF;
            } else if (D_800C3EB4[i].unk3 == 0) {
                D_800D39DC |= func_80089C08(i);
            }
        }
    }
    for (i = 3, ff = 0xFF, q = D_800C3D0C__b + 15, k = 0, ao = 0x450, go = 0x54; i < 11; q += 4, k++, ao += 0x170, i++, go += 0x1C) {
        if (D_800D2DCC[k + 3] != 0) {
            if (((u8*)D_800C3EB4 + 4)[go] == 0) {
                if (*(u16*)((u8*)D_800CCD34 + 0x30 + ao) & 0xC000) {
                    if (func_80089C9C(D_800C3608, i) == 0) {
                        *(u16*)((u8*)D_800CCD34 + ao) = 0;
                        *(volatile u8*)&D_800D2DE4[k + 3] = ff;
                        continue;
                    }
                    D_800D39DC |= func_80089C08(i);
                    continue;
                }
            } else {
                if (*(u16*)((u8*)D_800CCD34 + 0xD4 + ao) & 0xC000) {
                    if (func_80089C9C(D_800C3608, i) == 0) {
                        *(u32*)((u8*)D_800CCD34 + 0xB8 + ao) = 0;
                        *(volatile u8*)&D_800D2DE4[k + 3] = ff;
                        continue;
                    }
                    D_800D39DC |= func_80089C08(i);
                    continue;
                }
            }
            if (((u8*)D_800C3EB4 + 3)[go] == 0 && *q == 0) {
                D_800D39DC |= func_80089C08(i);
            }
        }
    }
    if (!(D_800D39DC & 0x7F8)) {
        D_800C48EA[0] = 1;
    }
    if (!(D_800D39DC & 0x7)) {
        D_800C48EA[0] = 0x81;
    }
    if (D_800C48EA[0] == 0) {
        mask = D_800D39DC;
        for (i = 0, go = 0; i < 11; i++, go += 0x170) {
            if (func_80089C9C(mask, i) && (*(u16*)((u8*)D_800CCD34 + 0x34 + go) & 0x1000)) {
                mask &= func_80089C48(i);
            }
        }
        if (mask == 0) {
            for (i = 0; i < 11; i++) {
                if (func_80089C9C(D_800D39DC, i) && (D_800CCD34[i].unk34 & 0x1000)) {
                    D_800CCD34[i].unk34 &= ~0x1000;
                    return;
                }
            }
        }
    }
}

#ifndef XENO_PC_PORT
/* Link `count` primitives of 0x28 bytes, every other one from `start`. */
void func_800728B8(void* prims, s32 count, s32 start) {
    s32 i;

    for (i = 0; i < count; i++) {
        AddPrim(D_800CCB04 + 4, (u8*)prims + (start + i * 2) * 0x28);
    }
}
#endif

#ifndef XENO_PC_PORT
extern void SetShadeTex(void* p, s32 tge);


/* func_80072938.s, byte-exact WITH A CONSTRUCT: for each primitive i from
 * start to end - 1, SetShadeTex(prims + (i * 2 + D_800CCB34) * 0x28, 0)
 * and set its colour bytes +4/+5/+6 to (0x80, 0, 0), or (0x80, 0x80, 0) for
 * mode 1.  Construct (decomp-permuter result, not the original code): the
 * last store sits in `if (end) ... else ...` with identical arms; the
 * branch only adds the block boundary that gives retail's register
 * allocation (without it the last block's registers differ: 348 vs 356).
 * Counted apart from natural matches. */
void func_80072938(void* prims, s32 start, s32 end, u8 mode) {
    s32 i;
    s32 k;

    for (i = start; i < end; i++) {
        k = i * 2;
        SetShadeTex((u8*)prims + (k + D_800CCB34) * 0x28, 0);
        if (mode != 1) {
            ((u8*)prims + (k + D_800CCB34) * 0x28)[4] = 0x80;
            ((u8*)prims + (k + D_800CCB34) * 0x28)[5] = 0;
        } else {
            ((u8*)prims + (k + D_800CCB34) * 0x28)[4] = 0x80;
            ((u8*)prims + (k + D_800CCB34) * 0x28)[5] = 0x80;
        }
        /* CONSTRUCT (not the original code): the two identical arms only add
         * the block boundary retail's register allocation implies. */
        if (end) {
            ((u8*)prims + (k + D_800CCB34) * 0x28)[6] = 0;
        } else {
            ((u8*)prims + (k + D_800CCB34) * 0x28)[6] = 0;
        }
    }
}
#endif
INCLUDE_ASM("asm/battle/nonmatchings/main2", func_80072A9C);
extern void* D_800C3EA4;
extern u16 D_800C3254[];
extern u8 D_800D3280;
extern u8 D_800D2D88[];
extern u16 D_800C3076[];
extern s32 func_80076A10(s32 index, u8* polys, s16 x, s16 y);

/* func_80072DA8.s, byte-exact: draw the five-glyph label of slot `slot`.
 * For each D_800D2D88 glyph that is not 0xFF, func_80076A10(glyph + 0x67,
 * the slot's prims at D_800C3EA4 + 0x3A88 + slot * 0xC80 + count * 80,
 * x = D_800C3076[slot * 24 + i] + D_800C3254[D_800D3280 * 3 + slot], y =
 * 0x10) adds its sprite count to the slot's D_800D2D28 +0xE0 word.  With
 * `flag`, func_80072938 then colours the prims from the old count to the
 * new one.  Codegen: the record offset is parenthesised (retail hoists
 * slot * 0xC80 + 0x3A88 as one value); func_80076A10 takes an s32 glyph
 * as its main4 definition does (main2 declared u8, which masks the
 * argument; retail passes glyph + 0x67 unmasked). */
void func_80072DA8(s32 slot, u8 flag) {
    s32 start;
    s32 i;

    start = *(s32*)((u8*)D_800D2D28 + slot * 4 + 0xE0);
    for (i = 0; i < 5; i++) {
        if (D_800D2D88[i] != 0xFF) {
            *(s32*)((u8*)D_800D2D28 + slot * 4 + 0xE0) += func_80076A10(D_800D2D88[i] + 0x67,
                (u8*)D_800C3EA4 + (slot * 0xC80 + 0x3A88) + *(s32*)((u8*)D_800D2D28 + slot * 4 + 0xE0) * 80,
                D_800C3076[slot * 24 + i] + D_800C3254[D_800D3280 * 3 + slot], 0x10);
        }
    }
    if (flag) {
        func_80072938((u8*)D_800C3EA4 + (slot * 0xC80 + 0x3A88), start, *(s32*)((u8*)D_800D2D28 + slot * 4 + 0xE0), flag);
    }
}
INCLUDE_ASM("asm/battle/nonmatchings/main2", func_80072F38);
INCLUDE_ASM("asm/battle/nonmatchings/main2", func_80073380);
extern void* D_800C3EA4;
typedef struct {
    u8 state;
    u8 unk1[0x1B];
} BattleSlotState;
extern BattleSlotState D_800C3EB6__s[] asm("D_800C3EB6");
extern u8 D_800D32A1[];
extern u16 D_800C3254[];
extern u8 D_800D3280;
extern void* D_800D2F5C;
extern s32 D_800CCB34__a[] asm("D_800CCB34");
extern u8 D_800D2D24[];
extern s32 func_80076A10(s32 index, u8* polys, s16 x, s16 y);
extern s32 func_80076A6C(u8 glyph, u8* prims, s16 x, s16 y);
extern void func_80076C34(u8* prim);
extern s32 func_80025FA8(void* font, s32 glyph, u8* prims, s32 buf, s32 x, s32 y, s32 sx, s32 sy, s32 a8);
extern void func_800765C4(s32 slot);
extern void func_80076710(s32 slot);

/* Party HUD per-slot state switch.  Shapes retail's codegen depends on:
 * `base` holds D_800D2D28 before `+ i` (retail adds base first);
 * D_800D32A1[i << 3] becomes its own loop.c pointer giv (la init, += 8);
 * the D_800CCB34 call arguments are read through the split-array alias,
 * which gives retail's a3-first argument loads. */
void func_80073538(void) {
    s32 i;
    s32 k;
    s32 scale = 0;
    u8* p;
    u8* base;

    *(u8*)(D_800D2D28 + 0x77) = 0;
    for (i = 0; i < 3; i++) {
        if (D_800C3EB6__s[i].state == 0x7F) {
            continue;
        }
        switch ((p = (base = (u8*)D_800D2D28) + i)[0x90]) {
        case 0:
            p[0x74] = 0;
            *(u8*)(D_800D2D28 + i + 0x74) +=
                func_80076A10(0x8F, D_800C3EA4 + 0x2E08 + i * 0x320 + *(u8*)(D_800D2D28 + i + 0x74) * 0x50,
                              i * 0x60 + (D_800C3254[D_800D3280 * 3 + i] + 0x44), 0x1F);
            *(u8*)(D_800D2D28 + i + 0x74) +=
                func_80076A6C(0x52, D_800C3EA4 + 0x2E08 + i * 0x320 + *(u8*)(D_800D2D28 + i + 0x74) * 0x50,
                              i * 0x60 + (D_800C3254[D_800D3280 * 3 + i] + 0x48), 0x1C);
            k = *(u8*)(D_800D2D28 + i + 0x74) * 2;
            *(u8*)(D_800D2D28 + i + 0x74) +=
                func_80076A6C(0x53, D_800C3EA4 + 0x2E08 + i * 0x320 + *(u8*)(D_800D2D28 + i + 0x74) * 0x50,
                              i * 0x60 + (D_800C3254[D_800D3280 * 3 + i] + 0x48), 0x1C);
            for (; k < *(u8*)(D_800D2D28 + i + 0x74) * 2; k += 2) {
                func_80076C34(D_800C3EA4 + 0x2E08 + i * 0x320 + (k + D_800CCB34) * 0x28);
            }
            *(u8*)(D_800D2D28 + i + 0x84) = D_800CCB34;
            *(u8*)(D_800D2D28 + i + 0xCC) = 1;
            break;
        case 1:
            scale = 0xC00;
            *(u8*)(D_800D2D28 + 0x77) =
                func_80025FA8(D_800D2F5C, 0x52, D_800C3EA4 + 0x3768, D_800CCB34__a[0], 0x10, 0x98, 0x1000, 0x1000, scale);
            k = *(u8*)(D_800D2D28 + 0x77) * 2;
            *(u8*)(D_800D2D28 + 0x77) += func_80025FA8(D_800D2F5C, 0x53,
                                                      D_800C3EA4 + 0x3768 + *(u8*)(D_800D2D28 + 0x77) * 0x50,
                                                      D_800CCB34__a[0], 0x10, 0x98, 0x1000, 0x1000, scale);
            for (; k < *(u8*)(D_800D2D28 + 0x77) * 2; k += 2) {
                func_80076C34(D_800C3EA4 + 0x3768 + (k + D_800CCB34) * 0x28);
            }
            *(u8*)(D_800D2D28 + 0x87) = D_800CCB34;
            break;
        case 3:
            if (D_800D32A1[i << 3] != 0 && D_800D2D24[i] != 7) {
                p[0xCC] = 0;
            } else {
                p[0x74] = 0;
            }
            func_800765C4(i);
        case 2:
            func_80076710(i);
            if (D_800D32A1[i << 3] == 0 || D_800D2D24[i] == 7) {
                *(u8*)(D_800D2D28 + i + 0x7F) = 0;
            }
            break;
        }
    }
}
extern void *D_800C3EA4;
extern s32 D_800CCB04__a_s32[] asm("D_800CCB04");

void func_80073A58(void) {
    if (*(u8 *)((s8*)(D_800C3EA4) + 0x6415) != 0) {
        *(u8 *)((s8*)(((*(u8 *)((s8*)(D_800C3EA4) + 0x6414) * 0x18) + D_800C3EA4)) + 0x63CC) = (u8) *(u8 *)((s8*)(D_800C3EA4) + 0x6410);
        *(u8 *)((s8*)(((*(u8 *)((s8*)(D_800C3EA4) + 0x6414) * 0x18) + D_800C3EA4)) + 0x63CD) = (u8) *(u8 *)((s8*)(D_800C3EA4) + 0x6410);
        *(u8 *)((s8*)(((*(u8 *)((s8*)(D_800C3EA4) + 0x6414) * 0x18) + D_800C3EA4)) + 0x63CE) = (u8) *(u8 *)((s8*)(D_800C3EA4) + 0x6410);
        AddPrim(D_800CCB04__a_s32[0] + 4, (*(u8 *)((s8*)(D_800C3EA4) + 0x6414) * 0x18) + 0x63C8 + D_800C3EA4);
        AddPrim(D_800CCB04__a_s32[0] + 4, (*(u8 *)((s8*)(D_800C3EA4) + 0x6414) * 0xC) + 0x63F8 + D_800C3EA4);
    }
}

INCLUDE_ASM("asm/battle/nonmatchings/main2", func_80073B64);
extern void *D_800C3EA4;

void func_80073E88(void) {
    s32 i;

    if (*(u8 *)((s8*)(D_800D2D28) + 0xCB) != 0) {
        for (i = 0; i < 2; i++) {
            func_800728B8(D_800C3EA4 + 0x641C + i * 0xFA0, *(s32 *)((s8*)(D_800D2D28) + 0xD0 + i * 4), *(u8 *)((s8*)(D_800D2D28) + 0xA3));
        }
    }
}

extern void *D_800C3EA4;

void func_80073F08(void) {
    if (*(u8 *)((s8*)(D_800D2D28) + 0x9C) != 0) {
        func_800728B8(D_800C3EA4 + 0xBA8, *(s32 *)((s8*)(D_800D2D28) + 0xF8), *(u8 *)((s8*)(D_800D2D28) + 0xA5));
    }
    if (*(u8 *)((s8*)(D_800D2D28) + 0x9E) != 0) {
        func_800728B8(D_800C3EA4 + 0x27C8, *(s32 *)((s8*)(D_800D2D28) + 0x100), *(u8 *)((s8*)(D_800D2D28) + 0xA7));
    }
    if (*(u8 *)((s8*)(D_800D2D28) + 0x9D) != 0) {
        func_800728B8(D_800C3EA4 + 0x1E68, *(s32 *)((s8*)(D_800D2D28) + 0xFC), *(u8 *)((s8*)(D_800D2D28) + 0xA6));
    }
}

extern u8 D_800C3E4C;
extern u8* D_800D2E38[];

/* Link prims into the OT at D_800CCB04 + 4: the D_800C3EA4 +0x8920 prim
 * for buffer D_800CCB34 (0xC stride); while D_800C3E4C is 1, a 0x34-byte
 * prim for each of the four slots whose D_800D2D28 +0x7F flag is set; the
 * +0x8908 prim; then for each of the seven D_800D2E38 records whose
 * D_800D2D28 +0xB0 flag is set, func_800728B8 on the record and ten of its
 * prims picked by the record's +0x5A4 index. */
void func_80073FB8(void) {
    s32 i;
    u8* e;

    AddPrim(D_800CCB04__a_s32[0] + 4, (D_800CCB34 * 0xC) + 0x8920 + D_800C3EA4);
    for (i = 0; i < 4; i++) {
        if (((u8*)D_800D2D28 + i)[0x7F] != 0 && D_800C3E4C == 1) {
            AddPrim(D_800CCB04__a_s32[0] + 4, ((i * 2 + ((u8*)D_800D2D28 + i)[0x84]) * 0x34) + 0x5A0 + D_800C3EA4);
        }
    }
    AddPrim(D_800CCB04__a_s32[0] + 4, (D_800CCB34 * 0xC) + 0x8908 + D_800C3EA4);
    for (i = 0; i < 7; i++) {
        if (*((u8*)D_800D2D28 + i + 0xB0) != 0) {
            e = D_800D2E38[i];
            func_800728B8(e, *(s32*)(e + 0x5A0), e[0x5A4]);
            AddPrim(D_800CCB04__a_s32[0] + 4, e + (e[0x5A4] * 0x28 + 0x280));
            AddPrim(D_800CCB04__a_s32[0] + 4, e + (e[0x5A4] * 0x28 + 0x2D0));
            AddPrim(D_800CCB04__a_s32[0] + 4, e + (e[0x5A4] * 0x28 + 0x320));
            AddPrim(D_800CCB04__a_s32[0] + 4, e + (e[0x5A4] * 0x28 + 0x370));
            AddPrim(D_800CCB04__a_s32[0] + 4, e + (e[0x5A4] * 0x28 + 0x140));
            AddPrim(D_800CCB04__a_s32[0] + 4, e + (e[0x5A4] * 0x28 + 0x190));
            AddPrim(D_800CCB04__a_s32[0] + 4, e + (e[0x5A4] * 0x28 + 0x1E0));
            AddPrim(D_800CCB04__a_s32[0] + 4, e + (e[0x5A4] * 0x28 + 0x230));
            AddPrim(D_800CCB04__a_s32[0] + 4, e + (e[0x5A4] * 0x24 + 0x3C0));
            AddPrim(D_800CCB04__a_s32[0] + 4, e + (e[0x5A4] * 0xC + 0x408));
        }
    }
}
extern u8 D_800C3EB6[];
extern u8 D_800D32A1[];

void func_800742A0(void) {
    s32 temp_s1;
    s32 temp_v0_2;
    s32 var_s0;
    s32 var_s2;
    void *temp_v0;

    var_s0 = 0;
    var_s2 = 0;
    do {
        if (*(u8 *)((s8*)((D_800D2D28 + var_s0)) + 0x7C) != 0) {
            temp_v0 = (var_s0 * 4) + D_800D2D28;
            *(s32 *)((s8*)(temp_v0) + 0xE0) = 0;
            *(s32 *)((s8*)(temp_v0) + 0xEC) = 0;
            temp_s1 = var_s0 * 8;
            if (D_800C3EB6[var_s2] != 0x7F) {
                func_80073380(var_s0);
                temp_v0_2 = func_80072F38(var_s0, D_800D32A1[temp_s1]);
                if (D_800D32A1[temp_s1] != 0) {
                    func_80072DA8(var_s0, temp_v0_2 & 0xFF);
                } else {
                    func_80072A9C(var_s0, temp_v0_2 & 0xFF);
                }
                *(u8 *)((s8*)((D_800D2D28 + var_s0)) + 0x93) = (u8) D_800CCB34;
                *(s8 *)((s8*)((D_800D2D28 + var_s0)) + 0x7C) = 0;
            }
        }
        var_s0 += 1;
        var_s2 += 0x1C;
    } while (var_s0 < 3);
}

/* func_800743A4.s: when the prim set at *(D_800C3EA4 + 0xA230) is enabled
 * (+0x669), add its four double-buffered 0x28-byte prims (0x50 apart; the
 * buffer index is +0x668) to OT entry 1 (D_800CCB04 + 4).  The set pointer
 * is re-read after every AddPrim, as in retail. */
void func_800743A4(void) {
    if (*(u8*)(*(u8**)(D_800C3EA4 + 0xA230) + 0x669) != 0) {
        AddPrim(D_800CCB04__a_s32[0] + 4,
                (*(u8*)(*(u8**)(D_800C3EA4 + 0xA230) + 0x668) * 0x28) + *(u8**)(D_800C3EA4 + 0xA230));
        AddPrim(D_800CCB04__a_s32[0] + 4,
                (*(u8*)(*(u8**)(D_800C3EA4 + 0xA230) + 0x668) * 0x28) + 0x50 + *(u8**)(D_800C3EA4 + 0xA230));
        AddPrim(D_800CCB04__a_s32[0] + 4,
                (*(u8*)(*(u8**)(D_800C3EA4 + 0xA230) + 0x668) * 0x28) + 0xA0 + *(u8**)(D_800C3EA4 + 0xA230));
        AddPrim(D_800CCB04__a_s32[0] + 4,
                (*(u8*)(*(u8**)(D_800C3EA4 + 0xA230) + 0x668) * 0x28) + 0xF0 + *(u8**)(D_800C3EA4 + 0xA230));
    }
}
extern void *D_800C3EA4;
extern s32 D_800CCB04__a_s32[] asm("D_800CCB04");

void func_800744BC(void) {
    AddPrim(D_800CCB04__a_s32[0] + 4, (*(u8 *)((s8*)(*(void **)((s8*)(D_800C3EA4) + 0xA230)) + 0x66A) * 0x28) + 0x320 + *(void **)((s8*)(D_800C3EA4) + 0xA230));
    AddPrim(D_800CCB04__a_s32[0] + 4, (*(u8 *)((s8*)(*(void **)((s8*)(D_800C3EA4) + 0xA230)) + 0x66A) * 0x28) + 0x370 + *(void **)((s8*)(D_800C3EA4) + 0xA230));
}

extern void *D_800C3EA4;
extern s32 D_800CCB04__a_s32[] asm("D_800CCB04");

void func_80074554(void) {
    AddPrim(D_800CCB04__a_s32[0] + 4, (*(u8 *)((s8*)(*(void **)((s8*)(D_800C3EA4) + 0xA230)) + 0x66A) * 0x28) + 0x280 + *(void **)((s8*)(D_800C3EA4) + 0xA230));
    AddPrim(D_800CCB04__a_s32[0] + 4, (*(u8 *)((s8*)(*(void **)((s8*)(D_800C3EA4) + 0xA230)) + 0x66A) * 0x28) + 0x2D0 + *(void **)((s8*)(D_800C3EA4) + 0xA230));
}

INCLUDE_ASM("asm/battle/nonmatchings/main2", func_800745EC);

#ifndef XENO_PC_PORT
void func_80074AB8(void) {
    u16 temp_v0;

    if (*(u8 *)((s8*)(D_800D2D28) + 0xC7) != 0) {
        func_800728B8(D_800D2DB4 + 0x3E80, *(u8 *)((s8*)(D_800D2DB4) + 0x5D77), *(u8 *)((s8*)(D_800D2DB4) + 0x5D86));
        func_800728B8(D_800D2DB4 + 0x5280, *(u8 *)((s8*)(D_800D2DB4) + 0x5D7C), *(u8 *)((s8*)(D_800D2DB4) + 0x5D8B));
        func_800728B8(D_800D2DB4 + 0x53C0, *(u8 *)((s8*)(D_800D2DB4) + 0x5D7D), *(u8 *)((s8*)(D_800D2DB4) + 0x5D8C));
        func_800728B8(D_800D2DB4 + 0x43D0, *(u8 *)((s8*)(D_800D2DB4) + 0x5D7F), *(u8 *)((s8*)(D_800D2DB4) + 0x5D8E));
    }
    if (*(u8 *)((s8*)(D_800D2D28) + 0xAD) != 0) {
        func_800728B8(D_800D2DB4 + 0x46A0, 0x14U, *(u8 *)((s8*)(D_800D2DB4) + 0x5D98));
        func_800728B8(D_800D2DB4, *(u8 *)((s8*)(D_800D2DB4) + 0x5D70), *(u8 *)((s8*)(D_800D2DB4) + 0x5D92));
        func_800728B8(D_800D2DB4 + 0x1720, *(u8 *)((s8*)(D_800D2DB4) + 0x5D74), *(u8 *)((s8*)(D_800D2DB4) + 0x5D83));
        temp_v0 = *(u16 *)((s8*)(D_800D2DB4) + 0x5DA2) + 1;
        *(u16 *)((s8*)(D_800D2DB4) + 0x5DA2) = temp_v0;
        if ((s16) temp_v0 < 0xF) {
            func_800728B8(D_800D2DB4 + 0x4CE0, *(u8 *)((s8*)(D_800D2DB4) + 0x5DA1), *(u8 *)((s8*)(D_800D2DB4) + 0x5DA0));
        } else if ((s16) temp_v0 >= 0x15) {
            *(u16 *)((s8*)(D_800D2DB4) + 0x5DA2) = 0U;
        }
        func_800728B8(D_800D2DB4 + 0x3AC0, *(u8 *)((s8*)(D_800D2DB4) + 0x5D75), *(u8 *)((s8*)(D_800D2DB4) + 0x5D84));
        func_800728B8(D_800D2DB4 + 0x3E30, *(u8 *)((s8*)(D_800D2DB4) + 0x5D76), *(u8 *)((s8*)(D_800D2DB4) + 0x5D85));
        func_800728B8(D_800D2DB4 + 0x4E70, *(u8 *)((s8*)(D_800D2DB4) + 0x5D78), *(u8 *)((s8*)(D_800D2DB4) + 0x5D87));
        func_800728B8(D_800D2DB4 + 0x4FB0, *(u8 *)((s8*)(D_800D2DB4) + 0x5D79), *(u8 *)((s8*)(D_800D2DB4) + 0x5D88));
        func_800728B8(D_800D2DB4 + 0x50A0, *(u8 *)((s8*)(D_800D2DB4) + 0x5D7A), *(u8 *)((s8*)(D_800D2DB4) + 0x5D89));
        func_800728B8(D_800D2DB4 + 0x51E0, *(u8 *)((s8*)(D_800D2DB4) + 0x5D7B), *(u8 *)((s8*)(D_800D2DB4) + 0x5D8A));
        func_800728B8(D_800D2DB4 + 0x2530, *(u8 *)((s8*)(D_800D2DB4) + 0x5D7E), *(u8 *)((s8*)(D_800D2DB4) + 0x5D8D));
        func_800728B8(D_800D2DB4 + 0x32A0, *(u8 *)((s8*)(D_800D2DB4) + 0x5D96), *(u8 *)((s8*)(D_800D2DB4) + 0x5D97));
        func_800728B8(D_800D2DB4 + 0x3C0, *(u8 *)((s8*)(D_800D2DB4) + 0x5D71), *(u8 *)((s8*)(D_800D2DB4) + 0x5D93));
        func_800728B8(D_800D2DB4 + 0xD20, *(u8 *)((s8*)(D_800D2DB4) + 0x5D72), *(u8 *)((s8*)(D_800D2DB4) + 0x5D94));
        func_800728B8(D_800D2DB4 + 0xFA0, *(u8 *)((s8*)(D_800D2DB4) + 0x5D73), *(u8 *)((s8*)(D_800D2DB4) + 0x5D95));
    }
}
#endif


#ifndef XENO_PC_PORT
void func_80074D4C(void) {
    s32 temp_a0;
    s32 temp_v0;
    s32 temp_v0_2;
    s32 var_s0;

    if (*(u8 *)((s8*)(D_800D2D28) + 0xC6) != 0) {
        if (*(u8 *)((s8*)(D_800C3E24) + 0xE5) != 0) {
            temp_v0 = *(s32 *)((s8*)(D_800C3E24) + 0xE0) - 4;
            *(s32 *)((s8*)(D_800C3E24) + 0xE0) = temp_v0;
            if (temp_v0 < 0x40) {
                *(u8 *)((s8*)(D_800C3E24) + 0xE5) = 0U;
                *(s32 *)((s8*)(D_800C3E24) + 0xE0) = 0x40;
            }
            goto block_6;
        }
        temp_v0_2 = *(s32 *)((s8*)(D_800C3E24) + 0xE0) + 4;
        *(s32 *)((s8*)(D_800C3E24) + 0xE0) = temp_v0_2;
        var_s0 = 0;
        if (temp_v0_2 >= 0x100) {
            *(u8 *)((s8*)(D_800C3E24) + 0xE5) = 1U;
            *(s32 *)((s8*)(D_800C3E24) + 0xE0) = 0xFC;
block_6:
            var_s0 = 0;
        }
        do {
            if (*(u8 *)((s8*)((D_800C3E24 + var_s0)) + 0xE6) != 0) {
                temp_a0 = var_s0 * 2;
                *(u8 *)((s8*)((((temp_a0 + *(u8 *)((s8*)(D_800C3E24) + 0xE4)) * 0x1C) + D_800C3E24)) + 4) = (u8) *(s32 *)((s8*)(D_800C3E24) + 0xE0);
                *(s8 *)((s8*)((((temp_a0 + *(u8 *)((s8*)(D_800C3E24) + 0xE4)) * 0x1C) + D_800C3E24)) + 5) = 0;
                *(s8 *)((s8*)((((temp_a0 + *(u8 *)((s8*)(D_800C3E24) + 0xE4)) * 0x1C) + D_800C3E24)) + 6) = 0;
                AddPrim(D_800CCB04 + 4, ((temp_a0 + *(u8 *)((s8*)(D_800C3E24) + 0xE4)) * 0x1C) + D_800C3E24);
            }
            var_s0 += 1;
        } while (var_s0 < 4);
    }
}
#endif


#ifndef XENO_PC_PORT
void func_80074EEC(void) {
    if (*(u8 *)((s8*)(D_800D2D28) + 0xA8) != 0) {
        func_800728B8(D_800D2DB4 + 0x5550, *(u8 *)((s8*)(D_800D2DB4) + 0x5D80), *(u8 *)((s8*)(D_800D2DB4) + 0x5D8F));
        func_800728B8(D_800D2DB4 + 0x5640, *(u8 *)((s8*)(D_800D2DB4) + 0x5D81), *(u8 *)((s8*)(D_800D2DB4) + 0x5D90));
        func_800728B8(D_800D2DB4 + 0x5C80, *(u8 *)((s8*)(D_800D2DB4) + 0x5D82), *(u8 *)((s8*)(D_800D2DB4) + 0x5D91));
    }
}
#endif


#ifndef XENO_PC_PORT
void func_80074F70(void) {
    if (*(u8 *)((s8*)(D_800D2D28) + 0xC8) != 0) {
        AddPrim(D_800CCB04 + 4, (*(u8 *)((s8*)(D_800D3278) + 0x7F4) * 0x28) + 0x7A4 + D_800D3278);
    }
    if (*(u8 *)((s8*)(D_800D2D28) + 0xC9) != 0) {
        func_80034888(D_800D2DAC, D_800CCB04 + 4, D_800CCB34);
    }
}
#endif

extern u8 D_800D3725[][0x60];
extern u8 D_800D3726[][0x60];
extern u8 D_800D36C8[][0x60];
/* Retail passes x zero-extended to 16 bits and the rest unconverted. */
extern void func_80076C78(u8* p, u16 x, s32 y, s32 u, s32 v, s32 w);

/* Draw the eight 0x60-byte gauge records at D_800D36C8: each active one
 * (+0x5D set) gets its +0x5E width centred on x 0x40 (record 0, y 0x2C) or
 * 0x9A (the rest, y 0xCA, v = i / 2 * 13), then the record's current-buffer
 * prim is linked into the OT. */
void func_8007500C(void) {
    s32 i;

    for (i = 0; i < 8; i++) {
        if (D_800D3725[i][0] != 0) {
            if (i == 0) {
                func_80076C78(D_800D36C8[i] + D_800CCB34 * 0x28, 0x40 - D_800D3726[i][0] / 2, 0x2C, 0, 0,
                              D_800D3726[i][0]);
            } else {
                func_80076C78(D_800D36C8[i] + D_800CCB34 * 0x28, 0x9A - D_800D3726[i][0] / 2, 0xCA, 0,
                              (u8)(i / 2 * 13), D_800D3726[i][0]);
            }
            AddPrim(D_800CCB04__a_s32[0] + 4, D_800D36C8[i] + D_800CCB04__a_s32[12] * 0x28);
        }
    }
}
INCLUDE_ASM("asm/battle/nonmatchings/main2", func_80075168);
INCLUDE_ASM("asm/battle/nonmatchings/main2", func_80075938);
#endif
#ifndef XENO_PC_PORT
extern u8 D_800C492A;
#endif
extern void func_8008FAD8(void);
extern void func_800742A0(void);
extern void func_80075938(void);
extern void func_80073538(void);
extern void func_80073A58(void);
extern void func_80073B64(void);
extern void func_80073E88(void);
extern void func_80073F08(void);
extern void func_8007500C(void);
extern void func_80074EEC(void);
extern void func_800745EC(void);
extern void func_80074D4C(void);
extern void func_80073FB8(void);
extern void func_80088B80(void);
extern void func_80074AB8(void);
/* func_80076418.s: the per-frame battle render chain, skipped entirely once
 * D_800C492A is set. */
void func_80076418(void) {
    if (D_800C492A == 0) {
        func_8008FAD8();
        func_800742A0();
        func_80075938();
        func_80073538();
        func_80073A58();
        func_80073B64();
        func_80073E88();
        func_80073F08();
        func_8007500C();
        func_80074EEC();
        func_800745EC();
        func_80074D4C();
        func_80073FB8();
        func_80088B80();
        func_80074AB8();
    }
}


extern u8 D_8005959C;
extern void func_8008FAD8(void);
extern void func_801DE594(void);
extern void func_80073FB8(void);


/* func_800764B4.s */
void func_800764B4(void) {
    D_8005959C = 0;
    func_8008FAD8();
    func_801DE594();
    func_80073FB8();
}
/* func_800764EC.s */
void func_800764EC(void) {
    func_8008FAD8();
    func_80073538();
    func_80073F08();
    func_8007500C();
    func_80074F70();
    func_80073FB8();
    func_80088B80();
    func_80074AB8();
}
