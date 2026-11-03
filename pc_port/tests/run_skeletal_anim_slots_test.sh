#!/usr/bin/env bash
# Runtime + source lock for the skeletal work-block pointer slots:
# FieldLoad publishes func_800303C8's return into pModel+0x14 as a u32, and
# func_800305D8 reads that packed control block with 4-byte loads.
set -euo pipefail
source "$(dirname "${BASH_SOURCE[0]}")/lib/sanitizer_preflight.sh"; ubsan_preflight
cd "$(dirname "$0")/../.."

OUT=${SKELETAL_ANIM_SLOTS_OUT:-pc_port/build_native/skeletal_anim_slots_test}
mkdir -p "$OUT"

MODEL_SRC=src/slus_006.64/system/temp2.c
FIELDLOAD_SRC=src/field/main/misc3.c

python3 - "$MODEL_SRC" "$FIELDLOAD_SRC" "$OUT" <<'PY'
from pathlib import Path
import sys

model = Path(sys.argv[1]).read_text()
field = Path(sys.argv[2]).read_text()
out = Path(sys.argv[3])

def extract(src, start, end_marker):
    a = src.index(start)
    b = src.index(end_marker, a)
    return src[a:b]

init = extract(model, "void* func_800303C8(u8* a0, s32 a1)", "\n#endif\n")
tick = extract(model, "void func_800305D8(u8* pAnimData)", "\n#endif\n")
(out / "bodies.c").write_text(
    '#include "common.h"\n#include <stdint.h>\n\n'
    "s32 func_8003014C(s32* pData);\n"
    "void func_800301C8(u8* pDst, u8* pSrc, s32 count, s16* pIndices);\n"
    "void func_80030228(u8* pVertices, s16* pDeltas, s32 count, s32 scale);\n"
    "void func_800302D4(u8* pNormals, s16* pDeltas, s32 count, s32 scale);\n"
    "void* HeapAlloc(u_int allocSize, u_int allocFlags);\n"
    "void HeapSetCurrentContentType(u_short contentTag);\n\n"
    + init + "\n" + tick
)
assert "*(u32*)(s3 + 0x0) = (u32)(uintptr_t)func_8003014C;" in init
assert "*(void**)(s3 + 0x0)" not in init
assert "(u8*)(uintptr_t)*(u32*)(pAnimData + 0x00)" in tick
assert "*(u8**)(pAnimData + 0x00)" not in tick
assert "pAnimInfo = func_800303C8(" in field
assert "*(u32*)((u8*)pModel + 0x14) = (u32)(uintptr_t)pAnimInfo;" in field
assert "*(s32*)((u8*)pModel + 0x14) = 0;" not in field
print("SKELETAL ANIM SLOTS SOURCE PASS")
PY

COMMON=(-include assert.h -std=gnu17 -fno-pie -fno-builtin -DXENO_PC_PORT
    -DSKIP_ASM -D_LANGUAGE_C -DUSE_EXTENDED_PRIM_POINTERS=0
    -ffunction-sections -fdata-sections
    -Ipc_port/include_shim -Iinclude -Ipc_port/src
    -Ipc_port/extern/PsyCross/include -Ipc_port/extern/PsyCross/include/psx)

for mode in ${SKELETAL_ANIM_SLOTS_MODES:-O0 O2 UBSan}; do
    flags=(-"$mode")
    if [ "$mode" = UBSan ]; then
        flags=(-O1 -fsanitize=undefined -fno-sanitize-recover=all)
    fi
    gcc "${COMMON[@]}" "${flags[@]}" -Wall -Wextra -Werror -c \
        pc_port/tests/skeletal_anim_slots_test.c -o "$OUT/$mode.test.o"
    gcc "${COMMON[@]}" "${flags[@]}" -w -c "$OUT/bodies.c" -o "$OUT/$mode.body.o"
    gcc -no-pie "${flags[@]}" -Wl,--gc-sections \
        "$OUT/$mode.test.o" "$OUT/$mode.body.o" -o "$OUT/$mode.test"
    "$OUT/$mode.test"
done

python3 - "$OUT/bodies.c" "$OUT" <<'PY'
from pathlib import Path
import sys
body = Path(sys.argv[1]).read_text()
out = Path(sys.argv[2])
old = "(u8*)(uintptr_t)*(u32*)(pAnimData + 0x00)"
new = "*(u8**)(pAnimData + 0x00)"
assert old in body
(out / "wide-pointer.c").write_text(body.replace(old, new, 1))
print("SKELETAL ANIM SLOTS MUTANT WRITTEN")
PY

gcc "${COMMON[@]}" -O2 -w -c "$OUT/wide-pointer.c" -o "$OUT/wide-pointer.body.o"
gcc -no-pie -O2 -Wl,--gc-sections \
    "$OUT/O2.test.o" "$OUT/wide-pointer.body.o" -o "$OUT/wide-pointer.test"
if "$OUT/wide-pointer.test" > "$OUT/wide-pointer.log" 2>&1; then
    echo "SKELETAL ANIM SLOTS mutant survived" >&2
    cat "$OUT/wide-pointer.log" >&2
    exit 1
fi
echo "SKELETAL ANIM SLOTS negative control PASS host-width load rejected"
