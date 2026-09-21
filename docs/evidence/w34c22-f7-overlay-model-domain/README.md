# W34C22 — F7 overlay model-domain witness

## Scope and anchor

This read-only rung witnesses W34C15 F7 on the accepted forced-world route at
starting HEAD `fa972d8e52f873260145da50603ef7067495b062`. Local HEAD matched
`origin/experiment/worldmap-open-gates-20260823` before instrumentation.

The source anchor remains unchanged:

- `func_801E72CC` in `pc_port/src/game_overrides.c` reads
  `D_801E8670[selector]` as a raw native pointer, then reads the entry's `+4`
  word as another raw native pointer.
- Its source comment declares the table entries to be 32-bit PSX pointers.
  A producer that publishes KSEG guest values would therefore conflict with
  the consumer's raw-native interpretation.
- `func_801E742C`, the field object/model instantiation entry, is still an
  auto-generated no-op stub. `pc_port/build_port.sh:70-73` explicitly retains
  the object draw/instantiation entries as safe no-ops.
- The linked native image places `D_801E8670` in generated BSS and provides the
  generated stub for `func_801E742C`; no port-owned producer was found.

## Natural-route witness

The accepted 120-frame route used scripted input
`0:0x2000,600:0x4000,916:0x2000,976:0` and detached before
`PcPort_WorldMapInitMain`. A temporary entry counter on `func_801E72CC` and
table samples at the frame-complete seam reported:

```text
frame 1:   consumer calls=0, table[0..3]=0,0,0,0
frame 60:  consumer calls=0, table[0..3]=0,0,0,0
frame 120: consumer calls=0, table[0..3]=0,0,0,0
```

No selector or table-entry predicate was evaluated because the consumer itself
was never dispatched. The run fulfilled frames 60 and 120 and returned from
the bounded loop. Capture digests remained the accepted baseline values:

```text
frame 60  26e59e424829349c4d47de6523b3dbbf9e7d91f35629ec026c4709e703e576b3
frame 120 ae06268a046507f3544a13e9c8708bbec57bcba16c526fd68eecf1e3f982a3dc
```

## Arming-route analysis

This is not a world-map traversal arm. Located consumers are field-side:

- a field script matrix query calls `func_801E72CC` in
  `src/field/main/misc.c:464`;
- the model hierarchy uses it for non-`0xFFFF` additional matrices in
  `src/field/main/misc2.c:1552-1563`;
- particle mode 1 uses it in `src/field/effects/particles.c:411-416`.

The table-producing route is the field object pipeline in
`src/field/main/main.c:214-247`: when `D_800B2264 != 0`, it calls
`func_801E742C` once per registered object. That overlay entry remains a no-op,
so it cannot populate `D_801E8670`. Ordinary field scripts, model hierarchy,
and particles can consume the table only after the overlay instantiator is
implemented with an explicit pointer-domain contract.

## Verdict

`UNREACHABLE_CONFIRMED (forced-world route)`

F7 produces no runtime domain crossing on the accepted world route: the
consumer is not called and its generated table remains zero. The source-level
contract mismatch remains queued for the future field object/member-change
overlay work. Before enabling its producer, classify both table levels as
guest or native and convert consistently; do not infer a repair from the
current all-zero state.

Temporary diagnostics were removed. A normal rebuild completed with `LINK OK`,
and the tracked production tree was restored before this evidence-only commit.
