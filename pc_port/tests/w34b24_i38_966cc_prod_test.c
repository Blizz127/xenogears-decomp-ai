/*
 * Focused production-linked oracle for retail helper 0x800966CC.
 *
 * Expected values are hand-derived from [0x800966CC, 0x800967E4).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_966cc.h"

#define LIST   0x80090000u
#define NAME   0x80091000u
#define DEST   0x80092000u
#define BE48   0x8009BE48u
#define CCB0   0x8009CCB0u
#define CCA8   0x8009CCA8u
#define CCA0   0x8009CCA0u
#define CANARY 0x8009B000u

static int s_failures;
static int s_open_calls;
static int s_open_flags;
static int s_open_fail;
static int s_seek_calls;
static int s_seek_off;
static int s_read_calls;
static int s_read_dest_ok;
static int s_read_n;
static int s_close_calls;
static int s_close_fd;

static void chk(const char *n, u32 got, u32 want)
{
    if (got != want) {
        fprintf(stderr, "ASSERTION %s: got=0x%08x expected=0x%08x\n", n, got,
                want);
        if (++s_failures > 40)
            exit(1);
    }
}

static u32 rd32(u32 a)
{
    u32 v;

    memcpy(&v, PSX_ADDR(a), 4);
    return v;
}

static void wr32(u32 a, u32 v) { memcpy(PSX_ADDR(a), &v, 4); }

void wm_966cc_test_store(u32 address, u32 value)
{
    (void)address;
    (void)value;
}

int PCopen(char *name, int flags, int perms)
{
    (void)perms;
    s_open_calls += 1;
    s_open_flags = flags;
    if (s_open_fail)
        return -1;
    if (name != (char *)PSX_ADDR(NAME))
        return -1;
    return 7;
}

int PClseek(int fd, int offset, int mode)
{
    (void)fd;
    (void)mode;
    s_seek_calls += 1;
    s_seek_off = offset;
    return 0;
}

int PCread(int fd, char *buff, int n)
{
    (void)fd;
    s_read_calls += 1;
    s_read_dest_ok = (buff == (char *)PSX_ADDR(DEST));
    s_read_n = n;
    if (buff != NULL && n > 0)
        buff[0] = 'Z';
    return n;
}

int PCclose(int fd)
{
    s_close_calls += 1;
    s_close_fd = fd;
    return 0;
}

static void rec(u32 i, u32 name, u32 off, u32 count, u32 dest)
{
    wr32(LIST + i * 16u, name);
    wr32(LIST + i * 16u + 4u, off);
    wr32(LIST + i * 16u + 8u, count);
    wr32(LIST + i * 16u + 12u, dest);
}

int main(void)
{
    PsxMemory_Init();
    wr32(CANARY, 0x11111111u);
    memcpy(PSX_ADDR(NAME), "foo", 4);

    wr32(BE48, 0xAAu);
    wr32(CCB0, 0xBBu);
    wr32(CCA8, 0xCCu);
    wr32(CCA0, 0xDDu);
    rec(0, 0u, 0u, 0u, 0u);
    wm_800966CC(LIST);
    chk("empty.be48", rd32(BE48), 0u);
    chk("empty.ccb0", rd32(CCB0), 0u);
    chk("empty.cca8", rd32(CCA8), 0u);
    chk("empty.cca0", rd32(CCA0), 0u);
    chk("empty.open", (u32)s_open_calls, 0u);

    s_open_fail = 1;
    s_open_calls = 0;
    rec(0, NAME, 0x10u, 4u, DEST);
    rec(1, 0u, 0u, 0u, 0u);
    wm_800966CC(LIST);
    chk("fail.open", (u32)s_open_calls, 8u);
    chk("fail.seek", (u32)s_seek_calls, 0u);
    chk("fail.read", (u32)s_read_calls, 0u);
    chk("fail.close", (u32)s_close_calls, 0u);

    s_open_fail = 0;
    s_open_calls = 0;
    s_seek_calls = 0;
    s_read_calls = 0;
    s_close_calls = 0;
    wr32(DEST, 0u);
    rec(0, NAME, 0x20u, 4u, DEST);
    rec(1, 0u, 0u, 0u, 0u);
    wm_800966CC(LIST);
    chk("ok.open", (u32)s_open_calls, 1u);
    chk("ok.flags", (u32)s_open_flags, 0u);
    chk("ok.seek", (u32)s_seek_calls, 1u);
    chk("ok.off", (u32)s_seek_off, 0x20u);
    chk("ok.read", (u32)s_read_calls, 1u);
    chk("ok.dest", (u32)s_read_dest_ok, 1u);
    chk("ok.n", (u32)s_read_n, 4u);
    chk("ok.close", (u32)s_close_calls, 1u);
    chk("ok.fd", (u32)s_close_fd, 7u);
    chk("ok.mark", (u32)((u8 *)PSX_ADDR(DEST))[0], (u32)'Z');
    chk("canary", rd32(CANARY), 0x11111111u);

    if (s_failures) {
        fprintf(stderr, "ASSERTION failures=%d\n", s_failures);
        return 1;
    }
    printf("W34B24-I38 0x800966CC focused oracle PASS\n");
    return 0;
}
