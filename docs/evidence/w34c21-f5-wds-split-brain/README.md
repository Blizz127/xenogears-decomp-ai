# W34C21 — F5 WDS authority witness

## Scope and anchor

This is a read-only runtime witness for W34C15 F5 on
`experiment/worldmap-open-gates-20260823` at starting HEAD
`7aa1881b16f685eefb43bb6b58c0b7261189f3e4`. Local HEAD matched
`origin/experiment/worldmap-open-gates-20260823` before instrumentation.

The W34C15 route classification was stale: `wm_first_wds_consumer` is part of
the current accepted route. `world_first_wds_consumer_enabled()` at
`pc_port/src/world_map_init.c:869` is true when either
`XENO_WORLD_FIRST_WDS_CONSUMER` or `XENO_WORLD_ARCHIVE_SET_INDEX` is set. The
accepted route sets the latter, reaches `wm_first_wds_consumer`, and executes
its publication to guest address `0x8006258C` once.

The split-brain anchor still matches source:

- `wm_first_wds_consumer` stores the raw native return bits from
  `SoundLoadWdsFile` only in `WM_U32(0x8006258C)`.
- The generated native symbol `g_GameCurLoadedWDS` is separate storage.
- Native cleanup consumers in `src/slus_006.64/system/temp3.c:393-398` and
  `src/field/main/misc4.c:97,192` read the native symbol, not the guest word.
- The field streaming path in `src/field/main/misc8.c:2759-2779` also writes
  the native symbol directly.

## Natural-route witness

The exact accepted 120-frame route used scripted input
`0:0x2000,600:0x4000,916:0x2000,976:0` and detached before
`PcPort_WorldMapInitMain`.

At the sole publication:

```text
source_valid=1
source_psx=0x801c330c
source_host=0x7171ec
SoundLoadWdsFile result=(nil)
native_before=(nil)
native_after=(nil)
guest_before=0x00000000
guest_after=0x00000000
```

`SoundLoadWdsFile` emitted the existing `SoundHandleError` stub trace and
returned null. Therefore the writer is reached, but this run cannot expose an
authority divergence: publishing zero leaves both independent stores equal by
coincidence. It does not prove that the guest-only raw-native-pointer
publication is valid for a successful load.

The run reached and fulfilled frames 60 and 120, then returned from the bounded
open loop. Capture digests were unchanged from the W34C18 accepted baseline:

```text
frame 60  26e59e424829349c4d47de6523b3dbbf9e7d91f35629ec026c4709e703e576b3
frame 120 ae06268a046507f3544a13e9c8708bbec57bcba16c526fd68eecf1e3f982a3dc
```

## Activation analysis

The producer is ordinary world initialization, not a rare interaction arm.
At current HEAD, `XENO_WORLD_ARCHIVE_SET_INDEX=1` transitively enables the
archive-ready poll and first-WDS consumer before `ArchiveSetIndex(36, 0)`.
The split becomes observable when the same call returns a non-null
`SoundWDSEntry *`.

The stale native half becomes consequential when native cleanup or field sound
code later frees or replaces `g_GameCurLoadedWDS`. Those consumers do not read
the guest twin. A successful WDS load is therefore the bounded state needed to
settle the intended authority; merely forcing a non-null pointer would be
unsafe and was not done here.

## Verdict

`REACHED_NULL_MASKED_SPLIT_BRAIN`

F5's writer is naturally reached on the accepted route, but its null result
masks the concrete source-level authority split. Queue the authority repair
behind a successful-load witness; do not convert the guest store mechanically,
because that would still leave the native consumers stale.

Temporary diagnostics were removed. A normal rebuild completed with `LINK OK`,
and the tracked production tree was restored before this evidence-only commit.
