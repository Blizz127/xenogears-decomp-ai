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
    "$test_out/$mode"
done
