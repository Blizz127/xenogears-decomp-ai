# W34C1 Rung 1 — native animation guard restoration

Date: 2026-08-24

Starting production HEAD: `045afb6beda4c7a1ade14a2cb4e2dfe41050906a`

## Repair

Eleven animation-state guards in the world-player and follower callbacks now
read byte `+0xAF` directly from the native sprite pointer stored in scheduler
slot `+0x4C`. This matches retail's direct loads, including
`0x8008CCAC: lb v1,0xAF(a0)` and `0x8008D894: lb v1,0xAF(a0)`.

Touched callback TUs:

- `world_map_callback_8a72c.c` — idle and walk guards
- `world_map_callback_8b644.c` — idle and walk guards
- `world_map_callback_8c844.c` — idle, walk, and animation-3 guards
- `world_map_callback_8d678.c` — animation-3, idle, walk, and state-30 guard

The focused production certificate uses native-backed sprite records and sets
the incorrectly remapped guest byte to a conflicting value. It checks exact
call counts for unchanged and changed idle/walk/animation-3 states. Results:

- O0: PASS
- O2: PASS
- nonrecovering UBSan: PASS
- strict warnings: clean
- source inventory: exactly 11 native-pointer guards
- `WM_ANIMATION_GUARD_MUTANT_GUEST_REMAP`: DETECTED by named assertion
- existing `wm_8008A72C` certificate: PASS, including the same mutant
- normal PC port: LINK OK

## Natural seam fork

Before the repair, frames after the first world seam showed:

`set_anim=4/frame`, `AnimScriptTick=0/frame`, `func_8001D2B0=0/frame`.

The same two sprite records were restarted twice per frame at animation 3,
wait 1, pose 0.

After the repair, frames 2–4 show:

`set_anim=0/frame`, `AnimScriptTick=0/frame`, `func_8001D2B0=0/frame`.

Both records remain animation 3, wait 1, pose 0. The retail-divergent restart
mechanism is closed, but the visual half of fault #3 remains blocked by a
separate missing stepper.

## Rung 1 fork verdict: stepper missing

Retail `AnimScriptTick` at `0x80023210` decrements sprite `+0x9E`; when it
reaches zero, `func_800248D4` advances animation bytecode and publishes the
new pose through `func_8001D2B0`.

For world-map sprites, the direct retail caller is inside `wm_80085CDC`:

- object loop begins at `0x80085E68`;
- `0x80085F10` calls `func_800223B0` for heading/direction;
- `0x80085F1C` calls `AnimScriptTick`;
- the loop advances through all 64 scheduler objects.

The current port of `wm_80085CDC` ends after its partial Phase 3 and omits the
retail tail `[0x80085E48,0x80085F58)`, including the animation tick. The
stepper is therefore not one of the eight W34B73 once-per-frame stubs; it is an
omitted tail in an already-ported world helper.

## Next exact task (Rung 1b)

Restore and certify retail `wm_80085CDC[0x80085E48,0x80085F58)`, including its
projection/depth gate, object submission, direction update, and exactly one
`AnimScriptTick` call per eligible object. Then repeat the seam probe and
require `+0x9E`/`+0x34` to advance without reintroducing animation restarts.

Untracked probe artifacts:

- `scratchpad/w34c1_animation_seam_after.gdb`
- `scratchpad/w34c1_animation_seam_after.log`
- `scratchpad/w34b73_animation_*.gdb/.log`
