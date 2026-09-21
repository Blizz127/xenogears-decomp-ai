# D3 tripwire hygiene audit

Evidence source: `slice_05_natural.log`, produced by the re-anchored
natural prologue diagnostic after W34B30. The registry in
`pc_port/src/world_map_init.c` still declares all 15 forbidden targets.

## Registry status

| target | status | evidence |
| --- | --- | --- |
| `0x800712D0` | intact | diagnostic entered the implemented driver; no `should_not_run` error |
| `0x80072238` | intact | mode-loop tripwire remains zero-hit |
| `0x8007299C` | intact | renderer sentinel and scheduler slot-2 guard are zero-hit |
| `0x8009766C` | intact | no forbidden-target error; 0x800967E4 diagnostics are clean |
| `0x80074E58` | intact | no forbidden-target error |
| `0x80075030` | intact | no forbidden-target error |
| `0x800739B8` | intact | no forbidden-target error |
| `0x80088F64` | intact | no forbidden-target error |
| `0x80037FD8` | intact | no forbidden-target error |
| world `DrawOTag` | intact | `world_DrawOTag: ZERO VERIFIED` |
| world `ArchiveCdDataSync` | intact | no forbidden-target error |
| loop dispatch `0x80072514` | intact | convergence/loop diagnostics remain zero-hit |
| `0x800967E4` | intact | route executes only through the implemented diagnostic path; forbidden counter remains zero |
| loop backedge `0x80072530` | intact | `loop_backedge: ZERO VERIFIED` |
| loop exit `0x80072538` | intact | `loop_exit: ZERO VERIFIED` |

The convergence and common-tail guards are also intact: excluded
`0x80072784`, common-tail `0x8007290C`, and the `0x8007299C` sentinel all
report `ZERO VERIFIED`. No tripwire was weakened, removed, or bypassed.

The W34B30 image-list boundary has its separate safe negative-path guard:
the focused certificate exercised unknown context/head/data values and
confirmed log-and-count behavior; the natural run observed zero unknown
values.
