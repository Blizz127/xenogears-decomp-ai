# Task: make the Xenogears `battle` overlay reproduce retail byte-for-byte

You are continuing an autonomous matching-decompilation pass in
`/var/home/blizz/Projects/xenogears-decomp-ai`. Read
`docs/ai_context/ACTIVE_HANDOFF.md` (newest entries are at the END; the last two
sections are the ones that matter) and `docs/ai_context/BATTLE_TARGET_BOUNDS_CONTRACT.md`
first, then begin. Do not trust the last chat turn; verify the tree.

## 0. Mission
`make build` must link `battle.elf` and produce `build/out/battle.bin` with
sha256 `1830b4ef1fe37129972fc310dfad534f8161d6c0b123e74254c3711334a3e291`
(the retail overlay, `disc/battle.bin`, 343936 bytes). Today it does NOT:
0 of 857 named battle functions are at their retail address. This is a real
matching problem, not a tooling nuisance. Fix the root causes in order.

## 1. Repo & state (verify before acting)
- Branch `experiment/worldmap-open-gates-20260823`, HEAD `f67fe692`, ahead 1.
- The tree is **dirty and uncommitted on purpose**. **No commits, pushes, resets,
  cleans, or overwrites of unrelated work.** `src/battle/` and `config/battle.yaml`
  are untracked; leave them that way.
- `disc/*.bin` (and `disc/battle.bin` sha `1830b4ef…`) are the retail authority —
  never modify or re-pin them. Never re-pin `config/checksum.sha`.
- `asm/` and `linker/` are splat/gears outputs, regenerated every build; do not
  hand-edit them as the fix.
- The tracked generator `tools/scripts/gen_battle_tus.py` is STALE: run in a
  throwaway mirror it rewrites 24 hand-edited `src/battle/main*.c` files
  (`mainc114.c` port `#ifdef`s, `main14`…`main44`, `main72/73/125`). Do not run it
  against the live tree.

## 2. Build / run environment (this is the only faithful one)
Matching build (splat 0.33.2 / spimdisasm 1.33.0 live in the image's `/.venv`;
`gears` calls `python3 -m splat`, so the venv must be active; `tools/gears`
locates the project by a path component named `xenogears-decomp`, hence the mount
path):

```bash
podman run --rm --security-opt label=disable --userns=keep-id \
  -v /var/home/blizz/Projects/xenogears-decomp-ai:/home/blizz/Projects/xenogears-decomp \
  -w /home/blizz/Projects/xenogears-decomp \
  localhost/xenogears-dev-toolchain:current \
  bash -lc '. /.venv/bin/activate && export TMPDIR=/home/blizz/Projects/xenogears-decomp/.xeno-tmp \
            && mkdir -p "$TMPDIR" && make build'
```
- Keep `TMPDIR` on the mounted repo: the host `/tmp` is a small tmpfs and, under
  quota exhaustion, **cc1 silently emits objects with NO function bodies while
  exiting 0**. This has caused false conclusions before.
- `make build` is from-clean: it runs `gears clean`, `gears matching` (splat
  regenerates all of `asm/` + `linker/`), then `ninja`.
- The port is separate and uses image `localhost/xenogears-dev-toolchain:krom-20260913`
  (`:current` lacks OpenSSL dev). The port does not compile/link battle TUs
  (`-DSKIP_ASM`, `REFERENCE_ONLY_GAME_TUS`); it is not the subject here.
- Known-red overlays: slus and field (and historically shop). Only `battle` is in
  scope.

## 3. Already done this pass — do not undo
`func_800BC460` was reconstructed and **installed** (TU-verified, not end-to-end):
- new `src/battle/mainc115.c` (path matches `battle/mainc` → `BattleCdk` preset,
  `gcc-2.7.2-cdk-psx` + `--dont-expand-li`). The body is 401/401 words, size
  `0x644`, byte-identical to `disc/battle.bin[0x4C970:0x4CFB4]`;
  `build/src/battle/mainc115.c.o` linked at the pinned retail symbol addresses has
  sha256 `4549ebf7a618d484078505b6dd7d5d925e37c579ff7b529778bce077a6814996`.
  The configured `gcc-2.7.2-psx` emits only 11/401 for the same source.
- `src/battle/mainl115.c` lost its `INCLUDE_ASM`; `config/battle.yaml` maps
  `[0x4C970, c, mainc115]` + `[0x4CFB4, c, mainl115]`; `pc_port/build_port.sh`
  lists `mainc115.c` as reference-only.
Keep all of this. It is byte-exact and adds zero link errors.

## 4. The problem, with evidence (reproduce it yourself)
`make build` fails at `build/out/battle.elf`:
`build/asm/battle/data/0.rodata.s.o` has `.word .L800xxxxx` references into
retained retail jump tables. In still-asm objects `jlabel` emits `.global .L…`
(`include/macro.inc`), so cross-object refs resolve; landed C bodies define none.

- **80 unique unresolved `.L800xxxxx` labels**, owned by only **four landed C
  TUs**: `main14` (48), `main38` (13), `main35` (12), `main36` (7).
- **A label-only fix is a FALSE GREEN.** Appending `.L800xxxxx = 0x800xxxxx;`
  (the address is encoded in the label name) for all 80 to a linker script makes
  the exact real link command succeed, but the result is
  `/var/tmp/battle-lab.bin`: 342908 bytes, sha256
  `25f8c7844313ec49368c3bf544775028de72ac67df2dc15e5bec04a988ee7105` — NOT
  `1830b4ef…`. Scratch evidence is already in `/var/tmp`:
  `/var/tmp/labdefs.ld`, `/var/tmp/battle-lab.elf`, `/var/tmp/battle-lab.bin`,
  `/var/tmp/build-install.log` (with install), `/var/tmp/build-baseline.log`
  (baseline, identical 89 errors). A baseline revert of the BC460 install
  reproduces the identical link failure, so none of this is from that install.
