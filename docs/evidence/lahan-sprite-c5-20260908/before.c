#include "common.h"
#include "field/actor.h"
#include "system/memory.h"
#include "psyq/libgte.h"
#ifdef XENO_PC_PORT
#include <assert.h>
#include <stdio.h>
#include <inline_c.h>

extern void func_8001D2B0(void* pSpriteData, s16 frameIndex);
#else
/* <assert.h> is unavailable under the matching build's -nostdinc MIPS
 * preprocessor. The assert(0) calls below mark unimplemented paths in
 * functions not yet byte-matched, so a no-op assert compiles safely there.
 * (uintptr_t comes from include/types.h for both builds.) */
#define assert(x) ((void)0)
#endif

/* C1 fixes the Z rotation at zero. Preserve the signed Q12 stages in
 * retail RotMatrix (8003F738..8003F8AC); the host trig-identity version
 * rounds at different points. Decomp rcos is sine, rsin is cosine, as
 * named by the retail symbol map and preserved by the port GTE shim. */
static void AnimationC1RotMatrix(const SVECTOR* angles, MATRIX* matrix) {
    s32 sx = rcos((u16)angles->vx & 0xFFF);
    s32 cx = rsin((u16)angles->vx & 0xFFF);
    s32 sy = rcos((u16)angles->vy & 0xFFF);
    s32 cy = rsin((u16)angles->vy & 0xFFF);
    s32 negativeY = (-sy * 4096) >> 12;

    matrix->m[0][0] = (s16)cy;
    matrix->m[0][1] = 0;
    matrix->m[0][2] = (s16)sy;
    matrix->m[1][0] = (s16)-((negativeY * sx) >> 12);
    matrix->m[1][1] = (s16)cx;
    /* 8003F890 negates the product BEFORE 8003F898 shifts it. */
    matrix->m[1][2] = (s16)((-cy * sx) >> 12);
    matrix->m[2][0] = (s16)((negativeY * cx) >> 12);
    matrix->m[2][1] = (s16)sx;
    matrix->m[2][2] = (s16)((cy * cx) >> 12);
}

static u32 AnimationRead32(const u8* p);
#ifdef XENO_PC_PORT
/* C4 calls retail ApplyMatrixLV (8004947C). Keep its two GTE passes:
 * a signed-15-bit split, SF=0 quotient rotation, SF=1 remainder rotation,
 * then wrapping high*8 + low. A full-width dot product is not equivalent
 * because GTE IR loads truncate each split component to signed 16 bits. */
static void AnimationC4ApplyMatrixLV(MATRIX* matrix, const u8* input, VECTOR* output) {
    u32 quotient[3];
    u32 remainder[3];
    u32 high[3];
    u32 result[3];
    s32 i;
    SetRotMatrix(matrix);
    for (i = 0; i < 3; ++i) {
        u32 value = AnimationRead32(input + i * 4);
        if (value & 0x80000000u) {
            u32 magnitude = 0u - value;
            /* Retail NEGU wraps even at INT_MIN, then SRA sign-extends. */
            u32 shifted = (magnitude >> 15) |
                ((magnitude & 0x80000000u) ? 0xFFFE0000u : 0u);
            quotient[i] = 0u - shifted;
            remainder[i] = 0u - (magnitude & 0x7FFFu);
        } else {
            quotient[i] = value >> 15;
            remainder[i] = value & 0x7FFFu;
        }
    }
    for (i = 0; i < 3; ++i) MTC2(quotient[i], 9 + i);
    doCOP2(0x041E012);
    for (i = 0; i < 3; ++i) high[i] = MFC2(25 + i);
    for (i = 0; i < 3; ++i) MTC2(remainder[i], 9 + i);
    doCOP2(0x049E012);
    for (i = 0; i < 3; ++i) result[i] = MFC2(25 + i) + (high[i] << 3);
    output->vx = (s32)result[0];
    output->vy = (s32)result[1];
    output->vz = (s32)result[2];
}
#endif

extern void AnimScriptStackPushU8(SpriteData* pSpriteData, u8 value);
extern int rand(void);
extern s32 func_80022CAC(void* pSpriteData, s32 value);

void* func_8001FBA4(SpriteData* pSpriteData, u8* pIndex) {
    u8* pData = (u8*)pSpriteData;
    u8 index = *pIndex;
    if (!(index & 0x80)) {
        s32 signedIndex = (s8)index;
        return pData + 0x8E + (s8)pData[0x8C] + signedIndex;
    }
    return (void*)(uintptr_t)(AnimationRead32(pData + 0x88) + (index & 0x7F));
}

extern s32 D_80059198;
extern void func_80022974(void* pSpriteData);
extern void func_8001CE74(void* pTargetEntry);
extern void func_80023290(u8* pSprite, s32 animType);
extern s32 func_80021AD8(s32 color, s32 value);
extern void func_800B2AEC(void* model, void* buffer0, void* buffer1,
                           s32 red, s32 green, s32 blue);

/* sbss 800592E4-EA ("800592E4 -> EA is bss local", rendering.c): image-blob
 * pointer + VRAM x/y handoff into func_8001FB30 (asm-only on the matching
 * build; port implementation in pc_port/src/game_overrides.c). */
extern u32 D_800592E4;
extern s16 D_800592E8;
extern s16 D_800592EA;
extern void func_8001FB30(void);
extern s32 func_80023124(s32 pointA, s32 pointB);
extern void func_80021FE0(void* pSpriteData, s16 angle);
extern void func_800223B0(void* pSpriteData, s16 angle);
extern void SpriteSetScale(SpriteData* pSpriteData, short scale);

/* Child-sprite spawner (temp1.c asm 80023B84; port implementation in
 * pc_port/src/game_overrides.c). Returns the child SpriteData. */
extern void* func_80023B84(void* pSpriteData, void* pScript,
                           void* pAnimPackage);

/* Texture-page latch (temp2.c asm 8002CC10; port implementation in
 * pc_port/src/game_overrides.c). */
extern void func_8002CC10(s32 x, s32 y);

/* Sound-bank plumbing for opcodes 0xB0/0xB9 (asm 8001FCF0..8001FD28).
 * func_80039E60 is the allocated-slot SFX API (sound.c). D_8005919C is the
 * sound module's selected-bank slot (sbss). The port keeps that slot in guest
 * RAM because the retail battle overlay writes it as MIPS, so it is read
 * through PSX_ADDR exactly like func_801E5CD8 does; bank references may be
 * guest addresses or native pointers (same aliasing as FieldSoundBankPointer
 * in pc_port/src/field_object_overlay.c). */
extern void func_80039E60(s32 packedId);
#ifdef XENO_PC_PORT
#include "../../../pc_port/src/psx_memory.h"
static u32 AnimationSoundBankSelector(void) {
    u32 bank;
    memcpy(&bank, PSX_ADDR(0x8005919C), sizeof(bank));
    return bank;
}
static u8* AnimationSoundBankPointer(u32 address) {
    if (address < 0x200000u || (address & 0xFFE00000u) == 0x80000000u ||
        (address & 0xFFE00000u) == 0xA0000000u) {
        return PSX_ADDR(address);
    }
    return (u8*)(uintptr_t)address;
}
#else
extern u32 D_8005919C;
static u32 AnimationSoundBankSelector(void) {
    return D_8005919C;
}
static u8* AnimationSoundBankPointer(u32 address) {
    return (u8*)(uintptr_t)address;
}
#endif

/* Model-data load helpers for opcodes 0xF5/0xF6. Relocators live in
 * temp2.c (func_8002C3E8 real; func_8002C59C asm-only on the matching
 * build, ported in pc_port/src/game_overrides.c); allocator/fixup pair
 * per the externs in src/field/main/misc3.c. */
extern void func_8002C59C(u8* pModel);
extern int func_8002C3E8(u8* pModel);
extern void func_8002CB54(void* modelData, u32* out1, u32* out2);
extern void func_8002C8CC(void* a0, void* a1, int a2);

/* 0xBC fixed vector blocks + track table + camera matrix (all named in the
 * retail asm of func_8001FBE4; splat carries their addresses). */
extern s32 D_8006F99C;
extern s32 D_8006F9AC;
extern u8 D_800C3EB0[];
extern MATRIX D_8004FBB8;

/* .L80020654 (asm 80020650-8002072C): anchor-table position for the 0xBC
 * sub-commands 8-0xA (target) and 0xB-0x11 (parent; 0x19-0x1F use the
 * player and stay unported).  Anchor entries are s8 (dx,dy) pairs at
 * transform(+0x20)->modelHeader(+0x34) + (idx-0xA)*8, X-mirrored by the
 * reference sprite's +0xAC bit 2 (NOT the +0x3C bit 3 the bit7-clear anchor
 * path uses - retail keeps them distinct), scaled by +0x2C (12-bit fixed,
 * round toward zero), added to the reference position.  Missing transform
 * or non-mode-1 references leave the vector untouched (zeroed by the
 * caller; uninitialized stack in retail). */
static void Xeno0xBCAnchorVec(u8* ref, s32 anchorIdx, SVECTOR* vec) {
    u8* pBase = (u8*)(uintptr_t)*(u32*)(ref + 0x20);
    u8* table;
    s32 dx = 0;
    s32 dy = 0;
    s32 scale;
    s32 sx;
    s32 sy;

    if (pBase == NULL) {
        return;
    }
    if ((*(u32*)(ref + 0x3C) & 0x3) != 1) {
        return;
    }
    table = (u8*)(uintptr_t)*(u32*)(pBase + 0x34);
    if (table != NULL) {
        u8* e = table + (anchorIdx - 0xA) * 8;

        dx = *(s8*)(e + 0x0);
        dy = *(s8*)(e + 0x1);
    }
    if ((*(u32*)(ref + 0xAC) >> 2) & 0x1) {
        dx = -dx;
    }
    scale = *(s16*)(ref + 0x2C);
    sx = dx * scale;
    if (sx < 0) {
        sx += 0xFFF;
    }
    sx >>= 12;
    sy = dy * scale;
    if (sy < 0) {
        sy += 0xFFF;
    }
    sy >>= 12;
    vec->vx = (s16)(sx + *(s16*)(ref + 0x2));
    vec->vy = (s16)(sy + *(s16*)(ref + 0x6));
    vec->vz = *(s16*)(ref + 0xA);
}

/* jtbl_800183D8 has 0x73 entries. These exact entries branch directly to
 * the register-restoring return at 80021AB8; they are not missing handlers.
 * Keep this predicate separate so its complete domain can be checked against
 * the retail table without exercising unrelated, unfinished opcode bodies. */
int AnimationScriptOpcodeIsNoop(u32 opcodeIndex) {
    switch ((u8)opcodeIndex) {
    case 0x8B: case 0x8E: case 0x8F: case 0x95:
    case 0x97: case 0x98: case 0x99: case 0x9A: case 0x9B:
    case 0x9C: case 0x9D: case 0x9E: case 0x9F:
    case 0xB1: case 0xB2: case 0xBE: case 0xC2: case 0xC3:
    case 0xC7: case 0xC8: case 0xCA: case 0xCB: case 0xD4:
    case 0xE1: case 0xE2: case 0xE3: case 0xE4: case 0xE8:
    case 0xEC: case 0xF0: case 0xF3: case 0xF4: case 0xF8:
    case 0xF9: case 0xFA: case 0xFB:
        return 1;
    default:
        return 0;
    }
}

/* Packed little-endian accesses for the retail data handlers. Byte accesses
 * also preserve script/sprite aliasing without C effective-type assumptions. */
static u16 AnimationRead16(const u8* p) {
    return (u16)((u32)p[0] | ((u32)p[1] << 8));
}
static u32 AnimationRead32(const u8* p) {
    return (u32)p[0] | ((u32)p[1] << 8) | ((u32)p[2] << 16) | ((u32)p[3] << 24);
}
static void AnimationWrite16(u8* p, u16 value) {
    p[0] = (u8)value;
    p[1] = (u8)(value >> 8);
}
static void AnimationWrite32(u8* p, u32 value) {
    p[0] = (u8)value;
    p[1] = (u8)(value >> 8);
    p[2] = (u8)(value >> 16);
    p[3] = (u8)(value >> 24);
}

