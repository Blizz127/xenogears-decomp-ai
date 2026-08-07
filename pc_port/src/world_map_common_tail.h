/*
 * World-map common-tail prefix + 0x80089160 initializer (W34B5A).
 *
 * Common-tail prefix: 0x8007290C–0x80072938.
 * Selector-dependent logic: C610=0 calls wm_80089160(14,0,0).
 * Reconvergence at 0x8007293C (first excluded = next-phase helper).
 *
 * 0x80089160: bounded table/state initializer 0x80089160–0x800893D4.
 * Leaf function, no direct calls. Record stride 672 bytes.
 */
#ifndef WORLD_MAP_COMMON_TAIL_H
#define WORLD_MAP_COMMON_TAIL_H

#include "common.h"

/* Retail cut PC constants. */
#define WM_COMMON_TAIL_P0_START      0x8007290Cu
#define WM_COMMON_TAIL_P0_CUT        0x8007293Cu  /* reconvergence / first excluded */
#define WM_COMMON_TAIL_P1_START      0x8007293Cu
#define WM_COMMON_TAIL_P1_CUT        0x80072944u  /* first excluded after wm_800978FC */
#define WM_COMMON_TAIL_P2_START      0x80072944u
#define WM_COMMON_TAIL_P2_CUT        0x8007294Cu  /* first excluded after wm_8008901C */
#define WM_COMMON_TAIL_P3_START      0x8007294Cu
#define WM_COMMON_TAIL_P3_CUT        0x80072954u  /* first excluded after wm_800865A0 */
#define WM_800865A0_START            0x800865A0u
#define WM_800865A0_END_EXCLUSIVE    0x800866C8u
#define WM_8008901C_START            0x8008901Cu
#define WM_8008901C_END_EXCLUSIVE    0x80089128u
#define WM_80089160_START            0x80089160u
#define WM_80089160_END_EXCLUSIVE    0x800893D8u
#define WM_800978FC_START            0x800978FCu
#define WM_800978FC_END_EXCLUSIVE    0x800979C8u

/* Next-phase forbidden targets. */
#define WM_NEXT_HELPER_978FC         0x800978FCu
#define WM_NEXT_HELPER_8901C         0x8008901Cu
#define WM_NEXT_HELPER_865A0         0x800865A0u
#define WM_NEXT_HELPER_85FE0         0x80085FE0u
#define WM_NEXT_HELPER_75228         0x80075228u
#define WM_SCHEDULER_97800           0x80097800u
#define WM_WORLD_LOOP_712D0          0x800712D0u

/* D_80059179 written by the prefix. */
#define WM_D_80059179_ABS            0x80059179u

/* 0x80089160 constants. */
#define WM_89160_TABLE_BASE_PTR      0x8009BCC0u
#define WM_89160_RECORD_STRIDE       672u    /* 0x2A0 */
#define WM_89160_SUBRECORD_STRIDE    0x54u   /* 84 bytes */
#define WM_89160_SUBRECORD_COUNT     8
#define WM_89160_FLAG_BYTE_OFFSET    0x4Fu
#define WM_89160_FLAG_BIT            0x80u

/* Production functions. */

/* wm_80089160: full native implementation of retail 0x80089160.
 * Leaf function. Arguments: a0=record index, a1=src ptr, a2=src ptr.
 * For natural C610=0 path: (14, 0, 0). */
void wm_80089160(u32 a0, u32 a1, u32 a2);

/* wm_8007290C_common_tail_p0: bounded common-tail prefix.
 * Reads C610, dispatches to wm_80089160 for C610=0.
 * Returns the exact new cut PC (0x8007293C). */
u32 wm_8007290C_common_tail_p0(void);

/* Per-world-init reset. Call at each new world init boundary. */
void wm_common_tail_p0_reset(void);

/* Counter accessors. */
int  wm_ctp0_get_entry(void);
int  wm_ctp0_get_c610_zero(void);
int  wm_ctp0_get_c610_nonzero(void);
int  wm_ctp0_get_89160_calls(void);
u32  wm_ctp0_get_last_cut(void);
int  wm_ctp0_get_forbidden_978fc(void);
int  wm_ctp0_get_forbidden_scheduler(void);
int  wm_ctp0_get_forbidden_world_loop(void);
int  wm_ctp0_get_forbidden_8901c(void);
int  wm_ctp0_get_forbidden_865a0(void);
int  wm_ctp0_get_forbidden_85fe0(void);
int  wm_ctp0_get_forbidden_75228(void);

