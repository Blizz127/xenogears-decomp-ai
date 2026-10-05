#!/usr/bin/env bash
# Retail differential for the memory-card Save/Load/Copy/Delete flows and the
# card-access indicator in the production src/menu/main/misc.c TU against
# the pinned disc/menu.bin bytes on the port MIPS interpreter.  See
# pc_port/tests/menu_card_flow_retail_test.c.  Regimes O0 / O2 / UBSan (shift-base
# ENABLED), then one deliberately-wrong mutant per body.
set -euo pipefail
source "$(dirname "${BASH_SOURCE[0]}")/lib/sanitizer_preflight.sh"; ubsan_preflight
cd "$(dirname "$0")/../.."
ulimit -c 0
CC=${CC:-gcc}
SOURCE=${MENU_CARD_FLOW_SOURCE:-src/menu/main/misc.c}
OUT=${MENU_CARD_FLOW_OUT:-$(mktemp -d "${TMPDIR:-/tmp}/xeno-menu-card-flow.XXXXXXXX")}
mkdir -p "$OUT/inc"
echo "MENU CARD FLOW RETAIL OUTPUT $OUT"
sha=$(sha256sum disc/menu.bin | awk '{print $1}')
[ "$sha" = 82f84a24ac1fd1979754e1cc9c861405fb59ce95dd78db00a76f00c8f1bd177d ] \
  || { echo "ERROR: disc/menu.bin SHA-256 mismatch: $sha" >&2; exit 1; }
# include/common.h defines offsetof() as the (T*)0->m idiom, which UBSan flags;
# the test-only wrapper below (first on the include path) swaps in the builtin.
cat > "$OUT/inc/common.h" <<'H'
#include_next "common.h"
#undef offsetof
#define offsetof(s, m) __builtin_offsetof(s, m)
H
COMMON=(-I"$OUT/inc" -include assert.h -std=gnu17 -fno-pie -fno-builtin
        -DXENO_PC_PORT -DXENO_FIELD_OBJECT_OVERLAY -DSKIP_ASM -D_LANGUAGE_C
        -DUSE_EXTENDED_PRIM_POINTERS=0 -ffunction-sections -fdata-sections -fpermissive -w
        -Ipc_port/include_shim -Iinclude -Ipc_port/src
        -Ipc_port/extern/PsyCross/include -Ipc_port/extern/PsyCross/include/psx)
