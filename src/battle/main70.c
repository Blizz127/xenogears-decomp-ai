#include "common.h"
#ifndef XENO_PC_PORT
#include "system/memory.h"
#include "psyq/libgte.h"
#endif


#ifndef XENO_PC_PORT
void func_800A22A8(s32 *);
void func_800A2D1C(s32 *);
void func_800A9FF0(s32);
extern s32 D_800C3D04;
extern s32 D_800C3D0C;
void func_800A8A88(void *arg0);
void func_800A9F94(void);
s32 func_800AA514(s16 arg0, s16 arg1, s32 arg2);
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/main70", func_800A5EB4);
INCLUDE_ASM("asm/battle/nonmatchings/main70", func_800A6444);
INCLUDE_ASM("asm/battle/nonmatchings/main70", func_800A64E4);
INCLUDE_ASM("asm/battle/nonmatchings/main70", func_800A6884);
INCLUDE_ASM("asm/battle/nonmatchings/main70", func_800A6AE8);
INCLUDE_ASM("asm/battle/nonmatchings/main70", func_800A6F98);
INCLUDE_ASM("asm/battle/nonmatchings/main70", func_800A7064);
INCLUDE_ASM("asm/battle/nonmatchings/main70", func_800A7948);

#ifndef XENO_PC_PORT
void func_800A8A88(void *arg0) {
    s32 *temp_a0;
    s32 *temp_a0_2;

    temp_a0 = *(s32 **)((s8*)(arg0) + 0x14);
    if (temp_a0 != NULL) {
        HeapFree(temp_a0);
        HeapFree(**(s32 **)((s8*)(arg0) + 0x1C));
        HeapFree(*(s32 **)((s8*)(arg0) + 0x1C));
        HeapFree(*(s32 **)((s8*)(arg0) + 0x20));
        temp_a0_2 = *(s32 **)((s8*)(arg0) + 0x18);
        if (temp_a0_2 != NULL) {
            HeapFree(temp_a0_2);
        }
        *(s32 **)((s8*)(arg0) + 0x14) = NULL;
    }
}
#endif

extern s32 D_800C3E88;
extern s16 D_800C3CF0;
extern u8 D_800C3D6C;
extern u8 D_800C3D68;
extern s16 D_800C3B7C;
extern u8 D_800C3B74;
typedef struct {
    u8 unk0[0x348];
    s16 count348; /* D_800C3D0C list size */
    s16 count34A; /* D_800C3D04 (gfx entry) list size */
} BattleSetupCounts;
extern BattleSetupCounts* D_800658C8;
extern u32 D_800D3368[];
extern u32 D_800C3ACC[][2];
extern s16 D_800D330A[];
extern u8* func_800A2234(u8* list, s32 count);
extern u8* func_800A2CA4(u8* list, s32 count);
extern void func_800B00D0(void);

/* Reset the battle effect state: clear the effect globals, raise
 * D_800C3B74, allocate the D_800C3D0C / D_800C3D04 lists at the setup's
 * sizes, run func_800B00D0, and clear the 32 actor slots D_800D3368, the
 * first word of the 20 D_800C3ACC pairs and both anchors' active flags
 * (D_800D3304 records, 0x14 apart; retail walks them by byte offset). */
void func_800A8B0C(void) {
    s32 i;
    s32 j;
    s32 off;

    D_800C3E88 = 0;
    D_800C3CF0 = 0;
    D_800C3D6C = 0;
    D_800C3D68 = 0;
    D_800C3B7C = 0;
    D_800C3B74 = 1;
    func_800A2234((u8*)&D_800C3D0C, D_800658C8->count348);
    func_800A2CA4((u8*)&D_800C3D04, D_800658C8->count34A);
    func_800B00D0();
    for (i = 31; i >= 0; i--) {
        D_800D3368[i] = 0;
    }
    for (j = 19; j >= 0; j--) {
        D_800C3ACC[j][0] = 0;
    }
    for (off = 0x14; off >= 0; off -= 0x14) {
        *(s16*)((u8*)D_800D330A + off) = 0;
    }
}
INCLUDE_ASM("asm/battle/nonmatchings/main70", func_800A8BF0);
typedef struct {
    u16 index;
    u16 unk2;
    void* buf;
} BattleFileEntry;

