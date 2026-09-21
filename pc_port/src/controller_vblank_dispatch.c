/* Retail asm/slus_006.64/26644.s, 8003634C-80036400. Scheduling and
 * channel-4 registration are separate from this interrupt body. */
#include <signal.h>
#include <stdint.h>
#include <string.h>
#include "system/controller_vblank.h"

extern int32_t D_80059488, D_80010000, D_80059390;
extern void ControllerPoll(void);
extern void ControllerPushState(void);
extern void func_80035E44(void);
extern void func_80036220(void);
/* Optional native input/test seams; absent in isolated retail-body tests. */
extern void PcPort_PrepareControllerPoll(void) __attribute__((weak));
extern void PcPort_BeforeControllerPush(void) __attribute__((weak));

/* This is native callback storage, not a serialized guest word. */
void (*D_800501FC)(void);

void func_800363F0(void (*callback)(void))
{
    D_800501FC = callback;
}

void func_800363E0(int enabled)
{
    D_80059390 = enabled;
}

void func_8003634C(void)
{
    uint32_t ticks = (uint32_t)D_80059488 + 1u;
    /* ADDIU wraps without trapping; preserve its bits without signed C UB. */
    memcpy(&D_80059488, &ticks, sizeof(ticks));
    if (PcPort_PrepareControllerPoll)
        PcPort_PrepareControllerPoll();
    ControllerPoll();
    if (PcPort_BeforeControllerPush)
        PcPort_BeforeControllerPush();
    ControllerPushState();
    func_80035E44();
    func_80036220();
    if (D_800501FC != NULL)
        D_800501FC();
    /* A native debugger can stop and resume at the retail BREAK 1 condition.
     * Flags must be read after the callback, which may change either one. */
    if (D_80010000 != -1 && D_80059390 != 0)
        raise(SIGTRAP);
}
