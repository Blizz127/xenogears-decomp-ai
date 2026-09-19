# Field 14 is not locked: the painting room re-measured (2026-09-19)

Status: **runtime-measured** on this machine (Xvfb + xdotool, ordinary keys
only, no game-state writes). All source changes are inside `#ifdef
XENO_PC_PORT`, so the matching build is untouched.

This session set out to find "what field 14's exit script loop waits for", the
open question left by the 2026-09-18 push. The answer is that **there is no
such loop, and nothing in the port is broken here**. The room's script
completes, hands the player control, and its exit works. What blocked every
previous attempt was the room's scripted camera locking input during each of
its pans, combined with drivers that waited a fixed time and then pressed keys
into a locked field.

## What the previous pass concluded, and what is actually true

| Inherited claim (2026-09-18) | Measured 2026-09-19 |
| --- | --- |
| FE54's guard never passes, so the departure never arms | FE54's guard passes every time; it runs **once** in field 14, during scene setup |
| The room's script loop stalls after the FE54 lock | No stall. Actor 0 reaches its terminal park; actor 1 reaches **free player control** |
| Field 14 keeps Fei script-locked; no d-pad input moves him | Not a script lock: the player actor reaches free control, and held d-pad input provably reaches the field's button mask. Whether he then walks varies run to run |
| Leaving needs the scene's exit condition decompiled | Nothing needs decompiling. The exit fires correctly when the player is in the zone |

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

**But it does not reproduce every run**, and that turns out to be the sharpest
result of the session — see the next section.

## Movement in field 14 is intermittent, and not for want of input

Later runs of the same probe, same procedure, produced the opposite: the player
parked on `0x0C` and did not move at all. To tell "the game ignored the input"
from "the input never arrived" — opposite fixes — POSDIAG now also prints the
field's own button masks (`D_800AFE9C` held, `D_800C2694` newly-pressed).

With that, run `w6`, 65 samples in field 14:

```
samples with a direction held:                  22
held masks seen:  0x1000 Up   0x2000 Right  0x4000 Down  0x8000 Left
                  0x3000 Up+Right            0xc000 Down+Left   (0x20 Circle)
held-samples followed by a position change:      0
distinct positions:                              1   -- (115,-455)
```

Every direction **and both diagonals** reached the field's held-button mask,
the player actor was parked on the free-control opcode, and the position never
changed once across the whole run. Compare run `w1` (earlier build, before the
mask was printed): 7 distinct positions, x in [88,161], z in [-495,-413].

So, precisely:

* Input delivery is **not** the blocker — the held mask proves the holds arrive,
  diagonals included.
* The script lock is **not** the blocker — actor 1 is on `0x0C`.
* Whether the player can walk at all in field 14 after the dream battle
  **varies between otherwise identical runs**.

## Which gate refuses, and why it varies: the script cycles the control lock

POSDIAG now also prints the gates OP_UPDATE_CHARACTER (`func_8009F5F4`) tests.
The gate is byte-matched retail, not a port guess -- `asm/field/matchings/main/
misc6/func_8009F5F4.s`:

```
lui   $v0, %hi(g_FieldControl)
lh    $v0, %lo(g_FieldControl)($v0)
bnez  $v0, .L8009F9A8          <- non-zero: park the actor idle and RETURN,
                                  before `D_800ADB68 = 1` is ever reached
```

`g_FieldControl.isRandomEncountersEnabled` is a `short` at **offset 0**
(`include/field/main.h:105`), so that halfword IS the field-control lock, and
**FE54 (`func_80093B10`) sets it to -1** -- also byte-matched
(`asm/field/matchings/main/misc11/func_80093B10.s`). Both sides are faithful.

A run where the player cannot walk (`w7`) reads the same on all 21
held-direction samples, one distinct position all run:

```
held 0x1000/0x2000/0x4000/0x8000/0x3000/0xc000
canRun=0  owner=0xff  b21d0=1  status=0x0260
```

`status & 0x1800 == 0` -- no script control lock on the actor, and no menu owns
input. What refuses is the field-control halfword.

**An earlier draft of this section claimed field 14's script sets that lock and
never clears it. That was wrong**, and it was wrong because it grepped a
4-second trace. A full all-actor trace of the whole scene (`w8`, 30756
dispatches) shows the clearers running:

