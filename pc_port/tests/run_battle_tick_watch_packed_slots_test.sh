#!/usr/bin/env bash
# Runtime + source lock: battle tick-watch callbacks keep packed 4-byte
# actor / callback slots (retail lw / jalr).
set -euo pipefail
source "$(dirname "${BASH_SOURCE[0]}")/lib/sanitizer_preflight.sh"; ubsan_preflight

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
OUT="${TICK_WATCH_SLOTS_OUT:-$ROOT/pc_port/build_native/battle_tick_watch_packed_slots}"
CC="${CC:-gcc}"
mkdir -p "$OUT"
cd "$ROOT"

FLAGS=(-std=gnu17 -Wall -Wextra -Werror -Wno-unused-parameter
       -m64 -fno-pie -no-pie -fno-builtin
       -DXENO_PC_PORT -DSKIP_ASM -D_LANGUAGE_C
       -Ipc_port/include_shim -Iinclude -Ipc_port/src
       -Ipc_port/extern/PsyCross/include -Ipc_port/extern/PsyCross/include/psx)

python3 - "$OUT/bodies.c" <<'PY'
from pathlib import Path
import sys

src84 = Path("src/battle/mainc84.c").read_text()
src130 = Path("src/battle/mainc130.c").read_text()

def extract_5854(src):
    a = src.index("void func_800B5854(u8* task)")
    b = src.index("\n#else", a)
    return src[a:b].strip() + "\n"

def extract_bf73c(src):
    a = src.index("void func_800BF73C(u8* task)")
    b = src.index("\n}\n\n\nextern u8* TimerWorkListAllocateTask", a) + 2
    return src[a:b].strip() + "\n"

body_5854 = extract_5854(src84)
body_bf73c = extract_bf73c(src130)
assert "*(u8**)" not in body_5854, "func_800B5854 still has *(u8**)"
assert "*(void (**)" not in body_5854, "func_800B5854 still has host-width callback load"
assert "(uintptr_t)*(u32*)(task + 0x1C)" in body_5854
assert "*(u8**)" not in body_bf73c.split("#else")[0], "port arm of func_800BF73C still has *(u8**)"
assert "(uintptr_t)*(u32*)(task + 0x1C)" in body_bf73c

bodies = (
    '#include "common.h"\n#include <stdint.h>\n\n'
    "extern u32 func_800B57E4(u8* s);\n\n"
    + body_5854
    + "\n"
    + body_bf73c
)
Path(sys.argv[1]).write_text(bodies)
print("TICK WATCH SLOTS SOURCE PASS")
PY

python3 - <<'PY'
from pathlib import Path
src84 = Path("src/battle/mainc84.c").read_text()
src130 = Path("src/battle/mainc130.c").read_text()
a = src84.index("void func_800B5854(u8* task)")
b = src84.index("\n#else", a)
body = src84[a:b]
assert "*(u8**)" not in body, "func_800B5854 still has *(u8**)"
assert "(uintptr_t)*(u32*)(task + 0x1C)" in body
a = src130.index("void func_800BF73C(u8* task)")
b = src130.index("\n}\n\n\nextern u8* TimerWorkListAllocateTask", a)
port = src130[a:b].split("#else")[0]
assert "*(u8**)" not in port, "func_800BF73C port arm still has *(u8**)"
assert "(uintptr_t)*(u32*)(task + 0x1C)" in port
print("TICK WATCH SLOTS CONTAINMENT PASS")
PY

compile_run() {
    local label="$1"
    local opt="$2"
    local sanitize="${3:-0}"
    local body="${4:-$OUT/bodies.c}"
    local san=()
    if [[ "$sanitize" == 1 ]]; then
        san=(-fsanitize=undefined -fno-sanitize-recover=all)
    fi
    "$CC" "${FLAGS[@]}" "$opt" "${san[@]}" \
        pc_port/tests/battle_tick_watch_packed_slots_test.c \
        -c -o "$OUT/${label}_test.o"
    "$CC" "${FLAGS[@]}" "$opt" "${san[@]}" -w \
        -c "$body" -o "$OUT/${label}_body.o"
    "$CC" -no-pie "$opt" "${san[@]}" \
        "$OUT/${label}_test.o" "$OUT/${label}_body.o" \
        -o "$OUT/${label}_func"
    "$OUT/${label}_func" | tee "$OUT/${label}_func.stdout"
}

compile_run O0 -O0
compile_run O2 -O2
compile_run UBSan -O2 1

# Mutant: restore the void** actor load in func_800BF73C. It must fail
# because the 8-byte +0x1C load concatenates the planted AF word.
python3 - "$OUT/bodies.c" "$OUT/bodies_mutant.c" <<'PY'
from pathlib import Path
import sys
text = Path(sys.argv[1]).read_text()
old = "    u8* tgt = (u8*)(uintptr_t)*(u32*)(task + 0x1C);"
new = "    u8* tgt = *(u8**)(task + 0x1C);"
if old not in text:
    raise SystemExit("fixed u32 actor load not found for mutant")
Path(sys.argv[2]).write_text(text.replace(old, new, 1))
PY

set +e
"$CC" "${FLAGS[@]}" -O0 \
    -c pc_port/tests/battle_tick_watch_packed_slots_test.c -o "$OUT/mutant_test.o"
"$CC" "${FLAGS[@]}" -O0 -w \
    -c "$OUT/bodies_mutant.c" -o "$OUT/mutant_body.o"
"$CC" -no-pie -O0 \
    "$OUT/mutant_test.o" "$OUT/mutant_body.o" \
    -o "$OUT/mutant_func"
"$OUT/mutant_func" >"$OUT/mutant.stdout" 2>"$OUT/mutant.stderr"
mutant_rc=$?
set -e
if [[ "$mutant_rc" -eq 0 ]]; then
    echo "FAIL: void** mutant was accepted" >&2
    cat "$OUT/mutant.stdout" >&2
    exit 1
fi
if ! grep -Eq 'sees the actor|FAIL:|Segmentation fault|POINTER' \
        "$OUT/mutant.stdout" "$OUT/mutant.stderr"; then
    if [[ "$mutant_rc" -lt 128 ]]; then
        echo "FAIL: void** mutant failed without a clobber or crash" >&2
        cat "$OUT/mutant.stdout" "$OUT/mutant.stderr" >&2
        exit 1
    fi
fi
echo "TICK WATCH SLOTS negative control PASS host-width load rejected"
echo "TICK WATCH SLOTS proofs: PASS"
