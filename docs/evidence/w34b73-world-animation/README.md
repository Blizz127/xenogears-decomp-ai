# W34B73 — world-map fixed-pose animation discriminator

Date: 2026-08-24

Starting/ending production HEAD: `045afb6beda4c7a1ade14a2cb4e2dfe41050906a`

Observed live after W34B68: world-map player and NPC models translate while
remaining in a fixed pose. This is fault #3, separate from the malformed world
geometry recorded after W34B72.

## Stub-frequency result

The accepted W34B72 120-frame log contains 960 `worldmap-stub` events. Eight
addresses occur exactly once per frame (120 occurrences each):

- `0x80096694`
- `0x80075104`
- `0x80074F2C`
- `0x8007299C`
- `0x80072238`
- `0x800250E0`
- `0x80025044`
- `0x8001D468`

No stub scales with the number of animated entities. The generic pending-frame
queue `D_80059190` is zero at the pre-`func_8001D468` seam on seven consecutive
world frames, so the local `func_8001D468` shadow stub has no natural work to
drain on this route. It is technical debt, but it is not the observed blocker.

## Animation-path counters

At the first world-frame seam, bootstrap totals were:

`func_800248D4=529`, `func_8001D2B0=688`, `func_800245D8=728`,
`TimerWorkListUpdate=951`, `WorkListUpdate=916`.

For each of the next three world frames:

- `func_800245D8`: 4 calls/frame
- `func_800248D4`: 0 calls/frame
- `func_8001D2B0`: 0 calls/frame
- `TimerWorkListUpdate`: 0 calls/frame
- `WorkListUpdate`: 0 calls/frame
- `D_80059190`: zero

The two affected sprite records are also absent from `g_TimerWorkList`; the
list head is zero at the inspected seams. This rules out a queued pose being
dropped by `func_8001D468` and rules out a stalled timer-list task for these
world sprites.

## First natural divergence

The same two native sprite pointers are reset twice per frame:

- `0x0064D9FC`, animation 3, from `wm_8008C844`
- `0x0064DC60`, animation 3, from `wm_8008D678`

Each reset occurs once in the scheduler pass inside `wm_800712D0` and once in
the scheduler pass in `wm_80071034`. At every `func_800245D8` entry, the real
sprite record already reports animation 3, wait 1, pose 0. The repeated calls
therefore restart an already-selected animation before its pose can advance.

The guards and the callee use different pointer domains:

- the callback loads a native heap pointer from scheduler slot `+0x4C`;
- the current guard reads `+0xAF` through `PSX_ADDR`, remapping that native
  pointer into the guest RAM mirror;
- `func_800245D8` correctly dereferences the original native pointer.

Retail reads the byte directly from the pointer in `$a0`, for example:

- `0x8008CCAC: lb v1,0xAF(a0)` before the animation-3 call in
  `wm_8008C844`;
- `0x8008D894: lb v1,0xAF(a0)` before the animation-3 call in
  `wm_8008D678`.

The current C therefore takes the call even though the actual sprite byte is
already 3. This is another host/guest pointer-domain transcription defect, not
an animation-driver stub.

The same incorrect guard pattern appears in 11 sites across:

- `pc_port/src/world_map_callback_8a72c.c`
- `pc_port/src/world_map_callback_8b644.c`
- `pc_port/src/world_map_callback_8c844.c`
- `pc_port/src/world_map_callback_8d678.c`

## Next bounded implementation

Correct only those retail direct-pointer animation guards, then add a focused
certificate using native-backed sprite records. It must distinguish native
heap pointers from guest addresses, prove exact `+0xAF` reads and conditional
`func_800245D8` calls, cover idle/walk/animation-3 paths in all four callbacks,
detect the current `PSX_ADDR` mutant, and run under O0/O2/nonrecovering UBSan.
Natural acceptance is that an unchanged animation is not restarted on either
scheduler pass, walk poses advance while entities translate, and stationary
entities settle to idle without renderer or scheduler changes.

## W34C1 Rung 1 resolution

The 11 guards were corrected and the natural restart count fell from four per
frame to zero. Poses remained frozen at wait 1 / pose 0. The follow-up proved
that retail steps these world sprites through `AnimScriptTick` at
`wm_80085CDC+0x240` (`0x80085F1C`), in a tail omitted by the current port.
See `docs/evidence/w34c1-rung1-animation-guards/README.md`.

Probe artifacts (untracked):

- `scratchpad/w34b73_animation_queue.gdb/.log`
- `scratchpad/w34b73_animation_tick.gdb/.log`
- `scratchpad/w34b73_animation_state.gdb/.log`
- `scratchpad/w34b73_animation_caller.gdb/.log`
