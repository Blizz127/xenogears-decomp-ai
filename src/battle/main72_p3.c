/* Retail TU part 3 of main72.c (see the BATTLE_TU_PART note there): the
 * still-assembly run below, then that file's part-3 C bodies. */
#define BATTLE_TU_PART 3
#include "common.h"
#ifndef XENO_PC_PORT
#include "psyq/libgte.h"
#include "battle/gte_inline.h"

/* The motion-event owner (battle model object): func_800AE2A4 walks its
 * command script at +0xA0 (start +0xA4) with the frame/step counters at
 * +0x98..+0x9E.  D_800D3368 holds the same objects per actor slot. */
typedef struct M72Obj {
    u8 pad0[4];
    u8* model;
    u8 pad8[0x14];
    s16 scale;
    u8 pad1E[2];
    u8 actor;
    u8 pad21[0x18];
    u8 volume;
    u8 pad3A[0x10];
    u16 flags4A;
    u8 pad4C[0x14];
    u16 groundY;
    u8 pad62[0x2E];
    s16 anchor;
    u8 pad92[2];
    u16 dx;
    u16 dy;
    s16 frame;
    s16 frames;
    s16 step;
    s16 steps;
    u8* cmd;
    u8* script;
    u8 padA8[8];
    u8* nodeB0;
    u8 padB4[0x56];
    u16 mask;
    u8 trailCount;
    u8 pad10D;
    u8 imageCount;
    u8 pad10F;
    s32 trails;
    u8 pad114[4];
    s32 images;
} M72Obj;

typedef struct {
    u16 v;
    u8 pad[0x1A];
} M72Cam;

typedef struct {
    s16 v;
    u8 pad[0x12];
} M72Slot;

extern s32 func_800AF400(void);
extern u32 func_800B12D0(u32 index, u32 mask);
extern s32 func_800AE220(M72Obj* obj, s32 sel);
extern void func_80039E60(s32 id);
extern void func_8003A2E4(s32 id, s32 volume);
extern void func_800A3484();
extern s32 func_800A32D8();
extern u8* func_800A3640();
extern void func_800A429C(u8* e);
extern void* func_800AA820(s32 kind);
extern void func_800AA454(u16 slot, u16 mask, s32 anim);
extern void func_800AA564(M72Obj* obj, u16 slot, u16 mask, s32 anim);
extern s32 func_800A579C(SVECTOR* pos);
extern s32 func_800A5870(SVECTOR* pos, s32 tri, VECTOR* normal);
extern void func_800AFB4C(u8* package, s16 index, SVECTOR* position, s16 direction, s16 scale, u8* info,
                          M72Obj* owner);
extern void func_800BF6CC(void);
extern u8* D_800658C8;
extern u8 D_8006BE10[];
extern u8 D_8005A474[];
extern u8 D_800C3D04[];
extern s16 D_800C3D40;
extern u16 D_800C3E30;
extern s32 D_800C360C;
typedef struct {
    u8 b[72];
} M72Row48;

extern M72Row48 D_800C4000[];
typedef struct {
    u8 b[0x170];
} M72Row170;

extern M72Row170 D_800CCDE9[];
extern M72Cam D_800C3EBE[];
extern M72Cam D_800C3EC0[];
extern M72Cam D_800C3EC2[];
extern u8* D_800D2FC0;
extern M72Slot D_800D330A[];
extern M72Slot D_800D330C[];
extern M72Slot D_800D330E[];
extern M72Slot D_800D3310[];
extern M72Slot D_800D3314[];
extern M72Slot D_800D3316[];
extern M72Obj* D_800D3368[];

