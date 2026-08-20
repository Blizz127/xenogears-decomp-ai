#include "common.h"
#include "psyq/libgte.h"
#include "psyq/inline_c.h"

void InitGeom(void) {
    __asm__ volatile(
        ".set noat\n\t"
        "lui $1, %%hi(D_800569F0)\n\t"
        "sw $31, %%lo(D_800569F0)($1)\n\t"
        "jal func_8004B4AC\n\t"
        "lui $31, %%hi(D_800569F0)\n\t"
        "lw $31, %%lo(D_800569F0)($31)\n\t"
        "nop\n\t"
        ".word 0x40026000\n\t"
        "lui $3, 0x4000\n\t"
        "or $2, $2, $3\n\t"
        ".word 0x40826000\n\t"
        "nop\n\t"
        "addiu $8, $0, 0x155\n\t"
        ".word 0x48c8e800\n\t"
        "nop\n\t"
        "addiu $8, $0, 0x100\n\t"
        ".word 0x48c8f000\n\t"
        "nop\n\t"
        "addiu $8, $0, 0x3e8\n\t"
        ".word 0x48c8d000\n\t"
        "nop\n\t"
        "addiu $8, $0, -0x1062\n\t"
        ".word 0x48c8d800\n\t"
        "nop\n\t"
        "lui $8, 0x140\n\t"
        ".word 0x48c8e000\n\t"
        "nop\n\t"
        ".word 0x48c0c000\n\t"
        ".word 0x48c0c800\n\t"
        "nop\n\t"
        ".set at"
        : : : "$2", "$3", "$8", "memory");
}

__asm__(
        ".globl SquareRoot0\n\t"
        ".ent SquareRoot0\n\t"
        "SquareRoot0:\n\t"
        ".set noat\n\t"
        ".word 0x4884f000\n\t"
        ".word 0x00000000\n\t"
        ".word 0x00000000\n\t"
        ".word 0x4802f800\n\t"
        ".word 0x24010020\n\t"
        ".word 0x10410019\n\t"
        ".word 0x00000000\n\t"
        ".word 0x30480001\n\t"
        ".word 0x240afffe\n\t"
        ".word 0x004a5024\n\t"
        ".word 0x2409001f\n\t"
        ".word 0x012a4822\n\t"
        ".word 0x00094843\n\t"
        ".word 0x214bffe8\n\t"
        ".word 0x05600003\n\t"
        ".word 0x00000000\n\t"
        ".word 0x01646004\n\t"
        ".word 0x10000003\n\t"
        ".word 0x240b0018\n\t"
        ".word 0x016a5822\n\t"
        ".word 0x01646007\n\t"
        ".word 0x218cffc0\n\t"
        ".word 0x000c6040\n\t"
        "lui $13, %hi(D_80056A00)\n\t"
        ".word 0x01ac6821\n\t"
        "lh $13, %lo(D_80056A00)($13)\n\t"
        ".word 0x00000000\n\t"
        ".word 0x012d6804\n\t"
        ".word 0x000d1302\n\t"
        ".word 0x03e00008\n\t"
        ".word 0x00000000\n\t"
        ".word 0x03e00008\n\t"
        ".word 0x24020000\n\t"
        ".set at\n\t"
        ".end SquareRoot0");

__asm__(
        ".globl InvSquareRoot\n\t"
        ".ent InvSquareRoot\n\t"
        "InvSquareRoot:\n\t"
        ".set noat\n\t"
        ".word 0x4884f000\n\t"
        ".word 0x00000000\n\t"
        ".word 0x00000000\n\t"
        ".word 0x4802f800\n\t"
        ".word 0x24010020\n\t"
        ".word 0x1041001b\n\t"
        ".word 0x00000000\n\t"
        ".word 0x10400019\n\t"
        ".word 0x00000000\n\t"
        ".word 0x30480001\n\t"
        ".word 0x240afffe\n\t"
        ".word 0x004a5024\n\t"
        ".word 0x2409001f\n\t"
        ".word 0x012a4822\n\t"
        ".word 0x00094843\n\t"
        ".word 0x214bffe8\n\t"
        ".word 0x05600003\n\t"
        ".word 0x00000000\n\t"
        ".word 0x01646004\n\t"
        ".word 0x10000003\n\t"
        ".word 0x240b0018\n\t"
        ".word 0x016a5822\n\t"
        ".word 0x01646007\n\t"
        ".word 0x218cffc0\n\t"
        ".word 0x000c6040\n\t"
        "lui $13, %hi(D_80056B94)\n\t"
        ".word 0x01ac6821\n\t"
        "lh $13, %lo(D_80056B94)($13)\n\t"
        ".word 0xacc90000\n\t"
        ".word 0xacad0000\n\t"
        ".word 0x24020001\n\t"
        ".word 0x03e00008\n\t"
        ".word 0x00000000\n\t"
        ".word 0x03e00008\n\t"
        ".word 0x2402ffff\n\t"
        ".set at\n\t"
        ".end InvSquareRoot");

__asm__(
        ".globl VectorNormalS\n\t"
        ".ent VectorNormalS\n\t"
        "VectorNormalS:\n\t"
        ".word 0x8c880000\n\t"
        ".word 0x8c890004\n\t"
        ".word 0x8c8a0008\n\t"
        ".word 0x10000011\n\t"
        ".word 0x03e03821\n\t"
        ".end VectorNormalS");

__asm__(
        ".globl VectorNormal\n\t"
        ".ent VectorNormal\n\t"
        "VectorNormal:\n\t"
        ".word 0x8c880000\n\t"
        ".word 0x8c890004\n\t"
        ".word 0x8c8a0008\n\t"
        ".word 0x03e03821\n\t"
        ".reloc ., R_MIPS_26, func_80048DD8\n\t"
        ".word 0x0c000000\n\t"
        ".word 0x00000000\n\t"
        ".word 0x00e0f821\n\t"
        ".word 0xaca80000\n\t"
        ".word 0xaca90004\n\t"
        ".word 0x03e00008\n\t"
        ".word 0xacaa0008\n\t"
        ".end VectorNormal");

__asm__(
        ".globl VectorNormalSS\n\t"
        ".ent VectorNormalSS\n\t"
        "VectorNormalSS:\n\t"
        ".word 0x84880000\n\t"
        ".word 0x84890002\n\t"
        ".word 0x848a0004\n\t"
        ".word 0x00000000\n\t"
        ".word 0x03e03821\n\t"
        ".reloc ., R_MIPS_26, func_80048DD8\n\t"
        ".word 0x0c000000\n\t"
        ".word 0x00000000\n\t"
        ".word 0x00e0f821\n\t"
        ".word 0xa4a80000\n\t"
        ".word 0xa4a90002\n\t"
        ".word 0x03e00008\n\t"
        ".word 0xa4aa0004\n\t"
        ".end VectorNormalSS");

__asm__(
        ".globl func_80048DD8\n\t"
        ".ent func_80048DD8\n\t"
        "func_80048DD8:\n\t"
        ".set noat\n\t"
        ".word 0x48884800\n\t"
        ".word 0x48895000\n\t"
        ".word 0x488a5800\n\t"
        ".word 0x00000000\n\t"
        ".word 0x4aa00428\n\t"
        ".word 0x480bc800\n\t"
        ".word 0x480cd000\n\t"
        ".word 0x480dd800\n\t"
        ".word 0x016c5820\n\t"
        ".word 0x016d1020\n\t"
        ".word 0x4882f000\n\t"
        ".word 0x00000000\n\t"
        ".word 0x00000000\n\t"
        ".word 0x4803f800\n\t"
        ".word 0x2401fffe\n\t"
        ".word 0x00611824\n\t"
        ".word 0x240e001f\n\t"
        ".word 0x01c37022\n\t"
        ".word 0x206bffe8\n\t"
        ".word 0x05600003\n\t"
        ".word 0x000e7043\n\t"
        ".word 0x10000004\n\t"
        ".word 0x01626004\n\t"
        ".word 0x240b0018\n\t"
        ".word 0x01635822\n\t"
        ".word 0x01626007\n\t"
        ".word 0x218cffc0\n\t"
        ".word 0x000c6040\n\t"
        "lui $13, %hi(D_80056B94)\n\t"
        ".word 0x01ac6821\n\t"
        "lh $13, %lo(D_80056B94)($13)\n\t"
        ".word 0x00000000\n\t"
        ".word 0x488d4000\n\t"
        ".word 0x48884800\n\t"
        ".word 0x48895000\n\t"
        ".word 0x488a5800\n\t"
        ".word 0x00000000\n\t"
        ".word 0x00000000\n\t"
        ".word 0x4b90003d\n\t"
        ".word 0x4808c800\n\t"
        ".word 0x4809d000\n\t"
        ".word 0x480ad800\n\t"
        ".word 0x01c84007\n\t"
        ".word 0x01c94807\n\t"
        ".word 0x01ca5007\n\t"
        ".word 0x03e00008\n\t"
        ".word 0x00000000\n\t"
        ".set at\n\t"
        ".end func_80048DD8");

__asm__(
        ".globl func_80048E94\n\t"
        ".ent func_80048E94\n\t"
        "func_80048E94:\n\t"
        ".word 0x84880000\n\t"
        ".word 0x84890002\n\t"
        ".word 0x848a0004\n\t"
        ".word 0x848b0006\n\t"
        ".word 0x848c0008\n\t"
        ".word 0x848d000a\n\t"
        ".word 0x48420000\n\t"
        ".word 0x48431000\n\t"
        ".word 0x48462000\n\t"
        ".word 0x48c80000\n\t"
        ".word 0x48c91000\n\t"
        ".word 0x48ca2000\n\t"
        ".word 0x488d5800\n\t"
        ".word 0x488b4800\n\t"
        ".word 0x488c5000\n\t"
        ".word 0x00000000\n\t"
        ".word 0x4b78000c\n\t"
        ".word 0x480fc800\n\t"
        ".word 0x4818d000\n\t"
        ".word 0x4819d800\n\t"
        ".word 0x48cb0000\n\t"
        ".word 0x48cc1000\n\t"
        ".word 0x48cd2000\n\t"
        ".word 0x00000000\n\t"
        ".word 0x4b78000c\n\t"
        ".word 0x488b0000\n\t"
        ".word 0x488c0800\n\t"
        ".word 0x488d1000\n\t"
        ".word 0x4808c800\n\t"
        ".word 0x4809d000\n\t"
        ".word 0x480ad800\n\t"
        ".word 0x48c20000\n\t"
        ".word 0x48c31000\n\t"
        ".word 0x48c62000\n\t"
        ".word 0x03e03821\n\t"
        ".reloc ., R_MIPS_26, func_80048DD8\n\t"
        ".word 0x0c000000\n\t"
        ".word 0x00000000\n\t"
        ".word 0xa4a80000\n\t"
        ".word 0xa4a90002\n\t"
        ".word 0xa4aa0004\n\t"
        ".word 0x48080000\n\t"
        ".word 0x48090800\n\t"
        ".word 0x480a1000\n\t"
        ".reloc ., R_MIPS_26, func_80048DD8\n\t"
        ".word 0x0c000000\n\t"
        ".word 0x00000000\n\t"
        ".word 0xa4a80006\n\t"
        ".word 0xa4a90008\n\t"
        ".word 0xa4aa000a\n\t"
        ".word 0x01e04021\n\t"
        ".word 0x03004821\n\t"
        ".reloc ., R_MIPS_26, func_80048DD8\n\t"
        ".word 0x0c000000\n\t"
        ".word 0x03205021\n\t"
        ".word 0x00e0f821\n\t"
        ".word 0xa4a8000c\n\t"
        ".word 0xa4a9000e\n\t"
        ".word 0x03e00008\n\t"
        ".word 0xa4aa0010\n\t"
        ".end func_80048E94");

