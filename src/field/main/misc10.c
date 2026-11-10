#include "common.h"
#include "main/game.h"
#include "field/actor.h"
#include "field/script_vm.h"

extern u8* func_8009501C(s32 itemId);
extern u8* func_800950A0(s32 itemId);
extern s32 func_80095124(s32 itemId);
extern s32 func_800951B8(s32 itemId);
void func_8009635C(s32 itemId);

#ifndef XENO_PC_PORT
#ifndef XENO_PC_PORT
/* Adopted from EtherAura xenogears (GPL-3.0, github.com/EtherAura/xenogears @e54f35a): OP_34 (src/field/f_80096214.c). */
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
#ifndef XM_S_OP_34
#define XM_S_OP_34
void xm_OP_34(void) __asm__("func_80096214");
#endif
#ifndef XM_S_getImmediateOrVariableUnsigned
#define XM_S_getImmediateOrVariableUnsigned
s32 xm_getImmediateOrVariableUnsigned(s32 offset) __asm__("FieldScriptVMGetArgument");
#endif
#ifndef XM_S_findInventoryItemSlot
#define XM_S_findInventoryItemSlot
s32 xm_findInventoryItemSlot(s32 item) __asm__("func_80095124");
#endif
#ifndef XM_S_getInventoryItemCounts
#define XM_S_getInventoryItemCounts
u8* xm_getInventoryItemCounts(s32 item) __asm__("func_800950A0");
#endif
#ifndef XM_S_getInventoryItemIds
#define XM_S_getInventoryItemIds
u8* xm_getInventoryItemIds(s32 item) __asm__("func_8009501C");
#endif
#ifndef XM_S_setVar
#define XM_S_setVar
void xm_setVar(s32 index, s32 value) __asm__("FieldScriptMemoryWriteU16");
#endif
#ifndef XM_S_readU16FromScript
#define XM_S_readU16FromScript
u16 xm_readU16FromScript(s32 offset) __asm__("FieldScriptVMGetInstructionArgument");
#endif
#ifndef XM_S_pCurrentFieldScriptActor
#define XM_S_pCurrentFieldScriptActor
extern sFieldScriptEntity * xm_pCurrentFieldScriptActor __asm__("g_FieldScriptVMCurActor");
#endif
void func_80096214(void) {
    s32 item = xm_getImmediateOrVariableUnsigned(1);
    s32 slot = xm_findInventoryItemSlot(item);
    u8* counts = xm_getInventoryItemCounts(item);

    xm_getInventoryItemIds(item);
    if (slot != -1) {
        xm_setVar(xm_readU16FromScript(3), counts[slot]);
    } else {
        xm_setVar(xm_readU16FromScript(3), 0);
    }
    xm_pCurrentFieldScriptActor->mCC_scriptPC += 5;
}
#endif
#else
void func_80096214(void) {
    s32 itemId = FieldScriptVMGetArgument(1);
    s32 idx = func_80095124(itemId);
    u8* pTable = func_800950A0(itemId);
    u8* pData = func_8009501C(itemId);
    u16 addr;
    s32 value;

    /* Retail 8009625C/80096274: the instruction-argument read is duplicated
     * into both arms; pData is called but its result is unused. */
    if (idx != -1) {
        addr = (u16)FieldScriptVMGetInstructionArgument(3);
        value = pTable[idx];
    } else {
        addr = (u16)FieldScriptVMGetInstructionArgument(3);
        value = 0;
    }

    FieldScriptMemoryWriteU16(addr, value);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
}
#endif /* XENO_PC_PORT */

void func_800962C0(void) {
    if (func_80095124(FieldScriptVMGetArgument(1)) != -1) {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
    } else {
        g_FieldScriptVMCurActor->scriptInstructionPointer = FieldScriptVMGetInstructionArgument(3);
    }
}

void func_8009631C(void) {
    func_8009635C(FieldScriptVMGetArgument(1));
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/main/misc10", func_8009635C);
#else
void func_8009635C(s32 itemId) {
    s32 slot = func_80095124(itemId);
    u8* quantities = func_800950A0(itemId);
    u8* itemIds = func_8009501C(itemId);

    if (slot != -1) {
        if (quantities[slot] < MAX_ITEM_QUANTITY) {
            quantities[slot]++;
        }
    } else {
        slot = func_800951B8(itemId);
        if (slot != -1) {
            itemIds[slot] = (u8)itemId;
            quantities[slot] = 1;
        }
    }
}
#endif /* XENO_PC_PORT */

extern u8* func_8009501C(s32 itemId);
extern u8* func_800950A0(s32 itemId);
extern s32 func_80095124(s32 itemId);

void func_8009640C(void) {
    s32 itemId = FieldScriptVMGetArgument(1);
    s32 idx = func_80095124(itemId);
    if (idx != -1) {
        /* Retail 80096440: pData (func_8009501C) is the array cleared to
         * 0xFF; pTable (func_800950A0) is the counter that is decremented. */
        u8* pData = func_8009501C(itemId);
        u8* pTable = func_800950A0(itemId);
        u8* pEntry = pTable + idx;
        u8 count = pEntry[0] - 1;
        pEntry[0] = count;
        if (count == 0) {
            pData[idx] = 0xFF;
        }
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

// Check if certain member is in current party
void FieldScriptCheckPartyMember(void) {
    int i;

    for (i = 0; i < MAX_PARTY_MEMBERS; i++) {
        if (SCRIPT_READ_U8_REL(1) == g_GamePartyMembers[i]) {
            g_FieldScriptVMCurActor->scriptInstructionPointer += 4;
            return;
        }        
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer = FieldScriptVMGetInstructionArgument(2);
}

void func_80096534(void) {
    u16 flags = *(u16*)((u8*)g_pGameState + 0x1D30);
    u8 bit = SCRIPT_READ_U8_REL(1);
    if ((flags >> bit) & 1) {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 4;
    } else {
        g_FieldScriptVMCurActor->scriptInstructionPointer = (u16)FieldScriptVMGetInstructionArgument(2);
    }
}

void func_800965A8(void) {
    g_pGameState->unk1D30 |= 1 << SCRIPT_READ_U8_REL(1);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
}

void func_800965F4(void) {
    g_pGameState->unk1D30 &= ~(1 << SCRIPT_READ_U8_REL(1));
    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
}
