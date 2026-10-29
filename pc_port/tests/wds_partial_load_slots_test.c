/*
 * LP64 proofs for func_800380D0 (field streamed WDS registration).
 *
 * Retail stores SoundWDSEntry.spuMemoryAddress / pNext with sw (4 bytes).
 * SoundLoadWdsFile already walks the list as SoundPsxAddress. The port arm
 * of func_800380D0 used void** at +0x28 (clobbers pNext) and walked
 * pNext as an 8-byte host pointer (concatenates the following header word).
 *
 * Trigger: boot/common WDS is already on g_SoundWdsLinkedList; a field with
 * its own bank finishes the first four 2KB stream chunks and func_800859DC
 * calls func_800380D0. The 8-byte load of the existing pNext (0) plus the
 * next header word is non-NULL, and the walk leaves the list.
 *
 * Build/run via pc_port/tests/run_wds_partial_load_slots_test.sh
 */
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "system/sound.h"

static int g_failures;

static void expect(int cond, const char* msg)
{
    if (!cond) {
        printf("FAIL: %s\n", msg);
        g_failures++;
    } else {
        printf("ok:   %s\n", msg);
    }
}

/* ---- layout: 8-byte store at +0x28 clobbers pNext at +0x2C ---- */

static void test_spu_slot_store_clobber(void)
{
    unsigned char block[0x40];
    void* fake = (void*)(uintptr_t)0x0000000010102030ull;

    memset(block, 0, sizeof(block));
    *(uint32_t*)(block + 0x2C) = 0xAABBCCDDu;
    *(void**)(block + 0x28) = fake;
    expect(*(uint32_t*)(block + 0x2C) == 0,
           "8-byte store at +0x28 zeros planted pNext at +0x2C");

    memset(block, 0, sizeof(block));
    *(uint32_t*)(block + 0x2C) = 0xAABBCCDDu;
    *(uint32_t*)(block + 0x28) = (uint32_t)(uintptr_t)fake;
    expect(*(uint32_t*)(block + 0x2C) == 0xAABBCCDDu,
           "4-byte u32 store at +0x28 preserves pNext at +0x2C");
}

/* ---- walk: 8-byte load of pNext concatenates the following header word ---- */

static void test_pnext_walk_concat(void)
{
    unsigned char entry[0x40];
    void* as_ptr;
    uint32_t as_u32;

    memset(entry, 0, sizeof(entry));
    *(uint32_t*)(entry + 0x2C) = 0;          /* pNext = NULL */
    *(uint32_t*)(entry + 0x30) = 0x11223344u; /* first payload word */

    memcpy(&as_ptr, entry + 0x2C, sizeof(as_ptr));
    expect((uintptr_t)as_ptr == 0x1122334400000000ull,
           "8-byte load of packed pNext concatenates the following header word");
    expect(as_ptr != NULL,
           "concatenated pNext looks non-NULL so a void** walk continues");

    as_u32 = *(uint32_t*)(entry + 0x2C);
    expect(as_u32 == 0, "4-byte load of pNext is the real terminator");
}

/* ---- named struct keeps retail offsets on LP64 ---- */

static void test_wds_entry_layout(void)
{
    expect(sizeof(SoundWDSEntry) == 0x30, "SoundWDSEntry stays 0x30");
    expect(offsetof(SoundWDSEntry, spuMemoryAddress) == 0x28,
           "spuMemoryAddress at retail +0x28");
    expect(offsetof(SoundWDSEntry, pNext) == 0x2C, "pNext at retail +0x2C");
}

int main(void)
{
    expect(sizeof(void*) == 8, "LP64 host");
    test_wds_entry_layout();
    test_spu_slot_store_clobber();
    test_pnext_walk_concat();

    if (g_failures) {
        printf("%d failure(s)\n", g_failures);
        return 1;
    }
    printf("all WDS partial-load slot proofs passed\n");
    return 0;
}
