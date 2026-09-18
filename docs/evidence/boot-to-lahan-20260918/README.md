# Boot → Lahan restored: two root causes found and fixed (2026-09-18)

Status: **runtime-verified** on this machine (Xvfb + scripted pad input + live
dpad key input, ordinary keys only, no game-state writes). The matching build is
unaffected: `disc/battle.bin` is still byte-exact and the overlay gates stay
green (see Verifications).

## Symptom that started this

The port built from this working tree SIGSEGV'd **on the title screen** as soon
as input advanced it:

```
Thread 1 received signal SIGSEGV, Segmentation fault.
#0 func_800399D4 (manager=0xdac0025f870003) at src/slus_006.64/system/sound.c:2011
#1 func_80078D44 ()                        at src/field/main/misc4.c:208
#2 FieldMain ()                            at src/field/main/main.c:499
```

It was **not** caused by battle-overlay leaf adoption: the crash reproduces with
1, 13, 18, 19, 24 and 92 adopted leaves, and with the committed 13-leaf
allowlist; only the 2026-09-11 pre-existing binary `xeno-port.pre-100857` booted
(its link layout happened to put padding under the overflowing buffer). Both host
gcc 16 and the container's gcc 13 reproduce it.

## Root cause 1 — boot archive read floods 32 KiB through 32-byte stubs

`pc_port/src/port_main.c:1388` calls `ArchiveInit(D_80010004, D_80018004, 0)` so
the port reads the retail archive index/header from disc instead of relying on
them being baked into the EXE. Both destinations were generated *data stubs*
sized `0x20` (`gen_port_stubs.py`'s 16-byte floor: neither the ELF nor any
symbol_addrs file gave them a size), while the read that fills each of them is
**32768 bytes** (`pc_port/src/archive_port.c:574` →
`pc_port/extern/PsyCross/src/psx/LIBCD.C:607` `CdReadSync`).

So the boot read overwrote 32 KiB of neighbouring port globals starting at
`D_80010004` (`0x9ff380` in the failing link). That span covered
`D_8004F304` (`0x9ff7a0`) and `D_80062528` (`0xa00fe0`), which is why the field
teardown `func_80078D44` then saw `D_8004F304 != 0` and called
`func_800399D4(D_80062528)` with a CD-data word.

Proof: a hardware watchpoint on `*(long*)&D_80062528` first trips inside
`ArchiveInit`'s `memmove`; retail sizes the same region as table `0x8000` at
`0x80010004` plus header at `0x80018004` inside rodata span `[0x80010000,0x80019524)`.

**Fix:** `config/symbol_addrs.port_buffers.txt` (new, port-only) annotates
`D_80010004 size:0x8000` and `D_80018004 size:0x1520`; `pc_port/build_port.sh`
passes it to `gen_port_stubs.py --symbol-addrs` alongside the matching configs.
The matching build never reads that file, so `config/symbol_addrs.slus_006.64.txt`
and the retail artifacts are untouched. After the fix the two buffers are
`0x10000`/`0x2a40` in the port and the 0x8000 read can no longer reach
`D_8004F304`/`D_80062528`.

## Root cause 2 — a retail library call could not be resolved

With boot fixed, the first battle started and died 19 instructions in:

```
[xeno-port][battle-mips] retail adapter ready: functions=101 shared-data=701
[xeno-port][battle-mips] enter retail battle.bin at 0x80070f40
[xeno-port][battle-mips] unresolved native call target=0x80032498 guest-pc=0x8008abd4
[xeno-port][battle-mips] stopped after 19 instructions: unresolved call target 0x80032498
```

`0x80032498` is `HeapChangeCurrentUser` (config/symbol_addrs.slus_006.64.txt:171).
The port **does** implement it (`nm xeno-port` → `T HeapChangeCurrentUser`), but
`tools/scripts/gen_battle_bridge_map.py` classified the address as *data*: it
only treated a name as a function when the matching ELF had a FUNC symbol (this
is un-decompiled PsyQ library code, so it has none) or for a tiny hardcoded name
set. The interpreter therefore refused to run the call.

**Fix:** the generator now also treats a name as a function when
(a) the port binary itself defines it as FUNC (`--host-elf`, passed by
`build_port.sh` when `$OUT/xeno-port` exists — a name the port can bind), or
(b) it is in the PsyQ heap family (`Heap*`, verified to be function code in this
EXE). Effect: the adapter's function count rises 101 → 590; a `jal` into the
retail library range now resolves to the port's implementation instead of
stopping the battle.

## Runtime verification (this machine)

Harness: `Xvfb :8x -screen 0 1280x960x24`, `SDL_VIDEODRIVER=x11`, the recorded
title-smoke pad schedule (`scratchpad/lahan-natural-visible.A56D5m/schedule.txt`)
for the boot route, plus live `xdotool` keys afterwards
(`scratchpad/lahan_drive2.py`: Z=Circle, C=Cross, arrows=d-pad). Logs and
screenshots under `/var/tmp/xeno-leaf-push/lahan-r*`.

Verified route (run `r3`, 15:07 → 15:18 wall clock, ordinary input only):

| step | marker | observed |
|---|---|---|
| title | `FieldLoad begin field=490` + `title loop enter choice=1` | 15:07:08 |
| New Game | `title confirm choice=2 keep=0` | 15:07 |
| prologue | `FieldLoad begin field=4` | 15:08:08 |
| **Lahan** | `FieldLoad begin field=2` | 15:09:59 |
| battle | `enter retail battle.bin at 0x80070f40` — renders and runs: gear models, Fei portrait, battle text; advances with Circle + Cross | 15:10:25 |
| **battle completes** | `retail battle returned` | **15:18:46** |
| after | `FieldLoad begin field=14` — the painting room renders (easel, canvases, stove, rug) | 15:18:46 → |

