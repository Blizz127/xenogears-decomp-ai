#include "common.h"
#include "main/game.h"
#include "system/math.h"
#include "field/main.h"
#include "field/camera.h"
#include "field/actor.h"
#include "field/script_vm.h"
#include "field/text_box.h"
#include "field/particles.h"
#include "field/effects.h"

void FieldScriptMemoryWriteU16(int, int);

extern int rcos(int);
extern int rsin(int);

extern s32 D_800AFD1C;
extern void func_80072254(s32);
extern void SpriteSetSpecialAnimFile(SpriteData*, void*);

void FieldSetScreenDimensions(void) {
    g_FieldRenderContexts[0].dispEnv.screen.x = 0;
    g_FieldRenderContexts[0].dispEnv.screen.y = 10;
    g_FieldRenderContexts[0].dispEnv.screen.w = 0x100;
    g_FieldRenderContexts[0].dispEnv.screen.h = 0xd8;
    g_FieldRenderContexts[1].dispEnv.screen.x = 0;
    g_FieldRenderContexts[1].dispEnv.screen.y = 10;
    g_FieldRenderContexts[1].dispEnv.screen.w = 0x100;
    g_FieldRenderContexts[1].dispEnv.screen.h = 0xd8;
}

extern u8 D_800B2358[]; // Is pause disabled?

void func_80086DE0(void) {
    D_800B2358[0] = SCRIPT_READ_U8_REL(1);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
}

extern s16 D_800ADB54;

