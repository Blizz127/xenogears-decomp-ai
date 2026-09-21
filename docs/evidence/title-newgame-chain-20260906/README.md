# Title -> New Game boot chain: not a code regression; the title only answers Circle, 2026-09-06

- Branch: `experiment/worldmap-open-gates-20260823`, HEAD `3a3e7aac` plus the
  uncommitted working tree of 2026-09-06 (nothing committed here).
- Verdict: **CHAIN_WORKS_END_TO_END: boot -> movie -> Map 490 -> title menu ->
  New Game -> Map 4 (prologue) -> Map 2 (Lahan) -> Map 14, headless, driven at
  the raw pad layer, no crash.**  The reported "title menu never entered" was
  produced by pressing Cross / Start / Up on a map whose script, whose FE60
  transition body and whose menu reader all test **Circle (0x20)** only.  No
  retail-path code needed fixing; the retail `func_800A7C58` body that landed
  in the working tree after commit `46157084` behaves as the asm says it must.

Claims are tagged **proven** (retail asm / matched decomp / disc data),
**build-verified** (compiled, linked, executed here), **observed** (seen at
runtime in the port) or **inferred** (reasoned from source, not executed).

Environment: podman image `localhost/xenogears-dev-toolchain:current`,
`--userns=keep-id`, `TMPDIR=/var/tmp`, `SDL_VIDEODRIVER=offscreen`,
`disc/disc1.bin`.  Build: `./pc_port/build_port.sh` -> `LINK OK`, 74 function
stubs (unchanged count).  Unit certificate ran in the same image (the Bazzite
host has no `libubsan` target, exactly as `run_field_script_vm_core_handlers.sh`
notes).

## 1. What the title map actually does (proven from the trace + handlers)

`XENO_VM_TRACE=0` (new, `src/field/scripts/virtual_machine.c`) prints each
first-seen instruction pointer of actor 0, Map 490's only actor.  Opcode names
are the port's own handler tables (`pc_port/src/data_field.c`,
`g_FieldScriptVMHandlers` / `g_FieldScriptVMHandlers2`, which mirror retail
`jtbl` order; the FE60/FE61/FE57 slots are the ones the 2026-09-02 grind log
already used):

```text
-- poll loop (every pass while idle) --
ip 107  26 20 80          Sleep 0x20 frames          (FieldScriptVMHandlerSleep)
ip 110  C6                extend VM budget           (func_800A1E9C)
ip 111  FE 86 00          FE86                       (func_80089F94)
ip 114  31 20 00 7A 00    OP31: held & 0x0020 ? ip+5 (119) : jump 0x007A (122)
ip 122  3C 04 04          IncVariable (idle counter)
ip 125  02 04 04 C0 03..  ConditionalJmp on the counter -> 133 (attract) once it passes
ip 151  26 00 80          Sleep 0
ip 154  01 6E 00          Jmp 110

-- attract branch (no Circle for the idle budget) --
ip 133  37 04 04 / 136 FECD / 140 02 .. / 148 01 EF 00 (Jmp 239)
ip 239  FECD / 243 02 .. / 251 B4 10 80 / 254 26 10 80 (Sleep 0x10) / 257 FE86
ip 260  FE 60 0D 80 00 80 6E 91 82 80   FE60: STR 13, frames 0..4462, arg7 0x82 (type 2 | 0x80)
ip 270  FE 61                            FE61: wait for func_800A7C58's D_800ADB7C
ip 272  FE85 / 276 02 .. / 290 B3 10 80 / 293 Sleep / 296 Jmp 314 / 314 01 6E 00 (Jmp 110)

-- Circle branch (OP31 fell through) --
ip 119  01 9D 00          Jmp 157
ip 157  37 04 04 / 160 FE86 01 / 163 37 46 00 / 166 75 FF 80
ip 169  FE 57             FE57: D_800ADB64 = 2 (title menu), D_8004F350++
ip 171  FE 87             wait for D_8004F350 == 0 (func_800799D4 clears it after MenuMain)
ip 173  26 03 80 / 176 02 46 00 .. / 184 02 02 04 ..   branch on the menu result
ip 192  FE 60 0D 80 ..    FE60 again: the opening STR after New Game (Circle skips it)
ip 202  FE 61 / 204 01 DB 00 (Jmp 219) / 219 FE41 / 223 FE83 / 227 5B ..  -> Map 4
```

