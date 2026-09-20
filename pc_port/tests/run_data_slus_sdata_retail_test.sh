#!/usr/bin/env bash
# data_slus_sdata — retail-byte certificate for pc_port/src/data_slus_sdata.c.
#
# Verifies the disc hash and main-exe section boundaries, then builds the
# generated-data test at O0/O2/UBSan and runs three negative controls that must
# each fail a named assertion:
#   zero     - re-zero D_8004FBB8 inside the generated data file
#   truncate - shrink D_8004FBB8 from 32 to 16 bytes
#   sdataend - move PSX_EXE_SDATA_END / PSX_EXE_SBSS_START back to 0x800576E4
set -euo pipefail
ulimit -c 0
cd "$(dirname "$0")/../.."
# Artifacts go to $TMPDIR, not pc_port/build_native/, so a concurrent
# build_port.sh cannot race with this test.
out=$(mktemp -d "${TMPDIR:-/tmp}/data_slus_sdata.XXXXXXXX")
echo "data_slus_sdata retail artifacts: $out"

python3 - <<'PY'
from pathlib import Path
from hashlib import sha256
b = Path('disc/SLUS_006.64').read_bytes()
assert sha256(b).hexdigest() == \
    'dc0b2dd786203d4cce5927c5a3fc85a18f39a3f7406078860076ebb0bbae7119', 'disc sha256'
assert b[:8] == b'PS-X EXE', 'PS-X EXE magic'
assert len(b) == 0x4A000, hex(len(b))
import struct
t_addr, t_size = struct.unpack_from('<II', b, 0x18)
assert t_addr == 0x80010000, hex(t_addr)
assert t_size == 0x49800, hex(t_size)
# Section boundaries from config/slus_006.64.yaml, corroborated by the bytes.
assert b[0x800 + (0x800592BB - 0x80010000)] != 0, 'last non-zero byte moved'
assert all(x == 0 for x in b[0x800 + (0x800592BC - 0x80010000):
                             0x800 + (0x80059800 - 0x80010000)]), 'retail .sbss non-zero'
assert all(x == 0 for x in b[0x800 + (0x800592BC - 0x80010000):
                             0x800 + (0x800592C0 - 0x80010000)]), '.data not zero'
# .sdata tail that the old 0x800576E4 loader end silently dropped.
assert any(x != 0 for x in b[0x800 + (0x800576E4 - 0x80010000):
                             0x800 + (0x800592BC - 0x80010000)]), 'empty sdata tail'
print('data_slus_sdata disc/section certificate: PASS')
PY

python3 - "$out" <<'PY'
import re
import sys
from pathlib import Path
out = Path(sys.argv[1])
src = Path('pc_port/src/data_slus_sdata.c').read_text()
pat = re.compile(r'(unsigned char D_8004FBB8\[32\][^=]*= \{)(.*?)(\};)', re.S)
m = pat.search(src)
assert m, 'D_8004FBB8 initializer not found'
vals = re.findall(r'0x[0-9a-fA-F]{2}', m.group(2))
assert len(vals) == 32, len(vals)

# Mutant 1: re-zero D_8004FBB8.
zeros = '\n' + '\n'.join('    ' + ','.join('0x00' for _ in range(16)) + ','
                         for _ in range(2)) + '\n'
(out / 'mutant_zero.c').write_text(
    pat.sub(lambda mm: mm.group(1) + zeros + mm.group(3), src, count=1))

# Mutant 2: truncate D_8004FBB8 to its first 16 bytes.
small = '\n    ' + ','.join(vals[:16]) + ',\n'
trunc = pat.sub(lambda mm: 'unsigned char D_8004FBB8[16] = {' + small + '};',
                src, count=1)
assert 'D_8004FBB8[16]' in trunc
(out / 'mutant_truncate.c').write_text(trunc)

# Mutant 3: revert the loader boundary constants.
hdr = Path('pc_port/src/psx_memory.h').read_text()
hdr2 = hdr.replace('#define PSX_EXE_SDATA_END     0x800592C0u',
                   '#define PSX_EXE_SDATA_END     0x800576E4u')
hdr2 = hdr2.replace('#define PSX_EXE_SBSS_START    0x800592C0u',
                    '#define PSX_EXE_SBSS_START    0x800576E4u')
assert hdr2 != hdr and '0x800576E4u' in hdr2
mdir = out / 'mutant_header'
mdir.mkdir()
(mdir / 'psx_memory.h').write_text(hdr2)
(mdir / 'data_slus_sdata.c').write_text(src)
print('mutants written to', out)
PY

base=(-std=gnu17 -m64 -DXENO_PC_PORT -DSKIP_ASM -D_LANGUAGE_C
      -Ipc_port/src -Ipc_port/include_shim -Iinclude
      -Ipc_port/extern/PsyCross/include -Ipc_port/extern/PsyCross/include/psx)
test_src=pc_port/tests/data_slus_sdata_retail_test.c

echo "== production regimes =="
have_ubsan=0
if gcc "${base[@]}" -O2 -fsanitize=undefined -fno-sanitize-recover=all \
        "$test_src" -o "$out/ubsan_probe" >/dev/null 2>&1; then
    have_ubsan=1
else
    echo "  WARNING: UBSan runtime unavailable (missing libubsan); skipping UBSan regime"
fi
for opt in O0 O2 UBSan; do
    if [[ "$opt" == UBSan && "$have_ubsan" -eq 0 ]]; then
        continue
    fi
    flags=(-"$opt")
    if [[ "$opt" == UBSan ]]; then
        flags=(-O2 -fsanitize=undefined -fno-sanitize-recover=all)
    fi
    gcc "${base[@]}" "${flags[@]}" -Wall -Wextra -Werror "$test_src" -o "$out/$opt"
    "$out/$opt"
    echo "  $opt: PASS"
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
run_mutant zero     -O0 -DDATA_SLUS_SDATA_SRC="\"$out/mutant_zero.c\""
run_mutant truncate -O0 -DDATA_SLUS_SDATA_SRC="\"$out/mutant_truncate.c\""
# The mutant header must win the quoted-include search before -Ipc_port/src.
PRE_INC=(-I"$out/mutant_header")
run_mutant sdataend -O0 \
    -DDATA_SLUS_SDATA_SRC="\"$out/mutant_header/data_slus_sdata.c\""

echo "data_slus_sdata retail certificate PASS (3/3 negative controls rejected)"