/* Run obj's motion-event commands for the current frame: while step < steps
 * and the next command's frame (+0) is obj->frame, dispatch on its kind (+2)
 * and advance obj->cmd past it (kind 9 with an anchor-relative position and
 * no anchor leaves the command for the next step): 1 spawns a sprite effect
 * at a command offset moved through the owner's (or a bone's) matrix,
 * 2 sets one of the two trail colour slots, 3/4 reset and refill a 0x70-byte
 * trail record, 5 plays a sound pair, 6 calls func_800BF6CC, 7 sets a model
 * part's +7 flag, 8 starts an animation on every flagged actor slot (keeping
 * D_800C3D40 / D_800C3E30), 9 builds or frees a 0x30-byte image record.
 * Then the frame counter steps, wrapping to the script start after
 * obj->frames.  gcc 2.6.3 (the GTE result is read back with halfword loads
 * only under 2.6.x).  Construct: `one` stands for the literal 1 in three
 * compares and one shift; retail keeps that constant in a loop-hoisted,
 * spilled register (rematerialised into $t7 at each use), which literal 1s
 * do not reach (loop.c's move threshold).  Shapes retail needs: cmd copied to a
 * block-local `c` where retail keeps it in a second register after a join,
 * the case-2 colour stores' second and third index written as a shift
 * (base-first address add), dir as (model +0x56 + 0x400) + offset, and the
 * D_800CCDE9 / D_800C4000 rows as struct arrays (the row offset is not
 * distributed). */
