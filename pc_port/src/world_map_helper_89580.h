/*
 * World-map 256-slot particle integrator 0x80089580.
 *
 * Retail boundary: [0x80089580, 0x80089748), 456 bytes / 114
 * instructions. Previous body ends jr $ra; nop at 0x80089578.
 * This body restores frame 0x10 and jr $ra; nop at 0x80089740.
 * Next is wm_80089748 (blocked 71A58 callee; this is its only
 * overlay prerequisite).
 *
 * Overlay callees: none. No JAL / JALR. No COP2 / GPU / OT.
 *
 * ABI: void. 89748 jal delay is nop; $a0 unused.
 */
#ifndef WORLD_MAP_HELPER_89580_H
#define WORLD_MAP_HELPER_89580_H

void wm_80089580(void);

#endif /* WORLD_MAP_HELPER_89580_H */
