/*
 * World-map keep-list GTE submitter 0x800747DC.
 *
 * Retail boundary: [0x800747DC, 0x80074E58), 1660 bytes / 415
 * instructions. Previous accepted wm_80074794 ends before this
 * prologue. This body restores frame 0x68 and jr $ra; nop at
 * 0x80074E50. Next is a new function at 0x80074E58.
 *
 * Overlay callees: wm_80093978, wm_80093740 (ACCEPTED). PsyQ:
 * OuterProduct12 0x8004A480, VectorNormal 0x80048D7C, RotMatrixY
 * 0x8004AFEC, MulMatrix 0x80049ACC, ScaleMatrix 0x80049DCC.
 * COP2: CTC2/RTIR/RT/RTPT/RTPS. No JALR.
 *
 * ABI: void. Early-out if *0x8009BE38 == 0. Loop count is that
 * word; each 8-byte record at *0x8009D30C is submitted into the
 * OT at *(*0x8009BE3C+0x70). Clears *0x8009BE38 on the way out.
 */
#ifndef WORLD_MAP_HELPER_747DC_H
#define WORLD_MAP_HELPER_747DC_H

#include "common.h"

void wm_800747DC(void);

#endif /* WORLD_MAP_HELPER_747DC_H */
