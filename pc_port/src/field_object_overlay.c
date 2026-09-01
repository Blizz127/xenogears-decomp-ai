/*
 * Field object overlay (archive 0x6B9, retail load address 0x801DC000).
 *
 * Phase-2B treated func_801E742C / 7D14 as mid-function entries of
 * member_change_menu.bin. That was the wrong overlay: during field play
 * archive 0x6B9 occupies 0x801DC000, and 0x801E72CC..0x801E8330 are
 * standalone functions in that blob (func_801E72CC is already ported in
 * game_overrides.c).
 *
 * This file ports the instantiate + draw path MAP16 needs so the four
 * registered object archives become field models (func_8002CB54 /
 * func_8002C8CC / func_8002C700), the same pipeline FieldLoad uses.
 */
#include "common.h"
#include "field/main.h"
#include "field/actor.h"
#include "psyq/libgte.h"
#include "psyq/libgpu.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HEAP_USER_MASA 0x4
extern void* HeapAlloc(u32 size, u32 flags);
extern void HeapChangeCurrentUser(u32 userTag, char** pContentTypes);
extern s32 func_8002C644(u8* a0);

extern int func_8002C3E8(u8* pModel);
extern void func_8002CB54(u8* modelData, u32* out1, u32* out2);
extern void func_8002C8CC(u8* a0, void* a1, s32 a2);
extern s32 func_8002C4BC(u8* pBlock);
extern void func_8002CC10(u16 x, u16 y);
extern void func_8002CC74(u16 a0, u16 a1);
extern s32 func_8002C700(void* a0, void* a1, void* a2, s32 a3);
extern s32 func_8002DDE4(void* pImageData, s32 texMode, s32 texX, s32 texY,
                         s32 clutMode, s32 clutX, s32 clutY);
extern unsigned int ResolveArchiveEntryPointers(u32* pFile);
extern u16 D_80059308;
extern u16 D_8005930C;
extern s32 D_80050104;
extern s32 D_80059578;
extern int g_FieldNumActors;
extern FieldActor* volatile g_FieldActors;

/* Cross-TU table consumed by func_801E72CC (game_overrides.c) and the
 * field object-anim path. 10 slots, matching overlay slti 0xA. */
u32 D_801E8670[10];
void* D_801E8644;
/* Retail 0x801E86A8: clip-tick workspace passed to 35D0/36BC/39F0. */
u8 D_801E86A8[0x80];
s16 D_801E86B0;
s16 D_801E863C;

#define OVLY_NODE_STRIDE 0x7C
#define OVLY_GROUP_STRIDE 0x38
#define OVLY_OBJ_SIZE 0x134
#define OVLY_SLOT_MAX 10
#define OVLY_CLIP_OPS 0x71

extern MATRIX* RotMatrix(SVECTOR* r, MATRIX* m);

typedef struct OvlyPtrTab {
    u32* ptrs;
    s32 count;
} OvlyPtrTab;

static OvlyPtrTab s_ptrTab[OVLY_SLOT_MAX];
static int s_inited;

static int OvlyDiag(void) {
    static int s_on = -1;
    if (s_on < 0) {
        const char* env = getenv("XENO_FIELD_DIAG");
        s_on = (env != NULL && env[0] != '\0' && env[0] != '0');
    }
    return s_on;
}

static void OvlyIdentMatrix(MATRIX* m, s32 tx, s32 ty, s32 tz) {
    memset(m, 0, sizeof(*m));
    m->m[0][0] = 0x1000;
    m->m[1][1] = 0x1000;
    m->m[2][2] = 0x1000;
    m->t[0] = tx;
    m->t[1] = ty;
    m->t[2] = tz;
}

/* Copy the field actor that owns this overlay slot into the root
 * translation. Retail 801E8330/36BC updates node matrices from animation;
 * without that, identity-at-origin leaves every overlay vert behind the
 * MAP16 script camera (dEmits=0). Slot is stamped at ActorData+0x12C bits
 * 13..15 by func_800A1364. */
static int OvlyBindActorPos(s32 slot, u8* node) {
    s32 i;
    if (node == NULL || g_FieldActors == NULL) {
        return 0;
    }
    for (i = 0; i < g_FieldNumActors; i++) {
        u8* actor = (u8*)g_FieldActors + i * 0x5C;
        u8* ad = (u8*)(uintptr_t)*(u32*)(actor + 0x4C);
        u32 flags4;
        s32 actorSlot;
        s32 tx;
        s32 ty;
        s32 tz;
        if (ad == NULL) {
            continue;
        }
        flags4 = *(u32*)(ad + 4);
        if ((flags4 & 0x2000) == 0) {
            continue;
        }
        actorSlot = (s32)((*(u32*)(ad + 0x12C) >> 13) & 7);
        if (actorSlot != slot) {
            continue;
        }
        tx = *(s32*)(actor + 0x20);
        ty = *(s32*)(actor + 0x24);
        tz = *(s32*)(actor + 0x28);
        *(s32*)(node + 0x5C) = tx;
        *(s32*)(node + 0x60) = ty;
        *(s32*)(node + 0x64) = tz;
        return 1;
    }
    return 0;
}

/* Extra bytes after the 2-byte opcode word, from overlay 39F0 handlers.
 * Wait (01) is special-cased. 04-07 etc. are no-ops at 801E5974. */
static const u8 s_clipExtra[OVLY_CLIP_OPS] = {
    [0x01] = 2, [0x11] = 2, [0x13] = 4, [0x14] = 2, [0x15] = 2, [0x16] = 2,
    [0x17] = 2, [0x18] = 2, [0x1A] = 12, [0x1D] = 18, [0x22] = 2, [0x23] = 2,
    [0x25] = 8, [0x28] = 2, [0x29] = 2, [0x2E] = 2, [0x31] = 2, [0x32] = 2,
    [0x33] = 2, [0x34] = 2, [0x35] = 2, [0x36] = 4, [0x37] = 2, [0x38] = 4,
    [0x39] = 2, [0x3B] = 2, [0x3C] = 2, [0x40] = 12, [0x41] = 6, [0x44] = 6,
    [0x45] = 6, [0x46] = 6, [0x47] = 6, [0x49] = 6, [0x4B] = 6, [0x4C] = 6,
    [0x4D] = 6, [0x4E] = 6, [0x50] = 6, [0x54] = 2, [0x55] = 4, [0x56] = 2,
    [0x5B] = 2, [0x5C] = 2, [0x5D] = 2, [0x5E] = 2, [0x5F] = 2, [0x62] = 8,
    [0x63] = 2, [0x64] = 2, [0x6B] = 2, [0x6E] = 2, [0x70] = 2,
};

