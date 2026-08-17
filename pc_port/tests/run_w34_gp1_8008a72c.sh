#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
BUILD_DIR="${W34_GP1_8A72C_BUILD_DIR:-$ROOT/pc_port/build_native/w34_gp1_8a72c}"
mkdir -p "$BUILD_DIR"
cd "$ROOT"

BASE=(-std=gnu17 -no-pie -DXENO_PC_PORT -DSKIP_ASM -D_LANGUAGE_C -DWM_8A72C_TEST_TRACE)
WARN=(-Wall -Wextra -Wconversion -Wsign-conversion -Werror)
INC=(-Ipc_port/include_shim -Iinclude -Ipc_port/src)
SRC=(pc_port/tests/w34_gp1_8008a72c_prod_test.c
     pc_port/src/psx_memory.c
     pc_port/src/world_map_callback_8a72c.c)

check_slice() {
    local skip="$1" count="$2" want="$3" name="$4" got
    got="$(dd if=disc/world_map.bin bs=1 skip=$((skip)) count="$count" status=none | sha256sum | awk '{print $1}')"
    if [[ "$got" != "$want" ]]; then
        echo "ERROR: $name slice SHA mismatch: $got" >&2
        exit 1
    fi
}

require_retail() {
    local fixture="disc/world_map.bin"
    local expected="4c15fd32b3a03d7cd5ea4403dcaabc70abaf99b6aaca65d63d6866803edaac70"
    local actual
    if [[ ! -f "$fixture" ]]; then
        echo "ERROR: missing $fixture" >&2
        exit 1
    fi
    if [[ "$(stat -Lc '%s' "$fixture")" != "180422" ]]; then
        echo "ERROR: $fixture size is not 180422" >&2
        exit 1
    fi
    actual="$(sha256sum "$fixture" | awk '{print $1}')"
    if [[ "$actual" != "$expected" ]]; then
        echo "ERROR: world_map.bin SHA mismatch: $actual" >&2
        exit 1
    fi
    # 0x8008A72C slice (VA - 0x8006FAF0 = 0x1AC3C).
    check_slice 0x1AC3C 2960 "1d58efac94432cb6892462260a1d7679dfcfddc9d0568b08244b1fd7cf1b4b3f" "0x8008A72C"
}

compile() {
    local cc="$1"
    local name="$2"
    shift 2
    "$cc" "${BASE[@]}" "${WARN[@]}" "${INC[@]}" "$@" "${SRC[@]}" -o "$BUILD_DIR/$name"
}

build_and_run() {
    local name="$1"
    shift
    compile gcc "$name" "$@"
    "$BUILD_DIR/$name" >"$BUILD_DIR/$name.raw" 2>"$BUILD_DIR/$name.stderr"
    rg -v 'PSX RAM emulation' "$BUILD_DIR/$name.raw" >"$BUILD_DIR/$name.stdout" || true
}

require_retail

echo "== focused production regimes =="
build_and_run focused_O0 -O0 -g
build_and_run focused_O2 -O2

if compile gcc focused_ubsan -O2 -g -fsanitize=undefined -fno-sanitize-recover=all 2>"$BUILD_DIR/ubsan_gcc.err"; then
    :
else
    echo "gcc ubsan unavailable; using clang"
    compile clang focused_ubsan -O2 -g -fsanitize=undefined -fno-sanitize-recover=all
fi
"$BUILD_DIR/focused_ubsan" >"$BUILD_DIR/focused_ubsan.raw" 2>"$BUILD_DIR/focused_ubsan.stderr"
rg -v 'PSX RAM emulation' "$BUILD_DIR/focused_ubsan.raw" >"$BUILD_DIR/focused_ubsan.stdout" || true

for regime in focused_O0 focused_O2 focused_ubsan; do
    rg -q '^W34-GP1 0x8008A72C focused oracle PASS' "$BUILD_DIR/$regime.stdout"
    test ! -s "$BUILD_DIR/$regime.stderr"
done
cmp "$BUILD_DIR/focused_O0.stdout" "$BUILD_DIR/focused_O2.stdout"
cmp "$BUILD_DIR/focused_O0.stdout" "$BUILD_DIR/focused_ubsan.stdout"
echo "FOCUSED O0/O2/UBSAN PASS; normalized output identical"

mutants=(
    WM_8A72C_MUTANT_PLUS4_CASE2
    WM_8A72C_MUTANT_90A84_RET1_CONTROL
    WM_8A72C_MUTANT_NO_SECOND_95414
    WM_8A72C_MUTANT_SKIP_74794
)

killed=0
for mutant in "${mutants[@]}"; do
    name="mutant_${mutant}"
    compile gcc "$name" -O0 -D"$mutant"
    set +e
    "$BUILD_DIR/$name" >"$BUILD_DIR/$name.stdout" 2>"$BUILD_DIR/$name.stderr"
    rc=$?
    set -e
    if [ "$rc" -eq 0 ] || ! rg -q '^ASSERTION ' "$BUILD_DIR/$name.stderr"; then
        echo "$mutant FAILED mutant gate (rc=$rc)" >&2
        tail -30 "$BUILD_DIR/$name.stderr" >&2 || true
        exit 1
    fi
    assertion="$(rg -m1 '^ASSERTION ' "$BUILD_DIR/$name.stderr")"
    echo "$mutant KILLED rc=$rc $assertion"
    killed=$((killed+1))
done

echo "MUTANTS ${killed}/${#mutants[@]} KILLED"
echo "W34-GP1 0x8008A72C focused certificate PASS"
