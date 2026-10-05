#!/usr/bin/env bash
set -euo pipefail
source "$(dirname "${BASH_SOURCE[0]}")/lib/sanitizer_preflight.sh"; ubsan_preflight
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
cd "$ROOT"
source pc_port/tests/lib/sprite_d2_lib.sh
BUILD_DIR="${D2_BUILD_DIR:-$ROOT/pc_port/build_native/sprite_tick_248d4}"
mkdir -p "$BUILD_DIR"
TEST_SRC=pc_port/tests/sprite_tick_248d4_retail_test.c
UUT_SRC=src/slus_006.64/system/temp1.c
WEAKEN=(--weaken-symbol=func_800245D8 --weaken-symbol=func_80022D44
        --weaken-symbol=func_800C11CC --weaken-symbol=func_8001D2B0
        --weaken-symbol=func_8001FBE4)
TEST_ARGS=(${T248D4_NVAR:-300})
d2_check_disc
d2_regimes
echo "SPRITE TICK 248D4 O0/O2/UBSAN PASS"
d2_mutant pose_plus2 '^void func_800248D4(' 's/\*(u16\*)(pData + 0x34) + 1)/*(u16*)(pData + 0x34) + 2)/'
echo "SPRITE TICK 248D4 mutant rejected: pose_plus2"
