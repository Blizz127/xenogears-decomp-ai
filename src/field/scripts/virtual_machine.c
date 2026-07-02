#include "common.h"
#include "main/game.h"
#include "field/main.h"
#include "field/actor.h"
#include "field/script_vm.h"
#include "field/effects.h"
#include "system/memory.h"
#include "system/archive.h"
#include "system/debug.h"
#include "psyq/libgpu.h"
#include "psyq/libcd.h"

extern s32 D_800AFFEC;
extern s32 D_800AFD1C; // Current actor index
extern s32 D_800B00C0;

extern void func_800379C8(char*, ...);
extern char D_8006FD44; // "STACKERR ACT=%d\n"
u_short FieldScriptGetBytecodeOffset(int scriptIndex, int routineIndex);

// Store instruction pointer + 5 on stack
void func_800A1730(void) {
    if (g_FieldScriptVMCurActor->flags12C_0x6 != SCRIPT_MAX_STACK_SIZE) {
        g_FieldScriptVMCurActor->scriptPointersStack[g_FieldScriptVMCurActor->flags12C_0x6] = g_FieldScriptVMCurActor->scriptInstructionPointer + 5;
        g_FieldScriptVMCurActor->scriptInstructionPointer = FieldScriptVMGetInstructionArgument(1);
        g_FieldScriptVMCurActor->flags12C_0x6++;
        return;
    }

    // Error
    if (g_FieldSystemMode == 0) {
        func_800379C8(&D_8006FD44, D_800AFD1C);
    }
    D_800B00C0 = 1;
}

// Store instruction pointer + 3 on stack
void func_800A17F4(void) {
    if (g_FieldScriptVMCurActor->flags12C_0x6 != SCRIPT_MAX_STACK_SIZE) {
        g_FieldScriptVMCurActor->scriptPointersStack[g_FieldScriptVMCurActor->flags12C_0x6] = g_FieldScriptVMCurActor->scriptInstructionPointer + 3;
        g_FieldScriptVMCurActor->scriptInstructionPointer = FieldScriptVMGetInstructionArgument(1);
        g_FieldScriptVMCurActor->flags12C_0x6++;
        return;
    }
    
    // Error
    if (g_FieldSystemMode == 0) {
        func_800379C8(&D_8006FD44, D_800AFD1C);
    }
    D_800B00C0 = 1;
}

// Restore instruction pointer from stack
void func_800A18B8(void) {
    // Error, invalid stack value
    if (!g_FieldScriptVMCurActor->flags12C_0x6) {
        if (g_FieldSystemMode == 0) {
            func_800379C8(&D_8006FD44, D_800AFD1C);
        }
        g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].flags_0x12 = 0xFF;
        g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].scriptId = 0xFF;
        D_800AFFEC = 1;
        D_800B00C0 = 1;
        return;
    }
    
    g_FieldScriptVMCurActor->flags12C_0x6--;
    g_FieldScriptVMCurActor->scriptInstructionPointer = g_FieldScriptVMCurActor->scriptPointersStack[g_FieldScriptVMCurActor->flags12C_0x6];
}

void func_800A19B0(void) {
    int i;
    
    for (i = 0; i < ACTOR_MAX_NUM_SCRIPTS; i++) {
        g_FieldScriptVMCurActor->scripts[i].waitTimer = 0;
        g_FieldScriptVMCurActor->scripts[i].state = SCRIPT_STATE_IDLE;
        g_FieldScriptVMCurActor->scripts[i].flags_0x12 = 0xF;
        g_FieldScriptVMCurActor->scripts[i].currentIP = 0xFFFF;
        g_FieldScriptVMCurActor->scripts[i].isInUse = 0;
        g_FieldScriptVMCurActor->scripts[i].scriptId = 0xFF;
        g_FieldScriptVMCurActor->scripts[i].flags_0 = 0xFFFF;
        g_FieldScriptVMCurActor->scripts[i].flags_0x17 = 0x0;
    }

    g_FieldScriptVMCurActor->curScriptIndex = 0;
    g_FieldScriptVMCurActor->unkCF = 0;
    g_FieldScriptVMCurActor->dialogFlags = 0;
    D_800B00C0 = 1;
    g_FieldScriptVMCurActor->flags12C_0x6 = 0;
}

