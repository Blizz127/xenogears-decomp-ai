# W34B24-PRE — Read-Only Audit of wm_80095414 (World Path-Node Movement Router)

Mode: READ ONLY. No implementation, no canonical changes.

## Step 1 — Boundary (independently recovered)

| Field | Value |
|---|---|
| target | `0x80095414` |
| start | `0x80095414` |
| end | `0x80095CD4` (exclusive) |
| byte_count | `0x8C0` = 2240 |
| instruction_count | 560 |
| retail_sha256 | `78ebf3efaa0c21a3649acb3f07228d7984fd236c270fe2d9a5c3b140fe04b6ed` |
| file offset | `0x25924` in `disc/world_map.bin` (VA − 0x8006FAF0) |

Adjacent boundaries confirmed: predecessor `wm_80095324` ends at `0x80095414`
(its own `jr $ra` tail); the word at `0x80095CD4` is `0x27BDFFB0`
(`addiu $sp,$sp,-0x50`) — a fresh prologue, so the body ends exactly at
`0x80095CD4`. Capstone decodes all 560 words; **no embedded data inside the
body**. The jump-table data lives OUTSIDE the body at `0x80070C80`
(early overlay table region, immediately after the `wm_80094A5C` dispatch
table which ends with a null word at `0x80070C7C`).

## Step 2 — ABI

```
signature   = s32 wm_80095414(u32 pos_vec, u32 dir_vec, u32 out_vec,
                              s32 scale, s32 mode /* 0x10($sp) */)
  $a0 = pos_vec  : guest ptr, s32 X at +0, s32 Y at +4 (read: height test
                   compares pos[4]>>12), s32 Z at +8. Saved to 0x20($sp).
  $a1 = dir_vec  : guest ptr, s32 X at +0, s32 Z at +8 ($s6).
  $a2 = out_vec  : guest ptr, s32 X/+0, angle-or-height /+4, Z/+8 ($s3).
  $a3 = scale    : s32, 20.12 fixed point (mult + mflo + sra 12). 0x28($sp).
  0x10($sp) (caller frame 0xC8($sp) here) = mode, passed through verbatim
                   as the 5th arg of all three wm_800951A8 sites.
  $v0 = s32: 1  (case-1 proximity hit, or jump-table slot-0 "landed on
                 node" — out[4] receives snapped height<<12)
             0  (redirected along a path link via wm_800952B0, projected
                 via wm_80095324, or tie-break dead-end zero-out)
             r  (wm_800951A8 result {0,1} on the fallback paths)
Stack frame  = 0xB8; saved $s0-$s7, $fp, $ra (10 regs at 0x90..0xB4).
Scratchpad   = heavy, via $fp = 0x1F800000:
               writes 0x60/0x64/0x68 (projected X / angle-0x4000 / Z);
               reads 0x74 (height out of wm_80085158's 0x70 block);
               passes 0x30/0x40/0x50 (redirect vectors), 0x60, 0x70,
               0x80 to callees.
GTE/COP2     = none in the body (no COP2 opcodes); GTE use only inside
               callees (libgte residents).
```

Signedness details: `mult`+`mflo` low-word products; `sra 12` arithmetic;
node/link ids handled as s16 (`sign16` before indexing, `lh` loads);
neighbor type field read as u16 (`lhu +0xC`); cached ids stored as s32.

## Step 3 — Jump table

```
table_address       = 0x80070C80 (outside body; overlay data region)
index_expression    = jr *(0x80070C80 + 4 * result_of_wm_80085760)
index_type          = u32 (sltiu guard on the raw return value)
valid_index_range   = 0..7
default_behavior    = slot 7 -> 0x80095C94 (loop tail: re-dispatch while
                      s4 != 0, return s5 when s4 == 0)
out_of_range        = s1 >= 8 branches to the same loop tail 0x80095C94
                      (never reads the table)
case_count          = 8 retail slots (all recorded individually in
                      JUMP_TABLE.csv; slots 1/2/4 share a shape but have
                      distinct targets — no slots collapsed)
```

## Step 4 — Control flow (see CFG.csv)

Two top-level modes selected by the cached-node global `D_8009C840`:

1. **Entry (always):** project `pos + (dir*scale >> 12)` per axis into
   scratch `0x60/0x68` AND `out[0]/out[8]`; `wm_80093354` wraps each;
   `wm_80093978(x,z) − 0x4000` (heading) into scratch `0x64` and `out[4]`.
