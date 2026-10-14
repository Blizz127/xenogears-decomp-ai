#include "common.h"

#ifndef XENO_PC_PORT
/* func_80022224's by-value (x, y) pairs (texture page, CLUT). */
typedef struct {
    s16 x;
    s16 y;
} C0FACPair;
extern void func_800C0F70(void);
extern u8 D_800C3620[];
extern u8 D_800C3622[];
extern u32 D_800C3A6C[];
extern s32 D_80010000;
extern u8 D_8005A474[];
extern void* SoundLoadWdsFile(void* pWdsFile, s32 mode);
extern void SoundAddSedsEntry(u8* p);
extern s16 func_8003BDFC(s32 a);
extern void* HeapInsertAlloc(void* pMem, u32 allocSize);
extern void func_80022224(void* pAnimPackage, void* pAnimPackageFile, C0FACPair tex, C0FACPair clut,
                          u32 arg4);

/* Register the sound data in battle file `base` (count *base - 3, entry
 * offsets from +0x10, each relative to the file).  "seds" entries go to
 * SoundAddSedsEntry (the last one is returned).  A "wds " entry runs
 * func_800C0F70, clears D_800C3620 and D_800C3622, loads the bank into
 * D_800C3A6C with SoundLoadWdsFile, waits while func_8003BDFC(0) is set
 * (break 1 whenever D_80010000 is not -1) and, when it is the last entry,
 * calls HeapInsertAlloc(base, entry offset).  Any other entry goes to
 * func_80022224 with the D_8005A474 package, texture page (0x380, 0x100)
 * and CLUT (0, 0x1F4).  `wds` takes D_800C3A6C's address before the calls,
 * which keeps its %hi in a register across the loop, as retail does;
 * `break 1024` is the aspsx encoding of retail's break 1 (as in
 * controller2.c). */
u8* func_800C0FAC(u8* base) {
    u8* seds = NULL;
    u8* file = base;
    s32 n;
    u8* e;
    C0FACPair tex;
    C0FACPair clut;
    u32* wds;

    n = *(s32*)file - 3;
    for (file += 0x10; n > 0; n--, file += 4) {
        e = (u8*)(*(u32*)file + (u32)base);
        switch (*(u32*)e) {
        case 0x73646573:
            seds = e;
            SoundAddSedsEntry(seds);
            break;
        case 0x20736477:
            wds = D_800C3A6C;
            func_800C0F70();
            D_800C3620[0] = 0;
            D_800C3622[0] = 0;
            *wds = (u32)SoundLoadWdsFile(e, 0);
            while (func_8003BDFC(0) != 0) {
                if (D_80010000 != -1) {
                    __asm__ volatile("break 1024");
                }
            }
            if (n == 1) {
                HeapInsertAlloc(base, *(u32*)file);
            }
            break;
        default:
            tex.x = 0x380;
            tex.y = 0x100;
            clut.x = 0;
            clut.y = 0x1F4;
            func_80022224(D_8005A474, e, tex, clut, 0);
            break;
        }
    }
    return seds;
}
extern void func_8003852C(void* p);

/* func_800C1140.s: walk the offset table of `file` (count *file - 3, offsets
 * from +0x10, each relative to the file) and call func_8003852C on every
 * entry whose first word is the 0x73646573 tag.  The parameter is reused as
 * the table cursor while `base` keeps the file start, as retail does with two
 * copies of it; the entry address is integer arithmetic (offset first). */
void func_800C1140(u8* file) {
    u8* base = file;
    s32 n;

    n = *(s32*)file - 3;
    for (file += 0x10; n > 0; n--, file += 4) {
        if (*(u32*)(*(u32*)file + (u32)base) == 0x73646573) {
            func_8003852C((u8*)(*(u32*)file + (u32)base));
        }
    }
}
#include "psyq/libgte.h"

/* XZ position passed by value to func_80023124 (angle between two sprites). */
typedef struct {
    s16 x;
    s16 y;
} C11CCPair;