void LoadAverage12(VECTOR* v0, VECTOR* v1, long p0, long p1, VECTOR* output) {
    __asm__ volatile(
        ".word 0x48864000\n\t"
        ".word 0xc8890000\n\t"
        ".word 0xc88a0004\n\t"
        ".word 0xc88b0008\n\t"
        ".word 0x00000000\n\t"
        ".word 0x4b98003d\n\t"
        ".word 0x4802f800\n\t"
        ".word 0x48874000\n\t"
        ".word 0xc8a90000\n\t"
        ".word 0xc8aa0004\n\t"
        ".word 0xc8ab0008\n\t"
        ".word 0x00000000\n\t"
        ".word 0x4ba8003e\n\t"
        ".word 0x8fa80010\n\t"
        ".word 0x00000000\n\t"
        ".word 0xe9090000\n\t"
        ".word 0xe90a0004\n\t"
        ".word 0xe90b0008"
        : : : "$2", "$8", "memory");
}

void LoadAverage0(VECTOR* v0, VECTOR* v1, long p0, long p1, VECTOR* output) {
    __asm__ volatile(
        ".word 0x48864000\n\t"
        ".word 0xc8890000\n\t"
        ".word 0xc88a0004\n\t"
        ".word 0xc88b0008\n\t"
        ".word 0x00000000\n\t"
        ".word 0x4b90003d\n\t"
        ".word 0x4802f800\n\t"
        ".word 0x48874000\n\t"
        ".word 0xc8a90000\n\t"
        ".word 0xc8aa0004\n\t"
        ".word 0xc8ab0008\n\t"
        ".word 0x00000000\n\t"
        ".word 0x4ba0003e\n\t"
        ".word 0x8fa80010\n\t"
        ".word 0x00000000\n\t"
        ".word 0xe9090000\n\t"
        ".word 0xe90a0004\n\t"
        ".word 0xe90b0008"
        : : : "$2", "$8", "memory");
}

__asm__(
        ".globl func_8004901C\n\t"
        ".ent func_8004901C\n\t"
        "func_8004901C:\n\t"
        ".word 0x8c880000\n\t"
        ".word 0x8c8a0004\n\t"
        ".word 0x00084c03\n\t"
        ".word 0x3108ffff\n\t"
        ".word 0x314affff\n\t"
        ".word 0x48864000\n\t"
        ".word 0x48884800\n\t"
        ".word 0x48895000\n\t"
        ".word 0x488a5800\n\t"
        ".word 0x00000000\n\t"
        ".word 0x4b98003d\n\t"
        ".word 0x8ca80000\n\t"
        ".word 0x8caa0004\n\t"
        ".word 0x00084c03\n\t"
        ".word 0x3108ffff\n\t"
        ".word 0x314affff\n\t"
        ".word 0x4802f800\n\t"
        ".word 0x48874000\n\t"
        ".word 0x48884800\n\t"
        ".word 0x48895000\n\t"
        ".word 0x488a5800\n\t"
        ".word 0x00000000\n\t"
        ".word 0x4ba8003e\n\t"
        ".word 0x48084800\n\t"
        ".word 0x48095000\n\t"
        ".word 0x3108ffff\n\t"
        ".word 0x00094c00\n\t"
        ".word 0x01094025\n\t"
        ".word 0x8fad0010\n\t"
        ".word 0x480a5800\n\t"
        ".word 0xada80000\n\t"
        ".word 0xadaa0004\n\t"
        ".word 0x03e00008\n\t"
        ".word 0x00000000\n\t"
        ".end func_8004901C");

__asm__(
        ".globl func_800490A4\n\t"
        ".ent func_800490A4\n\t"
        "func_800490A4:\n\t"
        ".word 0x8c880000\n\t"
        ".word 0x8c8a0004\n\t"
        ".word 0x00084c03\n\t"
        ".word 0x3108ffff\n\t"
        ".word 0x314affff\n\t"
        ".word 0x48864000\n\t"
        ".word 0x48884800\n\t"
        ".word 0x48895000\n\t"
        ".word 0x488a5800\n\t"
        ".word 0x00000000\n\t"
        ".word 0x4b90003d\n\t"
        ".word 0x8ca80000\n\t"
        ".word 0x8caa0004\n\t"
        ".word 0x00084c03\n\t"
        ".word 0x3108ffff\n\t"
        ".word 0x314affff\n\t"
        ".word 0x4802f800\n\t"
        ".word 0x48874000\n\t"
        ".word 0x48884800\n\t"
        ".word 0x48895000\n\t"
        ".word 0x488a5800\n\t"
        ".word 0x00000000\n\t"
        ".word 0x4ba0003e\n\t"
        ".word 0x48084800\n\t"
        ".word 0x48095000\n\t"
        ".word 0x3108ffff\n\t"
        ".word 0x00094c00\n\t"
        ".word 0x01094025\n\t"
        ".word 0x8fad0010\n\t"
        ".word 0x480a5800\n\t"
        ".word 0xada80000\n\t"
        ".word 0xadaa0004\n\t"
        ".word 0x03e00008\n\t"
        ".word 0x00000000\n\t"
        ".end func_800490A4");

void LoadAverageByte(u_char* v0, u_char* v1, long p0, long p1,
        u_char* output) {
    __asm__ volatile(
        ".word 0x90880000\n\t"
        ".word 0x90890001\n\t"
        ".word 0x48864000\n\t"
        ".word 0x48884800\n\t"
        ".word 0x48895000\n\t"
        ".word 0x00000000\n\t"
        ".word 0x4b90003d\n\t"
        ".word 0x90a80000\n\t"
        ".word 0x90a90001\n\t"
        ".word 0x4802f800\n\t"
        ".word 0x48874000\n\t"
        ".word 0x48884800\n\t"
        ".word 0x48895000\n\t"
        ".word 0x240b000c\n\t"
        ".word 0x4ba0003e\n\t"
        ".word 0x8fad0010\n\t"
        ".word 0x4808c800\n\t"
        ".word 0x4809d000\n\t"
        ".word 0x01684007\n\t"
        ".word 0x01694807\n\t"
        ".word 0xa1a80000\n\t"
        ".word 0xa1a90001"
        : : : "$2", "$8", "$9", "$11", "$13", "memory");
}

void LoadAverageCol(u_char* v0, u_char* v1, long p0, long p1,
        u_char* output) {
    __asm__ volatile(
        ".word 0x90880000\n\t"
        ".word 0x90890001\n\t"
        ".word 0x908a0002\n\t"
        ".word 0x48864000\n\t"
        ".word 0x48884800\n\t"
        ".word 0x48895000\n\t"
        ".word 0x488a5800\n\t"
        ".word 0x00000000\n\t"
        ".word 0x4b90003d\n\t"
        ".word 0x90a80000\n\t"
        ".word 0x90a90001\n\t"
        ".word 0x90aa0002\n\t"
        ".word 0x4802f800\n\t"
        ".word 0x48874000\n\t"
        ".word 0x48884800\n\t"
        ".word 0x48895000\n\t"
        ".word 0x488a5800\n\t"
        ".word 0x240b000c\n\t"
        ".word 0x4ba0003e\n\t"
        ".word 0x8fad0010\n\t"
        ".word 0x4808c800\n\t"
        ".word 0x4809d000\n\t"
        ".word 0x480ad800\n\t"
        ".word 0x01684007\n\t"
        ".word 0x01694807\n\t"
        ".word 0x016a5007\n\t"
        ".word 0xa1a80000\n\t"
        ".word 0xa1a90001\n\t"
        ".word 0xa1aa0002"
        : : : "$2", "$8", "$9", "$10", "$11", "$13", "memory");
}

MATRIX* MulMatrix0(MATRIX* m0, MATRIX* m1, MATRIX* m2) {
    MATRIX* result;

    /* Handwritten PsyQ routine.  Preserve the retail column-load and result
     * interleave so the packed 3x3 matrix lands at the original offsets. */
    __asm__ volatile(
        ".set noat\n\t"
        "lw $8, 0(%1)\n\t"
        "lw $9, 4(%1)\n\t"
        "lw $10, 8(%1)\n\t"
        "lw $11, 12(%1)\n\t"
        "lw $12, 16(%1)\n\t"
        "ctc2 $8, $0\n\t"
        "ctc2 $9, $1\n\t"
        "ctc2 $10, $2\n\t"
        "ctc2 $11, $3\n\t"
        "ctc2 $12, $4\n\t"
        "lhu $8, 0(%2)\n\t"
        "lw $9, 4(%2)\n\t"
        "lw $10, 12(%2)\n\t"
        "lui $1, 0xffff\n\t"
        "and $9, $9, $1\n\t"
        "or $8, $8, $9\n\t"
        "mtc2 $8, $0\n\t"
        "mtc2 $10, $1\n\t"
        "nop\n\t"
        ".word 0x4a486012\n\t"
        "lhu $8, 2(%2)\n\t"
        "lw $9, 8(%2)\n\t"
        "lh $10, 14(%2)\n\t"
        "sll $9, $9, 16\n\t"
        "or $8, $8, $9\n\t"
        "mfc2 $11, $9\n\t"
        "mfc2 $12, $10\n\t"
        "mfc2 $13, $11\n\t"
        "mtc2 $8, $0\n\t"
        "mtc2 $10, $1\n\t"
        "nop\n\t"
        ".word 0x4a486012\n\t"
        "lhu $8, 4(%2)\n\t"
        "lw $9, 8(%2)\n\t"
        "lw $10, 16(%2)\n\t"
        "lui $1, 0xffff\n\t"
        "and $9, $9, $1\n\t"
        "or $8, $8, $9\n\t"
        "mfc2 $14, $9\n\t"
        "mfc2 $15, $10\n\t"
        "mfc2 $24, $11\n\t"
        "mtc2 $8, $0\n\t"
        "mtc2 $10, $1\n\t"
        "nop\n\t"
        ".word 0x4a486012\n\t"
        "andi $11, $11, 0xffff\n\t"
        "sll $14, $14, 16\n\t"
        "or $14, $14, $11\n\t"
        "sw $14, 0(%3)\n\t"
        "andi $13, $13, 0xffff\n\t"
        "sll $24, $24, 16\n\t"
        "or $24, $24, $13\n\t"
        "sw $24, 12(%3)\n\t"
        "mfc2 $8, $9\n\t"
        "mfc2 $9, $10\n\t"
        "andi $8, $8, 0xffff\n\t"
        "sll $12, $12, 16\n\t"
        "or $8, $8, $12\n\t"
        "sw $8, 4(%3)\n\t"
        "andi $15, $15, 0xffff\n\t"
        "sll $9, $9, 16\n\t"
        "or $9, $9, $15\n\t"
        "sw $9, 8(%3)\n\t"
        "swc2 $11, 16(%3)\n\t"
        "move %0, %3\n\t"
        ".set at"
        : "=r"(result)
        : "r"(m0), "r"(m1), "r"(m2)
        : "$8", "$9", "$10", "$11", "$12", "$13", "$14", "$15",
          "$24", "memory");
    return result;
}

