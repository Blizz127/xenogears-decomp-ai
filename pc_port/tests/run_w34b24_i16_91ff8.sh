#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
BUILD_DIR="${W34B24_I16_BUILD_DIR:-$ROOT/pc_port/build_native/w34b24_i16}"
mkdir -p "$BUILD_DIR"
cd "$ROOT"

SCRATCH_LIB="/home/blizz/dev/xenogears-worktrees/w34-r4world-b-73b04-clean-scratch/lib"
if [[ -d "$SCRATCH_LIB" ]]; then
    export LIBRARY_PATH="$SCRATCH_LIB${LIBRARY_PATH:+:$LIBRARY_PATH}"
    export LD_LIBRARY_PATH="$SCRATCH_LIB${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
fi

CC="${CC:-gcc}"
BASE=(-std=gnu17 -DXENO_PC_PORT -DSKIP_ASM -D_LANGUAGE_C -DWM_91FF8_TEST_TRACE -no-pie)
WARN=(-Wall -Wextra -Wconversion -Wsign-conversion -Werror)
INC=(-Ipc_port/include_shim -Iinclude -Ipc_port/src)
SRC=(pc_port/tests/w34b24_i16_91ff8_prod_test.c
     pc_port/src/psx_memory.c
     pc_port/src/world_map_helper_91ff8.c)

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
    if [[ ! -f "$fixture" ]] || [[ "$(stat -Lc '%s' "$fixture")" != "180422" ]]; then
        echo "ERROR: missing or wrong-size $fixture" >&2
        exit 1
    fi
    local actual
    actual="$(sha256sum "$fixture" | awk '{print $1}')"
    if [[ "$actual" != "$expected" ]]; then
        echo "ERROR: world_map.bin SHA mismatch: $actual" >&2
        exit 1
    fi
    check_slice 0x22508 572 "9e3c8c4ceabb5b9f2d9bc213857d21570bd968ea7cc2b6fea95883b36d2c93f9" "0x80091FF8"
}

compile_one() {
    local name="$1"
    shift
    if ! "$CC" "${BASE[@]}" "${WARN[@]}" "${INC[@]}" "$@" "${SRC[@]}" -o "$BUILD_DIR/$name"; then
        if [[ "$CC" != clang ]]; then
            echo "NOTE: $CC failed for $name; retrying with clang" >&2
            clang "${BASE[@]}" "${WARN[@]}" "${INC[@]}" "$@" "${SRC[@]}" -o "$BUILD_DIR/$name"
        else
            return 1
        fi
    fi
}

build_and_run() {
    local name="$1"
    shift
    compile_one "$name" "$@"
    "$BUILD_DIR/$name" >"$BUILD_DIR/$name.raw" 2>"$BUILD_DIR/$name.stderr"
    rg -v 'PSX RAM emulation' "$BUILD_DIR/$name.raw" >"$BUILD_DIR/$name.stdout" || true
}

require_retail
echo "== focused production regimes =="
build_and_run focused_O0 -O0 -g
build_and_run focused_O2 -O2
build_and_run focused_ubsan -O2 -g -fsanitize=undefined -fno-sanitize-recover=all
for regime in focused_O0 focused_O2 focused_ubsan; do
    rg -q '^W34B24-I16 0x80091FF8 focused oracle PASS' "$BUILD_DIR/$regime.stdout"
    test ! -s "$BUILD_DIR/$regime.stderr"
done
cmp "$BUILD_DIR/focused_O0.stdout" "$BUILD_DIR/focused_O2.stdout"
cmp "$BUILD_DIR/focused_O0.stdout" "$BUILD_DIR/focused_ubsan.stdout"
echo "FOCUSED O0/O2/UBSAN PASS; normalized output identical"

mutants=(
    WM_91FF8_MUTANT_SKIP_96F18
    WM_91FF8_MUTANT_WRONG_96F18_DEST
    WM_91FF8_MUTANT_SKIP_93354
    WM_91FF8_MUTANT_SKIP_93660
    WM_91FF8_MUTANT_GRID_5
    WM_91FF8_MUTANT_OUTER_2
    WM_91FF8_MUTANT_MIN_START_NEG
    WM_91FF8_MUTANT_SKIP_MASK
    WM_91FF8_MUTANT_WRONG_STEP
    WM_91FF8_MUTANT_SKIP_SNAP
    WM_91FF8_MUTANT_WRONG_ABS_THRESH
    WM_91FF8_MUTANT_SKIP_TOWARD_ZERO
    WM_91FF8_MUTANT_WRONG_RETURN
    WM_91FF8_MUTANT_SKIP_HEADING
)
killed=0
for mutant in "${mutants[@]}"; do
    name="mutant_${mutant}"
    compile_one "$name" -O0 -D"$mutant"
    set +e
    "$BUILD_DIR/$name" >"$BUILD_DIR/$name.stdout" 2>"$BUILD_DIR/$name.stderr"
    rc=$?
    set -e
    if [ "$rc" -eq 0 ] || ! rg -q '^ASSERTION ' "$BUILD_DIR/$name.stderr"; then
        echo "$mutant FAILED mutant gate (rc=$rc)" >&2
        tail -20 "$BUILD_DIR/$name.stderr" >&2 || true
        exit 1
    fi
    echo "$mutant KILLED rc=$rc $(rg -m1 '^ASSERTION ' "$BUILD_DIR/$name.stderr")"
    killed=$((killed+1))
done
echo "MUTANTS ${killed}/${#mutants[@]} KILLED"
echo "W34B24-I16 0x80091FF8 focused certificate PASS"
