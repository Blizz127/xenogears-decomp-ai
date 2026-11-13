#!/usr/bin/env bash
# Packed-slot pin for func_8003ADCC: AudioManager +0x4 is a 4-byte
# SoundPsxAddress; a host void** store there clobbers the seq pointer at +0x8.
set -euo pipefail
source "$(dirname "${BASH_SOURCE[0]}")/lib/sanitizer_preflight.sh"; ubsan_preflight

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
BUILD_DIR="${SOUND_SNAPSHOT_SLOTS_BUILD_DIR:-$ROOT/pc_port/build_native/sound_manager_snapshot_slots}"
CC="${CC:-gcc}"
mkdir -p "$BUILD_DIR"
cd "$ROOT"

weaken_source() {
    local out="$1"
    python3 - "$out" <<'PY'
import sys
src = open('src/slus_006.64/system/sound.c').read()
for sig in (
    'void* SoundHeapAllocate(u32 allocSize) {',
    'void SoundHeapSetBlockMemory(void* pBlockMemory, void* pSrc, int size) {',
):
    line = '__attribute__((weak)) ' + sig
    assert src.count(sig) == 1, 'signature not unique: %s' % sig
    src = src.replace(sig, line, 1)
open(sys.argv[1], 'w').write(src)
PY
}

weaken_source "$BUILD_DIR/sound_weak.c"

BASE=(-std=gnu17 -fno-pie -no-pie -fno-builtin -fno-stack-protector
      -DXENO_PC_PORT -DSKIP_ASM -D_LANGUAGE_C -DUSE_EXTENDED_PRIM_POINTERS=0
      -include assert.h -fpermissive -ffunction-sections -fdata-sections)
TEST_BASE=(-std=gnu17 -fno-pie -no-pie -fno-builtin -fno-stack-protector
           -ffunction-sections -fdata-sections)
INC=(-Ipc_port/include_shim -Iinclude -Ipc_port/extern/PsyCross/include
     -Ipc_port/extern/PsyCross/include/psx -Ipc_port/src)

build_and_run() {
    local name="$1"
    local src="$2"
    shift 2
    "$CC" "${BASE[@]}" "${INC[@]}" -w -fno-inline "$@" \
        -fno-sanitize=object-size -c "$src" \
        -o "$BUILD_DIR/$name.sound.o"
    "$CC" "${TEST_BASE[@]}" "${INC[@]}" -Wall -Wextra -Werror "$@" \
        -c pc_port/tests/sound_manager_snapshot_slots_test.c \
        -o "$BUILD_DIR/$name.test.o"
    "$CC" -fno-pie -no-pie "$@" "$BUILD_DIR/$name.test.o" "$BUILD_DIR/$name.sound.o" \
        -Wl,--gc-sections -o "$BUILD_DIR/$name"
    "$BUILD_DIR/$name" >"$BUILD_DIR/$name.stdout" 2>"$BUILD_DIR/$name.stderr"
    test ! -s "$BUILD_DIR/$name.stderr"
    rg -q 'SOUND MANAGER SNAPSHOT packed \+0x4 slot PASS' "$BUILD_DIR/$name.stdout"
}

build_and_run O0 "$BUILD_DIR/sound_weak.c" -O0 -g
build_and_run O2 "$BUILD_DIR/sound_weak.c" -O2
build_and_run UBSan "$BUILD_DIR/sound_weak.c" -O2 -g -fsanitize=undefined -fno-sanitize-recover=all

python3 - "$BUILD_DIR/sound_mutant.c" <<'PY'
import sys
src = open('src/slus_006.64/system/sound.c').read()
for sig in (
    'void* SoundHeapAllocate(u32 allocSize) {',
    'void SoundHeapSetBlockMemory(void* pBlockMemory, void* pSrc, int size) {',
):
    src = src.replace(sig, '__attribute__((weak)) ' + sig, 1)
old = '''    if (manager->unk_Manager_0x4 == 0) {
        pAlloc = SoundHeapAllocate(size);
    } else {
        pAlloc = SOUND_PSX_TO_PTR(AudioManager, manager->unk_Manager_0x4);
    }'''
new = '''    if (*(void**)((u8*)pManager + 4) == NULL) {
        pAlloc = SoundHeapAllocate(size);
    } else {
        pAlloc = *(void**)((u8*)pManager + 4);
    }'''
assert src.count(old) == 1, 'mutant load anchor not unique'
src = src.replace(old, new, 1)
old = '    manager->unk_Manager_0x4 = SOUND_PTR_TO_PSX(pAlloc);'
new = '    *(void**)((u8*)pManager + 4) = pAlloc;'
assert src.count(old) == 1, 'mutant store anchor not unique'
src = src.replace(old, new, 1)
open(sys.argv[1], 'w').write(src)
PY

"$CC" "${BASE[@]}" "${INC[@]}" -w -fno-inline -O2 \
    -fno-sanitize=object-size -c "$BUILD_DIR/sound_mutant.c" \
    -o "$BUILD_DIR/mutant.sound.o"
"$CC" "${TEST_BASE[@]}" "${INC[@]}" -Wall -Wextra -Werror -O2 \
    -c pc_port/tests/sound_manager_snapshot_slots_test.c \
    -o "$BUILD_DIR/mutant.test.o"
"$CC" -fno-pie -no-pie -O2 "$BUILD_DIR/mutant.test.o" "$BUILD_DIR/mutant.sound.o" \
    -Wl,--gc-sections -o "$BUILD_DIR/mutant"
set +e
"$BUILD_DIR/mutant" >"$BUILD_DIR/mutant.stdout" 2>"$BUILD_DIR/mutant.stderr"
mutant_rc=$?
set -e
if [ "$mutant_rc" -eq 0 ]; then
    echo 'SOUND MANAGER SNAPSHOT mutant survived' >&2
    exit 1
fi
# Host-width load at +4 concatenates the seq-file word at +8 into a wild
# pointer (NULL backup + nonzero seq → non-NULL 8-byte value) and crashes
# on the copy, or the later canary check fires if the store only clobbers.
if [ "$mutant_rc" -ge 128 ] || rg -q 'ASSERTION sound.snapshot' "$BUILD_DIR/mutant.stderr"; then
    echo 'SOUND MANAGER SNAPSHOT negative control PASS host-width store rejected'
else
    echo "SOUND MANAGER SNAPSHOT mutant failed unexpectedly rc=$mutant_rc" >&2
    cat "$BUILD_DIR/mutant.stderr" >&2
    exit 1
fi
