#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
BUILD_DIR="${W34B64_BUILD_DIR:-$ROOT/pc_port/build_native/w34b64_terrain}"
CC="${CC:-clang}"
mkdir -p "$BUILD_DIR"
cd "$ROOT"

BASE=(-std=gnu17 -DXENO_PC_PORT -DSKIP_ASM -D_LANGUAGE_C -no-pie)
WARN=(-Wall -Wextra -Wconversion -Wsign-conversion -Werror)
INC=(-Ipc_port/include_shim -Iinclude -Ipc_port/extern/PsyCross/include
     -Ipc_port/extern/PsyCross/include/psx -Ipc_port/src)

compile_and_run() {
    local component="$1"
    local define="$2"
    local source="$3"
    local regime="$4"
    shift 4
    "$CC" "${BASE[@]}" "${WARN[@]}" "${INC[@]}" -D"$define" "$@" \
        pc_port/tests/w34b64_terrain_prod_test.c pc_port/src/psx_memory.c \
        "$source" -o "$BUILD_DIR/${component}_${regime}"
    "$BUILD_DIR/${component}_${regime}" \
        >"$BUILD_DIR/${component}_${regime}.raw" \
        2>"$BUILD_DIR/${component}_${regime}.stderr"
    rg -v 'PSX RAM emulation' "$BUILD_DIR/${component}_${regime}.raw" \
        >"$BUILD_DIR/${component}_${regime}.stdout" || true
    test ! -s "$BUILD_DIR/${component}_${regime}.stderr"
}

for component in dispatch grid submit; do
    if [[ "$component" == dispatch ]]; then
        define=W34B64_DISPATCH_TEST
        source=pc_port/src/world_map_helper_9932c.c
        expected='W34B64 terrain dispatcher certificate PASS'
    elif [[ "$component" == grid ]]; then
        define=W34B64_GRID_TEST
        source=pc_port/src/world_map_helper_99708.c
        expected='W34B64 terrain grid certificate PASS'
    else
        define=W34B64_SUBMIT_TEST
        source=pc_port/src/world_map_helper_9980c.c
        expected='W34B64 terrain submitter certificate PASS'
    fi
    compile_and_run "$component" "$define" "$source" O0 -O0 -g
    compile_and_run "$component" "$define" "$source" O2 -O2
    compile_and_run "$component" "$define" "$source" UBSan -O2 -g \
        -fsanitize=undefined -fno-sanitize-recover=all
    for regime in O0 O2 UBSan; do
        rg -q "^${expected}$" "$BUILD_DIR/${component}_${regime}.stdout"
    done
    cmp "$BUILD_DIR/${component}_O0.stdout" "$BUILD_DIR/${component}_O2.stdout"
    cmp "$BUILD_DIR/${component}_O0.stdout" "$BUILD_DIR/${component}_UBSan.stdout"
done

echo "W34B64 TERRAIN CERTIFICATE PASS; O0/O2/UBSan; strict warnings clean"
