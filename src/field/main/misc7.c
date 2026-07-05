#include "common.h"
#include "main/game.h"
#include "system/math.h"
#include "field/main.h"
#include "field/actor.h"
#include "field/camera.h"
#include "field/script_vm.h"
#include "field/text_box.h"

extern FieldActor* D_800B06B8;
extern s32 D_800AFD1C;
extern s32 g_PlayerActorIndex;
long FieldGetVec3Magnitude(long x, long y, long z);

void func_800972F4(void) {
    D_800B00C0 = 1;
    g_FieldScriptVMCurActor->scriptInstructionPointer++;
}

void FieldScriptFadeOut(void) {
    int duration;
    
    FieldFadeInitializePrimitives(0);
    duration = FieldScriptVMGetArgument(1);
    FieldFadeToBlack(duration);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void FieldScriptFadeIn(void) {
    int duration = FieldScriptVMGetArgument(1);
    FieldFadeToWhite(duration);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void func_800973A4(void) {
    if (func_8001B484(FieldScriptVMGetInstructionArgument(2) & 0xFFFF, SCRIPT_READ_U8_REL(1)) == 0) {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 4;
    }
}

void func_80097410(void) {
    g_FieldScriptVMCurActor->scriptInstructionPointer += FieldScriptVMGetArgument(1) * 3 + 3;
}

int FieldGetPlayerActorDirection(void) {
    int halfDirection = PSX_DEGREES(22.5);
    return (PSX_ANGLE_TO_DIRECTION_8(((ActorData*)(uintptr_t)g_FieldActors[g_PlayerActorIndex].pActorData)->rotation.vy + halfDirection) + 2) 
        & MASK_8DIR_MOVEMENT_NUM_DIRECTIONS;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc7", func_8009749C);

INCLUDE_ASM("asm/field/nonmatchings/main/misc7", func_800975C0);

INCLUDE_ASM("asm/field/nonmatchings/main/misc7", func_800976A8);

INCLUDE_ASM("asm/field/nonmatchings/main/misc7", func_800977A4);

INCLUDE_ASM("asm/field/nonmatchings/main/misc7", func_80097864);

INCLUDE_ASM("asm/field/nonmatchings/main/misc7", func_80097954);

INCLUDE_ASM("asm/field/nonmatchings/main/misc7", func_800979F0);

INCLUDE_ASM("asm/field/nonmatchings/main/misc7", func_80097A50);

INCLUDE_ASM("asm/field/nonmatchings/main/misc7", func_80098038);

INCLUDE_ASM("asm/field/nonmatchings/main/misc7", func_800980FC);

INCLUDE_ASM("asm/field/nonmatchings/main/misc7", func_80098184);

INCLUDE_ASM("asm/field/nonmatchings/main/misc7", func_80098274);

INCLUDE_ASM("asm/field/nonmatchings/main/misc7", func_80098370);

INCLUDE_ASM("asm/field/nonmatchings/main/misc7", func_80098430);

INCLUDE_ASM("asm/field/nonmatchings/main/misc7", func_800984EC);

extern u8 D_8006FD1C[];

void func_800985BC(void) {
    if (g_FieldSystemMode == 0) {
        s32 v = FieldScriptVMGetVariableValue(FieldScriptVMGetInstructionArgument(1) & 0xFFFF);
        func_800379C8(D_8006FD1C, v, v);
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc7", func_8009861C);

INCLUDE_ASM("asm/field/nonmatchings/main/misc7", func_80098738);

INCLUDE_ASM("asm/field/nonmatchings/main/misc7", func_800988B8);

void func_8009899C(void) {
    FieldScriptMemoryWriteU16(SCRIPT_IMM_ARG(1), g_FieldScriptVMCurActor->rotation.vy & 0xFFF);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc7", func_800989F0);

void func_80098A7C(void) {
    FieldActor* pFieldActor;
    SpriteData* pSpriteData;
    u8 argMask;

    pFieldActor = &g_FieldActors[D_800AFD1C];
    pSpriteData = (SpriteData*)(uintptr_t)pFieldActor->pSpriteData;

    g_FieldScriptVMCurActor->scriptFlags.flags |= 0x10000;
    g_FieldScriptVMCurActor->flags |= 0x200000;

    argMask = ((u8*)g_FieldScriptVMCurScriptData)[g_FieldScriptVMCurActor->scriptInstructionPointer + 7];
    g_FieldScriptVMCurActor->position.vx = FieldScriptArgument1(1, argMask) << 16;

    argMask = ((u8*)g_FieldScriptVMCurScriptData)[g_FieldScriptVMCurActor->scriptInstructionPointer + 7];
    g_FieldScriptVMCurActor->position.vz = FieldScriptArgument2(3, argMask) << 16;

    argMask = ((u8*)g_FieldScriptVMCurScriptData)[g_FieldScriptVMCurActor->scriptInstructionPointer + 7];
    g_FieldScriptVMCurActor->position.vy = FieldScriptArgument3(5, argMask) << 16;

    pFieldActor->transformMatrix.t[0] = g_FieldScriptVMCurActor->position.vx >> 16;
    pFieldActor->transformMatrix.t[1] = g_FieldScriptVMCurActor->position.vy >> 16;
    pFieldActor->transformMatrix.t[2] = g_FieldScriptVMCurActor->position.vz >> 16;

    pSpriteData->position.x = g_FieldScriptVMCurActor->position.vx;
    pSpriteData->position.y = g_FieldScriptVMCurActor->position.vy;
    pSpriteData->position.z = g_FieldScriptVMCurActor->position.vz;

    g_FieldScriptVMCurActor->scriptInstructionPointer += 8;
}

void func_80098C00(void) {
    g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].flags_0 = 0xFFFF;
    func_80098CAC(0);
}

void func_80098C3C(void) {
    if (g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].flags_0 == 0xFFFF) {
        g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].flags_0 = FieldScriptVMGetArgument(0xB);
    }
    func_80098CAC(1);
}

extern s32 D_800AFD1C;
extern s32 D_800B00C0;

void func_80098CAC(s32 arg0) {
    IVEC3 prevPosition;
    s16 rotation;
    s32 animationId;
    s32 scriptArg2;
    s32 scriptArg1;
    s32 scriptArg3;
    s32 speed;
    SpriteData* pSprite;
    FieldActor* pFieldActor;
    ActorData *pActorData;

    pFieldActor = g_FieldActors;
    pActorData = (ActorData*)(uintptr_t)pFieldActor[D_800AFD1C].pActorData;
    (void*)(uintptr_t)pFieldActor[D_800AFD1C].pSpriteData;
    if (pActorData->flags & 0x2000) {
        speed = CONV_TO_GTE(0x8000000 / g_FieldScriptVMCurActor->moveSpeed);
    } else {
        speed = CONV_TO_GTE(0x4000000 / g_FieldScriptVMCurActor->moveSpeed);
    }
    if (speed == 0) {
        speed = 1;
    }
    
    animationId = 1;
    g_FieldScriptVMCurActor->scriptFlags.fields.scriptFlags_0xX = 1;
    if (SCRIPT_READ_U8_REL(1) == 0) {
        scriptArg1 = CONV_FROM_GTE(FieldScriptArgument1(2, SCRIPT_READ_U8_REL(0x8)));
        scriptArg2 = CONV_FROM_GTE(FieldScriptArgument2(4, SCRIPT_READ_U8_REL(0x8)));
        scriptArg3 = CONV_FROM_GTE(FieldScriptArgument3(6, SCRIPT_READ_U8_REL(0x8)));
        g_FieldScriptVMCurActor->unk102 = FieldGetVec3Magnitude(
            CONV_TO_GTE(scriptArg1 - g_FieldScriptVMCurActor->position.vx), 
            CONV_TO_GTE(scriptArg3 - g_FieldScriptVMCurActor->position.vy), 
            CONV_TO_GTE(scriptArg2 - g_FieldScriptVMCurActor->position.vz)
        ) / speed;

        if (CONV_FROM_GTE(g_FieldScriptVMCurActor->unk102) == 0) {
            g_FieldScriptVMCurActor->unk102++;
        }
        
        g_FieldScriptVMCurActor->unkD0.vx = (scriptArg1 - g_FieldScriptVMCurActor->position.vx) / g_FieldScriptVMCurActor->unk102;
        g_FieldScriptVMCurActor->unkD0.vy = (scriptArg3 - g_FieldScriptVMCurActor->position.vy) / g_FieldScriptVMCurActor->unk102;
        g_FieldScriptVMCurActor->unkD0.vz = (scriptArg2 - g_FieldScriptVMCurActor->position.vz) / g_FieldScriptVMCurActor->unk102;
        if (CONV_TO_GTE(scriptArg1) != CONV_TO_GTE(g_FieldScriptVMCurActor->position.vx) || 
            CONV_TO_GTE(scriptArg2) != CONV_TO_GTE(g_FieldScriptVMCurActor->position.vz)
        ) {
            rotation = -ratan2(
                CONV_TO_GTE(g_FieldScriptVMCurActor->unkD0.vz), 
                CONV_TO_GTE(g_FieldScriptVMCurActor->unkD0.vx)
            );
            g_FieldScriptVMCurActor->rotation.vx = rotation;
            g_FieldScriptVMCurActor->rotation.vy = rotation;
        }
        g_FieldScriptVMCurActor->scriptInstructionPointer += 9;
    } else {
        if (g_FieldScriptVMCurActor->unk102 <= 0 || g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].flags_0 == 0) {
            g_FieldScriptVMCurActor->scriptInstructionPointer -= 9;
            if (g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].flags_0 != 0) {
                prevPosition.x = g_FieldScriptVMCurActor->position.vx;
                prevPosition.y = g_FieldScriptVMCurActor->position.vy;
                prevPosition.z = g_FieldScriptVMCurActor->position.vz;
                g_FieldScriptVMCurActor->position.vx = CONV_FROM_GTE(FieldScriptArgument1(2, SCRIPT_READ_U8_REL(0x8)));
                g_FieldScriptVMCurActor->position.vz = CONV_FROM_GTE(FieldScriptArgument2(4, SCRIPT_READ_U8_REL(0x8)));
                g_FieldScriptVMCurActor->position.vy = CONV_FROM_GTE(FieldScriptArgument3(6, SCRIPT_READ_U8_REL(0x8)));
                g_FieldScriptVMCurActor->moveModified.vx = g_FieldScriptVMCurActor->position.vx - prevPosition.x;
                g_FieldScriptVMCurActor->moveModified.vy = g_FieldScriptVMCurActor->position.vy - prevPosition.y;
                g_FieldScriptVMCurActor->moveModified.vz = g_FieldScriptVMCurActor->position.vz - prevPosition.z;
            }
            animationId = g_FieldScriptVMCurActor->defaultAnimationId;
            if (arg0 == 0) {
                g_FieldScriptVMCurActor->scriptInstructionPointer += 0xB;
            } else {
                g_FieldScriptVMCurActor->scriptInstructionPointer += 0xD;
            }
            g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].flags_0 = 0xFFFF;
        } else {
            g_FieldScriptVMCurActor->position.vx += g_FieldScriptVMCurActor->unkD0.vx;
            g_FieldScriptVMCurActor->position.vz += g_FieldScriptVMCurActor->unkD0.vz;
            g_FieldScriptVMCurActor->position.vy +=  g_FieldScriptVMCurActor->unkD0.vy;
            g_FieldScriptVMCurActor->moveModified.vx = g_FieldScriptVMCurActor->unkD0.vx;
            g_FieldScriptVMCurActor->moveModified.vy = g_FieldScriptVMCurActor->unkD0.vy;
            g_FieldScriptVMCurActor->moveModified.vz = g_FieldScriptVMCurActor->unkD0.vz;
            g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].flags_0--;
            D_800B00C0 = 1;
        }
        g_FieldScriptVMCurActor->unk102--;
        g_FieldActors[D_800AFD1C].transformMatrix.t[0] = CONV_TO_GTE(g_FieldScriptVMCurActor->position.vx);
        g_FieldActors[D_800AFD1C].transformMatrix.t[1] = CONV_TO_GTE(g_FieldScriptVMCurActor->position.vy);
        g_FieldActors[D_800AFD1C].transformMatrix.t[2] = CONV_TO_GTE(g_FieldScriptVMCurActor->position.vz);
        pSprite->position.x = g_FieldScriptVMCurActor->position.vx;
        pSprite->position.y = g_FieldScriptVMCurActor->position.vy;
        pSprite->position.z = g_FieldScriptVMCurActor->position.vz;
    }
    
    if (g_FieldScriptVMCurActor->unkAnimationId != 0xFF) {
        animationId = g_FieldScriptVMCurActor->unkAnimationId;
    }
    
    if (g_FieldScriptVMCurActor->curAnimationId != animationId &&  
        !g_FieldScriptVMCurActor->scriptFlags.fields.scriptFlags_0x18
    ) {
        g_FieldScriptVMCurActor->curAnimationId = animationId;
        func_800821F4(pSprite, animationId, D_800B06B8, g_FieldScriptVMCurActor);
    }
    func_80081F80(pSprite, g_FieldScriptVMCurActor->rotation.vx, D_800B06B8);
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc7", func_80099214);

