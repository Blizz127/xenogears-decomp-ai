void func_801D1464(void) {
    void* pMenu = g_Menu;
    void* pManager = g_Menu->pManager;
    if (*(u8*)((u8*)pManager + 0x49) != 0) {
        void* pData = *(void**)((u8*)pMenu + 0x43C);
        func_801CE198(1, (u8*)pData + 0x50, pData, *(u8*)((u8*)pData + 0x70));
    }
}