void func_80086E1C(void) {
    RECT rect;
    switch (FieldScriptVMGetArgument(1)) {
        case 0:
            rect.w = 0x500;
            rect.x = 0;
            rect.y = 0;
            rect.h = 0x200;
            ClearImage(&rect, 0x0, 0x0, 0x0);
            DrawSync(0);
            Vsync(0);
            SetDefDrawEnv(g_FieldRenderContexts[0].drawEnvs, 0, 0, 0x280, 0xE0);
            SetDefDrawEnv(g_FieldRenderContexts[1].drawEnvs, 0, 0x100, 0x280, 0xE0);
            SetDefDispEnv(&g_FieldRenderContexts[0].dispEnv, 0, 0x100, 0x280, 0xE0);
            SetDefDispEnv(&g_FieldRenderContexts[1].dispEnv, 0, 0, 0x280, 0xE0);
            FieldSetScreenDimensions();
            break;
        case 1:
            D_800ADB54 = 0x1;
            break;
        case 2:
            D_800ADB54 = 0x0;
            break;
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

// Set model animation for actor
void func_80086F7C(void) {
    g_FieldScriptVMCurActor->modelAnimation = (FieldScriptVMGetArgument(1) << 0xC) | FieldScriptVMGetArgument(3);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
}

void func_80086FD0(void) {
    switch (SCRIPT_READ_U8_REL(1)) {
        case 0:
            // Initialize Sprite List
            func_800AAC08();
            g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
            return;
        case 2:
            // Free Sprite List
            func_800AABD8();
            g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
            return;
        case 1:
            // Set X, Y of Sprite and add it to OT for drawing
            func_800AAE4C(
                FieldScriptVMGetArgument(2), // Sprite Index
                FieldScriptVMGetArgument(4), // X
                FieldScriptVMGetArgument(6), // Y
                FieldScriptVMGetArgument(8)  // Sprite type
            );
            g_FieldScriptVMCurActor->scriptInstructionPointer += 0xA;
            return;
        case 3:
            // Set Sprite Color
            func_800AADC8(
                FieldScriptVMGetArgument(2), // Sprite Index
                FieldScriptVMGetArgument(4), // Red
                FieldScriptVMGetArgument(6), // Green
                FieldScriptVMGetArgument(8)  // Blue
            );
            g_FieldScriptVMCurActor->scriptInstructionPointer += 0xA;
            return;
    }
}

void func_80087148(void) {
    s32 a = FieldScriptVMGetArgument(1);
    s32 b = FieldScriptVMGetArgument(3);
    *(u16*)((u8*)g_pGameState + a * 0x20 + 0x16DA) |= b;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
}

extern s32 D_800C3A48;
extern s32 D_800AF87C;
extern s16 D_800AFC58;
extern s16 D_800AFC5A;
extern s16 D_800AFC5C;
extern s16 D_800AFC5E;
extern s32 D_800ADBB4;

void func_800871B0(void) {
    u8 subOp = SCRIPT_READ_U8_REL(1);
    g_FieldScriptMaxInstructionCount += 0x20;

    switch (subOp) {
        case 0: {
            s32 arg2 = FieldScriptVMGetArgument(2);
            s32 arg4 = FieldScriptVMGetArgument(4);
            s32 size = arg4 << 9;
            D_800C3A48 = (s32)HeapAlloc(size, 0);
            D_800AF87C = (s32)HeapAlloc(size, 0);
            D_800AFC58 = 0;
            D_800AFC5A = (s16)arg2;
            D_800AFC5C = 0x100;
            D_800AFC5E = (s16)arg4;
            StoreImage(&D_800AFC58, (void*)(uintptr_t)D_800C3A48);
            D_800B00C0 = 1;
            g_FieldScriptVMCurActor->scriptInstructionPointer += 6;
            break;
        }
        case 1: {
            s32 arg2 = FieldScriptVMGetArgument(2);
            s32 arg4 = FieldScriptVMGetArgument(4);
            s32 ofs = arg4 << 9;
            func_80026F44(0x100, arg4,
                (void*)(uintptr_t)(D_800AF87C + ofs),
                (void*)(uintptr_t)(D_800C3A48 + ofs));
            D_800ADBB4 = 1;
            g_FieldScriptVMCurActor->scriptInstructionPointer += 6;
            break;
        }
        case 2:
            HeapFree((void*)(uintptr_t)D_800C3A48);
            HeapFree((void*)(uintptr_t)D_800AF87C);
            D_800B00C0 = 1;
            g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
            break;
        case 3:
            g_FieldScriptVMCurActor->scriptInstructionPointer = *(u16*)((u8*)g_FieldScriptVMCurActor + 0xCC);
            D_800B00C0 = 1;
            break;
    }
}

extern u8 D_800B225F[];

void func_800873C4(void) {
    D_800B225F[FieldScriptVMGetArgument(1)] = FieldScriptVMGetArgument(3);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
}

void func_80087420(void) {
    int arg1, arg2, arg3, arg4, arg5, arg6;
    int value1, value2;

    arg1 = FieldScriptVMGetArgument(ARG(1));
    arg2 = FieldScriptVMGetArgument(ARG(2));
    arg3 = FieldScriptVMGetArgument(ARG(3));
    arg4 = FieldScriptVMGetArgument(ARG(4));
    arg5 = FieldScriptVMGetArgument(ARG(5));
    arg6 = FieldScriptVMGetArgument(ARG(6));

    value1 = CONV_TO_GTE(
        CONV_FROM_GTE(arg5) / arg3 * arg1
    );
    value2 = CONV_TO_GTE(
        CONV_FROM_GTE(arg6) / arg4 * arg2
    );
    
    FieldScriptMemoryWriteU16(
        SCRIPT_IMM_ARG(7), 
        value1
    );
    FieldScriptMemoryWriteU16(
        SCRIPT_IMM_ARG(8), 
        value2
    );
    
    g_FieldScriptVMCurActor->scriptInstructionPointer += 0x11;
}

void func_8008752C(void) {
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void func_8008754C(void) {
    g_pGameState->unk22B6 |= 0x4000;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 1;
}

void FieldScriptCopyGear(void) {
    int sourceGearIndex = FieldScriptVMGetArgument(1);
    g_pGameState->gears[FieldScriptVMGetArgument(3)] = g_pGameState->gears[sourceGearIndex];
    g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
}

void func_8008764C(void) {
    s32 srcIdx = FieldScriptVMGetArgument(1);
    s32 dstIdx = FieldScriptVMGetArgument(3);
    u8* pGS = (u8*)g_pGameState;
    u8* pSrc = pGS + srcIdx * 164 + 0x26C;
    u8* pDst = pGS + dstIdx * 164 + 0x26C;
    s32 i;
    /* Copy 164 bytes (10*16 + 4) in 16-byte chunks */
    for (i = 0; i < 160; i += 16) {
        *(s32*)(pDst + i)      = *(s32*)(pSrc + i);
        *(s32*)(pDst + i + 4)  = *(s32*)(pSrc + i + 4);
        *(s32*)(pDst + i + 8)  = *(s32*)(pSrc + i + 8);
        *(s32*)(pDst + i + 12) = *(s32*)(pSrc + i + 12);
    }
    *(s32*)(pDst + 160) = *(s32*)(pSrc + 160);
    /* Copy 32 bytes at offset 0x16C0 using unaligned access */
    pSrc = pGS + srcIdx * 32 + 0x16C0;
    pDst = pGS + dstIdx * 32 + 0x16C0;
    for (i = 0; i < 32; i += 4) {
        *(s32*)(pDst + i) = *(s32*)(pSrc + i);
    }
    /* Set party flags */
    if (srcIdx == 9) {
        *(u16*)(pGS + 0x22B6) |= 0x2000;
    }
    if (srcIdx == 10) {
        *(u16*)(pGS + 0x22B6) |= 0x1000;
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
}

extern u8 D_80050622;

void func_80087800(void) {
    FieldScriptMemoryWriteU16(
        SCRIPT_IMM_ARG(1), 
        D_80050622
    );
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

extern s32 D_800ADBDC;
extern s32 D_800ADBE4;
extern s32 D_800ADB2C;
extern s32 D_8004F308;
extern u8 D_8005061C, D_8005061D, D_8005061E, D_8005061F, D_80050620, D_80050621;
extern s32 D_800ADB88;
extern s32 D_800ADBE8;

void func_80087848(void) {
    if (D_800ADBDC == 0 || D_800ADBE4 == 0 || D_800ADB2C != 0 || D_8004F308 == -1) {
        D_800B00C0 = 1;
        g_FieldScriptVMCurActor->scriptInstructionPointer -= 1;
        return;
    }
    func_800379B4(0);
    D_8005061C = (u8)FieldScriptVMGetArgument(1);
    D_8005061D = (u8)FieldScriptVMGetArgument(3);
    D_8005061E = (u8)FieldScriptVMGetArgument(5);
    D_8005061F = (u8)FieldScriptVMGetArgument(7);
    D_80050620 = (u8)FieldScriptVMGetArgument(9);
    D_80050621 = (u8)FieldScriptVMGetArgument(11);
    D_800ADB88 = 1;
    D_800ADBE8 = 0;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 0xD;
}

void func_80087960(void) {
    FieldScriptMemoryWriteU16(SCRIPT_IMM_ARG(1), *(u16*)((u8*)g_pGameState + 0x1844));
    FieldScriptMemoryWriteU16(SCRIPT_IMM_ARG(2), *(u16*)((u8*)g_pGameState + 0x1846));
    g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
}

void func_800879D0(void) {
    FieldScriptMemoryWriteU16(SCRIPT_IMM_ARG(1), g_pGameState->unk184E);
    FieldScriptMemoryWriteU16(SCRIPT_IMM_ARG(2), g_pGameState->unk1852);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
}

extern u8 D_800B2357;

void func_80087A40(void) {
    D_800B2357 = SCRIPT_READ_U8_REL(1);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
}

extern u8 D_800B2354;

void func_80087A7C(void) {
    D_800B2354 = SCRIPT_READ_U8_REL(1);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
}

void func_80087AB8(void) {
    ActorData* pActor = g_FieldScriptVMCurActor;
    void* pScript = g_FieldScriptVMCurScriptData;
    u8 scriptByte = *(u8*)(pScript + pActor->scriptInstructionPointer + 9);
    u8* pGS = (u8*)g_pGameState;
    s32 arg1 = FieldScriptArgument1(1, scriptByte);
    *(s16*)(pGS + 0x184E) = (s16)arg1;
    {
        s32 arg3 = FieldScriptArgument2(3, scriptByte);
        *(s16*)(pGS + 0x1852) = (s16)arg3;
    }
    *(s16*)(pGS + 0x1854) = 0;
    *(s16*)(pGS + 0x1850) = 0;
    *(s16*)(pGS + 0x1856) = 1;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 6;
}

void func_80087B5C(void) {
    u8* pGS = (u8*)g_pGameState;
    u16 arg1 = (u16)FieldScriptVMGetInstructionArgument(1);
    FieldScriptMemoryWriteU16(arg1, *(u16*)(pGS + 0x182C));
    arg1 = (u16)FieldScriptVMGetInstructionArgument(3);
    FieldScriptMemoryWriteU16(arg1, *(u16*)(pGS + 0x182E));
    arg1 = (u16)FieldScriptVMGetInstructionArgument(5);
    FieldScriptMemoryWriteU16(arg1, *(u16*)(pGS + 0x1830));
    arg1 = (u16)FieldScriptVMGetInstructionArgument(7);
    FieldScriptMemoryWriteU16(arg1, *(u16*)(pGS + 0x1832));
    g_FieldScriptVMCurActor->scriptInstructionPointer += 9;
}

extern s32 D_8004F300;

void func_80087C0C(void) {
    D_8004F300 = 1;
    g_FieldScriptVMCurActor->scriptInstructionPointer++;
}

void func_80087C34(void) {
    u8* pGS = (u8*)g_pGameState;
    u8 mask;
    mask = SCRIPT_READ_U8_REL(9);
    *(s16*)(pGS + 0x182C) = (s16)FieldScriptArgument1(1, mask);
    mask = SCRIPT_READ_U8_REL(9);
    *(s16*)(pGS + 0x182E) = (s16)FieldScriptArgument2(3, mask);
    mask = SCRIPT_READ_U8_REL(9);
    *(s16*)(pGS + 0x1830) = (s16)FieldScriptArgument3(5, mask);
    mask = SCRIPT_READ_U8_REL(9);
    *(s16*)(pGS + 0x1832) = (s16)FieldScriptArgument4(7, mask);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 10;
}

void func_80087D30(void) {
    FieldScriptMemoryWriteU16(SCRIPT_IMM_ARG(1), *(u16*)((u8*)g_pGameState + 0x1834));
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void func_80087D80(void) {
    *(u16*)((u8*)g_pGameState + 0x1834) = FieldScriptArgument1(1, SCRIPT_READ_U8_REL(3));
    g_FieldScriptVMCurActor->scriptInstructionPointer += 4;
}

extern u8 D_800B2355;
extern u8 D_800B2356;

void func_80087DE0(void) {
    int arg = FieldScriptVMGetArgument(2);
    if (SCRIPT_READ_U8_REL(1) == 0) {
        D_800B2355 = arg;
    } else {
        D_800B2356 = arg;
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 4;
}

extern s16 D_800B234C;

void func_80087E5C(void) {
    D_800B234C = FieldScriptVMGetArgument(1);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

extern s32 D_8005A444[];
extern s32 D_800ADBFC;
extern s32 g_PlayerActorIndex;
extern s16 D_800B233E;
extern s16 D_800B234E;

void func_80087E98(void) {
    s32 actorIdx = FieldScriptVMGetActorIndex(1);
    if (actorIdx != 0xFF) {
        s32 i;
        u32 mask = 0xFEFFBFFF;
        if (actorIdx == D_8005A444[0]) {
            D_800B234E = 0;
        } else {
            D_800B234E = 1;
        }
        g_PlayerActorIndex = actorIdx;
        D_800B233E = (s16)actorIdx;
        for (i = 0; i < D_800ADBFC; i++) {
            u8* pFieldActor = (u8*)g_FieldActors + i * 92;
            u32 pSub = *(u32*)(pFieldActor + 0x4C);
            *(u32*)(pSub) &= mask;
        }
        {
            u8* pFieldActor = (u8*)g_FieldActors + actorIdx * 92;
            u32 pSub = *(u32*)(pFieldActor + 0x4C);
            *(u32*)(pSub) |= 0x4000;
        }
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
}

extern u16 D_800B2348;

void func_80087FA4(void) {
    D_800B2348++;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 1;
}

void func_80087FD4(void) {
    func_800A8BA4();
    g_FieldScriptVMCurActor->scriptInstructionPointer += 1;
}

void func_8008800C(void) {
    MATRIX mtx;
    SVECTOR rot;
    SVECTOR vec;
    SVECTOR out;
    u8 mask;
    s32 arg1, arg2, arg3, arg4, arg5;

    mask = SCRIPT_READ_U8_REL(0xB);
    arg1 = FieldScriptArgument1(1, mask);
    mask = SCRIPT_READ_U8_REL(0xB);
    arg2 = FieldScriptArgument2(3, mask);
    mask = SCRIPT_READ_U8_REL(0xB);
    arg3 = FieldScriptArgument3(5, mask);
    mask = SCRIPT_READ_U8_REL(0xB);
    arg4 = FieldScriptArgument4(7, mask);
    mask = SCRIPT_READ_U8_REL(0xB);
    arg5 = FieldScriptArgument5(9, mask);

    rot.vx = (s16)arg3;
    rot.vy = (s16)arg4;
    rot.vz = (s16)arg5;
    vec.vx = 0;
    vec.vy = 0;
    vec.vz = 0;

    func_801E72CC(&mtx, NULL, arg2, arg1);
    SetRotMatrix(&mtx);
    SetTransMatrix(&mtx);
    RotTransSV(&vec, &out, (long*)&out);

    {
        u16 addr;
        addr = (u16)FieldScriptVMGetInstructionArgument(0xC);
        FieldScriptMemoryWriteU16(addr, out.vx);
        addr = (u16)FieldScriptVMGetInstructionArgument(0xE);
        FieldScriptMemoryWriteU16(addr, out.vy);
        addr = (u16)FieldScriptVMGetInstructionArgument(0x10);
        FieldScriptMemoryWriteU16(addr, out.vz);
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 0x12;
}

/* Field-script opcode: snapshot 20 GameState per-slot entries (+0x9DC->+0x9D8
 * word, +0x9B2->+0x9B0 half, 0xA4 stride), then IP += 1. As a no-op stub this
 * never advanced the IP -> the VM desynced MAP3's script and fed garbage actor
 * index 128 to func_8009EB78 (the boot SEGV) -- same class as the FE07 fix. */
void func_80088198(void) {
    u8* p = (u8*)g_pGameState;
    int i;

    for (i = 0; i < 0x14; i++) {
        *(u32*)(p + 0x9D8) = *(u32*)(p + 0x9DC);
        *(u16*)(p + 0x9B0) = *(u16*)(p + 0x9B2);
        p += 0xA4;
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 1;
}

extern void func_800A915C(void);
extern void func_800A91F0(void);

void func_800881E8(void) {
    ActorData* pActor = g_FieldScriptVMCurActor;
    void* pScript = g_FieldScriptVMCurScriptData;
    u8 val = *(u8*)(pScript + pActor->scriptInstructionPointer + 1);
    if (val == 0) {
        func_800A915C();
    } else {
        func_800A91F0();
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
}

extern s32 D_8004F308;

void func_8008825C(void) {
    if (D_8004F308 == -1) {
        g_FieldScriptVMCurActor->scriptInstructionPointer -= 1;
    } else {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 1;
    }
    D_800B00C0 = 1;
}

void func_800882B8(void) {
    s32 charId = func_8008CF3C(FieldScriptVMGetArgument(1));
    u16 dest = (u16)FieldScriptVMGetInstructionArgument(3);
    if (charId != 0xFF) {
        u8* pChar = (u8*)g_pGameState + charId * 164;
        FieldScriptMemoryWriteU16(dest, pChar[0x30C]);
    } else {
        FieldScriptMemoryWriteU16(dest, 0xFF);
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
}

void FieldScriptSetCharacterGear(void) {
    int characterId = FieldScriptVMGetArgument(1);
    g_pGameState->characters[characterId].gearId = FieldScriptVMGetArgument(3);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
}

void func_800883D4(void) {
    s32 charId = func_8008CF3C(FieldScriptVMGetArgument(2));
    if (charId != 0xFF) {
        ActorData* pActor = g_FieldScriptVMCurActor;
        void* pScript = g_FieldScriptVMCurScriptData;
        u8 scriptByte = *(u8*)(pScript + pActor->scriptInstructionPointer + 1);
        u8* pGS = (u8*)g_pGameState;
        u16 flags = *(u16*)(pGS + 0x2318);
        u32 mask = 1u << charId;
        if (scriptByte == 0) {
            *(u16*)(pGS + 0x2318) = (u16)(flags | mask);
        } else {
            *(u16*)(pGS + 0x2318) = (u16)(flags & ~mask);
        }
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 4;
}

extern s16 D_800B236C;

void func_8008848C(void) {
    D_800B236C = SCRIPT_READ_U8_REL(1) ^ 1;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
}

extern s16 D_800B21AC;

void func_800884CC(void) {
    D_800B21AC = FieldScriptVMGetArgument(1);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

extern s32 D_8005A444[];

void func_80088508(void) {
    s32 arg5 = FieldScriptVMGetArgument(5);
    s32 slot = D_8005A444[arg5];
    u16 arg1, arg3;
    g_FieldScriptMaxInstructionCount += 4;

    if (slot == 0xFF) {
        arg1 = (u16)FieldScriptVMGetInstructionArgument(1);
        FieldScriptMemoryWriteU16(arg1, 0);
        arg3 = (u16)FieldScriptVMGetInstructionArgument(3);
        FieldScriptMemoryWriteU16(arg3, 0);
    } else {
        u32 pFieldActor = (u32)((u8*)g_FieldActors + slot * 92);
        u32 pActorData = *(u32*)(pFieldActor + 0x04);
        u32 pSub;
        u16 val;
        arg1 = (u16)FieldScriptVMGetInstructionArgument(1);
        pSub = *(u32*)(pActorData + 0x7C);
        val = *(u16*)(pSub + 0x0C);
        FieldScriptMemoryWriteU16(arg1, val);
        arg3 = (u16)FieldScriptVMGetInstructionArgument(3);
        FieldScriptMemoryWriteU16(arg3, (u16)slot);
        pSub = *(u32*)(pActorData + 0x7C);
        val = *(u16*)(pSub + 0x0C);
        if (val != 1) {
            *(u16*)(pSub + 0x0C) = 0;
        }
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 7;
}

void func_8008861C(void)
{
    s32 i;
    u8* pBanks = (u8*)g_FieldDefaultParticleBanks;
    for (i = 0; i < 8; i++) {
        u8* pEntry = pBanks + i * 112;
        *(u16*)(pEntry + 0x32) = 0;
        *(u16*)(pEntry + 0x30) = 0;
    }
}

extern s32 D_800B2374;
extern s32 D_800B2378;
extern s32 D_800B237C;
extern s32 D_800B2380;
extern void FieldInitializeDefaultParticleBanks(s32 actorIdx);

void func_80088674(void) {
    s32 actorIdx = FieldScriptVMGetArgument(1);
    if (actorIdx == 0xFF) { actorIdx = 0; }
    D_800B2374 = FieldScriptVMGetArgument(1);
    D_800B2378 = FieldScriptVMGetArgument(3);
    D_800B237C = FieldScriptVMGetArgument(5);
    D_800B2380 = FieldScriptVMGetArgument(7);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 9;
    FieldInitializeDefaultParticleBanks(actorIdx);
    switch (D_800B2378) {
        case 0: D_800B2378 = 0; break;
        case 1: D_800B2378 = 0x10; break;
        case 2: D_800B2378 = 0x20; break;
        case 3: D_800B2378 = 0x30; break;
    }
    g_FieldScriptMaxInstructionCount += 4;
}

void func_80088790(void) {
    s32 actorIdx = FieldScriptVMGetActorIndex(1);
    if (actorIdx == 0xFF) { actorIdx = 0; }
    D_800B2374 = actorIdx;
    D_800B2378 = FieldScriptVMGetArgument(2);
    D_800B237C = FieldScriptVMGetArgument(4);
    D_800B2380 = FieldScriptVMGetArgument(6);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 8;
    FieldInitializeDefaultParticleBanks(actorIdx);
    switch (D_800B2378) {
        case 0: D_800B2378 = 0; break;
        case 1: D_800B2378 = 0x10; break;
        case 2: D_800B2378 = 0x20; break;
        case 3: D_800B2378 = 0x30; break;
    }
    g_FieldScriptMaxInstructionCount += 4;
}

void func_800888A4(void) {
    u32 actorIdx = (u32)D_800AFD1C;
    u8* pFieldActor = (u8*)g_FieldActors + actorIdx * 92;
    u32 pActorData = *(u32*)(pFieldActor + 0x04);
    s32 arg1 = FieldScriptVMGetArgument(1);
    s32 dirBits = arg1 & 0xF;
    s32 arg3 = FieldScriptVMGetArgument(3);
    s32 speed = (arg3 >> 4 << 8) | (arg3 & 0xF);
    u8* pActor = (u8*)g_FieldScriptVMCurActor;
    u32 pSub;

    func_8002303C(pActorData, 2, 0);

    pSub = *(u32*)(pActorData + 0x7C);
    pSub = *(u32*)(pSub + 0x18);
    *(s16*)(pSub + 0x04) = (s16)(dirBits << 6);

    {
        u32 flags12C = *(u32*)(pActor + 0x12C);
        flags12C = (flags12C & 0xF003FFFF) | (dirBits << 24);
        *(u32*)(pActor + 0x12C) = flags12C;
    }

    pSub = *(u32*)(pActorData + 0x7C);
    pSub = *(u32*)(pSub + 0x18);
    *(s16*)(pSub + 0x06) = (s16)speed;

    {
        u32 flags130 = *(u32*)(pActor + 0x130);
        u32 flags12C = *(u32*)(pActor + 0x12C);
        flags130 = (flags130 & 0xFFFFFE00) | (speed & 0x1FF);
        flags12C = (flags12C & 0xFFFCFFFF) | 0x10000;
        *(u32*)(pActor + 0x130) = flags130;
        *(u32*)(pActor + 0x12C) = flags12C;
    }

    *(u16*)(pActor + 0xCC) += 5;
}

void func_800889BC(void) {
    u32 actorIdx = (u32)D_800AFD1C;
    u8* pFieldActor = (u8*)g_FieldActors + actorIdx * 92;
    u32 pActorData = *(u32*)(pFieldActor + 0x04);
    s32 arg1 = FieldScriptVMGetArgument(1);
    s32 dirBits = arg1 & 0xF;
    s32 arg1b = FieldScriptVMGetArgument(1);
    s32 speed = ((s32)arg1b >> 4 << 8) | (arg1b & 0xF);
    s32 arg5 = FieldScriptVMGetArgument(5);
    s32 dirBits2 = arg5 & 0xF;
    s32 arg7 = FieldScriptVMGetArgument(7);
    s32 speed2 = ((s32)arg7 >> 4 << 8) | (arg7 & 0xF);
    u8* pActor = (u8*)g_FieldScriptVMCurActor;
    u32 pSub;

    func_8002303C((void*)pActorData, 3, 0);

    pSub = *(u32*)(pActorData + 0x7C);
    pSub = *(u32*)(pSub + 0x18);
    *(s16*)(pSub + 0x04) = (s16)(dirBits << 6);

    {
        u32 flags12C = *(u32*)(pActor + 0x12C);
        *(u32*)(pActor + 0x12C) = (flags12C & 0xF003FFFF) | (dirBits << 24);
    }

    pSub = *(u32*)(pActorData + 0x7C);
    pSub = *(u32*)(pSub + 0x18);
    *(s16*)(pSub + 0x06) = (s16)speed;

    {
        u32 flags130 = *(u32*)(pActor + 0x130);
        *(u32*)(pActor + 0x130) = (flags130 & 0xFFFFFE00) | (speed & 0x1FF);
    }

    pSub = *(u32*)(pActorData + 0x7C);
    pSub = *(u32*)(pSub + 0x18);
    *(s16*)(pSub + 0x08) = (s16)(dirBits2 << 6);

    {
        u32 flags130 = *(u32*)(pActor + 0x130);
        *(u32*)(pActor + 0x130) = (flags130 & 0xFFF801FF) | (dirBits2 << 15);
    }

    pSub = *(u32*)(pActorData + 0x7C);
    pSub = *(u32*)(pSub + 0x18);
    *(s16*)(pSub + 0x0A) = (s16)speed2;

    {
        u32 flags130 = *(u32*)(pActor + 0x130);
        u32 flags12C = *(u32*)(pActor + 0x12C);
        *(u32*)(pActor + 0x130) = (flags130 & 0xF007FFFF) | ((speed2 & 0x1FF) << 19);
        *(u32*)(pActor + 0x12C) = (flags12C & 0xFFFCFFFF) | 0x20000;
    }

    *(u16*)(pActor + 0xCC) += 9;
}

// Start of particle handlers
extern s32 D_800ADB40;
extern s32 D_800ADB8C;
extern s32 D_800B2374;
extern s32 D_800B2378;
extern s32 D_800B237C;
extern s32 D_800B2380;

typedef struct {
    /* 0x0 */ int bankIndex;
    /* 0x4 */ u8 unk4[0xC];
} ParticleBankHandle;

extern ParticleBankHandle D_800B2384;
extern s32 g_FieldScriptMaxInstructionCount;

void func_80088B68(void) {
    s32 arg1 = FieldScriptVMGetArgument(1);
    u32 flagBit = 0;
    if (arg1 == 1) {
        flagBit = 0x80;
    } else if (arg1 == 2) {
        flagBit = 0x40;
    }
    {
        u32 bankIdx = D_800B2384.bankIndex;
        u32 stride = (bankIdx * 16 - bankIdx) * 8;
        u8* pBank = (u8*)g_FieldDefaultParticleBanks + stride;
        u16 flags = *(u16*)(pBank + 0x2A);
        *(u16*)(pBank + 0x2A) = (u16)(flags | flagBit);
    }
    g_FieldScriptMaxInstructionCount += 4;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 7;
}

void func_80088C1C(void) {
    u32 bankIdx = D_800B2384.bankIndex;
    u32 stride = (bankIdx * 16 - bankIdx) * 8;
    u8* pBank = (u8*)g_FieldDefaultParticleBanks + stride;

    s32 arg1 = FieldScriptVMGetArgument(1);
    *(s16*)(pBank + 0x24) = (s16)arg1;

    arg1 = FieldScriptVMGetArgument(3);
    {
        u16 flags = *(u16*)(pBank + 0x2A);
        *(u16*)(pBank + 0x2A) = (u16)(flags | (arg1 << 8));
    }

    arg1 = FieldScriptVMGetArgument(5);
    *(s16*)(pBank + 0x76) = (s16)arg1;

    g_FieldScriptMaxInstructionCount += 4;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 7;
}

void func_80088CF8() {
    FieldScriptSetParticleBankDirections(0);
}

void func_80088D18() {
    FieldScriptSetParticleBankDirections(4);
}

void FieldScriptSetParticleBankDirections(int index) {
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].directions[index].x = FieldScriptArgument1(1, SCRIPT_READ_U8_REL(0x11));
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].directions[index].z = FieldScriptArgument2(3, SCRIPT_READ_U8_REL(0x11));
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].directions[index + 1].x = FieldScriptArgument3(5, SCRIPT_READ_U8_REL(0x11));
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].directions[index + 1].z = FieldScriptArgument4(7, SCRIPT_READ_U8_REL(0x11));
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].directions[index + 2].x = FieldScriptArgument5(9, SCRIPT_READ_U8_REL(0x11));
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].directions[index + 2].z = FieldScriptArgument6(0xB, SCRIPT_READ_U8_REL(0x11));
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].directions[index + 3].x = FieldScriptArgument7(0xD, SCRIPT_READ_U8_REL(0x11));
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].directions[index + 3].z = FieldScriptArgument8(0xF, SCRIPT_READ_U8_REL(0x11));
    g_FieldScriptMaxInstructionCount += 4;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 0x12;
}

void FieldScriptInitializeParticleBank(void) {
    D_800B2384.bankIndex = FieldScriptVMGetArgument(1);
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].speedMultiplier = 1;
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].targetActorID = D_800B2374;
    D_800ADB40 = g_FieldDefaultParticleBanks[D_800B2384.bankIndex].targetActorID;
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].unk0 = 0;
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].rotAngle = 0;
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].max = FieldScriptVMGetArgument(3);
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].swait = FieldScriptVMGetArgument(5);
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].ewait = FieldScriptVMGetArgument(7);
    func_8008861C();
    g_FieldScriptMaxInstructionCount += 4;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 9;
}

void FieldScriptSetParticleBankPosition(void) {
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].pos.vx = FieldScriptArgument1(1, SCRIPT_READ_U8_REL(0xD));
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].pos.vy  = FieldScriptArgument2(3, SCRIPT_READ_U8_REL(0xD));
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].pos.vz = FieldScriptArgument3(5, SCRIPT_READ_U8_REL(0xD));
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].epos.vx = FieldScriptArgument4(7, SCRIPT_READ_U8_REL(0xD));
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].epos.vy = FieldScriptArgument5(9, SCRIPT_READ_U8_REL(0xD));
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].epos.vz = FieldScriptArgument6(0xB, SCRIPT_READ_U8_REL(0xD));
    g_FieldScriptMaxInstructionCount += 4;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 0xE;
}

