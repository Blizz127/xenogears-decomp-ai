/*
 * World-map actor billboard submitter 0x80085CDC.
 *
 * Retail boundary: [0x80085CDC, 0x80085F58), 636 bytes / 159
 * instructions. Previous function wm_80085760 ends jr $ra; nop at
 * 0x80085CD4. This body restores frame 0x28 and jr $ra; nop; the next
 * function (init-only 0x80085F58) starts a new prologue.
 *
 * PRE4's [0x80085CDC, 0x80085FE0) incorrectly included 85F58.
 *
 * Overlay callee: wm_80093484 (ACCEPTED). SLUS/PsyQ: SetRotMatrix,
 * SetTransMatrix, func_80024FF4, func_8001E298, func_800223B0,
 * AnimScriptTick. COP2 RTPS + swc2 SZ3.
 *
 * ABI: void. 71A58 jal has a nop delay; $a0 is unused.
 */
#ifndef WORLD_MAP_HELPER_85CDC_H
#define WORLD_MAP_HELPER_85CDC_H

void wm_80085CDC(void);

#endif /* WORLD_MAP_HELPER_85CDC_H */
