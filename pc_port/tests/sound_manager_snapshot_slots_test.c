/* LP64 packed-slot pin for func_8003ADCC (audio-manager snapshot, mode 1).
 *
 * AudioManager.unk_Manager_0x4 is a 4-byte SoundPsxAddress at +0x4; the seq
 * file pointer lives in the next word at +0x8. A host-width `*(void**)` store
 * at +0x4 clobbers +0x8, so looping BGM restart (mode 2 on the tick path)
 * later restores from a snapshot whose seq pointer was already smashed, and
 * live playback follows the clobbered word.
 */
#include "common.h"
#include "system/sound.h"

#include <stdio.h>
#include <string.h>

extern void func_8003ADCC(void* pManager);

u_long g_unk_SoundEvent;

static unsigned char s_backup[0x400];
static void* s_alloc_ret;
static u32 s_alloc_size;
static unsigned s_alloc_calls;
static void* s_copy_dst;
static void* s_copy_src;
static int s_copy_size;
static unsigned s_copy_calls;

long DisableEvent(long event)
{
    (void)event;
    return 0;
}

long EnableEvent(long event)
{
    (void)event;
    return 0;
}

void* SoundHeapAllocate(u32 allocSize)
{
    s_alloc_calls++;
    s_alloc_size = allocSize;
    return s_alloc_ret;
}

void SoundHeapSetBlockMemory(void* pBlockMemory, void* pSrc, int size)
{
    s_copy_calls++;
    s_copy_dst = pBlockMemory;
    s_copy_src = pSrc;
    s_copy_size = size;
    memcpy(pBlockMemory, pSrc, (size_t)size);
    return;
}

int main(void)
{
    AudioManager manager;
    const u32 seqCanary = 0x12345678u;
    const u32 percCanary = 0x9ABCDEF0u;
    const u32 flagsCanary = 0x0001;
    const u32 backupAddr = (u32)(unsigned long)s_backup;
    s32 expectSize;

    if ((u8*)&manager.unk_Manager_0x4 - (u8*)&manager != 4) {
        fprintf(stderr, "ASSERTION sound.snapshot unk_Manager_0x4 offset\n");
        return 1;
    }
    if ((u8*)&manager.unk_0x8 - (u8*)&manager != 8) {
        fprintf(stderr, "ASSERTION sound.snapshot unk_0x8 offset\n");
        return 1;
    }
    if ((u8*)&manager.unk_0xc - (u8*)&manager != 0xC) {
        fprintf(stderr, "ASSERTION sound.snapshot unk_0xc offset\n");
        return 1;
    }
    if (sizeof(manager.unk_Manager_0x4) != 4) {
        fprintf(stderr, "ASSERTION sound.snapshot unk_Manager_0x4 width\n");
        return 1;
    }

    memset(&manager, 0, sizeof(manager));
    memset(s_backup, 0xA5, sizeof(s_backup));
    manager.elementCount = 1;
    manager.unk_0x8 = (s32)seqCanary;
    manager.unk_0xc = (s32)percCanary;
    manager.unk_Flags = (s16)flagsCanary;
    manager.unk_0x2c = 0x20;
    s_alloc_ret = s_backup;
    s_alloc_calls = 0;
    s_copy_calls = 0;

    func_8003ADCC(&manager);

    expectSize = SoundCalculateAudioManagerSize(1);
    if (s_alloc_calls != 1) {
        fprintf(stderr, "ASSERTION sound.snapshot allocate once\n");
        return 1;
    }
    if (s_copy_calls != 1) {
        fprintf(stderr, "ASSERTION sound.snapshot copy once\n");
        return 1;
    }
    if (s_alloc_size != (u32)expectSize) {
        fprintf(stderr, "ASSERTION sound.snapshot allocate size\n");
        return 1;
    }
    if (s_copy_dst != s_backup || s_copy_src != &manager || s_copy_size != expectSize) {
        fprintf(stderr, "ASSERTION sound.snapshot copy args\n");
        return 1;
    }

#define CHECK_U32(field, actual, expected) \
    do { \
        u32 _a = (u32)(actual); \
        u32 _e = (u32)(expected); \
        if (_a != _e) { \
            fprintf(stderr, \
                    "ASSERTION sound.snapshot field=%s actual=%08x expected=%08x\n", \
                    (field), _a, _e); \
            return 1; \
        } \
    } while (0)

    CHECK_U32("unk_0x8", manager.unk_0x8, seqCanary);
    CHECK_U32("unk_0xc", manager.unk_0xc, percCanary);
    CHECK_U32("unk_Manager_0x4", manager.unk_Manager_0x4, backupAddr);
    CHECK_U32("unk_Flags", (u16)manager.unk_Flags, flagsCanary | 0x10);
    CHECK_U32("unk_0x2c", manager.unk_0x2c, 0);
    CHECK_U32("backup.next", *(u32*)(s_backup + 0x0), 0);
    CHECK_U32("backup.unk_Manager_0x4", *(u32*)(s_backup + 0x4), 0);
    CHECK_U32("backup.unk_0x8", *(u32*)(s_backup + 0x8), seqCanary);

    /* Re-snapshot should reuse the packed 4-byte backup slot, not merge +8. */
    s_alloc_calls = 0;
    s_copy_calls = 0;
    manager.unk_0x2c = 0x40;
    func_8003ADCC(&manager);
    if (s_alloc_calls != 0) {
        fprintf(stderr, "ASSERTION sound.snapshot reuse must not allocate\n");
        return 1;
    }
    if (s_copy_calls != 1) {
        fprintf(stderr, "ASSERTION sound.snapshot reuse copy\n");
        return 1;
    }
    CHECK_U32("reuse unk_0x8", manager.unk_0x8, seqCanary);
    CHECK_U32("reuse unk_Manager_0x4", manager.unk_Manager_0x4, backupAddr);
    CHECK_U32("reuse unk_0x2c", manager.unk_0x2c, 0);

    puts("SOUND MANAGER SNAPSHOT packed +0x4 slot PASS");
    return 0;
}
