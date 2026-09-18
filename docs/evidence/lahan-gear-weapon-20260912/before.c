void func_801E4754(void* arg0, u8 charIdx) {
    u8* pGS = (u8*)&g_GameState + 0x978 + charIdx * 0xA4;
    void** pSrcBase = (void**)(arg0 + 0x18);
    u8 slot;
    u8* pSrc;

    slot = pGS[0xC];
    pSrc = (u8*)*pSrcBase + slot * 0x14;
    pGS[0x12] = pSrc[0xE];
    *(u16*)(pGS + 0x10) = *(u16*)(pSrc + 0x12);
    pGS[0x13] = pSrc[0x10];
    pGS[0x14] = pSrc[0x11];
    pGS[0x5C] = pSrc[0];
    pGS[0x5D] = pSrc[1];
    pGS[0x5E] = pSrc[2];
    pGS[0x5F] = pSrc[3];
    if (pGS[0x14] == 0x64) {
        u16 val = *(u16*)(pGS + 0x86);
        val = (val & 0xFFF) | *(u16*)(pGS + 0x10);
        *(u16*)(pGS + 0x86) = val;
    }
    if (charIdx == 5 || charIdx == 0xD) {
        slot = pGS[0x4];
        pSrc = (u8*)*pSrcBase + slot * 0x14;
        pGS[0x12] = pSrc[0xE];
        *(u16*)(pGS + 0x10) = *(u16*)(pSrc + 0x12);
        pGS[0x13] = pSrc[0x10];
        pGS[0x14] = pSrc[0x11];
        pGS[0x5D] = pSrc[1];

        slot = pGS[0x5];
        pSrc = (u8*)*pSrcBase + slot * 0x14;
        pGS[0x1A] = pSrc[0xE];
        *(u16*)(pGS + 0x18) = *(u16*)(pSrc + 0x12);
        pGS[0x1B] = pSrc[0x10];
        pGS[0x1C] = pSrc[0x11];
        pGS[0x5E] = pSrc[2];

        slot = pGS[0x7];
        pSrc = (u8*)*pSrcBase + slot * 0x14;
        pGS[0x22] = pSrc[0xE];
        *(u16*)(pGS + 0x20) = *(u16*)(pSrc + 0x12);
        pGS[0x23] = pSrc[0x10];
        pGS[0x24] = pSrc[0x11];
        pGS[0x5F] = pSrc[3];
    }
}

u8 func_801E4928(u8 idx) {
    u8* pEntry = (u8*)&g_GameState + 0x978 + (s32)idx * 0xA4;
    u16 val = *(u16*)(pEntry + 0x44);
    u8 base = pEntry[0x75];
    s32 result = (s32)(val / 15) - base;
    result /= 2;
    if (result < 0) result = 0;
    return (u8)(result & 0xFF);
}