/* Retail padding at 0x80049318, outside MulMatrix0's symbol bounds. */
__asm__(".word 0");

MATRIX* CompMatrix(MATRIX* m0, MATRIX* m1, MATRIX* m2) {
    MATRIX* result;

    /* Handwritten PsyQ routine.  Keep the retail load/store interleave: in
     * particular, m0->t is read before m2->t is overwritten so m0 == m2 is
     * safe. */
    __asm__ volatile(
        ".set noat\n\t"
        "lw $8, 0(%1)\n\t"
        "lw $9, 4(%1)\n\t"
        "lw $10, 8(%1)\n\t"
        "lw $11, 12(%1)\n\t"
        "lw $12, 16(%1)\n\t"
        "ctc2 $8, $0\n\t"
        "ctc2 $9, $1\n\t"
        "ctc2 $10, $2\n\t"
        "ctc2 $11, $3\n\t"
        "ctc2 $12, $4\n\t"
        "lhu $8, 0(%2)\n\t"
        "lw $9, 4(%2)\n\t"
        "lw $10, 12(%2)\n\t"
        "lui $1, 0xffff\n\t"
        "and $9, $9, $1\n\t"
        "or $8, $8, $9\n\t"
        "mtc2 $8, $0\n\t"
        "mtc2 $10, $1\n\t"
        "nop\n\t"
        ".word 0x4a486012\n\t"
        "lhu $8, 2(%2)\n\t"
        "lw $9, 8(%2)\n\t"
        "lh $10, 14(%2)\n\t"
        "sll $9, $9, 16\n\t"
        "or $8, $8, $9\n\t"
        "mfc2 $11, $9\n\t"
        "mfc2 $12, $10\n\t"
        "mfc2 $13, $11\n\t"
        "mtc2 $8, $0\n\t"
        "mtc2 $10, $1\n\t"
        "nop\n\t"
        ".word 0x4a486012\n\t"
        "lhu $8, 4(%2)\n\t"
        "lw $9, 8(%2)\n\t"
        "lw $10, 16(%2)\n\t"
        "lui $1, 0xffff\n\t"
        "and $9, $9, $1\n\t"
        "or $8, $8, $9\n\t"
        "mfc2 $14, $9\n\t"
        "mfc2 $15, $10\n\t"
        "mfc2 $24, $11\n\t"
        "mtc2 $8, $0\n\t"
        "mtc2 $10, $1\n\t"
        "nop\n\t"
        ".word 0x4a486012\n\t"
        "andi $11, $11, 0xffff\n\t"
        "sll $14, $14, 16\n\t"
        "or $14, $14, $11\n\t"
        "sw $14, 0(%3)\n\t"
        "andi $13, $13, 0xffff\n\t"
        "sll $24, $24, 16\n\t"
        "or $24, $24, $13\n\t"
        "sw $24, 12(%3)\n\t"
        "mfc2 $8, $9\n\t"
        "mfc2 $9, $10\n\t"
        "swc2 $11, 16(%3)\n\t"
        "lhu $13, 20(%2)\n\t"
        "lw $14, 24(%2)\n\t"
        "lw $10, 28(%2)\n\t"
        "sll $14, $14, 16\n\t"
        "or $13, $13, $14\n\t"
        "mtc2 $13, $0\n\t"
        "mtc2 $10, $1\n\t"
        "nop\n\t"
        ".word 0x4a486012\n\t"
        "sll $12, $12, 16\n\t"
        "andi $8, $8, 0xffff\n\t"
        "or $8, $8, $12\n\t"
        "sw $8, 4(%3)\n\t"
        "andi $15, $15, 0xffff\n\t"
        "sll $9, $9, 16\n\t"
        "or $9, $9, $15\n\t"
        "sw $9, 8(%3)\n\t"
        "mfc2 $8, $25\n\t"
        "mfc2 $9, $26\n\t"
        "mfc2 $10, $27\n\t"
        "lw $11, 20(%1)\n\t"
        "lw $12, 24(%1)\n\t"
        "lw $13, 28(%1)\n\t"
        "add $8, $8, $11\n\t"
        "add $9, $9, $12\n\t"
        "add $10, $10, $13\n\t"
        "sw $8, 20(%3)\n\t"
        "sw $9, 24(%3)\n\t"
        "sw $10, 28(%3)\n\t"
        "move %0, %3\n\t"
        ".set at"
        : "=r"(result)
        : "r"(m0), "r"(m1), "r"(m2)
        : "$8", "$9", "$10", "$11", "$12", "$13", "$14", "$15",
          "$24", "memory");
    return result;
}

VECTOR* ApplyMatrixLV(MATRIX* m, VECTOR* v0, VECTOR* v1) {
    VECTOR hi;
    VECTOR lo;

    gte_SetRotMatrix(m);
    hi = *v0;

    if (hi.vx < 0) {
        lo.vx = -(-hi.vx >> 15);
        hi.vx = -(-hi.vx & 0x7FFF);
    } else {
        lo.vx = hi.vx >> 15;
        hi.vx &= 0x7FFF;
    }
    if (hi.vy < 0) {
        lo.vy = -(-hi.vy >> 15);
        hi.vy = -(-hi.vy & 0x7FFF);
    } else {
        lo.vy = hi.vy >> 15;
        hi.vy &= 0x7FFF;
    }
    if (hi.vz < 0) {
        lo.vz = -(-hi.vz >> 15);
        hi.vz = -(-hi.vz & 0x7FFF);
    } else {
        lo.vz = hi.vz >> 15;
        hi.vz &= 0x7FFF;
    }

    gte_ldlvl(&lo);
    gte_rtir_sf0();
    gte_stlvnl(&lo);
    gte_ldlvl(&hi);
    gte_rtir();

    if (lo.vx < 0) lo.vx *= 8; else lo.vx <<= 3;
    if (lo.vy < 0) lo.vy *= 8; else lo.vy <<= 3;
    if (lo.vz < 0) lo.vz *= 8; else lo.vz <<= 3;
    gte_stlvnl(&hi);

    v1->vx = hi.vx + lo.vx;
    v1->vy = hi.vy + lo.vy;
    v1->vz = hi.vz + lo.vz;
    return v1;
}

VECTOR* ApplyRotMatrix(SVECTOR* input, VECTOR* output) {
    register VECTOR* result asm("$2");
    __asm__ volatile(
        ".word 0x8c880000\n\t"
        ".word 0x8c890004\n\t"
        ".word 0x48880000\n\t"
        ".word 0x48890800\n\t"
        ".word 0x00000000\n\t"
        ".word 0x4a486012\n\t"
        ".word 0xe8a90000\n\t"
        ".word 0xe8aa0004\n\t"
        ".word 0xe8ab0008\n\t"
        ".word 0x00c01021"
        : "=r"(result)
        : "r"(input), "r"(output)
        : "$8", "$9", "memory");
    return result;
}

void PushMatrix(void) {
    __asm__ volatile(
        ".set noat\n\t"
        "lui $14, %%hi(D_80056D2C)\n\t"
        "lw $14, %%lo(D_80056D2C)($14)\n\t"
        ".word 0x00000000\n\t"
        ".word 0x29c10280\n\t"
        ".word 0x1420000a\n\t"
        "lui $1, %%hi(D_80056D20)\n\t"
        "sw $31, %%lo(D_80056D20)($1)\n\t"
        "lui $4, %%hi(D_80056FB0)\n\t"
        ".reloc ., R_MIPS_26, printf\n\t"
        ".word 0x0c000000\n\t"
        "addiu $4, $4, %%lo(D_80056FB0)\n\t"
        "lui $31, %%hi(D_80056D20)\n\t"
        "lw $31, %%lo(D_80056D20)($31)\n\t"
        ".word 0x00000000\n\t"
        ".word 0x03e00008\n\t"
        ".word 0x00000000\n\t"
        "lui $15, %%hi(D_80056D30)\n\t"
        ".word 0x01ee7821\n\t"
        "addiu $15, $15, %%lo(D_80056D30)\n\t"
        ".word 0x48480000\n\t"
        ".word 0x48490800\n\t"
        ".word 0xade80000\n\t"
        ".word 0xade90004\n\t"
        ".word 0x48481000\n\t"
        ".word 0x48491800\n\t"
        ".word 0xade80008\n\t"
        ".word 0xade9000c\n\t"
        ".word 0x48482000\n\t"
        ".word 0x00000000\n\t"
        ".word 0xade80010\n\t"
        ".word 0x48482800\n\t"
        ".word 0x48493000\n\t"
        ".word 0x484a3800\n\t"
        ".word 0xade80014\n\t"
        ".word 0xade90018\n\t"
        ".word 0xadea001c\n\t"
        ".word 0x21ce0020\n\t"
        "lui $1, %%hi(D_80056D2C)\n\t"
        "sw $14, %%lo(D_80056D2C)($1)\n\t"
        ".set at"
        : : : "$4", "$8", "$9", "$10", "$14", "$15", "memory");
}

