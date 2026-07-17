#include "common.h"
#include "field/actor.h"
#include "field/main.h"
#include "field/text_box.h"
#include "field/effects.h"
#include "field/script_vm.h"
#include "system/memory.h"
#include "system/archive.h"
#ifdef XENO_PC_PORT
#include <stdlib.h>
#endif
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

    /* script slots: 8 entries of 8 bytes each at offset 0x8C */
    for (i = 0; i < 8; i++) {
        u8* e = p + 0x8C + i * 8;
        s32 flags = *(s32*)(e + 4);
        *(s16*)(e + 0) = 0xFFFF;
        *(u8*)(e + 2) = 0;
        *(u8*)(e + 3) = 0xFF;
        flags &= 0xFFFCFFFF;
        flags &= 0xFFBFFFFF;
        flags &= 0xFE7FFFFF;
        flags |= 0x3C0000;
        *(s32*)(e + 4) = flags;
        *(s16*)(e + 4) = 0xFFFF;
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

extern int rsin(int);
extern int rcos(int);
extern void func_80021FE0(void* pSpriteData, s16 angle);
extern void* D_801E8670[];

/* func_80081F80: compute the per-frame walk-step vector into the sprite's step
 * fields (spriteData +0x0C / +0x14, plus +0x18 for the 0x80000 case) from the
 * actor's facing angle. asm 80081F80-800821F0. Branches on fieldActor->status
 * bit 0x40 and actorData->flags. Fei (status 0x40 set; flags & 0x2000 and
 * & 0x80000 both clear) takes the func_80021FE0 path, which stamps the angle
 * into the sprite so func_80022974 derives the step from angle + sprite radius.
 * Divisions are guarded against moveSpeed==0 for host safety (PSX tolerates
 * div-by-zero; the guard is a no-op whenever moveSpeed != 0). */
void func_80081F80(void* pSpriteData, s16 angle, void* pFieldActor) {
    u8* pSprite = (u8*)pSpriteData;
    u8* pFA = (u8*)pFieldActor;
    u16 status = *(u16*)(pFA + 0x58);
    u8* pAD = NULL;
    u16 moveSpeed = 0;
    s32 stepMag = 0;
    s32 doStep = 0;
    s32 set0x18 = 0;
    s32 zeroStep = 0;

    if ((status & 0x40) == 0) {
        pAD = (u8*)(uintptr_t)*(u32*)(pFA + 0x4C);
        moveSpeed = *(u16*)(pAD + 0x76);
        stepMag = (((s32)(moveSpeed ? 0x40000 / moveSpeed : 0)) >> 8) << 5;
        if ((u16)angle & 0x8000) {
            zeroStep = 1;
        } else {
            doStep = 1;
        }
    } else if ((u16)angle & 0x8000) {
        zeroStep = 1;
    } else {
        u32 flags;
        pAD = (u8*)(uintptr_t)*(u32*)(pFA + 0x4C);
        flags = *(u32*)(pAD + 0x04);
        if (flags & 0x2000) {
            if (flags & 0x20000) {
                /* asm .L80082158: step comes from a special-actor work object
                 * indexed by flags12C (D_801E8670 table). */
                u32 f12C = *(u32*)(pAD + 0x12C);
                u8* pEntry = (u8*)&D_801E8670 + ((f12C >> 11) & 0x1C);
                u8* pObj = (u8*)(uintptr_t)*(u32*)pEntry;
                *(s32*)(pSprite + 0xC) = (s32)(-*(s32*)(pObj + 0x128)) << 16;
                *(s32*)(pSprite + 0x14) = (s32)(-*(s32*)(pObj + 0x130)) << 16;
            } else {
                moveSpeed = *(u16*)(pAD + 0x76);
                stepMag = (((s32)(moveSpeed ? 0x80000 / moveSpeed : 0)) >> 8) << 5;
                doStep = 1;
            }
        } else if (flags & 0x80000) {
            moveSpeed = *(u16*)(pAD + 0x76);
            stepMag = (((s32)(moveSpeed ? 0x40000 / moveSpeed : 0)) >> 8) << 5;
            doStep = 1;
            set0x18 = 1;
        } else {
            /* Fei's path: stamp the angle; func_80021FE0 -> func_80022974
             * computes the step from angle + sprite radius (spriteData+0x18). */
            func_80021FE0(pSpriteData, angle);
        }
    }

    if (zeroStep) {
        *(s32*)(pSprite + 0xC) = 0;
        *(s32*)(pSprite + 0x14) = 0;
    } else if (doStep) {
        s32 am = angle & 0xFFF;
        s16 scaleX = *(s16*)(pAD + 0xF4);
        s16 scaleZ = *(s16*)(pAD + 0xF8);
        *(s32*)(pSprite + 0xC) = ((rsin(am) * stepMag) >> 12) * scaleX;
        *(s32*)(pSprite + 0x14) = ((-(rcos(am) * stepMag)) >> 12) * scaleZ;
        if (set0x18) {
            *(s32*)(pSprite + 0x18) = (s32)(moveSpeed ? 0x4000000 / moveSpeed : 0);
        }
    }

    /* asm .L800821BC: clear the low 12 (sub-pixel) bits of the X/Z step. */
    *(s32*)(pSprite + 0xC) &= ~0xFFF;
    *(s32*)(pSprite + 0x14) &= ~0xFFF;
}

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
    s16* clipPoints;
    s32 x;
    s32 z;
    s32 position;
    s32 point0;
    s32 point1;
    s32 point2;
    s32 point3;

    if ((*(u32*)(actorData + 0x12C) & 0x1000) == 0) {
        return 0;
    }

    x = (*(s32*)(actorData + 0x20) + pVec[0]) >> 16;
    z = (*(s32*)(actorData + 0x28) + pVec[2]) >> 16;
    position = (x << 16) + z;

    clipPoints = (s16*)(uintptr_t)*(u32*)(actorData + 0x114);
    point0 = ((s32)clipPoints[0] << 16) + clipPoints[1];
    point1 = ((s32)clipPoints[2] << 16) + clipPoints[3];
    point2 = ((s32)clipPoints[4] << 16) + clipPoints[5];
    point3 = ((s32)clipPoints[6] << 16) + clipPoints[7];

    if (NormalClip(point0, point1, position) < 0 ||
        NormalClip(point1, point2, position) < 0 ||
        NormalClip(point2, point3, position) < 0) {
        return -1;
    }
    return NormalClip(point3, point0, position) >> 31;
}