Reproduced twice independently (`r2` and `r3` both took
field490 → field4 → field2 → battle).

This is *better* than the documented reference: `lahan-natural-visible` (the
TITLE SMOKE PASS) took the same route and stopped **inside** that battle
(`enter retail battle` ×1, no return); this run completes the battle and returns
to field 14 — the state the earlier `opening-battle-encounter` evidence calls
"opening battle completed ... returned field14".

### Harness trap found while verifying (not a port bug)

The battle first looked hung: the screen stayed byte-identical for 60 s, every
gdb sample sat in `runtime_bridge_call` called from the interpreted battle, and
the guest PC never left a tight loop. That loop is `func_8008A3EC`
(`asm/battle/nonmatchings/main43/`), the **pause handler** — it calls
`SoundMuteAllSpuChannels` / `GraphicsDrawPauseLetters` and spins on
`ControllerPopState` (loop head `0x8008A4E8`, back edge
`bnez $v0, .L8008A4E8` at `0x8008A660`). Cause: the driver pressed **Start**
every 8 s during its settle phase; Start opens that handler and only Start
closes it (Circle/Cross do not). Pressing Start once by hand resumed the game
and the battle then completed. `scratchpad/lahan_drive2.py` no longer sends
Start, with the reason recorded inline.

## Verifications (gates)

| gate | result |
|---|---|
| `pc_port/build_port.sh` (toolchain container `krom-20260913`) | LINK OK, 82 stubs, 0 collisions, "92 adopted leaves reach no generated stub" |
| overlay leaf bridge regression | PASS 40 × O0/O2/UBSan + always-interpret control |
| differential prover | PASS (O0/O2/UBSan + wrong-result control rejected) |
| matching `battle.bin` | byte-exact `1830b4ef1fe37129972fc310dfad534f8161d6c0b123e74254c3711334a3e291` |

## Known blockers on the way to the end of Lahan

1. **Field 14 (painting room) needs scripted navigation.** The generic driver
   (Circle + rotating d-pad) parks there: the room wants the documented opening
   slices (talk to the painting/Dan, then leave) that the 2026-09-08 natural runs
   drove by hand. Nothing crashes; the run just stops making progress.
2. **Title CONTINUE hangs.** Choosing CONTINUE on the title screen enters
   `func_801D9F98` (`src/menu/main/misc.c:6826`, still `INCLUDE_ASM`); the port
   stubs it, and the menu then spins without reaching the Vsync shim (the pad
   frame counter stops). Selecting *New Game* is unaffected. Only reachable by
   pressing Up at the title, which the recorded battle schedule does at frame
   2800 (the reference schedule presses it at 3200 and lands on New Game).
3. `pc_port/src/world_map_init.c:4077` (worldmap third-wave diagnostic) increments
   the retail counter `D_8004F304` by hand, which is the same flag the field
   teardown tests — a second landmine on this path.
4. Long-route pacing: the pad schedule caps at 4096 steps
   (`PAD_TEST_INPUT_MAX_STEPS`), so past ~Lahan the route needs live input
   (`lahan_drive2.py`) or a raised cap.

## Push from field 14 (the painting room): what works, what blocks

After the route above, work continued from the painting room with a faster loop:

- `scratchpad/seed_field14.py` boots once, drives the battle with Circle+Cross,
  and saves a quick-save checkpoint in field 14 (`/var/tmp/.../lahan14.xgqs`).
  The save only commits when the field's own safety gate opens, so the script
  clears the aftermath dialogue first and polls for the file.
- `scratchpad/explore.py` is meant to boot, quick-load that checkpoint (F8) and
  sweep the field with `XENO_FIELD_POS_DIAG=40` telemetry (map, player position,
  trigger zones).

Findings from that work:

1. **The title menu defaults to Continue and Continue hangs.** `func_801C58EC`
   (src/menu/main/misc.c) wraps choice 0..2: choice 0 = options, choice 1 =
   Continue, choice 2 = New Game, and the loop enters at choice 1. Circle alone
   therefore confirms **Continue**, which enters `func_801D9F98` (still
   `INCLUDE_ASM`, stubbed) and the menu then spins without reaching the Vsync
   shim. New Game needs **Up then Circle** - exactly what the recorded schedule
   does at frames 3200/3300. A real fix is decompiling/shimming func_801D9F98;
   until then, any player who presses Circle at the title hangs the port.
2. **Walk strides matter.** 0.5 s d-pad taps only nudge the actor, and a rotating
   direction sweep cancels itself out. The driver now takes a configurable long
   stride (`XENO_DRIVE_WALK_HOLD`, default 3.5 s) and one direction at a time.
3. **The quick-load gate needs free player control.** `checkpoint_is_safe()`
   requires `D_800ADB68 == 1`, `D_800ADB64 == 0xFF`, `D_800B21D0 == 0` and no
   script control lock on the player actor (`status & 0x1800 == 0`), and the
   commit happens on the field exit with code 4. Queued loads sat in
   "waiting for free field control" for as long as the scene held the player, so
   F8 cannot replace a locked scene with the checkpoint.
4. **Field 14 currently holds the player.** After the first battle returns to
   field 14 the room renders (easel, canvases, stove, rug, door) and Fei is
   drawn, but no d-pad direction (each tried for 15-25 s, including the
   diagonals, with Circle taps) moves him or fires a zone transition: the
   aftermath scene keeps the player script-locked. The 2026-09-08 natural runs
   hit the same point and switched to hand-driven slices there ("Automatic input
   stops at painting14" in scratchpad/lahan-natural-20260908-compmatrix), so
   leaving the room needs either those slices replayed or the scene's exit
   condition decompiled. Everything before it is verified working above.
