# W34N112 — mode-17 five-object particle renderer

- Branch: `experiment/worldmap-open-gates-20260823`
- Starting HEAD: `32492e27427ade49455a332f838b130e6c6e5c47`
- Date: 2026-08-31
- Verdict: **PASS**

## Retail anchors

The source was transcribed from `disc/world_map.bin` (load base
`0x8006FAF0`).  The focused certificate checks these exact slices before it
runs:

| range | role | SHA-256 |
| --- | --- | --- |
| `[0x80082F64,0x80083108)` | active-object transform and oscillator | `f385fea48beb367407abdf3ef2ecd981a50f142299a269365aa6d36e5717de07` |
| `[0x80083108,0x800831D8)` | primitive initialization | `4db38536cc83aad77b9390a751cf6f4502e75abd22d717788844482ce00e2beb` |
| `[0x800831D8,0x80083214)` | primitive colour writer | `3d05362719183b5ac038455cdd3a3886ed3ea5776ce763145f580c9c6cd7ab6c` |
| `[0x80083214,0x80083264)` | scheduler initializer | `a620e27bb61dede1c6b36de9bc3859bc5bf09908682677f27f2cc67d976b42e5` |
| `[0x80083264,0x800834D0)` | scheduler updater | `c79fa95b7b4e764e000663dc6b7e099a076da07e228b716bc5eb473ec7ce35be` |
| `[0x80082F64,0x800834D0)` | complete unit | `ab14db3e4a0a4e416db0aefd5223fb41b020258db87b21317a1c954bcbdc2eaa` |

The implementation restores the five slot-indexed object records, 32-byte
primitive streams, buffer mirroring, state tables at `AABC/AB48/ABD4`, colour
oscillation, angle/scale advance, matrix construction, and active-buffer colour
publication.  The scheduler resolves `0x80083214` and `0x80083264` symbolically;
no guest address is called as a native function pointer.

## Integration divergence and repair

The first natural run exposed a pre-existing pointer-domain defect in the
`0x80084580` C620 object-table producer.  `func_8002CB54` is shared native code
and returned low native heap pointers, which the producer stored unchanged in
record fields `+0x48/+0x4C`.  Retail stores PSX heap addresses there, and the
world callbacks correctly consume the fields through `PSX_ADDR`.

The pre-fix witness recorded:

- `g_PsxRam=0x587EA0`, `C620=0x800DAA98`, scheduler pool `0x800D8A90`;
- all five records shared descriptor `0x800A5738`, count `256`;
- slot 4 buffers were raw native `0x006A3640/0x006A5640`, but rebasing them
  selected guest aliases `0x800A3640/0x800A5640`;
- the slot-4 mirror therefore covered `0x800A5738` and overwrote the shared
  descriptor; slot 5 then read a corrupt count and overwrote scheduler records
  3 through 7.

This was witnessed directly: records 3 through 7 held the registered callback
pair immediately before slot 5 and contained primitive-looking garbage
immediately after it.  It was not an error in the retail primitive loop.

The bounded repair leaves the native model fill and native buffer mirror in
place, then converts only the two published record fields with
`host_ptr_to_psx_u32`.  A post-repair witness found the five pairs at:

```
slot 3  0x80117798 / 0x80119798
slot 4  0x8011B7A0 / 0x8011D7A0
slot 5  0x8011F7A8 / 0x801217A8
slot 6  0x801237B0 / 0x801257B0
slot 7  0x801277B8 / 0x801297B8
```

All are KSEG guest addresses and each descriptor still reports 256 primitives.

## Certificate

`pc_port/tests/run_w34n112_mode17_particles.sh` passes at O0, O2, and O2 with
nonrecovering UBSan under strict warnings.  It detects twelve named mutants:

1. wrong object stride;
2. wrong primitive opcode;
3. dropped final primitive;
4. missing buffer mirror;
5. state 2 using the state-1 table;
6. missing active flag;
7. short parameter copy;
8. wrong red lower clamp;
9. wrong angle mask;
10. missing matrix copy;
11. wrong active buffer side;
12. raw-native C620 buffer publication.

The mode-17 lifecycle certificate remains green, and the normal PC port build
links successfully.

## Natural acceptance

The accepted entrance-17 run used scripted input
`0:0x2000,600:0x4000,916:0x2000,976:0`, detached before
`PcPort_WorldMapInitMain`, and ran the corrected 120-frame cadence.

- `0x80083214`: 5 natural executions (slots 3–7)
- `0x80083264`: 600 natural executions (5 objects × 120 frames)
- `0x80083214/0x80083264` stub hits: 0
- upload-pump unknown records: 0
- OT-adapter abort diagnostics: 0
- frame 60 capture fulfilled at frame 60
- frame 120 capture fulfilled at frame 120
- bounded loop returned normally

Captures:

- frame 60: `scratchpad/w34n112_mode17_repaired_capture/world-frame-000060.bmp`
  — `5851ca7ca04f8f77b7e8eb1b9a22a4510ad97205109f0d34bdf462733b66d2d0`
- frame 120: `scratchpad/w34n112_mode17_repaired_capture/world-frame-000120.bmp`
  — `6a0f814c9a83160ea45b66338701e99bac400eb3e7b0b5196b2bd79907561b6a`

Both images show the stable mode-17 sky/distortion band.  Their hashes match
the W34N109–W34N111 baseline because this accepted route leaves the five
particle objects in their idle state; structural execution, not a forced visual
effect, is the natural gate.

## Remaining mode-17 frontier

The only remaining private pair on the accepted route is
`[0x800827EC,0x800828DC)`: initializer `0x800827EC` was unresolved once per
frame (120 hits), and updater `0x800828DC` was observed once after the bounded
return.  W34N113 should restore that pair next.
