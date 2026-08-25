#ifndef XENO_TEST_INPUT_H
#define XENO_TEST_INPUT_H

#include "common.h"

/* Parse XENO_TEST_INPUT before game startup. Returns zero when unset/valid. */
int PcPort_TestInputInit(void);

/* Advance the shared field/world frame clock, then inject at an input seam. */
void PcPort_TestInputAdvanceFrame(void);
void PcPort_TestInputInject(u16 *held_buttons);

#if defined(XENO_TEST_INPUT_CERTIFICATE)
void PcPort_TestInputResetForCertificate(void);
#endif

#endif