extern u8 D_800CCD88[];
extern u8 D_800CCDE8[];
extern u8 D_800C3509[];
extern BattleFileEntry* D_800C3B78;
extern void ArchiveGetArchiveOffsetIndices(s32* idx, s32* sub);
extern void ArchiveSetIndex(s32 idx, s32 sub);
extern s32 ArchiveDecodeAlignedSize(s32 index);
extern void func_80029AFC(BattleFileEntry* list, s32 a1, s32 a2);

/* Queue actor `actor`'s battle model files from archive 0x28: a 0x20-byte
 * list (published at D_800C3B78) of {file index, buffer} entries ending in
 * a zero entry.  The base file comes from the D_800C3508 pair table for the
 * actor's +0xCD88 kind; entries are base + 1, base + 2 and, when the actor's
 * +0xCDE8 extra is within the table's limit, base + 2 + extra.  The archive
 * position is restored afterwards. */
void func_800A9540(s32 actor) {
    s32 sub;
    u8 kind;
    s32 extra;
    BattleFileEntry* p;
    s32 idx;
    u8 n;
    s32 file;
    register BattleFileEntry* list asm("$18");

    ArchiveGetArchiveOffsetIndices(&idx, &sub);
    ArchiveSetIndex(0x28, 1);
    HeapChangeCurrentUser(4, 0);
    kind = D_800CCD88[actor * 0x170];
    extra = D_800CCDE8[actor * 0x170];
    if (D_800C3509[kind * 2] < extra) {
        extra = 0;
    }
    list = HeapAlloc(0x20, 1);
    D_800C3B78 = list;
    n = D_800C3509[kind * 2 - 1];
    p = list;
    file = n + 1;
    p->index = file;
    p->buf = HeapAlloc(ArchiveDecodeAlignedSize(file), 1);
    file = n + 2;
    list[1].index = file;
    list[1].buf = HeapAlloc(ArchiveDecodeAlignedSize(file), 0);
    p = &list[2];
    if (extra != 0) {
        file += extra;
        list[2].index = file;
        list[2].buf = HeapAlloc(ArchiveDecodeAlignedSize(file), 1);
        p = &list[3];
    }
    p->index = 0;
    p->buf = NULL;
    func_80029AFC(D_800C3B78, 0, 0);
    ArchiveSetIndex(idx, sub);
}
INCLUDE_ASM("asm/battle/nonmatchings/main70", func_800A96B4);
extern void ResolveArchiveEntryPointers(u8* entry);
extern void func_800A8BF0(s32 slot, u16 kind, void* a2, void* a3, s16 x, s16 y, s16 z, s16 w,
                          s32 a8);
extern void func_8002DDE4(void* a0, u32 a1, u32 a2, u32 a3, u32 s0, u32 s1, u32 s2);
extern void* memcpy(void* dst, const void* src, u32 n);
extern void DrawSync(s32 mode);

/* func_800A979C.s: set up slot idx through func_800A8BF0 from the
 * D_800C3B78 file list and set that slot's +0x38 and +0x22 bytes; then, when
 * list entry 2 is loaded, resolve it: with a nonzero halfword count, copy its
 * +8..+0xC data and set up `count` more slots idx * 2 + 0xD + i (kind 2 for
 * the first, 7 for the rest, 2 less for the last) from that copy, filling
 * each slot's +0x5E and +0x6A..+0x6E halfwords from the entry's table;
 * with a zero count, hand the entry's data to func_8002DDE4.  Frees the
 * entry, the list and list entry 0's buffer.  `slot` is built in two steps,
 * which keeps idx * 2 inside the loop as retail has it. */
