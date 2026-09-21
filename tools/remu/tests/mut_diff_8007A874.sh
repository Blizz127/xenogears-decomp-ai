#!/usr/bin/env bash
# Mutant for diff_8007A874: shift the table row by 5 instead of 6.
# Usage: mut_diff_8007A874.sh <tu-src> <mutant-out>
set -uo pipefail
python3 - "$1" "$2" <<'PY'
import sys
src = open(sys.argv[1]).read()
old = "(D_800D3430 + ((u32)index << 6))[p[2]]"
assert src.count(old) == 1
open(sys.argv[2], "w").write(src.replace(old, "(D_800D3430 + ((u32)index << 5))[p[2]]"))
PY
