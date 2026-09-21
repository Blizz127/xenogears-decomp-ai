#include "common.h"


#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/main2", func_80071AE0);
INCLUDE_ASM("asm/battle/nonmatchings/main2", func_80071B94);
INCLUDE_ASM("asm/battle/nonmatchings/main2", func_80072270);
INCLUDE_ASM("asm/battle/nonmatchings/main2", func_80072324);
INCLUDE_ASM("asm/battle/nonmatchings/main2", func_800723E0);
INCLUDE_ASM("asm/battle/nonmatchings/main2", func_8007252C);
INCLUDE_ASM("asm/battle/nonmatchings/main2", func_800728B8);
INCLUDE_ASM("asm/battle/nonmatchings/main2", func_80072938);
INCLUDE_ASM("asm/battle/nonmatchings/main2", func_80072A9C);
INCLUDE_ASM("asm/battle/nonmatchings/main2", func_80072DA8);
INCLUDE_ASM("asm/battle/nonmatchings/main2", func_80072F38);
INCLUDE_ASM("asm/battle/nonmatchings/main2", func_80073380);
INCLUDE_ASM("asm/battle/nonmatchings/main2", func_80073538);
INCLUDE_ASM("asm/battle/nonmatchings/main2", func_80073A58);
INCLUDE_ASM("asm/battle/nonmatchings/main2", func_80073B64);
INCLUDE_ASM("asm/battle/nonmatchings/main2", func_80073E88);
INCLUDE_ASM("asm/battle/nonmatchings/main2", func_80073F08);
INCLUDE_ASM("asm/battle/nonmatchings/main2", func_80073FB8);
INCLUDE_ASM("asm/battle/nonmatchings/main2", func_800742A0);
INCLUDE_ASM("asm/battle/nonmatchings/main2", func_800743A4);
INCLUDE_ASM("asm/battle/nonmatchings/main2", func_800744BC);
INCLUDE_ASM("asm/battle/nonmatchings/main2", func_80074554);
INCLUDE_ASM("asm/battle/nonmatchings/main2", func_800745EC);
INCLUDE_ASM("asm/battle/nonmatchings/main2", func_80074AB8);
INCLUDE_ASM("asm/battle/nonmatchings/main2", func_80074D4C);
INCLUDE_ASM("asm/battle/nonmatchings/main2", func_80074EEC);
INCLUDE_ASM("asm/battle/nonmatchings/main2", func_80074F70);
INCLUDE_ASM("asm/battle/nonmatchings/main2", func_8007500C);
INCLUDE_ASM("asm/battle/nonmatchings/main2", func_80075168);
INCLUDE_ASM("asm/battle/nonmatchings/main2", func_80075938);
#endif
#ifndef XENO_PC_PORT
extern u8 D_800C492A;
#endif
extern void func_8008FAD8(void);
extern void func_800742A0(void);
extern void func_80075938(void);
extern void func_80073538(void);
extern void func_80073A58(void);
extern void func_80073B64(void);
extern void func_80073E88(void);
extern void func_80073F08(void);
extern void func_8007500C(void);
extern void func_80074EEC(void);
extern void func_800745EC(void);
extern void func_80074D4C(void);
extern void func_80073FB8(void);
extern void func_80088B80(void);
extern void func_80074AB8(void);
/* func_80076418.s: the per-frame battle render chain, skipped entirely once
 * D_800C492A is set. */
void func_80076418(void) {
    if (D_800C492A == 0) {
        func_8008FAD8();
        func_800742A0();
        func_80075938();
        func_80073538();
        func_80073A58();
        func_80073B64();
        func_80073E88();
        func_80073F08();
        func_8007500C();
        func_80074EEC();
        func_800745EC();
        func_80074D4C();
        func_80073FB8();
        func_80088B80();
        func_80074AB8();
    }
}


extern u8 D_8005959C;
extern void func_8008FAD8(void);
extern void func_801DE594(void);
extern void func_80073FB8(void);


/* func_800764B4.s */
void func_800764B4(void) {
    D_8005959C = 0;
    func_8008FAD8();
    func_801DE594();
    func_80073FB8();
}
/* func_800764EC.s */
void func_800764EC(void) {
    func_8008FAD8();
    func_80073538();
    func_80073F08();
    func_8007500C();
    func_80074F70();
    func_80073FB8();
    func_80088B80();
    func_80074AB8();
}