void func_800A979C(s32 idx, s16 x, s16 y, s16 z, s16 w) {
    u8* entry;
    u16* p;
    s16 count;
    s32 size;
    void* buf;
    s32 i;
    s32 kind;
    s32 slot;
    s32 sub;

    func_800A8BF0(idx, 0, D_800C3B78[1].buf, D_800C3B78[0].buf, x, y, z, w, 0);
    ((u8*)D_800D3368[idx])[0x38] = 1;
    ((u8*)D_800D3368[idx])[0x22] = 1;
    entry = D_800C3B78[2].buf;
    if (entry != NULL) {
        ResolveArchiveEntryPointers(entry);
        p = *(u16**)(entry + 4);
        count = *(s16*)p;
        if (count != 0) {
            p += 2;
            size = *(s32*)(entry + 0xC) - *(s32*)(entry + 8);
            buf = HeapAlloc(size, 0);
            memcpy(buf, *(void**)(entry + 8), size);
            for (i = 0; i < count; i++) {
                if (i == 0) {
                    kind = 2;
                } else {
                    kind = 7;
                }
                if (i == count - 1) {
                    kind -= 2;
                }
                slot = idx * 2;
                sub = i + 0xD;
                slot += sub;
                func_800A8BF0(slot, kind, buf, *(void**)(entry + 0xC), x, y, z, w, 0);
                *(u16*)(D_800D3368[slot] + 0x5E) = *p++;
                ((u8*)D_800D3368[slot])[0x5C] = idx;
                ((u8*)D_800D3368[slot])[0x5D] = 2;
                ((u8*)D_800D3368[slot])[0x36] = 1;
                *(u16*)(D_800D3368[slot] + 0x6A) = *p++;
                *(u16*)(D_800D3368[slot] + 0x6C) = *p++;
                *(u16*)(D_800D3368[slot] + 0x6E) = *p++;
            }
        } else {
            func_8002DDE4(*(void**)(entry + 8), 1, x, y, 1, z, w);
        }
        HeapFree(entry);
    }
    HeapFree(D_800C3B78);
    DrawSync(0);
    HeapFree(D_800C3B78[0].buf);
}
extern s32 D_800CCC5C;
extern s16 D_800D39E8;
extern s16 D_800C3B80;
extern u16 D_800C3D40__u asm("D_800C3D40");
extern u16 D_800C3E30__u asm("D_800C3E30");
extern u8 D_800C3DF8;
extern MATRIX* D_800D2FC0;
extern u16 D_800C3D14;
extern s32 D_80050104;
extern s32 func_800AA600(s32 index);
extern s32 func_800AA650(s32 index);
void func_800B10EC(s32, s32, s32, s32);
void func_800A4CF8(s32 arg0);
extern void func_800AAA20(u8* actor, s32* list, s32 steps, s32 buf, s32 rate);
extern void func_800B026C(s32* list, s32 steps, s32 a2, s32 buf);
extern void func_800AAB34(u8* actor);
void func_800A44C0(u8** models);
void func_8009F844(u8* actor, MATRIX* view, MATRIX* light, s32 a3, s32 rate, u32* ot,
                   s32 buf);
void func_800A2FD8(u8* list, MATRIX* m, s32 step, u32* ot, s32 buf);

/* func_800A9A50.s: add D_800CCC5C + 1 to D_800C3E88 (capped at 6) and take
 * out whole steps of 2; advance the D_800C3CF0 and D_800C3B7C rsin phases
 * into D_800D39E8 and D_800C3B80, and derive three colour bytes from the
 * latter through func_800AA514.  When slot D_800C3D40 of D_800D3368 is set
 * and its +0x4A bit 0x20 is clear, compare it with slots 0..10
 * (func_800AA600 / func_800AA650 / func_800B10EC), then func_800A4CF8.  Run
 * func_800AAA20 on every set slot, func_800B026C when D_800C3DF8 is set,
 * func_800AAB34 on slots whose +0x5C byte is below 0xFF, func_800A44C0, and
 * while D_800C3B74 is set func_8009F844 on slots 0..30 with the derived or
 * the D_800658C8 back colour.  Ends with func_800A2FD8 on D_800C3D04. */