// Yield / Stop, but change IP conditionally
void func_800A1A8C(void) {
    int i;

    for (i = 0; i < ACTOR_MAX_NUM_SCRIPTS; i++) {
        if (g_FieldScriptVMCurActor->scripts[i].flags_0x12 == 7) {
            g_FieldScriptVMCurActor->scripts[i].currentIP = FieldScriptGetBytecodeOffset(D_800AFD1C, 1);
        }
    }

    g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].flags_0x12 = 0xFF;
    g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].scriptId = 0xFF;
    D_800B00C0 = 1;
}

// Yield / Stop Handler
void func_800A1B70(void) {
    g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].flags_0x12 = 0xFF;
    g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].scriptId = 0xFF;
    D_800AFFEC = 1;
    D_800B00C0 = 1;
}

void FieldScriptVMHandlerConditionalJmp(void) {
    int nValue2;
    int nValue1;
    int nArgumentsType;
    int nCondCheck;
    int nCondResult;
    int nConditionType;

    nValue2 = 0;
    nValue1 = 0;
    nArgumentsType = ((u_char*)g_FieldScriptVMCurScriptData)[g_FieldScriptVMCurActor->scriptInstructionPointer + 5] & 0xF0;
    switch (nArgumentsType) {
    case 0x0:
        nValue1 = FieldScriptVMGetVariableValue(FieldScriptVMGetInstructionArgument(1) & 0xFFFF);
        nValue2 = FieldScriptVMGetVariableValue(FieldScriptVMGetInstructionArgument(3) & 0xFFFF);
        if (FieldScriptVMGetVariableSign(FieldScriptVMGetInstructionArgument(1) & 0xFFFF) != FIELD_SCRIPT_VM_VAR_SIGNED) {
            nValue2 = (u_short)nValue2;
        } else {
            nValue2 = (short)nValue2;
        }
        break;
    case 0x40:
        nValue1 = FieldScriptVMGetVariableValue(FieldScriptVMGetInstructionArgument(1) & 0xFFFF);
        nValue2 = FieldScriptVMGetInstructionArgumentS16(3);
        if (FieldScriptVMGetVariableSign(FieldScriptVMGetInstructionArgument(1) & 0xFFFF) != FIELD_SCRIPT_VM_VAR_SIGNED) {
            nValue2 = (u_short)nValue2;
        }
        break;
    case 0x80:
        nValue1 = FieldScriptVMGetInstructionArgumentS16(1);
        nValue2 = FieldScriptVMGetVariableValue(FieldScriptVMGetInstructionArgument(3) & 0xFFFF);
        if (FieldScriptVMGetVariableSign(FieldScriptVMGetInstructionArgument(3) & 0xFFFF) != FIELD_SCRIPT_VM_VAR_SIGNED) {
            nValue1 = (u_short)nValue1;
        }
        break;
    case 0xC0:
        nValue1 = FieldScriptVMGetInstructionArgumentS16(1);
        nValue2 = FieldScriptVMGetInstructionArgumentS16(3);
        break;
    }
    
    nCondResult = 0;
    nConditionType = ((u_char*)g_FieldScriptVMCurScriptData)[g_FieldScriptVMCurActor->scriptInstructionPointer + 5] & 0xF;
    switch (nConditionType) {
        case FIELD_SCRIPT_VM_COND_EQUAL:
            if (nValue1 == nValue2) nCondResult++;
            break;
        case FIELD_SCRIPT_VM_COND_NOT_EQUAL2:
            if (nValue1 != nValue2) nCondResult++;
            break;
        case FIELD_SCRIPT_VM_COND_LT:
            nCondCheck = nValue2 < nValue1;
            if (nCondCheck) nCondResult++;
            break;
        case FIELD_SCRIPT_VM_COND_LT2:
            nCondCheck = nValue1 < nValue2;
            if (nCondCheck) nCondResult++;
            break;
        case FIELD_SCRIPT_VM_COND_GTE:
            if (nValue1 >= nValue2) nCondResult++;
            break;
        case FIELD_SCRIPT_VM_COND_GTE2:
            if (nValue2 >= nValue1) nCondResult++;
            break;
        case FIELD_SCRIPT_VM_COND_AND:
            nCondCheck = nValue1 & nValue2;
            if (nCondCheck) nCondResult++;
            break;
        case FIELD_SCRIPT_VM_COND_NOT_EQUAL:
            if (nValue1 != nValue2) nCondResult++;
            break;
        case FIELD_SCRIPT_VM_COND_OR:
            nCondCheck = nValue1 | nValue2;
            if (nCondCheck) nCondResult++;
            break;
        case FIELD_SCRIPT_VM_COND_AND2:
            nCondCheck = nValue1 & nValue2;
            if (nCondCheck) nCondResult++;
            break;
        case FIELD_SCRIPT_VM_COND_NAND:
            nCondCheck = ~nValue1 & nValue2;
            if (nCondCheck) nCondResult++;
            break;
    }
    
    if (nCondResult == 1) {
        g_FieldScriptVMCurActor->scriptInstructionPointer = g_FieldScriptVMCurActor->scriptInstructionPointer + 8;
    } else {
        g_FieldScriptVMCurActor->scriptInstructionPointer = FieldScriptVMGetInstructionArgument(6);
    }
}

