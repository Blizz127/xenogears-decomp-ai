#include "common.h"
#include "main/game.h"
#include "system/math.h"
#include "field/main.h"
#include "field/camera.h"
#include "field/actor.h"
#include "field/script_vm.h"
#include "field/text_box.h"
#include "field/particles.h"
#include "field/effects.h"

void FieldScriptMemoryWriteU16(int, int);

extern int rcos(int);
extern int rsin(int);

extern s32 D_800AFD1C;
extern void func_80072254(s32);
extern void SpriteSetSpecialAnimFile(SpriteData*, void*);

void FieldSetScreenDimensions(void) {
    g_FieldRenderContexts[0].dispEnv.screen.x = 0;
    g_FieldRenderContexts[0].dispEnv.screen.y = 10;
    g_FieldRenderContexts[0].dispEnv.screen.w = 0x100;
    g_FieldRenderContexts[0].dispEnv.screen.h = 0xd8;
    g_FieldRenderContexts[1].dispEnv.screen.x = 0;
    g_FieldRenderContexts[1].dispEnv.screen.y = 10;
    g_FieldRenderContexts[1].dispEnv.screen.w = 0x100;
    g_FieldRenderContexts[1].dispEnv.screen.h = 0xd8;
}

extern u8 D_800B2358[]; // Is pause disabled?

void func_80086DE0(void) {
    D_800B2358[0] = SCRIPT_READ_U8_REL(1);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
}

extern s16 D_800ADB54;

void func_80086E1C(void) {
    RECT rect;
    switch (FieldScriptVMGetArgument(1)) {
        case 0:
            rect.w = 0x500;
            rect.x = 0;
            rect.y = 0;
            rect.h = 0x200;
            ClearImage(&rect, 0x0, 0x0, 0x0);
            DrawSync(0);
            Vsync(0);
            SetDefDrawEnv(g_FieldRenderContexts[0].drawEnvs, 0, 0, 0x280, 0xE0);
            SetDefDrawEnv(g_FieldRenderContexts[1].drawEnvs, 0, 0x100, 0x280, 0xE0);
            SetDefDispEnv(&g_FieldRenderContexts[0].dispEnv, 0, 0x100, 0x280, 0xE0);
            SetDefDispEnv(&g_FieldRenderContexts[1].dispEnv, 0, 0, 0x280, 0xE0);
            FieldSetScreenDimensions();
            break;
        case 1:
            D_800ADB54 = 0x1;
            break;
        case 2:
            D_800ADB54 = 0x0;
            break;
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

// Set model animation for actor
void func_80086F7C(void) {
    g_FieldScriptVMCurActor->modelAnimation = (FieldScriptVMGetArgument(1) << 0xC) | FieldScriptVMGetArgument(3);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
}

void func_80086FD0(void) {
    switch (SCRIPT_READ_U8_REL(1)) {
        case 0:
            // Initialize Sprite List
            func_800AAC08();
            g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
            return;
        case 2:
            // Free Sprite List
            func_800AABD8();
            g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
            return;
        case 1:
            // Set X, Y of Sprite and add it to OT for drawing
            func_800AAE4C(
                FieldScriptVMGetArgument(2), // Sprite Index
                FieldScriptVMGetArgument(4), // X
                FieldScriptVMGetArgument(6), // Y
                FieldScriptVMGetArgument(8)  // Sprite type
            );
            g_FieldScriptVMCurActor->scriptInstructionPointer += 0xA;
            return;
        case 3:
            // Set Sprite Color
            func_800AADC8(
                FieldScriptVMGetArgument(2), // Sprite Index
                FieldScriptVMGetArgument(4), // Red
                FieldScriptVMGetArgument(6), // Green
                FieldScriptVMGetArgument(8)  // Blue
            );
            g_FieldScriptVMCurActor->scriptInstructionPointer += 0xA;
            return;
    }
}

void func_80087148(void) {
    s32 a = FieldScriptVMGetArgument(1);
    s32 b = FieldScriptVMGetArgument(3);
    *(u16*)((u8*)g_pGameState + a * 0x20 + 0x16DA) |= b;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_800871B0);

extern u8 D_800B225F[];

void func_800873C4(void) {
    D_800B225F[FieldScriptVMGetArgument(1)] = FieldScriptVMGetArgument(3);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
}

void func_80087420(void) {
    int arg1, arg2, arg3, arg4, arg5, arg6;
    int value1, value2;

    arg1 = FieldScriptVMGetArgument(ARG(1));
    arg2 = FieldScriptVMGetArgument(ARG(2));
    arg3 = FieldScriptVMGetArgument(ARG(3));
    arg4 = FieldScriptVMGetArgument(ARG(4));
    arg5 = FieldScriptVMGetArgument(ARG(5));
    arg6 = FieldScriptVMGetArgument(ARG(6));

    value1 = CONV_TO_GTE(
        CONV_FROM_GTE(arg5) / arg3 * arg1
    );
    value2 = CONV_TO_GTE(
        CONV_FROM_GTE(arg6) / arg4 * arg2
    );
    
    FieldScriptMemoryWriteU16(
        SCRIPT_IMM_ARG(7), 
        value1
    );
    FieldScriptMemoryWriteU16(
        SCRIPT_IMM_ARG(8), 
        value2
    );
    
    g_FieldScriptVMCurActor->scriptInstructionPointer += 0x11;
}

void func_8008752C(void) {
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void func_8008754C(void) {
    g_pGameState->unk22B6 |= 0x4000;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 1;
}

void FieldScriptCopyGear(void) {
    int sourceGearIndex = FieldScriptVMGetArgument(1);
    g_pGameState->gears[FieldScriptVMGetArgument(3)] = g_pGameState->gears[sourceGearIndex];
    g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008764C);

extern u8 D_80050622;

void func_80087800(void) {
    FieldScriptMemoryWriteU16(
        SCRIPT_IMM_ARG(1), 
        D_80050622
    );
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_80087848);

void func_80087960(void) {
    FieldScriptMemoryWriteU16(SCRIPT_IMM_ARG(1), *(u16*)((u8*)g_pGameState + 0x1844));
    FieldScriptMemoryWriteU16(SCRIPT_IMM_ARG(2), *(u16*)((u8*)g_pGameState + 0x1846));
    g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
}

void func_800879D0(void) {
    FieldScriptMemoryWriteU16(SCRIPT_IMM_ARG(1), g_pGameState->unk184E);
    FieldScriptMemoryWriteU16(SCRIPT_IMM_ARG(2), g_pGameState->unk1852);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
}

extern u8 D_800B2357;

void func_80087A40(void) {
    D_800B2357 = SCRIPT_READ_U8_REL(1);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
}

extern u8 D_800B2354;

void func_80087A7C(void) {
    D_800B2354 = SCRIPT_READ_U8_REL(1);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_80087AB8);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_80087B5C);

extern s32 D_8004F300;

void func_80087C0C(void) {
    D_8004F300 = 1;
    g_FieldScriptVMCurActor->scriptInstructionPointer++;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_80087C34);

void func_80087D30(void) {
    FieldScriptMemoryWriteU16(SCRIPT_IMM_ARG(1), *(u16*)((u8*)g_pGameState + 0x1834));
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void func_80087D80(void) {
    *(u16*)((u8*)g_pGameState + 0x1834) = FieldScriptArgument1(1, SCRIPT_READ_U8_REL(3));
    g_FieldScriptVMCurActor->scriptInstructionPointer += 4;
}

extern u8 D_800B2355;
extern u8 D_800B2356;

void func_80087DE0(void) {
    int arg = FieldScriptVMGetArgument(2);
    if (SCRIPT_READ_U8_REL(1) == 0) {
        D_800B2355 = arg;
    } else {
        D_800B2356 = arg;
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 4;
}

extern s16 D_800B234C;

void func_80087E5C(void) {
    D_800B234C = FieldScriptVMGetArgument(1);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_80087E98);

extern u16 D_800B2348;

void func_80087FA4(void) {
    D_800B2348++;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 1;
}

void func_80087FD4(void) {
    func_800A8BA4();
    g_FieldScriptVMCurActor->scriptInstructionPointer += 1;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008800C);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_80088198);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_800881E8);

extern s32 D_8004F308;

void func_8008825C(void) {
    if (D_8004F308 == -1) {
        g_FieldScriptVMCurActor->scriptInstructionPointer -= 1;
    } else {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 1;
    }
    D_800B00C0 = 1;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_800882B8);

void FieldScriptSetCharacterGear(void) {
    int characterId = FieldScriptVMGetArgument(1);
    g_pGameState->characters[characterId].gearId = FieldScriptVMGetArgument(3);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_800883D4);

extern s16 D_800B236C;

void func_8008848C(void) {
    D_800B236C = SCRIPT_READ_U8_REL(1) ^ 1;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
}

extern s16 D_800B21AC;

void func_800884CC(void) {
    D_800B21AC = FieldScriptVMGetArgument(1);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_80088508);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008861C);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_80088674);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_80088790);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_800888A4);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_800889BC);

