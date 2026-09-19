# Field 14 is not locked: the painting room re-measured (2026-09-19)

Status: **runtime-measured** on this machine (Xvfb + xdotool, ordinary keys
only, no game-state writes). All source changes are inside `#ifdef
XENO_PC_PORT`, so the matching build is untouched.

This session set out to find "what field 14's exit script loop waits for", the
open question left by the 2026-09-18 push. The answer is that **there is no
such loop**. The three inherited claims about field 14 are all wrong, and the
real blocker is one the previous pass never tested for.

## What the previous pass concluded, and what is actually true

| Inherited claim (2026-09-18) | Measured 2026-09-19 |
| --- | --- |
| FE54's guard never passes, so the departure never arms | FE54's guard passes every time; it runs **once** in field 14, during scene setup |
| The room's script loop stalls after the FE54 lock | No stall. Actor 0 reaches its terminal park; actor 1 reaches **free player control** |
| Field 14 keeps Fei script-locked; no d-pad input moves him | Fei moves on 5 of 6 strides; position changes every time |
| Leaving needs the scene's exit condition decompiled | Nothing needs decompiling. It is a **navigation** problem |

## The instrument that settled it

`XENO_VM_TRACE` already existed but only printed each instruction pointer
**once** per actor, so a script sitting in a loop printed its body and then
went silent — precisely the case under investigation was the one it could not
show. Three additions (`src/field/scripts/virtual_machine.c`):

* `XENO_VM_TRACE_REPEAT=1` — print every dispatch, not just first-seen.
* `XENO_VM_TRACE_MAX=<n>` — per-actor line cap (default 4000).
* `XENO_VM_TRACE_FIELD=<n>` — only trace while field `<n>` is loaded, so the
  budget is not burned on the boot/prologue fields.

Plus a monotonic sequence number per line so actors can be re-interleaved.

`scratchpad/vm_trace_annotate.py` maps the raw opcode bytes back to handler
symbols **offline**, by parsing the ordered `FIELD_VM_HANDLER(name)` lines out
of `pc_port/src/data_field.c`. Nothing has to be kept in sync at runtime, and
the mapping is verifiable: it reproduces `FE54 -> func_80093B10` and both
table lengths (0x100 / 0xE3). `--loop` run-length-encodes the tail of the
trace, which is what makes a park or a cycle visible at a glance.

## What field 14 actually does after the dream battle

Traced with `XENO_VM_TRACE=all XENO_VM_TRACE_REPEAT=1 XENO_VM_TRACE_FIELD=14`:

* **Actor 0** (scene master) runs setup — `FE54` once at ip=29, `FE26` (start
  the painting distortion) at ip=31, party/gear/camera setup — then parks
  permanently at ip=114 on opcode **`0x5B` (`func_80095284`)**. That opcode
  deliberately never advances the IP (`src/field/main/misc11.c:1524` documents
  it as halt-and-hold); it is the script's **terminal state**, not a hang.
* **Actor 1** (Fei, the player) plays the scene — `FE4A`, sleeps, camera
  waits (`FieldScriptWaitForCameraMovement`) — and ends parked at ip=406 on
  opcode **`0x0C` (`func_8009F5A8`)**. That is the player **idle-hold
  wrapper**: it calls `func_8009F5F4` = OP_UPDATE_CHARACTER every frame and
  re-runs the same IP. An actor parked on `0x0C` is an actor under **player
  control**.
* **Actor 17** alternates `FieldScriptCheckTriggerZone2D` with opcode `0x00`
  every frame — the room's trigger-zone poller is live, so a zone entry would
  fire.

So the script completes and hands control to the player, exactly as retail
should.

## The FE27 distortion wait is a red herring (but was worth measuring)

Actor 1 passes through `FE27` sub-op 1 at ip=216, which waits for
`g_FieldEffects.distortion.isActive` to clear — and the only writer that
clears it on completion is `FieldDistortionDraw` (`duration` expired **and**
`isFinished` set). That is a genuine candidate for a hang, so it was measured
rather than assumed (`XENO_DISTORTION_DIAG=1`):

