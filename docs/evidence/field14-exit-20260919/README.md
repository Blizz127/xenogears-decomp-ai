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

The open question is no longer "what clears it" but **why some runs end with
the lock still applied** -- an actor-11 FE54 not matched by its FE53, or an
ordering/timing difference between runs. That is a much smaller question than
where this session started.

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

1. **Find out why some runs end with field 14's control lock still applied.**
   The scene cycles it (actor 11 runs FE54/FE53 four times each; actor 1 clears
   it at ip=404 before parking on free control), and a run that ends locked has
   `g_FieldControl != 0`, so OP_UPDATE_CHARACTER parks the player idle and
   never reaches `D_800ADB68 = 1`. Trace all actors
   (`XENO_VM_TRACE=a XENO_VM_TRACE_REPEAT=1 XENO_VM_TRACE_FIELD=14`) on a
   walking run and a frozen one and diff the FE54/FE53 pairing.
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