/* wm_800978FC: world-map graphics buffer allocator.
 * Allocates two 64 KB buffers, initializes first with repeating
 * byte pattern (2048 records × 32 bytes), copies to second.
 * Self-contained leaf — only calls HeapAlloc. */
void wm_800978FC(void);

/* wm_8007293C_common_tail_p1: caller slice from accepted P0 frontier.
 * Calls wm_800978FC exactly once.
 * Returns exact new cut PC (0x80072944).
 * Requires P0 to have executed and returned 0x8007293C. */
u32 wm_8007293C_common_tail_p1(void);

/* P1 per-world-init reset. */
void wm_common_tail_p1_reset(void);

/* P1 counter accessors. */
int  wm_ctp1_get_entry(void);
int  wm_ctp1_get_978fc_calls(void);
u32  wm_ctp1_get_last_cut(void);

/* 0x80089160 instrumentation. */
int  wm_89160_get_calls(void);
int  wm_89160_get_iterations(void);
void wm_89160_reset(void);

/* 0x8008901C constants. */
#define WM_8901C_ALLOC_SIZE          0x2800u   /* 10240 bytes */
#define WM_8901C_ALLOC_COUNT         2
#define WM_8901C_RECORD_COUNT        256
#define WM_8901C_RECORD_STRIDE       40u       /* 0x28 */
#define WM_8901C_RECORD_BASE_OFFSET  7         /* ptr + 7 */
#define WM_8901C_COPY_CHUNK          16

/* Global addresses written by wm_8008901C. */
#define WM_D_8009BE1C_ABS            0x8009BE1Cu
#define WM_D_8009BE20_ABS            0x8009BE20u

/* 0x800865A0 constants. */
#define WM_865A0_ALLOC_SIZE          0x2D00u   /* 11520 bytes */
#define WM_865A0_ALLOC_COUNT         2
#define WM_865A0_RECORD_COUNT        288
#define WM_865A0_RECORD_STRIDE       40u       /* 0x28 */
#define WM_865A0_RECORD_BASE_OFFSET  14        /* ptr + 14 */
#define WM_865A0_COPY_CHUNK          16

/* Global addresses written by wm_800865A0. */
#define WM_D_8009D7F8_ABS            0x8009D7F8u
#define WM_D_8009D7FC_ABS            0x8009D7FCu

/* wm_8008901C: world-map secondary buffer allocator (retail 0x8008901C).
 * Allocates two 10240-byte buffers, initializes 256 × 40-byte records
 * in the first (with GetTPage/GetClut halfwords), copies to second.
 * Self-contained — only calls HeapAlloc + PsyQ GetTPage/GetClut. */
void wm_8008901C(void);

/* wm_80072944_common_tail_p2: caller slice from accepted P1 frontier.
 * Calls wm_8008901C exactly once.
 * Returns exact new cut PC (0x8007294C).
 * Requires P1 to have executed and returned 0x80072944. */
u32 wm_80072944_common_tail_p2(void);

/* P2 per-world-init reset. */
void wm_common_tail_p2_reset(void);

/* P2 counter accessors. */
int  wm_ctp2_get_entry(void);
int  wm_ctp2_get_8901c_calls(void);
u32  wm_ctp2_get_last_cut(void);
int  wm_ctp2_get_alloc_calls(void);
int  wm_ctp2_get_forbidden_865a0(void);
int  wm_ctp2_get_forbidden_85fe0(void);
int  wm_ctp2_get_forbidden_scheduler(void);
int  wm_ctp2_get_forbidden_world_loop(void);

/* wm_800865A0: world-map tertiary buffer allocator (retail 0x800865A0).
 * Allocates two 11520-byte (0x2D00) buffers via HeapAlloc.
 * Stores pointers at D_8009D7F8 and D_8009D7FC.
 *
 * Initializes 288 records (40 bytes each) in the first buffer.
 * Record base = alloc_ptr + 14.  Per-record writes:
 *   byte[+3]  = 9    (type marker)
 *   byte[+4]  = 38 (0x26)
 *   byte[+5]  = 38 (0x26)
 *   byte[+6]  = 38 (0x26)
 *   byte[+7]  = 44 (0x2C), then OR'd with 0x02 → 0x2E (SetSemiTrans)
 *   hw[+14]   = GetTPage(0, 1, 960, 256)
 *   hw[+22]   = GetClut(304, 510)
 *
 * Then copies first buffer → second buffer (11520 bytes, 16-byte chunks).
 *
 * Calls: HeapAlloc, PsyQ GetTPage, PsyQ GetClut, PsyQ SetSemiTrans. */
