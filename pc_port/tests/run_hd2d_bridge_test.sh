#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/../.."
task_hd2d_tmp=$(mktemp -d)
trap 'rm -rf "$task_hd2d_tmp"' EXIT
cc -std=c11 -Wall -Wextra -Werror -Ipc_port/include_shim -Ipc_port/src \
    pc_port/src/hd2d/hd2d_bridge.c pc_port/src/hd2d/hd2d_policy.c \
    pc_port/tests/hd2d/hd2d_bridge_test.c -o "$task_hd2d_tmp/test"
env -u XENO_HD2D_OCTOPATH -u XENO_HD2D_OPENING_MAPS "$task_hd2d_tmp/test"
