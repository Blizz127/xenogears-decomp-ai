#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
BUILD_DIR="${W34B24_I14_BUILD_DIR:-$ROOT/pc_port/build_native/w34b24_i14}"
mkdir -p "$BUILD_DIR"
cd "$ROOT"

SCRATCH_LIB="/home/blizz/dev/xenogears-worktrees/w34-r4world-b-73b04-clean-scratch/lib"
if [[ -d "$SCRATCH_LIB" ]]; then
    export LIBRARY_PATH="$SCRATCH_LIB${LIBRARY_PATH:+:$LIBRARY_PATH}"
    export LD_LIBRARY_PATH="$SCRATCH_LIB${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
fi

CC="${CC:-gcc}"
BASE=(-std=gnu17 -DXENO_PC_PORT -DSKIP_ASM -D_LANGUAGE_C -DWM_914D0_TEST_TRACE -no-pie)
WARN=(-Wall -Wextra -Wconversion -Wsign-conversion -Werror)
INC=(-Ipc_port/include_shim -Iinclude -Ipc_port/src)
SRC=(pc_port/tests/w34b24_i14_914d0_prod_test.c
     pc_port/src/psx_memory.c
     pc_port/src/world_map_callback_914d0.c)

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
    # wm_800914D0 body (VA - 0x8006FAF0 = 0x219E0).
    check_slice 0x219E0 1668 "1d9e6b5882ef8d968a46c7c8f3d2699d719f2aa97dcd65eb1ad9d9bc76cfb14e" "0x800914D0"
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
    rg -q '^W34B24-I14 0x800914D0 focused oracle PASS' "$BUILD_DIR/$regime.stdout"
    test ! -s "$BUILD_DIR/$regime.stderr"
done
cmp "$BUILD_DIR/focused_O0.stdout" "$BUILD_DIR/focused_O2.stdout"
cmp "$BUILD_DIR/focused_O0.stdout" "$BUILD_DIR/focused_ubsan.stdout"
echo "FOCUSED O0/O2/UBSAN PASS; normalized output identical"

mutants=(
    WM_914D0_MUTANT_SUBSTATE_POLARITY
    WM_914D0_MUTANT_SUB_HEADING
    WM_914D0_MUTANT_PAD_SHIFT
    WM_914D0_MUTANT_PAD_POLARITY
    WM_914D0_MUTANT_SYNC_THRESHOLD
    WM_914D0_MUTANT_STATE1_SKIP_CLEAR
    WM_914D0_MUTANT_CHASE_SRA
    WM_914D0_MUTANT_SLOW_SRA
    WM_914D0_MUTANT_SKIP_97770
    WM_914D0_MUTANT_WRONG_97770_ARGS
    WM_914D0_MUTANT_CLOSE_THRESHOLD
    WM_914D0_MUTANT_SNAP_Y_SHIFT
    WM_914D0_MUTANT_SKIP_93354
    WM_914D0_MUTANT_SKIP_PUBLISH
    WM_914D0_MUTANT_WRONG_RETURN
    WM_914D0_MUTANT_SKIP_60
    WM_914D0_MUTANT_APPROACH_SKIP_Y
    WM_914D0_MUTANT_JT1_GUARD
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
        tail -30 "$BUILD_DIR/$name.stderr" >&2 || true
        exit 1
    fi
    assertion="$(rg -m1 '^ASSERTION ' "$BUILD_DIR/$name.stderr")"
    echo "$mutant KILLED rc=$rc $assertion"
    killed=$((killed+1))
done

echo "MUTANTS ${killed}/${#mutants[@]} KILLED"
echo "W34B24-I14 0x800914D0 focused certificate PASS"
