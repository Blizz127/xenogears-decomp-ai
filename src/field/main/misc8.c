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
        pActor = g_FieldActors[actorIndex].pActorData;
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
        HeapFree(g_FieldActors[actorIndex].pShadow);
        func_800230A8(g_FieldActors[actorIndex].pSpriteData);
    }
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc8", func_80080968);

INCLUDE_ASM("asm/field/nonmatchings/main/misc8", func_800809D0);

extern s32 D_800ADB58;
extern s32 D_800ADB5C;

s32 func_80080A18(void) {
    ActorData* p = g_FieldActors[D_800ADB58].pActorData;
    s32 i = D_800ADB5C++;
    return ((s32*)p->unk118)[i];
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc8", func_80080A74);

INCLUDE_ASM("asm/field/nonmatchings/main/misc8", func_80080F44);

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

INCLUDE_ASM("asm/field/nonmatchings/main/misc8", func_80085560);

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
            func_80086590(&g_FieldActors[g_PlayerActorIndex].pActorData->position);
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
