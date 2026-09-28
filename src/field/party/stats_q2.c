/* Retail run 2 of stats.c: its retail-asm functions (not yet byte-matching
 * C), then its byte-exact C bodies; see the FIELD_SUB note in stats.c. */
#define FIELD_TU_SUB 2
#include "common.h"
#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/field/nonmatchings/party/stats_q2", func_80096AF4);
#endif
#include "stats.c"
