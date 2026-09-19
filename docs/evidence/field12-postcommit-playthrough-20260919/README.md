# Post-commit testing — 2026-09-19

The field-12 repair and compiled-MIPS audit were committed as `20d74007`.
This continuation runs broader regression tests and a fresh natural gameplay
route. No remote push is part of this work.

## Regression coverage

**64 suites PASS.** `tests.json` lists each suite, execution environment, initial
host outcome, final outcome, and local log path/hash. Coverage includes field
walkmesh and scripted movement, actor initialization, interaction, field/map/menu
transitions, field rendering/particles/gear, guest primitive links, battle pointer
and host bridges, translation/rotation/pose/object-tree transforms, model
relocation, selected world-map paths, and the new diagnostic-lifecycle check.
These are bounded tests with each runner's existing assertions and negative
controls; they do not establish whole-game parity.

Host toolchain: GCC 16.2.1 / Clang 22.1.8. The first attempt in the KROM build
container encountered missing Clang/ripgrep and an older GCC rejecting the
C-test `-fpermissive` flag under `-Werror`; those logs are preserved separately.
The host passed 58 of the original 63 suites. BIOS-font, party-gear, and three
world-map runners needed the KROM container's compatible compiler/UBSan runtime.
The latter three used the host's statically linked ripgrep mounted read-only.
All five then passed without source or assertion changes. The additional
logger test passed using Clang at O0/O2/UBSan and rejected its guard-removal mutant.

## New crash found by gameplay, and repair

The first post-commit run naturally reached field13/scenario7 after Dan's
conversation, then crashed near its exit. Core PID798114 shows:

```
PcPort_FieldPosDiag             pc_port/src/field_pos_diag.c:80
Vsync(mode=2)                  pc_port/src/psyq_compat.c
FieldDisplay                   src/field/main/misc5.c
func_800A5884(semiTrans=1,abr=1)
func_800A5C40
FieldMain                      src/field/main/main.c:593
```

The field-mode latch was still active, but `D_800ADB04 == 0` during transition;
`g_FieldActors[1].pActorData` contained stale `0xef115931`. The logger dereferenced
that address. `FieldMain` clears `D_800ADB04` before the reload/fade sequence and
restores it after completion, while the broader field-mode latch stays active.

`pc_port/src/field_pos_diag.c` now checks that existing availability flag before
reading any actor/header/zone data. This is port-only diagnostic code; no
matching-gameplay function or retail gate was changed. The logger is still inert
unless `XENO_FIELD_POS_DIAG` is set.

The new production-linked fixture supplies intentionally unreadable pointers
both while field mode is inactive and while field mode is active but transition
availability is zero. The original logger crashes; the repaired logger returns
without touching them at O0/O2/UBSan. Removing only the transition guard trips
the fixture's SIGSEGV handler and returns its dedicated failure code99. Native
rebuild passes `LINK OK`, 82 function stubs, and the 92-adopted-leaf stub gate.

Rebuilt executable SHA-256:
`b6bdce8c0423e46b9a7c4f943a5f7b292d1bd105e6772708c7f673d9a15e8666`.
The earlier audit hashes remain an intentionally dated snapshot of the
pre-diagnostic-fix tree; the matching source has not changed in this continuation.

## Runtime evidence

Local artifacts are under `scratchpad/astra-postcommit-20260919/`:
`playthrough.log` (crashing run), `core.798114`, `crash-gdb.log`,
`crash-lifecycle.log`, `playthrough-fixed.log` (fresh rebuilt run), test logs,
and native `build.log`. The core is local only and is not committed.

Both runs start normally at field14 through the established field-test launch;
all subsequent travel uses normal directional/confirm inputs. No teleport,
position write, forced scenario flag, or forced opcode completion is used.

The rebuilt run passed 14 → 13 → 1 → 11 → 12 with diagnostics enabled, including
the exact field13 transition that crashed earlier. In field12 the walk again
advanced from actor1 IP141/opcode4A to IP147/opcode69 (d=67 down to13 on pending
ticks). The **entire Alice wedding scene completed**, then the game returned to
field11 with **scenario8 and canRun=1**. Fei subsequently exited the house to
field1 through ordinary input. `alice-walk.png` and `alice-scene-complete.png`
in the local artifact directory show those later scene stages.

The run then crossed Lahan's exit into **field15**, scenario8, with normal player
control. `mountain-entry.png` captures this. A natural mountain encounter followed.
The adapter logged entry at retail `0x80070F40` and returned after **7,426,117
instructions**, then entered **field490 (the title field)** without a playable
fight. This is **not combat acceptance** and is the endpoint of this run.

Read-only inspection after that return found party `{0,255,255}`, Fei's
HP/maxHP/MP/maxMP all zero, and character2's corresponding values also zero.
`port_main.c` explicitly bypasses `func_8001BB50` new-game initialization under
`XENO_FIELD_TEST=1` and installs a roster/skin stand-in. Thus this launch is not a
valid initialized New Game combat test. Zero stats were measured **after** the
encounter, not before it; the precise battle-return branch has not been isolated.
Do not report this as a reproduced normal-New-Game battle defect or a battle pass.
No stats, inventory, flags, or battle results were forced to extend the route.
Next gameplay verification must start through normal New Game initialization.

Additional local evidence: `encounter-return-title.png`, `encounter-state.log`,
and the battle-entry/return lines in `playthrough-fixed.log`. Existing user
quicksaves were not changed. Owned game/Xvfb processes were stopped after capture.
