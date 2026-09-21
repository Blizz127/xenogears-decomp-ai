#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/../.."
BUILD_DIR="$(mktemp -d "${TMPDIR:-/tmp}/vblank-helpers.XXXXXX")"
trap 'rm -rf -- "$BUILD_DIR"' EXIT
for mode in O0 O2 UBSan; do
    flags=(-O2)
    [[ "$mode" != O0 ]] || flags=(-O0)
    [[ "$mode" != UBSan ]] || flags+=(-fsanitize=undefined -fno-sanitize-recover=all)
    "${CC:-gcc}" -std=c11 -Wall -Wextra -Werror "${flags[@]}" \
        pc_port/tests/controller_vblank_helpers_test.c \
        pc_port/src/controller_vblank_helpers.c -o "$BUILD_DIR/$mode"
    "$BUILD_DIR/$mode"
done
