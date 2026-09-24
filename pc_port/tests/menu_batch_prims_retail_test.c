/* Retail differential test for the menu batch-init builders called from
 * func_801D7C3C (func_801D5ED4, ...).  See lib/menu_retail_harness.h. */
#include "common.h"
#include "system/menu.h"
#include "main/game.h"
#include "menu_retail_harness.h"
#include <stddef.h>

#define G_MENU_PTR 0x800625A0u
#define MENU_BLOCK 0x80070000u
#define MANAGER 0x800A0000u
#define SIDE 0x800B0000u
#define ATLAS 0x800E0000u
#define GAMESTATE 0x8006D634u

static const HxField menu_fields[] = {
#include "system_menu_mirror.inc"
};
static SystemMenu native_menu;
SystemMenu* g_Menu = &native_menu;

enum { F_ATLAS = 0x8002675Cu, F_VERTS = 0x801C851Cu, F_RESET = 0x801E927Cu,
       F_TPAGE = 0x80043A1Cu, F_UV = 0x801E920Cu };
const HxCallee hx_callees[] = {
    {F_ATLAS, 7, "func_8002675C"}, {F_VERTS, 5, "func_801C851C"}, {F_RESET, 1, "func_801E927C"},
    {F_TPAGE, 4, "GetTPage"}, {F_UV, 7, "func_801E920C"},
};
const unsigned hx_ncallees = sizeof(hx_callees) / sizeof(hx_callees[0]);
static unsigned pattern;

uint32_t hx_effect(uint32_t fn, const uint32_t* a, unsigned index, int side) {
    (void)side;
    switch (fn) {
    case F_ATLAS: /* writes the glyph prim for the context: xy0 from x,y */
        hx_w16(a[2] + a[3] * 0x28 + 8, (uint16_t)(a[4] * 3 + pattern));
        hx_w16(a[2] + a[3] * 0x28 + 0xA, (uint16_t)(a[5] + 7 * pattern));
        return 1 + (pattern & 3);
    case F_RESET: hx_w32(a[0], 0x09000000u | index); hx_w32(a[0] + 4, 0x2C808080u); return 0;
    case F_TPAGE: return (a[0] << 7 | a[1] << 5 | a[2] >> 6 | a[3] >> 4) ^ (pattern << 11);
    case F_UV: hx_w8(a[0] + 0xC, (uint8_t)a[3]); hx_w8(a[0] + 0xD, (uint8_t)a[4]); return 0;
    default: return 0;
    }
}

#define N(fn, n, ...) hx_native(fn, n, __VA_ARGS__)
s32 func_8002675C(u8* atlas, s32 id, void* buf, s32 rc, s32 x, s32 y, s32 scale) {
    return (s32)N(F_ATLAS, 7, hx_addr(atlas), (uint32_t)id, hx_addr(buf), (uint32_t)rc, (uint32_t)x, (uint32_t)y, (uint32_t)scale);
}
void func_801C851C(SVECTOR* v, s32 x, s32 y, s32 w, s32 h) { N(F_VERTS, 5, hx_addr(v), (uint32_t)x, (uint32_t)y, (uint32_t)w, (uint32_t)h); }
void func_801E927C(POLY_FT4* p) { N(F_RESET, 1, hx_addr(p)); }
u_short GetTPage(int tp, int abr, int x, int y) { return (u_short)N(F_TPAGE, 4, (uint32_t)tp, (uint32_t)abr, (uint32_t)x, (uint32_t)y); }
void func_801E920C(POLY_FT4* p, s32 x, s32 y, s32 u, s32 v, s32 w, s32 h) {
    N(F_UV, 7, hx_addr(p), (uint32_t)x, (uint32_t)y, (uint32_t)u, (uint32_t)v, (uint32_t)w, (uint32_t)h);
}

extern void func_801D5ED4(u8, u8);

static void setup(unsigned rc) {
    uint32_t i;
    memset(&native_menu, 0, sizeof(native_menu));
    native_menu.renderContext = (int)rc;
    native_menu.unk2DC = (void*)(uintptr_t)ATLAS;
    native_menu.pManager = (MenuManager*)(uintptr_t)MANAGER;
    *(u32*)&native_menu.unk358[0] = SIDE;
    for (i = 0; i < 0x3000; ++i) hx_w8(SIDE + i, (uint8_t)(i * 13 + pattern));
    for (i = 0; i < 0x6C; ++i) hx_w8(MANAGER + i, (uint8_t)(i * 29 + pattern * 5));
    /* party character ids are 0..10 (the GameState character table) */
    for (i = 0; i < 4; ++i) hx_w8(MANAGER + 0x30 + i, (uint8_t)((i * 3 + pattern) % 11));
    for (i = 0; i < 0x2300; ++i) hx_w8(GAMESTATE + i, (uint8_t)(i * 7 + pattern * 3 + (i >> 8)));
    hx_w16(0x800595D4u, (uint16_t)(0x7000 + pattern));
    hx_w16(0x80059414u, (uint16_t)(0x3000 + pattern));
    hx_mirror_out(menu_fields, sizeof(menu_fields) / sizeof(menu_fields[0]), &native_menu, MENU_BLOCK);
    hx_w32(G_MENU_PTR, MENU_BLOCK);
}

static void finish(void) {
    static const uint32_t skip[][2] = {{MENU_BLOCK, MENU_BLOCK + 0x1E98}};
    hx_compare_events();
    hx_compare_ram(skip, 1);
    hx_mirror_compare(menu_fields, sizeof(menu_fields) / sizeof(menu_fields[0]), &native_menu, MENU_BLOCK);
}

int main(int argc, char** argv) {
    HX_REQUIRE(argc == 2, "usage: test disc/menu.bin");
    hx_map();
    hx_load(argv[1], 0x801C5000u, 0);
    /* func_801D5ED4(slot, variant) */
    hx_coverage(0x801D5ED4u, 0x801D6194u);
    for (pattern = 0; pattern < 12; ++pattern)
    for (unsigned rc = 0; rc < 2; ++rc)
    for (unsigned slot = 0; slot < 3; ++slot) /* party slots */
    for (unsigned variant = 0; variant < 3; ++variant) {
        ++hx_case;
        setup(rc);
        hx_begin_case();
        hx_run_retail(0x801D5ED4u, slot | 0xA500u, variant | 0x5A00u, 0, 0, 100000);
        hx_retail_done();
        func_801D5ED4((u8)slot, (u8)variant);
        finish();
    }
    hx_coverage_report("func_801D5ED4", NULL, 0);
    printf("MENU BATCH PRIMS PASS cases=%u checks=%u\n", hx_case, hx_checks);
    return 0;
}