extern long FieldGetVec2Magnitude(long x, long y);

/* ---- func_800825AC: 2D distance between two field actors ------------------
 * Faithful port of asm/.../misc8/func_800825AC.s (0x74 bytes). Reads each
 * actor's ActorData (FieldActor stride 0x5C, pActorData at +0x4C) integer XZ
 * position (+0x22 / +0x2A) and returns FieldGetVec2Magnitude of the delta.
 * Used by func_80082620's actor-follow branch. */
s32 func_800825AC(s32 actorA, s32 actorB) {
    u8* dataA = (u8*)(uintptr_t)*(u32*)((u8*)g_FieldActors + actorA * 0x5C + 0x4C);
    u8* dataB = (u8*)(uintptr_t)*(u32*)((u8*)g_FieldActors + actorB * 0x5C + 0x4C);
    s32 dx = *(s16*)(dataB + 0x22) - *(s16*)(dataA + 0x22);
    s32 dz = *(s16*)(dataB + 0x2A) - *(s16*)(dataA + 0x2A);
    return FieldGetVec2Magnitude(dx, dz);
}

extern u16 D_800ADFA8[];
extern s16 D_800ADFC4[];
extern void func_8007B614(s32* pOut, s16 scale, s16 angle);

/* ---- func_80082620: environmental / collision-aware auto-move -------------
 * Faithful port of asm/.../misc8/func_80082620.s (0x598 bytes). Beyond the
 * simple auto-move (LUT scale/angle -> func_8007B614), it now implements the
 * tail branches that the corrected walkmesh material (moveFlags = +0x14) drives
 * actors into: the slope-normal projected move (.L800826FC/.L80082820), the
 * actor-follow branch (.L800828E0: HeapAlloc work object + func_800825AC +
 * ratan2/rsin/rcos), and the D_801E8670 work-object divide path (.L80082AF4).
 * gotos mirror the shared asm labels; the previous "unsupported branch" assert
 * is now covered by real retail behavior. */
void func_80082620(s32 actorIndex, void* pFieldActor, void* pActorData) {
    u8* actor = pFieldActor;
    u8* actorData = pActorData;
    u8* spriteData = (u8*)(uintptr_t)*(u32*)(actor + 0x04);
    s32 moveVec[3];
    u32 moveFlags = 0;
    u32 flags0 = *(u32*)(actorData + 0x00);
    u32 flags4 = *(u32*)(actorData + 0x04);
    s32 shift = *(s16*)(actorData + 0x10) + 3;
    s16 scale;
    u16 angle;
    s32 s0move = 0;   /* $s0: X slope-move accumulator */
    s32 s3move = 0;   /* $s3: Z slope-move accumulator */
    s32 f0val;        /* $s4: actorData+0xF0 snapshot */
    s32 addTest;      /* moveVec-add gate (0x4000 via flags0 path, else 0x8000) */

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
        /* asm 800826E4: skip the slope path; gate the moveVec add on 0x4000 */
        addTest = moveFlags & 0x00004000;
        goto apply_movevec;
    }

    /* ---- main path (asm .L800826EC) ---- */
    f0val = *(s32*)(actorData + 0xF0);
    if ((moveFlags & 0x00420000) != 0) {
        /* slope-normal projected move (asm .L800826FC-.L8008281C) */
        VECTOR nIn;
        VECTOR nOut;
        s32 nvx = *(s32*)(actorData + 0x50);
        s32 nvy = *(s32*)(actorData + 0x54);
        s32 nvz = *(s32*)(actorData + 0x58);
        s32 vshift;
        s32 bound;
        s32 nf0;

        nIn.vx = (-(nvx * nvy)) >> 15;
        nIn.vz = (-(nvz * nvy)) >> 15;
        if (nIn.vx == 0) {
            nIn.vx = 1;
        }
        nIn.vy = 1; /* asm zeroes sp+0x24 then unconditionally stores 1 */
        if (nIn.vz == 0) {
            nIn.vz = 1;
        }
        VectorNormal(&nIn, &nOut);
        if (nOut.vx == 0) {
            nOut.vx = 1;
        }
        if (nOut.vy == 0) {
            nOut.vy = 1;
        }
        if (nOut.vz == 0) {
            nOut.vz = 1;
        }

        vshift = f0val >> 17;
        s0move = (nOut.vx * vshift) << 4;
        s3move = (nOut.vz * vshift) << 4;
        bound = ((moveFlags & 0x00400000) != 0) ? 0x18 : 0xC;

        if ((f0val >> 16) < bound) {
            nf0 = f0val + *(s32*)(spriteData + 0x1C);
        } else {
            nf0 = bound << 16;
        }
        *(s32*)(actorData + 0xF0) = nf0;
        *(s32*)(spriteData + 0x10) = *(s32*)(actorData + 0xF0) >> 1;
    }

    /* asm .L80082820 */
    if ((moveFlags & 0x00400000) != 0) {
        *(s32*)(actorData + 0x40) += s0move;
        *(u16*)(actorData + 0x104) |= 0x8000;
        *(s32*)(actorData + 0x48) += s3move;
    }

    /* asm .L80082850 */
    if (*(u8*)(actorData + 0x74) == 0xFF) {
        if ((moveFlags & 0x00020000) != 0) {
            *(s32*)(actorData + 0x40) += s0move;
            *(s32*)(actorData + 0x48) += s3move;
        }
        addTest = moveFlags & 0x00008000;
        goto apply_movevec;
    }
    goto follow_actor;

