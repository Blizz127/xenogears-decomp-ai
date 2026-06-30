#include "common.h"
#include "main/game.h"
#include "system/math.h"
#include "field/main.h"
#include "field/camera.h"
#include "field/actor.h"
#include "field/script_vm.h"
#include "field/text_box.h"
#include "field/particles.h"

void FieldScriptMemoryWriteU16(int, int);

extern int rcos(int);
extern int rsin(int);

extern s32 D_800AFD1C;

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

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_80087148);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_800871B0);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_800873C4);

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

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_80087960);

void func_800879D0(void) {
    FieldScriptMemoryWriteU16(SCRIPT_IMM_ARG(1), g_pGameState->unk184E);
    FieldScriptMemoryWriteU16(SCRIPT_IMM_ARG(2), g_pGameState->unk1852);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_80087A40);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_80087A7C);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_80087AB8);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_80087B5C);

extern s32 D_8004F300;

void func_80087C0C(void) {
    D_8004F300 = 1;
    g_FieldScriptVMCurActor->scriptInstructionPointer++;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_80087C34);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_80087D30);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_80087D80);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_80087DE0);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_80087E5C);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_80087E98);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_80087FA4);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_80087FD4);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008800C);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_80088198);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_800881E8);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008825C);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_800882B8);

void FieldScriptSetCharacterGear(void) {
    int characterId = FieldScriptVMGetArgument(1);
    g_pGameState->characters[characterId].gearId = FieldScriptVMGetArgument(3);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_800883D4);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008848C);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_800884CC);

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

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_80089F18);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_80089F54);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_80089F94);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_80089FD0);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008A08C);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008A148);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008A244);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008A2A0);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008A2E8);

void func_8008A4E0(void) {}
void func_8008A4E8(void) {}
void func_8008A4F0(void) {}
void func_8008A4F8(void) {}
void func_8008A500(void) {}
void func_8008A508(void) {}
void func_8008A510(void) {}
void func_8008A518(void) {}

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008A520);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008A558);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008A5A0);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008A604);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008A640);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008A6E0);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008A790);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008A7DC);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008A93C);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008A974);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008A9AC);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008AA60);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008AACC);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008ACE8);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008AE5C);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008AEC8);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008AFD8);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008B0E8);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008B144);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008B180);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008B210);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008B248);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008B2F0);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008B328);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008B45C);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008B518);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008B5D4);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008B894);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008B978);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008BC80);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008BDD8);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008BF38);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008C180);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008C334);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008C7D8);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008C84C);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008C938);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008CA60);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008CB4C);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008CC74);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008CD48);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008CDD4);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008CE64);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008CED0);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008CF3C);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008CF9C);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008CFEC);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008D078);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008D0F4);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008D180);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008D230);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008D26C);

void func_8008D2D8(void) {
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008D2E0);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008D30C);

static inline void CopySpriteFieldS32(FieldActor* actors, int dstId, int srcId, int offset) {
    void* dst = actors[dstId].pSpriteData;
    void* src = actors[srcId].pSpriteData;
    *(s32*)((u8*)dst + offset) = *(s32*)((u8*)src + offset);
}

static inline void CopySpriteFieldU16(FieldActor* actors, int dstId, int srcId, int offset) {
    void* dst = actors[dstId].pSpriteData;
    void* src = actors[srcId].pSpriteData;
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

    srcActor = actors[srcActorId].pActorData;
    dstActor = actors[dstActorId].pActorData;

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

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008D570);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008D5C8);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008D604);

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

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008DAFC);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008DB2C);


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

INCLUDE_ASM("asm/field/nonmatchings/main/misc", FieldScriptWriteActorFlags1);
/*
Matches as long as g_FieldActors is NOT volatile.

void FieldScriptWriteActorFlags1(void) {
    ActorData* pActorData;
    int actorIndex;

    actorIndex = FieldScriptVMGetActorIndex(1);
    if (actorIndex != ACTOR_ID_INVALID) {
        pActorData = g_FieldActors[actorIndex].pActorData;
        FieldScriptMemoryWriteU16(
            SCRIPT_IMM_ARG(1), 
            pActorData->scriptFlags
        );
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}
*/

INCLUDE_ASM("asm/field/nonmatchings/main/misc", FieldScriptWriteActorFlags2);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", FieldScriptWriteActorFlags3);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", FieldScriptWriteActorFlags4);

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
        pActorA = g_FieldActors[actorIndexA].pActorData;
        pActorB = g_FieldActors[actorIndexB].pActorData;
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

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008E298);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008E2EC);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008E340);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008E394);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008E3E8);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008E414);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008E440);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008E46C);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008E498);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008E4EC);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008E518);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008E544);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008E570);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008E59C);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008E718);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008E85C);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008E8C8);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008E9F8);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008EA58);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008EC30);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008EE14);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008EF5C);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008EFA0);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008EFE4);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008F070);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008F0B4);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008F1C8);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008F2D8);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008F348);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008F394);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008F3D0);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008F444);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008F4A0);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008F4FC);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008F558);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008F5E4);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008F668);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008F6AC);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008F724);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008F76C);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008F7B8);

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008F90C);