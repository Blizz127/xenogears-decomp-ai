/* battle mainasm_AD84: the retail-asm run at file offset 0xAD84, in retail order.
 * Functions still listed as INCLUDE_ASM are not byte-matching C yet. The whole
 * file is retail-build only; any port body for these functions lives in the
 * port's own TU. */
#include "common.h"

#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/mainasm_AD84", func_8007A874);
#endif
