# Xenogears PC Port (experimental, Shipwright-style)

A native PC source port built **on top of the matching decompilation**, in the
spirit of [Ship of Harkinian](https://github.com/HarbourMasters/Shipwright) for
Zelda OoT and the [Silent Hill PC port](https://github.com/SlickAmogus/silent-hill-decomp)
for PSX. This directory is **separate from the matching (gears/ninja/MIPS)
build** and does not affect it.

## Architecture

```
  Xenogears game logic (../src/*)         <- decompiled C
            │  calls the PsyQ SDK (DrawOTag, RotTransPers, Spu*, CdRead, ...)
            ▼
  PsyCross  (PSX hardware abstraction layer)
    GTE in software · LibGPU -> OpenGL · LibSPU -> OpenAL ·
    LibCD -> BIN/CUE · controllers -> SDL2
```

PsyCross is the PSX equivalent of libultraship (the N64 HAL behind SoH). The
port compiles the game against PsyCross's PsyQ-compatible headers
(`extern/PsyCross/include/psx`) and links its implementation, **instead of** the
on-hardware reimplementations in `../src/slus_006.64/psyq/*`.

## Status

- [x] **Phase 0 (scaffold):** PsyCross vendored (`extern/PsyCross`, gitignored)
      and building from source; `xeno-port` links against it and brings the
      runtime up/down.
- [x] **Phase 1 pattern (proven on a slice):** a real game translation unit
      (`src/field/game_logic/gold.c`) compiles natively in port mode, its
      undefined symbols are auto-stubbed, and the linked binary **runs real
      decompiled game logic on x86**, with the stub oracle logging the next
      function the code path needs. See "Phase 1 loop" below.
- [ ] **Phase 1 (scale-up):** compile *all* game TUs, reconcile the full PsyQ
      header surface, link the whole executable, hand `main` off to the game.
- [ ] **Phase 2:** asset/disc extraction pipeline feeding PsyCross's LibCD.
- [ ] **Phase 3:** climb the boot path, replacing stubs with decompiled C until
      the intro/field renders.

> A native port requires every executed function to be real C — raw `INCLUDE_ASM`
> (MIPS) cannot run on x86. Port progress is therefore gated on functional
> decompilation coverage; byte-for-byte matching is **not** required for the port.

## The Phase 1 loop (boot-path-driven decompilation)

The port doubles as the decompilation priority oracle:

1. Compile game TUs in port mode (`-DXENO_PC_PORT -DSKIP_ASM`, against the
   `include_shim/` -> PsyCross headers). `INCLUDE_ASM` becomes inert.
2. Collect undefined symbols: `nm -u *.o`.
3. `tools/scripts/gen_port_stubs.py` classifies each (function vs data, via the
   matching-build ELF symbol tables) and emits logging stubs + zeroed storage.
4. Link the executable and run it. It runs real decompiled code until it hits a
   stub on the live path, which logs `[stub] <name>`.
5. Decompile that function (matching optional), rebuild, repeat. Each step
   extends how far the game runs — and grows the matching % at the same time.

Reproduce the proven slice (Linux, in a container/distrobox with `gcc`,
`libc6-dev`, `binutils`, `python3`):

```bash
# from repo root; build the matching ELFs first (for symbol classification)
#   (in the ethos container)  make -B build

gcc -c src/field/game_logic/gold.c -DXENO_PC_PORT -DSKIP_ASM -D_LANGUAGE_C \
    -include assert.h -w \
    -I pc_port/include_shim -I include \
    -I pc_port/extern/PsyCross/include -I pc_port/extern/PsyCross/include/psx \
    -o gold.o
nm -u gold.o | awk '{print $2}' | sort -u > undef.txt
python3 tools/scripts/gen_port_stubs.py \
    --elf build/out/slus_006.64.elf --elf build/out/field.elf \
    --undefined undef.txt --out pc_port/generated/stubs.c
```

## Building (Linux)

Requires `cmake`, a C/C++ compiler, and dev packages for **SDL2**, **OpenAL**,
and **OpenGL**. On the immutable Bazzite host, build inside a container or a
`distrobox` (Ubuntu shown):

```bash
# from repo root
git clone --depth 1 https://github.com/OpenDriver2/PsyCross.git pc_port/extern/PsyCross   # if not present

cd pc_port
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j"$(nproc)"
./build/xeno-port      # opens a window on a real display
```

### Linux portability notes (handled in `CMakeLists.txt`, no vendored edits)
- PsyCross's bundled CMake is bypassed (case-sensitive globs miss its uppercase
  `*.C` PSX files; it expects SDL2/OpenAL as subprojects). We compile its sources
  directly and use system SDL2/OpenAL/OpenGL.
- `src/port_compat.h` is force-included to supply `strcasecmp`/`strncasecmp`
  (PsyCross's `include/psx/strings.h` shadows the system header).
- `-fpermissive` downgrades 64-bit function-pointer-to-int casts in the PSX SDK
  code (Phase 1 will revisit any that land on the boot path).

## Files
- `CMakeLists.txt` — native build: compiles PsyCross + the port executable.
- `src/port_main.c` — entry point; brings up PsyCross (Phase 1 hands off to the game).
- `src/xeno_pc.h` — port build identity + `INCLUDE_ASM` neutralisation plumbing.
- `src/port_compat.h` — Linux/gcc compat shim for PsyCross.
- `extern/PsyCross/` — vendored HAL (gitignored).
