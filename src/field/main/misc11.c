#include "common.h"
#include "main/game.h"
#include "system/math.h"
#include "field/main.h"
#include "field/camera.h"
#include "field/actor.h"
#include "field/script_vm.h"
#include "field/text_box.h"
#include "field/particles.h"

extern s32 D_800AFD1C;
extern s32 g_PlayerActorIndex;
extern FieldActor* D_800B06B8;

INCLUDE_ASM("asm/field/nonmatchings/main/misc11", func_80091944);

void func_80091A08(void) {
    *(s16*)((u8*)&g_Scene + 0x4C) = FieldScriptVMGetInstructionArgumentS16(1);
    *(s16*)((u8*)&g_Scene + 0x4E) = FieldScriptVMGetInstructionArgumentS16(3);
    *(s16*)((u8*)&g_Scene + 0x50) = FieldScriptVMGetInstructionArgumentS16(5);
    *(s16*)((u8*)&g_Scene + 0x52) = -FieldScriptVMGetInstructionArgumentS16(7);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 9;
}

extern u8 D_800B219C;
extern u8 D_800B219D;
extern u8 D_800B219E;

void func_80091A78(void) {
    D_800B219C = FieldScriptVMGetArgument(1);
    D_800B219D = FieldScriptVMGetArgument(3);
    D_800B219E = FieldScriptVMGetArgument(5);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 7;
}

