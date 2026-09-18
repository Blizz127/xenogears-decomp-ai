/*
 * Small retail functions whose original implementation is either a direct
 * memory write, a tail-call alias, or a GTE loop that cannot be assembled for
 * the x86-64 port.  Each body below follows the retail instructions in
 * disc/SLUS_006.64; no game state is synthesized here.
 */
#include "common.h"
#include "psyq/libgpu.h"
#include "psyq/libgte.h"
#include "system/font.h"

#include <stdarg.h>
#include <string.h>

extern u8 D_8005938C;
extern s32 D_80050618;
extern void func_80036718(int mode, char* format, va_list args);
extern void MTC2(unsigned int value, int reg);
extern unsigned int MFC2(int reg);
extern unsigned int CFC2(int reg);
extern void CTC2(unsigned int value, int reg);
extern int doCOP2(int op);

/* Retail 8004A0DC..8004A0E4: CFC2 v0,H; JR ra; NOP. */
s32 ReadGeomScreen(void)
{
    return (s32)CFC2(26);
}

/* Retail 8004987C..8004998C. This sets the GTE rotation matrix to m0*m1;
 * it does NOT overwrite either input matrix or their translations. Keep the
 * three MVMVA operations so saturation, IR/MAC and FLAG side effects come
 * from the hardware adapter, not a host floating-point matrix multiply. */
MATRIX* SetMulMatrix(MATRIX* m0, MATRIX* m1)
{
    u32 rotation[5];
    u32 result[3][3];
    s32 row, column;

    memcpy(rotation, m0, sizeof(rotation));
    for (row = 0; row < 5; row++)
        CTC2(rotation[row], row);
    for (column = 0; column < 3; column++) {
        u32 vz;
        /* Retail uses LW for columns 0/2, but LH for column 1. Retain even
         * the high halfword passed to the existing COP2 register adapter. */
        if (column == 1)
            vz = (u32)(s32)m1->m[2][column];
        else
            memcpy(&vz, (u8*)m1 + 12 + column * 2, sizeof(vz));
        MTC2((u16)m1->m[0][column] | ((u32)(u16)m1->m[1][column] << 16), 0);
        MTC2(vz, 1);
        doCOP2(0x00486012); /* MVMVA: rotation, V0, no translation, sf=12 */
        for (row = 0; row < 3; row++)
            result[row][column] = MFC2(9 + row);
    }
    CTC2((result[0][0] & 0xffffu) | (result[0][1] << 16), 0);
    CTC2((result[0][2] & 0xffffu) | (result[1][0] << 16), 1);
    CTC2((result[1][1] & 0xffffu) | (result[1][2] << 16), 2);
    CTC2((result[2][0] & 0xffffu) | (result[2][1] << 16), 3);
    CTC2(result[2][2], 4);
    return m0;
}

/* Retail libgpu 0x80043C60-0x80043C74 is exactly the setPolyFT3 macro: retain
 * the packet address/color fields and write only length 7 and opcode 0x24. */
void SetPolyFT3(POLY_FT3* primitive)
{
    setlen(primitive, 7);
    setcode(primitive, 0x24);
}

/* Retail 80043D78..80043D8C and src/slus_006.64/psyq/libgpu.c: only
 * the length byte and flat two-point line opcode are initialized. */
void SetLineF2(LINE_F2* primitive)
{
    setlen(primitive, 3);
    setcode(primitive, 0x40);
}

/* Retail 80043C74..80043C88 (asm/slus_006.64/matchings/psyq/libgpu/SetPolyG3.s):
 * exactly the setPolyG3 macro -- length 6, gouraud triangle opcode 0x30. The
 * retail battle overlay calls this main-exe symbol through the MIPS bridge
 * (dlsym by name), so it needs a real native body like SetPolyF3/SetPolyFT3. */
void SetPolyG3(POLY_G3* primitive)
{
    setlen(primitive, 6);
    setcode(primitive, 0x30);
}

/* Retail 80043C88..80043C9C (src/slus_006.64/psyq/libgpu.c): initialize
 * the gouraud textured triangle packet tag exactly like setPolyGT3. */
void SetPolyGT3(POLY_GT3* primitive)
{
    setlen(primitive, 9);
    setcode(primitive, 0x34);
}

/* Retail 80043D8C..80043DA0 (SetLineG2.s): length 4, gouraud two-point line
 * opcode 0x50. Same bridge reachability as SetPolyG3. */
void SetLineG2(LINE_G2* primitive)
{
    setlen(primitive, 4);
    setcode(primitive, 0x50);
}

/* All operands here are Q12 trig values/products in [-4096,4096]. Floor
 * each product separately as retail SRA does, not the combined expression. */
static s32 RotationZYXProduct(s32 a, s32 b)
{
    s32 product = a * b;
    return product >= 0 ? product / 4096 : -((4095 - product) / 4096);
}