void func_8001FBE4(void* pSpriteData, u32 opcodeIndex, void* operands) {
    u32 dispatchIndex = (u8)opcodeIndex - 0x8A;

    (void)pSpriteData;
    (void)operands;

    if (dispatchIndex >= 0x73) {
        return;
    }

    if ((u8)opcodeIndex == 0xB5) {
        /* Retail 80021318..80021340: signed byte in Q8, gated by mode.
         * SpriteSetScale is the native owner of retail 80022000. */
        u32 flags = AnimationRead32((u8*)pSpriteData + 0x3C);
        s16 scale = (s16)((s8)((u8*)operands)[0] * 256);
        if (flags & 3u) SpriteSetScale((SpriteData*)pSpriteData, scale);
        return;
    }

    if ((u8)opcodeIndex == 0x8C) { /* 8001FD50: face on X/Z plane. */
        u8* p = pSpriteData;
        u8* other = (u8*)(uintptr_t)AnimationRead32(p + 0x74);
        s32 pointA;
        s32 pointB;
        s16 angle;
        if (other == NULL) return;
        pointA = (u16)AnimationRead16(p + 0x2) |
                 ((u32)(u16)AnimationRead16(p + 0xA) << 16);
        pointB = (u16)AnimationRead16(other + 0x2) |
                 ((u32)(u16)AnimationRead16(other + 0xA) << 16);
        angle = (s16)func_80023124(pointA, pointB);
#ifdef XENO_TEST_8C
        extern void XenoTestAnimationSetAngle(void*, s16);
        extern void XenoTestAnimationApplyAngle(void*, s16);
        XenoTestAnimationSetAngle(p, angle);
        XenoTestAnimationApplyAngle(p, angle);
#else
        func_80021FE0(p, angle);
        func_800223B0(p, angle);
#endif
        return;
    }

    /* Retail data-only destinations from jtbl_800183D8. Remaining commands
     * still require their own bodies; this switch has no default skip. */
    switch ((u8)opcodeIndex) {
    case 0xA7: {
        /* Retail 8001FDF0..8001FE60. The operand may be a resolved sprite
         * variable (battle C8), so read it before updating the delay. */
        extern s32 g_WorkListCurTimer;
        u8* p = pSpriteData;
        u32 value = *(u8*)operands;
        if (value & 0x80u) {
            g_WorkListCurTimer = (value & 0x7Fu) + 1;
        } else {
            u32 scale = (AnimationRead32(p + 0xAC) >> 7) & 0xFFFu;
            /* Both factors are nonnegative; retail's signed /256
             * truncation has the same result as this shift. */
            u32 delay = ((value + 1u) * scale) >> 8;
            if (delay == 0) delay = 1;
            AnimationWrite16(p + 0x9E, (u16)AnimationRead16(p + 0x9E) + delay);
        }
        return;
    }
    case 0xD0: case 0xD1: case 0xD2: case 0xD3:
    case 0xD5: case 0xDD: case 0xDE: {
        /* 80020BB0/80020BE8/80020D68: resolve both variables before
         * loading either value. D0/D3/DD/DE share the retail ADD body;
         * D2/D5 share DIVU (including its all-ones zero-divisor result). */
        u8* left = func_8001FBA4((SpriteData*)pSpriteData, (u8*)operands);
        u8* right = func_8001FBA4((SpriteData*)pSpriteData, (u8*)operands + 1);
        u32 a = left[0];
        u32 b = right[0];
        u32 value;
        if ((u8)opcodeIndex == 0xD1) value = a * b;
        else if ((u8)opcodeIndex == 0xD2 || (u8)opcodeIndex == 0xD5)
            value = b ? a / b : 0xFFFFFFFFu;
        else value = a + b;
        left[0] = (u8)value;
        return;
    }
    case 0xD6: case 0xD7: case 0xD8:
    case 0xD9: case 0xDA: case 0xDB: case 0xDC: {
        /* 80020C50..80020D64: byte immediate arithmetic and shifts.
         * DA sign-extends its byte value; DC's assembled two-byte value
         * is zero-extended before SRAV. Variable shift counts use low 5 bits. */
        u8* slot = func_8001FBA4((SpriteData*)pSpriteData, (u8*)operands);
        u32 value = slot[0];
        s32 operand;
        if ((u8)opcodeIndex == 0xDB || (u8)opcodeIndex == 0xDC)
            value |= (u32)slot[1] << 8;
        operand = (s8)((u8*)operands)[1];
        switch ((u8)opcodeIndex) {
        case 0xD6: value += (u8)operand; break;
        case 0xD7: value *= (u32)operand; break;
        case 0xD8:
            value = operand ? (u32)((s32)value / operand) : 0xFFFFFFFFu;
            break;
        case 0xD9: case 0xDB: value <<= (u32)operand & 31u; break;
        case 0xDA: value = (u32)((s32)(s8)value >> ((u32)operand & 31u)); break;
        case 0xDC: value >>= (u32)operand & 31u; break;
        }
        slot[0] = (u8)value;
        if ((u8)opcodeIndex == 0xDB || (u8)opcodeIndex == 0xDC)
            slot[1] = (u8)(value >> 8);
        return;
    }
    case 0x8A: { /* 80021794: notably does not clear velocity-Y at +10. */
        u8* p = pSpriteData;
        AnimationWrite32(p + 0xC, 0);
        AnimationWrite32(p + 0x14, 0);
        AnimationWrite32(p + 0x18, 0);
        return;
    }
    case 0xA1: { /* 800219AC..80021A40: set vertical velocity. */
        u8* p = pSpriteData;
        u32 value = 0;
        u32 numerator;
        u32 divisor;
        u32 quotient;
        if (AnimationRead32(p + 0xA8) & 1u) {
            u8* state = (u8*)(uintptr_t)AnimationRead32(p + 0x7C);
            value = AnimationRead32(state);
        }
        if (value == 0) {
            /* Both MULT results and shifts wrap at 32 bits on retail. */
            value = (u32)(s32)(s8)((u8*)operands)[0] << 4;
            value *= (u32)D_80059198 + 1u;
            value *= (u32)(s32)(s16)AnimationRead16(p + 0x82);
            if ((s32)value < 0) value += 0xFFFu;
            value = (u32)((s32)value >> 12) << 8;
        }
        AnimationWrite32(p + 0x10, value);
        numerator = AnimationRead32(p + 0x10) << 8;
        divisor = (AnimationRead32(p + 0xAC) >> 7) & 0xFFFu;
        /* R3000 signed DIV defines LO even when its divisor is zero. */
        quotient = divisor ? (u32)((s32)numerator / (s32)divisor)
                           : ((s32)numerator < 0 ? 1u : 0xFFFFFFFFu);
        AnimationWrite32(p + 0x10, numerator);
        AnimationWrite32(p + 0x10, quotient);
        return;
    }
    case 0xA5: { /* 80021884..800218DC: add scaled speed, then rebuild velocity. */
        u8* p = pSpriteData;
        s32 operand = (s8)((u8*)operands)[0];
        u32 time = (u32)D_80059198 + 1u;
        u32 product = ((u32)operand << 4) * time;
        u32 displacement;

        product *= (u32)(s32)(s16)AnimationRead16(p + 0x82);
        if ((s32)product < 0) product += 0xFFFu;
        displacement = (u32)((s32)product >> 12) << 8;
        AnimationWrite32(p + 0x18, AnimationRead32(p + 0x18) + displacement);
        func_80022974(p);
        return;
    }
    case 0xA9: { /* 80021698..800216F4: scaled relative X displacement. */
        u8* p = pSpriteData;
        s32 operand = (s8)((u8*)operands)[0];
        s32 scale = (s16)AnimationRead16(p + 0x2C);
        u32 product = (u32)operand * (u32)scale;
        s32 displacement;
        u32 flags;
        u32 delta;

        if ((s32)product < 0) product += 0xFFFu;
        displacement = func_80022CAC(p, (s32)product >> 12);
        flags = AnimationRead32(p + 0xAC);
        delta = (u32)displacement << 16;
        if ((flags >> 2) & 1u) delta = 0u - delta;
        AnimationWrite32(p, AnimationRead32(p) + delta);
        return;
    }
    case 0xAA: { /* 800216F4..80021730: scaled relative Y displacement. */
        u8* p = pSpriteData;
        s32 operand = (s8)((u8*)operands)[0];
        s32 scale = (s16)AnimationRead16(p + 0x2C);
        s32 product = operand * scale;
        s32 displacement;
        if (product < 0) product += 0xFFF;
        displacement = func_80022CAC(p, product >> 12);
        AnimationWrite32(p + 4, AnimationRead32(p + 4) +
                         ((u32)displacement << 16));
        return;
    }
    case 0xAB: { /* 80021730..8002176C: scaled relative Z displacement. */
        u8* p = pSpriteData;
        s32 operand = (s8)((u8*)operands)[0];
        s32 scale = (s16)AnimationRead16(p + 0x2C);
        s32 product = operand * scale;
        s32 displacement;
        if (product < 0) product += 0xFFF;
        displacement = func_80022CAC(p, product >> 12);
        AnimationWrite32(p + 8, AnimationRead32(p + 8) +
                         ((u32)displacement << 16));
        return;
    }
    case 0xE7: { /* 80021340..80021374: add a doubled, narrowed scale delta. */
        u8* p = pSpriteData;
        u8* source = operands;
        u32 high = source[1];
        u32 packed = source[0] | (high << 8);
        s32 delta = (s16)(u16)(packed << 1);
        s32 scale = (s16)AnimationRead16(p + 0x2C);
        SpriteSetScale((SpriteData*)p, (s16)(scale + delta));
        return;
    }
    case 0x93: {
        /* Retail80020E30..80020F34: inherit parent direction transforms. */
        extern s32 func_8001EE68(void*);
        extern void func_8001D4E8(void*);
        u8* p = pSpriteData;
        u8* parent = (u8*)(uintptr_t)AnimationRead32(p + 0x70);
        u8* model;
        u8* parentModel;
        u32 i;
        if (parent == NULL || (AnimationRead32(p + 0x3C) & 3u) == 0) return;
        if (!func_8001EE68((void*)(uintptr_t)AnimationRead32(
                (u8*)(uintptr_t)AnimationRead32(p + 0x24)))) {
            AnimationWrite32(p + 0x40,
                (AnimationRead32(p + 0x40) & 0xFFFE1FFFu) | 0x1C000u);
        }
        model = (u8*)(uintptr_t)AnimationRead32(p + 0x20);
        if (model != NULL) {
            parentModel = (u8*)(uintptr_t)AnimationRead32(parent + 0x20);
            if (AnimationRead32(parentModel + 0x34) != 0) {
                func_8001D4E8(p);
                for (i = 0; i < 8; i++) {
                    u8* dst;
                    u8* src;
                    u32 first;
                    u32 second;
                    model = (u8*)(uintptr_t)AnimationRead32(p + 0x20);
                    parentModel = (u8*)(uintptr_t)AnimationRead32(parent + 0x20);
                    dst = (u8*)(uintptr_t)AnimationRead32(model + 0x34) + i * 8;
                    src = (u8*)(uintptr_t)AnimationRead32(parentModel + 0x34) + i * 8;
                    first = AnimationRead32(src);
                    second = AnimationRead32(src + 4);
                    AnimationWrite32(dst, first);
                    AnimationWrite32(dst + 4, second);
                }
                model = (u8*)(uintptr_t)AnimationRead32(p + 0x20);
                parentModel = (u8*)(uintptr_t)AnimationRead32(parent + 0x20);
                {
                    u8 first = parentModel[0x3C];
                    u8 second = parentModel[0x3D];
                    model[0x3C] = first;
                    model[0x3D] = second;
                }
            }
        }
        func_8001D2B0(p, (s16)AnimationRead16(p + 0x34));
        return;
    }
    case 0xBD: {
        /* Retail80021410..8002143C: indexed child script. The pointer at
         *8006BE20 is part of the packed shared block beginning8006BE10. */
        extern u8 D_8006BE10[];
        u8* p = pSpriteData;
        u32 index = *(u8*)operands;
        u8* table = (u8*)(uintptr_t)AnimationRead32(D_8006BE10 + 0x10);
        u32 offset = AnimationRead16(table + 2u + index * 2u);
        void* package = (void*)(uintptr_t)AnimationRead32(p + 0x24);
        func_80023B84(p, table + offset, package);
        return;
    }
    case 0xE9: case 0xEA: case 0xEB: {
        /* Retail 80021374..8002140C, then shared dirty flag at800215A4.
         * Read both operand bytes and the model pointer before any writes. */
        u8* p = pSpriteData;
        u8* source = operands;
        u32 high = source[1];
        u32 low = source[0];
        u8* model = (u8*)(uintptr_t)AnimationRead32(p + 0x20);
        u32 offset = 6u + ((u8)opcodeIndex - 0xE9u) * 2u;
        u32 delta = (low | (high << 8)) << 1;
        if (model != NULL) {
            /* The halfword store retains the same low16 bits for signed
             * and unsigned interpretations of the doubled operand. */
            AnimationWrite16(model + offset, AnimationRead16(model + offset) + delta);
            AnimationWrite32(p + 0x3C, AnimationRead32(p + 0x3C) | 0x10000000u);
        }
        return;
    }
    case 0xF2: { /* 80020FC8..800210F0: add signed RGB deltas. */
        u8* p = pSpriteData;
        u8* source = operands;
        s32 red = p[0x28];
        s32 delta = (s8)source[0];
        u8* base = (u8*)(uintptr_t)AnimationRead32(p + 0x20);
        s32 green;
        s32 blue;
        u32 flags;
        red = func_80021AD8(red, delta);
        green = p[0x29];
        p[0x28] = (u8)red;
        green = func_80021AD8(green, (s8)source[1]);
        blue = p[0x2A];
        p[0x29] = (u8)green;
        blue = func_80021AD8(blue, (s8)source[2]);
        flags = AnimationRead32(p + 0x3C);
        p[0x2A] = (u8)blue;
        if ((flags & 3u) == 2u) {
            delta = (s8)source[0];
            AnimationWrite16(base + 0x38,
                             AnimationRead16(base + 0x38) + delta);
            delta = (s8)source[1];
            AnimationWrite16(base + 0x3A,
                             AnimationRead16(base + 0x3A) + delta);
            delta = (s8)source[2];
            AnimationWrite16(base + 0x3C,
                             AnimationRead16(base + 0x3C) + delta);
        }
        if ((AnimationRead32(p + 0x3C) & 3u) == 1u) {
            func_8001F6B0(p);
        }
        flags = AnimationRead32(p + 0x40);
        if (((flags >> 13) & 0xFu) == 15u) {
            u8* currentBase = (u8*)(uintptr_t)AnimationRead32(p + 0x20);
            if (AnimationRead32(currentBase + 0x34) != 0 &&
                (flags & 2u) == 0) {
                /* Colors use the early base snapshot; pointers are reloaded. */
                green = (s16)AnimationRead16(base + 0x3A);
                red = (s16)AnimationRead16(base + 0x38);
                blue = (s16)AnimationRead16(base + 0x3C);
                func_800B2AEC(
                    (void*)(uintptr_t)AnimationRead32(currentBase + 0x34),
                    (void*)(uintptr_t)AnimationRead32(currentBase + 0x2C),
                    (void*)(uintptr_t)AnimationRead32(currentBase + 0x30),
                    red, green, blue);
            }
        }
        return;
    }
    case 0xF1: { /* 80020F4C..80020FC8: set RGB, preserving packed aliases. */
        u8* p = pSpriteData;
        u8* source = operands;
        u8 red = source[0];
        u8* base = (u8*)(uintptr_t)AnimationRead32(p + 0x20);
        u32 flags;
        p[0x28] = red;
        p[0x29] = source[1];
        flags = AnimationRead32(p + 0x3C);
        p[0x2A] = source[2];
        if ((flags & 3u) == 2u) {
            /* Each source byte is reloaded after the previous halfword store. */
            AnimationWrite16(base + 0x38, source[0]);
            AnimationWrite16(base + 0x3A, source[1]);
            AnimationWrite16(base + 0x3C, source[2]);
        }
        /* The halfword destinations may alias the sprite mode itself. */
        if ((AnimationRead32(p + 0x3C) & 3u) == 1u) {
            func_8001F6B0(p);
        }
        return;
    }
    case 0x91: /* 80020DE8 -> 80020E04: no operand; clear color-code bit 0. */
        ((u8*)pSpriteData)[0x2B] &= 0xFEu;
        func_8001F6B0(pSpriteData);
        return;
    case 0xBA: /* 80020F38..80020F4C: existing blend/type helper. */
        func_80023290((u8*)pSpriteData, ((u8*)operands)[0]);
        return;
    case 0xC4: { /* 800215B8..80021644: rotate the velocity about Z. */
        u8* p = pSpriteData;
        u32 randomByte = (u32)rand() & 0xFFu;
        u32 range = ((u8*)operands)[0];
        s32 delta = (s32)((randomByte * range) >> 8) - (s32)(range >> 1);
        SVECTOR angles;
        MATRIX matrix;
        VECTOR result;
        angles.vx = 0;
        angles.vy = 0;
        angles.vz = (s16)(delta * 16);
        /* With X=Y=0, this is exactly [[cos,-sin,0],[sin,cos,0],[0,0,ONE]]. */
        RotMatrix(&angles, &matrix);
#ifdef XENO_PC_PORT
        AnimationC4ApplyMatrixLV(&matrix, p + 0x0C, &result);
#else
        ApplyMatrixLV(&matrix, (VECTOR*)(p + 0x0C), &result);
#endif
        AnimationWrite32(p + 0x0C, (u32)result.vx);
        AnimationWrite32(p + 0x10, (u32)result.vy);
        AnimationWrite32(p + 0x14, (u32)result.vz);
        return;
    }
    case 0xAC: { /* 80021644..80021698: random angle adjustment. */
        u8* p = pSpriteData;
        u32 randomByte = (u32)rand() & 0xFFu;
        /* 8002164C reads after RNG, even when operands alias its seed. */
        u32 range = ((u8*)operands)[0];
        s32 delta = (s32)((randomByte * range) >> 8) - (s32)(range >> 1);
        s16 angle = (s16)(AnimationRead16(p + 0x32) + delta * 16);
        func_80021FE0(p, angle);
        return;
    }
    case 0xAD: { /* 80021468 -> 800214C8 */
        u8* p = pSpriteData;
        u32 flags = AnimationRead32(p + 0xA8) & 0xFFFFF801u;
        u32 value = ((u8*)operands)[0];
        AnimationWrite32(p + 0xA8, flags | (value << 1));
        return;
    }
    case 0xAE: case 0xAF: { /* 800214D4 / 80021570 */
        u8* p = pSpriteData;
        s32 value = (s8)((u8*)operands)[0] * 16;
        u8* base;
        if (AnimationRead32(p + 0xAC) & 4) value = -value;
        base = (u8*)(uintptr_t)AnimationRead32(p + 0x20);
        if (base == NULL) return;
        if ((u8)opcodeIndex == 0xAE) value += AnimationRead16(base + 4);
        AnimationWrite16(base + 4, (u16)value);
        AnimationWrite32(p + 0x3C, AnimationRead32(p + 0x3C) | 0x10000000u);
        return;
    }
    case 0xB6: case 0xB7: { /* 80021518 / 80021544 */
        u8* p = pSpriteData;
        u8* base = (u8*)(uintptr_t)AnimationRead32(p + 0x20);
        unsigned offset = (u8)opcodeIndex == 0xB6 ? 0 : 2;
        s32 value;
        if (base == NULL) return;
        value = (s8)((u8*)operands)[0] * 16;
        AnimationWrite16(base + offset, (u16)(AnimationRead16(base + offset) + value));
        AnimationWrite32(p + 0x3C, AnimationRead32(p + 0x3C) | 0x10000000u);
        return;
    }
    case 0xB4: /* 80021480..90: capture the byte before the stack push. */
        AnimScriptStackPushU8((SpriteData*)pSpriteData, ((u8*)operands)[0]);
        return;
    case 0xB8: { /* 80021494: signed byte decrement with byte wrap. */
        u8* p = pSpriteData;
        s32 value = (s8)((u8*)operands)[0];
        p[0x8C] = (u8)(p[0x8C] - value);
        return;
    }
    case 0xBB: { /* 80020E14..2C: signed byte added to the depth bias. */
        u8* p = pSpriteData;
        s32 delta = (s8)((u8*)operands)[0];
        AnimationWrite16(p + 0x30, (u16)(AnimationRead16(p + 0x30) + delta));
        return;
    }
    case 0xC0: { /* 80020158..800201F0: random displacement in the X/Z plane. */
        u8* p = pSpriteData;
        s32 radius = (s32)((u32)rand() & 0xFFu);
        s32 product;
        s32 angle;
        s32 component;
        u32 displacement;

        radius = (radius * ((u8*)operands)[0]) >> 8;
        product = radius * (s16)AnimationRead16(p + 0x2C);
        if (product < 0) product += 0xFFF;
        radius = product >> 12;
        angle = rand();
        /* Retail symbol rsin is the cosine entry; rcos is the sine entry. */
        component = func_80022CAC(p, (s32)rsin(angle));
        displacement = ((u32)component * (u32)radius) << 4;
        AnimationWrite32(p, AnimationRead32(p) + displacement);
        component = func_80022CAC(p, (s32)rcos(angle));
        displacement = ((u32)component * (u32)radius) << 4;
        AnimationWrite32(p + 8, AnimationRead32(p + 8) - displacement);
        return;
    }
    case 0xC1: { /* 800201F0..800202F0: scatter around the current position. */
        u8* p = pSpriteData;
        SVECTOR local;
        SVECTOR angles;
        VECTOR translation;
        MATRIX matrix;
        long flag;
        s32 radius;
        s32 product;

        /* The first RNG call precedes the operand and scale reads. */
        radius = (s32)((u32)rand() & 0xFFu);
        radius = (radius * ((u8*)operands)[0]) >> 8;
        product = radius * (s16)AnimationRead16(p + 0x2C);
        if (product < 0) product += 0xFFF;
        local.vx = (s16)func_80022CAC(p, product >> 12);
        local.vy = 0;
        local.vz = 0;
        angles.vx = (s16)rand();
        angles.vy = (s16)rand();
        angles.vz = 0;
        translation.vx = (s16)AnimationRead16(p + 2);
        translation.vy = (s16)AnimationRead16(p + 6);
        translation.vz = (s16)AnimationRead16(p + 0xA);
        TransMatrix(&matrix, &translation);
        SetTransMatrix(&matrix);
        AnimationC1RotMatrix(&angles, &matrix);
        SetRotMatrix(&matrix);
        RotTransSV(&local, &local, &flag);
        AnimationWrite32(p, (u32)(s32)local.vx << 16);
        AnimationWrite32(p + 4, (u32)(s32)local.vy << 16);
        AnimationWrite32(p + 8, (u32)(s32)local.vz << 16);
        return;
    }
    case 0xC9: { /* 8001FC78: two-byte sibling of C6. */
        u8* p = pSpriteData;
        if (AnimationRead32(p + 0xA8) & 1) {
            u16 value = AnimationRead16(operands);
            u8* state = (u8*)(uintptr_t)AnimationRead32(p + 0x7C);
            AnimationWrite16(state + 0xC, value);
        }
        return;
    }
    case 0xCD: { /* 8001FF0C..BC: model or indexed direction angle. */
        u8* p = pSpriteData;
        u8* model = (u8*)(uintptr_t)AnimationRead32(p + 0x20);
        u32 operand;
        u32 index;
        u16 angle;
        u8* destination;
        if (model == NULL) return;
        operand = AnimationRead16(operands);
        angle = (u16)((operand & 0x1FFu) << 3);
        index = (operand >> 9) & 7u;
        destination = model;
        if (index != 0) {
            u8* directions = (u8*)(uintptr_t)AnimationRead32(model + 0x34);
            if (directions == NULL) return;
            destination = directions + index * 8u + 2u;
        }
        if ((operand & 0x1000u) == 0)
            angle = (u16)(AnimationRead16(destination) + angle);
        AnimationWrite16(destination, angle);
        if (index == 0)
            AnimationWrite32(p + 0x3C, AnimationRead32(p + 0x3C) | 0x10000000u);
        return;
    }
    case 0xCC: { /* 8001FD2C: signed relative address, not a stream jump. */
        u8* p = pSpriteData;
        s32 relative = (s16)AnimationRead16(operands);
        u32 stream = AnimationRead32(p + 0x64);
        AnimationWrite32(p + 0x88, stream + (u32)relative);
        return;
    }
    case 0xBF: /* 8001FEE0: zero-extended operand byte to halfword +36. */
        AnimationWrite16((u8*)pSpriteData + 0x36, ((u8*)operands)[0]);
        return;
    case 0xA2: /* 8001FF00: operand byte to sprite+3D. */
        ((u8*)pSpriteData)[0x3D] = ((u8*)operands)[0];
        return;
    case 0xED: case 0xEF: { /* 80021A44 / 80021AA0 */
        u8* p = pSpriteData;
        u32 value = AnimationRead16(operands);
        AnimationWrite32(p + ((u8)opcodeIndex == 0xED ? 0 : 8), value << 16);
        return;
    }
    case 0xE5: { /* 80020C20..4C: random byte through the script destination. */
        u8* destination = func_8001FBA4((SpriteData*)pSpriteData, (u8*)operands);
        u32 random = (u32)rand() & 0xFFu;
        /* Retail resolves the destination before rand, then reads the range. */
        *destination = (u8)((random * ((u8*)operands)[1]) >> 8);
        return;
    }
    case 0xEE: { /* 80021A60: signed Q12 scale, truncation toward zero. */
        u8* p = pSpriteData;
        s32 value = (s16)AnimationRead16(operands);
        s32 scale = (s16)AnimationRead16(p + 0x2C);
        s32 base = (s16)AnimationRead16(p + 0x84);
        s32 product = value * scale;
        if (product < 0) product += 0xFFF;
        value = (product >> 12) + base;
        AnimationWrite32(p + 4, (u32)value << 16);
        return;
    }
    }

    if (dispatchIndex == 0x3) {
        /* Opcode 0x8D handler (asm 8001FC34-8001FC50): latch the sprite
         * texture page from the anim package's VRAM x/y halfwords (+4/+6);
         * package pointer at +0x24. Zero operand bytes. */
        u8* p = pSpriteData;
        u8* pkg = (u8*)(uintptr_t)*(u32*)(p + 0x24);

        func_8002CC10(*(s16*)(pkg + 0x4), *(s16*)(pkg + 0x6));
        return;
    }

    if (dispatchIndex == 0xA) {
        /* Opcode 0x94 handler (asm 8001FDC8-8001FDEC + shared tail
         * .L800215A4): mode-2 sprites only - copy the parent's angle
         * (+0x32, via the +0x70 back-link) into the transform block's
         * rotation-Y halfword (+0x2), then set the matrix-dirty flag
         * (+0x3C bit 28, recomputed by func_80022038). Zero operands.
         * Other modes return without touching anything. */
        u8* p = pSpriteData;

        if ((*(u32*)(p + 0x3C) & 0x3) == 2) {
            u8* pParent = (u8*)(uintptr_t)*(u32*)(p + 0x70);
            u8* pBase = (u8*)(uintptr_t)*(u32*)(p + 0x20);

            *(u16*)(pBase + 0x2) = *(u16*)(pParent + 0x32);
            *(u32*)(p + 0x3C) |= 0x10000000;
        }
        return;
    }

    if (AnimationScriptOpcodeIsNoop(opcodeIndex)) {
        return;
    }

    if (dispatchIndex == 0x29) {
        /* Opcode 0xB3 (asm 800214AC-800214D0): write speed-index bits in
         * sprite +0xA8 from the signed operand byte (low 6 bits << 11). */
        u8* p = pSpriteData;
        u8* ops = operands;
        u32 flags = *(u32*)(p + 0xA8) & 0xFFFE07FF;

        *(u32*)(p + 0xA8) = flags | ((((s8)ops[0]) & 0x3F) << 11);
        return;
    }

    if (dispatchIndex == 0x32) {
        /* Opcode 0xBC handler (asm 800202F4-80020BAC): multi-command
         * position opcode. Operand bit 7 set selects a position SOURCE
         * (op0 & 0x3F) from jtbl_800185A8 (0x27 entries); bit 7 clear takes
         * the attach-to-parent-anchor path (.L80020AD0). Every sub-command
         * ends in the shared tail .L80020A20/.L80020A24: optional
         * camera-relative transform (ApplyMatrixSV by D_8004FBB8 + the
         * matrix translation low-halfword adds, gated on sprite flag
         * +0x3F bit 0 unless the sub-case clears it), then operand bit 6
         * selects the destination - set stores the vector halfwords to
         * +0xA0/A2/A4 (target position), clear stores (s16)<<16 into the
         * position words +0x0/+0x4/+0x8.
         *
         * Retail reaches the tail with an UNINITIALIZED stack vector on the
         * degenerate paths (sub >= 0x27, missing transform block, non-mode-1
         * sprite, NULL parent for subs 0xB-0x11); the port zero-initializes
         * the vector so those paths are deterministic.
         *
         * Player/party-relative sub-commands (1-4, 0x19-0x23) read the
         * animation system's player-sprite global D_800C3E1C and party list
         * D_800D363C, which have NO writer in the port yet (their latch
         * lives in temp1's unported sprite-spawn region) - they stay
         * fail-loud. Sub 5 divides zeroed accumulators by a stale register
         * in retail (indeterminate) and stays fail-loud too. */
        u8* p = pSpriteData;
        u32 op0 = ((u8*)operands)[0];

        if (op0 & 0x80) {
            u32 sub = op0 & 0x3F;
            s32 cameraRelative = *(u8*)(p + 0x3F) & 1;
            SVECTOR vec;

            vec.vx = 0;
            vec.vy = 0;
            vec.vz = 0;

            if (sub >= 0x27) {
                /* Retail joins the tail with an uninitialized vector; no
                 * authored script should get here. */
#ifdef XENO_PC_PORT
                fprintf(stderr, "{\"event\":\"sprite_animation_bc_unimplemented\",\"sub\":%u,\"sprite\":\"%p\"}\n", (unsigned)sub, pSpriteData);
                fflush(stderr);
#endif
                assert(0 && "func_8001FBE4 opcode 0xBC sub-command is not implemented");
                return;
            }

            switch (sub) {
            case 0x00: {
                /* 80020740: target (+0x74) position halfwords. */
                u8* t = (u8*)(uintptr_t)*(u32*)(p + 0x74);

                vec.vx = *(s16*)(t + 0x2);
                vec.vy = *(s16*)(t + 0x6);
                vec.vz = *(s16*)(t + 0xA);
                break;
            }

            case 0x01:
            case 0x02:
            case 0x03:
            case 0x04:
            case 0x19:
            case 0x1A:
            case 0x1B:
            case 0x1C:
            case 0x1D:
            case 0x1E:
            case 0x1F:
            case 0x20:
            case 0x21:
            case 0x22:
            case 0x23:
                /* Player (D_800C3E1C) / party list (D_800D363C) relative -
                 * no port writer for those globals yet (temp1 sprite-spawn
                 * region); implementing now would deref NULL (or 0/0 in the
                 * sub-2 centroid). */
#ifdef XENO_PC_PORT
                fprintf(stderr, "{\"event\":\"sprite_animation_bc_unimplemented\",\"sub\":%u,\"sprite\":\"%p\"}\n", (unsigned)sub, pSpriteData);
                fflush(stderr);
#endif
                assert(0 && "func_8001FBE4 opcode 0xBC sub-command is not implemented");
                return;

            case 0x05:
                /* 800209B8: retail zeroes the accumulator block then divides
                 * it by a STALE register (indeterminate on hardware). */
#ifdef XENO_PC_PORT
                fprintf(stderr, "{\"event\":\"sprite_animation_bc_unimplemented\",\"sub\":%u,\"sprite\":\"%p\"}\n", (unsigned)sub, pSpriteData);
                fflush(stderr);
#endif
                assert(0 && "func_8001FBE4 opcode 0xBC sub-command is not implemented");
                return;

            case 0x06:
            case 0x07: {
                /* 800205D4/800205E8: fixed vector blocks - word>>16 for X,
                 * halfwords +0x6/+0xA for Y/Z. */
                u8* blk = (sub == 0x06) ? (u8*)&D_8006F99C : (u8*)&D_8006F9AC;

                vec.vx = (s16)(*(s32*)blk >> 16);
                vec.vy = *(s16*)(blk + 0x6);
                vec.vz = *(s16*)(blk + 0xA);
                break;
            }

            case 0x08:
            case 0x09:
            case 0x0A: {
                /* 80020620/28/30 -> .L80020650: target (+0x74) anchor-table
                 * position; fixed anchor index 0xC/0xB/0xD respectively
                 * (no static table: the matching linker discards new .sdata
                 * from this TU). */
                s32 aidx = (sub == 0x08) ? 0xC : (sub == 0x09) ? 0xB : 0xD;
                u8* t = (u8*)(uintptr_t)*(u32*)(p + 0x74);

                Xeno0xBCAnchorVec(t, aidx, &vec);
                break;
            }

            case 0x0B:
            case 0x0C:
            case 0x0D:
            case 0x0E:
            case 0x0F:
            case 0x10:
            case 0x11: {
                /* 80020638: parent (+0x70) anchor-table position, anchor
                 * index = sub; NULL parent joins the tail (zero vector in
                 * the port, uninitialized in retail). */
                u8* pParent = (u8*)(uintptr_t)*(u32*)(p + 0x70);

                if (pParent != NULL) {
                    Xeno0xBCAnchorVec(pParent, (s32)sub, &vec);
                }
                break;
            }

            case 0x12:
            case 0x13:
            case 0x14:
            case 0x15: {
                /* 800204BC/F4, 8002054C/8C: target (+0x74) position with a
                 * height offset - full +0x36, mid ((0x36+0x38)/2 blend),
                 * full +0x38, and half +0x38 (ceil) respectively. */
                u8* t = (u8*)(uintptr_t)*(u32*)(p + 0x74);

                vec.vx = *(s16*)(t + 0x2);
                vec.vy = *(s16*)(t + 0x6);
                vec.vz = *(s16*)(t + 0xA);
                if (sub == 0x12) {
                    vec.vy -= *(u16*)(t + 0x36);
                } else if (sub == 0x13) {
                    s32 h36 = *(u16*)(t + 0x36);
                    s32 h38 = *(u16*)(t + 0x38);
                    s32 half = (h36 - h38) >> 1;

                    if ((h36 - h38) < 0) {
                        half = (h36 - h38 + 1) >> 1;
                    }
                    vec.vy -= (s16)(h36 - half);
                } else if (sub == 0x14) {
                    vec.vy -= *(u16*)(t + 0x38);
                } else {
                    s32 h38 = *(u16*)(t + 0x38);

                    vec.vy -= (s16)(h38 - (h38 >> 1));
                }
                break;
            }

            case 0x16: {
                /* 8002044C: parent position, camera flag cleared. */
                u8* pParent = (u8*)(uintptr_t)*(u32*)(p + 0x70);

                vec.vx = *(s16*)(pParent + 0x2);
                vec.vy = *(s16*)(pParent + 0x6);
                vec.vz = *(s16*)(pParent + 0xA);
                cameraRelative = 0;
                break;
            }

            case 0x17: {
                /* 800203E8: screen-center delta - (0xA0-ofx)<<1,
                 * (0x70-ofy)<<1, own Z; camera flag cleared. */
                long ofx;
                long ofy;

                ReadGeomOffset(&ofx, &ofy);
                vec.vx = (s16)((0xA0 - ofx) << 1);
                vec.vy = (s16)((0x70 - ofy) << 1);
                vec.vz = *(s16*)(p + 0xA);
                cameraRelative = 0;
                break;
            }

            case 0x18:
                /* .L80020428: own position, camera flag cleared. */
                vec.vx = *(s16*)(p + 0x2);
                vec.vy = *(s16*)(p + 0x6);
                vec.vz = *(s16*)(p + 0xA);
                cameraRelative = 0;
                break;

            case 0x24:
            case 0x25: {
                /* 800203B0/CC: set (0x24) / clear (0x25) the wrapper task's
                 * sticky bit (unk14 bit 30), then own position, camera flag
                 * cleared (.L80020428 shared prologue). */
                u8* pWrapper = (u8*)(uintptr_t)*(u32*)(p + 0x6C);

                if (sub == 0x24) {
                    *(u32*)(pWrapper + 0x14) |= 0x40000000;
                } else {
                    *(u32*)(pWrapper + 0x14) &= 0xBFFFFFFF;
                }
                vec.vx = *(s16*)(p + 0x2);
                vec.vy = *(s16*)(p + 0x6);
                vec.vz = *(s16*)(p + 0xA);
                cameraRelative = 0;
                break;
            }

            case 0x26: {
                /* 8002033C: track-table vector - index from A8 bits 30-31 |
                 * (AC bits 0-1)<<2, entry stride 0x1C in D_800C3EB0;
                 * X = entry+0xE, Y = 0, Z = entry+0x10. Camera flag
                 * preserved. */
                u32 idx = (*(u32*)(p + 0xA8) >> 30) |
                          ((*(u32*)(p + 0xAC) & 0x3) << 2);
                u8* e = (u8*)D_800C3EB0 + idx * 0x1C;

                vec.vx = (s16)*(u16*)(e + 0xE);
                vec.vy = 0;
                vec.vz = (s16)*(u16*)(e + 0x10);
                break;
            }
            }

            /* Shared tail .L80020A20/.L80020A24. */
            if (cameraRelative != 0) {
                /* asm 80020A28-80020A70: rotate by the camera matrix and add
                 * its translation LOW HALFWORDS (retail lhu's the .t words). */
                ApplyMatrixSV(&D_8004FBB8, &vec, &vec);
                vec.vx = (s16)((u16)vec.vx +
                               *(u16*)((u8*)&D_8004FBB8 + 0x14));
                vec.vy = (s16)((u16)vec.vy +
                               *(u16*)((u8*)&D_8004FBB8 + 0x18));
                vec.vz = (s16)((u16)vec.vz +
                               *(u16*)((u8*)&D_8004FBB8 + 0x1C));
            }
            if (op0 & 0x40) {
                *(u16*)(p + 0xA0) = (u16)vec.vx;
                *(u16*)(p + 0xA2) = (u16)vec.vy;
                *(u16*)(p + 0xA4) = (u16)vec.vz;
            } else {
                *(s32*)(p + 0x0) = (s32)vec.vx << 16;
                *(s32*)(p + 0x4) = (s32)vec.vy << 16;
                *(s32*)(p + 0x8) = (s32)vec.vz << 16;
            }
            return;
        }

        /* Operand bit 7 clear: .L80020AD0 - position this child at its
         * parent's anchor-table entry op0 (full byte), scaled by the
         * parent's +0x2C factor.  NOTE: this path's mirror flag is the
         * PARENT's +0x3C bit 3, unlike the sub-command machine's +0xAC
         * bit 2 - retail keeps them distinct. */
        {
            u8* pParent = (u8*)(uintptr_t)*(u32*)(p + 0x70);
            u8* pBase;
            u8* table;
            s32 dx = 0;
            s32 dy = 0;
            s32 scale;
            s32 sx;
            s32 sy;

            if (pParent == NULL) {
                return;
            }
            pBase = (u8*)(uintptr_t)*(u32*)(pParent + 0x20);
            if (pBase == NULL) {
                return;
            }
            if ((*(u32*)(pParent + 0x3C) & 0x3) != 1) {
                return;
            }
            table = (u8*)(uintptr_t)*(u32*)(pBase + 0x34);
            if (table != NULL) {
                u8* e = table + op0 * 8;

                dx = *(s8*)(e + 0x0);
                dy = *(s8*)(e + 0x1);
            }
            if ((*(u32*)(pParent + 0x3C) >> 3) & 0x1) {
                dx = -dx;
            }
            scale = *(s16*)(pParent + 0x2C);
            sy = dy * scale;
            if (sy < 0) {
                sy += 0xFFF;
            }
            sy >>= 12;
            sx = dx * scale;
            if (sx < 0) {
                sx += 0xFFF;
            }
            sx >>= 12;
            *(s32*)(p + 0x8) = *(s32*)(pParent + 0x8);
            *(s32*)(p + 0x0) = *(s32*)(pParent + 0x0) + (sx << 16);
            *(s32*)(p + 0x4) = *(s32*)(pParent + 0x4) + (sy << 16);
        }
        return;
    }

    if (dispatchIndex == 0x19) {
        /* Opcode 0xA3 handler (asm 800217A4-80021880): set the sprite's
         * gravity at +0x1C (consumed by the func_80022B2C integrator). When
         * A8 bit0 is set and the shared state block (+0x7C) carries a
         * gravity value at +0x4, reuse it. Otherwise: signed operand scaled
         * by +0x82 with the >>12<<5 fixed-point step, times the squared
         * inverse of the speed divisor (AC bits 7-18, same unguarded
         * 0x10000/denom as func_80023538), times (D_80059198+1)^2, with the
         * negative rounding adjustments at each step. Retail's intermediate
         * +0x1C stores are dead (no calls intervene) and collapsed here,
         * matching the 0xA0 case below. */
        u8* p = pSpriteData;
        s32 a;
        s32 inv;
        s32 sq;
        s32 v;
        s32 t;

        if ((*(u32*)(p + 0xA8) & 1) == 1) {
            u32 shared =
                *(u32*)((u8*)(uintptr_t)*(u32*)(p + 0x7C) + 0x4);
            if (shared != 0) {
                *(u32*)(p + 0x1C) = shared;
                return;
            }
        }

        a = ((s32)(s8)((u8*)operands)[0] << 6) * *(s16*)(p + 0x82);
        if (a < 0) {
            a += 0xFFF;
        }
        a = (a >> 12) << 5;

        inv = 0x10000 / (s32)((*(u32*)(p + 0xAC) >> 7) & 0xFFF);
        sq = inv * inv;
        if (sq < 0) {
            sq += 0xFF;
        }
        v = a * (sq >> 8);
        if (v < 0) {
            v += 0xFF;
        }
        t = D_80059198 + 1;
        *(s32*)(p + 0x1C) = (v >> 8) * (t * t);
        return;
    }

    if (dispatchIndex == 0x1C) {
        /* Opcode 0xA6 handler (asm 800218DC-80021954): mode-A8-bit0 sprites
         * ignore it. Otherwise scale the signed operand by 16, the frame
         * factor, and sprite +0x82; round before >>12, convert to 16.16,
         * divide by the AC speed divisor, and add to sprite +0x10. */
        u8* p = pSpriteData;
        s32 value;
        s32 divisor;

        if ((*(u32*)(p + 0xA8) & 1) == 1) {
            return;
        }

        value = (s32)(s8)((u8*)operands)[0] * 16;
        value *= D_80059198 + 1;
        value *= *(s16*)(p + 0x82);
        if (value < 0) {
            value += 0xFFF;
        }
        value = (value >> 12) << 16;
        divisor = (s32)((*(u32*)(p + 0xAC) >> 7) & 0xFFF);
        *(s32*)(p + 0x10) += value / divisor;
        return;
    }

    if (dispatchIndex == 0x16) {
        /* Opcode 0xA0 handler (original asm at 0x80021958): fixed-point value
           from signed operand byte 0, (D_80059198 + 1), and signed pData+0x82,
           with the negative rounding adjustment, stored to pData+0x18, then
           func_80022974. */
        u8* p = pSpriteData;
        u8 op0 = ((u8*)operands)[0];

        s32 v = ((s32)(s8)op0 * 16) * (D_80059198 + 1);
        s32 val = v * *(s16*)(p + 0x82);

        if (val < 0) {
            val += 0xFFF;
        }

        *(u32*)(p + 0x18) = (val >> 12) << 8;
        func_80022974(p);
        return;
    }

    if (dispatchIndex == 0x0C) {
        /* Opcode 0x96 handler (asm 8001FEEC-8001FEFC): unlink work-list
         * entries owned by the sprite's self pointer at +0x6C. Zero operands. */
        u8* p = pSpriteData;
        func_8001CE74((void*)(uintptr_t)*(u32*)(p + 0x6C));
        return;
    }

    if (dispatchIndex == 0x3C) {
        /* Opcode 0xC6 handler (asm 8001FC54-8001FC74): if A8 bit0 is set,
         * store operand byte 0 as a halfword at *(pData+0x7C)+0xC. */
        u8* p = pSpriteData;

        if ((*(u32*)(p + 0xA8) & 1) == 1) {
            u8* pState = (u8*)(uintptr_t)*(u32*)(p + 0x7C);
            *(u16*)(pState + 0xC) = ((u8*)operands)[0];
        }
        return;
    }

    if (dispatchIndex == 0x44) {
        /* Opcode 0xCE handler (asm 8001FFC0-80020088): packed LE-s16
         * operand bits 0-8 encode a Y offset in units of 8, bits 9-11
         * select an 8-byte sub-entry (zero selects the transform block),
         * and bit 12 selects set rather than add. AC bit 2 mirrors the
         * offset. Only model writes join .L800215A4 and mark the transform
         * matrix dirty via +0x3C bit 28. */
        u8* p = pSpriteData;
        u8* ops = operands;
        u32 encoded = AnimationRead16(ops);
        u8* pBase = (u8*)(uintptr_t)AnimationRead32(p + 0x20);
        u32 subIndex = (encoded >> 9) & 0x7;
        s32 value = (encoded & 0x1FF) << 3;
        u8* pY;

        if (pBase == NULL) {
            return;
        }
        if ((AnimationRead32(p + 0xAC) & 0x4) != 0) {
            value = -value;
        }

        if (subIndex != 0) {
            u8* pEntries =
                (u8*)(uintptr_t)AnimationRead32(pBase + 0x34);
            if (pEntries == NULL) {
                return;
            }
            pY = pEntries + subIndex * 8 + 4;
        } else {
            pY = pBase + 0x2;
        }

        if ((encoded & 0x1000) != 0) {
            AnimationWrite16(pY, (u16)value);
        } else {
            AnimationWrite16(pY, (u16)(AnimationRead16(pY) + value));
        }
        if (subIndex == 0)
            AnimationWrite32(p + 0x3C, AnimationRead32(p + 0x3C) | 0x10000000u);
        return;
    }

    if (dispatchIndex == 0x56) {
        /* Opcode 0xE0 handler (asm 80021440-80021464): spawn a child sprite
         * whose animation script sits at a 16-bit sign-extended little-endian
         * offset from the operand bytes; the anim package pointer at +0x24
         * rides along. Return value (the child) is dropped, as in retail. */
        u8* p = pSpriteData;
        u8* ops = operands;
        s32 rel = ((s32)(s8)ops[1] << 8) + ops[0];

        func_80023B84(p, ops + rel, (void*)(uintptr_t)*(u32*)(p + 0x24));
        return;
    }

    if (dispatchIndex == 0x6B || dispatchIndex == 0x6C) {
        /* Opcode 0xF5 handler (asm 80021140-800211D8) and its sibling 0xF6
         * (asm 800211DC-80021270): (re)load the sprite's model data from a
         * script-embedded blob at a 24-bit sign-extended LE offset from the
         * operand bytes. 0xF5 relocates via func_8002C59C and uses the blob
         * directly as the model header; 0xF6 relocates via func_8002C3E8 and
         * the header sits at blob+0x10. Then, under heap user 5: free the
         * old model buffer (+0x2C of the transform block), rebuild it with
         * func_8002CB54 (+0x2C/+0x30 out-params), fix up internal pointers
         * with func_8002C8CC, mirror the buffer (+0x30 <- +0x2C, header+0x34
         * bytes), and latch the header into the transform block at +0x34.
         * Retail does not restore the heap user (shared tail .L8002130C is
         * only the +0x34 store). */
        u8* p = pSpriteData;
        u8* ops = operands;
        s32 rel = ((s32)(s8)ops[2] << 16) + (ops[1] << 8) + ops[0];
        u8* target = ops + rel;
        u8* pModelHdr;
        u8* pBase;
        u32 oldBuffer;

        HeapChangeCurrentUser(5, 0);
        if (dispatchIndex == 0x6B) {
            func_8002C59C(target);
            pModelHdr = target;
        } else {
            func_8002C3E8(target);
            pModelHdr = target + 0x10;
        }

        pBase = (u8*)(uintptr_t)*(u32*)(p + 0x20);
        oldBuffer = *(u32*)(pBase + 0x2C);
        if (oldBuffer != 0) {
            HeapFree((void*)(uintptr_t)oldBuffer);
        }
        func_8002CB54(pModelHdr, (u32*)(pBase + 0x2C), (u32*)(pBase + 0x30));
        func_8002C8CC(pModelHdr, (void*)(uintptr_t)*(u32*)(pBase + 0x2C), 0);
        memcpy((void*)(uintptr_t)*(u32*)(pBase + 0x30),
               (void*)(uintptr_t)*(u32*)(pBase + 0x2C),
               *(u32*)(pModelHdr + 0x34));
        *(u32*)(pBase + 0x34) = (u32)(uintptr_t)pModelHdr;
        return;
    }

    if (dispatchIndex == 0x72) {
        /* Opcode 0xFC handler (asm 8001FE64-8001FEDC): upload image data
         * embedded in the animation script to VRAM. The operand bytes form a
         * 24-bit little-endian offset (byte 2 sign-extended) from the operand
         * pointer to the image blob; VRAM x/y are the anim-package (+0x24)
         * halfwords +4/+6. All three are handed to func_8001FB30 through the
         * D_800592E4/E8/EA sbss slots. Retail repoints $sp into the scratch
         * block around the call (the upload path needs a deeper stack); the
         * native stack needs no switch, but the alloc/free pair is kept so
         * heap state stays retail-exact. */
        u8* p = pSpriteData;
        u8* ops = operands;
        void* scratch = HeapAlloc(0x2000, 0);
        u8* pkg = (u8*)(uintptr_t)*(u32*)(p + 0x24);
        s32 rel = ((s32)(s8)ops[2] << 16) + (ops[1] << 8) + ops[0];

        D_800592E4 = (u32)(uintptr_t)(ops + rel);
        D_800592E8 = *(s16*)(pkg + 0x4);
        D_800592EA = *(s16*)(pkg + 0x6);
        func_8001FB30();
        HeapFree(scratch);
        return;
    }

    if (dispatchIndex == 0x26 || dispatchIndex == 0x2F) {
        /* Opcodes 0xB0 (8001FCFC) and 0xB9 (8001FCF0): play SFX operand
         * byte 0 on a sound bank -- the sound module's selected bank
         * (D_8005919C) for 0xB0, the sprite's own bank (+0x50) for 0xB9;
         * both fall into the shared tail at 8001FD04. A null bank is a
         * retail no-op. The bank id halfword at +0x14 is the packed id's
         * high half (lhu; sll 16; or with the operand byte). */
        u8* p = pSpriteData;
        u32 bank = (dispatchIndex == 0x2F) ? AnimationRead32(p + 0x50)
                                           : AnimationSoundBankSelector();
        u16 id;
        if (bank == 0) return;
        id = AnimationRead16(AnimationSoundBankPointer(bank) + 0x14);
        func_80039E60((s32)(((u8*)operands)[0] | ((u32)id << 16)));
        return;
    }

    if (dispatchIndex == 0x1E) {
        /* Opcode 0xA8 (8002176C..80021790): nudge the facing angle at +0x32
         * by (s8)operand << 4 (the store sits in the call's delay slot, so
         * it lands before func_80022974 rebuilds the velocity from it). */
        u8* p = pSpriteData;
        u16 angle = AnimationRead16(p + 0x32);
        angle = (u16)(angle + (u16)((s32)(s8)((u8*)operands)[0] * 16));
        AnimationWrite16(p + 0x32, angle);
        func_80022974(p);
        return;
    }

    if (dispatchIndex == 0x45) {
        /* Opcode 0xCF (8002008C..80020154, tail 800215A4): adjust a model
         * part angle.  Operand halfword v (byte 1 sign-extended): bits 0-8 are
         * the amount (<<3), bits 9-11 select a part in the table at
         * model+0x34 (entry stride 8, halfword +6; part 0 is the model's own
         * halfword at +4), bit 12 selects set instead of add.  Sprite flag
         * +0xAC bit 2 negates the amount.  Null model or table -> no-op; the
         * part-0 paths also raise +0x3C bit 28. */
        u8* p = pSpriteData;
        u8* ops = operands;
        s32 v = (s32)ops[0] | ((s32)(s8)ops[1] * 256);
        u8* model = (u8*)(uintptr_t)AnimationRead32(p + 0x20);
        u32 a2 = (u32)v & 0xFFFFu;
        u32 index = (a2 >> 9) & 7u;
        s32 amount = (s32)(((u32)v & 0x1FFu) << 3);
        if (model == NULL) return;
        if ((AnimationRead32(p + 0xAC) >> 2) & 1u) amount = -amount;
        if (index != 0) {
            u8* table = (u8*)(uintptr_t)AnimationRead32(model + 0x34);
            u8* slot;
            if (table == NULL) return;
            slot = table + index * 8 + 6;
            AnimationWrite16(slot, (a2 & 0x1000u)
                                       ? (u16)amount
                                       : (u16)(AnimationRead16(slot) + amount));
            return;
        }
        AnimationWrite16(model + 4, (a2 & 0x1000u)
                                        ? (u16)amount
                                        : (u16)(AnimationRead16(model + 4) + amount));
        AnimationWrite32(p + 0x3C, AnimationRead32(p + 0x3C) | 0x10000000u);
        return;
    }

    if (dispatchIndex == 0x5C) {
        /* Opcode 0xE6 (80020DCC..80020DE4): resolve the variable slot,
         * load the operand before either store, clear the next byte, then
         * store the value. Preserve this order when operands alias the slot. */
        u8* slot = func_8001FBA4((SpriteData*)pSpriteData, (u8*)operands);
        u8 value = ((u8*)operands)[1];
        slot[1] = 0;
        slot[0] = value;
        return;
    }

    if (dispatchIndex == 0x55) {
        /* Opcode 0xDF (80020DB4..80020DC8): store operand byte 1 into the
         * script variable slot selected by operand byte 0 (func_8001FBA4:
         * signed stack-relative index, or 0x80|n into the +0x88 register
         * block). Pure data write, no side effects. */
        u8* slot = func_8001FBA4((SpriteData*)pSpriteData, (u8*)operands);
        *slot = ((u8*)operands)[1];
        return;
    }

    /* Failure-only host telemetry, no reads through possibly invalid inputs.
     * Retain the original hard stop; never treat this as a successful skip. */
#ifdef XENO_PC_PORT
    fprintf(stderr,
            "{\"event\":\"sprite_animation_unimplemented\",\"raw_opcode\":%u,"
            "\"opcode\":%u,\"dispatch_index\":%u,\"sprite\":\"%p\",\"operands\":\"%p\"}\n",
            (unsigned)opcodeIndex, (unsigned)(u8)opcodeIndex,
            (unsigned)dispatchIndex, pSpriteData, operands);
    fflush(stderr);
#endif
    assert(0 && "func_8001FBE4 dispatch path is not implemented");
}

