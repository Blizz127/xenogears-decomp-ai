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
INCLUDE_ASM("asm/battle/nonmatchings/main2", func_80071AE0);
INCLUDE_ASM("asm/battle/nonmatchings/main2", func_80071B94);
INCLUDE_ASM("asm/battle/nonmatchings/main2", func_80072270);
INCLUDE_ASM("asm/battle/nonmatchings/main2", func_80072324);
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

INCLUDE_ASM("asm/battle/nonmatchings/main2", func_80072938);
INCLUDE_ASM("asm/battle/nonmatchings/main2", func_80072A9C);
INCLUDE_ASM("asm/battle/nonmatchings/main2", func_80072DA8);
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
extern s32 func_80076A10(u8 glyph, u8* prims, s16 x, s16 y);
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

INCLUDE_ASM("asm/battle/nonmatchings/main2", func_800743A4);
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
