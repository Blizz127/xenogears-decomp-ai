/*
 * World-map POLY_G3 / bit-sprite submitter 0x800740B8.
 *
 * Retail boundary: [0x800740B8, 0x80074594), 1244 bytes / 311
 * instructions. Previous body ends jr $ra; nop at 0x800740B0.
 * This body restores frame 0x38 and jr $ra; nop at 0x8007458C.
 * Next word 0x80074594 is a separate HeapAlloc init (W17B), not
 * part of this function. 0x8007474C is the matching HeapFree.
 *
 * PRE4's [0x800740B8, 0x80074794) (1756 / 439) was wrong: it
 * swallowed those two siblings. 71A58 jals 0x800740B8 only.
 *
 * Overlay callees: none. PsyQ: RotMatrix 0x8003F738.
 * COP2: CTC2 R+T from 0x1F8000F0, RTPT 0x4A280030, swc2 SXY.
 * GPU/OT: insert at *(db+0x70), masks 0xFF000000 / 0x00FFFFFF.
 *
 * ABI: void. 71A58 jal delay is nop; $a0 unused.
 */
#ifndef WORLD_MAP_HELPER_740B8_H
#define WORLD_MAP_HELPER_740B8_H

void wm_800740B8(void);

#endif /* WORLD_MAP_HELPER_740B8_H */
