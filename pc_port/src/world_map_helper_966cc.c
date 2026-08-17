/*
 * World-map C624 PC-file record pass 0x800966CC.
 *
 * Register-faithful transcription of retail world_map.bin
 * [0x800966CC, 0x800967E4).  See world_map_helper_966cc.h.
 */
#include <string.h>

#include "common.h"
#include "psx_memory.h"
#include "world_map_helper_966cc.h"

int PCopen(char *name, int flags, int perms);
int PClseek(int fd, int offset, int mode);
int PCread(int fd, char *buff, int n);
int PCclose(int fd);

#define W6CC_BE48  0x8009BE48u
#define W6CC_CCB0  0x8009CCB0u
#define W6CC_CCA8  0x8009CCA8u
#define W6CC_CCA0  0x8009CCA0u
#define W6CC_STRIDE 16u

#if defined(WM_966CC_TEST_TRACE)
extern void wm_966cc_test_store(u32 address, u32 value);
#define W6CC_TRACE_STORE(a, v) wm_966cc_test_store((a), (v))
#else
#define W6CC_TRACE_STORE(a, v) ((void)0)
#endif

static u32 w6cc_lw(u32 a)
{
    u32 v;

    memcpy(&v, PSX_ADDR(a), 4);
    return v;
}

static void __attribute__((unused)) w6cc_sw(u32 a, u32 v)
{
    memcpy(PSX_ADDR(a), &v, 4);
    W6CC_TRACE_STORE(a, v);
}

void wm_800966CC(u32 list)
{
    u32 s3 = list;
    u32 s2;
    s32 s0;
    s32 s1;

#if defined(WM_966CC_MUTANT_SKIP_CLEAR)
    (void)0;
#else
    w6cc_sw(W6CC_BE48, 0u);
    w6cc_sw(W6CC_CCB0, 0u);
    w6cc_sw(W6CC_CCA8, 0u);
    w6cc_sw(W6CC_CCA0, 0u);
#endif

#if defined(WM_966CC_MUTANT_SKIP_EMPTY)
    if (0 && w6cc_lw(s3) == 0u)
#else
    if (w6cc_lw(s3) == 0u)
#endif
        return;

    s2 = list + 8u;

    do {
        s0 = 0;
        do {
#if defined(WM_966CC_MUTANT_WRONG_OPEN)
            s1 = PCopen((char *)PSX_ADDR(w6cc_lw(s3)), 1, 0);
#else
            s1 = PCopen((char *)PSX_ADDR(w6cc_lw(s3)), 0, 0);
#endif
            s0 += 1;
            if (s1 != -1)
                break;
        } while (s0 < 8);

        if (s1 == -1) {
            s3 += W6CC_STRIDE;
        } else {
#if defined(WM_966CC_MUTANT_SKIP_SEEK)
            (void)s2;
#else
            (void)PClseek(s1, (int)w6cc_lw(s2 - 4u), 0);
#endif
            s0 = 0;
            do {
#if defined(WM_966CC_MUTANT_SWAP_READ)
                if (PCread(s1, (char *)PSX_ADDR(w6cc_lw(s2)),
                           (int)w6cc_lw(s2 + 4u)) != 0)
#else
                if (PCread(s1, (char *)PSX_ADDR(w6cc_lw(s2 + 4u)),
                           (int)w6cc_lw(s2)) != 0)
#endif
                    break;
                s0 += 1;
            } while (s0 < 8);

            s0 = 0;
            do {
#if defined(WM_966CC_MUTANT_SKIP_CLOSE)
                break;
#else
                if (PCclose(s1) == 0)
                    break;
                s0 += 1;
#endif
            } while (s0 < 8);
            s3 += W6CC_STRIDE;
        }
        s2 += W6CC_STRIDE;
    } while (w6cc_lw(s3) != 0u);
}
