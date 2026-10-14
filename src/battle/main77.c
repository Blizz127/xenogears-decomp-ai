#include "common.h"


#ifndef XENO_PC_PORT
extern void SetShadeTex(void* p, s32 tge);
extern void SetSemiTrans(void* p, s32 abe);

#define B1720_TSB_FIX() \
    if (abr != 0) { \
        *(u16*)(p + 0xA) = (*(u16*)(p + 0xA) & 0xFF9F) | tp; \
    }
/* POLY_FT3 / POLY_FT4 / POLY_GT3 / POLY_GT4 uv, clut and tpage */
#define B1720_FT3_UV() \
    *(u16*)(prim + 0x16) = *(u16*)(p + 0xA); \
    *(u16*)(prim + 0xE) = *(u16*)(p + 6); \
    prim[0xC] = p[4]; prim[0xD] = p[5]; \
    prim[0x14] = p[8]; prim[0x15] = p[9]; \
    prim[0x1C] = p[0xC]; prim[0x1D] = p[0xD];
#define B1720_FT4_UV() \
    *(u16*)(prim + 0x16) = *(u16*)(p + 0xA); \
    *(u16*)(prim + 0xE) = *(u16*)(p + 6); \
    prim[0xC] = p[4]; prim[0xD] = p[5]; \
    prim[0x14] = p[8]; prim[0x15] = p[9]; \
    prim[0x1C] = p[0xC]; prim[0x1D] = p[0xD]; \
    prim[0x24] = p[0x10]; prim[0x25] = p[0x11];
#define B1720_GT3_UV() \
    *(u16*)(prim + 0x1A) = *(u16*)(p + 0xA); \
    *(u16*)(prim + 0xE) = *(u16*)(p + 6); \
    prim[0xC] = p[4]; prim[0xD] = p[5]; \
    prim[0x18] = p[8]; prim[0x19] = p[9]; \
    prim[0x24] = p[0xC]; prim[0x25] = p[0xD];
#define B1720_GT4_UV() \
    *(u16*)(prim + 0x1A) = *(u16*)(p + 0xA); \
    *(u16*)(prim + 0xE) = *(u16*)(p + 6); \
    prim[0xC] = p[4]; prim[0xD] = p[5]; \
    prim[0x18] = p[8]; prim[0x19] = p[9]; \
    prim[0x24] = p[0xC]; prim[0x25] = p[0xD]; \
    prim[0x30] = p[0x10]; prim[0x31] = p[0x11];

/* func_800B1720.s, byte-exact: convert the TMD-style primitive list of
 * `tmd` (count at +0x14, packets at tmd + the +0x10 offset) into GPU
 * packets at `prim`.  Per packet: tag length (+3) = the packet's olen
 * (byte 0), code (+7) = its mode (byte 3), SetShadeTex(prim, tge) and, when
 * `abr` is set, SetSemiTrans.  The type (mode & 0x1C, +0x100 when flag bit 0
 * is clear = unlit) picks the copy: flat F3/F4 colour, gouraud G3/G4
 * colours, textured FT3/FT4/GT3/GT4 colours (0x80 grey when unlit) plus uv,
 * clut and tpage -- for the textured types, when `abr` is set, the SOURCE
 * packet's tpage (+0xA) is first rewritten in place with abr - 1 as its
 * blend bits (0x60), so the caller's model data changes; other types copy
 * nothing.  Then prim advances by (olen + 1) words
 * and the packet by (ilen (byte 1) + 1) words.  Codegen: the case bodies
 * are in retail's order (gouraud first, each unlit textured type before its
 * lit twin) so cross-jumping shares the uv tails as retail does; the type
 * code is built in two statements and tp is an int. */
