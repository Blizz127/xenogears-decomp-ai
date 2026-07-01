#include "common.h"
#include "main/game.h"
#include "field/main.h"
#include "field/actor.h"
#include "field/script_vm.h"
#include "field/text_box.h"

extern s32 D_8005A444;
extern s32 D_8005A448;
extern s32 D_8005A44C;
extern s32 D_800AFD1C;

void func_8009BB0C(void) {
    int nTextBoxIndex;
    u32 lowerFlags;
    u32 upperFlags;

    // Check if there's no visible textbox owned by current actor
    if (func_8009CD18(&nTextBoxIndex) == -1) {
        g_FieldScriptMaxInstructionCount += 8;
        FieldScriptMemoryWriteU16(0x14, g_FieldScriptVMCurActor->unk81);
        g_FieldScriptVMCurActor->scriptInstructionPointer += 1;
        return;
    }
    
    if (((ActorData*)(uintptr_t)g_FieldActors[g_FieldTextBoxes[nTextBoxIndex].talkingActorID].pActorData)->flags & 0x200) {
        upperFlags = g_FieldScriptVMCurActor->dialogFlags >> 0x10;
        if (g_FieldScriptVMCurActor->dialogFlags >> 0x10 == 0) {
            lowerFlags = g_FieldScriptVMCurActor->dialogFlags & 0xFFFF;
        } else {
            lowerFlags = upperFlags & 0xFFFF;
        }
        
        if (!(lowerFlags & 0x1)) {
            if (g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].flags_0x12 != 7) {
                func_800A1B70();
            }
            g_FieldTextBoxes[nTextBoxIndex].status = 0;
        }
    }
    D_800B00C0 = 1;
}

INCLUDE_ASM("asm/field/nonmatchings/dialogue/text_box", func_8009BC98);

s32 func_8009BE58(void) {
    return ((g_FieldScriptVMCurActor->direction - (FieldGetCameraDirection() & 0xffff)) & MASK_8DIR_MOVEMENT_NUM_DIRECTIONS) < 5;
}

// Reset text box?
void func_8009BE9C(void) {
    int index;
    
    if (SCRIPT_READ_U8_REL(1) == 0) {
        if (func_8009CD18(&index) == 0) {
            g_FieldTextBoxes[index].status = 0;
            g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
        } else {
            g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
        }
    } else {
        g_FieldScriptVMCurActor->dialogPixelWidth = 0;
        g_FieldScriptVMCurActor->dialogPixelHeight = 0;
        g_FieldScriptVMCurActor->dialogWidth = 0;
        g_FieldScriptVMCurActor->dialogHeight = 0;
        g_FieldScriptVMCurActor->dialogFlags = 0;
        g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
    }
    D_800B00C0 = 1;
}

// modifies the face id.. is this changing out the portrait?
void func_8009BF8C(void) {
    if (FieldScriptVMGetActorIndex(1) != ACTOR_ID_INVALID) {
        g_FieldScriptVMCurActor->faceId = ((ActorData*)(uintptr_t)g_FieldActors[FieldScriptVMGetActorIndex(1)].pActorData)->faceId;
        func_8009C01C();
        return;
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 6;
}

INCLUDE_ASM("asm/field/nonmatchings/dialogue/text_box", func_8009C01C);

void func_8009C0B4(void) {
    func_8009C5A8(D_800AFD1C, 0);
}

void func_8009C0DC(void) {
    func_8009C5A8(D_800AFD1C, 1);
}

void func_8009C104(void) {
    func_8009C5A8(D_800AFD1C, 2);
}

void func_8009C12C(void) {
    func_8009C5A8(D_800AFD1C, 3);
}

// https://decomp.me/scratch/tL6mE
INCLUDE_ASM("asm/field/nonmatchings/dialogue/text_box", func_8009C154);

// Check if there is a portrait w/ targetId that's free to use?
int func_8009C538(int targetId) {
    int i;
    
    for (i = 0; i < 4; i++) {
        if (g_FieldTextBoxes[i].visibility || g_FieldTextBoxes[i].portrait.shouldRenderPortrait != 1) {
            continue;
        }
        
        if (g_FieldTextBoxes[i].portrait.portraitID == targetId) {
            return -1;
        }
    }

    return 0;
}


INCLUDE_ASM("asm/field/nonmatchings/dialogue/text_box", func_8009C5A8);

extern u16 D_800B2174[];

void func_8009CCF8(int arg0) {
    D_800B2174[0] |= 1 << arg0;
}
// find_first_unused()?
int func_8009CD18(int* pTextBoxIndex) {
    int i;

    for (i = 0; i < 4; i++) {
        if (g_FieldTextBoxes[i].ownerActorID == D_800AFD1C && g_FieldTextBoxes[i].visibility == 0) {
            *pTextBoxIndex = i;
            return 0;
        }
    }

    return -1;
}

u32 func_8009CD7C(int bytecodeOffset) {
    u32 nActorIndex = FieldScriptVMGetActorIndex(bytecodeOffset);
    if (nActorIndex == ACTOR_ID_INVALID) {
        return D_8005A444;
    }
    return nActorIndex;
}

u32 FieldScriptVMGetActorIndex(int bytecodeOffset) {
    u32 actorID = SCRIPT_READ_U8_REL(bytecodeOffset);
    if (actorID == ACTOR_ID_INVALID) {
        actorID = D_8005A444;  
    } else if (actorID == 0xFE) {
        actorID = D_8005A448;
    } else if (actorID == 0xFD) {
        actorID = D_8005A44C;
    } else if (actorID == 0xFB) {
        actorID = D_800AFD1C;
    }
    return actorID;
}


// Set up dialog window / text box
void func_8009CE48(void) {
    g_FieldScriptVMCurActor->dialogPixelWidth = SCRIPT_READ_U8_REL(1) * 2;
    g_FieldScriptVMCurActor->dialogPixelHeight = SCRIPT_READ_U8_REL(2);
    g_FieldScriptVMCurActor->dialogWidth = SCRIPT_READ_U8_REL(3) * 3;
    g_FieldScriptVMCurActor->dialogHeight = SCRIPT_READ_U8_REL(4);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
}


// Set up dialog window / text box
void func_8009CEE0(void) {
    g_FieldScriptVMCurActor->dialogPixelWidth = FieldScriptVMGetArgument(1);
    g_FieldScriptVMCurActor->dialogPixelHeight = FieldScriptVMGetArgument(3);
    g_FieldScriptVMCurActor->dialogWidth = FieldScriptVMGetArgument(5) * 3;
    g_FieldScriptVMCurActor->dialogHeight = FieldScriptVMGetArgument(7);
    g_FieldScriptVMCurActor->dialogFlags = FieldScriptVMGetArgument(9);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 0xB;
}
