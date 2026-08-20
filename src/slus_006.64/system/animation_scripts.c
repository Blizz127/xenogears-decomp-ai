#include "common.h"
#include "field/actor.h"
#include "system/memory.h"
#include "psyq/libgte.h"
#ifdef XENO_PC_PORT
#include <assert.h>

extern void func_8001D2B0(void* pSpriteData, s16 frameIndex);
#else
/* <assert.h> is unavailable under the matching build's -nostdinc MIPS
 * preprocessor. The assert(0) calls below mark unimplemented paths in
 * functions not yet byte-matched, so a no-op assert compiles safely there.
 * (uintptr_t comes from include/types.h for both builds.) */
#define assert(x) ((void)0)
#endif

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/animation_scripts", func_8001FBA4);
/*
Matches on GCC 2.7.2-970404, ASPSX 2.67
Co-Authored-By: eagleflo <eagleflo@users.noreply.github.com>

void* func_8001FBA4(SpriteData* pSpriteData, u8* pIndex) {
    u8 index = *pIndex;
    if (!(index & 0x80)) {
        s32 signedIndex = (s8)index;
        return &pSpriteData->stack[pSpriteData->stackIndex + signedIndex];
    }
    return &pSpriteData->field_0x88[index & 0x7F];
}
*/

extern s32 D_80059198;
extern void func_80022974(void* pSpriteData);
extern void func_8001CE74(void* pTargetEntry);

/* sbss 800592E4-EA ("800592E4 -> EA is bss local", rendering.c): image-blob
 * pointer + VRAM x/y handoff into func_8001FB30 (asm-only on the matching
 * build; port implementation in pc_port/src/game_overrides.c). */
extern u32 D_800592E4;
extern s16 D_800592E8;
extern s16 D_800592EA;
extern void func_8001FB30(void);

/* Child-sprite spawner (temp1.c asm 80023B84; port implementation in
 * pc_port/src/game_overrides.c). Returns the child SpriteData. */
extern void* func_80023B84(void* pSpriteData, void* pScript,
                           void* pAnimPackage);

/* Texture-page latch (temp2.c asm 8002CC10; port implementation in
 * pc_port/src/game_overrides.c). */
extern void func_8002CC10(s32 x, s32 y);

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

void func_8001FBE4(void* pSpriteData, u32 opcodeIndex, void* operands) {
    u32 dispatchIndex = (u8)opcodeIndex - 0x8A;

    (void)pSpriteData;
    (void)operands;

    if (dispatchIndex >= 0x73) {
        return;
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

    if (dispatchIndex == 0x28) {
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
                assert(0 && "func_8001FBE4 opcode 0xBC sub-command is not implemented");
                return;

            case 0x05:
                /* 800209B8: retail zeroes the accumulator block then divides
                 * it by a STALE register (indeterminate on hardware). */
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
         * offset. Successful writes join .L800215A4 and mark the transform
         * matrix dirty via +0x3C bit 28. */
        u8* p = pSpriteData;
        u8* ops = operands;
        u8* pBase = (u8*)(uintptr_t)*(u32*)(p + 0x20);
        s32 packed = ops[0] | ((s32)(s8)ops[1] << 8);
        u32 encoded = (u16)packed;
        u32 subIndex = (encoded >> 9) & 0x7;
        s32 value = (encoded & 0x1FF) << 3;
        u16* pY;

        if (pBase == NULL) {
            return;
        }
        if ((*(u32*)(p + 0xAC) & 0x4) != 0) {
            value = -value;
        }

        if (subIndex != 0) {
            u8* pEntries =
                (u8*)(uintptr_t)*(u32*)(pBase + 0x34);
            if (pEntries == NULL) {
                return;
            }
            pY = (u16*)(pEntries + subIndex * 8 + 4);
        } else {
            pY = (u16*)(pBase + 0x2);
        }

        if ((encoded & 0x1000) != 0) {
            *pY = value;
        } else {
            *pY = *pY + value;
        }
        *(u32*)(p + 0x3C) |= 0x10000000;
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

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/animation_scripts", AnimScriptStackPopU8);
/*
Matches on GCC 2.7.2-970404, ASPSX 2.67
Co-Authored-By: Mc-muffin <Mc-muffin@users.noreply.github.com>

u_char AnimScriptStackPopU8(SpriteData* pSpriteData) {
    u_char nStackValue;

    nStackValue = pSpriteData->stack[pSpriteData->stackIndex];
    pSpriteData->stackIndex++;
    return nStackValue;
}
*/

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/animation_scripts", AnimScriptStackPopU16);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/animation_scripts", AnimScriptStackPopU24);

void AnimScriptStackPushU8(SpriteData* pSpriteData, u8 value) {
    s8 idx = --pSpriteData->stackIndex;
    pSpriteData->stack[idx] = value;
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/animation_scripts", AnimScriptStackPushU16);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/animation_scripts", AnimScriptStackPushU24);

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
    *(s16*)((u8*)a0 + 0x32) = a1;
    func_80022974(a0);
}

void SpriteSetScale(SpriteData* pSpriteData, short scale) {
    u8* pData = (u8*)pSpriteData;
    u8* pBase = (u8*)(uintptr_t)*(u32*)(pData + 0x20);

    if (pBase) {
        *(s16*)(pData + 0x2C) = scale;
        *(s16*)(pBase + 0xA) = scale;
        *(s16*)(pBase + 0x8) = scale;
        *(s16*)(pBase + 0x6) = scale;
        *(u32*)(pData + 0x3C) |= 0x10000000;
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

    while (1) {
        u8* pc = (u8*)(uintptr_t)*(u32*)(pData + 0x64);
        u8 opcode;
        s32 delay;
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

        /* Retail-switch cases with side effects that are not yet ported:
         * 0x40-0x7F frame-op families, 0xBE (anim state), 0xE2 (relative
         * jump + AnimScriptStackPushU24). These must not be silently skipped. */
        if (opcode < 0x80 || opcode == 0xBE || opcode == 0xE2) {
            assert(0 && "func_80022660 bytecode path is not implemented");
        }

        /* asm .L80022930: every other opcode advances by the per-opcode
         * length table D_8004FC40 (retail: pc += D_8004FC40[opcode]). This
         * also owns the strides of 0xB2/0xB4, previously hardcoded here with
         * 0xB4 mis-transcribed as +4 (table value is 2). */
        *(u32*)(pData + 0x64) = (u32)(uintptr_t)(pc + D_8004FC40[opcode]);
    }
}

// Recompute animation speed
void func_80022974(void* pSpriteData) {
    u8* pData = pSpriteData;
    s32 radius = (*(s32*)(pData + 0x18) >> 4) << 8;
    s32 divisor = (*(u32*)(pData + 0xAC) >> 7) & 0xFFF;
    s32 scaledRadius = radius / divisor;
    s16 angle = *(s16*)(pData + 0x32);
    s32 sinValue;
    s32 cosValue;

    sinValue = rsin(angle) >> 2;
    *(s32*)(pData + 0x0C) = (sinValue * scaledRadius) >> 6;

    cosValue = rcos(angle) >> 2;
    *(s32*)(pData + 0x14) = -(cosValue * scaledRadius) >> 6;
}

s32 func_80022A00(s32* arg0) {
    return *arg0;
}
