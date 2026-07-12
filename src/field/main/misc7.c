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
extern s32 D_800B21D8;
extern s16 D_800AFB54;
extern s16 func_8007B1C4(s16 x, s16 z, s32 walkmeshId, s16* out, s32* state);
extern int FieldScriptArgument3(int index, int mask);
extern int FieldScriptArgument4(int index, int mask);
long FieldGetVec3Magnitude(long x, long y, long z);
long FieldGetVec2Magnitude(long x, long y);
s32 func_80099AC0(s32 useStoredAngle);
extern s32 func_80097A50(s32 targetValue);
extern s32 func_8007B694(s32* arg0);

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

/* VM opcode 0x55: set the current script's movement target to another actor.
 * The target actor index is the instruction byte at +1; its integer position
 * becomes the current actor's movement target before func_80097A50 advances
 * the interpolation. */
void func_800975C0(void) {
    ActorData* actor = g_FieldScriptVMCurActor;
    ActorScriptSlot* slot = &actor->scripts[actor->curScriptIndex];
    ActorData* target;

    slot->flags_0x17 = 2;

    target = (ActorData*)(uintptr_t)g_FieldActors[SCRIPT_READ_U8_REL(1)].pActorData;
    actor->unkD0.vx = target->position.vx >> 16;
    actor->unkD0.vz = target->position.vz >> 16;
    actor->unkD0.vy = target->position.vy >> 16;

    slot->flags_0 = 0xFFFF;
    if (func_80097A50(-1) == 0) {
        actor->scriptInstructionPointer += 5;
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

INCLUDE_ASM("asm/field/nonmatchings/main/misc7", func_800977A4);

INCLUDE_ASM("asm/field/nonmatchings/main/misc7", func_80097864);

INCLUDE_ASM("asm/field/nonmatchings/main/misc7", func_80097954);

INCLUDE_ASM("asm/field/nonmatchings/main/misc7", func_800979F0);

INCLUDE_ASM("asm/field/nonmatchings/main/misc7", func_80097A50);

/* asm 80098038-800980F8, opcode 0x53. Unlike 0x52, the caller supplies the
 * countdown seed and the useStoredAngle flag as an explicit script argument
 * rather than the fixed 0xFFFF sentinel. */
void func_80098038(void) {
    ActorData* actor = g_FieldScriptVMCurActor;
    ActorScriptSlot* slot = &actor->scripts[actor->curScriptIndex];

    slot->flags_0x17 = 2;

    if (slot->flags_0 == 0xFFFF) {
        slot->flags_0 = FieldScriptVMGetArgument(2);
    }

    if (func_80099AC0(FieldScriptVMGetArgument(2)) == 0) {
        actor->scriptInstructionPointer += 4;
    }
}

/* asm 800980FC-80098180, opcode 0x52. Thin wrapper: always (re)start the
 * per-script movement countdown at the 0xFFFF sentinel and wait via
 * func_80099AC0(0xFFFF) -- do not implement against the stub, which returns
 * 0 unconditionally and would fake instant movement completion. */
void func_800980FC(void) {
    ActorData* actor = g_FieldScriptVMCurActor;
    ActorScriptSlot* slot = &actor->scripts[actor->curScriptIndex];

    slot->flags_0x17 = 2;
    slot->flags_0 = 0xFFFF;

    if (func_80099AC0(0xFFFF) == 0) {
        actor->scriptInstructionPointer += 2;
    }
}

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
    pSprite = (SpriteData*)(uintptr_t)pFieldActor[D_800AFD1C].pSpriteData;
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
void func_80099214(void) {
    FieldActor* fieldActor;
    SpriteData* sprite;
    ActorData* actor;
    u8 modeByte;
    u8 mode;
    u8 argMask;
    s32 targetX;
    s32 targetZ;
    s32 targetY;
    s32 stepCount;
    s32 walkmeshId;
    s32 stepVy;
    s32 dx;
    s32 dz;
    s32 i;
    s32 state[4][4];
    s16 out[4][4];
    s16* outRow;
    s32 negDuration;
    s32 yDelta;
    s32 tmp;

    fieldActor = &g_FieldActors[D_800AFD1C];
    sprite = (SpriteData*)(uintptr_t)fieldActor->pSpriteData;
    actor = g_FieldScriptVMCurActor;

    actor->scriptFlags.flags |= 0x10000;
    modeByte = SCRIPT_READ_U8_REL(1);
    mode = modeByte & 3;

    if (mode == 1) {
        /* Absolute X/Z + speed → step count from XZ distance. */
        argMask = SCRIPT_READ_U8_REL(0xA);
        targetX = FieldScriptArgument1(2, argMask);
        argMask = SCRIPT_READ_U8_REL(0xA);
        targetZ = FieldScriptArgument2(4, argMask);
        dx = (targetX << 16) - actor->position.vx;
        dz = (targetZ << 16) - actor->position.vz;
        argMask = SCRIPT_READ_U8_REL(0xA);
        stepCount = FieldScriptArgument4(8, argMask);
        stepCount = FieldGetVec2Magnitude(dx >> 16, dz >> 16) / stepCount;
        goto shared_init;
    }

    if (mode >= 2) {
        if (mode == 2) {
            /* Gravity arc: step count from SquareRoot0 chain; shared_init
             * re-reads targets and recomputes step.vy. */
            argMask = SCRIPT_READ_U8_REL(0xA);
            FieldScriptArgument1(2, argMask);
            argMask = SCRIPT_READ_U8_REL(0xA);
            FieldScriptArgument2(4, argMask);
            argMask = SCRIPT_READ_U8_REL(0xA);
            targetY = FieldScriptArgument3(6, argMask);
            argMask = SCRIPT_READ_U8_REL(0xA);
            negDuration = -FieldScriptArgument4(8, argMask);

            /* asm: lh gravity@+0x1E (high half), SquareRoot0(hi * -2*arg4),
             * stash -(sqrt<<16) into step.vy (overwritten by shared_init),
             * discard SquareRoot0(-arg4), stepCount = SquareRoot0(|-arg4 - dy|). */
            tmp = (s16)(sprite->gravity >> 16);
            yDelta = (targetY << 16) - actor->position.vy;
            tmp = SquareRoot0(tmp * (negDuration << 1));
            sprite->step.y = -(tmp << 16);
            SquareRoot0(negDuration);

            tmp = negDuration - (yDelta >> 16);
            if (tmp < 0) {
                tmp = -tmp;
            }
            stepCount = SquareRoot0(tmp);
            if (stepCount < 0) {
                stepCount = -stepCount;
            }
            goto shared_init;
        }

        if (mode == 3) {
            if (modeByte == 0xF) {
                /* Cleanup: refresh walkmesh triangle ids, clear jump flags. */
                for (i = 0; i < D_800AFB54 - 1; i++) {
                    actor->walkmeshTriIds[i] = func_8007B1C4(
                        (s16)(actor->position.vx >> 16),
                        (s16)(actor->position.vz >> 16),
                        i,
                        out[i],
                        state[i]
                    );
                }
                actor->scriptFlags.flags &= ~0x10000;
                actor->flags &= ~0x200000;
                actor->scriptInstructionPointer += 2;
                D_800B00C0 = 1;
                return;
            }

            /* Continue / complete tick. */
            if (actor->unk102 < (s16)actor->unkE0) {
                actor->position.vx += actor->unkD0.vx;
                actor->position.vz += actor->unkD0.vz;
                actor->position.vy += sprite->step.y;
                sprite->step.y += sprite->gravity;

                if (actor->unkD0.vx != 0 || actor->unkD0.vz != 0) {
                    if (!(actor->scriptFlags.flags & 0x8000)) {
                        tmp = func_8007B694((s32*)&actor->unkD0) | 0x8000;
                        actor->rotation.vx = tmp;
                        actor->rotation.vy = tmp;
                    }
                }
            } else {
                /* Rewind to the paired init opcode to re-read snap targets. */
                actor->scriptInstructionPointer -= 0xB;

                argMask = SCRIPT_READ_U8_REL(0xA);
                targetX = FieldScriptArgument1(2, argMask);
                argMask = SCRIPT_READ_U8_REL(0xA);
                targetZ = FieldScriptArgument2(4, argMask);

                if (SCRIPT_READ_U8_REL(1) & 0x80) {
                    argMask = SCRIPT_READ_U8_REL(0xA);
                    walkmeshId = FieldScriptArgument3(6, argMask);
                    outRow = out[walkmeshId];
                    actor->walkmeshTriIds[walkmeshId] = func_8007B1C4(
                        targetX,
                        targetZ,
                        walkmeshId,
                        outRow,
                        state[walkmeshId]
                    );
                    targetY = outRow[1];
                } else {
                    argMask = SCRIPT_READ_U8_REL(0xA);
                    targetY = FieldScriptArgument3(6, argMask);
                }

                sprite->step.y = 0;
                actor->position.vx = targetX << 16;
                actor->position.vy = targetY << 16;
                actor->position.vz = targetZ << 16;
                actor->scriptFlags.flags &= ~0x10000;
                actor->flags &= ~0x200000;
                actor->scriptInstructionPointer += 0xD;
            }

            /* Sync field actor transform + sprite position; bump step index. */
            fieldActor->transformMatrix.t[0] = actor->position.vx >> 16;
            fieldActor->transformMatrix.t[1] = actor->position.vy >> 16;
            fieldActor->transformMatrix.t[2] = actor->position.vz >> 16;
            sprite->position.x = actor->position.vx;
            sprite->position.y = actor->position.vy;
            sprite->position.z = actor->position.vz;
            actor->unk102++;
            D_800B00C0 = 1;
            return;
        }

        D_800B00C0 = 1;
        return;
    }

    if (mode != 0) {
        D_800B00C0 = 1;
        return;
    }

    /* Mode 0: step count from arg4. */
    argMask = SCRIPT_READ_U8_REL(0xA);
    stepCount = FieldScriptArgument4(8, argMask);

shared_init:
    if (stepCount == 0) {
        stepCount = 1;
    }

    argMask = SCRIPT_READ_U8_REL(0xA);
    targetX = FieldScriptArgument1(2, argMask);
    argMask = SCRIPT_READ_U8_REL(0xA);
    targetZ = FieldScriptArgument2(4, argMask);

    if (SCRIPT_READ_U8_REL(1) & 0x80) {
        argMask = SCRIPT_READ_U8_REL(0xA);
        walkmeshId = FieldScriptArgument3(6, argMask);
        outRow = out[walkmeshId];
        func_8007B1C4(targetX, targetZ, walkmeshId, outRow, state[walkmeshId]);
        targetY = outRow[1];
        actor->walkmeshId = walkmeshId;
    } else {
        argMask = SCRIPT_READ_U8_REL(0xA);
        targetY = FieldScriptArgument3(6, argMask);
    }

    /* Initial vertical velocity: -gravity*steps/2, then add Y lerp term. */
    stepVy = (s32)sprite->gravity * stepCount;
    stepVy = (stepVy + ((u32)stepVy >> 31)) >> 1;
    stepVy = -stepVy;
    sprite->step.y = stepVy;
    sprite->step.y = stepVy + ((targetY << 16) - actor->position.vy) / stepCount;

    actor->unkD0.vy = 0;
    actor->unkE0 = stepCount;
    actor->unk102 = 0;
    actor->scriptInstructionPointer += 0xB;
    actor->unkD0.vx = ((targetX << 16) - actor->position.vx) / (stepCount + 1);
    actor->unkD0.vz = ((targetZ << 16) - actor->position.vz) / (stepCount + 1);
    D_800B00C0 = 1;
}

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

extern u32 FieldScriptVMGetActorIndex(int bytecodeOffset);
extern int FieldScriptArgument1(int index, int mask);
extern int FieldScriptArgument2(int index, int mask);

/*
 * asm 80099AC0-80099EF4. Movement-toward-target tick: advances a per-script
 * "steps remaining" counter (scripts[curScriptIndex].flags_0, a 16-bit
 * countdown reusing the ActorScriptSlot bitfield word at +0x90 -- verified
 * against func_80099980's asm, which writes the identical field at the same
 * offset) while g_FieldScriptVMCurActor closes on a target point selected by
 * scripts[curScriptIndex].flags_0x17 (0=relative to unkD0, 1=another actor,
 * 2=random point on a circle, 3=absolute). Returns -1 while still
 * approaching (and holds the VM via D_800B00C0=1), 0 once arrived (or if a
 * case-1 target actor index is invalid).
 */
s32 func_80099AC0(s32 useStoredAngle) {
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
        targetX = FieldScriptArgument1(ARG(1), SCRIPT_READ_U8_REL(5)) + actor->unkD0.vx;
        targetZ = FieldScriptArgument2(ARG(2), SCRIPT_READ_U8_REL(5)) + actor->unkD0.vz;
        break;

    case 1: {
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
        break;
    }

    case 2: {
        s32 angle = FieldScriptVMGetArgument(1) & 0xFFF;
        targetX = actor->unkD0.vx + ((rsin(angle) << 12) >> 12);
        targetZ = actor->unkD0.vz - ((rcos(angle) << 12) >> 12);
        break;
    }

    case 3:
        targetX = FieldScriptArgument1(ARG(1), SCRIPT_READ_U8_REL(5));
        targetZ = FieldScriptArgument2(ARG(2), SCRIPT_READ_U8_REL(5));
        break;
    }

    dx = targetX - selfX;
    dz = targetZ - selfZ;
    distance = FieldGetVec2Magnitude(dx, dz);
    deltaVec[0] = dx;
    deltaVec[1] = 0;
    deltaVec[2] = dz;

    actor->scriptFlags.flags |= 0x400000;

    if (slot->flags_0 != 0 && stepMagnitude + combinedSolidRange < distance) {
        /* asm 80099E9C: still approaching -- tick the countdown, face the
         * target, and hold the VM (D_800B00C0=1) instead of advancing IP. */
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

void func_8009A34C(void) {
    s32 duration;
    s32 targetAngle;

    duration = SCRIPT_READ_U8_REL(3);
    *(s16*)((u8*)&g_Scene + 0x8C) = duration;
    if (duration == 0) {
        duration = 1;
        *(s16*)((u8*)&g_Scene + 0x8C) = duration;
        D_800B21D8 += 2;
    }

    targetAngle = FieldScriptVMGetArgument(1);
    *(s32*)((u8*)&g_Scene + 0x90) = *(s16*)((u8*)&g_Scene + 0x6E) << 16;
    g_Scene.unk48 |= 1;
    *(s32*)((u8*)&g_Scene + 0x94) = -((*(s16*)((u8*)&g_Scene + 0x6E) - targetAngle) << 16) / duration;

    g_FieldScriptVMCurActor->scriptInstructionPointer += 4;
}

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

void func_8009BA7C(void) {
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
}
