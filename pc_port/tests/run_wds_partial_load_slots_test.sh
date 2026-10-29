#!/usr/bin/env bash
set -euo pipefail
source "$(dirname "${BASH_SOURCE[0]}")/lib/sanitizer_preflight.sh"; ubsan_preflight

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
OUT="${WDS_SLOTS_OUT:-$ROOT/pc_port/build_native/wds_partial_load_slots}"
CC="${CC:-gcc}"
mkdir -p "$OUT"
cd "$ROOT"

FLAGS=(-std=gnu17 -Wall -Wextra -Werror -Wno-unused-parameter
       -m64 -fno-pie -no-pie -fno-builtin
       -DXENO_PC_PORT -DSKIP_ASM -D_LANGUAGE_C
       -Ipc_port/include_shim -Iinclude -Ipc_port/src
       -Ipc_port/extern/PsyCross/include -Ipc_port/extern/PsyCross/include/psx)

# Extract the port arm of func_800380D0 so the live-append test compiles the
# shipped body (and the void** mutant can swap just that body).
python3 - "$OUT/func_800380D0.c" <<'PY'
import pathlib, re, sys
src = pathlib.Path("src/slus_006.64/system/sound.c").read_text()
m = re.search(
    r"#else\n// Loads part of a WDS file.*?void\* func_800380D0\(.*?\n\}\n#endif",
    src,
    re.S,
)
assert m, "func_800380D0 port arm not found"
pathlib.Path(sys.argv[1]).write_text(
    '#include "common.h"\n'
    '#include "system/sound.h"\n'
    '#include "psyq/libapi.h"\n\n'
    'extern SoundWDSEntry* SoundFindWdsEntry(int targetID);\n'
    'extern void SoundHandleError(s32 errorId);\n'
    'extern u32 SoundSpuMemoryAllocateWDS(void* pWdsFile, int mode);\n'
    'extern void SoundWdsSetTransferParamters(int transferAddress, int numBytesToTransfer);\n'
    'extern void* func_80039024(s32 size);\n'
    'extern int SoundSpuMemoryFreeBlock(int targetAddress);\n'
    'extern void SoundHeapSetBlockMemory(void* pBlockMemory, void* pSrc, int size);\n\n'
    + m.group(0)[5:-6]
)
PY

compile_run() {
    local label="$1"
    local opt="$2"
    local sanitize="${3:-0}"
    local body="${4:-$OUT/func_800380D0.c}"
    local san=()
    if [[ "$sanitize" == 1 ]]; then
        san=(-fsanitize=undefined -fno-sanitize-recover=all)
    fi
    "$CC" "${FLAGS[@]}" "$opt" "${san[@]}" \
        pc_port/tests/wds_partial_load_slots_test.c \
        -o "$OUT/${label}_layout"
    "$OUT/${label}_layout" | tee "$OUT/${label}_layout.stdout"
    "$CC" "${FLAGS[@]}" "$opt" "${san[@]}" \
        pc_port/tests/wds_partial_load_func_test.c \
        "$body" \
        -o "$OUT/${label}_func"
    "$OUT/${label}_func" | tee "$OUT/${label}_func.stdout"
}

compile_run O0 -O0
compile_run O2 -O2
compile_run UBSan -O2 1

# Mutant: restore the void** list walk. It must fail the live-append test
# because the 8-byte pNext load concatenates the planted +0x30 word.
python3 - "$OUT/func_800380D0.c" "$OUT/func_800380D0_mutant.c" <<'PY'
from pathlib import Path
import sys
text = Path(sys.argv[1]).read_text()
old = """    {
        SoundPsxAddress* pList = (SoundPsxAddress*)&g_SoundWdsLinkedList;
        while (SOUND_PSX_TO_PTR(SoundWDSEntry, *pList) != NULL) {
            pList = &SOUND_PSX_TO_PTR(SoundWDSEntry, *pList)->pNext;
        }
        *pList = SOUND_PTR_TO_PSX(pEntry);
    }"""
new = """    {
        void** ppList = (void**)&g_SoundWdsLinkedList;
        while (*ppList != NULL) {
            ppList = (void**)((u8*)*ppList + 0x2C);
        }
        *ppList = pEntry;
    }"""
if old not in text:
    raise SystemExit("fixed SoundPsxAddress walk not found for mutant")
Path(sys.argv[2]).write_text(text.replace(old, new, 1))
PY

set +e
"$CC" "${FLAGS[@]}" -O0 \
    pc_port/tests/wds_partial_load_func_test.c \
    "$OUT/func_800380D0_mutant.c" \
    -o "$OUT/mutant_func"
"$OUT/mutant_func" >"$OUT/mutant.stdout" 2>"$OUT/mutant.stderr"
mutant_rc=$?
set -e
if [[ "$mutant_rc" -eq 0 ]]; then
    echo "FAIL: void** mutant was accepted" >&2
    cat "$OUT/mutant.stdout" >&2
    exit 1
fi
# The 8-byte pNext load concatenates +0x30 and walks off the list: either
# the harness reports the clobber, or the walk SIGSEGVs. Both reject it.
if ! grep -Eq 'pNext=|payload clobbered|did not register' \
        "$OUT/mutant.stdout" "$OUT/mutant.stderr"; then
    if [[ "$mutant_rc" -lt 128 ]]; then
        echo "FAIL: void** mutant failed without a clobber or crash" >&2
        cat "$OUT/mutant.stdout" "$OUT/mutant.stderr" >&2
        exit 1
    fi
fi

echo "WDS partial-load slot proofs: PASS"