void func_800B1720(u8* tmd, u8* prim, s32 abr, s32 tge) {
    s32 count;
    u8* p;
    s32 tp;
    s32 i;
    s32 t;

    count = *(s32*)(tmd + 0x14);
    p = (u8*)(*(s32*)(tmd + 0x10) + (s32)tmd);
    if (abr != 0) {
        tp = (abr - 1) & 3;
        tp <<= 5;
    }
    for (i = 0; i != count; i++) {
        prim[3] = p[0];
        prim[7] = p[3];
        SetShadeTex(prim, (u8)tge);
        if (abr != 0) {
            SetSemiTrans(prim, abr != 0);
        }
        t = p[3] & 0x1C;
        t |= ((p[2] ^ 1) & 1) << 8;
        switch (t) {
        case 0x10:
        case 0x110:
            prim[4] = p[4]; prim[5] = p[5]; prim[6] = p[6];
            prim[0xC] = p[8]; prim[0xD] = p[9]; prim[0xE] = p[0xA];
            prim[0x14] = p[0xC]; prim[0x15] = p[0xD]; prim[0x16] = p[0xE];
            break;
        case 0x18:
        case 0x118:
            prim[4] = p[4]; prim[5] = p[5]; prim[6] = p[6];
            prim[0xC] = p[8]; prim[0xD] = p[9]; prim[0xE] = p[0xA];
            prim[0x14] = p[0xC]; prim[0x15] = p[0xD]; prim[0x16] = p[0xE];
            prim[0x1C] = p[0x10]; prim[0x1D] = p[0x11]; prim[0x1E] = p[0x12];
            break;
        case 0x00:
        case 0x08:
        case 0x100:
        case 0x108:
            prim[4] = p[4]; prim[5] = p[5]; prim[6] = p[6];
            break;
        case 0x104:
            B1720_TSB_FIX();
            prim[4] = 0x80; prim[5] = 0x80; prim[6] = 0x80;
            B1720_FT3_UV();
            break;
        case 0x04:
            B1720_TSB_FIX();
            prim[4] = p[0x10]; prim[5] = p[0x11]; prim[6] = p[0x12];
            B1720_FT3_UV();
            break;
        case 0x114:
            B1720_TSB_FIX();
            prim[4] = 0x80; prim[5] = 0x80; prim[6] = 0x80;
            prim[0x10] = 0x80; prim[0x11] = 0x80; prim[0x12] = 0x80;
            prim[0x1C] = 0x80; prim[0x1D] = 0x80; prim[0x1E] = 0x80;
            B1720_GT3_UV();
            break;
        case 0x14:
            B1720_TSB_FIX();
            prim[4] = p[0x10]; prim[5] = p[0x11]; prim[6] = p[0x12];
            prim[0x10] = p[0x14]; prim[0x11] = p[0x15]; prim[0x12] = p[0x16];
            prim[0x1C] = p[0x18]; prim[0x1D] = p[0x19]; prim[0x1E] = p[0x1A];
            B1720_GT3_UV();
            break;
        case 0x10C:
            B1720_TSB_FIX();
            prim[4] = 0x80; prim[5] = 0x80; prim[6] = 0x80;
            B1720_FT4_UV();
            break;
        case 0x0C:
            B1720_TSB_FIX();
            prim[4] = p[0x14]; prim[5] = p[0x15]; prim[6] = p[0x16];
            B1720_FT4_UV();
            break;
        case 0x11C:
            B1720_TSB_FIX();
            prim[4] = 0x80; prim[5] = 0x80; prim[6] = 0x80;
            prim[0x10] = 0x80; prim[0x11] = 0x80; prim[0x12] = 0x80;
            prim[0x1C] = 0x80; prim[0x1D] = 0x80; prim[0x1E] = 0x80;
            prim[0x28] = 0x80; prim[0x29] = 0x80; prim[0x2A] = 0x80;
            B1720_GT4_UV();
            break;
        case 0x1C:
            B1720_TSB_FIX();
            prim[4] = p[0x14]; prim[5] = p[0x15]; prim[6] = p[0x16];
            prim[0x10] = p[0x18]; prim[0x11] = p[0x19]; prim[0x12] = p[0x1A];
            prim[0x1C] = p[0x1C]; prim[0x1D] = p[0x1D]; prim[0x1E] = p[0x1E];
            prim[0x28] = p[0x20]; prim[0x29] = p[0x21]; prim[0x2A] = p[0x22];
            B1720_GT4_UV();
            break;
        }
        prim += (p[0] + 1) * 4;
        p += (p[1] + 1) * 4;
    }
}
#include "psyq/libgte.h"

typedef struct {
    s32 offset; /* vertex block offset from the header */
    s32 count;
    u8 pad[0x10];
    s32 flags;
} Header;

/* One-shot scale of a vertex block: if flag 0x8000 is clear, set it and
 * shift every vertex (vx, vy, vz) left by `shift`. */