2. **Case flag == −1 (no cached node):** `wm_80084D00` fills a candidate
   list; loop 1 filters it via `wm_80085418`; loop 2 probes survivors via
   `wm_80085158` and takes the first candidate whose height is within 11
   units of `pos[4]>>12` — on hit, snap `out[4] = height<<12`, cache
   (attr, node) into `D_8009C840`/`D_8009C16C`, return 1. No candidates →
   plain `wm_800951A8` fallback, cache reset to −1, return its result.
   Candidates but no proximity hit → `wm_80095324` projection, return 0.
3. **Case flag != −1 (cached node):** node-graph walking loop. Fetch the
   84-byte attr record via `*(D_8009C620) + 84*sign16(attr)`, node array =
   `rec[+0x44] + 8` (14-byte node records with s16 links at +6/+8/+0xA and
   u16 type at +0xC). Loop: `wm_80085760` classifies the projected
   position against the current node → jump table:
   - slot 0: landed — `wm_80085158`, snap height, update cache, return 1.
   - slots 1/2/4: follow link +6/+8/+0xA; missing link → `wm_800951A8`
     fallback (cache reset); neighbor type != 1 → keep walking (s0 =
     neighbor); type == 1 → redirect via `wm_800952B0` with scratch vector
     fp+0x30/0x40/0x50 respectively, return 0.
   - slots 3/5/6: two-link resolvers (masks on link presence, tie-breaks
     on neighbor types; dead-end → zero `out`, return 0).
   - slot 7 / out-of-range: re-dispatch.
   Loop exits only when a case clears `s4`; every terminal path assigns
   `s5` first (no live uninitialized return).

Dead code: the mode switch is a compiler switch skeleton — `v1` is
constructed strictly as 1 or 3, so the `v1==0` arm (a fourth `wm_800951A8`
site at `0x800955A8`) and the `return uninit-$s5` arms are retail-dead.

State transitions: globals `D_8009C840` (cached attr id) and `D_8009C16C`
(cached node id) — written on slot-0 land / case-1 hit (cache set) and on
all `wm_800951A8` fallbacks (reset to −1, i.e. next call re-enters case-1).

## Step 5 — Dependencies (see CALL_GRAPH.csv)

16 JAL sites, 9 unique callees. **The earlier W34B23 claim that 95414
references `wm_8008C844`/`wm_8008E76C` was wrong** — those rows in
FOCUS_EDGES are `JAL_CALLER` edges (they call 95414), not callees. The
instruction-level rescan confirms no JAL/JALR to them.

| Callee | Sites | Status |
|---|---|---|
| wm_80093354 | 2 | CANONICAL |
| wm_80093978 | 2 | CANONICAL |
| wm_800951A8 | 3 (1 dead) | CANONICAL (W34B23, acceptance pending) |
| wm_80085760 | 1 | CANONICAL |
| wm_800952B0 | 3 | **MISSING — READY_SMALL** (29 insns, leaf) |
| wm_80095324 | 1 | **MISSING — READY_SMALL** (60 insns; VectorNormal + OuterProduct12, both ported) |
| wm_80085158 | 2 | **MISSING — READY_BOUNDED** (176 insns; libgte + accepted 935DC) |
| wm_80085418 | 1 | **MISSING — READY_BOUNDED** (210 insns; libgte only) |
| wm_80084D00 | 1 | **MISSING — NEEDS_PREREQUISITE** (46 insns; needs wm_80084DB8) |
| └ wm_80084DB8 | (tail) | READY_BOUNDED w/ caveat (232 insns; ScaleMatrix/SetRotMatrix/SetTransMatrix + `func_8004A70C`, which is presently an auto-stub returning 0 — retail behavior must be resolved in that rung; 6 undecoded words = embedded data) |

New-chain consumption: **DIRECT.** 95414 calls `wm_800951A8` at two live
sites (case-1 no-candidate fallback and the dispatch-loop shared fallback),
which transitively pulls the entire accepted `94A5C + 94088 + 93FE4 +
93E8C` chain. `94A5C` is not called directly by 95414.

Missing surface total: 5 functions + 1 tail = **~753 insns**
(29 + 60 + 176 + 210 + 46 + 232).

## Step 6 — Runtime reachability (accepted harness, measured this session)

```
95414_naturally_reached = NO
call_count              = 0
first_call_frame        = n/a
caller                  = none (no native body, no native caller)
arguments_if_observed   = n/a
return_use              = n/a
```

Evidence: the accepted natural Lahan route at the deepest gate chain
(`XENO_WORLD_FRAME_PROLOGUE=1` + full conjunction, DISPLAY=:10) runs the
whole init ladder + scheduler and stops at

```
[worldmap-scheduler] MISSING CALLBACK FRONTIER slot=1 state=1 cb=0x8008a72c
```

