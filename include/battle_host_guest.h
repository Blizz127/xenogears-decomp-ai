/* Guest-address accessors for port-only battle overlay bodies.
 *
 * A port body under XENO_PC_PORT runs natively while the battle overlay's
 * data stays in the loaded retail image (g_PsxRam).  Retail code keeps
 * 32-bit guest addresses in its structs and globals, so a body written from
 * the disassembly is most faithful when it does the same: hold every overlay
 * pointer as a u32 guest address and dereference it through these macros.
 * That keeps the retail 32-bit address arithmetic (and the RAM mirror mask)
 * instead of turning it into host pointer arithmetic.
 *
 * Boundary rules (see pc_port/src/battle_mips_runtime.c, runtime_bridge_call):
 *  - the bridge translates a KSEG0/KSEG1/scratchpad argument into a host
 *    pointer, so a parameter that can carry an address is declared as a
 *    pointer and converted back with BG_ADDR() on entry;
 *  - a callee's pointer parameter gets BG_PTR(guest address);
 *  - main-executable globals are NOT in guest RAM on the port (the runtime
 *    binds them to their native copies), so they are accessed through their
 *    C extern names, never through these macros.
 *
 * Only for XENO_PC_PORT; the matching build never includes this. */
#ifndef XENO_BATTLE_HOST_GUEST_H
#define XENO_BATTLE_HOST_GUEST_H

#ifdef XENO_PC_PORT
#include <stdint.h>
#include "psx_memory.h"
#include "types.h"

static inline void* bg_ptr(u32 address) {
    if ((address & 0xFFFFF000u) == 0x1F800000u) {
        return g_PsxScratchpad + (address & 0xFFFu);
    }
    return PSX_ADDR(address);
}

static inline u32 bg_addr(const void* pointer) {
    uintptr_t host = (uintptr_t)pointer;
    uintptr_t pad = (uintptr_t)g_PsxScratchpad;
    if (pointer != NULL && host >= pad && host < pad + 0x1000u) {
        return 0x1F800000u | (u32)(host - pad);
    }
    return PsxMemory_GuestAddr(pointer);
}

#define BG_PTR(a) ((u8*)bg_ptr((u32)(a)))
#define BG_ADDR(p) bg_addr((const void*)(p))
#define BG_U8(a) (*(u8*)bg_ptr((u32)(a)))
#define BG_S8(a) (*(s8*)bg_ptr((u32)(a)))
#define BG_U16(a) (*(u16*)bg_ptr((u32)(a)))
#define BG_S16(a) (*(s16*)bg_ptr((u32)(a)))
#define BG_U32(a) (*(u32*)bg_ptr((u32)(a)))
#define BG_S32(a) (*(s32*)bg_ptr((u32)(a)))

/* R3000 DIV/DIVU results for the cases C leaves undefined (the interpreter,
 * pc_port/src/battle_mips_adapter.c, models the same values).  Only for
 * divisions retail emits without its own `break 7` zero check. */
static inline u32 bg_divu(u32 a, u32 b) { return b != 0 ? a / b : 0xFFFFFFFFu; }
static inline u32 bg_remu(u32 a, u32 b) { return b != 0 ? a % b : a; }
static inline s32 bg_div(s32 a, s32 b) {
    if (b == 0) return a >= 0 ? -1 : 1;
    if (a == (s32)0x80000000 && b == -1) return a;
    return a / b;
}
static inline s32 bg_rem(s32 a, s32 b) {
    if (b == 0) return a;
    if (a == (s32)0x80000000 && b == -1) return 0;
    return a % b;
}
#endif

#endif