(Attract and Circle branches: `run2/vm-trace.actor0.log`, first-seen ips in
dispatch order; the FE60 operand bytes above are the trace's `args`, e.g.
`0d 80` = immediate 13.)

- **proven** -- OP31 is `func_800961A0` -> `FieldScriptVMCheckControllerInput(D_800AFE9C)`
  (`src/field/main/input_script_handlers.c:8-17`): `mask = arg(1); if (mask & held) ip += 5; else ip = arg(3)`.
  `D_800AFE9C` is the **held** accumulator of `FieldPollControllers`
  (`src/field/main/misc2.c:1439`).  The mask in the script is `0x0020` = Circle.
  Nothing in that loop reads Cross (0x40), Start (0x800) or the d-pad.
- **observed** -- run 1 dispatched the loop 283 times (`run1/vm-trace.actor0.head.log`)
  before the idle counter sent the script to FE60.
- **proven** -- FE60 is `func_8008EC30` (`src/field/main/misc.c`, FE slot 0x60):
  it stores the STR index/frame range, `D_800ADB80 = arg7 & 0xC0`, `D_800ADB70 = 1`.
  It does NOT run the transition itself: `FieldMain` (`src/field/main/main.c:652`)
  calls `func_800A7C58()` when `D_800ADB70 && g_FieldCurRenderContextIndex == 1`.
