# W34B24-PRE4 — Read-Only Audit of wm_8008B644 (Slot-2/3 Companion Callback)

Mode: READ ONLY. No `wm_8008B644` source. G4 accepted first.

Canonical at audit: `88c3b227e7f07494ff381741a5a4edf4097b2d2a`
(`Implement world player callback 0x8008A72C`).

G4 = COMPLETE. G5 = ACTIVE. Live missing callback frontier is this function.

`0x80071A58` is slot-14 cb1 and is **not** on the live path. Do not implement it.

## 1. Boundary (independently verified)

| Field | Value |
|---|---|
| target | `wm_8008B644` (slot-2 cb1; also slot-3 cb1 — same guest address) |
| boundary | `[0x8008B644, 0x8008BB40)` |
| bytes / insns | 1276 / **319** (capstone 319/319, no undecoded words) |
| retail_sha256 | `be1d5c3bc2941db081002efb936eb6f219f3d0aefc69d99f1e56f1c52a609895` |
| file offset | `0x1BB54` in `disc/world_map.bin` (base `0x8006FAF0`) |
| edges | previous function `wm_8008B2BC` ends `jr $ra; nop` at `0x8008B63C`; this body ends `jr $ra; nop`; next word is `wm_8008BB40` prologue |

No overlay `jal 0x8008B644` sites. Two Table-A word refs: `0x80099EA0`, `0x80099EA8` (slot-2/3 cb1 slots).

## 2. ABI

```
signature  = s32 wm_8008B644(s32 slot_idx)   /* scheduler cb1: a0 = slot index */
return     = CONSTANT 1 ($s4 = 1 from the prologue; restored to $v0 on every
             epilogue path, including the +0x24-suppressed 74794 skip)
frame      = 0x38; saves $s0-$s4, $ra
slot rec   = $s1 = *(D_8009BE24) + slot_idx*128
$s2        = preserved slot_idx (object id = slot_idx + 0x2E)
$s3        = 0x1F800000 scratch
$s0        = scratch / neighbor / ring cursor (reused per arm)
COP2       = none
```

Natural measurement (no forced PC/callback/slot/state) after G4: this body is
the next dispatch after slot-1 `wm_8008A72C` returns 1. Observed
`a0 = 2`, slot-2 state 1.

## 3. Slot-record field map (s1-relative)

| Off | Width | Meaning |
|---|---|---|
| +0x04 | s16 | transient substate (1 -> state 8 + neighbor via `lh +6`; 2 -> state 0x28; 3 -> state 1; 5 -> state 0x30) |
| +0x06 | s16 | neighbor slot index (substate 1: `s0 = pool + (lh+6)<<7`) |
| +0x20 | s16 | MAIN STATE (JT1 index 0..0x40) |
| +0x24 | s16 | suppress-registration flag (tail calls 74794 only when 0) |
| +0x28..0x34 | s32x4 | position (20.12) |
| +0x38..0x44 | s32x4 | velocity / approach delta |
| +0x48 | u16 | heading |
| +0x4C | ptr | actor ptr (byte +0xAF; `func_800245D8`) |
| +0x50/+0x54 | s32 | approach target x/z (>>12) |
| +0x58 | s32 | ring-lag / follow index |
| +0x5C | s32 | raw warp heading |

## 4. Globals / tables

- `D_8009BE24` pool base (`0x8009BE24`). Neighbor pose at pool+0x180+(slot<<7) when the per-slot follow flag is set; slot-7 pose at pool+0x3A8/3AC/3B0/3C8.
- Breadcrumb ring: 32 × 0x14 at `0x8009CEC4`, u16 index `0x8009D154`. Follow arm uses `((lhu[0x8009D154] - slot[+0x58]) & 0x1F) * 0x14`.
- Per-slot follow flag: `lbu/sb [0x8006F8E4 + slot_idx]` (states 0/1 and 0x2A).
- Warp/spawn tables (slot-indexed):
  - `lhu [0x8006EF8A + slot*6]` / `lhu [0x8006EF8C + slot*6]` → pos.x/z <<12 (state 0x28)
  - `lhu [0x8006EE58 + slot*2]` → heading (state 0x28)
- Common tail: `wm_80074794(0, slot+0x28)` iff `lh slot[+0x24] == 0`. **No** `sra12` publish to `0x8006EE54/EE56/EE58` (that is the player-callback 8A72C epilogue, not this one).

## 5. Jump table

JT1 @ `0x80070670`, 65 words, index = `lh slot[+0x20]`, `sltiu 0x41` guard.
OOR (`>= 0x41`) and 53 parked slots → common tail `0x8008BAFC`.
Word after the table is 0 (null-terminated in the image).

11 distinct destinations; 10 live arms + park. Full map in `JUMP_TABLES.csv`.

Live arms:

