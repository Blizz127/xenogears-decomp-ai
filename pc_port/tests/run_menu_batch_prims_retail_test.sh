#!/usr/bin/env bash
# Menu batch-init builders (func_801D7C3C callees) vs retail MIPS from disc/menu.bin.
set -euo pipefail
cd "$(dirname "$0")/../.."
source pc_port/tests/lib/menu_retail_build.sh
test "$(sha256sum disc/menu.bin | cut -d' ' -f1)" = 82f84a24ac1fd1979754e1cc9c861405fb59ce95dd78db00a76f00c8f1bd177d
OUT=$(mktemp -d "${TMPDIR:-/tmp}/menu-batch-prims.XXXXXX")
python3 pc_port/tests/lib/gen_struct_mirror.py SystemMenu 1E98 > "$OUT/system_menu_mirror.inc"
HX_DEFSYMS=(g_GameState=0x8006D634 g_SystemPalette1=0x800595D4 g_SystemPalette2=0x80059414
  D_801EA578=0x801EA578 D_801EA5C4=0x801EA5C4 D_801E9D38=0x801E9D38 D_801E9D3C=0x801E9D3C)
run_mode() {
  local mode=$1 src=$2 exe=$3
  hx_build_misc "$mode" "$exe.misc.o" "$src"
  gcc "${HX_COMMON[@]}" $(hx_mode_flags "$mode") -I"$OUT" -Wall -Wextra -Werror -Wno-unused-function -c pc_port/tests/menu_batch_prims_retail_test.c -o "$exe.test.o"
  gcc "${HX_COMMON[@]}" $(hx_mode_flags "$mode") -c pc_port/src/battle_mips_adapter.c -o "$exe.cpu.o"
  hx_link "$mode" "$exe" "$exe.test.o" "$exe.misc.o" "$exe.cpu.o"
}
SRC=${MENU_SRC:-src/menu/main/misc.c}
for mode in O0 O2 UBSan; do
  run_mode "$mode" "$SRC" "$OUT/$mode"
  "$OUT/$mode" disc/menu.bin
done
mutate() { # name, python replace pair
  python3 - "$SRC" "$OUT/$1.c" "$2" "$3" <<'PY'
import sys, pathlib
s = pathlib.Path(sys.argv[1]).read_text(); a, b = sys.argv[3], sys.argv[4]
assert s.count(a) >= 1, a
pathlib.Path(sys.argv[2]).write_text(s.replace(a, b, 1))
PY
  run_mode O2 "$OUT/$1.c" "$OUT/$1"
  if "$OUT/$1" disc/menu.bin > "$OUT/$1.log" 2>&1 || ! grep -q '^HX FAIL' "$OUT/$1.log"; then
    echo "MUTANT SURVIVED: $1"; exit 1; fi
  echo "MUTANT REJECTED: $1 ($(grep -m1 '^HX FAIL' "$OUT/$1.log" | cut -c1-90))"
}
mutate 5ED4-parity 'parity = g_Menu->pManager->currentCharacterIDs[slot] & 1;' 'parity = g_Menu->pManager->currentCharacterIDs[slot] & 2;'
mutate 5ED4-width 'width = (u16)(variant * 0x18 + 0x48);' 'width = (u16)(variant * 0x10 + 0x48);'
mutate 5ED4-shift '(u16)(D_801E9D38 - shift)' '(u16)(D_801E9D38 + shift)'
echo "PASS menu batch prims (O0/O2/UBSan + mutants)"