void func_80091AD4(void) {
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc11", func_80091ADC);

INCLUDE_ASM("asm/field/nonmatchings/main/misc11", func_80091BBC);

INCLUDE_ASM("asm/field/nonmatchings/main/misc11", func_80091E00);

INCLUDE_ASM("asm/field/nonmatchings/main/misc11", func_80091E98);

/* ---- func_80091F84: VM opcode 0xDB — write clamped value into the actor's
 * animation dispatch table.
 * asm 80091F84-80092040: arg(1) = table index, arg(3) = value, clamped to
 * 0xFFF (slti 0x1000 / ori 0xFFF). If g_FieldActors[D_800AFD1C].status has
 * bit 0x2000, stores it at ((s32*)actorData->unk118)[index] — the 0x80-byte
 * table func_80080A74 allocates under the same status gate. IP += 5
 * unconditionally (sh at 8009202C). */
void func_80091F84(void) {
    s32 index = FieldScriptVMGetArgument(1);
    s32 value = FieldScriptVMGetArgument(3);

    if (value >= 0x1000) {
        value = 0xFFF;
    }
    if (g_FieldActors[D_800AFD1C].status & 0x2000) {
        ((s32*)(uintptr_t)g_FieldScriptVMCurActor->unk118)[index] = value;
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc11", func_80092044);

extern s16 D_800AFEA8;
extern void GfxLineScrollUpdate(void* pLineScroll);

void func_800920D8(void) {
    s32 i;
    u32* entries;

    if (D_800AFEA8 <= 0) {
        return;
    }

    entries = (u32*)((u8*)&D_800AFEA8 + 4);
    for (i = 0; i < D_800AFEA8; i++) {
        GfxLineScrollUpdate((void*)(uintptr_t)entries[i]);
    }
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc11", func_80092148);

INCLUDE_ASM("asm/field/nonmatchings/main/misc11", func_800921E8);

void func_800923E4(void) {
    g_FieldScriptVMCurActor->scriptInstructionPointer++;
}

void func_80092404(void) {
    g_FieldScriptVMCurActor->scriptInstructionPointer++;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc11", func_80092424);

extern u32* D_800AFB20;

void func_800924D4(s32 index, s32 component, s32 value) {
    u32* entry;
    u32 mask;

    switch (component) {
        case 0:
            entry = (u32*)((u8*)D_800AFB20 + index * 4);
            *entry = (*entry & ~0xFFu) | (value & 0xFF);
            break;
        case 1:
            entry = &D_800AFB20[index];
            mask = 0xFFFF00FF;
            *entry = (*entry & mask) | ((value & 0xFF) << 8);
            break;
        case 2:
            entry = &D_800AFB20[index];
            mask = 0xFF00FFFF;
            *entry = (*entry & mask) | ((value & 0xFF) << 16);
            break;
        case 3:
            entry = &D_800AFB20[index];
            mask = 0x00FFFFFF;
            *entry = (*entry & mask) | ((value & 0xFF) << 24);
            break;
    }
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc11", func_800925A0);

void FieldScriptVMHandlerSetControllerBtnMask(void) {
    g_FieldControl.controllerBtnMask = FieldScriptVMGetInstructionArgument(1);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void func_80092664(void) {
    func_800924D4(SCRIPT_READ_U8_REL(1), SCRIPT_READ_U8_REL(2), FieldScriptVMGetArgument(3));
    g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc11", func_800926C8);

INCLUDE_ASM("asm/field/nonmatchings/main/misc11", func_80092768);

void func_80092808(void) {
    u8* actorData = (u8*)g_FieldScriptVMCurActor;
    s16 angle = *(s16*)((u8*)D_800B06B8 + 0x52);
    s32 sinTerm = rsin(angle) * 9;
    s32 cosTerm;
    s32 zMove;

    *(s16*)(actorData + 0x60) = (s16)(u16)((u32)sinTerm >> 10);

    cosTerm = rcos(angle) * 9;
    zMove = -(cosTerm << 2);
    *(s16*)(actorData + 0x64) = (s16)(zMove >> 12);

    *(u32*)(actorData + 0x4) |= 0x800;
    *(u16*)(actorData + 0xCC) += 1;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc11", func_80092894);

INCLUDE_ASM("asm/field/nonmatchings/main/misc11", func_80092C20);

INCLUDE_ASM("asm/field/nonmatchings/main/misc11", func_80092DFC);

INCLUDE_ASM("asm/field/nonmatchings/main/misc11", func_80092EA0);

extern s32 g_GameSceneMapNum;

void func_80092F44(void) {
    FieldScriptMemoryWriteU16(4, g_GameSceneMapNum & 0x3FFF);
    FieldScriptMemoryWriteU16(6, FieldGetPlayerActorDirection() & 0xFFFF);
    FieldScriptMemoryWriteU16(8, FieldGetCameraDirection() & 0xFFFF);
    FieldScriptMemoryWriteU16(0x12, (s16)(FieldScriptVMGetVariableValue(0x12) + 1));
}

extern s32 D_800ADBD8;
extern s32 D_800B0064;

void func_80092FB4(void) {
    if (D_800ADBD8 != 0) {
        g_FieldControl.controllerBtnMask = -1;
        D_800ADBD8 = 0;
        D_800B0064 = FieldScriptVMGetArgument(1);
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc11", func_80093014);

void func_800931F8(void) {
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc11", func_80093200);

INCLUDE_ASM("asm/field/nonmatchings/main/misc11", func_800932D0);

INCLUDE_ASM("asm/field/nonmatchings/main/misc11", func_800933F8);

INCLUDE_ASM("asm/field/nonmatchings/main/misc11", func_80093568);

INCLUDE_ASM("asm/field/nonmatchings/main/misc11", func_80093664);

extern s32 D_8004F350;

void func_800936E4(void) {
    if (D_8004F350 == 0) {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 1;
    } else {
        g_FieldScriptVMCurActor->scriptInstructionPointer -= 1;
    }
    D_800B00C0 = 1;
}

extern s32 D_8004F350;
extern s8 D_80059171;
extern s32 D_800ADB64;
extern u16 D_800B236C;

void func_80093740(void) {
    D_800B00C0 = 1;
    D_800ADB64 = 0;
    D_80059171 = D_800B236C;
    D_8004F350++;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 1;
}

void func_80093790(void) {
    D_80059171 = 1;
    D_800ADB64 = 6;
    D_800B00C0 = 1;
    D_8004F350++;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 1;
}

void func_800937E0(void) {
    D_800ADB64 = 2;
    D_800B00C0 = 1;
    D_8004F350++;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 1;
}

void func_80093824(void) {
    D_80059171 = FieldScriptVMGetArgument(1);
    D_800ADB64 = 0x3;
    D_800B00C0 = 1;
    D_8004F350++;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc11", func_80093888);

INCLUDE_ASM("asm/field/nonmatchings/main/misc11", func_80093930);

void func_800939A0(void) {
    D_80059171 = FieldScriptVMGetArgument(1);
    D_800ADB64 = 0x4;
    D_800B00C0 = 1;
    D_8004F350++;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void func_80093A04(void) {
    D_80059171 = FieldScriptVMGetArgument(1);
    D_800ADB64 = 0x5;
    D_800B00C0 = 1;
    D_8004F350++;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}


// Random encounter stuff

// These are likely part of a struct
extern s8 D_800B21D0[];
extern s8 D_800B21D1[];

extern s32 D_800ADBDC;
extern s32 D_800ADBE4;
extern s32 D_800B00C0;

void func_80093A68(void) {
    g_Scene.unk48 &= 0x7FFF;
    g_FieldScriptVMCurActor->scriptInstructionPointer++;
}

void func_80093A98(void) {
    g_Scene.unk48 |= 0x8000;
    g_FieldScriptVMCurActor->scriptInstructionPointer++;
}

void func_80093AC8(void) {
    g_FieldControl.isRandomEncountersEnabled = 0;
    D_800B21D0[0] = 0;
    D_800B21D1[0] = 0;
    g_Scene.unk48 &= 0x3FFF;
    g_FieldScriptVMCurActor->scriptInstructionPointer++;
}

void func_80093B10(void) {
    g_FieldControl.isRandomEncountersEnabled = -1;
    D_800B21D0[0] = 1;
    D_800B21D1[0] = 1;
    g_Scene.unk48 |= 0xC000;
    if ((D_800ADBDC == 0) || (D_800ADBE4 == 0)) {
        D_800B00C0 = 1; // Stop script VM execution?
        g_FieldScriptVMCurActor->scriptInstructionPointer--;
        return;
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer++;
}

void func_80093BB0(void) {
    D_800B21D0[0] = 0x0;
    g_FieldScriptVMCurActor->scriptInstructionPointer++;
}

void func_80093BD4(void) {
    D_800B21D0[0] = 0x1;
    g_FieldScriptVMCurActor->scriptInstructionPointer++;
}

void FieldScriptEnableCompass(void) {
    D_800B21D1[0] = 0x0;
    g_FieldScriptVMCurActor->scriptInstructionPointer++;
}

void FieldScriptDisableCompass(void) {
    D_800B21D1[0] = 0x1;
    g_FieldScriptVMCurActor->scriptInstructionPointer++;
}

void FieldScriptVMHandlerDisableRandomEncounters(void) {
    g_FieldControl.isRandomEncountersEnabled = 0;
    g_FieldScriptVMCurActor->scriptInstructionPointer++;
}

void func_80093C6C(void) {
    if (D_800ADBDC == 0 || D_800ADBE4 == 0) {
        D_800B00C0 = 1;
        return;
    }
    g_FieldControl.isRandomEncountersEnabled = -1;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 1;
}
// End of random encounter stuff


// Write U8 Handler
// Arg1 + Arg3 = Offset to U8 value in script
// Arg2 = Write address
void func_80093CD0(void) {
    unsigned short offset;
    unsigned short arg1;

    arg1 = FieldScriptVMGetInstructionArgument(1);
    offset = arg1 + FieldScriptVMGetArgument(5);
    FieldScriptMemoryWriteU16(
        SCRIPT_IMM_ARG(2), 
        SCRIPT_READ_U8(offset)
    );
    g_FieldScriptVMCurActor->scriptInstructionPointer += 7;
}

// Write U16/S16 handler
// Arg1 + Arg3 = Offset in script to short to write
// Arg3: Write address
void func_80093D48(void) {
    int arg1;
    int nAddress;
    unsigned short nOffset;
    int nValue;
    int arg3;

    arg1 = FieldScriptVMGetInstructionArgument(1);
    nOffset = arg1 + FieldScriptVMGetArgument(5);
    if (SCRIPT_READ_U8_REL(7) == 0) {
        // W/O carry
        FieldScriptMemoryWriteU16(
            SCRIPT_IMM_ARG(2),
            SCRIPT_READ_U8(nOffset) | (SCRIPT_READ_U8(nOffset + 1) << 8)
        );
    } else {
        // With carry
        FieldScriptMemoryWriteU16(
            SCRIPT_IMM_ARG(2),
            (short)(SCRIPT_READ_U8(nOffset) + (SCRIPT_READ_U8(nOffset + 1) << 8))
        );
    }
    
    g_FieldScriptVMCurActor->scriptInstructionPointer += 8;
}

// The two functions seems to be related to handling room transitions
void func_80093E30(void) {
    if (!g_FieldScriptVMCurActor->scriptFlags.fields.scriptFlags_0x13) {
        if (!(g_FieldScriptVMCurActor->flags12C_0x5)) {
            g_FieldScriptVMCurActor->flags12C_0x5 = 1;
            g_FieldScriptVMCurActor->curDoorStep = 0;
            func_80085634(8, 3);
        } else {
            g_FieldScriptVMCurActor->curDoorStep++;
            if (g_FieldScriptVMCurActor->curDoorStep < 0x1F) {
                if (SCRIPT_READ_U8_REL(1) == 0) {
                    g_FieldActors[D_800AFD1C].rotation.y += 0x20;
                } else {
                    g_FieldActors[D_800AFD1C].rotation.y -= 0x20;
                }
            } else {
                g_FieldScriptVMCurActor->scriptFlags.fields.scriptFlags_0x13 = 0x1;
                g_FieldScriptVMCurActor->flags12C_0x5 = 0;
                g_FieldScriptVMCurActor->curDoorStep = 0;
                g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
            }
        }
    } else {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
    }
    
    func_80072254(D_800AFD1C);
}

void func_80093FC0(void) {
    if (g_FieldScriptVMCurActor->scriptFlags.fields.scriptFlags_0x13) {
        if (!(g_FieldScriptVMCurActor->flags12C_0x5)) {
            g_FieldScriptVMCurActor->flags12C_0x5 = 1;
            g_FieldScriptVMCurActor->curDoorStep = 0;
            func_80085634(8, 3);
        } else {
            g_FieldScriptVMCurActor->curDoorStep++;
            if (g_FieldScriptVMCurActor->curDoorStep < 0x1F) {
                if (SCRIPT_READ_U8_REL(1) == 0) {
                    g_FieldActors[D_800AFD1C].rotation.y -= 0x20;
                } else {
                    g_FieldActors[D_800AFD1C].rotation.y += 0x20;
                }
            } else {
                g_FieldScriptVMCurActor->scriptFlags.fields.scriptFlags_0x13 = 0;
                g_FieldScriptVMCurActor->flags12C_0x5 = 0;
                g_FieldScriptVMCurActor->curDoorStep = 0;
                g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
            }
        }
    } else {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
    }
    func_80072254(D_800AFD1C);
}

extern void func_80072254(int);
extern void func_80085634(int,int);
extern s32 D_800AFD1C;
extern FieldActor* D_800B06B8;

void func_80094158() {
    ActorData* pActor;
    int angle;
    int delta;

    if (!g_FieldScriptVMCurActor->scriptFlags.fields.scriptFlags_0x13) {
        if (!g_FieldScriptVMCurActor->flags12C_0x5) {
            g_FieldScriptVMCurActor->flags12C_0x5 = 0x1;
            g_FieldScriptVMCurActor->curDoorStep = 0;
            func_80085634(8, 3);
            g_FieldScriptVMCurActor->unkD0.vx = g_FieldScriptVMCurActor->position.vx;
            g_FieldScriptVMCurActor->unkD0.vy = g_FieldScriptVMCurActor->position.vy;
            g_FieldScriptVMCurActor->unkD0.vz = g_FieldScriptVMCurActor->position.vz;
        } else {
            g_FieldScriptVMCurActor->curDoorStep++;
            if (g_FieldScriptVMCurActor->curDoorStep < FieldScriptVMGetArgument(3)) {
                switch (FieldScriptVMGetArgument(5)) { 
                    case 0x1000:
                        g_FieldScriptVMCurActor->unkD0.vy -= FieldScriptVMGetArgument(1) * 0x10;
                        D_800B06B8->transformMatrix.t[1] = CONV_TO_GTE(g_FieldScriptVMCurActor->unkD0.vz);
                        break;
                    case 0x1001:
                        g_FieldScriptVMCurActor->unkD0.vy += FieldScriptVMGetArgument(1) * 0x10;
                        D_800B06B8->transformMatrix.t[1] = CONV_TO_GTE(g_FieldScriptVMCurActor->unkD0.vz);
                        break;
                    default:
                        delta = PSX_DEGREES(90);
                        angle = D_800B06B8->rotation.y + FieldScriptVMGetArgument(5) - delta;
                        g_FieldScriptVMCurActor->unkD0.vx += rsin(angle) * FieldScriptVMGetArgument(1);
                        g_FieldScriptVMCurActor->unkD0.vz -= rcos(angle) * FieldScriptVMGetArgument(1);
                        D_800B06B8->transformMatrix.t[0] = CONV_TO_GTE(g_FieldScriptVMCurActor->unkD0.vx);
                        D_800B06B8->transformMatrix.t[2] = CONV_TO_GTE(g_FieldScriptVMCurActor->unkD0.vz);
                        break;
                }
            } else {
                g_FieldScriptVMCurActor->scriptFlags.fields.scriptFlags_0x13 = 1;
                g_FieldScriptVMCurActor->flags12C_0x5 = 0;
                g_FieldScriptVMCurActor->curDoorStep = 0;   
                g_FieldScriptVMCurActor->scriptInstructionPointer += 7;
            }
        }
    } else {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 7;
    }
    func_80072254(D_800AFD1C);
}

void func_800943AC(void) {
    ActorData* temp_a1;
    int angle;
    int delta;
    
    if (g_FieldScriptVMCurActor->scriptFlags.fields.scriptFlags_0x13) {
        if (!g_FieldScriptVMCurActor->flags12C_0x5) {
            g_FieldScriptVMCurActor->flags12C_0x5 = 0x1;
            g_FieldScriptVMCurActor->curDoorStep = 0x0;
            func_80085634(8, 3);
        } else {
            g_FieldScriptVMCurActor->curDoorStep++;
            if (g_FieldScriptVMCurActor->curDoorStep < FieldScriptVMGetArgument(3)) {
                switch (FieldScriptVMGetArgument(5)) {
                    case 0x1000:
                        g_FieldScriptVMCurActor->unkD0.vy -= FieldScriptVMGetArgument(1) * 0x10;
                        D_800B06B8->transformMatrix.t[1] = CONV_TO_GTE(g_FieldScriptVMCurActor->unkD0.vz);
                        break;
                    case 0x1001:
                        g_FieldScriptVMCurActor->unkD0.vy += FieldScriptVMGetArgument(1) * 0x10;
                        D_800B06B8->transformMatrix.t[1] = CONV_TO_GTE(g_FieldScriptVMCurActor->unkD0.vz);
                        break;
                    default:
                        delta = PSX_DEGREES(90);
                        angle = D_800B06B8->rotation.y + FieldScriptVMGetArgument(5) - delta;
                        g_FieldScriptVMCurActor->unkD0.vx -= rsin(angle) * FieldScriptVMGetArgument(1);
                        g_FieldScriptVMCurActor->unkD0.vz += rcos(angle) * FieldScriptVMGetArgument(1);
                        D_800B06B8->transformMatrix.t[0] = CONV_TO_GTE(g_FieldScriptVMCurActor->unkD0.vx);
                        D_800B06B8->transformMatrix.t[2] = CONV_TO_GTE(g_FieldScriptVMCurActor->unkD0.vz);
                        break;
                }
            } else {
                g_FieldScriptVMCurActor->scriptFlags.fields.scriptFlags_0x13 = 0x0;
                g_FieldScriptVMCurActor->flags12C_0x5 = 0x0;
                g_FieldScriptVMCurActor->curDoorStep = 0;
                g_FieldScriptVMCurActor->scriptInstructionPointer += 7;
            }
        }
    } else {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 7;
    }
    func_80072254(D_800AFD1C);
}

extern s32 D_8004F318;
extern s32 D_8004F328;

void func_800945D4(void) {
    D_8004F318 = 0;
    D_8004F328 = 0xFF;
    FieldScriptMemoryWriteU16(
        0xA, 
        ((FieldScriptVMGetArgument(1) << 8) & 0xFF00) | (FieldScriptVMGetArgument(3) & 0xFF)
    );
    g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
}

void func_80094650(void) {
    D_8004F328 = SCRIPT_READ_U8_REL(1);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
}

void func_8009468C(void) {
    D_8004F318 = 0;
    D_8004F328 = 0xFF;
    g_FieldScriptVMCurActor->scriptInstructionPointer++;
}

void func_800946BC(void) {
    g_FieldScriptVMCurActor->flags12C_0 = 0x1;
    g_FieldScriptVMCurActor->unk70 = FieldScriptVMGetArgument(1);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void func_80094710(void) {
    g_FieldScriptVMCurActor->flags12C_0 = 0x2;
    g_FieldScriptVMCurActor->unk70 = FieldScriptVMGetArgument(1);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void func_80094764(void) {
    g_FieldScriptVMCurActor->flags12C_0 = 0x3;
    g_FieldScriptVMCurActor->unk70 = FieldScriptVMGetArgument(1);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

extern void func_80072254(int);

void func_800947B0(void) {
    FieldActor* pActor;

    if (FieldScriptVMGetActorIndex(2) != ACTOR_ID_INVALID) {
        pActor = &g_FieldActors[FieldScriptVMGetActorIndex(2)];
        switch (SCRIPT_READ_U8_REL(1)) {
            case 0:
                pActor->rotation.x += FieldScriptVMGetArgument(3);
                break;
            case 1:
                pActor->rotation.x -= FieldScriptVMGetArgument(3);
                break;
            case 2:
                pActor->rotation.y += FieldScriptVMGetArgument(3);
                break;
            case 3:
                pActor->rotation.y -= FieldScriptVMGetArgument(3);
                break;
            case 4:
                pActor->rotation.z += FieldScriptVMGetArgument(3);
                break;
            case 5:
                pActor->rotation.z -= FieldScriptVMGetArgument(3);
                break;
        }
        func_80072254(FieldScriptVMGetActorIndex(2));
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
}

// Set X, Y or Z rotaiton of Actor D_800AFD1C
void func_80094918(void) {
    switch (SCRIPT_READ_U8_REL(3)) { 
        case 0:
            g_FieldActors[D_800AFD1C].rotation.x = FieldScriptVMGetArgument(1);
            break;
        case 1:
            g_FieldActors[D_800AFD1C].rotation.y = FieldScriptVMGetArgument(1);
            break;
        case 2:
            g_FieldActors[D_800AFD1C].rotation.z = FieldScriptVMGetArgument(1);
            break;
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 4;
    func_80072254(D_800AFD1C);
}

// Increase X Rotation of Actor D_800AFD1C
void func_80094A5C(void) {
    (&g_FieldActors[D_800AFD1C])->rotation.x += FieldScriptVMGetArgument(1);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
    func_80072254(D_800AFD1C);
}

// Decrease X Rotation of Actor D_800AFD1C
void func_80094ACC(void) {
    (&g_FieldActors[D_800AFD1C])->rotation.x -= FieldScriptVMGetArgument(1);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
    func_80072254(D_800AFD1C);
}

// Increase Y Rotation of Actor D_800AFD1C
void func_80094B3C(void) {
    (&g_FieldActors[D_800AFD1C])->rotation.y += FieldScriptVMGetArgument(1);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
    func_80072254(D_800AFD1C);
}

// Decrease Y Rotation of Actor D_800AFD1C
void func_80094BAC(void) {
    (&g_FieldActors[D_800AFD1C])->rotation.y -= FieldScriptVMGetArgument(1);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
    func_80072254(D_800AFD1C);
}

// Increase Z Rotation of Actor D_800AFD1C
void func_80094C1C(void) {
    (&g_FieldActors[D_800AFD1C])->rotation.z += FieldScriptVMGetArgument(1);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
    func_80072254(D_800AFD1C);
}

// Decrease Z Rotation of Actor D_800AFD1C
void func_80094C8C(void) {
    (&g_FieldActors[D_800AFD1C])->rotation.z -= FieldScriptVMGetArgument(1);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
    func_80072254(D_800AFD1C);
}

s32 func_80094CFC(void) {
    s32 i;
    for (i = 0; i < 0x96; i++) {
        if (((u8*)g_pGameState)[0x1F90 + i] == 0) {
            return i;
        }
        if (((u8*)g_pGameState)[0x2026 + i] == 0) {
            return i;
        }
    }
    return -1;
}

s32 func_80094D4C(void) {
    s32 i;
    for (i = 0; i < 0x64; i++) {
        if (((u8*)g_pGameState)[0x1D38 + i] == 0) {
            return i;
        }
        if (((u8*)g_pGameState)[0x1D9C + i] == 0) {
            return i;
        }
    }
    return -1;
}

s32 func_80094D9C(void) {
    s32 i;
    for (i = 0; i < 0xC8; i++) {
        if (((u8*)g_pGameState)[0x1E00 + i] == 0) {
            return i;
        }
        if (((u8*)g_pGameState)[0x1EC8 + i] == 0) {
            return i;
        }
    }
    return -1;
}

s32 func_80094DEC(void) {
    s32 i;
    for (i = 0; i < 0x64; i++) {
        if (((u8*)g_pGameState)[0x20BC + i] == 0) {
            return i;
        }
        if (((u8*)g_pGameState)[0x2120 + i] == 0) {
            return i;
        }
    }
    return -1;
}

s32 func_80094E3C(void) {
    s32 i;
    for (i = 0; i < 0x96; i++) {
        if (((u8*)g_pGameState)[0x2184 + i] == 0) {
            return i;
        }
        if (((u8*)g_pGameState)[0x221A + i] == 0) {
            return i;
        }
    }
    return -1;
}

s32 func_80094E8C(s32 a0) {
    s32 i;
    for (i = 0; i < 0x96; i++) {
        if (((u8*)g_pGameState)[0x2026 + i] == a0 && ((u8*)g_pGameState)[0x1F90 + i] != 0) {
            return 0;
        }
    }
    return -1;
}

s32 func_80094EDC(s32 a0) {
    s32 i;
    for (i = 0; i < 0xC8; i++) {
        if (((u8*)g_pGameState)[0x1EC8 + i] == a0 && ((u8*)g_pGameState)[0x1E00 + i] != 0) {
            return i;
        }
    }
    return -1;
}

s32 func_80094F2C(s32 a0) {
    s32 i;
    for (i = 0; i < 0x64; i++) {
        if (((u8*)g_pGameState)[0x1D9C + i] == a0 && ((u8*)g_pGameState)[0x1D38 + i] != 0) {
            return i;
        }
    }
    return -1;
}

s32 func_80094F7C(s32 a0) {
    s32 i;
    for (i = 0; i < 0x64; i++) {
        if (((u8*)g_pGameState)[0x2120 + i] == a0 && ((u8*)g_pGameState)[0x20BC + i] != 0) {
            return i;
        }
    }
    return -1;
}

s32 func_80094FCC(s32 a0) {
    s32 i;
    for (i = 0; i < 0x96; i++) {
        if (((u8*)g_pGameState)[0x221A + i] == a0 && ((u8*)g_pGameState)[0x2184 + i] != 0) {
            return i;
        }
    }
    return -1;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc11", func_8009501C);

INCLUDE_ASM("asm/field/nonmatchings/main/misc11", func_800950A0);

INCLUDE_ASM("asm/field/nonmatchings/main/misc11", func_80095124);

INCLUDE_ASM("asm/field/nonmatchings/main/misc11", func_800951B8);

void func_8009524C(void) {
    func_80095284();
    g_FieldScriptVMCurActor->scriptInstructionPointer += 1;
}

/* ---- func_80095284: VM opcode 0x5B — halt actor movement and hold ----------
 * asm 80095284-800952FC: zeroes ActorData moveModified (+0x30..0x38) and move
 * (+0x40..0x48), ORs 0x8000 into rotation.vx/.vy (+0x104/+0x106, vy stored
 * first), zeroes three words in the FieldActor pSpriteData object (+0xC,
 * +0x14, +0x18), and sets D_800B00C0 = 1 (yields the VM turn). Does NOT
 * advance scriptInstructionPointer — the opcode re-runs every turn (hold).
 * func_8009524C (opcode 0x5A) is the halt-then-advance variant. */
void func_80095284(void) {
    FieldActor* fieldActor = &g_FieldActors[D_800AFD1C];
    u8* pSprite = (u8*)(uintptr_t)fieldActor->pSpriteData;
    ActorData* actor = g_FieldScriptVMCurActor;
    u16 rot = (u16)actor->rotation.vx;

    D_800B00C0 = 1;
    actor->moveModified.vx = 0;
    actor->moveModified.vy = 0;
    actor->moveModified.vz = 0;
    actor->move.vx = 0;
    actor->move.vy = 0;
    actor->move.vz = 0;
    rot |= 0x8000;
    actor->rotation.vy = rot;
    actor->rotation.vx = rot;
    *(s32*)(pSprite + 0x0C) = 0;
    *(s32*)(pSprite + 0x14) = 0;
    *(s32*)(pSprite + 0x18) = 0;
}

void func_80095300(void) {
    g_FieldControl.unkAngle = FieldScriptVMGetArgument(1);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

// Check if an actor is within a trigger zone when considering the entire scene projected to
// 2D, top-down. If so, call a script function.
void FieldScriptHandleTriggerZone2D(void) {
    ActorData* pActor;
    long actorPosition2D;
    long triggerPos2;
    long triggerPos3;
    long triggerPos4;
    long triggerPos1;
    int triggerIndex;
    FieldTriggerZone* pTrigger;

    triggerIndex = SCRIPT_READ_U8_REL(1);
    pActor = (ActorData*)(uintptr_t)g_FieldActors[g_PlayerActorIndex].pActorData;

    // Pack Z and X position into a long for use w/ NormalClip
    actorPosition2D = (CONV_TO_GTE(pActor->position.vz) << 0x10) + CONV_TO_GTE(pActor->position.vx);
    
    // Consider trigger zone in 2D from a top-down
    triggerPos1 = (g_pFieldTriggerZones[triggerIndex].z0 << 0x10) + g_pFieldTriggerZones[triggerIndex].x0;
    triggerPos2 = (g_pFieldTriggerZones[triggerIndex].z1 << 0x10) + g_pFieldTriggerZones[triggerIndex].x1;
    triggerPos3 = (g_pFieldTriggerZones[triggerIndex].z2 << 0x10) + g_pFieldTriggerZones[triggerIndex].x2;
    triggerPos4 = (g_pFieldTriggerZones[triggerIndex].z3 << 0x10) + g_pFieldTriggerZones[triggerIndex].x3;
    
    if (NormalClip(triggerPos1, triggerPos2, actorPosition2D) >= 0 && 
        NormalClip(triggerPos2, triggerPos3, actorPosition2D) >= 0 && 
        NormalClip(triggerPos3, triggerPos4, actorPosition2D) >= 0 && 
        NormalClip(triggerPos4, triggerPos1, actorPosition2D) >= 0
    ) {
        // If we're inside the trigger zone, call script function based on argument
        if (g_FieldScriptVMCurActor->flags12C_0x6 != 0x4) {
            g_FieldScriptVMCurActor->scriptPointersStack[g_FieldScriptVMCurActor->flags12C_0x6] = g_FieldScriptVMCurActor->scriptInstructionPointer + 4;
            g_FieldScriptVMCurActor->scriptInstructionPointer = FieldScriptVMGetInstructionArgument(2);
            g_FieldScriptVMCurActor->flags12C_0x6++;;
            return;
        }
    }
    g_FieldScriptMaxInstructionCount += 1;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 4;
}

// Check if an actor is within a trigger zone, considering height of the trigger zone as well.
// If so, call a script function.
void FieldScriptHandleTriggerZone(void) {
    ActorData* pActor;
    int actorPositionY;
    long actorPosition2D;
    long triggerPos1;
    long triggerPos2;
    long triggerPos3;
    long triggerPos4;
    int triggerIndex;
    FieldTriggerZone* pTrigger;

    triggerIndex = SCRIPT_READ_U8_REL(1);
    pActor = (ActorData*)(uintptr_t)g_FieldActors[g_PlayerActorIndex].pActorData;

    actorPositionY = CONV_TO_GTE(pActor->position.vy);
    if (g_pFieldTriggerZones[triggerIndex].y0 < actorPositionY && 
        (actorPositionY - pActor->height) < g_pFieldTriggerZones[triggerIndex].y0
    ) {
        triggerPos1 = (g_pFieldTriggerZones[triggerIndex].z0 << 0x10) + g_pFieldTriggerZones[triggerIndex].x0;
        triggerPos2 = (g_pFieldTriggerZones[triggerIndex].z1 << 0x10) + g_pFieldTriggerZones[triggerIndex].x1;
        triggerPos3 = (g_pFieldTriggerZones[triggerIndex].z2 << 0x10) + g_pFieldTriggerZones[triggerIndex].x2;
        triggerPos4 = (g_pFieldTriggerZones[triggerIndex].z3 << 0x10) + g_pFieldTriggerZones[triggerIndex].x3;
        actorPosition2D = (CONV_TO_GTE(pActor->position.vz) << 0x10) + CONV_TO_GTE(pActor->position.vx);

        if (
            NormalClip(triggerPos1, triggerPos2, actorPosition2D) >= 0 && 
            NormalClip(triggerPos2, triggerPos3, actorPosition2D) >= 0 && 
            NormalClip(triggerPos3, triggerPos4, actorPosition2D) >= 0 && 
            NormalClip(triggerPos4, triggerPos1, actorPosition2D) >= 0
        ) {
            // If we're inside the trigger zone, call script function based on argument
            if (g_FieldScriptVMCurActor->flags12C_0x6 != 0x4) {
                g_FieldScriptVMCurActor->scriptPointersStack[g_FieldScriptVMCurActor->flags12C_0x6] = g_FieldScriptVMCurActor->scriptInstructionPointer + 4;
                g_FieldScriptVMCurActor->scriptInstructionPointer = FieldScriptVMGetInstructionArgument(2);
                g_FieldScriptVMCurActor->flags12C_0x6++;;
                return;
            }
        }
    }
    g_FieldScriptMaxInstructionCount += 1;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 4;
}

void FieldScriptCheckTriggerZone2D(void) {
    ActorData* pActor;
    long actorPosition2D;
    long triggerPos2;
    long triggerPos3;
    long triggerPos4;
    long triggerPos1;
    int triggerIndex;
    FieldTriggerZone* pTrigger;

    triggerIndex = SCRIPT_READ_U8_REL(1);
    pActor = (ActorData*)(uintptr_t)g_FieldActors[g_PlayerActorIndex].pActorData;

    // Pack Z and X position into a long for use w/ NormalClip
    actorPosition2D = (CONV_TO_GTE(pActor->position.vz) << 0x10) + CONV_TO_GTE(pActor->position.vx);
    
    // Consider trigger zone in 2D from a top-down
    triggerPos1 = (g_pFieldTriggerZones[triggerIndex].z0 << 0x10) + g_pFieldTriggerZones[triggerIndex].x0;
    triggerPos2 = (g_pFieldTriggerZones[triggerIndex].z1 << 0x10) + g_pFieldTriggerZones[triggerIndex].x1;
    triggerPos3 = (g_pFieldTriggerZones[triggerIndex].z2 << 0x10) + g_pFieldTriggerZones[triggerIndex].x2;
    triggerPos4 = (g_pFieldTriggerZones[triggerIndex].z3 << 0x10) + g_pFieldTriggerZones[triggerIndex].x3;
    
    if (NormalClip(triggerPos1, triggerPos2, actorPosition2D) >= 0 && 
        NormalClip(triggerPos2, triggerPos3, actorPosition2D) >= 0 && 
        NormalClip(triggerPos3, triggerPos4, actorPosition2D) >= 0 && 
        NormalClip(triggerPos4, triggerPos1, actorPosition2D) >= 0
    ) {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 4;
        return;
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer = FieldScriptVMGetInstructionArgument(2);
    g_FieldScriptMaxInstructionCount += 1;
}

void FieldScriptCheckTriggerZone(void) {
    ActorData* pActor;
    int actorPositionY;
    long actorPosition2D;
    long triggerPos1;
    long triggerPos2;
    long triggerPos3;
    long triggerPos4;
    int triggerIndex;
    FieldTriggerZone* pTrigger;

    triggerIndex = SCRIPT_READ_U8_REL(1);
    pActor = (ActorData*)(uintptr_t)g_FieldActors[g_PlayerActorIndex].pActorData;

    actorPositionY = CONV_TO_GTE(pActor->position.vy);
    if (g_pFieldTriggerZones[triggerIndex].y0 < actorPositionY && 
        (actorPositionY - pActor->height) < g_pFieldTriggerZones[triggerIndex].y0
    ) {
        triggerPos1 = (g_pFieldTriggerZones[triggerIndex].z0 << 0x10) + g_pFieldTriggerZones[triggerIndex].x0;
        triggerPos2 = (g_pFieldTriggerZones[triggerIndex].z1 << 0x10) + g_pFieldTriggerZones[triggerIndex].x1;
        triggerPos3 = (g_pFieldTriggerZones[triggerIndex].z2 << 0x10) + g_pFieldTriggerZones[triggerIndex].x2;
        triggerPos4 = (g_pFieldTriggerZones[triggerIndex].z3 << 0x10) + g_pFieldTriggerZones[triggerIndex].x3;
        actorPosition2D = (CONV_TO_GTE(pActor->position.vz) << 0x10) + CONV_TO_GTE(pActor->position.vx);

        if (
            NormalClip(triggerPos1, triggerPos2, actorPosition2D) >= 0 && 
            NormalClip(triggerPos2, triggerPos3, actorPosition2D) >= 0 && 
            NormalClip(triggerPos3, triggerPos4, actorPosition2D) >= 0 && 
            NormalClip(triggerPos4, triggerPos1, actorPosition2D) >= 0
        ) {
            g_FieldScriptVMCurActor->scriptInstructionPointer += 4;
            return;
        }
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer = FieldScriptVMGetInstructionArgument(2);
    g_FieldScriptMaxInstructionCount += 1;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc11", FieldProjectActorOriginToScreen);

extern s32 D_800ADC18;

void func_80095B3C(void) {
    int screenX;
    int screenY;

    FieldProjectActorOriginToScreen(&screenX, &screenY);
    if (D_800ADC18 != 0) {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 4;
        return;
    }
    
    if ((screenY - 33) < 159U && (screenX - 33) < 255U) {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 4;
    } else {
        g_FieldScriptVMCurActor->scriptInstructionPointer = FieldScriptVMGetInstructionArgument(2);
    }
    
    D_800B00C0 = 1;
}

void FieldScriptCheckActorOnScreen(void) {
    int screenX;
    int screenY;

    FieldProjectActorOriginToScreen(&screenX, &screenY);
    if (D_800ADC18 != 0) {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 4;
        return;
    }
    
    if ((screenY - 1) < (SCREEN_HEIGHT - 1) && (screenX - 1) < (SCREEN_WIDTH - 1)) {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 4;
    } else {
        g_FieldScriptVMCurActor->scriptInstructionPointer = FieldScriptVMGetInstructionArgument(2);
    }
    
    D_800B00C0 = 1;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc11", func_80095CC4);

INCLUDE_ASM("asm/field/nonmatchings/main/misc11", func_80095D6C);

extern FieldActor* D_800B06B8;

void FieldScriptCheckActorDistance(void) {
    ActorData* pActor;
    ActorData* pActorOther;
    FieldActor* pFieldActor;
    int distance;
    int actorIndex;

    actorIndex = FieldScriptVMGetActorIndex(1);
    if (actorIndex != 0xFF) {
        pFieldActor = &g_FieldActors[FieldScriptVMGetActorIndex(1)];
        
        pActor = (ActorData*)(uintptr_t)D_800B06B8->pActorData;
        pActorOther = (ActorData*)(uintptr_t)pFieldActor->pActorData;
        distance = FieldGetVec3Magnitude(
            CONV_TO_GTE(pActor->position.vx) - CONV_TO_GTE(pActorOther->position.vx), 
            CONV_TO_GTE(pActor->position.vy) - CONV_TO_GTE(pActorOther->position.vy), 
            CONV_TO_GTE(pActor->position.vz) - CONV_TO_GTE(pActorOther->position.vz)
        );
        
        if (distance < FieldScriptVMGetArgument(2)) {
            g_FieldScriptVMCurActor->scriptInstructionPointer += 6;
            return;
        }
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer = FieldScriptVMGetInstructionArgument(4);
}
