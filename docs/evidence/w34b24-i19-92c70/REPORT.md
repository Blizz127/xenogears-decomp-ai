# W34B24-I19 — wm_80092C70 Implementation Report (slot-12 cb1)

Ladder after G5 922AC. Base: canonical
`dcc2ceba2b2700239e6e474d0649e376858949b0`. Live frontier was missing
slot-12 cb1 `0x80092C70` with MISSING=0 (main-exe string/event helpers).

## Identity

| Field | Value |
|---|---|
| Function | `wm_80092C70` (slot-12 cb1 text enqueue/render) |
| Boundary | `[0x80092C70, 0x80092DD0)` = 352 B / 88 insns |
| SHA-256 | `264e5f7d7b1d9bf228470253d45c650afd943eabd573fe9b53c336846f5e4815` |
| File off | `0x23180` |
| Slot | Table A/B slot 12 cb1 (`0x80099EEC`) |
| ABI | `$a0` = slot index (natural 12); `$v0` = 1 |
| Source | `pc_port/src/world_map_callback_92c70.{c,h}` |
| Test | `pc_port/tests/w34b24_i19_92c70_prod_test.c` + `run_w34b24_i19_92c70.sh` |

JALs are accepted main-exe helpers: `func_80034614`,
`GetStringEntry@0x80033728`, `func_80034714`, `func_80034888`.
No JALR / COP2 / scratchpad. GPU/OT only inside accepted `34888`.
Internal `+20` 0 waits for a valid `0x8009BD24` selection; 1 clears on
`-1` or re-enqueues when the cached `+50` word differs. Every path
ends in `34888`. Next overlay function `0x80092DD0` is a separate
10-insn destructor wrapper (not a Table A/B callback).

## Certification

- O0 / O2 / UBSan-nonrecovering: PASS, stdout identical (clang UBSan).
- Mutants: **16/16 KILLED**.
- Sibling I17 / I18 focused oracles: PASS.
- Build: LINK OK ×2; `compiled=47 skipped=0`; exactly one strong
  `T wm_80092C70` @ `00000000004ae158`; local thunk
  `t wm_sched_builtin_80092C70` @ `00000000004a12ee`; no stub shadow;
  stubs **240/524 unchanged**.
- Fresh clean-export at `/tmp/w34b24-i19-clean-export`: PASS (16/16),
  `world_map.bin` copied as a regular file (180422).
- `0x80071A58` was not implemented (not on the live path).

## RUNTIME PAYOFF — THE CALLBACK FRONTIER MOVED

Natural Lahan route, full deep-gate chain through
`XENO_WORLD_FRAME_PROLOGUE=1` (no forced PC/callback/slot/state).
`DISPLAY=:0`. `w18b_natural.gdb` (150s timeout; scheduler evidence
complete). First cb1 visit returns 1.

```
[worldmap-scheduler] slot=11 state=1 cb=0x800922ac executed ret=3
[worldmap-scheduler] slot=12 state=1 cb=0x80092c70 executed ret=1
[worldmap-scheduler] MISSING CALLBACK FRONTIER slot=13 state=1 cb=0x80092fd8
counters: dispatch=27 executed=26 missing=1 completed=1 frontier=0x80092fd8
```

```
92C70_BODY_EXECUTED=YES
92C70_RETURN=1
SLOT12_STATE_BEFORE=1
SLOT12_STATE_AFTER=1
SCHEDULER_COMPLETED_PASSES_BEFORE=1
SCHEDULER_COMPLETED_PASSES_AFTER=1
NEXT_CALLBACK_TARGET=0x80092FD8
NEXT_CALLBACK_SLOT=13
NEXT_CALLBACK_STATE=1
NEXT_CALLBACK_CLASS=MISSING
FRAME_FRONTIER=0x80092FD8
CRASH_OR_CUT_PC=0x80071490
```

Scheduler executed callbacks: 25 → **26**. NEW CALLBACK FRONTIER:
**`wm_80092FD8` (slot-13 cb1)**. Overlay identity is independently
audited on the next G5 rung.
