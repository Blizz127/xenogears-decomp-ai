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

# Concurrency-validation build (the third regime, alongside objdiff + behavioral):
# XENO_TSAN=1 rebuilds PsyCross + game TUs + stubs + link with ThreadSanitizer
# into separate build dirs (pc_port/build_tsan, pc_port/build_native_tsan) so
# the normal artifacts stay untouched. Used to prove the sound tick gate
# (g_SoundTickMutex) holds: zero data races on sound shared state under
# main-thread-vs-240Hz-tick contention.
TSAN_FLAGS=""
PSYX_BUILD="pc_port/build"
if [ "${XENO_TSAN:-0}" = "1" ]; then
    TSAN_FLAGS="-fsanitize=thread"
    PSYX_BUILD="pc_port/build_tsan"
    OUT="pc_port/build_native_tsan"
    OBJ="$OUT/obj"
    echo "==> XENO_TSAN=1: ThreadSanitizer build -> $OUT"
fi
mkdir -p "$OBJ"
if [ -n "$TSAN_FLAGS" ] && [ -f pc_port/build_native/stubs.c ] && [ ! -f "$OUT/stubs.c" ]; then
    # The TSan variant mirrors the normal build: reuse its validated stub
    # manifest (without matching ELFs the generator cannot create one, and
    # the undef set is identical by construction).
    cp pc_port/build_native/stubs.c "$OUT/stubs.c"
fi

# This must run in an environment with the toolchain + libs (the distrobox on
# Bazzite, NOT the immutable host). Fail fast with guidance if it's the wrong one.
for tool in cmake gcc pkg-config python3 ar nm objcopy; do
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
# XENO_FIELD_OBJECT_OVERLAY (Phase 3): activate func_800A1364's object-register
# body (sprite-load + script IP-advance/un-spin + object registration).  The
# object DRAW entries (func_801E742C/738C/7D14/8330) remain safe no-op stubs
# (Phase-2B proved they are incoherent mid-menu-function targets); this arms the
# un-spin so stuck field scripts (MAP3/MAP16) advance and field models build.
GFLAGS="-std=gnu17 -fpermissive -DXENO_PC_PORT -DXENO_FIELD_OBJECT_OVERLAY -DSKIP_ASM -D_LANGUAGE_C -DUSE_EXTENDED_PRIM_POINTERS=0 -include assert.h -w -O0 -g -m64 -fno-builtin"
# Optional compile-time diagnostics, for example:
# XENO_DIAG_DEFINES=-DXENO_DIAG_OPCODE_SWEEP ./scratchpad/run_build_port.sh
# Leave GFLAGS byte-for-byte unchanged when unset.
if [ -n "${XENO_DIAG_DEFINES:-}" ]; then
    GFLAGS="$GFLAGS $XENO_DIAG_DEFINES"
fi
# TSan instrumentation for game TUs (see XENO_TSAN above). GFLAGS stays
# byte-for-byte unchanged when unset.
if [ -n "$TSAN_FLAGS" ]; then
    GFLAGS="$GFLAGS $TSAN_FLAGS"
fi

# Build-integrity policy:
#
# Every game TU is expected to compile and link. Explicit exclusions are
# deliberately neither compiled nor linked. REFERENCE_ONLY_GAME_TUS have a
# permanent port-side runtime owner while their retail source remains in the
# matching build. The remaining exclusions are temporary HOLDs whose source
# now compiles but cannot safely enter the port link until routing/decomp work
# is resolved. In contrast, KNOWN_BROKEN_GAME_TUS are compile failures tolerated
# temporarily. Do not add a TU to any list merely to make a build pass; each
# entry needs a reviewed ownership decision or issue.
INTENTIONALLY_EXCLUDED_GAME_TU_PATTERNS=(
    "*/psyq/*"
)
INTENTIONALLY_EXCLUDED_GAME_TUS=(
    "src/slus_006.64/system/archive.c"
)
REFERENCE_ONLY_GAME_TUS=(
    "src/slus_006.64/system/work_list.c"
)
KNOWN_BROKEN_GAME_TUS=(
)

known_broken_tu_reason() {
    case "$1" in
    esac
}

is_known_broken_game_tu() {
    local candidate="$1"
    local broken
    for broken in "${KNOWN_BROKEN_GAME_TUS[@]}"; do
        [ "$candidate" = "$broken" ] && return 0
    done
    return 1
}

is_intentionally_excluded_game_tu() {
    local candidate="$1"
    local excluded
    local pattern

    for excluded in "${INTENTIONALLY_EXCLUDED_GAME_TUS[@]}"; do
        [ "$candidate" = "$excluded" ] && return 0
    done
    for pattern in "${INTENTIONALLY_EXCLUDED_GAME_TU_PATTERNS[@]}"; do
        [[ "$candidate" == $pattern ]] && return 0
    done
    return 1
}

is_reference_only_game_tu() {
    local candidate="$1"
    local reference
    for reference in "${REFERENCE_ONLY_GAME_TUS[@]}"; do
        [ "$candidate" = "$reference" ] && return 0
    done
    return 1
}

reference_only_game_tu_reason() {
    case "$1" in
        src/slus_006.64/system/work_list.c)
            echo "runtime replaced by pc_port/src/work_list_port.c (packed 0x1C entry layout)" ;;
    esac
}

excluded_game_tu_reason() {
    case "$1" in
        "*/psyq/*")
            echo "PsyQ originals are replaced at runtime by PsyCross" ;;
        src/slus_006.64/system/archive.c)
            echo "replaced by pc_port/src/archive_port.c" ;;
    esac
}

print_reference_only_game_tus() {
    echo "    Reference-only game TUs (matching source; replaced in the port link):"
    local reference
    for reference in "${REFERENCE_ONLY_GAME_TUS[@]}"; do
        echo "      $reference — $(reference_only_game_tu_reason "$reference")"
    done
}

print_intentionally_excluded_game_tus() {
    echo "    Intentionally excluded game TUs (not compiled or linked):"
    local excluded
    local pattern
    for excluded in "${INTENTIONALLY_EXCLUDED_GAME_TUS[@]}"; do
        echo "      $excluded — $(excluded_game_tu_reason "$excluded")"
    done
    for pattern in "${INTENTIONALLY_EXCLUDED_GAME_TU_PATTERNS[@]}"; do
        echo "      $pattern — $(excluded_game_tu_reason "$pattern")"
    done
}

print_known_broken_game_tus() {
    if [ "${#KNOWN_BROKEN_GAME_TUS[@]}" -eq 0 ]; then
        echo "    Allowlisted broken game TUs (compile failures tolerated): none"
        return
    fi
    echo "    WARNING: building with explicitly allowlisted broken game TUs:"
    local broken
    for broken in "${KNOWN_BROKEN_GAME_TUS[@]}"; do
        echo "      $broken — $(known_broken_tu_reason "$broken")"
    done
    echo "    WARNING: these are temporary exceptions; any other game-TU compile failure aborts."
}

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

# PsyCross bugfix (idempotent, grep-guarded), two coordinated edits. PsyX defers
# the GL backbuffer -> vram[] readback and, in stock form, materializes it on
# EVERY DrawSync. In the field the game presents from a debug-menu second
# display buffer whose rows (y>=240) also hold the dialog UI palette/tiles it
# uploads by LoadImage after the frame was latched; the blanket materialize
# splatted the stale frame snapshot over those newer uploads, erasing the UI
# CLUTs (flat 0x8004) so dialog boxes drew nothing. Fix: (A) drop the eager
# DrawSync materialize; (B) do it lazily in GR_CopyVRAM's VRAM-READ path
# (MoveImage source) and ONLY when the read rect overlaps the snapshot rect, so
# non-overlapping reads (e.g. the screen-capture MoveImage at x=704) leave the
# freshly uploaded rows intact. Matches PSX order (frame pixels are written at
# draw time, before any later LoadImage).
grep -q "_xeno_read_materialize" "$PSX/src/psx/LIBGPU.C" || \
perl -0777 -i -pe 's/\tGR_ReadFramebufferDataToVRAM\(\);\n\n\tif \(g_splitIndex/\t\/* _xeno_read_materialize: moved to GR_CopyVRAM read path; the eager\n\t * DrawSync materialize splatted stale frame snapshots over newer\n\t * LoadImage uploads. See build_port.sh. *\/\n\n\tif (g_splitIndex/' \
    "$PSX/src/psx/LIBGPU.C"
grep -q "_xeno_read_materialize" "$PSX/src/render/PsyX_render.cpp" || \
perl -0777 -i -pe 's/\tif \(!src\)\n\t\{\n\t\tframebuffer_need_update = 1;/\tif (!src)\n\t{\n\t\t\/* _xeno_read_materialize: reconcile the pending rendered-frame\n\t\t * snapshot into vram[] only when this VRAM read overlaps the\n\t\t * snapshot rect, so the read sees frame pixels without erasing\n\t\t * non-overlapping rows the game re-used for uploads. See build_port.sh. *\/\n\t\tif (framebuffer_need_update \&\&\n\t\t    x < g_PreviousFramebuffer.x + g_PreviousFramebuffer.w \&\&\n\t\t    x + w > g_PreviousFramebuffer.x \&\&\n\t\t    y < g_PreviousFramebuffer.y + g_PreviousFramebuffer.h \&\&\n\t\t    y + h > g_PreviousFramebuffer.y)\n\t\t{\n\t\t\tGR_ReadFramebufferDataToVRAM();\n\t\t}\n\n\t\tframebuffer_need_update = 1;/' \
    "$PSX/src/render/PsyX_render.cpp"

# PsyCross bugfix (idempotent, grep-guarded): GR_CopyRGBAFramebufferToVRAM read
# the GL backbuffer (RGBA8, R in the low byte) but extracted R into the RGB555
# B-field and B into the R-field, swapping red<->blue on every framebuffer->vram
# readback. VRAM readbacks (and any effect that re-uses the materialized frame,
# e.g. field water/reflection) therefore came out R/B swapped: the dialog text's
# blue outline (0xc086) materialized as red (0x1890), which is why VRAM-capture
# diagnostics kept reporting a "red overlay" even though the live GL display was
# already correct white/blue. The direct display path (GR_SwapWindow) is
# unaffected; this only corrects the readback so vram[] matches the screen.
grep -q "_xeno_fb_rgb" "$PSX/src/render/PsyX_render.cpp" || \
perl -0777 -i -pe 's/\t\t\tu_char b = \(\(c >> 3\) & 0x1F\);\n\t\t\tu_char g = \(\(c >> 11\) & 0x1F\);\n\t\t\tu_char r = \(\(c >> 19\) & 0x1F\);/\t\t\tu_char r = ((c >> 3) \& 0x1F); \/* _xeno_fb_rgb: RGBA source, R is the low byte; stock code swapped R<->B on readback. See build_port.sh. *\/\n\t\t\tu_char g = ((c >> 11) \& 0x1F);\n\t\t\tu_char b = ((c >> 19) \& 0x1F);/' \
    "$PSX/src/render/PsyX_render.cpp"

