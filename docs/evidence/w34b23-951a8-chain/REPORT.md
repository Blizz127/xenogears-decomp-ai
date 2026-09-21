# W34B23 — 0x800951A8 Movement-Resolver Chain Implementation Report

## Task selection (frontier measurement, 2026-08-16)

The W34B22 ladder closed with `wm_80094A5C` accepted at canonical `7fbce38`.
Per the checkpoint instruction the world runtime frontier was **measured
live**, not inferred:

- Route: accepted W18B natural Lahan route (`XENO_FIELD_TEST=1
  XENO_KERNEL_SEL=0 XENO_FIELD_MAP=1 XENO_FIELD_ENTRANCE=0`, D_800AFE9C input
  schedule 0x2000/0x4000/0), deepest gate chain enabled through
  `XENO_WORLD_FRAME_PROLOGUE=1` (full explicit conjunction:
  ARCHIVE_SET_INDEX, 967E4_ROUTE, READY_BUFFER_CONSUME, MODE_AUDIO_SETUP,
  CONVERGENCE_P1/P2, FRAMEBUFFER_GTE_INIT, TERRAIN_POSITION_INIT,
  COMMON_TAIL_P0..P5, SCHEDULER_97800, FRAME_PROLOGUE), DISPLAY=:10.
- Result: state 3 entered at frame 916, full init ladder runs, scheduler
  entered twice, 18 occupied slots, 17 callbacks executed, and the run stops
  at:

  ```
  [worldmap-scheduler] MISSING CALLBACK FRONTIER slot=1 state=1 cb=0x8008a72c
  ```

**The live frontier is the slot-1 cb1 body `wm_8008A72C` (740 insns), not
`0x800951A8` directly.** Dependency audit of `wm_8008A72C` (callgraph-v2 +
binary JAL scan + linked-symbol classification):

- Missing direct callees: `wm_80097770` (14), `wm_8008BEC8` (67),
  `wm_8008C1DC` (44), `wm_800941C4` (29), `wm_80094238` (75),
  `wm_80095414` (560).
- `wm_80095414` (the largest missing direct dep) calls `wm_800951A8` at three
  sites, and `wm_800951A8` is the **direct consumer of the accepted
  `wm_80094A5C`** plus a small selector chain
  (`wm_80094088` → `wm_80093FE4` → accepted `wm_80093E8C`).
- All other 951A8-chain dependencies are already accepted/ported
  (93E8C, 94A5C, libgte residents).

Chosen bounded rung: the three-function 951A8 consumer chain (125 insns
total), which closes the last unknown between the W34B22 family and
`wm_80095414`.

## Identity

| Function | VA range | Size | File offset | SHA-256 (slice) |
|---|---|---|---|---|
| `wm_80093FE4` | `[0x80093FE4, 0x80094004)` | 0x20 = 8 insns | 0x244F4 | `e85b0a8ae1bac650…4cd53991` |
| `wm_80094088` | `[0x80094088, 0x80094154)` | 0xCC = 51 insns | 0x24598 | `62ca577ffd7b7d8a…93013af7` |
| `wm_800951A8` | `[0x800951A8, 0x800952B0)` | 0x108 = 66 insns | 0x256B8 | `bb71acea957b2d9d…ba4f86dd` |

Overlay `disc/world_map.bin` (SHA-256
`4c15fd32b3a03d7cd5ea4403dcaabc70abaf99b6aaca65d63d6866803edaac70`,
180422 bytes) loaded at `0x8006FAF0`; file offset = VA − base. Boundaries from
`scratchpad/world-callgraph-v2/FUNCTIONS.csv` (93FE4/94088/951A8 rows), slice
SHAs re-verified by the run script on every invocation.

Sources: `pc_port/src/world_map_helper_{93fe4,94088,951a8}.{c,h}`
Test: `pc_port/tests/w34b23_951a8_chain_prod_test.c` + `run_w34b23_951a8_chain.sh`

## Retail contracts

### wm_80093FE4 — attribute nibble wrapper
```
s32 wm_80093FE4(u32 vec_addr)
  $v0 = wm_80093E8C($a0) & 0xF        (andi on the 32-bit register)
Saved: $ra only (0x18 frame).
```