void PopMatrix(void) {
    __asm__ volatile(
        ".set noat\n\t"
        "lui $14, %%hi(D_80056D2C)\n\t"
        "lw $14, %%lo(D_80056D2C)($14)\n\t"
        ".word 0x00000000\n\t"
        ".word 0x1dc0000a\n\t"
        "lui $1, %%hi(D_80056D20)\n\t"
        "sw $31, %%lo(D_80056D20)($1)\n\t"
        "lui $4, %%hi(D_80056FE1)\n\t"
        ".reloc ., R_MIPS_26, printf\n\t"
        ".word 0x0c000000\n\t"
        "addiu $4, $4, %%lo(D_80056FE1)\n\t"
        "lui $31, %%hi(D_80056D20)\n\t"
        "lw $31, %%lo(D_80056D20)($31)\n\t"
        ".word 0x00000000\n\t"
        ".word 0x03e00008\n\t"
        ".word 0x00000000\n\t"
        ".word 0x21ceffe0\n\t"
        "lui $1, %%hi(D_80056D2C)\n\t"
        "sw $14, %%lo(D_80056D2C)($1)\n\t"
        "lui $15, %%hi(D_80056D30)\n\t"
        ".word 0x01ee7821\n\t"
        "addiu $15, $15, %%lo(D_80056D30)\n\t"
        ".word 0x8de80000\n\t"
        ".word 0x8de90004\n\t"
        ".word 0x48c80000\n\t"
        ".word 0x48c90800\n\t"
        ".word 0x8de80008\n\t"
        ".word 0x8de9000c\n\t"
        ".word 0x48c81000\n\t"
        ".word 0x48c91800\n\t"
        ".word 0x8de80010\n\t"
        ".word 0x00000000\n\t"
        ".word 0x48c82000\n\t"
        ".word 0x00000000\n\t"
        ".word 0x8de80014\n\t"
        ".word 0x8de90018\n\t"
        ".word 0x8dea001c\n\t"
        ".word 0x48c82800\n\t"
        ".word 0x48c93000\n\t"
        ".word 0x48ca3800\n\t"
        ".set at"
        : : : "$4", "$8", "$9", "$10", "$14", "$15", "memory");
}

__asm__(
        ".globl ScaleMatrixL\n\t"
        ".ent ScaleMatrixL\n\t"
        "ScaleMatrixL:\n\t"
        ".word 0x8c880000\n\t"
        ".word 0x8cab0000\n\t"
        ".word 0x3109ffff\n\t"
        ".word 0x00094c00\n\t"
        ".word 0x00094c03\n\t"
        ".word 0x012b0019\n\t"
        ".word 0x00085403\n\t"
        ".word 0x8cac0004\n\t"
        ".word 0x8cad0008\n\t"
        ".word 0x8c880004\n\t"
        ".word 0x00801021\n\t"
        ".word 0x00004812\n\t"
        ".word 0x00094b03\n\t"
        ".word 0x3129ffff\n\t"
        ".word 0x014b0019\n\t"
        ".word 0x00005012\n\t"
        ".word 0x000a5303\n\t"
        ".word 0x000a5400\n\t"
        ".word 0x012a4825\n\t"
        ".word 0xac890000\n\t"
        ".word 0x3109ffff\n\t"
        ".word 0x00094c00\n\t"
        ".word 0x00094c03\n\t"
        ".word 0x012b0019\n\t"
        ".word 0x00085403\n\t"
        ".word 0x8c880008\n\t"
        ".word 0x00004812\n\t"
        ".word 0x00094b03\n\t"
        ".word 0x3129ffff\n\t"
        ".word 0x014c0019\n\t"
        ".word 0x00005012\n\t"
        ".word 0x000a5303\n\t"
        ".word 0x000a5400\n\t"
        ".word 0x012a4825\n\t"
        ".word 0xac890004\n\t"
        ".word 0x3109ffff\n\t"
        ".word 0x00094c00\n\t"
        ".word 0x00094c03\n\t"
        ".word 0x012c0019\n\t"
        ".word 0x00085403\n\t"
        ".word 0x8c88000c\n\t"
        ".word 0x00004812\n\t"
        ".word 0x00094b03\n\t"
        ".word 0x3129ffff\n\t"
        ".word 0x014c0019\n\t"
        ".word 0x00005012\n\t"
        ".word 0x000a5303\n\t"
        ".word 0x000a5400\n\t"
        ".word 0x012a4825\n\t"
        ".word 0xac890008\n\t"
        ".word 0x3109ffff\n\t"
        ".word 0x00094c00\n\t"
        ".word 0x00094c03\n\t"
        ".word 0x012d0019\n\t"
        ".word 0x00085403\n\t"
        ".word 0x8c880010\n\t"
        ".word 0x00004812\n\t"
        ".word 0x00094b03\n\t"
        ".word 0x3129ffff\n\t"
        ".word 0x014d0019\n\t"
        ".word 0x00005012\n\t"
        ".word 0x000a5303\n\t"
        ".word 0x000a5400\n\t"
        ".word 0x012a4825\n\t"
        ".word 0xac89000c\n\t"
        ".word 0x3109ffff\n\t"
        ".word 0x00094c00\n\t"
        ".word 0x00094c03\n\t"
        ".word 0x012d0019\n\t"
        ".word 0x00004812\n\t"
        ".word 0x00094b03\n\t"
        ".word 0x03e00008\n\t"
        ".word 0xac890010\n\t"
        ".end ScaleMatrixL");

__asm__(
        ".globl SetMulMatrix\n\t"
        ".ent SetMulMatrix\n\t"
        "SetMulMatrix:\n\t"
        ".word 0x8c880000\n\t"
        ".word 0x8c890004\n\t"
        ".word 0x8c8a0008\n\t"
        ".word 0x8c8b000c\n\t"
        ".word 0x8c8c0010\n\t"
        ".word 0x48c80000\n\t"
        ".word 0x48c90800\n\t"
        ".word 0x48ca1000\n\t"
        ".word 0x48cb1800\n\t"
        ".word 0x48cc2000\n\t"
        ".word 0x94a80000\n\t"
        ".word 0x8ca90004\n\t"
        ".word 0x8caa000c\n\t"
        ".word 0x3c01ffff\n\t"
        ".word 0x01214824\n\t"
        ".word 0x01094025\n\t"
        ".word 0x48880000\n\t"
        ".word 0x488a0800\n\t"
        ".word 0x00000000\n\t"
        ".word 0x4a486012\n\t"
        ".word 0x94a80002\n\t"
        ".word 0x8ca90008\n\t"
        ".word 0x84aa000e\n\t"
        ".word 0x00094c00\n\t"
        ".word 0x01094025\n\t"
        ".word 0x480b4800\n\t"
        ".word 0x480c5000\n\t"
        ".word 0x480d5800\n\t"
        ".word 0x48880000\n\t"
        ".word 0x488a0800\n\t"
        ".word 0x00000000\n\t"
        ".word 0x4a486012\n\t"
        ".word 0x94a80004\n\t"
        ".word 0x8ca90008\n\t"
        ".word 0x8caa0010\n\t"
        ".word 0x3c01ffff\n\t"
        ".word 0x01214824\n\t"
        ".word 0x01094025\n\t"
        ".word 0x480e4800\n\t"
        ".word 0x480f5000\n\t"
        ".word 0x48185800\n\t"
        ".word 0x48880000\n\t"
        ".word 0x488a0800\n\t"
        ".word 0x00000000\n\t"
        ".word 0x4a486012\n\t"
        ".word 0x316bffff\n\t"
        ".word 0x000e7400\n\t"
        ".word 0x01cb7025\n\t"
        ".word 0x31adffff\n\t"
        ".word 0x0018c400\n\t"
        ".word 0x030dc025\n\t"
        ".word 0x48084800\n\t"
        ".word 0x48095000\n\t"
        ".word 0x480a5800\n\t"
        ".word 0x3108ffff\n\t"
        ".word 0x000c6400\n\t"
        ".word 0x010c4025\n\t"
        ".word 0x31efffff\n\t"
        ".word 0x00094c00\n\t"
        ".word 0x012f4825\n\t"
        ".word 0x48ce0000\n\t"
        ".word 0x48c80800\n\t"
        ".word 0x48c91000\n\t"
        ".word 0x48d81800\n\t"
        ".word 0x48ca2000\n\t"
        ".word 0x00801021\n\t"
        ".word 0x03e00008\n\t"
        ".word 0x00000000\n\t"
        ".end SetMulMatrix");

__asm__(
        ".globl ApplyRotMatrixLV\n\t"
        ".ent ApplyRotMatrixLV\n\t"
        "ApplyRotMatrixLV:\n\t"
        ".word 0x8c880000\n\t"
        ".word 0x8c890004\n\t"
        ".word 0x8c8a0008\n\t"
        ".word 0x05010008\n\t"
        ".word 0x00085bc3\n\t"
        ".word 0x00084023\n\t"
        ".word 0x00085bc3\n\t"
        ".word 0x31087fff\n\t"
        ".word 0x000b5823\n\t"
        ".word 0x10000003\n\t"
        ".word 0x00084023\n\t"
        ".word 0x00085bc3\n\t"
        ".word 0x31087fff\n\t"
        ".word 0x05210008\n\t"
        ".word 0x000963c3\n\t"
        ".word 0x00094823\n\t"
        ".word 0x000963c3\n\t"
        ".word 0x31297fff\n\t"
        ".word 0x000c6023\n\t"
        ".word 0x10000003\n\t"
        ".word 0x00094823\n\t"
        ".word 0x000963c3\n\t"
        ".word 0x31297fff\n\t"
        ".word 0x05410008\n\t"
        ".word 0x000a6bc3\n\t"
        ".word 0x000a5023\n\t"
        ".word 0x000a6bc3\n\t"
        ".word 0x314a7fff\n\t"
        ".word 0x000d6823\n\t"
        ".word 0x10000003\n\t"
        ".word 0x000a5023\n\t"
        ".word 0x000a6bc3\n\t"
        ".word 0x314a7fff\n\t"
        ".word 0x488b4800\n\t"
        ".word 0x488c5000\n\t"
        ".word 0x488d5800\n\t"
        ".word 0x00000000\n\t"
        ".word 0x4a41e012\n\t"
        ".word 0x480bc800\n\t"
        ".word 0x480cd000\n\t"
        ".word 0x480dd800\n\t"
        ".word 0x48884800\n\t"
        ".word 0x48895000\n\t"
        ".word 0x488a5800\n\t"
        ".word 0x00000000\n\t"
        ".word 0x4a49e012\n\t"
        ".word 0x05610005\n\t"
        ".word 0x00000000\n\t"
        ".word 0x000b5823\n\t"
        ".word 0x000b58c0\n\t"
        ".word 0x10000002\n\t"
        ".word 0x000b5823\n\t"
        ".word 0x000b58c0\n\t"
        ".word 0x05810005\n\t"
        ".word 0x00000000\n\t"
        ".word 0x000c6023\n\t"
        ".word 0x000c60c0\n\t"
        ".word 0x10000002\n\t"
        ".word 0x000c6023\n\t"
        ".word 0x000c60c0\n\t"
        ".word 0x05a10005\n\t"
        ".word 0x00000000\n\t"
        ".word 0x000d6823\n\t"
        ".word 0x000d68c0\n\t"
        ".word 0x10000002\n\t"
        ".word 0x000d6823\n\t"
        ".word 0x000d68c0\n\t"
        ".word 0x4808c800\n\t"
        ".word 0x4809d000\n\t"
        ".word 0x480ad800\n\t"
        ".word 0x010b4021\n\t"
        ".word 0x012c4821\n\t"
        ".word 0x014d5021\n\t"
        ".word 0xaca80000\n\t"
        ".word 0xaca90004\n\t"
        ".word 0xacaa0008\n\t"
        ".word 0x03e00008\n\t"
        ".word 0x00a01021\n\t"
        ".end ApplyRotMatrixLV");

