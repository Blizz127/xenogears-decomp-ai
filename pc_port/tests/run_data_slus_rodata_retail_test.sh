#!/usr/bin/env bash
# data_slus_rodata — retail-byte certificate for pc_port/src/data_slus_rodata.c.
#
# Verifies the disc hash and main-exe .rodata boundaries, then builds the
# generated-data test at O0/O2/UBSan and runs five negative controls that must
# each fail a named assertion:
#   zero        - re-zero D_800181C8 inside the generated data file
#   truncate    - shrink D_80018258 from 82 to 40 bytes
#   flip        - flip the first byte of D_80018974
#   mode        - re-zero the D_80010000 build/mode word (was 0xFFFFFFFF)
#   rodataend   - move PSX_EXE_RODATA_END/PSX_EXE_TEXT_START back by 4 bytes
#
# UBSan: this host's gcc cannot link libubsan (missing /usr/lib64/libubsan.so.1.0.0),
# so fall back to clang -fsanitize=undefined. If neither compiler can build the
# regime the runner fails; it never silently skips it.
set -euo pipefail
ulimit -c 0
cd "$(dirname "$0")/../.."
# Artifacts go to $TMPDIR, not pc_port/build_native/, so a concurrent
# build_port.sh cannot race with this test.
out=$(mktemp -d "${TMPDIR:-/tmp}/data_slus_rodata.XXXXXXXX")
echo "data_slus_rodata retail artifacts: $out"

python3 - <<'PY'
from pathlib import Path
from hashlib import sha256
import struct
b = Path('disc/SLUS_006.64').read_bytes()
assert sha256(b).hexdigest() == \
    'dc0b2dd786203d4cce5927c5a3fc85a18f39a3f7406078860076ebb0bbae7119', 'disc sha256'
assert b[:8] == b'PS-X EXE', 'PS-X EXE magic'
assert len(b) == 0x4A000, hex(len(b))
t_addr, t_size = struct.unpack_from('<II', b, 0x18)
assert t_addr == 0x80010000, hex(t_addr)
assert t_size == 0x49800, hex(t_size)

def off(v):
    return 0x800 + v - 0x80010000

# .rodata section boundaries from config/slus_006.64.yaml, corroborated by the
# bytes: the three head objects must tile [0x80010000, 0x80018084).
assert b[off(0x80010000):off(0x80010000) + 4] == b'\xff\xff\xff\xff', 'mode word'
assert off(0x80010000) + 4 == off(0x80010004)
assert off(0x80010004) + 0x8000 == off(0x80018004)
assert off(0x80018004) + 0x80 == off(0x80018084)
# The first .text subsegment starts at 0x80019524 (config: [0x9D24, asm]).
assert off(0x80019524) == 0x9D24, hex(off(0x80019524))
# Every covered symbol is non-zero in retail (the audit's mismatch criterion).
for v, n in [(0x800180FC, 0xB1), (0x800181B8, 0x10), (0x800181C8, 0x14),
             (0x800181DC, 0x0C), (0x800181E8, 0x0A), (0x800181F4, 0x0A),
             (0x80018200, 0x1B), (0x8001821C, 0x02), (0x80018220, 0x04),
             (0x80018224, 0x13), (0x80018238, 0x1D), (0x80018258, 0x52),
             (0x8001833C, 0x14), (0x80018350, 0x12), (0x80018364, 0x13),
             (0x80018378, 0x15), (0x80018390, 0x13), (0x80018644, 0x20),
             (0x800188EC, 0x04), (0x800188F0, 0x04), (0x80018944, 0x10),
             (0x80018954, 0x0D), (0x80018964, 0x0E), (0x80018974, 0x0A)]:
    assert any(b[off(v):off(v) + n]), hex(v)
print('data_slus_rodata disc/section certificate: PASS')
PY

python3 - "$out" <<'PY'
import re
import sys
from pathlib import Path
out = Path(sys.argv[1])
src = Path('pc_port/src/data_slus_rodata.c').read_text()
pat = re.compile(r'(unsigned char (\w+)\[(\d+)\][^=]*= \{)(.*?)(\};)', re.S)


def body_bytes(body):
    return re.findall(r'0x[0-9a-fA-F]{2}', body)


def replace(name, mutate):
    pat_n = re.compile(r'(unsigned char %s\[(\d+)\][^=]*= \{)(.*?)(\};)' % name,
                       re.S)
    m = pat_n.search(src)
    assert m, name
    vals = body_bytes(m.group(3))
    decl_size = int(m.group(2))
    assert len(vals) == decl_size, (name, len(vals), decl_size)
    return pat_n.sub(lambda mm: mutate(mm.group(1), vals, mm.group(4)), src, count=1)


def as_lines(vals, per=16):
    return '\n' + '\n'.join(
        '    ' + ','.join(vals[i:i + per]) + ',' for i in range(0, len(vals), per)) + '\n'


# Mutant 1: re-zero D_800181C8.
(out / 'mutant_zero.c').write_text(
    replace('D_800181C8', lambda head, vals, tail:
            head + as_lines(['0x00'] * len(vals)) + tail))

