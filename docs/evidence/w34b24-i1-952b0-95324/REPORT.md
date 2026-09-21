# W34B24-I1 — World Helpers 0x800952B0 + 0x80095324 Implementation Report

## Canonical verification

| Field | Value |
|---|---|
| repo | `/home/blizz/dev/xenogears-decomp` (origin `Blizz127/xenogears-decomp-ai`) |
| canonical_branch | `ai-private-main` |
| canonical_head | `7fbce38b851c623ba14ec9b7780d0d1625dcfe70` |
| origin_canonical_head | `7fbce38b851c623ba14ec9b7780d0d1625dcfe70` (fetched, identical) |
| git_status | no tracked changes at lane start |
| candidate branch | `candidate/w34b24-i1-952b0-95324`, based on canonical `7fbce38` |

Note: W34B23 (`candidate/w34b23-951a8-chain` @ `ec779a3`) is still awaiting
independent acceptance; this pair has no code dependency on it, so the lane
is based directly on canonical.

## Identity (independently recovered)

| Function | Boundary | Size | File offset | SHA-256 (slice) |
|---|---|---|---|---|
| `wm_800952B0` | `[0x800952B0, 0x80095324)` | 116 B / **29 insns** | 0x257C0 | `b621ae26b802510c27575f80f5f92243037dff7ef64fb164aea607103057cfac` |
| `wm_80095324` | `[0x80095324, 0x80095414)` | 240 B / **60 insns** | 0x25834 | `ff48f82c9b8ecf1041fb581fc55db7f1bfbc5bd0935891487e2de5606e1efa36` |

Counts match W34B24-PRE exactly (29 / 60). All words decode; both end at the
next function's prologue (`0x80095324` / `0x80095414`). Slice SHAs are
re-verified by the run script on every invocation.

Sources: `pc_port/src/world_map_helper_{952b0,95324}.{c,h}`
Test: `pc_port/tests/w34b24_i1_952b0_95324_prod_test.c` + `run_w34b24_i1_952b0_95324.sh`

## Retail contracts

### wm_800952B0 — reference-vector sign projector (leaf)

```
void wm_800952B0(u32 mov_vec /*$a0*/, u32 out_vec /*$a1*/, u32 ref_vec /*$a2*/)
  dot = lo32(ref.X * mov.X) + lo32(ref.Z * mov.Z)   (mult/mflo, 32-bit wrap)
  dot < 0 : out = (-ref.X, 0, -ref.Z)   (negu wrap; store order +0, +8, +4)
  dot > 0 : out = ( ref.X, 0,  ref.Z)   (store order +0, +8, +4)
  dot == 0: out = (0, 0, 0)             (store order +8, +0, +4)
  out.Y = 0 written on EVERY path (jr delay slot).
  Sign paths RELOAD ref+8 after storing out+0 (faithful ordering).
  No frame, no saved regs, no callees, no scratchpad, no COP2.
  Retail $v0 residue is incidental (path-dependent dead value); the only
  retail caller (0x80095414 redirect tails) never reads it -> void in C.
```

### wm_80095324 — wall-tangent projector

```
void wm_80095324(u32 normal_vec /*$a0*/, u32 mov_vec /*$a1*/, u32 out_vec /*$a2*/)
  1. Down vector (0, -0x1000, 0) written to scratch 0x1F800010
     (retail store order +0x18, +0x10, +0x14).
  2. OuterProduct12(normal_vec, 0x1F800010, 0x1F800020).
  3. VectorNormal(0x1F800020, 0x1F800000)  (return ignored).
  4. Same ±projection as 952B0 with tan = unit tangent at 0x1F800000:
     dot = lo32(tan.X * mov.X) + lo32(tan.Z * mov.Z); sign paths reload
     0x1F800008; zero path stores +8 then +0; out.Y = 0 always.
  Frame 0x20, saves $s0/$s1/$ra.  Scratchpad 0x1F800000/0x10/0x20 via the
  accepted PSX_ADDR model (same as wm_80085760 — no private hack).
  GTE via canonical libgte residents only (no COP2 in the body).
  $v0 residue incidental; sole caller (95414 case-1 post-scan) ignores -> void.
```

## Dependencies

