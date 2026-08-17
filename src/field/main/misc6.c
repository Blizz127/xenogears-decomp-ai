#include "common.h"
#include "main/game.h"
#include "system/math.h"
#include "field/script_vm.h"
#include "field/actor.h"
#include "field/main.h"
#include "field/text_box.h"
#include "field/camera.h"
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
    
    g_FieldScriptVMCurActor->scriptFlags.flags &= 0xF7FFFF43;
    g_FieldScriptVMCurActor->scriptFlags.flags |= nUnkFlags;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

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

/* Field-script opcode 0x17: preserve eight decoded script arguments in the
 * actor-owned 16-byte state block.  The block is also included in the field
 * save/restore image when bit 0x1000 of ActorData+0x12C is set.
 * asm/field/nonmatchings/main/misc6/func_8009E91C.s */
typedef struct {
    u16 args[8];
} FieldScriptArgs;

/* ActorData's recovered bitfields split the original word at +0x12C.  Keep a
 * tiny overlay for this opcode, which operates on the whole word. */
typedef struct {
    u8 pad[0x12C];
    u32 flags;
} FieldScriptActorFlags;

void func_8009E91C(void) {
    if (!(((FieldScriptActorFlags*)g_FieldScriptVMCurActor)->flags & 0x1000)) {
        g_FieldScriptVMCurActor->unk114 =
            (u32)(uintptr_t)HeapAlloc(0x10, 0);
    }

    ((FieldScriptActorFlags*)g_FieldScriptVMCurActor)->flags |= 0x1000;
    ((FieldScriptArgs*)(uintptr_t)g_FieldScriptVMCurActor->unk114)->args[0] =
        FieldScriptArgument1(1, SCRIPT_READ_U8_REL(0x11));
    ((FieldScriptArgs*)(uintptr_t)g_FieldScriptVMCurActor->unk114)->args[1] =
        FieldScriptArgument2(3, SCRIPT_READ_U8_REL(0x11));
    ((FieldScriptArgs*)(uintptr_t)g_FieldScriptVMCurActor->unk114)->args[2] =
        FieldScriptArgument3(5, SCRIPT_READ_U8_REL(0x11));
    ((FieldScriptArgs*)(uintptr_t)g_FieldScriptVMCurActor->unk114)->args[3] =
        FieldScriptArgument4(7, SCRIPT_READ_U8_REL(0x11));
    ((FieldScriptArgs*)(uintptr_t)g_FieldScriptVMCurActor->unk114)->args[4] =
        FieldScriptArgument5(9, SCRIPT_READ_U8_REL(0x11));
    ((FieldScriptArgs*)(uintptr_t)g_FieldScriptVMCurActor->unk114)->args[5] =
        FieldScriptArgument6(0xB, SCRIPT_READ_U8_REL(0x11));
    ((FieldScriptArgs*)(uintptr_t)g_FieldScriptVMCurActor->unk114)->args[6] =
        FieldScriptArgument7(0xD, SCRIPT_READ_U8_REL(0x11));
    ((FieldScriptArgs*)(uintptr_t)g_FieldScriptVMCurActor->unk114)->args[7] =
        FieldScriptArgument8(0xF, SCRIPT_READ_U8_REL(0x11));
    g_FieldScriptVMCurActor->scriptInstructionPointer += 0x12;
}

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


extern u16 D_800AFE9C;      /* held-button field mask (d-pad in top nibble) */
extern s32 D_800ADB68;      /* playerCanRun flag */
extern s16 D_800ADB02;      /* stuck-frame counter */
extern u8  D_800B2354;      /* selects which d-pad->angle table */
extern u16 D_800ADF68[];    /* d-pad -> angle table 1 (16 entries) */
extern u16 D_800ADF88[];    /* d-pad -> angle table 2 (16 entries) */
extern void func_80079288(void); /* checkForRandomEncounter (side behavior) */

/* func_8009F5F4 = OP_UPDATE_CHARACTER (opcode 0xA7). Called every frame for the
 * player via the idle-hold wrapper func_8009F5A8. Retail asm 8009F5F4-8009F9FC;
 * Noah OP_UPDATE_CHARACTER is the readable reference. The 0x4000 test is on
 * scriptFlags at offset 0x00 (asm lw 0x0($a0)), NOT the ->flags field at 0x04
 * that the prior partial port checked. */