apply_movevec: /* asm .L8008288C */
    if (addTest != 0) {
        *(s32*)(actorData + 0x40) += moveVec[0];
        *(s32*)(actorData + 0x44) += moveVec[1];
        *(s32*)(actorData + 0x48) += moveVec[2];
    }
    /* asm .L800828D0 */
    if (*(u8*)(actorData + 0x74) == 0xFF) {
        goto work_object;
    }
    /* else fall through to the actor-follow branch */

follow_actor: /* asm .L800828E0 */
    {
        s32 followIdx = *(u8*)(actorData + 0x74);
        u8* followFA = (u8*)g_FieldActors + followIdx * 0x5C;
        u8* followData = (u8*)(uintptr_t)*(u32*)(followFA + 0x4C);
        u8* work;
        s32 myX;
        s32 myZ;
        s32 followX;
        s32 followZ;
        s32 s0y;   /* $s0: change in the followed actor's +0x52 bound */
        s32 dist;  /* $s1 */
        s32 followAngle;

        if ((*(u32*)(followData + 0x04) & 0xC0) != 0xC0) {
            goto work_object;
        }

        if ((*(u32*)(actorData + 0x134) & 0x80) == 0) {
            *(u32*)(actorData + 0x110) = (u32)(uintptr_t)HeapAlloc(0xC, 0);
            *(u32*)(actorData + 0x134) |= 0x80;
        }

        /* asm .L80082944 */
        work = (u8*)(uintptr_t)*(u32*)(actorData + 0x110);
        s0y = (s16)(*(u16*)(followFA + 0x52) - *(u16*)(work + 0x2));
        *(u16*)(work + 0x2) = *(u16*)(followFA + 0x52);

        myX = *(s32*)(actorData + 0x20);
        myZ = *(s32*)(actorData + 0x28);
        followX = *(s32*)(followData + 0x20);
        followZ = *(s32*)(followData + 0x28);

        if ((*(s16*)(actorData + 0x104) & 0x8000) == 0) {
            *(u16*)(work + 0x8) = (u16)func_800825AC(actorIndex, followIdx);
        }
        dist = *(s16*)(work + 0x8);

        followAngle = ratan2(followZ - myZ, followX - myX);
        followAngle = ((s16)followAngle - s0y) - 0x800;

        *(s32*)(actorData + 0x40) +=
            (followX + ((rsin(followAngle) * dist) << 4)) - myX;
        *(s32*)(actorData + 0x48) +=
            (followZ + ((rcos(followAngle) * dist) << 4)) - myZ;
    }

work_object: /* asm .L80082AF4 */
    if ((flags4 & 0x00022000) == 0x00022000) {
        s32 idx = D_800AF858;
        u8* obj = (u8*)(uintptr_t)D_801E8670[idx];
        u16 speed = *(u16*)(actorData + 0x76);

        *(s32*)(actorData + 0x40) -=
            ((*(s32*)(obj + 0x128) << 16) / speed) << 8;
        D_800AF858 = idx + 1;
        *(s32*)(actorData + 0x48) -=
            ((*(s32*)(obj + 0x130) << 16) / speed) << 8;
    }
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

extern void* func_8007CD3C(s32 arg0);
extern void func_8007CD60(s32 arg0);
extern void func_8007B07C(s16* arg0, s16* arg1, s16* arg2, s16* arg3, VECTOR* arg4);
extern MATRIX D_800AFC30;
/* GTE prototypes come through field/actor.h -> psyq/libgte.h (shimmed to
 * PsyCross's on the port build). */

/* Point-vs-actor-collision-mesh test (asm 80083288, "POLYCHECK"). Composes
 * the owning actor's model matrix - rotated about one axis by +0x70 when
 * +0x12C mode is 1/2/3, else the static/parented composition through the
 * camera matrix D_800AFC30 (and the +0x75 parent actor's +0x2C matrix when
 * set) - loads it into the GTE, then walks the mesh's primitive groups
 * (header word: type byte, count in the top half; types 0xC4/0xC8 skipped;
 * type bit 3 selects quads). Each tri/quad is transformed with RotTransSV
 * and tested with NormalClip winding checks against the packed (x<<16)|z
 * query point; for containing polygons func_8007B07C interpolates the
 * surface Y at (x, z) (also filling pOut2, which the caller consumes as the
 * contact data). Returns 0 with *pOutY = the minimum such Y, or -1 when no
 * polygon contains the point. All locals live in a func_8007CD3C(0xB8)
 * scratch block, mirroring the retail layout. */
