#!/usr/bin/env bash
# Pins Status work-buffer slot 0x438 (see menu_status_slot_pins_test.c) and
# checks that a raw g_Menu+0x438 pointer load is rejected.
set -euo pipefail
cd "$(dirname "$0")/../.."
ulimit -c 0
OUT=$(mktemp -d "${TMPDIR:-/tmp}/menu-status-slot.XXXXXX")
echo "OUTPUT $OUT"
SRC="${STATUS_SLOT_SOURCE:-src/menu/main/misc.c}"
awk '/^void func_801E1398\(/ {body=1} body {print} /^}/ {if(body){body=0}}' "$SRC" > "$OUT/e1398.inc"
awk '/^void func_801D1914\(/ {body=1} body {print} /^}/ {if(body){body=0}}' "$SRC" > "$OUT/d1914.inc"
test "$(grep -c '^void func_801E1398' "$OUT/e1398.inc")" -eq 1
test "$(grep -c '^void func_801D1914' "$OUT/d1914.inc")" -eq 1
grep -q 'HeapFree(MENU_RAW(0x438))' "$OUT/e1398.inc"
grep -q 'MenuRawPointer(0x438)' "$OUT/d1914.inc"
BASE=(-std=gnu17 -DXENO_PC_PORT -DSKIP_ASM -D_LANGUAGE_C -DUSE_EXTENDED_PRIM_POINTERS=0 -I"$OUT" -Ipc_port/src -Ipc_port/include_shim -Iinclude -Ipc_port/extern/PsyCross/include -Ipc_port/extern/PsyCross/include/psx)
"${CC:-gcc}" "${BASE[@]}" -O0 pc_port/tests/menu_status_slot_pins_test.c -o "$OUT/good"
timeout 30s "$OUT/good"
cp "$OUT/e1398.inc" "$OUT/good-e1398.inc"
cp "$OUT/d1914.inc" "$OUT/good-d1914.inc"
python3 - "$OUT/e1398.inc" <<'PY'
import sys
p=sys.argv[1]; s=open(p).read()
a='    HeapFree(MENU_RAW(0x438));'
assert a in s, s
s=s.replace(a,'    HeapFree(*(void**)((u8*)pMenu + 0x438));')
open(p,'w').write(s)
PY
! cmp -s "$OUT/good-e1398.inc" "$OUT/e1398.inc"
"${CC:-gcc}" "${BASE[@]}" -O0 pc_port/tests/menu_status_slot_pins_test.c -o "$OUT/raw-free"
if timeout 30s "$OUT/raw-free" >/dev/null 2>&1; then echo "FAIL mutant raw-free accepted"; exit 1; fi
echo "REJECTED raw-free"
cp "$OUT/good-e1398.inc" "$OUT/e1398.inc"
python3 - "$OUT/d1914.inc" <<'PY'
import sys
p=sys.argv[1]; s=open(p).read()
assert 'MenuRawPointer(0x438)' in s
s=s.replace('MenuRawPointer(0x438)','*(u8**)((u8*)g_Menu + 0x438)')
open(p,'w').write(s)
PY
! cmp -s "$OUT/good-d1914.inc" "$OUT/d1914.inc"
"${CC:-gcc}" "${BASE[@]}" -O0 pc_port/tests/menu_status_slot_pins_test.c -o "$OUT/raw-draw"
if timeout 30s "$OUT/raw-draw" >/dev/null 2>&1; then echo "FAIL mutant raw-draw accepted"; exit 1; fi
echo "REJECTED raw-draw"