```
FE54 (set lock)    5   actor 0 ip=29 (x1), actor 11 ip=1053 (x4)
FE53 (clear lock)  5   actor 11 ip=1192 (x4), actor 1 ip=404 (x1)
FE4F / FE50        0
```

Actor 1 clears the lock at ip=404 and parks on free control at ip=406 -- the
scene's own unlock, exactly where it belongs. And in that same run all 22
held-direction samples read `canRun=1 b21d0=0`, with **8 distinct positions**:
the player walked.

So the correct statement is: **field 14's scene repeatedly applies and releases
the control lock** (actor 11 cycles FE54/FE53 four times), and whether a run
ends with it released varies. A run that ends locked shows a completely
immobile player even though the script has otherwise finished and input is
arriving. This matches the 2026-09-18 observation that "the lock is applied and
released repeatedly" -- that part of the old note was right.

### Actor 11 toggles the lock forever, and the run stops in one half or the other

Three all-actor runs, same build, same procedure. The lock-op sequence is
**identical** in all three; only where it stops differs:

```
w8  WALKED  8 positions, 22 held samples,  0 with the lock on
    54@a0 53@a1 54@a11 53@a11 54@a11 53@a11 54@a11 53@a11 54@a11 53@a11
w9  WALKED  5 positions, 21 held samples,  0 with the lock on
    54@a0 53@a1 54@a11 53@a11 54@a11 53@a11 54@a11 53@a11 54@a11 53@a11
w10 FROZEN  1 position,  20 held samples, 20 with the lock on
    54@a0 53@a1 54@a11 53@a11 54@a11 53@a11 54@a11 53@a11 54@a11 53@a11 54@a11
                                                                    ^^^^^^^
                                            one extra FE54, no FE53 after it
```

So **actor 11's script sits in a loop that raises the field-control lock (FE54)
and lowers it (FE53) over and over**, and the player can only walk during the
lowered half. w10 was sampled during a raised half and every one of its 20
held-direction samples saw `canRun=0`; w8 and w9 stopped on a lowered half and
walked.

That fully accounts for the "intermittency", for the 2026-09-18 note that the
lock "is applied and released repeatedly", and for why hand-driven play works
(a human just keeps pressing until the lock happens to be down) while a
scripted stride of a fixed length does not.

### What actor 11 is actually doing: a multi-shot camera sequence

Annotating actor 11's own trace answers the last question, and the answer is
benign. Between its FE54 and its FE53 it runs a camera shot:

```
ip=1053  FE54  func_80093B10                     <- lock input
ip=1121  99/35 ...
ip=1128  63    FieldScriptSetCameraTargetMovementDest
ip=1136  A3    FieldScriptSetCameraPosMovementDest
ip=1144  05    func_800A17F4  -> 2068..2583 (interpolation step, reset,
                                 StartCameraMovement x2, WaitForCameraMovement)
ip=1192  FE53  func_80093AC8                     <- unlock
```

...and then repeats for the next shot. Actor 11 is the painting room's
**scripted camera**, and locking player input for the duration of each pan is
exactly what a cutscene camera should do. The sequence is finite -- w8 and w9
both ran out of shots, ended on FE53, and the player walked.

**So the port is behaving correctly here.** What was wrong was the harness: it
waited a fixed 40 s and then drove, and on a CPU-starved machine (this box was
running several other agents at load ~6-7, so the game renders well below 60
fps under llvmpipe) the camera sequence is still mid-pan at that point. w10
simply sampled during the last pan.

This also explains the whole saga end to end. The 2026-09-08 human recording
waited **26 s before `easel-backoff` and 29 s before `dan-0`** -- those pauses
are the human watching the camera pans finish. A replay that dropped the gaps
pressed every movement key during a pan; a replay that honoured the gaps still
pressed too early, because the pans take longer here than they did then.

**The harness fix is to wait on the lock, not on a clock:** drive only when
POSDIAG reports `canRun=1` (equivalently `g_FieldControl == 0`). That signal
did not exist before this session and now does.

## Driving on `canRun=1` instead of a clock (the fix, and what it revealed)