typedef struct C11CCSprite {
    s32 x;                      /* 0x00 */
    s32 y;                      /* 0x04 */
    s32 z;                      /* 0x08 */
    u8 pad0C[4];
    s32 unk10;                  /* 0x10 */
    u8 pad14[0xC];
    u8* unk20;                  /* 0x20 */
    u8 pad24[7];
    u8 unk2B;                   /* 0x2B */
    s16 unk2C;                  /* 0x2C */
    u8 pad2E[6];
    u16 unk34;                  /* 0x34 */
    u8 pad36[4];
    u16 unk3A;                  /* 0x3A */
    u32 mode3C : 2;             /* 0x3C */
    u32 b3C2 : 1;
    u32 flip3C : 1;
    u32 f3C4 : 1;
    u32 f3C5 : 3;
    u32 pad3C8 : 24;
    u32 pad40a : 13;            /* 0x40 */
    u32 kind40 : 4;
    u32 pad40b : 15;
    u8 pad44[4];
    s32 unk48;                  /* 0x48 */
    u8 pad4C[0x14];
    u8* unk60;                  /* 0x60 */
    u8* script;                 /* 0x64 */
    void (*callback)(struct C11CCSprite*); /* 0x68 */
    u8 pad6C[4];
    struct C11CCSprite* unk70;  /* 0x70 */
    struct C11CCSprite* target; /* 0x74 */
    u8 pad78[0xC];
    s16 unk84;                  /* 0x84 */
    u8 pad86[7];
    s8 unk8D;                   /* 0x8D */
    u8 pad8E[0x10];
    s16 wait;                   /* 0x9E */
    s16 unkA0;                  /* 0xA0 */
    s16 unkA2;                  /* 0xA2 */
    s16 unkA4;                  /* 0xA4 */
    u8 padA6[2];
    u32 padA8a : 11;            /* 0xA8 */
    u32 cnt11 : 6;
    u32 padA8b : 5;
    u32 cnt22 : 6;
    u32 state28 : 2;
    u32 slotLo : 2;
    u32 slotHi : 2;             /* 0xAC */
    u32 flipAC : 1;
    u32 bAC3 : 1;
    u32 padAC4 : 3;
    u32 speed : 12;
    u32 padAC19 : 5;
    s32 animAF : 8;
    s8 unkB0;                   /* 0xB0 */
} C11CCSprite;

/* 4-bit battle slot: +0xA8 bits 30-31 low, +0xAC bits 0-1 high (whole-word reads, as retail). */
#define C11CC_SLOT(p) \
    ({ u32 hi_ = *(u32*)((u8*)(p) + 0xA8) >> 30; ((*(u32*)((u8*)(p) + 0xAC) & 3) << 2) | hi_; })

extern u8 D_8004FC40[];
extern s16 D_80059454[];
extern s32 D_8006F99C[];
extern s32 D_8006F9AC[];
extern s32 D_800C360C[];
/* Stored through `*(T*)` casts: those accesses are not MEM_IN_STRUCT_P, so the
 * scheduler may move sprite-field loads past them as it did in retail. */
extern s8 D_800C3624__s[] asm("D_800C3624");
extern C11CCSprite* D_800C3E1C[];
extern u8 D_800C3EB0[];
extern u8 D_800C4928[];
extern SVECTOR D_800D30B0[];
extern s32 D_800D30B8[];
extern s16 D_800D3354[];
extern s16 D_800D335C[];
extern s16 D_800D3678[];
extern u8 g_WorkListCurTimer__b[] asm("g_WorkListCurTimer");