void func_800A9A50(MATRIX* view, MATRIX* light, u32* ot, s32 buf) {
    s32 steps;
    u8 bg[3];
    s32 i;
    s32 j;
    u8* body;
    u8* other;
    s32 v;
    u8** slot;
    u8* actor;

    steps = 0;
    D_800C3E88 += D_800CCC5C + 1;
    if (D_800C3E88 >= 7) {
        D_800C3E88 = 6;
    }
    while (D_800C3E88 >= 2) {
        D_800C3E88 -= 2;
        steps++;
    }
    D_800C3CF0 += steps * 56;
    D_800D39E8 = (rsin(D_800C3CF0) + 0x1000) / 800 + 4;
    D_800C3B7C += 0x80;
    D_800C3B80 = (rsin(D_800C3B7C) + 0x1000) / 32;
    bg[0] = func_800AA514(D_800C3B80, 0x20, ((u8*)D_800658C8)[0x474]);
    bg[1] = func_800AA514(D_800C3B80, 0x20, ((u8*)D_800658C8)[0x475]);
    bg[2] = func_800AA514(D_800C3B80, 0x20, ((u8*)D_800658C8)[0x476]);
    if (D_800D3368[D_800C3D40__u] != 0 && !(*(u16*)(D_800D3368[D_800C3D40__u] + 0x4A) & 0x20)) {
        for (j = 0; j < 11; j++) {
            if (D_800D3368[j] != 0 && D_800C3D40__u != j && ((u8*)D_800D3368[j])[0x5C] == 0xFF &&
                !((D_800C3E30__u >> j) & 1) && ((u8*)D_800D3368[j])[0x34] != 0) {
                body = *(u8**)(D_800D3368[D_800C3D40__u] + 4);
                v = func_800AA600(j);
                other = *(u8**)(D_800D3368[j] + 4);
                if (*(s32*)(other + 0x60) - v < *(s32*)(body + 0x60)) {
                    v = func_800AA600(D_800C3D40__u);
                    if (*(s32*)(*(u8**)(D_800D3368[D_800C3D40__u] + 4) + 0x60) - v <
                        *(s32*)(other + 0x60)) {
                        v = func_800AA650(j);
                        v += func_800AA650(D_800C3D40__u);
                        func_800B10EC(D_800C3D40__u, *(s32*)(*(u8**)(D_800D3368[j] + 4) + 0x5C),
                                      *(s32*)(*(u8**)(D_800D3368[j] + 4) + 0x64), v);
                    }
                }
            }
        }
        func_800A4CF8(D_800C3D40__u);
    }
    for (i = 0; i < 32; i++) {
        if (D_800D3368[i] != 0) {
            func_800AAA20((u8*)D_800D3368[i], &D_800C3D0C, steps, buf, D_800CCC5C);
        }
    }
    i = 0;
    if (D_800C3DF8 != 0) {
        func_800B026C(&D_800C3D0C, steps, 0, buf);
    }
    slot = (u8**)D_800D3368;
    for (; i < 32; i++) {
        actor = *slot++;
        if (actor != NULL && actor[0x5C] < 0xFF) {
            func_800AAB34(actor);
        }
    }
    func_800A44C0((u8**)D_800D3368);
    SetColorMatrix(D_800D2FC0);
    if (D_800C3B74 != 0) {
        for (i = 0; i < 31; i++) {
            if (D_800D3368[i] != 0) {
                if ((D_800C3D14 >> i) & 1) {
                    SetBackColor(bg[0], bg[1], bg[2]);
                } else {
                    SetBackColor(((u8*)D_800658C8)[0x474], ((u8*)D_800658C8)[0x475],
                                 ((u8*)D_800658C8)[0x476]);
                }
                if (*(u16*)(D_800D3368[i] + 0x4A) & 0x40) {
                    D_80050104 = 0;
                } else {
                    D_80050104 = 1;
                }
                func_8009F844((u8*)D_800D3368[i], view, light, 1, D_800CCC5C, ot, buf);
                D_80050104 = 0;
            }
        }
    }
    func_800A2FD8((u8*)&D_800C3D04, view, steps, ot, buf);
}

#ifndef XENO_PC_PORT
void func_800A9F94(void) {
    s32 var_s0;

    var_s0 = 0;
    do {
        func_800A9FF0(var_s0);
        var_s0 += 1;
    } while (var_s0 < 0x1F);
    func_800A22A8(&D_800C3D0C);
    func_800A2D1C(&D_800C3D04);
}
#endif

extern s32 D_800C3D0C__a[] asm("D_800C3D0C");
extern void func_8009F794(void* list, s32 release);
extern void func_8003852C(void* p);
extern void func_800A2ACC(s32 ctx, u8* nodes);
extern void func_800A2BB8(s32 ctx, u8* nodes, u8 id);
extern void func_8009F708(void* set);
extern void func_800B0060(u8* p);
extern void func_800A429C(void* p);

#define M70_REC ((u8*)D_800D3368[idx])

/* func_800A9FF0.s: free actor slot idx of D_800D3368 and everything it owns
 * (the +0xA8/+0xAC buffers, the +4 list, through func_800B0060 for slots
 * outside 0x13..0x1E, the +0x110 buffer, the +0x118 and +0x114 element
 * arrays), clear the slot, and for idx below 3 free slots idx * 2 + 0xD and
 * idx * 2 + 0xE as well. */
