#include "common.h"
#include "field/actor.h"
#include "field/main.h"
#include "field/text_box.h"
#include "field/effects.h"
#include "field/script_vm.h"
#include "system/memory.h"
#include "system/archive.h"
#include "system/sound.h"
#include "psyq/libetc.h"
#include "psyq/libcd.h"
#ifdef XENO_PC_PORT
#include <assert.h>
#else
/* <assert.h> is unavailable under the matching build's -nostdinc MIPS
 * preprocessor. The assert(...) calls below mark unimplemented/invariant
 * checks in functions not yet byte-matched, so a no-op assert compiles
 * safely there. */
#define assert(x) ((void)0)
#endif

extern int D_800ADBFC;
extern void func_80281678(void*);
void func_8008083C(int actorIndex) {
    ActorData* pActor;

    if (actorIndex < D_800ADBFC) {
        pActor = (ActorData*)(uintptr_t)g_FieldActors[actorIndex].pActorData;
        if (pActor->flags134 & 0x80) {
            HeapFree((void*)(uintptr_t)pActor->unk110);
        }
        if (pActor->flags12C_0xD) {
            HeapFree((void*)(uintptr_t)pActor->unk114);
        }
        if (g_FieldActors[actorIndex].status & 0x2000) {
            HeapFree((void*)(uintptr_t)pActor->unk118);
        }
        if (pActor->unk124 != -1) {
            HeapFree((void*)(uintptr_t)pActor->unk120);
        }
        HeapFree(pActor);
        HeapFree((void*)(uintptr_t)g_FieldActors[actorIndex].pShadow);
        func_800230A8((void*)(uintptr_t)g_FieldActors[actorIndex].pSpriteData);
    }
}

/* ---- func_80080968: actor state table lookup -------------------------------
 * Called from func_80080A74. Reads ActorData.field_10 as a state index,
 * checks a gate bit in field_04, then looks up a pointer in two-level
 * tables (D_800AFB24 / D_800AFB20) indexed by sub-state values.
 * Returns the looked-up pointer (stored at ActorData offset 0x14). */
extern s32 D_800AFB20;
extern s32 D_800AFB24[];

