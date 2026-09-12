/*
 * Regression: func_8002C8CC must store the packet buffer in a 4-byte slot
 * at modelData+0x18. An 8-byte host pointer write zeros +0x1C, which
 * func_800303C8 later reads as the skeletal descriptor.
 *
 * Build/run (no game/PsyCross needed):
 *   gcc -m64 -O0 -std=c11 -o /tmp/test_model_header_store_width \
 *       pc_port/tests/test_model_header_store_width.c
 *   /tmp/test_model_header_store_width
 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define SKELETAL_MAGIC 0x12345678u
#define PACKET_PTR     0x2000ABCDu

static void store_as_void_ptr(uint8_t* model, uint32_t packet)
{
    *(void**)(model + 0x18) = (void*)(uintptr_t)packet;
}

static void store_as_u32(uint8_t* model, uint32_t packet)
{
    *(uint32_t*)(model + 0x18) = packet;
}

static int check_slots(const char* label, uint8_t* model, int expect_clobber)
{
    uint32_t slot18 = *(uint32_t*)(model + 0x18);
    uint32_t slot1c = *(uint32_t*)(model + 0x1C);
    int failed = 0;

    printf("%s: +0x18=0x%08x +0x1C=0x%08x\n", label, slot18, slot1c);

    if (slot18 != PACKET_PTR) {
        printf("  FAIL: +0x18 lost packet pointer (want 0x%08x)\n", PACKET_PTR);
        failed = 1;
    }
    if (expect_clobber) {
        if (slot1c == SKELETAL_MAGIC) {
            printf("  FAIL: expected 8-byte store to clobber +0x1C\n");
            failed = 1;
        } else {
            printf("  ok: 8-byte store clobbered skeletal slot (now 0x%08x)\n", slot1c);
        }
    } else if (slot1c != SKELETAL_MAGIC) {
        printf("  FAIL: 4-byte store clobbered +0x1C (want 0x%08x)\n", SKELETAL_MAGIC);
        failed = 1;
    } else {
        printf("  ok: 4-byte store preserved skeletal descriptor\n");
    }
    return failed;
}

int main(void)
{
    uint8_t header[0x40];
    int failed = 0;

    if (sizeof(void*) != 8) {
        fprintf(stderr, "this test must run LP64 (sizeof(void*)=%zu)\n", sizeof(void*));
        return 2;
    }

    memset(header, 0, sizeof(header));
    *(uint32_t*)(header + 0x1C) = SKELETAL_MAGIC;
    store_as_void_ptr(header, PACKET_PTR);
    failed |= check_slots("void* store (buggy LP64)", header, 1);

    memset(header, 0, sizeof(header));
    *(uint32_t*)(header + 0x1C) = SKELETAL_MAGIC;
    store_as_u32(header, PACKET_PTR);
    failed |= check_slots("u32 store (fixed)", header, 0);

    if (failed) {
        printf("RESULT: FAIL\n");
        return 1;
    }
    printf("RESULT: PASS\n");
    return 0;
}