// Start of particle handlers
extern s32 D_800ADB40;
extern s32 D_800ADB8C;
extern s32 D_800B2374;
extern s32 D_800B2378;
extern s32 D_800B237C;
extern s32 D_800B2380;

typedef struct {
    /* 0x0 */ int bankIndex;
    /* 0x4 */ u8 unk4[0xC];
} ParticleBankHandle;

extern ParticleBankHandle D_800B2384;
extern s32 g_FieldScriptMaxInstructionCount;

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_80088B68);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_80088C1C);

void func_80088CF8() {
    FieldScriptSetParticleBankDirections(0);
}

void func_80088D18() {
    FieldScriptSetParticleBankDirections(4);
}

void FieldScriptSetParticleBankDirections(int index) {
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].directions[index].x = FieldScriptArgument1(1, SCRIPT_READ_U8_REL(0x11));
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].directions[index].z = FieldScriptArgument2(3, SCRIPT_READ_U8_REL(0x11));
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].directions[index + 1].x = FieldScriptArgument3(5, SCRIPT_READ_U8_REL(0x11));
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].directions[index + 1].z = FieldScriptArgument4(7, SCRIPT_READ_U8_REL(0x11));
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].directions[index + 2].x = FieldScriptArgument5(9, SCRIPT_READ_U8_REL(0x11));
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].directions[index + 2].z = FieldScriptArgument6(0xB, SCRIPT_READ_U8_REL(0x11));
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].directions[index + 3].x = FieldScriptArgument7(0xD, SCRIPT_READ_U8_REL(0x11));
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].directions[index + 3].z = FieldScriptArgument8(0xF, SCRIPT_READ_U8_REL(0x11));
    g_FieldScriptMaxInstructionCount += 4;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 0x12;
}

void FieldScriptInitializeParticleBank(void) {
    D_800B2384.bankIndex = FieldScriptVMGetArgument(1);
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].speedMultiplier = 1;
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].targetActorID = D_800B2374;
    D_800ADB40 = g_FieldDefaultParticleBanks[D_800B2384.bankIndex].targetActorID;
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].unk0 = 0;
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].rotAngle = 0;
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].max = FieldScriptVMGetArgument(3);
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].swait = FieldScriptVMGetArgument(5);
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].ewait = FieldScriptVMGetArgument(7);
    func_8008861C();
    g_FieldScriptMaxInstructionCount += 4;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 9;
}

void FieldScriptSetParticleBankPosition(void) {
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].pos.vx = FieldScriptArgument1(1, SCRIPT_READ_U8_REL(0xD));
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].pos.vy  = FieldScriptArgument2(3, SCRIPT_READ_U8_REL(0xD));
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].pos.vz = FieldScriptArgument3(5, SCRIPT_READ_U8_REL(0xD));
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].epos.vx = FieldScriptArgument4(7, SCRIPT_READ_U8_REL(0xD));
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].epos.vy = FieldScriptArgument5(9, SCRIPT_READ_U8_REL(0xD));
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].epos.vz = FieldScriptArgument6(0xB, SCRIPT_READ_U8_REL(0xD));
    g_FieldScriptMaxInstructionCount += 4;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 0xE;
}

void FieldScriptSetParticleBankPhysics(void) {
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].speed = FieldScriptArgument1(1, SCRIPT_READ_U8_REL(0xD));
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].gravity.vx = FieldScriptArgument2(3, SCRIPT_READ_U8_REL(0xD));
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].gravity.vy = FieldScriptArgument3(5, SCRIPT_READ_U8_REL(0xD));
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].gravity.vz = FieldScriptArgument4(7, SCRIPT_READ_U8_REL(0xD));
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].srange = FieldScriptArgument5(9, SCRIPT_READ_U8_REL(0xD));
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].erange = FieldScriptArgument6(0xB, SCRIPT_READ_U8_REL(0xD));
    g_FieldScriptMaxInstructionCount += 4;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 0xE;
}

void FieldScriptSetParticleBankParameters(void) {
    int flags;
    int flags_2;

    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].pswait = FieldScriptVMGetArgument(1);
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].pewait = FieldScriptVMGetArgument(3);
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].shape = FieldScriptVMGetArgument(5);
    flags = FieldScriptVMGetArgument(7);
    flags_2 = (FieldScriptVMGetArgument(9) * 2);
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].flags = flags | flags_2 | D_800B2378;
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].unk72 = D_800B237C;
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].unk74 = D_800B2380;
    g_FieldScriptMaxInstructionCount += 4;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 0xB;
}

