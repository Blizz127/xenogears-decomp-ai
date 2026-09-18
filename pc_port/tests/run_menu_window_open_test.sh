#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/../.."
ulimit -c 0
OUT=$(mktemp -d "${TMPDIR:-/tmp}/menu-window-open.XXXXXX")
echo "OUTPUT $OUT"
python3 - <<'PY'
import pathlib,re
b=pathlib.Path('disc/menu.bin').read_bytes()
s=pathlib.Path('asm/menu/matchings/main/misc/func_801D3B00.s').read_text()
for a,h in re.findall(r'/\* \w+ ([0-9A-F]{8}) ([0-9A-F]{8}) \*/',s):
    off=int(a,16)-0x801c5000
    assert b[off:off+4]==bytes.fromhex(h), a
print('PASS retail window instruction pin')
PY
awk '/^void func_801D3B00\(void\)/ {body=1} body {print} /^}/ {body=0}' "${WINDOW_OPEN_SOURCE:-src/menu/main/misc.c}" > "$OUT/window.inc"
BASE=(-std=gnu17 -DXENO_PC_PORT -DSKIP_ASM -D_LANGUAGE_C -DUSE_EXTENDED_PRIM_POINTERS=0 -I"$OUT" -Ipc_port/include_shim -Iinclude -Ipc_port/extern/PsyCross/include -Ipc_port/extern/PsyCross/include/psx)
for mode in O0 O2 UBSan; do
    flags=(-"$mode")
    if [[ "$mode" == UBSan ]]; then flags=(-O1 -fsanitize=undefined -fno-sanitize-recover=all); fi
    "${CC:-gcc}" "${BASE[@]}" "${flags[@]}" pc_port/tests/menu_window_open_test.c -o "$OUT/$mode"
    "$OUT/$mode"
done
cp "$OUT/window.inc" "$OUT/good.inc"
for mutant in never-ready early-ready swapped-dimensions wrong-index; do
    case "$mutant" in
        never-ready) sed 's/scrollX + scrollY/(scrollX | scrollY)/' "$OUT/good.inc" > "$OUT/window.inc" ;;
        early-ready) sed 's/scrollX + scrollY == 2/scrollX + scrollY != 0/' "$OUT/good.inc" > "$OUT/window.inc" ;;
        swapped-dimensions) sed 's/x, y, curW2, h,/x, y, h, curW2,/' "$OUT/good.inc" > "$OUT/window.inc" ;;
        wrong-index) sed 's/func_801D4D1C(pData->index,/func_801D4D1C(i,/' "$OUT/good.inc" > "$OUT/window.inc" ;;
    esac
    "${CC:-gcc}" "${BASE[@]}" -O2 pc_port/tests/menu_window_open_test.c -o "$OUT/$mutant"
    if "$OUT/$mutant" > "$OUT/$mutant.log" 2>&1; then echo "FAIL mutant survived: $mutant"; exit 1; fi
    grep -q 'Assertion' "$OUT/$mutant.log"
    echo "REJECTED $mutant"
done
cp "$OUT/good.inc" "$OUT/window.inc"
