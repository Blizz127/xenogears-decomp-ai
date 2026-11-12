#!/usr/bin/env bash
# Pins func_800ACB90: FieldLoadTIMWithClut y=0x100, and the 0x200-byte fill
# stays inside the allocation. Mutants: y=0, and the old i*4+0x1FC overflow.
set -euo pipefail
cd "$(dirname "$0")/../.."
ulimit -c 0
OUT=$(mktemp -d "${TMPDIR:-/tmp}/field-acb90-tim-y.XXXXXX")
echo "OUTPUT $OUT"
SRC="${ACB90_SOURCE:-src/field/main/misc9.c}"
awk '/^void func_800ACB90\(/ {body=1} body {print} body && /^}/ {exit}' "$SRC" > "$OUT/acb90.inc"
test "$(grep -c '^void func_800ACB90(' "$OUT/acb90.inc")" -eq 1
grep -F -q 'FieldLoadTIMWithClut(D_800AF784, 0x380, 0x100, 0, 0x1FF, 0, 0)' "$OUT/acb90.inc"
grep -F -q '*(s32*)(pBuf + i * 4) = -1;' "$OUT/acb90.inc"
BASE=(-std=gnu17 -I"$OUT")
"${CC:-gcc}" "${BASE[@]}" -O0 -g pc_port/tests/field_acb90_tim_y_pins_test.c -o "$OUT/good"
timeout 30s "$OUT/good"
cp "$OUT/acb90.inc" "$OUT/good.inc"

reject_mutant() {
    local name="$1"
    cp "$OUT/good.inc" "$OUT/acb90.inc"
    case "$name" in
        y-at-origin)
            sed -i 's/FieldLoadTIMWithClut(D_800AF784, 0x380, 0x100, 0, 0x1FF, 0, 0);/FieldLoadTIMWithClut(D_800AF784, 0x380, 0, 0, 0x1FF, 0, 0);/' "$OUT/acb90.inc"
            ;;
        fill-overflow)
            sed -i 's/\*(s32\*)(pBuf + i \* 4) = -1;/*(s32*)(pBuf + i * 4 + 0x1FC) = -1;/' "$OUT/acb90.inc"
            ;;
        *)
            echo "unknown mutant $name" >&2
            exit 1
            ;;
    esac
    ! cmp -s "$OUT/good.inc" "$OUT/acb90.inc"
    "${CC:-gcc}" "${BASE[@]}" -O0 -g pc_port/tests/field_acb90_tim_y_pins_test.c -o "$OUT/$name"
    if timeout 30s "$OUT/$name" >/dev/null 2>&1; then
        echo "FAIL mutant $name accepted"
        exit 1
    fi
    echo "REJECTED $name"
}

reject_mutant y-at-origin
reject_mutant fill-overflow