s32 func_80080968(u8* pActorData) {
    s16 stateIdx = *(s16*)(pActorData + 0x10);
    s32 gateWord = *(s32*)(pActorData + 0x04);

    /* Gate check: if bit (stateIdx+3) of field_04 is set, return 0 */
    if ((gateWord >> (stateIdx + 3)) & 1) {
        return 0;
    }

    /* Sub-state lookup: read s16 at pActorData[stateIdx].offset_08
     * (entries are 2 bytes each, starting at pActorData + 0) */
    {
        s16 subIdx = *(s16*)(pActorData + stateIdx * 2 + 0x08);
        s32 val = subIdx * 7;  /* subIdx * 8 - subIdx = subIdx * 7 */

        /* D_800AFB24[stateIdx] points to an array of 14-byte structs.
         * val * 2 indexes into it (14-byte stride). */
        u8* tableRow = (u8*)(uintptr_t)D_800AFB24[stateIdx];
        s32 byteVal;
        u32* finalTable;

        if (tableRow == NULL) return 0;  /* XENO_PC_PORT: guard stubbed table */

        byteVal = *(u8*)(tableRow + val * 2 + 0x0C);

        /* Final lookup: D_800AFB20[byteVal] */
        finalTable = (u32*)(uintptr_t)D_800AFB20;
        if (finalTable == NULL) return 0;  /* XENO_PC_PORT: guard stubbed table */

        return (s32)finalTable[byteVal];
    }
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc8", func_800809D0);

extern s32 D_800ADB58;
extern s32 D_800ADB5C;

s32 func_80080A18(void) {
    ActorData* p = (ActorData*)(uintptr_t)g_FieldActors[D_800ADB58].pActorData;
    s32 i = D_800ADB5C++;
    return ((s32*)(uintptr_t)p->unk118)[i];
}

/* ---- func_80080A74: per-actor second-pass ActorData initialization ----------
 * Called from func_80080F44 for each actor. Initializes ActorData with
 * hardcoded defaults and state-array entries. Calls func_80080968 for
 * additional setup. Conditionally runs func_8007B1C4 distance-init loop
 * (skipped in port: D_800AFB54 stubbed to 0/1).
 *
 * All offsets are byte offsets into the ActorData allocation (0x138 bytes),
 * verified against MIPS asm. */
extern s16 D_800AFB54;
extern s32 D_800AFB44[];

void func_80080A74(s32 actorIndex) {
    u8* p = (u8*)(uintptr_t)g_FieldActors[actorIndex].pActorData;
    s16 stateBuf[0x34] = { 0 };
    s32 useStateBuf = 0;
    s32 i;
    s32 actorByteOff = actorIndex * 0x5C;
    u8* pActorBytes = (u8*)g_FieldActors + actorByteOff;

    /* ---- early field defaults ---- */
    *(s32*)(p + 0x00) = 0xB0;
    *(s32*)(p + 0x04) = 0x800;
    *(s16*)(p + 0x18) = 0x10;
    *(s16*)(p + 0x1C) = 0x10;
    *(s16*)(p + 0x1A) = 0x60;
    *(u8*)(p + 0x74) = 0xFF;
    *(u8*)(p + 0x75) = 0xFF;
    *(s32*)(p + 0x40) = 0;
    *(s32*)(p + 0x44) = 0;
    *(s32*)(p + 0x48) = 0;
    *(s32*)(p + 0x30) = 0;
    *(s32*)(p + 0x34) = 0;
    *(s32*)(p + 0x38) = 0;
    *(s16*)(p + 0x64) = 0;
    *(s16*)(p + 0x60) = 0;
    *(s16*)(p + 0x62) = 0;
    *(s32*)(p + 0xD0) = 0;
    *(s32*)(p + 0xD4) = 0;
    *(s32*)(p + 0xD8) = 0;
    *(s16*)(p + 0xE6) = 0;
    *(s16*)(p + 0xEA) = 0xFF;
    *(u8*)(p + 0xE2) = 0;
    *(s16*)(p + 0xCC) = 0;
    *(s16*)(p + 0x6E) = 0;

    /* flags at 0x12C / 0x130 / 0x134: clear specific bits */
    *(s32*)(p + 0x12C) &= ~0x30000;
    *(s16*)(p + 0x1E) = *(s16*)(p + 0x18);
    *(s32*)(p + 0x12C) &= ~0x3;
    *(s16*)(p + 0x11E) = 0x200;
    *(u8*)(p + 0x101) = 0x80;
    *(u8*)(p + 0x100) = 0x80;
    *(u8*)(p + 0xFF)  = 0x80;
    *(u8*)(p + 0xFE)  = 0x80;
    *(u8*)(p + 0xFD)  = 0x80;
    *(u8*)(p + 0xFC)  = 0x80;
    *(s16*)(p + 0x128) = 0xFFFF;
    *(s32*)(p + 0x12C) &= 0xFFFCFFFF;
    *(s32*)(p + 0x130) &= 0xF007FFFF;
    *(s32*)(p + 0x130) &= ~0x200;
    *(s32*)(p + 0x12C) &= 0xF003FFFF;

    /* state array: 8 entries of 8 bytes each at offset 0x90 */
    for (i = 0; i < 8; i++) {
        u8* e = p + 0x90 + i * 8;
        *(s32*)(e + 0) = (*(s32*)(e + 0) & 0xFE7FFFFF) | 0x3C0000;
        *(u8*)(e + 0) = 0;
        *(s16*)(e + 2) = 0xFFFF;
        *(u8*)(e + 5) = 0xFF;
        *(s16*)(e + 6) = 0xFFFF;
    }

    *(s32*)(p + 0x120) = 0;
    *(s16*)(p + 0xE4) = 0xFF;
    *(s16*)(p + 0x76) = 0x100;
    *(s32*)(p + 0x12C) &= ~0x1C0;
    *(u8*)(p + 0x83) = 0;
    *(u8*)(p + 0x82) = 0;
    *(s16*)(p + 0x8A) = 0;
    *(s16*)(p + 0x88) = 0;
    *(s32*)(p + 0x84) = 0;
    *(u8*)(p + 0xCF) = 0;
    *(u8*)(p + 0xCE) = 0;
    *(s16*)(p + 0xE8) = 0;
    *(s16*)(p + 0x10) = 0;
    *(s16*)(p + 0xEC) = 0;
    *(s32*)(p + 0x134) &= ~0x80;
    *(s32*)(p + 0x12C) &= ~0xE00;
    *(s32*)(p + 0x12C) &= ~0x1000;
    *(s32*)(p + 0x134) &= ~0x60;

    *(s16*)(p + 0x102) = (s16)rand();
    *(s16*)(p + 0xF4) = 0x1000;
    *(s16*)(p + 0xF6) = 0x1000;
    *(s16*)(p + 0xF8) = 0x1000;
    *(u8*)(p + 0x10D) = 0xFF;
    *(u8*)(p + 0x80) = 0xFF;
    *(s16*)(p + 0x106) = -0x8000;
    *(s16*)(p + 0x104) = -0x8000;
    *(s16*)(p + 0x108) = -0x8000;
    *(s16*)(p + 0x124) = -1;
    *(u8*)(p + 0xE3) = 0;
    *(s16*)(p + 0x0E) = 0;
    *(s16*)(p + 0x0C) = 0;
    *(s16*)(p + 0x0A) = 0;
    *(s16*)(p + 0x08) = 0;
    *(s32*)(p + 0x12C) &= ~0x1C;

    /* ---- D_800AFB54 loop (distance-based init; skipped when count <= 1) ---- */
    if (D_800AFB54 > 1) {
        s16* pState = stateBuf;
        s16* pDst = (s16*)(p + 0x08);

        useStateBuf = 1;
        for (i = 0; i < D_800AFB54 - 1; i++) {
            s16 r = func_8007B1C4(
                *(s16*)(pActorBytes + 0x20),
                *(s16*)(pActorBytes + 0x28),
                i, (u8*)stateBuf + 0x40 + i * 8, pState);
            *pDst = r;
            if (r != -1 && (u32)r >= (u32)D_800AFB44[i]) {
                D_800AFB44[i] = 0;
                pState[0] = 0; pState[1] = 0; pState[2] = 0;
                pState[3] = 0; pState[4] = 0; pState[5] = 0;
            }
            pState += 8;
            pDst += 1;
        }

        /* status & 0x80 check + field_24 copy */
        if (!(*(s16*)(pActorBytes + 0x58) & 0x80)) {
            s16 choice = *(s16*)(p + 0x10);
            *(s32*)(pActorBytes + 0x24) = stateBuf[choice * 4 + (0x5A - 0x18)/2];
        }
    }

    /* ---- post-loop setup ---- */
    *(s32*)(p + 0x14) = func_80080968(p);
    {
        s16 choice = *(s16*)(p + 0x10);
        s16* stateBase = useStateBuf ? stateBuf : (s16*)(p + 0x18);
        *(s32*)(p + 0x50) = *(s32*)((u8*)stateBase + choice * 16 + 0);
        *(s32*)(p + 0x54) = *(s32*)((u8*)stateBase + choice * 16 + 4);
        *(s32*)(p + 0x58) = *(s32*)((u8*)stateBase + choice * 16 + 8);
    }

    /* Copy FieldActor fields 0x20/0x24/0x28 into ActorData */
    *(s32*)(p + 0x20) = *(s32*)(pActorBytes + 0x20) << 16;
    *(s32*)(p + 0x24) = *(s32*)(pActorBytes + 0x24) << 16;
    *(s32*)(p + 0x28) = *(s32*)(pActorBytes + 0x28) << 16;
    *(s16*)(p + 0x72) = *(s16*)(pActorBytes + 0x24);
}

/* ---- func_80080F44: per-actor data initialization ---------------------------
 * Called from FieldLoad for each actor (0..D_800ADBFC-1).
 * Allocates and zeroes the 0x138-byte ActorData block, sets up animation
 * dispatch if status & 0x2000, allocates a 0x70-byte shadow buffer, and calls
 * func_80080A74 / func_8007AA44 for further init.
 *
 * ASM-verified FieldActor byte offsets (struct uses u32 for PSX pointer fidelity):
 *   0x00 pModelData  0x04 pSpriteData  0x08 pShadow  0x4C pActorData
 *   0x50 rotation.x  0x52 rotation.y   0x54 rotation.z
 *   0x56 flags        0x58 status
 *   sizeof(FieldActor) = 0x5C
 *
 * ActorData byte offsets (allocated 0x138 bytes):
 *   0x110 unk110  0x114 unk114  0x118 pAnimTable  0x120 unk120
 *   0x124 unk124  0x12C flags12C  0x134 flags134
 */
extern s32 D_800B2180;
extern void func_8007AA44(void*);

void func_80080F44(s32 actorIndex) {
    FieldActor* pActor;
    ActorData* pData;
    s32 i;

    if (actorIndex >= D_800ADBFC) return;

    /* 1. Allocate and zero the ActorData block (sizeof == retail 0x138 now
     * that the 0x110 pointer run is u32 — see actor.h) */
    D_800B2180++;
    pData = (ActorData*)(uintptr_t)HeapAlloc(sizeof(ActorData), 0);
    g_FieldActors[actorIndex].pActorData = (u32)(uintptr_t)pData;

    for (i = 0; i < 0x4E; i++) {
        ((s32*)pData)[i] = 0;
    }

    /* 2. Zero field_5A (halfword at actor offset 0x5A, past status) */
    *(s16*)((u8*)&g_FieldActors[actorIndex] + 0x5A) = 0;

    /* 3. If status has bit 0x2000: allocate animation dispatch table */
    if (g_FieldActors[actorIndex].status & 0x2000) {
        u32* pModel = (u32*)(uintptr_t)g_FieldActors[actorIndex].pModelData;
        void* pAnimTable = HeapAlloc(0x80, 0);
        pData->unk118 = (u32)(uintptr_t)pAnimTable;

        if (pModel != NULL) {
            void* pAnimInfo = (void*)(uintptr_t)pModel[0x14 / 4];
            if (pAnimInfo != NULL) {
                s32 count = ((s32*)pAnimInfo)[0xC / 4];
                if (count > 0) {
                    void* pEntries = (void*)(uintptr_t)((u32*)pAnimInfo)[0x10 / 4];
                    for (i = 0; i < count; i++) {
                        ((u32*)pEntries)[i * 8] = (u32)(uintptr_t)func_80080A18;
                        ((s32*)pAnimTable)[i] = 0;
                    }
                }
            }
        }
    }

    /* 4. Per-actor init callback (stubbed in port) */
    func_80080A74(actorIndex);

    /* 5. Allocate 0x70-byte shadow buffer */
    g_FieldActors[actorIndex].pShadow = (u32)(uintptr_t)HeapAlloc(0x70, 0);
    func_8007AA44((void*)(uintptr_t)g_FieldActors[actorIndex].pShadow);
}

extern s32 D_800AF858;
extern s32 D_800ADC0C;
extern s32 D_800C3910;
extern s32 g_PlayerActorIndex;
extern u8 D_800B21CC;
extern char D_8006FC0C[];
extern char D_8006FC18[];
extern char D_8006FC24[];
extern char D_8006FC30[];
extern char D_8006FC3C[];
extern void func_800A2030(void);
extern void func_80281B00(void* arg0);
extern void func_80082620(s32 actorIndex, void* actor, void* actorData);
extern void func_80082BB8(s32 actorIndex, void* actor, void* actorData);
extern void func_800245D8(void* pSpriteData, s16 animIndex);
extern void func_800821F4(void* pSpriteData, s16 animIndex, void* pFieldActor);
extern void func_80084158(s32 actorIndex, void* actor, void* actorData);
extern void func_8008399C(s32 actorIndex, void* actor, void* actorData);
extern void func_800815F0(void);
extern s32 func_80084A40(s32 actorIndex, s32 y, void* pFieldActor, u8* actorData);

void func_8008110C(void) {
    s32 i;

    D_800C3910 = -1;
    func_800A2030();

    for (i = 0; i < D_800ADBFC; i++) {
        u8* actor = (u8*)g_FieldActors + i * 0x5C;
        u8* actorData = (u8*)(uintptr_t)*(u32*)(actor + 0x4C);

        *(s16*)(actorData + 0x68) = *(s16*)(actorData + 0x22);
        *(s16*)(actorData + 0x6A) = *(s16*)(actorData + 0x26);
        *(s16*)(actorData + 0x6C) = *(s16*)(actorData + 0x2A);
    }

    if (g_FieldSystemMode == 0) {
        func_80281B00(D_8006FC0C);
    }

    D_800AF858 = 0;

    for (i = 0; i < D_800ADBFC; i++) {
        u8* actor = (u8*)g_FieldActors + i * 0x5C;
        u8* actorData = (u8*)(uintptr_t)*(u32*)(actor + 0x4C);
        u16 status = *(u16*)(actor + 0x58);

        if ((status & 0x0F80) == 0x0200) {
            u32 flags0 = *(u32*)(actorData + 0x00);

            if ((flags0 & 0x00010001) == 0) {
                if ((*(u32*)(actorData + 0x04) & 0x600) != 0x200) {
                    *(s32*)(actorData + 0x14) = func_80080968(actorData);
                    func_80082620(i, actor, actorData);
                    func_80082BB8(i, actor, actorData);
                }
            } else if ((*(u32*)(actorData + 0x04) & 0x01000000) != 0 &&
                       (flags0 & 0x00010000) == 0) {
                if (*(s16*)(actorData + 0xE8) != *(s16*)(actorData + 0xEA)) {
                    *(s16*)(actorData + 0xEA) = 2;
                    *(s16*)(actorData + 0xE8) = *(u16*)(actorData + 0xEA);
                    func_800245D8((void*)(uintptr_t)*(u32*)(actor + 0x04),
                                  *(s16*)(actorData + 0xEA));
                }
            } else if ((*(u32*)(actorData + 0x04) & 0x00200000) != 0) {
                if (*(s16*)(actorData + 0xE8) != *(s16*)(actorData + 0xEA)) {
                    *(s16*)(actorData + 0xE8) = *(s16*)(actorData + 0xEA);
                    func_800821F4((void*)(uintptr_t)*(u32*)(actor + 0x04),
                                  *(s16*)(actorData + 0xEA), actor);
                }
            }
        }
    }

    if (g_FieldSystemMode == 0) {
        func_80281B00(D_8006FC18);
    }

    {
        u8* player = (u8*)g_FieldActors + g_PlayerActorIndex * 0x5C;
        func_80084158(g_PlayerActorIndex, player, (void*)(uintptr_t)*(u32*)(player + 0x4C));
    }

    if (g_FieldSystemMode == 0) {
        func_80281B00(D_8006FC24);
    }

    for (i = 0; i < D_800ADBFC; i++) {
        u8* actor = (u8*)g_FieldActors + i * 0x5C;
        u16 status = *(u16*)(actor + 0x58);

        if ((status & 0x0F00) != 0) {
            u8* actorData = (u8*)(uintptr_t)*(u32*)(actor + 0x4C);

            if ((*(u32*)(actorData + 0x04) & 0x600) != 0x200 &&
                (status & 0x0F80) == 0x0200 &&
                ((*(u32*)(actorData + 0x00) & 0x00010001) == 0) &&
                i != g_PlayerActorIndex) {
                func_80084A40(i, 0x7FFFFFFF, actor, actorData);

                {
                    u8* spriteData = (u8*)(uintptr_t)*(u32*)(actor + 0x04);
                    u8* frameData = (u8*)(uintptr_t)*(u32*)(spriteData + 0x7C);

                    if (*(u16*)(frameData + 0x0C) == 1) {
                        *(u32*)(actorData + 0x00) &= ~0x800u;
                    }
                }
            }
        }
    }

    if (g_FieldSystemMode == 0) {
        func_80281B00(D_8006FC30);
    }

    if (g_FieldControl.isRandomEncountersEnabled == 0 && D_800B21CC == 0) {
        u8* player = (u8*)g_FieldActors + g_PlayerActorIndex * 0x5C;
        func_8008399C(g_PlayerActorIndex, player, (void*)(uintptr_t)*(u32*)(player + 0x4C));
    }

    D_800ADC0C = 1;
    func_800815F0();

    if (g_FieldSystemMode == 0) {
        func_80281B00(D_8006FC3C);
    }
}

extern s16 D_800B234E;

void func_800815F0(void) {
    s32 i;

    for (i = 0; i < D_800ADBFC; i++) {
        u8* actor = (u8*)g_FieldActors + i * 0x5C;
        u8* actorData = (u8*)(uintptr_t)*(u32*)(actor + 0x4C);

        if ((*(u32*)(actorData + 0x00) & 0x01000000) == 0) {
            continue;
        }

        if (i == g_PlayerActorIndex || (*(u16*)(actor + 0x58) & 0x20) != 0) {
            continue;
        }

        if (D_800B234E != 0) {
            u8* spriteData = (u8*)(uintptr_t)*(u32*)(actor + 0x04);
            s16 anim = *(s16*)(actorData + 0xE6);

            if (*(s16*)(actorData + 0xE8) != anim) {
                *(s16*)(actorData + 0xE8) = anim;
                if (anim < 0) {
                    *(s16*)(actorData + 0xE8) = 0;
                }
                func_800821F4(spriteData, *(s16*)(actorData + 0xE8), actor);
            }
        } else {
            assert(!"func_800815F0 party-history sync branch not migrated");
        }
    }
}

extern s32 g_PlayerActorIndex;
extern u8 D_800B21CC;
extern s32 D_800B2360;
extern s32 D_800C3910;
extern u8 D_800B14F0;
extern u8 D_800B14F4;
extern u8 D_800B14F8;
extern u8 D_800B14FA;
extern u8 D_800B14FC;
extern u8 D_800B1500;
extern u8 D_800B1502;
extern u8 D_800B1504;
extern u8 D_800B1510;
extern u8 D_800B1514;
extern u8 D_800B1518;
extern u8 D_800B1530;
extern u8 D_800B1534;

void func_80081C54(s32 actorIndex) {
    u8* actor = (u8*)g_FieldActors + actorIndex * 0x5C;
    u8* actorData = (u8*)(uintptr_t)*(u32*)(actor + 0x4C);
    u8* spriteData = (u8*)(uintptr_t)*(u32*)(actor + 0x04);
    s32 slot;
    s32 off;
    s32 i;

    if (actorIndex != g_PlayerActorIndex) {
        return;
    }

    if (D_800B21CC != 0) {
        return;
    }

    slot = D_800B2360;
    off = (slot * 9) * 8;

    *(u32*)(&D_800B1510 + off) = *(u32*)(spriteData + 0x0C);
    *(u32*)(&D_800B1514 + off) = *(u32*)(spriteData + 0x10);
    *(u32*)(&D_800B1518 + off) = *(u32*)(spriteData + 0x14);

    *(u32*)(&D_800B1510 + off + 0x10) = *(u32*)(actorData + 0x50);
    *(u32*)(&D_800B1510 + off + 0x14) = *(u32*)(actorData + 0x54);
    *(u32*)(&D_800B1510 + off + 0x18) = *(u32*)(actorData + 0x58);

    *(u16*)(&D_800B1504 + off) = *(u16*)(actorData + 0x106) & 0x0FFF;
    *(u16*)(&D_800B1500 + off) = *(u16*)(spriteData + 0x84);
    *(u16*)(&D_800B14F8 + off) = *(u16*)(actorData + 0x22);
    *(u16*)(&D_800B14FA + off) = *(u16*)(actorData + 0x26);
    *(u16*)(&D_800B14FC + off) = *(u16*)(actorData + 0x2A);
    *(u16*)(&D_800B1502 + off) = *(u16*)(actorData + 0xE8);
    *(u32*)(&D_800B1530 + off) = *(u32*)(actorData + 0x14);
    *(u32*)(&D_800B14F0 + off) = *(u32*)(actorData + 0x00);
    *(u32*)(&D_800B14F4 + off) = *(u32*)(actorData + 0x04);

    for (i = 0; i < 4; i++) {
        *(u16*)(&D_800B1510 + off - 0x0A + i * 2) = *(u16*)(actorData + 0x08 + i * 2);
    }

    *(&D_800B1534 + off) = *(u16*)(actorData + 0x10);
    D_800C3910 = 0;
    D_800B2360 = (D_800B2360 - 1) & 0x1F;
}

s32 func_80081F5C(u32* a0) {
    u32 a = (a0[0] >> 9) & 3;
    u32 b = a0[5] >> 3;
    return -((a & b) != 0);
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc8", func_80081F80);

extern s16 D_800B2344;
extern s16 D_800B2346;

void func_800821F4(void* pSpriteData, s16 animIndex, void* pFieldActor) {
    u8* actor = pFieldActor;
    u8* actorData;
    s16 nextAnim = animIndex;
    u32 flags4;

    if ((*(u16*)(actor + 0x58) & 0x40) == 0) {
        return;
    }

    actorData = (u8*)(uintptr_t)*(u32*)(actor + 0x4C);

    if (nextAnim != 3 && D_800B2344 == 0) {
        *(u32*)(actorData + 0x00) &= ~0x800u;
    }

    if (nextAnim == 0xFF) {
        nextAnim = 0;
    }

    if (nextAnim != D_800B2346) {
        *(u32*)(actorData + 0x00) &= ~0x800u;
    }

    flags4 = *(u32*)(actorData + 0x04);
    if ((flags4 & 0x2000) == 0) {
        if ((flags4 & 0x01000000) == 0) {
            func_800245D8(pSpriteData, nextAnim);
        }
        return;
    }

    assert(!"func_800821F4 battle animation branch not migrated");
}

s32 func_8008237C(s32 x, s32 z, void* pActorData, s32 extraRadius) {
    u8* actorData = pActorData;
    s32 xMin;
    s32 xMax;
    s32 zMin;
    s32 zMax;
    long actorPos;
    long p0;
    long p1;
    long p2;
    long p3;

    actorPos = (x << 16) + z;
    xMin = *(s16*)(actorData + 0x22) - *(u16*)(actorData + 0x18) - extraRadius;
    xMax = *(s16*)(actorData + 0x22) + *(u16*)(actorData + 0x18) + extraRadius;
    zMin = *(s16*)(actorData + 0x2A) - *(u16*)(actorData + 0x1C) - extraRadius;
    zMax = *(s16*)(actorData + 0x2A) + *(u16*)(actorData + 0x1C) + extraRadius;

    p0 = (xMin << 16) + zMax;
    p1 = (xMax << 16) + zMax;
    p2 = (xMax << 16) + zMin;
    p3 = (xMin << 16) + zMin;

    if (NormalClip(p0, p1, actorPos) < 0 ||
        NormalClip(p1, p2, actorPos) < 0 ||
        NormalClip(p2, p3, actorPos) < 0 ||
        NormalClip(p3, p0, actorPos) < 0) {
        return -1;
    }

    if (g_FieldSystemMode == 0) {
        func_80281678(actorData);
    }
    return 0;
}

s32 func_80082494(s32* pVec, u8* actorData) {
    (void)pVec;

    if ((*(u32*)(actorData + 0x12C) & 0x1000) == 0) {
        return 0;
    }

    assert(!"func_80082494 clipping branch not migrated");
    return 0;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc8", func_800825AC);

extern u16 D_800ADFA8[];
extern s16 D_800ADFC4[];
extern void func_8007B614(s32* pOut, s16 scale, s16 angle);

void func_80082620(s32 actorIndex, void* pFieldActor, void* pActorData) {
    u8* actor = pFieldActor;
    u8* actorData = pActorData;
    s32 moveVec[3];
    u32 moveFlags = 0;
    u32 flags0 = *(u32*)(actorData + 0x00);
    u32 flags4 = *(u32*)(actorData + 0x04);
    s32 shift = *(s16*)(actorData + 0x10) + 3;
    s16 scale;
    u16 angle;

    (void)actorIndex;

    if (((flags4 >> shift) & 1) == 0) {
        if (D_800B21CC == 0) {
            moveFlags = *(u32*)(actorData + 0x14);
        }
    }

    scale = D_800ADFC4[((moveFlags >> 8) & 0x6) >> 1];
    angle = (D_800ADFA8[((moveFlags >> 10) & 0xE) >> 1] +
             *(u16*)((u8*)&g_FieldControl + 0x2)) & 0x0FFF;
    func_8007B614(moveVec, scale, angle);

    if ((flags0 & 0x00041800) != 0) {
        if (moveFlags & 0x00004000) {
            *(s32*)(actorData + 0x40) += moveVec[0];
            *(s32*)(actorData + 0x44) += moveVec[1];
            *(s32*)(actorData + 0x48) += moveVec[2];
        }
        assert(*(u8*)(actorData + 0x74) == 0xFF);
        assert((flags4 & 0x00022000) != 0x00022000);
        return;
    }

    /* asm .L80082888: pending-movement application — add the computed move
     * vector into the actor's position accumulators, then continue. */
    if (moveFlags & 0x00008000) {
        *(s32*)(actorData + 0x40) += moveVec[0];
        *(s32*)(actorData + 0x44) += moveVec[1];
        *(s32*)(actorData + 0x48) += moveVec[2];
    }

    if ((moveFlags & 0x00420000) != 0 ||
        (moveFlags & 0x00400000) != 0 ||
        *(u8*)(actorData + 0x74) != 0xFF ||
        (moveFlags & 0x00020000) != 0 ||
        (flags4 & 0x00022000) == 0x00022000) {
        assert(!"func_80082620 unsupported actor movement branch");
    }

    (void)actor;
}

extern s32 D_8005A448;
extern s32 D_8005A44C;
extern s16 D_800B2344;
extern s16 D_800B2346;
extern s32 D_800ADB68;
extern s32 D_800ADB98;
extern u16 D_800AFE9C;
extern void* func_8007B814(s32* pVec, void* pActorData, s32* pOut, s16 angle);
extern void* func_8007BAC0(s32* pVec, void* pActorData, s32* pOut, s16 angle);

extern s32 D_80065B08;

void func_80082BB8(s32 actorIndex, void* pFieldActor, void* pActorData) {
    u8* actor = pFieldActor;
    u8* actorData = pActorData;
    u8* spriteData = (u8*)(uintptr_t)*(u32*)(actor + 0x04);
    s16 angle = *(s16*)(actorData + 0x104);
    s32 anim = 1;
    s32 moveOrBlocked;
    s32 vec[3];
    s32 out[4];
    void* result = (void*)-1;
    u32 flags0;
    u32 flags4;

    D_80065B08 = actorIndex;

    flags0 = *(u32*)(actorData + 0x00);
    if ((flags0 & 0x01000000) != 0) {
        return;
    }

    if ((flags0 & 0x4000) != 0 &&
        (D_800AFE9C & 0x40) != 0 &&
        D_800ADB68 == 1) {
        anim = 2;
    }

    if ((flags0 & 0x1800) != 0) {
        s16 curAnim = *(s16*)(actorData + 0xE8);
        if (curAnim != anim) {
            if (curAnim == 1) {
                anim = 1;
            } else if (curAnim == 2) {
                anim = 2;
            }
        }
    }

    if (*(u8*)(actorData + 0xE3) >= 9) {
        *(u8*)(actorData + 0xE3) -= 1;
    }

    moveOrBlocked = *(s32*)(actorData + 0x40) |
                    *(s32*)(actorData + 0x44) |
                    *(s32*)(actorData + 0x48);
    if (func_8008492C(actorData) == -1) {
        moveOrBlocked = 1;
    }

    if ((angle & 0x8000) != 0 && moveOrBlocked == 0) {
        if ((*(u32*)(actorData + 0x00) & 0x00040800) == 0) {
            *(u32*)(actorData + 0xF0) = 0x00010000;
            *(s32*)(actorData + 0x40) = 0;
            *(s32*)(actorData + 0x44) = 0;
            *(s32*)(actorData + 0x48) = 0;
            vec[0] = 0;
            vec[1] = 0;
            vec[2] = 0;
            *(s32*)(spriteData + 0x0C) = 0;
            *(s32*)(spriteData + 0x14) = 0;
            *(u16*)(actorData + 0x106) |= 0x8000;
            goto update_animation;
        }

        anim = *(u16*)(actorData + 0xE6);
        *(u16*)(actorData + 0x104) |= 0x8000;
        *(u32*)(actorData + 0xF0) = 0x00010000;
        *(s32*)(actorData + 0x40) = 0;
        *(s32*)(actorData + 0x44) = 0;
        *(s32*)(actorData + 0x48) = 0;
        vec[0] = 0;
        vec[1] = 0;
        vec[2] = 0;
        *(s32*)(spriteData + 0x0C) = 0;
        *(s32*)(spriteData + 0x14) = 0;
        *(u16*)(actorData + 0x106) |= 0x8000;
        goto update_animation;
    }

    if ((angle & 0x8000) == 0) {
        func_80081F80(spriteData, angle, actor);
        vec[0] = *(s32*)(spriteData + 0x0C) + *(s32*)(actorData + 0x40);
        vec[1] = *(s32*)(spriteData + 0x10) + *(s32*)(actorData + 0x44);
        vec[2] = *(s32*)(spriteData + 0x14) + *(s32*)(actorData + 0x48);
        *(s16*)(actorData + 0x106) = angle;
    } else {
        vec[0] = *(s32*)(actorData + 0x40);
        vec[1] = *(s32*)(actorData + 0x44);
        vec[2] = *(s32*)(actorData + 0x48);
        angle = *(u16*)(actorData + 0x106) & 0x0FFF;
    }

    if (func_80082494(vec, actorData) != 0) {
        *(u32*)(actorData + 0xF0) = 0x00010000;
        *(s32*)(actorData + 0x40) = 0;
        *(s32*)(actorData + 0x44) = 0;
        *(s32*)(actorData + 0x48) = 0;
        vec[0] = 0;
        vec[1] = 0;
        vec[2] = 0;
        *(s32*)(spriteData + 0x0C) = 0;
        *(s32*)(spriteData + 0x14) = 0;
        *(u16*)(actorData + 0x106) |= 0x8000;
        goto update_animation;
    }

    if (vec[0] != 0 || vec[2] != 0) {
        angle = (-ratan2(vec[2], vec[0])) & 0x0FFF;
    }

    {
        s16 stateIdx = *(s16*)(actorData + 0x10);
        s16 stateVal = *(s16*)(actorData + 0x08 + stateIdx * 2);

        if (stateVal != -1) {
            u32 savedFlags0;

            if (actorIndex == g_PlayerActorIndex) {
                if (D_8005A448 != 0xFF) {
                    u8* other = (u8*)g_FieldActors + D_8005A448 * 0x5C;
                    u8* otherData = (u8*)(uintptr_t)*(u32*)(other + 0x4C);
                    *(u32*)(actorData + 0x00) |= *(u32*)(otherData + 0x00) & 0x600;
                }
                if (D_8005A44C != 0xFF) {
                    u8* other = (u8*)g_FieldActors + D_8005A44C * 0x5C;
                    u8* otherData = (u8*)(uintptr_t)*(u32*)(other + 0x4C);
                    *(u32*)(actorData + 0x00) |= *(u32*)(otherData + 0x00) & 0x600;
                }
            }

            savedFlags0 = *(u32*)(actorData + 0x00);
            if ((savedFlags0 & 0x00041800) != 0 ||
                *(u8*)(actorData + 0x74) != 0xFF ||
                D_800ADB98 != 0) {
                result = func_8007B814(vec, actorData, out, angle);
            } else {
                result = func_8007BAC0(vec, actorData, out, angle);
            }

            *(u32*)(actorData + 0x00) =
                (*(u32*)(actorData + 0x00) & ~0x600u) | (savedFlags0 & 0x600);
        }
    }

    if (result != (void*)-1) {
        *(u32*)(actorData + 0x04) &= ~0x1000u;
        goto update_animation;
    }

    *(u32*)(actorData + 0xF0) = 0x00010000;
    *(s32*)(actorData + 0x40) = 0;
    *(s32*)(actorData + 0x44) = 0;
    *(s32*)(actorData + 0x48) = 0;
    vec[0] = 0;
    vec[1] = 0;
    vec[2] = 0;
    *(s32*)(spriteData + 0x0C) = 0;
    *(s32*)(spriteData + 0x14) = 0;
    *(u16*)(actorData + 0x106) |= 0x8000;

update_animation:
    flags4 = *(u32*)(actorData + 0x04) & ~0x1000u;
    *(u32*)(actorData + 0x04) = flags4;

    if ((*(u32*)(actorData + 0x00) & 0x800) != 0) {
        if (D_800B2344 == 0 &&
            *(s16*)(spriteData + 0x06) != *(s16*)(spriteData + 0x84)) {
            if ((s16)anim == 2) {
                *(s32*)(spriteData + 0x18) =
                    ((s32)*(s16*)(spriteData + 0x82) * 3) << 5;
            } else {
                *(s32*)(spriteData + 0x18) =
                    ((s32)*(s16*)(spriteData + 0x82) * 3) << 4;
            }
        } else if (D_800B2344 == 0) {
            *(s32*)(spriteData + 0x18) = 0;
        }
        anim = D_800B2346;
    } else {
        if ((*(s16*)(actorData + 0x104) & 0x8000) != 0) {
            anim = *(u16*)(actorData + 0xE6);
        }

        if ((func_80080968(actorData) & 0x00200000) != 0) {
            if ((*(s16*)(actorData + 0x104) & 0x8000) == 0 ||
                *(s16*)(actorData + 0xE8) != 6) {
                anim = 6;
            } else {
                *(u32*)(actorData + 0x04) |= 0x1000u;
                anim = 6;
            }
        }
    }

    if (*(s16*)(actorData + 0xEA) != 0xFF) {
        anim = *(s16*)(actorData + 0xEA);
    }

    if (*(s16*)(actorData + 0xE8) != (s16)anim &&
        (*(u32*)(actorData + 0x00) & 0x02000000) == 0) {
        *(s16*)(actorData + 0xE8) = anim;
        func_800821F4(spriteData, anim, actor);
    }

    if ((*(u32*)(actorData + 0x14) & 0x100) != 0) {
        vec[0] >>= 1;
        vec[2] >>= 1;
    }

    *(s32*)(actorData + 0x30) = vec[0];
    *(s32*)(actorData + 0x34) = vec[1];
    *(s32*)(actorData + 0x38) = vec[2];
    *(s32*)(actorData + 0x40) = 0;
    *(s32*)(actorData + 0x44) = 0;
    *(s32*)(actorData + 0x48) = 0;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc8", func_80083178);

void func_800831D0(SVECTOR* out, s16* in) {
    out->vx = in[1];
    out->vy = in[3];
    out->vz = in[5];
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc8", func_800831F4);

INCLUDE_ASM("asm/field/nonmatchings/main/misc8", func_80083288);

void func_80083994(void) {
}

extern u16 D_800C2694;
extern u_short FieldScriptGetBytecodeOffset(int scriptIndex, int routineIndex);

void func_8008399C(s32 actorIndex, void* pFieldActor, void* pActorData) {
    u8* actorData = (u8*)pActorData;
    s32 playerY = *(s16*)(actorData + 0x26);
    s32 playerFloor = playerY - *(u16*)(actorData + 0x1A);
    s32 innerRadius = *(u16*)(actorData + 0x1E) + 8;
    s32 outerRadius = *(u16*)(actorData + 0x1E) + 0x20;
    s32 playerRot = *(u16*)(actorData + 0x106) & 0x0FFF;
    s32 playerX = *(s16*)(actorData + 0x22);
    s32 playerZ = *(s16*)(actorData + 0x2A);
    s32 found = 0;
    s32 defaultScriptId = 7;
    s32 i;

    (void)pFieldActor;

    for (i = 0; i < D_800ADBFC; i++) {
        u8* otherActor = (u8*)g_FieldActors + i * 0x5C;
        u8* otherData = (u8*)(uintptr_t)*(u32*)(otherActor + 0x4C);
        u32 otherFlags0;
        u32 otherFlags4;
        s32 otherY;
        s32 dx;
        s32 dz;
        s32 radius;
        s32 vec[3];
        s32 sq[3];
        s32 scriptId = 0xFF;
        s32 scriptRoutine = defaultScriptId;
        s32 forceInteraction = 0;

        otherFlags0 = *(u32*)(otherData + 0x00);
        if (otherFlags0 & 0x1) {
            continue;
        }

        if (*(u8*)(actorData + 0x74) == i) {
            *(u8*)(actorData + 0x74) = 0xFF;
            continue;
        }

        otherY = *(s16*)(otherData + 0x26) + *(s16*)(otherData + 0x62);
        otherFlags4 = *(u32*)(otherData + 0x04);

        if (otherFlags4 & 0x180) {
            assert(!"func_8008399C button/special interaction branch not migrated");
        }

        dx = *(s16*)(otherData + 0x22) - playerX + *(s16*)(otherData + 0x60);
        dz = *(s16*)(otherData + 0x2A) - playerZ + *(s16*)(otherData + 0x64);

        if (otherFlags0 & 0x2000) {
            if (otherY < playerFloor) {
                continue;
            }
            if (playerY < otherY - *(u16*)(otherData + 0x1A)) {
                continue;
            }
            if (i == actorIndex) {
                continue;
            }
            if (func_8008237C(playerX, playerZ, otherData, 0x10) != 0) {
                continue;
            }
            forceInteraction = 1;
        }

        radius = outerRadius + *(u16*)(otherData + 0x1E);
        vec[0] = dx;
        vec[1] = radius;
        vec[2] = dz;
        Square0((VECTOR*)vec, (VECTOR*)sq);

        if (!forceInteraction && sq[0] + sq[2] >= sq[1]) {
            continue;
        }
        if (otherY < playerFloor) {
            continue;
        }
        if (playerY < otherY - *(u16*)(otherData + 0x1A)) {
            continue;
        }
        if (i == actorIndex) {
            continue;
        }

        vec[0] = dx;
        vec[1] = innerRadius + *(u16*)(otherData + 0x1E);
        vec[2] = dz;
        Square0((VECTOR*)vec, (VECTOR*)sq);

        {
            s32 dist = sq[0] + sq[2];
            s32 innerLimit[3];
            s32 innerLimitSq[3];

            innerLimit[0] = innerRadius + *(u16*)(otherData + 0x1E);
            innerLimit[1] = 0;
            innerLimit[2] = outerRadius + *(u16*)(otherData + 0x1E);
            Square0((VECTOR*)innerLimit, (VECTOR*)innerLimitSq);

            if (forceInteraction || dist < innerLimitSq[0]) {
                if (D_800C2694 & 0x20) {
                    assert(!"func_8008399C confirm-button branch not migrated");
                }

                if ((otherFlags0 & 0x00A20000) == 0) {
                    s32 angle = ratan2(dz, dx);
                    s32 dir = (-angle >> 9) & 7;

                    scriptId = 3;
                    scriptRoutine = 4;
                    *(u32*)(otherData + 0x12C) =
                        (*(u32*)(otherData + 0x12C) & ~0xE00u) | (dir << 9);
                }
            }
        }

        if (scriptId != 0xFF) {
            s32 slot;
            u8* slotBase;

            for (slot = 0, slotBase = otherData; slot < 8; slot++, slotBase += 8) {
                if (*(u8*)(slotBase + 0x8F) == (u8)scriptId) {
                    break;
                }
            }

            if (slot == 8) {
                for (slot = 0, slotBase = otherData; slot < 8; slot++, slotBase += 8) {
                    u32 word = *(u32*)(slotBase + 0x90);
                    if (((word >> 18) & 0xF) == 0xF && ((word >> 22) & 1) == 0) {
                        u16 ip = FieldScriptGetBytecodeOffset(i, scriptId);
                        *(u16*)(slotBase + 0x8C) = ip;
                        *(u8*)(slotBase + 0x8F) = scriptId;
                        *(u32*)(slotBase + 0x90) =
                            (word & 0xFFC3FFFFu) | (scriptRoutine << 18);
                        *(u16*)(otherData + 0x106) |= 0x8000;
                        *(u16*)(otherData + 0x104) = *(u16*)(otherData + 0x106);
                        break;
                    }
                }
            }
        }

        (void)found;
    }
}

extern void* func_8007CD3C(s32 arg0);
extern void func_8007CD60(s32 arg0);
extern char D_8006FC58[];
extern s32 D_800ADB98;

void func_80084158(s32 actorIndex, void* pFieldActor, void* pActorData) {
    u8* actor = (u8*)pFieldActor;
    u8* actorData = (u8*)pActorData;
    s32 predicted[3];
    SVECTOR currentPos;
    s32 targetYMin;
    u32 actorFlags0;
    u32 actorFlags4;
    u32 actorFlags14;
    u8 oldInteractActor;
    s32 selectedY;
    s32 hasTarget;
    s32 targetState;
    s32 i;
    s32 scratch[8];

    func_8007CD3C(0x20);

    predicted[0] = *(s32*)(actorData + 0x20) + *(s32*)(actorData + 0x30);
    predicted[1] = *(s32*)(actorData + 0x24) + *(s32*)(actorData + 0x34);
    predicted[2] = *(s32*)(actorData + 0x28) + *(s32*)(actorData + 0x38);
    func_800831D0(&currentPos, (s16*)predicted);

    targetYMin = *(s16*)(actorData + 0x26) - *(u16*)(actorData + 0x1A);
    actorFlags0 = *(u32*)(actorData + 0x00);
    actorFlags4 = *(u32*)(actorData + 0x04);
    actorFlags14 = *(u32*)(actorData + 0x14);
    oldInteractActor = *(u8*)(actorData + 0x74);
    selectedY = 0x7FFFFFFF;
    hasTarget = 0;
    targetState = 0;

    for (i = 0; i < D_800ADBFC; i++) {
        u8* otherActor;
        u8* otherData;
        u32 otherFlags0;
        u32 otherFlags4;
        s32 floorY;

        if (i == actorIndex) {
            continue;
        }

        otherActor = (u8*)g_FieldActors + i * 0x5C;
        otherData = (u8*)(uintptr_t)*(u32*)(otherActor + 0x4C);
        otherFlags0 = *(u32*)(otherData + 0x00);
        if (otherFlags0 & 0x1) {
            continue;
        }

        otherFlags4 = *(u32*)(otherData + 0x04);
        *(u32*)(otherData + 0x04) = otherFlags4 & 0xFFFF3EFF;

        if (otherFlags4 & 0x80) {
            assert(!"func_80084158 func_80083288 branch not migrated");
        }

        if (otherFlags0 & 0x2000) {
            if (func_8008237C(currentPos.vx, currentPos.vz, otherData, 0) != 0) {
                *(u32*)(otherData + 0x04) &= 0xFF3FFFFF;
                continue;
            }
            scratch[4] = 0;
            scratch[5] = 1;
            scratch[6] = 0;
        } else {
            scratch[0] = ((*(s32*)(otherData + 0x20) + *(s32*)(otherData + 0x30)) >> 16) - currentPos.vx;
            scratch[1] = *(u16*)(actorData + 0x1E) + *(u16*)(otherData + 0x1E);
            scratch[2] = ((*(s32*)(otherData + 0x28) + *(s32*)(otherData + 0x38)) >> 16) - currentPos.vz;
            Square0((VECTOR*)scratch, (VECTOR*)&scratch[4]);
        }

        if (scratch[4] + scratch[6] < scratch[5]) {
            if (actorFlags14 & 0x00400000) {
                if (g_FieldSystemMode == 0) {
                    func_800379C8(D_8006FC58);
                }
                continue;
            }

            if ((otherFlags0 | actorFlags0) & 0x80) {
                continue;
            }

            if (D_800B21CC != 0) {
                continue;
            }

            floorY = *(s16*)(otherData + 0x26) - *(u16*)(otherData + 0x1A);
        } else {
            floorY = 0x7FFFFFFF;
        }

        if (floorY < targetYMin) {
            *(u32*)(otherData + 0x04) &= ~0x100;
            if (*(s16*)(actorData + 0x26) < floorY) {
                *(u32*)(otherData + 0x04) |= 0x00800000;
                if (floorY < selectedY) {
                    selectedY = floorY;
                }
            } else {
                *(u32*)(otherData + 0x04) &= ~0x00800000;
            }
        } else {
            *(u32*)(otherData + 0x04) |= 0x00400000;
        }
    }

    if (D_800ADB98 != 0) {
        extern s32 D_800ADB94;
        selectedY = D_800ADB94;
        hasTarget = 0;
        targetState++;
    }

    if (hasTarget == 0) {
        *(u8*)(actorData + 0x74) = 0xFF;
    } else {
        assert(!"func_80084158 interaction target branch not migrated");
    }

    if ((*(u32*)(actorData + 0x00) & 0x00010000) == 0 &&
        (*(u32*)(actorData + 0x04) & 0x00200000) == 0) {
        func_80084A40(actorIndex, selectedY, actor, actorData);
    }

    {
        u8* spriteData = (u8*)(uintptr_t)*(u32*)(actor + 0x04);
        u8* spriteInner = (u8*)(uintptr_t)*(u32*)(spriteData + 0x7C);

        if (*(u16*)(spriteInner + 0x0C) == 1) {
            ControllerResetState();
            *(u32*)(actorData + 0x00) &= ~0x800;
        }
    }

    (void)oldInteractActor;
    (void)targetState;
    func_8007CD60(0x20);
}

extern s32 D_800ADB98;
extern s32 D_800ADC0C;

s32 func_8008492C(u8* actorData) {
    if (*(u32*)(actorData + 0x14) & 0x00420000) {
        return -1;
    }

    if (D_800ADB98 != 0) {
        return -1;
    }

    if (*(u32*)(actorData + 0x30) != 0) {
        return -1;
    }

    if (*(u32*)(actorData + 0x34) != 0) {
        return -1;
    }

    if (*(u32*)(actorData + 0x38) != 0) {
        return -1;
    }

    if (D_800ADC0C != 1) {
        return -1;
    }

    if (*(u8*)(actorData + 0x74) != 0xFF) {
        return -1;
    }

    if (*(u32*)(actorData + 0x00) & 0x00401800) {
        return -1;
    }

    if ((*(u32*)(actorData + 0x04) & 0x1) && *(s16*)(actorData + 0x10) == 0) {
        return -1;
    }

    if ((*(u32*)(actorData + 0x04) & 0x2) && *(s16*)(actorData + 0x10) == 1) {
        return -1;
    }

    if (*(u32*)(actorData + 0x04) & 0x4) {
        return -((*(s16*)(actorData + 0x10) == 2) != 0);
    }

    return 0;
}

extern s16 D_800ADB00;
extern u8 D_800B21CF;
extern s32 g_FieldSystemMode;
extern char D_8006FC60[];
extern char D_8006FC74[];
extern void func_800379C8(char* arg0, s32 arg1);
extern s32 func_8007D3D4(u8* actorData, s32 idx, s32* outHeight0,
                         VECTOR* outNormal, s16* outTriangle, s32* outHeight1);

static void func_80084A40_RestoreActorState(u8* actorData, u8* spriteData,
                                            s32 actorIndex, s32 origX,
                                            s32 origZ, s16 origState,
                                            s16 savedStates[4]) {
    s32 i;
    u8* actor = (u8*)g_FieldActors + actorIndex * 0x5C;

    *(s32*)(actorData + 0x20) = origX;
    *(s32*)(actorData + 0x28) = origZ;
    *(s16*)(actorData + 0x10) = origState;
    *(s32*)(actorData + 0xF0) = 0;

    for (i = 0; i < 4; i++) {
        *(s16*)(actorData + 0x08 + i * 2) = savedStates[i];
    }

    if (*(s16*)(spriteData + 0x84) != *(s16*)(actorData + 0x26)) {
        *(s32*)(spriteData + 0x10) += *(s32*)(spriteData + 0x1C);
    }

    if (*(s32*)(spriteData + 0x10) < 0) {
        *(s32*)(spriteData + 0x10) = 0;
        *(s32*)(actorData + 0x24) = *(s32*)(spriteData + 0x04);
    }

    *(s32*)(spriteData + 0x00) = *(s32*)(actorData + 0x20);
    *(s32*)(spriteData + 0x04) = *(s32*)(actorData + 0x24);
    *(s32*)(spriteData + 0x08) = *(s32*)(actorData + 0x28);
    *(s32*)(actor + 0x24) = *(s16*)(actorData + 0x26);
}

s32 func_80084A40(s32 actorIndex, s32 y, void* pFieldActor, u8* actorData) {
    u8* actor = (u8*)g_FieldActors + actorIndex * 0x5C;
    u8* spriteData = (u8*)(uintptr_t)*(u32*)(actor + 0x04);
    s32 origX;
    s32 origY;
    s32 origZ;
    s16 origState;
    s16 savedStates[4];
    s32 low[4];
    s32 high[4];
    s32 stateIds[4];
    s16 triIds[4];
    VECTOR normals[4];
    s32 layer;
    s32 i;
    s32 currentLow;
    s32 stateFlags;

    (void)pFieldActor;

    if (actorIndex == g_PlayerActorIndex) {
        D_800ADB00 = -1;
    }

    if (*(u32*)(actorData + 0x00) & 0x01000000) {
        return -1;
    }
    if (*(u32*)(actorData + 0x04) & 0x00200000) {
        return -1;
    }

    if ((*(u32*)(actorData + 0x00) & 0x00010000) == 0) {
        if (actorIndex == g_PlayerActorIndex && D_800B21CF == 1) {
            goto run_collision;
        }
        if (*(s32*)(spriteData + 0x10) != 0) {
            goto run_collision;
        }
        if (func_8008492C(actorData) != 0) {
            goto run_collision;
        }
        if (*(s16*)(spriteData + 0x84) != *(s16*)(actorData + 0x26)) {
            goto run_collision;
        }
        return -1;
    }

run_collision:
    origX = *(s32*)(actorData + 0x20);
    origY = *(s32*)(actorData + 0x24);
    origZ = *(s32*)(actorData + 0x28);
    origState = *(s16*)(actorData + 0x10);

    for (i = 0; i < 4; i++) {
        savedStates[i] = *(s16*)(actorData + 0x08 + i * 2);
        low[i] = 0x7FFFFFFF;
        high[i] = 0x7FFFFFFF;
        stateIds[i] = i;
        triIds[i] = 0;
        normals[i].vx = 0;
        normals[i].vy = 0;
        normals[i].vz = 0;
        normals[i].pad = 0;
    }

    layer = 0;
    if (D_800AFB54 - 1 > 0) {
        for (layer = 0; layer < D_800AFB54 - 1; layer++) {
            if (func_8007D3D4(actorData, layer, &low[layer], &normals[layer],
                              &triIds[layer], &high[layer]) != 0) {
                break;
            }
        }
    }

    if (*(u32*)(actorData + 0x04) & 0x1) {
        low[0] = 0x7FFFFFFF;
        high[0] = 0x7FFFFFFF;
    }
    if (*(u32*)(actorData + 0x04) & 0x2) {
        low[1] = 0x7FFFFFFF;
        high[1] = 0x7FFFFFFF;
    }
    if (*(u32*)(actorData + 0x04) & 0x4) {
        low[2] = 0x7FFFFFFF;
        high[2] = 0x7FFFFFFF;
    }

    currentLow = low[*(s16*)(actorData + 0x10)];

    for (i = 0; i < 2; i++) {
        s32 j;
        for (j = 0; j < 2; j++) {
            if (low[j + 1] < low[j]) {
                s32 tmp;

                tmp = low[j]; low[j] = low[j + 1]; low[j + 1] = tmp;
                tmp = high[j]; high[j] = high[j + 1]; high[j + 1] = tmp;
                tmp = stateIds[j]; stateIds[j] = stateIds[j + 1]; stateIds[j + 1] = tmp;
            }
        }
    }

    if (layer == D_800AFB54 - 1) {
        if (layer > 0) {
            for (i = 0; i < D_800AFB54 - 1; i++) {
                *(s16*)(actorData + 0x08 + i * 2) = triIds[i];
            }
        }

        if (*(s16*)(actorData + 0x26) < currentLow ||
            (*(u32*)(actorData + 0x00) & 0x1800) != 0) {
            s16 actorY = *(s16*)(actorData + 0x26);
            s32 limit = D_800AFB54 - 1;

            for (i = 0; i < limit; i++) {
                if (low[i] >= actorY) {
                    *(s16*)(actorData + 0x10) = stateIds[i];
                    break;
                }
            }
        } else {
            s32 limit = D_800AFB54 - 1;
            s16 cur = *(s16*)(actorData + 0x10);

            for (i = 0; i < limit; i++) {
                if (stateIds[i] == cur) {
                    break;
                }
            }
        }

        stateFlags = func_80080968(actorData);
        if ((stateFlags & 4) != 0 && i != 0 &&
            (D_800AFB54 - 1) <= *(s16*)(actorData + 0x10)) {
            *(s16*)(actorData + 0x10) = stateIds[i - 1];
        }

        stateFlags = func_80080968(actorData);
        if ((((*(u32*)(actorData + 0x00) >> 8) & 7) & (stateFlags >> 5)) != 0) {
            if (g_FieldSystemMode == 0) {
                func_800379C8(D_8006FC60, actorIndex);
            }
            if (actorIndex == g_PlayerActorIndex) {
                D_800ADB00 = 0x0FFF;
            }
            *(s32*)(actorData + 0x24) += *(s32*)(spriteData + 0x10);
            func_80084A40_RestoreActorState(actorData, spriteData, actorIndex,
                                            origX, origZ, origState, savedStates);
            goto finish;
        }

        if (stateFlags & 0x800000) {
            if (g_FieldSystemMode == 0) {
                func_800379C8(D_8006FC74, actorIndex);
            }
            if (actorIndex == g_PlayerActorIndex) {
                D_800ADB00 = 0x0FFF;
            }
            *(s32*)(actorData + 0x24) += *(s32*)(spriteData + 0x10);
            goto blocked_restore;
        }

        *(s32*)(actorData + 0x20) += *(s32*)(actorData + 0x30);
        *(s32*)(actorData + 0x28) += *(s32*)(actorData + 0x38);

        if (D_800AFB54 - 1 > 0) {
            s16 cur = *(s16*)(actorData + 0x10);
            for (i = 0; i < D_800AFB54 - 1; i++) {
                if (stateIds[i] == cur) {
                    *(s16*)(spriteData + 0x84) = low[i];
                    break;
                }
            }
        }

        VectorNormal(&normals[*(s16*)(actorData + 0x10)], (VECTOR*)(actorData + 0x50));

        if (D_800ADB98 != 0) {
            if ((u32)y < 2) {
                *(s16*)(spriteData + 0x84) = y;
            }
        } else if (y != 0) {
            if (*(s16*)(spriteData + 0x84) < y + 10) {
                *(u8*)(actorData + 0x74) = 0xFF;
            }
            *(s16*)(spriteData + 0x84) = y;
            *(s32*)(actorData + 0x24) = y << 16;
        }

        if (*(u32*)(actorData + 0x00) & 0x00040000) {
            *(s32*)(actorData + 0x24) = *(s16*)(actorData + 0xEC) << 16;
            *(s32*)(spriteData + 0x10) = 0;
        }

        *(s32*)(actorData + 0x24) += *(s32*)(spriteData + 0x10);
        stateFlags = func_80080968(actorData);

        if (*(s16*)(actorData + 0x10) != origState) {
            *(u32*)(actorData + 0x00) &= 0xFBFFFFFF;
        }

        if ((*(u32*)(actorData + 0x00) & 0x04000000) == 0) {
            if (*(s16*)(actorData + 0x26) < *(s16*)(spriteData + 0x84)) {
                if (*(s16*)(spriteData + 0x84) != *(s16*)(actorData + 0x26)) {
                    *(s32*)(spriteData + 0x10) += *(s32*)(spriteData + 0x1C);
                }
                *(u32*)(actorData + 0x00) |= 0x1000;
                *(s32*)(actorData + 0xF0) = *(s32*)(spriteData + 0x10);
            } else {
                goto settle_on_ground;
            }
        } else if ((stateFlags & 0x420000) == 0) {
settle_on_ground:
            if (*(s32*)(spriteData + 0x10) > 0) {
                *(s32*)(spriteData + 0x10) = 0;
            }
            *(u32*)(actorData + 0x00) &= 0xFFBFEFFF;
            *(s32*)(actorData + 0x24) = *(s16*)(spriteData + 0x84) << 16;
        }

        *(u32*)(actorData + 0x00) &= 0xFBFFFFFF;

        if (D_800AFB54 - 1 > 0) {
            s16 actorY = *(s16*)(actorData + 0x26);
            s32 limit = D_800AFB54 - 1;
            for (i = 0; i < limit; i++) {
                if (low[i] >= actorY) {
                    if (actorY - *(u16*)(actorData + 0x1A) < high[i] &&
                        low[i] != high[i]) {
                        break;
                    }
                }
            }
        }

        if (i == D_800AFB54 - 1) {
            s16 cur = *(s16*)(actorData + 0x10);
            s16 tri = *(s16*)(actorData + 0x08 + cur * 2);
            u8* triTable = (u8*)(uintptr_t)(u32)D_800AFB24[cur];
            s32 extra = ((s8)*(triTable + tri * 14 + 0x0D)) << 2;

            if (extra < 0) {
                if (*(s16*)(actorData + 0x26) - *(u16*)(actorData + 0x1A) <
                    *(s16*)(spriteData + 0x84) + extra) {
                    goto blocked_restore;
                }
            }

            *(s32*)(spriteData + 0x00) = *(s32*)(actorData + 0x20);
            *(s32*)(spriteData + 0x04) = *(s32*)(actorData + 0x24);
            *(s32*)(spriteData + 0x08) = *(s32*)(actorData + 0x28);
            *(s32*)(actor + 0x20) = *(s16*)(actorData + 0x22);
            *(s32*)(actor + 0x24) = *(s16*)(actorData + 0x26);
            *(s32*)(actor + 0x28) = *(s16*)(actorData + 0x2A);
            *(s32*)(actorData + 0x14) = func_80080968(actorData);
            goto finish;
        }
    } else {
        *(s32*)(actorData + 0xF0) = 0;
    }

blocked_restore:
    func_80084A40_RestoreActorState(actorData, spriteData, actorIndex,
                                    origX, origZ, origState, savedStates);

finish:
    func_80081C54(actorIndex);
    return 0;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc8", func_800854D0);

extern s32 D_800ADB2C;
extern s32 D_800ADBB8;
extern s32 D_800AFEA4;

void func_80085560(s32 a0, s32 a1, s32 a2) {
    s32 r;
    D_800ADB2C = 1;
    r = ArchiveAllocStreamFile(8);
    D_800ADBB8 = r;
    ArchiveReadFileToBuffer(a0, r, 0, 0x100);
    D_800AFEA4 = a2;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc8", func_800855C8);

extern s32 D_800B21B8;

void func_80085634(int a0, int a1) {
    if (a0 == 0) {
        func_8003A20C((a1 & 7) * 2);
    } else {
        D_800B21B8 = a0;
        func_800855C8(a0, 0x7F, 0x40);
    }
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc8", func_80085678);

extern s16 D_800C3A38;
extern void* D_800B235C;

void func_80085738(void) {
    if (D_800C3A38 != 0xFF) {
        func_80039FF8();
        func_8003852C(D_800B235C);
        HeapFree(D_800B235C);
    }
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc8", func_80085788);

extern void* D_8006259C;
extern s32 D_8004F32C;
extern void* D_8005A4BC;

void func_80085890(void) {
    s32 size;

    ArchiveSetIndex(4, 0);
    size = ArchiveDecodeAlignedSize(0xA8);
    D_8006259C = HeapAlloc(size, 0);
    HeapPinBlock(D_8006259C);

    if (D_8004F32C == -1) {
        ArchiveReadFileToBuffer(0xA8, D_8006259C, 0, CdlModeSpeed);
        ArchiveCdDataSync(0);
    } else {
        memcpy(D_8006259C, D_8005A4BC, size);
        HeapUnpinBlock(D_8005A4BC);
        HeapFree(D_8005A4BC);
    }

    SoundAddSedsEntry(D_8006259C);
    func_8003BDFC(0x10);
    ArchiveSetIndex(4, 0);
    D_8004F32C = -1;
}

void func_80085988(void) {
    func_8003852C(D_8006259C);
    HeapUnpinBlock(D_8006259C);
    HeapFree(D_8006259C);
    D_8004F32C = -1;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc8", func_800859DC);

extern u8 D_800ADFCC[];
extern s32 D_8004F308;
extern s32 D_8004F33C;
extern s32 D_8004F354;
extern s32 D_800AFC54;
extern s32 D_800B2370;
extern void* D_800C3A1C;
extern void func_8001B66C(void);
extern void func_800859DC(void);

void func_80085B20(s32 a0) {
    u8 archiveFile;

    ArchiveCdDataSync(0);
    func_8001B66C();

    if (a0 == 0xFF) {
        D_8004F308 = 0;
        return;
    }

    ArchiveSetIndex(0x1C, 0);

    if (D_800ADFCC[a0 * 2 + 1] == 1) {
        func_80086024();
    }

    archiveFile = D_800ADFCC[a0 * 2];
    if (archiveFile != 0xFF && D_8004F33C != archiveFile) {
        func_80085560(archiveFile * 2 + 0x13, 1, (s32)func_800859DC);
        D_8004F354 = 1;
        D_800B2370 = 0;
        D_800C3A1C = HeapAlloc(0x2000, 1);
    }

    ArchiveSetIndex(4, 0);
    D_8004F308 = -1;
    D_800AFC54 = 1;
}

s32 func_80085C3C(void) {
    s32 count;
    for (count = 0; count < 5; count++) {
        if (func_800854D0() == -1) {
            return 0;
        }
    }
    return -1;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc8", func_80085C90);

extern void* D_8004F2FC;

void func_80085EEC(void) {
    if (D_8004F2FC != NULL) {
        func_80039C4C(D_8004F2FC);
        func_800399D4(D_8004F2FC);
        D_8004F2FC = NULL;
    }
}

extern s32 D_8004F364;
extern s32 D_8004F368;
extern s16 D_8004F384;
extern s32 D_80059560;
extern SoundWDSEntry* D_8006251C;
extern void* D_800B00E0; // WDS File Buffer

int func_80085F30(void) {
    void* pWdsEntry;

    if (ArchiveDataSync()) {
        return -1;
    }
    
    pWdsEntry = SoundLoadWdsFile(D_800B00E0, 0);
    D_8006251C = pWdsEntry;
    D_80059560 = pWdsEntry;
    func_8003BDFC(0x10);
    HeapFree(D_800B00E0);
    D_8004F364 = 1;
    D_8004F384 = 0;
    D_8004F368 = 0;
    return 0;
}

void func_80085FB8(void) {
    void* pWdsFileBuffer;

    ArchiveSetIndex(0x1C, 0x0);
    pWdsFileBuffer = HeapAlloc(ArchiveDecodeAlignedSize(3), 1);
    D_800B00E0 = pWdsFileBuffer;
    ArchiveReadFileToBuffer(3, pWdsFileBuffer, 0, CdlModeSpeed);
    ArchiveSetIndex(4, 0);
    D_8004F364 = 0x80;
}

void func_80086024(void) {
    if (D_8004F368 == 0) {
        D_8004F384 = 1;
        SoundFreeWdsEntry(D_8006251C);
        D_8004F368 = 1;
    }
    D_8004F364 = 0;
}


INCLUDE_ASM("asm/field/nonmatchings/main/misc8", func_80086078);

INCLUDE_ASM("asm/field/nonmatchings/main/misc8", func_800860F0);

INCLUDE_ASM("asm/field/nonmatchings/main/misc8", FieldActorWorldToScreenPosition);

INCLUDE_ASM("asm/field/nonmatchings/main/misc8", func_800862CC);

INCLUDE_ASM("asm/field/nonmatchings/main/misc8", func_800863E8);

extern u16 D_800AFE88[];
extern u16 D_800AFE8A[];

s32 func_80086470(s32 a0, s32 a1) {
    s32 i;
    if (a0 == -1) {
        return -1;
    }
    for (i = 0; i < 3; i++) {
        if (D_800AFE88[i * 3] == a1) {
            return i;
        }
    }
    return -1;
}

void func_800864B4(void) {
    s32 i;
    for (i = 0; i < 3; i++) {
        D_800AFE88[i * 3] = 0xFFFF;
        D_800AFE8A[i * 3] = 0xFFFF;
    }
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc8", func_800864F0);

extern s32 D_800ADBFC;
extern long FieldGetVec3Magnitude(long, long, long);
extern void func_8003A20C(s32);
extern void func_800860F0(s32, s32, s16, s32, s32);
extern void func_800862CC(s32, s32, s16, s32, s32);

void func_80086590(VECTOR* pos) {
    s32 dist[3];
    s32 soundId[3];
    s32 mode[3];
    s32 existing[3];
    s32 updateExisting[3];
    s32 actorIndex[3];
    s16 delta[3][4];
    s32 i;

    for (i = 0; i < 3; i++) {
        dist[i] = 0xFFFF;
        soundId[i] = -1;
        mode[i] = 0;
        existing[i] = 0;
        updateExisting[i] = 0;
        actorIndex[i] = 0;
    }

    for (i = 0; i < D_800ADBFC; i++) {
        ActorData* actor = (ActorData*)(uintptr_t)g_FieldActors[i].pActorData;
        s32 candidate;
        s32 candidateDist;

        if (*(u8*)((u8*)actor + 0x10D) == 0xFF) {
            *(u8*)((u8*)actor + 0x10D) = 0xFF;
            continue;
        }

        candidateDist = FieldGetVec3Magnitude(
            *(s16*)((u8*)pos + 0x02) - *(s16*)((u8*)actor + 0x22),
            *(s16*)((u8*)pos + 0x06) - *(s16*)((u8*)actor + 0x26),
            *(s16*)((u8*)pos + 0x0A) - *(s16*)((u8*)actor + 0x2A));

        if (dist[0] < dist[1]) {
            candidate = (dist[1] < dist[2]) ? 2 : 1;
        } else {
            candidate = (dist[0] < dist[2]) * 2;
        }

        if (candidateDist < dist[candidate]) {
            actorIndex[candidate] = i;
            dist[candidate] = candidateDist;
            soundId[candidate] = *(u16*)((u8*)actor + 0x10A);
            mode[candidate] = *(u8*)((u8*)actor + 0x10C);
            delta[candidate][0] = *(s16*)((u8*)pos + 0x02) - *(s16*)((u8*)actor + 0x22);
            delta[candidate][1] = *(s16*)((u8*)pos + 0x06) - *(s16*)((u8*)actor + 0x26);
            delta[candidate][2] = *(s16*)((u8*)pos + 0x0A) - *(s16*)((u8*)actor + 0x2A);
        }
    }

    for (i = 0; i < 3; i++) {
        s32 slot = func_80086470(soundId[i], actorIndex[i]);
        if (slot != -1) {
            existing[slot] = 1;
            updateExisting[i] = 1;
        }
    }

    for (i = 0; i < 3; i++) {
        if (existing[i] == 0 && D_800AFE8A[i * 3] != 0xFFFF) {
            func_8003A20C(i * 2);
            D_800AFE8A[i * 3] = 0xFFFF;
            D_800AFE88[i * 3] = 0xFFFF;
        }
    }

    for (i = 0; i < 3; i++) {
        if (soundId[i] != -1) {
            if (updateExisting[i] == 1) {
                func_800860F0(soundId[i], mode[i], delta[i][0], dist[i], actorIndex[i]);
            } else {
                func_800862CC(soundId[i], mode[i], delta[i][0], dist[i], actorIndex[i]);
            }
        }
    }
}

extern VECTOR g_CameraEye;
extern VECTOR g_CameraAt;
extern s16 D_800B22E0;
extern s32 g_PlayerActorIndex;

void func_80086908(void) {
    switch (D_800B22E0) {
        case 0:
            func_80086590(&((ActorData*)(uintptr_t)g_FieldActors[g_PlayerActorIndex].pActorData)->position);
            return;
        case 1:
            func_80086590(&g_CameraEye);
            return;
        case 2:
            func_80086590(&g_CameraAt);
            return;
    }
}

void FieldScriptVM2Run(void) {
    char *script = (char *)g_FieldScriptVMCurScriptData;
    size_t vmIP = ++(g_FieldScriptVMCurActor->scriptInstructionPointer);
    /* asm 800869E8: lbu — the extended opcode is zero-extended. A signed
     * char index sent FE 8E-style opcodes to handlers2[-0x72], i.e. into
     * the tail of the first handler table. */
    u8 opcode = (u8)script[vmIP];

    ScriptVMHandler *handler = &g_FieldScriptVMHandlers2[opcode];
    (*handler)();
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc8", func_80086A1C);

INCLUDE_ASM("asm/field/nonmatchings/main/misc8", func_80086BA8);

INCLUDE_ASM("asm/field/nonmatchings/main/misc8", func_80086C34);

void func_80086D4C(void) {
    GameSoftReset();
    D_800B00C0 = 1;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 1;
}
