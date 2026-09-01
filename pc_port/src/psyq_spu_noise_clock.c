/* Retail PsyQ SpuSetNoiseClock: clamp to six bits and update SPUCNT[13:8]. */
#include <string.h>

#include "common.h"
#include "psyq_spu_noise_clock.h"

/* The port's sound translation unit backs the retail memory-mapped SPU page
 * with native storage.  SPUCNT retains its retail byte offset, 0x1AA. */
extern void* g_pSoundSpuRegisters;

#if defined(W34N121_MUTANT_WRONG_REGISTER_OFFSET)
#define SNC_SPUCNT_OFFSET 0x1A8u
#else
#define SNC_SPUCNT_OFFSET 0x1AAu
#endif

static u16 snc_load_u16(const void* source)
{
    u16 value;
    memcpy(&value, source, sizeof(value));
    return value;
}

static void snc_store_u16(void* destination, u16 value)
{
    memcpy(destination, &value, sizeof(value));
}

long SpuSetNoiseClock(long noise_clock)
{
    long clamped;
    u8* registers = (u8*)g_pSoundSpuRegisters;
    u16 spucnt;
    u16 new_value;

    if (noise_clock < 0) {
#if defined(W34N121_MUTANT_NO_LOW_CLAMP)
        clamped = noise_clock;
#else
        clamped = 0;
#endif
    } else if (noise_clock > 0x3F) {
#if defined(W34N121_MUTANT_NO_HIGH_CLAMP)
        clamped = noise_clock;
#else
        clamped = 0x3F;
#endif
    } else {
        clamped = noise_clock;
    }

    spucnt = snc_load_u16(registers + SNC_SPUCNT_OFFSET);
#if defined(W34N121_MUTANT_WRONG_PRESERVE_MASK)
    new_value = (u16)((spucnt & 0x00FFu) |
                      (((u16)clamped & 0x003Fu) << 8));
#elif defined(W34N121_MUTANT_WRONG_SHIFT)
    new_value = (u16)((spucnt & 0xC0FFu) |
                      (((u16)clamped & 0x003Fu) << 7));
#else
    new_value = (u16)((spucnt & 0xC0FFu) |
                      (((u16)clamped & 0x003Fu) << 8));
#endif
    snc_store_u16(registers + SNC_SPUCNT_OFFSET, new_value);

#if defined(W34N121_MUTANT_RETURN_INPUT)
    return noise_clock;
#else
    return clamped;
#endif
}