void func_800AE2A4(M72Obj* obj, s32 id, s32 a2) {
    SVECTOR sv;
    VECTOR out;
    VECTOR nrm;
    MATRIX* m;
    u8* cmd;
    M72Obj* src;
    u8* pkg;
    u8* parent;
    u8* table;
    void* callback;
    s16 idx;
    s16 cam;
    s32 dir;
    s16 scale;
    s16 vol;
    s32 base;
    s32 add;
    s32 mode;
    s32 one = 1;
    s32 i;
    s32 v;
    u8 flag;
    s16 saveD40;
    u16 saveE30;
    s32 mask;
    u16 x0;
    u16 y0;
    u16 x1;
    u16 y1;
    u16 w;
    u16 dx;
    u16 dy;

    if (obj->frame >= 0) {
        for (; obj->step < obj->steps; (obj->step)++) {
            cmd = obj->cmd;
            if (obj->frame != *(s16*)cmd) {
                break;
            }
            switch (cmd[2]) {
            case 1:
                {
                    u8* c;

                    cam = 0;
                    m = (MATRIX*)0x1F800000;
                    c = cmd;
                    if (c[4] & 0x80) {
                        idx = (D_800CCDE9[obj->actor].b[c[3]] - 1) + c[3];
                    } else {
                        idx = c[3];
                    }
                    if ((c[0xC] & 0x80) || (c[4] & 0x80)) {
                        src = D_800D3368[func_800AF400()];
                        if (src == NULL) {
                            cam = func_800AF400() + 1;
                            src = obj;
                        }
                    } else {
                        src = obj;
                    }
                    if (idx >= 0) {
                        if (c[5] != 0) {
                            CompMatrix((MATRIX*)(src->model + 0xC), (MATRIX*)(src->model + c[5] * 0x7C + 0x2C),
                                       m);
                        } else {
                            *m = *(MATRIX*)(src->model + 0xC);
                        }
                        if (cam != 0) {
                            m->t[0] = D_800C3EBE[cam - 1].v;
                            m->t[1] = D_800C3EC2[cam - 1].v;
                            m->t[2] = D_800C3EC0[cam - 1].v;
                        }
                        SetRotMatrix(m);
                        SetTransMatrix(m);
                        sv.vx = *(u16*)(c + 6);
                        sv.vy = *(u16*)(c + 8);
                        sv.vz = *(u16*)(c + 0xA);
                        BATTLE_GTE_LDV0(&sv);
                        BATTLE_GTE_RTV0TR();
                        BATTLE_GTE_STLVNL(&out);
                        sv.vx = out.vx;
                        sv.vz = out.vz;
                        mode = c[0xC] & 0x7F;
                        if (mode == one) {
                            sv.vy = obj->groundY;
                        } else if (mode == 2) {
                            sv.vx = *(u16*)(D_800658C8 + 0x4DC);
                            sv.vy = *(u16*)(D_800658C8 + 0x4DE);
                            sv.vz = *(u16*)(D_800658C8 + 0x4E0);
                            func_800A5870(&sv, func_800A579C(&sv), &nrm);
                        } else {
                            sv.vy = out.vy;
                        }
                        if (c[0xD] != 0) {
                            dir = *(s16*)(c + 0xE);
                        } else {
                            dir = *(s16*)(obj->model + 0x56) + 0x400 + *(s16*)(c + 0xE);
                        }
                        pkg = D_8006BE10;
                        scale = (*(s16*)(c + 0x10) * obj->scale) >> 8;
                        if (c[0x12] != 0) {
                            pkg = D_8005A474;
                        }
                        if (func_800B12D0(func_800AF400(), c[4]) != 0) {
                            func_800AFB4C(pkg, idx, &sv, dir, scale, c, obj);
                        }
                    }
                    obj->cmd += 0x14;
                    break;
                }
            case 2:
                {
                    u8* c;

                    if (cmd[4] != 0) {
                        if (cmd[3] < 2) {
                            c = cmd;
                            if (c[5] != 0) {
                                D_800D3314[c[3]].v = -1;
                            } else {
                                D_800D3314[c[3]].v = obj->actor;
                            }
                            D_800D3316[c[3]].v = c[6];
                            *(s16*)(D_800D2FC0 + c[3] * 2 + 2) = c[7] << 4;
                            *(s16*)(D_800D2FC0 + (c[3] << 1) + 8) = c[8] << 4;
                            *(s16*)(D_800D2FC0 + (c[3] << 1) + 0xE) = c[9] << 4;
                            D_800D330C[c[3]].v = *(u16*)(c + 0xA);
                            D_800D330E[c[3]].v = *(u16*)(c + 0xC);
                            D_800D3310[c[3]].v = *(u16*)(c + 0xE);
                            D_800D330A[c[3]].v = *(u16*)(c + 0x10);
                        }
                        obj->cmd += 0x12;
                    } else {
                        D_800D330A[cmd[3]].v = 0;
                        obj->cmd += 6;
                    }
                    break;
                }
            case 3:
            case 4:
                func_800A3484(cmd[3] * 0x70 + obj->trails, a2);
                if (cmd[4] != 0) {
                    if (cmd[3] < obj->trailCount) {
                        func_800A32D8(cmd[3] * 0x70 + obj->trails, D_800C3D04, cmd[5], cmd[2] - 3, cmd[6],
                                      cmd[7], cmd[8], cmd[9], cmd[0xA], cmd[0xB], cmd[0xC], cmd[0xD],
                                      *(s16*)(cmd + 0xE), *(s16*)(cmd + 0x10), *(s16*)(cmd + 0x12),
                                      *(s16*)(cmd + 0x14), *(s16*)(cmd + 0x16), *(s16*)(cmd + 0x18), cmd[0x1A]);
                    }
                    obj->cmd += 0x1C;
                } else {
                    obj->cmd += 6;
                }
                break;
            case 5:
                {
                    u8* c = cmd;

                    if (func_800B12D0(func_800AF400(), c[4]) != 0) {
                        add = 0;
                        if (c[7] == one && D_800658C8[0x35F] != 0) {
                            func_80039E60((*(u16*)(*(u8**)(obj->nodeB0 + 8) + 0x14) << 16) |
                                          (D_800658C8[0x35F] + 0xA));
                        }
                        base = func_800AE220(obj, c[5]);
                        if (D_800658C8[0x35F] == 3 && c[7] == 2) {
                            add = 8;
                        }
                        func_80039E60(base + c[3] + add);
                        if (c[6] != 0) {
                            func_80039E60(base + c[6] + add);
                        }
                        vol = obj->volume;
                        if (D_800658C8[0x35F] == 3 && c[7] != 2) {
                            vol = vol * 60 / 107;
                        }
                        func_8003A2E4(base + c[3] + add, vol);
                        if (c[6] != 0) {
                            func_8003A2E4(base + c[6] + add, vol);
                        }
                    }
                    obj->cmd += 8;
                    break;
                }
            case 6:
                func_800BF6CC();
                obj->cmd += 4;
                break;
            case 7:
                (obj->model + cmd[4] * 0x7C)[7] = cmd[5] & 1;
                obj->cmd += 6;
                break;
            case 8:
                {
                    u8* c = cmd;

                    saveD40 = D_800C3D40;
                    saveE30 = D_800C3E30;
                    mask = one << D_800C3D40;
                    for (i = 0; i < 13; i++) {
                        if ((obj->mask >> i) & 1) {
                            flag = c[4];
                            v = -1;
                            if (D_800D3368[i] != NULL && (D_800D3368[i]->flags4A & 2)) {
                                flag = 0;
                            }
                            switch (D_800C4000[D_800C360C - 1].b[i]) {
                            case 0:
                            case 1:
                                if (D_800D3368[i] != NULL && (D_800D3368[i]->flags4A & 2)) {
                                    v = c[6];
                                } else {
                                    v = c[5];
                                }
                                break;
                            case 5:
                                v = c[7];
                                break;
                            case 4:
                                v = c[8];
                                break;
                            case 2:
                            case 3:
                                v = c[9];
                                break;
                            }
                            if (func_800B12D0(i, c[3]) == 0) {
                                v = -1;
                            }
                            if (D_800D3368[i] != NULL && v > 0) {
                                if (flag != 0) {
                                    func_800AA564(obj, i, mask, v);
                                } else {
                                    func_800AA454(i, mask, v);
                                }
                            }
                        }
                    }
                    D_800C3D40 = saveD40;
                    D_800C3E30 = saveE30;
                    obj->cmd += 0xA;
                    break;
                }
            case 9:
                if (cmd[4] != 0) {
                    if (cmd[3] < obj->imageCount) {
                        if (cmd[5] != 0xFF && cmd[5] < obj->imageCount) {
                            parent = (u8*)(cmd[5] * 0x30 + obj->images);
                        } else {
                            parent = NULL;
                        }
                        table = NULL;
                        if ((cmd[6] & 0x7F) >= 4) {
                            table = D_800D2FC0;
                        }
                        callback = func_800AA820(cmd[7]);
                        x0 = *(u16*)(cmd + 8);
                        y0 = *(u16*)(cmd + 0xA);
                        x1 = *(u16*)(cmd + 0xC);
                        y1 = *(u16*)(cmd + 0xE);
                        w = *(u16*)(cmd + 0x10);
                        if (cmd[6] & 0x80) {
                            if (obj->anchor < 0) {
                                break;
                            }
                            dx = obj->dx;
                            dy = obj->dy;
                            x0 += dx;
                            y0 += dy;
                            if ((cmd[0x12] >> 4) == one) {
                                x1 += dx;
                                y1 += dy;
                            }
                        }
                        func_800A3640(cmd[3] * 0x30 + obj->images, parent, cmd[6] & 0x7F, cmd[0x12] | 0x700,
                                      table, (s16)x0, (s16)y0, 0, (s16)x1, (s16)y1, (s16)w, (s16)x0, (s16)y0,
                                      cmd[0x13], cmd[0x14], *(s16*)(cmd + 0x16), *(s16*)(cmd + 0x18),
                                      *(s16*)(cmd + 0x1A), callback);
                    }
                    obj->cmd += 0x1C;
                } else {
                    func_800A429C((u8*)(cmd[3] * 0x30 + obj->images));
                    obj->cmd += 6;
                }
                break;
            }
        }
        (obj->frame)++;
        if (obj->frames >= 0 && obj->frame >= obj->frames) {
            obj->frame = 0;
            obj->step = 0;
            obj->cmd = obj->script;
        }
    }
}
#endif
#include "main72.c"
