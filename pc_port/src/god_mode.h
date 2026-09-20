#ifndef XENO_PC_PORT_GOD_MODE_H
#define XENO_PC_PORT_GOD_MODE_H
#include "battle_mips_adapter.h"
int PcPort_GodModeEnabled(void);
void PcPort_GodModeToggle(void);
void PcPort_GodModeBeforeGuest(PcPortMipsCpu* cpu, uint32_t target);
#endif
