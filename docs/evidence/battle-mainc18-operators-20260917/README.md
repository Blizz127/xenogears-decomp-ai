# battle/mainc18 operator family — 5/6 byte-exact (2026-09-17)

Status: **PROVEN** (full-overlay byte equality against retail), build-verified only
(no runtime observation — the battle overlay is still unported on the PC side).

## Result

`src/battle/mainc18.c` (6 `INCLUDE_ASM`, no C bodies for them) is replaced by
`src/battle/mainc18_q1.c` + an asm segment + `src/battle/mainc18_q2.c`.
Five functions are now C:

| Function | Op | Status |
|---|---|---|
| `func_8007ABD8` | cell += op, saturate 0xFFFF | C, byte-exact |
| `func_8007AC30` | cell -= op, clamp 0 | C, byte-exact |
| `func_8007AC80` | cell *= op, saturate 0xFFFF | **still asm** (see below) |
| `func_8007ACDC` | cell /= op (quotient) | C, byte-exact |
| `func_8007AD24` | cell %= op (remainder) | C, byte-exact |
| `func_8007AD6C` | cell &= op | C, byte-exact |

Battle `INCLUDE_ASM(` occurrences in `src/battle/*.c`: 528 → 523. (Counting the
bare token instead gives 537 → 532, but that also matches prose in comments —
use the `INCLUDE_ASM(` form.)

## Proof

The battle overlay links independently of the (currently red) slus link, and
its baseline was already byte-identical to retail. That makes full-overlay
byte equality the verification bar — any codegen drift anywhere in the overlay
would change the hash.

```
build/out/battle.bin 1830b4ef1fe37129972fc310dfad534f8161d6c0b123e74254c3711334a3e291
disc/battle.bin      1830b4ef1fe37129972fc310dfad534f8161d6c0b123e74254c3711334a3e291
differing 4-byte words: 0 / 85984
```

Reproduce (podman image `localhost/xenogears-dev-toolchain:current`, splat
0.33.2 / spimdisasm 1.33.0 from `/.venv`):

```
podman run --rm --security-opt label=disable --userns=keep-id \
  -v /var/home/blizz/Projects/xenogears-decomp-ai:/home/blizz/Projects/xenogears-decomp \
  -w /home/blizz/Projects/xenogears-decomp -e TMPDIR=/var/tmp \
  localhost/xenogears-dev-toolchain:current \
  bash -lc 'source /.venv/bin/activate && ninja build/out/battle.bin \
            && cmp disc/battle.bin build/out/battle.bin && echo BYTE-EXACT'
```

Unchanged in the same run: `member_change_menu.bin` and `shop_menu.bin` still
equal retail; `field.bin` / `menu.bin` remain known-red; the slus link still
fails (pre-existing, see below).

## Load-bearing source shapes

1. **Operand order.** The operand is `(p[3] << 8) + p[2]`, not
   `p[2] + (p[3] << 8)`. This single flip took the batch from 60 differing
   words to 4: it also moves the `lbu p[2]` after the cell-address computation,
   matching retail's schedule. Note the *compound-assignment* members of the
   same family (`|=`, `^=`, `&=` — `func_8007ADB0`, `func_8007ADF4`, and the
   new `func_8007AD6C`) use the opposite order, `p[2] + (p[3] << 8)`, and are
   byte-exact that way. The family is not internally consistent.

2. **The clamp is written on an `s32` temp**, then stored to the `u16` cell —
   semantically redundant for `+`/`*` on a u16 cell, but it is what retail does
   and it is what produces the `ori $zero,0xFFFF` / `slt` / `beqz` shape.

3. **Locals order** `off, base, p, row, v` reproduces retail's allocation.

## `func_8007AC80` — why it is still asm

Every candidate reproduces all 23 retail instructions and all but **one word**.
The residual is the multiply's register allocation, and the two halves are
coupled:

- `cell * op` → retail's `mult $a0, $v0` but `mflo $a0` (retail: `mflo $v1`)
- `op * cell` → retail's `mflo $v1` but `mult $v0, $a0` (operands reversed)

