/*
 * World-map four-quad submitter 0x800737EC.
 *
 * Retail boundary: [0x800737EC, 0x800739B8), 460 bytes / 115
 * instructions. PsyQ only (RotMatrixYXZ, CompMatrix, SetRotMatrix,
 * SetTransMatrix, RotTransPers4).
 *
 * ABI: void. Projects four quads at 0x8009A280 into packets at
 * 0x8009D194 + index*36 and inserts each into the DB OT when the
 * GTE flag is non-negative.
 */
#ifndef WORLD_MAP_HELPER_737EC_H
#define WORLD_MAP_HELPER_737EC_H

void wm_800737EC(void);

#endif /* WORLD_MAP_HELPER_737EC_H */
