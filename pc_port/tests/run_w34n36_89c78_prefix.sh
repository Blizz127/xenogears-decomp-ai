#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
BUILD_DIR="${W34N36_89C78_BUILD_DIR:-$ROOT/pc_port/build_native/w34n36_89c78}"
CC="${CC:-clang}"
mkdir -p "$BUILD_DIR"
cd "$ROOT"

BASE=(-std=gnu17 -DXENO_PC_PORT -DSKIP_ASM -D_LANGUAGE_C
      -DWM_93534_TEST_TRACE -fno-pie -no-pie)
WARN=(-Wall -Wextra -Wconversion -Wsign-conversion -Werror)
INC=(-Ipc_port/include_shim -Iinclude
     -Ipc_port/extern/PsyCross/include
     -Ipc_port/extern/PsyCross/include/psx -Ipc_port/src)
SRC=(pc_port/tests/w34n36_89c78_prefix_prod_test.c
     pc_port/src/psx_memory.c
     pc_port/src/world_map_helper_89c78.c
     pc_port/src/world_map_helper_93534.c)

compile_and_run() {
    local name="$1"
    local run_rc=0
    shift
    "$CC" "${BASE[@]}" "${WARN[@]}" "${INC[@]}" "$@" "${SRC[@]}" \
        -o "$BUILD_DIR/$name"
    "$BUILD_DIR/$name" >"$BUILD_DIR/$name.raw" \
        2>"$BUILD_DIR/$name.stderr" || run_rc=$?
    rg -v 'PSX RAM emulation' "$BUILD_DIR/$name.raw" \
        >"$BUILD_DIR/$name.stdout" || true
    return "$run_rc"
}

compile_and_run O0 -O0 -g
compile_and_run O2 -O2
compile_and_run UBSan -O2 -g -fsanitize=undefined \
    -fno-sanitize-recover=all

for regime in O0 O2 UBSan; do
    rg -q '^W34N36 0x80089C78 retail-prefix certificate PASS$' \
        "$BUILD_DIR/$regime.stdout"
    test ! -s "$BUILD_DIR/$regime.stderr"
done
cmp "$BUILD_DIR/O0.stdout" "$BUILD_DIR/O2.stdout"
cmp "$BUILD_DIR/O0.stdout" "$BUILD_DIR/UBSan.stdout"
echo "CERTIFICATE O0/O2/UBSan PASS; strict warnings clean"

mutants=(
    'M1:WM_89C78_MUTANT_FIXED_TABLE'
    'M2:WM_89C78_MUTANT_ZERO_IS_ACTIVE'
    'M3:WM_89C78_MUTANT_WRAP_RECORD'
    'M4:WM_89C78_MUTANT_WRONG_POSITION_FIELDS'
    'M5:WM_89C78_MUTANT_NO_CAMERA_SUBTRACT'
)
for entry in "${mutants[@]}"; do
    label="${entry%%:*}"
    define="${entry#*:}"
    set +e
    compile_and_run "$label" -O0 -g -D"$define"
    rc=$?
    set -e
    if [[ "$rc" -eq 0 ]] || ! rg -q '^ASSERTION ' "$BUILD_DIR/$label.stderr"; then
        echo "$label FAILED mutant gate rc=$rc" >&2
        exit 1
    fi
    assertion="$(rg -m1 '^ASSERTION ' "$BUILD_DIR/$label.stderr")"
    echo "$label DETECTED; $assertion"
done

echo "W34N36 0x80089C78 CERTIFICATE PASS; M1-M5 DETECTED"
