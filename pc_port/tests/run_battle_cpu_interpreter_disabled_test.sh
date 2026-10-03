#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/../.."
test_out=${CPU_DISABLED_TEST_OUT:-${TMPDIR:-$HOME/scratch}/xeno-cpu-disabled-test-$$}
mkdir -p "$test_out"
for mode in O0 O2 UBSan; do
    flags=(-O0)
    case "$mode" in
        O2) flags=(-O2);;
        UBSan) flags=(-O1 -fsanitize=undefined -fno-sanitize-recover=all);;
    esac
    clang -std=c11 -Wall -Wextra -Werror "${flags[@]}" -Ipc_port/src \
        pc_port/tests/battle_cpu_interpreter_disabled_test.c \
        pc_port/src/battle_mips_adapter.c -o "$test_out/$mode"
    "$test_out/$mode" >"$test_out/$mode.log" 2>&1
    cat "$test_out/$mode.log"
    test "$(grep -c '^\[xeno-port\]\[cpu-execution\]' "$test_out/$mode.log")" = 1
    grep -F 'entries=6 native_bridges=2 interpreted_instructions=6 disabled_refusals=2 unresolved_bridges=1' "$test_out/$mode.log"
    "$test_out/$mode" --empty >"$test_out/$mode.empty.log" 2>&1
    test "$(grep -c '^\[xeno-port\]\[cpu-execution\]' "$test_out/$mode.empty.log")" = 1
    grep -F 'entries=0 native_bridges=0 interpreted_instructions=0 disabled_refusals=0 unresolved_bridges=0' "$test_out/$mode.empty.log"
    "$test_out/$mode" --off >"$test_out/$mode.off.log" 2>&1
    if grep -q '^\[xeno-port\]\[cpu-execution\]' "$test_out/$mode.off.log"; then
        echo 'Unexpected execution report with statistics disabled' >&2
        exit 1
    fi
    clang -std=c11 -Wall -Wextra -Werror "${flags[@]}" -Ipc_port/src \
        pc_port/tests/battle_cpu_meter_disabled_test.c \
        pc_port/src/battle_mips_adapter.c -o "$test_out/$mode.meter"
    "$test_out/$mode.meter" >"$test_out/$mode.meter.log" 2>&1
    cat "$test_out/$mode.meter.log"
done