s32 func_80021AD8(s32 color, s32 value) {
    color += value;
    if (color >= 0x100) {
        color = 0xFF;
    } else if (color < 0) {
        color = 0;
    }
    return color;
}

void func_80021B04(void* arg0, s16 arg1, s16 arg2, s16 arg3) {
    *(s16*)((u8*)arg0 + 0x0) = arg1;
    *(s16*)((u8*)arg0 + 0x2) = arg2;
    *(s16*)((u8*)arg0 + 0x4) = arg3;
}

void func_80021B14(void* arg0, s32 arg1, s32 arg2, s32 arg3) {
    *(s32*)((u8*)arg0 + 0x0) = arg1;
    *(s32*)((u8*)arg0 + 0x4) = arg2;
    *(s32*)((u8*)arg0 + 0x8) = arg3;
}

void func_80021B24(void* arg0, void* arg1) {
    *(u16*)((u8*)arg0 + 0x0) = *(u16*)((u8*)arg1 + 0x0);
    *(u16*)((u8*)arg0 + 0x2) = *(u16*)((u8*)arg1 + 0x2);
    *(u16*)((u8*)arg0 + 0x4) = *(u16*)((u8*)arg1 + 0x4);
}

void func_80021B48(void* arg0, void* arg1) {
    *(s32*)((u8*)arg0 + 0x0) = *(s32*)((u8*)arg1 + 0x0);
    *(s32*)((u8*)arg0 + 0x4) = *(s32*)((u8*)arg1 + 0x4);
    *(s32*)((u8*)arg0 + 0x8) = *(s32*)((u8*)arg1 + 0x8);
}

