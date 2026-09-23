#ifndef XENO_SHIM_BATTLE_GUEST_CALL_H
#define XENO_SHIM_BATTLE_GUEST_CALL_H
/* Port-only.  A native battle-overlay C body that calls an overlay routine
 * which has no C body yet must still run that routine: a generated no-op stub
 * would silently drop its effect.  BATTLE_GUEST_CALL runs the retail bytes of
 * the callee through the battle MIPS runtime instead (the same service the
 * native file-1 controller uses, PcPort_BattleMipsCallGuest), so the host body
 * behaves exactly as the interpreted caller would have.
 *
 * Arguments are raw guest words: convert host pointers with
 * PsxMemory_GuestAddr() before passing them.  The result is the callee's v0.
 * A failed guest call is fatal -- retail would have faulted there too, and
 * resuming as if the helper had run would corrupt battle state quietly.
 *
 * Replace a call site with the direct C call once the callee has an adopted
 * host body. */
#if defined(XENO_PC_PORT)
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int PcPort_BattleMipsCallGuest(uint32_t target, const uint32_t *args,
                              unsigned argc, uint32_t *result);

static inline uint32_t BattleGuestCallN(uint32_t target, unsigned argc,
                                        const uint32_t *args)
{
    uint32_t result = 0;
    if (PcPort_BattleMipsCallGuest(target, args, argc, &result) != 0) {
        fprintf(stderr,
                "[xeno-port][battle-guest-call] guest call 0x%08x failed\n",
                (unsigned)target);
        abort();
    }
    return result;
}

#define BATTLE_GUEST_CALL0(target) BattleGuestCallN((target), 0u, NULL)
#define BATTLE_GUEST_CALL(target, ...)                                      \
    ({                                                                      \
        const uint32_t battle_guest_args_[] = { __VA_ARGS__ };              \
        BattleGuestCallN((target),                                          \
                         (unsigned)(sizeof(battle_guest_args_) /            \
                                    sizeof(battle_guest_args_[0])),         \
                         battle_guest_args_);                               \
    })
#endif

#endif