/* Retail 8004ABBC..8004AE48 is scalar fixed-point math, with no GTE writes.
 * Sequential RotMatrixX/Y/Z combines terms before rounding and differs by
 * one at ordinary mixed angles. The game shim names rcos=sine, rsin=cosine. */
MATRIX* RotMatrixZYX(SVECTOR* rotation, MATRIX* matrix)
{
    s32 sx = rcos(rotation->vx), cx = rsin(rotation->vx);
    s32 sy = rcos(rotation->vy), cy = rsin(rotation->vy);
    s32 sz = rcos(rotation->vz), cz = rsin(rotation->vz);
    s32 sxsy, sycx;

    matrix->m[2][0] = -sy;
    matrix->m[2][1] = RotationZYXProduct(sx, cy);
    matrix->m[2][2] = RotationZYXProduct(cx, cy);
    matrix->m[0][0] = RotationZYXProduct(cy, cz);
    matrix->m[1][0] = RotationZYXProduct(sz, cy);
    sxsy = RotationZYXProduct(sx, sy);
    matrix->m[0][1] = RotationZYXProduct(sxsy, cz) - RotationZYXProduct(sz, cx);
    matrix->m[1][1] = RotationZYXProduct(sxsy, sz) + RotationZYXProduct(cx, cz);
    sycx = RotationZYXProduct(sy, cx);
    matrix->m[0][2] = RotationZYXProduct(sycx, cz) + RotationZYXProduct(sx, sz);
    matrix->m[1][2] = RotationZYXProduct(sycx, sz) - RotationZYXProduct(sx, cz);
    return matrix;
}

/* Retail 0x8003633C-0x80036348: one byte store and return. */
void func_8003633C(s32 value)
{
    D_8005938C = (u8)value;
}

/* Retail 0x800379B4-0x800379C4: one word store and return. */
void func_800379B4(s32 value)
{
    D_80050618 = value;
}

/* Retail 0x800379C8 is a tail jump to FontPrintf.  A C variadic wrapper cannot
 * express a machine-level tail alias portably, so forward the same va_list to
 * FontPrintf's formatter while retaining FontPrintf's null-font guard. */
void func_800379C8(char* format, ...)
{
    va_list args;

    if (g_Font != NULL) {
        va_start(args, format);
        func_80036718(0, format, args);
        va_end(args);
    }
}

/* Retail 80026FE8..8002709C, SHA-256
 * df91dc9c5881d3488381d84c56ca367b3f5a46aa7c77877ae6e87450814eabaa.
 * Keep GPF12 and the final sourceB read in the branch delay slot. */
void func_80026FE8(s32 count, s32 factor, u16* destination,
                   const u16* sourceA, const u16* sourceB)
{
    if (factor > 32) factor = 32;
    MTC2((u32)factor << 7, 8);
    u32 remaining = (u32)count;
    for (;;) {
        u16 inputB = *(const volatile u16*)sourceB;
        if (remaining == 0) break;
        --remaining;
        u16 inputA = *sourceA;
        ++sourceB;
        u32 red = inputA & 0x001F;
        u32 green = inputA & 0x03E0;
        u32 blue = inputA & 0x7C00;
        MTC2((inputB & 0x001Fu) - red, 9);
        MTC2((inputB & 0x03E0u) - green, 10);
        MTC2((inputB & 0x7C00u) - blue, 11);
        doCOP2(0x0198003D);
        red += MFC2(9) & 0x001F;
        green += MFC2(10) & 0x03E0;
        blue += MFC2(11) & 0x7C00;
        *destination = (u16)(red | green | blue);
        ++sourceA;
        ++destination;
    }
}

/* Retail 0x80026F44-0x80026FE4.  The original writes IR0/IR1/IR2/IR3 and runs
 * GPF12 for every BGR555 pixel.  Keep that exact hardware boundary and let
 * PsyCross execute the GTE operation; only the MIPS loop mechanics are C. */
void func_80026F44(s32 count, s32 factor, u16* destination,
                   const u16* source)
{
    if (factor >= 0x20) {
        factor = 0x20;
    }
    MTC2((u32)factor << 7, 8);

    while (count-- != 0) {
        u16 input = *source++;
        u16 output;

        MTC2(input & 0x001F, 9);
        MTC2(input & 0x03E0, 10);
        MTC2(input & 0x7C00, 11);
        doCOP2(0x0198003D); /* GPF12 */

        output = (u16)((MFC2(9) & 0x001F) |
                       (MFC2(10) & 0x03E0) |
                       (MFC2(11) & 0x7C00));
        if (input != 0 && output == 0) {
            output = 1;
        }
        *destination++ = (u16)((input & 0x8000) | output);
    }
}
