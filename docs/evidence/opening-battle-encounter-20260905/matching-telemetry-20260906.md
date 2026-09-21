# Matching build telemetry leakage audit

Date: 2026-09-06
Scope: only `src/slus_006.64/system/animation_scripts.c` and
`src/slus_006.64/system/temp1.c`. No full native or matching build was run.

## Diagnosis

The matching build does not define `XENO_PC_PORT`, so it intentionally does
not include host `<stdio.h>`. The reported failures were host-only JSON
telemetry immediately before existing fail-loud `assert(0)` paths:

- `animation_scripts.c`: the dispatch fallback and three unimplemented `0xBC`
  subpaths.
- `temp1.c`: the dedicated-opcode fallback and the ordinary opcode fallback.

The asserts are retained unchanged. No opcode dispatch, data path, or
success/failure behavior was altered.

## Change

Each `fprintf(stderr, ...)` plus `fflush(stderr)` pair is now enclosed in the
smallest surrounding `#ifdef XENO_PC_PORT` guard. The host branch still emits
the diagnostics before asserting; the matching branch reaches its existing
assert macro without referencing stdio.

## Hashes

Hashes captured immediately before the guards:

```text
animation_scripts.c  51cef2ac3afc4ffce35925b28701370d1ac32843951bf207ffec65c6d4da4b53
temp1.c              9f112a23055c7beffb536597c23a7ff504588206735aed3d98ceee6c6a640441
```

Hashes after the guards:

```text
animation_scripts.c  7795145bda9f0e39577be0073110db2e7564fe9adab0a7cf4ef29e9bab357acf
temp1.c              ef557f005fecc41317f7992f968ca819854e1e21dff6f6a953eea8292f30f451
```

These files already contained unrelated dirty-worktree changes; the hashes
above are the before/after values for this bounded edit, not clean-repository
baselines.

## Verification

- Host preprocessor (`gcc -E -P -std=gnu17 -DXENO_PC_PORT` with project shim
  includes): both files preprocess successfully and retain their telemetry.
- Matching preprocessor (`gcc -E -P -std=gnu17` without `XENO_PC_PORT`): both
  files preprocess successfully; scans of the preprocessed output contain no
  `fprintf`, `fflush`, or `stderr` references.
- Source assert scan: four animation-script hard stops and two temp1 hard
  stops remain.
- `git diff --check` passes for both owned files.
- The prior matching failure evidence remains `/tmp/xeno-battle-result-probe-20260906/make-check.after.log`.

The host syntax-only check was not used as a gate because these translation
units have existing standalone dependencies outside this bounded telemetry
edit. Root owns integration and the real matching/native builds.

## Root integration

The isolated matching checkout held the exact recorded before hashes. Root
reviewed the diff: six preprocessor guards only, with every assert retained.
Atomic copies preserved possible hardlinks. Native build passed (`LINK OK`),
646 unresolved symbols / 79 function stubs / 566 data placeholders. Resulting
binary SHA256: `e5b7b574a23000404020db567f3bfeb17e41bb2044298cd445b4dec38513823a`.

`make check` still exits 2. It now compiles past the prior stderr errors and
fails at resident linking on unresolved jump-table labels (72 linker diagnostic lines), including
`.L80023490` and the `8003F.../80040...` family. This is not a matching pass.
Those newly exposed ownership failures were not repaired by this logging change.
Full error list and log pins: `/tmp/xeno-matching-telemetry-20260906/integration-result.json`;
logs `make-check.after.log` and `native-build.after.log` in that directory.
Runtime gameplay was not repeated for these preprocessor-only guards.

Count clarification: 72 undefined-reference diagnostic lines comprise 69 individually
listed references plus three “more undefined references follow” summary lines.
The total unresolved-reference count is not established by those lines.
