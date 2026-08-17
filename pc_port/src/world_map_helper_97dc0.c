/*
 * World-map helper 0x80097DC0 (asset loader B).
 */
#include <stdint.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_97dc0.h"
#include "world_map_helper_9623c.h"

extern u32 func_8002C3D8(void);
extern void* HeapAlloc(u32 size, u32 flags);
extern void* ArchiveDecodeSector(void* sector);

#define D_8009BCD8  0x8009BCD8u  /* archive base */
#define D_8009D570  0x8009D570u  /* index table */
#define D_8009C184  0x8009C184u  /* slot table */
#define D_8009D808  0x8009D808u  /* queue counter */
#define ASSET_SIZE  0x710

static u32 a97_lw(u32 a) { u32 v; memcpy(&v, PSX_ADDR(a), 4); return v; }
static void a97_sw(u32 a, u32 v) { memcpy(PSX_ADDR(a), &v, 4); }
static s16 a97_lh(u32 a) { s16 v; memcpy(&v, PSX_ADDR(a), 2); return v; }

void wm_80097DC0(void)
{
    s32 i, j;
    u32 r1 = func_8002C3D8();
    u32 r2 = func_8002C3D8();
    u32 ready = ((r1 < 1) | (~r2 < 1));

    if (ready) {
        u32 archive_base = a97_lw(D_8009BCD8);
        u32 decoded = (u32)(uintptr_t)ArchiveDecodeSector(
            (void*)(uintptr_t)archive_base);

        /* Process entries in nested loops */
        for (i = 3; i < 6; i++) {
            for (j = 3; j < 6; j++) {
                s16 slot_id = a97_lh(D_8009D570 + (i + j) * 2);
                u32 slot_ptr = a97_lw(D_8009C184 + (u32)slot_id * 4);

                if (slot_ptr == 0) {
                    void* buf = HeapAlloc(ASSET_SIZE, 0);
                    a97_sw(D_8009C184 + (u32)slot_id * 4,
                           (u32)(uintptr_t)buf);
                    wm_8009623C(decoded + (u32)slot_id,
                                ASSET_SIZE,
                                (u32)(uintptr_t)buf);
                }
            }
        }

        /* Queue zero entry */
        wm_8009623C(0, 0, 0);
        wm_80096328();

        /* Second pass */
        for (i = 0; i < 3; i++) {
            for (j = 0; j < 6; j++) {
                s16 slot_id = a97_lh(D_8009D570 + (i + j) * 2);
                u32 slot_ptr = a97_lw(D_8009C184 + (u32)slot_id * 4);

                if (slot_ptr == 0) {
                    void* buf = HeapAlloc(ASSET_SIZE, 0);
                    a97_sw(D_8009C184 + (u32)slot_id * 4,
                           (u32)(uintptr_t)buf);
                    wm_8009623C(decoded + (u32)slot_id,
                                ASSET_SIZE,
                                (u32)(uintptr_t)buf);
                }
            }
        }
    }
}