# Mutant 2: truncate D_80018258 from 82 to 40 bytes.
def trunc(head, vals, tail):
    head2 = re.sub(r'\[82\]', '[40]', head)
    assert head2 != head
    return head2 + as_lines(vals[:40]) + tail


(out / 'mutant_truncate.c').write_text(replace('D_80018258', trunc))

# Mutant 3: flip the first byte of D_80018974.
def flip(head, vals, tail):
    v = list(vals)
    assert v[0] == '0x25'
    v[0] = '0x24'
    return head + as_lines(v) + tail


(out / 'mutant_flip.c').write_text(replace('D_80018974', flip))

# Mutant 4: re-zero the D_80010000 build/mode word.
(out / 'mutant_mode.c').write_text(
    replace('D_80010000', lambda head, vals, tail:
            head + as_lines(['0x00'] * len(vals)) + tail))

# Mutant 5: revert the loader rodata boundary by one word.
hdr = Path('pc_port/src/psx_memory.h').read_text()
hdr2 = hdr.replace('#define PSX_EXE_RODATA_END    0x80019524u',
                   '#define PSX_EXE_RODATA_END    0x80019520u')
hdr2 = hdr2.replace('#define PSX_EXE_TEXT_START    0x80019524u',
                    '#define PSX_EXE_TEXT_START    0x80019520u')
assert hdr2 != hdr and '0x80019520u' in hdr2
mdir = out / 'mutant_header'
mdir.mkdir()
(mdir / 'psx_memory.h').write_text(hdr2)
(mdir / 'data_slus_rodata.c').write_text(src)
print('mutants written to', out)
PY

base=(-std=gnu17 -m64 -DXENO_PC_PORT -DSKIP_ASM -D_LANGUAGE_C
      -Ipc_port/src -Ipc_port/include_shim -Iinclude
      -Ipc_port/extern/PsyCross/include -Ipc_port/extern/PsyCross/include/psx)
test_src=pc_port/tests/data_slus_rodata_retail_test.c

# UBSan compiler selection: prefer gcc, fall back to clang, never skip.
ubsan_cc=
if gcc "${base[@]}" -O2 -fsanitize=undefined -fno-sanitize-recover=all \
        "$test_src" -o "$out/ubsan_probe_gcc" >/dev/null 2>&1; then
    ubsan_cc=gcc
elif command -v clang >/dev/null 2>&1 && \
     clang "${base[@]}" -O2 -fsanitize=undefined -fno-sanitize-recover=all \
        "$test_src" -o "$out/ubsan_probe_clang" >/dev/null 2>&1; then
    ubsan_cc=clang
    echo "  NOTE: gcc cannot link UBSan here (missing libubsan.so.1.0.0); using clang"
else
    echo "ERROR: no compiler can build the UBSan regime; refusing to skip silently" >&2
    exit 1
fi

echo "== production regimes =="
for opt in O0 O2 UBSan; do
    cc=gcc
    flags=(-"$opt")
    if [[ "$opt" == UBSan ]]; then
        cc="$ubsan_cc"
        flags=(-O2 -fsanitize=undefined -fno-sanitize-recover=all)
    fi
    "$cc" "${base[@]}" "${flags[@]}" -Wall -Wextra -Werror "$test_src" -o "$out/$opt"
    "$out/$opt"
    echo "  $opt ($cc): PASS"
done

run_mutant() {
    local name="$1"; shift
    gcc "${PRE_INC[@]}" "${base[@]}" "$@" "$test_src" -o "$out/$name"
    local rc=0
    "$out/$name" >"$out/$name.log" 2>&1 || rc=$?
    if [[ "$rc" -eq 0 ]]; then
        echo "NEGATIVE CONTROL $name PASSED (should have failed)" >&2
        exit 1
    fi
    if ! grep -q 'FAIL ' "$out/$name.log"; then
        echo "NEGATIVE CONTROL $name failed without an assertion: rc=$rc" >&2
        cat "$out/$name.log" >&2
        exit 1
    fi
    echo "negative control detected: $name (rc=$rc, $(grep -m1 'FAIL ' "$out/$name.log"))"
}

echo "== mutants (each must be rejected) =="
PRE_INC=()
run_mutant zero     -O0 -DDATA_SLUS_RODATA_SRC="\"$out/mutant_zero.c\""
run_mutant truncate -O0 -DDATA_SLUS_RODATA_SRC="\"$out/mutant_truncate.c\""
run_mutant flip     -O0 -DDATA_SLUS_RODATA_SRC="\"$out/mutant_flip.c\""
run_mutant mode     -O0 -DDATA_SLUS_RODATA_SRC="\"$out/mutant_mode.c\""
# The mutant header must win the quoted-include search before -Ipc_port/src.
PRE_INC=(-I"$out/mutant_header")
run_mutant rodataend -O0 \
    -DDATA_SLUS_RODATA_SRC="\"$out/mutant_header/data_slus_rodata.c\""

echo "data_slus_rodata retail certificate PASS (5/5 negative controls rejected)"