void FieldScriptVMHandlerJmp(void) {
    g_FieldScriptVMCurActor->scriptInstructionPointer = FieldScriptVMGetInstructionArgument(1);
}

void func_800A1E9C(void) {
    g_FieldScriptMaxInstructionCount += 0x20;
    g_FieldScriptVMCurActor->scriptInstructionPointer++;
}

void FieldScriptVMRun(int maxInstructionCount) {
    int nInstructionCount;
    u_short nInstructionPointer;
    u_char nHandlerIndex;

    D_800B00C0 = 0;
    g_FieldScriptMaxInstructionCount = maxInstructionCount;

    for (nInstructionCount = 0; nInstructionCount < g_FieldScriptMaxInstructionCount; nInstructionCount++) {
        /* TODO: delete this asm() when we hit 100% matching.
         *
         * The current working theory is that there was some debugging code injected here, most likely a break
         * instruction, and they just changed the define to an empty string for the release build. This caused
         * the loop optimization for this function to break, which is why you see a noop in the jump's delay
         * slot. From all testing done with code structure and compiler arguments, this appears to be the only
         * way to make the function match.
         */
        asm(ASM_BREAKPOINT);
        if (nInstructionCount > 0x400) {
            if (g_FieldSystemMode == SYSTEM_PC_HARDDRIVE) {
                func_800379C8(&D_8006FD84, D_800AFD1C); // Error printing
            }
            return;
        }
        nInstructionPointer = g_FieldScriptVMCurActor->scriptInstructionPointer;
        nHandlerIndex = ((u_char *)g_FieldScriptVMCurScriptData)[nInstructionPointer];
        g_FieldScriptVMHandlers[nHandlerIndex]();

        if (D_800AFFEC == 0) {
            g_FieldScriptMaxInstructionCount = 0xFFFF;
        }

        // Various exit checks
        if ((D_800ADB1C != 0 && (D_800ADBE0 == 0 ||  D_800ADBE4 == 0 ||  D_800ADBEC == 0))) {
            return;
        }

        if (D_800B00C0 == 1 && D_800AFFEC == D_800B00C0) {
            return;
        }

    }

}

INCLUDE_ASM("asm/field/nonmatchings/scripts/virtual_machine", func_800A2030);

