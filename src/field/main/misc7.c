#include "common.h"
#ifdef XENO_PC_PORT
#include <stdio.h>
#include <stdlib.h>
#endif
#include "main/game.h"
#include "system/math.h"
#include "field/main.h"
#include "field/actor.h"
#include "field/camera.h"
#include "field/script_vm.h"
#include "field/text_box.h"
/* Typed array views of g_Scene: retail schedules stores into it like stores
 * to an array-typed extern, so raw-offset accesses go through these. */
extern s16 g_Scene_s16[] asm("g_Scene");
extern s32 g_Scene_s32[] asm("g_Scene");
extern u16 g_Scene_u16[] asm("g_Scene");

extern FieldActor* D_800B06B8;
extern s32 D_800AFD1C;
extern s32 g_PlayerActorIndex;
extern s32 D_800B21D8[];
extern s16 D_800AFB54;
extern s16 func_8007B1C4(s16 x, s16 z, s32 walkmeshId, s16* out, s32* state);
extern int FieldScriptArgument1(int index, int mask);
extern int FieldScriptArgument2(int index, int mask);
extern int FieldScriptArgument3(int index, int mask);
extern int FieldScriptArgument4(int index, int mask);
extern u32 FieldScriptVMGetActorIndex(int bytecodeOffset);
long FieldGetVec3Magnitude(long x, long y, long z);
long FieldGetVec2Magnitude(long x, long y);
long FieldGetVec1Magnitude(long x);
s32 func_80099AC0(s32 useStoredAngle);
s32 func_80097A50(s32 targetValue);
extern s32 func_8007B694(s32* arg0);
extern long VectorNormal(VECTOR* v0, VECTOR* v1);
extern int FieldScriptVMGetArgument(int index);

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

void func_8009749C(void) {
    ActorData* actor = g_FieldScriptVMCurActor;
    ActorData* target;

    actor->scripts[actor->curScriptIndex].flags_0x17 = 2;

    target = (ActorData*)(uintptr_t)g_FieldActors[SCRIPT_READ_U8_REL(1)].pActorData;
    actor->unkD0.vx = target->position.vx >> 16;
    actor->unkD0.vz = target->position.vz >> 16;
    actor->unkD0.vy = target->position.vy >> 16;

    if (g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].flags_0 == 0xFFFF) {
        g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].flags_0 = FieldScriptVMGetArgument(5);
    }

    if (func_80097A50(FieldScriptVMGetArgument(5)) == 0) {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 7;
    }
}

/* VM opcode 0x55: set the current script's movement target to another actor.
 * The target actor index is the instruction byte at +1; its integer position
 * becomes the current actor's movement target before func_80097A50 advances
 * the interpolation. */
void func_800975C0(void) {
    ActorData* actor = g_FieldScriptVMCurActor;
    ActorData* target;

    actor->scripts[actor->curScriptIndex].flags_0x17 = 2;

    target = (ActorData*)(uintptr_t)g_FieldActors[SCRIPT_READ_U8_REL(1)].pActorData;
    actor->unkD0.vx = target->position.vx >> 16;
    actor->unkD0.vz = target->position.vz >> 16;
    actor->unkD0.vy = target->position.vy >> 16;

    {
        ActorData* pActor = g_FieldScriptVMCurActor;
        s32 result;

        pActor->scripts[pActor->curScriptIndex].flags_0 = 0xFFFF;
        result = func_80097A50(0xFFFF);
        if (result == 0) {
            g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
        }
    }
}

/* Movement-mode variant: capture the current actor position once, retain the
 * script-supplied duration in its slot, then let func_80097A50 advance it. */
void func_800976A8(void) {
    if (g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].flags_0x17 == 0) {
        g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].flags_0x17 = 1;
        g_FieldScriptVMCurActor->unkD0.vx = g_FieldScriptVMCurActor->position.vx >> 16;
        g_FieldScriptVMCurActor->unkD0.vy = g_FieldScriptVMCurActor->position.vy >> 16;
        g_FieldScriptVMCurActor->unkD0.vz = g_FieldScriptVMCurActor->position.vz >> 16;
    }

    if (g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].flags_0 == 0xFFFF) {
        g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].flags_0 = FieldScriptVMGetArgument(8);
    }

    if (func_80097A50(FieldScriptVMGetArgument(8)) == 0) {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 10;
    }
}

/* Capture current integer position into unkD0 once, force duration 0xFFFF,
 * then wait via func_80097A50 (mode flags_0x17 = 1). */
void func_800977A4(void) {
    ActorData* actor = g_FieldScriptVMCurActor;

    if (actor->scripts[actor->curScriptIndex].flags_0x17 == 0) {
        actor->scripts[actor->curScriptIndex].flags_0x17 = 1;
        actor->unkD0.vx = actor->position.vx >> 16;
        actor->unkD0.vy = actor->position.vy >> 16;
        actor->unkD0.vz = actor->position.vz >> 16;
    }

    {
        ActorData* pActor = g_FieldScriptVMCurActor;
        s32 result;

        pActor->scripts[pActor->curScriptIndex].flags_0 = 0xFFFF;
        result = func_80097A50(0xFFFF);
        if (result == 0) {
            g_FieldScriptVMCurActor->scriptInstructionPointer += 8;
        }
    }
}

/* Map001-reachable walk wait (opcode 0x46): mode flags_0x17 = 3, duration from
 * arg(5), IP += 8 when func_80097A50 reports done. */
void func_80097864(void) {
    ActorData* actor = g_FieldScriptVMCurActor;

    if (actor->scripts[actor->curScriptIndex].flags_0x17 == 0) {
        actor->scripts[actor->curScriptIndex].flags_0x17 = 3;
        actor->unkD0.vx = actor->position.vx >> 16;
        actor->unkD0.vy = actor->position.vy >> 16;
        actor->unkD0.vz = actor->position.vz >> 16;
    }

    if (g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].flags_0 == 0xFFFF) {
        g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].flags_0 =
            FieldScriptVMGetArgument(5);
    }

    if (func_80097A50(FieldScriptVMGetArgument(5)) == 0) {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 8;
    }
}

/* Duration-from-arg(8) wait wrapper; IP += 0xA on completion. */
void func_80097954(void) {
    if (g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].flags_0 == 0xFFFF) {
        g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].flags_0 = FieldScriptVMGetArgument(8);
    }

    if (func_80097A50(FieldScriptVMGetArgument(8)) == 0) {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 0xA;
    }
}

void func_800979F0(void) {
    ActorData* pActor = g_FieldScriptVMCurActor;
    s32 result;
    pActor->scripts[pActor->curScriptIndex].flags_0 = 0xFFFF;
    result = func_80097A50(0xFFFF);
    if (result == 0) {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 8;
    }
}

/* Transcribed from asm/field/nonmatchings/main/misc7/func_80097A50.s
 * (0x80097A50-0x80098034). Mode is bits 23-24 of the flags word at
 * actor+0x90+curScriptIndex*8 (not actor->scripts[] at the 0x90 label).
 * Countdown is the low half of that word. move.vy is stored then forced 0. */
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc7", func_80097A50);
#else
s32 func_80097A50(s32 targetValue) {
    FieldActor* refFieldActor;
    ActorData* refActorData;
    u8* refSpriteData;
    ActorData* actor;
    u8* actorBytes;
    u32* pFlagsWord;
    s32 mode;
    u16 countdown;
    u16 speed;
    s32 targetX;
    s32 targetY;
    s32 targetZ;
    s32 combinedSolidRange;
    s32 selfX;
    s32 selfY;
    s32 selfZ;
    s32 stepMagnitude;
    s32 distance;
    s32 hasMove;
    s32 speedShifted;
    s32 finalAngle;
    VECTOR fromTarget;
    VECTOR normal;
    VECTOR moveVec;

#ifdef NPC_EVENT_MUTANT_ALWAYS_DONE
    return 0;
#endif
#ifdef NPC_EVENT_MUTANT_NO_IP
    D_800B00C0 = 1;
    return -1;
#endif

    refFieldActor = (FieldActor*)((u8*)g_FieldActors + D_800AFD1C * 0x5C);
    refActorData = (ActorData*)(uintptr_t)refFieldActor->pActorData;
    refSpriteData = (u8*)(uintptr_t)refFieldActor->pSpriteData;
    actor = g_FieldScriptVMCurActor;
    actorBytes = (u8*)actor;
    targetX = 0;
    targetY = 0;
    targetZ = 0;
    combinedSolidRange = 0;
    hasMove = -1;

    speed = *(u16*)(actorBytes + 0x76);
    if (refActorData->flags & 0x2000) {
        *(s32*)(refSpriteData + 0x18) = 0x08000000 / speed;
    } else {
        *(s32*)(refSpriteData + 0x18) = 0x04000000 / speed;
    }
    stepMagnitude = FieldGetVec1Magnitude(*(s32*)(refSpriteData + 0x18) >> 15) + 1;

    pFlagsWord = (u32*)(actorBytes + 0x90 + actorBytes[0xCE] * 8);
    mode = (s32)((*pFlagsWord >> 23) & 3);
    switch (mode) {
    case 0:
        targetX = FieldScriptArgument1(1, SCRIPT_READ_U8_REL(5));
        targetZ = FieldScriptArgument2(3, SCRIPT_READ_U8_REL(5));
        targetY = FieldScriptArgument3(6, SCRIPT_READ_U8_REL(5));
        break;

    case 1:
        targetX = FieldScriptArgument1(1, SCRIPT_READ_U8_REL(5)) + actor->unkD0.vx;
        targetZ = FieldScriptArgument2(3, SCRIPT_READ_U8_REL(5)) + actor->unkD0.vz;
        targetY = FieldScriptArgument3(6, SCRIPT_READ_U8_REL(5)) + actor->unkD0.vy;
        break;

    case 2: {
        u32 otherIdx = FieldScriptVMGetActorIndex(1);
        ActorData* other;

        if (otherIdx == 0xFF) {
            return 0;
        }
        otherIdx = FieldScriptVMGetActorIndex(1);
        other = (ActorData*)(uintptr_t)((FieldActor*)((u8*)g_FieldActors + otherIdx * 0x5C))->pActorData;
        combinedSolidRange = FieldGetVec1Magnitude(other->solidRange + actor->solidRange);
        targetX = actor->unkD0.vx;
        targetZ = actor->unkD0.vz;
        targetY = FieldScriptArgument1(2, SCRIPT_READ_U8_REL(4));
        break;
    }

    case 3: {
        s32 angle = FieldScriptVMGetArgument(1) & 0xFFF;

        targetX = (actor->unkD0.vx + (rsin(angle) << 5)) >> 12;
        targetZ = actor->unkD0.vz + ((-(rcos(angle) << 5)) >> 12);
        targetY = actor->unkD0.vy + FieldScriptArgument1(3, SCRIPT_READ_U8_REL(7));
        break;
    }

    default:
        break;
    }

    selfX = *(s16*)(actorBytes + 0x22);
    selfY = *(s16*)(actorBytes + 0x26);
    selfZ = *(s16*)(actorBytes + 0x2A);

    fromTarget.vx = selfX - targetX;
    fromTarget.vy = selfY - targetY;
    fromTarget.vz = selfZ - targetZ;
    VectorNormal(&fromTarget, &normal);

    speedShifted = *(s32*)(refSpriteData + 0x18) >> 8;
    moveVec.vx = -((normal.vx * speedShifted) >> 4);
    moveVec.vy = -((normal.vy * speedShifted) >> 4);
    moveVec.vz = -((normal.vz * speedShifted) >> 4);

    actor->move.vx = moveVec.vx;
    actor->move.vy = moveVec.vy;
#ifndef FIELD_97A50_MUTANT_KEEP_MOVE_Y
    actor->move.vy = 0;
#endif
    actor->move.vz = moveVec.vz;

    distance = FieldGetVec3Magnitude(targetX - selfX, targetY - selfY, targetZ - selfZ);

    if (*(s16*)(actorBytes + 0x42) == 0 && *(s16*)(actorBytes + 0x4A) == 0) {
        hasMove = 0;
    }

    actor->scriptFlags.flags |= 0x400000;

    pFlagsWord = (u32*)(actorBytes + 0x90 + actorBytes[0xCE] * 8);
    countdown = *(u16*)pFlagsWord;
    if (countdown != 0 && stepMagnitude + combinedSolidRange < distance) {
        if (hasMove == -1) {
            finalAngle = (s16)func_8007B694((s32*)&moveVec);
            finalAngle |= 0x8000;
            actor->rotation.vx = finalAngle;
            actor->rotation.vy = finalAngle;
        }
        actor->unkEC = (s16)((actor->position.vy + moveVec.vy) >> 16);
        actor->scriptFlags.flags |= 0x40000;
        D_800B00C0 = 1;
        *(u16*)pFlagsWord = (u16)(countdown - 1);
        return -1;
    }

    if (hasMove == -1) {
        if (targetValue != 0) {
            if ((actor->scriptFlags.flags & 0x8000) == 0) {
                finalAngle = actor->rotation.vy | 0x8000;
            } else {
                finalAngle = actor->unk11C | 0x8000;
            }
        } else {
            finalAngle = (s16)func_8007B694((s32*)&moveVec);
            finalAngle |= 0x8000;
        }
        actor->rotation.vx = finalAngle;
        actor->rotation.vy = finalAngle;
    }

    actor->unkEC = (s16)((actor->position.vy + moveVec.vy) >> 16);
    pFlagsWord = (u32*)(actorBytes + 0x90 + actorBytes[0xCE] * 8);
    *pFlagsWord &= 0xFE7FFFFFu;
    *(u16*)pFlagsWord = 0xFFFF;
    return 0;
}
#endif /* XENO_PC_PORT */

/* asm 80098038-800980F8, opcode 0x53. Unlike 0x52, the caller supplies the
 * countdown seed and the useStoredAngle flag as an explicit script argument
 * rather than the fixed 0xFFFF sentinel. */
void func_80098038(void) {
    g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].flags_0x17 = 2;

    if (g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].flags_0 == 0xFFFF) {
        g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].flags_0 = FieldScriptVMGetArgument(2);
    }

    if (func_80099AC0(FieldScriptVMGetArgument(2)) == 0) {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 4;
    }
}

/* asm 800980FC-80098180, opcode 0x52. Thin wrapper: always (re)start the
 * per-script movement countdown at the 0xFFFF sentinel and wait via
 * func_80099AC0(0xFFFF) -- do not implement against the stub, which returns
 * 0 unconditionally and would fake instant movement completion. */
void func_800980FC(void) {
    ActorData* pActor = g_FieldScriptVMCurActor;
    s32 result;

    pActor->scripts[pActor->curScriptIndex].flags_0x17 = 2;
    pActor->scripts[pActor->curScriptIndex].flags_0 = 0xFFFF;
    result = func_80099AC0(0xFFFF);
    if (result == 0) {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
    }
}