void func_800B1EA0(Header* header, s32 shift) {
    s32 i;
    s32 n;
    SVECTOR* v;
    s32 f = header->flags;

    if (!(f & 0x8000)) {
        header->flags = f | 0x8000;
        v = (SVECTOR*)(header->offset + (s32)header);
        n = header->count;
        for (i = 0; i != n; i++) {
            v[i].vx <<= shift;
            v[i].vy <<= shift;
            v[i].vz <<= shift;
        }
    }
}

/* 8-byte prim (tag + one code word) copied from the D_800C3BF8 template. */
typedef struct {
    u32 tag;
    u32 code;
} M77Prim8;

/* Array views: the CDK build keeps each %hi in a register for the paired
 * %lo load/store (retail), and copies the template through its address. */
extern void* g_GfxCurWorkBuffer__a[] asm("g_GfxCurWorkBuffer");
extern void* g_GfxCurWorkBufferEnd__a[] asm("g_GfxCurWorkBufferEnd");
extern M77Prim8 D_800C3BF8[];
extern void AddPrim(void* ot, void* p);

/* Queue a copy of the D_800C3BF8 draw-mode prim at ot when the work buffer
 * has room. */
void func_800B1F0C(void* ot) {
    M77Prim8* p = g_GfxCurWorkBuffer__a[0];

    if ((u8*)p + 8 < (u8*)g_GfxCurWorkBufferEnd__a[0]) {
        g_GfxCurWorkBuffer__a[0] = (u8*)p + 8;
        *p = D_800C3BF8[0];
        AddPrim(ot, p);
    }
}
INCLUDE_ASM("asm/battle/nonmatchings/main77", func_800B1F6C);
extern u8 func_80021AD8(u8 c, s32 scale);

#define B2AEC_RGB(o, b0, b1, b2) \
    prim[(o)] = func_80021AD8((b0), r); \
    prim[(o) + 1] = func_80021AD8((b1), g); \
    prim[(o) + 2] = func_80021AD8((b2), b);
#define B2AEC_COPY(o) \
    prim2[(o)] = prim[(o)]; prim2[(o) + 1] = prim[(o) + 1]; prim2[(o) + 2] = prim[(o) + 2];

/* func_800B2AEC.s, byte-exact: re-tint two GPU packet buffers that follow
 * `tmd` packet for packet (the same walk as func_800B1720; the animation
 * script handler func_8001FBE4 passes the model's +0x34 list and its
 * +0x2C/+0x30 buffers): for each lit type (flat F3/F4,
 * gouraud G3/G4, textured FT3/FT4/GT3/GT4, and the unlit textured types
 * from 0x80 grey) every colour channel becomes func_80021AD8(source
 * channel, r / g / b) in `prim` and is mirrored into the second buffer
 * `prim2`; other types are skipped.  Both buffers advance by the packet's
 * (olen + 1) words, the source by (ilen + 1) words.  Codegen: the case
 * bodies are in retail's order, as func_800B1720's. */
