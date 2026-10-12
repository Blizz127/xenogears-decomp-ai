/* Retail TU part 2 of mainc114.c (see the BATTLE_TU_PART note there): the
 * still-assembly run below, then that file's part-2 C bodies. */
#define BATTLE_TU_PART 2
#include "common.h"
#ifndef XENO_PC_PORT
extern u8 D_800D30BC[];
extern u8 D_800D3354[];
extern u8 D_800D335C[];
extern u8 D_800C3730[];
extern s16 D_800C354C[];
extern u8 D_800C372C[];
extern u8 D_800C3EB0[];
extern void func_800BB844(u8* m, u8* eye, u8* at, u8* p);
extern void func_800A4654(u8* view, u8* light, u32 a2, u8* ot, s32 bufIndex, u8* eye, u8* at, s32 otCount);

/* Per-frame camera + effects: rebuild the view matrix D_800D30BC from the
 * eye/target pair (func_800BB844), add the D_800C354C shake to its
 * translation (the record that holds the matrix starts 0x20 earlier), then,
 * unless D_800C372C is set, render the battle effects (func_800A4654) into
 * the current draw buffer's OT. */
void func_800BB9D4(void) {
    u8* cam;
    u8* base;

    func_800BB844(D_800D30BC, D_800D3354, D_800D335C, D_800C3730);
    cam = D_800D30BC - 0x20;
    *(s32*)(cam + 0x34) += D_800C354C[0];
    *(s32*)(cam + 0x38) += D_800C354C[1];
    *(s32*)(cam + 0x3C) += D_800C354C[2];
    if (D_800C372C[0] == 0) {
        base = (u8*)(u32)D_800C3EB0;
        func_800A4654(D_800D30BC, 0, 0, *(u8**)(base + 0x8000 + 0xC54), *(s32*)(base + 0x8000 + 0xC84),
                      D_800D3354, D_800D335C, 0x1000);
    }
}
#include "psyq/libgte.h"
#include "battle/gte_inline.h"
extern s32 D_800C3CC0__s[] asm("D_800C3CC0");
extern s32 D_800C3CBC[];
extern u32 D_800C3678[];
extern s32 D_800C3674[];
extern s32 D_8006F99C__w[] asm("D_8006F99C");
extern s32 D_8006F9AC__w[] asm("D_8006F9AC");
extern s16 D_800D30A0__s16[] asm("D_800D30A0");
extern u8* D_800D39EC[];
extern s16 D_800C373C[];
extern s32 D_800C3738[];
typedef struct {
    u8 unk0[4];
    SVECTOR eye;
    SVECTOR at;
    s16 rx;
    s16 ry;
    s16 rz;
    s16 unk1A;
    s32 dist;
} BattleCamera;
extern BattleCamera D_800D309C__cam asm("D_800D309C");
extern s16 D_800D3354__s16[] asm("D_800D3354");
extern s16 D_800D335C__s16[] asm("D_800D335C");
extern void func_800BC460(u32 mask);

typedef struct { s32 x, y, z, pad; } BattleSetupLongVec;
typedef struct { s16 x, y, z, pad; } BattleSetupShortVec;

static inline void BattleSetupPackVector(BattleSetupLongVec* src, BattleSetupShortVec* dst) {
    dst->x = src->x >> 16;
    dst->y = src->y >> 16;
    dst->z = src->z >> 16;
}

/* Per-tick battle camera driver.  Setup mode (D_800C3CC0) 1 runs
 * func_800BC460; 2 packs the two 16.16 setup vectors D_8006F99C/D_8006F9AC
 * into the D_800D30A0 eye/target pair; 3 orbits the eye around the
 * D_800D39EC actor by the D_800C373C angle at the D_800C3738 radius.  While
 * D_800C3CBC == 1 the current eye/target (D_800D3354/D_800D335C) ease toward
 * the camera record's by the D_800C3674 factor (inline GTE GPF), snapping
 * when the step rounds to zero.  Finally the record's distance and pitch/yaw
 * are rebuilt from the eye->target delta.  The final block owns 48 bytes of
 * unused stack, as retail. */