void func_80098184(void) {
    ActorData* actor = g_FieldScriptVMCurActor;

    if (actor->scripts[actor->curScriptIndex].flags_0x17 == 0) {
        actor->scripts[actor->curScriptIndex].flags_0x17 = 3;
        actor->unkD0.vx = actor->position.vx >> 16;
        actor->unkD0.vy = actor->position.vy >> 16;
        actor->unkD0.vz = actor->position.vz >> 16;
    }

    if (g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].flags_0 == 0xFFFF) {
        g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].flags_0 = FieldScriptVMGetArgument(3);
    }

    if (func_80099AC0(FieldScriptVMGetArgument(3)) == 0) {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
    }
}

void func_80098274(void) {
    ActorData* actor = g_FieldScriptVMCurActor;

    if (actor->scripts[actor->curScriptIndex].flags_0x17 == 0) {
        actor->scripts[actor->curScriptIndex].flags_0x17 = 1;
        actor->unkD0.vx = actor->position.vx >> 16;
        actor->unkD0.vy = actor->position.vy >> 16;
        actor->unkD0.vz = actor->position.vz >> 16;
    }

    if (g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].flags_0 == 0xFFFF) {
        g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].flags_0 = FieldScriptVMGetArgument(6);
    }

    if (func_80099AC0(FieldScriptVMGetArgument(6)) == 0) {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 8;
    }
}

void func_80098370(void) {
    ActorData* actor = g_FieldScriptVMCurActor;

    if (actor->scripts[actor->curScriptIndex].flags_0x17 == 0) {
        actor->scripts[actor->curScriptIndex].flags_0x17 = 1;
        actor->unkD0.vx = actor->position.vx >> 16;
        actor->unkD0.vy = actor->position.vy >> 16;
        actor->unkD0.vz = actor->position.vz >> 16;
    }

    {
        ActorData* pActor = g_FieldScriptVMCurActor;
        s32 result;

        pActor->scripts[pActor->curScriptIndex].flags_0 = 0xFFFF;
        result = func_80099AC0(0xFFFF);
        if (result == 0) {
            g_FieldScriptVMCurActor->scriptInstructionPointer += 6;
        }
    }
}

/* asm 80098430-800984E8, opcode 0x4B. Transcribed from
 * asm/field/nonmatchings/main/misc7/func_80098430.s:
 *   lw 0x90(actor+idx*8); and 0xFE7FFFFF; sw (0x80098450-0x8009845C)
 *   lhu 0x90; if == 0xFFFF, jal FieldScriptVMGetArgument(6) and sh back
 *     after reloading g_FieldScriptVMCurActor (0x80098470-0x800984A4)
 *   always jal GetArgument(6) then func_80099AC0; on 0, lhu 0xCC += 8
 *     (0x800984A8-0x800984D8). */
void func_80098430(void) {
    ActorData* actor = g_FieldScriptVMCurActor;

#ifndef FIELD_98430_MUTANT_SKIP_MASK
    actor->scripts[actor->curScriptIndex].flags_0x17 = 0;
#endif
    if (actor->scripts[actor->curScriptIndex].flags_0 == 0xFFFF) {
        g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].flags_0 = FieldScriptVMGetArgument(6);
    }

    if (func_80099AC0(FieldScriptVMGetArgument(6)) == 0) {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 8;
    }
}

extern s16 D_800B21AE;
extern s16 D_800B21B0;
extern s16 D_800B21B2;
extern u8 D_800B21D2;

#ifndef XENO_PC_PORT
extern s32 g_FieldScriptVMCurScriptData_s32 asm("g_FieldScriptVMCurScriptData");
#endif
void func_800984EC(void) {
#ifdef XENO_PC_PORT
    u8 mask1 = SCRIPT_READ_U8_REL(7);
    D_800B21AE = (s16)FieldScriptArgument1(1, mask1);
    mask1 = SCRIPT_READ_U8_REL(7);
    D_800B21B0 = (s16)FieldScriptArgument2(3, mask1);
    mask1 = SCRIPT_READ_U8_REL(7);
    D_800B21B2 = (s16)FieldScriptArgument3(5, mask1);
    D_800B21D2 |= 0x80;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 8;
#else
    /* Raw-offset actor reads: a struct-field access lets gcc hoist the final
     * load above the D_800B21D2 store, which retail does not do. */
    D_800B21AE = FieldScriptArgument1(1, *(u8*)(*(u16*)((u8*)g_FieldScriptVMCurActor + 0xCC) + g_FieldScriptVMCurScriptData_s32 + 7));
    D_800B21B0 = FieldScriptArgument2(3, *(u8*)(*(u16*)((u8*)g_FieldScriptVMCurActor + 0xCC) + g_FieldScriptVMCurScriptData_s32 + 7));
    D_800B21B2 = FieldScriptArgument3(5, *(u8*)(*(u16*)((u8*)g_FieldScriptVMCurActor + 0xCC) + g_FieldScriptVMCurScriptData_s32 + 7));
    D_800B21D2 |= 0x80;
    *(u16*)((u8*)g_FieldScriptVMCurActor + 0xCC) = *(u16*)((u8*)g_FieldScriptVMCurActor + 0xCC) + 8;
#endif
}

extern u8 D_8006FD1C[];