// Changes current actor to the top of the field actor array and runs a script routine on it
extern FieldActor* D_800B06B8;
extern s32 D_800ADB1C;
void func_800A22AC(int scriptRoutineIndex) {
    int i;
    ActorData* pNewActor;

    // Change current actor to the first actor in the list
    D_800B06B8 = g_FieldActors;
    g_FieldScriptVMCurActor = (ActorData*)(uintptr_t)D_800B06B8->pActorData;
    pNewActor = HeapAlloc(0x138, 0x1);
    *pNewActor = *(ActorData*)(uintptr_t)D_800B06B8->pActorData;

    // Reset all event slots
    for (i = 0; i < ACTOR_MAX_NUM_SCRIPTS; i++) {
        g_FieldScriptVMCurActor->scripts[i].waitTimer = 0;
        g_FieldScriptVMCurActor->scripts[i].state = SCRIPT_STATE_IDLE;
        g_FieldScriptVMCurActor->scripts[i].flags_0x12 = 0xF;
        g_FieldScriptVMCurActor->scripts[i].currentIP = 0xFFFF;
        g_FieldScriptVMCurActor->scripts[i].isInUse = 0x0;
        g_FieldScriptVMCurActor->scripts[i].scriptId = 0xFF;
        g_FieldScriptVMCurActor->scripts[i].flags_0 = 0xFFFF;
        g_FieldScriptVMCurActor->scripts[i].flags_0x17 = 0x0;
    }
    
    D_800AFD1C = 0;
    D_800ADB1C = 0;
    D_800AFFEC = 0;

    // Run entry point routine
    g_FieldScriptVMCurActor->scriptInstructionPointer = FieldScriptGetBytecodeOffset(0, scriptRoutineIndex);
    FieldScriptVMRun(0xFFFF);
    D_800ADB1C = 1;

    // Running VM bytecode will likely have changed some fields, so they are copied back into the top
    // of the actor array.
    *(ActorData*)(uintptr_t)D_800B06B8->pActorData = *pNewActor;
    HeapFree(pNewActor);
}


INCLUDE_ASM("asm/field/nonmatchings/scripts/virtual_machine", func_800A2488);

INCLUDE_ASM("asm/field/nonmatchings/scripts/virtual_machine", func_800A24C4);

extern s32 g_GamePartySkinsInitialized;
extern s32 D_800ADBFC;
extern s32 D_800AFC74;
extern void func_80076AC0(s32, s32, void*, s32, s32, s32, s32);
void func_800A2714(void) {
    ActorData* pActor;
    FieldActor* pFieldActors;
    int i;
    void* pData;

    if (g_GamePartySkinsInitialized) {
        // Read animation files
        for (i = 0; i < D_800ADBFC; i++) {
            ArchiveSetIndex(0x4, 0x0);
            pActor = (ActorData*)(uintptr_t)g_FieldActors[i].pActorData;
            if (pActor->unk124 != -1) {
                g_FieldScriptVMCurActor = pActor;
                pData = HeapAlloc(ArchiveDecodeAlignedSize(pActor->unk124, pActor) + 8, 0x0);
                g_FieldScriptVMCurActor->unk120 = pData;
                ArchiveReadFileToBuffer(g_FieldScriptVMCurActor->unk124, pData, 0, CdlModeSpeed); // Read from disc into buffer
                ArchiveCdDataSync(0);
            }
        }

        // Set Special animation file
        for (i = 0; i < D_800ADBFC; i++) {
            pFieldActors = g_FieldActors;
            pActor = (ActorData*)(uintptr_t)pFieldActors[i].pActorData;
            if (pActor->unk124 != -1) {
                SpriteSetSpecialAnimFile((void*)(uintptr_t)pFieldActors[i].pSpriteData, pActor->unk120);
            }
        }
        
        func_800A3C8C();
        
        if (g_FieldEffects.distortion.isActive) {
            FieldDistortionInitialize(1);
        }
        
        FieldScriptMemoryWriteU16(0x10, 0x0);
        FieldScriptWritePartyMemberIDs();

        // Apply rotation and scale
        for (i = 0; i < D_800ADBFC; i++) {
            func_80072254(i);
        }
    }
}

