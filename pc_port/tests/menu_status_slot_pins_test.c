/* Pins Status slot 0x438 (SystemMenu.unk42C[3]) on the inflated host layout.
 *
 * Retail stores the Status work buffer at g_Menu+0x438. On LP64 that byte
 * offset is not unk42C[3] (host 0x54C); a native pointer load there HeapFrees
 * / draws through unrelated menu state. func_801E1398 and func_801D1914 must
 * go through MENU_RAW / MenuRawPointer, matching HeapFree(MENU_RAW(0x434))
 * in the Equip teardown. */
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "common.h"
#include "system/menu.h"

SystemMenu *g_Menu;
static SystemMenu menu;
static MenuManager manager;
static GfxEnvironment gfx;
static u8 status_buf[0x3000];
static u8 poison_buf[0x3000];
static void *freed;
static void *prims[8];
static int nprims;
static u8 *MenuRawPointer(u32 o)
{
    assert(o == 0x438);
    return status_buf;
}

void func_801D3674(void) {}
void func_801D4EA0(s32 a) { (void)a; }
void func_801C7BF4(void) {}
void func_801E8044(s32 a, void *p) { (void)a; (void)p; }
void func_801C72BC(s32 a) { (void)a; }
u_int HeapFree(void *p) { freed = p; return 0; }
void func_801CE198(s32 a, void *b, void *c, u8 d)
{
    (void)a;
    (void)b;
    (void)c;
    (void)d;
}
void func_801CE2B4(u8 a, void *b, u8 c)
{
    (void)a;
    (void)b;
    (void)c;
}
void AddPrim(void *ot, void *p)
{
    assert(ot == &gfx.ot[4]);
    assert(nprims < 8);
    prims[nprims++] = p;
}

#include "e1398.inc"
#include "d1914.inc"

int main(void)
{
    _Static_assert(offsetof(SystemMenu, unk42C[3]) != 0x438,
                   "this pin is for the inflated SystemMenu");
    _Static_assert(offsetof(SystemMenu, pGfxEnv) != 0x1D4,
                   "pGfxEnv also leaves its retail byte offset");

    memset(&menu, 0xAB, sizeof(menu));
    memset(&manager, 0, sizeof(manager));
    memset(status_buf, 0, sizeof(status_buf));
    memset(poison_buf, 0, sizeof(poison_buf));
    *(void **)((u8 *)&menu + 0x438) = poison_buf;

    g_Menu = &menu;
    menu.pManager = &manager;
    menu.pGfxEnv = &gfx;
    freed = NULL;
    func_801E1398();
    assert(freed == status_buf);
    assert(((u8 *)&manager)[0x4D] == 0);

    ((u8 *)&manager)[0x4D] = 1;
    status_buf[0x2596] = 1;
    status_buf[0x7D] = 0;
    nprims = 0;
    func_801D1914();
    assert(nprims == 2);
    assert(prims[0] == status_buf);
    assert(prims[1] == status_buf + 0x680);

    printf("PASS menu status slot pins (E1398/D1914 0x438 via MENU_RAW, ot[4])\n");
    return 0;
}