/* Retail 801E3534: install clip table and zero per-object anim state. */
static void OvlyClipInit(u8* obj, u8* clipTable, u32 clipAux) {
    s32 i;
    *(u16*)(obj + 0x3C) = 0xFFFF;
    obj[0x5C] = 0xFF;
    obj[0x39] = 0x6B;
    *(u32*)(obj + 8) = (u32)(uintptr_t)clipTable;
    *(u32*)(obj + 0xC) = 0;
    *(u32*)(obj + 0x10) = 0;
    *(u32*)(obj + 0x14) = clipAux;
    *(u32*)(obj + 0x18) = 0;
    obj[0x2B] = 0;
    *(s16*)(obj + 0x98) = -1;
    *(u16*)(obj + 0x58) = 0;
    obj[0x35] = 0;
    obj[0x37] = 0;
    obj[0x38] = 0;
    *(s16*)(obj + 0x3A) = -1;
    for (i = 0x70; i <= 0x8C; i += 2) {
        *(u16*)(obj + i) = 0;
    }
    *(u16*)(obj + 0x8E) = 1;
    obj[0x36] = 0;
    *(s16*)(obj + 0x1E) = -1;
}

static void OvlyRebuildNodeMatrix(u8* node) {
    MATRIX rot;
    SVECTOR r;
    if (node == NULL) {
        return;
    }
    r.vx = *(s16*)(node + 0x54);
    r.vy = *(s16*)(node + 0x56);
    r.vz = *(s16*)(node + 0x58);
    RotMatrix(&r, &rot);
    /* Retail DC5C0 keeps rotation at +0x2C (used by 39F0 vel) and the
     * translation-bearing matrix at +0xC. */
    memcpy(node + 0x2C, &rot, sizeof(MATRIX));
    rot.t[0] = *(s32*)(node + 0x5C);
    rot.t[1] = *(s32*)(node + 0x60);
    rot.t[2] = *(s32*)(node + 0x64);
    memcpy(node + 0xC, &rot, sizeof(MATRIX));
}

/* Retail 801E6910: clip-aux table lookup. */
static u8* OvlyClipLookup(u8* obj, s32 index) {
    u32 base;
    if (obj == NULL) {
        return NULL;
    }
    index &= 0xFF;
    if (index >= 0xFE) {
        index = obj[0x2A] & 0x7F;
    }
    if (index < 0x40) {
        base = *(u32*)(obj + 0x14);
        if (base == 0) {
            return NULL;
        }
        return (u8*)(uintptr_t)*(u32*)((u8*)(uintptr_t)base + index * 4 + 4);
    }
    base = *(u32*)(obj + 0x18);
    if (base == 0) {
        return NULL;
    }
    return (u8*)(uintptr_t)*(u32*)((u8*)(uintptr_t)base + index * 4 - 0xFC);
}

/* Retail 801DEF10 (bounded): pose s16 triples onto child rot (+0x54) and
 * translation (+0x5C). Flag bit 0 skips rot, bit 1 skips trans. */
static void OvlyApplyPose(u8* root, u8* pose) {
    s32 nnodes;
    s32 i;
    s32 irot;
    s32 itrans;
    s16* src;
    u16 flags;
    u16 nrot;
    u16 ntrans;
    if (root == NULL || pose == NULL) {
        return;
    }
    nnodes = *(u16*)(root + 0xA);
    if (nnodes < 2 || nnodes > 0x40) {
        return;
    }
    flags = *(u16*)(pose + 4);
    nrot = *(u16*)(pose + 0xC);
    ntrans = *(u16*)(pose + 0xE);
    src = (s16*)(pose + 0x18);
    if (*(s16*)(pose + 6) == 0) {
        src += (s32)(nrot + 1) * 3;
    }
    irot = 0;
    itrans = 0;
    for (i = 1; i < nnodes; i++) {
        u8* node = root + i * OVLY_NODE_STRIDE;
        if ((flags & 1) == 0 && irot < (s32)nrot) {
            *(s16*)(node + 0x54) = src[0];
            *(s16*)(node + 0x56) = src[1];
            *(s16*)(node + 0x58) = src[2];
            src += 3;
            irot++;
        }
        if ((flags & 2) == 0 && itrans < (s32)ntrans) {
            *(s32*)(node + 0x5C) = (s32)src[0];
            *(s32*)(node + 0x60) = (s32)src[1];
            *(s32*)(node + 0x64) = (s32)src[2];
            src += 3;
            itrans++;
        }
    }
    if (OvlyDiag()) {
        static s32 s_poseMeta;
        if (s_poseMeta < 12) {
            fprintf(stderr,
                    "[obj-ovly] pose-meta flags=%04x nrot=%u ntrans=%u nnodes=%d "
                    "skip6=%d applied-rot=%d applied-t=%d\n",
                    (unsigned)flags, (unsigned)nrot, (unsigned)ntrans, (int)nnodes,
                    (int)(*(s16*)(pose + 6) == 0), (int)irot, (int)itrans);
            s_poseMeta++;
        }
    }
}

static void OvlyRebuildTree(u8* root) {
    s32 nnodes;
    s32 n;
    if (root == NULL) {
        return;
    }
    nnodes = *(u16*)(root + 0xA);
    OvlyRebuildNodeMatrix(root);
    if (nnodes < 2 || nnodes > 0x40) {
        return;
    }
    for (n = 1; n < nnodes; n++) {
        u8* child = root + n * OVLY_NODE_STRIDE;
        MATRIX local;
        SVECTOR r;
        r.vx = *(s16*)(child + 0x54);
        r.vy = *(s16*)(child + 0x56);
        r.vz = *(s16*)(child + 0x58);
        RotMatrix(&r, &local);
        local.t[0] = *(s32*)(child + 0x5C);
        local.t[1] = *(s32*)(child + 0x60);
        local.t[2] = *(s32*)(child + 0x64);
        CompMatrix((MATRIX*)(root + 0xC), &local, (MATRIX*)(child + 0xC));
    }
}

/* Retail 801E39F0 (bounded): apply velocity deltas to the root node, then
 * walk the clip bytecode. Wait/end/zero-delta are live; other opcodes skip
 * their payload so IP stays in sync. */