### wm_80094088 — slide-vector selector
```
s32 wm_80094088(u32 attr_vec, u32 src_vec, u32 dst_vec)
  n    = wm_80093FE4($a0)             ; [0,15]
  idx  = (n << 16) >> 12              ; sll/sra = n*16
  tabX = lw 0x8009B264 + idx          ; lui 0x800A base, lw -0x4D9C
  tabZ = lw 0x8009B26C + idx          ; lw -0x4D94
  dot  = lo32(src.X * tabX) + lo32(src.Z * tabZ)   ; mult/mflo, 32-bit wrap
  dot == 0 : dst.X = src.X; RELOAD src.Z; dst.Z = src.Z; return 0
  dot <  0 : dst.X = -tabX (negu wrap); reload tabZ; dst.Z = -tabZ; return 1
  dot >  0 : dst.X = tabX;  reload tabZ; dst.Z = tabZ;  return 1
  dst+4 is NEVER written.
Saved: $s0, $s1, $ra (0x20 frame).
```
Table `0x8009B264` (initialized overlay data): 16 entries × 16 bytes of
fixed-point unit direction vectors (|v| ≈ 0x1000); values extracted from the
retail image and hand-transcribed into the test as the independent reference.

### wm_800951A8 — movement resolver
```
s32 wm_800951A8(u32 base_vec, u32 dir_vec, u32 out_vec, s32 scale, s32 mode)
  mode reaches 0x80094A5C via lh 0x30($sp): sign-extended low halfword only.
  v = wm_80094A5C(base, dir, scale, sign16(mode))
  v == 0: return 1                                    (no stores)
  v == 1: r = wm_80094088(0x1F800000, dir, out)
          r == 0: out zeroed, store order +8 +4 +0
          return 0
  v == 2: out.X = dir.X >= 0 ? 0x1000 : -0x1000      (bgez: 0 → +)
          store order +0 +8 +4; out.Y = out.Z = 0; return 0
  v == 3: store order +4 +0 then out.Z = dir.Z >= 0 ? 0x1000 : -0x1000
          out.X = out.Y = 0; return 0
  other : retail returns the caller's saved $s2 (indeterminate register
          leak). Unreachable: 0x80094A5C's result set is {0,1,2,3} (W34B22-I5
          certificate). The port aborts on this path (fail closed, no
          invented value).
Saved: $s0, $s1, $s2, $ra (0x20 frame).
```

Scratchpad `0x1F800000` flows through the accepted `PSX_ADDR` model
(same convention as the accepted 94A5C/private-collision chain — no private
address hack).

## Certification

- Harness `pc_port/tests/run_w34b23_951a8_chain.sh`:
  - Retail fixture gate: full-file SHA + three per-function slice SHAs.
  - Focused oracle at `-O0`, `-O2`, `-O2 -fsanitize=undefined
    -fno-sanitize-recover=all`; normalized stdout byte-identical across the
    three regimes; empty stderr.
  - Independent oracle inputs:
    - Section A verifies the 16-entry retail table in the loaded image
      against the hand-transcribed constants.
    - Section C expectations hand-derived from the disassembly + retail
      table; reference dot product implemented independently (s64 products
      truncated to low word), including a **low-word wraparound case**
      (`0x40000000 * 0x1000` → low word 0 → retail copy path) that any
      64-bit-dot implementation fails.
    - Sections B/D assert mask semantics, `lh` mode sign-extension, bgez
      zero-inclusion (±0x1000 selection at 0, INT_MIN, INT_MAX), exact store
      sets/order per case, workspace argument, and callee argument traces.
    - Section E runs the real `951A8 → 94088 → 93FE4` chain against the
      retail table in emulated RAM (only 93E8C forced — separately
      certified; 94A5C seam-forced — separately certified).
- Mutants: **14/14 KILLED**, each by a distinct semantic assertion
  (see MUTANTS.csv).
- Build/integration: registered in `PORT_SOURCES`; `./pc_port/build_port.sh`
  clean and incremental both `LINK OK`; three strong `T` symbols; **no stub
  shadow** (functions were previously absent, not stubbed — stub counts
  unchanged at 241/524).
- Noninterference:
  - Frontier measurement re-run post-integration: identical (state3 at
    frame 916, scheduler entry ×2, frontier still `0x8008a72c`, placeholder
    stable, no crash).
  - Hardened `w18b_natural.gdb` harness: `W18I_FORBIDDEN_VERDICT
    verdict=PASS exit=0 zero_verified=13 hit=5 not_instrumented=0
    instrumentation_error=0`.

## Runtime payoff

None yet by design: the only retail caller of `wm_800951A8` is
`wm_80095414` (not yet implemented), and the frontier callback
`wm_8008A72C` still has no body. This rung removes three of the unknowns on
that path. Remaining missing pieces before the frontier callback can run:
`wm_80097770` (14), `wm_800941C4` (29), `wm_8008C1DC` (44), `wm_8008BEC8`
(67), `wm_80094238` (75), `wm_800952B0` (29), `wm_80095324` (60),
`wm_80084D00` (46, +wm_80084DB8), `wm_80085158` (176), `wm_80085418` (210),
`wm_8008E0F0` (40), then `wm_80095414` (560; also references the large cb1
bodies `wm_8008C844`/`wm_8008E76C` — reachability of those call sites on the
natural route needs a jump-table audit before costing them in).
