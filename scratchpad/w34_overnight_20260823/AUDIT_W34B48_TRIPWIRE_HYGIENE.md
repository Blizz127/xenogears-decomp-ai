# W34B48 — tripwire and indirect-call hygiene check

Read-only D3 detour. No production source changed.

## Forbidden-target registry

`pc_port/src/world_map_init.c` still defines all 15 registered guards and
the corresponding `should_not_run` bodies:

| guard | status |
| --- | --- |
| `0x800712D0` | intact |
| `0x80072238` | intact; zero-hit in latest natural run |
| `0x8007299C` | intact; zero-hit in latest natural run |
| `0x8009766C` | intact |
| `0x80074E58` | intact |
| `0x80075030` | intact |
| `0x800739B8` | intact |
| `0x80088F64` | intact |
| `0x80037FD8` | intact |
| world `DrawOTag` callsite class | intact; zero-hit |
| world `ArchiveCdDataSync` callsite class | intact |
| loop dispatch `0x80072514` | intact |
| `0x800967E4` | intact diagnostic route; forbidden counter zero |
| loop backedge `0x80072530` | intact; zero-hit |
| loop exit `0x80072538` | intact; zero-hit |

The convergence/common-tail exclusions remain guarded as well, including
`0x80072784`, `0x8007290C`, and the `0x8007299C` slot-2 sentinel. No guard
was removed, weakened, bypassed, or naturally retired.

## Guest callback dispatch

`world_map_scheduler.c` keeps the established safe boundary:

- scheduler fields are read as guest `u32` values;
- `wm_sched_resolve()` maps the known guest addresses to explicit native
  wrappers or registered test bodies;
- recognized-but-unimplemented addresses return `MISSING` and stop before
  the slot write;
- all other values return `INVALID` and stop without a native call;
- only the resolved `wm_sched_callback_fn` is invoked.

The latest W34B45 census/natural route remains `53/53`, missing 0, invalid 0,
with no raw guest callback invocation.

## CD40-style indirect slots

The only retail function-pointer slot found in the current world source is
`0x8009CD40`, consumed by `wm_80086798`. W34B25’s guard remains intact:
`0x80086700` maps to the explicit native `wm_80086700_cd40_init`; zero is a
no-op; any other nonzero guest value increments/logs the boundary and is
skipped. No second CD40-style slot or raw `jalr` dispatch was found in the
world source.

The `0x8008B2BC`/`0x8008BB40` and related callback files also contain
host-owned sprite-object fields represented as 32-bit native pointer bits.
Those are host-object calls, not guest function-pointer dispatches; this
slice does not alter that pre-existing representation. A future callback
state audit must still preserve its no-truncation/no-invalid-object
invariant before making those dormant cb0 paths natural.

## Decision

Tripwire hygiene is clean and does not unblock the D554 class-(e) boundary.
No new guard or source change is justified. The next task remains the
reviewed D554-closure decision, with the three-region audit-ahead packet
available for morning approval.

Evidence: `DETOUR_D3_TRIPWIRE_AUDIT.md`, current registry/source inspection,
`slice_17_natural.log`, and `slice_18_census.log`.