| Callee | Site | Status |
|---|---|---|
| (952B0) none | — | leaf |
| OuterProduct12 (`0x8004A480`) | 0x80095364 | CANONICAL (PsyCross libgte) |
| VectorNormal (`0x80048D7C`) | 0x80095374 | CANONICAL (PsyCross libgte) |

No missing/noncanonical dependency appeared; no STOP condition.

## Certification

- Harness `run_w34b24_i1_952b0_95324.sh`: retail full-file + two slice SHA
  gates; focused oracle at `-O0`, `-O2`, `-O2 -fsanitize=undefined
  -fno-sanitize-recover=all`; normalized stdout byte-identical; empty stderr.
- Independent oracle: expectations hand-derived from the disassembly; the
  reference dot is an s64 formulation truncated to the low word. Coverage:
  dot>0 / dot<0 / exact zero / **low-word-wrap zero** (65536×65536) /
  **wrap sign flip** (0x10000×0x18000 → low word 0x80000000) / negu at
  INT_MIN / store-order audits per path / out.Y-always-zero canaries /
  (95324) GTE call order, exact argument identity (host-pointer equality
  against PSX_ADDR), down-vector contents at call time, unit-vs-raw
  tangent discrimination, tan.Z-from-+8 discrimination.
- The two libgte residents are provided by the certificate as recording,
  test-controlled implementations (same accepted pattern as the
  w34b21_c1b_85760 certificate); production links PsyCross.
- Mutants: **14/14 KILLED** (6 for 952B0, 8 for 95324), each by a distinct
  semantic assertion — see MUTANTS.csv.
- One test-harness fix during bring-up: the mutant-discrimination seam
  multiplied INT_MIN by 3 in signed arithmetic (UBSan-caught in the test
  file, not in production code); rewritten as u32 wrap.

## Noninterference

All 16 accepted suites in the tree PASS:
w34b18c_800712d0, w34b21_c1b_85760, w34b21_c2b_74794, w34b21_c4b_894c8,
w34b21_c5b_93534, w34b21_c6b_7528c, w34b22_a2_94060, w34b22_i1_93354,
w34b22_i2_93e8c, w34b22_i4a_93f18, w34b22_i4b_private_family,
w34b22_i5_94a5c, w34b23_i1_8c040, w34b24_i1 (this rung), w34b_r4world_73b04,
w34cb1_90a84.

`w34b_r4world_73b04` initially failed for an environmental reason only: it
defaults `PSYCROSS_LIB` to `pc_port/build_native/libpsycross.a`, which this
machine's build keeps in the PsyX CMake dir; with the suite's own documented
`W34B_73B04_PSYCROSS_LIB` override it passes fully. Not caused by this
change (the suite links none of this rung's files).

Hardened `w18b_natural.gdb`: `W18I_FORBIDDEN_VERDICT verdict=PASS exit=0
zero_verified=13 hit=5 not_instrumented=0 instrumentation_error=0`.

## Link / symbols

- Registered both sources in `PORT_SOURCES` (`build_port.sh`).
- Clean build: `LINK OK`; incremental rebuild: `LINK OK` (×2).
- `nm`: exactly one strong `T wm_800952B0`, one strong `T wm_80095324`.
- No stub shadow (neither symbol appears in generated `stubs.c`).
- `game_tus=47 compiled, 0 skipped`; `function_stubs_before=241`,
  `function_stubs_after=241` (unchanged — the pair was previously absent,
  not stubbed); `data_stubs=524`.

## Runtime payoff (measured, not inferred)

Frontier measurement (natural Lahan route, full deep-gate chain,
DISPLAY=:10) on the integrated candidate tree:

```
MEASURE_STATE3 frame=916 / scheduler entry x2 / 17 callbacks executed
FRONTIER 0x8008a72c = MISSING CALLBACK EXECUTION FRONTIER (slot=1 state=1)
```

`runtime_frontier_before = wm_8008A72C`
`runtime_frontier_after  = wm_8008A72C` (unchanged — expected: this pair
sits below the unimplemented `wm_80095414`, whose only live-path caller is
the frontier callback itself). Neither helper was force-called.

## Remaining 95414 ladder (from W34B24-PRE, updated)

Done: ~~952B0~~, ~~95324~~. Remaining: `wm_80085158` (176),
`wm_80085418` (210), `wm_80084DB8` (232, needs a `func_8004A70C` fidelity
decision) then `wm_80084D00` (46), then `wm_80095414` (560).