void FieldScriptSetParticleBankScale(void) {
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].scale.vx = FieldScriptArgument1(1, SCRIPT_READ_U8_REL(0x9));
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].scale.vy = FieldScriptArgument2(3, SCRIPT_READ_U8_REL(0x9));
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].scale.vz = 0;
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].scaleDelta.vx = FieldScriptArgument3(5, SCRIPT_READ_U8_REL(0x9));
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].scaleDelta.vy = FieldScriptArgument4(7, SCRIPT_READ_U8_REL(0x9));
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].scaleDelta.vz = 0;
    g_FieldScriptMaxInstructionCount += 4;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 0xA;
}

void FieldScriptSetParticleBankColor(void) {
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].color.r  = FieldScriptArgument1(1, SCRIPT_READ_U8_REL(0xD));
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].color.g = FieldScriptArgument2(3, SCRIPT_READ_U8_REL(0xD));
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].color.b = FieldScriptArgument3(5, SCRIPT_READ_U8_REL(0xD));
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].colorDelta.r = FieldScriptArgument4(7, SCRIPT_READ_U8_REL(0xD));
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].colorDelta.g = FieldScriptArgument5(9, SCRIPT_READ_U8_REL(0xD));
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].colorDelta.b = FieldScriptArgument6(0xB, SCRIPT_READ_U8_REL(0xD));
    g_FieldScriptMaxInstructionCount += 4;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 0xE;
}

void FieldScriptParticlesInitialize(void) {
    g_FieldScriptMaxInstructionCount += 4;
    if (D_800ADB8C == 0) {
        FieldInitializeParticleBanks(D_800AFD1C);
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 1;
}

void FieldScriptStopParticleActor(void) {
    g_FieldScriptMaxInstructionCount += 4;
    FieldParticleActorStop(D_800AFD1C, SCRIPT_READ_U8_REL(0x1));
    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
}
// End of particle handlers

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_80089B54);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_80089BF0);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_80089DCC);

extern s16 D_800B22E0;

void func_80089F18(void) {
    D_800B22E0 = FieldScriptVMGetArgument(1);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

extern u8 D_800B21D2;

void func_80089F54(void) {
    D_800B21D2 = FieldScriptVMGetArgument(1) - 0x80;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

extern s32 D_800AFE84;

void func_80089F94(void) {
    D_800AFE84 = SCRIPT_READ_U8_REL(1);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_80089FD0);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008A08C);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008A148);

extern s32 D_800ADB88;

void func_8008A244(void) {
    if (D_800ADB88 == 0) {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 1;
    } else {
        g_FieldScriptVMCurActor->scriptInstructionPointer -= 1;
    }
    D_800B00C0 = 1;
}

extern s32 D_800B06A0;

void func_8008A2A0(void) {
    FieldScriptMemoryWriteU16(SCRIPT_IMM_ARG(1), D_800B06A0);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008A2E8);

void func_8008A4E0(void) {}
void func_8008A4E8(void) {}
void func_8008A4F0(void) {}
void func_8008A4F8(void) {}
void func_8008A500(void) {}
void func_8008A508(void) {}
void func_8008A510(void) {}
void func_8008A518(void) {}

void func_8008A520(void) {
    while (func_8008A558()) {
        Vsync(0);
    }
}

extern s32 D_800ADB2C;
extern s32 D_800ADB90;
extern s32 D_800ADB1C;

s32 func_8008A558(void) {
    if (D_800ADB2C != 0) {
        return -1;
    }
    if (ArchiveDataSync() != 0) {
        return -1;
    }
    ArchiveCdDataSync(0);
    return 0;
}

void func_8008A5A0(void) {
    if (SCRIPT_READ_U8_REL(1) == 0) {
        func_8003633C(0);
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 1;
}

extern s16 D_800B21D4;

void func_8008A604(void) {
    D_800B21D4 = FieldScriptVMGetArgument(1);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008A640);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008A6E0);

s32 func_8008A790(s32 value, s32* outIndex) {
    s32 i;
    for (i = 0; i < MAX_PARTY_MEMBERS; i++) {
        if (g_GamePartyMembers[i] == value) {
            return -1;
        }
        if (g_GamePartyMembers[i] == 0xFF) {
            *outIndex = i;
            return 0;
        }
    }
    return -1;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008A7DC);

void func_8008A93C(void) {
    g_FieldScriptVMCurActor->unkAnimationId = ~SCRIPT_READ_U8_REL(1);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
}

void func_8008A974(void) {
    func_8008A93C();
    g_FieldScriptVMCurActor->flags &= 0xFFFEFFFF;
}

void func_8008A9AC(void) {
    if (func_8008A558() == 0) {
        D_800ADB90 = 0;
        SpriteSetSpecialAnimFile(
            (SpriteData*)(uintptr_t)g_FieldActors[D_800AFD1C].pSpriteData,
            (void*)(uintptr_t)g_FieldScriptVMCurActor->unk120);
        g_FieldScriptVMCurActor->scriptInstructionPointer += 1;
    } else {
        g_FieldScriptVMCurActor->scriptInstructionPointer -= 1;
    }
    D_800B00C0 = 1;
}

void func_8008AA60(void) {
    if (g_FieldScriptVMCurActor->unk124 != -1) {
        HeapFree((void*)(uintptr_t)g_FieldScriptVMCurActor->unk120);
        g_FieldScriptVMCurActor->unk124 = -1;
    }
    D_800B00C0 = 1;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 1;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008AACC);

void func_8008ACE8(void) {
    s32 arg;
    s32 archiveIndex;
    s32 archiveSize;
    void* pBuffer;

    if (D_800ADB90 == 0 && D_800ADB2C == 0) {
        if (func_8008A558() != 0) {
            D_800B00C0 = 1;
            g_FieldScriptVMCurActor->scriptInstructionPointer -= 1;
            return;
        }

        if (g_FieldScriptVMCurActor->unk124 != -1) {
            HeapFree((void*)(uintptr_t)g_FieldScriptVMCurActor->unk120);
            g_FieldScriptVMCurActor->unk124 = -1;
        }

        arg = FieldScriptVMGetArgument(1);
        archiveIndex = arg;
        ArchiveSetIndex(4, 0);
        archiveIndex += 0x77A;
        archiveSize = ArchiveDecodeAlignedSize(archiveIndex);
        g_FieldScriptVMCurActor->unk124 = archiveIndex;
        pBuffer = HeapAlloc(archiveSize + 8, 0);
        g_FieldScriptVMCurActor->unk120 = (u32)(uintptr_t)pBuffer;
        ArchiveReadFileToBuffer(
            archiveIndex,
            pBuffer,
            0,
            0x80);
        if (D_800ADB1C == 0) {
            ArchiveCdDataSync(0);
        }
        D_800ADB90 = 1;
        g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
    } else {
        g_FieldScriptVMCurActor->scriptInstructionPointer -= 1;
    }
    D_800B00C0 = 1;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008AE5C);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008AEC8);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008AFD8);

extern u8 D_800B225C;
extern u8 D_800B225D;
extern u8 D_800B225E;

