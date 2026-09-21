# W34N3 Phase 3 — vestigial-gate retirement

Phase 3 retired exactly one flag: `XENO_WORLD_967E4_ROUTE`.

The flag formerly guarded one bounded call to the already port-owned
`wm_800967E4_dispatch_cd_work()` after the W24E archive-index transition. The
Phase 2 off-probe completed all 120 frames with both standing baseline
digests. Retirement deletes the environment check and retains the dispatcher
at the same sequence point whenever `XENO_WORLD_ARCHIVE_SET_INDEX` has enabled
the containing transition. No dispatcher, queue, or state-machine behavior was
added or selected.

Verification after retirement:

- normal `pc_port/build_port.sh`: **LINK OK**
- frame 60:
  `26e59e424829349c4d47de6523b3dbbf9e7d91f35629ec026c4709e703e576b3`
- frame 120:
  `ae06268a046507f3544a13e9c8708bbec57bcba16c526fd68eecf1e3f982a3dc`
- `rg XENO_WORLD_967E4_ROUTE pc_port/src`: zero occurrences
- `git diff --check`: clean

`XENO_WORLD_DRAW_PACKETS` and `XENO_WORLD_FRAME_PROLOGUE` were not retired.
Their BASE probes were shadowed or mutually excluded, so they did not satisfy
Phase 3 condition (a).