# Host struct offsets come from the production headers, never hand-copied.
"$CC" "${COMMON[@]}" -no-pie -O0 pc_port/tests/menu_card_flow_layout_gen.c -o "$OUT/layout_gen"
"$OUT/layout_gen" > "$OUT/inc/menu_retail_layout.h"
build_source() { # src obj flags...  (callee spies defined by the test are weakened at link)
  local src=$1 obj=$2; shift 2
  # -fPIC makes every global function interposable: the optimiser may not assume the TU's own callee bodies
  # (which the test replaces with spies) when it propagates return ranges or constants.
  "$CC" "${COMMON[@]}" -iquote "$(dirname "$SOURCE")" "$@" -fPIC -fno-inline -fno-ipa-cp -fno-ipa-sra -fno-ipa-icf -fno-partial-inlining -c "$src" -o "$obj"
}
HOST=(-std=gnu17 -fno-pie -DXENO_PC_PORT -ffunction-sections -fdata-sections -I"$OUT/inc" -Ipc_port/src)
TESTC=pc_port/tests/menu_card_flow_retail_test.c
link_one() { # source-obj test-obj cpu-obj outbin flags...
  local src=$1 tst=$2 cpu=$3 bin=$4; shift 4
  local weak=()
  # every symbol the test defines strongly and the TU also defines is a spy/alias: weaken the TU's copy
  while read -r sym; do weak+=(--weaken-symbol="$sym"); done < <(
    comm -12 <(nm --defined-only -g "$tst" | awk '{print $3}' | sort -u) \
             <(nm --defined-only -g "$src" | awk '{print $3}' | sort -u))
  [ ${#weak[@]} -eq 0 ] || objcopy "${weak[@]}" "$src"
  "$CC" -no-pie "$@" -Wl,--gc-sections "$src" "$tst" "$cpu" -o "$bin"
}
for mode in ${MENU_CARD_FLOW_MODES:-O0 O2 UBSan}; do
  flags=(-"$mode")
  if [ "$mode" = UBSan ]; then flags=(-O1 -fsanitize=undefined -fno-sanitize-recover=all); fi # shift-base stays ENABLED
  build_source "$SOURCE" "$OUT/$mode.source.o" "${flags[@]}"
  "$CC" "${HOST[@]}" "${flags[@]}" -Wall -Wextra -Werror -c "$TESTC" -o "$OUT/$mode.test.o"
  "$CC" "${HOST[@]}" "${flags[@]}" -c pc_port/src/battle_mips_adapter.c -o "$OUT/$mode.cpu.o"
  link_one "$OUT/$mode.source.o" "$OUT/$mode.test.o" "$OUT/$mode.cpu.o" "$OUT/$mode" "${flags[@]}"
  "$OUT/$mode" disc/menu.bin ${MENU_CARD_FLOW_ONLY:-} > "$OUT/$mode.log" 2>&1 || { cat "$OUT/$mode.log"; exit 1; }
  cat "$OUT/$mode.log"
  # second binary: the same harness with the card scan (func_801C93A8) as the function under test
  "$CC" "${HOST[@]}" "${flags[@]}" -DFLOW_REAL_SCAN -Wall -Wextra -Werror -c "$TESTC" -o "$OUT/$mode.scan.test.o"
  link_one "$OUT/$mode.source.o" "$OUT/$mode.scan.test.o" "$OUT/$mode.cpu.o" "$OUT/$mode.scan" "${flags[@]}"
  "$OUT/$mode.scan" disc/menu.bin > "$OUT/$mode.scan.log" 2>&1 || { cat "$OUT/$mode.scan.log"; exit 1; }
  cat "$OUT/$mode.scan.log"
done
if [ -z "${MENU_CARD_FLOW_MODES:-}" ]; then
cmp "$OUT/O0.log" "$OUT/O2.log"
cmp "$OUT/O0.log" "$OUT/UBSan.log"
cmp "$OUT/O0.scan.log" "$OUT/O2.scan.log"
cmp "$OUT/O0.scan.log" "$OUT/UBSan.scan.log"
echo "MENU CARD FLOW RETAIL O0/O2/UBSan logs identical"
fi
if [ -n "${MENU_CARD_FLOW_MODES:-}" ]; then echo "MENU CARD FLOW RETAIL partial modes ${MENU_CARD_FLOW_MODES}: done (no controls)"; exit 0; fi

# Negative controls: each wrong body must make the differential go red.
python3 - "$SOURCE" "$OUT" <<'PY'
import sys, re
from pathlib import Path
s = Path(sys.argv[1]).read_text(); out = Path(sys.argv[2])
muts = [
 ('func_801C93A8', 'g_Menu->unk326 = 0x3C;', 'g_Menu->unk326 = 0x3D;'),
 ('func_801CAE08', '*(u32*)(MENU_RAW(0x44C) + 0x7B4) = 8;', '*(u32*)(MENU_RAW(0x44C) + 0x7B4) = 9;'),
 ('func_801CA8C0', 'card->unk4B98[0x1B] = (u8)(q + 0x4F);', 'card->unk4B98[0x1B] = (u8)(q + 0x50);'),
 ('func_801CD2AC', 'card[i * 0x5C + 0x58] = 0;', 'card[i * 0x5C + 0x58] = 1;'),
 ('func_801CB304', 'fd = open(path, 1);', 'fd = open(path, 2);'),
 ('func_801CBD90', 'chunk += 0x100;', 'chunk += 0x101;'),
 ('func_801CC6D8', 'rename(tmpName, dstName);', 'rename(dstName, tmpName);'),
]
ctl = []
for fn, old, new in muts:
    a = b = -1
    for m in re.finditer(r'\n(?:void|s32|u32|u8|int) %s\(' % fn, s):
        e = s.find('\n}\n', m.start())
        if s.find(old, m.start(), e) != -1:
            a, b = m.start(), e; break
    assert a != -1, (fn, 'body not found')
    body = s[a:b]
    assert body.count(old) == 1, (fn, old, body.count(old))
    (out / (fn + '.mut.c')).write_text(s[:a] + body.replace(old, new) + s[b:])
    ctl.append(fn)
(out / 'controls.txt').write_text('\n'.join(ctl) + '\n')
PY
while read -r fn; do
  build_source "$OUT/$fn.mut.c" "$OUT/$fn.source.o" -O2
  tobj="$OUT/O2.test.o"; [ "$fn" = func_801C93A8 ] && tobj="$OUT/O2.scan.test.o"
  link_one "$OUT/$fn.source.o" "$tobj" "$OUT/O2.cpu.o" "$OUT/$fn.mut" -O2
  rc=0
  "$OUT/$fn.mut" disc/menu.bin "$fn" > "$OUT/$fn.mut.log" 2>&1 || rc=$?
  if [ "$rc" != 1 ] || ! grep -q '^MENU CARD FLOW RETAIL FAIL ' "$OUT/$fn.mut.log"; then
    cat "$OUT/$fn.mut.log"; echo "MENU CARD FLOW RETAIL control failed to detect defect: $fn rc=$rc" >&2; exit 1
  fi
  echo "MENU CARD FLOW RETAIL CONTROL REJECTED $fn ($(grep -m1 '^MENU CARD FLOW RETAIL FAIL' "$OUT/$fn.mut.log" | cut -c1-110))"
done < "$OUT/controls.txt"
echo "MENU CARD FLOW RETAIL ALL PASS (differential O0/O2/UBSan + $(wc -l < "$OUT/controls.txt") negative controls)"