void func_80021B6C(SpriteData* pSpriteData) {
    pSpriteData->prim |= 1;
    func_8001F6B0(pSpriteData);
}

void SpriteSetColor(SpriteData *pSpriteData, u8 red, u8 green, u8 blue) {
    pSpriteData->red = red;
    pSpriteData->green = green;
    pSpriteData->blue = blue;
    pSpriteData->prim &= 0xFE;
    func_8001F6B0(pSpriteData);
}

void func_80021BCC(void* arg0, u32 arg1) {
    u32* p = (u32*)((u8*)arg0 + 0xAC);
    *p = (*p & ~0x7FF80) | ((arg1 & 0xFFF) << 7);
}

void SpriteSetSpecialAnimFile(SpriteData* pSpriteData, void* pAnimFile) {
#ifdef XENO_PC_PORT
    /* Field sprites are allocated as raw 0x164-byte PSX records. Host pointer
     * widening moves the typed member to +0x60, but retail stores a 32-bit
     * animation pointer at +0x4C. Keep the native boundary PSX-layout exact. */
    *(u32*)((u8*)pSpriteData + 0x4C) = (u32)(uintptr_t)pAnimFile;
#else
    pSpriteData->pSpecialAnimFile = pAnimFile;
#endif
}

void func_80021BF8(void* arg0, s32 arg1) {
    *(s32*)((u8*)arg0 + 0x68) = arg1;
}

