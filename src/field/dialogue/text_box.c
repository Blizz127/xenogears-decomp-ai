#include "common.h"
#include "main/game.h"
#include "field/main.h"
#include "field/actor.h"
#include "field/script_vm.h"
#include "field/text_box.h"

extern s32 D_8005A444;
extern s32 D_8005A448;
extern s32 D_8005A44C;
extern s32 D_800AFD04;
extern s32 D_800AFD1C;
extern s32 D_800ADB2C;
extern s32 D_800ADB64;
extern s32 D_800ADB70;
extern s32 D_800C4268;
extern void* D_800ADBF0;

u32 FieldScriptVMGetActorIndex(int bytecodeOffset);
s32 func_8008A558(void);
s32 func_80080720(void);
s32 func_80080760(void);
s32 func_800807B4(void);
s32 func_8009C154(s32 targetId);
void func_8007F814(s32 actorIndex, s32* screenX, s32* screenY, s32 yOffset);
void func_8007F8DC(s32 x, s32 y, s32 stringIndex, s32 textBoxIndex, s32 width, s32 height,
                  s32 ownerActorIndex, s32 talkingActorIndex, s32 mode, s32 orientationFlags,
                  s32 dialogFlags);
u8 DialogGetWidth(u16* pDialogData, int dialogIndex);
u8 DialogGetHeight(u16* pDialogData, int dialogIndex);
s32 func_80033CD0(void* arg0);
void func_80034800(void* arg0, s32 r, s32 g, s32 b);

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

void func_8009BC98(void) {
    s32 textBoxIndex;
    u8* pTextBox;
    s32 offset;
    s32 isReady;

    if (func_8009CD18(&textBoxIndex) != 0) {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
        D_800B00C0 = 1;
        return;
    }

    g_FieldScriptMaxInstructionCount += 8;
    pTextBox = (u8*)&g_FieldTextBoxes[textBoxIndex];
    isReady = func_80033CD0(pTextBox + 0x18);
    if (isReady != 1) {
        if (*(s16*)(pTextBox + 0x84) == 0 || *(u8*)(pTextBox + 0x6C) == 0) {
            D_800B00C0 = 1;
            return;
        }
    }

    *(s16*)(pTextBox + 0x37C) = 0;
    g_FieldScriptVMCurActor->unk81 = 0xFF;

    offset = textBoxIndex * 0x498;
    *(s16*)((u8*)g_FieldTextBoxes + offset + 0x37E) =
        ((u8*)g_FieldScriptVMCurScriptData)[g_FieldScriptVMCurActor->scriptInstructionPointer + 1] >> 4;
    *(s16*)((u8*)g_FieldTextBoxes + offset + 0x382) = 0;
    *(s16*)((u8*)g_FieldTextBoxes + offset + 0x380) =
        (((u8*)g_FieldScriptVMCurScriptData)[g_FieldScriptVMCurActor->scriptInstructionPointer + 1] & 0x0F) -
        *(s16*)((u8*)g_FieldTextBoxes + offset + 0x37E) + 1;
    func_80034800(pTextBox + 0x18, 0xEF, 0x1E, 0xF0);

    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
    D_800B00C0 = 1;
}

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