void func_800A9FF0(s32 idx) {
    s32 i;

    if (D_800D3368[idx] != 0) {
        if (*(void**)(M70_REC + 0xA8) != NULL) {
            HeapFree(*(void**)(M70_REC + 0xA8));
            func_8009F794(*(void**)M70_REC, 1);
        }
        if (M70_REC[0x62] != 0) {
            func_8003852C(*(void**)(*(u8**)(M70_REC + 0xB0) + 8));
        }
        if (*(void**)(M70_REC + 0xAC) != NULL) {
            HeapFree(*(void**)(M70_REC + 0xAC));
        }
        if ((u32)(idx - 0x13) >= 0xC) {
            if (*(u8**)(M70_REC + 4) != NULL) {
                func_800A2ACC((s32)D_800C3D0C__a, *(u8**)(M70_REC + 4));
                func_800A2BB8((s32)D_800C3D0C__a, *(u8**)(M70_REC + 4), 0xFF);
                func_8009F708(*(void**)(M70_REC + 4));
                *(void**)M70_REC = NULL;
                *(void**)(M70_REC + 4) = NULL;
            }
            func_800B0060(M70_REC);
        } else if (*(u8**)(M70_REC + 4) != NULL) {
            func_800A2ACC((s32)D_800C3D0C__a, *(u8**)(M70_REC + 4));
            func_800A2BB8((s32)D_800C3D0C__a, *(u8**)(M70_REC + 4), 0xFF);
            HeapFree(*(void**)(M70_REC + 4));
        }
        if (M70_REC[0x10C] != 0) {
            HeapFree(*(void**)(M70_REC + 0x110));
        }
        if (M70_REC[0x10E] != 0) {
            for (i = 0; i < M70_REC[0x10E]; i++) {
                func_800A429C((u8*)(i * 0x30 + *(s32*)(M70_REC + 0x118)));
            }
            HeapFree(*(void**)(M70_REC + 0x118));
        }
        if (M70_REC[0x10D] != 0) {
            for (i = 0; i < M70_REC[0x10D]; i++) {
                func_800A8A88((u8*)(i * 0x24 + *(s32*)(M70_REC + 0x114)));
            }
            HeapFree(*(void**)(M70_REC + 0x114));
        }
        HeapFree(M70_REC);
        D_800D3368[idx] = 0;
    }
    if (idx < 3) {
        func_800A9FF0(idx * 2 + 0xD);
        func_800A9FF0(idx * 2 + 0xE);
    }
}
#undef M70_REC
extern u32 D_800D3368[];
extern s16 D_800C3D40;
extern s16 D_800C3E30;
extern s32 D_800C3D0C;
extern void func_800AA934(u8 *obj, u8 *src, s32 id, s32 slot);

/* func_800AA320: rec = (u8*)D_800D3368[(u16)arg0]; D_800C3D40=arg0;
 * D_800C3E30=arg1; rec->0x35=0; reload rec (same table read, since the
 * intervening stores keep gcc from proving no-alias and it reloads from
 * memory rather than caching, matching retail's two separate lw's off the
 * same address register); if non-NULL, forward to func_800AA934(rec, rec,
 * &D_800C3D0C, arg2) (canonical extern signature from src/battle/
 * main72_p2.c's matched definition, id passed as the &D_800C3D0C address
 * value per this call site, same as src/battle/main75.c's XENO_PC_PORT
 * guest-side callers).
 * Register notes: writing "D_800D3368[idx]" plain (not through a manually
 * pinned pointer variable) lets gcc fold the table-address arithmetic into
 * one "la"+add exactly like retail; a `register T *x asm("$N")` pin on
 * that address/computed-pointer value is silently dropped by -O2's combine
 * pass here (no diagnostic) because the whole expression collapses into a
 * single addressing rtx before register allocation ever sees the pseudo -
 * pins only stick on values gcc keeps as a live SSA temp. The two reloads
 * of D_800D3368[idx] are two distinct C statements/variables (rec1, rec2)
 * since they are genuinely different pseudoregs in retail (v1 then a0); pin
 * only the second (rec2) to $4/a0, matching retail loading straight into
 * the outgoing call's first argument register with no extra move. */
