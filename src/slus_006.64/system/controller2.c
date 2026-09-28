#include "common.h"
#include "psyq/libgpu.h"
#include "psyq/libapi.h"
#include "system/controller.h"

/* Retail TU 0x80035E44..0x80036CD8 (controller state/callback helpers that
 * follow system/controller.c). Retail-only: the port has its own controller
 * layer and never linked these asm bodies. */
#ifndef XENO_PC_PORT

/* Two 8-byte actuator records at 0x8005A1BC (also reached through
 * D_8005A1C0/C2/C3 by the setters below). */
typedef struct {
    u8 b0;
    u8 b1;
    u8 b2;
    u8 b3;
    s16 count;
    u8 state;
    u8 disabled;
} ControllerActState;

extern ControllerActState D_8005A1BC[2];
extern void func_80040C3C(void* pSrc, s32 srcLen, void* pDst, s32 dstLen);

/* Play-time clock: frames, seconds, minutes, hours; stops at 100 hours. */
extern u8 D_800501F8; /* clock stopped */
extern u8 D_80059370; /* frames */
extern u8 D_80059418; /* seconds */
extern u8 D_80059420; /* minutes */
extern u8 D_80059484; /* hours */

void func_80035E44(void) {
    if (D_800501F8 == 0) {
        if (++D_80059370 == 60) {
            D_80059370 = 0;
            D_80059418++;
        }
        if (D_80059418 == 60) {
            D_80059418 = 0;
            D_80059420++;
        }
        if (D_80059420 == 60) {
            D_80059420 = 0;
            D_80059484++;
        }
        if (D_80059484 == 100) {
            D_800501F8 = 1;
        }
    }
}

/* Debug: dumps a VRAM rectangle to a 16bpp TIM file on the dev PC. */
extern int PCcreate(char* name, int mode);
extern int PCwrite(int fd, char* buf, int len);
extern int PCclose(int fd);

typedef struct {
    s32 id;
    s32 flag;
    s32 bnum;
    s16 x;
    s16 y;
    s16 w;
    s16 h;
} ControllerTimHeader;

void func_80035F1C(RECT* rect, char* name) {
    ControllerTimHeader tim;
    int fd;

    StoreImage(rect, (u_long*)0x80700000);
    tim.id = 0x10;
    tim.flag = 2;
    tim.bnum = rect->w * rect->h * 2 + 12;
    tim.x = rect->x;
    tim.y = rect->y;
    tim.w = rect->w;
    tim.h = rect->h;
    fd = PCcreate(name, 0);
    PCwrite(fd, (char*)&tim, 0x14);
    PCwrite(fd, (char*)0x80700000, rect->w * rect->h * 2);
    PCclose(fd);
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/controller2", func_80035FF8);

void func_8003611C(void) {
    ControllerActState* p = D_8005A1BC;

    p[0].b0 = 0;
    D_8005A1BC[0].count = 0;
    D_8005A1BC[0].state = 0;
    D_8005A1BC[0].disabled = 0;
    p[1] = p[0];
    func_80040C3C(p, 4, &p[1], 4);
}

void func_80036188(ControllerActState* p) {
    if (p->disabled == 0) {
        if (p->count != 0) {
            p->b0 = 1;
            p->b1 = 0x40;
            p->b2 = 1;
            p->b3 = 0;
            p->state = 1;
            p->count--;
        } else if (p->state == 1) {
            p->b0 = 1;
            p->b1 = 0x40;
            p->b2 = 0;
            p->b3 = 0;
            p->state = 2;
        } else if (p->state == 2) {
            p->b0 = 0;
            p->state = 0;
        }
    }
}

void func_80036220(void) {
    func_80036188(&D_8005A1BC[0]);
    func_80036188(&D_8005A1BC[1]);
}

/* 8-byte per-controller records at 0x8005A1C0 */
extern s16 D_8005A1C0[];

void func_80036258(s32 index, s32 value) {
    *(s16*)((u8*)D_8005A1C0 + index * 8) = value;
}

extern u8 D_8005A1C3[];

void func_80036270(s32 index, s32 value) {
    D_8005A1C3[index * 8] = value;
}

extern s32 D_80050204;
extern u8 D_8005938C;
extern s32 D_80059390;
extern void ControllerResetState(void);

void ControllerInit(void) {
    s32 i;
    u_char* p;

    InitPAD(g_C1Buffer, 0x22, g_C1Buffer + 0x22, 0x22);
    StartPAD();
    ChangeClearPAD(0);
    ControllerResetState();
    func_8003611C();
    D_80050204 = 0;
    D_8005938C = 1;
    D_80059390 = 0;
    for (i = 7, p = &g_ControllerButtonMappings[7]; i >= 0; i--) {
        *p-- = i;
    }
    /* USA layout: swap Circle/Cross and Triangle/Square */
    g_ControllerButtonMappings[0] = 1;
    g_ControllerButtonMappings[2] = 3;
    g_ControllerButtonMappings[1] = 0;
    g_ControllerButtonMappings[3] = 2;
}

void func_8003633C(s32 value) {
    D_8005938C = value;
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/controller2", func_8003634C);

void func_800363E0(s32 value) {
    D_80059390 = value;
}

extern void (*D_800501FC)(void);

void func_800363F0(void (*pCallback)(void)) {
    D_800501FC = pCallback;
}

void func_80036400(void (*pCallback)(void)) {
    D_80050200 = (int)pCallback; /* declared int in system/controller.h */
}

s32 func_80036410(void) {
    return g_ControllerIsStateStackFull;
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/controller2", func_80036420);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/controller2", func_80036528);

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/controller2", ControllerDebugPrint);

extern void (*D_80050594)(void);

void func_800366E0(void (*pCallback)(void)) {
    D_80050594 = pCallback;
}

void func_800366F0(void) {
    D_80050594();
}

INCLUDE_ASM("asm/slus_006.64/nonmatchings/system/controller2", func_80036718);

#endif /* !XENO_PC_PORT */
