# W34B24-I20 — wm_80092FD8 Implementation Report (slot-13 cb1)

Ladder after G5 92C70. Base: canonical
`1833486364570205fffdbf46618cab89c299b3af`. Live frontier was missing
slot-13 cb1 `0x80092FD8` with MISSING=0 (92C70 sibling + inline OT).

## Identity

| Field | Value |
|---|---|
| Function | `wm_80092FD8` (slot-13 cb1 window enqueue/render) |
| Boundary | `[0x80092FD8, 0x800931B0)` = 472 B / 118 insns |
| SHA-256 | `48ea7bdc89adc8380c26447944ffdd4ae81d8e0cbd620c55e6abc743ca06f7a4` |
| File off | `0x234E8` |
| Slot | Table A/B slot 13 cb1 (`0x80099EF4`) |
| ABI | `$a0` = slot index (natural 13); `$v0` = 1 |
| Source | `pc_port/src/world_map_callback_92fd8.{c,h}` |
| Test | `pc_port/tests/w34b24_i20_92fd8_prod_test.c` + `run_w34b24_i20_92fd8.sh` |

Same enqueue machine as `0x80092C70`, but the window is `0x8009BD64`
and the selection word is `0x8009CE68` (the 92DF8 pair). After
`func_80034888`, `+20==1` also AddPrims `0x8009D2B8+ctx*0x28` into
`*(*(0x8009BE3C)+0x70)`. JALs are accepted main-exe helpers only.
Next overlay function `0x800931B0` is a separate destructor wrapper.

## Certification

- O0 / O2 / UBSan-nonrecovering: PASS, stdout identical (clang UBSan).
- Mutants: **17/17 KILLED**.
- Sibling I19 focused oracle: PASS.
- Build: LINK OK ×2; `compiled=47 skipped=0`; exactly one strong
  `T wm_80092FD8` @ `00000000004ae56b`; local thunk
  `t wm_sched_builtin_80092FD8` @ `00000000004a1324`; no stub shadow;
  stubs **240/524 unchanged**.
- Fresh clean-export at `/tmp/w34b24-i20-clean-export`: PASS (17/17),
  `world_map.bin` copied as a regular file (180422).

## RUNTIME PAYOFF — THE CALLBACK FRONTIER MOVED

Natural Lahan route, full deep-gate chain through
`XENO_WORLD_FRAME_PROLOGUE=1` (no forced PC/callback/slot/state).
`DISPLAY=:0`. `w18b_natural.gdb` (150s timeout; scheduler evidence
complete).

```
[worldmap-scheduler] slot=12 state=1 cb=0x80092c70 executed ret=1
[worldmap-scheduler] slot=13 state=1 cb=0x80092fd8 executed ret=1
[worldmap-scheduler] MISSING CALLBACK FRONTIER slot=14 state=1 cb=0x80071a58
counters: dispatch=28 executed=27 missing=1 completed=1 frontier=0x80071a58
```

```
92FD8_BODY_EXECUTED=YES
92FD8_RETURN=1
SLOT13_STATE_BEFORE=1
SLOT13_STATE_AFTER=1
SCHEDULER_COMPLETED_PASSES_BEFORE=1
SCHEDULER_COMPLETED_PASSES_AFTER=1
NEXT_CALLBACK_TARGET=0x80071A58
NEXT_CALLBACK_SLOT=14
NEXT_CALLBACK_STATE=1
NEXT_CALLBACK_CLASS=MISSING
FRAME_FRONTIER=0x80071A58
CRASH_OR_CUT_PC=0x80071490
```

Scheduler executed callbacks: 26 → **27**. NEW CALLBACK FRONTIER:
**`0x80071A58` (slot-14 cb1)**. Natural runtime reached it. Independent
audit: `[0x80071A58, 0x80071B9C)` = 324 B / 81 insns. Eighteen overlay
JALs (`97440`, `97244`, `89748`, `89C78`, `85CDC`, `8615C`, `747DC`,
`848F4`, `980D4`, `981C8`, `96130`, `98CC0`, `983A0`, `9932C`, accepted
`73B04`, `737EC`, `86798`, `740B8`). **MISSING_CALLEES≠0**. Not
implemented here — genuine prerequisite-closure blocker, not a
92C70-family sibling.