void func_800AA320(s16 arg0, s16 arg1, s32 arg2) {
    u32 idx = (u16)arg0;
    u8 *rec1;
    register u8 *rec2 asm("$4");

    rec1 = (u8 *)D_800D3368[idx];
    D_800C3D40 = arg0;
    D_800C3E30 = arg1;
    rec1[0x35] = 0;
    rec2 = (u8 *)D_800D3368[idx];
    if (rec2 != NULL) {
        func_800AA934(rec2, rec2, (s32)&D_800C3D0C, arg2);
    }
}

extern s32 func_800AF400();
extern void func_800BF85C(u32 index, s32 bit);
extern u8 D_800C3D68;
extern s32 D_80059464;
extern u8 D_800591AC;
extern s32 D_800C360C;
extern u8 D_800C4000[];

/* Start actor `arg0`'s turn: publish it (D_800C3D40) and its target mask
 * (D_800C3E30), raise D_800C3D68 / D_800591AC and the record's +0x35 flag,
 * clear D_80059464, hand the lowest set target bit (func_800AF400) to
 * func_800BF85C, set D_800C360C, clear that target's D_800C4000 byte, then
 * forward to func_800AA934(rec, rec, &D_800C3D0C, arg2) when the record is
 * live.  Same reload-and-pin idiom as func_800AA320 above. */
void func_800AA384(u16 arg0, u16 arg1, s32 arg2) {
    u32 idx;
    u8 *rec1;
    register u8 *rec2 asm("$4");

    D_800C3D40 = arg0;
    idx = arg0;
    rec1 = (u8 *)D_800D3368[idx];
    D_800C3E30 = arg1;
    D_800C3D68 = 1;
    rec1[0x35] = 1;
    D_80059464 = 0;
    D_800591AC = 1;
    func_800BF85C(idx, func_800AF400());
    D_800C360C = 1;
    D_800C4000[func_800AF400()] = 0;
    rec2 = (u8 *)D_800D3368[idx];
    if (rec2 != NULL) {
        func_800AA934(rec2, rec2, (s32)&D_800C3D0C, arg2);
    }
}
s32 func_800AF400();                                /* extern */

void func_800AA454(u16 arg0, u16 arg1, s32 arg2) {
    u32 idx;
    u16 temp_s2;
    u16 temp_s3;
    u8 *rec1;
    register u8 *rec2 asm("$4");

    idx = arg0 & 0xFFFF;
    temp_s2 = D_800C3D40;
    temp_s3 = D_800C3E30;
    rec1 = (u8 *)D_800D3368[idx];
    D_800C3D40 = arg0;
    D_800C3E30 = arg1;
    rec1[0x35] = 0;
    if ((D_800D3368[idx] != 0) && (idx != func_800AF400())) {
        rec2 = (u8 *)D_800D3368[idx];
        func_800AA934(rec2, rec2, (s32)&D_800C3D0C, arg2);
    }
    D_800C3D40 = temp_s2;
    D_800C3E30 = temp_s3;
}


#ifndef XENO_PC_PORT
s32 func_800AA514(s16 arg0, s16 arg1, s32 arg2) {
    s16 temp_v0;
    s16 var_v1;

    temp_v0 = arg2 + ((s32) (arg1 * arg0) / 256);
    var_v1 = temp_v0;
    if (temp_v0 >= 0x100) {
        var_v1 = 0xFF;
    }
    return var_v1 & 0xFF;
}
#endif

#endif /* XENO_PC_PORT excludes the other main70 matching bodies above. */

#ifndef XENO_PC_PORT
s32 func_800AF400();                                /* extern */

void func_800AA564(u8 *arg0, s16 arg1, s16 arg2, s32 arg3) {
    u32 idx;
    u8 *rec;

    idx = (u16)arg1;
    rec = (u8 *)D_800D3368[idx];
    D_800C3D40 = arg1;
    D_800C3E30 = arg2;
    rec[0x35] = 0;
    if ((D_800D3368[idx] != 0) && (idx != func_800AF400())) {
        func_800AA934((u8 *)D_800D3368[idx], arg0, (s32)&D_800C3D0C, arg3);
    }
}
#endif