void func_8008B0E8(void) {
    D_800B225C = FieldScriptVMGetArgument(1);
    D_800B225D = FieldScriptVMGetArgument(3);
    D_800B225E = FieldScriptVMGetArgument(5);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 7;
}

extern s16 D_800B21B4;

void func_8008B144(void) {
    D_800B21B4 = FieldScriptVMGetArgument(1);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008B180);

void func_8008B210(void) {
    g_FieldScriptVMCurActor->unk11E = FieldScriptVMGetArgument(1);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void func_8008B248(void) {
    FieldFadeSetParameters(1,
        FieldScriptVMGetArgument(9),
        FieldScriptVMGetArgument(3),
        FieldScriptVMGetArgument(5),
        FieldScriptVMGetArgument(7),
        FieldScriptVMGetArgument(1));
    g_FieldScriptVMCurActor->scriptInstructionPointer += 0xB;
}

void func_8008B2F0(void) {
    FieldDistortionInitialize(0);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 0xF;
}

void func_8008B328(void) {
    switch (SCRIPT_READ_U8_REL(1)) {
        case 0:
            FieldDistortionSetTarget(0, 0, 0, 0, 0, 0, FieldScriptVMGetArgument(2));
            g_FieldEffects.distortion.isFinished = 1;
            g_FieldScriptVMCurActor->scriptInstructionPointer += 4;
            break;
        case 1:
            if (g_FieldEffects.distortion.isActive == 0) {
                g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
            } else {
                g_FieldScriptVMCurActor->scriptInstructionPointer -= 1;
            }
            break;
        case 2:
            g_FieldEffects.distortion.isActive = 0;
            g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
            break;
        case 3:
            FieldDistortionFree();
            g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
            break;
    }

    D_800B00C0 = 1;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008B45C);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008B518);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008B5D4);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008B894);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008B978);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008BC80);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008BDD8);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008BF38);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008C180);

extern s32 D_800ADBC4;
extern s32 g_GamePartyMemberSkins[];
extern void* g_PartyDataBuffers[];
extern s32 D_8005A444[];
extern s32 g_PlayerActorIndex;
extern s32 FieldCharacterIdToPartyId(s32 characterId);
extern s32 func_8008CF3C(s32);
extern void func_8008BF38(s32 partyId);
extern void func_8008C180(s32 partyId);

typedef struct {
    u32 words[4];
} PartyBufferBlock;

void func_8008C334(void) {
    s32 partyId;
    PartyBufferBlock* src;
    PartyBufferBlock* dst;
    PartyBufferBlock* end;
    ActorData* player;

    if (D_800ADBC4 != 0xFF) {
        D_800B00C0 = 1;
        g_FieldScriptVMCurActor->scriptInstructionPointer--;
        return;
    }

    DrawSync(0);
    partyId = FieldCharacterIdToPartyId(func_8008CF3C(SCRIPT_READ_U8_REL(1)));
    if (partyId == -1) {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
        return;
    }

    if (D_800ADB1C == 0) {
        switch (partyId) {
        case 0:
            if (g_GamePartyMembers[1] == 0xFF) {
                g_GamePartyMembers[0] = 0xFF;
                g_GamePartyMemberSkins[0] = 0xFF;
                g_pGameState->gearRide[0] = 0;
                break;
            }

            src = g_PartyDataBuffers[1];
            dst = g_PartyDataBuffers[0];
            end = src + (0x14000 / sizeof(*src));
            do {
                *dst++ = *src++;
            } while (src != end);

            g_GamePartyMembers[0] = g_GamePartyMembers[1];
            g_GamePartyMemberSkins[0] = g_GamePartyMemberSkins[1];
            g_GamePartyMembers[1] = 0xFF;
            g_GamePartyMemberSkins[1] = 0xFF;
            g_pGameState->gearRide[0] = g_pGameState->gearRide[1];

            if (g_GamePartyMembers[2] == 0xFF) {
                break;
            }

            src = g_PartyDataBuffers[2];
            dst = g_PartyDataBuffers[1];
            end = src + (0x14000 / sizeof(*src));
            do {
                *dst++ = *src++;
            } while (src != end);

            g_GamePartyMembers[1] = g_GamePartyMembers[2];
            g_GamePartyMemberSkins[1] = g_GamePartyMemberSkins[2];
            g_GamePartyMembers[2] = 0xFF;
            g_GamePartyMemberSkins[2] = 0xFF;
            g_pGameState->gearRide[1] = g_pGameState->gearRide[2];
            break;

        case 1:
            if (g_GamePartyMembers[2] == 0xFF) {
                g_GamePartyMembers[1] = 0xFF;
                g_GamePartyMemberSkins[1] = 0xFF;
                g_pGameState->gearRide[1] = 0;
                break;
            }

            src = g_PartyDataBuffers[2];
            dst = g_PartyDataBuffers[1];
            end = src + (0x14000 / sizeof(*src));
            do {
                *dst++ = *src++;
            } while (src != end);

            g_GamePartyMembers[1] = g_GamePartyMembers[2];
            g_GamePartyMemberSkins[1] = g_GamePartyMemberSkins[2];
            g_GamePartyMembers[2] = 0xFF;
            g_GamePartyMemberSkins[2] = 0xFF;
            g_pGameState->gearRide[1] = g_pGameState->gearRide[2];
            g_pGameState->gearRide[2] = 0;
            break;

        case 2:
            g_GamePartyMembers[2] = 0xFF;
            g_GamePartyMemberSkins[2] = 0xFF;
            g_pGameState->gearRide[2] = 0;
            break;
        default:
            goto advance;
        }
    } else {
        switch (partyId) {
        case 0:
            g_pGameState->gearRide[0] = g_pGameState->gearRide[1];
            g_pGameState->gearRide[1] = g_pGameState->gearRide[2];
            g_pGameState->gearRide[2] = 0;
            func_8008C180(0);
            func_8008BF38(0);
            func_8008BF38(1);
            break;
        case 1:
            g_pGameState->gearRide[1] = g_pGameState->gearRide[2];
            g_pGameState->gearRide[2] = 0;
            func_8008C180(1);
            func_8008BF38(1);
            break;
        case 2:
            g_pGameState->gearRide[2] = 0;
            func_8008C180(2);
            break;
        }

        if (g_GamePartyMembers[0] != 0xFF) {
            if (D_8005A444[0] != 0xFF) {
                g_PlayerActorIndex = D_8005A444[0];
                player = (ActorData*)(uintptr_t)g_FieldActors[D_8005A444[0]].pActorData;
                player->scriptFlags.flags = (player->scriptFlags.flags | 0x4400) & ~0x80;
            } else {
                g_PlayerActorIndex = 0;
            }
        } else {
            g_PlayerActorIndex = 0;
        }
    }

advance:
    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008C7D8);

extern s32 D_8004F36C;
extern s32 D_8004F324;
extern void *D_80062528;
extern void func_8003A89C();

