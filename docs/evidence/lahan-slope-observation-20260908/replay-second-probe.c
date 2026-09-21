#include "battle_mips_adapter.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
static uint8_t ram[0x200000],f[0x30000];static uint32_t gte[32];
static uint8_t* ptr(uint32_t a,unsigned w){if(a>=(uintptr_t)f&&(uint64_t)a+w<=(uintptr_t)f+sizeof(f))return(void*)(uintptr_t)a;if(a>=0x80000000&&(uint64_t)a+w<=0x80200000)return ram+(a&0x1fffff);return NULL;}
static int rd(void*u,uint32_t a,unsigned w,uint32_t*v){(void)u;uint8_t*p=ptr(a,w);if(!p)return-1;*v=0;for(unsigned i=0;i<w;i++)*v|=(uint32_t)p[i]<<(8*i);return 0;}
static int wr(void*u,uint32_t a,unsigned w,uint32_t v){(void)u;uint8_t*p=ptr(a,w);if(!p)return-1;for(unsigned i=0;i<w;i++)p[i]=v>>(8*i);return 0;}
static uint32_t cr(void*u,int c,unsigned r){(void)u;if(c)abort();return gte[r];}
static void cw(void*u,int c,unsigned r,uint32_t v){(void)u;if(c)abort();gte[r]=v;}
static int cmd(void*u,uint32_t ins){(void)u;if((ins&63)!=6)return-1;int64_t x[3],y[3];for(int i=0;i<3;i++){x[i]=(int16_t)gte[12+i];y[i]=(int16_t)(gte[12+i]>>16);}gte[24]=(uint32_t)(x[0]*y[1]+x[1]*y[2]+x[2]*y[0]-x[0]*y[2]-x[1]*y[0]-x[2]*y[1]);return 0;}
static void load(const char*p,uint8_t*d,size_t n){FILE*h=fopen(p,"rb");if(!h){perror(p);exit(2);}size_t k=fread(d,1,n,h);fclose(h);if(!k)exit(2);}
int main(void){
 FILE *slus=fopen("disc/SLUS_006.64","rb");if(!slus)return 2;fseek(slus,0x800,SEEK_SET);fread(ram+0x10000,1,sizeof(ram)-0x10000,slus);fclose(slus);
 load("disc/field.bin",ram+0x6faf0,sizeof(ram)-0x6faf0);
 load("scratchpad/lahan-natural-20260908-cd/walker-actor.bin",f,0x138);
 load("scratchpad/lahan-natural-20260907-arithmetic/field15-data/triangles.bin",f+0x1000,27104);
 load("scratchpad/lahan-natural-20260907-arithmetic/field15-data/vertices.bin",f+0x10000,8432);
 load("scratchpad/lahan-natural-20260908-cd/materials.bin",f+0x20000,1024);
 wr(0,0x800afb20,4,(uintptr_t)f+0x20000);wr(0,0x800afb24,4,(uintptr_t)f+0x1000);wr(0,0x800afb34,4,(uintptr_t)f+0x10000);wr(0,0x800b21cc,1,0);
 wr(0,(uintptr_t)f+0x300,4,0xfffd01b0);wr(0,(uintptr_t)f+0x304,4,0);wr(0,(uintptr_t)f+0x308,4,0xfffc1200);
 PcPortMipsBus bus={.read=rd,.write=wr,.cop2_read=cr,.cop2_write=cw,.cop2_command=cmd};PcPortMipsCpu c;PcPortMipsCpuInit(&c,&bus);
 c.gpr[4]=(uintptr_t)f+0x300;c.gpr[5]=(uintptr_t)f+0x20;c.gpr[6]=(uintptr_t)f;c.gpr[7]=(uintptr_t)f+0x310;c.gpr[29]=0x801ff000;c.gpr[31]=0xfffffffc;
 wr(0,c.gpr[29]+16,4,(uintptr_t)f+0x330);wr(0,c.gpr[29]+20,4,0xffffffff);wr(0,c.gpr[29]+24,4,(uintptr_t)f+0x370);
 int r=PcPortMipsRun(&c,0x8007bef4,0xfffffffc,10000);uint32_t flags;rd(0,(uintptr_t)f+0x370,4,&flags);
 printf("retail status=%d result=%08x flags=%08x steps=%llu error=%s\n",r,c.gpr[2],flags,(unsigned long long)c.steps,c.error);
 for(int i=0;i<8;i++){uint32_t v;rd(0,(uintptr_t)f+0x310+i*2,2,&v);printf("%04x ",v);}puts("");return r!=0;
}
