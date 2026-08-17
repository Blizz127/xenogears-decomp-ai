/*
 * World-map helpers 0x8009623C..0x8009699C — primitive submission queue
 * and related leaf functions for the world-map rendering system.
 *
 * These are small leaf functions (29-70 insns each) that manage a
 * primitive record queue and related operations.
 */
#ifndef WORLD_MAP_HELPER_9623C_H
#define WORLD_MAP_HELPER_9623C_H

#include "common.h"

/* Queue writer: writes 3 words to next record slot. Returns 0 on success, -1 if full. */
s32 wm_8009623C(u32 word0, u32 word1, u32 word2);

/* Queue writer variant with different record layout */
s32 wm_800962B0(u32 word0, u32 word1, u32 word2, u32 word3);

/* Queue writer with 4 words */
s32 wm_80096328(u32 word0, u32 word1, u32 word2, u32 word3);

/* Queue record initializer */
void wm_800963E4(u32 record_addr);

/* Queue record copier */
void wm_800964B0(u32 src, u32 dst);

/* Queue record copier with offset */
void wm_800965A4(u32 src, u32 dst, u32 offset);

/* GTE-related queue setup */
void wm_800966CC(u32 addr);

/* Queue submitter */
void wm_800967E4(u32 addr, u32 count);

/* Queue record zero-fill */
void wm_800968E0(u32 addr, u32 count);

/* Queue record writer with GTE ops */
void wm_8009699C(u32 addr, u32 val1, u32 val2);

/* Queue counter reset */
void wm_800980D4(void);

/* Queue flush helper */
void wm_80099708(u32 addr);

/* CD40 sink (simple record writer) */
void wm_80086700(u32 addr, u32 val);

#endif
