#include "common.h"


#ifndef XENO_PC_PORT
/* 6-byte target records at D_800C3EB0 + 0xA3C. */
typedef struct {
    u16 x;
    u16 z;
    u8 flag;
    u8 pad;
} M126Spot;

/* D_800C3EB0 seen as a struct: member reads keep 0xA3C as the load offset
 * from (base + i * 6), where pointer arithmetic folds it into the symbol. */
typedef struct {
    u8 pad[0xA3C];
    M126Spot spots[1];
} M126Global;

typedef struct {
    u8 pad0[2];
    s16 x;
    u8 pad4[6];
    s16 z;
    u8 padC[0x68];
    u32 anim;
    u8 pad78[0x28];
    s16 tx;
    s16 ty;
    s16 tz;
} M126Obj;

typedef struct {
    s16 x;
    s16 z;
} M126Pos2;

/* D_800C3610 is read through an array view so its loads split into
 * lui + lw %lo, with the %hi kept in s0 across the call (retail). */
extern u8* D_800C3610__a[] asm("D_800C3610");
extern M126Global D_800C3EB0__s asm("D_800C3EB0");
extern void func_800BF4F0(u8* obj, u32 anim);
extern void func_800BF1EC(u8* obj, s32 anim);
extern s32 func_800C07CC(M126Pos2 a, M126Pos2 b);
extern u32 func_800BEF8C(u8* obj);
extern void func_80021FE0(u8* obj, s16 angle);
extern void func_800223B0(u8* obj, s16 angle);
extern void func_800245D8(u32 a0, u32 a1);
extern void func_800BF0B4(u32 value);

/* Take the target record that D_800C3610's +0x2C index selects.  An unset
 * record (0xFFFF, 0xFFFF) sets obj's target (+0xA0/+0xA2/+0xA4) to its own
 * x/z and calls func_800BF4F0 with its +0x74 word; otherwise the record
 * becomes the target, func_800BF1EC runs with 3 or 2 (the record's flag)
 * and the index advances.  The index is re-read after each store through
 * obj; tz is stored before ty = 0 (retail's reload order). */
void func_800BF0C4(M126Obj* obj) {
    s32 i = *(s32*)(D_800C3610__a[0] + 0x2C);
    u8* ctl;
    s32 x;
    s32 z;

    if (D_800C3EB0__s.spots[i].x == 0xFFFF && D_800C3EB0__s.spots[i].z == D_800C3EB0__s.spots[i].x) {
        x = obj->x;
        z = obj->z;
        obj->ty = 0;
        obj->tx = x;
        obj->tz = z;
        func_800BF4F0((u8*)obj, obj->anim);
        return;
    }
    ctl = D_800C3610__a[0];
    obj->tx = D_800C3EB0__s.spots[*(s32*)(ctl + 0x2C)].x;
    obj->tz = D_800C3EB0__s.spots[*(s32*)(ctl + 0x2C)].z;
    obj->ty = 0;
    func_800BF1EC((u8*)obj, D_800C3EB0__s.spots[*(s32*)(ctl + 0x2C)].flag ? 3 : 2);
    (*(s32*)(D_800C3610__a[0] + 0x2C))++;
}

/* Store func_800C07CC's distance between obj's +2/+0xA pair and its
 * +0xA0/+0xA4 target in D_800C3610 + 0x44, pass func_800BEF8C(obj) to
 * func_80021FE0 and to func_800223B0, then call func_800245D8(obj, anim)
 * and func_800BF0B4(6).  The first pair goes through int temps so it loads
 * with lh as in retail (found by a permuter run: s32 then u32). */
void func_800BF1EC(u8* obj, s32 anim) {
    M126Pos2 a;
    M126Pos2 b;
    s32 x;
    u32 z;

    x = *(s16*)(obj + 2);
    a.x = x;
    z = *(s16*)(obj + 0xA);
    a.z = z;
    b.x = *(u16*)(obj + 0xA0);
    b.z = *(u16*)(obj + 0xA4);
    *(s32*)(D_800C3610__a[0] + 0x44) = func_800C07CC(a, b);
    func_80021FE0(obj, func_800BEF8C(obj));
    func_800223B0(obj, func_800BEF8C(obj));
    func_800245D8((u32)obj, anim);
    func_800BF0B4(6);
}
#endif
extern void func_800B8D7C(u8* task);
extern void ArchiveSetIndex(u32 a0, u32 a1);
extern u32 ArchiveDecodeAlignedSize(u32 file);
extern void ArchiveReadFileToBuffer(u32 file, u8* buf, u32 a2, u32 a3);
#ifndef XENO_PC_PORT
extern u32 D_800C3618[];
#endif
#ifndef XENO_PC_PORT
extern u32 D_800C361C;
#endif
#ifdef XENO_PC_PORT
/* Port body; the matching build compiles the byte-exact C in the #else
 * arm below. */