static void OvlyClipTick(u8* obj, s32 ticks, s32 startMode) {
    u8* node;
    u8* ip;
    u8* pkt;
    s32 n;
    s32 guard;
    static s32 s_clipLogs;

    if (obj == NULL) {
        return;
    }
    node = (u8*)(uintptr_t)*(u32*)(obj + 4);
    ip = (u8*)(uintptr_t)*(u32*)(obj + 0x10);
    if (ip == NULL) {
        return;
    }
    if (ticks < 0) {
        ticks = 0;
    }
    if (ticks > 8) {
        ticks = 8;
    }
    if (ticks > 0 && node != NULL) {
        for (n = 0; n < ticks; n++) {
            *(u16*)(obj + 0x70) = (u16)(*(u16*)(obj + 0x70) + *(u16*)(obj + 0x76));
            *(u16*)(obj + 0x72) = (u16)(*(u16*)(obj + 0x72) + *(u16*)(obj + 0x78));
            *(u16*)(obj + 0x74) = (u16)(*(u16*)(obj + 0x74) + *(u16*)(obj + 0x7A));
            *(u16*)(obj + 0x7C) = (u16)(*(u16*)(obj + 0x7C) + *(u16*)(obj + 0x82));
            *(u16*)(obj + 0x7E) = (u16)(*(u16*)(obj + 0x7E) + *(u16*)(obj + 0x84));
            *(u16*)(obj + 0x80) = (u16)(*(u16*)(obj + 0x80) + *(u16*)(obj + 0x86));
            *(s16*)(node + 0x54) = (s16)(*(s16*)(node + 0x54) +
                                         ((s16)*(u16*)(obj + 0x70) >> 3));
            *(s16*)(node + 0x56) = (s16)(*(s16*)(node + 0x56) +
                                         ((s16)*(u16*)(obj + 0x72) >> 3));
            *(s16*)(node + 0x58) = (s16)(*(s16*)(node + 0x58) +
                                         ((s16)*(u16*)(obj + 0x74) >> 3));
            /* Retail 39F0: scale vel by node+0x4C, rotate by +0x2C, then
             * add obj+0x1C * result / 0x1000 onto root translation. */
            {
                SVECTOR vel;
                VECTOR out;
                s32 scale = (s32)*(s16*)(obj + 0x1C);
                vel.vx = (s16)((s32)*(s16*)(obj + 0x7C) * (s32)*(s16*)(node + 0x4C) >> 12);
                vel.vy = (s16)((s32)*(s16*)(obj + 0x7E) * (s32)*(s16*)(node + 0x4E) >> 12);
                vel.vz = (s16)((s32)*(s16*)(obj + 0x80) * (s32)*(s16*)(node + 0x50) >> 12);
                ApplyMatrix((MATRIX*)(node + 0x2C), &vel, &out);
                *(s32*)(node + 0x5C) += (s32)((scale * out.vx) >> 12);
                *(s32*)(node + 0x60) += (s32)((scale * out.vy) >> 12);
                *(s32*)(node + 0x64) += (s32)((scale * out.vz) >> 12);
            }
        }
    }

    guard = 0;
    while (guard++ < 0x100) {
        u8 op;
        u8 param;
        s32 extra;
        pkt = ip;
        op = ip[0];
        param = ip[1];
        ip += 2;
        if (op >= OVLY_CLIP_OPS) {
            ip = pkt;
            break;
        }
        if (op == 0x00) {
            ip = pkt;
            break;
        }
        if (op == 0x01) {
            if (startMode) {
                ip = pkt;
                break;
            }
            {
                u16 wait = *(u16*)ip;
                s16 cur;
                ip += 2;
                cur = (s16)(*(u16*)(obj + 0x40) + (u16)ticks);
                *(u16*)(obj + 0x40) = (u16)cur;
                if (cur < (s16)wait) {
                    ip = pkt;
                    break;
                }
                *(u16*)(obj + 0x40) = 0;
                ticks = 0;
            }
            continue;
        }
        if (op == 0x0C) {
            for (n = 0x70; n <= 0x86; n += 2) {
                *(u16*)(obj + n) = 0;
            }
            continue;
        }
        if (op == 0x10 || op == 0x13) {
            s32 idx = param;
            if (op == 0x13) {
                u16 w1 = *(u16*)ip;
                ip += 2;
                ip += 2; /* second extra word (channel ids) */
                idx = (s32)(w1 & 0xFF);
            }
            {
                u8* pose = OvlyClipLookup(obj, idx);
                if (pose != NULL && (uintptr_t)pose > 0x10000) {
                    OvlyApplyPose(node, pose);
                    if (OvlyDiag() && s_clipLogs < 24 && node != NULL &&
                        *(u16*)(node + 0xA) >= 2) {
                        u8* ch = node + OVLY_NODE_STRIDE;
                        fprintf(stderr,
                                "[obj-ovly] pose op=%02x idx=%d pose=%p "
                                "child-r=(%d,%d,%d) child-t=(%d,%d,%d)\n",
                                op, idx, (void*)pose,
                                (int)*(s16*)(ch + 0x54),
                                (int)*(s16*)(ch + 0x56),
                                (int)*(s16*)(ch + 0x58),
                                (int)*(s32*)(ch + 0x5C),
                                (int)*(s32*)(ch + 0x60),
                                (int)*(s32*)(ch + 0x64));
                    }
                }
            }
            continue;
        }
        if (op == 0x21) {
            *(u16*)(obj + 0x3C) = param;
            ip = pkt;
            break;
        }
        if (op == 0x17) {
            ip = pkt;
            break;
        }
        extra = (s32)s_clipExtra[op];
        ip += extra;
        (void)param;
    }
    *(u32*)(obj + 0x10) = (u32)(uintptr_t)ip;
    if (OvlyDiag() && s_clipLogs < 48) {
        fprintf(stderr,
                "[obj-ovly] clip-tick ip=%p op=%02x rx=%d ry=%d rz=%d "
                "d70=%04x t=(%d,%d,%d)\n",
                (void*)ip, ip != NULL ? ip[0] : 0xFF,
                node ? (int)*(s16*)(node + 0x54) : 0,
                node ? (int)*(s16*)(node + 0x56) : 0,
                node ? (int)*(s16*)(node + 0x58) : 0,
                (unsigned)*(u16*)(obj + 0x70),
                node ? (int)*(s32*)(node + 0x5C) : 0,
                node ? (int)*(s32*)(node + 0x60) : 0,
                node ? (int)*(s32*)(node + 0x64) : 0);
        s_clipLogs++;
    }
}

/* Retail 801E35D0: queue a clip if one is already running, else bind
 * table[anim] and start the interpreter. */
