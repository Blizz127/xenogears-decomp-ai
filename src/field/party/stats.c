#include "common.h"
#include "main/game.h"
#include "field/main.h"
#include "field/actor.h"
#include "field/script_vm.h"

void FieldPartyMemberIncreaseHp(int i, int amount) {
    g_pGameState->characters[g_GamePartyMembers[i]].hp += amount;
    if (g_pGameState->characters[g_GamePartyMembers[i]].maxHp < g_pGameState->characters[g_GamePartyMembers[i]].hp) {
        g_pGameState->characters[g_GamePartyMembers[i]].hp = g_pGameState->characters[g_GamePartyMembers[i]].maxHp;
    }
}

void FieldPartyMemberDecreaseHp(int partyMemberIndex, int amount) {
    int newHp;
    int characterId = g_GamePartyMembers[partyMemberIndex];
    newHp = g_pGameState->characters[characterId].hp - amount;
    if (newHp <= 0) {
        newHp = 1;
    }
    g_pGameState->characters[characterId].hp = newHp;
}

void FieldPartyMemberIncreaseMp(int i, int amount) {
    g_pGameState->characters[g_GamePartyMembers[i]].mp += amount;
    if (g_pGameState->characters[g_GamePartyMembers[i]].maxMp < g_pGameState->characters[g_GamePartyMembers[i]].mp) {
        g_pGameState->characters[g_GamePartyMembers[i]].mp = g_pGameState->characters[g_GamePartyMembers[i]].maxMp;
    }
}

void FieldPartyMemberDecreaseMp(int partyMemberIndex, int amount) {
    int newMp;
    int characterId = g_GamePartyMembers[partyMemberIndex];
    newMp = g_pGameState->characters[characterId].mp - amount;
    if (newMp <= 0) {
        newMp = 1;
    }
    g_pGameState->characters[characterId].mp = newMp;
}

extern s16 g_FieldNumPartyMembersMasks[4];

