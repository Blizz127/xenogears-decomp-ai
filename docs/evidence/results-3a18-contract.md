# RESULTS caller 0x801E3A18 — guest-frame and call-sequence contract (oracle only)

Coordinator message 8651e42ce68125bd: one bounded source-only proof step
toward a nonempty-party battle-results slice. Started 2026-10-04 00:38:43Z.
This is not adoption, not a production body, and not runtime evidence.

## Boundary

HANDOVER and native-goal-85 name 0x801E3A18 as the next RESULTS caller. Its
six helpers are native and differential-tested, but it needs proof of a real
guest stack frame and the helpers' call order before any native caller exists.
This step supplies that proof as an oracle.

## Contract, from the retail bytes (module SHA f474fd48…)

- Frame: addiu sp,-32. s0→sp+16, s1→sp+20, s2→sp+24, ra→sp+28. The 16-byte
  argument area sp+0..15 is never written. All four words are restored and
  the frame popped at return.
- Loop over slots s1 = 0..2 with slot = word 0x801E44C8 + 0x170·s0. Each
  iteration stores the slot into word 0x801E44EC before the dead check
  (u16 +0x7C & 0x8000 skips the slot).
- The kind byte (+0x56) is re-read through 0x801E44EC before each gate,
  so a helper that changes the kind or 0x801E44EC changes the later gates.
- Gates: 3BE0 if kind ∉ {7,8}, with the nonzero low byte of v0 stored at
  base+0x101C+s0. 3D54 if kind ≠ 10, with the nonzero low byte stored at
  base+0x101F+s1. 3E14 if kind < 7 ∨ kind = 9 ∨ kind ≥ 11. 3F28 if
  kind < 8 ∨ kind ≥ 11. 3EA4 if kind = 8, and 3FB0 if kind = 7; both get
  whatever a0 is live (neither reads it). 3BE0/3D54/3E14/3F28 get a0 = kind.
  0x801E44C8 is re-read for each store.
- Exit registers: at, v0, v1, a0 and every helper temp flow through from the
  last executed path. A native body must reproduce them, as the adopted
  leaves do via BattleResults_SetExitReg.

## Proof

pc_port/tests/run_results_3a18_contract_test.sh extracts the module outside
the tree. The real bytes run in the production interpreter with scripted
helpers, which log (target, a0), clobber 17 caller-saved registers and
randomly mutate the kind, 0x801E44EC or 0x801E44C8. A register-exact C model
calls the same scripted helpers. 20,000 seeded cases (kinds 0..12 plus random
bytes, 25% dead slots) require identical call logs, all 32 GPRs, all 2 MB of
RAM and the explicit frame words.

Result (docker xenogears-dev:clang, gcc 14): PASS at O0/O2/UBSan.
Coverage: 3BE0=48671, 3D54=50882, 3E14=42713, 3F28=41488, 3EA4=5290,
3FB0=4098 calls; mutations kind=38409, slot=19284, base=19277; dead slots
7594. Mutants killed: caching the kind (MUTANT_CACHE_KIND) and omitting the
frame (MUTANT_NO_FRAME).

## Limits / next

The helpers here are scripted, not the real or native bodies. Composing the
model with the six adopted native helpers, which have real exit-register
sets, is the next step. It needs the results differential harness, and that
needs a port build (stubs.c), which is coordinator-sequenced. The model is
not registered for dispatch. No nonempty-party fixture exists yet, and this
oracle does not create one.

## Composition with the adopted native helpers (staged, NOT RUN)

`battle_results_host_differential_test.c --compose-3a18`, called from
run_battle_results_host_differential_test.sh for each of O0/O2/UBSan.
Retail side: 0x801E3A18 and its helpers fully interpreted (force_interpret).
Composed side: the same register-exact model, run with zero interpreted
instructions. It writes the guest frame through the production mapping and
calls the six adopted native helpers through production `results_try_host`,
which applies their exit-register sets.

4,000 cases use the existing helper-world shaper, three kinds per case
(0..12 or the leaf-test kind alphabet) and about 25% dead slots. Exit
registers, HI/LO, all RAM (frame included, no FRAME_SKIP), the native
g_GameState copy, the scratchpad and uncompared writes must all match. All
six helpers must be reached. Mutant `XENO_COMPOSE_3A18_MUTANT=drop-exit-regs`
must fail on exit registers.

Compile-only check: clang -Wall -Wextra -Werror at O0/O2/UBSan compiles
(stand-in RESULTS_GS_BYTES). The run needs pc_port/build_native/stubs.c from a
port build, so it is queued in the coordinator-sequenced gate request for
e52f8f1e1137. Still not dispatched: 0x801E3A18 stays off the allowlist.
