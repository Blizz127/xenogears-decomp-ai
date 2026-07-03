#include "common.h"
#include "main/game.h"
#include "system/math.h"
#include "field/script_vm.h"
#include "field/actor.h"
#include "field/main.h"
#include "field/text_box.h"
#ifdef XENO_PC_PORT
#include <assert.h>
#else
/* <assert.h> is unavailable under the matching build's -nostdinc MIPS
 * preprocessor. The assert(...) calls below mark invariant checks in
 * functions not yet byte-matched, so a no-op assert compiles safely there. */
#define assert(x) ((void)0)
#endif

extern void func_80076AC0(s32, s32, void*, s32, s32, s32, s32);
extern s32 D_800AFD1C; // Current actor index
extern s32 D_800ADB1C;
extern u8 D_800B21CE;
extern u16 D_800AEA54[];
extern s16 D_800AFB54;
extern s32 D_8005A444[];
extern s32 D_800AFFEC;
extern s32 D_800B2268;
extern s16 D_800AFD20;
extern s16 D_800B233E;
extern s32 g_PlayerActorIndex;
extern void* g_PartyDataBuffers[];
extern s16 func_8007B1C4(s16, s16, s32, s16*, s32*);
extern s32 func_80080968(u8*);
extern s32 func_8008CF3C(s32);
extern s32 FieldScriptVMGetArgument(s32);
extern void func_8009FA54(s32);

void FieldScriptVMHandlerDisableDialogActivation(void) {
    g_FieldScriptVMCurActor->scriptFlags.fields.scriptFlags_0xA = 0x1;
    g_FieldScriptVMCurActor->scriptInstructionPointer++;
}

void FieldScriptVMHandlerEnableDialogActivation(void) {
    g_FieldScriptVMCurActor->scriptFlags.fields.scriptFlags_0xA = 0x0;
    g_FieldScriptVMCurActor->scriptInstructionPointer++;
}

void func_8009DA70(void) {
    g_FieldScriptVMCurActor->scriptFlags.fields.scriptFlags_0x16 = 0x1;
    g_FieldScriptVMCurActor->scriptInstructionPointer++;
}

void func_8009DA98(void) {
    g_FieldScriptVMCurActor->scriptFlags.fields.scriptFlags_0x16 = 0;
    g_FieldScriptVMCurActor->scriptInstructionPointer++;
}