void func_80021C00(void* arg0, u32 arg1) {
    u32* p = (u32*)((u8*)arg0 + 0x40);
    *p = (*p & ~0x1F00) | ((arg1 & 0x1F) << 8);
}

u_char AnimScriptStackPopU8(SpriteData* pSpriteData) {
    u8* pData = (u8*)pSpriteData;
    u_char nStackValue;
    s8 idx = (s8)pData[0x8C];

    nStackValue = pData[0x8E + idx];
    pData[0x8C]++;
    return nStackValue;
}


s32 AnimScriptStackPopU16(SpriteData* pSpriteData) {
    /* Retail V0 is sign-extended to 32 bits; the native bridge consumes
     * the complete register, so a host short return is insufficient. */
    u8* pData = (u8*)pSpriteData;
    s8 idx = (s8)pData[0x8C];
    u16 lo = pData[0x8E + idx];
    u16 hi = pData[0x8F + idx];
    pData[0x8C] += 2;
    return (s16)(lo + hi * 256);
}

s32 AnimScriptStackPopU24(SpriteData* pSpriteData) {
    u8* pData = (u8*)pSpriteData;
    s8 idx = (s8)pData[0x8C];
    u32 b0 = pData[0x8E + idx];
    u32 b1 = pData[0x8F + idx];
    u32 b2 = pData[0x90 + idx];
    pData[0x8C] += 3;
    return b0 + b1 * 256 + b2 * 65536;
}

