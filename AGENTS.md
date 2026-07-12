# AGENTS.md

## Cursor Cloud specific instructions

This repo is a Xenogears (PSX, SLUS 006.64) matching decompilation **plus** an
experimental native PC port. There are two independent build targets:

1. **Matching decomp** (root `Makefile`, `gears` + `ninja` + MIPS toolchain) —
   see `README.md`. Produces `build/out/*.elf` and checks byte-matching.
2. **PC port** (`pc_port/`, native SDL2/OpenAL/OpenGL) — the runnable app. See
   `pc_port/README.md` and `pc_port/build_port.sh`.

### Copyrighted disc data is required for full builds (NOT in the repo)
Both targets ultimately need the original Xenogears disc data, which is **not**
committed. CI downloads it from repo secrets (`FILE_URL`, `FILE_URL_2..5`) into
`disc/` (`SLUS_006.64`, `movie.bin`, `field.bin`, `member_change_menu.bin`,
`shop_menu.bin`), and the PC port additionally reads assets from `disc/disc1.bin`
(override with `XENO_DISC`). Without it:
- `make build` / `make check` cannot run (`gears` has nothing to disassemble).
- `pc_port/build_port.sh` compiles everything and trial-links, then **stops at
  stub generation** because `gen_port_stubs.py` classifies not-yet-decompiled
  symbols using `build/out/*.elf` (which only exist after a matching build).

To do a full, correct build: place the disc files in `disc/`, run `make build`
(with the Python venv active), then `./pc_port/build_port.sh`.

### System dependencies (baked into the VM image)
`build-essential`, `binutils-mips-linux-gnu`, `cpp-mips-linux-gnu`,
`ninja-build`, `libsdl2-dev`, `libopenal-dev`, `libgl1-mesa-dev`,
`libglu1-mesa-dev`, `gdb`. `cc`/`c++` are pointed at `gcc-13`/`g++-13`
(via `update-alternatives`): the port's CMake step uses the default `c++`, and
this VM's default clang targets a gcc-14 dir that lacks `libstdc++.so`, so the
repo build scripts (which assume gcc) fail under clang.

### Python toolchain (`.venv`, refreshed by the update script)
`requirements.txt` (splat64/spimdisasm/rabbitizer/ninja/pyyaml) is only needed
for the **matching** build. Run matching-build commands with `.venv` active
(`. .venv/bin/activate`). The **PC port** build only uses the Python stdlib.

### PsyCross HAL (vendored, gitignored)
The PC port links the PSX hardware-abstraction layer from
`pc_port/extern/PsyCross` (gitignored, baked into the image). If missing:
`git clone --depth 1 https://github.com/OpenDriver2/PsyCross.git pc_port/extern/PsyCross`.
`build_port.sh` applies idempotent in-place patches to this vendored tree.

### Building / running the PC port
- Build: `./pc_port/build_port.sh` → `pc_port/build_native/xeno-port`.
- Run headless: the VM has a VNC display at `DISPLAY=:1`; OpenGL runs on
  software Mesa (`llvmpipe`), which is enough to render.
- Useful env vars (see `pc_port/src/port_main.c`, `game_overrides.c`):
  `XENO_BOOT_DELAY=<frames>` holds each boot screen (default 60 ≈ 1s at 60fps;
  set high to keep the title/menu window up); `XENO_DISC=<path>` points at the
  disc image; `XENO_FIELD_TEST=1` boots the debug KernelMenu instead.
- Disc-less run behavior: the app boots the PSX HAL, shows the splash, then the
  decompiled title/intro/new-game screens; selecting **New Game** enters the
  field, which reads the field overlay from disc and therefore crashes without
  `disc/disc1.bin`. This is expected — the port is a boot-path "oracle" that
  runs real decompiled code until it hits a not-yet-decompiled stub or missing
  data (see `pc_port/README.md`).

### Project working rules
`.cursor/rules/xenogears-decomp.mdc` defines strict decomp-engineering rules
(evidence-cited changes, bounded scope, labeled diagnostics). Task handoff log:
`docs/ai_context/ACTIVE_HANDOFF.md`; append task summaries to
`scratchpad/grind_log.md`.
