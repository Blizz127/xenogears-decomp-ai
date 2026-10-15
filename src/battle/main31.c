#include "common.h"

#ifdef XENO_PC_PORT
#include "battle_guest_call.h"
#define MAIN31_CALL_8FA60() BATTLE_GUEST_CALL(0x8008FA60u, 0u)
#define MAIN31_CALL_7765C() BATTLE_GUEST_CALL0(0x8007765Cu)
#else
#define MAIN31_CALL_8FA60() func_8008FA60(0)
#define MAIN31_CALL_7765C() func_8007765C()
#endif

#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/main31", func_8007EF6C);
extern u32 func_8007E954(u8** pp, u8 index);
extern u32 func_8007E98C(u8** pp, u8 index);
extern u32 func_8007E9D0(u8** pp, u8 index);
extern u32 func_8007EA08(u8** pp, u8 index);
extern u32 func_8007EA4C(u8** pp, u8 index);
extern u32 func_8007EA84(u8** pp, u8 index);
extern u32 func_8007EAC8(u8** pp, u8 index);
extern u32 func_8007EB08(u8** pp, u8 index);
extern u32 func_8007EB50(u8** pp, u8 index);
extern u32 func_8007EB90(u8** pp, u8 index);
extern u32 func_8007EBD8(u8** pp, u8 index);
extern u32 func_8007EC10(u8** pp, u8 index);
extern u32 func_8007EC54(u8** pp, u8 index);
extern u32 func_8007EC94(u8** pp, u8 index);
extern u32 func_8007ECDC(u8** pp, u8 index);
extern u32 func_8007ED14(u8** pp, u8 index);
extern u32 func_8007ED58(u8** pp, u8 index);
extern u32 func_8007ED98(u8** pp, u8 index);
extern u32 func_8007EDE0(u8** pp, u8 index);
extern u32 func_8007EE28(u8** pp, u8 index);
extern u32 func_8007EE70(u8** pp);
extern u32 func_8007EEA8(u8** pp);
extern u32 func_8007EED0(void);
extern u32 func_8007EEE8(void);
extern u32 func_8007EF44(u8 index);
extern void func_80079934(u8** pp);

/* func_8007F8C0.s, byte-exact: evaluate the AI-script condition at *pp
 * for enemy `index`.  0x80 and 0x9A are true, 0x99 opens an OR group,
 * 0x81..0x98 and 0x9B are main30's tests; func_80079934 steps past each
 * record.  Outside a group the first result is returned; inside one the
 * results are ORed while the next opcode is still >= 0x80. */
u8 func_8007F8C0(u8** pp, u8 index) {
    u8 result = 0;
    u8 multi = 0;
    u8 acc = 0;

    while (1) {
        switch (**pp) {
        case 0x80:
        case 0x9A:
            result = 1;
            break;
        case 0x99:
            multi = 1;
            break;
        case 0x81:
            result = func_8007E954(pp, index);
            break;
        case 0x82:
            result = func_8007E98C(pp, index);
            break;
        case 0x83:
            result = func_8007E9D0(pp, index);
            break;
        case 0x84:
            result = func_8007EA08(pp, index);
            break;
        case 0x85:
            result = func_8007EA4C(pp, index);
            break;
        case 0x86:
            result = func_8007EA84(pp, index);
            break;
        case 0x87:
            result = func_8007EAC8(pp, index);
            break;
        case 0x88:
            result = func_8007EB08(pp, index);
            break;
        case 0x89:
            result = func_8007EB50(pp, index);
            break;
        case 0x8A:
            result = func_8007EB90(pp, index);
            break;
        case 0x8B:
            result = func_8007EBD8(pp, index);
            break;
        case 0x8C:
            result = func_8007EC10(pp, index);
            break;
        case 0x8D:
            result = func_8007EC54(pp, index);
            break;
        case 0x8E:
            result = func_8007EC94(pp, index);
            break;
        case 0x8F:
            result = func_8007ECDC(pp, index);
            break;
        case 0x90:
            result = func_8007ED14(pp, index);
            break;
        case 0x91:
            result = func_8007ED58(pp, index);
            break;
        case 0x92:
            result = func_8007ED98(pp, index);
            break;
        case 0x93:
            result = func_8007EDE0(pp, index);
            break;
        case 0x94:
            result = func_8007EE28(pp, index);
            break;
        case 0x95:
            result = func_8007EE70(pp);
            break;
        case 0x96:
            result = func_8007EEA8(pp);
            break;
        case 0x97:
            result = func_8007EED0();
            break;
        case 0x98:
            result = func_8007EEE8();
            break;
        case 0x9B:
            result = func_8007EF44(index);
            break;
        }
        func_80079934(pp);
        if (multi == 0) {
            break;
        }
        acc |= result;
        if (**pp < 0x80) {
            result = acc;
            break;
        }
    }
    return result;
}
extern u8 D_800D2D24[];
extern u8* D_800D2D28;
extern void func_8008FA60(s32 a0);
extern void func_8007765C(void);
#endif

