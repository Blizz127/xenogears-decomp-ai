/* Gate mechanism tests, not a playable-slice or native-coverage claim. */
#define _POSIX_C_SOURCE 200809L
#include "battle_mips_adapter.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned reads, writes;
static const uint32_t code[] = {0x24020007u, 0x03e00008u, 0u};
static int read_word(void *opaque, uint32_t address, unsigned width, uint32_t *value)
{
    (void)opaque;
    reads++;
    if (width != 4 || address < 0x1000u || address >= 0x100cu)
        return -1;
    *value = code[(address - 0x1000u) / 4u];
    return 0;
}
static int write_word(void *opaque, uint32_t address, unsigned width, uint32_t value)
{
    (void)opaque; (void)address; (void)width; (void)value;
    writes++;
    return -1;
}
static int native_call(void *opaque, PcPortMipsCpu *cpu, uint32_t target)
{
    (void)opaque;
    if (target != 0x2000u) return 0;
    cpu->gpr[2] = 17u;
    return 1;
}
static void init(PcPortMipsCpu *cpu, int native)
{
    PcPortMipsBus bus = {0};
    bus.read = read_word;
    bus.write = write_word;
    if (native) bus.bridge = native_call;
    PcPortMipsCpuInit(cpu, &bus);
    cpu->gpr[31] = 0xfffffffcu;
    reads = writes = 0;
}
int main(void)
{
    PcPortMipsCpu cpu;
    assert(unsetenv("XENO_DISABLE_CPU_INTERPRETER") == 0);
    init(&cpu, 0);
    assert(PcPortMipsRun(&cpu, 0x1000u, 0xfffffffcu, 20) == PC_PORT_MIPS_HALTED);
    assert(cpu.gpr[2] == 7 && cpu.steps == 3 && reads == 3);
    assert(setenv("XENO_DISABLE_CPU_INTERPRETER", "0", 1) == 0);
    init(&cpu, 0);
    assert(PcPortMipsRun(&cpu, 0x1000u, 0xfffffffcu, 20) == PC_PORT_MIPS_HALTED);
    assert(cpu.gpr[2] == 7 && cpu.steps == 3 && reads == 3);
    assert(setenv("XENO_DISABLE_CPU_INTERPRETER", "1", 1) == 0);
    init(&cpu, 0);
    cpu.gpr[2] = 123u;
    assert(PcPortMipsRun(&cpu, 0x1000u, 0xfffffffcu, 20) == PC_PORT_MIPS_INTERPRETER_DISABLED);
    assert(cpu.steps == 0 && reads == 0 && writes == 0 && cpu.gpr[2] == 123u);
    assert(strstr(cpu.error, "unconverted target 0x00001000") != NULL);
    init(&cpu, 1);
    assert(PcPortMipsRun(&cpu, 0x2000u, 0xfffffffcu, 20) == PC_PORT_MIPS_HALTED);
    assert(cpu.gpr[2] == 17u && cpu.steps == 0 && reads == 0 && writes == 0);
    init(&cpu, 1);
    cpu.gpr[31] = 0x1000u;
    assert(PcPortMipsRun(&cpu, 0x2000u, 0xfffffffcu, 20) == PC_PORT_MIPS_INTERPRETER_DISABLED);
    assert(cpu.gpr[2] == 17u && cpu.steps == 0 && reads == 0 && writes == 0);
    puts("CPU INTERPRETER DISABLED gate PASS; gameplay coverage not tested");
    return 0;
}
