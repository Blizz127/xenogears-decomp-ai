/*
 * World-map POLY_FT4 billboard submitter 0x80089C78.
 *
 * Retail boundary: [0x80089C78, 0x8008A2C8), 1616 bytes / 404
 * instructions. Previous wm_80089748 ends jr $ra; nop. This body
 * restores frame 0x50 and jr $ra; nop. Next is accepted
 * wm_8008A2C8.
 *
 * Overlay callee: wm_80093534 (ACCEPTED). PsyQ: RotMatrixZ,
 * ScaleMatrix, ApplyMatrix, SetRotMatrix, SetTransMatrix.
 * COP2: CTC2 camera R + rtv0 (0x4A486012, cv=none), CTC2 work
 * MATRIX, RTPT 0x4A280030, RTPS 0x4A180001, cfc2 FLAG, swc2 SXY/SZ3.
 *
 * ABI: void. 71A58 jal delay is nop; $a0 unused.
 */
#ifndef WORLD_MAP_HELPER_89C78_H
#define WORLD_MAP_HELPER_89C78_H

void wm_80089C78(void);

#endif /* WORLD_MAP_HELPER_89C78_H */