static void OvlyClipBind(u8* obj, u8* src, s32 anim) {
    u8 q;
    u8* table;
    u32 clip;
    static s32 s_bindClipLogs;

    if (obj == NULL || src == NULL) {
        return;
    }
    q = obj[0x2B];
    if (q != 0) {
        if (q < 5) {
            obj[0x2B] = (u8)(q + 1);
        }
        q = obj[0x2B];
        obj[0x2A + q] = src[0x20];
        obj[0x2E + q] = (u8)anim;
        return;
    }
    table = (u8*)(uintptr_t)*(u32*)(src + 8);
    if (table == NULL) {
        return;
    }
    if (anim < 0x50) {
        clip = *(u32*)(table + anim * 4);
    } else {
        u8* aux = (u8*)(uintptr_t)*(u32*)(src + 0xC);
        if (aux == NULL) {
            return;
        }
        clip = *(u32*)((u8*)(uintptr_t)*(u32*)(aux + 4) + anim * 4 - 0x138);
    }
    if (clip < 0x10000) {
        if (OvlyDiag() && s_bindClipLogs < 8) {
            fprintf(stderr, "[obj-ovly] clip-bind reject anim=%d clip=%08x table=%p\n",
                    (int)anim, clip, (void*)table);
            s_bindClipLogs++;
        }
        return;
    }
    *(u32*)(obj + 0x10) = clip;
    *(u16*)(obj + 0x42) = 0;
    *(u16*)(obj + 0x40) = 0;
    *(u32*)(obj + 0x50) = 0;
    *(u32*)(obj + 0x54) = 0;
    *(u32*)(obj + 0x4C) = 0;
    obj[0x23] = 0;
    *(u16*)(obj + 0x10A) = (u16)D_801E863C;
    if (OvlyDiag() && s_bindClipLogs < 12) {
        u8* p = (u8*)(uintptr_t)clip;
        fprintf(stderr,
                "[obj-ovly] clip-bind anim=%d clip=%p bytes="
                "%02x%02x %02x%02x %02x%02x %02x%02x %02x%02x %02x%02x "
                "%02x%02x %02x%02x %02x%02x %02x%02x %02x%02x %02x%02x "
                "%02x%02x %02x%02x %02x%02x %02x%02x\n",
                (int)anim, (void*)(uintptr_t)clip,
                p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7],
                p[8], p[9], p[10], p[11], p[12], p[13], p[14], p[15],
                p[16], p[17], p[18], p[19], p[20], p[21], p[22], p[23],
                p[24], p[25], p[26], p[27], p[28], p[29], p[30], p[31]);
        s_bindClipLogs++;
    }
    OvlyClipTick(obj, 1, 1);
}

void func_801E36BC(u8* obj, void* ctx, s32 ticks, s32 unused) {
    u8* node;
    (void)ctx;
    (void)unused;
    if (obj == NULL || obj[0x34] == 0) {
        return;
    }
    if (ticks <= 0) {
        ticks = 1;
    }
    OvlyClipTick(obj, ticks, 0);
    node = (u8*)(uintptr_t)*(u32*)(obj + 4);
    if (node != NULL) {
        OvlyRebuildTree(node);
    }
}

static void OvlyDumpVerts(u8* model, unsigned idx) {
    s16* v;
    u8* cmds;
    if (!OvlyDiag() || model == NULL) {
        return;
    }
    v = (s16*)(uintptr_t)*(u32*)(model + 8);
    cmds = (u8*)(uintptr_t)*(u32*)(model + 0x10);
    if (v == NULL) {
        return;
    }
    fprintf(stderr,
            "[obj-ovly] verts idx=%u v0=(%d,%d,%d) v1=(%d,%d,%d) v2=(%d,%d,%d) "
            "cmd0=%02x nprim=%d\n",
            idx, (int)v[0], (int)v[1], (int)v[2],
            (int)v[4], (int)v[5], (int)v[6],
            (int)v[8], (int)v[9], (int)v[10],
            cmds != NULL ? cmds[0] : 0xFF,
            cmds != NULL ? (int)*(s16*)(cmds + 2) : -1);
}

static int OvlySanityCount(s32 n) {
    return n > 0 && n < 0x400;
}

static int OvlySanitySize(s32 n) {
    return n > 0 && n < 0x200000;
}

/* After C8CC, untextured GPU codes (0x20/0x30) or a zero tpage make PsyX
 * shade vertex color only. Force the textured bit when missing, but keep
 * C8CC's merged tpage/CLUT: CC74/CD24 already OR the file palette nibble
 * onto the dest (e.g. 3f05 = x=80,y=252). Overwriting with GetClut(0,252)
 * drops that nibble and samples empty VRAM in the FB gap. */
static void OvlyStampTpageClut(u8* work, s32 payload, u16 tpage, u16 clut) {
    u8* p;
    u8* end;
    s32 n = 0;
    s32 nTex = 0;
    s32 nForced = 0;

    if (work == NULL || payload < 0x10) {
        return;
    }
    p = work;
    end = work + payload;
    while (p + 8 < end) {
        u8 len = p[3];
        u8 code;
        u16 pktClut;
        u16 pktTpage;
        s32 step;
        if (len < 4 || len > 12) {
            break;
        }
        step = ((s32)len + 1) * 4;
        if (p + step > end) {
            break;
        }
        code = p[7];
        pktClut = *(u16*)(p + 0x0E);
        pktTpage = *(u16*)(p + 0x16);
        if (OvlyDiag() && n < 6) {
            fprintf(stderr,
                    "[obj-ovly] pkt%d len=%u code=%02x rgb=%02x%02x%02x "
                    "clut=%04x tpage=%04x uv0=%02x%02x uv1=%02x%02x uv2=%02x%02x\n",
                    (int)n, (unsigned)len, code, p[4], p[5], p[6],
                    (unsigned)pktClut, (unsigned)pktTpage,
                    p[0x0C], p[0x0D], p[0x14], p[0x15], p[0x1C], p[0x1D]);
        }
        if (len == 7 || len == 9) {
            u8 textured = (u8)((code & 0x04) != 0);
            if (!textured) {
                /* Keep gouraud/semi bits; set textured + poly. */
                p[7] = (u8)((code & 0x13) | ((len == 9) ? 0x2C : 0x24));
                nForced++;
            }
            /* Lighting is a stub (NormalLightCol), so D814 falls back to
             * RGB 808080. Modulating the DDE4 atlas by 50% gray crushes
             * 4-bit earth-tone palettes to near-black / night-blue fill.
             * SetShadeTex (code bit 0) samples the uploaded VRAM as-is. */
            if (p[4] == 0x80 && p[5] == 0x80 && p[6] == 0x80) {
                p[7] = (u8)(p[7] | 0x01);
            }
            if (pktTpage == 0) {
                *(u16*)(p + 0x16) = tpage;
            }
            if (pktClut == 0) {
                *(u16*)(p + 0x0E) = clut;
            }
            nTex++;
        }
        p += step;
        n++;
        if (n > 0x800) {
            break;
        }
    }
    if (OvlyDiag()) {
        fprintf(stderr,
                "[obj-ovly] stamp n=%d tex=%d forced=%d latch tpage=%04x clut=%04x\n",
                (int)n, (int)nTex, (int)nForced,
                (unsigned)tpage, (unsigned)clut);
    }
}

