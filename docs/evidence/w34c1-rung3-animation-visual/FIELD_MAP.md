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
| `+0x34` | native sprite | pose selected by the animation decoder |
| `+0x9e` | native sprite | wait/timer decremented by `AnimScriptTick` at `0x80023210` |
| `+0xaf` | native sprite | animation id initialized by `func_80023804`, generally set by `func_800245D8`, and restored by `func_80021D50`; world callback guards load it directly |
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
