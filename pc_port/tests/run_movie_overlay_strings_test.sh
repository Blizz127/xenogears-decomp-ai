#!/usr/bin/env bash
# Movie overlay strings read from guest RAM (binary-guard task 6be4993b0e61, E1).
#
# src/movie/main.c's developer-menu FontPrintf formats are MV_STR(vaddr, lit):
# in the port they read the movie overlay's own .rodata from guest RAM, where
# main_loop decompresses movie.bin before MovieMain runs (as on retail); the
# literal is not compiled in.  This checks that
#   1. every MV_STR address holds exactly its literal (NUL-terminated) in the
#      user's disc/movie.bin, and no FontPrintf passes a bare literal,
#   2. the overlay in guest RAM at MovieMain entry equals disc/movie.bin
#      (shipping boot -> state 6, dumped by XENO_MOVIE_OVERLAY_DUMP into a
#      private temp directory that is never written to the tree or to out/),
#   3. the built binary does not contain the menu strings.
# Negative control: a wrong MV_STR address must fail check 1.
#
#   run_movie_overlay_strings_test.sh [XENO_PORT]
set -euo pipefail
cd "$(dirname "$0")/../.."
BIN=${1:-pc_port/build_native/xeno-port}
DUMP=$(mktemp -d /tmp/movie_overlay_dump.XXXXXXXX)
trap 'rm -rf "$DUMP"' EXIT
XENO_MOVIE_OVERLAY_DUMP="$DUMP/overlay.bin" SDL_VIDEODRIVER=offscreen SDL_AUDIODRIVER=dummy \
    timeout 60 "$BIN" > "$DUMP/run.log" 2>&1 &
pid=$!
for _ in $(seq 1 120); do
    [ -s "$DUMP/overlay.bin" ] && break
    kill -0 "$pid" 2>/dev/null || break
    sleep 0.5
done
sleep 0.5
kill "$pid" 2>/dev/null || true
wait "$pid" 2>/dev/null || true
python3 - "$DUMP" "$BIN" <<'PY'
import re
import sys
from hashlib import sha256
from pathlib import Path
dump, binary = Path(sys.argv[1]), Path(sys.argv[2])
movie = Path('disc/movie.bin').read_bytes()
assert sha256(movie).hexdigest() == \
    '50e1a9d9e08b90eed0c2da1c289507e71cbf51749893a92f457ce79a59701f9a', 'disc/movie.bin'
src = Path('src/movie/main.c').read_text()
pairs = [(int(a, 16), eval(l)) for a, l in
         re.findall(r'MV_STR\(0x([0-9A-F]{8}), ("(?:[^"\\]|\\.)*")\)', src)]
def holds(addr, lit):
    raw = lit.encode('latin-1') + b'\0'
    o = addr - 0x8006FAF0
    return 0 <= o and movie[o:o + len(raw)] == raw
fails = 0
for addr, lit in pairs:
    if not holds(addr, lit):
        print('FAIL MV_STR 0x%08X does not hold %r' % (addr, lit)); fails += 1
bare = [l for l in src.splitlines() if 'FontPrintf(' in l and
        '"' in re.sub(r'MV_STR\(0x[0-9A-F]{8}, "(?:[^"\\]|\\.)*"\)', '', l)]
for l in bare:
    print('FAIL bare FontPrintf literal:', l.strip()); fails += 1
print('MV_STR addresses: %d checked against disc/movie.bin' % len(pairs))
addr, lit = pairs[0]
assert not holds(addr + 4, lit), 'negative control not detected'
print('negative control detected: MV_STR address +4')
ov = dump / 'overlay.bin'
data = ov.read_bytes() if ov.exists() else b''
if data != movie:
    print('FAIL guest-RAM overlay at MovieMain entry (%d bytes) != disc/movie.bin' % len(data))
    fails += 1
else:
    print('guest-RAM overlay at MovieMain entry == disc/movie.bin (%d bytes)' % len(data))
image = binary.read_bytes()
present = [lit for _, lit in pairs if len(lit) >= 8 and lit.encode('latin-1') + b'\0' in image]
for lit in present:
    print('FAIL string still in binary:', repr(lit)); fails += 1
print('MOVIE OVERLAY STRINGS %s (%d failures)' % ('PASS' if fails == 0 else 'FAIL', fails))
sys.exit(1 if fails else 0)
PY