s32 func_80083288(s32 ownIndex, void* pMesh, s32 curX, s32 curZ, s32* pOutY,
                  void* pOut2) {
    /* Retail places the workspace in the PSX scratchpad via
     * func_8007CD3C(0xB8) (returns 0x1F800000-based pointers, unmapped on
     * the PC port). Follow func_80084158's port convention: keep the arena
     * push/pop balanced but back the workspace with a local. */
    s32 wsBuf[0xB8 / 4];
    u8* ws = (u8*)wsBuf;
    u8* mesh = (u8*)pMesh;
    u8* ownEntry;
    u8* ownData;
    u8* prim;
    u32 rotMode;
    s32 groupCount;

    func_8007CD3C(0xB8);
    *(s32*)(ws + 0xA0) = 0x7FFFFFFF;
    *(u32*)(ws + 0xA4) = *(u32*)(mesh + 0x8);
    *(s32*)(ws + 0x10) = (curX << 16) + curZ;

    ownEntry = (u8*)g_FieldActors + ownIndex * 0x5C;
    ownData = (u8*)(uintptr_t)*(u32*)(ownEntry + 0x4C);
    rotMode = *(u32*)(ownData + 0x12C) & 0x3;

    if (rotMode != 0) {
        SVECTOR* rot = (SVECTOR*)(ws + 0xB0);

        if (rotMode == 1) {
            rot->vx = *(u16*)(ownData + 0x70);
            rot->vy = 0;
            rot->vz = 0;
        } else if (rotMode == 2) {
            rot->vx = 0;
            rot->vy = *(u16*)(ownData + 0x70);
            rot->vz = 0;
        } else {
            rot->vx = 0;
            rot->vy = 0;
            rot->vz = *(u16*)(ownData + 0x70);
        }
        RotMatrix(rot, (MATRIX*)(ws + 0x60));
        MulMatrix2((MATRIX*)(ownEntry + 0xC), (MATRIX*)(ws + 0x60));
        *(s32*)(ws + 0x74) = *(s32*)(ownEntry + 0x20);
        *(s32*)(ws + 0x78) = *(s32*)(ownEntry + 0x24);
        *(s32*)(ws + 0x7C) = *(s32*)(ownEntry + 0x28);
        CompMatrix(&g_Scene.worldRotationMatrix, (MATRIX*)(ws + 0x60),
                   (MATRIX*)(ws + 0x40));
    } else {
        u8 parentIdx;

        /* Retail zeroes both work matrices' translation columns first. */
        *(s32*)(ws + 0x54) = 0;
        *(s32*)(ws + 0x58) = 0;
        *(s32*)(ws + 0x5C) = 0;
        *(s32*)(ws + 0x94) = 0;
        *(s32*)(ws + 0x98) = 0;
        *(s32*)(ws + 0x9C) = 0;

        parentIdx = *(u8*)(ownData + 0x75);
        CompMatrix(&g_Scene.worldRotationMatrix, &D_800AFC30,
                   (MATRIX*)(ws + 0x80));
        if (parentIdx != 0xFF) {
            CompMatrix((MATRIX*)(ws + 0x80),
                       (MATRIX*)((u8*)g_FieldActors + parentIdx * 0x5C + 0x2C),
                       (MATRIX*)(ws + 0x60));
            CompMatrix((MATRIX*)(ws + 0x60), (MATRIX*)(ownEntry + 0xC),
                       (MATRIX*)(ws + 0x40));
        } else {
            CompMatrix((MATRIX*)(ws + 0x80), (MATRIX*)(ownEntry + 0xC),
                       (MATRIX*)(ws + 0x40));
        }
    }
    SetRotMatrix((MATRIX*)(ws + 0x40));
    SetTransMatrix((MATRIX*)(ws + 0x40));

    groupCount = *(u16*)(mesh + 0x6);
    prim = (u8*)(uintptr_t)*(u32*)(mesh + 0x10);

    for (; groupCount > 0; groupCount--) {
        u32 header = *(u32*)prim;
        u32 type = header & 0xFF;
        s32 primCount = header >> 16;
        s32 n;

        prim += 4;
        *(u32*)(ws + 0xAC) = type;
        if (type == 0xC4 || type == 0xC8) {
            continue;
        }

        if ((header & 0x8) == 0) {
            /* Triangles: 3 vertex indices + 1 pad halfword per primitive. */
            for (n = 0; n < primCount; n++) {
                u8* verts = (u8*)(uintptr_t)*(u32*)(ws + 0xA4);

                RotTransSV((SVECTOR*)(verts + *(u16*)(prim + 0x0) * 8),
                           (SVECTOR*)(ws + 0x14), (long*)(ws + 0x3C));
                RotTransSV((SVECTOR*)(verts + *(u16*)(prim + 0x2) * 8),
                           (SVECTOR*)(ws + 0x1C), (long*)(ws + 0x3C));
                prim += 4;
                RotTransSV((SVECTOR*)(verts + *(u16*)(prim + 0x0) * 8),
                           (SVECTOR*)(ws + 0x24), (long*)(ws + 0x3C));
                prim += 4;

                /* Packed (screenX<<16)|screenZ per vertex, ground plane. */
                *(s32*)(ws + 0x0) =
                    (*(s16*)(ws + 0x14) << 16) + *(s16*)(ws + 0x18);
                *(s32*)(ws + 0x4) =
                    (*(s16*)(ws + 0x1C) << 16) + *(s16*)(ws + 0x20);
                *(s32*)(ws + 0x8) =
                    (*(s16*)(ws + 0x24) << 16) + *(s16*)(ws + 0x28);

                if (NormalClip(*(s32*)(ws + 0x0), *(s32*)(ws + 0x4),
                               *(s32*)(ws + 0x10)) < 0 ||
                    NormalClip(*(s32*)(ws + 0x4), *(s32*)(ws + 0x8),
                               *(s32*)(ws + 0x10)) < 0 ||
                    NormalClip(*(s32*)(ws + 0x8), *(s32*)(ws + 0x0),
                               *(s32*)(ws + 0x10)) < 0 ||
                    NormalClip(*(s32*)(ws + 0x0), *(s32*)(ws + 0x4),
                               *(s32*)(ws + 0x8)) < 0) {
                    continue;
                }

                *(u16*)(ws + 0x34) = (u16)curX;
                *(u16*)(ws + 0x38) = (u16)curZ;
                func_8007B07C((s16*)(ws + 0x14), (s16*)(ws + 0x1C),
                              (s16*)(ws + 0x24), (s16*)(ws + 0x34),
                              (VECTOR*)pOut2);
                if (*(s16*)(ws + 0x36) < *(s32*)(ws + 0xA0)) {
                    *(s32*)(ws + 0xA0) = *(s16*)(ws + 0x36);
                }
            }
        } else {
            /* Quads: 4 vertex indices per primitive, wound (0,1,3,2); the
             * final diagonal test picks which half-triangle interpolates. */
            for (n = 0; n < primCount; n++) {
                u8* verts = (u8*)(uintptr_t)*(u32*)(ws + 0xA4);

                RotTransSV((SVECTOR*)(verts + *(u16*)(prim + 0x0) * 8),
                           (SVECTOR*)(ws + 0x14), (long*)(ws + 0x3C));
                RotTransSV((SVECTOR*)(verts + *(u16*)(prim + 0x2) * 8),
                           (SVECTOR*)(ws + 0x1C), (long*)(ws + 0x3C));
                prim += 4;
                RotTransSV((SVECTOR*)(verts + *(u16*)(prim + 0x0) * 8),
                           (SVECTOR*)(ws + 0x24), (long*)(ws + 0x3C));
                RotTransSV((SVECTOR*)(verts + *(u16*)(prim + 0x2) * 8),
                           (SVECTOR*)(ws + 0x2C), (long*)(ws + 0x3C));
                prim += 4;

                *(s32*)(ws + 0x0) =
                    (*(s16*)(ws + 0x14) << 16) + *(s16*)(ws + 0x18);
                *(s32*)(ws + 0x4) =
                    (*(s16*)(ws + 0x1C) << 16) + *(s16*)(ws + 0x20);
                *(s32*)(ws + 0x8) =
                    (*(s16*)(ws + 0x24) << 16) + *(s16*)(ws + 0x28);
                *(s32*)(ws + 0xC) =
                    (*(s16*)(ws + 0x2C) << 16) + *(s16*)(ws + 0x30);

                if (NormalClip(*(s32*)(ws + 0x0), *(s32*)(ws + 0x4),
                               *(s32*)(ws + 0x10)) < 0 ||
                    NormalClip(*(s32*)(ws + 0x4), *(s32*)(ws + 0xC),
                               *(s32*)(ws + 0x10)) < 0 ||
                    NormalClip(*(s32*)(ws + 0xC), *(s32*)(ws + 0x8),
                               *(s32*)(ws + 0x10)) < 0 ||
                    NormalClip(*(s32*)(ws + 0x8), *(s32*)(ws + 0x0),
                               *(s32*)(ws + 0x10)) < 0 ||
                    NormalClip(*(s32*)(ws + 0x0), *(s32*)(ws + 0x4),
                               *(s32*)(ws + 0x8)) < 0) {
                    continue;
                }

                *(u16*)(ws + 0x34) = (u16)curX;
                *(u16*)(ws + 0x38) = (u16)curZ;
                if (NormalClip(*(s32*)(ws + 0x4), *(s32*)(ws + 0x8),
                               *(s32*)(ws + 0x10)) >= 0) {
                    func_8007B07C((s16*)(ws + 0x14), (s16*)(ws + 0x1C),
                                  (s16*)(ws + 0x24), (s16*)(ws + 0x34),
                                  (VECTOR*)pOut2);
                } else {
                    func_8007B07C((s16*)(ws + 0x1C), (s16*)(ws + 0x2C),
                                  (s16*)(ws + 0x24), (s16*)(ws + 0x34),
                                  (VECTOR*)pOut2);
                }
                if (*(s16*)(ws + 0x36) < *(s32*)(ws + 0xA0)) {
                    *(s32*)(ws + 0xA0) = *(s16*)(ws + 0x36);
                }
            }
        }
    }

    if (*(s32*)(ws + 0xA0) != 0x7FFFFFFF) {
        *pOutY = *(s32*)(ws + 0xA0);
        func_8007CD60(0xB8);
        return 0;
    }
    func_8007CD60(0xB8);
    return -1;
}

