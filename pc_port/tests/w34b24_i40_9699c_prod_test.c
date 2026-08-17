/*
 * Focused production-linked oracle for retail helper 0x8009699C.
 *
 * Expected values are hand-derived from [0x8009699C, 0x80096A6C).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_9699c.h"

#define REC    0x80090000u
#define CEBC   0x8009CEBCu
#define CD44   0x8009CD44u
#define D3BC   0x8009D3BCu
#define BE48   0x8009BE48u
#define CCB0   0x8009CCB0u
#define CCA8   0x8009CCA8u
#define CCA0   0x8009CCA0u
#define D7F4   0x8009D7F4u
#define D614   0x8009D614u
#define D56C   0x8009D56Cu
#define CEB8   0x8009CEB8u
#define C590   0x8009C590u
#define CANARY 0x8009B000u

static int s_failures;
static int s_pos_i;
static int s_pos_ok;
static uintptr_t s_cb;
static int s_ctl_com;
static int s_ctl_ok;

static void chk(const char *n, u32 got, u32 want)
{
    if (got != want) {
        fprintf(stderr, "ASSERTION %s: got=0x%08x expected=0x%08x\n", n, got,
                want);
        if (++s_failures > 40)
            exit(1);
    }
}

static u32 rd32(u32 a)
{
    u32 v;

    memcpy(&v, PSX_ADDR(a), 4);
    return v;
}

static void wr32(u32 a, u32 v) { memcpy(PSX_ADDR(a), &v, 4); }

void wm_9699c_test_store(u32 address, u32 value)
{
    (void)address;
    (void)value;
}

int CdIntToPos(int i, void *p)
{
    s_pos_i = i;
    s_pos_ok = (p == PSX_ADDR(CEBC));
    wr32(CEBC, 0xC0DECDu);
    return 0;
}

void *CdSyncCallback(void *func)
{
    s_cb = (uintptr_t)func;
    return NULL;
}

int CdControlF(unsigned char com, unsigned char *param)
{
    s_ctl_com = (int)com;
    s_ctl_ok = (param == (unsigned char *)PSX_ADDR(CEBC));
    return 1;
}

int main(void)
{
    PsxMemory_Init();
    wr32(CANARY, 0x11111111u);
    wr32(BE48, 0xAAu);
    wr32(CCB0, 0xBBu);
    wr32(CCA8, 0xCCu);
    wr32(CCA0, 0xDDu);
    wr32(REC, 0x1234u);
    wr32(REC + 4u, 0x1001u);
    wr32(REC + 8u, 0x80092000u);

    wm_8009699C(REC);

    chk("cd44", rd32(CD44), 1u);
    chk("d3bc", rd32(D3BC), REC + 12u);
    chk("be48", rd32(BE48), 0u);
    chk("ccb0", rd32(CCB0), 0u);
    chk("cca8", rd32(CCA8), 0u);
    chk("cca0", rd32(CCA0), 0u);
    chk("d7f4", rd32(D7F4), 0x1234u);
    chk("d614", rd32(D614), 0x1234u);
    chk("d56c", rd32(D56C), (0x1001u + 0x7ffu) >> 11);
    chk("ceb8", rd32(CEB8), 0x1001u);
    chk("c590", rd32(C590), 0x80092000u);
    chk("pos.i", (u32)s_pos_i, 0x1234u);
    chk("pos.ok", (u32)s_pos_ok, 1u);
    chk("cebc", rd32(CEBC), 0xC0DECDu);
    chk("cb", (u32)s_cb, 0x80096A6Cu);
    chk("ctl.com", (u32)s_ctl_com, 2u);
    chk("ctl.ok", (u32)s_ctl_ok, 1u);
    chk("canary", rd32(CANARY), 0x11111111u);

    if (s_failures) {
        fprintf(stderr, "ASSERTION failures=%d\n", s_failures);
        return 1;
    }
    printf("W34B24-I40 0x8009699C focused oracle PASS\n");
    return 0;
}