| State | Target | Role |
|---|---|---|
| 0, 1 | `0x8008B718` | follow breadcrumb ring **or** copy neighbor pose (flag `0x8006F8E4+slot`) |
| 2 | `0x8008B880` | copy slot-7 pose (pool+0x3A8..) |
| 8 | `0x8008B8BC` | approach setup vs neighbor (`s0`) + `wm_800941C4` + `245D8(1)` + state++ |
| 9 | `0x8008B904` | approach step `wm_8008BEC8`; ==3 → state++; then `wm_8008C1DC` |
| 0xA | `0x8008B93C` | `wm_80097770(lh+6, 4)`; ok → +0x24=1, state=2, `wm_800894C8(slot+0x2E)` |
| 0x28 | `0x8008B96C` | warp-in from slot-indexed u16 tables + `93978` + `rcos`/`rsin` + `941C4` |
| 0x29, 0x31 | `0x8008BABC` | approach step; ==3 → state++ |
| 0x2A | `0x8008BA48` | clear follow flag; state=0x40 |
| 0x30 | `0x8008BA60` | approach vs pool+0xA8 (player pose) + `941C4` + `245D8(1)` + state++ + `8BEC8` |
| 0x32 | `0x8008BAE4` | `wm_80097770(1, 6)`; ok → state=0 |

No `wm_80095414`, `wm_8008C040`, `wm_80094238`, `wm_80090A84`, or `wm_8007528C`.
This is a companion/follower, not the player movement keystone.

## 6. Substate rewrite (before JT1)

`lh slot[+0x04]`:

- 1 → `sh 0 → +4`; `sh 8 → +0x20`; `s0 = pool + (lh +6)<<7`
- 2 → `sh 0 → +4`; `sh 0x28 → +0x20`
- 3 → `sh 0 → +4`; `sh 1 → +0x20`
- 5 → `sh 0 → +4`; `sh 0x30 → +0x20`
- else → fall through to current `lh +0x20`

## 7. Direct callees (20 JAL / 10 unique)

All ten targets are already strong on canonical. Closure **MISSING=0**.

| Callee | Sites | Status |
|---|---|---|
| `func_800245D8` | 4 | CANONICAL |
| `wm_800894C8` | 3 | CANONICAL |
| `wm_800941C4` | 3 | CANONICAL |
| `wm_8008C1DC` | 2 | CANONICAL |
| `wm_8008BEC8` | 2 | CANONICAL |
| `wm_80097770` | 2 | CANONICAL |
| `wm_80093978` | 1 | CANONICAL |
| `rcos` `0x8003F8B0` | 1 | CANONICAL (PsyCross/SLUS) |
| `rsin` `0x8003F8CC` | 1 | CANONICAL (PsyCross/SLUS) |
| `wm_80074794` | 1 | CANONICAL (epilogue, gated on +0x24==0) |

Delay slots are listed in `CALL_GRAPH.csv`. Every JAL delay is a real argument/store, not a dead nop, except `rsin` (nop) and the `8BEC8`/`97770` hold paths.

## 8. Implementation notes (G5, not started)

- Same scheduler pattern as 8A72C: weak `extern`, `wm_sched_builtin_8008B644` s16 thunk, resolver arm for `0x8008B644u`. Address already in `s_wm_sched_known_missing`.
- Slot 2 **and** slot 3 share this cb1. One body, two Table-A registrations.
- Common tail is **not** the 8A72C sra12-publish + 74794 pair. Do not copy that epilogue.
- `0x80071A58` remains out of scope.

## 9. G4 acceptance snapshot (this audit's parent)

Independent re-run of canonical I8 on `88c3b22` (not the parallel XENO-GP1 body):

- Retail `[0x8008A72C, 0x8008B2BC)` = 2960 B / 740 insns
- SHA `1d58efac94432cb6892462260a1d7679dfcfddc9d0568b08244b1fd7cf1b4b3f`
- Focused O0 / O2 / UBSan-nonrecovering PASS, stdout identical
- Mutants **17/17 KILLED**
- LINK OK ×2: `compiled=47 skipped=0`; stubs 240/524; exactly one strong `T wm_8008A72C` @ `00000000004a703b`; no stub shadow

Controller-natural measurement (no forced PC/callback/slot/state):

```
8A72C_BODY_EXECUTED=YES
8A72C_RETURN=1
SLOT1_STATE_BEFORE=1
SLOT1_STATE_AFTER=1
SCHEDULER_COMPLETED_PASSES_BEFORE=1
SCHEDULER_COMPLETED_PASSES_AFTER=1
NEXT_CALLBACK_TARGET=0x8008B644
NEXT_CALLBACK_SLOT=2
NEXT_CALLBACK_STATE=1
NEXT_CALLBACK_CLASS=MISSING
FRAME_FRONTIER=0x8008B644
CRASH_OR_CUT_PC=0x8008B644
```

Canonical I8 gdb (natural Lahan route) agrees: slot-1 `0x8008a72c` executed ret=1, then `MISSING CALLBACK FRONTIER slot=2 state=1 cb=0x8008b644`. That next-target value was measured, not baked into the oracle.