# PsyCross bugfix (idempotent): a PSX DR_MOVE executes in ordering-table order.
# PsyCross batches FT3/FT4s until the end of DrawOTag, but executes MoveImage
# immediately while parsing. Framebuffer-feedback effects (Map014's painting
# distortion is the first live case) therefore copied stale CPU VRAM instead of
# the polygons preceding the copy. At a DR_MOVE boundary, flush the completed
# batch, synchronously materialize the current draw buffer into both GPU/CPU
# VRAM, then perform the copy. The following textured split uploads the changed
# VRAM before it captures its texture ID, preserving GPU command order without
# forcing a readback for every one of the effect's adjacent strip copies.
# PsyCross is gitignored, so
# keep this durable source patch in the tracked build driver.
python3 - "$PSX" <<'DRMOVE_PY'
import sys

psx = sys.argv[1]
gpu = psx + "/src/gpu/PsyX_GPU.cpp"
ren = psx + "/src/render/PsyX_render.cpp"
hdr = psx + "/include/PsyX/PsyX_render.h"

def edit(path, marker, pairs):
    with open(path) as f:
        s = f.read()
    if marker in s:
        return
    for old, new, count in pairs:
        found = s.count(old)
        if found != count:
            sys.exit("ERROR: DR_MOVE patch anchor mismatch in %s for %s "
                     "(found %d, expected %d): %r" %
                     (path, marker, found, count, old[:80]))
        s = s.replace(old, new)
    with open(path, "w") as f:
        f.write(s)

edit(hdr, "_xeno_drmove_snapshot_decl", [(
"extern void\t\t\tGR_StoreFrameBuffer(int x, int y, int w, int h);\n",
"extern void\t\t\tGR_StoreFrameBuffer(int x, int y, int w, int h);\n"
"extern void\t\t\tGR_StoreFrameBufferImmediate(int x, int y, int w, int h); /* _xeno_drmove_snapshot_decl */\n",
1)])

edit(ren, "_xeno_fb_staging_local", [(
"\t\tglBlitFramebuffer(0, 0, g_windowWidth, g_windowHeight, x, y + h, x + w, y, GL_COLOR_BUFFER_BIT, GL_NEAREST);\n",
"\t\t/* _xeno_fb_staging_local: g_fbTexture is a w-by-h staging texture,\n"
"\t\t * not a full VRAM surface. x/y select the final VRAM destination\n"
"\t\t * below; copying to those nonzero coordinates here clips context 1\n"
"\t\t * (y=256) completely out of the staging FBO. */\n"
"\t\tglBlitFramebuffer(0, 0, g_windowWidth, g_windowHeight, 0, h, w, 0, GL_COLOR_BUFFER_BIT, GL_NEAREST);\n",
1)])

edit(ren, "_xeno_drmove_snapshot", [(
"void GR_CopyVRAM(unsigned short* src, int x, int y, int w, int h, int dst_x, int dst_y)\n",
"/* _xeno_drmove_snapshot: unlike the normal present path, DR_MOVE must see\n"
" * polygons submitted earlier in this same ordering table. GR_StoreFrameBuffer\n"
" * keeps the GPU VRAM texture current; this synchronous read completes the CPU\n"
" * vram[] mirror that MoveImage reads. The ordinary PBO path intentionally\n"
" * remains deferred for frame presentation. See build_port.sh. */\n"
"void GR_StoreFrameBufferImmediate(int x, int y, int w, int h)\n"
"{\n"
"\tGR_StoreFrameBuffer(x, y, w, h);\n"
"\n"
"#if USE_OPENGL\n"
"\tif (w <= 0 || h <= 0)\n"
"\t\treturn;\n"
"\n"
"\tu_int* pixels = (u_int*)malloc((size_t)w * h * sizeof(u_int));\n"
"\tif (!pixels)\n"
"\t\treturn;\n"
"\n"
"\tglActiveTexture(GL_TEXTURE0);\n"
"\tglBindTexture(GL_TEXTURE_2D, g_fbTexture);\n"
"\tglGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);\n"
"\tglBindTexture(GL_TEXTURE_2D, g_lastBoundTexture >= 0 ? g_lastBoundTexture : 0);\n"
"\tGR_CopyRGBAFramebufferToVRAM(pixels, x, y, w, h, 1, 0);\n"
"\tfree(pixels);\n"
"#endif\n"
"}\n"
"\n"
"void GR_CopyVRAM(unsigned short* src, int x, int y, int w, int h, int dst_x, int dst_y)\n",
1)])

edit(gpu, "_xeno_drmove_snapshot_dirty", [(
"int g_splitIndex = 0;\n",
"int g_splitIndex = 0;\n"
"/* _xeno_drmove_snapshot_dirty: a completed draw batch changes the GL\n"
" * framebuffer, so the next VRAM-read DR_MOVE must materialize it once. */\n"
"static int g_xeno_vram_snapshot_dirty = 1;\n",
1)])

edit(gpu, "_xeno_drmove_upload", [(
"\t// next code ideally should be called before EndScene\n"
"\tGR_UpdateVertexBuffer(g_vertexBuffer, g_vertexIndex);\n",
"\t// next code ideally should be called before EndScene\n"
"\t/* _xeno_drmove_upload: MoveImage changes the CPU VRAM mirror while an\n"
"\t * OT is being parsed. Upload it at the next actual draw boundary, rather\n"
"\t * than once per adjacent DR_MOVE packet. See build_port.sh. */\n"
"\tGR_UpdateVRAM();\n"
"\tGR_UpdateVertexBuffer(g_vertexBuffer, g_vertexIndex);\n",
1), (
"\tfor (int i = 1; i <= g_splitIndex; i++)\n"
"\t\tDrawSplit(g_splits[i]);\n"
"\n"
"\tClearSplits();\n",
"\tfor (int i = 1; i <= g_splitIndex; i++)\n"
"\t\tDrawSplit(g_splits[i]);\n"
"\n"
"\tif (g_vertexIndex != 0)\n"
"\t\tg_xeno_vram_snapshot_dirty = 1;\n"
"\n"
"\tClearSplits();\n",
1)])

# The first version synchronized CPU VRAM in DrawAllSplits. That happened after
# AddSplit had captured a double-buffered texture ID, so feedback FT4s bound
# stale VRAM. Migrate that version, then synchronize before textured splits
# capture g_vramTexture. The v2 marker intentionally contains the v1 marker.
edit(gpu, "_xeno_drmove_upload_v2", [(
"\t// next code ideally should be called before EndScene\n"
"\t/* _xeno_drmove_upload: MoveImage changes the CPU VRAM mirror while an\n"
"\t * OT is being parsed. Upload it at the next actual draw boundary, rather\n"
"\t * than once per adjacent DR_MOVE packet. See build_port.sh. */\n"
"\tGR_UpdateVRAM();\n"
"\tGR_UpdateVertexBuffer(g_vertexBuffer, g_vertexIndex);\n",
"\t// next code ideally should be called before EndScene\n"
"\t/* _xeno_drmove_upload_v2: textured splits synchronize VRAM before they\n"
"\t * capture g_vramTexture in AddSplit; doing it here would rotate the\n"
"\t * double-buffer after that ID was already recorded. */\n"
"\tGR_UpdateVertexBuffer(g_vertexBuffer, g_vertexIndex);\n",
1)])

edit(gpu, "_xeno_drmove_split_upload", [(
"\tTextureID textureId = textured ? g_vramTexture : g_whiteTexture;\n",
"\t/* _xeno_drmove_split_upload: MoveImage changes CPU VRAM during OT\n"
"\t * traversal. Synchronize before this split snapshots g_vramTexture;\n"
"\t * otherwise the double-buffer flips later in DrawAllSplits and the\n"
"\t * feedback FT4s sample the pre-MoveImage page. The helper is a no-op\n"
"\t * when VRAM is already current. */\n"
"\tif (textured)\n"
"\t\tGR_UpdateVRAM();\n"
"\n"
"\tTextureID textureId = textured ? g_vramTexture : g_whiteTexture;\n",
1)])

edit(gpu, "_xeno_drmove_order", [(
"\t\t\tMoveImage(&rect, x, y);\n"
"\t\t\tprimLength = 5;\n",
"\t\t\t/* _xeno_drmove_order: PsyCross normally defers polygons until the\n"
"\t\t\t * end of DrawOTag, but the PSX GPU executes this VRAM copy after\n"
"\t\t\t * every earlier packet. Finalize the still-open split before the\n"
"\t\t\t * flush, then materialize the draw target once before the first\n"
"\t\t\t * following VRAM read. See build_port.sh. */\n"
"\t\t\tif (g_splitIndex > 0)\n"
"\t\t\t{\n"
"\t\t\t\tGPUDrawSplit& lastSplit = g_splits[g_splitIndex];\n"
"\t\t\t\tlastSplit.numVerts = g_vertexIndex - lastSplit.startVertex;\n"
"\t\t\t\tDrawAllSplits();\n"
"\t\t\t}\n"
"\n"
"\t\t\tif (g_xeno_vram_snapshot_dirty)\n"
"\t\t\t{\n"
"\t\t\t\tGR_StoreFrameBufferImmediate(activeDrawEnv.clip.x, activeDrawEnv.clip.y,\n"
"\t\t\t\t\tactiveDrawEnv.clip.w, activeDrawEnv.clip.h);\n"
"\t\t\t\tg_xeno_vram_snapshot_dirty = 0;\n"
"\t\t\t}\n"
"\n"
"\t\t\tMoveImage(&rect, x, y);\n"
"\t\t\tprimLength = 5;\n",
1)])

