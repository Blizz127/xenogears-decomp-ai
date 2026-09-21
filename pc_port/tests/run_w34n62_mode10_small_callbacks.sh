#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
OUT="$ROOT/pc_port/build_tests/w34n62_mode10_small_callbacks"
CC_BIN="${CC:-cc}"
COMMON=(
  -std=gnu17 -DXENO_PC_PORT -D_LANGUAGE_C
  -Wall -Wextra -Werror -Wconversion -Wsign-conversion
  -I"$ROOT/pc_port/include_shim" -I"$ROOT/include"
  -I"$ROOT/pc_port/extern/PsyCross/include"
  -I"$ROOT/pc_port/extern/PsyCross/include/psx"
  -I"$ROOT/pc_port/src"
  "$ROOT/pc_port/tests/w34n62_mode10_small_callbacks_prod_test.c"
  "$ROOT/pc_port/src/world_map_callback_794d8.c"
  "$ROOT/pc_port/src/world_map_scheduler.c"
)

mkdir -p "$OUT"

check_slice() {
  local start="$1"
  local end="$2"
  local expected="$3"
  local actual
  actual="$(dd if="$ROOT/disc/world_map.bin" bs=1 \
    skip=$((start - 0x8006FAF0)) count=$((end - start)) status=none | \
    sha256sum | awk '{print $1}')"
  if [[ "$actual" != "$expected" ]]; then
    printf 'ERROR: retail slice 0x%X..0x%X SHA mismatch: %s\n' \
      "$start" "$end" "$actual" >&2
    exit 1
  fi
}

check_slice 0x800794D8 0x800795E4 256152e5da54eff64bd75bd4530688082473c4195dcfcdbffa3fcabd5b397992
check_slice 0x8007A410 0x8007A568 0eb78dd4b6f028a4a2475237bd2b91b9acee353b2d465289a0339896058d7c50
check_slice 0x8007A568 0x8007A5DC c305e4ab4e51001f447c7636fb56d5d65edbab362b1486b38ec6eeff853b3047

run_regime() {
  local name="$1"
  shift
  "$CC_BIN" "${COMMON[@]}" "$@" -o "$OUT/$name"
  "$OUT/$name"
}

run_mutant() {
  local name="$1"
  local define="$2"
  local assertion="$3"
  "$CC_BIN" "${COMMON[@]}" -O2 "$define" -o "$OUT/$name"
  if "$OUT/$name" >"$OUT/$name.log" 2>&1; then
    echo "ERROR: $name survived" >&2
    exit 1
  fi
  if ! grep -q "ASSERTION $assertion FAILED" "$OUT/$name.log"; then
    echo "ERROR: $name did not fail named assertion $assertion" >&2
    sed -n '1,180p' "$OUT/$name.log" >&2
    exit 1
  fi
  echo "$name DETECTED ($assertion)"
}

run_regime o0 -O0
run_regime o2 -O2
run_regime ubsan -O2 -fsanitize=undefined -fno-sanitize-recover=undefined

run_mutant m1 -DW34N62_MUTANT_WRONG_ROTATION_ANGLE orbit_init.rotation
run_mutant m2 -DW34N62_MUTANT_SKIP_HEIGHT_OFFSET orbit_follow.height
run_mutant m3 -DW34N62_MUTANT_WRONG_TIMER_SEED beacon_init.timer
run_mutant m4 -DW34N62_MUTANT_WRONG_ACTIVE_MARKER beacon_active.marker
run_mutant m5 -DW34N62_MUTANT_SKIP_RESET_HEADING beacon_reset.heading
run_mutant m6 -DW34N62_MUTANT_WRONG_INIT_MARKER marker_init.id

echo "W34N62 MODE10 SMALL CALLBACK CERTIFICATE PASS: O0/O2/UBSan; M1-M6 detected; strict warnings clean"
