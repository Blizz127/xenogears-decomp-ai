/*
 * LP64 proofs for func_800BAEB8 (battle-start party pose pass).
 *
 * D_800C3EB0 + 0x8C8C is an 11-entry table of packed 4-byte actor pointers.
 * The old port body indexed it as native u8** (8-byte load, stride 8).
 *
 * Trigger: func_800BFE48 walks active slots at battle start and calls
 * func_800BAEB8(i). Slot 0's 8-byte load concatenates the slot-1 word, so
 * the +0xAF state read and the func_800223B0/func_80021FE0 stores land on
 * a wild pointer.
 *
 * Build/run via pc_port/tests/run_battle_baeb8_slot_table_test.sh
 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"

uint8_t g_PsxRam[PSX_RAM_SIZE];
uint8_t g_PsxScratchpad[0x1000];

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

static void test_table_stride_concat(void)
{
    unsigned char table[16];
    void* as_ptr;
    uint32_t as_u32;

    memset(table, 0, sizeof(table));
    *(uint32_t*)(table + 0) = 0x800D0000u; /* slot 0 */
    *(uint32_t*)(table + 4) = 0x800D1000u; /* slot 1 */

    memcpy(&as_ptr, table, sizeof(as_ptr));
    expect((uintptr_t)as_ptr == 0x800D1000800D0000ull,
           "8-byte load of packed slot 0 concatenates slot 1");
    expect(as_ptr != (void*)(uintptr_t)0x800D0000u,
           "concatenated slot 0 is not the real actor pointer");

    as_u32 = *(uint32_t*)table;
    expect(as_u32 == 0x800D0000u, "4-byte load of slot 0 is the real pointer");

    memcpy(&as_ptr, table + 8, sizeof(as_ptr));
    expect((uintptr_t)as_ptr == 0,
           "u8** index 1 reads 8 bytes at +8 (slots 2/3), not slot 1");
    as_u32 = *(uint32_t*)(table + 4);
    expect(as_u32 == 0x800D1000u, "4-byte load of slot 1 is the real pointer");
}

int main(void)
{
    expect(sizeof(void*) == 8, "LP64 host");
    test_table_stride_concat();

    if (g_failures) {
        printf("%d failure(s)\n", g_failures);
        return 1;
    }
    printf("all func_800BAEB8 slot-table layout proofs passed\n");
    return 0;
}