__asm__(
        ".globl MulMatrix\n\t"
        ".ent MulMatrix\n\t"
        "MulMatrix:\n\t"
        ".word 0x8c880000\n\t"
        ".word 0x8c890004\n\t"
        ".word 0x8c8a0008\n\t"
        ".word 0x8c8b000c\n\t"
        ".word 0x8c8c0010\n\t"
        ".word 0x48c80000\n\t"
        ".word 0x48c90800\n\t"
        ".word 0x48ca1000\n\t"
        ".word 0x48cb1800\n\t"
        ".word 0x48cc2000\n\t"
        ".word 0x94a80000\n\t"
        ".word 0x8ca90004\n\t"
        ".word 0x8caa000c\n\t"
        ".word 0x3c01ffff\n\t"
        ".word 0x01214824\n\t"
        ".word 0x01094025\n\t"
        ".word 0x48880000\n\t"
        ".word 0x488a0800\n\t"
        ".word 0x00000000\n\t"
        ".word 0x4a486012\n\t"
        ".word 0x94a80002\n\t"
        ".word 0x8ca90008\n\t"
        ".word 0x84aa000e\n\t"
        ".word 0x00094c00\n\t"
        ".word 0x01094025\n\t"
        ".word 0x480b4800\n\t"
        ".word 0x480c5000\n\t"
        ".word 0x480d5800\n\t"
        ".word 0x48880000\n\t"
        ".word 0x488a0800\n\t"
        ".word 0x00000000\n\t"
        ".word 0x4a486012\n\t"
        ".word 0x94a80004\n\t"
        ".word 0x8ca90008\n\t"
        ".word 0x8caa0010\n\t"
        ".word 0x3c01ffff\n\t"
        ".word 0x01214824\n\t"
        ".word 0x01094025\n\t"
        ".word 0x480e4800\n\t"
        ".word 0x480f5000\n\t"
        ".word 0x48185800\n\t"
        ".word 0x48880000\n\t"
        ".word 0x488a0800\n\t"
        ".word 0x00000000\n\t"
        ".word 0x4a486012\n\t"
        ".word 0x316bffff\n\t"
        ".word 0x000e7400\n\t"
        ".word 0x01cb7025\n\t"
        ".word 0xac8e0000\n\t"
        ".word 0x31adffff\n\t"
        ".word 0x0018c400\n\t"
        ".word 0x030dc025\n\t"
        ".word 0xac98000c\n\t"
        ".word 0x48084800\n\t"
        ".word 0x48095000\n\t"
        ".word 0x3108ffff\n\t"
        ".word 0x000c6400\n\t"
        ".word 0x010c4025\n\t"
        ".word 0xac880004\n\t"
        ".word 0x31efffff\n\t"
        ".word 0x00094c00\n\t"
        ".word 0x012f4825\n\t"
        ".word 0xac890008\n\t"
        ".word 0xe88b0010\n\t"
        ".word 0x00801021\n\t"
        ".word 0x03e00008\n\t"
        ".word 0x00000000\n\t"
        ".end MulMatrix");

__asm__(
        ".globl MulMatrix2\n\t"
        ".ent MulMatrix2\n\t"
        "MulMatrix2:\n\t"
        ".word 0x8c880000\n\t"
        ".word 0x8c890004\n\t"
        ".word 0x8c8a0008\n\t"
        ".word 0x8c8b000c\n\t"
        ".word 0x8c8c0010\n\t"
        ".word 0x48c80000\n\t"
        ".word 0x48c90800\n\t"
        ".word 0x48ca1000\n\t"
        ".word 0x48cb1800\n\t"
        ".word 0x48cc2000\n\t"
        ".word 0x94a80000\n\t"
        ".word 0x8ca90004\n\t"
        ".word 0x8caa000c\n\t"
        ".word 0x3c01ffff\n\t"
        ".word 0x01214824\n\t"
        ".word 0x01094025\n\t"
        ".word 0x48880000\n\t"
        ".word 0x488a0800\n\t"
        ".word 0x00000000\n\t"
        ".word 0x4a486012\n\t"
        ".word 0x94a80002\n\t"
        ".word 0x8ca90008\n\t"
        ".word 0x84aa000e\n\t"
        ".word 0x00094c00\n\t"
        ".word 0x01094025\n\t"
        ".word 0x480b4800\n\t"
        ".word 0x480c5000\n\t"
        ".word 0x480d5800\n\t"
        ".word 0x48880000\n\t"
        ".word 0x488a0800\n\t"
        ".word 0x00000000\n\t"
        ".word 0x4a486012\n\t"
        ".word 0x94a80004\n\t"
        ".word 0x8ca90008\n\t"
        ".word 0x8caa0010\n\t"
        ".word 0x3c01ffff\n\t"
        ".word 0x01214824\n\t"
        ".word 0x01094025\n\t"
        ".word 0x480e4800\n\t"
        ".word 0x480f5000\n\t"
        ".word 0x48185800\n\t"
        ".word 0x48880000\n\t"
        ".word 0x488a0800\n\t"
        ".word 0x00000000\n\t"
        ".word 0x4a486012\n\t"
        ".word 0x316bffff\n\t"
        ".word 0x000e7400\n\t"
        ".word 0x01cb7025\n\t"
        ".word 0xacae0000\n\t"
        ".word 0x31adffff\n\t"
        ".word 0x0018c400\n\t"
        ".word 0x030dc025\n\t"
        ".word 0xacb8000c\n\t"
        ".word 0x48084800\n\t"
        ".word 0x48095000\n\t"
        ".word 0x3108ffff\n\t"
        ".word 0x000c6400\n\t"
        ".word 0x010c4025\n\t"
        ".word 0xaca80004\n\t"
        ".word 0x31efffff\n\t"
        ".word 0x00094c00\n\t"
        ".word 0x012f4825\n\t"
        ".word 0xaca90008\n\t"
        ".word 0xe8ab0010\n\t"
        ".word 0x00a01021\n\t"
        ".word 0x03e00008\n\t"
        ".word 0x00000000\n\t"
        ".end MulMatrix2");

VECTOR* ApplyMatrix(MATRIX* matrix, SVECTOR* input, VECTOR* output) {
    register VECTOR* result asm("$2");
    __asm__ volatile(
        ".word 0x8c880000\n\t"
        ".word 0x8c890004\n\t"
        ".word 0x8c8a0008\n\t"
        ".word 0x8c8b000c\n\t"
        ".word 0x8c8c0010\n\t"
        ".word 0x48c80000\n\t"
        ".word 0x48c90800\n\t"
        ".word 0x48ca1000\n\t"
        ".word 0x48cb1800\n\t"
        ".word 0x48cc2000\n\t"
        ".word 0xc8a00000\n\t"
        ".word 0xc8a10004\n\t"
        ".word 0x00000000\n\t"
        ".word 0x4a486012\n\t"
        ".word 0xe8d90000\n\t"
        ".word 0xe8da0004\n\t"
        ".word 0xe8db0008\n\t"
        ".word 0x00c01021"
        : "=r"(result)
        : "r"(matrix), "r"(input), "r"(output)
        : "$8", "$9", "$10", "$11", "$12", "memory");
    return result;
}

SVECTOR* ApplyMatrixSV(MATRIX* m, SVECTOR* v0, SVECTOR* v1) {
    gte_SetRotMatrix(m);
    gte_ldv0(v0);
    gte_rtv0();
    gte_stsv(v1);
    return v1;
}

MATRIX* TransMatrix(MATRIX* matrix, VECTOR* translation) {
    register MATRIX* result asm("$2");
    __asm__ volatile(
        ".word 0x8ca80000\n\t"
        ".word 0x8ca90004\n\t"
        ".word 0x8caa0008\n\t"
        ".word 0xac880014\n\t"
        ".word 0xac890018\n\t"
        ".word 0xac8a001c\n\t"
        ".word 0x00801021"
        : "=r"(result) : : "$8", "$9", "$10", "memory");
    return result;
}

__asm__(
        ".globl ScaleMatrix\n\t"
        ".ent ScaleMatrix\n\t"
        "ScaleMatrix:\n\t"
        ".word 0x8c880000\n\t"
        ".word 0x8cab0000\n\t"
        ".word 0x3109ffff\n\t"
        ".word 0x00094c00\n\t"
        ".word 0x00094c03\n\t"
        ".word 0x012b0019\n\t"
        ".word 0x8cac0004\n\t"
        ".word 0x00085403\n\t"
        ".word 0x8cad0008\n\t"
        ".word 0x8c880004\n\t"
        ".word 0x00801021\n\t"
        ".word 0x00004812\n\t"
        ".word 0x00094b03\n\t"
        ".word 0x3129ffff\n\t"
        ".word 0x014c0019\n\t"
        ".word 0x00005012\n\t"
        ".word 0x000a5303\n\t"
        ".word 0x000a5400\n\t"
        ".word 0x012a4825\n\t"
        ".word 0xac890000\n\t"
        ".word 0x3109ffff\n\t"
        ".word 0x00094c00\n\t"
        ".word 0x00094c03\n\t"
        ".word 0x012d0019\n\t"
        ".word 0x00085403\n\t"
        ".word 0x8c880008\n\t"
        ".word 0x00004812\n\t"
        ".word 0x00094b03\n\t"
        ".word 0x3129ffff\n\t"
        ".word 0x014b0019\n\t"
        ".word 0x00005012\n\t"
        ".word 0x000a5303\n\t"
        ".word 0x000a5400\n\t"
        ".word 0x012a4825\n\t"
        ".word 0xac890004\n\t"
        ".word 0x3109ffff\n\t"
        ".word 0x00094c00\n\t"
        ".word 0x00094c03\n\t"
        ".word 0x012c0019\n\t"
        ".word 0x00085403\n\t"
        ".word 0x8c88000c\n\t"
        ".word 0x00004812\n\t"
        ".word 0x00094b03\n\t"
        ".word 0x3129ffff\n\t"
        ".word 0x014d0019\n\t"
        ".word 0x00005012\n\t"
        ".word 0x000a5303\n\t"
        ".word 0x000a5400\n\t"
        ".word 0x012a4825\n\t"
        ".word 0xac890008\n\t"
        ".word 0x3109ffff\n\t"
        ".word 0x00094c00\n\t"
        ".word 0x00094c03\n\t"
        ".word 0x012b0019\n\t"
        ".word 0x00085403\n\t"
        ".word 0x8c880010\n\t"
        ".word 0x00004812\n\t"
        ".word 0x00094b03\n\t"
        ".word 0x3129ffff\n\t"
        ".word 0x014c0019\n\t"
        ".word 0x00005012\n\t"
        ".word 0x000a5303\n\t"
        ".word 0x000a5400\n\t"
        ".word 0x012a4825\n\t"
        ".word 0xac89000c\n\t"
        ".word 0x3109ffff\n\t"
        ".word 0x00094c00\n\t"
        ".word 0x00094c03\n\t"
        ".word 0x012d0019\n\t"
        ".word 0x00004812\n\t"
        ".word 0x00094b03\n\t"
        ".word 0x03e00008\n\t"
        ".word 0xac890010\n\t"
        ".end ScaleMatrix");

