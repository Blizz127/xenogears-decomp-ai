#ifndef XENO_SHIM_PSYQ_LIBGTE_H
#define XENO_SHIM_PSYQ_LIBGTE_H
/* Port build: redirect the game's PsyQ GTE header to PsyCross's implementation. */
#include <libgte.h>

/* Xenogears' decomp symbol map names the retail executable's two trig entry
 * points opposite to PsyCross's conventional implementations.  At 0x8003F8B0
 * the decomp `rcos` entry reads the sine halfwords rooted at D_800523F0,
 * while 0x8003F8CC (`rsin`) reads the cosine halfwords at D_800523F2.
 *
 * Keep PsyCross itself conventional, but preserve the retail entry-point
 * semantics for game translation units.  In particular, the field camera
 * calls retail `rsin` for its vertical component; using mathematical sine
 * puts an unclamped camera below the Lahan well instead of above it. */
static inline int xeno_retail_rsin(int angle) {
    return rcos(angle);
}

static inline int xeno_retail_rcos(int angle) {
    return rsin(angle);
}

#define rsin xeno_retail_rsin
#define rcos xeno_retail_rcos
#endif