s32 func_8009C5A8(s32 actorIndex, s32 mode) {
    s32 existingTextBoxIndex;
    s32 textBoxIndex;
    s32 stringIndex;
    s32 width;
    s32 height;
    s32 screenX = 0xA0;
    s32 screenY = 0;
    s32 x;
    s32 y;
    s32 usedBoxCount;
    s32 usedBoxFlags;
    s32 dialogFlags;
    s32 placementType;
    s32 orientationFlags = 0;
    s32 i;
    ActorData* targetActor;

    g_FieldScriptMaxInstructionCount += 0x20;

    if (D_800ADB2C != 0 || D_800AFD04 != 0 || D_800C4268 != 0 || D_800ADB64 != 0xFF ||
        (D_800ADB70 == 0 && func_8008A558() != 0)) {
        D_800B00C0 = 1;
        return -1;
    }

    if (g_FieldScriptVMCurActor->faceId != 0xFF && func_8009C154(g_FieldScriptVMCurActor->faceId) == -1) {
        D_800B00C0 = 1;
        return -1;
    }

    D_800C4268++;
    if (func_8009CD18(&existingTextBoxIndex) != -1) {
        D_800B00C0 = 1;
        g_FieldTextBoxes[existingTextBoxIndex].status = 0;
        return -1;
    }

    g_FieldScriptMaxInstructionCount += 8;
    stringIndex = FieldScriptVMGetInstructionArgument(1) & 0xFFFF;

    if (func_80080720() != 0) {
        textBoxIndex = func_80080760();
        if (textBoxIndex != 0xFFFF) {
            g_FieldTextBoxes[textBoxIndex].status = 0;
            D_800B00C0 = 1;
            return -1;
        }
    } else {
        textBoxIndex = func_800807B4();
    }

    usedBoxCount = 0;
    usedBoxFlags = 0;
    for (i = 0; i < 4; i++) {
        if (g_FieldTextBoxes[i].visibility == 0) {
            usedBoxCount++;
            usedBoxFlags |= g_FieldTextBoxes[i].flags;
        }
    }

    width = DialogGetWidth(D_800ADBF0, stringIndex);
    height = DialogGetHeight(D_800ADBF0, stringIndex);

    if (mode == 0 || mode == 3) {
        if (g_FieldScriptVMCurActor->dialogWidth != 0) {
            width = g_FieldScriptVMCurActor->dialogWidth;
        }
        if (g_FieldScriptVMCurActor->dialogHeight != 0) {
            height = g_FieldScriptVMCurActor->dialogHeight;
        }
    }

    dialogFlags = g_FieldScriptVMCurActor->dialogFlags & 0xFFFF;
    if (SCRIPT_READ_U8_REL(3) != 0) {
        dialogFlags = (dialogFlags & 0xFF00) | SCRIPT_READ_U8_REL(3);
        g_FieldScriptVMCurActor->dialogFlags = (dialogFlags << 16) | (g_FieldScriptVMCurActor->dialogFlags & 0xFFFF);
    }

    placementType = (dialogFlags >> 4) & 3;
    if (placementType == 0) {
        s32 facingCamera;

        facingCamera = ((g_FieldScriptVMCurActor->direction - (FieldGetCameraDirection() & 0xFFFF)) &
                        MASK_8DIR_MOVEMENT_NUM_DIRECTIONS) < 5;
        if (!facingCamera) {
            if ((usedBoxFlags & 0x80) != 0) {
                g_FieldTextBoxes[textBoxIndex].flags = 0x81;
            } else if (usedBoxCount != 0) {
                g_FieldTextBoxes[textBoxIndex].flags = 0x81;
            } else {
                g_FieldTextBoxes[textBoxIndex].flags = 1;
            }
        } else if ((usedBoxFlags & 0x80) == 0) {
            g_FieldTextBoxes[textBoxIndex].flags = 0x81;
        } else {
            g_FieldTextBoxes[textBoxIndex].flags = 1;
        }
    } else if (placementType == 1) {
        g_FieldTextBoxes[textBoxIndex].flags = 1;
    } else if (placementType == 2) {
        g_FieldTextBoxes[textBoxIndex].flags = 0x81;
    } else {
        g_FieldTextBoxes[textBoxIndex].flags = 1;
    }

    if (mode == 0 || mode == 3) {
        func_8007F814(actorIndex, &screenX, &screenY, -0x40);
        if (mode == 0) {
            y = screenY - (height * 14) - 0x24;
        } else {
            screenX = 0xA0;
            y = 0x14;
        }
    } else {
        width = 0x48;
        height = 4;
        y = 0x10;
        screenX = 0xA0;
    }

    if (g_FieldTextBoxes[textBoxIndex].flags & 0x80) {
        if (mode == 0) {
            y = screenY + 0x30;
        } else if (mode != 3) {
            width = 0x48;
            height = 4;
            y = 0x94;
            screenX = 0xA0;
        }
    }

    if (mode == 0 || mode == 3) {
        if (g_FieldScriptVMCurActor->faceId != 0xFF && !(dialogFlags & 2)) {
            if (width < 0x18) {
                width = 0x18;
            }
            width += 0x11;
            height = 4;
            y = (g_FieldTextBoxes[textBoxIndex].flags & 0x80) ? 0x94 : 0x10;
        }
    }

    x = screenX - 8 - width * 2;
    if (x < 0xC) {
        x = 0xC;
    }
    if (x + 0x10 + width * 4 >= 0x135) {
        x = 0x124 - width * 4;
    }

    if (y < 0x10) {
        y = 0x10;
    }
    if (y + 8 + height * 14 >= 0xD5) {
        y = 0xCC - height * 14;
    }

    if (mode == 0 || mode == 3) {
        if (g_FieldScriptVMCurActor->dialogPixelWidth != 0) {
            x = g_FieldScriptVMCurActor->dialogPixelWidth;
        }
        if (g_FieldScriptVMCurActor->dialogPixelHeight != 0) {
            y = g_FieldScriptVMCurActor->dialogPixelHeight;
        }
        if (g_FieldScriptVMCurActor->dialogWidth != 0) {
            width = g_FieldScriptVMCurActor->dialogWidth;
        }
        if (g_FieldScriptVMCurActor->dialogHeight != 0) {
            height = g_FieldScriptVMCurActor->dialogHeight;
        }
        if (g_FieldScriptVMCurActor->faceId != 0xFF && !(dialogFlags & 2)) {
            height = 4;
        }
    }

    if (dialogFlags & 0x40) {
        g_FieldTextBoxes[textBoxIndex].flags |= 0x40;
    }

    if ((dialogFlags & 0xC) == 0) {
        targetActor = (ActorData*)(uintptr_t)g_FieldActors[actorIndex].pActorData;
        orientationFlags = (((((s16)targetActor->rotation.vy >> 9) - (FieldGetCameraDirection() & 0xFFFF) + 1) &
                             MASK_8DIR_MOVEMENT_NUM_DIRECTIONS) < 4) ? 0 : 0x400;
    } else if (dialogFlags & 4) {
        orientationFlags = 0x400;
    }

    func_8007F8DC(x, y, stringIndex, textBoxIndex, width, height, D_800AFD1C, actorIndex, mode,
                  orientationFlags, dialogFlags);
    func_8009CCF8(textBoxIndex);
    g_FieldScriptVMCurActor->rotation.vx |= 0x8000;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 4;
    return 0;
}

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
