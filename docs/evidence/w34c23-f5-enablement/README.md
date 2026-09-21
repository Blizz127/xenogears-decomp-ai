# W34C23 — F5 enablement diagnosis

## Anchor and scope

Starting HEAD was `75cf17db15e1fdba6aeb9e2f645f936536632fe4` on
`experiment/worldmap-open-gates-20260823`; local matched origin. The committed
W34C21 record and source anchors agreed: the accepted route reaches
`wm_first_wds_consumer`, which calls `SoundLoadWdsFile` with the guest pointer
from `0x8009C88C` and publishes only to guest `0x8006258C`.

This rung stopped after Phase 1. No WDS-loading or authority change was made.

## Exact null origin

`SoundLoadWdsFile` is a real compiled implementation at
`src/slus_006.64/system/sound.c:789-818`, not a generated stub. With mode zero,
`SoundSpuMemoryAllocateWDS` selects the header's fixed SPU address and calls the
real port implementation of `SoundSpuMemoryAllocateBlockAtAddress`.

The decisive accepted-route trace showed:

```text
world WDS request: address=0x00038000 size=0x00030000 type=0
allocator state:  an existing block ends at 0x00067830
computed gap:     0x00000000
allocator result: 0x00000000 (reject=gap_too_small)
SoundLoadWdsFile: error 0x1F, return NULL
```

The exact failed allocator check is
`src/slus_006.64/system/sound.c:1726-1728`: `gap < size` returns zero.
`SoundLoadWdsFile` then takes its allocation-failure return at
`src/slus_006.64/system/sound.c:793-797`.

The conflicting block was allocated earlier on the field route at the same
fixed address:

```text
existing field/music allocation: address=0x00038000 size=0x0002F830
existing block end:              0x00067830
world allocation:                address=0x00038000 size=0x00030000
world block end requested:       0x00068000
```

Thus this is not the unused automatic allocator stub. The generated
`SoundSpuMemoryAllocateBlock` stub is selected only when the resolved mode is
zero after mode decoding; this WDS header supplies fixed address `0x38000`.
The generated `SoundHandleError` stub is reached only after the allocator has
already rejected the request, so it is not the cause.

## Input and domain

The call arguments at `wm_first_wds_consumer` were:

```text
source_psx=0x801C330C
domain=KSEG guest RAM
source_host=0x7171EC
allocated archive bytes=197024 (0x301A0)
mode=0 (fixed address from header)
```

The first 64 bytes were:

```text
77647320d7e5a644a001000001010000a001000000000300
a0010000160000002e000000000000000080030000000000
00000000d80118db00ff7f0d71077f00
```

They decode to a `wds ` signature, header size/ADPCM offset `0x1A0`, ADPCM size
`0x30000`, ID `0x2E`, and fixed SPU address `0x38000`. Header plus payload is
exactly `0x301A0`, equal to the archive allocation. The input is resident,
internally bounded, and structurally plausible; it is not an unpopulated
buffer.

The existing runtime trace only prints `[stub] SoundHandleError`; it does not
print the error ID. Source and the temporary diagnostic establish that the
argument was `0x1F`, the WDS SPU-allocation failure.

The run reached frames 60 and 120 and returned normally. Captures retained the
standing hashes, as expected because loading still failed:

```text
frame 60  26e59e424829349c4d47de6523b3dbbf9e7d91f35629ec026c4709e703e576b3
frame 120 ae06268a046507f3544a13e9c8708bbec57bcba16c526fd68eecf1e3f982a3dc
```

An initial diagnostic pass established the input/header facts but did not
distinguish the allocator's three zero-return checks. It was not used for the
verdict. The final run added only the missing check witness and produced the
numbers above. A separate malformed input schedule was rejected before game
execution and produced no runtime evidence.

## Phase 1 verdict

`PRECONDITION_FAILED`

The real fixed-address allocator rejects a valid world WDS because an earlier
field/music WDS still owns the same SPU range.

## Phase 2 gate

The gate did **not** open. Criterion (a) is satisfied, but (b) and (c) are not
yet supported strongly enough for a production change.

Source identifies the relevant existing subsystem boundaries:

- the port-only host-staged field bank path in
  `src/field/main/misc8.c:2901-2926` calls
  `SoundLoadWdsFileHostStaged` and ignores its returned WDS entry;
- `g_GameHasLoadedWDS` is then set at `misc8.c:2929`;
- `func_8001B5A8` in `src/slus_006.64/system/temp3.c:395-399` can free
  `g_GameCurLoadedWDS`, but only through the field cleanup state machine;
- the field-to-world exit in `src/field/main/misc4.c:385-397` invokes that
  cleanup only under its transition flags.

The run proves that the old SPU block survives into world initialization, but
does not by itself prove whether assigning the ignored host-staged return,
changing the transition cleanup gate, or both is the retail-correct bounded
repair. Enabling the load therefore requires a focused field-WDS lifecycle
witness covering publication into `g_GameCurLoadedWDS`,
`g_GameHasLoadedWDS`/transition flags, `func_8001B5A8`, linked-list removal,
and `SoundSpuMemoryFreeBlock`. It does not require a new loader or asset
pipeline, but the countable production diff cannot yet be stated without
speculation.

## Standing status

- F1-F3 remain repaired by W34C18 and neutral on their dormant accepted-route
  arms; naturally armed copy semantics still await confirmation.
- F5 remains null-masked and its authority question remains open. W34C23 names
  the masking cause as a surviving field/music SPU allocation, not bad world
  data and not a loader stub.

Temporary diagnostics were removed. A normal rebuild completed with `LINK OK`,
and the tracked production tree was restored before this evidence-only commit.
