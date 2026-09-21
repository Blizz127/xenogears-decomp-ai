#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/../.."
BUILD_DIR="$(mktemp -d "${TMPDIR:-/tmp}/vblank-dispatch.XXXXXX")"
trap 'rm -rf -- "$BUILD_DIR"' EXIT
for mode in O0 O2 UBSan; do
    flags=(-O2 -fPIE -pie)
    [[ "$mode" != O0 ]] || flags=(-O0 -fPIE -pie)
    [[ "$mode" != UBSan ]] || flags+=(-fsanitize=undefined -fno-sanitize-recover=all)
    "${CC:-gcc}" -std=c11 -Wall -Wextra -Werror -Iinclude "${flags[@]}" \
        pc_port/tests/controller_vblank_dispatch_test.c \
        pc_port/src/controller_vblank_dispatch.c \
        pc_port/src/controller_vblank_helpers.c \
        -Wl,--wrap=func_80035E44 -Wl,--wrap=func_80036220 \
        -o "$BUILD_DIR/$mode"
    "$BUILD_DIR/$mode"
done
"${CC:-gcc}" -std=c11 -Wall -Wextra -Werror -Iinclude -O2 -fPIE -pie \
    -DVBLANK_TEST_TRUNCATE_CALLBACK \
    pc_port/tests/controller_vblank_dispatch_test.c \
    pc_port/src/controller_vblank_dispatch.c \
    pc_port/src/controller_vblank_helpers.c \
    -Wl,--wrap=func_80035E44 -Wl,--wrap=func_80036220 \
    -Wl,--wrap=func_800363F0 -o "$BUILD_DIR/mutant"
if "$BUILD_DIR/mutant" >"$BUILD_DIR/mutant.log" 2>&1; then
    echo 'ERROR: truncated-callback mutant survived' >&2
    exit 1
fi
grep -q 'ASSERTION vblank.dispatch .*D_800501FC == callback_a' "$BUILD_DIR/mutant.log"
echo 'CONTROLLER VBLANK DISPATCH truncated-callback mutant detected'
