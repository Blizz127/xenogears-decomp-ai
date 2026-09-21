# W34C18 — F1-F3 guest-frame repair, anchored rerun

- Branch: `experiment/worldmap-open-gates-20260823`
- Starting HEAD: `f4dc3a2d364d64b376b966bc29217463059eb8e0`
- Starting origin: `f4dc3a2d364d64b376b966bc29217463059eb8e0`
- Date: 2026-08-28
- Verdict: **REPAIRED_VERIFIED**

This rerun supersedes the earlier W34C18 `SITE_MISMATCH` result.  Its anchors
are the committed W34C16 F1 record and the actually executed, committed
W34C17R F2/F3 record (`f4dc3a2d`).

## Anchors

All three evidence-named sites matched the starting source:

- F1: `world_map_callback_914d0.c`, native `delta_vec` passed to
  `wm_80093484`.
- F2: `world_map_helper_8e0f0.c`, native `dir` passed to `wm_80095414`.
- F3: `world_map_helper_95cd4.c`, native `attr_buf` passed to
  `wm_80084D00`.

## Established guest-frame pattern

`WM_95414_FRAME_ATTR` exists at `0x801FFE18`.  Its actual implementation
directly materializes retail's stack halfword in a dead guest-stack region;
there is no host mirror and therefore no copy-in/copy-out at that particular
site.  The source documents the heap ceiling as `0x801FC000` and the region
above it as otherwise unused by the port.

W34C18 follows that established hard-coded guest-frame reservation mechanism.
The new reservations are distinct, aligned, non-overlapping, below the
existing `WM_95414_FRAME_ATTR`, and above the heap ceiling:

```text
0x801FFDC0..0x801FFDCB  F1 delta_vec (12 bytes)
0x801FFDD0..0x801FFDDB  F2 dir       (12 bytes)
0x801FFDE0..0x801FFDEF  F3 attr_buf  (16 bytes)
0x801FFE18..0x801FFE19  existing WM_95414_FRAME_ATTR
```

No other production reservation in `0x801FF000..0x801FFFFF` was found.

## Per-site repair

- F1 copies all three host words into `WM_914D0_FRAME_DELTA`, passes that
  guest address to `wm_80093484`, then copies all three words back.  The callee
  reads X/Z and conditionally rewrites X/Z, so copy-back is required.
- F2 copies all three direction words into `WM_8E0F0_FRAME_DIR` and passes the
  guest address to `wm_80095414`.  The complete reachable callee chain treats
  `dir_vec` as input; output is written through the separate `out` argument,
  so there is no copy-back.
- F3 copies the host attribute-buffer image into `WM_95CD4_FRAME_ATTR`, passes
  the guest address to `wm_80084D00`, and copies back only on a nonzero return.
  `wm_80084D00` reads no output-buffer bytes and conditionally stores a
  halfword before returning nonzero; a zero return takes the caller's fallback
  without reading `attr_buf`.

Each site carries an F1/F2/F3 repair comment naming its witness and W34C18.
After the edit, the three affected files contain zero stack-address
`(u32)(uintptr_t)` truncations reaching `wm_80093484`, `wm_80095414`, or
`wm_80084D00`.

## Dormant-route neutrality

A normal build at the starting HEAD preceded the baseline run.  Both baseline
and repaired runs used the exact accepted environment, detached before
`PcPort_WorldMapInitMain`, ran 120 bounded frames, and used:

```text
XENO_TEST_INPUT=0:0x2000,600:0x4000,916:0x2000,976:0
```

Both runs fulfilled captures at frames 60 and 120 and returned from the
bounded world loop.  The artifacts are byte-identical:

```text
          baseline/post SHA-256
frame 60  26e59e424829349c4d47de6523b3dbbf9e7d91f35629ec026c4709e703e576b3
frame 120 ae06268a046507f3544a13e9c8708bbec57bcba16c526fd68eecf1e3f982a3dc

cmp frame 60  = identical
cmp frame 120 = identical
```

This is the required neutrality result: W34C16/W34C17R proved all three arms
dormant on this route, so no output difference was permissible.

## Updated F1-F3 standing note

F1, F2, and F3 are repaired by this rung: the native-stack truncations no
longer exist and all three callees now receive guest addresses backed by
distinct retail-stack reservations.

The residual is intentionally retained.  The accepted route does not arm any
of the three sites, so the copy-in/copy-out behavior has only been verified
neutral while dormant.  The first natural route that drives slot 9 into F1's
guard or wakes slot 7 and reaches F2/F3 must run a bounded confirmation of the
repaired live semantics before the findings are closed outright.

This standing-note update supersedes W34C17R's unrepaired status while leaving
the historical witness unchanged.

## Verification and hygiene

The repaired normal build completed with:

```text
LINK OK -> pc_port/build_native/xeno-port (port-owned addresses verified)
```

`git diff --check` passed.  The tracked diff for this commit is limited to the
three named production files and this evidence record.  Pre-existing untracked
workspace files were not modified or staged.