- **Localization**: `linker/battle.ld`'s `.text` object order equals the
  `config/battle.yaml` address order (checked 136/136), so the drift is per-TU
  **size/rodata**, not ordering. Per-symbol displacement from retail:
  `+0x130` for the earliest code (`func_80070E2C` lands at `0x80070F5C`), then
  `−0x234`, `−0x24C`, `−0x378`, `−0x458`, `−0x404`, `−0x898`, …; BC460 is `−0x378`.
- Retail code starts at `0x80070E2C` = base `0x8006FAF0` + `0x133C` of rodata.
  Built code starts at `0x80070F5C`, i.e. **+0x130 of extra rodata before text**.
  `main14`/`main35`/`main36` C objects have non-empty `.rodata` (their `switch`
  tables), while retail's tables are already retained in `asm/battle/data/` — a
  duplicate. The overlay is 1028 bytes short overall.
- Recorded "battle.bin `1830b4ef` PASS" gates in prior handoffs cannot be true for
  the built artifact; they pass trivially if someone hashed `disc/battle.bin`.
  Do not trust them; always hash the built `build/out/battle.bin`.

## 5. Task, in order (one increment at a time; test after each)
1. **Reproduce and pin the baseline.** Run the build above; save the log outside
   the repo. Confirm the 89 `undefined reference` lines and the 80 unique `.L`
   labels. Do not change source yet.
2. **Find the +0x130 rodata excess.** Compute the placed rodata size for battle
   (built text base − `0x8006FAF0`) and identify exactly which object(s) add the
   `0x130`. Compare the generated rodata (`asm/battle/data/0.rodata.s`,
   `asm/battle/133C.s.o`) plus each C TU `.rodata` against retail's `0x133C`.
   Beware: `mips-linux-gnu-size -A` prints **decimal** here (mainc115 `.text`
   prints `1604` = `0x644`), and `config/battle.yaml` offsets may span interleaved
   data — do not assume consecutive `c` offsets are pure text.
3. **Per-TU size audit.** For every battle TU, compare each built function's size
   (`mips-linux-gnu-nm -S build/src/battle/<tu>.c.o`) against its retail size (the
   function's span in `asm/battle/main<tu>.s` or the matchings/nonmatchings `.s`).
   Produce the list of functions where they differ. Those are the real drift
   sources (wrong preset / `li`-flavour assignment, or non-byte-exact C bodies).
   Preset assignment is per-path in `gears.toml` (`battle/mainl*` → 2.7.2-psx,
   `battle/mainc*` → CDK, both `--dont-expand-li`; `battle/main*` → Default). A
   wrong preset changes TU-wide `li` expansion and therefore size.
4. **Handle the four `switch` TUs properly.** `main14`/`main35`/`main36`/`main38`
   must both define their retail jump-table labels and not shift the layout with
   duplicate compiler-emitted `.rodata`. Establish how the project wants this
   represented (retail table retained as data vs. compiler table placed at the
   retail address) before hacking the link. Precedent: `field` has 0 `.word .L`
   refs in data; battle has 1206 — find out why and mirror the working case.
5. **Fix incrementally.** After each fix rerun the build and re-hash. Track the
   code-base offset (target `+0`) and the count of misplaced `func_########`
   symbols (target 0 of 857) as your progress metric.
6. **Do not stop at "it links".** The only acceptable end state is
   `build/out/battle.bin` == `1830b4ef…`.

## 6. Acceptance gates
- `mips-linux-gnu-ld … -T linker/battle.ld …` succeeds with **no** added label
  definitions or absolute-address substitutions.
- `build/out/battle.bin` sha256 == `1830b4ef1fe37129972fc310dfad534f8161d6c0b123e74254c3711334a3e291`
  and size 343936, verified by hashing the built file.
- `make rom-check` reports battle PASS; slus/field may stay known-red; do not
  change pins.
- BC460's `mainc115.o` stays byte-exact (`4549ebf7…`) and `mainl115.o` stays
  `80232ffa…` (`disc/battle.bin[0x4CFB4:0x4D00C]`).
- The port (image `:krom-20260913`) reaches its normal LINK OK; note it currently
  aborts pre-existing on `unclassified port source is not in PORT_SOURCES:
  pc_port/src/battle_target_bounds.c` — that is separate, do not "fix" it by
  deleting the file.

## 7. Rules & pitfalls
- Transcribe/verify from retail asm or a live trace that agrees with it. Unverified
  tails stay `INCLUDE_ASM`. Do not invent gap-fill.
- **Do not force the link**: no deleting relocations, no substituting addresses, no
  linker-script label definitions to make the undefined refs disappear. The Sep-13
  evidence doc (`docs/evidence/lahan-equip-oracle-layout-20260913/RESULT.md`) says
  the same. That path yields a wrong ROM.
- PSX-embedded pointers stay `u32`; do not dereference `0x1F000000`-style guest
  addresses on the host.
- Function-size convergence alone does not prove byte-exactness; and `LINK OK` is
  not fidelity.
- If a fix would touch production toolchain/generator/presets beyond what the
  evidence justifies, stop and report instead.

## 8. Deliverable
Update `docs/ai_context/ACTIVE_HANDOFF.md` (append, do not rewrite) with: the
change, the exact evidence (hashes, addresses, commands), remaining uncertainty,
and the next executable step. Update `docs/ai_context/BATTLE_TARGET_BOUNDS_CONTRACT.md`
only if the battle-target contract is affected. No commit, stage or push.
