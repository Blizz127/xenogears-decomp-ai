/*
 * World-map helper 0x80098CC0 (asset loader with queue management).
 */
#include <stdint.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_98cc0.h"
#include "world_map_helper_9623c.h"

/* PsyQ/system functions */
extern void* HeapAlloc(u32 size, u32 flags);
extern void HeapFree(void* ptr);
extern void* ArchiveGetFilePath(u32 archive_id);
extern void* ArchiveDecodeSector(void* sector);

/* Globals */
#define D_8009C184  0x8009C184u  /* slot table (0x51 entries, u32 each) */
#define D_8009D570  0x8009D570u  /* index table (0x51 entries, s16 each) */
#define D_8009D318  0x8009D318u  /* data table */
#define D_8009BCD8  0x8009BCD8u  /* archive base pointer */
#define D_8009BD08  0x8009BD08u  /* secondary table pointer */
#define D_8009D808  0x8009D808u  /* queue counter */
#define SLOT_COUNT  0x51
#define ASSET_SIZE  0x710

static u32 a98_lw(u32 a) { u32 v; memcpy(&v, PSX_ADDR(a), 4); return v; }
static void a98_sw(u32 a, u32 v) { memcpy(PSX_ADDR(a), &v, 4); }
static s16 a98_lh(u32 a) { s16 v; memcpy(&v, PSX_ADDR(a), 2); return v; }
static void a98_sh(u32 a, u16 v) { memcpy(PSX_ADDR(a), &v, 2); }

void wm_80098CC0(void)
{
    s32 i, j;
    u32 queue_counter_snapshot;

    /* Phase 1: Free unused slots */
    for (i = 0; i < SLOT_COUNT; i++) {
        s16 slot_id = a98_lh(D_8009D570 + i * 2);
        u32 slot_ptr = a98_lw(D_8009C184 + (u32)slot_id * 4);

        if (slot_ptr != 0) {
            /* Check if slot_id is in the active list */
            s32 found = 0;
            for (j = 0; j < SLOT_COUNT; j++) {
                if (a98_lh(D_8009D570 + j * 2) == slot_id) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                /* Not in active list — free it */
                /* W34C5: slots hold guest addresses. */
                HeapFree(PSX_ADDR(a98_lw(D_8009C184 + (u32)slot_id * 4)));
                a98_sw(D_8009C184 + (u32)slot_id * 4, 0);
            }
        }
    }

    /* Phase 2: Load new assets from archive */
    queue_counter_snapshot = a98_lw(D_8009D808);

    /* Decode archive sector */
    {
        u32 archive_base = a98_lw(D_8009BCD8);
        void* decoded = ArchiveDecodeSector((void*)(uintptr_t)archive_base);
        (void)decoded;
    }

    /* Phase 3: Allocate and queue new assets */
    for (i = 0; i < 2; i++) {
        for (j = 1; j < 7; j++) {
            s16 slot_id = a98_lh(D_8009D570 + (i * 6 + j) * 2);
            u32 existing = a98_lw(D_8009C184 + (u32)slot_id * 4);

            if (existing == 0) {
                /* Allocate new buffer */
                void* buf = HeapAlloc(ASSET_SIZE, 0);
                a98_sw(D_8009C184 + (u32)slot_id * 4, PsxMemory_GuestAddr(buf));

                /* Queue data copy */
                u32 src = a98_lw(D_8009BD08) + (u32)slot_id;
                wm_8009623C(src, ASSET_SIZE, PsxMemory_GuestAddr(buf));
            }
        }
    }

    /* Phase 4: Queue remaining assets */
    for (i = 0; i < 0x49; i++) {
        u32 secondary_base = a98_lw(D_8009BD08);
        s16 slot_id = a98_lh(D_8009D570 + (0x49 + i) * 2);
        u32 existing = a98_lw(D_8009C184 + (u32)slot_id * 4);

        if (existing == 0) {
            void* buf = HeapAlloc(ASSET_SIZE, 0);
            a98_sw(D_8009C184 + (u32)slot_id * 4, PsxMemory_GuestAddr(buf));
            wm_8009623C(secondary_base + (u32)slot_id, ASSET_SIZE,
                        PsxMemory_GuestAddr(buf));
        }
    }
}
