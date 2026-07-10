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

# This must run in an environment with the toolchain + libs (the distrobox on
# Bazzite, NOT the immutable host). Fail fast with guidance if it's the wrong one.
for tool in cmake gcc pkg-config python3 ar; do
    command -v "$tool" >/dev/null 2>&1 || {
        echo "ERROR: '$tool' not found in this shell."
        echo "       Run inside the dev container:  distrobox enter xenogears-dev"
        echo "       (the host base system is immutable and lacks the toolchain.)"
        exit 1
    }
done
pkg-config --exists sdl2 2>/dev/null || {
    echo "ERROR: SDL2 development files not found."
    echo "       You are probably on the host. Run:  distrobox enter xenogears-dev"
    exit 1
}

INC="-Ipc_port/include_shim -Iinclude -I$PSX/include -I$PSX/include/psx"
# -std=gnu17: the game predates C23; gcc >= 15 defaults to C23 and rejects it.
# -fpermissive: gcc >= 14 promotes old-C constructs (implicit decls, int/pointer
#   conversions) to hard errors that -w can't silence; -fpermissive demotes them.
# -DUSE_EXTENDED_PRIM_POINTERS=0: use PsyCross's simple (non-PGXP) primitive
#   pipeline. The extended/PGXP path (default 1) routes 2D prims through a
#   perspective/offscreen path that doesn't render here, and its 24-byte SPRT
#   layout mismatches the game's hardcoded 0x10 strides (font.c). MUST match the
#   value PsyCross's lib is built with (see pc_port/CMakeLists.txt).
GFLAGS="-std=gnu17 -fpermissive -DXENO_PC_PORT -DSKIP_ASM -D_LANGUAGE_C -DUSE_EXTENDED_PRIM_POINTERS=0 -include assert.h -w -O0 -g -m64 -fno-builtin"

# PsyCross bugfix (idempotent; the vendored tree is gitignored so this patch lives
# here in the tracked build, not as an untracked source edit). The sprite/tile
# primitive switch masks the code with 0xFD, which clears the semi-transparency bit
# but NOT bit 0. Every case (0x60..0x7C) has bit 0 == 0, so any "shaded" sprite with
# bit 0 set (0x65/0x75/0x7D) -- which the game's font uses (primitiveCode 0x75/0x7D
# in font.c) -- falls through unrendered. 0xFC clears both flag bits so they map to
# their base case; shading/semi-trans are read from the unmasked code, so this is
# strictly additive (bit-0-clear codes are unaffected).
sed -i 's/switch (polyTag->code & 0xFD)/switch (polyTag->code \& 0xFC)/' \
    "$PSX/src/gpu/PsyX_GPU.cpp"

# PsyCross bugfix (idempotent). DrawSplit() routes a split off-screen (to the
# render-to-VRAM offscreen RT, which never reaches the presented framebuffer)
# whenever the PSX "draw to display area" flag dfe (GP0(E1) bit 10) is 0. But
# dfe=0 is just normal back-buffer drawing on PSX: Xenogears' font issues a
# DR_TPAGE with dfe=0 (font.c SetDrawTPage(...,0,0,...)), so every KernelMenu/UI
# glyph after it was drawn off-screen and never seen (the cursor/background use
# dfe=1 and rendered). This title doesn't use the render-to-VRAM path at this
# stage, so always draw on-screen.
sed -i 's/const bool drawOnScreen = split.drawenv.dfe;/const bool drawOnScreen = true; \/* XENO_PC_PORT: dfe=0 is normal back-buffer draw; see build_port.sh *\//' \
    "$PSX/src/gpu/PsyX_GPU.cpp"

# PsyCross bugfix (idempotent). DR_MODE/DR_ENV packets may carry a zero command
# terminator inside the declared packet length. ProcessDrawEnv used to return
# only the number of non-zero GP0(E*) commands processed before the terminator;
# ParsePrimitivesLinkedList then advanced into the terminator word and treated it
# as a fake zero-length primitive. The packet has still been consumed on PSX, so
# advance by the tag's declared length.
sed -i 's/return processedLongs;$/return polyTag->len; \/* XENO_PC_PORT: consume full DR_MODE packet including zero terminator; see build_port.sh *\//' \
    "$PSX/src/gpu/PsyX_GPU.cpp"

# PsyCross bugfix (idempotent). In this native port the non-extended primitive
# tag stores a host pointer-sized addr plus len/code bytes, so DR_MODE has two
# payload words after the tag. The stock PSX setDrawMode macro still writes
# len=3; ParsePrimitivesLinkedList then advances one word past each DR_MODE and
# logs "diff=-12" when the field zoom fade queues draw-mode packets.
sed -i '/#define setDrawMode/,/((p)->code\\[1\\] = _get_tw/s/setlen(p, 3)/setlen(p, 2)/' \
    "$PSX/include/psx/libgpu.h"