void FieldScriptSetParticleBankPhysics(void) {
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].speed = FieldScriptArgument1(1, SCRIPT_READ_U8_REL(0xD));
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].gravity.vx = FieldScriptArgument2(3, SCRIPT_READ_U8_REL(0xD));
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].gravity.vy = FieldScriptArgument3(5, SCRIPT_READ_U8_REL(0xD));
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].gravity.vz = FieldScriptArgument4(7, SCRIPT_READ_U8_REL(0xD));
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].srange = FieldScriptArgument5(9, SCRIPT_READ_U8_REL(0xD));
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].erange = FieldScriptArgument6(0xB, SCRIPT_READ_U8_REL(0xD));
    g_FieldScriptMaxInstructionCount += 4;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 0xE;
}

void FieldScriptSetParticleBankParameters(void) {
    int flags;
    int flags_2;

    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].pswait = FieldScriptVMGetArgument(1);
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].pewait = FieldScriptVMGetArgument(3);
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].shape = FieldScriptVMGetArgument(5);
    flags = FieldScriptVMGetArgument(7);
    flags_2 = (FieldScriptVMGetArgument(9) * 2);
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].flags = flags | flags_2 | D_800B2378;
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].unk72 = D_800B237C;
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].unk74 = D_800B2380;
    g_FieldScriptMaxInstructionCount += 4;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 0xB;
}

void FieldScriptSetParticleBankScale(void) {
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].scale.vx = FieldScriptArgument1(1, SCRIPT_READ_U8_REL(0x9));
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].scale.vy = FieldScriptArgument2(3, SCRIPT_READ_U8_REL(0x9));
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].scale.vz = 0;
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].scaleDelta.vx = FieldScriptArgument3(5, SCRIPT_READ_U8_REL(0x9));
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].scaleDelta.vy = FieldScriptArgument4(7, SCRIPT_READ_U8_REL(0x9));
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].scaleDelta.vz = 0;
    g_FieldScriptMaxInstructionCount += 4;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 0xA;
}

void FieldScriptSetParticleBankColor(void) {
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].color.r  = FieldScriptArgument1(1, SCRIPT_READ_U8_REL(0xD));
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].color.g = FieldScriptArgument2(3, SCRIPT_READ_U8_REL(0xD));
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].color.b = FieldScriptArgument3(5, SCRIPT_READ_U8_REL(0xD));
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].colorDelta.r = FieldScriptArgument4(7, SCRIPT_READ_U8_REL(0xD));
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].colorDelta.g = FieldScriptArgument5(9, SCRIPT_READ_U8_REL(0xD));
    g_FieldDefaultParticleBanks[D_800B2384.bankIndex].colorDelta.b = FieldScriptArgument6(0xB, SCRIPT_READ_U8_REL(0xD));
    g_FieldScriptMaxInstructionCount += 4;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 0xE;
}