# Adjacent DR_MOVEs (the distortion effect queues 15 in a row) re-arm
# framebuffer_need_update in GR_CopyVRAM's read path without any new capture,
# so the second move's entry check splatted a two-downloads-stale PBO frame
# over the fresh synchronous snapshot and the first move's writes. Track
# whether vram[] is at least as fresh as any pending deferred capture: the
# synchronous snapshot sets the flag, and only an actual framebuffer advance
# (EndScene/Clear) clears it. The PBO's content can never be newer than the
# synchronous snapshot, so suppressing the splat while the flag is set is
# strictly lossless.
edit(ren, "_xeno_drmove_fb_synced", [(
"int framebuffer_need_update = 0;\n",
"int framebuffer_need_update = 0;\n"
"/* _xeno_drmove_fb_synced: 1 while the CPU vram[] framebuffer rect is at\n"
" * least as fresh as any pending deferred PBO capture. See build_port.sh. */\n"
"int g_xeno_vram_fb_synced = 0;\n",
1), (
"void GR_EndScene()\n{\n\tframebuffer_need_update = 1;\n",
"void GR_EndScene()\n{\n\tframebuffer_need_update = 1;\n\tg_xeno_vram_fb_synced = 0; /* _xeno_drmove_fb_synced */\n",
1), (
"void GR_Clear(int x, int y, int w, int h, unsigned char r, unsigned char g, unsigned char b)\n{\n\tframebuffer_need_update = 1;\n",
"void GR_Clear(int x, int y, int w, int h, unsigned char r, unsigned char g, unsigned char b)\n{\n\tframebuffer_need_update = 1;\n\tg_xeno_vram_fb_synced = 0; /* _xeno_drmove_fb_synced */\n",
1), (
"\tGR_CopyRGBAFramebufferToVRAM(pixels, x, y, w, h, 1, 0);\n\tfree(pixels);\n",
"\tGR_CopyRGBAFramebufferToVRAM(pixels, x, y, w, h, 1, 0);\n\tfree(pixels);\n"
"\tg_xeno_vram_fb_synced = 1; /* _xeno_drmove_fb_synced: vram[] is newest */\n",
1), (
"\t\tif (framebuffer_need_update &&\n",
"\t\t/* _xeno_drmove_fb_synced: skip the deferred splat while the synchronous\n"
"\t\t * snapshot in vram[] is newer than anything the PBO could hold. */\n"
"\t\tif (framebuffer_need_update && !g_xeno_vram_fb_synced &&\n",
1)])

print("    DR_MOVE ordering patches OK")
DRMOVE_PY

# PsyCross bugfix (idempotent, grep-guarded): gte_stflg writes FLAG as a
# 32-bit uint into the caller's slot. On LP64 the game stores that into a
# `long flag` and tests `flag < 0` (bit 63), while retail `bltz` tests bit 31
# of the 32-bit FLAG (e.g. 0x80021000 = SX/SY saturation). Sign-extend after
# each RotTransPers* write so matching C keeps working. Do NOT widen gte_stflg
# itself — RotTransPers4's local `int _flag` is a 32-bit destination.
grep -q "_xeno_gte_flag_sx" "$PSX/src/psx/LIBGTE.C" || \
perl -0777 -i -pe 's/(int RotTransPers\(SVECTOR\* v0, int\* sxy, long\* p, long\* flag\)\n\{\n\tint sz;\n\tgte_RotTransPers\(v0, sxy, p, flag, \&sz\);\n\n\treturn sz;\n\})/int RotTransPers(SVECTOR* v0, int* sxy, long* p, long* flag)\n{\n\tint sz;\n\tgte_RotTransPers(v0, sxy, p, flag, \&sz);\n\t*flag = (long)(int)(unsigned int)*flag; \/* _xeno_gte_flag_sx *\/\n\treturn sz;\n}/s' \
    "$PSX/src/psx/LIBGTE.C"
grep -q "_xeno_gte_flag_sx3" "$PSX/src/psx/LIBGTE.C" || \
perl -0777 -i -pe 's/(int RotTransPers3\(SVECTOR\* v0, SVECTOR\* v1, SVECTOR\* v2, long\* sxy0, long\* sxy1, long\* sxy2, long\* p, long\* flag\)\n\{\n\tint sz;\n\tgte_RotTransPers3\(v0, v1, v2, sxy0, sxy1, sxy2, p, flag, \&sz\);\n\n\treturn sz;\n\})/int RotTransPers3(SVECTOR* v0, SVECTOR* v1, SVECTOR* v2, long* sxy0, long* sxy1, long* sxy2, long* p, long* flag)\n{\n\tint sz;\n\tgte_RotTransPers3(v0, v1, v2, sxy0, sxy1, sxy2, p, flag, \&sz);\n\t*flag = (long)(int)(unsigned int)*flag; \/* _xeno_gte_flag_sx3 *\/\n\treturn sz;\n}/s' \
    "$PSX/src/psx/LIBGTE.C"
grep -q "_xeno_gte_flag_sx4" "$PSX/src/psx/LIBGTE.C" || \
perl -0777 -i -pe 's/(\t\*flag \|= _flag;\n\tgte_stszotz\(&sz\);\n\n\treturn sz;\n\})/\t*flag |= _flag;\n\t*flag = (long)(int)(unsigned int)*flag; \/* _xeno_gte_flag_sx4 *\/\n\tgte_stszotz(\&sz);\n\n\treturn sz;\n}/s' \
    "$PSX/src/psx/LIBGTE.C"

# PsyCross feature (idempotent, per-edit marker-guarded): PSX texture-window
# (GP0 E2h) emulation. PsyX parsed DR_TWIN into activeDrawEnv.tw but nothing
# ever APPLIED it -- primitives sampled raw UVs. Xenogears' dialog UI relies on
# the window: border/glyph SPRTs carry UVs like (128,192) and expect the E2
# window to confine/tile them into the small border tile, so without emulation
# they sampled unrelated VRAM (scattered black dashes). Edits, all general (no
# game-specific casing):
#   parse    - normalize raw E2 mask/offset into the same pixel RECT form
#              PutDrawEnv stores (w/h = window size, 0 = disabled; x/y =
#              pre-masked origin): one canonical representation, both sources;
#   split    - an E2 change breaks the draw batch (tw is repurposed as the
#              override-texture size when overrideTexture is active, so the
#              comparison is skipped there);
#   override - only the 32-bit override-texture path may stomp split tw with
#              the override size (previously stomped EVERY split with zeros);
#   draw     - DrawSplit forwards tw to the new GR_SetTextureWindow for PSX
#              texture formats;
#   render   - GR_SetTextureWindow re-derives the 5-bit hardware mask/offset
#              (only hardware-representable windows apply) and hands the
#              fragment shaders u_texWindow = (sizeX, sizeY, ofsX, ofsY);
#   shader   - samplePSX applies coord' = ofs + mod(coord, size) before every
#              VRAM tap (exact for all libgpu-encodable windows; identity when
#              disabled since size=256, ofs=0). Uniform is initialized to the
#              disabled window at compile so unset state stays a no-op.
python3 - "$PSX" <<'TEXWINDOW_PY'
import sys

psx = sys.argv[1]
GPU = psx + "/src/gpu/PsyX_GPU.cpp"
REN = psx + "/src/render/PsyX_render.cpp"
HDR = psx + "/include/PsyX/PsyX_render.h"

def edit(path, marker, pairs):
    with open(path) as f:
        s = f.read()
    if marker in s:
        return
    for old, new, count in pairs:
        n = s.count(old)
        if n != count:
            sys.exit("ERROR: texwindow patch anchor mismatch in %s for %s "
                     "(found %d, expected %d): %r" % (path, marker, n, count, old[:70]))
        s = s.replace(old, new)
    with open(path, "w") as f:
        f.write(s)

edit(GPU, "_xeno_texwindow_parse", [(
"\t\t\t// DR_TWIN\n"
"\t\t\tactiveDrawEnv.tw.w = (code & 0x1F);\n"
"\t\t\tactiveDrawEnv.tw.h = ((code >> 5) & 0x1F);\n"
"\t\t\tactiveDrawEnv.tw.x = ((code >> 10) & 0x1F);\n"
"\t\t\tactiveDrawEnv.tw.y = ((code >> 15) & 0x1F);\n",
"\t\t\t// DR_TWIN\n"
"\t\t\t/* _xeno_texwindow_parse: normalize raw E2 mask/offset fields into\n"
"\t\t\t * the pixel RECT form PutDrawEnv stores (w/h = window size, 0 =\n"
"\t\t\t * disabled; x/y = pre-masked origin) so one canonical form reaches\n"
"\t\t\t * GR_SetTextureWindow. See build_port.sh. */\n"
"\t\t\t{\n"
"\t\t\t\tconst u_int twMaskX = code & 0x1F;\n"
"\t\t\t\tconst u_int twMaskY = (code >> 5) & 0x1F;\n"
"\t\t\t\tactiveDrawEnv.tw.w = (256 - twMaskX * 8) & 0xFF;\n"
"\t\t\t\tactiveDrawEnv.tw.h = (256 - twMaskY * 8) & 0xFF;\n"
"\t\t\t\tactiveDrawEnv.tw.x = (((code >> 10) & 0x1F) & twMaskX) << 3;\n"
"\t\t\t\tactiveDrawEnv.tw.y = (((code >> 15) & 0x1F) & twMaskY) << 3;\n"
"\t\t\t}\n", 1)])

edit(GPU, "_xeno_texwindow_split", [(
"\t\tcurSplit.drawenv.dfe == activeDrawEnv.dfe &&\n",
"\t\tcurSplit.drawenv.dfe == activeDrawEnv.dfe &&\n"
"\t\t/* _xeno_texwindow_split: an E2 texture-window change must break the\n"
"\t\t * batch (tw is repurposed as the override size when overrideTexture\n"
"\t\t * is active, so the comparison is skipped there). See build_port.sh. */\n"
"\t\t(overrideTexture != 0 || (\n"
"\t\t\tcurSplit.drawenv.tw.x == activeDrawEnv.tw.x &&\n"
"\t\t\tcurSplit.drawenv.tw.y == activeDrawEnv.tw.y &&\n"
"\t\t\tcurSplit.drawenv.tw.w == activeDrawEnv.tw.w &&\n"
"\t\t\tcurSplit.drawenv.tw.h == activeDrawEnv.tw.h)) &&\n", 1)])

edit(GPU, "_xeno_texwindow_override", [(
"\tsplit.drawenv.tw.w = overrideTextureWidth;\n"
"\tsplit.drawenv.tw.h = overrideTextureHeight;\n",
"\tif (textured && overrideTexture != 0)\n"
"\t{\n"
"\t\t/* _xeno_texwindow_override: only the 32-bit override-texture path\n"
"\t\t * repurposes tw as the override size; PSX-format splits keep the\n"
"\t\t * game's E2 texture window. See build_port.sh. */\n"
"\t\tsplit.drawenv.tw.w = overrideTextureWidth;\n"
"\t\tsplit.drawenv.tw.h = overrideTextureHeight;\n"
"\t}\n", 1)])

edit(GPU, "_xeno_texwindow_draw", [(
"\tif (split.texFormat == TF_32_BIT_RGBA)\n"
"\t\tGR_SetOverrideTextureSize(split.drawenv.tw.w, split.drawenv.tw.h);\n",
"\tif (split.texFormat == TF_32_BIT_RGBA)\n"
"\t\tGR_SetOverrideTextureSize(split.drawenv.tw.w, split.drawenv.tw.h);\n"
"\telse\n"
"\t\tGR_SetTextureWindow(&split.drawenv.tw); /* _xeno_texwindow_draw: see build_port.sh */\n", 1)])