void func_800B2AEC(u8* tmd, u8* prim, u8* prim2, s32 r, s32 g, s32 b) {
    s32 count;
    u8* p;
    s32 i;
    s32 t;
    s32 n;

    count = *(s32*)(tmd + 0x14);
    p = (u8*)(*(s32*)(tmd + 0x10) + (s32)tmd);
    for (i = 0; i != count; i++) {
        t = p[3] & 0x1C;
        t |= ((p[2] ^ 1) & 1) << 8;
        switch (t) {
        case 0x00:
            B2AEC_RGB(4, p[4], p[5], p[6]);
            B2AEC_COPY(4);
            break;
        case 0x10:
            B2AEC_RGB(4, p[4], p[5], p[6]);
            B2AEC_RGB(0xC, p[8], p[9], p[0xA]);
            B2AEC_RGB(0x14, p[0xC], p[0xD], p[0xE]);
            B2AEC_COPY(4);
            B2AEC_COPY(0xC);
            B2AEC_COPY(0x14);
            break;
        case 0x18:
            B2AEC_RGB(4, p[4], p[5], p[6]);
            B2AEC_RGB(0xC, p[8], p[9], p[0xA]);
            B2AEC_RGB(0x14, p[0xC], p[0xD], p[0xE]);
            B2AEC_RGB(0x1C, p[0x10], p[0x11], p[0x12]);
            B2AEC_COPY(4);
            B2AEC_COPY(0xC);
            B2AEC_COPY(0x14);
            B2AEC_COPY(0x1C);
            break;
        case 0x08:
            B2AEC_RGB(4, p[4], p[5], p[6]);
            B2AEC_COPY(4);
            break;
        case 0x104:
            B2AEC_RGB(4, 0x80, 0x80, 0x80);
            B2AEC_COPY(4);
            break;
        case 0x04:
            B2AEC_RGB(4, p[0x10], p[0x11], p[0x12]);
            B2AEC_COPY(4);
            break;
        case 0x114:
            B2AEC_RGB(4, 0x80, 0x80, 0x80);
            B2AEC_RGB(0x10, 0x80, 0x80, 0x80);
            B2AEC_RGB(0x1C, 0x80, 0x80, 0x80);
            B2AEC_COPY(4);
            B2AEC_COPY(0x10);
            B2AEC_COPY(0x1C);
            break;
        case 0x14:
            B2AEC_RGB(4, p[0x10], p[0x11], p[0x12]);
            B2AEC_RGB(0x10, p[0x14], p[0x15], p[0x16]);
            B2AEC_RGB(0x1C, p[0x18], p[0x19], p[0x1A]);
            B2AEC_COPY(4);
            B2AEC_COPY(0x10);
            B2AEC_COPY(0x1C);
            break;
        case 0x10C:
            B2AEC_RGB(4, 0x80, 0x80, 0x80);
            B2AEC_COPY(4);
            break;
        case 0x0C:
            B2AEC_RGB(4, p[0x14], p[0x15], p[0x16]);
            B2AEC_COPY(4);
            break;
        case 0x11C:
            B2AEC_RGB(4, 0x80, 0x80, 0x80);
            B2AEC_RGB(0x10, 0x80, 0x80, 0x80);
            B2AEC_RGB(0x1C, 0x80, 0x80, 0x80);
            B2AEC_RGB(0x28, 0x80, 0x80, 0x80);
            B2AEC_COPY(4);
            B2AEC_COPY(0x10);
            B2AEC_COPY(0x1C);
            B2AEC_COPY(0x28);
            break;
        case 0x1C:
            B2AEC_RGB(4, p[0x14], p[0x15], p[0x16]);
            B2AEC_RGB(0x10, p[0x18], p[0x19], p[0x1A]);
            B2AEC_RGB(0x1C, p[0x1C], p[0x1D], p[0x1E]);
            B2AEC_RGB(0x28, p[0x20], p[0x21], p[0x22]);
            B2AEC_COPY(4);
            B2AEC_COPY(0x10);
            B2AEC_COPY(0x1C);
            B2AEC_COPY(0x28);
            break;
        }
        n = (p[0] + 1) * 4;
        prim += n;
        prim2 += n;
        p += (p[1] + 1) * 4;
    }
}
#endif


#ifndef XENO_PC_PORT
extern u8* D_800D2D28;
#endif
#ifndef XENO_PC_PORT
extern u8* D_800D2DC8;
#endif


/* func_800B3348.s */
void func_800B3348(void) {
}
/* func_800B3350.s */
void func_800B3350(void) {
}


#ifdef XENO_PC_PORT
#include "battle_host_guest.h"
/* func_800B1EA0.s: one-shot scale of a vertex block.  If flag 0x8000 of the
 * +0x18 word is clear, set it and shift every s16 x/y/z of the `count`
 * (+0x4) 8-byte vertices at header + header[0] left by `shift` (SLLV: low
 * five bits). */
void func_800B1EA0(u8* header, u32 shift) {
    u32 h = BG_ADDR(header);
    u32 flags = BG_U32(h + 0x18);
    u32 count;
    u32 v;
    u32 i;

    if ((flags & 0x8000) != 0) {
        return;
    }
    BG_U32(h + 0x18) = flags | 0x8000;
    v = BG_U32(h) + h;
    count = BG_U32(h + 4);
    shift &= 31;
    for (i = 0; i != count; i++, v += 8) {
        BG_U16(v + 0) = (u16)((u32)(s32)BG_S16(v + 0) << shift);
        BG_U16(v + 4) = (u16)((u32)(s32)BG_S16(v + 4) << shift);
        BG_U16(v + 2) = (u16)((u32)(s32)BG_S16(v + 2) << shift);
    }
}
#endif