static void OvlyResolve(void* p) {
    u32* file = (u32*)p;
    if (file == NULL) {
        return;
    }
    if (!OvlySanityCount((s32)file[0])) {
        if (OvlyDiag()) {
            fprintf(stderr, "[obj-ovly] skip Resolve count=%d p=%p\n", (int)file[0], p);
        }
        return;
    }
    ResolveArchiveEntryPointers(file);
}

/* 801DC22C: relocate a model blob (func_8002C3E8) and build a pointer table
 * to each 0x38-stride group starting at +0x10. */
static OvlyPtrTab* OvlyBuildPtrTab(u8* pModel, OvlyPtrTab* out) {
    s32 count;
    s32 i;
    u32* ptrs;

    HeapChangeCurrentUser(HEAP_USER_MASA, NULL);
    count = func_8002C3E8(pModel);
    out->ptrs = NULL;
    out->count = count;
    if (!OvlySanityCount(count)) {
        if (OvlyDiag()) {
            fprintf(stderr, "[obj-ovly] C3E8 count=%d model=%p\n", (int)count, (void*)pModel);
        }
        return out;
    }
    ptrs = (u32*)HeapAlloc((u32)count * 4u, 0);
    out->ptrs = ptrs;
    if (ptrs == NULL) {
        return out;
    }
    for (i = 0; i < count; i++) {
        ptrs[i] = (u32)(uintptr_t)(pModel + 0x10 + i * OVLY_GROUP_STRIDE);
    }
    if (OvlyDiag()) {
        fprintf(stderr, "[obj-ovly] ptrtab model=%p groups=%d first=%p size34=%d\n",
               (void*)pModel, (int)count,
               (void*)(uintptr_t)ptrs[0],
               (int)*(s32*)((u8*)(uintptr_t)ptrs[0] + 0x34));
    }
    return out;
}

static void OvlyInitNode(u8* node, s32 index, u8* parent) {
    memset(node, 0, OVLY_NODE_STRIDE);
    node[4] = 1;
    node[5] = 1;
    node[6] = 1;
    *(u16*)(node + 8) = 0xFFFF;
    *(u16*)(node + 0xA) = (u16)index;
    *(s16*)(node + 0x4C) = 0x1000;
    *(s16*)(node + 0x4E) = 0x1000;
    *(s16*)(node + 0x50) = 0x1000;
    OvlyIdentMatrix((MATRIX*)(node + 0xC), 0, 0, 0);
    if (parent != NULL) {
        *(u32*)node = (u32)(uintptr_t)parent;
    }
}

/* 801DC2D0: allocate a 0x7C-stride node array and run FieldLoad's model
 * build (CB54 + C8CC + memcpy of the double-buffer half) for each group
 * named by the u16 pair list. */
static u8* OvlyBuildNodes(OvlyPtrTab* tab, u16* list, s32 buildMode,
                          s16 texX, s16 texY, s16 clutX, s16 clutY) {
    s32 nrec = 0;
    s32 nnodes;
    u8* root;
    u8* node;
    s32 i;

    if (tab == NULL || tab->ptrs == NULL || !OvlySanityCount(tab->count)) {
        return NULL;
    }

    if (list != NULL) {
        for (;;) {
            u16 idx = list[nrec * 2];
            if (idx == 0xFFFF) {
                break;
            }
            if ((s32)idx >= tab->count) {
                break;
            }
            nrec++;
            if (nrec > tab->count + 4) {
                break;
            }
        }
    }
    if (nrec == 0) {
        nrec = tab->count;
        list = NULL;
    }
    if (OvlyDiag() && list != NULL) {
        fprintf(stderr, "[obj-ovly] named list nrec=%d pairs0=%04x/%04x count=%d\n",
                (int)nrec, (unsigned)list[0], (unsigned)list[1], (int)tab->count);
    }
    nnodes = nrec + 1;
    root = (u8*)HeapAlloc((u32)nnodes * OVLY_NODE_STRIDE, 0);
    if (root == NULL) {
        return NULL;
    }
    OvlyInitNode(root, nnodes, NULL);
    node = root + OVLY_NODE_STRIDE;
    for (i = 0; i < nrec; i++, node += OVLY_NODE_STRIDE) {
        u16 modelIdx = (list != NULL) ? list[i * 2] : (u16)i;
        u16 parentIdx = (list != NULL) ? list[i * 2 + 1] : 0xFFFF;
        u8* parent = NULL;
        u8* model;
        s32 payload;

        if ((s32)modelIdx >= tab->count) {
            OvlyInitNode(node, i + 1, NULL);
            continue;
        }
        if (parentIdx != 0xFFFF && (s32)parentIdx < nnodes) {
            parent = root + (s32)parentIdx * OVLY_NODE_STRIDE;
        }
        OvlyInitNode(node, i + 1, parent);
        /* Retail DC2D0 stores the ptrtab index at node+8; 7D14/DCC80 looks
         * C700 a0 up as ptrtab[*(u16*)(node+8)]. OvlyInitNode leaves 0xFFFF
         * (skip). */
        *(u16*)(node + 8) = modelIdx;
        model = (u8*)(uintptr_t)tab->ptrs[modelIdx];
        {
            u16 ngrp = *(u16*)(model + 6);
            u8* cmds = (u8*)(uintptr_t)*(u32*)(model + 0x10);
            s32 cmdCount = (cmds != NULL) ? (int)*(s16*)(cmds + 2) : -1;
            u8 cmd0 = cmds != NULL ? cmds[0] : 0xFF;
            if (OvlyDiag() && i < 4) {
                fprintf(stderr,
                        "[obj-ovly] group idx=%u flg=%04x ngrp=%u p8=%p p10=%p "
                        "p18=%p size34=%d cmd0=%02x cmdCount=%d\n",
                        (unsigned)modelIdx,
                        (unsigned)*(u16*)model,
                        (unsigned)ngrp,
                        (void*)(uintptr_t)*(u32*)(model + 8),
                        (void*)cmds,
                        (void*)(uintptr_t)*(u32*)(model + 0x18),
                        (int)*(s32*)(model + 0x34),
                        cmd0, cmdCount);
            }
            /* FieldLoad model headers keep the mesh-group count at +0x06
             * (pines log groups=2). Overlay 0x38 records often store 0
             * there after C3E8, which makes C8CC/C700 skip the stream
             * (ret=1, dEmits=0). Only force a count of 1 when the command
             * stream looks like a real prim group. */
            if (ngrp == 0 && cmds != NULL && OvlySanitySize(*(s32*)(model + 0x34)) &&
                cmd0 <= 0x10 && cmdCount > 0 && cmdCount < 0x4000) {
                *(u16*)(model + 6) = 1;
            }
            if (i < 8) {
                OvlyDumpVerts(model, (unsigned)modelIdx);
            }
        }
        payload = *(s32*)(model + 0x34);
        if (!OvlySanitySize(payload)) {
            if (OvlyDiag()) {
                fprintf(stderr, "[obj-ovly] skip group idx=%u payload=%d\n",
                       (unsigned)modelIdx, (int)payload);
            }
            continue;
        }
        func_8002CB54(model, (u32*)(node + 0x68), (u32*)(node + 0x6C));
        if (*(u32*)(node + 0x68) == 0) {
            continue;
        }
        if (buildMode != 0) {
            func_8002CC10((u16)texX, (u16)texY);
            func_8002CC74((u16)clutX, (u16)clutY);
            D_80059308 = GetTPage(0, 0, (int)texX, (int)texY);
            D_8005930C = GetClut((int)clutX, (int)clutY);
        }
        func_8002C8CC(model, (void*)(uintptr_t)*(u32*)(node + 0x68), buildMode);
        if (buildMode != 0) {
            u16 tpage = GetTPage(0, 0, (int)texX, (int)texY);
            u16 clut = GetClut((int)clutX, (int)clutY);
            OvlyStampTpageClut((u8*)(uintptr_t)*(u32*)(node + 0x68),
                               payload, tpage, clut);
        }
        memcpy((void*)(uintptr_t)*(u32*)(node + 0x6C),
               (void*)(uintptr_t)*(u32*)(node + 0x68), (size_t)payload);
        *(u32*)(node + 0x70) = (u32)(uintptr_t)model;
    }
    if (OvlyDiag()) {
        fprintf(stderr, "[obj-ovly] nodes root=%p nrec=%d groups=%d mode=%d\n",
               (void*)root, (int)nrec, (int)tab->count, (int)buildMode);
    }
    return root;
}

