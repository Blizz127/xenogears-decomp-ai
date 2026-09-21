# W34N38 — complete retail `wm_80089580` particle tick

Verdict: **FULL_BODY_TRANSCRIBED_AND_CERTIFIED**.

## Anchor and authority

- Branch: `experiment/worldmap-open-gates-20260823`
- Starting HEAD: `c2ba3920887a8d394571a09ca65fb281b8826730`
- Retail authority:
  `scratchpad/w34_render_scan_k/world_map_full.objdump.txt`,
  function `[0x80089580,0x80089748)`
- Date: 2026-08-30

This leaf is called at the end of live helper `wm_80089748` on every accepted
world frame.  It was chosen before completing the larger `0x80089748`
spawner so that the latter will not be transcribed and certified atop a
misimplemented tick function.

## Static divergence and repair

The old body had retained only the narrow W34B39 correction to the slot-table
global.  Direct comparison with the 114 retail instructions found that it
still:

- treated the high packed halfword as the lifetime counter and ran cleanup
  when it was zero; retail instead skips a zero high-half owner and treats the
  signed low half as remaining life;
- stored `owner-1` into the low half instead of decrementing remaining life;
- used a fabricated `0x2a0` owner-slot stride where retail computes
  `index * 0x54`;
- sourced green/blue color deltas from the packed owner/lifetime word instead
  of the three signed bytes at record `+0x40`.

The completed body now matches the retail sequence over all 256 records at
`D_8009BDF4`, each `0x4c` bytes:

1. skip records whose packed high-half owner is zero;
2. for positive signed low-half life, decrement that low half, perform both
   wrapping vector integrations (`+04/+08/+0c += +14/+18/+1c`, then
   `+14/+18/+1c += +24/+28/+2c`), advance UVs with `+38/+3a`, and apply
   three signed RGB deltas from `+0x40` with retail 0..255 saturation;
3. for expired owned records, use the signed slot index at the record base,
   decrement `D_8009BCC0[index * 0x54].+0x0a`, then clear the owner and
   packed-life words.

The historical W34B39 fixture was corrected to express this retail owner/life
encoding; its original three slot-base mutants remain detected.

## Focused certificate

`pc_port/tests/run_w34n38_89580.sh` passes:

- O0;
- O2;
- nonrecovering UBSan;
- strict warnings.

The production-linked fixture proves an unowned record is entirely read-only,
exact two-stage vector integration, low-half lifetime decrement, exact UV and
clamped RGB updates, exact owner release, and the final (256th) record at
`0x4c` stride.

Thirteen named mutants are detected:

| mutant | failure mode |
|---|---|
| M1 | process owner-zero records |
| M2 | take remaining life from the owner half |
| M3 | store `owner-1` as remaining life |
| M4 | omit second vector integration |
| M5 | use `+0x3a` for both UV deltas |
| M6 | source RGB deltas from owner/life |
| M7 | omit RGB saturation |
| M8 | use old `0x2a0` owner stride |
| M9 | load the old wrong slot global |
| M10 | treat the particle pool as the slot pool |
| M11 | load `D_8009BCC0+4` |
| M12 | process only 255 records |
| M13 | walk records at `0x54` rather than `0x4c` |

The prior W34B39 certificate also passes O0/O2/UBSan and detects all three of
its original mutants.

## Native frame-601 lifecycle regression

The normal product build reports:

```text
LINK OK -> pc_port/build_native/xeno-port (port-owned addresses verified)
```

The exact accepted detached route was rerun with schedules:

```text
XENO_TEST_INPUT=0:0x2000,600:0x4000,916:0x2000,976:0
XENO_WORLD_TEST_INPUT=0:0x2000,600:0x20,601:0
```

It retained all three accepted capture digests byte-for-byte:

```text
frame 60  4310197d3881367bc7859ef174dcf4b9afa8759f0b7dc2ca3eea94b0f84f1521
frame 120 8039c2f96b88e09491eaf1cba049d0efd829f146aa943da744f56c93e85b1c16
frame 600 2b636ae21af4bf0003f60d5e53f8ffd9a042fc5a72457e4c096cff19814e1a38
```

At frame 601 the unseeded `0x0020` rising edge again produced:

```text
[worldmap-open-loop] natural state exit frames=601 D7CC=0
```

No primitive-link or OT-adapter abort was logged.  The post-exit process was
stopped after the natural world lifecycle had completed.

## Bound and next target

The route proves dormant-state neutrality, not a naturally active particle's
complete trajectory.  The focused certificate supplies the active and expired
state oracle.  The adjacent live `wm_80089748` spawner remains materially
incomplete and is the next bounded retail implementation target.
