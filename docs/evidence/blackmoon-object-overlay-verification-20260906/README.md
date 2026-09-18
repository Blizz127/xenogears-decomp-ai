# Blackmoon object-overlay verification — 2026-09-06

`func_800A1364`, the field object-register script opcode that OPEN_ISSUES.md
item 8 names as MAP16/MAP3's shared root, now has a C body that is
**byte-exact against retail** and is compiled by both the matching build and
the port. The slus/field hash drift flagged by the preceding checkpoint is
**proven to be a splat/spimdisasm version change**, reproduced both ways.
The native port links in the project's podman toolchain image with the new
body. Runtime was observed by the coordinator on the preceding binary
(`docs/evidence/blackmoon-map16-runtime-20260906/README.md`), not repeated
here.

Evidence classes: **proven** (byte/instruction comparison or reproduced
hash), **build-verified** (compiles/links, not executed), **observed** (seen
at runtime), **inferred**.

## 1. `func_800A1364` is byte-exact (proven)

| item | value |
| --- | --- |
| retail range | `800A1364..800A14F0`, `disc/field.bin[0x31874:0x31A00]`, 396 bytes / 99 instructions, frame 0x30 |
| retail function SHA-256 | `abf8efc25897d54dc9dd9b7fa2d38ed2e464836d332eb3cbb9b506e598b7be98` |
| production TU function, relinked at retail addresses, SHA-256 | `abf8efc25897d54dc9dd9b7fa2d38ed2e464836d332eb3cbb9b506e598b7be98` (byte-identical) |
| object-level comparison | opcodes + relocations identical to the INCLUDE_ASM control, with and without `-DXENO_FIELD_OBJECT_OVERLAY` |
| source | `src/field/main/misc6.c`, SHA-256 `28e897adc81514378e8f481d2a948f9926d2814db188e1e5ee1cb959ee81f54d` |

Pipeline: the actual retail preset (`mips-linux-gnu-cpp` wrapper → `tools/gcc-2.7.2-psx/cc1 -O2 -G8 …` → `maspsx --run-assembler`), `objdump -dr` of the production object, and a relink with the ten real dependencies pinned to their retail addresses (`D_800AFD1C`, `g_FieldActors=0x800AFB10`, `g_FieldSpriteData=0x800AFB1C`, `g_FieldScriptVMCurActor=0x800B0078`, `D_800B2264`, `D_800B21DC`, `D_800B225F`, `FieldScriptVMGetArgument=0x800ACDEC`, `func_80076AC0`, `func_800A0C94`) and 84 unrelated externs at a placeholder; `objcopy -O binary -j .text` then slices the function. Details and hashes: `exact-function.json`.

### What was wrong before

The previous body sat behind `#ifdef XENO_FIELD_OBJECT_OVERLAY` with an
"asm-verified" comment. The matching build never defined that flag (it lived
only in `pc_port/build_port.sh` and pc_port test scripts), so the C had never
been compiled by the retail preset; through it, the body emitted **89
instructions / 356 bytes / frame 0x28** against retail's 99 / 396 / 0x30. No
behavioural divergence was found (the `slot` local held the same value
retail re-reads), but it was not retail.

### Source-shape facts that are load-bearing (proven by the 30-variant search)

1. **The two object-slot stores use pointer arithmetic on the array symbols**
   (`*(u16*)((u8*)D_800B21DC + D_800B2264 * 2)`), not `D_800B21DC[i]`. GCC
   2.7.2 treats an indexed store to a global array as unable to touch other
   scalar globals, so `D_800B21DC[i]`/`D_800B225F[i]` left `D_800B2264` and
   `g_FieldScriptVMCurActor` cached (v8: one load each); the pointer form
   makes it discard both, which is what retail's reloads after each store
   are (v11).
2. **The slot stamp reads the counter through a volatile lvalue**
   (`*(volatile s32*)&D_800B2264 & 7`). The `+0x12C` store between that read
   and `D_800B2264++` cannot alias the counter under any form tried
   (pointer local, global pointer, integer base: v11/v19/v35/v39 all merged
   the two reads); retail has two loads. This is the only construct that
   reproduced it (v24/v36).
3. **The IP and flag writes go through the global pointer and the actor mask
   through `g_FieldActors[D_800AFD1C]`**; caching either in a local reorders
   retail's loads (v11/v17 interleaved the IP and flag accesses; v34 fixed
   both), and the direct-indexed form gives retail's `addu v0,v0,v1`
   operand order for the second actor recompute where the pointer-local
   form gives `addu v1,v1,v0`.
4. **`s32 pad[2]` is never referenced.** Retail's frame is 8 bytes larger
   than the 7-argument `func_80076AC0` call plus `ra`/`s0` need; only an
   unused non-register local reproduces it (v2/v8).
5. `(u32)spriteId << 1` — the cast is byte-neutral (verified) and removes
   the left-shift-of-negative UB from the port path.