~30 source shapes were swept (temp for the cell, temp for the operand, decl
order permutations, `register`/`long`/`u32`/`u16` typing, casts, ternary,
duplicated stores, `p[3] * 256`, explicit cell pointer). Best results: 1
differing word for the `op * cell` family, 4 for the `cell * op` family. A
decomp-permuter run is the natural next step.

Because file-scope `INCLUDE_ASM` is emitted ahead of every compiled body in
this toolchain, leaving `func_8007AC80` as asm inside a single TU would hoist
it in front of `func_8007ABD8`/`func_8007AC30` and shift the whole overlay
(measured: 51 differing words). The fix is the repo's existing TU-split
pattern (cf. `main17_q1`/`main17_q2`, `main125_q1`/`main125_q2`):

```yaml
- [0xB0E8, c, mainc18_q1]
# retail bytes: 8007AC80 (C body not yet matching)
- [0xB190, asm]
- [0xB1EC, c, mainc18_q2]
```

## Incidental: slus link

`make check` cannot complete here — `build/out/slus_006.64.elf` fails to link.
This is **pre-existing, not caused by this change**: a pristine HEAD worktree
(`f67fe692`) fails the same link step (there on `.sdata` discarded in
`system/system.c`). In the working tree the failure was two undefined
references from the decompiled `psyq/libetc/intr.c`, `D_800578D6` and
`D_800578D8`; `linker/` is generated and gitignored, so this commit adds them
to the `Makefile`'s existing generated-symbol patch block, the same way
`g_Heap` and `ApplyMatrixSV` are already handled. That removes those two
diagnostics; the remaining slus link failure is untouched and still open.

## Not claimed

No runtime observation. The PC port does not build the battle overlay at all
(`func_80281204` is still a stub in `pc_port/src/game_overrides.c`), so these
bodies have been proven byte-identical to retail but never executed.

---

# Addendum — main62/main63 timing family (same session)

Attempted, **not banked**: `func_8009E278`, `func_8009E2EC`, `func_8009E364`
(main62) and `func_8009E410`, `func_8009E48C` (main63). All five reproduce every
retail instruction; all five differ only in the destination register of the
`mflo`/`mfhi` (retail `$v1`, every candidate `$a1`) — 26 differing words total.
Both source files were restored to their original coexistence form and
`battle.bin` re-verified at `1830b4ef…`, 0 of 85984 words differing.

Note `src/battle/*` is entirely **untracked** in git, so there is no `git
checkout` safety net for these files; restore-by-hand is the only option if a
batch is abandoned.

Durable result — the semantics, which were previously either absent or only
partly recorded:

| body | opcode at +0x5FA0 | count at +0x5F6C[D_800C3E50] | signedness |
|---|---|---|---|
| `func_8009E278` | 2 | `D_800D2DC8[0x4F] * *(u32*)(D_800D2DC8+0x64) / 10` | unsigned (`multu`, `srl 3`) |
| `func_8009E2EC` | 2 | `D_800C3DFC[0x11] * *(u32*)(D_800D2DC8+0x64) / 20` | unsigned (`multu`, `srl 4`) |
| `func_8009E364` | 2 | `D_800C3E00[0x5B] * D_800C3DFC[0x11]` | plain product |
| `func_8009E410` | 0xA | `*(u16*)(D_800D2DC8+0x3A) * D_800C3DFC[0x11] / 20` | signed (`mult` 0x66666667, `sra 3`, `subu`) |
| `func_8009E48C` | 0xB | same as `func_8009E410` | signed |

`func_8009E410` had no C body at all; it now carries a coexistence body
(port-side `#ifdef XENO_PC_PORT`, matching build unchanged), byte-gated as
above and syntax-checked under `-DXENO_PC_PORT`.

Two dead ends, measured, so they need not be re-walked: computing the count
inline at the store grows the overlay by 16 bytes, and assigning the local
*after* the byte store grows it by 8 — both re-materialise the symbol
addresses. Register allocation precedes `sched2` in this cc1, so the pseudo's
live range at allocation time spans the intervening byte store and is forced
off `$v0`/`$v1`; the `mflo` only *looks* late because the scheduler moved it.

This is the same residual as `func_8007AC80` above — one open toolchain
question about the `mflo` destination, not six independent bugs. Permuter next.