# PsyCross bugfix (idempotent). The full-size TILE primitive has three payload
# words after the tag (color/code, xy, wh). The stock PsyCross header used len=2
# while its parser consumes three payload words, so field fade TILE packets
# (code 0x62 with semi-transparency) leave ParsePrimitivesLinkedList one word
# past the packet and produce diff=-4 / zero-length primitive traversal noise.
sed -i 's/#define setTile(p)[[:space:]]*setlen(p, 2),[[:space:]]*setcode(p, 0x60)/#define setTile(p)\tsetlen(p, 3),  setcode(p, 0x60)/' \
    "$PSX/include/psx/libgpu.h"

# PsyCross bugfix (idempotent, grep-guarded so it inserts the pad field exactly
# once). ClearOTag/ClearOTagR build the OT linked list by casting the caller's
# array to OT_TAG* and striding by sizeof(OT_TAG). On PSX u_long is 4 bytes and
# OT_TAG (addr:24,len:8) is also 4, so they coincide. In the port u_long is 8
# bytes, so the game declares its OTs as 8-byte-strided u_long[] arrays (e.g.
# RenderContext.ot3[8], field ot1/ot2[0x1000], menu ot[0x10]) and indexes/draws
# them at 8-byte stride -- but ClearOTagR still wrote a 4-byte-strided list,
# leaving the upper half of every slot zeroed. DrawOTag(ot3+7) then read slot 7
# at byte 56 (zero) -> addr=0 (not the 0xffffff terminator) -> walked to null ->
# SIGSEGV in ParsePrimitivesLinkedList. Pad OT_TAG to the host u_long size so
# ClearOTag(R)'s stride matches the game's u_long[] OTs. (Port is always built
# non-extended; OT_TAG is only used by ClearOTag(R) + the unused prim_terminator.)
grep -q "_xeno_ot_pad" "$PSX/include/psx/libgpu.h" || \
sed -i 's|^} OT_TAG;|\tu_int _xeno_ot_pad; /* XENO_PC_PORT: pad OT slot to host u_long (8B) so ClearOTag(R) stride matches the game'"'"'s u_long[] OTs; see build_port.sh */\n} OT_TAG;|' \
    "$PSX/include/psx/libgpu.h"

# PsyCross bugfix (idempotent, grep-guarded). GR_CopyVRAM (the LoadImage backend)
# wrote to vram[dst_x + dst_y*VRAM_WIDTH] with no coordinate masking. The PSX GPU
# masks VRAM-transfer coords to the framebuffer dimensions (X to 10 bits, Y to 9).
# Field NPC sprite skins upload with texX = (texPageOffset<<4)+0x100 as high as
# 2368, which the hardware wraps to 2368 & 0x3FF = 320 on the SAME row -- exactly
# where the NPC prims' clut/tpage fields sample. Unmasked, the copy ran off the
# row into the wrong VRAM line, so the NPC palette/texel pages stayed empty and
# every NPC rendered fully transparent (invisible). Mask dst_x/dst_y to VRAM
# bounds. Legitimate uploads all use x<1024/y<512, so they are unchanged; only
# the >=1024 sprite-skin uploads move from a wrong line to the correct wrapped
# one. (A copy that itself straddles x=1024 is not row-wrapped; no field upload
# does that.)
grep -q "_xeno_vram_wrap" "$PSX/src/render/PsyX_render.cpp" || \
sed -i 's|\(\tunsigned short\* dst = vram + dst_x + dst_y \* VRAM_WIDTH;\)|\tdst_x \&= (VRAM_WIDTH - 1); dst_y \&= (VRAM_HEIGHT - 1); /* _xeno_vram_wrap: PSX coord mask; see build_port.sh */\n\1|' \
    "$PSX/src/render/PsyX_render.cpp"

echo "==> [1/5] Building PsyCross (libpsycross.a) via CMake"
# Drop a stale CMake cache generated under a different absolute path (e.g. from a
# different container mount) so it reconfigures cleanly in the current env.
if [ -f pc_port/build/CMakeCache.txt ] && \
   ! grep -q "CMAKE_HOME_DIRECTORY:INTERNAL=$ROOT/pc_port" pc_port/build/CMakeCache.txt; then
    echo "    (removing stale CMake cache)"; rm -rf pc_port/build
fi
cmake -S pc_port -B pc_port/build -DCMAKE_BUILD_TYPE=Debug >/dev/null
cmake --build pc_port/build --target psycross -j"$(nproc)" >/dev/null
PSYLIB="$(find pc_port/build -name 'libpsycross.a' 2>/dev/null | head -1)"
if [ -z "$PSYLIB" ] || [ ! -s "$PSYLIB" ]; then
    echo "ERROR: libpsycross.a was not built (the CMake step failed above)."
    echo "       Make sure you're in the distrobox with SDL2/OpenAL/OpenGL dev installed."
    exit 1
fi
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
# src/.../system/archive.c is excluded: the port replaces its async CD state
# machine with a synchronous PsyCross-libcd read in pc_port/src/archive_port.c
# (ArchiveReadFile). Its other (async/stream) symbols become no-op stubs.
done < <(find src -name '*.c' | grep -v '/psyq/' | grep -v '/system/archive\.c$' | sort)
echo "    compiled=$compiled  skipped=$skipped"
[ -n "$SKIPPED" ] && echo "    skipped (will be stubbed):$SKIPPED"