Search scratch: `/var/tmp/xeno-blackmoon-verify-mT7beL/a1364/` (`try.sh`,
`batch.sh`, `v*.body`, `*.dis`, `*.diff`).

### Whole-field consequence (proven)

With INCLUDE_ASM the function was emitted near the head of misc6's `.text`
(`0x8009a850`); as C it lands at its retail-relative position (`0x8009dc70`).
`field.bin` therefore re-hashes at the same size:

| generator | before (INCLUDE_ASM) | after (C) |
| --- | --- | --- |
| host splat 0.41.1 / spimdisasm 1.42.2 | `ececa463…` | `f49b4fd02316ddd6a1bb568440ff7dff99082d5a13c04947c267ada0bbf2d854` |

A per-function comparison of all **1054** field functions with relocation
fields masked (lui/addiu/ori/andi/loads/stores low 16 bits, j/jal low 26
bits) shows **0** functions whose words differ and 60 moved symbols: the
function itself +13,344 and the 59 functions between −396. slus, member,
shop and menu are unchanged (`matching-after.json`). The gate is still red on
slus/field/shop for the reasons already recorded; member is exact.
`asm/field/nonmatchings/main/misc6/func_800A1364.s` is no longer generated
(the whole `asm/` tree is gitignored); `func_800A06E8` is misc6's last
INCLUDE_ASM.

## 2. Differential regression test (proven)