void func_800BBAB8(void) {

    switch (D_800C3CC0__s[0]) {
    case 0:
        break;
    case 1:
        func_800BC460(D_800C3678[0]);
        break;
    case 2:
        BattleSetupPackVector((BattleSetupLongVec*)D_8006F99C__w, (BattleSetupShortVec*)D_800D30A0__s16);
        BattleSetupPackVector((BattleSetupLongVec*)D_8006F9AC__w, (BattleSetupShortVec*)D_800D30A0__s16 + 1);
        break;
    case 3: {
        SVECTOR a;
        SVECTOR b;
        u8* p = D_800D39EC[0];

        b.vx = a.vx = *(s32*)(p + 0) >> 16;
        a.vy = *(s32*)(p + 4) >> 16;
        a.vz = *(s32*)(p + 8) >> 16;
        b.vz = a.vz - rsin(D_800C373C[0]) * D_800C3738[0] / 4096;
        b.vy = a.vy - rcos(D_800C373C[0]) * D_800C3738[0] / 4096;
        D_800D309C__cam.eye = b;
        D_800D309C__cam.at = a;
        break;
    }
    }
    if (D_800C3CBC[0] == 1) {
        VECTOR v;

        BATTLE_GTE_LDDP(D_800C3674[0]);
        v.vx = D_800D309C__cam.eye.vx - D_800D3354__s16[0];
        v.vy = D_800D309C__cam.eye.vy - D_800D3354__s16[1];
        v.vz = D_800D309C__cam.eye.vz - D_800D3354__s16[2];
        BATTLE_GTE_LDLVL(&v);
        BATTLE_GTE_GPF12();
        BATTLE_GTE_STLVL(&v);
        if (v.vx | v.vz | v.vy) {
            D_800D3354__s16[0] += v.vx;
            D_800D3354__s16[1] += v.vy;
            D_800D3354__s16[2] += v.vz;
        } else {
            s16* t = (s16*)&D_800D309C__cam.eye;

            D_800D3354__s16[0] = t[0];
            D_800D3354__s16[1] = t[1];
            D_800D3354__s16[2] = t[2];
        }
        v.vx = D_800D309C__cam.at.vx - D_800D335C__s16[0];
        v.vy = D_800D309C__cam.at.vy - D_800D335C__s16[1];
        v.vz = D_800D309C__cam.at.vz - D_800D335C__s16[2];
        BATTLE_GTE_LDLVL(&v);
        BATTLE_GTE_GPF12();
        BATTLE_GTE_STLVL(&v);
        if (v.vx | v.vz | v.vy) {
            D_800D335C__s16[0] += v.vx;
            D_800D335C__s16[1] += v.vy;
            D_800D335C__s16[2] += v.vz;
        } else {
            s16* t = (s16*)&D_800D309C__cam.at;

            D_800D335C__s16[0] = t[0];
            D_800D335C__s16[1] = t[1];
            D_800D335C__s16[2] = t[2];
        }
    }
    {
    u8 pad0[0x20];
    VECTOR d;
    u8 pad1[0x10];
    VECTOR sq;
    s32 flat;

    d.vx = D_800D335C__s16[0] - D_800D3354__s16[0];
    d.vy = D_800D335C__s16[1] - D_800D3354__s16[1];
    d.vz = D_800D335C__s16[2] - D_800D3354__s16[2];
    Square0(&d, &sq);
    flat = SquareRoot0(sq.vx + sq.vz);
    D_800D309C__cam.dist = SquareRoot0(sq.vx + sq.vy + sq.vz);
    D_800D309C__cam.ry = -ratan2(d.vz, d.vx);
    D_800D309C__cam.rx = -ratan2(d.vy, flat);
    D_800D309C__cam.rz = 0;
    }
}
extern u8* D_800C3680__p[] asm("D_800C3680");
extern u8* D_800C3684__p[] asm("D_800C3684");
extern u8 D_800C37C8[];
extern s16 D_800C3CCC[];
extern s16 D_800C3CD4[];
extern s16 D_800D30A0__s16[] asm("D_800D30A0");
extern s16 D_800D30A8[];
extern u8 D_800C3CC4[];
extern u32 D_800C367C[];
extern void func_8001CE74(u8* task);
extern void TimerWorkListRemoveTask(void* task);
extern void WorkListRemoveTask(void* task);
extern void func_800BC2F0(u32 a0);

/* Free callback of the setup sprite tasks: a kind-10 sprite (+0x40 bits
 * 13..16) releases the D_800C3680 owner slot, any other the D_800C3684 one,
 * restoring the saved view vector (D_800C3CCC / D_800C3CD4) into
 * D_800D30A0[0] / [1] unless D_800C37C8 is set; then release the children
 * (when +0xAC bit 5 is set), unlink and free the task, and once the
 * D_800C3CC4 count drops to zero hand D_800C367C to func_800BC2F0.  The
 * later mainc114.c bodies type these globals with their own structs, hence
 * the asm-named aliases. */
void func_800BBEE0(u8* task) {
    u8* sprite = *(u8**)(task + 4);

    if (((*(u32*)(sprite + 0x40) >> 13) & 0xF) == 10) {
        if (D_800C3680__p[0] == task) {
            D_800C3680__p[0] = 0;
            if (D_800C37C8[0] == 0) {
                D_800D30A0__s16[0] = D_800C3CCC[0];
                D_800D30A0__s16[1] = D_800C3CCC[1];
                D_800D30A0__s16[2] = D_800C3CCC[2];
            }
        }
    } else {
        if (D_800C3684__p[0] == task) {
            D_800C3684__p[0] = 0;
            if (D_800C37C8[0] == 0) {
                D_800D30A8[0] = D_800C3CD4[0];
                D_800D30A8[1] = D_800C3CD4[1];
                D_800D30A8[2] = D_800C3CD4[2];
            }
        }
    }
    if ((*(u32*)(sprite + 0xAC) >> 5) & 1) {
        func_8001CE74(task);
    }
    TimerWorkListRemoveTask(task);
    WorkListRemoveTask(task + 0x1C);
    HeapFree(task);
    if (--D_800C3CC4[0] == 0) {
        func_800BC2F0(D_800C367C[0]);
    }
}
extern s32 D_8006F99C__w[] asm("D_8006F99C");
extern s32 D_8006F9AC__w[] asm("D_8006F9AC");
extern void AnimScriptTick(void* obj);
extern void func_80022CDC(void* obj);