edit(HDR, "_xeno_texwindow_decl", [(
"extern void\t\t\tGR_SetOverrideTextureSize(int width, int height);\n",
"extern void\t\t\tGR_SetOverrideTextureSize(int width, int height);\n"
"extern void\t\t\tGR_SetTextureWindow(const RECT16* tw); /* _xeno_texwindow_decl: see build_port.sh */\n", 1)])

edit(REN, "_xeno_texwindow_loc_struct", [(
"\tGLint texelSizeLoc;\n",
"\tGLint texelSizeLoc;\n"
"\tGLint texWindowLoc; /* _xeno_texwindow_loc_struct */\n", 1)])

edit(REN, "_xeno_texwindow_loc_global", [(
"GLint u_texelSizeLoc;\n",
"GLint u_texelSizeLoc;\n"
"GLint u_texWindowLoc = -1; /* _xeno_texwindow_loc_global */\n", 1)])

edit(REN, "_xeno_texwindow_loc_get", [(
"\tsh->lutLoc = glGetUniformLocation(sh->shader, \"s_rgLut\");\n",
"\tsh->lutLoc = glGetUniformLocation(sh->shader, \"s_rgLut\");\n"
"\t/* _xeno_texwindow_loc_get: fetch the texture-window uniform and default\n"
"\t * it to the disabled window so unset state is an exact no-op. */\n"
"\tsh->texWindowLoc = glGetUniformLocation(sh->shader, \"u_texWindow\");\n"
"\tif (sh->texWindowLoc != -1)\n"
"\t{\n"
"\t\tglUseProgram(sh->shader);\n"
"\t\tglUniform4f(sh->texWindowLoc, 256.0f, 256.0f, 0.0f, 0.0f);\n"
"\t\tglUseProgram(0);\n"
"\t}\n", 1)])

edit(REN, "_xeno_texwindow_loc_route", [
("\t\tlutLoc = g_gpu_shader_4.lutLoc;\n",
 "\t\tlutLoc = g_gpu_shader_4.lutLoc;\n"
 "\t\tu_texWindowLoc = g_gpu_shader_4.texWindowLoc; /* _xeno_texwindow_loc_route */\n", 1),
("\t\tlutLoc = g_gpu_shader_8.lutLoc;\n",
 "\t\tlutLoc = g_gpu_shader_8.lutLoc;\n"
 "\t\tu_texWindowLoc = g_gpu_shader_8.texWindowLoc;\n", 1),
("\t\tlutLoc = g_gpu_shader_16.lutLoc;\n",
 "\t\tlutLoc = g_gpu_shader_16.lutLoc;\n"
 "\t\tu_texWindowLoc = g_gpu_shader_16.texWindowLoc;\n", 1),
("\t\tu_texelSizeLoc = g_gpu_shader_32_rgba.texelSizeLoc;\n",
 "\t\tu_texelSizeLoc = g_gpu_shader_32_rgba.texelSizeLoc;\n"
 "\t\tu_texWindowLoc = -1;\n", 1)])

edit(REN, "_xeno_texwindow_func", [(
"void GR_SetOverrideTextureSize(int width, int height)\n",
"/* _xeno_texwindow_func: PSX texture window (GP0 E2h). tw is the pixel RECT\n"
" * form (w/h = window size, 0 = disabled; x/y = origin). Re-derive the 5-bit\n"
" * hardware mask/offset so only hardware-representable windows apply, then\n"
" * hand the shader size+origin: coord' = origin + mod(coord, size).\n"
" * See build_port.sh. */\n"
"void GR_SetTextureWindow(const RECT16* tw)\n"
"{\n"
"#if USE_OPENGL\n"
"\tconst int maskX = ((256 - (tw->w & 0xFF)) >> 3) & 0x1F;\n"
"\tconst int maskY = ((256 - (tw->h & 0xFF)) >> 3) & 0x1F;\n"
"\tconst float sizeX = (float)(256 - maskX * 8);\n"
"\tconst float sizeY = (float)(256 - maskY * 8);\n"
"\tconst float ofsX = (float)((((tw->x & 0xFF) >> 3) & maskX) << 3);\n"
"\tconst float ofsY = (float)((((tw->y & 0xFF) >> 3) & maskY) << 3);\n"
"\tif (u_texWindowLoc != -1)\n"
"\t\tglUniform4f(u_texWindowLoc, sizeX, sizeY, ofsX, ofsY);\n"
"#endif\n"
"}\n"
"\n"
"void GR_SetOverrideTextureSize(int width, int height)\n", 1)])

edit(REN, "_xeno_texwindow_shader_decl", [(
"\t\t\"\tconst vec2 c_VRAMTexel = vec2(1.0 / 1024.0, 1.0 / 512.0);\\n\"\\\n",
"\t\t\"\tconst vec2 c_VRAMTexel = vec2(1.0 / 1024.0, 1.0 / 512.0);\\n\"\\\n"
"\t\t\"\tuniform vec4 u_texWindow; // _xeno_texwindow_shader_decl\\n\"\\\n", 2)])

edit(REN, "_xeno_texwindow_shader_apply", [
("    \"   vec2 samplePSX(vec2 tc) {\\n\"\\\n",
 "    \"   vec2 samplePSX(vec2 tc) {\\n\"\\\n"
 "    \"       tc = u_texWindow.zw + mod(tc, u_texWindow.xy); // _xeno_texwindow_shader_apply\\n\"\\\n", 1),
("\t\"\tvec2 samplePSX(vec2 tc) {\\n\"\\\n",
 "\t\"\tvec2 samplePSX(vec2 tc) {\\n\"\\\n"
 "\t\"\t\ttc = u_texWindow.zw + mod(tc, u_texWindow.xy);\\n\"\\\n", 2)])

print("    texture-window patches OK")
TEXWINDOW_PY

# PsyCross DRAWENV.isbg background-clear (upstream TODO): real libgpu's
# PutDrawEnv issues a fill of the clip rect when isbg is set -- the menu AND
# the field both set isbg=1 and rely on the per-frame clear (without it the
# menu's open/reveal animations leave trails). Implemented at env-apply
# (PutDrawEnv), NOT in DrawPrim: immediate-mode callers (the splash fades)
# would over-clear between prims. ClearImage = GR_ClearVRAM (rect-accurate)
# + GR_Clear (full-backbuffer glClear -- correct: it runs post-present,
# pre-DrawOTag in both the menu and field frame flows).
python3 - "$PSX" <<'ISBG_PY'
import sys

psx = sys.argv[1]
libgpu = psx + "/src/psx/LIBGPU.C"

def edit(path, marker, pairs):
    with open(path) as f:
        s = f.read()
    if marker in s:
        return
    for old, new, count in pairs:
        found = s.count(old)
        if found != count:
            sys.exit("ERROR: isbg patch anchor mismatch in %s for %s "
                     "(found %d, expected %d): %r" %
                     (path, marker, found, count, old[:80]))
        s = s.replace(old, new)
    with open(path, "w") as f:
        f.write(s)

edit(libgpu, "_xeno_isbg", [
("\tmemcpy((char*)&activeDrawEnv, env, sizeof(DRAWENV));\n\treturn 0;\n",
 "\tmemcpy((char*)&activeDrawEnv, env, sizeof(DRAWENV));\n"
 "\t/* _xeno_isbg: honor the DRAWENV background-clear flag (upstream TODO).\n"
 "\t * Real libgpu fills the clip rect on env-apply when isbg is set; the\n"
 "\t * menu and field both rely on the per-frame clear. */\n"
 "\tif (env->isbg)\n"
 "\t\tClearImage(&env->clip, env->r0, env->g0, env->b0);\n"
 "\treturn 0;\n", 1)])

print("    isbg background-clear patch OK")
ISBG_PY

# PsyCross MoveImage->GL materialize (the menu-trails fix): the menu's
# per-frame clear is a MoveImage restoring its backdrop VRAM rect over the
# draw framebuffer, then prims draw on top. PsyX renders prims into the GL
# backbuffer only -- the restored backdrop never reaches GL, so the menu's
# quads accumulate across frames (trails). Fix: when a MoveImage dest overlaps
# the active draw env clip, blit the freshly-restored vram rect over the
# backbuffer as the frame's base layer (vram-texture FBO -> default fb,
# y-flipped, full-window -- prims then composite on a clean base). Both files
# compile as C++ (PsyCross .C convention), so plain C++ linkage.
python3 - "$PSX" <<'FBMAT_PY'
import sys

psx = sys.argv[1]
libgpu = psx + "/src/psx/LIBGPU.C"
ren = psx + "/src/render/PsyX_render.cpp"

def edit(path, marker, pairs):
    with open(path) as f:
        s = f.read()
    if marker in s:
        return
    for old, new, count in pairs:
        found = s.count(old)
        if found != count:
            sys.exit("ERROR: fb-materialize patch anchor mismatch in %s for %s "
                     "(found %d, expected %d): %r" %
                     (path, marker, found, count, old[:80]))
        s = s.replace(old, new)
    with open(path, "w") as f:
        f.write(s)

edit(ren, "_xeno_fb_materialize_impl", [
("void GR_SwapWindow()\n",
 "/* _xeno_fb_materialize_impl: composite a freshly MoveImage-restored draw-\n"
 " * framebuffer rect into the GL backbuffer as the frame's base layer (the\n"
 " * menu's per-frame backdrop restore; prims composite on top). */\n"
 "void GR_MaterializeFramebufferRect(int x, int y, int w, int h)\n"
 "{\n"
 "#if USE_OPENGL\n"
 "\tGR_UpdateVRAM();\n"
 "\tglBindFramebuffer(GL_READ_FRAMEBUFFER, g_glBlitFramebuffer);\n"
 "\tglFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, g_vramTexture, 0);\n"
 "\tglBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);\n"
 "\tglBlitFramebuffer(x, y, x + w, y + h,\n"
 "\t                  0, g_windowHeight, g_windowWidth, 0,\n"
 "\t                  GL_COLOR_BUFFER_BIT, GL_NEAREST);\n"
 "\tglBindFramebuffer(GL_FRAMEBUFFER, 0);\n"
 "#endif\n"
 "}\n"
 "\n"
 "void GR_SwapWindow()\n", 1)])

