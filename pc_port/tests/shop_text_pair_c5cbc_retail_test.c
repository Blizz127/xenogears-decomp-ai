#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "system/menu.h"

extern void func_801C5CBC(void*, u8*, s32, s32);
SystemMenu* g_Menu;

static u8 menu_storage[0x800];
static u8 output[0x400];
static u8 work[0x200];
static u8 bundle[0x40];
static unsigned string_calls;
static unsigned render_calls;
static unsigned shape_calls;
static unsigned upload_calls;
static s32 rendered_indices[8];
static s32 shaped_indices[8];
static s32 shaped_offsets[8];

static void check(int ok, const char* what, s32 value);

void* GetStringEntry(void* pBundle, s32 index)
{
    check(pBundle == bundle, "bundle", index);
    rendered_indices[string_calls++] = index;
    return bundle + index;
}

s32 SystemRenderStringEntry(void* string, void* pWork, s32 height, s32 flag)
{
    s32 index = (s32)((u8*)string - bundle);
    check(pWork == work && height == 0x18, "render args", index);
    check(flag == (s32)(render_calls & 1), "render flag", index);
    render_calls++;
    return 0x20 + index;
}

void func_801C5A7C(MenuString* pString, s32 index, s32 offset, u8 flags)
{
    check((u8*)pString >= output && (u8*)pString < output + sizeof(output), "shape pointer", index);
    check(flags == 0, "shape flags", index);
    shaped_indices[shape_calls] = index;
    shaped_offsets[shape_calls++] = offset;
}

int LoadImage(RECT* rect, u_long* data)
{
    check((u8*)rect == output + (upload_calls / 1) * 0x100 + 0x70, "upload rect", upload_calls);
    check((u8*)data == work, "upload buffer", upload_calls);
    upload_calls++;
    return 0;
}

int DrawSync(int mode)
{
    check(mode == 0, "draw sync", mode);
    return 0;
}

void check(int ok, const char* what, s32 value)
{
    if (!ok) {
        fprintf(stderr, "SHOP C5CBC FAIL %s value=%d\n", what, (int)value);
        exit(1);
    }
}

int main(void)
{
    static const u8 ids[] = {3, 9, 1, 8};
    unsigned i;

    memset(menu_storage, 0, sizeof(menu_storage));
    memset(output, 0xa5, sizeof(output));
    memset(work, 0x5a, sizeof(work));
    memset(bundle, 0, sizeof(bundle));
    *(void**)(menu_storage + 0x378) = bundle;
    *(void**)(menu_storage + 0x678) = work;
    g_Menu = (SystemMenu*)menu_storage;

    func_801C5CBC(output, (u8*)ids, 7, 4);

    check(string_calls == 4, "string call count", string_calls);
    check(render_calls == 4, "render call count", render_calls);
    check(shape_calls == 4, "shape call count", shape_calls);
    check(upload_calls == 2, "upload call count", upload_calls);
    check(rendered_indices[0] == 3 && rendered_indices[1] == 9 &&
          rendered_indices[2] == 1 && rendered_indices[3] == 8, "string IDs", 0);
    check(shaped_indices[0] == 0 && shaped_indices[1] == 1 &&
          shaped_indices[2] == 2 && shaped_indices[3] == 3, "shape indices", 0);
    for (i = 0; i < 4; i++) {
        check(shaped_offsets[i] == 7, "shape offset", shaped_offsets[i]);
    }
    check(*(u8*)(output + 0x82) == 0x23, "first width pair 0", output[0x82]);
    check(*(u8*)(output + 0x102) == 0x29, "second width pair 0", output[0x102]);
    check(*(u8*)(output + 0x182) == 0x21, "first width pair 1", output[0x182]);
    check(*(u8*)(output + 0x202) == 0x28, "second width pair 1", output[0x202]);
    puts("SHOP TEXT PAIR C5CBC certificate PASS checks=20");
    return 0;
}