/* Timer callback of the setup sprite tasks: tick the sprite's (task +4)
 * animation and save its position into D_8006F99C (kind 10) or D_8006F9AC;
 * while +0x64 is still live, repeat once more when +0xAC bit 6 is set; when
 * +0x64 has run out, call the task's +0xC callback. */
void func_800BC018(u8* task) {
    u8* s = *(u8**)(task + 4);

    AnimScriptTick(s);
    func_80022CDC(s);
    if (((*(u32*)(s + 0x40) >> 13) & 0xF) == 10) {
        D_8006F99C__w[0] = *(s32*)(s + 0);
        D_8006F99C__w[1] = *(s32*)(s + 4);
        D_8006F99C__w[2] = *(s32*)(s + 8);
    } else {
        D_8006F9AC__w[0] = *(s32*)(s + 0);
        D_8006F9AC__w[1] = *(s32*)(s + 4);
        D_8006F9AC__w[2] = *(s32*)(s + 8);
    }
    if (*(s32*)(s + 0x64) != 0) {
        if (!((*(u32*)(s + 0xAC) >> 6) & 1)) {
            return;
        }
        AnimScriptTick(s);
        func_80022CDC(s);
        if (((*(u32*)(s + 0x40) >> 13) & 0xF) == 10) {
            D_8006F99C__w[0] = *(s32*)(s + 0);
            D_8006F99C__w[1] = *(s32*)(s + 4);
            D_8006F99C__w[2] = *(s32*)(s + 8);
        } else {
            D_8006F9AC__w[0] = *(s32*)(s + 0);
            D_8006F9AC__w[1] = *(s32*)(s + 4);
            D_8006F9AC__w[2] = *(s32*)(s + 8);
        }
        if (*(s32*)(s + 0x64) != 0) {
            return;
        }
    }
    (*(void (**)(u8*))(task + 0xC))(task);
}
extern void WorkListTaskSetOnFreeCallback(void* task, void* cb);
extern void TimerWorkListSetTaskCallback(void* task, void* cb);

/* Init callback of the setup sprite tasks (sprite at task +0x38): claim the
 * D_800C3680 (kind 10) or D_800C3684 owner slot.  An empty slot saves the
 * current view vector (D_800D30A0[0] / [1]) for func_800BBEE0 to restore; a
 * held slot is taken over (its owner's +0xC callback runs) only when that
 * owner's +0x6C is not 1 and this sprite's +0x34 is 1, otherwise this sprite
 * is retired (+0x9E, +0x64 cleared).  Then count it in D_800C3CC4, set the
 * +0x32 facing from +0xAC bit 2, install the free/timer callbacks and enter
 * setup mode 2. */
void func_800BC158(u8* task) {
    u8* s = task + 0x38;
    u8* old;
    s32 flip;

    if (((*(u32*)(s + 0x40) >> 13) & 0xF) == 10) {
        old = D_800C3680__p[0];
        if (old != NULL) {
            if (*(u16*)(old + 0x6C) != 1 && *(u16*)(s + 0x34) == 1) {
                (*(void (**)(u8*))(old + 0xC))(old);
                D_800C3680__p[0] = task;
            } else {
                *(s16*)(s + 0x9E) = 0;
                *(s32*)(s + 0x64) = 0;
            }
        } else {
            D_800C3680__p[0] = task;
            D_800C3CCC[0] = D_800D30A0__s16[0];
            D_800C3CCC[1] = D_800D30A0__s16[1];
            D_800C3CCC[2] = D_800D30A0__s16[2];
        }
    } else {
        old = D_800C3684__p[0];
        if (old != NULL) {
            if (*(u16*)(old + 0x6C) != 1 && *(u16*)(s + 0x34) == 1) {
                (*(void (**)(u8*))(old + 0xC))(old);
                D_800C3684__p[0] = task;
            } else {
                *(s16*)(s + 0x9E) = 0;
                *(s32*)(s + 0x64) = 0;
            }
        } else {
            D_800C3684__p[0] = task;
            D_800C3CD4[0] = D_800D30A8[0];
            D_800C3CD4[1] = D_800D30A8[1];
            D_800C3CD4[2] = D_800D30A8[2];
        }
    }
    flip = (*(u32*)(s + 0xAC) >> 2) & 1;
    D_800C3CC4[0]++;
    if (flip) {
        *(s16*)(s + 0x32) = 0x800;
    } else {
        *(s16*)(s + 0x32) = 0;
    }
    WorkListTaskSetOnFreeCallback(task, func_800BBEE0);
    TimerWorkListSetTaskCallback(task, func_800BC018);
    func_800BC2F0(2);
}
#endif
#include "mainc114.c"