`bash pc_port/tests/run_field_object_register_retail_test.sh`
(`pc_port/tests/field_object_register_retail_test.c`), modelled on the
field-axis-matrix test: the raw 396 retail bytes run on the project's MIPS
interpreter against the production misc6 TU, with
`FieldScriptVMGetArgument`, `func_80076AC0` and `func_800A0C94` as explicit
spies on both sides (the last is in the same TU, so the runner compiles the
TU with `-fno-inline-functions` and weakens the symbol; a six-line forward
declaration shim keeps clang from rejecting the TU's implicit declarations).

- **O0 / O2 / UBSan: 1,440 cases, 57,702 checks and all 99 instruction slots
  each** (`regression-pins.json`, output `/var/tmp/xeno-blackmoon-verify-mT7beL/regtest`).
- Case space: 4 actor indices × 5 object slots × 12 script ids (including
  −1 and 0x80000001) × 6 status words, cycling 5 IP / 5 scriptFlags /
  5 flags / 5 `+0x12C` / 5 sprite-offset values and two fill patterns.
- Compared: the actor slot and whole 0x138 ActorData with 16-byte redzones,
  `D_800B21DC[0..7]`, `D_800B225F[0..4]`, `D_800B2264`, `D_800AFD1C`, the
  sprite package, and the ordered spy trace with all seven `func_80076AC0`
  arguments; independent semantic expectations from the asm are asserted on
  both sides; retail's 0x30 frame is asserted at the call.
- **Seven controls rejected** (`controls.json`): status set, IP advance,
  actor clear, flags4, id scale, slot shift, count increment.
- Limits: slots 0..4 only (retail keeps five flag bytes ahead of the
  counter); the spies do not prove the script VM, the sprite binder or the
  pose reset. The `(s32)base + offset` sprite-package idiom (shared with the
  matched `func_800A0D3C`) overflows natively for offsets within ~150 MB of
  2^31, which real packages never approach; the test stays below that.

## 3. The slus/field hash drift is a generator version change (proven, reproduced)

| build | generator | slus_006.64 | field.bin |
| --- | --- | --- | --- |
| 02:37 shop-resources record (`/tmp/xeno-global-jtbl-audit-20260906`, its log banner) | splat 0.33.2 / spimdisasm 1.33.0 (`requirements.txt` pins) | `5c674b3f…` | `f72b2245…` |
| 10:55 shop-string-uv and this pass on the host | splat 0.41.1 / spimdisasm 1.42.2 | `3192a514…` | `ececa463…` |
| this pass, `localhost/xenogears-dev-toolchain:current` `/.venv`, same 12:15 sources | splat 0.33.2 / spimdisasm 1.33.0 | `5c674b3f…` | `f72b2245…` |

- No build input under `src include config linker asm gears.toml Makefile`
  changed between the two records except the shop TU (already shown inert
  for slus/field); the generated data assembly differs by generator
  (1.42.2 adds `nonmatching`/`enddlabel` directives).
- `objdump -d` diffs of the two ELF pairs are pure data-address shifts:
  slus −12 bytes, field −4; 1,829 / 4,322 isolated bytes, opcodes unchanged.
- Moved symbols (link maps): slus 378, first `g_CurGameStateOverlayID`
  `0x80058d10 → 0x80058d04` — `.main_bss` starts 16-aligned under 1.33.0
  and directly after `.data` under 1.42.2; retail `g_CurGameStateOverlayID
  = 0x800592C0` is buffer+4, so 1.42.2 is retail-consistent there. Field
  449, first `D_800AF5F0` `0x800aaeb0 → 0x800aaeac` — `.field_bss` starts
  at data end +4 under 1.33.0 and at data end under 1.42.2; retail
  `0x800AF5F0` is data end +4, so 1.33.0 is retail-consistent there.
- A first pinned-generator rebuild from the live tree gave field
  `6b7f53a8…`: the live tree carries another agent's in-progress
  misc2/misc5/misc9 edits made after 12:15; with the 12:15 `src` restored
  the record reproduces exactly.

Consequence: every recorded slus/field hash, and the retail checksum gate's
distance from green, depends on the installed splat/spimdisasm version.
`requirements.txt` pins the versions the container carries; the host has
newer ones. Future baselines must state the generator. `drift-generator-version.json`.

## 4. Native port (build-verified)

`./pc_port/build_port.sh` inside `localhost/xenogears-dev-toolchain:current`
(coordinator's invocation, `--userns=keep-id`, checkout mounted as
`…/xenogears-decomp`) with the final source: 50 game TUs, 640 undefined
references → regenerated `stubs.c` with 74 function stubs / 565 data
symbols, **LINK OK, port-owned addresses verified**;
`pc_port/build_native/xeno-port` SHA-256
`522e901c0789888bb8b7e11a931b47bd30e2d265dc59917a04065d64baca4e5e`
(4,805,016 bytes). The build also works on the host without the image:
`PKG_CONFIG_PATH=/home/linuxbrew/.linuxbrew/lib/pkgconfig
CMAKE_PREFIX_PATH=/home/linuxbrew/.linuxbrew bash pc_port/build_port.sh`
linked `e86ad188…` earlier in the pass. Preserved binaries and hashes:
`final-build-pins.json`.

## 5. Runtime (observed by the coordinator, not by this pass)

MAP16 was booted on the preceding binary (`591ce91e…`, built from the same
tree before this body) and rendered terrain, a tree, a rope bridge and the
player sprite with 67/109 actor model builds and zero `func_800A1364`
spins: `docs/evidence/blackmoon-map16-runtime-20260906/README.md`. The
binary above with the exact body has **not** been run. My own xvfb attempts
in this session failed only because the `/tmp` user quota was exhausted at
the time (Xvfb could not create its lock/socket, SDL reported "x11 not
available"), which the coordinator has since cleared.

## 6. Environment facts corrected or added

- The build container exists as a **podman image**, not a distrobox
  container; earlier handoffs (and this README's first draft) were wrong to
  say the port could not be built or run here.
- `/tmp` is a 16 GB tmpfs with a 12,610 MB **user quota**; a concurrent
  agent's build tree filled it, producing `EDQUOT` for every shell and Write
  in this session for a while. Scratch for this pass now lives in
  `/var/tmp/xeno-blackmoon-verify-mT7beL/` (moved from `/tmp`), and
  `TMPDIR=/var/home/blizz/.cache/xeno-tmp` keeps compiler temporaries off
  the tmpfs.
- `tools/gears` still needs a path component named `xenogears-decomp`; both
  the scratch copies and the container mount satisfy it.
- The host `cpp` accepts `-lang-c` only by misparsing it as `-l ang-c`; the
  wrapper that drops it reproduces the container's `mips-linux-gnu-cpp`
  results for every module hash checked.
- `pc_port/build_port.sh` still defines `-DXENO_FIELD_OBJECT_OVERLAY`; it no
  longer gates anything in `src/` and can be retired together with the
  pc_port test scripts that pass it (not done here, out of scope).

## 7. Open

- OPEN_ISSUES.md items 5/8 were not edited: the render observation is the
  coordinator's, and this pass did not run the binary that contains the
  exact body. With their MAP16 observation plus this byte-exact proof, item
  8's "loader stub" root cause is closed and the item can be deleted; MAP3's
  status needs its own run.
- `pc_port/src/field_object_overlay.c` (the 0x801E742C/738C/7D14/8030/8330
  owners) and `func_800821F4`'s object-animation branch remain
  build-verified only; none was audited against retail bytes.
- The remaining slus/field/shop checksum reds are unchanged in nature.

## Pins

- Working tree: branch `experiment/worldmap-open-gates-20260823`, HEAD
  `3a3e7aac`; nothing staged, committed or pushed. Files touched by this
  pass: `src/field/main/misc6.c` (body + comment), the two test files above,
  this directory, `scratchpad/grind_log.md` (append), `ACTIVE_HANDOFF.md`
  (top paragraph). The neutralised `.claude/launch.json` written while the
  shell was dead has been deleted.
- Scratch root: `/var/tmp/xeno-blackmoon-verify-mT7beL/` (`xenogears-decomp/`
  host-generator copy, `pinned/` container copy, `a1364/`, `drift/`,
  `regtest/`, `native-backup/`, `pc_port_build_container/`, logs).