void SetRotMatrix(MATRIX* m) {
    gte_SetRotMatrix(m);
}

void SetLightMatrix(MATRIX* matrix) {
    __asm__ volatile(
        ".word 0x8c880000\n\t"
        ".word 0x8c890004\n\t"
        ".word 0x8c8a0008\n\t"
        ".word 0x8c8b000c\n\t"
        ".word 0x8c8c0010\n\t"
        ".word 0x48c84000\n\t"
        ".word 0x48c94800\n\t"
        ".word 0x48ca5000\n\t"
        ".word 0x48cb5800\n\t"
        ".word 0x48cc6000"
        : : : "$8", "$9", "$10", "$11", "$12");
}

void SetColorMatrix(MATRIX* matrix) {
    __asm__ volatile(
        ".word 0x8c880000\n\t"
        ".word 0x8c890004\n\t"
        ".word 0x8c8a0008\n\t"
        ".word 0x8c8b000c\n\t"
        ".word 0x8c8c0010\n\t"
        ".word 0x48c88000\n\t"
        ".word 0x48c98800\n\t"
        ".word 0x48ca9000\n\t"
        ".word 0x48cb9800\n\t"
        ".word 0x48cca000"
        : : : "$8", "$9", "$10", "$11", "$12");
}

void SetTransMatrix(MATRIX* m) {
    gte_SetTransMatrix(m);
}

void SetVertex0(SVECTOR* vertex) {
    __asm__ volatile(
        "lwc2 $0, 0(%0)\n\t"
        "lwc2 $1, 4(%0)"
        : : "r"(vertex));
}

void SetVertex1(SVECTOR* vertex) {
    __asm__ volatile(
        "lwc2 $2, 0(%0)\n\t"
        "lwc2 $3, 4(%0)"
        : : "r"(vertex));
}

void SetVertex2(SVECTOR* vertex) {
    __asm__ volatile(
        ".word 0xc8840000\n\t"
        ".word 0xc8850004"
        : : "r"(vertex));
}

void SetVertexTri(SVECTOR* v0, SVECTOR* v1, SVECTOR* v2) {
    __asm__ volatile(
        ".word 0xc8800000\n\t"
        ".word 0xc8810004\n\t"
        ".word 0xc8a20000\n\t"
        ".word 0xc8a30004\n\t"
        ".word 0xc8c40000\n\t"
        ".word 0xc8c50004");
}

void SetRGBfifo(CVECTOR* rgb0, CVECTOR* rgb1, CVECTOR* rgb2) {
    __asm__ volatile(
        ".word 0xc8940000\n\t"
        ".word 0xc8b50000\n\t"
        ".word 0xc8d60000");
}

void SetIR123(long ir1, long ir2, long ir3) {
    __asm__ volatile(
        "mtc2 %0, $9\n\t"
        "mtc2 %1, $10\n\t"
        "mtc2 %2, $11"
        : : "r"(ir1), "r"(ir2), "r"(ir3));
}

void SetIR0(s32 value) {
    __asm__ volatile("mtc2 %0, $8" : : "r"(value));
}

void SetSZfifo3(long sz1, long sz2, long sz3) {
    __asm__ volatile(
        "mtc2 %0, $17\n\t"
        "mtc2 %1, $18\n\t"
        "mtc2 %2, $19"
        : : "r"(sz1), "r"(sz2), "r"(sz3));
}

void SetSZfifo4(long sz0, long sz1, long sz2, long sz3) {
    __asm__ volatile(
        "mtc2 %0, $16\n\t"
        "mtc2 %1, $17\n\t"
        "mtc2 %2, $18\n\t"
        "mtc2 %3, $19"
        : : "r"(sz0), "r"(sz1), "r"(sz2), "r"(sz3));
}

void SetSXSYfifo(long sxy0, long sxy1, long sxy2) {
    __asm__ volatile(
        "mtc2 %0, $12\n\t"
        "mtc2 %1, $13\n\t"
        "mtc2 %2, $14"
        : : "r"(sxy0), "r"(sxy1), "r"(sxy2));
}

void SetRii(long r11r12, long r22r23, long r33) {
    __asm__ volatile(
        "ctc2 %0, $0\n\t"
        "ctc2 %1, $2\n\t"
        "ctc2 %2, $4"
        : : "r"(r11r12), "r"(r22r23), "r"(r33));
}

void SetMAC123(long mac1, long mac2, long mac3) {
    __asm__ volatile(
        "mtc2 %0, $25\n\t"
        "mtc2 %1, $26\n\t"
        "mtc2 %2, $27"
        : : "r"(mac1), "r"(mac2), "r"(mac3));
}

void SetData32(s32 value) {
    __asm__ volatile("mtc2 %0, $30" : : "r"(value));
}

void SetDQA(s32 value) {
    __asm__ volatile("ctc2 %0, $27" : : "r"(value));
}

void SetDQB(s32 value) {
    __asm__ volatile("ctc2 %0, $28" : : "r"(value));
}

void ReadGeomOffset(long* x, long* y) {
    __asm__ volatile(
        ".word 0x4848c000\n\t"
        ".word 0x4849c800\n\t"
        ".word 0x00084403\n\t"
        ".word 0x00094c03\n\t"
        ".word 0xac880000\n\t"
        ".word 0xaca90000");
}

long ReadGeomScreen(void) {
    long value;
    __asm__ volatile("cfc2 %0, $26" : "=r"(value));
    return value;
}

void SetBackColor(long red, long green, long blue) {
    red <<= 4;
    green <<= 4;
    blue <<= 4;
    __asm__ volatile(
        "ctc2 %0, $13\n\t"
        "ctc2 %1, $14\n\t"
        "ctc2 %2, $15"
        : : "r"(red), "r"(green), "r"(blue));
}

void SetFarColor(long red, long green, long blue) {
    red <<= 4;
    green <<= 4;
    blue <<= 4;
    __asm__ volatile(
        "ctc2 %0, $21\n\t"
        "ctc2 %1, $22\n\t"
        "ctc2 %2, $23"
        : : "r"(red), "r"(green), "r"(blue));
}

void SetGeomOffset(long x, long y) {
    x <<= 16;
    y <<= 16;
    __asm__ volatile(
        "ctc2 %0, $24\n\t"
        "ctc2 %1, $25"
        : : "r"(x), "r"(y));
}

void SetGeomScreen(long value) {
    __asm__ volatile("ctc2 %0, $26" : : "r"(value));
}

void LocalLight(SVECTOR* input, VECTOR* output) {
    __asm__ volatile(
        ".word 0xc8800000\n\t"
        ".word 0xc8810004\n\t"
        ".word 0x00000000\n\t"
        ".word 0x4a4a6412\n\t"
        ".word 0xe8a90000\n\t"
        ".word 0xe8aa0004\n\t"
        ".word 0xe8ab0008");
}

void DpqColor(CVECTOR* input, long p, CVECTOR* output) {
    __asm__ volatile(
        ".word 0xc8860000\n\t"
        ".word 0x48854000\n\t"
        ".word 0x00000000\n\t"
        ".word 0x4a780010\n\t"
        ".word 0xe8d60000");
}

void NormalColor(SVECTOR* normal, CVECTOR* output) {
    __asm__ volatile(
        ".word 0xc8800000\n\t"
        ".word 0xc8810004\n\t"
        ".word 0x00000000\n\t"
        ".word 0x4ac8041e\n\t"
        ".word 0xe8b60000");
}

void NormalColor3(SVECTOR* n0, SVECTOR* n1, SVECTOR* n2,
        CVECTOR* out0, CVECTOR* out1, CVECTOR* out2) {
    __asm__ volatile(
        ".word 0xc8800000\n\t"
        ".word 0xc8810004\n\t"
        ".word 0xc8a20000\n\t"
        ".word 0xc8a30004\n\t"
        ".word 0xc8c40000\n\t"
        ".word 0xc8c50004\n\t"
        ".word 0x00000000\n\t"
        ".word 0x4ad80420\n\t"
        ".word 0x8fa80010\n\t"
        ".word 0x8fa90014\n\t"
        ".word 0xe8f40000\n\t"
        ".word 0xe9150000\n\t"
        ".word 0xe9360000"
        : : : "$8", "$9", "memory");
}

void NormalColorDpq(SVECTOR* normal, CVECTOR* color, long p, CVECTOR* output) {
    __asm__ volatile(
        ".word 0xc8800000\n\t"
        ".word 0xc8810004\n\t"
        ".word 0xc8a60000\n\t"
        ".word 0x48864000\n\t"
        ".word 0x00000000\n\t"
        ".word 0x4ae80413\n\t"
        ".word 0xe8f60000");
}

void NormalColorDpq3(SVECTOR* n0, SVECTOR* n1, SVECTOR* n2, CVECTOR* color,
        long p, CVECTOR* out0, CVECTOR* out1, CVECTOR* out2) {
    __asm__ volatile(
        ".word 0xc8800000\n\t"
        ".word 0xc8810004\n\t"
        ".word 0xc8a20000\n\t"
        ".word 0xc8a30004\n\t"
        ".word 0xc8c40000\n\t"
        ".word 0xc8c50004\n\t"
        ".word 0xc8e60000\n\t"
        ".word 0xcba80010\n\t"
        ".word 0x00000000\n\t"
        ".word 0x4af80416\n\t"
        ".word 0x8fa80014\n\t"
        ".word 0x8fa90018\n\t"
        ".word 0x8faa001c\n\t"
        ".word 0xe9140000\n\t"
        ".word 0xe9350000\n\t"
        ".word 0xe9560000"
        : : : "$8", "$9", "$10", "memory");
}

