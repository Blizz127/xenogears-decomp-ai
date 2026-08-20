#include "common.h"
#include "psyq/libgte.h"
#include "psyq/inline_c.h"

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", InitGeom);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", SquareRoot0);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", InvSquareRoot);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", VectorNormalS);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", VectorNormal);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", VectorNormalSS);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", func_80048DD8);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", func_80048E94);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", LoadAverage12);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", LoadAverage0);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", func_8004901C);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", func_800490A4);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", LoadAverageByte);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", LoadAverageCol);

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

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", ApplyRotMatrix);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", PushMatrix);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", PopMatrix);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", ScaleMatrixL);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", SetMulMatrix);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", ApplyRotMatrixLV);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", MulMatrix);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", MulMatrix2);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", ApplyMatrix);

SVECTOR* ApplyMatrixSV(MATRIX* m, SVECTOR* v0, SVECTOR* v1) {
    gte_SetRotMatrix(m);
    gte_ldv0(v0);
    gte_rtv0();
    gte_stsv(v1);
    return v1;
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", TransMatrix);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", ScaleMatrix);

void SetRotMatrix(MATRIX* m) {
    gte_SetRotMatrix(m);
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", SetLightMatrix);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", SetColorMatrix);

void SetTransMatrix(MATRIX* m) {
    gte_SetTransMatrix(m);
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", SetVertex0);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", SetVertex1);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", SetVertex2);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", SetVertexTri);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", SetRGBfifo);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", SetIR123);

void SetIR0(s32 value) {
    __asm__ volatile("mtc2 %0, $8" : : "r"(value));
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", SetSZfifo3);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", SetSZfifo4);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", SetSXSYfifo);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", SetRii);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", SetMAC123);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", SetData32);

void SetDQA(s32 value) {
    __asm__ volatile("ctc2 %0, $27" : : "r"(value));
}

void SetDQB(s32 value) {
    __asm__ volatile("ctc2 %0, $28" : : "r"(value));
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", ReadGeomOffset);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", ReadGeomScreen);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", SetBackColor);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", SetFarColor);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", SetGeomOffset);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", SetGeomScreen);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", LocalLight);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", DpqColor);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", NormalColor);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", NormalColor3);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", NormalColorDpq);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", NormalColorDpq3);

#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", NormalLightCol);
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

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", NormalColorCol3);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", ColorDpq);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", ColorCol);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", AverageSZ3);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", AverageSZ4);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", LightColor);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", DpqColorLight);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", DpqColor3);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", Intpl);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", Square12);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", Square0);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", AverageZ3);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", AverageZ4);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", OuterProduct12);

#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", OuterProduct0);
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

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", Lzc);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", RotTransSV);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", SquareSS12);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", SquareSS0);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", SquareSL12);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", SquareSL0);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", RotTransPers);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", RotTransPers3);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", RotTrans);

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

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", RotAverageNclip4);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", TransposeMatrix);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", RotMatrixYXZ);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", RotMatrixZYX);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", RotMatrixX);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", RotMatrixY);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", RotMatrixZ);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", ratan2);
