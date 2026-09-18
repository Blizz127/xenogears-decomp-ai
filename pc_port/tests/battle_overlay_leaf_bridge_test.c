#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "psyq/libgpu.h"
#include "psyq/libgte.h"
#ifndef BATTLE_RUNTIME_SOURCE
#define BATTLE_RUNTIME_SOURCE "../src/battle_mips_runtime.c"
#endif
#include BATTLE_RUNTIME_SOURCE

uint8_t g_PsxRam[PSX_RAM_SIZE];
uint8_t g_PsxScratchpad[4096];
unsigned int MFC2(int reg) { (void)reg; abort(); }
unsigned int CFC2(int reg) { (void)reg; abort(); }
void MTC2(unsigned int v, int reg) { (void)v; (void)reg; abort(); }
void CTC2(unsigned int v, int reg) { (void)v; (void)reg; abort(); }
int doCOP2(int op) { (void)op; abort(); }
int g_GPUDisabledState;
unsigned g_PcPortPresentedFrames;
void ControllerPushState(void) {}
void ControllerPoll(void) {}
void PcPort_ServiceVblank(void) {}
void PcPort_PadVblankPump(void) {}
void PsyX_UpdateInput(void) {}
int PsyX_Sys_GetVBlankCount(void) { return 0; }
char PsyX_BeginScene(void) { return 1; }
void ClearSplits(void) { abort(); }
void DrawAllSplits(void) { abort(); }
void ParsePrimitivesLinkedList(u_long *p, int s) { (void)p; (void)s; abort(); }

__attribute__((used)) void func_80079934(uint32_t *pValue) { *pValue += 4; }
__attribute__((used)) void func_800A3484(int16_t *p) { *p = -1; }
__attribute__((used)) void func_800AEEEC(uint8_t *p) { *(int16_t *)(p + 0x98) = -1; }

#define WORD 0x80020100u
#define HALF 0x80020200u
#define REC  0x80020300u
#define STACK 0x801ff000u

static BattleMipsRuntime rt;
static PcPortMipsCpu cpu;
static unsigned checks;

#define CHECK(label, condition) do { checks++; if (!(condition)) { \
    fprintf(stderr, "BATTLE OVERLAY LEAF FAIL %s line=%d\n", label, __LINE__); \
    return 1; \
} } while (0)

static uint32_t word(uint32_t a)
{
    return load_le(PSX_ADDR(a), 4);
}

static uint16_t half(uint32_t a)
{
    return (uint16_t)load_le(PSX_ADDR(a), 2);
}

static void emit(uint32_t a, uint32_t w)
{
    store_le(PSX_ADDR(a), 4, w);
}

static void setup(void)
{
    memset(&rt, 0, sizeof(rt));
    memset(&cpu, 0, sizeof(cpu));
    memset(g_PsxRam, 0, sizeof(g_PsxRam));
    rt.initialized = 1;
    initialize_cpu(&cpu, &rt);
    cpu.gpr[29] = STACK;
    cpu.gpr[31] = BATTLE_HALT_PC;
    rt.bridge_cpu = &cpu;
    g_ActiveBattleRuntime = &rt;
    rt.function_count = 3;
    rt.functions[0].address = 0x80079934u;
    rt.functions[0].host = (void *)func_80079934;
    rt.functions[0].name = "func_80079934";
    rt.functions[1].address = 0x800a3484u;
    rt.functions[1].host = (void *)func_800A3484;
    rt.functions[1].name = "func_800A3484";
    rt.functions[2].address = 0x800aeeeCu;
    rt.functions[2].host = (void *)func_800AEEEC;
    rt.functions[2].name = "func_800AEEEC";
    /* Interpret path is a no-op: jr $ra / nop at each overlay address. */
    emit(0x80079934u, 0x03e00008u);
    emit(0x80079938u, 0);
    emit(0x800a3484u, 0x03e00008u);
    emit(0x800a3488u, 0);
    emit(0x800aeeeCu, 0x03e00008u);
    emit(0x800aeef0u, 0);
}

int main(void)
{
    int rc;
    volatile void *keep = (void *)func_80079934;
    keep = (void *)func_800A3484;
    keep = (void *)func_800AEEEC;
    (void)keep;

    setup();
    store_le(PSX_ADDR(WORD), 4, 10);
    cpu.gpr[4] = WORD;
    rc = runtime_bridge(&rt, &cpu, 0x80079934u);
    CHECK("79934 bridged", rc == 1);
    CHECK("79934 *p += 4", word(WORD) == 14);

    setup();
    store_le(PSX_ADDR(HALF), 2, 0x1234);
    cpu.gpr[4] = HALF;
    rc = runtime_bridge(&rt, &cpu, 0x800a3484u);
    CHECK("A3484 bridged", rc == 1);
    CHECK("A3484 *p = -1", half(HALF) == 0xffff);

    setup();
    store_le(PSX_ADDR(REC + 0x98), 2, 0x7777);
    cpu.gpr[4] = REC;
    rc = runtime_bridge(&rt, &cpu, 0x800aeeeCu);
    CHECK("AEEEC bridged", rc == 1);
    CHECK("AEEEC *(p+0x98) = -1", half(REC + 0x98) == 0xffff);

    /* A non-allowlisted overlay target still interprets (jr ra / nop). */
    setup();
    store_le(PSX_ADDR(WORD), 4, 10);
    cpu.gpr[4] = WORD;
    cpu.pc = 0x80071000u;
    rc = runtime_bridge(&rt, &cpu, 0x80071000u);
    CHECK("unadopted overlay interprets", rc == 0);
    CHECK("unadopted does not touch WORD", word(WORD) == 10);

    printf("BATTLE OVERLAY LEAF PASS checks=%u\n", checks);
    return 0;
}