// FE 0E — field music cue: sends a command to the music manager once a song is
// loaded (D_8004F36C); skips when no music is selected (id 0xFF) or the field is
// not yet live (D_800ADB1C == 0); otherwise retries until the song load completes.
void func_8008C84C(void) {
    s32 arg1;

    if (D_8004F36C != 0) {
        arg1 = FieldScriptVMGetArgument(1);
        func_8003A89C(D_80062528, arg1, FieldScriptVMGetArgument(3));
        g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
    } else if (D_8004F324 == 0xFF) {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
    } else if (D_800ADB1C == 0) {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
    } else {
        g_FieldScriptVMCurActor->scriptInstructionPointer -= 1;
    }
    D_800B00C0 = 1;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008C938);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008CA60);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008CB4C);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008CC74);

/* Extended VM opcode 0x13 (Noah OPX_13): set the actor's pending sound
 * fields (+0x10A id, +0x10C param, +0x10D state=0) and release any SPU
 * voice channel still bound to this actor (func_800863E8). Id 0 means
 * "none" -> state 0xFF. asm nonmatchings/main/misc/func_8008CD48.s. */
extern void func_800863E8(s32 actorIdx);

void func_8008CD48(void) {
    g_FieldScriptVMCurActor->unk10A = (s16)FieldScriptVMGetArgument(1);
    g_FieldScriptVMCurActor->unk10D = 0;
    g_FieldScriptVMCurActor->unk10C = (u_char)FieldScriptVMGetArgument(3);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
    func_800863E8(D_800AFD1C);
    if ((u16)g_FieldScriptVMCurActor->unk10A == 0) {
        g_FieldScriptVMCurActor->unk10D = 0xFF;
    }
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008CDD4);