void FieldScriptVMHandlerDecreasePartyHp(void) {
    int mask;
    int amount;
    int i;

    amount = FieldScriptArgument1(ARG(1), SCRIPT_READ_U8_REL(ARG(2)));
    mask = g_FieldNumPartyMembersMasks[SCRIPT_READ_U8_REL(ARG(2)) & 0x3];

    for (i = 0; i < MAX_PARTY_MEMBERS; i++) {
        if ((g_GamePartyMembers[i] != CHARACTER_ID_NONE) && (mask & 1)) {
            FieldPartyMemberDecreaseHp(i, amount);
        }
        mask >>= 1;
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 4;
}


extern s16 D_800B2340;
extern s16 D_800B2342;
extern s16 D_800B2344;
extern s16 D_800B2346;

/* FEAE — arm animation/dolly counters (not CLUT). */
void func_80096AF4(void) {
#ifdef XENO_PC_PORT
    D_800B2344 = FieldScriptVMGetArgument(1);
    D_800B2346 = FieldScriptVMGetArgument(3);
    D_800B2340 = FieldScriptVMGetArgument(5);
    D_800B2342 = 0;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 7;
#else
    D_800B2344 = FieldScriptVMGetArgument(1);
    D_800B2346 = FieldScriptVMGetArgument(3);
    D_800B2340 = FieldScriptVMGetArgument(5);
    D_800B2342 = 0;
    *(u16 *)((s8*)(g_FieldScriptVMCurActor) + 0xCC) = (u16) (*(u16 *)((s8*)(g_FieldScriptVMCurActor) + 0xCC) + 7);
#endif
}

void FieldScriptVMHandlerWritePartyMemberHp(void) {
    if (g_GamePartyMembers[SCRIPT_READ_U8_REL(ARG(2))] != CHARACTER_ID_NONE) {
        FieldScriptMemoryWriteU16(
            SCRIPT_IMM_ARG(1), 
            g_pGameState->characters[g_GamePartyMembers[SCRIPT_READ_U8_REL(ARG(2))]].hp
        );
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 4;
}

void FieldScriptVMHandlerWritePartyMemberMp(void) {
    if (g_GamePartyMembers[SCRIPT_READ_U8_REL(ARG(2))] != CHARACTER_ID_NONE) {
        FieldScriptMemoryWriteU16(
            SCRIPT_IMM_ARG(1), 
            g_pGameState->characters[g_GamePartyMembers[SCRIPT_READ_U8_REL(ARG(2))]].mp
        );
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 4;
}

// Sets the HP of a character to a target amount, but only if we have a full party
void FieldScriptVMHandlerSetPartyMemberHp(void) {
    int targetHp;
    int characterHp;

    if (g_GamePartyMembers[SCRIPT_READ_U8_REL(3)] != CHARACTER_ID_NONE) {
        targetHp = FieldScriptVMGetArgument(2);
        characterHp = g_pGameState->characters[g_GamePartyMembers[SCRIPT_READ_U8_REL(1)]].maxHp;
        if (characterHp < targetHp) {
            targetHp = characterHp;
        }
        g_pGameState->characters[g_GamePartyMembers[SCRIPT_READ_U8_REL(1)]].hp = targetHp;
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 4;
}

// Sets the MP of a character to a target amount, but only if we have a full party
void FieldScriptVMHandlerSetPartyMemberMp(void) {
    int targetMp;
    int characterMp;

    if (g_GamePartyMembers[SCRIPT_READ_U8_REL(3)] != CHARACTER_ID_NONE) {
        targetMp = FieldScriptVMGetArgument(2);
        characterMp = g_pGameState->characters[g_GamePartyMembers[SCRIPT_READ_U8_REL(1)]].maxMp;
        if (characterMp < targetMp) {
            targetMp = characterMp;
        }
        g_pGameState->characters[g_GamePartyMembers[SCRIPT_READ_U8_REL(1)]].mp = targetMp;
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 4;
}

void FieldScriptVMHandlerIncreasePartyMp(void) {
    int mask;
    int amount;
    int i;

    amount = FieldScriptArgument1(1, SCRIPT_READ_U8_REL(3));
    mask = g_FieldNumPartyMembersMasks[SCRIPT_READ_U8_REL(3) & 0x3];

    for (i = 0; i < MAX_PARTY_MEMBERS; i++) {
        if ((g_GamePartyMembers[i] != CHARACTER_ID_NONE) && (mask & 1)) {
            FieldPartyMemberIncreaseMp(i, amount);
        }
        mask >>= 1;
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 4;
}

void FieldScriptVMHandlerDecreasePartyMp(void) {
    int mask;
    int amount;
    int i;

    amount = FieldScriptArgument1(1, SCRIPT_READ_U8_REL(3));
    mask = g_FieldNumPartyMembersMasks[SCRIPT_READ_U8_REL(3) & 0x3];

    for (i = 0; i < MAX_PARTY_MEMBERS; i++) {
        if ((g_GamePartyMembers[i] != CHARACTER_ID_NONE) && (mask & 1)) {
            FieldPartyMemberDecreaseMp(i, amount);
        }
        mask >>= 1;
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 4;
}

extern s32 g_GamePartyMembers[];
extern s16 g_FieldNumPartyMembersMasks[];
extern void FieldPartyMemberIncreaseMp(s32, s32);

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from EtherAura xenogears (GPL-3.0, github.com/EtherAura/xenogears @e54f35a): OP_7E (src/field/f_80096844.c). */
#ifndef XM_T_sFieldScriptEntityScriptSlot
#define XM_T_sFieldScriptEntityScriptSlot
typedef struct {
     u16 m0_scriptPC;
     u8 m2_delay;
     u8 m3_scriptIndex; 
     u32 m4_x0_numSteps : 16;
              u32 m4_x10_status : 2;
              u32 m4_x12_priority : 4; 
              u32 m4_x16 : 1;
              u32 m4_x17_walkMode : 2;
              u32 m4_x19 : 1;
              u32 m4_x1A : 1;
              u32 m4_x1B : 2;
              u32 m4_x1D : 3;
} sFieldScriptEntityScriptSlot;
#endif
#ifndef XM_T_Vec2s16
#define XM_T_Vec2s16
typedef struct {
    s16 vx, vy;
} Vec2s16;
#endif
#ifndef XM_T_sFieldScriptEntity
#define XM_T_sFieldScriptEntity
typedef struct sFieldScriptEntity {
     u32 m0_fieldScriptFlags; 
     u32 m4_flags;
     s16 m8_currentWalkMeshTriangle[4];
     s16 m10_walkmeshId;
     u16 m12;
     u32 m14_currentTriangleFlag;
     s16 m18_boundingVolume[3];
     u16 m1E_collisionRadius; 
     VECTOR m20_position;
     VECTOR m30_stepVector;
     VECTOR m40_externalStep;
     VECTOR m50_surfaceNormal;
     SVECTOR m60_interactionOffset;
     s16 m68_oldPosition[3];
     s16 m6E_stuckCounter;
     s16 m70_rotationForRendering;
     s16 m72_elevation;
     u8 m74_touchedActor; 
     u8 m75_parentActorId; 
     u16 m76_timeScale; 
     u16 m78_stack[4];
     u8 m80_dialogPortrait; 
     u8 m81_selectedMultichoiceLine; 
     u8 m82_dialogWindowSize[2]; 
     s32 m84_dialogWindowFlags;
     s16 m88_dialogWindowPosition[2];
     sFieldScriptEntityScriptSlot m8C_scriptSlots[8];
     u16 mCC_scriptPC;
     u8 mCE_currentScriptSlot;
     u8 mCF_scriptSlotWaitedOn;
     VECTOR mD0_targetPositionOffset;
     s16 mE0_rotationLimit;
     u8 mE2_doorFrame;  
     u8 mE3_pushCounter;  
     s16 mE4_playableCharacterId;
     s16 mE6_idleAnimationId;
     s16 mE8_currentAnimationId;
     s16 mEA_forcedAnimation;
     s16 mEC_elevation;
     s16 mEE_spriteSplitHeight;
     s32 mF0_fallSpeed;
     s16 mF4_scale3d[3];
     s16 mFA;
     u8 mFC_spriteColor[3];
     u8 mFF_spriteColor2[3];
     s16 m102_numSteps;
     s16 m104_rotation;
     s16 m106_currentRotation;
     s16 m108_rotation3;
     u16 m10A_soundId;
     u8 m10C_soundVolume;
     u8 m10D_soundMode;  
     s16 m10E;
     s16* m110_modelRotation2;
     Vec2s16* m114_movementBoundingZone; 
     s32* m118_deformationStrength;
     s16 m11C_lockedRotation;
     s16 m11E_rotationSpeed;
     void* m120_special2dAnimation;
     s16 m124_special2dAnimationId;
     u8 m126_spriteSource;
     u8 m127_vramSlot;
     u16 m128_attachedMechaBone; 
     s16 m12A;
     u32 m12C_x0 : 2;
                u32 m12C_x2 : 3;
                u32 m12C_x5_doorMoving : 1; 
                u32 m12C_x6_stackDepth : 3; 
                u32 m12C_x9 : 3;
                u32 m12C_xC : 1;
                u32 m12C_xD : 3;
                u32 m12C_x10 : 2;
                u32 m12C_x12 : 10;
                u32 m12C_x1C : 4;
     u32 m130_x0 : 9;
                u32 m130_x9 : 10;
                u32 m130_x13 : 9;
                u32 m130_x1C : 2; 
                u32 m130_x1E : 2;
     u32 m134_x0 : 4;  
                u32 m134_x4 : 1;  
                u32 m134_x5 : 2;
                u32 m134_x7 : 1;  
                u32 m134_x8 : 24;
} sFieldScriptEntity;
#endif
#ifndef XM_S_OP_7E
#define XM_S_OP_7E
void xm_OP_7E(void) __asm__("func_80097108");
#endif
#ifndef XM_S_getVar80
#define XM_S_getVar80
s32 xm_getVar80(s32 offset, s32 flags) __asm__("FieldScriptArgument1");
#endif
#ifndef XM_S_pCurrentFieldScriptFile
#define XM_S_pCurrentFieldScriptFile
extern u8 * xm_pCurrentFieldScriptFile __asm__("g_FieldScriptVMCurScriptData");
#endif
#ifndef XM_S_pCurrentFieldScriptActor
#define XM_S_pCurrentFieldScriptActor
extern sFieldScriptEntity * xm_pCurrentFieldScriptActor __asm__("g_FieldScriptVMCurActor");
#endif
#ifndef XM_S_partySlotSelectionMasks
#define XM_S_partySlotSelectionMasks
extern s16 xm_partySlotSelectionMasks[4] __asm__("g_FieldNumPartyMembersMasks");
#endif
#ifndef XM_S_currentParty
#define XM_S_currentParty
extern s32 xm_currentParty[3] __asm__("g_GamePartyMembers");
#endif
#ifndef XM_S_addPartyMP
#define XM_S_addPartyMP
void xm_addPartyMP(s32 slot, s32 amount) __asm__("FieldPartyMemberIncreaseMp");
#endif
void func_80097108(void) {
    s32 amount = xm_getVar80(1, xm_pCurrentFieldScriptFile[xm_pCurrentFieldScriptActor->mCC_scriptPC + 3]);
    s32 mask = xm_partySlotSelectionMasks[xm_pCurrentFieldScriptFile[xm_pCurrentFieldScriptActor->mCC_scriptPC + 3] & 3];
    s32 i;

    for (i = 0; i < 3; i++) {
        if (xm_currentParty[i] != 0xFF && (mask & 1)) {
            xm_addPartyMP(i, amount);
        }
        mask >>= 1;
    }
    xm_pCurrentFieldScriptActor->mCC_scriptPC += 4;
}
#endif
#else
void func_80097108(void) {
    u8 mask1 = SCRIPT_READ_U8_REL(3);
    s32 mpAmount = FieldScriptArgument1(1, mask1);
    u8 partyMask = SCRIPT_READ_U8_REL(3) & 3;
    s16 memberMask = g_FieldNumPartyMembersMasks[partyMask];
    s32 i;
    for (i = 0; i < 3; i++) {
        if (g_GamePartyMembers[i] != 0xFF && (memberMask & 1)) {
            FieldPartyMemberIncreaseMp(i, mpAmount);
        }
        memberMask >>= 1;
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 4;
}
#endif /* XENO_PC_PORT */

void FieldScriptVMHandlerRestoreCharacterHpAndMp(void) {
    int id = FieldScriptVMGetArgument(1);
    g_pGameState->characters[id].hp = g_pGameState->characters[id].maxHp;
    g_pGameState->characters[id].mp = g_pGameState->characters[id].maxMp;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void FieldScriptVMHandlerRestoreHp(void) {
    int i;
    for (i = 0; i < MAX_GAME_CHARACTERS; i++) {
        g_pGameState->characters[i].hp = g_pGameState->characters[i].maxHp;
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer++;
}

void FieldScriptVMHandlerRestoreMp(void) {
    int i;
    for (i = 0; i < MAX_GAME_CHARACTERS; i++) {
        g_pGameState->characters[i].mp = g_pGameState->characters[i].maxMp;
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer++;
}
