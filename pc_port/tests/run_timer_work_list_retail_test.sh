#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/../.."
OUT=pc_port/build_native/timer_work_list_retail_test
mkdir -p "$OUT"
read -r retail_sha _ < <(dd if=disc/SLUS_006.64 bs=1 \
    skip=$((0x8001cc18 - 0x8000f800)) count=$((0x25c)) status=none | sha256sum)
if [ "$retail_sha" != 73e431046377a0795e3627cc44af06c84574a20c3b25032ecd6df2015519697d ]; then
    echo "TIMER WORK LIST RETAIL SLICE MISMATCH" >&2
    exit 1
fi
read -r lookup_sha _ < <(dd if=disc/SLUS_006.64 bs=1 \
    skip=$((0x8001d0a4 - 0x8000f800)) count=$((0x68)) status=none | sha256sum)
if [ "$lookup_sha" != 4d2e3d0559a80d25efb325eadf0c64edacc7fabd24e3147a3ade1cfd7c0f1090 ]; then
    echo "TIMER LOOKUP RETAIL SLICE MISMATCH" >&2
    exit 1
fi
COMMON=(-std=gnu17 -fno-pie -fno-builtin -DXENO_PC_PORT -DSKIP_ASM -D_LANGUAGE_C
    -DUSE_EXTENDED_PRIM_POINTERS=0 -ffunction-sections -fdata-sections
    -Ipc_port/include_shim -Iinclude -Ipc_port/src
    -Ipc_port/extern/PsyCross/include -Ipc_port/extern/PsyCross/include/psx)
CC="${CC:-gcc}"
for opt in O0 O2 UBSan; do
    flags=(-"$opt")
    if [ "$opt" = UBSan ]; then flags=(-O1 -fsanitize=undefined -fno-sanitize-recover=all); fi
    gcc "${COMMON[@]}" "${flags[@]}" -Wall -Wextra -Werror \
        -Wno-stringop-overflow \
        -c pc_port/tests/timer_work_list_retail_test.c -o "$OUT/$opt.test.o"
    gcc "${COMMON[@]}" "${flags[@]}" -Wall -Wextra -Werror \
        -c pc_port/src/work_list_port.c -o "$OUT/$opt.native.o"
    gcc "${COMMON[@]}" "${flags[@]}" -Wall -Wextra -Werror \
        -c pc_port/src/battle_mips_adapter.c -o "$OUT/$opt.cpu.o"
    linker="$CC"
    if [ "$opt" = UBSan ] && command -v clang >/dev/null 2>&1; then linker=clang; fi
    "$linker" -no-pie "${flags[@]}" -Wl,--gc-sections "$OUT/$opt.test.o" "$OUT/$opt.native.o" \
        "$OUT/$opt.cpu.o" -o "$OUT/$opt.test"
    "$OUT/$opt.test"
done