#ifndef XENO_PC_PORT
void NormalLightCol(SVECTOR* normal, CVECTOR* color, CVECTOR* output) {
    __asm__ volatile(
        ".word 0xc8800000\n\t"
        ".word 0xc8810004\n\t"
        ".word 0xc8a60000\n\t"
        ".word 0x00000000\n\t"
        ".word 0x4b08041b\n\t"
        ".word 0xe8d60000");
}
#else
/* asm: lwc2 $0/$1 <- normal (V0), lwc2 $6 <- rgb|code (RGBC), nccs,
 * swc2 $22 -> out (RGB2). Consumes the GTE light state (LLM/LCM/BK) set by
 * SetLightMatrix/SetColorMatrix/SetBackColor -- all real in PsyX. */
extern void MTC2(unsigned int value, int reg);
extern unsigned int MFC2(int reg);
extern void doCOP2(int op);

void NormalLightCol(void* pNormal, void* pInColor, void* pOutColor) {
    MTC2(*(u32*)((u8*)pNormal + 0), 0);
    MTC2(*(u32*)((u8*)pNormal + 4), 1);
    MTC2(*(u32*)pInColor, 6);
    doCOP2(0x0108041B); /* nccs */
    *(u32*)pOutColor = MFC2(22);
}
#endif

void NormalColorCol3(SVECTOR* n0, SVECTOR* n1, SVECTOR* n2, CVECTOR* color,
        CVECTOR* out0, CVECTOR* out1, CVECTOR* out2) {
    __asm__ volatile(
        ".word 0xc8800000\n\t"
        ".word 0xc8810004\n\t"
        ".word 0xc8a20000\n\t"
        ".word 0xc8a30004\n\t"
        ".word 0xc8c40000\n\t"
        ".word 0xc8c50004\n\t"
        ".word 0xc8e60000\n\t"
        ".word 0x00000000\n\t"
        ".word 0x4b18043f\n\t"
        ".word 0x8fa80010\n\t"
        ".word 0x8fa90014\n\t"
        ".word 0x8faa0018\n\t"
        ".word 0xe9140000\n\t"
        ".word 0xe9350000\n\t"
        ".word 0xe9560000"
        : : : "$8", "$9", "$10", "memory");
}

void ColorDpq(VECTOR* light, CVECTOR* color, long p, CVECTOR* output) {
    __asm__ volatile(
        ".word 0xc8890000\n\t"
        ".word 0xc88a0004\n\t"
        ".word 0xc88b0008\n\t"
        ".word 0xc8a60000\n\t"
        ".word 0x48864000\n\t"
        ".word 0x00000000\n\t"
        ".word 0x4b280414\n\t"
        ".word 0xe8f60000");
}

void ColorCol(VECTOR* light, CVECTOR* color, CVECTOR* output) {
    __asm__ volatile(
        ".word 0xc8890000\n\t"
        ".word 0xc88a0004\n\t"
        ".word 0xc88b0008\n\t"
        ".word 0xc8a60000\n\t"
        ".word 0x00000000\n\t"
        ".word 0x4b38041c\n\t"
        ".word 0xe8d60000");
}

long AverageSZ3(void) {
    long value;
    __asm__ volatile(
        ".word 0x4b58002d\n\t"
        "mfc2 %0, $7"
        : "=r"(value));
    return value;
}

long AverageSZ4(void) {
    long value;
    __asm__ volatile(
        ".word 0x4b68002e\n\t"
        "mfc2 %0, $7"
        : "=r"(value));
    return value;
}

void LightColor(VECTOR* input, VECTOR* output) {
    __asm__ volatile(
        ".word 0xc8890000\n\t"
        ".word 0xc88a0004\n\t"
        ".word 0xc88b0008\n\t"
        ".word 0x00000000\n\t"
        ".word 0x4a4da412\n\t"
        ".word 0xe8a90000\n\t"
        ".word 0xe8aa0004\n\t"
        ".word 0xe8ab0008");
}

void DpqColorLight(VECTOR* light, CVECTOR* color, long p, CVECTOR* output) {
    __asm__ volatile(
        ".word 0xc8890000\n\t"
        ".word 0xc88a0004\n\t"
        ".word 0xc88b0008\n\t"
        ".word 0xc8a60000\n\t"
        ".word 0x48864000\n\t"
        ".word 0x00000000\n\t"
        ".word 0x4a680029\n\t"
        ".word 0xe8f60000");
}

void DpqColor3(CVECTOR* c0, CVECTOR* c1, CVECTOR* c2, long p,
        CVECTOR* out0, CVECTOR* out1, CVECTOR* out2) {
    __asm__ volatile(
        ".word 0xc8940000\n\t"
        ".word 0xc8b50000\n\t"
        ".word 0xc8d60000\n\t"
        ".word 0xc8c60000\n\t"
        ".word 0x48874000\n\t"
        ".word 0x00000000\n\t"
        ".word 0x4af8002a\n\t"
        ".word 0x8fa80010\n\t"
        ".word 0x8fa90014\n\t"
        ".word 0x8faa0018\n\t"
        ".word 0xe9140000\n\t"
        ".word 0xe9350000\n\t"
        ".word 0xe9560000"
        : : : "$8", "$9", "$10", "memory");
}

void Intpl(VECTOR* input, long p, CVECTOR* output) {
    __asm__ volatile(
        ".word 0xc8890000\n\t"
        ".word 0xc88a0004\n\t"
        ".word 0xc88b0008\n\t"
        ".word 0x48854000\n\t"
        ".word 0x00000000\n\t"
        ".word 0x4a980011\n\t"
        ".word 0xe8d60000");
}

VECTOR* Square12(VECTOR* input, VECTOR* output) {
    __asm__ volatile(
        ".word 0xc8890000\n\t"
        ".word 0xc88a0004\n\t"
        ".word 0xc88b0008\n\t"
        ".word 0x00000000\n\t"
        ".word 0x4aa80428\n\t"
        ".word 0xe8b90000\n\t"
        ".word 0xe8ba0004\n\t"
        ".word 0xe8bb0008"
        : : "r"(input), "r"(output) : "$2", "memory");
    return output;
}

VECTOR* Square0(VECTOR* input, VECTOR* output) {
    __asm__ volatile(
        ".word 0xc8890000\n\t"
        ".word 0xc88a0004\n\t"
        ".word 0xc88b0008\n\t"
        ".word 0x00000000\n\t"
        ".word 0x4aa00428\n\t"
        ".word 0xe8b90000\n\t"
        ".word 0xe8ba0004\n\t"
        ".word 0xe8bb0008"
        : : "r"(input), "r"(output) : "$2", "memory");
    return output;
}

long AverageZ3(long sz1, long sz2, long sz3) {
    register long result asm("$2");
    __asm__ volatile(
        ".word 0x48848800\n\t"
        ".word 0x48859000\n\t"
        ".word 0x48869800\n\t"
        ".word 0x00000000\n\t"
        ".word 0x4b58002d\n\t"
        ".word 0x48023800"
        : "=r"(result));
    return result;
}

long AverageZ4(long sz0, long sz1, long sz2, long sz3) {
    register long result asm("$2");
    __asm__ volatile(
        ".word 0x48848000\n\t"
        ".word 0x48858800\n\t"
        ".word 0x48869000\n\t"
        ".word 0x48879800\n\t"
        ".word 0x00000000\n\t"
        ".word 0x4b68002e\n\t"
        ".word 0x48023800"
        : "=r"(result));
    return result;
}

void OuterProduct12(VECTOR* left, VECTOR* right, VECTOR* output) {
    __asm__ volatile(
        ".word 0x484d0000\n\t"
        ".word 0x484e1000\n\t"
        ".word 0x484f2000\n\t"
        ".word 0x8c880000\n\t"
        ".word 0x8c890004\n\t"
        ".word 0x8c8a0008\n\t"
        ".word 0x48c80000\n\t"
        ".word 0x48c91000\n\t"
        ".word 0x48ca2000\n\t"
        ".word 0xc8ab0008\n\t"
        ".word 0xc8a90000\n\t"
        ".word 0xc8aa0004\n\t"
        ".word 0x00000000\n\t"
        ".word 0x4b78000c\n\t"
        ".word 0xe8d90000\n\t"
        ".word 0xe8da0004\n\t"
        ".word 0xe8db0008\n\t"
        ".word 0x48cd0000\n\t"
        ".word 0x48ce1000\n\t"
        ".word 0x48cf2000"
        : : : "$8", "$9", "$10", "$13", "$14", "$15", "memory");
}

#ifndef XENO_PC_PORT
void OuterProduct0(VECTOR* left, VECTOR* right, VECTOR* output) {
    __asm__ volatile(
        ".word 0x484d0000\n\t"
        ".word 0x484e1000\n\t"
        ".word 0x484f2000\n\t"
        ".word 0x8c880000\n\t"
        ".word 0x8c890004\n\t"
        ".word 0x8c8a0008\n\t"
        ".word 0x48c80000\n\t"
        ".word 0x48c91000\n\t"
        ".word 0x48ca2000\n\t"
        ".word 0xc8ab0008\n\t"
        ".word 0xc8a90000\n\t"
        ".word 0xc8aa0004\n\t"
        ".word 0x00000000\n\t"
        ".word 0x4b70000c\n\t"
        ".word 0xe8d90000\n\t"
        ".word 0xe8da0004\n\t"
        ".word 0xe8db0008\n\t"
        ".word 0x48cd0000\n\t"
        ".word 0x48ce1000\n\t"
        ".word 0x48cf2000"
        : : : "$8", "$9", "$10", "$13", "$14", "$15", "memory");
}
#else
#include "psyq/libgte.h" /* shim -> PsyX libgte.h: VECTOR/SVECTOR types */
/* asm: loads v0 into the GTE rotation diagonal (D1/D2/D3), v1 into IR1/2/3,
 * executes OP with sf=0 and stores MAC1/2/3. GTE OP semantics:
 *   MAC1 = IR3*D2 - IR2*D3, MAC2 = IR1*D3 - IR3*D1, MAC3 = IR2*D1 - IR1*D2
 * i.e. out = cross(v0, v1); sf=0 means no shift and 32-bit MACs, so plain
 * integer math is exact for the small edge vectors this path feeds it. */
void OuterProduct0(VECTOR* v0, VECTOR* v1, VECTOR* out) {
    out->vx = v0->vy * v1->vz - v0->vz * v1->vy;
    out->vy = v0->vz * v1->vx - v0->vx * v1->vz;
    out->vz = v0->vx * v1->vy - v0->vy * v1->vx;
}
#endif

long Lzc(long value) {
    register long result asm("$2");
    __asm__ volatile(
        ".word 0x4884f000\n\t"
        ".word 0x00000000\n\t"
        ".word 0x00000000\n\t"
        ".word 0x4802f800"
        : "=r"(result));
    return result;
}

