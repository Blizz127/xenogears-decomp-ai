# W34C1 scheduler-slot and native-sprite field map

This is a read-only provenance note. It keeps the guest scheduler record,
native sprite record, and world controller-input domains separate.

| Field | Structure | Proven role |
|---|---|---|
| `+0x04` | scheduler slot | callback substate; `wm_80085CDC` reads it in the active-object loop |
| `+0x20` | scheduler slot | callback main state used by `wm_8008A72C`/`8B644`/`8C844`/`8D678` |
| `+0x24` | scheduler slot | suppress/render flag; not animation state |
| `+0x28` | scheduler slot | object X input consumed by `wm_80085CDC`, then written to native transform `+0x00` after camera delta |
| `+0x30` | scheduler slot | object Z input consumed by `wm_80085CDC`, then written to native transform `+0x08` after camera delta |
| `+0x38/+0x3c/+0x40` | scheduler slot | movement vector; slot 1's retail animation guard tests the three words, and the movement tail clears them |
| `+0x4c` | scheduler slot | native sprite pointer; retail loads directly at `0x8008CCAC`/`0x8008D894` |
| `+0x00/+0x04/+0x08` | native sprite | render transform populated by `wm_80085CDC` from slot state |
| `+0x34` | native sprite | signed pose selected by the animation decoder; live held input reached slot-1 poses 1, 2, 3, then the drawn pass remained pinned at 3 under the scheduler-cadence restart |
| `+0x9e` | native sprite | signed wait/timer decremented by `AnimScriptTick` at `0x80023210`; after the initial frame, the cadence fault resets it to 1 on both reassignment groups during effective-held frames 2-26 |
| `+0xaf` | native sprite | signed animation id initialized by `func_80023804`, generally set by `func_800245D8`, and restored by `func_80021D50`; world callback guards load it directly; live held input currently toggles slot 1 from 0 on the repeated outer pass to 1 on the drawn pass |
| `+0xd8/+0xdc` | native sprite | words that move smoothly for renderer-eligible slots 1/2; semantics not established and not the render transform |

The complete 64-slot census found only slots 1, 2, 4, and 5 with non-null
`+0x4c`. Slots 1/2 reach `func_8001E298`; slots 4/5 are suppressed. Earlier
changes in slots 0/10/15 were scheduler metadata, not hidden sprite records.

For slots 1/2, native transform `+0/+4/+8` remained fixed at capture seams
while `+0xd8/+0xdc` changed. A fixed camera-relative render transform is not
evidence that the object is stationary in world space.

## Input domains feeding slot-1 movement

`D_800AFE9C` is the field held-input word. The world frame driver instead
rebuilds guest accumulator `0x8009CD4C` from queued `g_C1ButtonState` values.
`wm_80090A84` reads `0x8009CD4C`, selects heading, and writes slot movement at
`+0x38/+0x40`. `wm_8008A72C` then selects animation 1 only when the movement
triple is nonzero.

The W34C1 scripted continuation wrote `D_800AFE9C=0x2000`, but the observed
world consumer had `g_C1ButtonState=0`, `0x8009CD4C=0`, and a zero slot-1
movement triple. That run cannot establish a walk-animation failure.

The prior Rung 3 probes successively conflated host and guest pointer domains,
the scheduler slot and native sprite structures, and finally field input with
world input. This map records all three boundaries explicitly.

## Live renderer-entry cadence result

The controller-source repair makes real Right input reach the world movement
consumer and select animation 1. A 50-frame state-only renderer-entry probe
then found two calls per eligible sprite per frame. Under held input, slot 1
is reset to animation 0 by the repeated outer scheduler and reselected to
animation 1 by the post-input drawn scheduler. Its wait timer is consequently
reset to 1 on both groups after the initial frame during effective-held frames
2-26, and its drawn selected pose stabilizes at 3 instead of advancing. Key-up
is issued at frame 26 and becomes observable in the drawn group on frame 27.
That group reaches animation 0 / wait 0 on frame 28; both groups are animation
0 / wait 0 from frame 29.

Retail does not run both scheduler sites at frame cadence. `0x80071064` is the
outer world-session pass; recurring displayed frames remain inside
`wm_800712D0` through `0x800719C8 -> 0x8007130C` and run only `0x80071488`.
The current flattened open loop repeats both sites once per bounded host
frame, which is the proven source of the animation restart. This is a nested
control-flow/cadence defect; it must not be repaired by special-casing the
animation guard or suppressing one scheduler call without restoring the
retail inner-frame boundary.