void FieldScriptParticlesInitialize(void) {
    g_FieldScriptMaxInstructionCount += 4;
    if (D_800ADB8C == 0) {
        FieldInitializeParticleBanks(D_800AFD1C);
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 1;
}

void FieldScriptStopParticleActor(void) {
    g_FieldScriptMaxInstructionCount += 4;
    FieldParticleActorStop(D_800AFD1C, SCRIPT_READ_U8_REL(0x1));
    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
}
// End of particle handlers

extern s32 D_8005A444[];
extern s32 D_800AFD1C;

void func_80089B54(void) {
    s32 i;
    for (i = 0; i < 3; i++) {
        if (D_8005A444[i] == D_800AFD1C) {
            u16 arg = (u16)FieldScriptVMGetInstructionArgument(1);
            FieldScriptMemoryWriteU16(arg, i);
            g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
            return;
        }
    }
    {
        u16 arg = (u16)FieldScriptVMGetInstructionArgument(1);
        FieldScriptMemoryWriteU16(arg, 0xFF);
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

extern s16 D_800B22E8[];
extern s16 D_800B22EA[];
extern s16 D_800B22EC[];
extern s16 D_800B2300[];
extern s16 D_800B2302[];
extern s16 D_800B2304[];
extern s32 D_800B2318[];

void func_80089BF0(void) {
    s32 idx;
    s32 arg1 = FieldScriptArgument1(1, SCRIPT_READ_U8_REL(0x11));
    idx = arg1 * 4;
    D_800B22E8[idx] = (s16)FieldScriptArgument2(3, SCRIPT_READ_U8_REL(0x11));
    D_800B22EA[idx] = (s16)FieldScriptArgument3(5, SCRIPT_READ_U8_REL(0x11));
    D_800B22EC[idx] = (s16)FieldScriptArgument4(7, SCRIPT_READ_U8_REL(0x11));
    D_800B2300[idx] = (s16)FieldScriptArgument5(9, SCRIPT_READ_U8_REL(0x11));
    D_800B2302[idx] = (s16)FieldScriptArgument6(0xB, SCRIPT_READ_U8_REL(0x11));
    D_800B2304[idx] = (s16)FieldScriptArgument7(0xD, SCRIPT_READ_U8_REL(0x11));
    D_800B2318[arg1] = FieldScriptArgument8(0xF, SCRIPT_READ_U8_REL(0x11));
    g_FieldScriptVMCurActor->scriptInstructionPointer += 0x12;
}

extern s16 D_800B2324[];
extern s16 D_800B2326[];
extern s16 D_800B2328[];
extern s16 D_800B22E2[];

void func_80089DCC(void) {
    u8 mask;
    s32 arg1, arg2, idx, actorIdx;
    mask = SCRIPT_READ_U8_REL(9);
    arg1 = FieldScriptArgument1(1, mask);
    mask = SCRIPT_READ_U8_REL(9);
    arg2 = FieldScriptArgument2(3, mask);
    idx = arg1 * 4;
    D_800B2324[idx] = (s16)arg2;
    mask = SCRIPT_READ_U8_REL(9);
    D_800B2328[idx] = (s16)FieldScriptArgument3(5, mask);
    mask = SCRIPT_READ_U8_REL(9);
    D_800B2326[idx] = (s16)FieldScriptArgument4(7, mask);
    actorIdx = FieldScriptVMGetActorIndex(0xA);
    if (actorIdx == 0xFF) {
        D_800B22E2[arg1] = -1;
    } else {
        D_800B22E2[arg1] = (s16)actorIdx;
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 0xB;
}

extern s16 D_800B22E0;

void func_80089F18(void) {
    D_800B22E0 = FieldScriptVMGetArgument(1);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

extern u8 D_800B21D2;

void func_80089F54(void) {
    D_800B21D2 = FieldScriptVMGetArgument(1) - 0x80;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

extern s32 D_800AFE84;

void func_80089F94(void) {
    D_800AFE84 = SCRIPT_READ_U8_REL(1);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
}

extern s16 D_800B0080, D_800B0082, D_800B0084, D_800B0086, D_800B0088, D_800B008A, D_800B008C, D_800B008E;

void func_80089FD0(void) {
    D_800B0080 = (s16)FieldScriptVMGetInstructionArgument(1);
    D_800B0082 = (s16)FieldScriptVMGetInstructionArgument(3);
    {
        s16 arg5 = (s16)FieldScriptVMGetInstructionArgument(5);
        D_800B0084 = arg5;
        if ((s16)arg5 == 0) {
            D_800B0084 = arg5 + 1;
        }
    }
    D_800B0086 = (s16)FieldScriptVMGetInstructionArgument(7);
    D_800B0088 = 0;
    D_800B008A = (s16)FieldScriptVMGetInstructionArgument(9);
    D_800B008C = (s16)FieldScriptVMGetInstructionArgument(11);
    D_800B008E = (s16)FieldScriptVMGetInstructionArgument(13);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 15;
}

extern s32 D_800B0090, D_800B0094, D_800B0098;

void func_8008A08C(void) {
    u8 mask = SCRIPT_READ_U8_REL(7);
    D_800B0090 = FieldScriptArgument1(1, mask);
    mask = SCRIPT_READ_U8_REL(7);
    D_800B0098 = FieldScriptArgument2(3, mask);
    mask = SCRIPT_READ_U8_REL(7);
    D_800B0094 = FieldScriptArgument3(5, mask);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 8;
}

extern u8 D_800B00A0, D_800B00A1, D_800B00A2, D_800B00A4, D_800B00A5, D_800B00A6, D_800B00A8, D_800B00A9, D_800B00AA;
extern s16 D_800B00AC, D_800B00AE, D_800B00B0, D_800B00B2;

void func_8008A148(void) {
    D_800B00A0 = (u8)FieldScriptVMGetArgument(1);
    D_800B00A1 = (u8)FieldScriptVMGetArgument(3);
    D_800B00A2 = (u8)FieldScriptVMGetArgument(5);
    D_800B00A4 = (u8)FieldScriptVMGetArgument(7);
    D_800B00A5 = (u8)FieldScriptVMGetArgument(9);
    D_800B00A6 = (u8)FieldScriptVMGetArgument(11);
    D_800B00A8 = (u8)FieldScriptVMGetArgument(13);
    D_800B00A9 = (u8)FieldScriptVMGetArgument(15);
    D_800B00AA = (u8)FieldScriptVMGetArgument(17);
    D_800B00AC = (s16)FieldScriptVMGetArgument(19);
    D_800B00AE = (s16)FieldScriptVMGetArgument(21);
    D_800B00B0 = (s16)FieldScriptVMGetArgument(23);
    D_800B00B2 = 1;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 25;
}

extern s32 D_800ADB88;

void func_8008A244(void) {
    if (D_800ADB88 == 0) {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 1;
    } else {
        g_FieldScriptVMCurActor->scriptInstructionPointer -= 1;
    }
    D_800B00C0 = 1;
}

extern s32 D_800B06A0;

void func_8008A2A0(void) {
    FieldScriptMemoryWriteU16(SCRIPT_IMM_ARG(1), D_800B06A0);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008A2E8);

void func_8008A4E0(void) {}
void func_8008A4E8(void) {}
void func_8008A4F0(void) {}
void func_8008A4F8(void) {}
void func_8008A500(void) {}
void func_8008A508(void) {}
void func_8008A510(void) {}
void func_8008A518(void) {}

void func_8008A520(void) {
    while (func_8008A558()) {
        Vsync(0);
    }
}

extern s32 D_800ADB2C;
extern s32 D_800ADB90;
extern s32 D_800ADB1C;

s32 func_8008A558(void) {
    if (D_800ADB2C != 0) {
        return -1;
    }
    if (ArchiveDataSync() != 0) {
        return -1;
    }
    ArchiveCdDataSync(0);
    return 0;
}

void func_8008A5A0(void) {
    if (SCRIPT_READ_U8_REL(1) == 0) {
        func_8003633C(0);
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 1;
}

extern s16 D_800B21D4;

void func_8008A604(void) {
    D_800B21D4 = FieldScriptVMGetArgument(1);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void func_8008A640(void) {
    s32 charId = func_8008CF3C(FieldScriptVMGetArgument(3));
    if (charId != 0xFF) {
        s32 arg1 = FieldScriptVMGetArgument(1);
        u8* pChar = (u8*)g_pGameState + charId * 164;
        u8 currentVal = *(pChar + 0x2E3);
        s32 newVal = arg1 - currentVal;
        if (newVal < 0) newVal = 0;
        *(pChar + 0x2E4) = (u8)newVal;
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
}

void func_8008A6E0(void) {
    s32 charId = func_8008CF3C(FieldScriptVMGetArgument(3));
    u16 dest;
    s32 val;
    if (charId != 0xFF) {
        u8* pChar = (u8*)g_pGameState + charId * 164;
        val = (s32)pChar[0x2E3] + (s32)pChar[0x2E4];
        dest = (u16)FieldScriptVMGetInstructionArgument(1);
        FieldScriptMemoryWriteU16(dest, val);
    } else {
        dest = (u16)FieldScriptVMGetInstructionArgument(1);
        FieldScriptMemoryWriteU16(dest, 0);
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
}

s32 func_8008A790(s32 value, s32* outIndex) {
    s32 i;
    for (i = 0; i < MAX_PARTY_MEMBERS; i++) {
        if (g_GamePartyMembers[i] == value) {
            return -1;
        }
        if (g_GamePartyMembers[i] == 0xFF) {
            *outIndex = i;
            return 0;
        }
    }
    return -1;
}

/* Skin-archive kickoff for a joining member (asm 8008A7DC): stage the
 * character skin (field maps, archive charId+5) or gear skin (gear maps,
 * GameCharacterGetGearID+0x10+5; 0xFF -> gear 0x10) into a heap staging
 * buffer via an async archive read, record slot/charId/buffer in
 * D_800ADBCC/C8/C0, and mark the load in flight (D_800ADBC4 = 1). */
extern s32 D_800ADB1C;
extern s32 D_800ADBC4;
extern s32 D_800ADBC8;
extern s32 D_800ADBCC;
extern void* D_800ADBC0;
extern s32 g_GameSceneMapNum;
extern s32 g_GamePartyMemberSkins[];
extern s32 GameCharacterGetGearID(s32 charId);

void func_8008A7DC(s32 charId, s32 slot) {
    s32 fileId;
    void* buf;

    D_800ADBCC = slot;
    D_800ADBC8 = charId;
    ArchiveSetIndex(4, 0);
    if (D_800ADB1C == 0) {
        ArchiveCdDataSync(0);
    }
    if ((g_GameSceneMapNum & 0xC000) == 0) {
        fileId = charId + 5;
        g_GamePartyMemberSkins[slot] = charId;
        buf = HeapAlloc(ArchiveDecodeAlignedSize(fileId), 0);
        D_800ADBC0 = buf;
    } else {
        s32 gearId = GameCharacterGetGearID(charId);

        if (gearId == 0xFF) {
            gearId = 0;
        }
        gearId += 0x10;
        fileId = gearId + 5;
        buf = HeapAlloc(ArchiveDecodeAlignedSize(fileId), 0);
        D_800ADBC0 = buf;
        g_GamePartyMemberSkins[slot] = gearId;
    }
    ArchiveReadFileToBuffer(fileId, buf, 0, 0x80);
    if (D_800ADB1C == 0) {
        ArchiveCdDataSync(0);
    }
    D_800ADBC4 = 1;
}

void func_8008A93C(void) {
    g_FieldScriptVMCurActor->unkAnimationId = ~SCRIPT_READ_U8_REL(1);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
}

void func_8008A974(void) {
    func_8008A93C();
    g_FieldScriptVMCurActor->flags &= 0xFFFEFFFF;
}

void func_8008A9AC(void) {
    if (func_8008A558() == 0) {
        D_800ADB90 = 0;
        SpriteSetSpecialAnimFile(
            (SpriteData*)(uintptr_t)g_FieldActors[D_800AFD1C].pSpriteData,
            (void*)(uintptr_t)g_FieldScriptVMCurActor->unk120);
        g_FieldScriptVMCurActor->scriptInstructionPointer += 1;
    } else {
        g_FieldScriptVMCurActor->scriptInstructionPointer -= 1;
    }
    D_800B00C0 = 1;
}

void func_8008AA60(void) {
    if (g_FieldScriptVMCurActor->unk124 != -1) {
        HeapFree((void*)(uintptr_t)g_FieldScriptVMCurActor->unk120);
        g_FieldScriptVMCurActor->unk124 = -1;
    }
    D_800B00C0 = 1;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 1;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008AACC);

void func_8008ACE8(void) {
    s32 arg;
    s32 archiveIndex;
    s32 archiveSize;
    void* pBuffer;

    if (D_800ADB90 == 0 && D_800ADB2C == 0) {
        if (func_8008A558() != 0) {
            D_800B00C0 = 1;
            g_FieldScriptVMCurActor->scriptInstructionPointer -= 1;
            return;
        }

        if (g_FieldScriptVMCurActor->unk124 != -1) {
            HeapFree((void*)(uintptr_t)g_FieldScriptVMCurActor->unk120);
            g_FieldScriptVMCurActor->unk124 = -1;
        }

        arg = FieldScriptVMGetArgument(1);
        archiveIndex = arg;
        ArchiveSetIndex(4, 0);
        archiveIndex += 0x77A;
        archiveSize = ArchiveDecodeAlignedSize(archiveIndex);
        g_FieldScriptVMCurActor->unk124 = archiveIndex;
        pBuffer = HeapAlloc(archiveSize + 8, 0);
        g_FieldScriptVMCurActor->unk120 = (u32)(uintptr_t)pBuffer;
        ArchiveReadFileToBuffer(
            archiveIndex,
            pBuffer,
            0,
            0x80);
        if (D_800ADB1C == 0) {
            ArchiveCdDataSync(0);
        }
        D_800ADB90 = 1;
        g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
    } else {
        g_FieldScriptVMCurActor->scriptInstructionPointer -= 1;
    }
    D_800B00C0 = 1;
}

void func_8008AE5C(void) {
    ActorData* pActor = g_FieldScriptVMCurActor;
    void* pScript = g_FieldScriptVMCurScriptData;
    u8 val = *(u8*)(pScript + pActor->scriptInstructionPointer + 1);
    if (val == 0) {
        pActor->flags |= 0x20000;
    } else {
        pActor->flags &= ~0x20000;
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
}

extern s16 D_800B221C[];

void func_8008AEC8(void) {
    u8 mask;
    s32 arg1, arg2;

    mask = SCRIPT_READ_U8_REL(9);
    arg1 = FieldScriptArgument1(1, mask);
    mask = SCRIPT_READ_U8_REL(9);
    arg2 = FieldScriptArgument2(3, mask);

    {
        s32 idx = arg2 * 3; /* stride 6 = 3 halfwords */
        D_800B221C[idx] = (s16)arg1;
        mask = SCRIPT_READ_U8_REL(9);
        D_800B221C[idx + 1] = (s16)FieldScriptArgument3(5, mask);
        mask = SCRIPT_READ_U8_REL(9);
        D_800B221C[idx + 2] = (s16)FieldScriptArgument4(7, mask);
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 10;
}

extern s16 D_800B223C[];

void func_8008AFD8(void) {
    u8 mask;
    s32 arg1, arg2;

    mask = SCRIPT_READ_U8_REL(9);
    arg1 = FieldScriptArgument1(1, mask);
    mask = SCRIPT_READ_U8_REL(9);
    arg2 = FieldScriptArgument2(3, mask);

    D_800B223C[arg2] = (s16)arg1;
    mask = SCRIPT_READ_U8_REL(9);
    D_800B223C[arg2 + 3] = (s16)FieldScriptArgument3(5, mask);
    mask = SCRIPT_READ_U8_REL(9);
    D_800B223C[arg2 + 6] = (s16)FieldScriptArgument4(7, mask);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 10;
}

extern u8 D_800B225C;
extern u8 D_800B225D;
extern u8 D_800B225E;

void func_8008B0E8(void) {
    D_800B225C = FieldScriptVMGetArgument(1);
    D_800B225D = FieldScriptVMGetArgument(3);
    D_800B225E = FieldScriptVMGetArgument(5);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 7;
}

extern s16 D_800B21B4;

void func_8008B144(void) {
    D_800B21B4 = FieldScriptVMGetArgument(1);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

extern s32 D_800ADB1C;
extern u16 D_800B21E4[];
extern void func_801E8330(s32 slot, s32 unused, s32 anim);

/* Field-script opcode: when D_800ADB1C is set, drive an object's anim via the
 * overlay entry func_801E8330 (no-op'd in the port -- Phase-2B) and record the
 * anim into D_800B21E4[arg1]. Always IP += 5. Stubbed, it never advanced the IP
 * -> another MAP3 script desync feeding func_8009EB78 (same class as FE07). */
void func_8008B180(void) {
    if (D_800ADB1C != 0) {
        s32 arg1 = FieldScriptVMGetArgument(1);
        s32 arg3 = FieldScriptVMGetArgument(3);
        func_801E8330(arg1 & 0xFFFF, 0, arg3);
        arg1 = FieldScriptVMGetArgument(1);
        arg3 = FieldScriptVMGetArgument(3);
        D_800B21E4[arg1] = (u16)arg3;
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
}

void func_8008B210(void) {
    g_FieldScriptVMCurActor->unk11E = FieldScriptVMGetArgument(1);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void func_8008B248(void) {
    FieldFadeSetParameters(1,
        FieldScriptVMGetArgument(9),
        FieldScriptVMGetArgument(3),
        FieldScriptVMGetArgument(5),
        FieldScriptVMGetArgument(7),
        FieldScriptVMGetArgument(1));
    g_FieldScriptVMCurActor->scriptInstructionPointer += 0xB;
}

void func_8008B2F0(void) {
    FieldDistortionInitialize(0);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 0xF;
}

void func_8008B328(void) {
    switch (SCRIPT_READ_U8_REL(1)) {
        case 0:
            FieldDistortionSetTarget(0, 0, 0, 0, 0, 0, FieldScriptVMGetArgument(2));
            g_FieldEffects.distortion.isFinished = 1;
            g_FieldScriptVMCurActor->scriptInstructionPointer += 4;
            break;
        case 1:
            if (g_FieldEffects.distortion.isActive == 0) {
                g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
            } else {
                g_FieldScriptVMCurActor->scriptInstructionPointer -= 1;
            }
            break;
        case 2:
            g_FieldEffects.distortion.isActive = 0;
            g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
            break;
        case 3:
            FieldDistortionFree();
            g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
            break;
    }

    D_800B00C0 = 1;
}

extern s16 D_800B2184, D_800B2186, D_800B2188;

void func_8008B45C(void) {
    u8 mask = SCRIPT_READ_U8_REL(7);
    D_800B2184 = (s16)FieldScriptArgument1(1, mask);
    mask = SCRIPT_READ_U8_REL(7);
    D_800B2188 = (s16)FieldScriptArgument2(3, mask);
    mask = SCRIPT_READ_U8_REL(7);
    D_800B2186 = (s16)FieldScriptArgument3(5, mask);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 8;
}

void func_8008B518(void) {
    u8 mask = SCRIPT_READ_U8_REL(7);
    g_Scene.camRotation.vx = (s16)FieldScriptArgument1(1, mask);
    mask = SCRIPT_READ_U8_REL(7);
    g_Scene.camRotation.vz = (s16)FieldScriptArgument2(3, mask);
    mask = SCRIPT_READ_U8_REL(7);
    g_Scene.camRotation.vy = (s16)FieldScriptArgument3(5, mask);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 8;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008B5D4);

/* --- Party-join chain (the Citan-join machinery) ------------------------
 * add-member opcodes (func_8008BC80 arg-variant / func_8008BDD8 immediate)
 * -> skin-archive kickoff (func_8008A7DC, D_800ADBC4=1 "loading")
 * -> wait/complete opcode (func_8008B894: sync, LZSS into the party
 *    buffer, activate via func_8008B978, D_800ADBC4=0xFF re-arm). */

extern s32 D_800ADB1C;
extern s32 D_800ADB2C;
extern s32 D_800ADBC4;
extern s32 D_800ADBC8;
extern s32 D_800ADBCC;
extern void* D_800ADBC0;
extern s32 D_800ADBFC;
extern void* D_800B06B8;
extern s32 D_800AFD1C;
extern s32 D_800AFFEC;
extern s32 D_800B00C0;
extern s32 D_8006F990[];
extern s32 g_GamePartyMembers[];
extern void* g_PartyDataBuffers[];
extern s32 g_PlayerActorIndex;
extern s32 func_8008A558(void);
extern s32 func_8008A790(s32 value, s32* outIndex);
extern void func_8008A7DC(s32 charId, s32 slot);
extern s32 GameCharacterGetGearID(s32 charId);
extern void LZSSDecompress(void* src, void* dst);
extern s32 ArchiveDataSync(void);
extern void func_80080A74(s32 actorIndex);
extern void FieldActorCopyPlacement(s32 actorIndex, s32 srcActorIndex);
extern void func_80077268(void);
extern u_short FieldScriptGetBytecodeOffset(int scriptIndex, int routineIndex);
extern void FieldScriptVMRun(s32 maxInstructionCount);

/* Wait-for-skin-load opcode (asm 8008B894): while the archive read is in
 * flight, retry (IP-1 + yield); once synced, LZSS-decompress the staged
 * skin into g_PartyDataBuffers[slot], free the staging buffer, activate the
 * joined member (func_8008B978), re-arm D_800ADBC4=0xFF, IP+1 + yield. */
void func_8008B894(void) {
    if (D_800ADBC4 != 0xFF) {
        if (ArchiveDataSync() == 0) {
            ArchiveCdDataSync(0);
            LZSSDecompress(D_800ADBC0, g_PartyDataBuffers[D_800ADBCC]);
            HeapFree(D_800ADBC0);
            func_8008B978(D_800ADBC8);
            D_800ADBC4 = 0xFF;
            D_800B00C0 = 1;
            g_FieldScriptVMCurActor->scriptInstructionPointer += 1;
            return;
        }
    }
    D_800B00C0 = 1;
    g_FieldScriptVMCurActor->scriptInstructionPointer -= 1;
}

/* Activate a joined member (asm 8008B978): set the character's roster bit
 * (gamestate+0x1D30), and for mid-game joins (D_800ADB1C) find the actor
 * declaring this character (script-0 starting with `0x16 <charId>`),
 * re-init it (func_80080A74), place it at the player, run its script 0,
 * settle (func_80077268), and on gear maps re-run the party slot's gear
 * actor too.  The VM context is saved/restored around the nested runs. */
void func_8008B978(s32 charId) {
    ActorData* savedCur = g_FieldScriptVMCurActor;
    void* savedB06B8 = D_800B06B8;
    s32 savedB00C0;
    s32 savedAFD1C;
    s32 savedMax;
    u16 savedIP;
    s32 i;

    *(u16*)((u8*)g_pGameState + 0x1D30) |= 1 << D_800ADBC8;
    if (D_800ADB1C == 0) {
        return;
    }

    savedIP = savedCur->scriptInstructionPointer;
    savedB00C0 = D_800B00C0;
    savedAFD1C = D_800AFD1C;
    savedMax = g_FieldScriptMaxInstructionCount;

    for (i = 0; i < D_800ADBFC; i++) {
        u16 off0 = FieldScriptGetBytecodeOffset(i, 0);
        u8* pc = (u8*)g_FieldScriptVMCurScriptData + off0;
        u16 off0b;

        if (pc[0] != 0x16 || pc[1] != charId) {
            continue;
        }

        D_800B06B8 = (u8*)g_FieldActors + i * 0x5C;
        g_FieldScriptVMCurActor =
            (ActorData*)(uintptr_t)g_FieldActors[i].pActorData;
        func_80080A74(i);
        ((ActorData*)(uintptr_t)g_FieldActors[i].pActorData)
            ->scriptInstructionPointer = off0;
        D_800AFD1C = i;
        off0b = FieldScriptGetBytecodeOffset(i, 0);
        g_FieldScriptVMCurActor->scriptInstructionPointer = off0b;
        D_800AFFEC = 0;
        FieldActorCopyPlacement(i, g_PlayerActorIndex);
        FieldScriptVMRun(0xFFFF);
        func_80077268();
        *(u16*)((u8*)g_pGameState + 0x1D30) |= 1 << D_800ADBC8;

        if (g_GameSceneMapNum & 0xC000) {
            s32 gi = D_8006F990[D_800ADBCC];

            D_800B06B8 = (u8*)g_FieldActors + gi * 0x5C;
            g_FieldScriptVMCurActor =
                (ActorData*)(uintptr_t)g_FieldActors[gi].pActorData;
            func_80080A74(gi);
            ((ActorData*)(uintptr_t)g_FieldActors[i].pActorData)
                ->scriptInstructionPointer = off0b;
            D_800AFD1C = gi;
            g_FieldScriptVMCurActor->scriptInstructionPointer =
                FieldScriptGetBytecodeOffset(gi, 0);
            D_800AFFEC = 0;
            FieldScriptVMRun(0xFFFF);
        }
        break;
    }

    D_800B06B8 = savedB06B8;
    g_FieldScriptVMCurActor = savedCur;
    D_800B00C0 = savedB00C0;
    D_800AFD1C = savedAFD1C;
    g_FieldScriptMaxInstructionCount = savedMax;
    savedCur->scriptInstructionPointer = savedIP;
}

/* Add-party-member opcode, 2-byte-argument variant (asm 8008BC80).
 * Busy (a load pending / transition / stream): retry (IP-1 + yield).
 * arg == 0xFF: no-op, IP+5.  Free slot: clear the member's gamestate byte
 * (+0x22B1+slot), write the roster, kick the skin load, IP+3.  Already a
 * member / party full: set the roster bit (+0x1D30), IP+5. */
void func_8008BC80(void) {
    s32 pending = D_800ADBC4;
    s32 charId;
    s32 slot;

    if (pending == 0xFF && D_800ADB2C == 0 && func_8008A558() == 0) {
        ArchiveCdDataSync(0);
        charId = FieldScriptVMGetArgument(1);
        if (charId == pending) {
            g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
            return;
        }
        if (func_8008A790(charId, &slot) == 0) {
            *((u8*)g_pGameState + 0x22B1 + slot) = 0;
            g_GamePartyMembers[slot] = charId;
            func_8008A7DC(charId, slot);
            g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
        } else {
            *(u16*)((u8*)g_pGameState + 0x1D30) |= 1 << charId;
            g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
        }
        return;
    }
    D_800B00C0 = 1;
    g_FieldScriptVMCurActor->scriptInstructionPointer -= 1;
}

/* Add-party-member opcode, immediate-byte variant (asm 8008BDD8): charId is
 * the raw byte at IP+1; success IP+2, already-member IP+4, busy retry. */
void func_8008BDD8(void) {
    s32 charId;
    s32 slot;

    if (D_800ADBC4 == 0xFF && D_800ADB2C == 0 && func_8008A558() == 0) {
        ArchiveCdDataSync(0);
        charId = SCRIPT_READ_U8_REL(1);
        if (func_8008A790(charId, &slot) == 0) {
            *((u8*)g_pGameState + 0x22B1 + slot) = 0;
            g_GamePartyMembers[slot] = SCRIPT_READ_U8_REL(1);
            func_8008A7DC(SCRIPT_READ_U8_REL(1), slot);
            g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
        } else {
            *(u16*)((u8*)g_pGameState + 0x1D30) |= 1 << SCRIPT_READ_U8_REL(1);
            g_FieldScriptVMCurActor->scriptInstructionPointer += 4;
        }
        return;
    }
    D_800B00C0 = 1;
    g_FieldScriptVMCurActor->scriptInstructionPointer -= 1;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc", func_8008BF38);

extern void* D_800B06B8;
extern void func_80080A74(s32 actorIndex);

void func_8008C180(s32 partyId) {
    s32 slot = D_8005A444[partyId];
    if (slot != 0xFF) {
        u8* pFieldActor = (u8*)g_FieldActors + slot * 92;
        u32 pActorData = *(u32*)(pFieldActor + 0x4C);
        u32 savedIP;
        void* savedB06B8 = D_800B06B8;
        s32 savedAFD1C = D_800AFD1C;
        u16 savedCC = g_FieldScriptVMCurActor->scriptInstructionPointer;

        D_800AFD1C = slot;
        *(u16*)(pFieldActor + 0x58) = (*(u16*)(pFieldActor + 0x58) & 0xF07F) | 0x200;

        func_80076AC0(slot, 0, g_PartyDataBuffers[0], 1, 0, 0, 1);

        {
            u32 flags0 = *(u32*)(g_FieldScriptVMCurActor);
            u32 flags4 = *(u32*)((u8*)g_FieldScriptVMCurActor + 4);
            *(u32*)(g_FieldScriptVMCurActor) = flags0 | 0x1;
            *(u32*)((u8*)g_FieldScriptVMCurActor + 4) = (flags4 | 0x100000) | 0x400;
            *(u32*)(g_FieldScriptVMCurActor) |= 0x20000;
        }

        D_800B00C0 = 0;
        g_FieldScriptVMCurActor = (void*)savedB06B8;
        D_800B06B8 = savedB06B8;
        D_800AFD1C = savedAFD1C;
        g_FieldScriptVMCurActor->scriptInstructionPointer = savedCC;
    }
    g_GamePartyMembers[partyId] = 0xFF;
    g_GamePartyMemberSkins[partyId] = 0xFF;
}

extern s32 D_800ADBC4;
extern s32 g_GamePartyMemberSkins[];
extern void* g_PartyDataBuffers[];
extern s32 D_8005A444[];
extern s32 g_PlayerActorIndex;
extern s32 FieldCharacterIdToPartyId(s32 characterId);
extern s32 func_8008CF3C(s32);
extern void func_8008BF38(s32 partyId);
extern void func_8008C180(s32 partyId);

typedef struct {
    u32 words[4];
} PartyBufferBlock;

void func_8008C334(void) {
    s32 partyId;
    PartyBufferBlock* src;
    PartyBufferBlock* dst;
    PartyBufferBlock* end;
    ActorData* player;

    if (D_800ADBC4 != 0xFF) {
        D_800B00C0 = 1;
        g_FieldScriptVMCurActor->scriptInstructionPointer--;
        return;
    }

    DrawSync(0);
    partyId = FieldCharacterIdToPartyId(func_8008CF3C(SCRIPT_READ_U8_REL(1)));
    if (partyId == -1) {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
        return;
    }

    if (D_800ADB1C == 0) {
        switch (partyId) {
        case 0:
            if (g_GamePartyMembers[1] == 0xFF) {
                g_GamePartyMembers[0] = 0xFF;
                g_GamePartyMemberSkins[0] = 0xFF;
                g_pGameState->gearRide[0] = 0;
                break;
            }

            src = g_PartyDataBuffers[1];
            dst = g_PartyDataBuffers[0];
            end = src + (0x14000 / sizeof(*src));
            do {
                *dst++ = *src++;
            } while (src != end);

            g_GamePartyMembers[0] = g_GamePartyMembers[1];
            g_GamePartyMemberSkins[0] = g_GamePartyMemberSkins[1];
            g_GamePartyMembers[1] = 0xFF;
            g_GamePartyMemberSkins[1] = 0xFF;
            g_pGameState->gearRide[0] = g_pGameState->gearRide[1];

            if (g_GamePartyMembers[2] == 0xFF) {
                break;
            }

            src = g_PartyDataBuffers[2];
            dst = g_PartyDataBuffers[1];
            end = src + (0x14000 / sizeof(*src));
            do {
                *dst++ = *src++;
            } while (src != end);

            g_GamePartyMembers[1] = g_GamePartyMembers[2];
            g_GamePartyMemberSkins[1] = g_GamePartyMemberSkins[2];
            g_GamePartyMembers[2] = 0xFF;
            g_GamePartyMemberSkins[2] = 0xFF;
            g_pGameState->gearRide[1] = g_pGameState->gearRide[2];
            break;

        case 1:
            if (g_GamePartyMembers[2] == 0xFF) {
                g_GamePartyMembers[1] = 0xFF;
                g_GamePartyMemberSkins[1] = 0xFF;
                g_pGameState->gearRide[1] = 0;
                break;
            }

            src = g_PartyDataBuffers[2];
            dst = g_PartyDataBuffers[1];
            end = src + (0x14000 / sizeof(*src));
            do {
                *dst++ = *src++;
            } while (src != end);

            g_GamePartyMembers[1] = g_GamePartyMembers[2];
            g_GamePartyMemberSkins[1] = g_GamePartyMemberSkins[2];
            g_GamePartyMembers[2] = 0xFF;
            g_GamePartyMemberSkins[2] = 0xFF;
            g_pGameState->gearRide[1] = g_pGameState->gearRide[2];
            g_pGameState->gearRide[2] = 0;
            break;

        case 2:
            g_GamePartyMembers[2] = 0xFF;
            g_GamePartyMemberSkins[2] = 0xFF;
            g_pGameState->gearRide[2] = 0;
            break;
        default:
            goto advance;
        }
    } else {
        switch (partyId) {
        case 0:
            g_pGameState->gearRide[0] = g_pGameState->gearRide[1];
            g_pGameState->gearRide[1] = g_pGameState->gearRide[2];
            g_pGameState->gearRide[2] = 0;
            func_8008C180(0);
            func_8008BF38(0);
            func_8008BF38(1);
            break;
        case 1:
            g_pGameState->gearRide[1] = g_pGameState->gearRide[2];
            g_pGameState->gearRide[2] = 0;
            func_8008C180(1);
            func_8008BF38(1);
            break;
        case 2:
            g_pGameState->gearRide[2] = 0;
            func_8008C180(2);
            break;
        }

        if (g_GamePartyMembers[0] != 0xFF) {
            if (D_8005A444[0] != 0xFF) {
                g_PlayerActorIndex = D_8005A444[0];
                player = (ActorData*)(uintptr_t)g_FieldActors[D_8005A444[0]].pActorData;
                player->scriptFlags.flags = (player->scriptFlags.flags | 0x4400) & ~0x80;
            } else {
                g_PlayerActorIndex = 0;
            }
        } else {
            g_PlayerActorIndex = 0;
        }
    }

advance:
    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
}

void func_8008C7D8(void) {
    ActorData* pActor = g_FieldScriptVMCurActor;
    u32 flags12C = *(u32*)((u8*)pActor + 0x12C);
    if (flags12C & 0x1000) {
        HeapFree((void*)(uintptr_t)*(u32*)((u8*)pActor + 0x114));
        *(u32*)((u8*)g_FieldScriptVMCurActor + 0x12C) &= ~0x1000u;
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 1;
}

extern s32 D_8004F36C;
extern s32 D_8004F324;
extern void *D_80062528;
extern void func_8003A89C();

// FE 0E — field music cue: sends a command to the music manager once a song is
// loaded (D_8004F36C); skips when no music is selected (id 0xFF) or the field is
// not yet live (D_800ADB1C == 0); otherwise retries until the song load completes.
void func_8008C84C(void) {
    s32 arg1;

    if (D_8004F36C != 0) {
        arg1 = FieldScriptVMGetArgument(1);
        func_8003A89C(D_80062528, arg1, FieldScriptVMGetArgument(3));
        g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
    } else if (D_8004F324 == 0xFF) {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
    } else if (D_800ADB1C == 0) {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
    } else {
        g_FieldScriptVMCurActor->scriptInstructionPointer -= 1;
    }
    D_800B00C0 = 1;
}

extern void func_8003A948(void* soundHandle, s32 a1, s32 a2);

void func_8008C938(void) {
    if (D_8004F36C != 0) {
        s32 arg1, arg2;
        arg1 = FieldScriptArgument1(1, SCRIPT_READ_U8_REL(5));
        arg2 = FieldScriptArgument2(3, SCRIPT_READ_U8_REL(5));
        func_8003A948(D_80062528, arg1, arg2);
        g_FieldScriptVMCurActor->scriptInstructionPointer += 6;
    } else if (D_8004F324 == 0xFF) {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 6;
    } else if (D_800ADB1C == 0) {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 6;
    } else {
        g_FieldScriptVMCurActor->scriptInstructionPointer -= 1;
    }
    D_800B00C0 = 1;
}

void func_8008CA60(void) {
    if (D_8004F36C != 0) {
        s32 arg1 = FieldScriptVMGetArgument(1);
        s32 arg3 = FieldScriptVMGetArgument(3);
        func_8003A838(D_80062528, arg1, arg3);
        g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
    } else if (D_8004F324 == 0xFF) {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
    } else if (D_800ADB1C == 0) {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
    } else {
        g_FieldScriptVMCurActor->scriptInstructionPointer -= 1;
    }
    D_800B00C0 = 1;
}

extern void func_8003A9BC(void* soundHandle, s32 a1, s32 a2);

void func_8008CB4C(void) {
    if (D_8004F36C != 0) {
        s32 arg1, arg2;
        arg1 = FieldScriptArgument1(1, SCRIPT_READ_U8_REL(5));
        arg2 = FieldScriptArgument2(3, SCRIPT_READ_U8_REL(5));
        func_8003A9BC(D_80062528, arg1, arg2);
        g_FieldScriptVMCurActor->scriptInstructionPointer += 6;
    } else if (D_8004F324 == 0xFF) {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 6;
    } else if (D_800ADB1C == 0) {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 6;
    } else {
        g_FieldScriptVMCurActor->scriptInstructionPointer -= 1;
    }
    D_800B00C0 = 1;
}

extern void func_8003AAC4(void* soundHandle, s32 soundId);

void func_8008CC74(void) {
    ActorData* pActor;
    if (D_8004F36C != 0) {
        s32 arg1 = FieldScriptVMGetArgument(1);
        func_8003AAC4(D_80062528, arg1);
        g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
    } else if (D_8004F324 == 0xFF) {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
    } else if (D_800ADB1C == 0) {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
    } else {
        g_FieldScriptVMCurActor->scriptInstructionPointer -= 1;
    }
    D_800B00C0 = 1;
}

/* Extended VM opcode 0x13 (Noah OPX_13): set the actor's pending sound
 * fields (+0x10A id, +0x10C param, +0x10D state=0) and release any SPU
 * voice channel still bound to this actor (func_800863E8). Id 0 means
 * "none" -> state 0xFF. asm nonmatchings/main/misc/func_8008CD48.s. */
extern void func_800863E8(s32 actorIdx);

void func_8008CD48(void) {
    g_FieldScriptVMCurActor->unk10A = (s16)FieldScriptVMGetArgument(1);
    g_FieldScriptVMCurActor->unk10D = 0;
    g_FieldScriptVMCurActor->unk10C = (u_char)FieldScriptVMGetArgument(3);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
    func_800863E8(D_800AFD1C);
    if ((u16)g_FieldScriptVMCurActor->unk10A == 0) {
        g_FieldScriptVMCurActor->unk10D = 0xFF;
    }
}

extern void func_800863E8(s32 actorIdx);
extern s32 D_800AFD1C;

void func_8008CDD4(void) {
    ActorData* pActor = g_FieldScriptVMCurActor;
    s32 arg1 = FieldScriptVMGetArgument(1);
    pActor->unk10A = (short)arg1;
    pActor->unk10D = 0x80;
    {
        s32 arg3 = FieldScriptVMGetArgument(3);
        g_FieldScriptVMCurActor->unk10C = (u_char)arg3;
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
    func_800863E8(D_800AFD1C);
    if (g_FieldScriptVMCurActor->unk10A == 0) {
        g_FieldScriptVMCurActor->unk10D = 0xFF;
    }
}

void func_8008CE64(void) {
    s32 v = func_8008CF3C(FieldScriptVMGetArgument(1));
    if (v != 0xFF) {
        g_pGameState->FrMask &= ~(1 << v);
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void func_8008CED0(void) {
    s32 v = func_8008CF3C(FieldScriptVMGetArgument(1));
    if (v != 0xFF) {
        g_pGameState->FrMask |= 1 << v;
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

s32 func_8008CF3C(s32 a0) {
    switch (a0) {
    case 0xFF:
        return g_GamePartyMembers[2];
    case 0xFE:
        return g_GamePartyMembers[1];
    case 0xFD:
        return g_GamePartyMembers[0];
    case 0xFC:
        return 0xFF;
    }
    return a0;
}

void func_8008CF9C(void) {
    g_FieldScriptVMCurActor->faceId = func_8008CF3C(FieldScriptVMGetArgument(1));
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

extern s16 D_800B21A0, D_800B21A2, D_800B21A4, D_800B21A6, D_800B21A8, D_800B21AA;

void func_8008CFEC(void) {
    D_800B21A0 = (s16)FieldScriptVMGetInstructionArgument(1);
    D_800B21A4 = (s16)FieldScriptVMGetInstructionArgument(3);
    D_800B21A2 = (s16)FieldScriptVMGetInstructionArgument(5);
    D_800B21A6 = (s16)FieldScriptVMGetInstructionArgument(7);
    D_800B21AA = (s16)FieldScriptVMGetInstructionArgument(9);
    D_800B21A8 = (s16)FieldScriptVMGetInstructionArgument(11);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 13;
}

void func_8008D078(void) {
    if (FieldScriptVMGetArgument(1) == 0) {
        *(s32*)((u8*)g_FieldScriptVMCurActor + 0x4) &= ~0x800;
    } else {
        *(s32*)((u8*)g_FieldScriptVMCurActor + 0x4) |= 0x800;
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

/* Extended field-script opcode EX-0x03 (SET_CURRENT_ACTOR_SCALE), reached via
 * the 0xFE prefix -> FieldScriptVM2Run. Sets the current actor's sprite scale
 * (SpriteData+0x2C) and 3D scale (scaleX/Y/Z), then advances the script IP by 3.
 * Decompiled from func_8008D0F4.s. This was previously INCLUDE_ASM, which on the
 * native port became a no-op logging stub that never advanced scriptInstructionPointer;
 * the VM then re-dispatched the operand bytes as a bogus top-level dialog opcode,
 * opening a garbage text box (the Lahan-intro "garbled dialog"). The IP += 3 here
 * is the load-bearing fix. */
void func_8008D0F4(void) {
    s32 scale = FieldScriptVMGetArgument(1);
    *(s16*)((u8*)(uintptr_t)g_FieldActors[D_800AFD1C].pSpriteData + 0x2C) = (scale * 3) >> 2;
    g_FieldScriptVMCurActor->scaleX = scale;
    g_FieldScriptVMCurActor->scaleY = scale;
    g_FieldScriptVMCurActor->scaleZ = scale;
    func_80072254(D_800AFD1C);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

extern s32 D_800AFD1C;

void func_8008D180(void) {
    s32 arg1 = FieldScriptVMGetArgument(1);
    s32 arg3 = FieldScriptVMGetArgument(3);
    s32 arg5 = FieldScriptVMGetArgument(5);
    u32 actorIdx = (u32)D_800AFD1C;
    u8* pFieldActor = (u8*)g_FieldActors + actorIdx * 92;
    u32 pActorData = *(u32*)(pFieldActor + 0x04);
    u8* pActor = (u8*)g_FieldScriptVMCurActor;

    *(s16*)(pActorData + 0x2C) = 0xC00;
    *(s16*)(pActor + 0xF4) = (s16)arg1;
    *(s16*)(pActor + 0xF6) = (s16)arg3;
    *(s16*)(pActor + 0xF8) = (s16)arg5;
    func_80072254(actorIdx);
    *(u16*)(pActor + 0xCC) += 7;
}

extern s16 D_800B218C;

/* FE3A — write party frame mask. */
void func_8008D230(void) {
    D_800B218C = FieldScriptVMGetArgument(1);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void func_8008D26C(void) {
    *(s16*)((u8*)(uintptr_t)g_FieldActors[D_800AFD1C].pSpriteData + 0x82) = FieldScriptVMGetArgument(1) << 1;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void func_8008D2D8(void) {
}

void func_8008D2E0(s32 a0, s32 a1) {
    *(u8*)(a1 + (s32)g_FieldScriptVMCurScriptData + 1) = a0 >> 8;
    *(u8*)((u8*)g_FieldScriptVMCurScriptData + a1) = a0;
}

s32 func_8008D30C(s32 a0, s32 a1) {
    s32 dx = g_FieldActors[a0].transformMatrix.t[0] - g_FieldActors[a1].transformMatrix.t[0];
    s32 dz = g_FieldActors[a0].transformMatrix.t[2] - g_FieldActors[a1].transformMatrix.t[2];
    return -(FieldGetVec2Magnitude(dx, dz) >= 0x10);
}

static inline void CopySpriteFieldS32(FieldActor* actors, int dstId, int srcId, int offset) {
    void* dst = (void*)(uintptr_t)actors[dstId].pSpriteData;
    void* src = (void*)(uintptr_t)actors[srcId].pSpriteData;
    *(s32*)((u8*)dst + offset) = *(s32*)((u8*)src + offset);
}

static inline void CopySpriteFieldU16(FieldActor* actors, int dstId, int srcId, int offset) {
    void* dst = (void*)(uintptr_t)actors[dstId].pSpriteData;
    void* src = (void*)(uintptr_t)actors[srcId].pSpriteData;
    *(u16*)((u8*)dst + offset) = *(u16*)((u8*)src + offset);
}

static inline void CopyActorFieldS32(FieldActor* base, int dstId, int srcId, int offset) {
    *(s32*)((u8*)&base[dstId] + offset) = *(s32*)((u8*)&base[srcId] + offset);
}

void FieldActorCopyPlacement(int dstActorId, int srcActorId) {
    FieldActor* actors = g_FieldActors;
    ActorData* dstActor;
    ActorData* srcActor;
    int i;

    srcActor = (ActorData*)(uintptr_t)actors[srcActorId].pActorData;
    dstActor = (ActorData*)(uintptr_t)actors[dstActorId].pActorData;

    for (i = 0; i < 4; i++) {
        dstActor->walkmeshTriIds[i] = srcActor->walkmeshTriIds[i];
    }
    {
      s16 walkmeshId = srcActor->walkmeshId;
      dstActor->walkmeshId = walkmeshId;
    }
    dstActor->curTriNormal.vx = srcActor->curTriNormal.vx;
    dstActor->curTriNormal.vy = srcActor->curTriNormal.vy;
    dstActor->curTriNormal.vz = srcActor->curTriNormal.vz;
    dstActor->position.vx = srcActor->position.vx;
    dstActor->position.vy = srcActor->position.vy;
    dstActor->position.vz = srcActor->position.vz;
    dstActor->unkEC = srcActor->unkEC;
    dstActor->curYPos = srcActor->curYPos;
    dstActor->curWalkmeshTriMaterial = srcActor->curWalkmeshTriMaterial;

    CopySpriteFieldU16(g_FieldActors, dstActorId, srcActorId, offsetof(SpriteData, field_0x84));
    CopySpriteFieldS32(g_FieldActors, dstActorId, srcActorId, offsetof(SpriteData, position.x));
    CopySpriteFieldS32(g_FieldActors, dstActorId, srcActorId, offsetof(SpriteData, position.y));
    CopySpriteFieldS32(g_FieldActors, dstActorId, srcActorId, offsetof(SpriteData, position.z));

    CopyActorFieldS32(g_FieldActors, dstActorId, srcActorId, offsetof(FieldActor, transformMatrix.t[0]));
    CopyActorFieldS32(g_FieldActors, dstActorId, srcActorId, offsetof(FieldActor, transformMatrix.t[1]));
    CopyActorFieldS32(g_FieldActors, dstActorId, srcActorId, offsetof(FieldActor, transformMatrix.t[2]));
}

extern s32 D_8005A444[];
extern u8 D_800B219F;

void func_8008D570(void) {
    s32 i;
    for (i = 0; i < 3; i++) {
        if (D_8005A444[i] == D_800AFD1C) {
            D_800B219F &= ~(1 << i);
        }
    }
}

extern u8 D_800B21CD;

/* FE26 — write distortion/effect flag byte. */
void func_8008D5C8(void) {
    D_800B21CD = SCRIPT_READ_U8_REL(1);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
}

/* FE07/opcode 0x107 -- set (arg 1) or clear (arg 0) the current actor's
 * 0x400 flag; any other arg leaves it untouched.  IP += 2.  As a no-op stub
 * this handler never advanced the IP, so the VM's next dispatch executed the
 * FE-prefix's extension byte as a BASE opcode -- desyncing the actor's whole
 * script stream (MAP3's boot SEGV: garbage actor index 128 fed to
 * func_8009EB78 several bogus opcodes later). */
void func_8008D604(void) {
    u8 arg = SCRIPT_READ_U8_REL(1);

    if (arg == 0) {
        g_FieldScriptVMCurActor->flags &= ~0x400;
    } else if (arg == 1) {
        g_FieldScriptVMCurActor->flags |= 0x400;
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
}

void func_8008D684(void) {
    int mask;
    unsigned int slotIndex;

    slotIndex = (unsigned int) (SCRIPT_IMM_ARG(1)) >> 4;
    mask = 1 << (FieldScriptVMGetInstructionArgument(1) & 0xF);
    FieldScriptMemoryWriteU16(slotIndex, FieldScriptVMGetVariableValue(slotIndex) | mask);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void func_8008D700(void) {
    int mask;
    unsigned int slotIndex;

    slotIndex = (unsigned int) (SCRIPT_IMM_ARG(1)) >> 4;
    mask = 1 << (FieldScriptVMGetInstructionArgument(1) & 0xF);
    FieldScriptMemoryWriteU16(slotIndex, FieldScriptVMGetVariableValue(slotIndex) & ~mask);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void func_8008D780(void) {
    short destination;
    int mask;
    unsigned int slotIndex;

    slotIndex = (unsigned int) (SCRIPT_IMM_ARG(1)) >> 4;
    mask = 1 << (FieldScriptVMGetInstructionArgument(1) & 0xF);
    if (FieldScriptVMGetVariableValue(slotIndex) & mask) {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
    } else {
        destination = FieldScriptVMGetInstructionArgument(3);
        g_FieldScriptVMCurActor->scriptInstructionPointer = destination;
    }
}

void func_8008D808(s32 arg0, s32 arg1, s32 arg2) {
    u8* pScript = g_FieldScriptVMCurScriptData;
    u16 ip = g_FieldScriptVMCurActor->scriptInstructionPointer;
    u8* pIP = pScript + ip;
    pIP[0x00] = 0x57;
    pIP[0x01] = 0x81;
    func_8008D2E0(arg0, ip + 2);
    func_8008D2E0(arg1, ip + 4);
    func_8008D2E0(arg2, ip + 6);
    func_8008D2E0(0xC, ip + 8);
    pScript = g_FieldScriptVMCurScriptData;
    ip = g_FieldScriptVMCurActor->scriptInstructionPointer;
    pIP = pScript + ip;
    pIP[0x0A] = 0xFF;
    pIP[0x0B] = 0x57;
    pIP[0x0C] = 0x8F;
    pIP[0x0D] = 0x26;
    pIP[0x0E] = 0x01;
    pIP[0x0F] = 0x80;
    pIP[0x10] = 0x57;
    pIP[0x11] = 0x0F;
}

void func_8008DA04(s32 arg0, s32 arg1) {
    u8* pScript = g_FieldScriptVMCurScriptData;
    u8* pIP = pScript + g_FieldScriptVMCurActor->scriptInstructionPointer;
    pIP[0x0C] = 0x4B;
    func_8008D2E0(arg0, g_FieldScriptVMCurActor->scriptInstructionPointer + 0x0D);
    func_8008D2E0(arg1, g_FieldScriptVMCurActor->scriptInstructionPointer + 0x0F);
    pIP = pScript + g_FieldScriptVMCurActor->scriptInstructionPointer;
    pIP[0x11] = 0xFF;
    pIP[0x12] = 0xFF;
    pIP[0x13] = 0x80;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 0xC;
}

void func_8008DAFC(void) {
    g_FieldScriptVMCurActor->parentActorId = 0xFF;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 1;
}

extern s16 D_800B233E;

void func_8008DB2C(void) {
    D_800B233E = FieldScriptVMGetActorIndex(1);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
}


int FieldPartyMemberIncreaseGearHp(int partyMemberIndex, unsigned int amount) {
    int gearId;
    unsigned int maxHp;
    unsigned int newHp;

    gearId = GameCharacterGetGearID(g_GamePartyMembers[partyMemberIndex]);
    if (gearId != CHARACTER_ID_NONE) {
        maxHp = g_pGameState->gears[gearId].maxHp;
        newHp = g_pGameState->gears[gearId].hp + amount;
        g_pGameState->gears[gearId].hp = newHp;
        if (maxHp < newHp) {
            g_pGameState->gears[gearId].hp = maxHp;
        }
    }
}

int FieldPartyMemberDecreaseGearHp(int partyMemberIndex, unsigned int amount) {
    int gearId;
    int newHp;

    gearId = GameCharacterGetGearID(g_GamePartyMembers[partyMemberIndex]);
    if (gearId != CHARACTER_ID_NONE) {
        newHp = g_pGameState->gears[gearId].hp - amount;
        if (newHp <= 0) {
            newHp = 1;
        }
        g_pGameState->gears[gearId].hp = newHp;
    }
}

extern s16 g_FieldNumPartyMembersMasks[4];

void FieldScriptVMHandlerIncreasePartyGearHp(void) {
    int mask;
    int amount;
    int i;

    amount = FieldScriptArgument1(1, SCRIPT_READ_U8_REL(3));
    mask = g_FieldNumPartyMembersMasks[SCRIPT_READ_U8_REL(3) & 0x3];

    for (i = 0; i < MAX_PARTY_MEMBERS; i++) {
        if ((g_GamePartyMembers[i] != CHARACTER_ID_NONE) && (mask & 1)) {
            FieldPartyMemberIncreaseGearHp(i, amount);
        }
        mask >>= 1;
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 4;
}

void FieldScriptVMHandlerDecreasePartyGearHp(void) {
    int mask;
    int amount;
    int i;

    amount = FieldScriptArgument1(1, SCRIPT_READ_U8_REL(3));
    mask = g_FieldNumPartyMembersMasks[SCRIPT_READ_U8_REL(3) & 0x3];

    for (i = 0; i < MAX_PARTY_MEMBERS; i++) {
        if ((g_GamePartyMembers[i] != CHARACTER_ID_NONE) && (mask & 1)) {
            FieldPartyMemberDecreaseGearHp(i, amount);
        }
        mask >>= 1;
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 4;
}

void FieldScriptSetParentActor(void) {
    int actorIndex = FieldScriptVMGetActorIndex(1);
    if (actorIndex != ACTOR_ID_INVALID) {
        g_FieldScriptVMCurActor->parentActorId = actorIndex;
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
}

void FieldScriptWriteActorFlags1(void) {
    FieldActor* actors = g_FieldActors;
    int actorIndex = FieldScriptVMGetActorIndex(1);

    if (actorIndex != ACTOR_ID_INVALID) {
        u8* pActorData = (u8*)(uintptr_t)actors[actorIndex].pActorData;
        FieldScriptMemoryWriteU16(SCRIPT_IMM_ARG(1), *(u32*)pActorData);
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void FieldScriptWriteActorFlags2(void) {
    FieldActor* actors = g_FieldActors;
    int actorIndex = FieldScriptVMGetActorIndex(1);

    if (actorIndex != ACTOR_ID_INVALID) {
        u8* pActorData = (u8*)(uintptr_t)actors[actorIndex].pActorData;
        FieldScriptMemoryWriteU16(SCRIPT_IMM_ARG(1), *(u16*)(pActorData + 2));
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void FieldScriptWriteActorFlags3(void) {
    FieldActor* actors = g_FieldActors;
    int actorIndex = FieldScriptVMGetActorIndex(1);

    if (actorIndex != ACTOR_ID_INVALID) {
        u8* pActorData = (u8*)(uintptr_t)actors[actorIndex].pActorData;
        FieldScriptMemoryWriteU16(SCRIPT_IMM_ARG(1), *(u32*)(pActorData + 4));
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void FieldScriptWriteActorFlags4(void) {
    FieldActor* actors = g_FieldActors;
    int actorIndex = FieldScriptVMGetActorIndex(1);

    if (actorIndex != ACTOR_ID_INVALID) {
        u8* pActorData = (u8*)(uintptr_t)actors[actorIndex].pActorData;
        FieldScriptMemoryWriteU16(SCRIPT_IMM_ARG(1), *(u16*)(pActorData + 6));
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

// TODO: These two handlers could use more semantic names since they're likely used
// in certain situations.
void FieldScriptVMConditionalJump6(unsigned short flag) {
    unsigned short argument = SCRIPT_IMM_ARG(1);
    if (argument & flag) {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 6;
    } else {
        g_FieldScriptVMCurActor->scriptInstructionPointer = FieldScriptVMGetInstructionArgument(4);
    }
}

void FieldScriptVMConditionalJump5(unsigned short flag) {
    unsigned short argument = SCRIPT_IMM_ARG(1);
    if (argument & flag) {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
    } else {
        g_FieldScriptVMCurActor->scriptInstructionPointer = SCRIPT_IMM_ARG(2);
    }
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc", FieldScriptWriteActorDistance);
/*
Matches as long as g_FieldActors is NOT volatile.

void FieldScriptWriteActorDistance(void) {
    ActorData* pActorA;
    ActorData* pActorB;
    int actorIndexA;
    int actorIndexB;
    int distance;

    distance = 0;
    actorIndexA = FieldScriptVMGetActorIndex(3);
    actorIndexB = FieldScriptVMGetActorIndex(4);
    if ((actorIndexA != ACTOR_ID_INVALID) && (actorIndexB != ACTOR_ID_INVALID)) {
        pActorA = (ActorData*)(uintptr_t)g_FieldActors[actorIndexA].pActorData;
        pActorB = (ActorData*)(uintptr_t)g_FieldActors[actorIndexB].pActorData;
        distance = FieldGetVec2Magnitude(
            CONV_TO_GTE(pActorA->position.vx) - CONV_TO_GTE(pActorB->position.vx), 
            CONV_TO_GTE(pActorA->position.vz) - CONV_TO_GTE(pActorB->position.vz)
        );
    }
    FieldScriptMemoryWriteU16(
        SCRIPT_IMM_ARG(1), 
        distance
    );
    g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
}
*/

void func_8008E298(void) {
    FieldScriptVMConditionalJump6(((u16*)g_FieldActors[FieldScriptVMGetActorIndex(3)].pActorData)[0]);
}

void func_8008E2EC(void) {
    FieldScriptVMConditionalJump6(((u16*)g_FieldActors[FieldScriptVMGetActorIndex(3)].pActorData)[1]);
}

void func_8008E340(void) {
    FieldScriptVMConditionalJump6(((u16*)g_FieldActors[FieldScriptVMGetActorIndex(3)].pActorData)[2]);
}

void func_8008E394(void) {
    FieldScriptVMConditionalJump6(((u16*)g_FieldActors[FieldScriptVMGetActorIndex(3)].pActorData)[3]);
}

void func_8008E3E8(void) {
    FieldScriptVMConditionalJump5(((u16*)g_FieldScriptVMCurActor)[0]);
}

void func_8008E414(void) {
    FieldScriptVMConditionalJump5(((u16*)g_FieldScriptVMCurActor)[1]);
}

void func_8008E440(void) {
    FieldScriptVMConditionalJump5(((u16*)g_FieldScriptVMCurActor)[2]);
}

void func_8008E46C(void) {
    FieldScriptVMConditionalJump5(((u16*)g_FieldScriptVMCurActor)[3]);
}

void func_8008E498(int a0) {
    FieldScriptMemoryWriteU16(SCRIPT_IMM_ARG(1), a0 & 0xFFFF);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void func_8008E4EC(void) {
    func_8008E498(((u16*)g_FieldScriptVMCurActor)[0]);
}

void func_8008E518(void) {
    func_8008E498(((u16*)g_FieldScriptVMCurActor)[1]);
}

void func_8008E544(void) {
    func_8008E498(((u16*)g_FieldScriptVMCurActor)[2]);
}

void func_8008E570(void) {
    func_8008E498(((u16*)g_FieldScriptVMCurActor)[3]);
}

void func_8008E59C(void) {
    u32 mask;

    mask = FieldScriptVMGetInstructionArgument(2) & 0xFFFF;
    switch (SCRIPT_READ_U8_REL(1)) {
        case 0:
            g_FieldScriptVMCurActor->scriptFlags.flags |= mask;
            break;

        case 1:
            g_FieldScriptVMCurActor->scriptFlags.flags |= mask << 16;
            break;

        case 2:
            g_FieldScriptVMCurActor->flags |= mask;
            break;

        case 3:
            g_FieldScriptVMCurActor->flags |= mask << 16;
            break;

        case 4:
            g_FieldScriptVMCurActor->scriptFlags.flags &= ~mask;
            break;

        case 5:
            g_FieldScriptVMCurActor->scriptFlags.flags &= ~(mask << 16);
            break;

        case 6:
            g_FieldScriptVMCurActor->flags &= ~mask;
            break;

        case 7:
            g_FieldScriptVMCurActor->flags &= ~(mask << 16);
            break;
    }

    g_FieldScriptVMCurActor->scriptInstructionPointer += 4;
}

/* func_8008E718 -- step-counter / formation-cooldown REFRESH. Decompiled from
 * asm/field/nonmatchings/main/misc/func_8008E718.s (was INCLUDE_ASM/stubbed).
 * Called by func_80079288 (roll, when the step counter hits 0) and by the
 * encounter-setup opcode func_8008E85C below. Reloads the step counter from the
 * encounter base and assigns each active formation a unique random cooldown.
 * See docs/ai_context/ACTIVE_HANDOFF.md. */
extern s32 D_800B2294, D_800B2298, D_800B229C;
extern s16 D_800B22A0[];

void func_8008E718(void) {
    u16 *timers = (u16 *)D_800B22A0;
    s32  i, j, roll;

    D_800B2294 = D_800B2298;

    if (D_800B229C == 0) {
        D_800B2298 = 0;
        return;
    }

    for (i = 31; i >= 0; i--) {
        timers[i] = 0xFFFF;
    }

    if (D_800B229C > 0) {
        for (i = 0; i < D_800B229C; i++) {
            do {
                roll = ((rand() * (D_800B2298 + 1)) >> 15) & 0xFFFF;
                for (j = 0; j < 32; j++) {
                    if (timers[j] == (u16)roll) break;
                }
            } while (j < 32);
            timers[i] = (u16)roll;
        }
    }

    if (D_800B229C > 0) {
        for (i = 0; i < D_800B229C; i++) {
            timers[i] = (u16)(timers[i] + 1);
        }
    }
}

void func_8008E85C(void) {
    D_800B2298 = FieldScriptVMGetArgument(1);
    D_800B229C = FieldScriptVMGetArgument(3);
    if (D_800B229C >= 0x21) {
        D_800B229C = 0x20;
    }
    func_8008E718();
    g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
}

void func_8008E8C8(void) {
    ActorData* pActor = g_FieldScriptVMCurActor;
    u8 subOp = SCRIPT_READ_U8_REL(1);

    switch (subOp) {
        case 0: {
            u32 flags0 = *(u32*)(pActor);
            if (flags0 & 0x8000) {
                *(u32*)(pActor) = flags0 & 0xFFFF7FFF;
            }
            pActor = g_FieldScriptVMCurActor;
            if (*(u32*)((u8*)pActor + 0x04) & 0x80000) {
                s32 idx = D_800AFD1C;
                u8* pFieldActor = (u8*)g_FieldActors + idx * 92;
                u32 pSub = *(u32*)(pFieldActor + 0x04);
                *(u32*)(pSub + 0x18) = 0;
                *(u32*)(pSub + 0x14) = 0;
                *(u32*)(pSub + 0x0C) = 0;
                *(u32*)((u8*)pActor + 0x04) &= 0xFFF7FFFF;
            }
            break;
        }
        case 1: {
            u32 flags0 = *(u32*)(pActor);
            u16 val = *(u16*)((u8*)pActor + 0x106);
            *(u32*)(pActor) = flags0 | 0x8000;
            *(u16*)((u8*)pActor + 0x11C) = val;
            break;
        }
        case 2: {
            u32 flags4 = *(u32*)((u8*)pActor + 0x04);
            *(u32*)((u8*)pActor + 0x04) = flags4 | 0x80000;
            break;
        }
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 2;
}

extern s32 D_800ADB7C;

void func_8008E9F8(void) {
    if (D_800ADB7C == 0) {
        g_FieldScriptVMCurActor->scriptInstructionPointer -= 1;
    } else {
        D_800ADB7C = 0;
        g_FieldScriptVMCurActor->scriptInstructionPointer += 1;
    }
    D_800B00C0 = 1;
}

extern s32 D_800ADBDC;
extern s16 D_800C3A20, D_800C3A22, D_800C3A24, D_800C3A26, D_800C3A28, D_800C3A2A, D_800C3A2C, D_800C3A2E;
extern s16 D_800C3A30, D_800C3A32, D_800C3A34, D_800C3A36, D_800C3A38, D_800C3A3A;
extern s32 D_800ADB80, D_800ADB74, D_800ADB70;

void func_8008EA58(void) {
    if (D_800ADBDC == 0) {
        D_800B00C0 = 1;
        g_FieldScriptVMCurActor->scriptInstructionPointer -= 1;
        return;
    }
    D_800C3A20 = (s16)FieldScriptArgument1(1, SCRIPT_READ_U8_REL(0xB));
    D_800C3A2A = (s16)FieldScriptArgument2(3, SCRIPT_READ_U8_REL(0xB));
    D_800C3A2C = (s16)FieldScriptArgument3(5, SCRIPT_READ_U8_REL(0xB));
    D_800C3A2E = (s16)FieldScriptArgument4(7, SCRIPT_READ_U8_REL(0xB));
    D_800C3A38 = (s16)FieldScriptArgument5(9, SCRIPT_READ_U8_REL(0xB));
    D_800C3A32 = 0x140;
    D_800ADB80 = 0x40;
    D_800C3A36 = 1;
    D_800C3A34 = 0x100;
    D_800C3A26 = 0;
    D_800C3A24 = 0;
    D_800C3A22 = 0;
    D_800C3A28 = 0x100;
    D_800C3A3A = 0;
    D_800C3A30 = (s16)(*(u16*)&D_800C3A30 & 0xF);
    D_800ADB74 = 0;
    D_800ADB70 = 1;
    D_800B00C0 = 1;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 0xC;
}

void func_8008EC30(void) {
    s32 arg7, type;

    if (D_800ADBDC == 0) {
        D_800B00C0 = 1;
        g_FieldScriptVMCurActor->scriptInstructionPointer -= 1;
        return;
    }
    D_800C3A20 = (s16)FieldScriptVMGetArgument(1);
    D_800C3A2A = (s16)FieldScriptVMGetArgument(3);
    D_800C3A2E = (s16)FieldScriptVMGetArgument(5);
    arg7 = FieldScriptVMGetArgument(7);
    type = arg7 & 0xF;
    D_800C3A30 = (s16)arg7;
    D_800ADB80 = arg7 & 0xC0;
    D_800C3A32 = 0x140;
    D_800C3A34 = 0x100;
    D_800C3A30 = (s16)type;
    D_800C3A2C = 1;

    switch (type) {
        case 0:
            D_800C3A22 = 0x140;
            D_800C3A24 = 0;
            D_800C3A26 = 0x140;
            D_800C3A28 = 0x100;
            D_800ADB74 = 1;
            D_800C3A36 = 0;
            break;
        case 1:
            D_800C3A26 = 0;
            D_800C3A24 = 0;
            D_800C3A22 = 0;
            D_800C3A28 = 0x100;
            D_800ADB74 = 0;
            D_800C3A36 = 0;
            break;
        case 2:
            D_800C3A26 = 0;
            D_800C3A24 = 0;
            D_800C3A22 = 0;
            D_800C3A28 = 0x100;
            D_800ADB74 = 0;
            D_800C3A36 = 1;
            break;
    }
    D_800C3A38 = 0xFF;
    D_800C3A3A = 0;
    D_800ADB70 = 1;
    D_800B00C0 = 1;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 9;
}

extern s16 D_800C3A20, D_800C3A22, D_800C3A24, D_800C3A26, D_800C3A28, D_800C3A2A, D_800C3A2C, D_800C3A2E;
extern s16 D_800C3A30, D_800C3A32, D_800C3A34, D_800C3A36, D_800C3A38, D_800C3A3A;
extern s32 D_800ADB80, D_800ADB74, D_800ADB70;

void func_8008EE14(void) {
    u16 arg9;
    D_800C3A20 = (s16)FieldScriptVMGetArgument(1);
    D_800C3A2A = (s16)FieldScriptVMGetArgument(3);
    D_800C3A2C = (s16)FieldScriptVMGetArgument(5);
    D_800C3A2E = (s16)FieldScriptVMGetArgument(7);
    arg9 = (u16)FieldScriptVMGetArgument(9);
    D_800C3A30 = (s16)arg9;
    if (arg9 == 0xFE) {
        D_800C3A3A = 1;
    } else {
        D_800C3A3A = 0;
    }
    D_800C3A22 = (s16)FieldScriptVMGetArgument(11);
    D_800C3A26 = D_800C3A22;
    D_800C3A24 = (s16)FieldScriptVMGetArgument(13);
    D_800C3A28 = D_800C3A24;
    D_800C3A32 = (s16)FieldScriptVMGetArgument(15);
    D_800C3A34 = (s16)FieldScriptVMGetArgument(17);
    D_800ADB80 = arg9 & 0x40;
    D_800C3A38 = 0xFF;
    D_800ADB74 = 2;
    D_800C3A36 = 0;
    D_800C3A30 = (s16)(arg9 & 0xF);
    D_800ADB70 = 1;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 0x13;
}

extern s32 D_800ADB3C;
extern s32 D_800ADB38;

void func_8008EF5C(void) {
    D_800ADB3C = FieldScriptVMGetArgument(1);
    D_800ADB38 = 1;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void func_8008EFA0(void) {
    D_800ADB3C = FieldScriptVMGetArgument(1);
    D_800ADB38 = 2;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void func_8008EFE4(void) {
    s32 arg1 = FieldScriptVMGetArgument(1);
    s32 arg3 = FieldScriptVMGetArgument(3);
    s32 arg5 = FieldScriptVMGetArgument(5);
    s32 arg7 = FieldScriptVMGetArgument(7);
    FieldSetClipDimensions((short)arg1, (short)arg3, (short)arg5, (short)arg7);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 9;
}

void func_8008F070(void) {
    D_800ADB3C = FieldScriptVMGetArgument(1);
    D_800ADB38 = 3;
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void func_8008F0B4(void) {
    s32 actorIdx = FieldScriptVMGetActorIndex(2);
    if (actorIdx != 0xFF) {
        u8* pFieldActor = (u8*)g_FieldActors + actorIdx * 92;
        u8* pScript = g_FieldScriptVMCurScriptData;
        u8 flags = pScript[g_FieldScriptVMCurActor->scriptInstructionPointer + 1];
        u32 pSub = *(u32*)(pFieldActor + 0x4C);

        if (flags & 1) {
            *(u8*)(pSub + 0xFC) = (u8)FieldScriptVMGetArgument(3);
            *(u8*)(pSub + 0xFD) = (u8)FieldScriptVMGetArgument(5);
            *(u8*)(pSub + 0xFE) = (u8)FieldScriptVMGetArgument(7);
        }
        if (flags & 2) {
            *(u8*)(pSub + 0xFF) = (u8)FieldScriptVMGetArgument(3);
            *(u8*)(pSub + 0x100) = (u8)FieldScriptVMGetArgument(5);
            *(u8*)(pSub + 0x101) = (u8)FieldScriptVMGetArgument(7);
        }
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 9;
}

/* Field-script opcode: conditionally (per the opcode's flag byte at IP+1) copy
 * two operand triples into the actor's color fields (unkFC..FE / unkFF..101),
 * then IP += 8. As a no-op stub it never advanced the IP -> MAP160's VM
 * desynced and fed a garbage actor index (128) to
 * FieldScriptVMHandlerEnableActorVM (misc6.c:80) -- the same FE07-class desync
 * fixed for MAP3 (func_80088198/func_8008B180). */
void func_8008F1C8(void) {
    if (SCRIPT_READ_U8_REL(1) & 0x1) {
        g_FieldScriptVMCurActor->unkFC = (u8)FieldScriptVMGetArgument(2);
        g_FieldScriptVMCurActor->unkFD = (u8)FieldScriptVMGetArgument(4);
        g_FieldScriptVMCurActor->unkFE = (u8)FieldScriptVMGetArgument(6);
    }
    if (SCRIPT_READ_U8_REL(1) & 0x2) {
        g_FieldScriptVMCurActor->unkFF = (u8)FieldScriptVMGetArgument(2);
        g_FieldScriptVMCurActor->unk100 = (u8)FieldScriptVMGetArgument(4);
        g_FieldScriptVMCurActor->unk101 = (u8)FieldScriptVMGetArgument(6);
    }
    g_FieldScriptVMCurActor->scriptInstructionPointer += 8;
}

void func_8008F2D8(void) {
    func_80023290((void*)(uintptr_t)g_FieldActors[D_800AFD1C].pSpriteData, FieldScriptVMGetArgument(1));
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

extern s32 D_800C3A5C;
extern s32 D_800C3A60;

void func_8008F348(void) {
    D_800C3A5C = FieldScriptVMGetArgument(1);
    D_800C3A60 = FieldScriptVMGetArgument(3);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
}

extern s16 D_800B233C;

void func_8008F394(void) {
    D_800B233C = FieldScriptVMGetArgument(1);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void func_8008F3D0(void) {
    func_8003A450(FieldScriptVMGetArgument(3) << 1, FieldScriptVMGetArgument(1), FieldScriptVMGetArgument(5));
    g_FieldScriptVMCurActor->scriptInstructionPointer += 7;
}

void func_8008F444(void) {
    func_8003A344(FieldScriptVMGetArgument(3) << 1, FieldScriptVMGetArgument(1));
    g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
}

void func_8008F4A0(void) {
    func_8003A55C(FieldScriptVMGetArgument(3) << 1, FieldScriptVMGetArgument(1));
    g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
}

void func_8008F4FC(void) {
    func_80085634(FieldScriptVMGetArgument(1), FieldScriptVMGetArgument(3));
    g_FieldScriptVMCurActor->scriptInstructionPointer += 5;
}

void func_8008F558(void) {
    s32 soundId = FieldScriptVMGetArgument(1);
    s32 volume = FieldScriptVMGetArgument(5);
    s32 pan = FieldScriptVMGetArgument(3);
    s32 channel = FieldScriptVMGetArgument(7);

    func_800855C8(soundId, volume, pan, channel);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 9;
}

/* FE 64 — sound-bit gate (asm 8008F5E4-8008F664).
 * FieldScriptVM2Run leaves IP on the 0x64 sub-opcode. Poll
 * func_8003A5D0(-1) & (arg1<<8): clear → IP+=3 (skip 64+2 args);
 * set → IP-=1 (back to FE, wait). Always yields (D_800B00C0=1).
 * A stubbed FE64 left IP on 0x64 and let the primary VM desync
 * A1 r5 past SHOW (0x22), so Fei stayed hidden after A14 HideById. */
extern s32 func_8003A5D0(s32);

void func_8008F5E4(void) {
    s32 soundBits = func_8003A5D0(-1);
    s32 mask = FieldScriptVMGetArgument(1) << 8;

    if ((soundBits & mask) != 0) {
        g_FieldScriptVMCurActor->scriptInstructionPointer -= 1;
    } else {
        g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
    }
    D_800B00C0 = 1;
}

void func_8008F668(void) {
    func_80085634(FieldScriptVMGetArgument(1), 3);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void func_8008F6AC(void) {
    func_800855C8(FieldScriptVMGetArgument(1), FieldScriptVMGetArgument(5), FieldScriptVMGetArgument(3), 3);
    g_FieldScriptVMCurActor->scriptInstructionPointer += 7;
}

extern s32 D_800ADBDC;
extern s32 D_8004F340;
extern s32 D_800ADB1C;
extern s32 D_8004F324;
extern s32 D_8004F308;
extern s32 D_8004F354;
extern void func_80085EEC(void);
extern void func_8001B66C(void);
extern void func_80085B20(s32);

void func_8008F724(void) {
    if (D_800ADBDC == 0) {
        D_800B00C0 = 1;
    } else {
        D_8004F340 = 0;
        func_8008F7B8();
    }
}

void func_8008F76C(void) {
    if (D_800ADBDC == 0) {
        D_800B00C0 = 1;
    } else {
        D_8004F340 = -1;
        func_8008F7B8();
    }
}

void func_8008F7B8(void) {
    s32 fieldId = FieldScriptVMGetArgument(1);

    if (D_800ADB1C == 0) {
        func_80085EEC();
        if (fieldId != D_8004F324) {
            func_8001B66C();
            D_8004F308 = -1;
        }
        D_8004F324 = fieldId;
        g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
        return;
    }

    if (func_8008A558() != 0) {
        D_800B00C0 = 1;
        return;
    }

    if (D_800ADBDC == 0) {
        D_800B00C0 = 1;
        return;
    }

    if (D_8004F354 == 1 || D_8004F308 == -1) {
        D_800B00C0 = 1;
        return;
    }

    if (fieldId != D_8004F324) {
        func_8001B66C();
        D_8004F324 = fieldId;
        D_8004F308 = -1;
        func_80085B20(fieldId);
    }

    g_FieldScriptVMCurActor->scriptInstructionPointer += 3;
}

void func_8008F90C(void) {
    u8* pScene = (u8*)&g_Scene;
    s32 arg1 = FieldScriptVMGetArgument(1);
    s32 arg3 = FieldScriptVMGetArgument(3);
    s32 arg5 = FieldScriptVMGetArgument(5);
    s32 steps = FieldScriptVMGetArgument(7);
    s32 dx, dy, dz;

    if (steps == 0) { steps = 1; }

    g_FieldScriptVMCurActor->scriptInstructionPointer += 9;

    dx = ((arg1 << 16) - *(s32*)(pScene + 0xA0)) / steps;
    dz = ((arg5 << 16) - *(s32*)(pScene + 0xA4)) / steps;
    dy = ((arg3 << 16) - *(s32*)(pScene + 0xA4)) / steps;

    *(s16*)(pScene + 0x9A) = (s16)steps;
    *(s16*)(pScene + 0x98) = 1;
    *(s32*)(pScene + 0xAC) = dx;
    *(s32*)(pScene + 0xB0) = dz;
    *(s32*)(pScene + 0xB4) = dy;

    if (arg1 == 0 && arg5 == 0 && arg3 == 0) {
        *(s16*)(pScene + 0x9A) = (s16)(steps + 2);
        *(s16*)(pScene + 0x9C) = 1;
    } else {
        *(s16*)(pScene + 0x9C) = 0;
    }
}