void func_801E738C(s32 arg0) {
    s32 i;

    (void)arg0;
    HeapChangeCurrentUser(HEAP_USER_MASA, NULL);
    if (!s_inited) {
        memset(D_801E8670, 0, sizeof(D_801E8670));
        memset(s_ptrTab, 0, sizeof(s_ptrTab));
        s_inited = 1;
    }
    for (i = 0; i < OVLY_SLOT_MAX; i++) {
        D_801E8670[i] = 0;
        s_ptrTab[i].ptrs = NULL;
        s_ptrTab[i].count = 0;
    }
    if (OvlyDiag()) {
        fprintf(stderr, "[obj-ovly] 738C init arg=%d\n", (int)arg0);
    }
}

void func_801E742C(s32 slot, s32 flags, void* pModelArc, void* pTexArc,
                  s32 x, s32 y, s32 z, s32 w, s16* vec) {
    u8* obj;
    u8* tex = (u8*)pTexArc;
    u8* modelArc = (u8*)pModelArc;
    u8* slice = NULL;
    u8* sliceEnd = NULL;
    u8* copy = NULL;
    u16* list = NULL;
    u8* nodes;
    s32 copySize;
    s32 texMode;
    OvlyPtrTab* tab;

    HeapChangeCurrentUser(HEAP_USER_MASA, NULL);
    if (!s_inited) {
        func_801E738C(0);
    }
    if (slot < 0 || slot >= OVLY_SLOT_MAX) {
        return;
    }
    if (D_801E8670[slot] != 0) {
        return;
    }

    if (OvlyDiag()) {
        fprintf(stderr, "[obj-ovly] 742C slot=%d flags=%d model=%p tex=%p xy=(%d,%d) zw=(%d,%d) vec=%p\n",
               (int)slot, (int)flags, pModelArc, pTexArc,
               (int)x, (int)y, (int)z, (int)w, (void*)vec);
        if (modelArc != NULL) {
            fprintf(stderr, "[obj-ovly]  model[0..3]=%08x %08x %08x %08x\n",
                   ((u32*)modelArc)[0], ((u32*)modelArc)[1],
                   ((u32*)modelArc)[2], ((u32*)modelArc)[3]);
        }
        if (tex != NULL) {
            fprintf(stderr, "[obj-ovly]  tex[0..3]=%08x %08x %08x %08x\n",
                   ((u32*)tex)[0], ((u32*)tex)[1],
                   ((u32*)tex)[2], ((u32*)tex)[3]);
        }
    }

    /* Relocate archive pointer tables (ResolveArchiveEntryPointers). */
    if (!(flags & 4) && tex != NULL) {
        OvlyResolve(tex);
        if (*(u32*)(tex + 0x10) != 0) {
            OvlyResolve((void*)(uintptr_t)*(u32*)(tex + 0x10));
        }
    }
    if (!(flags & 1) && modelArc != NULL) {
        OvlyResolve(modelArc);
        if (*(u32*)(modelArc + 8) != 0) {
            OvlyResolve((void*)(uintptr_t)*(u32*)(modelArc + 8));
        }
        if (*(u32*)(modelArc + 4) != 0) {
            u8* sub = (u8*)(uintptr_t)*(u32*)(modelArc + 4);
            OvlyResolve(sub);
            if (*(u32*)(sub + 4) != 0) {
                OvlyResolve((void*)(uintptr_t)*(u32*)(sub + 4));
            }
        }
    }

    obj = (u8*)HeapAlloc(OVLY_OBJ_SIZE, 0);
    if (obj == NULL) {
        return;
    }
    memset(obj, 0, OVLY_OBJ_SIZE);
    D_801E8670[slot] = (u32)(uintptr_t)obj;

    /* Texture upload from tex archive +4. Field calls this with flags==0. */
    texMode = (flags & 0x40) ? 0 : 1;
    if (!(flags & 1) && tex != NULL && *(u32*)(tex + 4) != 0) {
        void* image = (void*)(uintptr_t)*(u32*)(tex + 4);
        if (OvlyDiag()) {
            fprintf(stderr, "[obj-ovly] DDE4 image=%p mode=%d xy=(%d,%d)\n",
                   image, texMode, (int)x, (int)y);
        }
        func_8002DDE4(image, texMode, (s16)x, (s16)y, texMode, (s16)z, (s16)w);
    }

    /* Copy the model slice out of the tex archive: *(tex+8) .. *(tex+0xC).
     * func_80077AB4 HeapFree's D_8005A450 (pTexArc) after this returns. */
    if (tex != NULL) {
        slice = (u8*)(uintptr_t)*(u32*)(tex + 8);
        sliceEnd = (u8*)(uintptr_t)*(u32*)(tex + 0xC);
        if (slice != NULL && sliceEnd > slice &&
            OvlySanitySize((s32)(sliceEnd - slice))) {
            copySize = (s32)(sliceEnd - slice);
            copy = (u8*)HeapAlloc((u32)copySize, 1);
            if (copy != NULL) {
                memcpy(copy, slice, (size_t)copySize);
            }
        }
    }
    if (copy == NULL && modelArc != NULL) {
        /* Fallback: treat the model archive itself as the blob. */
        copy = modelArc;
        copySize = 0;
        if (OvlyDiag()) {
            fprintf(stderr, "[obj-ovly] fallback model-arc as blob %p\n", (void*)modelArc);
        }
    }
    if (copy == NULL) {
        if (OvlyDiag()) {
            fprintf(stderr, "[obj-ovly] no model blob slot=%d\n", (int)slot);
        }
        return;
    }

    tab = OvlyBuildPtrTab(copy, &s_ptrTab[slot]);
    if (modelArc != NULL && *(u32*)(modelArc + 8) != 0) {
        list = (u16*)(uintptr_t)*(u32*)(modelArc + 8);
    }
    nodes = OvlyBuildNodes(tab, list, (flags & 0x40) ? 0 : 2,
                           (s16)x, (s16)y, (s16)z, (s16)w);
    *(u32*)(obj + 4) = (u32)(uintptr_t)nodes;
    *(u32*)obj = (u32)(uintptr_t)tab;
    *(u8*)(obj + 0x34) = 1;
    *(u8*)(obj + 0x20) = (u8)slot;
    if (nodes != NULL && vec != NULL) {
        *(s32*)(nodes + 0x5C) = (s32)vec[0];
        *(s32*)(nodes + 0x60) = (s32)vec[1];
        *(s32*)(nodes + 0x64) = (s32)vec[2];
        OvlyIdentMatrix((MATRIX*)(nodes + 0xC),
                        (s32)vec[0], (s32)vec[1], (s32)vec[2]);
    }
    {
        u8* clipTab = NULL;
        u32 clipAux = 0;
        if (modelArc != NULL && *(u32*)(modelArc + 4) != 0) {
            u8* sub = (u8*)(uintptr_t)*(u32*)(modelArc + 4);
            clipTab = sub + 8;
            clipAux = *(u32*)(sub + 4);
        }
        if (tex != NULL && *(u32*)(tex + 0x10) != 0) {
            u8* hdr = (u8*)(uintptr_t)*(u32*)(tex + 0x10);
            u8* s2 = (u8*)(uintptr_t)*(u32*)(hdr + 4);
            if (s2 != NULL) {
                *(u16*)(obj + 0x1C) = *(u16*)(s2 + 8);
                *(u16*)(obj + 0x24) = *(u16*)(s2 + 2);
                *(u16*)(obj + 0x26) = *(u16*)(s2 + 4);
                *(u16*)(obj + 0x28) = *(u16*)(s2 + 6);
                obj[0x2A] = s2[0xA];
                *(u16*)(obj + 0x4A) = *(u16*)(s2 + 0xC);
            }
        }
        OvlyClipInit(obj, clipTab, clipAux);
        if (!(flags & 0x40)) {
            OvlyClipBind(obj, obj, 0);
        }
    }
    if (copy != modelArc) {
        *(u32*)(obj + 0xA8) = (u32)(uintptr_t)copy;
        (void)func_8002C644(copy);
    }
    if (OvlyDiag()) {
        fprintf(stderr, "[obj-ovly] 742C done slot=%d obj=%p nodes=%p groups=%d vec=(%d,%d,%d)\n",
               (int)slot, (void*)obj, (void*)nodes,
               tab != NULL ? (int)tab->count : 0,
               vec ? (int)vec[0] : 0, vec ? (int)vec[1] : 0,
               vec ? (int)vec[2] : 0);
    }
}

