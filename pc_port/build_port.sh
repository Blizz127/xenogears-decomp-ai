#!/usr/bin/env bash
#
# Phase 1 link driver for the Xenogears native PC port.
#
# Strategy (boot-path-driven): compile every game translation unit that compiles
# in port mode, link them against PsyCross, discover the undefined references
# (everything not yet decompiled), auto-generate logging stubs for exactly those,
# and link the whole xeno-port executable. Running it then reveals the first
# missing function on the live execution path (the oracle).
#
# Run inside a Linux toolchain env (distrobox/container) with:
#   gcc g++ cmake make pkg-config binutils python3 libsdl2-dev libopenal-dev libgl1-mesa-dev
#
set -uo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"
PSX="pc_port/extern/PsyCross"
OUT="pc_port/build_native"
OBJ="$OUT/obj"
mkdir -p "$OBJ"

INC="-Ipc_port/include_shim -Iinclude -I$PSX/include -I$PSX/include/psx"
GFLAGS="-DXENO_PC_PORT -DSKIP_ASM -D_LANGUAGE_C -include assert.h -w -O0 -g -m64 -fno-builtin"

echo "==> [1/5] Building PsyCross (libpsycross.a) via CMake"
cmake -S pc_port -B pc_port/build -DCMAKE_BUILD_TYPE=Debug >/dev/null
cmake --build pc_port/build --target psycross -j"$(nproc)" >/dev/null
PSYLIB="$(find pc_port/build -name 'libpsycross.a' | head -1)"
echo "    libpsycross.a: $PSYLIB"

echo "==> [2/5] Compiling game translation units in port mode"
compiled=0; skipped=0; SKIPPED=""
GAME_OBJS=()
while IFS= read -r f; do
    o="$OBJ/$(echo "$f" | tr '/' '_').o"
    if gcc -c "$f" $GFLAGS $INC -o "$o" 2>/dev/null; then
        GAME_OBJS+=("$o"); compiled=$((compiled+1))
    else
        skipped=$((skipped+1)); SKIPPED="$SKIPPED $f"
    fi
done < <(find src -name '*.c' | grep -v '/psyq/' | sort)
echo "    compiled=$compiled  skipped=$skipped"
[ -n "$SKIPPED" ] && echo "    skipped (will be stubbed):$SKIPPED"

echo "==> [2b/5] Compiling port-only game overrides (boot-path functions)"
if gcc -c pc_port/src/game_overrides.c $GFLAGS -Ipc_port/src -o "$OBJ/game_overrides.o" 2>/dev/null; then
    GAME_OBJS+=("$OBJ/game_overrides.o")
    echo "    game_overrides.o ok"
fi

echo "==> [3/5] Compiling port entry point"
gcc -c pc_port/src/port_main.c $GFLAGS -Ipc_port/src -I"$PSX/include" -o "$OBJ/port_main.o" 2>/dev/null

LIBS="$(pkg-config --libs sdl2 openal 2>/dev/null) -lGL -lm -lpthread -ldl"
LINK=(gcc -m64 "$OBJ/port_main.o" "${GAME_OBJS[@]}" "$PSYLIB" $LIBS -o "$OUT/xeno-port")

echo "==> [4/5] Trial link to discover undefined references"
"${LINK[@]}" 2> "$OUT/link1.err"
grep -oE "undefined reference to \`[A-Za-z0-9_]+'" "$OUT/link1.err" \
    | sed -E "s/.*\`([A-Za-z0-9_]+)'/\1/" | sort -u > "$OUT/undef.txt"
echo "    undefined symbols to stub: $(wc -l < "$OUT/undef.txt")"

if [ -s "$OUT/undef.txt" ]; then
    ELFS=()
    for e in build/out/slus_006.64.elf build/out/field.elf build/out/member_change_menu.elf build/out/shop_menu.elf; do
        [ -f "$e" ] && ELFS+=(--elf "$e")
    done
    python3 tools/scripts/gen_port_stubs.py "${ELFS[@]}" --undefined "$OUT/undef.txt" --out "$OUT/stubs.c"
    gcc -c "$OUT/stubs.c" -O0 -g -o "$OBJ/stubs.o"
fi

echo "==> [5/5] Final link"
gcc -m64 "$OBJ/port_main.o" "${GAME_OBJS[@]}" "$OBJ/stubs.o" "$PSYLIB" $LIBS -o "$OUT/xeno-port" 2> "$OUT/link2.err"
if [ -f "$OUT/xeno-port" ] && [ ! -s "$OUT/link2.err" ]; then
    echo "    LINK OK -> $OUT/xeno-port"
else
    echo "    LINK incomplete; remaining errors:"
    grep -oE "undefined reference to \`[A-Za-z0-9_]+'|multiple definition of \`[A-Za-z0-9_]+'" "$OUT/link2.err" | sort | uniq -c | sort -rn | head -20
fi