void func_8008CE64(void) {
    s32 v = func_8008CF3C(FieldScriptVMGetArgument(1));
    if (v != 0xFF) {
        g_pGameState->FrMask &= ~(1 << v);
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void func_8008CED0(void) {
    s32 v = func_8008CF3C(FieldScriptVMGetArgument(1));
    if (v != 0xFF) {
        g_pGameState->FrMask |= 1 << v;
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

s32 func_8008CF3C(s32 a0) {
    switch (a0) {
    case 0xFF:
        return g_GamePartyMembers[2];
    case 0xFE:
        return g_GamePartyMembers[1];
    case 0xFD:
        return g_GamePartyMembers[0];
    case 0xFC:
        return 0xFF;
    }
    return a0;
}

void func_8008CF9C(void) {
    g_FieldScriptVMCurActor->faceId = func_8008CF3C(FieldScriptVMGetArgument(1));
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008CFEC);

void func_8008D078(void) {
    if (FieldScriptVMGetArgument(1) == 0) {
        *(s32*)((u8*)g_FieldScriptVMCurActor + 0x4) &= ~0x800;
    } else {
        *(s32*)((u8*)g_FieldScriptVMCurActor + 0x4) |= 0x800;
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

/* Extended field-script opcode EX-0x03 (SET_CURRENT_ACTOR_SCALE), reached via
 * the 0xFE prefix -> FieldScriptVM2Run. Sets the current actor's sprite scale
 * (SpriteData+0x2C) and 3D scale (scaleX/Y/Z), then advances the script IP by 3.
 * Decompiled from func_8008D0F4.s. This was previously INCLUDE_ASM, which on the
 * native port became a no-op logging stub that never advanced scriptInstructionPointer;
 * the VM then re-dispatched the operand bytes as a bogus top-level dialog opcode,
 * opening a garbage text box (the Lahan-intro "garbled dialog"). The IP += 3 here
 * is the load-bearing fix. */
void func_8008D0F4(void) {
    s32 scale = FieldScriptVMGetArgument(1);
    *(s16*)((u8*)(uintptr_t)g_FieldActors[D_800AFD1C].pSpriteData + 0x2C) = (scale * 3) >> 2;
    g_FieldScriptVMCurActor->scaleX = scale;
    g_FieldScriptVMCurActor->scaleY = scale;
    g_FieldScriptVMCurActor->scaleZ = scale;
    func_80072254(D_800AFD1C);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008D180);

extern s16 D_800B218C;

/* FE3A — write party frame mask. */
void func_8008D230(void) {
    D_800B218C = FieldScriptVMGetArgument(1);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void func_8008D26C(void) {
    *(s16*)((u8*)(uintptr_t)g_FieldActors[D_800AFD1C].pSpriteData + 0x82) = FieldScriptVMGetArgument(1) << 1;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void func_8008D2D8(void) {
}

void func_8008D2E0(s32 a0, s32 a1) {
    *(u8*)(a1 + (s32)g_FieldScriptVMCurScriptData + 1) = a0 >> 8;
    *(u8*)((u8*)g_FieldScriptVMCurScriptData + a1) = a0;
}

s32 func_8008D30C(s32 a0, s32 a1) {
    s32 dx = g_FieldActors[a0].transformMatrix.t[0] - g_FieldActors[a1].transformMatrix.t[0];
    s32 dz = g_FieldActors[a0].transformMatrix.t[2] - g_FieldActors[a1].transformMatrix.t[2];
    return -(FieldGetVec2Magnitude(dx, dz) >= 0x10);
}

static inline void CopySpriteFieldS32(FieldActor* actors, int dstId, int srcId, int offset) {
    void* dst = (void*)(uintptr_t)actors[dstId].pSpriteData;
    void* src = (void*)(uintptr_t)actors[srcId].pSpriteData;
    *(s32*)((u8*)dst + offset) = *(s32*)((u8*)src + offset);
}

static inline void CopySpriteFieldU16(FieldActor* actors, int dstId, int srcId, int offset) {
    void* dst = (void*)(uintptr_t)actors[dstId].pSpriteData;
    void* src = (void*)(uintptr_t)actors[srcId].pSpriteData;
    *(u16*)((u8*)dst + offset) = *(u16*)((u8*)src + offset);
}

static inline void CopyActorFieldS32(FieldActor* base, int dstId, int srcId, int offset) {
    *(s32*)((u8*)&base[dstId] + offset) = *(s32*)((u8*)&base[srcId] + offset);
}

void FieldActorCopyPlacement(int dstActorId, int srcActorId) {
    FieldActor* actors = g_FieldActors;
    ActorData* dstActor;
    ActorData* srcActor;
    int i;

    srcActor = (ActorData*)(uintptr_t)actors[srcActorId].pActorData;
    dstActor = (ActorData*)(uintptr_t)actors[dstActorId].pActorData;

    for (i = 0; i < 4; i++) {
        dstActor->walkmeshTriIds[i] = srcActor->walkmeshTriIds[i];
    }
    {
      s16 walkmeshId = srcActor->walkmeshId;
      dstActor->walkmeshId = walkmeshId;
    }
    dstActor->curTriNormal.vx = srcActor->curTriNormal.vx;
    dstActor->curTriNormal.vy = srcActor->curTriNormal.vy;
    dstActor->curTriNormal.vz = srcActor->curTriNormal.vz;
    dstActor->position.vx = srcActor->position.vx;
    dstActor->position.vy = srcActor->position.vy;
    dstActor->position.vz = srcActor->position.vz;
    dstActor->unkEC = srcActor->unkEC;
    dstActor->curYPos = srcActor->curYPos;
    dstActor->curWalkmeshTriMaterial = srcActor->curWalkmeshTriMaterial;

    CopySpriteFieldU16(g_FieldActors, dstActorId, srcActorId, offsetof(SpriteData, field_0x84));
    CopySpriteFieldS32(g_FieldActors, dstActorId, srcActorId, offsetof(SpriteData, position.x));
    CopySpriteFieldS32(g_FieldActors, dstActorId, srcActorId, offsetof(SpriteData, position.y));
    CopySpriteFieldS32(g_FieldActors, dstActorId, srcActorId, offsetof(SpriteData, position.z));

    CopyActorFieldS32(g_FieldActors, dstActorId, srcActorId, offsetof(FieldActor, transformMatrix.t[0]));
    CopyActorFieldS32(g_FieldActors, dstActorId, srcActorId, offsetof(FieldActor, transformMatrix.t[1]));
    CopyActorFieldS32(g_FieldActors, dstActorId, srcActorId, offsetof(FieldActor, transformMatrix.t[2]));
}

extern s32 D_8005A444[];
extern u8 D_800B219F;

void func_8008D570(void) {
    s32 i;
    for (i = 0; i < 3; i++) {
        if (D_8005A444[i] == D_800AFD1C) {
            D_800B219F &= ~(1 << i);
        }
    }
}

extern u8 D_800B21CD;

/* FE26 — write distortion/effect flag byte. */
void func_8008D5C8(void) {
    D_800B21CD = SCRIPT_READ_U8_REL(1);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
}

/* FE07/opcode 0x107 -- set (arg 1) or clear (arg 0) the current actor's
 * 0x400 flag; any other arg leaves it untouched.  IP += 2.  As a no-op stub
 * this handler never advanced the IP, so the VM's next dispatch executed the
 * FE-prefix's extension byte as a BASE opcode -- desyncing the actor's whole
 * script stream (MAP3's boot SEGV: garbage actor index 128 fed to
 * func_8009EB78 several bogus opcodes later). */
void func_8008D604(void) {
    u8 arg = SCRIPT_READ_U8_REL(1);

    if (arg == 0) {
        g_FieldScriptVMCurActor->flags &= ~0x400;
    } else if (arg == 1) {
        g_FieldScriptVMCurActor->flags |= 0x400;
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
}

void func_8008D684(void) {
    int mask;
    unsigned int slotIndex;

    slotIndex = (unsigned int) (SCRIPT_IMM_ARG(1)) >> 4;
    mask = 1 << (FieldScriptVMGetInstructionArgument(1) & 0xF);
    FieldScriptMemoryWriteU16(slotIndex, FieldScriptVMGetVariableValue(slotIndex) | mask);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void func_8008D700(void) {
    int mask;
    unsigned int slotIndex;

    slotIndex = (unsigned int) (SCRIPT_IMM_ARG(1)) >> 4;
    mask = 1 << (FieldScriptVMGetInstructionArgument(1) & 0xF);
    FieldScriptMemoryWriteU16(slotIndex, FieldScriptVMGetVariableValue(slotIndex) & ~mask);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void func_8008D780(void) {
    short destination;
    int mask;
    unsigned int slotIndex;

    slotIndex = (unsigned int) (SCRIPT_IMM_ARG(1)) >> 4;
    mask = 1 << (FieldScriptVMGetInstructionArgument(1) & 0xF);
    if (FieldScriptVMGetVariableValue(slotIndex) & mask) {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
    } else {
        destination = FieldScriptVMGetInstructionArgument(3);
        g_FieldScriptVMCurActor->scriptInstructionPointer = destination;
    }
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008D808);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008DA04);

void func_8008DAFC(void) {
    g_FieldScriptVMCurActor->parentActorId = 0xFF;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 1;
}

extern s16 D_800B233E;

void func_8008DB2C(void) {
    D_800B233E = FieldScriptVMGetActorIndex(1);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
}


int FieldPartyMemberIncreaseGearHp(int partyMemberIndex, unsigned int amount) {
    int gearId;
    unsigned int maxHp;
    unsigned int newHp;

    gearId = GameCharacterGetGearID(g_GamePartyMembers[partyMemberIndex]);
    if (gearId != CHARACTER_ID_NONE) {
        maxHp = g_pGameState->gears[gearId].maxHp;
        newHp = g_pGameState->gears[gearId].hp + amount;
        g_pGameState->gears[gearId].hp = newHp;
        if (maxHp < newHp) {
            g_pGameState->gears[gearId].hp = maxHp;
        }
    }
}

int FieldPartyMemberDecreaseGearHp(int partyMemberIndex, unsigned int amount) {
    int gearId;
    int newHp;

    gearId = GameCharacterGetGearID(g_GamePartyMembers[partyMemberIndex]);
    if (gearId != CHARACTER_ID_NONE) {
        newHp = g_pGameState->gears[gearId].hp - amount;
        if (newHp <= 0) {
            newHp = 1;
        }
        g_pGameState->gears[gearId].hp = newHp;
    }
}

extern s16 g_FieldNumPartyMembersMasks[4];

void FieldScriptVMHandlerIncreasePartyGearHp(void) {
    int mask;
    int amount;
    int i;

    amount = FieldScriptArgument1(1, SCRIPT_READ_U8_REL(3));
    mask = g_FieldNumPartyMembersMasks[SCRIPT_READ_U8_REL(3) & 0x3];

    for (i = 0; i < MAX_PARTY_MEMBERS; i++) {
        if ((g_GamePartyMembers[i] != CHARACTER_ID_NONE) && (mask & 1)) {
            FieldPartyMemberIncreaseGearHp(i, amount);
        }
        mask >>= 1;
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 4;
}

void FieldScriptVMHandlerDecreasePartyGearHp(void) {
    int mask;
    int amount;
    int i;

    amount = FieldScriptArgument1(1, SCRIPT_READ_U8_REL(3));
    mask = g_FieldNumPartyMembersMasks[SCRIPT_READ_U8_REL(3) & 0x3];

    for (i = 0; i < MAX_PARTY_MEMBERS; i++) {
        if ((g_GamePartyMembers[i] != CHARACTER_ID_NONE) && (mask & 1)) {
            FieldPartyMemberDecreaseGearHp(i, amount);
        }
        mask >>= 1;
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 4;
}

void FieldScriptSetParentActor(void) {
    int actorIndex = FieldScriptVMGetActorIndex(1);
    if (actorIndex != ACTOR_ID_INVALID) {
        g_FieldScriptVMCurActor->parentActorId = actorIndex;
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
}

void FieldScriptWriteActorFlags1(void) {
    FieldActor* actors = g_FieldActors;
    int actorIndex = FieldScriptVMGetActorIndex(1);

    if (actorIndex != ACTOR_ID_INVALID) {
        u8* pActorData = (u8*)(uintptr_t)actors[actorIndex].pActorData;
        FieldScriptMemoryWriteU16(SCRIPT_IMM_ARG(1), *(u32*)pActorData);
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void FieldScriptWriteActorFlags2(void) {
    FieldActor* actors = g_FieldActors;
    int actorIndex = FieldScriptVMGetActorIndex(1);

    if (actorIndex != ACTOR_ID_INVALID) {
        u8* pActorData = (u8*)(uintptr_t)actors[actorIndex].pActorData;
        FieldScriptMemoryWriteU16(SCRIPT_IMM_ARG(1), *(u16*)(pActorData + 2));
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void FieldScriptWriteActorFlags3(void) {
    FieldActor* actors = g_FieldActors;
    int actorIndex = FieldScriptVMGetActorIndex(1);

    if (actorIndex != ACTOR_ID_INVALID) {
        u8* pActorData = (u8*)(uintptr_t)actors[actorIndex].pActorData;
        FieldScriptMemoryWriteU16(SCRIPT_IMM_ARG(1), *(u32*)(pActorData + 4));
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void FieldScriptWriteActorFlags4(void) {
    FieldActor* actors = g_FieldActors;
    int actorIndex = FieldScriptVMGetActorIndex(1);

    if (actorIndex != ACTOR_ID_INVALID) {
        u8* pActorData = (u8*)(uintptr_t)actors[actorIndex].pActorData;
        FieldScriptMemoryWriteU16(SCRIPT_IMM_ARG(1), *(u16*)(pActorData + 6));
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

// TODO: These two handlers could use more semantic names since they're likely used
// in certain situations.
void FieldScriptVMConditionalJump6(unsigned short flag) {
    unsigned short argument = SCRIPT_IMM_ARG(1);
    if (argument & flag) {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 6;
    } else {
        g_FieldScriptVMCurActor->scriptInstructionPointer = FieldScriptVMGetInstructionArgument(4);
    }
}

void FieldScriptVMConditionalJump5(unsigned short flag) {
    unsigned short argument = SCRIPT_IMM_ARG(1);
    if (argument & flag) {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
    } else {
        g_FieldScriptVMCurActor->scriptInstructionPointer = SCRIPT_IMM_ARG(2);
    }
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc", FieldScriptWriteActorDistance);
/*
Matches as long as g_FieldActors is NOT volatile.

void FieldScriptWriteActorDistance(void) {
    ActorData* pActorA;
    ActorData* pActorB;
    int actorIndexA;
    int actorIndexB;
    int distance;

    distance = 0;
    actorIndexA = FieldScriptVMGetActorIndex(3);
    actorIndexB = FieldScriptVMGetActorIndex(4);
    if ((actorIndexA != ACTOR_ID_INVALID) && (actorIndexB != ACTOR_ID_INVALID)) {
        pActorA = (ActorData*)(uintptr_t)g_FieldActors[actorIndexA].pActorData;
        pActorB = (ActorData*)(uintptr_t)g_FieldActors[actorIndexB].pActorData;
        distance = FieldGetVec2Magnitude(
            CONV_TO_GTE(pActorA->position.vx) - CONV_TO_GTE(pActorB->position.vx), 
            CONV_TO_GTE(pActorA->position.vz) - CONV_TO_GTE(pActorB->position.vz)
        );
    }
    FieldScriptMemoryWriteU16(
        SCRIPT_IMM_ARG(1), 
        distance
    );
    g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
}
*/

void func_8008E298(void) {
    FieldScriptVMConditionalJump6(((u16*)g_FieldActors[FieldScriptVMGetActorIndex(3)].pActorData)[0]);
}

void func_8008E2EC(void) {
    FieldScriptVMConditionalJump6(((u16*)g_FieldActors[FieldScriptVMGetActorIndex(3)].pActorData)[1]);
}

void func_8008E340(void) {
    FieldScriptVMConditionalJump6(((u16*)g_FieldActors[FieldScriptVMGetActorIndex(3)].pActorData)[2]);
}

void func_8008E394(void) {
    FieldScriptVMConditionalJump6(((u16*)g_FieldActors[FieldScriptVMGetActorIndex(3)].pActorData)[3]);
}

void func_8008E3E8(void) {
    FieldScriptVMConditionalJump5(((u16*)g_FieldScriptVMCurActor)[0]);
}

void func_8008E414(void) {
    FieldScriptVMConditionalJump5(((u16*)g_FieldScriptVMCurActor)[1]);
}

void func_8008E440(void) {
    FieldScriptVMConditionalJump5(((u16*)g_FieldScriptVMCurActor)[2]);
}

void func_8008E46C(void) {
    FieldScriptVMConditionalJump5(((u16*)g_FieldScriptVMCurActor)[3]);
}

void func_8008E498(int a0) {
    FieldScriptMemoryWriteU16(SCRIPT_IMM_ARG(1), a0 & 0xFFFF);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void func_8008E4EC(void) {
    func_8008E498(((u16*)g_FieldScriptVMCurActor)[0]);
}

void func_8008E518(void) {
    func_8008E498(((u16*)g_FieldScriptVMCurActor)[1]);
}

void func_8008E544(void) {
    func_8008E498(((u16*)g_FieldScriptVMCurActor)[2]);
}

void func_8008E570(void) {
    func_8008E498(((u16*)g_FieldScriptVMCurActor)[3]);
}

void func_8008E59C(void) {
    u32 mask;

    mask = FieldScriptVMGetInstructionArgument(2) & 0xFFFF;
    switch (SCRIPT_READ_U8_REL(1)) {
        case 0:
            g_FieldScriptVMCurActor->scriptFlags.flags |= mask;
            break;

        case 1:
            g_FieldScriptVMCurActor->scriptFlags.flags |= mask << 16;
            break;

        case 2:
            g_FieldScriptVMCurActor->flags |= mask;
            break;

        case 3:
            g_FieldScriptVMCurActor->flags |= mask << 16;
            break;

        case 4:
            g_FieldScriptVMCurActor->scriptFlags.flags &= ~mask;
            break;

        case 5:
            g_FieldScriptVMCurActor->scriptFlags.flags &= ~(mask << 16);
            break;

        case 6:
            g_FieldScriptVMCurActor->flags &= ~mask;
            break;

        case 7:
            g_FieldScriptVMCurActor->flags &= ~(mask << 16);
            break;
    }

    g_FieldScriptVMCurActor->scriptInstructionPointer += 4;
}

/* func_8008E718 -- step-counter / formation-cooldown REFRESH. Decompiled from
 * asm/field/nonmatchings/main/misc/func_8008E718.s (was INCLUDE_ASM/stubbed).
 * Called by func_80079288 (roll, when the step counter hits 0) and by the
 * encounter-setup opcode func_8008E85C below. Reloads the step counter from the
 * encounter base and assigns each active formation a unique random cooldown.
 * See docs/ai_context/ACTIVE_HANDOFF.md. */
extern s32 D_800B2294, D_800B2298, D_800B229C;
extern s16 D_800B22A0[];

void func_8008E718(void) {
    u16 *timers = (u16 *)D_800B22A0;
    s32  i, j, roll;

    D_800B2294 = D_800B2298;

    if (D_800B229C == 0) {
        D_800B2298 = 0;
        return;
    }

    for (i = 31; i >= 0; i--) {
        timers[i] = 0xFFFF;
    }

    if (D_800B229C > 0) {
        for (i = 0; i < D_800B229C; i++) {
            do {
                roll = ((rand() * (D_800B2298 + 1)) >> 15) & 0xFFFF;
                for (j = 0; j < 32; j++) {
                    if (timers[j] == (u16)roll) break;
                }
            } while (j < 32);
            timers[i] = (u16)roll;
        }
    }

    if (D_800B229C > 0) {
        for (i = 0; i < D_800B229C; i++) {
            timers[i] = (u16)(timers[i] + 1);
        }
    }
}

void func_8008E85C(void) {
    D_800B2298 = FieldScriptVMGetArgument(1);
    D_800B229C = FieldScriptVMGetArgument(3);
    if (D_800B229C >= 0x21) {
        D_800B229C = 0x20;
    }
    func_8008E718();
    g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008E8C8);

extern s32 D_800ADB7C;

void func_8008E9F8(void) {
    if (D_800ADB7C == 0) {
        g_FieldScriptVMCurActor->scriptInstructionPointer -= 1;
    } else {
        D_800ADB7C = 0;
        g_FieldScriptVMCurActor->scriptInstructionPointer += 1;
    }
    D_800B00C0 = 1;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008EA58);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008EC30);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008EE14);

extern s32 D_800ADB3C;
extern s32 D_800ADB38;

void func_8008EF5C(void) {
    D_800ADB3C = FieldScriptVMGetArgument(1);
    D_800ADB38 = 1;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void func_8008EFA0(void) {
    D_800ADB3C = FieldScriptVMGetArgument(1);
    D_800ADB38 = 2;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008EFE4);

void func_8008F070(void) {
    D_800ADB3C = FieldScriptVMGetArgument(1);
    D_800ADB38 = 3;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008F0B4);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008F1C8);

void func_8008F2D8(void) {
    func_80023290((void*)(uintptr_t)g_FieldActors[D_800AFD1C].pSpriteData, FieldScriptVMGetArgument(1));
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

extern s32 D_800C3A5C;
extern s32 D_800C3A60;

void func_8008F348(void) {
    D_800C3A5C = FieldScriptVMGetArgument(1);
    D_800C3A60 = FieldScriptVMGetArgument(3);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
}

extern s16 D_800B233C;

void func_8008F394(void) {
    D_800B233C = FieldScriptVMGetArgument(1);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void func_8008F3D0(void) {
    func_8003A450(FieldScriptVMGetArgument(3) << 1, FieldScriptVMGetArgument(1), FieldScriptVMGetArgument(5));
    g_FieldScriptVMCurActor->scriptInstructionPointer += 7;
}

void func_8008F444(void) {
    func_8003A344(FieldScriptVMGetArgument(3) << 1, FieldScriptVMGetArgument(1));
    g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
}

void func_8008F4A0(void) {
    func_8003A55C(FieldScriptVMGetArgument(3) << 1, FieldScriptVMGetArgument(1));
    g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
}

void func_8008F4FC(void) {
    func_80085634(FieldScriptVMGetArgument(1), FieldScriptVMGetArgument(3));
    g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
}

void func_8008F558(void) {
    s32 soundId = FieldScriptVMGetArgument(1);
    s32 volume = FieldScriptVMGetArgument(5);
    s32 pan = FieldScriptVMGetArgument(3);
    s32 channel = FieldScriptVMGetArgument(7);

    func_800855C8(soundId, volume, pan, channel);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 9;
}

/* FE 64 — sound-bit gate (asm 8008F5E4-8008F664).
 * FieldScriptVM2Run leaves IP on the 0x64 sub-opcode. Poll
 * func_8003A5D0(-1) & (arg1<<8): clear → IP+=3 (skip 64+2 args);
 * set → IP-=1 (back to FE, wait). Always yields (D_800B00C0=1).
 * A stubbed FE64 left IP on 0x64 and let the primary VM desync
 * A1 r5 past SHOW (0x22), so Fei stayed hidden after A14 HideById. */
extern s32 func_8003A5D0(s32);

void func_8008F5E4(void) {
    s32 soundBits = func_8003A5D0(-1);
    s32 mask = FieldScriptVMGetArgument(1) << 8;

    if ((soundBits & mask) != 0) {
        g_FieldScriptVMCurActor->scriptInstructionPointer -= 1;
    } else {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
    }
    D_800B00C0 = 1;
}

void func_8008F668(void) {
    func_80085634(FieldScriptVMGetArgument(1), 3);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void func_8008F6AC(void) {
    func_800855C8(FieldScriptVMGetArgument(1), FieldScriptVMGetArgument(5), FieldScriptVMGetArgument(3), 3);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 7;
}

extern s32 D_800ADBDC;
extern s32 D_8004F340;
extern s32 D_800ADB1C;
extern s32 D_8004F324;
extern s32 D_8004F308;
extern s32 D_8004F354;
extern void func_80085EEC(void);
extern void func_8001B66C(void);
extern void func_80085B20(s32);

void func_8008F724(void) {
    if (D_800ADBDC == 0) {
        D_800B00C0 = 1;
    } else {
        D_8004F340 = 0;
        func_8008F7B8();
    }
}

void func_8008F76C(void) {
    if (D_800ADBDC == 0) {
        D_800B00C0 = 1;
    } else {
        D_8004F340 = -1;
        func_8008F7B8();
    }
}

void func_8008F7B8(void) {
    s32 fieldId = FieldScriptVMGetArgument(1);

    if (D_800ADB1C == 0) {
        func_80085EEC();
        if (fieldId != D_8004F324) {
            func_8001B66C();
            D_8004F308 = -1;
        }
        D_8004F324 = fieldId;
        g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
        return;
    }

    if (func_8008A558() != 0) {
        D_800B00C0 = 1;
        return;
    }

    if (D_800ADBDC == 0) {
        D_800B00C0 = 1;
        return;
    }

    if (D_8004F354 == 1 || D_8004F308 == -1) {
        D_800B00C0 = 1;
        return;
    }

    if (fieldId != D_8004F324) {
        func_8001B66C();
        D_8004F324 = fieldId;
        D_8004F308 = -1;
        func_80085B20(fieldId);
    }

    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008F90C);
