#!/usr/bin/env bash
# Packed-slot pin for func_800BF2B8: SpriteData +0x7C is a 4-byte pointer;
# a host u32** load concatenates the scale halfwords at +0x80/+0x82.
set -euo pipefail
source "$(dirname "${BASH_SOURCE[0]}")/lib/sanitizer_preflight.sh"; ubsan_preflight
ulimit -c 0

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
BUILD_DIR="${PARTY_SLOT_ARCHIVE_BUILD_DIR:-$ROOT/pc_port/build_native/battle_party_slot_archive_packed_slots}"
CC="${CC:-gcc}"
mkdir -p "$BUILD_DIR"
cd "$ROOT"

BASE=(-std=gnu17 -fno-pie -no-pie -fno-builtin -fno-stack-protector
      -DXENO_PC_PORT -DXENO_BATTLE_OVERLAY_HOST_BODIES -DSKIP_ASM
      -D_LANGUAGE_C -DUSE_EXTENDED_PRIM_POINTERS=0
      -include assert.h -fpermissive -ffunction-sections -fdata-sections)
TEST_BASE=(-std=gnu17 -fno-pie -no-pie -fno-builtin -fno-stack-protector
           -DXENO_PC_PORT -DSKIP_ASM -D_LANGUAGE_C
           -DUSE_EXTENDED_PRIM_POINTERS=0
           -ffunction-sections -fdata-sections)
INC=(-Ipc_port/include_shim -Iinclude -Ipc_port/extern/PsyCross/include
     -Ipc_port/extern/PsyCross/include/psx -Ipc_port/src)

build_and_run() {
    local name="$1"
    shift
    "$CC" "${BASE[@]}" "${INC[@]}" -w -fno-inline "$@" \
        -fno-sanitize=object-size -c src/battle/mainc126.c \
        -o "$BUILD_DIR/$name.body.o"
    "$CC" "${TEST_BASE[@]}" "${INC[@]}" -Wall -Wextra -Werror "$@" \
        -c pc_port/tests/battle_party_slot_archive_packed_slots_test.c \
        -o "$BUILD_DIR/$name.test.o"
    "$CC" -fno-pie -no-pie "$@" "$BUILD_DIR/$name.test.o" "$BUILD_DIR/$name.body.o" \
        -Wl,--gc-sections -o "$BUILD_DIR/$name"
    "$BUILD_DIR/$name" >"$BUILD_DIR/$name.stdout" 2>"$BUILD_DIR/$name.stderr"
    test ! -s "$BUILD_DIR/$name.stderr"
    rg -q 'PARTY SLOT ARCHIVE packed \+0x7C slot PASS' "$BUILD_DIR/$name.stdout"
}

build_and_run O0 -O0 -g
build_and_run O2 -O2
build_and_run UBSan -O2 -g -fsanitize=undefined -fno-sanitize-recover=all

python3 - "$BUILD_DIR/mutant.c" <<'PY'
import sys
src = open('src/battle/mainc126.c').read()
old = '    file = BG_U32(*(u32*)(task + 0x7C));'
new = '    file = **(u32**)(task + 0x7C);'
assert src.count(old) == 1, 'mutant load anchor not unique'
open(sys.argv[1], 'w').write(src.replace(old, new, 1))
PY

"$CC" "${BASE[@]}" "${INC[@]}" -w -fno-inline -O2 \
    -fno-sanitize=object-size -c "$BUILD_DIR/mutant.c" \
    -o "$BUILD_DIR/mutant.body.o"
"$CC" "${TEST_BASE[@]}" "${INC[@]}" -Wall -Wextra -Werror -O2 \
    -c pc_port/tests/battle_party_slot_archive_packed_slots_test.c \
    -o "$BUILD_DIR/mutant.test.o"
"$CC" -fno-pie -no-pie -O2 "$BUILD_DIR/mutant.test.o" "$BUILD_DIR/mutant.body.o" \
    -Wl,--gc-sections -o "$BUILD_DIR/mutant"
set +e
"$BUILD_DIR/mutant" >"$BUILD_DIR/mutant.stdout" 2>"$BUILD_DIR/mutant.stderr"
mutant_rc=$?
set -e
if [ "$mutant_rc" -eq 0 ]; then
    echo 'PARTY SLOT ARCHIVE mutant survived' >&2
    exit 1
fi
# Host-width load at +0x7C concatenates scale 0x2000 at +0x82 into a
# non-canonical pointer and crashes on the file-id deref.
if [ "$mutant_rc" -ge 128 ] || rg -q 'ASSERTION party.archive' "$BUILD_DIR/mutant.stderr"; then
    echo 'PARTY SLOT ARCHIVE negative control PASS host-width load rejected'
else
    echo "PARTY SLOT ARCHIVE mutant failed unexpectedly rc=$mutant_rc" >&2
    cat "$BUILD_DIR/mutant.stderr" >&2
    exit 1
fi
