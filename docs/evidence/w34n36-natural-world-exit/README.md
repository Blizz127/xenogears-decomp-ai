# W34N36 — natural world exit through slot 2 and the field terminal lane

Verdict: **NATURAL_D554_EXIT_AND_TERMINAL_LIFECYCLE_REACHED**.

## Anchor and scope

- Branch: `experiment/worldmap-open-gates-20260823`
- Starting HEAD: `e0597ca792220d8ea00fd87523e5fe3c11641e7a`
- Date: 2026-08-30

This rung followed the accepted world session beyond frame 600 without
seeding `0x8009D554` or `0x8009D7CC`.  It added an independent,
world-frame-relative test-input schedule so a detached native run could
exercise retail's world controller accumulator.  It then followed the first
natural `D554: 1 -> 0` through base slot-2 teardown and the signed-D7CC-zero
terminal lane.

The rung also repaired two guest/host UI seams encountered only after the
new world input became live, and corrected the first bounded retail prefix of
`wm_80089C78` after a bad placeholder corrupted a teardown allocation global.

## Detached natural route

The normal binary was launched under GDB only for field bootstrap, then
detached before `PcPort_WorldMapInitMain`.  Relevant controls were:

```text
XENO_FIELD_TEST=1
XENO_KERNEL_SEL=0
XENO_FIELD_MAP=1
XENO_FIELD_ENTRANCE=0
XENO_TEST_INPUT=0:0x2000,600:0x4000,916:0x2000,976:0
XENO_WORLD_TEST_INPUT=0:0x2000,600:0x20,601:0
XENO_WORLD_FRAME_LIMIT=1200
SDL_AUDIODRIVER=dummy
```

`XENO_TEST_INPUT` performs only the pre-world field/bootstrap navigation.
`XENO_WORLD_TEST_INPUT` is a separate zero-based schedule merged after the
retail controller-drain loop.  It ORs held bits into `0x8009CD4C` and rising
edges into `0x8009BD10` and `0x8009BD18`; when unset it is inert.  At displayed
frame 601 the schedule's frame-600 `0x0020` edge naturally drove the active
world callback to clear `D554`:

```text
[world-test-input] frame=601 schedule_frame=600 held=0x0020 rising=0x0020
[w34n36-exit] pre frame=601 ... D554=0 ... CD4C=0x0020 ...
[worldmap-open-loop] natural state exit frames=601 D7CC=0
```

No diagnostic or harness code wrote `D554` or `D7CC`.

## Slot-2 and terminal proof

The diagnostic-only lifecycle trace recorded the live teardown authorities:

```text
slot2 entry: D7CC=0 D7E8=0x801c3a7c D7EC=0x801bea74
             BDF4=0x800f2ca4 BE24=0x800d7538
slot2 exit:  D7CC=0 slot2_complete=1
terminal-zero event order: 1,2,4,5,6,7,8,9,10
```

Event 3 is the conditional type-3 record cleanup and was correctly absent for
this natural record.  After the terminal lane, the trace returned through
`MainLoop`, completed the field-1 teardown, and entered `FieldMain` for map 15.
Thus this is not merely a frame-loop exit: the natural base-session resource
lifecycle and field terminal continuation both executed.

## UI guest-domain repair

The newly reachable slot-12/13 UI callbacks exposed two generated-port
assumptions:

1. They called the no-op `func_80033728` stub instead of compiled
   `GetStringEntry`, and passed a guest KSEG table value as though it were a
   native pointer.
2. They passed a guest OT value to compiled UI render helpers whose internal
   `AddPrim` operations treated links as native storage.

`wm_80092C70` and `wm_80092FD8` now rebase the string table and OT exactly at
the compiled-host boundary and call `GetStringEntry`.  The three primitive
links in `func_80034888` and the link in `func_80031798` use the existing
domain-aware guest/native linker only for `XENO_PC_PORT`; retail builds retain
their original `AddPrim` behavior.

The focused certificate passes O0, O2, and nonrecovering UBSan with strict
warnings and kills:

- M1 raw string-table pointer;
- M2 raw OT pointer;
- M3 skipped compiled string lookup.

The existing guest primitive-link certificate also passes O0/O2/UBSan and
kills M1-M4.

## `wm_80089C78` alias root cause and bounded repair

After the UI seams were repaired, slot-2 initially faulted while freeing
`D7EC`.  A hardware watchpoint found the earlier writer in `wm_80089C78`.
The obsolete placeholder iterated a fabricated fixed record table at
`0x8009B040` and passed `entry+0x28` to `wm_80093534`.  At record 133:

```text
0x8009B040 + 133 * 0x4c + 0x30 = 0x8009D7EC
```

The wrap helper therefore overwrote the allocator global later consumed by
slot 2.

Static retail authority is
`scratchpad/w34_render_scan_k/world_map_full.objdump.txt`, function range
`[0x80089C78,0x8008A2C8)`.  Retail PCs `0x80089D94..0x80089F38` instead:

- load the dynamic 256-record pool from `0x8009BDF4`;
- use stride `0x4c` and state halfword `record+6`;
- skip state zero and process nonzero records;
- rebuild the per-record model matrix, optional Z rotation, and 32-bit scale;
- derive camera-relative position from record `+8/+0xc/+0x10`;
- place that vector at scratch `0x1F800088`;
- call `wm_80093534(0x1F800088)`.

The production helper now implements that prefix through retail PC
`0x80089F38`.  Its focused certificate passes O0/O2/nonrecovering UBSan with
strict warnings and kills five mutants: fixed table, inverted state guard,
record-storage wrap, wrong position fields, and missing camera subtraction.
The natural pool was `0x800F2CA4`; one record (index 1, state `-1`) was active
on the witnessed route.

The later projection and FT4-publication body
`0x80089F40..0x8008A2C8` is deliberately not claimed here and remains the next
bounded transcription frontier.

## Captures and verification

The clean non-diagnostic detached run produced:

| displayed frame | SHA-256 |
|---:|---|
| 60 | `4310197d3881367bc7859ef174dcf4b9afa8759f0b7dc2ca3eea94b0f84f1521` |
| 120 | `8039c2f96b88e09491eaf1cba049d0efd829f146aa943da744f56c93e85b1c16` |
| 600 | `2b636ae21af4bf0003f60d5e53f8ffd9a042fc5a72457e4c096cff19814e1a38` |

Tracked capture paths are intentionally not added; the accepted artifacts
remain under `scratchpad/w34n36_clean_capture/` in this workspace.

Final verification set:

- world UI domain certificate: O0/O2/UBSan PASS, M1-M3 detected;
- `wm_80089C78` prefix certificate: O0/O2/UBSan PASS, M1-M5 detected;
- scripted input certificate: O0/O2/UBSan PASS, both input mutants detected;
- controller-source certificate: O0/O2/UBSan PASS, M1-M3 detected;
- slot-2 teardown certificate: O0/O2/UBSan PASS, M1-M9 detected;
- terminal-zero integration: O0/O2/UBSan PASS, M1-M10 detected;
- guest primitive-link certificate: O0/O2/UBSan PASS, M1-M4 detected;
- normal port build: `LINK OK`;
- diagnostic guards removed and `git diff --check` clean.

The clean process was allowed to continue after the natural world exit and
then stopped externally during the subsequent field run.  The separate
diagnostic lifecycle trace supplies the exact terminal ordering and confirms
entry into map 15; no claim is made that the later field session naturally
terminated.