edit(libgpu, "_xeno_fb_materialize", [
("int MoveImage(RECT16* rect, int x, int y)\n{\n"
 "\tGR_CopyVRAM(NULL, rect->x, rect->y, rect->w, rect->h, x, y);\n"
 "\treturn 0;\n}",
 "int MoveImage(RECT16* rect, int x, int y)\n{\n"
 "\tGR_CopyVRAM(NULL, rect->x, rect->y, rect->w, rect->h, x, y);\n"
 "\t/* _xeno_fb_materialize: a MoveImage restoring the ACTIVE draw\n"
 "\t * framebuffer is the game's per-frame backdrop (the menu); composite it\n"
 "\t * into the GL backbuffer as the frame's base layer. Dests outside the\n"
 "\t * draw clip (texture/backup moves) are untouched. */\n"
 "\tif (x < activeDrawEnv.clip.x + activeDrawEnv.clip.w &&\n"
 "\t    x + rect->w > activeDrawEnv.clip.x &&\n"
 "\t    y < activeDrawEnv.clip.y + activeDrawEnv.clip.h &&\n"
 "\t    y + rect->h > activeDrawEnv.clip.y)\n"
 "\t{\n"
 "\t\textern void GR_MaterializeFramebufferRect(int x, int y, int w, int h);\n"
 "\t\tGR_MaterializeFramebufferRect(x, y, rect->w, rect->h);\n"
 "\t}\n"
 "\treturn 0;\n}", 1)])

print("    MoveImage fb-materialize patch OK")
FBMAT_PY

# PsyCross fidelity fixes kept as tracked patches because the vendored tree is
# gitignored. Apply in dependency order: the ABR patch was generated after the
# raw-texture dither correction.
#
# PsyCross is an untracked vendor tree. `git -C $PSX apply` only works correctly
# when $PSX is its own git worktree. Without a local .git, git walks up to the
# monorepo, reverse --check can false-positive, and optional F22/F24/F26/F28
# patches are silently skipped. Ensure a local vendor repo before applying.
ensure_psycross_git_worktree() {
    local git_bin="$1"
    local psx_abs
    psx_abs="$(cd "$PSX" && pwd)"
    local toplevel
    toplevel="$("$git_bin" -C "$PSX" rev-parse --show-toplevel 2>/dev/null || true)"
    if [ -n "$toplevel" ] && [ "$toplevel" = "$psx_abs" ]; then
        return 0
    fi
    echo "    initializing local PsyCross git worktree for patch apply"
    "$git_bin" -C "$PSX" init -q
    "$git_bin" -C "$PSX" -c user.email=xeno@local -c user.name=xeno add -A
    "$git_bin" -C "$PSX" -c user.email=xeno@local -c user.name=xeno \
        commit -q -m "vendor baseline for patch apply" || true
    toplevel="$("$git_bin" -C "$PSX" rev-parse --show-toplevel 2>/dev/null || true)"
    if [ "$toplevel" != "$psx_abs" ]; then
        echo "ERROR: PsyCross git toplevel is '$toplevel', expected '$psx_abs'" >&2
        exit 1
    fi
}

apply_psycross_patch() {
    local patch="$1"
    local marker="$2"
    local patch_mode="${3:-}"
    local apply_args=()
    if [ "$patch_mode" = "unidiff-zero" ]; then
        apply_args+=(--unidiff-zero)
    fi
    if grep -Rqs "$marker" "$PSX"; then
        return
    fi
    local git_bin="$(command -v git || true)"
    if [ -z "$git_bin" ] && [ -x /run/host/usr/bin/git ]; then
        git_bin=/run/host/usr/bin/git
    fi
    if [ -z "$git_bin" ]; then
        echo "ERROR: git is required to apply PsyCross source patches" >&2
        exit 1
    fi
    ensure_psycross_git_worktree "$git_bin"
    if "$git_bin" -C "$PSX" apply "${apply_args[@]}" --reverse --check "$patch" >/dev/null 2>&1; then
        return
    fi
    "$git_bin" -C "$PSX" apply "${apply_args[@]}" --check "$patch" || {
        echo "ERROR: PsyCross patch does not apply: $patch" >&2
        exit 1
    }
    "$git_bin" -C "$PSX" apply "${apply_args[@]}" "$patch"
}

apply_psycross_patch "$ROOT/pc_port/patches/psycross_raw_texture_dither.patch" "_xeno_raw_texture_dither"
apply_psycross_patch "$ROOT/pc_port/patches/psycross_abr_clut_bit15.patch" "_xeno_clut_bit15_abr"
apply_psycross_patch "$ROOT/pc_port/patches/psycross_compmatrix_alias.patch" "_xeno_compmatrix_alias"
# Optional F22 logical-origin correction. This is intentionally applied only
# by the on-screen non-PGXP branch in GR_SetOffscreenState; unset/0 retains the
# original GR_Ortho2D call byte-for-byte, while offscreen VRAM and PGXP paths
# remain untouched.
apply_psycross_patch "$ROOT/pc_port/patches/psycross_halfpixel_origin.patch" "_xeno_half_pixel_origin"
# Optional F24 texel-center correction. This is independent of the F22
# coverage-origin correction and is enabled only for on-screen non-PGXP
# textured draws; offscreen VRAM and PGXP paths force the shader uniform off.
apply_psycross_patch "$ROOT/pc_port/patches/psycross_texel_center.patch" "_xeno_texel_center" "unidiff-zero"
# Optional F26 exact fixed-point UV interpolation. Polygon producers mark only
# FT3/FT4/GT3/GT4 vertices; the renderer then uses the PCSX-Redux edge/span
# arithmetic for on-screen non-PGXP genuine-PS1 textured triangles. Its
# internal +0x8000 replaces F24's shader offset while active. Sprites, tiles,
# glyphs, placeholders, PGXP, offscreen VRAM, and the disabled path stay on
# their existing routes.
apply_psycross_patch "$ROOT/pc_port/patches/psycross_fixed_uv.patch" "_xeno_fixed_uv_option" "unidiff-zero"
# Optional F28 exact raw-texture modulation identity. A dedicated vertex
# marker selects raw FT3/FT4 polygons only; the uniform is restricted to
# on-screen non-PGXP PS1-textured draws and is disabled by default.
apply_psycross_patch "$ROOT/pc_port/patches/psycross_raw_texture_identity.patch" "_xeno_raw_identity_option" "unidiff-zero"
# SPRT/SPRT_8/SPRT_16 rectangles share MakeTexcoordRect. Pair their upper
# screen edge with the base V so PSX top-down VRAM rows stay upright in the
# presented window; polygon UV paths remain independent.
apply_psycross_patch "$ROOT/pc_port/patches/psycross_sprite_v_orientation.patch" "_xeno_sprite_v_orientation" "unidiff-zero"
# World-map acceptance captures are requested by the game loop but fulfilled
# at PsyX_EndScene's sole effective pre-swap presentation boundary.
apply_psycross_patch "$ROOT/pc_port/patches/psycross_world_capture_request.patch" "_xeno_world_capture_request" "unidiff-zero"
# OpenGL framebuffer readback starts at the lower-left. Flip complete RGBA
# rows before SDL serializes screenshots so capture files match presentation.
apply_psycross_patch "$ROOT/pc_port/patches/psycross_capture_readback_orientation.patch" "_xeno_capture_readback_orientation" "unidiff-zero"
# Texture-cache format key (F10): GR_SetTexture's cache early-returned on
# texture ID alone (PsyX_render.cpp GR_SetTexture), and the return fires
# BEFORE the per-shader sampler uniforms (u_tex=0/u_lut=1) are initialized.
# A same-ID 4-bit -> 8-bit transition (observed on texture ID 5 in Lahan)
# left the 8-bit shader's LUT sampler at its GL default 0, so it decoded
# VRAM as its own palette (yellow/black model corruption). The cache key is
# now (texture ID, texture format); both cached fields reset together in
# GR_BeginScene and update only AFTER the sampler uniforms are set, so a
# failure between bind and uniform-set cannot poison the cache.
apply_psycross_patch "$ROOT/pc_port/patches/psycross_texcache_format_key.patch" "_xeno_texcache_format_key"
# Sound SDK Phase 0+1: SPU backend accessor + the RCnt2 (counter-2) event pump
# (OpenEvent/EnableEvent/DisableEvent registry + 240Hz dispatch on the interrupt
# thread). See OPEN_ISSUES.md "Sound cold-init" and pc_port/src/port_main.c.
apply_psycross_patch "$ROOT/pc_port/patches/psycross_sound_pump.patch" "_xeno_sound_pump"
# Sound SDK Phase 2 (init-proof): wire the 5 init-reached SDK primitives against
# the awake backend -- SpuSetReverbModeType/Depth + SpuSetReverbModeParam/Get
# (shared reverb state + EFX preset table -> OpenAL reverb), SpuSetCommonAttr
# (master volume -> listener gain), SpuSetIRQ/SpuSetIRQCallback (state only; no
# SPU IRQ source in the port). Behavioral, not objdiff-matched. See OPEN_ISSUES.md.
apply_psycross_patch "$ROOT/pc_port/patches/psycross_sound_prims.patch" "_xeno_sound_prims"
# Sound tick gate (tick-leg step 1, gate-first): g_SoundTickMutex, a recursive
# mutex mapping retail's DisableEvent/EnableEvent(g_unk_SoundEvent) bracket.
# DisableEvent on the counter-2 tick event acquires-and-holds, EnableEvent
# releases, the 240Hz pump TRY-locks around dispatch (a held bracket drops the
# tick, matching retail's disabled-event semantics; the pump can never stall
# or deadlock). Prerequisite for every real tick-body decomp. See OPEN_ISSUES.md.
apply_psycross_patch "$ROOT/pc_port/patches/psycross_sound_gate.patch" "_xeno_sound_gate"
# ADSR fidelity phase 1: per-voice hardware ADSR envelope (psx-spx 44100Hz
# counter machine) in the SPU backend -- KON=attack-from-zero, KOFF=release
# tail (source kept playing to level 0), composed single-writer AL_GAIN
# (base volume x envelope), live ENVX readback (SpuGetVoiceEnvelopeAttr, real
# -- backs seq cmd 0xFF's free-on-decay), ALC_REFRESH=240 render-granularity
# hint. Generated on top of the sound patches above (apply order matters).
# See OPEN_ISSUES.md "Audio fidelity: hardware-ADSR".
apply_psycross_patch "$ROOT/pc_port/patches/psycross_sound_adsr.patch" "_xeno_sound_adsr"
# ADPCM/Gaussian fidelity (the streaming mix-stage): SPU-faithful per-voice
# synthesis via AL_SOFT_callback_buffer -- integer-exact ADPCM decode,
# hardware loop/End+Mute semantics, 4-tap Gaussian resampling at the live
# pitch counter, AL_PITCH pinned 1.0.  Mixer-thread state handoff under
# s_StreamMutex (never held across AL calls); legacy cubic path kept
# (XENO_SOUND_LEGACY_RESAMPLER=1 / emscripten / no-extension fallback).
# Generated on top of the sound patches above (apply order matters).
apply_psycross_patch "$ROOT/pc_port/patches/psycross_sound_adpcm.patch" "_xeno_sound_adpcm"
# STR movie streaming (movie player module, retail cdstream): CdControl
# CdlSetmode/CdlReadS(NULL) start the spooler from the CdControlB(CdlSetloc)
# position, CdControlB CdlSetfilter/CdlPause report success (retail spins on
# a zero return), CdlPause joins the spooler thread, and the spooler paces
# sector delivery at the drive rate (75/s, 150/s with CdlModeSpeed) so the
# 32-slot STR ring is not overrun.  Generated against the tree with the
# patches above applied (the cdsync hunk sits in the same function).
apply_psycross_patch "$ROOT/pc_port/patches/psycross_cd_stream_movie.patch" "_xeno_cd_stream_movie"
# PSX display semantics for frames that draw no primitives: the STR movie
# player LoadImages pictures straight into the DISPENV buffer and flips with
# PutDispEnv, but PsyCross only presents rendered primitives.  Adds
# PsyX_IsSceneOpen / PsyX_PresentDisplayFromVRAM (CPU VRAM mirror -> RGBA8,
# 15-bit or isrgb24 24-bit, blitted over the window); pc_port's Vsync calls
# it at a blocking Vsync(0) when no scene was opened.
apply_psycross_patch "$ROOT/pc_port/patches/psycross_display_present.patch" "_xeno_display_present"
# CPU vram[] must mirror the presented framebuffer: on software GL the
# PBO/glGetTexImage download of the blit staging texture returns zeros, so
# every game-side MoveImage/StoreImage of the display area (field->menu
# backdrop snapshot func_800A476C, fades, distortion) copied black. Read the
# still-unswapped backbuffer into vram[] at present time instead.
apply_psycross_patch "$ROOT/pc_port/patches/psycross_fb_mirror_readpixels.patch" "_xeno_fb_mirror_readpixels"