void AnimScriptStackPushU8(SpriteData* pSpriteData, u8 value) {
    u8* pData = (u8*)pSpriteData;
    s8 idx = (s8)--pData[0x8C];
    pData[0x8E + idx] = value;
}

void AnimScriptStackPushU16(SpriteData* pSpriteData, u16 value) {
    u8* pData = (u8*)pSpriteData;
    s8 idx = (s8)(pData[0x8C] -= 2);
    pData[0x8E + idx] = (u8)value;
    idx = (s8)pData[0x8C];
    pData[0x8F + idx] = (u8)(value >> 8);
}

void AnimScriptStackPushU24(SpriteData* pSpriteData, s32 value) {
    u8* pData = (u8*)pSpriteData;
    s8 idx = (s8)(pData[0x8C] -= 3);
    pData[0x8E + idx] = (u8)value;
    idx = (s8)pData[0x8C];
    pData[0x8F + idx] = (u8)(value >> 8);
    idx = (s8)pData[0x8C];
    pData[0x90 + idx] = (u8)(value >> 16);
}

void func_80021D3C(void* arg0, s32 arg1, s32 arg2) {
    *(s32*)((u8*)arg0 + 0x8) = arg2 << 16;
    *(s32*)((u8*)arg0 + 0x0) = arg1 << 16;
}

extern s32 D_80059198;
extern void func_800245D8(void*, s32);
extern void AnimScriptTick(void*);