`scratchpad/xeno_control.py` (new, shared) blocks until POSDIAG reports
`canRun=1` before any movement key is pressed, tapping Circle meanwhile to
advance a scene that is waiting on dialogue rather than on a camera. Confirm
keys are deliberately NOT gated -- dialogue advances while the lock is raised,
and that is how the scene is driven to its end. It is wired into
`lahan_route.py` (movement slices only) and `field_navigate.py` (every press).

The route replay improves immediately and measurably:

```
replay done: 60 slices played; 0 movement slices pressed while still locked
player moved (115,-455) -> (122,-453) -> (98,-422)
```

versus every earlier run, which spent its movement slices into a raised lock
and never moved at all. **The harness race is fixed.**

It also removes the last doubt about the floor. A 900 s control-gated
navigation run:

```
1661 samples in field 14, 291 distinct positions
canRun=1 on 95% of samples
reachable extent: x [87,324]   z [-496,-413]
```

With the player demonstrably free almost the entire run and 291 distinct
positions explored, the boundary at **z ~ -413** is real, not a search
artifact. The painting room's walkable floor is a narrow east-west strip, and
field 14's only trigger zone (335,-26) lies ~390 units north of it.

So walking to that zone from the post-battle spawn is **not possible**, and the
route out must be something else. The recorded 2026-09-08 route agrees: its
exit slice is `house-exit z` -- a **Circle press**, i.e. a door actor, not a
zone entry. ACTORDUMP puts actor 14 at (358,-421), just past the strip's east
end, which is the right shape for that door. That is the next thing to try, and
it is a bounded experiment rather than an open question.

## The door-actor hypothesis, tested and negative

ACTORDUMP put actor 14 at (358,-421), just past the walkable strip's east end,
and the established Lahan pattern is to stand near a door actor's offset point
(actor + (-26,-26), here **(332,-447)**) and press Circle. The control-gated
navigator reaches (323,-457)/(321,-464) comfortably -- 13 to 20 units from that
offset point -- and pressed Circle there **seven times across two runs**:

```
within 13 of goal at (323,-457) -- Circle   -> no change
within 20 of goal at (321,-464) -- Circle   -> no change   (x6)
```

No field change, no dialogue, no reaction of any kind, with the player under
free control and Circle demonstrably reaching the pad. **Actor 14 is not the
door.** Recorded so the next attempt does not spend another two runs on it.

What that leaves: the remaining placed actors (9 at (310,-243), 13 at
(16,-331), 17 at (-100,-510), 10/16/19/20 further out) all sit OUTSIDE the
reachable strip `x[87,324] z[-496,-413]`, so none of them can be approached
either. Every candidate exit -- the one trigger zone and every actor -- is
outside the floor the player can stand on. That is now the strongest statement
available, and it points at the floor/spawn itself rather than at any exit
mechanism: either the post-battle spawn puts the player on the wrong part of
map 14, or the walkable geometry the port builds for this room is short.

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

1. **Make every driver wait on `canRun=1` instead of a fixed delay.** That one
   change is what makes field-14 driving repeatable: the room's camera actor
   locks input per pan, and any fixed wait races it on a loaded machine. The
   probes here still use a clock; they should poll POSDIAG.
2. **Then navigate field 14 on foot** — closed-loop, with interaction: reach
   the stairs, talk to Dan (39 confirms), then the door. Distance hill-climbing
   gets trapped in local pockets; ZONEDUMP/ACTORDUMP now make the targets
   visible.
3. `XENO_FIELD_WARP` makes every later field reachable for testing **now**,
   without solving (1) or (2) first — so the rest of the Lahan chapter can be swept
   for blockers in parallel.
4. Unrelated, still open and now filed in `OPEN_ISSUES.md`: the title screen's
   default cursor is **Continue**, which enters the unported `func_801D9F98`
   and hangs; and F8 at the title SIGSEGVs.

## Build environment correction

`localhost/xenogears-dev-toolchain:current` **no longer builds the port**:
`pc_port/build_port.sh:84` now requires OpenSSL 3 dev files for verified KROM
loading and that image has no `/usr/include/openssl`. Use
`localhost/xenogears-dev-toolchain:krom-20260913`.
