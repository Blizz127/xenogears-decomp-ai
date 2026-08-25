#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "test_input.h"

static int s_failures;

static void check(int condition, const char *name)
{
    if (!condition) {
        fprintf(stderr, "ASSERTION %s\n", name);
        s_failures++;
    }
}

static void reset_with(const char *schedule)
{
    PcPort_TestInputResetForCertificate();
    if (schedule == NULL)
        unsetenv("XENO_TEST_INPUT");
    else
        setenv("XENO_TEST_INPUT", schedule, 1);
}

static void expect_invalid(const char *schedule, const char *name)
{
    reset_with(schedule);
    check(PcPort_TestInputInit() != 0, name);
}

int main(void)
{
    static const u16 expected[] = {
        0x2000u, 0x2000u, 0x2000u, 0x4000u, 0x4000u, 0u, 0u
    };
    u16 destination;
    size_t i;

    reset_with(NULL);
    destination = 0xA55Au;
    check(PcPort_TestInputInit() == 0, "unset.init.success");
    PcPort_TestInputAdvanceFrame();
    PcPort_TestInputInject(&destination);
    PcPort_TestInputAdvanceFrame();
    PcPort_TestInputInject(&destination);
    check(destination == 0xA55Au, "unset.injection.inert");

    expect_invalid("", "malformed.empty");
    expect_invalid("1:0", "malformed.first_boundary");
    expect_invalid("0", "malformed.missing_colon");
    expect_invalid("0:0x10000", "malformed.value_range");
    expect_invalid("0:1,", "malformed.trailing_comma");
    expect_invalid("0:1,0:2", "malformed.nonincreasing");
    expect_invalid("0:1x", "malformed.trailing_junk");

    reset_with("0:0x2000,3:0x4000,5:0");
    check(PcPort_TestInputInit() == 0, "valid.init.success");
    for (i = 0u; i < sizeof(expected) / sizeof(expected[0]); i++) {
        destination = 0xFFFFu;
        PcPort_TestInputAdvanceFrame();
        PcPort_TestInputInject(&destination);
        check(destination == expected[i],
              i == 0u ? "boundary.frame0" :
              i == 1u ? "hold.frame1" :
              i == 2u ? "hold.frame2" :
              i == 3u ? "boundary.frame3" :
              i == 4u ? "hold.frame4" :
              i == 5u ? "boundary.frame5" : "hold.frame6");
    }

    reset_with("0:0");
    destination = 0xFFFFu;
    check(PcPort_TestInputInit() == 0, "zero_schedule.init.success");
    PcPort_TestInputAdvanceFrame();
    PcPort_TestInputInject(&destination);
    check(destination == 0u, "set.schedule.injects.zero");

    if (s_failures != 0) {
        fprintf(stderr, "%d assertion(s) failed\n", s_failures);
        return 1;
    }
    puts("W34C1 scripted input certificate PASS");
    return 0;
}
