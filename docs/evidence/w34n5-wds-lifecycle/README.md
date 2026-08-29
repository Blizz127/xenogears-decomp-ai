# W34N5 — field-to-world WDS ownership lifecycle

## Anchor and verdict

Starting HEAD was `d8263073418bd245ae159038e1c8c4243a4b0fc1` on
`experiment/worldmap-open-gates-20260823`; local matched origin and the tracked
tree was clean.  The committed W34C23 diagnosis remained accurate: a valid
world WDS request for SPU range `[0x38000,0x68000)` was rejected because the
field/music bank still owned `[0x38000,0x67830)`.

Verdict: `WDS_LIFECYCLE_ENABLED`.

The repair is bounded to the ownership lifecycle already present in retail:

1. the port-only host-staged field loader now publishes the returned
   `SoundWDSEntry *` to `g_GameCurLoadedWDS`;
2. the forced slot-1 setup spine restores retail's fresh-session call to
   `func_8001B66C` between cross-product setup and entry placement;
3. the world WDS consumer publishes a successful result to both the retail
   guest word at `0x8006258C` and the native symbol used by compiled cleanup
   consumers.

No loader, asset-pipeline, SPU allocator, or transition-hook behavior was
invented.

## Retail ordering anchor

The retail world overlay listing at
`scratchpad/w34b18_world_loop_7106c/W_72238_RAW.txt` establishes:

```text
0x80072378  jal 0x80098044        cross-product setup
0x80072384  lw  C894
0x8007238C  bnez C894, 0x800723A4
0x80072394  jal 0x8001B66C        fresh-session WDS/audio cleanup
0x800723D4  ...                    entry placement branch
0x800724D4  jal 0x80037FD8        SoundLoadWdsFile
```

Thus cleanup belongs inside base-world slot 1 when `C894 == 0`; it is not a
port-only field-to-world transition hook.  The current forced spine had the
surrounding stages but omitted this call.

## Pre-repair witness

An attached state-only witness on the accepted route showed:

```text
HOST_STAGED_RETURN ret=0x88a060 cur=0x0 has=0 f36c=0
WORLD_ENTRY                    cur=0x0 has=1 f36c=1
```

No `func_8001B66C`, `func_8001B5A8`, `SoundFreeWdsEntry`, or
`SoundSpuMemoryFreeBlock` occurred after the successful field-bank load and
before `PcPort_WorldMapInitMain`.  This proved two independent gaps: the
host-staged return was ignored, and the forced slot-1 sequence omitted retail
cleanup.

The pre-change detached acceptance produced:

```text
frame 60  26e59e424829349c4d47de6523b3dbbf9e7d91f35629ec026c4709e703e576b3
frame 120 ae06268a046507f3544a13e9c8708bbec57bcba16c526fd68eecf1e3f982a3dc
```

## Post-repair ownership witness

The same accepted route, attached only through the first world WDS consumer,
produced:

```text
FIELD_LOAD_RETURN ret=0x88a060 cur=0x0       has=0 f36c=0
WORLD_ENTRY                    cur=0x88a060 has=1 f36c=1
WORLD_CLEANUP_ENTRY hit=1      cur=0x88a060 has=1 f36c=1
WORLD_FREE_WDS hit=1 arg=0x88a060
WORLD_CLEANUP_RETURN           has=0 f36c=0
WORLD_LOAD_ENTRY
WORLD_LOAD_RETURN cleanup=1 free=1
  native cur=0x88a070
  guest  0x8006258C=0x0088a070
VERDICT=WDS_LIFECYCLE_ENABLED
```

The changed pointer value is expected reuse of the sound heap after the field
entry was freed.  The world load no longer emits `SoundHandleError`, returns a
real owner, and the previously null-masked F5 authority split is closed at the
publication seam: native and guest representations agree on the real result.

`g_GameHasLoadedWDS` is zero after the fresh-session cleanup and remains zero
after the world-specific load.  That flag belongs to the field music cleanup
state machine; this rung did not repurpose it for world teardown.

## Certificate and runtime acceptance

`pc_port/tests/run_w34n5_wds_lifecycle.sh` links the two production functions
from `world_map_init.c` and checks the field publication site.  It passes at
O0, O2, and nonrecovering UBSan with strict warnings on the focused test TU.
Mutants are killed by named assertions:

- M1: invert the `C894 == 0` cleanup gate;
- M2: drop native world-owner publication;
- M3: drop guest world-owner publication.

The normal port build completed with `LINK OK`.  A detached 120-frame run
fulfilled capture requests on their matching frames, reached the bounded exit,
and reproduced the pre-change hashes exactly:

```text
frame 60  26e59e424829349c4d47de6523b3dbbf9e7d91f35629ec026c4709e703e576b3
frame 120 ae06268a046507f3544a13e9c8708bbec57bcba16c526fd68eecf1e3f982a3dc
```

Visual equality is not evidence that the load failed: the ownership witness
proves a non-null world WDS entry, and this change affects audio state rather
than the rendered frame.

## Bound and next dependency

This closes the non-mechanical WDS/SPU ownership blocker named by W34N3 item 1.
It does **not** retire the early implication ladder or make retail slot 1 the
normal product path.  The remaining item-1 work is control-flow integration:
execute the already transcribed setup stages under `0x80072238` rather than as
environment-implied pre-initialization.

The retail dynamic trace/savestate oracle is a separate W34N5 checkpoint.  It
was not used as authority for this repair; the cleanup ordering above comes
directly from the retail overlay listing.
