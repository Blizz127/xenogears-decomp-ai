# W34N124 — natural on-foot world walk and location entry (Lahan Village, Mountain Path)

- Branch: `experiment/worldmap-open-gates-20260823`
- Starting HEAD: `8f397d19` (worldmap: restore vehicle controller state-2 slice)
- Date: 2026-09-01
- Verdict: **ON_FOOT_WALK_AND_ENTRY_VERIFIED_FOR_BOTH_LIST0_LOCATIONS**

## What was established

On the accepted natural route (Lahan exit 1 -> world session, cold new-game
state), the world player is **slot 1** (`wm_8008A72C`).  Slot 7
(`wm_8008E76C`) is the vehicle controller; its initializer `wm_8008E190`
returns scheduler state 3 (dormant) because the control halfword
`0x8006EE68 & 0x1FFF` is 0 on foot.

The on-foot controller reads the d-pad nibble of the world pad word
`0x8009CD4C` (bits 0x1000/0x2000/0x4000/0x8000, merged from the native
`g_C1ButtonState` by the frame driver, or from `XENO_WORLD_TEST_INPUT` in
harness runs) and walks Fei about 5.6 world units per frame.  The trigger
query `wm_80094238(pos, 0)` selects list-0 records; a circle (0x0020) rising
edge while a record is selected returns class 1 from `wm_80090A84`, which
clears `0x8009D554`/`0x8009D7CC` and lets the terminal-zero lane hand the
record's map/entrance to `FieldMain`.

## World-region data on this route

`wm_selector_producer(entrance=1, seed=0)` bins the scenario seed
(GameState `+0x1930`) against the threshold table at `0x8009B564`
(`0, 24, 54, 135, 149, 186, 198, 204, 237, 0xFFFF`) and selects record 1 of
`0x8009B57C` (archive base 43; second-wave files 44/45/46).  Its list 0 has
exactly two triggers:

| record | rect (x0, z0, w, h) | map | entrance | id |
|---:|---|---:|---:|---:|
| 0 | (30095, 10342, 253, 169) | 15 | 2 | 2 |
| 1 | (29792, 10572, 365, 354) | 1 | 9 | 1 |

Lists 1-3 are empty.  Record 1 is Lahan Village (map 1).  Record 0 is the
Mountain Path world entrance (map 15, the map Lahan's own zone-5 exit also
reaches via CHANGE_FIELD entrance 1).  Blackmoon Forest is not in this data
set; later scenario seeds select other archive bases (54, 65, ...).  The
port-only knob `XENO_WORLD_SCENARIO_SEED` (unset = retail) overrides the seed
so those sets can be probed without a save state.

## Harness

`pc_port/tests/run_w34n124_world_walk_entry.sh [lahan|mountain]` drives
`pc_port/tools/world_harness/w34n124_walk_entry.gdb`:

- `lahan`: 0x2000 for 30 world frames, circle at frame 40.  Fei walks from
  the arrival point (29947, -312, 11075) heading 0xE00 to (30116, -309,
  10905) heading 0x200, record id 1 is selected, the natural exit fires and
  `FieldMain` starts map 1.
- `mountain`: 0x2000 for 120 frames (Fei stops against the mountain wall at
  (30332, -316, 10496) inside record 0), circle at frame 130, `FieldMain`
  starts map 15.

The harness reads guest memory only; nothing writes `D554`/`D7CC` or the pad
words except the retail bodies.

## Results

`run_w34n124_world_walk_entry.sh mountain` summary:

```text
W34N124 PASS slot-1 call 1 recorded
W34N124 PASS guest position moved between call 1 and 121 (29947,-312,11075 -> 30332,-316,10496)
W34N124 PASS target trigger (record id 2) selected at call 121
W34N124 PASS natural world exit with D7CC=0
W34N124 PASS terminal lane entered FieldMain map 15
W34N124 PASS no worldmap stub reached
W34N124 WORLD WALK ENTRY PASS target=mountain
```

Log lines (guest units):

```text
W34N124 call=1   slot=1 pos=(29947,-312,11075) head=0xe00 BD24=-1
W34N124 call=31  slot=1 pos=(30116,-309,10905) head=0x200 BD24=1  D7D8=0x800b671c
W34N124 call=121 slot=1 pos=(30332,-316,10496) head=0x200 BD24=2  D7D8=0x800b670c
[worldmap-open-loop] natural state exit frames=131 D7CC=0
[FieldMain] g_pGameState=0x95acb8 g_GameSceneMapNum=15
```

`run_w34n124_world_walk_entry.sh lahan`:

```text
PROBE call=1   idx=1 pos=(29947,-312,11075) head=0xe00 BD24=-1
PROBE call=31  idx=1 pos=(30116,-309,10905) head=0x200 BD24=1  D7D8=0x800b671c
[world-test-input] frame=41 schedule_frame=40 held=0x0020 rising=0x0020
[worldmap-open-loop] natural state exit frames=41 D7CC=0
[FieldMain] g_pGameState=0x95acb8 g_GameSceneMapNum=1
[field-diag] FieldLoad begin field=1 mapBuf=0x7500ec
```

(The lahan lines above come from the pre-packaging probe run with the identical
schedule, `scratchpad/probe_lahan_entry.log`; the packaged `lahan` harness run
was still executing when this README was written and its `W34N124 ... PASS`
summary is recorded in the follow-up commit if it completed.)

```text
(see above)
```

Both runs print no `[worldmap-stub]` line.

## Scenario seed 24 (archive base 54) list 0

With `XENO_WORLD_SCENARIO_SEED=24` the same route loads second-wave files
56/57/58 and list 0 becomes:

| record | rect (x0, z0, w, h) | map | entrance | id | type |
|---:|---|---:|---:|---:|---:|
| 0 | (25872, 9587, 180, 5436) | 53 | 0 | 8 | 1 |
| 1 | (29430, 11204, 243, 152) | 22 | 0 | 4 | 0 |
| 2 | (28505, 12286, 209, 89) | 24 | 1 | 5 | 0 |
| 3 | (26917, 12008, 183, 87) | 35 | 14 | 6 | 0 |
| 4 | (26661, 12360, 87, 175) | 35 | 15 | 7 | 0 |
| 5 | (30095, 10342, 253, 169) | 15 | 2 | 2 | 0 |

Lahan Village (map 1) is absent and the Mountain Path entrance (map 15) is
retained, which matches the post-destruction world.  Record 1 (map 22) sits
about 500 units west of the arrival point and is the natural candidate for
the Blackmoon Forest entrance; its map identity was not confirmed against the
string table in this rung.

## Bound

The post-transition field load was observed to begin (`FieldLoad begin
field=1` / `field=15`) but the second field's main loop was not followed to
completion inside the harness timeout; the field lane after the world exit is
not certified here.  The return trips (Lahan/Mountain Path exit scripts
writing the arrival tuple with entrance 9 / 2) are field-script behavior and
were not exercised.
