# Field-12 scripted walk stall — resolved 2026-09-19

Baseline: `experiment/worldmap-open-gates-20260823`, HEAD
`f1830448959de0bde3aaf3b524d06553896a5bd6`. Production repair is two stores in
`src/field/main/misc4.c`, both under `#ifdef XENO_PC_PORT`. The five functions
protected by the task brief were not edited.

## Diagnosis on the stalled process

Started field 14/scenario 7 and used normal movement/dialogue input through
14 → 13 → 1 → 11 → 12. No teleport, position write, forced opcode completion,
or gate override was used. The controlled reproduction ran on Xvfb display
`:93` because two earlier attempts on the shared desktop received unexplained
inputs. Before-fix process 575183 stalled at approximately (29,135).

The requested first measurement on actor 1 was:

| Value | Observed |
|---|---|
| actorData | `0x74c8bc` |
| `*(s16*)(actorData+0x10)` | `0` |
| gate word / active gate | `0x04010400` / `0` |
| `D_800AFB24[stateIdx]` | `0x74a2d0` (non-null) |
| final material table | `0x74a954` |
| sub-index / material index / material value | `9` / `1` / `0` |

This rules out both proposed early-return explanations. Material zero also
occurs during ordinary movement before field 12. A breakpoint in the actual
movement path showed a nonzero sprite vector `{110592,0,-307200}` and collision
return `-1`; the brief's inference that movement never reaches collision was
incorrect.

The first collision probe had `sideMask=2`, `lastTri=11`, `triIndex=-1`, two
steps, and move `{286912,0,-501184}`. Its edge output was
`{33,0,133,1025,29,15,0,0}`. Retail vertices were `(33,0,133)` and `(29,15,133)`:
the second endpoint's z coordinate remained stale at zero.

Retail `func_8007BEF4` side-1 and side-2 branches jump from `8007C4C0` and
`8007C570` to a shared tail at `8007C628`. That tail reads b.z and stores it at
`8007C634` (`sh v0,0xC(s7)`). The port translation omitted that write in both
branches. The existing test repeated the same mistake by counting stores in
each branch without following the jump. Restoring `outEdge[6] = b[2]` fixes the
edge geometry used to slide past the doorway.

## Verification

- Regression fixture now checks all three single-edge cases plus six compound
  masks: 54 assertions per O0/O2/UBSan run. All pass. The old implementation
  fails with stale b.z `27499` versus expected `22`. Both the compound-mask
  mutant and shared-tail-removal mutant fail as intended.
- Native build: `LINK OK`, 82 generated function stubs; no skipped compilation.
- Fresh rebuilt process 643465 repeated the natural route. Actor 1's walk to
  `(70,25)` progressed through d=67,63,58,53,48,43,38,32,28,23,17,13 with movement.
  VM trace then advanced from IP141/opcode4A to IP147/opcode69 and later commands.
  Actor ended at approximately `(74,32)`, distance ≈8.06, below step11. The Alice
  scene advanced visually. `MOVE_DIAG` prints pending ticks only, so d<11 is
  established by final position plus opcode release, not a fabricated arrival
  log line.
- Non-port preprocessed misc4 is identical before/after. Its freshly compiled
  MIPS `.text` is also byte-identical before/after, SHA-256
  `14b24bbeb95e050d5d44ba09dc3a9fe3b7af289574b79fc52d026a8183ba1789`.
- Native executable SHA-256:
  `3ab30d4c17b4d6af196656d5df29063b03025aab85025f7633e9b4829c2b1497`.
- Both owned game processes and the owned Xvfb were stopped after capture.

## Correction to the handoff's byte-match claim

Fresh matching-build compilation with each TU's actual Ninja flags gives:

| Function | Retail bytes | Compiled bytes | Result |
|---|---:|---:|---|
| `func_80080968` | 104 | 120 | Pre-existing mismatch |
| `func_80082620` | 1432 | 1172 | Pre-existing mismatch |
| `func_80099AC0` | 1080 | 1012 | Pre-existing mismatch |
| `func_80084158` | 2004 | 1852 | Pre-existing mismatch |
| `func_8009E10C` | 148 | 148 | Byte-identical after resolving three relocations |
| `func_8007BEF4` | 1916 | 1504 | Pre-existing mismatch; this repair does not change matching bytes |

The first four functions were therefore not byte-matched at this HEAD, despite
the brief's statement. Their logic was preserved as requested. The independently
resolved `8009E10C` bytes have SHA-256
`feeab0a24015b6ebdb7e0c8bdde1ebac8c02afc96877b45e7b3067a839ffa45a` on both sides.
See `verify_9e10c.py` and the [whole-source audit](../current-port-mips-audit-20260919/README.md).

## Local raw evidence

`scratchpad/astra-field12-20260919/` retains before/after logs, GDB measurements,
`stalled.png`, `arrived.png`, test/build logs, compiled MIPS intermediates,
before/after text sections, and the before-fix executable. These are local
captures and are not published/licensed payloads in this report. Key files:

- `astra-field12-measurement.log`, `astra-walk-break.log`, `astra-collision.log`,
  `astra-collision-exit.log`, `astra-edge-vertices.log`.
- `astra-field12-after.log`: actor1 IP141 → IP147 at lines31781–31794.
- `astra-arrived.log`, `astra-edge-red.log`, `astra-edge-final.log`,
  `astra-field12-build.log`, `compile-mips.sh`, `compile-before.sh`.

Test command in the native build container:
`bash pc_port/tests/run_field_walkmesh_compound_edge_test.sh`.