- **observed** -- the title's FE60 arguments, from the new `[xeno-port][fe60]`
  line: `str=13 start=0 end=4462 mode=0 flags=0x80 loop=0 rgb24=1 sysmode=1`.
  So the attract movie is dir `0x18/1` file `13+2 = 15`, **4462 frames long**
  (about five minutes at the STR's ~15 fps), type 2 (24-bit), Circle-interrupt
  bit 0x80 set.
- **proven** -- `func_800A7C58` (`src/field/main/misc5.c`, compared branch for
  branch against `asm/field/nonmatchings/main/misc5/func_800A7C58.s`,
  `800A7F78-800A80B0`): with `g_FieldSystemMode != 0` (CD-ROM, the retail
  value) the only ways out of its attract loop are
  `D_800ADB80 & 0x80` **and** `FieldPollControllers(); D_800C3900 & 0x20`
  (Circle pressed-once; `800A8028: andi $v0,$v0,0x20`), or the frame budget
  `D_800B06A0 >= D_800C3A2E` with `D_800C3A3A == 0` (`800A8088-800A80AC`).
  `D_800B06A0` is advanced by the STR module's per-frame callback
  `func_800A7120` (`pc_port/src/movie_player.c:1050` -> `misc5.c:860`).
- **proven** -- FE61 is `func_8008E9F8`: spins (`ip -= 1`) until
  `D_800ADB7C`, which `func_800A7C58` stores at `800A7E58` before its loop.
  FE57 is `func_800937E0` (`src/field/main/misc11.c`): `D_800ADB64 = 2`,
  `D_8004F350++`.  `FieldMain` (`main.c:656`) turns `D_800ADB64 != 0xFF` into
  `func_800799D4` -> `MenuMain` -> `MenuExecute` slot 2 -> `func_801C62A8` ->
  `func_801C58EC`.
- **proven** -- the title menu reader `func_801C7D78` is **byte-matched**
  (`asm/menu/matchings/main/misc/func_801C7D78.s`): nav codes come from
  `g_C1ButtonStatePressedOnce & 0x2000/0x4000/0x8000/0x1000` (-> 0/1/2/3),
  confirm is `g_C1ButtonStateReleased & 0x20` (-> 4, the Circle **release**
  edge), cancel `& 0x40`.  There is no `0x800` (Start) test at all.
  `func_801C58EC` (`asm/menu/nonmatchings/main/misc/func_801C58EC.s`
  `801C595C-801C5A28`: `ori 0x3` / `slti 0x4` / `sltiu 0x3` on `0x336`)
  wraps `menu1Choice` 0..2 with UP (3) = `++`, DOWN (1) = `--`; the default
  cursor is 1 (Continue); New Game is choice 2 -> `func_801C531C(7)` entry 9
  -> `func_8001B970`.

So, on retail and in the port alike: **Circle interrupts the attract STR,
Circle (held) leaves the polling loop, Circle release confirms in the menu, UP
moves Continue -> New Game.**  Cross / Start / Up cannot open the title.

## 2. Why it looked like a regression (inferred, with the mechanism proven)

- At HEAD-era commit `46157084` the port's `func_800A7C58` was a six-store
  shim that returned immediately, so FE61 released within a frame and the
  script sat in its Circle poll from the first seconds.  The 2026-09-02 chain
  was driven with **Circle** (grind log: "Circle (`PADRright`/`0x20`) -> FE57").
- The working tree now carries the complete retail body.  With no Circle, the
  idle counter fires FE60 after ~280 loop iterations, `func_800A7C58` starts
  the 4462-frame STR, and only Circle or ~5 minutes of playback can end it.
  Every logged attempt (`~/.cache/xeno-run/ng*/run.log`, 110 s windows,
  Cross/Start/Up only) ends inside that loop: the second
  `[Psy-X] CD: 'CdlReadS' at 54133` in those logs is the attract STR starting,
  and nothing follows because nothing pressed Circle.  **observed** (those
  logs) + **proven** (the exit conditions above).
- The one earlier report of "title hits `[stub] func_801D9F98`" is the
  Continue arm: confirming on the default cursor.  The port's screen stub
  returns 0, so `func_801C531C` keeps the title loop running (certificate case
  `title.confirm.default.opens.continue`, **build-verified**).

## 3. End-to-end run 1 (observed; `run1/`)

`pc_port/tests/run_title_newgame_smoke.sh`, binary built from this tree,
600 s wall clock, ~32 presented frames/s headless.  Schedule
(`run1/schedule.txt`, `XENO_PAD_TEST_INPUT`, frames are Vsync-shim calls):

```text
0:0, 300:0x40, 320:0                 Cross skips the state-6 opening STR
1600:0x20, 1602:0                    Circle interrupts the title attract STR (in func_800A7C58)
2400:0x20, 2402:0                    Circle: OP31 falls through -> FE57 -> title menu
2800:0x1000, 2802:0                  UP: Continue (1) -> New Game (2)
2900:0x20, 2902:0                    Circle release confirms -> func_8001B970
3400:0x20, 3402:0, 3404:0x20, ...    2-on/2-off Circle pulses to the 4096-step cap
```

Log order (`run1/run.trimmed.log`, line numbers of the full log):

| line | event |
| --- | --- |
| 50 | `FieldLoad begin field=490` |
| 2081 | `[fe60] enter str=13 start=0 end=4462 mode=0 flags=0x80 loop=0 rgb24=1 sysmode=1` |
| 2084 | `[fe60] first STR frame delivered (D_800ADB7C=1)` |
| 2092 | `[fe60] exit reason=circle frame=64 end=4462` (the 1600 press) |
| 2111 | pad frame 2400 Circle |
| 2113 | `func_800799D4 request=2 D_80059460=2 -> MenuMain` |
| 2115 | `func_801C58EC title loop enter choice=1 D_80059460=2` |
| 2132 | `title confirm choice=2 keep=0 D_800594D0=0` (after UP at 2800, Circle at 2900) |
| 2136 | `MenuMain returned D_800594D0=0`; 2140 `WAIT_MENU D_8004F350=0` |
| 2141 | `[fe60] enter str=13 ...` **again**: the New Game path replays the opening STR |
| 2157 | `[fe60] exit reason=circle frame=111` (first prologue pulse skips it) |
| 2186 | `[FieldMain] teardown exitCode=3` |
| 2200 | `FieldLoad begin field=4` (prologue) |
| 3967 | `FieldLoad begin field=2` (Lahan), pad frame ~6800, ~3.5 min |
| 4358 | `[FieldMain] teardown exitCode=0` |
| 5667 | `FieldLoad begin field=14` (gear scene) |

No `[stub]`, `missing D_8004FE50`, OT, malloc or signal markers in the log.
Captures (`captures/`, converted from the BMP payload with ImageMagick):
`run1-frame-002340-title-logo-field.png` (Map 490 before the press: the
Xenogears logo and "(c) 1998 SQUARE" render in the field),
`run1-frame-002520-title-menu.png` (New Game / Continue / Sound, cursor on
Continue -- the backdrop behind the menu is black: MenuMain's (704,256)
snapshot path, the known pre-existing gap, not the field),
`run1-frame-005400-prologue-map4.png` (narration), `run1-frame-006900-lahan-map2.png`
(burning village, Fei and Citan), `run1-frame-007800-lahan-gear.png`,
`run1-frame-009000-map14.png`.

Run 1's only failing smoke gate was `script.reaches.fe57`: the first version of
the VM trace stopped after 2000 distinct-ip lines, which the polling loop
exhausted before FE60.  The trace was changed to first-seen-per-ip (bitset,
reset on script buffer change) and the run repeated -- see section 6.

## 4. Regression test (build-verified)

`pc_port/tests/run_title_newgame_chain.sh` + `title_newgame_chain_prod_test.c`
link the shipped `src/field/main/misc.c` (FE60, FE61), `src/field/main/misc11.c`
(FE57) and `src/menu/main/misc.c` (`func_801C7D78`, `func_801C58EC`,
`func_801C531C`) with `--gc-sections`; only the title screen's draw/resource
leaves and the per-frame pump are `objcopy --weaken-symbol`ed and supplied by
the test (the `run_npc_event.sh` idiom).  The menu object is compiled with
`-fno-inline` so those leaves cannot be inlined before objcopy runs.
Operands use the retail immediate encoding (bit 0x8000, `misc9.c:735`).

`certificate.container.log`: O0 / O2 / UBSan outputs identical, then

```text
M1 TITLE_CHAIN_MUTANT_FE60_DROPS_CIRCLE_GATE  -> ASSERTION fe60.keeps.circle.interrupt.bit
M2 TITLE_CHAIN_MUTANT_FE61_NEVER_RELEASES     -> ASSERTION fe61.releases.on.flag
M3 TITLE_CHAIN_MUTANT_FE57_WRONG_MENU         -> ASSERTION fe57.requests.title.menu.2
M4 TITLE_CHAIN_MUTANT_CONFIRM_ON_PRESS        -> ASSERTION menu.confirm.not.on.circle.press
M5 TITLE_CHAIN_MUTANT_TITLE_UP_IS_DOWN        -> ASSERTION title.up.moves.continue.to.newgame
```

Every mutant **builds** (the runner refuses a control that produced no
binary) and fails at runtime with its named assertion
(`certificate.mutant-stderr.log`).  The mutant hooks live in the production
sources behind `TITLE_CHAIN_MUTANT_*`, like the existing `MENU_MUTANT_*` /
`FIELD_VM_AUDIT_MUTANT_*` hooks.

`pc_port/tests/run_title_newgame_smoke.sh` is the runtime smoke (section 3);
`XENO_TITLE_SMOKE_CONTROL=cross` swaps every Circle for Cross and passes only
if the positive chain is absent (section 5).

## 5. Negative control (observed; `control1/`)

Same binary (rebuilt with the first-seen trace), same schedule with Cross
(0x40) in place of every Circle, 200 s.  Result (`control1/summary.txt`):
`field490=46 fe60=99 fe60exit=0 request2=0 title=0 newgame=0` -- the script
polled, idled into FE60 (`[fe60] enter str=13 ...` at line 99), the attract
STR started and **never exited** in 200 s (no `exit reason=` line; that is the
state every Cross/Start/Up attempt was stuck in), no `request=2`, no title
loop.  All five control gates PASS, i.e. the positive assertions fail on this
input, so they are not vacuous.

## 6. Final run with the first-seen VM trace (observed; `run2/`)

Same binary, positive schedule, 480 s: **all ten smoke gates PASS**
(`run2/summary.txt`, `run2/run.trimmed.log`, `run2/vm-trace.actor0.log`):

| line | event |
| --- | --- |
| 50 | `FieldLoad begin field=490` |
| 117 / 120 / 128 | `[fe60] enter str=13 ... flags=0x80` / first frame / `exit reason=circle frame=64` |
| 159 | trace: `ip=169 op=FE57` (after `ip=119 op=01 args=9d 00`, the OP31 fall-through) |
| 161 / 163 | `request=2 -> MenuMain` / `title loop enter choice=1` |
| 180 | `title confirm choice=2 keep=0 D_800594D0=0` |
| 196 / 212 | second `[fe60] enter` (opening STR) / `exit reason=circle frame=111` |
| 245 / 259 | `teardown exitCode=3` / `FieldLoad begin field=4` |
| 2110 | `FieldLoad begin field=2` |
| 2593 / 3900 | `teardown exitCode=0` / `FieldLoad begin field=14` |

## 7. Source changes (all additive, none on the retail control flow)

- `src/field/main/misc5.c` `func_800A7C58`: three `printf` lines (enter
  parameters, first STR frame, exit reason) and an `exitReason` string set
  before each retail `break`.  `#include <stdio.h>` under `XENO_PC_PORT`.
- `src/field/scripts/virtual_machine.c`: `PcPort_FieldVmTrace` (env
  `XENO_VM_TRACE=<actor>|all`), one call before the handler dispatch, under
  `XENO_PC_PORT`.  No `<string.h>` (it conflicts with the game's `strlen`
  prototype in `system/memory.h`).
- `pc_port/src/psyq_compat.c`: `PAD_TEST_INPUT_MAX_STEPS` 256 -> 4096 (the
  prologue needs hundreds of Circle pulses; same cap as the field schedule).
- Mutant hooks: `src/field/main/misc.c` (`func_8008EC30`, `func_8008E9F8`),
  `src/field/main/misc11.c` (`func_800937E0`), `src/menu/main/misc.c`
  (`func_801C7D78`, `func_801C58EC`).
- New: `pc_port/tests/title_newgame_chain_prod_test.c`,
  `pc_port/tests/run_title_newgame_chain.sh`,
  `pc_port/tests/run_title_newgame_smoke.sh`.

## 8. Not verified / still open

- Retail's idle budget before the attract STR (the `3C`/`02` counter at ip
  122-125) was not decoded numerically; only its effect (283 loop passes,
  then FE60) was observed.
- The title backdrop is still black behind the menu (pre-existing; grind log
  2026-09-02 "Title backdrop"), and the attract STR's own frames were not
  visually checked here (captures every 60/120 Vsyncs mostly fell on the
  field frames around it).
- `func_801D9F98` (Continue / save-load screen) remains a generated stub;
  Continue is not part of this chain.
- The smoke's first prologue pulse is what skips the post-New-Game opening
  STR (retail lets Circle skip it); a "let it play" variant was not run.
- Whether physical Start does anything at retail's title was checked only
  against the matched reader (`func_801C7D78` has no 0x800 test) and the map
  script (OP31 mask 0x0020); no hardware run.
