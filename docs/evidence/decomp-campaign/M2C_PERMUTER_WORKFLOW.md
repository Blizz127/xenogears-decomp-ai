# m2c / permuter workflow

The helper scripts live in `tools/scripts/decomp_assist/`. They read only the
splat output (`asm/`) and the local build. Scratch files go to
`$XENO_ASSIST_CACHE` (default `~/.cache/xeno/decomp_assist`). Builds run on the
host unless `XENO_BUILD_WRAP` names a wrapper, for example a docker runner.
m2c (`XENO_M2C`, default `~/.cache/xeno/m2c/m2c.py`) and decomp-permuter are
external clones and are not vendored.

`make rom-check` is the authority. A function counts as matched only when the
overlay rebuilds byte-identically and rom-check stays 10/10. Every script below
is a way to get there faster, and none of them replaces rom-check.

## Scripts

| Script | What it does |
| --- | --- |
| `m2c_try.py OV SEG [names]` | Drafts each INCLUDE_ASM function with m2c (`-t mips-gcc-c --valid-syntax`), compiles it standalone with the repo's cc1 + maspsx, and compares it with the retail words (relocations masked). Writes `results.json`. |
| `m2c_insert.py OV SEG names` | Replaces those functions' `INCLUDE_ASM(...)` lines in `src/OV/SEG.c` with the cleaned drafts. It merges the drafts' externs and prototypes and reconciles conflicting extern types. |
| `m2c_drive.sh OV VRAM SEG...` | Runs the whole loop: m2c_try, insert every standalone match, `make build`. It skips functions that fail to compile, or that `elf_cmp.py` shows mismatching inside their TU, and repeats until `build/out/OV.bin` equals `disc/OV.bin`. |
| `elf_cmp.py OV VRAM` | Masked per-function compare of the linked overlay against retail. |
| `try_func.py OV NAME [--keep]` | Flips one `#ifndef XENO_PC_PORT INCLUDE_ASM / #else <C>` function to C, rebuilds only its object, and shows an aligned diff. It reverts on a mismatch. |
| `perm_setup.py OV SEG NAME [BASE.c]` | Prepares a decomp-permuter directory (base.c, target.o, compile.sh). Run it as `nice -n 15 permuter.py -j1 --best-only --stop-on-zero DIR`, one at a time and never alongside a rom-check. |

A standalone match is not proof. The same draft can compile differently inside
its TU when an earlier declaration there gives a global or callee another type,
so m2c_drive always rebuilds and compares the whole overlay.

## Lessons that turned near-misses into matches

- **Void callees.** m2c guesses `int` for an unknown callee. If the call's result
  is never used, that guess keeps `v0` live across the call and changes register
  allocation. Declaring the callee `void` fixed 11 bcf1 handlers at once; the
  permuter found this on `func_battle_command_file1_801E6144`. m2c_try does it
  automatically (`AUTO_VOID=1`).
- **Array-typed externs.** gcc-2.7.2 keeps a global's address in a register
  (`lui/addiu`, then `lw/sw 0(reg)`), and schedules a store to it before later
  loads, only when the extern has array type. Declare `extern T X[];` and access
  `X[0]` (field script-VM setters, `D_800B2348`, `D_80076F3C`). For a struct
  global accessed by raw offset, use a typed alias:
  `extern s16 g_Scene_s16[] asm("g_Scene");`.
- **Reload after aliasing stores.** Retail re-reads a global pointer such as
  `g_FieldScriptVMCurActor` after any store that could alias it, and indexes
  `scripts[curScriptIndex]` inline. Caching either in a local changes the code.
- **Unused leading parameters.** m2c drops them, but retail still reads its
  operands from `a1`/`a2`. m2c_try restores them.
- **Signedness and casts.** An explicit `(u16)` on a value passed to an
  unprototyped callee, a `u32` comparison where retail uses `slt`, or `lh` vs
  `lhu` is often the whole difference.
- **INCLUDE_ASM order.** INCLUDE_ASM must keep source order with the C bodies.
  The maspsx dummy-function wrapper in `include/include_asm.h` does this. A bare
  top-level `__asm__` is emitted ahead of every C function at -O2.
- **objdiff is not byte-exact.** It scored field's `func_800A7948` 100% while two
  stores went to swapped `symbol + offset` targets.
