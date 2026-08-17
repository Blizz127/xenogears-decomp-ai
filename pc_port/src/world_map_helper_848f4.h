/*
 * World-map region model submitter 0x800848F4.
 *
 * Retail boundary: [0x800848F4, 0x80084D00), 1036 bytes / 259
 * instructions. Previous wm_800848B4 ends delay-slot sw; this body
 * restores frame 0x40 and jr $ra; nop. Next is accepted wm_80084D00.
 *
 * Overlay callee: wm_80093534 (ACCEPTED). PsyQ: ScaleMatrix,
 * CompMatrix, SetRotMatrix, SetTransMatrix. SLUS: func_8002C700.
 * COP2: RTIR columns, RotTrans MAC, RTPS + FLAG + SZ3.
 *
 * ABI: void. 71A58 jal delay is nop; $a0 unused.
 */
#ifndef WORLD_MAP_HELPER_848F4_H
#define WORLD_MAP_HELPER_848F4_H

void wm_800848F4(void);

#endif /* WORLD_MAP_HELPER_848F4_H */
