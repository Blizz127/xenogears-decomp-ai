void func_801DFE2C(u8 slotIdx) {
    void* pMenu = g_Menu;
    void* pManager = g_Menu->pManager;
    u8 charIdx = *(u8*)((u8*)pManager + 0x30 + slotIdx);
    u8* pEntry = (u8*)&g_GameState + 0x30C + charIdx * 0x28;
    u8 entryByte = pEntry[0];
    s32 gameData = *(s32*)((u8*)pMenu + 0x330);
    func_801E3ECC(gameData, entryByte);
    pMenu = g_Menu;
    pManager = g_Menu->pManager;
    charIdx = *(u8*)((u8*)pManager + 0x30 + slotIdx);
    pEntry = (u8*)&g_GameState + 0x30C + charIdx * 0x28;
    entryByte = pEntry[0];
    gameData = *(s32*)((u8*)pMenu + 0x330);
    func_801E3C2C(gameData, entryByte);
    {
        void* pGameData = *(void**)((u8*)pMenu + 0x330);
        *(u16*)((u8*)pGameData + 0xB8) = *(u16*)((u8*)pGameData + 0xB0);
        pGameData = *(void**)((u8*)pMenu + 0x330);
        *(u16*)((u8*)pGameData + 0xBA) = *(u16*)((u8*)pGameData + 0xA4);
        pGameData = *(void**)((u8*)pMenu + 0x330);
        *(u16*)((u8*)pGameData + 0xBC) = *(u16*)((u8*)pGameData + 0xA6);
        pGameData = *(void**)((u8*)pMenu + 0x330);
        *(u16*)((u8*)pGameData + 0xBE) = *(u8*)((u8*)pGameData + 0xB2);
        pGameData = *(void**)((u8*)pMenu + 0x330);
        *(u16*)((u8*)pGameData + 0xC0) = *(u8*)((u8*)pGameData + 0xB3);
        pGameData = *(void**)((u8*)pMenu + 0x330);
        *(u16*)((u8*)pGameData + 0xC2) = *(u8*)((u8*)pGameData + 0xB4);
    }
}
