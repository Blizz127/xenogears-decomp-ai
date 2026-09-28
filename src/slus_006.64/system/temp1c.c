#include "common.h"
#include "psyq/libgpu.h"
#include "psyq/libgte.h"
#include "system/memory.h"

/* Retail TU 0x80024FF4..0x80025044 (D_8004FBB8 matrix setter), split out of system/temp1.c: retail temp1
 * was several TUs, which is visible in how each group reaches the shared Gfx
 * globals (gp-relative for its own .sbss, absolute for other TUs'). The port
 * still builds these functions from temp1.c, so this TU is empty there. */
#ifndef XENO_PC_PORT

extern MATRIX D_8004FBB8;

/* Sets the D_8004FBB8 matrix. */
void func_80024FF4(MATRIX* matrix) {
    D_8004FBB8 = *matrix;
}

#endif /* !XENO_PC_PORT */
