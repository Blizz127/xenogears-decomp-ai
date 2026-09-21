# W34C20 — F8 archive queue pointer-domain census

- Branch: `experiment/worldmap-open-gates-20260823`
- Starting HEAD: `9ba4c614`
- Date: 2026-08-28
- Verdict: **VALID_DOMAINS_ONLY (forced-world bound)**
- Production changes: none

## Anchor

W34C15's polymorphic consumer remains at `archive_port.c:67-124`.
`ArchiveReadPsxStreamQueue` interprets each nonzero 32-bit `pData` as:

- guest KSEG when it is in `[0x80000000,0x80200000)`;
- otherwise a raw host pointer.

`func_80029AFC` selects this reader for the special `D_800B2394` queue or for
an entry table whose host address lies inside `g_PsxRam`
(`archive_port.c:274-292`).

## Natural-route census

The accepted detached 120-frame route completed normally.  The valid census
recorded every nonzero `pData` consumed by `ArchiveReadPsxStreamQueue` during
the run.  There were three queues and 12 entries:

```text
queue 1 (4 entries)
archive 2   pData=0x800a4238 KSEG
archive 4   pData=0x800adf30 KSEG
archive 19  pData=0x800ab458 KSEG
archive 19  pData=0x800b3b9c KSEG

queue 2 (3 entries)
archive 44  pData=0x80199bb4 KSEG
archive 45  pData=0x801ab42c KSEG
archive 46  pData=0x801a41dc KSEG

queue 3 (5 entries)
archive 47  pData=0x801c330c KSEG
archive 48  pData=0x800e3f38 KSEG
archive 49  pData=0x800e5374 KSEG
archive 50  pData=0x800ec30c KSEG
archive 51  pData=0x800edb44 KSEG
```

Counts by domain:

```text
KSEG                     12
RAW_LOW_NATIVE_IN_RAM     0
INVALID_OTHER             0
```

The first attempt used buffered stdout and lost/interleaved later records when
the completed post-loop placeholder process was terminated.  It was rejected
without a verdict.  The diagnostic was changed to stderr and the identical
natural route was repeated; the complete census above is from that valid run.

## Route analysis and bound

The three forced-world callers at `world_map_init.c:2542,2634,4189` pass the
guest queue at `0x8009D3F8`.  Every observed producer published KSEG, so the
consumer selected `PSX_ADDR` for all 12 entries.

This does not settle the contract globally.  `src/slus_006.64/system/temp3.c`
also calls `func_80029AFC` with special queue `D_800B2394`; that field/kernel
path was not reached by this forced-world run.  Other callers pass native
`StreamDataQueueEntry` arrays and take the separate native-layout path, so they
are not part of this F8 census.

The polymorphic fallback remains a watch item.  A future route reaching
`D_800B2394`, or a newly enabled guest-resident queue producer, should repeat
the same classification and reject any value outside KSEG or a demonstrably
low native address within `g_PsxRam`.

## Verdict

**VALID_DOMAINS_ONLY (forced-world bound)** — all 12 live values were valid
KSEG guest addresses.  No domain defect is observable in the exercised
consumer set, but unexercised special-queue producers remain undetermined.

## Hygiene

All `XENO_DIAG_W34C20_F8` instrumentation was removed.  The normal build
completed with:

```text
LINK OK -> pc_port/build_native/xeno-port (port-owned addresses verified)
```

`git diff --check` passed and the tracked production tree returned to the
starting content before this evidence-only commit.
