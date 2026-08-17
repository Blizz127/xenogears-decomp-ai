/*
 * World-map 512-emitter particle spawner 0x80089748.
 *
 * Retail boundary: [0x80089748, 0x80089C78), 1328 bytes / 332
 * instructions. Previous wm_80089580 ends jr $ra; nop at
 * 0x80089740. This body restores frame 0x40 and jr $ra; nop at
 * 0x80089C70. Next is accepted wm_80089C78.
 *
 * Overlay callee: wm_80089580 (ACCEPTED I30). PsyQ: RotMatrixYXZ
 * 0x8004A92C, ApplyMatrix 0x80049CEC, rand 0x8003FA38,
 * VectorNormal 0x80048D7C, ratan2 0x8004B32C. No JALR / COP2 /
 * GPU / OT.
 *
 * ABI: void. 71A58 jal delay is nop; $a0 unused.
 */
#ifndef WORLD_MAP_HELPER_89748_H
#define WORLD_MAP_HELPER_89748_H

void wm_80089748(void);

#endif /* WORLD_MAP_HELPER_89748_H */
