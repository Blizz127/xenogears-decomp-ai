#ifndef PC_PORT_KROM_ROM_H
#define PC_PORT_KROM_ROM_H
#include "krom_mapping.h"

typedef struct PcPortKromRom PcPortKromRom;
typedef enum {
    PC_PORT_KROM_ROM_OK,
    PC_PORT_KROM_ROM_IO,
    PC_PORT_KROM_ROM_SIZE,
    PC_PORT_KROM_ROM_HASH,
    PC_PORT_KROM_ROM_MEMORY
} PcPortKromRomResult;

/* Load one regular file of exactly 512KB, pinned to SCPH-5500 SHA256
 * 11052b6499e466bbf0a709b1f9cb6834a9418e66680387912451e971cf8a1fef.
 * No path search, fallback BIOS, global state or guest stack defaults.
 * out must be non-NULL and not own an existing handle; set NULL on failure.
 * A successful handle owns its verified copy, independent of later file edits. */
PcPortKromRomResult PcPortKromRomLoad(const char *path, PcPortKromRom **out);
/* NULL is allowed. All borrowed spans expire when their handle is freed. */
void PcPortKromRomFree(PcPortKromRom *rom);
/* rom must be a live successful load. See krom_mapping.h for result semantics.
 * Concurrent reads are allowed; callers must synchronize destruction. */
PcPortKromResult PcPortKromRomResolve(const PcPortKromRom *rom, uint32_t code,
                                    const uint8_t *selector, size_t length,
                                    const uint8_t **span);
/* Native code-only provider: immutable ROM is loaded once from XENO_BIOS,
 * or disc/scph5500.bin when unset. Returned bytes last for process lifetime.
 * BIOS rejection returns (const uint8_t*)(intptr_t)-1, never NULL.
 * Load errors, unknown selector-dependent mappings, and non-ROM spans abort
 * with KROM_UNRESOLVED instead of inventing glyphs or guest stack state.
 * Set the environment before any call; initialization is thread-safe. */
const uint8_t *PcPortKromFont(uint32_t code, size_t length);
#endif