echo "==> [1/5] Building PsyCross (libpsycross.a) via CMake"
# Drop a stale CMake cache generated under a different absolute path (e.g. from a
# different container mount) so it reconfigures cleanly in the current env.
if [ -f "$PSYX_BUILD/CMakeCache.txt" ] && \
   ! grep -q "CMAKE_HOME_DIRECTORY:INTERNAL=$ROOT/pc_port" "$PSYX_BUILD/CMakeCache.txt"; then
    echo "    (removing stale CMake cache)"; rm -rf "$PSYX_BUILD"
fi
if [ -n "$TSAN_FLAGS" ]; then
    cmake -S pc_port -B "$PSYX_BUILD" -DCMAKE_BUILD_TYPE=Debug \
        -DCMAKE_C_FLAGS="$TSAN_FLAGS" -DCMAKE_CXX_FLAGS="$TSAN_FLAGS" >/dev/null
else
    cmake -S pc_port -B "$PSYX_BUILD" -DCMAKE_BUILD_TYPE=Debug >/dev/null
fi
cmake --build "$PSYX_BUILD" --target psycross -j"$(nproc)" >/dev/null
PSYLIB="$(find "$PSYX_BUILD" -name 'libpsycross.a' 2>/dev/null | head -1)"
if [ -z "$PSYLIB" ] || [ ! -s "$PSYLIB" ]; then
    echo "ERROR: libpsycross.a was not built (the CMake step failed above)."
    echo "       Make sure you're in the distrobox with SDL2/OpenAL/OpenGL dev installed."
    exit 1
fi
echo "    libpsycross.a: $PSYLIB"

echo "==> [2/5] Compiling game translation units in port mode"
print_intentionally_excluded_game_tus
print_reference_only_game_tus
print_known_broken_game_tus

compiled=0; skipped=0; SKIPPED=""
GAME_OBJS=()
GAME_TU_OBJS=()
PORT_OVERRIDE_MANIFEST="pc_port/port_owned_overrides.txt"
if [ ! -f "$PORT_OVERRIDE_MANIFEST" ]; then
    echo "ERROR: missing port ownership manifest: $PORT_OVERRIDE_MANIFEST"
    exit 1
fi
PORT_OVERRIDE_SYMBOLS=()
declare -A PORT_OVERRIDE_REASON=()
declare -A PORT_OVERRIDE_EXIT=()
while IFS='|' read -r sym reason exit_condition; do
    if [[ ! "$sym" =~ ^[A-Za-z_][A-Za-z0-9_]*$ ]]; then
        echo "ERROR: invalid symbol in $PORT_OVERRIDE_MANIFEST: $sym"
        exit 1
    fi
    if [ -z "$reason" ] || [ -z "$exit_condition" ]; then
        echo "ERROR: manifest row needs reason and exit condition: $sym"
        exit 1
    fi
    PORT_OVERRIDE_SYMBOLS+=("$sym")
    PORT_OVERRIDE_REASON[$sym]="$reason"
    PORT_OVERRIDE_EXIT[$sym]="$exit_condition"
done < <(sed -e '/^[[:space:]]*#/d' -e '/^[[:space:]]*$/d' "$PORT_OVERRIDE_MANIFEST")
if [ "${#PORT_OVERRIDE_SYMBOLS[@]}" -eq 0 ]; then
    echo "ERROR: empty port ownership manifest: $PORT_OVERRIDE_MANIFEST"
    exit 1
fi
declare -A PORT_OVERRIDE_SEEN=()
for sym in "${PORT_OVERRIDE_SYMBOLS[@]}"; do
    if [ "${PORT_OVERRIDE_SEEN[$sym]+set}" = set ]; then
        echo "ERROR: duplicate symbol in $PORT_OVERRIDE_MANIFEST: $sym"
        exit 1
    fi
    PORT_OVERRIDE_SEEN[$sym]=1
done
while IFS= read -r f; do
    # Reference-only TUs remain in the matching build but have a deliberate,
    # complete port-side runtime owner and never enter the native port link.
    if is_reference_only_game_tu "$f"; then
        # Do not leave a formerly-compiled object available for accidental
        # reuse after a TU changes ownership classification.
        rm -f "$OBJ/$(echo "$f" | tr '/' '_').o" \
              "$OUT/$(echo "$f" | tr '/' '_').err"
        continue
    fi

    # Explicit exclusions are deliberately absent from both port compilation
    # and link while their routing decision remains held.
    if is_intentionally_excluded_game_tu "$f"; then
        continue
    fi

    o="$OBJ/$(echo "$f" | tr '/' '_').o"
    compile_err="$OUT/$(echo "$f" | tr '/' '_').err"
    rm -f "$o" "$compile_err"
    if gcc -c "$f" $GFLAGS $INC -o "$o" 2>"$compile_err"; then
        if is_known_broken_game_tu "$f"; then
            echo "ERROR: allowlisted broken game TU now compiles: $f"
            echo "       It may not silently enter the link; classify it as intentionally excluded"
            echo "       or remove it from the broken list after a reviewed routing decision."
            exit 1
        fi
        GAME_OBJS+=("$o")
        GAME_TU_OBJS+=("$o")
        compiled=$((compiled+1))
    elif is_known_broken_game_tu "$f"; then
        skipped=$((skipped+1)); SKIPPED="$SKIPPED $f"
        echo "    WARNING: allowlisted broken game TU failed: $f"
        echo "      $(known_broken_tu_reason "$f")"
        sed 's/^/      | /' "$compile_err"
    else
        echo "ERROR: game TU compilation failed and is not explicitly allowlisted."
        echo "       TU: $f"
        sed 's/^/       | /' "$compile_err"
        echo "ERROR: aborting; fix the TU or add a reviewed temporary allowlist entry."
        exit 1
    fi
done < <(find src -name '*.c' | sort)
echo "    compiled=$compiled  skipped=$skipped"
if [ -n "$SKIPPED" ]; then
    echo "    WARNING: skipped only by the explicit temporary allowlist:$SKIPPED"
    print_known_broken_game_tus
fi