void func_800985BC(void) {
    if (g_FieldSystemMode == 0) {
        s32 v = FieldScriptVMGetVariableValue(FieldScriptVMGetInstructionArgument(1) & 0xFFFF);
        func_800379C8(D_8006FD1C, v, v);
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void func_8009861C(void) {
#ifdef XENO_PC_PORT
    u8 mask;
    s32 arg2, arg3, arg4, arg5, addr;
    s32 dx, dz, dist;
    mask = SCRIPT_READ_U8_REL(0xB);
    arg2 = FieldScriptArgument2(3, mask);
    mask = SCRIPT_READ_U8_REL(0xB);
    arg3 = FieldScriptArgument3(5, mask);
    mask = SCRIPT_READ_U8_REL(0xB);
    arg4 = FieldScriptArgument4(7, mask);
    mask = SCRIPT_READ_U8_REL(0xB);
    arg5 = FieldScriptArgument5(9, mask);
    addr = FieldScriptVMGetInstructionArgument(1);
    dx = arg5 - arg2;
    dz = arg4 - arg3;
    dist = FieldGetVec2Magnitude(dx, dz);
    FieldScriptMemoryWriteU16((u16)addr, (u16)dist);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 0xC;
#else
    s32 x1 = FieldScriptArgument2(3, SCRIPT_READ_U8_REL_RAW(0xB));
    s32 z1 = FieldScriptArgument3(5, SCRIPT_READ_U8_REL_RAW(0xB));
    s32 x2 = FieldScriptArgument4(7, SCRIPT_READ_U8_REL_RAW(0xB));
    s32 z2 = FieldScriptArgument5(9, SCRIPT_READ_U8_REL_RAW(0xB));
    s32 addr = SCRIPT_IMM_ARG(1);
    FieldScriptMemoryWriteU16(addr, FieldGetVec2Magnitude(x2 - x1, z2 - z1));
    g_FieldScriptVMCurActor->scriptInstructionPointer += 0xC;
#endif
}

void func_80098738(void) {
#ifdef XENO_PC_PORT
    u8 mask;
    s32 a2, a3, a4, a5, a6, a7, addr, dist;
    mask = SCRIPT_READ_U8_REL(0xF);
    a2 = FieldScriptArgument2(3, mask);
    mask = SCRIPT_READ_U8_REL(0xF);
    a3 = FieldScriptArgument3(5, mask);
    mask = SCRIPT_READ_U8_REL(0xF);
    a4 = FieldScriptArgument3(7, mask);
    mask = SCRIPT_READ_U8_REL(0xF);
    a5 = FieldScriptArgument4(9, mask);
    mask = SCRIPT_READ_U8_REL(0xF);
    a6 = FieldScriptArgument5(0xB, mask);
    mask = SCRIPT_READ_U8_REL(0xF);
    a7 = FieldScriptArgument5(0xD, mask);
    addr = FieldScriptVMGetInstructionArgument(1);
    dist = FieldGetVec3Magnitude(a6 - a2, a7 - a3, a5 - a4);
    FieldScriptMemoryWriteU16((u16)addr, (u16)dist);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 0x10;
#else
    s32 x1 = FieldScriptArgument2(3, SCRIPT_READ_U8_REL_RAW(0xF));
    s32 y1 = FieldScriptArgument3(5, SCRIPT_READ_U8_REL_RAW(0xF));
    s32 z1 = FieldScriptArgument3(7, SCRIPT_READ_U8_REL_RAW(0xF));
    s32 x2 = FieldScriptArgument4(9, SCRIPT_READ_U8_REL_RAW(0xF));
    s32 y2 = FieldScriptArgument5(0xB, SCRIPT_READ_U8_REL_RAW(0xF));
    s32 z2 = FieldScriptArgument5(0xD, SCRIPT_READ_U8_REL_RAW(0xF));
    s32 addr = SCRIPT_IMM_ARG(1);
    FieldScriptMemoryWriteU16(addr, FieldGetVec3Magnitude(x2 - x1, z2 - z1, y2 - y1));
    g_FieldScriptVMCurActor->scriptInstructionPointer += 0x10;
#endif
}

extern s32 FieldMathInterpolateAngle(s32, s32, s32);

void func_800988B8(void) {
    u8 mask2 = SCRIPT_READ_U8_REL(9);
    s32 arg2 = FieldScriptArgument2(3, mask2);
    u8 mask3 = SCRIPT_READ_U8_REL(9);
    s32 arg3 = FieldScriptArgument3(5, mask3);
    u8 mask4 = SCRIPT_READ_U8_REL(9);
    s32 arg4 = FieldScriptArgument4(7, mask4);
    s32 result = FieldMathInterpolateAngle(arg2, arg3, arg4);
    u16 addr = (u16)FieldScriptVMGetInstructionArgument(1);
    FieldScriptMemoryWriteU16(addr, result);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 0xA;
}

void func_8009899C(void) {
    FieldScriptMemoryWriteU16(SCRIPT_IMM_ARG(1), g_FieldScriptVMCurActor->rotation.vy & 0xFFF);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void func_800989F0(void) {
    s32 actorIdx = FieldScriptVMGetActorIndex(1);
    if (actorIdx != 0xFF) {
        ActorData* pModel =
            (ActorData*)(uintptr_t)g_FieldActors[actorIdx].pActorData;
        u16 addr = (u16)FieldScriptVMGetInstructionArgument(2);
        u16 value = *(u16*)((u8*)pModel + 0x106) & 0xFFF;
        FieldScriptMemoryWriteU16(addr, value);
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 4;
}

#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc7", func_80098A7C);
#else
void func_80098A7C(void) {
    /* Instruction-faithful to retail 80098A7C-80098BFC: the sprite pointer is
     * latched before the three argument calls, the field-actor slot is
     * re-derived from D_800AFD1C afterwards, the arg mask byte is re-read
     * per call, and `x << 16` wraps as u32 (retail sll). */
    ActorData* actor;
    FieldActor* pFieldActor;
    SpriteData* pSpriteData;
    s32 value;

    actor = g_FieldScriptVMCurActor;
    pSpriteData = (SpriteData*)(uintptr_t)g_FieldActors[D_800AFD1C].pSpriteData;
    actor->scriptFlags.flags |= 0x10000;
    actor->flags |= 0x200000;

    value = FieldScriptArgument1(1, SCRIPT_READ_U8_REL(7));
    g_FieldScriptVMCurActor->position.vx = (s32)((u32)value << 16);
    value = FieldScriptArgument2(3, SCRIPT_READ_U8_REL(7));
    g_FieldScriptVMCurActor->position.vz = (s32)((u32)value << 16);
    value = FieldScriptArgument3(5, SCRIPT_READ_U8_REL(7));

    actor = g_FieldScriptVMCurActor;
    actor->position.vy = (s32)((u32)value << 16);
    pFieldActor = &g_FieldActors[D_800AFD1C];
    pFieldActor->transformMatrix.t[0] = (s16)(actor->position.vx >> 16);
    pFieldActor->transformMatrix.t[1] = (s16)(actor->position.vy >> 16);
    pFieldActor->transformMatrix.t[2] = (s16)(actor->position.vz >> 16);
    pSpriteData->position.x = actor->position.vx;
    pSpriteData->position.y = actor->position.vy;
    pSpriteData->position.z = actor->position.vz;
    actor->scriptInstructionPointer += 8;
}
#endif /* XENO_PC_PORT */

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

#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc7", func_80098CAC);
#else
extern void func_800821F4(void* pSpriteData, s16 animIndex, void* pFieldActor);
extern void func_80081F80(void* pSpriteData, s16 angle, void* pFieldActor);

void func_80098CAC(s32 arg0) {
    /* Instruction-faithful to retail 80098CAC-80099210.  Retail `div` has no
     * break guard: moveSpeed == 0 and the INT_MIN / -1 cases use the R3000
     * results (MIPS_DIV); position/step arithmetic wraps as u32. */
    FieldActor* pFieldActor;
    SpriteData* pSprite;
    ActorData* actor;
    u8* script;
    u16 ip;
    s32 speed;
    s32 animationId;
    s32 x, y, z;
    s32 quotient;
    s32 dvx, dvy, dvz;
    s32 prevX, prevY, prevZ;
    s32 value;

    pFieldActor = &g_FieldActors[D_800AFD1C];
    pSprite = (SpriteData*)(uintptr_t)pFieldActor->pSpriteData;
    if (((ActorData*)(uintptr_t)pFieldActor->pActorData)->flags & 0x2000) {
        speed = MIPS_DIV(0x8000000, g_FieldScriptVMCurActor->moveSpeed) >> 16;
    } else {
        speed = MIPS_DIV(0x4000000, g_FieldScriptVMCurActor->moveSpeed) >> 16;
    }
    if (speed == 0) {
        speed = 1;
    }

    actor = g_FieldScriptVMCurActor;
    script = (u8*)g_FieldScriptVMCurScriptData;
    ip = actor->scriptInstructionPointer;
    actor->scriptFlags.flags |= 0x10000;
    animationId = 1;
    if (script[ip + 1] == 0) {
        x = (s32)((u32)FieldScriptArgument1(2, script[ip + 8]) << 16);
        z = (s32)((u32)FieldScriptArgument2(4, SCRIPT_READ_U8_REL(8)) << 16);
        y = (s32)((u32)FieldScriptArgument3(6, SCRIPT_READ_U8_REL(8)) << 16);
        actor = g_FieldScriptVMCurActor;
        quotient = MIPS_DIV((s32)FieldGetVec3Magnitude(
                                (s32)((u32)x - (u32)actor->position.vx) >> 16,
                                (s32)((u32)y - (u32)actor->position.vy) >> 16,
                                (s32)((u32)z - (u32)actor->position.vz) >> 16),
                            speed);
        actor = g_FieldScriptVMCurActor;
        actor->unk102 = (s16)quotient;
        if ((s16)quotient == 0) {
            actor->unk102 = (s16)((u32)quotient + 1);
        }

        actor = g_FieldScriptVMCurActor;
        dvx = MIPS_DIV((s32)((u32)x - (u32)actor->position.vx), actor->unk102);
        dvy = MIPS_DIV((s32)((u32)y - (u32)actor->position.vy), actor->unk102);
        dvz = MIPS_DIV((s32)((u32)z - (u32)actor->position.vz), actor->unk102);
        actor->unkD0.vy = dvy;
        actor->unkD0.vx = dvx;
        actor->unkD0.vz = dvz;
        if ((x >> 16) != (s16)(actor->position.vx >> 16) ||
            (z >> 16) != (s16)(actor->position.vz >> 16)) {
            value = ratan2(dvz >> 16, (s16)(actor->unkD0.vx >> 16));
            g_FieldScriptVMCurActor->rotation.vx = (s16)(0u - (u32)value);
            g_FieldScriptVMCurActor->rotation.vy = (s16)(0u - (u32)value);
        }
        g_FieldScriptVMCurActor->scriptInstructionPointer += 9;
    } else {
        if (actor->unk102 <= 0 || actor->scripts[actor->curScriptIndex].flags_0 == 0) {
            actor->scriptInstructionPointer = (u16)(ip - 9);
            if (actor->scripts[actor->curScriptIndex].flags_0 != 0) {
                prevX = actor->position.vx;
                prevY = actor->position.vy;
                prevZ = actor->position.vz;
                value = FieldScriptArgument1(2, SCRIPT_READ_U8_REL(8));
                g_FieldScriptVMCurActor->position.vx = (s32)((u32)value << 16);
                value = FieldScriptArgument2(4, SCRIPT_READ_U8_REL(8));
                g_FieldScriptVMCurActor->position.vz = (s32)((u32)value << 16);
                value = FieldScriptArgument3(6, SCRIPT_READ_U8_REL(8));
                actor = g_FieldScriptVMCurActor;
                actor->position.vy = (s32)((u32)value << 16);
                actor->moveModified.vx = (s32)((u32)actor->position.vx - (u32)prevX);
                actor->moveModified.vy = (s32)((u32)actor->position.vy - (u32)prevY);
                actor->moveModified.vz = (s32)((u32)actor->position.vz - (u32)prevZ);
            }
            actor = g_FieldScriptVMCurActor;
            animationId = actor->defaultAnimationId;
            if (arg0 == 0) {
                actor->scriptInstructionPointer += 0xB;
            } else {
                actor->scriptInstructionPointer += 0xD;
            }
            actor = g_FieldScriptVMCurActor;
            actor->scripts[actor->curScriptIndex].flags_0 = 0xFFFF;
        } else {
            actor->position.vx = (s32)((u32)actor->position.vx + (u32)actor->unkD0.vx);
            actor->position.vz = (s32)((u32)actor->position.vz + (u32)actor->unkD0.vz);
            actor->position.vy = (s32)((u32)actor->position.vy + (u32)actor->unkD0.vy);
            actor->moveModified.vy = actor->unkD0.vy;
            actor->moveModified.vz = actor->unkD0.vz;
            actor->moveModified.vx = actor->unkD0.vx;
            D_800B00C0 = 1;
            actor->scripts[actor->curScriptIndex].flags_0--;
        }

        actor = g_FieldScriptVMCurActor;
        actor->unk102 = (s16)((u16)actor->unk102 - 1);
        pFieldActor = &g_FieldActors[D_800AFD1C];
        pFieldActor->transformMatrix.t[0] = (s16)(actor->position.vx >> 16);
        pFieldActor->transformMatrix.t[1] = (s16)(actor->position.vy >> 16);
        pFieldActor->transformMatrix.t[2] = (s16)(actor->position.vz >> 16);
        pSprite->position.x = actor->position.vx;
        pSprite->position.y = actor->position.vy;
        pSprite->position.z = actor->position.vz;
    }

    actor = g_FieldScriptVMCurActor;
    if (actor->unkAnimationId != 0xFF) {
        animationId = actor->unkAnimationId;
    }
    if (actor->curAnimationId != animationId && !(actor->scriptFlags.flags & 0x2000000)) {
        actor->curAnimationId = animationId;
        func_800821F4(pSprite, animationId, D_800B06B8);
    }
    func_80081F80(pSprite, g_FieldScriptVMCurActor->rotation.vx, D_800B06B8);
}
#endif /* XENO_PC_PORT */

/*
 * asm 80099214-8009997C, opcode 0x57. Actor jump/lerp with gravity (not a
 * camera opcode). Mode from SCRIPT_READ_U8_REL(1) & 3:
 *   0: init from absolute x/z/y + step count (arg4); bit 0x80 resolves Y via
 *      walkmesh (func_8007B1C4).
 *   1: init from absolute x/z + speed (arg4) → step count via XZ magnitude.
 *   2: arc/jump: derive step count from gravity + Y delta via SquareRoot0.
 *   3: if byte1==0xF, refresh walkmesh tris and clear 0x10000/0x200000;
 *      else tick: advance by unkD0 + sprite step.vy/gravity until unk102
 *      reaches unkE0, then rewind IP by 0xB to re-read init args, snap, and
 *      skip init+continue (IP += 0xD). Always yields via D_800B00C0=1.
 */
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc7", func_80099214);
#else
void func_80099214(void) {
    /* Instruction-faithful to retail 80099214-8009997C.  Retail `div` has no
     * break guard (step count 0 / -1 and INT_MIN / -1 use the R3000 results
     * via MIPS_DIV); `mult` keeps the low word; `x << 16`, position and step
     * arithmetic wrap as u32.  out/state mirror the retail stack rows
     * (sp+0x58 stride 8, sp+0x18 stride 16) handed to func_8007B1C4. */
    ActorData* actor;
    FieldActor* pFieldActor;
    u8* sprite;
    u8* script;
    u16 ip;
    u8 modeByte;
    s32 mode;
    s32 x, y, z;
    s32 stepCount;
    s32 walkmeshId;
    s32 negDuration;
    s32 yDelta;
    s32 dx, dz;
    s32 value;
    s32 i;
    s16 out[4][4];
    s32 state[4][4];

    actor = g_FieldScriptVMCurActor;
    script = (u8*)g_FieldScriptVMCurScriptData;
    ip = actor->scriptInstructionPointer;
    sprite = (u8*)(uintptr_t)g_FieldActors[D_800AFD1C].pSpriteData;
    actor->scriptFlags.flags |= 0x10000;
    modeByte = script[ip + 1];
    mode = modeByte & 3;

    if (mode == 1) {
        x = FieldScriptArgument1(2, script[ip + 0xA]);
        z = FieldScriptArgument2(4, SCRIPT_READ_U8_REL(0xA));
        actor = g_FieldScriptVMCurActor;
        dx = (s32)(((u32)x << 16) - (u32)actor->position.vx);
        dz = (s32)(((u32)z << 16) - (u32)actor->position.vz);
        stepCount = FieldScriptArgument4(8, SCRIPT_READ_U8_REL(0xA));
        stepCount = MIPS_DIV((s32)FieldGetVec2Magnitude(dx >> 16, dz >> 16), stepCount);
    } else if (mode == 0) {
        stepCount = FieldScriptArgument4(8, script[ip + 0xA]);
    } else if (mode == 2) {
        FieldScriptArgument1(2, script[ip + 0xA]);
        FieldScriptArgument2(4, SCRIPT_READ_U8_REL(0xA));
        y = FieldScriptArgument3(6, SCRIPT_READ_U8_REL(0xA));
        negDuration = (s32)(0u - (u32)FieldScriptArgument4(8, SCRIPT_READ_U8_REL(0xA)));
        value = *(s16*)(sprite + 0x1E);
        yDelta = (s32)(((u32)y << 16) - (u32)g_FieldScriptVMCurActor->position.vy);
        value = SquareRoot0((s32)((u32)value * ((u32)negDuration << 1)));
        *(s32*)(sprite + 0x10) = (s32)(0u - ((u32)value << 16));
        SquareRoot0(negDuration);
        value = (s32)((u32)negDuration - (u32)(yDelta >> 16));
        if (value < 0) {
            value = (s32)(0u - (u32)value);
        }
        stepCount = SquareRoot0(value);
        if (stepCount < 0) {
            stepCount = (s32)(0u - (u32)stepCount);
        }
    } else {
        if (modeByte == 0xF) {
            /* Cleanup: refresh walkmesh triangle ids (loop bound re-read). */
            for (i = 0; i < D_800AFB54 - 1; i++) {
                actor = g_FieldScriptVMCurActor;
                value = func_8007B1C4((s16)(actor->position.vx >> 16), (s16)(actor->position.vz >> 16),
                                      i, out[i], state[i]);
                g_FieldScriptVMCurActor->walkmeshTriIds[i] = (u16)value;
            }
            actor = g_FieldScriptVMCurActor;
            actor->scriptFlags.flags &= 0xFFFEFFFF;
            actor->flags &= 0xFFDFFFFF;
            actor->scriptInstructionPointer += 2;
            D_800B00C0 = 1;
            return;
        }

        if (actor->unk102 < (s16)actor->unkE0) {
            actor->position.vx = (s32)((u32)actor->position.vx + (u32)actor->unkD0.vx);
            actor->position.vz = (s32)((u32)actor->position.vz + (u32)actor->unkD0.vz);
            actor->position.vy = (s32)((u32)actor->position.vy + *(u32*)(sprite + 0x10));
            *(u32*)(sprite + 0x10) = *(u32*)(sprite + 0x10) + *(u32*)(sprite + 0x1C);
            if (actor->unkD0.vx != 0 || actor->unkD0.vz != 0) {
                if (!(actor->scriptFlags.flags & 0x8000)) {
                    value = func_8007B694((s32*)&actor->unkD0) | 0x8000;
                    g_FieldScriptVMCurActor->rotation.vx = (s16)value;
                    g_FieldScriptVMCurActor->rotation.vy = (s16)value;
                }
            }
        } else {
            /* Rewind to the paired init opcode to re-read the snap target. */
            actor->scriptInstructionPointer = (u16)(ip - 0xB);
            x = FieldScriptArgument1(2, script[(u16)(ip - 0xB) + 0xA]);
            z = FieldScriptArgument2(4, SCRIPT_READ_U8_REL(0xA));
            if (SCRIPT_READ_U8_REL(1) & 0x80) {
                walkmeshId = FieldScriptArgument3(6, SCRIPT_READ_U8_REL(0xA));
                value = func_8007B1C4(x, z, walkmeshId, out[walkmeshId], state[walkmeshId]);
                g_FieldScriptVMCurActor->walkmeshTriIds[walkmeshId] = (u16)value;
                y = out[walkmeshId][1];
            } else {
                y = FieldScriptArgument3(6, SCRIPT_READ_U8_REL(0xA));
            }
            actor = g_FieldScriptVMCurActor;
            *(s32*)(sprite + 0x10) = 0;
            actor->position.vx = (s32)((u32)x << 16);
            actor->position.vy = (s32)((u32)y << 16);
            actor->position.vz = (s32)((u32)z << 16);
            actor->scriptFlags.flags &= 0xFFFEFFFF;
            actor->scriptInstructionPointer += 0xD;
            actor->flags &= 0xFFDFFFFF;
        }

        actor = g_FieldScriptVMCurActor;
        pFieldActor = &g_FieldActors[D_800AFD1C];
        pFieldActor->transformMatrix.t[0] = (s16)(actor->position.vx >> 16);
        pFieldActor->transformMatrix.t[1] = (s16)(actor->position.vy >> 16);
        pFieldActor->transformMatrix.t[2] = (s16)(actor->position.vz >> 16);
        *(s32*)(sprite + 0x0) = actor->position.vx;
        *(s32*)(sprite + 0x4) = actor->position.vy;
        *(s32*)(sprite + 0x8) = actor->position.vz;
        actor->unk102 = (s16)((u16)actor->unk102 + 1);
        D_800B00C0 = 1;
        return;
    }

    /* Shared init (retail 800992E4). */
    if (stepCount == 0) {
        stepCount = 1;
    }
    x = FieldScriptArgument1(2, SCRIPT_READ_U8_REL(0xA));
    z = FieldScriptArgument2(4, SCRIPT_READ_U8_REL(0xA));
    if (SCRIPT_READ_U8_REL(1) & 0x80) {
        walkmeshId = FieldScriptArgument3(6, SCRIPT_READ_U8_REL(0xA));
        func_8007B1C4(x, z, walkmeshId, out[walkmeshId], state[walkmeshId]);
        y = out[walkmeshId][1];
        g_FieldScriptVMCurActor->walkmeshId = (s16)walkmeshId;
    } else {
        y = FieldScriptArgument3(6, SCRIPT_READ_U8_REL(0xA));
    }

    actor = g_FieldScriptVMCurActor;
    value = (s32)(*(u32*)(sprite + 0x1C) * (u32)stepCount);
    value = (s32)((u32)value + ((u32)value >> 31)) >> 1;
    value = (s32)(0u - (u32)value);
    *(s32*)(sprite + 0x10) = value;
    *(s32*)(sprite + 0x10) = (s32)((u32)value +
        (u32)MIPS_DIV((s32)(((u32)y << 16) - (u32)actor->position.vy), stepCount));
    dx = MIPS_DIV((s32)(((u32)x << 16) - (u32)actor->position.vx), (s32)((u32)stepCount + 1));
    dz = MIPS_DIV((s32)(((u32)z << 16) - (u32)actor->position.vz), (s32)((u32)stepCount + 1));
    actor->unkD0.vy = 0;
    actor->unkE0 = (u16)stepCount;
    actor->unk102 = 0;
    actor->scriptInstructionPointer += 0xB;
    actor->unkD0.vx = dx;
    actor->unkD0.vz = dz;
    D_800B00C0 = 1;
}
#endif /* XENO_PC_PORT */

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
    /* Retail ADDU wraps before the SDK lookup, including 0x80000000. */
    return SquareRoot0((s32)((u32)vecSquared.vx + (u32)vecSquared.vy));
}

long FieldGetVec1Magnitude(long x) {
    VECTOR vec;
    VECTOR vecSquared;

    vec.vx = x;
#ifdef XENO_PC_PORT
    vec.vy = 0;
    vec.vz = 0;
#endif
    Square0(&vec, &vecSquared);
    return SquareRoot0(vecSquared.vx);
}

extern int FieldScriptArgument1(int index, int mask);
extern int FieldScriptArgument2(int index, int mask);
extern int FieldScriptArgument5(int index, int mask);
extern int FieldScriptArgument6(int index, int mask);

/*
 * asm 80099AC0-80099EF4. Movement-toward-target tick: advances a per-script
 * "steps remaining" counter (scripts[curScriptIndex].flags_0, a 16-bit
 * countdown reusing the ActorScriptSlot bitfield word at +0x90 -- verified
 * against func_80099980's asm, which writes the identical field at the same
 * offset) while g_FieldScriptVMCurActor closes on a target point selected by
 * scripts[curScriptIndex].flags_0x17 (0=absolute, 1=relative to unkD0,
 * 2=another actor, 3=random point on a circle). Returns -1 while still
 * approaching (and holds the VM via D_800B00C0=1), 0 once arrived (or if a
 * case-1 target actor index is invalid).
 */
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc7", func_80099AC0);
#else
s32 func_80099AC0(s32 useStoredAngle) {
#ifdef NPC_EVENT_MUTANT_ALWAYS_DONE
    return 0;
#endif
#ifdef NPC_EVENT_MUTANT_NO_IP
    D_800B00C0 = 1;
    return -1;
#endif
    FieldActor* refFieldActor = &g_FieldActors[D_800AFD1C];
    ActorData* refActorData = (ActorData*)(uintptr_t)refFieldActor->pActorData;
    u8* refSpriteData = (u8*)(uintptr_t)refFieldActor->pSpriteData;
    ActorData* actor = g_FieldScriptVMCurActor;
    ActorScriptSlot* slot = &actor->scripts[actor->curScriptIndex];
    s32 selfX, selfZ, targetX = 0, targetZ = 0, combinedSolidRange = 0;
    s32 dx, dz, distance, stepMagnitude;
    s32 deltaVec[3];
    s16 finalAngle;

    /*
     * asm 80099AD8-80099B80: per-frame movement quantum, cached in the
     * reference actor's sprite data at +0x18 (an opaque SpriteData sub-field;
     * SpriteData itself has no declared layout in actor.h, matching the raw
     * pSpriteData+offset convention already used elsewhere, e.g. misc6.c).
     */
    if (refActorData->flags & 0x2000) {
        *(s32*)(refSpriteData + 0x18) = 0x08000000 / (s16)actor->moveSpeed;
    } else if (*(s32*)(refSpriteData + 0x18) == 0) {
        *(s32*)(refSpriteData + 0x18) = 0x04000000 / (s16)actor->moveSpeed;
    }
    stepMagnitude = FieldGetVec1Magnitude(*(s32*)(refSpriteData + 0x18) >> 15) + 1;

    /* asm 80099B98/80099BAC: upper halfword of self position.vx/vz. */
    selfX = *(s16*)((u8*)actor + 0x22);
    selfZ = *(s16*)((u8*)actor + 0x2A);

    switch (slot->flags_0x17) {
    case 0:
        targetX = FieldScriptArgument1(ARG(1), SCRIPT_READ_U8_REL(5));
        targetZ = FieldScriptArgument2(ARG(2), SCRIPT_READ_U8_REL(5));
        break;

    case 1:
        targetX = FieldScriptArgument1(ARG(1), SCRIPT_READ_U8_REL(5)) + actor->unkD0.vx;
        targetZ = FieldScriptArgument2(ARG(2), SCRIPT_READ_U8_REL(5)) + actor->unkD0.vz;
        break;

    case 2: {
#ifdef NPC_EVENT_MUTANT_MODE2_CIRCLE
        s32 angle = FieldScriptVMGetArgument(1) & 0xFFF;
        targetX = actor->unkD0.vx + ((rsin(angle) << 12) >> 12);
        targetZ = actor->unkD0.vz - ((rcos(angle) << 12) >> 12);
#else
        u32 otherIdx = FieldScriptVMGetActorIndex(1);
        ActorData* other;

        if (otherIdx == 0xFF) {
            return 0;
        }
        otherIdx = FieldScriptVMGetActorIndex(1);
        other = (ActorData*)(uintptr_t)g_FieldActors[otherIdx].pActorData;

        combinedSolidRange = FieldGetVec1Magnitude(other->solidRange + actor->solidRange);
        targetX = *(s16*)((u8*)other + 0x22);
        targetZ = *(s16*)((u8*)other + 0x2A);

        if (SCRIPT_READ_U8_REL(1) == g_PlayerActorIndex) {
            actor->scriptFlags.flags |= 0x200000;
        }
#endif
        break;
    }

    case 3: {
        s32 angle = FieldScriptVMGetArgument(1) & 0xFFF;
        targetX = actor->unkD0.vx + ((rsin(angle) << 12) >> 12);
        targetZ = actor->unkD0.vz - ((rcos(angle) << 12) >> 12);
        break;
    }
    }

    dx = targetX - selfX;
    dz = targetZ - selfZ;
    distance = FieldGetVec2Magnitude(dx, dz);
    deltaVec[0] = dx;
    deltaVec[1] = 0;
    deltaVec[2] = dz;

    actor->scriptFlags.flags |= 0x400000;

    if (slot->flags_0 != 0 && stepMagnitude + combinedSolidRange < distance) {
#ifdef XENO_PC_PORT
        /* DIAGNOSTIC (XENO_MOVE_DIAG=1): a scripted walk that never arrives is
         * what hangs the field-12 Alice scene -- Fei animates in place while
         * this tick keeps returning -1. This function does NOT translate the
         * actor; it only faces the target and raises scriptFlags 0x400000 for
         * the mover. So the question is whether the POSITION changes between
         * ticks: if `self` is pinned while `d` never shrinks, the mover is
         * refusing the move (the missing-floor theory) rather than the script
         * asking for something impossible. Removal: delete this block. */
        {
            static int s_on = -1;
            static s32 s_lastX, s_lastZ;
            static unsigned long s_ticks;

            if (s_on < 0) {
                const char* e = getenv("XENO_MOVE_DIAG");
                s_on = (e != NULL && e[0] != '\0' && e[0] != '0');
            }
            if (s_on) {
                int moved = (selfX != s_lastX) || (selfZ != s_lastZ);
                if ((s_ticks++ % 30) == 0 || moved) {
                    printf("[xeno-port][move] walk tick #%lu actor=%d "
                           "self=(%d,%d) target=(%d,%d) d=%d step=%d "
                           "solid=%d countdown=%u moved=%d\n",
                           s_ticks, (int)D_800AFD1C, (int)selfX, (int)selfZ,
                           (int)targetX, (int)targetZ, (int)distance,
                           (int)stepMagnitude, (int)combinedSolidRange,
                           (unsigned)slot->flags_0, moved);
                    fflush(stdout);
                }
                s_lastX = selfX;
                s_lastZ = selfZ;
            }
        }
#endif
        slot->flags_0 = slot->flags_0 - 1;
        finalAngle = (s16)func_8007B694(deltaVec);
        D_800B00C0 = 1;
        actor->rotation.vx = finalAngle;
        actor->rotation.vy = finalAngle;
        return -1;
    }

    /* asm 80099DF4+: arrived (or flags_0==0 short-circuit). Snap the final
     * facing angle, then reset the per-script movement state. */
    if (useStoredAngle != 0) {
        if ((actor->scriptFlags.flags & 0x8000) == 0) {
            finalAngle = actor->rotation.vy | 0x8000;
        } else {
            finalAngle = actor->unk11C | 0x8000;
        }
    } else {
        finalAngle = (s16)func_8007B694(deltaVec);
    }
    actor->rotation.vx = finalAngle;
    actor->rotation.vy = finalAngle;

    slot->flags_0 = 0xFFFF;
    slot->flags_0x17 = 0;
    actor->scriptFlags.flags &= 0xFDDFF7FF;
    return 0;
}
#endif /* XENO_PC_PORT */

void FieldScriptVMWriteCurCharacterID(void) {
    FieldScriptMemoryWriteU16(SCRIPT_IMM_ARG(1), g_FieldScriptVMCurActor->characterId);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

/* Retail 0x80099F48-0x80099FC4: write the character id belonging to the
 * player actor (not necessarily the VM's current actor), then consume the
 * three-byte instruction. */
void FieldScriptVMWritePartyLeaderCharacterID(void) {
#ifdef FIELD_VM_AUDIT_MUTANT_LEADER_CURRENT_ACTOR
    FieldScriptMemoryWriteU16(
        SCRIPT_IMM_ARG(1), g_FieldScriptVMCurActor->characterId);
#else
    ActorData* player =
        (ActorData*)(uintptr_t)g_FieldActors[g_PlayerActorIndex].pActorData;
    FieldScriptMemoryWriteU16(SCRIPT_IMM_ARG(1), player->characterId);
#endif
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

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

extern s32 D_8005A444[];

void func_8009A1E4(void) {
#ifdef XENO_PC_PORT
    void* pActor = g_FieldScriptVMCurActor;
    u8* pScriptData = (u8*)g_FieldScriptVMCurScriptData;
    u8 arg = *(u8*)(pScriptData + *(u16*)((u8*)pActor + 0xCC) + 1);
    s32 targetIdx = D_8005A444[arg];
    if (targetIdx != 0xFF) {
        ActorData* pTargetModel =
            (ActorData*)(uintptr_t)g_FieldActors[targetIdx].pActorData;
        s32 dx = *(s32*)((u8*)pTargetModel + 0x28) - *(s32*)((u8*)pActor + 0x28);
        s32 dz = *(s32*)((u8*)pTargetModel + 0x20) - *(s32*)((u8*)pActor + 0x20);
        s32 angle = -ratan2(dx, dz);
        angle |= 0x8000;
        *(u16*)((u8*)pActor + 0x104) = (u16)angle;
        *(u16*)((u8*)pActor + 0x106) = (u16)angle;
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
#else
    /* Retail reloads g_FieldScriptVMCurActor after the ratan2 call and again for
     * the IP advance, so the global is read at each use instead of being cached. */
    s32 targetIdx = D_8005A444[SCRIPT_READ_U8_REL(1)];
    if (targetIdx != 0xFF) {
        ActorData* target = (ActorData*)(uintptr_t)g_FieldActors[targetIdx].pActorData;
        s16 angle = -ratan2(target->position.vz - g_FieldScriptVMCurActor->position.vz,
                            target->position.vx - g_FieldScriptVMCurActor->position.vx) | 0x8000;
        g_FieldScriptVMCurActor->rotation.vx = angle;
        g_FieldScriptVMCurActor->rotation.vy = angle;
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
#endif
}

/* Opcode 0x6F (OP_ROTATE_TO_ACTOR), asm 8009A2A8-8009A348 line-verified.
 * Turn the current actor to face the target actor named by script byte +1,
 * then advance the IP by 2. The angle is ratan2(dz, dx) negated with the
 * 0x8000 "facing changed" latch, stored into both rotation.vx (+0x104) and
 * rotation.vy (+0x106); when the target id resolves to ACTOR_ID_INVALID the
 * rotation is left untouched but the IP still advances (retail reaches the
 * shared .L8009A324 tail either way). The stub previously bound here never
 * advanced the IP, so talk scripts spun on this opcode forever. */
void func_8009A2A8(void) {
    s32 targetIndex = FieldScriptVMGetActorIndex(1);

    if (targetIndex != ACTOR_ID_INVALID) {
        ActorData* target =
            (ActorData*)(uintptr_t)g_FieldActors[targetIndex].pActorData;
        s32 dz = target->position.vz - g_FieldScriptVMCurActor->position.vz;
        s32 dx = target->position.vx - g_FieldScriptVMCurActor->position.vx;
        s16 angle = (s16)((-ratan2(dz, dx)) | 0x8000);

        g_FieldScriptVMCurActor->rotation.vx = angle;
        g_FieldScriptVMCurActor->rotation.vy = angle;
    }

    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
}

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): field_event_blend_camera_distance (src/decomp/src/field/field_event.c:7572). */
#ifndef XM_T_xm_SavedView
#define XM_T_xm_SavedView
typedef struct {
    s16 octant;
    s32 projection;
    s16 elevation;
} xm_SavedView;
#endif
#ifndef XM_T_xm_ModelLight
#define XM_T_xm_ModelLight
typedef struct {
    s32 vx, vy, vz;
    u16 r, g, b;
} xm_ModelLight;
#endif
#ifndef XM_T_xm_MorphDelta
#define XM_T_xm_MorphDelta
typedef struct {
    s16 dx, dy, dz;
    s16 index;
} xm_MorphDelta;
#endif
#ifndef XM_T_xm_MorphTarget
#define XM_T_xm_MorphTarget
typedef struct {
    s32 count;
    void *deltas;
    xm_MorphDelta *normals;
} xm_MorphTarget;
#endif
#ifndef XM_T_xm_MorphTable
#define XM_T_xm_MorphTable
typedef struct {
    s32 count;
    xm_MorphTarget targets[1];
} xm_MorphTable;
#endif
#ifndef XM_T_xm_SpriteModel
#define XM_T_xm_SpriteModel
typedef struct {
    u16 flags;          
    u16 vertex_count;
    u16 primitive_count;
    u16 group_count;    
    SVECTOR *vertices;
    SVECTOR *normals;
    u8 *unk10;          
    u8 *unk14;
    u8 *unk18;
    xm_MorphTable *morphs; 
    SVECTOR box_min;    
    SVECTOR box_max;
    s32 aux_size;       
    s32 packet_size;    
} xm_SpriteModel;
#endif
#ifndef XM_T_xm_MorphChannel
#define XM_T_xm_MorphChannel
typedef struct xm_MorphChannel {
    s32 (*update)(struct xm_MorphChannel *channel);
    s32 target;
    s32 weight;
    s32 step;
    u8 unk10[0x10];
} xm_MorphChannel;
#endif
#ifndef XM_T_xm_MorphState
#define XM_T_xm_MorphState
typedef struct {
    xm_SpriteModel *model;
    SVECTOR *vertices;
    SVECTOR *normals;
    s32 count;
    xm_MorphChannel *channels;
} xm_MorphState;
#endif
#ifndef XM_T_xm_FieldInstance
#define XM_T_xm_FieldInstance
typedef struct {
    u8 unk00[4];
    xm_SpriteModel *mesh; 
    void *packets[2]; 
    u8 unk10[2];
    s16 mode;        
    xm_MorphState *anims; 
    s16 center[3];   
    s16 unk1E;
    s16 radius;      
} xm_FieldInstance;
#endif
#ifndef XM_T_xm_SpritePart
#define XM_T_xm_SpritePart
typedef struct {
    s16 x, y;              
    u8 u, v;               
    u8 w, h;               
    u8 byte8, byte9;       
    u16 tpage;             
    u16 clut;              
    u8 unknowne[2];
    u32 colour;            
    u32 flags;             
} xm_SpritePart;
#endif
#ifndef XM_T_xm_SpriteRendererEntry
#define XM_T_xm_SpriteRendererEntry
typedef struct {
    s8 byte0;              
    s8 byte1;              
    s16 half2;
    s16 half4;
    s16 half6;
} xm_SpriteRendererEntry;
#endif
#ifndef XM_T_xm_SpriteOffset
#define XM_T_xm_SpriteOffset
typedef struct {
    s8 x, y;
} xm_SpriteOffset;
#endif
#ifndef XM_T_xm_SpriteRenderer
#define XM_T_xm_SpriteRenderer
typedef struct {
    s16 angle_x, angle_y, angle_z; 
    s16 scale_x, scale_y, scale_z; 
    MATRIX matrix;                 
    xm_SpritePart *parts[2];          
    xm_SpriteRendererEntry *pointer34; 
    struct xm_Sprite *next_pending;   
    xm_SpriteOffset offset;           
    u8 unknown3e[2];
    s32 word40;                    
    SVECTOR light_angles;          
    u16 light_colour[3];           
} xm_SpriteRenderer;
#endif
#ifndef XM_T_xm_Sprite
#define XM_T_xm_Sprite
typedef struct xm_Sprite {
    s32 x, y, z;                
    s32 speed_x, speed_y, speed_z; 
    s32 speed;                  
    s32 gravity;                
    xm_SpriteRenderer *renderer;   
    void *image;                
    u8 red, green, blue;     
    u8 colour_flags;         
    s16 scale;               
    s16 depth;               
    s16 half30;              
    s16 direction;           
    u16 frame;               
    u16 height;              
    u16 extent_depth;        
    u16 rate;                
    union {
        u32 word;
        struct {
        unsigned sides : 2;      
        unsigned unknown2 : 1;
        unsigned flip : 1;       
        unsigned flip_y : 1;
        unsigned blend : 3;      
        unsigned unknown8 : 8;
        unsigned field16 : 4;
        unsigned mode : 4;       
        unsigned no_view : 1;    
        unsigned unknown25 : 3;
        unsigned dirty : 1;      
        unsigned unknown29 : 3;
        } bits;
        u8 bytes[4];         
    } render;                
    u32 flags;               
    s32 *resource_block;     
    s32 *animations;         
    s32 resource;            
    s32 word50;              
    u16 *frame_table;        
    u16 *animation;          
    u16 *facings;            
    u16 *word60;             
    u8 *script;              
    void (*callback)(struct xm_Sprite *sprite); 
    void *block;             
    struct xm_Sprite *parent;   
    struct xm_Sprite *partner;  
    s32 word78;              
    void *sequencer;         
    u16 word80;              
    u16 word82;              
    s16 ground;              
    u16 size;                
    u8 *frames;              
    s8 stack_top;            
    u8 unknown8d;
    u8 stack[0x10];          
    s16 countdown;           
    s16 target_x, target_y, target_z; 
    u8 unknowna6[2];
    struct {
        unsigned sequencer_owned : 1; 
        unsigned bounce : 10;    
        unsigned frame : 6;      
        unsigned step : 3;       
        unsigned phase : 2;      
        unsigned field22 : 6;    
        unsigned field28 : 2;
        unsigned unknown30 : 2;
    } frame_bits;            
    union {
        u32 word;
        struct {
        unsigned unknown0 : 2;
        unsigned mirror : 1;     
        unsigned frame_flip : 1; 
        unsigned unknown4 : 1;
        unsigned owns_children : 1; 
        unsigned double_step : 1;
        unsigned divisor : 12;   
        unsigned unknown19 : 13;
        } bits;
        u8 bytes[4];
    } motion;                
    union {
        u32 wordb0;          
        u8 byteb0;
        struct {
            unsigned unknown0 : 8;
            unsigned passive_children : 1; 
            unsigned share_rate : 1;       
            unsigned unknown10 : 22;
        } bits;
    } b0;                    
} xm_Sprite;
#endif
#ifndef XM_T_xm_FieldMarker
#define XM_T_xm_FieldMarker
typedef struct xm_FieldMarker {
    SVECTOR v[4];
    POLY_FT4 poly[2];
} xm_FieldMarker;
#endif
#ifndef XM_T_xm_Fixed
#define XM_T_xm_Fixed
typedef union {
    s32 value;
    struct {
        u16 fraction;
        s16 whole;
    } part;
} xm_Fixed;
#endif
#ifndef XM_T_xm_ScriptSlot
#define XM_T_xm_ScriptSlot
typedef struct xm_ScriptSlot {
    u16 resume_pc;      
    u8 countdown;       
    u8 tag;             
    u32 value : 16;     
    u32 unk16 : 2;
    u32 priority : 4;   
    u32 unk22 : 1;
    u32 move_mode : 2;  
    u32 unk25 : 7;
} xm_ScriptSlot;
#endif
#ifndef XM_T_xm_PlatformLink
#define XM_T_xm_PlatformLink
typedef struct xm_PlatformLink {
    SVECTOR rotation; 
    s16 radius;       
    s16 unkA;
} xm_PlatformLink;
#endif
#ifndef XM_T_xm_ActorState
#define XM_T_xm_ActorState
typedef struct {
    u32 mode : 2;   
    u32 unk2 : 3;   
    u32 unk5 : 1;   
    u32 depth : 3;  
    u32 octant : 3; 
    u32 unk12 : 1;  
    u32 layer : 3;  
    u32 unk16 : 2;  
    u32 unk18 : 10; 
    u32 unk28 : 4;
} xm_ActorState;
#endif
#ifndef XM_T_xm_FieldActor
#define XM_T_xm_FieldActor
typedef struct xm_FieldActor {
    u32 flags;           
    u32 layer_flags;     
    s16 triangle[4];     
    s16 layer;           
    u8 unk012[2];
    u32 floor_attribute; 
    s16 unk18;           
    s16 height;          
    xm_Fixed gravity;       
    s32 position[3];     
    u8 unk02C[4];
    s32 unk030[3];       
    u8 unk03C[4];
    s32 unk40[3];        
    u8 unk04C[4];
    s32 unk50[3];        
    u8 unk05C[4];
    s16 unk60;           
    s16 unk62;           
    s16 unk64;           
    u8 unk066[2];
    s16 last_position[3]; 
    u16 stuck;           
    s16 unk70;           
    s16 unk72;           
    u8 ridden_actor;     
    u8 unk075;           
    s16 unk76;           
    u16 call_stack[4];   
    u8 character;        
    u8 unk081;
    u8 unk82;            
    u8 unk83;            
    s32 unk84;           
    s16 unk88;           
    s16 unk8A;           
    xm_ScriptSlot slots[8]; 
    u16 pc;              
    u8 slot;             
    u8 unk0CF;
    s32 target[3];       
    u8 unk0DC[0xE2 - 0xDC];
    u8 unkE2;            
    u8 unk0E3;
    s16 unkE4;           
    s16 unkE6;           
    s16 unkE8;           
    s16 unk0EA;          
    s16 unkEC;           
    s16 unkEE;           
    s32 unkF0;           
    s16 scale[3];        
    u8 unk0FA[2];
    u8 color0[3];        
    u8 color1[3];        
    u16 unk102;          
    s16 heading;         
    s16 heading_goal;    
    s16 unk108;          
    u16 sound;           
    u8 sound_volume;     
    u8 sound_mode;       
    u8 unk10E[0x110 - 0x10E];
    struct xm_PlatformLink *link; 
    void *unk114;        
    s32 *list;           
    u16 unk11C;          
    s16 unk11E;          
    void *unk120;        
    s16 unk124;          
    u8 unk126;           
    u8 unk127;           
    u16 unk128;          
    u8 unk12A[2];
    union {
        u32 word;
        xm_ActorState bits;
    } state;             
    u32 unk130 : 9;      
    u32 unk130_9 : 10;   
    u32 unk130_19 : 9;   
    u32 sprite_kind : 2; 
    u32 unk130_30 : 2;
    u32 unk134;          
} xm_FieldActor;
#endif
#ifndef XM_T_xm_FieldDescriptor
#define XM_T_xm_FieldDescriptor
typedef struct xm_FieldDescriptor {
    xm_FieldInstance *instance; 
    xm_Sprite *model;           
    struct xm_FieldMarker *shadow; 
    MATRIX matrix;           
    MATRIX transform;        
    xm_FieldActor *actor;       
    SVECTOR rotation;        
    u16 flags;               
    u16 unk5A;               
} xm_FieldDescriptor;
#endif
#ifndef XM_T_xm_Attribute
#define XM_T_xm_Attribute
typedef union {
    u32 word;
    u8 bytes[4];
} xm_Attribute;
#endif
#ifndef XM_T_xm_CollisionTriangle
#define XM_T_xm_CollisionTriangle
typedef struct {
    s16 unk00[6];
    u8 attribute;
    u8 unk0D;
} xm_CollisionTriangle;
#endif
#ifndef XM_T_xm_FieldComponents
#define XM_T_xm_FieldComponents
typedef struct {
    s32 descriptor_count;                      
    xm_FieldDescriptor *descriptors;              
    s32 *geometry;                             
    s32 *collision;                            
    s32 *sprites;                              
    xm_Attribute *collision_attributes;           
    xm_CollisionTriangle *collision_triangles[4]; 
    void *collision_vertices[4];               
    s32 triangle_counts[4];                    
    s16 layer_count;                           
} xm_FieldComponents;
#endif
#ifndef XM_T_xm_FieldView
#define XM_T_xm_FieldView
typedef struct xm_FieldView {
    VECTOR eye;              
    VECTOR target;           
    VECTOR up;               
    VECTOR eye_goal;         
    VECTOR target_goal;      
    VECTOR unk050;           
    VECTOR shake_offset;     
    VECTOR saved_target;     
    VECTOR point_actor_a;    
    VECTOR saved_eye;        
    VECTOR point_actor_b;    
    s32 scripted_zoom;       
    s16 mode;                
    s16 scripted_elevation;  
    s16 scripted_heading;    
    s16 scripted_scale;      
    u16 scripted;            
    s16 target_steps;        
    VECTOR scripted_target;  
    VECTOR target_step;      
    s16 eye_steps;           
    s32 scripted_eye[3];     
    s32 unk0F0;              
    s32 eye_step[3];         
    s32 unk100;              
    s32 target_a;            
    s32 target_b;            
    s16 angle;               
    s16 view_angle;          
    MATRIX previous_view;    
    MATRIX orbit;            
    SVECTOR orbit_angles;    
    u32 flags;               
    s16 bounds[4];           
    SVECTOR heading_angles;  
    s32 heading_velocity;    
    s32 heading_high;        
    u8 heading_blocks[2];    
    s16 heading_steps;       
    s32 projection;          
    u16 elevation;           
    s16 distance;            
    s16 elevation_steps;     
    s32 elevation_value;     
    s32 elevation_step;      
    u32 heading;             
    s16 projection_steps;    
    s32 projection_value;    
    s32 projection_step;     
    s16 steps;               
    s32 start;               
    s32 step;                
    s16 shake;               
    s16 shake_time;          
    s16 shake_stop;          
    s32 shake_amplitude[3];  
    s32 shake_step[3];       
    xm_SavedView saved_view;    
    SVECTOR world_angles;    
    SVECTOR anchor;          
    MATRIX scaled_world;     
    MATRIX unk204;           
    MATRIX world_matrix;     
    s32 scale;               
    xm_ModelLight lights[3];    
    s16 back_color[3];       
    u8 unk28A[2];
    xm_FieldComponents components; 
} xm_FieldView;
#endif
#ifndef XM_T_EVENT_OPERAND_BYTE
#define XM_T_EVENT_OPERAND_BYTE
#define EVENT_OPERAND_BYTE(offset) (xm_field_event_bytecode[xm_field_current_event_actor->pc + (offset)])
#endif
#ifndef XM_T_xm_FadeChannel
#define XM_T_xm_FadeChannel
typedef struct {
    DR_MODE modes[2]; 
    TILE tiles[2];   
    s32 level[3];
    s32 step[3];
    u16 abr;
    s16 active;
    s16 steps;
    u16 pad;
} xm_FadeChannel;
#endif
#ifndef XM_T_xm_FieldWork
#define XM_T_xm_FieldWork
typedef struct xm_FieldWork {
    s16 unk2078;               
    s16 unk207A;               
    s16 effect_steps;          
    u8 unk207E[2];
    s32 effect_value[6];       
    s32 effect_step[6];        
    s16 unk20B0[2];            
    void *effect_buffers[4];   
    xm_FadeChannel fades[2];      
    u16 open_windows;          

    s16 encounter_inhibition;  
    s16 terrain_angle;         
    u16 input_mask;            
    s32 unk217C;               
    s32 unk2180;               
    SVECTOR sprite_angles;     
    s16 scale;                 
    s16 sprite_gate;           
    u8 fog_color[4];           
    u8 far_color[4];           
    s16 fog_range[2];          
    u8 clear_color[3];         
    u8 party_bits;             
    s16 unk21A0[6];            
    s16 emitter_range;         
    s16 piece_drift[3];        
    s16 unk21B4;               
    u8 unk21B6[2];
    s32 last_sound_effect;     
    s32 unk21BC[3];            
    u8 unk21C8[0x21CC - 0x21C8];
    u8 party_processing_mode;  
    u8 camera_floor_fixed;     
    u8 preserve_nonplayer_motion; 
    u8 forced_position;        
    u8 script_control[2];      
    u8 piece_drift_mode;       
    u8 unk21D3;
    s16 unk21D4;               
    s16 text_speed;            
    s32 camera_counter;        
    s16 unk21DC[4];            
    s16 unk21E4[4];            
    SVECTOR layer_positions[4]; 
    s32 layer_scales[4];       
    s16 unk221C[5][3];         
    u8 unk223A[2];
    s16 unk223C[3][3];         
    u8 unk224E[0x225C - 0x224E];
    u8 unk225C[3];             
    u8 unk225F[0x2264 - 0x225F]; 
    s32 unk2264;               
    s32 unk2268;               
    s32 controlled;            
    u16 encounter_music[16];   
    s16 battle_music;          
    u8 unk2292[2];
    s32 unk2294;               
    s32 unk2298;               
    s32 unk229C;               
    u16 unk22A0[32];           
    s16 unk22E0;               
    s16 emitter_descriptor[3]; 
    s16 unk22E8[3][4];         
    s16 unk2300[3][4];         
    s32 unk2318[3];            
    s16 emitter_position[3][4]; 
    u16 effects_kept;          
    u16 unk233E;               
    s16 repeat_delay;          
    s16 repeat_remaining;      
    s16 jump_mode;             
    s16 animation_mode;        
    u16 unk2348;               
    s16 unk234A;               
    s16 gear_riding_lock_override; 
    s16 followers_idle;        
    u32 unk2350;               
    u8 unk2354;                
    u8 unk2355;                
    u8 unk2356;                
    u8 unk2357;                
    u8 unk2358;                
    u8 unk2359[0x235C - 0x2359];
} xm_FieldWork;
#endif
#ifndef XM_S_field_event_blend_camera_distance
#define XM_S_field_event_blend_camera_distance
void xm_field_event_blend_camera_distance(void) __asm__("func_8009A34C");
#endif
#ifndef XM_S_field_view
#define XM_S_field_view
extern xm_FieldView xm_field_view __asm__("g_CameraEye");
#endif
#ifndef XM_S_field_event_bytecode
#define XM_S_field_event_bytecode
extern u8 *xm_field_event_bytecode __asm__("g_FieldScriptVMCurScriptData");
#endif
#ifndef XM_S_field_current_event_actor
#define XM_S_field_current_event_actor
extern xm_FieldActor *xm_field_current_event_actor __asm__("g_FieldScriptVMCurActor");
#endif
#ifndef XM_S_field_work
#define XM_S_field_work
extern xm_FieldWork xm_field_work __asm__("g_FieldEffects");
#endif
#ifndef XM_S_field_event_read_imm_or_var
#define XM_S_field_event_read_imm_or_var
s32 xm_field_event_read_imm_or_var(s32 offset) __asm__("FieldScriptVMGetArgument");
#endif
void func_8009A34C(void) {
    s32 step;

    xm_field_view.steps = EVENT_OPERAND_BYTE(3);
    if (xm_field_view.steps == 0) {
        xm_field_view.steps++;
        xm_field_work.camera_counter += 2;
    }
    step = -((xm_field_view.distance - xm_field_event_read_imm_or_var(1)) << 16) / xm_field_view.steps;
    xm_field_view.start = xm_field_view.distance << 16;
    xm_field_view.flags |= 1;
    xm_field_view.step = step;
    xm_field_current_event_actor->pc += 4;
}
#endif
#else
void func_8009A34C(void) {
    /* Instruction-faithful to retail 8009A34C-8009A41C: the divisor is the
     * re-read halfword at g_Scene+0x8C (1..255, never zero); the tween delta
     * `-((scale - target) << 16)` wraps as u32 (retail subu/sll/negu). */
    s32 duration;
    s32 targetAngle;
    s32 scale;
    s32 delta;

    duration = SCRIPT_READ_U8_REL(3);
    *(s16*)((u8*)&g_Scene + 0x8C) = duration;
    if (duration == 0) {
        *(s16*)((u8*)&g_Scene + 0x8C) = duration + 1;
        D_800B21D8[0] = (s32)((u32)D_800B21D8[0] + 2);
    }

    targetAngle = FieldScriptVMGetArgument(1);
    scale = *(s16*)((u8*)&g_Scene + 0x6E);
    delta = MIPS_DIV((s32)(0u - (((u32)scale - (u32)targetAngle) << 16)), *(s16*)((u8*)&g_Scene + 0x8C));
    *(s32*)((u8*)&g_Scene + 0x90) = (s32)((u32)scale << 16);
    g_Scene.unk48 |= 1;
    *(s32*)((u8*)&g_Scene + 0x94) = delta;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 4;
}
#endif /* XENO_PC_PORT */

extern s16 D_800AEA64[];

/* Start DIP (scene+0x6C) lerp to target over `duration` frames. */
void func_8009A420(s32 targetDip, s32 duration) {
    s32 curDip;
    s32 delta;

    if (duration == 0) {
        D_800B21D8[0] = 2;
        duration = 1;
    }

    curDip = *(s16*)((u8*)&g_Scene + 0x6C);
    delta = -((curDip - targetDip) << 16) / duration;
    *(s16*)((u8*)&g_Scene + 0x70) = duration;
    *(s32*)((u8*)&g_Scene + 0x74) = curDip << 16;
    g_Scene.unk48 |= 0x8;
    *(s32*)((u8*)&g_Scene + 0x78) = delta;
}

/* Opcode 0xA5 — DIP lerp wrapper: arg1 = target, byte3&0x7F = duration. */
void func_8009A490(void) {
    s32 target = FieldScriptArgument1(1, SCRIPT_READ_U8_REL(3));
    s32 duration = SCRIPT_READ_U8_REL(3) & 0x7F;

    func_8009A420(target, duration);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 4;
}

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

void func_8009AA00(void) {
#ifdef XENO_PC_PORT
    s32 idx1 = FieldScriptVMGetActorIndex(1);
    if (idx1 == 0xFF) goto advance;
    {
        s32 idx2 = FieldScriptVMGetActorIndex(2);
        if (idx2 == 0xFF) goto advance;
    }
    {
        ActorData* pModel2 = (ActorData*)(uintptr_t)
            g_FieldActors[FieldScriptVMGetActorIndex(2)].pActorData;
        ActorData* pModel1 = (ActorData*)(uintptr_t)
            g_FieldActors[FieldScriptVMGetActorIndex(1)].pActorData;
        s32 dx = *(s32*)((u8*)pModel2 + 0x28) - *(s32*)((u8*)pModel1 + 0x28);
        s32 dz = *(s32*)((u8*)pModel2 + 0x20) - *(s32*)((u8*)pModel1 + 0x20);
        s32 angle = -ratan2(dx, dz);
        angle |= 0x8000;
        if (D_800ADB1C == 0) {
            *(u16*)((u8*)pModel1 + 0x104) = (u16)angle;
            *(u16*)((u8*)pModel1 + 0x106) = (u16)angle;
            *(u16*)((u8*)pModel1 + 0x108) = (u16)angle;
        }
        *(u16*)((u8*)pModel1 + 0x104) = (u16)angle;
        *(u16*)((u8*)pModel1 + 0x106) = (u16)angle;
    }
advance:
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
#else
    if (FieldScriptVMGetActorIndex(1) != 0xFF && FieldScriptVMGetActorIndex(2) != 0xFF) {
        ActorData* target = (ActorData*)(uintptr_t)g_FieldActors[FieldScriptVMGetActorIndex(2)].pActorData;
        ActorData* self = (ActorData*)(uintptr_t)g_FieldActors[FieldScriptVMGetActorIndex(1)].pActorData;
        s16 angle = -ratan2(target->position.vz - self->position.vz,
                            target->position.vx - self->position.vx) | 0x8000;
        if (D_800ADB1C == 0) {
            self->rotation.vx = angle;
            self->rotation.vy = angle;
            self->rotation.vz = angle;
        }
        self->rotation.vx = angle;
        self->rotation.vy = angle;
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
#endif
}

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

/* Opcode 0x69 — set actor rotation from 8-way index, camera-relative. */
void func_8009AC34(void) {
    s32 directionIndex = FieldScriptVMGetArgument(2);
    s32 angle = (g_FieldAngleToDirectionLUT[directionIndex] - (u16)g_CamInterpolation.targetAngleY) & 0xFFF;

    FieldScriptSetActorRotation(angle);
}

void FieldScriptVMHandlerSetCurActorRotation(void) {
    int directionIndex = FieldScriptVMGetArgument(1);
    FieldSetCurrentActorRotation(g_FieldAngleToDirectionLUT[directionIndex]);
}

void func_8009ACB4(void) {
    int directionIndex = FieldScriptVMGetArgument(1);
    func_8009AB08(g_FieldAngleToDirectionLUT[directionIndex]);
}

extern u16 D_800AEA44[];

/* Opcode 0xAB — write camera-relative cardinal facing into actor rotation. */
void func_8009ACEC(void) {
    s32 directionIndex = SCRIPT_READ_U8_REL(1);
    s16 rotation = ((D_800AEA44[directionIndex] - (u16)g_CamInterpolation.targetAngleY) & 0xFFF) | 0x8000;

    g_FieldScriptVMCurActor->rotation.vx = rotation;
    g_FieldScriptVMCurActor->rotation.vy = rotation;
    if (D_800ADB1C == 0) {
        g_FieldScriptVMCurActor->rotation.vz = rotation;
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
}

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

/* Start SCRZ (scene+0x68) lerp to target over `duration` frames. */

void func_8009AE3C(s32 targetScrZ, s32 duration) {
#ifdef XENO_PC_PORT
    s32 curScrZ;
    s32 delta;
    s32* pFlags;

    if (duration != 0) {
        curScrZ = *(s32*)((u8*)&g_Scene + 0x68);
        delta = -((curScrZ - targetScrZ) << 16) / duration;
        pFlags = (s32*)((u8*)&g_Scene + 0x48);
        *(s16*)((u8*)&g_Scene + 0x80) = duration;
        *pFlags |= 0x10;
        *(s32*)((u8*)&g_Scene + 0x84) = curScrZ << 16;
        *(s32*)((u8*)&g_Scene + 0x88) = delta;
    } else {
        *(s32*)((u8*)&g_Scene + 0x68) = targetScrZ;
        *(s16*)((u8*)&g_Scene + 0x80) = 0;
        D_800B21D8[0] += 2;
    }

    pFlags = (s32*)((u8*)&g_Scene + 0x48);
    *pFlags &= ~0x2000;
#else
    if (duration != 0) {
        g_Scene_s16[0x80 / 2] = duration;
        g_Scene_s32[0x48 / 4] = (s32) (g_Scene_s32[0x48 / 4] | 0x10);
        g_Scene_s32[0x84 / 4] = (s32) (g_Scene_s32[0x68 / 4] << 0x10);
        g_Scene_s32[0x88 / 4] = (s32) ((s32) -((g_Scene_s32[0x68 / 4] - targetScrZ) << 0x10) / duration);
    } else {
        g_Scene_s32[0x68 / 4] = targetScrZ;
        g_Scene_s16[0x80 / 2] = 0;
        D_800B21D8[0] += 2;
    }
    g_Scene_s32[0x48 / 4] = (s32) (g_Scene_s32[0x48 / 4] & 0xDFFF);
#endif
}

extern s32 D_8005A444[];
extern s16 D_800B2348;
#ifdef XENO_PC_PORT
/* Retail passes the full target words (a0=$s3, a1=$s4 at 8009B0E0). */
extern void func_8009E574(s32 x, s32 z);
#else
extern void func_8009E574(s16 x, s16 z);
#endif

/* Retail 0x8009AEE0-0x8009B15C. Move one of the three field-party actors
 * toward an integer X/Z target. The return value is the retail poll contract:
 * -1 while ordinary movement is still in progress, 0 once the member is
 * absent, hidden, arrived, or placed through the retail stuck fallback. */
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc7", func_8009AEE0);
#else
s32 func_8009AEE0(s32 partySlot, s32 targetX, s32 targetZ, s32 direction) {
#ifdef FIELD_VM_AUDIT_MUTANT_PARTY_MOVE_ALWAYS_DONE
    return 0;
#else
    /* Instruction-faithful to retail 0x8009AEE0-0x8009B15C.  Move one of the
     * three field-party actors toward an integer X/Z target; returns -1 while
     * ordinary movement is still in progress, 0 once the member is absent,
     * hidden, arrived, or placed through the stuck fallback.  Retail `div`
     * has no break guard (moveSpeed == 0 -> MIPS_DIV); deltas wrap as u32;
     * the stuck counter compare is the signed `lh 0x6E; slti 0x41`; and
     * func_8009E574 receives the full target words (retail a0=$s3, a1=$s4). */
    s32 actorIndex;
    FieldActor* fieldActor;
    ActorData* actor;
    u8* sprite;
    s32 stepMagnitude;
    s32 distance;
    s32 delta[3];
    s32 facing;
    ActorData* savedActor;
    s32 savedActorIndex;

    actorIndex = D_8005A444[partySlot];
    if (actorIndex == ACTOR_ID_INVALID) {
        return 0;
    }

    fieldActor = &g_FieldActors[actorIndex];
    if (fieldActor->status & 0x20) {
        return 0;
    }

    sprite = (u8*)(uintptr_t)fieldActor->pSpriteData;
    actor = (ActorData*)(uintptr_t)fieldActor->pActorData;
    if (*(s32*)(sprite + 0x18) == 0) {
        *(s32*)(sprite + 0x18) = MIPS_DIV(0x04000000, actor->moveSpeed);
    }

    stepMagnitude = (s32)((u32)FieldGetVec1Magnitude(*(s32*)(sprite + 0x18) >> 15) + 1);
    delta[0] = (s32)((u32)targetX - (u32)(s16)(actor->position.vx >> 16));
    delta[1] = 0;
    delta[2] = (s32)((u32)targetZ - (u32)(s16)(actor->position.vz >> 16));
    distance = FieldGetVec2Magnitude(delta[0], delta[2]);
    actor->scriptFlags.flags |= 0x400000;

    if (stepMagnitude < distance) {
        if (actor->prevPosition.x == (s16)(actor->position.vx >> 16) &&
            actor->prevPosition.y == (s16)(actor->position.vy >> 16) &&
            actor->prevPosition.z == (s16)(actor->position.vz >> 16)) {
            actor->unk6E++;
        } else {
            actor->unk6E = 0;
        }

        facing = func_8007B694(delta);
        actor->rotation.vx = (s16)facing;
        actor->rotation.vy = (s16)facing;
        if ((s16)actor->unk6E < 0x41 && D_800B2348 == 0) {
            return -1;
        }

        /* The placement helper consumes the global current-actor context.
         * Retail temporarily lends it the party actor (index re-read from
         * D_8005A444), then restores both publications. */
        savedActor = g_FieldScriptVMCurActor;
        savedActorIndex = D_800AFD1C;
        g_FieldScriptVMCurActor = actor;
        D_800AFD1C = D_8005A444[partySlot];
        func_8009E574(targetX, targetZ);
        g_FieldScriptVMCurActor = savedActor;
        D_800AFD1C = savedActorIndex;
    }

    if ((actor->scriptFlags.flags & 0x8000) == 0) {
        if (direction == 0xFF) {
            facing = (u16)actor->rotation.vy | 0x8000;
        } else {
            facing = g_FieldAngleToDirectionLUT[direction] | 0x8000;
        }
    } else {
        facing = (u16)actor->unk11C | 0x8000;
    }
    actor->rotation.vx = (s16)facing;
    actor->rotation.vy = (s16)facing;
    actor->position.vx = (s32)((u32)targetX << 16);
    actor->position.vz = (s32)((u32)targetZ << 16);
    actor->unk6E = 0;
    actor->scriptFlags.flags &= 0xFDDFF7FF;
    return 0;
#endif
}
#endif /* XENO_PC_PORT */

extern u8 D_800B21CF[];

/* FE44 — enable player interact gate. */
void func_8009B15C(void) {
    D_800B21CF[0] = 1;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 1;
}

extern u8 D_800B21CF[];
extern u8 D_800B21CC;
extern s32 D_800B2368;
extern s32 D_800B2364;
extern s32 D_800B2360;
extern u8 D_800B21CE;
extern s32 g_PlayerActorIndex;
extern void func_80081C54(s32);

void func_8009B184(void) {
    s32 i;
    D_800B21CF[0] = 0;
    D_800B21CC = 0;
    D_800B2368 = 0;
    D_800B2364 = 0;
    D_800B2360 = 0;
    D_800B21CE = 0;
    for (i = 0; i < 0x20; i++) {
        func_80081C54(g_PlayerActorIndex);
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 1;
}

extern s32 D_8005A444[];
extern s32 func_8009AEE0(s32, s32, s32, s32);
extern u8 D_800B21CC;
extern s16 D_800B2348;
extern void func_8009B338(void);

void func_8009B210(void) {
#ifdef XENO_PC_PORT
    ActorData* pModel =
        (ActorData*)(uintptr_t)g_FieldActors[D_8005A444[0]].pActorData;
    s16 posX = *(s16*)((u8*)pModel + 0x22);
    s16 posZ = *(s16*)((u8*)pModel + 0x2A);
    s32 mask = 0;
    s32 r;
    r = func_8009AEE0(0, posX, posZ, 0xFF);
    mask |= (r == 0) ? 1 : 0;
    r = func_8009AEE0(1, posX, posZ, 0xFF);
    if (r == 0) mask |= 2;
    r = func_8009AEE0(2, posX, posZ, 0xFF);
    if (r == 0) mask |= 4;
    /* Retail writes one in the third call's branch delay slot on both paths. */
    D_800B00C0 = 1;
    if (mask == 7) {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 1;
        D_800B21CC = 0;
        D_800B2348 = 0;
        func_8009B338();
    } else {
        D_800B21CC = 1;
        g_FieldScriptVMCurActor->scriptInstructionPointer -= 1;
    }
#else
    /* Plain-pointer IP updates: as struct fields they are reordered across the
     * D_800B21CC/D_800B2348 stores (gcc 2.7.2 struct/non-struct alias rule). */
    ActorData* actor = (ActorData*)(uintptr_t)g_FieldActors[D_8005A444[0]].pActorData;
    s16 x = *(s16*)((u8*)actor + 0x22);
    s16 z = *(s16*)((u8*)actor + 0x2A);
    s32 blocked = func_8009AEE0(0, x, z, 0xFF) == 0;
    if (func_8009AEE0(1, x, z, 0xFF) == 0) {
        blocked |= 2;
    }
    if (func_8009AEE0(2, x, z, 0xFF) == 0) {
        blocked |= 4;
    }
    D_800B00C0 = 1;
    if (blocked == 7) {
        *(u16*)((u8*)g_FieldScriptVMCurActor + 0xCC) += 1;
        D_800B21CC = 0;
        D_800B2348 = 0;
        func_8009B338();
        return;
    }
    D_800B21CC = 1;
    *(u16*)((u8*)g_FieldScriptVMCurActor + 0xCC) -= 1;
#endif
}

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

/* Retail FE23, 0x8009B398-0x8009B664: move all three field-party members to
 * script-provided targets and poll until each helper reports completion. */
#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): field_event_gather_party (src/decomp/src/field/field_event.c:8019). */
#ifndef XM_S_field_event_gather_party
#define XM_S_field_event_gather_party
void xm_field_event_gather_party(void) __asm__("func_8009B398");
#endif
#ifndef XM_S_field_event_read_selected_operand_80
#define XM_S_field_event_read_selected_operand_80
s32 xm_field_event_read_selected_operand_80(s32 offset, s32 flags) __asm__("FieldScriptArgument1");
#endif
#ifndef XM_S_mode_party_actors
#define XM_S_mode_party_actors
extern s32 xm_mode_party_actors[3] __asm__("D_8005A444");
#endif
#ifndef XM_S_field_party_walk_member_toward
#define XM_S_field_party_walk_member_toward
s32 xm_field_party_walk_member_toward(s32 slot, s32 x, s32 z, s32 facing) __asm__("func_8009AEE0");
#endif
#ifndef XM_S_field_event_read_selected_operand_40
#define XM_S_field_event_read_selected_operand_40
s32 xm_field_event_read_selected_operand_40(s32 offset, s32 flags) __asm__("FieldScriptArgument2");
#endif
#ifndef XM_S_field_event_read_selected_operand_20
#define XM_S_field_event_read_selected_operand_20
s32 xm_field_event_read_selected_operand_20(s32 offset, s32 flags) __asm__("FieldScriptArgument3");
#endif
#ifndef XM_S_field_event_read_selected_operand_10
#define XM_S_field_event_read_selected_operand_10
s32 xm_field_event_read_selected_operand_10(s32 offset, s32 flags) __asm__("FieldScriptArgument4");
#endif
#ifndef XM_S_field_event_read_selected_operand_08
#define XM_S_field_event_read_selected_operand_08
s32 xm_field_event_read_selected_operand_08(s32 offset, s32 flags) __asm__("FieldScriptArgument5");
#endif
#ifndef XM_S_field_event_read_selected_operand_04
#define XM_S_field_event_read_selected_operand_04
s32 xm_field_event_read_selected_operand_04(s32 offset, s32 flags) __asm__("FieldScriptArgument6");
#endif
#ifndef XM_S_field_event_yield_requested
#define XM_S_field_event_yield_requested
extern s32 xm_field_event_yield_requested __asm__("D_800B00C0");
#endif
void func_8009B398(void) {
    xm_FieldActor *actor;
    s32 i;
    s32 near;

    if (xm_field_event_read_selected_operand_80(1, EVENT_OPERAND_BYTE(0xD)) == 0x7FFF) {
        xm_field_current_event_actor->pc += 0x14;
        xm_field_work.preserve_nonplayer_motion = 1;
        i = 0;
        do {
            if (xm_mode_party_actors[i] != 0xFF) {
                actor = xm_field_view.components.descriptors[xm_mode_party_actors[i]].actor;
                actor->heading_goal = actor->heading = actor->heading_goal | 0x8000;
            }
            i++;
        } while (i < 3);
        return;
    }
    near = xm_field_party_walk_member_toward(0, xm_field_event_read_selected_operand_80(1, EVENT_OPERAND_BYTE(0xD)), xm_field_event_read_selected_operand_40(3, EVENT_OPERAND_BYTE(0xD)),
                         xm_field_event_read_imm_or_var(0xE)) == 0;
    if (xm_field_party_walk_member_toward(1, xm_field_event_read_selected_operand_20(5, EVENT_OPERAND_BYTE(0xD)), xm_field_event_read_selected_operand_10(7, EVENT_OPERAND_BYTE(0xD)),
                      xm_field_event_read_imm_or_var(0x10)) == 0) {
        near |= 2;
    }
    if (xm_field_party_walk_member_toward(2, xm_field_event_read_selected_operand_08(9, EVENT_OPERAND_BYTE(0xD)), xm_field_event_read_selected_operand_04(0xB, EVENT_OPERAND_BYTE(0xD)),
                      xm_field_event_read_imm_or_var(0x12)) == 0) {
        near |= 4;
    }
    xm_field_event_yield_requested = 1;
    if (near == 7) {
        xm_field_work.unk2348 = 0;
        xm_field_current_event_actor->pc += 0x14;
        xm_field_work.party_processing_mode = 0;
    } else {
        xm_field_work.party_processing_mode = 1;
        xm_field_current_event_actor->pc -= 1;
    }
    xm_field_work.preserve_nonplayer_motion = 1;
}
#endif
#else
void func_8009B398(void) {
    /* Instruction-faithful to retail 8009B398-8009B660: the argument mask
     * byte is re-read before every FieldScriptArgumentN call and the three
     * arguments of each func_8009AEE0 call are evaluated into temporaries in
     * retail call order (ArgumentN, ArgumentN+1, GetArgument). */
    s32 completeMask;
    s32 targetX;
    s32 targetZ;
    s32 direction;
    s32 i;

    if (FieldScriptArgument1(1, SCRIPT_READ_U8_REL(0xD)) == 0x7FFF) {
#ifdef FIELD_VM_AUDIT_MUTANT_PARTY_GROUP_IP_1
        g_FieldScriptVMCurActor->scriptInstructionPointer += 1;
#else
        g_FieldScriptVMCurActor->scriptInstructionPointer += 0x14;
#endif
        D_800B21CE = 1;
        for (i = 0; i < 3; i++) {
            s32 actorIndex = D_8005A444[i];
            if (actorIndex != ACTOR_ID_INVALID) {
                ActorData* actor =
                    (ActorData*)(uintptr_t)g_FieldActors[actorIndex].pActorData;
                s16 facing = (s16)((u16)actor->rotation.vy | 0x8000);
                actor->rotation.vx = facing;
                actor->rotation.vy = facing;
            }
        }
        return;
    }

    targetX = FieldScriptArgument1(1, SCRIPT_READ_U8_REL(0xD));
    targetZ = FieldScriptArgument2(3, SCRIPT_READ_U8_REL(0xD));
    direction = FieldScriptVMGetArgument(0xE);
    completeMask = func_8009AEE0(0, targetX, targetZ, direction) == 0;

    targetX = FieldScriptArgument3(5, SCRIPT_READ_U8_REL(0xD));
    targetZ = FieldScriptArgument4(7, SCRIPT_READ_U8_REL(0xD));
    direction = FieldScriptVMGetArgument(0x10);
    if (func_8009AEE0(1, targetX, targetZ, direction) == 0) {
        completeMask |= 2;
    }

    targetX = FieldScriptArgument5(9, SCRIPT_READ_U8_REL(0xD));
    targetZ = FieldScriptArgument6(0xB, SCRIPT_READ_U8_REL(0xD));
    direction = FieldScriptVMGetArgument(0x12);
    if (func_8009AEE0(2, targetX, targetZ, direction) == 0) {
        completeMask |= 4;
    }

    D_800B00C0 = 1;
    if (completeMask == 7) {
        D_800B2348 = 0;
#ifdef FIELD_VM_AUDIT_MUTANT_PARTY_GROUP_IP_1
        g_FieldScriptVMCurActor->scriptInstructionPointer += 1;
#else
        g_FieldScriptVMCurActor->scriptInstructionPointer += 0x14;
#endif
        D_800B21CC = 0;
    } else {
        D_800B21CC = 1;
        g_FieldScriptVMCurActor->scriptInstructionPointer -= 1;
    }
    D_800B21CE = 1;
}
#endif /* XENO_PC_PORT */

void func_8009B664(void) {
    FieldScriptMemoryWriteU16(SCRIPT_IMM_ARG(1), g_Scene.sceneScrZ);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void func_8009B6AC(void) {
    func_8009AE3C(FieldScriptVMGetArgument(1), FieldScriptVMGetArgument(3));
    g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
}

/* Start yaw tween toward a relative 8-way facing over `duration` frames. */
void func_8009B708(s32 facingOffset, s32 duration) {
    s32 direction;
    s32 delta;
    s32 tableIndex;

    direction = FieldGetCameraDirection() & 0xFFFF;
    if (duration == 0) {
        D_800B21D8[0] += 2;
        duration = 1;
    }

    tableIndex = (direction << 3) + facingOffset;
    delta = ((s32)D_800AEA64[tableIndex] << 25) / duration;
    *(s16*)((u8*)&g_Scene + 0x66) = duration;
    *(s32*)((u8*)&g_Scene + 0x7C) = ((facingOffset + 4) & 7) << 9;
    *(s32*)((u8*)&g_Scene + 0x5C) = delta;
}

/* Nudge yaw ±0x200 (45°) over `duration` frames. a0==0 → +0x200, else −0x200. */
#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from ex-machina (MIT, github.com/brianblakely/xenogears-ex-machina @20011c4): field_camera_turn_one_octant (src/decomp/src/field/field_event.c:8096). */
#ifndef XM_T_FadeChannel
#define XM_T_FadeChannel
typedef struct {
    DR_MODE modes[2]; 
    TILE tiles[2];   
    s32 level[3];
    s32 step[3];
    u16 abr;
    s16 active;
    s16 steps;
    u16 pad;
} FadeChannel;
#endif
#ifndef XM_T_FieldWork
#define XM_T_FieldWork
typedef struct FieldWork {
    s16 unk2078;               
    s16 unk207A;               
    s16 effect_steps;          
    u8 unk207E[2];
    s32 effect_value[6];       
    s32 effect_step[6];        
    s16 unk20B0[2];            
    void *effect_buffers[4];   
    FadeChannel fades[2];      
    u16 open_windows;          

    s16 encounter_inhibition;  
    s16 terrain_angle;         
    u16 input_mask;            
    s32 unk217C;               
    s32 unk2180;               
    SVECTOR sprite_angles;     
    s16 scale;                 
    s16 sprite_gate;           
    u8 fog_color[4];           
    u8 far_color[4];           
    s16 fog_range[2];          
    u8 clear_color[3];         
    u8 party_bits;             
    s16 unk21A0[6];            
    s16 emitter_range;         
    s16 piece_drift[3];        
    s16 unk21B4;               
    u8 unk21B6[2];
    s32 last_sound_effect;     
    s32 unk21BC[3];            
    u8 unk21C8[0x21CC - 0x21C8];
    u8 party_processing_mode;  
    u8 camera_floor_fixed;     
    u8 preserve_nonplayer_motion; 
    u8 forced_position;        
    u8 script_control[2];      
    u8 piece_drift_mode;       
    u8 unk21D3;
    s16 unk21D4;               
    s16 text_speed;            
    s32 camera_counter;        
    s16 unk21DC[4];            
    s16 unk21E4[4];            
    SVECTOR layer_positions[4]; 
    s32 layer_scales[4];       
    s16 unk221C[5][3];         
    u8 unk223A[2];
    s16 unk223C[3][3];         
    u8 unk224E[0x225C - 0x224E];
    u8 unk225C[3];             
    u8 unk225F[0x2264 - 0x225F]; 
    s32 unk2264;               
    s32 unk2268;               
    s32 controlled;            
    u16 encounter_music[16];   
    s16 battle_music;          
    u8 unk2292[2];
    s32 unk2294;               
    s32 unk2298;               
    s32 unk229C;               
    u16 unk22A0[32];           
    s16 unk22E0;               
    s16 emitter_descriptor[3]; 
    s16 unk22E8[3][4];         
    s16 unk2300[3][4];         
    s32 unk2318[3];            
    s16 emitter_position[3][4]; 
    u16 effects_kept;          
    u16 unk233E;               
    s16 repeat_delay;          
    s16 repeat_remaining;      
    s16 jump_mode;             
    s16 animation_mode;        
    u16 unk2348;               
    s16 unk234A;               
    s16 gear_riding_lock_override; 
    s16 followers_idle;        
    u32 unk2350;               
    u8 unk2354;                
    u8 unk2355;                
    u8 unk2356;                
    u8 unk2357;                
    u8 unk2358;                
    u8 unk2359[0x235C - 0x2359];
} FieldWork;
#endif
#ifndef XM_T_SavedView
#define XM_T_SavedView
typedef struct {
    s16 octant;
    s32 projection;
    s16 elevation;
} SavedView;
#endif
#ifndef XM_T_ModelLight
#define XM_T_ModelLight
typedef struct {
    s32 vx, vy, vz;
    u16 r, g, b;
} ModelLight;
#endif
#ifndef XM_T_MorphDelta
#define XM_T_MorphDelta
typedef struct {
    s16 dx, dy, dz;
    s16 index;
} MorphDelta;
#endif
#ifndef XM_T_MorphTarget
#define XM_T_MorphTarget
typedef struct {
    s32 count;
    void *deltas;
    MorphDelta *normals;
} MorphTarget;
#endif
#ifndef XM_T_MorphTable
#define XM_T_MorphTable
typedef struct {
    s32 count;
    MorphTarget targets[1];
} MorphTable;
#endif
#ifndef XM_T_SpriteModel
#define XM_T_SpriteModel
typedef struct {
    u16 flags;          
    u16 vertex_count;
    u16 primitive_count;
    u16 group_count;    
    SVECTOR *vertices;
    SVECTOR *normals;
    u8 *unk10;          
    u8 *unk14;
    u8 *unk18;
    MorphTable *morphs; 
    SVECTOR box_min;    
    SVECTOR box_max;
    s32 aux_size;       
    s32 packet_size;    
} SpriteModel;
#endif
#ifndef XM_T_MorphChannel
#define XM_T_MorphChannel
typedef struct MorphChannel {
    s32 (*update)(struct MorphChannel *channel);
    s32 target;
    s32 weight;
    s32 step;
    u8 unk10[0x10];
} MorphChannel;
#endif
#ifndef XM_T_MorphState
#define XM_T_MorphState
typedef struct {
    SpriteModel *model;
    SVECTOR *vertices;
    SVECTOR *normals;
    s32 count;
    MorphChannel *channels;
} MorphState;
#endif
#ifndef XM_T_FieldInstance
#define XM_T_FieldInstance
typedef struct {
    u8 unk00[4];
    SpriteModel *mesh; 
    void *packets[2]; 
    u8 unk10[2];
    s16 mode;        
    MorphState *anims; 
    s16 center[3];   
    s16 unk1E;
    s16 radius;      
} FieldInstance;
#endif
#ifndef XM_T_SpritePart
#define XM_T_SpritePart
typedef struct {
    s16 x, y;              
    u8 u, v;               
    u8 w, h;               
    u8 byte8, byte9;       
    u16 tpage;             
    u16 clut;              
    u8 unknowne[2];
    u32 colour;            
    u32 flags;             
} SpritePart;
#endif
#ifndef XM_T_SpriteRendererEntry
#define XM_T_SpriteRendererEntry
typedef struct {
    s8 byte0;              
    s8 byte1;              
    s16 half2;
    s16 half4;
    s16 half6;
} SpriteRendererEntry;
#endif
#ifndef XM_T_SpriteOffset
#define XM_T_SpriteOffset
typedef struct {
    s8 x, y;
} SpriteOffset;
#endif
#ifndef XM_T_SpriteRenderer
#define XM_T_SpriteRenderer
typedef struct {
    s16 angle_x, angle_y, angle_z; 
    s16 scale_x, scale_y, scale_z; 
    MATRIX matrix;                 
    SpritePart *parts[2];          
    SpriteRendererEntry *pointer34; 
    struct Sprite *next_pending;   
    SpriteOffset offset;           
    u8 unknown3e[2];
    s32 word40;                    
    SVECTOR light_angles;          
    u16 light_colour[3];           
} SpriteRenderer;
#endif
#ifndef XM_T_Sprite
#define XM_T_Sprite
typedef struct Sprite {
    s32 x, y, z;                
    s32 speed_x, speed_y, speed_z; 
    s32 speed;                  
    s32 gravity;                
    SpriteRenderer *renderer;   
    void *image;                
    u8 red, green, blue;     
    u8 colour_flags;         
    s16 scale;               
    s16 depth;               
    s16 half30;              
    s16 direction;           
    u16 frame;               
    u16 height;              
    u16 extent_depth;        
    u16 rate;                
    union {
        u32 word;
        struct {
        unsigned sides : 2;      
        unsigned unknown2 : 1;
        unsigned flip : 1;       
        unsigned flip_y : 1;
        unsigned blend : 3;      
        unsigned unknown8 : 8;
        unsigned field16 : 4;
        unsigned mode : 4;       
        unsigned no_view : 1;    
        unsigned unknown25 : 3;
        unsigned dirty : 1;      
        unsigned unknown29 : 3;
        } bits;
        u8 bytes[4];         
    } render;                
    u32 flags;               
    s32 *resource_block;     
    s32 *animations;         
    s32 resource;            
    s32 word50;              
    u16 *frame_table;        
    u16 *animation;          
    u16 *facings;            
    u16 *word60;             
    u8 *script;              
    void (*callback)(struct Sprite *sprite); 
    void *block;             
    struct Sprite *parent;   
    struct Sprite *partner;  
    s32 word78;              
    void *sequencer;         
    u16 word80;              
    u16 word82;              
    s16 ground;              
    u16 size;                
    u8 *frames;              
    s8 stack_top;            
    u8 unknown8d;
    u8 stack[0x10];          
    s16 countdown;           
    s16 target_x, target_y, target_z; 
    u8 unknowna6[2];
    struct {
        unsigned sequencer_owned : 1; 
        unsigned bounce : 10;    
        unsigned frame : 6;      
        unsigned step : 3;       
        unsigned phase : 2;      
        unsigned field22 : 6;    
        unsigned field28 : 2;
        unsigned unknown30 : 2;
    } frame_bits;            
    union {
        u32 word;
        struct {
        unsigned unknown0 : 2;
        unsigned mirror : 1;     
        unsigned frame_flip : 1; 
        unsigned unknown4 : 1;
        unsigned owns_children : 1; 
        unsigned double_step : 1;
        unsigned divisor : 12;   
        unsigned unknown19 : 13;
        } bits;
        u8 bytes[4];
    } motion;                
    union {
        u32 wordb0;          
        u8 byteb0;
        struct {
            unsigned unknown0 : 8;
            unsigned passive_children : 1; 
            unsigned share_rate : 1;       
            unsigned unknown10 : 22;
        } bits;
    } b0;                    
} Sprite;
#endif
#ifndef XM_T_FieldMarker
#define XM_T_FieldMarker
typedef struct FieldMarker {
    SVECTOR v[4];
    POLY_FT4 poly[2];
} FieldMarker;
#endif
#ifndef XM_T_FieldDescriptor
#define XM_T_FieldDescriptor
typedef struct FieldDescriptor {
    FieldInstance *instance; 
    Sprite *model;           
    struct FieldMarker *shadow; 
    MATRIX matrix;           
    MATRIX transform;        
    FieldActor *actor;       
    SVECTOR rotation;        
    u16 flags;               
    u16 unk5A;               
} FieldDescriptor;
#endif
#ifndef XM_T_Attribute
#define XM_T_Attribute
typedef union {
    u32 word;
    u8 bytes[4];
} Attribute;
#endif
#ifndef XM_T_CollisionTriangle
#define XM_T_CollisionTriangle
typedef struct {
    s16 unk00[6];
    u8 attribute;
    u8 unk0D;
} CollisionTriangle;
#endif
#ifndef XM_T_FieldComponents
#define XM_T_FieldComponents
typedef struct {
    s32 descriptor_count;                      
    FieldDescriptor *descriptors;              
    s32 *geometry;                             
    s32 *collision;                            
    s32 *sprites;                              
    Attribute *collision_attributes;           
    CollisionTriangle *collision_triangles[4]; 
    void *collision_vertices[4];               
    s32 triangle_counts[4];                    
    s16 layer_count;                           
} FieldComponents;
#endif
#ifndef XM_T_FieldView
#define XM_T_FieldView
typedef struct FieldView {
    VECTOR eye;              
    VECTOR target;           
    VECTOR up;               
    VECTOR eye_goal;         
    VECTOR target_goal;      
    VECTOR unk050;           
    VECTOR shake_offset;     
    VECTOR saved_target;     
    VECTOR point_actor_a;    
    VECTOR saved_eye;        
    VECTOR point_actor_b;    
    s32 scripted_zoom;       
    s16 mode;                
    s16 scripted_elevation;  
    s16 scripted_heading;    
    s16 scripted_scale;      
    u16 scripted;            
    s16 target_steps;        
    VECTOR scripted_target;  
    VECTOR target_step;      
    s16 eye_steps;           
    s32 scripted_eye[3];     
    s32 unk0F0;              
    s32 eye_step[3];         
    s32 unk100;              
    s32 target_a;            
    s32 target_b;            
    s16 angle;               
    s16 view_angle;          
    MATRIX previous_view;    
    MATRIX orbit;            
    SVECTOR orbit_angles;    
    u32 flags;               
    s16 bounds[4];           
    SVECTOR heading_angles;  
    s32 heading_velocity;    
    s32 heading_high;        
    u8 heading_blocks[2];    
    s16 heading_steps;       
    s32 projection;          
    u16 elevation;           
    s16 distance;            
    s16 elevation_steps;     
    s32 elevation_value;     
    s32 elevation_step;      
    u32 heading;             
    s16 projection_steps;    
    s32 projection_value;    
    s32 projection_step;     
    s16 steps;               
    s32 start;               
    s32 step;                
    s16 shake;               
    s16 shake_time;          
    s16 shake_stop;          
    s32 shake_amplitude[3];  
    s32 shake_step[3];       
    SavedView saved_view;    
    SVECTOR world_angles;    
    SVECTOR anchor;          
    MATRIX scaled_world;     
    MATRIX unk204;           
    MATRIX world_matrix;     
    s32 scale;               
    ModelLight lights[3];    
    s16 back_color[3];       
    u8 unk28A[2];
    FieldComponents components; 
} FieldView;
#endif
#ifndef XM_S_field_camera_turn_one_octant
#define XM_S_field_camera_turn_one_octant
void xm_field_camera_turn_one_octant(s32 direction, s32 steps) __asm__("func_8009B7A8");
#endif
void func_8009B7A8(s32 direction, s32 steps) {
    s32 velocity;
    s32 heading;

    if (steps == 0) {
        steps = 1;
        xm_field_work.camera_counter += 2;
    }
    if (direction == 0) {
        xm_field_view.heading_velocity = 0x2000000 / steps;
        xm_field_view.heading += 0x200;
    } else {
        xm_field_view.heading_velocity = (s32)0xFE000000 / steps;
        xm_field_view.heading -= 0x200;
    }
    xm_field_view.heading_steps = steps;
}
#endif
#else
void func_8009B7A8(s32 negative, s32 duration) {
    /* Retail 8009B7A8-8009B820.  duration is never zero at the div (retail
     * substitutes 1) and the dividends are +-0x2000000, so the quotient is
     * defined; the g_Scene+0x7C target wraps as u32 (retail addiu). */
    s32 delta;
    s32 target;

    if (duration == 0) {
        D_800B21D8[0] = (s32)((u32)D_800B21D8[0] + 2);
        duration = 1;
    }

    if (negative == 0) {
        delta = 0x2000000 / duration;
        target = (s32)(*(u32*)((u8*)&g_Scene + 0x7C) + 0x200);
    } else {
        delta = (s32)0xFE000000 / duration;
        target = (s32)(*(u32*)((u8*)&g_Scene + 0x7C) - 0x200);
    }

    *(s32*)((u8*)&g_Scene + 0x7C) = target;
    *(s32*)((u8*)&g_Scene + 0x5C) = delta;
    *(s16*)((u8*)&g_Scene + 0x66) = duration;
}
#endif /* XENO_PC_PORT */

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

/* Opcode 0xB6 — scripted camera yaw to 8-way facing (instant if D_800ADB1C==0). */

void func_8009B8E4(void) {
#ifdef XENO_PC_PORT
    s32 facing = FieldScriptVMGetArgument(1);
    s32 duration = FieldScriptVMGetArgument(3);

    if (D_800ADB1C == 0) {
        s32 angle = ((facing + 4) & 7) << 9;
        *(s16*)((u8*)&g_Scene + 0x56) = angle;
        *(s32*)((u8*)&g_Scene + 0x7C) = angle;
        g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
        D_800B00C0 = 1;
    } else if (*(s16*)((u8*)&g_Scene + 0x66) == 0) {
        func_8009B708(facing, duration);
        g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
        D_800B00C0 = 1;
    } else {
        D_800B00C0 = 1;
    }
#else
    s32 temp_a1;
    s32 temp_s0;
    s16 temp_v0;

    temp_s0 = FieldScriptVMGetArgument(1);
    temp_a1 = FieldScriptVMGetArgument(3);
    if (D_800ADB1C == 0) {
        temp_v0 = (temp_s0 + 4) & 7;
        g_Scene_s16[0x56 / 2] = (s16) (temp_v0 << 9);
        g_Scene_s32[0x7C / 4] = (s32) (temp_v0 << 9);
        g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
    } else if (g_Scene_s16[0x66 / 2] == 0) {
        func_8009B708(temp_s0, temp_a1);
        *(u16 *)((s8*)(g_FieldScriptVMCurActor) + 0xCC) = (u16) (*(u16 *)((s8*)(g_FieldScriptVMCurActor) + 0xCC) + 5);
    }
    D_800B00C0 = 1;
#endif
}

void func_8009B9A0(void) {
    if (g_Scene_s16[0x66 / 2] == 0) {
        g_Scene_s16[0xB8 / 2] = FieldGetCameraDirection();
        g_Scene_s32[0xBC / 4] = g_Scene.sceneScrZ;
        g_Scene_u16[0xC0 / 2] = g_Scene.sceneDIP;
        g_FieldScriptVMCurActor->scriptInstructionPointer += 1;
    }
}

/* Opcode 0xA0 — restore camera yaw/SCRZ/DIP saved by func_8009B9A0 over 0x20 frames. */
void func_8009BA0C(void) {
    if (*(s16*)((u8*)&g_Scene + 0x66) == 0) {
        func_8009B708(*(s16*)((u8*)&g_Scene + 0xB8), 0x20);
        func_8009AE3C(*(s32*)((u8*)&g_Scene + 0xBC), 0x20);
        func_8009A420(*(s16*)((u8*)&g_Scene + 0xC0), 0x20);
        g_FieldScriptVMCurActor->scriptInstructionPointer += 1;
    }
}

void func_8009BA7C(void) {
#ifdef XENO_PC_PORT
    s32 angle;
    s32 scrZ;

    *(s16*)((u8*)&g_Scene + 0x6C) = FieldScriptVMGetArgument(3);

    angle = ((FieldScriptVMGetArgument(1) + 4) & 7) << 9;
    *(s32*)((u8*)&g_Scene + 0x7C) = angle;
    *(s16*)((u8*)&g_Scene + 0x56) = angle;
    *(s32*)((u8*)&g_Scene + 0x60) = angle << 16;

    scrZ = FieldScriptVMGetArgument(5);
    *(s32*)((u8*)&g_Scene + 0x68) = scrZ;
    SetGeomScreen(scrZ);

    g_FieldScriptVMCurActor->scriptInstructionPointer += 7;
#else
    s32 t;
    s16 angle;
    s32 scrZ;

    *(s16*)((u8*)&g_Scene + 0x6C) = FieldScriptVMGetArgument(3);

    t = (FieldScriptVMGetArgument(1) + 4) & 7;
    angle = t << 9;
    *(s32*)((u8*)&g_Scene + 0x7C) = angle;
    *(s16*)((u8*)&g_Scene + 0x56) = t << 9;
    *(s32*)((u8*)&g_Scene + 0x60) = angle << 16;

    scrZ = FieldScriptVMGetArgument(5);
    *(s32*)((u8*)&g_Scene + 0x68) = scrZ;
    SetGeomScreen(scrZ);

    g_FieldScriptVMCurActor->scriptInstructionPointer += 7;
#endif
}