void func_8009F5F4(void) {
    u8* p = (u8*)(uintptr_t)g_FieldScriptVMCurActor;
    u32 scriptFlags = *(u32*)(p + 0x00);

    if (scriptFlags & 0x4000) {
        /* ---- player-controlled branch (asm 8009F614-8009F9A4) ---- */
        s32 i;

        /* Dialog-window gate: idle if any text box is in use (m37C == 0) or the
         * field battle/control var is set (asm 8009F620-8009F66C). */
        for (i = 0; i < 4; i++) {
            if (*(s16*)((u8*)&g_FieldTextBoxes + 0x37C + i * 0x498) == 0) {
                break;
            }
        }
        if (i != 4 || *(s16*)((u8*)&g_FieldControl) != 0) {
            *(s16*)(p + 0x104) = (s16)0x8000; /* idle angle (asm .L8009F9A8, a1=0) */
            g_FieldScriptVMCurActor->scriptInstructionPointer++;
            return;
        }

        /* Random-encounter step roll when a direction is held (asm 8009F674-90).
         * func_80079288 is still a stub in the port; it only affects encounters,
         * not walking, and runs solely when a d-pad direction is pressed. */
        if ((D_800AFE9C >> 12) != 0) {
            func_80079288();
        }

        D_800ADB68 = 1; /* playerCanRun (asm 8009F6A0) */

        /* Stuck detection (asm 8009F6A8-8009F71C): count frames where a
         * 0x400000 move is requested but the position did not change. */
        if (*(u32*)(p + 0x14) & 0x00400000) {
            if (*(s16*)(p + 0x68) == *(s16*)(p + 0x22) &&
                *(s16*)(p + 0x6A) == *(s16*)(p + 0x26) &&
                *(s16*)(p + 0x6C) == *(s16*)(p + 0x2A)) {
                D_800ADB02++;
            }
        } else {
            D_800ADB02 = 0;
        }

        /* asm .L8009F720-.L8009F8FC: stuck-clamp + jump-button / NPC-talk
         * interaction sub-block. DEFERRED -- it needs several still-unported
         * event globals and does not affect the walk direction (every path
         * falls through to the direction computation below). Basic d-pad
         * walking works without it; jump/talk are a separate bounded feature. */

        /* Direction from d-pad + camera angle (asm .L8009F910-8009F9A4). */
        {
            u16 nibble = (u16)(D_800AFE9C >> 12);
            u16 angle;
            if (D_800B2354 == 0) {
                angle = D_800ADF68[nibble ^ 0xF];
            } else {
                angle = D_800ADF88[nibble ^ 0xF];
            }
            if ((angle & 0x8000) == 0) {
                angle = (u16)((angle - *(s16*)((u8*)&g_CamInterpolation + 0x8)) & 0xFFF);
            }
            *(s16*)(p + 0x104) = (s16)angle;
        }
        g_FieldScriptVMCurActor->scriptInstructionPointer++;
        return;
    }

    /* ---- not-player-controlled idle branch (asm .L8009F9BC) ----
     * Writes scriptFlags at 0x00 (asm sw 0x0($a0)), not ->flags at 0x04. */
    if (D_800B21CE == 0) {
        *(u32*)(p + 0x00) = scriptFlags | 0x1000000;
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

void func_8009FDD4(void) {
    s32 result = func_8009FC10(D_800AFD1C);
    if (result != 0xFF) {
        u8* pGS = (u8*)g_pGameState;
        if (pGS[result + 0x22B1] == 0) {
            func_800AD4D4();
        }
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 1;
}

void func_8009FE4C(void) {
    u8 slot = SCRIPT_READ_U8_REL(1);
    if (g_GamePartyMembers[slot] != 0xFF) {
        u8* pGS = (u8*)g_pGameState;
        if (pGS[slot + 0x22B1] != 0) {
            func_800ACFD0();
        }
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
}

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

void func_800A0524(s32 srcIdx, s32 dstIdx) {
    u8* pSrcField = (u8*)g_FieldActors + srcIdx * 92;
    u8* pDstField = (u8*)g_FieldActors + dstIdx * 92;
    u8* pSrcData = *(u8**)(pSrcField + 0x4C);
    u8* pDstData = *(u8**)(pDstField + 0x4C);
    s32 i;
    u16* ps = (u16*)(pSrcData + 0x08);
    u16* pd = (u16*)(pDstData + 0x08);
    for (i = 0; i < 4; i++) {
        *pd++ = *ps++;
    }
    *(u16*)(pDstData + 0x10) = *(u16*)(pSrcData + 0x10);
    *(u16*)(pDstData + 0xEC) = *(u16*)(pSrcData + 0xEC);
    *(u16*)(pDstData + 0x72) = *(u16*)(pSrcData + 0x72);
    *(u32*)(pDstData + 0x50) = *(u32*)(pSrcData + 0x50);
    *(u32*)(pDstData + 0x54) = *(u32*)(pSrcData + 0x54);
    *(u32*)(pDstData + 0x58) = *(u32*)(pSrcData + 0x58);
    *(u32*)(pDstData + 0x20) = *(u32*)(pSrcData + 0x20);
    *(u32*)(pDstData + 0x24) = *(u32*)(pSrcData + 0x24);
    *(u32*)(pDstData + 0x28) = *(u32*)(pSrcData + 0x28);
    FieldMatrixCopyTransform((u8*)g_FieldActors + srcIdx * 92 + 0xC,
                              (u8*)g_FieldActors + dstIdx * 92 + 0xC);
    FieldMatrixCopyTranslation((u8*)g_FieldActors + srcIdx * 92 + 0xC,
                                (u8*)g_FieldActors + dstIdx * 92 + 0xC);
    {
        u8* pSrcSub = *(u8**)(pSrcField + 0x4C);
        u8* pDstSprite = *(u8**)(pDstField + 0x04);
        *(u32*)(pDstSprite + 0x00) = *(u32*)(pSrcSub + 0x20);
        *(u32*)(pDstSprite + 0x04) = *(u32*)(pSrcSub + 0x24);
        *(u32*)(pDstSprite + 0x08) = *(u32*)(pSrcSub + 0x28);
    }
}

extern void func_800A0C94(void);

// Sprite-load script opcode (0x121): bind the current actor's sprite from its
// character id -> party id, then set up status/flags. Sibling of
// func_800A08B8; the non-party path uses sprite mode 1 and the party path
// mode 2 (with the extra func_800A0C94 setup + status 0x20 clears). Stubbed
// before this -> the actor's pSpriteData stayed NULL and its later
// boot-script sprite writes crashed the host (MAP2 actor 3).
#ifdef XENO_PC_PORT
/* Coexistence (d88f13c pattern): logic-verified port C body binds the sprite
 * (fixes the MAP2 SEGV cluster); the matching build keeps INCLUDE_ASM below
 * (byte-exact). Residual vs {}: the transcription is only ~24% fuzzy -- the
 * flag/status RMW ordering and the two actor-cursor reloads schedule
 * differently under mwcc; semantics traced 1:1 against the asm
 * (30BF8-30DC4). Not claimed as {}. */
void func_800A06E8(void) {
    FieldActor* fieldActor = &g_FieldActors[D_800AFD1C];
    s32 characterId = func_8008CF3C(FieldScriptVMGetArgument(1));
    s32 partyId = FieldCharacterIdToPartyId(characterId);

    fieldActor->status = (fieldActor->status & 0xF07F) | 0x200;

    if (partyId == -1) {
        func_80076AC0(D_800AFD1C, 0, g_PartyDataBuffers[0], 1, 0, 0, 1);
        g_FieldScriptVMCurActor->scriptFlags.flags |= 0x1;
        g_FieldScriptVMCurActor->flags |= 0x100000;
        D_800AFFEC = 1;
        D_800B00C0 = 1;
    } else {
        func_80076AC0(D_800AFD1C, partyId, g_PartyDataBuffers[partyId], 2, 0,
                      partyId, 1);
        fieldActor = &g_FieldActors[D_800AFD1C];
        D_800AFD20 = -0xC0;
        fieldActor->status &= 0xFFDF;
        func_800A0C94();
        g_FieldScriptVMCurActor->scriptFlags.flags =
            (g_FieldScriptVMCurActor->scriptFlags.flags | 0x100) & ~0x80;
        fieldActor = &g_FieldActors[D_800AFD1C];
        fieldActor->status &= 0xFFDF;
    }

    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}
#else
INCLUDE_ASM("asm/field/nonmatchings/main/misc6", func_800A06E8);
#endif

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

    /* Retail 800A0944: partyId 0 (the player) additionally latches the
     * player-actor index and the 0x4400 flags; NON-ZERO party slots
     * (followers -- Citan is in the party from New Game) skip straight to
     * the shared bind tail (.L800A0978).  Everything below is shared. */
    if (partyId == 0) {
        g_PlayerActorIndex = D_800AFD1C;
        D_800B233E = D_800AFD1C;
        g_FieldScriptVMCurActor->scriptFlags.flags =
            (g_FieldScriptVMCurActor->scriptFlags.flags | 0x4400) & ~0x80;
    }
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

extern s32 D_800B2264;
extern s32 D_801E8670[];
extern void func_801E8030(s32 idx);

void func_800A0EE8(void) {
    ActorData* pActor = g_FieldScriptVMCurActor;
    u32 flags12C = *(u32*)((u8*)pActor + 0x12C);
    u32 motionIdx = (flags12C >> 13) & 7;
    u8 subOp = SCRIPT_READ_U8_REL(1);

    *(u32*)((u8*)pActor + 0x04) &= 0xFFFFDFFF;

    switch (subOp) {
        case 0: {
            u32* pTable = (u32*)(uintptr_t)D_801E8670;
            u32 pEntry = pTable[motionIdx];
            *(u8*)(pEntry + 0x34) = 0;
            g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
            break;
        }
        case 1: {
            func_801E8030((*(u32*)((u8*)pActor + 0x12C) >> 13) & 7);
            D_800B2264--;
            g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
            break;
        }
    }
    D_800B00C0 = 1;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc6", func_800A0FD8);

extern s32 D_800B2264;
extern u16 D_800B21DC[];
extern u8 D_800B225F[];

// Object-sprite load opcode: bind the current actor's sprite from the map's
// sprite package (g_FieldSpriteData + offset[1], the same package slot the
// sibling func_800A0D3C binds), reset its position, then register the
// argument (<<1) into the D_800B2264-indexed object-slot pair
// (D_800B21DC[]/D_800B225F[]) and stamp (slot&7)<<13 into the actor's +0x12C
// word.  Stubbed before this -> the actor's pSpriteData stayed NULL and its
// later sprite writes crashed the host (MAP3 boot SEGV at func_8009EB78).
#ifdef XENO_FIELD_OBJECT_OVERLAY
/* STAGED, OFF BY DEFAULT: this body is asm-verified (31874-319FC) and binds
 * the sprite correctly (MAP3 actor 4 probe), but ACTIVATING it arms the
 * whole object pipeline: D_800B2264 != 0 -> func_80077884/func_80077AB4
 * load the per-slot model archives and instantiate them through the
 * member_change_menu overlay (func_801E742C, UNPORTED/unextracted), and the
 * correctly-synced streams then reach func_800821F4's battle-animation
 * assert on maps 16/80 (which "play" today only via the FE07 desync this
 * pass fixed).  Flip this on when the object-overlay pass lands; until
 * then the matching-build INCLUDE_ASM below also serves the port (stub). */
void func_800A1364(void) {
    FieldActor* fieldActor = &g_FieldActors[D_800AFD1C];
    s32 arg;
    s32 slot;
    u8* cur;

    fieldActor->status = (fieldActor->status & 0xF07F) | 0x200;
    arg = FieldScriptVMGetArgument(1);

    func_80076AC0(D_800AFD1C, 0,
                  (void*)((*(s32*)(g_FieldSpriteData + 4)) +
                          (s32)g_FieldSpriteData),
                  0, 0, 0x80, 1);
    func_800A0C94();

    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
    g_FieldScriptVMCurActor->scriptFlags.flags |= 0x100;
    fieldActor = &g_FieldActors[D_800AFD1C];
    fieldActor->status &= 0xFFDF;
    g_FieldScriptVMCurActor->flags =
        (g_FieldScriptVMCurActor->flags | 0x2000) & ~0x800;

    slot = D_800B2264;
    D_800B21DC[slot] = (u16)(arg << 1);
    D_800B225F[slot] = 0;
    cur = (u8*)g_FieldScriptVMCurActor;
    *(u32*)(cur + 0x12C) =
        (*(u32*)(cur + 0x12C) & 0xFFFF1FFF) | ((u32)(slot & 7) << 13);
    D_800B2264 = slot + 1;
}
#else
INCLUDE_ASM("asm/field/nonmatchings/main/misc6", func_800A1364);
#endif

void func_800A14F0(void) {
    u8* actorData;
    u8* fieldActor;
    u8* spritePackage;
    s32 skinIndex;
    s32 arg3;

    fieldActor = (u8*)g_FieldActors + D_800AFD1C * 0x5C;
    *(u16*)(fieldActor + 0x58) = (*(u16*)(fieldActor + 0x58) & 0xF07F) | 0x200;

    skinIndex = FieldScriptVMGetArgument(1);
    spritePackage = (u8*)g_FieldSpriteData +
                    *(u32*)((u8*)g_FieldSpriteData + (skinIndex * 4) + 4);
    arg3 = FieldScriptVMGetArgument(3);
    func_80076AC0(D_800AFD1C, skinIndex, spritePackage, 0, arg3, skinIndex | 0x80, 1);
    func_800A0C94();

    actorData = (u8*)g_FieldScriptVMCurActor;
    *(u16*)(actorData + 0xCC) += 5;
    *(u32*)(actorData + 0x0) = (*(u32*)(actorData + 0x0) | 0x100) & ~0x80;
    *(u32*)(actorData + 0x4) &= ~0x800;

    fieldActor = (u8*)g_FieldActors + D_800AFD1C * 0x5C;
    *(u16*)(fieldActor + 0x58) &= ~0x20;
}

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
