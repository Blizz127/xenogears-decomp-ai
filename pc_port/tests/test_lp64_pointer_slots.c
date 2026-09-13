/*
 * Layout proofs for LP64 pointer-slot clobbers fixed in this PR.
 *
 * Host `void*` / `T*` stores into 4-byte PSX slots (or 8-byte loads of those
 * slots) corrupt the adjacent packed word. These cases are independent of
 * PsyCross; they only need LP64 sizeof(void*)==8.
 *
 * Build/run:
 *   gcc -m64 -O0 -Wall -Werror -o /tmp/test_lp64_pointer_slots \
 *       pc_port/tests/test_lp64_pointer_slots.c && /tmp/test_lp64_pointer_slots
 */
#include <stddef.h>
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

/* ---- SpriteData: void* pBase/pVramData moves red from +0x28 to +0x30 ---- */

typedef struct {
    uint32_t pad0[8]; /* 0x00..0x1F */
    void* pBase;
    void* pVramData;
    uint8_t red, green, blue, prim;
} SpriteDataHostPtrs;

typedef struct {
    uint32_t pad0[8];
    uint32_t pBase;
    uint32_t pVramData;
    uint8_t red, green, blue, prim;
} SpriteDataU32Ptrs;

/* ---- FieldDistortion: 8-byte/8-aligned pointers grow FieldEffects ---- */

typedef struct {
    int16_t isActive, isFinished, duration;
    char _pad[2];
    int32_t v[6];
    int32_t delta[6];
    int16_t unk38, unk3C;
    void* buffer0;
    void* buffer1;
    void* buffer2;
    void* buffer3;
} FieldDistortionHostPtrs;

typedef struct {
    int16_t isActive, isFinished, duration;
    char _pad[2];
    int32_t v[6];
    int32_t delta[6];
    int16_t unk38, unk3C;
    uint32_t buffer0, buffer1, buffer2, buffer3;
} FieldDistortionU32Ptrs;

/* PSX DR_MODE=12, TILE=16 → FieldFade = 0x58 */
typedef struct {
    uint8_t drawModes[2][12];
    uint8_t tiles[2][16];
    int32_t r0, g0, b0, redDelta, greenDelta, blueDelta;
    int16_t semitransparency, isVisible, duration, _pad;
} FieldFadeMock;

typedef struct {
    FieldDistortionHostPtrs distortion;
    FieldFadeMock fades[2];
} FieldEffectsHostPtrs;

typedef struct {
    FieldDistortionU32Ptrs distortion;
    FieldFadeMock fades[2];
} FieldEffectsU32Ptrs;

/* ---- Packed BSS: 8-byte store at +8 clobbers +0xC (D_800AFC68/D_800AFC6C) */

static void test_packed_store_clobber(void) {
    unsigned char block[16];
    void* fake = (void*)(uintptr_t)0x0000000012345678ull;

    memset(block, 0, sizeof(block));
    *(uint32_t*)(block + 0xC) = 0xAABBCCDD; /* planted held-button mask */
    *(void**)(block + 0x8) = fake;
    expect(*(uint32_t*)(block + 0xC) == 0,
           "8-byte store at +0x8 zeros planted mask at +0xC");

    memset(block, 0, sizeof(block));
    *(uint32_t*)(block + 0xC) = 0xAABBCCDD;
    *(uint32_t*)(block + 0x8) = (uint32_t)(uintptr_t)fake;
    expect(*(uint32_t*)(block + 0xC) == 0xAABBCCDD,
           "4-byte u32 store at +0x8 preserves mask at +0xC");
}

/* ---- Packed BSS: 8-byte load of u32* concatenates D_800AFB20[0]+[1] ---- */

static void test_packed_load_concat(void) {
    uint32_t words[2] = { 0x1000u, 0x2000u };
    uint32_t* as_ptr;
    uint32_t as_u32;

    memcpy(&as_ptr, words, sizeof(as_ptr));
    /* little-endian LP64: low word 0x1000, high word 0x2000 */
    expect((uintptr_t)as_ptr == 0x200000001000ull,
           "8-byte load of packed u32* concatenates adjacent words");

    as_u32 = words[0];
    expect(as_u32 == 0x1000u, "4-byte load of D_800AFB20[0] is just the flags base");
}

static void test_sprite_red_offset(void) {
    expect(offsetof(SpriteDataHostPtrs, red) == 0x30,
           "void* pBase/pVramData places red at +0x30");
    expect(offsetof(SpriteDataU32Ptrs, red) == 0x28,
           "u32 pBase/pVramData places red at retail +0x28");
}

static void test_field_effects_size(void) {
    expect(sizeof(FieldFadeMock) == 0x58, "FieldFade with PSX prim tags is 0x58");
    expect(sizeof(FieldEffectsU32Ptrs) == 0xFC,
           "u32 distortion buffers keep FieldEffects at retail 0xFC");
    expect(sizeof(FieldEffectsHostPtrs) > 0xFC,
           "host pointer distortion buffers grow FieldEffects past 0xFC");
    expect(offsetof(FieldEffectsHostPtrs, fades) + 2 * sizeof(FieldFadeMock) > 0xFC,
           "host-pointer fades[1] extends past packed D_800B2174 at +0xFC");
}

int main(void) {
    expect(sizeof(void*) == 8, "LP64 host");
    test_sprite_red_offset();
    test_field_effects_size();
    test_packed_store_clobber();
    test_packed_load_concat();

    if (g_failures) {
        printf("%d failure(s)\n", g_failures);
        return 1;
    }
    printf("all layout proofs passed\n");
    return 0;
}