void wm_800865A0(void);

/* wm_8007294C_common_tail_p3: caller slice from accepted P2 frontier.
 * Calls wm_800865A0 exactly once.
 * Returns exact new cut PC (0x80072954).
 * Requires P2 to have executed and returned 0x8007294C. */
u32 wm_8007294C_common_tail_p3(void);

/* P3 per-world-init reset. */
void wm_common_tail_p3_reset(void);

/* P3 counter accessors. */
int  wm_ctp3_get_entry(void);
int  wm_ctp3_get_865a0_calls(void);
u32  wm_ctp3_get_last_cut(void);
int  wm_ctp3_get_alloc_calls(void);
int  wm_ctp3_get_forbidden_85fe0(void);
int  wm_ctp3_get_forbidden_scheduler(void);
int  wm_ctp3_get_forbidden_world_loop(void);
int  wm_ctp3_get_forbidden_75228(void);

/* 0x80085FE0 constants. */
#define WM_85FE0_ALLOC_SIZE          0x5000u   /* 20480 bytes */
#define WM_85FE0_ALLOC_COUNT         2
#define WM_85FE0_RECORD_COUNT        512
#define WM_85FE0_RECORD_STRIDE       40u       /* 0x28 */
#define WM_85FE0_RECORD_BASE_OFFSET  22        /* ptr + 22 */
#define WM_85FE0_COPY_CHUNK          16

/* Global addresses written by wm_80085FE0. */
#define WM_D_8009D7E8_ABS            0x8009D7E8u
#define WM_D_8009D7EC_ABS            0x8009D7ECu

/* Common-tail P4 constants. */
#define WM_COMMON_TAIL_P4_START      0x80072954u
#define WM_COMMON_TAIL_P4_CUT        0x8007295Cu  /* first excluded after wm_80085FE0 */

/* wm_80085FE0: world-map quaternary buffer allocator (retail 0x80085FE0).
 * Allocates two 20480-byte (0x5000) buffers via HeapAlloc.
 * Stores pointers at D_8009D7E8 and D_8009D7EC.
 *
 * Initializes 512 records (40 bytes each) in the first buffer.
 * Record base = alloc_ptr + 22.  Per-record writes:
 *   byte[-19] = 9    (type marker)
 *   byte[-18] = 128  (R)
 *   byte[-17] = 128  (G)
 *   byte[-16] = 128  (B)
 *   byte[-15] = 44   (code byte)
 *   byte[-10] = 0
 *   byte[-9]  = 64   (0x40)
 *   byte[-2]  = 31   (0x1F)
 *   byte[-1]  = 64   (0x40)
 *   byte[+6]  = 0
 *   byte[+7]  = 111  (0x6F)
 *   byte[+14] = 31   (0x1F)
 *   byte[+15] = 111  (0x6F)
 *   hw[-8]    = GetClut(240, 511)
 *   hw[+0]    = GetTPage(0, 1, 240, 511)
 *
 * Then copies first buffer → second buffer (20480 bytes, 16-byte chunks).
 *
 * Calls: HeapAlloc, PsyQ GetTPage, PsyQ GetClut. */
void wm_80085FE0(void);

/* wm_80072954_common_tail_p4: caller slice from accepted P3 frontier.
 * Calls wm_80085FE0 exactly once.
 * Returns exact new cut PC (0x8007295C).
 * Requires P3 to have executed and returned 0x80072954. */
u32 wm_80072954_common_tail_p4(void);

/* P4 per-world-init reset. */
void wm_common_tail_p4_reset(void);

/* P4 counter accessors. */
int  wm_ctp4_get_entry(void);
int  wm_ctp4_get_85fe0_calls(void);
u32  wm_ctp4_get_last_cut(void);
int  wm_ctp4_get_alloc_calls(void);
int  wm_ctp4_get_forbidden_75228(void);
int  wm_ctp4_get_forbidden_scheduler(void);
int  wm_ctp4_get_forbidden_world_loop(void);

#endif /* WORLD_MAP_COMMON_TAIL_H */