```
FE27 wait #1  isActive=1 isFinished=1 duration=69 v1=258400 draws=4888
FE27 wait #61 isActive=1 isFinished=1 duration=9  v1=33760  draws=4948
```

`duration` counts 69 -> 9 over 60 frames with the draw counter advancing, and
the script's ramp-down (`FE27` case 0 at ip=211, duration arg `0x46` = 70
frames) had already set `isFinished=1`. The wait is a normal ~1.2 s and
resolves. The lifecycle is healthy.

## Fei moves (`scratchpad/field14_walk.py`)

Boot to field 14, clear the post-battle dialogue, confirm actor 1 is parked on
`0x0C`, then one long stride per direction:

```
before      pos=(115,0,-455)
Up      ->  pos=(112,0,-463)   moved
Right   ->  pos=(88,0,-434)    moved
Down    ->  pos=(109,0,-413)   moved
Left    ->  pos=(157,0,-495)   moved
Up+Right->  pos=(157,0,-495)   no (wall)
Down+Left-> pos=(161,0,-492)   moved
```

`free-control opcode seen: True`. This is the direct refutation of the
script-lock reading. Note the displacements are **not** a fixed axis mapping —
field input is camera-relative and field 14's camera is scripted, which is why
earlier fixed-direction sweeps read as "nothing moves".

## The actual blocker: the only exit zone is outside the reachable floor

Two new read-only dumps in `pc_port/src/field_pos_diag.c` (both under the
existing `XENO_FIELD_POS_DIAG` switch):

* **ZONEDUMP** — every trigger zone's quad, once per loaded field.
* **ACTORDUMP** — every field actor's position/status/IP, periodically.

Field 14 has **exactly one** trigger zone:

```
ZONEDUMP map=14 count=1
ZONE  0 ... center=(335,-26)
```

That is the map 14 -> 13 door box already on record (x in [308,363], z in
[-52,0]). A closed-loop walk (`scratchpad/field_navigate.py`, which measures
what each direction does instead of assuming a mapping) reached only
`x in [88,323], z in [-496,-331]` before every direction stopped reducing the
distance, ~470 units short of the zone in Z, with `inZones=[]` in every sample
of every run.

Careful about what that does and does not prove: distance hill-climbing gets
trapped in local pockets, so "the search stopped here" is **not** "a wall is
here". ACTORDUMP settles it — field 14's actors stand at

```
9:(310,-243) 10:(270,255) 11:(75,-484) 12:(158,-544) 13:(16,-331)
14:(358,-421) 16:(-232,74) 17:(-100,-510) 19:(-350,-59) 20:(-350,-127)
```

i.e. the room extends to at least z=+255 and x=-350, well outside the region
the walk covered. So the floor is larger than the search found, and the honest
statement is: **no walk has yet reached field 14's only trigger zone**, and
the search method — not a proven wall — is the current limit.

Actor 14 at `(358,-421)` is the strongest exit candidate: it sits just past
the east edge of the covered region, which is the shape a door actor has (the
established Lahan pattern is to stand near a door actor's offset point and
press Circle, not to walk into a zone). `scenario=6` throughout. The recorded
2026-09-08 slice names agree that an NPC conversation stands between spawn and
exit: `stairs-*`, `dan-trigger`, `dan-0..38`, `house-exit-*`.

## Proof that the exit itself works: warp into the zone

Walking could not distinguish "cannot path there" from "the zone is broken", so
`pc_port/src/field_warp_diag.c` (`XENO_FIELD_WARP=<map>:<x>:<z>[:delay:repeats]`,
inert unless set) places the player once and the run reports what follows.
This one **writes game state**, unlike everything else here, so it lives in its
own translation unit with its own switch and its output is evidence about the
zone, not about ordinary play.

First attempt fired too early — at the first field-active frame, i.e. during
the room's setup scene, which then repositioned the player and erased it
(`WARP firing: (-151,1774)`, player ended at `(112,-458)`). With a delay:

```
WARP armed map=14 -> (335,-26) delay=600 repeats=5
WARP firing 1/5: (115,-455) -> (335,-26)
POSDIAG map=13 pos=(-37,0,0) ...
FIELD CHANGED -> 13   (route 490 -> 4 -> 2 -> 14 -> 13)
```

**The field changes on the very next sample.** The trigger zone, its poller
(actor 17), the transition and the field load are all working. The single
remaining problem in field 14 is getting the player from `(115,-455)` to the
door box on foot.

## Why replaying the recorded route does not solve it

`scratchpad/lahan-natural-20260908-compmatrix/run.log` proves the room is
navigable by hand: that session went `490 -> 4 -> 2 -> 14 -> 13 -> 1 -> 11 ->
12 -> 15 -> 17 -> 21 -> 19 ...`. Its `manual-input.log` records the way out:

```
easel-backoff    Down+Right 0.7s
easel-side       Up+Right   1.0s
stairs-approach  Up+Left    4.8s
stairs-adjust    Down+Left  0.15s
stairs-foot      Up+Left    1.4s
stairs           Up+Right   1.8s
dan-trigger      Up+Right   1.6s
dan-0 .. dan-38  z              (39 dialogue confirms)
house-exit-approach Up+Right 0.85s
house-exit       z
```

Replaying it is open-loop, and this session replayed **475 of the 934 slices**
(the previous pass replayed 60) with the field never leaving 14. A 4.8 s blind
stride cannot self-correct: any difference in start position, camera angle or
frame pacing compounds, and every later slice is then applied from the wrong
place. The recording is a record of what a human pressed *while watching*, so
it is a good description of the route and a bad script for reproducing it.

The fix is closed-loop navigation, not a better recording. Note the route also
shows the exit is gated behind **Dan's 39-step conversation**, so a navigator
has to interact, not just walk.

## Tools added this session

| Path | What it does |
| --- | --- |
| `src/field/scripts/virtual_machine.c` | `XENO_VM_TRACE_REPEAT` / `_MAX` / `_FIELD`, sequence numbers |
| `scratchpad/vm_trace_annotate.py` | offline opcode -> handler-symbol annotation, `--loop` cycle report |
| `pc_port/src/field_pos_diag.c` | ZONEDUMP (zone quads) + ACTORDUMP (actor positions) |
| `src/field/effects/distortion.c`, `src/field/main/misc.c` | `XENO_DISTORTION_DIAG` lifecycle counters |
| `scratchpad/field14_walk.py` | free-control probe: park-opcode check + one stride per direction |
| `scratchpad/field_navigate.py` | closed-loop navigator; zone / actor / patrol modes |
| `pc_port/src/field_warp_diag.c` | `XENO_FIELD_WARP` one-shot debug teleport (writes state) |
| `scratchpad/field14_warp_probe.py` | the zone-works probe above |

## What is actually left

1. **Navigate field 14 on foot** — closed-loop, with interaction: reach the
   stairs, talk to Dan (39 confirms), then the door. Distance hill-climbing
   alone gets trapped in local pockets; a coverage/patrol pass that reaches
   walls and corners is the next thing to try, and the ZONEDUMP/ACTORDUMP
   telemetry now makes the targets visible.
2. `XENO_FIELD_WARP` makes every later field reachable for testing **now**,
   without solving (1) first — so the rest of the Lahan chapter can be swept
   for blockers in parallel.
3. Unrelated, still open and now filed in `OPEN_ISSUES.md`: the title screen's
   default cursor is **Continue**, which enters the unported `func_801D9F98`
   and hangs; and F8 at the title SIGSEGVs.

## Build environment correction

`localhost/xenogears-dev-toolchain:current` **no longer builds the port**:
`pc_port/build_port.sh:84` now requires OpenSSL 3 dev files for verified KROM
loading and that image has no `/usr/include/openssl`. Use
`localhost/xenogears-dev-toolchain:krom-20260913`.