extern u32 AnimScriptStackPopU24(void* p);
extern u8 AnimScriptStackPopU8(void* p);
extern void AnimScriptStackPushU24(void* p, u8* v);
extern void AnimScriptStackPushU8(void* p, u8 v);
extern s32 ArchiveDataSync(void);
extern void* HeapAlloc(u32 size, u32 flags);
extern void func_8001D2B0(void* p, s32 frame);
extern u8* func_8001FBA4(void* p, u8* a);
extern void func_8001FBE4(void* p, u32 op, u8* a);
extern void func_80021B04(SVECTOR* out, s32 a, s32 b, s32 c);
extern void func_80021FC0(void* p, s32 v);
extern void func_80021FE0(void* p, s32 v);
extern void func_800223B0(void* p, s32 v);
extern void func_80022D44(void* p);
extern s16 func_80023124(C11CCPair a, C11CCPair b);
extern void func_80023538(void* p, u8* a);
extern void func_800245D8(void* p, s32 anim);
extern void func_80025180(void* p);
extern void func_800AA454(u16 slot, u16 mask, s32 anim);
extern u8* func_800B168C(u8* p, u32 a);
extern u32 func_800B16A4(u8* p);
extern void func_800B1720(u8* a, void* b, s32 c, s32 d);
extern void func_800B1EA0(u8* a, u32 b);
extern void func_800B3658(SVECTOR* v, s32 a);
extern void func_800B39C0(s32 ticks, s32 kind, u8 r, u8 g, u8 b);
extern u32 func_800B3B6C(void);
extern void func_800B3F04(u8* actor, s32 cmd, u8* args);
extern void func_800B5924(void* p, u32 a, u8* b);
extern void func_800BA614(void* p);
extern void func_800BA768(u8* actor);
extern void func_800BD2E4(void);
extern void func_800BF8CC(u8* actor);
extern s32 func_800BF954(void* p);
/* Deliberately not the builtin's prototype: with the standard one this cc1
 * expands the call as bcopy(src, dst, n); retail calls memcpy. */
extern void memcpy(void* dst, void* src, s32 n);

typedef struct {
    u8 pad0[0x8C58];
    u16 unk8C58;
} C11CCGlob;

static inline C11CCGlob* C11CC_Glob(void) {
    return (C11CCGlob*)D_800C3EB0;
}

/* Battle sprite animation-script interpreter (the battle counterpart of the
 * field VM func_80022660).  Runs opcodes at sprite->script (+0x64) until a
 * wait is pending (+0x9E).  0x00-0x3F step the frame and set a delay scaled
 * by the speed field, 0x40-0x7F reuse the previous delay; 0x80-0xFB dispatch
 * through a 124-entry table and everything else, plus the unlisted opcodes,
 * goes to the shared field handler func_8001FBE4.  The loop is a goto loop:
 * retail tests the wait at the top on every pass and hoists nothing. */
