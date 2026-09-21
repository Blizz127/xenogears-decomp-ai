void func_801DA518(void) {
    void* pMenu;
    void* pWork;

    func_801D3444();
    func_801D4EA0(3);
    func_801D4EA0(4);
    pMenu = g_Menu;
    ((u8*)g_Menu->pManager)[0x48] = 0;
    func_801C72BC(0x10);
    pMenu = g_Menu;
    pWork = *(void**)((u8*)pMenu + 0x42C);
    HeapFree(*(void**)((u8*)pWork + 0x1180));
    pMenu = g_Menu;
    HeapFree(*(void**)((u8*)pMenu + 0x42C));
    pMenu = g_Menu;
    HeapFree(*(void**)(*(void**)((u8*)pMenu + 0x330) + 0x1C));
}