echo "==> [2b/5] Compiling port-only sources (PSX RAM emu, overrides/dispatch table)"
for pf in pc_port/src/psx_memory.c pc_port/src/game_overrides.c pc_port/src/psyq_compat.c pc_port/src/archive_port.c pc_port/src/work_list_port.c pc_port/src/data_published_logo.c pc_port/src/data_font.c pc_port/src/data_kernel_menu.c pc_port/src/data_field.c; do
    o="$OBJ/$(basename "$pf").o"
    if gcc -c "$pf" $GFLAGS -Ipc_port/src $INC -o "$o" 2>/tmp/pcerr; then
        GAME_OBJS+=("$o"); echo "    $(basename "$pf") ok"
    else
        echo "    $(basename "$pf") FAILED:"; grep -m4 "error:" /tmp/pcerr | sed "s|^|      |"
    fi
done

echo "==> [3/5] Compiling port entry point"
gcc -c pc_port/src/port_main.c $GFLAGS -Ipc_port/src -I"$PSX/include" -I"$PSX/include/psx" -o "$OBJ/port_main.o" 2>/tmp/pmerr || {
    echo "    port_main FAILED:"; grep -m6 "error:" /tmp/pmerr | sed "s|^|      |"; }

LIBS="$(pkg-config --libs sdl2 openal 2>/dev/null) -lGL -lm -lpthread -ldl"
# -no-pie: link non-PIE so the executable loads at a fixed low base and ALL of
# its BSS (the emulated PSX RAM g_PsxRam[] plus every auto-stubbed data symbol)
# lives below 4 GiB. The decompiled game truncates its own pointers to 32 bits
# all over (e.g. the heap's `(u32)pHeapStart & -4`); keeping that memory in the
# low 32-bit address space makes every such truncation a lossless round-trip.
NOPIE="-no-pie -fno-pie"
LINK=(gcc -m64 $NOPIE "$OBJ/port_main.o" "${GAME_OBJS[@]}" "$PSYLIB" $LIBS -o "$OUT/xeno-port")

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
    # symbol_addrs files carry the real struct sizes (size:) the ELF omits, so data
    # stubs (e.g. g_GameState = 0x2300) are reserved at full size instead of 16 bytes.
    SYMS=()
    for s in config/symbol_addrs.slus_006.64.txt config/symbol_addrs.field.txt \
             config/symbol_addrs.member_change_menu.txt config/symbol_addrs.shop_menu.txt; do
        [ -f "$s" ] && SYMS+=(--symbol-addrs "$s")
    done
    if [ "${#ELFS[@]}" -gt 0 ]; then
        python3 tools/scripts/gen_port_stubs.py "${ELFS[@]}" "${SYMS[@]}" --undefined "$OUT/undef.txt" --out "$OUT/stubs.c"
    elif [ -f "$OUT/stubs.c" ]; then
        echo "    (warning: matching ELFs missing; reusing existing stubs.c)"
    else
        echo "ERROR: matching ELFs missing and no existing $OUT/stubs.c to reuse."
        exit 1
    fi
    nm -g --defined-only "$OBJ/port_main.o" "${GAME_OBJS[@]}" 2>/dev/null \
        | awk '{print $3}' | sort -u > "$OUT/defined.txt"
    python3 - "$OUT/stubs.c" "$OUT/defined.txt" <<'PY'
import re
import sys

stubs_path, defined_path = sys.argv[1], sys.argv[2]
with open(defined_path) as f:
    defined = set(f.read().split())
with open(stubs_path) as f:
    lines = f.readlines()

stub_re = re.compile(r"^long\s+([A-Za-z_]\w*)\(void\)\s+\{")
out = []
removed = []
for line in lines:
    m = stub_re.match(line)
    if m and m.group(1) in defined:
        removed.append(m.group(1))
        continue
    out.append(line)

func_count = sum(1 for line in out if stub_re.match(line))
for i, line in enumerate(out):
    if re.match(r"/\* ---- \d+ function symbols ---- \*/", line):
        out[i] = f"/* ---- {func_count} function symbols ---- */\n"
        break

if removed:
    with open(stubs_path, "w") as f:
        f.writelines(out)
    print(f"    pruned stale function stubs: {', '.join(removed[:8])}"
          f"{'...' if len(removed) > 8 else ''}")
PY
    gcc -c "$OUT/stubs.c" -O0 -g -o "$OBJ/stubs.o"
fi

echo "==> [5/5] Final link"
gcc -m64 $NOPIE "$OBJ/port_main.o" "${GAME_OBJS[@]}" "$OBJ/stubs.o" "$PSYLIB" $LIBS -o "$OUT/xeno-port" 2> "$OUT/link2.err"
if [ -f "$OUT/xeno-port" ] && [ ! -s "$OUT/link2.err" ]; then
    echo "    LINK OK -> $OUT/xeno-port"
else
    echo "    LINK incomplete; remaining errors:"
    grep -oE "undefined reference to \`[A-Za-z0-9_]+'|multiple definition of \`[A-Za-z0-9_]+'" "$OUT/link2.err" | sort | uniq -c | sort -rn | head -20
fi
