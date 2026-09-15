/*
 * Layout proof: FieldTextBox +0xA8 is a 4-byte PSX string-pointer slot.
 *
 * func_8007E114 writes the on-screen RECT as s16 x/y/w/h at +0xAC.
 * func_8007F8DC then stores the dialog string pointer at +0xA8.
 * On LP64, `*(void**)(p + 0xA8) = ptr` is an 8-byte write and zeros x/y,
 * so every opened field text box renders at (0, 0).
 *
 * Build/run:
 *   gcc -m64 -O0 -Wall -Werror -o /tmp/test_textbox_string_ptr_width \
 *       pc_port/tests/test_textbox_string_ptr_width.c \
 *       && /tmp/test_textbox_string_ptr_width
 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static int g_failures;

static void expect(int cond, const char* msg) {
    if (!cond) {
        printf("FAIL: %s\n", msg);
        g_failures++;
    } else {
        printf("ok:   %s\n", msg);
    }
}

static void test_string_ptr_store_clobber(void) {
    unsigned char box[0xC0];
    void* fake = (void*)(uintptr_t)0x0000000012345678ull;

    memset(box, 0, sizeof(box));
    *(int16_t*)(box + 0xAC) = 0x00A0; /* planted x */
    *(int16_t*)(box + 0xAE) = 0x0014; /* planted y */
    *(int16_t*)(box + 0xB0) = 0x0080; /* planted w */
    *(int16_t*)(box + 0xB2) = 0x0040; /* planted h */

    *(void**)(box + 0xA8) = fake;
    expect(*(int16_t*)(box + 0xAC) == 0 && *(int16_t*)(box + 0xAE) == 0,
           "8-byte store at +0xA8 zeros planted text-box x/y at +0xAC");
    expect(*(int16_t*)(box + 0xB0) == 0x0080 && *(int16_t*)(box + 0xB2) == 0x0040,
           "8-byte store at +0xA8 does not reach w/h at +0xB0");

    memset(box, 0, sizeof(box));
    *(int16_t*)(box + 0xAC) = 0x00A0;
    *(int16_t*)(box + 0xAE) = 0x0014;
    *(int16_t*)(box + 0xB0) = 0x0080;
    *(int16_t*)(box + 0xB2) = 0x0040;

    *(uint32_t*)(box + 0xA8) = (uint32_t)(uintptr_t)fake;
    expect(*(int16_t*)(box + 0xAC) == 0x00A0 && *(int16_t*)(box + 0xAE) == 0x0014,
           "4-byte u32 store at +0xA8 preserves text-box x/y");
    expect(*(int16_t*)(box + 0xB0) == 0x0080 && *(int16_t*)(box + 0xB2) == 0x0040,
           "4-byte u32 store at +0xA8 preserves text-box w/h");
    expect(*(uint32_t*)(box + 0xA8) == 0x12345678u,
           "4-byte store keeps the truncated host pointer");
}

int main(void) {
    expect(sizeof(void*) == 8, "LP64 host");
    test_string_ptr_store_clobber();

    if (g_failures) {
        printf("%d failure(s)\n", g_failures);
        return 1;
    }
    printf("all layout proofs passed\n");
    return 0;
}