echo "==> [2b/5] Compiling port-only sources (PSX RAM emu, overrides/dispatch table)"
PORT_SOURCES=(
    pc_port/src/psx_memory.c
    pc_port/src/guest_prim_link.c
    pc_port/src/model_prim_link.c
    pc_port/src/test_input.c
    pc_port/src/game_overrides.c
    pc_port/src/boot_menu.c
    pc_port/src/boot_str.c
    pc_port/src/boot_assets.c
    pc_port/src/movie_player.c
    pc_port/src/field_object_overlay.c
    pc_port/src/world_map_init.c
    pc_port/src/world_map_frame_driver.c
    pc_port/src/world_map_image_transfer_25044.c
    pc_port/src/world_map_upload_pump_74f2c.c
    pc_port/src/world_map_upload_pump_75104.c
    pc_port/src/world_map_frame_tail_71984.c
    pc_port/src/world_map_ot_adapter.c
    pc_port/src/world_map_frame_driver_712d0.c
    pc_port/src/world_map_pause.c
    pc_port/src/world_map_main_loop_71034.c
    pc_port/src/world_map_terminal_default_71264.c
    pc_port/src/world_map_terminal_one_711b0.c
    pc_port/src/world_map_terminal_zero_710e4.c
    pc_port/src/world_map_teardown_7299c.c
    pc_port/src/world_map_mode811_lifecycle.c
    pc_port/src/world_map_mode9_lifecycle.c
    pc_port/src/world_map_mode10_lifecycle.c
    pc_port/src/world_map_mode12_lifecycle.c
    pc_port/src/world_map_mode13_lifecycle.c
    pc_port/src/world_map_mode14_lifecycle.c
    pc_port/src/world_map_mode15_lifecycle.c
    pc_port/src/world_map_mode16_lifecycle.c
    pc_port/src/world_map_mode17_lifecycle.c
    pc_port/src/world_map_mode18_lifecycle.c
    pc_port/src/world_map_capture.c
    pc_port/src/world_map_gamestate_alias.c
    pc_port/src/world_map_selector.c
    pc_port/src/psyq_compat.c
    pc_port/src/archive_port.c
    pc_port/src/work_list_port.c
    pc_port/src/sound_transfer_callbacks.c
    pc_port/src/data_published_logo.c
    pc_port/src/data_font.c
    pc_port/src/data_kernel_menu.c
    pc_port/src/data_field.c
    pc_port/src/data_member_change_menu.c
    pc_port/src/data_main_menu.c
    pc_port/src/world_map_convergence.c
    pc_port/src/world_map_framebuffer_init.c
    pc_port/src/world_map_terrain_init.c
    pc_port/src/world_map_terrain_cell.c
    pc_port/src/world_map_plane_solver.c
    pc_port/src/world_map_terrain_normal.c
    pc_port/src/world_map_terrain_sampler.c
    pc_port/src/world_map_common_tail.c
    pc_port/src/world_map_scheduler.c
    pc_port/src/world_map_callback_923a8.c
    pc_port/src/world_map_callback_925a0.c
    pc_port/src/world_map_callback_8a2c8.c
    pc_port/src/world_map_callback_8b2bc.c
    pc_port/src/world_map_callback_8bb40.c
    pc_port/src/world_map_callback_8c530.c
    pc_port/src/world_map_callback_8d3f0.c
    pc_port/src/world_map_callback_8dd6c.c
    pc_port/src/world_map_callback_8e190.c
    pc_port/src/world_map_callback_906e0.c
    pc_port/src/world_map_callback_91430.c
    pc_port/src/world_map_callback_91b54.c
    pc_port/src/world_map_callback_92234.c
    pc_port/src/world_map_callback_92be4.c
    pc_port/src/world_map_callback_92df8.c
    pc_port/src/world_map_callback_71a50.c
    pc_port/src/world_map_callback_87710.c
    pc_port/src/world_map_callback_87734.c
    pc_port/src/world_map_callback_7756c.c
    pc_port/src/world_map_callback_76a14.c
    pc_port/src/world_map_callback_78948.c
    pc_port/src/world_map_callback_77dc8.c
    pc_port/src/world_map_callback_78e2c.c
    pc_port/src/world_map_callback_794d8.c
    pc_port/src/world_map_callback_795e4.c
    pc_port/src/world_map_callback_7a144.c
    pc_port/src/world_map_callback_7a9b4.c
    pc_port/src/world_map_callback_7ad34.c
    pc_port/src/world_map_callback_7b200.c
    pc_port/src/world_map_callback_7b604.c
    pc_port/src/world_map_callback_7ba08.c
    pc_port/src/world_map_callback_7bb60.c
    pc_port/src/world_map_callback_7c36c.c
    pc_port/src/world_map_callback_7c724.c
    pc_port/src/world_map_callback_7de14.c
    pc_port/src/world_map_callback_7e450.c
    pc_port/src/world_map_callback_7eca4.c
    pc_port/src/world_map_callback_7f8ac.c
    pc_port/src/world_map_callback_7fc8c.c
    pc_port/src/world_map_callback_8032c.c
    pc_port/src/world_map_callback_80578.c
    pc_port/src/world_map_callback_80900.c
    pc_port/src/world_map_callback_80a28.c
    pc_port/src/world_map_callback_81174.c
    pc_port/src/world_map_callback_813e8.c
    pc_port/src/world_map_callback_817a0.c
    pc_port/src/world_map_callback_819c8.c
    pc_port/src/world_map_callback_81c3c.c
    pc_port/src/world_map_callback_81fb4.c
    pc_port/src/world_map_callback_827c8.c
    pc_port/src/world_map_callback_827ec.c
    pc_port/src/world_map_callback_83214.c
    pc_port/src/world_map_callback_834d0.c
    pc_port/src/world_map_callback_838e8.c
    pc_port/src/world_map_callback_83a00.c
    pc_port/src/world_map_callback_84068.c
    pc_port/src/world_map_cold_defaults.c
    pc_port/src/world_map_mode_selector_73300.c
    pc_port/src/psyq_normal_light_col.c
    pc_port/src/psyq_spu_noise_clock.c
    pc_port/src/world_map_callback_7cc6c.c
    pc_port/src/world_map_callback_7ce84.c
    pc_port/src/world_map_callback_7d078.c
    pc_port/src/world_map_callback_7d228.c
    pc_port/src/world_map_callback_7d414.c
    pc_port/src/world_map_callback_7d600.c
    pc_port/src/world_map_callback_7d774.c
    pc_port/src/world_map_callback_8a72c.c
    pc_port/src/world_map_callback_8b644.c
    pc_port/src/world_map_callback_8c844.c
    pc_port/src/world_map_callback_8d678.c
    pc_port/src/world_map_callback_8e76c.c
    pc_port/src/world_map_state2_8eb64.c
    pc_port/src/world_map_callback_914d0.c
    pc_port/src/world_map_callback_91c18.c
    pc_port/src/world_map_callback_922ac.c
    pc_port/src/world_map_callback_92c70.c
    pc_port/src/world_map_callback_92fd8.c
    pc_port/src/world_map_r4world_71a58.c
    pc_port/src/world_map_helper_8bfd4.c
    pc_port/src/world_map_helper_8e078.c
    pc_port/src/world_map_helper_8e0f0.c
    pc_port/src/world_map_helper_90c68.c
    pc_port/src/world_map_helper_90e14.c
    pc_port/src/world_map_helper_907f4.c
    pc_port/src/world_map_helper_90fb4.c
    pc_port/src/world_map_helper_91ff8.c
    pc_port/src/world_map_helper_3101c.c
    pc_port/src/world_map_helper_93484.c
    pc_port/src/world_map_helper_96f18.c
    pc_port/src/world_map_helper_93a5c.c
    pc_port/src/world_map_helper_95cd4.c
    pc_port/src/world_map_helper_9623c.c
    pc_port/src/world_map_helper_983a0.c
    pc_port/src/world_map_helper_987ac.c
    pc_port/src/world_map_helper_9980c.c
    pc_port/src/world_map_helper_96328.c
    pc_port/src/world_map_helper_97dc0.c
    pc_port/src/world_map_helper_980d4.c
    pc_port/src/world_map_helper_73398.c
    pc_port/src/world_map_helper_73448.c
    pc_port/src/world_map_helper_72db4.c
    pc_port/src/world_map_helper_76858.c
    pc_port/src/world_map_helper_76954.c
    pc_port/src/world_map_helper_97070.c
    pc_port/src/world_map_helper_94154.c
    pc_port/src/world_map_helper_7565c.c
    pc_port/src/world_map_helper_75d4c.c
    pc_port/src/world_map_helper_75e7c.c
    pc_port/src/world_map_helper_762fc.c
    pc_port/src/world_map_helper_86124.c
    pc_port/src/world_map_helper_71fec.c
    pc_port/src/world_map_menu_lifecycle.c
    pc_port/src/world_map_session_setup_72238.c
    pc_port/src/world_map_helper_96130.c
    pc_port/src/world_map_helper_966cc.c
    pc_port/src/world_map_helper_737ec.c
    pc_port/src/world_map_helper_747dc.c
    pc_port/src/world_map_helper_740b8.c
    pc_port/src/world_map_helper_97244.c
    pc_port/src/world_map_helper_965a4.c
    pc_port/src/world_map_helper_89580.c
    pc_port/src/world_map_helper_89748.c
    pc_port/src/world_map_helper_98cc0.c
    pc_port/src/world_map_helper_9932c.c
    pc_port/src/world_map_helper_99bfc.c
    pc_port/src/world_map_helper_99708.c
    pc_port/src/world_map_helper_981c8.c
    pc_port/src/world_map_helper_848b4.c
    pc_port/src/world_map_helper_848f4.c
    pc_port/src/world_map_helper_85cdc.c
    pc_port/src/world_map_helper_89c78.c
    pc_port/src/world_map_helper_8615c.c
    pc_port/src/world_map_helper_86798.c
    pc_port/src/world_map_helper_c28c.c
    pc_port/src/world_map_helper_c364.c
    pc_port/src/world_map_helper_8dff4.c
    pc_port/src/world_map_helper_7528c.c
    pc_port/src/world_map_helper_8e034.c
    pc_port/src/world_map_helper_73b04.c
    pc_port/src/world_map_helper_90a84.c
    pc_port/src/world_map_helper_74794.c
    pc_port/src/world_map_helper_85760.c
    pc_port/src/world_map_helper_85418.c
    pc_port/src/world_map_helper_93354.c
    pc_port/src/world_map_helper_952b0.c
    pc_port/src/world_map_helper_95324.c
    pc_port/src/world_map_helper_93fe4.c
    pc_port/src/world_map_helper_94088.c
    pc_port/src/world_map_helper_951a8.c
    pc_port/src/world_map_helper_85158.c
    pc_port/src/world_map_helper_84db8.c
    pc_port/src/world_map_helper_84d00.c
    pc_port/src/world_map_helper_97770.c
    pc_port/src/world_map_helper_941c4.c
    pc_port/src/world_map_helper_94238.c
    pc_port/src/world_map_helper_94364.c
    pc_port/src/world_map_helper_8bec8.c
    pc_port/src/world_map_helper_8c1dc.c
    pc_port/src/world_map_helper_93534.c
    pc_port/src/world_map_helper_93e8c.c
    pc_port/src/world_map_helper_93f18.c
    pc_port/src/world_map_helper_94060.c
    pc_port/src/world_map_helper_8c040.c
    pc_port/src/world_map_private_collision.c
    pc_port/src/world_map_func_94a5c.c
    pc_port/src/world_map_func_95414.c
)

# The list above is the port link's explicit ownership registry. Refuse to
# silently ignore a new port-side TU: every C source under pc_port/src must be
# either the separately-built entry point or present in PORT_SOURCES.
while IFS= read -r pf; do
    [ "$pf" = "pc_port/src/port_main.c" ] && continue
    listed=0
    for listed_pf in "${PORT_SOURCES[@]}"; do
        if [ "$pf" = "$listed_pf" ]; then
            listed=1
            break
        fi
    done
    if [ "$listed" -eq 0 ]; then
        echo "ERROR: unclassified port source is not in PORT_SOURCES: $pf"
        echo "ERROR: aborting before trial link and stub generation."
        exit 1
    fi
done < <(find pc_port/src -type f -name '*.c' | sort)

for pf in "${PORT_SOURCES[@]}"; do
    if [ ! -f "$pf" ]; then
        echo "ERROR: listed port source does not exist: $pf"
        echo "ERROR: aborting before trial link and stub generation."
        exit 1
    fi
    o="$OBJ/$(basename "$pf").o"
    compile_err="$OUT/$(echo "$pf" | tr '/' '_').err"
    rm -f "$o" "$compile_err"
    if ! gcc -c "$pf" $GFLAGS -Ipc_port/src $INC -o "$o" 2>"$compile_err"; then
        echo "ERROR: port source compilation failed."
        echo "       TU: $pf"
        sed 's/^/       | /' "$compile_err"
        echo "ERROR: aborting before trial link and stub generation; port sources may not be stubbed."
        exit 1
    fi
    GAME_OBJS+=("$o")
    echo "    $(basename "$pf") ok"
done