(post-W34B23 re-measurement; identical pre/post). The hardened
`w18b_natural.gdb` harness remains `verdict=PASS exit=0`. The **actual
earlier blocker is `wm_8008A72C`** (slot-1 cb1, 740 insns), which contains
2 of the 9 retail call sites into 95414. The other callers (8C844, 8E0F0,
8E76C) are further from the current live path. No forced calls were made.

## Step 7 — Payoff

`wm_80095414` is the world-map **path/node movement router**: it projects
the player's next position, then either resolves free terrain movement
through the certified `951A8 → 94A5C` collision chain, or walks the road/
path node graph (link following, height snapping, movement redirection
along path segments, dead-end blocking) with a persistent (attr, node)
cache. Supported claims:

- world movement (it is the movement-resolution entry consumed by the
  frontier callback)
- terrain/collision (via the 951A8/94A5C chain and the 85xxx probes)
- world player state (out-vector + height snap + the two cache globals)
- dispatch progression (largest missing direct dependency of the measured
  frontier callback wm_8008A72C)

```
expected_runtime_payoff = none immediately on landing 95414 alone; it
unlocks wm_8008A72C's largest dependency. Runtime progress requires
wm_8008A72C itself (plus its remaining small deps 97770/941C4/94238/
8BEC8/8C1DC), at which point the scheduler frontier advances past slot 1.
```

## Step 8 — Implementability

```
classification      = NEEDS_PREREQUISITE
                      (and 95414 is NOT itself the current runtime blocker;
                      the measured blocker is wm_8008A72C, of which 95414
                      is the keystone dependency)

next_prerequisite   = wm_800952B0 + wm_80095324   (READY_SMALL pair)
reason              = leaf/libgte-only, 3+1 call sites inside 95414,
                      zero unknown dependencies, smallest bounded rung
estimated_scope     = 89 insns combined

prerequisite ladder (ranked):
  1. wm_800952B0 (29, leaf) + wm_80095324 (60, libgte)   — READY_SMALL
  2. wm_80085158 (176, libgte + accepted 935DC)           — READY_BOUNDED
  3. wm_80085418 (210, libgte)                            — READY_BOUNDED
  4. wm_80084DB8 (232) then wm_80084D00 (46)              — READY_BOUNDED,
     caveat: resolve func_8004A70C retail behavior (auto-stub today)
  5. wm_80095414 (560)                                    — then READY_LARGE

recommended_next_task = implement the READY_SMALL pair (952B0 + 95324),
                        then walk the ladder above toward 95414; the
                        actual live blocker remains wm_8008A72C
```

## Final report block

```
target=0x80095414
retail_boundary=[0x80095414,0x80095CD4)
instruction_count=560
jump_table_address=0x80070C80
jump_table_cases=8 (slot 7 = loop tail; out-of-range >= 8 branches to the same tail)
default_behavior=re-dispatch loop tail (return s5 when s4==0)
signature=s32 wm_80095414(u32 pos_vec, u32 dir_vec, u32 out_vec, s32 scale, s32 mode_on_stack)
direct_calls=93354 x2, 93978 x2, 951A8 x3 (1 dead), 85760 x1, 84D00 x1, 85158 x2, 85418 x1, 952B0 x3, 95324 x1
missing_dependencies=952B0, 95324, 85158, 85418, 84D00+84DB8 (~753 insns)
naturally_reached=NO
runtime_call_count=0
runtime_caller=none (live-path caller wm_8008A72C has no body; scheduler stops before it)
classification=NEEDS_PREREQUISITE
expected_runtime_payoff=unlocks wm_8008A72C's largest dependency; no direct runtime change until 8A72C lands
next_prerequisite=wm_800952B0 + wm_80095324 (READY_SMALL, 89 insns)
recommended_next_task=implement 952B0+95324, then 85158, 85418, 84DB8/84D00, then 95414
canonical_modified=NO
hard_blockers=none
warnings=(1) W34B23 REPORT.md overstated 95414's deps (8C844/8E76C are callers, not callees);
         (2) func_8004A70C is an auto-stub returning 0 — must be resolved before the 84DB8 rung;
         (3) the dispatch loop has no retail iteration bound — slot-7/out-of-range results with
             no state change would loop forever; the eventual implementation needs the project's
             bounded-dispatch convention;
         (4) dead switch arms (v1==0 path incl. the 0x800955A8 951A8 site, uninit-$s5 returns)
             must be transcribed or documented, not invented around.
```

W34B24-PRE COMPLETE — wm_80095414 DISPATCH AND IMPLEMENTABILITY RESOLVED
