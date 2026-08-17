#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
BUILD_DIR="${W34B24_I44_BUILD_DIR:-$ROOT/pc_port/build_native/w34b24_i44}"
mkdir -p "$BUILD_DIR"
cd "$ROOT"

SCRATCH_LIB="/home/blizz/dev/xenogears-worktrees/w34-r4world-b-73b04-clean-scratch/lib"
if [[ -d "$SCRATCH_LIB" ]]; then
    export LIBRARY_PATH="$SCRATCH_LIB${LIBRARY_PATH:+:$LIBRARY_PATH}"
    export LD_LIBRARY_PATH="$SCRATCH_LIB${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
fi

CC="${CC:-clang}"
BASE=(-std=gnu17 -DXENO_PC_PORT -DSKIP_ASM -D_LANGUAGE_C -DWM_747DC_TEST_TRACE -no-pie)
WARN=(-Wall -Wextra -Wconversion -Wsign-conversion -Werror)
INC=(-Ipc_port/include_shim -Iinclude -Ipc_port/src)
SRC=(pc_port/tests/w34b24_i44_747dc_prod_test.c
     pc_port/src/psx_memory.c
     pc_port/src/world_map_helper_747dc.c)

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
    check_slice 0x4CEC 1660 "ca255b493d7948e43c2bea3ff15f14bd714f3729adbbaf821884b5195bd959d4" "0x800747DC"
}

compile_one() {
    local name="$1"
    shift
    if ! "$CC" "${BASE[@]}" "${WARN[@]}" "${INC[@]}" "$@" "${SRC[@]}" -o "$BUILD_DIR/$name"; then
        if [[ "$CC" != gcc ]]; then
            echo "NOTE: $CC failed for $name; retrying with gcc" >&2
            gcc "${BASE[@]}" "${WARN[@]}" "${INC[@]}" "$@" "${SRC[@]}" -o "$BUILD_DIR/$name"
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
    rg -q '^W34B24-I44 0x800747DC focused oracle PASS' "$BUILD_DIR/$regime.stdout"
    test ! -s "$BUILD_DIR/$regime.stderr"
done
cmp "$BUILD_DIR/focused_O0.stdout" "$BUILD_DIR/focused_O2.stdout"
cmp "$BUILD_DIR/focused_O0.stdout" "$BUILD_DIR/focused_ubsan.stdout"
echo "FOCUSED O0/O2/UBSAN PASS; normalized output identical"

mutants=(
    WM_747DC_MUTANT_SKIP_EARLY
    WM_747DC_MUTANT_WRONG_COUNT
    WM_747DC_MUTANT_SKIP_93978
    WM_747DC_MUTANT_SKIP_93740
    WM_747DC_MUTANT_SKIP_MODE1
    WM_747DC_MUTANT_SKIP_MODE2
    WM_747DC_MUTANT_WRONG_SHIFT
    WM_747DC_MUTANT_WRONG_REC
    WM_747DC_MUTANT_WRONG_PKT
    WM_747DC_MUTANT_SKIP_OT
    WM_747DC_MUTANT_WRONG_ZSHIFT
    WM_747DC_MUTANT_SKIP_CLEAR
    WM_747DC_MUTANT_FORCE_CLIP
    WM_747DC_MUTANT_WRONG_POS
    WM_747DC_MUTANT_SKIP_SCRATCH
    WM_747DC_MUTANT_SKIP_PACK
    WM_747DC_MUTANT_SKIP_MIN_RTPS
    WM_747DC_MUTANT_WRONG_THRESH
)
killed=0
for mutant in "${mutants[@]}"; do
    name="mutant_${mutant}"
    compile_one "$name" -O0 -D"$mutant"
    set +e
    timeout 5 "$BUILD_DIR/$name" >"$BUILD_DIR/$name.stdout" 2>"$BUILD_DIR/$name.stderr"
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
echo "W34B24-I44 0x800747DC focused certificate PASS"
