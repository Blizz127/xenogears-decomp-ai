#include "common.h"


#ifndef XENO_PC_PORT
extern u8* D_800C3610[];
extern s32 D_800C360C[];
extern u16 D_800C3608[];
extern s16 D_800C3614[];
extern u16 D_800C3DF0[];
extern u8 D_800C3623[];
extern void func_80021BF8(u8* p, void* cb);
extern void func_800245D8(u32 a0, u32 a1);
extern void func_80079E18(u8 index);
extern void func_80079E4C(u8 index);
extern void func_800B8048(u32 v);
extern void func_800BC404(u32 v);
extern void func_800BF600(u32 id, u8* actor);
extern void func_800BF730(u8* actor);
/* The current command step is the 0x48-byte row D_800C360C of D_800C3EB0;
 * STEP_U16 reads its u16 at a D_800C3EB0-relative offset.  Retail adds the
 * offset in the load (lhu 0x172(row)), which needs the row as an array
 * element: flat `D_800C3EB0 + D_800C360C * 0x48 + 0x172` arithmetic folds
 * the 0x172 into the symbol's %lo. */
typedef union BattleStepRow {
    u8 b[0x48];
    u16 h[0x24];
} BattleStepRow;
extern BattleStepRow D_800C3EB0__steps[] asm("D_800C3EB0");
#define STEP_U16(off) (D_800C3EB0__steps[D_800C360C[0]].h[(off) / 2])

/* func_800B9284.s, byte-exact: one command-step action for step kinds
 * 0xF3..0xFA (from func_800B9C78 and func_800B9508).  Most kinds use the
 * step's u16 parameter at +0x172: 0xFA/0xF8/0xF7 set the command task's
 * +0x48 flag, call func_80021BF8(actor, 0) and pass the parameter to
 * func_80079E18 / func_80079E4C / D_800C3614; 0xF5 and 0xF4 call
 * func_800B8048(actor) and split it into a 6-bit field for D_800C3DF0 and a
 * 9-bit id for func_800BF600 (0xF5 then also calls func_800BF730 and
 * func_800245D8(actor, 0x13) and sets D_800C3623); 0xF6 passes the step's
 * +0x14E u16 to func_800BC404; 0xF3 ORs the parameter into D_800C3608; 0xF9
 * does nothing.
 * The switch table is this object's .rodata (jtbl_80070A28, placed by
 * config/battle.yaml).  Codegen: the cases are in retail's block order, the
 * globals are arrays (the CDK compiler's split %hi/%lo; as scalars the
 * stores become $at macros) and the parameter is an s32 (retail's sra; a
 * u16 shift is shortened to srl). */
void func_800B9284(u8* actor, u32 kind) {
    s32 param;

    switch (kind) {
    case 0xFA:
        D_800C3610[0][0x48] = 1;
        func_80021BF8(actor, 0);
        func_80079E18(STEP_U16(0x172));
        break;
    case 0xF8:
        D_800C3610[0][0x48] = 1;
        func_80021BF8(actor, 0);
        func_80079E4C(STEP_U16(0x172));
        break;
    case 0xF7:
        D_800C3610[0][0x48] = 1;
        func_80021BF8(actor, 0);
        D_800C3614[0] = STEP_U16(0x172);
        break;
    case 0xF5:
        func_800B8048((u32)actor);
        param = STEP_U16(0x172);
        D_800C3DF0[0] = (param >> 9) & 0x3F;
        actor[0xAF] = 0x11;
        func_800BF600(param & 0x1FF, actor);
        func_800BF730(actor);
        func_800245D8((u32)actor, 0x13);
        D_800C3623[0] = 1;
        break;
    case 0xF4:
        func_800B8048((u32)actor);
        param = STEP_U16(0x172);
        D_800C3DF0[0] = (param >> 9) & 0x3F;
        actor[0xAF] = 0x11;
        func_800BF600(param & 0x1FF, actor);
        break;
    case 0xF6:
        func_800BC404(STEP_U16(0x14E));
        break;
    case 0xF3:
        D_800C3610[0][0x48] = 1;
        D_800C3608[0] |= STEP_U16(0x172);
        break;
    }
}
INCLUDE_ASM("asm/battle/nonmatchings/mainc108", func_800B9508);
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


/* func_800B9B30.s */
#ifndef XENO_PC_PORT
extern u8* D_800C3610[];
#endif
void func_800B9B30(void) {
#ifdef XENO_PC_PORT
    /* main125.c/main68.c read D_800C3610 as a u8*, so the guest-RAM alias is
     * that pointer and the retail array's [0] element is the alias itself.
     * Indexing it a second time would subscript a u8. */
    D_800C3610[0x48] = 1;
    D_800C3610[0x49] = D_800C3610[0x1C];
#else
    D_800C3610[0][0x48] = 1;
    D_800C3610[0][0x49] = D_800C3610[0][0x1C];
#endif
}
