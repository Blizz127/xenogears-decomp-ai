/* Retail 26644.s: 80035E44-80035F1C and 80036188-80036258.
 * These helpers are prerequisites for the unresolved vblank dispatcher;
 * defining them does not yet install or schedule that dispatcher. */
#include <stdint.h>

extern uint8_t D_800501F8, D_80059370, D_80059418, D_80059420, D_80059484;
extern uint8_t D_8005A1BC[];

void func_80035E44(void)
{
    if (D_800501F8 != 0)
        return;
    ++D_80059370;
    if (D_80059370 == 60) {
        D_80059370 = 0;
        ++D_80059418;
    }
    if (D_80059418 == 60) {
        D_80059418 = 0;
        ++D_80059420;
    }
    if (D_80059420 == 60) {
        D_80059420 = 0;
        ++D_80059484;
    }
    if (D_80059484 == 100)
        D_800501F8 = 1;
}

void func_80036188(uint8_t* record)
{
    uint16_t count;
    if (record[7] != 0)
        return;
    /* Byte access preserves the little-endian record without host alignment
     * or aliasing assumptions. Retail's signed load is only compared to zero. */
    count = (uint16_t)(record[4] | ((uint16_t)record[5] << 8));
    if (count != 0) {
        record[0] = 1;
        record[1] = 0x40;
        record[2] = 1;
        record[3] = 0;
        record[6] = 1;
        --count;
        record[4] = (uint8_t)count;
        record[5] = (uint8_t)(count >> 8);
    } else if (record[6] == 1) {
        record[0] = 1;
        record[1] = 0x40;
        record[2] = 0;
        record[3] = 0;
        record[6] = 2;
    } else if (record[6] == 2) {
        record[0] = 0;
        record[6] = 0;
    }
}

void func_80036220(void)
{
    func_80036188(D_8005A1BC);
    func_80036188(D_8005A1BC + 8);
}
