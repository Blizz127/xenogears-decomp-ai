#include "common.h"
#include "psyq/libgte.h"

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

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", MulMatrix0);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", CompMatrix);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", ApplyMatrixLV);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", ApplyRotMatrix);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", PushMatrix);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", PopMatrix);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", ScaleMatrixL);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", SetMulMatrix);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", ApplyRotMatrixLV);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", MulMatrix);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", MulMatrix2);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", ApplyMatrix);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", ApplyMatrixSV);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", TransMatrix);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", ScaleMatrix);

void SetRotMatrix(MATRIX* m) {
    gte_SetRotMatrix(m);
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", SetLightMatrix);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", SetColorMatrix);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", SetTransMatrix);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", SetVertex0);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", SetVertex1);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", SetVertex2);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", SetVertexTri);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", SetRGBfifo);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", SetIR123);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", SetIR0);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", SetSZfifo3);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", SetSZfifo4);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", SetSXSYfifo);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", SetRii);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", SetMAC123);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", SetData32);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", SetDQA);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/psyq/libgte", SetDQB);

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
