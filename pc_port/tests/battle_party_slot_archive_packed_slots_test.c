/* LP64 packed-slot pin for func_800BF2B8 (party-slot archive load).
 *
 * SpriteData +0x7C is a 4-byte pointer (func_800239A0 stores pData+0xF4
 * there). +0x80/+0x82 are neighbouring s16 fields; the actor spawner writes
 * scale 0x2000 at +0x82. A host `*(u32**)` load concatenates those words
 * into a non-canonical pointer, so func_800BA4E0's party-slot cleanup
 * (index 0..2) crashes on the file-id deref.
 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "common.h"

extern void func_800BF2B8(u8* task);

uint8_t g_PsxRam[PSX_RAM_SIZE];
uint8_t g_PsxScratchpad[4096];

static unsigned s_release_calls;
static u32 s_archive_index_a0;
static u32 s_archive_index_a1;
static unsigned s_archive_index_calls;
static u32 s_decode_file;
static unsigned s_decode_calls;
static u32 s_read_file;
static u8* s_read_buf;
static u32 s_read_a2;
static u32 s_read_a3;
static unsigned s_read_calls;
static u32 s_alloc_size;
static u32 s_alloc_flags;
static unsigned s_alloc_calls;
static unsigned char s_heap[0x100];
static u32 s_file_id_word;

void func_800B8D7C(u8* task)
{
    (void)task;
    s_release_calls++;
}

void ArchiveSetIndex(u32 a0, u32 a1)
{
    s_archive_index_calls++;
    s_archive_index_a0 = a0;
    s_archive_index_a1 = a1;
}

u32 ArchiveDecodeAlignedSize(u32 file)
{
    s_decode_calls++;
    s_decode_file = file;
    return 0x80;
}

void ArchiveReadFileToBuffer(u32 file, u8* buf, u32 a2, u32 a3)
{
    s_read_calls++;
    s_read_file = file;
    s_read_buf = buf;
    s_read_a2 = a2;
    s_read_a3 = a3;
}

void* HeapAlloc(u32 size, u32 flags)
{
    s_alloc_calls++;
    s_alloc_size = size;
    s_alloc_flags = flags;
    return s_heap;
}

int main(void)
{
    unsigned char sprite[0xB0];
    const u32 fileId = 0x0000A11Eu;
    const u16 scaleCanary = 0x2000;
    const u16 padCanary = 0xA5A5;
    const u32 flagsA8 = 0xC0000005u;
    const u32 flagsAC = 0x00000002u;
    u32 packedPtr;

    if (sizeof(void*) != 8) {
        fprintf(stderr, "ASSERTION party.archive host is not LP64\n");
        return 1;
    }

    memset(g_PsxRam, 0, sizeof(g_PsxRam));
    memset(sprite, 0x5A, sizeof(sprite));
    s_file_id_word = fileId;
    packedPtr = (u32)(uintptr_t)&s_file_id_word;
    *(u32*)(sprite + 0x7C) = packedPtr;
    *(u16*)(sprite + 0x80) = padCanary;
    *(u16*)(sprite + 0x82) = scaleCanary;
    *(u32*)(sprite + 0xA8) = flagsA8;
    *(u32*)(sprite + 0xAC) = flagsAC;

    s_release_calls = 0;
    s_archive_index_calls = 0;
    s_decode_calls = 0;
    s_read_calls = 0;
    s_alloc_calls = 0;

    func_800BF2B8(sprite);

    if (s_release_calls != 1) {
        fprintf(stderr, "ASSERTION party.archive release once\n");
        return 1;
    }
    if (s_archive_index_calls != 1 || s_archive_index_a0 != 0x2C ||
        s_archive_index_a1 != 1) {
        fprintf(stderr, "ASSERTION party.archive ArchiveSetIndex\n");
        return 1;
    }
    if (s_decode_calls != 1 || s_decode_file != fileId) {
        fprintf(stderr,
                "ASSERTION party.archive file id actual=%08x expected=%08x\n",
                s_decode_file, fileId);
        return 1;
    }
    if (s_alloc_calls != 1 || s_alloc_size != 0x80 || s_alloc_flags != 1) {
        fprintf(stderr, "ASSERTION party.archive HeapAlloc\n");
        return 1;
    }
    if (s_read_calls != 1 || s_read_file != fileId || s_read_buf != s_heap ||
        s_read_a2 != 0 || s_read_a3 != 0x80) {
        fprintf(stderr, "ASSERTION party.archive ArchiveReadFileToBuffer\n");
        return 1;
    }
    if (*(u32*)PSX_ADDR(0x800C3618) != (u32)(uintptr_t)s_heap) {
        fprintf(stderr, "ASSERTION party.archive D_800C3618\n");
        return 1;
    }
    if (*(u32*)PSX_ADDR(0x800C361C) !=
        (((flagsAC & 3) << 2) | (flagsA8 >> 30))) {
        fprintf(stderr, "ASSERTION party.archive D_800C361C\n");
        return 1;
    }
    if (*(u32*)(sprite + 0x7C) != packedPtr) {
        fprintf(stderr, "ASSERTION party.archive +0x7C clobbered\n");
        return 1;
    }
    if (*(u16*)(sprite + 0x80) != padCanary) {
        fprintf(stderr, "ASSERTION party.archive +0x80 canary\n");
        return 1;
    }
    if (*(u16*)(sprite + 0x82) != scaleCanary) {
        fprintf(stderr, "ASSERTION party.archive +0x82 scale canary\n");
        return 1;
    }

    puts("PARTY SLOT ARCHIVE packed +0x7C slot PASS");
    return 0;
}
