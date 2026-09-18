#include "battle_mips_adapter.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern void func_8008AA74(uint32_t);
uint32_t D_8005919C;
uint8_t D_800D366C;
static uint8_t ram[0x200000];
static uint16_t config[16];
static unsigned calls;
static uint32_t command;
static void fail(const char *why) { fprintf(stderr,"COMMAND SOUND FAIL %s\n",why); exit(1); }
void func_80039DB8(uint32_t value) { ++calls; command=value; }
static int read_bus(void *unused,uint32_t a,unsigned w,uint32_t *v) {
    (void)unused; a &= 0x1fffffff;
    if ((uint64_t)a+w>sizeof(ram)) return -1;
    *v=0;
    for(unsigned i=0;i<w;++i) *v|=(uint32_t)ram[a+i]<<(8*i);
    return 0;
}
static int write_bus(void *unused,uint32_t a,unsigned w,uint32_t v) {
    (void)unused; a &= 0x1fffffff;
    if ((uint64_t)a+w>sizeof(ram)) return -1;
    for(unsigned i=0;i<w;++i) ram[a+i]=(uint8_t)(v>>(8*i));
    return 0;
}
static int bridge(void *unused,PcPortMipsCpu *cpu,uint32_t target) {
    (void)unused;
    if(target!=0x80039db8) return 0;
    func_80039DB8(cpu->gpr[4]); return 1;
}
int main(void) {
    FILE *f=fopen("disc/battle.bin","rb");
    if(!f) fail("retail missing");
    size_t n=fread(ram+0x6faf0,1,0x60000,f);
    if(n<=0x1afac || !feof(f) || fclose(f)) fail("retail read");
    if((uintptr_t)config>UINT32_MAX) fail("fixture requires non-PIE low address");
    D_8005919C=(uint32_t)(uintptr_t)config;
    const uint32_t upper[]={0,0x100,0x7fffff00,0xffffff00};
    const uint16_t groups[]={0,1,0x8000,0xffff};
    const uint8_t gates[]={0,1,255};
    unsigned checks=0;
    for(unsigned a=0;a<4;++a) for(unsigned low=0;low<256;++low)
    for(unsigned g=0;g<4;++g) for(unsigned e=0;e<3;++e) {
        uint32_t arg=upper[a]|low;
        config[10]=groups[g]; D_800D366C=gates[e];
        write_bus(NULL,0x8005919c,4,0x80100000);
        write_bus(NULL,0x80100014,2,groups[g]);
        ram[0xd366c]=gates[e]; calls=0; command=0xa5a5a5a5;
        PcPortMipsBus bus={.read=read_bus,.write=write_bus,.bridge=bridge};
        PcPortMipsCpu cpu; PcPortMipsCpuInit(&cpu,&bus);
        cpu.gpr[4]=arg; cpu.gpr[29]=0x801ff000; cpu.gpr[31]=0xfffffffcu;
        if(PcPortMipsRun(&cpu,0x8008aa74,0xfffffffcu,1000)) fail(cpu.error);
        unsigned expected_calls=gates[e]!=0;
        uint32_t expected=expected_calls ? ((uint32_t)groups[g]<<16)|low : 0xa5a5a5a5;
        if(calls!=expected_calls || command!=expected) fail("retail contract");
        calls=0; command=0xa5a5a5a5;
        func_8008AA74(arg);
        if(calls!=expected_calls || command!=expected || D_800D366C!=gates[e] ||
           config[10]!=groups[g]) fail("native differs from retail");
        ++checks;
    }
    printf("COMMAND SOUND PASS %u full-word/gate/group cases\n",checks);
    return 0;
}