void func_80099980(void) {
    g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].flags_0x17 = 0;
    g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].flags_0 = 0xFFFF;
    if (func_80099AC0(0xFFFF) == 0) {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 0x6;
    }
}

long FieldGetVec3Magnitude(long x, long y, long z) {
    VECTOR vec;
    VECTOR vecSquared;

    vec.vx = x;
    vec.vy = y;
    vec.vz = z;

    Square0(&vec, &vecSquared);
    return SquareRoot0(vecSquared.vx + vecSquared.vy + vecSquared.vz);
}

long FieldGetVec2Magnitude(long x, long y) {
    VECTOR vec;
    VECTOR vecSquared;

    vec.vx = x;
    vec.vy = y;
    vec.vz = 0;
    Square0(&vec, &vecSquared);
    return SquareRoot0(vecSquared.vx + vecSquared.vy);
}

long FieldGetVec1Magnitude(long x) {
    VECTOR vec;
    VECTOR vecSquared;

    vec.vx = x;
    Square0(&vec, &vecSquared);
    return SquareRoot0(vecSquared.vx);
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc7", func_80099AC0);

void FieldScriptVMWriteCurCharacterID(void) {
    FieldScriptMemoryWriteU16(SCRIPT_IMM_ARG(1), g_FieldScriptVMCurActor->characterId);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc7", FieldScriptVMWritePartyLeaderCharacterID);

void FieldScriptVMHandlerGetActorDirection(void) {
    int delta = PSX_DEGREES(22.5);

    // The direction is likely offset by two due to how g_FieldAngleToDirectionLUT is set up.
    FieldScriptMemoryWriteU16(
        SCRIPT_IMM_ARG(1),
        (PSX_ANGLE_TO_DIRECTION_8(g_FieldScriptVMCurActor->rotation.vy + delta) + 2) & MASK_8DIR_MOVEMENT_NUM_DIRECTIONS
    );
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void FieldScriptVMHandlerGetActorPosition(void) {
    int nActorIndex = FieldScriptVMGetActorIndex(1);
    if (nActorIndex != ACTOR_ID_INVALID) {
        FieldScriptMemoryWriteU16(SCRIPT_IMM_ARG_ALIGNED(1), g_FieldActors[nActorIndex].childMatrix.t[0]);
        FieldScriptMemoryWriteU16(SCRIPT_IMM_ARG_ALIGNED(2), g_FieldActors[nActorIndex].childMatrix.t[2]);
        FieldScriptMemoryWriteU16(SCRIPT_IMM_ARG_ALIGNED(3), g_FieldActors[nActorIndex].childMatrix.t[1]);
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 8;
}

void func_8009A0FC(void) {
    g_FieldScriptVMCurActor->defaultAnimationId = SCRIPT_READ_U8_REL(1);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
}

void FieldScriptVMHandlerPlayAnimation(void) {
    unsigned char animationID;
    
    g_FieldScriptVMCurActor->flags &= 0xFEFFFFFF;
    animationID = *(u8*)&g_FieldScriptVMCurScriptData[g_FieldScriptVMCurActor->scriptInstructionPointer + 1];
    g_FieldScriptVMCurActor->unkAnimationId = animationID;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
}

void func_8009A174(void) {
    FieldScriptVMHandlerPlayAnimation();
    g_FieldScriptVMCurActor->flags &= 0xFFFEFFFF;
}

void func_8009A1AC(void) {
    if (g_FieldScriptVMCurActor->flags & 0x10000) {
        g_FieldScriptVMCurActor->unkAnimationId = 0xFF;
        g_FieldScriptVMCurActor->scriptInstructionPointer += 1;
    }
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc7", func_8009A1E4);

INCLUDE_ASM("asm/field/nonmatchings/main/misc7", func_8009A2A8);

INCLUDE_ASM("asm/field/nonmatchings/main/misc7", func_8009A34C);

INCLUDE_ASM("asm/field/nonmatchings/main/misc7", func_8009A420);

INCLUDE_ASM("asm/field/nonmatchings/main/misc7", func_8009A490);

int FieldGetCameraDirection(void) {
    int halfDirection = PSX_DEGREES(22.5);
    return (7 - PSX_ANGLE_TO_DIRECTION_8(g_CamInterpolation.targetAngleY - halfDirection)) 
        & MASK_8DIR_MOVEMENT_NUM_DIRECTIONS;
}

void FieldScriptWriteCameraDirection(void) {
    FieldScriptMemoryWriteU16(SCRIPT_IMM_ARG(1), FieldGetCameraDirection() & 0xFFFF);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}


void func_8009A58C(void) {
    if ((g_Scene.unk48 & SCRIPT_READ_U8_REL(1)) == 0) {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
    } else {
        D_800B00C0 = 1;
    }
}

void func_8009A5E0(void) {
    if ((g_Scene.unk48 & SCRIPT_READ_U8_REL(1)) == 0) {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
    } else {
        D_800B00C0 = 1;
    }
}

void FieldScriptSetDollySet(void) {
    g_Scene.dollySet = FieldScriptVMGetArgument(1);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void FieldScriptSetDollyStop(void) {
    g_Scene.dollyStop = FieldScriptVMGetArgument(1);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void FieldScriptCos(void) {
    int nValue;
    int angle;
    int nFactor;
    int address;

    address = SCRIPT_IMM_ARG(1);
    angle = FieldScriptArgument2(ARG(2), SCRIPT_READ_U8_REL(7));
    nFactor = FieldScriptArgument3(ARG(3),  SCRIPT_READ_U8_REL(7)); // ?
    nValue = rcos(angle) * nFactor;
    FieldScriptMemoryWriteU16(address, nValue >> 12);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 8;
}

void FieldScriptSin(void) {
    int nValue;
    int angle;
    int nFactor;
    int address;

    address = SCRIPT_IMM_ARG(1);
    angle = FieldScriptArgument2(ARG(2), SCRIPT_READ_U8_REL(7));
    nFactor = FieldScriptArgument3(ARG(3),  SCRIPT_READ_U8_REL(7));
    nValue = rsin(angle) * nFactor;
    FieldScriptMemoryWriteU16(address, nValue >> 12);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 8;
}

void FieldScriptAtan2(void) {
    int x;
    int y;
    int address;
    
    address = SCRIPT_IMM_ARG(1);
    y = FieldScriptArgument2(ARG(2), SCRIPT_READ_U8_REL(7));
    x = FieldScriptArgument3(ARG(3), SCRIPT_READ_U8_REL(7));
    
    FieldScriptMemoryWriteU16(
        address & 0xFFFF, 
        (s16) ratan2(y, x)
    );
    g_FieldScriptVMCurActor->scriptInstructionPointer += 8;
}

int FieldScriptGetCurActorDirection(void) {
    // The direction is likely offset by two due to how g_FieldAngleToDirectionLUT is set up.
    int halfDirection = PSX_DEGREES(22.5);
    return (PSX_ANGLE_TO_DIRECTION_8(g_FieldScriptVMCurActor->rotation.vy + halfDirection) + 2) & MASK_8DIR_MOVEMENT_NUM_DIRECTIONS;
}

extern s32 D_800ADB1C; // Is current actor a 2D actor (0) maybe?
void FieldSetCurrentActorRotation(int rotation) {
    short rotationValue2D;
    short rotationValue3D;

    if (D_800ADB1C == 0) {
        rotationValue3D = rotation | 0x8000;
        g_FieldScriptVMCurActor->rotation.vx = rotationValue3D;
        g_FieldScriptVMCurActor->rotation.vy = rotationValue3D;
        g_FieldScriptVMCurActor->rotation.vz = rotationValue3D;
    }
    rotationValue2D = rotation | 0x8000;
    g_FieldScriptVMCurActor->rotation.vx = rotationValue2D;
    g_FieldScriptVMCurActor->rotation.vy = rotationValue2D;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void FieldScriptSetActorRotation(int angle) {
    ActorData* pActor;
    int actorIndex;
    int rotAngle3D;
    int rotAngle2D;

    actorIndex = FieldScriptVMGetActorIndex(1);
    if (actorIndex != ACTOR_ID_INVALID) {
        pActor = (ActorData*)(uintptr_t)g_FieldActors[FieldScriptVMGetActorIndex(1)].pActorData;
        if (D_800ADB1C == 0) {
            rotAngle3D = angle | 0x8000;
            pActor->rotation.vx = rotAngle3D;
            pActor->rotation.vy = rotAngle3D;
            pActor->rotation.vz = rotAngle3D;
            
        }
        rotAngle2D = angle | 0x8000;
        pActor->rotation.vx = rotAngle2D;
        pActor->rotation.vy = rotAngle2D;
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 4;
}


INCLUDE_ASM("asm/field/nonmatchings/main/misc7", func_8009AA00);

void func_8009AB08(int rotation) {
    short rotationValue;

    rotationValue = PSX_ANGLE(rotation - g_CamInterpolation.targetAngleY) | 0x8000;
    g_FieldScriptVMCurActor->rotation.vx = rotationValue;
    g_FieldScriptVMCurActor->rotation.vy = rotationValue;

    // 3D Actor?
    if (D_800ADB1C == 0) {
        g_FieldScriptVMCurActor->rotation.vz = rotationValue;
    }
    
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}


extern u16 g_FieldAngleToDirectionLUT[
    // PSX_DEGREES(270) | 0x8000,
    // PSX_DEGREES(315) | 0x8000,
    // PSX_DEGREES(0) | 0x8000,
    // PSX_DEGREES(45) | 0x8000,
    // PSX_DEGREES(90) | 0x8000,
    // PSX_DEGREES(135) | 0x8000,
    // PSX_DEGREES(180) | 0x8000,
    // PSX_DEGREES(225) | 0x8000,
];

// Rotate current actor N turns clockwise, where 1 turn is 45 degrees
void FieldScriptRotateActorClockwise(void) {
    int numTurns = FieldScriptVMGetArgument(1);
    FieldSetCurrentActorRotation(g_FieldAngleToDirectionLUT[
        numTurns + FieldScriptGetCurActorDirection() & MASK_8DIR_MOVEMENT_NUM_DIRECTIONS
    ]);
}

// Rotate current actor N turns counter-clockwise, where 1 turn is 45 degrees
void FieldScriptRotateActorCounterClockwise(void) {
    int numTurns = FieldScriptVMGetArgument(1);
    FieldSetCurrentActorRotation(g_FieldAngleToDirectionLUT[
        FieldScriptGetCurActorDirection() - numTurns & MASK_8DIR_MOVEMENT_NUM_DIRECTIONS
    ]);
}

void FieldScriptSetActorDirection(void) {
    int directionIndex = FieldScriptVMGetArgument(2);
    FieldScriptSetActorRotation(g_FieldAngleToDirectionLUT[directionIndex]);
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc7", func_8009AC34);

void FieldScriptVMHandlerSetCurActorRotation(void) {
    int directionIndex = FieldScriptVMGetArgument(1);
    FieldSetCurrentActorRotation(g_FieldAngleToDirectionLUT[directionIndex]);
}

void func_8009ACB4(void) {
    int directionIndex = FieldScriptVMGetArgument(1);
    func_8009AB08(g_FieldAngleToDirectionLUT[directionIndex]);
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc7", func_8009ACEC);

extern u16 D_800AEA54[];

void func_8009AD6C(void) {
    int directionIndex = SCRIPT_READ_U8_REL(1);
    short rotation = D_800AEA54[directionIndex] | 0x8000;

    g_FieldScriptVMCurActor->rotation.vx = rotation;
    g_FieldScriptVMCurActor->rotation.vy = rotation;
    if (D_800ADB1C == 0) {
        g_FieldScriptVMCurActor->rotation.vz = rotation;
    }

    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
}

void func_8009ADDC(void) {
    g_Scene.unk48 |= 0x4000;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 1;
}

void func_8009AE0C(void) {
    g_Scene.unk48 &= 0xBFFF;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 1;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc7", func_8009AE3C);

INCLUDE_ASM("asm/field/nonmatchings/main/misc7", func_8009AEE0);

INCLUDE_ASM("asm/field/nonmatchings/main/misc7", func_8009B15C);

INCLUDE_ASM("asm/field/nonmatchings/main/misc7", func_8009B184);

INCLUDE_ASM("asm/field/nonmatchings/main/misc7", func_8009B210);

extern s32 D_800B2360;
extern s32 D_800B2364;
extern s32 D_800B2368;
extern u8 D_800B21CE;

void func_8009B338(void) {
    s32 i;
    D_800B2368 = 0;
    D_800B2364 = 0;
    D_800B2360 = 0;
    D_800B21CE = 0;
    for (i = 0; i < 0x20; i++) {
        func_80081C54(g_PlayerActorIndex);
    }
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc7", func_8009B398);

void func_8009B664(void) {
    FieldScriptMemoryWriteU16(SCRIPT_IMM_ARG(1), g_Scene.sceneScrZ);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void func_8009B6AC(void) {
    func_8009AE3C(FieldScriptVMGetArgument(1), FieldScriptVMGetArgument(3));
    g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc7", func_8009B708);

INCLUDE_ASM("asm/field/nonmatchings/main/misc7", func_8009B7A8);

void func_8009B824(void) {
    if (*(s16*)((u8*)&g_Scene + 0x66) == 0) {
        func_8009B7A8(0, FieldScriptVMGetArgument(1));
        g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
    }
    D_800B00C0 = 1;
}

void func_8009B884(void) {
    if (*(s16*)((u8*)&g_Scene + 0x66) == 0) {
        func_8009B7A8(0, FieldScriptVMGetArgument(1));
        g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
    }
    D_800B00C0 = 1;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc7", func_8009B8E4);

void func_8009B9A0(void) {
    if (*(s16*)((u8*)&g_Scene + 0x66) == 0) {
        *(s16*)((u8*)&g_Scene + 0xB8) = FieldGetCameraDirection();
        *(s32*)((u8*)&g_Scene + 0xBC) = g_Scene.sceneScrZ;
        *(u16*)((u8*)&g_Scene + 0xC0) = g_Scene.sceneDIP;
        g_FieldScriptVMCurActor->scriptInstructionPointer += 1;
    }
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc7", func_8009BA0C);

INCLUDE_ASM("asm/field/nonmatchings/main/misc7", func_8009BA7C);
