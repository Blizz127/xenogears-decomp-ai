#!/usr/bin/env bash
# Runtime + source lock: type-2/7/15 render callbacks keep packed 4-byte
# work-list / SpriteData / per-context buffer slots (retail lw).
set -euo pipefail
source "$(dirname "${BASH_SOURCE[0]}")/lib/sanitizer_preflight.sh"; ubsan_preflight

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
OUT="${ANIM_RENDER_SLOTS_OUT:-$ROOT/pc_port/build_native/anim_render_packed_slots}"
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

src = Path("src/slus_006.64/system/temp1e.c").read_text()

def extract(sig):
    a = src.index(sig)
    b = src.index("\n#endif", a)
    return src[a:b].strip() + "\n"

for sig, name in (
    ("void func_80025718(u8* pEntry)", "func_80025718"),
    ("void func_800257F0(u8* pArg)", "func_800257F0"),
    ("void func_80025A88(u8* pEntry)", "func_80025A88"),
):
    body = extract(sig)
    assert "*(u8**)" not in body, f"{name} still has *(u8**)"
    assert "*(void**)" not in body, f"{name} still has *(void**)"
    assert "SpriteRenderAddress(*(u32*)" in body, f"{name} missing u32 slot load"

addr = """
static u8* SpriteRenderAddress(uintptr_t address) {
    if ((address & ~(uintptr_t)0x1FFFFFu) == 0x80000000u ||
        (address & ~(uintptr_t)0x1FFFFFu) == 0xA0000000u) {
        return (u8*)(uintptr_t)address;
    }
    return (u8*)address;
}
"""
bodies = (
    '#include "common.h"\n#include <stdint.h>\n'
    "typedef struct { short vx, vy, vz, pad; } SVECTOR;\n"
    "typedef struct { long vx, vy, vz, pad; } VECTOR;\n"
    "typedef struct { short m[3][3]; long t[3]; } MATRIX;\n\n"
    "extern void func_80022038(void* p);\n"
    "extern void func_8002C700(void* a, void* b, u_long* ot, s32 flags);\n"
    "extern void func_800B1F6C(void* a, void* b, u_long* ot, s32 c, s32 d, s32 e, s32 f);\n"
    "extern MATRIX* TransMatrix(MATRIX* m, VECTOR* t);\n"
    "extern MATRIX* CompMatrix(MATRIX* a, MATRIX* b, MATRIX* out);\n"
    "extern void SetRotMatrix(MATRIX* m);\n"
    "extern void SetTransMatrix(MATRIX* m);\n"
    "extern VECTOR* ApplyMatrix(MATRIX* m, SVECTOR* in, VECTOR* out);\n"
    "extern void PushMatrix(void);\n"
    "extern void PopMatrix(void);\n"
    "extern MATRIX* RotMatrix(void* r, MATRIX* m);\n"
    "extern MATRIX* MulMatrix0(MATRIX* a, MATRIX* b, MATRIX* out);\n"
    "extern void SetBackColor(long r, long g, long b);\n"
    "extern void SetColorMatrix(u32* m);\n"
    "extern void SetLightMatrix(MATRIX* m);\n"
    "extern void ReadGeomOffset(long* x, long* y);\n"
    "extern void SetGeomOffset(long x, long y);\n"
    "extern s32 g_GfxCurContext;\n"
    "extern s32 D_80050100;\n"
    "extern u_long* g_GfxCurOT;\n"
    "extern MATRIX D_8004FBB8;\n"
    "extern u16 D_8004FD80[];\n"
    "extern MATRIX D_8004FDA0;\n"
    + addr
    + extract("void func_80025718(u8* pEntry)")
    + extract("void func_800257F0(u8* pArg)")
    + extract("void func_80025A88(u8* pEntry)")
)
Path(sys.argv[1]).write_text(bodies)
print("ANIM RENDER SLOTS SOURCE PASS")
PY

SRC=src/slus_006.64/system/temp1e.c
python3 - "$SRC" <<'PY'
from pathlib import Path
import re, sys
src = Path(sys.argv[1]).read_text()
for name, start in (
    ("func_80025718", "void func_80025718(u8* pEntry)"),
    ("func_800257F0", "void func_800257F0(u8* pArg)"),
    ("func_80025A88", "void func_80025A88(u8* pEntry)"),
):
    a = src.index(start)
    b = src.index("\n#endif", a)
    body = src[a:b]
    assert "*(u8**)" not in body, f"{name} still has *(u8**)"
    assert "*(void**)" not in body, f"{name} still has *(void**)"
    assert "SpriteRenderAddress(*(u32*)" in body, f"{name} missing u32 slot load"
print("ANIM RENDER SLOTS CONTAINMENT PASS")
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
        pc_port/tests/anim_render_packed_slots_test.c \
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

# Mutant: restore the void** work-list load in func_80025718. It must fail
# because the 8-byte +0x4 load concatenates the planted callback.
python3 - "$OUT/bodies.c" "$OUT/bodies_mutant.c" <<'PY'
from pathlib import Path
import sys
text = Path(sys.argv[1]).read_text()
old = "u8* pSprite = SpriteRenderAddress(*(u32*)(pEntry + 0x04));"
new = "u8* pSprite = *(u8**)(pEntry + 0x04);"
if old not in text:
    raise SystemExit("fixed SpriteRenderAddress load not found for mutant")
Path(sys.argv[2]).write_text(text.replace(old, new, 1))
PY

set +e
"$CC" "${FLAGS[@]}" -O0 \
    -c pc_port/tests/anim_render_packed_slots_test.c -o "$OUT/mutant_test.o"
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
if ! grep -Eq 'sees SpriteData|FAIL:|Segmentation fault|POINTER' \
        "$OUT/mutant.stdout" "$OUT/mutant.stderr"; then
    if [[ "$mutant_rc" -lt 128 ]]; then
        echo "FAIL: void** mutant failed without a clobber or crash" >&2
        cat "$OUT/mutant.stdout" "$OUT/mutant.stderr" >&2
        exit 1
    fi
fi
echo "ANIM RENDER SLOTS negative control PASS host-width load rejected"
echo "ANIM RENDER SLOTS proofs: PASS"