void RotTransSV(SVECTOR* input, SVECTOR* output, long* flag) {
    register long result asm("$2");
    __asm__ volatile(
        ".word 0xc8800000\n\t"
        ".word 0xc8810004\n\t"
        ".word 0x00000000\n\t"
        ".word 0x4a480012\n\t"
        ".word 0x48024800\n\t"
        ".word 0x48035000\n\t"
        ".word 0xe8ab0004\n\t"
        ".word 0xa4a20000\n\t"
        ".word 0xa4a30002\n\t"
        ".word 0x4842f800"
        : "=r"(result)
        : "r"(input), "r"(output), "r"(flag)
        : "$3", "memory");
    *flag = result;
}

SVECTOR* SquareSS12(SVECTOR* input, SVECTOR* output) {
    __asm__ volatile(
        ".word 0x84820000\n\t"
        ".word 0x84830002\n\t"
        ".word 0x48824800\n\t"
        ".word 0x48835000\n\t"
        ".word 0xc88b0004\n\t"
        ".word 0x00000000\n\t"
        ".word 0x4aa80428\n\t"
        ".word 0x48024800\n\t"
        ".word 0x48035000\n\t"
        ".word 0xe8ab0004\n\t"
        ".word 0xa4a20000\n\t"
        ".word 0xa4a30002"
        : : "r"(input), "r"(output) : "$2", "$3", "memory");
    return output;
}

SVECTOR* SquareSS0(SVECTOR* input, SVECTOR* output) {
    __asm__ volatile(
        ".word 0x84820000\n\t"
        ".word 0x84830002\n\t"
        ".word 0x48824800\n\t"
        ".word 0x48835000\n\t"
        ".word 0xc88b0004\n\t"
        ".word 0x00000000\n\t"
        ".word 0x4aa00428\n\t"
        ".word 0x48024800\n\t"
        ".word 0x48035000\n\t"
        ".word 0xe8ab0004\n\t"
        ".word 0xa4a20000\n\t"
        ".word 0xa4a30002"
        : : "r"(input), "r"(output) : "$2", "$3", "memory");
    return output;
}

VECTOR* SquareSL12(SVECTOR* input, VECTOR* output) {
    __asm__ volatile(
        ".word 0x84820000\n\t"
        ".word 0x84830002\n\t"
        ".word 0x48824800\n\t"
        ".word 0x48835000\n\t"
        ".word 0xc88b0004\n\t"
        ".word 0x00000000\n\t"
        ".word 0x4aa80428\n\t"
        ".word 0xe8a90000\n\t"
        ".word 0xe8aa0004\n\t"
        ".word 0xe8ab0008"
        : : "r"(input), "r"(output) : "$2", "$3", "memory");
    return output;
}

VECTOR* SquareSL0(SVECTOR* input, VECTOR* output) {
    __asm__ volatile(
        ".word 0x84820000\n\t"
        ".word 0x84830002\n\t"
        ".word 0x48824800\n\t"
        ".word 0x48835000\n\t"
        ".word 0xc88b0004\n\t"
        ".word 0x00000000\n\t"
        ".word 0x4aa00428\n\t"
        ".word 0xe8a90000\n\t"
        ".word 0xe8aa0004\n\t"
        ".word 0xe8ab0008"
        : : "r"(input), "r"(output) : "$2", "$3", "memory");
    return output;
}

long RotTransPers(SVECTOR* input, long* sxy, long* p, long* flag) {
    register long result asm("$2");
    __asm__ volatile(
        ".word 0xc8800000\n\t"
        ".word 0xc8810004\n\t"
        ".word 0x00000000\n\t"
        ".word 0x4a180001\n\t"
        ".word 0xe8ae0000\n\t"
        ".word 0xe8c80000\n\t"
        ".word 0x4843f800\n\t"
        ".word 0x48029800\n\t"
        ".word 0xace30000"
        : "=r"(result)
        : "r"(input), "r"(sxy), "r"(p), "r"(flag)
        : "$3", "memory");
    return result >> 2;
}

long RotTransPers3(SVECTOR* v0, SVECTOR* v1, SVECTOR* v2,
        long* sxy0, long* sxy1, long* sxy2, long* p, long* flag) {
    register long result asm("$2");
    __asm__ volatile(
        ".word 0xc8800000\n\t"
        ".word 0xc8810004\n\t"
        ".word 0xc8a20000\n\t"
        ".word 0xc8a30004\n\t"
        ".word 0xc8c40000\n\t"
        ".word 0xc8c50004\n\t"
        ".word 0x00000000\n\t"
        ".word 0x4a280030\n\t"
        ".word 0x8fa80010\n\t"
        ".word 0x8fa90014\n\t"
        ".word 0x8faa0018\n\t"
        ".word 0x8fab001c\n\t"
        ".word 0xe8ec0000\n\t"
        ".word 0xe90d0000\n\t"
        ".word 0xe92e0000\n\t"
        ".word 0xe9480000\n\t"
        ".word 0x4843f800\n\t"
        ".word 0x48029800\n\t"
        ".word 0xad630000"
        : "=r"(result)
        : : "$3", "$8", "$9", "$10", "$11", "memory");
    return result >> 2;
}

void RotTrans(SVECTOR* input, VECTOR* output, long* flag) {
    register long result asm("$2");
    __asm__ volatile(
        ".word 0xc8800000\n\t"
        ".word 0xc8810004\n\t"
        ".word 0x00000000\n\t"
        ".word 0x4a480012\n\t"
        ".word 0xe8b90000\n\t"
        ".word 0xe8ba0004\n\t"
        ".word 0xe8bb0008\n\t"
        ".word 0x4842f800"
        : "=r"(result)
        : "r"(input), "r"(output), "r"(flag)
        : "memory");
    *flag = result;
}

long NormalClip(long sxy0, long sxy1, long sxy2) {
    long mac0;

    gte_ldsxy3(sxy0, sxy1, sxy2);
    gte_nclip();
    gte_stopz(&mac0);
    return mac0;
}

long RotTransPers4(SVECTOR* v0, SVECTOR* v1, SVECTOR* v2, SVECTOR* v3,
                   long* sxy0, long* sxy1, long* sxy2, long* sxy3,
                   long* p, long* flag) {
    long flag0;

    gte_ldv3(v0, v1, v2);
    gte_rtpt();
    gte_stsxy3(sxy0, sxy1, sxy2);
    gte_stflg(&flag0);

    gte_ldv0(v3);
    gte_rtps();
    gte_stsxy(sxy3);
    gte_stdp(p);
    gte_stflg(flag);
    *flag |= flag0;
    return *p >> 2;
}

/* PsyQ libgte, retail 0x8004A7BC.  The command order deliberately retains the
 * two separate FLAG samples: RTPT's result is ORed with RTPS's before AVSZ4. */
long RotAverage4(SVECTOR* v0, SVECTOR* v1, SVECTOR* v2, SVECTOR* v3,
                 long* sxy0, long* sxy1, long* sxy2, long* sxy3,
                 long* p, long* flag) {
    long flag0;

    gte_ldv3(v0, v1, v2);
    gte_rtpt();
    gte_stsxy3(sxy0, sxy1, sxy2);
    gte_stflg(&flag0);

    gte_ldv0(v3);
    gte_rtps();
    gte_stsxy(sxy3);
    gte_stflg(flag);
    gte_stdp(p);
    *flag |= flag0;

    gte_avsz4();
    gte_stotz(p);
    return *p;
}

long RotAverageNclip4(SVECTOR* v0, SVECTOR* v1, SVECTOR* v2, SVECTOR* v3,
        long* sxy0, long* sxy1, long* sxy2, long* sxy3,
        long* p, long* otz, long* flag) {
    register long result asm("$2");
    __asm__ volatile(
        ".word 0xc8800000\n\t"
        ".word 0xc8810004\n\t"
        ".word 0xc8a20000\n\t"
        ".word 0xc8a30004\n\t"
        ".word 0xc8c40000\n\t"
        ".word 0xc8c50004\n\t"
        ".word 0x00000000\n\t"
        ".word 0x4a280030\n\t"
        ".word 0x8fa80028\n\t"
        ".word 0x4843f800\n\t"
        ".word 0x00000000\n\t"
        ".word 0xad030000\n\t"
        ".word 0x4b400006\n\t"
        ".word 0x8fa80010\n\t"
        ".word 0x8fa90014\n\t"
        ".word 0x8faa0018\n\t"
        ".word 0x4802c000\n\t"
        ".word 0x00000000\n\t"
        ".word 0x1c400003\n\t"
        ".word 0x00000000\n\t"
        ".word 0x10000015\n\t"
        ".word 0x00000000\n\t"
        ".word 0xe90c0000\n\t"
        ".word 0xe92d0000\n\t"
        ".word 0xe94e0000\n\t"
        ".word 0xc8e00000\n\t"
        ".word 0xc8e10004\n\t"
        ".word 0x00000000\n\t"
        ".word 0x4a180001\n\t"
        ".word 0x8fa8001c\n\t"
        ".word 0x8fa90020\n\t"
        ".word 0x8faa0028\n\t"
        ".word 0xe90e0000\n\t"
        ".word 0x484bf800\n\t"
        ".word 0xe9280000\n\t"
        ".word 0x01635825\n\t"
        ".word 0xad4b0000\n\t"
        ".word 0x4b68002e\n\t"
        ".word 0x8fa90024\n\t"
        ".word 0x48083800\n\t"
        ".word 0x00000000\n\t"
        ".word 0xad280000"
        : "=r"(result)
        : : "$3", "$8", "$9", "$10", "$11", "memory");
    return result;
}

MATRIX* TransposeMatrix(MATRIX* input, MATRIX* output) {
    register MATRIX* result asm("$2");
    register long last asm("$10");
    __asm__ volatile(
        ".word 0x00a01021\n\t"
        ".word 0x8c890000\n\t"
        ".word 0x8c8a0004\n\t"
        ".word 0xaca90004\n\t"
        ".word 0xacaa0000\n\t"
        ".word 0xa4a90000\n\t"
        ".word 0x8c8b0008\n\t"
        ".word 0x8c89000c\n\t"
        ".word 0xacab000c\n\t"
        ".word 0xaca90008\n\t"
        ".word 0xa4aa000c\n\t"
        ".word 0xa4ab0008\n\t"
        ".word 0x848a0010\n\t"
        ".word 0xa4a90004"
        : "=r"(result), "=r"(last)
        : "r"(input), "r"(output)
        : "$9", "$11", "memory");
    *(s16*)((u8*)output + 0x10) = last;
    return result;
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", RotMatrixYXZ);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", RotMatrixZYX);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", RotMatrixX);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", RotMatrixY);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", RotMatrixZ);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", ratan2);
