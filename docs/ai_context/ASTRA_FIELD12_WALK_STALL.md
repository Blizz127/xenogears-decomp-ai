# Astra task: Lahan stops in field 12 — a scripted walk produces no movement

**Status:** root cause narrowed to one function and one suspect line. Not fixed.
**Branch:** `experiment/worldmap-open-gates-20260823` (see commits `15bf8e85`..`a30af79f`).

## The bug

Playing the Lahan chapter, the run reaches **field 12** (Alice's house). Alice
says her line, Fei turns, and then **Fei walks in place at the top of the
stairs and never enters the room**. The scene waits on him forever. The game is
*not* hung — it renders normally throughout.

## Exact repro (~10 min, needs a real display)

```bash
DISPLAY=:0 SDL_VIDEODRIVER=x11 XENO_FIELD_TEST=1 XENO_KERNEL_SEL=0 \
  XENO_FIELD_MAP=14 XENO_FIELD_ENTRANCE=0 \
  XENO_MOVE_DIAG=1 XENO_FIELD_POS_DIAG=30 \
  ./pc_port/build_native/xeno-port > /var/tmp/replay.log 2>&1
```

Then play: field **14 → 13 → 1 → 11 → 12**, and trigger the Alice scene.
`scratchpad/stall_watch.py`-style polling on the log will catch it; the stall
signature is a run of `moved=0` walk ticks at one position.

It does **not** reproduce on a cold direct boot of field 12 — it needs the real
story state (`scenario=7`, post-Alice).

## The measured causal chain

Every step below is from the live stalled process, not inference.

**1. The script is waiting on an arrival that never happens.**
Player is actor 1, parked at IP 141 on script bytes `4a 46 00` →
opcode **0x4A = `func_80099980`** (`src/field/main/misc7.c:987`). That opcode
advances the IP only when `func_80099AC0(0xFFFF)` returns 0.

**2. `func_80099AC0` keeps returning −1 ("still approaching").**
From `XENO_MOVE_DIAG=1`:

```
tick#17591 actor=1 self=(29,135) target=(102,106) d=78 step=11 countdown=65535 moved=1
tick#17611 actor=1 self=(29,135) target=(102,106) d=78 step=11 countdown=65535 moved=0
tick#17641 ... moved=0     (eight consecutive ticks, self never changes)
tick#17791 actor=1 self=(29,135) target=(70,25)  d=117 step=11 countdown=65535 moved=0
```

`d=78` vs `step=11` — this is not a near-miss threshold problem. He is asked to
walk 78 units and moves 0.

Note `countdown=65535` never decrements: `func_80099980` re-runs every frame
and **resets `slot->flags_0 = 0xFFFF` each time**, so the 65535-tick timeout in
`func_80099AC0` can never fire. The hang is genuinely infinite.

**3. No movement vector is ever produced.** Live read of actor 1's `ActorData`
while stalled:

```
+0x00 scriptFlags.flags = 0x00024430
+0x04 flags             = 0x04010400
+0x20/24/28 position    = 1965027 / 983040 / 8892711   (= (29,15,135) >>16)
+0x30/34/38 moveModified= 0 / 0 / 0        <-- ZERO
+0x40/44/48 move        = 0 / 0 / 0        <-- ZERO
+0x14 walkmesh material = 0x00000000       <-- ZERO
```

So collision is **not** cancelling a move; the mover never generates one.

**4. Why: `func_80082620` gates the move on `actorData+0x14`.**
(`src/field/main/misc8.c:1119`, byte-matched.)

```c
if (((flags4 >> shift) & 1) == 0) {
    if (D_800B21CC == 0) moveFlags = *(u32*)(actorData + 0x14);
}
...
addTest = moveFlags & 0x00004000;   /* or 0x8000 on the other path */
```

With `+0x14 == 0`, `moveFlags` is 0, both `addTest` gates are 0, and the
move-vector add never happens → zero translation.

**5. `+0x14` is written from `func_80080968`.**
`*(s32*)(actorData + 0x14) = func_80080968(actorData);`
(`src/field/main/misc8.c:277` and `:430`; also `misc6.c:337` as
`curWalkmeshTriMaterial`.)

## THE SUSPECT

`func_80080968` (`src/field/main/misc8.c:59`, **byte-matched**) returns 0 on
exactly three paths. One of them is a **port-only divergence the code itself
flags**:

```c
/* Native fail-closed guard. Retail assumes the relocated table owner
 * is valid here; retaining this branch is an audited port divergence. */
if (tableRow == NULL) return 0;          /* tableRow = D_800AFB24[stateIdx] */
```

The other two are retail:
```c
if ((gateWord >> (stateIdx + 3)) & 1) return 0;   /* gateWord = actorData+0x04 */
```
and the table lookup itself yielding 0.

**First thing to determine:** which of the three fires. For the stalled actor,
`actorData+0x04 = 0x04010400` (bits 10, 16, 26 set), so the retail gate returns
0 iff `stateIdx` (`*(s16*)(actorData+0x10)`) is 7, 13 or 23. Read `stateIdx`
and `D_800AFB24[stateIdx]` live and the answer is immediate. I lost the process
before capturing those two values — they are the single highest-value next
measurement.

If it is the `tableRow == NULL` guard, then `D_800AFB24` is not being populated
by the port and that is the real defect.

## Already ruled out — please do not redo these

- **"The player has no floor" (`hasTarget=0`, `selectedY=0x7FFFFFFF`).** Map 1,
  which walks perfectly, reports identical values on every sample. That search
  is about ride/talk targets, not ground.
- **"Field 14's actor 10 is stuck walking in place."** It pauses briefly and is
  walking fine by tick 932 (d 24→18→14). 413/430 of its ticks moved; map 1's
  11894 ticks all moved. Scripted walking is healthy on cold boots of both maps.
- **The model→actor binding rule.** Models are withheld from actors whose
  status has bit `0x40`; verified consistent against map 1.
- **`func_8002C644: skip invalid pin` warnings.** Map 1 emits the same four,
  on its own floor meshes, and walks fine.
- **A vblank/frame-pacing deadlock.** The serviced counter advances normally
  and the screen keeps animating; POSDIAG merely early-returns during scenes.
- **"Field 14 is sealed."** A hand-played run crossed it and left via field 13.

## Constraints

- `func_80080968`, `func_80082620`, `func_80099AC0`, `func_80084158` and
  `func_8009E10C` are **byte-matched retail**. Do not change their logic. Any
  port-side change belongs inside `#ifdef XENO_PC_PORT`, and a mechanical check
  that no added line escapes a guard is worth running (I used a small
  preprocessor-nesting script).
- Build: `localhost/xenogears-dev-toolchain:krom-20260913` (the `:current` tag
  no longer builds — the KROM gate needs OpenSSL 3). Run podman with the
  sandbox disabled.

## Acceptance

Fei completes the scripted walk in field 12 and the Alice scene advances, with
`XENO_MOVE_DIAG=1` showing `d` shrinking to below `step` and the opcode-0x4A
wait releasing. The port build stays green and no byte-matched function's
output changes.

## Tools available

`XENO_MOVE_DIAG` (walk ticks + interaction target), `XENO_FIELD_POS_DIAG`
(position, held buttons, `canRun`, ZONEDUMP, ACTORDUMP with model/mesh/flags),
`XENO_VM_TRACE[_REPEAT|_MAX|_FIELD]` + `scratchpad/vm_trace_annotate.py`,
`XENO_FIELD_WARP` / `XENO_FIELD_WARP_PROBE` (writes state — diagnostic runs
only, and note teleporting breaks movement so it cannot be used to test
walkability), `XENO_PAD_TEST_STOP_FIELD`, `scratchpad/xeno_control.py`
(gate drivers on `canRun=1`, never on a clock).
