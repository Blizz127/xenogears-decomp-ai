/*
 * World-map helpers 0x800966CC and 0x8009699C — file I/O and CD operations.
 */
#include <stdint.h>
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_966cc.h"

/* PsyQ file I/O (from PsyCross libsn.h) */
extern uintptr_t PCopen(char* name, int flags, int perms);
extern int PClseek(uintptr_t fd, int offset, int mode);
extern int PCread(uintptr_t fd, char* buff, int len);
extern int PCclose(uintptr_t fd);

/* PsyQ CD functions (from PsyCross) */
extern void CdSyncCallback(void (*func)(u8, u8*));
extern int CdControlF(u8 com, u8* param);
extern void CdIntToPos(int i, void* p);

#define D_8009BE48  0x8009BE48u
#define D_8009CCB0  0x8009CCB0u
#define D_8009CCA8  0x8009CCA8u
#define D_8009CCA0  0x8009CCA0u

static u32 f_lw(u32 a) { u32 v; memcpy(&v, PSX_ADDR(a), 4); return v; }
static void f_sw(u32 a, u32 v) { memcpy(PSX_ADDR(a), &v, 4); }
static u16 f_lhu(u32 a) { u16 v; memcpy(&v, PSX_ADDR(a), 2); return v; }

void wm_800966CC(u32 file_table)
{
    s32 retry;
    u32 table = file_table;

    /* Clear status globals */
    f_sw(D_8009BE48, 0);
    f_sw(D_8009CCB0, 0);
    f_sw(D_8009CCA8, 0);
    f_sw(D_8009CCA0, 0);

    /* Iterate over file table entries */
    while (f_lw(table) != 0) {
        u32 filename = f_lw(table);
        u32 offset = f_lw(table + 8);
        u32 buffer = f_lw(table + 4);
        u32 size = f_lw(table + 12);
        u32 fd;

        /* Open file with retries */
        for (retry = 0; retry < 8; retry++) {
            fd = PCopen((char*)PSX_ADDR(filename), 0, 0);
            if ((s32)fd != -1) break;
        }
        if ((s32)fd == -1) {
            table += 0x10;
            continue;
        }

        /* Seek to offset */
        PClseek(fd, (int)offset, 0);

        /* Read data with retries */
        for (retry = 0; retry < 8; retry++) {
            int result = PCread(fd, (char*)PSX_ADDR(buffer), (int)size);
            if (result != 0) break;
        }

        /* Close file with retries */
        for (retry = 0; retry < 8; retry++) {
            int result = PCclose(fd);
            if (result == 0) break;
        }

        table += 0x10;
    }
}

void wm_8009699C(u32 addr, u32 val1, u32 val2)
{
    /* Stub: CD sector loading via CdIntToPos/CdControlF/CdSyncCallback */
    /* Full implementation requires CD subsystem integration */
    (void)addr;
    (void)val1;
    (void)val2;
}
