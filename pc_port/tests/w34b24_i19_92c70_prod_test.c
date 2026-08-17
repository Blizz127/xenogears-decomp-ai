/*
 * Focused production-linked oracle for retail callback 0x80092C70.
 *
 * Expected values are hand-derived from [0x80092C70, 0x80092DD0).
 * NEXT_CALLBACK_TARGET is not an oracle input.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_callback_92c70.h"

#define POOL_GUEST   0x800D7538u
#define SLOT0        (POOL_GUEST)
#define SLOT11       (POOL_GUEST + 0x580u)
#define SLOT12       (POOL_GUEST + 0x600u)
#define SLOT13       (POOL_GUEST + 0x680u)
#define G_POOL       0x8009BE24u
#define G_SEL        0x8009BD24u
#define G_OBJ        0x8009D498u
#define G_TABLE      0x8009D784u
#define G_TABLE_ALT  0x8009D780u
#define G_DB         0x8009BE3Cu
#define G_CTX        0x8009D7F0u
#define DB_GUEST     0x8009BBC8u
#define OT_WORD      0x800F1000u
#define OT_ALT       0x800F2000u
#define TABLE_GUEST  0x800E2000u
#define TABLE_ALT    0x800E3000u
#define ENTRY_GUEST  0x800E2100u

enum {
    CALL_34614 = 1,
    CALL_33728 = 2,
    CALL_34714 = 3,
    CALL_34888 = 4
};

typedef struct CallRec {
    u32 which;
    u32 a0;
    u32 a1;
    u32 a2;
} CallRec;

static int s_failures;
static CallRec s_calls[16];
static int s_call_n;
static s16 s_inject_34614 = -1;
static s16 s_inject_33728 = -1;

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

static u16 rd16(u32 a)
{
    u16 v;

    memcpy(&v, PSX_ADDR(a), 2);
    return v;
}

static void wr32(u32 a, u32 v) { memcpy(PSX_ADDR(a), &v, 4); }
static void wr16(u32 a, u16 v) { memcpy(PSX_ADDR(a), &v, 2); }

static u32 host_pointer_as_guest(const void *pointer)
{
    uintptr_t base = (uintptr_t)(const void *)g_PsxRam;
    uintptr_t current = (uintptr_t)pointer;
    uintptr_t delta;

    if (current < base)
        return 0xFFFFFFFFu;
    delta = current - base;
    if (delta >= 0x00200000u)
        return 0xFFFFFFFFu;
    return 0x80000000u | (u32)delta;
}

static void rec(u32 which, u32 a0, u32 a1, u32 a2)
{
    if (s_call_n < 16) {
        s_calls[s_call_n].which = which;
        s_calls[s_call_n].a0 = a0;
        s_calls[s_call_n].a1 = a1;
        s_calls[s_call_n].a2 = a2;
    }
    s_call_n++;
}

void func_80034614(void *object)
{
    rec(CALL_34614, host_pointer_as_guest(object), 0, 0);
    if (s_inject_34614 != -1)
        wr16(G_SEL, (u16)s_inject_34614);
}

void *GetStringEntry(void *table, s32 index)
{
    rec(CALL_33728, host_pointer_as_guest(table), (u32)index, 0);
    if (s_inject_33728 != -1)
        wr16(G_SEL, (u16)s_inject_33728);
    return PSX_ADDR(ENTRY_GUEST);
}

void func_80034714(void *object, void *entry)
{
    rec(CALL_34714, host_pointer_as_guest(object),
        host_pointer_as_guest(entry), 0);
}

void func_80034888(void *object, void *ot, s32 render_context_index)
{
    rec(CALL_34888, host_pointer_as_guest(object), (u32)(uintptr_t)ot,
        (u32)render_context_index);
}

static int count_which(u32 which)
{
    int i;
    int n = 0;

    for (i = 0; i < s_call_n && i < 16; i++) {
        if (s_calls[i].which == which)
            n++;
    }
    return n;
}

static const CallRec *nth_which(u32 which, int nth)
{
    int i;
    int seen = 0;

    for (i = 0; i < s_call_n && i < 16; i++) {
        if (s_calls[i].which == which) {
            if (seen == nth)
                return &s_calls[i];
            seen++;
        }
    }
    return NULL;
}

static void reset_calls(void)
{
    memset(s_calls, 0, sizeof(s_calls));
    s_call_n = 0;
    s_inject_34614 = -1;
    s_inject_33728 = -1;
}

static void plant_common(void)
{
    wr32(G_POOL, POOL_GUEST);
    wr32(G_DB, DB_GUEST);
    wr32(DB_GUEST + 0x6Cu, OT_ALT);
    wr32(DB_GUEST + 0x70u, OT_WORD);
    wr32(G_CTX, 1u);
    wr32(G_TABLE, TABLE_GUEST);
    wr32(G_TABLE_ALT, TABLE_ALT);
    wr32(SLOT0, 0x11111111u);
    wr16(SLOT0 + 0x20u, 0x55u);
    wr32(SLOT0 + 0x50u, 0x22222222u);
    wr32(SLOT11, 0x5A5A5A5Au);
    wr32(SLOT13, 0xA5A5A5A5u);
    wr16(SLOT12 + 0x20u, 0);
    wr32(SLOT12 + 0x50u, 0xDEADBEEFu);
}

static s32 run12(void)
{
    return wm_80092C70(12);
}

int main(void)
{
    s32 r;
    const CallRec *c;
    u8 scratch_before[64];

    PsxMemory_Init();
    memcpy(scratch_before, g_PsxScratchpad, sizeof(scratch_before));

    /* state 0, selection -1: render only */
    reset_calls();
    plant_common();
    wr16(G_SEL, 0xFFFFu);
    wr16(SLOT12 + 0x20u, 0);
    r = run12();
    chk("s0idle.ret", (u32)r, 1u);
    chk("s0idle.34614", (u32)count_which(CALL_34614), 0u);
    chk("s0idle.33728", (u32)count_which(CALL_33728), 0u);
    chk("s0idle.34714", (u32)count_which(CALL_34714), 0u);
    chk("s0idle.34888", (u32)count_which(CALL_34888), 1u);
    chk("s0idle.state", (u32)rd16(SLOT12 + 0x20u), 0u);
    chk("s0idle.cache", rd32(SLOT12 + 0x50u), 0xDEADBEEFu);
    c = nth_which(CALL_34888, 0);
    chk("s0idle.34888.a0", c ? c->a0 : 0u, G_OBJ);
    chk("s0idle.34888.ot", c ? c->a1 : 0u, OT_WORD);
    chk("s0idle.34888.ctx", c ? c->a2 : 0u, 1u);
    chk("s0idle.iso11", rd32(SLOT11), 0x5A5A5A5Au);
    chk("s0idle.iso13", rd32(SLOT13), 0xA5A5A5A5u);
    chk("s0idle.iso0", rd32(SLOT0), 0x11111111u);

    /* state 0, valid selection: 34614 then fresh GetStringEntry/34714 */
    reset_calls();
    plant_common();
    wr16(G_SEL, 5);
    wr16(SLOT12 + 0x20u, 0);
    s_inject_34614 = 7;
    s_inject_33728 = 9;
    r = run12();
    chk("s0enq.ret", (u32)r, 1u);
    chk("s0enq.34614", (u32)count_which(CALL_34614), 1u);
    chk("s0enq.33728", (u32)count_which(CALL_33728), 1u);
    chk("s0enq.34714", (u32)count_which(CALL_34714), 1u);
    chk("s0enq.34888", (u32)count_which(CALL_34888), 1u);
    chk("s0enq.state", (u32)rd16(SLOT12 + 0x20u), 1u);
    chk("s0enq.cache", rd32(SLOT12 + 0x50u), 9u);
    chk("s0enq.slot0state", (u32)rd16(SLOT0 + 0x20u), 0x55u);
    c = nth_which(CALL_34614, 0);
    chk("s0enq.34614.a0", c ? c->a0 : 0u, G_OBJ);
    c = nth_which(CALL_33728, 0);
    chk("s0enq.33728.tbl", c ? c->a0 : 0u, TABLE_GUEST);
    chk("s0enq.33728.idx", c ? c->a1 : 0u, 7u);
    c = nth_which(CALL_34714, 0);
    chk("s0enq.34714.a0", c ? c->a0 : 0u, G_OBJ);
    chk("s0enq.34714.a1", c ? c->a1 : 0u, ENTRY_GUEST);
    c = nth_which(CALL_34888, 0);
    chk("s0enq.34888.a0", c ? c->a0 : 0u, G_OBJ);
    chk("s0enq.34888.ot", c ? c->a1 : 0u, OT_WORD);

    /* state 1, selection -1: reset and clear */
    reset_calls();
    plant_common();
    wr16(G_SEL, 0xFFFFu);
    wr16(SLOT12 + 0x20u, 1);
    wr32(SLOT12 + 0x50u, 4u);
    r = run12();
    chk("s1neg.ret", (u32)r, 1u);
    chk("s1neg.34614", (u32)count_which(CALL_34614), 1u);
    chk("s1neg.33728", (u32)count_which(CALL_33728), 0u);
    chk("s1neg.34714", (u32)count_which(CALL_34714), 0u);
    chk("s1neg.34888", (u32)count_which(CALL_34888), 1u);
    chk("s1neg.state", (u32)rd16(SLOT12 + 0x20u), 0u);
    chk("s1neg.cache", rd32(SLOT12 + 0x50u), 4u);

    /* state 1, selection equals cached word: render only */
    reset_calls();
    plant_common();
    wr16(G_SEL, 4);
    wr16(SLOT12 + 0x20u, 1);
    wr32(SLOT12 + 0x50u, 4u);
    r = run12();
    chk("s1same.ret", (u32)r, 1u);
    chk("s1same.34614", (u32)count_which(CALL_34614), 0u);
    chk("s1same.33728", (u32)count_which(CALL_33728), 0u);
    chk("s1same.state", (u32)rd16(SLOT12 + 0x20u), 1u);
    chk("s1same.cache", rd32(SLOT12 + 0x50u), 4u);
    chk("s1same.34888", (u32)count_which(CALL_34888), 1u);

    /* state 1, low 16 bits match but full word differs: re-enqueue */
    reset_calls();
    plant_common();
    wr16(G_SEL, 5);
    wr16(SLOT12 + 0x20u, 1);
    wr32(SLOT12 + 0x50u, 0x00010005u);
    s_inject_34614 = 8;
    s_inject_33728 = 11;
    r = run12();
    chk("s1chg.ret", (u32)r, 1u);
    chk("s1chg.34614", (u32)count_which(CALL_34614), 1u);
    chk("s1chg.33728", (u32)count_which(CALL_33728), 1u);
    chk("s1chg.34714", (u32)count_which(CALL_34714), 1u);
    chk("s1chg.state", (u32)rd16(SLOT12 + 0x20u), 1u);
    chk("s1chg.cache", rd32(SLOT12 + 0x50u), 11u);
    c = nth_which(CALL_33728, 0);
    chk("s1chg.33728.idx", c ? c->a1 : 0u, 8u);

    /* other internal state: render only */
    reset_calls();
    plant_common();
    wr16(G_SEL, 3);
    wr16(SLOT12 + 0x20u, 2);
    wr32(SLOT12 + 0x50u, 0xABCDu);
    r = run12();
    chk("s2.ret", (u32)r, 1u);
    chk("s2.34614", (u32)count_which(CALL_34614), 0u);
    chk("s2.33728", (u32)count_which(CALL_33728), 0u);
    chk("s2.state", (u32)rd16(SLOT12 + 0x20u), 2u);
    chk("s2.cache", rd32(SLOT12 + 0x50u), 0xABCDu);
    chk("s2.34888", (u32)count_which(CALL_34888), 1u);

    /* signed selection cache */
    reset_calls();
    plant_common();
    wr16(G_SEL, 0x8001u);
    wr16(SLOT12 + 0x20u, 0);
    r = run12();
    chk("s0neg.cache", rd32(SLOT12 + 0x50u), 0xFFFF8001u);
    chk("s0neg.state", (u32)rd16(SLOT12 + 0x20u), 1u);

    chk("scratch", memcmp(g_PsxScratchpad, scratch_before,
                          sizeof(scratch_before)) == 0 ? 1u : 0u, 1u);

    if (s_failures != 0) {
        fprintf(stderr, "%d assertion(s) failed\n", s_failures);
        return 1;
    }
    printf("W34B24-I19 0x80092C70 focused oracle PASS\n");
    return 0;
}