void func_8009DAC4(void) {
    int nTextBoxIndex;
    ActorData* pActor;
    FieldActor* pFieldActor;

    if (FieldScriptVMGetActorIndex(1) != ACTOR_ID_INVALID) {
        pActor = (ActorData*)(uintptr_t)g_FieldActors[FieldScriptVMGetActorIndex(1)].pActorData;
        pActor->scriptFlags.fields.scriptFlags_0x0 = 0x1;
        pActor->flags |= 0x100000;
        pFieldActor = &g_FieldActors[FieldScriptVMGetActorIndex(1)];
        pFieldActor->status |= ACTOR_STATUS_INVISIBLE;
        if (func_8009CD18(&nTextBoxIndex) == 0) {
            g_FieldTextBoxes[nTextBoxIndex].status = 0;
        }
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
}

void FieldScriptVMHandlerEnableActorVM(void) {
    ActorData* pActor;

    if (FieldScriptVMGetActorIndex(1) != ACTOR_ID_INVALID) {
        pActor = (ActorData*)(uintptr_t)g_FieldActors[FieldScriptVMGetActorIndex(1)].pActorData;
        pActor->scriptFlags.fields.scriptFlags_0x0 = 0;
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
}

void func_8009DC4C(void) {
    ActorData* pActor;
    int nTextBoxIndex;
    short nNewRotation;

    if (FieldScriptVMGetActorIndex(1) != ACTOR_ID_INVALID) {
        pActor = (ActorData*)(uintptr_t)g_FieldActors[FieldScriptVMGetActorIndex(1)].pActorData;
        pActor->moveModified.vx = 0;
        pActor->moveModified.vy = 0;
        pActor->moveModified.vz = 0;
        pActor->move.vx = 0;
        pActor->move.vy = 0;
        pActor->move.vz = 0;
        pActor->scriptFlags.fields.scriptFlags_0x0 = 1;
        nNewRotation = pActor->rotation.vx | 0x8000;
        pActor->rotation.vy = nNewRotation;
        pActor->rotation.vx = nNewRotation;
        if (func_8009CD18(&nTextBoxIndex) == 0) {
            g_FieldTextBoxes[nTextBoxIndex].status = 0;
        }
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
}

void FieldScriptVMHandlerSleep(void) {
    u_char nScriptId;

    nScriptId = g_FieldScriptVMCurActor->curScriptIndex;

    // Initialize the value with the argument as the time to sleep, else count down
    if (g_FieldScriptVMCurActor->scripts[nScriptId].waitTimer == 0) {
        g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].waitTimer = FieldScriptVMGetArgument(1);
    } else {
        g_FieldScriptVMCurActor->scripts[nScriptId].waitTimer--;
    }
    
    // When the timer has reached 0, we move to the next instruction
    if (g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].waitTimer == 0) {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
    }

    D_800B00C0 = 1;
}

void FieldScriptVMHandlerShowActorById(void) {
    FieldActor* pFieldActor;

    if (FieldScriptVMGetActorIndex(1) != ACTOR_ID_INVALID) {
        pFieldActor = &g_FieldActors[FieldScriptVMGetActorIndex(1)];
        if (!(((ActorData*)(uintptr_t)pFieldActor->pActorData)->flags & 0x100000)) {
            pFieldActor->status &= ~ACTOR_STATUS_INVISIBLE;
            ((ActorData*)(uintptr_t)pFieldActor->pActorData)->flags &= ~0x2000000;
        }
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
}

void FieldScriptVMHandlerHideActorById(void) {
    FieldActor* pFieldActor;

    if (FieldScriptVMGetActorIndex(1) != ACTOR_ID_INVALID) {
        pFieldActor = &g_FieldActors[FieldScriptVMGetActorIndex(1)];
        pFieldActor->status |= ACTOR_STATUS_INVISIBLE;
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
}

void FieldScriptVMHandlerShowActor(void) {
    FieldActor* pActor = &g_FieldActors[D_800AFD1C];
    pActor->status &= ~ACTOR_STATUS_INVISIBLE;
    g_FieldScriptVMCurActor->curAnimationId = 0xFF;
    g_FieldScriptVMCurActor->flags &= 0xFDFFFFFF;
    g_FieldScriptVMCurActor->scriptInstructionPointer++;
}

void func_8009DF78(void) {
    FieldActor* pFieldActor;

    if (FieldScriptVMGetActorIndex(1) != ACTOR_ID_INVALID) {
        pFieldActor = &g_FieldActors[FieldScriptVMGetActorIndex(1)];
        ((ActorData*)(uintptr_t)pFieldActor->pActorData)->flags |= 0x02000000;
        ((ActorData*)(uintptr_t)pFieldActor->pActorData)->flags |= 0x800;
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
}

void func_8009E014(void) {
    g_FieldScriptVMCurActor->flags |= 0x02000800;
    g_FieldScriptVMCurActor->scriptInstructionPointer++;
}

void FieldScriptVMHandlerHideActor(void) {
    FieldActor* pActor = &g_FieldActors[D_800AFD1C];
    pActor->status |=  ACTOR_STATUS_INVISIBLE;
    g_FieldScriptVMCurActor->scriptInstructionPointer++;
}

// Update sprite movement or animation speed?
void func_8009E094(void) {
    unsigned short nMoveSpeed;

    nMoveSpeed = FieldScriptVMGetArgument(1);
    g_FieldScriptVMCurActor->moveSpeed = nMoveSpeed;
    func_80021BCC((void*)(uintptr_t)g_FieldActors[D_800AFD1C].pSpriteData, nMoveSpeed);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc6", func_8009E10C);
/*
Matches when scriptFlags is a u32, not bitfields.

void func_8009E10C(void) {
    int nValue;
    int nUnkFlags;

    nValue = FieldScriptVMGetArgument(1);
    nUnkFlags = (nValue & 0x1) << 7; // if (nValue & 0x1) nUnkFlags |= 0x80;
    if (nValue & 0x4) nUnkFlags |= 0x20;
    if (nValue & 0x8) nUnkFlags |= 0x10;
    if (nValue & 0x10) nUnkFlags |= 8;
    if (nValue & 0x20) nUnkFlags |= 4;
    if (nValue & 0x40) nUnkFlags |= 0x08000000;
    
    g_FieldScriptVMCurActor->scriptFlags &= 0xF7FFFF43;
    g_FieldScriptVMCurActor->scriptFlags |= nUnkFlags;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}
*/

void func_8009E1A0(void) {
    u32 nValue;

    nValue = SCRIPT_READ_U8_REL(1) & 7;
    g_FieldScriptVMCurActor->flags &= ~7;
    g_FieldScriptVMCurActor->flags |= nValue;
    
    nValue = (SCRIPT_READ_U8_REL(1) >> 1) & 0x38;
    g_FieldScriptVMCurActor->flags &= ~0x38;
    g_FieldScriptVMCurActor->flags |= nValue;
    
    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
}

void func_8009E208(void) {
    g_FieldScriptVMCurActor->unkEC = 0;
    g_FieldScriptVMCurActor->scriptFlags.fields.scriptFlags_0x10 = 0x0;
    g_FieldScriptVMCurActor->scriptFlags.fields.scriptFlags_0x15 = 0x1;
    g_FieldScriptVMCurActor->scriptInstructionPointer++;
    g_FieldScriptVMCurActor->curYPos = CONV_TO_GTE(g_FieldScriptVMCurActor->position.vy);
}

void func_8009E248(void) {
    func_8009E574(
        FieldScriptVMGetInstructionArgumentS16(1),
        FieldScriptVMGetInstructionArgumentS16(3)
    );
    func_8009E810(FieldScriptVMGetInstructionArgumentS16(5));
    g_FieldScriptVMCurActor->scriptFlags.fields.scriptFlags_0x10 = 0x1;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 7;
}

void func_8009E2C8(void) {
    func_8009E810(FieldScriptArgument1(1, SCRIPT_READ_U8_REL(3)));
    g_FieldScriptVMCurActor->scriptFlags.fields.scriptFlags_0x10 = 0x1;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 4;
}

// Read short from bytecode, absolute offset
short func_8009E330(int offset) {
    return SCRIPT_READ_U8(offset) + (SCRIPT_READ_U8(offset + 1) << 8);
}

void func_8009E35C(void) {
    g_FieldScriptVMCurActor->walkmeshId = SCRIPT_READ_U8_REL(5);
    func_8009E574(
        FieldScriptArgument1(1, SCRIPT_READ_U8_REL(6)), 
        FieldScriptArgument2(3, SCRIPT_READ_U8_REL(6))
    );
    g_FieldScriptVMCurActor->flags &= ~0x200000;
    g_FieldScriptVMCurActor->scriptFlags.fields.scriptFlags_0xX = 0x0;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 7;
}

void func_8009E428(void) {
    ActorData* pActor;

    g_FieldScriptVMCurActor->walkmeshId = SCRIPT_READ_U8_REL(1);
    pActor = (ActorData*)(uintptr_t)g_FieldActors[D_800AFD1C].pActorData;
    func_8009E574(
        CONV_TO_GTE(pActor->position.vx), 
        CONV_TO_GTE(pActor->position.vz)
    );
    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
}

void func_8009E4BC(void) {
    func_8009E574(
        FieldScriptArgument1(1, SCRIPT_READ_U8_REL(5)), 
        FieldScriptArgument2(3, SCRIPT_READ_U8_REL(5))
    );
    g_FieldScriptVMCurActor->flags &= ~0x200000;
    g_FieldScriptVMCurActor->scriptFlags.fields.scriptFlags_0xX = 0x0;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 6;
}

void func_8009E574(s16 x, s16 z) {
    FieldActor* pFieldActor = &g_FieldActors[D_800AFD1C];
    ActorData* pActor = g_FieldScriptVMCurActor;
    u8* pSpriteData = (u8*)(uintptr_t)pFieldActor->pSpriteData;
    s32 state[4][4];
    s16 out[4][4];
    s32 i;
    s32 walkmeshId;
    s16 y;

    for (i = 0; i < D_800AFB54 - 1; i++) {
        pActor->walkmeshTriIds[i] = func_8007B1C4(x, z, i, out[i], state[i]);
    }

    pActor->curWalkmeshTriMaterial = func_80080968((u8*)pActor);
    walkmeshId = pActor->walkmeshId;
    pActor->curTriNormal.vx = state[walkmeshId][0];
    pActor->curTriNormal.vy = state[walkmeshId][1];
    pActor->curTriNormal.vz = state[walkmeshId][2];

    pFieldActor->transformMatrix.t[0] = x;
    pFieldActor->childMatrix.t[0] = x;
    y = out[walkmeshId][1];
    pFieldActor->transformMatrix.t[1] = y;
    pFieldActor->childMatrix.t[1] = y;
    pFieldActor->transformMatrix.t[2] = z;
    pFieldActor->childMatrix.t[2] = z;

    *(s16*)(pSpriteData + 0x84) = (u16)y;

    pActor->position.vx = x << 16;
    pActor->position.vz = z << 16;
    pActor->position.vy = y << 16;
    pActor->curYPos = (u16)y;

    *(u32*)(pSpriteData + 0x00) = pActor->position.vx;
    *(u32*)(pSpriteData + 0x04) = pActor->position.vy;
    *(u32*)(pSpriteData + 0x08) = pActor->position.vz;

    pActor->move.vx = 0;
    pActor->move.vy = 0;
    pActor->move.vz = 0;
    pActor->moveModified.vx = 0;
    pActor->moveModified.vy = 0;
    pActor->moveModified.vz = 0;
    pActor->unkD0.vx = 0;
    pActor->unkD0.vy = 0;
    pActor->unkD0.vz = 0;
    pActor->unk60.vx = 0;
    pActor->unk60.vy = 0;
    pActor->unk60.vz = 0;

    *(u32*)(pSpriteData + 0x0C) = 0;
    *(u32*)(pSpriteData + 0x10) = 0;
    *(u32*)(pSpriteData + 0x14) = 0;

    pActor->unkF0 = 0;
    pActor->unkEC = 0;
    pActor->curYPos = CONV_TO_GTE(pActor->position.vy);
    pActor->scriptFlags.flags &= ~0x40000;
    pActor->scriptFlags.flags |= 0x400000;
}

void func_8009E810(s16 arg0) {
    g_FieldScriptVMCurActor->position.vy = arg0 << 16;
    g_FieldScriptVMCurActor->unkEC = arg0;
    g_FieldScriptVMCurActor->curYPos = arg0;
}

void func_8009E83C(void) {
    unsigned char width;
    unsigned char zWidth;
    unsigned char height;
    unsigned char solidRange;

    width = SCRIPT_READ_U8_REL(1);
    if (width) {
        g_FieldScriptVMCurActor->width = width * 2;
    }
    
    zWidth = SCRIPT_READ_U8_REL(2);
    if (zWidth) {
        g_FieldScriptVMCurActor->zWidth = zWidth * 2;
    }
    
    height = SCRIPT_READ_U8_REL(3);
    if (height) {
        g_FieldScriptVMCurActor->height = height * 2;
    }
    
    solidRange = SCRIPT_READ_U8_REL(4);
    if (solidRange) {
        g_FieldScriptVMCurActor->solidRange = SCRIPT_READ_U8_REL(4) * 2;
    }
    
    g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc6", func_8009E91C);
/*
Matches, but awaits being compiled in until unk114 struct has been better recovered.


typedef struct {
    u16 f0, f1, f2, f3, f4, f5, f6, f7;
} Tmp;

void func_8009E91C(void) {
    if (!(g_FieldScriptVMCurActor->flags12C & 0x1000)) {
        g_FieldScriptVMCurActor->unk114 = HeapAlloc(0x10, 0x0);
    }
    g_FieldScriptVMCurActor->flags12C |= 0x1000;
    ((Tmp*)g_FieldScriptVMCurActor->unk114)->f0 = FieldScriptArgument1(0x1, SCRIPT_READ_U8_REL(0x11));
    ((Tmp*)g_FieldScriptVMCurActor->unk114)->f1 = FieldScriptArgument2(0x3, SCRIPT_READ_U8_REL(0x11));
    ((Tmp*)g_FieldScriptVMCurActor->unk114)->f2 = FieldScriptArgument3(0x5, SCRIPT_READ_U8_REL(0x11));
    ((Tmp*)g_FieldScriptVMCurActor->unk114)->f3 = FieldScriptArgument4(0x7, SCRIPT_READ_U8_REL(0x11));
    ((Tmp*)g_FieldScriptVMCurActor->unk114)->f4 = FieldScriptArgument5(0x9, SCRIPT_READ_U8_REL(0x11));
    ((Tmp*)g_FieldScriptVMCurActor->unk114)->f5 = FieldScriptArgument6(0xB, SCRIPT_READ_U8_REL(0x11));
    ((Tmp*)g_FieldScriptVMCurActor->unk114)->f6 = FieldScriptArgument7(0xD, SCRIPT_READ_U8_REL(0x11));
    ((Tmp*)g_FieldScriptVMCurActor->unk114)->f7 = FieldScriptArgument8(0xF, SCRIPT_READ_U8_REL(0x11));
    
    g_FieldScriptVMCurActor->scriptInstructionPointer += 0x12;
}
*/

// -1: Actor has a script slot occupied w/ target ID
//  0: Actor has no script slot w/ target ID
int FieldActorGetScriptStatus(ActorData* pActor, int targetId) {
    int i;

    for (i = 0; i < ACTOR_MAX_NUM_SCRIPTS; i++) {
        if (targetId == pActor->scripts[i].scriptId) {
            return -1;
        }
    }
    return 0;
}

void func_8009EB78(void) {
    ActorData* pActor;
    int actorIndex;
    int i;
    int curScriptId;

    if (FieldScriptVMGetActorIndex(1) != 0xFF) {
        actorIndex = FieldScriptVMGetActorIndex(1);
        pActor = (ActorData*)(uintptr_t)g_FieldActors[actorIndex].pActorData;
        if (pActor->flags & 0x100000) {
            curScriptId = g_FieldScriptVMCurActor->curScriptIndex;
            g_FieldScriptVMCurActor->scripts[curScriptId].state = SCRIPT_STATE_IDLE;
            pActor->scripts[g_FieldScriptVMCurActor->unkCF].isInUse = 0;
        } else if (FieldActorGetScriptStatus(pActor, SCRIPT_READ_U8_REL(2) & 0x1F) != ACTOR_SCRIPT_EXISTS) {
            for (i = 0; i < ACTOR_MAX_NUM_SCRIPTS; i++) {
                if (pActor->scripts[i].flags_0x12 != 0xF || pActor->scripts[i].isInUse) {
                    continue;
                }
                
                pActor->scripts[i].currentIP = FieldScriptGetBytecodeOffset(actorIndex, SCRIPT_READ_U8_REL(2) & 0x1F);
                pActor->scripts[i].flags_0x12 = SCRIPT_READ_U8_REL(2) >> 0x5;
                pActor->scripts[i].scriptId = SCRIPT_READ_U8_REL(2) & 0x1F;
                g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
                return;
            }
            return;
        }
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void func_8009ED68(void) {
    ActorData* pActor;
    int actorIndex;
    int i;

    if (FieldScriptVMGetActorIndex(1) == ACTOR_ID_INVALID) {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
        return;
    }
    
    actorIndex = FieldScriptVMGetActorIndex(1);
    pActor = (ActorData*)(uintptr_t)g_FieldActors[actorIndex].pActorData;
    if (pActor->flags & 0x100000) {
        g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].state = SCRIPT_STATE_IDLE;
        pActor->scripts[g_FieldScriptVMCurActor->unkCF].isInUse = 0;
    } else {
        switch (g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].state) {
        case 0:
            if (FieldActorGetScriptStatus(pActor, SCRIPT_READ_U8_REL(2) & 0x1F) == ACTOR_SCRIPT_EXISTS) {
                break;
            }
            
            for (i = 0; i < ACTOR_MAX_NUM_SCRIPTS; i++) {
                if (pActor->scripts[i].flags_0x12 != 0xF || pActor->scripts[i].isInUse) {
                    continue;
                }
                pActor->scripts[i].currentIP = FieldScriptGetBytecodeOffset(actorIndex, SCRIPT_READ_U8_REL(2) & 0x1F);
                pActor->scripts[i].flags_0x12 = SCRIPT_READ_U8_REL(2) >> 5;
                pActor->scripts[g_FieldScriptVMCurActor->unkCF].isInUse = 1;
                pActor->scripts[i].scriptId = SCRIPT_READ_U8_REL(2) & 0x1F;
                g_FieldScriptVMCurActor->unkCF = i;
                g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].state = 1;
                break;
            }
            return;
        case 1:
            if ((pActor->curScriptIndex == g_FieldScriptVMCurActor->unkCF) || pActor->scripts[g_FieldScriptVMCurActor->unkCF].flags_0x12 == 0xF) {
                g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
                g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].state = SCRIPT_STATE_IDLE;
                pActor->scripts[g_FieldScriptVMCurActor->unkCF].isInUse = 0;
            } else {
                D_800B00C0 = g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].state;
            }
            return;
        default:
            return;
        }
    }
    
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void func_8009F0A0(void) {
    ActorData* pActor;
    s32 actorIndex;
    int i;

    if (FieldScriptVMGetActorIndex(1) == ACTOR_ID_INVALID) {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
        return;
    }
    
    actorIndex = FieldScriptVMGetActorIndex(1);
    pActor = (ActorData*)(uintptr_t)g_FieldActors[actorIndex].pActorData;
    if (pActor->flags & 0x100000) {
        g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].state = SCRIPT_STATE_IDLE;
        pActor->scripts[g_FieldScriptVMCurActor->unkCF].isInUse = 0;
    } else {
        switch (g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].state) {
        case 0:
            if (FieldActorGetScriptStatus(pActor, SCRIPT_READ_U8_REL(2) & 0x1F) == ACTOR_SCRIPT_EXISTS) {
                g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
                break;
            }
            
            for (i = 0; i < ACTOR_MAX_NUM_SCRIPTS; i++) {
                if (pActor->scripts[i].flags_0x12 != 0xF || pActor->scripts[i].isInUse) {
                    continue;
                }
                pActor->scripts[i].currentIP = FieldScriptGetBytecodeOffset(actorIndex, SCRIPT_READ_U8_REL(2) & 0x1F);
                pActor->scripts[i].flags_0x12 = SCRIPT_READ_U8_REL(2) >> 5;
                pActor->scripts[g_FieldScriptVMCurActor->unkCF].isInUse = 1;
                g_FieldScriptVMCurActor->unkCF = i;
                g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].state = 1;
                pActor->scripts[i].scriptId = SCRIPT_READ_U8_REL(2) & 0x1F;
                return;
            }
            break;
        case 1:
            if ((pActor->curScriptIndex == g_FieldScriptVMCurActor->unkCF) || pActor->scripts[g_FieldScriptVMCurActor->unkCF].flags_0x12 == 0xF) {
                g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].state = 2;
                return;
            }
            D_800B00C0 = 1;
            break;
        case 2:
            if (pActor->scripts[g_FieldScriptVMCurActor->unkCF].flags_0x12 == 0xF) {
                g_FieldScriptVMCurActor->scripts[g_FieldScriptVMCurActor->curScriptIndex].state = SCRIPT_STATE_IDLE;
                pActor->scripts[g_FieldScriptVMCurActor->unkCF].isInUse = 0;
                g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
                return;
            }
            D_800B00C0 = 1;
            break;
        }
        return;
    }
    
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

// Randomize X rotation
void func_8009F424(void) {
    int nRotation;
    
    nRotation = g_FieldScriptVMCurActor->rotation.vy;
    g_FieldScriptVMCurActor->unk102++;
    if (!(g_FieldScriptVMCurActor->unk102 & 0xF)) {
        if (!(rand() & 0x1)) {
            nRotation = PSX_ANGLE(g_FieldScriptVMCurActor->rotation.vy + PSX_DEGREES(45));
        } else {
            nRotation = PSX_ANGLE(g_FieldScriptVMCurActor->rotation.vy - PSX_DEGREES(45));
        }
    }
    D_800B00C0 = 1;
    g_FieldScriptVMCurActor->rotation.vx = nRotation;
    g_FieldScriptVMCurActor->scriptInstructionPointer++;
}

void func_8009F4CC(void) {
    short nSpecialRotation;
    int nRotation;
    int nRand;
    int nDeltaRotation;
    int delta;

    nRotation = g_FieldScriptVMCurActor->rotation.vy;
    g_FieldScriptVMCurActor->unk102++;
    if (!(g_FieldScriptVMCurActor->unk102 & 0xF)) {
        nRand = rand();
        if (nRand & 0x30) {
            nSpecialRotation = g_FieldScriptVMCurActor->rotation.vy | ~0x7FFF;
            nRotation = nSpecialRotation;
            g_FieldScriptVMCurActor->rotation.vy = nSpecialRotation;
        } else {
            delta = PSX_DEGREES(45);
            if (!(nRand & 1)) {
                nDeltaRotation = g_FieldScriptVMCurActor->rotation.vy + delta;
            } else {
                nDeltaRotation = g_FieldScriptVMCurActor->rotation.vy - delta;
            }
            nRotation = PSX_ANGLE(nDeltaRotation);
        }
    }
    D_800B00C0 = 1;
    g_FieldScriptVMCurActor->rotation.vx = nRotation;
    g_FieldScriptVMCurActor->scriptInstructionPointer++;
}

void func_8009F5A8(void) {
    unsigned short nStoredIP = g_FieldScriptVMCurActor->scriptInstructionPointer;
    func_8009F5F4(); // Do Encounter
    D_800B00C0 = 1;
    g_FieldScriptVMCurActor->scriptInstructionPointer = nStoredIP;
}


void func_8009F5F4(void) {
    assert(!(g_FieldScriptVMCurActor->flags & 0x4000));

    if (D_800B21CE == 0) {
        g_FieldScriptVMCurActor->flags |= 0x1000000;
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer++;
}

int FieldCharacterIdToPartyId(int characterId) {
    int i;

    if (characterId == CHARACTER_ID_NONE) {
        return -1;
    }
    
    for (i = 0; i < 3; i++) {
        if (g_GamePartyMembers[i] == CHARACTER_ID_NONE)
            return -1;
        
        if (g_GamePartyMembers[i] == characterId) {
            return i;
        }
    }

    return -1;
}

void func_8009FA54(s32 scriptEntryIndex) {
    u8* scriptData = (u8*)g_FieldScriptVMCurScriptData;
    s32 entryOffset;
    s16 x;
    s16 z;
    u8 rotByte;
    s16 angle;

    if (scriptData[0] != 0xFF) {
        return;
    }

    entryOffset = scriptEntryIndex * 7;
    g_FieldScriptVMCurActor->walkmeshId = scriptData[entryOffset + 5];

    x = func_8009E330(entryOffset + 1);
    z = func_8009E330(entryOffset + 3);
    func_8009E574(x, z);

    rotByte = scriptData[entryOffset + 6];
    if (rotByte == 0xFF) {
        rotByte = FieldScriptVMGetVariableValue(0x8) + 4;
    } else {
        rotByte += 4;
    }
    angle = (rotByte & 0x7) << 9;
    *(s16*)((u8*)&g_Scene + 0x56) = angle;
    *(s32*)((u8*)&g_Scene + 0x7C) = angle;
    *(s32*)((u8*)&g_Scene + 0x60) = angle << 16;

    rotByte = scriptData[entryOffset + 7];
    if (rotByte == 0xFF) {
        rotByte = FieldScriptVMGetVariableValue(0x6) - 2;
    } else {
        rotByte -= 2;
    }
    angle = ((rotByte & 0x7) << 9) | 0x8000;
    g_FieldScriptVMCurActor->rotation.vx = angle;
    g_FieldScriptVMCurActor->rotation.vy = angle;
    g_FieldScriptVMCurActor->rotation.vz = angle;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc6", func_8009FB98);
/*
Matches, but the struct D_800B2268 is part of needs recovery first.

void func_8009FB98(void) {
    g_GameSceneMapNum |= 0xC000;
    GameWaitForCdData();
    GamePartySyncSkinData();
    GamePartySyncStreamedData();
    D_800B2268[0] = SCRIPT_READ_U8_REL(1);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
}
*/

extern s32 D_8006F990[];
int func_8009FC10(int actorIndex) {
    int i;

    for (i = 0; i < 3; i++) {
        if (D_8006F990[i] == actorIndex) {
            return i;
        }
    }
    
    return 0xFF;
}

void FieldScriptPartyMemberRideGear(void) {
    int index = FieldScriptVMGetArgument(1);
    if (index >= 3) {
        index = 2;
    }
    g_pGameState->gearRide[index] = 1;
    func_8009FD10(index);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void FieldScriptPartyMemberDisembarkGear(void) {
    int index = FieldScriptVMGetArgument(1);
    if (index >= 3) {
        index = 2;
    }
    g_pGameState->gearRide[index] = 0;
    func_8009FD10(index);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

extern int g_GameSceneMapNum;
void func_8009FD10(int partyMemberIndex) {
    switch (partyMemberIndex) { 
        case 0:
            FieldScriptMemoryWriteU16(0x2A, g_GameSceneMapNum & 0xFFF);
            FieldScriptMemoryWriteU16(0x2C, 0);
            FieldScriptMemoryWriteU16(0x2E, 0);
            break;
        case 1:
            FieldScriptMemoryWriteU16(0x30, g_GameSceneMapNum & 0xFFF);
            FieldScriptMemoryWriteU16(0x32, 0);
            FieldScriptMemoryWriteU16(0x34, 0);
            break;
        case 2:
            FieldScriptMemoryWriteU16(0x36, g_GameSceneMapNum & 0xFFF);
            FieldScriptMemoryWriteU16(0x38, 0);
            FieldScriptMemoryWriteU16(0x3A, 0);
            break;
    }
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc6", func_8009FDD4);

INCLUDE_ASM("asm/field/nonmatchings/main/misc6", func_8009FE4C);

INCLUDE_ASM("asm/field/nonmatchings/main/misc6", func_8009FEE4);

void func_800A0158(s32 arg0, s32* arg1, s32* arg2, s32* arg3) {
    switch (arg0) {
        case 0:
            *arg1 = FieldScriptVMGetVariableValue(0x2A);
            *arg2 = FieldScriptVMGetVariableValue(0x2C);
            *arg3 = FieldScriptVMGetVariableValue(0x2E);
            break;
        case 1:
            *arg1 = FieldScriptVMGetVariableValue(0x30);
            *arg2 = FieldScriptVMGetVariableValue(0x32);
            *arg3 = FieldScriptVMGetVariableValue(0x34);
            break;
        case 2:
            *arg1 = FieldScriptVMGetVariableValue(0x36);
            *arg2 = FieldScriptVMGetVariableValue(0x38);
            *arg3 = FieldScriptVMGetVariableValue(0x3A);
            break;
    }
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc6", func_800A0228);

INCLUDE_ASM("asm/field/nonmatchings/main/misc6", func_800A0524);

INCLUDE_ASM("asm/field/nonmatchings/main/misc6", func_800A06E8);

void func_800A08B8(void) {
    FieldActor* fieldActor = &g_FieldActors[D_800AFD1C];
    s32 characterId = func_8008CF3C(FieldScriptVMGetArgument(1));
    s32 partyId = FieldCharacterIdToPartyId(characterId);

    g_FieldScriptVMCurActor->characterId = characterId;
    fieldActor->status = (fieldActor->status & 0xF07F) | 0x200;

    assert(D_800B2268 == 0);

    if (partyId == -1) {
        func_80076AC0(D_800AFD1C, 0, g_PartyDataBuffers[0], 1, 0, 0, 1);
        g_FieldScriptVMCurActor->scriptFlags.flags |= 0x1;
        g_FieldScriptVMCurActor->flags |= 0x100000;
        D_800AFFEC = 1;
        D_800B00C0 = 1;
        g_FieldScriptVMCurActor->scriptFlags.flags |= 0x20000;
        g_FieldScriptVMCurActor->flags |= 0x400;
        g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
        return;
    }

    assert(partyId == 0);

    g_PlayerActorIndex = D_800AFD1C;
    D_800B233E = D_800AFD1C;
    g_FieldScriptVMCurActor->scriptFlags.flags =
        (g_FieldScriptVMCurActor->scriptFlags.flags | 0x4400) & ~0x80;
    D_8005A444[partyId] = D_800AFD1C;

    func_80076AC0(D_800AFD1C, partyId, g_PartyDataBuffers[partyId], 1, 0, partyId, 1);
    g_FieldScriptVMCurActor->scriptFlags.flags =
        (g_FieldScriptVMCurActor->scriptFlags.flags | 0x400) & ~0x300;

    fieldActor = &g_FieldActors[D_800AFD1C];
    fieldActor->status &= ~ACTOR_STATUS_INVISIBLE;
    D_800AFD20 = -0xC0;
    func_8009FA54(FieldScriptVMGetVariableValue(2));
    func_800A0C94();
    g_FieldScriptVMCurActor->flags &= ~0x800;
    g_FieldScriptVMCurActor->scriptFlags.flags |= 0x20000;
    g_FieldScriptVMCurActor->flags |= 0x400;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void func_800A0C4C(void) {
    ((ActorData*)(uintptr_t)g_FieldActors[g_PlayerActorIndex].pActorData)->scriptFlags.flags |= 0x80;
}

// FieldResetActorPosition, set translation and sprite position of actor based on current actor data position
void func_800A0C94(void) {
    u8* actorData = (u8*)g_FieldScriptVMCurActor;
    u8* fieldActor = (u8*)g_FieldActors + D_800AFD1C * 0x5C;
    u8* spriteData = (u8*)(uintptr_t)*(u32*)(fieldActor + 0x04);
    s16 x = *(s16*)(actorData + 0x22);
    s16 y = *(s16*)(actorData + 0x26);
    s16 z = *(s16*)(actorData + 0x2A);

    *(s32*)(fieldActor + 0x20) = x;
    *(s32*)(fieldActor + 0x40) = x;
    *(s32*)(fieldActor + 0x24) = y;
    *(s32*)(fieldActor + 0x44) = y;
    *(s32*)(fieldActor + 0x28) = z;
    *(s32*)(fieldActor + 0x48) = z;

    *(s32*)(spriteData + 0x00) = *(s32*)(actorData + 0x20);
    *(s32*)(spriteData + 0x04) = *(s32*)(actorData + 0x24);
    *(s32*)(spriteData + 0x08) = *(s32*)(actorData + 0x28);
    *(s32*)(spriteData + 0x10) = 0;
    *(s16*)(spriteData + 0x84) = y;
    *(s16*)(actorData + 0x72) = y;
}

void func_800A0D3C(void) {
    func_80076AC0(D_800AFD1C, 0, (void*)((*(s32*)(g_FieldSpriteData + 4)) + (s32)g_FieldSpriteData), 0, 0, 0x80, 1);
    func_800A0C94();
    g_FieldScriptVMCurActor->scriptFlags.fields.scriptFlags_0x8 = 0x1;
    g_FieldScriptVMCurActor->flags |= 0x800;
    g_FieldScriptVMCurActor->scriptInstructionPointer++;
}

extern s16 D_800B234A;

void func_800A0DC0(void) {
    D_800B234A = FieldScriptVMGetArgument(1);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void func_800A0DFC(void) {
    FieldScriptMemoryWriteU16(SCRIPT_IMM_ARG(1), ArchiveGetDiscNumber());
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

extern s32 D_800ADB74;

void func_800A0E54(void) {
    if (D_800ADB74 == 0) {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 1;
    } else {
        g_FieldScriptVMCurActor->scriptInstructionPointer -= 1;
    }
    D_800B00C0 = 1;
}

extern s32 D_800ADB84;

void func_800A0EB0(void) {
    D_800B00C0 = 1;
    D_800ADB84++;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 1;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc6", func_800A0EE8);

INCLUDE_ASM("asm/field/nonmatchings/main/misc6", func_800A0FD8);

INCLUDE_ASM("asm/field/nonmatchings/main/misc6", func_800A1364);

INCLUDE_ASM("asm/field/nonmatchings/main/misc6", func_800A14F0);

void func_800A1624(void) {
    u8* actorData;
    u8* fieldActor;
    u8* spritePackage;
    s32 skinIndex;

    fieldActor = (u8*)g_FieldActors + D_800AFD1C * 0x5C;
    *(u16*)(fieldActor + 0x58) = (*(u16*)(fieldActor + 0x58) & 0xF07F) | 0x200;

    skinIndex = FieldScriptVMGetArgument(1);
    spritePackage = (u8*)g_FieldSpriteData +
                    *(u32*)((u8*)g_FieldSpriteData + (skinIndex * 4) + 4);
    func_80076AC0(D_800AFD1C, skinIndex, spritePackage, 0, 0, skinIndex | 0x80, 0);
    func_800A0C94();

    actorData = (u8*)g_FieldScriptVMCurActor;
    *(u16*)(actorData + 0xCC) += 3;
    *(u32*)(actorData + 0x0) = (*(u32*)(actorData + 0x0) | 0x100) & ~0x80;
    *(u32*)(actorData + 0x4) &= ~0x800;

    fieldActor = (u8*)g_FieldActors + D_800AFD1C * 0x5C;
    *(u16*)(fieldActor + 0x58) &= ~0x20;
}
