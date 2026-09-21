void func_801E3A80(void* pCtx, u8 charIdx) {
    u8* pEntry = (u8*)&g_GameState.characters[charIdx];
    u8* pDst = (u8*)pCtx;
    s32 val;

    if (pEntry[0x56] == 4) {
        val = (s32)(pEntry[4] + pEntry[0x1C]) * 6 / 5;
        *(u16*)(pDst + 0xB8) = (u16)val;
    } else {
        val = pEntry[0x58] + pEntry[0x28] + pEntry[4];
        *(u16*)(pDst + 0xB8) = (u16)val;
    }
    *(u16*)(pDst + 0xBA) = (u16)(pEntry[0x5E] + pEntry[0x2E]);
    *(u16*)(pDst + 0xBC) = (u16)(pEntry[0x59] + pEntry[0x29] + pEntry[0x2D]);
    *(u16*)(pDst + 0xBE) = (u16)(pEntry[0x5F] + pEntry[0x2F]);
    *(u16*)(pDst + 0xC0) = (u16)(pEntry[0x5B] + pEntry[0x2B]);
    *(u16*)(pDst + 0xC2) = (u16)(pEntry[0x5C] + pEntry[0x2C]);
    *(u16*)(pDst + 0xC4) = (u16)(pEntry[0x5A] + pEntry[0x2A]);

    if (*(u16*)(pDst + 0xB8) >= 0xFB) {
        *(u16*)(pDst + 0xB8) = 0xFA;
    }
    if (*(u16*)(pDst + 0xBA) >= 0x64) {
        *(u16*)(pDst + 0xBA) = 0x63;
    }
    if (*(u16*)(pDst + 0xBC) >= 0xFB) {
        *(u16*)(pDst + 0xBC) = 0xFA;
    }
    if (*(u16*)(pDst + 0xBE) >= 0x64) {
        *(u16*)(pDst + 0xBE) = 0x63;
    }
    if (*(u16*)(pDst + 0xC0) >= 0xFB) {
        *(u16*)(pDst + 0xC0) = 0xFA;
    }
    if (*(u16*)(pDst + 0xC2) >= 0xFB) {
        *(u16*)(pDst + 0xC2) = 0xFA;
    }
    if (*(u16*)(pDst + 0xC4) >= 0x15) {
        *(u16*)(pDst + 0xC4) = 0x10;
    }
}
