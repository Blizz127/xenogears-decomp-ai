# W34C1 Rung 1b — retail world animation stepper

Date: 2026-08-25

Starting production HEAD: `49c36857a09452ca1b9d8864fadf108345257b54`

## Retail boundary and repair

`wm_80085CDC` ends at `0x80085F58`, not `0x80085FE0`. The port previously
stopped after copying object anchor coordinates and omitted the retail
projection/render/animation tail.

The restored implementation covers the complete retail function
`[0x80085CDC,0x80085F58)`:

- updates each active native-backed sprite transform from guest slot state;
- projects the native sprite anchor and stores raw `SZ3` by slot;
- publishes camera matrix `0x8009C808` through `func_80024FF4`;
- accepts state-zero, non-null objects with signed depth `< 0xB00`;
- submits to `OT + ((depth >> 4) * 4)` from the active guest environment;
- applies the retail wrapped heading clamp in steps of `0x100`;
- publishes `(heading - D_8009BD3A - 0x400) & 0xFFF`;
- calls `AnimScriptTick` after direction publication for every eligible object.

This bounded repair also corrects the pointer domain within this helper:
sprite transform and anchor accesses use the native sprite pointer stored in
guest slot `+0x4C`; pool, environment, camera, and OT metadata remain guest
address backed.

## Focused certificate

`pc_port/tests/run_w34c1_animation_stepper.sh` links the production helper and
uses asymmetric active, inactive, null, negative-depth, and depth-ceiling
records. It proves the 64-slot traversal, native transform writes, projection
eligibility, signed depth gate, exact OT bucket, both heading directions plus
snap, direction argument, and ordering:

`camera matrix -> render -> direction -> animation tick`

Results:

- O0: PASS
- O2: PASS
- nonrecovering UBSan: PASS
- strict warnings: clean
- M1 no animation tick: DETECTED
- M2 unsigned/wrong depth gate: DETECTED
- M3 wrong heading clamp: DETECTED
- M4 wrong OT bucket: DETECTED
- M5 wrong direction source: DETECTED
- W34C1 Rung 1 guard regression certificate: PASS
- normal PC port: LINK OK

## Natural seam

After the repair, frames 2–4 reach `AnimScriptTick` four times per world frame,
with zero animation restarts. An object-identity probe resolves those calls to
eligible sprite slots 1 and 2, each reached once per `wm_80085CDC` invocation.
The world driver invokes the helper twice per frame through its two scheduler
passes.

On the first observed active frame, both sprites enter with wait 1. The
`func_800248D4` decoder is reached twice and their timers drain to zero. The
previous seam watched scheduler callback slots 4 and 5 as though those numbers
were sprite slots; that assumption was incorrect.

## Detached native health

The accepted bootstrap debugger detached before `PcPort_WorldMapInitMain`.
The native open loop then:

- reached frame 60;
- reached frame 120;
- fulfilled both capture requests on their matching presentation frame;
- returned from the bounded 120-frame loop.

Captures:

- `scratchpad/w34c1_rung1b_capture/world-frame-000060.bmp`
  (`cf3c2261c23183b8dc906db0bae3fc2013ccdcc2dd75d53b40ae4dd1e4046510`)
- `scratchpad/w34c1_rung1b_capture/world-frame-000120.bmp`
  (`c3219feac7f766880f421d32771a2140e417a2312a0ec74fbbfd29470b2e62c7`)

The hashes differ. Walk/idle pose acceptance remains a separate Rung 3 gate
after Rung 2 provides deterministic in-process movement input.

Untracked diagnostic artifacts:

- `scratchpad/w34c1_animation_seam_rung1b.log`
- `scratchpad/w34c1_animation_tick_identity.gdb/.log`
- `scratchpad/w34c1_rung1b_native_detach.gdb/.log`
- `scratchpad/w34c1_rung1b_capture/`
