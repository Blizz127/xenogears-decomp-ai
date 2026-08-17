# W34B24-I6 — 8A72C Small-Dependency Pack (97770, 941C4, 94238, 8BEC8, 8C1DC)

## Lane

- START_CANONICAL: `3f8ed7ad5e0cb2fe723cdfc75f2912636e513cca` (fetched
  `7b378da`; canonical advanced by one docs-only evidence commit between
  fetch and worktree creation — delta verified `docs/`-only, so the lane
  proceeded on the newer base rather than stopping; the expected
  `7b378da` is contained in it).
- Branch: `candidate/w34b24-i6-a72c-smallpack` (worktree
  `/tmp/w34b24-i6-smallpack`). Candidate only; canonical untouched.
- These five are the remaining direct dependencies of the measured live
  callback frontier `wm_8008A72C` besides keystone `wm_80095414`.

## Identity (independently recovered; edges verified jr-ra/prologue)

| Function | Boundary | Insns | File off | SHA-256 (slice) |
|---|---|---|---|---|
| wm_80097770 | [0x80097770,0x800977A8) | 14 | 0x27C80 | b63a8eea…eca7e571 |
| wm_800941C4 | [0x800941C4,0x80094238) | 29 | 0x246D4 | 4fdffabe…a70e4ce8 |
| wm_80094238 | [0x80094238,0x80094364) | 75 | 0x24748 | 2f64495c…affcaf69 |
| wm_8008BEC8 | [0x8008BEC8,0x8008BFD4) | 67 | 0x1C3D8 | 2fbee93a…ace260da |
| wm_8008C1DC | [0x8008C1DC,0x8008C28C) | 44 | 0x1C6EC | 0ba6d1cf…c9438343 |

All slices re-verified by the run script on every invocation.

## Contracts

- **wm_80097770** `s32 (u32 slot_idx, s32 value)` — pool slot claim on the
  accepted convergence/scheduler pool `*(0x8009BE24)` (128-byte stride):
  busy when halfword +4 != 0 (return 0); else sh 1 → +0, sh value → +4,
  return 1. ~180 retail call sites across the world callbacks.
- **wm_800941C4** `s32 (u32 vec_a, u32 vec_b, u32 out_vec, u32 out_angle)`
  — heading = `(ratan2(B.Z−A.Z, B.X−A.X) + 0x400) & 0xFFF` (sh to
  *out_angle in rcos's delay slot); out.X = rcos(angle), stored BEFORE
  the lh reload that feeds rsin (aliasing-faithful);
  out.Z = negu(rsin(reload)). Returns the negated rsin residue.
- **wm_80094238** `s32 (u32 pos_vec, u32 list_index)` — rect-region
  trigger lookup: cell = (pos >> 12 srl) & 0xFFFF, signed inclusive
  bounds against 16-byte s16 rect records (+8 marker, −1 terminates;
  list ptr array at `*(0x8009BD00)`). Hit type==4: `*(0x8009D7D8)`=−1,
  sh −1→`0x8009BD24`, sh id→`0x8009CE68`; hit other: record guest addr →
  D7D8, sh −1→CE68, sh id→BD24; miss: all −1. Returns 1/0. (Recorded
  honestly: an sra-for-srl mutation is behaviorally equivalent here —
  the andi keeps only shifted bits [15:0] = original [27:12], identical
  under both shifts — so no such mutant is claimed; the andi itself is
  mutant-proven load-bearing instead.)
- **wm_8008BEC8** `s32 (u32 obj)` — two-axis approach stepper:
  d = |target − (pos sra 12)| (wrap abs); d<5 sets close bit (X=1,Z=2);
  else pos += lo32(vel × (sign16(step) arithmetic-halved toward zero));
  tail wraps via wm_80093354(obj+0x28) and stores
  wm_80093978(pos.X,pos.Z) → obj[0x2C]. Returns mask 0..3.
- **wm_8008C1DC** `void (u32 ctx, u32 obj, u32 out)` — class dispatch on
  sign16(wm_80093F18(obj+0x28)): ==3 builds the packed SVECTOR pair at
  out+0xA0/0xA8 (store order A0,A2,AC,A8,A4,AA; positions sra 12; low
  half of obj[0x5C] → +0xAA) and calls wm_80089160(ctx,+0xA0,+0xA8);
  else wm_800894C8(ctx). Void: both accepted callees are void and all 8
  retail call sites discard $v0 (instruction-verified in this lane).

Dependencies: all callees canonical (ratan2/rcos/rsin, 93354, 93978,
93F18, 89160, 894C8) — verified strong `T` in the canonical build.

## Certification

- `run_w34b24_i6_a72c_smallpack.sh`: retail full-file + five slice SHA
  gates; combined focused oracle sections A–E; `-O0`, `-O2`,
  `-O2 -fsanitize=undefined -fno-sanitize-recover=all` — normalized
  stdout byte-identical, stderr empty.
- Independent oracle highlights: pool stride/width/canary checks incl.
  sw-vs-sh detection; angle bias wrap (0xD00→0x100), negative ratan2
  (−1→0x3FF), u32 subtraction wrap, INT_MIN negu; inclusive rect bounds
  at both ends, second-record stride, empty list, wrapped-negative and
  mask-load-bearing coordinates; step −5 → −2 rounding toward zero,
  d==5-vs-4 threshold, low-word delta wrap (0x40000000×4→0); exact
  store-order and callee-argument traces throughout.
- Mutants: **25/25 KILLED** (5 per helper, distinct fault classes) —
  see MUTANTS.csv, including the honest equivalence note.
- Build: clean + incremental `LINK OK`; exactly one strong `T` per
  symbol; no stub shadows; `function_stubs_before=240,
  function_stubs_after=240` (none of the five was previously
  auto-stubbed — nothing referenced them), `data_stubs=524`,
  game TUs `compiled=47 skipped=0`.
- Suites: **20/20 PASS** (all accepted suites incl. the four landed this
  campaign, 73b04 via its documented lib override).
- Hardened `w18b_natural.gdb`: `verdict=PASS exit=0 zero_verified=13
  hit=5 not_instrumented=0 instrumentation_error=0`.

## Runtime payoff

None yet by design: these bodies are only called by `wm_8008A72C` (and
other unimplemented callbacks). With this pack accepted, `wm_8008A72C`'s
missing direct dependencies reduce to exactly one: `wm_80095414`
(keystone, in its own lane). G3 readiness advances accordingly.