void func_800C11CC(C11CCSprite* sprite) {
    SVECTOR v;
    MATRIX m;
    VECTOR out;
    C11CCPair self;
    C11CCPair tgt;
    u8* p;
    u8* a;
    u8 op;
    s32 wait;

loop:
    if (sprite->wait != 0) {
        return;
    }
    p = sprite->script;
    op = *p;
    a = p + 1;
    if (op < 0x80) {
        sprite->script = a;
        if (op < 0x10) {
            func_8001D2B0(sprite, sprite->unk34 + 1);
            wait = (op & 0xF) + 1;
        } else if (op < 0x20) {
            sprite->cnt11++;
            func_80022D44(sprite);
            wait = (op & 0xF) + 1;
        } else if (op < 0x30) {
            func_8001D2B0(sprite, sprite->unk34 - 1);
            wait = (op & 0xF) + 1;
        }
        if (op < 0x40) {
            wait = (op & 0xF) + 1;
        }
        wait = wait * sprite->speed / 256;
        if (wait == 0) {
            wait = 1;
        }
        sprite->wait += wait;
        if (++sprite->cnt22 == 0) {
            sprite->cnt22--;
        }
        return;
    }
    switch (op) {
    case 0xE8:
        func_800B3F04((u8*)sprite, a[0], func_8001FBA4(sprite, a + 1));
        break;
    case 0xCA: {
        u8* q = a + (((s8)a[1] << 8) | a[0]);

        func_800B39C0(q[3] >> 1, q[4] != 0 ? q[4] - 1 : func_800B3B6C(), q[0], q[1], q[2]);
        break;
    }
    case 0xCB: {
        u8* q = a + (((s8)a[1] << 8) | a[0]);

        func_80021B04(&v, q[0], q[1], q[2]);
        func_800B3658(&v, q[3]);
        break;
    }
    case 0xE3: {
        C11CCSprite* t = sprite->target;
        u8* dst = sprite->script + ((s16)(a[1] << 8) | a[0]);

        t->animAF = 0x3F;
        func_80023538(t, dst);
        break;
    }
    case 0xFB:
        func_800B5924(sprite, a[2] * 2, sprite->script + ((s16)(a[1] << 8) | a[0]));
        break;
    case 0xC3:
    case 0xEC:
    case 0xF9:
        func_800B3F04((u8*)sprite, a[0], a + 1);
        break;
    case 0x9D:
        if (sprite->kind40 == 0xA) {
            D_800D3354[0] = sprite->x >> 16;
            D_800D3354[1] = sprite->y >> 16;
            D_800D3354[2] = sprite->z >> 16;
        } else {
            D_800D335C[0] = sprite->x >> 16;
            D_800D335C[1] = sprite->y >> 16;
            D_800D335C[2] = sprite->z >> 16;
        }
        break;
    case 0x99:
        ((SVECTOR*)sprite->unk20)->vx = D_800D30B0[0].vx;
        ((SVECTOR*)sprite->unk20)->vy = D_800D30B0[0].vy;
        ((SVECTOR*)sprite->unk20)->vz = D_800D30B0[0].vz;
        break;
    case 0x9A: {
        SVECTOR* rot = (SVECTOR*)sprite->unk20;

        v.vx = D_800D30B8[0];
        v.vy = 0;
        v.vz = 0;
        RotMatrix(rot, &m);
        ApplyMatrix(&m, &v, &out);
        if (sprite->kind40 == 0xA) {
            sprite->x = D_8006F9AC[0] - (out.vx << 16);
            sprite->y = D_8006F9AC[1] - (out.vy << 16);
            sprite->z = D_8006F9AC[2] - (out.vz << 16);
        }
        if (sprite->kind40 == 0xB) {
            sprite->x = D_8006F99C[0] + (out.vx << 16);
            sprite->y = D_8006F99C[1] + (out.vy << 16);
            sprite->z = D_8006F99C[2] + (out.vz << 16);
        }
        break;
    }
    case 0x9B:
        sprite->unk2C = (D_80059454[0] << 12) / D_800D30B8[0];
        break;
    case 0x9C:
        v.vx = (D_80059454[0] << 12) / sprite->unk2C;
        v.vy = 0;
        v.vz = 0;
        RotMatrix(D_800D30B0, &m);
        ApplyMatrix(&m, &v, &out);
        if (sprite->kind40 == 0xA) {
            sprite->x = D_8006F9AC[0] - (out.vx << 16);
            sprite->y = D_8006F9AC[1] - (out.vy << 16);
            sprite->z = D_8006F9AC[2] - (out.vz << 16);
        }
        if (sprite->kind40 == 0xB) {
            sprite->x = D_8006F99C[0] + (out.vx << 16);
            sprite->y = D_8006F99C[1] + (out.vy << 16);
            sprite->z = D_8006F99C[2] + (out.vz << 16);
        }
        break;
    case 0xC2: {
        C11CCSprite* t;
        s32 d;

        sprite->y = (sprite->unk84 - 1) << 16;
        t = sprite->target;
        d = (s8)a[0] * sprite->unk2C / 4096;
        if (!sprite->flipAC) {
            d = -d;
        }
        sprite->unkA0 = (t->x >> 16) + d;
        sprite->unkA4 = t->z >> 16;
        sprite->unkA2 = 0;
        func_800BA768((u8*)sprite);
        break;
    }
    case 0x97: {
        C11CCSprite* t;

        if ((sprite->y >> 16) < sprite->unk84) {
            sprite->wait = 1;
            return;
        }
        t = sprite->target;
        func_80021FC0(sprite, 0);
        self.x = sprite->x >> 16;
        self.y = sprite->z >> 16;
        tgt.x = t->x >> 16;
        tgt.y = t->z >> 16;
        func_80021FE0(sprite, func_80023124(tgt, self));
        func_800223B0(sprite, func_80023124(tgt, self));
        func_80021FE0(t, func_80023124(self, tgt));
        func_800223B0(t, func_80023124(self, tgt));
        break;
    }
    case 0xA4: {
        C11CCSprite* t = sprite->target;
        s32 anim = (s8)a[0];

        if (t->unk48 == 0) {
            func_800AA454(C11CC_SLOT(t), 1 << C11CC_SLOT(sprite), anim);
        } else {
            func_800245D8(t, anim);
        }
        break;
    }
    case 0x9E:
        sprite->wait = 1;
        if (!(C11CC_Glob()->unk8C58 & 0x100)) {
            return;
        }
        break;
    case 0x95:
        sprite->wait = 1;
        if (ArchiveDataSync() != 0) {
            return;
        }
        break;
    case 0x89:
        func_800BA768((u8*)sprite);
        break;
    case 0x88:
        func_800BA614(sprite);
        break;
    case 0x8F:
        *(u8*)D_800C3624__s = 1;
        sprite->script = NULL;
        sprite->state28 = 0;
        return;
    case 0xF8: {
        s32 cond;
        /* Via a base pointer so (index - 1) * 0x48 is not distributed and
         * 0x150 stays the load offset; s16 gives retail's copy in case 4. */
        u8* base = D_800C3EB0;
        u8* rec = base + (D_800C360C[0] - 1) * 0x48;
        s16 c = (rec + C11CC_SLOT(sprite->target))[0x150];

        switch (a[2] & 0x7F) {
        case 8:
            cond = D_800C4928[0] != 0;
            break;
        case 9:
            cond = sprite->target == D_800C3E1C[0];
            break;
        case 0:
            cond = c == 0;
            break;
        case 1:
            cond = c == 1;
            break;
        case 2:
            cond = c == 2;
            break;
        case 3:
            cond = c == 3;
            break;
        case 4:
            if (c == 4 || c == 7) {
                cond = 1;
            } else {
                cond = 0;
            }
            break;
        case 5:
            cond = c == 5;
            break;
        case 6:
            cond = c == 6;
            break;
        case 7: {
            s32 n = D_800D3678[0];

            cond = n > func_800BF954(sprite->target) + 1;
            func_800BF8CC((u8*)sprite);
            break;
        }
        default:
            cond = 0;
            break;
        }
        if (a[2] & 0x80) {
            cond = !cond;
        }
        if (!cond) {
            break;
        }
        sprite->script += ((s16)(a[1] << 8) | a[0]);
        goto loop;
    }
    case 0xF3: {
        s32 off;
        u8* src;

        if (*(void**)(sprite->unk20 + 0x2C) != NULL) {
            func_80025180(*(void**)(sprite->unk20 + 0x2C));
        }
        off = ((s8)a[2] << 16) + (a[1] << 8) + a[0];
        src = (u8*)(off + (s32)a);
        if (off != 0) {
            u32 size = func_800B16A4(func_800B168C(src, 0));
            u8* buf = HeapAlloc(size * 2, 0);

            if (sprite->unk3A != 0) {
                func_800B1EA0(func_800B168C(src, 0), 3);
            }
            func_800B1720(func_800B168C(src, 0), buf, sprite->f3C5, sprite->unk2B & 1);
            memcpy(buf + size, buf, size);
            *(u8**)(sprite->unk20 + 0x2C) = buf;
            *(u8**)(sprite->unk20 + 0x30) = buf + size;
            *(u8**)(sprite->unk20 + 0x34) = func_800B168C(src, 0);
        } else {
            *(u8**)(sprite->unk20 + 0x2C) = NULL;
            *(u8**)(sprite->unk20 + 0x34) = NULL;
        }
        break;
    }
    case 0x8B:
        func_800BD2E4();
        break;
    case 0xBE: {
        s32 n = a[0] | ((s8)a[1] << 8);
        s32 w;
        s32 frame;

        w = ((n >> 11) & 0xF) + 1;
        frame = n & 0x1FF;
        w = w * sprite->speed / 256;
        if (w == 0) {
            w = 1;
        }
        if (sprite->mode3C != 1) {
            sprite->unk34 = frame;
            sprite->wait += w;
            sprite->script += 3;
            return;
        }
        if (n < 0 && frame != 0) {
            frame = sprite->unk60[frame - 1];
        }
        sprite->bAC3 = n >> 9;
        sprite->flip3C = sprite->bAC3 ^ sprite->flipAC;
        if ((n >> 10) & 1) {
            sprite->f3C4 = 1;
        } else {
            sprite->f3C4 = 0;
        }
        func_8001D2B0(sprite, frame);
        sprite->wait += w;
        sprite->script += 3;
        return;
    }
    case 0x8E:
        sprite->script = NULL;
        return;
    case 0xE2: {
        s32 n = a[0] + (s16)(a[1] << 8);

        AnimScriptStackPushU24(sprite, sprite->script + 3);
        sprite->script += (s16)n;
        goto loop;
    }
    case 0x85:
        sprite->script = (u8*)(((u32)sprite->script & 0xFF000000) | AnimScriptStackPopU24(sprite));
        goto loop;
    case 0xFA:
        if (*func_8001FBA4(sprite, a) == 0) {
            break;
        }
        sprite->script += ((s16)(a[2] << 8) | a[1]);
        goto loop;
    case 0xD4:
        sprite->script += ((s16)(a[1] << 8) | a[0]);
        if (sprite->callback != NULL) {
            sprite->callback(sprite);
        }
        goto loop;
    case 0x86:
        if (sprite->unk10 < 0) {
            sprite->wait = 1;
            return;
        }
        break;
    case 0x87:
        if ((sprite->y >> 16) < sprite->unk84) {
            sprite->wait = 1;
            return;
        }
        break;
    case 0x80:
    end:
        sprite->state28 = 0;
        if (sprite->callback != NULL) {
            sprite->callback(sprite);
            return;
        }
        if (sprite->unkB0 >= 0) {
            func_800245D8(sprite, sprite->unkB0);
        }
        sprite->state28 = 0;
        return;
    case 0x98: {
        C11CCSprite* t = sprite->unk70;

        if (t == NULL) {
            break;
        }
        sprite->wait = 1;
        if (t->animAF != sprite->unk8D) {
            break;
        }
        if (t->state28 == 2) {
            return;
        }
        break;
    }
    case 0x82: {
        s32 saved;

        if (sprite->callback != NULL) {
            sprite->callback(sprite);
        }
        saved = sprite->unk10;
        func_800245D8(sprite, sprite->animAF);
        sprite->unk10 = saved;
        sprite->wait = 0;
        func_800C11CC(sprite);
        return;
    }
    case 0x81:
        if (sprite->animAF == 0x3F) {
            goto end;
        }
        sprite->wait = 0;
        if (sprite->callback != NULL) {
            sprite->callback(sprite);
        }
        sprite->state28 = 1;
        return;
    case 0xE4: {
        u8 n = AnimScriptStackPopU8(sprite);

        if (n == 0) {
            break;
        }
        n--;
        AnimScriptStackPushU8(sprite, n);
        /* fall through: loop counter not exhausted, take the jump */
    }
    case 0xE1:
        sprite->script += ((s16)(a[1] << 8) | a[0]);
        goto loop;
    case 0xA7: {
        s32 w;

        sprite->script += D_8004FC40[op];
        if (a[0] & 0x80) {
            s32 t = (a[0] & 0x7F) + 1;

            *(s32*)g_WorkListCurTimer__b = t;
            sprite->wait++;
            return;
        }
        w = (a[0] + 2) * sprite->speed / 256;
        if (w == 0) {
            w = 1;
        }
        sprite->wait += w;
        return;
    }
    case 0xC8:
        func_8001FBE4(sprite, a[0], func_8001FBA4(sprite, a + 1));
        break;
    default:
        func_8001FBE4(sprite, op, a);
        break;
    case 0x9F:
        break;
    }
    sprite->script += D_8004FC40[op];
    goto loop;
}

/* Retail pads this TU's .text to 0x10A0 bytes and 5255C.data follows at once
 * (the overlay links with SUBALIGN(4)), so keep the trailing pad word.  It is
 * wrapped like INCLUDE_ASM: cc1 defers function bodies to the end of the TU,
 * so a bare file-scope __asm__ would land ahead of func_800C11CC. */
void __maspsx_include_asm_hack_pad_800C2048() {
    __asm__(
        ".text # maspsx-keep\n"
        "\t.space 4 # maspsx-keep\n"
    );
}
#endif
