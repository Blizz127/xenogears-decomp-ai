#include "common.h"
#include "psyq/libgpu.h"
#include "psyq/libgte.h"
#include "field/actor.h"
#include "field/main.h"
#include "field/camera.h"
#include "system/memory.h"

// Light data stuff
INCLUDE_ASM("asm/field/nonmatchings/main/misc3", func_8006FDEC);

void FieldLZSSDecompress(void* _unused, void* pCompressed, void* pDecompressed) {
    LZSSDecompress(pCompressed, pDecompressed);
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc3", FieldFree);

void FieldLoadTIMWithClut(u_long *pTimData, short x, short y, short clutX, short clutY, short clutWidth, short clutHeight) {
    TIM_IMAGE* pTIM;
    TIM_IMAGE tim;   

    OpenTIM(pTimData);
    pTIM = ReadTIM(&tim);

    if (pTIM) {
        if (tim.caddr) {
            if (clutY != -1) {
                tim.crect->x = clutX;
                tim.crect->y = clutY;
            }

            if (clutWidth)
                tim.crect->w = clutWidth;
            
            if (clutHeight)
                tim.crect->h = clutHeight;

            LoadImage(tim.crect, tim.caddr);
        }

        if (tim.paddr) {
            tim.prect->x = x;
        }
        tim.prect->y = y;
        LoadImage(tim.prect, tim.paddr);
    }
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc3", func_80070488);

extern s32 D_800ADB60;
extern void* D_800ADC14;

void func_80070508(void) {
    if (D_800ADB60 == 1) {
        ArchiveCdDataSync(0);
        DrawSync(0);
        HeapFree(D_800ADC14);
        D_800ADB60 = 0;
    }
    func_80078C5C();
}

void func_80070560(s32* dest, s16* src) {
    dest[0] = src[0] << 16;
    dest[1] = src[1] << 16;
    dest[2] = src[2] << 16;
}

void func_80070594(MATRIX* dest) {
    SVECTOR rotation;

    rotation.vx = 0;
    rotation.vy = 0;
    rotation.vz = 0;
    RotMatrix(&rotation, dest);
    dest->t[2] = 0;
    dest->t[1] = 0;
    dest->t[0] = 0;
}

INCLUDE_ASM("asm/field/nonmatchings/main/misc3", func_800705DC);

extern u16 D_800B06A4[];
extern u16 D_800B06A6[];
extern s32 D_800ADB0C;

void func_80070C84(void) {
    s32 i;
    for (i = 0; i < 3; i++) {
        D_800B06A4[i * 3] = 0xFF;
        D_800B06A6[i * 3] = 0xFF;
    }
    D_800ADB0C = 0;
}

/* ---- FieldLoad: parse the raw map file and build the field runtime state -----
 * Port-first functional decompile (control flow mirrors
 * asm/field/nonmatchings/main/misc3/FieldLoad.s 1:1). The map file has already
 * been streamed into the buffer pointed to by D_8005A4E0 (an ActorFile header
 * followed by LZSS-compressed sections). This routine:
 *   - copies the 0x100-byte TIM/CLUT header table out of the map,
 *   - HeapAllocs + FieldLZSSDecompresses each section (TIMs, cluts, model data,
 *     sprite data, scripts, dialogs, triggers, walkmesh),
 *   - sets the globals g_FieldActors / g_FieldNumActors / g_FieldSpriteData /
 *     g_pFieldTriggerZones / g_FieldCurScriptFile,
 *   - runs the per-actor (0x5C-byte) init loop.
 * Section offsets/sizes are read from the map header by raw byte offset to stay
 * faithful to the asm (see the ActorFile struct in field/actor.h for the map).
 * Every jal is preserved in order; callees not yet decompiled are no-op stubs in
 * the port. */

extern void* D_8005A4E0;

/* Data buffers / scratch globals touched by FieldLoad (raw port-stubbed data). */
extern u8 D_800B1F78[];     /* 0x100-byte header table copied out of the map */
extern u8 D_800B06BC[];     /* 4x4 grid of Quads (0x70 each) */
extern u8 D_800B0DBC[];     /* 5 Quads (0x70 each) */
extern void* D_800AFB14;    /* decompressed model-data buffer (0x114 section) */
extern void* D_800AFB18;    /* decompressed sprite/model-2 buffer (0x134 section) */
/* One contiguous scratch block in the retail binary: D_800AFB20 is the base,
 * D_800AFB24 == D_800AFB20[1], and D_800AFB54 == ((s16*)D_800AFB20)[0x1A].
 * Declared as a single array so the (port-stubbed) storage stays contiguous. */
extern u32 D_800AFB20[];    /* fixup base pointer table (>= 0x38 bytes) */
#define D_800AFB24 (D_800AFB20[1])
#define D_800AFB54 (((s16*)D_800AFB20)[0x1A])
extern s32 D_800AFD10;
extern s32 D_800ADBFC;      /* == g_FieldNumActors snapshot (script actor count) */
extern void* D_800ADBF0;    /* decompressed dialogs buffer (0x128 section) */
extern u8 D_800658DC[];     /* walkmesh decompress scratch/destination */
extern s32 D_8004F330;
extern s32 D_8004F334;

/* Geometry / camera work-area symbols used by the render-setup tail. Each is a
 * distinct absolute data symbol in the asm (auto-stubbed in the port). */
extern u8  D_800B223C[];    /* ZoomFadeEffect-ish work area (see main.h) */
extern s16 D_800B0080, D_800B0082, D_800B0084, D_800B0086, D_800B0088;
extern s16 D_800B008A, D_800B008C, D_800B008E;
extern s32 D_800B0090, D_800B0094, D_800B0098;
extern s8  D_800B00A0, D_800B00A1, D_800B00A2, D_800B00A4, D_800B00A5;
extern s8  D_800B00A6, D_800B00A8, D_800B00A9, D_800B00AA;
extern s16 D_800B00AC, D_800B00AE, D_800B00B0, D_800B00B2;
extern s8  D_800B225C, D_800B225D, D_800B225E;
extern u8  D_800AFC30[];
extern u32 D_800AFC44, D_800AFC48, D_800AFC4C;
extern s32 D_800ADB1C;
extern s32 D_800B007C;
extern u16 D_800B233E;
extern VECTOR g_CameraAt2;
extern u32 D_8006FAF4;      /* stack-init source (dead copy at entry) */
extern void func_8006FDEC(void* pLightData);

/* Externs for callees that don't yet have a C signature. */
extern void func_800705DC(void);
extern void func_8007A7F4(Quad* pPart, int x, int y, int tex);
extern void func_8007A5C4(void);
extern void func_80077844();
extern void func_80077C60(void);
extern void func_8007469C(void);
extern void func_80080F44(void);
extern void func_802812A4(void);
extern void func_800A28D4(void);
extern void func_800A2714(void);
extern void func_80073E38(void);
extern void func_80077268(void);
extern void func_800303C8(void* modelData, int a1);
extern void func_8002CB54(void* modelData, u32* out1, u32* out2);
extern void func_8002C8CC(void* a0, void* a1, int a2);
extern void func_8002C644(void* a0);
extern int  func_8002C3E8(void* a0);
extern int  func_8002709C();
extern void func_800223B0(void* a0, s16 a1);
extern void FieldTextBoxInitialize(void);
extern void FieldLoadTIM(u_long* pTimData);
extern void GfxLoadClutsAccelerated(void* pClutData);
extern void GfxAllocateWorkBuffers(int workBufferSize, unsigned int allocFlag);

void FieldLoad(void) {
    u8* pMap;
    u32* pTimTable;   /* decompressed TIM-package section */
    u32* pClutTable;  /* decompressed CLUT section */
    s32 numEntries;
    s32 i, j;
    s32 numActors;

    /* Entry: copies a 4-word blob out of D_8006FAF4 to the stack (dead; the
     * slots are never read again) then does field-camera / controller setup. */
    (void)D_8006FAF4;
    func_800705DC();

    /* Copy the 0x100-byte TIM/CLUT header table out of the map header. The asm
     * chooses an aligned (lw/sw) or unaligned (lwl/lwr) copy loop; both move the
     * same 0x100 bytes. */
    pMap = (u8*)D_8005A4E0;
    memcpy(D_800B1F78, pMap, 0x100);

    /* Build the 4x4 grid of compass/background quads, then 5 extra quads. */
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            s32 idx = i * 4 + j;
            func_8007A7F4((Quad*)(D_800B06BC + idx * 0x70), j, i, 0);
        }
    }
    func_8007A7F4((Quad*)(D_800B0DBC + 0x00), 4, 4, 1);
    func_8007A7F4((Quad*)(D_800B0DBC + 0x70), 5, 5, 1);
    func_8007A7F4((Quad*)(D_800B0DBC + 0xE0), 6, 6, 1);
    func_8007A7F4((Quad*)(D_800B0DBC + 0x150), 7, 7, 1);
    func_8007A7F4((Quad*)(D_800B0DBC + 0x1C0), 8, 8, 1);
    func_8007A5C4();

    /* --- TIM-package section (size 0x10C, offset 0x130): load each TIM ------- */
    pMap = (u8*)D_8005A4E0;
    pTimTable = (u32*)HeapAlloc(*(u32*)(pMap + 0x10C) + 0x10, 1);
    FieldLZSSDecompress(NULL,
                        (u8*)D_8005A4E0 + *(u32*)((u8*)D_8005A4E0 + 0x130),
                        pTimTable);
    numEntries = (s32)pTimTable[0];
    for (i = 0; i < numEntries; i++) {
        FieldLoadTIM((u_long*)((u8*)pTimTable + pTimTable[1 + i]));
    }

    /* --- CLUT section (size 0x11C, offset 0x140): accelerated CLUT upload ---- */
    pMap = (u8*)D_8005A4E0;
    pClutTable = (u32*)HeapAlloc(*(u32*)(pMap + 0x11C) + 0x10, 0);
    FieldLZSSDecompress(NULL,
                        (u8*)D_8005A4E0 + *(u32*)((u8*)D_8005A4E0 + 0x140),
                        pClutTable);
    numEntries = (s32)pClutTable[0];
    numEntries <<= 3; /* iterate the header table in 8-byte strides */
    for (i = 0; i < numEntries; i += 8) {
        /* D_800B1F78 header holds, per entry: [+0]=u16 idx, [+2]=u16, [+4]=s16 flag */
        s16 flag = *(s16*)(D_800B1F78 + i + 6);
        if (flag == 0) {
            /* clut table entries start after the count word */
            GfxLoadClutsAccelerated((u8*)pClutTable + pClutTable[1 + (i >> 3)]);
        }
    }
    DrawSync(0);
    HeapFree(pTimTable);
    HeapFree(pClutTable);

    /* --- Model-data section (size 0x114, offset 0x138) ---------------------- */
    D_800AFB14 = HeapAlloc(*(u32*)((u8*)D_8005A4E0 + 0x114) + 0x10, 0);
    FieldLZSSDecompress(NULL,
                        (u8*)D_8005A4E0 + *(u32*)((u8*)D_8005A4E0 + 0x138),
                        D_800AFB14);
    {
        /* buffer layout: [0]=count, [1..]=per-entry byte offsets into the buffer */
        u32* pOff = (u32*)D_800AFB14 + 1;
        numEntries = (s32)*(u32*)D_800AFB14;
        for (i = 0; i < numEntries; i++) {
            func_8002C3E8((u8*)D_800AFB14 + *pOff);
            pOff++;
            numEntries = (s32)*(u32*)D_800AFB14; /* asm reloads count each iter */
        }
    }

    /* --- Walkmesh section (size 0x124, offset 0x148): decompress into scratch */
    FieldLZSSDecompress(NULL,
                        (u8*)((u8*)D_8005A4E0 + *(u32*)((u8*)D_8005A4E0 + 0x148)),
                        D_800658DC + 0x10);

    /* --- Scripts section (size 0x120, offset 0x144) ------------------------- */
    g_FieldCurScriptFile =
        (ScriptsFile*)HeapAlloc(*(u32*)((u8*)D_8005A4E0 + 0x120) + 0x10, 0);
    FieldLZSSDecompress(NULL,
                        (u8*)D_8005A4E0 + *(u32*)((u8*)D_8005A4E0 + 0x144),
                        g_FieldCurScriptFile);
    D_800ADBFC = (s32)g_FieldCurScriptFile->numScripts;
    g_FieldScriptVMCurScriptData =
        (u8*)&g_FieldCurScriptFile->metadata + (g_FieldCurScriptFile->numScripts << 6);

    /* --- Triggers section (size 0x12C, offset 0x150) ------------------------ */
    g_pFieldTriggerZones =
        (FieldTriggerZone*)HeapAlloc(*(u32*)((u8*)D_8005A4E0 + 0x12C) + 0x10, 0);
    FieldLZSSDecompress(NULL,
                        (u8*)D_8005A4E0 + *(u32*)((u8*)D_8005A4E0 + 0x150),
                        g_pFieldTriggerZones);

    /* --- Dialogs section (size 0x128, offset 0x14C) ------------------------- */
    D_800ADBF0 = HeapAlloc(*(u32*)((u8*)D_8005A4E0 + 0x128) + 0x10, 0);
    FieldLZSSDecompress(NULL,
                        (u8*)D_8005A4E0 + *(u32*)((u8*)D_8005A4E0 + 0x14C),
                        D_800ADBF0);

    /* --- Second model/sprite section (size 0x110, offset 0x134) ------------- */
    D_800AFB18 = HeapAlloc(*(u32*)((u8*)D_8005A4E0 + 0x110) + 0x10, 0);
    FieldLZSSDecompress(NULL,
                        (u8*)D_8005A4E0 + *(u32*)((u8*)D_8005A4E0 + 0x134),
                        D_800AFB18);
    /* Fixup pass over the decompressed model-2 buffer. The buffer begins with a
     * count word, then a run of raw words. The first 4 are scaled ((w>>1)/7) and
     * written to D_800AFB18+0x2C..0x38; the rest are relocated (add the buffer
     * base) into the D_800AFB20 pointer table. */
    {
        const u32 kDiv = 0x92492493u; /* fixed-point reciprocal used for /7 */
        u8* pBufBase = (u8*)D_800AFB18;
        u32* pSrc;
        u32* pDst = (u32*)(pBufBase + 0x2C);
        u32* pBaseTab = D_800AFB20;

        D_800AFB54 = (s16)*(u32*)pBufBase;  /* count */
        pSrc = (u32*)pBufBase + 1;

        for (i = 0; i < 4; i++) {
            u32 w = *pSrc++;
            u64 prod = (u64)(w >> 1) * (u64)kDiv;
            *pDst++ = (u32)(prod >> 32) >> 2;
        }

        pBaseTab[0] = *pSrc++ + *(u32*)pBufBase;   /* D_800AFB20 = base + off */
        if (D_800AFB54 > 0) {
            u32* pA = &pBaseTab[1];        /* a2 walks from D_800AFB20+4  */
            u32* pB = &pBaseTab[1 + 4];    /* a1 walks from D_800AFB20+0x14 */
            s32 k = 0;
            do {
                pA[0] = *pSrc++ + pBaseTab[0];
                pB[0] = *pSrc++ + pBaseTab[0];
                k++;
                pA++;
                pB++;
            } while (k < *(s16*)((u8*)pBaseTab + 0x34)); /* lh 0x30(a2), a2 moved +4 */
        }
        D_800AFD10 = (s32)((D_800AFB24 - D_800AFB20[0]) >> 2);
    }

    /* --- Sprite-data section (size 0x118, offset 0x13C) --------------------- */
    g_FieldSpriteData = HeapAlloc(*(u32*)((u8*)D_8005A4E0 + 0x118) + 0x10, 0);
    FieldLZSSDecompress(NULL,
                        (u8*)D_8005A4E0 + *(u32*)((u8*)D_8005A4E0 + 0x13C),
                        g_FieldSpriteData);

    /* Reset scene world-rotation flags and load light data (header + 0x154). */
    *(s16*)((u8*)&g_Scene + 0x4C) = 1;
    *(s16*)((u8*)&g_Scene + 0x4E) = 1;
    *(s16*)((u8*)&g_Scene + 0x50) = 1;
    *(s16*)((u8*)&g_Scene + 0x52) = 1;
    func_8006FDEC((u8*)D_8005A4E0 + 0x154);

    /* --- Allocate + zero g_FieldActors, then per-actor init loop ------------ */
    {
        u16 nActors;
        u32* pClear;
        s32 nWords;

        pMap = (u8*)D_8005A4E0;
        nActors = *(u16*)(pMap + 0x18C);       /* header numEntitites */
        /* entity records begin at header + 0x190; s5 walks them */
        /* nWords = nActors * 0x5C / 4 = nActors * 0x17 -> see asm shift math.
         * FieldActor now uses u32 pointer slots so sizeof==0x5C on both the MIPS
         * matching target and the 64-bit port; the faithful nWords path is correct
         * everywhere. */
        nWords = ((((nActors << 1) + nActors) << 3) - nActors); /* nActors*0x17 */
        g_FieldNumActors = nActors;
        pClear = (u32*)HeapAlloc(nWords << 2, 0);
        g_FieldActors = (FieldActor*)pClear;
        for (i = 0; i < nWords; i++) {
            pClear[i] = 0;
        }
    }

    numActors = g_FieldNumActors;
    if (numActors > 0) {
        u16* pEntry = (u16*)((u8*)D_8005A4E0 + 0x190);
        for (i = 0; i < numActors; i++) {
            FieldActor* pActor = &g_FieldActors[i];
            u16 status;

            pActor->status = *pEntry; pEntry += 1;   /* 0x58 */
            pActor->rotation.x = *(pEntry + 0);       /* 0x50 */
            pActor->rotation.y = *(pEntry + 1);       /* 0x52 */
            pActor->rotation.z = *(pEntry + 2);       /* 0x54 */
            pEntry += 3;
            /* three 32-bit position pairs (0x20/0x40, 0x24/0x44, 0x28/0x48) */
            *(s32*)((u8*)pActor + 0x20) = *(pEntry + 0);
            *(s32*)((u8*)pActor + 0x40) = *(pEntry + 0);
            *(s32*)((u8*)pActor + 0x24) = *(pEntry + 1);
            *(s32*)((u8*)pActor + 0x44) = *(pEntry + 1);
            *(s32*)((u8*)pActor + 0x28) = *(pEntry + 2);
            *(s32*)((u8*)pActor + 0x48) = *(pEntry + 2);
            pEntry += 3;

            status = (u16)g_FieldActors[i].status;
            if ((status & 0x40) == 0) {
                void* pModel = HeapAlloc(0x24, 0);
                u32* pOffTab;
                u16 spriteId;

                g_FieldActors[i].pModelData = (u32)(uintptr_t)pModel;
                spriteId = *pEntry;
                pOffTab = (u32*)((u8*)D_800AFB14 + (spriteId << 2));
                /* pModel holds PSX 4-byte pointer slots (+0x4 modelData, +0x8/+0xC
                 * the model's double-buffer halves). Store/load them as truncated
                 * u32 (host RAM is linked below 4 GiB, so it round-trips): an 8-byte
                 * host-pointer store here would clobber the neighbouring slot. */
                *(u32*)((u8*)pModel + 0x4) =
                    (u32)((u8*)D_800AFB14 + pOffTab[1] + 0x10);
#ifndef XENO_PC_PORT
                /* XENO_PC_PORT stopgap: the per-actor model build below (double-
                 * buffer alloc, GPU command-list build, memcpy, skeletal work block,
                 * pin) dispatches through D_8004FE50 -> the 0x8002Exxx model-
                 * primitive processors, which aren't ported yet. The table is a
                 * zeroed stub so func_8002C8CC calls a NULL callback, and running the
                 * dependent memcpy/func_800303C8 without its setup corrupts the heap.
                 * Skip the whole model build so FieldLoad completes: each model actor
                 * keeps an allocated-but-empty control block (pModel, with modelData
                 * at +0x4; +0x8/+0xC stay NULL). The field then reaches its render
                 * loop and can show the map background without actor models. Remove
                 * once the D_8004FE50 primitive subsystem is ported. */
                func_8002CB54((void*)(u32)*(u32*)((u8*)pModel + 0x4),
                              (u32*)((u8*)pModel + 0x8),
                              (u32*)((u8*)pModel + 0xC));
                {
                    /* asm: a0 = pModel[0x4] (modelData, the model with header),
                     * a1 = pModel[0x8] (out1), a2 = (status & 0xC) >> 2. */
                    u16 st = (u16)g_FieldActors[i].status;
                    func_8002C8CC((void*)(u32)*(u32*)((u8*)pModel + 0x4),
                                  (void*)(u32)*(u32*)((u8*)pModel + 0x8),
                                  (st & 0xC) >> 2);
                }
                {
                    void* pSrcModel = (void*)(u32)*(u32*)((u8*)pModel + 0x4);
                    memcpy((void*)(u32)*(u32*)((u8*)pModel + 0xC),
                           (void*)(u32)*(u32*)((u8*)pModel + 0x8),
                           *(s32*)((u8*)pSrcModel + 0x34));
                }
                if (g_FieldActors[i].status & 0x2000) {
                    HeapChangeCurrentUser(HEAP_USER_KAZM, NULL);
                    func_800303C8((void*)(u32)*(u32*)((u8*)pModel + 0x4), 0);
                    /* asm: sw v0, 0x14(s0) captures HeapChangeCurrentUser's
                     * return (prior user tag); the shared header types it void,
                     * so preserve the store without the (unused) value. */
                    HeapChangeCurrentUser(HEAP_USER_YOSI, NULL);
                    *(s32*)((u8*)pModel + 0x14) = 0;
                }
                func_8002C644((void*)(u32)*(u32*)((u8*)pModel + 0x4));
#endif
            } else {
                g_FieldActors[i].status = status | 0x20;
                g_FieldActors[i].rotation.x = 0;
                g_FieldActors[i].rotation.y = 0;
                g_FieldActors[i].rotation.z = 0;
            }
            pEntry += 1;    /* asm: s5 += 2, delay slot of func_80080F44 */
            func_80080F44();
        }
    }

    /* --- PC-HDD dev-only path guard ---------------------------------------- */
    if (g_FieldSystemMode == 0) {
        func_802812A4();
    }
    FieldTextBoxInitialize();
    FieldFadeInitialize();

    HeapUnpinBlock(D_8005A4E0);
    HeapFree(D_8005A4E0);
    HeapChangeCurrentUser(HEAP_USER_MIYA, NULL);
    GfxAllocateWorkBuffers(0x3C00, 0);
    WorkListsReset();
    HeapChangeCurrentUser(HEAP_USER_YOSI, NULL);

    /* Geometry / camera work-area setup (D_800B223C.. and the D_800B00xx block).
     * The asm emits absolute stores to a set of distinct data symbols; each is a
     * separate (auto-stubbed) symbol in the port, so we store to them by name. */
    func_80077844((short*)D_800B223C, 0x800, 0, 0, 0x800, 0, 0, 0, 0);
    func_80077844((short*)(D_800B223C - 0x20), 0x1F8, -0xFC1, -0x1F8, 0, 0, 0, 0, 0);

    D_800B225E = 0x1E;
    D_800B225D = 0x1E;
    D_800B225C = 0x1E;
    D_800B0084 = 0x140;
    D_800B008E = 0;
    D_800B00A9 = 0;
    D_800B00A8 = 0;
    D_800B00A6 = 0;
    D_800B00A5 = 0;
    D_800B00A4 = 0;
    D_800B00A2 = 0;
    D_800B00A1 = 0;
    D_800B00A0 = 0;                 /* sb zero, 0x1C(s2) */
    D_800B0090 = 0;                 /* sw zero, 0xC(s2) */
    D_800B0098 = 0x1000;
    D_800B00B0 = 0;
    D_800B00AE = 0;
    D_800B00AC = 0;
    D_800B008C = 0;
    D_800B008A = 0;
    D_800B0088 = 0;
    D_800B0086 = 0;
    D_800B0082 = 0;
    D_800B0080 = 0;
    D_800B00B2 = 0;
    D_800B0094 = 0;
    D_800B00AA = 0x20;
    D_800ADB1C = 0;
    func_800A28D4();

    D_800ADB1C = 1;
    RotMatrix((SVECTOR*)(D_800B223C - 0xB8), (MATRIX*)D_800AFC30);
    D_800AFC4C = 0;
    D_800AFC48 = 0;
    D_800AFC44 = 0;
    if (D_800B00B2 != 0) {
        /* func_8002709C(&D_800B0084 block ...) -> D_800B007C. Mirror the asm's
         * argument marshalling (a mix of the D_800B00xx shorts). */
        D_800B007C = func_8002709C(
            D_800B0080, D_800B0082, D_800B0084, D_800B0086,
            D_800B0088, D_800B008A, D_800B008C, D_800B008E,
            (short*)(D_800B223C + 0x0C),   /* s1 = s2 + 0xC */
            (short*)(D_800B223C + 0x1C),   /* s0 = s2 + 0x1C */
            D_800B00AC, D_800B00AE, D_800B00B0);
    }

    HeapChangeCurrentUser(HEAP_USER_YOSI, NULL);
    {
        u16 nA2 = D_800B233E;
        s32 stride = (((nA2 << 1) + nA2) << 3) - nA2;   /* nA2 * 0x17 */
        u32* pA = (u32*)((u8*)g_FieldActors + (stride << 2));
        g_CameraAt2.vx = (s32)pA[8] << 16;              /* +0x20 */
        g_CameraAt2.vy = (s32)pA[9] << 16;              /* +0x24 */
        g_CameraAt2.vz = (s32)pA[10] << 16;             /* +0x28 */
    }

    /* Per-actor matrix setup loop (RotMatrix + copy transform blocks). */
    numActors = g_FieldNumActors;
    if (numActors > 0) {
        for (i = 0; i < numActors; i++) {
            FieldActor* pActor = &g_FieldActors[i];
            RotMatrix((SVECTOR*)((u8*)pActor + 0x50), (MATRIX*)((u8*)pActor + 0xC));
            /* copy 0xC..0x28 -> 0x2C..0x48 (two MATRIX-ish blocks) */
            *(s32*)((u8*)pActor + 0x2C) = *(s32*)((u8*)pActor + 0x0C);
            *(s32*)((u8*)pActor + 0x30) = *(s32*)((u8*)pActor + 0x10);
            *(s32*)((u8*)pActor + 0x34) = *(s32*)((u8*)pActor + 0x14);
            *(s32*)((u8*)pActor + 0x38) = *(s32*)((u8*)pActor + 0x18);
            *(s32*)((u8*)pActor + 0x3C) = *(s32*)((u8*)pActor + 0x1C);
            *(s32*)((u8*)pActor + 0x40) = *(s32*)((u8*)pActor + 0x20);
            *(s32*)((u8*)pActor + 0x44) = *(s32*)((u8*)pActor + 0x24);
            *(s32*)((u8*)pActor + 0x48) = *(s32*)((u8*)pActor + 0x28);
        }
    }

    func_80077C60();
    func_800A2714();
    D_8004F334 = -1;
    D_8004F330 = -1;
    func_80073E38();
    func_80077268();

    if (D_800B00B2 != 0) {
        g_FieldRenderContextUseOT2 = 1;
    } else {
        func_8007469C();
        g_FieldRenderContextUseOT2 = 1;
    }

    /* Final per-script-actor animation init loop. */
    numActors = D_800ADBFC;
    if (numActors > 0) {
        for (i = 0; i < numActors; i++) {
            FieldActor* pActor = &g_FieldActors[i];
            u16 status = *(u16*)((u8*)pActor + 0x58);
            if (status & 0x40) {
#ifndef XENO_PC_PORT
                /* This loop reads pActorData (offset 0x4C) which is populated by
                 * func_80080F44 (called per-actor above). In the port func_80080F44
                 * is a stub, so pActorData stays NULL and this dereference crashes.
                 * Guard it until func_80080F44 / the actor-data init is ported. */
                void* pModel = (void*)(uintptr_t)*(u32*)((u8*)pActor + 0x4C);
                u32 flag = *(u32*)((u8*)pModel + 0x4);
                if (flag & 0x1000000) {
                    func_80021FE0((void*)(uintptr_t)*(u32*)((u8*)pActor + 0x4),
                                  *(s16*)((u8*)pModel + 0x108));
                } else {
                    s16 v = (s16)(g_CamInterpolation.curAngleY +
                                  *(u16*)((u8*)pModel + 0x108));
                    func_800223B0((void*)(uintptr_t)*(u32*)((u8*)pActor + 0x4), v);
                }
#endif
            }
        }
    }
}

