void func_801D3C4C(u8 slotIdx, u16 x, u16 y, u16 h) {
    void* pMenu = g_Menu;
    u8* pBuf = *(u8**)((u8*)pMenu + 0x364 + slotIdx * 4);
    func_8002675C(*(s32*)((u8*)pMenu + 0x2DC), 0x105, pBuf + 0x410, *(s32*)((u8*)pMenu + 0x308), x, y, 0x1000);
    {
        s32 yAdj = (s32)y + (s32)(u16)h - 8;
        func_800263E4((u8*)(uintptr_t)*(u32*)((u8*)pMenu + 0x2DC), 0x105, pBuf + 0x460, *(s32*)((u8*)pMenu + 0x308), x, yAdj, 0x1000, 0, 1);
    }
    pMenu = g_Menu;
    func_8002675C(*(s32*)((u8*)pMenu + 0x2DC), 0x106, pBuf + 0x3C0, *(s32*)((u8*)pMenu + 0x308), x, y + 8, 0x1000);
    func_801C851C(pBuf + 0x6D0, x, y, 8, 8);
    func_801C851C(pBuf + 0x6F0, x, (y + h) & 0xFFFF, 8, (s32)(s16)0xFFF8);
    func_801C851C(pBuf + 0x6B0, x, (y + 8) & 0xFFFF, 8, (s32)(s16)h);
}
