#!/usr/bin/env bash
set -uo pipefail
python3 - "$1" "$2" <<'PY'
import sys
src = open(sys.argv[1]).read()
old = "*(u16*)(row + ((u32)p[1] << 1)) = result;"
assert src.count(old) == 1
open(sys.argv[2], "w").write(src.replace(old, "*(u16*)(row + ((u32)p[2] << 1)) = result;"))
PY