void func_80021D50(void* arg0, void* arg1) {
    s32 savedD80059198 = D_80059198;
    void* pData;
    D_80059198 = 0;
    *(u16*)((u8*)arg0 + 0x80) = *(u16*)((u8*)arg1 + 0x10);
    *(u8*)((u8*)arg0 + 0xAF) = *(u8*)((u8*)arg1 + 0x14);
    *(u8*)((u8*)arg0 + 0xB0) = *(u8*)((u8*)arg1 + 0x16);
    pData = *(void**)((u8*)arg0 + 0x20);
    *(u16*)((u8*)pData + 6) = *(u16*)((u8*)arg1 + 0x24);
    pData = *(void**)((u8*)arg0 + 0x20);
    *(u16*)((u8*)pData + 8) = *(u16*)((u8*)arg1 + 0x26);
    pData = *(void**)((u8*)arg0 + 0x20);
    *(u16*)((u8*)pData + 0xA) = *(u16*)((u8*)arg1 + 0x28);
    *(u16*)((u8*)arg0 + 0x82) = *(u16*)((u8*)arg1 + 0x2C);
    *(u16*)((u8*)arg0 + 0x2C) = *(u16*)((u8*)arg1 + 0x2A);
    func_800245D8(arg0, *(s8*)((u8*)arg0 + 0xAF));
    {
        s16 target = *(s16*)((u8*)arg1 + 0x18);
        while (((*(u32*)((u8*)arg0 + 0xA8) >> 22) & 0x3F) != target) {
            AnimScriptTick(arg0);
            *(u32*)((u8*)arg0 + 0x00) += *(u32*)((u8*)arg0 + 0x0C);
            *(u32*)((u8*)arg0 + 0x08) += *(u32*)((u8*)arg0 + 0x14);
            *(u32*)((u8*)arg0 + 0x04) += *(u32*)((u8*)arg0 + 0x10);
            *(u32*)((u8*)arg0 + 0x10) += *(u32*)((u8*)arg0 + 0x1C);
        }
    }
    *(u32*)((u8*)arg0 + 0x00) = *(u32*)((u8*)arg1 + 0x00);
    *(u32*)((u8*)arg0 + 0x04) = *(u32*)((u8*)arg1 + 0x04);
    *(u32*)((u8*)arg0 + 0x08) = *(u32*)((u8*)arg1 + 0x08);
    {
        void* pTable = *(void**)((u8*)arg0 + 0x7C);
        *(u32*)pTable = *(u32*)((u8*)arg1 + 0x1C);
        pTable = *(void**)((u8*)arg0 + 0x7C);
        *(u32*)((u8*)pTable + 4) = *(u32*)((u8*)arg1 + 0x20);
    }
    D_80059198 = savedD80059198;
}

void func_80021EBC(void* arg0, void* arg1) {
    u8* src = arg0;
    u8* dst = arg1;
    u8* src7C = (u8*)(uintptr_t)*(u32*)(src + 0x7C);
    u8* src20 = (u8*)(uintptr_t)*(u32*)(src + 0x20);
    s32 flagsA8 = *(s32*)(src + 0xA8);

    *(s32*)(dst + 0x00) = *(s32*)(src + 0x00);
    *(s32*)(dst + 0x04) = *(s32*)(src + 0x04);
    *(s32*)(dst + 0x08) = *(s32*)(src + 0x08);
    *(s16*)(dst + 0x10) = *(u16*)(src + 0x80);
    *(s16*)(dst + 0x12) = (flagsA8 >> 11) & 0x3F;
    *(s16*)(dst + 0x14) = (s8)*(u8*)(src + 0xAF);
    *(s16*)(dst + 0x16) = (s8)*(u8*)(src + 0xB0);
    *(s16*)(dst + 0x18) = (flagsA8 >> 22) & 0x3F;
    *(s32*)(dst + 0x1C) = *(s32*)(src7C + 0x00);
    *(s32*)(dst + 0x20) = *(s32*)(src7C + 0x04);
    *(s16*)(dst + 0x24) = *(u16*)(src20 + 0x06);
    *(s16*)(dst + 0x26) = *(u16*)(src20 + 0x08);
    *(s16*)(dst + 0x28) = *(u16*)(src20 + 0x0A);
    *(s16*)(dst + 0x2C) = *(u16*)(src + 0x82);
    *(s16*)(dst + 0x2A) = *(u16*)(src + 0x2C);
}

void func_80021FB8(u8* arg0, u8 arg1) {
    arg0[0xB0] = arg1;
}

extern void func_80022974(void* arg0);

void func_80021FC0(void* a0, s32 a1) {
    *(s32*)((u8*)a0 + 0x18) = a1;
    func_80022974(a0);
}

void func_80021FE0(void* a0, s16 a1) {
    AnimationWrite16((u8*)a0 + 0x32, (u16)a1);
    func_80022974(a0);
}

void SpriteSetScale(SpriteData* pSpriteData, short scale) {
    u8* pData = (u8*)pSpriteData;
    u8* pBase = (u8*)(uintptr_t)AnimationRead32(pData + 0x20);

    if (pBase) {
        /* Retail 80022000: these packed fields may overlap. Read flags
         * after the scale stores, including when pBase aliases pData. */
        AnimationWrite16(pData + 0x2C, (u16)scale);
        AnimationWrite16(pBase + 0xA, (u16)scale);
        AnimationWrite16(pBase + 0x8, (u16)scale);
        AnimationWrite16(pBase + 0x6, (u16)scale);
        AnimationWrite32(pData + 0x3C, AnimationRead32(pData + 0x3C) | 0x10000000u);
    }
}

// Recompute transform matrix if flag is set
void func_80022038(SpriteData* pSpriteData) {
    u8* pData = (u8*)pSpriteData;

    if ((*(u32*)(pData + 0x3C) >> 28) & 0x1) {
        SpriteComputeTransformMatrix(pSpriteData);
        *(u32*)(pData + 0x3C) &= ~0x10000000;
    }
}

extern MATRIX D_80018644;

void SpriteComputeTransformMatrix(SpriteData* pSpriteData) {
    u8* pData = (u8*)pSpriteData;
    u8* pBase = (u8*)(uintptr_t)*(u32*)(pData + 0x20);
    VECTOR scale;
    VECTOR scale2;
    MATRIX rotationMatrix;
    MATRIX transformMatrix;
    
    if (!(*(u32*)(pData + 0x40) & 1)) {
        scale.vx = *(s16*)(pBase + 0x6);
        scale.vy = *(s16*)(pBase + 0x8);
        scale.vz = *(s16*)(pBase + 0xA);
        RotMatrix((SVECTOR*)pBase, (MATRIX*)(pBase + 0xC));
        ScaleMatrixL((MATRIX*)(pBase + 0xC), &scale);
    } else {
        transformMatrix = D_80018644;
        scale2.vx = *(s16*)(pBase + 0x6);
        scale2.vy = *(s16*)(pBase + 0x8);
        scale2.vz = *(s16*)(pBase + 0xA);
        ScaleMatrixL(&transformMatrix, &scale2);
        RotMatrix((SVECTOR*)pBase, &rotationMatrix);
        MulMatrix0(&rotationMatrix, &transformMatrix, (MATRIX*)(pBase + 0xC));
    }
    if (*(u16*)(pData + 0x3A)) {
        scale.vx = *(u16*)(pData + 0x3A) >> 1;
        scale.vy = *(u16*)(pData + 0x3A) >> 1;
        scale.vz = *(u16*)(pData + 0x3A) >> 1;
        ScaleMatrix((MATRIX*)(pBase + 0xC), &scale);
    }
}

extern u8 D_800591AD;
extern u8 D_800591B0;
extern u8 D_800591B3;

extern s32 func_8001EE68(void*);

// Set animation package
void func_80022224(SpriteAnimPackage* pAnimPackage, void* pAnimPackageFile, SVEC2 tex, SVEC2 clut, u32 arg4) {
    u8* pPackage = (u8*)pAnimPackage;
    u8* pFile = pAnimPackageFile;

    *(SVEC2*)(pPackage + 0x4) = tex;
    *(SVEC2*)(pPackage + 0x8) = clut;
    *(u32*)(pPackage + 0xC) = (u32)(uintptr_t)(pFile + *(u32*)(pFile + 0xC));
    *(u32*)(pPackage + 0x0) = (u32)(uintptr_t)(pFile + *(u32*)(pFile + 0x8));
    D_800591B0 = 0;
    *(u32*)(pPackage + 0x10) = (u32)(uintptr_t)(pFile + *(u32*)(pFile + 0x4));

    if (D_800591AD) {
        u8 numAnimations = (*(u16*)(uintptr_t)*(u32*)(pPackage + 0x10) >> 6) & 0x3F;
        if (numAnimations != 0) {
            D_800591B3 = numAnimations;
        }
    }
}

void func_800222BC(SpriteData* pSprite, SpriteAnimPackageFileHeader* pAnimPackageFile) {
    u8* pSpriteData = (u8*)pSprite;
    u8* pAnimPackage;

    pAnimPackage = (u8*)(uintptr_t)*(u32*)(pSpriteData + 0x24);
    if (pAnimPackageFile) {
        if (pAnimPackageFile != (void*)(uintptr_t)*(u32*)(pSpriteData + 0x44)) {
            func_80022224((SpriteAnimPackage*)pAnimPackage,
                          pAnimPackageFile,
                          *(SVEC2*)(pAnimPackage + 0x4),
                          *(SVEC2*)(pAnimPackage + 0x8),
                          (*(u32*)(pSpriteData + 0x3C) >> 20) & 0xF);
            *(u32*)(pSpriteData + 0x44) = (u32)(uintptr_t)pAnimPackageFile;
            *(u32*)(pSpriteData + 0x3C) |= 0x40000000;
        }

        // Is battle?
        if (D_800591AD) {
            // Is not VRAM Pre-backed?
            if (!func_8001EE68((void*)(uintptr_t)*(u32*)(pAnimPackage + 0x0))) {
                *(s16*)(pAnimPackage + 0x6) = 0x100;
                *(s16*)(pAnimPackage + 0x4) = 0x300;
                return;
            }
            *(u32*)(pAnimPackage + 0x4) = *(u32*)((u8*)(uintptr_t)*(u32*)(pSpriteData + 0x7C) + 0xE);
        }
    }
}

extern void func_80022660(void* pSpriteData, void* pBytecode, s32 arg2);
extern void func_80022D44(void* pSpriteData);

void func_800223B0(void* pSpriteData, s16 arg1) {
    u8* pData = pSpriteData;
    u8* pAnim;
    s32 oldDirection;
    s32 direction;
    s32 mode;
    u32 flagsA8;
    u32 flagsAC;
    u32 flags3C;

    *(s16*)(pData + 0x80) = arg1;
    oldDirection = (*(u32*)(pData + 0xA8) >> 17) & 0x7;

    if (((arg1 + 0x400) & 0x1) != 0) {
        *(u32*)(pData + 0xAC) |= 0x4;
    } else {
        *(u32*)(pData + 0xAC) &= ~0x4;
    }

    if (*(u32*)(pData + 0x48) == 0) {
        return;
    }

    mode = (*(u32*)(pData + 0xA8) >> 20) & 0x3;
    pAnim = (u8*)(uintptr_t)*(u32*)(pData + 0x58);

    if (mode == 0) {
        if (((arg1 + 0x400) & 0xFFF) < 0x801) {
            *(u32*)(pData + 0xAC) &= ~0x4;
        } else {
            *(u32*)(pData + 0xAC) |= 0x4;
        }

        *(u32*)(pData + 0xA8) &= 0xFFF1FFFF;
        *(u32*)(pData + 0x5C) = (u32)(uintptr_t)(pAnim + 0x6);
        *(u32*)(pData + 0x54) = (u32)(uintptr_t)(pAnim + *(u16*)(pAnim + 0x4) + 0x4);
    } else if (mode == 1) {
        direction = ((arg1 + 0x600) >> 10) & 0x3;
        if (direction < 3) {
            *(u32*)(pData + 0xAC) &= ~0x4;
            *(u32*)(pData + 0x54) =
                (u32)(uintptr_t)(pAnim + (direction * 2) + 0x4 +
                                 *(u16*)(pAnim + (direction * 2) + 0x4));
        } else {
            *(u32*)(pData + 0xAC) |= 0x4;
            *(u32*)(pData + 0x54) = (u32)(uintptr_t)(pAnim + *(u16*)(pAnim + 0x6) + 0x6);
        }

        *(u32*)(pData + 0xA8) =
            (*(u32*)(pData + 0xA8) & 0xFFF1FFFF) | ((direction & 0x7) << 17);
    } else if (mode == 2) {
        direction = ((arg1 + 0x500) >> 9) & 0x7;
        if (direction < 5) {
            *(u32*)(pData + 0xAC) &= ~0x4;
            *(u32*)(pData + 0x54) =
                (u32)(uintptr_t)(pAnim + (direction * 2) + 0x4 +
                                 *(u16*)(pAnim + (direction * 2) + 0x4));
        } else {
            direction = (direction - 5) ^ 0x3;
            *(u32*)(pData + 0xAC) |= 0x4;
            *(u32*)(pData + 0x54) =
                (u32)(uintptr_t)(pAnim + (direction * 2) + 0x4 +
                                 *(u16*)(pAnim + (direction * 2) + 0x4));
        }

        *(u32*)(pData + 0xA8) =
            (*(u32*)(pData + 0xA8) & 0xFFF1FFFF) | ((direction & 0x7) << 17);
    }

    flagsA8 = *(u32*)(pData + 0xA8);
    if (oldDirection != ((flagsA8 >> 17) & 0x7)) {
        void* pOldBytecode = (void*)(uintptr_t)*(u32*)(pData + 0x64);
        s16 waitTimer = *(s16*)(pData + 0x9E);

        *(u32*)(pData + 0xA8) = (flagsA8 | 0x1F800) & 0xF03FFFFF;
        pAnim = (u8*)(uintptr_t)*(u32*)(pData + 0x58);
        *(u32*)(pData + 0x64) = (u32)(uintptr_t)(pAnim + *(u16*)(pAnim + 0x2) + 0x2);
        func_80022660(pData, pOldBytecode, (flagsA8 >> 22) & 0x3F);
        *(s16*)(pData + 0x9E) = waitTimer;
    }

    flags3C = *(u32*)(pData + 0x3C) & ~0x8;
    flagsAC = *(u32*)(pData + 0xAC);
    flags3C |= ((((flagsAC >> 3) & 0x1) ^ ((flagsAC >> 2) & 0x1)) << 3);
    *(u32*)(pData + 0x3C) = flags3C;
}