/* func_800BF2B8.s: read the task's archive file (id at **(task + 0x7C)) into a
 * fresh heap buffer recorded in D_800C3618, then pack the task's +0xAC low two
 * bits above the +0xA8 top two bits into D_800C361C. */
void func_800BF2B8(u8* task) {
    u32 file;
    u8* buf;

    func_800B8D7C(task);
    ArchiveSetIndex(0x2C, 1);
    file = **(u32**)(task + 0x7C);
    buf = HeapAlloc(ArchiveDecodeAlignedSize(file), 1);
    ArchiveReadFileToBuffer(file, buf, 0, 0x80);
    D_800C3618[0] = (u32)buf;
    D_800C361C = ((*(u32*)(task + 0xAC) & 3) << 2) | (*(u32*)(task + 0xA8) >> 30);
}
#else
/* D_800C361C is stored through an array view so the store splits into
 * lui + sw %lo like retail; the +0xA8 bits are read before the D_800C3618
 * store, as retail loads them (a load cannot pass that store). */
extern u32 D_800C361C__a[] asm("D_800C361C");

void func_800BF2B8(u8* task) {
    u32 file;
    u8* buf;
    u32 hi;

    func_800B8D7C(task);
    ArchiveSetIndex(0x2C, 1);
    file = **(u32**)(task + 0x7C);
    buf = HeapAlloc(ArchiveDecodeAlignedSize(file), 1);
    ArchiveReadFileToBuffer(file, buf, 0, 0x80);
    hi = *(u32*)(task + 0xA8) >> 30;
    D_800C3618[0] = (u32)buf;
    D_800C361C__a[0] = ((*(u32*)(task + 0xAC) & 3) << 2) | hi;
}
#endif
extern u8* TimerWorkListAllocateTask(u32 owner, u32 size);
extern void TimerWorkListSetTaskCallback(void* pTask, void* callback);
extern u32 func_800B57E4(u8* s);
extern void func_800B5B3C(u8* task);
extern void func_800B5854(void);
extern void func_800BF73C(void);
extern void func_800B5CC0(void);
extern void func_800BDC14(void);
extern void func_800BDF1C(void);
extern void func_8001E148(u32 v);
extern void func_800C08CC(u32 a0, void* a1, void* a2);
extern void func_800B51B0(void);
extern void func_800245D8(u32 a0, u32 a1);
#ifndef XENO_PC_PORT
extern u8 D_800C3EB0[];
#endif
extern u32 WorkListsAddTasks(u32 a0, u32 a1, void* a2, void* a3, void* a4);
extern void func_800B7424(u32 p);
extern void func_800B7364(void);
extern void func_800B6F0C(void);
extern void func_800B7134(void);
extern void WorkListSetTaskCallback(void* pTask, void* callback);
extern void D_80025A88(void);
#ifndef XENO_PC_PORT
extern u8 D_800D3420[];
#endif
#ifndef XENO_PC_PORT
extern u32 D_800C3CE8[];
#endif
extern void WorkListTaskSetOnFreeCallback(void* pTask, void* callback);
extern void func_800B3358(u8* p);
extern void func_800B3588(u8* p);
#ifndef XENO_PC_PORT
extern u32 D_800C3548[];
#endif


/* func_800BF354.s */
#ifndef XENO_PC_PORT
extern u8 D_800D3350[];
#endif
#ifndef XENO_PC_PORT
extern u32 D_800C3618[];
#endif
extern u32 func_800C0FAC(u32 p);
u32 func_800BF354(void) {
    u32 r;

    if (D_800D3350[0] == 0) {
        r = func_800C0FAC(D_800C3618[0]);
        D_800D3350[0] = 1;
    }
    return r;
}
/* func_800BF3A4.s */
#ifndef XENO_PC_PORT
extern u8 D_800D3350[];
#endif
#ifndef XENO_PC_PORT
extern u32 D_800C3618[];
#endif
extern void func_800C1140(u32 p);
void func_800BF3A4(void) {
    if (D_800D3350[0] != 0) {
        func_800C1140(D_800C3618[0]);
        D_800D3350[0] = 0;
    }
}
