#include "common.h"
#include "psyq/libgpu.h"
#include "system/memory.h"


extern void* g_SystemDataFile;
extern void* g_SystemFontFile;
extern void* g_SystemDataEntries;

extern u8 g_SystemPaletteData[];
extern s16 g_SystemPalette1;
extern s16 g_SystemPalette2;


INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/system", func_80032F54);

unsigned int ResolveArchiveEntryPointers(u32* pFile) {
    int i;
    u32* pEntries;
    u32 offset;

    offset = (u32)pFile;
    pEntries = pFile;
    for (i = 1; i <= *pFile; i++) {
        pFile[i] += offset;
    }
    
    return *pFile;
}

// Same as ResolveArchiveEntryPointers, but no return value
void ResolveFileEntryPointers(u32* pFile) {
    int i;
    u32* pEntries;
    u32 offset;

    offset = (u32)pFile;
    pEntries = pFile;
    for (i = 1; i <= *pFile; i++) {
        pFile[i] += offset;
    }
}

void* GetSystemFontFile(void) {
    return g_SystemFontFile;
}

void* GetSystemDataFile(void) {
    return g_SystemDataFile;
}

void SystemFreeFont(void) {
    HeapUnpinBlock(g_SystemFontFile);
    HeapFree(g_SystemFontFile);
    g_SystemFontFile = NULL;
}

void SystemFreeData(void) {
    HeapUnpinBlock(g_SystemDataFile);
    HeapFree(g_SystemDataFile);
    g_SystemDataFile = NULL;
}


INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/system", SystemInitializeFont);

void SystemInitializeData(void* pSystemData) {
    if (pSystemData == 0) {
        HeapSetCurrentContentType(HEAP_CONTENT_NONE);
        return;
    }
    HeapPinBlock(pSystemData);
    g_SystemDataFile = pSystemData;
    g_SystemDataEntries = pSystemData;
    ResolveArchiveEntryPointers(pSystemData);
    g_SystemDataEntries += 4;
}

void SystemInitialize(void* pSystemFont, void* pSystemData) {
    SystemInitializeFont(pSystemFont);
    SystemInitializeData(pSystemData);
}


void SystemTransferPaletteToVRAM(short xDest, short yDest) {
    RECT dest;
    setRECT(&dest, xDest, yDest, 32, 1);
    LoadImage(&dest, &g_SystemPaletteData);
    g_SystemPalette1 = GetClut(xDest, yDest);
    g_SystemPalette2 = GetClut(xDest + 16, yDest);
}

void* GetStringEntry(void* arg0, s32 arg1) {
    return (u8*)arg0 + *(u16*)((u8*)arg0 + arg1 * 2 + 4);
}

// TODO: Cleanup code
// Dialog data format:
// 0x0: Offset to dimension data
// 0x2: ?
// 0x4: ?
// 0x6: u16 dimensions[dialog_count]
u8 DialogGetWidth(u16* pDialogData, int dialogIndex) {
    u8* pDialog = (pDialogData + 3) + *pDialogData + dialogIndex;
    return pDialog[0];
}

u8 DialogGetHeight(u16* pDialogData, int dialogIndex) {
    u8* pDialog = (pDialogData + 3) + *pDialogData + dialogIndex;
    return pDialog[1];
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/system", func_80033784);

void* func_800337B8(s32 index) {
    return GetStringEntry(*(void**)((u8*)g_SystemDataEntries + 0x40), index);
}

void* GetAccessoryName(s32 index) {
    return GetStringEntry(*(void**)((u8*)g_SystemDataEntries + 0x44), index);
}

void* GetItemName(s32 index) {
    return GetStringEntry(*(void**)((u8*)g_SystemDataEntries + 0x58), index);
}

void* GetWeaponName(s32 index) {
    return GetStringEntry(*(void**)((u8*)g_SystemDataEntries + 0x5C), index);
}

void* func_80033878(s32 index) {
    return GetStringEntry(*(void**)((u8*)g_SystemDataEntries + 0x60), index);
}

void* func_800338A8(s32 index) {
    return GetStringEntry(*(void**)((u8*)g_SystemDataEntries + 0x64), index);
}

void* func_800338D8(s32 index) {
    return GetStringEntry(*(void**)((u8*)g_SystemDataEntries + 0x48), index);
}

void* func_80033908(s32 index) {
    return GetStringEntry(*(void**)((u8*)g_SystemDataEntries + 0x50), index);
}

void* func_80033938(s32 index) {
    return GetStringEntry(*(void**)((u8*)g_SystemDataEntries + 0x4C), index);
}

void* func_80033968(s32 index) {
    return GetStringEntry(*(void**)((u8*)g_SystemDataEntries + 0x54), index);
}

void* func_80033998(s32 index) {
    return GetStringEntry(*(void**)((u8*)g_SystemDataEntries + 0x6C), index);
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/system", func_800339C8);

void* func_800339FC(s32 index) {
    return GetStringEntry(*(void**)((u8*)g_SystemDataEntries + 0xC0), index);
}

void* func_80033A2C(s32 index) {
    return GetStringEntry(*(void**)((u8*)g_SystemDataEntries + 0xC8), index);
}

void* func_80033A5C(s32 index) {
    return GetStringEntry(*(void**)((u8*)g_SystemDataEntries + 0xCC), index);
}

void* func_80033A8C(s32 index) {
    return GetStringEntry(*(void**)((u8*)g_SystemDataEntries + 0xD0), index);
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/system", func_80033ABC);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/system", func_80033B34);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/system", func_80033BAC);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/system", func_80033C20);

s32 func_80033CD0(void* arg0) {
    if (*(u16*)((u8*)arg0 + 0x10) & 0x8) {
        return *(u8*)((u8*)arg0 + 0x6B);
    }
    return 0;
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/system", func_80033CF0);

void func_80033DD4(void* arg0, s32 arg1) {
    *(s32*)((u8*)arg0 + 0x20) = *(s32*)((u8*)arg0 + 0x1C);
    *(s32*)((u8*)arg0 + 0x1C) = arg1;
    *(u16*)((u8*)arg0 + 0x10) |= 0x80;
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/system", func_80033DF0);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/system", func_800345E0);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/system", func_80034614);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/system", func_8003463C);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/system", func_800346A4);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/system", func_800346D4);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/system", func_80034714);

s32 func_800347AC(void* arg0) {
    return *(s16*)((u8*)arg0 + 0x4) + (*(s16*)((u8*)arg0 + 0x0) << 2);
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/system", func_800347C0);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/system", func_80034800);

void func_80034874(void* arg0, u8 arg1) {
    *(u8*)((u8*)arg0 + 0x6E) = arg1;
}

int func_8003487C(void* arg0) {
    int value = 0xFF;
    *(u8*)((u8*)arg0 + 0x6E) = value;
    return value;
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/system", func_80034888);

// Render string entry to a buffer
INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/system", SystemRenderStringEntry);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/system", func_80034F98);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/system", func_80034FFC);