/* Animation bytecode length table. Retail lives in sdata
 * (asm/slus_006.64/data/3F290.sdata.s, dlabel D_8004FC40). Matching build
 * links that blob; the PC port needs a real definition (an auto-stubbed
 * zero table would advance the bytecode PC by 0 and hang the interpreter). */
#ifdef XENO_PC_PORT
const u8 D_8004FC40[256] = {
    16, 231, 46, 112, 80, 16, 60, 112, 255, 127, 80, 144, 160, 16, 88, 16,  /* 0x00 */
    251, 80, 144, 156, 16, 22, 80, 160, 14, 48, 160, 48, 82, 48, 134, 48,  /* 0x10 */
    31, 162, 176, 80, 208, 156, 48, 144, 48, 80, 48, 25,  0, 25, 63, 80,  /* 0x20 */
    128, 188, 48, 242, 48, 160, 144, 80, 144, 240, 144, 255, 127, 251, 62, 49,  /* 0x30 */
    80, 240, 24, 68, 65, 34, 49, 80, 80, 160, 240, 240, 48, 255, 142, 49,  /* 0x40 */
    18, 145, 226, 80, 240, 240, 240, 240, 80, 240, 80, 112, 160, 80, 191, 162,  /* 0x50 */
    112, 78, 48, 230, 17, 144, 241, 144, 209, 48, 146, 14, 48, 34, 254, 17,  /* 0x60 */
    48, 98, 46, 82,  4, 48, 48, 18, 50, 50, 48, 242, 124, 50,  0,  0,  /* 0x70 */
     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  /* 0x80 */
     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  /* 0x90 */
     2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  /* 0xA0 */
     2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  /* 0xB0 */
     2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  3,  /* 0xC0 */
     3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  /* 0xD0 */
     3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  /* 0xE0 */
     3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  /* 0xF0 */
};
#else
extern u8 D_8004FC40[256];
#endif

// Run Sprite Animation VM
void func_80022660(void* pSpriteData, void* pBytecode, s32 arg2) {
    u8* pData = pSpriteData;
    /* asm $s3: delay from the last 0x00-0x3F op, reused by 0x40-0x7F. */
    s32 delay = 0;

    while (1) {
        u8* pc = (u8*)(uintptr_t)*(u32*)(pData + 0x64);
        u8 opcode;
        u32 flags;
        u32 subIndex;

        if (pc == pBytecode && ((*(u32*)(pData + 0xA8) >> 22) & 0x3F) == (u32)arg2) {
            return;
        }

        opcode = *pc;
        if (opcode < 0x10 || (opcode >= 0x20 && opcode < 0x30)) {
            /* asm 12ED4-12F28: one-byte frame-step families. 0x00-0x0F steps
             * to the next sprite frame (curFrame+1), 0x20-0x2F to the
             * previous (curFrame-1); both then run the same shared delay
             * tail as the 0x30-0x3F case (delay = (op & 0xF) + 1). */
            s32 step = (opcode < 0x10) ? 1 : -1;

            *(u32*)(pData + 0x64) = (u32)(uintptr_t)(pc + 1);
            func_8001D2B0(pData, (s16)(*(u16*)(pData + 0x34) + step));

            delay = (opcode & 0xF) + 1;
            *(s16*)(pData + 0x9E) = *(u16*)(pData + 0x9E) + delay;

            flags = *(u32*)(pData + 0xA8) & 0xF03FFFFF;
            subIndex = (((*(u32*)(pData + 0xA8) >> 22) & 0x3F) + 1) & 0x3F;
            *(u32*)(pData + 0xA8) = flags | (subIndex << 22);

            if (subIndex == 0) {
                flags = *(u32*)(pData + 0xA8) & 0xF03FFFFF;
                subIndex = (((*(u32*)(pData + 0xA8) >> 22) & 0x3F) - 1) & 0x3F;
                *(u32*)(pData + 0xA8) = flags | (subIndex << 22);
            }
            continue;
        }

        if (opcode >= 0x10 && opcode < 0x20) {
            *(u32*)(pData + 0x64) = (u32)(uintptr_t)(pc + 1);
            flags = *(u32*)(pData + 0xA8);
            flags = (flags & 0xFFFE07FF) |
                    (((((flags >> 11) & 0x3F) + 1) & 0x3F) << 11);
            *(u32*)(pData + 0xA8) = flags;
            func_80022D44(pData);

            delay = (opcode & 0xF) + 1;
            *(s16*)(pData + 0x9E) = *(u16*)(pData + 0x9E) + delay;

            flags = *(u32*)(pData + 0xA8) & 0xF03FFFFF;
            subIndex = (((*(u32*)(pData + 0xA8) >> 22) & 0x3F) + 1) & 0x3F;
            *(u32*)(pData + 0xA8) = flags | (subIndex << 22);

            if (subIndex == 0) {
                flags = *(u32*)(pData + 0xA8) & 0xF03FFFFF;
                subIndex = (((*(u32*)(pData + 0xA8) >> 22) & 0x3F) - 1) & 0x3F;
                *(u32*)(pData + 0xA8) = flags | (subIndex << 22);
            }
            continue;
        }

        if (opcode >= 0x30 && opcode < 0x40) {
            *(u32*)(pData + 0x64) = (u32)(uintptr_t)(pc + 1);
            delay = (opcode & 0xF) + 1;
            *(s16*)(pData + 0x9E) = *(u16*)(pData + 0x9E) + delay;

            flags = *(u32*)(pData + 0xA8) & 0xF03FFFFF;
            subIndex = (((*(u32*)(pData + 0xA8) >> 22) & 0x3F) + 1) & 0x3F;
            *(u32*)(pData + 0xA8) = flags | (subIndex << 22);

            if (subIndex == 0) {
                flags = *(u32*)(pData + 0xA8) & 0xF03FFFFF;
                subIndex = (((*(u32*)(pData + 0xA8) >> 22) & 0x3F) - 1) & 0x3F;
                *(u32*)(pData + 0xA8) = flags | (subIndex << 22);
            }
            continue;
        }

        if (opcode >= 0x80 && opcode < 0x83) {
            return;
        }

        if (opcode == 0xB3) {
            /* asm .L800228F8-.L80022928: pc[1] sign-extended then masked to
             * 6 bits sets the speed-index field at +0xA8 bits 11-16; falls
             * straight into the table-advance tail with no early-return
             * check (unlike 0x86/0x87/0x97 below). */
            flags = *(u32*)(pData + 0xA8) & 0xFFFE07FF;
            *(u32*)(pData + 0xA8) = flags | ((((s8)pc[1]) & 0x3F) << 11);
            *(u32*)(pData + 0x64) = (u32)(uintptr_t)(pc + D_8004FC40[opcode]);
            continue;
        }

        if (opcode == 0x86 || opcode == 0x87 || opcode == 0x97) {
            /* asm .L80022920-.L80022930: no side effect beyond the shared
             * exit check and generic table advance. */
            if (pc == pBytecode) {
                return;
            }
            *(u32*)(pData + 0x64) = (u32)(uintptr_t)(pc + D_8004FC40[opcode]);
            continue;
        }

        /* asm 80022754-800227C0: 0x40-0x7F share the delay/subIndex tail
         * with stale $s3. No 1D2B0; pc advances by 1. */
        if (opcode < 0x80) {
            *(u32*)(pData + 0x64) = (u32)(uintptr_t)(pc + 1);
            *(s16*)(pData + 0x9E) = *(u16*)(pData + 0x9E) + delay;

            flags = *(u32*)(pData + 0xA8) & 0xF03FFFFF;
            subIndex = (((*(u32*)(pData + 0xA8) >> 22) & 0x3F) + 1) & 0x3F;
            *(u32*)(pData + 0xA8) = flags | (subIndex << 22);

            if (subIndex == 0) {
                flags = *(u32*)(pData + 0xA8) & 0xF03FFFFF;
                subIndex = (((*(u32*)(pData + 0xA8) >> 22) & 0x3F) - 1) & 0x3F;
                *(u32*)(pData + 0xA8) = flags | (subIndex << 22);
            }
            continue;
        }

        /* asm .L8002283C: packed unsigned pc[1]|(pc[2]<<8). Update AC/3C
         * flip bits, 1D2B0 when pose differs, wait += 1 + ((packed>>11)&0xF),
         * then the table-advance tail. */
        if (opcode == 0xBE) {
            u16 packed = (u16)pc[1] | ((u16)pc[2] << 8);
            s32 frame = packed & 0x1FF;
            u32 flagsAC = *(u32*)(pData + 0xAC) & ~0x8u;
            u32 flags3C = *(u32*)(pData + 0x3C) & ~0x8u;

            flagsAC |= (packed >> 6) & 0x8;
            flags3C |= ((((flagsAC >> 3) & 0x1) ^ ((flagsAC >> 2) & 0x1)) << 3);
            *(u32*)(pData + 0x3C) = flags3C;
            *(u32*)(pData + 0xAC) = flagsAC;
            if (*(u16*)(pData + 0x34) != (u16)frame) {
                func_8001D2B0(pData, (s16)frame);
            }
            *(s16*)(pData + 0x9E) =
                *(u16*)(pData + 0x9E) + 1 + ((packed >> 11) & 0xF);
            *(u32*)(pData + 0x64) = (u32)(uintptr_t)(pc + D_8004FC40[opcode]);
            continue;
        }

        /* asm .L800228C0: PushU24(pc+3), then signed 16-bit PC-relative jump. */
        if (opcode == 0xE2) {
            s32 offset = (s16)((u16)pc[1] | ((u16)pc[2] << 8));

            AnimScriptStackPushU24((SpriteData*)pData,
                                   (s32)(uintptr_t)(pc + 3));
            *(u32*)(pData + 0x64) = (u32)(uintptr_t)(pc + offset);
            continue;
        }

        /* asm .L80022930: every other opcode advances by the per-opcode
         * length table D_8004FC40 (retail: pc += D_8004FC40[opcode]). This
         * also owns the strides of 0xB2/0xB4, previously hardcoded here with
         * 0xB4 mis-transcribed as +4 (table value is 2). */
        *(u32*)(pData + 0x64) = (u32)(uintptr_t)(pc + D_8004FC40[opcode]);
    }
}

/* 80022974..80022A00: horizontal velocity, preserving R3000 DIV/MULT
 * and 32-bit shifts. Decomp rsin names retail cosine; rcos names sine. */
void func_80022974(void* pSpriteData) {
    u8* pData = pSpriteData;
    u32 radius = (u32)((s32)AnimationRead32(pData + 0x18) >> 4) << 8;
    u32 divisor = (AnimationRead32(pData + 0xAC) >> 7) & 0xFFFu;
    u32 scaledRadius = divisor ? (u32)((s32)radius / (s32)divisor)
                              : ((s32)radius < 0 ? 1u : 0xFFFFFFFFu);
    u16 angle = AnimationRead16(pData + 0x32);
    s32 trigValue;
    u32 product;

    /* Both retail entries directly index angle & FFF, including negatives. */
    trigValue = rsin(angle & 0xFFFu) >> 2;
    product = (u32)trigValue * scaledRadius;
    angle = AnimationRead16(pData + 0x32); /* 800229BC: before the +C store. */
    AnimationWrite32(pData + 0x0C, (u32)((s32)product >> 6));

    trigValue = rcos(angle & 0xFFFu) >> 2;
    product = 0u - (u32)trigValue * scaledRadius;
    AnimationWrite32(pData + 0x14, (u32)((s32)product >> 6));
}

s32 func_80022A00(s32* arg0) {
    return *arg0;
}
