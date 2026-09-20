/* Optional host debugging aid. OFF preserves the retail instruction stream. */
#include <stdatomic.h>
#include <stdio.h>
#include "god_mode.h"
static atomic_int enabled;
int PcPort_GodModeEnabled(void) { return atomic_load(&enabled); }
void PcPort_GodModeToggle(void)
{
    int value = !PcPort_GodModeEnabled();
    atomic_store(&enabled, value);
    fprintf(stderr, "[xeno-port][god-mode] %s (party HP damage, foot and Gear)\n",
            value ? "ON" : "OFF");
}
void PcPort_GodModeBeforeGuest(PcPortMipsCpu* cpu, uint32_t target)
{
    /* Retail 85618 consumes an action row: HP damage kinds 0/5/7/8,
     * amounts at C3FE8 + action*72 + slot*2, kinds at C4000+action*72.
     * Slots 0..2 are party, 3..10 enemies. C3EB8+slot*28 selects Gear.
     * Change the queued damage before HP subtraction/death flags, never HP
     * initialization, healing, fuel, EP, enemy state or story flags. */
    if (target != 0x80085618u || !PcPort_GodModeEnabled()) return;
    uint32_t row = (cpu->gpr[4] & 255u) * 72u;
    for (uint32_t slot = 0; slot < 3; ++slot) {
        uint32_t active, kind, gear, amount;
        uint32_t address = 0x800C3FE8u + row + slot * 2u;
        if (cpu->bus.read(cpu->bus.opaque, 0x800D2DCCu+slot, 1, &active) ||
            !active || cpu->bus.read(cpu->bus.opaque, 0x800C4000u+row+slot, 1, &kind) ||
            (kind != 0 && kind != 5 && kind != 7 && kind != 8) ||
            cpu->bus.read(cpu->bus.opaque, 0x800C3EB8u+slot*28u, 1, &gear) ||
            cpu->bus.read(cpu->bus.opaque, address, 2, &amount)) continue;
        if (amount == 0 || (!gear && (amount & 0x8000u))) continue;
        if (cpu->bus.write(cpu->bus.opaque, address, 2, 0) == 0)
            fprintf(stderr, "[xeno-port][god-mode] blocked %u HP damage slot=%u gear=%u\n",
                    amount, slot, gear != 0);
    }
}
