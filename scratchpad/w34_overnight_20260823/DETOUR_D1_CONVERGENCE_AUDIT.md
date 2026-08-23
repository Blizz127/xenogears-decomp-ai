# Detour D1 — convergence lane audit

The blocked frontier is the pre-mode-loop frame continuation at
`0x80071984`. The existing convergence lane is already covered by the
accepted production pieces; there is no bounded class-(a/b) gap to
implement without falsifying the mode-loop tripwire.

## Ordered path and coverage

| Retail range | Current coverage | Status |
|---|---|---|
| `0x80072238..0x80072378` | mode-entry state stores and state-transfer bridge in `world_map_init.c` | built behind explicit gates; full entry remains `should_not_run` |
| `0x80072378..0x800726C0` | mode setup/audio and the pre-convergence cut | built, natural evidence reaches the cut only after the frame driver returns |
| `0x800726C0..0x80072728` | `wm_800726C0_convergence_p1` | built and production-linked |
| `0x8007272C..0x80072780` | `wm_8007272C_convergence_p2` | built and production-linked |
| `0x80072784..0x8007290C` | excluded P2/common-tail arms | deliberately guarded; tripwires must remain zero |
| `0x8007290C..0x80072998` | `wm_8007290C_common_tail_p0..p5` | built and production-linked; returns sentinel `0x8007299C` |
| `0x8007299C..` | renderer/teardown function | deliberately `should_not_run` |

The natural proof logs confirm the convergence pieces execute during the
pre-existing setup path, while their forbidden successor paths remain zero.
The existing `world_map_convergence.c`, `world_map_common_tail.c`, and
`world_map_init.c` guards have no missing small leaf exposed between the
accepted pieces and the mode-loop entry. In particular, implementing the
excluded `0x80072784` arm or weakening `wm_80072238_should_not_run()` would
turn a future milestone into a forced entry rather than a natural one.

## D1 result

No D1 implementation slice is authorized. The productive next work is the
D2 audit-ahead queue below. The actual blocker remains upstream: the frame
continuation must cross `0x80074F2C`, `0x80075104`, `SetGeomOffset`, and the
DrawOTag/backedge sequence before the already-built convergence lane can be
entered naturally.
