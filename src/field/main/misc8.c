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

extern int D_800ADBFC;
void func_8008083C(int actorIndex) {
    ActorData* pActor;

    if (actorIndex < D_800ADBFC) {
        pActor = (ActorData*)(uintptr_t)g_FieldActors[actorIndex].pActorData;
        if (pActor->flags134 & 0x80) {
            HeapFree(pActor->unk110);
        }
        if (pActor->flags12C_0xD) {
            HeapFree(pActor->unk114);
        }
        if (g_FieldActors[actorIndex].status & 0x2000) {
            HeapFree(pActor->unk118);
        }
        if (pActor->unk124 != -1) {
            HeapFree(pActor->unk120);
        }
        HeapFree(pActor);
        HeapFree((void*)(uintptr_t)g_FieldActors[actorIndex].pShadow);
        func_800230A8((void*)(uintptr_t)g_FieldActors[actorIndex].pSpriteData);
    }
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc8", func_80080968);

INCLUDE_ASM("asm/field/nonmatchings/main/misc8", func_800809D0);

extern s32 D_800ADB58;
extern s32 D_800ADB5C;

s32 func_80080A18(void) {
    ActorData* p = (ActorData*)(uintptr_t)g_FieldActors[D_800ADB58].pActorData;
    s32 i = D_800ADB5C++;
    return ((s32*)p->unk118)[i];
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
        s16 stateBuf[6];
        s16* pState = stateBuf;
        s16* pDst = (s16*)(p + 0x08);

        for (i = 0; i < D_800AFB54 - 1; i++) {
            s16 r = func_8007B1C4(
                *(s16*)(pActorBytes + 0x20),
                *(s16*)(pActorBytes + 0x28),
                i, (u8*)p + 0x58 + i * 8, pState);
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
    func_80080968(p);
    *(s32*)(p + 0x14) = *(s16*)(p + 0x10);
    {
        s16 choice = *(s16*)(p + 0x10);
        s16* stateBase = (s16*)(p + 0x18);
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

    /* 1. Allocate and zero the 0x138-byte ActorData block */
    D_800B2180++;
    pData = (ActorData*)(uintptr_t)HeapAlloc(0x138, 0);
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

INCLUDE_ASM("asm/field/nonmatchings/main/misc8", func_8008110C);

INCLUDE_ASM("asm/field/nonmatchings/main/misc8", func_800815F0);

INCLUDE_ASM("asm/field/nonmatchings/main/misc8", func_80081C54);

s32 func_80081F5C(u32* a0) {
    u32 a = (a0[0] >> 9) & 3;
    u32 b = a0[5] >> 3;
    return -((a & b) != 0);
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc8", func_80081F80);

INCLUDE_ASM("asm/field/nonmatchings/main/misc8", func_800821F4);

INCLUDE_ASM("asm/field/nonmatchings/main/misc8", func_8008237C);

INCLUDE_ASM("asm/field/nonmatchings/main/misc8", func_80082494);

INCLUDE_ASM("asm/field/nonmatchings/main/misc8", func_800825AC);

INCLUDE_ASM("asm/field/nonmatchings/main/misc8", func_80082620);

INCLUDE_ASM("asm/field/nonmatchings/main/misc8", func_80082BB8);

INCLUDE_ASM("asm/field/nonmatchings/main/misc8", func_80083178);

INCLUDE_ASM("asm/field/nonmatchings/main/misc8", func_800831D0);

INCLUDE_ASM("asm/field/nonmatchings/main/misc8", func_800831F4);

INCLUDE_ASM("asm/field/nonmatchings/main/misc8", func_80083288);

void func_80083994(void) {
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc8", func_8008399C);

INCLUDE_ASM("asm/field/nonmatchings/main/misc8", func_80084158);

INCLUDE_ASM("asm/field/nonmatchings/main/misc8", func_8008492C);

INCLUDE_ASM("asm/field/nonmatchings/main/misc8", func_80084A40);

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

INCLUDE_ASM("asm/field/nonmatchings/main/misc8", func_80085890);

extern void* D_8006259C;
extern s32 D_8004F32C;

void func_80085988(void) {
    func_8003852C(D_8006259C);
    HeapUnpinBlock(D_8006259C);
    HeapFree(D_8006259C);
    D_8004F32C = -1;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc8", func_800859DC);

INCLUDE_ASM("asm/field/nonmatchings/main/misc8", func_80085B20);

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

INCLUDE_ASM("asm/field/nonmatchings/main/misc8", func_80086590);

INCLUDE_ASM("asm/field/nonmatchings/main/misc8", func_80086908);
/*
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
*/

void FieldScriptVM2Run(void) {
    char *script = (char *)g_FieldScriptVMCurScriptData;
    size_t vmIP = ++(g_FieldScriptVMCurActor->scriptInstructionPointer);

    ScriptVMHandler *handler = &g_FieldScriptVMHandlers2[script[vmIP]];
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
