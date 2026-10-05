/* Battle guest ABI: one convention for matched battle C that runs natively.
 *
 * (M-a, docs: battle-abi-design.md, approach (a).)  Retail battle code keeps
 * every pointer as a 32-bit guest address in guest RAM, and the MIPS
 * interpreter that still runs the rest of the overlay reads those same words.
 * So a native body must too: a guest word ALWAYS holds a guest-domain address,
 * and matched C touches guest pointers only through these macros.
 *
 *   GPTR(T, x)       x is a guest address (or a host pointer handed over by
 *                    the bridge): the T* to dereference.          (P1, P4 read)
 *   GADDR(p)         the u32 guest form of pointer/address p, for storing
 *                    into guest RAM or doing retail arithmetic.   (P5, P6)
 *   GSLOT(T)         parameter type of a T** that points at a 4-byte guest
 *                    slot (u8** ppBoard): u32* under the port.   (P2)
 *   GLOAD(T, pp)     the T* held by the guest slot *pp.          (P2, P3)
 *   GGET(sym)        the pointer held by pointer-kind overlay global sym,
 *                    written `*(void **)&sym` in retail C.       (P3 read)
 *   GSTORE(sym, v)   store pointer v into pointer-kind overlay global sym,
 *                    as its guest address.                       (P3 write)
 *
 * In the matching build every macro expands to the original expression (plus
 * redundant parentheses), so gcc 2.7.2 sees the same tree; `make rom-check`
 * proves it per change.  Under XENO_PC_PORT they route through bg_ptr/bg_addr,
 * which call the same resolver the interpreter's bus uses
 * (pc_port/src/battle_guest_resolver.h), so a native and an interpreted body
 * can never read different bytes for one guest address. */
#ifndef XENO_BATTLE_GUEST_ABI_H
#define XENO_BATTLE_GUEST_ABI_H

#ifndef XENO_PC_PORT

#define GPTR(T, x) ((T*)(x))
#define GADDR(p) ((u32)(p))
#define GSLOT(T) T**
#define GLOAD(T, pp) (*(pp))
#define GGET(sym) (*(void**)&(sym))
#define GSTORE(sym, v) ((*(void**)&(sym)) = (v))

#else

#include <stdint.h>
#include "battle_host_guest.h"

#define GPTR(T, x) ((T*)bg_ptr((u32)(uintptr_t)(x)))
#define GADDR(p) bg_addr((const void*)(uintptr_t)(p))
#define GSLOT(T) u32*
#define GLOAD(T, pp) ((T*)bg_ptr(*(pp)))
/* sym##__slot is the generated u32 lvalue of the global's guest word
 * (pc_port/src/battle_overlay_guest_ram.h); pasting suppresses the expansion
 * of sym itself, which is the dereferencing rvalue alias. */
#define GGET(sym) ((void*)bg_ptr(sym##__slot))
#define GSTORE(sym, v) (sym##__slot = GADDR(v))

#endif

#endif
