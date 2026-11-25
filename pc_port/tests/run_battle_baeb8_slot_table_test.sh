#!/usr/bin/env bash
set -euo pipefail
source "$(dirname "${BASH_SOURCE[0]}")/lib/sanitizer_preflight.sh"; ubsan_preflight

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
OUT="${BAEB8_SLOTS_OUT:-$ROOT/pc_port/build_native/battle_baeb8_slot_table}"
CC="${CC:-gcc}"
mkdir -p "$OUT"
cd "$ROOT"

FLAGS=(-std=gnu17 -Wall -Wextra -Werror -Wno-unused-parameter
       -m64 -fno-pie -no-pie -fno-builtin
       -DXENO_PC_PORT -DSKIP_ASM -D_LANGUAGE_C -DXENO_BATTLE_OVERLAY_HOST_BODIES
       -Ipc_port/include_shim -Iinclude -Ipc_port/src
       -Ipc_port/extern/PsyCross/include -Ipc_port/extern/PsyCross/include/psx)

# Extract the port arm of func_800BAEB8 so the live-table test compiles the
# shipped body (and the u8** mutant can swap just that body).
python3 - "$OUT/func_800BAEB8.c" <<'PY'
import pathlib, sys
src = pathlib.Path("src/battle/main111.c").read_text()
start = src.find("void func_800BAEB8(s32 a0) {")
assert start >= 0, "func_800BAEB8 port arm not found"
depth = 0
i = src.find("{", start)
for i in range(i, len(src)):
    if src[i] == "{":
        depth += 1
    elif src[i] == "}":
        depth -= 1
        if depth == 0:
            body = src[start : i + 1]
            break
else:
    raise SystemExit("func_800BAEB8 body not closed")
pathlib.Path(sys.argv[1]).write_text(
    '#include "common.h"\n'
    '#include "battle_host_guest.h"\n'
    'extern void func_800223B0(u8* p, s32 v);\n'
    'extern void func_80021FE0(u8* p, s32 v);\n\n'
    + body
    + "\n"
)
PY

compile_run() {
    local label="$1"
    local opt="$2"
    local sanitize="${3:-0}"
    local body="${4:-$OUT/func_800BAEB8.c}"
    local san=()
    if [[ "$sanitize" == 1 ]]; then
        san=(-fsanitize=undefined -fno-sanitize-recover=all)
    fi
    "$CC" "${FLAGS[@]}" "$opt" "${san[@]}" \
        pc_port/tests/battle_baeb8_slot_table_test.c \
        -o "$OUT/${label}_layout"
    "$OUT/${label}_layout" | tee "$OUT/${label}_layout.stdout"
    "$CC" "${FLAGS[@]}" "$opt" "${san[@]}" \
        pc_port/tests/battle_baeb8_slot_func_test.c \
        "$body" \
        -o "$OUT/${label}_func"
    "$OUT/${label}_func" | tee "$OUT/${label}_func.stdout"
}

compile_run O0 -O0
compile_run O2 -O2
compile_run UBSan -O2 1

# Mutant: restore the native u8** table index. It must fail the live-table
# test because the 8-byte load concatenates the next slot's pointer.
python3 - "$OUT/func_800BAEB8.c" "$OUT/func_800BAEB8_mutant.c" <<'PY'
from pathlib import Path
import sys
text = Path(sys.argv[1]).read_text()
old = "    u8* s1 = BG_PTR(BG_U32(0x800C3EB0u + a0 * 4 + 0x8C8C));"
new = "    u8* s1 = ((u8**)(D_800C3EB0 + 0x8C8C))[a0];"
if old not in text:
    raise SystemExit("fixed BG_U32 table load not found for mutant")
Path(sys.argv[2]).write_text(text.replace(old, new, 1))
PY

set +e
"$CC" "${FLAGS[@]}" -O0 \
    pc_port/tests/battle_baeb8_slot_func_test.c \
    "$OUT/func_800BAEB8_mutant.c" \
    -o "$OUT/mutant_func"
"$OUT/mutant_func" >"$OUT/mutant.stdout" 2>"$OUT/mutant.stderr"
mutant_rc=$?
set -e
if [[ "$mutant_rc" -eq 0 ]]; then
    echo "FAIL: u8** mutant was accepted" >&2
    cat "$OUT/mutant.stdout" >&2
    exit 1
fi
# The 8-byte load concatenates slot 1: either the harness reports the wrong
# actor, or the +0xAF read SIGSEGVs. Both reject it.
if ! grep -Eq 'slot 0 used|did not call|8-byte table load' \
        "$OUT/mutant.stdout" "$OUT/mutant.stderr"; then
    if [[ "$mutant_rc" -lt 128 ]]; then
        echo "FAIL: u8** mutant failed without a clobber or crash" >&2
        cat "$OUT/mutant.stdout" "$OUT/mutant.stderr" >&2
        exit 1
    fi
fi

echo "func_800BAEB8 slot-table proofs: PASS"
