#!/usr/bin/env bash
set -euo pipefail

root="$(cd "$(dirname "$0")/../.." && pwd)"
out="$(mktemp)"
trap 'rm -f "$out"' EXIT

gcc -std=gnu17 -fpermissive -DXENO_PC_PORT -DSKIP_ASM -D_LANGUAGE_C \
    -DUSE_EXTENDED_PRIM_POINTERS=0 -include assert.h \
    -ffunction-sections -fdata-sections \
    -I"$root/pc_port/include_shim" -I"$root/include" \
    -I"$root/pc_port/extern/PsyCross/include" \
    -I"$root/pc_port/extern/PsyCross/include/psx" \
    "$root/pc_port/tests/archive_stream_pump_test.c" \
    -Wl,--gc-sections -no-pie -o "$out"

"$out"
echo 'archive stream pump: PASS'