void func_800A28D4(void) {
    int i;

    if (g_GamePartySkinsInitialized) {
        assert(0 && "func_800A28D4 initialized party-skin path is not implemented");
        return;
    }

    FieldScriptMemoryWriteU16(0x10, 0);
    FieldScriptWritePartyMemberIDs();

    for (i = 0; i < D_800ADBFC; i++) {
        D_800AFD1C = i;
        D_800B06B8 = &g_FieldActors[i];
        g_FieldScriptVMCurActor = (ActorData*)(uintptr_t)D_800B06B8->pActorData;
        g_FieldScriptVMCurActor->scriptInstructionPointer = FieldScriptGetBytecodeOffset(i, 2);
        if (((u8*)g_FieldScriptVMCurScriptData)[g_FieldScriptVMCurActor->scriptInstructionPointer] == 0) {
            g_FieldScriptVMCurActor->flags |= 0x4000000;
        }

        D_800AFD1C = i;
        D_800B06B8 = &g_FieldActors[i];
        g_FieldScriptVMCurActor = (ActorData*)(uintptr_t)D_800B06B8->pActorData;
        g_FieldScriptVMCurActor->scriptInstructionPointer = FieldScriptGetBytecodeOffset(i, 0);
    }

    for (i = 0; i < D_800ADBFC; i++) {
        D_800AFD1C = i;
        D_800AFC74 = 0;
        D_800AFFEC = 0;
        D_800B06B8 = &g_FieldActors[i];
        g_FieldScriptVMCurActor = (ActorData*)(uintptr_t)D_800B06B8->pActorData;
        FieldScriptVMRun(0xFFFF);

        if (D_800AFC74 == 0) {
            u8* pSpriteData = (u8*)g_FieldSpriteData;
            func_80076AC0(i, 0, pSpriteData + *(s32*)(pSpriteData + 4), 0, 0, 0x80, 0);
            g_FieldScriptVMCurActor->flags |= 0x800;
        }
    }
}

void FieldScriptVMHandlerNop(void) {
    g_FieldScriptVMCurActor->scriptInstructionPointer++;
}

// Script files contains a sections of sign bits for variables. 
// This function is a bit of a convoluted way to check if a bit in section of data is set or not.
int FieldScriptVMGetVariableSign(int index) {
    return -((g_FieldCurScriptFile->signBits[index >> 6] & (1 << ((index >> 1) & 0x1F))) != 0);
}

int FieldScriptVMGetVariableValue(int index) {
    if (!(g_FieldCurScriptFile->signBits[index >> 6] & (1 << ((index >> 1) & 0x1F)))) {
        return ((short*)&g_FieldScriptMemory)[index >> 1];
    } else {
        return ((u_short*)&g_FieldScriptMemory)[index >> 1];
    }
}

INCLUDE_ASM("asm/field/nonmatchings/scripts/virtual_machine", FieldScriptMemoryWriteU16);

// scriptIndex here refers to the index of the script, which will (always?) correspond to an entity index
// routineIndex is an index into the offset table, which points to a bytecode routine in that script.
u_short FieldScriptGetBytecodeOffset(int scriptIndex, int routineIndex) {
    int nOffset;
    u_short* pScriptData;

    pScriptData = &g_FieldCurScriptFile->metadata;
    nOffset = (scriptIndex * (SCRIPT_OFFSET_TABLE_SIZE / sizeof(u_short)) + routineIndex);
    return *(pScriptData + nOffset);
}

void FieldScriptWritePartyMemberIDs(void) {
    FieldScriptMemoryWriteU16(0x3E, g_GamePartyMembers[0]);
    FieldScriptMemoryWriteU16(0x40, g_GamePartyMembers[1]);
    FieldScriptMemoryWriteU16(0x42, g_GamePartyMembers[2]);
}

extern u16 D_8005941C;
extern u8 D_80059418, D_80059420, D_80059484, D_800594D0;
extern s32 D_8004F2F4, D_8004F318, D_8004F324, D_8004F328;
extern s32 g_GameSceneMapNum, g_PlayerActorIndex;
extern u8 D_800B02C8;
extern u16 D_800AFC6C, D_800AFE9C;
extern int FieldGetPlayerActorDirection(void);
extern int FieldGetCameraDirection(void);
extern void func_8009FEE4(s32 arg0);