void func_80083994(void) {
}

extern u16 D_800C2694;
extern s16 D_800B2174;
extern s32 D_80285988;
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
                if ((D_800C2694 & 0x20) && found == 0 &&
                    !(otherFlags4 & 0x4000000)) {
                    /* Confirm-button (Circle released) talk: start the
                     * target's talk script (id 2, routine 3) and turn it
                     * toward the player; one interaction per poll (asm
                     * 80083C68-80083D68). Rejects: no-talk target flags
                     * (0x220000), global lock D_800B2174, and -- for
                     * targets flagged 0x40000 -- a facing cone requiring
                     * the player to face the target within ~+-0x2BB. */
                    s32 angle;
                    s32 dir;
                    s32 delta;

                    if (otherFlags0 & 0x220000) {
                        continue;
                    }
                    if (D_800B2174 != 0) {
                        continue;
                    }

                    angle = ratan2(dz, dx);
                    dir = (-angle >> 9) & 7;
                    delta = (playerRot - ((-angle) & 0xFFF)) & 0xFFF;

                    if ((otherFlags4 & 0x40000) &&
                        (u32)(delta - 0x2BC) < 0xA89u) {
                        continue;
                    }

                    found = 1;
                    scriptId = 2;
                    scriptRoutine = 3;
                    *(u32*)(otherData + 0x12C) =
                        (*(u32*)(otherData + 0x12C) & ~0xE00u) | (dir << 9);
                    if (g_FieldSystemMode == 0) {
                        D_80285988 = 1;
                    }
                } else if ((otherFlags0 & 0x00A20000) == 0) {
                    s32 angle = ratan2(dz, dx);
                    s32 dir = (-angle >> 9) & 7;

                    scriptId = 3;
                    scriptRoutine = 4;
                    *(u32*)(otherData + 0x12C) =
                        (*(u32*)(otherData + 0x12C) & ~0xE00u) | (dir << 9);
                    if (g_FieldSystemMode == 0) {
                        D_80285988 = 1;
                    }
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
extern void func_800379C8(char*, ...);
extern char D_8006FC48[];
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
            /* asm 800842B8-80084380: interaction-region actor. Test whether
             * our predicted (x, z) lies inside the other actor's collision
             * mesh (model header +0x4, via the FieldActor +0x0 pointer);
             * regionY[0] receives the surface Y under us, contact[] the
             * func_8007B07C contact data. Miss -> clear 0x400000|0x800000
             * and skip the actor, exactly like the far case below. */
            s32 regionY[4];
            s32 contact[4];
            u8* otherModel = (u8*)(uintptr_t)*(u32*)(otherActor + 0x0);
            s32 regionTop;

            if (func_80083288(i,
                              (void*)(uintptr_t)*(u32*)(otherModel + 0x4),
                              currentPos.vx, currentPos.vz,
                              regionY, contact) != 0) {
                *(u32*)(otherData + 0x04) &= 0xFF3FFFFF;
                continue;
            }

            /* Hit ("POLYCHECK"): asm 80084304-80084380 + tail joins. */
            if (g_FieldSystemMode == 0) {
                func_800379C8(D_8006FC48, i);
            }
            *(u32*)(otherData + 0x04) |= 0x100;
            regionTop = regionY[0] + *(u16*)(otherData + 0x1A);

            if (*(u8*)(actorData + 0x74) == i) {
                /* Already interacting with this actor: latch the contact
                 * data and mark it (asm 8008434C-80084380)... */
                *(u32*)(actorData + 0x50) = (u32)contact[0];
                *(u32*)(actorData + 0x54) = (u32)contact[1];
                *(u32*)(actorData + 0x58) = (u32)contact[2];
                *(u32*)(otherData + 0x04) |= 0x4000;
                /* ...then asm .L800844B8 routes to the select-target
                 * machinery (.L80084520) unless flags0 has 0x40800 set.
                 * That machinery (velocity latch + interact-actor commit)
                 * is not migrated yet - same gap the deliberate assert
                 * below the loop guards. */
                if ((actorFlags0 & 0x40800) == 0) {
                    assert(!"func_80084158 select-target branch not migrated");
                }
            }

            /* asm .L800844D8 dispatch: regionTop plays the raw-top role,
             * regionY[0] the floor-height role. */
            if (regionTop < targetYMin ||
                *(s16*)(actorData + 0x26) < regionY[0]) {
                /* .L80084660/.L8008465C track-height + .L800846A8 tail. */
                u32 v = *(u32*)(otherData + 0x04) & ~0x100u;

                if (*(s16*)(actorData + 0x26) < regionY[0]) {
                    v |= 0x00800000;
                    if (regionY[0] < selectedY) {
                        selectedY = regionY[0];
                    }
                } else {
                    v &= ~0x00800000u;
                }
                *(u32*)(otherData + 0x04) = v | 0x00400000;
                continue;
            }
            if (*(s16*)(actorData + 0x26) < regionY[0] + 0x10) {
                /* .L80084520 select-target machinery, not migrated. */
                assert(!"func_80084158 select-target branch not migrated");
            }
            if (*(u32*)(otherData + 0x04) & 0x00800000) {
                /* Falls into .L80084520 as well. */
                assert(!"func_80084158 select-target branch not migrated");
            }
            /* .L80084570 standing-on-top machinery, not migrated. */
            assert(!"func_80084158 on-top branch not migrated");
            continue;
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
/* (func_800379C8 is the field's variadic printf; declared once at the top
 * of this file's interaction section.) */
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

extern s32 D_800ADBBC;
extern s32 D_800ADBB8;
extern s32 D_800ADB2C;
extern s32 D_800AFEA4;
extern s32 func_80028B14(void);

// Per-frame CD-stream pump for the in-flight archive read kicked off by
// func_80085560: func_80028B14 (still-unimplemented low-level PSX CD/DMA
// polling primitive) reports which buffered chunk index is ready, or 0 when
// no chunk transfer is in flight. While a chunk is in flight, forward it to
// the read's registered per-chunk callback (D_800AFEA4, e.g. func_800859DC)
// and report busy. Once idle and ArchiveDataSync confirms the CD/archive
// subsystem is fully caught up, free the streaming buffer and clear
// D_800ADB2C -- the flag func_800932D0 (CHANGE_FIELD) gates on.
s32 func_800854D0(void) {
    s32 chunkIndex;

    chunkIndex = func_80028B14();
    D_800ADBBC = chunkIndex;
    if (chunkIndex != 0) {
        ((void (*)(s32))(uintptr_t)D_800AFEA4)(chunkIndex);
        return 0;
    }

    if (ArchiveDataSync() != 0) {
        return 0;
    }
    if (D_800ADBBC != 0) {
        return 0;
    }

    HeapFree((void*)(uintptr_t)D_800ADBB8);
    D_800ADB2C = 0;
    return -1;
}

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

/* XENO_PC_PORT temporary audio boundary:
 * Field transition scripts reach func_800855C8 for a sound cue. Retail then
 * enters the deeper sound voice/SED/WDS assignment path
 * (func_8003A20C/func_80039F9C/func_8003B644). That subsystem is not ported
 * yet, and this sound call does not own field transition state, so the native
 * port intentionally treats it as a no-op while field progression is traced.
 * Remove this shim when the real audio engine path is implemented.
 *
 * The variadic form preserves current native call sites while func_80085634 is
 * still a nonmatching register-style transcription that omits the fourth arg in
 * C but preserves it in the original MIPS register flow. */
void func_800855C8(s32 soundId, s32 volume, s32 pan, ...)
{
    (void)soundId;
    (void)volume;
    (void)pan;
}

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
#ifdef XENO_PC_PORT
/* Bank file index captured at stream start (func_80085B20) for the C90
 * staging substitute; see the comments at both sites. */
static s32 PcPort_PendingBankFile;
#endif
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
#ifdef XENO_PC_PORT
        /* Record the file index this stream was STARTED with: the port's
         * C90 bank-staging substitute must read the same file at completion
         * even if the requested music changes mid-stream (retail's chunk
         * pump doesn't have this problem -- the stream carries its data). */
        PcPort_PendingBankFile = archiveFile * 2 + 0x13;
#endif
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

extern s32 D_8004F338;
extern s32 D_8004F36C;
extern s32 D_8004F354;
extern s32 D_8004F364;
extern int func_80085F30(void);
extern void func_80085FB8(void);
extern void* SoundLoadWdsFile(void* pWdsFile, s32 mode);

extern s32 D_8004F33C;
extern s32 D_8004F340;
extern s32 D_8004F348;
extern s32 D_8004F358;
extern s32 D_8004F35C;
extern s32 D_800AFC54;
extern void* D_8004F2FC;
extern void* D_80062528;
extern u8 D_80062648[];
extern s32 g_GameHasLoadedWDS;
extern u8 D_800ADFCC[];
extern void* D_800C3A1C;
extern void func_8003BDFC(s32);

/* Song-start M3: the real per-frame music poller (retail func_80085C90
 * structure, transcribed from the matchings asm), with ONE documented port
 * substitution in the bank-swap leg. Retail streams the music WDS bank off
 * CD in 8-sector windows and pumps each chunk to SPU-RAM via the
 * func_800859DC callback (func_800380D0 header init + SoundTransferWdsPart);
 * the port has no section-queue/chunk machinery (func_80028B14 is a stub and
 * ArchiveReadFile declines CdlModeStream), so the stream completes as an
 * empty no-op and this poller lands the SAME end state at the completion
 * boundary with the proven buffered path: whole-file read +
 * SoundLoadWdsFile. File selection, flags and completion bookkeeping are
 * retail's own. Remove the substitution when CD streaming is ported. */
s32 func_80085C90(s32 a0) {
    extern void* func_80039850(void* pSongFile);
    extern void func_80039A80(void* manager, s32 level, s32 steps);
    extern void func_80039B68(void* manager, s32 level, s32 steps);
    extern void func_8003A89C(void* manager, s32 level, s32 steps);

    if (D_8004F354 == 1) {
        if (func_80085C3C() == -1) {
            return -1;
        }
        func_8003BDFC(0x10);
#ifdef XENO_PC_PORT
        /* PORT SUBSTITUTION (see header comment): retail's chunk pump has
         * already landed the bank in SPU-RAM by this point. The staging
         * buffer is HOST memory: retail never holds this file in main RAM
         * (it streams), and the field heap cannot fit ~190KB mid-load --
         * SoundLoadWdsFileHostStaged pushes it to the backend directly. */
        if (PcPort_PendingBankFile > 0) {
            extern void* SoundLoadWdsFileHostStaged(void* pWdsFile);
            s32 bankFile = PcPort_PendingBankFile;
            s32 bankSize;
            void* pBankBuf;
            PcPort_PendingBankFile = 0;
            ArchiveSetIndex(0x1C, 0);
            bankSize = ArchiveDecodeAlignedSize(bankFile);
            /* Sector-round the staging allocation: the CD layer works in
             * whole 2048-byte sectors and a tail write past the byte size
             * faults under allocators without slack (TSan mmap). */
            pBankBuf = (bankSize > 0)
                ? malloc(((bankSize + 2047) / 2048 + 1) * 2048) : NULL;
            if (pBankBuf != NULL) {
                ArchiveReadFileToBuffer(bankFile, pBankBuf, 0, CdlModeSpeed);
                SoundLoadWdsFileHostStaged(pBankBuf);
                free(pBankBuf);
            }
            ArchiveSetIndex(4, 0);
        }
#endif
        HeapFree(D_800C3A1C);
        g_GameHasLoadedWDS = 1;
        D_8004F354 = 0;
        D_8004F33C = D_800ADFCC[a0 * 2];
    }

    /* Common-bank leg: maps flagged 0 at D_800ADFCC[idx*2+1] use the shared
     * field bank (dir 0x1C file 3) -- kick its read, then complete it. */
    if (D_800ADFCC[a0 * 2 + 1] == 0) {
        s32 state = D_8004F364;
        if (state == 0) {
            func_80085FB8();
            return -1;
        }
        if (state & 0x80) {
            if (func_80085F30() == -1) {
                return -1;
            }
        }
    }

    /* Song-file leg: on the first frame after a music change, queue the
     * 'smds' song read into the static song buffer (D_80062648). */
    if (D_800AFC54 == 1) {
        if (D_8004F338 != a0) {
            ArchiveSetIndex(0x1C, 0);
            ArchiveReadFileToBuffer(a0 * 2 + 0x14, D_80062648, 0, 0x80);
            D_8004F358 = 1;
            ArchiveSetIndex(4, 0);
        }
        D_800AFC54 = 0;
        return -1;
    }
    if (ArchiveDataSync() != 0) {
        return -1;
    }

    /* Song landed: create/start the music manager (the M1 arming chain). */
    if (D_8004F358 == 1) {
        if (D_8004F348 == 0) {
            void* manager = func_80039850(D_80062648);
            D_80062528 = manager;
            if (D_8004F340 == -1) {
                func_80039A80(manager, 0x7F, 0);
            } else {
                /* Muted start: a pending scripted fade (FE 0E) raises it. */
                func_80039A80(manager, 0, 0);
                func_8003A89C(D_80062528, 0, 0);
            }
        } else {
            /* Same song, bank re-landed: resume the saved manager. */
            D_80062528 = D_8004F2FC;
            func_80039B68(D_8004F2FC, 0x7F, 0xF0);
            D_8004F348 = 0;
            D_8004F2FC = 0;
        }
        D_8004F358 = 0;
        D_8004F35C = 1;
        D_8004F338 = a0;
    }

    D_8004F340 = -1;
    D_8004F36C = 1;
    return 0;
}

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


extern s16 D_800B21AC;
extern u16 D_800AFE88[];
extern u16 D_800AFE8A[];
extern void func_8003A20C(s32);
extern void func_8003A344(s32, s32);
extern void func_8003A55C(s32, s32);
extern void func_80039F9C(s32, s32, s32, s32);

/* Attenuation from distance vs D_800B21AC, scaled by mode. Writes volume to *out. */
void func_80086078(s32 distance, s32* outVolume, s32 mode) {
    s32 maxDist = D_800B21AC;
    s32 scaled;
    u32 inv;
    u32 t;

    if (maxDist < distance) {
        distance = maxDist;
    }

    scaled = (0x7F0000 / maxDist) * distance;
    inv = 0x80 - (scaled >> 16);
    inv <<= 16;
    /* Unsigned reciprocal multiply by 0x02040811 ≈ 1/127, then >> 6. */
    t = (u32)(((u64)inv * 0x02040811u) >> 32);
    inv = inv - t;
    inv = (inv >> 1) + t;
    inv >>= 6;
    *outVolume = (s32)(((u64)inv * (u32)mode) >> 16);
}

/* Project actor origin through worldToScreen; write screen X/Y.
 * Retail addresses worldToScreen as &g_FieldActors - 0xAC (= g_Scene + 0xD4). */
void FieldActorWorldToScreenPosition(s32 actorIndex, s32* outX, s32* outY) {
    FieldActor* actors = g_FieldActors;
    MATRIX composed;
    SVECTOR local;
    long screenXY;
    long p;
    long flag;

    (void)FieldScriptVMGetActorIndex(1);

    CompMatrix(&g_Scene.worldToScreenMatrix, &actors[actorIndex].transformMatrix, &composed);
    local.vx = 0;
    local.vy = 0;
    local.vz = 0;
    SetRotMatrix(&composed);
    SetTransMatrix(&composed);
    RotTransPers(&local, &screenXY, &p, &flag);
    *outY = (s16)(screenXY >> 16);
    *outX = (s16)screenXY;
}

/* Asm multiply chain: (((x*3)*17)*257*2)>>16 == (x * 0x6666)>>16. */
static s32 FieldPositionalSfxScreenPan(s32 screenX) {
    s32 t = screenX;
    t = (t << 1) + t;
    t = t + (t << 4);
    t = t + (t << 8);
    t <<= 1;
    return t >> 16;
}

/* Update an already-bound positional SFX slot's volume/pan. */
void func_800860F0(s32 soundId, s32 mode, s16 deltaX, s32 distance, s32 actorIndex) {
    s32 i;
    s32 volume;
    s32 screenX;
    s32 screenY;
    s32 pan;
    s32 slot2;

    (void)soundId;
    (void)deltaX;

    for (i = 0; i < 3; i++) {
        if (D_800AFE88[i * 3] == (u16)actorIndex) {
            slot2 = i * 2;
            func_80086078(distance, &volume, mode);
            FieldActorWorldToScreenPosition(actorIndex, &screenX, &screenY);
            if (screenX >= 0x141) {
                screenX = 0x13F;
            }
            if (screenX < 0) {
                screenX = 0;
            }
            pan = FieldPositionalSfxScreenPan(screenX);
            func_8003A344(slot2, volume);
            func_8003A55C(slot2, pan);
            break;
        }
    }
}

/* Allocate a free positional SFX slot and start the voice with distance pan. */
void func_800862CC(s32 soundId, s32 mode, s16 deltaX, s32 distance, s32 actorIndex) {
    s32 i;
    s32 volume;
    s32 screenX;
    s32 screenY;
    s32 pan;
    s32 slot2;

    (void)deltaX;

    for (i = 0; i < 3; i++) {
        if (D_800AFE8A[i * 3] == 0xFFFF) {
            slot2 = i * 2;
            D_800AFE8A[i * 3] = (u16)soundId;
            D_800AFE88[i * 3] = (u16)actorIndex;
            func_80086078(distance, &volume, mode);
            FieldActorWorldToScreenPosition(actorIndex, &screenX, &screenY);
            if (screenX >= 0x141) {
                screenX = 0x13F;
            }
            if (screenX < 0) {
                screenX = 0;
            }
            pan = FieldPositionalSfxScreenPan(screenX);
            func_8003A20C(slot2);
            func_80039F9C(soundId, slot2, volume, pan);
            break;
        }
    }
}

/* Release the SPU voice channel bound to actor `actorIdx` in the 3-slot
 * actor->channel table (same table as func_80086470/func_800864F0):
 * on match, stop the voice (func_8003A20C(i*2)) and free the slot.
 * asm nonmatchings/main/misc8/func_800863E8.s. */
void func_800863E8(s32 actorIdx) {
    s32 i;

    for (i = 0; i < 3; i++) {
        if (D_800AFE88[i * 3] == (u16)actorIdx) {
            func_8003A20C(i * 2);
            D_800AFE8A[i * 3] = 0xFFFF;
            D_800AFE88[i * 3] = 0xFFFF;
            break;
        }
    }
}

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

extern s16 D_800B233C;
extern void func_8003A20C(s32);

// Field-transition sound-channel teardown: reset the 3-slot channel table
// (same shape as func_800864B4) and release any of the 4 low D_800B233C
// channel-mask bits that are still clear, one SPU voice-release call per bit.
void func_800864F0(void) {
    s32 i;
    s16 flags;

    for (i = 0; i < 3; i++) {
        D_800AFE8A[i * 3] = 0xFFFF;
        D_800AFE88[i * 3] = 0xFFFF;
    }

    flags = D_800B233C;
    for (i = 0; i < 4; i++) {
        if ((flags & 1) == 0) {
            func_8003A20C(i * 2);
        }
        flags = (u16)flags >> 1;
    }
    D_800B233C = flags;
}

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
