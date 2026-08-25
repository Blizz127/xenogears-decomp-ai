#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "test_input.h"

#define TEST_INPUT_MAX_STEPS 128u

typedef struct TestInputStep {
    uint32_t frame;
    uint16_t value;
} TestInputStep;

static TestInputStep s_steps[TEST_INPUT_MAX_STEPS];
static size_t s_step_count;
static size_t s_current_step;
static uint32_t s_frame;
static int s_enabled;
static int s_initialized;

static int test_input_error(const char *reason)
{
    fprintf(stderr, "[test-input] invalid XENO_TEST_INPUT: %s\n", reason);
    return -1;
}

int PcPort_TestInputInit(void)
{
    const char *schedule;
    const char *cursor;
    uint32_t previous_frame = 0u;

    if (s_initialized != 0)
        return 0;
    s_initialized = 1;
    s_frame = UINT32_MAX;
    schedule = getenv("XENO_TEST_INPUT");
    if (schedule == NULL) {
        s_enabled = 0;
        return 0;
    }
    if (*schedule == '\0')
        return test_input_error("empty schedule");

    cursor = schedule;
    while (*cursor != '\0') {
        char *end;
        unsigned long frame;
        unsigned long value;

        if (s_step_count == TEST_INPUT_MAX_STEPS)
            return test_input_error("too many frame/value pairs");
        errno = 0;
        frame = strtoul(cursor, &end, 10);
        if (errno != 0 || end == cursor || *end != ':' || frame > UINT32_MAX)
            return test_input_error("invalid frame boundary");
        cursor = end + 1;
        errno = 0;
        value = strtoul(cursor, &end, 0);
        if (errno != 0 || end == cursor || value > UINT16_MAX)
            return test_input_error("invalid input value");
        if (*end != '\0' && *end != ',')
            return test_input_error("expected comma between pairs");
        if (s_step_count == 0u && frame != 0u)
            return test_input_error("first frame must be zero");
        if (s_step_count != 0u && frame <= previous_frame)
            return test_input_error("frame boundaries must increase");

        s_steps[s_step_count].frame = (uint32_t)frame;
        s_steps[s_step_count].value = (uint16_t)value;
        s_step_count++;
        previous_frame = (uint32_t)frame;
        if (*end == '\0')
            break;
        cursor = end + 1;
        if (*cursor == '\0')
            return test_input_error("trailing comma");
    }

    s_enabled = 1;
    fprintf(stderr, "[test-input] enabled steps=%zu\n", s_step_count);
    return 0;
}

void PcPort_TestInputAdvanceFrame(void)
{
    if (s_enabled != 0) {
        size_t previous_step = s_current_step;

        s_frame++;
        while (s_current_step + 1u < s_step_count &&
               s_frame >= s_steps[s_current_step + 1u].frame)
            s_current_step++;
        if (s_frame == 0u || s_current_step != previous_step)
            fprintf(stderr, "[test-input] frame=%u value=0x%04x\n",
                    s_frame, (unsigned int)s_steps[s_current_step].value);
    }
}

void PcPort_TestInputInject(u16 *held_buttons)
{
    if (s_enabled == 0)
        return;

#if defined(XENO_TEST_INPUT_MUTANT_DROP_HOLD)
    if (s_frame == s_steps[s_current_step].frame)
        *held_buttons = s_steps[s_current_step].value;
    else
        *held_buttons = 0u;
#else
    *held_buttons = s_steps[s_current_step].value;
#endif
}

#if defined(XENO_TEST_INPUT_CERTIFICATE)
void PcPort_TestInputResetForCertificate(void)
{
    size_t i;

    for (i = 0u; i < TEST_INPUT_MAX_STEPS; i++) {
        s_steps[i].frame = 0u;
        s_steps[i].value = 0u;
    }
    s_step_count = 0u;
    s_current_step = 0u;
    s_frame = UINT32_MAX;
    s_enabled = 0;
    s_initialized = 0;
}
#endif