#ifndef XENO_PC_PORT
extern u32 D_800D3368[];
#define BATTLE_600_SLOT(index) D_800D3368[index]
#define BATTLE_600_U32(address) (*(u32*)(address))
#define BATTLE_600_S16(address) (*(s16*)(address))
#define BATTLE_600_MID register s32 mid asm("$2")
#else
#define BATTLE_600_SLOT(index) (*(u32*)PSX_ADDR(0x800D3368 + (index) * 4))
#define BATTLE_600_U32(address) (*(u32*)PSX_ADDR(address))
#define BATTLE_600_S16(address) (*(s16*)PSX_ADDR(address))
#define BATTLE_600_MID s32 mid
#endif

/* func_800AA600: look up actor record p = D_800D3368[index]; if present,
 * combine two fixed-point (Q12) multiplies: mid = (p->0x1C * (*(p->4))->0x4E) >> 12,
 * then result = (p->0x24 * mid) >> 12. Same table idiom as func_800AA760
 * in src/battle/main70.c. Byte-exact reg alloc needs the raw mult product
 * and its in-place shift to share one hard register (v0/$2): writing
 * `mid = a*b; mid >>= 12;` as two statements on the same named var (rather
 * than folding into `(a*b)>>12`) keeps the mflo destination and the sra
 * destination the same register, matching retail's `mflo v0`/`sra v0,v0,12`
 * for both multiplies; only the first needs an explicit pin, the final
 * result naturally lands in v0 as the function's return register. */
s32 func_800AA600(s32 index) {
    BATTLE_600_MID;
    u32 p;
    s32 result;

    p = BATTLE_600_SLOT(index);
    result = 0;
    if (p != 0) {
        u32 q = BATTLE_600_U32(p + 4);
        s16 val1 = BATTLE_600_S16(p + 0x1C);
        s16 val2 = BATTLE_600_S16(q + 0x4E);
        s16 val3;
        mid = val1 * val2;
        mid >>= 12;
        val3 = BATTLE_600_S16(p + 0x24);
        result = val3 * mid;
        result >>= 12;
    }
    return result;
}

#ifndef XENO_PC_PORT
s32 func_800AA650(s32 arg0) {
    register s32 mid asm("$2");
    u32 p;
    s32 a;
    s32 b;
    s32 result;

    p = D_800D3368[arg0];
    result = 0;
    if (p != 0) {
        if (*(u16 *)(p + 0x4A) & 8) {
            a = *(s16 *)(p + 0x26);
            b = *(s16 *)(*(u32 *)(p + 4) + 0x4C);
        } else {
            a = *(s16 *)(p + 0x28);
            b = *(s16 *)(*(u32 *)(p + 4) + 0x50);
        }
        mid = *(s16 *)(D_800D3368[arg0] + 0x1C) * b;
        mid >>= 12;
        result = a * mid;
        result >>= 12;
    }
    return result;
}
#endif

#undef BATTLE_600_SLOT
#undef BATTLE_600_U32
#undef BATTLE_600_S16
#undef BATTLE_600_MID
extern void* HeapAlloc(u32 size, u32 flags);
/* func_800AA6E0.s: allocate p[0x10C] 0x70-byte records into *(p + 0x110),
 * each with halfword +0 = -1 and word +8 = 0; nothing when the count is 0.
 * The count is re-read every iteration. */
void func_800AA6E0(u8* p) {
    u8 n = p[0x10C];

    if (n != 0) {
        u8* buf = HeapAlloc(n * 0x70, 0);
        s32 i;

        for (i = 0; i < p[0x10C]; i++) {
            *(s16*)(buf + i * 0x70) = -1;
            *(u32*)(buf + i * 0x70 + 8) = 0;
        }
        *(u8**)(p + 0x110) = buf;
    }
}
#ifndef XENO_PC_PORT
extern u32 D_800D3368[];
#endif
#ifndef XENO_PC_PORT
extern u8 D_800C3B74;
#endif


/* func_800AA760.s */
void func_800AA760(u32 index, u8 value) {
#ifdef XENO_PC_PORT
    /* Retail uses an unbounded guest table index and the slot contains a
     * guest address; translate those two memory accesses on the host. */
    u32 p = *(u32*)PSX_ADDR(0x800D3368 + index * 4);
#else
    u32 p = D_800D3368[index];
#endif

    if (p != 0) {
#ifdef XENO_PC_PORT
        *(u8*)PSX_ADDR(p + 0x2A) = value;
#else
        *(u8*)(p + 0x2A) = value;
#endif
    }
}
/* func_800AA788.s */
void func_800AA788(u32 value) {
    D_800C3B74 = (u8)(value & 1);
}