# Port fallbacks are deliberately strong. If a matching game TU later gains
# one of these retail definitions, weaken that duplicate in the game object
# before linking. Relocations retain the original symbol name, so every caller
# binds uniformly to the strong port owner; matching sources stay retail-shaped.
PORT_OVERRIDE_OBJECT="$OBJ/game_overrides.c.o"
if [ ! -f "$PORT_OVERRIDE_OBJECT" ]; then
    echo "ERROR: port ownership object was not built: $PORT_OVERRIDE_OBJECT"
    exit 1
fi
for sym in "${PORT_OVERRIDE_SYMBOLS[@]}"; do
    if ! nm -g --defined-only "$PORT_OVERRIDE_OBJECT" 2>/dev/null \
        | awk -v sym="$sym" '$3 == sym {found=1} END {exit !found}'; then
        echo "ERROR: port ownership manifest symbol is not defined by $PORT_OVERRIDE_OBJECT: $sym"
        exit 1
    fi
    matched_objects=0
    for o in "${GAME_TU_OBJS[@]}"; do
        if nm -g --defined-only "$o" 2>/dev/null \
            | awk -v sym="$sym" '$3 == sym {found=1} END {exit !found}'; then
            matched_objects=$((matched_objects+1))
            echo "    port-owned override: weakening matching definition $sym in $(basename "$o")"
            if ! objcopy --weaken-symbol="$sym" "$o"; then
                echo "ERROR: failed to weaken duplicate matching definition: $sym"
                exit 1
            fi
        fi
    done
    if [ "$matched_objects" -eq 0 ]; then
        symbol_sources="$(rg -l --glob '*.c' "\\b${sym}\\s*\\(" src 2>/dev/null || true)"
        excluded_sources=""
        broken_sources=""
        reference_sources=""
        while IFS= read -r candidate; do
            [ -z "$candidate" ] && continue
            if is_intentionally_excluded_game_tu "$candidate"; then
                excluded_sources="$excluded_sources $candidate"
            elif is_known_broken_game_tu "$candidate"; then
                broken_sources="$broken_sources $candidate"
            elif is_reference_only_game_tu "$candidate"; then
                reference_sources="$reference_sources $candidate"
            fi
        done <<< "$symbol_sources"
        if [ -n "$excluded_sources" ]; then
            echo "ERROR: port ownership row has no compiled definition because the matching TU is intentionally excluded: $sym"
            echo "       Sources:$excluded_sources"
        elif [ -n "$broken_sources" ]; then
            echo "ERROR: port ownership row has no compiled definition because the matching TU is allowlisted broken: $sym"
            echo "       Sources:$broken_sources"
        elif [ -n "$reference_sources" ]; then
            echo "ERROR: port ownership row has no compiled definition because the matching TU is reference-only: $sym"
            echo "       Sources:$reference_sources"
        else
            echo "ERROR: stale port ownership row has no matching game definition: $sym"
        fi
        echo "       Retire the row and its fallback together, or restore the matching TU."
        exit 1
    fi
    for o in "${GAME_TU_OBJS[@]}"; do
        if nm -g --defined-only "$o" 2>/dev/null \
            | awk -v sym="$sym" '$3 == sym && $2 !~ /^[Ww]$/ {found=1} END {exit !found}'; then
            echo "ERROR: matching object still has a strong retired override symbol: $sym"
            exit 1
        fi
    done
done
echo "    verified ${#PORT_OVERRIDE_SYMBOLS[@]} port-owned override symbols with retirement metadata"

echo "==> [3/5] Compiling port entry point"
PORT_MAIN_SOURCE="pc_port/src/port_main.c"
PORT_MAIN_OBJECT="$OBJ/port_main.o"
PORT_MAIN_ERR="$OUT/pc_port_src_port_main.c.err"
rm -f "$PORT_MAIN_OBJECT" "$PORT_MAIN_ERR"
if ! gcc -c "$PORT_MAIN_SOURCE" $GFLAGS -Ipc_port/src -Iinclude \
    -I"$PSX/include" -I"$PSX/include/psx" -o "$PORT_MAIN_OBJECT" \
    2>"$PORT_MAIN_ERR"; then
    echo "ERROR: port entry-point compilation failed."
    echo "       TU: $PORT_MAIN_SOURCE"
    sed 's/^/       | /' "$PORT_MAIN_ERR"
    echo "ERROR: aborting before trial link and stub generation; a stale port_main.o will not be reused."
    exit 1
fi

LIBS="$(pkg-config --libs sdl2 openal 2>/dev/null) -lGL -lm -lpthread -ldl"
if [ -n "$TSAN_FLAGS" ]; then
    # TSan's EH instrumentation of the C++ PsyCross objects references the
    # C++ personality routine (__gxx_personality_v0); the normal build does
    # not need libstdc++ at all.
    LIBS="$LIBS -lstdc++"
fi
# -no-pie: link non-PIE so the executable loads at a fixed low base and ALL of
# its BSS (the emulated PSX RAM g_PsxRam[] plus every auto-stubbed data symbol)
# lives below 4 GiB. The decompiled game truncates its own pointers to 32 bits
# all over (e.g. the heap's `(u32)pHeapStart & -4`); keeping that memory in the
# low 32-bit address space makes every such truncation a lossless round-trip.
NOPIE="-no-pie -fno-pie"
LINK=(gcc -m64 $NOPIE $TSAN_FLAGS "$PORT_MAIN_OBJECT" "${GAME_OBJS[@]}" "$PSYLIB" $LIBS -o "$OUT/xeno-port")

echo "==> [4/5] Trial link to discover undefined references"
"${LINK[@]}" 2> "$OUT/link1.err"
grep -oE "undefined reference to \`[A-Za-z0-9_]+'" "$OUT/link1.err" \
    | sed -E "s/.*\`([A-Za-z0-9_]+)'/\1/" | sort -u > "$OUT/undef.txt"
echo "    undefined symbols to stub: $(wc -l < "$OUT/undef.txt")"

if [ -s "$OUT/undef.txt" ]; then
    ELFS=()
    for e in build/out/slus_006.64.elf build/out/field.elf build/out/member_change_menu.elf build/out/shop_menu.elf build/out/menu.elf; do
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
        # Without matching ELFs the generator cannot safely classify a newly
        # undefined symbol as function vs data, nor size new data storage.
        # An existing stubs.c is usable only as an exact typed manifest for
        # this undef set; silently reusing it for a different set would make
        # the build link against stale or wrongly-shaped placeholders.
        echo "    matching ELFs missing; validating existing stubs.c against current undef.txt"
        if ! python3 - "$OUT/undef.txt" "$OUT/stubs.c" <<'PY'
import re
import sys

undef_path, stubs_path = sys.argv[1:]
with open(undef_path) as f:
    undefined = set(f.read().split())

# xeno_port_stub is defined by stubs.c itself and intentionally appears during
# the trial link before stubs.o is linked.
undefined.discard("xeno_port_stub")
with open(stubs_path) as f:
    source = f.read()

stubbed = set(re.findall(r"^unsigned char\s+([A-Za-z_]\w*)\[", source, re.M))
stubbed.update(re.findall(r"^long\s+([A-Za-z_]\w*)\(void\)", source, re.M))
missing = sorted(undefined - stubbed)
stale = sorted(stubbed - undefined)
if missing or stale:
    print("ERROR: existing stubs.c does not exactly match current undef.txt.", file=sys.stderr)
    if missing:
        print("  missing typed stubs: " + ", ".join(missing[:16]) +
              (" ..." if len(missing) > 16 else ""), file=sys.stderr)
    if stale:
        print("  stale stubs: " + ", ".join(stale[:16]) +
              (" ..." if len(stale) > 16 else ""), file=sys.stderr)
    print("  Matching ELFs are required to regenerate a safe typed stubs.c.", file=sys.stderr)
    sys.exit(1)
print(f"    existing typed stub manifest matches {len(undefined)} current undefined symbols")
PY
        then
            echo "ERROR: matching ELFs are absent and stubs.c is stale."
            echo "       Provide matching ELFs to regenerate typed stubs; refusing to link."
            exit 1
        fi
    else
        echo "ERROR: matching ELFs missing and no typed $OUT/stubs.c manifest exists."
        echo "       Provide matching ELFs to generate safe function/data stubs."
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
    gcc -c "$OUT/stubs.c" -O0 -g $TSAN_FLAGS -o "$OBJ/stubs.o"
fi

echo "==> [5/5] Final link"
LINK_MAP="$OUT/xeno-port.map"
gcc -m64 $NOPIE $TSAN_FLAGS "$PORT_MAIN_OBJECT" "${GAME_OBJS[@]}" "$OBJ/stubs.o" "$PSYLIB" $LIBS \
    -Wl,-Map="$LINK_MAP" -o "$OUT/xeno-port" 2> "$OUT/link2.err"
if [ -f "$OUT/xeno-port" ] && [ ! -s "$OUT/link2.err" ]; then
    port_text_start=""
    port_text_size=""
    read -r port_text_start port_text_size < <(
        awk -v obj="$PORT_OVERRIDE_OBJECT" \
            '$1 == ".text" && $4 == obj {print $2, $3; exit}' "$LINK_MAP"
    )
    if [ -z "$port_text_start" ] || [ -z "$port_text_size" ]; then
        echo "ERROR: final link map has no .text range for $PORT_OVERRIDE_OBJECT"
        exit 1
    fi
    for sym in "${PORT_OVERRIDE_SYMBOLS[@]}"; do
        port_symbol_offset="$(nm -g --defined-only "$PORT_OVERRIDE_OBJECT" 2>/dev/null \
            | awk -v sym="$sym" '$3 == sym {print "0x" $1; exit}')"
        resolved_address="$(nm -g --defined-only "$OUT/xeno-port" 2>/dev/null \
            | awk -v sym="$sym" '$3 == sym {print "0x" $1; exit}')"
        if [ -z "$port_symbol_offset" ] || [ -z "$resolved_address" ]; then
            echo "ERROR: final link ownership check has no address for $sym"
            exit 1
        fi
        expected_address="$(printf '0x%016x' $((port_text_start + port_symbol_offset)))"
        if [ "$resolved_address" != "$expected_address" ]; then
            echo "ERROR: final link owner is not the port definition for $sym"
            echo "       resolved=$resolved_address expected_port=$expected_address"
            exit 1
        fi
    done
    echo "    LINK OK -> $OUT/xeno-port (port-owned addresses verified)"
else
    echo "    LINK incomplete; remaining errors:"
    grep -oE "undefined reference to \`[A-Za-z0-9_]+'|multiple definition of \`[A-Za-z0-9_]+'" "$OUT/link2.err" | sort | uniq -c | sort -rn | head -20
    exit 1
fi
