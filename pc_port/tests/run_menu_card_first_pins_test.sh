#!/usr/bin/env bash
# Pins func_801C9D34 (see menu_card_first_pins_test.c) and checks that the old
# pManager[0x4D8] destination is rejected.
set -euo pipefail
cd "$(dirname "$0")/../.."
ulimit -c 0
OUT=$(mktemp -d "${TMPDIR:-/tmp}/menu-card-first.XXXXXX")
echo "OUTPUT $OUT"
SRC="${CARD_FIRST_SOURCE:-src/menu/main/misc.c}"
awk '/^s32 func_801C9D34\(/ {body=1} body {print} /^}/ {body=0}' "$SRC" > "$OUT/first.inc"
test "$(grep -c '^s32 func_801C9D34' "$OUT/first.inc")" -eq 1
BASE=(-std=gnu17 -DXENO_PC_PORT -DSKIP_ASM -D_LANGUAGE_C -DUSE_EXTENDED_PRIM_POINTERS=0 -I"$OUT" -Ipc_port/src -Ipc_port/include_shim -Iinclude -Ipc_port/extern/PsyCross/include -Ipc_port/extern/PsyCross/include/psx)
"${CC:-gcc}" "${BASE[@]}" -O0 pc_port/tests/menu_card_first_pins_test.c -o "$OUT/good"
timeout 30s "$OUT/good"
cp "$OUT/first.inc" "$OUT/good.inc"
sed -i 's/g_Menu->unk4CC\[0xC\] = 2;/((u8*)g_Menu->pManager)[0x4D8] = 2;/' "$OUT/first.inc"
! cmp -s "$OUT/good.inc" "$OUT/first.inc"
"${CC:-gcc}" "${BASE[@]}" -O0 pc_port/tests/menu_card_first_pins_test.c -o "$OUT/mgr"
if timeout 30s "$OUT/mgr" >/dev/null 2>&1; then echo "FAIL mutant manager-dest accepted"; exit 1; fi
echo "REJECTED manager-dest"