void func_800A30FC(void) {
    s32 i;
    u16* src;
    u16* dst;

    *(s16*)((u8*)g_pGameState + 0x231A) = g_GameSceneMapNum;
    *(s16*)((u8*)g_pGameState + 0x2322) = D_8004F324;
    *(s16*)((u8*)g_pGameState + 0x2320) = *(u16*)((u8*)g_pGameState + 0x1932);
    *(s16*)((u8*)g_pGameState + 0x231C) = *(u16*)((u8*)g_pGameState + 0x1938) << 9;

    FieldScriptMemoryWriteU16(0x44, D_8005941C);
    FieldScriptMemoryWriteU16(0x46, D_800594D0);
    FieldScriptMemoryWriteU16(0x6, FieldGetPlayerActorDirection() & 0xFFFF);
    FieldScriptMemoryWriteU16(0x8, FieldGetCameraDirection() & 0xFFFF);
    FieldScriptMemoryWriteU16(0x24, *(s16*)((u8*)&g_Scene + 0x6C));
    FieldScriptMemoryWriteU16(0x3C, g_GameSceneMapNum);
    FieldScriptWritePartyMemberIDs();

    src = (u16*)&g_FieldScriptMemory;
    dst = (u16*)((u8*)g_pGameState + 0x1930);
    for (i = 0; i < 0x200; i++) {
        *dst++ = *src++;
    }
}

void func_800A31E8(void) {
    s32 i;

    if (D_800B02C8 == 1) {
        return;
    }

    D_800AFC6C |= D_800AFE9C;

    for (i = 0; i < 3; i++) {
        ((u8*)g_pGameState)[0x1D34 + i] = g_GamePartyMembers[i];
    }

    func_800A30FC();

    D_8004F2F4 = 0;
    D_8004F318 += 1;

    for (i = 0; i < 3; i++) {
        if (((u8*)g_pGameState)[0x22B1 + i] == 1) {
            func_8009FEE4(i);
        }
    }

    if (D_8004F318 >= 0x1F) {
        D_8004F318 = 0;
        if (!(D_8004F328 & 0x80)) {
            s32 value = FieldScriptVMGetVariableValue(0xA);
            s32 seconds = value & 0xFF;
            s32 minutes = (value >> 8) & 0xFF;

            if (!(D_8004F328 & 0x4)) {
                seconds += 1;
                if (seconds >= 0x3D) {
                    seconds = 0;
                    minutes += 1;
                }
            } else if (seconds == 0) {
                if (minutes != 0) {
                    seconds = 0x3B;
                    minutes -= 1;
                }
            } else {
                seconds -= 1;
            }
            FieldScriptMemoryWriteU16(0xA, (minutes << 8) | (seconds & 0xFF));
        }
    }

    FieldScriptMemoryWriteU16(0xC, D_80059418 | (D_80059420 << 8));
    FieldScriptMemoryWriteU16(0xE, D_80059484);
    FieldScriptMemoryWriteU16(0x1E, *(s16*)((u8*)g_FieldActors[g_PlayerActorIndex].pActorData + 0x22));
    FieldScriptMemoryWriteU16(0x20, *(s16*)((u8*)g_FieldActors[g_PlayerActorIndex].pActorData + 0x2A));
    FieldScriptMemoryWriteU16(0x22, *(s16*)((u8*)g_FieldActors[g_PlayerActorIndex].pActorData + 0x26));
}

INCLUDE_ASM("asm/field/nonmatchings/scripts/virtual_machine", func_800A3474);

INCLUDE_ASM("asm/field/nonmatchings/scripts/virtual_machine", func_800A3C8C);

INCLUDE_ASM("asm/field/nonmatchings/scripts/virtual_machine", func_800A3F4C);

void func_800A4748(void) {
    func_800A476C(0x2c0,0x100);
}

void func_800A476C(int x, int y) {
    RECT rect;

    rect.w = 0x140;
    rect.y = 0;
    rect.x = 0;
    rect.h = 0xe0;
    SetGeomScreen(0x200);
    MoveImage(&rect, x, y);
    FieldRenderSync();
}
