#include "common.h"
#include "psyq/libgpu.h"
#include "psyq/libetc.h"
#include "system/memory.h"
#include "system/controller.h"

extern int g_FrameDeltaTime;


INCLUDE_ASM("asm/field/nonmatchings/main/main", FieldInitializeControllers);
/*
void FieldInitializeControllers(void) {
    FieldSetControllerBuffers(&g_C1Buffer, &g_C2Buffer);
    FieldSetMouseSpeed(3, 4);
    func_8007ADA4(0, 0x140, 0, 0xE0); // SetMouseArea?
    FieldSetMousePosition(0, 0x50, 100);
    FieldSetMousePosition(1, 0xFA, 100);
    func_8007ADA4(0, 300, 10, 0xDC); // SetMouseArea?
}
*/

void FieldRenderSync(void) {
    DrawSync(0);
    Vsync(0);
}

INCLUDE_ASM("asm/field/nonmatchings/main/main", FieldLoadUITextures);

extern s32 g_GameSceneMapNum;

void func_800777DC(void) {
    ArchiveCdDataSync(0);
    while (func_8001B484((g_GameSceneMapNum & 0xFFF) << 1, 0) != 0) {
    }
}

void FieldUpdateDeltaTime(void) {
    g_FrameDeltaTime = Vsync(1);
}

void func_80077844(short* dst, short a, short b, short c, short d, short e, short f, short g, short h, short i) {
    dst[0] = a;
    dst[1] = b;
    dst[2] = c;
    dst[3] = d;
    dst[4] = e;
    dst[5] = f;
    dst[6] = g;
    dst[7] = h;
    dst[8] = i;
}

INCLUDE_ASM("asm/field/nonmatchings/main/main", func_80077884);

INCLUDE_ASM("asm/field/nonmatchings/main/main", func_80077AB4);

void func_80077C60(void) {
    func_80077884();
    func_80077AB4();
}

extern void* g_PartyDataBuffers[];

void FieldPartyAllocateSkinDataBuffers(void) {
    HeapChangeCurrentUser(HEAP_USER_YOSI, NULL);
    g_PartyDataBuffers[0] = HeapAlloc(0x14000, 0);
    g_PartyDataBuffers[1] = HeapAlloc(0x14000, 0);
    g_PartyDataBuffers[2] = HeapAlloc(0x14000, 0);
    HeapPinBlock(g_PartyDataBuffers[0]);
    HeapPinBlock(g_PartyDataBuffers[1]);
    HeapPinBlock(g_PartyDataBuffers[2]);
}

void FieldPartyFreeSkinDataBuffers(void) {
    HeapUnpinBlock(g_PartyDataBuffers[0]);
    HeapUnpinBlock(g_PartyDataBuffers[1]);
    HeapUnpinBlock(g_PartyDataBuffers[2]);
    HeapFree(g_PartyDataBuffers[0]);
    HeapFree(g_PartyDataBuffers[1]);
    HeapFree(g_PartyDataBuffers[2]);
}

INCLUDE_ASM("asm/field/nonmatchings/main/main", func_80077DAC);

INCLUDE_ASM("asm/field/nonmatchings/main/main", func_80077E10);

INCLUDE_ASM("asm/field/nonmatchings/main/main", FieldMain);
