#!/usr/bin/env bash
set -euo pipefail
source "$(dirname "${BASH_SOURCE[0]}")/lib/sanitizer_preflight.sh"; ubsan_preflight
cd "$(dirname "$0")/../.."
OUT=pc_port/build_native/field_line_scroll_slots_test
mkdir -p "$OUT"

# The two handlers and the line-scroll BSS block are the units under test.
# Extract them so this suite does not need the PsyCross vendor tree.
python3 - "$OUT" <<'PY'
from pathlib import Path
import sys
out = Path(sys.argv[1])

src = Path("src/field/main/misc11.c").read_text()
def port_body(name):
    start = src.index(f"INCLUDE_ASM(\"asm/field/nonmatchings/main/misc11\", {name});")
    else_tok = src.index("#else", start)
    end = src.index("#endif /* XENO_PC_PORT */", else_tok)
    body = src[else_tok + len("#else"):end]
    if f"void {name}(void)" not in body:
        raise SystemExit(f"missing {name} port body")
    return body

(out / "line_scroll_handlers.c").write_text(
    '#include <stdint.h>\n'
    'typedef int s32;\n'
    'typedef short s16;\n'
    'typedef unsigned short u16;\n'
    'typedef unsigned int u32;\n'
    'typedef unsigned int u_int;\n'
    'typedef signed char s8;\n'
    'typedef unsigned char u8;\n'
    'extern s16 D_800AFEA8;\n'
    'extern s16 D_800AFFAC[];\n'
    'extern s32 D_800ADB8C;\n'
    'typedef struct { u16 scriptInstructionPointer; } ActorData;\n'
    'extern ActorData* g_FieldScriptVMCurActor;\n'
    'extern int FieldScriptVMGetInstructionArgument(int);\n'
    'extern int FieldScriptVMGetArgument(int);\n'
    'extern void* HeapAlloc(u_int, u_int);\n'
    'extern void* GfxLineScrollInitialize(void*, u16, u16, u16, s16, s16, u16, u16, s8*);\n'
    + port_body("func_80092148")
    + port_body("func_800921E8")
)

data = Path("pc_port/src/data_field.c").read_text()
if "g_FieldBss_800AFE9C[0x150]" not in data:
    raise SystemExit("data_field.c line-scroll block is not 0x150")
if "FIELD_BSS_ALIAS(D_800AFFAC, g_FieldBss_800AFE9C, 0x110)" not in data:
    raise SystemExit("data_field.c missing D_800AFFAC alias")

(out / "line_scroll_bss.c").write_text(
    "unsigned char g_FieldBss_800AFE9C[0x150] __attribute__((aligned(8)));\n"
    "unsigned int D_800AFB20[4];\n"
    'asm(".globl D_800AFEA8\\n.set D_800AFEA8, g_FieldBss_800AFE9C + 0x00C");\n'
    'asm(".globl D_800AFFAC\\n.set D_800AFFAC, g_FieldBss_800AFE9C + 0x110");\n'
)
PY

common=(-std=gnu17 -fno-pie -ffunction-sections -fdata-sections
    -Wall -Wextra -Werror)
for opt in O0 O2 UBSan; do
    flags=(-"$opt")
    if [ "$opt" = UBSan ]; then flags=(-O1 -fsanitize=undefined -fno-sanitize-recover=all); fi
    gcc "${common[@]}" "${flags[@]}" -c \
        pc_port/tests/field_line_scroll_slots_test.c -o "$OUT/$opt.test.o"
    # Handlers index packed tables through the s16 D_800AFEA8 alias. GCC/UBSan
    # treat that alias as a 2-byte object; the stores are in-bounds of the
    # 0x150-byte owner. Keep the body unsanitized and suppress the bounds note.
    body_opt=(-O2)
    [ "$opt" = O0 ] && body_opt=(-O0)
    gcc -std=gnu17 -fno-pie -ffunction-sections -fdata-sections -Wno-array-bounds \
        "${body_opt[@]}" -c \
        "$OUT/line_scroll_handlers.c" -o "$OUT/$opt.body.o"
    gcc "${common[@]}" "${flags[@]}" -c \
        "$OUT/line_scroll_bss.c" -o "$OUT/$opt.data.o"
    gcc -no-pie "${flags[@]}" -Wl,--gc-sections "$OUT/$opt.test.o" \
        "$OUT/$opt.body.o" "$OUT/$opt.data.o" -o "$OUT/$opt.test"
    "$OUT/$opt.test"
done

python3 - "$OUT" <<'PY'
from pathlib import Path
import sys
out = Path(sys.argv[1])
src = Path("src/field/main/misc11.c").read_text()
# Recreate the pre-fix host-width stores.
mut = src.replace(
    "    u32* pBufs;\n    u32* pLineScrolls;\n    void* pBuf;\n    void* pLineScroll;",
    "    void** pBufs;\n    void** pLineScrolls;\n    void* pBuf;\n    void* pLineScroll;",
).replace(
    "    pBufs = (u32*)((u8*)&D_800AFEA8 + 0x84);",
    "    pBufs = (void**)((u8*)&D_800AFEA8 + 0x84);",
).replace(
    "    pLineScrolls = (u32*)((u8*)&D_800AFEA8 + 0x4);",
    "    pLineScrolls = (void**)((u8*)&D_800AFEA8 + 0x4);",
).replace(
    "    pBufs[D_800AFEA8] = (u32)(uintptr_t)pBuf;",
    "    pBufs[D_800AFEA8] = pBuf;",
).replace(
    "    pLineScrolls[D_800AFEA8] = (u32)(uintptr_t)pLineScroll;",
    "    pLineScrolls[D_800AFEA8] = pLineScroll;",
)
start = mut.index('INCLUDE_ASM("asm/field/nonmatchings/main/misc11", func_800921E8);')
else_tok = mut.index("#else", start)
end = mut.index("#endif /* XENO_PC_PORT */", else_tok)
body = mut[else_tok + len("#else"):end]
(out / "wide-pointer.c").write_text(
    Path(out / "line_scroll_handlers.c").read_text().split("void func_800921E8")[0]
    + body
)
PY
gcc "${common[@]}" -O2 -Wno-array-bounds -c "$OUT/wide-pointer.c" -o "$OUT/wide-pointer.o"
gcc -no-pie -Wl,--gc-sections "$OUT/wide-pointer.o" "$OUT/O2.test.o" \
    "$OUT/O2.data.o" -o "$OUT/wide-pointer.test"
if "$OUT/wide-pointer.test" > "$OUT/wide-pointer.log" 2>&1; then
    echo 'LINE SCROLL SLOT mutant survived' >&2; exit 1
fi
rg -q 'LINE SCROLL SLOT FAIL' "$OUT/wide-pointer.log"
echo 'LINE SCROLL SLOT negative control PASS host-width store rejected'
