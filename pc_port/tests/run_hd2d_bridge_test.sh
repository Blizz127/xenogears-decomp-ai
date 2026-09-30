#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/../.."
task_hd2d_tmp=$(mktemp -d)
trap 'rm -rf "$task_hd2d_tmp"' EXIT
for api in present absent; do
    flags=()
    if [[ "$api" == absent ]]; then flags=(-DHD2D_TEST_NO_VALUE_API); fi
    for mode in O0 O2 UBSan; do
        checks=(-"$mode")
        if [[ "$mode" == UBSan ]]; then checks=(-O2 -fsanitize=undefined -fno-sanitize-recover=all); fi
        cc -std=c11 -Wall -Wextra -Werror "${flags[@]}" "${checks[@]}" -Ipc_port/include_shim -Ipc_port/src \
            pc_port/src/hd2d/hd2d_bridge.c pc_port/src/hd2d/hd2d_policy.c \
            pc_port/tests/hd2d/hd2d_bridge_test.c -o "$task_hd2d_tmp/test"
        env -u XENO_HD2D_OCTOPATH -u XENO_HD2D_OPENING_MAPS "$task_hd2d_tmp/test"
    done
done