/* func_8007FB70.s */
void func_8007FB70(u8 index) {
    if (D_800D2D24[index] == 4 && D_800D2D28[0x8E] != 0) {
        MAIN31_CALL_8FA60();
        MAIN31_CALL_7765C();
        D_800D2D28[0x8E] = 0;
    }
}

#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/main31", func_8007FBE0);
#endif


#ifndef XENO_PC_PORT
extern u8* D_800D2D28;
#endif
#ifndef XENO_PC_PORT
extern u32 D_800D367C;
#endif
#ifndef XENO_PC_PORT
extern u8 D_800D2D24[];
#endif
#ifndef XENO_PC_PORT
extern u8 D_800C3202[];
#endif

#ifdef XENO_PC_PORT
#include "battle_guest_call.h"
/* The port's HeapFree takes the host pointer; D_800D367C holds the guest
 * address the (bridged) HeapAlloc returned. */
extern u_int HeapFree(void* p);
#define MAIN31_HEAP_PTR(word) BATTLE_HOST_PTR(word)
#else
extern void HeapFree(u32 p);
#define MAIN31_HEAP_PTR(word) (word)
#endif


/* func_8007FCE8.s */
void func_8007FCE8(void) {
    if (D_800D2D28[0xAE] != 0) {
        HeapFree(MAIN31_HEAP_PTR(D_800D367C));
        D_800D2D28[0xAE] = 0;
    }
}


#ifdef XENO_PC_PORT
/* func_8007FBE0.s: lay out count-1 cursor cells (4 halfwords at +0x910 in
 * the D_800C3EA4 work area, 32-byte stride from slot D_800CCB34) from the
 * D_800C3202 row `count`, then record the selection (minus one when it equals
 * the count) and the base slot in D_800D2D28+0x97/+0x98. */
void func_8007FBE0(u8 count, u8 sel) {
    /* D_800C3EA4 (work area address) and D_800CCB34 (slot word) are read as
     * the raw guest words retail loads: other TUs declare them with other
     * types, so the shared guest-RAM aliases do not fit this view. */
    u8 chosen = sel;
    s32 n = (s32)count - 1;
    s32 i;

    if (count == sel) {
        chosen = sel - 1;
    }
    if (n > 0) {
        u32 work = BATTLE_G32(0x800C3EA4u);
        u8* src = D_800C3202 + count * 6;

        for (i = 0; i < n; i++, src++) {
            BATTLE_G16(work + (((u32)i * 2 + BATTLE_G32(0x800CCB34u)) << 4) + 0x910) = 0xC;
            BATTLE_G16(work + (((u32)i * 2 + BATTLE_G32(0x800CCB34u)) << 4) + 0x912) = *src + 0x5E;
            BATTLE_G16(work + (((u32)i * 2 + BATTLE_G32(0x800CCB34u)) << 4) + 0x914) = 0x12;
            BATTLE_G16(work + (((u32)i * 2 + BATTLE_G32(0x800CCB34u)) << 4) + 0x916) = *src + 0x5E;
        }
    }
    D_800D2D28[0x97] = chosen;
    D_800D2D28[0x98] = BATTLE_G8(0x800CCB34u);
}
#endif
