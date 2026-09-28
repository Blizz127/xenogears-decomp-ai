/* battle mainasm_BF14: the retail-asm run at file offset 0xBF14, in retail order.
 * Functions still listed as INCLUDE_ASM are not byte-matching C yet. The whole
 * file is retail-build only; any port body for these functions lives in the
 * port's own TU. */
#include "common.h"

#ifndef XENO_PC_PORT
INCLUDE_ASM("asm/battle/nonmatchings/mainasm_BF14", func_8007BA04);
INCLUDE_ASM("asm/battle/nonmatchings/mainasm_BF14", func_8007BA44);
#endif