void func_801E7D14(void* sceneData, void* arg1, void* prim, s32 renderContextIndex,
                   s32 arg4) {
    MATRIX* w2s = (MATRIX*)sceneData;
    s32 slot;
    static s32 s_drawLogs;

    (void)arg1;
    (void)arg4;
    if (w2s == NULL || prim == NULL) {
        return;
    }

    for (slot = 0; slot < OVLY_SLOT_MAX; slot++) {
        u8* obj = (u8*)(uintptr_t)D_801E8670[slot];
        u8* root;
        u16 nnodes;
        s32 n;
        s32 emitted = 0;

        if (obj == NULL || obj[0x34] == 0) {
            continue;
        }
        root = (u8*)(uintptr_t)*(u32*)(obj + 4);
        if (root == NULL) {
            continue;
        }
        nnodes = *(u16*)(root + 0xA);
        if (nnodes < 2 || nnodes > 0x200) {
            continue;
        }
        {
            static s32 s_bindLogs;
            /* Actor bind is applied, then we still compose identity-at-origin
             * if no actor owns the slot. Keep the bind: local meshes need it.
             * World-space groups at actor pos will be offset; dump verts to
             * tell the two apart. */
            int bound = OvlyBindActorPos(slot, root);
            /* Retail 7D14 snapshots root translation, ticks 36BC, then
             * stores the per-tick delta at obj+0x128/12C/130 for the
             * flags4&0x20000 walk path in func_80081F80. */
            *(s32*)(obj + 0x11C) = *(s32*)(root + 0x5C);
            *(s32*)(obj + 0x120) = *(s32*)(root + 0x60);
            *(s32*)(obj + 0x124) = *(s32*)(root + 0x64);
            func_801E36BC(obj, D_801E86A8, 1, renderContextIndex);
            *(s32*)(obj + 0x128) = *(s32*)(obj + 0x11C) - *(s32*)(root + 0x5C);
            *(s32*)(obj + 0x12C) = *(s32*)(obj + 0x120) - *(s32*)(root + 0x60);
            *(s32*)(obj + 0x130) = *(s32*)(obj + 0x124) - *(s32*)(root + 0x64);
            if (OvlyDiag() && s_bindLogs < 12) {
                u8* ch = (nnodes >= 2) ? root + OVLY_NODE_STRIDE : NULL;
                fprintf(stderr,
                        "[obj-ovly] bind slot=%d actor=%d t=(%d,%d,%d) nnodes=%u "
                        "child-t=(%d,%d,%d) dlt=(%d,%d,%d)\n",
                        (int)slot, bound,
                        (int)*(s32*)(root + 0x5C),
                        (int)*(s32*)(root + 0x60),
                        (int)*(s32*)(root + 0x64),
                        (unsigned)nnodes,
                        ch ? (int)*(s32*)(ch + 0x5C) : 0,
                        ch ? (int)*(s32*)(ch + 0x60) : 0,
                        ch ? (int)*(s32*)(ch + 0x64) : 0,
                        (int)*(s32*)(obj + 0x128),
                        (int)*(s32*)(obj + 0x12C),
                        (int)*(s32*)(obj + 0x130));
                s_bindLogs++;
            }
        }
        for (n = 1; n < (s32)nnodes; n++) {
            u8* node = root + n * OVLY_NODE_STRIDE;
            u8* model = (u8*)(uintptr_t)*(u32*)(node + 0x70);
            u32 buf0 = *(u32*)(node + 0x68);
            u32 buf1 = *(u32*)(node + 0x6C);
            u32 work = (renderContextIndex != 0 && buf1 != 0) ? buf1 : buf0;
            MATRIX composed;

            if (model == NULL || work == 0) {
                continue;
            }
            if (*(u16*)(model + 6) == 0 && *(u32*)(model + 0x10) != 0) {
                *(u16*)(model + 6) = 1;
            }
            CompMatrix(w2s, (MATRIX*)(node + 0xC), &composed);
            SetRotMatrix(&composed);
            SetTransMatrix(&composed);
            /* Field's func_800748E8 zeroes D_80050104 before func_8002C700
             * so the func_8003101C gate does not discard the mesh. */
            {
                s32 emitsBefore = D_80059578;
                s32 drew;
                u16 ngrp = *(u16*)(model + 6);
                u8* cmds = (u8*)(uintptr_t)*(u32*)(model + 0x10);
                D_80050104 = 0;
                if (OvlyDiag() && s_drawLogs < 8) {
                    SVECTOR* v0 = (SVECTOR*)(uintptr_t)*(u32*)(model + 8);
                    long xy = 0;
                    long p = 0;
                    long flag = 0;
                    long otz = 0;
                    if (v0 != NULL) {
                        otz = RotTransPers(v0, &xy, &p, &flag);
                    }
                    fprintf(stderr,
                            "[obj-ovly] xform slot=%d v0=(%d,%d,%d) otz=%ld "
                            "xy=%08lx flag=%ld t=(%d,%d,%d)\n",
                            (int)slot,
                            v0 ? (int)v0->vx : 0, v0 ? (int)v0->vy : 0,
                            v0 ? (int)v0->vz : 0, otz, (unsigned long)xy, flag,
                            (int)composed.t[0], (int)composed.t[1],
                            (int)composed.t[2]);
                }
                drew = func_8002C700(model, (void*)(uintptr_t)work, prim, 0);
                if (OvlyDiag() && s_drawLogs < 40) {
                    u8* pkt = (u8*)(uintptr_t)work;
                    s32 payload = *(s32*)(model + 0x34);
                    s32 live = 0;
                    fprintf(stderr,
                            "[obj-ovly] C700 slot=%d node=%d ngrp=%u cmd0=%02x "
                            "nprim=%d ret=%d dEmits=%d verts=%p\n",
                            (int)slot, (int)n, (unsigned)ngrp,
                            cmds != NULL ? cmds[0] : 0xFF,
                            cmds != NULL ? (int)*(s16*)(cmds + 2) : -1,
                            (int)drew, (int)(D_80059578 - emitsBefore),
                            (void*)(uintptr_t)*(u32*)(model + 8));
                    while (pkt != NULL && live < 6 &&
                           pkt + 0x10 < (u8*)(uintptr_t)work + payload) {
                        u8 len = pkt[3];
                        s32 step;
                        s16 x0 = *(s16*)(pkt + 0x08);
                        s16 y0 = *(s16*)(pkt + 0x0A);
                        if (len < 4 || len > 12) {
                            break;
                        }
                        step = ((s32)len + 1) * 4;
                        if (x0 != 0 || y0 != 0) {
                            fprintf(stderr,
                                    "[obj-ovly] draw-pkt slot=%d live=%d len=%u "
                                    "code=%02x rgb=%02x%02x%02x clut=%04x "
                                    "tpage=%04x xy0=(%d,%d) uv0=%02x%02x\n",
                                    (int)slot, (int)live, (unsigned)len, pkt[7],
                                    pkt[4], pkt[5], pkt[6],
                                    (unsigned)*(u16*)(pkt + 0x0E),
                                    (unsigned)*(u16*)(pkt + 0x16),
                                    (int)x0, (int)y0, pkt[0x0C], pkt[0x0D]);
                            live++;
                            if (live >= 2) {
                                break;
                            }
                        }
                        pkt += step;
                    }
                    if (live == 0 && pkt != NULL) {
                        u8* first = (u8*)(uintptr_t)work;
                        fprintf(stderr,
                                "[obj-ovly] draw-pkt slot=%d len=%u code=%02x "
                                "rgb=%02x%02x%02x clut=%04x tpage=%04x "
                                "xy0=(%d,%d) uv0=%02x%02x\n",
                                (int)slot, (unsigned)first[3], first[7],
                                first[4], first[5], first[6],
                                (unsigned)*(u16*)(first + 0x0E),
                                (unsigned)*(u16*)(first + 0x16),
                                (int)*(s16*)(first + 0x08),
                                (int)*(s16*)(first + 0x0A),
                                first[0x0C], first[0x0D]);
                    }
                }
            }
            emitted++;
        }
        if (OvlyDiag() && s_drawLogs < 16) {
            fprintf(stderr, "[obj-ovly] 7D14 slot=%d nodes=%u emitted=%d ctx=%d anim=%d\n",
                   (int)slot, (unsigned)nnodes, (int)emitted,
                   (int)renderContextIndex, (int)*(s32*)(obj + 0x4C));
            s_drawLogs++;
        }
    }
}

/* Overlay object-anim entry (retail 0x801E8330). Field scripts stamp a
 * slot + anim index; retail then runs 801E35D0/39F0 to bind the clip. */
void func_801E8330(s32 slot, s32 unused, s32 anim)
{
    u32 idx = (u32)slot & 0xFFFFu;
    u8* obj;
    static s32 s_animLogs;

    D_801E86B0 = (s16)slot;
    D_801E863C = (s16)unused;
    if (idx >= OVLY_SLOT_MAX) {
        return;
    }
    obj = (u8*)(uintptr_t)D_801E8670[idx];
    if (obj == NULL) {
        return;
    }
    obj[0x35] = 0;
    *(s16*)(obj + 0x10A) = (s16)unused;
    OvlyClipBind(obj, obj, anim);
    if (OvlyDiag() && s_animLogs < 16) {
        fprintf(stderr, "[obj-ovly] 8330 slot=%u anim=%d obj=%p ip=%p\n",
                idx, (int)anim, (void*)obj,
                (void*)(uintptr_t)*(u32*)(obj + 0x10));
        s_animLogs++;
    }
}
