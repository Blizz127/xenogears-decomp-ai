void func_801DA4A8(void) {
    void* pMenu;
    void* work;

    func_801D22F4(2);
    pMenu = g_Menu;
    func_801E8018(8, (u8*)pMenu + 0x10E0, D_801EA548,
                   (u8*)(g_Menu->pManager) + 0x38);
    work = HeapAlloc(0x1198, 0);
    pMenu = g_Menu;
    *(void**)((u8*)pMenu + 0x42C) = work;
    bzero(work, 0x1198);
    func_801C72BC(0);
}
